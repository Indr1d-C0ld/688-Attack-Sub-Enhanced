// Ascolto sintetico (S5d): righe e rumore del fascio in campioni PCM, ottava, accelerazione, WAV.
#include <doctest.h>

#include <cmath>
#include <filesystem>

#include "nucleo/ascolto.hpp"

using namespace m688;
namespace fs = std::filesystem;

namespace {

const CatalogoAcustico& catalogo() {
    static const CatalogoAcustico c = carica_acustica(M688_DATI "/acustica.toml");
    return c;
}

struct Scena {
    Ambiente amb;
    Ricevitore r;
    std::vector<Sorgente> s;
};

Scena charlie() {
    static const ClimaTeatro c = carica_clima(M688_DATI "/teatri/faroe.suono.toml");
    Scena x;
    x.amb.profilo = &c.profilo(2);
    x.amb.fondale_m = 3000.0;
    x.amb.vento_m_s = 7.0;
    x.amb.traffico = c.traffico;
    x.r.quota_m = 150.0;
    x.r.velocita_kn = 5.0;
    x.r.sensore = catalogo().sensore("tb16");
    UnitaAcustica u;
    u.id = 2;
    u.classe = "charlie2";
    u.categoria = "sottomarino";
    u.quota_m = 150.0;
    u.velocita_kn = 8.0;
    u.pos = {2000.0, 0.0};
    x.s.push_back(sorgente_acustica(catalogo(), u));
    return x;
}

double rms(const Suono& s) {
    double q = 0.0;
    for (const auto c : s.campioni) q += (c / 32767.0) * (c / 32767.0);
    return std::sqrt(q / static_cast<double>(s.campioni.size()));
}

}  // namespace

TEST_CASE("ascolto: rumore colorato e righe del fascio, ripetibili dal seme") {
    const Scena x = charlie();
    ParametriAscolto p;
    p.f_min_hz = 20.0;
    p.f_max_hz = 2000.0;
    const Suono a = sintetizza(x.s, x.r, x.amb, 90.0, p);
    CHECK(a.campioni.size() == 44100);
    CHECK(rms(a) > 0.01);
    // la riga dei 250 Hz si sente sopra il rumore accanto
    double riga = 0.0;
    for (const auto& r : righe_ricevute(x.s, x.r, x.amb, 90.0, 240.0, 260.0, 0.1))
        if (std::fabs(r.hz_emessa - 250.0) < 1e-6) riga = r.hz;
    REQUIRE(riga > 0.0);
    CHECK(ampiezza_a(a, riga) > 3.0 * ampiezza_a(a, riga + 7.3));
    // stesso seme, stessi campioni; altro seme, altro rumore
    CHECK(sintetizza(x.s, x.r, x.amb, 90.0, p).campioni == a.campioni);
    ParametriAscolto q = p;
    q.seme = 99;
    CHECK(sintetizza(x.s, x.r, x.amb, 90.0, q).campioni != a.campioni);
    // il fascio puntato altrove: la riga sparisce
    const Suono altrove = sintetizza(x.s, x.r, x.amb, 200.0, p);
    CHECK(ampiezza_a(altrove, riga) < 0.3 * ampiezza_a(a, riga));
}

TEST_CASE("ascolto: righe basse su di ottava, ascolto accelerato, filtro passa banda") {
    const Scena x = charlie();
    double bassa = 0.0;   // la pompa del Charlie a 31 Hz
    for (const auto& r : righe_ricevute(x.s, x.r, x.amb, 90.0, 25.0, 35.0, 0.1))
        if (std::fabs(r.hz_emessa - 31.0) < 1e-6) bassa = r.hz;
    REQUIRE(bassa > 0.0);
    ParametriAscolto p;
    p.f_min_hz = 50.0;
    p.f_max_hz = 2000.0;
    const Suono senza = sintetizza(x.s, x.r, x.amb, 90.0, p);
    p.ottava_su = true;
    const Suono con = sintetizza(x.s, x.r, x.amb, 90.0, p);
    CHECK(ampiezza_a(con, 2.0 * bassa) > 1.5 * ampiezza_a(senza, 2.0 * bassa));   // debole sul continuo, ma c'e'
    // x4: la riga dei 31 Hz suona a 124 Hz
    ParametriAscolto v;
    v.f_min_hz = 50.0;
    v.f_max_hz = 2000.0;
    v.accelerazione = 4.0;
    const Suono veloce = sintetizza(x.s, x.r, x.amb, 90.0, v);
    CHECK(ampiezza_a(veloce, 4.0 * bassa) > 1.8 * ampiezza_a(veloce, 4.0 * bassa + 2.0));   // ~9 dB sul continuo
    // il passa banda toglie la riga dei 250 Hz
    ParametriAscolto f;
    f.f_min_hz = 400.0;
    f.f_max_hz = 2000.0;
    const Suono alto = sintetizza(x.s, x.r, x.amb, 90.0, f);
    p.ottava_su = false;
    p.f_min_hz = 20.0;
    CHECK(ampiezza_a(alto, 250.0) < 0.2 * ampiezza_a(sintetizza(x.s, x.r, x.amb, 90.0, p), 250.0));
}

TEST_CASE("ascolto: WAV a 16 bit mono") {
    const Scena x = charlie();
    ParametriAscolto p;
    p.durata_s = 0.5;
    const Suono s = sintetizza(x.s, x.r, x.amb, 90.0, p);
    const fs::path f = fs::temp_directory_path() / "m688_prova_ascolto.wav";
    REQUIRE(scrivi_wav(f.string(), s));
    CHECK(fs::file_size(f) == 44 + 2 * s.campioni.size());
    fs::remove(f);
    CHECK_FALSE(scrivi_wav("/non/esiste/x.wav", s));
}

TEST_CASE("ascolto: la cuffia in tempo reale e' continua e non dipende da come si spezzano i passi") {
    const Scena x = charlie();
    ParametriAscolto p;
    p.f_min_hz = 20.0;
    p.f_max_hz = 2000.0;
    CuffiaSonar intera(p), a_pezzi(p), lunga(p);
    const std::vector<float> a = intera.passo(x.s, x.r, x.amb, 90.0, 2.0);
    std::vector<float> b;
    for (int i = 0; i < 20; ++i) {
        const auto v = a_pezzi.passo(x.s, x.r, x.amb, 90.0, 0.1);
        b.insert(b.end(), v.begin(), v.end());
    }
    REQUIRE(a.size() == 44100);
    REQUIRE(b.size() == a.size());
    double diff = 0.0;   // uguali salvo gli arrotondamenti della fase ridotta a fine passo
    for (std::size_t i = 0; i < a.size(); ++i) diff = std::max(diff, static_cast<double>(std::fabs(a[i] - b[i])));
    CHECK(diff < 1e-4);
    // la riga dei 250 Hz sta ~15 dB sopra il continuo in 0,1 Hz: in 4 s d'ascolto (0,25 Hz) si sente
    double riga = 0.0;
    for (const auto& r : righe_ricevute(x.s, x.r, x.amb, 90.0, 240.0, 260.0, 0.1))
        if (std::fabs(r.hz_emessa - 250.0) < 1e-6) riga = r.hz;
    const auto l = lunga.passo(x.s, x.r, x.amb, 90.0, 5.0);
    Suono s;
    s.campionamento_hz = 22050;
    for (std::size_t i = 22050; i < l.size(); ++i) s.campioni.push_back(static_cast<std::int16_t>(std::lround(l[i] * 32767.0f)));
    // (con le frange d'interferenza, nucleo/interferenza, a 2 km la riga cade 1,5 dB in una frangia: il rapporto da 2,6
    // scende a 2,4)
    CHECK(ampiezza_a(s, riga) > 2.2 * ampiezza_a(s, riga + 1.3));
    // niente sorgenti: solo il fondo, a -24 dBFS rms
    CuffiaSonar vuota(p);
    const auto f = vuota.passo({}, x.r, x.amb, 90.0, 2.0);
    double q = 0.0;
    for (std::size_t i = 4096; i < f.size(); ++i) q += f[i] * f[i];
    CHECK(20.0 * std::log10(std::sqrt(q / static_cast<double>(f.size() - 4096))) == doctest::Approx(-24.0).epsilon(0.08));
    // il passo tiene il resto dei campioni: 3 passi da 1/60 s fanno 1102 o 1103 campioni in tutto
    CuffiaSonar t(p);
    std::size_t n = 0;
    for (int i = 0; i < 3; ++i) n += t.passo(x.s, x.r, x.amb, 90.0, 1.0 / 60.0).size();
    CHECK(n == 1102);
}

TEST_CASE("ascolto: la cuffia segue il Doppler e il fascio") {
    Scena x = charlie();
    ParametriAscolto p;
    p.f_min_hz = 20.0;
    p.f_max_hz = 2000.0;
    CuffiaSonar c(p);
    const auto frequenza = [&](const Scena& sc) {
        for (const auto& r : righe_ricevute(sc.s, sc.r, sc.amb, 90.0, 240.0, 260.0, 0.1))
            if (std::fabs(r.hz_emessa - 250.0) < 1e-6) return r.hz;
        return 0.0;
    };
    const auto suono = [](const std::vector<float>& v) {
        Suono s;
        s.campionamento_hz = 22050;
        for (std::size_t i = v.size() / 2; i < v.size(); ++i) s.campioni.push_back(static_cast<std::int16_t>(std::lround(v[i] * 32767.0f)));
        return s;
    };
    const double ferma = frequenza(x);
    const auto prima = suono(c.passo(x.s, x.r, x.amb, 90.0, 6.0));
    // il Charlie accelera verso il ricevitore: la riga sale
    UnitaAcustica u;
    u.id = 2;
    u.classe = "charlie2";
    u.categoria = "sottomarino";
    u.quota_m = 150.0;
    u.velocita_kn = 12.0;
    u.rotta = 270.0;
    u.pos = {2000.0, 0.0};
    x.s = {sorgente_acustica(catalogo(), u)};
    const double avvicina = frequenza(x);
    REQUIRE(avvicina > ferma + 0.5);
    const auto dopo = suono(c.passo(x.s, x.r, x.amb, 90.0, 6.0));
    // nell'ultima meta' (3 s, 0,33 Hz) la riga sta sulla frequenza nuova
    CHECK(ampiezza_a(dopo, avvicina) > 1.5 * ampiezza_a(dopo, ferma));
    CHECK(ampiezza_a(prima, ferma) > 1.3 * ampiezza_a(prima, avvicina));   // 1,4 con la riga in una frangia
    // il fascio puntato altrove: la riga si spegne a rampa
    const auto altrove = suono(c.passo(x.s, x.r, x.amb, 200.0, 6.0));
    CHECK(ampiezza_a(altrove, avvicina) < 0.3 * ampiezza_a(dopo, avvicina));
    c.azzera();
    CHECK(c.passo(x.s, x.r, x.amb, 90.0, 0.5).size() == 11025);
}
