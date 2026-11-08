#include "schermata_serie.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <map>

#include "carta.hpp"
#include "grafica/cornici.hpp"
#include "grafica/grafici_serie.hpp"
#include "grafica/lamiera.hpp"
#include "grafica/quadro.hpp"
#include "grafica/simboli_carta.hpp"
#include "grafica/tema_stazioni.hpp"

namespace m688 {

namespace {

constexpr int PIEDE_Y = 428;   // il piede comune: i due tasti a 450-476, larghi 308 da x 8 e 324
const char* const LINGUETTE[] = {"1 ESITI", "2 PER PARTE", "3 ISTOGRAMMI"};

// i testi sul fosforo (font a celle variabili, formato 2), come grafica/quadro
int largo_q(const Font& f, std::string_view s) { return f.larghezza(s, 1, f.formato() == 2 ? f.spazio_predefinito() : 1, f.formato() != 2); }
void scrivi_q(Tela& t, const Font& f, int x, int y, std::string_view s, std::uint8_t c) {
    f.scrivi(t, x, y, s, c, 1, f.formato() == 2 ? f.spazio_predefinito() : 1, f.formato() != 2);
}

struct Voce {
    const char* etichetta;
    const char* unita;
    double scala;   // dal valore del file all'unita' mostrata
};
// i numeri di numeri_parte (nucleo/statistiche_sandbox) con l'etichetta e l'unita' della schermata, nell'ordine della
// tabella: esito, scoperta, reazione e TMA, armi, esposizione, navigazione
struct VoceNome {
    const char* nome;
    Voce v;
};
const std::vector<VoceNome>& voci() {
    static const std::vector<VoceNome> v = {
        {"unita", {"UNITA'", "", 1.0}},
        {"perse", {"PERSE", "", 1.0}},
        {"inflitte", {"INFLITTE", "", 1.0}},
        {"scambio", {"SCAMBIO", "", 1.0}},
        {"scoperte", {"SCOPERTE FATTE", "", 1.0}},
        {"scoperte_subite", {"SCOPERTE SUBITE", "", 1.0}},
        {"ritardo_medio_s", {"RITARDO DI SCOPERTA", "s", 1.0}},
        {"distanza_media_m", {"DISTANZA DI SCOPERTA", "km", 0.001}},
        {"reazione_media_s", {"SCOPERTA - ALLERTA", "s", 1.0}},
        {"ingaggio_medio_s", {"SCOPERTA - INGAGGIO", "s", 1.0}},
        {"lancio_medio_s", {"SCOPERTA - LANCIO", "s", 1.0}},
        {"gambe_medie", {"GAMBE DI TMA", "", 1.0}},
        {"soluzione_media_s", {"TEMPO ALLA SOLUZIONE", "s", 1.0}},
        {"errore_lancio_medio_m", {"ERRORE AL LANCIO", "m", 1.0}},
        {"lanci", {"LANCI", "", 1.0}},
        {"a_segno", {"A SEGNO", "", 1.0}},
        {"esche", {"ESCHE", "", 1.0}},
        {"esche_efficaci", {"ESCHE EFFICACI", "", 1.0}},
        {"evasione_media_s", {"MINACCIA - EVASIONE", "s", 1.0}},
        {"periscopio_medio_s", {"PERISCOPIO ALZATO", "s", 1.0}},
        {"antenna_media_s", {"ANTENNA ALZATA", "s", 1.0}},
        {"radar_medio_s", {"RADAR ACCESO", "s", 1.0}},
        {"ping", {"PING", "", 1.0}},
        {"radio", {"TRASMISSIONI RADIO", "", 1.0}},
        {"contro_scoperte", {"CONTRO-SCOPERTE", "", 1.0}},
        {"errore_nav_medio_m", {"ERRORE DI NAVIGAZIONE", "m", 1.0}},
        {"errore_nav_max_m", {"ERRORE NAV. MASSIMO", "m", 1.0}},
        {"punti_nave", {"PUNTI NAVE", "", 1.0}},
    };
    return v;
}
Voce voce(const std::string& nome) {
    for (const auto& x : voci())
        if (nome == x.nome) return x.v;
    return Voce{nullptr, "", 1.0};
}
std::string etichetta(const std::string& nome) {
    const Voce v = voce(nome);
    if (v.etichetta) return v.etichetta;
    std::string s = nome;
    for (auto& c : s) c = c == '_' ? ' ' : static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return s;
}
// 0x18: la parte del luogo (M7, unita' con schieramento = 'luogo'), una terza parte armata solo nell'Enhanced
const char* nome_parte(int p) { return p == 0x08 ? "NATO" : p == 0x10 ? "PATTO" : p == 0x18 ? "LUOGO" : "NEUTRALI"; }
std::uint8_t colore_parte(int p) {   // sul fosforo: NATO ciano chiaro, Patto rosso chiaro, luogo giallo, neutrali verde chiaro
    return static_cast<std::uint8_t>(p == 0x08 ? colori::CIANO_CHIARO : p == 0x10 ? 12 : p == 0x18 ? colori::GIALLO : colori::VERDE_CHIARO);
}
std::string maiu(std::string s) {
    for (auto& c : s) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return s;
}

}  // namespace

SchermataSerie::SchermataSerie(std::string dati, const std::filesystem::path& riassunto)
    : SchermataSerie(std::move(dati), leggi_serie(riassunto), riassunto.parent_path()) {}

SchermataSerie::SchermataSerie(std::string dati, DatiSerie serie, std::filesystem::path cartella)
    : dati_(std::move(dati)), s_(std::move(serie)), cartella_(std::move(cartella)) {
    try {
        targa_ = Font::carica(dati_ + "/font/incisione688_7.fnt");
        fosforo_ = Font::carica(dati_ + "/font/fosforo688_5x7.fnt");
    } catch (const std::exception&) {
        targa_.reset();
        fosforo_.reset();
    }
    for (const int p : {0x08, 0x10, 0x18, 0})
        if (s_.riassunto.parti.count(p)) parti_.push_back(p);
    // i numeri con almeno un valore, nell'ordine della tabella delle voci (poi gli altri)
    std::vector<std::string> tutti;
    for (const auto& [p, voci_p] : s_.riassunto.parti)
        for (const auto& [nome, v] : voci_p)
            if (v.n > 0 && std::find(tutti.begin(), tutti.end(), nome) == tutti.end()) tutti.push_back(nome);
    for (const auto& x : voci())
        if (std::find(tutti.begin(), tutti.end(), x.nome) != tutti.end()) numeri_.push_back(x.nome);
    for (const auto& nome : tutti)
        if (std::find(numeri_.begin(), numeri_.end(), nome) == numeri_.end()) numeri_.push_back(nome);
    // prima lo scambio, se c'e'
    if (const auto it = std::find(numeri_.begin(), numeri_.end(), "scambio"); it != numeri_.end()) numero_ = static_cast<int>(it - numeri_.begin());
}

void SchermataSerie::numero(int k) {
    if (numeri_.empty()) return;
    const int n = static_cast<int>(numeri_.size());
    numero_ = ((k % n) + n) % n;
}

std::vector<double> SchermataSerie::valori(int parte, const std::string& nome) const {
    const double scala = voce(nome).scala;
    std::vector<double> v;
    for (const PartitaSerie& p : s_.partite) {
        double x = std::numeric_limits<double>::quiet_NaN();
        if (const auto ip = p.numeri.find(parte); ip != p.numeri.end())
            if (const auto in = ip->second.find(nome); in != ip->second.end() && in->second >= 0.0) x = in->second * scala;
        v.push_back(x);
    }
    return v;
}

bool SchermataSerie::comandi(const Comandi& c, double) {
    if (c.esci || c.esc || c.invio) return true;
    if (c.tab != 0) pagina_ = (pagina_ + PAGINE + (c.tab > 0 ? 1 : -1)) % PAGINE;
    if (const int su = passi_freccia(c.su_giu, c.passi_su_giu, prec_su_giu_)) {   // un passo per pressione
        if (pagina_ == 2) numero(numero_ - su);
        if (pagina_ == 1) scorri_ = std::clamp(scorri_ - su, 0, std::max(0, static_cast<int>(numeri_.size()) - righe_tabella_));
    }
    for (const auto& [x, y] : c.clic) {
        if (y >= PIEDE_Y + 22 && y < PIEDE_Y + 48) {   // i tasti del piede: TAB la pagina seguente, ESC indietro
            if (x >= 324) return true;
            pagina_ = (pagina_ + 1) % PAGINE;
            continue;
        }
        bool linguetta = false;
        for (int k = 0; k < static_cast<int>(linguette_.size()); ++k) {
            const Rettangolo& r = linguette_[static_cast<std::size_t>(k)];
            if (x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h) pagina_ = k, linguetta = true;
        }
        if (linguetta) continue;
        if (pagina_ != 2) continue;
        for (std::size_t k = 0; k < barre_.size() && k < s_.partite.size(); ++k) {
            const Rettangolo& r = barre_[k];
            if (x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h && !s_.partite[k].traccia.empty()) {
                traccia_scelta_ = cartella_ / s_.partite[k].traccia;
                return true;
            }
        }
    }
    return false;
}

void SchermataSerie::palette(Palette& p) const {
    applica_palette_carta(p, StileCarta::vga);
    applica_tema(p, TemaStazione{});   // 16-47: la lamiera e il fosforo della variante C
}

void SchermataSerie::disegna(Tela& t, const Font& f) const {
    // variante C (roadmap 33, grafica/quadro): la consolle della sala analisi, uno schermo con le linguette, il piede
    namespace g = grafici;
    namespace q = quadro;
    const Font& fg = fosforo_ ? *fosforo_ : f;
    const Rettangolo u = q::consolle(t, targa_ ? *targa_ : f, "SALA ANALISI - SERIE DELLA SANDBOX");
    const std::string destra = std::to_string(s_.semi) + " PARTITE, SEMI " + std::to_string(s_.da) + "-" + std::to_string(s_.da + s_.semi - 1);
    const Rettangolo a = q::schermo(t, {u.x, u.y, u.w, PIEDE_Y - 4 - u.y}, fg, maiu(s_.missione), destra);
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
    barre_.clear();
    // la sintesi della pagina 1: l'esito piu' frequente e il Pk per arma, per parte
    const auto esito_frequente = [&] {
        const EsitoSerie* m = nullptr;
        for (const auto& [codice, e] : s_.riassunto.esiti)
            if (!m || e.volte > m->volte) m = &e;
        return m ? maiu(m->testo) + ": " + std::to_string(m->volte) + " SU " + std::to_string(s_.semi) : std::string("-");
    };
    std::vector<std::string> armi;
    for (const auto& [parte, voci_pk] : s_.riassunto.pk)
        for (const auto& [arma, x] : voci_pk)
            if (std::find(armi.begin(), armi.end(), arma) == armi.end()) armi.push_back(arma);
    const auto pk = [&](int parte, const std::string& arma) {
        if (const auto ip = s_.riassunto.pk.find(parte); ip != s_.riassunto.pk.end())
            if (const auto ia = ip->second.find(arma); ia != ip->second.end())
                return std::to_string(ia->second.first) + " SU " + std::to_string(ia->second.second);
        return std::string("-");
    };
    if (pagina_ == 0) {   // gli esiti
        std::vector<g::VoceEsito> v;
        for (const auto& [codice, e] : s_.riassunto.esiti) v.push_back({codice, maiu(e.testo), e.volte});
        std::sort(v.begin(), v.end(), [](const g::VoceEsito& x, const g::VoceEsito& z) { return x.volte > z.volte; });
        int y = y0 + 4;
        y += q::intestazione(t, a, y, fg, "COME SONO FINITE LE PARTITE");
        g::barre_esiti(t, f, {a.x + 6, y + 2, a.w - 12, 14 * std::max<int>(1, static_cast<int>(v.size()))}, v, fosforo(6), fosforo(6), fosforo(3));
        y += 14 * static_cast<int>(v.size()) + 14;
        // il Pk per classe d'arma, sommato sulle partite: a segno su lanciate, per parte
        if (!armi.empty() && y < fondo - 40) {
            std::vector<int> col;
            std::vector<std::string> nomi;
            for (std::size_t j = 0; j < parti_.size(); ++j) col.push_back(200 + 150 * static_cast<int>(j)), nomi.push_back(nome_parte(parti_[j]));
            y += q::intestazione(t, a, y, fg, "PK PER ARMA (A SEGNO SU LANCIATE)", col, nomi);
            for (const auto& arma : armi) {
                if (y + 11 > fondo - 30) break;
                std::vector<std::string> celle;
                for (const int parte : parti_) celle.push_back(pk(parte, arma));
                q::riga(t, a, y, fg, maiu(arma), col, celle);
                y += 11;
            }
        }
        if (!s_.codice.empty()) scrivi_q(t, fg, a.x + 4, fondo - 22, "CODICE " + s_.codice, fosforo(6));
        if (!s_.data.empty()) scrivi_q(t, fg, a.x + 4, fondo - 11, "GIOCATA IL " + s_.data, fosforo(4));
    } else if (pagina_ == 1) {   // per parte: media +- scarto, la barra statistica, min-max; NATO e Patto sulla stessa scala
        // le colonne del kit: per parte il valore, la barra larga 80 e l'intervallo
        constexpr int XN = 150, BN = 236, RN = 322, XP = 374, BP = 460, RP = 546, BW = 80;
        const int xs[2][3] = {{XN, BN, RN}, {XP, BP, RP}};
        std::vector<std::string> nomi;
        for (std::size_t j = 0; j < parti_.size() && j < 2; ++j) nomi.push_back(nome_parte(parti_[j])), nomi.push_back("MIN-MAX");
        std::vector<int> col;
        for (std::size_t j = 0; j < parti_.size() && j < 2; ++j) col.push_back(xs[j][0]), col.push_back(xs[j][2]);
        int y = y0;
        y += q::intestazione(t, a, y, fg, "MEDIA +- SCARTO", col, nomi);
        // in fondo la sintesi: l'esito, il Pk delle armi, il codice
        const int righe_sintesi = 2 + static_cast<int>(armi.size()) + (s_.codice.empty() ? 0 : 1);
        const int y_sintesi = fondo - righe_sintesi * 10 - 4;
        righe_tabella_ = std::max(1, (y_sintesi - 8 - y) / 10);
        const int massimo = std::max(0, static_cast<int>(numeri_.size()) - righe_tabella_);
        const int da = std::min(scorri_, massimo);
        for (int k = da; k < static_cast<int>(numeri_.size()) && k < da + righe_tabella_; ++k, y += 10) {
            const std::string& nome = numeri_[static_cast<std::size_t>(k)];
            const Voce v = voce(nome);
            q::riga(t, a, y, fg, etichetta(nome), {}, {}, (k - da) % 2 == 1);
            std::vector<g::Riassunto> rs;
            double scala = 1e-9;
            for (std::size_t j = 0; j < parti_.size() && j < 2; ++j) {
                rs.push_back(g::riassumi(valori(parti_[j], nome)));
                if (rs.back().n > 0) scala = std::max(scala, rs.back().max);
            }
            for (std::size_t j = 0; j < rs.size(); ++j) {
                const g::Riassunto& r = rs[j];
                if (r.n == 0) {
                    scrivi_q(t, fg, a.x + xs[j][0], y, "-", fosforo(3));
                    continue;
                }
                const std::string unita = *v.unita ? std::string(" ") + maiu(v.unita) : std::string();
                scrivi_q(t, fg, a.x + xs[j][0], y, g::numero(r.media) + " +- " + g::numero(r.dev) + unita, fosforo(6));
                std::string intervallo = r.min == r.max ? g::numero(r.min) : g::numero(r.min) + "-" + g::numero(r.max);
                const int largo_max = (j == 0 ? XP : a.w - 4) - xs[j][2] - 4;   // non sconfina nella colonna dopo
                while (!intervallo.empty() && largo_q(fg, intervallo) > largo_max) intervallo.pop_back();
                scrivi_q(t, fg, a.x + xs[j][2], y, intervallo, fosforo(4));
                q::barra_statistica(t, {a.x + xs[j][1], y, BW, fg.righe()}, scala, r.media, r.dev, r.min, r.max, colore_parte(parti_[j]));
            }
        }
        if (massimo > 0) scrivi_q(t, fg, a.x + a.w - 10, y_sintesi - 18, scorri_ < massimo ? "V" : "^", fosforo(7));
        // la sintesi della pagina 1, che sta sempre sotto gli occhi
        int ys = y_sintesi;
        std::vector<int> cs;
        std::vector<std::string> ns;
        for (std::size_t j = 0; j < parti_.size() && j < 2; ++j) cs.push_back(xs[j][0]), ns.push_back(nome_parte(parti_[j]));
        ys += q::intestazione(t, a, ys, fg, "IN SINTESI (PAGINA 1)", cs, ns) - 5;
        q::riga(t, a, ys, fg, "COME SONO FINITE", {XN}, {esito_frequente()});
        ys += 10;
        for (const auto& arma : armi) {
            std::vector<std::string> celle;
            for (std::size_t j = 0; j < parti_.size() && j < 2; ++j) celle.push_back(pk(parti_[j], arma));
            q::riga(t, a, ys, fg, "PK " + maiu(arma), cs, celle);
            ys += 10;
        }
        if (!s_.codice.empty()) q::riga(t, a, ys, fg, "CODICE DELLA SERIE", {XN}, {s_.codice});
    } else {   // gli istogrammi del numero scelto e le partite
        if (numeri_.empty()) {
            scrivi_q(t, fg, a.x + 6, y0 + 10, "NESSUN NUMERO NELLA SERIE", fosforo(4));
        } else {
            const std::string& nome = numeri_[static_cast<std::size_t>(numero_)];
            const Voce v = voce(nome);
            int y = y0 + 2;
            y += q::intestazione(t, a, y, fg, "< " + etichetta(nome) + " >   (FRECCE SU E GIU')");
            const int wi = (a.w - 24) / 2;
            for (std::size_t j = 0; j < parti_.size() && j < 2; ++j) {
                g::OpzioniIstogramma o;
                o.titolo = nome_parte(parti_[j]);
                o.unita = v.unita;
                o.colore = colore_parte(parti_[j]);
                o.colore_testo = fosforo(5);
                o.colore_mediana = fosforo(7);
                g::istogramma(t, f, {a.x + 6 + static_cast<int>(j) * (wi + 12), y, wi, 190}, valori(parti_[j], nome), o);
            }
            y += 200;
            // una barra per partita, della prima parte (la NATO se c'e'): un clic apre il replay di quella partita
            const int p0 = parti_.empty() ? 0x08 : parti_.front();
            y += q::intestazione(t, a, y, fg, std::string("LE PARTITE (") + nome_parte(p0) + ", CLIC: IL REPLAY)");
            const auto vv = valori(p0, nome);
            const Rettangolo rb{a.x + 6, y + 2, a.w - 12, std::max(20, fondo - 14 - y - 2)};
            barre_ = g::barre_partite(t, rb, vv, colore_parte(p0));
            // ogni partita ha un segno: lo zero e' una tacca alla base (la barra del kit e' alta un pixel), e sotto il seme
            const int base = rb.y + rb.h - 1;
            for (std::size_t k = 0; k < barre_.size() && k < vv.size(); ++k) {
                const Rettangolo& z = barre_[k];
                if (std::isfinite(vv[k]) && vv[k] == 0.0) t.rettangolo_pieno(z.x, base - 3, std::max(1, z.w - 1), 3, colore_parte(p0));
                const std::string seme = std::to_string(s_.partite[k].seme);
                if (z.w >= largo_q(fg, seme) + 2) scrivi_q(t, fg, z.x + (z.w - largo_q(fg, seme)) / 2, base + 3, seme, fosforo(4));
            }
            if (!barre_.empty() && barre_.front().w < 14 && !s_.partite.empty())   // troppo strette per i numeri: il primo e l'ultimo
                scrivi_q(t, fg, a.x + 6, base + 3, "SEME " + std::to_string(s_.partite.front().seme) + " ... " + std::to_string(s_.partite.back().seme),
                         fosforo(4));
        }
    }
    lamiera::piede(t, f, PIEDE_Y, "TAB PAGINA   FRECCE NUMERO O SCORRONO   CLIC SU UNA PARTITA: IL REPLAY",
                   {{"TAB  PAGINA SEGUENTE", ColoreLampada::ambra}, {"ESC  INDIETRO", ColoreLampada::verde}}, 1);
}

}  // namespace m688
