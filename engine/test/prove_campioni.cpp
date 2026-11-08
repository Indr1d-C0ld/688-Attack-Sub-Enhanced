// Libreria di campioni (R4) e mescolamento in cuffia (R5): caricamento, scelta, tessitura ricolorata dal modello.
#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <filesystem>

#include "nucleo/acustica_asw.hpp"
#include "nucleo/ascolto.hpp"
#include "nucleo/campioni.hpp"

using namespace m688;

namespace {

const CatalogoAcustico& catalogo() {
    static const CatalogoAcustico c = carica_acustica(M688_DATI "/acustica.toml");
    return c;
}

const LibreriaCampioni& libreria() {
    static const LibreriaCampioni l = carica_campioni(M688_DATI "/campioni.toml", M688_DATI "/..");
    return l;
}

Ambiente mare() {
    Ambiente a;
    a.fondale_m = 3000.0;
    a.vento_m_s = vento_per_stato_del_mare(3);
    return a;
}

// energia in una banda (FFT a pezzi: somma dei moduli quadri dei bin), dB
double banda_db(const std::vector<float>& v, double lo, double hi, int fs = 22050) {
    const std::size_t n = 4096;
    double e = 0.0;
    for (std::size_t a = 0; a + n <= v.size(); a += n)
        for (double f = lo; f < hi; f += static_cast<double>(fs) / n) {
            const double w = 2.0 * 3.14159265358979323846 * f / fs, c = 2.0 * std::cos(w);
            double s1 = 0.0, s2 = 0.0;
            for (std::size_t i = a; i < a + n; ++i) {
                const double s0 = v[i] + c * s1 - s2;
                s2 = s1;
                s1 = s0;
            }
            e += s1 * s1 + s2 * s2 - c * s1 * s2;
        }
    return 10.0 * std::log10(std::max(e, 1e-30));
}

Sorgente nave(Vec2 pos) {
    UnitaAcustica u;
    u.id = 4;
    u.classe = "mercantile";
    u.categoria = "nave";
    u.pos = pos;
    u.velocita_kn = 14.0;
    u.rotta = 0.0;
    return sorgente_acustica(catalogo(), u);
}

}  // namespace

TEST_CASE("campioni: la libreria si carica e sceglie in modo ripetibile") {
    const auto& l = libreria();
    REQUIRE(l.campioni.size() == 40);   // R4: 24 (3 esclusi per licenza); R8: 9 di aerei ed elicotteri (5 esclusi); M: 7 (la coda e il bioduck esclusi)
    CHECK(l.per_voce.at("mercantile").size() == 3);
    for (const auto& c : l.campioni) {
        CHECK_MESSAGE(!c.campioni.empty(), c.id);
        CHECK(c.f_max_hz > c.f_min_hz);
        CHECK(c.inviluppo_tempo.size() == 3);
        CHECK(!c.attribuzione.empty());
    }
    CHECK(l.scegli("mercantile", 5) == l.scegli("mercantile", 5));
    CHECK(l.scegli("nessuna", 5) == nullptr);
    CHECK(l.per_voce.at("t_phase_tettonico").size() == 1);
    CHECK(l.campioni[l.per_voce.at("t_phase_tettonico")[0]].fattore == 10.0);
    // la tessitura e' sbiancata: rms 0,25
    const Campione& m = *l.scegli("mercantile", 1);
    double q = 0.0;
    for (float x : m.campioni) q += static_cast<double>(x) * x;
    CHECK(std::sqrt(q / static_cast<double>(m.campioni.size())) == doctest::Approx(0.25).epsilon(0.05));
    // le voci
    CHECK(voce_campione(nave({0.0, 0.0})) == "mercantile");
    Sorgente b;
    b.fonte = catalogo().sorgente("balenottera_comune");
    CHECK(voce_campione(b) == "balenottera_comune");
    // WAV: andata e ritorno
    Suono s;
    s.campioni = {0, 16384, -16384, 32767};
    const auto f = std::filesystem::temp_directory_path() / "m688_prova_campioni.wav";
    REQUIRE(scrivi_wav(f.string(), s));
    std::vector<float> letti;
    int fs = 0;
    REQUIRE(leggi_wav(f.string(), letti, fs));
    CHECK(fs == 22050);
    CHECK(letti[1] == doctest::Approx(0.5));
    std::filesystem::remove(f);
}

TEST_CASE("campioni: in cuffia la tessitura col livello del modello") {
    Ricevitore r{{0.0, 0.0}, 150.0, 0.0, 5.0, catalogo().sensore("tb16")};
    ParametriAscolto p;
    p.f_min_hz = 20.0;
    p.f_max_hz = 5000.0;
    const auto ascolta = [&](const LibreriaCampioni* l, const std::vector<Sorgente>& s) {
        CuffiaSonar c(p);
        c.imposta_campioni(l);
        std::vector<float> v;
        for (int k = 0; k < 16; ++k) {   // 4 s a passi di 0,25 s
            const auto x = c.passo(s, r, mare(), 90.0, 0.25);
            v.insert(v.end(), x.begin(), x.end());
        }
        return v;
    };
    const std::vector<Sorgente> scena{nave({3000.0, 0.0})};
    const auto modello = ascolta(nullptr, scena), registrato = ascolta(&libreria(), scena);
    CHECK(registrato != modello);
    CHECK(registrato == ascolta(&libreria(), scena));   // ripetibile
    // stesso livello per banda (la tessitura porta la grana, il modello il colore): entro 2,5 dB (2 prima delle frange
    // d'interferenza, nucleo/interferenza: con la nave a 3 km la banda alta scende di una frangia larga, e la tessitura
    // la segue per bande piu' grosse del modello; 2,01 dB a 1600-4000 Hz)
    for (const auto& [lo, hi] : {std::pair{100.0, 400.0}, std::pair{400.0, 1600.0}, std::pair{1600.0, 4000.0}})
        CHECK_MESSAGE(std::fabs(banda_db(registrato, lo, hi) - banda_db(modello, lo, hi)) < 2.5, lo << ": registrato " << banda_db(registrato, lo, hi) << " modello " << banda_db(modello, lo, hi));
}

TEST_CASE("campioni: il vento e la pioggia del rumore ambiente con le loro tessiture") {
    Ricevitore r{{0.0, 0.0}, 150.0, 0.0, 5.0, catalogo().sensore("tb16")};
    ParametriAscolto p;
    p.f_min_hz = 20.0;
    p.f_max_hz = 5000.0;
    const auto ascolta = [&](const LibreriaCampioni* l, const Ambiente& a) {
        CuffiaSonar c(p);
        c.imposta_campioni(l);
        std::vector<float> v;
        for (int k = 0; k < 16; ++k) {
            const auto x = c.passo({}, r, a, 90.0, 0.25);
            v.insert(v.end(), x.begin(), x.end());
        }
        return v;
    };
    Ambiente vento = mare();
    vento.vento_m_s = 12.0;   // mare 5-6: il vento domina sopra 300 Hz
    Ambiente pioggia = vento;
    pioggia.pioggia_mm_h = 10.0;
    for (const Ambiente& a : {vento, pioggia}) {
        const auto modello = ascolta(nullptr, a), registrato = ascolta(&libreria(), a);
        CHECK(registrato != modello);
        for (const auto& [lo, hi] : {std::pair{400.0, 1600.0}, std::pair{1600.0, 4000.0}})
            CHECK_MESSAGE(std::fabs(banda_db(registrato, lo, hi) - banda_db(modello, lo, hi)) < 2.0,
                          lo << ": registrato " << banda_db(registrato, lo, hi) << " modello " << banda_db(modello, lo, hi));
    }
    // le parti del rumore ambiente sommano al totale
    for (double f : {50.0, 500.0, 5000.0}) {
        const ComponentiRumore c = componenti_rumore_ambiente(f, pioggia);
        double somma = 0.0;
        for (double db : {c.turbolenza, c.traffico, c.vento, c.termico, c.pioggia, c.gamberi, c.ghiaccio}) somma += std::pow(10.0, db / 10.0);
        CHECK(10.0 * std::log10(somma) == doctest::Approx(rumore_ambiente(f, pioggia)).epsilon(1e-9));
    }
}

TEST_CASE("campioni: collegati al catalogo, cascata e scoperta seguono il tratto della cuffia") {
    CatalogoAcustico c = carica_acustica(M688_DATI "/acustica.toml");
    Sorgente s;
    s.id = 30;
    s.fonte = c.sorgente("balenottera_comune");
    s.seme = 77;
    s.pos = {5000.0, 0.0};
    s.quota_m = 30.0;
    s.finestra_s = 2.0;
    const auto livello = [&](double t) {
        s.tempo_s = t;
        return livello_totale_fonte(s);
    };
    const double prima = livello(100.0);
    collega_campioni(c, libreria());
    s.fonte = c.sorgente("balenottera_comune");
    REQUIRE(s.fonte->campioni.size() == 1);
    const Campione& cp = *s.fonte->campioni[0];
    // il livello segue l'inviluppo del campione nel tratto letto: due istanti diversi danno livelli diversi, come vuole
    // modulazione_campione
    double minimo = 1e9, massimo = -1e9, media = 0.0;
    int n = 0;
    for (double t = 0.0; t < cp.durata_vera_s(); t += 2.0, ++n) {
        const double l = livello(t);
        minimo = std::min(minimo, l);
        massimo = std::max(massimo, l);
        media += std::pow(10.0, l / 10.0);
    }
    CHECK(massimo - minimo > 3.0);
    // in media il livello di lungo periodo del modello (20 ore: il ritmo va a serie con pause lunghe), entro 2 dB
    static const CatalogoAcustico modello = carica_acustica(M688_DATI "/acustica.toml");
    s.fonte = modello.sorgente("balenottera_comune");
    double media_modello = 0.0;
    int m = 0;
    for (double t = 0.0; t < 72000.0; t += 20.0, ++m) media_modello += std::pow(10.0, livello(t) / 10.0);
    CHECK(std::fabs(10.0 * std::log10(media / n) - 10.0 * std::log10(media_modello / m)) < 2.0);
    CHECK(prima == livello(100.0));   // senza collegamento: il modello di sempre
    // la cuffia legge lo stesso punto che l'attivita' segue
    CHECK(posizione_campione_s(cp, 77, 10.0) == doctest::Approx(std::fmod(77.0 / cp.campionamento_hz + 10.0 / cp.fattore, cp.durata_s())));
}

TEST_CASE("campioni: la modulazione con le somme prefisse e' la media diretta della finestra, anche oltre un giro") {
    if (libreria().vuota()) return;
    // la media diretta di prima, indice per indice ad anello
    const auto diretta = [](const Campione& c, std::uint64_t seme, double t, double w, std::size_t b) {
        const auto& v = c.inviluppo_tempo[b];
        double media = 0.0;
        for (float x : v) media += std::pow(10.0, x / 10.0);
        media /= static_cast<double>(v.size());
        const double fine = posizione_campione_s(c, seme, t) / c.passo_s;
        const double quanti = std::max(1.0, std::floor(std::max(w, 0.0) / c.fattore / c.passo_s));
        double somma = 0.0;
        for (double k = 0.0; k < quanti; k += 1.0) {
            const auto i = static_cast<std::size_t>(std::fmod(std::floor(fine - k) + 1e9 * static_cast<double>(v.size()), static_cast<double>(v.size())));
            somma += std::pow(10.0, v[i] / 10.0);
        }
        return std::clamp(somma / quanti / media, 0.001, 30.0);
    };
    int prove = 0;
    for (const Campione& c : libreria().campioni) {
        if (c.inviluppo_tempo.empty() || c.bande_hz.empty()) continue;
        const double giro = c.durata_vera_s();
        for (std::size_t b = 0; b < c.bande_hz.size() && b < c.inviluppo_tempo.size(); ++b) {
            if (c.inviluppo_tempo[b].empty()) continue;
            const double f = c.bande_hz[b].first + 1e-6;
            for (const double t : {0.0, 0.3, 17.0, giro * 0.5, giro - 0.1, giro * 3.7})
                for (const double w : {0.0, 1.0, 2.0, 30.0, giro, giro * 2.5}) {
                    CHECK(modulazione_campione(c, 77, t, w, f) == doctest::Approx(diretta(c, 77, t, w, b)).epsilon(1e-9));
                    ++prove;
                }
        }
    }
    CHECK(prove > 100);
}

TEST_CASE("campioni: un P-3 sulla verticale suona con la sua tessitura (R8), col livello del modello") {
    Ricevitore r{{0.0, 0.0}, 30.0, 0.0, 3.0, catalogo().sensore("bqq5_sfera")};
    AereoAcustico p3;
    p3.id = 50;
    p3.classe = "p3c";
    p3.pos = {5.0, 0.0};
    p3.quota_m = 91.44;
    p3.velocita_kn = 180.0;
    const auto s = sorgente_aerea(catalogo(), p3, r.pos, r.quota_m, 10.0, 2.0);
    REQUIRE(s.has_value());
    CHECK(voce_campione(*s) == "aereo_p3c");
    CHECK(libreria().scegli("aereo_p3c", 1) != nullptr);
    CHECK(libreria().scegli("elicottero_ka27", 1) == nullptr);   // Ka-27 e Ka-25 senza campione: il modello di sempre
    ParametriAscolto p;
    p.f_min_hz = 20.0;
    p.f_max_hz = 2000.0;
    const auto ascolta = [&](const LibreriaCampioni* l) {
        CuffiaSonar c(p);
        c.imposta_campioni(l);
        std::vector<float> v;
        for (int k = 0; k < 32; ++k) {   // il tempo di gioco avanza con l'audio, come in partita
            const auto t = sorgente_aerea(catalogo(), p3, r.pos, r.quota_m, 10.0 + 0.25 * k, 2.0);
            const auto x = c.passo({*t}, r, mare(), 90.0, 0.25);
            v.insert(v.end(), x.begin(), x.end());
        }
        return v;
    };
    const auto modello = ascolta(nullptr), registrato = ascolta(&libreria());
    CHECK(registrato != modello);
    for (const auto& [lo, hi] : {std::pair{100.0, 400.0}, std::pair{400.0, 1600.0}})
        CHECK_MESSAGE(std::fabs(banda_db(registrato, lo, hi) - banda_db(modello, lo, hi)) < 2.0,
                      lo << ": registrato " << banda_db(registrato, lo, hi) << " modello " << banda_db(modello, lo, hi));
}

TEST_CASE("campioni: le registrazioni di M (biologici e ghiaccio) hanno la loro voce e i crediti") {
    const auto& l = libreria();
    for (const char* v : {"capodoglio", "foca_barbata", "tricheco", "balenottera_minore_pacifica"}) CHECK_MESSAGE(l.per_voce.count(v), v);
    CHECK(l.per_voce.at("fessurazione_ghiaccio").size() == 2);
    CHECK(l.per_voce.at("orca").size() == 2);
    CHECK_FALSE(l.per_voce.count("balenottera_minore_antartica"));   // nessun teatro antartico
    for (const char* id : {"capodoglio", "foca_barbata", "fessurazione_ghiaccio", "fessurazione_ghiaccio_2", "tricheco",
                           "balenottera_minore_pacifica", "orca_2"}) {
        const auto it = std::find_if(l.campioni.begin(), l.campioni.end(), [&](const Campione& c) { return c.id == id; });
        REQUIRE_MESSAGE(it != l.campioni.end(), id);
        CHECK_MESSAGE(catalogo().sorgente(it->voce) != nullptr, id);   // la voce e' una sorgente della libreria
        CHECK_MESSAGE(!it->attribuzione.empty(), id);
        if (it->licenza.rfind("CC BY", 0) == 0) CHECK_MESSAGE(it->attribuzione.find("creativecommons.org") != std::string::npos, id);
    }
}
