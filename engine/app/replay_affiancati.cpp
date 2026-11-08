#include "replay_affiancati.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>

#include "grafica/lamiera.hpp"
#include "grafica/quadro.hpp"
#include "grafica/tema_stazioni.hpp"
#include "trascina_carta.hpp"

namespace m688 {

namespace {

constexpr int PIEDE_Y = 428;
constexpr int RIGHE_REGISTRO = 5;
constexpr double VELOCITA_MAX = 64.0;

int largo_q(const Font& f, std::string_view s) { return f.larghezza(s, 1, f.formato() == 2 ? f.spazio_predefinito() : 1, f.formato() != 2); }
void scrivi_q(Tela& t, const Font& f, int x, int y, std::string_view s, std::uint8_t c) {
    f.scrivi(t, x, y, s, c, 1, f.formato() == 2 ? f.spazio_predefinito() : 1, f.formato() != 2);
}
std::string maiu(std::string s) {
    for (auto& c : s) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return s;
}
std::string taglia(const Font& f, std::string s, int w) {
    if (largo_q(f, s) <= w) return s;
    while (!s.empty() && largo_q(f, s + "..") > w) s.pop_back();
    return s + "..";
}
std::string orologio(double s) {
    const int x = static_cast<int>(std::max(0.0, s));
    char b[16];
    std::snprintf(b, sizeof b, "%d:%02d:%02d", x / 3600, x / 60 % 60, x % 60);
    return b;
}
bool arma(const StatoUnita& u) { return u.categoria != "sottomarino" && u.categoria != "nave" && u.categoria != "aereo"; }

// le aree: la consolle (sotto la targa), due schermi affiancati, la fascia del tempo sopra il piede
constexpr int SCHERMI_Y = 22, SCHERMI_H = 372, FASCIA_Y = 400, FASCIA_H = 13;
Rettangolo schermo_lato(int k) { return k == 0 ? Rettangolo{8, SCHERMI_Y, 310, SCHERMI_H} : Rettangolo{322, SCHERMI_Y, 310, SCHERMI_H}; }

}  // namespace

ReplayAffiancati::ReplayAffiancati(std::string dati, Traccia a, Traccia b, double t_divergenza, std::uint64_t seme)
    : dati_(std::move(dati)), t_div_(t_divergenza), seme_(seme) {
    durata_ = std::max(a.durata, b.durata);
    a_ = std::make_unique<SchermataReplay>(dati_, std::move(a));
    b_ = std::make_unique<SchermataReplay>(dati_, std::move(b));
    a_->lettura(SchermataReplay::Lettura::verita);
    b_->lettura(SchermataReplay::Lettura::verita);
    try {
        targa_ = Font::carica(dati_ + "/font/incisione688_7.fnt");
        fosforo_ = Font::carica(dati_ + "/font/fosforo688_5x7.fnt");
    } catch (const std::exception&) {
        targa_.reset();
        fosforo_.reset();
    }
    vai(t_div_ >= 0.0 ? t_div_ : 0.0);
    centra();
}

void ReplayAffiancati::vai(double t) {
    t_ = std::clamp(t, 0.0, durata_);
    a_->vai(t_);
    b_->vai(t_);
}

void ReplayAffiancati::centra() {   // il baricentro delle unita' di A all'istante (le armi no)
    Vec2 s;
    int n = 0;
    for (const auto& u : a_->traccia().istante(t_).unita)
        if (!arma(u) && !u.distrutta) s.x += u.pos.x, s.y += u.pos.y, ++n;
    if (n) centro_ = {s.x / n, s.y / n};
    a_->vista(centro_, mpp_);
    b_->vista(centro_, mpp_);
}

bool ReplayAffiancati::comandi(const Comandi& c, double dt) {
    if (c.esci || c.esc) return true;
    if (c.pausa) pausa_ = !pausa_;
    int piu = c.compressione;
    for (const Azione a : c.azioni) piu += a == Azione::telegrafo_avanti ? 1 : a == Azione::telegrafo_indietro ? -1 : 0;
    for (; piu > 0; --piu) velocita_ = std::min(VELOCITA_MAX, velocita_ * 2.0);
    for (; piu < 0; ++piu) velocita_ = std::max(1.0, velocita_ / 2.0);
    if (c.ingrandisci) mpp_ = std::max(10.0, mpp_ / 1.5);
    if (c.riduci) mpp_ = std::min(2000.0, mpp_ * 1.5);
    rotella_carta(c, dentro_carta(c, carta_a()) ? carta_a() : carta_b(), centro_, mpp_);
    salto_attesa_ -= dt;
    const int verso = verso_freccia(c.ruota, c.passi_ruota);
    if (verso != salto_verso_) salto_attesa_ = 0.0;
    salto_verso_ = verso;
    if (verso != 0 && salto_attesa_ <= 0.0) vai(t_ + 30.0 * verso), salto_attesa_ = 0.2;
    for (const char l : c.lettere)
        if ((l == 'D' || l == 'd') && t_div_ >= 0.0) vai(t_div_), pausa_ = true;
    const Rettangolo b = linea();
    for (const auto& [x, y] : c.clic) {
        if (y >= b.y - 5 && y < b.y + b.h + 5 && x >= b.x - 4 && x < b.x + b.w + 4)
            vai(std::clamp(static_cast<double>(x - b.x) / std::max(1, b.w - 1), 0.0, 1.0) * durata_);
        else if (y >= PIEDE_Y + 22 && y < PIEDE_Y + 48 && x >= 324)
            return true;   // ESC del piede
        else if (y >= PIEDE_Y + 22 && y < PIEDE_Y + 48)
            pausa_ = !pausa_;
    }
    if (!pausa_) {
        vai(t_ + dt * velocita_);
        if (t_ >= durata_) pausa_ = true;
    }
    // trascinare una delle due carte le sposta insieme
    const auto [mx, my] = c.mouse;
    const auto dentro = [&](Rettangolo r) { return mx >= r.x && mx < r.x + r.w && my >= r.y && my < r.y + r.h; };
    if (c.premuto && (trascina_ || dentro(carta_a()) || dentro(carta_b()))) {
        if (trascina_ && (mx != trascina_->first || my != trascina_->second)) {
            centro_.x -= (mx - trascina_->first) * mpp_;
            centro_.y += (my - trascina_->second) * mpp_;
        }
        trascina_ = std::make_pair(mx, my);
    } else {
        trascina_.reset();
    }
    a_->vista(centro_, mpp_);
    b_->vista(centro_, mpp_);
    return false;
}

void ReplayAffiancati::palette(Palette& p) const {
    a_->palette(p);
    applica_tema(p, TemaStazione{});
}

void ReplayAffiancati::disegna(Tela& t, const Font& f) const {
    namespace q = quadro;
    const Font& fg = fosforo_ ? *fosforo_ : f;
    q::consolle(t, targa_ ? *targa_ : f, "SALA ANALISI - A/B AFFIANCATI");
    const std::string seme = seme_ ? "SEME " + std::to_string(seme_) : std::string();
    for (int k = 0; k < 2; ++k) {
        const SchermataReplay& r = k == 0 ? *a_ : *b_;
        const Rettangolo s = q::schermo(t, schermo_lato(k), fg, k == 0 ? "A  SENZA INTERVENTI" : "B  CON GLI INTERVENTI", seme);
        const Rettangolo c{s.x + 2, s.y + 2, s.w - 4, s.h - 8 - RIGHE_REGISTRO * 10};
        carte_[k] = c;
        r.disegna_carta_in(t, f, c);
        // gli ultimi fatti del registro fino all'istante
        std::vector<const EventoTraccia*> ultimi;
        for (const auto& e : r.traccia().registro)
            if (e.tempo <= t_ + 0.5) ultimi.push_back(&e);
        const std::size_t da = ultimi.size() > RIGHE_REGISTRO ? ultimi.size() - RIGHE_REGISTRO : 0;
        int y = c.y + c.h + 4;
        for (std::size_t i = da; i < ultimi.size(); ++i, y += 10) {
            const EventoTraccia& e = *ultimi[i];
            const bool dopo = t_div_ >= 0.0 && e.tempo >= t_div_ - 0.5;
            scrivi_q(t, fg, s.x + 4, y, taglia(fg, orologio(e.tempo).substr(2) + " " + maiu(e.testo_it.empty() ? e.tipo : e.testo_it), s.w - 8),
                     dopo ? colori::GIALLO : fosforo(6));
        }
    }
    // la fascia del tempo: la divergenza in giallo, gli interventi di B col colore della famiglia
    std::vector<q::TaccaTempo> tacche;
    for (const RecordIntervento& x : b_->traccia().interventi)
        if (x.fase == RecordIntervento::FASE_APPLICATO && durata_ > 0.0) tacche.push_back({x.t / durata_, colore_intervento(x.tipo), 100});
    const std::string sinistra = orologio(t_) + "  X" + std::to_string(static_cast<int>(velocita_)) + (pausa_ ? "  PAUSA" : "");
    const std::string destra = t_div_ >= 0.0 ? "DIVERGONO A " + orologio(t_div_) + "  (D)" : std::string("NON DIVERGONO");
    const Rettangolo b = barra_ = q::fascia_tempo(t, {8, FASCIA_Y, 624, FASCIA_H}, fg, sinistra, destra, durata_ > 0.0 ? t_ / durata_ : 0.0, tacche);
    if (t_div_ >= 0.0 && durata_ > 0.0) {
        const int x = b.x + static_cast<int>(t_div_ / durata_ * b.w);
        t.linea_verticale(x, b.y - 3, b.y + b.h + 2, colori::GIALLO);
    }
    lamiera::piede(t, f, PIEDE_Y, "P MARCIA   FRECCE 30 S   + - VELOCITA'   I O ZOOM   D LA DIVERGENZA   CLIC SULLA LINEA",
                   {{"P  MARCIA O PAUSA", ColoreLampada::ambra}, {"ESC  INDIETRO", ColoreLampada::verde}}, 1);
}

}  // namespace m688
