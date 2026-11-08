#pragma once
// Il codice partita (richiesta dell'utente, 26/10/2026): ricrea la stessa partita di una missione originale, della
// libreria, dell'editor o importata. Contiene:
// - l'impronta del file della missione: la missione e la sua versione, senza scrivere il nome;
// - il seme della partita: varianti dell'originale, dadi dell'IA, meteo, danni;
// - le opzioni del menu.
// Le missioni rapide hanno il loro codice (missioni/missione_rapida.hpp), che le genera.
//
// 90 bit in 18 caratteri di Crockford, a gruppi di sei: versione 3, origine 2, impronta 32, seme 31, GPS 1, stazioni
// 1989 1, carta d'epoca 1, meteo realistico 1, ASW 2, sonarista 2, navigazione realistica 1, senza traffico 1 (era la
// riserva: i codici di prima lo leggono acceso), controllo 12 (CRC-12).
// Versione 2 (29/10/2026, scelta dell'utente): il GPS dell'opzione «1990+» prende un bit al seme, che scende a 31 bit;
// i codici della versione 1 (seme a 32 bit, senza GPS) si leggono ancora.
// La lettura non distingue maiuscole e minuscole, ignora trattini e spazi; O vale 0, I e L valgono 1.

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "regolazioni_sandbox.hpp"

namespace m688 {

inline constexpr int VERSIONE_CODICE_PARTITA = 2;

enum class OrigineCodice : std::uint8_t { originale, libreria, editor, importata };
const char* nome_origine(OrigineCodice o);   // "originale", "libreria", "editor", "importata"

struct CodicePartita {
    OrigineCodice origine = OrigineCodice::originale;
    std::uint32_t impronta = 0;      // impronta_missione del file
    std::uint32_t seme = 0;
    bool anni_89 = false, carta_epoca = true, meteo_realistico = false;
    int asw = 1;                     // 0-3, come SceltaPartita
    int sonarista = 2;               // 0-3
    bool navigazione = false;        // la posizione propria incerta (nucleo/navigazione); i codici di prima: perfetta
    bool senza_traffico = false;     // il traffico civile spento (docs/20 §3); i codici di prima: acceso
    bool gps = false;                // l'opzione «1990+»: il GPS fra i punti nave (docs/08 §5); i codici v1: senza
    bool operator==(const CodicePartita&) const = default;
};

std::string scrivi_codice_partita(const CodicePartita& c);   // «XXXXXX-XXXXXX-XXXXXX»
std::optional<CodicePartita> leggi_codice_partita(std::string_view codice);

// Il codice sandbox (il vetrino dell'IA, docs/19_sandbox.md): il codice partita con le regolazioni del vetrino, in 150
// bit e 30 caratteri: la base di 77 bit; per NATO, Patto e neutrali la situazione (2), le ROE (2), il livello (2),
// l'allerta di partenza (3: missione, A0-A3) e il carico per classe (1); la durata in quarti d'ora (7), fermarsi
// all'esito e i sei interruttori dell'IA (7); l'arresto sugli eventi (1); senza traffico (1); riserva fino a 138;
// controllo 12. Meteo, navigazione, sensori e seme del vetrino sono quelli della base
struct CodiceSandbox {
    CodicePartita partita;
    RegolazioniSandbox regolazioni;
};
std::string scrivi_codice_sandbox(const CodiceSandbox& k);   // «XXXXXX-XXXXXX-XXXXXX-XXXXXX-XXXXXX»
std::optional<CodiceSandbox> leggi_codice_sandbox(std::string_view codice);

// FNV-1a a 32 bit dell'id e del contenuto del file: cambia se la missione cambia. 0 se il file non si legge
std::uint32_t impronta_missione(const std::string& id, const std::filesystem::path& file);

// La missione del codice fra i file .lua delle cartelle (l'id e' il nome del file senza estensione)
struct MissioneDelCodice {
    std::string id;
    std::filesystem::path file;
};
std::optional<MissioneDelCodice> trova_missione(std::uint32_t impronta, const std::vector<std::filesystem::path>& cartelle);

// Un seme nuovo per ogni partita dal menu: mai 0 (il «nessuno» della carriera)
std::uint32_t seme_nuovo();   // da 1 a 2^31 - 1: entra nel codice partita (31 bit)

}  // namespace m688
