#include "debriefing.hpp"

#include "nucleo/meteo.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <string>

#include "etichette_carta.hpp"
#include "figurina_evento.hpp"
#include "grafica/cartolina.hpp"
#include "grafica/lamiera.hpp"
#include "grafica/quadro.hpp"
#include "grafica/tema_stazioni.hpp"
#include "grafica/simboli_carta.hpp"
#include "nucleo/radio.hpp"
#include "stazione_radio.hpp"
#include "trascina_carta.hpp"

namespace m688 {

namespace {

constexpr int PIEDE_Y = 428;              // il piede comune: i quattro tasti a 450-476, larghi 150 da x 8, 166, 324, 482
constexpr int SINISTRA_W = 420, RIGA = 13;   // il pannello della carta; la riga della macchina da scrivere
// gli inchiostri sul foglio: il nero della macchina, il blu della penna, il rosso dei timbri, il grigio delle note
constexpr std::uint8_t NERO = 0, PENNA = 1, ROSSO = 4;

std::string maiuscolo(std::string s) {
    for (auto& c : s) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return s;
}

std::string nome_tipo(TipoEvento t) {
    std::string s = maiuscolo(nome_evento(t));
    std::replace(s.begin(), s.end(), '_', ' ');
    return s;
}

// T+HH:MM:SS dall'inizio della missione
std::string orologio(double t) {
    const int s = static_cast<int>(std::max(0.0, t));
    char b[24];
    std::snprintf(b, sizeof b, "T+%02d:%02d:%02d", s / 3600, s / 60 % 60, s % 60);
    return b;
}

std::uint8_t colore_lato(Lato l) {
    switch (l) {
        case Lato::ostile: return colori::ROSSO;
        case Lato::amico: return colori::CIANO_CHIARO;
        case Lato::neutrale: return colori::VERDE_CHIARO;
        default: return colori::GRIGIO;
    }
}
// lo stesso sul foglio bianco: inchiostri scuri
std::uint8_t inchiostro_lato(Lato l) {
    switch (l) {
        case Lato::ostile: return colori::ROSSO;
        case Lato::amico: return colori::BLU;
        case Lato::neutrale: return colori::VERDE;
        default: return colori::NERO;
    }
}

void cornice(Tela& t, int x, int y, int w, int h, std::uint8_t col) {
    t.linea_orizzontale(x, x + w - 1, y, col);
    t.linea_orizzontale(x, x + w - 1, y + h - 1, col);
    t.linea_verticale(x, y, y + h - 1, col);
    t.linea_verticale(x + w - 1, y, y + h - 1, col);
}

// Il testo tagliato alla larghezza, con i puntini
std::string taglia(const Font& f, std::string s, int w) {
    if (f.larghezza(s, 1) <= w) return s;
    while (!s.empty() && f.larghezza(s + "..", 1) > w) s.pop_back();
    return s + "..";
}

}  // namespace

SchermataDebriefing::SchermataDebriefing(const Partita& p, bool urss, StileCarta stile, const Periscopio* periscopio,
                                         const Palette* vista)
    : p_(p), urss_(urss), esito_(p.missione().esito_dettagliato()), periscopio_(vista ? periscopio : nullptr), vista_(vista) {
    carta_.restyle = true;
    carta_.urss = urss;
    carta_.stile = stile;
    carta_.livelli.battello = false;       // la traccia e il segno del battello sono del debriefing
    carta_.livelli.intestazione = false;
    carta_.livelli.intelligence = false;
    carta_.livelli.mobilia = carta_.livelli.fondale;   // la carta nautica intera, come nel briefing: cornice, rosa, nomi
    const Missione& m = p.missione();
    fine_s_ = m.tempo();
    for (const auto& e : m.eventi_missione()) {
        const bool lancio = e.tipo == TipoEvento::lancio_giocatore || e.tipo == TipoEvento::lancio_nemico;
        const int u = lancio ? e.altra : e.unita;   // nei lanci il lato e' del bersaglio
        if (u >= 0 && e.lato != Lato::nessuno) lati_[u] = e.lato;
    }
    // il confronto delle classificazioni: le voci dagli eventi del registro (le «per durata» del motore sono automatiche)
    std::vector<VoceClassificazione> voci;
    for (const EventoMissione& e : m.eventi_missione())
        if (e.tipo == TipoEvento::classificazione) {
            VoceClassificazione v;
            v.contatto = e.unita;
            v.t = e.tempo;
            v.chi = e.visiva ? VoceClassificazione::Chi::visiva
                    : e.valore == 0.0 ? VoceClassificazione::Chi::automatica
                    : e.codice == 411 ? VoceClassificazione::Chi::equipaggio
                                      : VoceClassificazione::Chi::giocatore_sonar;
            v.livello = e.livello;
            v.classe_creduta = e.classe_creduta;
            v.giusta = e.giusta;
            v.classe_vera = e.classe;
            voci.push_back(std::move(v));
        }
    if (ConfrontoClassificazioni c = confronta_classificazioni(voci); c.acustiche + c.visive > 0) confronto_ = std::move(c);
    ricostruisci();
    inquadra();
}

void SchermataDebriefing::ricostruisci() {
    const double t = corrente_ < eventi() ? tempo_corrente() : 1e18;
    const Missione& m = p_.missione();
    std::vector<const EventoMissione*> tutti;
    if (tutti_)
        for (const auto& e : m.eventi_missione()) tutti.push_back(&e);
    else
        tutti = m.momenti_salienti();
    eventi_.clear();
    for (const auto* e : tutti)   // senza la Verita' solo quello che il giocatore sapeva allora
        if (verita_ || e->noto) eventi_.push_back(e);
    corrente_ = eventi();
    for (int i = 0; i < eventi(); ++i)
        if (eventi_[static_cast<std::size_t>(i)]->tempo >= t) {
            corrente_ = i;
            break;
        }
}

void SchermataDebriefing::inquadra() {
    // il riquadro della traccia del battello e degli eventi, poi la scala piu' grande che lo contiene
    double x0 = 1e18, y0 = 1e18, x1 = -1e18, y1 = -1e18;
    const auto allarga = [&](Vec2 q) {
        x0 = std::min(x0, q.x), y0 = std::min(y0, q.y), x1 = std::max(x1, q.x), y1 = std::max(y1, q.y);
    };
    for (const auto& q : p_.tracce())
        if (q.id == p_.id_proprio()) allarga(q.pos);
    for (const auto* e : eventi_) allarga(e->pos);
    if (x0 > x1) {
        if (const auto* g = p_.missione().giocatore()) allarga(g->pos);
        else allarga({});
    }
    centro_ = {(x0 + x1) / 2.0, (y0 + y1) / 2.0};
    const double mpu = p_.missione().metri_per_unita();
    while (carta_.ingrandisci() == 0) {}   // dalla scala piu' grande
    while ((x1 - x0) / (carta_.scala() * mpu / 2.0) > mappa_.w - 40 || (y1 - y0) / (carta_.scala() * mpu / 2.0) > mappa_.h - 40)
        if (carta_.riduci() != 0) break;
    for (const auto& q : p_.tracce()) allarga(q.pos);
    for (const auto& e : p_.missione().eventi_missione()) allarga(e.pos);
    limite_min_ = {x0, y0};
    limite_max_ = {x1, y1};
}

double SchermataDebriefing::tempo_corrente() const {
    return corrente_ < eventi() ? eventi_[static_cast<std::size_t>(corrente_)]->tempo : fine_s_;
}

void SchermataDebriefing::vai(int i) { corrente_ = std::clamp(i, 0, ultima_pagina()); }

double SchermataDebriefing::metri_per_pixel() const { return carta_.scala() * p_.missione().metri_per_unita() / 2.0 / lente_; }

void SchermataDebriefing::piu_vicino() {
    if (carta_.scala() > 2) carta_.ingrandisci();
    else lente_ = std::min(lente_ * 2, 4);
}

void SchermataDebriefing::piu_lontano() {
    if (lente_ > 1) lente_ /= 2;
    else carta_.riduci();
}

bool SchermataDebriefing::comandi(const Comandi& c, double dt) {
    tempo_ += dt;
    if (conferma_) {   // «Riprendi da qui»: Invio riparte dall'evento, Esc torna al debriefing
        if (c.invio) {
            riprendi_ = tempo_corrente();
            return true;
        }
        if (c.esc) conferma_ = false;
        return c.esci;
    }
    const auto chiedi_ripresa = [&] { conferma_ = corrente_ < eventi(); };
    if (std::find(c.lettere.begin(), c.lettere.end(), 'R') != c.lettere.end()) chiedi_ripresa();
    if (std::find(c.lettere.begin(), c.lettere.end(), 'P') != c.lettere.end()) return replay_ = true;   // il replay completo
    if (std::find(c.lettere.begin(), c.lettere.end(), 'S') != c.lettere.end()) return statistiche_ = true;   // le statistiche
    // le frecce: un evento per pressione
    if (const int lato = passi_freccia(c.ruota, c.passi_ruota, prec_ruota_)) vai(corrente_ + lato);
    for (const auto& [x, y] : c.clic) {
        if (y >= PIEDE_Y + 22 && y < PIEDE_Y + 48) {   // i tasti del piede: ESC CHIUDI, R RIPRENDI, P REPLAY, E ESITO
            switch (std::clamp((x - 8) / 158, 0, 3)) {
                case 0: return true;
                case 1: chiedi_ripresa(); break;
                case 2: return replay_ = true;
                default: vai(eventi()); break;
            }
        }
        // un clic su una riga dell'elenco porta a quell'evento
        if (x >= foglio_.x && x < foglio_.x + foglio_.w && lista_visibili_ > 0 && y >= lista_y_ &&
            y < lista_y_ + lista_visibili_ * RIGA && corrente_ < eventi()) {
            const int i = lista_primo_ + (y - lista_y_) / RIGA;
            if (i < eventi()) vai(i);
        }
    }
    if (c.tab) tutti_ = !tutti_, ricostruisci();
    if (c.verita) verita_ = !verita_, ricostruisci();
    if (c.evento) vai(eventi());
    if (c.ingrandisci) piu_vicino();
    if (c.riduci) piu_lontano();
    if (c.rotella && dentro_carta(c, mappa_))   // sopra la carta la rotella, attorno al punto sotto il cursore
        for (int k = std::abs(c.rotella); k > 0; --k) {
            const double prima = metri_per_pixel();
            if (c.rotella > 0) piu_vicino();
            else piu_lontano();
            zoom_sul_cursore(c, mappa_, prima, metri_per_pixel(), centro_);
        }
    trascina_carta(c, mappa_, metri_per_pixel(), presa_, centro_);
    centro_.x = std::clamp(centro_.x, limite_min_.x, limite_max_.x);
    centro_.y = std::clamp(centro_.y, limite_min_.y, limite_max_.y);
    if (c.invio) {   // dall'esito si chiude, come in F-19 (prima le classificazioni e il rapporto ACINT, se ci sono)
        if (corrente_ >= ultima_pagina()) return true;
        vai(corrente_ + 1);
    }
    // la carta va all'evento nuovo se e' fuori dal riquadro; sullo stesso evento resta dove l'ha messa chi guarda
    if (corrente_ < eventi() && corrente_ != corrente_visto_) {
        const Vec2 pos = eventi_[static_cast<std::size_t>(corrente_)]->pos;
        const double mpp = metri_per_pixel();
        if (std::abs(pos.x - centro_.x) / mpp > mappa_.w / 2 - 24 || std::abs(pos.y - centro_.y) / mpp > mappa_.h / 2 - 24)
            centro_ = pos;
    }
    corrente_visto_ = corrente_;
    return c.esc || c.esci;
}

void SchermataDebriefing::imposta_dati(const std::string& dati) {
    try {
        targa_ = Font::carica(dati + "/font/incisione688_7.fnt");
    } catch (const std::exception&) {
        targa_.reset();
    }
    try {
        classi_ = carica_classi(dati + "/classi.toml");
    } catch (const std::exception&) {
        classi_.clear();
    }
}

void SchermataDebriefing::disegna(Tela& t, const Font& f) const {
    const Missione& m = p_.missione();
    t.riempi(colori::NERO);
    const auto oro = static_cast<std::uint8_t>(colori::GIALLO);
    // la consolle con la testata classificata: il timbro a sinistra, il gruppo data-ora dell'evento a destra
    const Font& tg = targa_ ? *targa_ : f;
    const Rettangolo u = quadro::consolle(t, tg, std::string(urss_ ? "RAPPORTO DI CAMPAGNA - " : "DEBRIEFING - ") + maiuscolo(m.titolo()));
    const double tc = tempo_corrente();
    lamiera::testata_classificata(t, tg, urss_ ? "SEKRETNO" : "SECRET", m.radio() ? m.radio()->dtg(tc) : orologio(tc));
    // a sinistra la carta, incassata nel vetro del pannello
    const Rettangolo pl{u.x, u.y, SINISTRA_W, PIEDE_Y - 4 - u.y};
    lamiera::pannello(t, pl, 8);
    const Rettangolo vr{pl.x + 9, pl.y + 9, pl.w - 18, pl.h - 18};
    lamiera::vetro(t, vr);
    mappa_ = {vr.x + 2, vr.y + 2, vr.w - 4, vr.h - 4};
    disegna_carta_tracce(t, f);
    // a destra il rapporto, sul foglio bianco
    foglio_ = lamiera::foglio(t, {u.x + SINISTRA_W + 6, u.y, u.w - SINISTRA_W - 9, PIEDE_Y - 7 - u.y});
    lista_visibili_ = 0;
    if (corrente_ < eventi()) disegna_riquadro(t, f);
    else if (corrente_ == eventi()) disegna_esito(t, f);
    else if (corrente_ == pagina_classificazioni()) disegna_classificazioni(t, f);
    else disegna_acint(t, f);
    // il piede comune: quattro tasti (scelta dell'utente), gli altri comandi nella riga d'aiuto
    const lamiera::Stato riprendi = corrente_ < eventi() ? lamiera::Stato::normale : lamiera::Stato::disabilitato;
    lamiera::piede(t, f, PIEDE_Y,
                   std::string("FRECCE EVENTO   TAB ") + (tutti_ ? "SALIENTI" : "TUTTI") + "   V " + (verita_ ? "QUADRO" : "VERITA'") +
                       "   I O ZOOM   S STATISTICHE   INVIO AVANTI",
                   {{"ESC  CHIUDI", ColoreLampada::rossa}, {"R  RIPRENDI DA QUI", ColoreLampada::ambra, riprendi},
                    {"P  REPLAY", ColoreLampada::ambra}, {"E  ESITO", ColoreLampada::verde}},
                   3);
    const auto ombra = [&](int x, int y, const std::string& s, std::uint8_t col) {
        f.scrivi(t, x + 1, y + 1, s, colori::NERO, 1);
        f.scrivi(t, x, y, s, col, 1);
    };
    if (ripresa_)   // questa partita e' una ripresa: fuori carriera
        ombra(mappa_.x + 6, mappa_.y + 4, "RIPRESA DA " + orologio(*ripresa_) + " - NON CONTA PER LA CARRIERA", oro);
    if (conferma_) {   // la domanda, al centro della carta
        const int w = 380, h = 64, x = mappa_.x + (mappa_.w - w) / 2, y = mappa_.y + (mappa_.h - h) / 2;
        t.rettangolo_pieno(x, y, w, h, colori::NERO);
        cornice(t, x, y, w, h, colori::GIALLO);
        const std::string righe[] = {"RIPRENDERE IL COMANDO DA " + orologio(tempo_corrente()) + "?",
                                     ripresa_ ? "ANCHE LA NUOVA RIPRESA NON CONTA PER LA CARRIERA."
                                              : "LA MISSIONE COSÌ COM'È VA AGLI ATTI; LA RIPRESA NON CONTA.",
                                     "INVIO SÌ      ESC NO"};
        for (int i = 0; i < 3; ++i)
            f.scrivi(t, x + w / 2 - f.larghezza(righe[i], 1) / 2, y + 10 + i * 17, righe[i], i == 2 ? oro : colori::BIANCO, 1);
    }
}

void SchermataDebriefing::disegna_carta_tracce(Tela& t, const Font& f) const {
    const Missione& m = p_.missione();
    VistaCarta v;
    v.x = mappa_.x;
    v.y = mappa_.y;
    v.w = mappa_.w;
    v.h = mappa_.h;
    v.centro = centro_;
    v.metri_per_pixel = metri_per_pixel();
    disegna_carta(t, f, m, carta_, {}, -1, std::nullopt, &v);
    t.imposta_ritaglio(mappa_);

    // le tracce fino all'istante dell'evento: il battello sempre, le altre unita' con la Verita'
    const double tc = tempo_corrente();
    // i simboli del modulo comune (grafica/simboli_carta): colori NTDS e telai APP-6 per parte, il battello nel verde
    // della stazione, le armi e le boe come oggetti; un solo Pennello, quindi un solo alone nero
    namespace sc = simboli;
    const auto parte_di = [this](int id) {
        const auto l = lati_.find(id);
        switch (l == lati_.end() ? Lato::nessuno : l->second) {
            case Lato::amico: return sc::Parte::amico;
            case Lato::ostile: return sc::Parte::ostile;
            case Lato::neutrale: return sc::Parte::neutrale;
            default: return sc::Parte::ignoto;
        }
    };
    std::map<int, Vec2> ultimo, penultimo;
    sc::Pennello pn;
    for (const auto& q : p_.tracce()) {
        if (q.tempo > tc) break;
        const bool proprio = q.id == p_.id_proprio();
        if (!proprio && !verita_) continue;
        const auto it = ultimo.find(q.id);
        if (it != ultimo.end()) {
            const auto u = m.unita().find(q.id);
            const bool oggetto = u != m.unita().end() && sc::e_oggetto(u->second.categoria);
            // la scia nel colore attenuato della parte; le armi tratteggiate nel colore pieno
            const std::uint8_t col = proprio ? sc::VERDE_PROPRIO : sc::indice_parte(parte_di(q.id), !oggetto);
            pn.linea(v.a_tela(it->second), v.a_tela(q.pos), col, oggetto ? 2 : 0);
            penultimo[q.id] = it->second;
        }
        ultimo[q.id] = q.pos;
    }
    for (const auto& [id, pos] : ultimo) {   // dove sono all'istante: il simbolo della parte
        if (id == p_.id_proprio()) continue;
        const Vec2 q = v.a_tela(pos);
        const sc::Parte parte = parte_di(id);
        const auto u = m.unita().find(id);
        if (u == m.unita().end()) {
            sc::disegna_simbolo_unita(pn, q, parte, sc::Dominio::ignoto, sc::indice_parte(parte));
        } else if (sc::e_oggetto(u->second.categoria)) {
            double rotta = u->second.rotta;   // lungo la traccia fino all'istante, se c'e'
            if (const auto pp = penultimo.find(id); pp != penultimo.end() && (pos.x != pp->second.x || pos.y != pp->second.y))
                rotta = gradi(std::atan2(pos.x - pp->second.x, pos.y - pp->second.y));
            sc::disegna_oggetto(pn, q, sc::oggetto_da_classe(u->second.categoria, u->second.classe), sc::indice_parte(parte), rotta);
        } else {
            sc::disegna_simbolo_unita(pn, q, parte, sc::dominio_da_categoria(u->second.categoria, u->second.classe),
                                      sc::indice_parte(parte));
        }
    }
    if (verita_) {   // ASW aereo (A5): le boe posate fino all'istante, nel colore di chi le ha posate; finite, attenuate
        for (const Sonoboa& b : m.boe()) {
            if (b.t_posa > tc) continue;
            sc::disegna_oggetto(pn, v.a_tela(b.pos), sc::Oggetto::boa, sc::indice_parte(parte_di(b.lanciatore), tc >= b.t_fine));
        }
    }
    if (const auto it = ultimo.find(p_.id_proprio()); it != ultimo.end()) sc::disegna_proprio(pn, v.a_tela(it->second));
    pn.stendi(t);

    // gli eventi gia' passati come segnaposto numerati, raccolti se si toccano (app/etichette_carta); quello corrente in
    // giallo con la cornice, fuori dai gruppi e sopra tutti
    const int fino = std::min(corrente_, eventi() - 1);
    const auto segnaposto = [&](int cx, int cy, const std::string& n, bool qui, std::uint8_t bordo) {
        const int w = f.larghezza(n, 1) + 4;
        t.rettangolo_pieno(cx - w / 2, cy - 5, w, ALTEZZA_ETICHETTA, qui ? colori::GIALLO : colori::NERO);
        cornice(t, cx - w / 2, cy - 5, w, ALTEZZA_ETICHETTA, qui ? colori::BIANCO : bordo);
        f.scrivi(t, cx - w / 2 + 2, cy - 3, n, qui ? colori::NERO : colori::BIANCO, 1);
    };
    std::vector<PuntoEtichetta> punti;
    for (int i = 0; i <= fino; ++i) {
        if (i == corrente_) continue;
        const EventoMissione& e = *eventi_[static_cast<std::size_t>(i)];
        const Vec2 q = v.a_tela(e.pos);
        const int x = static_cast<int>(q.x), y = static_cast<int>(q.y);
        if (x >= mappa_.x - 12 && x < mappa_.x + mappa_.w + 12 && y >= mappa_.y - 8 && y < mappa_.y + mappa_.h + 8)   // fuori non raccoglie
            punti.push_back({x, y, i + 1, e.interesse});
    }
    for (const EtichettaCarta& g : raccogli_etichette(punti, [&](const std::string& n) { return f.larghezza(n, 1); }))
        segnaposto(static_cast<int>(g.x), static_cast<int>(g.y), g.testo(), false, colore_lato(eventi_[static_cast<std::size_t>(g.capo - 1)]->lato));
    if (corrente_ < eventi()) {
        const Vec2 q = v.a_tela(eventi_[static_cast<std::size_t>(corrente_)]->pos);
        segnaposto(static_cast<int>(q.x), static_cast<int>(q.y), std::to_string(corrente_ + 1), true, colori::GIALLO);
    }
    t.togli_ritaglio();
    const char* nota = verita_ ? "VERITA': TRACCE VERE DI TUTTE LE UNITA'" : "I O O LA ROTELLA ZOOM, TRASCINA SPOSTA";
    t.rettangolo_pieno(mappa_.x + 2, mappa_.y + mappa_.h - 14, f.larghezza(nota, 1) + 8, 12, colori::NERO);
    f.scrivi(t, mappa_.x + 6, mappa_.y + mappa_.h - 12, nota, verita_ ? colori::CIANO_CHIARO : colori::BIANCO, 1);
}

void SchermataDebriefing::disegna_riquadro(Tela& t, const Font& f) const {
    // sul foglio: l'evento a macchina, la cartolina incollata, la descrizione, poi l'elenco con l'evento corrente
    const EventoMissione& e = *eventi_[static_cast<std::size_t>(corrente_)];
    const int x0 = foglio_.x, w = foglio_.w;
    char riga[96];
    int y = foglio_.y;
    std::snprintf(riga, sizeof riga, "EVENTO %d DI %d", corrente_ + 1, eventi());
    f.scrivi(t, x0, y, riga, NERO, 1);
    f.scrivi(t, x0 + w - f.larghezza(orologio(e.tempo), 1), y, orologio(e.tempo), PENNA, 1);
    t.linea_orizzontale(x0, x0 + w - 1, y + 10, NERO);
    y += RIGA + 3;
    f.scrivi(t, x0, y, taglia(f, nome_tipo(e.tipo), w), inchiostro_lato(e.lato), 1);
    y += RIGA;
    // la minicartolina: la scena dell'evento, l'icona di cio' che non si vede, o quella di riserva (app/figurina_evento)
    const int ch = 80;
    n_colori_ = 0;
    disegna_cartolina_evento(t, f, e, {x0 + 1, y + 1, w - 2, ch - 2});
    cornice(t, x0, y, w, ch, NERO);
    y += ch + 5;
    std::string testo = e.testo_it.empty() ? e.testo_en : e.testo_it;
    if (!e.noto) testo = "(NON NOTO ALLORA) " + testo;
    int righe_testo = 0;
    for (const auto& l : StazioneRadio::a_capo(f, maiuscolo(testo), w)) {
        if (++righe_testo > 4) break;
        f.scrivi(t, x0, y, l, NERO, 1);
        y += RIGA;
    }
    if (e.distanza_m > 0.0) {
        std::snprintf(riga, sizeof riga, "%.1f NM", e.distanza_m / 1852.0);
        lamiera::voce_modulo(t, f, x0, y, w, "DISTANZA", riga, PENNA);
        y += RIGA;
    }
    // l'interesse, a tacche di penna
    f.scrivi(t, x0, y, "INTERESSE", NERO, 1);
    const int bx = x0 + 6 + f.larghezza("INTERESSE", 1), bw = x0 + w - bx;
    cornice(t, bx, y + 1, bw, 6, NERO);
    t.rettangolo_pieno(bx + 1, y + 2, (bw - 2) * std::clamp(e.interesse, 0, 100) / 100, 4, PENNA);
    y += RIGA + 4;
    // l'elenco compatto, con l'evento corrente al centro, in negativo
    f.scrivi(t, x0, y, tutti_ ? "TUTTI GLI EVENTI" : "MOMENTI SALIENTI", NERO, 1);
    t.linea_orizzontale(x0, x0 + w - 1, y + 10, NERO);
    y += RIGA + 2;
    lista_y_ = y;
    lista_visibili_ = std::max(1, (foglio_.y + foglio_.h - y) / RIGA);
    lista_primo_ = std::clamp(corrente_ - lista_visibili_ / 2, 0, std::max(0, eventi() - lista_visibili_));
    for (int i = lista_primo_; i < std::min(eventi(), lista_primo_ + lista_visibili_); ++i) {
        const EventoMissione& x = *eventi_[static_cast<std::size_t>(i)];
        const int yy = lista_y_ + (i - lista_primo_) * RIGA;
        const bool qui = i == corrente_;
        if (qui) t.rettangolo_pieno(x0 - 2, yy - 2, w + 4, RIGA, NERO);
        const int s = static_cast<int>(x.tempo);
        std::snprintf(riga, sizeof riga, "%2d %02d:%02d ", i + 1, s / 3600, s / 60 % 60);
        const int fine = f.scrivi(t, x0, yy, riga, qui ? colori::GIALLO : NERO, 1);
        f.scrivi(t, fine, yy, taglia(f, nome_tipo(x.tipo), x0 + w - fine), qui ? colori::BIANCO : inchiostro_lato(x.lato), 1);
    }
}

Figurina SchermataDebriefing::figurina(const EventoMissione& e) const {
    const Partita::Fotografia* foto = p_.fotografia(e.id);
    const Figurina k = figurina_evento(e, foto ? &foto->mondo : nullptr);
    return k == Figurina::scena && !periscopio_ ? Figurina::riserva : k;
}

void SchermataDebriefing::disegna_cartolina_evento(Tela& t, const Font& f, const EventoMissione& e, Rettangolo r) const {
    Tela piccola(r.w, r.h);
    const Figurina figura = figurina(e);
    if (figura != Figurina::scena) {   // l'icona animata
        disegna_icona(piccola, f, {0, 0, r.w, r.h}, figura == Figurina::icona ? icona_da_nome(e.inquadratura.icona) : icona_di_riserva(e.tipo), tempo_,
                      static_cast<std::uint32_t>(e.id));
    } else {
        const Partita::Fotografia* foto = p_.fotografia(e.id);
        const double quando = foto ? foto->tempo : e.tempo;
        const Effetti* effetti = foto ? &foto->effetti : nullptr;
        if ((!cartolina_ || cartolina_di_ != e.id) && e.tipo == TipoEvento::foto_periscopio) {
            // la foto del periscopio: la vista dall'occhio del battello col rilevamento e l'ingrandimento dello scatto,
            // nell'oculare del periscopio che ha la fotocamera; dal centro della vista, ridotta alla cartolina
            const Mondo mondo = mondo_scena(e, foto ? &foto->mondo : nullptr);
            const Nave* proprio = mondo.trova(p_.id_proprio());
            const PostoPeriscopi* pp = p_.missione().posto_periscopi();
            const DatiPeriscopio* d = nullptr;
            if (pp)
                for (const auto& x : pp->periscopi()) if (x.foto) d = &x;
            if (proprio && d) {
                StatoPeriscopio s;
                s.scelto = d->id;
                s.alzata = 1.0;
                s.su = s.comandato_su = s.testa_fuori = true;
                s.altezza_testa_m = 1.5;
                s.ingrandimento = std::clamp(e.livello, 0, std::max(0, static_cast<int>(d->ingrandimenti.size()) - 1));
                VistaPeriscopio v;
                v.rilevamento = e.valore;
                v.tempo_s = quando;
                v.oculare = parametri_oculare(*d, s);
                Tela grande;
                periscopio_->disegna(grande, mondo, *proprio, v, effetti);
                cartolina_ = std::make_unique<Tela>(r.w, r.h);
                const int sh = ALTEZZA_VISTA * 3 / 4, sw = sh * r.w / std::max(r.h, 1);   // il centro, nelle proporzioni della cartolina
                const int x0 = (LARGHEZZA_TELA - sw) / 2, y0 = (ALTEZZA_VISTA - sh) / 2;
                for (int y = 0; y < r.h; ++y)
                    for (int x = 0; x < r.w; ++x) cartolina_->pixel(x, y, grande.at(x0 + x * sw / r.w, y0 + y * sh / r.h));
                cartolina_di_ = e.id;
            } else {   // nessun occhio da cui guardare
                disegna_icona(piccola, f, {0, 0, r.w, r.h}, icona_di_riserva(e.tipo), tempo_, static_cast<std::uint32_t>(e.id));
            }
        } else if ((!cartolina_ || cartolina_di_ != e.id) && scena_affondamento(e) != ScenaAffondamento::nessuna) {
            const ScenaAffondamento fine = scena_affondamento(e);
            const double lunga = lunghezza_affondamento(classi_, e);
            const Mondo mondo = mondo_scena(e, foto ? &foto->mondo : nullptr, lunga);
            // il punto: dov'e' nella fotografia, o dov'era, se della scena non fa parte
            const int id = soggetto_scena(e);
            const Nave da_posa = nave_da_posa(*posa_evento(e, id));
            const Nave* scafo = nave_in_foto(mondo, id);
            const Nave& luogo = scafo ? *scafo : da_posa;
            const Effetti segni = effetti_affondamento(e, fine, luogo.pos, lunga, mondo);
            const StaccoScena stacco = stacco_affondamento(fine, lunga);
            const PuntoDiVista q = dal_registro(luogo, nullptr, stacco.distanza_m, stacco.elevazione_deg, 70.0, quando);
            cartolina_ = std::make_unique<Tela>(r.w, r.h);
            disegna_cartolina(*cartolina_, {0, 0, r.w, r.h}, *periscopio_, mondo, q, &segni);
            cartolina_di_ = e.id;
        } else if (!cartolina_ || cartolina_di_ != e.id) {   // la scena costa: una volta per evento
            const Mondo mondo = mondo_scena(e, foto ? &foto->mondo : nullptr);
            const int id = soggetto_scena(e);
            const Nave* soggetto = nave_in_foto(mondo, id);
            const StaccoScena stacco = stacco_scena(e);
            const Nave* secondo = secondo_in_campo(mondo, *soggetto, id, e.inquadratura.secondo, stacco.elevazione_deg);
            const PuntoDiVista q = dal_registro(*soggetto, secondo, stacco.distanza_m, stacco.elevazione_deg, e.inquadratura.azimut_deg, quando);
            cartolina_ = std::make_unique<Tela>(r.w, r.h);
            disegna_cartolina(*cartolina_, {0, 0, r.w, r.h}, *periscopio_, mondo, q, effetti);
            cartolina_di_ = e.id;
        }
        if (cartolina_ && cartolina_di_ == e.id) piccola = *cartolina_;
    }
    if (!vista_) {   // senza la palette del giorno (nessun periscopio): gli indici cosi' come sono
        for (int y = 0; y < r.h; ++y)
            for (int x = 0; x < r.w; ++x) t.pixel(r.x + x, r.y + y, piccola.at(x, y));
        return;
    }
    // i colori usati, i piu' frequenti negli indici della cartolina, gli altri sul piu' vicino di quelli
    std::array<int, 256> conto{};
    for (int y = 0; y < r.h; ++y)
        for (int x = 0; x < r.w; ++x) ++conto[piccola.at(x, y)];
    std::vector<int> usati;
    for (int i = 0; i < 256; ++i)
        if (conto[static_cast<std::size_t>(i)] > 0) usati.push_back(i);
    // nella scena dell'affondamento fumo, mare e cielo finirebbero i posti, e i grigi dello scafo cadrebbero sul celeste: prima loro
    const bool grigi_prima = figura == Figurina::scena && scena_affondamento(e) != ScenaAffondamento::nessuna;
    const auto peso = [&](int i) {
        return conto[static_cast<std::size_t>(i)] + (grigi_prima && i >= 144 && i <= 158 ? r.w * r.h : 0);
    };
    std::stable_sort(usati.begin(), usati.end(), [&](int a, int b) { return peso(a) > peso(b); });
    n_colori_ = std::min(static_cast<int>(usati.size()), N_CARTOLINA);
    std::array<std::uint8_t, 256> mappa{};
    for (int k = 0; k < n_colori_; ++k) {
        colori_[static_cast<std::size_t>(k)] = vista_->rgb(usati[static_cast<std::size_t>(k)]);
        mappa[static_cast<std::size_t>(usati[static_cast<std::size_t>(k)])] = static_cast<std::uint8_t>(CARTOLINA + k);
    }
    for (std::size_t u = static_cast<std::size_t>(n_colori_); u < usati.size(); ++u) {
        const auto a = vista_->rgb(usati[u]);
        long migliore = -1;
        for (int k = 0; k < n_colori_; ++k) {
            const auto& b = colori_[static_cast<std::size_t>(k)];
            const long dr = a[0] - b[0], dg = a[1] - b[1], db = a[2] - b[2];
            const long d = 3 * dr * dr + 4 * dg * dg + 2 * db * db;
            if (migliore < 0 || d < migliore) migliore = d, mappa[static_cast<std::size_t>(usati[u])] = static_cast<std::uint8_t>(CARTOLINA + k);
        }
    }
    for (int y = 0; y < r.h; ++y)
        for (int x = 0; x < r.w; ++x) t.pixel(r.x + x, r.y + y, mappa[piccola.at(x, y)]);
}

void SchermataDebriefing::palette(Palette& p) const {
    applica_palette_carta(p, carta_.stile);
    applica_palette_rilievo(p, carta_.stile);
    TemaStazione tema;   // 16-47: la lamiera e le spie (quella dell'Alfa per il comandante sovietico)
    tema.battello = urss_ ? BattelloTema::alfa : BattelloTema::usa688;
    applica_tema(p, tema);
    for (int k = 0; k < n_colori_; ++k) {
        const auto& c = colori_[static_cast<std::size_t>(k)];
        p.imposta(CARTOLINA + k, c[0], c[1], c[2]);
    }
}

// Il rapporto ACINT (S5d, nucleo/acint.hpp): per ogni contatto registrato dal sonar la classificazione dalle righe stabili,
// le pale e le righe che la libreria non spiegava (le novita' da passare all'intelligence)
void SchermataDebriefing::disegna_acint(Tela& t, const Font& f) const {
    const int x0 = foglio_.x, w = foglio_.w, fondo = foglio_.y + foglio_.h - RIGA;
    int y = foglio_.y;
    const auto scrivi = [&](const std::string& s, std::uint8_t col) {
        for (const auto& r : StazioneRadio::a_capo(f, maiuscolo(s), w)) {
            if (y > fondo - RIGA) return;
            f.scrivi(t, x0, y, r, col, 1);
            y += RIGA;
        }
    };
    f.scrivi(t, x0, y, urss_ ? "RAPPORTO ACINT (GRU)" : "ACINT REPORT", NERO, 1);
    t.linea_orizzontale(x0, x0 + w - 1, y + 10, NERO);
    y += RIGA + 6;
    const auto& testo = acint_->testo_it;
    for (std::size_t i = 0; i < testo.size(); ++i) {
        scrivi(testo[i], i == 0 ? ROSSO : NERO);
        y += 3;
    }
    if (acint_->voci.empty()) scrivi("NESSUN CONTATTO REGISTRATO A LUNGO", NERO);
    f.scrivi(t, x0, fondo, "INVIO CHIUDE", PENNA, 1);
}

void SchermataDebriefing::disegna_esito(Tela& t, const Font& f) const {
    // il rapporto di missione dattiloscritto: il timbro dell'esito, le voci a puntini coi valori a penna, le caselle
    // degli obiettivi
    const int x0 = foglio_.x, w = foglio_.w;
    const int fondo = foglio_.y + foglio_.h - (codice_.empty() ? 1 : 2) * RIGA;
    int y = foglio_.y;
    char riga[96];
    const auto scrivi = [&](const std::string& s, std::uint8_t col, int rientro = 0) {
        const auto righe = StazioneRadio::a_capo(f, maiuscolo(s), w - rientro);
        for (std::size_t i = 0; i < righe.size(); ++i) {
            if (y + RIGA > fondo) return;
            const bool ultima = y + 2 * RIGA > fondo && i + 1 < righe.size();   // il resto non entra: i puntini
            f.scrivi(t, x0 + rientro, y, ultima ? taglia(f, righe[i] + " ...", w - rientro) : righe[i], col, 1);
            y += RIGA;
        }
    };
    const auto voce = [&](const std::string& etichetta, const std::string& valore, std::uint8_t penna = PENNA) {
        if (y + RIGA > fondo) return;
        lamiera::voce_modulo(t, f, x0, y, w, etichetta, valore, penna);
        y += RIGA;
    };
    const auto titolo = [&](const char* s) {
        if (y + RIGA > fondo) return;
        f.scrivi(t, x0, y, s, NERO, 1);
        t.linea_orizzontale(x0, x0 + f.larghezza(s, 1), y + 10, NERO);
        y += RIGA + 1;
    };
    f.scrivi(t, x0, y, urss_ ? "RAPPORTO DI CAMPAGNA" : "RAPPORTO DI MISSIONE", NERO, 1);
    t.linea_orizzontale(x0, x0 + w - 1, y + 10, NERO);
    y += RIGA + 6;
    // l'esito, col timbro: verde il successo, marrone il parziale o il richiamo, rosso il resto
    using C = EsitoDettagliato::Categoria;
    const C cat = esito_.categoria;
    const std::uint8_t col_cat = cat == C::successo                                   ? colori::VERDE
                                 : cat == C::successo_parziale || cat == C::richiamato ? colori::MARRONE
                                                                                        : ROSSO;
    std::string nome = nome_categoria_esito(cat);
    std::replace(nome.begin(), nome.end(), '_', ' ');
    nome = maiuscolo(nome);
    f.scrivi(t, x0, y + 2, "ESITO", NERO, 1);
    lamiera::timbro(t, f, x0 + 40, y - 2, taglia(f, nome, w - 52), col_cat);
    y += RIGA + 7;
    // il font non ha il segno di percentuale
    std::snprintf(riga, sizeof riga, "%d PER CENTO", static_cast<int>(std::lround(esito_.completezza * 100.0)));
    voce("COMPLETEZZA", riga);
    voce("DURATA", orologio(esito_.durata_s));
    if (p_.missione().meteo()) {   // il meteo a fine missione (Enhanced, docs/15 M4)
        const StatoMeteo& s = p_.missione().meteo_qui();
        std::snprintf(riga, sizeof riga, "MARE %d, VIS. %.0f KM%s", s.stato_mare(), s.visibilita_m / 1000.0,
                      s.lampi_al_minuto > 0.0 ? ", TEMP." : s.pioggia_mm_h > 0.5 ? ", PIOGGIA" : s.nebbia != Nebbia::nessuna ? ", NEBBIA" : "");
        voce("METEO", riga);
    }
    y += 4;
    titolo("OBIETTIVI");
    for (const auto& o : esito_.obiettivi) {
        using S = Obiettivo::Stato;
        if (y + RIGA > fondo) break;
        // la casella: la X se raggiunto; il fallito in rosso, il parziale o in corso col conto a penna
        lamiera::casella(t, x0, y, o.stato == S::raggiunto, PENNA);
        std::string s = (o.testo_it.empty() ? o.testo_en : o.testo_it);
        if (o.richiesti > 1) s += ": " + std::to_string(o.fatti) + " SU " + std::to_string(o.richiesti);
        scrivi(s, o.stato == S::fallito ? ROSSO : o.primario ? NERO : colori::GRIGIO_SCURO, 13);
    }
    y += 4;
    titolo("ARMI E BERSAGLI");
    voce("LANCI / A SEGNO", std::to_string(esito_.lanci) + " / " + std::to_string(esito_.lanci_a_segno));
    voce("AFFONDATI: OSTILI", std::to_string(esito_.affondati_ostili));
    voce("NEUTRALI / AMICI", std::to_string(esito_.affondati_neutrali) + " / " + std::to_string(esito_.affondati_amici),
         esito_.affondati_neutrali + esito_.affondati_amici > 0 ? ROSSO : PENNA);
    voce("SILURI EVITATI", std::to_string(esito_.siluri_evitati));
    voce("COLPI SUBITI", std::to_string(esito_.colpi_subiti), esito_.colpi_subiti > 0 ? ROSSO : PENNA);
    voce("CLASSIFICAZIONI GIUSTE", std::to_string(esito_.classificazioni_giuste));
    voce("ERRATE", std::to_string(esito_.classificazioni_errate));
    if (esito_.danno_massimo > 0.0) {
        std::snprintf(riga, sizeof riga, "%d PER CENTO", static_cast<int>(std::lround(esito_.danno_massimo * 100.0)));
        voce("DANNO MASSIMO", riga, ROSSO);
    }
    if (!esito_.ordini_rispettati) scrivi("ORDINI NON RISPETTATI", ROSSO);
    if (!esito_.testo_it.empty()) {   // il messaggio del comando, con quello che resta del foglio
        y += 4;
        scrivi(esito_.testo_it, PENNA);
    }
    const int ultima = foglio_.y + foglio_.h - RIGA;
    if (!codice_.empty()) f.scrivi(t, x0, ultima - RIGA, "CODICE " + codice_, NERO, 1);
    f.scrivi(t, x0, ultima, confronto_ ? "INVIO: CLASSIFICAZIONI" : acint_ ? "INVIO: RAPPORTO ACINT" : "INVIO CHIUDE", PENNA, 1);
}

// Le classificazioni della missione (docs/25 §3.2): il sonar (equipaggio o giocatore) contro la vista al periscopio, con
// la verita'. I totali con le percentuali, chi e' arrivato prima, poi per contatto la classe vera e le due credute
void SchermataDebriefing::disegna_classificazioni(Tela& t, const Font& f) const {
    const ConfrontoClassificazioni& c = *confronto_;
    const int x0 = foglio_.x, w = foglio_.w, fondo = foglio_.y + foglio_.h - RIGA;
    int y = foglio_.y;
    char riga[96];
    f.scrivi(t, x0, y, urss_ ? "CLASSIFICAZIONI: GIDROAKUSTIKA E VISTA" : "CLASSIFICAZIONI: SONAR E VISTA", NERO, 1);
    t.linea_orizzontale(x0, x0 + w - 1, y + 10, NERO);
    y += RIGA + 6;
    const auto voce = [&](const std::string& etichetta, const std::string& valore, std::uint8_t penna = PENNA) {
        if (y + RIGA > fondo) return;
        lamiera::voce_modulo(t, f, x0, y, w, etichetta, valore, penna);
        y += RIGA;
    };
    // il font non ha il segno di percentuale
    std::snprintf(riga, sizeof riga, "%d, %d PER CENTO", c.acustiche, static_cast<int>(std::lround(c.percento_acustica())));
    voce("SONAR", c.acustiche ? riga : "NESSUNA");
    std::snprintf(riga, sizeof riga, "%d, %d PER CENTO", c.visive, static_cast<int>(std::lround(c.percento_visiva())));
    voce("VISTA", c.visive ? riga : "NESSUNA");
    voce("PRIMA GIUSTA", "SONAR " + std::to_string(c.prima_acustica) + ", VISTA " + std::to_string(c.prima_visiva));
    if (c.automatiche) voce("AUTOMATICHE", std::to_string(c.automatiche), colori::GRIGIO_SCURO);
    y += 6;
    // per contatto: la classe vera, poi S e V con la credute e l'esito (giusta, della famiglia, solo la categoria...)
    const auto nome = [](std::string k) {
        std::replace(k.begin(), k.end(), '_', ' ');
        return maiuscolo(k);
    };
    const auto parte = [&](const char* chi, const ConfrontoContatto::Parte& p) {
        if (y + RIGA > fondo) return;
        using E = EsitoClassificazione;
        const std::uint8_t col = p.esito == E::giusta ? colori::VERDE : p.esito == E::sbagliata ? ROSSO
                                 : p.esito == E::assente ? colori::GRIGIO_SCURO : colori::MARRONE;
        const std::string s = p.esito == E::assente ? std::string(chi) + " -"
                                                    : std::string(chi) + " " + nome(p.classe.empty() ? "categoria" : p.classe) + ": " + maiuscolo(nome_esito(p.esito));
        f.scrivi(t, x0 + 8, y, taglia(f, s, w - 8), col, 1);
        y += RIGA;
    };
    int fatti = 0;
    for (const ConfrontoContatto& k : c.contatti) {
        if (y + 3 * RIGA > fondo) break;
        const char* primo = k.prima == ConfrontoContatto::Prima::acustica ? "  (PRIMA IL SONAR)"
                            : k.prima == ConfrontoContatto::Prima::visiva  ? "  (PRIMA LA VISTA)" : "";
        f.scrivi(t, x0, y, taglia(f, nome(k.classe_vera) + primo, w), NERO, 1);
        y += RIGA;
        parte("S", k.acustica);
        parte("V", k.visiva);
        y += 2;
        ++fatti;
    }
    if (fatti < static_cast<int>(c.contatti.size()))
        f.scrivi(t, x0, fondo - RIGA, "E ALTRI " + std::to_string(static_cast<int>(c.contatti.size()) - fatti), NERO, 1);
    f.scrivi(t, x0, fondo, acint_ ? "INVIO: RAPPORTO ACINT" : "INVIO CHIUDE", PENNA, 1);
}

}  // namespace m688
