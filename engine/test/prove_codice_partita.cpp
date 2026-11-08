#include <doctest.h>

#include <filesystem>
#include <fstream>

#include "codice_partita.hpp"
#include "missioni/missione_rapida.hpp"

using namespace m688;
namespace fs = std::filesystem;

TEST_CASE("codice partita: andata e ritorno, forma, letture tolleranti") {
    CodicePartita c;
    c.origine = OrigineCodice::libreria;
    c.impronta = 0xDEADBEEFu;
    c.seme = 0x01234567u;
    c.anni_89 = true;
    c.carta_epoca = false;
    c.meteo_realistico = true;
    c.asw = 3;
    c.sonarista = 0;
    const std::string s = scrivi_codice_partita(c);
    REQUIRE(s.size() == 20);
    CHECK(s[6] == '-');
    CHECK(s[13] == '-');
    CHECK(leggi_codice_partita(s) == c);
    // minuscole, senza trattini, con O e I al posto di 0 e 1
    std::string m;
    for (const char x : s)
        if (x != '-') m += static_cast<char>(x == '0' ? 'o' : x == '1' ? 'i' : std::tolower(static_cast<unsigned char>(x)));
    CHECK(leggi_codice_partita(m) == c);
    // ogni combinazione delle opzioni e delle origini torna uguale
    for (int o = 0; o < 4; ++o)
        for (int a = 0; a < 4; ++a)
            for (int k = 0; k < 8; ++k) {
                CodicePartita x = c;
                x.origine = static_cast<OrigineCodice>(o);
                x.asw = a;
                x.sonarista = 3 - a;
                x.anni_89 = k & 1;
                x.carta_epoca = k & 2;
                x.meteo_realistico = k & 4;
                x.navigazione = (k + o) & 1;
                x.senza_traffico = (k >> 1) & 1;
                x.gps = (k + a) & 1;
                x.seme = 0x7FFFFFFFu - static_cast<std::uint32_t>(o * 97 + k);   // versione 2: 31 bit di seme
                CHECK(leggi_codice_partita(scrivi_codice_partita(x)) == x);
            }
}

TEST_CASE("codice partita: i codici della versione 1 si leggono ancora, senza GPS") {
    // un codice v1 vero (SCENE1A, dal briefing del 28/10/2026): seme a 32 bit e nessun bit del GPS
    const auto c = leggi_codice_partita("4XGQT4-8R0000-02K3QN");
    REQUIRE(c);
    CHECK_FALSE(c->gps);
    CHECK(c->origine == OrigineCodice::originale);
    // riscritto esce in versione 2, con lo stesso seme se sta nei 31 bit
    if (c->seme <= 0x7FFFFFFFu) CHECK(leggi_codice_partita(scrivi_codice_partita(*c)) == *c);
}

TEST_CASE("codice partita: un carattere sbagliato o due scambiati non passano, e non si confonde con le rapide") {
    CodicePartita c;
    c.impronta = 0x13579BDFu;
    c.seme = 42;
    const std::string s = scrivi_codice_partita(c);
    const std::string alfabeto = "0123456789ABCDEFGHJKMNPQRSTVWXYZ";
    for (std::size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '-') continue;
        for (const char a : alfabeto) {
            if (a == s[i]) continue;
            std::string e = s;
            e[i] = a;
            CHECK_FALSE(leggi_codice_partita(e));
        }
        std::size_t j = i + 1;
        if (j < s.size() && s[j] == '-') ++j;
        if (j < s.size() && s[i] != s[j]) {
            std::string e = s;
            std::swap(e[i], e[j]);
            CHECK_FALSE(leggi_codice_partita(e));
        }
    }
    CHECK_FALSE(leggi_codice_partita(""));
    CHECK_FALSE(leggi_codice_partita(s.substr(0, 13)));
    // il codice di una rapida non e' un codice partita, e viceversa
    ParametriRapida p;
    p.seme = 7;
    CHECK_FALSE(leggi_codice_partita(codice_missione(p)));
    CHECK_FALSE(leggi_codice(s));
}

TEST_CASE("codice partita: l'impronta trova la missione e cambia col file") {
    const fs::path d = fs::temp_directory_path() / "m688_prova_codice_partita";
    fs::remove_all(d);
    fs::create_directories(d / "a");
    fs::create_directories(d / "b");
    std::ofstream(d / "a" / "UNO.lua") << "return { id = 'UNO' }\n";
    std::ofstream(d / "b" / "DUE.lua") << "return { id = 'DUE' }\n";
    const std::uint32_t uno = impronta_missione("UNO", d / "a" / "UNO.lua"), due = impronta_missione("DUE", d / "b" / "DUE.lua");
    CHECK(uno != 0);
    CHECK(uno != due);
    CHECK(impronta_missione("ALTRO", d / "a" / "UNO.lua") != uno);   // l'id conta
    CHECK(impronta_missione("UNO", d / "manca.lua") == 0);
    const auto t = trova_missione(due, {d / "a", d / "b"});
    REQUIRE(t);
    CHECK(t->id == "DUE");
    CHECK(t->file == d / "b" / "DUE.lua");
    std::ofstream(d / "b" / "DUE.lua") << "return { id = 'DUE', cambiata = true }\n";   // la missione cambia
    CHECK_FALSE(trova_missione(due, {d / "a", d / "b"}));
    std::ofstream(d / "a" / "UNO.lua", std::ios::binary) << "return { id = 'UNO' }\r\n";   // i ritorni di Windows no
    CHECK(impronta_missione("UNO", d / "a" / "UNO.lua") == uno);
    fs::remove_all(d);
    // i semi nuovi non sono mai 0 e cambiano
    const std::uint32_t s1 = seme_nuovo(), s2 = seme_nuovo();
    CHECK(s1 != 0);
    CHECK(s1 != s2);
    CHECK(s1 <= 0x7FFFFFFFu);   // stanno nei 31 bit del codice partita
    CHECK(s2 <= 0x7FFFFFFFu);
}

TEST_CASE("codice sandbox: andata e ritorno con le regolazioni, distinto dal codice partita") {
    CodiceSandbox k;
    k.partita.origine = OrigineCodice::originale;
    k.partita.impronta = 0x2468ACE0u;
    k.partita.seme = 987654321u;
    k.partita.meteo_realistico = true;
    k.partita.navigazione = true;
    RegolazioniSandbox& r = k.regolazioni;
    r.nato = {SituazioneScelta::guerra, RoeScelta::libere};
    r.patto = {SituazioneScelta::crisi, RoeScelta::trattenute};
    r.neutrali = {SituazioneScelta::pace, RoeScelta::missione};
    r.nato.livello = LivelloScelta::esperto;
    r.patto.allerta = 3;
    r.neutrali.carico_per_classe = true;
    r.durata_min = 240;
    r.ferma_all_esito = false;
    r.difesa = false;
    r.sonaristi_ia = false;
    r.ferma_eventi = true;
    r.traffico = false;
    k.partita.senza_traffico = true;   // la lettura lo rimette dalle regolazioni
    const std::string s = scrivi_codice_sandbox(k);
    REQUIRE(s.size() == 34);   // 30 caratteri e quattro trattini
    const auto l = leggi_codice_sandbox(s);
    REQUIRE(l);
    CHECK(l->partita == k.partita);
    CHECK(l->regolazioni.nato.situazione == SituazioneScelta::guerra);
    CHECK(l->regolazioni.nato.roe == RoeScelta::libere);
    CHECK(l->regolazioni.patto.situazione == SituazioneScelta::crisi);
    CHECK(l->regolazioni.patto.roe == RoeScelta::trattenute);
    CHECK(l->regolazioni.neutrali.roe == RoeScelta::missione);
    CHECK(l->regolazioni.nato.livello == LivelloScelta::esperto);
    CHECK(l->regolazioni.ferma_eventi);
    CHECK_FALSE(l->regolazioni.traffico);
    CHECK(l->regolazioni.patto.allerta == 3);
    CHECK(l->regolazioni.nato.allerta == -1);
    CHECK(l->regolazioni.neutrali.carico_per_classe);
    CHECK(l->regolazioni.durata_min == 240);
    CHECK_FALSE(l->regolazioni.ferma_all_esito);
    CHECK_FALSE(l->regolazioni.difesa);
    CHECK(l->regolazioni.armi);
    CHECK_FALSE(l->regolazioni.sonaristi_ia);
    CHECK(l->regolazioni.meteo_realistico);
    CHECK(l->regolazioni.navigazione_realistica);
    REQUIRE(l->regolazioni.seme);
    CHECK(*l->regolazioni.seme == 987654321u);
    // non si confonde con gli altri codici, e un carattere sbagliato non passa
    CHECK_FALSE(leggi_codice_partita(s));
    CHECK_FALSE(leggi_codice_sandbox(scrivi_codice_partita(k.partita)));
    std::string e = s;
    e[3] = e[3] == 'A' ? 'B' : 'A';
    CHECK_FALSE(leggi_codice_sandbox(e));
}
