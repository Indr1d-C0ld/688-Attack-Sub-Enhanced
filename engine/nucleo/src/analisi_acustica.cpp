#include "nucleo/analisi_acustica.hpp"

#include "nucleo/interferenza.hpp"
#include "nucleo/schiera.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iterator>
#include <map>
#include <optional>
#include <tuple>

namespace m688 {

namespace {

constexpr double C_RIFERIMENTO = 1500.0;
constexpr double POSATO_DB = -10.0;          // come in acustica.cpp: macchinari ridotti
// la schiera danneggiata (dossier 04 §6.3) e le righe del proprio battello (blocco 2 del sonar, F; dossier 28 §5, STIMA)
constexpr double SCHIERA_DANNEGGIATA_DB = 6.0, SCHIERA_DANNEGGIATA_HZ = 300.0;
constexpr double PROPRIO_BAFFLES_DB = 6.0, PROPRIO_SCHIERA_FUOCO_DEG = 30.0, PROPRIO_SCHIERA_DIRETTIVITA_DB = 10.0;
constexpr double CIECO_DB = 30.0;
constexpr double GRANA_DB = 1.5;
constexpr double DEMON_BANDA_HZ = 3500.0;     // 2-8 kHz
constexpr double DEMON_LARGHEZZA_HZ = 6000.0;
constexpr double DEMON_PROFONDITA_DB = 10.0;  // righe d'inviluppo sopra il S/N in alta frequenza (STIMA)
constexpr double TOLLERANZA_DOPPLER = 0.006;  // 0,6%: 17 kn di velocita' radiale
constexpr double PUNTI_ESTRANEA = 0.4;        // penalita' per una riga osservata che la classe non spiega
constexpr double TEMPERATURA = 0.35;          // softmax delle ipotesi
constexpr double COMPATTEZZA_MIN = 0.5;       // famiglie armoniche: membri / armonica piu' alta (STIMA)

double potenza(double db) { return std::pow(10.0, db / 10.0); }
double decibel(double p) { return 10.0 * std::log10(std::max(p, 1e-30)); }

bool cava(const Sorgente& s) {
    if (s.cavitazione) return *s.cavitazione;
    if (!s.firma) return false;
    // le navi civili generate hanno l'innesco vero (nucleo/navi_civili); le altre navi lo hanno nella tabella (0)
    if (s.firma->nave && s.firma->v_cavitazione_kn > 0.0) return s.velocita_kn > s.firma->v_cavitazione_kn;
    return cavita(*s.firma, s.velocita_kn, s.quota_m);
}

// velocita' di avvicinamento (m/s, positiva se la distanza cala) fra sorgente e ricevitore
double avvicinamento(const Sorgente& s, const Ricevitore& r) {
    const double d = distanza(r.pos, s.pos);
    if (d < 1.0) return 0.0;
    const Vec2 u{(s.pos.x - r.pos.x) / d, (s.pos.y - r.pos.y) / d};   // dal ricevitore verso la sorgente
    const double rs = radianti(s.rotta), rr = radianti(r.rotta);
    const double vs = s.velocita_kn * NODO * (std::sin(rs) * u.x + std::cos(rs) * u.y);
    const double vr = r.velocita_kn * NODO * (std::sin(rr) * u.x + std::cos(rr) * u.y);
    return vr - vs;
}

// stato di un fascio puntato a un rilevamento vero
struct Fascio {
    double relativo = 0.0;
    bool baffles = false, cieco = false;
};
Fascio fascio(const Ricevitore& r, double rilevamento_vero) {
    const Sensore& sn = *r.sensore;
    Fascio f;
    f.relativo = normalizza_180(rilevamento_vero - r.rotta);
    f.baffles = sn.baffles_gradi > 0.0 && std::fabs(f.relativo) > 180.0 - sn.baffles_gradi;
    f.cieco = f.baffles || (sn.endfire_gradi > 0.0 && std::fabs(f.relativo) < sn.endfire_gradi) ||
              r.rateo_virata_deg_s > sn.virata_max_deg_s;
    return f;
}

// rumore nel fascio, spettrale (per Hz), meno il guadagno d'antenna
double rumore_fascio(double f, const Ricevitore& r, const Ambiente& amb, const Fascio& fa) {
    const Sensore& sn = *r.sensore;
    const double v = r.sul_fondo ? 0.0 : r.velocita_kn;
    const double nl = 10.0 * std::log10(potenza(rumore_ambiente(f, amb)) + potenza(rumore_proprio_ricevitore(f, r, v, fa.baffles)));
    return nl - (fa.baffles ? 0.0 : indice_direttivita(sn, f)) + (r.schiera_danneggiata ? SCHIERA_DANNEGGIATA_DB : 0.0);
}

// Le righe del proprio battello che arrivano al fascio (blocco 2 del sonar, F): (hz, potenza ricevuta in uPa^2, tipo).
// Sfera e sensori di scafo: per via strutturale, in tutti i fasci, con l'isolamento del sensore (6 dB in meno nei
// baffles). Schiere lineari: per l'acqua, solo nei fasci entro 30 gradi dal fuoco anteriore, con la divergenza sul cavo
// e la direttivita' (STIMA, dossier 28 §5)
std::vector<std::tuple<double, double, TipoRiga>> righe_del_proprio(const Ricevitore& r, const Fascio& fa) {
    std::vector<std::tuple<double, double, TipoRiga>> out;
    if (!r.proprio || !r.sensore) return out;
    const Sensore& sn = *r.sensore;
    double attenuazione = 0.0;
    if (sn.antenna == Antenna::lineare) {
        if (std::fabs(fa.relativo) > PROPRIO_SCHIERA_FUOCO_DEG) return out;
        const double d = std::max(r.cavo_filato_m, 100.0) + 0.5 * sn.dimensione_m;
        attenuazione = 20.0 * std::log10(d) + PROPRIO_SCHIERA_DIRETTIVITA_DB;
    } else {
        if (sn.isolamento_proprio_db < 0.0) return out;
        attenuazione = sn.isolamento_proprio_db - (fa.baffles ? PROPRIO_BAFFLES_DB : 0.0);
    }
    for (const RigaEmessa& re : righe_emesse(*r.proprio))
        if (!r.schiera_danneggiata || re.hz <= SCHIERA_DANNEGGIATA_HZ) out.emplace_back(re.hz, potenza(re.db - attenuazione), re.tipo);
    return out;
}

// risposta del fascio (dB) verso una sorgente: lobo gaussiano largo quanto il fascio, poi i lobi laterali; la schiera sente
// anche lo specchio rispetto al suo asse
double risposta(const Ricevitore& r, const Fascio& fa, double rilevamento_sorgente, double f, double ampiezza_deg = 0.0) {
    const Sensore& sn = *r.sensore;
    const double rel_s = normalizza_180(rilevamento_sorgente - r.rotta);
    double d = std::fabs(normalizza_180(fa.relativo - rel_s));
    if (sn.conico) d = std::min(d, std::fabs(normalizza_180(fa.relativo + rel_s)));
    // una sorgente diffusa allarga il lobo quanto il settore che occupa
    const double theta = std::hypot(larghezza_fascio_deg(sn, f, fa.relativo), ampiezza_deg);
    // lobi laterali: 15 dB sotto l'isotropo per un'antenna direttiva, quasi nulli senza direttivita' (STIMA)
    const double di = indice_direttivita(sn, f);
    return std::max(-12.0 * std::pow(d / theta, 2.0), -std::min(di + 15.0, 2.0 * di + 3.0));
}

// settore occupato da una sorgente diffusa vista a distanza d (gradi)
double ampiezza_sorgente(const Sorgente& s, double d) {
    if (s.estensione_m <= 0.0) return 0.0;
    return gradi(2.0 * std::atan(s.estensione_m / std::max(d, 1.0)));
}

// perdita di trasmissione interpolata in log f fra pochi nodi (lo spettro ha migliaia di bin)
// `diretti`: per le frange (nucleo/interferenza) il peso dei cammini diretti, 1 dove il livello A ha come migliore il
// diretto, il fondo o l'acqua bassa, 0 nei rifratti (condotto, zone di convergenza)
struct PerditaInterpolata {
    std::vector<double> lf, tl, diretti;
    PerditaInterpolata(double f_min, double f_max, double r, double zs, double zr, const Ambiente& amb) {
        constexpr int NODI = 7;
        const double a = std::log(std::max(f_min, 1.0)), b = std::log(std::max(f_max, f_min * 1.01 + 1.0));
        for (int k = 0; k < NODI; ++k) {
            const double x = a + (b - a) * k / (NODI - 1);
            lf.push_back(x);
            const Perdita p = perdita_trasmissione(std::exp(x), r, zs, zr, amb);
            tl.push_back(p.tl_db);
            diretti.push_back(p.percorso == Percorso::diretto || p.percorso == Percorso::fondo || p.percorso == Percorso::acqua_bassa ? 1.0 : 0.0);
        }
    }
    double operator()(double f) const { return interpola(tl, f); }
    double peso_diretti(double f) const { return interpola(diretti, f); }
    double interpola(const std::vector<double>& v, double f) const {
        const double x = std::log(std::max(f, 1.0));
        if (x <= lf.front()) return v.front();
        if (x >= lf.back()) return v.back();
        const auto it = std::upper_bound(lf.begin(), lf.end(), x);
        const std::size_t i = static_cast<std::size_t>(it - lf.begin());
        const double t = (x - lf[i - 1]) / (lf[i] - lf[i - 1]);
        return v[i - 1] + t * (v[i] - v[i - 1]);
    }
};

}  // namespace

// --- righe emesse ---

double frequenza_asse_hz(const Sorgente& s) {
    if (!s.firma || s.firma->pale <= 0 || (s.firma->giri_per_nodo <= 0.0 && s.firma->giri_costanti <= 0.0) || s.velocita_kn < 0.5) return 0.0;
    const double f = (s.firma->giri_costanti > 0.0 ? s.firma->giri_costanti : s.firma->giri_per_nodo * s.velocita_kn) / 60.0;
    return s.var.attive ? f * s.var.fattore_giri : f;
}

namespace {
// R6: un numero in [0, 1) dal seme dell'unita' e da due indici (riga, intervallo di tempo)
double u01_unita(std::uint64_t seme, std::uint64_t a, std::uint64_t b) {
    std::uint64_t z = seme ^ (0x9e3779b97f4a7c15ull * (a + 1)) ^ (0xbf58476d1ce4e5b9ull * (b + 1));
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ull;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebull;
    z ^= z >> 31;
    return static_cast<double>(z >> 11) * 0x1.0p-53;
}
constexpr double AUSILIARI_ACCESI = 0.7, AUSILIARI_TURNO_S = 1200.0;   // STIMA: accesi il 70% del tempo, turni di 20 min
// righe del regime e dell'individuo: le righe legate ai giri, quelle sulla rete, quelle fisse
bool riga_dei_giri(TipoRiga t) { return t == TipoRiga::turbina || t == TipoRiga::riduttore; }
bool riga_sulla_rete(TipoRiga t) { return t == TipoRiga::pompa || t == TipoRiga::ausiliario || t == TipoRiga::diesel; }
}  // namespace

std::vector<RigaEmessa> righe_emesse(const Sorgente& s) {
    if (s.muta) return {};   // solo eco: niente righe
    std::vector<RigaEmessa> out;
    if (!s.firma) {   // sorgente della libreria: tonali e famiglie della finestra
        TipoRiga tipo = TipoRiga::biologico;
        for (const auto& [hz, db] : righe_fonte(s, &tipo)) out.push_back({hz, db, tipo});
        return out;
    }
    const FirmaRumore& f = *s.firma;
    const double v = s.velocita_kn;
    const VariazioniSorgente& var = s.var;
    // R6: la pompa lenta di riferimento per la legge di Ross (potenza da squilibrio ~ n^4: +40 log r a giri r volte piu' alti)
    const RigaFirma* lenta = nullptr;
    for (const RigaFirma& r : f.righe)
        if (r.tipo == TipoRiga::pompa && r.v_max_kn < 1e8 && (!lenta || r.hz < lenta->hz)) lenta = &r;
    for (std::size_t i = 0; i < f.righe.size(); ++i) {
        const RigaFirma& r = f.righe[i];
        if (r.tipo == TipoRiga::diesel && f.snorkel_db > 0.0 && !s.snorkel) continue;   // SSK: i diesel solo a snorkel
        if (!var.attive || r.tipo != TipoRiga::pompa || var.regime == RegimePompe::automatico) {
            if (v < r.v_min_kn || v >= r.v_max_kn) continue;
        } else {   // regime forzato: le lente sono quelle con un massimo, le veloci quelle con un minimo
            const bool e_lenta = r.v_max_kn < 1e8, e_veloce = r.v_min_kn > 0.0;
            if (var.regime == RegimePompe::spente) continue;
            if (var.regime == RegimePompe::lente && e_veloce) continue;
            if (var.regime == RegimePompe::veloci && e_lenta) continue;
        }
        double hz = r.hz, db = f.tonale_db + r.db_relativo + (s.sul_fondo ? POSATO_DB : 0.0);
        if (r.multiplo_asse > 0.0) {   // legata ai giri (gia' con l'individuo): ferma con l'asse
            hz = r.multiplo_asse * frequenza_asse_hz(s);
            if (hz <= 0.0) continue;
        }
        if (var.attive && r.tipo != TipoRiga::rete) {
            if (r.tipo == TipoRiga::ausiliario &&   // gli ausiliari si accendono e si spengono a turni
                u01_unita(var.seme, i, static_cast<std::uint64_t>(std::max(var.tempo_s, 0.0) / AUSILIARI_TURNO_S)) >= AUSILIARI_ACCESI)
                continue;
            const bool fissa = r.multiplo_asse <= 0.0;   // le legate all'asse hanno gia' il fattore dei giri
            if (fissa && riga_dei_giri(r.tipo)) hz *= var.fattore_giri;
            else if (fissa && riga_sulla_rete(r.tipo)) hz *= var.fattore_pompe;
            if (r.tipo == TipoRiga::pompa && lenta && &r != lenta && r.v_min_kn > 0.0)
                db = f.tonale_db + lenta->db_relativo + 40.0 * std::log10(r.hz / lenta->hz) + (s.sul_fondo ? POSATO_DB : 0.0);
            db += 2.5 * (2.0 * u01_unita(var.seme, i, 1u << 20) - 1.0);   // il ritratto dello scafo: +-2,5 dB (STIMA)
            if (var.danno_macchine > 0.0) db += 6.0 + 14.0 * var.danno_macchine;   // Shin91: +6-8 significativo, +20 guasto
        }
        out.push_back({hz, db, r.tipo});
    }
    if (const double fs = frequenza_asse_hz(s); fs > 0.0) {
        const double pala = livello_pala_db(s);   // nucleo/acustica.hpp: la stessa della scoperta
        const double fp = f.pale * fs;
        out.push_back({fs, pala - 6.0, TipoRiga::elica});
        out.push_back({fp, pala, TipoRiga::elica});
        out.push_back({2.0 * fp, pala - 6.0, TipoRiga::elica});
        if (var.attive && var.danno_elica > 0.0) {   // pale non identiche: bande laterali a +-1 volta i giri (Ross 8.6)
            out.push_back({fp - fs, pala - 12.0 + 8.0 * var.danno_elica, TipoRiga::elica});
            out.push_back({fp + fs, pala - 12.0 + 8.0 * var.danno_elica, TipoRiga::elica});
        }
        if (var.attive && var.cantante_hz > 0.0 && v >= var.cantante_da_kn && v < var.cantante_a_kn)
            out.push_back({var.cantante_hz, pala - 3.0, TipoRiga::elica});   // l'elica che canta (STIMA sul livello)
    }
    // il falso contatto (D3): mancano le righe che un'impronta della sorgente e della frequenza mette sopra la fedelta',
    // le altre perdono (1 - fedelta') x 6 dB; sotto FEDELTA_SENZA_PALE niente righe d'elica (STIMA)
    if (s.fedelta < 1.0) {
        std::erase_if(out, [&](const RigaEmessa& re) {
            if (re.tipo == TipoRiga::elica && s.fedelta < FEDELTA_SENZA_PALE) return true;
            std::uint64_t z = static_cast<std::uint64_t>(s.id) * 0x9E3779B97F4A7C15ull ^
                              static_cast<std::uint64_t>(std::llround(re.hz * 100.0)) * 0xC2B2AE3D27D4EB4Full;
            z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ull;
            z ^= z >> 31;
            return static_cast<double>(z >> 11) * 0x1.0p-53 >= s.fedelta;
        });
        for (auto& re : out) re.db -= (1.0 - s.fedelta) * 6.0;
    }
    return out;
}

// --- spettro LOFAR ---

namespace {

// risposta in potenza di una finestra di Hann a x bin dal centro (1 al centro, 0,25 a un bin, poi i lobi laterali)
double hann(double x) {
    x = std::fabs(x);
    if (x < 1e-9) return 1.0;
    if (std::fabs(1.0 - x * x) < 1e-6) return 0.25;
    const double a = std::sin(PI * x) / (PI * x) / (1.0 - x * x);
    return a * a;
}

// Le frange di una sorgente (nucleo/interferenza): la distanza cambia di velocita' radiale x 1 / bin nel tempo
// d'integrazione. Le navi stanno a quota zero nella missione, ma il rumore viene dall'elica: per le frange la sorgente
// e' alla sua quota (STIMA: 0,6-0,7 del pescaggio di un mercantile di 7-9 m)
constexpr double ELICA_NAVE_M = 5.0;
constexpr double FRANGE_SOTTO_DB = 15.0;   // le frange salgono al piu' di 10 dB (Interferenza::max_db), con margine
// la vibrazione del cavo della schiera filata (blocco 1 del sonar, D; STIMA): la fondamentale 6 dB sopra la densita'
// del rumore del fascio (in 1 Hz), la seconda armonica 6 dB sotto; sparse dell'1% (la corrente cambia lungo il cavo)
constexpr double VIBRAZIONE_DB = 6.0, VIBRAZIONE_ARMONICA_DB = -6.0, VIBRAZIONE_LARGHEZZA = 0.01;
Interferenza frange_di(const Sorgente& s, const Ricevitore& r, const Ambiente& amb, double d, double bin) {
    const double zs = s.nave ? std::max(s.quota_m, ELICA_NAVE_M) : s.quota_m;
    return Interferenza(d, zs, r.quota_m, amb, -avvicinamento(s, r) / bin);
}

// Lo spettro del fascio: con `finestra` le righe si spargono sui bin vicini con la risposta di Hann (il verniero, che
// stima il picco fra i bin); senza, ogni riga sta nel suo bin. `ricevute`: le righe che cadono nella banda, col S/N.
// `coerente`: le frange d'interferenza dei cammini diretti sul continuo e sulle righe
Spettro spettro_interno(const std::vector<Sorgente>& sorgenti, const Ricevitore& r, const Ambiente& amb,
                        double rilevamento_fascio, double f_min, double f_max, double bin, bool finestra,
                        std::vector<RigaRicevuta>* ricevute, bool con_righe = true, bool coerente = false) {
    Spettro out;
    out.f0_hz = f_min;
    out.df_hz = bin;
    if (!r.sensore || f_max <= f_min || bin <= 0.0) return out;
    const std::size_t n = static_cast<std::size_t>(std::ceil((f_max - f_min) / bin));
    out.snr_db.assign(n, 0.0f);
    const Fascio fa = fascio(r, rilevamento_fascio);
    // continuo e rumore cambiano lentamente con la frequenza: si calcolano ogni ~1 Hz e si interpolano in dB
    const std::size_t passo = std::max<std::size_t>(1, static_cast<std::size_t>(std::lround(1.0 / bin)));
    const auto interpolato = [&](const auto& valore) {
        std::vector<double> v(n);
        for (std::size_t i = 0; i < n; i += passo) v[i] = valore(out.frequenza(i));
        const std::size_t ultimo = (n - 1) / passo * passo;
        if (ultimo != n - 1) v[n - 1] = valore(out.frequenza(n - 1));
        for (std::size_t i = 0; i < n; ++i) {
            if (i % passo == 0 || i == n - 1) continue;
            const std::size_t a = i / passo * passo, b = std::min(a + passo, n - 1);
            const double t = static_cast<double>(i - a) / static_cast<double>(b - a);
            v[i] = v[a] + t * (v[b] - v[a]);
        }
        return v;
    };
    if (ricevute && !con_righe && !finestra) {
        // solo le righe ricevute (righe_ricevute): il continuo non serve, e il rumore solo nei bin delle righe, con la
        // stessa interpolazione dello spettro intero (stessi valori, bit per bit)
        std::map<std::size_t, double> nodi;
        const auto nodo = [&](std::size_t i) {
            const auto it = nodi.find(i);
            if (it != nodi.end()) return it->second;
            const double v = rumore_fascio(out.frequenza(i), r, amb, fa) + 10.0 * std::log10(bin);
            nodi.emplace(i, v);
            return v;
        };
        const std::size_t ultimo = (n - 1) / passo * passo;
        const auto rumore_in = [&](std::size_t i) {
            if (i % passo == 0 || (i == n - 1 && ultimo != n - 1)) return potenza(nodo(i));
            const std::size_t a = i / passo * passo, b = std::min(a + passo, n - 1);
            const double t = static_cast<double>(i - a) / static_cast<double>(b - a);
            const double va = nodo(a), vb = nodo(b);
            return potenza(va + t * (vb - va));
        };
        const double cieco = fa.cieco && !fa.baffles ? CIECO_DB : 0.0;
        for (const Sorgente& s : sorgenti) {
            if (!s.firma && !s.fonte) continue;
            const double d = distanza(r.pos, s.pos);
            const double brg = d > 0.0 ? rilevamento(r.pos, s.pos) : rilevamento_fascio;
            const double ampiezza = ampiezza_sorgente(s, d);
            const PerditaInterpolata tl(f_min, f_max, d, s.quota_m, r.quota_m, amb);
            const double doppler = 1.0 + avvicinamento(s, r) / C_RIFERIMENTO;
            const std::optional<Interferenza> frange = coerente ? std::optional<Interferenza>(frange_di(s, r, amb, d, bin)) : std::nullopt;
            for (const RigaEmessa& re : righe_emesse(s)) {
                const double f = re.hz * doppler;
                if (f < f_min || f >= f_min + static_cast<double>(n) * bin) continue;
                const std::size_t i = static_cast<std::size_t>(std::floor((f - f_min) / bin + 1e-9));
                const double asp = s.var.attive ? aspetto_db(s, r.pos, re.hz) : 0.0;
                const double fr = frange ? tl.peso_diretti(f) * frange->db(f) : 0.0;
                const double p = potenza(re.db + asp - tl(f) + fr + risposta(r, fa, brg, f, ampiezza) - cieco);
                ricevute->push_back({f, decibel(p / rumore_in(i)), s.id, re.tipo, re.hz});
            }
        }
        for (const auto& [hz, p, tipo] : righe_del_proprio(r, fa)) {   // le righe del proprio battello (sorgente -3)
            if (hz < f_min || hz >= f_min + static_cast<double>(n) * bin) continue;
            const std::size_t i = static_cast<std::size_t>(std::floor((hz - f_min) / bin + 1e-9));
            ricevute->push_back({hz, decibel(p / rumore_in(i)), -3, tipo, hz});
        }
        if (r.cavo_filato_m > 0.0) {   // la vibrazione del cavo: righe proprie (sorgente -2)
            const auto fv = frequenze_vibrazione(r.sensore->cavo_diametro_m, r.velocita_kn);
            for (std::size_t h = 0; h < fv.size(); ++h) {
                if (fv[h] < f_min || fv[h] >= f_min + static_cast<double>(n) * bin) continue;
                const double db = VIBRAZIONE_DB + (h > 0 ? VIBRAZIONE_ARMONICA_DB : 0.0);
                ricevute->push_back({fv[h], db - 10.0 * std::log10(bin), -2, TipoRiga::ausiliario, fv[h]});
            }
        }
        return out;
    }
    std::vector<double> rumore = interpolato([&](double f) { return rumore_fascio(f, r, amb, fa) + 10.0 * std::log10(bin); });
    for (double& x : rumore) x = potenza(x);
    std::vector<double> segnale(n, 0.0);
    const double cieco = fa.cieco && !fa.baffles ? CIECO_DB : 0.0;   // nei baffles il rumore e' gia' alto
    for (const Sorgente& s : sorgenti) {
        if (!s.firma && !s.fonte) continue;
        const double d = distanza(r.pos, s.pos);
        const double brg = d > 0.0 ? rilevamento(r.pos, s.pos) : rilevamento_fascio;
        const double ampiezza = ampiezza_sorgente(s, d);
        const PerditaInterpolata tl(f_min, f_max, d, s.quota_m, r.quota_m, amb);
        const double doppler = 1.0 + avvicinamento(s, r) / C_RIFERIMENTO;
        // i pezzi che non dipendono dalla frequenza, una volta per sorgente (livelli delle componenti, banda larga)
        const std::vector<double> livelli = s.fonte ? livelli_componenti(s) : std::vector<double>{};
        const double banda_larga = s.firma ? livello_banda_larga(s) : 0.0;
        const std::vector<double> continuo = interpolato([&](double f) {
            double sl = s.fonte ? livello_fonte_da(s, f, false, livelli) : livello_sorgente_da(f, s, banda_larga);   // le righe nei loro bin
            if (s.var.attive) sl += aspetto_db(s, r.pos, f);
            return sl - tl(f) + 10.0 * std::log10(bin) + risposta(r, fa, brg, f, ampiezza) - cieco;
        });
        const std::optional<Interferenza> frange = coerente ? std::optional<Interferenza>(frange_di(s, r, amb, d, bin)) : std::nullopt;
        // le frange del continuo solo dove si vedono: una sorgente sotto il rumore di FRANGE_SOTTO_DB in tutta la banda
        // non cambia lo spettro (e il costo e' tutto qui, 2-3 ms per 4000 bin)
        bool visibile = false;
        for (std::size_t i = 0; frange && !visibile && i < n; i += passo) visibile = continuo[i] - decibel(rumore[i]) > -FRANGE_SOTTO_DB;
        if (frange && visibile) {   // bin per bin: cambiano troppo in fretta per l'interpolazione del continuo
            const std::vector<float> fr = frange->db_griglia(out.f0_hz, bin, n);
            for (std::size_t i = 0; i < n; ++i) segnale[i] += potenza(continuo[i] + tl.peso_diretti(out.frequenza(i)) * fr[i]);
        } else {
            for (std::size_t i = 0; i < n; ++i) segnale[i] += potenza(continuo[i]);
        }
        for (const RigaEmessa& re : righe_emesse(s)) {
            const double f = re.hz * doppler;
            if (f < f_min || f >= f_min + static_cast<double>(n) * bin) continue;
            const std::size_t i = static_cast<std::size_t>(std::floor((f - f_min) / bin + 1e-9));
            const double asp = s.var.attive ? aspetto_db(s, r.pos, re.hz) : 0.0;
            const double fr = frange ? tl.peso_diretti(f) * frange->db(f) : 0.0;
            const double p = potenza(re.db + asp - tl(f) + fr + risposta(r, fa, brg, f, ampiezza) - cieco);
            if (ricevute) ricevute->push_back({f, decibel(p / rumore[i]), s.id, re.tipo, re.hz});
            if (!con_righe) continue;
            if (!finestra) {
                segnale[i] += p;
                continue;
            }
            const double x = (f - f_min) / bin - 0.5;   // posizione in bin rispetto ai centri
            for (long j = static_cast<long>(i) - 3; j <= static_cast<long>(i) + 3; ++j)
                if (j >= 0 && j < static_cast<long>(n)) segnale[static_cast<std::size_t>(j)] += p * hann(x - static_cast<double>(j));
        }
    }
    if (r.cavo_filato_m > 0.0) {   // la vibrazione del cavo: righe proprie, uguali in tutti i fasci, un po' sparse
        const auto fv = frequenze_vibrazione(r.sensore->cavo_diametro_m, r.velocita_kn);
        for (std::size_t h = 0; h < fv.size(); ++h) {
            if (fv[h] < f_min || fv[h] >= f_min + static_cast<double>(n) * bin) continue;
            const std::size_t c = static_cast<std::size_t>(std::floor((fv[h] - f_min) / bin + 1e-9));
            const double potenza_riga = rumore[c] / bin * potenza(VIBRAZIONE_DB + (h > 0 ? VIBRAZIONE_ARMONICA_DB : 0.0));
            const double sigma = VIBRAZIONE_LARGHEZZA * fv[h] / bin;   // in bin
            const long larga = static_cast<long>(std::ceil(3.0 * sigma));
            double somma = 0.0;
            std::vector<std::pair<std::size_t, double>> pesi;
            for (long j = static_cast<long>(c) - larga; j <= static_cast<long>(c) + larga; ++j) {
                if (j < 0 || j >= static_cast<long>(n)) continue;
                const double x = (out.frequenza(static_cast<std::size_t>(j)) - fv[h]) / bin;
                const double w = sigma > 0.5 ? std::exp(-0.5 * x * x / (sigma * sigma)) : (static_cast<std::size_t>(j) == c ? 1.0 : 0.0);
                pesi.emplace_back(static_cast<std::size_t>(j), w);
                somma += w;
            }
            for (const auto& [j, w] : pesi) segnale[j] += potenza_riga * w / std::max(somma, 1e-12);
            if (ricevute) ricevute->push_back({fv[h], decibel(potenza_riga / rumore[c]), -2, TipoRiga::ausiliario, fv[h]});
        }
    }
    for (const auto& [hz, p, tipo] : righe_del_proprio(r, fa)) {   // le righe del proprio battello, in ogni fascio
        if (hz < f_min || hz >= f_min + static_cast<double>(n) * bin) continue;
        const std::size_t i = static_cast<std::size_t>(std::floor((hz - f_min) / bin + 1e-9));
        if (ricevute) ricevute->push_back({hz, decibel(p / rumore[i]), -3, tipo, hz});
        if (!con_righe) continue;
        if (!finestra) {
            segnale[i] += p;
            continue;
        }
        const double x = (hz - f_min) / bin - 0.5;
        for (long j = static_cast<long>(i) - 3; j <= static_cast<long>(i) + 3; ++j)
            if (j >= 0 && j < static_cast<long>(n)) segnale[static_cast<std::size_t>(j)] += p * hann(x - static_cast<double>(j));
    }
    if (r.schiera_danneggiata)   // la schiera danneggiata non ha l'alta: solo rumore sopra 300 Hz
        for (std::size_t i = 0; i < n; ++i)
            if (out.frequenza(i) > SCHIERA_DANNEGGIATA_HZ) segnale[i] = 0.0;
    for (std::size_t i = 0; i < n; ++i) out.snr_db[i] = static_cast<float>(decibel((segnale[i] + rumore[i]) / rumore[i]));
    return out;
}

}  // namespace

Spettro spettro_fascio(const std::vector<Sorgente>& sorgenti, const Ricevitore& r, const Ambiente& amb,
                       double rilevamento_fascio, double f_min, double f_max, double bin, bool coerente) {
    return spettro_interno(sorgenti, r, amb, rilevamento_fascio, f_min, f_max, bin, false, nullptr, true, coerente);
}

Spettro spettro_verniero(const std::vector<Sorgente>& sorgenti, const Ricevitore& r, const Ambiente& amb,
                         double rilevamento_fascio, double centro_hz, double larghezza_hz, double bin_hz, bool coerente) {
    const double f0 = std::max(0.1, centro_hz - 0.5 * larghezza_hz);
    return spettro_interno(sorgenti, r, amb, rilevamento_fascio, f0, f0 + larghezza_hz, bin_hz, true, nullptr, true, coerente);
}

Spettro spettro_continuo(const std::vector<Sorgente>& sorgenti, const Ricevitore& r, const Ambiente& amb,
                         double rilevamento_fascio, double f_min_hz, double f_max_hz, double bin_hz, bool coerente) {
    return spettro_interno(sorgenti, r, amb, rilevamento_fascio, f_min_hz, f_max_hz, bin_hz, false, nullptr, false, coerente);
}

double snr_impulso_db(const Sorgente& s, const Ricevitore& r, const Ambiente& amb, double rilevamento_fascio, double f) {
    if (!r.sensore || !s.fonte || s.fonte->componenti.empty()) return -300.0;
    // la finestra stretta quanto la prima componente: la sua attivita' vale 1, le piu' lunghe anche, le piu' brevi meno
    Sorgente x = s;
    x.finestra_s = std::max(s.fonte->componenti.front().durata_s, 0.01);
    x.tempo_s = x.finestra_s;
    const Fascio fa = fascio(r, rilevamento_fascio);
    const double d = distanza(r.pos, s.pos);
    const double brg = d > 0.0 ? rilevamento(r.pos, s.pos) : rilevamento_fascio;
    const double cieco = fa.cieco && !fa.baffles ? CIECO_DB : 0.0;
    const double rl = livello_fonte(x, f, false) - perdita_trasmissione(f, std::max(d, 1.0), s.quota_m, r.quota_m, amb).tl_db +
                      risposta(r, fa, brg, f, ampiezza_sorgente(s, d)) - cieco;
    return rl - rumore_fascio(f, r, amb, fa);
}

std::vector<RigaRicevuta> righe_ricevute(const std::vector<Sorgente>& sorgenti, const Ricevitore& r, const Ambiente& amb,
                                         double rilevamento_fascio, double f_min_hz, double f_max_hz, double bin_hz,
                                         bool coerente) {
    std::vector<RigaRicevuta> out;
    spettro_interno(sorgenti, r, amb, rilevamento_fascio, f_min_hz, f_max_hz, bin_hz, false, &out, false, coerente);   // solo le righe
    return out;
}

std::vector<RigaPropria> righe_proprie(const Ricevitore& r, const Ambiente& amb, double rilevamento_fascio, double bin_hz) {
    std::vector<RigaPropria> out;
    if (!r.sensore || bin_hz <= 0.0) return out;
    const Fascio fa = fascio(r, rilevamento_fascio);
    for (const auto& [hz, p, tipo] : righe_del_proprio(r, fa)) {
        const double rumore = potenza(rumore_fascio(hz, r, amb, fa) + 10.0 * std::log10(bin_hz));
        out.push_back({hz, decibel(p / rumore), tipo, nome_tipo_riga(tipo)});
    }
    // la vibrazione del cavo filato: righe proprie anche lei (sorgente -2 in righe_ricevute), quindi nel catalogo e nella
    // maschera. 03/11/2026: senza, la maschera della stazione e quella del sonarista la lasciavano fra le righe del
    // contatto, e a 4 nodi le righe della TB-16 votavano «nave» per un Victor III vero
    if (r.cavo_filato_m > 0.0) {
        const auto fv = frequenze_vibrazione(r.sensore->cavo_diametro_m, r.velocita_kn);
        for (std::size_t h = 0; h < fv.size(); ++h)
            out.push_back({fv[h], VIBRAZIONE_DB + (h > 0 ? VIBRAZIONE_ARMONICA_DB : 0.0) - 10.0 * std::log10(bin_hz),
                           TipoRiga::ausiliario, "cavo"});
    }
    std::sort(out.begin(), out.end(), [](const RigaPropria& a, const RigaPropria& b) { return a.hz < b.hz; });
    return out;
}

void togli_righe(std::vector<RigaOsservata>& righe, const std::vector<double>& hz, double tolleranza_hz) {
    std::erase_if(righe, [&](const RigaOsservata& x) {
        return std::any_of(hz.begin(), hz.end(), [&](double h) { return std::fabs(x.hz - h) <= tolleranza_hz; });
    });
}

// --- cascata ---

RigaCascata cascata(const std::vector<Sorgente>& sorgenti, const Ricevitore& r, const Ambiente& amb, int fasci,
                    double banda_hz, Casuale* caso, const std::vector<const CampoPerdita*>* campi) {
    RigaCascata out;
    if (!r.sensore || fasci <= 0) return out;
    const Sensore& sn = *r.sensore;
    // banda predefinita: la piu' alta del sensore, cioe' il fascio piu' stretto (la sfera a 300 Hz ha un fascio di 60 gradi)
    const double f = banda_hz > 0.0 ? banda_hz
                                    : (sn.bande_hz.empty() ? 1000.0 : *std::max_element(sn.bande_hz.begin(), sn.bande_hz.end()));
    const double w = 10.0 * std::log10(0.7 * f);
    out.rilevamento0 = 0.0;
    out.passo = 360.0 / fasci;
    // livello ricevuto di ogni sorgente (senza il fascio)
    struct Ricevuta { double brg, db, ampiezza; };
    std::vector<Ricevuta> ricevute;
    for (std::size_t i = 0; i < sorgenti.size(); ++i) {
        const Sorgente& s = sorgenti[i];
        if (!s.firma && !s.fonte) continue;
        const double d = distanza(r.pos, s.pos);
        const CampoPerdita* c = campi && i < campi->size() ? (*campi)[i] : nullptr;
        const auto raggi = c ? c->tl(f, d, r.quota_m) : std::nullopt;
        ricevute.push_back({d > 0.0 ? rilevamento(r.pos, s.pos) : 0.0,
                            livello_sorgente(f, s) + (s.var.attive ? aspetto_db(s, r.pos, f) : 0.0) + w -
                                (raggi ? *raggi : perdita_trasmissione(f, d, s.quota_m, r.quota_m, amb).tl_db),
                            ampiezza_sorgente(s, d)});
    }
    for (int b = 0; b < fasci; ++b) {
        const Fascio fa = fascio(r, b * out.passo);
        const double n = potenza(rumore_fascio(f, r, amb, fa) + w);
        double s = 0.0;
        for (const Ricevuta& x : ricevute)
            s += potenza(x.db + risposta(r, fa, x.brg, f, x.ampiezza) - (fa.cieco && !fa.baffles ? CIECO_DB : 0.0));
        double db = decibel((s + n) / n);
        if (caso) {
            const double u1 = std::max(caso->uniforme(), 1e-12), u2 = caso->uniforme();
            db += GRANA_DB * std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * PI * u2);
        }
        out.db.push_back(static_cast<float>(std::max(db, 0.0)));
        out.cieco.push_back(fa.cieco);
    }
    return out;
}

// --- DEMON ---

namespace {
// le righe DEMON di una sorgente nel fascio `fa` (risposta del fascio in dB verso di lei), sommate in `out` col massimo
void demon_in(Spettro& out, const Sorgente& s, const Ricevitore& r, const Ambiente& amb, const Fascio& fa, double risposta_db) {
    if (!s.firma || !cava(s) || s.fedelta < FEDELTA_SENZA_PALE) return;   // il falso contatto senza elica vera (D3)
    const double fs = frequenza_asse_hz(s);
    if (fs <= 0.0) return;
    const std::size_t n = out.snr_db.size();
    const double bin = out.df_hz;
    const double d = distanza(r.pos, s.pos);
    const double w = 10.0 * std::log10(DEMON_LARGHEZZA_HZ);
    const double snr_alta = livello_sorgente(DEMON_BANDA_HZ, s) + (s.var.attive ? aspetto_db(s, r.pos, DEMON_BANDA_HZ) : 0.0) + w -
                            perdita_trasmissione(DEMON_BANDA_HZ, d, s.quota_m, r.quota_m, amb).tl_db -
                            (rumore_fascio(DEMON_BANDA_HZ, r, amb, fa) + w) - (fa.cieco && !fa.baffles ? CIECO_DB : 0.0) + risposta_db;
    const double riga = std::clamp(snr_alta + DEMON_PROFONDITA_DB, 0.0, 40.0);
    if (riga <= 0.0) return;
    const double fp = s.firma->pale * fs;
    const auto metti = [&](double f, double db) {
        if (f <= 0.0 || f >= static_cast<double>(n) * bin || db <= 0.0) return;
        float& x = out.snr_db[static_cast<std::size_t>(std::floor(f / bin + 1e-9))];
        x = std::max(x, static_cast<float>(db));
    };
    metti(fs, riga - 6.0);
    metti(2.0 * fs, riga - 10.0);
    for (int k = 1; k <= 3; ++k) metti(k * fp, riga - 4.0 * (k - 1));
}
Spettro demon_vuoto(double f_max, double bin) {
    Spettro out;
    out.f0_hz = 0.0;
    out.df_hz = bin;
    if (bin > 0.0 && f_max > 0.0) out.snr_db.assign(static_cast<std::size_t>(std::ceil(f_max / bin)), 0.0f);
    return out;
}
}  // namespace

Spettro demon(const Sorgente& s, const Ricevitore& r, const Ambiente& amb, double f_max, double bin) {
    Spettro out = demon_vuoto(f_max, bin);
    if (out.snr_db.empty() || !r.sensore) return out;
    const double d = distanza(r.pos, s.pos);
    demon_in(out, s, r, amb, fascio(r, d > 0.0 ? rilevamento(r.pos, s.pos) : r.rotta), 0.0);   // il fascio sulla sorgente
    return out;
}

Spettro demon_fascio(const std::vector<Sorgente>& sorgenti, const Ricevitore& r, const Ambiente& amb, double rilevamento_fascio,
                     double f_max, double bin) {
    Spettro out = demon_vuoto(f_max, bin);
    if (out.snr_db.empty() || !r.sensore) return out;
    const Fascio fa = fascio(r, rilevamento_fascio);
    for (const Sorgente& s : sorgenti) {
        const double d = distanza(r.pos, s.pos);
        const double brg = d > 0.0 ? rilevamento(r.pos, s.pos) : rilevamento_fascio;
        demon_in(out, s, r, amb, fa, risposta(r, fa, brg, DEMON_BANDA_HZ, ampiezza_sorgente(s, d)));
    }
    return out;
}

std::vector<RigaOsservata> righe_osservate(const Spettro& s, double soglia) {
    std::vector<RigaOsservata> out;
    const std::size_t n = s.snr_db.size();
    constexpr std::size_t FINESTRA = 15;
    for (std::size_t i = 0; i < n; ++i) {
        const float v = s.snr_db[i];
        if (v < soglia) continue;
        if (i > 0 && s.snr_db[i - 1] > v) continue;
        if (i + 1 < n && s.snr_db[i + 1] >= v) continue;
        // il fondo locale: la mediana attorno al picco (un continuo forte non e' una riga)
        std::vector<float> w;
        for (std::size_t j = i >= FINESTRA ? i - FINESTRA : 0; j < std::min(n, i + FINESTRA + 1); ++j)
            if (j != i) w.push_back(s.snr_db[j]);
        std::nth_element(w.begin(), w.begin() + static_cast<std::ptrdiff_t>(w.size() / 2), w.end());
        const double fondo = w.empty() ? 0.0 : w[w.size() / 2];
        if (v - fondo >= soglia) out.push_back({s.frequenza(i), v});
    }
    return out;
}

std::optional<LetturaDemon> leggi_demon(const Spettro& d, double soglia) {
    const auto picchi = righe_osservate(d, soglia);
    if (picchi.size() < 2) return std::nullopt;
    const auto pala = std::max_element(picchi.begin(), picchi.end(), [](const auto& a, const auto& b) { return a.snr_db < b.snr_db; });
    for (const auto& p : picchi) {   // dal piu' basso: la fondamentale d'asse
        if (p.hz >= pala->hz) break;
        const double z = pala->hz / p.hz;
        const int zi = static_cast<int>(std::lround(z));
        // l'asse a 1-3 Hz quantizzato nel bin sposta il rapporto di qualche decimo
        if (zi >= 2 && zi <= 9 && std::fabs(z - zi) < 0.45) return LetturaDemon{p.hz, pala->hz, zi};
    }
    return std::nullopt;
}

std::vector<LetturaDemon> leggi_demon_fascio(const Spettro& d, double soglia) {
    std::vector<LetturaDemon> out;
    std::vector<RigaOsservata> picchi = righe_osservate(d, soglia);
    // una serie alla volta: la piu' forte, poi si tolgono le sue armoniche (asse, doppio dell'asse, pala per 1-3) e si
    // riprova con quello che resta
    while (picchi.size() >= 2 && out.size() < 4) {
        Spettro resto = d;
        std::fill(resto.snr_db.begin(), resto.snr_db.end(), 0.0f);
        for (const auto& p : picchi) {
            const std::size_t i = static_cast<std::size_t>(std::floor((p.hz - d.f0_hz) / d.df_hz));
            if (i < resto.snr_db.size()) resto.snr_db[i] = static_cast<float>(p.snr_db);
        }
        const auto l = leggi_demon(resto, soglia);
        if (!l) break;
        out.push_back(*l);
        const double tol = 1.5 * d.df_hz;
        std::erase_if(picchi, [&](const RigaOsservata& p) {
            for (double h : {l->asse_hz, 2.0 * l->asse_hz, l->pala_hz, 2.0 * l->pala_hz, 3.0 * l->pala_hz})
                if (std::fabs(p.hz - h) <= tol) return true;
            return false;
        });
    }
    return out;
}

// --- analizzatore di firma ---

std::vector<Ipotesi> confronta_firma(const std::vector<RigaOsservata>& righe, const CatalogoAcustico& c,
                                     std::optional<int> pale, double bin_hz) {
    std::vector<Ipotesi> out;
    std::vector<double> punti;
    for (const auto& [classe, f] : c.firme) {
        if (f.righe.empty()) continue;   // armi, esche e firme generiche non si classificano
        double p = 0.0;
        int concordi = 0;
        std::vector<bool> spiegata(righe.size(), false);
        for (const RigaFirma& e : f.righe) {
            if (e.multiplo_asse > 0.0) continue;   // legate all'asse: la loro frequenza dipende dalla velocita'
            bool presa = false;
            for (std::size_t k = 0; k < righe.size(); ++k)
                if (std::fabs(righe[k].hz - e.hz) <= std::max(bin_hz, TOLLERANZA_DOPPLER * e.hz)) {
                    spiegata[k] = true;
                    presa = true;
                }
            if (presa) { p += std::pow(10.0, e.db_relativo / 20.0); ++concordi; }
        }
        p -= PUNTI_ESTRANEA * static_cast<double>(std::count(spiegata.begin(), spiegata.end(), false));
        if (pale) p += f.pale == *pale ? 1.0 : -0.5;
        out.push_back({classe, f.nazionalita, f.tipo_unita, 0.0, concordi});
        punti.push_back(p);
    }
    if (out.empty()) return out;
    const double massimo = *std::max_element(punti.begin(), punti.end());
    double somma = 0.0;
    for (std::size_t i = 0; i < out.size(); ++i) somma += out[i].confidenza = std::exp((punti[i] - massimo) / TEMPERATURA);
    for (auto& h : out) h.confidenza /= somma;
    std::stable_sort(out.begin(), out.end(), [](const Ipotesi& a, const Ipotesi& b) { return a.confidenza > b.confidenza; });
    return out;
}

}  // namespace m688

// --- libreria consultabile (S5b) ---

namespace m688 {

namespace {

std::string numero_breve(double x) {
    char b[32];
    std::snprintf(b, sizeof b, x == std::floor(x) ? "%.0f" : "%.1f", x);
    return b;
}

// `rete`: la fondamentale della rete di bordo della classe (la sua riga di rete piu' bassa)
std::string etichetta_riga(const RigaFirma& r, double rete) {
    std::string e = std::string(nome_tipo_riga(r.tipo)) + " " + numero_breve(r.hz) + " Hz";
    if (r.tipo == TipoRiga::rete && rete > 0.0) {
        const double k = r.hz / rete;
        if (std::fabs(k - std::round(k)) < 1e-6 && k >= 2.0) e = "rete " + numero_breve(rete) + " Hz x" + numero_breve(std::round(k));
    }
    if (r.v_min_kn > 0.0 && r.v_max_kn < 1e8) e += " (" + numero_breve(r.v_min_kn) + "-" + numero_breve(r.v_max_kn) + " kn)";
    else if (r.v_min_kn > 0.0) e += " (da " + numero_breve(r.v_min_kn) + " kn)";
    else if (r.v_max_kn < 1e8) e += " (fino a " + numero_breve(r.v_max_kn) + " kn)";
    return e;
}

std::string categoria_scheda(const std::string& categoria_sorgente) {
    if (categoria_sorgente == "civile" || categoria_sorgente == "industriale") return "umano";
    return categoria_sorgente;
}

std::string descrivi_ritmo(const Ritmo& r) {
    std::string s;
    switch (r.processo) {
        case Processo::continuo: return "continuo";
        case Processo::singolo: s = "una volta"; break;
        case Processo::periodico: s = "ogni " + numero_breve(r.intervallo_min_s) + "-" + numero_breve(r.intervallo_max_s) + " s"; break;
        case Processo::poisson: s = numero_breve(r.tasso_h) + " eventi all'ora, a caso"; break;
        case Processo::treno:
            s = "ogni " + numero_breve(r.intervallo_min_s) + "-" + numero_breve(r.intervallo_max_s) + " s in serie di " +
                numero_breve(r.serie_min) + " min con pause di " + numero_breve(r.pausa_s) + " s";
            break;
    }
    if (r.gruppo_max > 1)
        s += ", a gruppi di " + std::to_string(r.gruppo_min) + "-" + std::to_string(r.gruppo_max) + " a " +
             numero_breve(r.interno_min_s) + "-" + numero_breve(r.interno_max_s) + " s";
    return s;
}

VoceLibreria voce_da_firma(const FirmaRumore& f) {
    VoceLibreria v;
    v.id = f.classe; v.nome = f.classe; v.nome_breve = f.classe;
    v.categoria = f.tipo_unita; v.categoria_scheda = "navale"; v.nazionalita = f.nazionalita; v.tipo_unita = f.tipo_unita;
    v.unita_navale = true;
    double rete = 0.0;
    for (const RigaFirma& r : f.righe)
        if (r.tipo == TipoRiga::rete && (rete == 0.0 || r.hz < rete)) rete = r.hz;
    for (const RigaFirma& r : f.righe) {
        if (r.multiplo_asse > 0.0) continue;   // legate all'asse: cambiano con la velocita', vanno negli indizi
        v.righe.push_back({r.hz, r.db_relativo, r.tipo, etichetta_riga(r, rete), 0.0, 0.0, r.v_min_kn, r.v_max_kn});
    }
    v.pale = f.pale; v.giri_per_nodo = f.giri_per_nodo;
    if (f.pale > 0 && f.giri_per_nodo > 0.0)
        v.indizi.push_back("elica a " + std::to_string(f.pale) + " pale: pala = " + std::to_string(f.pale) + " x giri/60");
    if (f.pale > 0 && f.giri_costanti > 0.0)
        v.indizi.push_back("passo variabile: l'asse gira a " + numero_breve(f.giri_costanti) + " giri al minuto a ogni velocita', asse e pala fissi");
    for (const RigaFirma& r : f.righe)
        if (r.multiplo_asse > 0.0 && r.tipo == TipoRiga::diesel && f.pale > 0) {   // la prima: le accensioni del motore
            v.indizi.push_back("diesel in presa diretta: accensioni a " + numero_breve(r.multiplo_asse) + " volte l'asse, " +
                               numero_breve(r.multiplo_asse / f.pale) + " volte la pala a ogni velocita'");
            break;
        }
    {   // turbina e riduttore legati all'asse (i sottomarini nucleari, blocco 2 del sonar)
        std::string legate;
        for (const RigaFirma& r : f.righe)
            if (r.multiplo_asse > 0.0 && (r.tipo == TipoRiga::turbina || r.tipo == TipoRiga::riduttore))
                legate += (legate.empty() ? "" : ", ") + std::string(nome_tipo_riga(r.tipo)) + " " + numero_breve(r.multiplo_asse);
        if (!legate.empty())
            v.indizi.push_back("righe deboli che salgono coi giri, in volte la frequenza d'asse: " + legate);
    }
    v.ritmo = "continuo"; v.fonte = f.fonte; v.confidenza = "bassa";
    return v;
}

VoceLibreria voce_da_sorgente(const SorgenteLibreria& f) {
    VoceLibreria v;
    v.id = f.id; v.nome = f.nome; v.nome_breve = f.nome_breve;
    v.categoria = f.categoria; v.categoria_scheda = categoria_scheda(f.categoria); v.sottocategoria = f.sottocategoria;
    v.nazionalita = f.categoria; v.tipo_unita = f.sottocategoria;
    const TipoRiga tipo = f.categoria == "biologico" ? TipoRiga::biologico
                          : (f.categoria == "geofisico" || f.categoria == "meteo") ? TipoRiga::geofisico : TipoRiga::industriale;
    for (const Componente& c : f.componenti)
        switch (c.tipo) {
            case TipoComponente::tonale:
                v.righe.push_back({c.hz, c.livello_rel_db, tipo, "tonale " + numero_breve(c.hz) + " Hz"});
                break;
            case TipoComponente::famiglia:
                for (int k = 1; k <= std::max(c.armoniche, 1); ++k)
                    v.righe.push_back({k * c.hz, c.livello_rel_db - c.decadimento_db * (k - 1), tipo,
                                       "famiglia " + numero_breve(c.hz) + " Hz x" + std::to_string(k)});
                break;
            case TipoComponente::sweep:
                v.righe.push_back({c.f_da, c.livello_rel_db, tipo, "sweep " + numero_breve(c.f_da) + "-" + numero_breve(c.f_a) + " Hz",
                                   c.f_a, c.durata_s});
                break;
            case TipoComponente::banda:
            case TipoComponente::impulso: v.bande.emplace_back(c.f_lo, c.f_hi); break;
        }
    v.indizi = f.indizi; v.confonde_con = f.confonde_con;
    v.ritmo = descrivi_ritmo(f.ritmo); v.aurale = f.aurale; v.fonte = f.fonte; v.confidenza = f.confidenza;
    return v;
}

}  // namespace

std::vector<std::string> elenco_libreria(const CatalogoAcustico& c, const std::string& scheda) {
    std::vector<std::pair<std::string, std::string>> voci;   // (nome, id)
    for (const auto& [id, f] : c.firme)
        if (!f.righe.empty() && (scheda.empty() || scheda == "navale")) voci.emplace_back(id, id);
    for (const auto& [id, f] : c.sorgenti)
        if (scheda.empty() || scheda == categoria_scheda(f.categoria)) voci.emplace_back(f.nome, id);
    std::sort(voci.begin(), voci.end());
    std::vector<std::string> out;
    for (const auto& [nome, id] : voci) out.push_back(id);
    return out;
}

std::optional<VoceLibreria> voce_libreria(const CatalogoAcustico& c, const std::string& id) {
    if (const auto* f = c.firma(id); f && !f->righe.empty()) {
        VoceLibreria v = voce_da_firma(*f);
        if (const TipoCivile* t = c.tipo_civile(id)) {   // i tipi del traffico: il nome e gli intervalli dell'individuo
            v.nome = t->nome;
            v.sottocategoria = "civile";
            v.indizi.push_back("ogni nave e' diversa: pale " + numero_breve(t->pale.min) + "-" + numero_breve(t->pale.max) +
                               ", rete a 50 o 60 Hz, giri e cilindri propri; le righe sono di una nave tipica");
            v.fonte = t->fonte;
            v.confidenza = t->confidenza;
        }
        return v;
    }
    if (const auto* s = c.sorgente(id)) return voce_da_sorgente(*s);
    return std::nullopt;
}

std::vector<ConfrontoRapporti> confronta_rapporti(const std::vector<RigaOsservata>& righe, const CatalogoAcustico& c,
                                                  std::optional<int> pale, double tol, double doppler_max) {
    std::vector<ConfrontoRapporti> out;
    constexpr double ESTRANEA = 0.4, TEMP = 0.35;
    constexpr double TOLLERANZA_GIRI = 0.035, TOLLERANZA_RETE = 0.006, PESO_GIRI = 0.7;
    for (const std::string& id : elenco_libreria(c)) {
        const VoceLibreria v = *voce_libreria(c, id);
        std::vector<const RigaLibreria*> fisse;
        for (const auto& r : v.righe) if (r.durata_s == 0.0 || r.hz_fine == 0.0) fisse.push_back(&r);
        if (fisse.empty()) continue;
        // R6: le righe dei giri variano dell'1-3% da uno scafo all'altro, quelle sulla rete dello 0,5%: si accettano
        // piu' larghe e contano meno; la rete e l'elica tengono il fattore Doppler preciso
        const auto tolleranza = [&](const RigaLibreria& r) {
            return riga_dei_giri(r.tipo) ? std::max(tol, TOLLERANZA_GIRI) : riga_sulla_rete(r.tipo) ? std::max(tol, TOLLERANZA_RETE) : tol;
        };
        const auto peso = [](const RigaLibreria& r) { return riga_dei_giri(r.tipo) ? PESO_GIRI : 1.0; };
        // il fattore k che allinea piu' righe: ogni coppia osservata/libreria ne propone uno
        const auto valuta_k = [&](double k, ConfrontoRapporti* dettaglio) {
            double p = 0.0;
            std::vector<bool> presa(fisse.size(), false), spiegata(righe.size(), false);
            for (std::size_t o = 0; o < righe.size(); ++o)
                for (std::size_t l = 0; l < fisse.size(); ++l)
                    if (std::fabs(righe[o].hz - k * fisse[l]->hz) <= tolleranza(*fisse[l]) * righe[o].hz) {
                        spiegata[o] = true;
                        if (!presa[l]) { presa[l] = true; p += peso(*fisse[l]) * std::pow(10.0, fisse[l]->db_relativo / 20.0); }
                        if (dettaglio) dettaglio->concordi.emplace_back(righe[o], *fisse[l]);
                    }
            p -= ESTRANEA * static_cast<double>(std::count(spiegata.begin(), spiegata.end(), false));
            if (pale) p += v.pale == *pale ? 1.0 : -0.5;
            if (dettaglio) {
                for (std::size_t l = 0; l < fisse.size(); ++l) if (!presa[l]) dettaglio->mancanti.push_back(*fisse[l]);
                std::stable_sort(dettaglio->mancanti.begin(), dettaglio->mancanti.end(),
                                 [](const RigaLibreria& a, const RigaLibreria& b) { return a.db_relativo > b.db_relativo; });
                for (std::size_t o = 0; o < righe.size(); ++o) if (!spiegata[o]) dettaglio->in_piu.push_back(righe[o]);
            }
            return p;
        };
        double migliore_k = 1.0, migliore = valuta_k(1.0, nullptr);
        for (const auto& o : righe)
            for (const RigaLibreria* l : fisse) {
                const double k = o.hz / l->hz;
                if (std::fabs(k - 1.0) > doppler_max) continue;
                const double p = valuta_k(k, nullptr);
                if (p > migliore + 1e-9 || (std::fabs(p - migliore) <= 1e-9 && std::fabs(k - 1.0) < std::fabs(migliore_k - 1.0))) {
                    migliore = p;
                    migliore_k = k;
                }
            }
        ConfrontoRapporti r;
        r.id = id;
        valuta_k(migliore_k, &r);
        // k affinato sulle coppie concordi
        if (!r.concordi.empty()) {
            double num = 0.0, den = 0.0;
            for (const auto& [o, l] : r.concordi) { num += o.hz * l.hz; den += l.hz * l.hz; }
            migliore_k = num / den;
        }
        r.fattore_doppler = migliore_k;
        r.velocita_radiale_kn = (migliore_k - 1.0) * 1500.0 / NODO;
        r.punteggio = migliore;
        out.push_back(std::move(r));
    }
    if (out.empty()) return out;
    const double massimo = std::max_element(out.begin(), out.end(), [](const auto& a, const auto& b) { return a.punteggio < b.punteggio; })->punteggio;
    double somma = 0.0;
    for (auto& r : out) somma += r.confidenza = std::exp((r.punteggio - massimo) / TEMP);
    for (auto& r : out) r.confidenza /= somma;
    std::stable_sort(out.begin(), out.end(), [](const auto& a, const auto& b) { return a.confidenza > b.confidenza; });
    return out;
}

std::vector<FamigliaArmonica> famiglie_armoniche(const std::vector<RigaOsservata>& righe, double f_max,
                                                 const CatalogoAcustico* c, double tol, int minimo) {
    struct Candidata {
        double f1;
        std::vector<int> m;
        std::vector<std::size_t> membri;
        double compattezza = 0.0;   // membri / armonica piu' alta: una famiglia vera occupa le armoniche basse
    };
    std::vector<Candidata> candidate;
    for (const auto& o : righe)
        for (int n = 1; n <= 6; ++n) {
            const double f1 = o.hz / n;
            if (f1 < 1.0) continue;
            Candidata x{f1, {}, {}};
            for (std::size_t i = 0; i < righe.size(); ++i) {
                const int m = static_cast<int>(std::lround(righe[i].hz / f1));
                if (m >= 1 && std::fabs(righe[i].hz - m * f1) <= tol * righe[i].hz &&
                    std::find(x.m.begin(), x.m.end(), m) == x.m.end()) { x.m.push_back(m); x.membri.push_back(i); }
            }
            if (static_cast<int>(x.m.size()) < minimo) continue;
            double num = 0.0, den = 0.0;   // fondamentale ai minimi quadrati
            for (std::size_t j = 0; j < x.m.size(); ++j) { num += righe[x.membri[j]].hz * x.m[j]; den += static_cast<double>(x.m[j]) * x.m[j]; }
            x.f1 = num / den;
            x.compattezza = static_cast<double>(x.m.size()) / *std::max_element(x.m.begin(), x.m.end());
            if (x.compattezza < COMPATTEZZA_MIN) continue;   // sottoarmoniche e coincidenze sparse
            std::sort(x.membri.begin(), x.membri.end());
            candidate.push_back(std::move(x));
        }
    const auto peso = [](const Candidata& x) { return static_cast<double>(x.m.size()) * x.compattezza; };
    std::sort(candidate.begin(), candidate.end(), [&](const Candidata& a, const Candidata& b) {
        if (std::fabs(peso(a) - peso(b)) > 1e-9) return peso(a) > peso(b);
        return a.f1 > b.f1;
    });
    // una famiglia a k*f1 che spiega almeno meta' dei membri, altrettanto compatta, sostituisce quella a f1 (le righe di
    // rete a 50, 100, 150 Hz non diventano una famiglia a 25 Hz)
    const auto multipla = [&](const Candidata& x) -> const Candidata* {
        const Candidata* meglio = nullptr;
        for (const auto& g : candidate) {
            const double k = g.f1 / x.f1;
            if (k < 1.5 || std::fabs(k - std::round(k)) > 0.01 * k) continue;
            if (!std::includes(x.membri.begin(), x.membri.end(), g.membri.begin(), g.membri.end())) continue;
            if (2 * g.membri.size() < x.membri.size() || g.compattezza + 1e-9 < x.compattezza) continue;
            if (!meglio || g.f1 > meglio->f1) meglio = &g;
        }
        return meglio;
    };
    std::vector<FamigliaArmonica> out;
    std::vector<std::vector<std::size_t>> prese;
    for (const auto& candidata : candidate) {
        const Candidata* x = &candidata;
        while (const Candidata* g = multipla(*x)) x = g;
        bool coperta = false;   // tutti i membri tranne al piu' uno sono gia' in una famiglia presa
        for (const auto& p : prese) {
            std::vector<std::size_t> comuni;
            std::set_intersection(p.begin(), p.end(), x->membri.begin(), x->membri.end(), std::back_inserter(comuni));
            if (comuni.size() + 1 >= x->membri.size()) { coperta = true; break; }
        }
        if (coperta) continue;
        prese.push_back(x->membri);
        FamigliaArmonica f;
        f.f1_hz = x->f1;
        f.armoniche = x->m;
        std::sort(f.armoniche.begin(), f.armoniche.end());
        f.copertura = static_cast<double>(x->m.size()) / std::max(1.0, std::floor(f_max / x->f1));
        const auto vicino = [&](double base) { return std::fabs(x->f1 - base) <= 0.01 * base; };
        if (vicino(60.0)) f.indizio = "rete 60 Hz";
        else if (vicino(50.0)) f.indizio = "rete 50 Hz";
        else if (vicino(400.0)) f.indizio = "rete 400 Hz";
        // l'etichetta della libreria solo se si vedono abbastanza armoniche di quella famiglia
        if (f.indizio.empty() && c)
            for (const auto& [id, s] : c->sorgenti)
                for (const Componente& k : s.componenti) {
                    if (k.tipo != TipoComponente::famiglia || std::fabs(x->f1 - k.hz) > 0.02 * k.hz || !f.indizio.empty()) continue;
                    const auto viste = std::count_if(f.armoniche.begin(), f.armoniche.end(), [&](int m) { return m <= k.armoniche; });
                    if (viste >= std::max(3, (k.armoniche + 1) / 2)) f.indizio = s.nome_breve;
                }
        if (f.indizio.empty() && x->f1 < 30.0) {
            f.indizio = "elica (pala)";
            f.pala_hz = x->f1;
            for (const auto& o : righe) {   // la riga d'asse sotto la pala: pala / asse = pale
                if (o.hz >= x->f1 * 0.9) continue;
                const double z = x->f1 / o.hz;
                const int zi = static_cast<int>(std::lround(z));
                if (zi >= 3 && zi <= 9 && std::fabs(z - zi) < 0.15) { f.pale_stimate = zi; break; }
            }
        }
        if (f.indizio.empty()) f.indizio = "ignota";
        out.push_back(std::move(f));
    }
    return out;
}

Spettro differenza(const Spettro& a, const Spettro& b) {
    Spettro out = a;
    for (std::size_t i = 0; i < a.snr_db.size(); ++i) {
        const double f = a.frequenza(i);
        const double x = (f - b.f0_hz) / b.df_hz - 0.5;   // posizione nella griglia di b (centri dei bin)
        double vb = 0.0;
        if (!b.snr_db.empty() && x >= -0.5 && x <= static_cast<double>(b.snr_db.size()) - 0.5) {
            const auto j = static_cast<std::size_t>(std::clamp(std::floor(x), 0.0, static_cast<double>(b.snr_db.size() - 1)));
            const std::size_t k = std::min(j + 1, b.snr_db.size() - 1);
            const double t = std::clamp(x - static_cast<double>(j), 0.0, 1.0);
            vb = (1.0 - t) * b.snr_db[j] + t * b.snr_db[k];
        }
        out.snr_db[i] = static_cast<float>(a.snr_db[i] - vb);
    }
    return out;
}

}  // namespace m688
