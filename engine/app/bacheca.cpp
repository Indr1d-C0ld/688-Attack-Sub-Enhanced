#include "bacheca.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>

#include "museo.hpp"
#include "grafica/cerimonia.hpp"
#include "grafica/cornici.hpp"
#include "grafica/medaglioni.hpp"
#include "grafica/ritratto.hpp"

namespace m688 {

using namespace cerimonia;

namespace {

constexpr auto INCHIOSTRO = static_cast<std::uint8_t>(NERO + 1), TENUE = static_cast<std::uint8_t>(NERO + 3),
               CHIARO = static_cast<std::uint8_t>(BIANCO + 4);
constexpr int PIEDE_Y = 428;   // il piede di cerimonia (grafica/cornici), come l'albo
constexpr int COLONNE = 8, LATO = 48, PASSO_X = 72, PASSO_Y = 76, GRIGLIA_X = 34, GRIGLIA_Y = 78;

struct Scheda {
    const char* categoria;
    const char* titolo;
};
constexpr Scheda SCHEDE[] = {{"tattico", "TATTICI"}, {"stealth", "STEALTH"},  {"tecnico", "TECNICI"}, {"storico", "STORICI"},
                             {"carriera", "CARRIERA"}, {"editor", "EDITOR"}, {"nascosto", "NASCOSTI"}};
constexpr int N_SCHEDE = static_cast<int>(std::size(SCHEDE));

std::string maiuscolo(std::string s) { return testo_per_font(std::move(s)); }

std::string taglia(const Font& f, std::string s, int w) {
    while (!s.empty() && f.larghezza(s, 1) > w) {
        while (s.size() > 1 && (static_cast<unsigned char>(s.back()) & 0xC0) == 0x80) s.pop_back();
        s.pop_back();
    }
    return s;
}

void scrivania(Tela& t) {   // il piano di legno dell'albo
    for (int y = 0; y < ALTEZZA_TELA; ++y)
        for (int x = 0; x < LARGHEZZA_TELA; ++x)
            t.pixel(x, y, static_cast<std::uint8_t>(LEGNO + retina(2.2 + 1.2 * std::sin((x + 9 * std::sin(y * 0.05)) * 0.05) +
                                                                       0.5 * caso(x / 5, y, 2), x, y, 7)));
}

std::string numero(double v) {   // 12 o 12,5
    char b[24];
    if (std::fabs(v - std::round(v)) < 1e-6) std::snprintf(b, sizeof b, "%.0f", v);
    else std::snprintf(b, sizeof b, "%.1f", v);
    std::string s = b;
    std::replace(s.begin(), s.end(), '.', ',');
    return s;
}

}  // namespace

SchermataBacheca::SchermataBacheca(const CatalogoAchievement& cat, const TavolaDecorazioni& tav, std::optional<Fascicolo> comandante)
    : cat_(cat), tav_(tav), comandante_(std::move(comandante)), file_(leggi_achievement()), globali_(!comandante_) {}

std::vector<const DefAchievement*> SchermataBacheca::voci() const {
    std::vector<const DefAchievement*> v;
    for (const auto& d : cat_.achievement)
        if (d.categoria == SCHEDE[scheda_].categoria) v.push_back(&d);
    return v;
}

const StatoAchievement& SchermataBacheca::stato() const {
    if (globali_) return file_.globali;
    const auto it = file_.comandanti.find(comandante_->id);
    return it != file_.comandanti.end() ? it->second : vuoto_;
}

Rettangolo SchermataBacheca::casella(int i) const {
    return {GRIGLIA_X + (i % COLONNE) * PASSO_X, GRIGLIA_Y + (i / COLONNE) * PASSO_Y, LATO + 20, LATO + 22};
}

void SchermataBacheca::scegli(int scheda, int voce) {
    scheda_ = std::clamp(scheda, 0, N_SCHEDE - 1);
    voce_ = std::max(0, voce);
    sporca_ = true;
}

bool SchermataBacheca::comandi(const Comandi& c) {
    if (c.esci || c.esc) return true;
    const int prima_scheda = scheda_, prima_voce = voce_;
    if (c.tab) scheda_ = (scheda_ + N_SCHEDE + c.tab) % N_SCHEDE;
    for (const int k : c.cifre)
        if (k >= 1 && k <= N_SCHEDE) scheda_ = k - 1;
    bool alterna = false;
    for (const auto& [x, y] : c.clic) {
        if (y >= 44 && y < 64) scheda_ = std::clamp((x - 16) / 87, 0, N_SCHEDE - 1);
        switch (tasto_cerimonia(x, y, PIEDE_Y, 3)) {   // i tasti del piede: indietro, comandante o giocatore, scheda
            case 0: return true;
            case 1: alterna = comandante_.has_value(); break;
            case 2: scheda_ = (scheda_ + 1) % N_SCHEDE; break;
            default: break;
        }
        for (int i = 0; i < static_cast<int>(voci().size()); ++i) {
            const Rettangolo r = casella(i);
            if (x >= r.x && y >= r.y && x < r.x + r.w && y < r.y + r.h) voce_ = i;
        }
    }
    if (scheda_ != prima_scheda) voce_ = 0;
    const int n = static_cast<int>(voci().size());
    voce_ += passi_freccia(c.ruota, c.passi_ruota, prec_ruota_);
    voce_ -= COLONNE * passi_freccia(c.su_giu, c.passi_su_giu, prec_su_giu_);
    voce_ = std::clamp(voce_, 0, std::max(0, n - 1));
    if (comandante_ && std::find(c.lettere.begin(), c.lettere.end(), 'G') != c.lettere.end()) alterna = true;
    if (alterna) globali_ = !globali_;
    if (scheda_ != prima_scheda || voce_ != prima_voce || !c.lettere.empty() || alterna) sporca_ = true;
    return false;
}

void SchermataBacheca::palette(Palette& p) const {
    ParametriRitratto r;
    if (comandante_) r.marina = comandante_->marina;
    prepara_palette_ritratto(p, r, &tav_);
}

void SchermataBacheca::disegna(Tela& t, const Font& f) const {
    if (!cache_ || sporca_) {
        cache_ = std::make_unique<Tela>(t.larghezza(), t.altezza());
        componi(*cache_, f);
        sporca_ = false;
    }
    t = *cache_;
}

void SchermataBacheca::componi(Tela& t, const Font& f) const {
    scrivania(t);
    const Marina m = comandante_ ? comandante_->marina : Marina::usn;
    const std::string chi = globali_ ? "I RICONOSCIMENTI DEL GIOCATORE, IN TUTTE LE CARRIERE"
                                     : "I RICONOSCIMENTI DI " + maiuscolo(comandante_->nome);
    intestazione(t, f, {0, 0, LARGHEZZA_TELA, 38}, "BACHECA DEI RICONOSCIMENTI", chi, m, false);
    const StatoAchievement& s = stato();
    for (int k = 0; k < N_SCHEDE; ++k) {   // le schede, coi presi su quanti
        int presi = 0, tutti = 0;
        for (const auto& d : cat_.achievement)
            if (d.categoria == SCHEDE[k].categoria) {
                ++tutti;
                presi += s.livello(d.id) > 0;
            }
        const std::string testo = std::to_string(k + 1) + " " + SCHEDE[k].titolo + " " + std::to_string(presi) + "/" + std::to_string(tutti);
        pulsante(t, f, {16 + k * 87, 44, 84, 20}, testo, k == scheda_);
    }
    const auto v = voci();
    for (int i = 0; i < static_cast<int>(v.size()); ++i) {
        const DefAchievement& d = *v[static_cast<std::size_t>(i)];
        const int liv = s.livello(d.id);
        const Rettangolo r = casella(i);
        if (i == voce_) cornice(t, {r.x - 3, r.y - 3, r.w + 6, r.h + 6}, StileCornice::oro, 3);
        disegna_medaglione(t, {r.x + 10, r.y, LATO, LATO}, d.icona, livello_medaglione(liv), liv > 0, d.nascosto,
                           TavolozzaMedaglione::cerimonia, d.categoria);
        // il nome sotto, su due righe al piu' (a capo sulle parole)
        const std::string nome = d.nascosto && !liv ? "?" : maiuscolo(d.nome_it);
        std::string righe[2];
        std::size_t k = 0;
        for (std::size_t a = 0; a <= nome.size() && k < 2;) {
            const std::size_t b = std::min(nome.find(' ', a), nome.size());
            const std::string parola = nome.substr(a, b - a);
            const std::string prova = righe[k].empty() ? parola : righe[k] + " " + parola;
            if (f.larghezza(prova, 1) > r.w && !righe[k].empty() && k == 0) righe[++k] = parola;
            else righe[k] = prova;
            a = b + 1;
        }
        for (int j = 0; j < 2; ++j) {
            const std::string x = taglia(f, righe[j], r.w);
            f.scrivi(t, r.x + r.w / 2 - f.larghezza(x, 1) / 2, r.y + LATO + 4 + j * 10, x, liv ? CHIARO : static_cast<std::uint8_t>(BIANCO + 1), 1);
        }
    }
    // il riquadro del medaglione scelto
    const Rettangolo foglio{24, 310, 592, 114};   // fino a 424: sotto c'e' il piede di cerimonia
    carta(t, foglio, 17);
    if (!v.empty()) {
        const DefAchievement& d = *v[static_cast<std::size_t>(std::clamp(voce_, 0, static_cast<int>(v.size()) - 1))];
        const int liv = s.livello(d.id);
        const bool coperto = d.nascosto && !liv;
        disegna_medaglione(t, {foglio.x + 12, foglio.y + 12, 64, 64}, d.icona, livello_medaglione(liv), liv > 0, d.nascosto,
                           TavolozzaMedaglione::cerimonia, d.categoria);
        const int x = foglio.x + 92, w = foglio.w - 104;
        f.scrivi(t, x, foglio.y + 12, coperto ? "RICONOSCIMENTO NASCOSTO" : maiuscolo(d.nome_it), INCHIOSTRO, 2);
        int y = foglio.y + 34;
        const std::string descr = coperto ? "SI SCOPRE QUANDO LO SI GUADAGNA." : maiuscolo(d.descrizione_it);
        f.scrivi(t, x, y, taglia(f, descr, w), TENUE, 1);
        y += 14;
        static constexpr const char* LIVELLI[] = {"NON ANCORA PRESO", "BRONZO", "ARGENTO", "ORO"};
        std::string stato_testo = LIVELLI[std::clamp(liv, 0, 3)];
        if (liv) {
            const auto it = s.p.find(d.id);
            if (it != s.p.end() && !it->second.missione.empty())
                stato_testo += "   " + maiuscolo(it->second.missione) + (it->second.data.empty() ? "" : ", " + it->second.data);
        }
        f.scrivi(t, x, y, stato_testo, liv ? INCHIOSTRO : TENUE, 1);
        y += 14;
        if (!coperto && d.ambito == AmbitoAchievement::somma && !d.livelli.empty()) {   // l'avanzamento verso la soglia dopo
            const double a = s.avanzamento(d);
            // tre soglie (bronzo, argento, oro) o una sola (oro): la prossima, o l'ultima se e' gia' oro
            const std::size_t k = d.livelli.size() == 3 ? static_cast<std::size_t>(std::min(liv, 2)) : 0;
            const double soglia = d.livelli[k];
            const std::string av = "AVANZAMENTO " + numero(a) + " SU " + numero(soglia);
            f.scrivi(t, x, y, av, INCHIOSTRO, 1);
        }
    }
    // il piede comune della cerimonia (scelta dell'utente del 29/10/2026): il tasto G c'e' sempre, spento senza un comandante
    piede_cerimonia(t, f, PIEDE_Y, "1-7 O TAB LE SCHEDE, LE FRECCE IL MEDAGLIONE",
                    {{"ESC  INDIETRO"}, {globali_ ? "G  COMANDANTE" : "G  GIOCATORE", comandante_.has_value()}, {"TAB  SCHEDA"}}, -1);
}

}  // namespace m688
