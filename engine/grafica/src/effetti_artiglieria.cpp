// Effetti visivi dell'artiglieria navale contro il sottomarino (docs/16_artiglieria.md, pezzo G4; dossier
// docs/ricerca/22_artiglieria_contro_sottomarini.md §6 e §9.6, citato «22»): dagli EventoArtiglieria della missione
// (nucleo/acustica_artiglieria.hpp) le particelle nel mondo.
#include <algorithm>
#include <cmath>

#include "grafica/effetti.hpp"

namespace m688 {

namespace {

constexpr double G = 9.81;
constexpr double VETRO_M = 100.0;              // entro questa distanza la colonna bagna la finestra (22 §6.2)
constexpr double V_RIMBALZO_M_S = 250.0;       // il proietto dopo il rimbalzo (STIMA)
constexpr double V_TRACCIANTE_M_S = 880.0;     // AK-630, OR-84 (22 §3.1)
constexpr double TRACCIANTE_VISIBILE_S = 0.6;  // l'ultimo tratto del volo che si disegna

// Colonne per calibro (22 §6.1, STIMA dalla tabella del gioco): calibro (mm), altezza e diametro (m), durata (s)
struct Riga {
    double calibro, H, D, durata;
};
constexpr Riga COLONNE[] = {{30, 3.5, 0.75, 1.5}, {57, 6.5, 1.25, 2.5}, {76, 12.5, 2.25, 4.0}, {100, 17.5, 3.0, 5.0},
                            {130, 25.0, 4.0, 6.5}, {406, 80.0, 9.0, 12.5}};

double gaussiana(Casuale& c) {   // Box-Muller
    const double u1 = std::max(c.uniforme(), 1e-12), u2 = c.uniforme();
    return std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * PI * u2);
}

}  // namespace

Effetti::MisureColonna Effetti::colonna_proietto(double calibro_mm, double carica_kg) {
    // interpolazione in log(calibro) fra le righe della tabella
    const double c = std::clamp(calibro_mm, COLONNE[0].calibro, COLONNE[std::size(COLONNE) - 1].calibro);
    std::size_t i = 0;
    while (i + 2 < std::size(COLONNE) && c > COLONNE[i + 1].calibro) ++i;
    const Riga& a = COLONNE[i];
    const Riga& b = COLONNE[i + 1];
    const double u = std::clamp(std::log(c / a.calibro) / std::log(b.calibro / a.calibro), 0.0, 1.0);
    MisureColonna m{a.H + (b.H - a.H) * u, a.D + (b.D - a.D) * u, a.durata + (b.durata - a.durata) * u};
    if (carica_kg <= 0.0) {   // inerte: senza esplosivo solo il getto dell'impatto, un terzo dell'altezza (STIMA)
        m.altezza_m *= 0.35;
        m.diametro_m *= 0.6;
        m.durata_s *= 0.5;
    }
    return m;
}

void Effetti::artiglieria(const EventoArtiglieria& e, double t_missione) {
    coda_artiglieria_.push_back({e, tempo_ + std::max(0.0, e.tempo_s - t_missione)});
}

void Effetti::illuminante(Vec2 pos, double quota_m, double calibro_mm) {
    // 5"/38 Illum Mk 30: ~50 s sotto il paracadute (22 §6.3, alta); gli altri calibri come quello (STIMA)
    const double durata = calibro_mm >= 100.0 ? 50.0 : 40.0;
    illuminanti_.push_back({pos.x, pos.y, quota_m, tempo_, durata});
    auto& p = nuova(Materia::illuminante);
    p.x = pos.x; p.y = pos.y; p.z = quota_m;
    p.vita = durata;
    p.r = 1.0;
    p.opacita = 1.0;
    // la fumata dell'espulsione, grigia, che il vento porta via
    for (int i = 0; i < 6; ++i) {
        auto& f = nuova(Materia::fumo);
        f.x = pos.x + caso_.uniforme(-3, 3); f.y = pos.y + caso_.uniforme(-3, 3); f.z = quota_m + caso_.uniforme(0, 6);
        f.r = 2.0; f.dr = 0.6; f.r0 = f.r;
        f.vita = caso_.uniforme(8, 14);
        f.tono = 0.55;
        f.opacita = 0.6;
    }
}

double Effetti::luce_illuminanti(Vec2 pos) const {
    double l = 0.0;
    for (const auto& b : illuminanti_) {
        const double t = tempo_ - b.t0;
        if (t < 0 || t > b.durata) continue;
        // si accende in un secondo, brucia pieno, cala negli ultimi 5 s; scende col paracadute come la particella
        const double intensita = std::clamp(t, 0.0, 1.0) * std::clamp((b.durata - t) / 5.0, 0.0, 1.0);
        const double z = std::max(0.0, b.z - 4.0 * t);
        const double d = std::sqrt((pos.x - b.x) * (pos.x - b.x) + (pos.y - b.y) * (pos.y - b.y) + z * z);
        // illumina il mare per 1-2 km (STIMA, 22 §6.3): mezza luce a ~1 km
        l += intensita * 0.6 / (1.0 + (d / 1000.0) * (d / 1000.0));
    }
    return std::clamp(l, 0.0, 1.0);
}

double Effetti::acqua_sul_vetro(Vec2 occhio) const {
    double a = 0.0;
    for (const auto& i : impatti_) {
        const double d = std::hypot(occhio.x - i.x, occhio.y - i.y);
        if (d > VETRO_M) continue;
        // l'acqua arriva quando la colonna e' su (circa il tempo di salita) e cola via in 4 s
        const double ta = 0.8 * std::sqrt(2.0 * i.H / G), t = tempo_ - i.t;
        const double inviluppo = t < 0.5 * ta ? 0.0 : t < ta ? (t - 0.5 * ta) / (0.5 * ta) : std::exp(-(t - ta) / 2.0);
        a = std::max(a, (1.0 - d / VETRO_M) * std::min(1.0, i.H / 15.0) * inviluppo);
    }
    return std::clamp(a, 0.0, 1.0);
}

void Effetti::colonna(double x, double y, double calibro_mm, double carica_kg, double ritardo, bool tinta) {
    const MisureColonna m = colonna_proietto(calibro_mm, carica_kg);
    const double H = m.altezza_m * caso_.uniforme(0.8, 1.2), D = m.diametro_m * caso_.uniforme(0.85, 1.15);
    impatti_.push_back({x, y, tempo_ + ritardo, H});
    // il getto: particelle che salgono a quote distribuite fino a H e ricadono (salita in ~sqrt(2H/g), 22 §6.1)
    const int n = std::clamp(static_cast<int>(18 + 2.0 * H), 18, 220);
    for (int i = 0; i < n; ++i) {
        const double h = H * (0.2 + 0.8 * std::pow(caso_.uniforme(), 0.6));
        const double a = caso_.uniforme(0, 2 * PI), rho = 0.5 * D * std::sqrt(caso_.uniforme()) * (0.4 + 0.6 * h / H);
        auto& p = nuova(tinta && caso_.uniforme() < 0.45 ? Materia::tinta : Materia::spruzzo);   // acqua tinta, non fuoco
        p.x = x + 0.3 * rho * std::cos(a); p.y = y + 0.3 * rho * std::sin(a);
        p.H = h;
        p.ta = 0.8 * std::sqrt(2.0 * h / G) + 0.05;
        p.vx = 0.7 * rho * std::cos(a) / p.ta; p.vy = 0.7 * rho * std::sin(a) / p.ta;
        p.v_limite = 22;
        p.r = std::max(0.25, 0.18 * D + 0.02 * H); p.dr = 0.05 * H / p.ta;
        p.r_max = 0.4 * D + 0.05 * H;
        p.ritardo = ritardo + caso_.uniforme(0, 0.08);
        p.vita = p.ta + std::sqrt(2.0 * h / G) + 0.4;
        p.opacita = 0.95;
        const double ta = p.ta;
        if (i % 5 == 0) {   // la nebbia che resta dopo la ricaduta, col vento
            auto& b = nuova(Materia::nebbia);
            b.x = x + rho * std::cos(a); b.y = y + rho * std::sin(a); b.z = h * caso_.uniforme(0.3, 0.8);
            b.r = 0.3 * D + 0.05 * H; b.dr = 0.3;
            b.ritardo = ritardo + ta;
            b.vita = m.durata_s * caso_.uniforme(0.8, 1.3);
            b.vz = -0.6;
            b.opacita = 0.45;
        }
    }
    // la schiuma bianca sul mare dove e' caduto
    auto& s = nuova(Materia::schiuma);
    s.x = x; s.y = y; s.piatta = true;
    s.r = 0.5 * D; s.dr = 0.4; s.r_max = 1.5 * D + 2.0;
    s.ritardo = ritardo;
    s.vita = 2.0 * m.durata_s;
    s.opacita = 0.7;
    if (carica_kg > 0.0) {   // il lampo rossastro alla base degli HE (22 §6.3), un decimo di secondo
        auto& f = nuova(Materia::fuoco);
        f.x = x; f.y = y; f.z = f.z0 = 0.3;
        f.r = 0.6 * D + 0.5; f.dr = 0;
        f.ritardo = ritardo;
        f.vita = 0.12;
        f.calore = 0.75;
        f.opacita = 1.0;
    }
}

void Effetti::emetti(const EventoArtiglieria& e, const Mondo& mondo) {
    const Nave* tiratore = nullptr;
    for (const auto& n : mondo.navi())
        if (n.id == e.tiratore) tiratore = &n;
    // la linea di tiro: dal tiratore al punto (o, senza tiratore, da nord)
    double ux = 0.0, uy = -1.0;
    if (tiratore) {
        const double dx = e.pos.x - tiratore->pos.x, dy = e.pos.y - tiratore->pos.y, d = std::hypot(dx, dy);
        if (d > 1.0) { ux = dx / d; uy = dy / d; }
    }
    const bool tinta = e.calibro_mm >= 400.0;   // il 16" dell'Iowa: le colonne tinte (22 §6.1)
    switch (e.tipo) {
        case TipoEventoArtiglieria::scoppio_superficie:
            colonna(e.pos.x, e.pos.y, e.calibro_mm, std::max(e.carica_kg, 0.01), 0.0, tinta);
            break;
        case TipoEventoArtiglieria::tonfo:
            colonna(e.pos.x, e.pos.y, e.calibro_mm, 0.0, 0.0, false);
            break;
        case TipoEventoArtiglieria::rimbalzo: {
            // lo spruzzo basso e lungo del primo contatto, poi un secondo e un terzo piu' avanti sulla linea di tiro
            const MisureColonna m = colonna_proietto(e.calibro_mm, 0.0);
            double x = e.pos.x, y = e.pos.y, ritardo = 0.0, scala = 1.0;
            for (int k = 0; k < 3; ++k) {
                for (int i = 0; i < 10; ++i) {   // spruzzi lungo 10-20 m di scia
                    const double s = caso_.uniforme(0, 15.0 * scala);
                    spruzzi(x + ux * s, y + uy * s, 0.0, 2, std::max(1.0, 0.6 * m.altezza_m * scala), 0.6);
                    for (std::size_t j = particelle_.size() - 2; j < particelle_.size(); ++j) particelle_[j].ritardo = ritardo + s / V_RIMBALZO_M_S;
                }
                impatti_.push_back({x, y, tempo_ + ritardo, 1.5 * scala});
                const double salto = caso_.uniforme(80.0, 250.0) * scala;
                x += ux * salto; y += uy * salto;
                ritardo += salto / V_RIMBALZO_M_S;
                scala *= 0.6;
            }
            break;
        }
        case TipoEventoArtiglieria::scoppio_aria: {
            // VT: sbuffo grigio-nero alla quota, col lampo; sotto la chiazza di spruzzi delle schegge (22 §6.1)
            const double z = std::max(e.quota_m, 2.0);
            auto& f = nuova(Materia::fuoco);
            f.x = e.pos.x; f.y = e.pos.y; f.z = f.z0 = z;
            f.r = 1.5; f.vita = 0.1; f.calore = 1.0; f.opacita = 1.0;
            for (int i = 0; i < 14; ++i) {
                auto& p = nuova(Materia::fumo);
                p.x = e.pos.x + caso_.uniforme(-2, 2); p.y = e.pos.y + caso_.uniforme(-2, 2); p.z = z + caso_.uniforme(-1.5, 1.5);
                p.r = caso_.uniforme(1.5, 3.0); p.dr = caso_.uniforme(0.3, 0.8); p.r0 = p.r; p.r_max = 8.0;
                p.vita = caso_.uniforme(5, 15);
                p.tono = caso_.uniforme(0.05, 0.3);
                p.opacita = 0.85;
            }
            const double raggio = std::min(20.0, 0.6 * z + 5.0), cade = z / 200.0;   // le schegge scendono in fretta
            for (int i = 0; i < 20; ++i) {
                const double a = caso_.uniforme(0, 2 * PI), r = raggio * std::sqrt(caso_.uniforme());
                spruzzi(e.pos.x + r * std::cos(a), e.pos.y + r * std::sin(a), 0.0, 1, caso_.uniforme(1.0, 3.0), 0.4);
                particelle_.back().ritardo = cade + caso_.uniforme(0, 0.15);
            }
            impatti_.push_back({e.pos.x, e.pos.y, tempo_ + cade, 2.5});
            break;
        }
        case TipoEventoArtiglieria::raffica: {
            // AK-630 e AK-230: colonnine di 2-5 m in fila attorno al punto mirato (il «ribollire»), coi traccianti
            const int n = std::clamp(e.colpi, 1, 400);
            const double passo = std::max(e.intervallo_s, 0.005);
            for (int k = 0; k < n; ++k) {
                const double t = k * passo;
                // la dispersione: allungata sulla linea di tiro (STIMA: 12 m lungo, 5 m di traverso)
                const double lungo = 12.0 * gaussiana(caso_), traverso = 5.0 * gaussiana(caso_);
                const double x = e.pos.x + ux * lungo - uy * traverso, y = e.pos.y + uy * lungo + ux * traverso;
                spruzzi(x, y, 0.0, 3, caso_.uniforme(2.0, 5.0), 0.5);
                for (std::size_t j = particelle_.size() - 3; j < particelle_.size(); ++j) particelle_[j].ritardo = t;
                if (k % 4 == 0) impatti_.push_back({x, y, tempo_ + t, 3.5});
                if (tiratore && k % 2 == 0) {   // un tracciante ogni due colpi, l'ultimo tratto del volo
                    auto& p = nuova(Materia::tracciante);
                    const double tx = x - tiratore->pos.x, ty = y - tiratore->pos.y, d = std::hypot(tx, ty);
                    const double tratto = std::min(d, V_TRACCIANTE_M_S * TRACCIANTE_VISIBILE_S);
                    p.x = x - tx / d * tratto; p.y = y - ty / d * tratto; p.z = 6.0 * tratto / std::max(d, 1.0) + 0.5;
                    p.vx = tx / d * V_TRACCIANTE_M_S; p.vy = ty / d * V_TRACCIANTE_M_S; p.vz = -p.z / (tratto / V_TRACCIANTE_M_S);
                    p.propulsa = true;
                    p.ritardo = std::max(0.0, t - tratto / V_TRACCIANTE_M_S);
                    p.vita = tratto / V_TRACCIANTE_M_S;
                    p.opacita = 1.0;
                }
            }
            break;
        }
        case TipoEventoArtiglieria::partenza: {
            // la vampa di bocca (una frazione di secondo, la si vede fino all'orizzonte di notte) e il fumo grigio che
            // resta 5-10 s sulla nave; il cannone a ~8 m sul mare (STIMA), la vampa un poco verso il bersaglio
            double bx = 0.0, by = 0.0;
            for (const auto& n : mondo.navi())
                if (n.id == e.bersaglio) {
                    const double dx = n.pos.x - e.pos.x, dy = n.pos.y - e.pos.y, d = std::hypot(dx, dy);
                    if (d > 1.0) { bx = dx / d; by = dy / d; }
                }
            // la vampa: 6 m col 76 mm, 8 col 5", 19 col 16" (STIMA dalle foto)
            const double r = 3.0 + e.calibro_mm / 25.0, z = e.calibro_mm >= 400.0 ? 12.0 : 8.0;
            for (int i = 0; i < 6; ++i) {
                auto& f = nuova(Materia::fuoco);
                const double s = caso_.uniforme(0.0, 2.5 * r);
                f.x = e.pos.x + bx * s; f.y = e.pos.y + by * s; f.z = f.z0 = z;
                f.r = r * caso_.uniforme(0.5, 1.0); f.vita = caso_.uniforme(0.08, 0.16);
                f.calore = 1.0; f.opacita = 1.0;
            }
            for (int i = 0; i < 8; ++i) {
                auto& p = nuova(Materia::fumo);
                const double s = caso_.uniforme(0.0, 3.0 * r);
                p.x = e.pos.x + bx * s; p.y = e.pos.y + by * s; p.z = z + caso_.uniforme(-1, 1);
                p.r = 0.8 * r; p.dr = 0.5; p.r0 = p.r; p.r_max = 3.0 * r;
                p.vita = caso_.uniforme(5, 10);
                p.tono = caso_.uniforme(0.45, 0.7);
                p.opacita = 0.8;
            }
            break;
        }
        case TipoEventoArtiglieria::colpo_testa:
        case TipoEventoArtiglieria::passaggio:
            break;   // l'oculare (ParametriOculare) e l'audio
        case TipoEventoArtiglieria::illuminante:   // pos e quota_m d'apertura del paracadute
            illuminante(e.pos, std::max(e.quota_m, 100.0), e.calibro_mm);
            break;
    }
}

void Effetti::avanza_artiglieria(const Mondo& mondo) {
    // gli eventi arrivati al loro tempo
    std::vector<EventoArtiglieria> pronti;
    for (auto it = coda_artiglieria_.begin(); it != coda_artiglieria_.end();) {
        if (it->t <= tempo_) { pronti.push_back(it->e); it = coda_artiglieria_.erase(it); }
        else ++it;
    }
    for (const auto& e : pronti) emetti(e, mondo);
    // impatti e illuminanti finiti
    impatti_.erase(std::remove_if(impatti_.begin(), impatti_.end(), [this](const Impatto& i) { return tempo_ - i.t > 20.0; }), impatti_.end());
    illuminanti_.erase(std::remove_if(illuminanti_.begin(), illuminanti_.end(),
                                      [this](const Illuminante& b) { return tempo_ - b.t0 > b.durata; }),
                       illuminanti_.end());
}

}  // namespace m688
