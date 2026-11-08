#include <doctest.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "grafica/font.hpp"
#include "grafica/modello.hpp"
#include "grafica/pagina_squadre.hpp"
#include "grafica/palette.hpp"
#include "grafica/spaccato.hpp"
#include "grafica/tela.hpp"
#include "interfaccia.hpp"
#include "missioni/danni_sottomarini.hpp"
#include "missioni/missione.hpp"
#include "missioni/stazioni_missione.hpp"
#include "nucleo/battello.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/comando.hpp"
#include "nucleo/ordini.hpp"
#include "nucleo/scafo.hpp"
#include "piattaforma/finestra.hpp"

using namespace m688;
namespace fs = std::filesystem;

// La pagina SQUADRE di F3 (grafica/pagina_squadre, docs/10 §10.19): il disegno da solo, poi dal vero, dal clic su un pulsante
// all'ordine consegnato. Il modello dell'isolamento ha le sue prove (prove_battello, prove_danni_sottomarini).

namespace {

bool ci_sono_dati() { return fs::exists(std::string(M688_DATI) + "/font/fosforo688_5x7.fnt") && fs::exists(std::string(M688_DATI) + "/modelli/alfa.mdl"); }

Font font(const char* nome) { return Font::carica(std::string(M688_DATI) + "/font/" + nome); }

constexpr std::uint8_t SEGNO = 201;   // il fondo delle prove: un colore che il disegno non usa
const Rettangolo PAGINA{4, 164, 632, 148};
constexpr int DESTRA = 636, Y_TASTI = 312;

Tela tela_segnata() {
    Tela t;
    t.rettangolo_pieno(0, 0, LARGHEZZA_TELA, ALTEZZA_TELA, SEGNO);
    return t;
}

// i pixel toccati fuori dal rettangolo
int fuori_da(const Tela& t, Rettangolo r) {
    int n = 0;
    Tela& v = const_cast<Tela&>(t);
    for (int y = 0; y < ALTEZZA_TELA; ++y)
        for (int x = 0; x < LARGHEZZA_TELA; ++x)
            if (v.riga(y)[x] != SEGNO && (x < r.x || x >= r.x + r.w || y < r.y || y >= r.y + r.h)) ++n;
    return n;
}

DatiPaginaSquadre dati_prova(bool alfa, PortaOtsek::Stato stato) {
    DatiPaginaSquadre d;
    d.alfa = alfa;
    for (int i = 0; i < 13; ++i) {
        RigaSquadre r;
        r.nome = "calcolatore del sonar";
        r.efficienza = i * 0.08;
        r.massimo = 0.9;
        r.squadre = i % 4 == 0;
        d.righe.push_back(r);
    }
    // il caso piu' largo che si vede davvero: oltre meta' d'acqua il fuoco si spegne
    for (int i = 0; i < (alfa ? 6 : 3); ++i) {
        PortaOtsek p;
        p.nome = alfa ? "III TSP" : "C3 MACCHINE";
        p.stato = stato;
        p.conto_s = 59;
        p.uomini_dentro = alfa ? 0 : 19;
        p.acqua = 0.49;
        p.fuoco = 0.8;
        p.ossigeno = 0.14;
        p.squadre = 3;
        d.porte.push_back(p);
    }
    d.squadre = 3;
    d.scelto = 0;
    return d;
}

std::string scena(const std::string& classe) {
    const auto f = fs::temp_directory_path() / ("m688_prova_squadre_" + classe + ".lua");
    std::ofstream(f) << "return { id = 'PROVA', teatro = 'prova', script = {}, varianti = { { probabilita = 1, unita = {\n"
                     << "{ id = 0, classe = '" << classe << "', categoria = 'sottomarino', flag = 9, giocatore = true,"
                     << "  posizione = { x = 0, y = 0 }, quota_m = 100, rotta = 0, velocita_kn = 5,"
                     << "  modello = { massima_kn = 30, virata_gradi_s = 1.44, accel_kn_s = 0.05, quota_m_s = 1, quota_max_m = 450, integrita = 160 } },"
                     << "\n}, timer = {} } } }\n";
    return f.string();
}

// il quadro danni vero, come in partita: si disegna, si clicca dove il pulsante e' stato disegnato, l'ordine arriva ai danni
struct Banco {
    std::map<std::string, ClasseNave> classi = carica_classi(std::string(M688_DATI) + "/classi.toml");
    Missione m;
    DanniSottomarini ds;
    StazioniMissione st;
    PostoDiComando posto;
    RegistroOrdini ordini;
    std::vector<Messaggio> equipaggio;
    InterfacciaMissione ui;
    Font f = font("originale.fnt");
    VistaPeriscopio v;
    Tela t;

    explicit Banco(const std::string& classe) : m(scena(classe), 1, false), ds(classi, M688_DATI, 1), st(m, 1u), ui(m, st, posto, ordini, equipaggio, classi, 3) {
        m.impatti_dell_originale(false);
        ds.prepara(m, 0);
        st.usa_danni(&ds);
        auto pal = Palette::carica(std::string(M688_DATI) + "/palette/giorno.pal");
        Spaccato::imposta_palette(pal);
        const auto modello = Modello::carica(std::string(M688_DATI) + "/modelli/" + classe + ".mdl");
        const auto scafo = DatiScafo::carica(std::string(M688_DATI) + "/scafi/" + classe + ".toml");
        ui.imposta_spaccato(std::make_unique<Spaccato>(classe, modello, scafo));
    }
    const BattelloDanni& b() const { return *ds.sottomarino(0)->danni; }
    void passo() {
        Comandi c;
        ui.comandi(c, v, 1.0 / 9.0);
        for (const Ordine& o : ordini.consegna(m.tempo())) ds.consegna(m, o);
        m.avanza(1.0 / 9.0);
        ds.avanza(m, 1.0 / 9.0);
    }
    std::set<std::string> pulsanti() {
        ui.disegna(t, f, v);
        std::set<std::string> s;
        for (const auto& a : ui.aree_vista())
            if (a.comando.rfind("pulsante_", 0) == 0) s.insert(a.comando);
        return s;
    }
    // come il ciclo di main.cpp: dal punto cliccato all'area, dall'area al comando
    bool clic(int cifra) {
        ui.disegna(t, f, v);
        const std::string nome = "pulsante_" + std::to_string(cifra);
        for (const auto& a : ui.aree_vista()) {
            if (a.comando != nome) continue;
            const int x = a.x + a.w / 2, y = a.y + a.h / 2;
            for (const auto& q : ui.aree_vista())
                if (q.contiene(x, y)) {
                    Comandi c;
                    c.cifre.push_back(std::stoi(q.comando.substr(9)));   // applica() di piattaforma/finestra.cpp, che qui non si collega
                    ui.comandi(c, v, 1.0 / 9.0);
                    for (const Ordine& o : ordini.consegna(m.tempo())) ds.consegna(m, o);
                    m.avanza(1.0 / 9.0);
                    ds.avanza(m, 1.0 / 9.0);
                    return q.comando == nome;
                }
        }
        return false;
    }
    bool sentito(int codice) const {
        const auto& v_ = ds.messaggi();
        return std::any_of(v_.begin(), v_.end(), [&](const Messaggio& x) { return x.codice == codice; });
    }
};

}  // namespace

TEST_CASE("pagina squadre: il disegno resta nel suo rettangolo, 13 sistemi e 6 compartimenti ci stanno" * doctest::skip(!ci_sono_dati())) {
    const Font piccolo = font("fosforo688_5x7.fnt");
    using S = PortaOtsek::Stato;
    for (const bool alfa : {false, true})
        for (const S s : {S::aperto, S::in_chiusura, S::chiuso, S::comando}) {
            Tela t = tela_segnata();
            const auto d = dati_prova(alfa, s);
            disegna_pagina_squadre(t, piccolo, PAGINA, d);
            CHECK(fuori_da(t, PAGINA) == 0);
            // l'ultima riga di sistemi e l'ultimo compartimento sopra il conto delle squadre
            CHECK(PAGINA.y + 14 + 12 * 10 + 8 <= PAGINA.y + PAGINA.h);
            const int passo = std::min(24, (PAGINA.h - 28) / static_cast<int>(d.porte.size()));
            CHECK(14 + (static_cast<int>(d.porte.size()) - 1) * passo + 9 + 8 <= PAGINA.h - 11);
        }
    SUBCASE("le voci di un compartimento non finiscono sotto chi e' dentro") {
        const int largo = PAGINA.w - 408 - 9;
        const int voci = piccolo.larghezza("ACQUA 49  FUOCO 80  O2 14", 1), dentro = piccolo.larghezza("19 DENTRO", 1);
        CHECK(voci + dentro + 2 <= largo);
    }
    SUBCASE("il cursore e il compartimento fuori misura, e i dati vuoti, non fanno danni") {
        Tela t = tela_segnata();
        auto d = dati_prova(false, S::aperto);
        d.cursore = 99;
        d.scelto = 99;
        disegna_pagina_squadre(t, piccolo, PAGINA, d);
        d.cursore = -1;
        d.scelto = -1;
        disegna_pagina_squadre(t, piccolo, PAGINA, d);
        disegna_pagina_squadre(t, piccolo, PAGINA, DatiPaginaSquadre{});
        CHECK(fuori_da(t, PAGINA) == 0);
    }
}

TEST_CASE("pagina squadre: i pulsanti non si coprono e non coprono la scritta del compartimento" * doctest::skip(!ci_sono_dati())) {
    const Font piccolo = font("fosforo688_5x7.fnt"), originale = font("originale.fnt");
    using S = PortaOtsek::Stato;
    struct Caso { bool alfa; S stato; const char* scritta; };
    // la scritta col «piu' colpito» viene solo a Tab tornato sull'automatico; l'Alfa non ha il conto dello sgombero
    const std::vector<Caso> casi = {
        {false, S::aperto, "COMPARTIMENTO 3 (IL PIU' COLPITO)   TAB SCEGLIE"},
        {false, S::in_chiusura, "COMPARTIMENTO 3 (IL PIU' COLPITO)   TAB SCEGLIE"},
        {false, S::chiuso, "COMPARTIMENTO 3 (IL PIU' COLPITO)   TAB SCEGLIE"},
        {false, S::comando, "COMPARTIMENTO 1 (IL PIU' COLPITO)   TAB SCEGLIE"},
        {true, S::aperto, "OTSEK III (IL PIU' COLPITO)   TAB SCEGLIE"},
        {true, S::chiuso, "OTSEK III (IL PIU' COLPITO)   TAB SCEGLIE"},
        {true, S::comando, "OTSEK III (IL PIU' COLPITO)   TAB SCEGLIE"},
    };
    for (const Caso& k : casi)
        for (const bool aperta : {false, true}) {
            CAPTURE(k.scritta);
            CAPTURE(aperta);
            CAPTURE(static_cast<int>(k.stato));
            Tela t = tela_segnata();
            const auto d = dati_prova(k.alfa, k.stato);
            std::vector<AreaPulsante> aree;
            const int primo = disegna_tasti_squadre(t, piccolo, DESTRA, Y_TASTI, d, aperta, &aree);
            CHECK(fuori_da(t, {primo, Y_TASTI, DESTRA - primo, 12}) == 0);
            CHECK(8 + originale.larghezza(k.scritta, 1) < primo);
            std::set<int> cifre;
            for (std::size_t i = 0; i < aree.size(); ++i) {
                const Rettangolo& r = aree[i].first;
                cifre.insert(aree[i].second);
                CHECK(r.y == Y_TASTI);
                CHECK(r.h == 12);
                CHECK(r.x + r.w <= DESTRA);
                for (std::size_t j = i + 1; j < aree.size(); ++j) {
                    const Rettangolo& q = aree[j].first;
                    CHECK((r.x + r.w <= q.x || q.x + q.w <= r.x));
                }
            }
            CHECK(cifre == (aperta ? std::set<int>{1, 2, 3, 4, 5} : std::set<int>{1, 5}));
        }
}

TEST_CASE("pagina squadre: il pulsante 1 dice che cosa fa adesso" * doctest::skip(!ci_sono_dati())) {
    const Font piccolo = font("fosforo688_5x7.fnt");
    using S = PortaOtsek::Stato;
    const auto larghezza = [&](bool alfa, S s) {
        Tela t = tela_segnata();
        std::vector<AreaPulsante> aree;
        disegna_tasti_squadre(t, piccolo, DESTRA, Y_TASTI, dati_prova(alfa, s), false, &aree);
        for (const auto& [r, n] : aree)
            if (n == 1) return r.w;
        return 0;
    };
    constexpr int MARGINE = 18;   // la cifra e i bordi
    CHECK(larghezza(false, S::aperto) == piccolo.larghezza("ISOLA", 1) + MARGINE);
    CHECK(larghezza(false, S::in_chiusura) == piccolo.larghezza("CHIUDI SUBITO", 1) + MARGINE);
    CHECK(larghezza(false, S::chiuso) == piccolo.larghezza("RIAPRI", 1) + MARGINE);
    CHECK(larghezza(false, S::comando) == piccolo.larghezza("NON SI ISOLA", 1) + MARGINE);
    CHECK(larghezza(true, S::aperto) == piccolo.larghezza("GERMETIZ.", 1) + MARGINE);
    CHECK(larghezza(true, S::chiuso) == piccolo.larghezza("OTKRYT", 1) + MARGINE);
    CHECK(larghezza(true, S::comando) == piccolo.larghezza("POST", 1) + MARGINE);
}

TEST_CASE("pagina squadre: sullo spaccato solo i compartimenti chiusi o in chiusura" * doctest::skip(!ci_sono_dati())) {
    const Font f = font("originale.fnt");
    using S = PortaOtsek::Stato;
    auto pal = Palette::carica(std::string(M688_DATI) + "/palette/giorno.pal");
    Spaccato::imposta_palette(pal);
    const auto modello = Modello::carica(std::string(M688_DATI) + "/modelli/los_angeles.mdl");
    const auto scafo = DatiScafo::carica(std::string(M688_DATI) + "/scafi/los_angeles.toml");
    const Spaccato sp("los_angeles", modello, scafo);
    const Rettangolo intero{4, 20, 632, 108};
    Tela t = tela_segnata();
    auto d = dati_prova(false, S::aperto);
    disegna_porte_spaccato(t, f, sp, intero, d);
    CHECK(fuori_da(t, {0, 0, 0, 0}) == 0);   // niente da nessuna parte
    d = dati_prova(false, S::comando);
    disegna_porte_spaccato(t, f, sp, intero, d);
    CHECK(fuori_da(t, {0, 0, 0, 0}) == 0);
    for (const S s : {S::in_chiusura, S::chiuso}) {
        Tela u = tela_segnata();
        d = dati_prova(false, s);
        disegna_porte_spaccato(u, f, sp, intero, d);
        CHECK(fuori_da(u, {0, 0, 0, 0}) > 0);
        CHECK(fuori_da(u, intero) == 0);
    }
}

TEST_CASE("pagina squadre, dal vero: 688, dal clic all'ordine" * doctest::skip(!ci_sono_dati())) {
    Banco k("los_angeles");
    k.ds.incendio(k.m, 0, 2, 0.8);
    k.passo();
    CHECK(k.pulsanti() == std::set<std::string>{"pulsante_1", "pulsante_5"});

    SUBCASE("la pagina e le priorita'") {
        CHECK_FALSE(k.clic(2));   // a pagina chiusa non c'e'
        REQUIRE(k.clic(5));
        CHECK(k.pulsanti() == std::set<std::string>{"pulsante_1", "pulsante_2", "pulsante_3", "pulsante_4", "pulsante_5"});
        const std::vector<Sistema> prima = k.b().ordine_riparazioni();
        REQUIRE(k.clic(3));
        REQUIRE(k.clic(3));
        REQUIRE(k.clic(2));   // il cursore sulla seconda riga
        REQUIRE(k.clic(4));
        CHECK(k.b().ordine_riparazioni().front() == prima[1]);
        CHECK(k.b().ordine_riparazioni()[1] == prima[0]);
        REQUIRE(k.clic(5));
        CHECK(k.pulsanti() == std::set<std::string>{"pulsante_1", "pulsante_5"});
    }
    SUBCASE("isola, chiude subito, riapre: sempre sul compartimento in fiamme") {
        const auto& c = (*k.ds.compartimenti(0))[2];
        REQUIRE(k.clic(1));
        CHECK(c.porte_s >= 0.0);
        CHECK(k.b().caduti() == 0);
        CHECK_FALSE(c.isolato);
        for (int i = 0; i < 9 * 20; ++i) k.passo();
        REQUIRE(k.clic(1));
        CHECK(c.isolato);
        CHECK(c.dentro > 0);
        CHECK(c.dentro < 19);
        REQUIRE(k.clic(1));
        CHECK_FALSE(c.senza_squadre());
        CHECK(c.dentro == 0);
    }
    SUBCASE("lo sgombero finisce da solo, e le porte si riaprono con lo stesso pulsante") {
        const auto& c = (*k.ds.compartimenti(0))[2];
        REQUIRE(k.clic(1));
        for (int i = 0; i < 9 * 61; ++i) k.passo();
        CHECK(c.isolato);
        CHECK(c.dentro == 0);
        REQUIRE(k.clic(1));
        CHECK_FALSE(c.senza_squadre());
    }
    SUBCASE("il posto di comando rifiuta l'ordine") {
        Comandi tab;
        tab.tab = 1;
        k.ui.comandi(tab, k.v, 1.0 / 9.0);   // Tab: dal piu' colpito al primo, C1
        REQUIRE(k.clic(1));
        CHECK(k.sentito(418));
        CHECK_FALSE((*k.ds.compartimenti(0))[0].senza_squadre());
    }
}

TEST_CASE("pagina squadre, dal vero: l'Alfa chiude subito e senza nessuno dentro" * doctest::skip(!ci_sono_dati())) {
    Banco k("alfa");
    k.ds.incendio(k.m, 0, 4, 0.8);
    k.passo();
    const auto& c = (*k.ds.compartimenti(0))[4];
    REQUIRE(k.clic(1));
    CHECK(c.isolato);
    CHECK(c.porte_s < 0.0);
    CHECK(c.dentro == 0);
    CHECK(k.b().limiti().velocita > 0.5);
    REQUIRE(k.clic(1));
    CHECK_FALSE(c.senza_squadre());
}
