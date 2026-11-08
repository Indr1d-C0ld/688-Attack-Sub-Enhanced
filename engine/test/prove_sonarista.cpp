// Il sonarista dell'IA (S5d): ritardi, classificazione, tracce perse e ritrovate, errori secondo l'esperienza.
#include <doctest.h>

#include <algorithm>

#include <cmath>

#include "nucleo/sonarista.hpp"

using namespace m688;

namespace {

const CatalogoAcustico& catalogo() {
    static const CatalogoAcustico c = carica_acustica(M688_DATI "/acustica.toml");
    return c;
}

std::vector<RigaOsservata> righe_di(const char* id) {
    std::vector<RigaOsservata> v;
    const auto voce = voce_libreria(catalogo(), id);
    for (const auto& r : voce->righe) v.push_back({r.hz, 10.0 + r.db_relativo});
    return v;
}

AscoltoSonar ascolto(int id, double rilevamento, double se, const std::vector<RigaOsservata>& righe = {}, bool bio = false) {
    AscoltoSonar a;
    a.sorgente = id;
    a.rilevamento = rilevamento;
    a.se_db = se;
    a.errore_deg = 1.0;
    a.righe = righe;
    a.biologico = bio;
    return a;
}

// quanti operatori su `n` (semi diversi) sbagliano la classificazione del Charlie
double errori(Esperienza e, int n) {
    int sbagli = 0;
    const auto righe = righe_di("charlie2");
    for (int s = 1; s <= n; ++s) {
        Sonarista op(e, &catalogo());
        Casuale caso(static_cast<std::uint64_t>(s));
        for (double t = 0.0; t <= 200.0; t += 5.0) op.passo(t, {ascolto(7, 120.0, 10.0, righe)}, caso);
        REQUIRE(op.traccia_di(7) != nullptr);
        sbagli += op.traccia_di(7)->errata;
    }
    return static_cast<double>(sbagli) / n;
}

}  // namespace

TEST_CASE("sonarista: ritardo, Sierra, classificazione, traccia persa e ritrovata") {
    Sonarista op(Esperienza::medio, &catalogo());
    Casuale caso(1);
    const auto righe = righe_di("charlie2");
    std::vector<RapportoSonar> tutti;
    for (double t = 0.0; t <= 600.0; t += 5.0) {
        const bool c_e = t < 300.0 || t >= 500.0;   // il segnale sparisce fra 300 e 500 s
        std::vector<AscoltoSonar> a;
        if (c_e) a.push_back(ascolto(7, 245.0, 8.0, righe));
        for (auto& r : op.passo(t, a, caso)) tutti.push_back(r);
    }
    REQUIRE(tutti.size() == 4);
    CHECK(tutti[0].tipo == RapportoSonar::Tipo::nuovo);
    CHECK(tutti[0].t == doctest::Approx(30.0));   // il ritardo dell'operatore medio
    CHECK(tutti[0].testo_it.find("designato Sierra 1") != std::string::npos);
    CHECK(tutti[0].testo_it.find("Comandante, sonar:") == 0);
    CHECK(tutti[1].tipo == RapportoSonar::Tipo::classificato);
    CHECK(tutti[1].t == doctest::Approx(75.0));   // piu' i 45 s d'analisi
    CHECK(tutti[2].tipo == RapportoSonar::Tipo::perso);
    CHECK(tutti[2].t == doctest::Approx(420.0));  // oltre i 120 s di tenuta dall'ultima volta (295 s)
    CHECK(tutti[3].tipo == RapportoSonar::Tipo::ritrovato);
    const TracciaSonar* x = op.traccia_di(7);
    REQUIRE(x != nullptr);
    CHECK(x->numero == 1);
    CHECK(x->stato == TracciaSonar::Stato::classificata);
    if (!x->errata) {
        CHECK(x->classe == "charlie2");
        CHECK(x->confidenza >= Confidenza::probsub);
    }
    CHECK(std::fabs(normalizza_180(x->rilevamento - 245.0)) < 10.0);
}

TEST_CASE("sonarista: l'esperienza cambia soglia, ritardo ed errori") {
    // un segnale debole (2 dB): l'esperto se ne accorge, la recluta no
    const auto primo = [](Esperienza e, double se) {
        Sonarista op(e, &catalogo());
        Casuale caso(2);
        for (double t = 0.0; t <= 300.0; t += 5.0)
            for (const auto& r : op.passo(t, {ascolto(3, 10.0, se)}, caso))
                if (r.tipo == RapportoSonar::Tipo::nuovo) return r.t;
        return -1.0;
    };
    CHECK(primo(Esperienza::esperto, 2.0) == doctest::Approx(15.0));
    CHECK(primo(Esperienza::recluta, 2.0) < 0.0);
    CHECK(primo(Esperienza::recluta, 5.0) == doctest::Approx(60.0));
    // classificazioni sbagliate in proporzione (la seconda ipotesi della libreria)
    const double recluta = errori(Esperienza::recluta, 300), esperto = errori(Esperienza::esperto, 300);
    CHECK(recluta == doctest::Approx(0.30).epsilon(0.3));
    CHECK(esperto < 0.12);
    CHECK(recluta > esperto);
    // ripetibile dal seme
    CHECK(errori(Esperienza::medio, 50) == errori(Esperienza::medio, 50));
}

TEST_CASE("sonarista: biologici presi per sottomarini, tracce scambiate negli incroci") {
    // un biologico senza righe di classe: a volte diventa POSSUB (la recluta piu' spesso)
    const auto bio = [](Esperienza e) {
        int sub = 0;
        for (int s = 1; s <= 300; ++s) {
            Sonarista op(e, &catalogo());
            Casuale caso(static_cast<std::uint64_t>(s));
            for (double t = 0.0; t <= 200.0; t += 5.0) op.passo(t, {ascolto(5, 30.0, 8.0, {{20.0, 9.0}}, true)}, caso);
            sub += op.traccia_di(5)->errata;
        }
        return sub / 300.0;
    };
    CHECK(bio(Esperienza::recluta) > bio(Esperienza::esperto));
    // due sorgenti i cui rilevamenti si incrociano: lo scambio capita con la probabilita' dell'esperienza
    const auto scambi = [](Esperienza e) {
        int n = 0;
        for (int s = 1; s <= 200; ++s) {
            Sonarista op(e, &catalogo());
            Casuale caso(static_cast<std::uint64_t>(1000 + s));
            for (double t = 0.0; t <= 600.0; t += 5.0)
                op.passo(t, {ascolto(1, 80.0 + t / 30.0, 9.0), ascolto(2, 100.0 - t / 30.0, 9.0)}, caso);
            n += op.scambi();
        }
        return n / 200.0;
    };
    const double r = scambi(Esperienza::recluta), e = scambi(Esperienza::esperto);
    CHECK(r > e);
    CHECK(r == doctest::Approx(0.40).epsilon(0.35));
    CHECK(std::string(nome_esperienza(Esperienza::esperto)) == "esperto");
}

TEST_CASE("sonarista: una nave con una riga sola resta NONSUB; senza righe non si classifica") {
    // la turbina a 118 Hz di una Spruance vista sopra soglia, la rete no: niente classe, ma la natura e' di superficie
    const auto classifica = [](const std::vector<RigaOsservata>& righe) {
        Sonarista op(Esperienza::medio, &catalogo());
        Casuale caso(3);
        for (double t = 0.0; t <= 300.0; t += 5.0) op.passo(t, {ascolto(9, 60.0, 12.0, righe)}, caso);
        REQUIRE(op.traccia_di(9));
        return *op.traccia_di(9);
    };
    const TracciaSonar una = classifica({{118.0, 10.0}});
    CHECK(una.stato == TracciaSonar::Stato::classificata);
    CHECK(una.confidenza == Confidenza::nonsub);
    // senza righe: resta in analisi; con righe sotto la soglia del medio (3 dB) idem
    CHECK(classifica({}).stato == TracciaSonar::Stato::in_analisi);
    CHECK(classifica({{118.0, 2.0}}).stato == TracciaSonar::Stato::in_analisi);
    // l'esperto vede anche le righe da 2 dB
    Sonarista e(Esperienza::esperto, &catalogo());
    Casuale caso(3);
    for (double t = 0.0; t <= 300.0; t += 5.0) e.passo(t, {ascolto(9, 60.0, 12.0, {{118.0, 2.0}})}, caso);
    CHECK(e.traccia_di(9)->stato == TracciaSonar::Stato::classificata);
}

TEST_CASE("sonarista: il livello e il seme del sonarista dell'IA") {
    CHECK(esperienza_ia(0) == Esperienza::recluta);
    CHECK(esperienza_ia(1) == Esperienza::medio);
    CHECK(esperienza_ia(2) == Esperienza::esperto);
    CHECK(esperienza_ia(3) == Esperienza::esperto);
    CHECK(esperienza_ia(-1) == Esperienza::recluta);
    CHECK(esperienza_ia(0, "esperto") == Esperienza::esperto);   // la chiave d'unita' vince
    CHECK(esperienza_ia(3, "recluta") == Esperienza::recluta);
    CHECK(esperienza_ia(2, "boh") == Esperienza::esperto);       // sconosciuta: non conta
    CHECK(esperienza_ia(1, "") == Esperienza::medio);
    CHECK_FALSE(esperienza_da_nome("Medio").has_value());
    // semi distinti per unita' e per missione, stabili
    CHECK(seme_sonarista_ia(7, 1) == seme_sonarista_ia(7, 1));
    CHECK(seme_sonarista_ia(7, 1) != seme_sonarista_ia(7, 2));
    CHECK(seme_sonarista_ia(7, 1) != seme_sonarista_ia(8, 1));
    Casuale a(seme_sonarista_ia(7, 1)), b(seme_sonarista_ia(7, 2));
    CHECK(a.prossimo() != b.prossimo());
}

TEST_CASE("sonarista: le righe del proprio battello, tolte dall'esperto, lasciate in parte dalla recluta") {
    // un contatto debole di cui si sentono solo le righe del proprio 688 (blocco 2 del sonar, F)
    const auto proprie = righe_di("los_angeles");
    const auto prova = [&](Esperienza e) {
        Sonarista op(e, &catalogo());
        Casuale caso(3);
        AscoltoSonar a = ascolto(11, 200.0, 8.0, proprie);
        for (const auto& r : proprie) a.proprie.push_back(r.hz);
        for (double t = 0.0; t <= 300.0; t += 5.0) op.passo(t, {a}, caso);
        REQUIRE(op.traccia_di(11) != nullptr);
        return *op.traccia_di(11);
    };
    // l'esperto e il medio le tolgono tutte o quasi: niente classe dal proprio rumore
    const TracciaSonar esperto = prova(Esperienza::esperto);
    CHECK(esperto.stato == TracciaSonar::Stato::in_analisi);
    // la recluta ne lascia alcune e classifica il contatto col proprio rumore: l'errore del dossier 09 §3.9
    const TracciaSonar recluta = prova(Esperienza::recluta);
    MESSAGE("recluta: " << recluta.classe);
    CHECK(recluta.stato == TracciaSonar::Stato::classificata);
}

TEST_CASE("sonarista: la maschera delle righe proprie segue il seme della partita") {
    // le righe che la recluta lascia: le stesse con lo stesso seme, diverse fra partite diverse
    std::vector<double> hz;
    for (const auto& r : righe_di("los_angeles")) hz.push_back(r.hz);
    for (int i = 1; i <= 40; ++i) hz.push_back(37.3 * i);   // piu' righe: la prova non dipende dalla firma
    const auto lasciate = [&](std::uint64_t seme) {
        std::vector<double> v;
        for (double f : hz)
            if (!riga_propria_tolta(f, seme, 0.6)) v.push_back(f);
        return v;
    };
    CHECK(lasciate(11) == lasciate(11));
    int diverse = 0;
    for (std::uint64_t s = 1; s <= 10; ++s) diverse += lasciate(s) != lasciate(s + 100) ? 1 : 0;
    CHECK(diverse == 10);
    // circa il 40% lasciate, i casi limite
    std::size_t n = 0;
    for (std::uint64_t s = 1; s <= 50; ++s) n += lasciate(s).size();
    CHECK(static_cast<double>(n) / (50.0 * static_cast<double>(hz.size())) == doctest::Approx(0.4).epsilon(0.15));
    for (double f : hz) {
        CHECK(riga_propria_tolta(f, 5, 1.0));
        CHECK_FALSE(riga_propria_tolta(f, 5, 0.0));
    }
    // il Sonarista prende il seme al primo passo dal generatore della partita, senza spostarne la sequenza
    const auto seme_di = [](std::uint64_t seme_partita) {
        Sonarista op(Esperienza::recluta, &catalogo());
        Casuale caso(seme_partita), riferimento(seme_partita);
        op.passo(0.0, {}, caso);
        CHECK(caso.prossimo() == riferimento.prossimo());
        return op.seme_maschera();
    };
    CHECK(seme_di(3) == seme_di(3));
    CHECK(seme_di(3) != seme_di(4));
    CHECK(seme_di(3) != 0);
}

TEST_CASE("sonarista: la classificazione a orecchio e col DEMON quando non ci sono righe utili (passo B)") {
    // la regola sulle eliche delle classi tipiche (giri d'asse alla velocita' in cui cavitano)
    const auto classe = [](int pale, double asse) { return classifica_a_orecchio({LetturaDemon{asse, asse * pale, pale}}); };
    CHECK_FALSE(classifica_a_orecchio({}).valida);
    CHECK_FALSE(classe(5, 2.5).sottomarino);                    // Perry a 25 nodi
    CHECK(classe(5, 2.5).tipo_it == "nave da guerra veloce");
    CHECK(classe(4, 1.2).tipo_it == "mercantile");              // mercantile a 12 nodi
    CHECK_FALSE(classe(6, 1.1).sottomarino);                    // la cargo a sei pale gira piano
    CHECK(classe(5, 1.6).tipo_it.empty());                      // in mezzo: nave, senza tipo
    CHECK(classe(7, 2.5).sottomarino);                          // Los Angeles
    CHECK(classe(7, 1.0).sottomarino);                          // Typhoon a 15 nodi
    CHECK(classe(6, 2.0).sottomarino);                          // Kilo
    CHECK(classe(4, 2.67).sottomarino);                         // Victor III a 20 nodi: giri alti
    CHECK(classe(7, 2.5).tipo_it.empty());
    // il DEMON di una Perry a 25 nodi (l'innesco della cavitazione delle navi, solo per il DEMON) si legge: 5 pale
    UnitaAcustica u;
    u.id = 4; u.classe = "perry"; u.categoria = "nave"; u.velocita_kn = 25.0; u.pos = {0.0, 8000.0};
    const Sorgente s = sorgente_acustica(catalogo(), u);
    Ricevitore r{{0.0, 0.0}, 100.0, 0.0, 5.0, catalogo().sensore("sovietico_2gen")};
    Ambiente amb;
    amb.fondale_m = 3000.0;
    const auto serie = leggi_demon_fascio(demon_fascio({s}, r, amb, 0.0));
    REQUIRE_FALSE(serie.empty());
    CHECK(serie.front().pale == 5);
    CHECK_FALSE(classifica_a_orecchio(serie).sottomarino);
    // nel Sonarista: senza righe utili, dopo 1,5 volte l'analisi, la categoria e il tipo; mai la classe. Senza DEMON aspetta
    const auto prova = [&](bool col_demon, bool sottomarino_vero, std::uint64_t seme) {
        Sonarista op(Esperienza::medio, &catalogo());
        Casuale caso(seme);
        AscoltoSonar a = ascolto(4, 0.0, 10.0);
        if (col_demon) a.demon = serie;
        a.sottomarino = sottomarino_vero;
        std::vector<RapportoSonar> rapporti;
        for (double t = 0.0; t <= 200.0; t += 5.0)
            for (auto& x : op.passo(t, {a}, caso)) rapporti.push_back(x);
        return std::make_pair(*op.traccia_di(4), rapporti);
    };
    const auto [nave, rapporti] = prova(true, false, 2);   // col seme 1 il medio sbaglia la categoria (7,5%)
    CHECK(nave.stato == TracciaSonar::Stato::classificata);
    CHECK(nave.classe.empty());
    CHECK(nave.confidenza == Confidenza::nonsub);
    CHECK(nave.tipo == "nave da guerra veloce");
    CHECK_FALSE(nave.errata);
    CHECK(std::any_of(rapporti.begin(), rapporti.end(), [](const RapportoSonar& x) { return x.testo_it.find("a orecchio, nave da guerra veloce") != std::string::npos; }));
    CHECK(prova(false, false, 1).first.stato == TracciaSonar::Stato::in_analisi);   // senza DEMON aspetta, come prima
    // la categoria detta e' giusta quasi sempre (medio: meta' del 15%); se la sorgente era un sottomarino e' errata
    int sbagliate = 0;
    for (std::uint64_t seme = 1; seme <= 100; ++seme) sbagliate += prova(true, false, seme).first.errata ? 1 : 0;
    CHECK(sbagliate <= 15);
    CHECK(prova(true, true, 2).first.errata);
}

// La rivalutazione (il collaudo della libreria, 30/10/2026: un Alfa detto NONSUB a 76 s restava NONSUB finche' non si
// perdeva): ogni 300 s una traccia classificata con righe viste si riguarda, e cambia solo in meglio
TEST_CASE("sonarista: una traccia classificata si rivaluta ogni 5 minuti, solo in meglio") {
    // l'Alfa da lontano: una riga sola, la piu' debole che le navi hanno pure (la rete a 50 Hz), poi da vicino tutte
    const auto tutte = righe_di("alfa");
    REQUIRE(tutte.size() >= 2);
    std::vector<RigaOsservata> una;
    for (const auto& r : tutte)
        if (std::fabs(r.hz - 50.0) < 0.6) una.push_back(r);
    if (una.empty()) una.push_back(tutte.back());
    Sonarista op(Esperienza::esperto, &catalogo());
    Casuale caso(3);
    std::vector<RapportoSonar> rapporti;
    for (double t = 0.0; t <= 200.0; t += 5.0)
        for (auto& r : op.passo(t, {ascolto(5, 40.0, 8.0, una)}, caso)) rapporti.push_back(r);
    const TracciaSonar* x = op.traccia_di(5);
    REQUIRE(x != nullptr);
    REQUIRE(x->stato == TracciaSonar::Stato::classificata);
    const Confidenza prima = x->confidenza;
    const std::string classe_prima = x->classe;
    MESSAGE("da lontano: ", classe_prima.empty() ? std::string("senza classe") : classe_prima, ", confidenza ", static_cast<int>(prima));
    CHECK(classe_prima.empty());
    // per mezz'ora le righe tutte: alla prima rivalutazione l'Alfa con la sua classe, sottomarino
    for (double t = 205.0; t <= 1800.0; t += 5.0)
        for (auto& r : op.passo(t, {ascolto(5, 40.0, 15.0, tutte)}, caso)) rapporti.push_back(r);
    x = op.traccia_di(5);
    MESSAGE("dopo mezz'ora: ", x->classe, ", confidenza ", static_cast<int>(x->confidenza));
    CHECK(x->classe == "alfa");
    CHECK(static_cast<int>(x->confidenza) >= static_cast<int>(Confidenza::possub));
    const auto riclassificati = std::count_if(rapporti.begin(), rapporti.end(), [](const RapportoSonar& r) {
        return r.testo_it.find("riclassificato") != std::string::npos;
    });
    CHECK(riclassificati == 1);   // una volta: dopo, niente di meglio
    // e mai all'indietro: di nuovo una riga sola, la classe resta
    for (double t = 1805.0; t <= 3000.0; t += 5.0) op.passo(t, {ascolto(5, 40.0, 8.0, una)}, caso);
    CHECK(op.traccia_di(5)->classe == "alfa");
}

TEST_CASE("sonarista: la rivalutazione non tocca una nave classificata giusta") {
    const auto righe = righe_di("spruance");
    Sonarista op(Esperienza::medio, &catalogo());
    Casuale caso(2);
    int rapporti = 0;
    for (double t = 0.0; t <= 2400.0; t += 5.0)
        for (const auto& r : op.passo(t, {ascolto(9, 200.0, 12.0, righe)}, caso))
            rapporti += r.testo_it.find("riclassificato") != std::string::npos;
    const TracciaSonar* x = op.traccia_di(9);
    REQUIRE(x != nullptr);
    CHECK(x->confidenza == Confidenza::nonsub);
    CHECK(rapporti <= 1);   // al piu' la classe giusta dove c'era quella sbagliata
}

// i transitori dello scafo (31/10/2026; missione_scafo del runtime): un colpo metallico dice «sottomarino» subito
TEST_CASE("sonarista: un transitorio dello scafo da' POSSUB al contatto nuovo e PROBSUB alla traccia che c'e'") {
    // una traccia in analisi, senza righe: col transitorio diventa PROBSUB senza classe
    Sonarista op(Esperienza::medio, &catalogo());
    Casuale caso(5);
    for (double t = 0.0; t <= 120.0; t += 5.0) op.passo(t, {ascolto(21, 90.0, 9.0)}, caso);
    REQUIRE(op.traccia_di(21));
    CHECK(op.traccia_di(21)->stato == TracciaSonar::Stato::in_analisi);
    AscoltoSonar colpo = ascolto(21, 90.0, 9.0);
    colpo.transitorio_scafo = true;
    colpo.sottomarino = true;
    const auto r = op.passo(125.0, {colpo}, caso);
    CHECK(op.traccia_di(21)->stato == TracciaSonar::Stato::classificata);
    CHECK(op.traccia_di(21)->confidenza == Confidenza::probsub);
    CHECK(op.traccia_di(21)->classe.empty());
    CHECK_FALSE(op.traccia_di(21)->errata);
    CHECK(std::any_of(r.begin(), r.end(), [](const RapportoSonar& x) { return x.tipo == RapportoSonar::Tipo::classificato; }));
    // un secondo transitorio non abbassa ne' ripete
    const auto r2 = op.passo(130.0, {colpo}, caso);
    CHECK(op.traccia_di(21)->confidenza == Confidenza::probsub);
    CHECK(r2.empty());
    // un contatto nuovo con un transitorio sentito poco prima: POSSUB appena designato
    Sonarista nuovo(Esperienza::medio, &catalogo());
    Casuale c2(5);
    AscoltoSonar primo = ascolto(22, 200.0, 9.0);
    primo.transitorio_scafo = true;
    primo.sottomarino = true;
    nuovo.passo(0.0, {primo}, c2);
    for (double t = 5.0; t <= 120.0 && !nuovo.traccia_di(22); t += 5.0) nuovo.passo(t, {ascolto(22, 200.0, 9.0)}, c2);
    REQUIRE(nuovo.traccia_di(22));
    CHECK(nuovo.traccia_di(22)->stato == TracciaSonar::Stato::classificata);
    CHECK(nuovo.traccia_di(22)->confidenza == Confidenza::possub);
}

// il falso contatto del vetrino di Dio (docs/28 §3.5, D3): lo smascheramento, uguale per giocatore e IA
TEST_CASE("sonarista: il falso contatto, smascherato secondo la fedelta' e l'esperienza, e subito con l'eco d'esca") {
    const auto righe = righe_di("charlie2");
    const auto smascherati = [&](Esperienza e, double fedelta, int n) {
        int k = 0;
        for (int s = 1; s <= n; ++s) {
            Sonarista op(e, &catalogo());
            Casuale caso(static_cast<std::uint64_t>(s));
            AscoltoSonar a = ascolto(31, 120.0, 10.0, righe);
            a.falso = true;
            a.fedelta = fedelta;
            for (double t = 0.0; t <= 200.0; t += 5.0) op.passo(t, {a}, caso);
            REQUIRE(op.traccia_di(31));
            k += op.traccia_di(31)->tipo == "falso contatto";
        }
        return k;
    };
    CHECK(smascherati(Esperienza::esperto, 1.0, 20) == 0);   // la firma perfetta inganna tutti
    CHECK(smascherati(Esperienza::esperto, 0.3, 20) > 10);  // P 0,63
    CHECK(smascherati(Esperienza::recluta, 0.3, 20) < smascherati(Esperienza::esperto, 0.3, 20));
    // preso per vero: la classificazione e' errata; alla rivalutazione (300 s) si riprova
    {
        Sonarista op(Esperienza::medio, &catalogo());
        Casuale caso(2);
        AscoltoSonar a = ascolto(32, 60.0, 10.0, righe);
        a.falso = true;
        a.fedelta = 1.0;
        for (double t = 0.0; t <= 200.0; t += 5.0) op.passo(t, {a}, caso);
        REQUIRE(op.traccia_di(32));
        CHECK(op.traccia_di(32)->errata);
        CHECK(op.traccia_di(32)->tipo != "falso contatto");
        // un ping trova un'eco d'esca: smascherato subito
        a.eco_esca = true;
        const auto r = op.passo(205.0, {a}, caso);
        CHECK(op.traccia_di(32)->tipo == "falso contatto");
        CHECK(op.traccia_di(32)->confidenza == Confidenza::nonsub);
        CHECK_FALSE(op.traccia_di(32)->errata);
        CHECK_FALSE(r.empty());
        // non si ripete
        CHECK(op.passo(210.0, {a}, caso).empty());
    }
    // l'eco d'esca smaschera anche un contatto creduto NONSUB senza classe (03/11/2026: il falso preso per nave a
    // orecchio, un mercantile a 4 pale e 1,2 Hz d'asse, restava creduto vero dopo il ping)
    {
        Sonarista op(Esperienza::medio, &catalogo());
        Casuale caso(2);   // col seme 3 il medio sbaglia la categoria a orecchio (7,5%)
        AscoltoSonar a = ascolto(33, 60.0, 10.0);
        a.falso = true;
        a.fedelta = 1.0;
        a.demon = {LetturaDemon{1.2, 4.8, 4}};
        for (double t = 0.0; t <= 200.0; t += 5.0) op.passo(t, {a}, caso);
        REQUIRE(op.traccia_di(33));
        REQUIRE(op.traccia_di(33)->stato == TracciaSonar::Stato::classificata);
        CHECK(op.traccia_di(33)->confidenza == Confidenza::nonsub);
        CHECK(op.traccia_di(33)->classe.empty());
        CHECK(op.traccia_di(33)->tipo != "falso contatto");
        a.eco_esca = true;
        op.passo(205.0, {a}, caso);
        CHECK(op.traccia_di(33)->tipo == "falso contatto");
        CHECK_FALSE(op.traccia_di(33)->errata);
    }
}

TEST_CASE("sonarista: il falso scadente, senza righe utili ne' DEMON, si smaschera in analisi; il vero no") {
    // nota del runtime del 03/11/2026 (§4.3): sotto fedelta' 0,7 il falso non aveva pale ne' righe e restava in analisi
    // per sempre, piu' sicuro di uno buono. Ora la prova della firma incoerente si fa dopo il tempo del passo B e poi
    // ogni 5 minuti
    const auto smascherati = [](Esperienza e, double fedelta, bool falso, double fino_s) {
        int k = 0;
        for (int s = 1; s <= 20; ++s) {
            Sonarista op(e, &catalogo());
            Casuale caso(static_cast<std::uint64_t>(s));
            AscoltoSonar a = ascolto(41, 120.0, 10.0);
            a.falso = falso;
            a.fedelta = fedelta;
            for (double t = 0.0; t <= fino_s; t += 5.0) op.passo(t, {a}, caso);
            REQUIRE(op.traccia_di(41));
            if (op.traccia_di(41)->tipo == "falso contatto") ++k;
            else CHECK(op.traccia_di(41)->stato == TracciaSonar::Stato::in_analisi);
        }
        return k;
    };
    CHECK(smascherati(Esperienza::medio, 0.5, true, 60.0) == 0);       // prima del tempo del passo B niente
    CHECK(smascherati(Esperienza::medio, 0.5, true, 3600.0) >= 15);    // P 0,3 ogni 5 minuti: in un'ora quasi tutti
    CHECK(smascherati(Esperienza::recluta, 0.5, true, 3600.0) <= smascherati(Esperienza::esperto, 0.5, true, 3600.0));
    CHECK(smascherati(Esperienza::esperto, 1.0, true, 3600.0) == 0);   // la firma perfetta, anche senza righe
    CHECK(smascherati(Esperienza::esperto, 0.5, false, 3600.0) == 0);  // un contatto vero senza righe resta in analisi
}
