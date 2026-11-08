// Le bozze della schermata dei comandi: le righe di «In mare» della tabella dei tasti del manuale, divise nei gruppi della
// tabella, con la stessa funzione del gioco. I tasti sono quelli scritti nella tabella, non risolti dalla mappa.
// Uso: bozze_comandi <dati> <tasti_schermate.toml> <cartella d'uscita>
#include <toml.hpp>

#include <cstdio>
#include <map>
#include <string>
#include <vector>

#include "grafica/font.hpp"
#include "grafica/palette.hpp"
#include "grafica/schermata_comandi.hpp"
#include "grafica/tela.hpp"
#include "grafica/tema_stazioni.hpp"

using namespace m688;

namespace {
std::string tasti_di(const toml::table& r) {
    const auto* v = r["tasti"].as_array() ? r["tasti"].as_array() : r["uso"].as_array();
    std::string s;
    if (!v) return s;
    for (const auto& x : *v) {
        std::string k = x.value_or(std::string{});
        if (const auto d = k.find(':'); d != std::string::npos) k = k.substr(d + 1);
        s += (s.empty() ? "" : ", ") + k;
    }
    return s;
}
}  // namespace

int main(int argc, char** argv) {
    if (argc < 4) {
        std::fprintf(stderr, "uso: %s <dati> <tasti_schermate.toml> <cartella d'uscita>\n", argv[0]);
        return 2;
    }
    const std::string dati = argv[1], fuori = argv[3];
    const Font f = Font::carica(dati + "/font/originale.fnt"), piccolo = Font::carica(dati + "/font/fosforo688_5x7.fnt"),
               targa = Font::carica(dati + "/font/incisione688_7.fnt");
    const auto doc = toml::parse_file(argv[2]);
    DatiSchermataComandi d;
    for (const auto& n : *doc["schermata"].as_array()) {
        const auto& s = *n.as_table();
        if (s["titolo"].value_or(std::string{}) != "In mare") continue;
        // i gruppi della tabella nelle nove schede della partita, dal nome del gruppo
        const std::vector<std::pair<std::string, std::string>> SCHEDE = {
            {"Radio", "F2 RADIO"}, {"Stato", "F3 STATO"}, {"Manovra", "F4 MANOVRA"}, {"Periscopio", "F5 PERISCOPIO"},
            {"Navigazione", "F6 NAVIGAZIONE"}, {"Carta", "F6 NAVIGAZIONE"}, {"Armi", "F7 ARMI"}, {"Sonar", "F8 SONAR"},
            {"Centrale", "F9 CENTRALE"}, {"", "OVUNQUE"}};
        std::map<std::string, SchedaComandi> perche;
        for (const auto& t : *s["tasto"].as_array()) {
            const auto& r = *t.as_table();
            const std::string g = r["gruppo"].value_or(std::string{});
            std::string e;
            for (const auto& [prefisso, etichetta] : SCHEDE)
                if (e.empty() && g.rfind(prefisso, 0) == 0) e = etichetta;
            auto& sc = perche[e];
            sc.etichetta = e;
            if (sc.gruppi.empty() || sc.gruppi.back().titolo != g) sc.gruppi.push_back({g, {}});
            sc.gruppi.back().righe.push_back({tasti_di(r), r["cosa"].value_or(std::string{})});
        }
        for (const char* e : {"F2 RADIO", "F3 STATO", "F4 MANOVRA", "F5 PERISCOPIO", "F6 NAVIGAZIONE", "F7 ARMI", "F8 SONAR", "F9 CENTRALE", "OVUNQUE"})
            d.schede.push_back(perche[e]), d.schede.back().etichetta = e;
    }
    if (d.schede.empty()) return 1;
    d.schede[3].consigli = {"Prima di alzarlo: quota periscopica e velocità sotto i 6 nodi, o la testa non regge.", "Tieni il mare d'occhio: più di due minuti fuori e ti vedono."};
    Palette pal = Palette::carica(dati + "/palette/giorno.pal");
    applica_tema(pal, TemaStazione{});
    Tela t;
    const auto salva = [&](const std::string& nome) { t.salva_bmp(fuori + "/" + nome + ".bmp", pal); };
    const std::size_t piu = 4;   // Navigazione, la piu' piena
    d.scheda = 3;
    d.pagina = 0;
    int pagine = disegna_schermata_comandi(t, f, piccolo, targa, d);
    salva("comandi_periscopio");
    std::printf("periscopio: %d pagine\n", pagine);
    d.scheda = static_cast<int>(piu);
    pagine = disegna_schermata_comandi(t, f, piccolo, targa, d);
    salva("comandi_piena_p1");
    d.pagina = pagine - 1;
    disegna_schermata_comandi(t, f, piccolo, targa, d);
    salva("comandi_piena_ultima");
    std::printf("la piu' piena, %s: %d pagine\n", d.schede[piu].etichetta.c_str(), pagine);
    d.alfa = true;
    d.pagina = 0;
    disegna_schermata_comandi(t, f, piccolo, targa, d);
    salva("comandi_alfa");
    return 0;
}
