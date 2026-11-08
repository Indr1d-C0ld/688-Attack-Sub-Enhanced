#include <algorithm>
#include <cmath>
#include <filesystem>
#include <string>

#include <toml.hpp>

#include "missioni/collisioni_missione.hpp"
#include "missioni/periscopi_missione.hpp"
#include "nucleo/acustica_urto.hpp"
#include "nucleo/battello.hpp"
#include "nucleo/evasione.hpp"

// Collisioni fra unita' nella missione dell'Enhanced (docs/ricerca/16_collisioni.md, citato «16»): la rilevazione
// geometrica (P1), il Crazy Ivan dei sottomarini sovietici (P3, dossier 03 §C4) e le quasi collisioni del giocatore
// (P6). I danni sono del kit (nucleo/collisioni.hpp: valuta_urto; DanniSottomarini e DanniMissione li applicano
// leggendo urti()). Solo con la regola di simulazione: nel classico le unita' si attraversano come nell'originale.

namespace m688 {

namespace {

constexpr double RIPETI_URTO_S = 2.0;           // a contatto continuo, un altro Urto dopo 2 s (contratto del kit)
constexpr double SEPARA_M = 10.0, SEPARA_S = 120.0, SALTO_SEPARA_M = 40.0;   // dopo l'urto: margine, manovra e salto di quota (STIMA)
constexpr double SOVRASTRUTTURE_M = 8.0;        // le navi sopra il galleggiamento (STIMA)
constexpr double VELA_LUNGHEZZA = 0.15, VELA_LARGHEZZA_M = 3.5, VELA_AVANTI = 0.2;   // senza dati della classe (16 §4.1)
constexpr double QUOTA_PERISCOPICA_CLASSE_M = 18.3;   // la chiglia a quota periscopica senza i dati dei periscopi (688)
constexpr double QUASI_IMMERSI_M = 200.0, QUASI_PERISCOPICA_M = 500.0;   // 16 §4.6
constexpr double USCITA_QUASI = 1.2;            // il passaggio finisce oltre 1,2 volte la soglia
constexpr double VICINI_M = 3000.0;             // oltre, una coppia non si guarda nemmeno
constexpr int URTI_DA = 940000, URTI_PASSO = 25, URTI_QUANTI = 400;   // acustica_urto.hpp: al piu' 22 id per urto
constexpr double CONTATTO_URTO = 1000.0;        // dopo l'urto entrambi sanno (16 §4.5): traccia solida
// Crazy Ivan (03 §C4, parametri [B])
constexpr double IVAN_MIN_S = 1800.0, IVAN_MAX_S = 7200.0;   // ogni 30-120 minuti
constexpr double IVAN_DOPO_ACCOSTATA_MIN_S = 300.0, IVAN_DOPO_ACCOSTATA_MAX_S = 900.0;
constexpr double IVAN_ACCOSTATA_GRADI = 30.0;   // un cambio di rotta che lo rende piu' probabile
constexpr double IVAN_VIRATA_MIN = 120.0, IVAN_VIRATA_MAX = 180.0, IVAN_PIU_KN = 2.0;
constexpr double IVAN_P_QUOTA = 0.3, IVAN_QUOTA_MIN_M = 20.0, IVAN_QUOTA_MAX_M = 40.0;
constexpr double IVAN_ASCOLTO_MIN_S = 120.0, IVAN_ASCOLTO_MAX_S = 300.0, IVAN_ASCOLTO_KN = 4.0;
constexpr double IVAN_VELOCITA_MIN_KN = 3.0, IVAN_QUOTA_MINIMA_M = 30.0;
// immersione d'emergenza (16 §4.4): almeno 150 ft, la vela 10 m sotto la chiglia delle navi entro 5 km
constexpr double EMERGENZA_MIN_M = 45.72, EMERGENZA_MARGINE_M = 10.0, EMERGENZA_NAVI_M = 5000.0, EMERGENZA_FRANCO_M = 15.0;

struct Scatola {                  // orientata sulla rotta nel piano, verticale fra z0 e z1 (positive in giu')
    Vec2 c, a;                    // centro, asse lungo la rotta
    double hl = 0.0, hw = 0.0, z0 = 0.0, z1 = 0.0;
};

struct Forma {
    int id = -1;
    bool sottomarino = false, in_superficie = false;
    Scatola scafo, vela;
    bool con_vela = false;
};

double scalare(Vec2 a, Vec2 b) { return a.x * b.x + a.y * b.y; }
double raggio(const Scatola& s, Vec2 asse) {
    const Vec2 b{s.a.y, -s.a.x};
    return s.hl * std::fabs(scalare(s.a, asse)) + s.hw * std::fabs(scalare(b, asse));
}

struct Sovrapposizione {
    bool tocca = false;
    double penetrazione = 0.0;    // la minima fra gli assi (orizzontali) e la verticale
    double separazione = 1e18;    // se non si toccano: la distanza lungo il miglior asse separatore (con la quota)
    Vec2 normale;                 // orizzontale, da a verso b (se verticale, `verticale`)
    bool verticale = false;
    Vec2 punto;
    double quota = 0.0;
};

// SAT fra due scatole orientate nel piano, con la sovrapposizione delle quote
Sovrapposizione sovrapponi(const Scatola& p, const Scatola& q) {
    Sovrapposizione r;
    const Vec2 d{q.c.x - p.c.x, q.c.y - p.c.y};
    const Vec2 assi[4] = {p.a, {p.a.y, -p.a.x}, q.a, {q.a.y, -q.a.x}};
    double pen_min = 1e18, sep_max = -1e18;
    Vec2 asse_min{1, 0};
    for (const Vec2& ax : assi) {
        const double pen = raggio(p, ax) + raggio(q, ax) - std::fabs(scalare(d, ax));
        sep_max = std::max(sep_max, -pen);
        if (pen < pen_min) { pen_min = pen; asse_min = ax; }
    }
    const double oz = std::min(p.z1, q.z1) - std::max(p.z0, q.z0);
    if (pen_min <= 0.0 || oz <= 0.0) {
        r.separazione = std::hypot(std::max(0.0, sep_max), std::max(0.0, -oz));
        return r;
    }
    r.tocca = true;
    r.quota = 0.5 * (std::max(p.z0, q.z0) + std::min(p.z1, q.z1));
    if (oz < pen_min) {   // si toccano di piu' in quota che di fianco: urto verticale
        r.verticale = true;
        r.penetrazione = oz;
    } else {
        r.penetrazione = pen_min;
        r.normale = scalare(d, asse_min) >= 0.0 ? asse_min : Vec2{-asse_min.x, -asse_min.y};
    }
    // il punto: a meta' delle sovrapposizioni delle proiezioni, lungo la normale (o l'asse di q) e di traverso
    const Vec2 n = r.verticale ? q.a : r.normale, t{n.y, -n.x};
    const auto meta = [&](Vec2 ax) {
        const double cp = scalare(p.c, ax), cq = scalare(q.c, ax), rp = raggio(p, ax), rq = raggio(q, ax);
        return 0.5 * (std::max(cp - rp, cq - rq) + std::min(cp + rp, cq + rq));
    };
    const double mn = meta(n), mt = meta(t);
    r.punto = {n.x * mn + t.x * mt, n.y * mn + t.y * mt};
    return r;
}

// la schiera trainata delle navi (dossier 04 §2.2 e §3, SQR-19 TACTAS): cavo, schiera, quota massima e velocita' v0
// dell'angolo critico (sin phi = v0 / v: 380 m di quota a 10 nodi col cavo di 1.700 m). Il ripiego senza il modello della
// schiera (profilo_cavo): con l'acustica il cavo e' la forma vera di nucleo/schiera
struct DatiCavo { double cavo_m, schiera_m, quota_max_m, v0_kn; };
std::optional<DatiCavo> cavo_di(const std::string& classe) {
    if (classe == "perry" || classe == "spruance") return DatiCavo{1700.0, 242.0, 360.0, 2.2};
    return std::nullopt;
}
constexpr double SCIA_OGNI_S = 5.0, PASSO_CAVO_M = 5.0, IMPIGLIO_OLTRE_M = 1.0, TOCCO_M = 0.5;   // l'elica: entro D/2 + 1 m dal mozzo
constexpr double PERDITA_ELICA = 0.85, VELOCITA_IMPIGLIATO_KN = 2.0;   // K-324: immobilizzato (STIMA)

}  // namespace

void Missione::collisioni_enhanced(const std::map<std::string, ClasseNave>& classi, const std::string& cartella_dati) {
    if (collisioni_ || collisioni_spente_) return;
    auto s = std::make_unique<StatoCollisioniMissione>();
    s->classi = classi;
    s->cartella_dati = cartella_dati;
    for (const auto& [id, c] : classi) {   // i dislocamenti degli scafi del kit (scafi/<classe>.toml)
        if (c.scafo.empty()) continue;
        try {
            const toml::table t = toml::parse_file((std::filesystem::path(cartella_dati) / c.scafo).string());
            const auto cerca = [&](const char* k) -> double {
                if (const auto v = t[k].value<double>()) return *v;
                if (const auto* sub = t["sottomarino"].as_table())
                    if (const auto v = (*sub)[k].value<double>()) return *v;
                return 0.0;
            };
            const double pub = cerca("dislocamento_pubblicato_t"), em = cerca("dislocamento_emersione_t"), im = cerca("dislocamento_immersione_t");
            s->masse[id] = {em > 0.0 ? em : pub, im > 0.0 ? im : pub};
        } catch (const std::exception&) {}
    }
    s->caso = Casuale(seme_ ^ 0xC0111510C0111510ull);
    collisioni_ = std::move(s);
}

double Missione::quota_immersione_emergenza() const {
    const UnitaClassica* g = giocatore();
    if (!g) return EMERGENZA_MIN_M;
    double diametro = 10.0, vela = 6.0;   // senza le classi: un 688
    if (collisioni_)
        if (const auto c = collisioni_->classi.find(g->classe); c != collisioni_->classi.end()) {
            if (c->second.larghezza_m > 0.0) diametro = c->second.larghezza_m;
            if (c->second.altezza_vela_m > 0.0) vela = c->second.altezza_vela_m;
        }
    double q = EMERGENZA_MIN_M;
    for (const auto& [id, u] : unita_)
        if (u.categoria == "nave" && !u.distrutta && !u.persa && distanza(u.pos, g->pos) <= EMERGENZA_NAVI_M)
            q = std::max(q, pescaggio(u) + EMERGENZA_MARGINE_M + diametro + vela);   // la cima della vela sotto la chiglia
    double massima = g->quota_max - EMERGENZA_MARGINE_M;
    if (fondale_)
        if (const double f = profondita(g->pos); std::isfinite(f)) massima = std::min(massima, f - EMERGENZA_FRANCO_M);
    return std::max(std::min(q, massima), 10.0);   // in acqua bassa quella che c'e'
}

std::vector<std::pair<double, double>> Missione::profilo_cavo(const UnitaClassica& nave) const {
    std::vector<std::pair<double, double>> p;
    // la forma vera (nucleo/schiera): il cavo del pesce del VDS dalla poppa, poi la schiera (sulla Knox trainata dal
    // pesce: il suo profilo parte gia' da li')
    if (const auto t = traini_.find(nave.id); t != traini_.end() && t->second.vds.cavo_m > 0.0 && !t->second.vds.persa)
        p = t->second.vds.forma.profilo;
    if (const auto s = schiere_.find(nave.id); s != schiere_.end() && (nave.flag54 & 1) && s->second.cavo_m > 0.0 && !s->second.persa)
        p.insert(p.end(), s->second.forma.profilo.begin(), s->second.forma.profilo.end());
    if (!p.empty()) {
        std::stable_sort(p.begin(), p.end(), [](const auto& a, const auto& b) { return a.first < b.first; });
        return p;
    }
    // il ripiego: all'angolo critico, in orizzontale cavo x cos phi, poi la schiera alla quota del fondo del cavo
    const auto dati = cavo_di(nave.classe);
    if (!dati || !(nave.flag54 & 1)) return p;
    const double seno = std::min(1.0, dati->v0_kn / std::max(nave.velocita, 0.1)), coseno = std::sqrt(1.0 - seno * seno);
    const double quota = std::min(dati->quota_max_m, dati->cavo_m * seno);
    const double orizzontale = dati->cavo_m * coseno + dati->schiera_m;
    const double fine_cavo = quota * std::max(coseno, 0.05) / std::max(seno, 1e-9);   // dove il cavo arriva alla quota
    p = {{0.0, 0.0}, {std::min(fine_cavo, orizzontale), quota}, {orizzontale, quota}};
    return p;
}

std::vector<std::pair<Vec2, double>> Missione::punti_cavo(const UnitaClassica& nave) const {
    std::vector<std::pair<Vec2, double>> v;
    if (!collisioni_) return v;
    const auto profilo = profilo_cavo(nave);
    const auto sc = collisioni_->scie.find(nave.id);
    if (profilo.size() < 2 || sc == collisioni_->scie.end()) return v;
    const auto c = collisioni_->classi.find(nave.classe);
    const double l = c != collisioni_->classi.end() ? c->second.lunghezza_m : 130.0;
    const double r = radianti(nave.rotta);
    std::vector<Vec2> scia{{nave.pos.x - std::sin(r) * l / 2.0, nave.pos.y - std::cos(r) * l / 2.0}};
    scia.insert(scia.end(), sc->second.begin(), sc->second.end());
    // lungo la scia vera (la rotta seguita), ogni PASSO_CAVO_M, alla quota del profilo
    const double orizzontale = profilo.back().first;
    std::size_t k = 0;
    const auto quota_a = [&](double x) {
        while (k + 1 < profilo.size() && profilo[k + 1].first < x) ++k;
        if (k + 1 >= profilo.size()) return profilo.back().second;
        const auto& [x0, z0] = profilo[k];
        const auto& [x1, z1] = profilo[k + 1];
        return x1 - x0 > 1e-9 ? z0 + (z1 - z0) * std::clamp((x - x0) / (x1 - x0), 0.0, 1.0) : z1;
    };
    double x = 0.0;
    for (std::size_t i = 0; i + 1 < scia.size() && x < orizzontale; ++i) {
        const Vec2 a = scia[i], b = scia[i + 1];
        const double tratto = distanza(a, b);
        for (double t = 0.0; t < tratto && x < orizzontale; t += PASSO_CAVO_M, x += PASSO_CAVO_M) {
            const Vec2 p{a.x + (b.x - a.x) * t / std::max(tratto, 1e-9), a.y + (b.y - a.y) * t / std::max(tratto, 1e-9)};
            double z = quota_a(x);
            if (fondale_)
                if (const double f = profondita(p); std::isfinite(f)) z = std::min(z, std::max(0.0, f - 1.0));   // sul fondo
            v.push_back({p, z});
        }
    }
    return v;
}

double Missione::distanza_dal_cavo(int sottomarino, int nave) const {
    const UnitaClassica* s = nullptr;
    const UnitaClassica* n = nullptr;
    if (const auto it = unita_.find(sottomarino); it != unita_.end()) s = &it->second;
    if (const auto it = unita_.find(nave); it != unita_.end()) n = &it->second;
    if (!s || !n || !collisioni_) return -1.0;
    const auto punti = punti_cavo(*n);
    if (punti.empty()) return -1.0;
    const auto c = collisioni_->classi.find(s->classe);
    const double l = c != collisioni_->classi.end() ? c->second.lunghezza_m : 100.0, d = c != collisioni_->classi.end() ? c->second.larghezza_m : 10.0;
    const double asse = chiglia_vera(*s) - d / 2.0, r = radianti(s->rotta);
    const Vec2 a{std::sin(r), std::cos(r)};
    double minimo = 1e18;
    for (const auto& [p, z] : punti) {
        const Vec2 rel{p.x - s->pos.x, p.y - s->pos.y};
        const double t = std::clamp(rel.x * a.x + rel.y * a.y, -l / 2.0, l / 2.0);
        const double h = std::hypot(rel.x - t * a.x, rel.y - t * a.y);
        minimo = std::min(minimo, std::max(0.0, std::hypot(h, z - asse) - d / 2.0));
    }
    return minimo;
}

void Missione::passo_cavi() {
    auto& S = *collisioni_;
    // la scia della poppa di chi fila la schiera
    for (const auto& [id, u] : unita_) {
        if (u.categoria != "nave" || u.distrutta || u.persa || profilo_cavo(u).empty()) { S.scie.erase(id); continue; }
        auto& sc = S.scie[id];
        if (tempo_ - S.ultima_scia[id] < SCIA_OGNI_S && !sc.empty()) continue;
        S.ultima_scia[id] = tempo_;
        const auto c = S.classi.find(u.classe);
        const double l = c != S.classi.end() ? c->second.lunghezza_m : 130.0, r = radianti(u.rotta);
        sc.insert(sc.begin(), Vec2{u.pos.x - std::sin(r) * l / 2.0, u.pos.y - std::cos(r) * l / 2.0});
        if (sc.size() > 2000) sc.pop_back();
    }
    for (const auto& [nid, scia] : S.scie) {
        const UnitaClassica& n = unita_.at(nid);
        const auto profilo = profilo_cavo(n);
        const double lunghezza = profilo.empty() ? 0.0 : profilo.back().first;
        std::vector<std::pair<Vec2, double>> punti;
        for (auto& [sid, s] : unita_) {
            if (s.categoria != "sottomarino" || s.distrutta || s.persa) continue;
            if (distanza(s.pos, n.pos) > lunghezza + 500.0) continue;
            const auto chiave = std::make_pair(sid, nid);
            if (S.impigliati.count(chiave)) continue;
            if (punti.empty()) punti = punti_cavo(n);
            const auto c = S.classi.find(s.classe);
            const double l = c != S.classi.end() ? c->second.lunghezza_m : 100.0, d = c != S.classi.end() ? c->second.larghezza_m : 10.0;
            const double asse = chiglia_vera(s) - d / 2.0, r = radianti(s.rotta);
            const Vec2 elica{s.pos.x - std::sin(r) * l / 2.0, s.pos.y - std::cos(r) * l / 2.0};
            double vicino_elica = 1e18;
            Vec2 dove = elica;
            for (const auto& [p, z] : punti) {
                const double e = std::hypot(distanza(p, elica), z - asse);
                if (e < vicino_elica) { vicino_elica = e; dove = p; }
            }
            const double scafo = distanza_dal_cavo(sid, nid);
            if (vicino_elica <= d / 2.0 + IMPIGLIO_OLTRE_M) {   // il cavo sulla poppa all'altezza dell'elica: la prende
                S.impigliati.insert(chiave);
                incidenti_cavo_.push_back({tempo_, sid, nid, true, dove});
                guasti_.push_back({sid, static_cast<int>(Sistema::propulsione), PERDITA_ELICA, "Cavo nell'elica"});
                s.ordine.velocita_max_kn = VELOCITA_IMPIGLIATO_KN;
                s.ordine.fino_a = 1e18;
                UnitaClassica& nave = unita_.at(nid);
                nave.flag54 &= ~1;   // la schiera e' persa: il cavo si spezza sul verricello
                if (const auto sc = schiere_.find(nid); sc != schiere_.end()) { sc->second.persa = true; sc->second.cavo_m = 0.0; }
                if (Contatto* k = nave.contatti.count(sid) ? &nave.contatti.at(sid) : nuovo_contatto(nave, sid)) {
                    k->qualita = std::max(k->qualita, 1000.0);
                    k->ultima_scoperta = tempo_;
                    k->pos_stimata = s.pos;
                    k->quota_stimata = s.quota;
                    k->posizione_nota = true;
                    k->t_posizione = tempo_;
                    k->sensore = "urto";
                }
                if (catalogo_) {   // il cavo che si avvolge sull'elica: un colpo e lo stridio
                    Urto x;
                    x.tempo_s = tempo_;
                    x.a.unita = sid;
                    x.a.classe = s.classe;
                    x.a.sottomarino = true;
                    x.a.pos = s.pos;
                    x.a.quota_m = chiglia_vera(s);
                    x.b.unita = nid;
                    x.punto = elica;
                    x.quota_punto_m = asse;
                    EsitoUrto e;
                    e.a.parte = ParteUrto::eliche;
                    e.b.parte = ParteUrto::schiera;
                    e.livello_sorgente_db = 165.0;   // STIMA
                    e.durata_strisciamento_s = 5.0;
                    const int base = URTI_DA + URTI_PASSO * S.prossimo_transitorio;
                    S.prossimo_transitorio = (S.prossimo_transitorio + 1) % URTI_QUANTI;
                    for (auto& t : transitori_urto(x, e, base, seme_ ^ static_cast<std::uint64_t>(base))) transitori_armi_.push_back(std::move(t));
                }
            } else if (scafo >= 0.0 && scafo <= TOCCO_M && !S.toccati.count(chiave)) {
                S.toccati.insert(chiave);
                incidenti_cavo_.push_back({tempo_, sid, nid, false, s.pos});
            }
        }
    }
}

int Missione::manovre_crazy_ivan(int id) const {
    if (!collisioni_) return 0;
    const auto it = collisioni_->ivan.find(id);
    return it == collisioni_->ivan.end() ? 0 : it->second.fatti;
}

double Missione::chiglia_vera(const UnitaClassica& u) const {
    // la quota del motore e' la chiglia; vicino alla superficie la quota periscopica del classico (6,1 m) e' quella
    // della classe (688: 18,3 m), e fra la superficie e la quota periscopica si interpola dal pescaggio
    if (u.categoria != "sottomarino") return pescaggio(u);
    double qp = QUOTA_PERISCOPICA_CLASSE_M;
    if (peri_)
        if (const DatiClassePeriscopi* c = peri_->dati.classe(u.classe)) qp = c->quota_m;
    const double pesc = pescaggio(u);
    if (u.quota >= qp) return u.quota;
    if (u.quota <= QUOTA_PERISCOPICA_MISSIONE_M) return pesc + (qp - pesc) * std::max(0.0, u.quota) / QUOTA_PERISCOPICA_MISSIONE_M;
    return qp;
}

std::optional<Missione::ComandoManovra> Missione::guida_crazy_ivan(const UnitaClassica& u) const {
    if (!collisioni_ || crazy_ivan_spento_) return std::nullopt;
    const auto it = collisioni_->ivan.find(u.id);
    if (it == collisioni_->ivan.end() || tempo_ >= it->second.fine) return std::nullopt;
    const auto& v = it->second;
    const bool ascolto = tempo_ >= v.fine_virata;
    return ComandoManovra{v.rotta, ascolto ? std::min(v.velocita, IVAN_ASCOLTO_KN) : v.velocita, v.quota};
}

void Missione::passo_collisioni(double dt) {
    auto& S = *collisioni_;
    // --- Crazy Ivan: i sottomarini sovietici dell'IA in moto, immersi, non in caccia o in evasione ---
    if (!crazy_ivan_spento_)
        for (const auto& [id, u] : unita_) {
            if (u.giocatore || u.categoria != "sottomarino" || !(u.flag & 0x10) || u.distrutta || u.persa) continue;
            auto& v = S.ivan[id];
            if (v.prossimo < 0.0) v.prossimo = tempo_ + S.caso.uniforme(IVAN_MIN_S, IVAN_MAX_S);
            if (!v.vista) { v.rotta_vista = u.rotta_ordinata; v.vista = true; }
            if (tempo_ < v.fine) continue;   // manovra in corso
            if (std::fabs(normalizza_180(u.rotta_ordinata - v.rotta_vista)) > IVAN_ACCOSTATA_GRADI) {   // ha accostato
                v.rotta_vista = u.rotta_ordinata;
                v.prossimo = std::min(v.prossimo, tempo_ + S.caso.uniforme(IVAN_DOPO_ACCOSTATA_MIN_S, IVAN_DOPO_ACCOSTATA_MAX_S));
            }
            if (tempo_ < v.prossimo) continue;
            const bool pronto = u.velocita >= IVAN_VELOCITA_MIN_KN && u.quota >= IVAN_QUOTA_MINIMA_M && !u.sul_fondo &&
                                u.agganciato < 0 && !(asw_ && guida_evasione(u));
            v.prossimo = tempo_ + S.caso.uniforme(IVAN_MIN_S, IVAN_MAX_S);
            if (!pronto) continue;
            const double virata = S.caso.uniforme(IVAN_VIRATA_MIN, IVAN_VIRATA_MAX) * (S.caso.uniforme() < 0.5 ? -1.0 : 1.0);
            v.rotta = normalizza_360(u.rotta + virata);
            v.velocita = std::min(u.avanti_tutta, u.velocita + IVAN_PIU_KN);
            v.quota = u.quota_ordinata;
            if (S.caso.uniforme() < IVAN_P_QUOTA) {   // a volte con un cambio di quota (K-19: da 60 a 90 m)
                const double dq = S.caso.uniforme(IVAN_QUOTA_MIN_M, IVAN_QUOTA_MAX_M) * (S.caso.uniforme() < 0.5 ? -1.0 : 1.0);
                double massima = std::max(IVAN_QUOTA_MINIMA_M, u.quota_max - 20.0);
                if (fondale_)
                    if (const double f = profondita(u.pos); std::isfinite(f)) massima = std::min(massima, f - 30.0);
                v.quota = std::clamp(u.quota + dq, IVAN_QUOTA_MINIMA_M, std::max(IVAN_QUOTA_MINIMA_M, massima));
            }
            v.fine_virata = tempo_ + std::fabs(virata) / std::max(u.virata, 0.3) + 30.0;
            v.fine = v.fine_virata + S.caso.uniforme(IVAN_ASCOLTO_MIN_S, IVAN_ASCOLTO_MAX_S);
            v.rotta_vista = u.rotta_ordinata;   // l'Ivan non e' un'accostata: finito, il ritorno in rotta non ne chiama un altro
            ++v.fatti;
        }

    // --- forme ---
    std::vector<Forma> forme;
    for (const auto& [id, u] : unita_) {
        if (u.distrutta || u.persa || (u.flag & 0x20) || (u.flag & 0x80) || (u.flag & 3) == 3) continue;
        if (u.categoria != "sottomarino" && u.categoria != "nave") continue;
        const auto c = S.classi.find(u.classe);
        if (c == S.classi.end() || c->second.lunghezza_m <= 0.0) continue;
        const ClasseNave& k = c->second;
        Forma f;
        f.id = id;
        f.sottomarino = u.categoria == "sottomarino";
        const double r = radianti(u.rotta);
        const Vec2 asse{std::sin(r), std::cos(r)};
        f.scafo = {u.pos, asse, k.lunghezza_m / 2.0, std::max(k.larghezza_m, 1.0) / 2.0, 0.0, 0.0};
        if (f.sottomarino) {
            const double chiglia = chiglia_vera(u), d = std::max(k.larghezza_m, 1.0);
            f.in_superficie = u.quota <= 1.0;
            f.scafo.z0 = chiglia - d;
            f.scafo.z1 = chiglia;
            const double hv = k.altezza_vela_m > 0.0 ? k.altezza_vela_m : 0.6 * d;
            double x0 = k.vela_x_m[0], x1 = k.vela_x_m[1];
            if (x1 <= x0) {
                x0 = (VELA_AVANTI - VELA_LUNGHEZZA / 2.0) * k.lunghezza_m;
                x1 = (VELA_AVANTI + VELA_LUNGHEZZA / 2.0) * k.lunghezza_m;
            }
            const double xm = 0.5 * (x0 + x1);
            f.vela = {{u.pos.x + asse.x * xm, u.pos.y + asse.y * xm}, asse, 0.5 * (x1 - x0), VELA_LARGHEZZA_M / 2.0, chiglia - d - hv, chiglia - d};
            f.con_vela = true;
        } else {
            f.in_superficie = true;
            f.scafo.z0 = -SOVRASTRUTTURE_M;
            f.scafo.z1 = std::max(pescaggio(u), 1.0);
        }
        forme.push_back(f);
    }
    const UnitaClassica* g = giocatore();
    std::vector<std::pair<int, int>> viste;
    for (std::size_t i = 0; i < forme.size(); ++i)
        for (std::size_t j = i + 1; j < forme.size(); ++j) {
            const Forma& fa = forme[i];
            const Forma& fb = forme[j];
            if (!fa.sottomarino && !fb.sottomarino) continue;   // le navi fra loro no (per ora)
            const double dc = distanza(fa.scafo.c, fb.scafo.c);
            if (dc > fa.scafo.hl + fb.scafo.hl + VICINI_M) continue;
            const auto chiave = std::make_pair(std::min(fa.id, fb.id), std::max(fa.id, fb.id));
            auto& cp = S.coppie[chiave];
            viste.push_back(chiave);
            // la sovrapposizione piu' profonda fra scafi e vele
            Sovrapposizione migliore;
            double sep = 1e18;
            for (const Scatola* p : {&fa.scafo, fa.con_vela ? &fa.vela : nullptr})
                for (const Scatola* q : {&fb.scafo, fb.con_vela ? &fb.vela : nullptr}) {
                    if (!p || !q) continue;
                    const Sovrapposizione x = sovrapponi(*p, *q);
                    if (x.tocca && (!migliore.tocca || x.penetrazione > migliore.penetrazione)) migliore = x;
                    sep = std::min(sep, x.separazione);
                }
            if (migliore.tocca) sep = 0.0;
            // --- quasi collisioni del giocatore (P6) ---
            const bool del_giocatore = g && (fa.id == g->id || fb.id == g->id);
            if (del_giocatore) {
                const Forma& fg = fa.id == g->id ? fa : fb;
                const Forma& fo = fa.id == g->id ? fb : fa;
                const UnitaClassica& o = unita_.at(fo.id);
                const bool periscopica = g->quota <= QUOTA_PERISCOPICA_MISSIONE_M + 1.0;
                double soglia = 0.0;
                if (fo.sottomarino && !fg.in_superficie && !fo.in_superficie && !periscopica && o.quota > QUOTA_PERISCOPICA_MISSIONE_M + 1.0)
                    soglia = QUASI_IMMERSI_M;
                else if (!fo.sottomarino && periscopica && !fg.in_superficie)
                    soglia = QUASI_PERISCOPICA_M;
                if (soglia > 0.0) {
                    if (sep < soglia) {
                        if (!cp.vicini) { cp.vicini = true; cp.minimo = 1e18; cp.urtati = false; }
                        if (sep < cp.minimo) {
                            cp.minimo = sep;
                            cp.t_minimo = tempo_;
                            cp.pos_minimo = g->pos;
                            cp.quota_a = g->quota;
                            cp.quota_b = o.quota;
                        }
                    } else if (cp.vicini && sep > USCITA_QUASI * soglia) {
                        cp.vicini = false;
                        if (!cp.urtati) {
                            QuasiCollisione q;
                            q.tempo_s = cp.t_minimo;
                            q.a = g->id;
                            q.b = fo.id;
                            q.distanza_m = cp.minimo;
                            q.quota_a_m = cp.quota_a;
                            q.quota_b_m = cp.quota_b;
                            q.pos = cp.pos_minimo;
                            if (const auto c = g->contatti.find(fo.id); c != g->contatti.end() && c->second.visibile()) {
                                q.a_sapeva = true;
                                q.qualita_a = c->second.qualita;
                            }
                            if (const auto c = o.contatti.find(g->id); c != o.contatti.end() && c->second.visibile()) {
                                q.b_sapeva = true;
                                q.qualita_b = c->second.qualita;
                            }
                            quasi_collisioni_.push_back(q);
                        }
                    }
                }
            }
            // --- l'urto (P1) ---
            if (!migliore.tocca) { cp.contatto = false; continue; }
            // gia' sovrapposti alla partenza (le posizioni dell'originale: SCENE1B, 1C, 4A, dal giro lungo del 31/10/2026):
            // niente urti finche' restano a contatto; quando si staccano, un urto vero conta di nuovo
            if (tempo_ < 1.0) { cp.contatto = true; cp.ultimo_urto = 1e18; continue; }
            if (cp.contatto && tempo_ - cp.ultimo_urto < RIPETI_URTO_S) continue;
            cp.contatto = true;
            cp.ultimo_urto = tempo_;
            cp.urtati = true;
            const auto corpo = [&](const Forma& f) {
                const UnitaClassica& u = unita_.at(f.id);
                CorpoUrto c;
                c.unita = f.id;
                c.classe = u.classe;
                c.sottomarino = f.sottomarino;
                c.in_superficie = f.sottomarino && f.in_superficie;
                const auto m = S.masse.find(u.classe);
                const ClasseNave& k = S.classi.at(u.classe);
                const double stima = 1.025 * k.lunghezza_m * k.larghezza_m * std::max(f.scafo.z1, 1.0) * 0.55;   // senza dati (STIMA)
                if (f.sottomarino) {
                    const double immerso = 1.025 * PI * k.larghezza_m * k.larghezza_m / 4.0 * k.lunghezza_m * 0.75;
                    c.massa_t = m != S.masse.end() ? (c.in_superficie ? m->second.first : m->second.second) : immerso;
                    c.quota_m = f.scafo.z1;
                } else {
                    c.massa_t = m != S.masse.end() && m->second.first > 0.0 ? m->second.first : stima;
                    c.quota_m = 0.0;
                }
                if (c.massa_t <= 0.0) c.massa_t = f.sottomarino ? 5000.0 : stima;
                c.pos = u.pos;
                c.rotta_gradi = u.rotta;
                c.velocita_m_s = u.velocita * m_s_per_nodo_;
                if (const auto qp = S.quote_prec.find(f.id); qp != S.quote_prec.end() && dt > 0.0) c.rateo_verticale_m_s = (u.quota - qp->second) / dt;
                c.periscopio_alzato = (u.flag54 & 2) != 0;
                c.alberi_alzati = (u.flag54 & 4) != 0;
                c.schiera_filata = (u.flag54 & 1) != 0;
                return c;
            };
            Urto x;
            x.tempo_s = tempo_;
            x.a = corpo(fa);
            x.b = corpo(fb);
            x.punto = migliore.punto;
            x.quota_punto_m = migliore.quota;
            if (migliore.verticale) {   // da a verso b: in giu' se a sta sopra
                const double za = 0.5 * (fa.scafo.z0 + fa.scafo.z1), zb = 0.5 * (fb.scafo.z0 + fb.scafo.z1);
                x.normale[0] = x.normale[1] = 0.0;
                x.normale[2] = za < zb ? -1.0 : 1.0;
            } else {
                x.normale[0] = migliore.normale.x;
                x.normale[1] = migliore.normale.y;
                x.normale[2] = 0.0;
            }
            // l'urtante: quello che va verso l'altro piu' in fretta
            const auto chiusura = [&](const CorpoUrto& c, double segno) {
                const double r = radianti(c.rotta_gradi);
                return segno * (c.velocita_m_s * (std::sin(r) * x.normale[0] + std::cos(r) * x.normale[1]) - c.rateo_verticale_m_s * x.normale[2]);
            };
            if (chiusura(x.b, -1.0) > chiusura(x.a, 1.0)) {
                std::swap(x.a, x.b);
                for (double& k : x.normale) k = -k;
            }
            const std::uint64_t seme = seme_ ^ (static_cast<std::uint64_t>(urti_.size() + 1) * 0xC011150Bull);
            EsitoUrto e = valuta_urto(x, S.classi, S.cartella_dati, seme);
            // la scena acustica: il colpo, lo stridio, l'elica
            if (catalogo_) {
                const int base = URTI_DA + URTI_PASSO * S.prossimo_transitorio;
                S.prossimo_transitorio = (S.prossimo_transitorio + 1) % URTI_QUANTI;
                for (auto& t : transitori_urto(x, e, base, seme)) transitori_armi_.push_back(std::move(t));
            }
            // dopo l'urto entrambi sanno (16 §4.5): il contatto reciproco con la posizione
            for (const auto& [chi, altro] : {std::pair<int, int>{x.a.unita, x.b.unita}, {x.b.unita, x.a.unita}}) {
                UnitaClassica& o = unita_.at(chi);
                const UnitaClassica& b = unita_.at(altro);
                Contatto* k = nullptr;
                if (const auto ci = o.contatti.find(altro); ci != o.contatti.end()) k = &ci->second;
                else k = nuovo_contatto(o, altro);
                if (!k) continue;
                k->qualita = std::max(k->qualita, CONTATTO_URTO);
                k->lampeggio = 0;
                k->ultima_scoperta = tempo_;
                k->rilevamento = rilevamento(o.pos, b.pos);
                k->pos_stimata = b.pos;
                k->quota_stimata = b.quota;
                k->posizione_nota = true;
                k->t_posizione = tempo_;
                k->sensore = "urto";
            }
            urti_.push_back({x, e});
            // dopo l'urto si separano (la principale, 30/10/2026: nella S15 l'Ugra urtava il Charlie 86 volte, nella S08 un
            // Foxtrot tamponava l'altro nel canale ogni 5 s): le unita' dell'IA arretrano di quanto sono entrate piu'
            // SEPARA_M, lungo la normale (da fa verso fb) o in quota se l'urto e' verticale (fra sottomarini chi sta sopra
            // sale, chi sta sotto scende); il giocatore no, l'altra fa tutta la strada. Con le forze, per SEPARA_S, la nave
            // che urta perde l'abbrivio e si ferma sulla sua rotta, gli altri accostano via
            const bool ga = unita_.at(fa.id).giocatore, gb = unita_.at(fb.id).giocatore;
            const double quota_parte = ga || gb ? 1.0 : 0.5;
            const auto separa = [&](const Forma& f, const Forma& altra, double verso) {
                UnitaClassica& u = unita_.at(f.id);
                if (u.giocatore || (migliore.verticale && !f.sottomarino)) return;
                const double pen = (migliore.penetrazione + SEPARA_M) * quota_parte;
                if (migliore.verticale) {
                    const bool sopra = f.scafo.z0 + f.scafo.z1 < altra.scafo.z0 + altra.scafo.z1;
                    u.quota = sopra ? std::max(0.0, u.quota - pen) : std::min(u.quota_max, u.quota + pen);
                } else {
                    u.pos.x += verso * migliore.normale.x * pen;
                    u.pos.y += verso * migliore.normale.y * pen;
                }
                // i sottomarini dell'IA si separano in quota di SALTO_SEPARA_M, piu' in fretta d'una virata (come
                // l'anticollisione, 16 §4.3): fra due sottomarini chi sta sotto scende e chi sta sopra sale (a pari quota
                // l'id minore scende), sotto una nave il sottomarino scende; non quello fermo in superficie all'ormeggio
                if (f.sottomarino && !(u.quota < 1.0 && std::fabs(u.velocita) < 0.5)) {
                    const UnitaClassica& v = unita_.at(altra.id);
                    const bool scende = !altra.sottomarino || u.quota > v.quota + 0.5 || (std::fabs(u.quota - v.quota) <= 0.5 && f.id < altra.id);
                    const double fondo = profondita(u.pos);
                    const double sotto = std::min(u.quota_max, std::isfinite(fondo) && fondo > 0.0 ? fondo - 30.0 : 1e9);
                    const double giu = u.quota + SALTO_SEPARA_M, su = std::max(20.0, u.quota - SALTO_SEPARA_M);
                    u.quota_ordinata = scende ? (giu <= sotto ? giu : u.quota_ordinata) : (su < u.quota ? su : u.quota_ordinata);
                    u.rateo_quota = std::max(u.rateo_quota, u.rateo_quota_modello);
                }
                if (!forze_) return;
                const UnitaClassica& o = unita_.at(altra.id);
                if (f.id == x.a.unita && !f.sottomarino) {   // la nave che urta: perde l'abbrivio e si ferma sulla rotta
                    u.velocita = std::min(std::fabs(u.velocita), 0.5 * std::fabs(o.velocita));
                    const double r = radianti(u.rotta);
                    manovra_difesa(f.id, {u.pos.x + 3000.0 * std::sin(r), u.pos.y + 3000.0 * std::cos(r)}, 0.0, SEPARA_S);
                } else {   // l'urtato, e il sottomarino che urta (gli serve l'abbrivio per cambiare quota), accosta via
                    const double via = radianti(normalizza_360(rilevamento(o.pos, u.pos)));
                    manovra_difesa(f.id, {u.pos.x + 3000.0 * std::sin(via), u.pos.y + 3000.0 * std::cos(via)}, std::max(u.velocita, 5.0), SEPARA_S);
                }
            };
            separa(fa, fb, -1.0);
            separa(fb, fa, 1.0);
        }
    passo_cavi();
    // le coppie che non si vedono piu' (lontane o uscite): si dimenticano
    std::sort(viste.begin(), viste.end());
    for (auto it = S.coppie.begin(); it != S.coppie.end();)
        it = std::binary_search(viste.begin(), viste.end(), it->first) ? std::next(it) : S.coppie.erase(it);
    for (const auto& [id, u] : unita_) S.quote_prec[id] = u.quota;
}

}  // namespace m688
