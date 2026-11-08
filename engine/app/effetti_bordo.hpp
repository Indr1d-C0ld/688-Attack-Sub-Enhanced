#pragma once
// Gli effetti di bordo in mare (roadmap 38; nota condivisa trailer1-audio-schermate-elenco.md §4, approvata il
// 31/10/2026): gli allarmi e i rumori del battello del giocatore nell'Enhanced, dai fatti della missione (eventi del
// registro, messaggi, alberi, quota) e dai messaggi dell'equipaggio che compone l'interfaccia. Sul 688 gli allarmi USN,
// sull'Alfa quelli della VMF (il ревун, le campane). Suonano dalla regia (app/regia_audio), sovrapposti; gli allarmi
// lunghi si tagliano a 4 s, l'immersione rapida col suo doppio «aooga» intero.
//
// Nel classico non suona niente di questo: restano i suoni del 1989 (app/interfaccia, audio_classico).

#include <cstddef>

namespace m688 {

class Missione;
class RegiaAudio;

class EffettiDiBordo {
public:
    EffettiDiBordo(RegiaAudio& regia, bool sovietico) : r_(regia), urss_(sovietico) {}

    // a ogni fotogramma in mare: i fatti nuovi dall'ultima volta
    void aggiorna(const Missione& m);
    // un messaggio dell'equipaggio nella barra (InterfacciaMissione::messaggio): l'immersione rapida, la ricarica, le
    // quote, e il bip leggero di ogni messaggio
    void messaggio_equipaggio(int codice);

    bool posti_di_combattimento_fatti() const { return gq_; }   // almeno una volta in questa missione

private:
    void allarme(const char* usn, const char* vmf, double massimo_s = 4.0);
    void bip_esm();

    RegiaAudio& r_;
    bool urss_ = false;
    bool iniziato_ = false, gq_ = false;
    std::size_t eventi_visti_ = 0, messaggi_visti_ = 0;
    int alberi_ = -1;                 // il flag54 dell'ultimo fotogramma (-1: ancora nessuno)
    bool in_superficie_ = false;      // per l'immersione dalla superficie
};

// Quello attivo in mare, per l'interfaccia (nullptr fuori dal mare, nel classico e senza audio)
EffettiDiBordo* effetti_di_bordo();
void imposta_effetti_di_bordo(EffettiDiBordo* e);

}  // namespace m688
