#include <doctest.h>

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>

#include "../app/pannello_interventi.hpp"
#include "../app/parti_credute.hpp"
#include "../app/segni_ambiente.hpp"
#include "../app/replay.hpp"
#include "grafica/simboli_carta.hpp"
#include "grafica/tela.hpp"
#include "missioni/danni_missione.hpp"
#include "missioni/interventi.hpp"
#include "missioni/missione.hpp"
#include "missioni/traccia_missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/traccia_replay.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Il pannello degli interventi del vetrino di Dio (app/pannello_interventi, docs/28 §6) sul catalogo vero del motore
// (missioni/interventi.hpp): la traduzione delle voci, i passi del pannello, la richiesta e l'intervento accettato
// dalla missione.

namespace {

const std::string SUB = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200, rumore = 8, sonar = 19 }";
const std::string NAVE = "modello = { massima_kn = 32, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 0, integrita = 100, rumore = 10, sonar = 20 }";

// la scena in mare aperto delle Faroe: attorno a MARE, per 20 km in ogni verso, sempre piu' di 200 m d'acqua (le
// istantanee non mostrano unita' sulla terraferma; la missione di prova non ha il fondale e non le fermerebbe)
const Vec2 MARE{-85000.0, -60000.0};
Vec2 in_mare(double x, double y) { return {MARE.x + x, MARE.y + y}; }

std::string scena() {
    const auto f = fs::temp_directory_path() / "m688_prova_pannello_interventi.lua";
    const auto pos = [](double x, double y) {
        const Vec2 p = in_mare(x, y);
        return "posizione = { x = " + std::to_string(static_cast<int>(p.x)) + ", y = " + std::to_string(static_cast<int>(p.y)) + " }";
    };
    std::ofstream(f) << "return { id = 'PROVA_PANNELLO', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false,"
                        " script = {}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, " << pos(0, -15000) << ","
                        "  quota_m = 200, rotta = 90, velocita_kn = 3, " << SUB << " },\n"
                        "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, " << pos(0, 0) << ", quota_m = 0, rotta = 0, velocita_kn = 10, " << NAVE << " },\n"
                        "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, " << pos(8000, 0) << ", quota_m = 150, rotta = 0, velocita_kn = 5, " << SUB << " },\n"
                        "}, timer = {} } } }\n";
    return f.string();
}

const std::vector<std::pair<std::string, std::string>> CLASSI = {{"perry", "Oliver H. Perry"}, {"spruance", "Spruance"}};

std::vector<UnitaPannello> unita() {
    return {{0, "LOS ANGELES (0)", "NATO", "sottomarino"}, {2, "PERRY (2)", "NATO", "nave"}, {3, "VICTOR III (3)", "PATTO", "sottomarino"}};
}

Comandi tasto(int tab = 0, bool invio = false, int ruota = 0) {
    Comandi c;
    c.tab = tab;
    c.invio = invio;
    c.ruota = ruota;
    return c;
}

// dal gruppo `g` al tipo `t` (Invio sul gruppo, Tab fino al tipo, Invio)
void scegli(PannelloInterventi& p, const std::vector<VocePannello>& cat, const std::string& g, const std::string& t) {
    std::vector<std::string> gruppi;
    for (const auto& v : cat)
        if (std::find(gruppi.begin(), gruppi.end(), v.gruppo) == gruppi.end()) gruppi.push_back(v.gruppo);
    const int ig = static_cast<int>(std::find(gruppi.begin(), gruppi.end(), g) - gruppi.begin());
    if (ig > 0) p.comandi(tasto(ig), 0.0);
    p.comandi(tasto(0, true), 0.0);
    int it = 0;
    for (const auto& v : cat) {
        if (v.gruppo != g) continue;
        if (v.tipo == t) break;
        ++it;
    }
    if (it > 0) p.comandi(tasto(it), 0.0);
    p.comandi(tasto(0, true), 0.0);
}

VocePannello voce(const std::vector<VocePannello>& cat, const std::string& t) {
    return *std::find_if(cat.begin(), cat.end(), [&](const VocePannello& v) { return v.tipo == t; });
}

}  // namespace

TEST_CASE("pannello degli interventi: le voci dal catalogo del motore") {
    const auto cat = catalogo_pannello(catalogo_interventi(), CLASSI);
    REQUIRE(cat.size() == catalogo_interventi().size());
    const VocePannello o = voce(cat, "ordine");
    CHECK(o.gruppo == "FORZE");
    CHECK(o.vuole_unita);
    CHECK(o.punto_facoltativo);
    CHECK(o.categorie == std::vector<std::string>{"nave", "sottomarino"});
    CHECK(std::none_of(o.parametri.begin(), o.parametri.end(), [](const ParametroPannello& p) { return p.nome == "gestore"; }));   // testo libero
    for (const auto& p : o.parametri) {   // facoltativi: si parte da «invariato», sotto il minimo
        CAPTURE(p.nome);
        CHECK(p.facoltativo);
        CHECK(p.valore < p.minimo);
    }
    const VocePannello n = voce(cat, "unita_nuova");
    REQUIRE(n.parametri.size() >= 2);
    CHECK(n.parametri[0].nome == "classe");   // prima i testi
    CHECK(n.parametri[0].scelte == std::vector<std::string>{"perry", "spruance"});
    CHECK(n.parametri[0].etichette.front() == "OLIVER H. PERRY");
    CHECK(n.parametri[1].nome == "parte");
    VoceCatalogoIntervento zona;   // con vuole_raggio il pannello aggiunge il raggio
    zona.tipo = "zona";
    zona.vuole_raggio = true;
    const auto z = catalogo_pannello({zona}, {});
    REQUIRE(z.size() == 1);
    CHECK(std::any_of(z[0].parametri.begin(), z[0].parametri.end(), [](const ParametroPannello& p) { return p.nome == "raggio_m"; }));
    CHECK_FALSE(voce(cat, "togli").ha_durata);
}

TEST_CASE("pannello degli interventi: un ordine senza punto, solo la velocita', accettato dalla missione") {
    const auto cat = catalogo_pannello(catalogo_interventi(), CLASSI);
    PannelloInterventi p(cat);
    p.apri(600.0, unita());
    scegli(p, cat, "FORZE", "ordine");
    p.comandi(tasto(1), 0.0);   // la Perry
    p.comandi(tasto(0, true), 0.0);
    CHECK(p.aspetta_punto());
    p.comandi(tasto(0, true), 0.0);   // senza punto
    CHECK_FALSE(p.aspetta_punto());
    p.comandi(tasto(1), 0.0);   // la velocita': da «invariato» a 15 nodi, un passo ogni 0,12 s
    for (int i = 0; i < 16; ++i) p.comandi(tasto(0, false, 1), 0.2);
    const auto q = [&] {
        p.comandi(tasto(4), 0.0);   // durata, fra, APPLICA
        return p.comandi(tasto(0, true), 0.0);
    }();
    REQUIRE(q);
    CHECK_FALSE(p.aperto());
    CHECK(q->tipo == "ordine");
    CHECK(q->unita == 2);
    CHECK_FALSE(q->punto);
    CHECK(q->parametri == std::map<std::string, double>{{"velocita_kn", 15.0}});
    CHECK(q->subito);
    CHECK(q->riga == "10:00 ORDINA A UN'UNITA': PERRY (2), VELOCITA' 15");
    const Intervento x = intervento_di(*q);
    CHECK(x.tempo == -1.0);
    Missione mis(scena(), 1, false);
    mis.acustica_enhanced(M688_DATI);
    mis.senza_giocatore(true);
    const auto e = mis.intervieni(x);
    CHECK(e.ok);
    mis.avanza(1.0 / 9.0);
    REQUIRE(mis.stati_interventi().size() == 1);
    CHECK(mis.stati_interventi()[0].applicato);
}

TEST_CASE("pannello degli interventi: un'unita' nuova col punto sulla carta, a tempo; Esc torna indietro") {
    const auto cat = catalogo_pannello(catalogo_interventi(), CLASSI);
    PannelloInterventi p(cat);
    p.apri(60.0, unita());
    scegli(p, cat, "FORZE", "unita_nuova");
    CHECK(p.aspetta_punto());   // niente unita' da scegliere: subito il punto
    Comandi esc;
    esc.esc = true;
    p.comandi(esc, 0.0);        // indietro ai tipi, e di nuovo avanti
    CHECK_FALSE(p.aspetta_punto());
    p.comandi(tasto(0, true), 0.0);
    REQUIRE(p.aspetta_punto());
    p.punto_scelto(in_mare(5000.0, -2000.0));
    // classe (perry), parte (usa), rotta, velocita', quota, fra: «fra» a 2 minuti
    p.comandi(tasto(5), 0.0);
    for (int i = 0; i < 2; ++i) p.comandi(tasto(0, false, 1), 0.2);
    p.comandi(tasto(1), 0.0);
    const auto q = p.comandi(tasto(0, true), 0.0);
    REQUIRE(q);
    CHECK(q->testi.at("classe") == "perry");
    CHECK(q->testi.at("parte") == "usa");
    REQUIRE(q->punto);
    CHECK(q->punto->x == doctest::Approx(in_mare(5000.0, 0.0).x));
    CHECK_FALSE(q->subito);
    CHECK(q->tempo == doctest::Approx(180.0));
    CHECK(q->durata_s == 0.0);   // senza durata
    Missione mis(scena(), 1, false);
    mis.acustica_enhanced(M688_DATI);
    mis.senza_giocatore(true);
    CHECK(mis.intervieni(intervento_di(*q)).ok);
    CHECK(mis.interventi().at(0).tempo == doctest::Approx(180.0));
}

TEST_CASE("pannello degli interventi: i segni nel vivo e nel replay (linea del tempo, carta, G al seguente)") {
    Missione mis(scena(), 1, false);
    mis.acustica_enhanced(M688_DATI);
    mis.senza_giocatore(true);
    Traccia vuota;
    vuota.missione = "PROVA_PANNELLO";
    vuota.seme = 1;
    SchermataReplay v(M688_DATI, vuota, "faroe");
    v.dal_vivo(true);
    Intervento o;   // l'ordine alla Perry con il «vai a», per 10 minuti
    o.tipo = "ordine";
    o.unita = 2;
    o.punto = in_mare(6000.0, 4000.0);
    o.durata_s = 600.0;
    REQUIRE(mis.intervieni(o).ok);
    Intervento x;   // il Victor tolto a 20 s
    x.tipo = "togli";
    x.unita = 3;
    x.tempo = 20.0;
    REQUIRE(mis.intervieni(x).ok);
    RegistratoreTraccia reg("PROVA_PANNELLO", 1);
    RaccoltaTraccia raccolta;
    for (int k = 0; k < 60 * 9; ++k) {
        mis.avanza(1.0 / 9.0);
        if (reg.serve(mis.tempo(), mis.eventi_missione().size())) {
            const Fotogramma f = fotogramma_da(mis, nullptr, DatiApp{}, reg.ultimo_evento(), &raccolta);
            reg.aggiungi(f, mis.eventi_missione().size());
            v.aggiungi(f);
        }
        v.adesso(mis.tempo(), 1.0, false);
    }
    REQUIRE(v.traccia().interventi.size() >= 2);
    v.vista(in_mare(3000.0, 2000.0), 40.0);
    Tela t;
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    v.disegna(t, f);
    const auto magenta = [&](int x0, int y0, int x1, int y1) {
        int n = 0;
        for (int y = y0; y < y1; ++y)
            for (int px = x0; px < x1; ++px) n += t.dati()[y * LARGHEZZA_TELA + px] == 13;
        return n;
    };
    const Rettangolo c = v.carta(), l = v.linea_tempo();
    CHECK(magenta(c.x, c.y, c.x + c.w, c.y + c.h) > 40);   // il «vai a», la lettera F, la croce del Victor tolto
    CHECK(magenta(l.x - 4, l.y - 8, l.x + l.w + 4, l.y) >= 6);   // i triangoli e la riga della durata
    if (const char* d = std::getenv("M688_ISTANTANEE")) {
        Palette pv = Palette::carica(M688_DATI "/immagini/MAIN.pal");
        v.palette(pv);
        t.salva_bmp(std::string(d) + "/vivo_segni_interventi.bmp", pv);
    }
    // l'avviso allo spettatore: sopra la carta finche' non scade coi dt di comandi_vivo
    v.avviso("intervento rifiutato: unita' assente", 2.0);
    CHECK(v.avviso_attuale() == "intervento rifiutato: unita' assente");
    Tela ta;
    v.disegna(ta, f);
    int giallo = 0;
    for (int y = c.y; y < c.y + 30; ++y)
        for (int px = c.x; px < c.x + c.w; ++px) giallo += ta.dati()[y * LARGHEZZA_TELA + px] == colori::GIALLO;
    CHECK(giallo > 50);
    if (const char* d = std::getenv("M688_ISTANTANEE")) {
        Palette pv = Palette::carica(M688_DATI "/immagini/MAIN.pal");
        v.palette(pv);
        ta.salva_bmp(std::string(d) + "/vivo_avviso.bmp", pv);
    }
    v.comandi_vivo(Comandi{}, 1.5);
    CHECK_FALSE(v.avviso_attuale().empty());
    v.comandi_vivo(Comandi{}, 1.0);
    CHECK(v.avviso_attuale().empty());
    // il replay della stessa traccia: G va al primo intervento, poi al secondo
    SchermataReplay rp(M688_DATI, leggi_traccia(reg.dati()), "faroe");
    REQUIRE(rp.traccia().interventi.size() >= 2);
    Comandi g;
    g.lettere = {'G'};
    rp.comandi(g, 0.0);
    const double primo = rp.tempo();
    CHECK(primo > 0.0);
    CHECK(primo < 2.0);
    rp.comandi(g, 0.0);
    CHECK(rp.tempo() == doctest::Approx(20.0).epsilon(0.02));
}

// D3: il falso contatto entra nella traccia (un'unita' «arma» senza lanciatore, id 990000-994999) e lo spettatore lo
// vede: nella verita' la classe imitata col segno dell'inganno e la scritta FALSO; nel quadro della parte che lo sente
// il segno e, se la posizione e' stimata, la doppia freccia dalla sorgente vera a dove la si crede
TEST_CASE("pannello degli interventi: il falso contatto nella traccia e nella vista dello spettatore") {
    Missione mis(scena(), 1, false);
    mis.acustica_enhanced(M688_DATI);
    mis.senza_giocatore(true);
    Intervento x;   // un falso Victor davanti alla Perry, che pinga ogni 30 s
    x.tipo = "contatto_falso";
    x.punto = in_mare(3000.0, 3000.0);
    x.testi = {{"classe", "victor3"}, {"parte", "urss"}};
    x.parametri = {{"velocita_kn", 6.0}, {"quota_m", 150.0}, {"ping_s", 30.0}};
    x.durata_s = 1800.0;
    REQUIRE(mis.intervieni(x).ok);
    RegistratoreTraccia reg("PROVA_PANNELLO", 1);
    RaccoltaTraccia raccolta;
    for (int k = 0; k < 90 * 9; ++k) {
        mis.avanza(1.0 / 9.0);
        if (reg.serve(mis.tempo(), mis.eventi_missione().size()))
            reg.aggiungi(fotogramma_da(mis, nullptr, DatiApp{}, reg.ultimo_evento(), &raccolta), mis.eventi_missione().size());
    }
    int id = -1;
    for (const auto& [k, u] : mis.unita())
        if (mis.contatto_falso(k)) id = k;
    REQUIRE(id >= 990000);
    REQUIRE(mis.unita().at(2).contatti.count(id) == 1);   // la Perry lo sente
    SchermataReplay rp(M688_DATI, leggi_traccia(reg.dati()), "faroe");
    const auto it = rp.traccia().unita.find(id);
    REQUIRE(it != rp.traccia().unita.end());
    REQUIRE_FALSE(it->second.empty());
    CHECK(it->second.back().second.categoria == "arma");
    CHECK(it->second.back().second.classe == "victor3");
    CHECK(it->second.back().second.lanciatore < 0);
    const QuadroParte* qp = rp.traccia().quadro_parte(0x08, rp.traccia().durata);
    REQUIRE(qp != nullptr);
    CHECK(std::any_of(qp->voci.begin(), qp->voci.end(), [&](const VoceParte& v) { return v.bersaglio == id; }));
    rp.vai(rp.traccia().durata);
    rp.vista(in_mare(3000.0, 2000.0), 40.0);
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    const auto inganno = simboli::indice_famiglia(simboli::FamigliaIntervento::inganno);
    const auto conta = [&](SchermataReplay::Lettura l, const char* nome) {
        rp.lettura(l);
        Tela t;
        rp.disegna(t, f);
        if (const char* d = std::getenv("M688_ISTANTANEE")) {
            Palette pv = Palette::carica(M688_DATI "/immagini/MAIN.pal");
            rp.palette(pv);
            t.salva_bmp(std::string(d) + "/falso_" + nome + ".bmp", pv);
        }
        const Rettangolo c = rp.carta();
        int n = 0;
        for (int y = c.y; y < c.y + c.h; ++y)
            for (int px = c.x; px < c.x + c.w; ++px) n += t.dati()[y * LARGHEZZA_TELA + px] == inganno;
        return n;
    };
    const int fondo = conta(SchermataReplay::Lettura::quadro, "quadro");   // il giocatore non lo sente: nessun segno
    CHECK(conta(SchermataReplay::Lettura::verita, "verita") > fondo + 30);        // il segno e FALSO VICTOR III
    CHECK(conta(SchermataReplay::Lettura::parte_nato, "parte_nato") > fondo + 20);   // il segno sul contatto della Perry
}

// D3, le comunicazioni per lo spettatore: il disturbo senza zona e l'ordine falso in alto a sinistra sulla carta, come
// ogni unita' che l'ha ricevuto ha preso l'ordine (dal registro, reazione_ia «ordine_...») sotto il suo simbolo
TEST_CASE("pannello degli interventi: il disturbo radio e l'ordine falso nella vista dello spettatore") {
    Missione mis(scena(), 1, false);
    mis.acustica_enhanced(M688_DATI);
    mis.senza_giocatore(true);
    mis.forze_enhanced(M688_DATI);   // gli ordini falsi passano dalle forze (in partita le accende DanniMissione)
    Intervento d;   // il disturbo su tutta la parte del Patto
    d.tipo = "disturbo_radio";
    d.testi = {{"parte", "urss"}};
    d.parametri = {{"perdita", 0.5}, {"ritardo", 2.0}};
    d.durata_s = 1800.0;
    REQUIRE(mis.intervieni(d).ok);
    Intervento o;   // il falso rientro alla NATO
    o.tipo = "ordine_falso";
    o.testi = {{"parte", "usa"}, {"ordine", "rientro"}};
    o.parametri = {{"autenticazione", 0.5}};
    o.durata_s = 1800.0;
    REQUIRE(mis.intervieni(o).ok);
    RegistratoreTraccia reg("PROVA_PANNELLO", 1);
    RaccoltaTraccia raccolta;
    for (int k = 0; k < 150 * 9; ++k) {
        mis.avanza(1.0 / 9.0);
        if (reg.serve(mis.tempo(), mis.eventi_missione().size()))
            reg.aggiungi(fotogramma_da(mis, nullptr, DatiApp{}, reg.ultimo_evento(), &raccolta), mis.eventi_missione().size());
    }
    SchermataReplay rp(M688_DATI, leggi_traccia(reg.dati()), "faroe");
    const auto& registro = rp.traccia().registro;
    const auto reazione = std::find_if(registro.begin(), registro.end(), [](const EventoTraccia& e) {
        return e.tipo == "reazione_ia" && e.unita == 2 && e.classe_arma.rfind("ordine_", 0) == 0;
    });
    REQUIRE(reazione != registro.end());   // la Perry l'ha ricevuto dalla rete
    MESSAGE("la Perry: ", reazione->classe_arma);
    rp.vai(rp.traccia().durata);
    rp.lettura(SchermataReplay::Lettura::verita);
    rp.vista(in_mare(3000.0, 2000.0), 40.0);
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    Tela t;
    rp.disegna(t, f);
    if (const char* dir = std::getenv("M688_ISTANTANEE")) {
        Palette pv = Palette::carica(M688_DATI "/immagini/MAIN.pal");
        rp.palette(pv);
        t.salva_bmp(std::string(dir) + "/d3_comunicazioni.bmp", pv);
    }
    const Rettangolo c = rp.carta();
    int bianco = 0;   // le due righe in alto a sinistra, nel bianco delle comunicazioni
    for (int y = c.y + 4; y < c.y + 32; ++y)
        for (int x = c.x + 4; x < c.x + c.w / 2; ++x) bianco += t.dati()[y * LARGHEZZA_TELA + x] == colori::BIANCO;
    CHECK(bianco > 150);
}

// D3, l'inganno per lo spettatore: lo scoppio provocato (la stella sul punto, per due minuti), il siluro «di nessuno»
// col segno dell'inganno e la parte creduta, la spia in alto a sinistra sulla carta; la false flag della D4 (il siluro
// del Victor con la parte della NATO)
TEST_CASE("pannello degli interventi: l'incidente provocato e la spia nella vista dello spettatore") {
    Missione mis(scena(), 1, false);
    mis.acustica_enhanced(M688_DATI);
    mis.senza_giocatore(true);
    mis.forze_enhanced(M688_DATI);    // la spia passa dalle forze
    mis.siluri_enhanced(M688_DATI);   // il siluro di nessuno ha il modello dei siluri
    mis.avanza(1.0);
    Intervento s;   // il Patto vede il quadro vero della NATO
    s.tipo = "spia";
    s.testi = {{"parte", "urss"}, {"altra", "usa"}};
    s.durata_s = 1800.0;
    REQUIRE(mis.intervieni(s).ok);
    Intervento b;   // lo scoppio a nord della Perry
    b.tipo = "incidente";
    b.punto = in_mare(2000.0, 5000.0);
    b.testi = {{"incidente", "scoppio"}};
    b.parametri = {{"kg", 300.0}, {"quota_m", 40.0}};
    REQUIRE(mis.intervieni(b).ok);
    Intervento x;   // il siluro «sovietico» di nessuno sulla Perry, da 4 km a est
    x.tipo = "incidente";
    x.unita = 2;
    x.punto = in_mare(4000.0, 1000.0);
    x.testi = {{"incidente", "siluro"}, {"parte", "urss"}};
    x.parametri = {{"quota_m", 20.0}};
    REQUIRE(mis.intervieni(x).ok);
    Intervento ff;   // la false flag (D4): il Victor lancia sulla Perry con la parte della NATO
    ff.tipo = "false_flag";
    ff.unita = 3;
    ff.testi = {{"parte", "usa"}};
    ff.parametri = {{"bersaglio", 2.0}};
    REQUIRE(mis.intervieni(ff).ok);
    RegistratoreTraccia reg("PROVA_PANNELLO", 1);
    RaccoltaTraccia raccolta;
    for (int k = 0; k < 40 * 9; ++k) {
        mis.avanza(1.0 / 9.0);
        if (reg.serve(mis.tempo(), mis.eventi_missione().size()))
            reg.aggiungi(fotogramma_da(mis, nullptr, DatiApp{}, reg.ultimo_evento(), &raccolta), mis.eventi_missione().size());
    }
    SchermataReplay rp(M688_DATI, leggi_traccia(reg.dati()), "faroe");
    REQUIRE(rp.traccia().interventi.size() >= 4);
    bool travestito = false;   // il siluro della false flag nella traccia: lanciato dal Victor, con la parte della NATO
    for (const auto& [id, serie] : rp.traccia().unita)
        travestito = travestito || (!serie.empty() && serie.back().second.lanciatore == 3 && (serie.back().second.flag & 0x18) == 0x08);
    CHECK(travestito);
    rp.vai(rp.traccia().durata);
    rp.lettura(SchermataReplay::Lettura::verita);
    rp.vista(in_mare(2000.0, 2000.0), 30.0);
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    Tela t;
    rp.disegna(t, f);
    if (const char* dir = std::getenv("M688_ISTANTANEE")) {
        Palette pv = Palette::carica(M688_DATI "/immagini/MAIN.pal");
        rp.palette(pv);
        t.salva_bmp(std::string(dir) + "/d3_inganno.bmp", pv);
    }
    const auto inganno = simboli::indice_famiglia(simboli::FamigliaIntervento::inganno);
    const Rettangolo c = rp.carta();
    int alto = 0, carta = 0;
    for (int y = c.y; y < c.y + c.h; ++y)
        for (int px = c.x; px < c.x + c.w; ++px) {
            const bool m = t.dati()[y * LARGHEZZA_TELA + px] == inganno;
            carta += m;
            alto += m && y < c.y + 32 && px < c.x + c.w / 2;
        }
    CHECK(alto > 80);            // la spia e l'incidente, in alto a sinistra
    CHECK(carta > alto + 100);   // la stella dello scoppio, il siluro di nessuno, le scritte
}

// il cambio di parte (D4) per lo spettatore: la Perry passa al Patto. La NATO la crede ancora sua finche' non lo sa, e la
// scritta se ne va quando lo sanno tutte e due le parti
TEST_CASE("pannello degli interventi: l'etichetta CREDUTA NATO del cambio di parte") {
    Missione mis(scena(), 1, false);
    mis.acustica_enhanced(M688_DATI);
    mis.senza_giocatore(true);
    mis.forze_enhanced(M688_DATI);
    mis.avanza(1.0);
    RegistratoreTraccia reg("PROVA_PANNELLO", 1);
    RaccoltaTraccia raccolta;
    const auto gira = [&](int secondi) {
        for (int k = 0; k < secondi * 9; ++k) {
            mis.avanza(1.0 / 9.0);
            if (reg.serve(mis.tempo(), mis.eventi_missione().size()))
                reg.aggiungi(fotogramma_da(mis, nullptr, DatiApp{}, reg.ultimo_evento(), &raccolta), mis.eventi_missione().size());
        }
    };
    gira(10);
    Intervento x;
    x.tipo = "cambia_parte";
    x.unita = 2;
    x.testi = {{"parte", "urss"}};
    REQUIRE(mis.intervieni(x).ok);
    gira(20);
    Traccia tr = leggi_traccia(reg.dati());
    const auto cambi = cambi_creduti(tr, tr.durata);
    REQUIRE(cambi.count(2) == 1);
    const CambioCreduto& c = cambi.at(2);
    CHECK(c.prima == 0x08);
    CHECK(c.nuova == 0x10);
    CHECK(c.sa_patto);                 // la parte nuova lo sa subito
    CHECK_FALSE(c.sa_nato);
    CHECK(etichetta_creduta(c) == "CREDUTA NATO");
    CHECK(parte_creduta_da(cambi, 2, 0x08, -1, 0x10) == 0x08);
    CHECK(parte_creduta_da(cambi, 2, 0x10, -1, 0x10) == 0x10);
    CHECK(parte_creduta_da(cambi, 3, 0x08, -1, 0x10) == 0x10);   // chi non ha cambiato parte: la sua

    // chi attacca e' scoperto: la vittima prima della sua parte, poi tutta la parte, e la scritta se ne va
    EventoTraccia e;
    e.tipo = "reazione_ia";
    e.tempo = tr.durata;
    e.unita = 7;
    e.altra = 2;
    e.classe_arma = "cambio_saputo";
    tr.registro.push_back(e);
    auto dopo = cambi_creduti(tr, tr.durata);
    CHECK(parte_creduta_da(dopo, 2, 0x08, 7, 0x10) == 0x10);
    CHECK(parte_creduta_da(dopo, 2, 0x08, 8, 0x10) == 0x08);
    e.unita = 2;
    e.altra = -1;
    e.classe_arma = "cambio_saputo_usa";
    tr.registro.push_back(e);
    dopo = cambi_creduti(tr, tr.durata);
    CHECK(dopo.at(2).sa_nato);
    CHECK(parte_creduta_da(dopo, 2, 0x08, -1, 0x10) == 0x10);
    CHECK(etichetta_creduta(dopo.at(2)).empty());
    // un annuncio solo ai vecchi: restano ingannati gli altri
    CambioCreduto solo;
    solo.prima = 0x08;
    solo.nuova = 0x10;
    solo.sa_nato = true;
    CHECK(etichetta_creduta(solo) == "CREDUTA NATO DAL PATTO");

    // sulla carta: i pixel dell'inganno prima del cambio e dopo. L'evento aggiunto a mano non e' nella traccia del replay
    SchermataReplay rp(M688_DATI, leggi_traccia(reg.dati()), "faroe");
    rp.lettura(SchermataReplay::Lettura::verita);
    rp.vista(in_mare(0.0, 0.0), 30.0);
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    const auto inganno = simboli::indice_famiglia(simboli::FamigliaIntervento::inganno);
    const auto pixel = [&](double quando, const char* nome) {
        rp.vai(quando);
        Tela t;
        rp.disegna(t, f);
        if (const char* dir = std::getenv("M688_ISTANTANEE")) {
            Palette pv = Palette::carica(M688_DATI "/immagini/MAIN.pal");
            rp.palette(pv);
            t.salva_bmp(std::string(dir) + "/" + nome + ".bmp", pv);
        }
        const Rettangolo r = rp.carta();
        int n = 0;
        for (int y = r.y; y < r.y + r.h; ++y)
            for (int px = r.x; px < r.x + r.w; ++px) n += t.dati()[y * LARGHEZZA_TELA + px] == inganno;
        return n;
    };
    const int prima = pixel(5.0, "d4_creduta_prima");
    const int dopo_cambio = pixel(rp.traccia().durata, "d4_creduta_nato");
    CHECK(dopo_cambio > prima + 30);
}

// i segni dell'ambiente della D4 sulla carta del replay: la zona con la sua scritta, cio' che vale per tutto il teatro in alto
TEST_CASE("pannello degli interventi: strato, ghiaccio e polynya sulla carta del replay") {
    Missione mis(scena(), 1, false);
    mis.acustica_enhanced(M688_DATI);
    mis.senza_giocatore(true);
    RegistratoreTraccia reg("PROVA_PANNELLO", 1);
    RaccoltaTraccia raccolta;
    for (int k = 0; k < 40 * 9; ++k) {
        mis.avanza(1.0 / 9.0);
        if (reg.serve(mis.tempo(), mis.eventi_missione().size()))
            reg.aggiungi(fotogramma_da(mis, nullptr, DatiApp{}, reg.ultimo_evento(), &raccolta), mis.eventi_missione().size());
    }
    Traccia tr = leggi_traccia(reg.dati());
    const auto voce = [](int id, const char* tipo, std::map<std::string, std::string> k, double raggio, std::optional<Vec2> punto) {
        RecordIntervento x;
        x.id = id;
        x.t = 10.0;
        x.fase = RecordIntervento::FASE_APPLICATO;
        x.tipo = tipo;
        x.chiavi = codifica_chiavi(k);
        x.raggio_m = raggio;
        if (punto) x.ha_punto = true, x.punto = *punto;
        return x;
    };
    const RecordIntervento strato = voce(1, "strato", {{"modifica", "sposta"}, {"#quota_m", "120"}, {"#transizione_s", "60"}}, 6000.0, in_mare(-6000.0, 4000.0));
    const RecordIntervento deriva = voce(2, "ghiaccio_deriva", {{"#rotta", "90"}, {"#velocita_kn", "0.5"}}, 0.0, std::nullopt);
    const RecordIntervento polynya = voce(3, "polynya", {{"azione", "chiudi"}}, 3000.0, in_mare(6000.0, -4000.0));
    CHECK(riga_ambiente(strato, 40.0) == "STRATO A 120 M IN CORSO");   // a meta' della transizione
    CHECK(riga_ambiente(strato, 70.0) == "STRATO A 120 M");
    CHECK(riga_ambiente(voce(4, "strato", {{"modifica", "rinforza"}, {"#fattore", "2.5"}}, 0.0, std::nullopt), 70.0) == "STRATO X2,5");
    CHECK(riga_ambiente(voce(5, "strato", {{"modifica", "togli"}}, 0.0, std::nullopt), 70.0) == "STRATO TOLTO");
    CHECK(riga_ambiente(voce(6, "strato", {{"modifica", "canale"}, {"#quota_m", "600"}}, 0.0, std::nullopt), 70.0) == "CANALE A 600 M");
    CHECK(riga_ambiente(deriva, 70.0) == "GHIACCIO VERSO 090 A 0,5 KN");
    CHECK(riga_ambiente(polynya, 70.0) == "POLYNYA SI CHIUDE");
    CHECK(riga_ambiente(voce(7, "spia", {}, 0.0, std::nullopt), 70.0).empty());

    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    const auto ambiente = simboli::indice_famiglia(simboli::FamigliaIntervento::ambiente);
    const auto pixel = [&](const Traccia& traccia, int& alto) {
        SchermataReplay rp(M688_DATI, traccia, "faroe");
        rp.lettura(SchermataReplay::Lettura::verita);
        rp.vista(in_mare(0.0, 0.0), 30.0);
        rp.vai(traccia.durata);
        Tela t;
        rp.disegna(t, f);
        if (const char* dir = std::getenv("M688_ISTANTANEE"); dir && !traccia.interventi.empty()) {
            Palette pv = Palette::carica(M688_DATI "/immagini/MAIN.pal");
            rp.palette(pv);
            t.salva_bmp(std::string(dir) + "/d4_ambiente.bmp", pv);
        }
        const Rettangolo c = rp.carta();
        int n = 0;
        alto = 0;
        for (int y = c.y; y < c.y + c.h; ++y)
            for (int px = c.x; px < c.x + c.w; ++px) {
                const bool m = t.dati()[y * LARGHEZZA_TELA + px] == ambiente;
                n += m;
                alto += m && y < c.y + 20 && px < c.x + c.w / 2;
            }
        return n;
    };
    int alto_senza = 0, alto_con = 0;
    const int senza = pixel(tr, alto_senza);
    tr.interventi = {strato, deriva, polynya};
    const int con = pixel(tr, alto_con);
    CHECK(alto_con > alto_senza + 40);   // la deriva del pack, in alto a sinistra
    CHECK(con > senza + alto_con + 80);  // le due zone, coi cerchi e le scritte
}

// la false flag (D4): chi attacca, poi il bersaglio dalla lista delle unita' (senza chi attacca), non un id da far scorrere
TEST_CASE("pannello degli interventi: il bersaglio della false flag si sceglie dalla lista delle unita'") {
    const auto cat = catalogo_pannello(catalogo_interventi(), CLASSI);
    const VocePannello ff = voce(cat, "false_flag");
    CHECK(ff.vuole_bersaglio);
    for (const auto& q : ff.parametri) CHECK(q.nome != "bersaglio");
    PannelloInterventi p(cat);
    p.apri(0.0, unita());
    scegli(p, cat, "INGANNO", "false_flag");
    p.comandi(tasto(2), 0.0);              // il Victor (3), terzo della lista
    p.comandi(tasto(0, true), 0.0);
    p.comandi(tasto(1), 0.0);              // fra i bersagli il Victor non c'e': la seconda voce e' la Perry (2)
    p.comandi(tasto(0, true), 0.0);
    Comandi esc;
    esc.esc = true;
    p.comandi(esc, 0.0);                   // Esc torna al bersaglio, e di nuovo avanti
    p.comandi(tasto(1), 0.0);
    p.comandi(tasto(0, true), 0.0);
    p.comandi(tasto(2), 0.0);              // la parte, «quando», APPLICA
    const auto q = p.comandi(tasto(0, true), 0.0);
    REQUIRE(q);
    CHECK(q->unita == 3);
    CHECK(q->parametri.at("bersaglio") == doctest::Approx(2.0));
    CHECK(q->riga.find("VICTOR III (3) SU PERRY (2)") != std::string::npos);
    Missione mis(scena(), 1, false);
    mis.acustica_enhanced(M688_DATI);
    mis.senza_giocatore(true);
    mis.siluri_enhanced(M688_DATI);
    mis.avanza(1.0);
    REQUIRE(mis.intervieni(intervento_di(*q)).ok);
    mis.avanza(1.0);
    REQUIRE_FALSE(mis.stati_interventi().empty());
    CHECK(mis.stati_interventi().back().applicato);
    CHECK_FALSE(mis.stati_interventi().back().fallito);
}

// i due eventi del registro per lo spettatore: il messaggio perso nel disturbo, la localizzazione della rete strategica
// (unita -1: niente linea, il segno sull'unita' localizzata). Gli eventi nella traccia, per non dipendere dal caso
TEST_CASE("pannello degli interventi: il messaggio perso e il DF della rete strategica sulla carta") {
    Missione mis(scena(), 1, false);
    mis.acustica_enhanced(M688_DATI);
    mis.senza_giocatore(true);
    RegistratoreTraccia reg("PROVA_PANNELLO", 1);
    RaccoltaTraccia raccolta;
    for (int k = 0; k < 20 * 9; ++k) {
        mis.avanza(1.0 / 9.0);
        if (reg.serve(mis.tempo(), mis.eventi_missione().size()))
            reg.aggiungi(fotogramma_da(mis, nullptr, DatiApp{}, reg.ultimo_evento(), &raccolta), mis.eventi_missione().size());
    }
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    const auto disegna = [&](bool con_eventi, Tela& t) {
        Traccia tr = leggi_traccia(reg.dati());
        if (con_eventi) {
            EventoTraccia perso;
            perso.tempo = tr.durata - 1.0;
            perso.tipo = "reazione_ia";
            perso.classe_arma = "messaggio_perso";
            perso.unita = 2;
            tr.registro.push_back(perso);
            EventoTraccia rete = perso;
            rete.classe_arma = "localizza";
            rete.unita = -1;
            rete.altra = 3;
            tr.registro.push_back(rete);
        }
        SchermataReplay rp(M688_DATI, std::move(tr), "faroe");
        rp.vai(rp.traccia().durata);
        rp.lettura(SchermataReplay::Lettura::verita);
        rp.vista(in_mare(3000.0, 2000.0), 40.0);
        rp.disegna(t, f);
        return rp.carta();
    };
    Tela senza, con;
    disegna(false, senza);
    const Rettangolo c = disegna(true, con);
    const auto conta = [&](const Tela& t, int col) {
        int n = 0;
        for (int y = c.y; y < c.y + c.h; ++y)
            for (int x = c.x; x < c.x + c.w; ++x) n += t.dati()[y * LARGHEZZA_TELA + x] == col;
        return n;
    };
    CHECK(conta(con, colori::GIALLO) > conta(senza, colori::GIALLO) + 30);   // DF DELLA RETE
    CHECK(conta(con, colori::GRIGIO) > conta(senza, colori::GRIGIO) + 30);   // MESSAGGIO PERSO
}

// la false flag fino in fondo: il siluro del Victor con la parte della NATO colpisce la Perry; il registro dice a chi la
// parte colpita da' la colpa, e la carta lo scrive sotto l'unita' colpita per due minuti
TEST_CASE("pannello degli interventi: il colpo della false flag e la colpa sotto l'unita' colpita") {
    const auto classi = carica_classi(M688_DATI "/classi.toml");
    Missione mis(scena(), 1, false);
    mis.acustica_enhanced(M688_DATI);
    mis.senza_giocatore(true);
    DanniMissione dm(classi, M688_DATI, 1, RegolaDanni::simulazione);
    const auto avanti = [&] {
        mis.avanza(1.0 / 9.0);
        dm.avanza(mis, 1.0 / 9.0);
        mis.raccogli_danni(nullptr, &dm);
    };
    for (int k = 0; k < 9 * 30; ++k) avanti();   // il tempo di vedersi e sentirsi
    Intervento ff;
    ff.tipo = "false_flag";
    ff.unita = 3;
    ff.testi = {{"parte", "usa"}, {"arma", "mk48"}};
    ff.parametri = {{"bersaglio", 2.0}};
    REQUIRE(mis.intervieni(ff).ok);
    RegistratoreTraccia reg("PROVA_PANNELLO", 1);
    RaccoltaTraccia raccolta;
    bool colpa = false;
    for (int k = 0; k < 9 * 900 && !colpa; ++k) {
        avanti();
        if (reg.serve(mis.tempo(), mis.eventi_missione().size()))
            reg.aggiungi(fotogramma_da(mis, nullptr, DatiApp{}, reg.ultimo_evento(), &raccolta), mis.eventi_missione().size());
        for (const auto& q : mis.reazioni_ia()) colpa = colpa || q.evento == "false_flag";
    }
    REQUIRE(colpa);   // il siluro ha colpito
    for (int k = 0; k < 9 * 5; ++k) {
        avanti();
        if (reg.serve(mis.tempo(), mis.eventi_missione().size()))
            reg.aggiungi(fotogramma_da(mis, nullptr, DatiApp{}, reg.ultimo_evento(), &raccolta), mis.eventi_missione().size());
    }
    reg.aggiungi(fotogramma_da(mis, nullptr, DatiApp{}, reg.ultimo_evento(), &raccolta), mis.eventi_missione().size());
    SchermataReplay rp(M688_DATI, leggi_traccia(reg.dati()), "faroe");
    const auto& registro = rp.traccia().registro;
    const auto e = std::find_if(registro.begin(), registro.end(), [](const EventoTraccia& x) {
        return x.tipo == "reazione_ia" && x.classe_arma.rfind("attribuisce_", 0) == 0;
    });
    REQUIRE(e != registro.end());
    MESSAGE("la colpa: ", e->classe_arma, ", unita' ", e->unita, " a ", e->tempo, " s");
    rp.vai(rp.traccia().durata);
    rp.lettura(SchermataReplay::Lettura::verita);
    const StatoUnita* colpita = nullptr;
    const IstanteTraccia i = rp.traccia().istante(rp.tempo());
    for (const auto& u : i.unita)
        if (u.id == e->unita) colpita = &u;
    REQUIRE(colpita != nullptr);
    rp.vista(colpita->pos, 30.0);
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    Tela t;
    rp.disegna(t, f);
    if (const char* dir = std::getenv("M688_ISTANTANEE")) {
        Palette pv = Palette::carica(M688_DATI "/immagini/MAIN.pal");
        rp.palette(pv);
        t.salva_bmp(std::string(dir) + "/d4_colpa.bmp", pv);
    }
    const auto inganno = simboli::indice_famiglia(simboli::FamigliaIntervento::inganno);
    const Rettangolo c = rp.carta();
    int scritta = 0;   // la riga sotto l'unita' colpita, al centro della carta: DA' LA COLPA A ...
    const int yc = c.y + c.h / 2, xc = c.x + c.w / 2;
    for (int y = yc + 14; y < yc + 28; ++y)
        for (int x = xc - 14; x < xc + 110; ++x) scritta += t.dati()[y * LARGHEZZA_TELA + x] == inganno;
    CHECK(scritta > 40);
}
