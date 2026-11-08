#pragma once
// Editor di missione (roadmap punto 10, fase E4; proposta nelle note condivise «editor-missione-proposta.md»): lo
// scenario (missioni/scenario_missione.hpp) sulla carta del teatro, nella grafica VGA della stazione Navigazione.
//
// - A sinistra la carta (448x400, grafica/carta_vga): unita' coi simboli NTDS per parte e categoria, rotte, zone (col
//   quadrato che controllano le regole), cerchi degli obiettivi. Trascinare il vuoto sposta la carta, I e O ingrandiscono.
// - A destra le schede (1-8 o clic): Missione, Unita', Rotte, Zone, Obiettivi, Regole, Briefing, Prova. Nelle schede ogni
//   campo cambia col clic (sinistro avanti, destro indietro); i testi si scrivono (Invio conferma, Esc annulla).
// - Sulla carta: clic sinistro sceglie, trascinare sposta l'unita' o la zona scelta, clic destro aggiunge (un'unita'
//   della classe scelta, un punto di rotta, una zona, il centro dell'obiettivo) secondo la scheda.
// - In basso gli avvisi della verifica (clic: mostra l'unita'), S salva, E esporta il pacchetto da scambiare
//   (missioni/pacchetto_missione.hpp), G prova in mare, Esc torna al menu.
//
// Salva in cartella_missioni_editor() / <id>.lua: il file giocabile col blocco `editor`, da cui l'editor lo riapre.

#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "grafica/carta_vga.hpp"
#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/tela.hpp"
#include "missioni/scenario_missione.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/sole.hpp"
#include "piattaforma/finestra.hpp"
#include "risorse.hpp"

namespace m688 {

class SchermataEditor {
public:
    enum class Uscita { nessuna, menu, gioca };   // gioca: «Prova in mare» della missione in lavorazione (gia' salvata)
    enum class Scheda { missione, unita, rotte, zone, obiettivi, regole, briefing, prova };

    // `dati`: data/motore. Senza `iniziale` una missione nuova (688 nelle Faer Oer, solo il battello del giocatore).
    // Lancia std::runtime_error se mancano i dati delle missioni rapide (modelli, kit) o il fondale del teatro.
    SchermataEditor(const Risorse& r, std::string dati, std::optional<ScenarioMissione> iniziale = std::nullopt);

    Uscita comandi(const Comandi& c, double dt);
    void disegna(Tela& t, const Font& f) const;
    void palette(Palette& p) const;   // carta (acqua, rilievo, NTDS) sopra la palette del giorno
    bool vuole_testo() const { return campo_.has_value(); }   // un campo di testo aperto: la finestra passa i caratteri

    // dopo Uscita::gioca: il file salvato, da caricare come le rapide
    std::filesystem::path cartella() const { return cartella_; }
    std::string id() const { return s_.id; }
    std::uint64_t seme() const { return 1; }
    DataOra inizio() const { return {s_.anno, s_.mese, s_.giorno, s_.ora_utc}; }
    bool meteo_realistico() const { return s_.meteo_realistico; }

    // per le istantanee senza finestra e le prove
    void scheda(Scheda k);
    void scegli_unita(int indice);
    const ScenarioMissione& scenario() const { return s_; }
    const std::vector<AvvisoScenario>& avvisi() const { return avvisi_; }
    bool salva();                                // scrive il file; falso (con l'errore nel piede) se lo scenario non si scrive
    bool esporta();                              // il pacchetto .m688m in cartella_missioni_esportate() (E)
    void cartella_di_esportazione(std::filesystem::path p) { esportate_ = std::move(p); }   // le prove
    std::filesystem::path pacchetto() const { return esportate_ / (s_.id + ".m688m"); }
    void cartella_di_salvataggio(std::filesystem::path p) { cartella_ = std::move(p); }   // le prove: non la cartella del giocatore
    std::string messaggio() const { return messaggio_; }
    std::string simula(double secondi);          // la missione senza finestra nel classico: il riepilogo (scheda Prova)

private:
    struct Elemento {                            // una riga cliccabile del pannello
        Rettangolo r;
        std::string testo;
        int colore = 7;
        std::function<void(int)> azione;         // +1 clic sinistro, -1 destro; vuota: solo testo
        bool acceso = false;
    };
    struct Campo {                               // il testo in scrittura
        std::string* destinazione = nullptr;
        std::string buffer, etichetta;
    };

    // carta
    GeometriaCarta geometria() const;
    void centra(Vec2 p);
    void inquadra();                             // la zona delle unita' (e di rotte, zone, obiettivi) dentro il teatro
    void limita_vista();                         // scala e centro dentro il teatro: niente nero attorno alla carta
    void carica_teatro();
    int unita_in(Vec2 tela) const;               // indice dell'unita' sotto il punto (-1)
    int zona_in(Vec2 tela) const;
    void clic_carta(Vec2 q, bool destro);
    void disegna_carta(Tela& t, const Font& f) const;
    void disegna_simbolo(Tela& t, Vec2 q, const UnitaScenario& u, bool scelta) const;

    // pannello
    void costruisci() const;                     // gli elementi della scheda in vista
    void riga(int& y, const std::string& testo, int colore, std::function<void(int)> azione = {}, bool acceso = false) const;
    void voce(int& y, const std::string& nome, const std::string& valore, std::function<void(int)> azione) const;
    void scrivi_testo(std::string* destinazione, const std::string& etichetta);
    void pannello_missione() const;
    void pannello_unita() const;
    void pannello_rotte() const;
    void pannello_zone() const;
    void pannello_obiettivi() const;
    void pannello_regole() const;
    void pannello_briefing() const;
    void pannello_prova() const;
    void pannello_radio(int& y) const;

    // modifiche
    void cambiato();                             // rifa' la verifica e gli elementi
    void imposta_carico();                       // carico standard della parte e del tipo di missione
    int nuova_unita(Vec2 pos);
    void elimina_unita(int indice);
    void briefing_automatico();
    void prossimo_id();

    const Risorse& r_;
    std::string dati_;
    DatiScenario d_;
    ScenarioMissione s_;
    std::unique_ptr<Fondale> fondale_;
    std::string teatro_caricato_;
    mutable FondaleVga vga_;
    Vec2 centro_;
    double mpp_ = 500.0;                         // metri per pixel della carta
    Scheda scheda_ = Scheda::missione;
    int unita_ = 0, zona_ = -1, obiettivo_ = -1, regola_ = -1, azione_ = -1, riga_briefing_ = -1, lingua_briefing_ = 0;   // 2: la radio
    int messaggio_radio_ = -1;
    bool esc_avvisato_ = false;                  // Esc con modifiche non salvate: il secondo esce
    std::string parte_nuova_ = "urss", classe_nuova_ = "krivak2";
    std::optional<Campo> campo_;
    std::vector<AvvisoScenario> avvisi_;
    std::string messaggio_, riepilogo_;
    std::filesystem::path cartella_, esportate_;
    bool modificato_ = false;
    // trascinamento
    std::optional<Vec2> trascina_da_;            // punto della tela dove e' cominciato
    int trascina_unita_ = -1, trascina_zona_ = -1;
    Vec2 centro_iniziale_;
    bool trascinato_ = false;
    double ripeti_ = 0.0;                        // ripetizione dello scorrimento del pannello
    // pannello
    mutable std::vector<Elemento> elementi_;
    mutable bool elementi_validi_ = false;
    mutable int scorri_ = 0;
    // gruppo 4 della passata estetica (variante C, «SALA PIANI»; scelta dell'utente del 29/10/2026)
    std::optional<Font> targa_, fosforo_;        // incisione688_7 e fosforo688_5x7 (senza: il font del gioco)
    int parte_data_ = 0;                         // la riga DATA: 0 giorno, 1 mese, 2 anno
    mutable std::vector<Rettangolo> linguette_;  // le linguette delle schede dell'ultimo disegno, per i clic
    const Font& font_fosforo() const { return fosforo_ ? *fosforo_ : r_.font; }
};

}  // namespace m688
