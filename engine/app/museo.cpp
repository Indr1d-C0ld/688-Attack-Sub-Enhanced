#include "museo.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <filesystem>

#include "carta.hpp"
#include "grafica/cascata_firma.hpp"
#include "grafica/effetti.hpp"
#include "grafica/lamiera.hpp"
#include "grafica/quadro.hpp"
#include "grafica/tema_stazioni.hpp"
#include "grafica/modello.hpp"
#include "grafica/periscopio.hpp"
#include "grafica/riscala.hpp"
#include "grafica/sagome_armi.hpp"
#include "grafica/spaccato.hpp"
#include "nucleo/acustica.hpp"
#include "nucleo/asw_aereo.hpp"
#include "nucleo/carico.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/geometria.hpp"
#include "nucleo/periscopi.hpp"
#include "stazione_radio.hpp"

namespace m688 {

namespace {

constexpr int PIEDE_Y = 428;   // il piede comune: i tre tasti a 450-476, larghi 202 da x 8, 218, 428
constexpr int ELENCO_W = 196, PASSO_ELENCO = 14, PASSO_TESTO = 11;

// lo schedario: gli stessi rettangoli per il disegno e per i clic
constexpr Rettangolo SCHEDARIO{6, 25, ELENCO_W, PIEDE_Y - 4 - 25};
constexpr Rettangolo SEZIONE_PREC{SCHEDARIO.x + 8, SCHEDARIO.y + 8, 22, 17};
constexpr Rettangolo SEZIONE_SUCC{SCHEDARIO.x + SCHEDARIO.w - 30, SCHEDARIO.y + 8, 22, 17};
constexpr Rettangolo NOME_SEZIONE{SEZIONE_PREC.x + SEZIONE_PREC.w, SCHEDARIO.y + 8, SEZIONE_SUCC.x - SEZIONE_PREC.x - SEZIONE_PREC.w, 17};
constexpr Rettangolo VETRO_ELENCO{SCHEDARIO.x + 8, SCHEDARIO.y + 46, SCHEDARIO.w - 16, SCHEDARIO.h - 54};

bool dentro(const Rettangolo& r, int x, int y) { return x >= r.x && y >= r.y && x < r.x + r.w && y < r.y + r.h; }

int righe_in_vista() { return std::max(1, (VETRO_ELENCO.h - 4) / PASSO_ELENCO); }

// la prima riga in vista: quella scelta sta a meta' del vetro, finche' l'elenco lo permette
int prima_in_vista(int scelta, int tot) { return std::clamp(scelta - righe_in_vista() / 2, 0, std::max(0, tot - righe_in_vista())); }

// la riga sotto la y di un clic nel vetro, -1 se li' non ce n'e'
int riga_sotto(int y, int scelta, int tot) {
    const int k = (y - (VETRO_ELENCO.y + 2)) / PASSO_ELENCO, i = prima_in_vista(scelta, tot) + k;
    return y >= VETRO_ELENCO.y + 2 && k < righe_in_vista() && i < tot ? i : -1;
}

// l'ordine delle sezioni del Museo per categoria; le altre in fondo, come vengono
constexpr const char* ORDINE[] = {"sottomarino", "nave", "aereo", "elicottero", "siluro", "missile", "mina", "esca",
                                  "boa", "marcatore", "sonar", "radar", "esm", "periscopio", "sistema", "bersaglio",
                                  "teatro", "base"};

// Maiuscole, e i caratteri che il font del gioco non ha con un equivalente: virgolette, apici, segni matematici
std::string maiuscolo(std::string s) { return testo_per_font(std::move(s)); }

std::string stem(const std::string& f) { return std::filesystem::path(f).stem().string(); }

}  // namespace

SchermataMuseo::SchermataMuseo(const Enciclopedia& e, const Risorse& r, std::string dati, const std::string& voce)
    : e_(e), r_(r), dati_(std::move(dati)) {
    try {
        targa_ = Font::carica(dati_ + "/font/incisione688_7.fnt");
    } catch (const std::exception&) {
        targa_.reset();
    }
    std::vector<std::pair<int, std::string>> s;
    for (const auto& g : e_.gruppi) {
        const auto voci = e_.del_gruppo(g);
        if (voci.empty()) continue;
        const std::string cat = voci.front()->categoria;
        int k = static_cast<int>(std::size(ORDINE));
        for (int i = 0; i < static_cast<int>(std::size(ORDINE)); ++i)
            if (cat == ORDINE[i]) k = i;
        s.emplace_back(k, g);
    }
    std::stable_sort(s.begin(), s.end(), [](const auto& a, const auto& b) { return a.first < b.first; });
    for (const auto& [k, g] : s) sezioni_.push_back(g);
    if (!voce.empty()) vai(voce);
}

const std::vector<const VoceEnciclopedia*>& SchermataMuseo::voci_sezione() const {
    if (elenco_di_ != sezione_) {
        elenco_ = sezioni_.empty() ? std::vector<const VoceEnciclopedia*>{} : e_.del_gruppo(sezioni_[static_cast<std::size_t>(sezione_)]);
        elenco_di_ = sezione_;
    }
    return elenco_;
}

const VoceEnciclopedia* SchermataMuseo::voce() const {
    const auto& v = voci_sezione();
    return riga_ >= 0 && riga_ < static_cast<int>(v.size()) ? v[static_cast<std::size_t>(riga_)] : nullptr;
}

void SchermataMuseo::vai(const std::string& id) {
    const VoceEnciclopedia* x = e_.voce(id);
    if (!x) return;
    for (int s = 0; s < static_cast<int>(sezioni_.size()); ++s)
        if (sezioni_[static_cast<std::size_t>(s)] == x->gruppo) {
            sezione_ = s;
            const auto& v = voci_sezione();
            for (int i = 0; i < static_cast<int>(v.size()); ++i)
                if (v[static_cast<std::size_t>(i)] == x) riga_ = i;
        }
    immagine_ = scorri_ = 0;
}

void SchermataMuseo::apri_sezione(int s) {
    const int n = quante_sezioni();
    sezione_ = riga_indice_ = (s % n + n) % n;
    riga_ = immagine_ = scorri_ = 0;
}

std::string SchermataMuseo::nome_sezione(int s) const {
    const std::string& g = sezioni_[static_cast<std::size_t>(s)];
    if (inglese_)
        if (const auto v = e_.del_gruppo(g); !v.empty() && !v.front()->gruppo_en.empty()) return maiuscolo(v.front()->gruppo_en);
    return maiuscolo(g);
}

bool SchermataMuseo::comandi(const Comandi& c, double dt) {
    if (sezioni_.empty()) return c.esc || c.esci;
    // sezione: sinistra e destra, una per pressione, in giro
    if (c.passi_ruota != 0) apri_sezione(sezione_ + c.passi_ruota);
    // su e giu', la voce o la riga dell'indice: una per pressione, e la ripetizione tenendo premuto
    int passi = c.passi_su_giu;
    if (passi != 0) {
        ripeti_ = 0.35;
    } else if (c.su_giu != 0 && c.su_giu == prec_su_giu_) {
        ripeti_ -= dt;
        if (ripeti_ <= 0.0) {
            passi = c.su_giu;
            ripeti_ = 0.08;
        }
    }
    prec_su_giu_ = c.su_giu;
    if (passi != 0 && indice_) {
        riga_indice_ = std::clamp(riga_indice_ - passi, 0, quante_sezioni() - 1);   // su: verso l'alto dell'elenco
    } else if (passi != 0) {
        riga_ = std::clamp(riga_ - passi, 0, std::max(0, static_cast<int>(voci_sezione().size()) - 1));
        immagine_ = scorri_ = 0;
    }
    // Invio apre l'indice delle sezioni e, dall'indice, la sezione scelta
    if (c.invio) {
        if (indice_) apri_sezione(riga_indice_);
        else riga_indice_ = sezione_;
        indice_ = !indice_;
    }
    for (const auto& [x, y] : c.clic) {   // lo schedario: le due frecce, il nome che apre l'indice, le righe
        if (dentro(SEZIONE_PREC, x, y)) {
            apri_sezione(sezione_ - 1);
        } else if (dentro(SEZIONE_SUCC, x, y)) {
            apri_sezione(sezione_ + 1);
        } else if (dentro(NOME_SEZIONE, x, y)) {
            riga_indice_ = sezione_;
            indice_ = !indice_;
        } else if (dentro(VETRO_ELENCO, x, y) && indice_) {
            if (const int i = riga_sotto(y, riga_indice_, quante_sezioni()); i >= 0) {
                apri_sezione(i);
                indice_ = false;
            }
        } else if (dentro(VETRO_ELENCO, x, y)) {
            if (const int i = riga_sotto(y, riga_, static_cast<int>(voci_sezione().size())); i >= 0) {
                riga_ = i;
                immagine_ = scorri_ = 0;
            }
        }
    }
    const VoceEnciclopedia* v = voce();
    if (c.tab && v && !v->immagini.empty())
        immagine_ = (immagine_ + c.tab + static_cast<int>(v->immagini.size())) % static_cast<int>(v->immagini.size());
    if (c.compressione) scorri_ = std::clamp(scorri_ + (c.compressione > 0 ? -1 : 1), 0, pagine_testo_ - 1);   // Pag su e giu': una pagina del testo
    for (const auto& [x, y] : c.clic)   // i tre tasti del piede: ESC, TAB l'immagine, PAG il testo che segue
        if (y >= PIEDE_Y + 22 && y < PIEDE_Y + 48) {
            if (x < 214 && indice_) {
                indice_ = false;
                continue;
            }
            if (x < 214) return storia_.empty() ? true : (vai(storia_.back()), storia_.pop_back(), false);
            if (x < 424 && v && !v->immagini.empty()) immagine_ = (immagine_ + 1) % static_cast<int>(v->immagini.size());
            if (x >= 424) scorri_ = scorri_ + 1 >= pagine_testo_ ? 0 : scorri_ + 1;
        }
    if (c.lancia) inglese_ = !inglese_;   // L: la lingua
    for (const int k : c.cifre)           // 1-9: il riferimento
        if (v && k >= 1 && k <= static_cast<int>(v->riferimenti.size()) && e_.voce(v->riferimenti[static_cast<std::size_t>(k - 1)].voce)) {
            storia_.push_back(v->id);
            vai(v->riferimenti[static_cast<std::size_t>(k - 1)].voce);
            break;
        }
    if (c.esc) {
        if (indice_) {
            indice_ = false;
            return c.esci;
        }
        if (storia_.empty()) return true;
        const std::string id = storia_.back();
        storia_.pop_back();
        vai(id);
    }
    return c.esci;
}

// Le scale dei tre disegni usano gli stessi indici alti (96-143 e 199-247): vale quella dell'immagine in vista
void SchermataMuseo::palette(Palette& p) const {
    oculare::palette(p, oculare_);
    if (tipo_mostrato_ == "cascata") applica_palette_cascata(p);
    else if (tipo_mostrato_ == "carta") applica_palette_carta(p, StileCarta::vga);
    else if (tipo_mostrato_ == "spaccato") Spaccato::imposta_palette(p);
}

void SchermataMuseo::disegna(Tela& t, const Font& f) const {
    // gruppo 4 della passata estetica: la lamiera di A coi documenti di B. Prima la cornice, poi la rimappatura nei colori
    // della vista (le immagini usano anche 16-79), infine l'immagine
    t.riempi(colori::NERO);
    oculare_ = ParametriOculare{};
    tipo_mostrato_.clear();
    const Rettangolo u = quadro::consolle(t, targa_ ? *targa_ : f,
                                          inglese_ ? "MUSEUM - ENCYCLOPEDIA OF THE SIMULATION" : "MUSEO - ENCICLOPEDIA DELLA SIMULAZIONE");
    if (sezioni_.empty()) {
        f.scrivi(t, u.x + 8, u.y + 8, "ENCICLOPEDIA NON TROVATA (DATA/MOTORE/ENCICLOPEDIA.TOML)", colori::ROSSO, 1);
        return;
    }
    disegna_elenco(t, f);
    const VoceEnciclopedia* v = voce();
    if (v) disegna_voce(t, f, *v);
    // il piede comune: PAG predefinito quando il testo continua
    const int k_imm = v && !v->immagini.empty() ? std::clamp(immagine_, 0, static_cast<int>(v->immagini.size()) - 1) + 1 : 0;
    const int n_imm = v ? static_cast<int>(v->immagini.size()) : 0;
    const int pag = std::clamp(scorri_, 0, pagine_testo_ - 1) + 1;
    lamiera::piede(t, f, PIEDE_Y,
                   indice_ ? (inglese_ ? "UP DOWN SECTION   ENTER OR CLICK OPENS IT   ESC CLOSES THE INDEX"
                                       : "SU GIÙ SEZIONE   INVIO O CLIC LA APRE   ESC CHIUDE L'INDICE")
                           : (inglese_ ? "UP DOWN ENTRY   LEFT RIGHT SECTION   ENTER INDEX   1-9 SEE   L LANGUAGE"
                                       : "SU GIÙ VOCE   SINISTRA DESTRA SEZIONE   INVIO INDICE   1-9 VEDI   L LINGUA"),
                   {{inglese_ ? "ESC  BACK" : "ESC  INDIETRO", ColoreLampada::rossa},
                    {(inglese_ ? "TAB  IMAGE " : "TAB  IMMAGINE ") + std::to_string(k_imm) + "/" + std::to_string(n_imm), ColoreLampada::ambra,
                     n_imm > 1 ? lamiera::Stato::normale : lamiera::Stato::disabilitato},
                    {(inglese_ ? "PAG  MORE " : "PAG  SEGUE ") + std::to_string(pag) + "/" + std::to_string(pagine_testo_), ColoreLampada::verde,
                     pagine_testo_ > 1 ? lamiera::Stato::normale : lamiera::Stato::disabilitato}},
                   pag < pagine_testo_ ? 2 : 0);
    // la lamiera nei grigi e negli EGA della vista, poi l'immagine coi suoi colori
    if (!col_tema_) {
        col_tema_ = r_.giorno;
        applica_tema(*col_tema_, TemaStazione{});
    }
    lamiera::in_colori_vista(t, {0, 0, LARGHEZZA_TELA, ALTEZZA_TELA}, *col_tema_);
    if (v && !v->immagini.empty())
        disegna_immagine(t, f, v->immagini[static_cast<std::size_t>(k_imm - 1)], riquadro_immagine_);
}

void SchermataMuseo::disegna_elenco(Tela& t, const Font& f) const {
    // lo schedario: la sezione fra due frecce, l'indice nel vetro con la voce scelta sulla lampada ambra
    const auto& v = voci_sezione();
    const Rettangolo& pl = SCHEDARIO;
    const Rettangolo& ev = VETRO_ELENCO;
    lamiera::pannello(t, pl, 8);
    lamiera::tasto(t, SEZIONE_PREC, "<", ColoreLampada::ambra, lamiera::Stato::normale, false, f);
    lamiera::tasto(t, SEZIONE_SUCC, ">", ColoreLampada::ambra, lamiera::Stato::normale, false, f);
    std::string nome_sez = nome_sezione(sezione_);
    while (!nome_sez.empty() && f.larghezza(nome_sez, 1) > pl.w - 76) nome_sez.pop_back();
    lamiera::incisa(t, f, pl.x + pl.w / 2 - f.larghezza(nome_sez, 1) / 2, pl.y + 13, nome_sez, colori::BIANCO);
    char riga[96];
    if (indice_) std::snprintf(riga, sizeof riga, inglese_ ? "INDEX OF THE %zu SECTIONS" : "INDICE DELLE %zu SEZIONI", sezioni_.size());
    else std::snprintf(riga, sizeof riga, inglese_ ? "SECTION %d OF %zu, %d ENTRIES" : "SEZIONE %d DI %zu, %d VOCI", sezione_ + 1, sezioni_.size(),
                       static_cast<int>(v.size()));
    lamiera::secondaria(t, f, pl.x + 10, pl.y + 32, riga);
    std::vector<std::string> nomi;
    const auto aggiungi = [&](std::string nome) {
        while (!nome.empty() && f.larghezza(nome, 1) > ev.w - 18) {   // una lettera intera, anche accentata
            while (nome.size() > 1 && (static_cast<unsigned char>(nome.back()) & 0xC0) == 0x80) nome.pop_back();
            nome.pop_back();
        }
        nomi.push_back(std::move(nome));
    };
    if (indice_) {
        for (int s = 0; s < quante_sezioni(); ++s) aggiungi(nome_sezione(s));
    } else {
        for (const VoceEnciclopedia* x : v) aggiungi(maiuscolo(x->nome_mostrato(inglese_)));
    }
    const int scelta = indice_ ? riga_indice_ : riga_;
    lamiera::elenco_vetro(t, ev, f, nomi, scelta, PASSO_ELENCO, prima_in_vista(scelta, static_cast<int>(nomi.size())));
}

void SchermataMuseo::disegna_voce(Tela& t, const Font& f, const VoceEnciclopedia& v) const {
    const auto tr = [&](const std::string& it, const std::string& en) -> const std::string& { return inglese_ && !en.empty() ? en : it; };
    const Rettangolo pr{6 + ELENCO_W + 6, 25, LARGHEZZA_TELA - 12 - ELENCO_W - 6, PIEDE_Y - 4 - 25};
    lamiera::pannello(t, pr, 8);
    lamiera::incisa(t, targa_ ? *targa_ : f, pr.x + 10, pr.y + 8, maiuscolo(v.nome_mostrato(inglese_)), colori::BIANCO);
    lamiera::secondaria(t, f, pr.x + 10, pr.y + 22, maiuscolo(tr(v.gruppo, v.gruppo_en) + " - " + v.nazione));
    // l'immagine nel vetro (la disegna disegna(), dopo la rimappatura), la didascalia, TAB e il numero
    const Rettangolo iv{pr.x + 8, pr.y + 34, 240, 132};
    lamiera::vetro(t, iv);
    riquadro_immagine_ = {iv.x + 2, iv.y + 2, iv.w - 4, iv.h - 4};
    t.rettangolo_pieno(riquadro_immagine_.x, riquadro_immagine_.y, riquadro_immagine_.w, riquadro_immagine_.h, colori::BLU);
    if (!v.immagini.empty()) {
        const int k = std::clamp(immagine_, 0, static_cast<int>(v.immagini.size()) - 1);
        const ImmagineVoce& im = v.immagini[static_cast<std::size_t>(k)];
        std::string did = maiuscolo(tr(im.didascalia_it, im.didascalia_en));
        if (did.empty() && im.tipo == "foto")
            did = maiuscolo((inglese_ ? "PHOTO: " : "FOTO: ") + im.testo("autore") + " (" + im.testo("licenza") + ")");
        while (!did.empty() && f.larghezza(did, 1) > iv.w - 62) did.pop_back();
        lamiera::secondaria(t, f, iv.x, iv.y + iv.h + 5, did);
        lamiera::tasto_tastiera(t, {iv.x + iv.w - 54, iv.y + iv.h + 3, 26, 12}, "TAB", f);
        lamiera::incisa(t, f, iv.x + iv.w - 24, iv.y + iv.h + 5, std::to_string(k + 1) + "/" + std::to_string(v.immagini.size()), colori::BIANCO);
    }
    // la scheda tecnica battuta a macchina su un cartellino, i valori a penna (le STIME con l'asterisco)
    const Rettangolo fs = lamiera::foglio(t, {iv.x + iv.w + 6, iv.y, pr.x + pr.w - 8 - (iv.x + iv.w + 6), iv.h + 16});
    int y = fs.y;
    f.scrivi(t, fs.x, y, inglese_ ? "DATA SHEET" : "SCHEDA TECNICA", colori::NERO, 1);
    y += 14;
    int voci_scheda = 0;
    for (const auto& r : v.scheda) {
        if (voci_scheda >= 8 || y + 10 > fs.y + fs.h) break;
        std::string etichetta = maiuscolo(tr(r.nome_it, r.nome_en));
        std::string valore = maiuscolo(!r.testo_lat.empty() ? r.testo_lat : tr(r.testo_it, r.testo_en)) + (r.conf == "STIMA" ? " *" : "");
        while (!etichetta.empty() && f.larghezza(etichetta, 1) > fs.w / 2) etichetta.pop_back();
        while (!valore.empty() && f.larghezza(valore, 1) > fs.w - f.larghezza(etichetta, 1) - 8) valore.pop_back();
        lamiera::voce_modulo(t, f, fs.x, y, fs.w, etichetta, valore, colori::BLU);
        y += 12;
        ++voci_scheda;
    }
    // il testo su carta: la storia a macchina, IN GIOCO a penna blu col segno a margine, i rimandi in rosso, le fonti
    const Rettangolo ft = lamiera::foglio(t, {pr.x + 8, iv.y + iv.h + 20, pr.w - 16, pr.y + pr.h - 8 - (iv.y + iv.h + 20)});
    struct Riga { std::string testo; std::uint8_t col; int rientro; bool margine; };
    std::vector<Riga> righe;
    const auto aggiungi = [&](const std::string& testo, std::uint8_t col, int rientro = 0, bool margine = false) {
        for (const auto& r : StazioneRadio::a_capo(f, maiuscolo(testo), ft.w - rientro)) righe.push_back({r, col, rientro, margine});
        righe.push_back({"", colori::NERO, 0, false});
    };
    aggiungi(tr(v.testo_it, v.testo_en), colori::NERO);
    if (const std::string& uso = tr(v.uso_it, v.uso_en); !uso.empty())
        aggiungi(std::string(inglese_ ? "IN GAME: " : "IN GIOCO: ") + uso, colori::BLU, 6, true);
    if (!v.riferimenti.empty()) {
        std::string rif = inglese_ ? "SEE:" : "VEDI:";
        for (std::size_t i = 0; i < v.riferimenti.size() && i < 9; ++i) {
            const auto& x = v.riferimenti[i];
            const VoceEnciclopedia* d = e_.voce(x.voce);
            rif += "   " + std::to_string(i + 1) + " " + (d ? d->nome_mostrato(inglese_) : x.voce);
        }
        aggiungi(rif, colori::ROSSO);
    }
    if (!v.armi_tipiche_altre.empty()) {
        std::string a = inglese_ ? "TYPICAL WEAPONS: " : "ARMI TIPICHE: ";
        for (std::size_t i = 0; i < v.armi_tipiche_altre.size(); ++i) a += (i ? ", " : "") + v.armi_tipiche_altre[i];
        aggiungi(a, colori::NERO);
    }
    if (!v.fonti.empty()) {
        std::string s = inglese_ ? "SOURCES: " : "FONTI: ";
        for (std::size_t i = 0; i < v.fonti.size(); ++i) s += (i ? "; " : "") + v.fonti[i];
        aggiungi(s, colori::GRIGIO_SCURO);
    }
    for (const auto& c : v.crediti) aggiungi(std::string(inglese_ ? "PHOTO: " : "FOTO: ") + c.autore + " (" + c.licenza + ")", colori::GRIGIO_SCURO);
    // a pagine: l'ultima riga del foglio e' per «(SEGUE) k/n»
    const int per_pagina = std::max(1, ft.h / PASSO_TESTO - 1);
    pagine_testo_ = std::max(1, (static_cast<int>(righe.size()) + per_pagina - 1) / per_pagina);
    const int pagina = std::clamp(scorri_, 0, pagine_testo_ - 1);
    int yt = ft.y;
    for (int i = pagina * per_pagina; i < std::min(static_cast<int>(righe.size()), (pagina + 1) * per_pagina); ++i) {
        const Riga& r = righe[static_cast<std::size_t>(i)];
        f.scrivi(t, ft.x + r.rientro, yt, r.testo, r.col, 1);
        if (r.margine) t.linea_verticale(ft.x + 1, yt - 1, yt + PASSO_TESTO - 3, colori::BLU);   // il segno a margine
        yt += PASSO_TESTO;
    }
    if (pagina + 1 < pagine_testo_) {
        const std::string segue = "(SEGUE) " + std::to_string(pagina + 1) + "/" + std::to_string(pagine_testo_);
        f.scrivi(t, ft.x + ft.w - f.larghezza(segue, 1), ft.y + per_pagina * PASSO_TESTO, segue, colori::BLU, 1);
    }
}

// Un'immagine della voce nel riquadro, disegnata una volta e tenuta (gli effetti e l'oculare costano). Come le tavole
// del kit (strumenti/anteprima_enciclopedia): sprite e modello 3D in scala, sagoma dell'arma, oculare con una Perry a
// 3 km, boa o marcatore in mare a 300 m, foto «alla 1989», cascata della firma, carta d'epoca, spaccato col sistema
void SchermataMuseo::disegna_immagine(Tela& t, const Font& f, const ImmagineVoce& im, Rettangolo r) const {
    const VoceEnciclopedia* v = voce();
    const std::string chiave = (v ? v->id : std::string()) + "#" + std::to_string(immagine_);
    if (im.tipo == "oculare") {   // la palette dei modi monocromi vale anche quando l'immagine viene dalla cache
        oculare_.attivo = true;
        const std::string modo = im.testo("modo", "giorno");
        oculare_.modo = modo == "bassa_luminosita" ? ModoOculare::bassa_luminosita : modo == "intensificatore" ? ModoOculare::intensificatore
                        : modo == "tv" ? ModoOculare::tv : modo == "termico" ? ModoOculare::termico : ModoOculare::giorno;
    }
    tipo_mostrato_ = im.tipo;
    if (!cache_ || cache_di_ != chiave) {
        cache_ = std::make_unique<Tela>(r.w, r.h);
        Tela& c = *cache_;
        c.riempi(colori::BLU);
        const Rettangolo q{0, 0, r.w, r.h};
        const double aob = im.numero("aob", 90.0), el = im.numero("elevazione", 0.0);
        const bool dritta = im.vero("dritta", true);
        if (im.tipo == "sprite") {
            const auto is = r_.sprite.find(stem(im.testo("file")));
            if (is != r_.sprite.end()) {
                const Sprite& s = is->second.per_angolo(aob, dritta, el);
                const double scala = std::min((q.w - 12.0) / s.w, (q.h - 12.0) / s.h);
                c.disegna_sprite(s, q.w / 2.0 + (s.ancora_x - s.w / 2.0) * scala, q.h / 2.0 + (s.ancora_y - s.h / 2.0) * scala, scala);
            }
        } else if (im.tipo == "modello") {
            const std::string cl = stem(im.testo("file"));
            const auto m3 = r_.modelli.find(cl);
            const auto ic = r_.classi.find(cl);
            if (m3 != r_.modelli.end() && ic != r_.classi.end() && ic->second.lunghezza_m > 0.0) {
                const Sprite s = genera_sprite(m3->second, PosaNave{}, aob, dritta, rampa_grigi(r_.giorno), (q.w - 16.0) / ic->second.lunghezza_m);
                const double scala = std::min({1.0, (q.w - 8.0) / s.w, (q.h - 8.0) / s.h});
                c.disegna_sprite(s, q.w / 2.0 + (s.ancora_x - s.w / 2.0) * scala, q.h / 2.0 + (s.ancora_y - s.h / 2.0) * scala, scala);
            }
        } else if (im.tipo == "sagoma") {
            static const CatalogoCarico carico = carica_carico(dati_ + "/carico.toml");
            const auto a = carico.armi.find(im.testo("arma"));
            if (a != carico.armi.end()) disegna_sagoma_arma(c, sagoma_da_scheda(a->second), Rettangolo{6, 20, q.w - 12, q.h - 40});
            else if (sagoma_nota(im.testo("arma")))   // i siluri senza voce nel carico (Mk 46, AT-1M, UMGT-1...)
                disegna_sagoma_arma(c, sagoma_da_id(im.testo("arma")), Rettangolo{6, 20, q.w - 12, q.h - 40});
        } else if (im.tipo == "oculare" || im.tipo == "effetto") {
            Periscopio per(r_.font, r_.classi, r_.sprite);
            per.usa_modelli(r_.modelli, rampa_grigi(r_.giorno));
            per.usa_palette(r_.giorno);
            Mondo m;
            Nave occhio;
            occhio.id = -1;
            VistaPeriscopio w;
            w.quota_occhio_m = 1.5;
            w.tempo_s = 30.0;
            Effetti e(3);
            if (im.tipo == "oculare") {
                Nave n;
                n.id = 1;
                n.classe = "perry";
                n.pos = {3000.0 * std::sin(radianti(1.0)), 3000.0 * std::cos(radianti(1.0))};
                n.pos_prec = n.pos;
                n.rotta = n.rotta_prec = 300.0;
                m.aggiungi(n);
                static const CatalogoPeriscopi cat = carica_periscopi(dati_ + "/periscopi.toml");
                if (const DatiPeriscopio* d = cat.periscopio(im.testo("periscopio"))) {
                    StatoPeriscopio s;
                    s.scelto = d->id;
                    s.alzata = 1.0;
                    s.su = s.comandato_su = s.testa_fuori = true;
                    s.altezza_testa_m = 1.5;
                    s.ingrandimento = static_cast<int>(im.numero("ingrandimento", 0.0));
                    s.modo = oculare_.modo == ModoOculare::tv ? ModoVista::tv : oculare_.modo == ModoOculare::bassa_luminosita ? ModoVista::bassa_luminosita
                             : oculare_.modo == ModoOculare::intensificatore ? ModoVista::intensificatore
                             : oculare_.modo == ModoOculare::termico ? ModoVista::termico : ModoVista::giorno;
                    w.oculare = parametri_oculare(*d, s);
                    w.oculare.modo = oculare_.modo;
                }
                if (w.oculare.modo != ModoOculare::giorno) w.luce = 0.05;
                w.rilevamento = 1.0;
                oculare_ = w.oculare;
            } else {
                const std::string ef = im.testo("effetto"), og = im.testo("oggetto");
                for (double tt = 0; tt < 70.0; tt += 0.1) {
                    if (ef == "boa") {
                        Sonoboa b;
                        b.id = 1;
                        b.tipo = og;
                        b.t_posa = 2.0;
                        b.t_impatto = 5.0;
                        b.t_attiva = 125.0;
                        b.t_fine = 1e9;
                        e.boe_asw({b}, tt);
                    } else if (tt == 0.0) {
                        e.marcatore_asw(og == "mk58" ? MarcatoreAsw::mk58 : og == "omab" ? MarcatoreAsw::omab_giorno : MarcatoreAsw::mk25, {0, 0}, 900);
                    }
                    e.avanza(0.1, m);
                }
                occhio.pos = {0.0, -300.0};
                w.alto_ingrandimento = true;
                w.tempo_s = 70.0;
            }
            Tela vista;
            vista.riempi(0);
            per.disegna(vista, m, occhio, w, im.tipo == "effetto" ? &e : nullptr);
            const Rettangolo s = im.tipo == "oculare" ? oculare::area(w.oculare) : Rettangolo{160, 110, 320, 200};
            const double scala = std::min(static_cast<double>(q.w) / s.w, static_cast<double>(q.h) / s.h);
            const int w2 = static_cast<int>(s.w * scala), h2 = static_cast<int>(s.h * scala);
            for (int y = 0; y < h2; ++y)
                for (int x = 0; x < w2; ++x)
                    c.pixel((q.w - w2) / 2 + x, (q.h - h2) / 2 + y, vista.at(s.x + static_cast<int>(x / scala), s.y + static_cast<int>(y / scala)));
        } else if (im.tipo == "foto") {
            try {
                const ImmagineIndicizzata foto = ImmagineIndicizzata::carica(dati_ + "/" + im.testo("file"));
                const double scala = std::min(static_cast<double>(q.w) / foto.w, static_cast<double>(q.h) / foto.h);
                const int w2 = static_cast<int>(foto.w * scala), h2 = static_cast<int>(foto.h * scala);
                for (int y = 0; y < h2; ++y)
                    for (int x = 0; x < w2; ++x)
                        c.pixel((q.w - w2) / 2 + x, (q.h - h2) / 2 + y,
                                foto.pix[static_cast<std::size_t>(static_cast<int>(y / scala) * foto.w + static_cast<int>(x / scala))]);
            } catch (const std::exception&) {
            }
        } else if (im.tipo == "cascata") {   // la classe al traverso, ferma, coi disegni del kit (grafica/cascata_firma)
            static const CatalogoAcustico acu = carica_acustica(dati_ + "/acustica.toml");
            disegna_cascata_firma(c, q, acu, im.testo("classe"), im.numero("velocita_kn"), 0x688u, im.testo("sensore"), &f);
        } else if (im.tipo == "carta" || im.tipo == "spaccato") {
            try {
                if (im.tipo == "carta") {   // la carta della base attorno al suo punto, o il teatro intero (carte_basi.py, carta_epoca.py)
                    const Fondale carta = Fondale::carica(dati_ + "/" + im.testo("file"));
                    const bool punto = im.numeri.count("lat") && im.numeri.count("lon");
                    const Geografiche g{im.numero("lat"), im.numero("lon")};
                    disegna_carta_museo(c, f, q, carta, punto ? &g : nullptr);
                } else {   // lo spaccato del battello col contorno del sistema
                    const std::string cl = im.testo("classe");
                    const Spaccato sp(cl, Modello::carica(dati_ + "/modelli/" + cl + ".mdl"), DatiScafo::carica(dati_ + "/scafi/" + cl + ".toml"));
                    c.riempi(colori::NERO);
                    disegna_spaccato_sistema(c, q, sp, im.testo("sistema"));
                }
            } catch (const std::exception&) {
                const std::string d = maiuscolo(im.tipo) + (inglese_ ? ": NOT AVAILABLE" : ": NON DISPONIBILE");
                f.scrivi(c, q.w / 2 - f.larghezza(d, 1) / 2, q.h / 2 - 3, d, colori::GRIGIO, 1);
            }
        }
        cache_di_ = chiave;
    }
    for (int y = 0; y < r.h; ++y)
        for (int x = 0; x < r.w; ++x) t.pixel(r.x + x, r.y + y, cache_->at(x, y));
}

}  // namespace m688
