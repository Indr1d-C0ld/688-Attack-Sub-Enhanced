#pragma once
// La regia dell'audio (roadmap 38; nota condivisa trailer1-audio-schermate-elenco.md, approvata il 31/10/2026): una sola
// uscita per tutto il gioco, il mixer, la musica di ogni schermata e gli effetti dell'interfaccia e di bordo.
// - La musica: i brani FM di data/motore/musica/fm (nucleo/musica_fm, sul canale `musica`), o un PCM gia' reso (i titoli
//   del 1989 nel classico). Al cambio di brano quello di prima sfuma in 1 s e il nuovo parte quando tace; lo stesso
//   brano non riparte.
// - Gli effetti: le voci di data/motore/effetti.toml (WAV in data/suoni/effetti), caricate alla prima richiesta, si
//   sommano fra loro (il Mixer accoda, non sovrappone) e vanno al mixer in un flusso continuo sul canale degli effetti.
//   Gli allarmi lunghi si tagliano a `massimo_s` con una breve dissolvenza.
// - In mare la missione usa questo mixer: cuffia, bordo e interfaccia li riempiono i loro produttori; produci() li somma
//   alla musica e agli effetti.
// Il dispositivo (piattaforma/audio) resta a chi chiama: cosi' il modulo non dipende da SDL e gira nelle prove. Con la finestra
// i campioni li chiede il filo dell'audio (produci_dal_filo) e il thread dei fotogrammi chiama aggiorna_brano, che legge i
// file; tutto il resto si puo' chiamare mentre il filo gira.

#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

#include "nucleo/mixer_audio.hpp"
#include "nucleo/musica_fm.hpp"

namespace m688 {

// la musica: l'FM dal vivo, o i WAV resi con l'MT-32 dalle ROM dell'utente (tools/mt32, nella sua cache: mai nel
// progetto), dove ci sono ([audio] musica_uscita = "fm" | "mt32" nelle preferenze)
enum class UscitaMusica { fm, mt32 };

class RegiaAudio {
public:
    // `dati`: data/motore; `campionamento_hz`: quello dell'uscita
    explicit RegiaAudio(std::string dati, int campionamento_hz = 48000);
    ~RegiaAudio();

    Mixer& mixer() { return mixer_; }
    void resa(Resa r);                      // della missione; fuori dal mare: piena

    // --- la musica ---
    // il brano (il nome in musica/fm, senza .mid; vuoto: silenzio); `in_loop` falso: una volta, poi silenzio
    void musica(const std::string& brano, bool in_loop = true);
    // un brano gia' reso in PCM (i titoli del 1989), una volta; `nome` per riconoscerlo (lo stesso non riparte)
    void musica_pcm(const std::string& nome, std::vector<float> campioni, int campionamento_hz);
    const std::string& brano() const { return voluto_; }   // quello chiesto (anche mentre sfuma il precedente)
    bool musica_in_corso() const;
    void volume_musica(float v);            // 0..1 ([audio] musica nelle preferenze)
    // fm o mt32; `cartella_mt32` vuota: quella del pacchetto accanto ai dati (data/mt32), se c'e', se no
    // ~/.cache/melchior688/mt32 (o $XDG_CACHE_HOME). Vale dal brano seguente
    void uscita_musica(UscitaMusica u, std::string cartella_mt32 = {});
    static std::string cartella_mt32_predefinita();
    // la cartella dei brani MT-32 per i dati `dati` (data/motore): <dati>/../mt32 se c'e', se no la predefinita
    static std::string cartella_mt32_per(const std::string& dati);
    float volume_musica() const { return volume_musica_; }

    // --- gli effetti ---
    // l'effetto `id` di effetti.toml; `massimo_s` 0: intero. False se l'id o il file mancano
    bool effetto(const std::string& id, float volume = 1.0f, double massimo_s = 4.0);
    void suono(std::vector<float> campioni, int campionamento_hz, float volume = 1.0f);   // un suono sintetizzato
    bool ha_effetto(const std::string& id) const { return voci_effetti_.count(id) > 0; }
    std::size_t effetti_in_corso() const {
        std::lock_guard blocco(mutex_);
        return attive_.size();
    }

    // Ctrl+Q: tutto muto (anche la missione), o di nuovo acceso
    void muto(bool si);
    bool muto() const { return muto_; }

    // n campioni d'uscita, alla frequenza del mixer: musica, effetti e canali della missione (le prove)
    std::vector<float> produci(std::size_t n);
    // Le due meta' di produci, per chi tiene il dispositivo su un altro filo: il brano che tace lascia il posto a quello voluto
    // (legge i file: sul thread dei fotogrammi) e i campioni (dal filo dell'audio: niente disco)
    void aggiorna_brano();
    std::vector<float> produci_dal_filo(std::size_t n);

    static constexpr int EFFETTI_HZ = 22050;

private:
    struct Voce {
        std::vector<float> campioni;
        std::size_t letti = 0;
        float volume = 1.0f;
    };
    struct BranoPcm {
        std::string nome;
        std::vector<float> campioni;
        int hz = 0;
        std::size_t letti = 0;
        bool loop = false;
        double sfuma_resta = 0.0, sfuma_tot = 0.0;   // in campioni (sfuma_tot 0: non sfuma)
    };
    void parti(const std::string& brano, bool in_loop);
    void sfuma(double s);
    const std::vector<float>* campioni_effetto(const std::string& id);
    void aggiorna_volumi();

    mutable std::mutex mutex_;   // la musica in corso, le voci e i resti: li usa anche il filo. Le tavole dei file e voluto_, solo i fotogrammi
    std::string dati_;
    int campionamento_hz_;       // non cambia: il filo non legge i parametri del mixer
    Mixer mixer_;
    std::unique_ptr<SuonatoreFm> fm_;
    std::map<std::string, BranoMidi> brani_;
    std::string voluto_, in_corso_;
    bool loop_voluto_ = true;
    std::optional<BranoPcm> pcm_;
    UscitaMusica uscita_musica_ = UscitaMusica::fm;
    std::string cartella_mt32_;
    std::map<std::string, BranoPcm> wav_mt32_;                    // letti, gia' alzati
    float volume_musica_ = 0.7f;
    bool muto_ = false;
    std::map<std::string, std::string> voci_effetti_;            // id -> file (relativo a data/)
    std::map<std::string, std::vector<float>> effetti_;          // caricati, a EFFETTI_HZ
    std::vector<Voce> attive_;
    double resto_fm_ = 0.0, resto_pcm_ = 0.0, resto_eff_ = 0.0;   // le frazioni di campione fra un blocco e l'altro
};

}  // namespace m688
