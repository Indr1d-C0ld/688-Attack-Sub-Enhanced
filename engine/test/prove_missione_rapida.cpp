#include <doctest.h>

#include <cmath>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>

#include "missioni/danni_missione.hpp"
#include "missioni/danni_sottomarini.hpp"
#include "missioni/missione.hpp"
#include "missioni/missione_rapida.hpp"
#include "nucleo/casuale.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/fondale.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Missioni rapide procedurali (roadmap punto 10, missioni/missione_rapida.hpp): il codice missione, il determinismo,
// la compatibilita' fra teatri e tipi, i file generati caricati e simulati nei due livelli, la chiusura e la carriera

namespace {

const std::string DATI = M688_DATI;
bool ci_sono_dati() { return fs::exists(DATI + "/missioni_rapide.toml") && fs::exists(DATI + "/teatri/faroe.fdl"); }

std::string scrivi(const MissioneRapida& m) {
    const auto f = fs::temp_directory_path() / (m.id + ".lua");
    std::ofstream(f, std::ios::binary) << m.lua;
    return f.string();
}

// parametri a caso, tutti i campi (anche quelli «a caso»), per le prove del codice
ParametriRapida a_caso(Casuale& r) {
    ParametriRapida p;
    p.seme = static_cast<std::uint32_t>(r.prossimo());
    const int t = static_cast<int>(r.prossimo() % 8);
    p.teatro = t == 0 ? "" : TEATRI_RAPIDE[t - 1];
    p.stagione = static_cast<StagioneRapida>(r.prossimo() % 5);
    p.ora = static_cast<MomentoRapido>(r.prossimo() % 5);
    p.meteo_realistico = r.prossimo() & 1u;
    p.tipo = static_cast<TipoRapida>(r.prossimo() % 8);
    p.alfa = r.prossimo() & 1u;
    p.forze = static_cast<int>(r.prossimo() % 4);
    p.paranoia = static_cast<int>(r.prossimo() % 4);
    p.difficolta = static_cast<int>(r.prossimo() % 6);
    p.asw_aereo = r.prossimo() & 1u;
    p.carriera = r.prossimo() & 1u;
    return p;
}

// Un seme per tipo, con il teatro e la parte a caso fra quelli compatibili
ParametriRapida per_tipo(TipoRapida tipo, std::uint32_t seme) {
    ParametriRapida p;
    p.seme = seme;
    p.tipo = tipo;
    p.alfa = (seme % 3) == 1;
    p.paranoia = static_cast<int>(seme % 4);
    p.difficolta = 0;
    p.forze = 0;
    p.asw_aereo = (seme % 5) != 0;
    p.meteo_realistico = (seme % 2) == 0;
    return p;   // il teatro a caso fra quelli dove il tipo si gioca con quella parte
}

// Partita dell'Enhanced come nelle altre prove delle missioni: missione, danni con la regola di simulazione (che accende
// acustica, dinamica, meteo, radio, ESM, ASW aereo e periscopi) e danni dei sottomarini
struct Enhanced {
    std::map<std::string, ClasseNave> classi = carica_classi(DATI + "/classi.toml");
    Missione m;
    DanniMissione dm;
    DanniSottomarini ds;
    Enhanced(const std::string& file, std::uint64_t seme)
        : m(file, seme), dm(classi, DATI, seme, RegolaDanni::simulazione), ds(classi, DATI, seme) {
        std::map<std::string, double> pescaggi;
        for (const auto& [id, c] : classi) pescaggi[id] = c.pescaggio_m;
        m.imposta_fondale(std::make_shared<const Fondale>(Fondale::carica(DATI + "/teatri/" + m.teatro() + ".fdl")), pescaggi);
        if (const auto* g = m.giocatore()) ds.prepara(m, g->id);
    }
    void avanza(double s) {
        constexpr double DT = 1.0 / 9.0;
        for (long i = 0; i < static_cast<long>(s * 9.0); ++i) {
            m.avanza(DT);
            dm.avanza(m, DT);
            ds.avanza(m, DT);
            m.raccogli_danni(&ds, &dm);
        }
    }
};

void controlla_caricata(const MissioneRapida& r, Missione& m) {
    CAPTURE(r.codice);
    CAPTURE(nome_tipo_rapida(r.parametri.tipo));
    CAPTURE(r.parametri.teatro);
    REQUIRE(m.giocatore() != nullptr);
    CHECK(m.id() == r.id);
    CHECK(m.teatro() == r.parametri.teatro);
    CHECK(m.errori().empty());
    CHECK(m.non_implementate().empty());
    CHECK(m.gestori_giocatore().count("telegrafo") == 1);
    // ogni nave e sottomarino in acqua sul fondale vero (l'aereo vola)
    static std::map<std::string, Fondale> fondali;   // uno per teatro, per tutte le prove
    auto it = fondali.find(r.parametri.teatro);
    if (it == fondali.end()) it = fondali.emplace(r.parametri.teatro, Fondale::carica(DATI + "/teatri/" + r.parametri.teatro + ".fdl")).first;
    const Fondale& f = it->second;
    for (const auto& [id, u] : m.unita()) {
        if (u.categoria != "nave" && u.categoria != "sottomarino") continue;
        CAPTURE(id);
        CAPTURE(u.classe);
        CHECK(f.profondita(u.pos) >= (u.categoria == "sottomarino" ? 20.0 : 8.0));
    }
    // obiettivi della missione, non quelli generici (sopravvivere)
    CHECK(m.esito_dettagliato().obiettivi.size() >= 2);
}

}  // namespace

TEST_CASE("missione rapida: il codice va e torna, maiuscole o minuscole, con O, I e L di Crockford") {
    Casuale r(688);
    for (int i = 0; i < 500; ++i) {
        const ParametriRapida p = a_caso(r);
        const std::string c = codice_missione(p);
        REQUIRE(c.size() == 15);
        CHECK(c[4] == '-');
        CHECK(c[9] == '-');
        const auto q = leggi_codice(c);
        REQUIRE(q);
        CHECK(*q == p);
        std::string piccolo;
        for (const char x : c) piccolo += static_cast<char>(x >= 'A' && x <= 'Z' ? x - 'A' + 'a' : x);
        CHECK(leggi_codice(piccolo) == q);
        std::string compatto, ambiguo;
        for (const char x : c)
            if (x != '-') {
                compatto += x;
                ambiguo += x == '0' ? 'O' : x == '1' ? (i % 2 ? 'I' : 'l') : x;
            }
        CHECK(leggi_codice(compatto) == q);
        CHECK(leggi_codice(ambiguo) == q);
    }
    CHECK_FALSE(leggi_codice(""));
    CHECK_FALSE(leggi_codice("7KQ2-M9A3-PX4F"));      // corto
    CHECK_FALSE(leggi_codice("7KQ2-M9A3-PX4FUU"));    // U non e' nell'alfabeto
}

TEST_CASE("missione rapida: il controllo scarta ogni carattere sbagliato e ogni scambio di due vicini") {
    Casuale r(1989);
    const std::string alfabeto = "0123456789ABCDEFGHJKMNPQRSTVWXYZ";
    for (int i = 0; i < 40; ++i) {
        std::string c;
        for (const char x : codice_missione(a_caso(r)))
            if (x != '-') c += x;
        for (std::size_t k = 0; k < c.size(); ++k) {
            for (const char x : alfabeto) {
                if (x == c[k]) continue;
                std::string s = c;
                s[k] = x;
                CHECK_FALSE(leggi_codice(s));
            }
            if (k + 1 < c.size() && c[k] != c[k + 1]) {
                std::string s = c;
                std::swap(s[k], s[k + 1]);
                CHECK_FALSE(leggi_codice(s));
            }
        }
    }
}

TEST_CASE("missione rapida: stesso codice, stesso file byte per byte; semi diversi, missioni diverse" * doctest::skip(!ci_sono_dati())) {
    ParametriRapida p;
    p.seme = 4242;
    const auto a = genera_missione_rapida(p, DATI);
    const auto b = genera_missione_rapida(*leggi_codice(a.codice), DATI);
    CHECK(a.lua == b.lua);
    CHECK(a.id == "RAPIDA_" + a.codice.substr(0, 4) + a.codice.substr(5, 4) + a.codice.substr(10, 5));
    CHECK(a.parametri == risolvi(p, DATI));
    CHECK(risolvi(a.parametri, DATI) == a.parametri);   // gia' risolti: non cambiano
    CHECK(a.parametri.tipo != TipoRapida::a_caso);
    CHECK_FALSE(a.parametri.teatro.empty());
    std::set<std::string> testi;
    for (std::uint32_t s = 1; s <= 20; ++s) {
        p.seme = s;
        testi.insert(genera_missione_rapida(p, DATI).lua);
    }
    CHECK(testi.size() == 20);
    // il file nomina il codice e la data, il briefing comincia con gli ordini
    CHECK(a.lua.find("codice_rapida = \"" + a.codice + "\"") != std::string::npos);
    REQUIRE(a.briefing_it.size() >= 6);
    CHECK(a.briefing_it[0] == "ORDINI DI MISSIONE");
    CHECK(a.briefing_en[0] == "MISSION ORDERS");
    CHECK(a.mese >= 1);
    CHECK(a.mese <= 12);
    CHECK(a.ora_utc >= 0.0);
    CHECK(a.ora_utc < 24.0);
}

TEST_CASE("missione rapida: i tipi si giocano dove il teatro li consente, gli altri sono rifiutati" * doctest::skip(!ci_sono_dati())) {
    int giocabili = 0;
    for (const char* t : TEATRI_RAPIDE)
        for (int k = 1; k <= 7; ++k)
            for (const bool alfa : {false, true}) {
                ParametriRapida p;
                p.seme = 17;
                p.teatro = t;
                p.tipo = static_cast<TipoRapida>(k);
                p.alfa = alfa;
                CAPTURE(t);
                CAPTURE(k);
                CAPTURE(alfa);
                if (compatibile(t, p.tipo, alfa, DATI)) {
                    ++giocabili;
                    const auto m = genera_missione_rapida(p, DATI);
                    Missione mis(scrivi(m), p.seme);
                    controlla_caricata(m, mis);
                } else {
                    CHECK_THROWS_AS(genera_missione_rapida(p, DATI), std::runtime_error);
                }
            }
    CHECK(giocabili >= 70);
    // il tipo esplicito con il teatro a caso sceglie un teatro dove si gioca
    for (std::uint32_t s = 0; s < 30; ++s) {
        ParametriRapida p;
        p.seme = s;
        p.tipo = TipoRapida::inserimento;
        const auto q = risolvi(p, DATI);
        CHECK(compatibile(q.teatro, q.tipo, q.alfa, DATI));
    }
}

TEST_CASE("missione rapida: 50 semi per tipo, caricati e simulati 3 minuti nel classico senza errori" * doctest::skip(!ci_sono_dati())) {
    for (int k = 1; k <= 7; ++k)
        for (std::uint32_t s = 1; s <= 50; ++s) {
            const ParametriRapida p = per_tipo(static_cast<TipoRapida>(k), s * 7919u + static_cast<std::uint32_t>(k));
            const auto r = genera_missione_rapida(p, DATI);
            Missione m(scrivi(r), p.seme);
            controlla_caricata(r, m);
            for (int i = 0; i < 90; ++i) m.avanza(2.0);
            CAPTURE(r.codice);
            CHECK(m.errori().empty());
            CHECK(m.non_implementate().empty());
            fs::remove(fs::temp_directory_path() / (r.id + ".lua"));
        }
}

TEST_CASE("missione rapida: alcuni semi per tipo nell'Enhanced, 2 minuti senza errori" * doctest::skip(!ci_sono_dati())) {
    for (int k = 1; k <= 7; ++k)
        for (std::uint32_t s = 1; s <= 2; ++s) {
            const ParametriRapida p = per_tipo(static_cast<TipoRapida>(k), s * 104729u + static_cast<std::uint32_t>(k));
            const auto r = genera_missione_rapida(p, DATI);
            Enhanced e(scrivi(r), p.seme);
            e.m.usa_meteo(r.parametri.meteo_realistico ? Missione::ModoMeteo::realistico : Missione::ModoMeteo::semplificato);
            e.avanza(120.0);
            CAPTURE(r.codice);
            CAPTURE(nome_tipo_rapida(r.parametri.tipo));
            CHECK(e.m.errori().empty());
            CHECK(e.m.non_implementate().empty());
            fs::remove(fs::temp_directory_path() / (r.id + ".lua"));
        }
}

// Lunga (circa un quarto d'ora): `prove -tc="*50 semi per tipo nell'Enhanced*" --no-skip`
TEST_CASE("missione rapida: 50 semi per tipo nell'Enhanced, 3 minuti senza errori" * doctest::skip(true)) {
    for (int k = 1; k <= 7; ++k)
        for (std::uint32_t s = 1; s <= 50; ++s) {
            const ParametriRapida p = per_tipo(static_cast<TipoRapida>(k), s * 7919u + static_cast<std::uint32_t>(k));
            const auto r = genera_missione_rapida(p, DATI);
            Enhanced e(scrivi(r), p.seme);
            e.avanza(180.0);
            CAPTURE(r.codice);
            CHECK(e.m.errori().empty());
            CHECK(e.m.non_implementate().empty());
            fs::remove(fs::temp_directory_path() / (r.id + ".lua"));
        }
}

TEST_CASE("missione rapida: chiusura dopo la zona (ricognizione, inserimento) e a tempo scaduto" * doctest::skip(!ci_sono_dati())) {
    const auto cerchio = [](const std::string& lua, const std::string& id) {
        const auto i = lua.find("{ id = \"" + id + "\"");
        REQUIRE(i != std::string::npos);
        const auto c = lua.find("cerchio = { x = ", i);
        double x = 0, y = 0;
        REQUIRE(std::sscanf(lua.c_str() + c, "cerchio = { x = %lf, y = %lf", &x, &y) == 2);
        return std::pair<double, double>{x, y};
    };
    {   // ricognizione: nella zona d'osservazione, poi fuori di 12 miglia: esito 20
        ParametriRapida p;
        p.seme = 5;
        p.teatro = "kunashir";
        p.tipo = TipoRapida::ricognizione;
        const auto r = genera_missione_rapida(p, DATI);
        Missione m(scrivi(r), p.seme);
        const auto [x, y] = cerchio(r.lua, "osservazione");
        REQUIRE(m.comando("posizione", {x, y}));
        for (int i = 0; i < 30 && !m.esito(); ++i) m.avanza(2.0);
        CHECK_FALSE(m.esito());
        REQUIRE(m.comando("posizione", {x + 40000.0, y}));
        for (int i = 0; i < 30 && !m.esito(); ++i) m.avanza(2.0);
        REQUIRE(m.esito());
        CHECK(*m.esito() == 20);
        CHECK(m.testo_esito(20).find("Missione conclusa") == 0);
    }
    {   // inserimento: allo sbarco, poi all'esfiltrazione
        ParametriRapida p;
        p.seme = 9;
        p.teatro = "baltico";
        p.tipo = TipoRapida::inserimento;
        const auto r = genera_missione_rapida(p, DATI);
        CHECK(r.lua.find("ospiti = 6") != std::string::npos);
        Missione m(scrivi(r), p.seme);
        const auto [xs, ys] = cerchio(r.lua, "sbarco");
        const auto [xe, ye] = cerchio(r.lua, "esfiltrazione");
        REQUIRE(m.comando("posizione", {xe, ye}));   // l'esfiltrazione prima dello sbarco non chiude
        for (int i = 0; i < 30; ++i) m.avanza(2.0);
        CHECK_FALSE(m.esito());
        REQUIRE(m.comando("posizione", {xs, ys}));
        for (int i = 0; i < 30; ++i) m.avanza(2.0);
        REQUIRE(m.comando("posizione", {xe, ye}));
        for (int i = 0; i < 30 && !m.esito(); ++i) m.avanza(2.0);
        REQUIRE(m.esito());
        CHECK(*m.esito() == 20);
    }
    {   // pattuglia in tempo di pace, giocatore fermo: si chiude a tempo scaduto (o quando i transiti sono usciti)
        ParametriRapida p;
        p.seme = 3;
        p.teatro = "faroe";
        p.tipo = TipoRapida::pattuglia;
        p.asw_aereo = false;
        const auto r = genera_missione_rapida(p, DATI);
        Missione m(scrivi(r), p.seme);
        for (int i = 0; i < 6 * 1800 && !m.esito(); ++i) m.avanza(2.0);
        REQUIRE(m.esito());
        CHECK((*m.esito() == 20 || *m.esito() == 21));
        CHECK(m.errori().empty());
    }
}

TEST_CASE("missione rapida: la carriera solo se chiesta, con difficolta' 3 e paranoia 1 o piu'" * doctest::skip(!ci_sono_dati())) {
    ParametriRapida p;
    p.seme = 77;
    p.teatro = "barents";
    p.tipo = TipoRapida::caccia;
    for (const bool chiesta : {false, true})
        for (int d = 1; d <= 5; ++d)
            for (int par = 0; par <= 3; ++par) {
                p.carriera = chiesta;
                p.difficolta = d;
                p.paranoia = par;
                const auto r = genera_missione_rapida(p, DATI);
                const bool conta = chiesta && d >= 3 && par >= 1;
                CAPTURE(chiesta);
                CAPTURE(d);
                CAPTURE(par);
                CHECK(r.conta_carriera == conta);
                Missione m(scrivi(r), p.seme);
                CHECK(m.blocco_carriera().has_value() == conta);
                if (conta) {
                    CHECK(m.blocco_carriera()->difficolta == d);
                    CHECK(m.blocco_carriera()->artico);
                }
                const std::string riga = conta ? "La missione conta" : "Missione fuori carriera";
                CHECK(r.briefing_it.back().find(riga) == 0);
            }
}

TEST_CASE("missione rapida: la cartella accanto ai fascicoli") {
    CHECK(cartella_missioni_rapide().filename() == "missioni_rapide");
}

// La versione 2 del generatore (29/10/2026): R_nave lascia le navi di superficie. Il codice dice la versione; con gli
// stessi parametri la missione e' la stessa (il caso non dipende dalla versione) e i due file differiscono solo nello
// script di R_nave. I codici della 1 si leggono ancora e rigenerano la 1
TEST_CASE("missione rapida: la versione 2 nel codice, la stessa missione con R_nave corretto, la 1 ancora leggibile" * doctest::skip(!ci_sono_dati())) {
    ParametriRapida p;
    p.seme = 4242;
    p.teatro = "gibilterra";
    p.tipo = TipoRapida::pattuglia;
    CHECK(p.versione == VERSIONE_RAPIDE);
    CHECK(VERSIONE_RAPIDE == 2);
    ParametriRapida p1 = p;
    p1.versione = 1;
    const auto a2 = genera_missione_rapida(p, DATI);
    const auto a1 = genera_missione_rapida(p1, DATI);
    CHECK(a1.codice != a2.codice);
    REQUIRE(leggi_codice(a1.codice));
    CHECK(leggi_codice(a1.codice)->versione == 1);
    CHECK(leggi_codice(a2.codice)->versione == 2);
    CHECK(genera_missione_rapida(*leggi_codice(a1.codice), DATI).lua == a1.lua);
    // le righe diverse: il codice nel file e lo script di R_nave
    const auto righe = [](const std::string& s) {
        std::vector<std::string> v;
        std::istringstream in(s);
        for (std::string r; std::getline(in, r);) v.push_back(r);
        return v;
    };
    const auto r1 = righe(a1.lua), r2 = righe(a2.lua);
    REQUIRE(r1.size() == r2.size());
    int diverse = 0;
    bool r_nave = false;
    for (std::size_t i = 0; i < r1.size(); ++i) {
        if (r1[i] == r2[i]) continue;
        ++diverse;
        r_nave = r_nave || r2[i] == "  if m688.bersaglio_aereo(u) or m688.bersaglio_nave(u) then goto ignora end";
        const bool atteso = r1[i].find(a1.codice) != std::string::npos || r1[i].find(a1.id) != std::string::npos ||
                            r1[i].find("versione") != std::string::npos || r1[i].find("nave da guerra") != std::string::npos ||
                            r1[i].find("ATTACK") != std::string::npos || r1[i].find("bersaglio_aereo") != std::string::npos;
        CHECK_MESSAGE(atteso, (r1[i] + " / " + r2[i]));
    }
    CHECK(r_nave);
    CHECK(diverse < 20);
}
