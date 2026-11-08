#pragma once
// Suoni in partita del classico, come in GAME.EXE del 1989 (docs/originale/suoni.md): i sei campioni .T (PCM a 4 bit) e
// i bip dell'altoparlante, con le regole dell'originale su quando suonano. Il modulo osserva soltanto: riceve gli eventi
// che la missione e l'interfaccia gia' conoscono (messaggi in barra, ping, lanci, ANALIZZA, la vista del periscopio) e
// dice quali suoni far uscire, in ordine. Nell'originale ogni suono blocca il gioco (il tempo si ferma) e quindi due
// suoni escono uno dopo l'altro: qui il gioco non si ferma, e la stessa sequenza la da' una coda sola nel mixer.
//
// Il modulo e' puro e deterministico (il seme serve solo per la frequenza a caso della balena di ANALIZZA).

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "nucleo/casuale.hpp"

namespace m688 {

// DS:03E2: 0 tutto, 1 senza campioni (Ctrl-N: i bip restano), 2 muto (Ctrl-Q)
enum class ModoSuoniClassici : std::uint8_t { tutti, senza_campioni, muti };

// Un .T dell'originale: nibble 0..15 in ordine (prima l'alto), e la provenienza
struct CampioneClassico {
    std::string nome;                    // "EXP1", "SONAR1", "TORPRAMP", "PROP2", "PROPV2", "WHALE2"
    std::vector<std::uint8_t> nibble;
    std::string origine;                 // "DISK2/EXP1.T"
};
struct DatiSuoniClassici {
    std::map<std::string, CampioneClassico> campioni;
    const CampioneClassico* campione(const std::string& nome) const;   // nullptr se manca
};
// data/motore/suoni_classici.toml e i .t indicati, relativi a `radice_dati` (data/); vuoto se il toml manca,
// std::runtime_error se un file indicato non si legge
DatiSuoniClassici carica_suoni_classici(const std::string& percorso_toml, const std::string& radice_dati);

struct SuonoClassico {
    enum class Tipo : std::uint8_t { campione, bip };
    Tipo tipo = Tipo::campione;
    std::string campione;     // per Tipo::campione
    int frequenza_hz = 0;     // campione: frequenza di riproduzione; bip: 1193182 / divisore del PIT
    int ripetizioni = 1;
    double durata_s = 0.0;    // bip: alla velocita' di un XT a 4,77 MHz (STIMA: il ciclo d'attesa dipende dalla CPU)
    float volume = 1.0f;      // campioni 1; bip VOLUME_BIP
    std::string causa;        // "colpo", "colpo_periscopio", "danno", "ping_proprio", "ping_altrui", "ping_ripetuto",
                              // "lancio", "analizza", "testo", "tasto"
};

// Il contatto di ANALIZZA: tipo (+4F, 0..2Dh) e categoria, gli indicatori 08h e 10h di 1000:990C, che non sono i bit
// di +06: nave = (+06 & 6) != 0 (gli aerei, +06 & 3 = 3, ANALIZZA li rifiuta prima), arma = (+06 & 0x20) != 0
struct ContattoAnalizzato {
    int tipo = 0;
    bool nave = false, arma = false;
    static ContattoAnalizzato da_flag(int tipo, int flag06) { return {tipo, (flag06 & 6) != 0, (flag06 & 0x20) != 0}; }
};

class SuoniClassici {
public:
    static constexpr double SECONDI_PER_GIRO = 7.3e-6;   // 4 NOP e LOOP del ciclo di BD3F su un XT (STIMA, ~35 cicli)
    static constexpr float VOLUME_BIP = 0.35f;           // STIMA: l'onda quadra del PIT contro il PCM a 4 bit
    static constexpr int PIT_HZ = 1193182;

    explicit SuoniClassici(std::uint64_t seme = 0) : caso_(seme ^ 0x7e9a1d55u) {}
    void modo(ModoSuoniClassici m) { modo_ = m; }
    ModoSuoniClassici modo() const { return modo_; }

    // Gli eventi, nell'ordine in cui accadono; ognuno accoda i suoi suoni, in ordine.
    // Un messaggio del gestore (1000:8AB0) che il classico mostra in barra: il suo campione (7, 50, 19, 20, 26, 30, 8)
    // e il bip del testo, nell'ordine dell'originale
    void messaggio(int codice);
    bool testo_soppresso() const { return soppresso_; }   // l'ultimo messaggio() era un 8 ripetuto: niente testo
    void testo_in_barra();                                 // un altro testo in barra (stazioni, rifiuti): solo il bip
    void ping_proprio();                                   // C9h accettato: il bip del suo testo e sonar1 a 5500 Hz
    void lancio_proprio();                                 // lancio accettato da un tubo (790C): TORPRAMP a 6500 Hz
    void analizza(const ContattoAnalizzato& c);            // ANALIZZA accettata: il bip del suo testo e la firma
    void tasto_modo();                                     // Ctrl-N o Ctrl-Q (dopo modo()): il bip, in ogni modo
    // La vista del periscopio occupa la finestra (DS:03AA e DS:03A0 = 20h): i colpi suonano con l'animazione
    void vista_periscopio(bool a_schermo) { vista_ = a_schermo; }
    bool vista_periscopio() const { return vista_; }
    void colpo(int unita, int stato) { stati_[unita] = stato & 0xFF; }   // +5E dopo un colpo (1, 0xC9, 0xD3)
    // Un ridisegno della vista con le unita' disegnate nel campo: avanza le animazioni dei colpi (97BD) e suona EXP1
    // quando una arriva al quarto fotogramma (322A, 2AA8). Senza la vista a schermo non fa niente
    void fotogramma_periscopio(const std::vector<int>& in_vista);
    int stato_colpo(int unita) const;                      // +5E dell'animazione (0: nessun colpo)

    std::vector<SuonoClassico> prendi();                   // i suoni nati dall'ultima prendi(), in ordine; svuota

    // PCM -1..1 pronto per Mixer::aggiungi alla frequenza del suono, ripetizioni comprese
    static std::vector<float> pcm(const CampioneClassico& c, int ripetizioni, float volume = 1.0f);
    // Il bip come onda quadra a `campionamento_hz`
    static std::vector<float> pcm_bip(const SuonoClassico& s, int campionamento_hz);

private:
    void campione(const char* nome, int hz, int ripetizioni, const char* causa);
    void bip(int divisore, int giri, const char* causa);

    ModoSuoniClassici modo_ = ModoSuoniClassici::tutti;
    Casuale caso_;
    std::vector<SuonoClassico> uscita_;
    std::map<int, int> stati_;
    int ultimo_ = 0;          // DS:03C8: l'ultimo messaggio scritto dal gestore
    bool soppresso_ = false;
    bool vista_ = false;
};

}  // namespace m688
