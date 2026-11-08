#pragma once
// Risorse dell'app caricate una volta: palette del giorno, font, classi, sprite e modelli 3D

#include <map>
#include <string>

#include "grafica/font.hpp"
#include "grafica/modello.hpp"
#include "grafica/palette.hpp"
#include "grafica/sprite.hpp"
#include "nucleo/classi.hpp"

namespace m688 {

struct Risorse {
    Palette giorno;
    Font font;
    std::map<std::string, ClasseNave> classi;
    std::map<std::string, InsiemeSprite> sprite;
    std::map<std::string, Modello> modelli;
};

inline Risorse carica_risorse(const std::string& dati) {
    Risorse r{Palette::carica(dati + "/palette/giorno.pal"), Font::carica(dati + "/font/originale.fnt"),
              carica_classi(dati + "/classi.toml"), {}, {}};
    for (const auto& [id, c] : r.classi) {
        r.sprite.emplace(id, InsiemeSprite::carica(dati + "/" + c.sprite));
        if (!c.modello.empty()) r.modelli.emplace(id, Modello::carica(dati + "/" + c.modello));
    }
    return r;
}

}  // namespace m688
