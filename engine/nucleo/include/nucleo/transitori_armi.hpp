#pragma once
// Transitori delle armi e degli scafi (pezzo R7 del dossier docs/ricerca/18_audio_reale.md §8; numeri e fonti in
// docs/ricerca/18_note/transitori_armi_r7.md, citato «r7»): le sequenze di transitori della libreria [transitori] di
// acustica.toml che un evento della missione produce. Lancio dal tubo e da pozzo, esplosione con le pulsazioni della
// bolla (Cole e Willis, dossier 07 §1), implosione di uno scafo alla quota di collasso coi cedimenti che seguono,
// affondamento di una nave, e la cadenza dei ping della testa attiva di un siluro.
//
// Il modulo e' puro e deterministico: le funzioni restituiscono Transitorio da mettere nella scena acustica (la
// missione li passa a sorgente_transitoria come gli altri); la cuffia e il bordo li sentono da soli. Gli id vanno da
// `id_base` in su e devono essere unici nella scena.

#include <cstdint>
#include <string>
#include <vector>

#include "nucleo/acustica_asw.hpp"

namespace m688 {

// --- lancio ---
enum class TipoLancio : std::uint8_t { siluro, harpoon, tomahawk_tubo, tomahawk_vls };
// La sequenza di un lancio con l'istante 0 allo sparo (t0): l'allagamento (35 s prima) e l'equalizzazione (10 s
// prima) se `preparazione`, la porta (5 s prima), lo sparo del pistone ad acqua col ritorno 3-8 s dopo, poi l'avvio del
// siluro 0,5-2 s dopo (o i tappi della capsula dell'Harpoon 20-60 s dopo, o il booster del Tomahawk). Dal pozzo VLS:
// il portello e il booster
std::vector<Transitorio> transitori_lancio(TipoLancio tipo, Vec2 pos, double quota_m, double t0, bool preparazione,
                                           int id_base, std::uint64_t seme);

// --- esplosioni ---
// Periodo della prima bolla (Rayleigh-Willis, dossier 07 §1.4): T = 2,11 W^1/3 / (d + 10,1)^5/6, s
double periodo_bolla_s(double tnt_kg, double quota_m);
// Il guadagno sulla voce di 1 kg di TNT: picco di Cole, 20 x 1,13 x log10(W^1/3)
double guadagno_esplosione_db(double tnt_kg);
// L'urto e le pulsazioni della bolla (T1 di Willis, poi T2 = 0,74 T1 e T3 = 0,63 T1, 14, 20 e 26 dB sotto l'urto;
// r7 §3.2). Vicino alla superficie (quota < raggio massimo della bolla, ~3,4 W^1/3 m) la bolla sfiata: niente pulsazioni
std::vector<Transitorio> transitori_esplosione(double tnt_kg, Vec2 pos, double quota_m, double t0, int id_base,
                                               int origine = -1);

// --- scafi ---
// L'implosione di uno scafo alla quota di collasso: TNT equivalente (2-3) p V / 4,184 MJ (r7 §4.2, da Rule: Scorpion
// 6 t a 470 m, San Juan 5,2 t a 468 m), urto 12 dB sotto quello di un'esplosione uguale, le bolle, poi 10-20 cedimenti
// minori in 200 s sempre piu' profondi
double tnt_implosione_kg(double quota_m, double volume_m3);
// Il volume asciutto che implode, dalle dimensioni della classe (ClasseNave::lunghezza_m e larghezza_m): 0,29 L B^2,
// cioe' il cilindro del diametro massimo per 0,75 di forma e per meta' asciutto al collasso (STIMA tarata sullo
// Scorpion: 76,7 x 9,6 m danno ~2050 m^3, ~6 t a 470 m come Rule; un 688 ~3200 m^3), fra 300 e 15 000 m^3
double volume_implosione_m3(double lunghezza_m, double larghezza_m);
std::vector<Transitorio> transitori_implosione(double quota_m, double volume_m3, Vec2 pos, double t0, int id_base,
                                               std::uint64_t seme, int origine = -1);
// Una nave che affonda: allagamento, aria che sfugge, la rottura (5-10 minuti di colpi), e l'urto sul fondo quando
// ci arriva (`t_fondo`, < 0 se non ci arriva)
std::vector<Transitorio> transitori_affondamento(Vec2 pos, double t0, double t_fondo, double fondale_m, int id_base,
                                                 std::uint64_t seme, int origine = -1);

// --- teste attive dei siluri ---
// La voce del ping della testa di una classe d'arma ("mk48" -> "ping_testa_mk48"; type53 e i filoguidati senza
// testa attiva: "")
std::string transitorio_testa(const std::string& classe_arma);
// Intervallo fra due ping (r7 §2.4: PRI = 1,2 x 2R / c con R la distanza dal bersaglio entro la portata di ricerca;
// 5,5 s a 3,6 km, 0,16 s a 100 m)
double intervallo_ping_testa_s(double distanza_m, double portata_m = 3600.0);

}  // namespace m688
