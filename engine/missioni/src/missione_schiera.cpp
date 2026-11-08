// La schiera trainata dei sottomarini nell'Enhanced (docs/20_sonar_blocco1.md §7, D; modulo puro nucleo/schiera.hpp;
// regole di danno del dossier docs/ricerca/04_towed_array.md §6.3, tutte STIMA DI GIOCO). Il collegamento:
// - chi fila (flag 54h, bit 1, dagli script come nel classico) srotola il cavo alla velocita' di filata del sistema;
//   l'IA fila solo il cavo che non tocca il fondo (cavo_sicuro_m), il giocatore tutto;
// - a ogni passo la forma d'equilibrio col ritardo del cavo (CavoSchiera), il fondo ai 16 punti del profilo lungo la
//   scia (la rotta attuale all'indietro), lo sfregamento; il Ricevitore della schiera (ricevitore_acustico) ascolta
//   alla quota vera, dietro la poppa;
// - ogni minuto i danni: il fondo (0,15 + 0,02 v, x2 su roccia; 70% danneggiata, 30% persa), l'eccesso di velocita',
//   la quota massima (schiacciata), le reti a strascico al lavoro (60% persa, 40% danneggiata); un'esplosione entro
//   500 m: 50% persa. La danneggiata ascolta 6 dB peggio; la persa resta persa per la missione.
// Per il giocatore e per l'IA, con la stessa regola (simmetria). Il classico non lo usa (serve l'acustica).

#include <algorithm>
#include <cmath>

#include "missioni/missione.hpp"

namespace m688 {

namespace {

constexpr double MINUTO_S = 60.0;
constexpr double ESPLOSIONE_M = 500.0, P_ESPLOSIONE = 0.5;
constexpr double RETE_M = 150.0, P_RETE_MINUTO = 0.5;   // la schiera entro 150 m da una rete al lavoro (STIMA)
constexpr double P_SCHIACCIATA_MINUTO = 0.2;            // oltre la quota massima del sistema (STIMA)
constexpr double CAVO_SCIA_M = 800.0;                   // l'IA guarda il fondale dei prossimi 800 m di scia
constexpr double MARGINE_QUOTA_M = 20.0;                // l'IA tiene il centro della schiera sotto la sua quota massima
// i messaggi di bordo (codici del blocco della principale; testi in tools/pipeline/esporta_motore.py)
constexpr int MSG_SCHIERA_FONDO = 396, MSG_SCHIERA_DANNEGGIATA = 397, MSG_SCHIERA_PERSA = 398, MSG_SCHIERA_RETE = 399;

Vec2 dietro(Vec2 da, double rotta, double m) {
    const double r = rotta * 3.14159265358979323846 / 180.0;
    return {da.x - m * std::sin(r), da.y - m * std::cos(r)};
}

}  // namespace

const DatiSchiera* Missione::dati_schiera(const UnitaClassica& u) const {
    if (!catalogo_ || (u.categoria != "sottomarino" && u.categoria != "nave")) return nullptr;   // le navi: docs/ricerca/33
    for (const Sensore* s : catalogo_->sensori_di(u.classe))
        if (s->antenna == Antenna::lineare)
            if (const DatiSchiera* d = catalogo_->schiera(s->id)) return d;
    return nullptr;
}

std::optional<Missione::StatoSchieraUnita> Missione::stato_schiera(int id) const {
    const auto it = schiere_.find(id);
    if (it == schiere_.end()) return std::nullopt;
    const SchieraUnita& s = it->second;
    StatoSchieraUnita r;
    r.cavo_filato_m = s.cavo_m;
    r.quota_centro_m = s.forma.quota_centro_m;
    r.quota_bassa_m = s.forma.quota_bassa_m;
    r.dietro_m = s.forma.dietro_m;
    r.fondo = s.fondo.tocca;
    r.oltre_quota_max = s.forma.oltre_quota_max;
    r.danneggiata = s.danneggiata;
    r.persa = s.persa;
    return r;
}

bool Missione::schiera_persa(int id) const {
    const auto it = schiere_.find(id);
    return it != schiere_.end() && it->second.persa;
}

void Missione::danneggia_schiera(UnitaClassica& u, SchieraUnita& s, bool persa, int messaggio) {
    if (s.persa) return;
    if (persa || s.danneggiata) {   // una seconda volta sulla schiera gia' danneggiata: persa
        s.persa = true;
        s.cavo_m = 0.0;
        if (u.flag54 & 1) { u.flag54 &= ~1; u.sonar -= 2; }   // come il recupero (F54_1)
        if (u.giocatore && MSG_SCHIERA_PERSA) messaggio_di_bordo(u.id, MSG_SCHIERA_PERSA);
        return;
    }
    s.danneggiata = true;
    if (u.giocatore && messaggio) messaggio_di_bordo(u.id, messaggio);
}

void Missione::passo_schiere(double dt) {
    if (!caso_schiera_) caso_schiera_.emplace(seme_ ^ 0x5C41E7A5ull);
    Casuale& caso = *caso_schiera_;
    // le esplosioni dall'ultimo passo, per la regola dei 500 m
    std::vector<Vec2> scoppi;
    for (; colpi_visti_schiera_ < colpi_.size(); ++colpi_visti_schiera_) scoppi.push_back(colpi_[colpi_visti_schiera_].pos);
    for (auto& [id, u] : unita_) {
        if (u.distrutta || u.persa || (u.categoria != "sottomarino" && u.categoria != "nave") || (u.flag & 0x20)) continue;
        const bool filata = (u.flag54 & 1) != 0;
        auto it = schiere_.find(id);
        if (it == schiere_.end()) {
            if (!filata) continue;
            const DatiSchiera* d = dati_schiera(u);
            if (!d) continue;
            it = schiere_.emplace(id, SchieraUnita{}).first;
            it->second.dati = d;
            // gia' filata all'inizio della missione (il modello dello scenario; l'acustica arriva al primo passo): tutto
            // il cavo
            if (tempo_ <= 1.0) it->second.cavo_m = -1.0;   // < 0: gia' fuori quanto voluto (sotto)
        }
        SchieraUnita& s = it->second;
        if (s.persa) continue;
        const DatiSchiera& d = *s.dati;
        const double v = std::fabs(u.velocita);
        if (!filata) {   // recuperata (gli script hanno gia' il loro tempo): dentro
            s.cavo_m = 0.0;
            s.stato = CavoSchiera{};
            s.fondo = ContattoFondo{};
            s.avvisato_fondo = false;
            continue;
        }
        // il cavo: il giocatore umano fila tutto; l'IA, e il comandante IA che guida il giocatore nella sandbox, solo
        // quello che non tocca il fondo (cavo_sicuro_m sul fondale piu' basso dei prossimi 800 m di scia), e lo accorcia
        // o lo allunga alle velocita' del verricello
        // sulla Knox la SQR-18A e' trainata dal pesce dell'SQS-35 (docs/ricerca/33 §1): parte dalla sua quota, dietro di lui
        const auto pesce = traino_dal_pesce(u);
        const double quota_traino = pesce ? pesce->first : u.quota;
        double voluto = d.cavo_max_m;
        if ((!u.giocatore || senza_giocatore_) && fondale_) {
            double basso = 1e9;
            for (double m = 0.0; m <= CAVO_SCIA_M; m += 100.0) basso = std::min(basso, profondita(dietro(u.pos, u.rotta, m)));
            if (std::isfinite(basso) && basso < 1e8) voluto = std::min(voluto, cavo_sicuro_m(d, v, quota_traino, basso));
        }
        // e non la porta oltre la quota massima, dove si schiaccerebbe (le navi lente con la SQR-19: docs/ricerca/33), alla
        // velocita' piu' bassa del suo sprint and drift (missione_vds.cpp)
        const auto tr = traini_.find(id);
        const double v_quota = tr != traini_.end() && tr->second.ciclo ? std::min(v, tr->second.v_deriva_kn) : v;
        if (!u.giocatore || senza_giocatore_) voluto = std::min(voluto, cavo_per_quota_m(d, v_quota, quota_traino, d.quota_max_m - MARGINE_QUOTA_M));
        // la nave dell'IA che recupera prima d'accelerare: il cavo rientra al verricello
        if (tr != traini_.end() && tr->second.recupero) voluto = 0.0;
        if (s.cavo_m < 0.0) s.cavo_m = voluto;   // filata prima della missione: il cavo voluto
        else if (s.cavo_m < voluto) s.cavo_m = std::min(voluto, s.cavo_m + d.filata_m_s * dt);
        else s.cavo_m = std::max(voluto, s.cavo_m - d.recupero_m_s * dt);
        s.forma = s.stato.avanza(d, dt, v, quota_traino, s.cavo_m);
        if (pesce) {
            for (auto& [x, q] : s.forma.profilo) x += pesce->second;
            s.forma.dietro_m += pesce->second;
        }
        // il fondo lungo la scia, ai punti del profilo
        std::vector<double> fondo;
        fondo.reserve(s.forma.profilo.size());
        for (const auto& [m, q] : s.forma.profilo) fondo.push_back(fondale_ ? profondita(dietro(u.pos, u.rotta, m)) : -1.0);
        s.fondo = contatto_fondo(s.forma, fondo);
        const Vec2 centro = dietro(u.pos, u.rotta, s.forma.dietro_m);
        if (clima_) s.tipo_fondo = clima_->fondo(fondale_ ? profondita(centro) : 1000.0);
        if (s.fondo.tocca && !s.avvisato_fondo) {
            s.avvisato_fondo = true;
            if (u.giocatore && MSG_SCHIERA_FONDO) messaggio_di_bordo(id, MSG_SCHIERA_FONDO);
        }
        if (!s.fondo.tocca) s.avvisato_fondo = false;
        // un'esplosione entro 500 m dalla schiera
        for (const Vec2& p : scoppi)
            if (distanza(p, centro) <= ESPLOSIONE_M && caso.uniforme() < P_ESPLOSIONE) danneggia_schiera(u, s, true, 0);
        if (s.persa) continue;
        // ogni minuto le regole del §6.3
        s.minuto_s += dt;
        if (s.minuto_s < MINUTO_S) continue;
        s.minuto_s -= MINUTO_S;
        if (s.fondo.tocca) {
            const double p = (0.15 + 0.02 * v) * (s.tipo_fondo == TipoFondo::roccia ? 2.0 : 1.0);
            if (caso.uniforme() < p) danneggia_schiera(u, s, caso.uniforme() < 0.3, MSG_SCHIERA_DANNEGGIATA);
        }
        if (!s.persa && v > d.v_max_kn) {
            const bool molto = v > d.v_max_kn + 6.0;
            if (caso.uniforme() < 0.02 * (v - d.v_max_kn) * (molto ? 3.0 : 1.0))
                danneggia_schiera(u, s, molto && caso.uniforme() < 0.5, MSG_SCHIERA_DANNEGGIATA);
        }
        if (!s.persa && s.forma.oltre_quota_max && caso.uniforme() < P_SCHIACCIATA_MINUTO)
            danneggia_schiera(u, s, false, MSG_SCHIERA_DANNEGGIATA);
        // le reti a strascico al lavoro (passo_reti): la schiera che ci passa vicino, alla loro quota
        if (!s.persa)
            for (const auto& r : reti_) {
                if (distanza(r.pos, centro) > RETE_M || s.forma.quota_bassa_m < r.quota_m - 30.0) continue;
                if (caso.uniforme() >= P_RETE_MINUTO) continue;
                if (u.giocatore && MSG_SCHIERA_RETE) messaggio_di_bordo(id, MSG_SCHIERA_RETE);
                danneggia_schiera(u, s, caso.uniforme() < 0.6, 0);
                break;
            }
    }
}

}  // namespace m688
