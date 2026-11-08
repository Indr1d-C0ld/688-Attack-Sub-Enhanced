#include "albo.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>

#include "esito.hpp"
#include "museo.hpp"
#include "grafica/cerimonia.hpp"
#include "grafica/cornici.hpp"
#include "grafica/ritratto.hpp"
#include "nucleo/traccia_replay.hpp"

namespace m688 {

using namespace cerimonia;

namespace {

constexpr auto INCHIOSTRO = static_cast<std::uint8_t>(NERO + 1), TENUE = static_cast<std::uint8_t>(NERO + 3),
               ROSSO = static_cast<std::uint8_t>(TIMBRO + 2), CHIARO = static_cast<std::uint8_t>(BIANCO + 4),
               DORATO = static_cast<std::uint8_t>(ORO + 6);
constexpr int PIEDE_Y = 428, RIGA = 11;   // il piede di cerimonia (grafica/cornici): la geometria del piede comune

// in maiuscolo col font del gioco: sigle e nomi sovietici in cirillico traslitterati, «» e ª sostituiti (app/museo)
std::string maiuscolo(std::string s) { return testo_per_font(std::move(s)); }

std::string taglia(const Font& f, std::string s, int w) {
    while (!s.empty() && f.larghezza(s, 1) > w) {
        while (s.size() > 1 && (static_cast<unsigned char>(s.back()) & 0xC0) == 0x80) s.pop_back();
        s.pop_back();
    }
    return s;
}

// Le righe di un paragrafo a capo sulle parole, larghe al piu' w
std::vector<std::string> a_capo(const Font& f, const std::string& testo, int w) {
    std::vector<std::string> out;
    std::string riga, parola;
    for (std::size_t i = 0; i <= testo.size(); ++i) {
        const char c = i < testo.size() ? testo[i] : ' ';
        if (c != ' ' && c != '\n') {
            parola += c;
            continue;
        }
        if (!parola.empty()) {
            const std::string prova = riga.empty() ? parola : riga + " " + parola;
            if (f.larghezza(prova, 1) > w && !riga.empty()) {
                out.push_back(riga);
                riga = parola;
            } else {
                riga = prova;
            }
            parola.clear();
        }
        if (c == '\n') {
            out.push_back(riga);
            riga.clear();
        }
    }
    if (!riga.empty()) out.push_back(riga);
    return out;
}

void scrivania(Tela& t) {   // il piano di legno, come sotto il rapporto d'esito
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

std::string nome_stato(const Fascicolo& f) {
    if (f.stato == "morto") return f.marina == Marina::usn ? "ON ETERNAL PATROL" : "POGIB PRI ISPOLNENII";
    if (f.stato == "rimosso") return "RIMOSSO";
    if (f.stato == "congedo") return "CONGEDATO";
    if (f.stato == "pensione") return "IN PENSIONE";
    if (f.stato == "trasferito") return "TRASFERITO A TERRA";
    return "IN SERVIZIO";
}

std::string sigla_grado(const RegoleCarriera& r, const Fascicolo& f) {
    const Grado* g = r.grado(f.marina, f.grado);
    return g && !g->sigla.empty() ? maiuscolo(g->sigla) : maiuscolo(f.grado);
}

std::string nome_qualifica(const std::string& id) {
    if (id == "dolphins") return "DOLPHINS: QUALIFICATO AI SOMMERGIBILI";
    if (id == "esami_guardia") return "ESAMI DI GUARDIA";
    if (id == "comando") return "COMMAND AT SEA: QUALIFICATO AL COMANDO";
    if (id == "vsok") return "VSOK: AMMESSO AL COMANDO";
    if (id == "ingegnere") return "QUALIFICA DI INGEGNERE NUCLEARE";
    return maiuscolo(id);
}

std::string punti_interi(double v) {
    char b[24];
    std::snprintf(b, sizeof b, "%.0f", v);
    return b;
}

}  // namespace

// ------------------------------------------------------------------------------------------------------- fascicolo

SchermataFascicolo::SchermataFascicolo(const RegoleCarriera& r, const TavolaDecorazioni& tav, Fascicolo f)
    : r_(r), tav_(tav), f_(std::move(f)) {
    pagine_ = {Pagina::copertina, Pagina::servizio, Pagina::note, Pagina::decorazioni};
    if (!f_.commissioni.empty() || !f_.provvedimenti.empty()) pagine_.push_back(Pagina::provvedimenti);
}

void SchermataFascicolo::vai(int i) {
    pagina_ = std::clamp(i, 0, pagine() - 1);
    scorri_ = 0;
}

bool SchermataFascicolo::comandi(const Comandi& c) {
    if (const int lato = passi_freccia(c.ruota, c.passi_ruota, prec_ruota_)) vai(pagina_ + lato);
    const bool note = pagine_[static_cast<std::size_t>(pagina_)] == Pagina::note;
    if (const int su = passi_freccia(c.su_giu, c.passi_su_giu, prec_su_giu_)) {
        if (note && !f_.missioni.empty()) {   // la missione scelta; il foglio la segue (circa tre righe per missione)
            scelta_ = std::clamp(scelta_ - su, 0, static_cast<int>(f_.missioni.size()) - 1);
            scorri_ = std::max(0, scelta_ * 3 - 18);
        } else {
            scorri_ = std::max(0, scorri_ - su * 5);
        }
    }
    if (note && !f_.missioni.empty() && std::find(c.lettere.begin(), c.lettere.end(), 'P') != c.lettere.end()) {
        const VoceMissione& m = f_.missioni[f_.missioni.size() - 1 - static_cast<std::size_t>(scelta_)];
        if (!m.replay.empty()) replay_ = percorso_replay(f_.id, m.missione, m.data).parent_path() / m.replay;
    }
    if (c.invio) vai(pagina_ + 1);
    bool esci = c.esc || c.esci;
    for (const auto& [x, y] : c.clic)   // i tasti del piede (vedi componi): indietro, replay, prima, dopo
        switch (tasto_cerimonia(x, y, PIEDE_Y, 4)) {
            case 0: esci = true; break;
            case 1:
                if (note && !f_.missioni.empty()) {
                    const VoceMissione& m = f_.missioni[f_.missioni.size() - 1 - static_cast<std::size_t>(scelta_)];
                    if (!m.replay.empty()) replay_ = percorso_replay(f_.id, m.missione, m.data).parent_path() / m.replay;
                }
                break;
            case 2: vai(pagina_ - 1); break;
            case 3: vai(pagina_ + 1); break;
            default: break;
        }
    return esci;
}

void SchermataFascicolo::palette(Palette& p) const { prepara_palette_ritratto(p, ritratto_comandante(r_, f_), &tav_); }

void SchermataFascicolo::disegna(Tela& t, const Font& f) const {
    if (!cache_ || cache_di_ != pagina_ || cache_scorri_ != scorri_ || cache_scelta_ != scelta_) {
        cache_ = std::make_unique<Tela>(t.larghezza(), t.altezza());
        componi(*cache_, f);
        cache_di_ = pagina_;
        cache_scorri_ = scorri_;
        cache_scelta_ = scelta_;
    }
    t = *cache_;
}

void SchermataFascicolo::componi(Tela& t, const Font& f) const {
    switch (pagine_[static_cast<std::size_t>(pagina_)]) {
        case Pagina::copertina: copertina(t, f); break;
        case Pagina::servizio: servizio(t, f); break;
        case Pagina::note: note(t, f); break;
        case Pagina::decorazioni: decorazioni(t, f); break;
        case Pagina::provvedimenti: provvedimenti(t, f); break;
    }
    // il piede comune della cerimonia (scelta dell'utente del 29/10/2026), come l'albo
    char piede[96];
    const bool note = pagine_[static_cast<std::size_t>(pagina_)] == Pagina::note;
    std::snprintf(piede, sizeof piede, "PAGINA %d DI %d   %s", pagina_ + 1, pagine(), note ? "SU/GIU' LA MISSIONE" : "SU/GIU' SCORRE");
    bool con_replay = false;
    if (note && !f_.missioni.empty())
        con_replay = !f_.missioni[f_.missioni.size() - 1 - static_cast<std::size_t>(scelta_)].replay.empty();
    piede_cerimonia(t, f, PIEDE_Y, piede,
                    {{"ESC  INDIETRO"}, {"P  REPLAY", con_replay}, {"SINISTRA  PRIMA", pagina_ > 0}, {"DESTRA  DOPO", pagina_ + 1 < pagine()}},
                    3);
}

int SchermataFascicolo::foglio(Tela& t, const Font& f, const std::string& titolo) const {
    const bool usn = f_.marina == Marina::usn;
    scrivania(t);
    const Rettangolo r{32, 8, 576, 416};   // fino a 424: sotto c'e' il piede di cerimonia
    carta(t, r, f_.seme + static_cast<std::uint32_t>(pagina_));
    f.scrivi(t, r.x + 20, r.y + 14, usn ? "OFFICER SERVICE RECORD" : "LICHNOE DELO", INCHIOSTRO, 2);
    f.scrivi(t, r.x + 20, r.y + 34, titolo + " - " + maiuscolo(f_.nome), TENUE, 1);
    timbro(t, f, r.x + r.w - 80, r.y + 26, usn ? "CONFIDENTIAL" : "CEKPETHO", -7, 2, f_.seme);
    t.linea_orizzontale(r.x + 20, r.x + r.w - 20, r.y + 48, TENUE);
    return r.y + 58;
}

void SchermataFascicolo::copertina(Tela& t, const Font& f) const {
    const bool usn = f_.marina == Marina::usn;
    const int y0 = foglio(t, f, "FASCICOLO PERSONALE");
    cornice(t, {50, y0, 196, 252}, usn ? StileCornice::oro : StileCornice::rossa, 6);
    disegna_ritratto(t, {56, y0 + 6, 184, 240}, ritratto_comandante(r_, f_), &tav_);
    double punti = 0.0;
    for (const auto& m : f_.missioni) punti += m.punti;
    std::string incarico = "-";
    if (!f_.incarichi.empty()) {
        const Incarico& i = f_.incarichi.back();
        incarico = maiuscolo(i.incarico) + (i.battello.empty() ? "" : ", " + maiuscolo(i.battello));
    }
    const std::pair<std::string, std::string> righe[] = {
        {"NOME", maiuscolo(f_.nome)},
        {"GRADO", nome_grado_comandante(r_, f_) + " (" + sigla_grado(r_, f_) + ")"},
        {"MARINA", usn ? "UNITED STATES NAVY" : "VOENNO-MORSKOJ FLOT SSSR"},
        {"PROVENIENZA", maiuscolo(f_.provenienza)},
        {"IN SERVIZIO DAL", std::to_string(f_.anno_nomina)},
        {"DATA", data_estesa(f_.data)},
        {"INCARICO", incarico},
        {"STATO", nome_stato(f_)},
        {"CARRIERA", std::string(f_.carriera_breve ? "BREVE, DAL COMANDO" : "COMPLETA") + (f_.aperta ? ", APERTA (EDITOR)" : "")},
        {"INDULGENZA", maiuscolo(std::string(nome_indulgenza(f_.indulgenza)))},
        {"MISSIONI", std::to_string(f_.missioni.size())},
        {"PUNTI", punti_interi(punti) + " DI CARRIERA"},
        {"DECORAZIONI", std::to_string(f_.decorazioni.size())},
    };
    int y = y0 + 4;
    for (const auto& [k, v] : righe) {
        f.scrivi(t, 270, y, k, TENUE, 1);
        const auto r = a_capo(f, v, 300);
        for (std::size_t i = 0; i < r.size(); ++i, y += RIGA) f.scrivi(t, 380, y, r[i], k == "STATO" && !f_.in_servizio() ? ROSSO : INCHIOSTRO, 1);
        y += 6;
    }
    if (!f_.in_servizio()) timbro(t, f, 470, 386, f_.stato == "morto" ? (usn ? "ETERNAL PATROL" : "VECHNAYA PAMYAT") : "CHIUSO", 8, 2, f_.seme + 3);
}

// Le pagine di testo: le righe (a capo sulle parole) dalla riga scorri_, fin dove c'e' posto
namespace {
struct Riga { std::string testo; std::uint8_t col; int rientro = 0; };
void scrivi_righe(Tela& t, const Font& f, int x, int y, int w, const std::vector<Riga>& righe, int scorri, int y_max) {
    std::vector<Riga> tutte;
    for (const auto& r : righe)
        for (const auto& s : a_capo(f, r.testo, w - r.rientro)) tutte.push_back({s, r.col, r.rientro});
    for (std::size_t i = static_cast<std::size_t>(std::min<int>(scorri, static_cast<int>(tutte.size()))); i < tutte.size() && y <= y_max; ++i, y += RIGA)
        f.scrivi(t, x + tutte[i].rientro, y, tutte[i].testo, tutte[i].col, 1);
}
}  // namespace

void SchermataFascicolo::servizio(Tela& t, const Font& f) const {
    const int y0 = foglio(t, f, "STATO DI SERVIZIO");
    std::vector<Riga> r;
    r.push_back({"INCARICHI", ROSSO});
    if (f_.incarichi.empty()) r.push_back({"NESSUN INCARICO REGISTRATO", TENUE, 12});
    for (const auto& i : f_.incarichi) {
        const Grado* g = r_.grado(f_.marina, i.grado);
        r.push_back({i.dal + " - " + (i.al.empty() ? "IN CORSO" : i.al) + "   " + (g ? maiuscolo(g->sigla) : maiuscolo(i.grado)) + "   " +
                         maiuscolo(i.incarico) + (i.battello.empty() ? "" : ", " + maiuscolo(i.battello)),
                     INCHIOSTRO, 12});
    }
    r.push_back({"", INCHIOSTRO});
    r.push_back({"QUALIFICHE", ROSSO});
    if (f_.qualifiche.empty()) r.push_back({"NESSUNA", TENUE, 12});
    for (const auto& q : f_.qualifiche) r.push_back({q.data + "   " + nome_qualifica(q.id), INCHIOSTRO, 12});
    scrivi_righe(t, f, 56, y0 + 4, 528, r, scorri_, 412);
}

void SchermataFascicolo::note(Tela& t, const Font& f) const {
    const bool usn = f_.marina == Marina::usn;
    const int y0 = foglio(t, f, usn ? "NOTE CARATTERISTICHE (FITNESS REPORTS)" : "ATTESTAZIONI (ATTESTATSII)");
    std::vector<Riga> r;
    if (f_.missioni.empty()) r.push_back({"NESSUNA MISSIONE ANCORA: IL FASCICOLO SI APRE AL PRIMO RIENTRO.", TENUE});
    for (auto it = f_.missioni.rbegin(); it != f_.missioni.rend(); ++it) {   // la piu' recente in alto
        const VoceMissione& m = *it;
        const Grado* g = r_.grado(f_.marina, m.grado);
        const bool scelta = it - f_.missioni.rbegin() == scelta_;
        char testa[192];
        std::snprintf(testa, sizeof testa, "%s%s   %s   %s   PUNTEGGIO %.0f   %d DI %d%s", scelta ? "> " : "", m.data.c_str(),
                      maiuscolo(m.missione).c_str(), g ? maiuscolo(g->sigla).c_str() : maiuscolo(m.grado).c_str(), m.punteggio,
                      m.graduatoria, m.pari, m.replay.empty() ? "" : "   [P REPLAY]");
        r.push_back({testa, scelta ? ROSSO : INCHIOSTRO});
        if (!m.giudizio_it.empty()) r.push_back({maiuscolo(m.giudizio_it), TENUE, 12});
        r.push_back({"", INCHIOSTRO});
    }
    scrivi_righe(t, f, 56, y0 + 4, 528, r, scorri_, 412);
}

void SchermataFascicolo::decorazioni(Tela& t, const Font& f) const {
    const int y0 = foglio(t, f, "DECORAZIONI");
    const auto barretta = Carriera(r_, f_).barretta();
    int y = y0 + 4;
    if (!barretta.empty()) {
        const Rettangolo b = tav_.disegna_barretta(t, LARGHEZZA_TELA / 2, y0 + 4 + misura_barretta(barretta.size(), Carriera::per_fila(f_.marina), 30, 9).h,
                                                   barretta, Carriera::per_fila(f_.marina), 30, 9);
        y = b.y + b.h + 10;
    }
    std::vector<Riga> r;
    if (f_.decorazioni.empty()) r.push_back({"NESSUNA DECORAZIONE", TENUE});
    for (const auto& k : f_.decorazioni) {
        const Decorazione* d = r_.decorazione(k.decorazione);
        std::string nome = d ? maiuscolo(d->nome_it.empty() ? d->nome : d->nome_it) : maiuscolo(k.decorazione);
        if (k.numero > 1) nome += " (" + std::to_string(k.numero) + "° CONFERIMENTO)";
        if (k.dispositivo == "v") nome += " CON LA «V»";
        if (k.postuma) nome += ", ALLA MEMORIA";
        r.push_back({k.data + "   " + nome + (k.missione.empty() ? "" : "   " + maiuscolo(k.missione)), INCHIOSTRO});
        for (const auto& m : k.motivazione_it) r.push_back({maiuscolo(m), TENUE, 12});
    }
    scrivi_righe(t, f, 56, y, 528, r, scorri_, 412);
}

void SchermataFascicolo::provvedimenti(Tela& t, const Font& f) const {
    const int y0 = foglio(t, f, "COMMISSIONI E PROVVEDIMENTI");
    std::vector<Riga> r;
    r.push_back({"COMMISSIONI", ROSSO});
    if (f_.commissioni.empty()) r.push_back({"NESSUNA", TENUE, 12});
    for (const auto& c : f_.commissioni) {
        char riga[160];
        std::snprintf(riga, sizeof riga, "%s   %s   ESITO: %s   MEDIA %.0f", c.data.c_str(), maiuscolo(c.grado).c_str(), maiuscolo(c.esito).c_str(), c.media);
        r.push_back({riga, INCHIOSTRO, 12});
    }
    r.push_back({"", INCHIOSTRO});
    r.push_back({"PROVVEDIMENTI", ROSSO});
    if (f_.provvedimenti.empty()) r.push_back({"NESSUNO", TENUE, 12});
    for (const auto& p : f_.provvedimenti) {
        std::string tipo = maiuscolo(p.tipo);
        std::replace(tipo.begin(), tipo.end(), '_', ' ');
        r.push_back({p.data + "   " + tipo + (p.missione.empty() ? "" : "   " + maiuscolo(p.missione)), INCHIOSTRO, 12});
        if (!p.motivo.empty()) r.push_back({maiuscolo(p.motivo), TENUE, 24});
    }
    scrivi_righe(t, f, 56, y0 + 4, 528, r, scorri_, 412);
}

// ----------------------------------------------------------------------------------------------------------- albo

SchermataAlbo::SchermataAlbo(const RegoleCarriera& r, const TavolaDecorazioni& tav) : r_(r), tav_(tav) { ricarica(); }

void SchermataAlbo::ricarica() {
    voci_ = leggi_albo();
    for (std::size_t i = 0; i < voci_.size(); ++i)
        if (voci_[i].in_carica && cursore_ == 0) cursore_ = static_cast<int>(i);
    cursore_ = std::clamp(cursore_, 0, std::min(static_cast<int>(voci_.size()), ALBO_MAX - 1));
    sporca_ = true;
}

const Fascicolo* SchermataAlbo::scelto() const {
    return cursore_ < static_cast<int>(voci_.size()) ? &voci_[static_cast<std::size_t>(cursore_)].f : nullptr;
}

// gruppo 4 della passata estetica (scelta dell'utente del 29/10/2026): i cartellini da 182, per far posto al piede
Rettangolo SchermataAlbo::cartellino(int i) const { return {12 + (i % 4) * 156, 46 + (i / 4) * 190, 148, 182}; }

void SchermataAlbo::crea() {
    Modulo& m = *modulo_;
    std::string nome = m.nome;
    while (!nome.empty() && nome.back() == ' ') nome.pop_back();
    if (nome.empty()) nome = m.marina == Marina::usn ? "J. R. Halloran" : "V. A. Suchkov";
    Fascicolo f = Carriera::nuovo(r_, m.marina, nome, m.breve, m.indulgenza);
    f.id = id_libero(f.id);
    try {
        salva_fascicolo(f, percorso_fascicolo(f.id));
        imposta_in_carica(f.marina, f.id);
        marina_scelta_ = f.marina;
        avviso_ = maiuscolo(f.nome) + ": FASCICOLO APERTO, IN CARICA PER " + (f.marina == Marina::usn ? "LA US NAVY (688)" : "LA VMF (ALFA)");
    } catch (const std::exception& e) {
        avviso_ = maiuscolo(std::string("impossibile salvare: ") + e.what());
    }
    modulo_.reset();
    ricarica();
    for (std::size_t i = 0; i < voci_.size(); ++i)
        if (voci_[i].f.id == f.id) cursore_ = static_cast<int>(i);
}

SchermataAlbo::Uscita SchermataAlbo::comandi(const Comandi& c) {
    if (c.esci) return Uscita::menu;
    const int su = passi_freccia(c.su_giu, c.passi_su_giu, prec_su_giu_), lato = passi_freccia(c.ruota, c.passi_ruota, prec_ruota_);
    if (modulo_) {   // carriera nuova: il nome si scrive, il resto si sceglie con le frecce o col clic; Invio passa al campo dopo
        Modulo& m = *modulo_;
        if (c.esc) {
            modulo_.reset();
            return Uscita::nessuna;
        }
        for (const auto& [x, y] : c.clic) {
            const auto dentro = [&, x = x, y = y](const Rettangolo& r) { return x >= r.x && y >= r.y && x < r.x + r.w && y < r.y + r.h; };
            if (dentro(tasto_modulo(false))) {
                modulo_.reset();
                return Uscita::nessuna;
            }
            if (dentro(tasto_modulo(true))) {
                crea();
                return Uscita::nessuna;
            }
            if (dentro(casella_modulo(0, 0))) m.campo = 0;
            for (int k = 0; k < 2; ++k)
                if (dentro(casella_modulo(1, k))) m.campo = 1, m.marina = k == 0 ? Marina::usn : Marina::vmf;
            for (int k = 0; k < 2; ++k)
                if (dentro(casella_modulo(2, k))) m.campo = 2, m.breve = k == 0;
            for (int k = 0; k < 3; ++k)
                if (dentro(casella_modulo(3, k))) m.campo = 3, m.indulgenza = static_cast<Indulgenza>(k);
        }
        if (c.invio) {
            if (m.campo == 3) {
                crea();
                return Uscita::nessuna;
            }
            ++m.campo;
            return Uscita::nessuna;
        }
        if (su) m.campo = std::clamp(m.campo - su, 0, 3);
        if (c.tab) m.campo = (m.campo + c.tab + 4) % 4;
        if (m.campo == 0) {
            for (const char ch : c.testo)
                if (static_cast<unsigned char>(ch) >= 32 && m.nome.size() < 28) m.nome += ch;
            if (c.cancella && !m.nome.empty()) {
                while (m.nome.size() > 1 && (static_cast<unsigned char>(m.nome.back()) & 0xC0) == 0x80) m.nome.pop_back();
                m.nome.pop_back();
            }
        } else if (lato) {
            if (m.campo == 1) m.marina = m.marina == Marina::usn ? Marina::vmf : Marina::usn;
            if (m.campo == 2) m.breve = !m.breve;
            if (m.campo == 3) m.indulgenza = static_cast<Indulgenza>((static_cast<int>(m.indulgenza) + 3 + lato % 3) % 3);
        }
        return Uscita::nessuna;
    }
    const int ultimo = std::min(static_cast<int>(voci_.size()), ALBO_MAX - 1);
    const auto muovi = [&](int i) {
        cursore_ = std::clamp(i, 0, ultimo);
        conferma_ = 0;
        avviso_.clear();
    };
    bool invio = c.invio;
    std::vector<char> lettere = c.lettere;
    for (const auto& [x, y] : c.clic) {
        if (y >= PIEDE_Y + 22 && y < PIEDE_Y + 48) {   // i tasti del piede (vedi disegna): la geometria di piede_cerimonia
            const bool con_comandante = scelto() != nullptr;
            if (conferma_ == 1 && con_comandante) {   // la conferma: due tasti
                if (x >= LARGHEZZA_TELA / 2) lettere.push_back('E');
                else conferma_ = 0;
                continue;
            }
            const int n = con_comandante ? 5 : 4, w = (LARGHEZZA_TELA - 24 - 8 * (n - 1)) / n;
            const int k = std::clamp((x - 12) / (w + 8), 0, n - 1);
            if (k == 0) return Uscita::menu;
            if (k == 1) lettere.push_back(con_comandante ? 'F' : 'O');
            if (k == 2) lettere.push_back('S');
            if (k == 3 && con_comandante) lettere.push_back('E');
            if (k == n - 1) invio = true;
            continue;
        }
        for (int i = 0; i <= ultimo; ++i) {
            const Rettangolo r = cartellino(i);
            if (x < r.x || y < r.y || x >= r.x + r.w || y >= r.y + r.h) continue;
            if (i == cursore_) invio = true;
            else muovi(i);
        }
    }
    if (lato) muovi(cursore_ + lato);
    if (su) muovi(cursore_ - 4 * su);
    if (c.esc) {
        if (conferma_) {
            conferma_ = 0;
            return Uscita::nessuna;
        }
        return Uscita::menu;
    }
    const Fascicolo* f = scelto();
    if (invio && conferma_ == 1 && f) {   // nella conferma il tasto evidenziato e' il no
        conferma_ = 0;
        return Uscita::nessuna;
    }
    if ((c.cancella || std::find(lettere.begin(), lettere.end(), 'E') != lettere.end()) && f) {   // due volte: definitivo, come in F-117A
        if (++conferma_ >= 2) {
            avviso_ = maiuscolo(f->nome) + ": FASCICOLO ELIMINATO";
            if (marina_scelta_ == f->marina) marina_scelta_.reset();
            elimina_dall_albo(f->id);
            conferma_ = 0;
            ricarica();
        }
        return Uscita::nessuna;
    }
    if (std::find(lettere.begin(), lettere.end(), 'F') != lettere.end() && f) return Uscita::fascicolo;
    if (std::find(lettere.begin(), lettere.end(), 'B') != lettere.end()) return Uscita::bacheca;   // senza scelto: i globali
    if (std::find(lettere.begin(), lettere.end(), 'O') != lettere.end()) return Uscita::onore;
    if (std::find(lettere.begin(), lettere.end(), 'S') != lettere.end()) return Uscita::statistiche;   // senza scelto: le globali
    if (invio) {
        conferma_ = 0;
        if (!f) {
            if (static_cast<int>(voci_.size()) < ALBO_MAX) modulo_.emplace();
        } else if (!caricabile(*f)) {
            avviso_ = maiuscolo(f->nome) + ": " + nome_stato(*f) + ", NON PUO' ANDARE IN MARE";
        } else {
            try {
                imposta_in_carica(f->marina, f->id);
                marina_scelta_ = f->marina;
                avviso_ = maiuscolo(f->nome) + " IN CARICA PER " + (f->marina == Marina::usn ? "LA US NAVY (688)" : "LA VMF (ALFA)");
            } catch (const std::exception& e) {
                avviso_ = maiuscolo(std::string("impossibile salvare: ") + e.what());
            }
            ricarica();
        }
    }
    return Uscita::nessuna;
}

void SchermataAlbo::palette(Palette& p) const {
    const Fascicolo* f = scelto();
    if (!f && !voci_.empty()) f = &voci_.front().f;
    if (f) prepara_palette_ritratto(p, ritratto_comandante(r_, *f), &tav_);
    else prepara_palette_ritratto(p, ParametriRitratto{}, &tav_);
}

void SchermataAlbo::componi(Tela& t, const Font& f) const {
    scrivania(t);
    intestazione(t, f, {0, 0, LARGHEZZA_TELA, 38}, "ALBO DEI COMANDANTI", "CHI VA IN MARE CON LA PROSSIMA MISSIONE", Marina::usn, false);
    for (int i = 0; i < ALBO_MAX; ++i) {
        const Rettangolo r = cartellino(i);
        if (i > static_cast<int>(voci_.size())) {   // gli altri posti dell'albo: fili tratteggiati numerati
            posto_libero(t, r, f, i + 1);
            continue;
        }
        if (i == static_cast<int>(voci_.size())) {   // il cartellino della carriera nuova, di carta: il modulo da riempire
            carta(t, r, 11);
            const char* a = "NUOVA CARRIERA";
            f.scrivi(t, r.x + r.w / 2 - f.larghezza(a, 1) / 2, r.y + 8, a, INCHIOSTRO, 1);
            const Rettangolo foto{r.x + 34, r.y + 22, 80, 96};   // il posto della foto, tratteggiato, con la croce
            for (int k = 0; k < foto.w; k += 3) t.pixel(foto.x + k, foto.y, TENUE), t.pixel(foto.x + k, foto.y + foto.h - 1, TENUE);
            for (int k = 0; k < foto.h; k += 3) t.pixel(foto.x, foto.y + k, TENUE), t.pixel(foto.x + foto.w - 1, foto.y + k, TENUE);
            const int cx = foto.x + foto.w / 2, cy = foto.y + foto.h / 2;
            t.linea_orizzontale(cx - 10, cx + 10, cy, INCHIOSTRO);
            t.linea_verticale(cx, cy - 10, cy + 10, INCHIOSTRO);
            int yy = foto.y + foto.h + 6;
            for (const char* s : {"IL NOME", "688 O ALFA", "COMPLETA O BREVE", "L'INDULGENZA"}) {
                f.scrivi(t, r.x + r.w / 2 - f.larghezza(s, 1) / 2, yy, s, TENUE, 1);
                yy += 10;
            }
            continue;
        }
        const VoceAlbo& v = voci_[static_cast<std::size_t>(i)];
        const Fascicolo& x = v.f;
        carta(t, r, x.seme);
        disegna_ritratto(t, {r.x + 34, r.y + 8, 80, 105}, ritratto_comandante(r_, x), &tav_);
        int y = r.y + 118;
        const auto riga = [&](const std::string& s, std::uint8_t col) {
            f.scrivi(t, r.x + 8, y, taglia(f, s, r.w - 16), col, 1);
            y += RIGA;
        };
        riga(maiuscolo(x.nome), INCHIOSTRO);
        riga(sigla_grado(r_, x) + "   " + (x.marina == Marina::usn ? "US NAVY" : "VMF"), INCHIOSTRO);
        riga("MISSIONI " + std::to_string(x.missioni.size()) + "  PUNTI " + punti_interi(v.punti), TENUE);
        riga(std::to_string(x.decorazioni.size()) + " DECORAZIONI" + (x.carriera_breve ? "  BREVE" : ""), TENUE);
        riga(nome_stato(x), caricabile(x) ? INCHIOSTRO : ROSSO);
        if (v.in_carica) timbro(t, f, r.x + r.w / 2, r.y + 96, "IN CARICA", -5, 1, x.seme);   // sulla fototessera
        if (x.stato == "morto")   // il nastro nero del lutto sull'angolo
            for (int k = 0; k < 10; ++k) t.linea_orizzontale(r.x + r.w - 30 + k, r.x + r.w - 1, r.y + k, static_cast<std::uint8_t>(NERO));
    }
}

void SchermataAlbo::disegna(Tela& t, const Font& f) const {
    if (!cache_ || sporca_) {
        cache_ = std::make_unique<Tela>(t.larghezza(), t.altezza());
        componi(*cache_, f);
        sporca_ = false;
    }
    t = *cache_;
    cornice_semplice(t, cartellino(cursore_), 3, DORATO);
    // il piede di cerimonia (gruppo 4): legno e targhette d'ottone, la geometria del piede comune, senza toccare i
    // ritratti negli indici 16-47; la conferma e gli avvisi al posto della riga d'aiuto
    const Fascicolo* sc = scelto();
    std::string messaggio;
    if (conferma_ == 1 && sc) messaggio = "ELIMINARE " + maiuscolo(sc->nome) + "? LA CARRIERA VA PERSA PER SEMPRE";
    else if (!avviso_.empty()) messaggio = avviso_;
    const std::string aiuto = !messaggio.empty() ? std::string()
                              : sc ? "B RICONOSCIMENTI   O ALBO D'ONORE"
                                   : "F FASCICOLO, B RICONOSCIMENTI, E ELIMINA: QUANDO C'E' UN COMANDANTE";
    if (conferma_ == 1 && sc)
        piede_cerimonia(t, f, PIEDE_Y, aiuto, {{"ESC  NO, LO TENGO"}, {"E  SI', ELIMINA"}}, 0);
    else if (sc)
        piede_cerimonia(t, f, PIEDE_Y, aiuto, {{"ESC  MENU"}, {"F  FASCICOLO"}, {"S  STATISTICHE"}, {"E  ELIMINA"}, {"INVIO  IN CARICA", caricabile(*sc)}}, 4);
    else
        piede_cerimonia(t, f, PIEDE_Y, aiuto, {{"ESC  MENU"}, {"O  ALBO D'ONORE"}, {"S  STATISTICHE"}, {"INVIO  NUOVA CARRIERA"}}, 3);
    if (!messaggio.empty()) f.scrivi(t, 12, PIEDE_Y + 6, taglia(f, messaggio, LARGHEZZA_TELA - 24), DORATO, 1);
    if (modulo_) disegna_modulo(t, f);
}

namespace {
const Rettangolo MODULO{84, 96, 472, 268};
constexpr int MODULO_RIGA_Y = 48, MODULO_RIGA = 40, MODULO_VALORI_X = 104, MODULO_CASELLA_H = 24;
}  // namespace

Rettangolo SchermataAlbo::casella_modulo(int campo, int k) {
    const int y = MODULO.y + MODULO_RIGA_Y + campo * MODULO_RIGA, x0 = MODULO.x + MODULO_VALORI_X, w = MODULO.w - MODULO_VALORI_X - 12;
    if (campo == 0) return {x0, y, w, MODULO_CASELLA_H};
    const int n = campo == 3 ? 3 : 2, passo = w / n;
    return {x0 + k * passo, y, passo - 6, MODULO_CASELLA_H};
}

Rettangolo SchermataAlbo::tasto_modulo(bool apri) {
    return apri ? Rettangolo{MODULO.x + MODULO.w - 192, MODULO.y + MODULO.h - 40, 180, 26} : Rettangolo{MODULO.x + 12, MODULO.y + MODULO.h - 40, 120, 26};
}

void SchermataAlbo::disegna_modulo(Tela& t, const Font& f) const {
    const Modulo& m = *modulo_;
    const Rettangolo r = MODULO;
    carta(t, r, 11);
    f.scrivi(t, r.x + 20, r.y + 12, "NUOVA CARRIERA", INCHIOSTRO, 2);
    const auto cornice = [&](const Rettangolo& q, std::uint8_t c) {
        t.linea_orizzontale(q.x, q.x + q.w - 1, q.y, c);
        t.linea_orizzontale(q.x, q.x + q.w - 1, q.y + q.h - 1, c);
        t.linea_verticale(q.x, q.y, q.y + q.h - 1, c);
        t.linea_verticale(q.x + q.w - 1, q.y, q.y + q.h - 1, c);
    };
    // una scelta: la casella piena e' quella presa, tutte sempre in vista e tutte cliccabili
    const auto casella = [&](const Rettangolo& q, const char* testo, bool presa) {
        if (presa) t.rettangolo_pieno(q.x, q.y, q.w, q.h, INCHIOSTRO);
        cornice(q, presa ? INCHIOSTRO : TENUE);
        const std::string s = taglia(f, testo, q.w - 6);
        f.scrivi(t, q.x + q.w / 2 - f.larghezza(s, 1) / 2, q.y + (q.h - f.righe()) / 2, s, presa ? CHIARO : INCHIOSTRO, 1);
    };
    static constexpr const char* TITOLI[] = {"NOME", "MARINA", "CARRIERA", "INDULGENZA"};
    for (int i = 0; i < 4; ++i) {
        const int y = casella_modulo(i, 0).y + (MODULO_CASELLA_H - f.righe()) / 2;
        if (i == m.campo) f.scrivi(t, r.x + 8, y, ">", ROSSO, 1);
        f.scrivi(t, r.x + 18, y, TITOLI[i], i == m.campo ? ROSSO : TENUE, 1);
    }
    const Rettangolo nome = casella_modulo(0, 0);
    cornice(nome, m.campo == 0 ? ROSSO : TENUE);
    if (m.nome.empty() && m.campo != 0)   // il nome di serie, della marina scelta
        f.scrivi(t, nome.x + 6, nome.y + (nome.h - f.righe()) / 2, maiuscolo(m.marina == Marina::usn ? "J. R. Halloran" : "V. A. Suchkov"), TENUE, 1);
    else
        f.scrivi(t, nome.x + 6, nome.y + (nome.h - f.righe()) / 2, maiuscolo(m.nome) + (m.campo == 0 ? "_" : ""), INCHIOSTRO, 1);
    casella(casella_modulo(1, 0), "US NAVY (688)", m.marina == Marina::usn);
    casella(casella_modulo(1, 1), "VMF (ALFA)", m.marina == Marina::vmf);
    casella(casella_modulo(2, 0), "BREVE: DAL COMANDO", m.breve);
    casella(casella_modulo(2, 1), "COMPLETA: DALL'INIZIO", !m.breve);
    static constexpr const char* INDULGENZE[] = {"REALISTICA", "INDULGENTE", "ARCADE"};
    for (int k = 0; k < 3; ++k) casella(casella_modulo(3, k), INDULGENZE[k], static_cast<int>(m.indulgenza) == k);
    f.scrivi(t, r.x + 18, tasto_modulo(true).y - 16, "CLIC, O FRECCE: TAB E SU/GIU' IL CAMPO, SINISTRA/DESTRA LA SCELTA", TENUE, 1);
    casella(tasto_modulo(false), "ESC  ANNULLA", false);
    casella(tasto_modulo(true), m.campo == 3 ? "INVIO  APRI IL FASCICOLO" : "APRI IL FASCICOLO", true);
    if (m.campo < 3) {
        const std::string s = "INVIO: AVANTI";
        f.scrivi(t, tasto_modulo(false).x + tasto_modulo(false).w + 12, tasto_modulo(true).y + (26 - f.righe()) / 2, s, ROSSO, 1);
    }
}

}  // namespace m688
