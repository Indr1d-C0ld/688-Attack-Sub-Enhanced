#include <doctest.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <map>
#include <memory>
#include <set>
#include <string>

#include "nucleo/acustica.hpp"
#include "nucleo/classi.hpp"

using namespace m688;

// Acustica dell'Enhanced (Fase 2; dossier 02 §1-4 e §10, dossier 01 §3)

namespace {

const CatalogoAcustico& catalogo() {
    static const CatalogoAcustico c = carica_acustica(M688_DATI "/acustica.toml");
    return c;
}
const ClimaTeatro& clima(const char* teatro) {
    static std::map<std::string, ClimaTeatro> cache;
    auto it = cache.find(teatro);
    if (it == cache.end())
        it = cache.emplace(teatro, carica_clima(std::string(M688_DATI "/teatri/") + teatro + ".suono.toml")).first;
    return it->second;
}

// profilo sintetico con un condotto di 30 m sopra un termoclino (dossier 02 §3.4 b: taglio a circa 590 Hz)
ProfiloSuono condotto_30m() {
    ProfiloSuono p;
    p.z_m = {0.0, 30.0, 200.0, 1000.0};
    p.c_m_s = {1500.0, 1500.5, 1490.0, 1485.0};
    p.analizza();
    return p;
}

Sorgente sorgente(const std::string& classe, double kn, double quota, Vec2 pos, double rotta = 90.0) {
    Sorgente s;
    s.firma = catalogo().firma(classe);
    s.velocita_kn = kn; s.quota_m = quota; s.pos = pos; s.rotta = rotta;
    return s;
}

// distanza massima con SE >= 0, a passi del 5%, al traverso del ricevitore (a est, ricevitore con rotta 0)
double portata(Sorgente s, const Ricevitore& r, const Ambiente& amb) {
    double out = 0.0;
    for (double d = 200.0; d < 200000.0; d *= 1.05) {
        s.pos = {r.pos.x + d, r.pos.y};
        if (valuta(s, r, amb).se_db >= 0.0) out = d;
    }
    return out;
}

}  // namespace

TEST_CASE("acustica: formule di base (velocita' del suono, assorbimento, rumore ambiente, stato del mare)") {
    CHECK(velocita_suono_mackenzie(10.0, 35.0, 0.0) == doctest::Approx(1489.80).epsilon(1e-4));
    // tabella del §3.2 (Atlantico 4 gradi, 35 psu, pH 8)
    CHECK(assorbimento_db_km(1000.0, 4.0, 35.0, 0.0) == doctest::Approx(0.064).epsilon(0.03));
    CHECK(assorbimento_db_km(3500.0, 4.0, 35.0, 0.0) == doctest::Approx(0.23).epsilon(0.05));
    CHECK(assorbimento_db_km(10000.0, 4.0, 35.0, 0.0) == doctest::Approx(1.17).epsilon(0.05));
    CHECK(assorbimento_thorp_db_km(1000.0) == doctest::Approx(0.069).epsilon(0.05));
    // Coates con traffico 0,5 (tabella del §4.1): 1 kHz a 5 m/s 61 dB, 100 Hz a 0 m/s 67 dB
    Ambiente a;
    a.traffico = 0.5;
    a.vento_m_s = 5.0;
    CHECK(rumore_ambiente(1000.0, a) == doctest::Approx(61.0).epsilon(0.02));
    a.vento_m_s = 0.0;
    CHECK(rumore_ambiente(100.0, a) == doctest::Approx(67.0).epsilon(0.02));
    // pioggia, gamberi e margine del ghiaccio alzano il rumore
    const double base = rumore_ambiente(5000.0, a);
    Ambiente p = a;
    p.pioggia_mm_h = 50.0;
    CHECK(rumore_ambiente(5000.0, p) > base + 10.0);
    Ambiente g = a;
    g.gamberi = true;
    CHECK(rumore_ambiente(5000.0, g) > base + 20.0);
    Ambiente i = a;
    i.ghiaccio = 0.5;
    CHECK(rumore_ambiente(1000.0, i) > rumore_ambiente(1000.0, a) + 5.0);
    CHECK(stato_del_mare(5.0) == 2);    // 9,7 kn
    CHECK(stato_del_mare(10.0) == 4);   // 19,4 kn
    CHECK(stato_del_mare(vento_per_stato_del_mare(5)) == 5);
}

TEST_CASE("acustica: profili dei teatri, layer, asse e zone di convergenza") {
    for (const char* t : {"baltico", "barents", "celtica", "faroe", "gibilterra", "kunashir", "otranto"})
        for (int m = 1; m <= 12; ++m) CHECK(clima(t).profilo(m).z_m.size() >= 2);
    // Mar di Norvegia d'inverno (dossier §3.4 d: circa 32 nm = 59 km); Mediterraneo d'estate (22 nm = 41 km), asse
    // a 100-200 m; d'inverno il Mediterraneo e' isotermo e la CZ non c'e'
    CHECK(clima("faroe").profilo(2).distanza_cz_m == doctest::Approx(55000.0).epsilon(0.2));
    const ProfiloSuono& otranto_ago = clima("otranto").profilo(8);
    CHECK(otranto_ago.distanza_cz_m == doctest::Approx(41000.0).epsilon(0.2));
    CHECK(otranto_ago.asse_m >= 50.0);
    CHECK(otranto_ago.asse_m <= 200.0);
    CHECK(clima("otranto").profilo(2).distanza_cz_m == 0.0);
    // acqua bassa o asse in superficie: nessuna CZ
    CHECK(clima("barents").profilo(8).distanza_cz_m == 0.0);
    CHECK(clima("kunashir").profilo(2).distanza_cz_m == 0.0);
    // inverno: strato rimescolato profondo; estate: termoclino dalla superficie
    CHECK(clima("faroe").profilo(2).sld_m > 100.0);
    CHECK(clima("faroe").profilo(8).gradiente_sotto < 0.0);
    CHECK(clima("faroe").profilo(14).sld_m == clima("faroe").profilo(2).sld_m);   // mese ripiegato
}

TEST_CASE("acustica: perdita di trasmissione per percorsi (sferica, condotto, layer, acqua bassa, CZ, fondo)") {
    Ambiente amb;   // isotermo, mare profondo
    const Perdita vicino = perdita_trasmissione(1000.0, 100.0, 50.0, 50.0, amb);
    CHECK(vicino.tl_db == doctest::Approx(37.0).epsilon(0.05));   // 20 log 100 - 3 dB dell'immagine incoerente
    CHECK(vicino.percorso == Percorso::diretto);
    // condotto di 30 m: intrappola sopra il taglio (circa 590 Hz), non sotto
    const ProfiloSuono c30 = condotto_30m();
    amb.profilo = &c30;
    CHECK(perdita_trasmissione(1000.0, 10000.0, 15.0, 20.0, amb).percorso == Percorso::condotto);
    CHECK(perdita_trasmissione(300.0, 10000.0, 15.0, 20.0, amb).percorso != Percorso::condotto);
    CHECK(perdita_trasmissione(1000.0, 10000.0, 15.0, 20.0, amb).tl_db < 20.0 * std::log10(10000.0));
    // attraversare il layer costa
    CHECK(perdita_trasmissione(1000.0, 5000.0, 15.0, 100.0, amb).tl_db >
          perdita_trasmissione(1000.0, 5000.0, 15.0, 20.0, amb).tl_db + 5.0);
    // acqua bassa: Marsh-Schulkin
    Ambiente basso;
    basso.fondale_m = 100.0;
    CHECK(perdita_trasmissione(1000.0, 10000.0, 30.0, 50.0, basso).percorso == Percorso::acqua_bassa);
    // CZ: alla distanza della zona si sente meglio che a tre quarti
    Ambiente med;
    med.profilo = &clima("otranto").profilo(8);
    med.fondale_m = 3000.0;
    const double rz = med.profilo->distanza_cz_m;
    const Perdita in_zona = perdita_trasmissione(300.0, rz, 100.0, 100.0, med);
    CHECK(in_zona.percorso == Percorso::zona_convergenza);
    CHECK(in_zona.zona == 1);
    CHECK(in_zona.tl_db < perdita_trasmissione(300.0, 0.75 * rz, 100.0, 100.0, med).tl_db - 5.0);
    // senza l'eccesso di profondita' la CZ non c'e'; nell'ombra arriva il rimbalzo sul fondo
    med.fondale_m = 2000.0;
    CHECK(perdita_trasmissione(300.0, rz, 100.0, 100.0, med).percorso != Percorso::zona_convergenza);
    CHECK(perdita_trasmissione(300.0, 10000.0, 100.0, 100.0, med).percorso == Percorso::fondo);
    // il fango riflette peggio della roccia
    Ambiente roccia = med, fango = med;
    roccia.fondo = TipoFondo::roccia;
    fango.fondo = TipoFondo::fango;
    CHECK(perdita_trasmissione(300.0, 10000.0, 100.0, 100.0, fango).tl_db >
          perdita_trasmissione(300.0, 10000.0, 100.0, 100.0, roccia).tl_db + 3.0);
}

TEST_CASE("acustica: rumore irradiato con velocita', cavitazione e snorkel; rumore proprio e direttivita'") {
    // 688: +15,5 dB da 4,5 a 15 kn in profondita' (tabella 01 §3.2); a 10 m a 15 kn cavita
    const Sorgente lento = sorgente("los_angeles", 4.5, 150.0, {}), veloce = sorgente("los_angeles", 15.0, 150.0, {});
    CHECK(livello_sorgente(150.0, veloce) - livello_sorgente(150.0, lento) == doctest::Approx(15.5).epsilon(0.01));
    const Sorgente basso = sorgente("los_angeles", 15.0, 10.0, {});
    CHECK(livello_sorgente(1000.0, basso) > livello_sorgente(1000.0, veloce) + 10.0);
    // spettro: -20 dB/decade sopra 200 Hz
    CHECK(livello_sorgente(200.0, lento) - livello_sorgente(2000.0, lento) == doctest::Approx(20.0));
    // Kilo a batteria contro snorkel
    Sorgente kilo = sorgente("kilo", 4.5, 15.0, {});
    const double batteria = livello_sorgente(150.0, kilo);
    kilo.snorkel = true;
    CHECK(livello_sorgente(150.0, kilo) - batteria == doctest::Approx(22.5));
    // indice di direttivita' (§1.2): sfera 20 log(pi D / lambda), schiera 10 log(2 L / lambda)
    CHECK(indice_direttivita(*catalogo().sensore("bqq5_sfera"), 1000.0) == doctest::Approx(19.7).epsilon(0.01));
    CHECK(indice_direttivita(*catalogo().sensore("tb16"), 1000.0) == doctest::Approx(19.9).epsilon(0.01));
    CHECK(indice_direttivita(*catalogo().sensore("bqq5_sfera"), 10.0) == 0.0);
    // rumore proprio: il flusso domina con la velocita', i baffles e la sordita' lo alzano
    const Sensore& sfera = *catalogo().sensore("bqq5_sfera");
    CHECK(rumore_proprio(1000.0, sfera, 20.0, false) > rumore_proprio(1000.0, sfera, 5.0, false) + 15.0);
    CHECK(rumore_proprio(1000.0, sfera, 5.0, true) - rumore_proprio(1000.0, sfera, 5.0, false) == doctest::Approx(15.0));
    CHECK(rumore_proprio(1000.0, sfera, 30.0, false) > rumore_proprio(1000.0, sfera, 24.0, false) + 30.0);
    // forza di bersaglio: il traverso piu' della prua; anecoici 10 dB sotto
    const FirmaRumore& akula = *catalogo().firma("akula");
    CHECK(forza_bersaglio(akula, 90.0) > forza_bersaglio(akula, 0.0) + 10.0);
    CHECK(forza_bersaglio(*catalogo().firma("los_angeles"), 90.0) - forza_bersaglio(akula, 90.0) == doctest::Approx(10.0));
}

TEST_CASE("acustica: equazione del sonar, portate relative, attivo e probabilita'") {
    Ambiente amb;
    amb.profilo = &clima("faroe").profilo(2);
    amb.fondale_m = 3000.0;
    amb.vento_m_s = 7.0;
    amb.traffico = clima("faroe").traffico;
    amb.ph = clima("faroe").ph;
    Ricevitore r;
    r.quota_m = 150.0;
    r.velocita_kn = 5.0;
    r.sensore = catalogo().sensore("tb16");
    // un battello rumoroso si sente piu' lontano di uno silenzioso, e chi corre piu' lontano di chi va piano
    const double akula = portata(sorgente("akula", 5.0, 150.0, {}), r, amb);
    const double victor = portata(sorgente("victor3", 5.0, 150.0, {}), r, amb);
    const double akula15 = portata(sorgente("akula", 15.0, 150.0, {}), r, amb);
    CHECK(akula > 1000.0);
    CHECK(victor > 2.0 * akula);
    CHECK(akula15 > 3.0 * akula);
    // la schiera trainata batte la sfera su un bersaglio silenzioso
    Ricevitore sfera = r;
    sfera.sensore = catalogo().sensore("bqq5_sfera");
    CHECK(portata(sorgente("akula", 5.0, 150.0, {}), sfera, amb) < akula);
    // il mare grosso accorcia la larga banda
    Ambiente mosso = amb;
    mosso.vento_m_s = 15.0;
    CHECK(portata(sorgente("victor3", 5.0, 150.0, {}), sfera, mosso) < portata(sorgente("victor3", 5.0, 150.0, {}), sfera, amb));
    // baffles: la stessa sorgente a poppa si sente peggio
    const Sorgente prua = sorgente("victor3", 5.0, 150.0, {0.0, 8000.0}), poppa = sorgente("victor3", 5.0, 150.0, {0.0, -8000.0});
    const Rilevazione rp = valuta(prua, sfera, amb), rpp = valuta(poppa, sfera, amb);
    CHECK_FALSE(rp.baffles);
    CHECK(rpp.baffles);
    CHECK(rpp.se_db < rp.se_db - 5.0);
    // attivo: l'eco al traverso e' piu' forte di quella di prua, e cala con la distanza
    Ricevitore att = r;
    att.sensore = catalogo().sensore("bqq5_attivo");
    const Rilevazione traverso = valuta(sorgente("akula", 5.0, 150.0, {0.0, 8000.0}, 90.0), att, amb);
    const Rilevazione di_prua = valuta(sorgente("akula", 5.0, 150.0, {0.0, 8000.0}, 180.0), att, amb);
    CHECK(traverso.ts_db > di_prua.ts_db + 10.0);
    CHECK(traverso.se_db > valuta(sorgente("akula", 5.0, 150.0, {0.0, 20000.0}, 90.0), att, amb).se_db);
    // probabilita' lambda-sigma
    CHECK(probabilita_rilevazione(0.0, 5.0) == doctest::Approx(0.5));
    CHECK(probabilita_rilevazione(5.0, 5.0) == doctest::Approx(0.8413).epsilon(0.001));
    CHECK(probabilita_rilevazione(-1.0, 0.0) == 0.0);
}

TEST_CASE("acustica: fluttuazione AR(1) stazionaria con la sigma data") {
    Casuale caso(7);
    Fluttuazione f;
    double somma = 0.0, quadrati = 0.0;
    constexpr int N = 40000;
    for (int i = 0; i < N; ++i) {
        f.avanza(10.0, 100.0, 4.0, caso);
        somma += f.db;
        quadrati += f.db * f.db;
    }
    const double media = somma / N;
    CHECK(std::fabs(media) < 0.5);
    CHECK(std::sqrt(quadrati / N - media * media) == doctest::Approx(4.0).epsilon(0.08));
}

TEST_CASE("acustica: catalogo di firme e sensori coerente con le classi") {
    const auto classi = carica_classi(M688_DATI "/classi.toml");
    // le firme delle piattaforme hanno le chiavi di classi.toml; le altre sono armi, esche, biologici e generiche
    const std::set<std::string> altre = {"biologico", "adc_mk2", "mk48", "mk48_adcap", "tigerfish", "spearfish", "uset80",
                                         "test71m", "type53_65k", "mk46", "at1", "siluro_generico",
                                         "sottomarino_generico", "nave_generica"};
    for (const auto& [c, f] : catalogo().firme) {
        CHECK(f.punti.size() >= 2);
        if (!altre.count(c)) CHECK_MESSAGE(classi.count(c) == 1, c);
    }
    for (const auto& [c, ids] : catalogo().suite) CHECK_MESSAGE(classi.count(c) == 1, c);
    // suite di bordo: il 688 ha sfera, schiera, attivo e intercettazione; un mercantile niente
    const auto suite = catalogo().sensori_di("los_angeles");
    REQUIRE(suite.size() == 4);
    CHECK(suite[1]->id == "tb16");
    CHECK(catalogo().sensori_di("mercantile").empty());
    // la cavitazione in superficie e' la stessa tabella della dinamica (C6a)
    CHECK(catalogo().firma("los_angeles")->v_cavitazione_kn == 5.0);
    CHECK(catalogo().firma("alfa")->v_cavitazione_kn == 4.0);
    for (const char* c : {"los_angeles", "akula", "victor3", "alfa", "typhoon", "kilo", "perry", "mercantile"})
        CHECK_MESSAGE(catalogo().firma(c) != nullptr, c);
    // i sensori di bordo lavorano sulle bande del modello; boe e sonar a immersione (A2) sulle loro frequenze vere
    for (const auto& [classe, ids] : catalogo().suite)
        for (const auto& id : ids)
            for (double b : catalogo().sensore(id)->bande_hz) {
                bool c_e = false;
                for (double x : BANDE_HZ) c_e |= x == b;
                CHECK_MESSAGE(c_e, id);
            }
    CHECK_THROWS(carica_acustica("/non/esiste.toml"));
    CHECK_THROWS(carica_clima("/non/esiste.toml"));
}

TEST_CASE("acustica: sorgenti dalle unita' (firme per classe e categoria) e scala dei siluri") {
    const auto& c = catalogo();
    const auto unita = [](const std::string& classe, const std::string& categoria, double kn = 5.0, bool falso = false) {
        UnitaAcustica u;
        u.classe = classe; u.categoria = categoria; u.velocita_kn = kn; u.quota_m = categoria == "nave" ? 0.0 : 150.0;
        u.falso_bersaglio = falso;
        return u;
    };
    CHECK(sorgente_acustica(c, unita("akula", "sottomarino")).firma == c.firma("akula"));
    CHECK(sorgente_acustica(c, unita("type53", "arma", 40.0)).firma == c.firma("uset80"));
    CHECK(sorgente_acustica(c, unita("los_angeles", "arma", 1.0, true)).firma == c.firma("adc_mk2"));   // esca lanciata
    CHECK(sorgente_acustica(c, unita("biologico", "nessuna", 0.0, true)).firma == c.firma("biologico"));
    CHECK(sorgente_acustica(c, unita("ignoto", "sottomarino")).firma == c.firma("sottomarino_generico"));
    CHECK(sorgente_acustica(c, unita("ignota", "nave")).firma == c.firma("nave_generica"));
    CHECK(sorgente_acustica(c, unita("ignota", "nave")).nave);
    const Sorgente aereo = sorgente_acustica(c, unita("p3c", "aereo"));
    CHECK(aereo.firma == nullptr);
    Ricevitore r;
    r.sensore = c.sensore("tb16");
    CHECK(valuta(aereo, r, Ambiente{}).se_db < -1e8);
    // scala della testa: il 688 silenzioso a 130 dB come lo stimatore della C6b per il rumore 10
    const SorgenteAcustica a = sorgente_per_siluri(sorgente_acustica(c, unita("los_angeles", "sottomarino", 4.5)));
    CHECK(a.livello_db == doctest::Approx(130.5));
    CHECK(a.velocita == doctest::Approx(4.5 * NODO));
    CHECK(sorgente_per_siluri(sorgente_acustica(c, unita("los_angeles", "arma", 1.0, true))).esca);
    // un siluro in corsa e' molto rumoroso (siluro in acqua)
    CHECK(livello_banda_larga(sorgente_acustica(c, unita("mk48", "arma", 55.0))) == doctest::Approx(160.0));
}

TEST_CASE("acustica: cavitazione dalla firma o dalla dinamica, posato sul fondo") {
    const FirmaRumore& f = *catalogo().firma("los_angeles");
    CHECK(velocita_cavitazione_kn(f, 0.0) == doctest::Approx(5.0));
    CHECK(cavita(f, 15.0, 10.0));
    CHECK_FALSE(cavita(f, 15.0, 150.0));
    Sorgente s = sorgente("los_angeles", 15.0, 150.0, {});
    const double senza = livello_sorgente(1000.0, s);
    s.cavitazione = true;   // la dinamica dice che cavita (eliche danneggiate, giri alti)
    CHECK(livello_sorgente(1000.0, s) >= senza + 10.0);
    // posato: 10 dB in meno, e l'eco attiva si confonde con il fondo
    Sorgente libero = sorgente("akula", 0.0, 150.0, {0.0, 6000.0}), posato = libero;
    posato.sul_fondo = true;
    CHECK(livello_banda_larga(libero) - livello_banda_larga(posato) == doctest::Approx(10.0));
    Ambiente amb;
    amb.fondale_m = 400.0;
    Ricevitore r;
    r.quota_m = 100.0;
    r.sensore = catalogo().sensore("bqq5_attivo");
    CHECK(valuta(posato, r, amb).se_db < valuta(libero, r, amb).se_db - 10.0);
}

TEST_CASE("acustica: schiera trainata cieca in virata e nell'endfire, errore del rilevamento") {
    Ambiente amb;
    amb.fondale_m = 3000.0;
    Ricevitore r;
    r.quota_m = 150.0;
    r.velocita_kn = 5.0;
    r.sensore = catalogo().sensore("tb16");
    const Sorgente traverso = sorgente("victor3", 5.0, 150.0, {6000.0, 0.0});
    const Rilevazione base = valuta(traverso, r, amb);
    CHECK(base.conico);
    CHECK_FALSE(base.cieco);
    Ricevitore in_virata = r;
    in_virata.rateo_virata_deg_s = 1.0;
    const Rilevazione v = valuta(traverso, in_virata, amb);
    CHECK(v.cieco);
    CHECK(v.se_db == doctest::Approx(base.se_db - 30.0));
    Ricevitore raddrizza = r;
    raddrizza.tempo_da_virata_s = 60.0;   // a meta' dei 2 minuti: 5 dB in meno
    CHECK(valuta(traverso, raddrizza, amb).se_db == doctest::Approx(base.se_db - 5.0));
    CHECK(valuta(sorgente("victor3", 5.0, 150.0, {0.0, 6000.0}), r, amb).cieco);   // davanti: endfire
    // errore del rilevamento: cala con l'eccesso, cresce verso l'endfire, mai sotto 0,2 gradi
    Rilevazione x = base;
    x.se_db = 0.0;
    const double a0 = errore_rilevamento_deg(x, *r.sensore);
    x.se_db = 20.0;
    const double a20 = errore_rilevamento_deg(x, *r.sensore);
    CHECK(a20 < a0);
    CHECK(a20 >= 0.2);
    x.se_db = 0.0;
    x.rilevamento_relativo = 150.0;
    CHECK(errore_rilevamento_deg(x, *r.sensore) > a0);
    // la sfera ha il fascio piu' largo della schiera alla stessa frequenza
    Rilevazione y = base;
    y.se_db = 0.0;
    y.rilevamento_relativo = 90.0;
    CHECK(errore_rilevamento_deg(y, *catalogo().sensore("bqq5_sfera")) > errore_rilevamento_deg(y, *r.sensore));
}

TEST_CASE("acustica: intercettazione di un impulso attivo (contro-scoperta)") {
    Ambiente amb;
    amb.profilo = &clima("faroe").profilo(2);
    amb.fondale_m = 3000.0;
    // chi fa il ping: un 688 con il BQQ-5 attivo; chi ascolta: un Akula con l'intercettazione
    Sorgente pinger = sorgente("los_angeles", 5.0, 150.0, {});
    Ricevitore akula;
    akula.quota_m = 150.0;
    akula.velocita_kn = 5.0;
    akula.sensore = catalogo().sensore("intercettazione_sov");
    const auto portata_intercettazione = [&](double ping_db) {
        Sorgente s = pinger;
        s.ping_db = ping_db;
        double out = 0.0;
        for (double d = 500.0; d < 200000.0; d *= 1.05) {
            akula.pos = {0.0, d};
            if (valuta(s, akula, amb).se_db >= 0.0) out = d;
        }
        return out;
    };
    const double silenzio = portata_intercettazione(0.0), ping = portata_intercettazione(225.0);
    CHECK(ping > 10.0 * std::max(silenzio, 500.0));
    // contro-scoperta: l'impulso si sente piu' lontano di quanto il pinger veda l'eco (02 §5.3)
    Ricevitore att;
    att.quota_m = 150.0;
    att.velocita_kn = 5.0;
    att.sensore = catalogo().sensore("bqq5_attivo");
    double eco = 0.0;
    for (double d = 500.0; d < 200000.0; d *= 1.05)
        if (valuta(sorgente("akula", 5.0, 150.0, {0.0, d}), att, amb).se_db >= 0.0) eco = d;
    CHECK(ping > 1.5 * eco);
    Sorgente s = pinger;
    s.ping_db = 225.0;
    akula.pos = {0.0, 20000.0};
    CHECK(valuta(s, akula, amb).ping);
}

TEST_CASE("acustica: file e mese del teatro") {
    CHECK(file_clima("/dati", "faroe") == "/dati/teatri/faroe.suono.toml");
    CHECK(clima("faroe").mese_predefinito == 2);
    CHECK(clima("otranto").mese_predefinito == 8);
}

TEST_CASE("acustica: effetto pomeriggio (intensita', microstrato, portate verso la superficie)") {
    CHECK(intensita_pomeriggio(6.0, 3.0, 0.1) == 0.0);
    CHECK(intensita_pomeriggio(12.0, 3.0, 0.1) == doctest::Approx(0.5));
    CHECK(intensita_pomeriggio(15.0, 3.0, 0.1) == doctest::Approx(1.0));
    CHECK(intensita_pomeriggio(20.0, 3.0, 0.1) == doctest::Approx(0.5));
    CHECK(intensita_pomeriggio(15.0, 8.0, 0.1) == 0.0);   // 15,5 kn: il vento rimescola
    CHECK(intensita_pomeriggio(15.0, 3.0, 0.6) == 0.0);   // cielo coperto
    CHECK(intensita_pomeriggio(-1.0, 3.0, 0.1) == 0.0);   // ora ignota
    // Mediterraneo d'inverno: condotto profondo; il pomeriggio mette un microtermoclino nei primi 10 m
    const ProfiloSuono& inverno = clima("otranto").profilo(2);
    const ProfiloSuono sole = applica_pomeriggio(inverno, 1.0);
    CHECK(inverno.microstrato_m == 0.0);
    CHECK(sole.microstrato_m == doctest::Approx(10.0));
    CHECK(sole.microstrato_dc > 3.0);
    CHECK(applica_pomeriggio(inverno, 0.0).microstrato_m == 0.0);
    Ambiente mattina, pomeriggio;
    mattina.profilo = &inverno;
    pomeriggio.profilo = &sole;
    mattina.fondale_m = pomeriggio.fondale_m = 3000.0;
    mattina.vento_m_s = pomeriggio.vento_m_s = 3.0;
    CHECK(perdita_trasmissione(1000.0, 10000.0, 10.0, 15.0, pomeriggio).tl_db >
          perdita_trasmissione(1000.0, 10000.0, 10.0, 15.0, mattina).tl_db + 5.0);
    // in profondita' quasi nulla (cambia solo un po' il gradiente medio dello strato)
    CHECK(perdita_trasmissione(1000.0, 10000.0, 150.0, 200.0, pomeriggio).tl_db ==
          doctest::Approx(perdita_trasmissione(1000.0, 10000.0, 150.0, 200.0, mattina).tl_db).epsilon(0.03));
    // anche i raggi lo vedono
    const CampoPerdita cm = calcola_campo(10.0, mattina), cp = calcola_campo(10.0, pomeriggio);
    CHECK(*cp.tl(1000.0, 10000.0, 15.0) > *cm.tl(1000.0, 10000.0, 15.0) + 1.0);
}

TEST_CASE("acustica: LOFAR sulle tonali (schiera trainata contro battelli silenziosi)") {
    Ambiente amb;
    amb.profilo = &clima("faroe").profilo(2);
    amb.fondale_m = 3000.0;
    amb.vento_m_s = 7.0;
    amb.traffico = clima("faroe").traffico;
    Ricevitore r;
    r.quota_m = 150.0;
    r.velocita_kn = 5.0;
    r.sensore = catalogo().sensore("tb16");
    Sensore senza = *catalogo().sensore("tb16");
    senza.lofar_bin_hz = 0.0;
    Ricevitore r_senza = r;
    r_senza.sensore = &senza;
    const Sorgente la = sorgente("los_angeles", 5.0, 150.0, {});
    const double con_lofar = portata(la, r, amb), larga_banda = portata(la, r_senza, amb);
    CHECK(con_lofar > 1.3 * larga_banda);
    const Rilevazione x = valuta(sorgente("los_angeles", 5.0, 150.0, {4000.0, 0.0}), r, amb);
    CHECK(x.lofar);
    CHECK(x.banda_hz == 300.0);   // la quinta armonica dei 60 Hz
    CHECK(x.se_tonale_db == x.se_db);
    CHECK(valuta(sorgente("akula", 5.0, 150.0, {4000.0, 0.0}), r, amb).banda_hz == 250.0);   // 50 Hz sovietici
    // un sensore senza LOFAR non da' l'eccesso sulla tonale (l'SQS-53 ha il LOFAR grossolano dal 29/10/2026: qui senza)
    Sensore sqs53_senza = *catalogo().sensore("sqs53");
    sqs53_senza.lofar_bin_hz = 0.0;
    Ricevitore nave = r;
    nave.sensore = &sqs53_senza;
    CHECK(valuta(la, nave, amb).se_tonale_db < -1e8);
    // posato: i macchinari ridotti abbassano anche la tonale
    Sorgente posato = sorgente("los_angeles", 0.0, 150.0, {4000.0, 0.0});
    const double prima = valuta(posato, r, amb).se_tonale_db;
    posato.sul_fondo = true;
    CHECK(valuta(posato, r, amb).se_tonale_db == doctest::Approx(prima - 10.0));
}

TEST_CASE("acustica: tracciamento dei raggi (sferica vicino, condotto, ombra, CZ, acqua bassa)") {
    Ambiente iso;
    iso.fondale_m = 4000.0;
    const CampoPerdita ci = calcola_campo(100.0, iso);
    CHECK(*ci.tl(1000.0, 1000.0, 100.0) == doctest::Approx(20.0 * std::log10(1000.0)).epsilon(0.06));
    CHECK_FALSE(ci.tl(1000.0, 150000.0, 100.0));   // fuori dalla griglia
    // condotto invernale delle Faroe: a 20 km molto meglio della sferica
    Ambiente faroe;
    faroe.profilo = &clima("faroe").profilo(2);
    faroe.fondale_m = 3000.0;
    faroe.vento_m_s = 7.0;
    CHECK(*calcola_campo(30.0, faroe).tl(1000.0, 20000.0, 30.0) < 20.0 * std::log10(20000.0) - 5.0);
    // Mediterraneo d'estate, sorgente e ricevitore a 20 m: ombra a 5 km, zona di convergenza a 40 km
    Ambiente med;
    med.profilo = &clima("otranto").profilo(8);
    med.fondale_m = 3000.0;
    med.vento_m_s = 7.0;
    const CampoPerdita cm = calcola_campo(20.0, med);
    CHECK(*cm.tl(1000.0, 5000.0, 20.0) > 20.0 * std::log10(5000.0) + 10.0);
    // il campo oscilla per le caustiche: medie in energia su finestre di distanza
    const double r_cz = med.profilo->distanza_cz_m;
    const auto media = [&](double a, double b) {
        double e = 0.0;
        int n = 0;
        for (double r = a; r <= b; r += 250.0, ++n) e += std::pow(10.0, -*cm.tl(1000.0, r, 20.0) / 10.0);
        return -10.0 * std::log10(e / n);
    };
    // fra l'ombra vicina e la zona arriva anche il rimbalzo sul fondo: si confronta con l'ombra e con il dopo-zona
    const double in_zona = media(0.85 * r_cz, 1.1 * r_cz);
    CHECK(in_zona < media(0.15 * r_cz, 0.25 * r_cz) - 5.0);
    CHECK(in_zona < media(1.15 * r_cz, 1.3 * r_cz) - 5.0);
    // l'assorbimento cresce con la frequenza (Mediterraneo d'estate: circa 0,7 dB/km a 10 kHz)
    CHECK(*cm.tl(10000.0, r_cz, 20.0) > *cm.tl(1000.0, r_cz, 20.0) + 20.0);
    // acqua bassa: la griglia finisce al fondo e le perdite sul fondo si accumulano
    Ambiente basso = faroe;
    basso.fondale_m = 100.0;
    const CampoPerdita cb = calcola_campo(30.0, basso);
    CHECK(cb.nz == 11);
    // sulla sabbia i raggi radenti, sotto l'angolo critico, perdono poco (27/10: prima +15 dB, 5 dB a rimbalzo); sul
    // fango a ogni angolo
    CHECK(*cb.tl(1000.0, 60000.0, 50.0) > *cb.tl(1000.0, 20000.0, 50.0) + 8.0);
    basso.fondo = TipoFondo::fango;
    const CampoPerdita cf = calcola_campo(30.0, basso);
    CHECK(*cf.tl(1000.0, 60000.0, 50.0) > *cf.tl(1000.0, 20000.0, 50.0) + 10.0);
    // il contrasto fra sabbia radente e fango e' delle basse frequenze (a 1 kHz la sabbia perde 1,8 dB anche radente).
    // Le soglie sono sulla tesi e non sui numeri di un profilo: col profilo 1955-2022 delle Faroe il contrasto a 150 Hz
    // era 10,7 dB, col 1985-1994 (29/10/2026) 3,9, con meno rimbalzi sul fondo nei 100 m d'acqua
    CHECK(*cf.tl(150.0, 60000.0, 50.0) > *cb.tl(150.0, 60000.0, 50.0) + 3.0);
    CHECK(*cf.tl(1000.0, 60000.0, 50.0) > *cb.tl(1000.0, 60000.0, 50.0));
    CHECK(*cf.tl(150.0, 60000.0, 50.0) - *cb.tl(150.0, 60000.0, 50.0) > *cf.tl(1000.0, 60000.0, 50.0) - *cb.tl(1000.0, 60000.0, 50.0));
}

TEST_CASE("acustica: memoria dei campi e valuta con i raggi") {
    CacheCampi cache;
    cache.limite_per_passo = 100;
    Ambiente amb;
    amb.profilo = &clima("faroe").profilo(2);
    amb.fondale_m = 3000.0;
    amb.vento_m_s = 7.0;
    Sorgente s = sorgente("victor3", 5.0, 150.0, {});
    s.id = 7;
    Ricevitore r;
    r.pos = {8000.0, 0.0};
    r.quota_m = 150.0;
    r.velocita_kn = 5.0;
    r.sensore = catalogo().sensore("tb16");
    const Rilevazione x = valuta(s, r, amb, cache);
    CHECK(x.percorso == Percorso::raggi);
    CHECK(cache.calcoli() == 1);
    s.pos = {1000.0, 0.0};   // 1 km: lo stesso campo
    valuta(s, r, amb, cache);
    CHECK(cache.calcoli() == 1);
    s.pos = {10000.0, 0.0};   // oltre 3 nm: si rifa'
    valuta(s, r, amb, cache);
    CHECK(cache.calcoli() == 2);
    s.quota_m = 300.0;   // cambio di quota
    valuta(s, r, amb, cache);
    CHECK(cache.calcoli() == 3);
    const ProfiloSuono estate = clima("faroe").profilo(8);
    amb.profilo = &estate;   // cambio d'ambiente
    valuta(s, r, amb, cache);
    CHECK(cache.calcoli() == 4);
    // fuori dalla griglia (oltre 100 km) torna il livello A
    r.pos = {200000.0, 0.0};
    CHECK(valuta(s, r, amb, cache).percorso != Percorso::raggi);
    // l'attivo usa lo stesso campo per andata e ritorno
    Ricevitore att = r;
    att.pos = {14000.0, 0.0};
    att.sensore = catalogo().sensore("bqq5_attivo");
    const Rilevazione a = valuta(s, att, amb, cache);
    CHECK(a.percorso == Percorso::raggi);
    CHECK(a.ts_db != 0.0);
}

TEST_CASE("acustica: limite di ricalcoli per passo nella memoria dei campi") {
    CacheCampi cache;   // limite predefinito: un campo per passo
    Ambiente amb;
    amb.fondale_m = 3000.0;
    Sorgente a = sorgente("victor3", 5.0, 150.0, {}), b = sorgente("akula", 5.0, 150.0, {0.0, 5000.0});
    a.id = 1;
    b.id = 2;
    Ricevitore r;
    r.pos = {8000.0, 0.0};
    r.quota_m = 150.0;
    r.sensore = catalogo().sensore("tb16");
    CHECK(valuta(a, r, amb, cache).percorso == Percorso::raggi);
    CHECK(valuta(b, r, amb, cache).percorso != Percorso::raggi);   // oltre il limite: livello A per ora
    CHECK(cache.calcoli() == 1);
    cache.nuovo_passo();
    CHECK(valuta(b, r, amb, cache).percorso == Percorso::raggi);
    CHECK(cache.calcoli() == 2);
    // una sorgente che si sposta oltre la soglia tiene il campo vecchio finche' il passo non ha posto
    a.pos = {-10000.0, 0.0};
    CHECK(valuta(a, r, amb, cache).percorso == Percorso::raggi);
    CHECK(cache.calcoli() == 2);
    cache.nuovo_passo();
    valuta(a, r, amb, cache);
    CHECK(cache.calcoli() == 3);
}

TEST_CASE("acustica: memoria dei raggi asincrona (istante fisso, ripetibile, senza attese alla richiesta)") {
    Ambiente amb;
    amb.fondale_m = 3000.0;
    amb.vento_m_s = 7.0;
    // il profilo vive solo qui: il thread deve averne una copia
    auto profilo = std::make_unique<ProfiloSuono>(clima("faroe").profilo(2));
    amb.profilo = profilo.get();
    const CampoPerdita atteso = calcola_campo(150.0, amb);
    CacheCampiAsincrona cache;
    const auto t0 = std::chrono::steady_clock::now();
    CHECK(cache.campo(5, {0.0, 0.0}, 150.0, amb, 0.0) == nullptr);   // chiesto ora, in vigore a 10 s
    const double ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count();
    CHECK(ms < 20.0);   // la richiesta non aspetta il calcolo (circa 60 ms)
    profilo.reset();    // l'ambiente originale sparisce prima che il calcolo finisca
    Ambiente dopo = amb;
    dopo.profilo = nullptr;
    // fino a 10 s nessun campo (livello A), poi quello calcolato: identico al sincrono
    const ProfiloSuono stesso = clima("faroe").profilo(2);
    dopo.profilo = &stesso;
    CHECK(cache.campo(5, {0.0, 0.0}, 150.0, dopo, 9.9) == nullptr);
    const CampoPerdita* c = cache.campo(5, {0.0, 0.0}, 150.0, dopo, 10.0);
    REQUIRE(c != nullptr);
    CHECK(c->tl_db == atteso.tl_db);
    // chi disegna legge il campo in vigore senza chiederne uno: per la partita e' come se non ci fosse
    const std::size_t chieste = cache.richieste();
    CHECK(cache.in_vigore(5) == c);
    CHECK(cache.in_vigore(6) == nullptr);
    CHECK(cache.richieste() == chieste);
    CHECK(cache.richieste() == 2);   // il profilo e' un altro oggetto: una richiesta nuova, in vigore a 20 s
    CHECK(cache.campo(5, {0.0, 0.0}, 150.0, dopo, 15.0) == c);
    // la sorgente si sposta oltre 3 nm: il campo vecchio resta in vigore fino a 10 s dopo la richiesta
    cache.campo(5, {0.0, 0.0}, 150.0, dopo, 20.0);   // entra quello chiesto a 10 s
    const std::size_t prima = cache.richieste();
    const CampoPerdita* vecchio = cache.campo(5, {12000.0, 0.0}, 150.0, dopo, 30.0);
    CHECK(cache.richieste() == prima + 1);
    CHECK(cache.campo(5, {12000.0, 0.0}, 150.0, dopo, 39.0) == vecchio);
    CHECK(cache.campo(5, {12000.0, 0.0}, 150.0, dopo, 40.0) != vecchio);
    // valuta con il tempo di gioco: senza campo il livello A, poi i raggi
    CacheCampiAsincrona altra;
    Sorgente s = sorgente("victor3", 5.0, 150.0, {});
    s.id = 9;
    Ricevitore r;
    r.pos = {8000.0, 0.0};
    r.quota_m = 150.0;
    r.sensore = catalogo().sensore("tb16");
    CHECK(valuta(s, r, dopo, altra, 0.0).percorso != Percorso::raggi);
    CHECK(valuta(s, r, dopo, altra, 10.0).percorso == Percorso::raggi);
}

TEST_CASE("acustica: i campi condivisi fra le navi del traffico (una chiave, un calcolo; la differenza acustica)") {
    Ambiente amb;
    amb.fondale_m = 2800.0;
    amb.vento_m_s = 7.0;
    const ProfiloSuono p = clima("faroe").profilo(2);
    amb.profilo = &p;
    Ambiente amb2 = amb;
    amb2.fondale_m = 2900.0;   // la stessa fascia del 10%
    CacheCampiAsincrona cache;
    cache.condividi(20);
    cache.condividi(21);
    // due navi nella stessa chiave (quota 5 e 7 m, fondale 2800 e 2900): un calcolo solo, coi dati della prima
    CHECK(cache.campo(20, {0.0, 0.0}, 5.0, amb, 0.0) == nullptr);
    CHECK(cache.campo(21, {30000.0, 0.0}, 7.0, amb2, 0.0) == nullptr);
    CHECK(cache.richieste() == 1);
    CHECK(cache.chiavi_condivise() == 1);
    const CampoPerdita* a = cache.campo(20, {0.0, 0.0}, 5.0, amb, 10.0);
    REQUIRE(a != nullptr);
    CHECK(cache.campo(21, {30000.0, 0.0}, 7.0, amb2, 10.0) == a);
    CHECK(cache.in_vigore(21) == a);
    CHECK(cache.in_vigore(23) == nullptr);
    CHECK(a->tl_db == calcola_campo(5.0, amb).tl_db);
    // una nave non segnata resta per conto suo, come prima
    CHECK(cache.campo(22, {0.0, 0.0}, 5.0, amb, 10.0) == nullptr);
    CHECK(cache.richieste() == 2);
    // la chiave che nessuno usa da 10 minuti si butta; chi torna tiene il campo di prima finche' arriva il nuovo
    // (la pulizia la fa la prima richiesta condivisa dopo il minuto: la chiave vecchia esce e se ne apre una nuova)
    CHECK(cache.campo(20, {0.0, 0.0}, 5.0, amb, 710.0) == a);
    CHECK(cache.richieste() == 3);
    CHECK(cache.chiavi_condivise() == 1);
    CHECK(cache.campo(20, {0.0, 0.0}, 5.0, amb, 720.0) != nullptr);
    cache.dimentica(20);
    CHECK(cache.campo(20, {0.0, 0.0}, 5.0, amb, 730.0) == nullptr);   // non piu' condivisa: una voce sua, da capo
    // la differenza acustica: per coppie nella stessa chiave (quota nella stessa fascia di 10 m, fondale nella stessa
    // del 10%), in quattro teatri e due mesi, la perdita a 1 kHz fra 2 e 60 km e fra 20 e 300 m col campo proprio e con
    // quello condiviso
    std::vector<double> d;
    for (const char* teatro : {"barents", "faroe", "gibilterra", "otranto"})
        for (int mese : {2, 8}) {
            const ProfiloSuono& q = clima(teatro).profilo(mese);
            for (const auto& [z1, z2, f1, f2] : {std::array<double, 4>{5.0, 7.0, 280.0, 290.0}, std::array<double, 4>{5.0, 9.0, 1500.0, 1600.0},
                                                 std::array<double, 4>{2.0, 4.0, 3000.0, 3250.0}}) {
                Ambiente e1;
                e1.profilo = &q;
                e1.vento_m_s = 7.0;
                e1.fondale_m = f1;
                Ambiente e2 = e1;
                e2.fondale_m = f2;
                const CampoPerdita c1 = calcola_campo(z1, e1), c2 = calcola_campo(z2, e2);
                for (double r = 2000.0; r <= 60000.0; r += 2000.0)
                    for (double z = 20.0; z <= std::min(300.0, f1 - 10.0); z += 40.0) {
                        const auto t1 = c1.tl(1000.0, r, z), t2 = c2.tl(1000.0, r, z);
                        if (t1 && t2 && *t1 < 200.0 && *t2 < 200.0) d.push_back(std::fabs(*t1 - *t2));
                    }
            }
        }
    REQUIRE(d.size() > 500);
    std::sort(d.begin(), d.end());
    const double mediana = d[d.size() / 2], p90 = d[d.size() * 9 / 10];
    MESSAGE("campi condivisi: |dTL| mediana " << mediana << " dB, 90% " << p90 << " dB su " << d.size() << " punti");
    CHECK(mediana < 1.5);
    CHECK(p90 < 6.0);
}

TEST_CASE("acustica: l'impulso del BQS-15 all'intercettazione (WLR-9, Enisej): ~10 km nel fascio, ~6 fuori") {
    // la spazzata del BQS-15 (missioni/src/missione_bqs15.cpp): 205 dB a 25 kHz per 10 ms, -20 dB coi lobi laterali.
    // L'impulso va a un rivelatore d'energia sulla sua durata, non col guadagno del rumore a banda larga, e i ricevitori
    // d'intercettazione non hanno guadagno d'antenna. Il dossier 06 §2 da' ~8 km nel fascio e 2-4 fuori con 6 dB/km e
    // lobi a -20/-25; nell'acqua fredda delle Faroe l'assorbimento a 25 kHz e' ~4,5 dB/km: un po' di piu'
    Ambiente amb;
    amb.profilo = &clima("faroe").profilo(2);
    amb.fondale_m = 3000.0;
    amb.vento_m_s = vento_per_stato_del_mare(3);
    for (const char* ricevitore : {"wlr9", "intercettazione_sov"}) {
        const Ricevitore r{{0.0, 0.0}, 150.0, 0.0, 5.0, catalogo().sensore(ricevitore)};
        REQUIRE(r.sensore);
        Sorgente s = sorgente("los_angeles", 5.0, 150.0, {});
        s.ping_hz = 25000.0;
        s.ping_s = 0.01;
        s.ping_db = 205.0;
        const double nel = portata(s, r, amb);
        s.ping_db = 185.0;
        const double fuori = portata(s, r, amb);
        MESSAGE(std::string(ricevitore) << ": nel fascio " << nel << " m, fuori " << fuori << " m");
        CHECK(nel > 7000.0);
        CHECK(nel < 12000.0);
        CHECK(fuori > 3000.0);
        CHECK(fuori < nel * 0.75);
        // senza l'impulso il 688 a 5 kn a 25 kHz non si sente nemmeno vicino
        s.ping_db = 0.0;
        CHECK(portata(s, r, amb) < 1500.0);
    }
}

TEST_CASE("acustica: una sorgente muta (il relitto della I10) non si sente al passivo, ma da' l'eco all'attivo") {
    Ambiente amb;
    amb.profilo = &clima("faroe").profilo(2);
    amb.fondale_m = 3000.0;
    amb.vento_m_s = vento_per_stato_del_mare(3);
    const Ricevitore passivo{{0.0, 0.0}, 150.0, 0.0, 5.0, catalogo().sensore("bqq5_sfera")};
    const Ricevitore attivo{{0.0, 0.0}, 150.0, 0.0, 5.0, catalogo().sensore("bqq5_attivo")};
    REQUIRE(passivo.sensore);
    REQUIRE(attivo.sensore);
    Sorgente s = sorgente("charlie2", 8.0, 150.0, {3000.0, 0.0});
    const double eco = valuta(s, attivo, amb).se_db;
    CHECK(valuta(s, passivo, amb).se_db > 0.0);   // un battello vero a 3 km si sente
    s.muta = true;
    CHECK(valuta(s, passivo, amb).se_db < -100.0);
    CHECK(valuta(s, attivo, amb).se_db == doctest::Approx(eco));   // l'eco resta uguale
    CHECK(livello_banda_larga(s) < -100.0);
}

TEST_CASE("acustica: il mare grosso sui sensori vicino alla superficie, uguale per tutti (24 §7.4)") {
    Ambiente mare3, mare6;
    mare3.vento_m_s = vento_per_stato_del_mare(3);
    mare6.vento_m_s = vento_per_stato_del_mare(6);
    const Sensore& scafo = *catalogo().sensore("sqs53");
    const Sensore& sfera = *catalogo().sensore("bqq5_sfera");
    const Sensore& schiera = *catalogo().sensore("tb16");
    CHECK(rumore_mare_proprio_db(scafo, 6.0, mare3) == 0.0);
    CHECK(rumore_mare_proprio_db(scafo, 6.0, mare6) == doctest::Approx(10.0 * (1.0 - 6.0 / 100.0)));
    CHECK(rumore_mare_proprio_db(sfera, 150.0, mare6) == 0.0);            // in profondita' niente
    CHECK(rumore_mare_proprio_db(sfera, 18.0, mare6) > 7.0);              // a quota periscopica si'
    CHECK(rumore_mare_proprio_db(schiera, 60.0, mare6) == doctest::Approx(6.0 * (1.0 - 60.0 / 150.0)));
    Ambiente mare5 = mare3;
    mare5.vento_m_s = vento_per_stato_del_mare(5);
    CHECK(rumore_mare_proprio_db(scafo, 6.0, mare5) == doctest::Approx(0.6 * 10.0 * (1.0 - 6.0 / 100.0)));
    // boe e sonar a immersione no: il mare li tocca gia' con mare_max e col dip vietato (24 §7.4, campo mare_proprio_db)
    for (const char* id : {"rgb1", "rgb_nm", "ssq53b", "aqs13f_passivo", "vgs3_passivo"}) {
        const Sensore* b = catalogo().sensore(id);
        REQUIRE(b);
        CHECK_MESSAGE(rumore_mare_proprio_db(*b, 20.0, mare6) == 0.0, id);
    }
    // la nave col mare 6 perde piu' portata del sottomarino profondo (a parita' di rumore ambiente)
    mare3.profilo = mare6.profilo = &clima("faroe").profilo(2);
    mare3.fondale_m = mare6.fondale_m = 3000.0;
    mare6.vento_m_s = mare3.vento_m_s;   // stesso rumore del vento: conta solo il rumore proprio
    mare6.hs_m = 6.0;                    // il mare dalla Hs: forza 6
    const Sorgente s = sorgente("victor3", 8.0, 150.0, {});
    const double nave3 = portata(s, Ricevitore{{0, 0}, 6.0, 0.0, 10.0, &scafo}, mare3);
    const double nave6 = portata(s, Ricevitore{{0, 0}, 6.0, 0.0, 10.0, &scafo}, mare6);
    const double sub3 = portata(s, Ricevitore{{0, 0}, 150.0, 0.0, 5.0, &sfera}, mare3);
    const double sub6 = portata(s, Ricevitore{{0, 0}, 150.0, 0.0, 5.0, &sfera}, mare6);
    CHECK(nave6 < nave3);
    CHECK(nave6 / nave3 < sub6 / sub3 - 0.05);   // l'onda tocca anche la propagazione: il sottomarino perde meno
}

TEST_CASE("acustica: il vantaggio di scoperta fra le classi del 1989 esce dal motore (24 §5.1, entro 4 dB)") {
    // A sente B meno B sente A, alla stessa distanza e quota (150 m), a 5 nodi, Faroe d'inverno e mare 3, ciascuno col
    // migliore dei suoi sensori passivi: il vantaggio del dossier 24 §5.1 (rumore piu' sensore) non e' una regola a
    // parte, deve venire dalle firme (dossier 01) e dai sensori
    Ambiente amb;
    amb.profilo = &clima("faroe").profilo(2);
    amb.fondale_m = 3000.0;
    amb.vento_m_s = vento_per_stato_del_mare(3);
    const auto se_migliore = [&](const std::string& chi, const std::string& chi_ascolta, double distanza_m) {
        double se = -1e9;
        for (const auto& id : catalogo().suite.at(chi_ascolta)) {
            const Sensore* sn = catalogo().sensore(id);
            if (!sn || sn->modo != ModoSensore::passivo || id == "wlr9" || id == "intercettazione_sov") continue;
            const Ricevitore r{{0.0, 0.0}, 150.0, 0.0, 5.0, sn};
            se = std::max(se, valuta(sorgente(chi, 5.0, 150.0, {distanza_m, 0.0}), r, amb).se_db);
        }
        return se;
    };
    struct Coppia { const char* a; const char* b; double vantaggio; };
    for (const Coppia& c : {Coppia{"los_angeles", "victor3", 15.0}, Coppia{"los_angeles", "akula", 3.0},
                            Coppia{"los_angeles", "alfa", 23.0}, Coppia{"los_angeles", "kilo", 8.0},
                            Coppia{"los_angeles", "typhoon", 17.5}, Coppia{"sturgeon", "victor3", 10.0},
                            Coppia{"trafalgar", "akula", 5.0}}) {
        const double v = se_migliore(c.b, c.a, 10000.0) - se_migliore(c.a, c.b, 10000.0);
        MESSAGE(std::string(c.a) << " -> " << std::string(c.b) << ": " << v << " dB (dossier " << c.vantaggio << ")");
        const std::string coppia = std::string(c.a) + " -> " + c.b;
        CHECK_MESSAGE(std::fabs(v - c.vantaggio) <= 4.0, coppia);
    }
}

TEST_CASE("acustica: il fondo del teatro per profondita' e i gamberi nelle acque basse e calde") {
    const ClimaTeatro& f = clima("faroe");
    CHECK(f.fondo(100.0) == TipoFondo::roccia);     // piattaforma delle Faer Oer
    CHECK(f.fondo(800.0) == TipoFondo::sabbia);     // scarpata
    CHECK(f.fondo(3000.0) == TipoFondo::fango);     // piana
    CHECK(clima("baltico").fondo(50.0) == TipoFondo::fango);
    CHECK(clima("celtica").fondo(120.0) == TipoFondo::sabbia);
    // il fondo cambia il rimbalzo: con la roccia si perde meno che col fango
    Ambiente a;
    a.fondale_m = 400.0;
    a.fondo = TipoFondo::roccia;
    const double roccia = perdita_trasmissione(1000.0, 2000.0, 50.0, 50.0, a).tl_db;
    a.fondo = TipoFondo::fango;
    CHECK(perdita_trasmissione(1000.0, 2000.0, 50.0, 50.0, a).tl_db > roccia);
    // gamberi: Otranto d'agosto sotto costa si', d'inverno no; mai in acqua profonda o fredda
    const ClimaTeatro& o = clima("otranto");
    CHECK(gamberi_attivi(40.0, o.profilo(8).temperatura_superficie));
    CHECK_FALSE(gamberi_attivi(40.0, o.profilo(2).temperatura_superficie));
    CHECK_FALSE(gamberi_attivi(200.0, o.profilo(8).temperatura_superficie));
    CHECK_FALSE(gamberi_attivi(30.0, clima("baltico").profilo(8).temperatura_superficie));
}

TEST_CASE("acustica: il campo di raggi in acqua bassa d'estate converge, e le acque profonde non cambiano") {
    const ClimaTeatro barents = carica_clima(M688_DATI "/teatri/barents.suono.toml");
    Ambiente a;
    // ottobre: 20 m rimescolati sopra un termoclino forte (col profilo 1955-2022 era settembre, 10 m; col 1985-1994 del
    // 29/10/2026 settembre non ha piu' lo strato)
    a.profilo = &barents.profilo(10);
    a.fondale_m = 131.0;
    a.fondo = TipoFondo::sabbia;
    const CampoPerdita c = calcola_campo(0.5, a);
    ParametriRaggi fitto;               // il riferimento: 1601 raggi su 30 gradi
    fitto.raggi = 1601;
    fitto.raggi_acqua_bassa = 1601;
    fitto.angolo_acqua_bassa_gradi = 30.0;
    const CampoPerdita rif = calcola_campo(0.5, a, fitto);
    // il condotto di 20 m ha il taglio sopra 1 kHz: a 150 Hz i suoi raggi non contano
    CHECK(c.taglio_condotto_hz > 1000.0);
    double prima = 0.0;
    for (const double r : {10e3, 20e3, 30e3, 40e3, 61e3}) {
        const double tl = c.tl(150.0, r, 90.0).value(), t_rif = rif.tl(150.0, r, 90.0).value();
        CHECK(std::fabs(tl - t_rif) < 6.0);
        CHECK(tl > prima - 3.0);   // cresce con la distanza (prima della correzione: 97 a 30 km, 92 a 61)
        prima = tl;
    }
    // vicino alla sorgente i cammini ripidi restano: un SSN a 300 m e una boa a 18 m, a 400 m, in 450 m d'acqua
    const ClimaTeatro faroe = carica_clima(M688_DATI "/teatri/faroe.suono.toml");
    Ambiente scarpata;
    scarpata.profilo = &faroe.profilo(10);
    scarpata.fondale_m = 450.0;
    scarpata.fondo = TipoFondo::sabbia;
    const CampoPerdita v = calcola_campo(300.0, scarpata);
    CHECK(v.tl(1000.0, 400.0, 18.0).value() < 60.0);   // circa la sferica (52 dB), non 74 come col ventaglio di 15 gradi
    const double tl61 = c.tl(150.0, 61000.0, 90.0).value();
    CHECK(tl61 > 90.0);    // 94,8 col profilo di ottobre (con quello di settembre del 1955-2022: 97-110)
    CHECK(tl61 < 105.0);
    // sotto l'angolo critico la sabbia riflette quasi tutto; il fango no
    // in basso resta la sola perdita nel sedimento; a 1 kHz anche rugosita' e strati (27/10, seconda correzione)
    CHECK(perdita_fondo_db(TipoFondo::sabbia, 250.0, 9.0) < 1.0);
    CHECK(perdita_fondo_db(TipoFondo::sabbia, 1000.0, 9.0) > 1.5);
    CHECK(perdita_fondo_db(TipoFondo::sabbia, 1000.0, 9.0) < 2.5);
    CHECK(perdita_fondo_db(TipoFondo::sabbia, 1000.0, 40.0) == doctest::Approx(5.0 * 1.5));
    CHECK(perdita_fondo_db(TipoFondo::fango, 1000.0, 9.0) == doctest::Approx(11.0));
    CHECK(perdita_fondo_db(TipoFondo::roccia, 250.0, 30.0) < 1.2);   // a meta' dell'angolo critico
    CHECK(perdita_fondo_db(TipoFondo::roccia, 1000.0, 30.0) < 2.0);
    // acque profonde col fondo di fango: come prima della correzione (valori del 27/10, entro 0,1 dB a 1 kHz; a 150 Hz
    // entro 0,5, perche' la superficie mossa ora si calcola a 250 Hz e disperde un po' meno). Rifatti il 29/10/2026 coi
    // profili 1985-1994: il tracciatore e' lo stesso, cambia solo l'acqua
    struct Caso { const char* teatro; int mese; double zs, zr; std::array<double, 6> tl; };
    for (const Caso& k : {Caso{"gibilterra", 8, 50, 100, {94.58, 82.59, 82.65, 95.08, 84.08, 85.51}},
                          Caso{"faroe", 2, 30, 200, {79.50, 87.94, 86.61, 80.53, 90.18, 90.28}}}) {
        const ClimaTeatro cl = carica_clima(std::string(M688_DATI "/teatri/") + k.teatro + ".suono.toml");
        Ambiente p;
        p.profilo = &cl.profilo(k.mese);
        p.fondale_m = 3000.0;
        p.fondo = TipoFondo::fango;
        const CampoPerdita f = calcola_campo(k.zs, p);
        int i = 0;
        for (const double hz : {150.0, 1000.0})
            for (const double r : {10e3, 30e3, 60e3}) {
                const double atteso = k.tl[static_cast<std::size_t>(i++)];
                CHECK(std::fabs(f.tl(hz, r, k.zr).value() - atteso) < (hz < 500.0 ? 0.5 : 0.13));
            }
    }
}

TEST_CASE("acustica: sotto la volta del ghiaccio le basse frequenze passano, le alte si perdono (dossier 06 §5.1)") {
    // il Barents d'inverno, acqua profonda di prova: il semicanale artico con la superficie fredda
    Ambiente libero;
    libero.profilo = &clima("barents").profilo(2);
    libero.fondale_m = 3000.0;
    libero.fondo = TipoFondo::fango;
    libero.vento_m_s = 3.0;
    Ambiente primo = libero, vecchio = libero;
    primo.ghiaccio = vecchio.ghiaccio = 1.0;
    primo.rugosita_ghiaccio_m = 1.0;
    vecchio.rugosita_ghiaccio_m = 2.5;
    const CampoPerdita c0 = calcola_campo(60.0, libero), c1 = calcola_campo(60.0, primo), c2 = calcola_campo(60.0, vecchio);
    const auto tl = [](const CampoPerdita& c, double hz) { return c.tl(hz, 50000.0, 60.0).value(); };
    MESSAGE("a 50 km, 250 Hz: libero ", tl(c0, 250.0), ", primo anno ", tl(c1, 250.0), ", pluriennale ", tl(c2, 250.0),
            "; 1 kHz: ", tl(c0, 1000.0), ", ", tl(c1, 1000.0), ", ", tl(c2, 1000.0));
    // le alte frequenze: la volta ruvida le disperde, di piu' il pack vecchio
    CHECK(tl(c1, 1000.0) > tl(c0, 1000.0) + 5.0);    // 8,5 dB col primo anno (29/10/2026)
    CHECK(tl(c2, 1000.0) > tl(c0, 1000.0) + 15.0);   // 27,6 col pluriennale
    CHECK(tl(c2, 1000.0) > tl(c1, 1000.0));
    // le basse (le bande sotto 500 Hz usano la griglia a 250 Hz) perdono meno delle alte
    CHECK(tl(c1, 250.0) - tl(c0, 250.0) < tl(c1, 1000.0) - tl(c0, 1000.0));
    // senza ghiaccio nessun effetto della rugosita'
    Ambiente rugoso = libero;
    rugoso.rugosita_ghiaccio_m = 5.0;
    CHECK(calcola_campo(60.0, rugoso).tl_db == c0.tl_db);
}

// Sotto la volta (opzione A, punto 5; dossier 06 §5.1): il rumore del pack deformato e delle fessure termiche, e la
// riverberazione della volta rugosa per l'attivo. Senza creste ne' fessure il rumore del ghiaccio resta quello di prima
TEST_CASE("acustica sotto la volta: le creste a bassa frequenza, le fessure ad alta, la volta che riverbera l'attivo") {
    Ambiente quieto;   // il pack chiuso dell'Artico: niente vento sul mare, quasi niente traffico lontano
    quieto.ghiaccio = 1.0;
    quieto.vento_m_s = 0.0;
    quieto.traffico = 0.0;
    Ambiente creste = quieto, fessure = quieto;
    creste.ghiaccio_creste = 1.0;
    fessure.ghiaccio_fessure = 1.0;
    // il pack quieto e' quello di sempre
    Ambiente zero = quieto;
    zero.ghiaccio_creste = 0.0;
    zero.ghiaccio_fessure = 0.0;
    CHECK(rumore_ambiente(300.0, zero) == rumore_ambiente(300.0, quieto));
    MESSAGE("100 Hz: quieto ", rumore_ambiente(100.0, quieto), ", creste ", rumore_ambiente(100.0, creste), "; 2 kHz: quieto ",
            rumore_ambiente(2000.0, quieto), ", fessure ", rumore_ambiente(2000.0, fessure));
    CHECK(rumore_ambiente(100.0, creste) > rumore_ambiente(100.0, quieto) + 5.0);
    CHECK(rumore_ambiente(5000.0, creste) < rumore_ambiente(5000.0, quieto) + 1.0);
    CHECK(rumore_ambiente(2000.0, fessure) > rumore_ambiente(2000.0, quieto) + 5.0);
    CHECK(rumore_ambiente(30.0, fessure) < rumore_ambiente(30.0, quieto) + 1.0);
    CHECK(componenti_rumore_ambiente(100.0, creste).ghiaccio > componenti_rumore_ambiente(100.0, quieto).ghiaccio + 5.0);
    // l'attivo: sotto la volta la riverberazione di superficie sale e l'eccesso di segnale cala; piu' rugosa, peggio
    Ricevitore r;
    r.quota_m = 100.0;
    r.sensore = catalogo().sensore("bqq5_attivo");
    // a 2 km i raggi toccano la volta con un angolo utile (a 8 km quasi non la vedono, la riverberazione resta di volume)
    const Sorgente bersaglio = sorgente("victor3", 5.0, 100.0, {2000.0, 0.0});
    Ambiente mare;
    mare.fondale_m = 3000.0;
    mare.vento_m_s = 2.0;
    Ambiente volta = mare;
    volta.ghiaccio = 1.0;
    Ambiente pluriennale = volta;
    pluriennale.rugosita_ghiaccio_m = 2.5;
    const Rilevazione a = valuta(bersaglio, r, mare), b = valuta(bersaglio, r, volta), c = valuta(bersaglio, r, pluriennale);
    MESSAGE("attivo a 2 km: riverberazione mare ", a.rl_db, ", volta ", b.rl_db, ", pluriennale ", c.rl_db, "; eccesso ", a.se_db,
            " e ", b.se_db);
    CHECK(b.rl_db > a.rl_db + 6.0);
    CHECK(c.rl_db > b.rl_db);
    CHECK(b.se_db < a.se_db - 6.0);   // l'attivo ad alta potenza sotto la volta e' quasi inutile (sotto-ghiaccio-piano.md)
}

// Il Polinom dell'Udaloy (docs/ricerca/31 §4): la cupola grande a bassa frequenza sente e trova piu' lontano del sonar navale
// sovietico generico, in passivo e in attivo; l'Udaloy l'ha nella sua suite, col «Horse Tail» a profondita' variabile
// (docs/ricerca/33: polinom_vds, alla quota del pesce in prove_vds)
TEST_CASE("acustica: il Polinom dell'Udaloy oltre il sonar navale sovietico, in passivo e in attivo") {
    const auto suite = catalogo().sensori_di("udaloy");
    REQUIRE(suite.size() == 4);
    CHECK(suite[0]->id == "polinom");
    CHECK(suite[1]->id == "polinom_attivo");
    CHECK(suite[2]->id == "polinom_vds");
    CHECK(suite[3]->id == "polinom_vds_attivo");
    Ambiente amb;
    amb.fondale_m = 3000.0;
    amb.vento_m_s = 7.0;
    const auto se = [&](const char* sensore, double d) {
        Ricevitore r;
        r.quota_m = 7.0;
        r.velocita_kn = 12.0;
        r.sensore = catalogo().sensore(sensore);
        return valuta(sorgente("victor3", 8.0, 100.0, {d, 0.0}), r, amb).se_db;
    };
    MESSAGE("Victor III a 8 nodi: passivo a 8 km Polinom ", se("polinom", 8000.0), " dB, generico ", se("nave_sovietica", 8000.0),
            "; attivo a 12 km Polinom ", se("polinom_attivo", 12000.0), ", Titan-2 ", se("titan2_attivo", 12000.0));
    CHECK(se("polinom", 8000.0) > se("nave_sovietica", 8000.0) + 3.0);
    CHECK(se("polinom_attivo", 12000.0) > se("titan2_attivo", 12000.0));
}
