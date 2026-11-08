#include <doctest.h>

#include <cmath>

#include "nucleo/sagoma_notte.hpp"

using namespace m688;

// La sagoma a luci spente di notte (docs/27 §2): la Luna, la luce del cielo, la taratura sui 3 km, la foschia, il
// binocolo, l'aspetto, il controluce

namespace {

const Sagoma CACCIA{130.0, 12.0, 30.0, 0.0};   // la sagoma di riferimento

Osservatore vedetta() {
    Osservatore o;
    o.altezza_occhio_m = 20.0;
    o.allertato = true;
    return o;
}

CondizioniVista vista(double visibilita_m) {
    CondizioniVista c;
    c.luce = 0.0;
    c.visibilita_m = visibilita_m;
    return c;
}

CieloNotte cielo(double lux) {
    CieloNotte c;
    c.lux = lux;
    return c;
}

}  // namespace

TEST_CASE("sagoma di notte: la falce della Luna, il lembo illuminato verso il Sole") {
    // gennaio 2000: il primo quarto il 14 alle 13.34 UT, l'ultimo il 28 alle 7.57 UT. A nord la Luna crescente ha il
    // lembo a destra, la calante a sinistra; a sud il contrario
    const PosizioneLuna crescente = posizione_luna(DataOra{2000, 1, 14, 17.5}, 41.9, 12.5);   // Roma, la sera
    const PosizioneLuna calante = posizione_luna(DataOra{2000, 1, 28, 5.0}, 41.9, 12.5);      // Roma, il mattino
    const PosizioneLuna sud = posizione_luna(DataOra{2000, 1, 14, 10.5}, -33.9, 151.2);       // Sydney, la sera
    MESSAGE("crescente: ", crescente.illuminata, " lembo ", crescente.angolo_lembo, "; calante: ", calante.illuminata, " lembo ",
            calante.angolo_lembo, "; a sud: ", sud.illuminata, " lembo ", sud.angolo_lembo);
    REQUIRE(crescente.elevazione > 10.0);
    REQUIRE(calante.elevazione > 10.0);
    REQUIRE(sud.elevazione > 10.0);
    CHECK(crescente.illuminata == doctest::Approx(0.5).epsilon(0.1));
    CHECK(calante.illuminata == doctest::Approx(0.5).epsilon(0.1));
    CHECK(crescente.angolo_lembo > 30.0);
    CHECK(crescente.angolo_lembo < 150.0);
    CHECK(calante.angolo_lembo > 210.0);
    CHECK(calante.angolo_lembo < 330.0);
    CHECK(sud.angolo_lembo > 210.0);
    CHECK(sud.angolo_lembo < 330.0);
    // nel cielo della vista: gli stessi numeri; senza data niente Luna
    const CieloNotte c = cielo_notte(DataOra{2000, 1, 14, 17.5}, 41.9, 12.5, 0.0);
    CHECK(c.luna_illuminata == crescente.illuminata);
    CHECK(c.luna_angolo_lembo == crescente.angolo_lembo);
    CHECK(cielo_notte(0.0, 0.0).luna_illuminata == 0.0);
}

TEST_CASE("sagoma di notte: la Luna piena e la nuova, alta e sotto l'orizzonte; la luce del cielo") {
    // l'eclisse totale del 21/1/2000 (piena alle 4.40 UT), la nuova del 6/1/2000 alle 18.14 UT
    const DataOra piena{2000, 1, 21, 4.67}, nuova{2000, 1, 6, 18.23};
    const PosizioneLuna p = posizione_luna(piena, 40.7, -74.0);   // New York, l'eclisse alta nel cielo
    MESSAGE("piena: illuminata ", p.illuminata, ", elevazione ", p.elevazione, ", azimut ", p.azimut);
    CHECK(p.illuminata > 0.97);
    CHECK(p.elevazione > 40.0);
    CHECK(posizione_luna(piena, -40.7, 106.0).elevazione < 0.0);   // dall'altra parte della Terra
    CHECK(posizione_luna(nuova, 40.7, -74.0).illuminata < 0.03);
    // la luce: il crepuscolo dalle tavole, le stelle col sereno e col coperto, la piena che illumina
    CHECK(lux_sole(-12.0) == doctest::Approx(0.008).epsilon(0.05));
    CHECK(lux_sole(-6.0) == doctest::Approx(3.16).epsilon(0.05));
    CHECK(lux_sole(0.0) > 500.0);
    CHECK(lux_sole(-20.0) == 0.0);
    CHECK(cielo_notte(0.0, 0.0).lux == doctest::Approx(0.002));
    CHECK(cielo_notte(0.0, 8.0).lux < 0.0005);
    CHECK(cielo_notte(0.5, 0.0).lux > 1.0);   // il crepuscolo
    const CieloNotte con_luna = cielo_notte(piena, 40.7, -74.0, 0.0);
    CHECK(con_luna.luna_su);
    CHECK(con_luna.lux > 0.1);
    CHECK(cielo_notte(DataOra{2000, 1, 7, 5.0}, 40.7, -74.0, 0.0).lux < 0.003);   // la notte dopo la nuova: le stelle
}

TEST_CASE("sagoma di notte: la taratura sui 3 km, il binocolo, la Luna, il coperto, la foschia, l'aspetto") {
    const Osservatore o = vedetta();
    const CondizioniVista c = vista(30000.0);
    const double stelle = portata_sagoma_notte_m(CACCIA, 90.0, o, c, cielo(0.002), 0.0);
    CHECK(stelle == doctest::Approx(3000.0).epsilon(0.02));
    const double binocolo = portata_sagoma_notte_m(CACCIA, 90.0, o, c, cielo(0.002), 0.0, INGRANDIMENTO_BINOCOLO);
    const double luna = portata_sagoma_notte_m(CACCIA, 90.0, o, c, cielo(0.25), 0.0);
    const double coperto = portata_sagoma_notte_m(CACCIA, 90.0, o, c, cielo(0.0003), 0.0);
    const double prora = portata_sagoma_notte_m(CACCIA, 0.0, o, c, cielo(0.002), 0.0);
    Osservatore distratta = o;
    distratta.allertato = false;
    const double non_allertata = portata_sagoma_notte_m(CACCIA, 90.0, distratta, c, cielo(0.002), 0.0);
    const double nebbia = portata_sagoma_notte_m(CACCIA, 90.0, o, vista(1000.0), cielo(0.25), 0.0);
    MESSAGE("caccia di 130 m al traverso: stelle ", stelle, ", 7x50 ", binocolo, ", Luna piena ", luna, ", coperto ",
            coperto, ", di prora ", prora, ", non allertata ", non_allertata, ", nebbia di 1 km con la Luna ", nebbia);
    CHECK(binocolo > 1.3 * stelle);
    CHECK(binocolo < 2.0 * stelle);
    CHECK(luna > 1.6 * stelle);
    CHECK(coperto < 0.85 * stelle);
    CHECK(coperto > 0.6 * stelle);
    CHECK(prora < 0.6 * stelle);
    CHECK(non_allertata < stelle);
    CHECK(nebbia < 900.0);   // di notte ben prima della visibilita'
    CHECK(nebbia > 300.0);
    // il sensore: l'intensificatore vede piu' lontano dell'occhio; il termico non sente la Luna
    Osservatore notturna = o;
    notturna.notte = ModoVista::intensificatore;
    CHECK(portata_sagoma_notte_m(CACCIA, 90.0, notturna, c, cielo(0.002), 0.0) > 2.0 * stelle);
    notturna.notte = ModoVista::termico;
    CHECK(portata_sagoma_notte_m(CACCIA, 90.0, notturna, c, cielo(0.002), 0.0) ==
          doctest::Approx(portata_sagoma_notte_m(CACCIA, 90.0, notturna, c, cielo(0.25), 0.0)));
    // il 688 in superficie si vede molto meno della nave; il peschereccio poco
    const double sub = portata_sagoma_notte_m(sagoma_da_tipo("sottomarino", 110.0), 90.0, o, c, cielo(0.002), 0.0);
    const double pesca = portata_sagoma_notte_m(sagoma_da_tipo("peschereccio", 25.0), 90.0, o, c, cielo(0.002), 0.0);
    MESSAGE("688 in superficie al traverso con le stelle ", sub, " m, peschereccio di 25 m ", pesca);
    CHECK(sub < 0.6 * stelle);
    CHECK(pesca < stelle);
    // l'orizzonte della cima: da un periscopio a pelo d'acqua con la Luna piena un mercantile lontano resta sotto
    Osservatore periscopio = o;
    periscopio.altezza_occhio_m = 1.0;
    const Sagoma petroliera = sagoma_da_tipo("mercantile", 300.0);
    CHECK(portata_sagoma_notte_m(petroliera, 90.0, periscopio, vista(50000.0), cielo(0.25), 0.0, 6.0) <=
          3843.0 * (1.0 + std::sqrt(petroliera.altezza_max_m)) + 1e-6);
}

TEST_CASE("sagoma di notte: la Luna bassa dietro la nave la stacca; la probabilita'") {
    const Osservatore o = vedetta();
    const CondizioniVista c = vista(30000.0);
    CieloNotte bassa = cielo(0.05);
    bassa.luna_su = true;
    bassa.luna_elevazione = 10.0;
    bassa.luna_azimut = 90.0;
    const double dietro = portata_sagoma_notte_m(CACCIA, 90.0, o, c, bassa, 80.0);
    const double altrove = portata_sagoma_notte_m(CACCIA, 90.0, o, c, bassa, 200.0);
    CHECK(dietro > 1.2 * altrove);   // x1,5 in aria limpida, meno con la foschia
    bassa.luna_elevazione = 50.0;   // alta non stacca
    CHECK(portata_sagoma_notte_m(CACCIA, 90.0, o, c, bassa, 80.0) == doctest::Approx(altrove));
    // la probabilita': la meta' del tasso alla portata, quasi sicura vicino in un minuto, nulla oltre il doppio
    const double r = portata_sagoma_notte_m(CACCIA, 90.0, o, c, cielo(0.002), 0.0);
    CHECK(probabilita_sagoma_notte(CACCIA, 90.0, o, c, cielo(0.002), 0.0, r, 10.0) ==
          doctest::Approx(1.0 - std::exp(-0.05 * 10.0)));
    CHECK(probabilita_sagoma_notte(CACCIA, 90.0, o, c, cielo(0.002), 0.0, 500.0, 60.0) > 0.99);
    CHECK(probabilita_sagoma_notte(CACCIA, 90.0, o, c, cielo(0.002), 0.0, 2.01 * r, 60.0) == 0.0);
    CHECK(probabilita_sagoma_notte(CACCIA, 90.0, o, c, cielo(0.002), 0.0, 500.0, 0.0) == 0.0);
}

TEST_CASE("sagoma a vista di giorno: la stessa legge da' i 15-20 km d'oggi col sereno, meno con la foschia e all'alba") {
    const Osservatore o = vedetta();
    const CieloNotte giorno = cielo_notte(1.0, 0.0), coperto = cielo_notte(1.0, 8.0), alba = cielo_notte(0.3, 0.0);
    CHECK(giorno.lux > 10000.0);
    const double sereno = portata_sagoma_notte_m(CACCIA, 90.0, o, vista(30000.0), giorno, 0.0);
    const double foschia = portata_sagoma_notte_m(CACCIA, 90.0, o, vista(10000.0), giorno, 0.0);
    const double nebbia = portata_sagoma_notte_m(CACCIA, 90.0, o, vista(2000.0), giorno, 0.0);
    const double grigio = portata_sagoma_notte_m(CACCIA, 90.0, o, vista(30000.0), coperto, 0.0);
    const double crepuscolo = portata_sagoma_notte_m(CACCIA, 90.0, o, vista(30000.0), alba, 0.0);
    const double stelle = portata_sagoma_notte_m(CACCIA, 90.0, o, vista(30000.0), cielo(0.002), 0.0);
    MESSAGE("caccia di 130 m al traverso di giorno: ", sereno, " m col sereno, ", foschia, " a 10 km di visibilita', ", nebbia,
            " a 2 km, ", grigio, " col coperto, ", crepuscolo, " all'alba");
    CHECK(sereno > 14000.0);
    CHECK(sereno < 21000.0);
    // di giorno la sagoma scura si perde vicino alla visibilita' meteorologica (Koschmieder, contrasto 0,02)
    CHECK(foschia > 7000.0);
    CHECK(foschia < 10000.0);
    CHECK(nebbia > 1500.0);
    CHECK(nebbia <= 2000.5);   // mai oltre la visibilita' di giorno
    CHECK(grigio < sereno);
    CHECK(grigio > 0.8 * sereno);
    CHECK(crepuscolo < sereno);
    CHECK(crepuscolo > stelle);
    // il periscopio a pelo d'acqua: l'orizzonte della cima
    Osservatore periscopio = o;
    periscopio.altezza_occhio_m = 1.0;
    CHECK(portata_sagoma_notte_m(CACCIA, 90.0, periscopio, vista(50000.0), giorno, 0.0) <= 3843.0 * (1.0 + std::sqrt(30.0)) + 1e-6);
    // il 688 emerso di giorno: la vela si vede, ma molto meno della nave
    const double sub = portata_sagoma_notte_m(sagoma_da_tipo("sottomarino", 110.0), 90.0, o, vista(30000.0), giorno, 0.0);
    MESSAGE("688 emerso di giorno: ", sub, " m");
    CHECK(sub < 0.7 * sereno);   // la vela e lo scafo bassi: a 10 km col 7x50 (STIMA)
    CHECK(sub > 3000.0);
}
