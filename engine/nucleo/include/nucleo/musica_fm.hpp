#pragma once
// Musica FM dal vivo (roadmap 38, pezzo A; docs/ricerca/18_audio_reale.md): un file MIDI suonato da un OPL3 (YMF262, il
// chip FM della Sound Blaster Pro 2 e della SB16) emulato con ymfm (terzi/ymfm, Aaron Giles, BSD-3). Dal renderer dei
// trailer (tools/trailer/opl_midi.cpp), ma dal vivo: timbri FM a due operatori per programma General MIDI e per le note
// del ritmo (canale 10) da data/motore/timbri_fm.toml (tools/pipeline/timbri_fm.py); 18 voci, la piu' vecchia cede.
//
// Il brano gira in loop fra i marcatori «loop_inizio» e «loop_fine» del file (meta testo 0x06), o tutto se non ci sono;
// ferma() con una dissolvenza. L'uscita e' mono alla frequenza del chip (49.716 Hz): la ricampiona il Mixer, sul canale
// `musica`, che non passa dalla resa a 8 bit (sulle schede d'epoca l'FM usciva analogico, non dal DAC dei campioni).
//
// Il modulo e' puro e deterministico: a parita' di brano, banco e chiamate esce sempre lo stesso segnale.
#include <array>
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace m688 {

struct OperatoreFm {
    int mult = 1, tl = 0, ar = 15, dr = 0, sl = 0, rr = 7, onda = 0, ksl = 0;   // i registri del chip
    bool vib = false, am = false, tenuta = true, ksr = false;
};
struct TimbroFm {
    std::string nome;
    OperatoreFm mod, por;      // modulatore e portante
    int retro = 0;             // feedback 0-7
    int connessione = 0;       // 0 FM, 1 additiva
    int nota_fissa = -1;       // percussioni: la nota suonata (MIDI)
};
struct BancoFm {
    std::map<int, TimbroFm> programmi;   // programma GM (0-based) -> timbro
    std::map<int, TimbroFm> ritmo;       // nota del canale 10 -> timbro
    int programma_riserva = 48;          // per i programmi che il banco non ha (archi)
};
BancoFm carica_banco_fm(const std::string& percorso);   // lancia std::runtime_error

struct EventoMidi {
    double t_s = 0.0;
    std::array<std::uint8_t, 3> dati{};   // stato e fino a due byte
};
struct BranoMidi {
    std::vector<EventoMidi> eventi;       // in ordine di tempo, gli spegnimenti prima a pari tempo
    double durata_s = 0.0;                // l'ultimo evento
    double loop_inizio_s = 0.0, loop_fine_s = -1.0;   // -1: la fine del brano
};
BranoMidi leggi_midi(const std::vector<std::uint8_t>& smf);   // file MIDI tipo 0 o 1; lancia std::runtime_error
BranoMidi leggi_midi_file(const std::string& percorso);

class SuonatoreFm {
public:
    explicit SuonatoreFm(BancoFm banco);
    ~SuonatoreFm();
    SuonatoreFm(const SuonatoreFm&) = delete;
    SuonatoreFm& operator=(const SuonatoreFm&) = delete;

    void suona(const BranoMidi& brano, bool in_loop);   // dall'inizio; ferma quello di prima
    void ferma(double dissolvenza_s = 0.0);              // 0: subito
    bool suona_ancora() const;
    double tempo_s() const;                              // nel brano (col loop torna indietro)
    void volume(float v);                                // 0..1, sopra la dissolvenza
    int campionamento_hz() const;                        // del chip (49.716)
    std::vector<float> genera(std::size_t n);            // n campioni mono, -1..1; silenzio a brano finito

private:
    struct Stato;
    std::unique_ptr<Stato> s_;
};

}  // namespace m688
