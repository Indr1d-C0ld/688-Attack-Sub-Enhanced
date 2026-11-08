// Tavole di controllo dell'enciclopedia (docs/00_ROADMAP.md, requisito 14; tools/pipeline/enciclopedia.py): per una
// voce di data/motore/enciclopedia.toml una pagina 640x480 con le immagini in gioco (sprite per angolo ed elevazione,
// modello 3D in posa, sagoma dell'arma, vista dall'oculare, effetti in mare), la scheda tecnica, il testo e i
// riferimenti. Le foto digitalizzate (.img con la loro palette) le affianca tools/pipeline/tavole_enciclopedia.py.
// Uso: anteprima_enciclopedia <dati> <id> <uscita.bmp>
//      anteprima_enciclopedia <dati> --manuale <cartella>   le prime due immagini in gioco di ogni voce, una per file
//      (<cartella>/<id>_<k>.bmp, il disegno nel riquadro 0,0-380x240), per le schede del volume 3 del manuale

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <string>
#include <vector>

#include <toml.hpp>

#include "carta.hpp"
#include "grafica/cascata_firma.hpp"
#include "grafica/effetti.hpp"
#include "grafica/modello.hpp"
#include "grafica/oculare.hpp"
#include "grafica/periscopio.hpp"
#include "grafica/sagome_armi.hpp"
#include "grafica/spaccato.hpp"
#include "nucleo/acustica.hpp"
#include "nucleo/carico.hpp"
#include "risorse.hpp"

using namespace m688;

namespace {

constexpr std::uint8_t FONDO = colori::NERO, RIQUADRO = colori::BLU, TESTO = colori::BIANCO, ETICHETTA = colori::GRIGIO;

// testo a capo in una larghezza; ritorna la y dopo l'ultima riga
int paragrafo(Tela& t, const Font& f, int x, int y, int w, const std::string& testo, std::uint8_t col, int max_righe = 99) {
    std::string riga, parola;
    int righe = 0;
    auto emetti = [&]() {
        if (righe < max_righe) f.scrivi(t, x, y, riga, col, 1);
        y += 10;
        ++righe;
        riga.clear();
    };
    for (std::size_t i = 0; i <= testo.size(); ++i) {
        const char c = i < testo.size() ? testo[i] : ' ';
        if (c != ' ') { parola += c; continue; }
        if (parola.empty()) continue;
        const std::string prova = riga.empty() ? parola : riga + " " + parola;
        if (f.larghezza(prova, 1) > w && !riga.empty()) { emetti(); riga = parola; }
        else riga = prova;
        parola.clear();
    }
    if (!riga.empty()) emetti();
    return std::min(y, y);
}

// copia una vista 640x360 rimpicciolita (pixel piu' vicino) in un riquadro
void rimpicciolisci(Tela& t, const Tela& vista, const Rettangolo& sorgente, const Rettangolo& r) {
    for (int y = 0; y < r.h; ++y)
        for (int x = 0; x < r.w; ++x)
            t.pixel(r.x + x, r.y + y, vista.at(sorgente.x + x * sorgente.w / r.w, sorgente.y + y * sorgente.h / r.h));
}

std::string stem(const std::string& f) { return std::filesystem::path(f).stem().string(); }

}  // namespace

int main(int argc, char** argv) {
    if (argc < 4) {
        std::fprintf(stderr, "uso: anteprima_enciclopedia <dati> <id> <uscita.bmp>\n"
                             "     anteprima_enciclopedia <dati> --manuale <cartella>\n");
        return 1;
    }
    const std::string dati = argv[1], id = argv[2], uscita = argv[3];
    const auto ris = carica_risorse(dati);
    const auto doc = toml::parse_file(dati + "/enciclopedia.toml");
    const CatalogoCarico carico = carica_carico(dati + "/carico.toml");
    const auto periscopi = toml::parse_file(dati + "/periscopi.toml");
    const auto rampa = rampa_grigi(ris.giorno);
    // ordine dei riquadri: il primo sprite, il modello, gli altri tipi, poi gli altri sprite (le foto le affianca lo script)
    auto scegli = [](const toml::table& v) {
        std::vector<const toml::table*> scelte;
        if (const auto* imm = v["immagini"].as_array()) {
        auto tipo_di = [](const toml::node& n) { return n.as_table()->get("tipo")->value<std::string>().value_or(""); };
        std::vector<const toml::table*> sprite, altri;
        const toml::table* modello = nullptr;
        for (const auto& n : *imm) {
            const std::string tp = tipo_di(n);
            if (tp == "sprite") sprite.push_back(n.as_table());
            else if (tp == "modello") modello = n.as_table();
            else if (tp != "foto") altri.push_back(n.as_table());
        }
        if (!sprite.empty()) scelte.push_back(sprite[0]);
        if (modello) scelte.push_back(modello);
        scelte.insert(scelte.end(), altri.begin(), altri.end());
        if (sprite.size() > 1) scelte.insert(scelte.end(), sprite.begin() + 1, sprite.end());
        }
        return scelte;
    };
    // un'immagine in gioco nel riquadro r (la palette cambia per cascate, carte e spaccati)
    auto disegna = [&](Tela& t, Palette& pal, const toml::table& im, const Rettangolo r) {
            const std::string tipo = im["tipo"].value<std::string>().value_or("");
            t.rettangolo_pieno(r.x, r.y, r.w, r.h, RIQUADRO);
            t.imposta_ritaglio(r);
            const double aob = im["aob"].value<double>().value_or(90.0), el = im["elevazione"].value<double>().value_or(0.0);
            const bool dritta = im["dritta"].value<bool>().value_or(true);
            if (tipo == "sprite") {
                const auto is = ris.sprite.find(stem(im["file"].value<std::string>().value_or("")));
                if (is != ris.sprite.end()) {
                    const Sprite& s = is->second.per_angolo(aob, dritta, el);
                    const double scala = std::min((r.w - 12.0) / s.w, (r.h - 12.0) / s.h);
                    t.disegna_sprite(s, r.x + r.w / 2.0 + (s.ancora_x - s.w / 2.0) * scala, r.y + r.h / 2.0 + (s.ancora_y - s.h / 2.0) * scala, scala);
                }
            } else if (tipo == "modello") {
                const std::string c = stem(im["file"].value<std::string>().value_or(""));
                const auto im3 = ris.modelli.find(c);
                const auto cl = ris.classi.find(c);
                if (im3 != ris.modelli.end() && cl != ris.classi.end()) {
                    const Sprite s = genera_sprite(im3->second, PosaNave{}, aob, dritta, rampa, (r.w - 16.0) / cl->second.lunghezza_m);
                    const double scala = std::min({1.0, (r.w - 8.0) / s.w, (r.h - 8.0) / s.h});
                    t.disegna_sprite(s, r.x + r.w / 2.0 + (s.ancora_x - s.w / 2.0) * scala, r.y + r.h / 2.0 + (s.ancora_y - s.h / 2.0) * scala, scala);
                }
            } else if (tipo == "sagoma") {
                const std::string arma = im["arma"].value<std::string>().value_or("");
                const auto a = carico.armi.find(arma);
                if (a != carico.armi.end()) disegna_sagoma_arma(t, sagoma_da_scheda(a->second), Rettangolo{r.x + 6, r.y + 20, r.w - 12, r.h - 40});
                else if (sagoma_nota(arma)) disegna_sagoma_arma(t, sagoma_da_id(arma), Rettangolo{r.x + 6, r.y + 20, r.w - 12, r.h - 40});   // come il Museo
            } else if (tipo == "oculare" || tipo == "effetto") {
                // una scena di prova: una Perry a 3 km (oculare); la boa o il marcatore a 300 m (effetti)
                Periscopio per(ris.font, ris.classi, ris.sprite);
                per.usa_modelli(ris.modelli, rampa);
                per.usa_palette(ris.giorno);
                Mondo m;
                Nave occhio;
                occhio.id = -1;
                VistaPeriscopio w;
                w.quota_occhio_m = 1.5;
                w.tempo_s = 30.0;
                Effetti e(3);
                if (tipo == "oculare") {
                    Nave n;
                    n.id = 1; n.classe = "perry"; n.pos = {3000.0 * std::sin(radianti(1.0)), 3000.0 * std::cos(radianti(1.0))};
                    n.pos_prec = n.pos; n.rotta = n.rotta_prec = 300.0;
                    m.aggiungi(n);
                    const auto& p = *periscopi["periscopi"][im["periscopio"].value<std::string>().value_or("")].as_table();
                    const int ingr = static_cast<int>(im["ingrandimento"].value<std::int64_t>().value_or(0));
                    w.oculare.attivo = true;
                    w.oculare.campo_gradi = (*p["campi_gradi"].as_array())[static_cast<std::size_t>(ingr)].value<double>().value_or(8.0);
                    w.oculare.passo_gradi = (*p["reticolo_gradi"].as_array())[static_cast<std::size_t>(ingr)].value<double>().value_or(1.0);
                    w.oculare.passo_fine_gradi = (*p["reticolo_fine_gradi"].as_array())[static_cast<std::size_t>(ingr)].value<double>().value_or(0.25);
                    const std::string ret = p["reticolo"].value<std::string>().value_or("americano");
                    w.oculare.reticolo = ret == "sovietico" ? Reticolo::sovietico : ret == "britannico" ? Reticolo::britannico : Reticolo::americano;
                    const std::string modo = im["modo"].value<std::string>().value_or("giorno");
                    w.oculare.modo = modo == "bassa_luminosita" ? ModoOculare::bassa_luminosita : modo == "intensificatore" ? ModoOculare::intensificatore
                                     : modo == "tv" ? ModoOculare::tv : modo == "termico" ? ModoOculare::termico : ModoOculare::giorno;
                    if (w.oculare.modo != ModoOculare::giorno) w.luce = 0.05;
                    w.rilevamento = 1.0;
                    oculare::palette(pal, w.oculare);
                } else {
                    const std::string ef = im["effetto"].value<std::string>().value_or(""), og = im["oggetto"].value<std::string>().value_or("");
                    for (double tt = 0; tt < 70.0; tt += 0.1) {
                        if (ef == "boa") {
                            Sonoboa b;
                            b.id = 1; b.tipo = og; b.pos = {0.0, 0.0}; b.t_posa = 2.0; b.t_impatto = 5.0; b.t_attiva = 125.0; b.t_fine = 1e9;
                            e.boe_asw({b}, tt);
                        } else if (tt == 0.0) {
                            e.marcatore_asw(og == "mk58" ? MarcatoreAsw::mk58 : og == "omab" ? MarcatoreAsw::omab_giorno : MarcatoreAsw::mk25, {0, 0}, 900);
                        }
                        e.avanza(0.1, m);
                    }
                    occhio.pos = {0.0, -300.0};
                    w.alto_ingrandimento = true;
                    w.rilevamento = 0.0;
                    w.tempo_s = 70.0;
                }
                static Tela vista;
                vista.riempi(0);
                per.disegna(vista, m, occhio, w, tipo == "effetto" ? &e : nullptr);
                const Rettangolo sorg = tipo == "oculare" ? oculare::area(w.oculare) : Rettangolo{160, 110, 320, 200};
                rimpicciolisci(t, vista, sorg, r);
            } else if (tipo == "cascata") {   // le tre palette usano 96-143 e 199-247: vale quella dell'ultima immagine
                static const CatalogoAcustico acu = carica_acustica(dati + "/acustica.toml");
                applica_palette_cascata(pal);
                disegna_cascata_firma(t, r, acu, im["classe"].value<std::string>().value_or(""), im["velocita_kn"].value<double>().value_or(0.0),
                                      0x688u, im["sensore"].value<std::string>().value_or(""), &ris.font);
            } else if (tipo == "carta") {
                try {
                    const Fondale carta = Fondale::carica(dati + "/" + im["file"].value<std::string>().value_or(""));
                    const auto lat = im["lat"].value<double>(), lon = im["lon"].value<double>();
                    const Geografiche g{lat.value_or(0.0), lon.value_or(0.0)};
                    applica_palette_carta(pal, StileCarta::vga);
                    disegna_carta_museo(t, ris.font, r, carta, lat && lon ? &g : nullptr);
                } catch (const std::exception& e) {
                    ris.font.scrivi(t, r.x + 8, r.y + r.h / 2 - 4, e.what(), ETICHETTA, 1);
                }
            } else if (tipo == "spaccato") {
                const std::string cl = im["classe"].value<std::string>().value_or("");
                try {
                    const Spaccato sp(cl, Modello::carica(dati + "/modelli/" + cl + ".mdl"), DatiScafo::carica(dati + "/scafi/" + cl + ".toml"));
                    Spaccato::imposta_palette(pal);
                    t.rettangolo_pieno(r.x, r.y, r.w, r.h, colori::NERO);
                    disegna_spaccato_sistema(t, r, sp, im["sistema"].value<std::string>().value_or(""));
                } catch (const std::exception& e) {
                    ris.font.scrivi(t, r.x + 8, r.y + r.h / 2 - 4, e.what(), ETICHETTA, 1);
                }
            } else {
                ris.font.scrivi(t, r.x + 8, r.y + r.h / 2 - 4, "[" + tipo + "]", ETICHETTA, 1);
            }
            t.togli_ritaglio();
    };
    if (id == "--manuale") {   // tutte le voci: le prime due immagini in gioco, un file ciascuna
        std::filesystem::create_directories(uscita);
        int n = 0;
        for (const auto& [chiave, nodo] : *doc["voci"].as_table()) {
            int k = 0;
            for (const toml::table* pim : scegli(*nodo.as_table())) {
                if (k >= 2) break;
                Palette pal = ris.giorno;
                Tela t;
                t.riempi(FONDO);
                disegna(t, pal, *pim, Rettangolo{0, 0, 380, 240});
                t.salva_bmp(uscita + "/" + std::string(chiave.str()) + "_" + std::to_string(k) + ".bmp", pal);
                ++k;
                ++n;
            }
        }
        std::printf("%d immagini in %s\n", n, uscita.c_str());
        return 0;
    }
    const auto* vp = doc["voci"][id].as_table();
    if (!vp) {
        std::fprintf(stderr, "voce sconosciuta: %s\n", id.c_str());
        return 1;
    }
    const auto& v = *vp;
    auto str = [](const toml::node_view<const toml::node>& n) { return n.value<std::string>().value_or(""); };
    Palette pal = ris.giorno;
    Tela t;
    t.riempi(FONDO);
    ris.font.scrivi(t, 8, 6, str(v["nome"]), TESTO, 2);
    ris.font.scrivi(t, 8, 26, str(v["gruppo"]) + " - " + str(v["nazione"]) + " - " + id, ETICHETTA, 1);
    // immagini in gioco: quattro riquadri 2x2
    int k = 0;
    for (const toml::table* pim : scegli(v)) {
        if (k >= 4) break;
        const Rettangolo r{8 + (k % 2) * 196, 40 + (k / 2) * 134, 190, 120};
        disegna(t, pal, *pim, r);
        paragrafo(t, ris.font, r.x + 2, r.y + r.h + 2, r.w, (*pim)["didascalia_it"].value<std::string>().value_or(""), ETICHETTA, 1);
        ++k;
    }

    // scheda tecnica a destra
    int y = 40;
    ris.font.scrivi(t, 404, y, "SCHEDA TECNICA", TESTO, 1);
    y += 12;
    if (const auto* sc = v["scheda"].as_array())
        for (const auto& nodo : *sc) {
            const auto& r = *nodo.as_table();
            if (y > 290) break;
            const std::string conf = r["conf"].value<std::string>().value_or("");
            const std::string riga = r["nome_it"].value<std::string>().value_or("") + ": " + r["testo_it"].value<std::string>().value_or("") +
                                     (conf == "STIMA" ? " (stima)" : "");
            y = paragrafo(t, ris.font, 404, y, 228, riga, conf == "STIMA" ? ETICHETTA : TESTO, 3);
            const std::string gioco = r["gioco_testo_it"].value<std::string>().value_or("");
            if (!gioco.empty() && y <= 290) y = paragrafo(t, ris.font, 412, y, 220, "(" + gioco + ")", ETICHETTA, 2);
        }
    // testo, uso, riferimenti in basso
    y = 312;
    y = paragrafo(t, ris.font, 8, y, 624, str(v["testo_it"]), TESTO, 7);
    const std::string uso = str(v["uso_it"]);
    if (!uso.empty()) y = paragrafo(t, ris.font, 8, y + 2, 624, "IN GIOCO: " + uso, colori::GIALLO, 5);
    std::string rif;
    if (const auto* rr = v["riferimenti"].as_array())
        for (const auto& nodo : *rr) {
            const auto& r = *nodo.as_table();
            rif += (rif.empty() ? "" : ", ") + r["relazione"].value<std::string>().value_or("") + ": " + r["voce"].value<std::string>().value_or("");
        }
    if (!rif.empty() && y < 460) y = paragrafo(t, ris.font, 8, y + 2, 624, "VEDI: " + rif, colori::CIANO_CHIARO, 2);
    std::string crediti;
    if (const auto* cc = v["crediti"].as_array())
        crediti = "FOTO: " + std::to_string(cc->size()) + " crediti (" + (*cc)[0].as_table()->get("licenza")->value<std::string>().value_or("") + "...)";
    if (!crediti.empty() && y < 470) paragrafo(t, ris.font, 8, y + 2, 624, crediti, ETICHETTA, 1);
    t.salva_bmp(uscita, pal);
    return 0;
}
