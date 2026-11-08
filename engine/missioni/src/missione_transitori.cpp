#include <algorithm>
#include <cmath>
#include <string>

#include "missioni/missione.hpp"
#include "nucleo/transitori_armi.hpp"

// Transitori delle armi e degli scafi nella scena acustica dell'Enhanced (pezzo R7, nucleo/transitori_armi.hpp del
// disassemblatore): lanci dal tubo dei sottomarini, esplosioni dei colpi, navi che affondano, scafi che implodono e
// i ping delle teste attive. Solo con il catalogo acustico (l'acustica dell'Enhanced): il classico non ne ha.
// Fasce di id: 960000-979999 lanci e teste, 980000-989999 esplosioni, implosioni e affondamenti.

namespace m688 {

namespace {

constexpr int LANCI_DA = 960000, LANCI_A = 979999, SCOPPI_DA = 980000, SCOPPI_A = 989999;
constexpr double TNT_PREDEFINITO_KG = 100.0;   // colpo senza carica nota (STIMA)
// volume di uno scafo che implode senza dati della classe: quello di un 688 (volume_implosione_m3, ~3240 m^3)
constexpr double VOLUME_SCAFO_M3 = 3240.0;
constexpr double AFFONDA_M_S = 2.0;            // nave che affonda: velocita' di discesa verso il fondo (STIMA)

bool e_arma(const UnitaClassica& u) { return (u.flag & 0x20) != 0 || u.categoria == "arma"; }

TipoLancio tipo_lancio(const std::string& classe) {
    if (classe.find("harpoon") != std::string::npos) return TipoLancio::harpoon;
    if (classe.find("tomahawk") != std::string::npos || classe.find("tlam") != std::string::npos) return TipoLancio::tomahawk_tubo;
    return TipoLancio::siluro;
}

// il prossimo id della fascia per `n` transitori, ricominciando dall'inizio quando la fascia finisce (i vecchi sono
// finiti da un pezzo)
int prenota(int& prossimo, int da, int a, std::size_t n) {
    if (prossimo + static_cast<int>(n) > a) prossimo = da;
    const int id = prossimo;
    prossimo += static_cast<int>(n);
    return id;
}

}  // namespace

constexpr double COLPO_IA_S = 2.0;   // come colpo_s del giocatore (nucleo/armi_giocatore.hpp)

void Missione::passo_transitori_armi() {
    const auto aggiungi = [&](std::vector<Transitorio> v) {
        for (auto& x : v) transitori_armi_.push_back(std::move(x));
    };
    // lanci dal tubo: le armi nuove di un sottomarino immerso (non le esche, tranne le semoventi)
    for (const auto& [id, u] : unita_) {
        if (!e_arma(u) || ((u.flag & 0x80) && !esche_mobili_.count(id)) || armi_viste_.count(id)) continue;
        armi_viste_.insert(id);
        const auto* l = trova(u.lanciatore);
        if (!l || l->categoria != "sottomarino" || l->quota <= 3.0) continue;
        // il lanciatore dell'IA si fa sentire come il giocatore (stazioni_missione: evento_acustico al lancio), per la
        // simmetria (disassemblatore, 31/10/2026): prima il lancio dell'IA non alzava il suo rumore
        if (!l->giocatore) evento_acustico(l->id, COLPO_IA_S);
        const std::uint64_t seme = seme_ ^ ((static_cast<std::uint64_t>(id) + 1) * 0xA24BAED4963EE407ULL);
        auto v = transitori_lancio(tipo_lancio(u.classe), l->pos, l->quota, tempo_, false, 0, seme);
        const int base = prenota(prossimo_lancio_tr_, LANCI_DA, LANCI_A, v.size());
        for (std::size_t i = 0; i < v.size(); ++i) { v[i].id = base + static_cast<int>(i); v[i].origine = l->id; }
        aggiungi(std::move(v));
    }
    // esplosioni dei colpi: la carica vera se c'e', se no la testata della classe
    for (; colpi_suonati_ < colpi_.size(); ++colpi_suonati_) {
        const Colpo& c = colpi_[colpi_suonati_];
        double tnt = c.tnt_kg;
        if (tnt <= 0.0)
            if (const DatiSiluro* d = tabella_siluri_.siluro(c.classe_arma)) tnt = d->testata_kg;
        if (tnt <= 0.0) tnt = TNT_PREDEFINITO_KG;
        const int base = prenota(prossimo_scoppio_tr_, SCOPPI_DA, SCOPPI_A, 4);
        aggiungi(transitori_esplosione(tnt, c.pos, std::max(0.5, c.quota), tempo_, base, c.lanciatore));
    }
    // navi che affondano e sottomarini distrutti oltre la loro quota massima (lo scafo implode)
    for (const auto& [id, u] : unita_) {
        if (u.affondamento <= 0 || affondati_visti_.count(id)) continue;
        affondati_visti_.insert(id);
        const std::uint64_t seme = seme_ ^ ((static_cast<std::uint64_t>(id) + 3) * 0x9FB21C651E98DF25ULL);
        if (u.categoria == "nave") {
            const double fondo = profondita(u.pos);
            const double t_fondo = std::isfinite(fondo) && fondo < 2000.0 ? tempo_ + fondo / AFFONDA_M_S : -1.0;
            const int base = prenota(prossimo_scoppio_tr_, SCOPPI_DA, SCOPPI_A, 40);
            aggiungi(transitori_affondamento(u.pos, tempo_, t_fondo, std::isfinite(fondo) ? fondo : 4000.0, base, seme, id));
        } else if (u.categoria == "sottomarino" && u.quota >= u.quota_max) {
            const int base = prenota(prossimo_scoppio_tr_, SCOPPI_DA, SCOPPI_A, 40);
            const auto dim = dimensioni_classi_.find(u.classe);
            const double volume = dim != dimensioni_classi_.end() && dim->second.lunghezza_m > 0.0
                                      ? volume_implosione_m3(dim->second.lunghezza_m, dim->second.larghezza_m) : VOLUME_SCAFO_M3;
            aggiungi(transitori_implosione(u.quota, volume, u.pos, tempo_, base, seme, id));
        }
    }
    // ping delle teste attive, alla cadenza della distanza dal bersaglio (piu' fitta vicino)
    for (const auto& [id, sm] : siluri_) {
        const Siluro& s = sm.siluro;
        if (!s.ping_in_corso()) continue;
        const std::string voce = transitorio_testa(s.dati().classe);
        if (voce.empty()) continue;
        double d = s.dati().acquisizione_attiva_m;
        if (const auto* b = trova(s.bersaglio())) d = distanza(s.pos(), b->pos);
        const auto it = ultimo_ping_testa_.find(id);
        if (it != ultimo_ping_testa_.end() && tempo_ - it->second < intervallo_ping_testa_s(d, s.dati().acquisizione_attiva_m)) continue;
        ultimo_ping_testa_[id] = tempo_;
        if (const SorgenteLibreria* t = catalogo_->transitorio(voce)) emetti_impulso(id, voce, t->livello_db);   // E
        const int base = prenota(prossimo_lancio_tr_, LANCI_DA, LANCI_A, 1);
        transitori_armi_.push_back({base, voce, s.pos(), s.quota(), tempo_, id, seme_ ^ static_cast<std::uint64_t>(base), 0.0});
    }
    std::erase_if(transitori_armi_, [&](const Transitorio& t) { return transitorio_finito(*catalogo_, t, tempo_); });
}

// D3, l'incidente provocato (docs/28 §3.5): uno scoppio in acqua senza arma e senza lanciatore. Si sente come quello di
// un'arma vera (gli stessi transitori); chi gli sta vicino reagisce come a un'arma: la formazione entro SCOPPIO_ALLERTA_M va
// ad A3, il sottomarino entro SCOPPIO_EVASIONE_M evade dal punto. Senza attribuzione nessuno provoca nessuno (D4)
void Missione::scoppio_provocato(Vec2 p, double quota_m, double kg) {
    constexpr double SCOPPIO_ALLERTA_M = 20000.0, SCOPPIO_EVASIONE_M = 3000.0;   // STIMA, come il transitorio di lancio
    if (catalogo_) {
        const int base = prenota(prossimo_scoppio_tr_, SCOPPI_DA, SCOPPI_A, 4);
        for (auto& t : transitori_esplosione(std::max(kg, 1.0), p, std::max(0.5, quota_m), tempo_, base, -1)) transitori_armi_.push_back(std::move(t));
    }
    std::vector<int> vicine;
    for (const auto& [id, u] : unita_)
        if (!u.distrutta && !u.persa && !e_arma(u) && (u.flag & 3) != 3 && distanza(u.pos, p) <= SCOPPIO_ALLERTA_M) vicine.push_back(id);
    for (const int id : vicine) {
        const auto* u = trova(id);
        if (!u) continue;
        if (forze_ && !u->giocatore) evento_allerta(id, "arma");
        if (!u->giocatore && u->categoria == "sottomarino" && u->quota > 3.0 && distanza(u->pos, p) <= SCOPPIO_EVASIONE_M)
            stimola_evasione(id, StimoloEvasione::ping_vicino, p);
    }
}

}  // namespace m688
