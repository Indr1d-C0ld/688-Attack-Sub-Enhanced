#pragma once
// Il riassunto di una serie della sandbox (docs/19 §6; nota condivisa sandbox-v3-serie-contratto.md): la stessa
// missione con le stesse regolazioni su piu' semi, giocata da strumenti/serie_sandbox e mostrata da app/schermata_serie.
// Il file e' serie.toml nella cartella della serie, accanto alle tracce delle partite:
//
//   [serie]      missione, codice, da (primo seme), semi, durata_s, data
//   [esiti]      "codice" = { volte, testo, frequenza }   (-1: non finita)
//   [parti.X]    nome = { media, dev, min, max, q10, mediana, q90, n }   per X = nato, patto, neutrali (numeri_parte)
//   [pk.X]       "classe d'arma" = { a_segno, lanciate }
//   [[partita]]  seme, esito, esito_testo, traccia (relativa alla cartella), e per parte i numeri di quella partita
//
// I valori mancanti sono -1 nel file, come in nucleo/statistiche_sandbox.

#include <cstdint>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include "nucleo/statistiche_sandbox.hpp"

namespace m688 {

struct PartitaSerie {
    std::uint64_t seme = 0;
    int esito = -1;
    std::string esito_testo;
    std::string traccia;                                    // relativa alla cartella della serie
    std::map<int, std::map<std::string, double>> numeri;   // parte (0x08, 0x10, 0) -> numeri_parte (-1 mancante)
};

struct DatiSerie {
    std::string missione, codice, data;
    std::uint64_t da = 1;
    int semi = 0;
    double durata_s = 0.0;
    RiassuntoSerie riassunto;
    std::vector<PartitaSerie> partite;                     // nell'ordine dei semi
};

// le parti nel file: 0x08 «nato», 0x10 «patto», 0 «neutrali»
const char* nome_parte_serie(int parte);
int parte_serie(const std::string& nome);   // -1 se sconosciuta

// Mette insieme il riassunto: le statistiche delle partite (nell'ordine dei semi, da `da`) e le loro tracce
DatiSerie dati_serie(const std::vector<StatisticheSandbox>& partite, const std::vector<std::string>& tracce, std::uint64_t da);

void scrivi_serie(const std::filesystem::path& p, const DatiSerie& d);   // std::runtime_error se non si scrive
DatiSerie leggi_serie(const std::filesystem::path& p);                     // std::runtime_error se non si legge

}  // namespace m688
