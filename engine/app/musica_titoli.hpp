#pragma once
// La musica dei titoli dell'originale (CALLS.EXE, 1989) nel menu, come all'avvio del gioco: le tre versioni dell'epoca,
// AdLib (OPL2, resa con ymfm), Tandy 1000 e Covox, da data/motore/musica (tools/pipeline/musica.py). Suona una volta,
// dall'inizio, finche' si resta nel menu; si sceglie con M nella scelta della missione. Senza dispositivo audio, o con
// --audio spento, tace.

#include <memory>
#include <string>
#include <vector>

#include "piattaforma/audio.hpp"

namespace m688 {

enum class VersioneMusica { adlib, tandy, covox, spenta };
const char* nome_versione_musica(VersioneMusica v);   // «ADLIB», «TANDY», «COVOX», «SPENTA»
// La scelta resta fra un avvio e l'altro: una parola in <dati utente>/melchior688/musica.txt (accanto ai fascicoli).
// Senza file, o illeggibile: AdLib, la versione di riferimento del 1989
VersioneMusica leggi_preferenza_musica();
void salva_preferenza_musica(VersioneMusica v);

class MusicaTitoli {
public:
    explicit MusicaTitoli(std::string cartella_dati) : dati_(std::move(cartella_dati)) {}
    // ricomincia dall'inizio con quella versione (spenta: silenzio); false se il file o il dispositivo mancano
    bool suona(VersioneMusica v);
    void alimenta();   // a ogni fotogramma: tiene in coda un poco di musica
    VersioneMusica versione() const { return versione_; }

private:
    std::string dati_;
    VersioneMusica versione_ = VersioneMusica::spenta;
    std::unique_ptr<UscitaAudio> uscita_;
    std::vector<float> campioni_;
    std::size_t letti_ = 0;
};

}  // namespace m688
