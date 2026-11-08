#pragma once
// Modello 3D di una classe (data/motore/modelli/<classe>.mdl, tools/navi/esporta_modello.py) e sprite generati al
// volo per le navi danneggiate (docs/10_danni_affondamento_esplosioni.md §5.1). Il modello e' quello del kit con
// l'opera viva; il colore viene da una tessitura "cotta" dalle stesse foto degli sprite. Il disegno ripete la
// pipeline di tools/navi/costruisci.py (camera da 4 km, sovracampionamento 3x3, ombreggiatura di ripiego, 16 grigi
// con retinatura di Bayer), quindi a nave intatta lo sprite generato coincide con quello precalcolato.

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include "grafica/palette.hpp"
#include "grafica/sprite.hpp"
#include "nucleo/mondo.hpp"

namespace m688 {

class Modello {
public:
    struct Triangolo {
        std::array<std::array<float, 3>, 3> v;      // vertici (x a prua, y a sinistra, z in alto), m
        std::array<std::array<float, 2>, 3> uv[2];  // coordinate di tessitura (texel): faccia diretta, opposta
        float albedo = 0.5f;
        bool retinato = false;                      // dischi di eliche e rotori: a scacchiera
        std::uint16_t parte = 0;
    };

    static Modello carica(const std::string& percorso);   // lancia std::runtime_error

    const std::vector<Triangolo>& triangoli() const { return tri_; }
    const std::vector<std::string>& parti() const { return parti_; }
    float lunghezza_m() const { return lunghezza_; }      // fuori tutto, senza opera viva (come gli sprite)
    float quota_max_m() const { return quota_max_; }      // sopra il galleggiamento
    float tono() const { return tono_; }
    float luce_ambiente() const { return ambiente_; }
    int larghezza_tessitura() const { return tw_; }
    int altezza_tessitura() const { return th_; }
    // valore delle foto e loro quota rispetto all'ombreggiatura (0..1), interpolati in (u, v) texel
    void campiona(float u, float v, float& valore, float& quota_foto) const;

private:
    std::vector<Triangolo> tri_;
    std::vector<std::string> parti_;
    float lunghezza_ = 0.0f, quota_max_ = 0.0f, tono_ = 0.45f, ambiente_ = 0.55f;
    int tw_ = 0, th_ = 0;
    std::vector<std::uint8_t> valore_, quota_foto_;
};

// Rampa dei 16 grigi degli sprite (canale rosso degli indici 144-158 della palette del giorno).
std::array<double, 15> rampa_grigi(const Palette& giorno);

// Sprite della nave nella posa data, visto dall'angolo sulla prua `aob` (0-180) sul lato indicato, a `scala_px_m`
// pixel per metro (3,75 come gli sprite precalcolati; meno per le navi lontane). La parte sotto il mare non si
// vede; l'ancora e' la proiezione del centro nave sul livello del mare. `sovracampiona` x `sovracampiona` campioni
// per pixel (3 come gli sprite precalcolati; 2 per le navi intatte vicine rigenerate a ogni grado, un quarto del costo).
Sprite genera_sprite(const Modello& m, const PosaNave& posa, double aob, bool dritta, const std::array<double, 15>& rampa,
                     double scala_px_m = 3.75, int sovracampiona = 3);

}  // namespace m688
