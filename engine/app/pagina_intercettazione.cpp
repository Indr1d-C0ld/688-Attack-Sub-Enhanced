#include "pagina_intercettazione.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <optional>

#include "grafica/consolle.hpp"
#include "grafica/palette.hpp"
#include "grafica/simboli_carta.hpp"
#include "nucleo/geometria.hpp"

namespace m688 {

namespace {

constexpr int ROSSO_CHIARO = 12;   // EGA 12: l'acquisizione
constexpr int N_FINESTRE = static_cast<int>(std::size(PaginaIntercettazione::FINESTRE_MIN));

std::vector<AreaAttiva>* aree_ = nullptr;
void area(int x, int y, int w, int h, std::string comando) {
    if (aree_) aree_->push_back({x, y, w, h, std::move(comando)});
}

const char* classe_breve(TipoEmissione t) {
    switch (t) {
        case TipoEmissione::scafo: return "SCAFO";
        case TipoEmissione::immersione: return "IMMERSIONE";
        case TipoEmissione::boa: return "BOA";
        case TipoEmissione::testa: return "TESTA";
        case TipoEmissione::ricerca_hf: return "ALTA FREQ.";
        case TipoEmissione::ignoto: return "IGNOTO";
    }
    return "?";
}

std::string virgola(std::string s) {
    for (char& c : s) if (c == '.') c = ',';
    return s;
}

std::string con_formato(const char* formato, double v) {
    char r[32];
    std::snprintf(r, sizeof r, formato, v);
    return virgola(r);
}

std::string frequenza(double f_hz) { return con_formato(f_hz < 10000.0 ? "%.1f KHZ" : "%.0f KHZ", f_hz / 1000.0); }
std::string durata(double s) { return s < 0.0 ? "--" : s < 0.1 ? con_formato("%.0f MS", s * 1000.0) : con_formato("%.1f S", s); }
std::string cadenza(double s) { return s < 0.0 ? "--" : con_formato(s < 10.0 ? "%.1f S" : "%.0f S", s); }
std::string modulazione(int fm) { return fm < 0 ? "--" : fm ? "FM" : "CW"; }
std::string doppler(int d) { return d > 0 ? "+" : d < 0 ? "-" : "?"; }
std::string rilevamento(double r) {
    char b[16];
    std::snprintf(b, sizeof b, "%03d", static_cast<int>(std::lround(normalizza_360(r))) % 360);
    return b;
}

// il colore dell'allarme di una traccia, o quello del testo
int colore_allarme(Allarme a, int altrimenti) {
    return a == Allarme::siluro_in_acquisizione ? ROSSO_CHIARO : a == Allarme::siluro_in_acqua ? colori::GIALLO : altrimenti;
}

}  // namespace

// --- comandi ---

void PaginaIntercettazione::su_giu(const RicevitoreIntercettazione& r, int verso) {
    const auto& tr = r.tracce();
    if (tr.empty()) { scelta_ = -1; return; }
    int i = -1;
    for (std::size_t k = 0; k < tr.size(); ++k)
        if (tr[k].id == scelta_) i = static_cast<int>(k);
    i = i < 0 ? 0 : std::clamp(i + (verso > 0 ? 1 : -1), 0, static_cast<int>(tr.size()) - 1);
    scelta_ = tr[static_cast<std::size_t>(i)].id;
}

void PaginaIntercettazione::finestra(int verso) { finestra_ = std::clamp(finestra_ + (verso > 0 ? 1 : -1), 0, N_FINESTRE - 1); }

// --- testo e geometria ---

std::string PaginaIntercettazione::riga(const TracciaIntercetto& tr) {
    std::string s = "BRG " + rilevamento(tr.rilevamento) + "  " + frequenza(tr.f_hz) + "  " + durata(tr.durata_s) + "  PRI " +
                    cadenza(tr.pri_s) + "  " + con_formato("%+.1f°/MIN", tr.deriva_deg_min) + "  " +
                    con_formato("%+.0f DB", tr.livello_db) + "  " + modulazione(tr.fm) + "  " + classe_breve(tr.classe) +
                    "  DOPPLER " + doppler(tr.doppler);
    return s;
}

bool PaginaIntercettazione::viva(const TracciaIntercetto& tr, double ora) {
    return ora - tr.ultimo_t <= std::max(60.0, 3.0 * tr.pri_s);
}

Rettangolo PaginaIntercettazione::cascata(Rettangolo a) {
    return {a.x + LARGO_ROSA + MARGINE_FREQ, a.y + ALTO_ALLARME + 14, a.w - LARGO_ROSA - MARGINE_FREQ - 6, ALTO_CENTRO - 28};
}

double PaginaIntercettazione::y_frequenza(Rettangolo c, double f_hz) {
    const double u = std::log10(std::clamp(f_hz, F_MIN_HZ, F_MAX_HZ) / F_MIN_HZ) / std::log10(F_MAX_HZ / F_MIN_HZ);
    return c.y + (1.0 - u) * (c.h - 1);
}

double PaginaIntercettazione::x_tempo(Rettangolo c, double t, double ora) const {
    return c.x + c.w - 1 - (ora - t) / (finestra_min() * 60.0) * (c.w - 1);
}

// --- disegno ---

void PaginaIntercettazione::disegna(Tela& t, const Font& f, Rettangolo a, const RicevitoreIntercettazione* r, double ora,
                                    const StileIntercettazione& s, std::vector<AreaAttiva>* aree) const {
    aree_ = aree;
    t.imposta_ritaglio(a);
    t.rettangolo_pieno(a.x, a.y, a.w, a.h, colori::NERO);
    const int testo = s.modo_1989 ? colori::VERDE_CHIARO : colori::BIANCO;
    const int tenue = s.modo_1989 ? colori::VERDE : colori::GRIGIO;
    char buf[96];

    // l'allarme, in alto: le due lampade, il testo, il ricevitore e le tracce vive
    const Allarme allarme = r ? r->allarme() : Allarme::nessuno;
    lampada_rettangolare(t, {a.x + 6, a.y + 5, 26, 10}, ColoreLampada::ambra, allarme != Allarme::nessuno);
    lampada_rettangolare(t, {a.x + 36, a.y + 5, 26, 10}, ColoreLampada::rossa, allarme == Allarme::siluro_in_acquisizione);
    if (!r) {
        f.scrivi(t, a.x + 72, a.y + 6, "NESSUN INTERCETTATORE A BORDO", static_cast<std::uint8_t>(tenue), 1);
        t.togli_ritaglio();
        aree_ = nullptr;
        return;
    }
    f.scrivi(t, a.x + 72, a.y + 6, allarme == Allarme::nessuno ? std::string("NESSUN ALLARME") : nome_allarme(allarme),
             static_cast<std::uint8_t>(colore_allarme(allarme, tenue)), 1);
    const auto& tracce = r->tracce();
    const int vive = static_cast<int>(std::count_if(tracce.begin(), tracce.end(), [&](const TracciaIntercetto& x) { return viva(x, ora); }));
    std::string nome = r->dati().id;
    std::transform(nome.begin(), nome.end(), nome.begin(), [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    std::snprintf(buf, sizeof buf, "INTERCETTATORE %s   TRACCE VIVE %d", nome.c_str(), vive);
    f.scrivi(t, a.x + a.w - 8 - f.larghezza(buf, 1), a.y + 6, buf, static_cast<std::uint8_t>(tenue), 1);
    t.linea_orizzontale(a.x, a.x + a.w - 1, a.y + ALTO_ALLARME, colori::GRIGIO_SCURO);

    // la rosa dei rilevamenti, a sinistra: le tracce vive come raggi, prua in alto, lunghi col livello
    {
        const Vec2 c{a.x + LARGO_ROSA / 2.0, a.y + ALTO_ALLARME + ALTO_CENTRO / 2.0};
        const double raggio = std::min(LARGO_ROSA, ALTO_CENTRO) / 2.0 - 14.0;
        simboli::Pennello pn;
        pn.arco(c, raggio, 0.0, 360.0, static_cast<std::uint8_t>(tenue));
        for (int g = 0; g < 360; g += 30) {   // le tacche ogni 30 gradi relativi
            const double ang = radianti(g);
            pn.linea({c.x + (raggio - 4) * std::sin(ang), c.y - (raggio - 4) * std::cos(ang)},
                     {c.x + raggio * std::sin(ang), c.y - raggio * std::cos(ang)}, static_cast<std::uint8_t>(tenue));
        }
        for (const auto& tr : tracce) {
            if (!viva(tr, ora)) continue;
            const double ang = radianti(tr.rilevamento - s.rotta);
            const double l = raggio * std::clamp(0.3 + tr.livello_db / 40.0, 0.3, 1.0);
            const Vec2 punta{c.x + l * std::sin(ang), c.y - l * std::cos(ang)};
            const auto col = static_cast<std::uint8_t>(colore_allarme(tr.allarme, testo));
            pn.linea(c, punta, col);
            if (tr.id == scelta_) {   // la scelta: il raggio doppio
                pn.linea({c.x + 1, c.y}, {punta.x + 1, punta.y}, col);
                pn.linea({c.x, c.y + 1}, {punta.x, punta.y + 1}, col);
            }
        }
        pn.stendi(t, false);
        f.scrivi(t, static_cast<int>(c.x) - f.larghezza("PRUA", 1) / 2, a.y + ALTO_ALLARME + 3, "PRUA", static_cast<std::uint8_t>(tenue), 1);
    }

    // la cascata frequenza-tempo, a destra
    const Rettangolo c = cascata(a);
    const double finestra_s = finestra_min() * 60.0;
    // le decadi sull'asse
    for (const double dec : {1000.0, 10000.0, 100000.0}) {
        const int y = static_cast<int>(y_frequenza(c, dec));
        t.linea_orizzontale(c.x - 3, c.x - 1, y, static_cast<std::uint8_t>(tenue));
        const std::string e = dec < 5000.0 ? "1K" : dec < 50000.0 ? "10K" : "100K";
        f.scrivi(t, c.x - 4 - f.larghezza(e, 1), y - 3, e, static_cast<std::uint8_t>(tenue), 1);
    }
    t.linea_verticale(c.x - 1, c.y, c.y + c.h - 1, colori::GRIGIO_SCURO);
    // il tempo: le tacche ogni minuto (ogni 5 con la finestra di 30), l'etichetta della finestra coi suoi tasti
    const int passo_min = finestra_min() >= 30 ? 5 : 1;
    for (int m = 0; m <= finestra_min(); m += passo_min) {
        const int x = static_cast<int>(x_tempo(c, ora - m * 60.0, ora));
        t.linea_verticale(x, c.y + c.h, c.y + c.h + 2, static_cast<std::uint8_t>(tenue));
    }
    std::snprintf(buf, sizeof buf, "-%d MIN", finestra_min());
    f.scrivi(t, c.x, c.y + c.h + 4, buf, static_cast<std::uint8_t>(tenue), 1);
    f.scrivi(t, c.x + c.w - f.larghezza("ADESSO", 1), c.y + c.h + 4, "ADESSO", static_cast<std::uint8_t>(tenue), 1);
    {
        std::snprintf(buf, sizeof buf, "FINESTRA %d MIN", finestra_min());
        const int wl = f.larghezza(buf, 1), xm = c.x + c.w - wl - 40, y = a.y + ALTO_ALLARME + 3;
        f.scrivi(t, xm - 16, y, "<", static_cast<std::uint8_t>(tenue), 1);
        area(xm - 18, y - 2, 12, 11, "intercetto_finestra_meno");
        f.scrivi(t, xm, y, buf, static_cast<std::uint8_t>(testo), 1);
        f.scrivi(t, xm + wl + 6, y, ">", static_cast<std::uint8_t>(tenue), 1);
        area(xm + wl + 4, y - 2, 12, 11, "intercetto_finestra_piu");
    }
    // gli intercetti: una tacca larga quanto la durata (almeno 2 px), il colore dal livello sopra la soglia
    t.imposta_ritaglio(c);
    const double tau = s.persistenza == cascate::Persistenza::breve ? 2.0 : s.persistenza == cascate::Persistenza::lunga ? 10.0 : 5.0;
    std::optional<cascate::Fosforo> fosforo;
    if (s.fosforo) fosforo.emplace(c.w, c.h);
    std::vector<Rettangolo> scelti;   // le tacche della traccia scelta, cerchiate dopo
    const TracciaIntercetto* sc = nullptr;
    for (const auto& tr : tracce)
        if (tr.id == scelta_) sc = &tr;
    for (const Intercetto& x : r->storia()) {
        if (ora - x.t > finestra_s || x.t > ora) continue;
        const int x0 = static_cast<int>(x_tempo(c, x.t, ora));
        const int larg = std::max(2, static_cast<int>(std::lround((x.durata_s > 0.0 ? x.durata_s : 0.0) / finestra_s * c.w)));
        const int y = static_cast<int>(y_frequenza(c, x.f_hz)) - 1;
        const double u = cascate::frazione(x.livello_db, 0.0, 30.0);
        if (fosforo) {   // il lampo dell'impulso nuovo che si spegne fino a un velo fermo
            const double v = std::max(0.35, u) * (0.45 + 0.55 * std::exp(-(ora - x.t) / tau));
            for (int dy = 0; dy < 3; ++dy)
                for (int dx = 0; dx < larg; ++dx) fosforo->deponi(x0 - c.x + dx, y - c.y + dy, v);
        } else {
            for (int dy = 0; dy < 3; ++dy)
                for (int dx = 0; dx < larg; ++dx)
                    t.pixel(x0 + dx, y + dy, cascate::indice(std::max(0.15, u), x0 + dx, y + dy, s.scala));
        }
        if (sc && std::any_of(sc->intercetti.begin(), sc->intercetti.end(), [&](const Intercetto& k) { return k.t == x.t && k.f_hz == x.f_hz; }))
            scelti.push_back({x0 - 2, y - 2, larg + 4, 7});
    }
    if (fosforo) fosforo->componi(t, c.x, c.y, s.scala, cascate::Fosforo::Effetti{});
    // le bande del ricevitore sopra gli intercetti (il fosforo ricomposto copre tutta la cascata): i limiti tratteggiati e
    // i nomi B1... sul bordo sinistro
    const auto& bande = r->dati().bande;
    for (std::size_t b = 0; b < bande.size(); ++b) {
        const int y_hi = static_cast<int>(y_frequenza(c, bande[b].f_hi_hz)), y_lo = static_cast<int>(y_frequenza(c, bande[b].f_lo_hz));
        for (int x = c.x; x < c.x + c.w; x += 4) {
            t.pixel(x, y_hi, colori::GRIGIO_SCURO);
            t.pixel(x, y_lo, colori::GRIGIO_SCURO);
        }
        std::snprintf(buf, sizeof buf, "B%zu", b + 1);
        f.scrivi(t, c.x + 3, (y_hi + y_lo) / 2 - 3, buf, static_cast<std::uint8_t>(tenue), 1);
    }
    for (const auto& q : scelti) {   // la traccia scelta: le tacche cerchiate di bianco
        t.linea_orizzontale(q.x, q.x + q.w - 1, q.y, colori::BIANCO);
        t.linea_orizzontale(q.x, q.x + q.w - 1, q.y + q.h - 1, colori::BIANCO);
        t.linea_verticale(q.x, q.y, q.y + q.h - 1, colori::BIANCO);
        t.linea_verticale(q.x + q.w - 1, q.y, q.y + q.h - 1, colori::BIANCO);
    }
    t.imposta_ritaglio(a);

    // la lista delle tracce, in basso
    const int y_lista = a.y + ALTO_ALLARME + ALTO_CENTRO + 4;
    t.linea_orizzontale(a.x, a.x + a.w - 1, y_lista - 3, colori::GRIGIO_SCURO);
    static constexpr int COL[] = {6, 30, 66, 126, 172, 222, 290, 334, 368, 432, 520};
    static constexpr const char* TITOLI[] = {"N", "BRG", "FREQ", "DUR", "PRI", "DERIVA", "LIV", "MOD", "DOPPLER", "CLASSE", "ETA'"};
    for (int i = 0; i < 11; ++i) f.scrivi(t, a.x + COL[i], y_lista, TITOLI[i], static_cast<std::uint8_t>(tenue), 1);
    const int righe = (a.y + a.h - y_lista - RIGA_LISTA - 2) / RIGA_LISTA;
    // le righe attorno alla scelta, se la lista e' lunga
    int primo = 0;
    for (std::size_t k = 0; k < tracce.size(); ++k)
        if (tracce[k].id == scelta_) primo = std::max(0, static_cast<int>(k) - righe + 1);
    for (int i = primo; i < static_cast<int>(tracce.size()) && i < primo + righe; ++i) {
        const TracciaIntercetto& tr = tracce[static_cast<std::size_t>(i)];
        const int y = y_lista + RIGA_LISTA * (i - primo + 1);
        const bool su = tr.id == scelta_;
        if (su) t.rettangolo_pieno(a.x + 2, y - 1, a.w - 4, RIGA_LISTA, s.modo_1989 ? colori::VERDE : colori::GRIGIO_SCURO);
        const int col = viva(tr, ora) ? colore_allarme(tr.allarme, su && s.modo_1989 ? colori::NERO : testo) : tenue;
        const std::string celle[] = {std::to_string(tr.id), rilevamento(tr.rilevamento), frequenza(tr.f_hz), durata(tr.durata_s),
                                     cadenza(tr.pri_s), con_formato("%+.1f°/MIN", tr.deriva_deg_min), con_formato("%+.0f DB", tr.livello_db),
                                     modulazione(tr.fm), doppler(tr.doppler), classe_breve(tr.classe),
                                     con_formato("%.0f S", std::max(0.0, ora - tr.ultimo_t))};
        for (int k = 0; k < 11; ++k) f.scrivi(t, a.x + COL[k], y, celle[k], static_cast<std::uint8_t>(col), 1);
        area(a.x, y - 1, a.w, RIGA_LISTA, "intercetto_riga_" + std::to_string(i));
    }
    if (tracce.empty()) f.scrivi(t, a.x + 6, y_lista + RIGA_LISTA, "NESSUN IMPULSO INTERCETTATO", static_cast<std::uint8_t>(tenue), 1);
    t.togli_ritaglio();
    aree_ = nullptr;
}

}  // namespace m688
