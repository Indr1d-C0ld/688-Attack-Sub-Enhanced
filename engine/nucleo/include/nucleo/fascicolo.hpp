#pragma once
// Fascicolo personale del comandante (Fase 4a, M3; dossier docs/ricerca/13_carriera_gradi_decorazioni.md §5.4 C):
// anagrafica, grado e fase della carriera, calendario, incarichi, qualifiche, note caratteristiche o attestazioni,
// decorazioni, commissioni, provvedimenti e stato. Si salva in TOML (versione di formato nel file) sotto
// ~/.local/share/melchior688/fascicoli/ (o $XDG_DATA_HOME/melchior688/fascicoli/); il passaggio a SQLite piu' avanti
// cambia solo questo modulo. Le regole che lo fanno avanzare sono in nucleo/carriera.hpp.

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace m688 {

enum class Marina : std::uint8_t { usn, vmf };
enum class Indulgenza : std::uint8_t { realistica, indulgente, arcade };
std::string_view nome_marina(Marina m);
std::optional<Marina> marina_da_nome(std::string_view s);
std::string_view nome_indulgenza(Indulgenza i);
std::optional<Indulgenza> indulgenza_da_nome(std::string_view s);

struct Conferimento {
    std::string decorazione, dispositivo;   // dispositivo: "" | "v"
    int numero = 1;                         // conferimento n-esimo (stellina USA, numero URSS)
    std::string data, missione;             // data "AAAA-MM" del calendario della carriera
    bool postuma = false;
    std::vector<int> fatti;                 // id degli eventi del registro
    std::vector<std::string> motivazione_it, motivazione_en;
};
struct Commissione {
    std::string data, grado, esito;         // esito: anticipata | selezionato | non_selezionato | bloccato | congedo
    double media = 0.0;
};
struct Provvedimento {
    std::string data, tipo, motivo;         // tipo: rimozione | corte_marziale | inchiesta | richiamo | biasimo |
                                            // trasferimento | congedo | degradazione | seconda_possibilita
    std::string missione;
};
struct Qualifica { std::string id, data; };   // dolphins, esami_guardia, comando, vsok
struct Incarico { std::string grado, incarico, battello, dal, al; };
struct VoceMissione {                        // una nota caratteristica (USA) o riga d'attestazione (URSS)
    std::string missione, origine, data, grado;
    std::uint64_t seme = 0;
    double punteggio = 0.0, peso = 1.0, punti = 0.0;   // punti = punteggio x peso
    double ordini = 0.0;                     // la voce O normalizzata (per la caratteristica del zampolit)
    std::string giudizio_it, giudizio_en;
    int graduatoria = 1, pari = 1;           // «1 di N» fra i pari grado simulati
    std::string replay;                      // il file della traccia del replay (nucleo/traccia_replay), "" nessuno
};

struct Fascicolo {
    int versione = 1;
    std::string id, nome;
    Marina marina = Marina::usn;
    bool carriera_breve = false, aperta = false;   // aperta: la carriera delle missioni dell'editor
    Indulgenza indulgenza = Indulgenza::realistica;
    std::string provenienza;                 // USNA, NROTC, OCS / nome della VVMU
    std::uint32_t seme = 1;                  // graduatorie simulate
    // stato della carriera
    std::string grado;
    int fase = 0;                            // indice in RegoleCarriera::fasi
    int missioni_nella_fase = 0;
    int prossima_commissione = 0;            // missioni nella fase alla prossima commissione (0 = da calcolare)
    int mancate = 0, blocco_promozioni = 0, mediocri_di_fila = 0;
    bool seconda_possibilita_usata = false, fine_comando_data = false;
    std::string data;                        // calendario "AAAA-MM"
    int anno_nomina = 1989;
    std::string stato = "servizio";          // servizio | congedo | pensione | rimosso | trasferito | morto
    // storia
    std::vector<Incarico> incarichi;
    std::vector<Qualifica> qualifiche;
    std::vector<VoceMissione> missioni;
    std::vector<Conferimento> decorazioni;
    std::vector<Commissione> commissioni;
    std::vector<Provvedimento> provvedimenti;

    bool in_servizio() const { return stato == "servizio"; }
    bool ha_qualifica(const std::string& q) const;
};

// La cartella dei fascicoli: $XDG_DATA_HOME/melchior688/fascicoli o ~/.local/share/melchior688/fascicoli (non la crea)
std::filesystem::path cartella_fascicoli();
std::filesystem::path percorso_fascicolo(const std::string& id);   // <cartella>/<id>.toml
std::vector<std::string> elenco_fascicoli();                       // gli id, in ordine
// std::runtime_error se il file manca o non e' un fascicolo di una versione nota
Fascicolo leggi_fascicolo(const std::filesystem::path& p);
// crea la cartella se serve; scrive in un file temporaneo e lo rinomina, cosi' un'interruzione non lo tronca
void salva_fascicolo(const Fascicolo& f, const std::filesystem::path& p);

}  // namespace m688
