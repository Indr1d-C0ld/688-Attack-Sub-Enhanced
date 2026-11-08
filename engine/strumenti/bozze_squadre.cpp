// Le bozze della pagina SQUADRE della stazione Stato (F3; punto 4 del piano): il quadro danni vero, spaccato e didascalie dal
// BattelloDanni, con la pagina al posto del dettaglio. Le porte e la posizione del cursore sono messe a mano: il modello
// dell'isolamento non c'e' ancora.
// Uso: bozze_squadre <dati> <cartella d'uscita> <font della pagina, sotto font/>
#include <cstdio>
#include <string>
#include <vector>

#include "grafica/font.hpp"
#include "grafica/modello.hpp"
#include "grafica/pagina_squadre.hpp"
#include "grafica/palette.hpp"
#include "grafica/spaccato.hpp"
#include "grafica/tela.hpp"
#include "nucleo/battello.hpp"
#include "nucleo/scafo.hpp"

using namespace m688;

namespace {

struct Bozza {
    const char* uscita;
    const char* classe;
    int bruciato;                   // il compartimento in fiamme
    std::vector<PortaOtsek> porte;
    int scelto, cursore, libere;
    bool pagina = true;
};

// l'ordine di partenza delle riparazioni (nucleo/battello.cpp): propulsione, motori, idroplani, timone, aria, pompe, sensori
const Sistema ORDINE[] = {Sistema::propulsione, Sistema::motori, Sistema::idroplani, Sistema::timone, Sistema::aria, Sistema::pompe,
                          Sistema::sonar_prua, Sistema::schiera, Sistema::sonar_tiro, Sistema::calcolatore_sonar, Sistema::periscopio,
                          Sistema::antenna, Sistema::inerziale};

}  // namespace

int main(int argc, char** argv) {
    if (argc < 4) {
        std::fprintf(stderr, "uso: %s <dati> <cartella d'uscita> <font>\n", argv[0]);
        return 2;
    }
    const std::string dati = argv[1], fuori = argv[2];
    const Font originale = Font::carica(dati + "/font/originale.fnt"), piccolo = Font::carica(dati + "/font/" + argv[3]);
    using S = PortaOtsek::Stato;
    const std::vector<Bozza> bozze = {
        {"squadre_688_chiusura", "los_angeles", 2, {{"C1 COMANDO", S::comando}, {"C2 REATTORE", S::aperto}, {"C3 MACCHINE", S::in_chiusura, 42, 19}}, 2, 4, 0},
        {"squadre_688_chiuso", "los_angeles", 2, {{"C1 COMANDO", S::comando}, {"C2 REATTORE", S::aperto}, {"C3 MACCHINE", S::chiuso, 0, 6}}, 2, 0, 1},
        {"squadre_alfa_chiuso", "alfa", 4, {{"I TORP.", S::aperto}, {"II AKK.", S::aperto}, {"III TSP", S::comando}, {"IV REA.", S::aperto}, {"V TURB.", S::chiuso}, {"VI VSP.", S::aperto}}, 4, 3, 1},
        {"squadre_alfa_pagina_normale", "alfa", 4, {{"I TORP.", S::aperto}, {"II AKK.", S::aperto}, {"III TSP", S::comando}, {"IV REA.", S::aperto}, {"V TURB.", S::aperto}, {"VI VSP.", S::aperto}}, 4, 0, 1, false},
        {"squadre_688_pagina_normale", "los_angeles", 2, {{"C1 COMANDO", S::comando}, {"C2 REATTORE", S::aperto}, {"C3 MACCHINE", S::in_chiusura, 42, 19}}, 2, 0, 0, false},
    };
    for (const Bozza& z : bozze) {
        const auto modello = Modello::carica(dati + "/modelli/" + z.classe + ".mdl");
        const auto scafo = DatiScafo::carica(dati + "/scafi/" + z.classe + ".toml");
        auto pal = Palette::carica(dati + "/palette/giorno.pal");
        Spaccato::imposta_palette(pal);
        const Spaccato sp(z.classe, modello, scafo);
        BattelloDanni b(scafo, 7);
        const StatoMoto moto{100.0, 6.0};
        b.urto(135, 25, 1.57, b.compartimenti()[static_cast<std::size_t>(z.bruciato)].xc, 100.0);
        b.incendio(z.bruciato, 0.8);
        for (double x = 0; x < 90.0; x += 0.5) b.avanza(0.5, moto);

        Tela t;
        t.rettangolo_pieno(0, 0, 640, 360, colori::NERO);
        const bool alfa = std::string(z.classe) == "alfa";
        originale.scrivi(t, 8, 4, alfa ? "PULT ZHIVUCHESTI" : "QUADRO DANNI", colori::CIANO_CHIARO, 1);
        const Rettangolo intero{4, 20, 632, 108}, didascalie{4, 128, 632, 34};
        const StatoSpaccato stato = StatoSpaccato::da(b, 100.0, 0.0, 90.37);
        sp.disegna(t, intero, stato);
        sp.disegna_didascalie(t, originale, intero, didascalie, stato);

        DatiPaginaSquadre d;
        d.alfa = alfa;
        d.porte = z.porte;
        d.scelto = z.scelto;
        d.cursore = z.cursore;
        d.squadre = 3;
        d.libere = z.libere;
        for (const Sistema s : ORDINE) {
            RigaSquadre r;
            const StatoSistema& st = b.sistemi()[static_cast<std::size_t>(s)];
            r.nome = nome_sistema(s);
            r.efficienza = st.efficienza;
            r.massimo = st.massimo;
            for (const auto& q : b.squadre())
                if (q.sistema == static_cast<int>(s)) r.squadre += q.numero;
            d.righe.push_back(r);
        }
        disegna_porte_spaccato(t, originale, sp, intero, d);
        for (std::size_t i = 0; i < d.porte.size(); ++i) {
            const auto& c = b.compartimenti()[i];
            auto& p = d.porte[i];
            p.acqua = c.volume > 0.0 ? c.acqua / c.volume : 0.0;
            p.fuoco = c.fuoco;
            p.ossigeno = c.ossigeno;
            for (const auto& q : b.squadre())
                if (q.compartimento == static_cast<int>(i)) p.squadre += q.numero;
        }
        if (z.pagina) disegna_pagina_squadre(t, piccolo, {4, 164, 632, 148}, d);
        else sp.disegna_dettaglio(t, {4, 164, 632, 148}, stato, b.compartimenti()[static_cast<std::size_t>(z.scelto)].x0, b.compartimenti()[static_cast<std::size_t>(z.scelto)].x1);
        originale.scrivi(t, 8, 314, alfa ? "OTSEK V   TAB SCEGLIE" : "COMPARTIMENTO 3   TAB SCEGLIE", colori::BIANCO, 1);
        disegna_tasti_squadre(t, piccolo, 636, 312, d, z.pagina);
        Tela ritaglio(640, 360);
        for (int y = 0; y < 360; ++y) std::copy(t.riga(y), t.riga(y) + 640, ritaglio.riga(y));
        ritaglio.salva_bmp(fuori + "/" + z.uscita + ".bmp", pal);
    }
    return 0;
}
