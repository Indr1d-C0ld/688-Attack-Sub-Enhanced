#include <doctest.h>

#include <algorithm>
#include <cstdlib>
#include <string>

#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/scala_cascata.hpp"
#include "grafica/tela.hpp"
#include "grafica/tema_stazioni.hpp"
#include "nucleo/intercettazione.hpp"
#include "nucleo/transitori_armi.hpp"
#include "pagina_intercettazione.hpp"

using namespace m688;

// La pagina dell'intercettatore della stazione sonar (app/pagina_intercettazione.hpp; docs/22 §2, E): la cascata
// frequenza-tempo, la rosa e la lista delle tracce di un RicevitoreIntercettazione vero, le lampade dell'allarme, i comandi.

namespace {

const CatalogoAcustico& acustica() {
    static const CatalogoAcustico c = carica_acustica(M688_DATI "/acustica.toml");
    return c;
}
const CatalogoIntercettazione& catalogo() {
    static const CatalogoIntercettazione c = carica_intercettazione(M688_DATI "/acustica.toml");
    return c;
}
Ambiente calmo() {
    Ambiente a;
    a.fondale_m = 3000.0;
    a.vento_m_s = 4.0;
    a.traffico = 0.4;
    return a;
}
Ricevitore il_688() { return Ricevitore{{0.0, 0.0}, 150.0, 0.0, 5.0, acustica().sensore("wlr9")}; }
const Rettangolo AREA{0, 0, LARGHEZZA_TELA, ALTEZZA_VISTA};

// una fregata a 045, 20 km, che pinga ogni 25 s col SQS-53 (FM di 1,5 s a 3,3 kHz), per `n` impulsi
double fregata(RicevitoreIntercettazione& ric, int n) {
    double t = 0.0;
    for (int k = 0; k < n; ++k) {
        EmissioneAttiva x;
        x.emettitore = 3;
        x.chiave = "sqs53_attivo";
        x.pos = {14000.0, 14000.0};
        x.quota_m = 7.0;
        x.t = t = 25.0 * k;
        x.sl_db = 235.0;
        ric.ascolta({x}, il_688(), calmo(), x.t, 25.0);
    }
    return t;
}

// una testa che arriva da 4 km a 45 kn coi ping alla cadenza della missione, fino all'acquisizione
double testa(RicevitoreIntercettazione& ric) {
    double prossimo = 0.0, t = 0.0;
    for (; t < 400.0 && ric.allarme() != Allarme::siluro_in_acquisizione; t += 0.1) {
        const double d = 4000.0 - 45.0 * NODO * t;
        if (d < 200.0) break;
        std::vector<EmissioneAttiva> e;
        if (t >= prossimo) {
            EmissioneAttiva x;
            x.emettitore = 7;
            x.chiave = "ping_testa_mk48";
            x.pos = {0.0, d};
            x.quota_m = 150.0;
            x.rotta = 180.0;
            x.velocita_kn = 45.0;
            x.t = t;
            x.sl_db = 210.0;
            e.push_back(x);
            prossimo = t + intervallo_ping_testa_s(d, 1500.0);
        }
        ric.ascolta(e, il_688(), calmo(), t, 0.1);
    }
    return t;
}

bool acceso(const Tela& t, Rettangolo r) {   // c'e' qualcosa che non e' il nero del fondo?
    for (int y = r.y; y < r.y + r.h; ++y)
        for (int x = r.x; x < r.x + r.w; ++x)
            if (t.at(x, y) != 0) return true;
    return false;
}
bool con_colore(const Tela& t, Rettangolo r, std::uint8_t c) {
    for (int y = r.y; y < r.y + r.h; ++y)
        for (int x = r.x; x < r.x + r.w; ++x)
            if (t.at(x, y) == c) return true;
    return false;
}

}  // namespace

TEST_CASE("pagina intercettazione: la fregata sulla cascata in B1, la riga della traccia, la scelta e la finestra") {
    RicevitoreIntercettazione ric(*catalogo().intercettatore_di("los_angeles"), catalogo(), 1);
    const double ora = fregata(ric, 4) + 1.0;
    REQUIRE(ric.tracce().size() == 1);
    const TracciaIntercetto& tr = ric.tracce().front();
    // la riga a fosforo: rilevamento, frequenza, durata, cadenza, classe; il WLR-9 non distingue CW e FM
    const std::string s = PaginaIntercettazione::riga(tr);
    MESSAGE(s);
    CHECK(s.rfind("BRG 0", 0) == 0);
    CHECK(s.find("3,3 KHZ") != std::string::npos);
    CHECK(s.find("PRI 25 S") != std::string::npos);
    CHECK(s.find("SCAFO") != std::string::npos);
    CHECK(s.find(" --  SCAFO") != std::string::npos);
    CHECK(PaginaIntercettazione::viva(tr, ora));
    CHECK_FALSE(PaginaIntercettazione::viva(tr, ora + 600.0));

    PaginaIntercettazione p;
    CHECK(p.traccia_scelta() == -1);
    p.su_giu(ric, +1);
    CHECK(p.traccia_scelta() == tr.id);
    p.su_giu(ric, +1);   // una sola traccia: resta li'
    CHECK(p.traccia_scelta() == tr.id);
    CHECK(p.finestra_min() == 5);
    p.finestra(-1);
    CHECK(p.finestra_min() == 2);
    p.finestra(-1);
    CHECK(p.finestra_min() == 2);
    for (int i = 0; i < 5; ++i) p.finestra(+1);
    CHECK(p.finestra_min() == 30);
    p.finestra(-1);
    p.finestra(-1);
    REQUIRE(p.finestra_min() == 5);

    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    Tela t;
    std::vector<AreaAttiva> aree;
    p.disegna(t, f, AREA, &ric, ora, {}, &aree);
    // ogni intercetto e' una tacca alla sua frequenza e al suo istante, nella banda B1 (1-6 kHz)
    const Rettangolo c = PaginaIntercettazione::cascata(AREA);
    for (const Intercetto& x : ric.storia()) {
        const int px = static_cast<int>(p.x_tempo(c, x.t, ora)), py = static_cast<int>(PaginaIntercettazione::y_frequenza(c, x.f_hz));
        CHECK(acceso(t, {px - 2, py - 4, 8, 8}));
        CHECK(py > PaginaIntercettazione::y_frequenza(c, 6000.0));
        CHECK(py < PaginaIntercettazione::y_frequenza(c, 1000.0));
    }
    // fra 15 e 45 kHz (B3) non c'e' niente, oltre al nome della banda sul bordo sinistro
    const int y3 = static_cast<int>(PaginaIntercettazione::y_frequenza(c, 40000.0)), y3b = static_cast<int>(PaginaIntercettazione::y_frequenza(c, 17000.0));
    CHECK_FALSE(acceso(t, {c.x + 20, y3 + 2, c.w - 22, y3b - y3 - 4}));
    // la traccia scelta e' cerchiata di bianco
    const Intercetto& ultimo = ric.storia().back();
    const int ux = static_cast<int>(p.x_tempo(c, ultimo.t, ora)), uy = static_cast<int>(PaginaIntercettazione::y_frequenza(c, ultimo.f_hz));
    CHECK(con_colore(t, {ux - 4, uy - 5, 10, 10}, colori::BIANCO));
    // nessun allarme: le lampade spente
    CHECK_FALSE(con_colore(t, {6, 5, 26, 10}, lampada(ColoreLampada::ambra, true)));
    CHECK_FALSE(con_colore(t, {36, 5, 26, 10}, lampada(ColoreLampada::rossa, true)));
    // le aree: una riga per traccia e i due tasti della finestra
    CHECK(std::count_if(aree.begin(), aree.end(), [](const AreaAttiva& a) { return a.comando == "intercetto_riga_0"; }) == 1);
    CHECK(std::count_if(aree.begin(), aree.end(), [](const AreaAttiva& a) { return a.comando.rfind("intercetto_finestra_", 0) == 0; }) == 2);
    // fuori dalla finestra di 2 minuti i primi impulsi non si vedono piu'
    p.finestra(-1);
    Tela t2;
    p.disegna(t2, f, AREA, &ric, ora + 60.0, {});
    const Intercetto& primo = ric.storia().front();
    CHECK(p.x_tempo(c, primo.t, ora + 60.0) < c.x);
}

TEST_CASE("pagina intercettazione: la testa in acquisizione accende la lampada rossa e colora la sua riga") {
    RicevitoreIntercettazione ric(*catalogo().intercettatore_di("los_angeles"), catalogo(), 2);
    const double ora = testa(ric);
    REQUIRE(ric.allarme() == Allarme::siluro_in_acquisizione);
    PaginaIntercettazione p;
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    Tela t;
    StileIntercettazione s;
    s.modo_1989 = true;
    s.fosforo = true;
    p.disegna(t, f, AREA, &ric, ora, s);
    CHECK(con_colore(t, {6, 5, 26, 10}, lampada(ColoreLampada::ambra, true)));
    CHECK(con_colore(t, {36, 5, 26, 10}, lampada(ColoreLampada::rossa, true)));
    // la riga della testa in rosso (EGA 12), nella lista
    const int y_lista = PaginaIntercettazione::ALTO_ALLARME + PaginaIntercettazione::ALTO_CENTRO + 4;
    CHECK(con_colore(t, {0, y_lista + PaginaIntercettazione::RIGA_LISTA, LARGHEZZA_TELA, PaginaIntercettazione::RIGA_LISTA}, 12));
    // la testa sta in B3 (15-45 kHz): il fosforo acceso li'
    const Rettangolo c = PaginaIntercettazione::cascata(AREA);
    const int y = static_cast<int>(PaginaIntercettazione::y_frequenza(c, 30000.0));
    CHECK(acceso(t, {c.x + c.w - 30, y - 6, 30, 12}));
    if (const char* d = std::getenv("M688_ISTANTANEE")) {
        Palette pal = Palette::carica(M688_DATI "/palette/giorno.pal");
        cascate::applica(pal, cascate::Tinta::fosforo);
        applica_tema(pal, {BattelloTema::usa688, LuceSala::bianca, Fosforo::del_battello});
        t.salva_bmp(std::string(d) + "/pagina_intercettazione.bmp", pal);
    }
}

TEST_CASE("pagina intercettazione: il Rubin non misura durata ne' modulazione; senza ricevitore la pagina lo dice") {
    RicevitoreIntercettazione rubin(catalogo().intercettatori.at("rubin"), catalogo(), 5);
    EmissioneAttiva x;
    x.emettitore = 4;
    x.chiave = "sqs53_attivo";
    x.pos = {0.0, 15000.0};
    x.quota_m = 7.0;
    x.sl_db = 235.0;
    const Ricevitore r{{0.0, 0.0}, 150.0, 0.0, 5.0, acustica().sensore("intercettazione_sov")};
    for (int k = 0; k < 3; ++k) {
        x.t = 25.0 * k;
        rubin.ascolta({x}, r, calmo(), x.t, 25.0);
    }
    REQUIRE_FALSE(rubin.tracce().empty());
    const std::string s = PaginaIntercettazione::riga(rubin.tracce().front());
    MESSAGE(s);
    CHECK(s.find("KHZ  --  PRI") != std::string::npos);   // la durata
    CHECK(s.find("DB  --  ") != std::string::npos);        // CW o FM
    PaginaIntercettazione p;
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    Tela t;
    p.disegna(t, f, AREA, nullptr, 0.0, {});
    CHECK(acceso(t, {70, 4, 300, 12}));                    // la scritta
    CHECK_FALSE(acceso(t, PaginaIntercettazione::cascata(AREA)));
}
