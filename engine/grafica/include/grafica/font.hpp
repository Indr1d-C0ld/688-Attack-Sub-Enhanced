#pragma once
// Font proporzionale dell'originale (GAME.EXE, DS:9D5C) con le lettere accentate italiane aggiunte dalla
// pipeline. Le stringhe sono UTF-8. Lo slot delle minuscole contiene maiuscole grandi (7x7), quello delle
// maiuscole maiuscole piccole (5x5): "Rilevamento" si legge come maiuscolo con l'iniziale piccola, come
// nell'originale. Sulla tela 640x480 il font si usa di norma raddoppiato (scala 2).
// L'originale allinea i glifi in alto: cifre e maiuscole piccole (5 righe) accanto alle maiuscole grandi
// (7 righe) sembrano apici. Con `allinea_base` (predefinito) i glifi alti 5 righe scendono di 2 sulla linea
// di base delle maiuscole grandi; per ridisegnare le schermate originali si disattiva.
//
// Due formati di file (tools/pipeline/esporta_motore.py, tools/pipeline/font_stazioni.py):
// - FNT1, il font dell'originale: righe da un byte (al piu' 8 colonne), un solo colore, le regole qui sopra;
// - FNT2, i font delle stazioni della Fase 3 (dossier 19 §9.4): fino a 16 colonne, 2 bit per pixel (1 mezzo tono,
//   2 tratto, 3 nucleo luminoso), solo maiuscole: le minuscole e le accentate minuscole si scrivono con le maiuscole,
//   senza maiuscole piccole ne' discese. Il livello 1 da' le estremita' ammorbidite dell'incisione, l'alone del
//   fosforo, i segmenti spenti delle cifre; scrivi() disegna i livelli 2 e 3 col colore dato, scrivi_toni() ognuno
//   col suo.

#include <array>
#include <cstdint>
#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace m688 {

class Tela;

class Font {
public:
    static Font carica(const std::string& percorso);   // lancia std::runtime_error

    int righe() const { return righe_; }
    int altezza_riga(int scala = 2) const { return (righe_ + 1) * scala; }
    // `grandi` (predefinito): ogni lettera, maiuscola o minuscola, si scrive con le maiuscole grandi dello slot delle
    // minuscole, cosi' i testi in italiano normale ("Tempo scaduto") escono uniformi; con false si usa la convenzione
    // dell'originale (maiuscole = maiuscole piccole), utile per testi secondari piu' compatti.
    int larghezza(std::string_view testo, int scala = 2, int spazio = 1, bool grandi = true) const;
    // Scrive il testo con l'angolo in alto a sinistra in (x, y); restituisce la x dopo l'ultimo carattere.
    int scrivi(Tela& t, int x, int y, std::string_view testo, std::uint8_t colore, int scala = 2,
               int spazio = 1, bool grandi = true) const;
    // Con un indice per livello (toni[0] mezzo tono, toni[1] tratto, toni[2] nucleo); un indice < 0 non si disegna
    int scrivi_toni(Tela& t, int x, int y, std::string_view testo, const std::array<int, 3>& toni, int scala = 1,
                    int spazio = 1) const;
    int formato() const { return formato_; }          // 1 o 2
    int spazio_predefinito() const { return spazio_; }   // FNT2: lo spazio fra le lettere del disegno (FNT1: 1)
    bool ha(char32_t cp) const { return glifi_.count(cp) != 0; }
    void allinea_base(bool si) { allinea_base_ = si; }

    static std::vector<char32_t> decodifica_utf8(std::string_view s);
    // I caratteri del testo come li scrive il font: una lettera che il font non ha si scrive con la sua traslitterazione
    // latina di riserva (Æ AE, Ø O, Å Ä À A, Ö O, Ü U, ß ss, Ñ N, Š S, Ł L...), le minuscole allo stesso modo; senza
    // riserva, «?». Cosi' «FÆR ØER» si scrive «FAER OER» con ogni font. Lo usano scrivi() e larghezza().
    std::vector<char32_t> caratteri(std::string_view testo, bool grandi = true) const;

private:
    struct Glifo {
        int w = 0;
        int discesa = 0;   // righe di cui scendere con allinea_base
        std::vector<std::uint32_t> righe;   // 2 bit per colonna, colonna 0 nei bit alti (30-31)
        int livello(int r, int c) const { return static_cast<int>((righe[static_cast<std::size_t>(r)] >> (30 - 2 * c)) & 3u); }
    };
    const Glifo& glifo(char32_t cp, bool grandi = false) const;
    const Glifo* trova(char32_t cp, bool grandi) const;   // nullptr se il font non ha la lettera
    int disegna(Tela& t, int x, int y, std::string_view testo, const std::array<int, 3>& toni, int scala, int spazio,
                bool grandi) const;

    int formato_ = 1, spazio_ = 1;
    int righe_ = 15;
    bool allinea_base_ = true;
    std::map<char32_t, Glifo> glifi_;
};

}  // namespace m688
