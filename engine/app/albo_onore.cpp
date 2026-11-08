#include "albo_onore.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>

#include "grafica/cerimonia.hpp"
#include "grafica/cornici.hpp"

namespace m688 {

using namespace cerimonia;

namespace {

constexpr auto INCHIOSTRO = static_cast<std::uint8_t>(NERO + 1), TENUE = static_cast<std::uint8_t>(NERO + 3),
               ROSSO = static_cast<std::uint8_t>(TIMBRO + 2), CHIARO = static_cast<std::uint8_t>(BIANCO + 4),
               DORATO = static_cast<std::uint8_t>(ORO + 6), RIGATURA = static_cast<std::uint8_t>(CARTA + 2);
constexpr int PIEDE_Y = 452, RIGA = 11;
constexpr Rettangolo FOGLIO{18, 62, 604, 384};   // il registro aperto
constexpr const char* SCHEDE[3] = {"MIGLIORI MISSIONI", "MIGLIORI CARRIERE", "COMANDANTI STORICI"};

// in maiuscolo col font del gioco: il cirillico (le sigle dei gradi della VMF) traslitterato, le accentate italiane
// nelle maiuscole accentate del font, virgolette e trattini tipografici in quelli semplici, il paragrafo in «PAR.»
// (come testo_per_font del Museo, per i testi dell'albo)
std::string maiuscolo(const std::string& s) {
    static const std::pair<const char*, const char*> SOST[] = {
        {"\u00e0", "\u00c0"}, {"\u00e8", "\u00c8"}, {"\u00e9", "\u00c9"}, {"\u00ec", "\u00cc"}, {"\u00f2", "\u00d2"},
        {"\u00f9", "\u00d9"}, {"\u00ab", "\""}, {"\u00bb", "\""}, {"\u2019", "'"}, {"\u2018", "'"}, {"\u2013", "-"},
        {"\u2014", "-"}, {"\u00a7", "PAR."}, {"\u00b0", " GRADI"}, {"\u010d", "C"}, {"\u010c", "C"}, {"\u0161", "S"},
        {"\u0160", "S"}, {"\u017e", "Z"}, {"\u017d", "Z"}, {"\u00fc", "U"}, {"\u00f6", "O"}, {"\u00e4", "A"}, {"\u00e1", "A"},
        {"\u00e7", "C"}, {"\u00f1", "N"}};
    static const char* const CIRILLICO[32] = {"A", "B", "V", "G", "D", "E", "ZH", "Z", "I", "Y", "K", "L", "M", "N", "O", "P",
                                              "R", "S", "T", "U", "F", "KH", "TS", "CH", "SH", "SHCH", "", "Y", "'", "E", "YU", "YA"};
    std::string r;
    for (std::size_t i = 0; i < s.size();) {
        // А-Я (D0 90 - D0 AF), а-п (D0 B0 - D0 BF), р-я (D1 80 - D1 8F)
        const int b0 = static_cast<unsigned char>(s[i]), b1 = i + 1 < s.size() ? static_cast<unsigned char>(s[i + 1]) : 0;
        int k = -1;
        if (b0 == 0xD0 && b1 >= 0x90 && b1 <= 0xAF) k = b1 - 0x90;
        else if (b0 == 0xD0 && b1 >= 0xB0 && b1 <= 0xBF) k = b1 - 0xB0;
        else if (b0 == 0xD1 && b1 >= 0x80 && b1 <= 0x8F) k = b1 - 0x80 + 16;
        if (k >= 0) {
            r += CIRILLICO[k];
            i += 2;
            continue;
        }
        bool fatto = false;
        for (const auto& [da, a] : SOST) {
            const std::size_t n = std::char_traits<char>::length(da);
            if (s.compare(i, n, da) == 0) {
                r += a;
                i += n;
                fatto = true;
                break;
            }
        }
        if (fatto) continue;
        const char c = s[i++];
        r += c >= 'a' && c <= 'z' ? static_cast<char>(c - 'a' + 'A') : c;
    }
    return r;
}
std::string taglia(const Font& f, std::string s, int w) {
    while (!s.empty() && f.larghezza(s, 1) > w) {
        while (s.size() > 1 && (static_cast<unsigned char>(s.back()) & 0xC0) == 0x80) s.pop_back();
        s.pop_back();
    }
    return s;
}
std::vector<std::string> a_capo(const Font& f, const std::string& testo, int w) {
    std::vector<std::string> out;
    std::string riga, parola;
    for (std::size_t i = 0; i <= testo.size(); ++i) {
        const char c = i < testo.size() ? testo[i] : ' ';
        if (c != ' ') {
            parola += c;
            continue;
        }
        if (parola.empty()) continue;
        const std::string prova = riga.empty() ? parola : riga + " " + parola;
        if (f.larghezza(prova, 1) > w && !riga.empty()) {
            out.push_back(riga);
            riga = parola;
        } else {
            riga = prova;
        }
        parola.clear();
    }
    if (!riga.empty()) out.push_back(riga);
    return out;
}
void scrivania(Tela& t) {   // il piano di legno, come sotto l'albo dei comandanti
    for (int y = 0; y < ALTEZZA_TELA; ++y)
        for (int x = 0; x < LARGHEZZA_TELA; ++x)
            t.pixel(x, y, static_cast<std::uint8_t>(LEGNO + retina(2.2 + 1.2 * std::sin((x + 9 * std::sin(y * 0.05)) * 0.05) +
                                                                       0.5 * caso(x / 5, y, 2), x, y, 7)));
}
void cornice_semplice(Tela& t, Rettangolo r, int spessore, std::uint8_t col) {
    for (int k = 0; k < spessore; ++k) {
        t.linea_orizzontale(r.x - k, r.x + r.w - 1 + k, r.y - k, col);
        t.linea_orizzontale(r.x - k, r.x + r.w - 1 + k, r.y + r.h - 1 + k, col);
        t.linea_verticale(r.x - k, r.y - k, r.y + r.h - 1 + k, col);
        t.linea_verticale(r.x + r.w - 1 + k, r.y - k, r.y + r.h - 1 + k, col);
    }
}
std::string nome_marina(const std::string& m) { return m == "vmf" ? "VMF" : "US NAVY"; }
std::string sigla_grado(const RegoleCarriera& r, const std::string& marina, const std::string& id) {
    const Grado* g = r.grado(marina == "vmf" ? Marina::vmf : Marina::usn, id);
    return g && !g->sigla.empty() ? maiuscolo(g->sigla) : maiuscolo(id);
}
std::string nome_stato(const VoceOnoreCarriera& v) {
    if (v.stato == "morto") return v.marina == "vmf" ? "POGIB PRI ISPOLNENII" : "ON ETERNAL PATROL";
    if (v.stato == "rimosso") return "RIMOSSO";
    if (v.stato == "congedo") return "CONGEDATO";
    if (v.stato == "pensione") return "IN PENSIONE";
    if (v.stato == "trasferito") return "TRASFERITO A TERRA";
    return maiuscolo(v.stato);
}
std::string nome_origine(const std::string& o) {
    if (o == "rapida") return "RAPIDA";
    if (o == "editor") return "EDITOR";
    if (o == "libreria") return "LIBRERIA";
    return "";
}
// la croce dei caduti (US Navy) o il nastro nero (VMF), alto `h` pixel con lo spigolo in alto a sinistra in (x, y)
void segno_caduto(Tela& t, int x, int y, int h, bool vmf) {
    const auto nero = static_cast<std::uint8_t>(NERO);
    if (vmf) {   // il nastro nero di traverso, con la piega
        for (int k = 0; k < h; ++k) {
            t.linea_orizzontale(x + k / 2, x + k / 2 + 3, y + k, nero);
            t.pixel(x + k / 2 + 4, y + k, static_cast<std::uint8_t>(NERO + 3));
        }
        return;
    }
    const int cx = x + h / 3;
    t.rettangolo_pieno(cx - 1, y, 2, h, nero);              // l'asta
    t.rettangolo_pieno(x, y + h / 3 - 1, 2 * (h / 3) + 1, 2, nero);   // il braccio
}

}  // namespace

SchermataAlboOnore::SchermataAlboOnore(const RegoleCarriera& r, const std::string& dati, std::filesystem::path albo)
    : r_(r), dati_(dati), percorso_(std::move(albo)) {
    try {
        storici_ = carica_storici(dati_ + "/albo_onore_storico.toml");
    } catch (const std::exception&) {
        storici_.clear();   // scheda vuota
    }
    ricarica();
}

void SchermataAlboOnore::ricarica() {
    albo_ = leggi_albo_onore(percorso_);
    cache_scheda_ = -1;
}

void SchermataAlboOnore::scheda(int i) {
    const int s = std::clamp(i, 0, 2);
    if (s != scheda_) scorri_ = 0;
    scheda_ = s;
}

Rettangolo SchermataAlboOnore::linguetta(int i) const { return {FOGLIO.x + 8 + i * 196, FOGLIO.y - 20, 188, 21}; }

SchermataAlboOnore::Uscita SchermataAlboOnore::comandi(const Comandi& c) {
    if (c.esci || c.esc) return Uscita::menu;
    for (const int k : c.cifre)
        if (k >= 1 && k <= 3) scheda(k - 1);
    if (c.tab != 0) scheda((scheda_ + 3 + c.tab) % 3);
    for (const auto& [x, y] : c.clic)
        for (int i = 0; i < 3; ++i) {
            const Rettangolo r = linguetta(i);
            if (x >= r.x && y >= r.y && x < r.x + r.w && y < r.y + r.h) scheda(i);
        }
    // su e giu' a scatti (il tasto tenuto non corre)
    scorri_ = std::max(0, scorri_ - passi_freccia(c.su_giu, c.passi_su_giu, prec_su_giu_));
    const int righe = scheda_ == 0 ? static_cast<int>(albo_.missioni.size()) : scheda_ == 1 ? static_cast<int>(albo_.carriere.size())
                                                                                              : (static_cast<int>(storici_.size()) + 1) / 2;
    const int visibili = scheda_ == 0 ? 15 : scheda_ == 1 ? 8 : 4;
    scorri_ = std::min(scorri_, std::max(0, righe - visibili));
    return Uscita::nessuna;
}

void SchermataAlboOnore::palette(Palette& p) const { imposta_palette(p, Marina::usn); }

void SchermataAlboOnore::disegna(Tela& t, const Font& f) const {
    if (!cache_ || cache_scheda_ != scheda_ || cache_scorri_ != scorri_) {
        cache_ = std::make_unique<Tela>(t.larghezza(), t.altezza());
        componi(*cache_, f);
        cache_scheda_ = scheda_;
        cache_scorri_ = scorri_;
    }
    t = *cache_;
}

void SchermataAlboOnore::componi(Tela& t, const Font& f) const {
    scrivania(t);
    static constexpr const char* SOTTO[3] = {"LE VENTI MIGLIORI MISSIONI, SENZA AIUTI", "LE DIECI MIGLIORI CARRIERE CHIUSE",
                                             "IMPRESE DOCUMENTATE DELLA GUERRA FREDDA, SENZA PUNTEGGI"};
    intestazione(t, f, {0, 0, LARGHEZZA_TELA, 38}, "ALBO D'ONORE", SOTTO[scheda_], Marina::usn, false);
    // le linguette dietro il registro, poi il registro aperto e la linguetta scelta davanti
    for (int i = 0; i < 3; ++i) {
        const Rettangolo r = linguetta(i);
        carta(t, {r.x, r.y, r.w, r.h + 6}, static_cast<std::uint32_t>(40 + i));
        f.scrivi(t, r.x + r.w / 2 - f.larghezza(SCHEDE[i], 1) / 2, r.y + 6, SCHEDE[i], i == scheda_ ? INCHIOSTRO : TENUE, 1);
        char n[12];
        std::snprintf(n, sizeof n, "%d", i + 1);
        f.scrivi(t, r.x + 5, r.y + 6, n, i == scheda_ ? ROSSO : TENUE, 1);
    }
    carta(t, FOGLIO, 7);
    const Rettangolo sc = linguetta(scheda_);
    carta(t, {sc.x, sc.y, sc.w, sc.h + 4}, static_cast<std::uint32_t>(40 + scheda_));
    f.scrivi(t, sc.x + sc.w / 2 - f.larghezza(SCHEDE[scheda_], 1) / 2, sc.y + 6, SCHEDE[scheda_], INCHIOSTRO, 1);
    char n[12];
    std::snprintf(n, sizeof n, "%d", scheda_ + 1);
    f.scrivi(t, sc.x + 5, sc.y + 6, n, ROSSO, 1);
    const Rettangolo r{FOGLIO.x + 14, FOGLIO.y + 10, FOGLIO.w - 28, FOGLIO.h - 20};
    if (scheda_ == 0) missioni(t, f, r);
    else if (scheda_ == 1) carriere(t, f, r);
    else storici(t, f, r);
    t.rettangolo_pieno(0, PIEDE_Y - 2, LARGHEZZA_TELA, ALTEZZA_TELA - PIEDE_Y + 2, static_cast<std::uint8_t>(LEGNO));
    f.scrivi(t, 12, PIEDE_Y + 10, "1-3 O TAB LE SCHEDE   SU/GIU' SCORRE   ESC MENU", CHIARO, 1);
}

void SchermataAlboOnore::missioni(Tela& t, const Font& f, Rettangolo r) const {
    // colonne del registro: numero, punti, comandante (grado e marina), missione (e codice), date
    const int cx[6] = {r.x, r.x + 22, r.x + 70, r.x + 260, r.x + 452, r.x + r.w};
    static constexpr const char* TITOLI[5] = {"N.", "PUNTI", "COMANDANTE", "MISSIONE", "DATA"};
    for (int k = 0; k < 5; ++k) f.scrivi(t, cx[k], r.y, TITOLI[k], ROSSO, 1);
    t.linea_orizzontale(r.x, r.x + r.w - 1, r.y + RIGA, INCHIOSTRO);
    t.linea_orizzontale(r.x, r.x + r.w - 1, r.y + RIGA + 2, INCHIOSTRO);
    if (albo_.missioni.empty()) {
        for (const auto& [i, s] : {std::pair{0, "NESSUNA MISSIONE ANCORA NEL REGISTRO."},
                                   {1, "ENTRANO LE MISSIONI PORTATE A TERMINE SENZA AIUTI (REALISMO PIENO), MAI QUELLE RIPRESE"},
                                   {2, "DA UN EVENTO DEL DEBRIEFING. OGNUNA COL SUO CODICE, PER RIGIOCARLA."}})
            f.scrivi(t, r.x + 4, r.y + 40 + i * 14, taglia(f, s, r.w - 8), TENUE, 1);
        return;
    }
    constexpr int H = 22;   // due righe per voce: i nomi e le date
    for (int i = scorri_; i < static_cast<int>(albo_.missioni.size()); ++i) {
        const int y = r.y + 18 + (i - scorri_) * H;
        if (y + H > r.y + r.h) break;
        const VoceOnoreMissione& v = albo_.missioni[static_cast<std::size_t>(i)];
        char buf[64];
        std::snprintf(buf, sizeof buf, "%d", i + 1);
        f.scrivi(t, cx[0], y, buf, i < 3 ? ROSSO : INCHIOSTRO, 1);
        std::snprintf(buf, sizeof buf, "%.0f", v.punteggio);
        f.scrivi(t, cx[1], y, buf, INCHIOSTRO, 1);
        f.scrivi(t, cx[2], y, taglia(f, maiuscolo(v.nome), cx[3] - cx[2] - 6), INCHIOSTRO, 1);
        f.scrivi(t, cx[2], y + RIGA, taglia(f, sigla_grado(r_, v.marina, v.grado) + "  " + nome_marina(v.marina), cx[3] - cx[2] - 6), TENUE, 1);
        const std::string orig = nome_origine(v.origine);
        f.scrivi(t, cx[3], y, taglia(f, maiuscolo(v.missione), cx[4] - cx[3] - 6), INCHIOSTRO, 1);
        if (!v.codice.empty()) {   // il codice (della rapida o della partita), in un riquadro, ben leggibile
            // «CODICE» davanti finche' sta nella colonna; i codici partita (18 caratteri e due trattini) vanno da soli
            std::string cod = "CODICE " + maiuscolo(v.codice);
            if (f.larghezza(cod, 1) + 6 > cx[4] - cx[3] - 4) cod = maiuscolo(v.codice);
            cod = taglia(f, cod, cx[4] - cx[3] - 10);
            const int w = f.larghezza(cod, 1) + 6;
            cornice_semplice(t, {cx[3] - 2, y + RIGA - 1, w, RIGA}, 1, ROSSO);
            f.scrivi(t, cx[3] + 1, y + RIGA + 1, cod, ROSSO, 1);
        } else if (!orig.empty()) {
            f.scrivi(t, cx[3], y + RIGA, orig, TENUE, 1);
        }
        f.scrivi(t, cx[4], y, maiuscolo(v.data), INCHIOSTRO, 1);
        f.scrivi(t, cx[4], y + RIGA, taglia(f, "GIOCATA " + maiuscolo(v.data_reale), cx[5] - cx[4]), TENUE, 1);
        t.linea_orizzontale(r.x, r.x + r.w - 1, y + H - 2, RIGATURA);
    }
    if (static_cast<int>(albo_.missioni.size()) > 15) {
        char buf[48];
        std::snprintf(buf, sizeof buf, "%d-%d DI %zu", scorri_ + 1, std::min(scorri_ + 15, static_cast<int>(albo_.missioni.size())),
                      albo_.missioni.size());
        f.scrivi(t, r.x + r.w - f.larghezza(buf, 1), r.y + r.h - RIGA + 2, buf, TENUE, 1);
    }
}

void SchermataAlboOnore::carriere(Tela& t, const Font& f, Rettangolo r) const {
    const int cx[6] = {r.x, r.x + 22, r.x + 70, r.x + 262, r.x + 430, r.x + r.w};
    static constexpr const char* TITOLI[5] = {"N.", "PUNTI", "COMANDANTE", "DECORAZIONE", "STATO"};
    for (int k = 0; k < 5; ++k) f.scrivi(t, cx[k], r.y, TITOLI[k], ROSSO, 1);
    t.linea_orizzontale(r.x, r.x + r.w - 1, r.y + RIGA, INCHIOSTRO);
    t.linea_orizzontale(r.x, r.x + r.w - 1, r.y + RIGA + 2, INCHIOSTRO);
    if (albo_.carriere.empty()) {
        f.scrivi(t, r.x + 4, r.y + 40, "NESSUNA CARRIERA CHIUSA NEL REGISTRO.", TENUE, 1);
        f.scrivi(t, r.x + 4, r.y + 54, taglia(f, "ENTRANO A FINE CARRIERA: CONGEDO, PENSIONE, RIMOZIONE O TRASFERIMENTO. I CADUTI RESTANO.", r.w - 8),
                 TENUE, 1);
        return;
    }
    constexpr int H = 40;   // tre righe per voce
    for (int i = scorri_; i < static_cast<int>(albo_.carriere.size()); ++i) {
        const int y = r.y + 18 + (i - scorri_) * H;
        if (y + H > r.y + r.h) break;
        const VoceOnoreCarriera& v = albo_.carriere[static_cast<std::size_t>(i)];
        const bool caduto = v.stato == "morto";
        char buf[64];
        std::snprintf(buf, sizeof buf, "%d", i + 1);
        f.scrivi(t, cx[0], y, buf, i < 3 ? ROSSO : INCHIOSTRO, 1);
        std::snprintf(buf, sizeof buf, "%.0f", v.punti);
        f.scrivi(t, cx[1], y, buf, INCHIOSTRO, 1);
        int xn = cx[2];
        if (caduto) {
            segno_caduto(t, xn, y - 1, 10, v.marina == "vmf");
            xn += 12;
        }
        f.scrivi(t, xn, y, taglia(f, maiuscolo(v.nome), cx[3] - xn - 6), INCHIOSTRO, 1);
        f.scrivi(t, cx[2], y + RIGA, taglia(f, sigla_grado(r_, v.marina, v.grado) + "  " + nome_marina(v.marina), cx[3] - cx[2] - 6), TENUE, 1);
        int yd = y;
        for (const auto& riga : a_capo(f, v.decorazione.empty() ? "-" : maiuscolo(v.decorazione), cx[4] - cx[3] - 8)) {
            if (yd > y + RIGA) break;
            f.scrivi(t, cx[3], yd, riga, INCHIOSTRO, 1);
            yd += RIGA;
        }
        f.scrivi(t, cx[4], y, taglia(f, nome_stato(v), cx[5] - cx[4]), caduto ? ROSSO : INCHIOSTRO, 1);
        std::snprintf(buf, sizeof buf, "%s   %d MISSIONI", maiuscolo(v.data).c_str(), v.missioni);
        f.scrivi(t, cx[4], y + RIGA, taglia(f, buf, cx[5] - cx[4]), TENUE, 1);
        t.linea_orizzontale(r.x, r.x + r.w - 1, y + H - 4, RIGATURA);
    }
}

void SchermataAlboOnore::storici(Tela& t, const Font& f, Rettangolo r) const {
    if (storici_.empty()) {
        f.scrivi(t, r.x + 4, r.y + 40, "LE SCHEDE DEI COMANDANTI STORICI NON SONO DISPONIBILI.", TENUE, 1);
        return;
    }
    // le schede in due colonne, quattro file per pagina
    const int w = (r.w - 12) / 2, h = 88;
    for (int i = scorri_ * 2; i < static_cast<int>(storici_.size()); ++i) {
        const int k = i - scorri_ * 2, col = k % 2, fila = k / 2;
        if (fila >= 4) break;
        const Rettangolo s{r.x + col * (w + 12), r.y + fila * (h + 4), w, h};
        const ImpresaStorica& v = storici_[static_cast<std::size_t>(i)];
        cornice_semplice(t, s, 1, RIGATURA);
        char anno[8];
        std::snprintf(anno, sizeof anno, "%d", v.anno);
        f.scrivi(t, s.x + 6, s.y + 5, anno, ROSSO, 2);
        const int xt = s.x + 6 + f.larghezza(anno, 2) + 8;
        f.scrivi(t, xt, s.y + 5, taglia(f, maiuscolo(v.battello), s.x + s.w - xt - 36), INCHIOSTRO, 1);
        const std::string com = v.comandante.empty() ? "COMANDANTE NON PUBBLICATO" : "COMANDANTE " + maiuscolo(v.comandante);
        f.scrivi(t, xt, s.y + 5 + RIGA, taglia(f, com, s.x + s.w - xt - 6), TENUE, 1);
        // l'impresa su due righe
        const int passo = 13;
        int y = s.y + 29;
        for (const auto& riga : a_capo(f, maiuscolo(v.impresa_it), s.w - 12)) {
            if (y > s.y + 29 + passo) break;
            f.scrivi(t, s.x + 6, y, riga, INCHIOSTRO, 1);
            y += passo;
        }
        f.scrivi(t, s.x + 6, s.y + 29 + 2 * passo, taglia(f, maiuscolo(v.decorazione_it), s.w - 12), static_cast<std::uint8_t>(ORO + 1), 1);
        f.scrivi(t, s.x + 6, s.y + 75, taglia(f, "FONTE: " + maiuscolo(v.fonte) + " (" + maiuscolo(v.confidenza) + ")", s.w - 12), TENUE, 1);
        // la marina nell'angolo
        if (v.marina == "vmf") f.scrivi(t, s.x + s.w - 30, s.y + 5, "VMF", ROSSO, 1);
        else f.scrivi(t, s.x + s.w - 30, s.y + 5, "USN", INCHIOSTRO, 1);
    }
    if (storici_.size() > 8) {
        char buf[48];
        std::snprintf(buf, sizeof buf, "SCHEDE %d-%d DI %zu", scorri_ * 2 + 1, std::min(scorri_ * 2 + 8, static_cast<int>(storici_.size())),
                      storici_.size());
        f.scrivi(t, r.x + r.w - f.larghezza(buf, 1), r.y + r.h - RIGA + 2, buf, TENUE, 1);
    }
}

}  // namespace m688
