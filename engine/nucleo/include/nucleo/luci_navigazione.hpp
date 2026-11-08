#pragma once
// Le luci di navigazione e di fonda (docs/27_luci_navigazione.md, approvato dall'utente il 29/10/2026; solo
// nell'Enhanced): i fanali di ogni classe da data/motore/luci.toml (tools/pipeline/luci.py), con le regole del COLREG
// 1972 per i settori e le portate. Modulo puro: lo stato delle luci di un'unita' lo sceglie il runtime (in moto, alla
// fonda, in pesca, manovra limitata, senza governo, spente), la vista del periscopio traduce i colori in indici.
// Le posizioni sono sul modello 3D che il gioco disegna (x a prua, y a sinistra, z sul galleggiamento, metri).

#include <array>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace m688::luci {

// Lo stato delle luci di un'unita' (la chiave `luci` dello scenario): spente vuol dire nessun fanale
enum class Stato : std::uint8_t { spente, moto, fonda, pesca, manovra_limitata, non_governa };
// Il colore della luce, senza la palette: la vista lo traduce (aloni compresi). luci_coperta: le luci da lavoro
enum class Colore : std::uint8_t { bianco, rosso, verde, giallo, ambra, luci_coperta };

struct Fanale {
    std::string ruolo;                       // albero, albero_poppiero, laterale_sinistro, coronamento, fonda_prora...
    Colore colore = Colore::bianco;
    std::array<double, 3> pos_m{};           // sul modello: x a prua, y a sinistra, z sul galleggiamento
    double settore_da = -180.0;              // gradi dalla prua, positivi a dritta: da settore_da a settore_a in senso
    double settore_a = 180.0;                // orario (il coronamento va da 112,5 a -112,5 passando per la poppa)
    double portata_m = 0.0;                  // la portata del regolamento (regola 22), in metri
    std::uint8_t stati = 0;                  // un bit per Stato: 1 << static_cast<int>(stato)
    bool lampi_7077 = false;                 // il faro ambra dei sottomarini USN: 1 s acceso tre volte, poi 3 s spento
};

struct Configurazione {
    std::string classe, tipo, pesca;         // tipo: mercantile, peschereccio, militare, sottomarino
    double lunghezza_m = 0.0;
    std::vector<Fanale> fanali;
};

// Le configurazioni di luci.toml per classe. Lancia std::runtime_error se il file non si legge
std::map<std::string, Configurazione> carica(const std::string& percorso);

// Il fanale si vede da chi lo guarda con questo rilevamento relativo (gradi dalla prua dell'unita' verso chi guarda,
// positivi a dritta)? Il bordo del settore e' compreso
bool nel_settore(const Fanale& f, double rilevamento_relativo_gradi);

// Il faro ambra (32 CFR 707.7): un lampo al secondo per tre secondi, poi tre secondi spento, in un ciclo di 6 s. Il
// lampo dura mezzo secondo (STIMA: la norma non lo dice): acceso da 0 a 0,5 s, da 1 a 1,5 e da 2 a 2,5 del ciclo
bool lampo_acceso(double tempo_s);

// I fanali accesi in quello stato, adesso (i lampi col tempo). Spente: nessuno
std::vector<const Fanale*> accesi(const Configurazione& c, Stato s, double tempo_s);

// Lo stato predefinito di un'unita' (lo sovrascrivono la chiave `luci` dello scenario e i danni, che il runtime mette
// in non_governa), dal tipo della sua configurazione:
// - spente se la missione lo dice, e per le militari e i sottomarini in guerra;
// - pesca per i pescherecci sotto i 5 nodi (lo strascico va a 3-4 nodi, STIMA);
// - fonda se e' ferma (sotto mezzo nodo);
// - moto altrimenti.
// `velocita_m_s` come Nave::velocita
Stato stato_predefinito(const std::string& tipo, double velocita_m_s, bool guerra, bool missione_spente);

// Lo stato dal nome della chiave dello scenario («moto», «fonda», «pesca», «manovra_limitata», «non_governa»,
// «spente»); un nome sconosciuto e' `spente` e `ok` falso
Stato stato_da_nome(const std::string& nome, bool* ok = nullptr);

}  // namespace m688::luci
