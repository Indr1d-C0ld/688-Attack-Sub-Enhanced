#include "grafica/testo_font.hpp"

#include <cctype>
#include <utility>

namespace m688 {

std::string testo_per_font(std::string s) {
    static const std::pair<const char*, const char*> SOSTITUZIONI[] = {
        {"\u00ab", "\""}, {"\u00bb", "\""}, {"\u201c", "\""}, {"\u201d", "\""}, {"\u2018", "'"}, {"\u2019", "'"},
        {"\u00b2", "2"}, {"\u00b3", "3"}, {"\u00d7", "X"}, {"\u2032", "'"}, {"\u2033", "\""}, {"\u2013", "-"}, {"\u2014", "-"},
        {"\u2248", "~"}, {"\u00b1", "+-"}, {"\u2265", ">="}, {"\u2264", "<="}, {"\u2192", "->"}, {"\u00b5", "U"}, {"\u2026", "..."},
        // il font ha solo le accentate italiane: paragrafo, lettere straniere e greche
        {"\u00a7", "PAR."}, {"\u00aa", "A"}, {"\u00fc", "U"}, {"\u00dc", "U"}, {"\u00f6", "O"}, {"\u00d6", "O"}, {"\u00e4", "A"},
        {"\u00c4", "A"}, {"\u00eb", "E"}, {"\u00ce", "I"}, {"\u00ee", "I"}, {"\u00d8", "O"}, {"\u00f8", "O"}, {"\u00e5", "A"},
        {"\u00e7", "C"}, {"\u00f1", "N"}, {"\u014d", "O"}, {"\u0144", "N"}, {"\u010d", "C"}, {"\u010c", "C"}, {"\u017e", "Z"},
        {"\u017d", "Z"}, {"\u0161", "S"}, {"\u0160", "S"}, {"\u03c3", "SIGMA"},
        // le accentate minuscole nelle maiuscole accentate del font; la a acuta manca nel set grande: A semplice
        {"\u00e0", "\u00c0"}, {"\u00e1", "A"}, {"\u00c1", "A"}, {"\u00e8", "\u00c8"}, {"\u00e9", "\u00c9"}, {"\u00ec", "\u00cc"},
        {"\u00ed", "\u00cd"}, {"\u00f2", "\u00d2"}, {"\u00f3", "\u00d3"}, {"\u00f9", "\u00d9"}, {"\u00fa", "\u00da"}};
    // il cirillico traslitterato (le voci hanno gia' nome_lat e testo_lat; questo vale per il resto)
    static const char* const CIRILLICO[32] = {"A", "B", "V", "G", "D", "E", "ZH", "Z", "I", "Y", "K", "L", "M", "N", "O", "P",
                                              "R", "S", "T", "U", "F", "KH", "TS", "CH", "SH", "SHCH", "", "Y", "'", "E", "YU", "YA"};
    std::string r;
    for (std::size_t i = 0; i < s.size();) {   // А-Я (D0 90 - D0 AF), а-п (D0 B0 - D0 BF), р-я (D1 80 - D1 8F)
        const int b0 = static_cast<unsigned char>(s[i]), b1 = i + 1 < s.size() ? static_cast<unsigned char>(s[i + 1]) : 0;
        int k = -1;
        if (b0 == 0xD0 && b1 >= 0x90 && b1 <= 0xAF) k = b1 - 0x90;
        else if (b0 == 0xD0 && b1 >= 0xB0 && b1 <= 0xBF) k = b1 - 0xB0;
        else if (b0 == 0xD1 && b1 >= 0x80 && b1 <= 0x8F) k = b1 - 0x80 + 16;
        if (k >= 0) {
            r += CIRILLICO[k];
            i += 2;
        } else {
            r += s[i++];
        }
    }
    s = std::move(r);
    for (const auto& [da, a] : SOSTITUZIONI)
        for (std::size_t k = s.find(da); k != std::string::npos; k = s.find(da, k + std::char_traits<char>::length(a)))
            s.replace(k, std::char_traits<char>::length(da), a);
    for (auto& c : s) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return s;
}

}  // namespace m688
