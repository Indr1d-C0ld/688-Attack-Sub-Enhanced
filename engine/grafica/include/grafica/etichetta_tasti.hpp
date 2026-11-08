#pragma once
// Come si scrivono a schermo i tasti di `[` e `]` nell'Enhanced: "[ ]", o la forma del layout dove non sono diretti, come "ALTGR+È/+"
// sulla tastiera italiana. La imposta la finestra dalla mappa dei tasti (piattaforma/src/finestra.cpp); le schermate senza finestra e
// le prove restano a "[ ]", o prendono M688_TASTI_VALORE. Il classico non la usa: le sue scritte non cambiano.

#include <cstdlib>
#include <string>

namespace m688 {

inline std::string& tasti_valore() {
    static std::string s = [] {
        const char* e = std::getenv("M688_TASTI_VALORE");
        return std::string(e ? e : "[ ]");
    }();
    return s;
}

}  // namespace m688
