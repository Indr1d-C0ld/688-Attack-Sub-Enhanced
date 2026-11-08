#pragma once
// Esito e cerimonie dopo il debriefing (Fase 4a, docs/12_schermate_di_missione.md M5; proposta in docs/ricerca/15
// §7.8-7.9 e 13 §5.4), sulla carriera della M3 (nucleo/carriera.hpp, missioni/carriera_missione.hpp) e sulla grafica
// della M4 (grafica/cerimonia, ritratto, decorazioni, cornici). Solo con la regola di simulazione.
//
// Le pagine, una dopo l'altra (Invio): il rapporto d'esito («PATROL REPORT», per la VMF «DONESENIE O POKHODE») con la
// frase d'esito, le voci del punteggio, il moltiplicatore scomposto, il totale timbrato, il giudizio, il motivo del
// fallimento, l'equipaggio e le decorazioni automatiche; poi una cerimonia per la decorazione personale e una per
// quella di reparto (ritratto, o l'equipaggio schierato per il reparto, medaglia nell'astuccio, nastrino,
// motivazione); la promozione (le due insegne); il provvedimento (lettera timbrata). Dopo il rapporto, per gli esiti
// che fanno notizia (un neutrale o un amico colpito, il battello perduto), la pagina di giornale d'epoca o il
// comunicato TASS (da F-19 e F-14); niente per le operazioni speciali. Se il comandante e' caduto (stato «morto»),
// in fondo la pagina «In memoriam». Le pagine senza contenuto non compaiono.

#include <memory>
#include <string>
#include <vector>

#include "grafica/decorazioni.hpp"
#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/ritratto.hpp"
#include "grafica/tela.hpp"
#include "missioni/registro_missione.hpp"
#include "nucleo/achievement.hpp"
#include "nucleo/carriera.hpp"
#include "partita.hpp"
#include "piattaforma/finestra.hpp"

namespace m688 {

// La missione chiusa per la carriera: riassunto, contesto, esito della carriera e il fascicolo prima e dopo
// Un achievement nuovo o salito di livello, per la pagina «NUOVI RICONOSCIMENTI» (nomi dal catalogo)
struct RiconoscimentoNuovo {
    std::string id, icona, categoria, nome, descrizione;
    int livello = 0, prima = 0;          // 1 bronzo, 2 argento, 3 oro; prima 0 la prima volta
    bool solo_globale = false;           // la missione non contava per il comandante: vale solo per il giocatore
};

// Il realismo della partita (docs/26 §1): il moltiplicatore e gli aiuti accesi col loro peso, per il rapporto
struct AiutoPartita {
    std::string nome;
    double peso = 0.0;
};
struct RealismoPartita {
    double valore = 1.0;   // 1 senza aiuti, al minimo 0,5
    std::vector<AiutoPartita> aiuti;
};

struct MissioneChiusa {
    EsitoDettagliato dettaglio;
    EsitoPerCarriera riassunto;
    ContestoMissione contesto;
    EsitoCarriera esito;
    Fascicolo prima, dopo;
    std::vector<VoceBarretta> barretta;   // dopo la missione
    std::string titolo;                   // della missione
    EsitoAchievement achievement;         // i nuovi sblocchi (del comandante e globali)
    std::vector<RiconoscimentoNuovo> riconoscimenti;   // gli stessi coi nomi, uno per achievement, per la pagina
    int posto_onore = 0;
    std::vector<AiutoPartita> aiuti;      // gli aiuti accesi, per la catena del punteggio nel rapporto                  // la posizione della missione nell'albo d'onore (1-20), 0 se non entra
};
// Chiude la partita sul fascicolo `f` (aggiornato in uscita). `battello`, `teatro`: i nomi per le motivazioni.
// `origine` e `seme` per Carriera::conta: una rapida conta col suo seme, «0» la lascia fuori carriera. Con `dati`
// registra anche gli achievement (data/motore/achievement.toml) e l'albo d'onore; `codice` e' quello della rapida.
// `realismo`: il moltiplicatore e gli aiuti accesi (docs/26 §1), 1 senza aiuti e nel classico; `righe_acint`: le righe
// nuove del rapporto ACINT della stazione Sonar (l'achievement «Per l'intelligence»)
MissioneChiusa chiudi_partita(const Partita& p, const RegoleCarriera& r, Fascicolo& f, const std::string& battello,
                              const std::string& teatro, const std::string& origine = "originale", std::uint64_t seme = 0,
                              const std::string& dati = "", const std::string& codice = "", const RealismoPartita& realismo = {},
                              int righe_acint = 0);
// Il fascicolo del comandante della marina: quello in carica nell'albo (nucleo/albo.hpp), altrimenti il primo in
// servizio, altrimenti uno nuovo («carriera breve», dal comando)
Fascicolo fascicolo_comandante(const RegoleCarriera& r, Marina m);
// Il ritratto ufficiale dal fascicolo (grado, dolphins e comando, barretta, eta'): cerimonie, albo e fascicolo
ParametriRitratto ritratto_comandante(const RegoleCarriera& r, const Fascicolo& f);
// Il grado per esteso in maiuscolo («CAPITANO DI FREGATA»), o l'id se non c'e' nelle regole
std::string nome_grado_comandante(const RegoleCarriera& r, const Fascicolo& f);
// «1989-10» -> «OTTOBRE 1989»
std::string data_estesa(const std::string& d);

class SchermateEsito {
public:
    enum class Uscita { nessuna, debriefing, fine };
    SchermateEsito(const MissioneChiusa& c, const RegoleCarriera& r, const TavolaDecorazioni& tav);
    Uscita comandi(const Comandi& c);
    void disegna(Tela& t, const Font& f) const;   // la pagina si compone una volta e poi si copia
    void palette(Palette& p) const;

    int pagine() const { return static_cast<int>(pagine_.size()); }
    int pagina() const { return pagina_; }
    void vai(int i);

private:
    enum class Tipo { esito, giornale, riconoscimenti, cerimonia, promozione, provvedimento, memoria };
    struct Pagina {
        Tipo tipo;
        const Conferimento* conferimento = nullptr;
        int da = 0;                       // riconoscimenti: il primo della pagina
    };
    static constexpr int RICONOSCIMENTI_PER_PAGINA = 6;
    void componi(Tela& t, const Font& f) const;
    void disegna_esito(Tela& t, const Font& f) const;
    void disegna_cerimonia(Tela& t, const Font& f, const Conferimento& k) const;
    void disegna_promozione(Tela& t, const Font& f) const;
    void disegna_provvedimento(Tela& t, const Font& f) const;
    void disegna_giornale_esito(Tela& t, const Font& f) const;
    void disegna_memoria(Tela& t, const Font& f) const;
    void disegna_riconoscimenti(Tela& t, const Font& f, int da) const;
    bool fa_notizia() const;
    ParametriRitratto ritratto(const Fascicolo& f) const;
    std::string nome_grado(const Fascicolo& f) const;

    const MissioneChiusa& c_;
    const RegoleCarriera& r_;
    const TavolaDecorazioni& tav_;
    Marina marina_;
    std::vector<Pagina> pagine_;
    int pagina_ = 0;
    mutable std::unique_ptr<Tela> cache_;
    mutable int pagina_cache_ = -1;
};

}  // namespace m688
