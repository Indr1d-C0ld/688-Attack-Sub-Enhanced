#include "schermata_confronto.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>

#include "carta.hpp"
#include "grafica/lamiera.hpp"
#include "grafica/quadro.hpp"
#include "grafica/tema_stazioni.hpp"

namespace m688 {

namespace {

constexpr int PIEDE_Y = 428;   // il piede comune: i due tasti a 450-476, larghi 308 da x 8 e 324
constexpr int RIGA = 10;       // l'altezza di una riga delle tabelle
const char* const LINGUETTE[] = {"1 ESITI", "2 PER PARTE", "3 COPPIE"};
constexpr int PAGINE = 3;

// i testi sul fosforo (font a celle variabili, formato 2), come grafica/quadro
int largo_q(const Font& f, std::string_view s) { return f.larghezza(s, 1, f.formato() == 2 ? f.spazio_predefinito() : 1, f.formato() != 2); }
void scrivi_q(Tela& t, const Font& f, int x, int y, std::string_view s, std::uint8_t c) {
    f.scrivi(t, x, y, s, c, 1, f.formato() == 2 ? f.spazio_predefinito() : 1, f.formato() != 2);
}
std::string maiu(std::string s) {
    for (auto& c : s) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return s;
}
std::string taglia(const Font& f, std::string s, int w) {
    if (largo_q(f, s) <= w) return s;
    while (!s.empty() && largo_q(f, s + "..") > w) s.pop_back();
    return s + "..";
}
std::string pct(double p) {
    char b[16];
    std::snprintf(b, sizeof b, "%.0f", std::fabs(p) < 0.005 ? 0.0 : p * 100.0);   // niente «-0»
    return b;
}
std::string con_segno(const std::string& s) { return s.empty() || s[0] == '-' ? s : "+" + s; }
std::string valore(double v, const std::string& nome) {
    char b[24];
    const double x = std::fabs(v * scala_numero(nome)) < 0.05 ? 0.0 : v * scala_numero(nome);
    std::snprintf(b, sizeof b, std::fabs(x) >= 100.0 ? "%.0f" : "%.1f", x);
    return b;
}
std::string orologio(double s) {
    const int x = static_cast<int>(std::max(0.0, s));
    char b[16];
    std::snprintf(b, sizeof b, "%d:%02d:%02d", x / 3600, x / 60 % 60, x % 60);
    return b;
}

}  // namespace

SchermataConfronto::SchermataConfronto(std::string dati, const std::filesystem::path& confronto)
    : SchermataConfronto(std::move(dati), leggi_confronto(confronto), confronto.parent_path()) {}

SchermataConfronto::SchermataConfronto(std::string dati, DatiConfronto d, std::filesystem::path cartella)
    : dati_(std::move(dati)), d_(std::move(d)), cartella_(std::move(cartella)) {
    try {
        targa_ = Font::carica(dati_ + "/font/incisione688_7.fnt");
        fosforo_ = Font::carica(dati_ + "/font/fosforo688_5x7.fnt");
    } catch (const std::exception&) {
        targa_.reset();
        fosforo_.reset();
    }
}

std::filesystem::path SchermataConfronto::traccia_a(int k) const {
    return k >= 0 && k < static_cast<int>(d_.coppie.size()) ? cartella_ / d_.coppie[static_cast<std::size_t>(k)].traccia_a : std::filesystem::path{};
}
std::filesystem::path SchermataConfronto::traccia_b(int k) const {
    return k >= 0 && k < static_cast<int>(d_.coppie.size()) ? cartella_ / d_.coppie[static_cast<std::size_t>(k)].traccia_b : std::filesystem::path{};
}

std::filesystem::path SchermataConfronto::scrivi_rapporto(const std::vector<std::string>& immagini) const {
    const auto p = cartella_ / "rapporto.md";
    std::ofstream f(p);
    if (!f) return {};
    f << rapporto_confronto(d_, immagini);
    return f ? p : std::filesystem::path{};
}

bool SchermataConfronto::comandi(const Comandi& c, double) {
    coppia_.reset();
    if (c.esci || c.esc || c.invio) return true;
    if (c.tab != 0) pagina_ = (pagina_ + PAGINE + (c.tab > 0 ? 1 : -1)) % PAGINE, scorri_ = 0;
    for (const int k : c.cifre)
        if (k >= 1 && k <= PAGINE) pagina_ = k - 1, scorri_ = 0;
    scorri_ = std::max(0, scorri_ - passi_freccia(c.su_giu, c.passi_su_giu, prec_su_giu_));   // un passo per pressione
    for (const char l : c.lettere)
        if (l == 'E' || l == 'e') {   // il rapporto scritto
            const auto p = scrivi_rapporto();
            avviso_ = p.empty() ? "IL RAPPORTO NON SI E' POTUTO SCRIVERE" : "RAPPORTO SCRITTO: " + p.filename().string();
        }
    for (const auto& [x, y] : c.clic) {
        if (y >= PIEDE_Y + 22 && y < PIEDE_Y + 48) {   // i tasti del piede: TAB la pagina seguente, ESC indietro
            if (x >= 324) return true;
            pagina_ = (pagina_ + 1) % PAGINE, scorri_ = 0;
            continue;
        }
        bool linguetta = false;
        for (int k = 0; k < static_cast<int>(linguette_.size()); ++k) {
            const Rettangolo& r = linguette_[static_cast<std::size_t>(k)];
            if (x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h) pagina_ = k, scorri_ = 0, linguetta = true;
        }
        if (linguetta || pagina_ != 2) continue;
        for (std::size_t k = 0; k < righe_coppie_.size(); ++k) {
            const Rettangolo& r = righe_coppie_[k];
            if (x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h && k < indici_coppie_.size()) {
                const int i = indici_coppie_[k];
                if (i < static_cast<int>(d_.coppie.size()) && !d_.coppie[static_cast<std::size_t>(i)].traccia_a.empty()) {
                    coppia_ = i;
                    return true;
                }
            }
        }
    }
    return false;
}

void SchermataConfronto::palette(Palette& p) const {
    applica_palette_carta(p, StileCarta::vga);
    applica_tema(p, TemaStazione{});   // 16-47: la lamiera e il fosforo della variante C
}

void SchermataConfronto::disegna(Tela& t, const Font& f) const {
    namespace q = quadro;
    const Font& fg = fosforo_ ? *fosforo_ : f;
    const Rettangolo u = q::consolle(t, targa_ ? *targa_ : f, "SALA ANALISI - CONFRONTO A/B");
    const std::string destra = std::to_string(d_.coppie.size()) + " COPPIE, SEMI " + std::to_string(d_.da) + "-" +
                               std::to_string(d_.da + (d_.coppie.empty() ? 0 : d_.coppie.size() - 1));
    const std::string titolo = maiu(d_.titolo.empty() ? d_.missione : d_.titolo);
    const Rettangolo a = q::schermo(t, {u.x, u.y, u.w, PIEDE_Y - 4 - u.y}, fg, taglia(fg, titolo, u.w - 40 - largo_q(fg, destra)), destra);
    int y0 = a.y + 2;
    {
        const std::vector<std::string> nomi(std::begin(LINGUETTE), std::end(LINGUETTE));
        linguette_.clear();
        int x = a.x + 2;   // come quadro::linguette
        for (const auto& n : nomi) {
            const int w = largo_q(fg, n) + 8;
            linguette_.push_back({x, y0, w, fg.righe() + 3});
            x += w + 4;
        }
        y0 += q::linguette(t, a, y0, fg, nomi, pagina_) + 3;
    }
    const int fondo = a.y + a.h - 2;
    righe_coppie_.clear();
    indici_coppie_.clear();
    const auto testo_esito = [&](int k) {
        const auto it = d_.testi_esiti.find(k);
        return it != d_.testi_esiti.end() ? maiu(it->second) : k < 0 ? std::string("NON FINITA") : "ESITO " + std::to_string(k);
    };
    if (pagina_ == 0) {   // gli esiti: le tre colonne, poi la differenza su una scala comune -100..+100
        constexpr int XA = 230, XB = 320, XD = 410, XC = 540;
        int y = y0 + 2;
        y += q::intestazione(t, a, y, fg, "COME SONO FINITE (%, 95%)", {XA, XB, XD, XC}, {"A SENZA", "B CON", "B - A", "SOLO A/B"});
        int k = 0;
        for (const auto& [codice, e] : d_.confronto.esiti) {
            if (y + RIGA > fondo - 90) break;
            const std::string dif = con_segno(pct(e.differenza)) + " (" + pct(e.basso) + ";" + pct(e.alto) + ")";
            q::riga(t, a, y, fg, taglia(fg, testo_esito(codice), XA - 12), {XA, XB, XD, XC},
                    {pct(e.a.p) + " (" + pct(e.a.basso) + "-" + pct(e.a.alto) + ")", pct(e.b.p) + " (" + pct(e.b.basso) + "-" + pct(e.b.alto) + ")",
                     dif, std::to_string(e.solo_a) + "/" + std::to_string(e.solo_b)},
                    k++ % 2 == 1);
            if (e.significativo()) scrivi_q(t, fg, a.x + XD, y, dif, colori::GIALLO);
            y += RIGA;
        }
        // la differenza di ogni esito su una scala comune: lo zero al centro, l'intervallo, il punto
        y += 6;
        y += q::intestazione(t, a, y, fg, "B - A CON L'INTERVALLO (DA -100 A +100)");
        const int x0 = a.x + XA, w = a.w - XA - 16, xz = x0 + w / 2;
        const auto xp = [&](double p) { return x0 + static_cast<int>(std::lround((std::clamp(p, -1.0, 1.0) + 1.0) / 2.0 * w)); };
        const int y_scala = y;
        for (const auto& [codice, e] : d_.confronto.esiti) {
            if (y + RIGA > fondo - 40) break;
            scrivi_q(t, fg, a.x + 4, y, taglia(fg, testo_esito(codice), XA - 12), fosforo(4));
            const std::uint8_t c = e.significativo() ? static_cast<std::uint8_t>(colori::GIALLO) : fosforo(6);
            t.linea_orizzontale(xp(e.basso), xp(e.alto), y + 3, c);
            t.rettangolo_pieno(xp(e.differenza) - 1, y + 1, 3, 5, c);
            y += RIGA;
        }
        t.linea_verticale(xz, y_scala - 2, y, fosforo(3));
        // gli interventi di B e l'ipotesi
        int yi = std::max(y + 6, fondo - 36);
        if (!d_.interventi.empty()) {
            scrivi_q(t, fg, a.x + 4, yi, taglia(fg, "INTERVENTI: " + maiu(d_.interventi.front()) + (d_.interventi.size() > 1 ? " (+" + std::to_string(d_.interventi.size() - 1) + ")" : ""), a.w - 8), fosforo(6));
            yi += RIGA;
        }
        if (!d_.mostra.empty()) scrivi_q(t, fg, a.x + 4, yi, taglia(fg, maiu(d_.mostra), a.w - 8), fosforo(4)), yi += RIGA;
        if (!avviso_.empty()) scrivi_q(t, fg, a.x + 4, yi, avviso_, colori::GIALLO);
    } else if (pagina_ == 1) {   // per parte: le mediane e la differenza bootstrap
        constexpr int XA = 230, XB = 310, XD = 390, XS = 540;
        std::vector<std::pair<std::string, std::vector<std::string>>> righe;   // (etichetta, celle); etichetta vuota: una parte
        std::vector<bool> significative, intestazioni;
        for (const auto& [parte, m] : d_.confronto.parti) {
            righe.push_back({nome_parte_confronto(parte), {}});
            significative.push_back(false), intestazioni.push_back(true);
            for (const auto& [nome, n] : m) {
                if (n.coppie == 0) continue;
                const std::string un = unita_numero(nome);
                righe.push_back({etichetta_numero(nome) + (un.empty() ? "" : " (" + maiu(un) + ")"),
                                 {valore(n.mediana_a, nome), valore(n.mediana_b, nome),
                                  con_segno(valore(n.differenza, nome)) + " (" + valore(n.basso, nome) + ";" + valore(n.alto, nome) + ")",
                                  std::to_string(n.sale) + "/" + std::to_string(n.scende) + "/" + std::to_string(n.uguale)}});
                significative.push_back(significativo(n)), intestazioni.push_back(false);
            }
        }
        int y = y0 + 2;
        y += q::intestazione(t, a, y, fg, "MEDIANE E DIFFERENZA (95%)", {XA, XB, XD, XS}, {"A SENZA", "B CON", "B - A", "SU/GIU/="});
        righe_visibili_ = std::max(1, (fondo - 14 - y) / RIGA);
        const int da = std::min(scorri_, std::max(0, static_cast<int>(righe.size()) - righe_visibili_));
        for (int k = da; k < static_cast<int>(righe.size()) && k < da + righe_visibili_; ++k, y += RIGA) {
            const auto& [et, celle] = righe[static_cast<std::size_t>(k)];
            if (intestazioni[static_cast<std::size_t>(k)]) {
                scrivi_q(t, fg, a.x + 4, y, et, colori::BIANCO);
                continue;
            }
            q::riga(t, a, y, fg, taglia(fg, et, XA - 12), {XA, XB, XD, XS}, celle, (k - da) % 2 == 1);
            if (significative[static_cast<std::size_t>(k)]) scrivi_q(t, fg, a.x + XD, y, celle[2], colori::GIALLO);
        }
        std::string nota = static_cast<int>(righe.size()) > righe_visibili_ ? "FRECCE SU E GIU': SCORRONO.  " : "";
        if (static_cast<int>(d_.coppie.size()) < COPPIE_MINIME)   // il bootstrap con poche coppie: niente giallo
            nota += "POCHE COPPIE: IN GIALLO DA " + std::to_string(COPPIE_MINIME) + " IN SU";
        if (!nota.empty()) scrivi_q(t, fg, a.x + 4, fondo - 10, nota, fosforo(4));
    } else {   // le coppie: il punto di divergenza e il primo evento diverso; un clic apre i due replay
        constexpr int XE = 50, XB = 140, XV = 230, XD = 300, XC = 360, XP = 430;
        int y = y0 + 2;
        y += q::intestazione(t, a, y, fg, "SEME", {XE, XB, XV, XD, XC, XP}, {"ESITO A", "ESITO B", "DIVERGE", "DOPO", "COME", "PRIMO EVENTO DIVERSO"});
        righe_visibili_ = std::max(1, (fondo - 14 - y) / RIGA);
        const int n = static_cast<int>(d_.coppie.size());
        const int da = std::min(scorri_, std::max(0, n - righe_visibili_));
        for (int k = da; k < n && k < da + righe_visibili_; ++k, y += RIGA) {
            const CoppiaConfronto& c = d_.coppie[static_cast<std::size_t>(k)];
            const std::string ev = !c.evento_b.empty() ? c.evento_b : c.evento_a;
            q::riga(t, a, y, fg, std::to_string(c.seme), {XE, XB, XV, XD, XC, XP},
                    {taglia(fg, testo_esito(c.esito_a), XB - XE - 6), taglia(fg, testo_esito(c.esito_b), XV - XB - 6),
                     c.divergono ? orologio(c.t_s) : std::string("MAI"),
                     c.dopo_intervento_s >= 0.0 ? std::to_string(static_cast<int>(c.dopo_intervento_s)) + " S" : std::string("-"),
                     maiu(c.come.empty() ? std::string("-") : c.come), taglia(fg, maiu(ev.empty() ? std::string("-") : ev), a.w - XP - 6)},
                    (k - da) % 2 == 1);
            if (c.esito_a != c.esito_b) scrivi_q(t, fg, a.x + XB, y, taglia(fg, testo_esito(c.esito_b), XV - XB - 6), colori::GIALLO);
            righe_coppie_.push_back({a.x, y - 1, a.w, RIGA});
            indici_coppie_.push_back(k);
        }
        scrivi_q(t, fg, a.x + 4, fondo - 10, "IN GIALLO: L'ESITO CAMBIA.  CLIC SU UNA COPPIA: I DUE REPLAY AL PUNTO DI DIVERGENZA", fosforo(4));
    }
    lamiera::piede(t, f, PIEDE_Y, "TAB PAGINA   FRECCE SCORRONO   CLIC SU UNA COPPIA: I DUE REPLAY   E: IL RAPPORTO",
                   {{"TAB  PAGINA SEGUENTE", ColoreLampada::ambra}, {"ESC  INDIETRO", ColoreLampada::verde}}, 1);
}

}  // namespace m688
