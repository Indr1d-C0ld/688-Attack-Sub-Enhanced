// Il sonar fuori scafo delle navi dell'IA nell'Enhanced (docs/ricerca/33; un solo modello con le schiere, nucleo/schiera):
// - il pesce del VDS e' una schiera corta al cavo: la quota ordinata si raggiunge filando il cavo (cavo_per_quota_m), al
//   piu' fino all'equilibrio col cavo intero e alla quota massima; la velocita' lo fa risalire. La quota, con lo strato
//   (la nave ha il batitermografo), e' SOTTO_STRATO_M sotto lo strato; senza, due terzi della massima; mai entro
//   FONDO_VDS_M dal fondo. L'acustica lo sente alla quota e alla posizione del pesce (ricevitore_acustico);
// - la schiera trainata (missione_schiera.cpp, come i sottomarini): la nave la fila da sola (flag 54h, bit 1, come gli
//   script) sotto la velocita' per filare; sulla Knox la SQR-18A e' trainata dal pesce dell'SQS-35, quindi prima il pesce;
// - recupero prima di accelerare: col cavo fuori il pilota non supera la velocita' di traino; se lo script vuole di
//   piu' la nave recupera alla velocita' per filare e accelera col cavo dentro. Con un siluro contro di lei accelera
//   subito, e il traino veloce porta il danno (passo_schiere e qui);
// - sprint and drift (dossier 02 §5, come il comandante dell'IA): in deriva DERIVA_S a V_DERIVA_KN (ferma per i sonar
//   calabili), in scatto SPRINT_S alla velocita' che tiene la media dello script: la nave arriva dove deve quando deve.
//   Niente ciclo se lo scatto supererebbe la velocita' di traino o l'avanti tutta, in caccia o in manovra;
// - il registro: un evento per manovra (eventi_traino), per il debriefing e il replay: filato quando arriva in quota, con
//   la quota; recuperato quando e' dentro, se era stato detto filato. Si comincia a filare solo con la voluta che regge
//   da STABILE_S (l'accostata del classico la fa saltare ogni pochi secondi).
// Solo le navi dell'IA, con la stessa regola del giocatore (simmetria). Il classico non lo usa (serve l'acustica).

#include <algorithm>
#include <cmath>

#include "missioni/missione.hpp"

namespace m688 {

namespace {

constexpr double SOTTO_STRATO_M = 30.0, FONDO_VDS_M = 30.0, QUOTA_VDS_MIN_M = 20.0;   // STIMA
constexpr double FONDALE_FILARE_M = 60.0;          // con meno acqua la nave non cala niente (STIMA)
constexpr double SPRINT_S = 600.0, DERIVA_S = 300.0, DERIVA_CALABILE_S = 600.0, V_DERIVA_KN = 6.0;   // STIMA
constexpr double MINACCIA_M = 15000.0;            // il siluro contro la nave entro questa distanza: si accelera subito
constexpr double MINUTO_S = 60.0, ESPLOSIONE_M = 500.0, P_ESPLOSIONE = 0.5;   // come la schiera
constexpr double LIBERA_KN = 1e9;
constexpr double PASSO_TRAINO_S = 1.0;            // il passo della missione (al piu'), per l'inizio
constexpr double QUOTA_OGNI_S = 30.0;              // la quota voluta del pesce si rivede ogni mezzo minuto
// l'isteresi: la nave comincia a filare (o rallenta per filare) solo se la velocita' voluta regge il traino da un minuto.
// L'accostata del classico porta la voluta a un quarto e indietro ogni pochi secondi: senza, il pesce andava su e giu'
// (la libreria, 30/10/2026: centinaia di «recupera»)
constexpr double STABILE_S = 60.0;

Vec2 dietro(Vec2 da, double rotta, double m) {
    const double r = rotta * 3.14159265358979323846 / 180.0;
    return {da.x - m * std::sin(r), da.y - m * std::cos(r)};
}

bool e_vds(const Sensore& s) { return s.id.find("vds") != std::string::npos; }

}  // namespace

const DatiSchiera* Missione::dati_vds(const UnitaClassica& u) const {
    if (!catalogo_ || u.categoria != "nave") return nullptr;
    for (const Sensore* s : catalogo_->sensori_di(u.classe))
        if (e_vds(*s))
            if (const DatiSchiera* d = catalogo_->schiera(s->id)) return d;
    return nullptr;
}

bool Missione::vds_in_acqua(int id) const {
    const auto it = traini_.find(id);
    return it != traini_.end() && it->second.vds.cavo_m > 0.0 && !it->second.vds.persa;
}

std::optional<Missione::StatoVds> Missione::stato_vds(int id) const {
    const auto it = traini_.find(id);
    if (it == traini_.end() || !it->second.vds.dati) return std::nullopt;
    const TrainoNave& t = it->second;
    StatoVds r;
    r.cavo_filato_m = t.vds.cavo_m;
    r.quota_m = t.vds.forma.quota_centro_m;
    r.quota_ordinata_m = t.quota_vds_m;
    r.dietro_m = t.vds.forma.dietro_m;
    r.in_acqua = t.vds.cavo_m > 0.0 && !t.vds.persa;
    r.recupero = t.recupero;
    r.persa = t.vds.persa;
    return r;
}

std::optional<std::pair<double, double>> Missione::traino_dal_pesce(const UnitaClassica& u) const {
    const auto it = traini_.find(u.id);
    if (it == traini_.end() || !vds_in_acqua(u.id)) return std::nullopt;
    return std::make_pair(it->second.vds.forma.quota_centro_m, it->second.vds.forma.dietro_m);
}

double Missione::velocita_traino(const UnitaClassica& u, double v) const {
    const auto it = traini_.find(u.id);
    if (it == traini_.end()) return v;
    const TrainoNave& t = it->second;
    if (t.libera) return v;
    v = std::min(v, t.limite_kn);
    if (!t.ciclo || t.recupero) return v;
    // sprint and drift: la media dello script tenuta dallo scatto
    const double deriva = t.deriva_s, giro = SPRINT_S + deriva;
    const double fase = std::fmod(tempo_ + 37.0 * u.id, giro);
    const double vd = std::min(t.v_deriva_kn, v);
    if (fase >= SPRINT_S) return vd;
    return std::min((v * giro - vd * deriva) / SPRINT_S, t.limite_kn);
}

void Missione::nota_traino(const UnitaClassica& u, const std::string& sistema, bool filato, double quota_m) {
    eventi_traino_.push_back({tempo_, u.id, sistema, filato, quota_m});
}

void Missione::passo_traini(double dt) {
    if (!caso_traino_) caso_traino_.emplace(seme_ ^ 0x7A1D05EEull);
    Casuale& caso = *caso_traino_;
    std::vector<Vec2> scoppi;
    for (; colpi_visti_traino_ < colpi_.size(); ++colpi_visti_traino_) scoppi.push_back(colpi_[colpi_visti_traino_].pos);
    const bool inizio = tempo_ <= 2.0 * PASSO_TRAINO_S;   // gia' fuori all'inizio della missione (il modello dello scenario)
    for (auto& [id, u] : unita_) {
        if (u.categoria != "nave" || u.giocatore || u.distrutta || u.persa || (u.flag & 0x20)) continue;
        const DatiSchiera* dv = dati_vds(u);
        const DatiSchiera* ds = dati_schiera(u);
        if (!dv && !ds) continue;
        TrainoNave& t = traini_[id];
        if (dv && !t.vds.dati) t.vds.dati = dv;
        if (dv && t.vds.persa) dv = nullptr;
        if (ds && schiera_persa(id)) ds = nullptr;
        const double v = std::fabs(u.velocita);
        const double fondale = fondale_ ? profondita(u.pos) : 1e9;
        const bool acqua = !std::isfinite(fondale) || fondale > FONDALE_FILARE_M;
        // la minaccia: un siluro contro di lei, vicino: accelera senza recuperare
        t.libera = false;
        for (const auto& [wid, w] : unita_)
            if ((w.flag & 0x20) && !w.distrutta && !w.persa && w.bersaglio == id && distanza(w.pos, u.pos) < MINACCIA_M) {
                t.libera = true;
                break;
            }
        const auto st = stato_schiera(id);
        const double cavo_schiera = st ? st->cavo_filato_m : 0.0;
        const bool schiera_fuori = ds && (u.flag54 & 1);
        const bool vds_fuori = dv && t.vds.cavo_m > 0.0;
        const bool calabile = dv && dv->v_max_kn < V_DERIVA_KN;
        // il ciclo: la media dello script tenuta dallo scatto; il calabile scatta col pesce dentro
        const double voluta = std::fabs(u.velocita_ordinata);
        t.v_deriva_kn = calabile ? 0.0 : V_DERIVA_KN;
        t.deriva_s = calabile ? DERIVA_CALABILE_S : DERIVA_S;
        const double giro = SPRINT_S + t.deriva_s, vd = std::min(t.v_deriva_kn, voluta);
        const double scatto = (voluta * giro - vd * t.deriva_s) / SPRINT_S;
        double tetto = u.avanti_tutta * 280.0 / 256.0;
        if (ds) tetto = std::min(tetto, ds->v_max_kn);
        if (dv && !calabile) tetto = std::min(tetto, dv->v_max_kn);
        const bool in_manovra = u.insegui || u.ordine.meta || u.ordine.velocita_max_kn >= 0.0 || manovre_difesa_.count(id) ||
                                reazioni_in_corso_.count(id);
        t.ciclo = !in_manovra && !t.libera && acqua && voluta > t.v_deriva_kn + 2.0 && scatto <= tetto;
        const double fase = std::fmod(tempo_ + 37.0 * u.id, giro);
        // la velocita' che il pilota avra' (velocita_traino), su cui si decide
        const double obiettivo = t.ciclo ? (fase < SPRINT_S ? scatto : vd) : voluta;
        const auto regge = [&](const DatiSchiera* d) { return d && obiettivo <= d->v_max_kn + 1e-6; };
        // il recupero: oltre la velocita' di traino o senza l'acqua; finito quando tutto e' dentro
        if (!t.libera && ((schiera_fuori && (!regge(ds) || !acqua)) || (vds_fuori && (!regge(dv) || !acqua)))) t.recupero = true;
        if (t.recupero) {
            if (schiera_fuori && t.schiera_ia && cavo_schiera <= 0.0 && !inizio) {   // la schiera e' dentro (passo_schiere)
                u.flag54 &= ~1;
                u.sonar -= 2;
                t.schiera_ia = false;
                if (!t.schiera_in_filata) nota_traino(u, ds->id, false, 0.0);   // annunciata in quota: il registro lo dice
                t.schiera_in_filata = false;
            }
            if (!((u.flag54 & 1) && t.schiera_ia) && t.vds.cavo_m <= 0.0) t.recupero = false;
        }
        // filare: con l'acqua e la velocita' che regge da STABILE_S; prima il pesce (la SQR-18A della Knox ci e' trainata)
        const bool puo = !t.recupero && !t.libera && acqua;
        t.voluta_bassa_s = puo && (regge(dv) || regge(ds)) ? t.voluta_bassa_s + dt : 0.0;
        const bool stabile = inizio || t.voluta_bassa_s >= STABILE_S;
        t.vds_fila = puo && regge(dv) && (vds_fuori || stabile);
        bool rallenta = false;   // per filare la nave scende alla velocita' per filare
        if (t.vds_fila && t.vds.cavo_m <= 0.0 && v > dv->v_manovra_kn + 1e-6 && !inizio) { t.vds_fila = false; rallenta = true; }
        if (puo && stabile && regge(ds) && !(u.flag54 & 1) && (!dv || vds_fuori)) {
            if (v <= ds->v_manovra_kn + 1e-6 || inizio) {
                u.flag54 |= 1;
                u.sonar += 2;
                t.schiera_ia = true;
                t.schiera_in_filata = true;
                if (inizio) {   // gia' fuori: passo_schiere fila subito il cavo voluto
                    SchieraUnita& s = schiere_[id];
                    s.dati = ds;
                    s.cavo_m = -1.0;
                }
            } else {
                rallenta = true;
            }
        }
        // la schiera in filata: il cavo cresce ancora (passo_schiere); in quota il registro la dice
        if (t.schiera_in_filata && (u.flag54 & 1)) {
            if (cavo_schiera > 0.0 && cavo_schiera <= t.cavo_schiera_prima + 1e-9) {
                t.schiera_in_filata = false;
                if (st) nota_traino(u, ds->id, true, st->quota_centro_m);
            } else {
                rallenta = true;
            }
        }
        t.cavo_schiera_prima = cavo_schiera;
        // il pesce: il cavo verso quello della quota voluta, alle velocita' del verricello
        if (dv) {
            SchieraUnita& s = t.vds;
            double cavo_voluto = 0.0;
            if (t.vds_fila && (tempo_ >= t.quota_s || t.quota_vds_m <= 0.0)) {   // la quota voluta, ogni QUOTA_OGNI_S
                t.quota_s = tempo_ + QUOTA_OGNI_S;
                double q = dv->quota_max_m * 2.0 / 3.0;
                // lo strato del batitermografo: quello del mese, sotto il microstrato del pomeriggio (che ne azzera la SLD)
                if (clima_)
                    if (const double sld = sld_in(u.pos); sld > 0.0) q = sld + SOTTO_STRATO_M;
                if (std::isfinite(fondale)) q = std::min(q, fondale - FONDO_VDS_M);
                t.quota_vds_m = std::clamp(q, QUOTA_VDS_MIN_M, dv->quota_max_m);
            }
            if (t.vds_fila) cavo_voluto = cavo_per_quota_m(*dv, v, u.quota, t.quota_vds_m);
            // la SQR-18A dal pesce: il pesce resta fuori finche' la schiera non e' dentro
            if (t.recupero && (u.flag54 & 1) && t.schiera_ia) cavo_voluto = s.cavo_m;
            const bool era_fuori = s.cavo_m > 0.0;
            if (inizio && t.vds_fila) s.cavo_m = cavo_voluto;
            else if (s.cavo_m < cavo_voluto) s.cavo_m = std::min(cavo_voluto, s.cavo_m + dv->filata_m_s * dt);
            else s.cavo_m = std::max(cavo_voluto, s.cavo_m - dv->recupero_m_s * dt);
            if (s.cavo_m > 0.0) s.forma = s.stato.avanza(*dv, dt, v, u.quota, s.cavo_m);
            else { s.stato = CavoSchiera{}; s.forma = FormaSchiera{}; }
            if (!era_fuori && s.cavo_m > 0.0) t.annunciare = true;
            if (t.vds_fila && s.cavo_m < cavo_voluto - 1.0) rallenta = true;
            if (t.annunciare && cavo_voluto > 0.0 && s.cavo_m >= cavo_voluto - 1.0) {   // in quota: il registro lo dice
                t.annunciare = false;
                nota_traino(u, dv->id, true, s.forma.quota_centro_m);
            }
            // dentro: il registro lo dice se l'aveva detto in quota (un pesce calato a meta' e tirato su non e' una manovra)
            if (era_fuori && s.cavo_m <= 0.0) {
                if (!t.annunciare) nota_traino(u, dv->id, false, 0.0);
                t.annunciare = false;
            }
            // i danni: un'esplosione vicina, il traino oltre la velocita' (come la schiera); il pesce perso resta perso
            if (s.cavo_m > 0.0) {
                const Vec2 centro = dietro(u.pos, u.rotta, s.forma.dietro_m);
                for (const Vec2& p : scoppi)
                    if (distanza(p, centro) <= ESPLOSIONE_M && caso.uniforme() < P_ESPLOSIONE) s.persa = true;
                s.minuto_s += dt;
                if (s.minuto_s >= MINUTO_S) {
                    s.minuto_s -= MINUTO_S;
                    if (v > dv->v_max_kn && caso.uniforme() < 0.02 * (v - dv->v_max_kn) * (v > dv->v_max_kn + 6.0 ? 3.0 : 1.0))
                        s.persa = true;
                }
                if (s.persa) { s.cavo_m = 0.0; s.forma = FormaSchiera{}; nota_traino(u, dv->id, false, 0.0); }
            }
        }
        // il limite del pilota: per recuperare e per filare la velocita' per filare, col cavo fuori quella di traino
        double vm = LIBERA_KN, vn = LIBERA_KN;
        if ((u.flag54 & 1) && ds) { vm = std::min(vm, ds->v_max_kn); vn = std::min(vn, ds->v_manovra_kn); }
        if (dv && (t.vds.cavo_m > 0.0 || t.vds_fila)) { vm = std::min(vm, dv->v_max_kn); vn = std::min(vn, dv->v_manovra_kn); }
        if (rallenta) {
            if (ds && !(u.flag54 & 1) && regge(ds)) vn = std::min(vn, ds->v_manovra_kn);
            if (dv && regge(dv)) vn = std::min(vn, dv->v_manovra_kn);
        }
        t.limite_kn = t.libera ? LIBERA_KN : (t.recupero || rallenta) ? vn : vm;
    }
}

}  // namespace m688
