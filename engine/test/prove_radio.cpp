#include <doctest.h>

#include <algorithm>
#include <cmath>

#include "nucleo/radio.hpp"

using namespace m688;

// Sala radio dell'Enhanced (pezzo S4): dossier docs/ricerca/08_comunicazioni_radio.md §9.

namespace {

constexpr double DT = 1.0;

ParametriRadio senza_fondo() {
    ParametriRadio p;
    p.fondo_ogni_s = 1e12;   // niente traffico di fondo: solo i messaggi della prova
    return p;
}

MessaggioRadio messaggio(Precedenza p, const char* oggetto = "ORDINI") {
    MessaggioRadio m;
    m.precedenza = p;
    m.oggetto = oggetto;
    m.testo = {"Nuova zona di pattugliamento.", "NEW PATROL AREA."};
    m.caratteri = 300;   // 60 s di acquisizione piu' 50 s in onda
    return m;
}

StatoAntenne albero(double quota = 10.0, double kn = 5.0) {
    StatoAntenne s;
    s.quota_m = quota;
    s.velocita_kn = kn;
    s.albero = true;
    return s;
}

StatoAntenne profondo(double quota = 200.0, double kn = 5.0) {
    StatoAntenne s;
    s.quota_m = quota;
    s.velocita_kn = kn;
    return s;
}

void avanza(SalaRadio& r, double& t, double fino, const StatoAntenne& s) {
    for (; t < fino; t += DT) r.avanza(t, DT, s);
}

bool c_e(const std::vector<EventoRadio>& v, EventoRadio::Tipo tipo) {
    return std::any_of(v.begin(), v.end(), [&](const EventoRadio& e) { return e.tipo == tipo; });
}

}  // namespace

TEST_CASE("radio: gruppo data-ora dall'ora e dalla data della missione") {
    SalaRadio r(LatoRadio::usa, 1, 15.5 * 3600.0, 24, 10, 1989);
    CHECK(r.dtg(0.0) == "241530Z OCT 89");
    CHECK(r.dtg(9.0 * 3600.0) == "250030Z OCT 89");
    SalaRadio fine_mese(LatoRadio::usa, 1, 23.0 * 3600.0, 31, 12, 1989);
    CHECK(fine_mese.dtg(2.0 * 3600.0) == "010100Z JAN 90");
}

TEST_CASE("radio: con l'albero alzato il messaggio si riceve per intero, e prima i FLASH") {
    SalaRadio r(LatoRadio::usa, 1, 0.0, 24, 10, 1989, senza_fondo());
    r.programma(messaggio(Precedenza::routine, "WX"), 0.0);
    const int flash = r.programma(messaggio(Precedenza::flash, "ATTACCO"), 0.0);
    double t = 0.0;
    avanza(r, t, 30.0, albero());
    REQUIRE(r.in_onda());
    CHECK(r.in_onda()->serie == flash);              // il FLASH passa avanti
    CHECK(r.progresso() > 0.2);
    avanza(r, t, 300.0, albero());
    REQUIRE(r.ricevuti().size() == 2);
    CHECK(r.ricevuti()[0].messaggio.serie == flash);
    CHECK(r.ricevuti()[0].messaggio.dtg == "240000Z OCT 89");
    const auto ev = r.eventi();
    CHECK(c_e(ev, EventoRadio::Tipo::in_arrivo));
    CHECK(c_e(ev, EventoRadio::Tipo::ricevuto));
    CHECK(r.nuovi().size() == 2);
    CHECK(r.persi().empty());
}

TEST_CASE("radio: in profondita' senza antenne si perde; la ripetizione del ciclo dopo si puo' ricevere") {
    SalaRadio r(LatoRadio::usa, 1, 0.0, 24, 10, 1989, senza_fondo());
    const int routine = r.programma(messaggio(Precedenza::routine), 0.0);    // una sola messa in onda
    const int priority = r.programma(messaggio(Precedenza::priority), 0.0);  // due, a due ore
    double t = 0.0;
    avanza(r, t, 600.0, profondo());
    CHECK(r.ricevuti().empty());
    CHECK(r.ricezione(Banda::vlf) == Ricezione::no);
    // alla seconda messa in onda del PRIORITY il battello e' all'albero
    avanza(r, t, 7200.0, profondo());
    avanza(r, t, 7600.0, albero());
    REQUIRE(r.ricevuti().size() == 1);
    CHECK(r.ricevuti()[0].messaggio.serie == priority);
    // il ROUTINE non torna: si vede il buco nella serie
    const auto persi = r.persi();
    REQUIRE(persi.size() == 1);
    CHECK(persi[0] == routine);
    CHECK(c_e(r.eventi(), EventoRadio::Tipo::perso));
}

TEST_CASE("radio: filo galleggiante, VLF fino a 120 m e 8 nodi, al 50% fino a 12, cieco dopo le virate, si rompe a 20") {
    SalaRadio r(LatoRadio::usa, 3, 0.0, 24, 10, 1989, senza_fondo());
    double t = 0.0;
    r.cala_filo(t);
    avanza(r, t, 60.0, profondo(100.0, 6.0));
    CHECK(r.filo_in_filata());
    CHECK(r.ricezione(Banda::vlf) == Ricezione::no);
    avanza(r, t, 130.0, profondo(100.0, 6.0));
    CHECK(r.filo());
    CHECK(r.ricezione(Banda::vlf) == Ricezione::si);
    CHECK(r.ricezione(Banda::hf) == Ricezione::no);       // solo VLF
    avanza(r, t, 131.0, profondo(100.0, 10.0));
    CHECK(r.ricezione(Banda::vlf) == Ricezione::meta);
    avanza(r, t, 132.0, profondo(100.0, 14.0));
    CHECK(r.ricezione(Banda::vlf) == Ricezione::no);
    avanza(r, t, 133.0, profondo(150.0, 6.0));
    CHECK(r.ricezione(Banda::vlf) == Ricezione::no);      // troppo in basso
    StatoAntenne vira = profondo(100.0, 6.0);
    vira.rateo_virata_deg_s = 1.0;
    avanza(r, t, 140.0, vira);
    avanza(r, t, 200.0, profondo(100.0, 6.0));
    CHECK(r.ricezione(Banda::vlf) == Ricezione::no);      // due minuti dopo la virata
    avanza(r, t, 300.0, profondo(100.0, 6.0));
    CHECK(r.ricezione(Banda::vlf) == Ricezione::si);
    r.eventi();
    avanza(r, t, 3000.0, profondo(100.0, 25.0));
    CHECK(r.filo_rotto());
    CHECK(c_e(r.eventi(), EventoRadio::Tipo::filo_rotto));
    r.cala_filo(t);
    CHECK(r.filo_rotto());                                // non si ricala
}

TEST_CASE("radio: la richiesta al satellite scarica tutto, persi compresi; oltre 72 gradi niente satellite") {
    SalaRadio r(LatoRadio::usa, 1, 0.0, 24, 10, 1989, senza_fondo());
    r.programma(messaggio(Precedenza::routine), 0.0);
    r.programma(messaggio(Precedenza::routine), 0.0);
    double t = 0.0;
    avanza(r, t, 1000.0, profondo());
    CHECK(r.ricevuti().empty());
    StatoAntenne nord = albero();
    nord.latitudine = 75.0;
    const auto no = r.trasmetti(TipoTrasmissione::richiesta, t, nord, {});
    CHECK(no.fatta);
    CHECK(no.banda == Banda::hf);
    CHECK(c_e(r.eventi(), EventoRadio::Tipo::satellite_assente));
    const auto si = r.trasmetti(TipoTrasmissione::richiesta, t, albero(), {});
    CHECK(si.banda == Banda::uhf);
    avanza(r, t, t + 179.0, albero());
    CHECK(r.ricevuti().empty());
    avanza(r, t, t + 3.0, albero());
    CHECK(r.ricevuti().size() == 2);
    CHECK(r.ricevuti()[0].banda == Banda::uhf);
    CHECK(c_e(r.eventi(), EventoRadio::Tipo::ssixs));
}

TEST_CASE("radio: trasmettere espone alla radiogoniometria, P = P_rete (1 - e^(-t/tau))") {
    SalaRadio r(LatoRadio::usa, 1);
    CHECK_FALSE(r.trasmetti(TipoTrasmissione::contatto, 0.0, profondo(), {}).fatta);   // senza albero
    // satellite con un ESM nemico vicino: 1,5 s, solo il rilevamento, subito
    const auto uhf = r.trasmetti(TipoTrasmissione::contatto, 0.0, albero(), {true, true});
    CHECK(uhf.banda == Banda::uhf);
    CHECK(uhf.durata_s == doctest::Approx(1.5));
    CHECK(uhf.probabilita_fix == doctest::Approx(0.6 * (1.0 - std::exp(-0.5))));
    CHECK(uhf.errore_m == 0.0);
    CHECK(uhf.ritardo_s == 0.0);
    // senza satellite: HF lungo, quasi certo con la rete nemica, errore di decine di km e ore di ritardo
    StatoAntenne nord = albero();
    nord.latitudine = 75.0;
    const auto hf = r.trasmetti(TipoTrasmissione::situazione, 0.0, nord, {});
    CHECK(hf.banda == Banda::hf);
    CHECK(hf.durata_s == doctest::Approx(160.0));
    CHECK(hf.probabilita_fix == doctest::Approx(0.9).epsilon(0.001));
    CHECK(hf.ritardo_s >= 3600.0);
    CHECK(hf.ritardo_s <= 3.0 * 3600.0);
    // il burst sovietico da 0,72 s contro Bullseye: circa un terzo
    SalaRadio alfa(LatoRadio::urss, 1);
    const auto b = alfa.trasmetti(TipoTrasmissione::contatto, 0.0, albero(), {});
    CHECK(b.durata_s == doctest::Approx(0.72));
    CHECK(b.probabilita_fix == doctest::Approx(0.9 * (1.0 - std::exp(-0.48))));
}

TEST_CASE("radio: finestre di comunicazione e campanello ELF dei FLASH per chi ha l'ELF") {
    SalaRadio usa(LatoRadio::usa, 1, 10.0 * 3600.0);
    CHECK(usa.prossima_finestra(0.0) == doctest::Approx(2.0 * 3600.0));     // alle 12Z, +-2 ore
    CHECK(usa.prossima_finestra(3.0 * 3600.0) == doctest::Approx(2.0 * 3600.0));   // alle 13Z e' ancora quella
    CHECK(usa.prossima_finestra(5.0 * 3600.0) == doctest::Approx(14.0 * 3600.0));
    SalaRadio alfa(LatoRadio::urss, 1, 8.0 * 3600.0, 24, 10, 1989, senza_fondo());   // 09Z = 12:00 di Mosca
    CHECK(alfa.prossima_finestra(0.0) == doctest::Approx(3600.0));
    CHECK(alfa.elf());
    CHECK_FALSE(usa.elf());
    double t = 0.0;
    alfa.cala_filo(t);
    avanza(alfa, t, 200.0, profondo(250.0, 10.0));
    CHECK(alfa.ricezione(Banda::elf) == Ricezione::si);
    CHECK(alfa.ricezione(Banda::vlf) == Ricezione::no);   // troppo in basso per il VLF
    alfa.programma(messaggio(Precedenza::flash), t);
    avanza(alfa, t, t + 5.0, profondo(250.0, 10.0));
    CHECK(c_e(alfa.eventi(), EventoRadio::Tipo::campanello));
}

TEST_CASE("radio: il traffico di fondo da' i numeri di serie e gli ordini restano rileggibili") {
    SalaRadio r(LatoRadio::usa, 7);
    MessaggioRadio ordini;
    ordini.oggetto = "TORPEX NLANT";
    ordini.testo = {"Ordini di missione.", "MISSION ORDERS."};
    r.ordini_di_missione(ordini);
    double t = 0.0;
    avanza(r, t, 4.0 * 3600.0, albero());
    CHECK(r.ricevuti().size() >= 5);   // circa uno ogni mezz'ora
    for (std::size_t i = 1; i < r.ricevuti().size(); ++i) CHECK(r.ricevuti()[i].messaggio.serie == static_cast<int>(i));
    REQUIRE(r.ordini().size() == 1);
    CHECK(r.ordini()[0]->messaggio.serie == 0);
    MessaggioRadio cambio = messaggio(Precedenza::immediate);
    cambio.ordine = "NUOVA_ZONA";
    r.programma(cambio, t);
    avanza(r, t, t + 300.0, albero());
    CHECK(r.ordini().size() == 2);
}

// --- R1: orologio di missione, regole d'ingaggio, intelligence, debriefing ---

namespace {

ParametriRadio di_missione_senza_fondo() {
    ParametriRadio p = parametri_di_missione(900.0);
    p.fondo_ogni_s = 1e12;
    return p;
}

CambioRoe roe(CategoriaRoe c, StatoRoe s) {
    CambioRoe r{};
    r[static_cast<std::size_t>(c)] = s;
    return r;
}

}  // namespace

TEST_CASE("radio R1: orologio di missione, ciclo di 15 minuti e finestre in scala") {
    const ParametriRadio p = parametri_di_missione(900.0);
    CHECK(p.ciclo_s == 900.0);
    CHECK(p.fondo_ogni_s == doctest::Approx(300.0));
    SalaRadio r(LatoRadio::usa, 1, 0.0, 24, 10, 1989, p);
    CHECK(r.tolleranza_finestra_s() == doctest::Approx(300.0));
    CHECK(r.prossima_finestra(0.0) == doctest::Approx(900.0));
    CHECK(r.prossima_finestra(1100.0) == doctest::Approx(900.0));     // ancora dentro la tolleranza
    CHECK(r.prossima_finestra(1300.0) == doctest::Approx(2700.0));
    // un PRIORITY torna in onda dopo un ciclo di 15 minuti, non di due ore
    SalaRadio q(LatoRadio::usa, 1, 0.0, 24, 10, 1989, di_missione_senza_fondo());
    q.programma(messaggio(Precedenza::priority), 0.0);
    double t = 0.0;
    avanza(q, t, 200.0, profondo());
    REQUIRE(q.prossimo_broadcast());
    CHECK(q.prossimo_broadcast()->inizio == doctest::Approx(900.0).epsilon(0.01));
    CHECK(q.prossimo_broadcast()->ripetizione == 2);
    CHECK(q.prossimo_inizio_ciclo(t) == doctest::Approx(900.0).epsilon(0.01));
}

TEST_CASE("radio R1: ROE trasmesse e ricevute, giudizio dei lanci e debriefing con le serie perse") {
    SalaRadio r(LatoRadio::usa, 1, 0.0, 24, 10, 1989, di_missione_senza_fondo());
    r.roe_iniziali(roe(CategoriaRoe::navi, StatoRoe::libero));
    MessaggioRadio stretta = messaggio(Precedenza::immediate, "ROE CHANGE");
    stretta.roe = roe(CategoriaRoe::navi, StatoRoe::trattenuto);
    const int serie = r.programma(stretta, 0.0);
    double t = 0.0;
    avanza(r, t, 200.0, profondo());   // in profondita': non lo copia
    CHECK(r.roe_trasmesse(t)[CategoriaRoe::navi] == StatoRoe::trattenuto);
    CHECK(r.roe_trasmesse(t).serie[static_cast<std::size_t>(CategoriaRoe::navi)] == serie);
    CHECK(r.roe_ricevute()[CategoriaRoe::navi] == StatoRoe::libero);
    // trasmesse da meno di un ciclo: solo una nota; dopo un ciclo: infrazione per non aver copiato
    CHECK(r.giudica_lancio(5, CategoriaRoe::navi, false, t).esito == GiudizioRoe::Esito::nota);
    CHECK(r.giudica_lancio(5, CategoriaRoe::navi, false, 1200.0).esito == GiudizioRoe::Esito::infrazione_non_copiato);
    CHECK(r.giudica_lancio(6, CategoriaRoe::sottomarini, false, 1200.0).esito == GiudizioRoe::Esito::permesso);
    // copiato alla ripetizione: violare le ricevute e' un'infrazione piena
    avanza(r, t, 1100.0, albero());
    CHECK(r.roe_ricevute()[CategoriaRoe::navi] == StatoRoe::trattenuto);
    CHECK(c_e(r.eventi(), EventoRadio::Tipo::ordini_cambiati));
    CHECK(r.giudica_lancio(5, CategoriaRoe::navi, false, t).esito == GiudizioRoe::Esito::infrazione);
    // stretto: solo con il bersaglio identificato ostile
    SalaRadio s(LatoRadio::usa, 1, 0.0, 24, 10, 1989, di_missione_senza_fondo());
    s.roe_iniziali(roe(CategoriaRoe::sottomarini, StatoRoe::stretto));
    CHECK(s.giudica_lancio(2, CategoriaRoe::sottomarini, true, 0.0).esito == GiudizioRoe::Esito::permesso);
    CHECK(s.giudica_lancio(2, CategoriaRoe::sottomarini, false, 0.0).esito == GiudizioRoe::Esito::infrazione);
    // il giocatore credeva fosse una nave da guerra: per il debriefing e' uno scambio, non una violazione consapevole
    const auto g = s.giudica_lancio(8, CategoriaRoe::mercantili, false, 0.0, CategoriaRoe::navi, "perry");
    CHECK(g.scambiato());
    CHECK(g.classe_creduta == "perry");
    CHECK_FALSE(s.giudica_lancio(2, CategoriaRoe::sottomarini, true, 0.0, CategoriaRoe::sottomarini).scambiato());
    CHECK_FALSE(s.giudica_lancio(2, CategoriaRoe::sottomarini, true, 0.0).scambiato());   // non classificato
    // debriefing: trasmessi, persi e lanci
    SalaRadio d(LatoRadio::usa, 1, 0.0, 24, 10, 1989, di_missione_senza_fondo());
    d.programma(messaggio(Precedenza::routine, "PERSO"), 0.0);
    double u = 0.0;
    avanza(d, u, 300.0, profondo());
    d.programma(messaggio(Precedenza::routine, "PRESO"), u);
    avanza(d, u, 600.0, albero());
    const Debriefing db = d.debriefing();
    REQUIRE(db.trasmessi.size() == 2);
    REQUIRE(db.persi.size() == 1);
    CHECK(db.persi[0].messaggio.oggetto == "PERSO");
    CHECK(db.trasmessi[1].ricevuto > 0.0);
    CHECK(r.debriefing().lanci.size() == 4);
}

TEST_CASE("radio R1: contatto d'intelligence compilato alla messa in onda, con l'ellisse che cresce") {
    SalaRadio r(LatoRadio::usa, 1, 0.0, 24, 10, 1989, di_missione_senza_fondo());
    MessaggioRadio m = messaggio(Precedenza::priority, "TANKER GROUP POSIT");
    RichiestaIntelligence q;
    q.unita = {4, 5};
    q.eta_s = 900.0;
    q.errore_m = 3000.0;
    q.sensore = "p3";
    q.rotta_err_deg = 15.0;
    q.velocita_err_kn = 2.0;
    m.richiesta = q;
    const int serie = r.programma(m, 60.0);
    CHECK(r.da_compilare(30.0).empty());           // non ancora dovuto
    const auto dc = r.da_compilare(60.0);
    REQUIRE(dc.size() == 1);
    CHECK(dc[0].first == serie);
    DatiIntelligence d;
    d.unita = q.unita;
    d.pos = {1000.0, 2000.0};
    d.t_oss = 60.0 - 900.0;
    d.rotta = 90.0;
    d.velocita_kn = 12.0;
    d.errore_m = 3000.0;
    d.rotta_err_deg = 15.0;
    d.velocita_err_kn = 2.0;
    d.sensore = "p3";
    r.compila(serie, d);
    CHECK(r.da_compilare(60.0).empty());
    double t = 0.0;
    avanza(r, t, 300.0, albero());
    const auto c = r.intelligence(t);
    REQUIRE(c.size() == 1);
    CHECK(c[0].serie == serie);
    CHECK(c_e(r.eventi(), EventoRadio::Tipo::intelligence));
    // a 1200 s dall'osservazione: il centro e' andato a est, l'ellisse cresce lungo la rotta e di traverso
    const double t1 = d.t_oss + 1200.0;
    CHECK(c[0].centro(t1).x == doctest::Approx(1000.0 + 12.0 * NODO * 1200.0));
    CHECK(c[0].centro(t1).y == doctest::Approx(2000.0));
    CHECK(c[0].semiasse_rotta(t1) == doctest::Approx(3000.0 + 2.0 * NODO * 1200.0));
    CHECK(c[0].semiasse_traverso(t1) == doctest::Approx(3000.0 + 12.0 * NODO * std::sin(15.0 * 3.14159265358979323846 / 180.0) * 1200.0));
    CHECK(c[0].semiasse_rotta(t1 + 600.0) > c[0].semiasse_rotta(t1));
    CHECK(c[0].orientamento() == 90.0);
    // un dato nuovo sulle stesse unita' sostituisce il vecchio; dopo tre cicli si toglie dalla carta
    MessaggioRadio nuovo = m;
    nuovo.richiesta.reset();
    DatiIntelligence d2 = d;
    d2.unita = {5};
    d2.t_oss = t;
    nuovo.intelligence = d2;
    r.programma(nuovo, t);
    avanza(r, t, t + 300.0, albero());
    REQUIRE(r.intelligence(t).size() == 1);
    CHECK(r.intelligence(t)[0].dati.unita == std::vector<int>{5});
    CHECK(r.intelligence(t + 3.0 * 900.0 + 100.0).empty());
}

// --- la sala radio nella missione (regola di simulazione) ---

#include <filesystem>
#include <fstream>

#include "missioni/danni_missione.hpp"
#include "missioni/missione.hpp"
#include "nucleo/classi.hpp"

namespace {

namespace fs = std::filesystem;

// il 688 (alla quota data, con l'antenna alzata se a quota periscopica) e una Krivak II a `distanza` metri
std::string missione_radio(double quota, double distanza, bool con_radio = false) {
    const auto dir = fs::temp_directory_path() / "m688_prova_radio";
    fs::create_directories(dir / "radio");
    const auto f = dir / "PROVA_RADIO.lua";
    const std::string modello = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200 }";
    std::ofstream(f) << "return { id = 'PROVA_RADIO', teatro = 'faroe', lato = 'usa', data = '1989-10-24', ora_utc = 15.5,"
                        " briefing = { it = { 'ORDINI DI MISSIONE', 'DA: CINCLANTFLT', 'A: USS LOS ANGELES', 'OGG.: PROVA', 'Pattugliare.' },"
                        "              en = { 'MISSION ORDERS', 'FM: CINCLANTFLT', 'TO: USS LOS ANGELES', 'SUBJ: TEST', 'PATROL.' } },"
                        " script = { SU = function(u) m688.antenna(u, true) m688.salva_modello(u) end,"
                        "            ZONA = function(u) m688.variabile(u, 5, 7) end },"
                        " varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
                        "  posizione = { x = 0, y = 0 }, quota_m = " << quota << ", rotta = 0, velocita_kn = 4, " << modello << " },"
                        "{ id = 2, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = " << distanza << " },"
                        "  quota_m = 0, rotta = 90, velocita_kn = 0, " << modello << " },"
                        "\n}, timer = { { dopo_s = 0, unita = 0, gestore = 'SU' } } } } }\n";
    const auto r = dir / "radio" / "PROVA_RADIO.lua";
    if (con_radio)
        std::ofstream(r) << "return { { dopo_s = 60, precedenza = 'immediate', oggetto = 'NUOVA ZONA', gestore = 'ZONA',"
                            " testo = { it = 'Spostarsi a nord.', en = 'MOVE NORTH.' } } }\n";
    else
        fs::remove(r);
    return f.string();
}

struct PartitaRadio {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    explicit PartitaRadio(const std::string& f) : m(f, 1, false), dm(classi, M688_DATI, 1, RegolaDanni::simulazione) {}
    void avanza(double s) { for (double t = 0; t < s; t += 1.0 / 9.0) { m.avanza(1.0 / 9.0); dm.avanza(m, 1.0 / 9.0); } }
};

bool c_e_radio(const Missione& m, int codice) {
    for (const auto& x : m.messaggi_radio()) if (x.codice == codice) return true;
    return false;
}

}  // namespace

TEST_CASE("radio nella missione: niente nel classico; ordini di missione dal briefing come serie 0") {
    const auto f = missione_radio(5.0, 30000.0);
    Missione classica(f, 1, false);
    classica.avanza(10.0);
    CHECK(classica.radio() == nullptr);
    PartitaRadio p(f);
    p.avanza(1.0);
    REQUIRE(p.m.radio() != nullptr);
    REQUIRE(p.m.radio()->ordini().size() == 1);
    const auto& o = p.m.radio()->ordini()[0]->messaggio;
    CHECK(o.serie == 0);
    CHECK(o.da == "CINCLANTFLT");
    CHECK(o.a == "USS LOS ANGELES");
    CHECK(o.oggetto == "TEST");
    CHECK(o.testo.it == "Pattugliare.");
    CHECK(o.dtg == "241530Z OCT 89");
}

TEST_CASE("radio nella missione: all'antenna si ricevono il traffico e gli ordini nuovi, che eseguono il gestore") {
    const auto f = missione_radio(5.0, 30000.0, true);
    PartitaRadio p(f);
    p.avanza(600.0);
    const SalaRadio& r = *p.m.radio();
    CHECK(r.ricezione(Banda::vlf) == Ricezione::si);
    CHECK(c_e_radio(p.m, 315));                // in arrivo
    CHECK(c_e_radio(p.m, 317));                // IMMEDIATE ricevuto
    CHECK(r.ordini().size() == 2);
    CHECK(p.m.variabili()[5] == 7);            // il gestore ZONA ha cambiato gli ordini
}

TEST_CASE("radio nella missione: in profondita' senza filo non si riceve niente") {
    const auto f = missione_radio(150.0, 30000.0, true);
    PartitaRadio p(f);
    p.avanza(600.0);
    CHECK(p.m.radio()->ricezione(Banda::vlf) == Ricezione::no);
    CHECK(p.m.radio()->ordini().size() == 1);   // solo gli ordini di missione
    CHECK(p.m.variabili()[5] != 7);
    // col filo galleggiante (due minuti per filarlo) a 150 m e' troppo in basso: si sale a 100
    p.m.radio_filo(true);
    p.avanza(200.0);
    CHECK(p.m.radio()->filo());
}

TEST_CASE("radio nella missione: trasmettere con una nave nemica vicina le da' il rilevamento del giocatore") {
    const auto f = missione_radio(5.0, 20000.0);
    PartitaRadio p(f);
    p.avanza(2.0);
    bool localizzato = false;
    for (int i = 0; i < 40 && !localizzato; ++i) {
        const auto e = p.m.radio_trasmetti(TipoTrasmissione::contatto);
        REQUIRE(e.fatta);
        CHECK(e.banda == Banda::uhf);
        localizzato = e.localizzato;
        p.avanza(1.0);
    }
    REQUIRE(localizzato);                       // 0,24 a trasmissione: in 40 quasi certo
    REQUIRE(p.m.unita().at(2).contatti.count(0) == 1);
    const Contatto& c = p.m.unita().at(2).contatti.at(0);
    CHECK(c.sensore == "radiogoniometria");
    CHECK_FALSE(c.posizione_nota);              // un solo sensore: niente distanza
    CHECK(c_e_radio(p.m, 324));
}

TEST_CASE("radio R1 nella missione: file radio, intelligence con la verita', ROE ricevute, premio del rapporto e boa SLOT") {
    const auto dir = fs::temp_directory_path() / "m688_prova_radio_r1";
    fs::create_directories(dir / "radio");
    const auto f = dir / "PROVA_R1.lua";
    const std::string modello = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200 }";
    std::ofstream(f) << "return { id = 'PROVA_R1', teatro = 'faroe', lato = 'usa',"
                        " script = { SU = function(u) m688.antenna(u, true) m688.salva_modello(u) end,"
                        "  TR = function(u) if m688.trasmesso(u, 'contatto') then m688.variabile(u, 8, 1) end"
                        "   if m688.trasmesso(u, 'contatto', 520) then m688.variabile(u, 9, 1) end"
                        "   if m688.trasmesso(u, 'attacco') then m688.variabile(u, 10, 1) end"
                        "   if m688.evento(u, 'trasmissione') then m688.variabile(u, 12, 1) end"
                        "   if m688.evento(u, 'incaglio') then m688.variabile(u, 13, 1) end end },"
                        " varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
                        "  posizione = { x = 0, y = 0 }, quota_m = 5, rotta = 0, velocita_kn = 4, " << modello << " },"
                        "{ id = 2, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = 60000 },"
                        "  quota_m = 0, rotta = 90, velocita_kn = 12, " << modello << " },"
                        "{ id = 3, classe = 'cargo', categoria = 'nave', flag = 2, posizione = { x = 20000, y = 60000 },"
                        "  quota_m = 0, rotta = 0, velocita_kn = 10, " << modello << " },"
                        "\n}, timer = { { dopo_s = 0, unita = 0, gestore = 'SU' }, { dopo_s = 540, unita = 0, gestore = 'TR' } } } } }\n";
    std::ofstream(dir / "radio" / "PROVA_R1.lua")
        << "return { formato_radio = 2,"
           " stato_iniziale = { roe = { navi = 'trattenuto' }, neutrali = { 3 } },"
           " rapporti = { contatto = { gestore = 'PREMIO', ritardo_s = 60 } },"
           " slot = { boe = 2, ritardo_min_s = 60, ritardo_max_s = 600, portata_vhf_m = 40000 },"
           " gestori = { PREMIO = function(u) m688.variabile(u, 6, 1) end },"
           " { id = 'INT1', dopo_s = 30, precedenza = 'priority', oggetto = 'KRIVAK POSIT',"
           "   testo = { it = 'Krivak a nord.', en = 'KRIVAK NORTH.' },"
           "   contatto = { unita = { 2 }, eta_s = 600, errore_m = 2000, sensore = 'p3', rotta_err = 10, velocita_err_kn = 2 } },"
           " { id = 'ROE2', dopo_s = 60, precedenza = 'immediate', oggetto = 'ROE CHANGE', roe = { navi = 'libero' },"
           "   testo = { it = 'Armi libere sulle navi.', en = 'WEAPONS FREE ON SHIPS.' } } }\n";
    PartitaRadio p(f.string());
    p.avanza(500.0);
    const SalaRadio& r = *p.m.radio();
    CHECK(r.parametri().ciclo_s == 900.0);                      // orologio di missione
    CHECK(p.m.categoria_roe(p.m.unita().at(3)) == CategoriaRoe::neutrali);
    CHECK(p.m.categoria_roe(p.m.unita().at(2)) == CategoriaRoe::navi);
    CHECK(r.roe_ricevute()[CategoriaRoe::navi] == StatoRoe::libero);   // ROE2 ricevuto all'antenna
    CHECK(c_e_radio(p.m, 326));
    // contatto d'intelligence: la Krivak di 600 s prima, con l'errore e la sua rotta
    const auto intel = r.intelligence(p.m.tempo());
    REQUIRE(intel.size() == 1);
    CHECK(intel[0].dati.unita == std::vector<int>{2});
    CHECK(c_e_radio(p.m, 327));
    const Vec2 vera = p.m.unita().at(2).pos;
    CHECK(distanza(intel[0].centro(p.m.tempo()), vera) < intel[0].semiasse_rotta(p.m.tempo()) + intel[0].semiasse_traverso(p.m.tempo()) + 6000.0);
    // rapporto di contatto: il premio dopo un minuto, una volta sola
    REQUIRE(p.m.radio_trasmetti(TipoTrasmissione::contatto).fatta);
    p.avanza(70.0);
    CHECK(p.m.variabili()[6] == 1);
    // m688.trasmesso (libreria I07): il rapporto di contatto a 500 s, il timer a 540 lo trova; non dopo 520, non d'attacco
    CHECK(p.m.variabili()[8] == 1);
    CHECK(p.m.variabili().count(9) == 0);
    CHECK(p.m.variabili().count(10) == 0);
    // m688.evento (libreria I03): l'evento del registro per nome; nessun incaglio
    CHECK(p.m.variabili()[12] == 1);
    CHECK(p.m.variabili().count(13) == 0);
    CHECK(c_e_radio(p.m, 331));
    // boa SLOT: ritardo nei limiti, una boa in meno, trasmette dopo il ritardo
    CHECK_FALSE(p.m.radio_slot(30.0));
    REQUIRE(p.m.radio_slot(100.0));
    CHECK(p.m.boe_slot() == 1);
    REQUIRE(p.m.slot_in_attesa());
    p.avanza(110.0);
    CHECK_FALSE(p.m.slot_in_attesa());
    CHECK(c_e_radio(p.m, 332));
}

TEST_CASE("radio nella missione vera: il broadcast va in onda e si annuncia; senza antenne non si riceve" * doctest::skip(!fs::exists(M688_MISSIONI "/SCENE4A.lua"))) {
    const auto classi = carica_classi(M688_DATI "/classi.toml");
    Missione m(M688_MISSIONI "/SCENE4A.lua", 1);
    m.imposta_data_ora(24, 10, 1989, 9.0);
    DanniMissione dm(classi, M688_DATI, 1, RegolaDanni::simulazione);
    for (int i = 0; i < 9 * 1000; ++i) { m.avanza(1.0 / 9.0); dm.avanza(m, 1.0 / 9.0); }
    REQUIRE(m.radio() != nullptr);
    const SalaRadio& r = *m.radio();
    CHECK(r.dtg(0.0) == "240900Z OCT 89");
    CHECK(r.debriefing().trasmessi.size() >= 2);       // traffico di fondo ogni 5 minuti circa
    CHECK(r.prossimo_inizio_ciclo(m.tempo()) > m.tempo());   // il prossimo broadcast e' annunciato
    if (r.ricezione(Banda::vlf) == Ricezione::no) CHECK(r.ricevuti().size() == 1);   // solo gli ordini di missione
}

// --- R2: zone, inganno, scadenze, sessioni ---

TEST_CASE("radio R2: zone trasmesse e ricevute, cerchio e zona mobile attorno al battello amico") {
    SalaRadio r(LatoRadio::usa, 1, 0.0, 24, 10, 1989, di_missione_senza_fondo());
    Zona a;
    a.id = "A";
    a.tipo = TipoZona::protetta;
    a.centro = Vec2{0.0, 0.0};
    a.raggio_m = 1000.0;
    r.zone_iniziali({a});
    MessaggioRadio m = messaggio(Precedenza::immediate, "HAVEN");
    Zona b;
    b.id = "HAVEN1";
    b.tipo = TipoZona::protetta_mobile;
    b.unita = 7;
    b.avanti_m = 9000.0;
    b.dietro_m = 9000.0;
    b.lato_m = 4000.0;
    m.zone = CambioZone{{b}, {"A"}};
    const int serie = r.programma(m, 0.0);
    double t = 0.0;
    avanza(r, t, 300.0, profondo());
    REQUIRE(r.zone_ricevute().size() == 1);
    CHECK(r.zone_ricevute()[0].id == "A");          // il giocatore non l'ha copiato
    REQUIRE(r.zone_trasmesse(t).size() == 1);
    CHECK(r.zone_trasmesse(t)[0].id == "HAVEN1");
    CHECK(r.zone_trasmesse(t)[0].serie == serie);
    avanza(r, t, 1200.0, albero());                  // copiato alla ripetizione
    REQUIRE(r.zone_ricevute().size() == 1);
    CHECK(r.zone_ricevute()[0].id == "HAVEN1");
    CHECK(c_e(r.eventi(), EventoRadio::Tipo::ordini_cambiati));
    // geometria: cerchio; rettangolo della mobile col battello in (0, 0) a rotta 090
    CHECK(dentro_zona(a, {500.0, 500.0}));
    CHECK_FALSE(dentro_zona(a, {900.0, 900.0}));
    const PosaUnita posa{{0.0, 0.0}, 90.0};
    CHECK(dentro_zona(b, {8000.0, 0.0}, posa));      // davanti
    CHECK(dentro_zona(b, {-8000.0, 3000.0}, posa));  // dietro, di lato
    CHECK_FALSE(dentro_zona(b, {0.0, 5000.0}, posa));
    CHECK_FALSE(dentro_zona(b, {8000.0, 0.0}));      // senza posa la mobile non c'e'
    CHECK(poligono_zona(b, posa).size() == 4);
}

TEST_CASE("radio R2: inganno riconoscibile dall'autenticazione; compito scaduto; ricevuto tardi non vale") {
    SalaRadio r(LatoRadio::usa, 1, 0.0, 24, 10, 1989, di_missione_senza_fondo());
    r.roe_iniziali(roe(CategoriaRoe::navi, StatoRoe::trattenuto));
    MessaggioRadio falso = messaggio(Precedenza::immediate, "ROE CHANGE");
    falso.roe = roe(CategoriaRoe::navi, StatoRoe::libero);
    falso.autentico = false;
    falso.ordine = "INGANNO";
    const int serie = r.programma(falso, 0.0);
    double t = 0.0;
    avanza(r, t, 300.0, albero());
    REQUIRE(r.ricevuti().size() == 1);
    CHECK(r.nuovi().empty());                                         // il gestore di un inganno non gira
    CHECK(r.roe_ricevute()[CategoriaRoe::navi] == StatoRoe::libero);  // entra fra le ricevute: da verificare
    CHECK_FALSE(r.roe_ricevute().dubbio(CategoriaRoe::navi));
    CHECK(r.roe_ricevute(true)[CategoriaRoe::navi] == StatoRoe::trattenuto);
    CHECK(r.roe_trasmesse(t)[CategoriaRoe::navi] == StatoRoe::trattenuto);   // il comando non l'ha mandato
    const auto g = r.giudica_lancio(5, CategoriaRoe::navi, false, t);
    CHECK(g.esito == GiudizioRoe::Esito::infrazione);
    CHECK(g.ordine_falso_creduto);                                    // «ordine non autenticato, creduto»
    CHECK_FALSE(r.verifica(serie));                                   // il giocatore verifica: il gruppo non torna
    CHECK(r.ricevuti()[0].verificato);
    CHECK(r.roe_ricevute()[CategoriaRoe::navi] == StatoRoe::libero);  // resta, ma dubbia
    CHECK(r.roe_ricevute().dubbio(CategoriaRoe::navi));
    // compito valido 100 s: in profondita' scade; alla ripetizione arriva tardi e non esegue il gestore
    SalaRadio s(LatoRadio::usa, 1, 0.0, 24, 10, 1989, di_missione_senza_fondo());
    MessaggioRadio compito = messaggio(Precedenza::flash, "INTERCEPT");
    compito.scade_s = 100.0;
    compito.ordine = "VAI";
    compito.gestore_scaduto = "TROPPO_TARDI";
    s.programma(compito, 0.0);
    double u = 0.0;
    avanza(s, u, 300.0, profondo());
    CHECK(c_e(s.eventi(), EventoRadio::Tipo::scaduto));
    REQUIRE(s.messaggio(1) != nullptr);
    CHECK(s.messaggio(1)->gestore_scaduto == "TROPPO_TARDI");
    avanza(s, u, 1200.0, albero());
    REQUIRE(s.ricevuti().size() == 1);
    CHECK(s.ricevuti()[0].tardi);
    CHECK(s.nuovi().empty());
}

TEST_CASE("radio R2: sessioni obbligatorie, mancate e il guinzaglio dopo due") {
    ParametriRadio p = di_missione_senza_fondo();
    p.finestra_ogni_s = 600.0;
    p.finestra_primo_s = 300.0;
    p.tolleranza_finestra_s = 60.0;
    p.sessioni_obbligatorie = true;
    p.ricevuta_richiesta = true;
    p.saltate_max = 2;
    SalaRadio r(LatoRadio::urss, 1, 0.0, 24, 10, 1989, p);
    double t = 0.0;
    avanza(r, t, 1000.0, profondo());                // le prime due (240-360 e 840-960) saltate
    const auto ev = r.eventi();
    CHECK(std::count_if(ev.begin(), ev.end(), [](const EventoRadio& e) { return e.tipo == EventoRadio::Tipo::sessione_mancata; }) == 2);
    CHECK(c_e(ev, EventoRadio::Tipo::guinzaglio));
    // la terza fatta: ricezione all'albero e ricevuta trasmessa nella finestra
    avanza(r, t, 1450.0, albero());
    REQUIRE(r.trasmetti(TipoTrasmissione::conferma, t, albero(), {}).fatta);
    avanza(r, t, 1600.0, albero());
    const auto ss = r.sessioni(t);
    REQUIRE(ss.size() >= 3);
    CHECK(ss[0].stato == Sessione::Stato::mancata);
    CHECK(ss[1].stato == Sessione::Stato::mancata);
    CHECK(ss[2].stato == Sessione::Stato::fatta);
    CHECK(ss.back().stato == Sessione::Stato::futura);   // la prossima
}

TEST_CASE("radio R2 nella missione: zona protetta mobile, ingresso, verifica di un falso e richiamo con l'esito") {
    const auto dir = fs::temp_directory_path() / "m688_prova_radio_r2";
    fs::create_directories(dir / "radio");
    const auto f = dir / "PROVA_R2.lua";
    const std::string modello = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200 }";
    std::ofstream(f) << "return { id = 'PROVA_R2', teatro = 'faroe', lato = 'usa', paesaggio_sonoro = false,"
                        " script = { SU = function(u) m688.antenna(u, true) m688.salva_modello(u) end },"
                        " varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
                        "  posizione = { x = 0, y = 0 }, quota_m = 5, rotta = 0, velocita_kn = 3, " << modello << " },"
                        "{ id = 4, classe = 'sturgeon', categoria = 'sottomarino', flag = 9, posizione = { x = 0, y = 15000 },"
                        "  quota_m = 100, rotta = 180, velocita_kn = 10, " << modello << " },"
                        "\n}, timer = { { dopo_s = 0, unita = 0, gestore = 'SU' } } } } }\n";
    std::ofstream(dir / "radio" / "PROVA_R2.lua")
        << "return { formato_radio = 2,"
           " stato_iniziale = { zone = { { id = 'HAVEN1', tipo = 'protetta_mobile', unita = 4, avanti_m = 14000, dietro_m = 9000, lato_m = 4000 } } },"
           " gestori = { RIENTRO = function(u) m688.messaggio(u, 105) end, FALSO = function(u) m688.variabile(u, 7, 1) end },"
           " { id = 'FAKE', dopo_s = 30, precedenza = 'immediate', oggetto = 'PROCEED NORTH', autentico = false, gestore = 'FALSO',"
           "   testo = { it = 'Procedere a nord.', en = 'PROCEED NORTH.' } },"
           " { id = 'RICH', dopo_s = 200, precedenza = 'flash', oggetto = 'BREAK OFF', gestore = 'RIENTRO',"
           "   testo = { it = 'Interrompere e rientrare.', en = 'BREAK OFF AND RTB.' } } }\n";
    PartitaRadio p(f.string());
    p.avanza(10.0);
    // la zona mobile segue lo Sturgeon che scende verso sud a 10 nodi: centro sul battello, rettangolo orientato
    auto zc = p.m.zone_ricevute();
    REQUIRE(zc.size() == 1);
    CHECK(zc[0].zona.id == "HAVEN1");
    REQUIRE(zc[0].centro);
    CHECK(distanza(*zc[0].centro, p.m.unita().at(4).pos) < 1.0);
    CHECK(zc[0].vertici.size() == 4);
    CHECK(zc[0].orientamento == doctest::Approx(180.0));
    // il bordo davanti parte a 1 km dal giocatore: in pochi minuti ci arriva, avviso d'ingresso e voce nel debriefing
    p.avanza(400.0);
    CHECK(c_e_radio(p.m, 328));
    REQUIRE_FALSE(p.m.radio()->debriefing().ingressi.empty());
    CHECK(p.m.radio()->debriefing().ingressi[0].second == "HAVEN1");
    // il falso non esegue il suo gestore; la verifica lo riconosce
    CHECK(p.m.variabili()[7] != 1);
    int falso = -1;
    for (const auto& r : p.m.radio()->ricevuti()) if (r.messaggio.id == "FAKE") falso = r.messaggio.serie;
    REQUIRE(falso > 0);
    CHECK_FALSE(p.m.radio_verifica(falso));
    CHECK(c_e_radio(p.m, 339));
    // il richiamo ricevuto accoda l'esito della pagina 5
    REQUIRE(p.m.esito());
    CHECK(*p.m.esito() == 5);
}
