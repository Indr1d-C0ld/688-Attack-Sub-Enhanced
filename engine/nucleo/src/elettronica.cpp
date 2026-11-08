#include "nucleo/elettronica.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <toml.hpp>

namespace m688 {

namespace {

constexpr double PIEDE = 0.3048;
constexpr double QUOTA_ALBERO_M = 18.0;          // antenna ESM fuori dall'acqua (dossier 01)
constexpr double ALLARME_S = 1200.0;             // senza allarmi da 20 minuti: EMCON di partenza
constexpr double PERDITA_S = 60.0;               // emettitore perso dopo un minuto senza passaggi (o tre periodi)

RuoloRadar ruolo_da_nome(const std::string& s) {
    if (s == "superficie") return RuoloRadar::superficie;
    if (s == "ricerca_aerea") return RuoloRadar::ricerca_aerea;
    if (s == "asw_aereo") return RuoloRadar::asw_aereo;
    if (s == "tiro") return RuoloRadar::tiro;
    if (s == "testa_missile") return RuoloRadar::testa_missile;
    return RuoloRadar::navigazione;
}

const char* nome_ruolo(RuoloRadar r) {
    switch (r) {
        case RuoloRadar::navigazione: return "di navigazione";
        case RuoloRadar::superficie: return "di superficie";
        case RuoloRadar::ricerca_aerea: return "di ricerca aerea";
        case RuoloRadar::asw_aereo: return "antisommergibile d'aereo";
        case RuoloRadar::tiro: return "di tiro";
        case RuoloRadar::testa_missile: return "di testa di missile";
    }
    return "";
}

int ordine(StatoEmcon s) {   // silenzio < navigazione < ricerca < ingaggio
    switch (s) {
        case StatoEmcon::silenzio: return 0;
        case StatoEmcon::navigazione: return 1;
        case StatoEmcon::ricerca: return 2;
        case StatoEmcon::intermittente: return 2;
        case StatoEmcon::ingaggio: return 3;
    }
    return 0;
}

}  // namespace

const Radar* CatalogoElettronico::trova_radar(const std::string& id) const {
    const auto it = radar.find(id);
    return it == radar.end() ? nullptr : &it->second;
}

double CatalogoElettronico::altezza_antenna(const std::string& classe, const Radar& r) const {
    const auto it = antenne.find({classe, r.id});
    return it == antenne.end() ? r.altezza_m : it->second;
}

std::vector<const Radar*> CatalogoElettronico::radar_di(const std::string& classe) const {
    std::vector<const Radar*> v;
    if (const auto it = suite.find(classe); it != suite.end())
        for (const auto& id : it->second) if (const Radar* r = trova_radar(id)) v.push_back(r);
    return v;
}

const RicevitoreEsm* CatalogoElettronico::esm_di(const std::string& classe) const {
    const auto it = esm_di_classe.find(classe);
    if (it == esm_di_classe.end()) return nullptr;
    const auto e = esm.find(it->second);
    return e == esm.end() ? nullptr : &e->second;
}

const Intermittenza* CatalogoElettronico::intermittenza_di(const std::string& classe) const {
    const auto it = intermittenza.find(classe);
    return it == intermittenza.end() ? nullptr : &it->second;
}

CatalogoElettronico carica_elettronica(const std::string& percorso) {
    toml::table t;
    try {
        t = toml::parse_file(percorso);
    } catch (const toml::parse_error& e) {
        throw std::runtime_error(percorso + ": " + std::string(e.description()));
    }
    CatalogoElettronico c;
    const auto numero = [&](const toml::table& x, const std::string& chi, const char* campo) {
        const auto v = x[campo].value<double>();
        if (!v) throw std::runtime_error(percorso + ": '" + chi + "' senza " + campo);
        return *v;
    };
    if (const auto* rr = t["radar"].as_table())
        for (auto&& [k, nodo] : *rr) {
            const auto* x = nodo.as_table();
            if (!x) continue;
            Radar r;
            r.id = std::string(k.str());
            r.nome = (*x)["nome"].value_or(r.id);
            r.banda = (*x)["banda"].value_or(std::string{"I"});
            r.gruppo = (*x)["gruppo"].value_or(std::string{});
            r.ruolo = ruolo_da_nome((*x)["ruolo"].value_or(std::string{}));
            r.ghz = numero(*x, r.id, "ghz");
            r.periodo_s = numero(*x, r.id, "periodo_s");
            r.altezza_m = numero(*x, r.id, "altezza_m");
            r.r50_periscopio_mare2_m = numero(*x, r.id, "r50_periscopio_mare2_m");
            r.r50_periscopio_mare4_m = numero(*x, r.id, "r50_periscopio_mare4_m");
            r.r50_snorkel_mare2_m = numero(*x, r.id, "r50_snorkel_mare2_m");
            r.contro_nave_m = numero(*x, r.id, "contro_nave_m");
            r.fattore_esm = numero(*x, r.id, "fattore_esm");
            r.puntato = (*x)["puntato"].value_or(false);
            r.firma_unica = (*x)["firma_unica"].value_or(false);
            r.agilita_frequenza = (*x)["agilita_frequenza"].value_or(false);
            c.radar[r.id] = r;
        }
    if (const auto* ee = t["esm"].as_table())
        for (auto&& [k, nodo] : *ee) {
            const auto* x = nodo.as_table();
            if (!x) continue;
            RicevitoreEsm e;
            e.id = std::string(k.str());
            e.nome = (*x)["nome"].value_or(e.id);
            e.ghz_min = numero(*x, e.id, "ghz_min");
            e.ghz_max = numero(*x, e.id, "ghz_max");
            e.p_intercetto = numero(*x, e.id, "p_intercetto");
            e.p_intercetto_basse = numero(*x, e.id, "p_intercetto_basse");
            e.errore_ij_gradi = numero(*x, e.id, "errore_ij_gradi");
            e.errore_eh_gradi = numero(*x, e.id, "errore_eh_gradi");
            e.errore_bd_gradi = numero(*x, e.id, "errore_bd_gradi");
            e.altezza_m = numero(*x, e.id, "altezza_m");
            c.esm[e.id] = e;
        }
    if (const auto* s = t["suite"].as_table())
        for (auto&& [k, nodo] : *s)
            if (const auto* a = nodo.as_array())
                for (const auto& v : *a) if (const auto id = v.value<std::string>()) c.suite[std::string(k.str())].push_back(*id);
    if (const auto* s = t["esm_di"].as_table())
        for (auto&& [k, nodo] : *s) if (const auto id = nodo.value<std::string>()) c.esm_di_classe[std::string(k.str())] = *id;
    if (const auto* s = t["intermittente"].as_table())
        for (auto&& [k, nodo] : *s)
            if (const auto* a = nodo.as_array(); a && a->size() == 4)
                c.intermittenza[std::string(k.str())] = {(*a)[0].value_or(6.0), (*a)[1].value_or(18.0), (*a)[2].value_or(120.0), (*a)[3].value_or(480.0)};
    // le antenne misurate sui modelli del kit (tools/pipeline/emettitori.py): { parte, quota_m } per classe e radar
    if (const auto* a = t["antenne"].as_table())
        for (auto&& [classe, nodo] : *a)
            if (const auto* radar_classe = nodo.as_table())
                for (auto&& [id, x] : *radar_classe)
                    if (const auto* q = x.as_table())
                        if (const auto h = (*q)["quota_m"].value<double>(); h && *h > 0.0)
                            c.antenne[{std::string(classe.str()), std::string(id.str())}] = *h;
    if (c.radar.empty()) throw std::runtime_error(percorso + ": nessun radar");
    for (const auto& [classe, lista] : c.suite)
        for (const auto& id : lista)
            if (!c.trova_radar(id)) throw std::runtime_error(percorso + ": la classe '" + classe + "' cita il radar '" + id + "' che manca");
    return c;
}

// --- scoperta radar dell'albero ---

double orizzonte_radar_m(double altezza_radar_m, double altezza_bersaglio_m) {
    return 1.23 * (std::sqrt(std::max(altezza_radar_m, 0.0) / PIEDE) + std::sqrt(std::max(altezza_bersaglio_m, 0.0) / PIEDE)) * MIGLIO;
}

double altezza_esposta_m(Esposizione e) {
    switch (e) {
        case Esposizione::periscopio: return 1.5;
        case Esposizione::albero: return 2.0;
        case Esposizione::snorkel: return 1.5;
        case Esposizione::emerso: return 6.0;
    }
    return 1.5;
}

double r50_m(const Radar& r, Esposizione e, int stato_mare) {
    const double ss = std::clamp(static_cast<double>(stato_mare), 0.0, 9.0);
    // dossier 21 §6.1: col mare calmo meglio (1,3 e 1,15 di mare 2); fra 2 e 4 i dati del radar; oltre, il radar per
    // periscopi (APS-116/137: a mare 4 tiene almeno 0,55 di mare 2) scende a 0,4, 0,2, 0,1; gli altri a zero a mare 5
    const double r2 = r.r50_periscopio_mare2_m, r4 = r.r50_periscopio_mare4_m;
    const bool per_periscopi = r2 > 0.0 && r4 >= 0.55 * r2;
    double peri = 0.0;
    if (ss < 2.0) peri = r2 * (ss < 1.0 ? 1.3 - 0.15 * ss : 1.15 - 0.15 * (ss - 1.0));
    else if (ss <= 4.0) peri = r2 + (r4 - r2) * (ss - 2.0) / 2.0;
    else if (per_periscopi) {
        static constexpr double OLTRE[] = {0.6, 0.4, 0.2, 0.1};   // mare 4 (dai dati), 5, 6, 7+
        const int i = std::min(static_cast<int>(ss) - 4, 3);
        peri = i == 0 ? r4 : r2 * OLTRE[i] * (r4 / (0.6 * r2) < 1.0 ? r4 / (0.6 * r2) : 1.0);
    } else peri = ss < 5.0 ? r4 * (5.0 - ss) : 0.0;
    switch (e) {
        case Esposizione::periscopio: return peri;
        case Esposizione::albero: return peri * std::pow(2.0, 0.25);   // circa 1 m2 contro i 0,5 del periscopio
        case Esposizione::snorkel: {
            const double s2 = r.r50_snorkel_mare2_m;
            return ss >= 5.0 ? 0.3 * s2 : s2 + (0.3 * s2 - s2) * (ss - 2.0) / 3.0;
        }
        case Esposizione::emerso: return r.contro_nave_m > 0.0 ? 0.6 * r.contro_nave_m : std::numeric_limits<double>::infinity();
    }
    return 0.0;
}

namespace {
// La pioggia sul radar (dossier 21 §6.2): attenuazione in banda X, andata e ritorno (ITU, ~0,0101 R^1,276 dB/km a
// 10 GHz, trascurabile sotto i 4 GHz), e l'eco della pioggia che nasconde le teste piccole (STIMA)
double fattore_pioggia(const Radar& r, double distanza_m, double pioggia_mm_h, bool testa) {
    if (pioggia_mm_h <= 0.0) return 1.0;
    const double a = r.ghz >= 6.0 ? 0.0101 * std::pow(pioggia_mm_h, 1.276) : r.ghz >= 4.0 ? 0.002 * pioggia_mm_h : 0.0;
    const double perdita_db = 2.0 * a * distanza_m / 1000.0;
    return std::pow(10.0, -perdita_db / 40.0) * (testa ? 1.0 / (1.0 + pioggia_mm_h / 20.0) : 1.0);
}
}  // namespace

double probabilita_giro(const Radar& r, double distanza_m, double altezza_radar_m, Esposizione e, int stato_mare,
                        double f_op, double velocita_kn, double pioggia_mm_h, double altezza_vela_m) {
    const double r50 = r50_m(r, e, stato_mare) * fattore_pioggia(r, distanza_m, pioggia_mm_h, e != Esposizione::emerso);
    const double esposta = e == Esposizione::emerso && altezza_vela_m > 0.0 ? altezza_vela_m : altezza_esposta_m(e);
    if (!(r50 > 0.0) || distanza_m > orizzonte_radar_m(altezza_radar_m, esposta)) return 0.0;
    const double f_vel = velocita_kn <= 5.0 ? 1.0 : velocita_kn <= 8.0 ? 1.5 : 2.0;   // pennacchio
    const double q = std::isinf(r50) ? 0.0 : std::pow(distanza_m / r50, 4.0);
    return std::min(0.95, f_op * f_vel / (1.0 + q));
}

double probabilita_giro(const Radar& r, double distanza_m, double altezza_radar_m, const TestaEsposta& testa,
                        int stato_mare, double f_op, double velocita_kn, double pioggia_mm_h) {
    if (!(testa.sezione_m2 > 0.0) || !(testa.altezza_m > 0.0)) return 0.0;
    const double r50 = r50_m(r, Esposizione::periscopio, stato_mare) * std::pow(testa.sezione_m2 / 0.5, 0.25) *
                       fattore_pioggia(r, distanza_m, pioggia_mm_h, true);
    if (!(r50 > 0.0) || distanza_m > orizzonte_radar_m(altezza_radar_m, testa.altezza_m)) return 0.0;
    const double f_vel = 1.0 + ((velocita_kn <= 5.0 ? 1.0 : velocita_kn <= 8.0 ? 1.5 : 2.0) - 1.0) * testa.piuma;
    return std::min(0.95, f_op * f_vel / (1.0 + std::pow(distanza_m / r50, 4.0)));
}

// --- EMCON ---

const char* nome_emcon(StatoEmcon s) {
    switch (s) {
        case StatoEmcon::silenzio: return "silenzio";
        case StatoEmcon::navigazione: return "navigazione";
        case StatoEmcon::ricerca: return "ricerca";
        case StatoEmcon::intermittente: return "intermittente";
        case StatoEmcon::ingaggio: return "ingaggio";
    }
    return "";
}

StatoEmcon emcon_di_partenza(const std::string& categoria, bool sovietico, bool aereo, bool mercantile) {
    if (aereo) return StatoEmcon::intermittente;
    if (mercantile) return StatoEmcon::navigazione;
    if (categoria != "nave") return StatoEmcon::silenzio;   // sottomarini, armi
    return sovietico ? StatoEmcon::ricerca : StatoEmcon::navigazione;
}

Emcon::Emcon(StatoEmcon partenza, std::optional<Intermittenza> ciclo, std::uint64_t seme)
    : partenza_(partenza), stato_(partenza), ciclo_(ciclo), caso_(seme ^ 0xE3C0ull) {}

void Emcon::evento(EventoEmcon e, double t) {
    ultimo_allarme_ = t;
    if (partenza_ == StatoEmcon::intermittente) {   // aerei: radar continuo finche' tengono l'albero
        if (e == EventoEmcon::albero_visto) continuo_ = true;
        return;
    }
    if (partenza_ == StatoEmcon::silenzio && e != EventoEmcon::lancio_missili) return;   // sottomarini: restano zitti
    StatoEmcon nuovo = stato_;
    switch (e) {
        case EventoEmcon::scoperti: nuovo = StatoEmcon::ricerca; break;
        case EventoEmcon::contatto_sonar: break;   // USA: resta; i sovietici sono gia' in ricerca
        case EventoEmcon::minaccia_aerea:
        case EventoEmcon::albero_visto:
        case EventoEmcon::lancio_missili: nuovo = StatoEmcon::ingaggio; break;
    }
    if (ordine(nuovo) > ordine(stato_)) stato_ = nuovo;
}

void Emcon::avanza(double t) {
    if (t - ultimo_allarme_ > ALLARME_S && (stato_ != partenza_ || continuo_)) {
        stato_ = partenza_;
        continuo_ = false;
    }
    if (stato_ != StatoEmcon::intermittente || !ciclo_) return;
    const Intermittenza& c = *ciclo_;
    if (prossimo_cambio_ < 0.0) prossimo_cambio_ = t + c.off_min + (c.off_max - c.off_min) * caso_.uniforme();
    while (t >= prossimo_cambio_) {
        acceso_ = !acceso_;
        prossimo_cambio_ += acceso_ ? c.on_min + (c.on_max - c.on_min) * caso_.uniforme()
                                    : c.off_min + (c.off_max - c.off_min) * caso_.uniforme();
    }
}

bool Emcon::emette(const Radar& r) const {
    switch (stato_) {
        case StatoEmcon::silenzio: return false;
        case StatoEmcon::navigazione: return r.ruolo == RuoloRadar::navigazione || r.ruolo == RuoloRadar::superficie;
        case StatoEmcon::ricerca:
            return r.ruolo == RuoloRadar::navigazione || r.ruolo == RuoloRadar::superficie || r.ruolo == RuoloRadar::ricerca_aerea;
        case StatoEmcon::intermittente:
            return (continuo_ || acceso_ || !ciclo_) && r.ruolo != RuoloRadar::tiro && r.ruolo != RuoloRadar::testa_missile;
        case StatoEmcon::ingaggio: return true;
    }
    return false;
}

// --- ESM ---

const char* nome_rischio(Rischio r) {
    switch (r) {
        case Rischio::sicuro: return "SICURO";
        case Rischio::attenzione: return "ATTENZIONE";
        case Rischio::probabile: return "CONTRO-SCOPERTA PROBABILE";
        case Rischio::scoperto: return "SCOPERTO";
    }
    return "";
}

EsmBattello::EsmBattello(const CatalogoElettronico& c, const RicevitoreEsm& r, std::uint64_t seme)
    : catalogo_(&c), ricevitore_(r), caso_(seme ^ 0xE5Aull) {}

double EsmBattello::gauss() {
    const double u1 = std::max(caso_.uniforme(), 1e-12), u2 = caso_.uniforme();
    return std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * PI * u2);
}

void EsmBattello::avanza(double t, double dt, Vec2 pos, double quota_m, bool albero, Esposizione esposizione,
                         int stato_mare, const std::vector<EmissioneVista>& emissioni) {
    if (albero && quota_m <= QUOTA_ALBERO_M) {
        for (const auto& e : emissioni) {
            if (!e.radar) continue;
            const Radar& r = *e.radar;
            if (r.ghz < ricevitore_.ghz_min || r.ghz > ricevitore_.ghz_max) continue;
            const double d = std::max(distanza(pos, e.pos), 1.0);
            double portata = r.fattore_esm * orizzonte_radar_m(e.altezza_m, ricevitore_.altezza_m);
            double p = std::min(1.0, dt / std::max(r.periodo_s, 0.05)) * (r.ghz < 2.0 ? ricevitore_.p_intercetto_basse : ricevitore_.p_intercetto);
            if (r.puntato && !e.puntato_su_di_noi) { portata /= 3.0; p *= 0.3; }   // lobi laterali
            if (d > portata || caso_.uniforme() >= p) continue;
            // un passaggio del fascio
            auto& tr = tracce_[{e.unita, r.id}];
            Intercettazione& i = tr.i;
            if (i.passaggi == 0) {
                i.unita = e.unita;
                i.radar = r.id;
                i.banda = r.banda;
                i.aereo = e.aereo;
                i.asw_aereo = r.ruolo == RuoloRadar::asw_aereo;
                i.prima = t;
                eventi_.push_back({EventoEsm::Tipo::nuovo, t, e.unita, r.id});
                if (i.asw_aereo) eventi_.push_back({EventoEsm::Tipo::aereo_asw, t, e.unita, r.id});
            }
            ++i.passaggi;
            i.ultima = t;
            const double sigma = r.ghz >= 8.0 ? ricevitore_.errore_ij_gradi : r.ghz >= 2.0 ? ricevitore_.errore_eh_gradi : ricevitore_.errore_bd_gradi;
            const double misura = (rilevamento(pos, e.pos) + sigma * gauss()) * PI / 180.0;
            tr.somma_sin += std::sin(misura);
            tr.somma_cos += std::cos(misura);
            i.rilevamento = normalizza_360(std::atan2(tr.somma_sin, tr.somma_cos) * 180.0 / PI);
            i.errore_gradi = std::max(2.0, sigma / std::sqrt(static_cast<double>(i.passaggi)));
            // distanza dall'ampiezza, circa +-40% a 1 sigma per misura; media mobile sui passaggi recenti
            const double campione = std::log10(d) + 0.15 * gauss();
            tr.somma_log_distanza = i.passaggi == 1 ? campione : 0.7 * tr.somma_log_distanza + 0.3 * campione;
            i.distanza_stimata_m = std::pow(10.0, tr.somma_log_distanza);
            i.ampiezza = std::clamp(5 - static_cast<int>(d / portata * 5.0), 1, 5);
            // tendenza: il livello (sola andata) di un minuto fa
            const double db = -20.0 * std::log10(d);
            tr.livelli.push_back({t, db});
            while (tr.livelli.size() > 1 && t - tr.livelli.front().first > 120.0) tr.livelli.pop_front();
            i.tendenza = Tendenza::stabile;
            for (const auto& [tv, dbv] : tr.livelli)
                if (t - tv >= 60.0) {
                    const double pendenza = (db - dbv) / ((t - tv) / 60.0);
                    i.tendenza = pendenza > 3.0 ? Tendenza::avvicinamento : pendenza < -3.0 ? Tendenza::allontanamento : Tendenza::stabile;
                    break;
                }
            // identificazione: periodo dal secondo passaggio, identita' dal terzo
            if (i.passaggi >= 3 && !tr.identita_decisa) {
                tr.identita_decisa = true;
                if (caso_.uniforme() < (r.agilita_frequenza ? 0.3 : r.firma_unica ? 0.9 : 0.6)) {
                    i.identificato = true;
                    i.identita = r.nome;
                    if (caso_.uniforme() < 0.1)   // errore: un altro del gruppo d'ambiguita'
                        for (const auto& [id, altro] : catalogo_->radar)
                            if (id != r.id && !r.gruppo.empty() && altro.gruppo == r.gruppo) { i.identita = altro.nome; break; }
                }
            }
            if (!i.identificato) i.identita = std::string("radar ") + nome_ruolo(r.ruolo) + " in banda " + r.banda;
            if (i.passaggi < 2) i.identita = std::string("radar in banda ") + r.banda;
            // rischio di contro-scoperta
            if (e.puntato_su_di_noi) {
                i.rischio = Rischio::scoperto;
                if (!tr.illuminato_avvisato) { tr.illuminato_avvisato = true; eventi_.push_back({EventoEsm::Tipo::illuminato, t, e.unita, r.id}); }
            } else {
                const double r50 = r50_m(r, esposizione, stato_mare);
                const double rho = r50 > 0.0 ? i.distanza_stimata_m / r50 : std::numeric_limits<double>::infinity();
                i.rischio = rho <= 1.0 ? Rischio::probabile : rho <= 2.0 ? Rischio::attenzione : Rischio::sicuro;
            }
            const bool alto = i.rischio == Rischio::probabile || i.rischio == Rischio::scoperto;
            if (alto && !tr.rischio_avvisato) eventi_.push_back({EventoEsm::Tipo::rischio_alto, t, e.unita, r.id});
            tr.rischio_avvisato = alto;
        }
    }
    // emettitori persi
    for (auto it = tracce_.begin(); it != tracce_.end();) {
        const Radar* r = catalogo_->trova_radar(it->first.second);
        const double limite = std::max(PERDITA_S, 3.0 * (r ? r->periodo_s : 3.0));
        if (t - it->second.i.ultima > limite) {
            eventi_.push_back({EventoEsm::Tipo::perso, t, it->first.first, it->first.second});
            it = tracce_.erase(it);
        } else {
            ++it;
        }
    }
    lista_.clear();
    for (const auto& [k, tr] : tracce_) lista_.push_back(tr.i);
}

int EsmBattello::livello_originale() const {
    int livello = 0;
    for (const auto& i : lista_) livello = std::max(livello, 17 * i.ampiezza);   // 5 -> 85
    return livello;
}

std::vector<EventoEsm> EsmBattello::eventi() {
    std::vector<EventoEsm> v;
    v.swap(eventi_);
    return v;
}

}  // namespace m688
