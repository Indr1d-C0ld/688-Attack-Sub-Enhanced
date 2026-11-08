#pragma once
// Asset dell'originale a 320x200 portati sulla tela 640x480 senza colori nuovi (docs/00_ROADMAP.md, "Grafica"):
// Scale2x (EPX) raddoppia preservando i contorni, poi uno stiramento verticale 5:6 al pixel piu' vicino recupera
// l'aspetto 4:3 del monitor CRT (i pixel del 320x200 non erano quadrati).

#include <cstdint>
#include <string>
#include <vector>

namespace m688 {

struct ImmagineIndicizzata {
    int w = 0, h = 0;
    std::vector<std::uint8_t> pix;

    static ImmagineIndicizzata carica(const std::string& percorso);   // file .img della pipeline
};

ImmagineIndicizzata scale2x(const ImmagineIndicizzata& src);
ImmagineIndicizzata raddoppia_nitido(const ImmagineIndicizzata& src);   // pixel duplicati, senza Scale2x
ImmagineIndicizzata stira_verticale(const ImmagineIndicizzata& src, int nuova_altezza);   // pixel piu' vicino
// 320x200 -> 640x480: Scale2x (o raddoppio nitido) e stiramento 5:6.
ImmagineIndicizzata porta_a_640x480(const ImmagineIndicizzata& src, bool nitido = false);

}  // namespace m688
