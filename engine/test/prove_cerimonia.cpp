// Prove della grafica attorno alla missione (M4, docs/12): decorazioni da carriera.toml e decorazioni_grafica.toml,
// barretta, medaglie, ritratto, cornici e timbri, minicartoline e icone.
#include <doctest.h>

#include <chrono>
#include <cmath>
#include <filesystem>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "albo_onore.hpp"
#include "../app/figurina_evento.hpp"
#include "../app/scelta_fotografie.hpp"
#include "grafica/cartolina.hpp"
#include "grafica/cerimonia.hpp"
#include "grafica/cornici.hpp"
#include "grafica/decorazioni.hpp"
#include "grafica/giornale.hpp"
#include "grafica/medaglioni.hpp"
#include "grafica/ritratto.hpp"
#include "nucleo/carriera.hpp"
#include "risorse.hpp"

using namespace m688;
namespace fs = std::filesystem;

namespace {

bool ci_sono_dati() { return fs::exists(M688_DATI "/carriera.toml") && fs::exists(M688_DATI "/decorazioni_grafica.toml"); }

const TavolaDecorazioni& tavola() {
    static const RegoleCarriera regole = carica_carriera(M688_DATI "/carriera.toml");
    static const TavolaDecorazioni t = TavolaDecorazioni::carica(regole, M688_DATI "/decorazioni_grafica.toml");
    return t;
}

int conta(const Tela& t, Rettangolo r, int da, int a) {
    int n = 0;
    for (int y = r.y; y < r.y + r.h; ++y)
        for (int x = r.x; x < r.x + r.w; ++x) n += t.at(x, y) >= da && t.at(x, y) <= a;
    return n;
}

bool uguali(const Tela& a, const Tela& b, Rettangolo r) {
    for (int y = r.y; y < r.y + r.h; ++y)
        for (int x = r.x; x < r.x + r.w; ++x)
            if (a.at(x, y) != b.at(x, y)) return false;
    return true;
}

}  // namespace

TEST_CASE("decorazioni: una sola fonte, gli stessi id in carriera.toml e in decorazioni_grafica.toml" * doctest::skip(!ci_sono_dati())) {
    const auto& t = tavola();
    CHECK(t.solo_in_carriera().empty());
    CHECK(t.solo_in_grafica().empty());
    for (const auto& id : t.solo_in_carriera()) MESSAGE("manca in decorazioni_grafica.toml: " << id);
    for (const auto& id : t.solo_in_grafica()) MESSAGE("manca in carriera.toml: " << id);
    REQUIRE(t.tutte().size() >= 40);
    std::set<int> colori;
    for (const auto& d : t.tutte()) {
        CAPTURE(d.id);
        double somma = 0;
        for (const auto& s : d.nastro) {
            somma += s.larghezza;
            colori.insert(s.colore);
            CHECK(s.colore >= cerimonia::NASTRO);
            CHECK(s.colore < cerimonia::NASTRO + cerimonia::N_NASTRO);
        }
        if (!d.nastro.empty()) CHECK(somma == doctest::Approx(36.0));
        // i nastrini mancano solo a distintivi e attestati
        if (d.nastro.empty()) CHECK((d.forma == FormaMedaglia::distintivo || d.forma == FormaMedaglia::attestato));
        if (d.a_vite) CHECK(d.marina == Marina::vmf);
    }
    CHECK(colori.size() <= static_cast<std::size_t>(cerimonia::N_NASTRO));
    // ordine di precedenza per marina
    for (std::size_t i = 1; i < t.tutte().size(); ++i)
        if (t.tutte()[i].marina == t.tutte()[i - 1].marina) CHECK(t.tutte()[i - 1].precedenza <= t.tutte()[i].precedenza);
    CHECK(t.decorazione("navy_cross")->forma == FormaMedaglia::croce_patente);
    CHECK(t.decorazione("medal_of_honor")->stelle);
    CHECK(t.decorazione("presidential_unit_citation")->orizzontale);
    CHECK(t.decorazione("navy_e_ribbon")->lettera == 'E');
    CHECK(t.decorazione("nessuna") == nullptr);
}

TEST_CASE("decorazioni: barretta in ordine di precedenza, fila incompleta in alto, stelline e V" * doctest::skip(!ci_sono_dati())) {
    const auto& t = tavola();
    Tela tela;
    tela.riempi(0);
    const std::vector<VoceBarretta> v = {{"legion_of_merit", "", 1}, {"meritorious_service", "", 2}, {"navy_commendation", "v", 1},
                                         {"navy_achievement", "", 1}, {"dolphins", "", 1}};   // i distintivi si saltano
    const auto r = t.disegna_barretta(tela, 320, 200, v, 3, 35, 10);
    CHECK(r.w == 105);   // 4 nastrini: una fila da 3 e la fila in alto da 1
    CHECK(r.h == 20);
    CHECK(r.y == 180);
    // in alto, centrata, la piu' alta (Legion of Merit: cremisi con i bordi bianchi)
    CHECK(tela.at(320, 185) != 0);
    CHECK(tela.at(320 - 35, 185) == 0);
    const int cremisi = tela.at(320, 185);
    CHECK(tela.at(320, 195) != cremisi);   // nella seconda fila, al centro, la Navy Commendation (verde)
    // dispositivi: una stellina d'oro (secondo conferimento) e la V di bronzo
    CHECK(conta(tela, {320 - 53, 190, 35, 10}, cerimonia::ORO + 6, cerimonia::ORO + 6) > 0);
    CHECK(conta(tela, {320 - 18, 190, 35, 10}, cerimonia::BRONZO + 4, cerimonia::BRONZO + 4) > 0);
    CHECK(misura_barretta(0, 3, 10, 4).w == 0);
}

TEST_CASE("decorazioni: ogni medaglia, nastrino o attestato si disegna nel suo riquadro" * doctest::skip(!ci_sono_dati())) {
    const auto& t = tavola();
    for (const auto& d : t.tutte()) {
        CAPTURE(d.id);
        Tela tela;
        tela.riempi(0);
        const Rettangolo r{100, 100, 120, 180};
        t.disegna_medaglia(tela, r, d.id);
        CHECK(conta(tela, r, 1, 255) > 300);
        CHECK(conta(tela, {0, 0, 640, 100}, 1, 255) == 0);   // niente fuori dal riquadro
        CHECK(conta(tela, {0, 280, 640, 200}, 1, 255) == 0);
    }
}

TEST_CASE("ritratto: deterministico, diverso col volto, uniformi, distintivi e tempo" * doctest::skip(!ci_sono_dati())) {
    const auto& tv = tavola();
    ParametriRitratto p;
    p.grado = 5;
    p.comando = true;
    p.berretto = true;
    p.nastrini = {{"legion_of_merit", "", 1}, {"meritorious_service", "", 2}};
    const Rettangolo r{10, 10, 236, 310};
    Tela a, b;
    const auto t0 = std::chrono::steady_clock::now();
    disegna_ritratto(a, r, p, &tv);
    const double ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count();
    MESSAGE("ritratto 236x310: " << ms << " ms");
    CHECK(ms < 1500.0);   // di norma 0,35 s; largo per le macchine lente
    disegna_ritratto(b, r, p, &tv);
    CHECK(uguali(a, b, r));
    // calotta bianca del berretto USN, pelle, dolphins e barretta
    CHECK(conta(a, {r.x, r.y, r.w, r.h / 3}, cerimonia::BIANCO, cerimonia::BIANCO + cerimonia::N_BIANCO - 1) > 200);
    CHECK(conta(a, r, cerimonia::PELLE, cerimonia::PELLE + cerimonia::N_PELLE - 1) > 3000);
    CHECK(conta(a, r, cerimonia::ORO, cerimonia::ORO + cerimonia::N_ORO - 1) > 80);
    CHECK(conta(a, r, cerimonia::NASTRO, cerimonia::NASTRO + cerimonia::N_NASTRO - 1) > 40);
    // un altro volto cambia la testa ma non l'uniforme
    p.volto.seme = 99;
    disegna_ritratto(b, r, p, &tv);
    CHECK_FALSE(uguali(a, b, {r.x, r.y, r.w, r.h / 2}));
    // VMF: tuzurka nera, controspalline d'oro, niente calotta bianca
    ParametriRitratto v = p;
    v.marina = Marina::vmf;
    v.grado = 6;
    v.nastrini = {{"stella_rossa", "", 1}};
    Tela c;
    disegna_ritratto(c, r, v, &tv);
    CHECK(conta(c, {r.x, r.y, r.w, r.h / 3}, cerimonia::BIANCO, cerimonia::BIANCO + cerimonia::N_BIANCO - 1) < 40);
    CHECK(conta(c, r, cerimonia::NERO, cerimonia::NERO + cerimonia::N_NERO - 1) > 5000);
    CHECK(conta(c, {r.x, r.y + r.h / 2, r.w, r.h / 2}, cerimonia::ORO, cerimonia::ORO + cerimonia::N_ORO - 1) > 300);
    // l'insegna del grado: tre galloni medi del capitano di fregata contro quattro del capitano di vascello
    Tela g5, g6;
    disegna_insegna_grado(g5, {0, 0, 176, 100}, Marina::usn, 5);
    disegna_insegna_grado(g6, {0, 0, 176, 100}, Marina::usn, 6);
    CHECK(conta(g6, {0, 0, 176, 100}, cerimonia::ORO, cerimonia::ORO + 7) > conta(g5, {0, 0, 176, 100}, cerimonia::ORO, cerimonia::ORO + 7));
}

TEST_CASE("cornici e timbri: nel modo vista solo interfaccia e grigi") {
    const auto font = Font::carica(M688_DATI "/font/originale.fnt");
    Tela t;
    t.riempi(0);
    cornice(t, {10, 10, 200, 120}, StileCornice::console, 6, true);
    cornice(t, {220, 10, 200, 120}, StileCornice::oro, 6, true);
    intestazione(t, font, {0, 140, 640, 38}, "DEBRIEFING", "MISSIONE", Marina::usn, true);
    timbro(t, font, 320, 300, "CLASSIFIED", -10, 2, 3, true);
    pulsante(t, font, {10, 400, 100, 22}, "CONTINUA", true);
    std::set<int> fuori;
    for (int y = 0; y < 480; ++y)
        for (int x = 0; x < 640; ++x) {
            const int c = t.at(x, y);
            if (!(c <= 15 || (c >= 80 && c <= 95) || (c >= 144 && c <= 158))) fuori.insert(c);
        }
    CHECK(fuori.empty());
    // il timbro ruotato e' consumato: inchiostro, ma non pieno
    const int rosso = conta(t, {220, 260, 200, 80}, 4, 4) + conta(t, {220, 260, 200, 80}, 12, 12);
    CHECK(rosso > 150);
    CHECK(rosso < 200 * 80 / 2);
}

TEST_CASE("cartolina: la nave dal suggerimento del registro, le icone dai nomi" * doctest::skip(!fs::exists(M688_DATI "/classi.toml"))) {
    const auto ris = carica_risorse(M688_DATI);
    Periscopio per(ris.font, ris.classi, ris.sprite);
    per.usa_modelli(ris.modelli, rampa_grigi(ris.giorno));
    Mondo m;
    Nave n;
    n.id = 1;
    n.classe = "krivak2";
    n.rotta = 60;
    m.aggiungi(n);
    const auto q = dal_registro(m.navi()[0], nullptr, 3500, 0, 90, 0);
    CHECK(distanza(q.occhio, n.pos) == doctest::Approx(3500).epsilon(0.01));
    CHECK(q.alto_ingrandimento);
    Tela t;
    t.riempi(0);
    const Rettangolo r{50, 50, 128, 96};
    disegna_cartolina(t, r, per, m, q);
    // i grigi dello sprite al centro del riquadro, il cielo e il mare attorno
    CHECK(conta(t, {r.x + 32, r.y, 64, r.h}, 144, 158) > 60);
    CHECK(conta(t, {0, 0, 640, 50}, 1, 255) == 0);
    // senza la nave i grigi spariscono
    Mondo vuoto;
    Tela t2;
    t2.riempi(0);
    disegna_cartolina(t2, r, per, vuoto, q);
    CHECK(conta(t2, {r.x + 32, r.y, 64, r.h}, 144, 158) < conta(t, {r.x + 32, r.y, 64, r.h}, 144, 158) / 4);
    // col secondo da tenere in campo la camera si allontana
    Nave s2 = n;
    s2.pos = {3000, 0};
    const auto q2 = dal_registro(m.navi()[0], &s2, 3500, 0, 90, 0);
    CHECK(distanza(q2.occhio, {1500, 0}) > 3500);
    // dall'alto: campo largo
    CHECK_FALSE(dal_registro(m.navi()[0], nullptr, 3500, 8, 90, 0).alto_ingrandimento);
    // icone con i nomi del registro
    CHECK(icona_da_nome("radio") == Icona::telescrivente);
    CHECK(icona_da_nome("radiogoniometro") == Icona::radiogoniometro);
    CHECK(icona_da_nome("danni") == Icona::danni);
    CHECK(icona_da_nome("zona") == Icona::zona);
    CHECK(icona_da_nome("esm") == Icona::esm);
    CHECK(icona_da_nome("mina") == Icona::mina);
    CHECK(icona_da_nome("aereo_in_sorvolo") == Icona::aereo);
    CHECK(icona_da_nome("boa_vicina") == Icona::boa);
    CHECK(icona_da_nome("ping_sonar_immersione") == Icona::elicottero);
    CHECK(icona_da_nome("marcatore_avvistato") == Icona::marcatore);
    CHECK(icona_da_nome("boh") == Icona::sonar);
    for (const Icona i : {Icona::cascata, Icona::sonar, Icona::telescrivente, Icona::esm, Icona::radiogoniometro, Icona::danni, Icona::zona, Icona::mina,
                         Icona::aereo, Icona::boa, Icona::elicottero, Icona::marcatore}) {
        Tela ti;
        ti.riempi(0);
        disegna_icona(ti, ris.font, r, i, 1.0, 3, "MESSAGGIO DI PROVA");
        CHECK(conta(ti, r, 1, 255) > 500);
        CHECK(conta(ti, {0, 0, 640, 50}, 1, 255) == 0);
        CHECK(conta(ti, {0, 150, 640, 330}, 1, 255) == 0);
    }
}

TEST_CASE("equipaggio: ritratto di gruppo che cambia con feriti e caduti" * doctest::skip(!ci_sono_dati())) {
    const auto font = Font::carica(M688_DATI "/font/originale.fnt");
    const Rettangolo r{20, 20, 236, 310};
    ParametriEquipaggio p;
    p.battello = "700";
    Tela a, b;
    disegna_equipaggio(a, font, r, p);
    disegna_equipaggio(b, font, r, p);
    CHECK(uguali(a, b, r));
    CHECK(conta(a, {0, 0, 640, 20}, 1, 255) == 0);   // niente fuori dal riquadro
    const int pelle = conta(a, r, cerimonia::PELLE, cerimonia::PELLE + cerimonia::N_PELLE - 1);
    CHECK(pelle > 1500);
    CHECK(conta(a, r, cerimonia::ORO, cerimonia::ORO + cerimonia::N_ORO - 1) > 10);   // bottoni e berretti degli ufficiali
    // quattro caduti: meno volti e il nastro di lutto nell'angolo
    ParametriEquipaggio q = p;
    q.caduti = 4;
    q.feriti = 3;
    Tela c;
    disegna_equipaggio(c, font, r, q);
    CHECK(conta(c, r, cerimonia::PELLE, cerimonia::PELLE + cerimonia::N_PELLE - 1) < pelle);
    CHECK(conta(c, {r.x + r.w - 20, r.y, 20, 20}, cerimonia::NERO, cerimonia::NERO + 2) > conta(a, {r.x + r.w - 20, r.y, 20, 20}, cerimonia::NERO, cerimonia::NERO + 2));
    // VMF: niente berretti bianchi, le bustine nere e le righe della maglia
    ParametriEquipaggio v = p;
    v.marina = Marina::vmf;
    Tela d;
    disegna_equipaggio(d, font, r, v);
    CHECK(conta(d, r, cerimonia::SMALTO_BLU, cerimonia::SMALTO_BLU + cerimonia::N_SMALTO_BLU - 1) > 100);
}

TEST_CASE("giornale: pagina d'epoca e comunicato TASS nel loro riquadro; tabella datata del fascicolo" * doctest::skip(!ci_sono_dati())) {
    const auto font = Font::carica(M688_DATI "/font/originale.fnt");
    const Rettangolo r{100, 10, 400, 460};
    Tela a, b;
    a.riempi(0);
    b.riempi(0);
    disegna_giornale(a, font, r, Marina::usn, "The Washington Herald", "Norway protests", "Oslo ha convocato l'ambasciatore.", "14 ottobre 1989", 3);
    disegna_giornale(b, font, r, Marina::usn, "The Washington Herald", "Norway protests", "Oslo ha convocato l'ambasciatore.", "14 ottobre 1989", 3);
    CHECK(uguali(a, b, r));
    CHECK(conta(a, {0, 0, 100, 480}, 1, 255) == 0);
    CHECK(conta(a, {500, 0, 140, 480}, 1, 255) == 0);
    CHECK(conta(a, r, cerimonia::CARTA, cerimonia::CARTA + cerimonia::N_CARTA - 1) > 100000);
    CHECK(conta(a, r, cerimonia::NERO, cerimonia::NERO + 3) > 5000);   // testata, testo, retino della foto
    Tela c;
    c.riempi(0);
    disegna_giornale(c, font, r, Marina::vmf, "Pravda", "Il sommergibile non e' rientrato", "Sono in corso le ricerche.", "15 ottobre 1989", 4);
    CHECK(conta(c, r, cerimonia::SMALTO, cerimonia::SMALTO + cerimonia::N_SMALTO - 1) > 40);   // le stelle accanto alla testata
    // tabella: una riga in piu' scende di una riga
    Tela d;
    d.riempi(0);
    const Rettangolo q{50, 50, 500, 300};
    const std::vector<ColonnaTabella> col = {{"data", 1}, {"evento", 2}, {"classe", 1}};
    const int y1 = disegna_tabella(d, font, q, col, {{"12.03.1988", "affondato", "krivak ii"}}, "affondamenti");
    const int y2 = disegna_tabella(d, font, q, col, {{"12.03.1988", "affondato", "krivak ii"}, {"02.07.1988", "affondato", "kara"}}, "affondamenti");
    CHECK(y2 > y1);
    CHECK(y2 <= q.y + q.h + 2);
    CHECK(conta(d, {0, 0, 640, 50}, 1, 255) == 0);
}

TEST_CASE("medaglioni degli achievement: icone, livelli, non presi, nascosti e tavolozza della vista") {
    const auto& ic = icone_medaglioni();
    CHECK(ic.size() == 68);   // 41, i 14 di docs/26 e i 13 del secondo giro
    for (const auto& id : ic) CHECK_FALSE(categoria_medaglione(id).empty());
    CHECK(categoria_medaglione("sconosciuta").empty());
    auto disegna = [](std::string_view id, LivelloMedaglione l, bool preso, bool nascosto, TavolozzaMedaglione tav, int lato) {
        Tela t;
        t.riempi(255);
        disegna_medaglione(t, {100, 100, lato, lato}, id, l, preso, nascosto, tav);
        return t;
    };
    auto fuori_e_colori = [](const Tela& t, int lato, int& fuori, std::set<int>& colori) {
        fuori = 0;
        for (int y = 0; y < ALTEZZA_VISTA; ++y)
            for (int x = 0; x < LARGHEZZA_TELA; ++x) {
                const int c = t.at(x, y);
                if (c == 255) continue;
                if (x < 100 || y < 100 || x >= 100 + lato || y >= 100 + lato) ++fuori;
                colori.insert(c);
            }
    };
    for (const auto& id : ic) {
        CAPTURE(id);
        int fuori = 0;
        std::set<int> oro, bronzo, vista;
        fuori_e_colori(disegna(id, LivelloMedaglione::oro, true, false, TavolozzaMedaglione::cerimonia, 48), 48, fuori, oro);
        CHECK(fuori == 0);
        CHECK(oro.count(cerimonia::ORO + cerimonia::N_ORO - 1) + oro.count(cerimonia::ORO + cerimonia::N_ORO - 2) > 0);
        fuori_e_colori(disegna(id, LivelloMedaglione::bronzo, true, false, TavolozzaMedaglione::cerimonia, 48), 48, fuori, bronzo);
        CHECK(bronzo.count(cerimonia::ORO + cerimonia::N_ORO - 1) == 0);
        // la striscia del debriefing: solo indici che la luce della vista non tocca
        fuori_e_colori(disegna(id, LivelloMedaglione::argento, true, false, TavolozzaMedaglione::vista, 24), 24, fuori, vista);
        CHECK(fuori == 0);
        for (const int c : vista) CHECK(((c >= 0 && c <= 15) || (c >= 80 && c <= 95) || (c >= 160 && c <= 195) || c == 254));
    }
    // non preso: diverso dal preso; nascosto e non preso: il «?», uguale per tutti
    const Tela preso = disegna("fantasma", LivelloMedaglione::oro, true, false, TavolozzaMedaglione::cerimonia, 48);
    const Tela spento = disegna("fantasma", LivelloMedaglione::oro, false, false, TavolozzaMedaglione::cerimonia, 48);
    const Tela q1 = disegna("sasso", LivelloMedaglione::oro, false, true, TavolozzaMedaglione::cerimonia, 48);
    const Tela q2 = disegna("pendolare", LivelloMedaglione::oro, false, true, TavolozzaMedaglione::cerimonia, 48);
    const Tela sasso = disegna("sasso", LivelloMedaglione::oro, true, true, TavolozzaMedaglione::cerimonia, 48);
    int d_spento = 0, d_q = 0, d_sasso = 0;
    for (int y = 100; y < 148; ++y)
        for (int x = 100; x < 148; ++x) {
            d_spento += preso.at(x, y) != spento.at(x, y);
            d_q += q1.at(x, y) != q2.at(x, y);
            d_sasso += q1.at(x, y) != sasso.at(x, y);
        }
    CHECK(d_spento > 500);
    CHECK(d_q == 0);
    CHECK(d_sasso > 300);   // preso, il nascosto mostra il suo simbolo
    // un'icona sconosciuta disegna comunque (l'ancora)
    int fuori = 0;
    std::set<int> c;
    fuori_e_colori(disegna("sconosciuta", LivelloMedaglione::bronzo, true, false, TavolozzaMedaglione::cerimonia, 48), 48, fuori, c);
    CHECK(c.size() > 5);
}

TEST_CASE("albo d'onore: schede, scorrimento e uscita; l'albo vuoto e quello pieno si disegnano") {
    const auto regole = carica_carriera(std::string(M688_DATI) + "/carriera.toml");
    const auto font = Font::carica(std::string(M688_DATI) + "/font/originale.fnt");
    const auto file = std::filesystem::temp_directory_path() / "m688_prova_albo_onore.toml";
    AlboOnore a;
    for (int i = 0; i < 20; ++i) {
        VoceOnoreMissione v;
        v.punteggio = 1000 - i;
        v.nome = "Prova";
        v.marina = i % 2 ? "vmf" : "usn";
        v.grado = i % 2 ? "kap2" : "O-5";
        v.missione = "SCENE1A";
        v.origine = i == 3 ? "rapida" : "originale";
        if (i == 3) v.codice = "ABCD-1234";
        a.missioni.push_back(v);
    }
    VoceOnoreCarriera c;
    c.nome = "Caduto";
    c.marina = "vmf";
    c.grado = "kap1";
    c.stato = "morto";
    a.carriere.push_back(c);
    salva_albo_onore(a, file);
    SchermataAlboOnore s(regole, M688_DATI, file);
    Comandi k;
    CHECK(s.comandi(k) == SchermataAlboOnore::Uscita::nessuna);
    CHECK(s.scheda() == 0);
    // le frecce scorrono a scatti fino all'ultima pagina, non oltre
    for (int i = 0; i < 40; ++i) {
        Comandi g;
        g.su_giu = i % 2 ? 0 : -1;
        s.comandi(g);
    }
    Tela t;
    s.disegna(t, font);
    int pieni = 0;
    for (int y = 70; y < 440; ++y)
        for (int x = 30; x < 600; ++x) pieni += t.at(x, y) == static_cast<std::uint8_t>(cerimonia::NERO + 1);
    CHECK(pieni > 500);   // le righe scritte
    Comandi due;
    due.cifre = {2};
    s.comandi(due);
    CHECK(s.scheda() == 1);
    Comandi tab;
    tab.tab = 1;
    s.comandi(tab);
    CHECK(s.scheda() == 2);
    s.comandi(tab);
    CHECK(s.scheda() == 0);   // Tab gira
    Comandi clic;
    clic.clic = {{18 + 8 + 196 + 50, 62 - 10}};   // la linguetta delle carriere
    s.comandi(clic);
    CHECK(s.scheda() == 1);
    s.disegna(t, font);
    Comandi esc;
    esc.esc = true;
    CHECK(s.comandi(esc) == SchermataAlboOnore::Uscita::menu);
    // senza file: le schede vuote, niente crolli
    std::filesystem::remove(file);
    SchermataAlboOnore vuota(regole, M688_DATI, file);
    for (int i = 0; i < 3; ++i) {
        vuota.scheda(i);
        vuota.disegna(t, font);
    }
    CHECK(vuota.scheda() == 2);
}

// La figurina di ogni evento (app/figurina_evento.hpp; scelta dell'utente del 07/11/2026): mai un riquadro vuoto
TEST_CASE("cartolina: le icone sus e periscopio, gli eventi di routine dell'IA senza fotografia") {
    CHECK(icona_da_nome("sus") == Icona::marcatore);
    CHECK(icona_da_nome("periscopio") == Icona::periscopio);
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    Tela t;
    t.riempi(0);
    disegna_icona(t, f, {50, 50, 128, 96}, Icona::periscopio, 1.0, 7);
    CHECK(conta(t, {50, 50, 128, 96}, 144, 158) > 30);   // la testa del periscopio
    CHECK(conta(t, {50, 50, 128, 96}, 15, 15) > 10);     // il baffo
    SceltaFotografie scelta;
    const auto evento = [](int id, TipoEvento tipo, int interesse) {
        EventoMissione e;
        e.id = id;
        e.tipo = tipo;
        e.interesse = interesse;
        return e;
    };
    for (const TipoEvento k : {TipoEvento::classificazione_ia, TipoEvento::comandante_ia, TipoEvento::reazione_ia, TipoEvento::tma_ia,
                               TipoEvento::scoperta_ia, TipoEvento::ingaggio_ia, TipoEvento::ritirata_ia, TipoEvento::evasione_ia})
        CHECK_FALSE(scelta.scatta(evento(1, k, 90), 0));
    for (const TipoEvento k : {TipoEvento::scoperta, TipoEvento::colpo, TipoEvento::lancio_nemico, TipoEvento::lancio_ia, TipoEvento::affondamento})
        CHECK(scelta.scatta(evento(2, k, 30), 0));
}

TEST_CASE("cartolina: il soggetto che non c'e' piu' torna dalla posa, senza fotografia si inquadra lo stesso" * doctest::skip(!fs::exists(M688_DATI "/classi.toml"))) {
    const auto ris = carica_risorse(M688_DATI);
    Periscopio per(ris.font, ris.classi, ris.sprite);
    per.usa_modelli(ris.modelli, rampa_grigi(ris.giorno));
    EventoMissione e;
    e.tipo = TipoEvento::affondamento;
    e.inquadratura.soggetto = 7;
    e.inquadratura.distanza_m = 3500.0;
    PosaEvento posa;
    posa.unita = 7;
    posa.classe = "krivak2";
    posa.categoria = "nave";
    posa.pos = {1000.0, 2000.0};
    posa.rotta = 60.0;
    e.pose.push_back(posa);
    Mondo foto;   // la fotografia, 4 s dopo: la nave non c'e' piu', un'altra si'
    Nave altra;
    altra.id = 3;
    altra.classe = "krivak2";
    altra.pos = {9000.0, 9000.0};
    foto.aggiungi(altra);
    REQUIRE(nave_in_foto(foto, 7) == nullptr);
    CHECK(figurina_evento(e, &foto) == Figurina::scena);
    CHECK(figurina_evento(e, nullptr) == Figurina::scena);
    const Mondo dalla_foto = mondo_scena(e, &foto);
    CHECK(dalla_foto.trova(3) != nullptr);   // il resto della fotografia resta
    REQUIRE(dalla_foto.trova(7) != nullptr);
    CHECK(dalla_foto.trova(7)->pos.x == doctest::Approx(1000.0));
    const Mondo dalle_pose = mondo_scena(e, nullptr);
    REQUIRE(nave_in_foto(dalle_pose, 7) != nullptr);
    // si vede: i grigi del modello nel centro della cartolina
    const auto q = dal_registro(*nave_in_foto(dalle_pose, 7), nullptr, e.inquadratura.distanza_m, 0.0, 90.0, 0.0);
    Tela t;
    t.riempi(0);
    const Rettangolo r{50, 50, 128, 96};
    disegna_cartolina(t, r, per, dalle_pose, q);
    CHECK(conta(t, {r.x + 32, r.y, 64, r.h}, 144, 158) > 60);
    // l'immagine che i danni disegnano al posto dell'unita' vale come l'unita'
    Mondo con_immagine;
    Nave immagine = altra;
    immagine.id = DanniMissione::ID_IMMAGINE + 7;
    con_immagine.aggiungi(immagine);
    CHECK(figurina_evento(e, &con_immagine) == Figurina::scena);
    CHECK(mondo_scena(e, &con_immagine).navi().size() == 1);
    // un battello profondo: nella sola scena a pelo d'acqua, e si vede (l'affondamento ha la sua scena, qui sotto)
    EventoMissione profondo = e;
    profondo.tipo = TipoEvento::colpo;
    profondo.pose[0].classe = "los_angeles";
    profondo.pose[0].categoria = "sottomarino";
    profondo.pose[0].quota = 150.0;
    profondo.inquadratura.distanza_m = 400.0;
    const Mondo a_galla = mondo_scena(profondo, nullptr);
    REQUIRE(a_galla.trova(7) != nullptr);
    CHECK(a_galla.trova(7)->quota == doctest::Approx(0.0));
    const auto qp = dal_registro(*a_galla.trova(7), nullptr, 400.0, 0.0, 90.0, 0.0);
    Tela tp;
    tp.riempi(0);
    disegna_cartolina(tp, r, per, a_galla, qp);
    Tela vuota;   // la stessa scena senza il battello: quello che cambia e' lo scafo che affiora
    vuota.riempi(0);
    disegna_cartolina(vuota, r, per, Mondo{}, qp);
    int scafo = 0;
    for (int y = r.y; y < r.y + r.h; ++y)
        for (int x = r.x; x < r.x + r.w; ++x) scafo += tp.at(x, y) != vuota.at(x, y);
    CHECK(scafo > 40);
    // un aereo alto esce dal campo stretto: nella sola scena a 20 m
    EventoMissione in_volo = e;
    in_volo.tipo = TipoEvento::colpo;
    in_volo.pose[0].classe = "sh60b";
    in_volo.pose[0].categoria = "aereo";
    in_volo.pose[0].quota = 150.0;
    const Mondo basso = mondo_scena(in_volo, nullptr);
    REQUIRE(basso.trova(7) != nullptr);
    CHECK(basso.trova(7)->quota == doctest::Approx(-20.0));
    const auto qv = dal_registro(*basso.trova(7), nullptr, 800.0, 0.0, 90.0, 0.0);
    Tela tv, vuotav;
    tv.riempi(0);
    vuotav.riempi(0);
    disegna_cartolina(tv, r, per, basso, qv);
    disegna_cartolina(vuotav, r, per, Mondo{}, qv);
    int aereo = 0;
    for (int y = r.y; y < r.y + r.h; ++y)
        for (int x = r.x; x < r.x + r.w; ++x) aereo += tv.at(x, y) != vuotav.at(x, y);
    CHECK(aereo > 20);
    // lo stacco: mai da lontano e dall'alto, che lascerebbe pochi pixel
    EventoMissione lontana = e;
    lontana.inquadratura.distanza_m = 3704.0;
    lontana.inquadratura.elevazione_deg = 5.0;
    CHECK(stacco_scena(lontana).distanza_m == doctest::Approx(1500.0));
    CHECK(stacco_scena(lontana).elevazione_deg == doctest::Approx(0.0));
    EventoMissione vicina = e;
    vicina.inquadratura.distanza_m = 500.0;
    vicina.inquadratura.elevazione_deg = 3.0;
    CHECK(stacco_scena(vicina).distanza_m == doctest::Approx(500.0));
    CHECK(stacco_scena(vicina).elevazione_deg == doctest::Approx(3.0));
    // un siluro non ha sagoma: si inquadra chi lo riguarda
    EventoMissione siluro = e;
    siluro.inquadratura.secondo = 3;
    siluro.pose[0].categoria = "arma";
    siluro.pose[0].quota = 100.0;
    PosaEvento battello = posa;
    battello.unita = 3;
    siluro.pose.push_back(battello);
    CHECK(soggetto_scena(siluro) == 3);
    siluro.inquadratura.distanza_m = 200.0;
    siluro.inquadratura.elevazione_deg = 5.0;
    CHECK(stacco_scena(siluro).distanza_m == doctest::Approx(800.0));   // non i 200 m del siluro
    CHECK(stacco_scena(siluro).elevazione_deg == doctest::Approx(0.0));   // e senza l'alzo, che apre il campo
    siluro.pose[0].quota = -50.0;   // un missile in volo si inquadra lui
    CHECK(soggetto_scena(siluro) == 7);
    // la coppia in campo solo se e' vicina: lontana resta il soggetto, grande
    Mondo coppia;
    Nave primo = altra, vicino = altra, lontano = altra;
    primo.id = 1;
    primo.pos = {0.0, 0.0};
    vicino.id = 2;
    vicino.pos = {400.0, 0.0};
    lontano.id = 4;
    lontano.pos = {9000.0, 0.0};
    coppia.aggiungi(primo);
    coppia.aggiungi(vicino);
    coppia.aggiungi(lontano);
    CHECK(secondo_in_campo(coppia, primo, 1, 2, 3.0) != nullptr);
    CHECK(secondo_in_campo(coppia, primo, 1, 4, 3.0) == nullptr);
    CHECK(secondo_in_campo(coppia, primo, 1, -1, 3.0) == nullptr);
    CHECK(secondo_in_campo(coppia, primo, 1, 1, 3.0) == nullptr);
    // senza nulla da inquadrare: la riserva; cio' che non si vede: la sua icona
    EventoMissione senza = e;
    senza.pose.clear();
    CHECK(figurina_evento(senza, nullptr) == Figurina::riserva);
    CHECK(icona_di_riserva(TipoEvento::affondamento) == Icona::danni);
    EventoMissione nascosto = e;
    nascosto.inquadratura.visibile = false;
    CHECK(figurina_evento(nascosto, nullptr) == Figurina::icona);
}

// La scena dell'affondamento (l'utente, 07/11/2026). Attese scritte prima del codice: ghiaccio-d4-tacche-cartoline-07-11.md
namespace {
EventoMissione affondamento(int id, const char* classe, const char* categoria, double quota) {
    EventoMissione e;
    e.id = id + 1000;
    e.tipo = TipoEvento::affondamento;
    e.unita = id;
    e.categoria = categoria;
    e.inquadratura.soggetto = id;
    e.inquadratura.distanza_m = 390.0;
    PosaEvento p;
    p.unita = id;
    p.classe = classe;
    p.categoria = categoria;
    p.pos = {1000.0, 2000.0};
    p.rotta = 60.0;
    p.quota = quota;
    e.pose.push_back(p);
    return e;
}

// i grigi degli sprite in una colonna: la riga piu' alta, o -1
int cima_grigia(const Tela& t, Rettangolo r, int x) {
    for (int y = r.y; y < r.y + r.h; ++y)
        if (t.at(x, y) >= 144 && t.at(x, y) <= 158) return y;
    return -1;
}

bool affonda_davvero(const PosaNave& p) {
    return p.danneggiata && (std::fabs(p.prodiero.sbandamento) >= 15.0 || std::fabs(p.prodiero.assetto) >= 6.0) && p.prodiero.immersione > 0.0;
}
}  // namespace

TEST_CASE("cartolina dell'affondamento: quale scena per chi, e gli altri eventi restano quelli di prima") {
    CHECK(scena_affondamento(affondamento(7, "petroliera", "nave", 0.0)) == ScenaAffondamento::scafo);
    CHECK(scena_affondamento(affondamento(7, "krivak2", "nave", 0.0)) == ScenaAffondamento::scafo);
    CHECK(scena_affondamento(affondamento(7, "los_angeles", "sottomarino", 0.0)) == ScenaAffondamento::scafo);
    CHECK(scena_affondamento(affondamento(7, "los_angeles", "sottomarino", 3.0)) == ScenaAffondamento::scafo);
    CHECK(scena_affondamento(affondamento(7, "los_angeles", "sottomarino", 3.5)) == ScenaAffondamento::segni);
    CHECK(scena_affondamento(affondamento(7, "los_angeles", "sottomarino", 200.0)) == ScenaAffondamento::segni);
    CHECK(scena_affondamento(affondamento(7, "p3c", "aereo", 150.0)) == ScenaAffondamento::segni);
    CHECK(scena_affondamento(affondamento(7, "mk48", "arma", 100.0)) == ScenaAffondamento::nessuna);
    EventoMissione senza = affondamento(7, "petroliera", "nave", 0.0);
    senza.pose.clear();
    CHECK(scena_affondamento(senza) == ScenaAffondamento::nessuna);

    // la posa della fisica nella fotografia: 4 s dopo, ancora quasi niente, ma con una falla
    Mondo foto;
    Nave immagine;
    immagine.id = DanniMissione::ID_IMMAGINE + 7;
    immagine.classe = "petroliera";
    immagine.pos = {1100.0, 2100.0};
    immagine.posa.danneggiata = true;
    immagine.posa.prodiero.sbandamento = -1.4;
    immagine.posa.prodiero.immersione = 0.6;
    immagine.posa.segni.push_back({});
    foto.aggiungi(immagine);
    Nave altra;
    altra.id = 3;
    altra.classe = "krivak2";
    foto.aggiungi(altra);

    // gli altri eventi: la stessa fotografia, la stessa posa di prima
    for (const TipoEvento t : {TipoEvento::colpo, TipoEvento::scoperta, TipoEvento::danno_subito, TipoEvento::fine}) {
        EventoMissione e = affondamento(7, "petroliera", "nave", 0.0);
        e.tipo = t;
        CHECK(scena_affondamento(e) == ScenaAffondamento::nessuna);
        const Nave* n = nave_in_foto(mondo_scena(e, &foto), 7);
        REQUIRE(n != nullptr);
        CHECK(n->posa.prodiero.sbandamento == doctest::Approx(-1.4));
        CHECK(n->posa.prodiero.immersione == doctest::Approx(0.6));
        CHECK(n->posa.prodiero.assetto == 0.0);
    }

    // la nave: la fotografia con la falla, la fotografia senza la nave, nessuna fotografia
    const EventoMissione nave = affondamento(7, "petroliera", "nave", 0.0);
    const Mondo con = mondo_scena(nave, &foto, 240.0);
    REQUIRE(nave_in_foto(con, 7) != nullptr);
    CHECK(nave_in_foto(con, 7)->pos.x == doctest::Approx(1100.0));
    CHECK(affonda_davvero(nave_in_foto(con, 7)->posa));
    CHECK(nave_in_foto(con, 7)->posa.segni.size() == 1);   // la falla della fisica resta
    CHECK(nave_in_foto(con, 7)->quota == 0.0);
    CHECK(con.trova(3) != nullptr);
    CHECK_FALSE(con.trova(3)->posa.danneggiata);   // le altre non si toccano
    Mondo vuota;
    vuota.aggiungi(altra);
    const Mondo senza_nave = mondo_scena(nave, &vuota, 240.0);
    REQUIRE(nave_in_foto(senza_nave, 7) != nullptr);
    CHECK(affonda_davvero(nave_in_foto(senza_nave, 7)->posa));
    const Mondo senza_foto = mondo_scena(nave, nullptr, 240.0);
    REQUIRE(nave_in_foto(senza_foto, 7) != nullptr);
    CHECK(affonda_davvero(nave_in_foto(senza_foto, 7)->posa));

    // la fisica dice che e' gia' sotto: la scena la mostra lo stesso a mezz'acqua, non affondata del tutto
    Mondo sotto = foto;
    for (Nave& n : sotto.navi()) if (n.id == immagine.id) n.posa.prodiero.immersione = 60.0;
    CHECK(nave_in_foto(mondo_scena(nave, &sotto, 240.0), 7)->posa.prodiero.immersione < 30.0);

    // spezzata resta spezzata: due tronconi a V
    Mondo rotta = foto;
    for (Nave& n : rotta.navi()) if (n.id == immagine.id) n.posa.spezzata = true, n.posa.x_rottura = 12.0;
    const PosaNave v = nave_in_foto(mondo_scena(nave, &rotta, 240.0), 7)->posa;
    CHECK(v.spezzata);
    CHECK(v.prodiero.x_perno == 12.0);
    CHECK(v.poppiero.x_perno == 12.0);
    CHECK(v.prodiero.assetto > 6.0);
    CHECK(v.poppiero.assetto < -6.0);

    // il modo: i tre delle navi e sempre lo stesso per la stessa; il battello solo con la prua alta
    const auto posa_di = [](int id, const char* c, const char* k) { return posa_affondamento(affondamento(id, c, k, 0.0), {}, 120.0); };
    CHECK(posa_di(300, "perry", "nave").prodiero.assetto < -6.0);
    CHECK(posa_di(301, "perry", "nave").prodiero.assetto > 6.0);
    CHECK(std::fabs(posa_di(302, "perry", "nave").prodiero.sbandamento) >= 15.0);
    CHECK(posa_di(300, "perry", "nave").prodiero.assetto == posa_di(300, "perry", "nave").prodiero.assetto);
    for (const int id : {300, 301, 302}) CHECK(posa_di(id, "los_angeles", "sottomarino").prodiero.assetto > 6.0);

    // l'immerso e chi era in volo: lui non c'e', il resto della fotografia si'
    const EventoMissione sub = affondamento(7, "los_angeles", "sottomarino", 200.0);
    const Mondo immerso = mondo_scena(sub, &foto);
    CHECK(nave_in_foto(immerso, 7) == nullptr);
    CHECK(immerso.trova(3) != nullptr);
    CHECK(nave_in_foto(mondo_scena(sub, nullptr), 7) == nullptr);

    // la camera e gli effetti
    CHECK(stacco_affondamento(ScenaAffondamento::scafo, 240.0).distanza_m == doctest::Approx(816.0));
    CHECK(stacco_affondamento(ScenaAffondamento::scafo, 30.0).distanza_m == doctest::Approx(350.0));
    const std::map<std::string, ClasseNave> classi = [] {
        ClasseNave c;
        c.lunghezza_m = 77.0;
        return std::map<std::string, ClasseNave>{{"petroliera", c}};
    }();
    CHECK(lunghezza_affondamento(classi, nave) == doctest::Approx(77.0));
    CHECK(lunghezza_affondamento({}, nave) == doctest::Approx(130.0));
    CHECK(lunghezza_affondamento({}, sub) == doctest::Approx(100.0));
    const auto quanti = [&](const Effetti& fx, Materia m) {
        return std::count_if(fx.particelle().begin(), fx.particelle().end(), [&](const Particella& p) { return p.materia == m; });
    };
    const Effetti a = effetti_affondamento(nave, ScenaAffondamento::scafo, {0.0, 0.0}, 240.0, vuota);
    const Effetti b = effetti_affondamento(nave, ScenaAffondamento::scafo, {0.0, 0.0}, 240.0, vuota);
    CHECK(a.numero() == b.numero());   // la stessa figurina, sempre
    CHECK(quanti(a, Materia::fumo) > 0);
    CHECK(quanti(a, Materia::schiuma) > 0);
    CHECK(quanti(a, Materia::nafta) > 0);
    CHECK(quanti(a, Materia::detrito) > 0);
    const Effetti c = effetti_affondamento(sub, ScenaAffondamento::segni, {0.0, 0.0}, 100.0, vuota);
    CHECK(quanti(c, Materia::fumo) == 0);
    CHECK(quanti(c, Materia::schiuma) > 0);
    CHECK(quanti(c, Materia::nafta) > 0);
    CHECK(quanti(c, Materia::detrito) > 0);
}

TEST_CASE("cartolina dell'affondamento: si vede lo scafo che va giu', dell'immerso nessuno scafo" * doctest::skip(!fs::exists(M688_DATI "/classi.toml"))) {
    const auto ris = carica_risorse(M688_DATI);
    Periscopio per(ris.font, ris.classi, ris.sprite);
    per.usa_modelli(ris.modelli, rampa_grigi(ris.giorno));
    const Rettangolo r{0, 0, 184, 78};
    // senza effetti, per guardare il solo scafo: le navi a tre modi e i battelli, dal modello della classe
    struct Caso {
        int id;
        const char* classe;
        const char* categoria;
        bool inclinata;
    };
    for (const Caso& k : {Caso{300, "petroliera", "nave", true}, Caso{301, "mercantile", "nave", true}, Caso{302, "kirov", "nave", false}, Caso{300, "perry", "nave", true},
                          Caso{301, "nimitz", "nave", true}, Caso{302, "krivak2", "nave", false}, Caso{300, "los_angeles", "sottomarino", true}, Caso{301, "typhoon", "sottomarino", true}}) {
        CAPTURE(k.classe);
        CAPTURE(k.id);
        const EventoMissione e = affondamento(k.id, k.classe, k.categoria, 0.0);
        const double lunga = ris.classi.at(k.classe).lunghezza_m;
        const Mondo mondo = mondo_scena(e, nullptr, lunga);
        const Nave& n = *nave_in_foto(mondo, k.id);
        const StaccoScena st = stacco_affondamento(ScenaAffondamento::scafo, lunga);
        const PuntoDiVista q = dal_registro(n, nullptr, st.distanza_m, st.elevazione_deg, 70.0, 0.0);
        Tela t(r.w, r.h);
        disegna_cartolina(t, r, per, mondo, q);
        CHECK(conta(t, r, 144, 158) > 150);
        // tutta in campo: i grigi non toccano i due bordi
        int primo = -1, ultimo = -1;
        for (int x = 0; x < r.w; ++x)
            if (cima_grigia(t, r, x) >= 0) {
                if (primo < 0) primo = x;
                ultimo = x;
            }
        CHECK(primo > 0);
        CHECK(ultimo < r.w - 1);
        if (k.inclinata) {   // la cima dello scafo sale da un capo all'altro
            const int a = cima_grigia(t, r, primo + (ultimo - primo) / 8), b = cima_grigia(t, r, ultimo - (ultimo - primo) / 8);
            CHECK(std::abs(a - b) >= 4);
        }
        // e' diversa dalla stessa nave a galla, intera
        Mondo intera;
        Nave sana = nave_da_posa(e.pose[0]);
        intera.aggiungi(sana);
        Tela u(r.w, r.h);
        disegna_cartolina(u, r, per, intera, dal_registro(sana, nullptr, st.distanza_m, st.elevazione_deg, 70.0, 0.0));
        int diversi = 0;
        for (int y = 0; y < r.h; ++y)
            for (int x = 0; x < r.w; ++x) diversi += t.at(x, y) != u.at(x, y);
        CHECK(diversi > 100);
    }
    // l'immerso: che nella fotografia ci sia o no, la scena e' la stessa, e nessun grigio di scafo
    const EventoMissione sub = affondamento(7, "los_angeles", "sottomarino", 200.0);
    Mondo foto;
    Nave immagine;
    immagine.id = DanniMissione::ID_IMMAGINE + 7;
    immagine.classe = "los_angeles";
    immagine.pos = {1000.0, 2000.0};
    immagine.quota = 200.0;
    foto.aggiungi(immagine);
    Nave luogo = nave_da_posa(sub.pose[0]);
    const StaccoScena st = stacco_affondamento(ScenaAffondamento::segni, 110.0);
    const PuntoDiVista q = dal_registro(luogo, nullptr, st.distanza_m, st.elevazione_deg, 70.0, 0.0);
    Mondo vuoto;
    const Effetti fx = effetti_affondamento(sub, ScenaAffondamento::segni, luogo.pos, 110.0, vuoto);
    Tela a(r.w, r.h), b(r.w, r.h), mare(r.w, r.h);
    disegna_cartolina(a, r, per, mondo_scena(sub, &foto), q, &fx);
    disegna_cartolina(b, r, per, mondo_scena(sub, nullptr), q, &fx);
    disegna_cartolina(mare, r, per, vuoto, q);
    int diversi = 0, dal_mare = 0;
    for (int y = 0; y < r.h; ++y)
        for (int x = 0; x < r.w; ++x) {
            diversi += a.at(x, y) != b.at(x, y);
            dal_mare += a.at(x, y) != mare.at(x, y);
        }
    CHECK(diversi == 0);
    CHECK(dal_mare > 200);   // il segno sul mare si vede
}

// Le scene si guardano da fuori: niente reticolo (l'utente, 07/11/2026). Il periscopio vero, e con lui la foto del periscopio, lo tiene
// Le scene vuote del 07/11/2026: comandante_ia a 500 m e 10 gradi, reazione_ia a 800 m e 12: oltre la vista, solo mare
TEST_CASE("cartolina: con l'alzo del registro il soggetto resta nella scena" * doctest::skip(!fs::exists(M688_DATI "/classi.toml"))) {
    const auto ris = carica_risorse(M688_DATI);
    Periscopio per(ris.font, ris.classi, ris.sprite);
    per.usa_modelli(ris.modelli, rampa_grigi(ris.giorno));
    const Rettangolo r{0, 0, 184, 78};
    struct Caso {
        const char* classe;
        const char* categoria;
        double distanza_m, alzo;
        double atteso;
    };
    for (const Caso& k : {Caso{"los_angeles", "sottomarino", 500.0, 10.0, 5.0}, Caso{"krivak2", "nave", 800.0, 12.0, 5.0}, Caso{"krivak2", "nave", 800.0, 8.0, 5.0},
                          Caso{"krivak2", "nave", 390.0, 5.0, 5.0}, Caso{"krivak2", "nave", 390.0, 4.0, 4.0}}) {
        CAPTURE(k.classe);
        CAPTURE(k.alzo);
        EventoMissione e;
        e.tipo = TipoEvento::comandante_ia;
        e.inquadratura.soggetto = 7;
        e.inquadratura.distanza_m = k.distanza_m;
        e.inquadratura.elevazione_deg = k.alzo;
        e.inquadratura.azimut_deg = 90.0;   // quello del registro: di fianco
        PosaEvento p;
        p.unita = 7;
        p.classe = k.classe;
        p.categoria = k.categoria;
        p.pos = {1000.0, 2000.0};
        p.rotta = 60.0;
        p.quota = k.categoria == std::string("sottomarino") ? 150.0 : 0.0;
        e.pose.push_back(p);
        const StaccoScena st = stacco_scena(e);
        CHECK(st.distanza_m == doctest::Approx(k.distanza_m));
        CHECK(st.elevazione_deg == doctest::Approx(k.atteso));
        CHECK(180.0 + std::tan(radianti(st.elevazione_deg)) * Periscopio::pixel_per_radiante(false) < ALTEZZA_VISTA);
        const Mondo mondo = mondo_scena(e, nullptr);
        const Nave& n = *nave_in_foto(mondo, 7);
        const PuntoDiVista q = dal_registro(n, nullptr, st.distanza_m, st.elevazione_deg, e.inquadratura.azimut_deg, 0.0);
        Tela con(r.w, r.h), senza(r.w, r.h);
        disegna_cartolina(con, r, per, mondo, q);
        disegna_cartolina(senza, r, per, Mondo{}, q);
        int scafo = 0;
        for (int y = 0; y < r.h; ++y)
            for (int x = 0; x < r.w; ++x) scafo += con.at(x, y) != senza.at(x, y);
        CHECK(scafo > 100);
    }
}

// Le altre scene vuote del censimento del 07/11/2026
TEST_CASE("cartolina: l'aereo nel campo stretto, l'esca e il contatto biologico non lasciano il mare vuoto" * doctest::skip(!fs::exists(M688_DATI "/classi.toml"))) {
    const auto ris = carica_risorse(M688_DATI);
    Periscopio per(ris.font, ris.classi, ris.sprite);
    per.usa_modelli(ris.modelli, rampa_grigi(ris.giorno));
    const auto posa = [](int id, const char* classe, const char* categoria, double quota) {
        PosaEvento p;
        p.unita = id;
        p.classe = classe;
        p.categoria = categoria;
        p.pos = {1000.0, 2000.0};
        p.quota = quota;
        return p;
    };
    // l'aereo in sorvolo: 400 m e -20 gradi dal registro
    EventoMissione sorvolo;
    sorvolo.tipo = TipoEvento::aereo_in_sorvolo;
    sorvolo.unita = 5;
    sorvolo.inquadratura.soggetto = 5;
    sorvolo.inquadratura.distanza_m = 400.0;
    sorvolo.inquadratura.elevazione_deg = -20.0;
    sorvolo.inquadratura.azimut_deg = 90.0;
    sorvolo.pose.push_back(posa(5, "il38", "aereo", 7000.0));
    const StaccoScena st = stacco_scena(sorvolo);
    CHECK(st.distanza_m == doctest::Approx(1500.0));
    const Mondo m = mondo_scena(sorvolo, nullptr);
    const Rettangolo r{0, 0, 184, 78};
    const PuntoDiVista q = dal_registro(*nave_in_foto(m, 5), nullptr, st.distanza_m, st.elevazione_deg, 90.0, 0.0);
    Tela con(r.w, r.h), senza(r.w, r.h);
    disegna_cartolina(con, r, per, m, q);
    disegna_cartolina(senza, r, per, Mondo{}, q);
    int aereo = 0;
    for (int y = 0; y < r.h; ++y)
        for (int x = 0; x < r.w; ++x) aereo += con.at(x, y) != senza.at(x, y);
    CHECK(aereo > 100);
    // con l'alzo il campo e' quello largo, e la distanza del registro va bene; una nave non ha limiti
    EventoMissione largo = sorvolo;
    largo.inquadratura.elevazione_deg = 3.0;
    CHECK(stacco_scena(largo).distanza_m == doctest::Approx(400.0));
    EventoMissione nave = sorvolo;
    nave.pose[0] = posa(5, "krivak2", "nave", 0.0);
    CHECK(stacco_scena(nave).distanza_m == doctest::Approx(400.0));

    // la coppia nel campo stretto: 400 m porterebbero la camera a 9 km, nel campo largo restano in due
    {
        Mondo coppia;
        Nave uno, due;
        uno.id = 1;
        due.id = 2;
        due.pos = {400.0, 0.0};
        coppia.aggiungi(uno);
        coppia.aggiungi(due);
        CHECK(secondo_in_campo(coppia, uno, 1, 2, 0.0) == nullptr);
        CHECK(secondo_in_campo(coppia, uno, 1, 2, -20.0) == nullptr);
        CHECK(secondo_in_campo(coppia, uno, 1, 2, 3.0) != nullptr);
        for (Nave& n : coppia.navi()) if (n.id == 2) n.pos = {60.0, 0.0};
        CHECK(secondo_in_campo(coppia, uno, 1, 2, 0.0) != nullptr);
        for (Nave& n : coppia.navi()) if (n.id == 2) n.pos = {600.0, 0.0};
        CHECK(secondo_in_campo(coppia, uno, 1, 2, 3.0) == nullptr);
    }

    // il siluro sedotto: non l'esca, che non si vede, ma chi resta
    EventoMissione esca;
    esca.tipo = TipoEvento::esca_sedotta;
    esca.unita = 21;
    esca.altra = 22;
    esca.giocatore = 0;
    esca.inquadratura.soggetto = 21;
    esca.inquadratura.secondo = 22;
    esca.pose = {posa(21, "mk48", "arma", 200.0), posa(22, "mg74", "arma", 200.0), posa(0, "los_angeles", "sottomarino", 100.0)};
    CHECK(soggetto_scena(esca) == 0);
    CHECK(figurina_evento(esca, nullptr) == Figurina::scena);
    // se non resta nessuno: la riserva, non il mare
    esca.pose.pop_back();
    CHECK(soggetto_scena(esca) == 21);
    CHECK(figurina_evento(esca, nullptr) == Figurina::riserva);

    // il contatto biologico: l'icona, anche se la fotografia lo tiene
    EventoMissione bio;
    bio.tipo = TipoEvento::scoperta;
    bio.unita = 1;
    bio.inquadratura.soggetto = 1;
    bio.pose.push_back(posa(1, "biologico", "nessuna", 46.0));
    Mondo foto;
    Nave b;
    b.id = 1;
    b.classe = "biologico";
    b.quota = 46.0;
    foto.aggiungi(b);
    CHECK(figurina_evento(bio, &foto) == Figurina::riserva);
    CHECK(figurina_evento(bio, nullptr) == Figurina::riserva);
    CHECK(icona_di_riserva(bio.tipo) == Icona::cascata);
    // una nave resta una scena, con la fotografia o senza
    bio.pose[0] = posa(1, "krivak2", "nave", 0.0);
    CHECK(figurina_evento(bio, &foto) == Figurina::scena);
    CHECK(figurina_evento(bio, nullptr) == Figurina::scena);
}

TEST_CASE("cartolina: la scena e' pulita, il periscopio vero tiene il suo reticolo" * doctest::skip(!fs::exists(M688_DATI "/classi.toml"))) {
    const auto ris = carica_risorse(M688_DATI);
    Periscopio per(ris.font, ris.classi, ris.sprite);
    per.usa_modelli(ris.modelli, rampa_grigi(ris.giorno));
    const Rettangolo r{0, 0, 184, 78};
    for (const double elevazione : {0.0, 3.0, 5.0}) {   // i due campi, dov'erano croce e tacche
        CAPTURE(elevazione);
        Tela t(r.w, r.h);
        disegna_cartolina(t, r, per, Mondo{}, inquadra({0.0, 0.0}, 800.0, 90.0, 0.0, elevazione));
        CHECK(conta(t, r, colori::NERO, colori::NERO) == 0);
    }
    // la vista vera, con lo stesso mondo vuoto: croce e tacche di prima, col campo predefinito
    Nave occhio;
    occhio.id = -1;
    VistaPeriscopio v;
    Tela vera;
    vera.riempi(0);
    per.disegna(vera, Mondo{}, occhio, v);
    int croce = 0;
    for (int y = 40; y < ALTEZZA_VISTA - 30; ++y) croce += vera.at(LARGHEZZA_TELA / 2, y) == colori::NERO;
    CHECK(croce > 100);
    CHECK(vera.at(LARGHEZZA_TELA / 2, 5) == colori::NERO);   // la fascia della scala
    v.reticolo = false;
    Tela senza;
    senza.riempi(0);
    per.disegna(senza, Mondo{}, occhio, v);
    int croce_senza = 0;
    for (int y = 40; y < ALTEZZA_VISTA - 30; ++y) croce_senza += senza.at(LARGHEZZA_TELA / 2, y) == colori::NERO;
    CHECK(croce_senza == 0);
    // con l'oculare il campo non conta: il reticolo e' il suo
    VistaPeriscopio oc;
    oc.oculare.attivo = true;
    oc.oculare.campo_gradi = 8.0;
    Tela con_oculare, senza_oculare;
    con_oculare.riempi(0);
    senza_oculare.riempi(0);
    per.disegna(con_oculare, Mondo{}, occhio, oc);
    oc.reticolo = false;
    per.disegna(senza_oculare, Mondo{}, occhio, oc);
    int diversi = 0;
    for (int y = 0; y < ALTEZZA_VISTA; ++y)
        for (int x = 0; x < LARGHEZZA_TELA; ++x) diversi += con_oculare.at(x, y) != senza_oculare.at(x, y);
    CHECK(diversi == 0);
}

TEST_CASE("cartolina: su una partita vera nessun momento saliente resta senza figurina, anche senza fotografie" * doctest::skip(!fs::exists(M688_MISSIONI "/prove/PROVA_S5_CONVOGLIO.lua"))) {
    const auto classi = carica_classi(M688_DATI "/classi.toml");
    Missione m(M688_MISSIONI "/prove/PROVA_S5_CONVOGLIO.lua", 1);
    DanniMissione dm(classi, M688_DATI, 1, RegolaDanni::simulazione);
    const auto ha_affondamento = [&] {
        for (const auto& e : m.eventi_missione())
            if (e.tipo == TipoEvento::affondamento) return true;
        return false;
    };
    for (int k = 0; k < 1500 * 9 && !ha_affondamento(); ++k) {
        m.avanza(1.0 / 9.0);
        dm.avanza(m, 1.0 / 9.0);
        m.raccogli_danni(nullptr, &dm);
    }
    REQUIRE(ha_affondamento());
    int riserve = 0;
    for (const auto& e : m.eventi_missione()) {
        if (e.tipo == TipoEvento::affondamento) CHECK(figurina_evento(e, nullptr) == Figurina::scena);
        riserve += figurina_evento(e, nullptr) == Figurina::riserva;
    }
    MESSAGE("eventi: ", m.eventi_missione().size(), ", di riserva: ", riserve);
    for (const EventoMissione* e : m.momenti_salienti()) {
        CAPTURE(nome_evento(e->tipo));
        CHECK(figurina_evento(*e, nullptr) != Figurina::riserva);
    }
}

