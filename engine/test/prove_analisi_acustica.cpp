#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <string>

#include "nucleo/analisi_acustica.hpp"

using namespace m688;

// Dati della stazione sonar completa (Fase 3, S3; dossier 02 §2.1-2.2, §2.7)

namespace {

const CatalogoAcustico& catalogo() {
    static const CatalogoAcustico c = carica_acustica(M688_DATI "/acustica.toml");
    return c;
}

Sorgente sorgente(const char* classe, double kn, double quota, Vec2 pos, double rotta = 90.0) {
    Sorgente s;
    s.firma = catalogo().firma(classe);
    s.velocita_kn = kn; s.quota_m = quota; s.pos = pos; s.rotta = rotta;
    return s;
}

}  // namespace

TEST_CASE("analisi: righe emesse dai macchinari e dall'elica") {
    // 688 a 10 kn: rete a 60 Hz e armoniche, pompe lente, turbogeneratore, riduttore; asse a 1 Hz, pala a 7 Hz
    const auto righe = righe_emesse(sorgente("los_angeles", 10.0, 150.0, {}));
    const auto c_e = [&](double hz) {
        for (const auto& r : righe) if (std::fabs(r.hz - hz) < 1e-6) return true;
        return false;
    };
    CHECK(c_e(60.0));
    CHECK(c_e(300.0));
    CHECK(c_e(42.3));
    CHECK_FALSE(c_e(84.6));   // pompe veloci da 15 kn
    CHECK(c_e(1.0));
    CHECK(c_e(7.0));
    CHECK(frequenza_asse_hz(sorgente("los_angeles", 10.0, 150.0, {})) == doctest::Approx(1.0));
}

TEST_CASE("analisi: il peschereccio al lavoro tira la rete a giri costanti; l'AGI in deriva col generatore") {
    const auto c_e = [](const std::vector<RigaEmessa>& righe, double hz) {
        for (const auto& r : righe) if (std::fabs(r.hz - hz) < 1e-6) return true;
        return false;
    };
    // a 4 kn al lavoro: i cavi e i verricelli; la pala dell'elica a passo variabile non si sposta con la velocita'
    const auto lavoro = righe_emesse(sorgente("peschereccio", 4.0, 2.0, {}));
    const auto trasferimento = righe_emesse(sorgente("peschereccio", 12.0, 2.0, {}));
    CHECK(c_e(lavoro, 14.0));
    CHECK(c_e(lavoro, 225.0));
    CHECK_FALSE(c_e(trasferimento, 14.0));
    CHECK_FALSE(c_e(trasferimento, 225.0));
    CHECK(c_e(lavoro, 10.7));
    CHECK(c_e(trasferimento, 10.7));
    // al lavoro forte quasi come in trasferimento, piu' che a 7 kn a passo ridotto
    const double l4 = livello_banda_larga(sorgente("peschereccio", 4.0, 2.0, {}));
    CHECK(l4 > livello_banda_larga(sorgente("peschereccio", 7.0, 2.0, {})) + 4.0);
    CHECK(l4 > livello_banda_larga(sorgente("peschereccio", 12.0, 2.0, {})) - 6.0);
    // l'AGI ferma: la rete e il generatore, non il motore principale; in moto anche il motore e la pala
    const auto deriva = righe_emesse(sorgente("agi", 0.0, 2.0, {}));
    CHECK(c_e(deriva, 25.0));
    CHECK(c_e(deriva, 250.0));
    CHECK_FALSE(c_e(deriva, 15.3));
    CHECK(c_e(righe_emesse(sorgente("agi", 9.0, 2.0, {})), 15.3));
    CHECK(livello_banda_larga(sorgente("agi", 0.0, 2.0, {})) < livello_banda_larga(sorgente("mercantile", 5.0, 2.0, {})) - 20.0);
}

namespace {
Ambiente faroe_inverno() {
    static const ClimaTeatro c = carica_clima(M688_DATI "/teatri/faroe.suono.toml");
    Ambiente a;
    a.profilo = &c.profilo(2);
    a.fondale_m = 3000.0;
    a.vento_m_s = 7.0;
    a.traffico = c.traffico;
    a.ph = c.ph;
    return a;
}
Ricevitore ricevitore(const char* sensore) {
    Ricevitore r;
    r.quota_m = 150.0;
    r.velocita_kn = 5.0;
    r.sensore = catalogo().sensore(sensore);
    return r;
}
bool ha_riga(const std::vector<RigaOsservata>& righe, double hz, double tolleranza = 0.2) {
    for (const auto& r : righe) if (std::fabs(r.hz - hz) <= tolleranza) return true;
    return false;
}
}  // namespace

TEST_CASE("analisi: diesel a snorkel, battello posato e cavitazione sulle righe") {
    Sorgente kilo = sorgente("kilo", 5.0, 15.0, {});
    const auto diesel = [](const std::vector<RigaEmessa>& r) {
        for (const auto& x : r) if (x.tipo == TipoRiga::diesel) return true;
        return false;
    };
    CHECK_FALSE(diesel(righe_emesse(kilo)));
    kilo.snorkel = true;
    CHECK(diesel(righe_emesse(kilo)));
    CHECK(diesel(righe_emesse(sorgente("mercantile", 12.0, 0.0, {}))));   // i mercantili sempre
    Sorgente la = sorgente("los_angeles", 10.0, 150.0, {}), posato = la;
    posato.sul_fondo = true;
    CHECK(righe_emesse(posato)[0].db == doctest::Approx(righe_emesse(la)[0].db - 10.0));
    // la riga di pala sale di 10 dB quando l'elica cava (piu' il livello a banda larga)
    Sorgente cava = la;
    cava.cavitazione = true;
    const auto pala = [](const std::vector<RigaEmessa>& r) {
        for (const auto& x : r) if (x.tipo == TipoRiga::elica && std::fabs(x.hz - 7.0) < 1e-6) return x.db;
        return -1e9;
    };
    CHECK(pala(righe_emesse(cava)) >= pala(righe_emesse(la)) + 10.0);
    // pump-jet e biologici: nessuna riga d'elica
    CHECK(frequenza_asse_hz(sorgente("trafalgar", 10.0, 150.0, {})) == 0.0);
    CHECK(frequenza_asse_hz(sorgente("biologico", 2.0, 50.0, {})) == 0.0);
}

TEST_CASE("analisi: spettro LOFAR del fascio (righe, Doppler, filtri, fascio)") {
    const Ambiente amb = faroe_inverno();
    const Ricevitore r = ricevitore("tb16");
    const Sorgente charlie = sorgente("charlie2", 8.0, 150.0, {3000.0, 0.0}, 0.0);
    const Spettro sp = spettro_fascio({charlie}, r, amb, 90.0, 10.0, 410.0, 0.1);
    CHECK(sp.snr_db.size() == 4000);
    const auto righe = righe_osservate(sp);
    for (double hz : {31.0, 50.0, 72.0, 100.0, 118.0, 150.0, 196.0, 250.0}) CHECK_MESSAGE(ha_riga(righe, hz), hz);
    // filtro: fuori dalla banda scelta niente
    const auto alte = righe_osservate(spettro_fascio({charlie}, r, amb, 90.0, 200.0, 410.0, 0.1));
    CHECK_FALSE(ha_riga(alte, 31.0));
    CHECK(ha_riga(alte, 250.0));
    // fascio puntato altrove: la sorgente sparisce (tranne lo specchio della schiera, a 270 gradi)
    CHECK(righe_osservate(spettro_fascio({charlie}, r, amb, 180.0, 200.0, 410.0, 0.1)).empty());
    CHECK(ha_riga(righe_osservate(spettro_fascio({charlie}, r, amb, 270.0, 200.0, 410.0, 0.1)), 250.0));
    // Doppler: a 12 kn verso il ricevitore la riga dei 250 Hz sale di circa 1 Hz (a 20 kn il Charlie cava e il rumore
    // copre le righe)
    const Sorgente avvicina = sorgente("charlie2", 12.0, 150.0, {3000.0, 0.0}, 270.0);
    const auto d = righe_osservate(spettro_fascio({avvicina}, r, amb, 90.0, 240.0, 260.0, 0.1));
    CHECK(ha_riga(d, 250.0 * (1.0 + 12.0 * NODO / 1500.0), 0.15));
    CHECK_FALSE(ha_riga(d, 250.0, 0.4));
    // la distanza toglie le righe deboli: a 20 km restano le piu' forti
    const auto lontane = righe_osservate(spettro_fascio({sorgente("charlie2", 8.0, 150.0, {20000.0, 0.0}, 0.0)}, r, amb, 90.0, 10.0, 410.0, 0.1));
    CHECK(lontane.size() < righe.size());
    CHECK(ha_riga(lontane, 250.0));
}

TEST_CASE("analisi: analizzatore di firma con confidenza, ambiguita' e DEMON") {
    const Ambiente amb = faroe_inverno();
    const Ricevitore r = ricevitore("tb16");
    const auto analizza = [&](const char* classe, double d, std::optional<int> pale = std::nullopt) {
        const Spettro sp = spettro_fascio({sorgente(classe, 8.0, 150.0, {d, 0.0}, 0.0)}, r, amb, 90.0, 5.0, 410.0, 0.1);
        return confronta_firma(righe_osservate(sp), catalogo(), pale, 0.1);
    };
    // molte righe: la classe giusta con alta confidenza
    const auto c = analizza("charlie2", 3000.0);
    REQUIRE_FALSE(c.empty());
    CHECK(c[0].classe == "charlie2");
    CHECK(c[0].confidenza > 0.9);
    CHECK(c[0].nazionalita == "urss");
    CHECK(c[0].tipo_unita == "ssgn");
    const auto m = analizza("mercantile", 3000.0);
    CHECK(m[0].classe == "mercantile");
    CHECK(m[0].confidenza > 0.5);
    // una riga sola (battello silenzioso): ipotesi vicine, ma la nazionalita' si legge dalla rete a 50 Hz
    const auto v = analizza("victor3", 3000.0);
    CHECK(v[0].confidenza < 0.3);
    CHECK(v[0].nazionalita == "urss");
    // senza righe: nessuna ipotesi prevale
    const auto vuota = confronta_firma({}, catalogo());
    CHECK(vuota[0].confidenza < 0.1);
    // il numero di pale dal DEMON aiuta (Victor III: eliche in tandem a 4 pale)
    const auto con_pale = analizza("victor3", 3000.0, 4);
    CHECK(con_pale[0].confidenza > v[0].confidenza);
    for (std::size_t i = 0; i < 3; ++i) CHECK(catalogo().firma(con_pale[i].classe)->pale == 4);
    // la somma delle confidenze e' 1
    double somma = 0.0;
    for (const auto& h : c) somma += h.confidenza;
    CHECK(somma == doctest::Approx(1.0));
}

TEST_CASE("analisi: cascata per rilevamento (traccia, specchio della schiera, settori ciechi)") {
    const Ambiente amb = faroe_inverno();
    const Sorgente mercantile = sorgente("mercantile", 12.0, 0.0, {-7000.0, 7000.0}, 0.0);   // per 315, vicino: lo spettro delle civili e' 17 dB piu' basso
    const Ricevitore tb16 = ricevitore("tb16");
    const RigaCascata t = cascata({mercantile}, tb16, amb, 72);
    REQUIRE(t.db.size() == 72);
    CHECK(t.passo == 5.0);
    const auto massimo = static_cast<std::size_t>(std::max_element(t.db.begin(), t.db.end()) - t.db.begin());
    CHECK((massimo == 63 || massimo == 9));                 // 315 o il suo specchio a 45
    CHECK(t.db[63] == doctest::Approx(t.db[9]).epsilon(0.05));   // ambiguita' destra-sinistra
    CHECK(t.db[63] > t.db[36] + 15.0);                       // il fascio di poppa non lo vede
    CHECK(t.cieco[0]);                                       // endfire verso il battello
    CHECK_FALSE(t.cieco[18]);
    // la sfera non ha lo specchio ma ha i baffles a poppa
    const RigaCascata s = cascata({mercantile}, ricevitore("bqq5_sfera"), amb, 72);
    CHECK(s.db[63] > s.db[9] + 3.0);
    CHECK(s.cieco[36]);
    CHECK_FALSE(s.cieco[0]);
    // la grana e' ripetibile con lo stesso seme
    Casuale a(3), b(3);
    CHECK(cascata({mercantile}, tb16, amb, 72, 0.0, &a).db == cascata({mercantile}, tb16, amb, 72, 0.0, &b).db);
}

TEST_CASE("analisi: DEMON (asse, pala, numero di pale) solo con la cavitazione") {
    const Ambiente amb = faroe_inverno();
    const Ricevitore r = ricevitore("bqq5_sfera");
    const Sorgente veloce = sorgente("victor3", 20.0, 30.0, {8000.0, 0.0});   // a 30 m e 20 kn cava
    REQUIRE(cavita(*veloce.firma, 20.0, 30.0));
    const auto l = leggi_demon(demon(veloce, r, amb));
    REQUIRE(l);
    CHECK(l->pale == 4);
    CHECK(l->asse_hz == doctest::Approx(8.0 * 20.0 / 60.0).epsilon(0.03));
    CHECK(60.0 * l->asse_hz / catalogo().firma("victor3")->giri_per_nodo == doctest::Approx(20.0).epsilon(0.03));   // la velocita'
    // a 150 m e 8 kn non cava: niente DEMON
    CHECK_FALSE(leggi_demon(demon(sorgente("victor3", 8.0, 150.0, {8000.0, 0.0}), r, amb)));
    const Spettro piatto = demon(sorgente("victor3", 8.0, 150.0, {8000.0, 0.0}), r, amb);
    CHECK(*std::max_element(piatto.snr_db.begin(), piatto.snr_db.end()) == 0.0f);
    // con la cavitazione dalla dinamica il DEMON c'e' anche in profondita'
    Sorgente forzata = sorgente("akula", 12.0, 150.0, {8000.0, 0.0});
    forzata.cavitazione = true;
    const auto la = leggi_demon(demon(forzata, r, amb));
    REQUIRE(la);
    CHECK(la->pale == 7);
}

TEST_CASE("libreria: elenco per scheda e voci di classi e sorgenti") {
    const auto& c = catalogo();
    const auto tutte = elenco_libreria(c);
    const auto navali = elenco_libreria(c, "navale"), bio = elenco_libreria(c, "biologico"), umani = elenco_libreria(c, "umano");
    CHECK(tutte.size() == navali.size() + bio.size() + elenco_libreria(c, "geofisico").size() +
                              elenco_libreria(c, "meteo").size() + umani.size());
    CHECK(std::find(navali.begin(), navali.end(), "los_angeles") != navali.end());
    CHECK(std::find(navali.begin(), navali.end(), "mk48") != navali.end());   // R7: i siluri hanno le loro righe
    CHECK(std::find(navali.begin(), navali.end(), "sottomarino_generico") == navali.end());   // le generiche no
    CHECK(std::find(bio.begin(), bio.end(), "balenottera_comune") != bio.end());
    CHECK(std::find(umani.begin(), umani.end(), "airgun") != umani.end());              // industriale
    CHECK(std::find(umani.begin(), umani.end(), "peschereccio_piccolo") != umani.end()); // civile
    // voce di una classe: etichette leggibili, elica, nazionalita'
    const auto la = voce_libreria(c, "los_angeles");
    REQUIRE(la);
    CHECK(la->unita_navale);
    CHECK(la->categoria_scheda == "navale");
    CHECK(la->nazionalita == "usa");
    CHECK(la->pale == 7);
    bool quinta = false, veloce = false;
    for (const auto& r : la->righe) {
        quinta |= r.etichetta == "rete 60 Hz x5";
        veloce |= r.etichetta == "pompa 84.6 Hz (da 15 kn)";
    }
    CHECK(quinta);
    CHECK(veloce);
    // voce di una sorgente: famiglia, sweep, bande e ritmo descritto
    const auto t = voce_libreria(c, "tremore_iceberg");
    REQUIRE(t);
    CHECK_FALSE(t->unita_navale);
    CHECK(t->categoria_scheda == "geofisico");
    CHECK(t->righe.size() == 8);
    const auto b = voce_libreria(c, "balenottera_comune");
    REQUIRE(b);
    CHECK(b->righe[0].hz == 40.0);
    CHECK(b->righe[0].hz_fine == 15.0);
    CHECK(b->ritmo.find("gruppi di 1-2") != std::string::npos);
    CHECK(voce_libreria(c, "gamberi")->bande.size() == 1);
    CHECK_FALSE(voce_libreria(c, "inesistente"));
}

TEST_CASE("libreria: confronto per rapporti insensibile al Doppler, con il perche'") {
    const auto& c = catalogo();
    // le righe del Charlie II spostate dell'0,8% (circa 15,6 kn di avvicinamento), piu' una riga estranea
    const double k = 1.008;
    std::vector<RigaOsservata> oss;
    for (const auto& r : c.firma("charlie2")->righe)
        if (r.multiplo_asse <= 0.0) oss.push_back({r.hz * k, 10.0});   // le fisse: quelle legate all'asse vanno coi giri
    oss.push_back({333.3, 6.0});
    const auto r = confronta_rapporti(oss, c);
    REQUIRE_FALSE(r.empty());
    CHECK(r[0].id == "charlie2");
    CHECK(r[0].fattore_doppler == doctest::Approx(k).epsilon(1e-4));
    CHECK(r[0].velocita_radiale_kn == doctest::Approx((k - 1.0) * 1500.0 / NODO).epsilon(0.02));
    CHECK(r[0].velocita_radiale_kn > 0.0);   // le righe salgono: si avvicina
    // le righe legate all'asse (blocco 2 del sonar) non sono nella libreria: si confrontano solo le fisse
    const auto& righe_charlie = c.firma("charlie2")->righe;
    CHECK(r[0].concordi.size() == static_cast<std::size_t>(std::count_if(righe_charlie.begin(), righe_charlie.end(),
                                                                         [](const RigaFirma& x) { return x.multiplo_asse <= 0.0; })));
    CHECK(r[0].mancanti.empty());
    REQUIRE(r[0].in_piu.size() == 1);
    CHECK(r[0].in_piu[0].hz == 333.3);
    CHECK(r[0].confidenza > 0.9);
    // il confronto senza Doppler (tolleranza 0,6%) perde le righe alte con lo 0,8%
    const auto f = confronta_firma(oss, c);
    CHECK(f[0].righe_concordi < static_cast<int>(c.firma("charlie2")->righe.size()));
    // con poche righe: le mancanti dicono cosa cercare, le piu' forti prima
    const auto poche = confronta_rapporti({{250.0, 8.0}, {31.0, 5.0}}, c);
    const auto ch = std::find_if(poche.begin(), poche.end(), [](const auto& x) { return x.id == "charlie2"; });
    REQUIRE(ch != poche.end());
    REQUIRE(ch->mancanti.size() >= 2);
    CHECK(ch->mancanti[0].db_relativo >= ch->mancanti[1].db_relativo);
    // una sorgente della libreria: il tremore d'iceberg dalle sue armoniche
    std::vector<RigaOsservata> tremore;
    for (int n = 1; n <= 5; ++n) tremore.push_back({40.0 * n * 0.999, 8.0});
    CHECK(confronta_rapporti(tremore, c)[0].id == "tremore_iceberg");
    double somma = 0.0;
    for (const auto& x : r) somma += x.confidenza;
    CHECK(somma == doctest::Approx(1.0));
}

TEST_CASE("libreria: famiglie armoniche (rete, libreria, elica con le pale)") {
    const auto& c = catalogo();
    const auto rete = famiglie_armoniche({{60.0, 6}, {120.0, 5}, {180.0, 4}, {300.0, 9}}, 410.0, &c);
    REQUIRE_FALSE(rete.empty());
    CHECK(rete[0].f1_hz == doctest::Approx(60.0));
    CHECK(rete[0].indizio == "rete 60 Hz");
    CHECK(rete[0].armoniche == std::vector<int>{1, 2, 3, 5});
    CHECK(rete[0].copertura == doctest::Approx(4.0 / 6.0));
    const auto ghiaccio = famiglie_armoniche({{40.1, 6}, {80.2, 5}, {120.3, 4}, {160.4, 4}}, 410.0, &c);
    REQUIRE_FALSE(ghiaccio.empty());
    CHECK(ghiaccio[0].indizio == c.sorgente("tremore_iceberg")->nome_breve);
    // elica: pala a 10,5 Hz con le armoniche, e la riga d'asse a 1,5 Hz: 7 pale
    const auto elica = famiglie_armoniche({{1.5, 4}, {10.5, 9}, {21.0, 6}, {31.5, 4}}, 50.0, &c);
    REQUIRE_FALSE(elica.empty());
    CHECK(elica[0].indizio == "elica (pala)");
    CHECK(elica[0].pala_hz == doctest::Approx(10.5));
    CHECK(elica[0].pale_stimate == 7);
    CHECK(elica.size() == 1);   // la «famiglia a 1,5 Hz» non si aggiunge
    // due famiglie distinte nella stessa lista
    const auto due = famiglie_armoniche({{50.0, 6}, {100.0, 5}, {150.0, 4}, {44.0, 5}, {88.0, 5}, {132.0, 4}}, 410.0, &c);
    REQUIRE(due.size() == 2);
    CHECK((due[0].indizio == "rete 50 Hz" || due[1].indizio == "rete 50 Hz"));
    CHECK(famiglie_armoniche({{50.0, 6}, {137.0, 5}}, 410.0, &c).empty());   // meno di tre membri
    // la rete a 50 Hz non diventa una sottoarmonica a 25 Hz «strascico», anche con tre multipli dispari di 25 in piu'
    const auto charlie = famiglie_armoniche({{50.0, 6}, {75.0, 4}, {100.0, 5}, {150.0, 5}, {175.0, 4}, {225.0, 4}, {250.0, 9}}, 410.0, &c);
    REQUIRE_FALSE(charlie.empty());
    CHECK(charlie[0].indizio == "rete 50 Hz");
    for (const auto& f : charlie) CHECK(f.indizio != c.sorgente("peschereccio_strascico")->nome_breve);
    // la famiglia della libreria si riconosce solo con abbastanza armoniche: 25, 50, 75 Hz si', 75, 175, 225 no
    CHECK(famiglie_armoniche({{25.0, 6}, {50.0, 5}, {75.0, 5}}, 410.0, &c)[0].indizio == c.sorgente("peschereccio_strascico")->nome_breve);
    CHECK(famiglie_armoniche({{75.0, 6}, {175.0, 5}, {225.0, 5}}, 410.0, &c).empty());   // sparse: niente famiglia
    // il Charlie II dal LOFAR: una sola famiglia, la rete a 50 Hz, e niente «elica» spuria
    {
        static const ClimaTeatro cl = carica_clima(M688_DATI "/teatri/faroe.suono.toml");
        Ambiente amb;
        amb.profilo = &cl.profilo(2);
        amb.fondale_m = 3000.0;
        amb.vento_m_s = 7.0;
        Ricevitore r;
        r.quota_m = 150.0;
        r.velocita_kn = 5.0;
        r.sensore = c.sensore("tb16");
        const auto oss = righe_osservate(spettro_fascio({sorgente("charlie2", 8.0, 150.0, {3000.0, 0.0}, 0.0)}, r, amb, 90.0, 10.0, 410.0, 0.1));
        const auto f = famiglie_armoniche(oss, 410.0, &c);
        REQUIRE(f.size() == 1);
        CHECK(f[0].indizio == "rete 50 Hz");
    }
}

TEST_CASE("libreria: differenza fra spettri per gli slot") {
    Spettro a, b;
    a.f0_hz = b.f0_hz = 10.0;
    a.df_hz = b.df_hz = 0.5;
    a.snr_db = {0, 3, 10, 3, 0};
    b.snr_db = {0, 3, 4, 3, 0};
    const Spettro d = differenza(a, b);
    REQUIRE(d.snr_db.size() == 5);
    CHECK(d.snr_db[2] == doctest::Approx(6.0));
    CHECK(d.snr_db[0] == doctest::Approx(0.0));
    // griglie diverse: b interpolato sulla griglia di a
    Spettro fine;
    fine.f0_hz = 10.0;
    fine.df_hz = 0.25;
    fine.snr_db = {0, 0, 2, 2, 4, 4, 2, 2, 0, 0};
    const Spettro e = differenza(a, fine);
    CHECK(e.snr_db.size() == a.snr_db.size());
    CHECK(e.snr_db[2] == doctest::Approx(6.0));
}

TEST_CASE("analisi: il DEMON del fascio, con piu' sorgenti, e le sue serie (blocco 3 del sonar)") {
    const auto& c = catalogo();
    const Ambiente amb = faroe_inverno();
    const Ricevitore r = ricevitore("bqq5_sfera");
    Sorgente charlie = sorgente("charlie2", 12.0, 100.0, {3000.0, 0.0}, 0.0);
    charlie.cavitazione = true;
    Sorgente victor = sorgente("victor3", 18.0, 100.0, {5000.0, 0.0}, 0.0);
    victor.cavitazione = true;
    // una sola: lo stesso DEMON di demon() sulla sorgente, una serie con le pale del Charlie
    const Spettro uno = demon_fascio({charlie}, r, amb, 90.0);
    CHECK(uno.snr_db == demon(charlie, r, amb).snr_db);
    const auto s1 = leggi_demon_fascio(uno);
    REQUIRE(s1.size() == 1);
    CHECK(s1[0].pale == c.firma("charlie2")->pale);
    // due nello stesso fascio: due serie, con le loro pale
    const auto s2 = leggi_demon_fascio(demon_fascio({charlie, victor}, r, amb, 90.0));
    REQUIRE(s2.size() == 2);
    std::vector<int> pale = {s2[0].pale, s2[1].pale};
    std::sort(pale.begin(), pale.end());
    CHECK(pale == std::vector<int>{c.firma("victor3")->pale, c.firma("charlie2")->pale});
    // senza cavitazione piatto; il fascio puntato altrove non le sente
    Sorgente quieto = charlie;
    quieto.cavitazione = false;
    CHECK(leggi_demon_fascio(demon_fascio({quieto}, r, amb, 90.0)).empty());
    CHECK(leggi_demon_fascio(demon_fascio({charlie}, r, amb, 270.0)).empty());
    // il bersaglio accelera: l'asse sale coi giri (indizio di manovra)
    Sorgente veloce = charlie;
    veloce.velocita_kn = 18.0;
    const auto sv = leggi_demon_fascio(demon_fascio({veloce}, r, amb, 90.0));
    REQUIRE(sv.size() == 1);
    CHECK(sv[0].asse_hz > s1[0].asse_hz * 1.3);
}

// Il primo lotto della libreria allargata (docs/ricerca/31): le cinque classi ci sono, coi dati delle fonti (assi, pale,
// nazione, tipo), il rumore nell'ordine delle generazioni, e l'analizzatore le riconosce dalle loro righe
TEST_CASE("libreria allargata, primo lotto: Victor I, Delta III, Sierra I, Udaloy, Grisha III") {
    const auto& c = catalogo();
    struct Atteso { const char* classe; const char* tipo; int pale, assi; bool nave; };
    for (const Atteso& a : {Atteso{"victor1", "ssn", 5, 1, false}, Atteso{"delta3", "ssbn", 5, 2, false}, Atteso{"sierra", "ssn", 7, 1, false},
                            Atteso{"udaloy", "cacciatorpediniere", 5, 2, true}, Atteso{"grisha3", "corvetta", 4, 3, true}}) {
        CAPTURE(std::string(a.classe));
        const auto* f = c.firma(a.classe);
        REQUIRE(f);
        CHECK(f->pale == a.pale);
        CHECK(f->assi == a.assi);
        CHECK(f->nave == a.nave);
        const auto v = voce_libreria(c, a.classe);
        REQUIRE(v);
        CHECK(v->nazionalita == "urss");
        CHECK(v->tipo_unita == a.tipo);
        bool rete = false;
        for (const auto& r : v->righe) rete |= r.etichetta == "rete 50 Hz x5";
        CHECK(rete);
    }
    // il rumore a 8 nodi nell'ordine delle generazioni: Victor I sopra il Charlie II, il Delta III fra il Charlie II e il
    // Victor III, il Sierra fra il Victor III e l'Akula
    const auto l = [](const char* k) { return livello_banda_larga(sorgente(k, 8.0, 150.0, {})); };
    CHECK(l("victor1") > l("charlie2"));
    CHECK(l("delta3") < l("charlie2"));
    CHECK(l("delta3") > l("victor3"));
    CHECK(l("sierra") < l("victor3"));
    CHECK(l("sierra") > l("akula"));
    // l'analizzatore di firma: le rumorose si riconoscono dalle loro righe a 3 km, la nazione sempre dalla rete
    const Ambiente amb = faroe_inverno();
    const Ricevitore r = ricevitore("tb16");
    const auto analizza = [&](const char* classe, double kn) {
        const Spettro sp = spettro_fascio({sorgente(classe, kn, classe[0] == 'u' || classe[0] == 'g' ? 0.0 : 150.0, {3000.0, 0.0}, 0.0)},
                                          r, amb, 90.0, 5.0, 410.0, 0.1);
        return confronta_firma(righe_osservate(sp), catalogo(), std::nullopt, 0.1);
    };
    for (const char* k : {"victor1", "udaloy", "grisha3"}) {   // rumorose: la classe dalle righe
        CAPTURE(std::string(k));
        const auto h = analizza(k, 8.0);
        REQUIRE_FALSE(h.empty());
        CHECK(h[0].classe == k);
        CHECK(h[0].confidenza > 0.3);   // la prima ipotesi; con poche righe la confidenza resta media
        CHECK(h[0].nazionalita == "urss");
    }
    for (const char* k : {"delta3", "sierra"}) {   // silenziose come il Victor III: a 3 km solo la rete, la nazione si'
        CAPTURE(std::string(k));
        const auto h = analizza(k, 8.0);
        REQUIRE_FALSE(h.empty());
        CHECK(h[0].confidenza < 0.3);
        CHECK(h[0].nazionalita == "urss");
    }
}

// Il secondo lotto (docs/ricerca/32): Ohio, Ticonderoga, Knox, Type 22, Type 206; il rumore nell'ordine che dicono le fonti
// (l'Ohio sotto il 688, il 206 sotto il Kilo, la Knox a vapore sopra le turbine con la cortina), la nazione dalla rete a
// 60 Hz, le navi riconosciute dalle loro righe
TEST_CASE("libreria allargata, secondo lotto: Ohio, Ticonderoga, Knox, Type 22, Type 206") {
    const auto& c = catalogo();
    struct Atteso { const char* classe; const char* nazione; const char* tipo; int pale, assi; };
    for (const Atteso& a : {Atteso{"ohio", "usa", "ssbn", 7, 1}, Atteso{"type206", "germania", "ssk", 5, 1},
                            Atteso{"ticonderoga", "usa", "incrociatore", 5, 2}, Atteso{"knox", "usa", "fregata", 5, 1},
                            Atteso{"type22", "uk", "fregata", 5, 2}}) {
        CAPTURE(std::string(a.classe));
        const auto* f = c.firma(a.classe);
        REQUIRE(f);
        CHECK(f->pale == a.pale);
        CHECK(f->assi == a.assi);
        const auto v = voce_libreria(c, a.classe);
        REQUIRE(v);
        CHECK(v->nazionalita == a.nazione);
        CHECK(v->tipo_unita == a.tipo);
        bool rete = false;
        for (const auto& r : v->righe) rete |= r.etichetta == "rete 60 Hz x5";
        CHECK(rete);
    }
    const auto l = [&](const char* k, double kn) { return livello_banda_larga(sorgente(k, kn, c.firma(k)->nave ? 0.0 : 100.0, {})); };
    CHECK(l("ohio", 8.0) < l("los_angeles", 8.0));
    CHECK(l("ohio", 20.0) < l("los_angeles", 8.0) + 8.0);   // a 20 nodi quieto quasi come il 688 a 8 (31 §1: come un Lafayette a 6)
    CHECK(l("type206", 8.0) < l("kilo", 8.0));
    CHECK(l("knox", 15.0) > l("spruance", 15.0));
    // l'analizzatore: le navi dalle loro righe a 3 km, la nazione dalla rete; il 206 silenzioso a batteria dice solo la rete
    const Ambiente amb = faroe_inverno();
    const Ricevitore r = ricevitore("tb16");
    const auto analizza = [&](const char* classe, double kn, double quota, double d = 3000.0) {
        const Spettro sp = spettro_fascio({sorgente(classe, kn, quota, {d, 0.0}, 0.0)}, r, amb, 90.0, 5.0, 410.0, 0.1);
        return confronta_firma(righe_osservate(sp), catalogo(), std::nullopt, 0.1);
    };
    for (const char* k : {"ticonderoga", "knox", "type22"}) {
        CAPTURE(std::string(k));
        const auto h = analizza(k, 8.0, 0.0);
        REQUIRE_FALSE(h.empty());
        CHECK(h[0].classe == k);
        CHECK(h[0].nazionalita != "urss");
    }
    // il 206 a batteria a 4 nodi: a 3 km non si sente riga, a 800 m solo la rete (300 Hz, 60 x 5), che non lo fa sembrare
    // sovietico (STIMA della rete, docs/ricerca/32 §5)
    const auto s = analizza("type206", 4.0, 150.0, 800.0);
    REQUIRE_FALSE(s.empty());
    CHECK(s[0].nazionalita != "urss");
}

// il falso contatto del vetrino di Dio (docs/28 §3.5, D3): la firma rovinata dalla fedelta'
TEST_CASE("analisi: il falso contatto, le righe e le pale rovinate dalla fedelta'") {
    const Sorgente vera = sorgente("victor3", 20.0, 30.0, {8000.0, 0.0});
    const auto piene = righe_emesse(vera);
    Sorgente uguale = vera;
    uguale.fedelta = 1.0;
    CHECK(righe_emesse(uguale).size() == piene.size());
    // a 0,8: le pale ci sono, manca circa un quinto delle righe, le altre 1,2 dB sotto
    Sorgente buona = vera;
    buona.fedelta = 0.8;
    const auto r8 = righe_emesse(buona);
    CHECK(r8.size() < piene.size());
    CHECK(std::any_of(r8.begin(), r8.end(), [](const RigaEmessa& x) { return x.tipo == TipoRiga::elica; }));
    for (const auto& x : r8) {
        const auto p = std::find_if(piene.begin(), piene.end(), [&](const RigaEmessa& y) { return y.hz == x.hz; });
        REQUIRE(p != piene.end());
        CHECK(x.db == doctest::Approx(p->db - 1.2));
    }
    // a 0,5: niente righe d'elica e niente DEMON (una sorgente senza elica vera)
    Sorgente cattiva = vera;
    cattiva.fedelta = 0.5;
    const auto r5 = righe_emesse(cattiva);
    CHECK(r5.size() < r8.size());
    CHECK(std::none_of(r5.begin(), r5.end(), [](const RigaEmessa& x) { return x.tipo == TipoRiga::elica; }));
    const Ambiente amb = faroe_inverno();
    const Ricevitore r = ricevitore("bqq5_sfera");
    CHECK(leggi_demon(demon(vera, r, amb)));
    CHECK_FALSE(leggi_demon(demon(cattiva, r, amb)));
    // la stessa sorgente perde sempre le stesse righe
    CHECK(righe_emesse(cattiva).size() == r5.size());
}
