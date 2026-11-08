#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <string>

#include <filesystem>
#include <fstream>
#include <iterator>
#include <map>
#include <set>

#include "missioni/carriera_missione.hpp"
#include "missioni/missione.hpp"
#include "missioni/stazioni_missione.hpp"
#include "nucleo/armi_giocatore.hpp"
#include "nucleo/carico.hpp"
#include "nucleo/compiti_carico.hpp"

using namespace m688;

// Carico e Sala siluri (Fase 4a, M2; dossier docs/ricerca/14_carico_armi.md §5)

namespace {

const CatalogoCarico& catalogo() {
    static const CatalogoCarico c = carica_carico(M688_DATI "/carico.toml");
    return c;
}

LimitiMissione originale_688() {   // SCENE1A: dotazione e tubi dell'originale, nessun deposito
    LimitiMissione l;
    l.dotazione = {{"mk48", 22}, {"harpoon", 2}, {"esca", 6}};
    l.tubi_originali = {"mk48", "mk48", "mk48", "mk48", "esca", "esca", "harpoon"};
    return l;
}

ArmaDisponibile cerca(const std::vector<ArmaDisponibile>& v, const std::string& id) {
    const auto it = std::find_if(v.begin(), v.end(), [&](const ArmaDisponibile& d) { return d.id == id; });
    REQUIRE(it != v.end());
    return *it;
}

}  // namespace

TEST_CASE("carico: catalogo, schede e locali") {
    const auto& c = catalogo();
    const SchedaArma* adcap = c.arma("mk48_adcap");
    REQUIRE(adcap);
    CHECK(adcap->famiglia == "mk48");
    CHECK(adcap->siluro == "mk48_adcap");
    CHECK(adcap->in_servizio_dal == 1988);
    CHECK(adcap->peso_kg == doctest::Approx(1676.0));
    CHECK(c.arma("tasm")->pozzo_verticale);
    CHECK(c.arma("tasm")->sagoma == "sprite:navi/tomahawk.spr");
    CHECK(c.arma("sea_lance")->what_if);
    CHECK(c.arma("shkval")->solo_ia);
    CHECK(c.arma("pmr1")->posti == doctest::Approx(1.0));   // mina-razzo lunga come un siluro
    CHECK(c.arma("mdt")->posti == doctest::Approx(0.5));    // due per tubo
    // efficacia per bersaglio (dossier 15 §7.5)
    CHECK(c.arma("mk48")->stelle("sottomarino") == 3);
    CHECK(c.arma("harpoon")->stelle("sottomarino") == 0);
    CHECK(c.arma("type53_65k")->stelle("sottomarino") == 0);
    CHECK(c.arma("tlam_c")->stelle("terra") == 3);
    CHECK(c.arma("mk67")->stelle("mine") == 3);
    CHECK(c.arma("mk48")->stelle("ignoto") == 0);
    CHECK(c.arma("mk60")->efficacia_confidenza == "STIMA");
    for (const auto& [id, a] : c.armi) CHECK(a.efficacia.size() == 4);
    CHECK(c.predefinita("type53", "urss") == "set65");
    CHECK(c.predefinita("esca", "usa") == "adc_mk2");
    CHECK(c.locale("los_angeles")->pozzi == 0);
    CHECK(c.locale("los_angeles", "flight2")->pozzi == 12);
    CHECK(c.locale("los_angeles", "flight1")->id == "los_angeles");
    CHECK(c.locale("alfa")->tubi + c.locale("alfa")->rastrelliera == 18);
    CHECK(c.locale("perry") == nullptr);
    CHECK_THROWS(SalaSiluri(c, "perry", {}));
}

TEST_CASE("carico: missione originale, carico predefinito come la dotazione") {
    SalaSiluri s(catalogo(), "los_angeles", originale_688());
    s.carico_iniziale();
    const CaricoPartenza& k = s.carico();
    // Flight I: il missile del tubo 7 nell'ultimo tubo da siluri, il tubo 7 resta vuoto
    CHECK(k.posizioni == std::vector<std::string>{"mk48", "mk48", "mk48", "harpoon", "adc_mk2", "adc_mk2", ""});
    CHECK(k.rastrelliera.at("mk48") == 19);
    CHECK(k.rastrelliera.at("harpoon") == 1);
    CHECK(k.esche.at("adc_mk2") == 6);
    CHECK(s.posti_usati() == doctest::Approx(24.0));
    CHECK(s.posti_totali() == 25);
    CHECK(s.pronti_nei_tubi() == 4);
    const auto d = s.disponibili();
    CHECK(cerca(d, "mk48").rimaste() == 0);
    CHECK(cerca(d, "mk48_adcap").rimaste() == 0);   // il 1989 della missione: dal deposito della famiglia, finito
    CHECK(cerca(d, "tlam_c").motivo == "non al deposito");
    CHECK(cerca(d, "mk67").motivo == "non al deposito");   // le mine solo se la missione le dichiara
    CHECK(cerca(d, "sea_lance").motivo == "what-if");
    CHECK(std::none_of(d.begin(), d.end(), [](const ArmaDisponibile& a) { return a.id == "set65"; }));   // altra marina
    const auto a = s.avvisi();
    CHECK(std::find(a.begin(), a.end(), "posti liberi in rastrelliera: 1") != a.end());
}

TEST_CASE("carico: Sea Lance dell'originale solo con l'opzione what-if") {
    LimitiMissione l = originale_688();
    l.dotazione = {{"mk48", 16}, {"sea_lance", 8}, {"esca", 6}};
    l.tubi_originali.back() = "sea_lance";
    SalaSiluri senza(catalogo(), "los_angeles", l);
    senza.carico_iniziale();
    CHECK(senza.carico().posizioni[3] == "mk48");   // il tubo resta al siluro
    CHECK(!senza.carico().rastrelliera.count("sea_lance"));
    SalaSiluri con(catalogo(), "los_angeles", l, true);
    con.carico_iniziale();
    CHECK(con.carico().posizioni[3] == "sea_lance");
    CHECK(con.carico().rastrelliera.at("sea_lance") == 7);
    const auto a = con.avvisi();
    CHECK(std::find(a.begin(), a.end(), "armi what-if a bordo") != a.end());
}

TEST_CASE("carico: Flight II, i Tomahawk della dotazione nei pozzi") {
    LimitiMissione l = originale_688();
    l.dotazione = {{"mk48", 20}, {"tomahawk", 4}, {"esca", 6}};
    l.tubi_originali.back() = "tomahawk";
    l.variante_battello = "flight2";
    SalaSiluri s(catalogo(), "los_angeles", l);
    s.carico_iniziale();
    CHECK(s.carico().pozzi.at("tasm") == 4);
    CHECK(s.carico().posizioni[6] == "tasm");   // la posizione dei pozzi mostra la variante
    CHECK(s.pozzi_usati() == 4);
    CHECK(s.carico().posizioni[3] == "mk48");
    CHECK(s.carico().rastrelliera.at("mk48") == 16);
}

TEST_CASE("carico: deposito, varianti, anno e tetto dell'ADCAP") {
    LimitiMissione l;
    l.deposito = {{"mk48", 25}, {"harpoon", 6}, {"tlam_c", 4}, {"esca", 6}};
    l.anno = 1989;
    SalaSiluri s(catalogo(), "los_angeles", l);
    auto d = s.disponibile("mk48_adcap");
    CHECK(!d.negata);
    CHECK(d.deposito == 6);   // tetto dei primi due anni, dal deposito della famiglia
    CHECK(s.disponibile("tlam_c").deposito == 4);
    for (int k = 0; k < 6; ++k) REQUIRE(s.metti(Posto::rastrelliera, "mk48_adcap").fatto);
    CHECK(s.metti(Posto::rastrelliera, "mk48_adcap").motivo == "deposito esaurito");
    CHECK(s.disponibile("mk48").deposito == 19);   // la famiglia ne ha date 6 all'ADCAP
    l.anno = 1987;
    SalaSiluri prima(catalogo(), "los_angeles", l);
    CHECK(prima.disponibile("mk48_adcap").motivo == "in servizio dal 1988");
    l.anno = 1991;
    SalaSiluri dopo(catalogo(), "los_angeles", l);
    CHECK(dopo.disponibile("mk48_adcap").deposito == 25);   // senza tetto
    l.anno = 1995;
    l.deposito["tomahawk"] = 4;
    SalaSiluri ritirata(catalogo(), "los_angeles", l);
    CHECK(ritirata.disponibile("tasm").motivo == "ritirata nel 1994");
    l.anno = 1989;
    l.tipi_negati = {"harpoon"};
    SalaSiluri negata(catalogo(), "los_angeles", l);
    CHECK(negata.disponibile("harpoon").motivo == "negata dalla missione");
    CHECK(negata.metti(Posto::tubo, "harpoon", 1).motivo == "negata dalla missione");
}

TEST_CASE("carico: posti condivisi, tubi, pozzi ed esche") {
    LimitiMissione l;
    l.deposito = {{"mk48", 30}, {"mk67", 10}, {"tomahawk", 4}, {"esca", 8}};
    l.ospiti = 6;
    SalaSiluri s(catalogo(), "los_angeles", l);
    CHECK(s.posti_totali() == 19);   // 4 + 21 - 6 per gli incursori
    CHECK(s.metti(Posto::tubo, "mk67", 1).fatto);   // le mine si posano dai tubi
    CHECK(s.metti(Posto::tubo, "mk48", 1).motivo == "tubo occupato");
    CHECK(s.metti(Posto::tubo, "mk48", 5).motivo == "tubo inesistente");
    CHECK(s.metti(Posto::tubo, "adc_mk2", 2).motivo == "le esche hanno i loro lanciatori");
    CHECK(s.metti(Posto::pozzi, "tasm").motivo == "il battello non ha pozzi verticali");
    for (int k = 0; k < 15; ++k) REQUIRE(s.metti(Posto::rastrelliera, "mk48").fatto);
    CHECK(s.metti(Posto::rastrelliera, "mk48").motivo == "posti finiti");
    CHECK(s.togli(Posto::rastrelliera, "mk48").fatto);
    CHECK(s.metti(Posto::rastrelliera, "tasm").fatto);   // Tomahawk da tubo: un siluro in meno
    for (int k = 0; k < 6; ++k) REQUIRE(s.metti(Posto::esche, "adc_mk2").fatto);
    CHECK(s.metti(Posto::esche, "adc_mk2").motivo == "scorta delle esche piena");
    CHECK(s.carico().posizioni[4] == "adc_mk2");
    CHECK(s.posti_usati() == doctest::Approx(16.0));
    CHECK(s.peso_kg() == doctest::Approx(754.0 + 14 * 1558.0 + 1590.0));
    CHECK(s.togli(Posto::tubo, "mk48", 1).motivo == "arma assente");
    CHECK(s.togli(Posto::tubo, "mk67", 1).fatto);
    l.variante_battello = "flight2";
    SalaSiluri f2(catalogo(), "los_angeles", l);
    CHECK(f2.metti(Posto::pozzi, "mk48").motivo == "non va nei pozzi");
    CHECK(f2.metti(Posto::pozzi, "tasm").fatto);
}

TEST_CASE("carico: carichi standard adattati al deposito") {
    LimitiMissione l;
    l.deposito = {{"mk48", 25}, {"harpoon", 6}, {"tomahawk", 4}, {"esca", 6}};
    l.tipo_missione = "antinave";
    SalaSiluri s(catalogo(), "los_angeles", l);
    s.carico_iniziale();
    CHECK(s.carico().posizioni == std::vector<std::string>{"mk48", "mk48", "mk48", "harpoon", "adc_mk2", "adc_mk2", ""});
    CHECK(s.carico().rastrelliera.at("mk48") == 13);
    CHECK(s.carico().rastrelliera.at("harpoon") == 4);
    CHECK(s.carico().rastrelliera.at("tasm") == 4);
    CHECK(s.posti_usati() == doctest::Approx(25.0));
    // minamento senza mine al deposito: al posto delle Mk 67 i Mk 48
    s.carico_standard("minamento");
    CHECK(s.carico().posizioni[2] == "mk48");
    CHECK(!s.carico().rastrelliera.count("mk67"));
    // tipo ignoto: il predefinito del locale (antisom)
    s.carico_standard("pesca");
    CHECK(s.carico().rastrelliera.at("mk48") == 19);
    // Alfa: SET-65 e 53-65K dal deposito della famiglia type53
    LimitiMissione a;
    a.deposito = {{"type53", 18}, {"esca", 4}};
    SalaSiluri alfa(catalogo(), "alfa", a);
    alfa.carico_iniziale();
    CHECK(alfa.carico().posizioni == std::vector<std::string>{"set65", "set65", "set65", "type53_65k", "type53_65k", "mg84"});
    CHECK(alfa.carico().rastrelliera.at("set65") + alfa.carico().rastrelliera.at("type53_65k") == 13);
    CHECK(alfa.carico().esche.at("mg84") == 4);
    CHECK(alfa.disponibile("uset80").motivo == "non al deposito");   // solo se dichiarato
    CHECK(alfa.disponibile("shkval").motivo == "solo IA");
}

namespace {

const std::vector<std::string> TUBI_688 = {"mk48", "mk48", "mk48", "mk48", "esca", "esca", "harpoon"};

void arma(StazioneArmi& s, int tubo, double t) {
    REQUIRE(s.stato(tubo) == StatoTubo::carico);
    s.consegna({t, TipoOrdine::carica_tubo, 0, 0, tubo}, {});
    REQUIRE(s.stato(tubo) == StatoTubo::in_armamento);
    s.messaggio_missione(9 + tubo);
    REQUIRE(s.stato(tubo) == StatoTubo::armato);
}

}  // namespace

TEST_CASE("carico: la stazione Armi parte con il carico della sala siluri") {
    LimitiMissione l;
    l.deposito = {{"mk48", 25}, {"harpoon", 6}, {"tomahawk", 8}, {"esca", 6}};
    l.tipo_missione = "antinave";
    l.variante_battello = "flight2";
    SalaSiluri sala(catalogo(), "los_angeles", l);
    sala.carico_iniziale();
    REQUIRE(sala.carico().pozzi.at("tasm") == 8);
    StazioneArmi s(TUBI_688, {{"mk48", 22}, {"harpoon", 2}, {"esca", 6}}, 7, 4);
    s.imposta_carico(sala.carico(), catalogo(), parametri_ricarica("los_angeles"));
    CHECK(s.con_carico());
    CHECK(s.ricarica_realistica());
    for (int t = 1; t <= 4; ++t) CHECK(s.stato(t) == StatoTubo::carico);
    CHECK(s.arma(4) == Arma::harpoon);
    CHECK(s.variante(4) == "harpoon");
    CHECK(s.arma(5) == Arma::esca);
    CHECK(s.stato(5) == StatoTubo::vuoto);   // i lanciatori delle esche come nel classico
    CHECK(s.pozzo(7));
    CHECK(s.arma(7) == Arma::tomahawk);
    CHECK(s.stato(7) == StatoTubo::carico);
    CHECK(s.pozzi().at("tasm") == 7);
    CHECK(s.scorte(Arma::mk48) == 17);
    CHECK(s.scorte(Arma::tomahawk) == 7);
    CHECK(s.rastrelliera().at("harpoon") == 4);

    // lancio dal tubo 1: la ricarica automatica riprende un Mk 48 dalla rastrelliera
    arma(s, 1, 0.0);
    const EsitoStazione e = s.consegna({0.0, TipoOrdine::lancia, 0, 0, 1}, {});
    CHECK(std::any_of(e.alla_missione.begin(), e.alla_missione.end(), [](const Ordine& o) { return o.tipo == TipoOrdine::lancia; }));
    CHECK(s.lanciata(1) == "mk48");
    CHECK(s.stato(1) == StatoTubo::in_ricarica);
    CHECK(s.rastrelliera().at("mk48") == 16);

    // cambio d'arma: in ricarica ricomincia con l'Harpoon, carico lo scambia con una ricarica intera (in coda)
    CHECK(s.consegna(s.scegli_arma(1, "harpoon", 10.0), {}).messaggio == MSG_CAMBIO_ARMA);
    CHECK(s.arma(1) == Arma::harpoon);
    CHECK(s.rastrelliera().at("mk48") == 17);
    CHECK(s.rastrelliera().at("harpoon") == 3);
    CHECK(s.consegna(s.scegli_arma(2, "harpoon", 10.0), {}).messaggio == MSG_CAMBIO_ARMA);
    CHECK(s.stato(2) == StatoTubo::in_ricarica);
    CHECK(s.rastrelliera().at("mk48") == 18);   // il Mk 48 del tubo 2 torna in rastrelliera
    CHECK(s.consegna(s.scegli_arma(3, "harpoon", 10.0), {}).messaggio == MSG_RICARICA_IN_CODA);
    CHECK(s.in_coda(3));
    CHECK(s.consegna(s.scegli_arma(3, "tasm", 10.0), {}).messaggio == MSG_ARMA_NON_IN_RASTRELLIERA);   // nei pozzi
    CHECK(s.consegna(s.scegli_arma(7, "mk48", 10.0), {}).messaggio == 0);   // i pozzi non cambiano arma
    arma(s, 4, 10.0);
    CHECK(s.consegna(s.scegli_arma(4, "mk48", 10.0), {}).messaggio == MSG_PRIMA_DISARMARE);

    // i pozzi verticali: dopo il lancio la cella successiva e' subito carica
    SituazioneArmi sit;
    sit.bersaglio_presente = sit.bersaglio_nave = sit.contatto = sit.traccia_solida = true;
    sit.distanza_m = 30000.0;
    s.consegna({20.0, TipoOrdine::designa_bersaglio, 0, 0, 3}, sit);
    arma(s, 7, 20.0);
    s.consegna({20.0, TipoOrdine::lancia, 0, 0, 7}, sit);
    CHECK(s.lanciata(7) == "tasm");
    CHECK(s.stato(7) == StatoTubo::carico);
    CHECK(s.pozzi().at("tasm") == 6);
    // il missile da un tubo da siluri ha le regole del tubo 7 dell'originale
    sit.quota = 50.0;
    CHECK(s.consegna({21.0, TipoOrdine::lancia, 0, 0, 4}, sit).messaggio == MSG_QUOTA_PERISCOPICA);

    // la ricarica del tubo 1 finisce: un Harpoon, armato come nell'originale
    const EsitoRicarica r = s.avanza(10.0 + 300.0);
    CHECK(std::find(r.messaggi.begin(), r.messaggi.end(), MSG_RICARICATO) != r.messaggi.end());
    CHECK(s.stato(1) == StatoTubo::in_armamento);
    CHECK(s.messaggio_missione(10) == MSG_ARMATO + static_cast<int>(Arma::harpoon));
}

TEST_CASE("carico: esche mobili da tubo, la MOSS solo nel modo moderno e la MG-74 sull'Alfa") {
    LimitiMissione l;
    l.deposito = {{"mk48", 25}, {"sea_lance", 4}, {"esca_mobile", 4}, {"esca", 6}};
    // 688 nel modo 1989: niente MOSS, nemmeno col what-if del giocatore (che consente la Sea Lance)
    const SalaSiluri anni89(catalogo(), "los_angeles", l, true, false);
    CHECK(anni89.disponibile("moss_mk70").motivo == "what-if");
    CHECK_FALSE(anni89.disponibile("sea_lance").negata);
    // nel modo moderno la MOSS c'e', la Sea Lance resta what-if senza l'opzione del giocatore
    SalaSiluri moderno(catalogo(), "los_angeles", l, false, true);
    CHECK_FALSE(moderno.disponibile("moss_mk70").negata);
    CHECK(moderno.disponibile("moss_mk70").deposito == 4);
    CHECK(moderno.disponibile("sea_lance").motivo == "what-if");
    REQUIRE(moderno.metti(Posto::tubo, "moss_mk70", 1).fatto);
    REQUIRE(moderno.metti(Posto::rastrelliera, "moss_mk70").fatto);
    CHECK(moderno.metti(Posto::esche, "moss_mk70").motivo == "non e' un'esca");   // un posto da siluro
    // prima del 1976 non c'e' nemmeno nel modo moderno
    LimitiMissione l1975 = l;
    l1975.anno = 1975;
    CHECK(SalaSiluri(catalogo(), "los_angeles", l1975, true, true).disponibile("moss_mk70").motivo == "in servizio dal 1976");
    // i carichi standard del 688 non la prendono: uguali nei due modi
    for (const char* tipo : {"antisom", "antinave", "attacco_terra", "minamento", "scorta"}) {
        SalaSiluri a(catalogo(), "los_angeles", l, false, false), b(catalogo(), "los_angeles", l, false, true);
        a.carico_standard(tipo);
        b.carico_standard(tipo);
        CHECK(a.carico() == b.carico());
    }
    // l'Alfa: due MG-74 nel carico antisom; senza esche mobili al deposito, al loro posto il siluro principale
    LimitiMissione la;
    la.deposito = {{"type53", 18}, {"esca_mobile", 2}, {"esca", 4}};
    la.tipo_missione = "antisom";
    SalaSiluri alfa(catalogo(), "alfa", la);
    alfa.carico_iniziale();
    CHECK(alfa.carico().rastrelliera.at("mg74") == 2);
    la.deposito.erase("esca_mobile");
    SalaSiluri senza(catalogo(), "alfa", la);
    senza.carico_iniziale();
    CHECK(senza.carico().rastrelliera.count("mg74") == 0);
    CHECK(senza.carico().rastrelliera.at("set65") == 8);
    CHECK(senza.disponibile("mg74").motivo == "non al deposito");
    // nella stazione: il tubo con l'esca mobile, la ricarica dalla rastrelliera, il lancio e la variante uscita
    StazioneArmi s({"type53", "type53", "type53", "type53", "type53", "esca"}, {}, 6, 6);
    s.imposta_carico(alfa.carico(), catalogo(), parametri_ricarica("alfa"));
    CHECK(s.scorte(Arma::esca_mobile) == 2);
    CHECK(s.consegna(s.scegli_arma(1, "mg74", 0.0), {}).messaggio == MSG_CAMBIO_ARMA);
    CHECK(s.arma(1) == Arma::esca_mobile);
    CHECK(s.rastrelliera().at("mg74") == 1);
    s.avanza(75.0);
    CHECK(s.stato(1) == StatoTubo::in_armamento);
    CHECK(s.messaggio_missione(10) == MSG_ARMATO + static_cast<int>(Arma::esca));   // «esca pronta»
    const EsitoStazione e = s.consegna({80.0, TipoOrdine::lancia, 0, 0, 1}, {});
    CHECK(std::any_of(e.alla_missione.begin(), e.alla_missione.end(), [](const Ordine& o) { return o.tipo == TipoOrdine::lancia; }));
    CHECK(s.lanciata(1) == "mg74");
    CHECK(s.in_acqua() == 1);   // come le esche dei lanciatori, finche' resta in gioco
    CHECK(nome_arma(Arma::esca_mobile) == "esca_mobile");
    CHECK(arma_da_nome("esca_mobile") == Arma::esca_mobile);
    // missioni originali (senza deposito): due esche mobili in piu' nella dotazione. Il 688 nel modo 1989 ha la MOSS
    // negata; nel moderno al deposito, ma il carico predefinito resta lo stesso (la carica il giocatore); l'Alfa ha la
    // MG-74 nel carico, al posto di due SET-65
    SalaSiluri o89(catalogo(), "los_angeles", originale_688(), true, false), omod(catalogo(), "los_angeles", originale_688(), false, true);
    o89.carico_iniziale();
    omod.carico_iniziale();
    CHECK(o89.carico() == omod.carico());
    CHECK(omod.carico().rastrelliera.count("moss_mk70") == 0);
    CHECK(omod.carico().rastrelliera.at("harpoon") == 1);
    CHECK(o89.disponibile("moss_mk70").motivo == "what-if");
    CHECK(omod.disponibile("moss_mk70").rimaste() == 2);
    CHECK(omod.metti(Posto::rastrelliera, "moss_mk70").fatto);   // nel posto libero
    LimitiMissione oa;
    oa.dotazione = {{"type53", 18}, {"esca", 4}};
    oa.tubi_originali = {"type53", "type53", "type53", "type53", "type53", "esca"};
    SalaSiluri alfa_orig(catalogo(), "alfa", oa);
    alfa_orig.carico_iniziale();
    CHECK(alfa_orig.carico().rastrelliera.at("mg74") == 2);
    CHECK(alfa_orig.carico().rastrelliera.at("set65") == 11);
}

TEST_CASE("carico: mine nei tubi, tubi vuoti alla partenza") {
    LimitiMissione l;
    l.deposito = {{"mk48", 20}, {"mk67", 4}, {"esca", 6}};
    SalaSiluri sala(catalogo(), "los_angeles", l);
    REQUIRE(sala.metti(Posto::tubo, "mk67", 1).fatto);
    for (int k = 0; k < 10; ++k) REQUIRE(sala.metti(Posto::rastrelliera, "mk48").fatto);
    StazioneArmi s(TUBI_688, {}, 7, 4);
    s.imposta_carico(sala.carico(), catalogo(), parametri_ricarica("los_angeles"));
    CHECK(s.arma(1) == Arma::mina);
    CHECK(s.arma(2) == Arma::mk48);   // vuoto: la famiglia della rastrelliera
    CHECK(s.stato(2) == StatoTubo::vuoto);
    CHECK(s.arma(7) == Arma::nessuna);   // Flight I: nessun tubo dei missili
    s.consegna({0.0, TipoOrdine::carica_tubo, 0, 0, 1}, {});
    CHECK(s.messaggio_missione(10) == MSG_MINA_ARMATA);
    const EsitoStazione posa = s.consegna({1.0, TipoOrdine::lancia, 0, 0, 1}, {});   // la posa la fa la missione
    CHECK(std::any_of(posa.alla_missione.begin(), posa.alla_missione.end(), [](const Ordine& o) { return o.tipo == TipoOrdine::lancia; }));
    CHECK(s.lanciata(1) == "mk67");
    CHECK(s.in_acqua() == 0);   // le mine non sono armi in acqua
    CHECK(s.consegna({1.0, TipoOrdine::carica_tubo, 0, 0, 2}, {}).messaggio == MSG_RICARICA_INIZIATA);
    CHECK(s.variante(2) == "mk48");
    CHECK(s.rastrelliera().at("mk48") == 9);
    CHECK(s.indice_variante("mk67") >= 0);
    CHECK(s.indice_variante("tasm") == -1);
}

TEST_CASE("carico: scarto dal carico standard") {
    LimitiMissione l;
    l.deposito = {{"mk48", 25}, {"harpoon", 6}, {"tomahawk", 4}, {"esca", 6}};
    SalaSiluri s(catalogo(), "los_angeles", l);
    s.carico_standard("antinave");
    CHECK(scarto_dal_standard(s.carico(), catalogo(), "los_angeles", "antinave") == doctest::Approx(0.0));
    // contro l'antisom: 7 Mk 48 in meno, 3 Harpoon e 4 TASM in piu'
    const double x = scarto_dal_standard(s.carico(), catalogo(), "los_angeles", "antisom");
    CHECK(x == doctest::Approx(0.5 * 14 / 25.0));
    CHECK(scarto_dal_standard(s.carico(), catalogo(), "los_angeles", "pesca") == 0.0);
}

TEST_CASE("carico: campi della missione e blocco carriera") {
    const auto f = std::filesystem::temp_directory_path() / "m688_prova_carico.lua";
    {
        std::ofstream o(f);
        o << "return { id = 'PROVA_CARICO', teatro = 'faroe', paesaggio_sonoro = false, data = '1990-03-01',\n"
             "dotazione = { mk48 = 20, esca = 6 }, tubi = { 'mk48', 'mk48', 'mk48', 'mk48', 'esca', 'esca' },\n"
             "deposito = { mk48 = 21, mk48_adcap = 4, tomahawk = 8 }, tipi_negati = { 'harpoon' },\n"
             "tipo_missione = 'antinave', variante_battello = 'flight2', ospiti = 6, what_if = true,\n"
             "carriera = { situazione = 'crisi', avversario = 'veterano', difficolta = 4, artico = true },\n"
             "script = {}, varianti = { { probabilita = 1, unita = {\n"
             "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
             "  posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 0, velocita_kn = 4 },\n"
             "} } } }\n";
    }
    Missione m(f.string(), 1, false);
    REQUIRE(m.errori().empty());
    const LimitiMissione& l = m.limiti_carico();
    CHECK(l.deposito.at("mk48_adcap") == 4);
    CHECK(l.tipi_negati.count("harpoon"));
    CHECK(l.tipo_missione == "antinave");
    CHECK(l.variante_battello == "flight2");
    CHECK(l.ospiti == 6);
    CHECK(l.what_if);
    CHECK(l.anno == 1990);
    CHECK(l.dotazione.at("mk48") == 20);
    CHECK(l.tubi_originali.size() == 6);
    REQUIRE(m.blocco_carriera());
    CHECK(m.blocco_carriera()->situazione == "crisi");
    const RegoleCarriera r = carica_carriera(M688_DATI "/carriera.toml");
    const ContestoMissione c = contesto_carriera(m, r, "editor");
    CHECK(c.blocco_carriera);
    CHECK(c.situazione == Situazione::crisi);
    CHECK(c.avversario == 2);
    CHECK(c.difficolta == 4);
    CHECK(c.artico);
    CHECK(c.avversario_calcolato == 0);   // nessuna unita' ostile
    SalaSiluri s(catalogo(), "los_angeles", l);
    CHECK(s.locale().pozzi == 12);
    CHECK(s.posti_totali() == 19);
    std::filesystem::remove(f);
}

TEST_CASE("carico: l'ADCAP scelto in sala siluri parte con il suo modello (SCENE1A, Enhanced)" *
          doctest::skip(!std::filesystem::exists(std::string(M688_MISSIONI) + "/SCENE1A.lua"))) {
    Missione m(std::string(M688_MISSIONI) + "/SCENE1A.lua", 1);
    StazioniMissione st(m);
    m.siluri_enhanced(M688_DATI);
    SalaSiluri sala(catalogo(), "los_angeles", m.limiti_carico());
    sala.carico_iniziale();
    REQUIRE(sala.togli(Posto::tubo, "mk48", 1).fatto);
    REQUIRE(sala.metti(Posto::tubo, "mk48_adcap", 1).fatto);
    st.imposta_carico(m, sala.carico(), catalogo());
    REQUIRE(st.armi().con_carico());
    CHECK(m.tubi()[3] == "harpoon");   // il missile del tubo 7 nell'ultimo tubo del Flight I
    CHECK(m.tubi()[6].empty());
    RegistroOrdini ordini;
    const auto avanza = [&](double s) {
        for (int i = 0; i < static_cast<int>(std::lround(s * 9.0)); ++i) {
            for (const auto& o : ordini.consegna(m.tempo())) st.consegna(m, o);
            m.avanza(1.0 / 9.0);
            st.dopo_passo(m);
        }
    };
    avanza(1.0 / 9.0);
    ordini.ordina({m.tempo(), TipoOrdine::carica_tubo, 0, 0, 1});
    avanza(21.0);
    REQUIRE(st.armi().stato(1) == StatoTubo::armato);
    for (const auto& [id, u] : m.unita())   // nessuno a tiro
        if (id != m.giocatore()->id) m.trova(id)->pos.x += 400000.0;
    ordini.ordina(st.armi().lancio(m.tempo()));
    avanza(2.0);
    CHECK(st.armi().lanciata(1) == "mk48_adcap");
    const Siluro* sil = nullptr;
    for (const auto& [id, u] : m.unita())
        if (u.lanciatore == m.giocatore()->id && m.siluro(id)) sil = m.siluro(id);
    REQUIRE(sil);
    CHECK(sil->dati().classe == "mk48_adcap");
    // 22 della famiglia: 2 Mk 48 e l'ADCAP nei tubi, 19 in rastrelliera; la ricarica prende un Mk 48 (ADCAP non ce n'e')
    CHECK(st.armi().rastrelliera().at("mk48") == 18);
    CHECK(st.armi().variante(1) == "mk48");
    CHECK(m.errori().empty());
}

// I carichi per compito della schermata (06/11/2026): dati veri, dentro i locali, e senza toccare le partite

TEST_CASE("carichi per compito: ognuno sta nel suo locale, con armi del gioco, e con questo deposito sale intero") {
    const CatalogoCarico& cat = catalogo();
    const auto compiti = carica_compiti(M688_DATI "/carico.toml");
    for (const auto& [locale, tipi] : cat.carichi) {
        const LocaleSiluri& l = cat.locali.at(locale);
        for (const auto& [tipo, c] : tipi) {
            CAPTURE(locale);
            CAPTURE(tipo);
            CHECK(c.tubi.size() == static_cast<std::size_t>(l.tubi));
            double posti = static_cast<double>(c.tubi.size());
            int pozzi = 0;
            LimitiMissione lim;
            const auto arma_vera = [&](const std::string& id) {
                const SchedaArma* a = cat.arma(id);
                REQUIRE(a != nullptr);
                CHECK(a->nazione == l.nazione);
                CHECK_FALSE(a->solo_ia);
                CHECK_FALSE(a->what_if);
                CHECK_FALSE(a->nucleare);
                return a;
            };
            for (const auto& id : c.tubi) { arma_vera(id); ++lim.deposito[id]; }
            for (const auto& [id, n] : c.rastrelliera) { posti += n * arma_vera(id)->posti; lim.deposito[id] += n; }
            for (const auto& [id, n] : c.pozzi) { CHECK(arma_vera(id)->pozzo_verticale); pozzi += n; lim.deposito[id] += n; }
            CHECK(posti <= l.tubi + l.rastrelliera);
            CHECK(pozzi <= l.pozzi);
            // col deposito che le ha tutte sale proprio quello, salvo l'anno
            SalaSiluri s(cat, locale == "alfa" ? "alfa" : "los_angeles", [&] {
                LimitiMissione x = lim;
                if (locale == "los_angeles_flight2") x.variante_battello = "flight2";
                if (locale == "los_angeles_688i") x.variante_battello = "688i";
                return x;
            }());
            REQUIRE(s.locale().id == locale);
            s.carico_standard(tipo);
            int k = 0;
            for (std::size_t i = 0; i < s.carico().tipi.size(); ++i)
                if (s.carico().tipi[i] == "tubo") CHECK(s.carico().posizioni[i] == c.tubi[static_cast<std::size_t>(k++)]);
            CHECK(s.carico().rastrelliera == c.rastrelliera);
            CHECK(s.carico().pozzi == c.pozzi);
            // nome, nota e fonte per l'elenco
            REQUIRE(compiti.count(tipo) == 1);
            CHECK_FALSE(compiti.at(tipo).nome.empty());
            CHECK_FALSE(compiti.at(tipo).nota.empty());
            CHECK_FALSE(compiti.at(tipo).fonte.empty());
            CHECK_FALSE(compiti.at(tipo).confidenza.empty());
        }
    }
    CHECK(cat.carichi.at("los_angeles").size() == 9);
    CHECK(cat.carichi.at("alfa").size() == 7);
}

TEST_CASE("carichi per compito: la partenza e l'IA non cambiano, e le chiavi nuove non sono tipi di missione") {
    const CatalogoCarico& cat = catalogo();
    for (const auto& [locale, tipi] : cat.carichi) CHECK(cat.predefiniti.at(locale) == "antisom");
    // i carichi che partivano prima restano uguali (dossier 14 §5.1)
    const auto& la = cat.carichi.at("los_angeles");
    CHECK(la.at("antisom").rastrelliera == std::map<std::string, int>{{"mk48", 19}, {"harpoon", 2}});
    CHECK(la.at("minamento").rastrelliera == std::map<std::string, int>{{"mk48", 9}, {"mk67", 10}, {"harpoon", 2}});
    const auto& alfa = cat.carichi.at("alfa");
    CHECK(alfa.at("antisom").rastrelliera == std::map<std::string, int>{{"set65", 6}, {"type53_65k", 5}, {"mg74", 2}});
    CHECK(alfa.at("minamento").rastrelliera == std::map<std::string, int>{{"set65", 2}, {"pmr2", 6}, {"mdt", 10}});
    CHECK(alfa.count("scorta") == 0);   // oggi una missione «scorta» da' all'Alfa il carico antisom, anche all'IA
    // nessuna missione, ne' l'editor, dichiara un tipo nuovo: il carico dell'IA lo prende da li' (armi_ia.cpp)
    const std::set<std::string> nuovi = {"caccia", "saturazione", "costiero", "barriera", "agguato"};
    for (const char* t : {"antisom", "antinave", "minamento", "scorta"}) CHECK(nuovi.count(t) == 0);
    int file = 0;
    for (const auto& e : std::filesystem::recursive_directory_iterator(M688_MISSIONI)) {
        if (e.path().extension() != ".lua") continue;
        ++file;
        std::ifstream in(e.path());
        const std::string s((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        for (const auto& n : nuovi) {
            CAPTURE(e.path().string());
            CHECK(s.find("tipo_missione = \"" + n + "\"") == std::string::npos);
        }
    }
    CHECK(file > 100);
}

TEST_CASE("carichi per compito: il deposito e l'anno comandano") {
    // 25 Mk 48, 2 Harpoon, niente mine: la barriera diventa siluri, con le armi che ci sono
    LimitiMissione l;
    l.deposito = {{"mk48", 25}, {"harpoon", 2}, {"esca", 6}};
    SalaSiluri s(catalogo(), "los_angeles", l);
    s.carico_standard("barriera");
    CHECK(s.carico().rastrelliera.count("mk60") == 0);
    CHECK(s.posti_usati() <= s.posti_totali());
    // la caccia pura: al piu' 6 ADCAP nel 1989 (dalla famiglia mk48), e prima del 1988 solo Mk 48
    s.carico_standard("caccia");
    CHECK(s.carico().rastrelliera.at("mk48_adcap") == 2);
    int adcap = 0;
    for (const auto& p : s.carico().posizioni) adcap += p == "mk48_adcap";
    CHECK(adcap == 4);
    LimitiMissione prima = l;
    prima.anno = 1985;
    SalaSiluri v(catalogo(), "los_angeles", prima);
    v.carico_standard("caccia");
    CHECK(v.carico().rastrelliera.count("mk48_adcap") == 0);
    for (const auto& p : v.carico().posizioni) CHECK(p != "mk48_adcap");
}
