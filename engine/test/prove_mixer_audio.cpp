// Mixer dell'audio (R1): canali, ricampionamento continuo, volumi, rese piena, originale e Sound Blaster.
#include <doctest.h>

#include <cmath>
#include <set>
#include <string>

#include "nucleo/mixer_audio.hpp"

using namespace m688;

namespace {

constexpr double PI_P = 3.14159265358979323846;

std::vector<float> seno(double hz, int fs, std::size_t n, double ampiezza = 0.5, double fase0 = 0.0) {
    std::vector<float> v(n);
    for (std::size_t i = 0; i < n; ++i) v[i] = static_cast<float>(ampiezza * std::sin(2.0 * PI_P * hz * static_cast<double>(i) / fs + fase0));
    return v;
}

double ampiezza(const std::vector<float>& x, double hz, int fs) {   // Goertzel
    const double w = 2.0 * PI_P * hz / fs, c = 2.0 * std::cos(w);
    double s1 = 0.0, s2 = 0.0;
    for (const float v : x) {
        const double s0 = v + c * s1 - s2;
        s2 = s1;
        s1 = s0;
    }
    return 2.0 * std::hypot(s1 - s2 * std::cos(w), s2 * std::sin(w)) / static_cast<double>(x.size());
}

}  // namespace

TEST_CASE("mixer audio: canali, volumi e silenzio") {
    ParametriMixer p;
    p.volume = {1.0f, 0.5f, 0.25f};
    Mixer m(p);
    CHECK(m.produci(10) == std::vector<float>(10, 0.0f));   // vuoto: silenzio
    m.aggiungi(Canale::cuffia, std::vector<float>(100, 0.2f), 48000);
    m.aggiungi(Canale::bordo, std::vector<float>(50, 0.4f), 48000);
    CHECK(m.in_attesa(Canale::cuffia) == 100);
    const auto x = m.produci(100);
    CHECK(x[0] == doctest::Approx(0.2f + 0.5f * 0.4f));
    CHECK(x[60] == doctest::Approx(0.2f));               // il bordo e' finito a 50
    CHECK(m.in_attesa(Canale::cuffia) == 0);
    // la somma non va oltre la scala
    m.aggiungi(Canale::cuffia, std::vector<float>(4, 0.9f), 48000);
    m.aggiungi(Canale::interfaccia, std::vector<float>(4, 0.9f), 48000);
    CHECK(m.produci(4)[0] <= 1.0f);
    CHECK(Mixer::a_16_bit({1.0f, -1.0f, 0.0f}) == std::vector<std::int16_t>{32767, -32767, 0});
    CHECK(std::string(nome_resa(Resa::sound_blaster)) == "Sound Blaster");
}

TEST_CASE("mixer audio: ricampionamento continuo fra un blocco e l'altro") {
    // 1 kHz a 22.050 Hz in un blocco solo o in 37 blocchi da 600: stessa uscita a 48 kHz
    const auto ingresso = seno(1000.0, 22050, 22200);
    Mixer intero, a_pezzi;
    intero.aggiungi(Canale::cuffia, ingresso, 22050);
    for (std::size_t i = 0; i < ingresso.size(); i += 600)
        a_pezzi.aggiungi(Canale::cuffia, std::vector<float>(ingresso.begin() + static_cast<long>(i),
                                                            ingresso.begin() + static_cast<long>(std::min(i + 600, ingresso.size()))), 22050);
    REQUIRE(intero.in_attesa(Canale::cuffia) == a_pezzi.in_attesa(Canale::cuffia));
    const std::size_t n = intero.in_attesa(Canale::cuffia);
    CHECK(n == doctest::Approx(22200.0 * 48000.0 / 22050.0).epsilon(0.001));
    const auto a = intero.produci(n), b = a_pezzi.produci(n);
    double diff = 0.0;
    for (std::size_t i = 0; i < n; ++i) diff = std::max(diff, static_cast<double>(std::fabs(a[i] - b[i])));
    CHECK(diff < 1e-5);
    // la frequenza resta 1 kHz
    CHECK(ampiezza(a, 1000.0, 48000) == doctest::Approx(0.5).epsilon(0.02));
    CHECK(ampiezza(a, 2205.0, 48000) < 0.01);
}

TEST_CASE("mixer audio: le rese d'epoca") {
    const auto ingresso = seno(440.0, 48000, 48000, 0.8);
    const auto rendi = [&](Resa r) {
        ParametriMixer p;
        p.resa = r;
        p.accoppiamento_alternata = false;   // i gradini nudi; l'alternata in fondo
        Mixer m(p);
        m.aggiungi(Canale::cuffia, ingresso, 48000);
        return m.produci(48000);
    };
    // piena: identica all'ingresso
    const auto piena = rendi(Resa::piena);
    CHECK(piena == ingresso);
    // originale: 16 livelli, tenuti a ~5,5 kHz
    const auto orig = rendi(Resa::originale);
    std::set<float> livelli(orig.begin(), orig.end());
    CHECK(livelli.size() <= 16);
    int cambi = 0;
    for (std::size_t i = 1; i < orig.size(); ++i) cambi += orig[i] != orig[i - 1];
    CHECK(cambi <= 5500);
    CHECK(ampiezza(orig, 440.0, 48000) == doctest::Approx(0.8).epsilon(0.1));   // la musica resta riconoscibile
    // Sound Blaster a 22.050 Hz senza anti-aliasing: un 15 kHz si ripiega a 7.050 Hz
    ParametriMixer p;
    p.resa = Resa::sound_blaster;
    Mixer m(p);
    m.aggiungi(Canale::cuffia, seno(15000.0, 48000, 48000, 0.5), 48000);
    const auto sb = m.produci(48000);
    CHECK(ampiezza(sb, 7050.0, 48000) > 0.05);
    ParametriMixer q = p;
    q.resa = Resa::piena;
    Mixer pulito(q);
    pulito.aggiungi(Canale::cuffia, seno(15000.0, 48000, 48000, 0.5), 48000);
    CHECK(ampiezza(pulito.produci(48000), 7050.0, 48000) < 0.001);
    // ripetibile
    CHECK(rendi(Resa::sound_blaster) == rendi(Resa::sound_blaster));
    // niente continua: il silenzio e' 0 esatto (il livello di riposo 8 su 16 non esce +1/15) e il seno ha media nulla
    for (const Resa r : {Resa::originale, Resa::sound_blaster}) {
        ParametriMixer s;
        s.resa = r;
        Mixer muto(s);
        muto.aggiungi(Canale::interfaccia, std::vector<float>(4800, 0.0f), 48000);
        for (const float x : muto.produci(4800)) CHECK(x == 0.0f);
    }
    ParametriMixer a;
    a.resa = Resa::originale;
    Mixer accoppiato(a);
    accoppiato.aggiungi(Canale::cuffia, ingresso, 48000);
    const auto ac = accoppiato.produci(48000);
    double media = 0.0;
    for (std::size_t i = 4800; i < ac.size(); ++i) media += ac[i];
    CHECK(std::fabs(media / static_cast<double>(ac.size() - 4800)) < 0.005);
    CHECK(ampiezza(ac, 440.0, 48000) == doctest::Approx(0.8).epsilon(0.1));   // a 440 Hz l'alternata non toglie niente
}
