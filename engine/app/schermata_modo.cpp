#include "schermata_modo.hpp"

#include <string>
#include <vector>

#include "grafica/lamiera.hpp"
#include "grafica/tema_stazioni.hpp"

namespace m688 {

namespace {
constexpr int PIEDE_Y = 392, PIEDE_TASTI_Y = PIEDE_Y + 22, PIEDE_TASTI_H = 26;
constexpr int PANNELLO_Y = 62, PANNELLO_H = 260, PANNELLO_W = 306, CLASSICO_X = 8, ENHANCED_X = 326;

Rettangolo pannello_di(bool classico) { return {classico ? CLASSICO_X : ENHANCED_X, PANNELLO_Y, PANNELLO_W, PANNELLO_H}; }
bool dentro(const Rettangolo& r, int x, int y) { return x >= r.x && y >= r.y && x < r.x + r.w && y < r.y + r.h; }
}  // namespace

std::optional<bool> SchermataModo::comandi(const Comandi& c, bool& esci) {
    esci = c.esc || c.esci;
    if (const int lato = passi_freccia(c.ruota, c.passi_ruota, prec_ruota_)) classico_ = lato < 0;
    for (const int k : c.cifre) {
        if (k == 1) classico_ = true;
        if (k == 2) classico_ = false;
    }
    bool conferma = c.invio;
    for (const auto& [x, y] : c.clic) {
        for (const bool classico : {true, false})
            if (dentro(pannello_di(classico), x, y)) {   // sul pannello scelto, conferma; sull'altro, lo sceglie
                conferma = conferma || classico == classico_;
                classico_ = classico;
            }
        if (y >= PIEDE_TASTI_Y && y < PIEDE_TASTI_Y + PIEDE_TASTI_H) {
            if (x >= 320) conferma = true;
            else esci = true;
        }
    }
    if (conferma) return classico_;
    return std::nullopt;
}

void SchermataModo::disegna(Tela& t, const Font& f) const {
    namespace la = lamiera;
    la::lamiera(t, {0, 0, LARGHEZZA_TELA, ALTEZZA_TELA}, 7);
    la::targa(t, f, LARGHEZZA_TELA / 2, 6, "688 ATTACK SUB");
    la::principale(t, f, LARGHEZZA_TELA / 2 - f.larghezza("COME VUOI GIOCARE?", 1) / 2, 38, "COME VUOI GIOCARE?");
    const auto scheda = [&](bool classico, const char* titolo, const std::vector<const char*>& righe) {
        const Rettangolo r = pannello_di(classico);
        const bool scelto = classico == classico_;
        la::pannello(t, r, 8);
        la::vetro(t, {r.x + 6, r.y + 6, r.w - 12, r.h - 12});
        if (scelto) la::smusso(t, {r.x - 2, r.y - 2, r.w + 4, r.h + 4}, lampada(ColoreLampada::ambra, true), lampada(ColoreLampada::ambra, true), 2);
        la::spia_voce(t, {r.x + 14, r.y + 14, 10, 10}, ColoreLampada::ambra, scelto);
        f.scrivi(t, r.x + 32, r.y + 14, std::string(classico ? "1  " : "2  ") + titolo, scelto ? colori::GIALLO : colori::BIANCO, 1);
        int y = r.y + 36;
        for (const char* riga : righe) {
            for (const std::string& s : la::a_capo(f, riga, r.w - 32)) {
                f.scrivi(t, r.x + 16, y, s, scelto ? colori::BIANCO : colori::GRIGIO, 1);
                y += f.righe() + 3;
            }
            y += 8;
        }
    };
    scheda(true, "CLASSICO",
           {"IL GIOCO DEL 1989, COM'ERA.", "LE MISSIONI ORIGINALI CON LE LORO REGOLE E I LORO SCHERMI, CON IL 688 O CON L'ALFA.",
            "SI VA SUBITO IN MARE: NIENTE BRIEFING, NIENTE CARRIERA."});
    scheda(false, "ENHANCED",
           {"IL REMAKE: LE STESSE MISSIONI E MOLTE ALTRE.", "SONAR, ACUSTICA, METEO E IA MODELLATI SUL VERO.",
            "CARRIERA CON L'ALBO DEI COMANDANTI, US NAVY O VMF, BRIEFING E DEBRIEFING.",
            "MISSIONI RAPIDE, EDITOR, VETRINO DELL'IA."});
    la::piede(t, f, PIEDE_Y, "SI CAMBIA QUANDO VUOI NEL MENU DELLE MISSIONI, COL TASTO O. LA SCELTA RESTA.",
              {{"ESC  ESCI", ColoreLampada::rossa}, {"INVIO  VAI", ColoreLampada::verde}}, 1);
}

void SchermataModo::palette(Palette& p) const { applica_tema(p, TemaStazione{}); }

}  // namespace m688
