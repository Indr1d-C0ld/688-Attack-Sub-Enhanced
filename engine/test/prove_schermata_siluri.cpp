#include <doctest.h>

#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include "grafica/font.hpp"
#include "grafica/tela.hpp"
#include "nucleo/carico.hpp"
#include "nucleo/compiti_carico.hpp"
#include "piattaforma/finestra.hpp"
#include "schermata_siluri.hpp"

using namespace m688;

// La Sala siluri: l'elenco dei carichi per compito, da tasti e da clic (06/11/2026). Il carico consigliato e' solo evidenziato.

namespace {

constexpr int X_ELENCO = 200;   // dentro la pianta

const CatalogoCarico& catalogo() {
    static const CatalogoCarico c = carica_carico(M688_DATI "/carico.toml");
    return c;
}
const std::map<std::string, CompitoCarico>& compiti() {
    static const auto c = carica_compiti(M688_DATI "/carico.toml");
    return c;
}

LimitiMissione tutto_al_deposito() {
    LimitiMissione l;
    l.deposito = {{"mk48", 25}, {"harpoon", 10}, {"tomahawk", 12}, {"mina", 12}, {"mk60", 12}, {"tlam_c", 8}, {"esca", 6}};
    return l;
}

Comandi tasto(char l) {
    Comandi c;
    c.lettere.push_back(l);
    return c;
}
Comandi invio() {
    Comandi c;
    c.invio = true;
    return c;
}
Comandi esc() {
    Comandi c;
    c.esc = true;
    return c;
}
Comandi clic(int x, int y) {
    Comandi c;
    c.clic.emplace_back(x, y);
    return c;
}
Comandi freccia(int su_giu) {
    Comandi c;
    c.su_giu = su_giu;
    return c;
}

int riga_di(const SchermataSiluri& s, const std::string& tipo) {
    const auto& v = s.voci();
    const auto it = std::find_if(v.begin(), v.end(), [&](const auto& x) { return x.tipo == tipo; });
    REQUIRE(it != v.end());
    return static_cast<int>(it - v.begin());
}

}  // namespace

TEST_CASE("sala siluri: l'elenco si apre con C e col clic, e il consigliato non si applica da solo") {
    LimitiMissione l = tutto_al_deposito();
    l.tipo_missione = "minamento";
    SalaSiluri sala(catalogo(), "los_angeles", l);
    sala.carico_standard("antisom");
    const CaricoPartenza prima = sala.carico();
    SchermataSiluri s(sala, "Los Angeles", 1989, l.tipo_missione, compiti());
    CHECK(s.consigliato() == "minamento");
    CHECK(s.tipi()[static_cast<std::size_t>(s.tipo_scelto())] == "minamento");   // il cursore e' sul consigliato...
    CHECK(sala.carico() == prima);                                               // ...ma a bordo resta quello di prima
    CHECK_FALSE(s.elenco_aperto());
    s.comandi(tasto('C'), 0.1);
    CHECK(s.elenco_aperto());
    CHECK(sala.carico() == prima);
    s.comandi(esc(), 0.1);   // Esc chiude e non cambia niente, e non torna al briefing
    CHECK_FALSE(s.elenco_aperto());
    CHECK(sala.carico() == prima);
    s.comandi(clic(8 + 126 + 20, 448 + 10), 0.1);   // il pulsante del carico apre, non applica
    CHECK(s.elenco_aperto());
    CHECK(sala.carico() == prima);
    s.comandi(clic(10, 10), 0.1);   // un clic fuori chiude senza cambiare
    CHECK_FALSE(s.elenco_aperto());
    CHECK(sala.carico() == prima);
}

TEST_CASE("sala siluri: dall'elenco si applica coi tasti e col clic, e poi si modifica a mano") {
    SalaSiluri sala(catalogo(), "los_angeles", tutto_al_deposito());
    SchermataSiluri s(sala, "Los Angeles", 1989, "antisom", compiti());
    REQUIRE(s.voci().size() == 9);
    // il clic sulla riga applica proprio quella
    s.comandi(tasto('C'), 0.1);
    const int r = riga_di(s, "barriera");
    s.comandi(clic(X_ELENCO, s.y_riga(r) + 4), 0.1);
    CHECK_FALSE(s.elenco_aperto());
    CHECK(sala.carico() == s.voci()[static_cast<std::size_t>(r)].carico);
    CHECK(sala.carico().rastrelliera.at("mk60") == 10);
    // frecce e Invio: la riga sotto
    s.comandi(tasto('C'), 0.1);
    s.comandi(Comandi{}, 0.1);
    s.comandi(freccia(-1), 0.1);
    CHECK(s.tipo_scelto() == r + 1);
    s.comandi(Comandi{}, 0.1);
    s.comandi(invio(), 0.1);
    CHECK_FALSE(s.elenco_aperto());
    CHECK(sala.carico() == s.voci()[static_cast<std::size_t>(r + 1)].carico);
    // poi a mano: svuota, e l'arma scelta nel tubo 1. Invio fuori dall'elenco parte come prima
    const double usati = sala.posti_usati();
    s.comandi(tasto('X'), 0.1);
    CHECK(sala.posti_usati() < usati);
    s.scegli("mk48");
    Comandi uno;
    uno.cifre.push_back(1);
    s.comandi(uno, 0.1);
    CHECK(sala.carico().posizioni[0] == "mk48");
    for (const auto& v : s.voci()) CHECK_FALSE(sala.carico() == v.carico);
    // Tab e S restano
    const int t0 = s.tipo_scelto();
    Comandi tab;
    tab.tab = 1;
    s.comandi(tab, 0.1);
    CHECK(s.tipo_scelto() == (t0 + 1) % 9);
    s.comandi(tasto('S'), 0.1);
    CHECK(sala.carico() == s.voci()[static_cast<std::size_t>(s.tipo_scelto())].carico);
    s.comandi(tasto('X'), 0.1);
    CHECK(s.comandi(invio(), 0.1) == SchermataSiluri::Uscita::nessuna);   // con gli avvisi prima chiede...
    CHECK(s.comandi(invio(), 0.1) == SchermataSiluri::Uscita::in_mare);   // ...poi parte
}

TEST_CASE("sala siluri: l'elenco dice che cosa sale davvero, e lo scarto che pesa in carriera") {
    // la missione tipica: solo Mk 48, Harpoon ed esche
    LimitiMissione l;
    l.deposito = {{"mk48", 23}, {"harpoon", 2}, {"esca", 6}};
    l.tipo_missione = "antisom";
    SalaSiluri sala(catalogo(), "los_angeles", l);
    sala.carico_iniziale();
    const CaricoPartenza prima = sala.carico();
    SchermataSiluri s(sala, "Los Angeles", 1989, l.tipo_missione, compiti());
    CHECK(sala.carico() == prima);   // calcolare l'elenco non tocca il carico
    const auto voce = [&](const char* t) { return s.voci()[static_cast<std::size_t>(riga_di(s, t))]; };
    CHECK_FALSE(voce("antisom").adattato);
    CHECK(voce("antisom").scarto == doctest::Approx(0.0));
    CHECK(voce("barriera").adattato);   // niente Mk 60 al deposito
    CHECK(voce("barriera").carico.rastrelliera.count("mk60") == 0);
    CHECK(voce("barriera").mancano.find("12 MK60") != std::string::npos);
    CHECK(voce("antisom").mancano.empty());
    CHECK_FALSE(voce("antisom").doppione);
    CHECK(voce("minamento").doppione == (voce("minamento").carico == voce("barriera").carico));   // senza mine sono lo stesso carico
    CHECK(voce("barriera").scarto == doctest::Approx(scarto_dal_standard(voce("barriera").carico, catalogo(), "los_angeles", "antisom")));
    // con 21 Mk 48, come tante missioni, nemmeno l'antisom sale intero: l'elenco lo dice, ed e' lo scarto che la carriera conta gia'
    LimitiMissione poche = l;
    poche.deposito["mk48"] = 21;
    SalaSiluri sala3(catalogo(), "los_angeles", poche);
    SchermataSiluri s3(sala3, "Los Angeles", 1989, poche.tipo_missione, compiti());
    CHECK(s3.voci()[static_cast<std::size_t>(riga_di(s3, "antisom"))].adattato);
    CHECK(s3.voci()[static_cast<std::size_t>(riga_di(s3, "antisom"))].scarto == doctest::Approx(0.04));
    // senza tipo dichiarato la carriera non misura lo scarto, e l'elenco non lo mostra
    LimitiMissione senza = l;
    senza.tipo_missione = "";
    SalaSiluri sala2(catalogo(), "los_angeles", senza);
    SchermataSiluri s2(sala2, "Los Angeles", 1989, senza.tipo_missione, compiti());
    CHECK(s2.consigliato() == "antisom");
    for (const auto& v : s2.voci()) CHECK(v.scarto == doctest::Approx(0.0));
    // l'ordine e' quello dei compiti, non alfabetico; l'Alfa ha i suoi sette, col deposito della famiglia
    CHECK(s.voci().front().tipo == "antisom");
    LimitiMissione a;
    a.deposito = {{"type53", 18}, {"esca", 4}, {"esca_mobile", 2}};
    a.tipo_missione = "antisom";
    SalaSiluri alfa(catalogo(), "alfa", a);
    SchermataSiluri sa(alfa, "Alfa", 1989, a.tipo_missione, compiti());
    REQUIRE(sa.voci().size() == 7);
    CHECK(sa.voci()[static_cast<std::size_t>(riga_di(sa, "agguato"))].carico.rastrelliera.at("saet60m") == 8);
    CHECK(sa.voci()[static_cast<std::size_t>(riga_di(sa, "barriera"))].adattato);   // niente PMR-2 al deposito
}

TEST_CASE("sala siluri: il disegno dell'elenco evidenzia il consigliato e non tocca il carico") {
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    const auto giallo = [&](const std::string& consigliato) {   // i gialli sulla riga del minamento
        SalaSiluri sala(catalogo(), "los_angeles", tutto_al_deposito());
        SchermataSiluri s(sala, "Los Angeles", 1989, consigliato, compiti());
        s.apri_elenco();
        const CaricoPartenza prima = sala.carico();
        Tela t;
        s.disegna(t, f);
        CHECK(sala.carico() == prima);
        const int y = s.y_riga(riga_di(s, "minamento"));
        int n = 0;
        for (int yy = y; yy < y + 10; ++yy)
            for (int x = 170; x < 620; ++x) n += t.at(x, yy) == colori::GIALLO;
        return n;
    };
    CHECK(giallo("minamento") > 0);
    CHECK(giallo("antisom") == 0);
}
