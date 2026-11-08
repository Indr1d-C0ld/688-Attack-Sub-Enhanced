#include <doctest.h>

#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>

#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/simboli_carta.hpp"
#include "grafica/tela.hpp"
#include "grafica/tema_stazioni.hpp"
#include "missioni/contatti_giocatore.hpp"
#include "missioni/danni_missione.hpp"
#include "missioni/missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/geometria.hpp"
#include "stazione_missili.hpp"

using namespace m688;
namespace fs = std::filesystem;

// La pagina di tiro dei missili di F7 (app/stazione_missili.hpp; docs/21 §3-§4): i comandi che diventano gli ordini
// imposta_missile e prepara_missile, il coperchio del FIRE, la carta di tiro con la scatola e i contatti nell'area, le
// spie della WLC e i colori degli avvisi, sulla scena PROVA_M1 (data/missioni/prove/missili).

namespace {

const std::string M1 = M688_DATI "/../missioni/prove/PROVA_M1_ANTINAVE.lua";

bool c_e() { return fs::exists(M688_DATI "/missili.toml") && fs::exists(M1); }

struct Partita {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    explicit Partita(const std::string& file, std::uint64_t seme = 1) : m(file, seme, false), dm(classi, M688_DATI, seme, RegolaDanni::simulazione) {}
    void avanza(double s) { for (long i = 0; i < static_cast<long>(s * 9.0); ++i) { m.avanza(1.0 / 9.0); dm.avanza(m, 1.0 / 9.0); } }
    void applica(const std::vector<Ordine>& v) { for (const auto& o : v) m.applica(o); }
    // aspetta il contatto sull'unita' (al piu' `s` secondi)
    bool aspetta_contatto(int id, double s) {
        for (double t = 0.0; t < s; t += 10.0) {
            if (m.giocatore()->contatti.count(id)) return true;
            avanza(10.0);
        }
        return m.giocatore()->contatti.count(id) > 0;
    }
};

// i tubi della WLC come li passa l'interfaccia (il carico della M1: 1, 2 e 7 Harpoon)
ContestoMissili contesto(int tubo, bool porta_1 = false) {
    ContestoMissili k;
    k.tubo = tubo;
    k.tubi = {{1, "UGM-84 HARPOON", porta_1}, {2, "UGM-84 HARPOON", false}, {3, "MK48", false}, {4, "MK48", false},
              {7, "UGM-84 HARPOON", false}};
    return k;
}

bool ordine(const Ordine& o, TipoOrdine tipo, int tubo, double a, double b = 0.0) {
    return o.tipo == tipo && o.n == tubo && std::abs(o.a - a) < 1e-6 && std::abs(o.b - b) < 1e-6;
}
double campo(CampoMissile c) { return static_cast<double>(c); }

// c'e' il colore entro `r` pixel dal punto?
bool vicino(const Tela& t, Vec2 q, std::uint8_t colore, int r = 1) {
    for (int dy = -r; dy <= r; ++dy)
        for (int dx = -r; dx <= r; ++dx) {
            const int x = static_cast<int>(std::lround(q.x)) + dx, y = static_cast<int>(std::lround(q.y)) + dy;
            if (x >= 0 && y >= 0 && x < t.larghezza() && y < t.altezza() && t.at(x, y) == colore) return true;
        }
    return false;
}
bool nel_rettangolo(const Tela& t, Rettangolo r, std::uint8_t colore) {
    for (int y = r.y; y < r.y + r.h; ++y)
        for (int x = r.x; x < r.x + r.w; ++x)
            if (t.at(x, y) == colore) return true;
    return false;
}
// la spia `i` (PWR, ALN, RDY, DOOR, SLIDE) della riga `n` della WLC (come in stazione_missili.cpp)
Rettangolo spia(int riga, int i) {
    using S = StazioneMissili;
    return {S::WLC_X + i * S::WLC_PASSO, ALTEZZA_VISTA + S::WLC_Y + S::WLC_RIGA * riga, S::WLC_W, S::WLC_H};
}

}  // namespace

TEST_CASE("stazione missili: i comandi della consolle diventano gli ordini giusti" * doctest::skip(!c_e())) {
    Partita p(M1);
    p.avanza(1.0);
    REQUIRE(p.m.con_missili());
    StazioneMissili s;
    using R = StazioneMissili::Riga;
    CHECK(s.righe(p.m, 3).empty());   // un Mk 48: niente pagina
    CHECK(s.righe(p.m, 1) == std::vector<R>{R::fonte, R::modo, R::ricerca, R::preparazione});
    REQUIRE(s.riga(p.m, 1) == R::fonte);
    const double t = p.m.tempo();
    // FONTE a giro: TMA -> periscopio, e col periscopio compare la DISTANZA
    auto o = s.cambia(p.m, 1, +1, t);
    REQUIRE(o.size() == 1);
    CHECK(ordine(o[0], TipoOrdine::imposta_missile, 1, 1.0, campo(CampoMissile::fonte)));
    CHECK(o[0].tempo == t);
    o = s.cambia(p.m, 1, -1, t);
    CHECK(ordine(o[0], TipoOrdine::imposta_missile, 1, 3.0, campo(CampoMissile::fonte)));   // all'indietro: la radio
    p.applica(s.cambia(p.m, 1, +1, t));
    CHECK(p.m.tiro_missile(1)->fonte == FonteDati::periscopio);
    CHECK(s.righe(p.m, 1) == std::vector<R>{R::fonte, R::distanza, R::modo, R::ricerca, R::preparazione});
    s.su_giu(p.m, 1, +1);
    REQUIRE(s.riga(p.m, 1) == R::distanza);
    o = s.cambia(p.m, 1, +1, t);   // senza stadimetro si parte da 5 km
    CHECK(ordine(o[0], TipoOrdine::imposta_missile, 1, 5500.0, campo(CampoMissile::distanza_m)));
    // MODO: RBL -> BOL, e al posto della RICERCA vengono SRCH e DSTR
    s.su_giu(p.m, 1, +1);
    REQUIRE(s.riga(p.m, 1) == R::modo);
    o = s.cambia(p.m, 1, +1, t);
    CHECK(ordine(o[0], TipoOrdine::imposta_missile, 1, 1.0, campo(CampoMissile::modo)));
    p.applica(o);
    CHECK(s.righe(p.m, 1) == std::vector<R>{R::fonte, R::distanza, R::modo, R::srch, R::dstr, R::preparazione});
    s.su_giu(p.m, 1, +1);
    REQUIRE(s.riga(p.m, 1) == R::srch);
    o = s.cambia(p.m, 1, +1, t);   // dalla predefinita (senza bersaglio nessuna accensione): un miglio
    CHECK(ordine(o[0], TipoOrdine::imposta_missile, 1, MIGLIO, campo(CampoMissile::srch_m)));
    p.applica(o);
    CHECK(s.cambia(p.m, 1, -1, t)[0].a == 0.0);   // sotto il miglio torna la predefinita
    s.su_giu(p.m, 1, +1);
    REQUIRE(s.riga(p.m, 1) == R::dstr);
    const double portata = p.m.tiro_missile(1)->dati->portata_m;
    o = s.cambia(p.m, 1, -1, t);   // dalla portata un miglio in meno
    CHECK(ordine(o[0], TipoOrdine::imposta_missile, 1, (std::round(portata / MIGLIO) - 1.0) * MIGLIO, campo(CampoMissile::dstr_m)));
    CHECK(o[0].a < portata);
    CHECK(s.cambia(p.m, 1, +1, t).empty());   // gia' fino alla portata
    // PREPARAZIONE: destra avvia, sinistra spegne; giu' oltre l'ultima riga resta li'
    s.su_giu(p.m, 1, +1);
    s.su_giu(p.m, 1, +1);
    REQUIRE(s.riga(p.m, 1) == R::preparazione);
    CHECK(ordine(s.cambia(p.m, 1, +1, t)[0], TipoOrdine::prepara_missile, 1, 1.0));
    CHECK(ordine(s.cambia(p.m, 1, -1, t)[0], TipoOrdine::prepara_missile, 1, 0.0));
    // di nuovo RBL: la RICERCA si ferma ai capi
    p.m.applica({t, TipoOrdine::imposta_missile, 0.0, campo(CampoMissile::modo), 1});
    s.cambio_tubo();
    s.su_giu(p.m, 1, +1);
    s.su_giu(p.m, 1, +1);
    s.su_giu(p.m, 1, +1);
    REQUIRE(s.riga(p.m, 1) == R::ricerca);
    o = s.cambia(p.m, 1, +1, t);
    CHECK(ordine(o[0], TipoOrdine::imposta_missile, 1, 2.0, campo(CampoMissile::taglia)));
    p.applica(o);
    CHECK(s.cambia(p.m, 1, +1, t).empty());
    CHECK(s.cambia(p.m, 1, 0, t).empty());
    // la soluzione del MATE: cinque ordini, un valore negativo passa com'e' (la missione dimentica il campo)
    o = StazioneMissili::ordini_soluzione(t, 2, 38000.0, 210.0, 15.0, 1500.0, -1.0);
    REQUIRE(o.size() == 5);
    CHECK(ordine(o[0], TipoOrdine::imposta_missile, 2, 38000.0, campo(CampoMissile::distanza_m)));
    CHECK(ordine(o[1], TipoOrdine::imposta_missile, 2, 210.0, campo(CampoMissile::rotta)));
    CHECK(ordine(o[2], TipoOrdine::imposta_missile, 2, 15.0, campo(CampoMissile::velocita_kn)));
    CHECK(ordine(o[3], TipoOrdine::imposta_missile, 2, 1500.0, campo(CampoMissile::sigma_distanza_m)));
    CHECK(ordine(o[4], TipoOrdine::imposta_missile, 2, -1.0, campo(CampoMissile::sigma_trasversale_m)));
}

TEST_CASE("stazione missili: il coperchio del FIRE, lo zoom e il centro della carta" * doctest::skip(!c_e())) {
    Partita p(M1);
    p.avanza(1.0);
    StazioneMissili s;
    CHECK(s.rifiuto_lancio() == std::string("COPERCHIO CHIUSO"));
    s.coperchio();
    CHECK(s.coperchio_alzato());
    CHECK_FALSE(s.rifiuto_lancio().has_value());
    s.lanciato();
    CHECK_FALSE(s.coperchio_alzato());
    s.coperchio();
    s.su_giu(p.m, 1, +1);
    s.cambio_tubo();
    CHECK_FALSE(s.coperchio_alzato());
    CHECK(s.riga(p.m, 1) == StazioneMissili::Riga::fonte);
    // la scala: automatica, poi I e O a passi fissi dentro i limiti
    const VistaCarta a = s.vista(p.m, 1);
    CHECK(a.h == StazioneMissili::ALTEZZA_CARTA);
    // senza bersaglio la scala di partenza, non la piu' stretta
    CHECK(a.metri_per_pixel == doctest::Approx(StazioneMissili::SCALE_KM[StazioneMissili::SCALA_PARTENZA] * 1000.0 / LARGHEZZA_TELA));
    s.zoom(-1);
    const VistaCarta b = s.vista(p.m, 1);
    CHECK(b.metri_per_pixel > a.metri_per_pixel);
    s.zoom(+1);
    CHECK(s.vista(p.m, 1).metri_per_pixel == doctest::Approx(a.metri_per_pixel));
    for (int i = 0; i < 10; ++i) s.zoom(+1);
    CHECK(s.vista(p.m, 1).metri_per_pixel == doctest::Approx(StazioneMissili::SCALE_KM[0] * 1000.0 / LARGHEZZA_TELA));
    for (int i = 0; i < 10; ++i) s.zoom(-1);
    CHECK(s.vista(p.m, 1).metri_per_pixel == doctest::Approx(200.0 * 1000.0 / LARGHEZZA_TELA));
    s.centra();   // sul bersaglio (senza: sul battello), poi sull'area di lancio
    s.centra();
    const auto* g = p.m.giocatore();
    const Vec2 io = p.m.con_navigazione() ? p.m.posizione_stimata(g->id) : g->pos;
    CHECK(distanza(s.vista(p.m, 1).centro, io) < 1.0);
}

TEST_CASE("stazione missili: la carta di tiro e il pannello sulla scena M1" * doctest::skip(!c_e())) {
    Partita p(M1);
    p.avanza(1.0);
    REQUIRE(p.aspetta_contatto(2, 1800.0));   // lo Slava al sonar
    p.m.applica({p.m.tempo(), TipoOrdine::designa_bersaglio, 0, 0, 2});
    // il MATE «perfetto» sullo Slava, come lo manderebbe l'interfaccia
    const auto* g = p.m.giocatore();
    const auto& slava = p.m.unita().at(2);
    p.applica(StazioneMissili::ordini_soluzione(p.m.tempo(), 1, distanza(g->pos, slava.pos), 210.0, 15.0, 1500.0, -1.0));
    const auto tiro = p.m.tiro_missile(1);
    REQUIRE(tiro.has_value());
    REQUIRE(tiro->bersaglio.has_value());
    CHECK(tiro->piano.valido);
    REQUIRE(tiro->piano.area.forma == AreaRicerca::Forma::scatola);
    REQUIRE(tiro->piano.area.contorno.size() == 4);
    MESSAGE("contatti nell'area: ", tiro->piano.in_area.size(), ", P successo ", tiro->piano.p_successo);

    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    CartaGiocatore carta;
    const auto segni = segni_contatti(p.m, false);
    StazioneMissili s;
    ContestoMissili k = contesto(1, true);
    k.bersaglio = 2;
    Tela t;
    s.disegna_carta(t, f, p.m, carta, segni, k);
    // il contorno della scatola: il magenta a un quarto di ogni lato (a meta' del lato vicino passa la rotta)
    const VistaCarta v = s.vista(p.m, 1);
    const auto& c = tiro->piano.area.contorno;
    for (std::size_t i = 0; i < c.size(); ++i) {
        const Vec2 m{(3.0 * c[i].x + c[(i + 1) % c.size()].x) / 4.0, (3.0 * c[i].y + c[(i + 1) % c.size()].y) / 4.0};
        const Vec2 q = v.a_tela(m);
        REQUIRE(q.x >= 0.0);
        REQUIRE(q.y >= 0.0);
        REQUIRE(q.x < LARGHEZZA_TELA);
        REQUIRE(q.y < StazioneMissili::ALTEZZA_CARTA);
        CHECK(vicino(t, q, colori_carta::MAGENTA));
    }
    // il punto previsto: la croce gialla
    CHECK(vicino(t, v.a_tela(tiro->piano.previsto), static_cast<std::uint8_t>(colori_carta::MATITA + 4), 3));
    // la carta non esce dalle sue righe
    for (int x = 0; x < LARGHEZZA_TELA; ++x) CHECK(t.at(x, StazioneMissili::ALTEZZA_CARTA + 1) == 0);

    // il pannello: le spie della WLC per i tubi 1, 2 e 7 (riga 0, 1, 2); la porta solo sull'1
    std::vector<AreaAttiva> aree;
    s.disegna_pannello(t, f, p.m, k, &aree);
    const auto verde_acceso = lampada(ColoreLampada::verde, true), ambra_accesa = lampada(ColoreLampada::ambra, true);
    CHECK(nel_rettangolo(t, spia(0, 0), verde_acceso));   // PWR: il missile della partenza e' pronto
    CHECK(nel_rettangolo(t, spia(0, 1), verde_acceso));   // ALN
    CHECK(nel_rettangolo(t, spia(0, 2), verde_acceso));   // RDY: piano valido e allineato
    CHECK(nel_rettangolo(t, spia(0, 3), ambra_accesa));   // DOOR aperta sul tubo 1
    CHECK_FALSE(nel_rettangolo(t, spia(1, 3), ambra_accesa));
    CHECK(nel_rettangolo(t, spia(0, 4), verde_acceso));   // SLIDE libera
    CHECK_FALSE(nel_rettangolo(t, spia(3, 0), verde_acceso));   // solo tre tubi col missile
    // gli avvisi: dati validi in verde, nessun bloccante
    const Rettangolo riga_avvisi{8, ALTEZZA_VISTA + 14 + 92 - 9, 404, 9};
    CHECK(nel_rettangolo(t, riga_avvisi, colori::VERDE_CHIARO));
    CHECK_FALSE(nel_rettangolo(t, riga_avvisi, 12));
    // le aree: una per riga che si sceglie, e il FIRE
    const auto n_righe = s.righe(p.m, 1).size();
    CHECK(std::count_if(aree.begin(), aree.end(), [](const AreaAttiva& a) { return a.comando.rfind("missili_riga_", 0) == 0; }) ==
          static_cast<long>(n_righe));
    CHECK(std::count_if(aree.begin(), aree.end(), [](const AreaAttiva& a) { return a.comando == "missili_fuoco"; }) == 1);

    // a quota periscopica l'Harpoon e' sotto la quota minima: l'avviso in ambra, non bloccante
    p.m.applica({p.m.tempo(), TipoOrdine::quota_periscopica});
    p.avanza(180.0);
    const auto basso = p.m.tiro_missile(1);
    REQUIRE(basso.has_value());
    REQUIRE(std::find(basso->piano.avvisi.begin(), basso->piano.avvisi.end(), Avviso::quota_bassa) != basso->piano.avvisi.end());
    s.disegna_pannello(t, f, p.m, k);
    if (basso->piano.valido) {
        CHECK(nel_rettangolo(t, riga_avvisi, colori::GIALLO));
        CHECK_FALSE(nel_rettangolo(t, riga_avvisi, 12));
    }
    // fuori portata: il rosso dei bloccanti e RDY spento
    p.applica(StazioneMissili::ordini_soluzione(p.m.tempo(), 1, 300000.0, 210.0, 15.0, 1500.0, -1.0));
    const auto lontano = p.m.tiro_missile(1);
    REQUIRE_FALSE(lontano->piano.valido);
    s.disegna_pannello(t, f, p.m, k);
    CHECK(nel_rettangolo(t, riga_avvisi, 12));
    CHECK_FALSE(nel_rettangolo(t, spia(0, 2), verde_acceso));
}

TEST_CASE("stazione missili: i contatti nell'area e i missili in volo come STIMA" * doctest::skip(!c_e())) {
    // M1 con l'elettronica dell'Enhanced: in superficie col radar acceso i contatti hanno la posizione
    // (il passivo da' solo il rilevamento), e il cargo neutrale vicino allo Slava cade nella scatola
    Partita p(M1);
    p.m.acustica_enhanced(M688_DATI);
    // lo Slava trattiene le armi sui sottomarini: col 688 in superficie in guerra lo affonderebbe col P-500 (I11)
    Missione::RegolazioniParte trattenuta;
    trattenuta.roe = std::map<CategoriaRoe, bool>{{CategoriaRoe::sottomarini, false}};
    p.m.regolazioni({{0x10, trattenuta}});
    p.avanza(1.0);
    REQUIRE(p.m.con_missili());
    p.m.applica({p.m.tempo(), TipoOrdine::quota, 0.0, 1.0});
    p.avanza(240.0);
    const bool antenna = p.m.applica({p.m.tempo(), TipoOrdine::antenna, 0, 0, 1});
    const bool radar = p.m.applica({p.m.tempo(), TipoOrdine::radar, 0, 0, 1});
    REQUIRE(antenna);
    REQUIRE(radar);
    const auto con_posizione = [&](int id) {
        const auto& c = p.m.giocatore()->contatti;
        return c.count(id) && c.at(id).posizione_nota;
    };
    for (int i = 0; i < 180 && !con_posizione(2); ++i) p.avanza(10.0);
    REQUIRE(con_posizione(2));
    p.m.applica({p.m.tempo(), TipoOrdine::designa_bersaglio, 0, 0, 2});
    const auto* g = p.m.giocatore();
    const auto& slava = p.m.unita().at(2);
    p.applica(StazioneMissili::ordini_soluzione(p.m.tempo(), 1, distanza(g->pos, slava.pos), 210.0, 15.0, 1500.0, -1.0));
    const auto tiro = p.m.tiro_missile(1);
    REQUIRE(tiro.has_value());
    REQUIRE(tiro->piano.valido);   // in superficie l'Harpoon e' sotto la quota minima: solo l'avviso
    MESSAGE("contatti nell'area: ", tiro->piano.in_area.size());
    CHECK_FALSE(tiro->piano.in_area.empty());

    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    CartaGiocatore carta;
    const auto segni = segni_contatti(p.m, false);
    StazioneMissili s;
    ContestoMissili k = contesto(1);
    k.bersaglio = 2;
    Tela t;
    s.disegna_carta(t, f, p.m, carta, segni, k);
    const VistaCarta v = s.vista(p.m, 1);
    // ogni contatto nell'area ha la cornice in EVIDENZA attorno al suo segno
    std::size_t incorniciati = 0;
    for (const auto& x : segni_su_carta(v, p.m, segni))
        if (std::find(tiro->piano.in_area.begin(), tiro->piano.in_area.end(), x.segno.id) != tiro->piano.in_area.end()) {
            CHECK(vicino(t, {x.q.x - 8.0, x.q.y}, simboli::EVIDENZA));
            ++incorniciati;
        }
    CHECK(incorniciati == tiro->piano.in_area.size());
    s.disegna_pannello(t, f, p.m, k);
    if (const char* d = std::getenv("M688_ISTANTANEE")) {
        Palette pal = Palette::carica(M688_DATI "/palette/giorno.pal");
        applica_palette_carta(pal, StileCarta::vga);
        applica_tema(pal, {BattelloTema::usa688, LuceSala::bianca, Fosforo::del_battello});
        t.salva_bmp(std::string(d) + "/stazione_missili.bmp", pal);
    }

    // dopo il lancio il missile in volo e' la stima del tempo di volo, marcata STIMA in bianco
    REQUIRE(p.m.applica({p.m.tempo(), TipoOrdine::lancia, 0, 0, 1}));
    s.lanciato();
    p.avanza(20.0);
    const auto volo = p.m.missili_in_volo();
    REQUIRE(volo.size() == 1);
    Tela t2;
    s.disegna_carta(t2, f, p.m, carta, segni_contatti(p.m, false), k);
    const VistaCarta v2 = s.vista(p.m, 1);
    const Vec2 q = v2.a_tela(volo[0].stima.pos);
    CHECK(vicino(t2, q, static_cast<std::uint8_t>(colori_carta::MATITA + 5), 3));
    CHECK(vicino(t2, {q.x + 12.0, q.y + 1.0}, static_cast<std::uint8_t>(colori_carta::MATITA + 5), 6));   // la scritta
}

TEST_CASE("stazione missili: il TLAM della M2, la missione dalla libreria, l'ordine dalla radio, l'area di lancio" *
          doctest::skip(!fs::exists(M688_DATI "/../missioni/prove/PROVA_M2_TLAM.lua"))) {
    Partita p(M688_DATI "/../missioni/prove/PROVA_M2_TLAM.lua");
    p.m.radio_enhanced();   // la sala radio: l'ordine d'esecuzione arriva col broadcast
    p.avanza(1.0);
    REQUIRE(p.m.con_missili());
    REQUIRE(p.m.libreria_tlam().size() == 2);
    StazioneMissili s;
    using R = StazioneMissili::Riga;
    REQUIRE(s.righe(p.m, 1) == std::vector<R>{R::missione, R::preparazione});
    // MISSIONE a giro sulla libreria: la prima, poi la seconda, e indietro di nuovo la prima
    auto o = s.cambia(p.m, 1, +1, p.m.tempo());
    REQUIRE(o.size() == 1);
    CHECK(ordine(o[0], TipoOrdine::imposta_missile, 1, 0.0, campo(CampoMissile::missione_tlam)));
    p.applica(o);
    auto tiro = p.m.tiro_missile(1);
    REQUIRE(tiro->tlam);
    CHECK(tiro->tlam->id == "0417");
    o = s.cambia(p.m, 1, +1, p.m.tempo());
    CHECK(o[0].a == 1.0);
    o = s.cambia(p.m, 1, -1, p.m.tempo());
    CHECK(o[0].a == 1.0);   // da 0 indietro: l'ultima
    // il battello parte a ~22 nm dal PLP della 0417, fra VALIDA e RIPIANIFICA; lanciando adesso il missile arriverebbe
    // prima dell'ora sul bersaglio: FUORI FINESTRA. L'ordine arriva col messaggio dei 30 minuti, con l'antenna fuori
    REQUIRE(tiro->stato_tlam.has_value());
    CHECK(tiro->stato_tlam->distanza_plp_m > TLAM_VALIDA_M);
    CHECK(tiro->stato_tlam->distanza_plp_m < TLAM_RIPIANIFICA_M);
    CHECK(tiro->stato_tlam->validita == ValiditaTlam::fuori_finestra);
    CHECK_FALSE(tiro->ordine_esecuzione);
    CHECK_FALSE(tiro->rdy);
    // a quota periscopica con l'antenna (nell'Enhanced 5,5 m, e gli alberi restano su fino a 7,1 m: runtime, 28/10)
    p.m.applica({p.m.tempo(), TipoOrdine::quota_periscopica});
    for (int i = 0; i < 90 && p.m.giocatore()->quota > 7.0; ++i) p.avanza(10.0);
    REQUIRE(p.m.applica({p.m.tempo(), TipoOrdine::antenna, 0, 0, 1}));
    p.avanza(std::max(0.0, 1900.0 - p.m.tempo()));
    CHECK(p.m.giocatore()->quota < 7.1);
    CHECK((p.m.giocatore()->flag54 & 4) != 0);   // l'antenna e' rimasta su per tutta l'attesa
    tiro = p.m.tiro_missile(1);
    CHECK(tiro->ordine_esecuzione);
    CHECK(p.m.tlam_eseguibile("0417"));
    CHECK_FALSE(p.m.tlam_eseguibile("0418"));
    // la carta: i cerchi di 10 nm (verde chiaro) e 40 nm (giallo) attorno al PLP, la scritta, la mappa TERCOM al landfall
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    CartaGiocatore carta;
    ContestoMissili k = contesto(1);
    Tela t;
    s.disegna_carta(t, f, p.m, carta, segni_contatti(p.m, false), k);
    const VistaCarta v = s.vista(p.m, 1);
    const Vec2 plp = v.a_tela(tiro->tlam->plp);
    const double r10 = TLAM_VALIDA_M / v.metri_per_pixel;
    int verdi = 0;
    for (int g = 0; g < 360; g += 10) {
        const Vec2 q{plp.x + r10 * std::sin(radianti(g)), plp.y - r10 * std::cos(radianti(g))};
        if (q.x > 2 && q.y > 2 && q.x < LARGHEZZA_TELA - 3 && q.y < StazioneMissili::ALTEZZA_CARTA - 3 &&
            vicino(t, q, colori::VERDE_CHIARO, 2)) ++verdi;
    }
    CHECK(verdi >= 10);   // il cerchio e' tratteggiato: non tutti i punti
    // il pannello: le due righe che si scelgono e l'ordine ricevuto in verde
    std::vector<AreaAttiva> aree;
    s.disegna_pannello(t, f, p.m, k, &aree);
    CHECK(std::count_if(aree.begin(), aree.end(), [](const AreaAttiva& a) { return a.comando.rfind("missili_riga_", 0) == 0; }) == 2);
    if (const char* d = std::getenv("M688_ISTANTANEE")) {
        Palette pal = Palette::carica(M688_DATI "/palette/giorno.pal");
        applica_palette_carta(pal, StileCarta::vga);
        applica_tema(pal, {BattelloTema::usa688, LuceSala::bianca, Fosforo::del_battello});
        t.salva_bmp(std::string(d) + "/stazione_missili_tlam.bmp", pal);
    }
}
