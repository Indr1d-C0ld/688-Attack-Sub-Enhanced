// Le tavole del libretto di riconoscimento (docs/25_libretto_riconoscimento.md): per ogni classe di
// data/motore/libretto.toml la tavola grande in stile ONI (le quattro viste a tutta pagina, coi dati) e la pagina del
// libretto (grafica/libretto, 210x340) come si vede nel terzo destro della vista. Stampa l'altezza vera del modello
// accanto a quella dell'intelligence, per il controllo.
// Uso: libretto_tavole <dati> <cartella d'uscita> [classe...]
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

#include <toml.hpp>

#include "grafica/font.hpp"
#include "grafica/libretto.hpp"
#include "grafica/modello.hpp"
#include "grafica/palette.hpp"
#include "grafica/tela.hpp"

using namespace m688;
namespace lb = m688::libretto;

namespace {

int largo(const Font& f, const std::string& s) { return f.larghezza(s, 1, f.formato() == 2 ? f.spazio_predefinito() : 1, f.formato() != 2); }
void scrivi(Tela& t, const Font& f, int x, int y, const std::string& s, std::uint8_t c) {
    f.scrivi(t, x, y, s, c, 1, f.formato() == 2 ? f.spazio_predefinito() : 1, f.formato() != 2);
}

void incolla(Tela& t, const Sprite& s, int cx, int base_y) {
    const int x0 = cx - s.w / 2, y0 = base_y - static_cast<int>(std::lround(s.ancora_y));
    for (int y = 0; y < s.h; ++y)
        for (int x = 0; x < s.w; ++x) {
            const std::uint8_t c = s.pix[static_cast<std::size_t>(y * s.w + x)];
            if (c != TRASPARENTE) t.pixel(x0 + x, y0 + y, c);
        }
}

Sprite adatta(const Modello& m, double aob, double scala, int largo_max, const std::array<double, 15>& rampa, bool vela);

// la tavola grande: carta, testata, il profilo a tutta larghezza, poi prua, 45 e 135, i dati sotto
void tavola(Tela& t, const Font& f, const Font& p, const lb::Voce& v, const Modello& m, const std::array<double, 15>& rampa) {
    t.riempi(15);
    for (int k = 0; k < 2; ++k) {   // il doppio filo della tavola
        const int d = 8 + 3 * k;
        t.linea_orizzontale(d, 639 - d, d, 0), t.linea_orizzontale(d, 639 - d, 479 - d, 0);
        t.linea_verticale(d, d, 479 - d, 0), t.linea_verticale(639 - d, d, 479 - d, 0);
    }
    scrivi(t, p, 20, 20, "OFFICE OF NAVAL INTELLIGENCE - RECOGNITION PLATE", 0);
    scrivi(t, p, 620 - largo(p, v.classifica), 20, v.classifica, 4);
    scrivi(t, f, 20, 34, v.nome, 0);
    scrivi(t, p, 20, 46, v.sottotitolo + "   " + v.sezione, 0);
    const double scala = std::min(580.0 / std::max<double>(m.lunghezza_m(), 1.0), 130.0 / std::max<double>(m.quota_max_m(), 1.0));
    const Sprite profilo = adatta(m, 90.0, scala, 590, rampa, v.sottomarino);
    const int base1 = 70 + static_cast<int>(m.quota_max_m() * scala) + 8;
    incolla(t, profilo, 320, base1);
    for (int x = 24; x < 616; ++x) if (x % 3) t.pixel(x, base1, 8);
    scrivi(t, p, 24, base1 + 4, "PROFILO, DRITTA (90 GR.)", 8);
    const double s2 = v.sottomarino ? scala * 0.6 : std::min(scala * 0.6, 180.0 / (0.8 * std::max<double>(m.lunghezza_m(), 1.0)));
    const int base2 = base1 + 16 + static_cast<int>(m.quota_max_m() * s2) + 8;
    const struct { double aob; const char* nome; int cx; } viste[] = {{0, "PRUA (0 GR.)", 110}, {45, "TRE QUARTI (45 GR.)", 320}, {135, "TRE QUARTI (135 GR.)", 530}};
    for (const auto& x : viste) {
        incolla(t, adatta(m, x.aob, s2, 190, rampa, v.sottomarino), x.cx, base2);
        for (int k = x.cx - 95; k < x.cx + 95; ++k) if (k % 3) t.pixel(k, base2, 8);
        scrivi(t, p, x.cx - largo(p, x.nome) / 2, base2 + 4, x.nome, 8);
    }
    int y = base2 + 22;
    t.linea_orizzontale(20, 619, y, 0);
    y += 6;
    char riga[160];
    std::snprintf(riga, sizeof riga, "LUNGHEZZA %.1f M    ALBERO %.0f M +- %.1f (INTELLIGENCE 1989)", v.lunghezza_m, v.albero_m, v.albero_sigma_m);
    scrivi(t, f, 20, y, riga, 0);
    y += f.righe() + 6;
    const auto blocco = [&](const std::string& et, const std::vector<std::string>& v2) {
        std::string s;
        for (const auto& x : v2) s += (s.empty() ? "" : ", ") + x;
        if (s.empty()) return;
        scrivi(t, p, 20, y, et, 0);
        int yy = y;
        std::string r;
        for (std::size_t i = 0; i <= s.size(); ++i) {   // a capo semplice sulla virgola o sullo spazio
            const char c = i < s.size() ? s[i] : ' ';
            if (c == ' ' && largo(p, r) > 490) { scrivi(t, p, 96, yy, r, 0); yy += p.righe() + 2; r.clear(); continue; }
            if (i < s.size()) r += c;
        }
        if (!r.empty()) scrivi(t, p, 96, yy, r, 0), yy += p.righe() + 2;
        y = yy + 3;
    };
    blocco("SEGNI", {v.segni});
    blocco("ARMI", v.armi);
    blocco("SENSORI", v.sensori);
    blocco("RADAR", v.emettitori);
}

// la sagoma alla scala data, ridotta se e' piu' larga di `largo_max` (le vele ritagliate dei sottomarini)
Sprite adatta(const Modello& m, double aob, double scala, int largo_max, const std::array<double, 15>& rampa, bool vela) {
    Sprite s = lb::sagoma(m, aob, scala, rampa, vela);
    if (s.w > largo_max) s = lb::sagoma(m, aob, scala * largo_max / s.w, rampa, vela);
    return s;
}

}  // namespace

int main(int argc, char** argv) {
    if (argc < 3) {
        std::fprintf(stderr, "uso: %s <dati> <cartella d'uscita> [classe...]\n", argv[0]);
        return 2;
    }
    const std::string dati = argv[1], fuori = argv[2];
    const Palette giorno = Palette::carica(dati + "/palette/giorno.pal");
    const auto rampa = rampa_grigi(giorno);
    const Font f = Font::carica(dati + "/font/originale.fnt"), p = Font::carica(dati + "/font/fosforo688_5x7.fnt");
    const std::vector<lb::Voce> voci = lb::carica(dati + "/libretto.toml");
    const toml::table classi = toml::parse_file(dati + "/classi.toml");
    std::vector<std::string> scelte(argv + 3, argv + argc);
    const int totale = static_cast<int>(voci.size());
    // la bozza del filtro: le fregate del Patto con i lanciatori, il cursore sulla riga dei lanciatori
    const lb::Filtro esempio{"patto", "fregata", {"lanciatori"}};
    const std::vector<int> passano = lb::filtra(voci, esempio);
    for (const lb::Voce& v : voci) {
        const std::string& id = v.classe;
        const int n = v.ordine;
        if (!scelte.empty() && std::find(scelte.begin(), scelte.end(), id) == scelte.end()) continue;
        const auto mod = classi[id]["modello"].value<std::string>();
        if (!mod) { std::fprintf(stderr, "%s: senza modello, saltata\n", id.c_str()); continue; }
        const Modello m = Modello::carica(dati + "/" + *mod);
        std::printf("%s: albero del modello %.1f m, del libretto %.0f +- %.1f\n", id.c_str(), m.quota_max_m(), v.albero_m, v.albero_sigma_m);
        Tela grande(640, 480);
        tavola(grande, f, p, v, m, rampa);
        grande.salva_bmp(fuori + "/tavola_" + id + ".bmp", giorno);
        // la pagina, alla scala che fa stare la nave nel profilo (196 px) e nelle viste piccole (62 px)
        // i sottomarini: la scala la decide la vela, che e' tutto quello che si vede
        const double sp = v.sottomarino ? 38.0 / std::max<double>(m.quota_max_m(), 1.0)
                                        : std::min(194.0 / std::max<double>(m.lunghezza_m(), 1.0), 38.0 / std::max<double>(m.quota_max_m(), 1.0));
        const double sv = std::min(sp, 28.0 / std::max<double>(m.quota_max_m(), 1.0));
        const double s3 = v.sottomarino ? sv * 0.8 : std::min(sv, 62.0 / (0.78 * std::max<double>(m.lunghezza_m(), 1.0)));   // a 45 gradi la nave e' lunga 0,7-0,8 L
        const Sprite a = adatta(m, 90.0, sp, 194, rampa, v.sottomarino), b = adatta(m, 0.0, sv, 62, rampa, v.sottomarino),
                     c = adatta(m, 45.0, s3, 62, rampa, v.sottomarino), d = adatta(m, 135.0, s3, 62, rampa, v.sottomarino);
        Tela pag(640, 480);
        pag.riempi(TRASPARENTE);
        if (!lb::pagina(pag, {422, 8, 210, 340}, f, p, v, {&a, &b, &c, &d}, n, totale))
            std::printf("%s: IL TESTO NON ENTRA NELLA PAGINA, accorciare segni, armi o radar\n", id.c_str());
        pag.salva_bmp(fuori + "/pagina_" + id + ".bmp", giorno);
        const auto k = std::find(passano.begin(), passano.end(), static_cast<int>(&v - voci.data()));
        if (k == passano.begin()) {   // la prima tavola che passa, con la striscia nella testata
            pag.riempi(TRASPARENTE);
            lb::pagina(pag, {422, 8, 210, 340}, f, p, v, {&a, &b, &c, &d}, 1, static_cast<int>(passano.size()), &esempio);
            pag.salva_bmp(fuori + "/pagina_filtrata.bmp", giorno);
        }
    }
    Tela scheda(640, 480);
    scheda.riempi(TRASPARENTE);
    lb::scheda_filtro(scheda, {422, 8, 210, 340}, f, p, voci, esempio, 4, "SU E GIU' LA RIGA, SINISTRA E DESTRA LA SCELTA, SPAZIO SEGNA, ESC CHIUDE");
    scheda.salva_bmp(fuori + "/pagina_filtro.bmp", giorno);
    // la scheda che scorre: senza filtro, il cursore sulle etichette dei sottomarini in fondo all'elenco
    scheda.riempi(TRASPARENTE);
    lb::scheda_filtro(scheda, {422, 8, 210, 340}, f, p, voci, {"", "sottomarino", {"vela_prua"}}, lb::righe_filtro(voci) - 2,
                      "SU E GIU' LA RIGA, SINISTRA E DESTRA LA SCELTA, SPAZIO SEGNA, ESC CHIUDE");
    scheda.salva_bmp(fuori + "/pagina_filtro_scorre.bmp", giorno);
    // la tavola «luci» (docs/27): gli schemi del COLREG dalle configurazioni di luci.toml
    scheda.riempi(TRASPARENTE);
    if (!lb::pagina_luci(scheda, {422, 8, 210, 340}, f, p, luci::carica(dati + "/luci.toml"), totale + 1, totale + 1))
        std::printf("pagina luci: manca una classe degli schemi\n");
    scheda.salva_bmp(fuori + "/pagina_luci.bmp", giorno);
    scheda.riempi(TRASPARENTE);
    if (!lb::pagina_luci_aob(scheda, {422, 8, 210, 340}, f, p, luci::carica(dati + "/luci.toml"), totale + 2, totale + 2))
        std::printf("pagina AOB dalle luci: manca il mercantile\n");
    scheda.salva_bmp(fuori + "/pagina_luci_aob.bmp", giorno);
    return 0;
}
