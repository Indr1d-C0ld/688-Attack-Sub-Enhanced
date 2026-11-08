#include "nucleo/mine.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

#include <toml.hpp>

namespace m688 {

namespace {
constexpr double VISTA_OPERATORE = 3.0;   // la mina comandata: l'operatore segue il battello entro 3 raggi (STIMA)
}  // namespace

std::map<std::string, DatiMina> carica_mine(const std::string& percorso) {
    toml::table t;
    try {
        t = toml::parse_file(percorso);
    } catch (const toml::parse_error& e) {
        throw std::runtime_error(percorso + ": " + std::string(e.description()));
    }
    std::map<std::string, DatiMina> out;
    if (const auto* mine = t["mine"].as_table())
        for (auto&& [chiave, nodo] : *mine) {
            const auto* tab = nodo.as_table();
            if (!tab) continue;
            const toml::table& r = *tab;
            DatiMina d;
            d.id = std::string(chiave.str());
            const std::string tipo = r["tipo"].value_or(std::string{});
            if (tipo == "influenza") d.tipo = TipoMina::influenza;
            else if (tipo == "captor") d.tipo = TipoMina::captor;
            else if (tipo == "mina_siluro") d.tipo = TipoMina::mina_siluro;
            else if (tipo == "mina_razzo") d.tipo = TipoMina::mina_razzo;
            else throw std::runtime_error(percorso + ": mina '" + d.id + "' di tipo sconosciuto '" + tipo + "'");
            d.carica_kg = r["carica_kg"].value_or(0.0);
            d.raggio_m = r["raggio_m"].value_or(0.0);
            d.profondita_min_m = r["profondita_min_m"].value_or(0.0);
            d.profondita_max_m = r["profondita_max_m"].value_or(1e9);
            d.corsa_max_m = r["corsa_max_m"].value_or(0.0);
            d.velocita_corsa_kn = r["velocita_corsa_kn"].value_or(0.0);
            d.ormeggio_m = r["ormeggio_m"].value_or(0.0);
            d.quota_mina_m = r["quota_mina_m"].value_or(0.0);
            d.errore_razzo_m = r["errore_razzo_m"].value_or(0.0);
            d.armamento_s = r["armamento_s"].value_or(600.0);
            d.siluro = r["siluro"].value_or(std::string{});
            d.navi = r["navi"].value_or(true);
            d.sottomarini = r["sottomarini"].value_or(true);
            d.quota_min_bersaglio_m = r["quota_min_bersaglio_m"].value_or(0.0);
            d.solo_sopra = r["solo_sopra"].value_or(false);
            d.fonte = r["fonte"].value_or(std::string{});
            d.nota = r["nota"].value_or(std::string{});
            out[d.id] = std::move(d);
        }
    return out;
}

const DatiMina* CampoMinato::dati(const std::string& tipo) const {
    const auto it = dati_.find(tipo);
    return it == dati_.end() ? nullptr : &it->second;
}

int CampoMinato::posa(const std::string& tipo, Vec2 da, double rotta, double distanza_m, double tempo, int posatore, int lato,
                      bool gia_armata, bool sicura) {
    const DatiMina* d = dati(tipo);
    if (!d) return -1;
    Mina m;
    m.id = prossimo_id_++;
    m.tipo = tipo;
    m.pos = da;
    m.posata_t = tempo;
    m.posatore = posatore;
    m.lato = lato;
    m.rotta = normalizza_360(rotta);
    m.gia_armata = gia_armata;
    m.sicura = sicura;
    m.corsa_m = d->tipo == TipoMina::influenza ? std::clamp(distanza_m, 0.0, d->corsa_max_m) : 0.0;
    m.stato = Mina::Stato::in_corsa;   // anche le altre: si posano al primo passo, nel punto del lancio
    mine_.push_back(m);
    return m.id;
}

void CampoMinato::posa_sul_fondo(Mina& m, const DatiMina& d, double tempo, double fondo, Esito& e) {
    m.fondale = fondo;
    if (fondo < d.profondita_min_m || fondo > d.profondita_max_m) {   // fondale inadatto (o terra): inerte
        m.stato = Mina::Stato::inerte;
        m.quota = std::max(0.0, fondo);
        e.inerti.push_back(m.id);
        return;
    }
    m.quota = std::max(0.0, fondo - d.ormeggio_m);
    if (d.quota_mina_m > 0.0) m.quota = std::min(m.quota, d.quota_mina_m);
    if (m.comandata && m.alla_stazione) {   // comandata e gia' alla stazione: pronta all'ordine
        m.stato = Mina::Stato::armata;
        m.armata_t = tempo;
        e.posate.push_back(m.id);
        return;
    }
    if (m.sicura || m.comandata) {   // in sicurezza (o comandata in attesa): al suo posto e alla sua quota, inerte
        m.stato = Mina::Stato::inerte;
        e.posate.push_back(m.id);
        return;
    }
    m.armata_t = m.gia_armata ? tempo : tempo + d.armamento_s;
    m.stato = Mina::Stato::armandosi;
    e.posate.push_back(m.id);
}

CampoMinato::Esito CampoMinato::avanza(double tempo, double dt_m, const std::vector<BersaglioMina>& bersagli,
                                       const std::function<double(Vec2)>& fondale) {
    Esito e;
    const auto fondo = [&](Vec2 p) { return fondale ? fondale(p) : 1000.0; };
    for (auto& m : mine_) {
        const DatiMina* d = dati(m.tipo);
        if (!d) continue;
        switch (m.stato) {
            case Mina::Stato::in_corsa: {
                const double passo = std::min(m.corsa_m, d->velocita_corsa_kn * NODO * dt_m);
                if (passo > 0.0) {
                    const double r = radianti(m.rotta);
                    m.pos = {m.pos.x + passo * std::sin(r), m.pos.y + passo * std::cos(r)};
                    m.corsa_m -= passo;
                }
                const double f = fondo(m.pos);
                if (f <= 0.0) { posa_sul_fondo(m, *d, tempo, f, e); break; }   // a terra durante la corsa
                if (m.corsa_m <= 1e-9) posa_sul_fondo(m, *d, tempo, f, e);
                break;
            }
            case Mina::Stato::armandosi:
                if (tempo + 1e-9 >= m.armata_t) {
                    m.stato = Mina::Stato::armata;
                    e.armate.push_back(m.id);
                }
                break;
            case Mina::Stato::armata: {
                // il bersaglio adatto piu' vicino entro il raggio (a parita', l'id piu' basso)
                const BersaglioMina* scelto = nullptr;
                double migliore = std::numeric_limits<double>::max();
                if (m.comandata) {
                    // l'operatore della stazione segue sul suo tracciato un sottomarino sopra la mina, entro VISTA_OPERATORE
                    // volte il raggio (in pianta; STIMA), e la fa scoppiare dopo il suo ritardo (10-30 s, STIMA, fisso per
                    // mina) se e' ancora li': il danno lo da' la distanza vera di quel momento. Se intanto se ne va, niente
                    // (prima la vista era il raggio stesso: a 10 nodi un battello attraversa i 50 m della MDT in 10 s e non
                    // scoppiava mai; la S18, 31/10/2026)
                    for (const auto& b : bersagli) {
                        if (!b.sottomarino || b.quota >= m.quota) continue;
                        const double h = distanza(m.pos, b.pos);
                        if (h > VISTA_OPERATORE * d->raggio_m || h > migliore || (h == migliore && scelto && b.id > scelto->id)) continue;
                        migliore = h;
                        scelto = &b;
                    }
                    if (!scelto) { m.ordine_t = -1.0; break; }
                    if (m.ordine_t < 0.0) { m.ordine_t = tempo + 10.0 + static_cast<double>((m.id * 7919) % 21); break; }
                    if (tempo + 1e-9 < m.ordine_t) break;
                    migliore = std::hypot(migliore, m.quota - scelto->quota);
                } else {
                    for (const auto& b : bersagli) {
                        if (!(b.nave && d->navi) && !(b.sottomarino && d->sottomarini)) continue;
                        if (b.sottomarino && b.quota < d->quota_min_bersaglio_m) continue;
                        if (d->solo_sopra && b.quota >= m.quota) continue;
                        const double h = distanza(m.pos, b.pos), dz = m.quota - b.quota;
                        const double r = std::sqrt(h * h + dz * dz);
                        if (r > d->raggio_m || r > migliore || (r == migliore && scelto && b.id > scelto->id)) continue;
                        migliore = r;
                        scelto = &b;
                    }
                }
                if (!scelto) break;
                InnescoMina x;
                x.mina = m.id;
                x.bersaglio = scelto->id;
                x.posatore = m.posatore;
                x.lato = m.lato;
                x.tipo = m.tipo;
                x.siluro = d->tipo != TipoMina::influenza;
                x.classe_siluro = d->siluro;
                x.pos = m.pos;
                x.quota = m.quota;
                x.distanza_m = migliore;
                x.dz_m = m.quota - scelto->quota;
                x.carica_kg = d->carica_kg;
                x.rotta = rilevamento(m.pos, scelto->pos);
                if (d->tipo == TipoMina::mina_razzo) {   // il razzo arriva sul bersaglio e scoppia accanto a lui
                    x.siluro = false;
                    x.classe_siluro.clear();
                    x.pos = scelto->pos;
                    x.quota = scelto->quota;
                    x.distanza_m = d->errore_razzo_m;
                    x.dz_m = 0.0;
                }
                e.inneschi.push_back(x);
                m.stato = Mina::Stato::scoppiata;
                break;
            }
            case Mina::Stato::scoppiata:
            case Mina::Stato::inerte: break;
        }
    }
    return e;
}

void CampoMinato::imposta_comando(int id, const std::string& gruppo) {
    for (auto& m : mine_)
        if (m.id == id) {
            m.comandata = true;
            m.gruppo = gruppo;
        }
}

int CampoMinato::comanda(const std::string& gruppo) {
    int n = 0;
    for (auto& m : mine_) {
        if (!m.comandata || m.alla_stazione || m.stato == Mina::Stato::scoppiata || (!gruppo.empty() && m.gruppo != gruppo)) continue;
        m.alla_stazione = true;
        if (m.stato == Mina::Stato::inerte) m.stato = Mina::Stato::armata;   // gia' sul fondo: pronta all'ordine
        ++n;
    }
    return n;
}

}  // namespace m688
