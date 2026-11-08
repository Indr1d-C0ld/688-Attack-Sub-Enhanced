#include "schermata_siluri.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <utility>

#include "grafica/sagome_armi.hpp"
#include "stazione_radio.hpp"

namespace m688 {

namespace {

constexpr int TESTATA = 28, PIEDE_Y = 448;
constexpr int DEP_X = 0, DEP_Y = 32, DEP_W = 160, DEP_H = 300;
constexpr int PIA_X = 160, PIA_Y = 32, PIA_W = 480, PIA_H = 300;
constexpr int SCH_Y = 332, SCH_H = 112;
constexpr int RIGA = 12;
constexpr int PULSANTI = 5, PULSANTE_W = 120, PULSANTE_PASSO = 126;
// l'elenco copre tutta la pianta, sotto la sua testata
constexpr int ELE_X = PIA_X + 8, ELE_W = PIA_W - 16, ELE_Y = PIA_Y + 16, ELE_H = PIA_H - 18, ELE_TITOLO = 20, ELE_RIGA = 23;

std::string maiuscolo(std::string s) {
    for (auto& c : s) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return s;
}

std::string virgola(double v, int decimali = 1) {   // 22,5 all'italiana
    char b[32];
    std::snprintf(b, sizeof b, "%.*f", decimali, v);
    std::string s = b;
    std::replace(s.begin(), s.end(), '.', ',');
    return s;
}

std::uint8_t colore_arma(const SchedaArma& a) {
    switch (a.tipo) {
        case TipoArmaCarico::siluro: return colori::VERDE_CHIARO;
        case TipoArmaCarico::missile: return colori::CIANO_CHIARO;
        case TipoArmaCarico::mina: return colori::GIALLO;
        default: return colori::GRIGIO;
    }
}

void cornice(Tela& t, int x, int y, int w, int h, std::uint8_t col) {
    t.linea_orizzontale(x, x + w - 1, y, col);
    t.linea_orizzontale(x, x + w - 1, y + h - 1, col);
    t.linea_verticale(x, y, y + h - 1, col);
    t.linea_verticale(x + w - 1, y, y + h - 1, col);
}

std::string taglia(const Font& f, std::string s, int w) {
    if (f.larghezza(s, 1) <= w) return s;
    while (!s.empty() && f.larghezza(s + ".", 1) > w) s.pop_back();
    return s + ".";
}

// la sigla di un'arma: l'ultima parte dell'id (53-65K, ADCAP), per intero se e' una lettera sola (TLAM_C)
std::string sigla(const std::string& id) {
    const std::string s = maiuscolo(id);
    const auto us = s.rfind('_');
    return us == std::string::npos || s.size() - us <= 2 ? s : s.substr(us + 1);
}

// il contenuto di un carico in una riga
std::string contenuto(const CaricoPartenza& c) {
    std::map<std::string, int> tubi;
    for (std::size_t i = 0; i < c.tipi.size() && i < c.posizioni.size(); ++i)
        if (c.tipi[i] == "tubo" && !c.posizioni[i].empty()) ++tubi[c.posizioni[i]];
    std::string s;
    const auto parte = [&s](const char* nome, const std::map<std::string, int>& m) {
        std::string p;
        for (const auto& [id, n] : m)
            if (n > 0) p += " " + std::to_string(n) + " " + sigla(id);
        if (!p.empty()) s += (s.empty() ? "" : "  ") + std::string(nome) + p;
    };
    parte("TUBI", tubi);
    parte("RAST", c.rastrelliera);
    parte("POZZI", c.pozzi);
    return s;
}

// le armi per conto, tubi compresi
std::map<std::string, int> conto_armi(const CaricoPartenza& c) {
    std::map<std::string, int> n;
    for (std::size_t i = 0; i < c.tipi.size() && i < c.posizioni.size(); ++i)
        if (c.tipi[i] == "tubo" && !c.posizioni[i].empty()) ++n[c.posizioni[i]];
    for (const auto* m : {&c.rastrelliera, &c.pozzi})
        for (const auto& [id, q] : *m) n[id] += q;
    return n;
}

// che cosa dello standard non sale: «12 MK67, 2 MK48»
std::string armi_mancanti(const CaricoPartenza& c, const CaricoStandard& s) {
    std::map<std::string, int> vuole;
    for (const auto& id : s.tubi) ++vuole[id];
    for (const auto* m : {&s.rastrelliera, &s.pozzi})
        for (const auto& [id, q] : *m) vuole[id] += q;
    const auto ci_sono = conto_armi(c);
    std::string r;
    for (const auto& [id, q] : vuole) {
        const auto it = ci_sono.find(id);
        const int meno = q - (it != ci_sono.end() ? it->second : 0);
        if (meno > 0) r += (r.empty() ? "" : ", ") + std::to_string(meno) + " " + sigla(id);
    }
    return r;
}

// il carico a bordo e' proprio quello standard del tipo?
bool uguale_allo_standard(const CaricoPartenza& c, const CaricoStandard& s) {
    std::size_t k = 0;
    for (std::size_t i = 0; i < c.tipi.size() && i < c.posizioni.size(); ++i)
        if (c.tipi[i] == "tubo" && (k >= s.tubi.size() || c.posizioni[i] != s.tubi[k++])) return false;
    const auto uguali = [](const std::map<std::string, int>& a, const std::map<std::string, int>& b) {
        std::map<std::string, int> x, y;
        for (const auto& [id, n] : a) if (n > 0) x[id] = n;
        for (const auto& [id, n] : b) if (n > 0) y[id] = n;
        return x == y;
    };
    return k == s.tubi.size() && uguali(c.rastrelliera, s.rastrelliera) && uguali(c.pozzi, s.pozzi);
}

}  // namespace

SchermataSiluri::SchermataSiluri(SalaSiluri& sala, std::string battello, int anno, std::string consigliato, std::map<std::string, CompitoCarico> compiti)
    : sala_(sala), battello_(std::move(battello)), consigliato_(std::move(consigliato)), anno_(anno), compiti_(std::move(compiti)) {
    const auto& cat = sala_.catalogo();
    if (const auto it = cat.carichi.find(sala_.locale().id); it != cat.carichi.end())
        for (const auto& [tipo, c] : it->second) tipi_.push_back(tipo);
    const auto ordine = [this](const std::string& tipo) {
        const auto it = compiti_.find(tipo);
        return it != compiti_.end() ? it->second.ordine : static_cast<int>(compiti_.size());
    };
    std::stable_sort(tipi_.begin(), tipi_.end(), [&](const std::string& a, const std::string& b) { return ordine(a) < ordine(b); });
    // lo scarto si misura sul tipo dichiarato dalla missione, come in carriera (esito.cpp): senza, niente scarto
    const std::string dichiarato = consigliato_;
    for (const std::string& tipo : tipi_) {
        SalaSiluri prova = sala_;   // una copia: la schermata non cambia il carico a bordo
        prova.carico_standard(tipo);
        const CaricoStandard& standard = cat.carichi.at(sala_.locale().id).at(tipo);
        VoceElenco v{tipo, prova.carico(), !uguale_allo_standard(prova.carico(), standard),
                     scarto_dal_standard(prova.carico(), cat, sala_.locale().id, dichiarato), armi_mancanti(prova.carico(), standard), false};
        v.doppione = std::any_of(voci_.begin(), voci_.end(), [&](const VoceElenco& x) { return x.carico == v.carico; });
        voci_.push_back(std::move(v));
    }
    if (consigliato_.empty())
        if (const auto it = cat.predefiniti.find(sala_.locale().id); it != cat.predefiniti.end()) consigliato_ = it->second;
    for (int i = 0; i < static_cast<int>(tipi_.size()); ++i)
        if (tipi_[static_cast<std::size_t>(i)] == consigliato_) tipo_ = i;
    armi_ = sala_.disponibili();
}

const std::string& SchermataSiluri::arma_scelta() const {
    static const std::string nessuna;
    return armi_.empty() ? nessuna : armi_[static_cast<std::size_t>(riga_)].id;
}

void SchermataSiluri::scegli(const std::string& arma) {
    for (int i = 0; i < static_cast<int>(armi_.size()); ++i)
        if (armi_[static_cast<std::size_t>(i)].id == arma) riga_ = i;
}

void SchermataSiluri::esito(const EsitoCarico& e) {
    if (!e.fatto) {
        avviso_ = e.motivo;
        avviso_s_ = 4.0;
    }
    armi_ = sala_.disponibili();
    conferma_ = false;
}

void SchermataSiluri::metti() {
    if (armi_.empty()) return;
    const SchedaArma& a = sala_.scheda(arma_scelta());
    if (a.tipo == TipoArmaCarico::esca) return esito(sala_.metti(Posto::esche, a.id));
    if (a.pozzo_verticale && sala_.locale().pozzi > 0) {
        const EsitoCarico e = sala_.metti(Posto::pozzi, a.id);
        if (e.fatto) return esito(e);
    }
    esito(sala_.metti(Posto::rastrelliera, a.id));
}

void SchermataSiluri::togli() {
    if (armi_.empty()) return;
    const SchedaArma& a = sala_.scheda(arma_scelta());
    if (a.tipo == TipoArmaCarico::esca) return esito(sala_.togli(Posto::esche, a.id));
    const EsitoCarico e = sala_.togli(Posto::rastrelliera, a.id);
    if (!e.fatto && a.pozzo_verticale && sala_.locale().pozzi > 0) return esito(sala_.togli(Posto::pozzi, a.id));
    esito(e);
}

int SchermataSiluri::y_riga(int i) const { return ELE_Y + ELE_TITOLO + i * ELE_RIGA; }

void SchermataSiluri::applica_tipo() {
    sala_.carico_standard(tipi_[static_cast<std::size_t>(tipo_)]);
    esito({true, ""});
    elenco_ = false;
}

void SchermataSiluri::comandi_elenco(const Comandi& c) {
    const int n = static_cast<int>(tipi_.size());
    if (const int su = passi_freccia(c.su_giu, c.passi_su_giu, prec_su_giu_); su != 0 && n > 0) tipo_ = std::clamp(tipo_ - su, 0, n - 1);
    if (c.tab && n > 0) tipo_ = (tipo_ + (c.tab > 0 ? 1 : n - 1)) % n;
    if (c.esc) elenco_ = false;
    if ((c.invio || std::find(c.lettere.begin(), c.lettere.end(), 'S') != c.lettere.end()) && n > 0) applica_tipo();
    if (elenco_ && std::find(c.lettere.begin(), c.lettere.end(), 'C') != c.lettere.end()) elenco_ = false;
    for (const auto& [x, y] : c.clic) {
        if (!elenco_) break;
        const int riga = y - ELE_Y - ELE_TITOLO;
        if (x >= ELE_X && x < ELE_X + ELE_W && riga >= 0 && riga < n * ELE_RIGA) {   // un clic su una riga la applica
            tipo_ = riga / ELE_RIGA;
            applica_tipo();
        } else if (y < ELE_Y || y >= ELE_Y + ELE_H || x < ELE_X || x >= ELE_X + ELE_W) {
            elenco_ = false;   // fuori: chiude senza cambiare niente
        }
    }
}

void SchermataSiluri::tubo(int n) {
    if (armi_.empty()) return;
    const CaricoPartenza& c = sala_.carico();
    int k = 0;
    std::string dentro;
    for (std::size_t i = 0; i < c.tipi.size(); ++i)
        if (c.tipi[i] == "tubo" && ++k == n) dentro = c.posizioni[i];
    if (n > k) return;
    const std::string& id = arma_scelta();
    if (dentro == id) return esito(sala_.togli(Posto::tubo, id, n));
    if (!dentro.empty()) sala_.togli(Posto::tubo, dentro, n);
    const EsitoCarico e = sala_.metti(Posto::tubo, id, n);
    if (!e.fatto && !dentro.empty()) sala_.metti(Posto::tubo, dentro, n);   // rifiutata: torna quella di prima
    esito(e);
}

SchermataSiluri::Uscita SchermataSiluri::comandi(const Comandi& c, double dt) {
    avviso_s_ = std::max(0.0, avviso_s_ - dt);
    if (conferma_) {   // «pronti a partire?»
        if (c.invio) return Uscita::in_mare;
        if (c.esc) conferma_ = false;
        return Uscita::nessuna;
    }
    if (elenco_) {
        comandi_elenco(c);
        prec_ruota_ = c.ruota;
        return Uscita::nessuna;
    }
    if (const int su = passi_freccia(c.su_giu, c.passi_su_giu, prec_su_giu_); su != 0 && !armi_.empty())
        riga_ = std::clamp(riga_ - su, 0, static_cast<int>(armi_.size()) - 1);
    for (int lato = passi_freccia(c.ruota, c.passi_ruota, prec_ruota_); lato != 0; lato -= (lato > 0) - (lato < 0)) lato > 0 ? metti() : togli();
    for (const int n : c.cifre) tubo(n);
    if (c.tab && !tipi_.empty())
        tipo_ = (tipo_ + (c.tab > 0 ? 1 : static_cast<int>(tipi_.size()) - 1)) % static_cast<int>(tipi_.size());
    for (const char l : c.lettere) {
        if (l == 'S' && !tipi_.empty()) sala_.carico_standard(tipi_[static_cast<std::size_t>(tipo_)]), esito({true, ""});
        if (l == 'X') sala_.svuota(), esito({true, ""});
        if (l == 'D') sala_.carico_predefinito(), esito({true, ""});
        if (l == 'C' && !tipi_.empty()) elenco_ = true;
    }
    for (const auto& [x, y] : c.clic) {
        if (x >= DEP_X && x < DEP_X + DEP_W && y >= DEP_Y + 16 && y < DEP_Y + DEP_H) {   // una riga del deposito
            const int primo = std::clamp(riga_ - 10, 0, std::max(0, static_cast<int>(armi_.size()) - 22));
            const int i = primo + (y - DEP_Y - 16) / RIGA;
            if (i < static_cast<int>(armi_.size())) riga_ = i;
        }
        if (y >= PIEDE_Y)
            for (int k = 0; k < PULSANTI; ++k) {
                if (x < 8 + k * PULSANTE_PASSO || x >= 8 + k * PULSANTE_PASSO + PULSANTE_W) continue;
                if (k == 0) return Uscita::briefing;
                if (k == 1 && !tipi_.empty()) elenco_ = true;
                if (k == 2) sala_.svuota(), esito({true, ""});
                if (k == 3) sala_.carico_predefinito(), esito({true, ""});
                if (k == 4) {
                    if (sala_.avvisi().empty()) return Uscita::in_mare;
                    conferma_ = true;
                }
            }
    }
    if (c.esc) return Uscita::briefing;
    if (c.invio) {
        if (sala_.avvisi().empty()) return Uscita::in_mare;
        conferma_ = true;
    }
    return Uscita::nessuna;
}

void SchermataSiluri::palette(Palette& p) const { tav_ = TavolozzaSagome::ricca(p, 96); }

const TavolozzaSagome& SchermataSiluri::tav() const { return tav_ ? *tav_ : TavolozzaSagome::fissa(); }

void SchermataSiluri::disegna(Tela& t, const Font& f) const {
    t.riempi(colori::NERO);
    const std::string titolo = "SALA SILURI   " + maiuscolo(battello_);
    f.scrivi(t, 8, 8, sala_.locale().nazione == "urss" ? "SEKRETNO" : "SECRET", colori::ROSSO, 1);
    f.scrivi(t, LARGHEZZA_TELA / 2 - f.larghezza(titolo, 1) / 2, 8, titolo, colori::GIALLO, 1);
    const std::string anno = "ANNO " + std::to_string(anno_);
    f.scrivi(t, LARGHEZZA_TELA - 8 - f.larghezza(anno, 1), 8, anno, colori::ROSSO, 1);
    t.linea_orizzontale(0, LARGHEZZA_TELA - 1, TESTATA - 2, colori::GRIGIO_SCURO);
    disegna_deposito(t, f);
    disegna_pianta(t, f);
    disegna_scheda(t, f);

    t.linea_orizzontale(0, LARGHEZZA_TELA - 1, PIEDE_Y - 2, colori::GRIGIO_SCURO);
    std::string standard = "C CARICHI";
    if (!tipi_.empty()) {
        const std::string& tipo = tipi_[static_cast<std::size_t>(tipo_)];
        standard = "C " + maiuscolo(tipo == "attacco_terra" ? "attacco" : tipo) + (tipo == consigliato_ ? " *" : "");
    }
    const std::string testi[PULSANTI] = {"ESC BRIEFING", standard, "X SVUOTA", "D DOTAZIONE", "INVIO IN MARE"};
    for (int k = 0; k < PULSANTI; ++k) {
        const int x = 8 + k * PULSANTE_PASSO;
        t.rettangolo_pieno(x, PIEDE_Y + 6, PULSANTE_W, 20, colori::GRIGIO_SCURO);
        f.scrivi(t, x + PULSANTE_W / 2 - f.larghezza(testi[k], 1) / 2, PIEDE_Y + 12, testi[k],
                 k == 4 ? colori::GIALLO : colori::BIANCO, 1);
    }

    if (elenco_) disegna_elenco(t, f);
    if (conferma_) {   // la conferma con gli avvisi, sopra la pianta
        const int x0 = PIA_X + 60, y0 = PIA_Y + 70, w = PIA_W - 120;
        const auto avvisi = sala_.avvisi();
        const int h = 50 + static_cast<int>(avvisi.size()) * 11;
        t.rettangolo_pieno(x0, y0, w, h, colori::NERO);
        cornice(t, x0, y0, w, h, colori::GIALLO);
        f.scrivi(t, x0 + 10, y0 + 8, "PRONTI A PARTIRE?", colori::GIALLO, 1);
        int y = y0 + 22;
        for (const auto& a : avvisi) {
            f.scrivi(t, x0 + 10, y, taglia(f, maiuscolo(a), w - 20), colori::BIANCO, 1);
            y += 11;
        }
        f.scrivi(t, x0 + 10, y0 + h - 14, "INVIO IN MARE   ESC TORNA AL CARICO", colori::GRIGIO, 1);
    }
}

void SchermataSiluri::disegna_deposito(Tela& t, const Font& f) const {
    f.scrivi(t, DEP_X + 4, DEP_Y + 2, "DEPOSITO", colori::CIANO_CHIARO, 1);
    f.scrivi(t, DEP_X + DEP_W - 6 - f.larghezza("RESTANO", 1), DEP_Y + 2, "RESTANO", colori::GRIGIO, 1);
    const int visibili = (DEP_H - 18) / RIGA;
    const int n = static_cast<int>(armi_.size());
    const int primo = std::clamp(riga_ - visibili / 2, 0, std::max(0, n - visibili));
    for (int i = primo; i < std::min(n, primo + visibili); ++i) {
        const ArmaDisponibile& a = armi_[static_cast<std::size_t>(i)];
        const SchedaArma& s = sala_.scheda(a.id);
        const int y = DEP_Y + 16 + (i - primo) * RIGA;
        const bool qui = i == riga_;
        if (qui) t.rettangolo_pieno(DEP_X, y - 2, DEP_W - 2, RIGA, colori::BLU);
        const std::uint8_t col = a.negata ? colori::GRIGIO_SCURO : a.rimaste() > 0 ? colore_arma(s) : colori::GRIGIO;
        t.rettangolo_pieno(DEP_X + 4, y + 1, 4, 5, col);
        const std::string q = a.negata ? "-" : std::to_string(a.rimaste());
        const int wq = f.larghezza(q, 1);
        f.scrivi(t, DEP_X + 12, y, taglia(f, maiuscolo(s.nome), DEP_W - 26 - wq), a.negata ? colori::GRIGIO : colori::BIANCO, 1);
        f.scrivi(t, DEP_X + DEP_W - 6 - wq, y, q, a.negata ? colori::GRIGIO_SCURO : colori::GIALLO, 1);
    }
    t.linea_verticale(DEP_X + DEP_W - 1, DEP_Y, DEP_Y + DEP_H - 1, colori::GRIGIO_SCURO);
}

void SchermataSiluri::disegna_pianta(Tela& t, const Font& f) const {
    const LocaleSiluri& loc = sala_.locale();
    const CaricoPartenza& c = sala_.carico();
    f.scrivi(t, PIA_X + 8, PIA_Y + 2, "LOCALE SILURI  " + maiuscolo(loc.nome), colori::CIANO_CHIARO, 1);
    const auto scheda = [&](const std::string& id) -> const SchedaArma* { return sala_.catalogo().arma(id); };

    // i tubi a prua (a destra), uno per riga, con l'arma che ci sta
    int k = 0, ultimo_y = PIA_Y + 18;
    for (std::size_t i = 0; i < c.tipi.size(); ++i) {
        if (c.tipi[i] != "tubo") continue;
        ++k;
        // il tubo in sezione con l'arma caricata in porto (asciutto, porta chiusa), il nome a sinistra
        const int y = PIA_Y + 18 + (k - 1) * 20, x = PIA_X + 300, w = 172;
        const SchedaArma* a = c.posizioni[i].empty() ? nullptr : scheda(c.posizioni[i]);
        SagomaInTubo st;
        if (a) st.arma = sagoma_da_scheda(*a), st.stato = FaseTubo::carico;
        disegna_tubo(t, {x, y, w, 18}, st, tav());
        const std::string nome = a ? taglia(f, maiuscolo(a->nome), 118) : std::string("VUOTO");
        f.scrivi(t, x - 20 - f.larghezza(nome, 1), y + 6, nome, a ? colore_arma(*a) : colori::GRIGIO_SCURO, 1);
        f.scrivi(t, x - 12, y + 6, std::to_string(k), colori::GIALLO, 1);
        ultimo_y = y + 20;
    }

    // pozzi verticali (Flight II e 688I) ed esche, a sinistra dei tubi
    int y = PIA_Y + 20;
    if (loc.pozzi > 0) {
        f.scrivi(t, PIA_X + 8, y, "POZZI VERTICALI", colori::GRIGIO, 1);
        std::vector<std::uint8_t> celle;
        for (const auto& [id, n] : c.pozzi)
            if (const SchedaArma* a = scheda(id))
                for (int j = 0; j < n; ++j) celle.push_back(colore_arma(*a));
        for (int j = 0; j < loc.pozzi; ++j) {
            const int cx = PIA_X + 8 + (j % 6) * 18, cy = y + 12 + (j / 6) * 18;
            if (j < static_cast<int>(celle.size())) t.rettangolo_pieno(cx + 1, cy + 1, 12, 12, celle[static_cast<std::size_t>(j)]);
            cornice(t, cx, cy, 14, 14, colori::GRIGIO);
        }
        y += 12 + ((loc.pozzi + 5) / 6) * 18 + 6;
    }
    {
        int esche = 0;
        for (const auto& [id, n] : c.esche) esche += n;
        f.scrivi(t, PIA_X + 8, y, "ESCHE " + std::to_string(esche) + " / " + std::to_string(loc.esche), colori::GRIGIO, 1);
        for (int j = 0; j < loc.esche; ++j) {
            const int cx = PIA_X + 8 + (j % 12) * 9, cy = y + 12 + (j / 12) * 9;
            t.rettangolo_pieno(cx, cy, 7, 7, j < esche ? colori::GRIGIO : colori::GRIGIO_SCURO);
        }
        y += 12 + ((loc.esche + 11) / 12) * 9 + 6;
    }

    // la rastrelliera: i posti condivisi meno i tubi, ogni arma prende i suoi posti
    const int tubi = k, slot = std::max(0, sala_.posti_totali() - tubi);
    const int ry = std::max(ultimo_y, y) + 6;
    f.scrivi(t, PIA_X + 8, ry, "RASTRELLIERA  " + std::to_string(slot) + " POSTI", colori::GRIGIO, 1);
    const int per_riga = 12, sw = 36, sh = 22;
    std::vector<const SchedaArma*> celle(static_cast<std::size_t>(slot), nullptr);
    double pos = 0.0;
    for (const auto& [id, n] : c.rastrelliera)
        if (const SchedaArma* a = scheda(id))
            for (int j = 0; j < n; ++j) {
                const auto cella = static_cast<std::size_t>(pos);
                if (cella < celle.size()) celle[cella] = a;
                pos += a->posti;
            }
    for (int j = 0; j < slot; ++j) {
        const int cx = PIA_X + 8 + (j % per_riga) * (sw + 2), cy = ry + 12 + (j / per_riga) * (sh + 2);
        const SchedaArma* a = celle[static_cast<std::size_t>(j)];
        if (a) {
            // la sagoma a scala comune (4 px/m: ci sta il PMR-1 da 7,8 m) e sotto la sigla, l'ultima parte dell'id
            t.rettangolo_pieno(cx, cy, sw, sh, colori::GRIGIO_SCURO);
            t.rettangolo_pieno(cx, cy, 3, sh, colore_arma(*a));
            disegna_sagoma_arma(t, sagoma_da_scheda(*a), {cx + 3, cy + 1, sw - 4, 9}, 4.0, tav());
            std::string breve = maiuscolo(a->id);
            if (const auto us = breve.rfind('_'); us != std::string::npos) breve = breve.substr(us + 1);
            f.scrivi(t, cx + 5, cy + 12, taglia(f, breve, sw - 7), colore_arma(*a), 1);
        } else {
            cornice(t, cx, cy, sw, sh, colori::GRIGIO_SCURO);
        }
    }

    // la legenda della rastrelliera
    int ly = ry + 12 + ((slot + per_riga - 1) / per_riga) * (sh + 2) + 4, lx = PIA_X + 8;
    for (const auto& [id, n] : c.rastrelliera)
        if (const SchedaArma* a = scheda(id); a && n > 0) {
            const std::string voce = maiuscolo(a->nome) + " X" + std::to_string(n);
            const int w = f.larghezza(voce, 1) + 16;
            if (lx + w > PIA_X + PIA_W - 8) lx = PIA_X + 8, ly += 11;
            t.rettangolo_pieno(lx, ly + 1, 4, 5, colore_arma(*a));
            f.scrivi(t, lx + 7, ly, voce, colori::GRIGIO, 1);
            lx += w;
        }

    // i totali (da Falcon 3.0: al posto di peso e resistenza i posti), gli avvisi, l'ultimo rifiuto
    const int ty = ly + 18;
    char riga[160];
    std::snprintf(riga, sizeof riga, "POSTI %s / %d   PRONTI NEI TUBI %d / %d", virgola(sala_.posti_usati()).c_str(),
                  sala_.posti_totali(), sala_.pronti_nei_tubi(), tubi);
    std::string s = riga;
    if (loc.pozzi > 0) s += "   POZZI " + std::to_string(sala_.pozzi_usati()) + " / " + std::to_string(loc.pozzi);
    s += "   PESO " + virgola(sala_.peso_kg() / 1000.0) + " T";
    f.scrivi(t, PIA_X + 8, ty, s, colori::BIANCO, 1);
    int ay = ty + 12;
    if (avviso_s_ > 0.0 && !avviso_.empty()) {
        f.scrivi(t, PIA_X + 8, ay, "NO: " + maiuscolo(avviso_), colori::ROSSO, 1);
        ay += 11;
    }
    for (const auto& a : sala_.avvisi()) {
        if (ay > PIA_Y + PIA_H - 12) break;
        f.scrivi(t, PIA_X + 8, ay, maiuscolo(a), colori::GIALLO, 1);
        ay += 11;
    }
}

void SchermataSiluri::disegna_elenco(Tela& t, const Font& f) const {
    t.rettangolo_pieno(PIA_X + 2, ELE_Y - 2, PIA_W - 2, PIA_H - 14, colori::NERO);   // copre la pianta
    cornice(t, ELE_X, ELE_Y, ELE_W, ELE_H, colori::GIALLO);
    f.scrivi(t, ELE_X + 8, ELE_Y + 6, "CARICHI PER COMPITO", colori::GIALLO, 1);
    const auto compito = [this](const std::string& tipo) -> const CompitoCarico* {
        const auto it = compiti_.find(tipo);
        return it != compiti_.end() ? &it->second : nullptr;
    };
    for (int i = 0; i < static_cast<int>(voci_.size()); ++i) {
        const VoceElenco& v = voci_[static_cast<std::size_t>(i)];
        const int y = y_riga(i);
        const bool qui = i == tipo_, suggerito = v.tipo == consigliato_;
        if (qui) t.rettangolo_pieno(ELE_X + 2, y - 1, ELE_W - 4, ELE_RIGA - 1, colori::BLU);
        const CompitoCarico* c = compito(v.tipo);
        // a destra le etichette: a bordo, lo scarto che pesa in carriera, adattato, consigliato
        int x = ELE_X + ELE_W - 8;
        const auto etichetta = [&](const std::string& s, std::uint8_t col) {
            x -= f.larghezza(s, 1);
            f.scrivi(t, x, y + 1, s, col, 1);
            x -= 8;
        };
        if (sala_.carico() == v.carico) etichetta("A BORDO", colori::CIANO_CHIARO);
        if (suggerito) etichetta("CONSIGLIATO", colori::GIALLO);
        if (v.scarto >= 0.005) etichetta("SCARTO " + std::to_string(static_cast<int>(std::lround(v.scarto * 100.0))) + "/100", colori::GRIGIO);
        if (v.adattato) etichetta("ADATTATO", colori::ROSSO);
        // il nome nello spazio che resta; grigio se qui sale uguale a una riga sopra: con questo deposito non e' una scelta in piu'
        const std::string nome = maiuscolo(c && !c->nome.empty() ? c->nome : v.tipo);
        f.scrivi(t, ELE_X + 8, y + 1, taglia(f, nome, x - ELE_X - 8), suggerito ? colori::GIALLO : v.doppione ? colori::GRIGIO : colori::BIANCO, 1);
        f.scrivi(t, ELE_X + 8, y + 11, taglia(f, contenuto(v.carico), ELE_W - 16), colori::GRIGIO, 1);
    }
    // il compito scelto: che cosa vuole dire, che cosa qui non sale, da dove viene; in fondo alla cornice
    const int yp = ELE_Y + ELE_H - 46;
    if (!voci_.empty()) {
        const VoceElenco& v = voci_[static_cast<std::size_t>(tipo_)];
        if (const CompitoCarico* c = compito(v.tipo)) {
            f.scrivi(t, ELE_X + 8, yp, taglia(f, maiuscolo(c->nota), ELE_W - 16), colori::BIANCO, 1);
            f.scrivi(t, ELE_X + 8, yp + 22, taglia(f, maiuscolo("FONTE: " + c->fonte + " (" + c->confidenza + ")"), ELE_W - 16), colori::GRIGIO, 1);
        }
        if (!v.mancano.empty()) f.scrivi(t, ELE_X + 8, yp + 11, taglia(f, "NON AL DEPOSITO: " + v.mancano, ELE_W - 16), colori::ROSSO, 1);
    }
    f.scrivi(t, ELE_X + 8, yp + 33, "FRECCE SCEGLI   INVIO APPLICA   ESC CHIUDE", colori::GRIGIO, 1);
}

void SchermataSiluri::disegna_scheda(Tela& t, const Font& f) const {
    t.linea_orizzontale(0, LARGHEZZA_TELA - 1, SCH_Y, colori::GRIGIO_SCURO);
    if (armi_.empty()) return;
    const ArmaDisponibile& d = armi_[static_cast<std::size_t>(riga_)];
    const SchedaArma& a = sala_.scheda(d.id);
    t.rettangolo_pieno(4, SCH_Y + 6, 192, 64, colori::BLU);
    cornice(t, 4, SCH_Y + 6, 192, 64, colori::GRIGIO);
    // in scala comune per tutte le armi (22 px/m: la capsula del Tomahawk ci sta), centrata
    disegna_sagoma_arma(t, sagoma_da_scheda(a), {8, SCH_Y + 10, 184, 44}, 22.0, tav());
    if (a.lunghezza_m > 0.0) {
        const std::string l = virgola(a.lunghezza_m, 2) + " M";
        f.scrivi(t, 192 - f.larghezza(l, 1), SCH_Y + 58, l, colori::GRIGIO, 1);
    }
    // sotto la sagoma: quante e il divieto
    std::string stato = d.negata ? "NON DISPONIBILE: " + maiuscolo(d.motivo)
                                 : "AL DEPOSITO " + std::to_string(d.deposito) + ", A BORDO " + std::to_string(d.caricate);
    f.scrivi(t, 4, SCH_Y + 76, taglia(f, stato, 192), d.negata ? colori::ROSSO : colori::BIANCO, 1);
    // la MOSS e' esistita, ma non sui 688 del 1989: what-if del modo moderno; la Sea Lance non e' mai entrata in servizio
    if (a.what_if) f.scrivi(t, 4, SCH_Y + 88, a.moderno ? "WHAT-IF: MAI SUI 688 (MODERNO)" : "WHAT-IF (MAI IN SERVIZIO)", colori::GIALLO, 1);

    // i dati, in due colonne
    const int x1 = 210, x2 = 430;
    f.scrivi(t, x1, SCH_Y + 6, maiuscolo(a.nome), colore_arma(a), 1);
    const auto valore = [&](int x, int y, const char* nome, const std::string& v) {
        f.scrivi(t, x, y, nome, colori::GRIGIO, 1);
        f.scrivi(t, x + 70, y, v.empty() ? "-" : v, colori::BIANCO, 1);
    };
    const auto o = [](double v, const std::string& u, int dec = 0) { return v > 0.0 ? virgola(v, dec) + " " + u : std::string(); };
    int y = SCH_Y + 20;
    valore(x1, y, "GUIDA", taglia(f, maiuscolo(a.guida), x2 - x1 - 80));
    valore(x2, y, "IN SERVIZIO", a.in_servizio_dal ? std::to_string(a.in_servizio_dal) +
                                                        (a.ritirata_nel ? "-" + std::to_string(a.ritirata_nel) : "")
                                                  : "MAI");
    y += 11;
    valore(x1, y, "VELOCITA'", o(a.velocita_kn, "NODI"));
    valore(x2, y, "POSTI", virgola(a.posti, a.posti == std::floor(a.posti) ? 0 : 2) + (a.pozzo_verticale ? " O POZZO" : ""));
    y += 11;
    valore(x1, y, "PORTATA", o(a.portata_km, "KM"));
    valore(x2, y, "PESO", o(a.peso_kg, "KG"));
    y += 11;
    valore(x1, y, "TESTATA", a.nucleare ? "NUCLEARE" : o(a.testata_kg, "KG"));
    valore(x2, y, "DIAMETRO", std::to_string(a.diametro_mm) + " MM");
    y += 13;
    // l'efficacia per bersaglio (da F-117A): da 0 a 3 stelle, STIMA del gioco con la sua nota
    {
        static constexpr std::pair<const char*, const char*> BERSAGLI[] = {
            {"sottomarino", "SOTTOMARINI"}, {"superficie", "NAVI"}, {"terra", "TERRA"}, {"mine", "MINE"}};
        int x = x1;
        f.scrivi(t, x, y, "EFFICACIA", colori::GRIGIO, 1);
        x += 70;
        for (const auto& [chiave, nome] : BERSAGLI) {
            x = f.scrivi(t, x, y, nome, colori::BIANCO, 1) + 4;
            const int n = a.stelle(chiave);
            for (int k = 0; k < 3; ++k) {   // una stella: un rombo pieno o vuoto
                const std::uint8_t col = k < n ? colori::GIALLO : colori::GRIGIO_SCURO;
                for (int r = 0; r < 4; ++r) t.linea_orizzontale(x + 3 - r, x + 3 + r, y + 4 - (3 - r), col),
                                            t.linea_orizzontale(x + 3 - r, x + 3 + r, y + 4 + (3 - r), col);
                x += 9;
            }
            x += 8;
        }
        y += 13;
    }
    std::string fonte = "FONTE: " + a.fonte + (a.confidenza.empty() ? "" : " (" + a.confidenza + ")");
    for (const auto& l : StazioneRadio::a_capo(f, maiuscolo(fonte), LARGHEZZA_TELA - x1 - 8)) {
        if (y > SCH_Y + SCH_H - 12) break;
        f.scrivi(t, x1, y, l, colori::GRIGIO_SCURO, 1);
        y += 11;
    }
    if (!a.nota.empty())
        for (const auto& l : StazioneRadio::a_capo(f, maiuscolo(a.nota), LARGHEZZA_TELA - x1 - 8)) {
            if (y > SCH_Y + SCH_H - 12) break;
            f.scrivi(t, x1, y, l, colori::GRIGIO, 1);
            y += 11;
        }
}

}  // namespace m688
