#include "editor_missione.hpp"

#include <algorithm>
#include <iterator>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <set>
#include <stdexcept>

#include "carta.hpp"
#include "grafica/simboli_carta.hpp"
#include "grafica/cornici.hpp"
#include "grafica/lamiera.hpp"
#include "grafica/quadro.hpp"
#include "grafica/tema_stazioni.hpp"
#include "missioni/missione.hpp"
#include "missioni/pacchetto_missione.hpp"
#include "missioni/registro_missione.hpp"
#include "nucleo/geometria.hpp"

namespace m688 {

namespace {

// la tela: testata, carta a sinistra, pannello a destra, piede con gli avvisi
constexpr int TESTATA = 20;
// gruppo 4 della passata estetica (variante C, «SALA PIANI»): la consolle u = {6, 25, 628, 445}, la carta nello schermo
// grande {u.x, u.y, 392, 424 - u.y}, la scheda nello schermo a destra; le misure sono quelle del kit (quadro::schermo)
constexpr int CARTA_X = 11, CARTA_Y = 44, CARTA_W = 382, CARTA_H = 375;
constexpr int PANNELLO_X = 407, PANNELLO_W = 222;   // l'area della scheda nello schermo di destra
constexpr int SCHEDE_Y = 44, SCHEDA_H = 12;         // le linguette su due righe
constexpr int CONTENUTO_Y = 72, CONTENUTO_FINE = 378, RIGA = 10;
constexpr int AVVISI_Y = 380, VERIFICA_Y = 405;     // due avvisi cliccabili, poi la riga della verifica con la spia
constexpr int PIEDE_Y = 428;                        // il piede comune: i quattro tasti a 450-476, larghi 150

const char* const NOMI_SCHEDE[] = {"MISS", "UNIT", "ROTT", "ZONE", "OBIE", "REGO", "BRIE", "PROV"};
const char* const TEATRI[] = {"faroe", "barents", "celtica", "gibilterra", "otranto", "kunashir", "baltico"};
const char* const NOMI_TEATRI[] = {"FAER OER", "BARENTS", "PIATTAFORMA CELTICA", "GIBILTERRA", "OTRANTO", "KUNASHIR", "BALTICO"};
const char* const SITUAZIONI[] = {"pace", "crisi", "guerra"};
const char* const COMPORTAMENTI[] = {"", "R_nave", "R_sottomarino", "R_aereo", "R_pedina"};
const char* const NOMI_COMPORTAMENTI[] = {"SOLO ROTTA", "NAVE", "SOTTOMARINO", "AEREO", "PEDINAMENTO"};
const char* const TIPI_OBIETTIVO[] = {"affondare", "proteggere", "raggiungere", "scoprire", "non_essere_scoperto", "sopravvivere", "minare",
                                      "variabile", "contatto", "foto", "zona_inviolata", "colpire", "tlam", "scatola",
                                      "identificare", "telemetro", "rotta_stimata"};   // gli ultimi tre: docs/25, I13 e I14
const char* const TAGLIE_SCATOLA[] = {"piccola", "media", "grande"};   // scatola: la ricerca piu' grande permessa (I11)
const char* const EMCON[] = {"", "silenzio", "navigazione", "ricerca", "intermittente", "ingaggio"};   // vuoto: della parte
const char* const LUCI[] = {"", "moto", "fonda", "pesca", "manovra_limitata", "non_governa", "spente"};   // vuoto: del motore (docs/27)
const char* const TRASMISSIONI[] = {"contatto", "attacco", "situazione", "conferma", "richiesta", "previsione"};   // m688.trasmesso
const char* const PARTI_ZONA[] = {"", "amica", "neutrale"};   // zona inviolata: vuoto = ostile
const char* const CATEGORIE_ESITO[] = {"", "successo", "successo_parziale", "fallimento", "richiamato"};
const char* const CONDIZIONI[] = {"TEMPO", "UNITA COLPITA", "UNITA FUORI", "GIOCATORE IN ZONA", "GIOCATORE FUORI ZONA",
                                  "UNITA IN ZONA", "ALLARME", "GIOCATORE HA LANCIATO", "VARIABILE", "REGOLA SCATTATA",
                                  "GIOCATORE SOTTO I NODI", "GIOCATORE A QUOTA", "GIOCATORE IN SUPERFICIE", "ALBERI ALZATI",
                                  "OBIETTIVO COMPIUTO", "CONTATTO SU", "SENZA CONTATTO SU", "PING VICINO A", "OSTILE HA LANCIATO",
                                  "UNITA VEDE", "UNITA NON VEDE", "VICINO AL CAVO", "UNITA IN SUPERFICIE", "GIOCATORE SOPRA I NODI",
                                  "UNITA IN AVARIA", "UNITA A RIMORCHIO", "GIOCATORE HA TRASMESSO", "EVENTO", "UNITA CLASSIFICA",
                                  "GIOCATORE VICINO A"};
constexpr int N_CONDIZIONI = static_cast<int>(std::size(CONDIZIONI));
const char* const AZIONI[] = {"MESSAGGIO", "FINE MISSIONE", "VARIABILE", "COMPORTAMENTO", "VAI A", "ENTRA IN SCENA",
                              "CUE ASW", "ALLARME", "SPOSTA ZONA", "RADIO", "SITUAZIONE", "POSA MINE", "AVARIA", "RIMORCHIA",
                              "MOLLA IL RIMORCHIO", "ESERCITAZIONE", "ATTACCA UNITA'", "LANCIA MISSILE", "QUOTA", "RUMORE", "COMANDA MINE", "TOGLI UNITA'", "SALVATI"};
const char* const TIPI_MINA[] = {"mk67", "mdt", "mk60", "pmr1", "pmr2"};
const char* const STATI_UNITA[] = {"", "avaria", "incagliata", "relitto"};   // M6 e I10: lo stato iniziale delle unita' dell'IA
const char* const ANTENNE_SONAR[] = {"", "lineare", "scafo"};                // sonar speciale: vuoto = sfera
const char* const ZONE_RADIO[] = {"", "azione", "libera_asw", "protetta"};   // zone dichiarate dalla sala radio (R2)
constexpr int N_AZIONI = static_cast<int>(std::size(AZIONI));
const char* const SITUAZIONI_IA[] = {"pace", "crisi", "guerra"};
// radio (R3 ed E6): precedenze, classifiche, eventi per `se`, categorie e stati delle ROE
const char* const PRECEDENZE[] = {"routine", "priority", "immediate", "flash"};
const char* const CLASSIFICHE[] = {"UNCLASSIFIED", "CONFIDENTIAL", "SECRET", "TOP SECRET"};
const char* const EVENTI_RADIO[] = {"", "lancio_nemico", "avvistato", "scoperta", "affondamento"};
const char* const CATEGORIE_RADIO[] = {"sottomarini", "navi", "mercantili", "neutrali", "aerei"};
const char* const STATI_ROE[] = {"", "libero", "stretto", "trattenuto"};
const char* const NEBBIE[] = {"", "nessuna", "avvezione", "fumo_di_mare", "radiazione", "precipitazione"};

// categorie e parti delle classi che l'editor offre (quelle coi modelli del classico)
const std::set<std::string> SOTTOMARINI = {"los_angeles", "sturgeon", "trafalgar", "victor3", "akula", "alfa", "oscar",
                                           "charlie2", "typhoon", "kilo", "foxtrot"};
const std::set<std::string> AEREI = {"sh60b", "sh3h", "ka25", "ka27", "p3c", "s3b", "il38", "tu142", "be12"};
const std::set<std::string> USA_CLASSI = {"perry", "kidd", "spruance", "iowa", "nimitz", "sacramento", "los_angeles",
                                          "sturgeon", "trafalgar", "sh60b", "sh3h", "p3c", "s3b"};
const std::set<std::string> CIVILI = {"mercantile", "cargo", "petroliera", "peschereccio"};
const std::set<std::string> AUSILIARIE = {"sacramento", "agi"};   // non combattenti con la bandiera di una marina

// le esche semoventi dichiarate nel deposito (la chiave della famiglia)
int esche_mobili(const CaricoScenario& c) {
    for (const auto& v : c.deposito)
        if (v.arma == "esca_mobile") return v.numero;
    return 0;
}
void imposta_esche_mobili(CaricoScenario& c, int n) {
    std::erase_if(c.deposito, [](const VoceCarico& v) { return v.arma == "esca_mobile"; });
    if (n > 0) c.deposito.push_back({"esca_mobile", n});
}

std::string categoria(const std::string& classe) {
    return SOTTOMARINI.count(classe) ? "sottomarino" : AEREI.count(classe) ? "aereo" : "nave";
}

// Maiuscolo per il font del gioco: le vocali accentate italiane con l'apostrofo, gli altri segni fuori dall'ASCII come «?»
std::string maiu(const std::string& s) {
    std::string o;
    for (std::size_t i = 0; i < s.size(); ++i) {
        const unsigned char c = static_cast<unsigned char>(s[i]);
        if (c < 0x80) {
            o += static_cast<char>(c >= 'a' && c <= 'z' ? c - 'a' + 'A' : c);
            continue;
        }
        if (c == 0xC3 && i + 1 < s.size()) {
            const unsigned char d = static_cast<unsigned char>(s[++i]);
            const char* m = "?";
            if (d == 0xA0 || d == 0x80) m = "A'";
            else if (d == 0xA8 || d == 0xA9 || d == 0x88 || d == 0x89) m = "E'";
            else if (d == 0xAC || d == 0x8C) m = "I'";
            else if (d == 0xB2 || d == 0x92) m = "O'";
            else if (d == 0xB9 || d == 0x99) m = "U'";
            else if (d == 0xB8 || d == 0x98) m = "O";   // ø
            else if (d == 0xA6 || d == 0x86) m = "AE";
            o += m;
            continue;
        }
        if (c == 0xC2 && i + 1 < s.size() && static_cast<unsigned char>(s[i + 1]) == 0xB0) {   // °
            ++i;
            o += 'O';
            continue;
        }
        while (i + 1 < s.size() && (static_cast<unsigned char>(s[i + 1]) & 0xC0) == 0x80) ++i;   // il resto del carattere
        o += '?';
    }
    return o;
}

// il testo tagliato alla larghezza (scala 1)
std::string taglia(const Font& f, std::string s, int w) {
    while (!s.empty() && f.larghezza(s, 1) > w) s.pop_back();
    return s;
}

void linea(Tela& t, Vec2 a, Vec2 b, std::uint8_t c, int tratto = 0) {
    const int n = static_cast<int>(std::max(std::fabs(b.x - a.x), std::fabs(b.y - a.y)));
    for (int i = 0; i <= n; ++i) {
        if (tratto > 0 && (i / tratto) % 2) continue;
        const double s = n ? static_cast<double>(i) / n : 0.0;
        t.pixel(static_cast<int>(std::lround(a.x + (b.x - a.x) * s)), static_cast<int>(std::lround(a.y + (b.y - a.y) * s)), c);
    }
}

void arco(Tela& t, Vec2 c, double r, double da, double a, std::uint8_t col) {   // gradi matematici, 0 = est
    const int n = std::max(12, static_cast<int>(r * 3.0));
    for (int i = 0; i <= n; ++i) {
        const double g = radianti(da + (a - da) * i / n);
        t.pixel(static_cast<int>(std::lround(c.x + r * std::cos(g))), static_cast<int>(std::lround(c.y - r * std::sin(g))), col);
    }
}

template <std::size_t N>
int indice(const char* const (&v)[N], const std::string& s) {
    for (std::size_t i = 0; i < N; ++i)
        if (s == v[i]) return static_cast<int>(i);
    return 0;
}
template <std::size_t N>
std::string ciclo(const char* const (&v)[N], const std::string& s, int dir) {
    return v[(indice(v, s) + dir + static_cast<int>(N)) % static_cast<int>(N)];
}
int ciclo_intero(int x, int dir, int da, int a) { return x + dir < da ? a : x + dir > a ? da : x + dir; }

std::string si_no(bool b) { return b ? "SI" : "NO"; }
std::string ore(double s) {
    char b[16];
    std::snprintf(b, sizeof b, "%d:%02d", static_cast<int>(s / 3600.0), static_cast<int>(std::fmod(s, 3600.0) / 60.0));
    return b;
}

}  // namespace

// --- costruzione -------------------------------------------------------------------------------------------------------

SchermataEditor::SchermataEditor(const Risorse& r, std::string dati, std::optional<ScenarioMissione> iniziale)
    : r_(r), dati_(std::move(dati)), d_(carica_dati_scenario(dati_)), cartella_(cartella_missioni_editor()),
      esportate_(cartella_missioni_esportate()) {
    try {
        targa_ = Font::carica(dati_ + "/font/incisione688_7.fnt");
        fosforo_ = Font::carica(dati_ + "/font/fosforo688_5x7.fnt");
    } catch (const std::exception&) {
        targa_.reset();
        fosforo_.reset();
    }
    if (iniziale) {
        s_ = std::move(*iniziale);
        s_.rapida.reset();   // una rapida aperta nell'editor diventa una missione dell'editor
        s_.libreria = 2;
        if (s_.id.rfind("EDITOR_", 0) != 0) s_.id = "EDITOR_" + s_.id;
        carica_teatro();
    } else {
        s_.id = "EDITOR_NUOVA";
        s_.titolo = {"Nuova missione", "New mission"};
        s_.teatro = "faroe";
        s_.giorno = 15;
        s_.mese = 10;
        s_.ora_utc = 12.0;
        s_.esito_fine = d_.esito_fine;
        s_.esito_tempo = d_.esito_tempo;
        carica_teatro();
        UnitaScenario g;
        g.id = 0;
        g.classe = "los_angeles";
        g.categoria = "sottomarino";
        g.parte = "usa";
        g.flag = 0x09;
        g.giocatore = true;
        g.velocita = 5.0;
        g.quota = 45.0;
        // il punto piu' profondo vicino al centro del teatro
        double meglio = -1.0;
        for (double y = -60000.0; y <= 60000.0; y += 4000.0)
            for (double x = -60000.0; x <= 60000.0; x += 4000.0)
                if (const double z = fondale_->profondita({x, y}); z > 200.0 && z < 1e9 && (meglio < 0 || std::hypot(x, y) < meglio)) {
                    meglio = std::hypot(x, y);
                    g.pos = {x, y};
                }
        s_.unita.push_back(g);
        ObiettivoScenario vivo;
        vivo.id = "vivo";
        vivo.tipo = "sopravvivere";
        vivo.testo = {"Riportare a casa il battello", "Bring the boat home"};
        s_.obiettivi.push_back(vivo);
        imposta_carico();
        briefing_automatico();
    }
    inquadra();
    cambiato();
    modificato_ = false;
}

void SchermataEditor::carica_teatro() {
    if (teatro_caricato_ == s_.teatro && fondale_) return;
    fondale_ = std::make_unique<Fondale>(Fondale::carica(dati_ + "/teatri/" + s_.teatro + ".fdl"));
    teatro_caricato_ = s_.teatro;
    if (fondale_->georeferenziato()) {
        s_.centro_lat = fondale_->centro().lat;
        s_.centro_lon = fondale_->centro().lon;
    }
    vga_.invalida();
}

// --- carta ---------------------------------------------------------------------------------------------------------------

GeometriaCarta SchermataEditor::geometria() const {
    GeometriaCarta g;
    g.x = CARTA_X;
    g.y = CARTA_Y;
    g.w = CARTA_W;
    g.h = CARTA_H;
    g.centro = centro_;
    g.metri_per_pixel = mpp_;
    return g;
}

void SchermataEditor::centra(Vec2 p) {
    centro_ = p;
    limita_vista();
}

void SchermataEditor::limita_vista() {
    if (!fondale_) return;
    const double w = fondale_->larghezza() * fondale_->cella_m(), h = fondale_->altezza() * fondale_->cella_m();
    mpp_ = std::clamp(mpp_, 25.0, std::min(w / CARTA_W, h / CARTA_H));   // la carta riempie sempre il riquadro
    const double mx = std::max(0.0, w / 2.0 - CARTA_W / 2.0 * mpp_), my = std::max(0.0, h / 2.0 - CARTA_H / 2.0 * mpp_);
    centro_.x = std::clamp(centro_.x, -mx, mx);
    centro_.y = std::clamp(centro_.y, -my, my);
}

void SchermataEditor::inquadra() {
    double x0 = 1e18, y0 = 1e18, x1 = -1e18, y1 = -1e18;
    const auto punto = [&](Vec2 p, double r = 0.0) {
        x0 = std::min(x0, p.x - r); x1 = std::max(x1, p.x + r);
        y0 = std::min(y0, p.y - r); y1 = std::max(y1, p.y + r);
    };
    for (const auto& u : s_.unita) {
        if (u.categoria == "aereo") continue;   // gli aerei vengono da lontano: contano le loro rotte
        punto(u.pos);
        for (const auto& q : u.punti) punto(q);
    }
    for (const auto& z : s_.zone) punto({z.cerchio.x, z.cerchio.y}, z.cerchio.r);
    for (const auto& o : s_.obiettivi)
        if (o.cerchio) punto({o.cerchio->x, o.cerchio->y}, o.cerchio->r);
    if (x0 > x1) {   // niente da inquadrare: il teatro intero
        centro_ = {};
        mpp_ = 1e9;
        limita_vista();
        return;
    }
    const double w = std::max(150000.0, (x1 - x0) * 1.2), h = std::max(150000.0, (y1 - y0) * 1.2);   // almeno 150 km: la costa vicina
    mpp_ = std::max(w / CARTA_W, h / CARTA_H);
    centro_ = {(x0 + x1) / 2.0, (y0 + y1) / 2.0};
    limita_vista();
}

int SchermataEditor::unita_in(Vec2 q) const {
    const GeometriaCarta g = geometria();
    int meglio = -1;
    double d = 7.0;
    for (std::size_t i = 0; i < s_.unita.size(); ++i) {
        const Vec2 p = g.a_tela(s_.unita[i].pos);
        if (const double x = std::hypot(p.x - q.x, p.y - q.y); x < d) { d = x; meglio = static_cast<int>(i); }
    }
    return meglio;
}

int SchermataEditor::zona_in(Vec2 q) const {
    const GeometriaCarta g = geometria();
    for (std::size_t i = 0; i < s_.zone.size(); ++i) {
        const Vec2 p = g.a_tela({s_.zone[i].cerchio.x, s_.zone[i].cerchio.y});
        if (std::hypot(p.x - q.x, p.y - q.y) < 7.0) return static_cast<int>(i);
    }
    return -1;
}

void SchermataEditor::clic_carta(Vec2 q, bool destro) {
    const Vec2 p = geometria().a_mondo(q);
    if (!destro) return;
    switch (scheda_) {
        case Scheda::unita:
            unita_ = nuova_unita(p);
            break;
        case Scheda::rotte:
            if (unita_ >= 0 && unita_ < static_cast<int>(s_.unita.size())) s_.unita[static_cast<std::size_t>(unita_)].punti.push_back(p);
            break;
        case Scheda::zone: {
            int n = static_cast<int>(s_.zone.size()) + 1;
            std::string id;
            do id = "Z" + std::to_string(n++);
            while (std::any_of(s_.zone.begin(), s_.zone.end(), [&](const ZonaScenario& z) { return z.id == id; }));
            s_.zone.push_back({id, {"Zona " + id.substr(1), "Zone " + id.substr(1)}, {p.x, p.y, 3000.0}});
            zona_ = static_cast<int>(s_.zone.size()) - 1;
            break;
        }
        case Scheda::obiettivi:
            if (obiettivo_ >= 0 && obiettivo_ < static_cast<int>(s_.obiettivi.size())) {
                auto& o = s_.obiettivi[static_cast<std::size_t>(obiettivo_)];
                o.cerchio = CerchioScenario{p.x, p.y, o.cerchio ? o.cerchio->r : 3000.0};
            }
            break;
        case Scheda::regole:
            if (regola_ >= 0 && regola_ < static_cast<int>(s_.regole.size()) && azione_ >= 0 &&
                azione_ < static_cast<int>(s_.regole[static_cast<std::size_t>(regola_)].allora.size()))
                s_.regole[static_cast<std::size_t>(regola_)].allora[static_cast<std::size_t>(azione_)].punto = p;
            break;
        default: return;
    }
    cambiato();
}

void SchermataEditor::disegna_simbolo(Tela& t, Vec2 q, const UnitaScenario& u, bool scelta) const {
    // i simboli APP-6 della carta (grafica/simboli_carta del kit), come nel replay e nella vista dal vivo
    namespace sc = simboli;
    const std::string mia = s_.alfa ? "urss" : "usa";
    const sc::Parte p = u.parte == mia ? sc::Parte::amico : u.parte == "neutrale" ? sc::Parte::neutrale : sc::Parte::ostile;
    const auto c = sc::indice_parte(p);
    if (u.giocatore) sc::disegna_proprio(t, q);
    else sc::disegna_simbolo_unita(t, q, p, sc::dominio_da_categoria(u.categoria, u.classe), c);
    if (!u.entra_con.empty()) t.pixel(static_cast<int>(q.x), static_cast<int>(q.y) + 2, c);   // di riserva: un punto sotto
    if (scelta) sc::disegna_scelta(t, q);
}

void SchermataEditor::disegna_carta(Tela& t, const Font& f) const {
    const GeometriaCarta g = geometria();
    // le etichette non si sovrappongono: scendono di una riga finche' trovano posto, altrimenti si tacciono
    std::vector<Rettangolo> occupate;
    const auto etichetta = [&](int x, int y, const std::string& testo, std::uint8_t col) {
        const int w = f.larghezza(testo, 1) + 2, h = 9;
        for (int prova = 0; prova < 4; ++prova, y += h) {
            const bool libera = std::none_of(occupate.begin(), occupate.end(), [&](const Rettangolo& r) {
                return x < r.x + r.w && r.x < x + w && y < r.y + r.h && r.y < y + h;
            });
            if (libera) {
                occupate.push_back({x, y, w, h});
                testo_con_alone(t, x, y, testo, f, col, colori::NERO);
                return;
            }
        }
    };
    OpzioniCarta o;
    o.stile = StileCarta::vga;
    vga_.disegna(t, g, *fondale_, o);
    t.imposta_ritaglio({CARTA_X, CARTA_Y, CARTA_W, CARTA_H});
    // zone: il cerchio e il quadrato che controllano le regole
    for (std::size_t i = 0; i < s_.zone.size(); ++i) {
        const auto& z = s_.zone[i];
        const Vec2 c = g.a_tela({z.cerchio.x, z.cerchio.y});
        const double r = z.cerchio.r / mpp_;
        const auto col = static_cast<std::uint8_t>(static_cast<int>(i) == zona_ ? colori_carta::EVIDENZA : colori_carta::ZONA + 1);
        arco(t, c, r, 0.0, 360.0, col);
        linea(t, {c.x - r, c.y - r}, {c.x + r, c.y - r}, col, 3);
        linea(t, {c.x + r, c.y - r}, {c.x + r, c.y + r}, col, 3);
        linea(t, {c.x + r, c.y + r}, {c.x - r, c.y + r}, col, 3);
        linea(t, {c.x - r, c.y + r}, {c.x - r, c.y - r}, col, 3);
        etichetta(static_cast<int>(c.x) + 3, static_cast<int>(c.y) - 3, maiu(z.id), col);
    }
    // cerchi degli obiettivi
    for (std::size_t i = 0; i < s_.obiettivi.size(); ++i) {
        const auto& ob = s_.obiettivi[i];
        if (!ob.cerchio) continue;
        const Vec2 c = g.a_tela({ob.cerchio->x, ob.cerchio->y});
        const auto col = static_cast<std::uint8_t>(static_cast<int>(i) == obiettivo_ ? colori_carta::EVIDENZA : colori_carta::MAGENTA);
        arco(t, c, ob.cerchio->r / mpp_, 0.0, 360.0, col);
        etichetta(static_cast<int>(c.x) + 3, static_cast<int>(c.y) + 3, maiu(ob.id), col);
    }
    // rotte
    for (std::size_t i = 0; i < s_.unita.size(); ++i) {
        const auto& u = s_.unita[i];
        if (u.punti.empty()) continue;
        const bool scelta = static_cast<int>(i) == unita_;
        const auto col = static_cast<std::uint8_t>(scelta ? colori_carta::EVIDENZA : colori_carta::MATITA + 5);
        Vec2 a = g.a_tela(u.pos);
        for (std::size_t k = 0; k < u.punti.size(); ++k) {
            const Vec2 b = g.a_tela(u.punti[k]);
            linea(t, a, b, col, scelta ? 0 : 2);
            t.rettangolo_pieno(static_cast<int>(b.x) - 1, static_cast<int>(b.y) - 1, 3, 3, col);
            if (scelta) etichetta(static_cast<int>(b.x) + 3, static_cast<int>(b.y) - 3, std::to_string(k + 1), col);
            a = b;
        }
        if (u.ciclica) linea(t, a, g.a_tela(u.punti.front()), col, 4);
    }
    // unita'
    for (std::size_t i = 0; i < s_.unita.size(); ++i) disegna_simbolo(t, g.a_tela(s_.unita[i].pos), s_.unita[i], static_cast<int>(i) == unita_);
    // scala e coordinate del puntatore
    t.togli_ritaglio();
    char b[64];
    std::snprintf(b, sizeof b, "SCALA %.0f M/PX", mpp_);
    testo_con_alone(t, CARTA_X + 4, CARTA_Y + CARTA_H - 10, b, f, colori::BIANCO, colori::NERO);
}

// --- pannello ------------------------------------------------------------------------------------------------------------

// Una riga del pannello; il testo troppo lungo va a capo fra le parole (le righe seguenti rientrano di due spazi)
void SchermataEditor::riga(int& y, const std::string& testo, int colore, std::function<void(int)> azione, bool acceso) const {
    const int w = PANNELLO_W - 10;
    const Font& g = font_fosforo();
    const auto largo = [&](const std::string& x) { return g.larghezza(x, 1, g.formato() == 2 ? g.spazio_predefinito() : 1, g.formato() != 2); };
    std::string resto = testo, rientro;
    do {
        std::string pezzo = rientro + resto;
        if (largo(pezzo) > w) {
            std::size_t taglio = pezzo.size();
            while (taglio > rientro.size() && largo(pezzo.substr(0, taglio)) > w) --taglio;
            const auto spazio = pezzo.rfind(' ', taglio);
            if (spazio != std::string::npos && spazio > rientro.size() + 4) taglio = spazio;
            resto = pezzo.substr(taglio);
            while (!resto.empty() && resto.front() == ' ') resto.erase(0, 1);
            pezzo = pezzo.substr(0, taglio);
        } else {
            resto.clear();
        }
        elementi_.push_back({{PANNELLO_X, y, PANNELLO_W, RIGA}, pezzo, colore, azione, acceso});
        y += RIGA;
        rientro = "  ";
    } while (!resto.empty());
}

void SchermataEditor::voce(int& y, const std::string& nome, const std::string& valore, std::function<void(int)> azione) const {
    riga(y, nome + ": " + valore, azione ? colori::BIANCO : colori::GRIGIO, std::move(azione));
}

void SchermataEditor::scrivi_testo(std::string* destinazione, const std::string& etichetta) {
    campo_ = Campo{destinazione, *destinazione, etichetta};
}

void SchermataEditor::costruisci() const {
    if (elementi_validi_) return;
    elementi_.clear();
    switch (scheda_) {
        case Scheda::missione: pannello_missione(); break;
        case Scheda::unita: pannello_unita(); break;
        case Scheda::rotte: pannello_rotte(); break;
        case Scheda::zone: pannello_zone(); break;
        case Scheda::obiettivi: pannello_obiettivi(); break;
        case Scheda::regole: pannello_regole(); break;
        case Scheda::briefing: pannello_briefing(); break;
        case Scheda::prova: pannello_prova(); break;
    }
    elementi_validi_ = true;
}

// Gli elementi cambiano lo scenario attraverso un puntatore non costante: il pannello si costruisce in disegna (const),
// le azioni si eseguono in comandi
#define QUI const_cast<SchermataEditor*>(this)

void SchermataEditor::pannello_missione() const {
    int y = CONTENUTO_Y;
    auto* e = QUI;
    riga(y, "LA MISSIONE", colori::CIANO_CHIARO);
    voce(y, "TITOLO", maiu(s_.titolo[0]), [e](int) { e->scrivi_testo(&e->s_.titolo[0], "TITOLO"); });
    voce(y, "TITOLO EN", maiu(s_.titolo[1]), [e](int) { e->scrivi_testo(&e->s_.titolo[1], "TITOLO INGLESE"); });
    voce(y, "AUTORE", maiu(s_.autore), [e](int) { e->scrivi_testo(&e->s_.autore, "AUTORE"); });
    voce(y, "ID", s_.id, {});
    y += 3;
    riga(y, "LUOGO E TEMPO", colori::CIANO_CHIARO);
    voce(y, "TEATRO", NOMI_TEATRI[indice(TEATRI, s_.teatro)], [e](int d) {
        e->s_.teatro = ciclo(TEATRI, e->s_.teatro, d);
        e->carica_teatro();
        e->inquadra();
        e->cambiato();
    });
    voce(y, "PARTE", s_.alfa ? "ALFA (URSS)" : "688 (USA)", [e](int) {
        e->s_.alfa = !e->s_.alfa;
        auto& g = e->s_.unita[0];
        g.classe = e->s_.alfa ? "alfa" : "los_angeles";
        g.parte = e->s_.alfa ? "urss" : "usa";
        g.flag = e->s_.alfa ? 0x11 : 0x09;
        e->imposta_carico();
        e->cambiato();
    });
    // la data in una riga sola; il valore si cambia per parti (clic e destro sulla riga), la riga sotto sceglie la parte
    static const char* const MESI[] = {"GEN", "FEB", "MAR", "APR", "MAG", "GIU", "LUG", "AGO", "SET", "OTT", "NOV", "DIC"};
    static const char* const PARTI_DATA[] = {"GIORNO", "MESE", "ANNO"};
    char data[48];
    std::snprintf(data, sizeof data, "%02d %s %04d", s_.giorno, MESI[std::clamp(s_.mese, 1, 12) - 1], s_.anno);
    voce(y, "DATA", data, [e](int d) {
        if (e->parte_data_ == 0) e->s_.giorno = ciclo_intero(e->s_.giorno, d, 1, 28);
        else if (e->parte_data_ == 1) e->s_.mese = ciclo_intero(e->s_.mese, d, 1, 12);
        else e->s_.anno = ciclo_intero(e->s_.anno, d, 1985, 1991);
        e->cambiato();
    });
    voce(y, "  SI CAMBIA", PARTI_DATA[std::clamp(parte_data_, 0, 2)], [e](int d) {
        e->parte_data_ = ((e->parte_data_ + d) % 3 + 3) % 3;
        e->elementi_validi_ = false;
    });
    voce(y, "ORA UTC", ore(s_.ora_utc * 3600.0), [e](int d) { e->s_.ora_utc = std::fmod(e->s_.ora_utc + 0.5 * d + 24.0, 24.0); e->cambiato(); });
    voce(y, "METEO", s_.meteo_realistico ? "REALISTICO" : "SEMPLIFICATO", [e](int) { e->s_.meteo_realistico = !e->s_.meteo_realistico; e->cambiato(); });
    // meteo fissato (E6): fermo per tutta la missione, sopra il meteo del teatro
    voce(y, "METEO FISSO", si_no(s_.meteo_fisso.has_value()), [e](int) {
        if (e->s_.meteo_fisso) e->s_.meteo_fisso.reset();
        else e->s_.meteo_fisso = MeteoFissoScenario{std::nullopt, std::nullopt, 3.0, 20000.0, std::nullopt, std::nullopt, std::nullopt, ""};
        e->cambiato();
    });
    if (s_.meteo_fisso) {
        const auto& m = *s_.meteo_fisso;
        const auto testo = [](const std::optional<double>& v, const char* unita, double scala) {
            return v ? std::to_string(static_cast<int>(*v / scala)) + unita : std::string("DEL TEATRO");
        };
        voce(y, "  MARE", testo(m.stato_mare, "", 1.0), [e](int d) {
            auto& v = e->s_.meteo_fisso->stato_mare;
            const double x = v.value_or(-1.0) + d;
            if (x < 0.0 || x > 9.0) v.reset();
            else v = x;
            e->cambiato();
        });
        voce(y, "  VENTO", testo(m.vento_m_s, " M/S", 1.0), [e](int d) {
            auto& v = e->s_.meteo_fisso->vento_m_s;
            const double x = v.value_or(-2.0) + 2.0 * d;
            if (x < 0.0 || x > 30.0) v.reset();
            else v = x;
            e->cambiato();
        });
        voce(y, "  VISIBILITA'", testo(m.visibilita_m, " KM", 1000.0), [e](int d) {
            static const double PASSI[] = {500.0, 1000.0, 2000.0, 5000.0, 10000.0, 20000.0, 40000.0};
            auto& v = e->s_.meteo_fisso->visibilita_m;
            int i = -1;
            for (int k = 0; k < 7; ++k)
                if (v && *v == PASSI[k]) i = k;
            i += d;
            if (i < 0 || i > 6) v.reset();
            else v = PASSI[i];
            e->cambiato();
        });
        voce(y, "  NEBBIA", m.nebbia.empty() ? "DEL TEATRO" : maiu(m.nebbia), [e](int d) {
            auto& v = e->s_.meteo_fisso->nebbia;
            v = ciclo(NEBBIE, v, d);
            e->cambiato();
        });
    }
    voce(y, "SITUAZIONE", maiu(s_.situazione), [e](int d) { e->s_.situazione = ciclo(SITUAZIONI, e->s_.situazione, d); e->cambiato(); });
    // M7: con unita' del luogo, la situazione di partenza fra il luogo e le altre due parti (senza: in pace con tutti)
    if (s_.situazione_luogo || std::any_of(s_.unita.begin(), s_.unita.end(), [](const UnitaScenario& u) { return u.parte == "luogo"; })) {
        const SituazioneLuogo sl = s_.situazione_luogo.value_or(SituazioneLuogo{});
        voce(y, "LUOGO CON USA", maiu(sl.usa), [e](int d) {
            auto& v = e->s_.situazione_luogo;
            if (!v) v = SituazioneLuogo{};
            v->usa = ciclo(SITUAZIONI, v->usa, d);
            e->cambiato();
        });
        voce(y, "LUOGO CON URSS", maiu(sl.urss), [e](int d) {
            auto& v = e->s_.situazione_luogo;
            if (!v) v = SituazioneLuogo{};
            v->urss = ciclo(SITUAZIONI, v->urss, d);
            e->cambiato();
        });
    }
    y += 3;
    riga(y, "REGOLE", colori::CIANO_CHIARO);
    // danni iniziali (Enhanced, scheda I07): nessuno, lievi (propulsione al 70%, schiera persa), gravi (un'esplosione
    // vicina, propulsione al 40%, acqua e fuoco in sala macchine); le voci fini si scrivono nel file
    const auto livello_danni = [](const std::optional<DanniInizialiScenario>& d) {
        return !d ? 0 : d->urto ? 2 : 1;
    };
    static const char* const DANNI[] = {"NESSUNO", "LIEVI", "GRAVI"};
    voce(y, "DANNI INIZIALI", DANNI[livello_danni(s_.danni_iniziali)], [e, livello_danni](int d) {
        const int n = (livello_danni(e->s_.danni_iniziali) + (d < 0 ? 2 : 1)) % 3;
        auto& x = e->s_.danni_iniziali;
        if (n == 0) x.reset();
        else if (n == 1) x = DanniInizialiScenario{std::nullopt, {{"propulsione", 0.7}, {"schiera", 0.0}}, {}, {}, {}};
        else x = DanniInizialiScenario{UrtoIniziale{150.0, 60.0, 90.0}, {{"propulsione", 0.4}}, {{"macchine", 10.0}}, {}, {{"macchine", 0.3}}};
        e->cambiato();
    });
    // armi da esercitazione (I03, S01): tutte inerti, il colpo si conta (obiettivo «colpire») senza danni
    voce(y, "ARMI", s_.esercitazione ? "DA ESERCITAZIONE" : "VERE", [e](int) { e->s_.esercitazione = !e->s_.esercitazione; e->cambiato(); });
    voce(y, "PARANOIA", std::to_string(s_.paranoia), [e](int d) { e->s_.paranoia = ciclo_intero(e->s_.paranoia, d, 0, 3); e->cambiato(); });
    voce(y, "DIFFICOLTA'", std::to_string(s_.difficolta), [e](int d) { e->s_.difficolta = ciclo_intero(e->s_.difficolta, d, 1, 5); e->cambiato(); });
    voce(y, "DURATA", ore(s_.durata_s), [e](int d) { e->s_.durata_s = std::clamp(e->s_.durata_s + 1800.0 * d, 1800.0, 12.0 * 3600.0); e->cambiato(); });
    static const char* const TIPI[] = {"antisom", "antinave", "minamento", "scorta"};
    voce(y, "CARICO", maiu(s_.carico.tipo_missione), [e](int d) {
        e->s_.carico.tipo_missione = ciclo(TIPI, e->s_.carico.tipo_missione, d);
        e->imposta_carico();
        e->cambiato();
    });
    // le esche semoventi (famiglia esca_mobile di carico.toml): la MG-74 dell'Alfa; la MOSS Mk 70 del 688 e' what-if e
    // c'e' solo nel modo moderno. Si dichiarano nel deposito: 0, 2 o 4
    voce(y, s_.alfa ? "ESCHE MG-74" : "ESCHE MOSS (WHAT-IF)", std::to_string(esche_mobili(s_.carico)), [e](int d) {
        const int n = esche_mobili(e->s_.carico);
        imposta_esche_mobili(e->s_.carico, d > 0 ? (n >= 4 ? 0 : n + 2) : (n <= 0 ? 4 : n - 2));
        e->cambiato();
    });
    voce(y, "OSPITI", std::to_string(s_.carico.ospiti), [e](int) { e->s_.carico.ospiti = e->s_.carico.ospiti ? 0 : 6; e->cambiato(); });
    voce(y, "CARRIERA", si_no(s_.carriera.has_value()), [e](int) {
        if (e->s_.carriera) e->s_.carriera.reset();
        else {
            static const char* const AVV[] = {"impreparato", "regolare", "veterano", "elite"};
            e->s_.carriera = CarrieraScenario{e->s_.situazione, AVV[std::clamp(e->s_.paranoia, 0, 3)], e->s_.difficolta, false,
                                              e->s_.teatro == "barents"};
        }
        e->cambiato();
    });
    static const char* const CHIUSURE[] = {"SOLO TEMPO", "BERSAGLI FINITI"};
    const bool bersagli = s_.chiusura.tipo == ChiusuraScenario::Tipo::unita_finite;
    voce(y, "FINE", CHIUSURE[bersagli ? 1 : 0], [e](int) {
        if (e->s_.chiusura.tipo == ChiusuraScenario::Tipo::unita_finite) e->s_.chiusura = {};
        else {
            e->s_.chiusura = {};
            e->s_.chiusura.tipo = ChiusuraScenario::Tipo::unita_finite;
            for (const auto& o : e->s_.obiettivi)
                if (o.tipo == "affondare") e->s_.chiusura.unita.insert(e->s_.chiusura.unita.end(), o.unita.begin(), o.unita.end());
            if (e->s_.chiusura.unita.empty()) e->s_.chiusura = {};
        }
        e->cambiato();
    });
    y += 4;
    riga(y, "BERSAGLI FINITI: LE UNITA' DEGLI", colori::GRIGIO);
    riga(y, "OBIETTIVI AFFONDARE. LE ALTRE FINI", colori::GRIGIO);
    riga(y, "CON LE REGOLE (FINE MISSIONE).", colori::GRIGIO);
}

void SchermataEditor::pannello_unita() const {
    int y = CONTENUTO_Y;
    auto* e = QUI;
    riga(y, "NUOVA UNITA' (CLIC DESTRO SULLA CARTA)", colori::CIANO_CHIARO);
    static const char* const PARTI[] = {"usa", "urss", "neutrale", "luogo"};
    voce(y, "PARTE", maiu(parte_nuova_), [e](int d) { e->parte_nuova_ = ciclo(PARTI, e->parte_nuova_, d); e->elementi_validi_ = false; });
    voce(y, "CLASSE", maiu(classe_nuova_), [e](int d) {
        std::vector<std::string> v;
        for (const auto& [k, m] : e->d_.modelli)
            if (k.rfind("giocatore_", 0) != 0) v.push_back(k);
        auto it = std::find(v.begin(), v.end(), e->classe_nuova_);
        int i = it == v.end() ? 0 : static_cast<int>(it - v.begin());
        i = (i + d + static_cast<int>(v.size())) % static_cast<int>(v.size());
        e->classe_nuova_ = v[static_cast<std::size_t>(i)];
        e->parte_nuova_ = CIVILI.count(e->classe_nuova_) ? "neutrale" : USA_CLASSI.count(e->classe_nuova_) ? "usa" : "urss";
        e->elementi_validi_ = false;
    });
    y += 4;
    if (unita_ < 0 || unita_ >= static_cast<int>(s_.unita.size())) {
        riga(y, "NESSUNA UNITA' SCELTA", colori::GRIGIO);
        return;
    }
    const auto& u = s_.unita[static_cast<std::size_t>(unita_)];
    const auto i = static_cast<std::size_t>(unita_);
    riga(y, "UNITA' " + std::to_string(u.id) + (u.giocatore ? " (GIOCATORE)" : ""), colori::CIANO_CHIARO);
    voce(y, "CLASSE", maiu(u.classe), {});
    voce(y, "PARTE", maiu(u.parte), u.giocatore ? std::function<void(int)>{} : std::function<void(int)>([e, i](int d) {
        static const char* const P[] = {"usa", "urss", "neutrale", "luogo"};   // luogo: la terza parte armata (M7)
        auto& x = e->s_.unita[i];
        x.parte = ciclo(P, x.parte, d);
        const int lato = x.parte == "usa" ? 0x08 : x.parte == "urss" ? 0x10 : x.parte == "luogo" ? 0x18 : 0;
        x.flag = (x.flag & ~0x18) | lato;
        e->cambiato();
    }));
    voce(y, "ROTTA", std::to_string(static_cast<int>(u.rotta)), [e, i](int d) { e->s_.unita[i].rotta = normalizza_360(e->s_.unita[i].rotta + 15.0 * d); e->cambiato(); });
    voce(y, "VELOCITA' KN", std::to_string(static_cast<int>(std::lround(u.velocita))), [e, i](int d) {
        e->s_.unita[i].velocita = std::max(0.0, e->s_.unita[i].velocita + d);
        e->cambiato();
    });
    if (u.categoria == "sottomarino")
        voce(y, "QUOTA M", std::to_string(static_cast<int>(u.quota)), [e, i](int d) { e->s_.unita[i].quota = std::max(10.0, e->s_.unita[i].quota + 10.0 * d); e->cambiato(); });
    if (u.giocatore) return;
    if (u.classe == "foxtrot" || u.classe == "kilo")   // la batteria dei diesel dell'IA (Enhanced): a 0,05 emergono per forza
        voce(y, "BATTERIA", u.batteria ? std::to_string(static_cast<int>(std::lround(*u.batteria * 100.0))) + "%" : "80% (PREDEFINITA)", [e, i](int d) {
            auto& b = e->s_.unita[i].batteria;
            const double x = b.value_or(0.8) + 0.1 * d;
            if (x > 1.0001 || x < 0.0999) b.reset();
            else b = x;
            e->cambiato();
        });
    if (u.categoria != "aereo")   // M6: in avaria (senza propulsione) o incagliata, finche' una regola non la fa rimorchiare
        voce(y, "STATO", u.stato.empty() ? std::string("IN MARE") : maiu(u.stato), [e, i](int d) {
            e->s_.unita[i].stato = ciclo(STATI_UNITA, e->s_.unita[i].stato, d);
            e->cambiato();
        });
    if (u.categoria == "nave")   // un sonar attivo speciale (Enhanced): l'esempio dello Slava, 230 dB a 3,5 kHz, antenna di 15 m
        voce(y, "SONAR SPECIALE", u.sonar_speciale ? std::to_string(static_cast<int>(u.sonar_speciale->sl_db)) + " DB" +
                                                         (u.sonar_speciale->ogni_s > 0.0 ? ", PING OGNI MINUTO" : "")
                                                   : std::string("NO"),
             [e, i](int d) {
                 auto& x = e->s_.unita[i].sonar_speciale;
                 if (!x) x = SonarSpecialeScenario{230.0, 3500.0, 1.0, 0.005, 0.0, 15.0};
                 else if (d > 0 && x->ogni_s <= 0.0) x->ogni_s = 60.0;
                 else x.reset();
                 e->cambiato();
             });
    if (u.sonar_speciale)
        voce(y, "ANTENNA", u.sonar_speciale->antenna.empty() ? std::string("SFERA") : maiu(u.sonar_speciale->antenna), [e, i](int d) {
            auto& a = e->s_.unita[i].sonar_speciale->antenna;
            a = ciclo(ANTENNE_SONAR, a, d);
            e->cambiato();
        });
    // le armi che lancia sono inerti (il Kilo dell'I03, il giocatore dell'S01)
    voce(y, "ARMI", u.armi_esercitazione ? "DA ESERCITAZIONE" : "VERE", [e, i](int) {
        e->s_.unita[i].armi_esercitazione = !e->s_.unita[i].armi_esercitazione;
        e->cambiato();
    });
    // l'EMCON di partenza (Enhanced; S13: il bersaglio coi simulatori radar accesi); vuoto: quello della parte
    voce(y, "EMCON", u.emcon.empty() ? std::string("DELLA PARTE") : maiu(u.emcon), [e, i](int d) {
        e->s_.unita[i].emcon = ciclo(EMCON, e->s_.unita[i].emcon, d);
        e->cambiato();
    });
    // le luci di navigazione di notte (Enhanced, docs/27); vuoto: le sceglie il motore dal tipo, dalla velocita' e dalla guerra
    voce(y, "LUCI", u.luci.empty() ? std::string("AUTOMATICHE") : maiu(u.luci), [e, i](int d) {
        e->s_.unita[i].luci = ciclo(LUCI, e->s_.unita[i].luci, d);
        e->cambiato();
    });
    voce(y, "COMPORTAMENTO", NOMI_COMPORTAMENTI[indice(COMPORTAMENTI, u.comportamento)], [e, i](int d) {
        e->s_.unita[i].comportamento = ciclo(COMPORTAMENTI, e->s_.unita[i].comportamento, d);
        e->cambiato();
    });
    const auto opz = [](const auto& o, auto f) { return o ? f(*o) : std::string("MISSIONE"); };
    voce(y, "PARANOIA", opz(u.parametri.paranoia, [](int x) { return std::to_string(x); }), [e, i](int d) {
        auto& p = e->s_.unita[i].parametri.paranoia;
        const int v = p ? *p + d : (d > 0 ? 0 : 3);
        if (v < 0 || v > 3) p.reset();
        else p = v;
        e->cambiato();
    });
    if (u.categoria == "sottomarino")
        voce(y, "SALVE", opz(u.parametri.salve, [](int x) { return std::to_string(x); }), [e, i](int d) {
            auto& p = e->s_.unita[i].parametri.salve;
            const int v = p ? *p + d : (d > 0 ? 1 : 6);
            if (v < 1 || v > 6) p.reset();
            else p = v;
            e->cambiato();
        });
    if (u.categoria != "aereo")
        voce(y, "PORTATA NM", opz(u.parametri.portata_nm, [](double x) { return std::to_string(static_cast<int>(x)); }), [e, i](int d) {
            auto& p = e->s_.unita[i].parametri.portata_nm;
            const double v = p ? *p + d : (d > 0 ? 1.0 : 8.0);
            if (v < 1.0 || v > 8.0) p.reset();
            else p = v;
            e->cambiato();
        });
    if (u.comportamento == "R_pedina")
        voce(y, "DISTANZA NM", opz(u.parametri.distanza_nm, [](double x) { return std::to_string(static_cast<int>(x)); }), [e, i](int d) {
            auto& p = e->s_.unita[i].parametri.distanza_nm;
            const double v = p ? *p + d : (d > 0 ? 1.0 : 8.0);
            if (v < 1.0 || v > 8.0) p.reset();
            else p = v;
            e->cambiato();
        });
    voce(y, "NEL BRIEFING", si_no(!u.nascosta), [e, i](int) { e->s_.unita[i].nascosta = !e->s_.unita[i].nascosta; e->cambiato(); });
    std::vector<std::string> regole{""};
    for (const auto& r : s_.regole) regole.push_back(r.id);
    voce(y, "DI RISERVA", u.entra_con.empty() ? "NO" : "REGOLA " + maiu(u.entra_con), [e, i, regole](int d) {
        auto it = std::find(regole.begin(), regole.end(), e->s_.unita[i].entra_con);
        int k = it == regole.end() ? 0 : static_cast<int>(it - regole.begin());
        k = (k + d + static_cast<int>(regole.size())) % static_cast<int>(regole.size());
        e->s_.unita[i].entra_con = regole[static_cast<std::size_t>(k)];
        e->cambiato();
    });
    if (u.categoria == "aereo") {
        const bool asw = std::any_of(s_.aerei_asw.begin(), s_.aerei_asw.end(), [&](const AereoAswScenario& a) { return a.unita == u.id; });
        voce(y, "ASW ENHANCED", si_no(asw), [e, i](int) {
            auto& v = e->s_.aerei_asw;
            const int id = e->s_.unita[i].id;
            const auto it = std::find_if(v.begin(), v.end(), [&](const AereoAswScenario& a) { return a.unita == id; });
            if (it != v.end()) v.erase(it);
            else {
                AereoAswScenario a;
                a.unita = id;
                a.situazione = e->s_.situazione;
                a.contro_giocatore = e->s_.unita[i].parte != e->s_.unita[0].parte;
                a.zona = e->s_.unita[i].pos;
                v.push_back(a);
            }
            e->cambiato();
        });
    }
    y += 4;
    riga(y, "ELIMINA L'UNITA' (X)", colori::ROSSO, [e](int) { e->elimina_unita(e->unita_); });
}

void SchermataEditor::pannello_rotte() const {
    int y = CONTENUTO_Y;
    auto* e = QUI;
    riga(y, "ROTTA (CLIC DESTRO: UN PUNTO)", colori::CIANO_CHIARO);
    if (unita_ < 0 || unita_ >= static_cast<int>(s_.unita.size())) {
        riga(y, "SCEGLI UN'UNITA' SULLA CARTA", colori::GRIGIO);
        return;
    }
    const auto i = static_cast<std::size_t>(unita_);
    const auto& u = s_.unita[i];
    riga(y, "UNITA' " + std::to_string(u.id) + " " + maiu(u.classe), colori::BIANCO);
    voce(y, "PUNTI", std::to_string(u.punti.size()), {});
    static const char* const FINI[] = {"SI FERMA", "RICOMINCIA", "ESCE DAL TEATRO"};
    const int fine = u.ciclica ? 1 : u.esce ? 2 : 0;
    voce(y, "A FINE ROTTA", FINI[fine], [e, i](int d) {
        const int k = ((e->s_.unita[i].ciclica ? 1 : e->s_.unita[i].esce ? 2 : 0) + d + 3) % 3;
        e->s_.unita[i].ciclica = k == 1;
        e->s_.unita[i].esce = k == 2;
        e->cambiato();
    });
    riga(y, "TOGLI L'ULTIMO PUNTO (X)", colori::BIANCO, [e, i](int) {
        if (!e->s_.unita[i].punti.empty()) e->s_.unita[i].punti.pop_back();
        e->cambiato();
    });
    riga(y, "CANCELLA LA ROTTA", colori::ROSSO, [e, i](int) { e->s_.unita[i].punti.clear(); e->cambiato(); });
    y += 4;
    if (u.giocatore) riga(y, "LA ROTTA DEL GIOCATORE NON SI USA", colori::GRIGIO);
}

void SchermataEditor::pannello_zone() const {
    int y = CONTENUTO_Y;
    auto* e = QUI;
    riga(y, "ZONE (CLIC DESTRO: UNA NUOVA)", colori::CIANO_CHIARO);
    for (std::size_t k = 0; k < s_.zone.size(); ++k)
        riga(y, (static_cast<int>(k) == zona_ ? "> " : "  ") + maiu(s_.zone[k].id) + " " + maiu(s_.zone[k].nome[0]),
             static_cast<int>(k) == zona_ ? colori::GIALLO : colori::BIANCO, [e, k](int) { e->zona_ = static_cast<int>(k); e->elementi_validi_ = false; });
    if (zona_ < 0 || zona_ >= static_cast<int>(s_.zone.size())) return;
    const auto i = static_cast<std::size_t>(zona_);
    const auto& z = s_.zone[i];
    y += 4;
    voce(y, "ID", maiu(z.id), [e, i](int) { e->scrivi_testo(&e->s_.zone[i].id, "ID DELLA ZONA"); });
    voce(y, "NOME", maiu(z.nome[0]), [e, i](int) { e->scrivi_testo(&e->s_.zone[i].nome[0], "NOME DELLA ZONA"); });
    voce(y, "NOME EN", maiu(z.nome[1]), [e, i](int) { e->scrivi_testo(&e->s_.zone[i].nome[1], "NOME INGLESE"); });
    voce(y, "RAGGIO M", std::to_string(static_cast<int>(z.cerchio.r)), [e, i](int d) {
        e->s_.zone[i].cerchio.r = std::clamp(e->s_.zone[i].cerchio.r + 500.0 * d, 500.0, 50000.0);
        e->cambiato();
    });
    // la sala radio la dichiara al giocatore (Enhanced): l'area d'azione, dove navi e aerei amici non usano armi antisom
    voce(y, "DALLA RADIO", z.radio.empty() ? std::string("NO") : maiu(z.radio), [e, i](int d) {
        e->s_.zone[i].radio = ciclo(ZONE_RADIO, e->s_.zone[i].radio, d);
        e->cambiato();
    });
    if (!z.radio.empty())
        voce(y, "DICHIARATA", z.da_messaggio ? "DA UN MESSAGGIO" : "ALL'INIZIO", [e, i](int) {
            e->s_.zone[i].da_messaggio = !e->s_.zone[i].da_messaggio;
            e->cambiato();
        });
    riga(y, "ELIMINA LA ZONA (X)", colori::ROSSO, [e, i](int) {
        e->s_.zone.erase(e->s_.zone.begin() + static_cast<long>(i));
        e->zona_ = -1;
        e->cambiato();
    });
    y += 4;
    riga(y, "LE REGOLE CONTROLLANO IL CERCHIO.", colori::GRIGIO);
}

void SchermataEditor::pannello_obiettivi() const {
    int y = CONTENUTO_Y;
    auto* e = QUI;
    riga(y, "OBIETTIVI", colori::CIANO_CHIARO);
    for (std::size_t k = 0; k < s_.obiettivi.size(); ++k)
        riga(y, (static_cast<int>(k) == obiettivo_ ? "> " : "  ") + maiu(s_.obiettivi[k].id) + " " + maiu(s_.obiettivi[k].tipo),
             static_cast<int>(k) == obiettivo_ ? colori::GIALLO : colori::BIANCO,
             [e, k](int) { e->obiettivo_ = static_cast<int>(k); e->elementi_validi_ = false; });
    riga(y, "+ NUOVO OBIETTIVO", colori::VERDE_CHIARO, [e](int) {
        ObiettivoScenario o;
        o.id = "O" + std::to_string(e->s_.obiettivi.size() + 1);
        o.tipo = "affondare";
        e->s_.obiettivi.push_back(o);
        e->obiettivo_ = static_cast<int>(e->s_.obiettivi.size()) - 1;
        e->cambiato();
    });
    if (obiettivo_ < 0 || obiettivo_ >= static_cast<int>(s_.obiettivi.size())) return;
    const auto i = static_cast<std::size_t>(obiettivo_);
    const auto& o = s_.obiettivi[i];
    y += 4;
    voce(y, "ID", maiu(o.id), [e, i](int) { e->scrivi_testo(&e->s_.obiettivi[i].id, "ID DELL'OBIETTIVO"); });
    voce(y, "TIPO", maiu(o.tipo), [e, i](int d) { e->s_.obiettivi[i].tipo = ciclo(TIPI_OBIETTIVO, e->s_.obiettivi[i].tipo, d); e->cambiato(); });
    std::string ids;
    for (const int x : o.unita) ids += std::to_string(x) + " ";
    voce(y, "UNITA'", ids.empty() ? "-" : ids, {});
    riga(y, "+ AGGIUNGI L'UNITA' SCELTA SULLA CARTA", colori::BIANCO, [e, i](int) {
        if (e->unita_ < 0 || e->unita_ >= static_cast<int>(e->s_.unita.size())) return;
        auto& v = e->s_.obiettivi[i].unita;
        const int id = e->s_.unita[static_cast<std::size_t>(e->unita_)].id;
        if (std::find(v.begin(), v.end(), id) == v.end()) v.push_back(id);
        e->cambiato();
    });
    riga(y, "- TOGLI L'ULTIMA UNITA'", colori::BIANCO, [e, i](int) {
        if (!e->s_.obiettivi[i].unita.empty()) e->s_.obiettivi[i].unita.pop_back();
        e->cambiato();
    });
    if (o.tipo == "contatto" || o.tipo == "zona_inviolata")   // contatto: minuti di contatto; zona: la tolleranza dentro
        voce(y, o.tipo == "contatto" ? "MINUTI" : "TOLLERANZA MIN", std::to_string(static_cast<int>(o.minuti)), [e, i](int d) {
            auto& v = e->s_.obiettivi[i].minuti;
            v = std::clamp(v + (e->s_.obiettivi[i].tipo == "contatto" ? 5.0 : 1.0) * d, 0.0, 120.0);
            e->cambiato();
        });
    if (o.tipo == "foto" || o.tipo == "raggiungere")   // foto: entro la distanza; raggiungere senza zona: un'unita' in moto
        voce(y, "DISTANZA", o.distanza_m > 0.0 ? std::to_string(static_cast<int>(o.distanza_m)) + " M" : "-", [e, i](int d) {
            auto& v = e->s_.obiettivi[i].distanza_m;
            v = std::clamp(v + 500.0 * d, 0.0, 20000.0);
            e->cambiato();
        });
    if (o.tipo == "zona_inviolata")   // nessuna unita' della parte nella zona
        voce(y, "PARTE", o.parte.empty() ? "OSTILE" : maiu(o.parte), [e, i](int d) {
            auto& v = e->s_.obiettivi[i].parte;
            v = ciclo(PARTI_ZONA, v, d);
            e->cambiato();
        });
    if (o.tipo == "tlam") {   // i TLAM della missione arrivati (MINIMO), di serie nella finestra del tempo sul bersaglio
        std::vector<std::string> missioni{""};
        for (const auto& x : s_.tlam) missioni.push_back(x.id);
        voce(y, "MISSIONE TLAM", o.missione.empty() ? std::string("QUALUNQUE") : maiu(o.missione), [e, i, missioni](int d) {
            auto& v = e->s_.obiettivi[i].missione;
            auto it = std::find(missioni.begin(), missioni.end(), v);
            const int n = static_cast<int>(missioni.size());
            const int k = it == missioni.end() ? 0 : static_cast<int>(it - missioni.begin());
            v = missioni[static_cast<std::size_t>(((k + d) % n + n) % n)];
            e->cambiato();
        });
        voce(y, "NELLA FINESTRA", si_no(o.finestra), [e, i](int) { e->s_.obiettivi[i].finestra = !e->s_.obiettivi[i].finestra; e->cambiato(); });
    }
    if (o.tipo == "scatola")   // il lancio dell'Harpoon con la ricerca al piu' di questa taglia
        voce(y, "SCATOLA MASSIMA", o.massima.empty() ? std::string("-") : maiu(o.massima), [e, i](int d) {
            auto& v = e->s_.obiettivi[i].massima;
            v = ciclo(TAGLIE_SCATOLA, v, d);
            e->cambiato();
        });
    if (o.tipo == "telemetro")   // le distanze lette col telemetro entro questo errore relativo, a passi del 5%
        voce(y, "ERRORE MASSIMO", o.errore > 0.0 ? std::to_string(static_cast<int>(std::lround(o.errore * 100.0))) + "%" : "-", [e, i](int d) {
            auto& v = e->s_.obiettivi[i].errore;
            v = std::clamp(std::round((v + 0.05 * d) * 20.0) / 20.0, 0.0, 0.5);
            e->cambiato();
        });
    if (o.tipo == "rotta_stimata")   // le rotte dall'AOB entro questo errore, a passi di 5 gradi
        voce(y, "ERRORE MASSIMO", o.errore > 0.0 ? std::to_string(static_cast<int>(std::lround(o.errore))) + " GRADI" : "-", [e, i](int d) {
            auto& v = e->s_.obiettivi[i].errore;
            v = std::clamp(v + 5.0 * d, 0.0, 90.0);
            e->cambiato();
        });
    if (o.tipo == "rotta_stimata")   // dall'AOB al periscopio, o dalla TMA al MATE (anche ripetute sulla stessa unita')
        voce(y, "STIMA", o.da_tma ? "TMA AL MATE" : "AOB AL PERISCOPIO", [e, i](int) {
            auto& v = e->s_.obiettivi[i].da_tma;
            v = !v;
            e->cambiato();
        });
    if (o.tipo == "variabile")   // raggiunto con la variabile d'autore almeno al minimo
        voce(y, "VARIABILE", o.indice < 0 ? "-" : "V" + std::to_string(o.indice), [e, i](int d) {
            auto& v = e->s_.obiettivi[i].indice;
            v = ciclo_intero(v < 0 ? (d > 0 ? -1 : 1) : v, d, 0, 9);
            e->cambiato();
        });
    voce(y, "MINIMO", o.minimo < 0 ? "TUTTE" : std::to_string(o.minimo), [e, i](int d) {
        auto& m = e->s_.obiettivi[i].minimo;
        m = m + d < -1 ? 20 : m + d > 20 ? -1 : m + d;
        if (m == 0) m = d > 0 ? 1 : -1;
        e->cambiato();
    });
    voce(y, "ZONA", o.cerchio ? "R " + std::to_string(static_cast<int>(o.cerchio->r)) + " M (CLIC DESTRO)" : "NO (CLIC DESTRO)", [e, i](int d) {
        auto& c = e->s_.obiettivi[i].cerchio;
        if (!c) return;
        c->r = c->r + 500.0 * d;
        if (c->r < 500.0) c.reset();
        e->cambiato();
    });
    voce(y, "ENTRO", o.entro_s < 0 ? "SEMPRE" : ore(o.entro_s), [e, i](int d) {
        auto& t = e->s_.obiettivi[i].entro_s;
        t = t < 0 ? (d > 0 ? 1800.0 : e->s_.durata_s) : t + 1800.0 * d;
        if (t <= 0.0 || t > e->s_.durata_s) t = -1.0;
        e->cambiato();
    });
    voce(y, "PRIMARIO", si_no(o.primario), [e, i](int) { e->s_.obiettivi[i].primario = !e->s_.obiettivi[i].primario; e->cambiato(); });
    if (o.tipo == "proteggere")
        voce(y, "NON COLPIRE", si_no(!o.colpire), [e, i](int) { e->s_.obiettivi[i].colpire = !e->s_.obiettivi[i].colpire; e->cambiato(); });
    voce(y, "TESTO", maiu(o.testo[0]), [e, i](int) { e->scrivi_testo(&e->s_.obiettivi[i].testo[0], "TESTO DELL'OBIETTIVO"); });
    voce(y, "TESTO EN", maiu(o.testo[1]), [e, i](int) { e->scrivi_testo(&e->s_.obiettivi[i].testo[1], "TESTO INGLESE"); });
    riga(y, "ELIMINA L'OBIETTIVO (X)", colori::ROSSO, [e, i](int) {
        e->s_.obiettivi.erase(e->s_.obiettivi.begin() + static_cast<long>(i));
        e->obiettivo_ = -1;
        e->cambiato();
    });
}

void SchermataEditor::pannello_regole() const {
    int y = CONTENUTO_Y;
    auto* e = QUI;
    riga(y, "REGOLE: QUANDO... ALLORA...", colori::CIANO_CHIARO);
    for (std::size_t k = 0; k < s_.regole.size(); ++k)
        riga(y, (static_cast<int>(k) == regola_ ? "> " : "  ") + maiu(s_.regole[k].id), static_cast<int>(k) == regola_ ? colori::GIALLO : colori::BIANCO,
             [e, k](int) { e->regola_ = static_cast<int>(k); e->azione_ = -1; e->elementi_validi_ = false; });
    riga(y, "+ NUOVA REGOLA", colori::VERDE_CHIARO, [e](int) {
        RegolaScenario r;
        r.id = "R" + std::to_string(e->s_.regole.size() + 1);
        CondizioneScenario c;
        c.valore = 600.0;
        r.quando.push_back(c);
        AzioneScenario a;
        a.codice = 35;
        r.allora.push_back(a);
        e->s_.regole.push_back(r);
        e->regola_ = static_cast<int>(e->s_.regole.size()) - 1;
        e->cambiato();
    });
    if (regola_ < 0 || regola_ >= static_cast<int>(s_.regole.size())) return;
    const auto ri = static_cast<std::size_t>(regola_);
    const auto& r = s_.regole[ri];
    y += 4;
    voce(y, "ID", maiu(r.id), [e, ri](int) { e->scrivi_testo(&e->s_.regole[ri].id, "ID DELLA REGOLA"); });
    voce(y, "UNA VOLTA", si_no(r.una_volta), [e, ri](int) { e->s_.regole[ri].una_volta = !e->s_.regole[ri].una_volta; e->cambiato(); });
    // i valori che si scelgono a ciclo: unita', zone, regole
    std::vector<int> ids;
    for (const auto& u : s_.unita) ids.push_back(u.id);
    std::vector<std::string> zone, regole;
    for (const auto& z : s_.zone) zone.push_back(z.id);
    for (const auto& x : s_.regole) regole.push_back(x.id);
    const auto ciclo_id = [ids](int x, int d) {
        if (ids.empty()) return x;
        auto it = std::find(ids.begin(), ids.end(), x);
        int k = it == ids.end() ? 0 : static_cast<int>(it - ids.begin()) + d;
        return ids[static_cast<std::size_t>((k + static_cast<int>(ids.size())) % static_cast<int>(ids.size()))];
    };
    const auto ciclo_nome = [](const std::vector<std::string>& v, const std::string& x, int d) {
        if (v.empty()) return x;
        auto it = std::find(v.begin(), v.end(), x);
        int k = it == v.end() ? 0 : static_cast<int>(it - v.begin()) + d;
        return v[static_cast<std::size_t>((k + static_cast<int>(v.size())) % static_cast<int>(v.size()))];
    };
    riga(y, "QUANDO (TUTTE VERE):", colori::CIANO_CHIARO);
    for (std::size_t k = 0; k < r.quando.size(); ++k) {
        const auto& c = r.quando[k];
        using T = CondizioneScenario::Tipo;
        riga(y, " " + std::string(CONDIZIONI[static_cast<int>(c.tipo)]), colori::BIANCO, [e, ri, k](int d) {
            auto& x = e->s_.regole[ri].quando[k];
            x.tipo = static_cast<T>((static_cast<int>(x.tipo) + d + N_CONDIZIONI) % N_CONDIZIONI);
            e->cambiato();
        });
        std::string valore;
        std::function<void(int)> az;
        switch (c.tipo) {
            case T::tempo:
                valore = ore(c.valore);
                az = [e, ri, k](int d) { auto& v = e->s_.regole[ri].quando[k].valore; v = std::max(0.0, v + 300.0 * d); e->cambiato(); };
                break;
            case T::unita_colpita:
            case T::unita_fuori:
                valore = "UNITA' " + std::to_string(c.unita);
                az = [e, ri, k, ciclo_id](int d) { auto& v = e->s_.regole[ri].quando[k].unita; v = ciclo_id(v, d); e->cambiato(); };
                break;
            case T::giocatore_in_zona:
            case T::giocatore_fuori_zona:
                valore = "ZONA " + maiu(c.zona);
                az = [e, ri, k, zone, ciclo_nome](int d) { auto& v = e->s_.regole[ri].quando[k].zona; v = ciclo_nome(zone, v, d); e->cambiato(); };
                break;
            case T::unita_in_zona:
                valore = "UNITA' " + std::to_string(c.unita) + " ZONA " + maiu(c.zona);
                az = [e, ri, k, zone, ciclo_id, ciclo_nome](int d) {
                    auto& x = e->s_.regole[ri].quando[k];
                    if (d > 0) x.unita = ciclo_id(x.unita, 1);
                    else x.zona = ciclo_nome(zone, x.zona, 1);
                    e->cambiato();
                };
                break;
            case T::variabile:
                valore = "V" + std::to_string(c.indice) + " " + c.confronto + " " + std::to_string(static_cast<int>(c.valore));
                az = [e, ri, k](int d) {
                    auto& x = e->s_.regole[ri].quando[k];
                    if (d > 0) x.valore += 1.0;
                    else x.confronto = x.confronto == "=" ? ">" : x.confronto == ">" ? "<" : "=";
                    e->cambiato();
                };
                break;
            case T::regola_scattata:
                valore = "REGOLA " + maiu(c.regola);
                az = [e, ri, k, regole, ciclo_nome](int d) { auto& v = e->s_.regole[ri].quando[k].regola; v = ciclo_nome(regole, v, d); e->cambiato(); };
                break;
            case T::vicino_al_cavo:
            case T::giocatore_vicino_a: {   // sinistro: la distanza (al cavo a passi di 20 m, all'unita' di 500); destro: l'unita'
                const bool cavo = c.tipo == T::vicino_al_cavo;
                valore = "UNITA' " + std::to_string(c.unita) + " ENTRO " + std::to_string(static_cast<int>(c.valore)) + " M";
                az = [e, ri, k, ciclo_id, cavo](int d) {
                    auto& x = e->s_.regole[ri].quando[k];
                    if (d > 0) x.unita = ciclo_id(x.unita, 1);
                    else if (cavo) x.valore = x.valore >= 500.0 ? 20.0 : x.valore + 20.0;
                    else x.valore = x.valore >= 20000.0 ? 500.0 : x.valore + 500.0;
                    e->cambiato();
                };
                break;
            }
            case T::unita_in_superficie:
            case T::unita_in_avaria:
            case T::unita_rimorchiata:
                valore = "UNITA' " + std::to_string(c.unita);
                az = [e, ri, k, ciclo_id](int d) { auto& v = e->s_.regole[ri].quando[k].unita; v = ciclo_id(v, d); e->cambiato(); };
                break;
            case T::giocatore_sopra_nodi:
            case T::giocatore_sotto_nodi:
                valore = std::to_string(static_cast<int>(c.valore)) + " NODI";
                az = [e, ri, k](int d) { auto& v = e->s_.regole[ri].quando[k].valore; v = std::clamp(v + d, 1.0, 40.0); e->cambiato(); };
                break;
            case T::giocatore_a_quota:   // sinistro: il fondo della fascia; destro: la cima (passi di 10 m)
                valore = "DA " + std::to_string(static_cast<int>(c.valore)) + " A " + std::to_string(static_cast<int>(c.fino_a)) + " M";
                az = [e, ri, k](int d) {
                    auto& x = e->s_.regole[ri].quando[k];
                    if (d > 0) x.fino_a = x.fino_a >= 600.0 ? x.valore : x.fino_a + 10.0;
                    else x.valore = x.valore + 10.0 > x.fino_a ? 0.0 : x.valore + 10.0;
                    e->cambiato();
                };
                break;
            case T::obiettivo_compiuto: {
                std::vector<std::string> obiettivi;
                for (const auto& o : s_.obiettivi) obiettivi.push_back(o.id);
                valore = "OBIETTIVO " + maiu(c.obiettivo);
                az = [e, ri, k, obiettivi, ciclo_nome](int d) { auto& v = e->s_.regole[ri].quando[k].obiettivo; v = ciclo_nome(obiettivi, v, d); e->cambiato(); };
                break;
            }
            case T::contatto_su:
            case T::senza_contatto_su:
                valore = "UNITA' " + std::to_string(c.unita) + " " + std::to_string(static_cast<int>(c.valore)) + " MIN";
                az = [e, ri, k, ciclo_id](int d) {
                    auto& x = e->s_.regole[ri].quando[k];
                    if (d > 0) x.unita = ciclo_id(x.unita, 1);
                    else x.valore = x.valore >= 60.0 ? 1.0 : x.valore + 1.0;
                    e->cambiato();
                };
                break;
            case T::ping_vicino:
                valore = "UNITA' " + std::to_string(c.unita) + " ENTRO " + std::to_string(static_cast<int>(c.valore)) + " M";
                az = [e, ri, k, ciclo_id](int d) {
                    auto& x = e->s_.regole[ri].quando[k];
                    if (d > 0) x.unita = ciclo_id(x.unita, 1);
                    else x.valore = x.valore >= 20000.0 ? 500.0 : x.valore + 500.0;
                    e->cambiato();
                };
                break;
            case T::unita_vede:
            case T::unita_non_vede:
            case T::unita_classifica: {   // sinistro: l'osservatore; destro: i minuti (non per la classificazione); l'osservata
                                          // in un'altra riga
                static const char* const CONFIDENZE[] = {"NONSUB", "POSSUB", "PROBSUB", "CERTSUB"};   // unita_classifica
                const bool minuti = c.tipo != T::unita_classifica;
                valore = "UNITA' " + std::to_string(c.unita) +
                         (minuti ? " " + std::to_string(static_cast<int>(c.valore)) + " MIN"
                                 : std::string(" DA ") + CONFIDENZE[std::clamp(static_cast<int>(c.valore), 0, 3)]);
                az = [e, ri, k, ciclo_id, minuti](int d) {
                    auto& x = e->s_.regole[ri].quando[k];
                    if (d < 0) x.unita = ciclo_id(x.unita, 1);
                    else if (minuti) x.valore = x.valore >= 60.0 ? 1.0 : x.valore + 1.0;
                    else x.valore = x.valore >= 3.0 ? 0.0 : x.valore + 1.0;
                    e->cambiato();
                };
                riga(y, "   " + valore, colori::GRIGIO, az);
                valore = c.bersaglio < 0 ? "SU: IL GIOCATORE" : "SU: UNITA' " + std::to_string(c.bersaglio);
                az = [e, ri, k, ids](int d) {
                    auto& v = e->s_.regole[ri].quando[k].bersaglio;
                    auto it = std::find(ids.begin(), ids.end(), v);
                    const int i = it == ids.end() ? -1 : static_cast<int>(it - ids.begin());
                    const int n = static_cast<int>(ids.size()) + 1;
                    const int j = ((i + 1 + d) % n + n) % n;
                    v = j == 0 ? -1 : ids[static_cast<std::size_t>(j - 1)];
                    e->cambiato();
                };
                break;
            }
            case T::giocatore_ha_trasmesso:   // sinistro: dopo quanto (passi di 5 minuti); destro: il tipo di trasmissione
                valore = (c.trasmissione.empty() ? std::string("CONTATTO") : maiu(c.trasmissione)) + " DOPO " + ore(c.valore);
                az = [e, ri, k](int d) {
                    auto& x = e->s_.regole[ri].quando[k];
                    if (d > 0) x.trasmissione = ciclo(TRASMISSIONI, x.trasmissione, 1);
                    else x.valore = x.valore >= e->s_.durata_s ? 0.0 : x.valore + 300.0;
                    e->cambiato();
                };
                break;
            case T::evento: {   // sinistro: dopo quanto (passi di 5 minuti); destro: l'evento del registro (nome_evento)
                valore = (c.evento.empty() ? std::string("-") : maiu(c.evento)) + " DOPO " + ore(c.valore);
                az = [e, ri, k](int d) {
                    auto& x = e->s_.regole[ri].quando[k];
                    if (d > 0) {
                        const int n = static_cast<int>(TipoEvento::numero);
                        int j = -1;
                        for (int t = 0; t < n; ++t)
                            if (x.evento == nome_evento(static_cast<TipoEvento>(t))) j = t;
                        x.evento = nome_evento(static_cast<TipoEvento>((j + 1) % n));
                    } else {
                        x.valore = x.valore >= e->s_.durata_s ? 0.0 : x.valore + 300.0;
                    }
                    e->cambiato();
                };
                break;
            }
            case T::ostile_ha_lanciato:
                valore = c.unita < 0 ? "UNA QUALUNQUE" : "UNITA' " + std::to_string(c.unita);
                az = [e, ri, k, ids](int d) {
                    auto& v = e->s_.regole[ri].quando[k].unita;
                    auto it = std::find(ids.begin(), ids.end(), v);
                    const int i = it == ids.end() ? -1 : static_cast<int>(it - ids.begin());
                    const int n = static_cast<int>(ids.size()) + 1;   // -1 e poi le unita'
                    const int j = ((i + 1 + d) % n + n) % n;
                    v = j == 0 ? -1 : ids[static_cast<std::size_t>(j - 1)];
                    e->cambiato();
                };
                break;
            default: break;
        }
        if (az) riga(y, "   " + valore, colori::GRIGIO, az);
        riga(y, "   - TOGLI", colori::ROSSO, [e, ri, k](int) {
            auto& q = e->s_.regole[ri].quando;
            q.erase(q.begin() + static_cast<long>(k));
            e->cambiato();
        });
    }
    riga(y, " + CONDIZIONE", colori::VERDE_CHIARO, [e, ri](int) { e->s_.regole[ri].quando.push_back({}); e->cambiato(); });
    riga(y, "ALLORA:", colori::CIANO_CHIARO);
    for (std::size_t k = 0; k < r.allora.size(); ++k) {
        const auto& a = r.allora[k];
        using T = AzioneScenario::Tipo;
        riga(y, (static_cast<int>(k) == azione_ ? ">" : " ") + std::string(AZIONI[static_cast<int>(a.tipo)]), colori::BIANCO, [e, ri, k](int d) {
            auto& x = e->s_.regole[ri].allora[k];
            x.tipo = static_cast<T>((static_cast<int>(x.tipo) + d + N_AZIONI) % N_AZIONI);
            if (x.tipo == T::avaria || x.tipo == T::esercitazione) x.valore = 1;   // di partenza: in avaria, armi inerti
            e->azione_ = static_cast<int>(k);
            e->cambiato();
        });
        switch (a.tipo) {
            case T::messaggio:   // un codice di messaggi_bordo.tsv, o un testo proprio della missione (E6)
                if (a.testo[0].empty())
                    riga(y, "   CODICE " + std::to_string(a.codice), colori::GRIGIO,
                         [e, ri, k](int d) { auto& v = e->s_.regole[ri].allora[k].codice; v = ciclo_intero(v, d, 1, 99); e->cambiato(); });
                riga(y, "   TESTO PROPRIO " + (a.testo[0].empty() ? std::string("-") : maiu(a.testo[0])), colori::GRIGIO,
                     [e, ri, k](int) { e->scrivi_testo(&e->s_.regole[ri].allora[k].testo[0], "MESSAGGIO DI BORDO"); });
                if (!a.testo[0].empty())
                    riga(y, "   TESTO EN " + maiu(a.testo[1]), colori::GRIGIO,
                         [e, ri, k](int) { e->scrivi_testo(&e->s_.regole[ri].allora[k].testo[1], "MESSAGGIO IN INGLESE"); });
                break;
            case T::fine:
                riga(y, "   ESITO " + std::to_string(a.codice) + " " + maiu(a.categoria.empty() ? "dagli obiettivi" : a.categoria), colori::GRIGIO,
                     [e, ri, k](int d) {
                         auto& x = e->s_.regole[ri].allora[k];
                         if (d > 0) x.codice = ciclo_intero(x.codice < 30 ? 29 : x.codice, 1, 30, 99);
                         else x.categoria = ciclo(CATEGORIE_ESITO, x.categoria, 1);
                         e->cambiato();
                     });
                riga(y, "   TESTO " + maiu(a.testo[0]), colori::GRIGIO, [e, ri, k](int) { e->scrivi_testo(&e->s_.regole[ri].allora[k].testo[0], "TESTO DELL'ESITO"); });
                riga(y, "   TESTO EN " + maiu(a.testo[1]), colori::GRIGIO, [e, ri, k](int) { e->scrivi_testo(&e->s_.regole[ri].allora[k].testo[1], "TESTO INGLESE"); });
                break;
            case T::variabile:
                riga(y, "   V" + std::to_string(a.indice) + " " + a.operazione + " " + std::to_string(a.valore), colori::GRIGIO, [e, ri, k](int d) {
                    auto& x = e->s_.regole[ri].allora[k];
                    if (d > 0) x.valore = ciclo_intero(x.valore, 1, 0, 20);
                    else x.operazione = x.operazione == "=" ? "+" : x.operazione == "+" ? "-" : "=";
                    e->cambiato();
                });
                break;
            case T::comportamento:
                riga(y, "   UNITA' " + std::to_string(a.unita) + " " + NOMI_COMPORTAMENTI[indice(COMPORTAMENTI, a.comportamento)], colori::GRIGIO,
                     [e, ri, k, ciclo_id](int d) {
                         auto& x = e->s_.regole[ri].allora[k];
                         if (d > 0) x.unita = ciclo_id(x.unita, 1);
                         else x.comportamento = ciclo(COMPORTAMENTI, x.comportamento, 1);
                         e->cambiato();
                     });
                break;
            case T::vai_a:
            case T::cue_asw:
                riga(y, a.tipo == T::vai_a ? "   UNITA' " + std::to_string(a.unita) + " (CLIC DESTRO: META)" : "   CLIC DESTRO: IL PUNTO", colori::GRIGIO,
                     [e, ri, k, ciclo_id](int d) {
                         e->azione_ = static_cast<int>(k);
                         auto& x = e->s_.regole[ri].allora[k];
                         if (x.tipo == T::vai_a) x.unita = ciclo_id(x.unita, d);
                         e->cambiato();
                     });
                if (a.tipo == T::vai_a)
                    riga(y, "   VELOCITA' " + (a.velocita < 0.0 ? std::string("DI PRIMA") : std::to_string(static_cast<int>(a.velocita)) + " NODI"), colori::GRIGIO,
                         [e, ri, k](int d) {
                             auto& v = e->s_.regole[ri].allora[k].velocita;
                             v = v < 0.0 ? (d > 0 ? 0.0 : 30.0) : v + 2.0 * d;
                             if (v > 30.0 || v < 0.0) v = -1.0;
                             e->cambiato();
                         });
                break;
            case T::entra:
                riga(y, "   UNITA' DI RISERVA " + std::to_string(a.unita), colori::GRIGIO,
                     [e, ri, k, ciclo_id](int d) { auto& v = e->s_.regole[ri].allora[k].unita; v = ciclo_id(v, d); e->cambiato(); });
                break;
            case T::sposta_zona:
                riga(y, "   ZONA " + maiu(a.zona) + " (CLIC DESTRO: DOVE)", colori::GRIGIO, [e, ri, k, zone, ciclo_nome](int d) {
                    e->azione_ = static_cast<int>(k);
                    auto& v = e->s_.regole[ri].allora[k].zona;
                    v = ciclo_nome(zone, v, d);
                    e->cambiato();
                });
                break;
            case T::allarme: break;
            case T::radio: {
                std::vector<std::string> radio;
                if (s_.radio)
                    for (const auto& m : s_.radio->messaggi) radio.push_back(m.id);
                riga(y, "   MESSAGGIO " + (a.zona.empty() ? std::string("-") : maiu(a.zona)) + (radio.empty() ? " (NESSUNO: SCHEDA BRIEFING)" : ""),
                     colori::GRIGIO, [e, ri, k, radio, ciclo_nome](int d) {
                         auto& v = e->s_.regole[ri].allora[k].zona;
                         v = ciclo_nome(radio, v, d);
                         e->cambiato();
                     });
                break;
            }
            case T::posa_mina:   // sinistro: l'unita' che posa; destro: tipo, poi quante
                riga(y, "   UNITA' " + std::to_string(a.unita) + " " + maiu(a.comportamento.empty() ? std::string("-") : a.comportamento) + " X" +
                            std::to_string(std::max(1, a.valore)),
                     colori::GRIGIO, [e, ri, k, ciclo_id](int d) {
                         auto& x = e->s_.regole[ri].allora[k];
                         if (d > 0) x.unita = ciclo_id(x.unita, 1);
                         else if (x.comportamento == "pmr2") { x.comportamento = "mk67"; x.valore = x.valore >= 8 ? 1 : std::max(1, x.valore) + 1; }
                         else x.comportamento = ciclo(TIPI_MINA, x.comportamento, 1);
                         e->cambiato();
                     });
                break;
            case T::avaria:   // sinistro: l'unita'; destro: in avaria o riparata (esercitazione: armi inerti o vere)
            case T::esercitazione:
            case T::molla_rimorchio:
                riga(y, "   UNITA' " + std::to_string(a.unita) +
                            (a.tipo == T::avaria          ? (a.valore == 0 ? " RIPARATA" : " IN AVARIA")
                             : a.tipo == T::esercitazione ? (a.valore == 0 ? " ARMI VERE" : " ARMI INERTI")
                                                          : ""),
                     colori::GRIGIO, [e, ri, k, ciclo_id](int d) {
                         auto& x = e->s_.regole[ri].allora[k];
                         if (d > 0 || x.tipo == T::molla_rimorchio) x.unita = ciclo_id(x.unita, d);
                         else x.valore = x.valore == 0 ? 1 : 0;
                         e->cambiato();
                     });
                break;
            case T::comanda_mine: {   // i gruppi delle mine comandate dello scenario, e «tutte»
                std::vector<std::string> gruppi = {""};
                for (const auto& mn : e->s_.mine)
                    if (mn.comandata && std::find(gruppi.begin(), gruppi.end(), mn.gruppo) == gruppi.end()) gruppi.push_back(mn.gruppo);
                riga(y, "   GRUPPO " + (a.zona.empty() ? std::string("TUTTE") : maiu(a.zona)), colori::GRIGIO, [e, ri, k, gruppi](int d) {
                    auto& x = e->s_.regole[ri].allora[k];
                    auto it = std::find(gruppi.begin(), gruppi.end(), x.zona);
                    const int n = static_cast<int>(gruppi.size());
                    const int i = it == gruppi.end() ? 0 : static_cast<int>(it - gruppi.begin());
                    x.zona = gruppi[static_cast<std::size_t>(((i + d) % n + n) % n)];
                    e->cambiato();
                });
                break;
            }
            case T::quota:    // sinistro: l'unita'; destro: la quota, a passi di 10 m fino a 300
            case T::rumore:   // l'unita' che fa il rumore
            case T::togli:    // l'unita' che esce dal gioco
                riga(y, "   UNITA' " + std::to_string(a.unita) + (a.tipo == T::quota ? " A " + std::to_string(a.valore) + " M" : std::string()),
                     colori::GRIGIO, [e, ri, k, ciclo_id](int d) {
                         auto& x = e->s_.regole[ri].allora[k];
                         if (d < 0 || x.tipo != T::quota) x.unita = ciclo_id(x.unita, 1);
                         else x.valore = x.valore >= 300 ? 0 : x.valore + 10;
                         e->cambiato();
                     });
                break;
            case T::salvati:   // sinistro: gli uomini in acqua (a passi di 5); destro: i salvati, fino a quelli in acqua
                riga(y, "   SALVATI " + std::to_string(a.valore) + " SU " + std::to_string(a.indice), colori::GRIGIO, [e, ri, k](int d) {
                    auto& x = e->s_.regole[ri].allora[k];
                    if (d < 0) x.indice = x.indice >= 200 ? 5 : x.indice + 5;
                    else x.valore = x.valore >= x.indice ? 0 : x.valore + 1;
                    x.valore = std::min(x.valore, x.indice);
                    e->cambiato();
                });
                break;
            case T::rimorchia:   // sinistro: il rimorchiatore; destro: il rimorchiato; poi la velocita'
                riga(y, "   UNITA' " + std::to_string(a.unita) + " RIMORCHIA " + std::to_string(a.bersaglio), colori::GRIGIO,
                     [e, ri, k, ciclo_id](int d) {
                         auto& x = e->s_.regole[ri].allora[k];
                         if (d < 0) x.unita = ciclo_id(x.unita, 1);
                         else x.bersaglio = ciclo_id(x.bersaglio, 1);
                         e->cambiato();
                     });
                riga(y, "   A " + std::to_string(static_cast<int>(a.velocita > 0.0 ? a.velocita : 6.0)) + " NODI, CAVO " +
                            std::to_string(a.valore > 0 ? a.valore : 300) + " M",
                     colori::GRIGIO, [e, ri, k](int d) {
                         auto& x = e->s_.regole[ri].allora[k];
                         if (d > 0) x.velocita = x.velocita <= 0.0 ? 7.0 : x.velocita >= 12.0 ? 2.0 : x.velocita + 1.0;   // da 6
                         else x.valore = x.valore <= 0 ? 400 : x.valore >= 1000 ? 100 : x.valore + 100;                   // da 300
                         e->cambiato();
                     });
                break;
            case T::situazione:
                riga(y, "   ARMI DELL'IA: " + maiu(a.comportamento.empty() ? std::string("-") : a.comportamento), colori::GRIGIO, [e, ri, k](int d) {
                    auto& v = e->s_.regole[ri].allora[k].comportamento;
                    v = ciclo(SITUAZIONI_IA, v, d);
                    e->cambiato();
                });
                // M7: fra quali parti cambia (vuoto: tutte, come prima); col luogo solo la coppia scelta
                riga(y, a.fra_a.empty() ? std::string("   FRA: TUTTE LE PARTI") : "   FRA: " + maiu(a.fra_a) + " E " + maiu(a.fra_b), colori::GRIGIO,
                     [e, ri, k](int d) {
                         static const std::pair<const char*, const char*> COPPIE[] = {{"", ""}, {"usa", "luogo"}, {"urss", "luogo"}, {"usa", "urss"}};
                         auto& x = e->s_.regole[ri].allora[k];
                         int j = 0;
                         for (int c = 0; c < 4; ++c)
                             if (x.fra_a == COPPIE[c].first && x.fra_b == COPPIE[c].second) j = c;
                         j = ((j + d) % 4 + 4) % 4;
                         x.fra_a = COPPIE[j].first;
                         x.fra_b = COPPIE[j].second;
                         e->cambiato();
                     });
                break;
            case T::lancia_missile: {   // sinistro: chi lancia; destro: il bersaglio (il giocatore o un'unita'); poi quanti (I11)
                riga(y, "   UNITA' " + std::to_string(a.unita) + " SU " + (a.bersaglio < 0 ? std::string("IL GIOCATORE") : "UNITA' " + std::to_string(a.bersaglio)),
                     colori::GRIGIO, [e, ri, k, ciclo_id, ids](int d) {
                         auto& x = e->s_.regole[ri].allora[k];
                         if (d < 0) {
                             x.unita = ciclo_id(x.unita, 1);
                         } else {   // -1 il giocatore, poi le unita'
                             auto it = std::find(ids.begin(), ids.end(), x.bersaglio);
                             const int i = it == ids.end() ? -1 : static_cast<int>(it - ids.begin());
                             const int n = static_cast<int>(ids.size()) + 1;
                             const int j = (i + 2) % n;
                             x.bersaglio = j == 0 ? -1 : ids[static_cast<std::size_t>(j - 1)];
                         }
                         e->cambiato();
                     });
                riga(y, "   MISSILI: " + std::to_string(std::max(1, a.valore)), colori::GRIGIO, [e, ri, k](int d) {
                    auto& v = e->s_.regole[ri].allora[k].valore;
                    v = std::clamp(std::max(1, v) + d, 1, 8);
                    e->cambiato();
                });
                break;
            }
            case T::attacca_unita:   // sinistro: chi attacca; destro: il bersaglio; poi con o senza soluzione (I03, I05)
                riga(y, "   UNITA' " + std::to_string(a.unita) + " ATTACCA " + std::to_string(a.bersaglio), colori::GRIGIO,
                     [e, ri, k, ciclo_id](int d) {
                         auto& x = e->s_.regole[ri].allora[k];
                         if (d < 0) x.unita = ciclo_id(x.unita, 1);
                         else x.bersaglio = ciclo_id(x.bersaglio, 1);
                         e->cambiato();
                     });
                riga(y, a.valore == 1 ? "   SENZA SOLUZIONE: SUL RILEVAMENTO VERO" : "   CON LA SOLUZIONE DELLA TMA", colori::GRIGIO, [e, ri, k](int) {
                    auto& v = e->s_.regole[ri].allora[k].valore;
                    v = v == 1 ? 0 : 1;
                    e->cambiato();
                });
                break;
        }
        riga(y, "   - TOGLI", colori::ROSSO, [e, ri, k](int) {
            auto& v = e->s_.regole[ri].allora;
            v.erase(v.begin() + static_cast<long>(k));
            e->azione_ = -1;
            e->cambiato();
        });
    }
    riga(y, " + AZIONE", colori::VERDE_CHIARO, [e, ri](int) {
        AzioneScenario a;
        a.codice = 35;
        e->s_.regole[ri].allora.push_back(a);
        e->cambiato();
    });
    riga(y, "ELIMINA LA REGOLA (X)", colori::ROSSO, [e, ri](int) {
        e->s_.regole.erase(e->s_.regole.begin() + static_cast<long>(ri));
        e->regola_ = -1;
        e->cambiato();
    });
}

void SchermataEditor::pannello_briefing() const {
    int y = CONTENUTO_Y;
    auto* e = QUI;
    static const char* const VISTE[] = {"BRIEFING (ITALIANO)", "BRIEFING (INGLESE)", "RADIO (ENHANCED)"};
    riga(y, VISTE[lingua_briefing_], colori::CIANO_CHIARO, [e](int d) {
        e->lingua_briefing_ = (e->lingua_briefing_ + (d < 0 ? 2 : 1)) % 3;
        e->riga_briefing_ = -1;
        e->elementi_validi_ = false;
    });
    if (lingua_briefing_ == 2) {
        pannello_radio(y);
        return;
    }
    auto& righe = s_.briefing[static_cast<std::size_t>(lingua_briefing_)];
    for (std::size_t k = 0; k < righe.size(); ++k)
        riga(y, (static_cast<int>(k) == riga_briefing_ ? "> " : "") + maiu(righe[k]), static_cast<int>(k) == riga_briefing_ ? colori::GIALLO : colori::BIANCO,
             [e, k](int) {
                 if (e->riga_briefing_ == static_cast<int>(k))
                     e->scrivi_testo(&e->s_.briefing[static_cast<std::size_t>(e->lingua_briefing_)][k], "RIGA DEL BRIEFING");
                 e->riga_briefing_ = static_cast<int>(k);
                 e->elementi_validi_ = false;
             });
    y += 4;
    riga(y, "+ AGGIUNGI UNA RIGA", colori::VERDE_CHIARO, [e](int) {
        auto& v = e->s_.briefing[static_cast<std::size_t>(e->lingua_briefing_)];
        v.push_back("");
        e->riga_briefing_ = static_cast<int>(v.size()) - 1;
        e->scrivi_testo(&v.back(), "RIGA DEL BRIEFING");
        e->cambiato();
    });
    riga(y, "- TOGLI LA RIGA SCELTA (X)", colori::ROSSO, [e](int) {
        auto& v = e->s_.briefing[static_cast<std::size_t>(e->lingua_briefing_)];
        if (e->riga_briefing_ >= 0 && e->riga_briefing_ < static_cast<int>(v.size())) v.erase(v.begin() + e->riga_briefing_);
        e->riga_briefing_ = -1;
        e->cambiato();
    });
    riga(y, "RIFAI IL BRIEFING AUTOMATICO", colori::BIANCO, [e](int) { e->briefing_automatico(); e->cambiato(); });
    riga(y, "CLIC SU UNA RIGA SCELTA: SCRIVERLA", colori::GRIGIO);
}

// La sala radio della missione (R3 ed E6): intestazione degli ordini, ROE di partenza, messaggi a tempo, su condizione
// o mandati da una regola (azione «radio»)
void SchermataEditor::pannello_radio(int& y) const {
    auto* e = QUI;
    voce(y, "RADIO DELLA MISSIONE", si_no(s_.radio.has_value()), [e](int) {
        if (e->s_.radio) e->s_.radio.reset();
        else e->s_.radio = RadioScenario{};
        e->messaggio_radio_ = -1;
        e->cambiato();
    });
    if (!s_.radio) {
        riga(y, "SENZA: SOLO GLI ORDINI DAL BRIEFING", colori::GRIGIO);
        riga(y, "E IL TRAFFICO DI FONDO", colori::GRIGIO);
        return;
    }
    const auto& r = *s_.radio;
    voce(y, "INTESTAZIONE DEGLI ORDINI", r.ordini ? "SI" : "DAL BRIEFING", [e](int) {
        auto& o = e->s_.radio->ordini;
        if (o) o.reset();
        else o = OrdiniScenario{e->s_.alfa ? "KP SF" : "COMSUBLANT", e->s_.alfa ? "PL" : "USS LOS ANGELES (SSN 688)", maiu(e->s_.titolo[1]),
                                e->s_.alfa ? "SOV. SEKRETNO" : "SECRET"};
        e->cambiato();
    });
    if (r.ordini) {
        voce(y, "  DA", maiu(r.ordini->da), [e](int) { e->scrivi_testo(&e->s_.radio->ordini->da, "MITTENTE DEGLI ORDINI"); });
        voce(y, "  A", maiu(r.ordini->a), [e](int) { e->scrivi_testo(&e->s_.radio->ordini->a, "DESTINATARIO"); });
        voce(y, "  OGGETTO", maiu(r.ordini->oggetto), [e](int) { e->scrivi_testo(&e->s_.radio->ordini->oggetto, "OGGETTO DEGLI ORDINI"); });
        voce(y, "  CLASSIFICA", r.ordini->classifica, [e](int d) {
            auto& v = e->s_.radio->ordini->classifica;
            v = ciclo(CLASSIFICHE, v, d);
            e->cambiato();
        });
    }
    // ROE di partenza e di un messaggio: una voce per categoria
    const auto voci_roe = [&](int& yy, const RoeScenario& roe, std::function<RoeScenario&()> dove, const char* rientro) {
        for (const char* c : CATEGORIE_RADIO) {
            const auto it = roe.find(c);
            voce(yy, std::string(rientro) + maiu(c), it == roe.end() ? "-" : maiu(it->second), [e, c, dove](int d) {
                auto& m = dove();
                const std::string v = ciclo(STATI_ROE, m.count(c) ? m.at(c) : std::string(), d);
                if (v.empty()) m.erase(c);
                else m[c] = v;
                e->cambiato();
            });
        }
    };
    riga(y, "ROE DI PARTENZA", colori::CIANO_CHIARO);
    voci_roe(y, r.roe, [e]() -> RoeScenario& { return e->s_.radio->roe; }, "  ");
    y += 4;
    riga(y, "MESSAGGI", colori::CIANO_CHIARO);
    for (std::size_t i = 0; i < r.messaggi.size(); ++i) {
        const auto& m = r.messaggi[i];
        const std::string quando = m.su_richiesta ? "DA REGOLA" : !m.se_evento.empty() ? maiu(m.se_evento) : ore(m.dopo_s);
        riga(y, (static_cast<int>(i) == messaggio_radio_ ? "> " : "  ") + maiu(m.id) + " " + maiu(m.precedenza) + " " + quando,
             static_cast<int>(i) == messaggio_radio_ ? colori::GIALLO : colori::BIANCO, [e, i](int) {
                 e->messaggio_radio_ = static_cast<int>(i);
                 e->elementi_validi_ = false;
             });
    }
    riga(y, "+ MESSAGGIO", colori::VERDE_CHIARO, [e](int) {
        auto& v = e->s_.radio->messaggi;
        MessaggioScenario m;
        m.id = "M" + std::to_string(v.size() + 1);
        m.dopo_s = 600.0;
        m.da = e->s_.radio->ordini ? e->s_.radio->ordini->da : e->s_.alfa ? "KP SF" : "COMSUBLANT";
        m.classifica = e->s_.alfa ? "SECRET" : "CONFIDENTIAL";
        v.push_back(m);
        e->messaggio_radio_ = static_cast<int>(v.size()) - 1;
        e->cambiato();
    });
    if (messaggio_radio_ < 0 || messaggio_radio_ >= static_cast<int>(r.messaggi.size())) return;
    const auto i = static_cast<std::size_t>(messaggio_radio_);
    const auto& m = r.messaggi[i];
    const auto msg = [e, i]() -> MessaggioScenario& { return e->s_.radio->messaggi[i]; };
    y += 4;
    voce(y, "ID", maiu(m.id), [e, msg](int) { e->scrivi_testo(&msg().id, "ID DEL MESSAGGIO"); });
    voce(y, "DA REGOLA", si_no(m.su_richiesta), [e, msg](int) { msg().su_richiesta = !msg().su_richiesta; e->cambiato(); });
    if (!m.su_richiesta) {
        voce(y, "DOPO", ore(m.dopo_s), [e, msg](int d) { msg().dopo_s = std::max(0.0, msg().dopo_s + 300.0 * d); e->cambiato(); });
        voce(y, "SE (EVENTO)", m.se_evento.empty() ? "-" : maiu(m.se_evento), [e, msg](int d) {
            msg().se_evento = ciclo(EVENTI_RADIO, msg().se_evento, d);
            e->cambiato();
        });
    }
    voce(y, "PRECEDENZA", maiu(m.precedenza), [e, msg](int d) { msg().precedenza = ciclo(PRECEDENZE, msg().precedenza, d); e->cambiato(); });
    voce(y, "DA", maiu(m.da), [e, msg](int) { e->scrivi_testo(&msg().da, "MITTENTE"); });
    voce(y, "OGGETTO", maiu(m.oggetto), [e, msg](int) { e->scrivi_testo(&msg().oggetto, "OGGETTO"); });
    voce(y, "CLASSIFICA", m.classifica, [e, msg](int d) { msg().classifica = ciclo(CLASSIFICHE, msg().classifica, d); e->cambiato(); });
    voce(y, "TESTO", maiu(m.testo[0]), [e, msg](int) { e->scrivi_testo(&msg().testo[0], "TESTO DEL MESSAGGIO"); });
    voce(y, "TESTO EN", maiu(m.testo[1]), [e, msg](int) { e->scrivi_testo(&msg().testo[1], "TESTO INGLESE"); });
    voce(y, "AUTENTICO", m.autentico ? "SI" : "NO (INGANNO)", [e, msg](int) { msg().autentico = !msg().autentico; e->cambiato(); });
    // le zone della radio (scheda Zone, «dalla radio») che il messaggio dichiara o toglie: una per voce
    std::vector<std::string> zone_radio{""};
    for (const auto& z : s_.zone)
        if (!z.radio.empty()) zone_radio.push_back(z.id);
    const auto voce_zona = [&](const char* nome, std::vector<std::string> MessaggioScenario::*campo) {
        const auto& v = m.*campo;
        voce(y, nome, v.empty() ? std::string("-") : maiu(v.front()), [e, msg, zone_radio, campo](int d) {
            auto& w = msg().*campo;
            const auto it = std::find(zone_radio.begin(), zone_radio.end(), w.empty() ? std::string() : w.front());
            const long n = static_cast<long>(zone_radio.size());
            const long k = ((it == zone_radio.end() ? 0 : it - zone_radio.begin()) + d + n) % n;
            w = zone_radio[static_cast<std::size_t>(k)].empty() ? std::vector<std::string>{} : std::vector<std::string>{zone_radio[static_cast<std::size_t>(k)]};
            e->cambiato();
        });
    };
    voce_zona("DICHIARA LA ZONA", &MessaggioScenario::zone_aggiungi);
    voce_zona("TOGLIE LA ZONA", &MessaggioScenario::zone_togli);
    riga(y, "CAMBIA LE ROE", colori::CIANO_CHIARO);
    voci_roe(y, m.roe, [msg]() -> RoeScenario& { return msg().roe; }, "  ");
    riga(y, "- TOGLI IL MESSAGGIO", colori::ROSSO, [e, i](int) {
        auto& v = e->s_.radio->messaggi;
        v.erase(v.begin() + static_cast<long>(i));
        e->messaggio_radio_ = -1;
        e->cambiato();
    });
}

void SchermataEditor::pannello_prova() const {
    int y = CONTENUTO_Y;
    auto* e = QUI;
    riga(y, "PROVA", colori::CIANO_CHIARO);
    riga(y, "SIMULA 10 MINUTI (CLASSICO)", colori::BIANCO, [e](int) { e->riepilogo_ = e->simula(600.0); e->elementi_validi_ = false; });
    riga(y, "SALVA (S)", colori::BIANCO, [e](int) { e->salva(); });
    riga(y, "ESPORTA IL PACCHETTO (E)", colori::BIANCO, [e](int) { e->esporta(); });
    riga(y, "PROVA IN MARE (G)", colori::BIANCO, {});
    if (!riepilogo_.empty()) {
        y += 4;
        std::size_t i = 0;
        while (i < riepilogo_.size()) {
            const auto j = riepilogo_.find('\n', i);
            riga(y, riepilogo_.substr(i, j == std::string::npos ? std::string::npos : j - i), colori::VERDE_CHIARO);
            if (j == std::string::npos) break;
            i = j + 1;
        }
    }
    y += 4;
    riga(y, "VERIFICA: " + std::to_string(avvisi_.size()) + " AVVISI", colori::CIANO_CHIARO);
    for (const auto& a : avvisi_) {
        const int u = a.unita;
        riga(y, maiu(a.testo), a.gravita == AvvisoScenario::Gravita::errore ? colori::ROSSO : colori::GIALLO, [e, u](int) {
            for (std::size_t k = 0; k < e->s_.unita.size(); ++k)
                if (e->s_.unita[k].id == u) {
                    e->unita_ = static_cast<int>(k);
                    e->centra(e->s_.unita[k].pos);
                }
            e->elementi_validi_ = false;
        });
    }
}

#undef QUI

// --- modifiche -----------------------------------------------------------------------------------------------------------

void SchermataEditor::cambiato() {
    modificato_ = true;
    esc_avvisato_ = false;
    elementi_validi_ = false;
    if (unita_ >= static_cast<int>(s_.unita.size())) unita_ = static_cast<int>(s_.unita.size()) - 1;
    if (fondale_) avvisi_ = verifica(s_, *fondale_, d_);
}

void SchermataEditor::imposta_carico() {
    auto& c = s_.carico;
    if (!s_.alfa) {
        c.tubi = {"mk48", "mk48", "mk48", "mk48", "esca", "esca", "harpoon"};
        c.dotazione = {{"mk48", 22}, {"harpoon", 2}, {"esca", 6}};
        if (c.tipo_missione == "antinave") {
            c.dotazione = {{"mk48", 18}, {"harpoon", 4}, {"tomahawk", 4}, {"esca", 6}};
            c.deposito = {{"mk48", 17}, {"harpoon", 4}, {"tomahawk", 4}, {"esca", 6}};
        } else if (c.tipo_missione == "minamento") {
            c.deposito = {{"mk48", 13}, {"harpoon", 2}, {"esca", 6}, {"mk67", 10}};
        } else {
            c.deposito = {{"mk48", 21}, {"harpoon", 2}, {"esca", 6}};
        }
    } else {
        if (c.tipo_missione == "scorta") c.tipo_missione = "antisom";
        c.tubi = {"type53", "type53", "type53", "type53", "type53", "esca", "esca"};
        c.dotazione = {{"type53", 18}, {"esca", 4}};
        c.deposito = c.tipo_missione == "minamento" ? std::vector<VoceCarico>{{"type53", 4}, {"esca", 4}, {"pmr2", 6}}
                                                    : std::vector<VoceCarico>{{"type53", 18}, {"esca", 4}};
        imposta_esche_mobili(c, 2);   // l'Alfa ha sempre la MG-74 dal tubo (decisione del 26/10/2026)
    }
}

int SchermataEditor::nuova_unita(Vec2 pos) {
    UnitaScenario u;
    u.id = 2;   // l'id 1 e' il secondo giocatore dell'originale (incagli, esito 4 se distrutto): l'IA parte da 2
    for (const auto& x : s_.unita) u.id = std::max(u.id, x.id + 1);
    u.classe = classe_nuova_;
    u.categoria = categoria(classe_nuova_);
    u.parte = parte_nuova_;
    u.non_combattente = CIVILI.count(u.classe) > 0 || AUSILIARIE.count(u.classe) > 0;
    const int lato = u.parte == "usa" ? 0x08 : u.parte == "urss" ? 0x10 : 0;
    u.flag = (u.categoria == "sottomarino" ? 1 : u.categoria == "aereo" ? 3 : 2) | lato | (u.non_combattente ? 0x40 : 0);
    u.pos = pos;
    const auto m = d_.modelli.find(u.classe);
    const double massima = m == d_.modelli.end() ? 20.0 : m->second.massima;
    if (u.categoria == "sottomarino") {
        const double fondo = fondale_->profondita(pos);
        u.velocita = 6.0;
        u.quota = std::max(20.0, std::min(100.0, fondo - 30.0));
        u.profonda = std::max(u.quota, std::min((m == d_.modelli.end() ? 300.0 : m->second.quota_max) * 0.8, fondo - 30.0));
        u.comportamento = "R_sottomarino";
    } else if (u.categoria == "aereo") {
        u.velocita = massima * 0.6;
        u.comportamento = "R_aereo";
    } else {
        u.velocita = std::min(12.0, massima * 0.8);
        u.comportamento = u.non_combattente ? "" : "R_nave";
    }
    s_.unita.push_back(u);
    return static_cast<int>(s_.unita.size()) - 1;
}

void SchermataEditor::elimina_unita(int i) {
    if (i <= 0 || i >= static_cast<int>(s_.unita.size())) return;   // il giocatore resta
    const int id = s_.unita[static_cast<std::size_t>(i)].id;
    s_.unita.erase(s_.unita.begin() + i);
    for (auto& o : s_.obiettivi) o.unita.erase(std::remove(o.unita.begin(), o.unita.end(), id), o.unita.end());
    s_.aerei_asw.erase(std::remove_if(s_.aerei_asw.begin(), s_.aerei_asw.end(), [&](const AereoAswScenario& a) { return a.unita == id; }),
                       s_.aerei_asw.end());
    s_.chiusura.unita.erase(std::remove(s_.chiusura.unita.begin(), s_.chiusura.unita.end(), id), s_.chiusura.unita.end());
    unita_ = -1;
    cambiato();
}

void SchermataEditor::briefing_automatico() {
    static const char* const MESI_IT[] = {"gennaio", "febbraio", "marzo", "aprile", "maggio", "giugno", "luglio", "agosto",
                                          "settembre", "ottobre", "novembre", "dicembre"};
    static const char* const MESI_EN[] = {"January", "February", "March", "April", "May", "June", "July", "August",
                                          "September", "October", "November", "December"};
    char ora[8];
    const int min = static_cast<int>(std::lround(s_.ora_utc * 60.0)) % 1440;
    std::snprintf(ora, sizeof ora, "%02d%02dZ", min / 60, min % 60);
    const std::string teatro = NOMI_TEATRI[indice(TEATRI, s_.teatro)];
    std::map<std::string, int> ostili;
    const std::string mia = s_.alfa ? "urss" : "usa";
    for (const auto& u : s_.unita)
        if (!u.giocatore && !u.nascosta && u.parte != mia && u.parte != "neutrale" && u.entra_con.empty()) ++ostili[u.classe];
    std::string forze_it, forze_en;
    for (const auto& [c, n] : ostili) {
        forze_it += (forze_it.empty() ? "" : ", ") + std::to_string(n) + " " + c;
        forze_en += (forze_en.empty() ? "" : ", ") + std::to_string(n) + " " + c;
    }
    auto& it = s_.briefing[0];
    auto& en = s_.briefing[1];
    it = {"ORDINI DI MISSIONE", "OGG.: " + s_.titolo[0],
          std::to_string(s_.giorno) + " " + MESI_IT[s_.mese - 1] + " " + std::to_string(s_.anno) + ", ore " + ora + ". Teatro: " + teatro + "."};
    en = {"MISSION ORDERS", "SUBJ: " + s_.titolo[1],
          std::to_string(s_.giorno) + " " + MESI_EN[s_.mese - 1] + " " + std::to_string(s_.anno) + ", " + ora + ". Theatre: " + teatro + "."};
    if (!forze_it.empty()) {
        it.push_back("Forze avversarie segnalate: " + forze_it + ".");
        en.push_back("Reported opposing forces: " + forze_en + ".");
    }
    for (const auto& o : s_.obiettivi) {
        if (!o.testo[0].empty()) it.push_back("- " + o.testo[0]);
        if (!o.testo[1].empty()) en.push_back("- " + o.testo[1]);
    }
    static const char* const ROE_IT[] = {"Regole d'ingaggio: tempo di pace. Armi trattenute.", "Regole d'ingaggio: crisi. Aprire il fuoco solo se attaccati.",
                                         "Regole d'ingaggio: ostilita' aperte. Armi libere."};
    static const char* const ROE_EN[] = {"Rules of engagement: peacetime. Weapons hold.", "Rules of engagement: crisis. Fire only if fired upon.",
                                         "Rules of engagement: open hostilities. Weapons free."};
    const int k = indice(SITUAZIONI, s_.situazione);
    it.push_back(ROE_IT[k]);
    en.push_back(ROE_EN[k]);
    it.push_back("Tempo a disposizione: " + ore(s_.durata_s) + ".");
    en.push_back("Time available: " + ore(s_.durata_s) + ".");
}

void SchermataEditor::prossimo_id() {
    if (s_.id != "EDITOR_NUOVA" && !s_.id.empty()) return;
    std::string id = "EDITOR_";
    for (const char c : maiu(s_.titolo[0]))
        if ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) id += c;
        else if (id.back() != '_') id += '_';
    while (id.back() == '_') id.pop_back();
    s_.id = id.size() > 7 ? id : "EDITOR_MISSIONE";
}

bool SchermataEditor::salva() {
    prossimo_id();
    try {
        const std::string testo = scrivi_missione_lua(s_, d_);
        std::filesystem::create_directories(cartella_);
        std::ofstream(cartella_ / (s_.id + ".lua"), std::ios::binary) << testo;
        modificato_ = false;
        messaggio_ = "SALVATA: " + s_.id + ".LUA";
        elementi_validi_ = false;
        return true;
    } catch (const std::exception& ex) {
        messaggio_ = "NON SALVATA: " + maiu(ex.what());
        return false;
    }
}

bool SchermataEditor::esporta() {
    if (!salva()) return false;
    try {
        const auto oggi = std::chrono::year_month_day{std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now())};
        char data[16];
        std::snprintf(data, sizeof data, "%04d-%02u-%02u", static_cast<int>(oggi.year()), static_cast<unsigned>(oggi.month()),
                      static_cast<unsigned>(oggi.day()));
        const std::string testo = esporta_pacchetto(s_, d_, dati_, data);
        std::filesystem::create_directories(esportate_);
        std::ofstream(pacchetto(), std::ios::binary) << testo;
        messaggio_ = "ESPORTATA: " + maiu(pacchetto().filename().string()) + " (MISSIONI_ESPORTATE)";
        return true;
    } catch (const std::exception& ex) {
        messaggio_ = "NON ESPORTATA: " + maiu(ex.what());
        return false;
    }
}

std::string SchermataEditor::simula(double secondi) {
    try {
        const auto f = std::filesystem::temp_directory_path() / (s_.id + "_prova.lua");
        std::ofstream(f, std::ios::binary) << scrivi_missione_lua(s_, d_);
        // coi limiti delle missioni importate (app/partita.cpp): lo scenario puo' venire da un pacchetto di terzi
        Missione m(f.string(), seme(), LimitiScript{10'000'000, std::size_t{64} << 20});
        for (int i = 0; i < static_cast<int>(secondi / 2.0) && !m.esito(); ++i) m.avanza(2.0);
        std::string r = "DOPO " + ore(m.tempo()) + ": " + std::to_string(m.unita().size()) + " UNITA'\n";
        r += std::to_string(m.messaggi().size()) + " MESSAGGI, " + std::to_string(m.colpi().size()) + " COLPI\n";
        if (m.esito()) r += "ESITO " + std::to_string(*m.esito()) + "\n";
        r += m.errori().empty() ? "NESSUN ERRORE DEGLI SCRIPT" : "ERRORE: " + maiu(m.errori().front());
        std::filesystem::remove(f);
        return r;
    } catch (const std::exception& ex) {
        return "NON SI CARICA: " + maiu(ex.what());
    }
}

// --- comandi ---------------------------------------------------------------------------------------------------------------

void SchermataEditor::scheda(Scheda k) {
    scheda_ = k;
    scorri_ = 0;
    elementi_validi_ = false;
}

void SchermataEditor::scegli_unita(int i) {
    unita_ = i;
    elementi_validi_ = false;
}

SchermataEditor::Uscita SchermataEditor::comandi(const Comandi& c, double dt) {
    // un testo in scrittura prende tutto
    if (campo_) {
        campo_->buffer += c.testo;
        if (c.cancella && !campo_->buffer.empty()) {
            while (!campo_->buffer.empty() && (static_cast<unsigned char>(campo_->buffer.back()) & 0xC0) == 0x80) campo_->buffer.pop_back();
            if (!campo_->buffer.empty()) campo_->buffer.pop_back();
        }
        if (c.invio) {
            *campo_->destinazione = campo_->buffer;
            campo_.reset();
            cambiato();
        } else if (c.esc) {
            campo_.reset();
            elementi_validi_ = false;
        }
        return Uscita::nessuna;
    }
    if (c.esc) {
        if (!modificato_ || esc_avvisato_) return Uscita::menu;
        esc_avvisato_ = true;
        messaggio_ = "MODIFICHE NON SALVATE: S SALVA, ESC DI NUOVO ESCE";
    }
    for (const int k : c.cifre)
        if (k >= 1 && k <= 8) scheda(static_cast<Scheda>(k - 1));
    for (const char l : c.lettere) {
        if (l == 'S') salva();
        if (l == 'E') esporta();
        if (l == 'G') {
            bool errori = false;
            for (const auto& a : avvisi_) errori = errori || a.gravita == AvvisoScenario::Gravita::errore;
            if (errori) messaggio_ = "PRIMA CORREGGI GLI ERRORI (SCHEDA PROVA)";
            else if (salva()) return Uscita::gioca;
        }
        if (l == 'X') {
            if (scheda_ == Scheda::unita) elimina_unita(unita_);
            else if (scheda_ == Scheda::rotte && unita_ >= 0 && unita_ < static_cast<int>(s_.unita.size()) &&
                     !s_.unita[static_cast<std::size_t>(unita_)].punti.empty()) {
                s_.unita[static_cast<std::size_t>(unita_)].punti.pop_back();
                cambiato();
            }
        }
    }
    if (c.ingrandisci) mpp_ /= 1.5;
    if (c.riduci) mpp_ *= 1.5;
    if (c.ingrandisci || c.riduci) limita_vista();
    const bool su_pannello = c.mouse.first >= PANNELLO_X;
    if (c.su_giu != 0 || c.ruota != 0 || c.passi_su_giu != 0) {
        if (su_pannello) {
            ripeti_ -= dt;
            if (ripeti_ <= 0.0) {
                scorri_ = std::max(0, scorri_ - verso_freccia(c.su_giu, c.passi_su_giu));
                ripeti_ = 0.08;
            }
        } else {
            const double px = 240.0 * dt * (c.veloce ? 3.0 : 1.0);
            centro_.x += c.ruota * px * mpp_;
            centro_.y += c.su_giu * px * mpp_;
            limita_vista();
        }
    }
    const auto in_carta = [](int x, int y) { return x >= CARTA_X && x < CARTA_X + CARTA_W && y >= CARTA_Y && y < CARTA_Y + CARTA_H; };
    // clic sinistro
    for (const auto& [x, y] : c.clic) {
        bool linguetta = false;   // le linguette delle schede (dall'ultimo disegno)
        for (int k = 0; k < static_cast<int>(linguette_.size()) && k < 8; ++k) {
            const Rettangolo& r = linguette_[static_cast<std::size_t>(k)];
            if (x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h) scheda(static_cast<Scheda>(k)), linguetta = true;
        }
        if (linguetta) continue;
        if (y >= PIEDE_Y + 22 && y < PIEDE_Y + 48) {   // i tasti del piede: ESC, S SALVA, E ESPORTA, G PROVA IN MARE
            const int k = std::clamp((x - 8) / 158, 0, 3);
            Comandi tasto;
            if (k == 0) tasto.esc = true;
            else tasto.lettere.push_back(k == 1 ? 'S' : k == 2 ? 'E' : 'G');
            const Uscita u = comandi(tasto, 0.0);
            if (u != Uscita::nessuna) return u;
            continue;
        }
        if (in_carta(x, y)) {
            const Vec2 q{static_cast<double>(x), static_cast<double>(y)};
            trascina_da_ = q;
            centro_iniziale_ = centro_;
            trascinato_ = false;
            trascina_unita_ = trascina_zona_ = -1;
            if (scheda_ == Scheda::zone && (trascina_zona_ = zona_in(q)) >= 0) zona_ = trascina_zona_;
            else if ((trascina_unita_ = unita_in(q)) >= 0) unita_ = trascina_unita_;
            elementi_validi_ = false;
            continue;
        }
        if (y >= AVVISI_Y && y < AVVISI_Y + 2 * RIGA && x >= PANNELLO_X) {   // gli avvisi sopra la verifica
            const int k = (y - AVVISI_Y) / RIGA;
            if (k >= 0 && k < static_cast<int>(avvisi_.size()) && k < 2) {
                for (std::size_t i = 0; i < s_.unita.size(); ++i)
                    if (s_.unita[i].id == avvisi_[static_cast<std::size_t>(k)].unita) {
                        unita_ = static_cast<int>(i);
                        centra(s_.unita[i].pos);
                    }
                elementi_validi_ = false;
            }
            continue;
        }
        costruisci();
        for (const auto& e : elementi_) {
            const int ry = e.r.y - scorri_ * RIGA;
            if (e.azione && ry >= CONTENUTO_Y && ry < CONTENUTO_FINE && y >= ry && y < ry + e.r.h) {
                const auto azione = e.azione;   // l'azione puo' rifare gli elementi
                azione(+1);
                break;
            }
        }
    }
    // trascinamento
    if (trascina_da_ && c.premuto && c.mouse.first >= 0) {
        const Vec2 m{static_cast<double>(c.mouse.first), static_cast<double>(c.mouse.second)};
        if (std::hypot(m.x - trascina_da_->x, m.y - trascina_da_->y) > 3.0) trascinato_ = true;
        if (trascinato_) {
            const GeometriaCarta g = geometria();
            if (trascina_unita_ >= 0 && trascina_unita_ < static_cast<int>(s_.unita.size()))
                s_.unita[static_cast<std::size_t>(trascina_unita_)].pos = g.a_mondo(m);
            else if (trascina_zona_ >= 0 && trascina_zona_ < static_cast<int>(s_.zone.size())) {
                const Vec2 p = g.a_mondo(m);
                s_.zone[static_cast<std::size_t>(trascina_zona_)].cerchio.x = p.x;
                s_.zone[static_cast<std::size_t>(trascina_zona_)].cerchio.y = p.y;
            } else {
                centro_.x = centro_iniziale_.x - (m.x - trascina_da_->x) * mpp_;
                centro_.y = centro_iniziale_.y + (m.y - trascina_da_->y) * mpp_;
                limita_vista();
            }
        }
    }
    if (trascina_da_ && (!c.rilascio.empty() || !c.premuto)) {
        if (trascinato_ && (trascina_unita_ >= 0 || trascina_zona_ >= 0)) cambiato();
        trascina_da_.reset();
        trascina_unita_ = trascina_zona_ = -1;
    }
    // clic destro
    for (const auto& [x, y] : c.clic_destro) {
        if (in_carta(x, y)) {
            clic_carta({static_cast<double>(x), static_cast<double>(y)}, true);
            continue;
        }
        if (x < PANNELLO_X || y < CONTENUTO_Y || y >= CONTENUTO_FINE) continue;
        costruisci();
        for (const auto& e : elementi_) {
            const int ry = e.r.y - scorri_ * RIGA;
            if (e.azione && y >= ry && y < ry + e.r.h) {
                const auto azione = e.azione;
                azione(-1);
                break;
            }
        }
    }
    return Uscita::nessuna;
}

// --- disegno ---------------------------------------------------------------------------------------------------------------

void SchermataEditor::palette(Palette& p) const {
    applica_palette_carta(p, StileCarta::vga);
    applica_palette_rilievo(p, StileCarta::vga);
    applica_tema(p, TemaStazione{});   // 16-47: la consolle e il fosforo della «SALA PIANI»
}

void SchermataEditor::disegna(Tela& t, const Font& f) const {
    // gruppo 4 della passata estetica (variante C): la consolle «SALA PIANI», la carta nello schermo grande, la scheda
    // nello schermo di destra con le linguette, i campi e la verifica sempre in vista; il piede comune
    namespace q = quadro;
    const Font& g = font_fosforo();
    const auto largo = [&](const std::string& x) { return g.larghezza(x, 1, g.formato() == 2 ? g.spazio_predefinito() : 1, g.formato() != 2); };
    const auto scrivi = [&](int x, int y, const std::string& testo, std::uint8_t col) {
        g.scrivi(t, x, y, testo, col, 1, g.formato() == 2 ? g.spazio_predefinito() : 1, g.formato() != 2);
    };
    const Rettangolo u = q::consolle(t, targa_ ? *targa_ : f, "SALA PIANI - EDITOR DI MISSIONE");
    const std::string teatro = std::string(NOMI_TEATRI[indice(TEATRI, s_.teatro)]) + (s_.alfa ? " - ALFA" : " - 688");
    std::string titolo = maiu(s_.titolo[0]) + (modificato_ ? " *" : "");
    while (!titolo.empty() && largo(titolo) > 392 - 30 - largo(teatro)) titolo.pop_back();
    q::schermo(t, {u.x, u.y, 392, 424 - u.y}, g, titolo, teatro);
    disegna_carta(t, f);
    static const char* const NOMI_LUNGHI[] = {"LA MISSIONE", "LE UNITA'", "LE ROTTE", "LE ZONE", "GLI OBIETTIVI", "LE REGOLE", "IL BRIEFING", "LA PROVA"};
    const Rettangolo a = q::schermo(t, {u.x + 396, u.y, u.w - 396, 424 - u.y}, g, "SCHEDA " + std::to_string(static_cast<int>(scheda_) + 1),
                                    NOMI_LUNGHI[static_cast<int>(scheda_)]);
    // le linguette su due righe (e dove sono, per i clic: come quadro::linguette)
    linguette_.clear();
    for (int riga_l = 0; riga_l < 2; ++riga_l) {
        std::vector<std::string> nomi;
        for (int k = 4 * riga_l; k < 4 * riga_l + 4; ++k) nomi.push_back(std::to_string(k + 1) + " " + NOMI_SCHEDE[k]);
        const int y = SCHEDE_Y + riga_l * SCHEDA_H;
        int x = a.x + 2;
        for (const auto& n : nomi) {
            const int w = largo(n) + 8;
            linguette_.push_back({x, y, w, g.righe() + 3});
            x += w + 4;
        }
        const int k = static_cast<int>(scheda_) - 4 * riga_l;
        q::linguette(t, a, y, g, nomi, k >= 0 && k < 4 ? k : -1);
    }
    // i campi: le intestazioni dei gruppi, l'etichetta e il valore; velate le righe alterne, accesa la riga attiva
    costruisci();
    t.imposta_ritaglio({a.x, CONTENUTO_Y, a.w, CONTENUTO_FINE - CONTENUTO_Y});
    int ultima = 0, k_riga = 0;
    // prima le fasce (velate le righe alterne, accesa la riga attiva), poi i testi: il carattere scende sotto la riga
    for (const auto& e : elementi_) {
        const int y = e.r.y - scorri_ * RIGA;
        if (y + e.r.h < CONTENUTO_Y || y >= CONTENUTO_FINE) continue;
        if (e.colore == colori::CIANO_CHIARO && !e.azione) {
            k_riga = 0;
            continue;
        }
        if (e.acceso || k_riga % 2 == 1) t.rettangolo_pieno(a.x, y, a.w, RIGA, fosforo(e.acceso ? 2 : 1));
        if (e.acceso) t.rettangolo_pieno(a.x, y, 2, RIGA, lampada(ColoreLampada::ambra, true));
        ++k_riga;
    }
    for (const auto& e : elementi_) {
        const int y = e.r.y - scorri_ * RIGA;
        ultima = std::max(ultima, e.r.y + e.r.h);
        if (y + e.r.h < CONTENUTO_Y || y >= CONTENUTO_FINE) continue;
        if (e.colore == colori::CIANO_CHIARO && !e.azione) {   // l'intestazione di un gruppo
            q::intestazione(t, a, y, g, e.testo);
            continue;
        }
        const std::uint8_t col = e.colore == colori::ROSSO          ? std::uint8_t{12}
                                 : e.colore == colori::GIALLO       ? std::uint8_t{colori::GIALLO}
                                 : e.colore == colori::VERDE_CHIARO ? std::uint8_t{colori::VERDE_CHIARO}
                                 : e.colore == colori::CIANO_CHIARO ? fosforo(7)
                                 : e.colore == colori::GRIGIO || !e.azione ? fosforo(4)
                                                                           : fosforo(6);
        const auto due_punti = e.testo.find(": ");
        if (due_punti != std::string::npos && e.azione && e.colore != colori::ROSSO) {   // l'etichetta smorzata, il valore acceso
            const std::string etichetta = e.testo.substr(0, due_punti + 1);
            scrivi(e.r.x + 4, y + 1, etichetta, fosforo(4));
            scrivi(e.r.x + 4 + largo(etichetta + " "), y + 1, e.testo.substr(due_punti + 2), col);
        } else {
            scrivi(e.r.x + 4, y + 1, e.testo, col);
        }
    }
    t.togli_ritaglio();
    if (ultima - scorri_ * RIGA > CONTENUTO_FINE) scrivi(a.x + a.w - 8, CONTENUTO_FINE - 9, "V", fosforo(7));
    if (scorri_ > 0) scrivi(a.x + a.w - 8, CONTENUTO_Y, "^", fosforo(7));
    // gli avvisi (un clic porta all'unita') e la verifica sempre in vista, con la spia
    int errori = 0;
    for (const auto& x : avvisi_) errori += x.gravita == AvvisoScenario::Gravita::errore;
    for (std::size_t i = 0; i < avvisi_.size() && i < 2; ++i) {
        std::string testo = maiu(avvisi_[i].testo);
        while (!testo.empty() && largo(testo) > a.w - 8) testo.pop_back();
        scrivi(a.x + 4, AVVISI_Y + static_cast<int>(i) * RIGA, testo, avvisi_[i].gravita == AvvisoScenario::Gravita::errore ? std::uint8_t{12} : std::uint8_t{colori::GIALLO});
    }
    t.linea_orizzontale(a.x + 2, a.x + a.w - 3, VERIFICA_Y - 3, fosforo(2));
    lamiera::spia_voce(t, {a.x + 3, VERIFICA_Y - 1, 12, 9},
                       errori ? ColoreLampada::rossa : avvisi_.empty() ? ColoreLampada::verde : ColoreLampada::ambra, true);
    scrivi(a.x + 20, VERIFICA_Y, std::to_string(errori) + " ERRORI, " + std::to_string(avvisi_.size() - static_cast<std::size_t>(errori)) + " ATTENZIONI",
           fosforo(6));
    // il campo di testo aperto
    if (campo_) {
        const Rettangolo r{40, 180, 560, 50};
        pannello(t, r, false, true);
        f.scrivi(t, r.x + 8, r.y + 8, campo_->etichetta + " (INVIO CONFERMA, ESC ANNULLA)", colori::CIANO_CHIARO, 1);
        f.scrivi(t, r.x + 8, r.y + 26, taglia(f, maiu(campo_->buffer) + "_", r.w - 16), colori::BIANCO, 1);
    }
    // il piede comune: i messaggi dell'editor al posto della riga d'aiuto
    lamiera::piede(t, f, PIEDE_Y,
                   messaggio_.empty() ? std::string("1-8 SCHEDE  CLIC SCEGLIE  TRASCINA SPOSTA  DESTRO AGGIUNGE  I O SCALA") : taglia(f, messaggio_, 600),
                   {{"ESC  INDIETRO", ColoreLampada::rossa}, {"S  SALVA", ColoreLampada::ambra}, {"E  ESPORTA", ColoreLampada::ambra},
                    {"G  PROVA IN MARE", ColoreLampada::verde}},
                   3);
}

}  // namespace m688
