// Traccia campionata del replay completo (nucleo/traccia_replay.hpp, missioni/traccia_missione.hpp; roadmap 16)
#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>

#include "missioni/missione.hpp"
#include "missioni/stazioni_missione.hpp"
#include "missioni/traccia_missione.hpp"
#include "nucleo/carriera.hpp"
#include "nucleo/fascicolo.hpp"
#include "nucleo/geometria.hpp"
#include "nucleo/traccia_replay.hpp"

using namespace m688;
namespace fs = std::filesystem;

namespace {

// 40 navi lente, 5 aerei, armi che entrano ed escono; 15 contatti; un evento ogni 25 s circa
Fotogramma sintetico(double t) {
    Fotogramma f;
    f.t = t;
    for (int i = 0; i < 40; ++i) {
        StatoUnita u;
        u.id = i;
        u.classe = i == 0 ? "los_angeles" : "krivak2";
        u.categoria = i == 0 ? "sottomarino" : "nave";
        // navi del Patto: le neutrali senza contatto si scriverebbero rade (PASSO_NEUTRALI_S), e la loro stima che
        // ondeggia non tornerebbe uguale nella vista dal vivo
        u.flag = i == 0 ? 0x08 : 0x10;
        u.rotta = std::fmod(10.0 * i + 0.5 * t / 60.0, 360.0);
        u.velocita_kn = 12.0;
        const double v = 12.0 * 0.5144;
        u.pos = {1000.0 * i + v * t * std::sin(u.rotta * PI / 180.0), v * t * std::cos(u.rotta * PI / 180.0)};
        u.quota = i == 0 ? 150.0 : 0.0;
        u.alberi = i == 0 && std::fmod(t, 600.0) < 60.0 ? 0x02 : 0;
        // versione 2: la stima che deriva piano dalla vera, l'ellisse che cresce, l'allerta
        u.pos_stimata = {u.pos.x + 40.0 * std::sin(t / 900.0 + i), u.pos.y + 25.0 * std::cos(t / 700.0 + i)};
        u.sigma_nav_m = 30.0 + 0.05 * std::fmod(t, 3600.0);
        u.allerta = 1;
        f.unita.push_back(u);
    }
    for (int i = 0; i < 5; ++i) {
        StatoUnita u;
        u.id = 100 + i;
        u.classe = "p3c";
        u.categoria = "aereo";
        u.rotta = std::fmod(t * 3.0 + 72.0 * i, 360.0);   // in giro
        u.velocita_kn = 300.0;
        u.pos = {20000.0 + 3000.0 * std::cos(t / 60.0 + i), 20000.0 + 3000.0 * std::sin(t / 60.0 + i)};
        u.quota = -300.0;
        f.unita.push_back(u);
    }
    const int ciclo = static_cast<int>(t / 300.0);   // un'arma in acqua per 4 minuti ogni 5
    if (std::fmod(t, 300.0) < 240.0) {
        StatoUnita a;
        a.id = 1000 + ciclo;
        a.classe = "mk48";
        a.categoria = "arma";
        a.pos = {50.0 * std::fmod(t, 300.0), 0.0};
        a.velocita_kn = 55.0;
        a.rotta = 90.0;
        a.quota = 100.0;
        a.lanciatore = 0;
        a.collegata = 0;
        f.unita.push_back(a);
    }
    for (int i = 1; i <= 15; ++i) {
        StatoContatto c;
        c.id = i;
        c.lettera = static_cast<char>('A' + i - 1);
        c.qualita = std::min(1500.0, t);
        c.durata = t / 2.0;
        c.rilevamento = std::fmod(20.0 * i + t / 30.0, 360.0);
        c.sensore = i % 2 ? "bqq5_sfera" : "tb16";
        c.classe_creduta = t > 600.0 ? "krivak2" : "";
        f.contatti.push_back(c);
    }
    for (double r = 0.0; r <= t; r += 30.0) f.rilevamenti.push_back({r, {0.0, 0.0}, std::fmod(r / 10.0, 360.0)});
    f.bersaglio_tma = 3;
    f.app.modo_sonar = "moderno";
    f.app.sensore = "bqq5_sfera";
    f.app.fascio = 45.0;
    f.app.soluzione = true;
    f.app.proposta = {9260.0, 120.0, 4.0};
    return f;
}

const StatoUnita* unita(const IstanteTraccia& i, int id) {
    for (const auto& u : i.unita)
        if (u.id == id) return &u;
    return nullptr;
}

std::vector<std::uint8_t> con_intestazione(const std::vector<std::uint8_t>& dati, const std::string& da, const std::string& a,
                                           const std::string& aggiunta = {}) {
    // rilegge l'intestazione, la cambia, la riscrive davanti allo stesso corpo
    std::size_t p = 7, k = 0;
    std::uint64_t n = 0;
    for (int s = 0;; s += 7) {
        const std::uint8_t c = dati[p++];
        n |= static_cast<std::uint64_t>(c & 0x7f) << s;
        if (!(c & 0x80)) break;
    }
    std::string testo(dati.begin() + static_cast<long>(p), dati.begin() + static_cast<long>(p + n));
    if (!da.empty() && (k = testo.find(da)) != std::string::npos) testo.replace(k, da.size(), a);
    testo += aggiunta;
    std::vector<std::uint8_t> out(dati.begin(), dati.begin() + 7);
    for (std::uint64_t v = testo.size(); ; v >>= 7) {
        if (v < 0x80) { out.push_back(static_cast<std::uint8_t>(v)); break; }
        out.push_back(static_cast<std::uint8_t>(v | 0x80));
    }
    out.insert(out.end(), testo.begin(), testo.end());
    out.insert(out.end(), dati.begin() + static_cast<long>(p + n), dati.end());
    return out;
}

}  // namespace

TEST_CASE("traccia: due ore con 50 unita' stanno in poco piu' di un mega; si rilegge e si interpola") {
    RegistratoreTraccia reg("SINTETICA", 42, -1.0);
    reg.metadato("comandante", "halloran");
    std::size_t eventi = 0;
    for (double t = 0.0; t <= 7200.0 + 1e-9; t += 1.0) {
        if (std::fmod(t, 25.0) == 0.0) ++eventi;
        if (!reg.serve(t, eventi)) continue;
        Fotogramma f = sintetico(t);
        f.evento = static_cast<int>(eventi) - 1;
        for (int e = reg.ultimo_evento() + 1; e < static_cast<int>(eventi); ++e) {
            EventoTraccia x;
            x.id = e;
            x.tempo = 25.0 * e;
            x.tipo = "scoperta";
            x.testo_it = "Contatto " + std::to_string(e);
            x.testo_en = "Contact " + std::to_string(e);
            x.interesse = e % 10 == 0 ? 80 : 20;
            x.noto = e % 3 != 0;
            x.unita = 7;
            f.eventi.push_back(std::move(x));
        }
        reg.aggiungi(f, eventi);
    }
    const auto dati = reg.dati();
    MESSAGE("traccia di 2 ore: " << dati.size() << " byte, " << reg.fotogrammi() << " fotogrammi");
    CHECK(dati.size() < 1650000);   // 1,63 MB: il record V (stima, ellisse intera, allerta) quando cambiano
    const Traccia tr = leggi_traccia(dati);
    CHECK(tr.versione == 2);
    CHECK(tr.missione == "SINTETICA");
    CHECK(tr.seme == 42);
    CHECK(tr.metadati.at("comandante") == "halloran");
    CHECK(tr.fotogrammi_saltati == 0);
    CHECK(tr.durata == doctest::Approx(7200.0));
    CHECK(tr.eventi.size() == eventi);
    // i segnalibri coi testi: tutti, i piu' interessanti, solo quelli noti allora
    const auto tutti = segnalibri(tr);
    REQUIRE(tutti.size() == eventi);
    CHECK(tutti[12].testo_it == "Contatto 12");
    CHECK(tutti[12].testo_en == "Contact 12");
    CHECK(tutti[12].tipo == "scoperta");
    CHECK(tutti[12].tempo == doctest::Approx(300.0));
    CHECK(tutti[12].unita == 7);
    CHECK(segnalibri(tr, 50).size() == (eventi + 9) / 10);
    CHECK(segnalibri(tr, 0, true).size() < eventi);
    // a un istante campionato: i valori quantizzati
    const Fotogramma vero = sintetico(3000.0);
    const IstanteTraccia a = tr.istante(3000.0);
    for (const int id : {0, 7, 39, 102}) {
        const StatoUnita* u = unita(a, id);
        REQUIRE(u);
        const auto& v = *std::find_if(vero.unita.begin(), vero.unita.end(), [&](const StatoUnita& x) { return x.id == id; });
        CHECK(std::fabs(u->pos.x - v.pos.x) <= 0.26);
        CHECK(std::fabs(u->pos.y - v.pos.y) <= 0.26);
        CHECK(std::fabs(normalizza_180(u->rotta - v.rotta)) <= 0.006);
        CHECK(std::fabs(u->quota - v.quota) <= 0.051);
        CHECK(u->classe == v.classe);
    }
    // fra due campioni: lineare; la nave fra 3000 e 3005, l'aereo campionato ogni 2 s
    const Fotogramma dopo = sintetico(3005.0);
    const StatoUnita* m = unita(tr.istante(3002.5), 7);
    REQUIRE(m);
    CHECK(m->pos.y == doctest::Approx((vero.unita[7].pos.y + dopo.unita[7].pos.y) / 2.0).epsilon(1e-4));
    CHECK(tr.unita.at(102).size() > tr.unita.at(7).size() * 2);
    // le armi: entrano ed escono
    CHECK(unita(tr.istante(100.0), 1000));
    CHECK_FALSE(unita(tr.istante(250.0), 1000));
    CHECK(unita(tr.istante(400.0), 1001));
    CHECK(unita(tr.istante(100.0), 1000)->collegata == 0);
    // il quadro, la TMA e l'app
    const IstanteTraccia b = tr.istante(3001.0);
    CHECK(b.contatti.size() == 15);
    CHECK(b.contatti.front().lettera == 'A');
    CHECK(b.contatti.front().classe_creduta == "krivak2");
    CHECK(b.contatti[1].sensore == "tb16");
    CHECK(b.rilevamenti.size() == 101);
    CHECK(b.bersaglio_tma == 3);
    CHECK(b.app.modo_sonar == "moderno");
    CHECK(b.app.proposta.distanza_m == doctest::Approx(9260.0));
    CHECK(b.app.proposta.rotta == doctest::Approx(120.0));
    CHECK(tr.istante(30.0).contatti.front().classe_creduta.empty());
    CHECK(unita(tr.istante(30.0), 0)->alberi == 0x02);
    CHECK(unita(tr.istante(100.0), 0)->alberi == 0);
}

TEST_CASE("traccia: lo schema per nome regge campi rinominati, tipi nuovi e un fotogramma rovinato") {
    RegistratoreTraccia reg("X", 1);
    for (double t = 0.0; t <= 1200.0; t += 5.0) {   // un evento ogni 25 s, senza i testi
        Fotogramma f = sintetico(t);
        const auto n = static_cast<std::size_t>(t / 25.0);
        f.evento = static_cast<int>(n) - 1;
        reg.aggiungi(f, n);
    }
    const auto dati = reg.dati();
    // un motore che non conosce «alberi» (rinominato): lo lascia al predefinito, il resto si legge
    const Traccia r = leggi_traccia(con_intestazione(dati, "alberi:i", "albero:i"));
    CHECK(r.fotogrammi_saltati == 0);
    CHECK(unita(r.istante(30.0), 0)->alberi == 0);
    CHECK(unita(r.istante(30.0), 0)->quota == doctest::Approx(150.0));
    // un tipo di record e una chiave d'intestazione nuovi (di un motore piu' recente): ignorati
    const Traccia z = leggi_traccia(con_intestazione(dati, "", "", "record Z a:i b:q0.5\nnuova=1\n"));
    CHECK(z.fotogrammi_saltati == 0);
    CHECK(z.metadati.at("nuova") == "1");
    // un byte rovinato a meta': si salta fino al fotogramma chiave dopo, il resto c'e'
    auto rotti = dati;
    rotti[rotti.size() / 2] ^= 0xff;
    rotti[rotti.size() / 2 + 1] ^= 0xff;
    const Traccia x = leggi_traccia(rotti);
    CHECK(x.durata >= 1100.0);   // dal fotogramma chiave dopo il danno
    CHECK(unita(x.istante(10.0), 7));
    CHECK_THROWS(leggi_traccia({1, 2, 3}));
    // una traccia senza i testi degli eventi (come quelle scritte prima): i segnalibri col solo tempo e l'id
    CHECK(r.registro.empty());
    REQUIRE_FALSE(segnalibri(r).empty());
    CHECK(segnalibri(r).front().testo_it.empty());
}

TEST_CASE("traccia: una missione vera, il fotogramma dalla missione, il file e le cartelle" *
          doctest::skip(!fs::exists(M688_DATI "/acustica.toml"))) {
    const fs::path dati_casa = fs::temp_directory_path() / "m688_prova_traccia";
    fs::remove_all(dati_casa);
    setenv("XDG_DATA_HOME", dati_casa.c_str(), 1);
    Missione m(std::string(M688_MISSIONI) + "/SCENE1A.lua", 1, false);
    StazioniMissione st(m, 1);
    RegistratoreTraccia reg("SCENE1A", 1);
    Vec2 a_150;
    for (int k = 0; k < 300 * 9; ++k) {
        m.avanza(1.0 / 9.0);
        if (reg.serve(m.tempo(), m.eventi_missione().size())) {
            DatiApp app;
            app.stazione = 8;
            reg.aggiungi(fotogramma_da(m, &st, app, reg.ultimo_evento()), m.eventi_missione().size());
        }
        if (std::fabs(m.tempo() - 150.0) < 0.06 && m.giocatore()) a_150 = m.giocatore()->pos;
    }
    CHECK(reg.fotogrammi() >= 60);
    const fs::path p = percorso_replay("", "SCENE1A", "2026-10-26");
    CHECK(p.parent_path().filename() == "liberi");
    reg.salva(p);
    const Traccia tr = carica_traccia(p);
    CHECK(tr.missione == "SCENE1A");
    const IstanteTraccia i = tr.istante(150.0);
    REQUIRE(m.giocatore());
    const StatoUnita* g = unita(i, m.giocatore()->id);
    REQUIRE(g);
    CHECK(distanza(g->pos, a_150) < 5.0);
    CHECK(i.unita.size() >= 5);
    CHECK(i.app.stazione == 8);
    // i segnalibri sono gli eventi del registro, coi testi
    const auto sl = segnalibri(tr);
    REQUIRE(sl.size() == m.eventi_missione().size());
    for (std::size_t k = 0; k < sl.size(); ++k) {
        CHECK(sl[k].id == m.eventi_missione()[k].id);
        CHECK(sl[k].testo_it == m.eventi_missione()[k].testo_it);
    }
    // fuori carriera se ne tengono 20
    for (int k = 0; k < 24; ++k) reg.salva(percorso_replay("", "SCENE1A", "2026-10-" + std::to_string(k)));
    CHECK(pulisci_replay_liberi(20) == 5);
    // in carriera il fascicolo tiene il nome del file
    static const RegoleCarriera r = carica_carriera(M688_DATI "/carriera.toml");
    Fascicolo f = Carriera::nuovo(r, Marina::usn, "J. R. Halloran", false, Indulgenza::realistica);
    VoceMissione v;
    v.missione = "SCENE1A";
    v.replay = percorso_replay(f.id, "SCENE1A", "2026-10-26").filename().string();
    f.missioni.push_back(v);
    salva_fascicolo(f, percorso_fascicolo(f.id));
    CHECK(leggi_fascicolo(percorso_fascicolo(f.id)).missioni.back().replay == v.replay);
    unsetenv("XDG_DATA_HOME");
    fs::remove_all(dati_casa);
}

namespace {

// una traccia della versione 1 scritta a mano (lo schema U senza i campi della 2, nessun record I), come quelle dei
// fascicoli salvati prima della sandbox
std::vector<std::uint8_t> traccia_v1() {
    const auto u = [](std::vector<std::uint8_t>& b, std::uint64_t v) {
        while (v >= 0x80) { b.push_back(static_cast<std::uint8_t>(v | 0x80)); v >>= 7; }
        b.push_back(static_cast<std::uint8_t>(v));
    };
    const auto s = [&](std::vector<std::uint8_t>& b, std::int64_t v) {
        u(b, (static_cast<std::uint64_t>(v) << 1) ^ static_cast<std::uint64_t>(v >> 63));
    };
    const std::string testa = "versione=1\nmissione=VECCHIA\nseme=7\nripresa_da=-1\ndelta=U C\n"
                              "record U id:k x:q0.5 y:q0.5 quota:q0.1 rotta:a0.01 velocita:q0.01 stato:i alberi:i lanciatore:i collegata:i\n"
                              "record N id:k classe:s categoria:s flag:i\n";
    std::vector<std::uint8_t> out{'M', '6', '8', '8', 'T', 'R', '\n'};
    u(out, testa.size());
    out.insert(out.end(), testa.begin(), testa.end());
    for (int k = 0; k < 2; ++k) {
        std::vector<std::uint8_t> f;
        u(f, static_cast<std::uint64_t>(k * 5000));   // ms
        s(f, -1);
        u(f, k == 0 ? 1u : 0u);
        if (k == 0) {
            for (const std::string x : {"los_angeles", "sottomarino"}) {
                f.push_back('T'); u(f, x == "los_angeles" ? 0u : 1u); u(f, x.size()); f.insert(f.end(), x.begin(), x.end());
            }
            f.push_back('N'); u(f, 1); s(f, 3); s(f, 0); s(f, 1); s(f, 9);
        }
        f.push_back('U'); u(f, 1); s(f, 3);
        const std::int64_t x = k == 0 ? 2000 : 20, y = k == 0 ? -1000 : 0;   // mezzi metri; la seconda in differenza
        for (const std::int64_t v : {x, y, k == 0 ? std::int64_t{900} : std::int64_t{0}, k == 0 ? std::int64_t{9000} : std::int64_t{0},
                                     k == 0 ? std::int64_t{500} : std::int64_t{0}, std::int64_t{0}, std::int64_t{0},
                                     k == 0 ? std::int64_t{-1} : std::int64_t{0}, k == 0 ? std::int64_t{-1} : std::int64_t{0}})
            s(f, v);
        out.push_back('F');
        u(out, f.size());
        out.insert(out.end(), f.begin(), f.end());
    }
    return out;
}

}  // namespace

TEST_CASE("traccia v2: stima, ellisse, allerta, emissioni e i fatti dell'IA vanno e tornano") {
    RegistratoreTraccia reg("V2", 3);
    for (int k = 0; k <= 3; ++k) {
        Fotogramma f;
        f.t = 5.0 * k;
        StatoUnita u;
        u.id = 4; u.classe = "los_angeles"; u.categoria = "sottomarino"; u.flag = 9;
        u.pos = {100.0 * k, 0.0};
        u.pos_stimata = {100.0 * k + 37.0, -12.0};
        u.sigma_nav_m = 80.0 + k;
        u.allerta = k >= 2 ? 3 : 1;
        u.emissioni = k == 3 ? StatoUnita::EMESSO_RADAR | StatoUnita::EMESSO_PING : 0;
        if (k >= 2) {   // la TMA in corso e le armi a bordo
            u.tma_bersaglio = 2;
            u.tma_gambe = k;
            u.tma_soluzione = k == 3;
            u.tma_osservabile = k == 3;
            u.tma_pos = {12000.0, -3400.0};
            u.tma_rotta = 255.0;
            u.tma_velocita_kn = 7.5;
            u.armi_rimaste = 22 - k;
            u.tubi_carichi = 4;
        }
        f.unita.push_back(u);
        if (k == 1) {   // due fatti nello stesso fotogramma, fuori ordine: la lettura li ordina
            EventoIa a;
            a.t = 4.2; a.tipo = "contatto_solido"; a.unita = 4; a.altra = 2; a.a = 61130.4; a.b = 16.5; a.s = "tb16";
            EventoIa b = a;
            b.t = 3.1; b.tipo = "contatto_nuovo"; b.b = 3.2;
            f.eventi_ia = {a, b};
        }
        if (k == 3) {
            EventoIa e;
            e.t = 14.0; e.tipo = "siluro"; e.unita = 70; e.altra = 4; e.n = 2; e.a = -500.25; e.b = 1200.5; e.c = 45.67; e.s = "aggancio";
            f.eventi_ia = {e};
        }
        reg.aggiungi(f, 0);
    }
    const Traccia t = leggi_traccia(reg.dati());
    CHECK(t.versione == 2);
    REQUIRE(t.unita.count(4));
    const StatoUnita& ultima = t.unita.at(4).back().second;
    CHECK(ultima.pos_stimata.x == doctest::Approx(337.0));
    CHECK(ultima.pos_stimata.y == doctest::Approx(-12.0));
    CHECK(ultima.sigma_nav_m == doctest::Approx(83.0));
    CHECK(ultima.allerta == 3);
    CHECK(ultima.emissioni == (StatoUnita::EMESSO_RADAR | StatoUnita::EMESSO_PING));
    CHECK(t.unita.at(4).front().second.allerta == 1);
    CHECK(t.unita.at(4).front().second.tma_bersaglio == -1);
    CHECK(t.unita.at(4).front().second.armi_rimaste == -1);
    CHECK(ultima.tma_bersaglio == 2);
    CHECK(ultima.tma_gambe == 3);
    CHECK(ultima.tma_soluzione);
    CHECK(ultima.tma_osservabile);
    CHECK(ultima.tma_pos.x == doctest::Approx(12000.0));
    CHECK(ultima.tma_pos.y == doctest::Approx(-3400.0));
    CHECK(ultima.tma_rotta == doctest::Approx(255.0));
    CHECK(ultima.tma_velocita_kn == doctest::Approx(7.5));
    CHECK(ultima.armi_rimaste == 19);
    CHECK(ultima.tubi_carichi == 4);
    REQUIRE(t.eventi_ia.size() == 3);
    CHECK(t.eventi_ia[0].tipo == "contatto_nuovo");
    CHECK(t.eventi_ia[1].tipo == "contatto_solido");
    CHECK(t.eventi_ia[1].a == doctest::Approx(61130.4));
    CHECK(t.eventi_ia[1].b == doctest::Approx(16.5));
    CHECK(t.eventi_ia[1].s == "tb16");
    const EventoIa& s = t.eventi_ia[2];
    CHECK(s.unita == 70);
    CHECK(s.altra == 4);
    CHECK(s.n == 2);
    CHECK(s.c == doctest::Approx(45.67));
    CHECK(s.s == "aggancio");
    CHECK(t.eventi_ia_fino(3.0) == 0);
    CHECK(t.eventi_ia_fino(4.2) == 2);
    CHECK(t.eventi_ia_fino(100.0) == 3);
}

TEST_CASE("traccia v2: le tracce della versione 1 dei fascicoli si leggono ancora") {
    const Traccia t = leggi_traccia(traccia_v1());
    CHECK(t.versione == 1);
    CHECK(t.missione == "VECCHIA");
    CHECK(t.fotogrammi_saltati == 0);
    CHECK(t.eventi_ia.empty());
    REQUIRE(t.unita.count(3));
    REQUIRE(t.unita.at(3).size() == 2);
    const StatoUnita& u = t.unita.at(3).back().second;
    CHECK(u.classe == "los_angeles");
    CHECK(u.pos.x == doctest::Approx(1010.0));
    CHECK(u.pos.y == doctest::Approx(-500.0));
    CHECK(u.pos_stimata.x == doctest::Approx(u.pos.x));   // senza la stima: la vera
    CHECK(u.pos_stimata.y == doctest::Approx(u.pos.y));
    CHECK(u.allerta == -1);
    CHECK(u.sigma_nav_m == 0.0);
    CHECK(u.emissioni == 0);
    CHECK(u.quota == doctest::Approx(90.0));
    CHECK(t.istante(2.5).unita.size() == 1);
}

TEST_CASE("traccia v2: la raccolta dei fatti dell'IA su una scena dell'Enhanced" *
          doctest::skip(!fs::exists(M688_DATI "/acustica.toml") || !fs::exists(M688_MISSIONI "/prove/PROVA_S6_USCITA.lua"))) {
    Missione m(std::string(M688_MISSIONI) + "/prove/PROVA_S6_USCITA.lua", 1);
    m.acustica_enhanced(M688_DATI);
    REQUIRE(m.con_acustica());
    StazioniMissione st(m, 1);
    RegistratoreTraccia reg("PROVA_S6_USCITA", 1);
    RaccoltaTraccia raccolta;
    for (int k = 0; k < 200 * 9; ++k) {
        m.avanza(1.0 / 9.0);
        if (reg.serve(m.tempo(), m.eventi_missione().size()))
            reg.aggiungi(fotogramma_da(m, &st, {}, reg.ultimo_evento(), &raccolta), m.eventi_missione().size());
    }
    const Traccia t = leggi_traccia(reg.dati());
    CHECK(t.versione == 2);
    REQUIRE_FALSE(t.eventi_ia.empty());
    int nuovi = 0;
    std::map<std::pair<int, int>, int> per_coppia;
    for (const EventoIa& e : t.eventi_ia) {
        if (e.tipo != "contatto_nuovo") continue;
        ++nuovi;
        CHECK(e.unita >= 0);
        CHECK(e.altra >= 0);
        CHECK(e.a > 0.0);                // la distanza vera
        CHECK_FALSE(e.s.empty());        // il sensore
        ++per_coppia[{e.unita, e.altra}];
    }
    CHECK(nuovi >= 3);
    // per ogni coppia nuovo e perso si alternano, e un contatto che va e torna entro ASSENZA_PERSO_S non fa righe. Se torna
    // dopo e' di nuovo un contatto nuovo, ed e' giusto: il Typhoon sul peschereccio 6, sulla soglia a 72 km, esce a 102 s e
    // rientra a 149 s (tools/re/prove/portata_sonar/punto1/esiti/fatti/riassunto_semi.txt)
    std::map<std::pair<int, int>, double> perso_a;   // -1 mentre il contatto c'e', poi il tempo del perso
    for (const EventoIa& e : t.eventi_ia) {
        const std::pair<int, int> coppia{e.unita, e.altra};
        const auto it = perso_a.find(coppia);
        if (e.tipo == "contatto_nuovo") {
            if (it != perso_a.end()) {
                CHECK_MESSAGE(it->second >= 0.0, "nuovo senza perso: ", coppia.first, " su ", coppia.second);
                CHECK_MESSAGE(e.t - it->second >= RaccoltaTraccia::ASSENZA_PERSO_S, coppia.first, " su ", coppia.second);
            }
            perso_a[coppia] = -1.0;
        } else if (e.tipo == "contatto_perso") {
            CHECK_MESSAGE((it != perso_a.end() && it->second < 0.0), "perso senza nuovo: ", coppia.first, " su ", coppia.second);
            perso_a[coppia] = e.t;
        }
    }
    CHECK(per_coppia.count({4, 7}) == 1);   // il LA sul peschereccio 7, nei primi 90 s
    for (std::size_t k = 1; k < t.eventi_ia.size(); ++k) CHECK(t.eventi_ia[k - 1].t <= t.eventi_ia[k].t);
    // la stima e l'ellisse di chi naviga a stima
    REQUIRE(t.unita.count(4));
    const StatoUnita& la = t.unita.at(4).back().second;
    CHECK(distanza(la.pos_stimata, la.pos) < 5000.0);
    // senza la raccolta, come una traccia della versione 1: niente fatti dell'IA
    Missione m2(std::string(M688_MISSIONI) + "/prove/PROVA_S6_USCITA.lua", 1);
    m2.acustica_enhanced(M688_DATI);
    RegistratoreTraccia reg2("PROVA_S6_USCITA", 1);
    for (int k = 0; k < 20 * 9; ++k) {
        m2.avanza(1.0 / 9.0);
        if (reg2.serve(m2.tempo(), m2.eventi_missione().size()))
            reg2.aggiungi(fotogramma_da(m2, nullptr, {}, reg2.ultimo_evento()), m2.eventi_missione().size());
    }
    CHECK(leggi_traccia(reg2.dati()).eventi_ia.empty());
}

constexpr double SOGLIA_STIMA_PROVA = 11.0;   // il file riscrive la stima quando si sposta di 10 m

TEST_CASE("traccia v2: la vista dal vivo aggiunge i fotogrammi come li rileggerebbe dal file") {
    RegistratoreTraccia reg("DAL_VIVO", 5);
    Traccia vivo;
    std::size_t eventi = 0;
    for (double t = 0.0; t <= 600.0 + 1e-9; t += 1.0) {
        if (std::fmod(t, 25.0) == 0.0) ++eventi;
        if (!reg.serve(t, eventi)) continue;
        Fotogramma f = sintetico(t);
        if (t > 300.0) f.unita.pop_back();   // un'unita' esce dal gioco
        f.evento = static_cast<int>(eventi) - 1;
        for (int e = reg.ultimo_evento() + 1; e < static_cast<int>(eventi); ++e) {
            EventoTraccia x;
            x.id = e;
            x.tempo = 25.0 * e;
            x.tipo = "scoperta";
            f.eventi.push_back(x);
        }
        if (std::fmod(t, 50.0) == 0.0) {
            EventoIa a;
            a.t = t - 0.5; a.tipo = "contatto_nuovo"; a.unita = 0; a.altra = static_cast<int>(t / 50.0); a.s = "bqq5_sfera";
            f.eventi_ia.push_back(a);
        }
        f.unita[0].sigma_min_m = 20.0 + t / 100.0;
        f.unita[0].orientamento_deg = 135.0;
        reg.aggiungi(f, eventi);
        aggiungi_fotogramma(vivo, f);
    }
    const Traccia file = leggi_traccia(reg.dati());
    CHECK(vivo.durata == doctest::Approx(file.durata));
    CHECK(vivo.registro.size() == file.registro.size());
    REQUIRE(vivo.eventi_ia.size() == file.eventi_ia.size());
    for (std::size_t k = 0; k < vivo.eventi_ia.size(); ++k) CHECK(vivo.eventi_ia[k].altra == file.eventi_ia[k].altra);
    for (const double t : {12.0, 299.0, 450.0, 600.0}) {
        const IstanteTraccia a = vivo.istante(t), b = file.istante(t);
        CHECK(a.unita.size() == b.unita.size());
        for (const StatoUnita& u : a.unita)
            if (const StatoUnita* v = unita(b, u.id)) {
                CHECK(distanza(u.pos, v->pos) < 1.0);
                CHECK(distanza(u.pos_stimata, v->pos_stimata) < SOGLIA_STIMA_PROVA);
                CHECK(u.allerta == v->allerta);
            }
        CHECK(a.contatti.size() == b.contatti.size());
    }
    // l'ellisse intera dal record V
    const StatoUnita& u0 = file.unita.at(0).back().second;
    CHECK(u0.orientamento_deg == doctest::Approx(135.0));
    CHECK(u0.sigma_min_m == doctest::Approx(26.0).epsilon(0.2));
}

TEST_CASE("traccia v2: i contatti di ogni unita' (record K) e il quadro di un'unita' a un istante") {
    RegistratoreTraccia reg("K", 1);
    Traccia vivo;
    const auto riga = [](double t, int o, int b, bool solido, bool perso = false) {
        ContattoUnita c;
        c.t = t; c.osservatore = o; c.bersaglio = b; c.qualita = solido ? 900 : 300; c.solido = solido; c.perso = perso;
        c.rilevamento = 45.0; c.pos_stimata = {1000.0 * b, 2000.0}; c.sensore = "tb16"; c.classe_creduta = solido ? "victor3" : "";
        return c;
    };
    for (int k = 0; k <= 6; ++k) {
        Fotogramma f;
        f.t = 10.0 * k;
        StatoUnita u;
        u.id = 4; u.classe = "los_angeles"; u.categoria = "sottomarino";
        f.unita.push_back(u);
        if (k == 1) f.contatti_unita = {riga(10, 4, 2, false), riga(10, 4, 3, false), riga(10, 5, 4, false)};
        if (k == 3) f.contatti_unita = {riga(30, 4, 2, true)};
        if (k == 5) f.contatti_unita = {riga(48, 4, 3, false, true)};   // perso a 48 s
        reg.aggiungi(f, 0);
        aggiungi_fotogramma(vivo, f);
    }
    const Traccia t = leggi_traccia(reg.dati());
    REQUIRE(t.contatti_unita.size() == 5);
    CHECK(t.quadro_unita(4, 5.0).empty());
    auto q = t.quadro_unita(4, 20.0);
    REQUIRE(q.size() == 2);
    CHECK_FALSE(q[0].solido);
    CHECK(q[0].eta_s == doctest::Approx(10.0));   // riga a 10 s, guardata a 20
    q = t.quadro_unita(4, 40.0);
    REQUIRE(q.size() == 2);
    CHECK(q[0].bersaglio == 2);
    CHECK(q[0].solido);
    CHECK(q[0].classe_creduta == "victor3");
    CHECK(q[0].pos_stimata.x == doctest::Approx(2000.0));
    CHECK(q[0].sensore == "tb16");
    q = t.quadro_unita(4, 60.0);
    REQUIRE(q.size() == 1);   // il 3 e' perso
    CHECK(q[0].bersaglio == 2);
    CHECK(t.quadro_unita(5, 60.0).size() == 1);
    // dal vivo lo stesso
    CHECK(vivo.contatti_unita.size() == 5);
    CHECK(vivo.quadro_unita(4, 60.0).size() == 1);
    CHECK(vivo.quadro_unita(4, 40.0).size() == 2);
}

TEST_CASE("traccia v2: il quadro di parte (record P), anche vuoto, a un istante") {
    RegistratoreTraccia reg("P", 1);
    Traccia vivo;
    const auto voce = [](int b, double x, bool pos) {
        VoceParte v;
        v.bersaglio = b; v.posizione = pos; v.pos = {x, 500.0}; v.origine = {0.0, 0.0}; v.rilevamento = 80.0;
        v.semiasse_maggiore_m = 900.0; v.semiasse_minore_m = 300.0; v.orientamento_deg = 170.0; v.t_dato = 3.0;
        v.sensore = pos ? "tma" : "tb16"; v.fonte = 4; v.categoria = "sottomarino"; v.classe = pos ? "victor3" : "";
        v.qualita = 650.0;
        return v;
    };
    for (int k = 0; k <= 4; ++k) {
        Fotogramma f;
        f.t = 5.0 * k;
        StatoUnita u;
        u.id = 4; u.classe = "los_angeles"; u.categoria = "sottomarino";
        f.unita.push_back(u);
        if (k == 1) f.quadri_parte = {{5.0, 0x08, {voce(2, 1000.0, false), voce(3, 5000.0, false)}}, {5.0, 0x10, {voce(4, 0.0, false)}}};
        if (k == 3) f.quadri_parte = {{15.0, 0x08, {voce(2, 1200.0, true)}}};
        if (k == 4) f.quadri_parte = {{20.0, 0x10, {}}};   // il Patto perde tutto
        reg.aggiungi(f, 0);
        aggiungi_fotogramma(vivo, f);
    }
    const Traccia t = leggi_traccia(reg.dati());
    REQUIRE(t.quadri_parte.size() == 4);
    CHECK_FALSE(t.quadro_parte(0x08, 2.0));
    const QuadroParte* q = t.quadro_parte(0x08, 10.0);
    REQUIRE(q);
    REQUIRE(q->voci.size() == 2);
    CHECK(q->voci[1].bersaglio == 3);
    CHECK(q->voci[1].semiasse_maggiore_m == doctest::Approx(900.0));
    CHECK(q->voci[1].orientamento_deg == doctest::Approx(170.0));
    CHECK(q->voci[1].sensore == "tb16");
    q = t.quadro_parte(0x08, 30.0);
    REQUIRE(q);
    REQUIRE(q->voci.size() == 1);
    CHECK(q->voci[0].posizione);
    CHECK(q->voci[0].classe == "victor3");
    CHECK(q->voci[0].pos.x == doctest::Approx(1200.0));
    REQUIRE(t.quadro_parte(0x10, 12.0));
    CHECK(t.quadro_parte(0x10, 12.0)->voci.size() == 1);
    REQUIRE(t.quadro_parte(0x10, 30.0));
    CHECK(t.quadro_parte(0x10, 30.0)->voci.empty());
    REQUIRE(vivo.quadro_parte(0x08, 30.0));
    CHECK(vivo.quadro_parte(0x08, 30.0)->voci.size() == 1);
    CHECK(vivo.quadro_parte(0x10, 30.0)->voci.empty());
}

TEST_CASE("traccia v2: la raccolta accesa su una missione vera, classica ed Enhanced, dal primo passo" *
          doctest::skip(!fs::exists(M688_DATI "/acustica.toml"))) {
    for (const bool enhanced : {false, true}) {
        CAPTURE(enhanced);
        Missione m(std::string(M688_MISSIONI) + "/SCENE1A.lua", 1, false);
        if (enhanced) {
            m.acustica_enhanced(M688_DATI);
            m.registra_statistiche(true);
        }
        StazioniMissione st(m, 1);
        RegistratoreTraccia reg("SCENE1A", 1);
        RaccoltaTraccia raccolta;
        Traccia vivo;
        // anche il fotogramma a 0 s, prima del primo passo (come Partita all'avvio)
        for (int k = 0; k <= 30 * 9; ++k) {
            if (reg.serve(m.tempo(), m.eventi_missione().size())) {
                Fotogramma f = fotogramma_da(m, &st, {}, reg.ultimo_evento(), &raccolta);
                reg.aggiungi(f, m.eventi_missione().size());
                aggiungi_fotogramma(vivo, f);
            }
            m.avanza(1.0 / 9.0);
        }
        const Traccia t = leggi_traccia(reg.dati());
        CHECK(t.durata >= 25.0);
        CHECK(t.unita.size() >= 5);
        CHECK(vivo.unita.size() == t.unita.size());
        if (!enhanced) {   // il classico non raccoglie niente
            CHECK(t.eventi_ia.empty());
            CHECK(t.contatti_unita.empty());
            CHECK(t.quadri_parte.empty());
        }
    }
}

TEST_CASE("traccia: le navi neutrali senza contatto ogni 25 s, subito se cambia lo stato (blocco 1 del sonar)") {
    RegistratoreTraccia reg("prova", 1);
    const auto unita = [](int id, int flag, double t) {
        StatoUnita u;
        u.id = id;
        u.classe = id == 2 ? "mercantile" : "krivak2";
        u.categoria = "nave";
        u.flag = flag;
        u.pos = {t * 6.0, static_cast<double>(id) * 1000.0};
        u.rotta = 90.0;
        u.velocita_kn = 12.0;
        u.distrutta = id == 2 && t >= 137.0;
        return u;
    };
    for (double t = 0.0; t <= 300.0 + 1e-9; t += 1.0) {
        if (!reg.serve(t, 0)) continue;
        Fotogramma f;
        f.t = t;
        f.unita = {unita(2, 0x00, t), unita(3, 0x10, t), unita(4, 0x00, t)};
        if (t >= 200.0) {   // il giocatore prende a contatto la neutrale 4: torna a ogni fotogramma lento
            StatoContatto c;
            c.id = 4;
            f.contatti.push_back(c);
        }
        reg.aggiungi(f, 0);
    }
    const Traccia tr = leggi_traccia(reg.dati());
    const auto passi = [&](int id, double da, double a) {
        std::vector<double> d;
        const auto& s = tr.unita.at(id);
        for (std::size_t i = 1; i < s.size(); ++i)
            if (s[i - 1].first >= da && s[i].first <= a) d.push_back(s[i].first - s[i - 1].first);
        return d;
    };
    // i fotogrammi lenti, coi passi da 1 s, vengono ogni 6 s
    for (double d : passi(3, 0.0, 300.0)) CHECK(d <= 6.0 + 1e-9);          // l'ostile: a ogni fotogramma lento
    for (double d : passi(4, 0.0, 195.0)) CHECK(d <= 25.0 + 1e-9);        // la neutrale: mai oltre 25 s,
    CHECK(passi(4, 0.0, 195.0).size() < 12);                               // e di rado
    for (double d : passi(4, 205.0, 300.0)) CHECK(d <= 6.0 + 1e-9);       // a contatto: a ogni fotogramma lento
    // la neutrale 2 distrutta a 137 s: scritta al primo fotogramma dopo
    bool distrutta = false;
    for (const auto& [t, u] : tr.unita.at(2))
        if (u.distrutta) { CHECK(t <= 140.0 + 1e-9); distrutta = true; break; }
    CHECK(distrutta);
    // il replay la fa scorrere: a meta' fra due campioni la posizione e' interpolata
    const IstanteTraccia ist = tr.istante(112.5);
    CHECK(ist.unita.size() == 3);
    for (const StatoUnita& u : ist.unita)
        if (u.id == 4) CHECK(u.pos.x == doctest::Approx(6.0 * 112.5).epsilon(0.01));
}

// il vetrino di Dio (docs/28, D1; il contratto col runtime del 31/10/2026): il record X degli interventi
TEST_CASE("traccia: gli interventi del vetrino vanno e tornano, in ordine; le chiavi coi caratteri riservati") {
    const std::map<std::string, std::string> par = {{"sensore", "bqq5"}, {"strano;=\\nome", "a=b;c\\d"}, {"vuoto", ""}};
    CHECK(leggi_chiavi(codifica_chiavi(par)) == par);
    CHECK(leggi_chiavi("").empty());
    RegistratoreTraccia reg("VETRINO", 5);
    Traccia viva;
    for (int k = 0; k <= 2; ++k) {
        Fotogramma f;
        f.t = 10.0 * k;
        StatoUnita u;
        u.id = 2; u.classe = "perry"; u.categoria = "nave"; u.flag = 10;
        u.pos = {100.0 * k, 0.0};
        f.unita.push_back(u);
        if (k == 1) {   // due nella stessa finestra, in disordine
            RecordIntervento b;
            b.id = 8; b.t = 9.888889; b.fase = RecordIntervento::FASE_APPLICATO; b.tipo = "meteo";
            b.ha_punto = true; b.punto = {1234.56, -789.01}; b.raggio_m = 20000.0; b.durata_s = 1800.0;
            b.chiavi = codifica_chiavi({{"stato_mare", "6"}}); b.riga_it = "Il mare sale a 6"; b.riga_en = "Sea state rises to 6";
            RecordIntervento a;
            a.id = 7; a.t = 5.111111; a.fase = RecordIntervento::FASE_APPLICATO; a.tipo = "sensore_guasto"; a.unita = 2;
            a.chiavi = codifica_chiavi(par); a.riga_it = "Il sonar della Perry si guasta"; a.riga_en = "The Perry's sonar fails";
            f.interventi = {b, a};
        }
        if (k == 2) {
            RecordIntervento c;
            c.id = 7; c.t = 19.5; c.fase = RecordIntervento::FASE_FINITO; c.tipo = "sensore_guasto"; c.unita = 2;
            f.interventi.push_back(c);
        }
        reg.aggiungi(f, 0);
        aggiungi_fotogramma(viva, f);
    }
    for (const Traccia& t : {leggi_traccia(reg.dati()), viva}) {
        REQUIRE(t.interventi.size() == 3);
        CHECK(t.interventi[0].id == 7);
        CHECK(t.interventi[0].t == doctest::Approx(5.111).epsilon(1e-4));   // al millisecondo
        CHECK(t.interventi[0].unita == 2);
        CHECK_FALSE(t.interventi[0].ha_punto);
        CHECK(leggi_chiavi(t.interventi[0].chiavi) == par);
        CHECK(t.interventi[0].riga_en == "The Perry's sonar fails");
        CHECK(t.interventi[1].id == 8);
        CHECK(t.interventi[1].tipo == "meteo");
        CHECK(t.interventi[1].ha_punto);
        CHECK(t.interventi[1].punto.x == doctest::Approx(1234.6).epsilon(1e-4));
        CHECK(t.interventi[1].raggio_m == doctest::Approx(20000.0));
        CHECK(t.interventi[1].durata_s == doctest::Approx(1800.0));
        CHECK(leggi_chiavi(t.interventi[1].chiavi).at("stato_mare") == "6");
        CHECK(t.interventi[2].fase == RecordIntervento::FASE_FINITO);
        CHECK(t.interventi[2].t == doctest::Approx(19.5));
    }
    // una traccia senza il vetrino: nessun intervento
    RegistratoreTraccia senza("SENZA", 1);
    Fotogramma f;
    f.t = 0.0;
    senza.aggiungi(f, 0);
    CHECK(leggi_traccia(senza.dati()).interventi.empty());
}
