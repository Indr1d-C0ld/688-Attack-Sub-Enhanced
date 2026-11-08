#include "nucleo/luci_navigazione.hpp"

#include <cmath>
#include <stdexcept>

#include <toml.hpp>

namespace m688::luci {

namespace {

constexpr double MIGLIO_M = 1852.0;

Colore colore_da_nome(const std::string& s) {
    if (s == "rosso") return Colore::rosso;
    if (s == "verde") return Colore::verde;
    if (s == "giallo") return Colore::giallo;
    if (s == "ambra") return Colore::ambra;
    if (s == "luci_coperta") return Colore::luci_coperta;
    return Colore::bianco;
}

// l'angolo in (-180, 180]
double normale(double a) {
    a = std::fmod(a, 360.0);
    if (a <= -180.0) a += 360.0;
    if (a > 180.0) a -= 360.0;
    return a;
}

}  // namespace

Stato stato_da_nome(const std::string& nome, bool* ok) {
    if (ok) *ok = true;
    if (nome == "moto" || nome == "accese") return Stato::moto;
    if (nome == "fonda") return Stato::fonda;
    if (nome == "pesca") return Stato::pesca;
    if (nome == "manovra_limitata") return Stato::manovra_limitata;
    if (nome == "non_governa") return Stato::non_governa;
    if (ok) *ok = nome == "spente";
    return Stato::spente;
}

Stato stato_predefinito(const std::string& tipo, double velocita_m_s, bool guerra, bool missione_spente) {
    constexpr double NODO = 1852.0 / 3600.0;
    if (missione_spente) return Stato::spente;
    if (guerra && (tipo == "militare" || tipo == "sottomarino")) return Stato::spente;
    const double nodi = std::abs(velocita_m_s) / NODO;
    if (tipo == "peschereccio" && nodi < 5.0) return Stato::pesca;
    if (nodi < 0.5) return Stato::fonda;
    return Stato::moto;
}

std::map<std::string, Configurazione> carica(const std::string& percorso) {
    toml::table t;
    try {
        t = toml::parse_file(percorso);
    } catch (const toml::parse_error& e) {
        throw std::runtime_error("luci: " + percorso + ": " + std::string(e.description()));
    }
    std::map<std::string, Configurazione> out;
    const auto* classi = t["classe"].as_table();
    if (!classi) return out;
    for (const auto& [chiave, nodo] : *classi) {
        const auto* c = nodo.as_table();
        if (!c) continue;
        Configurazione cfg;
        cfg.classe = std::string(chiave.str());
        cfg.tipo = (*c)["tipo"].value_or(std::string());
        cfg.pesca = (*c)["pesca"].value_or(std::string());
        cfg.lunghezza_m = (*c)["lunghezza_m"].value_or(0.0);
        if (const auto* fa = (*c)["fanale"].as_array())
            for (const auto& x : *fa) {
                const auto* f = x.as_table();
                if (!f) continue;
                Fanale l;
                l.ruolo = (*f)["ruolo"].value_or(std::string());
                l.colore = colore_da_nome((*f)["colore"].value_or(std::string("bianco")));
                if (const auto* p = (*f)["pos_m"].as_array())
                    for (std::size_t i = 0; i < 3 && i < p->size(); ++i) l.pos_m[i] = (*p)[i].value_or(0.0);
                if (const auto* s = (*f)["settore"].as_array(); s && s->size() == 2)
                    l.settore_da = (*s)[0].value_or(-180.0), l.settore_a = (*s)[1].value_or(180.0);
                l.portata_m = (*f)["portata_nm"].value_or(0.0) * MIGLIO_M;
                if (const auto* st = (*f)["stati"].as_array())
                    for (const auto& n : *st) {
                        bool ok = false;
                        const Stato s = stato_da_nome(n.value_or(std::string()), &ok);
                        if (ok && s != Stato::spente) l.stati = static_cast<std::uint8_t>(l.stati | (1u << static_cast<int>(s)));
                    }
                l.lampi_7077 = (*f)["lampi"].value_or(std::string()) == "707.7";
                cfg.fanali.push_back(std::move(l));
            }
        out.emplace(cfg.classe, std::move(cfg));
    }
    return out;
}

bool nel_settore(const Fanale& f, double rilevamento_relativo_gradi) {
    const double ampiezza = std::fmod(f.settore_a - f.settore_da + 720.0, 360.0);
    if (ampiezza == 0.0 && f.settore_a != f.settore_da) return true;   // tutt'intorno: da -180 a 180
    const double d = std::fmod(normale(rilevamento_relativo_gradi) - f.settore_da + 720.0, 360.0);
    return d <= ampiezza + 1e-9;
}

bool lampo_acceso(double tempo_s) {
    const double c = std::fmod(std::fmod(tempo_s, 6.0) + 6.0, 6.0);
    return c < 3.0 && std::fmod(c, 1.0) < 0.5;
}

std::vector<const Fanale*> accesi(const Configurazione& c, Stato s, double tempo_s) {
    std::vector<const Fanale*> out;
    if (s == Stato::spente) return out;
    const auto bit = static_cast<std::uint8_t>(1u << static_cast<int>(s));
    for (const auto& f : c.fanali)
        if ((f.stati & bit) && (!f.lampi_7077 || lampo_acceso(tempo_s))) out.push_back(&f);
    return out;
}

}  // namespace m688::luci
