#include "pannello_interventi.hpp"

#include <algorithm>
#include <cmath>
#include <cctype>
#include <cstdio>

#include "grafica/cornici.hpp"
#include "grafica/palette.hpp"
#include "grafica/quadro.hpp"
#include "grafica/tema_stazioni.hpp"

namespace m688 {

namespace {

constexpr const char* DURATA = "durata_min";   // i due parametri che il pannello aggiunge a ogni tipo
constexpr const char* FRA = "fra_min";
constexpr double RIPETI_S = 0.12;              // le frecce tenute: un passo ogni 0,12 s

std::string orologio(double s) {
    const int m = static_cast<int>(s / 60.0), r = static_cast<int>(s) % 60;
    char b[16];
    std::snprintf(b, sizeof b, "%02d:%02d", m, r);
    return b;
}

// il testo tagliato alla larghezza w, coi puntini
std::string taglia(const Font& f, std::string s, int w) {
    if (f.larghezza(s, 1) <= w) return s;
    while (!s.empty() && f.larghezza(s + "..", 1) > w) s.pop_back();
    return s + "..";
}

std::string maiu(std::string s) {   // i nomi stabili del motore («sonar») come li legge lo spettatore
    for (char& c : s) c = (c == '_') ? ' ' : static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return s;
}

// il nome di un parametro nella riga: il testo fino alla parentesi («VELOCITA' (NODI)» -> «VELOCITA'»)
std::string nome_breve(const std::string& testo) {
    const auto k = testo.find(" (");
    return k == std::string::npos ? testo : testo.substr(0, k);
}

bool invariato(const ParametroPannello& p) { return p.facoltativo && p.scelte.empty() && p.valore < p.minimo - 1e-9; }

int indice(const ParametroPannello& p) {
    return std::clamp(static_cast<int>(std::lround(p.valore)), 0, static_cast<int>(p.scelte.size()) - 1);
}

std::string valore_testo(const ParametroPannello& p) {
    if (!p.scelte.empty()) {
        const auto i = static_cast<std::size_t>(indice(p));
        if (p.scelte[i].empty()) return "NESSUNO";
        return i < p.etichette.size() ? p.etichette[i] : maiu(p.scelte[i]);
    }
    if (invariato(p)) return "INVARIATO";
    char b[32];
    if (p.unita.empty() && p.minimo == 0.0 && p.massimo == 1.0) {   // una frazione: in percento
        std::snprintf(b, sizeof b, "%.0f %%", p.valore * 100.0);
        return b;
    }
    if (std::fabs(p.passo - std::round(p.passo)) < 1e-9) std::snprintf(b, sizeof b, "%.0f", p.valore);
    else std::snprintf(b, sizeof b, "%.1f", p.valore);
    return std::string(b) + (p.unita.empty() ? "" : " " + p.unita);
}

ParametroPannello parametro(std::string nome, std::string testo, double minimo, double massimo, double passo, double valore,
                              std::string unita = {}) {
    ParametroPannello p;
    p.nome = std::move(nome);
    p.testo = std::move(testo);
    p.minimo = minimo, p.massimo = massimo, p.passo = passo, p.valore = valore;
    p.unita = std::move(unita);
    return p;
}

ParametroPannello scelta(std::string nome, std::string testo, std::vector<std::string> voci, bool come_testo = true) {
    ParametroPannello p = parametro(std::move(nome), std::move(testo), 0.0, static_cast<double>(voci.size() - 1), 1.0, 0.0);
    p.scelte = std::move(voci);
    p.come_testo = come_testo;
    return p;
}

}  // namespace

std::vector<VocePannello> catalogo_pannello(const std::vector<VoceCatalogoIntervento>& c,
                                            const std::vector<std::pair<std::string, std::string>>& classi) {
    std::vector<VocePannello> v;
    for (const auto& x : c) {
        VocePannello p;
        p.gruppo = x.gruppo;
        p.tipo = x.tipo;
        p.testo = maiu(x.testo_it);
        p.vuole_unita = x.vuole_unita;
        p.vuole_punto = x.vuole_punto;
        p.punto_facoltativo = x.punto_facoltativo;
        p.vuole_raggio = x.vuole_raggio;
        p.ha_durata = x.ha_durata;
        p.categorie = x.categorie;
        for (const auto& t : x.testi) {   // prima i testi (la classe, la parte), poi i numeri
            std::vector<std::string> voci = t.scelte, etichette;
            if (voci.empty() && t.nome == "classe")
                for (const auto& [chiave, nome] : classi) voci.push_back(chiave), etichette.push_back(maiu(nome));
            if (voci.empty()) continue;   // un testo libero (il gestore): non dal pannello
            if (t.facoltativo) {
                voci.insert(voci.begin(), std::string());
                if (!etichette.empty()) etichette.insert(etichette.begin(), "NESSUNO");
            }
            ParametroPannello q = scelta(t.nome, maiu(t.testo_it), std::move(voci));
            q.etichette = std::move(etichette);
            q.facoltativo = t.facoltativo;
            p.parametri.push_back(std::move(q));
        }
        if (x.vuole_raggio) p.parametri.push_back(parametro("raggio_m", "RAGGIO (0: TUTTO IL TEATRO)", 0, 100000, 5000, 0, "M"));
        for (const auto& n : x.parametri) {
            if (n.nome == "bersaglio") {   // un id: si sceglie dalla lista delle unita', in un passo suo
                p.vuole_bersaglio = true;
                continue;
            }
            ParametroPannello q = parametro(n.nome, maiu(n.testo_it), n.minimo, n.massimo, n.passo, n.predefinito);
            q.facoltativo = n.facoltativo;
            if (q.facoltativo) q.valore = q.minimo - q.passo;   // si parte da «invariato»
            p.parametri.push_back(std::move(q));
        }
        v.push_back(std::move(p));
    }
    return v;
}

Intervento intervento_di(const RichiestaIntervento& q) {
    Intervento x;
    x.tempo = q.subito ? -1.0 : q.tempo;
    x.tipo = q.tipo;
    x.unita = q.unita;
    x.punto = q.punto;
    x.raggio_m = q.raggio_m;
    x.parametri = q.parametri;
    x.testi = q.testi;
    x.durata_s = q.durata_s;
    return x;
}

void PannelloInterventi::apri(double tempo_partita, std::vector<UnitaPannello> unita) {
    aperto_ = true;
    passo_ = Passo::gruppo;
    cursore_ = 0;
    tempo_ = tempo_partita;
    unita_ = std::move(unita);
    punto_.reset();
    unita_scelta_ = bersaglio_scelto_ = -1;
}

std::vector<std::string> PannelloInterventi::gruppi() const {
    std::vector<std::string> g;
    for (const auto& x : catalogo_)
        if (std::find(g.begin(), g.end(), x.gruppo) == g.end()) g.push_back(x.gruppo);
    return g;
}

std::vector<int> PannelloInterventi::unita_adatte() const {
    std::vector<int> v;
    const auto& cat = tipo_scelto().categorie;
    for (int i = 0; i < static_cast<int>(unita_.size()); ++i)
        if (cat.empty() || std::find(cat.begin(), cat.end(), unita_[static_cast<std::size_t>(i)].categoria) != cat.end()) v.push_back(i);
    return v;
}

std::vector<int> PannelloInterventi::bersagli() const {
    std::vector<int> v;
    for (int i = 0; i < static_cast<int>(unita_.size()); ++i)
        if (unita_[static_cast<std::size_t>(i)].id != unita_scelta_) v.push_back(i);
    return v;
}

PannelloInterventi::Passo PannelloInterventi::dopo(Passo p) const {
    if (p == Passo::tipo && tipo_scelto().vuole_unita) return Passo::unita;
    if ((p == Passo::tipo || p == Passo::unita) && tipo_scelto().vuole_bersaglio) return Passo::bersaglio;
    if (p != Passo::punto && tipo_scelto().vuole_punto) return Passo::punto;
    return Passo::parametri;
}

std::vector<int> PannelloInterventi::tipi_del_gruppo() const {
    std::vector<int> v;
    for (int i = 0; i < static_cast<int>(catalogo_.size()); ++i)
        if (catalogo_[static_cast<std::size_t>(i)].gruppo == gruppo_) v.push_back(i);
    return v;
}

int PannelloInterventi::voci() const {
    switch (passo_) {
        case Passo::gruppo: return static_cast<int>(gruppi().size());
        case Passo::tipo: return static_cast<int>(tipi_del_gruppo().size());
        case Passo::unita: return static_cast<int>(unita_adatte().size());
        case Passo::bersaglio: return static_cast<int>(bersagli().size());
        case Passo::punto: return 0;
        case Passo::parametri: return static_cast<int>(valori_.size()) + 1;   // l'ultima voce: APPLICA
    }
    return 0;
}

void PannelloInterventi::punto_scelto(Vec2 p) {
    if (!aspetta_punto()) return;
    punto_ = p;
    passo_ = Passo::parametri;
    cursore_ = 0;
}

RichiestaIntervento PannelloInterventi::richiesta() const {
    RichiestaIntervento q;
    q.tipo = tipo_scelto().tipo;
    q.unita = unita_scelta_;
    q.punto = punto_;
    if (tipo_scelto().vuole_bersaglio) q.parametri["bersaglio"] = static_cast<double>(bersaglio_scelto_);
    for (const auto& p : valori_) {
        if (p.nome == DURATA) q.durata_s = p.valore * 60.0;
        else if (p.nome == FRA) q.tempo = tempo_ + p.valore * 60.0, q.subito = p.valore <= 0.0;
        else if (p.nome == "raggio_m" && tipo_scelto().vuole_raggio) q.raggio_m = p.valore;
        else if (invariato(p)) continue;
        else if (p.come_testo && !p.scelte.empty()) {
            if (const std::string& t = p.scelte[static_cast<std::size_t>(indice(p))]; !t.empty()) q.testi[p.nome] = t;
        } else q.parametri[p.nome] = p.valore;
    }
    q.riga = riga(q);
    return q;
}

std::string PannelloInterventi::riga(const RichiestaIntervento& q) const {
    std::string s = orologio(q.tempo) + " ";
    const auto t = std::find_if(catalogo_.begin(), catalogo_.end(), [&](const VocePannello& x) { return x.tipo == q.tipo; });
    s += t != catalogo_.end() ? t->testo : q.tipo;
    if (q.unita >= 0) {
        const auto u = std::find_if(unita_.begin(), unita_.end(), [&](const UnitaPannello& x) { return x.id == q.unita; });
        s += ": " + (u != unita_.end() ? u->nome : "#" + std::to_string(q.unita));
    }
    if (const auto b = q.parametri.find("bersaglio"); b != q.parametri.end()) {   // la false flag: il bersaglio col suo nome
        const int id = static_cast<int>(std::lround(b->second));
        const auto u = std::find_if(unita_.begin(), unita_.end(), [&](const UnitaPannello& x) { return x.id == id; });
        s += " SU " + (u != unita_.end() ? u->nome : "#" + std::to_string(id));
    }
    if (t != catalogo_.end()) {
        for (const auto& p : t->parametri) {
            if (const auto it = q.testi.find(p.nome); it != q.testi.end()) {
                ParametroPannello v = p;
                const auto k = std::find(p.scelte.begin(), p.scelte.end(), it->second);
                v.valore = static_cast<double>(k - p.scelte.begin());
                s += ", " + (k != p.scelte.end() ? valore_testo(v) : maiu(it->second));
            } else if (const auto jt = q.parametri.find(p.nome); jt != q.parametri.end()) {
                ParametroPannello v = p;   // i numeri col loro nome: «LIVELLO 3», «VELOCITA' 15»
                v.valore = jt->second;
                s += ", " + nome_breve(p.testo) + " " + valore_testo(v);
            }
        }
    }
    if (q.durata_s > 0.0) s += " PER " + std::to_string(static_cast<int>(std::lround(q.durata_s / 60.0))) + " MIN";
    return s;
}

std::optional<RichiestaIntervento> PannelloInterventi::comandi(const Comandi& c, double dt) {
    if (!aperto_) return std::nullopt;
    if (c.esc) {   // indietro di un passo; dal primo chiude
        if (passo_ == Passo::gruppo) aperto_ = false;
        else if (passo_ == Passo::tipo) passo_ = Passo::gruppo;
        else if (passo_ == Passo::unita) passo_ = Passo::tipo;
        else if (passo_ == Passo::bersaglio) passo_ = tipo_scelto().vuole_unita ? Passo::unita : Passo::tipo;
        else {   // dai parametri il passo prima; dal punto quello prima del punto
            const bool punto = passo_ == Passo::parametri && tipo_scelto().vuole_punto;
            passo_ = punto ? Passo::punto : tipo_scelto().vuole_bersaglio ? Passo::bersaglio : tipo_scelto().vuole_unita ? Passo::unita : Passo::tipo;
        }
        cursore_ = 0;
        return std::nullopt;
    }
    const int n = voci();
    if (c.tab != 0 && n > 0) cursore_ = ((cursore_ + c.tab) % n + n) % n;
    // le frecce destra e sinistra cambiano il parametro sotto il cursore, un passo ogni RIPETI_S
    ripeti_ = std::max(0.0, ripeti_ - dt);
    const int verso = verso_freccia(c.ruota, c.passi_ruota);
    if (passo_ == Passo::parametri && verso != 0 && ripeti_ <= 0.0 && cursore_ < static_cast<int>(valori_.size())) {
        auto& p = valori_[static_cast<std::size_t>(cursore_)];
        const double basso = p.facoltativo && p.scelte.empty() ? p.minimo - p.passo : p.minimo;   // sotto: «invariato»
        p.valore = std::clamp(p.valore + verso * p.passo, basso, p.massimo);
        ripeti_ = RIPETI_S;
    }
    if (!c.invio) return std::nullopt;
    switch (passo_) {
        case Passo::gruppo:
            if (n == 0) return std::nullopt;
            gruppo_ = gruppi()[static_cast<std::size_t>(cursore_)];
            passo_ = Passo::tipo;
            cursore_ = 0;
            return std::nullopt;
        case Passo::tipo: {
            if (n == 0) return std::nullopt;
            tipo_ = tipi_del_gruppo()[static_cast<std::size_t>(cursore_)];
            valori_ = tipo_scelto().parametri;
            if (tipo_scelto().ha_durata) valori_.push_back(parametro(DURATA, "DURATA (0: PER SEMPRE)", 0, 240, 5, 0, "MIN"));
            valori_.push_back(parametro(FRA, "QUANDO: FRA (0: SUBITO)", 0, 240, 1, 0, "MIN"));
            unita_scelta_ = bersaglio_scelto_ = -1;
            punto_.reset();
            passo_ = dopo(Passo::tipo);
            cursore_ = 0;
            return std::nullopt;
        }
        case Passo::unita:
            if (n == 0) return std::nullopt;
            unita_scelta_ = unita_[static_cast<std::size_t>(unita_adatte()[static_cast<std::size_t>(cursore_)])].id;
            passo_ = dopo(Passo::unita);
            cursore_ = 0;
            return std::nullopt;
        case Passo::bersaglio:
            if (n == 0) return std::nullopt;
            bersaglio_scelto_ = unita_[static_cast<std::size_t>(bersagli()[static_cast<std::size_t>(cursore_)])].id;
            passo_ = dopo(Passo::bersaglio);
            cursore_ = 0;
            return std::nullopt;
        case Passo::punto:   // il punto arriva da punto_scelto (un clic sulla carta); Invio lo salta se e' facoltativo
            if (!tipo_scelto().punto_facoltativo) return std::nullopt;
            punto_.reset();
            passo_ = Passo::parametri;
            cursore_ = 0;
            return std::nullopt;
        case Passo::parametri:
            if (cursore_ != static_cast<int>(valori_.size())) return std::nullopt;   // Invio su APPLICA
            aperto_ = false;
            return richiesta();
    }
    return std::nullopt;
}

void PannelloInterventi::disegna(Tela& t, const Font& f, Rettangolo r) const {
    if (!aperto_) return;
    // il titolo: dove si e' (il gruppo, poi l'intervento), tagliato alla larghezza del riquadro
    const std::string titolo = passo_ == Passo::gruppo     ? "INTERVENTI"
                               : passo_ == Passo::tipo      ? "INTERVENTI - " + gruppo_
                               : passo_ == Passo::bersaglio ? "IL BERSAGLIO"
                                                            : tipo_scelto().testo;
    // la cornice del kit (grafica/cornici): la prima volta per l'area delle voci, che da' le righe di una pagina; la
    // seconda con la fascia accesa sulla riga del cursore, nella pagina che lo contiene
    const std::string testata = taglia(f, titolo, r.w - 8);
    const int h = f.righe() + 3;
    const int righe = std::max(1, (pannello_interventi(t, f, r, testata, -1).h - 2) / h);
    const bool a_pagine = passo_ == Passo::unita || passo_ == Passo::bersaglio || passo_ == Passo::parametri;
    const int inizio = a_pagine ? cursore_ / righe * righe : 0;
    const Rettangolo a = pannello_interventi(t, f, r, testata, passo_ == Passo::punto ? -1 : cursore_ - inizio);
    int y = a.y + 3;
    const auto voce = [&](int i, const std::string& s, const std::string& destra = {}) {
        const bool qui = i == cursore_;
        const int wd = destra.empty() ? 0 : f.larghezza(destra, 1) + 8;
        f.scrivi(t, a.x + 6, y, taglia(f, s, a.w - 12 - wd), qui ? colori::BIANCO : fosforo(6), 1);
        if (!destra.empty()) f.scrivi(t, a.x + a.w - 6 - f.larghezza(destra, 1), y, destra, qui ? colori::GIALLO : fosforo(6), 1);
        y += h;
    };
    switch (passo_) {
        case Passo::gruppo: {
            const auto g = gruppi();
            for (int i = 0; i < static_cast<int>(g.size()); ++i) voce(i, g[static_cast<std::size_t>(i)]);
            break;
        }
        case Passo::tipo: {
            const auto v = tipi_del_gruppo();
            for (int i = 0; i < static_cast<int>(v.size()); ++i) voce(i, catalogo_[static_cast<std::size_t>(v[static_cast<std::size_t>(i)])].testo);
            break;
        }
        case Passo::unita:
        case Passo::bersaglio: {   // la lista scorre col cursore: si vede la pagina che lo contiene
            const auto v = passo_ == Passo::unita ? unita_adatte() : bersagli();
            for (int i = inizio; i < static_cast<int>(v.size()) && i < inizio + righe; ++i) {
                const UnitaPannello& u = unita_[static_cast<std::size_t>(v[static_cast<std::size_t>(i)])];
                voce(i, u.nome, u.parte);
            }
            break;
        }
        case Passo::punto:
            if (istruzione_punto_) {
                f.scrivi(t, a.x + 6, y, "CLIC SULLA CARTA: IL PUNTO", colori::GIALLO, 1);
                y += h;
            }
            if (tipo_scelto().punto_facoltativo) f.scrivi(t, a.x + 6, y, "INVIO: SENZA PUNTO", fosforo(6), 1);
            break;
        case Passo::parametri: {
            const int n = static_cast<int>(valori_.size());
            for (int i = inizio; i <= n && i < inizio + righe; ++i)
                if (i == n) voce(i, "APPLICA");
                else voce(i, valori_[static_cast<std::size_t>(i)].testo, valore_testo(valori_[static_cast<std::size_t>(i)]));
            break;
        }
    }
    const char* aiuto = passo_ == Passo::parametri ? "TAB VOCE  FRECCE VALORE  INVIO  ESC" : "TAB VOCE  INVIO SCEGLI  ESC INDIETRO";
    f.scrivi(t, a.x + 6, r.y + r.h - 13, taglia(f, aiuto, a.w - 12), fosforo(4), 1);   // fra il filo e il fondo
}

}  // namespace m688
