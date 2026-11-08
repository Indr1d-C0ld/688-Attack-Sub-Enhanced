#pragma once
// Esiti narrativi (M4, dossier 15 §7.8): la pagina di giornale d'epoca (USA) e il comunicato TASS (VMF) al posto della
// cerimonia, per la protesta diplomatica dopo un neutrale colpito o il «battello in ritardo» dopo una perdita; e la
// tabella datata del fascicolo (affondamenti e contatti classificati, alla Aces of the Deep). Nelle rampe di
// cerimonia.hpp (carta, inchiostro, timbro, smalto).

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "grafica/cerimonia.hpp"
#include "grafica/font.hpp"
#include "grafica/tela.hpp"

namespace m688 {

// Pagina di giornale (USA): testata, riga della data, titolo su piu' colonne, foto a retino col sommergibile, il testo
// in colonne e il resto della pagina con altri articoli illeggibili. VMF: prima pagina con la testata e le decorazioni
// del giornale, e il comunicato TASS in un riquadro col titolo e il testo. Il testo va a capo da solo; '\n' forza.
void disegna_giornale(Tela& t, const Font& f, Rettangolo r, Marina m, std::string_view testata, std::string_view titolo,
                      std::string_view testo, std::string_view data = {}, std::uint32_t seme = 1);

// Tabella datata del fascicolo su carta: intestazioni e righe (prima colonna la data), con le righe tirate a
// macchina, le colonne in larghezze relative e le righe alternate. Restituisce la y dopo l'ultima riga.
struct ColonnaTabella {
    std::string titolo;
    double larghezza = 1.0;   // relativa
};
int disegna_tabella(Tela& t, const Font& f, Rettangolo r, const std::vector<ColonnaTabella>& colonne,
                    const std::vector<std::vector<std::string>>& righe, std::string_view titolo = {});

}  // namespace m688
