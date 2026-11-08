#include <doctest.h>

#include <cmath>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include "nucleo/mixer_audio.hpp"
#include "nucleo/musica_fm.hpp"

using namespace m688;

// La musica FM dal vivo (roadmap 38, A): il lettore MIDI, il suonatore OPL3 di ymfm, il loop, la dissolvenza, il canale
// musica del Mixer

namespace {

void vlq(std::vector<std::uint8_t>& b, std::uint32_t v) {
    std::uint8_t t[4];
    int n = 0;
    do {
        t[n++] = static_cast<std::uint8_t>(v & 0x7F);
        v >>= 7;
    } while (v);
    while (n--) b.push_back(static_cast<std::uint8_t>(t[n] | (n ? 0x80 : 0)));
}

void meta_testo(std::vector<std::uint8_t>& t, std::uint32_t delta, const std::string& s) {
    vlq(t, delta);
    t.insert(t.end(), {0xFF, 0x06});
    vlq(t, static_cast<std::uint32_t>(s.size()));
    t.insert(t.end(), s.begin(), s.end());
}

// un La a 440 Hz di mezzo secondo (il flauto, quasi una sinusoide) in un brano di un secondo, coi marcatori del loop
std::vector<std::uint8_t> la_440() {
    std::vector<std::uint8_t> t;
    vlq(t, 0);
    t.insert(t.end(), {0xFF, 0x51, 0x03, 0x07, 0xA1, 0x20});   // 500.000 us la semiminima: 96 tick = 0,5 s
    meta_testo(t, 0, "loop_inizio");
    vlq(t, 0);
    t.insert(t.end(), {0xC0, 73});   // il flauto
    vlq(t, 0);
    t.insert(t.end(), {0x90, 69, 100});
    vlq(t, 96);
    t.insert(t.end(), {0x80, 69, 0});
    meta_testo(t, 96, "loop_fine");
    vlq(t, 0);
    t.insert(t.end(), {0xFF, 0x2F, 0x00});
    std::vector<std::uint8_t> b = {'M', 'T', 'h', 'd', 0, 0, 0, 6, 0, 0, 0, 1, 0, 96, 'M', 'T', 'r', 'k'};
    const auto n = static_cast<std::uint32_t>(t.size());
    b.insert(b.end(), {static_cast<std::uint8_t>(n >> 24), static_cast<std::uint8_t>(n >> 16), static_cast<std::uint8_t>(n >> 8),
                       static_cast<std::uint8_t>(n)});
    b.insert(b.end(), t.begin(), t.end());
    return b;
}

double rms(const std::vector<float>& x, std::size_t da, std::size_t a) {
    double s = 0.0;
    for (std::size_t i = da; i < a && i < x.size(); ++i) s += static_cast<double>(x[i]) * x[i];
    return std::sqrt(s / static_cast<double>(std::max<std::size_t>(1, a - da)));
}

BancoFm banco() {
    static const BancoFm b = carica_banco_fm(M688_DATI "/timbri_fm.toml");
    return b;
}

}  // namespace

TEST_CASE("musica FM: il lettore MIDI coi tempi e i marcatori del loop") {
    const BranoMidi b = leggi_midi(la_440());
    REQUIRE(b.eventi.size() == 3);   // programma, accensione, spegnimento
    CHECK(b.eventi[1].t_s == doctest::Approx(0.0));
    CHECK(b.eventi[2].t_s == doctest::Approx(0.5));
    CHECK(b.loop_inizio_s == doctest::Approx(0.0));
    CHECK(b.loop_fine_s == doctest::Approx(1.0));
    CHECK(b.durata_s == doctest::Approx(1.0));
    CHECK_THROWS(leggi_midi(std::vector<std::uint8_t>{'R', 'I', 'F', 'F'}));
}

TEST_CASE("musica FM: il banco dei timbri" * doctest::skip(!std::filesystem::exists(M688_DATI "/timbri_fm.toml"))) {
    const BancoFm b = banco();
    CHECK(b.programmi.size() >= 20);
    CHECK(b.programmi.count(73));
    CHECK(b.ritmo.count(36));
    CHECK(b.programmi.at(48).nome == "archi");
}

TEST_CASE("musica FM: il La suona a 440 Hz, la coda finisce, e lo stesso brano da' lo stesso segnale" *
          doctest::skip(!std::filesystem::exists(M688_DATI "/timbri_fm.toml"))) {
    SuonatoreFm s(banco());
    const int fs = s.campionamento_hz();
    CHECK(fs == doctest::Approx(49716).epsilon(0.001));   // 14.318.180 Hz / 288
    s.suona(leggi_midi(la_440()), false);
    const std::vector<float> x = s.genera(static_cast<std::size_t>(fs));   // un secondo
    CHECK(rms(x, static_cast<std::size_t>(0.1 * fs), static_cast<std::size_t>(0.4 * fs)) > 0.01);
    // la frequenza dagli attraversamenti dello zero (in salita) fra 0,1 e 0,4 s
    int salite = 0;
    for (std::size_t i = static_cast<std::size_t>(0.1 * fs); i < static_cast<std::size_t>(0.4 * fs); ++i)
        salite += x[i - 1] < 0.0f && x[i] >= 0.0f;
    CHECK(salite / 0.3 == doctest::Approx(440.0).epsilon(0.03));
    // dopo lo spegnimento il rilascio, poi il silenzio
    CHECK(rms(x, static_cast<std::size_t>(0.9 * fs), static_cast<std::size_t>(fs)) < 0.2 * rms(x, static_cast<std::size_t>(0.1 * fs), static_cast<std::size_t>(0.4 * fs)));
    CHECK(s.suona_ancora());
    s.genera(static_cast<std::size_t>(4 * fs));   // la coda di 3 s dopo l'ultimo evento
    CHECK_FALSE(s.suona_ancora());
    CHECK(rms(s.genera(1000), 0, 1000) == 0.0);
    // deterministico
    SuonatoreFm a(banco()), c(banco());
    a.suona(leggi_midi(la_440()), false);
    c.suona(leggi_midi(la_440()), false);
    CHECK(a.genera(20000) == c.genera(20000));
}

TEST_CASE("musica FM: il loop torna all'inizio, la dissolvenza ferma" * doctest::skip(!std::filesystem::exists(M688_DATI "/timbri_fm.toml"))) {
    SuonatoreFm s(banco());
    const auto fs = static_cast<std::size_t>(s.campionamento_hz());
    s.suona(leggi_midi(la_440()), true);
    const std::vector<float> x = s.genera(fs * 13 / 10);   // 1,3 s: il La di nuovo dopo il secondo
    const double prima = rms(x, fs / 10, fs * 4 / 10);
    CHECK(rms(x, fs * 11 / 10, fs * 13 / 10) == doctest::Approx(prima).epsilon(0.2));
    CHECK(s.tempo_s() < 0.5);
    CHECK(s.suona_ancora());
    s.ferma(0.5);
    s.genera(fs * 6 / 10);
    CHECK_FALSE(s.suona_ancora());
    // il volume
    SuonatoreFm m(banco());
    m.volume(0.5f);
    m.suona(leggi_midi(la_440()), false);
    const std::vector<float> y = m.genera(fs / 2);
    CHECK(rms(y, fs / 10, fs * 4 / 10) == doctest::Approx(0.5 * prima).epsilon(0.05));
}

TEST_CASE("mixer: la musica FM si somma dopo la resa a 8 bit, senza perdere i dettagli") {
    ParametriMixer p;
    p.resa = Resa::sound_blaster;
    // una sinusoide piu' piccola di mezzo gradino a 8 bit (1/255): sul canale dei campioni diventa un'onda quadra fra due
    // gradini, sulla musica resta com'e'
    std::vector<float> piccolo(4800);
    for (std::size_t i = 0; i < piccolo.size(); ++i)
        piccolo[i] = 0.002f * static_cast<float>(std::sin(2.0 * 3.14159265358979 * 1000.0 * static_cast<double>(i) / 48000.0));
    const auto scarto = [&](const std::vector<float>& y) {
        double e = 0.0;
        for (std::size_t i = 2400; i < y.size(); ++i) e = std::max(e, static_cast<double>(std::fabs(y[i] - piccolo[i])));
        return e;
    };
    Mixer m(p);
    m.aggiungi(Canale::bordo, piccolo, 48000);
    CHECK(scarto(m.produci(4800)) > 0.002);
    Mixer f(p);
    f.aggiungi(Canale::musica, piccolo, 48000);
    CHECK(scarto(f.produci(4800)) < 1e-6);
    // gli effetti sono PCM: passano dalla resa come i campioni
    Mixer e(p);
    e.aggiungi(Canale::effetti, piccolo, 48000);
    CHECK(scarto(e.produci(4800)) > 0.002);
}
