#pragma once
// Finestra SDL3: presenta la tela 640x480 indicizzata attraverso la palette, ingrandita a multipli interi
// senza filtri (pixel nitidi, bande nere ai lati se la finestra non e' un multiplo esatto).

#include <cstdint>
#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "nucleo/comando.hpp"
#include "nucleo/mappa_tasti.hpp"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

namespace m688 {

class Tela;
class Palette;

struct Comandi {
    bool esci = false;                // finestra chiusa
    bool esc = false;                 // Esc: annulla una scelta in corso, altrimenti esce
    bool invio = false;               // Invio: conferma una scelta in corso
    int tab = 0;                      // Tab +1, Maiuscolo+Tab -1: scorre le zone di una scelta
    std::vector<char> lettere;        // lettere premute (A-Z), per scegliere un segno con TARGET
    int su_giu = 0;                   // -1 giu', +1 su (frecce tenute premute: cursore della carta)
    int ruota = 0;                    // -1 sinistra, +1 destra (tasto tenuto premuto)
    // le pressioni delle frecce dall'ultima lettura, contate dagli eventi (senza la ripetizione): -1 giu' e sinistra, +1 su
    // e destra. Al contrario di su_giu e ruota, non si perdono se il tasto si preme e si rilascia dentro un fotogramma
    int passi_su_giu = 0, passi_ruota = 0;
    bool veloce = false;              // Maiuscolo tenuto: rotazione rapida
    bool cambia_ingrandimento = false;   // frecce su e giu' al periscopio
    bool ingrandisci = false, riduci = false;   // I e O: periscopio, o zoom della carta
    int compressione = 0;             // +1 / -1: raddoppia o dimezza la compressione del tempo
    bool verita = false;              // mostra o nasconde la diagnosi dei contatti
    bool istantanea = false;          // salva la tela corrente in BMP
    std::vector<Azione> azioni;       // ordini rapidi del posto di comando (docs/11_comandi_e_stazioni.md §5)
    // stazioni (fasi C3-C4): tasti dell'originale R, A, M (tubi 1, 5, 7), L lancio, G bersaglio; S scelta del siluro
    // filoguidato, C ricerca, D DETONATE; W e K punti di rotta, J proiezione, [ e ] i suoi minuti, B sfondo della carta
    int stazione = 0;                 // F3 Stato, F4 Manovra, F6 Navigazione, F7 Armi, F8 Sonar, F9 Centrale (0: invariata)
    std::vector<int> tubi;            // R, A, M
    std::vector<int> cifre;           // 1-9: i pulsanti della stazione (tubi alle Armi, comandi al Sonar)
    std::vector<std::string> nominati;   // comandi delle stazioni senza un campo proprio (pulti dell'Alfa: boksit_rotta...)
    bool punto = false, cancella_punto = false, proiezione = false, sfondo = false;
    bool tma = false;                 // T alla carta: strumento MATE (Enhanced)
    int filo_rotta = 0;               // Z -1, X +1: rotta del siluro filoguidato di 10 gradi (Enhanced)
    bool taglia_filo = false;         // Y: taglia il filo del siluro scelto (Enhanced)
    bool variante_arma = false;       // Ctrl+V: arma successiva della rastrelliera nel tubo scelto (Enhanced, carico)
    bool enciclopedia = false;        // F1: il Museo dal menu, la scheda dell'enciclopedia in missione (Enhanced)
    bool pausa = false;               // P: pausa (ferma il tempo finche' non si ripreme)
    bool stadimetro = false;          // H: misura della distanza al periscopio
    std::vector<std::pair<int, int>> clic;   // clic del tasto sinistro, in pixel della tela 640x480
    std::vector<std::pair<int, int>> clic_destro;   // clic del tasto destro (editor: menu sul punto)
    std::pair<int, int> mouse{-1, -1};       // posizione del puntatore sulla tela (-1: fuori o sconosciuta)
    bool premuto = false;                    // tasto sinistro tenuto giu': col movimento di `mouse`, il trascinamento
    std::vector<std::pair<int, int>> rilascio;   // dove il tasto sinistro e' stato rilasciato (fine del trascinamento)
    int rotella = 0;                  // gli scatti della rotella del mouse dall'ultima lettura: + avanti, - indietro
    int minuti = 0;                   // +1 / -1 i minuti della proiezione
    bool lancia = false, bersaglio = false, siluro = false, cerca = false, detona = false;
    // carta dell'Enhanced (N2): E evento con l'ora, F aggancia l'estremo del righello, Q regolo di Ekelund,
    // Maiuscolo+K pulisce il lucido
    bool evento = false, aggancia = false, ekelund = false, pulisci = false;
    bool silenzio = false;            // Ctrl+S: silenzio a bordo (Enhanced: ricariche lente e senza transitori)
    // testo scritto (UTF-8) e Backspace, solo con la digitazione accesa (note sulla carta)
    std::string testo;
    bool cancella = false;
};

// I passi di una freccia nelle schermate «uno per pressione»: le pressioni contate, che non si perdono dentro un fotogramma.
// Chi riempie i Comandi a mano (le prove) da' solo lo stato del tasto: allora vale il suo fronte. `prec`: lo stato di prima
inline int passi_freccia(int tenuto, int passi, int& prec) {
    const int r = passi != 0 ? passi : tenuto != prec ? tenuto : 0;
    prec = tenuto;
    return r;
}

// Il verso di una freccia nelle schermate che ripetono finche' e' tenuta: anche se e' gia' stata rilasciata in questa lettura
inline int verso_freccia(int tenuto, int passi) { return tenuto != 0 ? tenuto : (passi > 0) - (passi < 0); }

// Il comando di un tasto (nome di nucleo/mappa_tasti) nei campi di Comandi
void applica(const std::string& comando, Comandi& c);

class Finestra {
public:
    Finestra(const std::string& titolo, int scala_iniziale = 2);   // lancia std::runtime_error
    ~Finestra();
    Finestra(const Finestra&) = delete;
    Finestra& operator=(const Finestra&) = delete;

    Comandi eventi();
    void imposta_tasti(MappaTasti m);   // tasti rimappati (predefiniti: MappaTasti::predefinita())
    // Digitazione del testo (le note sulla carta): con la digitazione accesa i caratteri arrivano in Comandi::testo
    void modo_testo(bool si);
    void presenta(const Tela& t, const Palette& p);
    // L'uscita a schermo (E9 §2.1): nitida a multipli interi (predefinita, come il classico), o «sharp bilinear»
    // (multiplo intero e poi lineare fino alla finestra intera); con `crt` righe di scansione e maschera leggere
    enum class Uscita { nitida, sharp_bilinear };
    void imposta_uscita(Uscita u, bool crt);
    // F10 (comando uscita_schermo): il giro nitida -> sharp bilinear -> sharp bilinear col CRT -> nitida, in ogni
    // schermata, con un avviso breve in alto; `salva` registra la scelta (le preferenze, in app/main.cpp)
    void al_cambio_uscita(std::function<void(Uscita, bool)> salva) { salva_uscita_ = std::move(salva); }
    static double secondi();   // orologio monotono ad alta risoluzione
    double ultimo_present_s() const { return ultimo_present_s_; }   // la durata dell'ultimo SDL_RenderPresent (l'attesa del vsync e' li')
    std::string descrizione() const;   // driver, renderer, vsync e grandezza: per il registro dei tempi

private:
    SDL_Window* finestra_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    double ultimo_present_s_ = 0.0;
    SDL_Texture* trama_ = nullptr;
    SDL_Texture* intermedia_ = nullptr;   // sharp bilinear: la tela al multiplo intero
    SDL_Texture* crt_ = nullptr;          // le righe di scansione e la maschera, sopra
    int scala_intermedia_ = 0, crt_w_ = 0, crt_h_ = 0;
    Uscita uscita_ = Uscita::nitida;
    bool con_crt_ = false;
    std::function<void(Uscita, bool)> salva_uscita_;
    std::string avviso_;          // l'avviso dell'uscita, fino a avviso_fino_ (secondi dell'orologio)
    double avviso_fino_ = 0.0;
    std::vector<std::uint32_t> argb_;
    MappaTasti tasti_ = MappaTasti::predefinita();
    bool testo_ = false;
};

}  // namespace m688
