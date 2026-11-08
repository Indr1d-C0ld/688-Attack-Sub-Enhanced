#include "regolazioni_sandbox.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <stdexcept>
#include <vector>

#include <toml.hpp>

#include "carta.hpp"
#include "codice_partita.hpp"
#include "grafica/consolle.hpp"
#include "grafica/lamiera.hpp"
#include "grafica/tema_stazioni.hpp"
#include "nucleo/traccia_replay.hpp"

namespace m688 {

namespace {

// La variante A «lamiera di bordo» (roadmap 33, docs/ricerca/25_bozze, scelta dell'utente del 28/10/2026): le tre
// parti in colonna, la partita e le parti dell'IA come un quadro di levette, il piede comune
constexpr int PIEDE_Y = 392, PIEDE_TASTI_Y = PIEDE_Y + 22, PIEDE_TASTI_H = 26;
constexpr int PARTE_X0 = 8, PARTE_PASSO = 210, PARTE_W = 204, PARTE_Y = 46, PARTE_H = 160, PARTE_RIGA = 27;
constexpr int PARTITA_X = 8, PARTITA_Y = 212, PARTITA_W = 440, PANNELLO_BASSO_H = 176, PARTITA_RIGA = 19;
constexpr int IA_X = 454, IA_W = 178, IA_RIGA = 24;
constexpr std::uint8_t CIANO_CAMBIATO = 11;   // il valore cambiato rispetto alle regolazioni di base

// le righe, nell'ordine: le tre parti (situazione e ROE), poi le globali e gli interruttori
// per parte cinque righe: situazione, ROE, livello, allerta, carico
enum Riga {
    nato_sit, nato_roe, nato_liv, nato_all, nato_car, patto_sit, patto_roe, patto_liv, patto_all, patto_car, neutr_sit,
    neutr_roe, neutr_liv, neutr_all, neutr_car, meteo, navigazione, sensori, seme, durata, esito, eventi, traffico, difesa, armi,
    reazioni, forze, esm_ia, sonaristi_ia, RIGHE
};
constexpr int RIGHE_PARTE = 5;

std::string maiuscolo(std::string s) {
    for (auto& c : s) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return s;
}

// Il rettangolo di una riga sullo schermo: per il disegno e per il clic
Rettangolo area_riga(int r) {
    if (r < 3 * RIGHE_PARTE) {
        const int p = r / RIGHE_PARTE, i = r % RIGHE_PARTE;
        return {PARTE_X0 + p * PARTE_PASSO + 8, PARTE_Y + 22 + i * PARTE_RIGA, PARTE_W - 16, PARTE_RIGA - 1};
    }
    if (r < difesa) return {PARTITA_X + 8, PARTITA_Y + 24 + (r - meteo) * PARTITA_RIGA, PARTITA_W - 16, PARTITA_RIGA - 2};
    return {IA_X + 8, PARTITA_Y + 26 + (r - difesa) * IA_RIGA, IA_W - 16, IA_RIGA - 2};
}

bool dentro(const Rettangolo& a, int x, int y) { return x >= a.x && x < a.x + a.w && y >= a.y && y < a.y + a.h; }

// --- la seconda pagina, gli interventi programmati (docs/28 §5-6) ---
// in alto a destra, su entrambe le pagine: il tasto TAB e il nome dell'altra pagina (anche col clic)
constexpr int SCHEDA_X = 520, SCHEDA_H = 30;
// il pannello dell'esperimento (quattro campi) e quello degli interventi, fino al piede
constexpr int ESP_X = 8, ESP_Y = 46, ESP_W = 624, ESP_H = 106, ESP_RIGA = 19, CAMPI = 4;
constexpr int INT_X = 8, INT_Y = 156, INT_W = 624, INT_H = 230, LISTA_Y = INT_Y + 24, LISTA_RIGA = 15, LISTA_RIGHE = 13;
// col pannello degli interventi aperto: la cartina a sinistra, il pannello a destra
constexpr Rettangolo CARTINA{16, LISTA_Y, 352, 200}, PANNELLO{376, INT_Y + 6, 248, INT_H - 12};
constexpr double ORA_MAX_S = 24 * 3600.0;
constexpr int SEMI_MAX = 500;
constexpr std::uint8_t MAGENTA = 13;

Rettangolo area_campo(int r) { return {ESP_X + 8, ESP_Y + 24 + r * ESP_RIGA, ESP_W - 16, ESP_RIGA - 2}; }
Rettangolo area_lista(int i) { return {INT_X + 8, LISTA_Y + i * LISTA_RIGA, INT_W - 16, LISTA_RIGA - 1}; }
// la prima riga visibile della lista: la pagina che contiene la scelta
int inizio_lista(int scelto) { return scelto < 0 ? 0 : scelto / LISTA_RIGHE * LISTA_RIGHE; }

std::string orologio(double s) {
    const int t = static_cast<int>(std::lround(std::max(0.0, s)));
    char b[24];
    std::snprintf(b, sizeof b, "%02d:%02d", t / 60, t % 60);
    return b;
}

// il testo tagliato alla larghezza w, coi puntini
std::string taglia(const Font& f, std::string s, int w) {
    if (f.larghezza(s, 1) <= w) return s;
    while (!s.empty() && f.larghezza(s + "..", 1) > w) s.pop_back();
    return s + "..";
}

// Il nome di un file dal testo dell'utente: lettere, cifre, - e _ (gli spazi diventano _), con .m688v
std::string nome_file(std::string s) {
    if (s.size() > 6 && s.substr(s.size() - 6) == ".m688v") s.resize(s.size() - 6);
    std::string r;
    for (const char ch : s) {
        const auto u = static_cast<unsigned char>(ch);
        if (std::isalnum(u) || ch == '-' || ch == '_') r += static_cast<char>(std::tolower(u));
        else if (ch == ' ' && !r.empty() && r.back() != '_') r += '_';
    }
    while (!r.empty() && r.back() == '_') r.pop_back();
    return (r.empty() ? std::string("esperimento") : r) + ".m688v";
}

// La cartina su cui si sceglie il punto: tutte le unita' della missione con un margine, almeno 40 km di lato, il nord in alto
struct Cartina {
    Vec2 centro;
    double metri_per_pixel = 1.0;
    Vec2 a_tela(Vec2 p) const {
        return {CARTINA.x + CARTINA.w / 2.0 + (p.x - centro.x) / metri_per_pixel, CARTINA.y + CARTINA.h / 2.0 - (p.y - centro.y) / metri_per_pixel};
    }
    Vec2 a_mondo(int x, int y) const {
        return {centro.x + (x - CARTINA.x - CARTINA.w / 2.0) * metri_per_pixel, centro.y - (y - CARTINA.y - CARTINA.h / 2.0) * metri_per_pixel};
    }
};

Cartina cartina(const std::vector<UnitaEsperimento>& u) {
    Cartina c;
    double x0 = 0, x1 = 0, y0 = 0, y1 = 0;
    for (std::size_t i = 0; i < u.size(); ++i) {
        const Vec2 p = u[i].posizione;
        if (i == 0) x0 = x1 = p.x, y0 = y1 = p.y;
        x0 = std::min(x0, p.x), x1 = std::max(x1, p.x), y0 = std::min(y0, p.y), y1 = std::max(y1, p.y);
    }
    c.centro = {(x0 + x1) / 2.0, (y0 + y1) / 2.0};
    const double lato_x = std::max((x1 - x0) * 1.3, 40000.0), lato_y = std::max((y1 - y0) * 1.3, 40000.0);
    c.metri_per_pixel = std::max(lato_x / CARTINA.w, lato_y / CARTINA.h);
    return c;
}

// la scheda in alto a destra: il tasto TAB e il nome dell'altra pagina
void scheda(Tela& t, const Font& f, std::string_view altra) {
    const int x = LARGHEZZA_TELA - 10 - f.larghezza(altra, 1);
    lamiera::tasto_tastiera(t, {x - 34, 8, 28, 14}, "TAB", f);
    lamiera::incisa(t, f, x, 11, altra, colori::BIANCO);
}

std::uint8_t colore_parte(const std::string& parte) {
    return parte == "NATO" ? colori::CIANO_CHIARO : parte == "PATTO" ? colori::ROSSO : colori::VERDE_CHIARO;
}

}  // namespace

std::filesystem::path cartella_esperimenti() { return cartella_replay(std::string()).parent_path().parent_path() / "esperimenti"; }

std::vector<std::filesystem::path> elenco_esperimenti(const std::filesystem::path& cartella) {
    std::vector<std::filesystem::path> v;
    std::error_code ec;
    for (const auto& e : std::filesystem::directory_iterator(cartella, ec))
        if (e.path().extension() == ".m688v" && e.is_regular_file(ec)) v.push_back(e.path());
    std::sort(v.begin(), v.end());
    return v;
}

SchermataRegolazioni::SchermataRegolazioni(std::string titolo_missione, RegolazioniSandbox r)
    : titolo_(std::move(titolo_missione)), r_(r), pannello_(catalogo_pannello(catalogo_interventi(), {})) {}

const char* nome_situazione_scelta(SituazioneScelta s) {
    switch (s) {
        case SituazioneScelta::missione: return "DELLA MISSIONE";
        case SituazioneScelta::pace: return "PACE";
        case SituazioneScelta::crisi: return "CRISI";
        case SituazioneScelta::guerra: return "GUERRA";
    }
    return "DELLA MISSIONE";
}

const char* nome_livello_scelta(LivelloScelta l) {
    switch (l) {
        case LivelloScelta::missione: return "DELLA MISSIONE";
        case LivelloScelta::recluta: return "RECLUTA";
        case LivelloScelta::regolare: return "REGOLARE";
        case LivelloScelta::esperto: return "ESPERTO";
    }
    return "DELLA MISSIONE";
}

const char* nome_roe_scelta(RoeScelta r) {
    switch (r) {
        case RoeScelta::missione: return "DELLA MISSIONE";
        case RoeScelta::trattenute: return "TUTTE TRATTENUTE";
        case RoeScelta::libere: return "TUTTE LIBERE";
    }
    return "DELLA MISSIONE";
}

std::vector<VetrinoPronto> leggi_vetrini(const std::filesystem::path& cartella_prove, bool inglese) {
    std::vector<VetrinoPronto> r;
    toml::table t;
    try {
        t = toml::parse_file((cartella_prove / "vetrini.toml").string());
    } catch (const std::exception&) {
        return r;
    }
    const toml::array* voci = t["vetrino"].as_array();
    if (!voci) return r;
    const char* lingua = inglese ? "en" : "it";
    for (const auto& n : *voci) {
        const toml::table* v = n.as_table();
        if (!v || !(*v)["pronto"].value_or(false)) continue;
        VetrinoPronto p;
        p.missione = (*v)["missione"].value_or(std::string{});
        p.file = cartella_prove / (p.missione + ".lua");
        if (p.missione.empty() || !std::filesystem::exists(p.file)) continue;
        p.titolo = (*v)["titolo"][lingua].value_or(p.missione);
        p.mostra = (*v)["mostra"][lingua].value_or(std::string{});
        RegolazioniSandbox& g = p.regolazioni;
        g.durata_min = std::clamp((*v)["durata_min"].value_or(g.durata_min), 15, 24 * 60);
        if (const toml::table* rg = (*v)["regolazioni"].as_table()) {
            g.meteo_realistico = (*rg)["meteo_realistico"].value_or(g.meteo_realistico);
            g.navigazione_realistica = (*rg)["navigazione_realistica"].value_or(g.navigazione_realistica);
            g.anni_89 = (*rg)["anni_89"].value_or(g.anni_89);
            g.ferma_all_esito = (*rg)["ferma_all_esito"].value_or(g.ferma_all_esito);
            g.ferma_eventi = (*rg)["ferma_eventi"].value_or(g.ferma_eventi);
        }
        r.push_back(std::move(p));
    }
    return r;
}

void SchermataRegolazioni::cambia(int riga, int verso) {
    regolazioni_cambiate_ = true;   // il codice del file caricato non vale piu'
    const auto gira = [verso](auto& e, int n) {
        e = static_cast<std::remove_reference_t<decltype(e)>>(((static_cast<int>(e) + verso) % n + n) % n);
    };
    if (riga < 3 * RIGHE_PARTE) {   // le righe delle tre parti
        SceltaParteSandbox& p = riga < RIGHE_PARTE ? r_.nato : riga < 2 * RIGHE_PARTE ? r_.patto : r_.neutrali;
        switch (riga % RIGHE_PARTE) {
            case 0: gira(p.situazione, 4); break;
            case 1: gira(p.roe, 3); break;
            case 2: gira(p.livello, 4); break;
            case 3: p.allerta = ((p.allerta + 1 + verso) % 5 + 5) % 5 - 1; break;   // missione, A0..A3
            case 4: p.carico_per_classe = !p.carico_per_classe; break;
        }
        return;
    }
    switch (riga) {
        case meteo: r_.meteo_realistico = !r_.meteo_realistico; break;
        case navigazione: r_.navigazione_realistica = !r_.navigazione_realistica; break;
        case sensori: r_.anni_89 = !r_.anni_89; break;
        case seme:   // nuovo, poi semi fissi 1, 2, 3... per i confronti
            if (!r_.seme) { if (verso > 0) r_.seme = 1u; }
            else if (*r_.seme + verso == 0) r_.seme.reset();
            else r_.seme = *r_.seme + static_cast<std::uint32_t>(verso);
            break;
        case durata: r_.durata_min = std::clamp(r_.durata_min + 15 * verso, 15, 24 * 60); break;
        case esito: r_.ferma_all_esito = !r_.ferma_all_esito; break;
        case eventi: r_.ferma_eventi = !r_.ferma_eventi; break;
        case traffico: r_.traffico = !r_.traffico; break;
        case difesa: r_.difesa = !r_.difesa; break;
        case armi: r_.armi = !r_.armi; break;
        case reazioni: r_.reazioni = !r_.reazioni; break;
        case forze: r_.forze = !r_.forze; break;
        case esm_ia: r_.esm_ia = !r_.esm_ia; break;
        case sonaristi_ia: r_.sonaristi_ia = !r_.sonaristi_ia; break;
        default: break;
    }
}

SchermataRegolazioni::Uscita SchermataRegolazioni::comandi(const Comandi& c, double dt) {
    if (pagina_ == Pagina::interventi) return comandi_interventi(c, dt);
    if (c.esci || c.esc) return Uscita::indietro;
    if (c.invio) return Uscita::avvia;
    if (std::find(c.lettere.begin(), c.lettere.end(), 'S') != c.lettere.end()) return Uscita::serie;
    if (c.tab != 0) {   // la seconda pagina
        pagina_ = Pagina::interventi;
        prec_su_giu_ = c.su_giu;
        prec_ruota_ = c.ruota;
        return Uscita::nessuna;
    }
    if (const int su = passi_freccia(c.su_giu, c.passi_su_giu, prec_su_giu_)) riga_ = std::clamp(riga_ - su, 0, RIGHE - 1);
    if (const int lato = passi_freccia(c.ruota, c.passi_ruota, prec_ruota_)) cambia(riga_, lato);
    for (const auto& [x, y] : c.clic) {
        if (x >= SCHEDA_X && y < SCHEDA_H) {   // la scheda in alto a destra: la seconda pagina
            pagina_ = Pagina::interventi;
            return Uscita::nessuna;
        }
        if (y >= PIEDE_TASTI_Y && y < PIEDE_TASTI_Y + PIEDE_TASTI_H) {   // i tre tasti del piede, larghi 202
            if (x >= 428) return Uscita::avvia;
            if (x < 210) return Uscita::indietro;
            if (x >= 218 && x < 420) return Uscita::serie;
        }
        for (int r = 0; r < RIGHE; ++r)
            if (dentro(area_riga(r), x, y)) {
                if (r == riga_) cambia(r, +1);   // un clic sulla riga scelta cambia il valore
                riga_ = r;
            }
    }
    return Uscita::nessuna;
}

void SchermataRegolazioni::palette(Palette& p) const {
    applica_palette_carta(p, StileCarta::vga);
    applica_tema(p, TemaStazione{});   // per ultimo: la lamiera, i tasti e le lampade (16-47)
}

void SchermataRegolazioni::disegna(Tela& t, const Font& f) const {
    if (pagina_ == Pagina::interventi) {
        disegna_interventi(t, f);
        return;
    }
    namespace la = lamiera;
    const RegolazioniSandbox base{};
    la::lamiera(t, {0, 0, LARGHEZZA_TELA, ALTEZZA_TELA}, 7);
    la::targa(t, f, LARGHEZZA_TELA / 2, 6, "SANDBOX: IL VETRINO DELL'IA");
    la::incisa(t, f, 10, 10, maiuscolo(titolo_), CIANO_CAMBIATO);
    scheda(t, f, "INTERVENTI");
    la::incisa(t, f, 16, 32, "NESSUN GIOCATORE: ANCHE IL BATTELLO E' COMANDATO DALL'IA, CON LE STESSE REGOLE DI TUTTI.", colori::BIANCO);
    // una riga: l'etichetta incisa e il valore nel vetro (ciano se cambiato), la riga scelta col filo giallo
    const auto riga = [&](int r, const std::string& etichetta, const std::string& valore, bool cambiato, bool in_colonna) {
        const Rettangolo a = area_riga(r);
        const bool scelta = r == riga_;
        const Rettangolo w = in_colonna ? Rettangolo{a.x + 4, a.y + 11, a.w - 20, 14} : Rettangolo{a.x + 204, a.y + 1, a.w - 206, 14};
        la::incisa(t, f, a.x + 4, in_colonna ? a.y : a.y + 4, etichetta, scelta ? colori::GIALLO : colori::BIANCO);
        la::vetro(t, w);
        f.scrivi(t, w.x + 4, w.y + 3, valore, scelta ? colori::GIALLO : cambiato ? CIANO_CAMBIATO : colori::GRIGIO, 1);
        if (in_colonna) lampada_rettangolare(t, {a.x + a.w - 12, a.y + 13, 8, 10}, ColoreLampada::ambra, scelta);
        if (scelta) la::smusso(t, {w.x - 1, w.y - 1, w.w + 2, w.h + 2}, colori::GIALLO, colori::GIALLO);
    };
    // le tre parti in colonna
    static constexpr const char* PARTI[] = {"NATO", "PATTO DI VARSAVIA", "NEUTRALI"};
    const SceltaParteSandbox* parti[] = {&r_.nato, &r_.patto, &r_.neutrali};
    const SceltaParteSandbox& pb = base.nato;
    for (int p = 0; p < 3; ++p) {
        la::pannello(t, {PARTE_X0 + p * PARTE_PASSO, PARTE_Y, PARTE_W, PARTE_H}, 8);
        la::secondaria(t, f, PARTE_X0 + p * PARTE_PASSO + 12, PARTE_Y + 8, PARTI[p]);
        const SceltaParteSandbox& s = *parti[p];
        const int r0 = p * RIGHE_PARTE;
        riga(r0, "SITUAZIONE", nome_situazione_scelta(s.situazione), s.situazione != pb.situazione, true);
        riga(r0 + 1, "ROE", nome_roe_scelta(s.roe), s.roe != pb.roe, true);
        riga(r0 + 2, "LIVELLO DELL'IA", nome_livello_scelta(s.livello), s.livello != pb.livello, true);
        riga(r0 + 3, "ALLERTA DI PARTENZA", s.allerta < 0 ? "DELLA MISSIONE" : "A" + std::to_string(s.allerta), s.allerta != pb.allerta, true);
        riga(r0 + 4, "CARICO", s.carico_per_classe ? "PER CLASSE E ANNO" : "DELLA MISSIONE", s.carico_per_classe != pb.carico_per_classe, true);
    }
    // la partita
    la::pannello(t, {PARTITA_X, PARTITA_Y, PARTITA_W, PANNELLO_BASSO_H}, 8);
    la::secondaria(t, f, PARTITA_X + 12, PARTITA_Y + 8, "LA PARTITA");
    char seme_txt[32];
    if (r_.seme) std::snprintf(seme_txt, sizeof seme_txt, "%u", static_cast<unsigned>(*r_.seme));
    char durata_txt[32];
    std::snprintf(durata_txt, sizeof durata_txt, "%d MIN", r_.durata_min);
    riga(meteo, "METEO", r_.meteo_realistico ? "DEL TEATRO E DELLA STAGIONE" : "MARE 3, SERENO", r_.meteo_realistico != base.meteo_realistico, false);
    riga(navigazione, "NAVIGAZIONE", r_.navigazione_realistica ? "REALISTICA" : "PERFETTA", r_.navigazione_realistica != base.navigazione_realistica, false);
    riga(sensori, "SENSORI E STAZIONI", r_.anni_89 ? "1989" : "MODERNI", r_.anni_89 != base.anni_89, false);
    riga(seme, "SEME", r_.seme ? seme_txt : "NUOVO A OGNI PARTITA", r_.seme.has_value(), false);
    riga(durata, "DURATA MASSIMA", durata_txt, r_.durata_min != base.durata_min, false);
    riga(esito, "ALL'ESITO DELLA MISSIONE", r_.ferma_all_esito ? "SI FERMA" : "CONTINUA FINO ALLA DURATA", r_.ferma_all_esito != base.ferma_all_esito, false);
    riga(eventi, "TEMPO COMPRESSO AGLI EVENTI", r_.ferma_eventi ? "TORNA A 1X" : "NON SI FERMA", r_.ferma_eventi != base.ferma_eventi, false);
    riga(traffico, "TRAFFICO CIVILE", r_.traffico ? "NAVI CIVILI NEI TEATRI" : "NESSUNO", r_.traffico != base.traffico, false);
    // le parti dell'IA: un quadro di levette con la spia verde
    la::pannello(t, {IA_X, PARTITA_Y, IA_W, PANNELLO_BASSO_H}, 8);
    la::secondaria(t, f, IA_X + 12, PARTITA_Y + 8, "PARTI DELL'IA");
    static constexpr const char* IA[] = {"DIFESA", "ARMI", "REAZIONI", "FORZE", "ESM", "SONARISTI"};
    static constexpr const char* IA_NOTA[] = {"ESCHE, EVASIONE, RITIRATA", "CARICO, TMA, FILO", "AGLI ALLEATI", "ALLERTA, MESSAGGI, ZONE",
                                              "DELLE UNITA'", "DELL'IA"};
    const bool accese[] = {r_.difesa, r_.armi, r_.reazioni, r_.forze, r_.esm_ia, r_.sonaristi_ia};
    for (int i = 0; i < 6; ++i) {
        const Rettangolo a = area_riga(difesa + i);
        const bool scelta = difesa + i == riga_;
        if (scelta) la::vetro(t, a);
        levetta(t, a.x + 10, a.y + 10, accese[i]);
        lampada_rettangolare(t, {a.x + 22, a.y + 5, 10, 10}, ColoreLampada::verde, accese[i]);
        la::incisa(t, f, a.x + 40, a.y + 2, IA[i], scelta ? colori::GIALLO : accese[i] ? colori::BIANCO : colori::GRIGIO);
        la::secondaria(t, f, a.x + 40, a.y + 12, IA_NOTA[i], true);
    }
    la::piede(t, f, PIEDE_Y, "SU E GIU' SCELGONO LA RIGA, SINISTRA E DESTRA CAMBIANO IL VALORE. CIANO: CAMBIATO.",
              {{"ESC  INDIETRO", ColoreLampada::rossa}, {"S  SERIE DI 10 SEMI", ColoreLampada::ambra},
               {"INVIO  AVVIA IL VETRINO", ColoreLampada::verde}}, 2);
}


// --- la seconda pagina: gli interventi programmati del vetrino di Dio (docs/28 §5-6) ---

void SchermataRegolazioni::imposta_missione(std::vector<UnitaEsperimento> unita,
                                            std::vector<std::pair<std::string, std::string>> classi) {
    unita_ = std::move(unita);
    classi_ = std::move(classi);
    pannello_ = PannelloInterventi(catalogo_pannello(catalogo_interventi(), classi_));
    pannello_.istruzione_punto(false);   // l'istruzione la scrive la cartina (disegna_interventi)
    apri_pannello();   // il pannello conosce le unita' anche da chiuso: le righe ne dicono il nome
    pannello_.chiudi();
}

void SchermataRegolazioni::imposta_cartelle(std::filesystem::path utente, std::filesystem::path pronti) {
    cartella_utente_ = std::move(utente);
    cartella_pronti_ = std::move(pronti);
}

std::filesystem::path SchermataRegolazioni::cartella_utente() const {
    return cartella_utente_.empty() ? cartella_esperimenti() : cartella_utente_;
}

Esperimento SchermataRegolazioni::esperimento() const {
    Esperimento e = esperimento_;
    if (!codice_caricato_.empty() && !regolazioni_cambiate_) e.codice = codice_caricato_;
    else if (codice_) e.codice = codice_(r_);
    else e.codice = codice_caricato_;
    for (Intervento& x : e.interventi) {
        x.id = -1;
        x.tempo = std::max(0.0, x.tempo);
    }
    return e;
}

std::optional<std::uint32_t> SchermataRegolazioni::impronta_corrente() const {
    if (!codice_) return std::nullopt;
    const auto k = leggi_codice_sandbox(codice_(r_));
    if (!k) return std::nullopt;
    return k->partita.impronta;
}

std::string SchermataRegolazioni::riga_intervento(const Intervento& x) const {
    RichiestaIntervento q;
    q.tipo = x.tipo;
    q.unita = x.unita;
    q.punto = x.punto;
    q.raggio_m = x.raggio_m;
    q.parametri = x.parametri;
    q.testi = x.testi;
    q.durata_s = x.durata_s;
    q.tempo = std::max(0.0, x.tempo);
    q.subito = false;
    return pannello_.riga(q);
}

void SchermataRegolazioni::apri_pannello() {
    std::vector<UnitaPannello> v;
    v.reserve(unita_.size());
    for (const auto& u : unita_) v.push_back(u.unita);
    pannello_.apri(0.0, std::move(v));   // dall'inizio: «quando: fra» e' gia' l'ora, che si rifinisce dopo
}

void SchermataRegolazioni::ordina(int scelto) {
    auto& v = esperimento_.interventi;
    std::vector<std::pair<Intervento, bool>> w;
    w.reserve(v.size());
    for (int i = 0; i < static_cast<int>(v.size()); ++i) w.emplace_back(std::move(v[static_cast<std::size_t>(i)]), i == scelto);
    std::stable_sort(w.begin(), w.end(), [](const auto& a, const auto& b) { return a.first.tempo < b.first.tempo; });
    v.clear();
    for (int i = 0; i < static_cast<int>(w.size()); ++i) {
        if (w[static_cast<std::size_t>(i)].second) riga2_ = CAMPI + i;
        v.push_back(std::move(w[static_cast<std::size_t>(i)].first));
    }
}

void SchermataRegolazioni::conferma_ora() {
    auto& v = esperimento_.interventi;
    if (nuovo_) {
        Intervento x = *nuovo_;
        x.id = -1;
        x.tempo = *ora_;
        v.push_back(std::move(x));
        ordina(static_cast<int>(v.size()) - 1);
        avviso_ = "AGGIUNTO: " + riga_intervento(v[static_cast<std::size_t>(riga2_ - CAMPI)]);
    } else if (riga2_ >= CAMPI && riga2_ - CAMPI < static_cast<int>(v.size())) {
        v[static_cast<std::size_t>(riga2_ - CAMPI)].tempo = *ora_;
        ordina(riga2_ - CAMPI);
    }
    ora_.reset();
    nuovo_.reset();
}

void SchermataRegolazioni::togli_scelto() {
    auto& v = esperimento_.interventi;
    const int i = riga2_ - CAMPI;
    if (i < 0 || i >= static_cast<int>(v.size())) return;
    avviso_ = "TOLTO: " + riga_intervento(v[static_cast<std::size_t>(i)]);
    v.erase(v.begin() + i);
    riga2_ = std::min(riga2_, CAMPI + static_cast<int>(v.size()) - 1);
}

void SchermataRegolazioni::apri_caricamento() {
    std::vector<VoceCaricamento> v;
    const auto impronta = impronta_corrente();
    const auto aggiungi = [&](const std::filesystem::path& cartella, bool pronto) {
        for (const auto& p : elenco_esperimenti(cartella)) {
            VoceCaricamento x;
            x.file = p;
            x.pronto = pronto;
            try {
                const Esperimento e = leggi_esperimento(p);
                x.titolo = e.titolo.empty() ? p.stem().string() : e.titolo;
                if (const auto k = leggi_codice_sandbox(e.codice); k && impronta) x.altra_missione = k->partita.impronta != *impronta;
            } catch (const std::exception&) {
                x.titolo = p.stem().string();
                x.rovinato = true;
            }
            v.push_back(std::move(x));
        }
    };
    aggiungi(cartella_utente(), false);
    if (!cartella_pronti_.empty()) aggiungi(cartella_pronti_, true);
    if (v.empty()) {
        avviso_ = "NESSUN ESPERIMENTO DA CARICARE";
        return;
    }
    caricamento_ = std::move(v);
    scelta_caricamento_ = 0;
}

void SchermataRegolazioni::carica(const std::filesystem::path& p) {
    Esperimento e;
    try {
        e = leggi_esperimento(p);
    } catch (const std::exception&) {
        avviso_ = "IL FILE NON SI LEGGE: " + p.filename().string();
        return;
    }
    const auto k = leggi_codice_sandbox(e.codice);
    if (const auto impronta = impronta_corrente(); impronta && k && k->partita.impronta != *impronta) {
        avviso_ = "L'ESPERIMENTO E' DI UN'ALTRA MISSIONE";
        return;
    }
    if (k) r_ = k->regolazioni;   // le regolazioni dell'esperimento anche nella prima pagina
    codice_caricato_ = e.codice;
    regolazioni_cambiate_ = false;
    e.codice.clear();
    for (Intervento& x : e.interventi) {
        x.id = -1;
        x.tempo = std::max(0.0, x.tempo);   // un «subito» nel file: dall'inizio
    }
    std::stable_sort(e.interventi.begin(), e.interventi.end(), [](const Intervento& a, const Intervento& b) { return a.tempo < b.tempo; });
    esperimento_ = std::move(e);
    riga2_ = esperimento_.interventi.empty() ? 0 : CAMPI;
    std::string titolo = esperimento_.titolo.empty() ? p.stem().string() : esperimento_.titolo;
    avviso_ = "CARICATO: " + maiuscolo(titolo);
}

void SchermataRegolazioni::salva(const std::string& nome) {
    const Esperimento e = esperimento();
    if (e.codice.empty()) {
        avviso_ = "MANCA IL CODICE SANDBOX DELLA PARTITA: NON SI PUO' SALVARE";
        return;
    }
    const auto cartella = cartella_utente();
    const auto file = cartella / nome_file(nome);
    try {
        std::error_code ec;
        std::filesystem::create_directories(cartella, ec);
        scrivi_esperimento(file, e);
        avviso_ = "SALVATO: " + maiuscolo(file.filename().string());
    } catch (const std::exception&) {
        avviso_ = "NON SI PUO' SCRIVERE " + maiuscolo(file.string());
    }
}

SchermataRegolazioni::Uscita SchermataRegolazioni::comandi_interventi(const Comandi& c, double dt) {
    if (c.esci) return Uscita::indietro;
    // le frecce a scatti, come nella prima pagina
    const int su_giu = passi_freccia(c.su_giu, c.passi_su_giu, prec_su_giu_), ruota = passi_freccia(c.ruota, c.passi_ruota, prec_ruota_);
    const auto ha = [&](char l) { return std::find(c.lettere.begin(), c.lettere.end(), l) != c.lettere.end(); };
    auto& v = esperimento_.interventi;

    if (campo_) {   // un testo in scrittura prende tutto
        campo_->buffer += c.testo;
        if (c.cancella && !campo_->buffer.empty()) {   // un carattere UTF-8 intero
            while (!campo_->buffer.empty() && (static_cast<unsigned char>(campo_->buffer.back()) & 0xC0) == 0x80) campo_->buffer.pop_back();
            if (!campo_->buffer.empty()) campo_->buffer.pop_back();
        }
        if (c.invio) {
            const Campo k = *campo_;
            campo_.reset();
            switch (k.quale) {
                case CampoTesto::titolo: esperimento_.titolo = k.buffer; break;
                case CampoTesto::mostra: esperimento_.mostra = k.buffer; break;
                case CampoTesto::ipotesi: esperimento_.ipotesi = k.buffer; break;
                case CampoTesto::nome_file: salva(k.buffer); break;
            }
        } else if (c.esc) {
            campo_.reset();
        }
        return Uscita::nessuna;
    }
    if (pannello_.aperto()) {   // il pannello prende tastiera e clic; su e giu' scorrono le voci come Tab
        const Cartina k = cartina(unita_);
        for (const auto& [x, y] : c.clic)
            if (pannello_.aspetta_punto() && dentro(CARTINA, x, y)) pannello_.punto_scelto(k.a_mondo(x, y));
        Comandi p = c;
        if (p.tab == 0 && su_giu != 0) p.tab = -su_giu;
        if (const auto q = pannello_.comandi(p, dt)) {   // composto: poi l'ora
            nuovo_ = intervento_di(*q);
            ora_ = std::clamp(q->tempo, 0.0, ORA_MAX_S);
        }
        return Uscita::nessuna;
    }
    if (ora_) {   // l'ora: sinistra e destra un minuto (Maiuscolo dieci), su e giu' dieci secondi
        if (c.esc) {
            ora_.reset();
            nuovo_.reset();
        } else if (c.invio) {
            conferma_ora();
        } else {
            ora_ = std::clamp(*ora_ + ruota * (c.veloce ? 600.0 : 60.0) + su_giu * 10.0, 0.0, ORA_MAX_S);
        }
        return Uscita::nessuna;
    }
    if (caricamento_) {   // la lista dei file: su e giu' (o Tab), Invio carica, Esc chiude
        const int n = static_cast<int>(caricamento_->size());
        if (c.esc) {
            caricamento_.reset();
            return Uscita::nessuna;
        }
        const int passo = c.tab != 0 ? c.tab : -su_giu;
        if (passo != 0) scelta_caricamento_ = std::clamp(scelta_caricamento_ + passo, 0, n - 1);
        for (const auto& [x, y] : c.clic)
            for (int i = 0; i < std::min(n, LISTA_RIGHE); ++i)
                if (dentro(area_lista(i), x, y)) {
                    const int j = inizio_lista(scelta_caricamento_) + i;
                    if (j >= n) break;
                    if (j == scelta_caricamento_) {   // il clic sulla voce scelta la carica
                        const auto p = (*caricamento_)[static_cast<std::size_t>(j)].file;
                        caricamento_.reset();
                        carica(p);
                        return Uscita::nessuna;
                    }
                    scelta_caricamento_ = j;
                }
        if (c.invio) {
            const auto p = (*caricamento_)[static_cast<std::size_t>(scelta_caricamento_)].file;
            caricamento_.reset();
            carica(p);
        }
        return Uscita::nessuna;
    }

    if (c.esc || c.tab != 0) {   // di nuovo le regolazioni
        pagina_ = Pagina::regolazioni;
        return Uscita::nessuna;
    }
    const int righe = CAMPI + static_cast<int>(v.size());
    const auto attiva = [&](int r) {   // Invio, o il clic sulla riga gia' scelta
        if (r == 0) campo_ = Campo{CampoTesto::titolo, esperimento_.titolo};
        else if (r == 1) campo_ = Campo{CampoTesto::mostra, esperimento_.mostra};
        else if (r == 2) campo_ = Campo{CampoTesto::ipotesi, esperimento_.ipotesi};
        else if (r >= CAMPI && r < righe) ora_ = v[static_cast<std::size_t>(r - CAMPI)].tempo;
    };
    const auto confronto = [&] {
        if (v.empty()) {
            avviso_ = "NESSUN INTERVENTO PROGRAMMATO: N PER AGGIUNGERNE UNO";
            return false;
        }
        return true;
    };
    if (su_giu) riga2_ = std::clamp(riga2_ - su_giu, 0, righe - 1);
    if (ruota) {
        if (riga2_ == 3) esperimento_.semi = std::clamp(esperimento_.semi + ruota * (c.veloce ? 10 : 1), 1, SEMI_MAX);
        else if (riga2_ >= CAMPI && riga2_ < righe) {
            auto& x = v[static_cast<std::size_t>(riga2_ - CAMPI)];
            x.tempo = std::clamp(x.tempo + ruota * (c.veloce ? 600.0 : 60.0), 0.0, ORA_MAX_S);
            ordina(riga2_ - CAMPI);
        }
    }
    if (c.invio) attiva(riga2_);
    if (c.cancella || ha('T')) togli_scelto();
    if (ha('N')) {
        apri_pannello();
        return Uscita::nessuna;
    }
    if (ha('S')) {   // il nome del file: dal titolo, se c'e'
        std::string nome = nome_file(esperimento_.titolo.empty() ? titolo_ : esperimento_.titolo);
        campo_ = Campo{CampoTesto::nome_file, nome.substr(0, nome.size() - 6)};
        return Uscita::nessuna;
    }
    if (ha('C')) {
        apri_caricamento();
        return Uscita::nessuna;
    }
    if (ha('A') && confronto()) return Uscita::esperimento;
    for (const auto& [x, y] : c.clic) {
        if (x >= SCHEDA_X && y < SCHEDA_H) {
            pagina_ = Pagina::regolazioni;
            return Uscita::nessuna;
        }
        if (y >= PIEDE_TASTI_Y && y < PIEDE_TASTI_Y + PIEDE_TASTI_H) {   // i tre tasti del piede, come nella prima pagina
            if (x < 210) {
                pagina_ = Pagina::regolazioni;
                return Uscita::nessuna;
            }
            if (x >= 218 && x < 420) {
                apri_pannello();
                return Uscita::nessuna;
            }
            if (x >= 428 && confronto()) return Uscita::esperimento;
            continue;
        }
        for (int r = 0; r < CAMPI; ++r)
            if (dentro(area_campo(r), x, y)) {
                if (r == riga2_) attiva(r);
                riga2_ = r;
            }
        const int inizio = inizio_lista(riga2_ - CAMPI);
        for (int i = 0; i < LISTA_RIGHE; ++i)
            if (dentro(area_lista(i), x, y) && inizio + i < static_cast<int>(v.size())) {
                if (CAMPI + inizio + i == riga2_) attiva(riga2_);
                riga2_ = CAMPI + inizio + i;
            }
    }
    return Uscita::nessuna;
}

void SchermataRegolazioni::disegna_interventi(Tela& t, const Font& f) const {
    namespace la = lamiera;
    la::lamiera(t, {0, 0, LARGHEZZA_TELA, ALTEZZA_TELA}, 7);
    la::targa(t, f, LARGHEZZA_TELA / 2, 6, "INTERVENTI PROGRAMMATI");
    la::incisa(t, f, 10, 10, maiuscolo(titolo_), CIANO_CAMBIATO);
    scheda(t, f, "REGOLAZIONI");
    la::incisa(t, f, 16, 32,
               taglia(f, avviso_.empty() ? "IL VETRINO DI DIO: LO STESSO SCENARIO SENZA (A) E CON (B) GLI INTERVENTI, SUGLI STESSI SEMI." : avviso_, 608),
               avviso_.empty() ? colori::BIANCO : colori::GIALLO);
    const auto& v = esperimento_.interventi;
    const bool modale = campo_ || ora_ || caricamento_ || pannello_.aperto();

    // l'esperimento: quattro campi, l'etichetta incisa e il valore nel vetro
    la::pannello(t, {ESP_X, ESP_Y, ESP_W, ESP_H}, 8);
    la::secondaria(t, f, ESP_X + 12, ESP_Y + 8, "L'ESPERIMENTO");
    char semi_txt[48];
    std::snprintf(semi_txt, sizeof semi_txt, "%d COPPIE A/B, DAL SEME %llu", esperimento_.semi,
                  static_cast<unsigned long long>(esperimento_.da));
    const std::string valori[CAMPI] = {esperimento_.titolo, esperimento_.mostra, esperimento_.ipotesi, semi_txt};
    static constexpr const char* ETICHETTE[CAMPI] = {"TITOLO", "COSA MOSTRA", "IPOTESI", "SEMI"};
    for (int r = 0; r < CAMPI; ++r) {
        const Rettangolo a = area_campo(r);
        const bool scelta = r == riga2_;
        const Rettangolo w{a.x + 124, a.y + 1, a.w - 126, 14};
        la::incisa(t, f, a.x + 4, a.y + 4, ETICHETTE[r], scelta ? colori::GIALLO : colori::BIANCO);
        la::vetro(t, w);
        const std::string testo = valori[r].empty() ? (r == 2 ? "(FACOLTATIVA)" : "(VUOTO: INVIO PER SCRIVERE)") : maiuscolo(valori[r]);
        f.scrivi(t, w.x + 4, w.y + 3, taglia(f, testo, w.w - 8), scelta ? colori::GIALLO : valori[r].empty() ? colori::GRIGIO_SCURO : CIANO_CAMBIATO, 1);
        if (scelta) la::smusso(t, {w.x - 1, w.y - 1, w.w + 2, w.h + 2}, colori::GIALLO, colori::GIALLO);
    }

    // gli interventi: l'ora e la riga del pannello, la pagina che contiene la scelta
    la::pannello(t, {INT_X, INT_Y, INT_W, INT_H}, 8);
    char testata[64];
    std::snprintf(testata, sizeof testata, "INTERVENTI PROGRAMMATI: %d", static_cast<int>(v.size()));
    la::secondaria(t, f, INT_X + 12, INT_Y + 8, testata);
    if (pannello_.aperto()) {   // la cartina delle unita' e il pannello
        la::vetro(t, CARTINA);
        const Cartina k = cartina(unita_);
        t.imposta_ritaglio(CARTINA);
        for (const auto& u : unita_) {
            const Vec2 q = k.a_tela(u.posizione);
            const int x = static_cast<int>(std::lround(q.x)), y = static_cast<int>(std::lround(q.y));
            t.rettangolo_pieno(x - 1, y - 1, 3, 3, colore_parte(u.unita.parte));
            f.scrivi(t, x + 4, y - 3, std::to_string(u.unita.id), colori::GRIGIO, 1);
        }
        for (const auto& x : v)   // i punti degli interventi gia' programmati
            if (x.punto) {
                const Vec2 q = k.a_tela(*x.punto);
                const int px = static_cast<int>(std::lround(q.x)), py = static_cast<int>(std::lround(q.y));
                for (int d = -3; d <= 3; ++d) t.pixel(px + d, py, MAGENTA), t.pixel(px, py + d, MAGENTA);
            }
        t.togli_ritaglio();
        char lato[48];
        std::snprintf(lato, sizeof lato, "NORD IN ALTO, LATO %.0f KM", k.metri_per_pixel * CARTINA.w / 1000.0);
        f.scrivi(t, CARTINA.x + 4, CARTINA.y + CARTINA.h - 10, lato, colori::GRIGIO_SCURO, 1);
        if (pannello_.aspetta_punto()) f.scrivi(t, CARTINA.x + 4, CARTINA.y + 4, "CLIC SULLA CARTINA: IL PUNTO", colori::GIALLO, 1);
        pannello_.disegna(t, f, PANNELLO);
    } else if (caricamento_) {   // i file da caricare: prima quelli dell'utente, poi i pronti
        la::vetro(t, {INT_X + 6, LISTA_Y - 2, INT_W - 12, LISTA_RIGHE * LISTA_RIGA + 4});
        f.scrivi(t, INT_X + 220, INT_Y + 8, "CARICA: SU E GIU', INVIO, ESC", colori::GIALLO, 1);
        const int n = static_cast<int>(caricamento_->size()), inizio = inizio_lista(scelta_caricamento_);
        for (int i = 0; i < LISTA_RIGHE && inizio + i < n; ++i) {
            const VoceCaricamento& x = (*caricamento_)[static_cast<std::size_t>(inizio + i)];
            const Rettangolo a = area_lista(i);
            const bool scelta = inizio + i == scelta_caricamento_;
            const std::string tag = x.rovinato ? "ROVINATO" : x.altra_missione ? "ALTRA MISSIONE" : x.pronto ? "PRONTO" : "TUO";
            const int wt = f.larghezza(tag, 1);
            f.scrivi(t, a.x + 6, a.y + 3, taglia(f, maiuscolo(x.titolo + "  (" + x.file.filename().string() + ")"), a.w - wt - 24),
                     scelta ? colori::GIALLO : x.rovinato || x.altra_missione ? colori::GRIGIO_SCURO : colori::GRIGIO, 1);
            f.scrivi(t, a.x + a.w - 6 - wt, a.y + 3, tag, x.pronto ? colori::VERDE_CHIARO : CIANO_CAMBIATO, 1);
            if (scelta) la::smusso(t, a, colori::GIALLO, colori::GIALLO);
        }
    } else {
        la::vetro(t, {INT_X + 6, LISTA_Y - 2, INT_W - 12, LISTA_RIGHE * LISTA_RIGA + 4});
        if (v.empty()) f.scrivi(t, INT_X + 14, LISTA_Y + 3, "NESSUN INTERVENTO: N PER AGGIUNGERNE UNO COL PANNELLO", colori::GRIGIO, 1);
        const int inizio = inizio_lista(riga2_ - CAMPI);
        for (int i = 0; i < LISTA_RIGHE && inizio + i < static_cast<int>(v.size()); ++i) {
            const Rettangolo a = area_lista(i);
            const bool scelta = CAMPI + inizio + i == riga2_;
            f.scrivi(t, a.x + 6, a.y + 3, taglia(f, riga_intervento(v[static_cast<std::size_t>(inizio + i)]), a.w - 12),
                     scelta ? colori::GIALLO : colori::GRIGIO, 1);
            if (scelta) la::smusso(t, a, colori::GIALLO, colori::GIALLO);
        }
        if (inizio + LISTA_RIGHE < static_cast<int>(v.size()))
            f.scrivi(t, INT_X + INT_W - 60, INT_Y + 8, "ALTRI...", colori::GRIGIO, 1);
    }
    if (ora_) {   // l'ora: il riquadro sopra la lista
        const Rettangolo r{120, 236, 400, 74};
        la::pannello(t, r, 9);
        la::secondaria(t, f, r.x + 12, r.y + 8, nuovo_ ? "ORA DEL NUOVO INTERVENTO" : "ORA DELL'INTERVENTO");
        const Rettangolo w{r.x + 12, r.y + 24, r.w - 24, 22};
        la::vetro(t, w);
        const std::string o = orologio(*ora_);
        f.scrivi(t, w.x + (w.w - f.larghezza(o, 2)) / 2, w.y + 3, o, colori::GIALLO, 2);
        f.scrivi(t, r.x + 12, r.y + 54, taglia(f, "FRECCE 1 MIN (MAIUSC. 10), SU E GIU' 10 S, INVIO, ESC", r.w - 24), colori::BIANCO, 1);
    }
    if (campo_) {   // il testo in scrittura
        static constexpr const char* NOMI[] = {"TITOLO", "COSA MOSTRA", "IPOTESI", "NOME DEL FILE (.M688V)"};
        const Rettangolo r{40, 246, 560, 56};
        la::pannello(t, r, 9);
        la::secondaria(t, f, r.x + 12, r.y + 8, std::string(NOMI[static_cast<int>(campo_->quale)]) + " (INVIO CONFERMA, ESC ANNULLA)");
        const Rettangolo w{r.x + 12, r.y + 26, r.w - 24, 16};
        la::vetro(t, w);
        std::string s = maiuscolo(campo_->buffer) + "_";
        while (f.larghezza(s, 1) > w.w - 8 && s.size() > 1) s.erase(0, 1);   // la coda, dove si scrive
        f.scrivi(t, w.x + 4, w.y + 4, s, colori::BIANCO, 1);
    }
    const char* aiuto = modale ? "" : "SU E GIU' LA RIGA, INVIO SCRIVE, FRECCE SEMI E ORA. T TOGLIE, S SALVA, C CARICA, TAB REGOLAZIONI.";
    const la::Stato stato = modale ? la::Stato::disabilitato : la::Stato::normale;
    la::piede(t, f, PIEDE_Y, aiuto,
              {{"ESC  REGOLAZIONI", ColoreLampada::rossa, stato}, {"N  NUOVO INTERVENTO", ColoreLampada::ambra, stato},
               {"A  CONFRONTO A/B", ColoreLampada::verde, v.empty() ? la::Stato::disabilitato : stato}}, 2);
}

}  // namespace m688
