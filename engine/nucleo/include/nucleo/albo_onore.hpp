#pragma once
// Albo d'onore (dossier docs/ricerca/15 §7.10; proposta approvata il 26/10/2026): le migliori missioni e le migliori
// carriere, salvate in albo_onore.toml accanto ad albo.toml, e le schede fisse dei comandanti storici
// (data/motore/albo_onore_storico.toml, da tools/pipeline/achievement.py: fatti documentati, senza punteggi).
// Regole:
// - le missioni entrano solo senza aiuti (realismo 1) e mai se riprese; le rapide col loro codice, per rigiocarle;
// - le carriere entrano solo chiuse (congedo, pensione, rimosso, trasferito, morto): i caduti restano;
// - una voce resta anche se il comandante si toglie dall'albo dei comandanti.
// Il modulo non legge l'orologio: la data reale la passa l'app.

#include <filesystem>
#include <string>
#include <vector>

#include "nucleo/carriera.hpp"

namespace m688 {

inline constexpr int ONORE_MISSIONI = 20, ONORE_CARRIERE = 10;

struct VoceOnoreMissione {
    double punteggio = 0.0;              // Punteggio::totale
    std::string missione, origine;       // id e origine (originale, rapida, editor, libreria)
    std::string codice;                  // rapide: il seme in esadecimale ("" per le altre)
    std::string data, data_reale;        // "AAAA-MM" della carriera; "AAAA-MM-GG" del giorno in cui si e' giocata
    std::string comandante, nome, marina, grado;   // id del fascicolo, nome, usn | vmf, grado alla missione
};
struct VoceOnoreCarriera {
    double punti = 0.0;                  // la somma dei punti delle missioni (come l'albo dei comandanti)
    int missioni = 0;
    std::string comandante, nome, marina, grado;
    std::string decorazione;             // la piu' alta per precedenza ("" nessuna)
    std::string stato;                   // congedo | pensione | rimosso | trasferito | morto
    std::string data;                    // "AAAA-MM" della chiusura
};
struct AlboOnore {
    std::vector<VoceOnoreMissione> missioni;   // dalla migliore, al piu' ONORE_MISSIONI
    std::vector<VoceOnoreCarriera> carriere;   // dalla migliore, al piu' ONORE_CARRIERE
};

std::filesystem::path percorso_albo_onore();   // <cartella dei fascicoli>/../albo_onore.toml
AlboOnore leggi_albo_onore(const std::filesystem::path& p = percorso_albo_onore());   // vuoto se manca o rovinato
void salva_albo_onore(const AlboOnore& a, const std::filesystem::path& p = percorso_albo_onore());

// La posizione (1..ONORE_MISSIONI) in cui entra la missione, 0 se non entra (ripresa, con aiuti, punteggio troppo
// basso). A parita' di punteggio resta prima la piu' vecchia
int proponi_missione(AlboOnore& a, const VoceOnoreMissione& v, bool ripresa, double realismo);
// Una carriera chiusa: la voce di un comandante gia' presente si aggiorna. La posizione, 0 se non entra (in servizio o
// punti troppo bassi)
int proponi_carriera(AlboOnore& a, const Fascicolo& f, const RegoleCarriera* r = nullptr);
// Le due insieme, lette e salvate sul file
int registra_missione_onore(const VoceOnoreMissione& v, bool ripresa, double realismo,
                            const std::filesystem::path& p = percorso_albo_onore());
int registra_carriera_onore(const Fascicolo& f, const RegoleCarriera* r = nullptr,
                            const std::filesystem::path& p = percorso_albo_onore());

// Comandanti storici (dossier 13 §3.3 e §3.6)
struct ImpresaStorica {
    std::string id, marina, battello, comandante, impresa_it, impresa_en, decorazione_it, fonte, confidenza;
    int anno = 0;
};
std::vector<ImpresaStorica> carica_storici(const std::string& percorso);   // in ordine d'anno; std::runtime_error

}  // namespace m688
