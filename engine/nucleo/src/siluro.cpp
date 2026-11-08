#include "nucleo/siluro.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#define TOML_EXCEPTIONS 1
#include <toml.hpp>

namespace m688 {

namespace {

constexpr double LIVELLO_RIFERIMENTO_DB = 130.0;    // sorgente su cui vale acquisizione_passiva_m
constexpr double TS_RIFERIMENTO_DB = 15.0;          // sottomarino al traverso medio, su cui vale acquisizione_attiva_m
constexpr double RUMORE_PROPRIO_ALTA = 0.6;         // la testa passiva sente meno in alta velocita' (STIMA)
constexpr double RATEO_QUOTA_M_S = 4.0;
constexpr double ARMAMENTO_M = 500.0;               // percorso minimo prima che la spoletta si armi (STIMA)
constexpr double SCIA_PORTATA_M = 1500.0;           // segui-scia: scia di una nave in moto (STIMA)
constexpr double SCIA_VELOCITA_MIN_M_S = 2.0;

}  // namespace

const DatiSiluro* TabellaSiluri::siluro(const std::string& classe) const {
    const auto it = siluri.find(classe);
    return it == siluri.end() ? nullptr : &it->second;
}

const DatiEsca* TabellaSiluri::esca(const std::string& classe) const {
    const auto it = esche.find(classe);
    return it == esche.end() ? nullptr : &it->second;
}

TabellaSiluri carica_siluri(const std::string& percorso) {
    toml::table t;
    try {
        t = toml::parse_file(percorso);
    } catch (const toml::parse_error& e) {
        throw std::runtime_error(percorso + ": " + std::string(e.description()));
    }
    TabellaSiluri out;
    const auto numero = [&](const toml::table& tab, const std::string& classe, const char* campo) {
        const auto v = tab[campo].value<double>();
        if (!v) throw std::runtime_error(percorso + ": classe '" + classe + "' senza " + campo);
        return *v;
    };
    const auto vero = [&](const toml::table& tab, const std::string& classe, const char* campo) {
        const auto v = tab[campo].value<bool>();
        if (!v) throw std::runtime_error(percorso + ": classe '" + classe + "' senza " + campo);
        return *v;
    };
    if (const auto* siluri = t["siluri"].as_table())
        for (auto&& [chiave, nodo] : *siluri) {
            const auto* tab = nodo.as_table();
            if (!tab) continue;
            DatiSiluro d;
            d.classe = std::string(chiave.str());
            d.nome = (*tab)["nome"].value_or(d.classe);
            d.velocita_alta_kn = numero(*tab, d.classe, "velocita_alta_kn");
            d.velocita_bassa_kn = numero(*tab, d.classe, "velocita_bassa_kn");
            d.portata_alta_m = numero(*tab, d.classe, "portata_alta_m");
            d.portata_bassa_m = numero(*tab, d.classe, "portata_bassa_m");
            d.quota_max_m = numero(*tab, d.classe, "quota_max_m");
            d.virata_gradi_s = numero(*tab, d.classe, "virata_gradi_s");
            d.testata_kg = numero(*tab, d.classe, "testata_kg");
            d.spoletta_m = numero(*tab, d.classe, "spoletta_m");
            d.filoguidato = vero(*tab, d.classe, "filoguidato");
            d.filo_siluro_m = numero(*tab, d.classe, "filo_siluro_m");
            d.filo_battello_m = numero(*tab, d.classe, "filo_battello_m");
            d.attivo = vero(*tab, d.classe, "attivo");
            d.passivo = vero(*tab, d.classe, "passivo");
            d.segui_scia = vero(*tab, d.classe, "segui_scia");
            d.acquisizione_attiva_m = numero(*tab, d.classe, "acquisizione_attiva_m");
            d.acquisizione_passiva_m = numero(*tab, d.classe, "acquisizione_passiva_m");
            d.semiapertura_gradi = numero(*tab, d.classe, "semiapertura_gradi");
            d.affidabilita = (*tab)["affidabilita"].value_or(1.0);
            d.fondale_minimo_m = (*tab)["fondale_minimo_m"].value_or(0.0);
            d.spoletta_scafo_m = (*tab)["spoletta_scafo_m"].value_or(-1.0);
            d.fonte = (*tab)["fonte"].value_or(std::string{});
            d.in_servizio_dal = (*tab)["in_servizio_dal"].value_or(0);
            out.siluri.emplace(d.classe, std::move(d));
        }
    if (const auto* esche = t["esche"].as_table())
        for (auto&& [chiave, nodo] : *esche) {
            const auto* tab = nodo.as_table();
            if (!tab) continue;
            DatiEsca e;
            e.classe = std::string(chiave.str());
            e.livello_db = numero(*tab, e.classe, "livello_db");
            e.durata_s = numero(*tab, e.classe, "durata_s");
            e.quota_m = numero(*tab, e.classe, "quota_m");
            e.velocita_kn = (*tab)["velocita_kn"].value_or(0.0);
            e.quota_max_m = (*tab)["quota_max_m"].value_or(0.0);
            e.eco_db = (*tab)["eco_db"].value_or(e.eco_db);
            e.imita_rumore = (*tab)["imita_rumore"].value_or(false);
            e.virata_ogni_s = (*tab)["virata_ogni_s"].value_or(0.0);
            e.virata_gradi = (*tab)["virata_gradi"].value_or(0.0);
            e.in_servizio_dal = (*tab)["in_servizio_dal"].value_or(0);
            out.esche.emplace(e.classe, std::move(e));
        }
    if (out.siluri.empty()) throw std::runtime_error(percorso + ": nessun siluro");
    return out;
}

double distanza_dallo_scafo(Vec2 p, double quota, Vec2 centro, double quota_scafo, double rotta_deg, double lunghezza_m,
                            double larghezza_m, bool nave, double pescaggio_m) {
    const double r = rotta_deg * PI / 180.0;
    const double ax = std::sin(r), ay = std::cos(r);   // l'asse della rotta (x est, y nord)
    const double dx = p.x - centro.x, dy = p.y - centro.y;
    const double mezza = std::max(lunghezza_m, 0.0) / 2.0;
    const double t = std::clamp(dx * ax + dy * ay, -mezza, mezza);
    const double laterale = std::hypot(dx - t * ax, dy - t * ay);
    const double raggio = (larghezza_m > 0.0 ? larghezza_m : 10.0) / 2.0;
    if (nave) {   // carena dalla superficie al pescaggio
        const double fondo = pescaggio_m > 0.0 ? pescaggio_m : 10.0;
        return std::hypot(std::max(0.0, laterale - raggio), std::max(0.0, quota - fondo));
    }
    return std::max(0.0, std::hypot(laterale, quota - quota_scafo) - raggio);
}

double rotta_esca_mobile(const DatiEsca& d, double rotta0, double t) {
    if (d.virata_ogni_s <= 0.0 || d.virata_gradi == 0.0 || t < d.virata_ogni_s) return normalizza_360(rotta0);
    const auto k = static_cast<long long>(std::floor(t / d.virata_ogni_s));   // 1, 2, 3...: a dritta, a sinistra...
    return normalizza_360(rotta0 + (k % 2 ? 0.5 : -0.5) * d.virata_gradi);
}

const DatiEsca& dati_adc_mk2() {
    static const DatiEsca d{};
    return d;
}

const char* nome_fase(FaseSiluro f) {
    switch (f) {
        case FaseSiluro::corsa: return "corsa";
        case FaseSiluro::ricerca: return "ricerca";
        case FaseSiluro::verifica: return "verifica";
        case FaseSiluro::inseguimento: return "inseguimento";
        case FaseSiluro::riacquisizione: return "riacquisizione";
        case FaseSiluro::spento: return "spento";
        case FaseSiluro::esaurito: return "esaurito";
        case FaseSiluro::esploso: return "esploso";
    }
    return "?";
}

std::optional<CampoSiluro> campo_siluro(const std::string& nome) {
    static const std::pair<const char*, CampoSiluro> nomi[] = {
        {"rotta", CampoSiluro::rotta}, {"modo", CampoSiluro::modo}, {"velocita", CampoSiluro::velocita},
        {"rte", CampoSiluro::rte}, {"quota_corsa", CampoSiluro::quota_corsa}, {"quota_ricerca", CampoSiluro::quota_ricerca},
        {"soffitto", CampoSiluro::soffitto}, {"pavimento", CampoSiluro::pavimento}, {"schema", CampoSiluro::schema},
        {"anti_circolare", CampoSiluro::anti_circolare}};
    for (const auto& [n, c] : nomi)
        if (nome == n) return c;
    return std::nullopt;
}

bool applica(ImpostazioniSiluro& imp, CampoSiluro campo, double valore) {
    const int i = static_cast<int>(std::lround(valore));
    switch (campo) {
        case CampoSiluro::rotta: imp.rotta = normalizza_360(valore); return true;
        case CampoSiluro::modo: if (i < 0 || i > 2) return false; imp.modo = static_cast<ModoTesta>(i); return true;
        case CampoSiluro::velocita: if (i < 0 || i > 1) return false; imp.velocita = static_cast<VelocitaSiluro>(i); return true;
        case CampoSiluro::rte: if (valore < 0.0) return false; imp.rte_m = valore; return true;
        case CampoSiluro::quota_corsa: if (valore < 0.0) return false; imp.quota_corsa_m = valore; return true;
        case CampoSiluro::quota_ricerca: if (valore < 0.0) return false; imp.quota_ricerca_m = valore; return true;
        case CampoSiluro::soffitto: if (valore < 0.0 || valore > imp.pavimento_m) return false; imp.soffitto_m = valore; return true;
        case CampoSiluro::pavimento: if (valore < imp.soffitto_m) return false; imp.pavimento_m = valore; return true;
        case CampoSiluro::schema: if (i < 0 || i > 2) return false; imp.schema = static_cast<SchemaRicerca>(i); return true;
        case CampoSiluro::anti_circolare: imp.anti_circolare = valore != 0.0; return true;
    }
    return false;
}

// ---------------------------------------------------------------------------------------------------------------

double Siluro::filo_siluro_frazione() const {
    return dati_.filo_siluro_m > 0.0 ? filo_siluro_ / dati_.filo_siluro_m : 0.0;
}
double Siluro::filo_battello_frazione() const {
    return dati_.filo_battello_m > 0.0 ? filo_battello_ / dati_.filo_battello_m : 0.0;
}
bool Siluro::filo_a_rischio() const {
    return filo_ && (filo_siluro_frazione() >= FILO_QUASI || filo_battello_frazione() >= FILO_QUASI || fuori_limite_s_ > 0.0);
}

Siluro::Siluro(const DatiSiluro& dati, const ImpostazioniSiluro& imp, Vec2 pos, double quota, int lanciatore,
               std::uint64_t seme)
    : dati_(dati), imp_(imp), casuale_(seme), pos_(pos), quota_(quota),
      passo_velocita_(imp.velocita), modo_(imp.modo), lanciatore_(lanciatore) {
    if (!dati_.attivo && modo_ != ModoTesta::passivo) modo_ = ModoTesta::passivo;
    if (!dati_.passivo && modo_ != ModoTesta::attivo) modo_ = ModoTesta::attivo;
    imp_.pavimento_m = std::min(imp_.pavimento_m, dati_.quota_max_m);
    imp_.quota_ricerca_m = std::clamp(imp_.quota_ricerca_m, 0.0, imp_.pavimento_m);
    imp_.quota_corsa_m = std::clamp(imp_.quota_corsa_m, 0.0, imp_.pavimento_m);
    rotta_ = rotta_ordinata_ = normalizza_360(imp_.rotta);
    rotta_ricerca_ = rotta_;
    quota_ordinata_ = imp_.quota_corsa_m;
    velocita_ = velocita_di(passo_velocita_);
    filo_ = dati_.filoguidato;
    if (dati_.affidabilita < 1.0) guasto_ = casuale_.uniforme() >= dati_.affidabilita;   // solo chi ne ha una: gli altri invariati
}

double Siluro::velocita_di(VelocitaSiluro v) const {
    return (v == VelocitaSiluro::alta ? dati_.velocita_alta_kn : dati_.velocita_bassa_kn) * NODO;
}

double Siluro::autonomia_m() const {
    const double portata = passo_velocita_ == VelocitaSiluro::alta ? dati_.portata_alta_m : dati_.portata_bassa_m;
    return std::max(0.0, energia_) * portata;
}

void Siluro::evento(EventoSiluro::Tipo t, int sorgente) { eventi_.push_back({t, tempo_, sorgente, pos_, quota_}); }

std::vector<EventoSiluro> Siluro::eventi() {
    std::vector<EventoSiluro> e;
    e.swap(eventi_);
    return e;
}

void Siluro::cambia_fase(FaseSiluro f) {
    if (f == fase_) return;
    fase_ = f;
    tempo_fase_ = 0.0;
}

bool Siluro::attraversa_strato(double q1, double q2, const std::vector<double>& strati) const {
    for (const double s : strati)
        if (s > 0.0 && ((q1 < s) != (q2 < s))) return true;
    return false;
}

bool Siluro::sul_fondo(const SorgenteAcustica& s) const {
    return s.velocita < DOPPLER_ESCA_M_S && s.quota > fondale_m_ - VICINO_FONDO_M;
}

double Siluro::portata_su(const SorgenteAcustica& s, const std::vector<double>& strati) const {
    if (guasto_) return 0.0;
    if (dati_.segui_scia) {   // solo la scia di una nave in moto (dossier 01 §6.1: immune alle esche acustiche)
        return s.nave && !s.esca && s.velocita >= SCIA_VELOCITA_MIN_M_S ? SCIA_PORTATA_M : 0.0;
    }
    const bool attivo_ora = modo_ == ModoTesta::attivo ||
                            (modo_ == ModoTesta::misto && (fase_ == FaseSiluro::verifica || fase_ == FaseSiluro::inseguimento));
    double r;
    if (attivo_ora) {
        // eco: perdita su andata e ritorno, 40 log r
        r = dati_.acquisizione_attiva_m * std::pow(10.0, (s.forza_bersaglio_db - TS_RIFERIMENTO_DB) / 40.0);
    } else {
        r = dati_.acquisizione_passiva_m * std::pow(10.0, (s.livello_db - LIVELLO_RIFERIMENTO_DB) / 20.0);
        if (passo_velocita_ == VelocitaSiluro::alta) r *= RUMORE_PROPRIO_ALTA;
    }
    if (attraversa_strato(quota_, s.quota, strati)) r *= STRATO_FATTORE;
    if (attivo_ora && sul_fondo(s)) r *= FONDO_FATTORE;
    return r;
}

bool Siluro::vede(const SorgenteAcustica& s, const std::vector<double>& strati, double& distanza, double& rilev) const {
    if (std::find(rifiutate_.begin(), rifiutate_.end(), s.id) != rifiutate_.end()) return false;
    if (s.lanciatore && imp_.anti_circolare) return false;
    if (!dati_.segui_scia && (s.quota < imp_.soffitto_m || s.quota > imp_.pavimento_m)) return false;
    const double dx = s.pos.x - pos_.x, dy = s.pos.y - pos_.y, dz = s.quota - quota_;
    distanza = std::sqrt(dx * dx + dy * dy + dz * dz);
    rilev = rilevamento(pos_, s.pos);
    if (std::fabs(normalizza_180(rilev - rotta_)) > dati_.semiapertura_gradi && distanza > 50.0) return false;
    return distanza <= portata_su(s, strati);
}

void Siluro::ascolta(const std::vector<SorgenteAcustica>& sorgenti, const std::vector<double>& strati, double dt) {
    testa_.clear();
    ping_ = false;
    if (fase_ == FaseSiluro::corsa) return;
    const bool attivo_ora = !dati_.segui_scia &&
                            (modo_ == ModoTesta::attivo ||
                             (modo_ == ModoTesta::misto && (fase_ == FaseSiluro::verifica || fase_ == FaseSiluro::inseguimento)));
    ping_ = attivo_ora;
    // la sorgente piu' "forte" alla testa: livello ricevuto (passivo) o eco (attivo)
    const SorgenteAcustica* migliore = nullptr;
    double forza_migliore = -1e9;
    bool bersaglio_visto = false;
    for (const auto& s : sorgenti) {
        double d = 0.0, r = 0.0;
        if (!vede(s, strati, d, r)) continue;
        RilevamentoTesta t{s.id, r, attivo_ora ? d : -1.0, s.velocita < DOPPLER_ESCA_M_S};
        testa_.push_back(t);
        if (s.id == bersaglio_) bersaglio_visto = true;
        const double logd = 20.0 * std::log10(std::max(d, 1.0));
        const double forza = attivo_ora ? s.forza_bersaglio_db - 2.0 * logd : s.livello_db - logd;
        if (forza > forza_migliore) { forza_migliore = forza; migliore = &s; }
    }
    // un noisemaker vicino nel fascio della testa attiva: il rumore copre le eco vere (anche se l'esca e' gia' rifiutata)
    bool disturbata = false;
    if (attivo_ora)
        for (const auto& s : sorgenti)
            if (s.esca && s.lanciatore == false) {
                const double dx = s.pos.x - pos_.x, dy = s.pos.y - pos_.y;
                if (std::hypot(dx, dy) <= ACCECAMENTO_M &&
                    std::fabs(normalizza_180(rilevamento(pos_, s.pos) - rotta_)) <= dati_.semiapertura_gradi) { disturbata = true; break; }
            }
    const bool riavvio = disturbata && casuale_.uniforme() < P_RIAVVIO_S * dt;
    switch (fase_) {
        case FaseSiluro::ricerca:
        case FaseSiluro::riacquisizione:
            if (migliore) { candidato_ = migliore->id; cambia_fase(FaseSiluro::verifica); }
            break;
        case FaseSiluro::verifica: {
            const SorgenteAcustica* c = nullptr;
            for (const auto& s : sorgenti)
                if (s.id == candidato_) c = &s;
            double d = 0.0, r = 0.0;
            if (!c || !vede(*c, strati, d, r)) { candidato_ = -1; cambia_fase(FaseSiluro::ricerca); break; }
            if (riavvio && !c->esca) { tempo_fase_ = 0.0; break; }   // il disturbo riavvia la verifica
            if (tempo_fase_ < VERIFICA_S) break;
            // contro-contromisure (dossier 05 §5.3, 5.6): Doppler e dimensione dell'eco per l'attivo; il passivo e'
            // sedotto facilmente da una sorgente piu' rumorosa; il segui-scia non vede le esche
            if (attivo_ora && !c->esca && sul_fondo(*c) && casuale_.uniforme() < FONDO_RIFIUTO) {   // e' il fondo
                rifiutate_.push_back(c->id);
                candidato_ = -1;
                cambia_fase(FaseSiluro::ricerca);
                break;
            }
            double p_rifiuto = 0.1;
            if (attivo_ora) {
                p_rifiuto = 0.0;
                if (c->velocita < DOPPLER_ESCA_M_S) p_rifiuto += 0.5;
                if (c->lunghezza_m < 20.0) p_rifiuto += 0.2;
            } else if (!c->esca) {
                p_rifiuto = 0.0;
            }
            if (c->esca && casuale_.uniforme() < p_rifiuto) {
                rifiutate_.push_back(c->id);
                evento(EventoSiluro::Tipo::esca_rifiutata, c->id);
                candidato_ = -1;
                cambia_fase(FaseSiluro::ricerca);
            } else {
                bersaglio_ = c->id;
                evento(c->esca ? EventoSiluro::Tipo::esca_sedotta : EventoSiluro::Tipo::aggancio, c->id);
                tempo_senza_contatto_ = 0.0;
                passo_velocita_ = VelocitaSiluro::alta;   // aggancio: velocita' d'attacco
                cambia_fase(FaseSiluro::inseguimento);
            }
            break;
        }
        case FaseSiluro::inseguimento:
            if (riavvio) {   // il disturbo copre l'eco: aggancio perso, si riacquisisce
                const bool esca = std::any_of(sorgenti.begin(), sorgenti.end(), [&](const auto& s) { return s.id == bersaglio_ && s.esca; });
                if (!esca) {
                    evento(EventoSiluro::Tipo::perdita, bersaglio_);
                    bersaglio_ = -1;
                    rotta_ricerca_ = rotta_;
                    cambia_fase(FaseSiluro::riacquisizione);
                    break;
                }
            }
            if (bersaglio_visto) tempo_senza_contatto_ = 0.0;
            else if ((tempo_senza_contatto_ += dt) > PERDITA_S) {
                evento(EventoSiluro::Tipo::perdita, bersaglio_);
                bersaglio_ = -1;
                rotta_ricerca_ = rotta_;
                cambia_fase(FaseSiluro::riacquisizione);
            }
            break;
        default: break;
    }
}

void Siluro::controlla_filo(double dt, const std::optional<Lanciatore>& l) {
    if (!filo_) return;
    filo_siluro_ = percorso_;
    if (!l) { filo_ = false; evento(EventoSiluro::Tipo::filo_rotto); return; }   // lanciatore perso
    lanciatore_pos_ = l->pos;
    filo_battello_ += l->velocita * dt;
    const bool fuori = l->velocita > FILO_VELOCITA_MAX_KN * NODO || l->rateo_virata > FILO_VIRATA_MAX_GRADI_S;
    fuori_limite_s_ = fuori ? fuori_limite_s_ + dt : 0.0;
    if (fuori_limite_s_ > FILO_TOLLERANZA_S || filo_siluro_ > dati_.filo_siluro_m || filo_battello_ > dati_.filo_battello_m) {
        filo_ = false;
        evento(EventoSiluro::Tipo::filo_rotto);
    }
}

void Siluro::governa(double dt, const SorgenteAcustica* b) {
    switch (fase_) {
        case FaseSiluro::corsa:
            quota_ordinata_ = imp_.quota_corsa_m;
            if (!rotta_da_filo_) rotta_ordinata_ = normalizza_360(imp_.rotta);
            break;
        case FaseSiluro::ricerca:
        case FaseSiluro::verifica:
            quota_ordinata_ = imp_.quota_ricerca_m;
            if (fase_ == FaseSiluro::verifica && b) {   // durante la verifica punta il candidato
                rotta_ordinata_ = rilevamento(pos_, b->pos);
                quota_ordinata_ = std::clamp(b->quota, imp_.soffitto_m, imp_.pavimento_m);
            } else if (imp_.schema == SchemaRicerca::serpentina) {
                rotta_ordinata_ = normalizza_360(rotta_ricerca_ + SERPENTINA_GRADI * std::sin(2.0 * PI * tempo_fase_ / SERPENTINA_PERIODO_S));
            } else if (imp_.schema == SchemaRicerca::circolare) {
                rotta_ordinata_ = normalizza_360(rotta_ + dati_.virata_gradi_s * dt * 2.0);
            } else {
                rotta_ordinata_ = rotta_ricerca_;
            }
            break;
        case FaseSiluro::inseguimento:
            if (b) {   // inseguimento con anticipo: punto futuro del bersaglio al tempo di arrivo (al piu' 30 s)
                const double d = distanza(pos_, b->pos), tgo = std::min(30.0, d / std::max(velocita_, 1.0));
                const Vec2 futuro{b->pos.x + b->velocita * std::sin(radianti(b->rotta)) * tgo,
                                  b->pos.y + b->velocita * std::cos(radianti(b->rotta)) * tgo};
                rotta_ordinata_ = rilevamento(pos_, futuro);
                quota_ordinata_ = std::clamp(b->quota, dati_.segui_scia ? 0.0 : imp_.soffitto_m, imp_.pavimento_m);
            }
            break;
        case FaseSiluro::riacquisizione:   // cerchio (riattacco) attorno al punto della perdita
            quota_ordinata_ = imp_.quota_ricerca_m;
            rotta_ordinata_ = normalizza_360(rotta_ + dati_.virata_gradi_s * dt * 2.0);
            break;
        default: break;
    }
}

void Siluro::avanza(double dt, const std::vector<SorgenteAcustica>& sorgenti, const std::vector<double>& strati,
                    const std::optional<Lanciatore>& lanciatore) {
    if (!attivo()) {   // spento o esaurito: fermo, affonda
        velocita_ = 0.0;
        ping_ = false;
        testa_.clear();
        if (fase_ != FaseSiluro::esploso) quota_ += AFFONDAMENTO_M_S * dt;
        return;
    }
    tempo_ += dt;
    tempo_fase_ += dt;
    controlla_filo(dt, lanciatore);
    ascolta(sorgenti, strati, dt);
    const SorgenteAcustica* b = nullptr;
    const int cercato = fase_ == FaseSiluro::verifica ? candidato_ : bersaglio_;
    for (const auto& s : sorgenti)
        if (s.id == cercato) b = &s;
    governa(dt, b);

    // manovra: virata e quota limitate, velocita' del passo scelto
    const double e = normalizza_180(rotta_ordinata_ - rotta_), m = dati_.virata_gradi_s * dt;
    rotta_ = normalizza_360(rotta_ + std::clamp(e, -m, m));
    const double qmax = std::min(imp_.pavimento_m, dati_.quota_max_m);
    const double qo = std::clamp(quota_ordinata_, 0.0, qmax);
    quota_ += std::clamp(qo - quota_, -RATEO_QUOTA_M_S * dt, RATEO_QUOTA_M_S * dt);
    const double portata = passo_velocita_ == VelocitaSiluro::alta ? dati_.portata_alta_m : dati_.portata_bassa_m;
    velocita_ = velocita_di(passo_velocita_);
    const double passo = velocita_ * dt;
    pos_ = {pos_.x + passo * std::sin(radianti(rotta_)), pos_.y + passo * std::cos(radianti(rotta_))};
    percorso_ += passo;
    energia_ -= passo / std::max(portata, 1.0);

    // attivazione della testa dopo la corsa
    if (fase_ == FaseSiluro::corsa && percorso_ >= imp_.rte_m) {
        rotta_ricerca_ = rotta_;
        cambia_fase(FaseSiluro::ricerca);
        evento(EventoSiluro::Tipo::attivazione);
    }
    // anti-circolare: si spegne se, dopo l'attivazione, punta al lanciatore entro ACR_DISTANZA_M
    std::optional<Vec2> pl = lanciatore ? std::optional<Vec2>(lanciatore->pos) : lanciatore_pos_;
    for (const auto& s : sorgenti)
        if (s.lanciatore) pl = s.pos;
    if (imp_.anti_circolare && pl && fase_ != FaseSiluro::corsa && distanza(pos_, *pl) < ACR_DISTANZA_M &&
        std::fabs(normalizza_180(rilevamento(pos_, *pl) - rotta_)) < ACR_SEMIAPERTURA_GRADI) {
        cambia_fase(FaseSiluro::spento);
        evento(EventoSiluro::Tipo::spento);
        return;
    }
    // spoletta: prossimita' a un'unita' vera (non alle esche) dopo l'armamento
    if (percorso_ >= std::min(imp_.rte_m, ARMAMENTO_M)) {
        for (const auto& s : sorgenti) {
            if (s.esca || (s.lanciatore && imp_.anti_circolare)) continue;
            // dallo scafo se lo si conosce (cilindro della classe), se no dal centro con un margine per lo scafo
            const double dx = s.pos.x - pos_.x, dy = s.pos.y - pos_.y, dz = s.quota - quota_;
            const double d = s.larghezza_m > 0.0
                                 ? distanza_dallo_scafo(pos_, quota_, s.pos, s.quota, s.rotta, s.lunghezza_m, s.larghezza_m, s.nave, s.pescaggio_m)
                                 : std::sqrt(dx * dx + dy * dy + dz * dz) - (s.nave ? 10.0 : 5.0);
            const double raggio = s.larghezza_m > 0.0 && dati_.spoletta_scafo_m >= 0.0 ? dati_.spoletta_scafo_m : dati_.spoletta_m;
            if (d <= raggio) {
                bersaglio_ = s.id;
                cambia_fase(FaseSiluro::esploso);
                evento(EventoSiluro::Tipo::esplosione, s.id);
                return;
            }
        }
    }
    if (energia_ <= 0.0) {
        cambia_fase(FaseSiluro::esaurito);
        evento(EventoSiluro::Tipo::esaurito);
    }
}

void Siluro::comanda(const ComandoFilo& c) {
    if (!filo_ || !attivo()) return;
    switch (c.tipo) {
        case ComandoFiloTipo::rotta:
            rotta_ordinata_ = rotta_ricerca_ = normalizza_360(c.valore);
            imp_.rotta = rotta_ordinata_;
            rotta_da_filo_ = true;
            break;
        case ComandoFiloTipo::quota:
            imp_.quota_corsa_m = imp_.quota_ricerca_m = std::clamp(c.valore, 0.0, imp_.pavimento_m);
            break;
        case ComandoFiloTipo::velocita: passo_velocita_ = c.velocita; break;
        case ComandoFiloTipo::modo:
            if ((c.modo == ModoTesta::passivo && dati_.passivo) || (c.modo != ModoTesta::passivo && dati_.attivo)) modo_ = c.modo;
            break;
        case ComandoFiloTipo::abilita:
            if (fase_ == FaseSiluro::corsa) {
                rotta_ricerca_ = rotta_;
                cambia_fase(FaseSiluro::ricerca);
                evento(EventoSiluro::Tipo::attivazione);
            }
            break;
        case ComandoFiloTipo::rifiuta:
            rifiutate_.push_back(c.sorgente);
            if (bersaglio_ == c.sorgente || candidato_ == c.sorgente) {
                evento(EventoSiluro::Tipo::esca_rifiutata, c.sorgente);
                bersaglio_ = candidato_ = -1;
                rotta_ricerca_ = rotta_;
                cambia_fase(FaseSiluro::ricerca);
            }
            break;
        case ComandoFiloTipo::assegna:
            rifiutate_.erase(std::remove(rifiutate_.begin(), rifiutate_.end(), c.sorgente), rifiutate_.end());
            bersaglio_ = c.sorgente;
            tempo_senza_contatto_ = 0.0;
            passo_velocita_ = VelocitaSiluro::alta;
            cambia_fase(FaseSiluro::inseguimento);
            evento(EventoSiluro::Tipo::aggancio, c.sorgente);
            break;
        case ComandoFiloTipo::spegni:
            cambia_fase(FaseSiluro::spento);
            evento(EventoSiluro::Tipo::spento);
            break;
        case ComandoFiloTipo::taglia:
            filo_ = false;
            filo_tagliato_ = true;
            break;
    }
}

// ---------------------------------------------------------------------------------------------------------------

Esca::Esca(Vec2 pos, double quota, double tempo_lancio, const DatiEsca& dati)
    : dati_(dati), pos_(pos), quota_(quota), inizio_(tempo_lancio) {}

SorgenteAcustica Esca::sorgente(int id) const {
    SorgenteAcustica s;
    s.id = id;
    s.pos = pos_;
    s.quota = quota_;
    s.velocita = 0.0;               // staziona: Doppler zero
    s.livello_db = dati_.livello_db;
    s.forza_bersaglio_db = 10.0;    // ripetitore puntuale (STIMA): eco piu' debole di un sottomarino
    s.lunghezza_m = 1.0;
    s.esca = true;
    return s;
}

}  // namespace m688
