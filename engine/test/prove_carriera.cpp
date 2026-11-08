#include <doctest.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>

#include "missioni/carriera_missione.hpp"
#include "missioni/danni_missione.hpp"
#include "missioni/missione.hpp"
#include "missioni/registro_missione.hpp"
#include "missioni/stazioni_missione.hpp"
#include "nucleo/ordini.hpp"
#include "nucleo/carriera.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/fascicolo.hpp"

using namespace m688;

// Carriera, punteggio, decorazioni e fascicolo (Fase 4a, M3; dossier docs/ricerca/13_carriera_gradi_decorazioni.md)

namespace {

const RegoleCarriera& regole() {
    static const RegoleCarriera r = carica_carriera(M688_DATI "/carriera.toml");
    return r;
}

// una missione pulita: obiettivi raggiunti, nessuna esposizione, ordini eseguiti, nessun lancio
EsitoPerCarriera pulita(double riuscita = 1.0) {
    EsitoPerCarriera e;
    e.riuscita = riuscita;
    e.completezza = 1.0;
    return e;
}

ContestoMissione contesto(const std::string& id, Situazione s = Situazione::pace) {
    ContestoMissione c;
    c.missione = id;
    c.situazione = s;
    c.battello = "Los Angeles (SSN 688)";
    c.teatro = "Mare di Norvegia";
    c.teatro_en = "Norwegian Sea";
    return c;
}

// missioni diverse per non pagare i rendimenti decrescenti
EsitoCarriera gioca(Carriera& k, int n, Situazione s, const EsitoPerCarriera& e) {
    static int conta = 0;
    return k.chiudi_missione(contesto("PROVA" + std::to_string(conta++) + "_" + std::to_string(n), s), e);
}

}  // namespace

TEST_CASE("carriera: regole caricate dal toml") {
    const auto& r = regole();
    REQUIRE(r.gradi.at("usn").size() == 7);
    CHECK(r.gradi.at("usn")[4].id == "O-5");
    CHECK(r.gradi.at("vmf")[4].nome == "kapitan 2-go ranga");
    CHECK(r.fasi.at("usn")[4].uscita == "commissione");
    CHECK(r.fasi.at("usn")[4].in_zona == doctest::Approx(400.0));
    const Decorazione* nc = r.decorazione("navy_cross");
    REQUIRE(nc);
    CHECK(nc->precedenza == 2);
    int larghezza = 0;
    for (const auto& riga : nc->righe) larghezza += riga.larghezza;
    CHECK(larghezza == 36);
    CHECK(r.decorazione("presidential_unit_citation")->orizzontale);
    CHECK(r.decorazione("eroe_urss")->con == "lenin");
    CHECK(r.soglie.at("usn.guerra").back().decorazione == "medal_of_honor");
    CHECK(r.originali.at("SCENE7A").situazione == Situazione::guerra);
    CHECK(r.originali.at("SCENE1A").situazione == Situazione::pace);
    CHECK(r.originali.count("SCENE9C"));
    CHECK(!r.testi.at("usn").at("conferisce_it.alta").empty());
}

TEST_CASE("carriera: punteggio di missione") {
    const auto& r = regole();
    ContestoMissione c = contesto("X");
    Punteggio p = punteggio_missione(pulita(), c, r);
    CHECK(p.riuscita == doctest::Approx(400.0));
    CHECK(p.completezza == doctest::Approx(150.0 * (0.5 + 0.3 * 0.5 + 0.2)));   // nessuna classificazione: neutra
    CHECK(p.discrezione == doctest::Approx(150.0));
    CHECK(p.ordini == doctest::Approx(100.0));
    CHECK(p.efficienza == doctest::Approx(100.0 * (0.7 * 0.7 + 0.3)));   // senza lanci: 0,7 della parte dei colpi
    CHECK(p.totale == doctest::Approx(856.5));
    CHECK(p.giudizio_it.rfind("Eccezionale", 0) == 0);

    EsitoPerCarriera e = pulita(0.5);
    e.esposizioni = {{"ping", 4}, {"fix", 2}};   // 20 + 50 su 100
    e.lanci = 4;
    e.colpi = 1;
    e.infrazioni_roe = 1;
    p = punteggio_missione(e, c, r);
    CHECK(p.discrezione == doctest::Approx(45.0));
    CHECK(p.efficienza == doctest::Approx(100.0 * (0.7 * 0.25 + 0.3)));
    CHECK(p.ordini == doctest::Approx(70.0));
    CHECK(p.giudizio_it.find("discrezione da migliorare") != std::string::npos);

    // le luci accese emerso di notte (31/10/2026): in pace sono la regola, in crisi pesano 10 l'una
    EsitoPerCarriera luci = pulita();
    luci.esposizioni = {{"luci", 3}};
    CHECK(punteggio_missione(luci, c, r).discrezione == doctest::Approx(150.0));
    ContestoMissione crisi = c;
    crisi.situazione = Situazione::crisi;
    CHECK(punteggio_missione(luci, crisi, r).discrezione == doctest::Approx(150.0 * 0.7));

    // moltiplicatori: guerra, veterano dichiarato ma regolare dai dati, pericolo massimo
    e = pulita();
    e.distanza_min_siluro_m = 0.0;
    c.situazione = Situazione::guerra;
    c.avversario = 2;
    c.avversario_calcolato = 1;
    p = punteggio_missione(e, c, r);
    CHECK(p.m_situazione == doctest::Approx(1.6));
    CHECK(p.m_avversario == doctest::Approx(1.0));
    CHECK(p.pericolo == doctest::Approx(0.25));
    CHECK(p.totale == doctest::Approx(856.5 * 1.6 * 1.25));

    // violazioni e squalifica; battello perso
    e = pulita();
    e.neutrali_colpiti = 1;
    p = punteggio_missione(e, contesto("X"), r);
    CHECK(p.violazioni == doctest::Approx(-150.0));
    e.amici_colpiti = 2;
    p = punteggio_missione(e, contesto("X"), r);
    CHECK(p.squalifica);
    CHECK(p.totale == 0.0);
    e = pulita();
    e.battello_perso = true;
    CHECK(punteggio_missione(e, contesto("X"), r).danni == doctest::Approx(-300.0));
}

TEST_CASE("carriera: dall'Ensign alla commissione, con le dolphins") {
    Carriera k(regole(), Carriera::nuovo(regole(), Marina::usn, "Bart Mancuso", false, Indulgenza::realistica));
    CHECK(k.fascicolo().grado == "O-1");
    CHECK(k.fascicolo().id == "bart_mancuso");
    CHECK(!gioca(k, 1, Situazione::pace, pulita()).promozione);
    EsitoCarriera x = gioca(k, 2, Situazione::pace, pulita());
    CHECK(x.promozione == std::optional<std::string>("O-2"));
    CHECK(!x.commissione);   // le uscite semplici non sono commissioni
    for (int i = 0; i < 2; ++i) CHECK(!gioca(k, 3, Situazione::pace, pulita()).promozione);
    x = gioca(k, 4, Situazione::pace, pulita());
    CHECK(x.promozione == std::optional<std::string>("O-3"));
    CHECK(x.qualifica->id == "dolphins");
    CHECK(k.fascicolo().ha_qualifica("dolphins"));
    CHECK(std::any_of(x.automatiche.begin(), x.automatiche.end(), [](const Conferimento& c) { return c.decorazione == "dolphins"; }));
    // LT: la prima commissione a 3 missioni, sotto zona: media alta -> promozione anticipata
    for (int i = 0; i < 2; ++i) gioca(k, 5, Situazione::pace, pulita());
    x = gioca(k, 6, Situazione::pace, pulita());
    REQUIRE(x.commissione);
    CHECK(x.commissione->esito == "anticipata");
    CHECK(k.fascicolo().grado == "O-4");
    CHECK(k.fascicolo().data > "1989-06");
    CHECK(k.fascicolo().incarichi.size() == 4);
}

TEST_CASE("carriera: carriera breve, decorazioni in guerra, ripetizioni e rendimenti") {
    Carriera k(regole(), Carriera::nuovo(regole(), Marina::usn, "Bart Mancuso", true, Indulgenza::realistica));
    const Fascicolo& f = k.fascicolo();
    CHECK(f.grado == "O-5");
    CHECK(f.anno_nomina == 1973);
    CHECK(f.ha_qualifica("dolphins"));
    CHECK(f.ha_qualifica("comando"));
    const auto b0 = k.barretta();
    CHECK(std::any_of(b0.begin(), b0.end(), [](const VoceBarretta& v) { return v.decorazione == "national_defense"; }));
    // guerra: 856,5 x 1,6 = 1370 -> Navy Cross (la Medal of Honor vuole il pericolo massimo)
    ContestoMissione c = contesto("SCENE8A", Situazione::guerra);
    EsitoPerCarriera e = pulita();
    e.fatti_it = {"Affondò il sottomarino lanciamissili nemico."};
    e.fatti_en = {"Sank the enemy ballistic missile submarine."};
    e.attaccato = true;
    EsitoCarriera x = k.chiudi_missione(c, e);
    REQUIRE(x.personale);
    CHECK(x.personale->decorazione == "navy_cross");
    REQUIRE(x.personale->motivazione_it.size() == 3);
    CHECK(x.personale->motivazione_it[0].find("Il Presidente degli Stati Uniti") == 0);
    CHECK(x.personale->motivazione_it[0].find("capitano di fregata Bart Mancuso") != std::string::npos);
    CHECK(x.personale->motivazione_it[0].find("USS Los Angeles (SSN 688)") != std::string::npos);
    CHECK(x.personale->motivazione_en[1] == "Sank the enemy ballistic missile submarine.");
    REQUIRE(x.reparto);
    CHECK(x.reparto->decorazione == "presidential_unit_citation");
    CHECK(std::any_of(x.automatiche.begin(), x.automatiche.end(), [](const Conferimento& a) { return a.decorazione == "combat_action_ribbon"; }));
    // la stessa missione vale la meta': 685 -> Silver Star (la seconda Navy Cross vorrebbe 1275)
    x = k.chiudi_missione(c, e);
    CHECK(x.peso == doctest::Approx(0.5));
    CHECK(x.personale->decorazione == "silver_star");
    // un'altra missione da 1370: la seconda Navy Cross richiede 850 x 1,5 = 1275
    x = k.chiudi_missione(contesto("SCENE9A", Situazione::guerra), e);
    CHECK(x.personale->decorazione == "navy_cross");
    CHECK(x.personale->numero == 2);
    const auto b = k.barretta();
    REQUIRE(b.size() >= 3);
    CHECK(b[0].decorazione == "navy_cross");
    CHECK(b[0].numero == 2);
    CHECK(b[1].decorazione == "silver_star");
}

TEST_CASE("carriera: pace, violazioni e soglie") {
    Carriera k(regole(), Carriera::nuovo(regole(), Marina::usn, "Comandante Prova", true, Indulgenza::realistica));
    EsitoCarriera x = gioca(k, 1, Situazione::pace, pulita());   // 856,5 in pace
    CHECK(x.personale->decorazione == "legion_of_merit");
    CHECK(x.reparto->decorazione == "navy_unit_commendation");
    x = gioca(k, 2, Situazione::crisi, pulita(0.5));   // 656,5 x 1,3 = 853 -> la seconda Legion of Merit vorrebbe 850 x 1,5
    CHECK(x.personale->decorazione == "meritorious_service");
    EsitoPerCarriera e = pulita();
    e.zone_vietate = 1;
    x = gioca(k, 3, Situazione::pace, e);
    CHECK(!x.personale);   // nessuna decorazione con violazioni
    CHECK(!x.reparto);
}

TEST_CASE("carriera: un soccorso riuscito in pace da' la decorazione al coraggio, con la motivazione del salvataggio") {
    const auto unisci = [](const std::vector<std::string>& v) {
        std::string s;
        for (const auto& x : v) s += x + " ";
        return s;
    };
    for (const Marina m : {Marina::usn, Marina::vmf}) {
        Carriera k(regole(), Carriera::nuovo(regole(), m, "Comandante Prova", true, Indulgenza::realistica));
        // un punteggio modesto: la decorazione al coraggio vale se e' piu' alta di quella dei punti
        EsitoPerCarriera e = pulita(0.2);
        e.completezza = 0.0;
        e.esposizioni = {{"fix", 4}};   // sotto i 400 punti: per la VMF sopra c'e' il Servizio alla Patria, che precede
        e.soccorso = true;
        EsitoCarriera x = gioca(k, 1, Situazione::pace, e);
        REQUIRE(x.personale);
        CHECK(x.personale->decorazione == (m == Marina::usn ? "navy_marine_corps_medal" : "coraggio_personale"));
        const std::string it = unisci(x.personale->motivazione_it);
        CHECK_MESSAGE(it.find(m == Marina::usn ? "soccorso di uomini in mare" : "salvataggio di vite umane in mare") != std::string::npos, it);
        // in guerra il soccorso non basta
        Carriera g(regole(), Carriera::nuovo(regole(), m, "Comandante Prova", true, Indulgenza::realistica));
        x = gioca(g, 1, Situazione::guerra, e);
        CHECK((!x.personale || x.personale->decorazione != (m == Marina::usn ? "navy_marine_corps_medal" : "coraggio_personale")));
    }
}

TEST_CASE("carriera: acque territoriali straniere in pace, l'incidente se ci scoprono dentro (S-363/U-137, 1981)") {
    const auto& r = regole();
    const ContestoMissione pace = contesto("ACQUE");
    const Punteggio base = punteggio_missione(pulita(), pace, r);
    // entrarci pesa sugli ordini come un'infrazione delle ROE (30 a entrata)
    EsitoPerCarriera dentro = pulita();
    dentro.acque_territoriali = 2;
    const Punteggio p = punteggio_missione(dentro, pace, r);
    CHECK(p.ordini == doctest::Approx(base.ordini - 60.0));
    CHECK(p.violazioni == doctest::Approx(base.violazioni));
    // scoperti dentro: l'incidente diplomatico, come un neutrale colpito, e il richiamo
    dentro.scoperto_in_acque = true;
    const Punteggio q = punteggio_missione(dentro, pace, r);
    CHECK(q.violazioni == doctest::Approx(base.violazioni - 150.0));
    Carriera k(r, Carriera::nuovo(r, Marina::usn, "J. R. Halloran", true, Indulgenza::realistica));
    const EsitoCarriera e = k.chiudi_missione(pace, dentro);
    REQUIRE(e.provvedimento);
    CHECK(e.provvedimento->tipo == "richiamo");
    CHECK(e.provvedimento->motivo == "incidente diplomatico nelle acque territoriali");
    Carriera v(r, Carriera::nuovo(r, Marina::vmf, "V. A. Suchkov", true, Indulgenza::realistica));
    CHECK(v.chiudi_missione(pace, dentro).provvedimento->tipo == "biasimo");
    // in guerra non conta; nelle operazioni speciali l'entrata e' l'ordine e la scoperta vale la meta', senza richiamo
    const Punteggio g = punteggio_missione(dentro, contesto("ACQUE", Situazione::guerra), r);
    const Punteggio gb = punteggio_missione(pulita(), contesto("ACQUE", Situazione::guerra), r);
    CHECK(g.ordini == doctest::Approx(gb.ordini));
    CHECK(g.violazioni == doctest::Approx(gb.violazioni));
    ContestoMissione speciale = pace;
    speciale.operazione_speciale = true;
    const Punteggio s1 = punteggio_missione(dentro, speciale, r);
    const Punteggio sb = punteggio_missione(pulita(), speciale, r);
    CHECK(s1.ordini == doctest::Approx(sb.ordini));
    CHECK(s1.violazioni == doctest::Approx(sb.violazioni - 75.0));
    Carriera ks(r, Carriera::nuovo(r, Marina::usn, "A. B. Speciale", true, Indulgenza::realistica));
    CHECK_FALSE(ks.chiudi_missione(speciale, dentro).provvedimento);
}

TEST_CASE("carriera: il riassunto conta le acque territoriali e la scoperta dentro, da una missione vera" *
          doctest::skip(!std::filesystem::exists(M688_DATI "/artiglieria.toml"))) {
    const auto classi = carica_classi(M688_DATI "/classi.toml");
    const std::string sub = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200, rumore = 8, sonar = 19 }";
    const std::string nave = "modello = { massima_kn = 32, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 0, integrita = 100, rumore = 10, sonar = 20 }";
    // il 688 immerso nelle acque svedesi, una Koni che pinga: lo trova e lo avverte con le bombe (dossier 22)
    const auto scena = [&](const std::string& nome, double x) {
        const auto f = std::filesystem::temp_directory_path() / ("m688_prova_carriera_acque_" + nome + ".lua");
        std::ofstream(f) << "return { id = 'ACQUE_" << nome << "', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12,"
                            " paesaggio_sonoro = false, carriera = { situazione = 'pace', difficolta = 3 },"
                            " acque_territoriali = { { nome = 'Svezia', lato = 'neutrale', cerchio = { x = 0, y = 0, r = 12000 } } },"
                            " script = { P = function(u) m688.impulso_sonar(u) end }, varianti = { { probabilita = 1, unita = {\n"
                            "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = "
                         << x << ", y = 0 }, quota_m = 60, rotta = 90, velocita_kn = 0, " << sub << " },\n"
                            "{ id = 2, classe = 'koni', categoria = 'nave', flag = 26, posizione = { x = " << x << ", y = 3000 }, quota_m = 0,"
                            " rotta = 180, velocita_kn = 3, " << nave << " },\n"
                            "}, timer = { { dopo_s = 20, unita = 2, gestore = 'P' }, { dopo_s = 140, unita = 2, gestore = 'P' } } } } }\n";
        return f.string();
    };
    const auto gioca_scena = [&](const std::string& file) {
        Missione m(file, 1);
        DanniMissione dm(classi, M688_DATI, 1, RegolaDanni::simulazione);
        for (int k = 0; k < 9 * 300; ++k) {
            m.avanza(1.0 / 9.0);
            dm.avanza(m, 1.0 / 9.0);
        }
        return riassumi(m);
    };
    const EsitoPerCarriera dentro = gioca_scena(scena("dentro", 0.0));
    CHECK(dentro.acque_territoriali == 1);
    CHECK(dentro.tempo_in_acque_s > 250.0);
    CHECK(dentro.scoperto_in_acque);
    const EsitoPerCarriera fuori = gioca_scena(scena("fuori", 30000.0));
    CHECK(fuori.acque_territoriali == 0);
    CHECK(fuori.tempo_in_acque_s == 0.0);
    CHECK_FALSE(fuori.scoperto_in_acque);
}

TEST_CASE("carriera: rimozione, inchiesta, indulgenza e morte") {
    EsitoPerCarriera neutrale = pulita();
    neutrale.neutrali_colpiti = 1;
    Carriera dura(regole(), Carriera::nuovo(regole(), Marina::usn, "A", true, Indulgenza::realistica));
    EsitoCarriera x = gioca(dura, 1, Situazione::pace, neutrale);
    CHECK(x.provvedimento->tipo == "rimozione");
    CHECK(x.fine_carriera);
    CHECK(x.stato == "rimosso");
    CHECK(!dura.conta(contesto("ALTRA")));   // la carriera e' chiusa

    Carriera buona(regole(), Carriera::nuovo(regole(), Marina::usn, "B", true, Indulgenza::indulgente));
    x = gioca(buona, 1, Situazione::pace, neutrale);
    CHECK(x.provvedimento->tipo == "seconda_possibilita");
    CHECK(!x.fine_carriera);
    CHECK(buona.fascicolo().blocco_promozioni == 2);
    x = gioca(buona, 2, Situazione::pace, neutrale);
    CHECK(x.fine_carriera);

    Carriera arcade(regole(), Carriera::nuovo(regole(), Marina::usn, "C", true, Indulgenza::arcade));
    x = gioca(arcade, 1, Situazione::pace, neutrale);
    CHECK(x.provvedimento->tipo == "malus");
    CHECK(!x.fine_carriera);

    // battello perso con l'equipaggio salvo, buona condotta: inchiesta per fatalita'
    Carriera d(regole(), Carriera::nuovo(regole(), Marina::usn, "D", true, Indulgenza::realistica));
    EsitoPerCarriera perso = pulita();
    perso.battello_perso = perso.equipaggio_salvo = true;
    x = gioca(d, 1, Situazione::guerra, perso);
    CHECK(x.provvedimento->tipo == "inchiesta");
    CHECK(!x.fine_carriera);

    // perso con tutto l'equipaggio in un'azione valorosa: decorazione postuma
    EsitoPerCarriera morte = pulita();
    morte.battello_perso = true;
    x = gioca(d, 2, Situazione::guerra, morte);   // (856,5 - 300) x 1,6 = 890
    CHECK(x.stato == "morto");
    CHECK(x.fine_carriera);
    REQUIRE(x.personale);
    CHECK(x.personale->postuma);
    CHECK(x.personale->motivazione_it[0].find("alla memoria") != std::string::npos);

    // incaglio: richiamo, due commissioni senza promozione
    Carriera i(regole(), Carriera::nuovo(regole(), Marina::usn, "E", true, Indulgenza::realistica));
    EsitoPerCarriera incaglio = pulita();
    incaglio.incaglio = true;
    x = gioca(i, 1, Situazione::pace, incaglio);
    CHECK(x.provvedimento->tipo == "richiamo");
    CHECK(i.fascicolo().blocco_promozioni == 2);
}

TEST_CASE("carriera: commissioni, «up or out» e trasferimento") {
    Carriera k(regole(), Carriera::nuovo(regole(), Marina::usn, "F", true, Indulgenza::realistica));
    EsitoPerCarriera media = pulita(0.0);   // 381,5: sopra la soglia dei mediocri, sotto quella della commissione O-6
    media.esposizioni = {{"ping", 10}};
    EsitoCarriera x;
    for (int n = 1; n <= 9; ++n) x = gioca(k, n, Situazione::pace, media);
    REQUIRE(x.commissione);   // sotto zona alla nona missione al comando
    CHECK(x.commissione->esito == "non_selezionato");
    CHECK(k.fascicolo().mancate == 0);
    x = gioca(k, 10, Situazione::pace, media);
    CHECK(x.commissione->esito == "non_selezionato");
    CHECK(k.fascicolo().mancate == 1);
    for (int n = 11; n <= 12; ++n) CHECK(!gioca(k, n, Situazione::pace, media).commissione);
    x = gioca(k, 13, Situazione::pace, media);
    CHECK(x.commissione->esito == "congedo");
    CHECK(x.stato == "congedo");   // 16-17 anni di servizio: niente pensione
    CHECK(k.fascicolo().fine_comando_data);   // la decorazione di fine comando dopo sei missioni

    Carriera m(regole(), Carriera::nuovo(regole(), Marina::usn, "G", true, Indulgenza::realistica));
    EsitoPerCarriera scarsa;   // nessun obiettivo, scoperto, ordini trascurati, danni: sotto i 150
    scarsa.esposizioni = {{"fix", 4}};
    scarsa.infrazioni_roe = 3;
    scarsa.danni = 1.0;
    for (int n = 1; n <= 2; ++n) CHECK(!gioca(m, n, Situazione::pace, scarsa).fine_carriera);
    x = gioca(m, 3, Situazione::pace, scarsa);
    CHECK(x.provvedimento->tipo == "trasferimento");
    CHECK(x.stato == "trasferito");
}

TEST_CASE("carriera: VMF, classi del Servizio alla Patria e Stella Rossa") {
    Carriera k(regole(), Carriera::nuovo(regole(), Marina::vmf, "Marko Ramius", true, Indulgenza::realistica));
    CHECK(k.fascicolo().grado == "kap2");
    CHECK(k.fascicolo().provenienza == "VVMU «Frunze»");
    EsitoPerCarriera e = pulita(0.25);   // 556,5 x 0,8 = 445 contro un avversario impreparato
    ContestoMissione c = contesto("V1");
    c.avversario = 0;
    EsitoCarriera x = k.chiudi_missione(c, e);
    CHECK(x.personale->decorazione == "servizio_patria_3");
    c.missione = "V2";
    x = k.chiudi_missione(c, e);
    CHECK(x.personale->decorazione == "servizio_patria_2");   // la classe successiva, senza soglia maggiorata
    REQUIRE(x.personale->motivazione_it.size() >= 3);
    CHECK(x.personale->motivazione_it[0] == "Decreto del Presidium del Soviet Supremo dell'URSS.");
    CHECK(x.personale->motivazione_it[1].find("capitano di 2° rango Marko Ramius") != std::string::npos);
    c.missione = "V3";
    c.avversario = 1;
    x = k.chiudi_missione(c, pulita());   // 856,5 -> Bandiera Rossa
    CHECK(x.personale->decorazione == "bandiera_rossa");
    CHECK(x.reparto->decorazione == "encomio_comandante");
}

TEST_CASE("carriera: quali missioni contano") {
    Carriera k(regole(), Carriera::nuovo(regole(), Marina::usn, "H", false, Indulgenza::realistica));
    ContestoMissione c = contesto("rapida");
    c.origine = "rapida";
    CHECK(!k.conta(c));   // senza seme registrato
    c.seme = 42;
    CHECK(k.conta(c));
    c.modificata = true;
    CHECK(!k.conta(c));
    c = contesto("mia.lua");
    c.origine = "editor";
    c.blocco_carriera = true;
    CHECK(!k.conta(c));   // l'editor solo nella carriera aperta
    Carriera aperta(regole(), Carriera::nuovo(regole(), Marina::usn, "H", false, Indulgenza::realistica, true));
    CHECK(aperta.conta(c));
    CHECK(aperta.fascicolo().id == "h_aperta");
    c.blocco_carriera = false;
    CHECK(!aperta.conta(c));
    const EsitoCarriera x = k.chiudi_missione(c, pulita());
    CHECK(!x.contata);
    CHECK(k.fascicolo().missioni.empty());
}

TEST_CASE("carriera: fascicolo salvato e riletto") {
    Carriera k(regole(), Carriera::nuovo(regole(), Marina::usn, "Bart Mancuso", true, Indulgenza::indulgente));
    EsitoPerCarriera e = pulita();
    e.eventi_chiave = {3, 17};
    e.fatti_it = {"Fatto uno."};
    k.chiudi_missione(contesto("SCENE7A", Situazione::guerra), e);
    const auto cartella = std::filesystem::temp_directory_path() / "prove_fascicolo";
    std::filesystem::remove_all(cartella);
    const auto p = cartella / "bart_mancuso.toml";
    salva_fascicolo(k.fascicolo(), p);
    const Fascicolo f = leggi_fascicolo(p);
    const Fascicolo& g = k.fascicolo();
    CHECK(f.id == g.id);
    CHECK(f.nome == g.nome);
    CHECK(f.marina == g.marina);
    CHECK(f.indulgenza == Indulgenza::indulgente);
    CHECK(f.grado == g.grado);
    CHECK(f.data == g.data);
    CHECK(f.seme == g.seme);
    REQUIRE(f.missioni.size() == 1);
    CHECK(f.missioni[0].punti == doctest::Approx(g.missioni[0].punti));
    CHECK(f.missioni[0].giudizio_it == g.missioni[0].giudizio_it);
    REQUIRE(f.decorazioni.size() == g.decorazioni.size());
    const auto nc = std::find_if(f.decorazioni.begin(), f.decorazioni.end(), [](const Conferimento& c) { return c.decorazione == "navy_cross"; });
    REQUIRE(nc != f.decorazioni.end());
    CHECK(nc->fatti == std::vector<int>{3, 17});
    CHECK(nc->motivazione_it.size() == 3);
    CHECK(f.qualifiche.size() == g.qualifiche.size());
    CHECK(f.incarichi.size() == g.incarichi.size());
    CHECK(Carriera(regole(), f).barretta().size() == k.barretta().size());
    std::filesystem::remove_all(cartella);
    CHECK_THROWS(leggi_fascicolo(p));
}

TEST_CASE("carriera: riassunto e contesto da una missione originale") {
    Missione m(std::string(M688_MISSIONI) + "/SCENE1A.lua", 1, false);
    for (int i = 0; i < 9 * 60; ++i) m.avanza(1.0 / 9.0);
    REQUIRE(m.errori().empty());
    const EsitoPerCarriera e = riassumi(m);
    CHECK(e.esposizioni.count("ping"));
    CHECK(!e.battello_perso);
    CHECK(e.lanci == 0);
    const ContestoMissione c = contesto_carriera(m, regole());
    CHECK(c.missione == "SCENE1A");
    CHECK(c.situazione == Situazione::pace);   // TORPEX: esercitazione
    CHECK(c.avversario == 0);
    const auto [avv, diff] = minaccia_calcolata(m);
    CHECK(avv >= 1);   // tre cacciatorpediniere e un elicottero ostili
    CHECK(diff >= 2);
    CHECK(c.battello == "Los Angeles (SSN 688)");
    Carriera k(regole(), Carriera::nuovo(regole(), Marina::usn, "Prova", true, Indulgenza::realistica));
    const EsitoCarriera x = k.chiudi_missione(c, e);
    CHECK(x.contata);
    CHECK(k.fascicolo().missioni.size() == 1);
    // un TLAM-C (carico M2) e' un lancio nell'esito, ma non conta fra i lanci dell'efficienza
    std::vector<std::string> tubi = m.tubi();
    tubi[0] = "attacco_terra";
    m.imposta_tubi(tubi);
    REQUIRE(m.applica({m.tempo(), TipoOrdine::lancia, 0, 0, 1}));
    CHECK(m.esito_dettagliato().per_arma.at("tlam_c").lanci == 1);
    CHECK(riassumi(m).lanci == 0);
}

TEST_CASE("carriera: fuoco senza provocazione nell'esito di una missione originale (SCENE2A)") {
    Missione m(std::string(M688_MISSIONI) + "/SCENE2A.lua", 1);
    m.siluri_enhanced(M688_DATI);
    StazioniMissione st(m);
    RegistroOrdini ordini;
    ordini.ordina({1.0, TipoOrdine::carica_tubo, 0, 0, 1});
    ordini.ordina({40.0, TipoOrdine::lancia, 0, 0, 1});
    for (int i = 0; i < 9 * 1200 && !m.esito(); ++i) {
        for (const auto& o : ordini.consegna(m.tempo())) st.consegna(m, o);
        m.avanza(1.0 / 9.0);
        st.dopo_passo(m);
    }
    REQUIRE(m.esito());
    CHECK((*m.esito() == 7 || *m.esito() == 3));
    const ContestoMissione c = contesto_carriera(m, regole());
    CHECK(c.fuoco_senza_ordine);
    CHECK(c.richiamato);
    const EsitoPerCarriera e = riassumi(m);
    const Punteggio p = punteggio_missione(e, c, regole());
    CHECK(p.violazioni <= -100.0);
    CHECK(p.ordini <= 20.0);
    CHECK(p.giudizio_it.find("ordini e regole d'ingaggio rispettati") == std::string::npos);
    Carriera k(regole(), Carriera::nuovo(regole(), Marina::usn, "Prova", true, Indulgenza::realistica));
    const EsitoCarriera x = k.chiudi_missione(c, e);
    REQUIRE(x.provvedimento);
    CHECK(x.provvedimento->tipo == "richiamo");
    CHECK(!x.personale);   // nessuna decorazione con violazioni
}
