#include "nucleo/traccia_replay.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include <set>
#include <sstream>
#include <stdexcept>

#include "nucleo/albo.hpp"
#include "nucleo/geometria.hpp"

namespace m688 {

namespace {

constexpr int VERSIONE = 2;
constexpr const char* MAGICO = "M688TR\n";
constexpr double SENZA_INTERPOLARE_S = 30.0;   // fra due campioni piu' lontani di cosi' non si interpola

// gli schemi che questo motore scrive: tag, campi «nome:tipo[passo]» (k chiave assoluta, i intero, q quantizzato,
// a angolo, s stringa). I tipi delta sono i record U e C
const char* const SCHEMA_U = "id:k x:q0.5 y:q0.5 quota:q0.1 rotta:a0.01 velocita:q0.01 stato:i alberi:i lanciatore:i collegata:i";
// versione 2: stima (meno la vera), ellisse, allerta ed emissioni di un'unita', solo quando cambiano (e nei fotogrammi
// chiave): valgono dal fotogramma in cui compaiono fino al prossimo record V dell'unita'
const char* const SCHEMA_V = "id:k ex:q1 ey:q1 sigma:q1 allerta:i emissioni:i sigma_min:q1 orientamento:a1";
// la TMA in corso e le armi a bordo, a parte perche' cambiano di rado: solo quando cambiano (e nei fotogrammi chiave)
const char* const SCHEMA_A = "id:k tma_b:i tma_g:i tma_s:i tma_o:i tma_x:q1 tma_y:q1 tma_r:a1 tma_v:q0.1 armi:i tubi:i";
constexpr double SOGLIA_STIMA_M = 10.0, SOGLIA_SIGMA_M = 5.0, SOGLIA_ORIENTAMENTO = 5.0, SOGLIA_TMA_M = 50.0;   // l'asse: gradi, modulo 180
const char* const SCHEMA_N = "id:k classe:s categoria:s flag:i";
const char* const SCHEMA_C = "id:k lettera:i qualita:q1 durata:q1 rilevamento:a0.01 specchio:a0.01 ha_specchio:i nota:i "
                             "x:q0.5 y:q0.5 quota:q0.1 sensore:s classe:s";
const char* const SCHEMA_R = "tempo:q0.1 x:q0.5 y:q0.5 rilevamento:a0.01";
const char* const SCHEMA_E = "id:k tempo:q0.1 tipo:s testo_it:s testo_en:s interesse:i noto:i unita:i altra:i distanza:q1 "
                             "classe:s categoria:s arma:s lato:i giusta:i livello:i scoperta:q0.1 codice:i valore:q0.001 "
                             "id_arma:i creduta:s x:q1 y:q1";   // gli ultimi quattro dalla versione 2
const char* const SCHEMA_I = "tempo:q0.1 tipo:s unita:i altra:i n:i m:i a:q0.1 b:q0.1 c:q0.01 d:q0.01 s:s";   // versione 2
const char* const SCHEMA_K = "tempo:q0.1 osservatore:i bersaglio:i qualita:q1 rilevamento:a0.1 eta:q1 solido:i nota:i "
                             "perso:i x:q1 y:q1 sensore:s classe:s";   // versione 2
const char* const SCHEMA_P = "tempo:q0.1 lato:i vuoto:i bersaglio:i posizione:i x:q1 y:q1 ox:q1 oy:q1 rilevamento:a0.1 "
                             "mag:q1 min:q1 orient:a1 dato:q0.1 sensore:s fonte:i categoria:s classe:s qualita:q1";   // v2
// il vetrino di Dio (docs/28, D1; 31/10/2026): i lettori di prima saltano il record X, questo legge le tracce senza
const char* const SCHEMA_X = "id:i tempo:q0.001 fase:i tipo:s unita:i punto:i x:q0.1 y:q0.1 raggio:q0.1 durata:q0.001 "
                             "chiavi:s riga_it:s riga_en:s";
const char* const SCHEMA_S = "modo:s sensore:s fascio:a0.01 filtro:i stazione:i silenzio:i bersaglio:i soluzione:i "
                             "distanza:q1 rotta:a0.01 velocita:q0.01 bersaglio_tma:i";

enum Stato : int { DISTRUTTA = 1, PERSA = 2, INCAGLIATA = 4, SUL_FONDO = 8, RIMOSSA = 16 };

struct Campo {
    std::string nome;
    char tipo = 'i';
    double passo = 1.0;
};
std::vector<Campo> schema(const std::string& testo) {
    std::vector<Campo> out;
    std::istringstream in(testo);
    for (std::string x; in >> x;) {
        const auto p = x.find(':');
        if (p == std::string::npos || p + 1 >= x.size()) continue;
        Campo c;
        c.nome = x.substr(0, p);
        c.tipo = x[p + 1];
        if (p + 2 < x.size()) c.passo = std::atof(x.c_str() + p + 2);
        if (c.passo <= 0.0) c.passo = 1.0;
        out.push_back(std::move(c));
    }
    return out;
}

// --- varint zigzag ---
void scrivi_u(std::vector<std::uint8_t>& b, std::uint64_t v) {
    while (v >= 0x80) {
        b.push_back(static_cast<std::uint8_t>(v | 0x80));
        v >>= 7;
    }
    b.push_back(static_cast<std::uint8_t>(v));
}
void scrivi_s(std::vector<std::uint8_t>& b, std::int64_t v) {
    scrivi_u(b, (static_cast<std::uint64_t>(v) << 1) ^ static_cast<std::uint64_t>(v >> 63));
}
struct Lettore {
    const std::uint8_t* p;
    const std::uint8_t* fine;
    std::uint64_t u() {
        std::uint64_t v = 0;
        for (int k = 0; k < 64; k += 7) {
            if (p >= fine) throw std::runtime_error("traccia troncata");
            const std::uint8_t c = *p++;
            v |= static_cast<std::uint64_t>(c & 0x7f) << k;
            if (!(c & 0x80)) return v;
        }
        throw std::runtime_error("varint troppo lungo");
    }
    std::int64_t s() {
        const std::uint64_t z = u();
        return static_cast<std::int64_t>(z >> 1) ^ -static_cast<std::int64_t>(z & 1);
    }
    std::uint8_t byte() {
        if (p >= fine) throw std::runtime_error("traccia troncata");
        return *p++;
    }
};

std::int64_t quantizza(double v, double passo) { return static_cast<std::int64_t>(std::llround(v / passo)); }
std::int64_t angolo(double gradi, double passo) {   // in [0, giro)
    const std::int64_t giro = quantizza(360.0, passo);
    return ((quantizza(normalizza_360(gradi), passo) % giro) + giro) % giro;
}
std::int64_t delta_angolo(std::int64_t v, std::int64_t prima, double passo) {
    const std::int64_t giro = quantizza(360.0, passo);
    std::int64_t d = ((v - prima) % giro + giro) % giro;
    if (d > giro / 2) d -= giro;
    return d;
}

bool veloce(const std::string& categoria) { return categoria == "aereo" || categoria == "arma"; }

}  // namespace

// --- registratore ---

RegistratoreTraccia::RegistratoreTraccia(std::string missione, std::uint64_t seme, double ripresa_da) {
    meta_["missione"] = std::move(missione);
    meta_["seme"] = std::to_string(seme);
    meta_["ripresa_da"] = std::to_string(ripresa_da);
}

void RegistratoreTraccia::metadato(const std::string& chiave, const std::string& valore) { meta_[chiave] = valore; }

bool RegistratoreTraccia::serve(double t, std::size_t eventi) const {
    return fotogrammi_ == 0 || t >= prossimo_veloci_ - 1e-9 || eventi > eventi_visti_;
}

int RegistratoreTraccia::stringa(std::vector<std::uint8_t>& b, const std::string& s) {
    const auto it = stringhe_.find(s);
    if (it != stringhe_.end()) return it->second;
    const int k = static_cast<int>(stringhe_.size());
    stringhe_[s] = k;
    b.push_back('T');
    scrivi_u(b, static_cast<std::uint64_t>(k));
    scrivi_u(b, s.size());
    b.insert(b.end(), s.begin(), s.end());
    return k;
}

void RegistratoreTraccia::aggiungi(const Fotogramma& f, std::size_t eventi) {
    const bool nuovo_evento = eventi > eventi_visti_;
    const bool chiave = fotogrammi_ % OGNI_CHIAVE == 0;
    const bool lento = chiave || f.t >= prossimo_ - 1e-9 || nuovo_evento;
    std::vector<std::uint8_t> b, stringhe;   // le stringhe nuove vanno prima dei record che le usano
    const auto gruppo = [&](char tag, std::size_t n) {
        b.push_back(static_cast<std::uint8_t>(tag));
        scrivi_u(b, n);
    };
    // unita': nuove (anagrafica), da scrivere (veloci sempre, lente nei fotogrammi lenti), uscite dal gioco
    std::vector<const StatoUnita*> scritte, nuove;
    std::map<int, const StatoUnita*> presenti;
    std::set<int> a_contatto;
    for (const StatoContatto& c : f.contatti) a_contatto.insert(c.id);
    for (const StatoUnita& u : f.unita) {
        presenti[u.id] = &u;
        const bool nuova = !ultimo_unita_.count(u.id);
        if (nuova) {
            nuove.push_back(&u);
            veloce_[u.id] = veloce(u.categoria);
        }
        bool scrivi = nuova || lento || veloce_[u.id];
        if (scrivi && !nuova && !chiave && (u.flag & 0x18) == 0 && u.categoria == "nave" && !a_contatto.count(u.id) &&
            f.t + 1.25 * PASSO_S - scritta_[u.id] <= PASSO_NEUTRALI_S) {   // il prossimo fotogramma lento ci sta ancora
            // una neutrale rada: solo se le cambia lo stato (le ultime voci del record U)
            const auto& prima = ultimo_unita_[u.id];
            const int stato = (u.distrutta ? DISTRUTTA : 0) | (u.persa ? PERSA : 0) | (u.incagliata ? INCAGLIATA : 0) |
                              (u.sul_fondo ? SUL_FONDO : 0);
            scrivi = prima.size() < 9 || prima[5] != stato || prima[6] != u.alberi || prima[7] != u.lanciatore || prima[8] != u.collegata;
        }
        if (scrivi) {
            scritte.push_back(&u);
            scritta_[u.id] = f.t;
        }
    }
    std::vector<int> uscite;
    for (const auto& [id, v] : ultimo_unita_)
        if (!presenti.count(id)) uscite.push_back(id);
    if (!nuove.empty()) {
        std::vector<std::uint8_t> g;
        for (const StatoUnita* u : nuove) {
            scrivi_s(g, u->id);
            scrivi_s(g, stringa(stringhe, u->classe));
            scrivi_s(g, stringa(stringhe, u->categoria));
            scrivi_s(g, u->flag);
        }
        gruppo('N', nuove.size());
        b.insert(b.end(), g.begin(), g.end());
    }
    const auto valori_unita = [](const StatoUnita& u, bool rimossa) {
        const int stato = (u.distrutta ? DISTRUTTA : 0) | (u.persa ? PERSA : 0) | (u.incagliata ? INCAGLIATA : 0) |
                          (u.sul_fondo ? SUL_FONDO : 0) | (rimossa ? RIMOSSA : 0);
        return std::vector<std::int64_t>{quantizza(u.pos.x, 0.5), quantizza(u.pos.y, 0.5), quantizza(u.quota, 0.1),
                                         angolo(u.rotta, 0.01), quantizza(u.velocita_kn, 0.01), stato, u.alberi,
                                         u.lanciatore, u.collegata};
    };
    if (!scritte.empty() || !uscite.empty()) {
        gruppo('U', scritte.size() + uscite.size());
        const auto scrivi = [&](int id, std::vector<std::int64_t> v) {
            scrivi_s(b, id);
            auto& prima = ultimo_unita_[id];
            const bool assoluto = chiave || prima.empty();
            for (std::size_t k = 0; k < v.size(); ++k) {
                if (assoluto) scrivi_s(b, v[k]);
                else if (k == 3) scrivi_s(b, delta_angolo(v[k], prima[k], 0.01));
                else scrivi_s(b, v[k] - prima[k]);
            }
            prima = std::move(v);
        };
        for (const StatoUnita* u : scritte) scrivi(u->id, valori_unita(*u, false));
        for (const int id : uscite) {   // l'ultimo stato noto, con la bandierina d'uscita
            std::vector<std::int64_t> v = ultimo_unita_[id];
            v[5] |= RIMOSSA;
            scrivi(id, v);
            ultimo_unita_.erase(id);
            ultimo_contatto_.erase(id);
        }
    }
    // la navigazione, l'allerta e le emissioni delle unita' scritte, quando sono cambiate
    {
        std::vector<std::uint8_t> g;
        std::size_t n = 0;
        for (const StatoUnita* u : scritte) {
            const Vec2 e{u->pos_stimata.x - u->pos.x, u->pos_stimata.y - u->pos.y};
            auto it = ultimo_extra_.find(u->id);
            const bool cambia = chiave || it == ultimo_extra_.end() || distanza(e, it->second.errore) >= SOGLIA_STIMA_M ||
                                std::fabs(u->sigma_nav_m - it->second.sigma) >= SOGLIA_SIGMA_M ||
                                std::fabs(u->sigma_min_m - it->second.sigma_min) >= SOGLIA_SIGMA_M ||
                                std::fabs(normalizza_180(2.0 * (u->orientamento_deg - it->second.orientamento))) >= 2.0 * SOGLIA_ORIENTAMENTO ||
                                u->allerta != it->second.allerta || u->emissioni != it->second.emissioni;
            if (!cambia) continue;
            scrivi_s(g, u->id);
            scrivi_s(g, quantizza(e.x, 1.0));
            scrivi_s(g, quantizza(e.y, 1.0));
            scrivi_s(g, quantizza(u->sigma_nav_m, 1.0));
            scrivi_s(g, u->allerta);
            scrivi_s(g, u->emissioni);
            scrivi_s(g, quantizza(u->sigma_min_m, 1.0));
            scrivi_s(g, angolo(u->orientamento_deg, 1.0));
            ultimo_extra_[u->id] = {e, u->sigma_nav_m, u->sigma_min_m, u->orientamento_deg, u->allerta, u->emissioni};
            ++n;
        }
        for (const int id : uscite) ultimo_extra_.erase(id);
        if (n) {
            gruppo('V', n);
            b.insert(b.end(), g.begin(), g.end());
        }
    }
    // la TMA in corso e le armi, quando cambiano (record A)
    {
        std::vector<std::uint8_t> g;
        std::size_t n = 0;
        for (const StatoUnita* u : scritte) {
            auto it = ultima_tma_armi_.find(u->id);
            const bool vuota = u->tma_bersaglio < 0 && u->armi_rimaste < 0 && u->tubi_carichi < 0;
            const bool cambia = it == ultima_tma_armi_.end() ? !vuota
                                : chiave || u->tma_bersaglio != it->second.tma_bersaglio ||
                                  u->tma_gambe != it->second.tma_gambe || u->tma_soluzione != it->second.tma_soluzione ||
                                  u->tma_osservabile != it->second.tma_osservabile ||
                                  u->armi_rimaste != it->second.armi_rimaste || u->tubi_carichi != it->second.tubi_carichi ||
                                  (u->tma_soluzione &&
                                   (distanza(u->tma_pos, it->second.tma_pos) >= SOGLIA_TMA_M ||
                                    std::fabs(normalizza_180(u->tma_rotta - it->second.tma_rotta)) >= 2.0 ||
                                    std::fabs(u->tma_velocita_kn - it->second.tma_velocita_kn) >= 0.5));
            if (!cambia) continue;
            scrivi_s(g, u->id);
            scrivi_s(g, u->tma_bersaglio);
            scrivi_s(g, u->tma_gambe);
            scrivi_s(g, u->tma_soluzione);
            scrivi_s(g, u->tma_osservabile);
            scrivi_s(g, quantizza(u->tma_pos.x, 1.0));
            scrivi_s(g, quantizza(u->tma_pos.y, 1.0));
            scrivi_s(g, angolo(u->tma_rotta, 1.0));
            scrivi_s(g, quantizza(u->tma_velocita_kn, 0.1));
            scrivi_s(g, u->armi_rimaste);
            scrivi_s(g, u->tubi_carichi);
            ultima_tma_armi_[u->id] = *u;
            ++n;
        }
        for (const int id : uscite) ultima_tma_armi_.erase(id);
        if (n) {
            gruppo('A', n);
            b.insert(b.end(), g.begin(), g.end());
        }
    }
    if (lento) {
        // il quadro del giocatore: tutti i contatti di adesso
        gruppo('C', f.contatti.size());
        for (const StatoContatto& c : f.contatti) {
            const std::vector<std::int64_t> v{c.lettera, quantizza(c.qualita, 1.0), quantizza(c.durata, 1.0),
                                              angolo(c.rilevamento, 0.01), angolo(c.specchio.value_or(0.0), 0.01),
                                              c.specchio.has_value(), c.posizione_nota, quantizza(c.pos_stimata.x, 0.5),
                                              quantizza(c.pos_stimata.y, 0.5), quantizza(c.quota_stimata, 0.1),
                                              stringa(stringhe, c.sensore), stringa(stringhe, c.classe_creduta)};
            scrivi_s(b, c.id);
            auto& prima = ultimo_contatto_[c.id];
            const bool assoluto = chiave || prima.empty();
            for (std::size_t k = 0; k < v.size(); ++k) {
                if (assoluto || k >= 10) scrivi_s(b, v[k]);   // le stringhe sempre assolute
                else if (k == 3 || k == 4) scrivi_s(b, delta_angolo(v[k], prima[k], 0.01));
                else scrivi_s(b, v[k] - prima[k]);
            }
            prima = v;
        }
        // le linee di rilevamento nuove
        std::vector<const Rilevamento*> nuovi;
        for (const Rilevamento& r : f.rilevamenti)
            if (r.tempo > ultima_tma_ + 1e-9) nuovi.push_back(&r);
        if (!nuovi.empty()) {
            gruppo('R', nuovi.size());
            for (const Rilevamento* r : nuovi) {
                scrivi_s(b, quantizza(r->tempo, 0.1));
                scrivi_s(b, quantizza(r->proprio.x, 0.5));
                scrivi_s(b, quantizza(r->proprio.y, 0.5));
                scrivi_s(b, angolo(r->rilevamento, 0.01));
                ultima_tma_ = std::max(ultima_tma_, r->tempo);
            }
        }
        // l'app e la TMA
        const DatiApp& a = f.app;
        gruppo('S', 1);
        for (const std::int64_t x : {std::int64_t{stringa(stringhe, a.modo_sonar)}, std::int64_t{stringa(stringhe, a.sensore)},
                                     angolo(a.fascio, 0.01), std::int64_t{a.filtro}, std::int64_t{a.stazione},
                                     std::int64_t{a.silenzio}, std::int64_t{a.bersaglio}, std::int64_t{a.soluzione},
                                     quantizza(a.proposta.distanza_m, 1.0), angolo(a.proposta.rotta, 0.01),
                                     quantizza(a.proposta.velocita_m_s, 0.01), std::int64_t{f.bersaglio_tma}})
            scrivi_s(b, x);
        prossimo_ = f.t + PASSO_S;
    }
    // gli eventi nuovi del registro, coi testi
    std::vector<const EventoTraccia*> nuovi_eventi;
    for (const EventoTraccia& e : f.eventi)
        if (e.id > ultimo_evento_) nuovi_eventi.push_back(&e);
    if (!nuovi_eventi.empty()) {
        gruppo('E', nuovi_eventi.size());
        for (const EventoTraccia* e : nuovi_eventi) {
            scrivi_s(b, e->id);
            scrivi_s(b, quantizza(e->tempo, 0.1));
            scrivi_s(b, stringa(stringhe, e->tipo));
            scrivi_s(b, stringa(stringhe, e->testo_it));
            scrivi_s(b, stringa(stringhe, e->testo_en));
            scrivi_s(b, e->interesse);
            scrivi_s(b, e->noto);
            scrivi_s(b, e->unita);
            scrivi_s(b, e->altra);
            scrivi_s(b, quantizza(e->distanza_m, 1.0));
            scrivi_s(b, stringa(stringhe, e->classe));
            scrivi_s(b, stringa(stringhe, e->categoria));
            scrivi_s(b, stringa(stringhe, e->classe_arma));
            scrivi_s(b, e->lato);
            scrivi_s(b, e->giusta);
            scrivi_s(b, e->livello);
            scrivi_s(b, quantizza(e->tempo_scoperta, 0.1));
            scrivi_s(b, e->codice);
            scrivi_s(b, quantizza(e->valore, 0.001));
            scrivi_s(b, e->arma);
            scrivi_s(b, stringa(stringhe, e->classe_creduta));
            scrivi_s(b, quantizza(e->pos.x, 1.0));
            scrivi_s(b, quantizza(e->pos.y, 1.0));
            ultimo_evento_ = std::max(ultimo_evento_, e->id);
        }
    }
    // i fatti dell'IA nuovi: tutti (li sceglie RaccoltaTraccia), assoluti
    if (!f.eventi_ia.empty()) {
        gruppo('I', f.eventi_ia.size());
        for (const EventoIa& e : f.eventi_ia) {
            scrivi_s(b, quantizza(e.t, 0.1));
            scrivi_s(b, stringa(stringhe, e.tipo));
            scrivi_s(b, e.unita);
            scrivi_s(b, e.altra);
            scrivi_s(b, e.n);
            scrivi_s(b, e.m);
            scrivi_s(b, quantizza(e.a, 0.1));
            scrivi_s(b, quantizza(e.b, 0.1));
            scrivi_s(b, quantizza(e.c, 0.01));
            scrivi_s(b, quantizza(e.d, 0.01));
            scrivi_s(b, stringa(stringhe, e.s));
        }
    }
    // gli interventi del vetrino nuovi: tutti, assoluti
    if (!f.interventi.empty()) {
        gruppo('X', f.interventi.size());
        for (const RecordIntervento& x : f.interventi) {
            scrivi_s(b, x.id);
            scrivi_s(b, quantizza(x.t, 0.001));
            scrivi_s(b, x.fase);
            scrivi_s(b, stringa(stringhe, x.tipo));
            scrivi_s(b, x.unita);
            scrivi_s(b, x.ha_punto);
            scrivi_s(b, quantizza(x.punto.x, 0.1));
            scrivi_s(b, quantizza(x.punto.y, 0.1));
            scrivi_s(b, quantizza(x.raggio_m, 0.1));
            scrivi_s(b, quantizza(x.durata_s, 0.001));
            scrivi_s(b, stringa(stringhe, x.chiavi));
            scrivi_s(b, stringa(stringhe, x.riga_it));
            scrivi_s(b, stringa(stringhe, x.riga_en));
        }
    }
    if (!f.contatti_unita.empty()) {
        gruppo('K', f.contatti_unita.size());
        for (const ContattoUnita& c : f.contatti_unita) {
            scrivi_s(b, quantizza(c.t, 0.1));
            scrivi_s(b, c.osservatore);
            scrivi_s(b, c.bersaglio);
            scrivi_s(b, quantizza(c.qualita, 1.0));
            scrivi_s(b, angolo(c.rilevamento, 0.1));
            scrivi_s(b, quantizza(c.eta_s, 1.0));
            scrivi_s(b, c.solido);
            scrivi_s(b, c.posizione_nota);
            scrivi_s(b, c.perso);
            scrivi_s(b, quantizza(c.pos_stimata.x, 1.0));
            scrivi_s(b, quantizza(c.pos_stimata.y, 1.0));
            scrivi_s(b, stringa(stringhe, c.sensore));
            scrivi_s(b, stringa(stringhe, c.classe_creduta));
        }
    }
    std::size_t righe_p = 0;   // un quadro vuoto e' una riga «vuoto», perche' anche lo svuotarsi va registrato
    for (const QuadroParte& q : f.quadri_parte) righe_p += std::max<std::size_t>(q.voci.size(), 1);
    if (righe_p) {
        gruppo('P', righe_p);
        for (const QuadroParte& q : f.quadri_parte) {
            const auto riga = [&](const VoceParte* v) {
                const VoceParte vuota;
                const VoceParte& x = v ? *v : vuota;
                scrivi_s(b, quantizza(q.t, 0.1));
                scrivi_s(b, q.lato);
                scrivi_s(b, v ? 0 : 1);
                scrivi_s(b, x.bersaglio);
                scrivi_s(b, x.posizione);
                scrivi_s(b, quantizza(x.pos.x, 1.0));
                scrivi_s(b, quantizza(x.pos.y, 1.0));
                scrivi_s(b, quantizza(x.origine.x, 1.0));
                scrivi_s(b, quantizza(x.origine.y, 1.0));
                scrivi_s(b, angolo(x.rilevamento, 0.1));
                scrivi_s(b, quantizza(x.semiasse_maggiore_m, 1.0));
                scrivi_s(b, quantizza(x.semiasse_minore_m, 1.0));
                scrivi_s(b, angolo(x.orientamento_deg, 1.0));
                scrivi_s(b, quantizza(x.t_dato, 0.1));
                scrivi_s(b, stringa(stringhe, x.sensore));
                scrivi_s(b, x.fonte);
                scrivi_s(b, stringa(stringhe, x.categoria));
                scrivi_s(b, stringa(stringhe, x.classe));
                scrivi_s(b, quantizza(x.qualita, 1.0));
            };
            if (q.voci.empty()) riga(nullptr);
            for (const VoceParte& v : q.voci) riga(&v);
        }
    }
    prossimo_veloci_ = f.t + PASSO_VELOCI_S;
    eventi_visti_ = std::max(eventi_visti_, eventi);
    if (b.empty()) return;   // un fotogramma dei veloci senza veloci
    std::vector<std::uint8_t> testa;
    scrivi_u(testa, static_cast<std::uint64_t>(std::llround(std::max(f.t, 0.0) * 1000.0)));
    scrivi_s(testa, nuovo_evento ? f.evento : -1);
    scrivi_u(testa, chiave ? 1u : 0u);
    corpo_.push_back('F');
    scrivi_u(corpo_, testa.size() + stringhe.size() + b.size());
    corpo_.insert(corpo_.end(), testa.begin(), testa.end());
    corpo_.insert(corpo_.end(), stringhe.begin(), stringhe.end());
    corpo_.insert(corpo_.end(), b.begin(), b.end());
    ++fotogrammi_;
}

std::vector<std::uint8_t> RegistratoreTraccia::dati() const {
    std::ostringstream h;
    h << "versione=" << VERSIONE << "\n";
    for (const auto& [k, v] : meta_) h << k << "=" << v << "\n";
    h << "delta=U C\n";
    h << "record U " << SCHEMA_U << "\n" << "record N " << SCHEMA_N << "\n" << "record C " << SCHEMA_C << "\n"
      << "record R " << SCHEMA_R << "\n" << "record S " << SCHEMA_S << "\n" << "record E " << SCHEMA_E << "\n"
      << "record I " << SCHEMA_I << "\n" << "record V " << SCHEMA_V << "\n" << "record K " << SCHEMA_K << "\n" << "record P " << SCHEMA_P << "\n"
      << "record A " << SCHEMA_A << "\n" << "record X " << SCHEMA_X << "\n";
    const std::string testo = h.str();
    std::vector<std::uint8_t> out(MAGICO, MAGICO + 7);
    scrivi_u(out, testo.size());
    out.insert(out.end(), testo.begin(), testo.end());
    out.insert(out.end(), corpo_.begin(), corpo_.end());
    return out;
}

void RegistratoreTraccia::salva(const std::filesystem::path& p) const {
    if (p.has_parent_path()) std::filesystem::create_directories(p.parent_path());
    const auto d = dati();
    const auto tmp = p.string() + ".tmp";
    {
        std::ofstream o(tmp, std::ios::binary);
        if (!o) throw std::runtime_error("impossibile scrivere " + tmp);
        o.write(reinterpret_cast<const char*>(d.data()), static_cast<std::streamsize>(d.size()));
    }
    std::filesystem::rename(tmp, p);
}

// --- lettura ---

namespace {

struct Extra {   // il record V in vigore per un'unita'
    Vec2 errore;
    double sigma = 0.0;
    int allerta = -1, emissioni = 0;
    double sigma_min = 0.0, orientamento = 0.0;
};
struct TmaArmi {   // il record A in vigore per un'unita'
    int tma_bersaglio = -1, tma_gambe = 0;
    bool tma_soluzione = false, tma_osservabile = false;
    Vec2 tma_pos;
    double tma_rotta = 0.0, tma_velocita = 0.0;
    int armi = -1, tubi = -1;
};
void applica_a(StatoUnita& u, const TmaArmi& x) {
    u.tma_bersaglio = x.tma_bersaglio;
    u.tma_gambe = x.tma_gambe;
    u.tma_soluzione = x.tma_soluzione;
    u.tma_osservabile = x.tma_osservabile;
    u.tma_pos = x.tma_pos;
    u.tma_rotta = x.tma_rotta;
    u.tma_velocita_kn = x.tma_velocita;
    u.armi_rimaste = x.armi;
    u.tubi_carichi = x.tubi;
}
void applica(StatoUnita& u, const Extra& x) {
    u.pos_stimata = {u.pos.x + x.errore.x, u.pos.y + x.errore.y};
    u.sigma_nav_m = x.sigma;
    u.sigma_min_m = x.sigma_min;
    u.orientamento_deg = x.orientamento;
    u.allerta = x.allerta;
    u.emissioni = x.emissioni;
}

// i valori di un record letti per nome (quelli che mancano: 0, o la stringa vuota)
struct Record {
    std::map<std::string, std::int64_t> v;
    std::map<std::string, std::string> s;
    double q(const std::string& k, const std::map<std::string, Campo>& schema) const {
        const auto it = v.find(k);
        if (it == v.end()) return 0.0;
        const auto c = schema.find(k);
        return static_cast<double>(it->second) * (c == schema.end() ? 1.0 : c->second.passo);
    }
    std::int64_t i(const std::string& k, std::int64_t d = 0) const {
        const auto it = v.find(k);
        return it == v.end() ? d : it->second;
    }
};

}  // namespace

Traccia leggi_traccia(const std::vector<std::uint8_t>& dati) {
    if (dati.size() < 8 || !std::equal(MAGICO, MAGICO + 7, dati.begin())) throw std::runtime_error("non e' una traccia del replay");
    Lettore l{dati.data() + 7, dati.data() + dati.size()};
    const std::uint64_t lunghezza = l.u();
    if (lunghezza > static_cast<std::uint64_t>(l.fine - l.p)) throw std::runtime_error("intestazione troncata");
    const std::string testo(reinterpret_cast<const char*>(l.p), static_cast<std::size_t>(lunghezza));
    l.p += lunghezza;
    Traccia t;
    std::map<char, std::vector<Campo>> schemi;
    std::string delta = "U C";
    std::istringstream in(testo);
    for (std::string riga; std::getline(in, riga);) {
        if (riga.rfind("record ", 0) == 0 && riga.size() > 8) {
            schemi[riga[7]] = schema(riga.substr(8));
            continue;
        }
        const auto p = riga.find('=');
        if (p == std::string::npos) continue;
        const std::string k = riga.substr(0, p), v = riga.substr(p + 1);
        if (k == "versione") t.versione = std::atoi(v.c_str());
        else if (k == "missione") t.missione = v;
        else if (k == "seme") t.seme = std::strtoull(v.c_str(), nullptr, 10);
        else if (k == "ripresa_da") t.ripresa_da = std::atof(v.c_str());
        else if (k == "delta") delta = v;
        else t.metadati[k] = v;
    }
    std::map<char, std::map<std::string, Campo>> per_nome;
    for (const auto& [tag, campi] : schemi)
        for (const auto& c : campi) per_nome[tag][c.nome] = c;
    std::vector<std::string> stringhe;
    std::map<std::pair<char, std::int64_t>, std::vector<std::int64_t>> prima;   // delta: per tag e chiave
    std::map<int, StatoUnita> anagrafica;
    std::map<int, Extra> extra;
    std::map<int, TmaArmi> tma_armi;
    bool serve_chiave = false;   // dopo un fotogramma rovinato: si riparte dal prossimo fotogramma chiave
    while (l.p < l.fine) {
        if (l.byte() != 'F') {
            ++t.fotogrammi_saltati;
            break;   // il formato si e' perso: il resto non si legge
        }
        const std::uint64_t n = l.u();
        if (n > static_cast<std::uint64_t>(l.fine - l.p)) {
            ++t.fotogrammi_saltati;
            break;
        }
        Lettore f{l.p, l.p + n};
        l.p += n;
        try {
            const double tempo = static_cast<double>(f.u()) / 1000.0;
            const std::int64_t evento = f.s();
            const bool chiave = (f.u() & 1u) != 0;
            if (serve_chiave && !chiave) {
                ++t.fotogrammi_saltati;
                continue;
            }
            serve_chiave = false;
            if (chiave) prima.clear();
            t.durata = std::max(t.durata, tempo);
            if (evento >= 0) t.eventi.emplace_back(tempo, static_cast<int>(evento));
            Traccia::Quadro quadro;
            quadro.t = tempo;
            bool con_quadro = false;
            while (f.p < f.fine) {
                const char tag = static_cast<char>(f.byte());
                if (tag == 'T') {
                    const std::uint64_t k = f.u(), lun = f.u();
                    if (lun > static_cast<std::uint64_t>(f.fine - f.p)) throw std::runtime_error("stringa troncata");
                    if (stringhe.size() <= k) stringhe.resize(static_cast<std::size_t>(k) + 1);
                    stringhe[static_cast<std::size_t>(k)].assign(reinterpret_cast<const char*>(f.p), static_cast<std::size_t>(lun));
                    f.p += lun;
                    continue;
                }
                const auto sc = schemi.find(tag);
                if (sc == schemi.end()) throw std::runtime_error("record senza schema");
                const bool a_delta = delta.find(tag) != std::string::npos;
                const std::uint64_t quanti = f.u();
                for (std::uint64_t r = 0; r < quanti; ++r) {
                    Record rec;
                    std::vector<std::int64_t> grezzi;
                    std::int64_t chiave_rec = 0;
                    for (const Campo& c : sc->second) grezzi.push_back(c.tipo == 'k' ? (chiave_rec = f.s()) : f.s());
                    if (a_delta) {
                        auto& p = prima[{tag, chiave_rec}];
                        if (!p.empty() && p.size() == grezzi.size())
                            for (std::size_t k = 0; k < grezzi.size(); ++k) {
                                const Campo& c = sc->second[k];
                                if (c.tipo == 'k' || c.tipo == 's') continue;
                                grezzi[k] += p[k];
                                if (c.tipo == 'a') {
                                    const std::int64_t giro = quantizza(360.0, c.passo);
                                    grezzi[k] = ((grezzi[k] % giro) + giro) % giro;
                                }
                            }
                        p = grezzi;
                    }
                    for (std::size_t k = 0; k < grezzi.size(); ++k) {
                        const Campo& c = sc->second[k];
                        if (c.tipo == 's') {
                            const auto i = static_cast<std::size_t>(std::max<std::int64_t>(0, grezzi[k]));
                            rec.s[c.nome] = i < stringhe.size() ? stringhe[i] : std::string{};
                        } else {
                            rec.v[c.nome] = grezzi[k];
                        }
                    }
                    const auto& pn = per_nome[tag];
                    if (tag == 'N') {
                        StatoUnita& a = anagrafica[static_cast<int>(rec.i("id"))];
                        a.id = static_cast<int>(rec.i("id"));
                        a.classe = rec.s["classe"];
                        a.categoria = rec.s["categoria"];
                        a.flag = static_cast<int>(rec.i("flag"));
                    } else if (tag == 'U') {
                        const int id = static_cast<int>(rec.i("id"));
                        StatoUnita u = anagrafica[id];
                        u.id = id;
                        u.pos = {rec.q("x", pn), rec.q("y", pn)};
                        u.quota = rec.q("quota", pn);
                        u.rotta = rec.q("rotta", pn);
                        u.velocita_kn = rec.q("velocita", pn);
                        const auto st = rec.i("stato");
                        u.distrutta = st & DISTRUTTA;
                        u.persa = st & PERSA;
                        u.incagliata = st & INCAGLIATA;
                        u.sul_fondo = st & SUL_FONDO;
                        u.rimossa = st & RIMOSSA;
                        u.alberi = static_cast<int>(rec.i("alberi"));
                        u.lanciatore = static_cast<int>(rec.i("lanciatore", -1));
                        u.collegata = static_cast<int>(rec.i("collegata", -1));
                        applica(u, extra[id]);   // l'ultimo record V dell'unita' (v1: nessuno, la stima e' la vera)
                        applica_a(u, tma_armi[id]);
                        if (u.rimossa) {   // l'id si puo' riusare: le differenze ripartono, anche per il contatto
                            prima.erase({'U', id});
                            prima.erase({'C', id});
                            extra.erase(id);
                            tma_armi.erase(id);
                        }
                        t.unita[id].emplace_back(tempo, std::move(u));
                    } else if (tag == 'C') {
                        StatoContatto c;
                        c.id = static_cast<int>(rec.i("id"));
                        c.lettera = static_cast<char>(rec.i("lettera"));
                        c.qualita = rec.q("qualita", pn);
                        c.durata = rec.q("durata", pn);
                        c.rilevamento = rec.q("rilevamento", pn);
                        if (rec.i("ha_specchio")) c.specchio = rec.q("specchio", pn);
                        c.posizione_nota = rec.i("nota") != 0;
                        c.pos_stimata = {rec.q("x", pn), rec.q("y", pn)};
                        c.quota_stimata = rec.q("quota", pn);
                        c.sensore = rec.s["sensore"];
                        c.classe_creduta = rec.s["classe"];
                        quadro.contatti.push_back(std::move(c));
                        con_quadro = true;
                    } else if (tag == 'R') {
                        t.rilevamenti.push_back({rec.q("tempo", pn), {rec.q("x", pn), rec.q("y", pn)}, rec.q("rilevamento", pn)});
                    } else if (tag == 'E') {
                        EventoTraccia e;
                        e.id = static_cast<int>(rec.i("id", -1));
                        e.tempo = rec.q("tempo", pn);
                        e.tipo = rec.s["tipo"];
                        e.testo_it = rec.s["testo_it"];
                        e.testo_en = rec.s["testo_en"];
                        e.interesse = static_cast<int>(rec.i("interesse"));
                        e.noto = rec.i("noto", 1) != 0;
                        e.unita = static_cast<int>(rec.i("unita", -1));
                        e.altra = static_cast<int>(rec.i("altra", -1));
                        e.distanza_m = rec.q("distanza", pn);
                        e.classe = rec.s["classe"];
                        e.categoria = rec.s["categoria"];
                        e.classe_arma = rec.s["arma"];
                        e.lato = static_cast<int>(rec.i("lato"));
                        e.giusta = rec.i("giusta", 1) != 0;
                        e.livello = static_cast<int>(rec.i("livello"));
                        e.tempo_scoperta = rec.v.count("scoperta") ? rec.q("scoperta", pn) : -1.0;
                        e.codice = static_cast<int>(rec.i("codice"));
                        e.valore = rec.q("valore", pn);
                        e.arma = static_cast<int>(rec.i("id_arma", -1));
                        e.classe_creduta = rec.s["creduta"];
                        e.pos = {rec.q("x", pn), rec.q("y", pn)};
                        t.registro.push_back(std::move(e));
                    } else if (tag == 'V') {   // dopo i record U del fotogramma: vale anche per il campione di adesso
                        const int id = static_cast<int>(rec.i("id"));
                        Extra& x = extra[id];
                        x = {{rec.q("ex", pn), rec.q("ey", pn)}, rec.q("sigma", pn), static_cast<int>(rec.i("allerta", -1)),
                             static_cast<int>(rec.i("emissioni")), rec.q("sigma_min", pn), rec.q("orientamento", pn)};
                        if (auto it = t.unita.find(id); it != t.unita.end() && !it->second.empty() &&
                                                         it->second.back().first == tempo)
                            applica(it->second.back().second, x);
                    } else if (tag == 'A') {   // dopo i record U del fotogramma, come V
                        const int id = static_cast<int>(rec.i("id"));
                        TmaArmi& x = tma_armi[id];
                        x = {static_cast<int>(rec.i("tma_b", -1)), static_cast<int>(rec.i("tma_g")), rec.i("tma_s") != 0,
                             rec.i("tma_o") != 0, {rec.q("tma_x", pn), rec.q("tma_y", pn)}, rec.q("tma_r", pn),
                             rec.q("tma_v", pn), static_cast<int>(rec.i("armi", -1)), static_cast<int>(rec.i("tubi", -1))};
                        if (auto it = t.unita.find(id); it != t.unita.end() && !it->second.empty() &&
                                                         it->second.back().first == tempo)
                            applica_a(it->second.back().second, x);
                    } else if (tag == 'P') {   // le righe di un quadro hanno lo stesso tempo e lato, una dopo l'altra
                        const double tq = rec.q("tempo", pn);
                        const int lato = static_cast<int>(rec.i("lato"));
                        if (t.quadri_parte.empty() || t.quadri_parte.back().t != tq || t.quadri_parte.back().lato != lato)
                            t.quadri_parte.push_back({tq, lato, {}});
                        if (!rec.i("vuoto")) {
                            VoceParte v;
                            v.bersaglio = static_cast<int>(rec.i("bersaglio", -1));
                            v.posizione = rec.i("posizione") != 0;
                            v.pos = {rec.q("x", pn), rec.q("y", pn)};
                            v.origine = {rec.q("ox", pn), rec.q("oy", pn)};
                            v.rilevamento = rec.q("rilevamento", pn);
                            v.semiasse_maggiore_m = rec.q("mag", pn);
                            v.semiasse_minore_m = rec.q("min", pn);
                            v.orientamento_deg = rec.q("orient", pn);
                            v.t_dato = rec.q("dato", pn);
                            v.sensore = rec.s["sensore"];
                            v.fonte = static_cast<int>(rec.i("fonte", -1));
                            v.categoria = rec.s["categoria"];
                            v.classe = rec.s["classe"];
                            v.qualita = rec.q("qualita", pn);
                            t.quadri_parte.back().voci.push_back(std::move(v));
                        }
                    } else if (tag == 'K') {
                        ContattoUnita c;
                        c.t = rec.q("tempo", pn);
                        c.osservatore = static_cast<int>(rec.i("osservatore", -1));
                        c.bersaglio = static_cast<int>(rec.i("bersaglio", -1));
                        c.qualita = rec.q("qualita", pn);
                        c.rilevamento = rec.q("rilevamento", pn);
                        c.eta_s = rec.q("eta", pn);
                        c.solido = rec.i("solido") != 0;
                        c.posizione_nota = rec.i("nota") != 0;
                        c.perso = rec.i("perso") != 0;
                        c.pos_stimata = {rec.q("x", pn), rec.q("y", pn)};
                        c.sensore = rec.s["sensore"];
                        c.classe_creduta = rec.s["classe"];
                        t.contatti_unita.push_back(std::move(c));
                    } else if (tag == 'X') {
                        RecordIntervento x;
                        x.id = static_cast<int>(rec.i("id", -1));
                        x.t = rec.q("tempo", pn);
                        x.fase = static_cast<int>(rec.i("fase"));
                        x.tipo = rec.s["tipo"];
                        x.unita = static_cast<int>(rec.i("unita", -1));
                        x.ha_punto = rec.i("punto") != 0;
                        x.punto = {rec.q("x", pn), rec.q("y", pn)};
                        x.raggio_m = rec.q("raggio", pn);
                        x.durata_s = rec.q("durata", pn);
                        x.chiavi = rec.s["chiavi"];
                        x.riga_it = rec.s["riga_it"];
                        x.riga_en = rec.s["riga_en"];
                        t.interventi.push_back(std::move(x));
                    } else if (tag == 'I') {
                        EventoIa e;
                        e.t = rec.q("tempo", pn);
                        e.tipo = rec.s["tipo"];
                        e.unita = static_cast<int>(rec.i("unita", -1));
                        e.altra = static_cast<int>(rec.i("altra", -1));
                        e.n = static_cast<int>(rec.i("n"));
                        e.m = static_cast<int>(rec.i("m"));
                        e.a = rec.q("a", pn);
                        e.b = rec.q("b", pn);
                        e.c = rec.q("c", pn);
                        e.d = rec.q("d", pn);
                        e.s = rec.s["s"];
                        t.eventi_ia.push_back(std::move(e));
                    } else if (tag == 'S') {
                        DatiApp& a = quadro.app;
                        a.modo_sonar = rec.s["modo"];
                        a.sensore = rec.s["sensore"];
                        a.fascio = rec.q("fascio", pn);
                        a.filtro = static_cast<int>(rec.i("filtro"));
                        a.stazione = static_cast<int>(rec.i("stazione"));
                        a.silenzio = rec.i("silenzio") != 0;
                        a.bersaglio = static_cast<int>(rec.i("bersaglio", -1));
                        a.soluzione = rec.i("soluzione") != 0;
                        a.proposta = {rec.q("distanza", pn), rec.q("rotta", pn), rec.q("velocita", pn)};
                        quadro.bersaglio_tma = static_cast<int>(rec.i("bersaglio_tma", -1));
                        con_quadro = true;
                    }
                    // un tag sconosciuto con lo schema: letto e lasciato (motore piu' vecchio del file)
                }
            }
            if (con_quadro) t.quadri.push_back(std::move(quadro));
        } catch (const std::exception&) {
            ++t.fotogrammi_saltati;
            serve_chiave = true;
        }
    }
    // in un fotogramma ci sono fatti di liste diverse: in ordine di tempo
    std::stable_sort(t.eventi_ia.begin(), t.eventi_ia.end(), [](const EventoIa& a, const EventoIa& b) { return a.t < b.t; });
    std::stable_sort(t.contatti_unita.begin(), t.contatti_unita.end(),
                     [](const ContattoUnita& a, const ContattoUnita& b) { return a.t < b.t; });
    std::stable_sort(t.interventi.begin(), t.interventi.end(),
                     [](const RecordIntervento& a, const RecordIntervento& b) { return a.t < b.t; });
    return t;
}

const QuadroParte* Traccia::quadro_parte(int lato, double t) const {
    const QuadroParte* q = nullptr;
    for (const QuadroParte& x : quadri_parte) {
        if (x.t > t + 1e-9) break;
        if (x.lato == lato) q = &x;
    }
    return q;
}

std::vector<ContattoUnita> Traccia::quadro_unita(int osservatore, double t) const {
    std::map<int, const ContattoUnita*> ultima;
    for (const ContattoUnita& c : contatti_unita) {
        if (c.t > t + 1e-9) break;
        if (c.osservatore == osservatore) ultima[c.bersaglio] = &c;
    }
    std::vector<ContattoUnita> out;
    for (const auto& [b, c] : ultima)
        if (!c->perso) {
            ContattoUnita x = *c;
            x.eta_s += t - c->t;
            out.push_back(std::move(x));
        }
    return out;
}

void aggiungi_fotogramma(Traccia& t, const Fotogramma& f) {
    t.durata = std::max(t.durata, f.t);
    std::set<int> presenti;
    for (const StatoUnita& u : f.unita) {
        presenti.insert(u.id);
        t.unita[u.id].emplace_back(f.t, u);
    }
    for (auto& [id, serie] : t.unita)   // uscite dal gioco: l'ultimo stato con la bandierina, come nel file
        if (!presenti.count(id) && !serie.empty() && !serie.back().second.rimossa) {
            StatoUnita u = serie.back().second;
            u.rimossa = true;
            serie.emplace_back(f.t, std::move(u));
        }
    double ultima_tma = -1e18;
    for (const Rilevamento& r : t.rilevamenti) ultima_tma = std::max(ultima_tma, r.tempo);
    for (const Rilevamento& r : f.rilevamenti)
        if (r.tempo > ultima_tma + 1e-9) t.rilevamenti.push_back(r);
    Traccia::Quadro q;
    q.t = f.t;
    q.contatti = f.contatti;
    q.bersaglio_tma = f.bersaglio_tma;
    q.app = f.app;
    t.quadri.push_back(std::move(q));
    const int ultimo = t.registro.empty() ? -1 : t.registro.back().id;
    bool nuovo = false;
    for (const EventoTraccia& e : f.eventi)
        if (e.id > ultimo) {
            t.registro.push_back(e);
            nuovo = true;
        }
    if (nuovo && f.evento >= 0) t.eventi.emplace_back(f.t, f.evento);
    for (const QuadroParte& qp : f.quadri_parte) t.quadri_parte.push_back(qp);
    for (const ContattoUnita& c : f.contatti_unita) {
        const auto dove = std::upper_bound(t.contatti_unita.begin(), t.contatti_unita.end(), c.t,
                                           [](double x, const ContattoUnita& a) { return x < a.t; });
        t.contatti_unita.insert(dove, c);
    }
    for (const RecordIntervento& x : f.interventi) {
        const auto dove = std::upper_bound(t.interventi.begin(), t.interventi.end(), x.t,
                                           [](double v, const RecordIntervento& a) { return v < a.t; });
        t.interventi.insert(dove, x);
    }
    for (const EventoIa& e : f.eventi_ia) {   // in ordine di tempo anche se il fotogramma li mescola
        const auto dove = std::upper_bound(t.eventi_ia.begin(), t.eventi_ia.end(), e.t,
                                           [](double x, const EventoIa& a) { return x < a.t; });
        t.eventi_ia.insert(dove, e);
    }
}

// «nome=valore;...»: ';', '=' e la barra rovescia, nei nomi e nei valori, preceduti dalla barra rovescia
std::string codifica_chiavi(const std::map<std::string, std::string>& m) {
    const auto protetto = [](const std::string& s) {
        std::string o;
        for (const char c : s) {
            if (c == ';' || c == '=' || c == '\\') o += '\\';
            o += c;
        }
        return o;
    };
    std::string out;
    for (const auto& [k, v] : m) {
        if (!out.empty()) out += ';';
        out += protetto(k) + '=' + protetto(v);
    }
    return out;
}

std::map<std::string, std::string> leggi_chiavi(const std::string& s) {
    std::map<std::string, std::string> out;
    std::string k, v, *dove = &k;
    bool nome = true;
    for (std::size_t i = 0; i < s.size(); ++i) {
        const char c = s[i];
        if (c == '\\' && i + 1 < s.size()) {
            *dove += s[++i];
        } else if (c == '=' && nome) {
            nome = false;
            dove = &v;
        } else if (c == ';') {
            if (!k.empty() || !v.empty()) out[k] = v;
            k.clear();
            v.clear();
            nome = true;
            dove = &k;
        } else {
            *dove += c;
        }
    }
    if (!k.empty() || !v.empty()) out[k] = v;
    return out;
}

std::size_t Traccia::eventi_ia_fino(double t) const {
    return static_cast<std::size_t>(std::upper_bound(eventi_ia.begin(), eventi_ia.end(), t + 1e-9,
                                                     [](double x, const EventoIa& e) { return x < e.t; }) -
                                    eventi_ia.begin());
}

std::vector<EventoTraccia> segnalibri(const Traccia& t, int interesse_min, bool noti) {
    std::vector<EventoTraccia> out;
    if (t.registro.empty()) {   // traccia vecchia: il tempo e l'id
        for (const auto& [tempo, id] : t.eventi) {
            EventoTraccia e;
            e.id = id;
            e.tempo = tempo;
            if (interesse_min <= 0) out.push_back(std::move(e));
        }
        return out;
    }
    for (const EventoTraccia& e : t.registro)
        if (e.interesse >= interesse_min && (!noti || e.noto)) out.push_back(e);
    std::stable_sort(out.begin(), out.end(), [](const EventoTraccia& a, const EventoTraccia& b) { return a.tempo < b.tempo; });
    return out;
}

Traccia carica_traccia(const std::filesystem::path& p) {
    std::ifstream in(p, std::ios::binary);
    if (!in) throw std::runtime_error("impossibile leggere " + p.string());
    const std::vector<std::uint8_t> d((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    return leggi_traccia(d);
}

IstanteTraccia Traccia::istante(double t) const {
    IstanteTraccia out;
    out.t = t;
    for (const auto& [id, serie] : unita) {
        const auto dopo = std::upper_bound(serie.begin(), serie.end(), t, [](double x, const auto& c) { return x < c.first; });
        if (dopo == serie.begin()) continue;   // non ancora in gioco
        const auto& [ta, a] = *std::prev(dopo);
        if (a.rimossa) continue;                // uscita dal gioco
        StatoUnita u = a;
        if (dopo != serie.end() && dopo->first - ta <= SENZA_INTERPOLARE_S && dopo->first > ta) {
            const double k = (t - ta) / (dopo->first - ta);
            const StatoUnita& b = dopo->second;
            u.pos = {a.pos.x + (b.pos.x - a.pos.x) * k, a.pos.y + (b.pos.y - a.pos.y) * k};
            u.quota = a.quota + (b.quota - a.quota) * k;
            u.velocita_kn = a.velocita_kn + (b.velocita_kn - a.velocita_kn) * k;
            u.rotta = normalizza_360(a.rotta + normalizza_180(b.rotta - a.rotta) * k);
        }
        out.unita.push_back(std::move(u));
    }
    const auto q = std::upper_bound(quadri.begin(), quadri.end(), t, [](double x, const Quadro& c) { return x < c.t; });
    if (q != quadri.begin()) {
        const Quadro& x = *std::prev(q);
        out.contatti = x.contatti;
        out.bersaglio_tma = x.bersaglio_tma;
        out.app = x.app;
    }
    for (const Rilevamento& r : rilevamenti)
        if (r.tempo <= t + 1e-9) out.rilevamenti.push_back(r);
    return out;
}

// --- cartelle ---

std::filesystem::path cartella_replay(const std::string& comandante) {
    return percorso_albo().parent_path() / "replay" / (comandante.empty() ? std::string("liberi") : comandante);
}

std::filesystem::path percorso_replay(const std::string& comandante, const std::string& missione, const std::string& data) {
    std::string nome = missione + "_" + data;
    for (char& c : nome)
        if (!(std::isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '_')) c = '_';
    return cartella_replay(comandante) / (nome + ".m688tr");
}

int pulisci_replay_liberi(int tenere) {
    std::vector<std::pair<std::filesystem::file_time_type, std::filesystem::path>> file;
    std::error_code ec;
    for (const auto& e : std::filesystem::directory_iterator(cartella_replay(), ec))
        if (e.is_regular_file() && e.path().extension() == ".m688tr") file.emplace_back(e.last_write_time(), e.path());
    std::sort(file.begin(), file.end(), [](const auto& a, const auto& b) { return a.first > b.first; });
    int tolti = 0;
    for (std::size_t k = static_cast<std::size_t>(std::max(0, tenere)); k < file.size(); ++k)
        if (std::filesystem::remove(file[k].second, ec)) ++tolti;
    return tolti;
}

}  // namespace m688
