#include "statistiche.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <limits>

#include "carta.hpp"
#include "grafica/lamiera.hpp"
#include "grafica/quadro.hpp"
#include "grafica/tema_stazioni.hpp"
#include "nucleo/geometria.hpp"
#include "replay.hpp"

namespace m688 {

namespace {

constexpr int TESTATA = 14;
constexpr int SX = 6, SW = 306;     // la colonna di sinistra
constexpr int DX = 322, DW = 312;   // la colonna di destra
constexpr int CALORE = 232, N_CALORE = 16;   // la rampa della heatmap: dal blu al rosso
constexpr int RIGA = 10;

void linea(Tela& t, Vec2 a, Vec2 b, std::uint8_t c, int tratto = 0) {
    const int n = static_cast<int>(std::max(std::fabs(b.x - a.x), std::fabs(b.y - a.y)));
    for (int i = 0; i <= n; ++i) {
        if (tratto > 0 && (i / tratto) % 2) continue;
        const double s = n ? static_cast<double>(i) / n : 0.0;
        t.pixel(static_cast<int>(std::lround(a.x + (b.x - a.x) * s)), static_cast<int>(std::lround(a.y + (b.y - a.y) * s)), c);
    }
}

std::string orologio(double s) {
    const int x = static_cast<int>(std::max(0.0, s));
    char b[16];
    std::snprintf(b, sizeof b, "%d:%02d:%02d", x / 3600, x / 60 % 60, x % 60);
    return b;
}

std::string ore_minuti(double s) {   // le durate lunghe delle cumulate
    const int m = static_cast<int>(std::lround(std::max(0.0, s) / 60.0));
    char b[24];
    std::snprintf(b, sizeof b, "%d H %02d MIN", m / 60, m % 60);
    return b;
}

std::string numero(double v, int decimali = 0) {
    char b[32];
    std::snprintf(b, sizeof b, "%.*f", decimali, v);
    std::string s = b;
    for (auto& c : s)
        if (c == '.') c = ',';
    return s;
}

std::string maiu(std::string s) {
    for (auto& c : s) c = c == '_' ? ' ' : static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return s;
}

std::string taglia(const Font& f, std::string s, int w) {
    while (!s.empty() && f.larghezza(s, 1) > w) s.pop_back();
    return s;
}

void titolo_riquadro(Tela& t, const Font& f, Rettangolo r, const std::string& titolo) {
    f.scrivi(t, r.x, r.y, titolo, colori::CIANO_CHIARO, 1);
    t.linea_orizzontale(r.x, r.x + r.w - 1, r.y + 9, colori::GRIGIO_SCURO);
}

// una riga «etichetta ... valore» su una colonna larga w
void voce(Tela& t, const Font& f, int x, int y, int w, const std::string& etichetta, const std::string& valore,
          std::uint8_t col = colori::BIANCO) {
    f.scrivi(t, x, y, taglia(f, etichetta, w - f.larghezza(valore, 1) - 6), colori::GRIGIO, 1);
    f.scrivi(t, x + w - f.larghezza(valore, 1), y, valore, col, 1);
}

std::string come_scoperti(const std::string& come) {
    if (come == "avvistato") return "CI HA AVVISTATI";
    if (come == "fix_radiogoniometria") return "FIX RADIOGONIOMETRICO DOPO UNA TRASMISSIONE";
    if (come == "ping_subito") return "CI HA TROVATI COL SONAR ATTIVO";
    return maiu(come);
}

// i segni dell'asse del tempo: ogni 5, 10, 15, 30 o 60 minuti, perche' ce ne stiano circa sei
double passo_tempo(double durata) {
    for (double p : {300.0, 600.0, 900.0, 1800.0, 3600.0})
        if (durata / p <= 7.0) return p;
    return 7200.0;
}

void asse_tempo(Tela& t, const Font& f, Rettangolo g, double durata) {
    if (durata <= 0.0) return;
    const double p = passo_tempo(durata);
    for (double s = 0.0; s <= durata + 1e-6; s += p) {
        const int x = g.x + static_cast<int>(std::lround(s / durata * (g.w - 1)));
        t.linea_verticale(x, g.y + g.h, g.y + g.h + 2, colori::GRIGIO);
        const std::string e = std::to_string(static_cast<int>(s / 60.0)) + "'";
        const int xe = std::clamp(x - f.larghezza(e, 1) / 2, g.x, g.x + g.w - f.larghezza(e, 1));
        f.scrivi(t, xe, g.y + g.h + 3, e, colori::GRIGIO, 1);
    }
}

void cornice_grafico(Tela& t, Rettangolo g) {
    t.linea_orizzontale(g.x, g.x + g.w - 1, g.y + g.h, colori::GRIGIO);
    t.linea_verticale(g.x - 1, g.y, g.y + g.h, colori::GRIGIO);
}

// i testi sul fosforo (font a celle variabili, formato 2), come grafica/quadro
int largo_q(const Font& f, std::string_view s) { return f.larghezza(s, 1, f.formato() == 2 ? f.spazio_predefinito() : 1, f.formato() != 2); }
void scrivi_q(Tela& t, const Font& f, int x, int y, std::string_view s, std::uint8_t c) {
    f.scrivi(t, x, y, s, c, 1, f.formato() == 2 ? f.spazio_predefinito() : 1, f.formato() != 2);
}
std::string taglia_q(const Font& f, std::string s, int w) {
    while (!s.empty() && largo_q(f, s) > w) s.pop_back();
    return s;
}

// La heatmap sulla carta del teatro: le celle colorate dal tempo passato (radice dei secondi, cosi' le soste lunghe
// non spengono il resto), retinate a meta' perche' si veda il fondale sotto; la vista e' il riquadro delle celle
void disegna_heatmap(Tela& t, const Font& f, Rettangolo r, const Fondale* fondale, FondaleVga& vga, const Heatmap& h,
                     double cella, const std::string& sotto) {
    const Rettangolo carta{r.x, r.y, r.w, r.h - 24};
    GeometriaCarta g;
    g.x = carta.x;
    g.y = carta.y;
    g.w = carta.w;
    g.h = carta.h;
    double smax = 0.0;
    int x0 = std::numeric_limits<int>::max(), y0 = x0, x1 = std::numeric_limits<int>::min(), y1 = x1;
    for (const auto& [c, s] : h) {
        x0 = std::min(x0, c.first);
        x1 = std::max(x1, c.first);
        y0 = std::min(y0, c.second);
        y1 = std::max(y1, c.second);
        smax = std::max(smax, s);
    }
    if (h.empty()) {
        g.centro = {};   // il centro del teatro
        g.metri_per_pixel = 150.0;
    } else {
        g.centro = {(x0 + x1 + 1) * cella / 2.0, (y0 + y1 + 1) * cella / 2.0};
        const double larga = (x1 - x0 + 1) * cella, alta = (y1 - y0 + 1) * cella;
        g.metri_per_pixel = std::max({larga / carta.w * 1.3, alta / carta.h * 1.3, 15000.0 / carta.w});   // almeno 15 km
    }
    if (fondale) {
        OpzioniCarta o;
        o.stile = StileCarta::vga;
        vga.disegna(t, g, *fondale, o);
    } else {
        t.rettangolo_pieno(carta.x, carta.y, carta.w, carta.h, static_cast<std::uint8_t>(colori_carta::ACQUA + 30));
    }
    t.imposta_ritaglio(carta);
    for (const auto& [c, s] : h) {
        const Vec2 a = g.a_tela({c.first * cella, (c.second + 1) * cella}), b = g.a_tela({(c.first + 1) * cella, c.second * cella});
        const int k = std::clamp(static_cast<int>(std::sqrt(s / smax) * N_CALORE), 0, N_CALORE - 1);
        const auto col = static_cast<std::uint8_t>(CALORE + k);
        const int xa = static_cast<int>(std::floor(a.x)), ya = static_cast<int>(std::floor(a.y));
        const int xb = std::max(xa + 1, static_cast<int>(std::ceil(b.x))), yb = std::max(ya + 1, static_cast<int>(std::ceil(b.y)));
        const bool retina = xb - xa >= 3 && k < N_CALORE - 4;   // le celle piu' calde piene, le altre a scacchiera
        for (int y = ya; y < yb; ++y)
            for (int x = xa; x < xb; ++x)
                if (!retina || (x + y) % 2 == 0) t.pixel(x, y, col);
    }
    t.togli_ritaglio();
    t.linea_orizzontale(carta.x, carta.x + carta.w - 1, carta.y - 1, colori::GRIGIO_SCURO);
    t.linea_orizzontale(carta.x, carta.x + carta.w - 1, carta.y + carta.h, colori::GRIGIO_SCURO);
    // la legenda: la rampa, il massimo, il lato della cella
    const int yl = carta.y + carta.h + 4;
    f.scrivi(t, r.x, yl, "POCO", colori::GRIGIO, 1);
    const int xr = r.x + f.larghezza("POCO ", 1);
    for (int k = 0; k < N_CALORE; ++k) t.rettangolo_pieno(xr + 6 * k, yl, 6, 7, static_cast<std::uint8_t>(CALORE + k));
    f.scrivi(t, xr + 6 * N_CALORE + 4, yl, "MOLTO", colori::GRIGIO, 1);
    const std::string mx = h.empty() ? "NESSUN DATO" : "MAX " + orologio(smax);
    f.scrivi(t, r.x + r.w - f.larghezza(mx, 1), yl, mx, colori::GRIGIO, 1);
    f.scrivi(t, r.x, yl + RIGA + 1, taglia(f, "CELLE DI " + numero(cella / 1000.0) + " KM, " + sotto, r.w), colori::GRIGIO, 1);
}

// --- la pagina «IMPRESE» (le voci di docs/26 §3 e del secondo giro del 31/10/2026: ghiaccio, notte, coda, mine, ASW
// aereo, soccorso, riconoscimento), uguale per il report di una missione e per le cumulate ---

struct VoceImpresa {
    std::string sezione, etichetta, valore;
    std::uint8_t colore = 0;   // 0: il fosforo normale
};
using Imprese = std::vector<VoceImpresa>;

std::string sensore_mine(const std::string& s) {
    return s == "bqs15" ? "BQS-15" : s == "bqs8" ? "BQS-8" : s == "mg24" ? "MG-24" : s == "mg70" ? "MG-70" : maiu(s);
}
std::string su(int a, int b) { return std::to_string(a) + " / " + std::to_string(b); }
std::string metri_o_trattino(double m) { return m > 0.0 ? numero(m) + " M" : std::string("-"); }

// le due colonne della pagina dalle stesse grandezze del report e delle cumulate (i nomi uguali nei due tipi; quelli
// diversi, il tratto in coda e gli errori medi, come argomenti)
template <class S>
std::pair<Imprese, Imprese> imprese_di(const S& s, double in_coda_s, double errore_telemetro_pct, double errore_aob_deg, double errore_navigazione_m,
                                       bool cumulate) {
    const int classi = static_cast<int>(s.classi_giuste.size());
    Imprese a, b;
    const auto verde = [](int n) { return n > 0 ? std::uint8_t{colori::VERDE_CHIARO} : std::uint8_t{0}; };
    a.push_back({"GHIACCIO", "SOTTO LA BANCHISA", cumulate ? ore_minuti(s.sotto_banchisa_s) : orologio(s.sotto_banchisa_s)});
    a.push_back({"GHIACCIO", "URTI COL GHIACCIO", std::to_string(s.urti_ghiaccio), s.urti_ghiaccio ? std::uint8_t{12} : std::uint8_t{0}});
    a.push_back({"GHIACCIO", "EMERSIONI NEL GHIACCIO", std::to_string(s.emersioni_ghiaccio)});
    a.push_back({"GHIACCIO", "AVVISI DELLA CHIGLIA", std::to_string(s.avvisi_chiglia)});
    a.push_back({"NOTTE", "IDENTIFICATI AI SOLI FANALI", std::to_string(s.visive_notte_fanali), verde(s.visive_notte_fanali)});
    a.push_back({"NOTTE", "IDENTIFICATI A LUCI SPENTE", std::to_string(s.visive_luci_spente), verde(s.visive_luci_spente)});
    a.push_back({"NOTTE", "LE TUE LUCI ACCESE", cumulate ? ore_minuti(s.luci_accese_s) : orologio(s.luci_accese_s)});
    a.push_back({"PEDINAMENTO", cumulate ? "IN CODA PIU' A LUNGO (PRIMATO)" : "IN CODA PIU' A LUNGO", orologio(in_coda_s),
                 cumulate && in_coda_s > 0.0 ? std::uint8_t{colori::GIALLO} : std::uint8_t{0}});
    a.push_back({"PEDINAMENTO", "SOTTOMARINO STRANIERO PIU' VICINO", metri_o_trattino(s.distanza_min_sottomarino_m),
                 s.distanza_min_sottomarino_m > 0.0 && s.distanza_min_sottomarino_m < 1000.0 ? std::uint8_t{colori::GIALLO} : std::uint8_t{0}});
    a.push_back({"PEDINAMENTO", "CLASSIFICATI DA UN SOTTOMARINO", std::to_string(s.classificati_da_sottomarini),
                 s.classificati_da_sottomarini ? std::uint8_t{colori::GIALLO} : std::uint8_t{0}});
    a.push_back({"SOCCORSO", "UOMINI SALVATI", std::to_string(s.salvati), verde(s.salvati)});
    a.push_back({"RICONOSCIMENTO", "CLASSI DIVERSE RICONOSCIUTE", std::to_string(classi), verde(classi)});
    a.push_back({"RICONOSCIMENTO", "A VISTA (GIUSTE / ERRATE)", su(s.visive_giuste, s.visive_errate)});
    a.push_back({"RICONOSCIMENTO", "TELEMETRO (MISURE, ERRORE)", std::to_string(s.misure_telemetro) + ", " + numero(errore_telemetro_pct, 1) + " %"});
    a.push_back({"RICONOSCIMENTO", "AOB (OSSERVAZIONI, ERRORE)", std::to_string(s.osservazioni_aob) + ", " + numero(errore_aob_deg, 1) + " GRADI"});
    int mine = 0;
    for (const auto& [x, n] : s.mine_per_sensore) mine += n;
    b.push_back({"MINE", "SCOPERTE", std::to_string(std::max(mine, s.mine_scoperte))});
    for (const auto& [sensore, n] : s.mine_per_sensore) b.push_back({"MINE", "  CON " + sensore_mine(sensore), std::to_string(n)});
    b.push_back({"MINE", "ORMEGGIATE / DI FONDO", su(s.mine_ormeggiate, s.mine_di_fondo)});
    b.push_back({"MINE", "AGGIRATE", std::to_string(s.mine_aggirate), verde(s.mine_aggirate)});
    b.push_back({"MINE", "ATTRAVERSAMENTI DI UN CAMPO", std::to_string(s.attraversamenti_mine)});
    b.push_back({"ASW AEREO", "SORVOLI DI ELICOTTERI / AEREI", su(s.sorvoli_elicotteri, s.sorvoli_aerei)});
    b.push_back({"ASW AEREO", "SILURI AEREI EVITATI", std::to_string(s.siluri_aerei_evitati), verde(s.siluri_aerei_evitati)});
    b.push_back({"ASW AEREO", "ALLARMI SILURO / EVITATI", su(s.allarmi_siluro, s.siluri_evitati_allarme)});
    b.push_back({"ARMI E NAVIGAZIONE", "MISSILI LANCIATI / A SEGNO", su(s.missili_lanciati, s.missili_a_segno)});
    b.push_back({"ARMI E NAVIGAZIONE", "NEUTRALI NELLA SCATOLA", std::to_string(s.neutrali_in_scatola),
                 s.neutrali_in_scatola ? std::uint8_t{12} : std::uint8_t{0}});
    b.push_back({"ARMI E NAVIGAZIONE", "PUNTI NAVE (ERRORE MEDIO)", std::to_string(s.punti_nave) + ", " + metri_o_trattino(errore_navigazione_m)});
    b.push_back({"ARMI E NAVIGAZIONE", "PING DELL'ECOSCANDAGLIO", std::to_string(s.ping_ecoscandaglio)});
    b.push_back({"ARMI E NAVIGAZIONE", "VERIFICHE RADIO / FALSI SCOPERTI", su(s.verifiche_radio, s.non_autentici)});
    b.push_back({"ARMI E NAVIGAZIONE", "COLLISIONI / SFIORATE", su(s.collisioni, s.quasi_collisioni),
                 s.collisioni ? std::uint8_t{12} : std::uint8_t{0}});
    b.push_back({"ARMI E NAVIGAZIONE", "COLPI D'ARTIGLIERIA SUBITI", std::to_string(s.colpi_artiglieria),
                 s.colpi_artiglieria ? std::uint8_t{12} : std::uint8_t{0}});
    return {a, b};
}

// la consolle della pagina: due schermi a fosforo, le sezioni con l'intestazione, le righe a 10 px velate alterne; senza
// missioni i trattini spenti al posto degli zeri
void disegna_imprese(Tela& t, const Font& targa, const Font& g, const std::string& posto, const std::string& titolo, const std::string& destra,
                     const std::pair<Imprese, Imprese>& v, bool vuote) {
    namespace q = quadro;
    const Rettangolo u = q::consolle(t, targa, posto);
    const auto schermo = [&](Rettangolo r, const std::string& tit, const std::string& dx, const Imprese& voci) {
        const Rettangolo a = q::schermo(t, r, g, taglia_q(g, tit, r.w - 30 - largo_q(g, dx)), dx);
        int y = a.y + 2, k = 0;
        std::string sezione;
        for (const auto& x : voci) {
            if (x.sezione != sezione) {
                if (!sezione.empty()) y += 4;
                if (y + 22 > a.y + a.h) break;
                y += q::intestazione(t, a, y, g, x.sezione);
                sezione = x.sezione;
                k = 0;
            }
            if (y + 10 > a.y + a.h - 2) break;
            q::riga(t, a, y, g, taglia_q(g, x.etichetta, a.w - 92), {}, {}, k++ % 2 == 1);
            const std::string val = vuote ? std::string("-") : x.valore;
            scrivi_q(t, g, a.x + a.w - 4 - largo_q(g, val), y, val, vuote ? fosforo(3) : x.colore ? x.colore : fosforo(6));
            y += 10;
        }
    };
    const int meta = (u.w - 4) / 2;
    schermo({u.x, u.y, meta, 424 - u.y}, titolo, destra, v.first);
    schermo({u.x + meta + 4, u.y, u.w - meta - 4, 424 - u.y}, "MINE, AEREI, ARMI E NAVIGAZIONE", {}, v.second);
}

void palette_statistiche(Palette& p) {
    applica_palette_carta(p, StileCarta::vga);
    applica_palette_rilievo(p, StileCarta::vga);
    // blu, ciano, verde, giallo, rosso: 16 passi
    const int tappe[5][3] = {{20, 40, 160}, {0, 170, 220}, {40, 200, 60}, {240, 220, 40}, {230, 30, 20}};
    for (int k = 0; k < N_CALORE; ++k) {
        const double u = 4.0 * k / (N_CALORE - 1);
        const int a = std::min(3, static_cast<int>(u));
        const double s = u - a;
        const auto mix = [&](int c) { return static_cast<std::uint8_t>(std::lround(tappe[a][c] + (tappe[a + 1][c] - tappe[a][c]) * s)); };
        p.imposta(CALORE + k, mix(0), mix(1), mix(2));
    }
}

}  // namespace

// ---------------------------------------------------------------------------------------------------------------
// Una missione

SchermataStatistiche::SchermataStatistiche(std::string dati, ReportMissione report, Traccia traccia)
    : dati_(std::move(dati)), r_(std::move(report)), tr_(std::move(traccia)) {
    teatro_ = r_.teatro;
    if (teatro_.empty())
        if (const auto it = tr_.metadati.find("teatro"); it != tr_.metadati.end()) teatro_ = it->second;
    {
        const std::string dal_file = teatro_dal_file(dati_, tr_.missione, termoclini_);
        if (teatro_.empty()) teatro_ = dal_file;
    }
    if (!teatro_.empty()) {
        try {
            fondale_ = std::make_unique<Fondale>(Fondale::carica(dati_ + "/teatri/" + teatro_ + ".fdl"));
        } catch (const std::exception&) {
            fondale_.reset();
        }
    }
    if (const auto it = tr_.metadati.find("giocatore"); it != tr_.metadati.end()) giocatore_id_ = std::atoi(it->second.c_str());
    if (const auto it = tr_.unita.find(giocatore_id_); it != tr_.unita.end())
        for (const auto& [t, u] : it->second) quote_.push_back({t, u.quota});
    for (const auto& e : tr_.registro) {
        if (e.tipo == "ping_attivo") ping_.push_back(e.tempo);
        else if (e.tipo == "trasmissione") trasmissioni_.push_back(e.tempo);
    }
    try {
        targa_ = Font::carica(dati_ + "/font/incisione688_7.fnt");
        fosforo_ = Font::carica(dati_ + "/font/fosforo688_5x7.fnt");
    } catch (const std::exception&) {
        targa_.reset();
        fosforo_.reset();
    }
}

bool SchermataStatistiche::comandi(const Comandi& c, double) {
    if (c.esci || c.esc || c.invio) return true;
    if (c.tab != 0) pagina_ = 1 - pagina_;
    for (const auto& [x, y] : c.clic)
        if (pagina_ == 1 && y >= 428 + 22 && y < 428 + 48) {   // i tasti del piede delle imprese: TAB il report, ESC indietro
            if (x >= 324) return true;
            pagina_ = 0;
        }
    return false;
}

void SchermataStatistiche::palette(Palette& p) const {
    palette_statistiche(p);
    applica_tema(p, TemaStazione{});   // 16-47: la lamiera e il fosforo della pagina delle imprese
}

void SchermataStatistiche::disegna(Tela& t, const Font& f) const {
    if (pagina_ == 1) {   // le imprese, nella variante C
        disegna_imprese(t, targa_ ? *targa_ : f, fosforo_ ? *fosforo_ : f, "SALA ANALISI - IMPRESE",
                        "LA MISSIONE - " + maiu(r_.missione.empty() ? tr_.missione : r_.missione), orologio(r_.durata_s),
                        imprese_di(r_, r_.in_coda_s, r_.errore_telemetro_pct, r_.errore_aob_deg, r_.errore_navigazione_medio_m, false), false);
        lamiera::piede(t, f, 428, "TAB IL REPORT DELLA MISSIONE   ESC O INVIO: INDIETRO",
                       {{"TAB  REPORT", ColoreLampada::ambra}, {"ESC  INDIETRO", ColoreLampada::verde}}, 1);
        return;
    }
    t.riempi(colori::NERO);
    std::string titolo = "STATISTICHE - " + maiu(r_.missione.empty() ? tr_.missione : r_.missione);
    if (const auto it = tr_.metadati.find("comandante"); it != tr_.metadati.end()) titolo += " - " + maiu(it->second);
    f.scrivi(t, 6, 3, taglia(f, titolo, 380), colori::GIALLO, 1);
    const std::string destra = maiu(r_.classe) + (teatro_.empty() ? "" : "  " + maiu(teatro_)) + "  " + orologio(r_.durata_s);
    f.scrivi(t, LARGHEZZA_TELA - 6 - f.larghezza(destra, 1), 3, destra, colori::CIANO_CHIARO, 1);
    t.linea_orizzontale(0, LARGHEZZA_TELA - 1, TESTATA, colori::GRIGIO_SCURO);
    disegna_quota(t, f, {SX, 20, SW, 112});
    disegna_livello(t, f, {SX, 138, SW, 112});
    disegna_armi(t, f, {SX, 256, SW, 118});
    titolo_riquadro(t, f, {DX, 20, DW, 10}, "DOVE SEI STATO");
    disegna_heatmap(t, f, {DX, 32, DW, 230}, fondale_.get(), vga_, r_.heatmap, CELLA_MISSIONE_M,
                    numero(r_.distanza_nm, 1) + " MIGLIA PERCORSE");
    disegna_scoperta(t, f, {DX, 268, DW, 106});
    disegna_tempi(t, f, {SX, 380, LARGHEZZA_TELA - 2 * SX, 80});
    f.scrivi(t, 6, 468, "LIVELLO: PING IN CIANO, TRASMISSIONI IN ROSSO.   TAB: LE IMPRESE.   ESC O INVIO: INDIETRO", colori::GRIGIO, 1);
}

void SchermataStatistiche::disegna_quota(Tela& t, const Font& f, Rettangolo r) const {
    titolo_riquadro(t, f, r, "QUOTA NEL TEMPO");
    const std::string sopra = "MEDIA " + numero(r_.quota_media_m) + " M  MAX " + numero(r_.quota_max_m) + " M";
    f.scrivi(t, r.x + r.w - f.larghezza(sopra, 1), r.y, sopra, colori::GRIGIO, 1);
    const Rettangolo g{r.x + 30, r.y + 14, r.w - 32, r.h - 26};
    double massima = 50.0;
    for (const auto& [s, q] : quote_) massima = std::max(massima, q);
    massima = std::ceil(massima * 1.2 / 50.0) * 50.0;   // i termoclini piu' profondi della scala restano fuori
    const auto y_di = [&](double q) { return g.y + std::clamp(q / massima, 0.0, 1.0) * g.h; };
    const auto x_di = [&](double s) { return g.x + (r_.durata_s > 0.0 ? s / r_.durata_s : 0.0) * (g.w - 1); };
    // la fascia periscopica, retinata (almeno 4 pixel: nella scala della missione e' di 2-7,1 m); la superficie
    const int y_peri0 = static_cast<int>(y_di(QUOTA_SUPERFICIE_M));
    const int y_peri1 = std::max(static_cast<int>(y_di(QUOTA_PERISCOPICA_MAX_M)), y_peri0 + 3);
    for (int y = y_peri0; y <= y_peri1; ++y)
        for (int x = g.x; x < g.x + g.w; ++x)
            if ((x + y) % 2 == 0) t.pixel(x, y, colori::BLU);
    f.scrivi(t, g.x + 2, y_peri1 + 1, "PERISCOPICA", colori::GRIGIO_SCURO, 1);
    for (double tc : termoclini_)
        if (tc < massima) linea(t, {static_cast<double>(g.x), y_di(tc)}, {static_cast<double>(g.x + g.w - 1), y_di(tc)}, colori::CIANO, 3);
    for (double q : {0.0, massima / 2.0, massima}) {
        const std::string e = numero(q);
        f.scrivi(t, g.x - 4 - f.larghezza(e, 1), std::clamp(static_cast<int>(y_di(q)) - 4, g.y, g.y + g.h - 8), e, colori::GRIGIO, 1);
    }
    for (std::size_t i = 1; i < quote_.size(); ++i)
        linea(t, {x_di(quote_[i - 1].first), y_di(quote_[i - 1].second)}, {x_di(quote_[i].first), y_di(quote_[i].second)}, colori::VERDE_CHIARO);
    cornice_grafico(t, g);
    asse_tempo(t, f, g, r_.durata_s);
}

void SchermataStatistiche::disegna_livello(Tela& t, const Font& f, Rettangolo r) const {
    titolo_riquadro(t, f, r, "LIVELLO IRRADIATO (DB)");
    const Rettangolo g{r.x + 30, r.y + 14, r.w - 32, r.h - 26};
    const auto& lv = r_.livello_irradiato;
    if (lv.empty()) {
        f.scrivi(t, g.x, g.y + g.h / 2 - 4, "SENZA LA FIRMA DELLA CLASSE", colori::GRIGIO, 1);
        cornice_grafico(t, g);
        return;
    }
    const std::string sopra = "MEDIO " + numero(r_.livello_medio_db) + "  MAX " + numero(r_.livello_max_db);
    f.scrivi(t, r.x + r.w - f.larghezza(sopra, 1), r.y, sopra, colori::GRIGIO, 1);
    double lo = lv.front().second, hi = lo;
    for (const auto& [s, l] : lv) {
        lo = std::min(lo, l);
        hi = std::max(hi, l);
    }
    lo = std::floor((lo - 3.0) / 5.0) * 5.0;
    hi = std::ceil((hi + 3.0) / 5.0) * 5.0;
    const auto y_di = [&](double l) { return g.y + g.h - (l - lo) / std::max(1.0, hi - lo) * g.h; };
    const auto x_di = [&](double s) { return g.x + (r_.durata_s > 0.0 ? s / r_.durata_s : 0.0) * (g.w - 1); };
    for (double l : {lo, hi}) {
        const std::string e = numero(l);
        f.scrivi(t, g.x - 4 - f.larghezza(e, 1), std::clamp(static_cast<int>(y_di(l)) - 4, g.y, g.y + g.h - 8), e, colori::GRIGIO, 1);
    }
    linea(t, {static_cast<double>(g.x), y_di(r_.livello_medio_db)}, {static_cast<double>(g.x + g.w - 1), y_di(r_.livello_medio_db)},
          colori::GRIGIO_SCURO, 2);
    for (std::size_t i = 1; i < lv.size(); ++i)
        linea(t, {x_di(lv[i - 1].first), y_di(lv[i - 1].second)}, {x_di(lv[i].first), y_di(lv[i].second)}, colori::GIALLO);
    // i segni: i ping in ciano dal basso, le trasmissioni in rosso dall'alto
    for (double s : ping_) t.linea_verticale(static_cast<int>(x_di(s)), g.y + g.h - 8, g.y + g.h - 1, colori::CIANO_CHIARO);
    for (double s : trasmissioni_) t.linea_verticale(static_cast<int>(x_di(s)), g.y, g.y + 7, colori::ROSSO);
    cornice_grafico(t, g);
    asse_tempo(t, f, g, r_.durata_s);
}

void SchermataStatistiche::disegna_armi(Tela& t, const Font& f, Rettangolo r) const {
    titolo_riquadro(t, f, r, "ARMI");
    const std::string sopra = std::to_string(r_.lanci) + (r_.lanci == 1 ? " LANCIO, " : " LANCI, ") + std::to_string(r_.colpi) +
                              " A SEGNO, " + std::to_string(r_.affondati_ostili) + (r_.affondati_ostili == 1 ? " OSTILE AFFONDATO" : " OSTILI AFFONDATI");
    f.scrivi(t, r.x + r.w - f.larghezza(sopra, 1), r.y, taglia(f, sopra, r.w - 40), colori::GRIGIO, 1);
    int y = r.y + 14;
    const int meta = r.x + r.w / 2;
    f.scrivi(t, r.x, y, "PER CLASSE", colori::BIANCO, 1);
    f.scrivi(t, meta + 4, y, "PER BERSAGLIO", colori::BIANCO, 1);
    y += RIGA + 2;
    int ya = y;
    if (r_.per_arma.empty()) f.scrivi(t, r.x, ya, "NESSUN LANCIO", colori::GRIGIO, 1);
    for (const auto& [arma, c] : r_.per_arma) {
        if (ya > r.y + r.h - RIGA) break;
        voce(t, f, r.x, ya, r.w / 2 - 6, arma.empty() ? std::string("ARMA SCONOSCIUTA") : maiu(arma),
             std::to_string(c.lanci) + " / " + std::to_string(c.colpi),
             c.colpi ? colori::VERDE_CHIARO : colori::BIANCO);
        ya += RIGA;
    }
    int yb = y;
    if (r_.per_bersaglio.empty()) f.scrivi(t, meta + 4, yb, "NESSUNO COLPITO", colori::GRIGIO, 1);
    for (const auto& [id, b] : r_.per_bersaglio) {
        if (yb > r.y + r.h - RIGA) break;
        voce(t, f, meta + 4, yb, r.w / 2 - 4, maiu(b.classe.empty() ? "UNITA' " + std::to_string(id) : b.classe),
             (b.affondato ? "AFFONDATO" : std::to_string(b.colpi) + (b.colpi == 1 ? " COLPO" : " COLPI")),
             b.affondato ? colori::ROSSO : colori::GIALLO);
        yb += RIGA;
    }
    t.linea_verticale(meta, y, r.y + r.h - 2, colori::GRIGIO_SCURO);
}

void SchermataStatistiche::disegna_scoperta(Tela& t, const Font& f, Rettangolo r) const {
    titolo_riquadro(t, f, r, "LA PRIMA VOLTA CHE IL NEMICO TI HA TROVATO");
    int y = r.y + 14;
    if (!r_.prima_scoperta) {
        f.scrivi(t, r.x, y, "MAI: NESSUN AVVISTAMENTO, FIX O PING SUBITO", colori::VERDE_CHIARO, 1);
        y += RIGA + 4;
    } else {
        const auto& s = *r_.prima_scoperta;
        voce(t, f, r.x, y, r.w, "QUANDO", orologio(s.tempo), colori::GIALLO);
        voce(t, f, r.x, y += RIGA, r.w, "CHI", maiu(s.classe.empty() ? std::string("SCONOSCIUTO") : s.classe), colori::GIALLO);
        voce(t, f, r.x, y += RIGA, r.w, "COME", taglia(f, come_scoperti(s.come), r.w - 40), colori::GIALLO);
        voce(t, f, r.x, y += RIGA, r.w, "A CHE DISTANZA",
             s.distanza_m > 0.0 ? numero(s.distanza_m / 1000.0, 1) + " KM (" + numero(s.distanza_m / MIGLIO, 1) + " NM)" : std::string("-"),
             colori::GIALLO);
        y += RIGA + 4;
    }
    voce(t, f, r.x, y, r.w, "SCOPERTE NEMICHE IN TUTTO", std::to_string(r_.scoperte_nemiche));
    voce(t, f, r.x, y += RIGA, r.w, "CONTATTI / CLASSIFICATI", std::to_string(r_.contatti) + " / " + std::to_string(r_.classificati));
    voce(t, f, r.x, y += RIGA, r.w, "CLASSIFICAZIONI GIUSTE / ERRATE", std::to_string(r_.giuste) + " / " + std::to_string(r_.errate),
         r_.errate ? colori::GIALLO : colori::BIANCO);
    if (r_.tempo_classificazione_s > 0.0)
        voce(t, f, r.x, y += RIGA, r.w, "TEMPO MEDIO PER CLASSIFICARE", orologio(r_.tempo_classificazione_s));
}

void SchermataStatistiche::disegna_tempi(Tela& t, const Font& f, Rettangolo r) const {
    titolo_riquadro(t, f, r, "TEMPI ED ESPOSIZIONI");
    const int w = (r.w - 24) / 3;
    const auto colonna = [&](int k, const std::vector<std::pair<std::string, std::string>>& v) {
        int y = r.y + 14;
        for (const auto& [e, x] : v) {
            voce(t, f, r.x + k * (w + 12), y, w, e, x);
            y += RIGA;
        }
    };
    const double d = r_.durata_s;
    colonna(0, {{"DURATA", orologio(d)},
                {"DISTANZA", numero(r_.distanza_nm, 1) + " NM"},
                {"VELOCITA' MEDIA", numero(r_.velocita_media_kn, 1) + " KN"},
                {"VELOCITA' MAX", numero(r_.velocita_max_kn, 1) + " KN"},
                {"CAVITAZIONE", orologio(r_.cavitazione_s)}});
    colonna(1, {{"IN SUPERFICIE", orologio(r_.superficie_s)},
                {"A QUOTA PERISCOPICA", orologio(r_.periscopica_s)},
                {"QUOTA MEDIA", numero(r_.quota_media_m) + " M"},
                {"QUOTA MAX", numero(r_.quota_max_m) + " M"},
                {"LIVELLO MEDIO", r_.livello_medio_db > 0.0 ? numero(r_.livello_medio_db) + " DB" : "-"}});
    colonna(2, {{"PERISCOPIO ALZATO", orologio(r_.periscopio_s)},
                {"ANTENNA ALZATA", orologio(r_.antenna_s)},
                {"SCHIERA FILATA", orologio(r_.schiera_s)},
                {"TRASMISSIONI", std::to_string(r_.trasmissioni)},
                {"PING ATTIVI", std::to_string(r_.ping)}});
}

// ---------------------------------------------------------------------------------------------------------------
// Le cumulate

SchermataStatisticheCumulate::SchermataStatisticheCumulate(std::string dati, StatisticheCumulate statistiche, std::string chi)
    : dati_(std::move(dati)), chi_(std::move(chi)), s_(std::move(statistiche)) {
    try {
        targa_ = Font::carica(dati_ + "/font/incisione688_7.fnt");
        fosforo_ = Font::carica(dati_ + "/font/fosforo688_5x7.fnt");
    } catch (const std::exception&) {
        targa_.reset();
        fosforo_.reset();
    }
    for (const auto& [teatro, h] : s_.heatmap)
        if (!h.empty()) teatri_.push_back(teatro);
    // prima il teatro dove si e' passato piu' tempo
    std::stable_sort(teatri_.begin(), teatri_.end(), [&](const std::string& a, const std::string& b) {
        const auto tot = [&](const std::string& k) {
            double x = 0.0;
            for (const auto& [c, v] : s_.heatmap.at(k)) x += v;
            return x;
        };
        return tot(a) > tot(b);
    });
    carica_fondale();
}

const std::string& SchermataStatisticheCumulate::teatro() const {
    static const std::string nessuno;
    return teatri_.empty() ? nessuno : teatri_[static_cast<std::size_t>(scelto_)];
}

void SchermataStatisticheCumulate::scegli_teatro(int k) {
    if (teatri_.empty()) return;
    scelto_ = ((k % static_cast<int>(teatri_.size())) + static_cast<int>(teatri_.size())) % static_cast<int>(teatri_.size());
    carica_fondale();
}

void SchermataStatisticheCumulate::carica_fondale() {
    fondale_.reset();
    vga_.invalida();
    if (teatro().empty()) return;
    try {
        fondale_ = std::make_unique<Fondale>(Fondale::carica(dati_ + "/teatri/" + teatro() + ".fdl"));
    } catch (const std::exception&) {
        fondale_.reset();
    }
}

bool SchermataStatisticheCumulate::comandi(const Comandi& c, double) {
    if (c.esci || c.esc || c.invio) return true;
    for (const int k : c.cifre)   // 1 e 2: la pagina
        if (k == 1 || k == 2) pagina_ = k - 1;
    if (pagina_ == 1) {   // le imprese: TAB (anche il tasto del piede) torna alle statistiche
        if (c.tab != 0) pagina_ = 0;
        for (const auto& [x, y] : c.clic)
            if (y >= 428 + 22 && y < 428 + 48) {
                if (x >= 324) return true;
                pagina_ = 0;
            }
        return false;
    }
    const int passo = c.tab != 0 ? c.tab : 0;
    if (passo) scegli_teatro(scelto_ + passo);
    for (const auto& [x, y] : c.clic) {
        if (x >= carta_.x && x < carta_.x + carta_.w && y >= carta_.y && y < carta_.y + carta_.h) scegli_teatro(scelto_ + 1);   // il teatro successivo
        if (y >= 428 + 22 && y < 428 + 48) {   // i tasti del piede: TAB l'altro teatro, ESC indietro
            if (x >= 324) return true;
            scegli_teatro(scelto_ + 1);
        }
    }
    return false;
}

void SchermataStatisticheCumulate::palette(Palette& p) const {
    palette_statistiche(p);
    applica_tema(p, TemaStazione{});   // 16-47: la lamiera e il fosforo della variante C
}

void SchermataStatisticheCumulate::disegna(Tela& t, const Font& f) const {
    // gruppo 4 della passata estetica (variante C, grafica/quadro): la consolle, a sinistra i numeri, a destra la carta
    namespace q = quadro;
    const Font& g = fosforo_ ? *fosforo_ : f;
    const bool vuote = s_.missioni == 0;
    const std::string destra = std::to_string(s_.missioni) + (s_.missioni == 1 ? " MISSIONE" : " MISSIONI");
    if (pagina_ == 1) {   // le imprese: i totali, e i primati del pedinamento e della distanza
        disegna_imprese(t, targa_ ? *targa_ : f, g, "SALA ANALISI - IMPRESE", maiu(chi_), destra,
                        imprese_di(s_, s_.in_coda_max_s, s_.errore_telemetro_pct(), s_.errore_aob_deg(), s_.errore_navigazione_m(), true), vuote);
        lamiera::piede(t, f, 428, "1 STATISTICHE   2 IMPRESE",
                       {{"TAB  STATISTICHE", ColoreLampada::ambra}, {"ESC  INDIETRO", ColoreLampada::verde}}, 1);
        return;
    }
    const Rettangolo u = q::consolle(t, targa_ ? *targa_ : f, "SALA ANALISI - STATISTICHE GLOBALI");
    const Rettangolo a = q::schermo(t, {u.x, u.y, 300, 424 - u.y}, g, taglia_q(g, maiu(chi_), 300 - 30 - largo_q(g, destra)), destra);
    const double d = s_.durata_s;
    const int n = std::max(1, s_.missioni);
    int y = a.y + 2, k = 0;
    const auto sezione = [&](const char* titolo) {
        y += q::intestazione(t, a, y, g, titolo);
        k = 0;
    };
    // le righe a 10 px, velate alterne; senza missioni un trattino spento al posto degli zeri
    const auto riga = [&](const std::string& e, const std::string& v, std::uint8_t col = 0) {
        if (y + 10 > a.y + a.h - 14) return;
        q::riga(t, a, y, g, taglia_q(g, e, 196), {}, {}, k++ % 2 == 1);
        if (vuote) scrivi_q(t, g, a.x + 206, y, "-", fosforo(3));
        else scrivi_q(t, g, a.x + 206, y, taglia_q(g, v, a.w - 210), col ? col : fosforo(6));
        y += 10;
    };
    sezione("TOTALI");
    riga("IN MARE", ore_minuti(d));
    riga("MIGLIA PERCORSE", numero(s_.distanza_nm, 1));
    riga("IN SUPERFICIE", ore_minuti(s_.superficie_s));
    riga("A QUOTA PERISCOPICA", ore_minuti(s_.periscopica_s));
    riga("IN CAVITAZIONE", ore_minuti(s_.cavitazione_s));
    riga("QUOTA MEDIA", numero(s_.quota_media_m()) + " M");
    riga("PERISCOPIO / ANTENNA", ore_minuti(s_.periscopio_s) + " / " + ore_minuti(s_.antenna_s));
    riga("TRASMISSIONI / PING", std::to_string(s_.trasmissioni) + " / " + std::to_string(s_.ping));
    riga("LANCI / A SEGNO", std::to_string(s_.lanci) + " / " + std::to_string(s_.colpi));
    riga("OSTILI AFFONDATI", std::to_string(s_.affondati_ostili), s_.affondati_ostili ? std::uint8_t{colori::VERDE_CHIARO} : std::uint8_t{0});
    y += 4;
    sezione("MEDIE PER MISSIONE");
    riga("DURATA", orologio(d / n));
    riga("MIGLIA", numero(s_.distanza_nm / n, 1));
    riga("LANCI", numero(static_cast<double>(s_.lanci) / n, 1));
    y += 4;
    sezione("RILEVAMENTI");
    riga("MISSIONI IN CUI TI HANNO TROVATO", std::to_string(s_.missioni_scoperti) + " SU " + std::to_string(s_.missioni),
         s_.missioni_scoperti ? std::uint8_t{colori::GIALLO} : std::uint8_t{colori::VERDE_CHIARO});
    for (const auto& [come, v] : s_.scoperte_per_modo)
        riga("  " + come_scoperti(come), std::to_string(v.first) + ", " + numero(v.second / std::max(1, v.first) / 1000.0, 1) + " KM");
    riga("CLASSIFICATI (GIUSTE / ERRATE)", std::to_string(s_.classificati) + " (" + std::to_string(s_.giuste) + " / " + std::to_string(s_.errate) + ")");
    if (s_.tempo_classificazione_s() > 0.0) riga("TEMPO MEDIO PER CLASSIFICARE", orologio(s_.tempo_classificazione_s()));
    y += 4;
    sezione("PRIMATI");
    riga("QUOTA MASSIMA", numero(s_.quota_max_m) + " M", colori::GIALLO);
    riga("VELOCITA' MASSIMA", numero(s_.velocita_max_kn, 1) + " KN", colori::GIALLO);
    riga("MISSIONE PIU' LUNGA", orologio(s_.missione_piu_lunga_s), colori::GIALLO);
    if (s_.scoperta_piu_lontana_m > 0.0)
        riga("SCOPERTI PIU' LONTANO / VICINO",
             numero(s_.scoperta_piu_lontana_m / 1000.0, 1) + " / " + numero(s_.scoperta_piu_vicina_m / 1000.0, 1) + " KM", colori::GIALLO);
    if (vuote) scrivi_q(t, g, a.x + 4, a.y + a.h - 12, "I NUMERI ARRIVANO DOPO IL PRIMO DEBRIEFING", fosforo(4));
    // a destra: la carta del teatro scelto, la scala, poi armi e affondati
    const std::string quanti = teatri_.empty() ? std::string("-") : "TEATRO " + std::to_string(scelto_ + 1) + "/" + std::to_string(teatri_.size());
    const Rettangolo b = q::schermo(t, {u.x + 304, u.y, u.w - 304, 424 - u.y}, g,
                                    teatri_.empty() ? std::string("DOVE SEI STATO") : "DOVE SEI STATO - " + maiu(teatro()), quanti);
    carta_ = {b.x + 4, b.y + 4, b.w - 8, 230};
    const auto it = s_.heatmap.find(teatro());
    if (vuote || teatri_.empty()) {   // la griglia spenta e lo stato vuoto
        for (int yy = carta_.y; yy < carta_.y + carta_.h; yy += 12)
            for (int xx = carta_.x; xx < carta_.x + carta_.w; xx += 2) t.pixel(xx, yy, fosforo(1));
        for (int xx = carta_.x; xx < carta_.x + carta_.w; xx += 12)
            for (int yy = carta_.y; yy < carta_.y + carta_.h; yy += 2) t.pixel(xx, yy, fosforo(1));
        q::stato_vuoto(t, carta_, g, "NESSUNA MISSIONE ANCORA",
                       {"LA CARTA DEL TEATRO SI COLORA DOVE PORTI", "IL BATTELLO, A CELLE DI 2 KM.", "TAB: GLI ALTRI TEATRI"});
    } else {   // la heatmap di oggi nella carta; la sua legenda la copre la scala del fosforo
        disegna_heatmap(t, f, {carta_.x, carta_.y, carta_.w, carta_.h + 24}, fondale_.get(), vga_, it == s_.heatmap.end() ? Heatmap{} : it->second,
                        CELLA_CUMULATA_M, {});
        t.rettangolo_pieno(b.x, carta_.y + carta_.h + 1, b.w, 24, fosforo(0));
    }
    int yd = carta_.y + carta_.h + 6;
    scrivi_q(t, g, b.x + 4, yd, "POCO", fosforo(4));
    for (int c = 0; c < 8; ++c)
        t.rettangolo_pieno(b.x + 34 + 12 * c, yd, 11, 7, vuote ? fosforo(1 + c % 2) : static_cast<std::uint8_t>(CALORE + c * 2 + 1));
    scrivi_q(t, g, b.x + 34 + 12 * 8 + 4, yd, "MOLTO", fosforo(4));
    if (!vuote) scrivi_q(t, g, b.x + b.w - 4 - largo_q(g, numero(s_.distanza_nm, 1) + " MIGLIA"), yd, numero(s_.distanza_nm, 1) + " MIGLIA", fosforo(5));
    yd += 16;
    yd += q::intestazione(t, b, yd, g, "ARMI (LANCI / A SEGNO)", {150}, {"AFFONDATI"});
    int ya = yd, yb = yd;
    if (s_.per_arma.empty()) scrivi_q(t, g, b.x + 4, ya, "NESSUN LANCIO", fosforo(3));
    for (const auto& [arma, c] : s_.per_arma) {
        if (ya + 10 > b.y + b.h) break;
        scrivi_q(t, g, b.x + 4, ya, taglia_q(g, arma.empty() ? std::string("SCONOSCIUTA") : maiu(arma), 90), fosforo(4));
        scrivi_q(t, g, b.x + 96, ya, std::to_string(c.lanci) + " / " + std::to_string(c.colpi), fosforo(6));
        ya += 10;
    }
    if (s_.affondati_per_classe.empty()) scrivi_q(t, g, b.x + 150, yb, "NESSUNO", fosforo(3));
    for (const auto& [classe, n_aff] : s_.affondati_per_classe) {
        if (yb + 10 > b.y + b.h) break;
        scrivi_q(t, g, b.x + 150, yb, taglia_q(g, maiu(classe), b.w - 180), fosforo(4));
        scrivi_q(t, g, b.x + b.w - 4 - largo_q(g, std::to_string(n_aff)), yb, std::to_string(n_aff), 12);
        yb += 10;
    }
    lamiera::piede(t, f, 428, "TAB ALTRO TEATRO   CLIC SULLA CARTA: IL TEATRO SEGUENTE   2 IMPRESE",
                   {{"TAB  ALTRO TEATRO", ColoreLampada::ambra}, {"ESC  INDIETRO", ColoreLampada::verde}}, 1);
}

}  // namespace m688

// --- le statistiche tecniche della sandbox ---

namespace m688 {

namespace {

const char* const LINGUETTE[] = {"1 RIEPILOGO", "2 SCOPERTE", "3 ARMI", "4 REAZIONI E TMA", "5 ESPOSIZIONE"};
const char* const LINGUETTE_BREVI[] = {"1 RIEPILOGO", "2 SCOPERTE", "3 ARMI", "4 REAZIONI", "5 ESPOSIZ."};   // nello schermo stretto
constexpr int PIEDE_Y = 428;                     // il piede comune: i due tasti a 450-476, larghi 308 da x 8 e 324
constexpr int RIGA_Q = 11;                       // le righe sul fosforo

// il colore di una riga sul fosforo: il bianco normale resta il fosforo; rosso, giallo e grigio restano leggibili
std::uint8_t colore_q(std::uint8_t c) {
    return c == colori::ROSSO ? std::uint8_t{12} : c == colori::GIALLO ? std::uint8_t{colori::GIALLO} : c == colori::GRIGIO ? fosforo(3) : fosforo(6);
}

std::string tempo_breve(double s) { return s < 0.0 ? std::string("-") : orologio(s).substr(2); }   // m:ss (h tolte)
std::string secondi(double s) { return s < 0.0 ? std::string("-") : numero(s) + " S"; }
std::string km(double m) { return m < 0.0 ? std::string("-") : numero(m / 1000.0, 1) + " KM"; }
std::string metri(std::optional<double> m) { return m ? numero(*m) + " M" : std::string("-"); }
std::string gradi(std::optional<double> g) { return g ? numero(*g) : std::string("-"); }
// 0x18: la parte del luogo (M7, unita' con schieramento = 'luogo'), una terza parte armata solo nell'Enhanced
std::string nome_parte(int p) { return p == 0x08 ? "NATO" : p == 0x10 ? "PATTO" : p == 0x18 ? "LUOGO" : "NEUTRALI"; }

}  // namespace

SchermataStatisticheSandbox::SchermataStatisticheSandbox(std::string dati, const Traccia& traccia)
    : dati_(std::move(dati)), s_(statistiche_sandbox(traccia)), missione_(traccia.missione) {
    try {
        targa_ = Font::carica(dati_ + "/font/incisione688_7.fnt");
        fosforo_ = Font::carica(dati_ + "/font/fosforo688_5x7.fnt");
    } catch (const std::exception&) {
        targa_.reset();
        fosforo_.reset();
    }
    for (const auto& [id, serie] : traccia.unita)
        if (!serie.empty()) nomi_[id] = maiu(serie.front().second.classe) + " " + std::to_string(id);
    prepara(traccia);
}

std::string SchermataStatisticheSandbox::nome(int id) const {
    const auto it = nomi_.find(id);
    return it != nomi_.end() ? it->second : id >= 0 ? std::to_string(id) : std::string("-");
}

void SchermataStatisticheSandbox::prepara(const Traccia&) {
    pagine_.assign(PAGINE, {});
    colonne_.assign(PAGINE, {});
    const auto titolo = [](std::vector<std::string> celle) {
        Riga r;
        r.celle = std::move(celle);
        r.colore = colori::CIANO_CHIARO;
        r.titolo = true;
        return r;
    };
    const auto riga = [](std::vector<std::string> celle, double t = -1.0, std::uint8_t col = colori::BIANCO) {
        Riga r;
        r.celle = std::move(celle);
        r.t = t;
        r.colore = col;
        return r;
    };
    // 1. riepilogo: una colonna per parte
    {
        auto& p = pagine_[0];
        colonne_[0] = {6, 230, 360, 490};
        std::vector<const StatParte*> parti;
        for (const auto& [k, v] : s_.parti) parti.push_back(&v);
        std::vector<std::string> testa{"PER PARTE"};
        for (const auto* x : parti) testa.push_back(nome_parte(x->parte));
        p.push_back(titolo(testa));
        const auto voce = [&](const char* etichetta, auto valore) {
            std::vector<std::string> c{etichetta};
            for (const auto* x : parti) c.push_back(valore(*x));
            p.push_back(riga(c));
        };
        voce("UNITA'", [](const StatParte& x) { return std::to_string(x.unita); });
        voce("PERSE / INFLITTE", [](const StatParte& x) { return std::to_string(x.perse) + " / " + std::to_string(x.inflitte); });
        voce("SCAMBIO", [](const StatParte& x) { return numero(x.scambio, 1); });
        voce("SCOPERTE FATTE / SUBITE", [](const StatParte& x) { return std::to_string(x.scoperte) + " / " + std::to_string(x.scoperte_subite); });
        voce("RITARDO MEDIO DI SCOPERTA", [](const StatParte& x) { return secondi(x.ritardo_medio_s); });
        voce("DISTANZA MEDIA DI SCOPERTA", [](const StatParte& x) { return km(x.distanza_media_m); });
        voce("LANCI / A SEGNO", [](const StatParte& x) { return std::to_string(x.lanci) + " / " + std::to_string(x.a_segno); });
        voce("ESCHE / EFFICACI", [](const StatParte& x) { return std::to_string(x.esche) + " / " + std::to_string(x.esche_efficaci); });
        voce("SCOPERTA - ALLERTA", [](const StatParte& x) { return secondi(x.reazione_media_s); });
        voce("SCOPERTA - INGAGGIO", [](const StatParte& x) { return secondi(x.ingaggio_medio_s); });
        voce("TEMPO ALLA SOLUZIONE", [](const StatParte& x) { return secondi(x.soluzione_media_s); });
        voce("ERRORE AL LANCIO", [](const StatParte& x) {
            return x.errore_lancio_medio_m < 0.0 ? std::string("-") : numero(x.errore_lancio_medio_m) + " M";
        });
        p.push_back(titolo({"PK PER ARMA (A SEGNO / LANCIATE)"}));
        std::map<std::string, std::vector<std::string>> pk;
        for (std::size_t k = 0; k < parti.size(); ++k)
            for (const auto& [arma, v] : parti[k]->pk) {
                auto& c = pk[arma];
                c.resize(parti.size(), "-");
                c[k] = std::to_string(v.first) + " / " + std::to_string(v.second);
            }
        for (auto& [arma, c] : pk) {
            std::vector<std::string> cel{maiu(arma)};
            cel.insert(cel.end(), c.begin(), c.end());
            p.push_back(riga(cel));
        }
        if (!s_.complete) p.push_back(riga({"TRACCIA SENZA I FATTI DELL'IA: STATISTICHE PARZIALI"}, -1.0, colori::GIALLO));
    }
    // 2. catene di scoperta, in ordine di contatto
    {
        auto& p = pagine_[1];
        colonne_[1] = {6, 120, 234, 312, 362, 412, 462, 530, 584};
        p.push_back(titolo({"OSSERVATORE", "BERSAGLIO", "SENSORE", "POSS.", "CONT.", "SOLIDO", "CLASS.", "DIST.", "RITARDO"}));
        auto v = s_.catene;
        std::sort(v.begin(), v.end(), [](const CatenaScoperta& a, const CatenaScoperta& b) { return a.t_contatto < b.t_contatto; });
        for (const auto& c : v)
            p.push_back(riga({nome(c.osservatore), nome(c.bersaglio), maiu(c.sensore), tempo_breve(c.t_possibile), tempo_breve(c.t_contatto),
                              tempo_breve(c.t_solido), c.classificato ? tempo_breve(c.t_classificato) + (c.classe_giusta ? "" : " X") : "-",
                              km(c.distanza_m), secondi(c.ritardo_s())},
                             c.t_contatto, c.classificato && !c.classe_giusta ? colori::GIALLO : colori::BIANCO));
    }
    // 3. armi
    {
        auto& p = pagine_[2];
        colonne_[2] = {6, 92, 206, 320, 372, 424, 476, 548};
        p.push_back(titolo({"ARMA", "LANCIATORE", "BERSAGLIO", "LANCIO", "DURATA", "FILO", "ESITO", "VITTIMA"}));
        for (const auto& a : s_.armi) {
            const bool colpo = a.esito == ArmaLanciata::Esito::a_segno || a.esito == ArmaLanciata::Esito::uccisione;
            p.push_back(riga({maiu(a.classe), nome(a.lanciatore), nome(a.bersaglio), tempo_breve(a.t_lancio), secondi(a.durata_s),
                              secondi(a.filo_s), maiu(nome_esito(a.esito)), a.vittima >= 0 ? nome(a.vittima) : "-"},
                             a.t_lancio, colpo ? colori::ROSSO : colori::BIANCO));
        }
        if (s_.armi.empty()) p.push_back(riga({"NESSUN LANCIO"}, -1.0, colori::GRIGIO));
    }
    // 4. reazioni e TMA
    {
        auto& p = pagine_[3];
        colonne_[3] = {6, 120, 234, 300, 366, 432, 498, 564};
        p.push_back(titolo({"UNITA'", "SU", "SCOP.", "ALLERTA", "INGAGGIO", "LANCIO", "MINACCIA", "EVASIONE"}));
        for (const auto& r : s_.reazioni)
            p.push_back(riga({nome(r.unita), nome(r.su), tempo_breve(r.t_scoperta), secondi(r.allerta_s), secondi(r.ingaggio_s), secondi(r.lancio_s),
                              tempo_breve(r.minaccia_t), secondi(r.evasione_s)},
                             r.t_scoperta));
        p.push_back(riga({""}));
        p.push_back(titolo({"UNITA'", "BERSAGLIO", "GAMBE", "SOLUZ.", "E. DIST.", "E. ROTTA", "E. VEL.", "RINUNCIA"}));
        for (const auto& m : s_.tma)
            p.push_back(riga({nome(m.unita), nome(m.bersaglio), std::to_string(m.gambe), secondi(m.soluzione_s), metri(m.errore_distanza_m),
                              gradi(m.errore_rotta_gradi), gradi(m.errore_velocita_kn), m.rinuncia ? "SI" : "NO"}));
    }
    // 5. esposizione e navigazione
    {
        auto& p = pagine_[4];
        colonne_[4] = {6, 140, 220, 300, 380, 460, 510, 560};
        p.push_back(titolo({"UNITA'", "PERISCOPIO", "ANTENNA", "SCHIERA", "RADAR", "PING", "RADIO", "CONTRO"}));
        for (const auto& e : s_.esposizione)
            p.push_back(riga({nome(e.unita), secondi(e.periscopio_s), secondi(e.antenna_s), secondi(e.schiera_s), secondi(e.radar_s), std::to_string(e.ping),
                              std::to_string(e.radio), std::to_string(e.contro_scoperte)}));
        p.push_back(riga({""}));
        p.push_back(titolo({"UNITA'", "ERRORE MEDIO", "ERRORE MASSIMO", "PUNTI NAVE", "MANCATI"}));
        for (const auto& n : s_.navigazione)
            p.push_back(riga({nome(n.unita), numero(n.errore_medio_m) + " M", numero(n.errore_max_m) + " M", std::to_string(n.punti_nave),
                              std::to_string(n.punti_mancati)}));
    }
}

std::size_t SchermataStatisticheSandbox::righe() const {
    return static_cast<std::size_t>(std::count_if(pagine_[pagina_].begin(), pagine_[pagina_].end(), [](const Riga& r) { return !r.titolo; }));
}

bool SchermataStatisticheSandbox::comandi(const Comandi& c, double) {
    if (c.esci || c.esc || c.invio) return true;
    if (c.tab != 0) pagina((pagina_ + PAGINE + (c.tab > 0 ? 1 : -1)) % PAGINE);
    const int massimo = std::max(0, static_cast<int>(pagine_[pagina_].size()) - visibili_);
    if (const int su = verso_freccia(c.su_giu, c.passi_su_giu)) scorri_ = std::clamp(scorri_ - su, 0, massimo);
    for (const auto& [x, y] : c.clic) {
        if (y >= PIEDE_Y + 22 && y < PIEDE_Y + 48) {   // i tasti del piede: TAB la pagina seguente, ESC indietro
            if (x >= 324) return true;
            pagina((pagina_ + 1) % PAGINE);
            continue;
        }
        bool linguetta = false;
        for (int k = 0; k < static_cast<int>(linguette_.size()); ++k) {
            const Rettangolo& r = linguette_[static_cast<std::size_t>(k)];
            if (x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h) {
                pagina(k);
                linguetta = true;
            }
        }
        if (linguetta) continue;
        for (const auto& [r, t] : zone_)
            if (x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h) {
                replay_da_ = t;
                return true;
            }
    }
    return false;
}

void SchermataStatisticheSandbox::palette(Palette& p) const {
    palette_statistiche(p);
    applica_tema(p, TemaStazione{});   // 16-47: la lamiera e il fosforo della variante C
}

void SchermataStatisticheSandbox::disegna(Tela& t, const Font& f) const {
    // variante C (roadmap 33, grafica/quadro; scelta dell'utente del 28/10/2026): la consolle della sala analisi, uno
    // schermo per la tabella (nel riepilogo stretto, coi numeri in figura nel secondo schermo) e il piede comune
    namespace q = quadro;
    const Font& g = fosforo_ ? *fosforo_ : f;
    const Rettangolo u = q::consolle(t, targa_ ? *targa_ : f, "SALA ANALISI - STATISTICHE DELLA SANDBOX");
    const bool riepilogo = pagina_ == 0;
    const int largo_schermo = riepilogo ? 384 : u.w;
    const std::string titolo = maiu(missione_.empty() ? std::string("SANDBOX") : missione_);
    const std::string durata = orologio(s_.durata_s);
    const Rettangolo a = q::schermo(t, {u.x, u.y, largo_schermo, PIEDE_Y - 4 - u.y}, g, taglia_q(g, titolo, largo_schermo - 30 - largo_q(g, durata)),
                                    durata);
    // le linguette (nello schermo stretto coi nomi brevi), e dove sono per i clic
    int y = a.y + 2;
    {
        std::vector<std::string> nomi;
        for (int k = 0; k < PAGINE; ++k) nomi.emplace_back(riepilogo ? LINGUETTE_BREVI[k] : LINGUETTE[k]);
        linguette_.clear();
        int x = a.x + 2;   // come quadro::linguette
        for (const auto& n : nomi) {
            const int w = largo_q(g, n) + 8;
            linguette_.push_back({x, y, w, g.righe() + 3});
            x += w + 4;
        }
        y += q::linguette(t, a, y, g, nomi, pagina_) + 4;
    }
    // le colonne della pagina, dallo schermo largo di 628 a quello vero (con 40 pixel in piu' per l'ultima colonna);
    // nel riepilogo le parti a destra, larghe 60
    const auto& righe_p = pagine_[pagina_];
    const auto& col_p = colonne_[pagina_];
    std::vector<int> col;
    if (riepilogo) {
        const int n = std::max<int>(1, static_cast<int>(col_p.size()) - 1);
        for (int k = 0; k < n; ++k) col.push_back(a.w - 8 - (n - k) * 60);
    } else {
        for (std::size_t k = 1; k < col_p.size(); ++k) col.push_back(4 + (col_p[k] - 6) * (a.w - 48) / 622);
    }
    const int prima = col.empty() ? a.w : col.front();
    zone_.clear();
    const int fine = a.y + a.h - 2;
    visibili_ = std::max(1, (fine - y) / RIGA_Q);
    int k_dati = 0;
    for (std::size_t k = static_cast<std::size_t>(scorri_); k < righe_p.size() && y + RIGA_Q <= fine; ++k) {
        const Riga& r = righe_p[k];
        const auto cella = [&](std::size_t c) {
            const int w = (c + 1 < r.celle.size() && c < col.size() ? col[c] : a.w - 4) - (c == 0 ? 4 : col[c - 1]) - 4;
            return taglia_q(g, r.celle[c], r.celle.size() == 1 ? a.w - 8 : c == 0 ? prima - 8 : w);
        };
        if (r.titolo) {
            if (k != static_cast<std::size_t>(scorri_)) y += 4;
            std::vector<std::string> nomi;
            for (std::size_t c = 1; c < r.celle.size(); ++c) nomi.push_back(cella(c));
            y += q::intestazione(t, a, y, g, r.celle.empty() ? std::string() : cella(0), col, nomi);
            k_dati = 0;
            continue;
        }
        std::vector<std::string> valori;
        for (std::size_t c = 1; c < r.celle.size(); ++c) valori.push_back(cella(c));
        const std::string etichetta = r.celle.empty() ? std::string() : cella(0);
        if (r.colore == colori::BIANCO) {
            q::riga(t, a, y, g, etichetta, col, valori, k_dati % 2 == 1);   // righe alterne velate: l'occhio segue la riga
        } else {
            if (k_dati % 2 == 1) t.rettangolo_pieno(a.x, y - 1, a.w, g.righe() + 2, fosforo(1));
            const std::uint8_t c = colore_q(r.colore);
            scrivi_q(t, g, a.x + 4, y, etichetta, c);
            for (std::size_t i = 0; i < valori.size() && i < col.size(); ++i) scrivi_q(t, g, a.x + col[i], y, valori[i], c);
        }
        if (r.t >= 0.0) zone_.push_back({{a.x, y - 1, a.w, RIGA_Q}, r.t});
        y += RIGA_Q;
        ++k_dati;
    }
    if (scorri_ > 0) scrivi_q(t, g, a.x + a.w - 10, a.y + 2, "^", fosforo(7));
    if (scorri_ + visibili_ < static_cast<int>(righe_p.size())) scrivi_q(t, g, a.x + a.w - 10, fine - g.righe(), "V", fosforo(7));
    if (riepilogo) disegna_figura(t, g, q::schermo(t, {u.x + 388, u.y, u.w - 388, PIEDE_Y - 4 - u.y}, g, "IN FIGURA"));
    lamiera::piede(t, f, PIEDE_Y, "TAB PAGINA   FRECCE SCORRONO   CLIC SU UNA RIGA: IL REPLAY DA LI'",
                   {{"TAB  PAGINA SEGUENTE", ColoreLampada::ambra}, {"ESC  INDIETRO", ColoreLampada::verde}}, 1);
}

void SchermataStatisticheSandbox::disegna_figura(Tela& t, const Font& g, Rettangolo b) const {
    namespace q = quadro;
    std::vector<const StatParte*> parti;
    for (const auto& [k, v] : s_.parti) parti.push_back(&v);
    int y = b.y + 4;
    const int x0 = b.x + 52, w0 = b.w - 52 - 28;   // le barre: dopo il nome della parte, col numero in fondo
    // le scoperte fatte (piene) e subite (spente), in scala comune
    y += q::intestazione(t, b, y, g, "SCOPERTE FATTE E SUBITE");
    int massimo = 1;
    for (const auto* p : parti) massimo = std::max({massimo, p->scoperte, p->scoperte_subite});
    const auto barra = [&](int yy, int n, int scala, std::uint8_t c) {
        const int w = n > 0 ? std::max(2, w0 * n / scala) : 0;
        t.rettangolo_pieno(x0, yy, w, 7, c);
        scrivi_q(t, g, x0 + w + 4, yy, std::to_string(n), fosforo(6));
    };
    for (const auto* p : parti) {
        scrivi_q(t, g, b.x + 4, y + 4, nome_parte(p->parte), fosforo(5));
        barra(y, p->scoperte, massimo, fosforo(6));
        barra(y + 10, p->scoperte_subite, massimo, fosforo(3));
        y += 26;
    }
    t.rettangolo_pieno(b.x + 4, y, 10, 6, fosforo(6));
    scrivi_q(t, g, b.x + 18, y, "FATTE", fosforo(4));
    t.rettangolo_pieno(b.x + 74, y, 10, 6, fosforo(3));
    scrivi_q(t, g, b.x + 88, y, "SUBITE", fosforo(4));
    y += 20;
    // i lanci e le esche, a parole
    y += q::intestazione(t, b, y, g, "LANCI E ESCHE");
    for (const auto* p : parti) {
        if (y + RIGA_Q > b.y + b.h) break;
        std::string s = nome_parte(p->parte) + ": ";
        s += p->lanci ? std::to_string(p->lanci) + (p->lanci == 1 ? " LANCIO, " : " LANCI, ") + std::to_string(p->a_segno) + " A SEGNO"
                      : std::string("NESSUN LANCIO");
        if (p->esche) s += ", " + std::to_string(p->esche) + (p->esche == 1 ? " ESCA" : " ESCHE");
        scrivi_q(t, g, b.x + 4, y, taglia_q(g, s, b.w - 8), fosforo(6));
        y += RIGA_Q;
    }
    y += 10;
    // i tempi medi di reazione per parte, in scala comune (almeno 60 s)
    y += q::intestazione(t, b, y, g, "TEMPI MEDI DALLA SCOPERTA (S)");
    double scala = 60.0;
    for (const auto* p : parti) scala = std::max({scala, p->reazione_media_s, p->soluzione_media_s, p->ingaggio_medio_s});
    for (const auto* p : parti) {
        if (y + 4 * 12 > b.y + b.h) break;
        scrivi_q(t, g, b.x + 4, y, nome_parte(p->parte), fosforo(5));
        y += 11;
        const struct { const char* n; double s; } tempi[] = {{"ALLERTA", p->reazione_media_s}, {"SOLUZIONE", p->soluzione_media_s},
                                                             {"INGAGGIO", p->ingaggio_medio_s}};
        for (const auto& x : tempi) {
            scrivi_q(t, g, b.x + 10, y, x.n, fosforo(4));
            if (x.s < 0.0) {
                scrivi_q(t, g, b.x + 70, y, "-", fosforo(3));
            } else {
                const int w = std::max(1, static_cast<int>((b.w - 104) * x.s / scala));
                t.rettangolo_pieno(b.x + 70, y, w, 7, fosforo(5));
                scrivi_q(t, g, b.x + 74 + w, y, numero(x.s), fosforo(6));
            }
            y += 12;
        }
        y += 4;
    }
    if (parti.empty()) scrivi_q(t, g, b.x + 4, y, "NESSUNA PARTE IN GIOCO", fosforo(3));
}

}  // namespace m688
