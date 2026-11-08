#include "briefing.hpp"

#include <algorithm>
#include <optional>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <vector>

#include "grafica/lamiera.hpp"
#include "grafica/palette.hpp"
#include "grafica/quadro.hpp"
#include "grafica/tema_stazioni.hpp"
#include "nucleo/radio.hpp"
#include "stazione_radio.hpp"
#include "trascina_carta.hpp"

namespace m688 {

namespace {

constexpr int PIEDE_Y = 428;                    // il piede comune: i tre tasti a 450-476, larghi 202 da x 8, 218, 428
constexpr int SINISTRA_W = 420, RIGA_LIVELLO = 12;
constexpr const char* NOMI[SchermataBriefing::LIVELLI] = {"ORDINI E OBIETTIVI", "ZONE OPERATIVE", "INTELLIGENCE",
                                                          "BASI E RADAR COSTIERI", "PORTATE ASW NEMICHE", "SOSUS",
                                                          "FONDALE", "EVENTI SPECIALI"};
struct Pulsante {
    const char* testo;
    SchermataBriefing::Uscita uscita;
};
constexpr Pulsante PULSANTI[] = {{"ESC  RIFIUTA LA MISSIONE", SchermataBriefing::Uscita::rifiuta},
                                 {"F7  SALA SILURI", SchermataBriefing::Uscita::armi},
                                 {"INVIO  IN MARE", SchermataBriefing::Uscita::in_mare}};

std::string maiuscolo(std::string s) {
    for (auto& c : s) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return s;
}

std::string ora_dtg(const std::string& dtg) { return dtg.size() >= 7 ? dtg.substr(2, 5) : dtg; }

VistaCarta vista(const Missione& m, const CartaGiocatore& c, Vec2 centro, Rettangolo r) {
    VistaCarta v;
    v.x = r.x;
    v.y = r.y;
    v.w = r.w;
    v.h = r.h;
    v.centro = centro;
    v.metri_per_pixel = c.scala() * m.metri_per_unita() / 2.0;
    return v;
}

}  // namespace

SchermataBriefing::SchermataBriefing(const Missione& m, bool urss, bool anni_89, StileCarta stile, const std::string& cartella_dati)
    : urss_(urss) {
    if (!cartella_dati.empty()) {
        for (auto& a : m.aerei_asw_previsti(cartella_dati))   // gli aerei ASW dell'altra parte (la parte ignota conta come nemica)
            if (a.parte != (urss ? "urss" : "usa")) asw_nemici_.push_back(std::move(a));
        try {
            targa_ = Font::carica(cartella_dati + "/font/incisione688_7.fnt");
        } catch (const std::exception&) {
            targa_.reset();
        }
    }
    carta_.restyle = true;
    carta_.urss = urss;
    carta_.anni_89 = anni_89;
    carta_.stile = stile;
    for (int i = 0; i < 4; ++i) carta_.riduci();   // una vista piu' ampia del teatro della stazione
    for (int i = 0; i < LIVELLI; ++i) livelli_[static_cast<std::size_t>(i)] = disponibile(m, i);
    if (const auto* g = m.giocatore()) centro_ = g->pos;
    applica_livelli();
}

bool SchermataBriefing::disponibile(const Missione& m, int i) const {
    switch (i) {
        case 0: return true;                                           // la partenza (gli obiettivi dalla M1)
        case 1: return !m.zone_ricevute().empty();
        case 2: return m.radio() && !m.radio()->intelligence(m.tempo()).empty();
        case 4: return !asw_nemici_.empty();
        case 6: return m.fondale() != nullptr;
        default: return false;                                         // basi, portate ASW, SOSUS, eventi: dati futuri
    }
}

void SchermataBriefing::applica_livelli() {
    carta_.livelli.fondale = livelli_[6];
    carta_.livelli.zone = livelli_[1];
    carta_.livelli.intelligence = livelli_[2];
    carta_.livelli.battello = false;    // al suo posto la «P» della partenza
    carta_.livelli.intestazione = false;
    carta_.livelli.mobilia = livelli_[6];   // col fondale, la carta nautica intera
}

SchermataBriefing::Uscita SchermataBriefing::comandi(const Comandi& c, double dt) {
    tempo_ += dt * (c.veloce ? 6.0 : 1.0);
    for (const int n : c.cifre) {
        if (n >= 1 && n <= LIVELLI) livelli_[static_cast<std::size_t>(n - 1)] = !livelli_[static_cast<std::size_t>(n - 1)];
        if (n == 9) {   // tutto o niente, come «Select All / Clear All» di F-117A
            const bool qualcuno = std::any_of(livelli_.begin(), livelli_.end(), [](bool b) { return b; });
            livelli_.fill(!qualcuno);
        }
    }
    const int y_livelli = livelli_r_.y + 21;   // le righe delle spie, come in disegna
    for (const auto& [x, y] : c.clic) {   // righe dei livelli e tasti del piede
        if (x >= livelli_r_.x && x < livelli_r_.x + livelli_r_.w && y >= y_livelli && y < y_livelli + LIVELLI * RIGA_LIVELLO) {
            const auto i = static_cast<std::size_t>((y - y_livelli) / RIGA_LIVELLO);
            livelli_[i] = !livelli_[i];
        }
        if (y >= PIEDE_Y + 22 && y < PIEDE_Y + 48) return PULSANTI[x < 214 ? 0 : x < 424 ? 1 : 2].uscita;
    }
    applica_livelli();
    if (c.tab) ordini_ = !ordini_;
    // con gli ordini aperti le frecce su e giu' girano le pagine; chiusi spostano la carta; zoom con I e O
    const int su = passi_freccia(c.su_giu, c.passi_su_giu, prec_su_giu_);   // una pagina per pressione
    if (ordini_ && su != 0) pagina_ = std::clamp(pagina_ - su, 0, std::max(0, pagine_ - 1));
    if (!ordini_) {
        const double passo = 200.0 * dt * carta_.scala() * 23.0 / 2.0;
        centro_.x += c.ruota * passo;
        centro_.y += c.su_giu * passo;
    }
    if (c.ingrandisci) carta_.ingrandisci();
    if (c.riduci) carta_.riduci();
    const auto mpp = [&] { return carta_.scala() * 23.0 / 2.0; };
    if (c.rotella && dentro_carta(c, mappa_))   // come sul debriefing: attorno al punto sotto il cursore
        for (int k = std::abs(c.rotella); k > 0; --k) {
            const double prima = mpp();
            if (c.rotella > 0) carta_.ingrandisci();
            else carta_.riduci();
            zoom_sul_cursore(c, mappa_, prima, mpp(), centro_);
        }
    trascina_carta(c, mappa_, mpp(), presa_, centro_);
    if (c.esc) return Uscita::rifiuta;
    if (c.stazione == 7) return Uscita::armi;
    if (c.invio) return Uscita::in_mare;
    return Uscita::nessuna;
}

void SchermataBriefing::palette(Palette& p) const {
    applica_palette_carta(p, carta_.stile);
    applica_palette_rilievo(p, carta_.stile);
    TemaStazione tema;   // 16-47: la lamiera e le spie (quella dell'Alfa per il comandante sovietico)
    tema.battello = urss_ ? BattelloTema::alfa : BattelloTema::usa688;
    applica_tema(p, tema);
}

void SchermataBriefing::disegna(Tela& t, const Font& f, const Missione& m) const {
    t.riempi(colori::NERO);
    // la consolle della sala briefing con la testata classificata: il timbro a sinistra, il gruppo data-ora a destra
    const Font& tg = targa_ ? *targa_ : f;
    const Rettangolo u = quadro::consolle(t, tg, "SALA BRIEFING - " + maiuscolo(m.titolo()));
    lamiera::testata_classificata(t, tg, urss_ ? "SEKRETNO" : "SECRET", m.radio() ? m.radio()->dtg(m.tempo()) : std::string());
    const auto oro = static_cast<std::uint8_t>(colori::GIALLO);
    // a sinistra la carta nel pannello, coi livelli scelti e la partenza
    const Rettangolo pl{u.x, u.y, SINISTRA_W, PIEDE_Y - 4 - u.y};
    lamiera::pannello(t, pl, 8);
    mappa_ = {pl.x + 9, pl.y + 9, pl.w - 18, pl.h - 18};
    const VistaCarta v = vista(m, carta_, centro_, mappa_);
    disegna_carta(t, f, m, carta_, {}, -1, std::nullopt, &v);
    t.imposta_ritaglio(mappa_);
    if (livelli_[4]) disegna_asw(t, f, m, v);
    if (livelli_[0])
        if (const auto* g = m.giocatore()) {
            const Vec2 q = v.a_tela(g->pos);
            const int x = static_cast<int>(q.x), y = static_cast<int>(q.y);
            if (x > mappa_.x + 6 && x < mappa_.x + mappa_.w - 6 && y > mappa_.y + 6 && y < mappa_.y + mappa_.h - 6) {
                t.rettangolo_pieno(x - 6, y - 6, 13, 13, colori::NERO);
                t.linea_orizzontale(x - 6, x + 6, y - 6, oro);
                t.linea_orizzontale(x - 6, x + 6, y + 6, oro);
                t.linea_verticale(x - 6, y - 6, y + 6, oro);
                t.linea_verticale(x + 6, y - 6, y + 6, oro);
                f.scrivi(t, x - f.larghezza("P", 1) / 2, y - 3, "P", oro, 1);
            }
        }
    t.togli_ritaglio();
    lamiera::smusso(t, mappa_, smalto(2), smalto(10));
    if (ordini_) disegna_ordini(t, f, m);
    // a destra i livelli, come spie verdi; quelli senza dati della missione spenti e scuri
    const int x0 = u.x + SINISTRA_W + 6, w0 = u.w - SINISTRA_W - 6;
    livelli_r_ = {x0, u.y, w0, LIVELLI * RIGA_LIVELLO + 26};
    lamiera::pannello(t, livelli_r_, 8);
    lamiera::secondaria(t, f, x0 + 10, u.y + 8, "LIVELLI  1-8, 9 TUTTO");
    for (int i = 0; i < LIVELLI; ++i) {
        const int yy = u.y + 22 + i * RIGA_LIVELLO;
        const bool ok = disponibile(m, i), acceso = ok && livelli_[static_cast<std::size_t>(i)];
        lamiera::spia_voce(t, {x0 + 9, yy - 1, 12, 9}, ColoreLampada::verde, acceso);
        const std::string s = std::to_string(i + 1) + " " + NOMI[i];
        if (acceso) lamiera::incisa(t, f, x0 + 26, yy, s, colori::BIANCO);
        else if (ok) lamiera::secondaria(t, f, x0 + 26, yy, s);
        else f.scrivi(t, x0 + 26, yy, s, smalto(5), 1);
    }
    // la scheda della missione: etichette incise, valori nei vetri
    int ys = livelli_r_.y + livelli_r_.h + 4;
    const Rettangolo ps{x0, ys, w0, PIEDE_Y - 4 - ys};
    lamiera::pannello(t, ps, 8);
    ys += 8;
    const int wv = ps.w - 16;
    const auto campo = [&](const char* etichetta, const std::vector<std::pair<std::string, std::uint8_t>>& testi) {
        std::vector<std::pair<std::string, std::uint8_t>> righe;
        for (const auto& [s, c] : testi)
            for (auto& r : lamiera::a_capo(f, s, wv - 8)) righe.emplace_back(std::move(r), c);
        const int h = f.righe() + 2 + static_cast<int>(righe.size()) * (f.righe() + 1) + 4;
        if (ys + h > ps.y + ps.h - 4) return;   // non c'e' piu' posto: il campo si tace
        ys += lamiera::campo_vetro(t, f, ps.x + 8, ys, wv, etichetta, righe) + 4;
    };
    {
        std::string teatro = maiuscolo(m.teatro());
        while (!teatro.empty() && static_cast<unsigned char>(teatro.back()) <= ' ') teatro.pop_back();
        std::vector<std::pair<std::string, std::uint8_t>> righe;
        if (const auto* g = m.giocatore()) {
            std::string classe = maiuscolo(g->classe);
            std::replace(classe.begin(), classe.end(), '_', ' ');
            righe.emplace_back(classe + "  " + teatro, colori::BIANCO);
        }
        if (const SalaRadio* r = m.radio())
            righe.emplace_back("RADIO " + ora_dtg(r->dtg(r->prossima_finestra(m.tempo()))) + " +- " +
                                   std::to_string(static_cast<int>(r->tolleranza_finestra_s() / 60.0)) + " MIN",
                               colori::BIANCO);
        campo("BATTELLO, TEATRO, RADIO", righe);
    }
    if (const SalaRadio* r = m.radio()) {
        const Roe roe = r->roe_ricevute();
        std::string s;
        static constexpr const char* BREVI[] = {"SOTT.", "NAVI", "MERC.", "NEUTR.", "AEREI"};
        for (int c = 0; c < static_cast<int>(CategoriaRoe::numero); ++c)
            s += std::string(c ? "  " : "") + BREVI[c] + " " + maiuscolo(nome_stato_roe(roe.stato[static_cast<std::size_t>(c)]));
        campo("REGOLE D'INGAGGIO", {{s, oro}});
    }
    {
        std::vector<std::pair<std::string, std::uint8_t>> righe;
        scrivi_meteo(m, [&](const std::string& s, std::uint8_t c) { righe.emplace_back(s, c); });
        if (!righe.empty()) campo("METEO E PREVISIONE 12 H", righe);
    }
    if (!codice_.empty()) campo("CODICE DELLA PARTITA", {{codice_, colori::CIANO_CHIARO}});   // per rigiocarla, o darla a un amico
    // il piede comune
    lamiera::piede(t, f, PIEDE_Y,
                   ordini_ ? "1-8 LIVELLI   9 TUTTI   TAB CHIUDE GLI ORDINI   FRECCE PAGINA   MAIUSC ACCELERA"
                           : "1-8 LIVELLI   9 TUTTI   TAB ORDINI   FRECCE SPOSTA   I O ZOOM",
                   {{PULSANTI[0].testo, ColoreLampada::rossa}, {PULSANTI[1].testo, ColoreLampada::ambra},
                    {PULSANTI[2].testo, ColoreLampada::verde}},
                   2);
}

// PORTATE ASW NEMICHE: per ogni aereo ASW dell'altra parte la zona che pattuglia (o, senza zona, il raggio attorno alla
// base o all'unita'), a puntini rossi, con la classe e il compito; la base con una B
void SchermataBriefing::disegna_asw(Tela& t, const Font& f, const Missione& m, const VistaCarta& v) const {
    constexpr std::uint8_t ROSSO_CHIARO = 12;
    for (const auto& a : asw_nemici_) {
        std::optional<Vec2> base = a.base;
        if (!base && a.base_unita >= 0)
            if (const auto it = m.unita().find(a.base_unita); it != m.unita().end()) base = it->second.pos;
        std::optional<Vec2> centro = a.zona ? a.zona : base;
        if (!centro && a.unita >= 0)
            if (const auto it = m.unita().find(a.unita); it != m.unita().end()) centro = it->second.pos;
        if (!centro) continue;
        const double raggio = a.raggio_zona_m > 0.0 ? a.raggio_zona_m : 25.0 * MIGLIO;   // STIMA senza zona: 25 nm
        const Vec2 q = v.a_tela(*centro);
        const double r = raggio / v.metri_per_pixel;
        const int n = std::clamp(static_cast<int>(2.0 * PI * r / 3.0), 24, 2000);
        for (int k = 0; k < n; ++k) {
            const double ang = 2.0 * PI * k / n;
            t.pixel(static_cast<int>(q.x + r * std::sin(ang)), static_cast<int>(q.y - r * std::cos(ang)), ROSSO_CHIARO);
        }
        std::string nome = a.classe + " " + a.missione;
        for (auto& ch : nome) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        f.scrivi(t, static_cast<int>(q.x) - f.larghezza(nome, 1) / 2, static_cast<int>(q.y - r) - 10, nome, ROSSO_CHIARO, 1);
        if (base) {
            const Vec2 b = v.a_tela(*base);
            f.scrivi(t, static_cast<int>(b.x) - 2, static_cast<int>(b.y) - 3, "B", ROSSO_CHIARO, 1);
        }
    }
}

// METEO E MARE (M4, dossier 21 §5): lo stato all'inizio nel punto di partenza e la previsione a 12 ore, con l'errore
// delle previsioni del 1989; il semplificato e' dichiarato tale
void SchermataBriefing::scrivi_meteo(const Missione& m, const std::function<void(const std::string&, std::uint8_t)>& scrivi) const {
    const Meteo* me = m.meteo();
    if (!me || !m.giocatore()) return;
    const auto riga = [&](const char* titolo, const StatoMeteo& s) {
        char b[200];
        std::snprintf(b, sizeof b, "%s VENTO %03.0f° FORZA %d (%.0f M/S), MARE %d (%.1f M), VIS. %.0f KM, NUBI %.0f/8%s%s", titolo,
                      normalizza_360(s.vento_da_gradi), beaufort(s.vento_m_s), s.vento_m_s, s.stato_mare(), s.hs_m(),
                      s.visibilita_m / 1000.0, s.nubi_ottavi, s.pioggia_mm_h > 0.5 ? ", PIOGGIA" : s.neve_mm_h > 0.2 ? ", NEVE" : "",
                      s.nebbia != Nebbia::nessuna ? ", NEBBIA" : "");
        std::string r = b;
        for (auto& ch : r) if (ch == '.') ch = ',';
        return r;
    };
    const StatoMeteo ora = m.meteo_qui();
    scrivi(riga("METEO:", ora), colori::CIANO_CHIARO);
    scrivi(riga(urss_ ? "PROGNOZ 12 H:" : "WEAX 12 H:", me->previsione(m.tempo(), m.tempo() + 12.0 * 3600.0)), colori::GRIGIO);
}

void SchermataBriefing::disegna_ordini(Tela& t, const Font& f, const Missione& m) const {
    // gli ordini sopra la carta, sul modulo continuo della telescrivente: il messaggio della serie 0 e le sezioni fisse,
    // a macchina in nero; le righe cadono nelle fasce del modulo
    const int passo = f.righe() + 3;
    const Rettangolo a = lamiera::modulo_continuo(t, mappa_, passo);
    std::vector<std::string> righe;
    const auto aggiungi = [&](const std::string& s) {
        for (auto& l : lamiera::a_capo(f, maiuscolo(s), a.w)) righe.push_back(std::move(l));
    };
    const SalaRadio* r = m.radio();
    const Ricevuto* serie0 = nullptr;
    if (r)
        for (const Ricevuto* x : r->ordini())
            if (x->messaggio.serie == 0) serie0 = x;
    std::string classifica;
    if (serie0) {
        const MessaggioRadio& msg = serie0->messaggio;
        classifica = maiuscolo(msg.classifica);
        aggiungi("ORDINI DI MISSIONE  " + msg.dtg);
        if (!msg.da.empty() || !msg.a.empty()) aggiungi("DA: " + msg.da + "   A: " + msg.a);
        if (!msg.oggetto.empty()) aggiungi("OGG.: " + msg.oggetto);
        righe.emplace_back();
        aggiungi(msg.testo.it.empty() ? msg.testo.en : msg.testo.it);
    } else {
        aggiungi(m.titolo());
    }
    if (r) {
        righe.emplace_back();
        aggiungi("REGOLE D'INGAGGIO");
        const Roe roe = r->roe_ricevute();
        for (int c = 0; c < static_cast<int>(CategoriaRoe::numero); ++c)
            aggiungi(std::string("  ") + nome_categoria(static_cast<CategoriaRoe>(c)) + ": " +
                     nome_stato_roe(roe.stato[static_cast<std::size_t>(c)]));
        righe.emplace_back();
        aggiungi("COMUNICAZIONI");
        aggiungi("  broadcast ogni " + std::to_string(static_cast<int>(r->parametri().ciclo_s / 60.0)) + " minuti; prima finestra alle " +
                 ora_dtg(r->dtg(r->prossima_finestra(m.tempo()))));
    }
    // a pagine: l'ultima riga del modulo e' per «(SEGUE)» e il numero della pagina
    const int per_pagina = std::max(1, a.h / passo - 1);
    pagine_ = std::max(1, (static_cast<int>(righe.size()) + per_pagina - 1) / per_pagina);
    const int pagina = std::clamp(pagina_, 0, pagine_ - 1);
    if (!classifica.empty()) lamiera::timbro(t, f, a.x + a.w - f.larghezza(classifica, 1) - 12, a.y - 2, classifica, colori::ROSSO);
    // la telescrivente: i caratteri arrivano a 90 al secondo (Maiuscolo per accelerare), dalla prima pagina
    std::size_t da_mostrare = static_cast<std::size_t>(tempo_ * 90.0);
    for (int i = 0; i < pagina * per_pagina && i < static_cast<int>(righe.size()); ++i)
        da_mostrare -= std::min(da_mostrare, righe[static_cast<std::size_t>(i)].size());
    int y = a.y;
    for (int i = pagina * per_pagina; i < std::min(static_cast<int>(righe.size()), (pagina + 1) * per_pagina); ++i) {
        std::string s = righe[static_cast<std::size_t>(i)];
        const std::size_t intera = s.size();
        if (s.size() > da_mostrare) s.resize(da_mostrare);
        da_mostrare -= std::min(da_mostrare, intera);
        f.scrivi(t, a.x, y, s, colori::NERO, 1);
        y += passo;
        if (s.size() < intera) break;
    }
    char piede[48];
    std::snprintf(piede, sizeof piede, "%s   %d/%d", pagina + 1 < pagine_ ? "(SEGUE)" : "(FINE)", pagina + 1, pagine_);
    f.scrivi(t, a.x, a.y + per_pagina * passo, piede, colori::BLU, 1);
}

}  // namespace m688
