#include <doctest.h>

#include <cstring>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <vector>

#include "../app/effetti_bordo.hpp"
#include "../app/regia_audio.hpp"
#include "missioni/missione.hpp"

using namespace m688;

// La regia dell'audio (app/regia_audio, roadmap 38) senza dispositivo: la musica per schermata con la dissolvenza, gli
// effetti sommati e tagliati, il muto.

namespace {

double energia(const std::vector<float>& x) {
    double e = 0.0;
    for (const float v : x) e += static_cast<double>(v) * v;
    return x.empty() ? 0.0 : std::sqrt(e / static_cast<double>(x.size()));
}

// un WAV a 16 bit stereo, come quelli resi con l'MT-32
void scrivi_wav(const std::filesystem::path& p, const std::vector<float>& x, int hz) {
    std::ofstream f(p, std::ios::binary);
    const auto u32 = [&](std::uint32_t v) { f.write(reinterpret_cast<const char*>(&v), 4); };
    const auto u16 = [&](std::uint16_t v) { f.write(reinterpret_cast<const char*>(&v), 2); };
    const auto dati = static_cast<std::uint32_t>(x.size() * 4);
    f.write("RIFF", 4), u32(36 + dati), f.write("WAVEfmt ", 8), u32(16), u16(1), u16(2), u32(static_cast<std::uint32_t>(hz)),
        u32(static_cast<std::uint32_t>(hz) * 4), u16(4), u16(16), f.write("data", 4), u32(dati);
    for (const float v : x) {
        const auto s = static_cast<std::int16_t>(v * 32767.0f);
        f.write(reinterpret_cast<const char*>(&s), 2), f.write(reinterpret_cast<const char*>(&s), 2);
    }
}

// s secondi d'uscita, a blocchi di 0,05 s come i fotogrammi
std::vector<float> secondi(RegiaAudio& r, double s) {
    std::vector<float> tutto;
    for (int k = 0; k < static_cast<int>(s / 0.05 + 0.5); ++k) {
        const auto b = r.produci(2400);
        tutto.insert(tutto.end(), b.begin(), b.end());
    }
    return tutto;
}

}  // namespace

TEST_CASE("regia audio: la musica della schermata, il cambio con la dissolvenza, lo stesso brano non riparte") {
    RegiaAudio r(M688_DATI);
    CHECK(energia(secondi(r, 0.5)) < 1e-6);   // nessun brano: silenzio
    r.musica("menu");
    CHECK(r.brano() == "menu");
    CHECK(energia(secondi(r, 3.0)) > 1e-3);
    CHECK(r.musica_in_corso());
    r.musica("menu");   // lo stesso: continua
    r.musica("albo");
    CHECK(r.brano() == "albo");
    secondi(r, 1.5);    // dopo la dissolvenza di 1 s parte l'albo
    CHECK(r.musica_in_corso());
    CHECK(energia(secondi(r, 2.0)) > 1e-3);
    r.musica("");
    secondi(r, 1.5);
    CHECK_FALSE(r.musica_in_corso());
    CHECK(energia(secondi(r, 0.5)) < 1e-6);
}

TEST_CASE("regia audio: gli effetti si sommano, gli allarmi lunghi si tagliano, il muto spegne tutto") {
    RegiaAudio r(M688_DATI);
    REQUIRE(r.ha_effetto("clic_interfaccia"));
    REQUIRE(r.ha_effetto("allarme_collisione"));
    CHECK_FALSE(r.effetto("inesistente"));
    CHECK(r.effetto("allarme_collisione"));   // 13 s, tagliato a 4
    CHECK(r.effetto("clic_interfaccia"));     // insieme, non dopo
    CHECK(r.effetti_in_corso() == 2);
    const auto primo = secondi(r, 0.5);
    CHECK(energia(primo) > 1e-3);
    secondi(r, 3.7);
    CHECK(r.effetti_in_corso() == 0);         // finito entro 4 s (piu' il blocco)
    CHECK(r.effetto("tromba_immersione_h8", 1.0f, 0.0));   // intero: 4,1 s
    secondi(r, 3.9);
    CHECK(r.effetti_in_corso() == 1);
    secondi(r, 0.5);
    CHECK(r.effetti_in_corso() == 0);
    r.muto(true);
    r.effetto("clic_interfaccia");
    r.musica("museo");
    CHECK(energia(secondi(r, 1.0)) < 1e-6);
    r.muto(false);
    CHECK(energia(secondi(r, 2.0)) > 1e-3);
}

TEST_CASE("regia audio: un PCM gia' reso come musica, una volta") {
    RegiaAudio r(M688_DATI);
    std::vector<float> tono(11025);   // mezzo secondo a 22.050 Hz
    for (std::size_t i = 0; i < tono.size(); ++i) tono[i] = 0.5f * static_cast<float>(std::sin(2.0 * 3.14159265 * 440.0 * static_cast<double>(i) / 22050.0));
    r.musica_pcm("titoli", tono, 22050);
    CHECK(r.musica_in_corso());
    CHECK(energia(secondi(r, 0.4)) > 1e-2);
    secondi(r, 0.3);
    CHECK_FALSE(r.musica_in_corso());
    r.musica("menu");   // dal PCM a un brano FM: parte subito
    CHECK(energia(secondi(r, 2.0)) > 1e-3);
}

TEST_CASE("regia audio: l'uscita MT-32 dalla cache dell'utente, alzata e in loop; senza il WAV, l'FM") {
    const auto cartella = std::filesystem::temp_directory_path() / "m688_prova_mt32";
    std::filesystem::create_directories(cartella);
    std::vector<float> tono(32000);   // un secondo a 32 kHz, basso come i file veri (picco 0,1)
    for (std::size_t i = 0; i < tono.size(); ++i) tono[i] = 0.1f * static_cast<float>(std::sin(2.0 * 3.14159265 * 220.0 * static_cast<double>(i) / 32000.0));
    scrivi_wav(cartella / "menu.wav", tono, 32000);
    RegiaAudio r(M688_DATI);
    r.volume_musica(1.0f);
    r.uscita_musica(UscitaMusica::mt32, cartella.string());
    r.musica("menu");
    const auto a = secondi(r, 2.5);   // oltre la fine del file: il loop continua
    float picco = 0.0f;
    for (std::size_t i = a.size() / 2; i < a.size(); ++i) picco = std::max(picco, std::fabs(a[i]));
    CHECK(picco > 0.5f);              // alzato verso 0,8
    CHECK(r.musica_in_corso());
    r.musica("albo");                 // senza WAV: l'FM, dopo la dissolvenza
    secondi(r, 1.3);
    CHECK(r.musica_in_corso());
    CHECK(energia(secondi(r, 2.0)) > 1e-3);
    std::filesystem::remove_all(cartella);
}

// Il filo dell'audio chiede blocchi della grandezza del dispositivo, non di un fotogramma. Bip e campioni del classico escono
// uguali al byte, con la resa piena e con quelle d'epoca. La musica passa dal ricampionatore: il primo blocco di un brano resta
// corto di un campione, dov'e' che finisca (un solo zero, poi si riallinea), e la fase si somma in virgola mobile: dopo cambiano
// gli arrotondamenti, nient'altro (il chip FM ne e' fuori). Gli effetti
// sono gli stessi campioni, con qualche zero isolato dove un blocco finisce prima del ricampionatore (2 a blocchi di 1600, 4 di 1024)
TEST_CASE("regia audio: gli stessi campioni a blocchi di qualunque grandezza") {
    constexpr float SCARTO_ARROTONDAMENTI = 1e-5f;
    const auto suono = [](Resa resa, bool musica, const std::vector<std::size_t>& blocchi) {
        RegiaAudio r(M688_DATI);
        r.resa(resa);
        if (musica) r.musica("menu");
        std::vector<float> bip(4000);
        for (std::size_t i = 0; i < bip.size(); ++i) bip[i] = (i / 20) % 2 ? 0.4f : -0.4f;
        const std::vector<float> campione(bip.begin(), bip.begin() + 1500);
        constexpr std::size_t TOTALE = 3 * 48000, EVENTI[] = {0, 10000, 30000, 70000, 100000};
        std::vector<float> out;
        std::size_t prossimo_blocco = 0, evento = 0;
        while (out.size() < TOTALE) {
            while (evento < std::size(EVENTI) && out.size() >= EVENTI[evento]) {
                r.mixer().aggiungi(Canale::interfaccia, evento % 2 ? campione : bip, evento % 2 ? 8000 : 22050);
                ++evento;
            }
            std::size_t n = std::min(blocchi[prossimo_blocco++ % blocchi.size()], TOTALE - out.size());
            if (evento < std::size(EVENTI)) n = std::min(n, EVENTI[evento] - out.size());
            const auto x = r.produci(n);
            out.insert(out.end(), x.begin(), x.end());
        }
        return out;
    };
    const auto uguali = [](const std::vector<float>& a, const std::vector<float>& b) {
        return a.size() == b.size() && std::memcmp(a.data(), b.data(), a.size() * sizeof(float)) == 0;
    };
    for (const Resa resa : {Resa::piena, Resa::originale, Resa::sound_blaster})
        for (const bool musica : {false, true}) {
            CAPTURE(nome_resa(resa));
            CAPTURE(musica);
            const auto fissi = suono(resa, musica, {1600});
            CHECK(energia(fissi) > 1e-6);
            for (const auto& blocchi : {std::vector<std::size_t>{1024}, std::vector<std::size_t>{1024, 7, 333, 2048, 1}}) {
                const auto x = suono(resa, musica, blocchi);
                if (!musica) {
                    CHECK(uguali(fissi, x));
                } else {
                    REQUIRE(x.size() == fissi.size());
                    float scarto = 0.0f;
                    for (std::size_t i = 2048; i < x.size(); ++i) scarto = std::max(scarto, std::fabs(x[i] - fissi[i]));
                    MESSAGE("scarto massimo ", static_cast<double>(scarto));
                    CHECK(scarto < SCARTO_ARROTONDAMENTI);
                }
            }
        }
}

TEST_CASE("effetti di bordo: il fischietto all'inizio sul 688 e non sull'Alfa, l'immersione rapida intera, il bip") {
    Missione mis(M688_MISSIONI "/SCENE1A.lua", 1, false);
    mis.avanza(1.0 / 9.0);
    RegiaAudio r(M688_DATI);
    EffettiDiBordo usn(r, false);
    usn.aggiorna(mis);
    CHECK(r.effetti_in_corso() == 1);         // il fischietto del nostromo (al piu' 4 s)
    secondi(r, 4.2);
    CHECK(r.effetti_in_corso() == 0);
    usn.messaggio_equipaggio(430);            // l'immersione rapida: la tromba H8 intera (4,1 s)
    secondi(r, 3.9);
    CHECK(r.effetti_in_corso() == 1);
    secondi(r, 0.5);
    CHECK(r.effetti_in_corso() == 0);
    usn.messaggio_equipaggio(210);            // un messaggio qualunque: il bip leggero
    CHECK(r.effetti_in_corso() == 1);
    RegiaAudio r2(M688_DATI);
    EffettiDiBordo vmf(r2, true);
    vmf.aggiorna(mis);
    CHECK(r2.effetti_in_corso() == 0);        // la VMF non ha il fischietto
    vmf.messaggio_equipaggio(430);            // il ревун, tagliato a 4 s
    CHECK(r2.effetti_in_corso() == 1);
}
