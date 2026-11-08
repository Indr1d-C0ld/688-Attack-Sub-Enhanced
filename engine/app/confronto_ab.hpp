#pragma once
// Il confronto di un esperimento A/B del vetrino di Dio (docs/28 §5, D2; nota condivisa trailer1-d2-forma-m688v.md):
// due serie sugli stessi semi, A senza e B con gli interventi, partita i di A in coppia con la partita i di B. Dalle tracce
// delle due serie il confronto statistico (nucleo/confronto_serie del disassemblatore: Wilson e Newcombe appaiato per gli
// esiti, bootstrap sulle coppie per i numeri) e il punto di divergenza di ogni coppia. Lo scrive strumenti/serie_ab in
// <uscita>/confronto.toml, accanto alle cartelle A e B delle due serie; lo legge app/schermata_confronto. Il rapporto
// scritto (rapporto_confronto) e' in Markdown: ipotesi, interventi, semi, numeri con gli intervalli, coppie.
//
//   [confronto]   titolo, mostra, ipotesi, missione, codice, da, coppie, cartella_a, cartella_b, data
//   [interventi]  righe = [ "40:00 sensore (perry 3), fattore 0, per 20 min", ... ]   (dalla prima traccia di B)
//   [esiti."codice"]  testo, a = { k, n, p, basso, alto }, b = {...}, differenza, basso, alto, solo_a, solo_b, entrambe, nessuna
//   [numeri."parte".nome]  coppie, mediana_a, mediana_b, differenza, basso, alto, sale, scende, uguale   (parte 8, 16, 0)
//   [generali.nome]   lo stesso (durata_s)
//   [[coppia]]    seme, traccia_a, traccia_b, esito_a, esito_b, divergono, t_s, unita, come, t_intervento_s,
//                 dopo_intervento_s, indice_evento, evento_a, evento_b, t_evento_a, t_evento_b
// I valori mancanti sono -1.

#include <cstdint>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include "nucleo/confronto_serie.hpp"
#include "nucleo/traccia_replay.hpp"

namespace m688 {

struct CoppiaConfronto {
    std::uint64_t seme = 0;
    std::string traccia_a, traccia_b;       // relative alla cartella del confronto
    int esito_a = -1, esito_b = -1;
    bool divergono = false;
    double t_s = -1.0;                      // il punto di divergenza
    int unita = -1;
    std::string come;
    double t_intervento_s = -1.0, dopo_intervento_s = -1.0;
    int indice_evento = -1;                 // il primo evento del registro che cambia
    std::string evento_a, evento_b;         // i loro testi (vuoto: manca in quella partita)
    double t_evento_a = -1.0, t_evento_b = -1.0;
};

struct DatiConfronto {
    std::string titolo, mostra, ipotesi, missione, codice, data;
    std::string cartella_a = "A", cartella_b = "B";
    std::uint64_t da = 1;
    std::vector<std::string> interventi;   // le righe degli interventi applicati (della prima partita di B)
    ConfrontoSerie confronto;
    std::map<int, std::string> testi_esiti;
    std::vector<CoppiaConfronto> coppie;
};

// Dalle tracce delle due serie, nell'ordine dei semi da `da` (la stessa lunghezza); `nomi_a` e `nomi_b` i loro file,
// relativi alla cartella del confronto
DatiConfronto confronta_tracce(const std::vector<Traccia>& a, const std::vector<Traccia>& b, const std::vector<std::string>& nomi_a,
                               const std::vector<std::string>& nomi_b, std::uint64_t da, const ParametriBootstrap& par = {});

void scrivi_confronto(const std::filesystem::path& p, const DatiConfronto& d);   // std::runtime_error se non si scrive
DatiConfronto leggi_confronto(const std::filesystem::path& p);                     // std::runtime_error se non si legge

// Il bootstrap sulle coppie con poche coppie da' intervalli stretti e falsi (con 2 coppie ci sono 4 ricampionamenti
// diversi): una differenza di numeri si dice significativa solo da COPPIE_MINIME coppie in su. Gli esiti (Wilson e
// Newcombe) restano validi anche con poche coppie: i loro intervalli si allargano da soli
inline constexpr int COPPIE_MINIME = 5;
inline bool significativo(const ConfrontoNumero& n) { return n.coppie >= COPPIE_MINIME && n.significativo(); }

// L'etichetta di un numero di numeri_parte e la sua unita' (come la schermata delle serie)
std::string etichetta_numero(const std::string& nome);
std::string unita_numero(const std::string& nome);
double scala_numero(const std::string& nome);   // dal valore del file all'unita' mostrata (km per i metri delle distanze)
const char* nome_parte_confronto(int parte);    // NATO, PATTO, LUOGO, NEUTRALI

// Il rapporto scritto, in Markdown; `immagini`: i file delle pagine della schermata (relativi al rapporto), in fondo
std::string rapporto_confronto(const DatiConfronto& d, const std::vector<std::string>& immagini = {});

}  // namespace m688
