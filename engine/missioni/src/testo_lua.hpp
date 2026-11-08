#pragma once
// Aiuti per scrivere testo Lua (missioni rapide e scenari): numeri senza zeri in coda, stringhe tra virgolette.
#include <cstdio>
#include <string>

namespace m688::testo_lua {

inline std::string num(double x, int decimali = 1) {
    char b[64];
    std::snprintf(b, sizeof b, "%.*f", decimali, x);
    std::string s = b;
    if (s.find('.') != std::string::npos) {
        while (s.back() == '0') s.pop_back();
        if (s.back() == '.') s.pop_back();
    }
    if (s == "-0") s = "0";
    return s;
}

// numero esatto (per il blocco `editor`, che si rilegge): %.17g
inline std::string esatto(double x) {
    char b[64];
    std::snprintf(b, sizeof b, "%.17g", x);
    return b;
}

inline std::string stringa(const std::string& s) {
    std::string o = "\"";
    for (const char c : s) {
        if (c == '"' || c == '\\') o += '\\';
        if (c == '\n') { o += "\\n"; continue; }
        o += c;
    }
    return o + "\"";
}

}  // namespace m688::testo_lua
