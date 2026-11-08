#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include "../app/carta.hpp"
#include "../app/replay.hpp"
#include "../app/segni_ambiente.hpp"
#include "missioni/griglia_ghiaccio.hpp"
#include "../app/vista_ghiaccio.hpp"
#include "grafica/palette.hpp"
#include "grafica/simboli_carta.hpp"
#include "grafica/tela.hpp"
#include "missioni/danni_missione.hpp"
#include "missioni/danni_sottomarini.hpp"
#include "missioni/interventi.hpp"
#include "missioni/missione.hpp"
#include "missioni/traccia_missione.hpp"
#include "nucleo/acustica.hpp"
#include "nucleo/campo_ghiaccio.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/geometria.hpp"
#include "nucleo/modifica_profilo.hpp"
#include "nucleo/ordini.hpp"
#include "nucleo/periscopi.hpp"
#include "nucleo/traccia_replay.hpp"
#include "nucleo/meteo.hpp"
#include "nucleo/volta_ghiaccio.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Il ghiaccio che si muove del vetrino di Dio (D4, docs/28 §3.3), da chi guarda: la vista del periscopio attaccata al campo
// vero (app/vista_ghiaccio), la polynya sulla carta dello spettatore dai soli record X (polynye_a), la carta del giocatore
// che resta il clima e la volta che misura.

namespace {

const std::string SUB = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200, rumore = 8, sonar = 19 }";

// il 688 e un Victor; col ghiaccio >= 0 il pack fisso della missione
std::string scena(const std::string& teatro, const std::string& data, double ghiaccio, double quota_688, double velocita_kn,
                  Vec2 pos_688 = {50000.0, 50000.0}, Vec2 pos_victor = {80000.0, 50000.0}, int ora_utc = 12) {
    const auto f = fs::temp_directory_path() / ("m688_prova_vetrino_ghiaccio_" + teatro + ".lua");
    std::ofstream o(f);
    o << "return { id = 'PROVA_VETRINO_GHIACCIO', teatro = '" << teatro << "', lato = 'usa', data = '" << data << "', ora_utc = " << ora_utc << ", paesaggio_sonoro = false,";
    if (ghiaccio >= 0.0) o << " meteo = { fisso = { ghiaccio = " << ghiaccio << " } },";
    if (teatro == "kunashir") o << " posa = { centro = { lat = 44.45, lon = 146.65 } },";   // come LIB_I17: il meteo guarda i punti vicini
    o << " script = {}, varianti = { { probabilita = 1, unita = {\n"
         "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = " << pos_688.x << ", y = " << pos_688.y << " },"
         "  quota_m = " << quota_688 << ", rotta = 0, velocita_kn = " << velocita_kn << ", " << SUB << " },\n"
         "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = " << pos_victor.x << ", y = " << pos_victor.y << " },"
         "  quota_m = 200, rotta = 0, velocita_kn = 0, " << SUB << " },\n"
         "}, timer = {} } } }\n";
    return f.string();
}

// la partita del giocatore, con danni, fondale e collisioni dell'Enhanced
struct Partita {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    DanniSottomarini ds;
    explicit Partita(const std::string& file) : m(file, 1, false), dm(classi, M688_DATI, 1, RegolaDanni::simulazione), ds(classi, M688_DATI, 1) {
        std::map<std::string, double> pescaggi;
        for (const auto& [id, c] : classi) pescaggi[id] = c.pescaggio_m;
        m.imposta_fondale(std::make_shared<const Fondale>(200, 200, 1000.0, 1.0, -600.0, std::vector<std::uint16_t>(200 * 200, 0)), pescaggi);
        m.collisioni_enhanced(classi, M688_DATI);
        ds.prepara(m, m.giocatore()->id);
    }
    void avanza(double s) {
        for (long i = 0; i < static_cast<long>(s * 9.0); ++i) {
            m.avanza(1.0 / 9.0);
            dm.avanza(m, 1.0 / 9.0);
            ds.avanza(m, 1.0 / 9.0);
            m.raccogli_danni(&ds, &dm);
        }
    }
};

// il vetrino: senza giocatore, con gli interventi
struct Vetrino {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    RaccoltaTraccia raccolta;
    std::vector<RecordIntervento> record;
    explicit Vetrino(const std::string& file, bool meteo_realistico = false) : m(file, 1, false), dm(classi, M688_DATI, 1, RegolaDanni::simulazione) {
        m.acustica_enhanced(M688_DATI);
        if (meteo_realistico) m.usa_meteo(Missione::ModoMeteo::realistico);   // col semplificato il ghiaccio e' quello del teatro, uguale ovunque
        m.meteo_enhanced(M688_DATI);
        m.senza_giocatore(true);
    }
    void avanza(double s) {
        for (long i = 0; i < static_cast<long>(s * 9.0 + 0.5); ++i) {
            m.avanza(1.0 / 9.0);
            dm.avanza(m, 1.0 / 9.0);
        }
        const auto nuovi = raccolta.interventi(m);
        record.insert(record.end(), nuovi.begin(), nuovi.end());
    }
};

// 80 s di vetrino alle Faroe, senza griglia: la traccia di partenza delle prove sul disegno
Traccia traccia_breve() {
    Missione mis(scena("faroe", "1989-06-15", -1.0, 200.0, 0.0), 1, false);
    mis.acustica_enhanced(M688_DATI);
    mis.senza_giocatore(true);
    RegistratoreTraccia reg("PROVA_VETRINO_GHIACCIO", 1);
    RaccoltaTraccia raccolta;
    for (int k = 0; k < 80 * 9; ++k) {
        mis.avanza(1.0 / 9.0);
        if (reg.serve(mis.tempo(), mis.eventi_missione().size()))
            reg.aggiungi(fotogramma_da(mis, nullptr, DatiApp{}, reg.ultimo_evento(), &raccolta), mis.eventi_missione().size());
    }
    return leggi_traccia(reg.dati());
}

// il primo punto di mare fondo a spirale da `da`
Vec2 in_mare(const Fondale& f, Vec2 da, double fondo_m) {
    for (double r = 0.0; r < 400000.0; r += 2000.0)
        for (double a = 0.0; a < 360.0; a += 15.0) {
            const Vec2 q{da.x + r * std::sin(radianti(a)), da.y + r * std::cos(radianti(a))};
            if (f.profondita(q) > fondo_m) return q;
        }
    return da;
}

Intervento polynya(const char* azione, Vec2 punto, double raggio_m, double transizione_s, double durata_s) {
    Intervento x;
    x.tipo = "polynya";
    x.testi["azione"] = azione;
    x.parametri["transizione_s"] = transizione_s;
    x.punto = punto;
    x.raggio_m = raggio_m;
    x.durata_s = durata_s;
    return x;
}

}  // namespace

TEST_CASE("vista del periscopio: senza cambi e' quella di sempre, dopo una deriva e una polynya resta attaccata al campo") {
    {   // senza il campo (senza l'Enhanced) la vista resta spenta
        Missione classico(scena("barents", "1989-01-15", 1.0, 60.0, 0.0), 1, false);
        classico.avanza(1.0);
        CHECK_FALSE(vista_ghiaccio(classico).attivo);
    }
    Partita p(scena("barents", "1989-01-15", 1.0, 60.0, 0.0));
    p.avanza(5.0);
    CampoGhiaccio* g = p.m.ghiaccio_da_cambiare();
    REQUIRE(g != nullptr);
    const Vec2 qui = p.m.giocatore()->pos;

    // bit per bit: la copertura del meteo qui, la deriva dei parametri per il tempo
    oculare::GhiaccioVista v = vista_ghiaccio(p.m);
    REQUIRE(v.attivo);
    CHECK(v.copertura == p.m.meteo_qui().ghiaccio);
    CHECK(v.cella_m == g->parametri().cella_m);
    CHECK(v.deriva_m.x == g->parametri().deriva_m_s.x * p.m.tempo());
    CHECK(v.deriva_m.y == g->parametri().deriva_m_s.y * p.m.tempo());

    // il pack cambia deriva: il disegno gli va dietro, non la deriva dei parametri
    g->cambia_deriva(p.m.tempo(), {1.5, -1.0});
    p.avanza(100.0);
    v = vista_ghiaccio(p.m);
    const double t = p.m.tempo();
    CHECK(v.deriva_m.x == g->spostamento(t).x);
    CHECK(v.deriva_m.y == g->spostamento(t).y);
    CHECK(std::fabs(v.deriva_m.x - g->parametri().deriva_m_s.x * t) > 100.0);
    // e i campioni sono quelli del campo nello stesso istante
    for (const Vec2 q : {qui, Vec2{qui.x + 1234.0, qui.y - 777.0}, Vec2{qui.x - 3000.0, qui.y + 2500.0}}) {
        const PuntoGhiaccio x = g->in(q, t);
        const oculare::CampioneGhiaccio c = v.campione(q);
        CHECK(c.ghiaccio == x.ghiaccio);
        CHECK(c.nilas == x.nilas);
        CHECK(c.spessore_m == x.spessore_m);
    }

    // la polynya sopra il battello: per la vista e' acqua libera, mentre il meteo e il suo bollettino dicono ancora pack
    ZonaGhiaccio z;
    z.centro = p.m.giocatore()->pos;
    z.raggio_m = 2000.0;
    z.apri = true;
    z.t0 = p.m.tempo();
    g->forza_zona(z);
    p.avanza(1.0);
    v = vista_ghiaccio(p.m);
    CHECK(p.m.meteo_qui().ghiaccio == 1.0);
    CHECK(v.copertura == 0.0);
    CHECK_FALSE(v.campione(p.m.giocatore()->pos).ghiaccio);
}

TEST_CASE("carta dello spettatore: la polynya dai record X fa la stessa strada del campo, all'andata e al ritorno") {
    const Vec2 punto{50000.0, 50000.0};
    {   // si apre nel pack: dentro il raggio raggiunto acqua, appena fuori ancora pack
        Vetrino v(scena("barents", "1989-01-15", 1.0, 200.0, 0.0));
        v.avanza(2.0);
        const double raggio = 3000.0;
        REQUIRE(v.m.intervieni(polynya("apri", punto, raggio, 100.0, 300.0)).ok);
        int viste = 0, ritorni = 0;
        for (int k = 0; k < 25; ++k) {
            v.avanza(20.0);
            const double t = v.m.tempo();
            const bool finito = v.m.stati_interventi().back().finito;
            const auto zone = polynye_a(v.record, t);
            if (zone.empty()) {
                CHECK(finito);
                CHECK(v.m.copertura_ghiaccio(punto) == 1.0);
                continue;
            }
            REQUIRE(zone.size() == 1);
            const PolynyaVista& z = zone[0];
            ++viste, ritorni += z.ritorno;
            CHECK(z.apri);
            CHECK(z.ritorno == finito);
            CHECK(z.raggio_m == raggio);
            const double aperta = raggio * z.fase;
            if (aperta > 60.0) CHECK(v.m.copertura_ghiaccio({punto.x + aperta - 50.0, punto.y}) == 0.0);
            if (aperta < raggio) CHECK(v.m.copertura_ghiaccio({punto.x + aperta + 50.0, punto.y}) == 1.0);
        }
        CHECK(viste >= 18);
        CHECK(ritorni >= 4);
    }
    {   // si chiude sul mare libero: la copertura del campo e' la fase, fino al ritorno
        Vetrino v(scena("faroe", "1989-02-15", -1.0, 200.0, 0.0));
        v.avanza(2.0);
        REQUIRE(v.m.copertura_ghiaccio(punto) == 0.0);
        REQUIRE(v.m.intervieni(polynya("chiudi", punto, 2000.0, 100.0, 300.0)).ok);
        for (int k = 0; k < 25; ++k) {
            v.avanza(20.0);
            const auto zone = polynye_a(v.record, v.m.tempo());
            CHECK(v.m.copertura_ghiaccio(punto) == doctest::Approx(zone.empty() ? 0.0 : zone[0].fase).epsilon(1e-9));
            if (!zone.empty()) CHECK_FALSE(zone[0].apri);
        }
    }
    {   // senza durata resta per sempre; un intervento fallito non si vede; gli altri tipi nemmeno
        RecordIntervento x;
        x.id = 1;
        x.t = 10.0;
        x.fase = RecordIntervento::FASE_APPLICATO;
        x.tipo = "polynya";
        x.ha_punto = true;
        x.punto = punto;
        x.raggio_m = 1000.0;
        x.chiavi = codifica_chiavi({{"azione", "apri"}, {"#transizione_s", "60"}});
        CHECK(polynye_a({x}, 5.0).empty());
        REQUIRE(polynye_a({x}, 40.0).size() == 1);
        CHECK(polynye_a({x}, 40.0)[0].fase == doctest::Approx(0.5));
        CHECK(polynye_a({x}, 1.0e6)[0].fase == 1.0);
        CHECK_FALSE(polynye_a({x}, 1.0e6)[0].ritorno);
        RecordIntervento fallito = x;
        fallito.fase = RecordIntervento::FASE_FALLITO;
        CHECK(polynye_a({fallito}, 40.0).empty());
        RecordIntervento strato = x;
        strato.tipo = "strato";
        CHECK(polynye_a({strato}, 40.0).empty());
    }
}

TEST_CASE("carta dello spettatore: la polynya si disegna, cresce, torna e poi la carta e' quella di prima") {
    const Traccia base = traccia_breve();
    REQUIRE(base.durata >= 75.0);
    // a 10 s, venti di transizione, trenta in tutto: piena a 30, il ritorno da 40 a 60
    const auto con = [&](const char* azione) {
        Traccia tr = base;
        RecordIntervento x;
        x.id = 1;
        x.t = 10.0;
        x.fase = RecordIntervento::FASE_APPLICATO;
        x.tipo = "polynya";
        x.ha_punto = true;
        x.punto = {50000.0, 53000.0};
        x.raggio_m = 3000.0;
        x.durata_s = 30.0;
        x.chiavi = codifica_chiavi({{"azione", azione}, {"#transizione_s", "20"}});
        tr.interventi.push_back(x);
        x.t = 40.0;
        x.fase = RecordIntervento::FASE_FINITO;
        tr.interventi.push_back(x);
        return tr;
    };
    const Traccia apri = con("apri"), chiudi = con("chiudi");
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    const auto carta = [&](const Traccia& tr, double t) {   // i pixel della carta a un istante, con la vista ferma
        SchermataReplay rp(M688_DATI, tr, "faroe");
        rp.lettura(SchermataReplay::Lettura::verita);
        rp.vista({50000.0, 53000.0}, 30.0);
        rp.vai(t);
        Tela tela;
        rp.disegna(tela, f);
        const Rettangolo r = rp.carta();
        std::vector<std::uint8_t> v;
        for (int y = r.y; y < r.y + r.h; ++y)
            for (int x = r.x; x < r.x + r.w; ++x) v.push_back(tela.dati()[y * LARGHEZZA_TELA + x]);
        if (const char* dir = std::getenv("M688_ISTANTANEE")) {
            Palette pv = Palette::carica(M688_DATI "/immagini/MAIN.pal");
            rp.palette(pv);
            tela.salva_bmp(std::string(dir) + "/polynya_" + (&tr == &apri ? "apri_" : &tr == &chiudi ? "chiudi_" : "base_") + std::to_string(static_cast<int>(t)) + ".bmp", pv);
        }
        return v;
    };
    const auto in_piu = [&](const Traccia& tr, double t, int colore) {   // quanti pixel di quel colore in piu' della carta senza
        const auto a = carta(tr, t), b = carta(base, t);
        return std::count(a.begin(), a.end(), static_cast<std::uint8_t>(colore)) - std::count(b.begin(), b.end(), static_cast<std::uint8_t>(colore));
    };
    const int acqua = colori::CIANO_CHIARO, ghiaccio = colori::BIANCO;
    const int ambiente = simboli::indice_famiglia(simboli::FamigliaIntervento::ambiente);

    // prima dell'intervento niente
    CHECK(carta(apri, 5.0) == carta(base, 5.0));
    CHECK(carta(chiudi, 5.0) == carta(base, 5.0));
    // si apre dal centro: l'acqua cresce e torna indietro con la stessa legge. Il colore dell'AMBIENTE e' quello dei puntini, quindi
    // nel conto ci sono anche i due cerchi e la scritta; a meta' i puntini sono circa 490, a pieno circa 1960
    const auto a_meta = in_piu(apri, 20.0, acqua), a_piena = in_piu(apri, 35.0, acqua), a_ritorno = in_piu(apri, 50.0, acqua);
    MESSAGE("acqua libera in pixel: a meta' ", a_meta, ", piena ", a_piena, ", al ritorno ", a_ritorno);
    CHECK(a_meta > 100);
    CHECK(a_piena > a_meta + 500);
    CHECK(a_ritorno == doctest::Approx(a_meta).epsilon(0.15));
    // si chiude: tutta la zona, il nilas rado, il ghiaccio fatto piu' fitto
    const auto c_nilas = in_piu(chiudi, 20.0, ghiaccio), c_fatto = in_piu(chiudi, 35.0, ghiaccio);
    MESSAGE("tratteggio del ghiaccio in pixel: nilas ", c_nilas, ", fatto ", c_fatto);
    CHECK(c_nilas > 500);
    CHECK(c_fatto > c_nilas + c_nilas / 4);
    // al ritorno, dopo la fine dell'intervento, il cerchio e la scritta restano (per quella che si apre: a_ritorno come a_meta)
    CHECK(in_piu(chiudi, 50.0, ambiente) > 100);
    // a ritorno finito la carta e' quella senza
    CHECK(carta(apri, 70.0) == carta(base, 70.0));
    CHECK(carta(chiudi, 70.0) == carta(base, 70.0));
}

TEST_CASE("carta del giocatore: la polynya e la deriva non ci sono, salvo la volta che misura il suo sondatore") {
    const Vec2 partenza{50000.0, 50000.0};
    int avvisi_acqua = 0;
    const auto giro = [&](bool cambia, std::vector<std::uint8_t>* pixel) {
        Partita p(scena("barents", "1989-01-15", 1.0, 60.0, 10.0));
        p.avanza(1.0);
        REQUIRE(p.m.applica({p.m.tempo(), TipoOrdine::bqs15, 0.0, 0.0, 1}));
        if (cambia) {
            CampoGhiaccio* g = p.m.ghiaccio_da_cambiare();
            REQUIRE(g != nullptr);
            g->cambia_deriva(p.m.tempo(), {-0.5, 0.0});
            ZonaGhiaccio z;
            z.centro = {partenza.x, partenza.y + 500.0};
            z.raggio_m = 2000.0;
            z.apri = true;
            z.t0 = p.m.tempo();
            g->forza_zona(z);
        }
        p.avanza(200.0);
        const Meteo* meteo = p.m.meteo();
        REQUIRE(meteo != nullptr);
        const Font f = Font::carica(M688_DATI "/font/originale.fnt");
        Tela t;
        VistaCarta v;
        v.centro = partenza;
        v.metri_per_pixel = 20.0;
        disegna_meteo_carta(t, f, v, p.m, *meteo, partenza, false);
        pixel->assign(t.dati(), t.dati() + LARGHEZZA_TELA * ALTEZZA_TELA);
        // le celle sondate sulla sua rotta: {libere, sondate}
        const QuadroVolta* q = p.m.quadro_volta(0);
        REQUIRE(q != nullptr);
        if (cambia)
            for (const Messaggio& k : p.m.messaggi()) avvisi_acqua += k.codice == 488 && k.unita == 0;
        std::pair<int, int> celle{0, 0};
        for (double dy = 100.0; dy < 900.0; dy += 200.0)
            if (const auto c = q->leggi({partenza.x, partenza.y + dy}); c && c->misure > 0) {
                ++celle.second;
                celle.first += c->libere == c->misure;
            }
        return celle;
    };
    std::vector<std::uint8_t> senza, con;
    const auto nel_pack = giro(false, &senza), nella_polynya = giro(true, &con);
    MESSAGE("celle della rotta libere/sondate: nel pack ", nel_pack.first, "/", nel_pack.second, ", nella polynya ", nella_polynya.first, "/", nella_polynya.second);
    // la carta del clima non sa niente: le stesse righe, col pack disegnato anche sopra la polynya
    CHECK(con == senza);
    CHECK(std::count(con.begin(), con.end(), static_cast<std::uint8_t>(colori_carta::MATITA + 5)) > 1000);
    // il sondatore si': sopra di lui l'acqua libera, e l'equipaggio lo dice
    REQUIRE(nella_polynya.second >= 3);
    CHECK(nella_polynya.first == nella_polynya.second);
    CHECK(avvisi_acqua == 1);
    REQUIRE(nel_pack.second >= 3);
    CHECK(nel_pack.first < nel_pack.second);
}

TEST_CASE("griglia del ghiaccio: si scrive e si rilegge, e una traccia rovinata non ferma niente") {
    GrigliaGhiaccio g;
    g.x0 = -5000.0;
    g.y0 = 5000.0;
    g.passo_m = 10000.0;
    g.nx = 3;
    g.ny = 2;
    g.celle = {0, 128, 255, 255, 255, 255};
    const auto l = leggi_griglia(codifica_griglia(g));
    REQUIRE(l);
    CHECK(l->nx == 3);
    CHECK(l->ny == 2);
    CHECK(l->celle == g.celle);
    CHECK(l->in({-5000.0, 5000.0}) == 0.0);                       // al centro di una cella, il suo valore
    CHECK(l->in({5000.0, 5000.0}) == doctest::Approx(128.0 / 255.0));
    CHECK(l->in({0.0, 5000.0}) == doctest::Approx(64.0 / 255.0)); // a meta' fra due
    CHECK(l->in({-5000.0, 15000.0}) == 1.0);                      // verso nord la seconda riga
    CHECK(l->in({-1.0e7, 5000.0}) == 0.0);                        // fuori: il bordo
    CHECK(l->in({1.0e7, 1.0e7}) == 1.0);
    const std::string buona = codifica_griglia(g);
    CHECK_FALSE(leggi_griglia(""));
    CHECK_FALSE(leggi_griglia("niente"));
    CHECK_FALSE(leggi_griglia(buona.substr(0, buona.size() - 1)));   // una cella in meno
    CHECK_FALSE(leggi_griglia(buona + "00"));                        // una in piu'
    CHECK_FALSE(leggi_griglia(buona.substr(0, buona.size() - 1) + "z"));
    CHECK_FALSE(leggi_griglia("0;0;-1;1;1;00"));                     // il passo
}

TEST_CASE("griglia del ghiaccio: solo nel vetrino e solo col ghiaccio; le altre tracce non cambiano") {
    const auto fondale = [] { return std::make_shared<const Fondale>(200, 200, 1000.0, 1.0, -600.0, std::vector<std::uint16_t>(200 * 200, 0)); };
    {   // la partita del giocatore: niente griglia, anche col pack
        Partita p(scena("barents", "1989-01-15", 1.0, 60.0, 0.0));
        p.avanza(1.0);
        REQUIRE(p.m.meteo() != nullptr);
        CHECK(griglia_ghiaccio_missione(p.m).empty());
    }
    {   // il vetrino col pack: la griglia copre il fondale ed e' piena
        Vetrino v(scena("barents", "1989-01-15", 1.0, 200.0, 0.0));
        CHECK(griglia_ghiaccio_missione(v.m).empty());   // senza il fondale non si sa quanto e' grande il teatro
        v.m.imposta_fondale(fondale());
        v.avanza(1.0);
        const std::string s = griglia_ghiaccio_missione(v.m);
        const auto g = leggi_griglia(s);
        REQUIRE(g);
        CHECK(g->nx == 20);
        CHECK(g->ny == 20);
        CHECK(g->in({0.0, 0.0}) == 1.0);
        // la scrive main prima del primo passo, non la fine della partita
        RegistratoreTraccia r("PROVA", 1);
        metadati_conti(v.m, r);
        CHECK(leggi_traccia(r.dati()).metadati.count(METADATO_GRIGLIA_GHIACCIO) == 0);
    }
    {   // il vetrino senza ghiaccio: niente
        Vetrino v(scena("faroe", "1989-06-15", -1.0, 200.0, 0.0));
        v.m.imposta_fondale(fondale());
        v.avanza(1.0);
        CHECK(griglia_ghiaccio_missione(v.m).empty());
    }
}

TEST_CASE("campo dello spettatore: dai record X la stessa deriva e lo stesso pack della missione, a Kunashir in febbraio") {
    const auto fondo = std::make_shared<const Fondale>(Fondale::carica(M688_DATI "/teatri/kunashir.fdl"));
    const Vec2 mare = in_mare(*fondo, {0.0, 0.0}, 800.0);
    Vetrino v(scena("kunashir", "1989-02-15", -1.0, 200.0, 0.0, mare, in_mare(*fondo, {mare.x + 15000.0, mare.y}, 800.0)), true);
    v.m.imposta_fondale(fondo);
    v.avanza(2.0);
    const std::string testo = griglia_ghiaccio_missione(v.m);
    const auto griglia = leggi_griglia(testo);
    REQUIRE(griglia);
    MESSAGE("griglia di ", griglia->nx, " x ", griglia->ny, ", passo ", griglia->passo_m, " m, ", testo.size(), " caratteri");
    int con_pack = 0;
    for (const std::uint8_t c : griglia->celle) con_pack += c >= 128;
    const int celle = griglia->nx * griglia->ny;
    MESSAGE("celle col pack: ", con_pack, " su ", celle);
    CHECK(con_pack > 0);          // il bordo del pack e' nel teatro: ne' tutto, ne' niente
    CHECK(con_pack < celle);
    const auto vera = std::make_shared<const GrigliaGhiaccio>(*griglia);
    // due derive che si sovrappongono e una polynya; a 3 nodi per dieci minuti il bordo si muove di quasi 1 km, a 30 nodi di 9
    const auto deriva = [](double rotta, double kn, double durata) {
        Intervento d;
        d.tipo = "ghiaccio_deriva";
        d.parametri = {{"rotta", rotta}, {"velocita_kn", kn}};
        d.durata_s = durata;
        return d;
    };
    REQUIRE(v.m.intervieni(deriva(90.0, 3.0, 400.0)).ok);
    v.avanza(100.0);
    REQUIRE(v.m.intervieni(deriva(200.0, 2.0, 100.0)).ok);
    REQUIRE(v.m.intervieni(polynya("apri", mare, 20000.0, 50.0, 0.0)).ok);
    const CampoGhiaccio* g = v.m.ghiaccio();
    REQUIRE(g != nullptr);
    const double lato = griglia->passo_m * griglia->nx / 2.0;
    int confronti = 0, diversi = 0;
    double scarto = 0.0;
    for (int k = 0; k < 12; ++k) {
        v.avanza(50.0);
        const double t = v.m.tempo();
        const CampoGhiaccio c = campo_spettatore(vera, v.record);
        CHECK(c.spostamento(t).x == doctest::Approx(g->spostamento(t).x));
        CHECK(c.spostamento(t).y == doctest::Approx(g->spostamento(t).y));
        for (double x = -lato; x <= lato; x += lato / 10.0)
            for (double y = -lato; y <= lato; y += lato / 10.0) {
                ++confronti;
                const double a = c.copertura_in({x, y}, t), b = v.m.copertura_ghiaccio({x, y});
                diversi += (a >= 0.5) != (b >= 0.5);
                scarto = std::max(scarto, std::fabs(a - b));
            }
        CHECK(c.copertura_in(mare, t) == 0.0);   // la polynya, ferma nel mondo
    }
    MESSAGE("il pack dello spettatore e quello vero: ", diversi, " punti diversi su ", confronti, ", scarto massimo della copertura ", scarto);
    CHECK(diversi * 50 < confronti);   // meno del 2%: la griglia e' da 10 km, il meteo vero a celle di 5, e attorno a 0,5 il clima varia poco
    CHECK(scarto < 0.05);
}

TEST_CASE("carta dello spettatore: il pack dalla griglia, col bordo che segue la deriva; le tracce senza griglia si aprono come prima") {
    const Traccia base = traccia_breve();
    // il pack a ovest di x = 45 km
    GrigliaGhiaccio griglia;
    griglia.x0 = 0.0;
    griglia.y0 = 3000.0;
    griglia.nx = griglia.ny = 11;
    griglia.celle.assign(121, 0);
    for (int j = 0; j < 11; ++j)
        for (int i = 0; i <= 4; ++i) griglia.celle[static_cast<std::size_t>(j) * 11 + i] = 255;
    const std::string testo = codifica_griglia(griglia);
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    struct Carta { std::vector<std::uint8_t> v; int w = 0; };
    const auto carta = [&](const Traccia& tr, double t, const std::string* griglia_vivo = nullptr) {
        SchermataReplay rp(M688_DATI, tr, "faroe");
        if (griglia_vivo) rp.griglia_ghiaccio(*griglia_vivo);
        rp.lettura(SchermataReplay::Lettura::verita);
        rp.vista({45000.0, 53000.0}, 5.0);
        rp.vai(t);
        Tela tela;
        rp.disegna(tela, f);
        const Rettangolo r = rp.carta();
        Carta c;
        c.w = r.w;
        for (int y = r.y; y < r.y + r.h; ++y)
            for (int x = r.x; x < r.x + r.w; ++x) c.v.push_back(tela.dati()[y * LARGHEZZA_TELA + x]);
        return c;
    };
    // la colonna piu' a destra col tratteggio, nelle righe di mezzo, libere dai riquadri
    const auto bordo = [](const Carta& c) {
        int destra = -1;
        for (int y = 150; y < 250; ++y)
            for (int x = 0; x < c.w; ++x)
                if (c.v[static_cast<std::size_t>(y) * c.w + x] == colori::BIANCO) destra = std::max(destra, x);
        return destra;
    };
    Traccia con_griglia = base;
    con_griglia.metadati[METADATO_GRIGLIA_GHIACCIO] = testo;

    // senza griglia (le tracce vecchie, i teatri senza ghiaccio, il giocatore) e con una rovinata: la carta di sempre
    const Carta senza = carta(base, 40.0);
    Traccia rovinata = base;
    rovinata.metadati[METADATO_GRIGLIA_GHIACCIO] = testo.substr(0, testo.size() / 2);
    CHECK(carta(rovinata, 40.0).v == senza.v);
    const std::string vuota;
    CHECK(carta(base, 40.0, &vuota).v == senza.v);
    CHECK(bordo(senza) < 0);
    // con la griglia il pack c'e', col bordo a meta' carta; da file e dal vivo e' lo stesso disegno
    const Carta con = carta(con_griglia, 40.0);
    CHECK(con.v != senza.v);
    CHECK(carta(base, 40.0, &testo).v == con.v);
    const int prima = bordo(con);
    MESSAGE("il bordo del pack, in colonne: fermo ", prima, " su ", con.w);
    CHECK(std::abs(prima - con.w / 2) < 40);

    // a 3 nodi verso est per 60 s il bordo va avanti di 92 m, 18 pixel a questa scala; con la deriva a 0 il pack resta indietro
    const auto con_deriva = [&](double rotta, double kn) {
        Traccia tr = con_griglia;
        RecordIntervento x;
        x.id = 1;
        x.t = 10.0;
        x.fase = RecordIntervento::FASE_APPLICATO;
        x.tipo = "ghiaccio_deriva";
        x.chiavi = codifica_chiavi({{"#rotta", std::to_string(rotta)}, {"#velocita_kn", std::to_string(kn)}});
        tr.interventi.push_back(x);
        return tr;
    };
    const Traccia est = con_deriva(90.0, 3.0);
    const int a_10 = bordo(carta(est, 5.0)), a_70 = bordo(carta(est, 70.0));
    MESSAGE("il bordo con la deriva: a 5 s ", a_10, ", a 70 s ", a_70);
    CHECK(a_70 - a_10 == doctest::Approx(18.5).epsilon(0.3));

    // una polynya nel pack: il tratteggio sparisce dentro il disco
    Traccia buco = con_griglia;
    RecordIntervento z;
    z.id = 2;
    z.t = 5.0;
    z.fase = RecordIntervento::FASE_APPLICATO;
    z.tipo = "polynya";
    z.ha_punto = true;
    z.punto = {44500.0, 53000.0};
    z.raggio_m = 300.0;
    z.chiavi = codifica_chiavi({{"azione", "apri"}});
    buco.interventi.push_back(z);
    const auto bianchi = [](const Carta& c) { return std::count(c.v.begin(), c.v.end(), static_cast<std::uint8_t>(colori::BIANCO)); };
    CHECK(bianchi(carta(buco, 40.0)) < bianchi(con) - 200);
}

// Un'istantanea da guardare, col teatro e il pack veri: si salva solo con M688_ISTANTANEE
TEST_CASE("carta dello spettatore: istantanea a Kunashir in febbraio" * doctest::skip(std::getenv("M688_ISTANTANEE") == nullptr)) {
    const auto fondo = std::make_shared<const Fondale>(Fondale::carica(M688_DATI "/teatri/kunashir.fdl"));
    // il battello e la polynya dentro il pack, il Victor piu' a sud; il bordo del pack in mezzo
    const Vec2 mare = in_mare(*fondo, {-60000.0, 50000.0}, 800.0), altro = in_mare(*fondo, {mare.x + 40000.0, mare.y - 45000.0}, 800.0);
    Vetrino v(scena("kunashir", "1989-02-15", -1.0, 200.0, 0.0, mare, altro), true);
    v.m.imposta_fondale(fondo);
    RegistratoreTraccia reg("PROVA_VETRINO_GHIACCIO", 1);
    RaccoltaTraccia raccolta;
    const auto gira = [&](double s) {
        for (long k = 0; k < static_cast<long>(s * 9.0); ++k) {
            v.m.avanza(1.0 / 9.0);
            v.dm.avanza(v.m, 1.0 / 9.0);
            if (reg.serve(v.m.tempo(), v.m.eventi_missione().size()))
                reg.aggiungi(fotogramma_da(v.m, nullptr, DatiApp{}, reg.ultimo_evento(), &raccolta), v.m.eventi_missione().size());
        }
    };
    reg.metadato(METADATO_GRIGLIA_GHIACCIO, griglia_ghiaccio_missione(v.m));
    gira(2.0);
    REQUIRE(v.m.intervieni(polynya("apri", {mare.x, mare.y + 15000.0}, 10000.0, 60.0, 0.0)).ok);
    gira(120.0);
    metadati_conti(v.m, reg);
    const Traccia tr = leggi_traccia(reg.dati());
    SchermataReplay rp(M688_DATI, tr, "kunashir");
    rp.lettura(SchermataReplay::Lettura::verita);
    rp.vista({mare.x + 30000.0, mare.y - 15000.0}, 300.0);
    rp.vai(tr.durata);
    Tela t;
    rp.disegna(t, Font::carica(M688_DATI "/font/originale.fnt"));
    Palette pv = Palette::carica(M688_DATI "/immagini/MAIN.pal");
    rp.palette(pv);
    t.salva_bmp(std::string(std::getenv("M688_ISTANTANEE")) + "/pack_kunashir.bmp", pv);
    MESSAGE("il battello a ", mare.x, " ", mare.y, ", fondo ", fondo->profondita(mare), " m; il Victor a ", altro.x, " ", altro.y, ", fondo ", fondo->profondita(altro), " m");
}

// Il profilo del suono nella scheda dello spettatore (docs/28 §3.3): lo strato che cambia, sulla stessa strada del mare vero
namespace {
Intervento strato(const char* modifica, double transizione_s, double durata_s) {
    Intervento x;
    x.tipo = "strato";
    x.testi["modifica"] = modifica;
    x.parametri["transizione_s"] = transizione_s;
    x.durata_s = durata_s;
    return x;
}
}  // namespace

TEST_CASE("scheda dello spettatore: dai record X lo stesso profilo della missione, nella zona e fuori, all'andata e al ritorno") {
    // di notte: niente effetto pomeriggio, che la scheda non ha
    Vetrino v(scena("faroe", "1989-02-15", -1.0, 200.0, 0.0, {50000.0, 50000.0}, {80000.0, 50000.0}, 2));
    v.avanza(2.0);
    const ClimaTeatro clima = carica_clima(M688_DATI "/teatri/faroe.suono.toml");
    const ProfiloSuono& base = clima.profilo(v.m.mese_acustica());
    const Vec2 dentro{0.0, 0.0}, fuori{5000.0, 0.0};
    Intervento zona = strato("sposta", 120.0, 600.0);
    zona.parametri["quota_m"] = 100.0;
    zona.punto = dentro;
    zona.raggio_m = 2000.0;
    REQUIRE(v.m.intervieni(zona).ok);
    v.avanza(30.0);
    Intervento canale = strato("canale", 60.0, 300.0);   // sopra, in tutto il teatro
    canale.parametri["quota_m"] = 600.0;
    REQUIRE(v.m.intervieni(canale).ok);
    int cambiati = 0, confronti = 0;
    for (int k = 0; k < 40; ++k) {
        v.avanza(25.0);
        const double t = v.m.tempo();
        for (const Vec2 q : {dentro, fuori}) {
            const ProfiloSuono s = profilo_spettatore(base, v.record, q, t);
            const ProfiloSuono* m = v.m.profilo_in(q, q);
            REQUIRE(m != nullptr);
            REQUIRE(s.z_m == m->z_m);
            for (std::size_t i = 0; i < s.c_m_s.size(); ++i) REQUIRE(s.c_m_s[i] == doctest::Approx(m->c_m_s[i]));
            CHECK(s.sld_m == doctest::Approx(m->sld_m));
            CHECK(s.asse_m == doctest::Approx(m->asse_m));
            ++confronti;
            cambiati += s.c_m_s != base.c_m_s;
        }
    }
    MESSAGE("profili confrontati ", confronti, ", cambiati ", cambiati);
    CHECK(cambiati > 10);
    CHECK(cambiati < confronti);
    // finito tutto, di nuovo quello del mese
    CHECK(profilo_spettatore(base, v.record, dentro, v.m.tempo()).c_m_s == base.c_m_s);
    // un intervento fallito non conta
    RecordIntervento fallito;
    fallito.tipo = "strato";
    fallito.fase = RecordIntervento::FASE_FALLITO;
    fallito.chiavi = codifica_chiavi({{"modifica", "togli"}});
    CHECK(profilo_spettatore(base, {fallito}, dentro, 10.0).c_m_s == base.c_m_s);
}

TEST_CASE("scheda dello spettatore: le righe dicono com'era, e nelle acque polari non inventano uno strato") {
    const ClimaTeatro faroe = carica_clima(M688_DATI "/teatri/faroe.suono.toml");
    const ProfiloSuono& febbraio = faroe.profilo(2);
    const std::string prima = std::to_string(static_cast<int>(std::lround(febbraio.sld_m)));
    CHECK(riga_strato(febbraio, febbraio) == "STRATO " + prima + " M");
    CHECK(riga_strato(modifica_profilo(febbraio, {ModificaStrato::Tipo::sposta, 100.0}), febbraio).find("(ERA " + prima + " M)") != std::string::npos);
    CHECK(riga_strato(modifica_profilo(febbraio, {ModificaStrato::Tipo::togli, 0.0}), febbraio) == "STRATO NESSUNO (ERA " + prima + " M)");
    const ProfiloSuono canale = modifica_profilo(febbraio, {ModificaStrato::Tipo::canale, 900.0});
    CHECK(riga_asse(febbraio, febbraio).find("ERA") == std::string::npos);
    CHECK(riga_asse(canale, febbraio).find("ERA") != std::string::npos);
    const ClimaTeatro barents = carica_clima(M688_DATI "/teatri/barents.suono.toml");
    REQUIRE_FALSE(modifica_applicabile(barents.profilo(1), ModificaStrato::Tipo::sposta));
    CHECK(riga_strato(barents.profilo(1), barents.profilo(1)) == "STRATO NESSUNO");
    // la scala: lo strato e l'unita' dentro, multipli di 50, non oltre i dati
    CHECK(scala_suono(febbraio, febbraio, 0.0) >= 1.4 * febbraio.sld_m);
    CHECK(scala_suono(febbraio, febbraio, 800.0) >= 1000.0);
    CHECK(static_cast<int>(scala_suono(febbraio, febbraio, 0.0)) % 50 == 0);
    ProfiloSuono corto = febbraio;
    corto.z_m = {0.0, 80.0};
    corto.c_m_s = {1480.0, 1479.0};
    CHECK(scala_suono(corto, corto, 0.0) == 80.0);
}

TEST_CASE("scheda dello spettatore: solo nel vetrino; senza il mese la scheda e il replay restano quelli di prima") {
    {   // il giocatore: niente metadato
        Partita p(scena("faroe", "1989-02-15", -1.0, 200.0, 0.0));
        p.avanza(1.0);
        RegistratoreTraccia r("PROVA", 1);
        metadati_conti(p.m, r);
        CHECK(leggi_traccia(r.dati()).metadati.count(METADATO_MESE_ACUSTICA) == 0);
    }
    {   // il vetrino: il mese dell'acqua
        Vetrino v(scena("faroe", "1989-02-15", -1.0, 200.0, 0.0));
        v.avanza(1.0);
        RegistratoreTraccia r("PROVA", 1);
        metadati_conti(v.m, r);
        const Traccia tr = leggi_traccia(r.dati());
        REQUIRE(tr.metadati.count(METADATO_MESE_ACUSTICA) == 1);
        CHECK(tr.metadati.at(METADATO_MESE_ACUSTICA) == "2");
    }
    const Traccia base = traccia_breve();
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    // i pixel di tutto lo schermo; dal vivo con un'unita' scelta, per la scheda
    const auto schermo = [&](const Traccia& tr, bool vivo, std::optional<int> mese = std::nullopt) {
        SchermataReplay rp(M688_DATI, tr, "faroe");
        if (mese) rp.mese_acustica(*mese);
        rp.lettura(SchermataReplay::Lettura::verita);
        if (vivo) {
            rp.dal_vivo(true);
            rp.adesso(tr.durata, 1.0, true);
            Comandi c;
            c.tab = 1;
            rp.comandi_vivo(c, 0.1);
        }
        rp.vai(tr.durata);
        Tela t;
        rp.disegna(t, f);
        return std::vector<std::uint8_t>(t.dati(), t.dati() + LARGHEZZA_TELA * ALTEZZA_TELA);
    };
    for (const bool vivo : {false, true}) {
        const auto senza = schermo(base, vivo);
        // un metadato sbagliato non ferma niente
        Traccia rovinata = base;
        rovinata.metadati[METADATO_MESE_ACUSTICA] = "niente";
        CHECK(schermo(rovinata, vivo) == senza);
        CHECK(schermo(base, vivo, 13) == senza);
        // col mese c'e', da file e dal vivo uguale
        Traccia con = base;
        con.metadati[METADATO_MESE_ACUSTICA] = "2";
        const auto col_mese = schermo(con, vivo);
        CHECK(col_mese != senza);
        CHECK(schermo(base, vivo, 2) == col_mese);
        // lo strato cambiato si vede nella stessa scheda
        Traccia cambiato = con;
        RecordIntervento x;
        x.id = 1;
        x.t = 5.0;
        x.fase = RecordIntervento::FASE_APPLICATO;
        x.tipo = "strato";
        x.chiavi = codifica_chiavi({{"modifica", "sposta"}, {"#quota_m", "100"}});
        cambiato.interventi.push_back(x);
        CHECK(schermo(cambiato, vivo) != col_mese);
    }
}

// Due istantanee da guardare, alle Faroe in febbraio col mare vero: si salvano solo con M688_ISTANTANEE
TEST_CASE("scheda dello spettatore: istantanea alle Faroe" * doctest::skip(std::getenv("M688_ISTANTANEE") == nullptr)) {
    const auto fondo = std::make_shared<const Fondale>(Fondale::carica(M688_DATI "/teatri/faroe.fdl"));
    const Vec2 mare = in_mare(*fondo, {0.0, 0.0}, 600.0), altro = in_mare(*fondo, {mare.x + 12000.0, mare.y + 8000.0}, 600.0);
    Vetrino v(scena("faroe", "1989-02-15", -1.0, 150.0, 4.0, mare, altro, 2));
    v.m.imposta_fondale(fondo);
    RegistratoreTraccia reg("PROVA_VETRINO_GHIACCIO", 1);
    reg.metadato("teatro", "faroe");
    if (const auto* g = v.m.giocatore()) reg.metadato("giocatore", std::to_string(g->id));
    RaccoltaTraccia raccolta;
    const auto gira = [&](double s) {
        for (long k = 0; k < static_cast<long>(s * 9.0); ++k) {
            v.m.avanza(1.0 / 9.0);
            v.dm.avanza(v.m, 1.0 / 9.0);
            if (reg.serve(v.m.tempo(), v.m.eventi_missione().size()))
                reg.aggiungi(fotogramma_da(v.m, nullptr, DatiApp{}, reg.ultimo_evento(), &raccolta), v.m.eventi_missione().size());
        }
    };
    gira(2.0);
    Intervento x = strato("sposta", 60.0, 0.0);
    x.parametri["quota_m"] = 100.0;
    x.punto = Vec2{mare.x - 2500.0, mare.y - 3500.0};   // il battello dentro la zona, le scritte non si coprono
    x.raggio_m = 8000.0;
    REQUIRE(v.m.intervieni(x).ok);
    gira(120.0);
    metadati_conti(v.m, reg);
    const Traccia tr = leggi_traccia(reg.dati());
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    const std::string dir = std::getenv("M688_ISTANTANEE");
    Palette pv = Palette::carica(M688_DATI "/immagini/MAIN.pal");
    {
        SchermataReplay rp(M688_DATI, tr, "faroe");
        rp.lettura(SchermataReplay::Lettura::verita);
        rp.vista(mare, 60.0);
        rp.vai(tr.durata);
        Tela t;
        rp.disegna(t, f);
        Palette p = pv;
        rp.palette(p);
        t.salva_bmp(dir + "/suono_replay.bmp", p);
    }
    {
        Traccia vuota = tr;
        SchermataReplay rp(M688_DATI, vuota, "faroe");
        rp.dal_vivo(true);
        rp.adesso(tr.durata, 1.0, true);
        Comandi c;
        c.tab = 1;
        rp.comandi_vivo(c, 0.1);
        rp.vista(mare, 60.0);
        Tela t;
        rp.disegna(t, f);
        Palette p = pv;
        rp.palette(p);
        t.salva_bmp(dir + "/suono_vivo.bmp", p);
    }
    MESSAGE("il battello a ", mare.x, " ", mare.y, ", fondo ", fondo->profondita(mare), " m; il Victor a ", altro.x, " ", altro.y, ", fondo ", fondo->profondita(altro), " m");
}

TEST_CASE("carta dello spettatore: una polynya che si apre dove non c'e' pack non disegna acqua; la griglia va presa prima del meteo fermo") {
    const Traccia base = traccia_breve();
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    // i puntini della polynya: quanti pixel CIANO_CHIARO in piu', su una griglia di 4 pixel
    const auto puntini = [&](const std::optional<std::string>& griglia, bool con_polynya, int* a_ovest = nullptr) {
        Traccia tr = base;
        if (griglia) tr.metadati[METADATO_GRIGLIA_GHIACCIO] = *griglia;
        if (con_polynya) {
            RecordIntervento z;
            z.id = 1;
            z.t = 5.0;
            z.fase = RecordIntervento::FASE_APPLICATO;
            z.tipo = "polynya";
            z.ha_punto = true;
            z.punto = {45000.0, 53000.0};
            z.raggio_m = 3000.0;
            z.chiavi = codifica_chiavi({{"azione", "apri"}});
            tr.interventi.push_back(z);
        }
        SchermataReplay rp(M688_DATI, tr, "faroe");
        rp.lettura(SchermataReplay::Lettura::verita);
        rp.vista({45000.0, 53000.0}, 30.0);
        rp.vai(40.0);
        Tela t;
        rp.disegna(t, f);
        const Rettangolo r = rp.carta();
        int n = 0, o = 0;
        for (int y = r.y; y < r.y + r.h; y += 1)
            for (int x = r.x; x < r.x + r.w; x += 1) {
                if (y % 4 != 0 || x % 4 != 0 || t.dati()[y * LARGHEZZA_TELA + x] != colori::CIANO_CHIARO) continue;
                ++n;
                o += x < r.x + r.w / 2;
            }
        if (a_ovest) *a_ovest = o;
        return n;
    };
    // il pack a ovest di x = 45 km: i puntini solo dove c'era
    GrigliaGhiaccio meta;
    meta.y0 = 3000.0;
    meta.nx = meta.ny = 11;
    meta.celle.assign(121, 0);
    for (int j = 0; j < 11; ++j)
        for (int i = 0; i <= 4; ++i) meta.celle[static_cast<std::size_t>(j) * 11 + i] = 255;
    const std::string testo = codifica_griglia(meta);
    int o_tutti = 0, o_meta = 0, o_senza = 0;
    const int vecchia = puntini(std::nullopt, true, &o_tutti) - puntini(std::nullopt, false);
    const int con_pack = puntini(testo, true, &o_meta) - puntini(testo, false, &o_senza);
    const int mare_libero = puntini(std::string(), true) - puntini(std::string(), false);
    MESSAGE("puntini: traccia senza griglia ", vecchia, ", col pack a ovest ", con_pack, " (a ovest ", o_meta - o_senza, "), mare libero ", mare_libero);
    CHECK(vecchia > 300);                                // le tracce vecchie, come prima: tutto il disco
    CHECK(con_pack == doctest::Approx(vecchia / 2.0).epsilon(0.25));   // meta' disco
    CHECK(o_meta - o_senza > con_pack * 8 / 10);         // la meta' a ovest
    CHECK(std::abs(mare_libero) < 0.1 * vecchia);        // niente acqua da aprire: restano il cerchio e la scritta

    // un vetrino di oggi senza griglia (ha il mese dell'acqua) e' un teatro senza ghiaccio
    Traccia oggi = base;
    oggi.metadati[METADATO_MESE_ACUSTICA] = "6";
    RecordIntervento z;
    z.id = 1;
    z.t = 5.0;
    z.fase = RecordIntervento::FASE_APPLICATO;
    z.tipo = "polynya";
    z.ha_punto = true;
    z.punto = {45000.0, 53000.0};
    z.raggio_m = 3000.0;
    z.chiavi = codifica_chiavi({{"azione", "apri"}});
    oggi.interventi.push_back(z);
    SchermataReplay a(M688_DATI, oggi, "faroe"), b(M688_DATI, oggi, "faroe");
    b.griglia_ghiaccio(std::string());
    Tela ta, tb;
    for (SchermataReplay* r : {&a, &b}) {
        r->lettura(SchermataReplay::Lettura::verita);
        r->vista({45000.0, 53000.0}, 30.0);
        r->vai(40.0);
    }
    a.disegna(ta, f);
    b.disegna(tb, f);
    CHECK(std::equal(ta.dati(), ta.dati() + LARGHEZZA_TELA * ALTEZZA_TELA, tb.dati()));
}

// Valori attesi scritti prima del codice (nota disassemblatore-d4-strato-ghiaccio-proposta.md, 06/11)
TEST_CASE("meteo fermato da un intervento: il mare cambia in tutto il teatro, il pack resta dov'e'") {
    const auto fondo = std::make_shared<const Fondale>(Fondale::carica(M688_DATI "/teatri/kunashir.fdl"));
    const Vec2 mare = in_mare(*fondo, {0.0, 0.0}, 800.0);
    Vetrino v(scena("kunashir", "1989-02-15", -1.0, 200.0, 0.0, mare, in_mare(*fondo, {mare.x + 15000.0, mare.y}, 800.0)), true);
    v.m.imposta_fondale(fondo);
    const std::string testo_prima = griglia_ghiaccio_missione(v.m);
    const auto prima = leggi_griglia(testo_prima);
    REQUIRE(prima);
    const auto diverse = [](const GrigliaGhiaccio& g) { return std::set<std::uint8_t>(g.celle.begin(), g.celle.end()).size(); };
    v.avanza(2.0);
    // un punto nel pack e uno in acqua, dalla griglia
    std::optional<Vec2> nel_pack, in_acqua;
    for (int j = 0; j < prima->ny; ++j)
        for (int i = 0; i < prima->nx; ++i) {
            const Vec2 q{prima->x0 + i * prima->passo_m, prima->y0 + j * prima->passo_m};
            const double c = prima->celle[static_cast<std::size_t>(j) * prima->nx + i] / 255.0;
            if (!nel_pack && c >= 0.6) nel_pack = q;
            if (!in_acqua && c < 0.1) in_acqua = q;
        }
    REQUIRE(nel_pack);
    REQUIRE(in_acqua);
    const double pack0 = v.m.copertura_ghiaccio(*nel_pack), acqua0 = v.m.copertura_ghiaccio(*in_acqua);
    Intervento burrasca;
    burrasca.tipo = "meteo";
    burrasca.parametri = {{"mare", 6.0}};
    burrasca.durata_s = 100.0;
    REQUIRE(v.m.intervieni(burrasca).ok);
    v.avanza(20.0);
    REQUIRE(v.m.meteo()->fisso());
    const std::string testo_durante = griglia_ghiaccio_missione(v.m);
    const auto durante = leggi_griglia(testo_durante);
    REQUIRE(durante);
    MESSAGE("valori diversi nella griglia: prima ", diverse(*prima), ", durante ", diverse(*durante), "; copertura nel pack ", pack0, " -> ", v.m.copertura_ghiaccio(*nel_pack),
            ", in acqua ", acqua0, " -> ", v.m.copertura_ghiaccio(*in_acqua));
    CHECK(testo_durante == testo_prima);
    CHECK(diverse(*durante) == 171);
    CHECK(v.m.copertura_ghiaccio(*nel_pack) == doctest::Approx(pack0).epsilon(0.03));
    CHECK(v.m.copertura_ghiaccio(*in_acqua) == doctest::Approx(acqua0).epsilon(0.03));
    // il mare e' quello dell'intervento, ovunque
    CHECK(classe_douglas(v.m.meteo()->in(*nel_pack).hs_m()) == 6);
    CHECK(classe_douglas(v.m.meteo()->in(*in_acqua).hs_m()) == 6);
    CHECK(v.m.meteo()->in(*nel_pack).hs_m() == v.m.meteo()->in(*in_acqua).hs_m());
    // finito: il meteo del teatro, e la griglia di prima
    v.avanza(100.0);
    CHECK_FALSE(v.m.meteo()->fisso());
    CHECK(griglia_ghiaccio_missione(v.m) == testo_prima);

    // sopra un meteo fisso (l'autore, il semplificato) resta uniforme, com'era
    DatiMeteo dati = carica_meteo(M688_DATI "/meteo.toml");
    Meteo m(dati, "kunashir", 2, 12.0, 1);
    m.imposta_origine(44.45, 146.65);
    StatoMeteo fermo = m.stato();
    fermo.ghiaccio = 0.2;
    m.fissa(fermo);
    m.fissa_sopra(fermo);
    CHECK(m.in({-60000.0, 50000.0}).ghiaccio == 0.2);
    CHECK(m.in({60000.0, -50000.0}).ghiaccio == 0.2);
    // da libero tiene i punti; due di seguito pure; libera e fissa azzerano
    Meteo n(dati, "kunashir", 2, 12.0, 1);
    n.imposta_origine(44.45, 146.65);
    const double a = n.in({-60000.0, 50000.0}).ghiaccio, b = n.in({60000.0, -50000.0}).ghiaccio;
    REQUIRE(a != b);
    n.fissa_sopra(n.stato());
    n.fissa_sopra(n.stato());
    CHECK(n.in({-60000.0, 50000.0}).ghiaccio == doctest::Approx(a));
    CHECK(n.in({60000.0, -50000.0}).ghiaccio == doctest::Approx(b));
    n.fissa(n.stato());
    CHECK(n.in({-60000.0, 50000.0}).ghiaccio == n.in({60000.0, -50000.0}).ghiaccio);
    n.libera();
    CHECK(n.in({-60000.0, 50000.0}).ghiaccio == doctest::Approx(a));
}

TEST_CASE("telecamera verticale dell'Alfa: la quota del battello e la copertura sopra di lui, anche sotto una polynya") {
    {   // senza il campo (senza l'Enhanced): niente ghiaccio
        Missione classico(scena("barents", "1989-01-15", 1.0, 60.0, 0.0), 1, false);
        classico.avanza(1.0);
        CHECK(dati_telecamera(classico).ghiaccio == 0.0);
    }
    Partita p(scena("barents", "1989-01-15", 1.0, 60.0, 0.0));
    p.avanza(5.0);
    CampoGhiaccio* g = p.m.ghiaccio_da_cambiare();
    REQUIRE(g != nullptr);
    DatiTelecamera c = dati_telecamera(p.m);
    CHECK(c.quota_m == doctest::Approx(60.0).epsilon(0.1));
    CHECK(c.ghiaccio == 1.0);
    ZonaGhiaccio z;
    z.centro = p.m.giocatore()->pos;
    z.raggio_m = 2000.0;
    z.t0 = p.m.tempo();
    g->forza_zona(z);
    p.avanza(1.0);
    c = dati_telecamera(p.m);
    CHECK(c.ghiaccio == 0.0);
    // e l'immagine cambia: col pack sopra e in acqua libera, alla stessa quota
    const auto immagine = [](double quota, double ghiaccio) {
        Tela t;
        oculare::telecamera_verticale(t, {0, 0, 200, 200}, quota, 0.8, ghiaccio, 10.0);
        return std::vector<std::uint8_t>(t.dati(), t.dati() + LARGHEZZA_TELA * 200);
    };
    CHECK(immagine(60.0, 1.0) != immagine(60.0, 0.0));
    CHECK(immagine(60.0, 0.0) != immagine(0.0, 0.0));
}

// Le misure per le due proposte del 06/11 (nota disassemblatore-d4-strato-ghiaccio-proposta.md): non provano niente, stampano i numeri.
// Si accendono con M688_MISURE
TEST_CASE("misure: il mare e i falsi echi sotto una polynya" * doctest::skip(std::getenv("M688_MISURE") == nullptr)) {
    const auto fondo = std::make_shared<const Fondale>(Fondale::carica(M688_DATI "/teatri/kunashir.fdl"));
    const ClimaTeatro clima = carica_clima(M688_DATI "/teatri/kunashir.suono.toml");
    const Vec2 pack = in_mare(*fondo, {-60000.0, 50000.0}, 800.0);
    // il mare: quello di oggi (smorzato dalla copertura del meteo), quello di una polynya col suo fetch (SMB) e quello senza ghiaccio
    for (const std::uint64_t seme : {1ull, 2ull, 3ull, 4ull}) {
        const std::string f = scena("kunashir", "1989-03-15", -1.0, 100.0, 0.0, pack, in_mare(*fondo, {pack.x + 40000.0, pack.y - 45000.0}, 800.0));
        std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
        Missione m(f, seme, false);
        m.acustica_enhanced(M688_DATI);
        m.usa_meteo(Missione::ModoMeteo::realistico);
        m.meteo_enhanced(M688_DATI);
        const StatoMeteo q = m.meteo()->in(pack);
        const double oggi = q.hs_m(), senza = q.ghiaccio < 1.0 ? oggi / (1.0 - q.ghiaccio) : oggi, u = q.vento_m_s;
        const auto smb = [&](double fetch_m) { return 0.0016 * std::sqrt(9.81 * fetch_m / (u * u)) * u * u / 9.81; };
        Ambiente a;
        a.profilo = &clima.profilo(3);
        a.fondale_m = fondo->profondita(pack);
        a.vento_m_s = u;
        a.ghiaccio = 0.0;
        const auto riga = [&](const char* nome, double hs) {
            a.hs_m = hs;
            const ComponentiRumore n = componenti_rumore_ambiente(1000.0, a);
            MESSAGE("seme ", seme, " ", nome, ": Hs ", hs, " m, Douglas ", classe_douglas(hs), ", rumore del vento ", n.vento, " dB, perdita 50 -> 100 m a 10 km ",
                    perdita_trasmissione(1000.0, 10000.0, 50.0, 100.0, a).tl_db, " dB, 50 -> 50 m a 2 km ", perdita_trasmissione(1000.0, 2000.0, 50.0, 50.0, a).tl_db, " dB");
        };
        MESSAGE("seme ", seme, ": vento ", u, " m/s, copertura ", q.ghiaccio);
        riga("oggi", oggi);
        riga("fetch 4 km (raggio 2)", smb(4000.0));
        riga("fetch 20 km (raggio 10)", smb(20000.0));
        riga("senza ghiaccio", senza);
    }
    // i falsi echi: quelli veri del ping nel pack e le chiglie candidate attorno a una polynya, col ping dal centro
    Partita p(scena("barents", "1989-01-15", 1.0, 40.0, 10.0));   // come la prova dei falsi echi in prove_ghiaccio_missione
    p.avanza(1.0);
    REQUIRE(p.m.applica({p.m.tempo(), TipoOrdine::impulso_sonar, 0.0, 0.0, 0}));
    p.avanza(1.0);
    MESSAGE("il ping nel pack: ", p.m.echi_falsi_attivo().size(), " falsi echi");
    const Vec2 qui = p.m.giocatore()->pos;
    CampoGhiaccio* g = p.m.ghiaccio_da_cambiare();
    REQUIRE(g != nullptr);
    const auto candidate = [&](const CampoGhiaccio& c) { return echi_chiglie(c, qui, 40.0, p.m.giocatore()->rotta, 240.0, 8000.0, 20.0, p.m.tempo(), 100.0, 6.0).size(); };
    MESSAGE("chiglie candidate nel pack: ", candidate(*g));
    for (const double r : {1000.0, 2000.0, 4000.0, 6000.0, 8000.0}) {
        CampoGhiaccio c = *g;
        ZonaGhiaccio z;
        z.centro = qui;
        z.raggio_m = r;
        z.t0 = p.m.tempo();
        c.forza_zona(z);
        MESSAGE("chiglie candidate col ping al centro di una polynya di raggio ", r, " m: ", candidate(c));
    }
}

// Valori attesi scritti prima del codice (nota disassemblatore-d4-strato-ghiaccio-proposta.md, 06/11)
TEST_CASE("falsi echi: una polynya non zittisce le chiglie attorno; senza zone i numeri di sempre") {
    const auto echi = [](double raggio_m, std::vector<double>* distanze = nullptr) {
        Partita p(scena("barents", "1989-01-15", 1.0, 40.0, 10.0));
        p.avanza(1.0);
        if (raggio_m > 0.0) {
            CampoGhiaccio* g = p.m.ghiaccio_da_cambiare();
            REQUIRE(g != nullptr);
            ZonaGhiaccio z;
            z.centro = p.m.giocatore()->pos;
            z.raggio_m = raggio_m;
            z.t0 = p.m.tempo();
            g->forza_zona(z);
        }
        REQUIRE(p.m.applica({p.m.tempo(), TipoOrdine::impulso_sonar, 0.0, 0.0, 0}));
        p.avanza(1.0);
        if (distanze)
            for (const auto& e : p.m.echi_falsi_attivo()) distanze->push_back(e.distanza_m);
        return static_cast<int>(p.m.echi_falsi_attivo().size());
    };
    std::vector<double> nel_pack;
    const int e0 = echi(0.0, &nel_pack);
    std::sort(nel_pack.begin(), nel_pack.end());
    std::string lista;
    for (const double d : nel_pack) lista += std::to_string(static_cast<int>(d)) + " ";
    MESSAGE("i falsi echi nel pack, a metri: ", lista);
    CHECK(e0 == 7);
    // dentro la polynya restano le chiglie oltre il bordo: le stesse, un'eco piu' o meno
    for (const double r : {1000.0, 2000.0, 4000.0, 6000.0, 8000.0}) {
        const int oltre = static_cast<int>(std::count_if(nel_pack.begin(), nel_pack.end(), [&](double d) { return d > r; }));
        const int qui = echi(r);
        MESSAGE("polynya di ", r, " m: ", qui, " falsi echi, nel pack erano oltre il bordo ", oltre);
        CHECK(std::abs(qui - oltre) <= 1);
    }
    CHECK(echi(8000.0) == 0);
}

TEST_CASE("telecamera verticale: si accende solo sull'Alfa, ed e' solo la vista") {
    const std::string f = (fs::temp_directory_path() / "m688_prova_telecamera.lua").string();
    const auto stato = [&](const char* classe) {
        std::ofstream(f) << "return { id = 'PROVA_TELECAMERA', teatro = 'barents', lato = 'usa', data = '1989-01-15', ora_utc = 12, paesaggio_sonoro = false,"
                            " script = {}, varianti = { { probabilita = 1, unita = {\n"
                            "{ id = 0, classe = '" << classe << "', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 50000, y = 50000 },"
                            "  quota_m = 60, rotta = 0, velocita_kn = 0, " << SUB << " },\n"
                            "}, timer = {} } } }\n";
        Partita p(f);
        p.avanza(1.0);
        const PostoPeriscopi* pp = p.m.posto_periscopi();
        REQUIRE(pp != nullptr);
        const StatoPeriscopio prima = pp->stato();
        p.m.vista_periscopio(prima.ingrandimento, prima.modo, prima.filtro, prima.sdoppiamento_gradi, !prima.telecamera_verticale);
        const bool accesa = pp->stato().telecamera_verticale;
        CHECK_FALSE(prima.telecamera_verticale);
        p.m.vista_periscopio(prima.ingrandimento, prima.modo, prima.filtro, prima.sdoppiamento_gradi, false);
        CHECK_FALSE(pp->stato().telecamera_verticale);
        return accesa;
    };
    CHECK(stato("alfa"));
    CHECK_FALSE(stato("los_angeles"));
    fs::remove(f);
}
