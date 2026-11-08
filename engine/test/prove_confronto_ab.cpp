#include <doctest.h>

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "../app/confronto_ab.hpp"
#include "../app/esperimento.hpp"
#include "../app/esperimento_ab.hpp"
#include "../app/replay_affiancati.hpp"
#include "../app/schermata_confronto.hpp"
#include "grafica/tela.hpp"
#include "missioni/danni_missione.hpp"
#include "missioni/interventi.hpp"
#include "missioni/missione.hpp"
#include "missioni/traccia_missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/traccia_replay.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Il confronto A/B del vetrino di Dio (docs/28 §5, D2): coppie di partite sugli stessi semi, B col Victor tolto a 20 s;
// il confronto delle serie, il punto di divergenza, confronto.toml, il rapporto, la schermata e i replay affiancati.

namespace {

const std::string SUB = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200, rumore = 8, sonar = 19 }";
const std::string NAVE = "modello = { massima_kn = 32, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 0, integrita = 100, rumore = 10, sonar = 20 }";

std::string scena() {
    const auto f = fs::temp_directory_path() / "m688_prova_confronto_ab.lua";
    std::ofstream(f) << "return { id = 'PROVA_CONFRONTO', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false,"
                        " script = {}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = -20000 },"
                        "  quota_m = 200, rotta = 90, velocita_kn = 3, " << SUB << " },\n"
                        "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 0, velocita_kn = 10, " << NAVE << " },\n"
                        "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 8000, y = 0 }, quota_m = 150, rotta = 0, velocita_kn = 5, " << SUB << " },\n"
                        "}, timer = {} } } }\n";
    return f.string();
}

// una partita del vetrino di 60 s, salvata in `file`; con l'intervento il Victor sparisce a 20 s
Traccia partita(std::uint64_t seme, bool con_intervento, const fs::path& file) {
    Missione m(scena(), seme, false);
    m.acustica_enhanced(M688_DATI);
    m.senza_giocatore(true);
    if (con_intervento) {
        Intervento x;
        x.tipo = "togli";
        x.unita = 3;
        x.tempo = 20.0;
        REQUIRE(m.intervieni(x).ok);
    }
    RegistratoreTraccia reg("PROVA_CONFRONTO", seme);
    RaccoltaTraccia raccolta;
    for (int k = 0; k < 60 * 9; ++k) {
        m.avanza(1.0 / 9.0);
        if (reg.serve(m.tempo(), m.eventi_missione().size()))
            reg.aggiungi(fotogramma_da(m, nullptr, DatiApp{}, reg.ultimo_evento(), &raccolta), m.eventi_missione().size());
    }
    reg.salva(file);
    return leggi_traccia(reg.dati());
}

struct TreCoppie {
    std::vector<Traccia> a, b;
    std::vector<std::string> na, nb;
    fs::path cartella;
};
TreCoppie& esperimento() {   // tre coppie, scritte anche su disco per la schermata e gli affiancati
    static TreCoppie e = [] {
        TreCoppie x;
        x.cartella = fs::temp_directory_path() / "m688_prova_confronto";
        fs::create_directories(x.cartella / "A");
        fs::create_directories(x.cartella / "B");
        for (std::uint64_t s = 1; s <= 3; ++s) {
            x.na.push_back("A/" + std::to_string(s) + ".m688tr");
            x.nb.push_back("B/" + std::to_string(s) + ".m688tr");
            x.a.push_back(partita(s, false, x.cartella / x.na.back()));
            x.b.push_back(partita(s, true, x.cartella / x.nb.back()));
        }
        return x;
    }();
    return e;
}

int accesi(const Tela& t, Rettangolo r) {
    int n = 0;
    for (int y = r.y; y < r.y + r.h; ++y)
        for (int x = r.x; x < r.x + r.w; ++x) n += t.dati()[y * LARGHEZZA_TELA + x] != 0;
    return n;
}

}  // namespace

TEST_CASE("confronto A/B: le coppie divergono all'intervento, il confronto delle serie, confronto.toml e il rapporto") {
    TreCoppie& e = esperimento();
    DatiConfronto d = confronta_tracce(e.a, e.b, e.na, e.nb, 1);
    d.titolo = "La scorta senza il Victor";
    d.mostra = "Che cosa cambia se il Victor sparisce a 20 s";
    REQUIRE(d.coppie.size() == 3);
    CHECK(d.confronto.coppie == 3);
    CHECK_FALSE(d.interventi.empty());   // la riga del «togli» dalla prima partita di B
    for (const CoppiaConfronto& c : d.coppie) {
        CAPTURE(c.seme);
        CHECK(c.divergono);
        CHECK(c.t_s == doctest::Approx(20.0).epsilon(0.05));
        CHECK(c.t_intervento_s == doctest::Approx(20.0).epsilon(0.05));
        CHECK(c.dopo_intervento_s < 1.0);
        CHECK_FALSE(c.come.empty());
    }
    CHECK_FALSE(d.confronto.parti.empty());
    // la scrittura e la lettura
    scrivi_confronto(e.cartella / "confronto.toml", d);
    const DatiConfronto l = leggi_confronto(e.cartella / "confronto.toml");
    CHECK(l.titolo == d.titolo);
    CHECK(l.interventi == d.interventi);
    REQUIRE(l.coppie.size() == 3);
    CHECK(l.coppie[1].traccia_b == "B/2.m688tr");
    CHECK(l.coppie[1].t_s == doctest::Approx(d.coppie[1].t_s));
    CHECK(l.coppie[1].come == d.coppie[1].come);
    CHECK(l.confronto.esiti.size() == d.confronto.esiti.size());
    CHECK(l.confronto.parti.size() == d.confronto.parti.size());
    for (const auto& [parte, m] : d.confronto.parti)
        for (const auto& [nome, n] : m) {
            CAPTURE(nome);
            CHECK(l.confronto.parti.at(parte).at(nome).mediana_b == doctest::Approx(n.mediana_b).epsilon(1e-5));
        }
    const std::string r = rapporto_confronto(l, {"pagina1.bmp"});
    CHECK(r.find("# La scorta senza il Victor") != std::string::npos);
    CHECK(r.find("## Gli esiti") != std::string::npos);
    CHECK(r.find("## Le coppie") != std::string::npos);
    CHECK(r.find("![pagina1.bmp](pagina1.bmp)") != std::string::npos);
}

TEST_CASE("confronto A/B: la schermata in tre pagine, il clic su una coppia, i replay affiancati al punto di divergenza") {
    TreCoppie& e = esperimento();
    DatiConfronto d = confronta_tracce(e.a, e.b, e.na, e.nb, 1);
    d.titolo = "La scorta senza il Victor";
    SchermataConfronto s(M688_DATI, d, e.cartella);
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    std::vector<Tela> pagine(3);
    for (int k = 0; k < 3; ++k) {
        s.pagina(k);
        s.disegna(pagine[static_cast<std::size_t>(k)], f);
        CHECK(accesi(pagine[static_cast<std::size_t>(k)], {20, 60, 600, 300}) > 2000);
    }
    // la pagina delle coppie: un clic sulla seconda riga sceglie la coppia del seme 2
    s.pagina(2);
    Tela t;
    s.disegna(t, f);
    REQUIRE(s.righe_coppie().size() == 3);
    Comandi c;
    const Rettangolo r = s.righe_coppie()[1];
    c.clic = {{r.x + 20, r.y + r.h / 2}};
    CHECK(s.comandi(c, 0.0));
    REQUIRE(s.coppia_scelta());
    CHECK(*s.coppia_scelta() == 1);
    CHECK(s.traccia_b(1) == e.cartella / "B/2.m688tr");
    // E: il rapporto nella cartella
    Comandi ce;
    ce.lettere = {'E'};
    CHECK_FALSE(s.comandi(ce, 0.0));
    CHECK(fs::exists(e.cartella / "rapporto.md"));
    // i due replay affiancati, fermi al punto di divergenza
    ReplayAffiancati ra(M688_DATI, carica_traccia(s.traccia_a(1)), carica_traccia(s.traccia_b(1)), d.coppie[1].t_s, 2);
    CHECK(ra.tempo() == doctest::Approx(d.coppie[1].t_s));
    Tela ta;
    ra.disegna(ta, f);
    // gli interventi di B sulla linea del tempo col colore della loro famiglia, due tipi di famiglie diverse
    {
        Traccia tb = carica_traccia(s.traccia_b(1));
        const auto& cat = catalogo_interventi();
        REQUIRE(cat.size() >= 2);
        std::string tipo_a = cat.front().tipo, tipo_b;
        for (const auto& v : cat)
            if (colore_intervento(v.tipo) != colore_intervento(tipo_a)) tipo_b = v.tipo;
        REQUIRE_FALSE(tipo_b.empty());
        RecordIntervento x;
        x.fase = RecordIntervento::FASE_APPLICATO;
        x.t = 0.25 * tb.durata;
        x.tipo = tipo_a;
        tb.interventi.push_back(x);
        x.t = 0.75 * tb.durata;
        x.tipo = tipo_b;
        tb.interventi.push_back(x);
        Traccia ta_tr = carica_traccia(s.traccia_a(1));
        ReplayAffiancati ri(M688_DATI, std::move(ta_tr), std::move(tb), 0.0, 2);
        Tela ti;
        ri.disegna(ti, f);
        const Rettangolo b = ri.linea();
        const double durata = std::max(carica_traccia(s.traccia_a(1)).durata, carica_traccia(s.traccia_b(1)).durata);
        const auto colonna = [&](double t_s) { return b.x + static_cast<int>(t_s / durata * (b.w - 1) + 0.5); };
        const double t_b = carica_traccia(s.traccia_b(1)).durata;
        CHECK(ti.at(colonna(0.25 * t_b), b.y) == colore_intervento(tipo_a));
        CHECK(ti.at(colonna(0.75 * t_b), b.y) == colore_intervento(tipo_b));
        if (const char* dir = std::getenv("M688_ISTANTANEE")) {
            Palette pi = Palette::carica(M688_DATI "/immagini/MAIN.pal");
            ri.palette(pi);
            ri.disegna(ti, f);
            ti.salva_bmp(std::string(dir) + "/affiancati_interventi.bmp", pi);
        }
    }
    CHECK(accesi(ta, ra.carta_a()) > 1000);
    CHECK(accesi(ta, ra.carta_b()) > 1000);
    Comandi frecce;
    frecce.ruota = 1;
    ra.comandi(frecce, 0.3);
    CHECK(ra.tempo() > d.coppie[1].t_s + 20.0);
    Comandi dd;
    dd.lettere = {'D'};
    ra.comandi(dd, 0.0);
    CHECK(ra.tempo() == doctest::Approx(d.coppie[1].t_s));
    if (const char* dir = std::getenv("M688_ISTANTANEE")) {
        Palette p = Palette::carica(M688_DATI "/immagini/MAIN.pal"), pa = p;
        s.palette(p);
        for (int k = 0; k < 3; ++k) pagine[static_cast<std::size_t>(k)].salva_bmp(std::string(dir) + "/confronto_" + std::to_string(k + 1) + ".bmp", p);
        ra.palette(pa);
        ra.disegna(ta, f);
        ta.salva_bmp(std::string(dir) + "/confronto_affiancati.bmp", pa);
    }
}

TEST_CASE("confronto A/B: mostra_confronto col suo anello, un clic su una coppia, Esc dagli affiancati, poi Esc") {
    TreCoppie& e = esperimento();
    DatiConfronto d = confronta_tracce(e.a, e.b, e.na, e.nb, 1);
    scrivi_confronto(e.cartella / "confronto.toml", d);
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    const Palette base = Palette::carica(M688_DATI "/immagini/MAIN.pal");
    Tela t;
    // dove cade la prima riga delle coppie (lo stesso disegno della schermata)
    SchermataConfronto righe(M688_DATI, d, e.cartella);
    righe.pagina(2);
    righe.disegna(t, f);
    REQUIRE_FALSE(righe.righe_coppie().empty());
    // la pagina delle coppie (3), un clic sulla prima riga, Esc dagli affiancati, Esc dal confronto
    std::vector<Comandi> copione(6);
    copione[0].cifre = {3};
    copione[2].clic = {{100, righe.righe_coppie()[0].y + 4}};
    copione[4].esc = true;
    copione[5].esc = true;
    std::size_t passo = 0;
    int presentate = 0, senza_musica = 0;
    AnelloFinestra w{[&] { return passo < copione.size() ? copione[passo++] : Comandi{}; },
                     [&](const Tela&, const Palette&, const std::string& brano) {
                         ++presentate;
                         if (brano.empty()) ++senza_musica;   // gli affiancati: i replay sono in silenzio
                     },
                     [] { return 0.0; }};
    CHECK(mostra_confronto(w, t, f, base, M688_DATI, e.cartella / "confronto.toml"));
    CHECK(passo == 6);
    CHECK(senza_musica >= 1);
    CHECK(presentate >= 3);
    Comandi chiudi;
    chiudi.esci = true;
    AnelloFinestra w2{[&] { return chiudi; }, [](const Tela&, const Palette&, const std::string&) {}, [] { return 0.0; }};
    CHECK_FALSE(mostra_confronto(w2, t, f, base, M688_DATI, e.cartella / "confronto.toml"));
}

// gli esperimenti pronti (data/motore/esperimenti, trailer-1) si applicano tutti nella loro scena: la S5 del convoglio
// con le forze, i siluri e i danni della simulazione, come la sandbox; ogni intervento applicato, nessuno fallito
TEST_CASE("confronto A/B: gli esperimenti pronti si applicano nella scena S5, nessun intervento fallisce") {
    const auto classi = carica_classi(M688_DATI "/classi.toml");
    int n = 0;
    for (const auto& voce : std::filesystem::directory_iterator(std::filesystem::path(M688_DATI) / "esperimenti")) {
        if (voce.path().extension() != ".m688v") continue;
        CAPTURE(voce.path().filename().string());
        const Esperimento e = leggi_esperimento(voce.path());
        REQUIRE_FALSE(e.interventi.empty());
        Missione m(M688_MISSIONI "/prove/PROVA_S5_CONVOGLIO.lua", 1);
        m.acustica_enhanced(M688_DATI);
        m.senza_giocatore(true);
        DanniMissione dm(classi, M688_DATI, 1, RegolaDanni::simulazione);
        double fine = 0.0;
        for (const Intervento& x : e.interventi) {
            REQUIRE(m.intervieni(x).ok);
            fine = std::max(fine, x.tempo);
        }
        for (int k = 0; k < static_cast<int>((fine + 5.0) * 9.0); ++k) {
            m.avanza(1.0 / 9.0);
            dm.avanza(m, 1.0 / 9.0);
            m.raccogli_danni(nullptr, &dm);
        }
        for (const StatoIntervento& st : m.stati_interventi()) {
            CHECK(st.applicato);
            CHECK_FALSE(st.fallito);
            if (st.fallito) MESSAGE(st.motivo_it);
        }
        ++n;
    }
    CHECK(n >= 7);
}
