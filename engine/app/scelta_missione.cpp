#include "scelta_missione.hpp"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <regex>
#include <set>

#include "carta.hpp"
#include "codice_partita.hpp"
#include "grafica/consolle.hpp"
#include "grafica/lamiera.hpp"
#include "grafica/palette.hpp"
#include "grafica/tema_stazioni.hpp"
#include "museo.hpp"
#include "stazione_radio.hpp"

namespace m688 {

namespace {

// La variante A «lamiera di bordo» (roadmap 33, docs/ricerca/25_bozze, scelta dell'utente del 28/10/2026): due
// pannelli incassati (l'elenco e la scheda), le schede come tasti a spia, il piede comune di lamiera::piede
constexpr int PIEDE_Y = 392, PIEDE_TASTI_Y = PIEDE_Y + 22, PIEDE_TASTI_H = 26;
constexpr int PANNELLO_Y = 52, PANNELLO_H = 336;
constexpr int LISTA_X = 8, LISTA_Y = 76, LISTA_W = 400, RIGA = 30;
constexpr int SCHEDA_X = 414, SCHEDA_W = 218;
constexpr int COL_688 = LISTA_X + 300, COL_ALFA = LISTA_X + 348, CELLA_W = 28;
constexpr int SCHEDE_Y = 30, SCHEDA_TAB_W = 150, SCHEDA_TAB_PASSO = 158;
constexpr int CAMPI_RAPIDA = 13, RIGA_RAPIDA = 23;   // i parametri della rapida, l'ultimo e' il codice
// i tre tasti del piede: (624 - 16) / 3 = 202 di larghezza
constexpr int PIEDE_ESCI_X1 = 210, PIEDE_CENTRO_X0 = 218, PIEDE_CENTRO_X1 = 420, PIEDE_AVANTI_X0 = 428;
constexpr std::uint8_t ROSSO_CHIARO = 12;   // il rosso chiaro della palette del giorno (la lente dell'Alfa)
bool nel_piede(int y) { return y >= PIEDE_TASTI_Y && y < PIEDE_TASTI_Y + PIEDE_TASTI_H; }

std::string maiuscolo(std::string s) {
    for (auto& c : s) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return s;
}

void cornice(Tela& t, int x, int y, int w, int h, std::uint8_t col) {
    t.linea_orizzontale(x, x + w - 1, y, col);
    t.linea_orizzontale(x, x + w - 1, y + h - 1, col);
    t.linea_verticale(x, y, y + h - 1, col);
    t.linea_verticale(x + w - 1, y, y + h - 1, col);
}

}  // namespace

std::vector<MissioneInElenco> elenco_missioni(const std::string& cartella) {
    std::vector<MissioneInElenco> v;
    // delimitatori «re»: nei modelli ci sono le virgolette chiuse da parentesi
    const std::regex numero(R"re(^\s*missione\s*=\s*(\d+))re"),
        titolo(R"re(titolo\s*=\s*\{\s*it\s*=\s*"([^"]*)"\s*,\s*en\s*=\s*"([^"]*)")re"),
        lato(R"re(^\s*lato\s*=\s*"(\w+)")re"), teatro(R"re(^\s*teatro\s*=\s*"(\w+)")re"), id(R"re(^\s*id\s*=\s*"(\w+)")re");
    std::error_code ec;
    for (const auto& e : std::filesystem::directory_iterator(cartella, ec)) {
        if (e.path().extension() != ".lua" || e.path().filename().string().rfind("SCENE", 0) != 0) continue;
        std::ifstream f(e.path());
        MissioneInElenco m;
        std::string riga;
        std::smatch r;
        for (int k = 0; k < 60 && std::getline(f, riga); ++k) {   // l'intestazione della missione, senza caricarla
            if (m.id.empty() && std::regex_search(riga, r, id)) m.id = r[1];
            if (!m.numero && std::regex_search(riga, r, numero)) m.numero = std::stoi(r[1]);
            if (m.titolo_it.empty() && std::regex_search(riga, r, titolo)) { m.titolo_it = r[1]; m.titolo_en = r[2]; }
            if (m.lato.empty() && std::regex_search(riga, r, lato)) m.lato = r[1];
            if (m.teatro.empty() && std::regex_search(riga, r, teatro)) m.teatro = r[1];
        }
        if (!m.id.empty() && m.numero) v.push_back(m);
    }
    std::sort(v.begin(), v.end(), [](const MissioneInElenco& a, const MissioneInElenco& b) { return a.numero != b.numero ? a.numero < b.numero : a.id < b.id; });
    return v;
}

SchermataScelta::SchermataScelta(std::vector<MissioneInElenco> missioni, std::string dati) : missioni_(std::move(missioni)), dati_(std::move(dati)) {
    std::set<int> n;
    for (const auto& m : missioni_) n.insert(m.numero);
    numeri_.assign(n.begin(), n.end());
    if (const MissioneInElenco* m = voce(numeri_.empty() ? 0 : numeri_.front(), false)) opzioni_.missione = m->id;
    // il seme della prima rapida dall'orologio (le missioni rapide sono riproducibili dal codice, non dall'avvio)
    rapida_.seme = static_cast<std::uint32_t>(std::chrono::steady_clock::now().time_since_epoch().count() * 2654435761ull >> 16);
}

const MissioneRapida* SchermataScelta::anteprima() const {
    if (anteprima_di_ != rapida_) {
        anteprima_di_ = rapida_;
        anteprima_.reset();
        errore_.clear();
        try {
            anteprima_ = genera_missione_rapida(rapida_, dati_);
        } catch (const std::exception& e) {
            const ParametriRapida r = risolvi(rapida_, dati_);
            errore_ = !compatibile(r.teatro, r.tipo, r.alfa, dati_) ? "TIPO NON POSSIBILE IN QUESTO TEATRO CON QUESTA PARTE"
                                                                   : testo_per_font(e.what());
        }
    }
    return anteprima_ ? &*anteprima_ : nullptr;
}

// Il codice digitato (P in ogni scheda, o Invio sulla riga del codice della rapida): lettere e cifre, i trattini si
// possono omettere. Quello di una rapida porta alla scheda Missione rapida coi suoi parametri; quello di una partita
// cerca la missione dell'impronta fra originali, libreria, missioni dell'editor e importate, e parte subito con il
// seme e le opzioni del codice
std::optional<SceltaPartita> SchermataScelta::comandi_codice(const Comandi& c, bool& esci) {
    esci = c.esci;
    for (const char ch : c.testo) {
        const char u = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        if ((std::isalnum(static_cast<unsigned char>(u)) || u == '-') && codice_.size() < 40) codice_ += u;   // fino al codice sandbox coi trattini
    }
    if (c.cancella && !codice_.empty()) codice_.pop_back();
    if (c.esc) scrive_codice_ = false, avviso_.clear();
    if (!c.invio) return std::nullopt;
    scrive_codice_ = false;
    if (const auto p = leggi_codice(codice_)) {
        rapida_ = *p;
        scheda_ = 2;
        avviso_ = "CODICE LETTO: LA MISSIONE E' QUELLA DEL CODICE";
        return std::nullopt;
    }
    std::optional<RegolazioniSandbox> del_vetrino;   // un codice sandbox: la sua base e le regolazioni
    std::optional<CodicePartita> k = leggi_codice_partita(codice_);
    if (!k)
        if (const auto ks = leggi_codice_sandbox(codice_)) k = ks->partita, del_vetrino = ks->regolazioni;
    if (!k) {
        avviso_ = "CODICE NON VALIDO";
        return std::nullopt;
    }
    std::vector<std::filesystem::path> cartelle;
    if (!cartella_missioni_.empty())   // prove: i vetrini pronti della sandbox
        cartelle = {cartella_missioni_, std::filesystem::path(cartella_missioni_) / "libreria", std::filesystem::path(cartella_missioni_) / "prove"};
    cartelle.push_back(cartella_missioni_editor());
    cartelle.push_back(cartella_missioni_importate());
    const auto m = trova_missione(k->impronta, cartelle);
    if (!m) {
        avviso_ = "MISSIONE DEL CODICE NON TROVATA, O CAMBIATA DA ALLORA";
        return std::nullopt;
    }
    opzioni_.anni_89 = k->anni_89;
    opzioni_.carta_epoca = k->carta_epoca;
    opzioni_.meteo_realistico = k->meteo_realistico;
    opzioni_.asw = k->asw;
    opzioni_.sonarista = k->sonarista;
    opzioni_.navigazione_realistica = k->navigazione;
    opzioni_.gps = k->gps;
    SceltaPartita s = opzioni_;
    s.missione = m->id;
    s.seme = k->seme;
    s.sandbox = del_vetrino.has_value();
    s.regolazioni = del_vetrino;
    if (k->origine == OrigineCodice::originale) {   // la riga della missione nell'elenco, per quando si torna
        for (std::size_t i = 0; i < numeri_.size(); ++i)
            for (const bool alfa : {false, true})
                if (const MissioneInElenco* v = voce(numeri_[i], alfa); v && v->id == m->id) riga_ = static_cast<int>(i), alfa_ = alfa;
        opzioni_.missione = m->id;
        return s;
    }
    std::ifstream f(m->file, std::ios::binary);
    const std::string testo{std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};
    auto sc = leggi_scenario(testo);
    if (!sc) {
        avviso_ = "MISSIONE ILLEGGIBILE: " + m->file.filename().string();
        return std::nullopt;
    }
    s.cartella_scenario = m->file.parent_path().string();
    s.scenario = std::move(*sc);
    return s;
}

std::optional<SceltaPartita> SchermataScelta::comandi_rapida(const Comandi& c, bool& esci) {
    esci = c.esc || c.esci;
    const int su = passi_freccia(c.su_giu, c.passi_su_giu, prec_su_giu_), lato = passi_freccia(c.ruota, c.passi_ruota, prec_ruota_);
    if (su) campo_ = std::clamp(campo_ - su, 0, CAMPI_RAPIDA - 1);
    const auto gira = [](int v, int d, int n) { return ((v + d) % n + n) % n; };
    if (lato) {
        ParametriRapida& p = rapida_;
        const int d = lato;
        avviso_.clear();
        switch (campo_) {
            case 0: {   // teatro: «a caso» e i sette teatri
                int i = 0;
                for (int k = 0; k < static_cast<int>(std::size(TEATRI_RAPIDE)); ++k)
                    if (p.teatro == TEATRI_RAPIDE[k]) i = k + 1;
                i = gira(i, d, static_cast<int>(std::size(TEATRI_RAPIDE)) + 1);
                p.teatro = i ? TEATRI_RAPIDE[i - 1] : "";
                break;
            }
            case 1: p.stagione = static_cast<StagioneRapida>(gira(static_cast<int>(p.stagione), d, 5)); break;
            case 2: p.ora = static_cast<MomentoRapido>(gira(static_cast<int>(p.ora), d, 5)); break;
            case 3: p.tipo = static_cast<TipoRapida>(gira(static_cast<int>(p.tipo), d, 8)); break;
            case 4: p.alfa = !p.alfa; break;
            case 5: p.forze = gira(p.forze, d, 4); break;
            case 6: p.paranoia = gira(p.paranoia, d, 4); break;
            case 7: p.difficolta = gira(p.difficolta, d, 6); break;
            case 8: p.asw_aereo = !p.asw_aereo; break;
            case 9: p.meteo_realistico = !p.meteo_realistico; break;
            case 10: p.carriera = !p.carriera; break;
            case 11: p.seme = p.seme * 1664525u + 1013904223u + (d < 0 ? 7u : 0u); break;   // un seme nuovo
            default: break;
        }
    }
    if (c.punto) rapida_.meteo_realistico = !rapida_.meteo_realistico;   // W, come nelle originali
    if (std::find(c.lettere.begin(), c.lettere.end(), 'E') != c.lettere.end())   // E: la rapida in vista nell'editor
        if (const MissioneRapida* m = anteprima()) {
            scenario_editor_ = m->scenario;
            editor_ = true;
            return std::nullopt;
        }
    bool conferma = c.invio;
    for (const auto& [x, y] : c.clic) {
        if (x < LISTA_X + LISTA_W && y >= LISTA_Y && y < LISTA_Y + CAMPI_RAPIDA * RIGA_RAPIDA) campo_ = (y - LISTA_Y) / RIGA_RAPIDA;
        if (nel_piede(y) && x >= PIEDE_AVANTI_X0) conferma = true;
        if (nel_piede(y) && x < PIEDE_ESCI_X1) esci = true;
    }
    if (conferma && campo_ == CAMPI_RAPIDA - 1 && c.invio) {   // Invio sulla riga del codice: si digita
        scrive_codice_ = true;
        codice_.clear();
        return std::nullopt;
    }
    if (conferma || vetrino_)
        if (const MissioneRapida* m = anteprima()) {
            SceltaPartita s = opzioni_;
            s.sandbox = vetrino_;
            s.missione = m->id;
            s.meteo_realistico = m->parametri.meteo_realistico;
            s.rapida = *m;
            return s;
        }
    return std::nullopt;
}

const MissioneInElenco* SchermataScelta::voce(int numero, bool alfa) const {
    for (const auto& m : missioni_)
        if (m.numero == numero && m.lato == (alfa ? "urss" : "usa")) return &m;
    return nullptr;
}

const Fondale* SchermataScelta::fondale(const std::string& teatro) const {
    auto it = fondali_.find(teatro);
    if (it == fondali_.end()) {
        std::unique_ptr<Fondale> f;
        try {
            f = std::make_unique<Fondale>(Fondale::carica(dati_ + "/teatri/" + teatro + ".fdl"));
        } catch (const std::exception&) {
        }
        it = fondali_.emplace(teatro, std::move(f)).first;
    }
    return it->second.get();
}

void SchermataScelta::imposta_classico(bool si) {
    classico_ = si;
    if (si) scheda_ = 1;
}

void SchermataScelta::vai_al_battello(bool alfa) {
    rapida_.alfa = alfa;
    alfa_ = alfa;
    if (numeri_.empty() || voce(numeri_[static_cast<std::size_t>(riga_)], alfa)) return;
    for (std::size_t i = 0; i < numeri_.size(); ++i)
        if (voce(numeri_[i], alfa)) {
            riga_ = static_cast<int>(i);
            return;
        }
}

std::optional<SceltaPartita> SchermataScelta::comandi(const Comandi& ingresso, bool& esci) {
    if (scrive_codice_) return comandi_codice(ingresso, esci);
    // il clic su una riga delle opzioni vale il suo tasto
    Comandi c = ingresso;
    for (const auto& [x, y] : ingresso.clic)
        for (const ZonaOpzione& z : zone_opzioni_) {
            if (x < z.r.x || y < z.r.y || x >= z.r.x + z.r.w || y >= z.r.y + z.r.h) continue;
            if (z.tasto == "TAB") c.tab = 1;
            else if (z.tasto == "W") c.punto = true;
            else if (z.tasto == "K") c.cancella_punto = true;
            else if (z.tasto == "S") c.siluro = true;
            else c.lettere.push_back(z.tasto[0]);   // D, N, M, A
        }
    // O, o il clic sul primo tasto in alto: classico o Enhanced
    bool cambia_modo = std::find(c.lettere.begin(), c.lettere.end(), 'O') != c.lettere.end();
    for (const auto& [x, y] : c.clic)
        cambia_modo = cambia_modo || (y >= SCHEDE_Y && y < SCHEDE_Y + 18 && x >= 8 && x < 8 + SCHEDA_TAB_W);
    if (cambia_modo) imposta_classico(!classico_);
    if (classico_) {   // solo le originali: niente schede, codici, vetrino, albo e opzioni dell'Enhanced
        vetrino_ = false;
        if (std::find(c.lettere.begin(), c.lettere.end(), 'M') != c.lettere.end()) musica_ = (musica_ + 1) % 6;
        if (std::find(c.lettere.begin(), c.lettere.end(), 'A') != c.lettere.end()) video_ = (video_ + 1) % 3;
        if (c.enciclopedia) museo_ = true;
        return comandi_originali(c, esci);
    }
    vetrino_ = std::find(c.lettere.begin(), c.lettere.end(), 'V') != c.lettere.end();   // V: la sandbox su questa missione
    if (std::find(c.lettere.begin(), c.lettere.end(), 'P') != c.lettere.end()) {   // P: un codice, in ogni scheda
        scrive_codice_ = true;
        codice_.clear();
        avviso_.clear();
        return std::nullopt;
    }
    // le schede: R passa fra le originali e la rapida, o il clic sulla scheda
    const int prima = scheda_;
    if (std::find(c.lettere.begin(), c.lettere.end(), 'R') != c.lettere.end()) scheda_ = scheda_ % 3 + 1;
    for (const auto& [x, y] : c.clic) {
        if (y >= SCHEDE_Y && y < SCHEDE_Y + 18 && x >= 8) {
            const int k = (x - 8) / SCHEDA_TAB_PASSO;
            if (k >= 1 && k <= 3) scheda_ = k;
        }
        if (nel_piede(y) && x >= PIEDE_CENTRO_X0 && x < PIEDE_CENTRO_X1) scheda_ = scheda_ % 3 + 1;   // il tasto di mezzo: R
    }
    if (scheda_ == 3 && prima != 3) ricarica_scenari();   // rilette a ogni ingresso
    // le opzioni della partita valgono per le due schede
    if (c.tab) opzioni_.anni_89 = !opzioni_.anni_89;
    // D, su ogni tastiera; [ ] resta per chi li ha (su quella italiana non arrivano: nota ghiaccio-d4-tastiera)
    if (c.minuti || std::find(c.lettere.begin(), c.lettere.end(), 'D') != c.lettere.end()) opzioni_.carta_epoca = !opzioni_.carta_epoca;
    if (c.cancella_punto) opzioni_.asw = (opzioni_.asw + 1) % 4;           // K
    if (c.siluro) opzioni_.sonarista = (opzioni_.sonarista + 1) % 4;       // S
    if (std::find(c.lettere.begin(), c.lettere.end(), 'M') != c.lettere.end()) musica_ = (musica_ + 1) % 6;   // M: musica
    if (std::find(c.lettere.begin(), c.lettere.end(), 'N') != c.lettere.end()) {   // N: perfetta, realistica, realistica col GPS
        if (!opzioni_.navigazione_realistica) opzioni_.navigazione_realistica = true, opzioni_.gps = false;
        else if (!opzioni_.gps) opzioni_.gps = true;
        else opzioni_.navigazione_realistica = opzioni_.gps = false;
    }
    if (std::find(c.lettere.begin(), c.lettere.end(), 'A') != c.lettere.end()) video_ = (video_ + 1) % 3;   // A: il video d'apertura
    if (c.enciclopedia) museo_ = true;                                      // F1
    if (c.cerca) albo_ = true;                                              // C
    if (scheda_ == 2) return comandi_rapida(c, esci);
    if (scheda_ == 3) return comandi_editor(c, esci);
    return comandi_originali(c, esci);
}

std::optional<SceltaPartita> SchermataScelta::comandi_originali(const Comandi& c, bool& esci) {
    esci = c.esc || c.esci;
    const int n = static_cast<int>(numeri_.size());
    if (n == 0) return std::nullopt;
    // frecce (un passo per pressione: i tasti tenuti arrivano a ogni fotogramma)
    if (const int su = passi_freccia(c.su_giu, c.passi_su_giu, prec_su_giu_)) riga_ = std::clamp(riga_ - su, 0, n - 1);
    if (const int lato = passi_freccia(c.ruota, c.passi_ruota, prec_ruota_)) alfa_ = lato > 0;
    for (const int k : c.cifre)   // le cifre scelgono la missione (0 = la decima)
        if (k >= 1 && k <= n) riga_ = k - 1;
    if (c.punto && !classico_) opzioni_.meteo_realistico = !opzioni_.meteo_realistico;   // W
    bool conferma = c.invio;
    for (const auto& [x, y] : c.clic) {
        if (x < LISTA_X + LISTA_W && y >= LISTA_Y && y < LISTA_Y + n * RIGA) {   // solo nell'elenco: a destra ci sono le opzioni
            riga_ = (y - LISTA_Y) / RIGA;
            if (x >= COL_688 && x < COL_688 + CELLA_W) alfa_ = false;
            if (x >= COL_ALFA && x < COL_ALFA + CELLA_W) alfa_ = true;
        }
        if (nel_piede(y) && x >= PIEDE_AVANTI_X0) conferma = true;
        if (nel_piede(y) && x < PIEDE_ESCI_X1) esci = true;
    }
    const MissioneInElenco* m = voce(numeri_[static_cast<std::size_t>(riga_)], alfa_);
    if (!m) m = voce(numeri_[static_cast<std::size_t>(riga_)], !alfa_);   // la missione ha un solo lato
    if (m) {
        alfa_ = m->lato == "urss";
        opzioni_.missione = m->id;
    }
    if ((conferma || vetrino_) && m) {
        SceltaPartita s = opzioni_;
        s.sandbox = vetrino_;
        s.classico = classico_;
        return s;
    }
    return std::nullopt;
}

void SchermataScelta::palette(Palette& p) const {
    applica_palette_carta(p, StileCarta::vga);   // la carta in miniatura (96-143, 199-231)
    applica_tema(p, TemaStazione{});             // per ultimo: la lamiera, i tasti e le lampade (16-47)
}

void SchermataScelta::disegna(Tela& t, const Font& f) const {
    namespace la = lamiera;
    la::lamiera(t, {0, 0, LARGHEZZA_TELA, ALTEZZA_TELA}, 7);
    la::targa(t, f, LARGHEZZA_TELA / 2, 6, "SCELTA DELLA MISSIONE");
    // i due comandi su una targhetta di vetro: sulla lamiera il rosso ha poca luce e la diagonale della N spariva («1ORD»)
    const auto comando = [&](int x, const char* nome, std::uint8_t colore) {
        la::vetro(t, {x - 4, 7, f.larghezza(nome, 1) + 8, 14});
        f.scrivi(t, x, 10, nome, colore, 1);
    };
    comando(14, "COMSUBLANT", colori::GIALLO);
    comando(LARGHEZZA_TELA - 14 - f.larghezza("FLOTTA DEL NORD", 1), "FLOTTA DEL NORD", ROSSO_CHIARO);
    // il modo di gioco, sempre in vista, poi le tre schede come tasti a spia; nel classico solo le originali
    static constexpr const char* SCHEDE[] = {"", "MISSIONI ORIGINALI", "MISSIONE RAPIDA", "EDITOR"};
    la::tasto(t, {8, SCHEDE_Y, SCHEDA_TAB_W, 18}, classico_ ? "O  MODO: CLASSICO" : "O  MODO: ENHANCED", classico_ ? ColoreLampada::ambra : ColoreLampada::verde,
              la::Stato::evidenziato, false, f);
    for (int k = 1; k < 4; ++k)
        la::tasto(t, {8 + k * SCHEDA_TAB_PASSO, SCHEDE_Y, SCHEDA_TAB_W, 18}, SCHEDE[k], ColoreLampada::ambra,
                  k == scheda_ ? la::Stato::evidenziato : classico_ ? la::Stato::disabilitato : la::Stato::normale, false, f);
    la::pannello(t, {LISTA_X, PANNELLO_Y, LISTA_W, PANNELLO_H}, 8);
    la::pannello(t, {SCHEDA_X, PANNELLO_Y, SCHEDA_W, PANNELLO_H}, 8);
    if (scheda_ == 2 || scheda_ == 3) {   // rapida ed editor: il loro contenuto sul vetro nero dei due pannelli
        la::vetro(t, {LISTA_X + 6, PANNELLO_Y + 6, LISTA_W - 12, PANNELLO_H - 12});
        la::vetro(t, {SCHEDA_X + 4, PANNELLO_Y + 4, SCHEDA_W - 8, PANNELLO_H - 8});
        if (scheda_ == 2) disegna_rapida(t, f);
        else disegna_editor(t, f);
        return;
    }
    // l'elenco con le spie 688 e Alfa
    la::secondaria(t, f, LISTA_X + 14, PANNELLO_Y + 8, "N. MISSIONE");
    la::incisa(t, f, COL_688 + 4, PANNELLO_Y + 8, "688", colori::BIANCO, colori::BLU);
    la::incisa(t, f, COL_ALFA, PANNELLO_Y + 8, "ALFA", colori::BIANCO, colori::ROSSO);
    for (std::size_t i = 0; i < numeri_.size(); ++i) {
        const int y = LISTA_Y + static_cast<int>(i) * RIGA, numero = numeri_[i];
        const bool scelta = static_cast<int>(i) == riga_;
        if (scelta) {
            la::vetro(t, {LISTA_X + 8, y - 3, LISTA_W - 16, RIGA - 2});
            lampada_rettangolare(t, {LISTA_X + 10, y + 5, 6, 10}, ColoreLampada::ambra, true);
        }
        const MissioneInElenco* qualsiasi = voce(numero, false) ? voce(numero, false) : voce(numero, true);
        const std::uint8_t testo = scelta ? colori::GIALLO : colori::BIANCO;
        la::incisa(t, f, LISTA_X + 20, y + 2, std::to_string(numero % 10) + ".", testo);
        if (qualsiasi) {
            la::incisa(t, f, LISTA_X + 40, y + 2, maiuscolo(qualsiasi->titolo_it), testo);
            if (scelta) f.scrivi(t, LISTA_X + 40, y + 13, maiuscolo(qualsiasi->teatro), colori::GRIGIO, 1);
            else la::secondaria(t, f, LISTA_X + 40, y + 13, maiuscolo(qualsiasi->teatro));
        }
        for (const bool alfa : {false, true}) {
            const bool c_e = voce(numero, alfa) != nullptr, sua = scelta && alfa == alfa_;
            const Rettangolo r{alfa ? COL_ALFA : COL_688, y + 1, 20, 18};
            la::spia(t, r, c_e, alfa ? ROSSO_CHIARO : static_cast<std::uint8_t>(9));   // 9: il blu chiaro
            if (sua) lamiera::smusso(t, {r.x - 2, r.y - 2, r.w + 4, r.h + 4}, lampada(ColoreLampada::ambra, true), lampada(ColoreLampada::ambra, false));
        }
    }
    // la scheda della missione scelta: titolo, lato, teatro con l'anteprima, opzioni della partita
    const MissioneInElenco* m = nullptr;
    for (const auto& x : missioni_)
        if (x.id == opzioni_.missione) m = &x;
    const int X = SCHEDA_X + 12;
    int y = PANNELLO_Y + 8;
    la::secondaria(t, f, X, y, "MISSIONE SCELTA");
    y += 16;
    if (m) {
        la::incisa(t, f, X, y, maiuscolo(m->titolo_it), colori::GIALLO);
        y += 12;
        if (maiuscolo(m->titolo_en) != maiuscolo(m->titolo_it)) {   // il titolo dell'originale, se la traduzione cambia
            la::secondaria(t, f, X, y, "(" + m->titolo_en + ")");
            y += 11;
        }
        la::secondaria(t, f, X, y, m->lato == "urss" ? "VMF - ALFA (PROGETTO 705)" : "US NAVY - LOS ANGELES (SSN-688)");
        y += 10;
        la::secondaria(t, f, X, y, "TEATRO: " + maiuscolo(m->teatro) + "   1989");
        y += 12;
        // anteprima del teatro sul vetro: il fondale intero a fasce
        const int ax = SCHEDA_X + 7, ay = y, aw = SCHEDA_W - 14, ah = 104;
        la::vetro(t, {ax - 1, ay - 1, aw + 2, ah + 2});
        if (const Fondale* fo = fondale(m->teatro)) {
            const double lx = fo->larghezza() * fo->cella_m(), ly = fo->altezza() * fo->cella_m();
            for (int py = 0; py < ah; ++py)
                for (int px = 0; px < aw; ++px) {
                    const Vec2 p{(px + 0.5) / aw * lx - lx / 2.0, ly / 2.0 - (py + 0.5) / ah * ly};
                    t.pixel(ax + px, ay + py, colore_carta(fo->profondita(p)));
                }
        }
        y += ah + 8;
    }
    disegna_opzioni(t, f, y, true);
    if (classico_) disegna_piede(t, f, "F1 MUSEO   O MODO ENHANCED: CARRIERA, BRIEFING, MISSIONI RAPIDE", "R  SCHEDA SUCCESSIVA", "INVIO  IN MARE");
    else disegna_piede(t, f, "F1 MUSEO   P CODICE   V VETRINO   C ALBO DEI COMANDANTI   R SCHEDA SUCCESSIVA", "R  SCHEDA SUCCESSIVA",
                       "INVIO  AL BRIEFING");
}

void SchermataScelta::disegna_piede(Tela& t, const Font& f, const char* aiuto, const char* centro, const char* avanti) const {
    lamiera::piede(t, f, PIEDE_Y, aiuto,
                   {{"ESC  ESCI", ColoreLampada::rossa},
                    {centro, ColoreLampada::ambra, classico_ ? lamiera::Stato::disabilitato : lamiera::Stato::normale},
                    {avanti, ColoreLampada::verde}}, 2);
    // il codice in scrittura e l'ultimo avviso al posto della riga d'aiuto, coi loro colori
    std::string riga;
    std::uint8_t colore = colori::GIALLO;
    if (scrive_codice_) {
        riga = "CODICE: " + codice_ + "_";
    } else if (!avviso_.empty()) {
        riga = avviso_;
        if (avviso_.rfind("CODICE LETTO", 0) != 0) colore = ROSSO_CHIARO;
    }
    if (riga.empty()) return;
    t.rettangolo_pieno(8, PIEDE_Y, 624, 13, colori::NERO);
    f.scrivi(t, 14, PIEDE_Y + 3, riga, colore, 1);
}

// Le opzioni della partita in fondo alla scheda, uguali per originali e rapide (il meteo della rapida e' fra i suoi
// parametri), con chi va in mare. Sulla lamiera (le originali) col tasto della tastiera accanto, sul vetro come testo
void SchermataScelta::disegna_opzioni(Tela& t, const Font& f, int y, bool su_lamiera) const {
    static constexpr const char* ASW[] = {"FACILE", "NORMALE", "REALISTICO", "MINACCIA PERCEPITA"};
    static constexpr const char* SONARISTA[] = {"NESSUNO", "RECLUTA", "MEDIO", "ESPERTO"};
    // M (roadmap 38): FM o MT-32 suonano il tema del menu dell'Enhanced, le tre del 1989 i titoli di Hubbard
    static constexpr const char* MUSICA[] = {"FM", "MT-32", "1989 ADLIB", "1989 TANDY", "1989 COVOX", "SPENTA"};
    std::vector<std::pair<const char*, std::string>> voci;
    if (!classico_) {
    voci = {
        {"TAB", std::string("STAZIONI: ") + (opzioni_.anni_89 ? "1989" : "MODERNE")},
        {"D", std::string("CARTA: ") + (opzioni_.carta_epoca ? "D'EPOCA" : "FONDALE VERO")},
    };
    if (scheda_ == 1) voci.push_back({"W", std::string("METEO: ") + (opzioni_.meteo_realistico ? "DEL TEATRO" : "MARE 3, SERENO")});
    // la stessa difficolta' per l'ASW aereo e per l'artiglieria delle navi (docs/16 §9.9)
    voci.push_back({"K", std::string("NEMICI: ") + ASW[std::clamp(opzioni_.asw, 0, 3)]});
    voci.push_back({"S", std::string("SONARISTA: ") + SONARISTA[std::clamp(opzioni_.sonarista, 0, 3)]});
    voci.push_back({"N", std::string("NAVIGAZIONE: ") + (!opzioni_.navigazione_realistica ? "PERFETTA" : opzioni_.gps ? "REALISTICA, GPS 1990+" : "REALISTICA")});
    }
    voci.push_back({"M", std::string("MUSICA: ") + MUSICA[std::clamp(musica_, 0, 5)]});
    static constexpr const char* VIDEO[] = {"SEMPRE", "LA PRIMA VOLTA", "MAI"};
    voci.push_back({"A", std::string("VIDEO D'APERTURA: ") + VIDEO[std::clamp(video_, 0, 2)]});
    const bool alfa = scheda_ == 2 ? (anteprima_ ? anteprima_->parametri.alfa : rapida_.alfa) : alfa_;
    const std::string& in_carica = alfa ? comandante_vmf_ : comandante_usn_;
    const std::string in_mare = classico_ ? std::string("IL GIOCO DEL 1989: NIENTE BRIEFING")
                                          : "IN MARE: " + (in_carica.empty() ? std::string("UN COMANDANTE NUOVO") : in_carica);
    const int X = SCHEDA_X + 12;
    zone_opzioni_.clear();
    if (su_lamiera) {
        lamiera::secondaria(t, f, X, y, "OPZIONI: CLIC O TASTO");
        y += 14;
        for (const auto& [tasto, testo] : voci) {
            zone_opzioni_.push_back({{X, y, SCHEDA_W - 16, 15}, tasto});
            lamiera::tasto_tastiera(t, {X, y, 26, 12}, tasto, f);
            lamiera::incisa(t, f, X + 32, y + 2, testo, colori::BIANCO);
            y += 15;
        }
        lamiera::incisa(t, f, X, y + 2, in_mare, classico_ || in_carica.empty() ? colori::BIANCO : colori::GIALLO);
        return;
    }
    f.scrivi(t, SCHEDA_X + 8, y, "OPZIONI: CLIC O TASTO", colori::CIANO_CHIARO, 1);
    y += 12;
    for (const auto& [tasto, testo] : voci) {   // il tasto e il testo in due colonne
        zone_opzioni_.push_back({{SCHEDA_X + 6, y - 1, SCHEDA_W - 12, 11}, tasto});
        f.scrivi(t, SCHEDA_X + 8, y, tasto, colori::CIANO_CHIARO, 1);
        f.scrivi(t, SCHEDA_X + 38, y, testo, colori::BIANCO, 1);
        y += 11;
    }
    f.scrivi(t, SCHEDA_X + 8, y, in_mare, in_carica.empty() ? colori::GRIGIO : colori::GIALLO, 1);
}

void SchermataScelta::ricarica_scenari() {
    // prima la libreria di missioni pronte del gioco, poi le missioni del giocatore, poi le importate
    scenari_.clear();
    importate_.clear();
    libreria_.clear();
    const auto aggiungi = [&](const std::filesystem::path& cartella, bool importate, bool libreria) {
        for (auto& v : elenco_scenari(cartella)) {
            scenari_.push_back(std::move(v));
            importate_.push_back(importate);
            libreria_.push_back(libreria);
        }
    };
    if (!cartella_missioni_.empty()) aggiungi(std::filesystem::path(cartella_missioni_) / "libreria", false, true);
    vetrini_ = cartella_missioni_.empty() ? std::vector<VetrinoPronto>{} : leggi_vetrini(std::filesystem::path(cartella_missioni_) / "prove");
    aggiungi(cartella_missioni_editor(), false, false);
    aggiungi(cartella_missioni_importate(), true, false);
}

// I: importa i pacchetti messi in «da_importare»; il codice dei pacchetti non entra mai, il file si riscrive dallo
// scenario (pacchetto_missione)
void SchermataScelta::importa() {
    esiti_import_.clear();
    std::error_code ec;
    std::filesystem::create_directories(cartella_da_importare(), ec);
    try {
        for (const auto& e : importa_cartella(cartella_da_importare(), cartella_missioni_importate(), carica_dati_scenario(dati_), dati_)) {
            const bool a_mano = std::any_of(e.problemi.begin(), e.problemi.end(), [](const std::string& p) {
                return p.find("codice del pacchetto") != std::string::npos;
            });
            std::string r = std::string(!e.importato ? "RIFIUTATA: " : a_mano ? "IMPORTATA, MODIFICATA A MANO: " : "IMPORTATA: ") + e.sorgente;
            if (e.importato && !e.titolo.empty()) r += " (" + e.titolo + ")";
            for (const auto& p : e.problemi) r += ". " + p;
            esiti_import_.push_back(testo_per_font(r));
        }
    } catch (const std::exception& e) {
        esiti_import_.push_back(testo_per_font(std::string("IMPORTAZIONE NON RIUSCITA: ") + e.what()));
    }
    if (esiti_import_.empty()) esiti_import_.push_back("NESSUN PACCHETTO DA IMPORTARE. I FILE .M688M RICEVUTI VANNO IN " + cartella_da_importare().string());
    ricarica_scenari();
}

// La scheda Editor: «Nuova missione» e le missioni salvate; Invio (o il clic sulla riga scelta) apre l'editor
std::optional<SceltaPartita> SchermataScelta::comandi_editor(const Comandi& c, bool& esci) {
    esci = c.esc || c.esci;
    if (std::find(c.lettere.begin(), c.lettere.end(), 'I') != c.lettere.end()) importa();
    // G: una missione salvata (della libreria, del giocatore o importata) si gioca subito, senza aprire l'editor
    // un vetrino pronto (V, G o Invio): sempre la sandbox, con le sue regolazioni consigliate (poi modificabili)
    const auto vetrino = [&]() -> std::optional<SceltaPartita> {
        const VetrinoPronto& p = vetrini_[static_cast<std::size_t>(riga_editor_ - 2)];
        std::ifstream f(p.file, std::ios::binary);
        const std::string testo{std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};
        auto sc = leggi_scenario(testo);
        if (!sc) {
            avviso_ = "MISSIONE ILLEGGIBILE: " + p.file.filename().string();
            return std::nullopt;
        }
        SceltaPartita s = opzioni_;
        s.missione = p.missione;
        s.meteo_realistico = p.regolazioni.meteo_realistico;
        s.cartella_scenario = p.file.parent_path().string();
        s.scenario = std::move(*sc);
        s.sandbox = true;
        s.regolazioni = p.regolazioni;
        return s;
    };
    const bool e_vetrino = riga_editor_ >= 2 && riga_editor_ < prima_scenario();
    if (e_vetrino && (vetrino_ || std::find(c.lettere.begin(), c.lettere.end(), 'G') != c.lettere.end())) return vetrino();
    if ((vetrino_ || std::find(c.lettere.begin(), c.lettere.end(), 'G') != c.lettere.end()) && riga_editor_ >= prima_scenario()) {
        const auto& v = scenari_[static_cast<std::size_t>(riga_editor_ - prima_scenario())];
        std::ifstream f(v.file, std::ios::binary);
        const std::string testo{std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};
        if (auto sc = leggi_scenario(testo)) {
            SceltaPartita s = opzioni_;
            s.missione = v.file.stem().string();
            s.meteo_realistico = sc->meteo_realistico;
            s.cartella_scenario = v.file.parent_path().string();
            s.scenario = std::move(*sc);
            s.sandbox = vetrino_;
            return s;
        }
        avviso_ = "MISSIONE ILLEGGIBILE: " + v.file.filename().string();
        return std::nullopt;
    }
    const int n = static_cast<int>(scenari_.size()) + prima_scenario();
    const int su = passi_freccia(c.su_giu, c.passi_su_giu, prec_su_giu_), lato = passi_freccia(c.ruota, c.passi_ruota, prec_ruota_);   // (riga 1: la scelta)
    if (su) riga_editor_ = std::clamp(riga_editor_ - su, 0, n - 1);
    if (lato && riga_editor_ == 1 && !missioni_.empty())   // la missione originale da convertire
        originale_ = ((originale_ + lato) % static_cast<int>(missioni_.size()) + static_cast<int>(missioni_.size())) %
                     static_cast<int>(missioni_.size());
    bool apri = c.invio;
    for (const auto& [x, y] : c.clic) {
        if (x < LISTA_X + LISTA_W && y >= LISTA_Y && y < LISTA_Y + n * RIGA_RAPIDA) {
            const int i = (y - LISTA_Y) / RIGA_RAPIDA;
            if (i == riga_editor_) apri = true;
            riga_editor_ = i;
        }
        if (nel_piede(y) && x >= PIEDE_AVANTI_X0) apri = true;
        if (nel_piede(y) && x < PIEDE_ESCI_X1) esci = true;
    }
    // una missione con la navigazione realistica di serie (LIB-I16, sotto il pack): scelta la riga, l'opzione N si
    // propone accesa, una volta; il giocatore puo' ancora toglierla
    if (riga_editor_ >= prima_scenario() && riga_editor_ != riga_nav_proposta_) {
        riga_nav_proposta_ = riga_editor_;
        std::ifstream f(scenari_[static_cast<std::size_t>(riga_editor_ - prima_scenario())].file, std::ios::binary);
        const std::string testo{std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};
        if (const auto sc = leggi_scenario(testo); sc && sc->navigazione && sc->navigazione->realistica_di_serie &&
                                                   !opzioni_.navigazione_realistica) {
            opzioni_.navigazione_realistica = true;
            opzioni_.gps = false;
            avviso_ = "QUESTA MISSIONE PROPONE LA NAVIGAZIONE REALISTICA (N PER CAMBIARE)";
        }
    }
    if (!apri) return std::nullopt;
    if (riga_editor_ >= 2 && riga_editor_ < prima_scenario()) return vetrino();   // un vetrino non si apre nell'editor
    scenario_editor_.reset();
    if (riga_editor_ == 1) {   // da una originale: unita', rotte, dotazione e briefing; la logica degli script no
        if (missioni_.empty()) return std::nullopt;
        note_originale_.clear();
        try {
            scenario_editor_ = scenario_da_originale(cartella_missioni_, missioni_[static_cast<std::size_t>(originale_)].id, dati_, &note_originale_);
        } catch (const std::exception& e) {
            avviso_ = testo_per_font(e.what());
            return std::nullopt;
        }
    } else if (riga_editor_ >= prima_scenario()) {   // una missione salvata: lo scenario dal suo blocco `editor`
        const auto& v = scenari_[static_cast<std::size_t>(riga_editor_ - prima_scenario())];
        std::ifstream f(v.file, std::ios::binary);
        const std::string testo{std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};
        scenario_editor_ = leggi_scenario(testo);
        if (!scenario_editor_) {
            avviso_ = "MISSIONE ILLEGGIBILE: " + v.file.filename().string();
            return std::nullopt;
        }
    }
    editor_ = true;
    return std::nullopt;
}

void SchermataScelta::disegna_editor(Tela& t, const Font& f) const {
    const auto oro = static_cast<std::uint8_t>(colori::GIALLO), grigio = static_cast<std::uint8_t>(colori::GRIGIO),
               bianco = static_cast<std::uint8_t>(colori::BIANCO);
    f.scrivi(t, LISTA_X + 10, LISTA_Y - 10, "LIBRERIA E MISSIONI DELL'EDITOR", colori::CIANO_CHIARO, 1);
    const int n = static_cast<int>(scenari_.size()) + prima_scenario(), visibili = (PIEDE_Y - 20 - LISTA_Y) / RIGA_RAPIDA;
    const int primo = std::clamp(riga_editor_ - visibili / 2, 0, std::max(0, n - visibili));
    for (int i = primo; i < n && i < primo + visibili; ++i) {
        const int y = LISTA_Y + (i - primo) * RIGA_RAPIDA;
        const bool qui = i == riga_editor_;
        if (qui) t.rettangolo_pieno(LISTA_X + 6, y, LISTA_W - 12, RIGA_RAPIDA - 4, colori::GRIGIO_SCURO);
        if (i == 0) {
            f.scrivi(t, LISTA_X + 8, y + 7, "+ NUOVA MISSIONE", qui ? oro : bianco, 1);
            continue;
        }
        if (i == 1) {
            std::string o = "+ DA UNA ORIGINALE: ";
            if (!missioni_.empty()) {
                const auto& m = missioni_[static_cast<std::size_t>(originale_)];
                o += m.id + " " + maiuscolo(m.titolo_it);
            }
            if (qui) o = "< " + o + " >";
            while (f.larghezza(o, 1) > LISTA_W - 16 && o.size() > 3) o.pop_back();
            f.scrivi(t, LISTA_X + 8, y + 7, o, qui ? oro : bianco, 1);
            continue;
        }
        if (i < prima_scenario()) {   // un vetrino pronto della sandbox
            std::string titolo = "V " + maiuscolo(testo_per_font(vetrini_[static_cast<std::size_t>(i - 2)].titolo));
            while (f.larghezza(titolo, 1) > 250 && titolo.size() > 3) titolo.pop_back();
            f.scrivi(t, LISTA_X + 8, y + 7, titolo, qui ? oro : bianco, 1);
            f.scrivi(t, LISTA_X + 270, y + 7, "VETRINO DELL'IA", colori::VERDE_CHIARO, 1);
            continue;
        }
        const auto& v = scenari_[static_cast<std::size_t>(i - prima_scenario())];
        std::string titolo = testo_per_font(v.titolo[0].empty() ? v.id : v.titolo[0]);
        while (f.larghezza(titolo, 1) > 250 && titolo.size() > 3) titolo.pop_back();
        f.scrivi(t, LISTA_X + 8, y + 7, titolo, qui ? oro : bianco, 1);
        const std::size_t k = static_cast<std::size_t>(i - prima_scenario());
        f.scrivi(t, LISTA_X + 270, y + 7, maiuscolo(v.teatro) + (v.alfa ? "  ALFA" : "  688") + (importate_[k] ? "  IMP." : libreria_[k] ? "  LIB." : ""),
                 grigio, 1);
    }
    int y = 64;
    const int larghezza = SCHEDA_W - 16;
    std::string testo;
    if (riga_editor_ == 0) {
        testo = "UNA MISSIONE NUOVA: IL 688 NELLE FAER OER, DA COMPORRE CON UNITA', ROTTE, ZONE, OBIETTIVI, REGOLE E BRIEFING. "
                "PER PARTIRE DA UNA RAPIDA: E DALLA SCHEDA MISSIONE RAPIDA.";
    } else if (riga_editor_ == 1) {
        testo = "DA UNA MISSIONE ORIGINALE (SINISTRA/DESTRA LA SCEGLIE): UNITA', ROTTE, DOTAZIONE, TUBI E BRIEFING DELLA "
                "VARIANTE PIU' PROBABILE. LA LOGICA DEGLI SCRIPT ORIGINALI NON SI RIPRENDE.";
        for (const auto& nota : note_originale_) testo += " " + testo_per_font(nota) + ".";
    } else if (riga_editor_ < prima_scenario()) {
        const VetrinoPronto& p = vetrini_[static_cast<std::size_t>(riga_editor_ - 2)];
        testo = "VETRINO PRONTO DELLA SANDBOX: " + maiuscolo(testo_per_font(p.titolo)) + ". " + maiuscolo(testo_per_font(p.mostra)) +
                " DURATA " + std::to_string(p.regolazioni.durata_min) + " MIN. V, G O INVIO: LE REGOLAZIONI CONSIGLIATE, POI IL "
                "VETRINO SENZA GIOCATORE.";
    } else {
        const auto& v = scenari_[static_cast<std::size_t>(riga_editor_ - prima_scenario())];
        testo = testo_per_font(v.titolo[0]) + ". TEATRO " + maiuscolo(v.teatro) + ", " + (v.alfa ? "ALFA (VMF)" : "688 (US NAVY)") +
                (v.autore.empty() ? "" : ". AUTORE " + testo_per_font(v.autore)) + ". FILE " + v.file.filename().string();
    }
    for (const auto& riga : StazioneRadio::a_capo(f, testo, larghezza)) {
        f.scrivi(t, SCHEDA_X + 8, y, riga, bianco, 1);
        y += 11;
    }
    if (!esiti_import_.empty()) {   // l'ultima importazione
        y += 6;
        f.scrivi(t, SCHEDA_X + 8, y, "IMPORTAZIONE", colori::CIANO_CHIARO, 1);
        y += 12;
        for (const auto& e : esiti_import_)
            for (const auto& riga : StazioneRadio::a_capo(f, e, larghezza)) {
                if (y > PIEDE_Y - 124) break;
                f.scrivi(t, SCHEDA_X + 8, y, riga, e.rfind("IMPORTATA", 0) == 0 ? oro : static_cast<std::uint8_t>(colori::ROSSO), 1);
                y += 11;
            }
    }
    disegna_opzioni(t, f, std::max(y + 8, PIEDE_Y - 112));
    const bool su_vetrino = riga_editor_ >= 2 && riga_editor_ < prima_scenario();
    disegna_piede(t, f, "F1 MUSEO   P CODICE   I IMPORTA   G GIOCA   R SCHEDA SUCCESSIVA", "R  SCHEDA SUCCESSIVA",
                  su_vetrino ? "INVIO  IL VETRINO" : "INVIO  APRE L'EDITOR");
}

// La scheda Missione rapida: a sinistra i parametri (i campi «a caso» col valore che il seme ha scelto), a destra
// l'operazione generata con il teatro, la data e l'ora, il briefing e se conta per la carriera
void SchermataScelta::disegna_rapida(Tela& t, const Font& f) const {
    const auto oro = static_cast<std::uint8_t>(colori::GIALLO), grigio = static_cast<std::uint8_t>(colori::GRIGIO),
               bianco = static_cast<std::uint8_t>(colori::BIANCO);
    const MissioneRapida* m = anteprima();
    const ParametriRapida& p = rapida_;
    const ParametriRapida r = m ? m->parametri : risolvi(rapida_, dati_);
    static constexpr const char* STAGIONI[] = {"A CASO", "INVERNO", "PRIMAVERA", "ESTATE", "AUTUNNO"};
    static constexpr const char* MOMENTI[] = {"A CASO", "ALBA", "GIORNO", "TRAMONTO", "NOTTE"};
    static constexpr const char* FORZE[] = {"A CASO", "POCHE", "MEDIE", "MOLTE"};
    static constexpr const char* PARANOIA[] = {"DISTRATTA", "NORMALE", "ALL'ERTA", "PARANOICA"};
    const auto a_caso = [](bool caso, const std::string& scelto, const std::string& valore) { return caso ? "A CASO (" + scelto + ")" : valore; };
    const std::string tipo_r = maiuscolo(nome_tipo_rapida(r.tipo)), tipo_p = maiuscolo(nome_tipo_rapida(p.tipo));
    char seme[16];
    std::snprintf(seme, sizeof seme, "%u", p.seme);
    const std::pair<const char*, std::string> righe[CAMPI_RAPIDA] = {
        {"TEATRO", a_caso(p.teatro.empty(), maiuscolo(r.teatro), maiuscolo(p.teatro))},
        {"STAGIONE", a_caso(p.stagione == StagioneRapida::a_caso, STAGIONI[static_cast<int>(r.stagione)], STAGIONI[static_cast<int>(p.stagione)])},
        {"MOMENTO", a_caso(p.ora == MomentoRapido::a_caso, MOMENTI[static_cast<int>(r.ora)], MOMENTI[static_cast<int>(p.ora)])},
        {"TIPO", a_caso(p.tipo == TipoRapida::a_caso, tipo_r, tipo_p)},
        {"BATTELLO", p.alfa ? "ALFA (VMF)" : "688 (US NAVY)"},
        {"FORZE", a_caso(p.forze == 0, FORZE[std::clamp(r.forze, 0, 3)], FORZE[std::clamp(p.forze, 0, 3)])},
        {"PARANOIA", PARANOIA[std::clamp(p.paranoia, 0, 3)]},
        {"DIFFICOLTA'", a_caso(p.difficolta == 0, std::to_string(r.difficolta), std::to_string(p.difficolta))},
        {"ASW AEREO", p.asw_aereo ? "SI'" : "NO"},
        {"METEO (W)", p.meteo_realistico ? "DEL TEATRO E DELLA STAGIONE" : "MARE 3, SERENO"},
        {"CARRIERA", p.carriera ? "CHIESTA" : "NO"},
        {"SEME", seme},
        {"CODICE", scrive_codice_ ? codice_ + "_" : "INVIO PER DIGITARNE UNO"},
    };
    f.scrivi(t, LISTA_X + 10, LISTA_Y - 10, "PARAMETRI DELLA MISSIONE", colori::CIANO_CHIARO, 1);
    for (int i = 0; i < CAMPI_RAPIDA; ++i) {
        const int y = LISTA_Y + i * RIGA_RAPIDA;
        const bool qui = i == campo_;
        if (qui) t.rettangolo_pieno(LISTA_X + 6, y, LISTA_W - 12, RIGA_RAPIDA - 4, colori::GRIGIO_SCURO);
        f.scrivi(t, LISTA_X + 8, y + 7, righe[i].first, qui ? oro : grigio, 1);
        std::string v = righe[i].second;
        if (qui && i != CAMPI_RAPIDA - 1) v = "< " + v + " >";
        f.scrivi(t, LISTA_X + 130, y + 7, v, qui ? oro : bianco, 1);
    }
    // la scheda dell'operazione
    int y = 64;
    const int larghezza = SCHEDA_W - 16;
    if (!m) {
        for (const auto& riga : StazioneRadio::a_capo(f, errore_.empty() ? "NESSUNA MISSIONE" : errore_, larghezza)) {
            f.scrivi(t, SCHEDA_X + 8, y, riga, colori::ROSSO, 1);
            y += 11;
        }
    } else {
        f.scrivi(t, SCHEDA_X + 8, y, testo_per_font(m->titolo_it), oro, 1);
        y += 12;
        f.scrivi(t, SCHEDA_X + 8, y, "CODICE " + m->codice, bianco, 1);
        y += 12;
        f.scrivi(t, SCHEDA_X + 8, y, r.alfa ? "VMF - ALFA (PROGETTO 705)" : "US NAVY - LOS ANGELES (SSN-688)", bianco, 1);
        y += 12;
        char data[64];
        const int ore = static_cast<int>(m->ora_utc), minuti = static_cast<int>(std::lround((m->ora_utc - ore) * 60.0)) % 60;
        std::snprintf(data, sizeof data, "%s  %02d/%02d/%d  %02d%02dZ", maiuscolo(r.teatro).c_str(), m->giorno, m->mese, m->anno, ore, minuti);
        f.scrivi(t, SCHEDA_X + 8, y, data, bianco, 1);
        y += 12;
        const int ax = SCHEDA_X + 6, ay = y, aw = SCHEDA_W - 12, ah = 80;
        if (const Fondale* fo = fondale(r.teatro)) {
            const double lx = fo->larghezza() * fo->cella_m(), ly = fo->altezza() * fo->cella_m();
            for (int py = 0; py < ah; ++py)
                for (int px = 0; px < aw; ++px) {
                    const Vec2 q{(px + 0.5) / aw * lx - lx / 2.0, ly / 2.0 - (py + 0.5) / ah * ly};
                    t.pixel(ax + px, ay + py, colore_carta(fo->profondita(q)));
                }
        }
        cornice(t, ax, ay, aw, ah, grigio);
        y += ah + 6;
        std::string briefing;   // senza l'intestazione del messaggio (ORDINI DI MISSIONE, DA, A, OGG.)
        for (const auto& b : m->briefing_it) {
            const std::string u = maiuscolo(b);
            if (u.rfind("ORDINI DI MISSIONE", 0) == 0 || u.rfind("DA:", 0) == 0 || u.rfind("A:", 0) == 0 || u.rfind("OGG", 0) == 0) continue;
            briefing += (briefing.empty() ? "" : " ") + b;
        }
        int n = 0;
        for (const auto& riga : StazioneRadio::a_capo(f, testo_per_font(briefing), larghezza)) {
            if (++n > 4) break;   // il resto nel briefing
            f.scrivi(t, SCHEDA_X + 8, y, riga, grigio, 1);
            y += 11;
        }
        y += 3;
        f.scrivi(t, SCHEDA_X + 8, y, m->conta_carriera ? "CONTA PER LA CARRIERA" : "FUORI CARRIERA", m->conta_carriera ? oro : grigio, 1);
        y += 14;
    }
    disegna_opzioni(t, f, std::max(y, PIEDE_Y - 112));
    disegna_piede(t, f, "F1 MUSEO   P CODICE   E EDITOR   C ALBO DEI COMANDANTI   R SCHEDA SUCCESSIVA", "R  SCHEDA SUCCESSIVA",
                  "INVIO  AL BRIEFING");
}

}  // namespace m688
