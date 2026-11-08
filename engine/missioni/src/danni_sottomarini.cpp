#include "missioni/danni_sottomarini.hpp"

#include <algorithm>
#include <cmath>

#include "nucleo/armi_giocatore.hpp"
#include "nucleo/armi_ia.hpp"
#include "nucleo/carico.hpp"
#include "nucleo/geometria.hpp"
#include "nucleo/missili.hpp"

namespace m688 {

namespace {

constexpr double ASCOLTO_M = 20.0 * MIGLIO;   // sonar del giocatore: 20 nm, come per le navi (D5)
constexpr double PESO_SIGNIFICATIVO = 5.0e4;  // N (circa 5 t): sotto, la quota la decidono ancora barra e script
constexpr double QUOTA_EMERSIONE_IA = 150.0;  // m: l'IA tenta l'emersione d'emergenza solo sopra (dossier 07 §8.4)
constexpr double FONDO_SENZA_SCAMPO = 600.0;  // s pesante sul fondo con l'aria finita: battello perso
constexpr double VELOCITA_EMERSO_KN = 3.0;    // emerso danneggiato: quasi fermo
constexpr double ROTTURA_PROFONDA = 300.0;    // m: oltre, in superficie arrivano tardi nafta e pochi rottami (dossier 07 §8.3)
// teste dei siluri leggeri in TNT equivalente: Mk 46, 44 kg di PBXN-103 = 108 kg (la stessa testa del Mk 54: dossier 07
// §1.5, [Brain16]); AT-1, 70 kg di miscela sovietica alluminizzata, x 1,5 (STIMA: nessuna tabella pubblica, 07 §13 punto 3)
constexpr double TNT_MK46 = 108.0, TNT_AT1 = 105.0;
// lo Sting Ray: 45 kg in carica cava (docs/ricerca/38); lo scoppio conta come la carica, il getto fora lo scafo
// resistente anche oltre l'intercapedine dei doppi scafi: una falla di ~25 cm e un incendio dove entra (STIMA)
constexpr double TNT_STINGRAY = 45.0, FALLA_CAVA_M2 = 0.05;
// Mk 48 (e ADCAP): 295 kg di PBXN-103 = 544 kg di TNT (dossier 07 §1.5, [NW-US], [Brain16])
constexpr double TNT_MK48 = 544.0;
// l'Alfa (dossier 19): il peso di un'arma che esce dal tubo (SET-65 1,9 t; un'esca circa 0,3 t; una mina 1 t) e la
// compressione dello scafo con la quota (qualche tonnellata a 600 m). STIMA.
constexpr double ARMA_KG = 1900.0, ESCA_KG = 300.0, MINA_KG = 1000.0;
constexpr double COMPRESSIONE_ALFA_T_100M = 0.5;
// i missili che missili.toml non ha: la frazione d'alto esplosivo della testata (kit, nota kit-depositi-carburante-dati.md
// §0.1: Harpoon 100 kg di 221, Tomahawk 205 di 454, Exocet e Penguin 0,45; STIMA)
constexpr double FRAZIONE_ESPLOSIVO_MISSILE = 0.45;
constexpr int MSG_TAN_INSERITO = 443, MSG_TAN_DISINSERITO = 444, MSG_TAN_FUORI_SERVIZIO = 445, MSG_AVARIA_CASSE = 446;
constexpr int MSG_SFOGHI_APERTI = 449;
constexpr int MSG_SGOMBERO = 415, MSG_ISOLATO = 416, MSG_RIAPERTO = 417, MSG_COMANDO_NON_ISOLABILE = 418, MSG_UOMINI_PERDUTI = 419, MSG_COMPARTIMENTO_PERDUTO = 468;

}  // namespace

double EmersioneViolenta::altezza_prua() const {
    return fuori * lunghezza * std::sin(radianti(angolo));
}

PosaNave posa_emersione(const EmersioneViolenta& e, double t) {
    // la prua ruota attorno al punto dello scafo che resta al galleggiamento; prima sale dal basso (4 m), poi ricade
    // con l'accelerazione di gravita', infine beccheggio, sussulto e rollio smorzati
    PosaNave p;
    p.danneggiata = true;   // disegno dal modello 3D: lo sprite precalcolato non ha assetti
    auto& a = p.prodiero;
    a.x_perno = e.lunghezza / 2 - e.fuori * e.lunghezza;
    const double tau = t - e.inizio;
    if (tau < e.salita_s) {
        const double k = std::max(tau, 0.0) / e.salita_s;
        a.assetto = e.angolo * std::sin(k * PI / 2);
        a.immersione = 4.0 * (1 - k) * (1 - k);
    } else if (tau < e.salita_s + e.caduta_s) {
        const double k = (tau - e.salita_s) / e.caduta_s;
        a.assetto = e.angolo * (1 - k * k);
    } else {
        const double k = tau - e.salita_s - e.caduta_s, smorza = std::exp(-k / 6.0);
        a.assetto = -0.2 * e.angolo * smorza * std::sin(2 * PI * k / 7.0);
        a.immersione = 1.2 * std::exp(-k / 4.0) * std::cos(2 * PI * k / 5.0);
        a.sbandamento = 4.0 * std::exp(-k / 8.0) * std::sin(2 * PI * k / 9.0) * (e.unita % 2 ? 1.0 : -1.0);
    }
    return p;
}

DanniSottomarini::DanniSottomarini(const std::map<std::string, ClasseNave>& classi, std::string cartella_dati,
                                   std::uint64_t seme)
    : classi_(classi), dati_(std::move(cartella_dati)), caso_(seme ^ 0xd6c0u) {}

bool DanniSottomarini::ha_fisica(const UnitaClassica& u) const {
    if (u.categoria != "sottomarino") return false;
    const auto it = classi_.find(u.classe);
    return it != classi_.end() && !it->second.scafo.empty();
}

void DanniSottomarini::esaurisci(const Missione& m, int unita) {
    const auto it = m.unita().find(unita);
    if (it == m.unita().end() || !ha_fisica(it->second)) return;
    prendi(it->second, m.tempo()).danni->esaurisci();
}

bool DanniSottomarini::consegna(const Missione& m, const Ordine& o) {
    if (o.tipo == TipoOrdine::isola_compartimento || o.tipo == TipoOrdine::priorita_sistema) {   // il controllo danni, sui due battelli
        const auto* g = m.giocatore();
        if (!g || !ha_fisica(*g)) return false;
        auto& s = prendi(*g, m.tempo());
        auto& b = *s.danni;
        if (s.distrutto) return true;
        if (o.tipo == TipoOrdine::priorita_sistema) {
            if (o.n >= 0 && o.n < static_cast<int>(Sistema::numero)) b.priorita(static_cast<Sistema>(o.n));
        } else if (o.a >= 2.0) {
            b.riapri(o.n);
        } else if (o.a >= 1.0) {
            b.chiudi_subito(o.n);
        } else if (o.n == b.compartimento_di(Sistema::inerziale)) {
            annuncia(m, g->id, MSG_COMANDO_NON_ISOLABILE, true);
        } else if (!b.isolabile(o.n)) {
            annuncia(m, g->id, MSG_COMPARTIMENTO_PERDUTO, true);
        } else {
            b.isola(o.n);
        }
        return true;
    }
    if (o.tipo != TipoOrdine::sfoga && o.tipo != TipoOrdine::compensa && o.tipo != TipoOrdine::tan) return false;
    const auto* g = m.giocatore();
    if (!g || g->classe != "alfa" || !ha_fisica(*g)) return false;
    auto& s = prendi(*g, m.tempo());
    auto& b = *s.danni;
    if (s.distrutto) return false;
    switch (o.tipo) {
        case TipoOrdine::sfoga:
            b.sfoga();
            annuncia(m, g->id, MSG_SFOGHI_APERTI, true);
            return true;
        case TipoOrdine::compensa:
            if (b.tan()) return false;
            b.compensa(o.a);
            return true;
        default:
            if (o.n != 0 && b.sistema(Sistema::pompe).efficienza < 0.5) {
                annuncia(m, g->id, MSG_TAN_FUORI_SERVIZIO, true);
                return false;
            }
            b.tan(o.n != 0);
            s.tan_prec = b.tan();
            annuncia(m, g->id, b.tan() ? MSG_TAN_INSERITO : MSG_TAN_DISINSERITO, true);
            return true;
    }
}

bool DanniSottomarini::tan(int unita) const {
    const auto* s = sottomarino(unita);
    return s && s->danni->tan();
}

bool DanniSottomarini::guasto_casse(int unita) const {
    const auto* s = sottomarino(unita);
    return s && s->danni->guasto_casse();
}

double esplosivo_arma(const CatalogoCarico& cat, const std::string& variante, const CatalogoMissili* missili, int anno) {
    const SchedaArma* a = variante.empty() ? nullptr : cat.arma(variante);
    if (!a) return 0.0;
    switch (a->tipo) {
        case TipoArmaCarico::esca: return 0.0;
        case TipoArmaCarico::missile: {   // l'esplosivo del colpo (missili.toml), per l'anno o, fuori dagli anni, il primo
            const DatiMissile* d = missili ? missili->per_arma(variante, anno) : nullptr;
            if (!d && missili)
                for (const auto& x : missili->armi)
                    if (x.arma == variante && (d = missili->missile(x.missile))) break;
            return d && d->esplosivo_kg > 0.0 ? d->esplosivo_kg : FRAZIONE_ESPLOSIVO_MISSILE * a->testata_kg;
        }
        default: return a->testata_kg;   // siluri e mine: la testata e' alto esplosivo
    }
}

double esplosivo_a_bordo(const StazioneArmi& armi, const CatalogoCarico& cat, const CatalogoMissili* missili, int anno) {
    double kg = 0.0;
    for (int t = 1; t <= armi.numero_tubi(); ++t)   // in ricarica l'arma e' gia' uscita dalla rastrelliera
        if (!armi.pozzo(t) && armi.stato(t) != StatoTubo::vuoto) kg += esplosivo_arma(cat, armi.variante(t), missili, anno);
    for (const auto& [v, n] : armi.rastrelliera()) kg += n * esplosivo_arma(cat, v, missili, anno);
    return kg;
}

double esplosivo_a_bordo(const CaricoIa& carico, const CatalogoCarico& cat, const std::string& classe, const CatalogoMissili* missili,
                         int anno) {
    double kg = 0.0;
    for (const auto& v : carico.tubi) kg += esplosivo_arma(cat, v, missili, anno);
    const LocaleSiluri* l = cat.locale(classe);
    int pozzi = l ? l->pozzi : 0;   // i pozzi verticali si riempiono per primi, fuori dallo scafo resistente
    for (const auto& [v, n] : carico.riserva) {
        int dentro = n;
        if (const SchedaArma* a = cat.arma(v); a && a->pozzo_verticale && pozzi > 0) {
            const int fuori = std::min(n, pozzi);
            pozzi -= fuori;
            dentro -= fuori;
        }
        kg += dentro * esplosivo_arma(cat, v, missili, anno);
    }
    return kg;
}

void DanniSottomarini::carico_vivo(int unita, double kg) {
    for (auto& s : sub_)
        if (s->unita == unita && !s->distrutto) applica_carico(*s, kg);
}

void DanniSottomarini::applica_carico(SottomarinoColpito& s, double kg) {
    // i locali siluri dello scafo (la Foxtrot ne ha due, a prua e a poppa): ognuno la sua parte, come la carica del kit
    auto& b = *s.danni;
    const auto& sc = scafi_.at(s.classe);
    std::vector<std::pair<int, double>> locali;
    double tutto = 0.0;
    for (const auto& c : sc.celle) {
        if (c.deposito != "siluri") continue;
        for (std::size_t k = 0; k < b.compartimenti().size(); ++k)
            if (b.compartimenti()[k].nome == c.nome) {
                const double w = c.carica_kg > 0.0 ? c.carica_kg : c.x1 - c.x0;
                locali.emplace_back(static_cast<int>(k), w);
                tutto += w;
            }
    }
    if (tutto <= 0.0) return;
    for (const auto& [k, w] : locali) {
        const auto& c = b.compartimenti()[static_cast<std::size_t>(k)];
        const bool saltato = s.carico_dato > 0.0 && c.carica_kg <= 0.0 && c.propellente_kg <= 0.0;
        if (!saltato) b.carico_deposito(k, std::max(0.0, kg) * w / tutto);
    }
    s.carico_dato = std::max(0.0, kg);
}

void DanniSottomarini::danni_iniziali(const Missione& m) {
    const auto& D = m.danni_iniziali();
    const auto* g = m.giocatore();
    if (D.vuoti() || !g || !ha_fisica(*g)) return;
    auto& b = *prendi(*g, m.tempo()).danni;
    const auto compartimento = [&](const std::string& c) {
        const auto& v = b.compartimenti();
        for (std::size_t i = 0; i < v.size(); ++i)
            if (v[i].nome == c || v[i].uso == c) return static_cast<int>(i);
        return -1;
    };
    if (D.urto) urto(m, g->id, D.urto_tnt_kg, D.urto_distanza_m, radianti(D.urto_angolo_gradi));
    for (const auto& [nome, eff] : D.sistemi)
        if (const auto s = sistema_da_chiave(nome)) {   // le chiavi dei sistemi: nucleo/battello.hpp
            const double perdita = b.sistema(*s).efficienza - eff;
            if (perdita > 0.0) b.guasto_sistema(*s, perdita, "danni della missione");
        }
    for (const auto& x : D.acqua) if (const int c = compartimento(x.compartimento); c >= 0) b.imbarca(c, x.valore);
    for (const auto& x : D.falle) if (const int c = compartimento(x.compartimento); c >= 0) b.aggiungi_falla(c, x.valore);
    for (const auto& x : D.incendi) if (const int c = compartimento(x.compartimento); c >= 0) b.incendio(c, std::clamp(x.valore, 0.0, 1.0));
}

void DanniSottomarini::pesa_lanci(const Missione& m, SottomarinoColpito& s) {
    auto& b = *s.danni;
    double prua = -1e9;
    for (const auto& c : b.compartimenti()) prua = std::max(prua, c.x1);   // i tubi: a prua
    for (const auto& [id, a] : m.unita())
        if ((a.flag & 0x20) && a.lanciatore == s.unita && !m.da_mina(id) && s.lanci_pesati.insert(id).second)
            b.varia_peso(-((a.flag & 0x80) ? ESCA_KG : ARMA_KG), prua);
    const auto& mine = m.campo_minato().mine();
    std::size_t posate = 0;
    for (const auto& x : mine) posate += x.posatore == s.unita;
    for (; s.mine_pesate < posate; ++s.mine_pesate) b.varia_peso(-MINA_KG, prua);
    // Tan che si disinserisce da solo (pompe), e la finestrella delle casse che si accende
    if (s.tan_prec && !b.tan()) annuncia(m, s.unita, MSG_TAN_FUORI_SERVIZIO, true);
    s.tan_prec = b.tan();
    const bool guasto = b.guasto_casse();
    if (guasto && !s.guasto_casse_prec) annuncia(m, s.unita, MSG_AVARIA_CASSE, true);
    s.guasto_casse_prec = guasto;
}

void DanniSottomarini::prepara(const Missione& m, int unita) {
    const auto it = m.unita().find(unita);
    if (it != m.unita().end() && ha_fisica(it->second)) prendi(it->second, m.tempo());
}

void DanniSottomarini::urto(const Missione& m, int unita, double tnt_kg, double distanza_m, double angolo_rad) {
    const auto it = m.unita().find(unita);
    if (it == m.unita().end() || !ha_fisica(it->second)) return;
    auto& b = *prendi(it->second, m.tempo()).danni;
    const auto& comp = b.compartimenti();
    const double x0 = comp.front().x1, x1 = comp.back().x0;
    b.urto(tnt_kg, distanza_m, angolo_rad, (x0 + x1) / 2 + caso_.uniforme(-0.3, 0.3) * (x0 - x1), it->second.quota);
}

void DanniSottomarini::collisione(const Missione& m, const Urto& u, const EsitoUrto& e) {
    for (const auto* p : {&u.a, &u.b}) {
        const auto it = m.unita().find(p->unita);
        if (p->unita < 0 || it == m.unita().end() || !ha_fisica(it->second)) continue;
        const DanniUrtoUnita& d = p == &u.a ? e.a : e.b;
        auto& s = prendi(it->second, m.tempo());
        if (s.superficie) s.superficie->urto_meccanico(d);
        else s.danni->urto_meccanico(d);
        s.rumore_urto_db = std::min(12.0, s.rumore_urto_db + d.rumore_db);
    }
}

const char* nome_guasto_testa(GuastoTesta g) {
    switch (g) {
        case GuastoTesta::intatta: return "intatta";
        case GuastoTesta::degradata: return "degradata";
        case GuastoTesta::bloccata: return "bloccata";
        case GuastoTesta::cieca: return "cieca";
    }
    return "?";
}

const DanniArtiglieriaBattello* DanniSottomarini::artiglieria(int unita) const {
    const auto it = artiglieria_.find(unita);
    return it == artiglieria_.end() ? nullptr : &it->second;
}

void DanniSottomarini::colpo_artiglieria(const Missione& m, const ImpattoArtiglieria& i) {
    using E = ImpattoArtiglieria::Esito;
    if (i.avvertimento || i.bersaglio < 0 || i.parte == ParteEsposta::nessuna) return;
    if (i.esito != E::diretto && i.esito != E::schegge) return;   // acqua, rimbalzo, tonfo, aria: niente sulle parti
    const auto it = m.unita().find(i.bersaglio);
    if (it == m.unita().end() || it->second.distrutta || it->second.persa) return;
    const UnitaClassica& u = it->second;
    auto& a = artiglieria_[i.bersaglio];
    const bool diretto = i.esito == E::diretto;
    (diretto ? a.colpi : a.schegge) += 1;
    a.ultimo = i.tempo_impatto_s;
    const double cal = i.calibro_mm;
    // le schegge: piu' probabili da vicino; r_eff del pezzo (artiglieria.toml, lo stesso del tiro), o per calibro se
    // l'impatto non lo porta (22 §5.3: 4 m col 5" e il 130, 2 col 76, 1 col 30)
    const double r_eff = i.r_eff_m > 0.0 ? i.r_eff_m : cal >= 100.0 ? 4.0 : cal >= 57.0 ? 2.0 : 1.0;
    const double f = diretto ? 1.0 : std::clamp(1.0 - std::max(i.distanza_parte_m, 0.0) / r_eff, 0.2, 1.0);
    SottomarinoColpito* s = ha_fisica(u) ? &prendi(u, m.tempo()) : nullptr;
    BattelloDanni* b = s && !s->distrutto ? s->danni.get() : nullptr;
    const std::uint32_t seme = static_cast<std::uint32_t>(caso_.prossimo() >> 32) | 1u;
    // il punto lungo lo scafo (coordinate del kit, prua positiva)
    const double rr = radianti(u.rotta);
    const double x = (i.punto.x - u.pos.x) * std::sin(rr) + (i.punto.y - u.pos.y) * std::cos(rr);
    switch (i.parte) {
        case ParteEsposta::periscopio_attacco:
        case ParteEsposta::periscopio_ricerca: {
            if (!diretto && caso_.uniforme() > f) break;   // le schegge mancano la testa
            const bool ricerca = i.parte == ParteEsposta::periscopio_ricerca;
            TestaColpita& t = ricerca ? a.ricerca : a.attacco;
            // 22 §5.3 col colpo diretto: vetro rotto 50%, testa piegata 30%, graffi 20%; le schegge piu' spesso graffi
            const double x0 = caso_.uniforme();
            const GuastoTesta g = diretto ? (x0 < 0.5 ? GuastoTesta::cieca : x0 < 0.8 ? GuastoTesta::bloccata : GuastoTesta::degradata)
                                          : (x0 < 0.25 ? GuastoTesta::cieca : x0 < 0.4 ? GuastoTesta::bloccata : GuastoTesta::degradata);
            if (static_cast<int>(g) > static_cast<int>(t.guasto)) t.guasto = g;   // peggiora soltanto
            t.incrinato = std::max(t.incrinato, g == GuastoTesta::cieca ? 0.9 : g == GuastoTesta::bloccata ? 0.35 : 0.45);
            t.spento = std::max(t.spento, g == GuastoTesta::cieca ? 0.5 : 0.0);
            t.allaga_canna |= g == GuastoTesta::cieca;
            t.seme = seme;
            t.tempo = i.tempo_impatto_s;
            if (ricerca) a.esm = false;   // l'ESM nella testa del Type 18 (dossier 20)
            if (b) {
                const std::string testo = std::string("colpo d'artiglieria sul periscopio ") + (ricerca ? "di ricerca" : "d'attacco") +
                                          ", testa " + nome_guasto_testa(t.guasto);
                b->guasto_sistema(Sistema::periscopio, g == GuastoTesta::degradata ? 0.2 : 0.5, testo);
            }
            break;
        }
        case ParteEsposta::albero_esm:
            if (diretto || caso_.uniforme() < f) a.esm = false;
            break;
        case ParteEsposta::albero_radio:
            if (diretto || caso_.uniforme() < f) {
                a.radio = false;
                if (b) b->guasto_sistema(Sistema::antenna, 1.0, "colpo d'artiglieria sull'antenna radio");
            }
            break;
        case ParteEsposta::radar:
            if (diretto || caso_.uniforme() < f) a.radar = false;
            break;
        case ParteEsposta::snorkel:
            if (diretto || caso_.uniforme() < f) a.snorkel = false;
            break;
        case ParteEsposta::falsatorre: {
            // 22 §9.5 per colpo da 5" e 130 mm: timoni della falsatorre bloccati 10-25%, falla piccola a un passaggio
            // 5-15%; coi calibri minori una frazione (STIMA: 76 mm x0,3, 30 mm x0,05), col 16" il triplo
            const double k = cal >= 400.0 ? 3.0 : cal >= 100.0 ? 1.0 : cal >= 57.0 ? 0.3 : 0.05;
            const bool piani = u.classe == "los_angeles" || u.classe == "sturgeon";
            const double p_timoni = k * caso_.uniforme(0.10, 0.25) * f, x_timoni = caso_.uniforme();   // in quest'ordine
            const double p_falla = k * caso_.uniforme(0.05, 0.15) * f, x_falla = caso_.uniforme();
            if (piani && x_timoni < p_timoni) {
                a.timoni_falsatorre_bloccati = true;
                if (b) b->guasto_sistema(Sistema::idroplani, 1.0, "colpo d'artiglieria: timoni della falsatorre bloccati");
            }
            if (b && x_falla < p_falla)
                b->aggiungi_falla(b->compartimento_di(Sistema::periscopio), caso_.uniforme(0.001, 0.004));   // infiltrazione
            break;
        }
        case ParteEsposta::plancia:
            // vedette e ufficiale di guardia: il 30 mm e' letale, i calibri grossi spazzano la plancia (STIMA)
            a.feriti_plancia += diretto ? 1 + static_cast<int>(caso_.uniforme() * (cal >= 100.0 ? 4.0 : 3.0))
                                        : static_cast<int>(caso_.uniforme() * 2.0 * f);
            a.plancia_colpita = i.tempo_impatto_s;
            break;
        case ParteEsposta::scafo: {
            // le estremita', fuori dallo scafo resistente: le casse zavorra a prua e a poppa sotto lo scafo leggero
            // (22 §5.4). Un colpo le squarcia: 0,15 col 30 mm, 0,4 col 57-76, 0,7 coi 100-130, sempre col 16"
            // (STIMA; le schegge in proporzione). La cassa perde l'aria e si allaga: il battello perde la sua parte di
            // riserva e si appruora o si appoppa (kit, 31/10/2026; l'effetto in missione e' del runtime)
            if (b && !b->casse().empty()) {
                double x_min = 1e9, x_max = -1e9;
                for (const auto& c : b->compartimenti()) {
                    x_min = std::min({x_min, c.x0, c.x1});
                    x_max = std::max({x_max, c.x0, c.x1});
                }
                if (x > x_max || x < x_min) {
                    int k = -1;
                    double vicina = 1e9;
                    for (std::size_t j = 0; j < b->casse().size(); ++j) {
                        const auto& c = b->casse()[j];
                        if ((x > x_max) != (c.xc > 0.0) || std::fabs(c.xc - x) >= vicina) continue;
                        vicina = std::fabs(c.xc - x);
                        k = static_cast<int>(j);
                    }
                    const double p_cassa = (cal >= 400.0 ? 1.0 : cal >= 100.0 ? 0.7 : cal >= 57.0 ? 0.4 : 0.15) * f;
                    if (k >= 0 && !b->casse()[static_cast<std::size_t>(k)].squarciata && caso_.uniforme() < p_cassa) {
                        b->squarcia_cassa(k);
                        ++a.casse_squarciate;
                        if (s->superficie) {   // gia' a galla danneggiato: la cella della cassa si allaga dal fondo
                            const auto& corpo = s->superficie->corpi().front();
                            for (std::size_t j = 0; j < corpo.celle.size(); ++j)
                                if (corpo.celle[j].dati.nome == b->casse()[static_cast<std::size_t>(k)].nome) {
                                    const auto& ce = corpo.celle[j];
                                    const auto& st = corpo.stazioni[static_cast<std::size_t>(ce.stazioni[ce.stazioni.size() / 2])];
                                    s->superficie->aggiungi_falla({static_cast<int>(j), st.x, std::clamp(0.0, ce.dati.y0, ce.dati.y1),
                                                                   st.z_chiglia + 0.5, 0.5, false, false});
                                }
                        }
                    }
                }
            }
            // 22 §9.5: falla piccola per colpo 5-10% da 100-130 mm, 1-2% col 76, niente col 30; il 16" sempre, e il
            // battello non si immerge piu'
            const double p = cal >= 400.0 ? 1.0 : cal >= 100.0 ? caso_.uniforme(0.05, 0.10) : cal >= 57.0 ? caso_.uniforme(0.01, 0.02) : 0.0;
            const double estratto = caso_.uniforme();
            if (estratto < p * f) {
                const double area = cal >= 400.0 ? 0.5 : cal >= 100.0 ? caso_.uniforme(0.005, 0.02) : caso_.uniforme(0.002, 0.005);
                if (b) b->aggiungi_falla(b->compartimento_a(x), area);
                if (cal >= 400.0) a.non_si_immerge = true;
            }
            break;
        }
        case ParteEsposta::nessuna:
            break;
    }
}

double DanniSottomarini::rumore_urto_db(int unita) const {
    const auto* s = sottomarino(unita);
    return s ? s->rumore_urto_db : 0.0;
}

void DanniSottomarini::appesantisci(const Missione& m, int unita, double tonnellate) {
    const auto it = m.unita().find(unita);
    if (it == m.unita().end() || !ha_fisica(it->second)) return;
    prendi(it->second, m.tempo()).danni->assetto(tonnellate / 1.025);
}

void DanniSottomarini::incendio(const Missione& m, int unita, int compartimento, double intensita, bool aria) {
    const auto it = m.unita().find(unita);
    if (it == m.unita().end() || !ha_fisica(it->second)) return;
    prendi(it->second, m.tempo()).danni->incendio(compartimento, intensita, aria);
}

void DanniSottomarini::esplosione_interna(const Missione& m, int unita, int compartimento, double tnt_kg) {
    const auto it = m.unita().find(unita);
    if (it == m.unita().end() || !ha_fisica(it->second)) return;
    prendi(it->second, m.tempo()).danni->esplosione_interna(compartimento, tnt_kg);
}

void DanniSottomarini::alleggerisci(int unita) {
    for (auto& s : sub_)
        if (s->unita == unita) s->danni->assetto(0.0);
}

const std::vector<EventoBattello>& DanniSottomarini::rapporti(int unita) const {
    static const std::vector<EventoBattello> nessuno;
    const auto* s = sottomarino(unita);
    return s ? s->danni->eventi() : nessuno;
}

ScossaBattello DanniSottomarini::scossa(int unita) const {
    ScossaBattello r;
    const auto* s = sottomarino(unita);
    if (!s) return r;
    for (const auto& e : s->danni->eventi())
        if (e.tipo == EventoBattello::Tipo::scossa) r = {e.tempo, e.intensita};
    return r;
}

const std::vector<CompartimentoBattello>* DanniSottomarini::compartimenti(int unita) const {
    const auto* s = sottomarino(unita);
    return s ? &s->danni->compartimenti() : nullptr;
}

std::optional<double> DanniSottomarini::rateo_quota(int unita) const {
    const auto* s = sottomarino(unita);
    if (!s || !s->quota_fisica || s->distrutto) return std::nullopt;
    return s->rateo_quota;
}

const SottomarinoColpito* DanniSottomarini::sottomarino(int unita) const {
    for (const auto& s : sub_) if (s->unita == unita) return s.get();
    return nullptr;
}

const std::vector<StatoSistema>* DanniSottomarini::sistemi(int unita) const {
    const auto* s = sottomarino(unita);
    return s ? &s->danni->sistemi() : nullptr;
}

LimitiBattello DanniSottomarini::limiti(int unita) const {
    const auto* s = sottomarino(unita);
    return s ? s->danni->limiti() : LimitiBattello{};
}

SottomarinoColpito& DanniSottomarini::prendi(const UnitaClassica& u, double tempo) {
    for (auto& s : sub_) if (s->unita == u.id) return *s;
    auto& sc = scafi_[u.classe];
    if (sc.stazioni.empty()) sc = DatiScafo::carica(dati_ + "/" + classi_.at(u.classe).scafo);
    auto s = std::make_unique<SottomarinoColpito>();
    s->unita = u.id;
    s->classe = u.classe;
    s->danni = std::make_unique<BattelloDanni>(sc, caso_.prossimo());
    s->danni->imposta_tempo(tempo);   // eventi e segni nel tempo di missione
    if (u.classe == "alfa") s->danni->senza_personale(true);   // dossier 19 §3.1-3.2
    if (u.giocatore && u.classe == "alfa") {   // le casse dell'Alfa (dossier 19): peso variabile e Tan
        s->casse_alfa = true;
        s->danni->abilita_compressione(COMPRESSIONE_ALFA_T_100M, u.quota);   // in assetto alla quota di partenza
    }
    sub_.push_back(std::move(s));
    return *sub_.back();
}

void DanniSottomarini::colpisci(SottomarinoColpito& s, const UnitaClassica& u, const Colpo& c) {
    // la testata e la sua distanza dallo scafo: dalla geometria vera dello scoppio quando c'e', altrimenti stimate per
    // arma in un punto scelto dal seme (la missione sa solo che il colpo e' a segno)
    auto& b = *s.danni;
    const auto& comp = b.compartimenti();
    const double x0 = comp.front().x1, x1 = comp.back().x0;
    double x = (x0 + x1) / 2 + caso_.uniforme(-0.3, 0.3) * (x0 - x1);
    double tnt = 0, distanza = 0, angolo = 0;
    bool cava = false;
    const auto& sc = scafi_.at(s.classe);
    switch (c.tipo) {
        case Colpo::Tipo::arma:
            if (c.classe_arma.find("mk48") != std::string::npos) {   // ADCAP: spoletta d'influenza sotto lo scafo
                tnt = TNT_MK48; distanza = caso_.uniforme(3.0, 8.0); angolo = PI / 2;
            } else if (c.classe_arma.find("mk46") != std::string::npos) {   // siluro leggero a contatto
                tnt = TNT_MK46; distanza = 0.5; angolo = 0.0;
            } else if (c.classe_arma.find("stingray") != std::string::npos) {   // carica cava a contatto
                tnt = TNT_STINGRAY; distanza = 0.5; angolo = 0.0; cava = true;
            } else if (c.classe_arma.find("at1") != std::string::npos) {
                tnt = TNT_AT1; distanza = 0.5; angolo = 0.0;
            } else {                                                 // siluri sovietici da 533 mm a contatto
                tnt = 300.0; distanza = 1.0; angolo = 0.0;
            }
            if (c.influenza && !c.geometria_vera) {   // mina a influenza (M2b) senza geometria: sotto lo scafo, a distanza
                distanza = caso_.uniforme(5.0, 15.0);
                angolo = PI / 2;
            }
            if (c.tnt_kg > 0) tnt = c.tnt_kg;   // la carica vera (mine dell'Enhanced)
            break;
        case Colpo::Tipo::attacco:   // carica di profondita' o razzo: a qualche decina di metri
            if (!c.riuscito) return;
            tnt = 135.0; distanza = caso_.uniforme(5.0, 25.0); angolo = caso_.uniforme(0.3, PI / 2);
            break;
        case Colpo::Tipo::attacco_casuale:   // missile: solo su un battello in superficie
            if (u.quota > 5.0) return;
            tnt = 220.0; distanza = 2.0; angolo = 0.0;
            break;
    }
    if (c.lanciatore >= 0) s.lanciatore = c.lanciatore;
    if (c.geometria_vera) {
        // siluro del modello dell'Enhanced, bomba o mina: lo scoppio nel riferimento della vittima (lungo l'asse verso
        // prua, di traverso, in quota) e la distanza dallo scafo come da un cilindro lungo quanto lo scafo, non dal
        // centro: la spoletta scatta a 10-15 m dal centro, che su un battello di 100 m e' spesso a contatto della poppa
        const double r = radianti(u.rotta), ex = c.pos.x - u.pos.x, ey = c.pos.y - u.pos.y;
        double lungo = ex * std::sin(r) + ey * std::cos(r), traverso = ex * std::cos(r) - ey * std::sin(r);
        // la posizione dello scoppio vale se torna con la distanza registrata (i colpi senza: tutta di traverso)
        if (std::fabs(std::sqrt(ex * ex + ey * ey + c.dz_m * c.dz_m) - c.distanza_m) > 1.0) {
            lungo = 0.0;
            traverso = std::sqrt(std::max(0.0, c.distanza_m * c.distanza_m - c.dz_m * c.dz_m));
        }
        const double fuori = std::max(0.0, std::fabs(lungo) - sc.lunghezza / 2);
        const double radiale = std::hypot(traverso, c.dz_m);
        distanza = std::max(std::hypot(fuori, std::max(0.0, radiale - sc.larghezza / 2)), 0.5);
        angolo = std::asin(std::min(1.0, std::fabs(c.dz_m) / std::max(radiale, 0.1)));
        x = std::clamp(lungo, x1, x0);
    }
    // doppio scafo: l'intercapedine fra lo scafo leggero e quello resistente (scafi/<classe>.toml: 1-1,2 m sui sovietici,
    // 0 sui 688) allontana lo scoppio dallo scafo resistente
    if (!cava) distanza += sc.sottomarino.intercapedine_m;
    b.urto(tnt, distanza, angolo, x, u.quota);
    if (cava) {   // il getto della carica cava nello scafo resistente
        const int k = b.compartimento_a(x);
        b.aggiungi_falla(k, FALLA_CAVA_M2);
        b.incendio(k, 0.3);
    }
}

void DanniSottomarini::annuncia(const Missione& m, int unita, int codice, bool solo_giocatore) {
    const auto* g = m.giocatore();
    if (!g) return;
    if (solo_giocatore ? g->id != unita : (g->id == unita || distanza(g->pos, m.unita().at(unita).pos) > ASCOLTO_M)) return;
    for (const auto& x : messaggi_)   // un solo messaggio dello stesso tipo per istante
        if (x.codice == codice && x.unita == unita && x.tempo == m.tempo()) return;
    messaggi_.push_back({m.tempo(), unita, codice});
}

void DanniSottomarini::avanza(Missione& m, double dt) {
    tempo_ = m.tempo();
    for (const auto& [id, u] : m.unita())
        if (ha_fisica(u) && !m.alla_fisica(id)) m.affida_alla_fisica(id);
    if (!iniziali_fatti_) {
        iniziali_fatti_ = true;
        danni_iniziali(m);
    }
    if (const auto* g = m.giocatore(); g && g->classe == "alfa" && ha_fisica(*g)) {   // l'Alfa: le casse dal primo passo
        auto& s = prendi(*g, m.tempo());
        if (!s.distrutto) pesa_lanci(m, s);
    }
    // colpi nuovi sui sottomarini
    const auto& colpi = m.colpi();
    for (; colpi_visti_ < colpi.size(); ++colpi_visti_) {
        const Colpo& c = colpi[colpi_visti_];
        const auto it = m.unita().find(c.vittima);
        if (it == m.unita().end() || !ha_fisica(it->second)) continue;
        auto& s = prendi(it->second, m.tempo());
        if (!s.distrutto) colpisci(s, it->second, c);
    }
    // collisioni (nucleo/collisioni.hpp): gli urti rilevati dalla missione, gia' valutati
    const auto& urti = m.urti();
    for (; urti_visti_ < urti.size(); ++urti_visti_) collisione(m, urti[urti_visti_].urto, urti[urti_visti_].esito);
    // artiglieria (G3): colpi diretti e schegge sulle parti esposte
    const auto& impatti = m.impatti_artiglieria();
    for (; impatti_visti_ < impatti.size(); ++impatti_visti_) {
        const ImpattoArtiglieria& i = impatti[impatti_visti_];
        colpo_artiglieria(m, i);
        // i colpi diretti si sommano: consumano l'integrita', e l'ultimo manda il battello fuori combattimento, anche il
        // giocatore e il comandante IA. Per colpo sullo scafo, sulla vela o sulla plancia: 0,2 col 57-76 mm, 1 coi 100-130,
        // 5 col 16", niente col 30, che non finisce nessuno (STIMA, principale, 04/11/2026: 1252 Mk 75 a segno non fermavano un Victor)
        const bool struttura = i.parte == ParteEsposta::scafo || i.parte == ParteEsposta::falsatorre || i.parte == ParteEsposta::plancia;
        if (i.avvertimento || i.esito != ImpattoArtiglieria::Esito::diretto || !struttura) continue;
        UnitaClassica* u = m.trova(i.bersaglio);
        if (!u || u->categoria != "sottomarino" || u->distrutta || u->persa) continue;
        const int colpi = artiglieria_[i.bersaglio].colpi;
        const int usura = i.calibro_mm >= 400.0 ? 5 : i.calibro_mm >= 100.0 ? 1 : i.calibro_mm >= 57.0 ? (colpi % 5 == 0 ? 1 : 0) : 0;
        if (usura == 0) continue;
        u->integrita = std::max(0, u->integrita - usura);
        u->lanciatore = i.tiratore;   // a chi va il battello, se e' l'ultimo
        if (u->integrita == 0) m.distruggi(i.bersaglio);
    }
    // sistemi rotti dalla missione (il periscopio oltre la velocita' di danno)
    const auto& guasti = m.guasti_richiesti();
    for (; guasti_visti_ < guasti.size(); ++guasti_visti_) {
        const GuastoRichiesto& r = guasti[guasti_visti_];
        const auto it = m.unita().find(r.unita);
        if (it == m.unita().end() || !ha_fisica(it->second) || r.sistema < 0 || r.sistema >= static_cast<int>(Sistema::numero)) continue;
        auto& s = prendi(it->second, m.tempo());
        if (!s.distrutto) s.danni->guasto_sistema(static_cast<Sistema>(r.sistema), r.perdita, r.testo);
    }
    for (auto& p : sub_) {
        auto& s = *p;
        const auto it = m.unita().find(s.unita);
        if (it == m.unita().end() || it->second.distrutta || it->second.persa) {
            // tolta dalla missione (urto col fondo, fine dell'affondamento): i segni restano dove era
            if (!s.distrutto && (s.danni->acqua_imbarcata_m3() > 1.0 || s.danni->perso())) lascia_segni(s, m.tempo());
            s.distrutto = true;
            continue;
        }
        const UnitaClassica& u = it->second;
        auto& b = *s.danni;
        // il carico vivo del deposito dell'IA (la dotazione che si vuota coi lanci); il giocatore dalle stazioni
        if (!u.giocatore)
            if (const auto kg = m.esplosivo_ia(s.unita)) applica_carico(s, *kg);
        // la canna del periscopio col vetro rotto, aperta, imbarca acqua nella centrale sotto la vela (runtime: G3)
        if (const double q = m.allagamento_periscopio_m3_s(s.unita); q > 0.0 && dt > 0.0)
            b.imbarca(b.compartimento_di(Sistema::periscopio), q * dt);
        // emersione violenta (E1): un battello dell'IA che arriva in superficie risalendo a piu' di 1 m/s
        double salita = s.quota_prec >= 0.0 && dt > 0 ? (s.quota_prec - u.quota) / dt : 0.0, assetto = 15.0;
        if (const auto* d = m.dinamica(s.unita)) { salita = -d->rateo_quota_m_s(); assetto = d->assetto_deg(); }
        s.salita = 0.7 * s.salita + 0.3 * salita;
        const bool decide_ia = !u.giocatore || m.sandbox();   // anche il comandante IA, che ha giocatore = true e non ha Ctrl+E
        bool casse = false;   // solo dopo un esaurimento d'emergenza, non per un'emersione normale degli script
        if (decide_ia && s.emersione < 0 && u.quota <= 0.5 && s.quota_prec > 0.5 && s.salita > 1.0)
            for (const auto& ev : b.eventi()) casse |= ev.tipo == EventoBattello::Tipo::esaurimento;
        if (casse) {
            const auto& sc = scafi_.at(s.classe);
            EmersioneViolenta e;
            e.unita = s.unita; e.inizio = m.tempo(); e.pos = u.pos; e.rotta = u.rotta;
            // velocita' sul fondo dallo spostamento vero (il moto classico non va a u.velocita * NODO)
            e.velocita = dt > 0 && s.quota_prec >= 0.0 ? distanza(u.pos, s.pos) / dt : u.velocita * NODO;
            e.salita = s.salita; e.lunghezza = sc.lunghezza;
            e.angolo = std::clamp(std::max(assetto, 5.0) + 2.5 * s.salita, 10.0, 35.0);
            e.fuori = std::clamp(0.08 + 0.04 * s.salita + 0.006 * e.velocita, 0.12, 0.40);
            e.salita_s = std::clamp(1.0 + 0.4 * s.salita, 1.5, 4.0);
            e.caduta_s = 1.4 * std::sqrt(2 * std::max(e.altezza_prua(), 0.5) / 9.81);
            s.emersione = static_cast<int>(emersioni_.size());
            emersioni_.push_back(e);
        }
        s.quota_prec = u.quota;
        s.pos = u.pos;
        s.quota = u.quota;
        const auto forze = b.avanza(dt, {u.quota, u.velocita * NODO});   // velocita' vere, non quelle del moto classico
        // l'IA tenta l'emersione d'emergenza dopo un allagamento, sopra i 150 m, nel 30-70% dei casi
        if (decide_ia && !s.emersione_tentata && b.acqua_imbarcata_m3() > 1.0) {
            s.emersione_tentata = true;
            if (u.quota < QUOTA_EMERSIONE_IA && caso_.uniforme() < caso_.uniforme(0.3, 0.7)) b.esaurisci();
        }
        // ...e dopo 5 minuti di incendio fuori controllo o di fumo denso, a qualunque quota, per ventilare
        // (Komsomolets 1989, Bonefish 1988)
        double fuoco = 0, fumo = 0;
        bool sventrato = false;
        for (const auto& c : b.compartimenti()) {
            fuoco = std::max(fuoco, c.fuoco);
            fumo = std::max(fumo, c.fumo);
            sventrato |= c.distrutto;
        }
        s.incendio_forte = fuoco > 0.6 || fumo > 0.8 ? s.incendio_forte + dt : 0.0;
        if (decide_ia && s.incendio_forte > 300.0 && !b.esaurimento() && u.quota > 1.0 && b.aria_bombole_Nm3() > 0) b.esaurisci();
        // quota dalla fisica quando il peso netto conta; piani nel verso della quota ordinata; limiti dei sistemi
        OrdineMotore o = u.ordine;
        o.fino_a = m.tempo() + 2.0;
        const LimitiBattello l = b.limiti();
        if (u.giocatore) {
            m.inerziale_giocatore(l.inerziale);   // la navigazione incerta: a inerziale giu', a stima
            m.schiera_giocatore(l.schiera);       // la schiera guasta non ascolta, anche se filata
        } else {
            m.inerziale_ia(u.id, l.inerziale);    // il navigatore dell'IA, con la stessa regola
            m.sonar_ia(u.id, l.sonar, l.schiera); // e cupola e schiera guaste, come quelle del giocatore
        }
        // limiti().velocita e' la frazione della massima (MAX = avanti tutta x 280/256)
        o.velocita_max_kn = l.velocita < 1.0 ? l.velocita * u.avanti_tutta * 280.0 / 256.0 : -1.0;
        s.quota_fisica = std::fabs(forze.peso_netto_N) > PESO_SIGNIFICATIVO || b.perso();
        if (s.quota_fisica) {
            const int piani = !l.idroplani ? 0 : u.quota_ordinata < u.quota - 1.0 ? +1 : u.quota_ordinata > u.quota + 1.0 ? -1 : 0;
            s.rateo_quota = rateo_verticale_regime(forze.peso_netto_N, b.area_pianta_m2(), u.velocita * NODO, piani);
            if (u.quota <= 0.0 && s.rateo_quota < 0.0) s.rateo_quota = 0.0;   // in superficie: galleggia
        }
        o.quota_fisica = s.quota_fisica && !b.perso();   // dopo il collasso il relitto lo muove l'affondamento
        o.rateo_quota_fisica = s.rateo_quota;
        // per la dinamica dell'Enhanced (C6a): forze e governi in avaria
        o.peso_netto_N = forze.peso_netto_N;
        o.momento_assetto_Nm = forze.momento_assetto_Nm;
        o.timone = l.timone;
        o.idroplani = l.idroplani;
        // esiti (D6e): emerso danneggiato, fermo in superficie
        if (u.quota <= 0.5 && (b.acqua_imbarcata_m3() > 1.0 || fuoco > 0.0 || fumo > 0.3 || sventrato)) s.emerso_danneggiato = true;
        if (s.emerso_danneggiato && decide_ia)
            o.velocita_max_kn = o.velocita_max_kn < 0.0 ? VELOCITA_EMERSO_KN : std::min(o.velocita_max_kn, VELOCITA_EMERSO_KN);
        // emerso danneggiato: la fisica delle navi, in superficie finche' non affonda
        if (s.emerso_danneggiato && !s.superficie && !b.perso()) in_superficie(s);
        bool affondato_in_superficie = false;
        if (s.superficie) {
            // falle nuove del modello d'immersione (tenute bruciate, esplosioni): anche alla fisica delle navi
            const auto& corpo = s.superficie->corpi().front();
            for (; s.falle_passate < b.falle().size(); ++s.falle_passate) {
                const auto& f = b.falle()[s.falle_passate];
                const auto& nome = b.compartimenti()[static_cast<std::size_t>(f.compartimento)].nome;
                for (std::size_t j = 0; j < corpo.celle.size(); ++j)
                    if (corpo.celle[j].dati.nome == nome) {
                        const auto& ce = corpo.celle[j];
                        const auto& st = corpo.stazioni[static_cast<std::size_t>(ce.stazioni[ce.stazioni.size() / 2])];
                        s.superficie->aggiungi_falla({static_cast<int>(j), st.x, std::clamp(0.0, ce.dati.y0, ce.dati.y1),
                                                      st.z_chiglia + 0.5, f.area, false, false});
                    }
            }
            s.superficie->avanza(dt);
            o.quota_fisica = true;
            o.rateo_quota_fisica = u.quota > 0.0 ? -0.5 : 0.0;   // a galla
            o.peso_netto_N = o.momento_assetto_Nm = 0.0;
            affondato_in_superficie = s.superficie->affondata();
        }
        m.ordina(s.unita, o);
        // sul fondo e pesante: l'IA esaurisce le casse; con l'aria finita, dopo 10 minuti il battello e' perso
        const bool pesante_sul_fondo = u.sul_fondo && forze.peso_netto_N > PESO_SIGNIFICATIVO;
        s.sul_fondo_pesante = pesante_sul_fondo ? s.sul_fondo_pesante + dt : 0.0;
        if (pesante_sul_fondo && decide_ia && !b.esaurimento() && b.aria_bombole_Nm3() > 0) b.esaurisci();
        if (pesante_sul_fondo && u.giocatore && !s.fondo_annunciato) {
            s.fondo_annunciato = true;
            annuncia(m, s.unita, msg_sottomarino::FERMI_SUL_FONDO, true);
        }
        // tutti i compartimenti allagati (l'aria che resta e' quella compressa dal mare): nessuno a bordo si salva
        bool allagato = true;
        for (const auto& c : b.compartimenti()) allagato &= c.imploso || c.aria() < 0.1 * c.volume;
        const bool sul_fondo_perso = (s.sul_fondo_pesante > FONDO_SENZA_SCAMPO && b.aria_bombole_Nm3() <= 0.0) || allagato ||
                                     affondato_in_superficie;
        // fuori combattimento (solo l'IA): integrita' dell'originale finita e battello menomato. L'ultimo colpo di cannone
        // che consuma integrita' vale per tutti, sopra, nel giro degli impatti
        const bool fuori_combattimento = !u.giocatore && u.integrita <= 0 && menomato(s);
        if ((b.perso() || sul_fondo_perso || fuori_combattimento) && !s.distrutto && u.affondamento == 0) {
            m.distruggi(s.unita);
            if (b.perso() || sul_fondo_perso) lascia_segni(s, m.tempo());
        }
        // messaggi
        for (; s.eventi_visti < b.eventi().size(); ++s.eventi_visti) {
            const auto& e = b.eventi()[s.eventi_visti];
            using T = EventoBattello::Tipo;
            int codice = -1;
            switch (e.tipo) {
                case T::allagamento: codice = msg_sottomarino::ALLAGAMENTO; break;
                case T::guasto: codice = msg_sottomarino::GUASTO; break;
                case T::scram: codice = msg_sottomarino::SCRAM; break;
                case T::riparazione:
                    codice = e.sistema < 0 ? msg_sottomarino::REATTORE : msg_sottomarino::RIPARATO;
                    break;
                case T::falla_tamponata: codice = msg_sottomarino::TAMPONATA; break;
                case T::paratia_ceduta: codice = msg_sottomarino::PARATIA; break;
                case T::esaurimento:
                    codice = msg_sottomarino::ESAURIMENTO;
                    annuncia(m, s.unita, msg_sottomarino::EMERSIONE_NEMICA, false);   // il sonar sente l'aria
                    break;
                case T::aria_finita: codice = msg_sottomarino::ARIA_FINITA; break;
                case T::collasso:
                    if (e.compartimento >= 0) codice = msg_sottomarino::ROTTURA;   // rottura dello scafo resistente
                    break;
                case T::incendio: codice = msg_sottomarino::INCENDIO; break;
                case T::esplosione:
                    codice = msg_sottomarino::ESPLOSIONE;
                    annuncia(m, s.unita, msg_sottomarino::ESPLOSIONE_NEMICA, false);   // il sonar sente lo scoppio
                    break;
                case T::incendio_spento: codice = msg_sottomarino::INCENDIO_SPENTO; break;
                case T::cloro: codice = msg_sottomarino::CLORO; break;
                case T::aria_nell_incendio: codice = msg_sottomarino::ARIA_NELL_INCENDIO; break;
                case T::sgombero: codice = MSG_SGOMBERO; break;
                case T::isolamento: codice = MSG_ISOLATO; break;
                case T::riapertura: codice = MSG_RIAPERTO; break;
                case T::caduti: codice = MSG_UOMINI_PERDUTI; break;
                default: break;
            }
            if (codice >= 0) annuncia(m, s.unita, codice, true);
            // sottomarini nemici: cio' che il sonar del giocatore sente
            if ((e.tipo == T::collasso || (e.tipo == T::paratia_ceduta)) && !s.rotto_annunciato) {
                s.rotto_annunciato = true;
                annuncia(m, s.unita, msg_sottomarino::RUMORI_ROTTURA, false);
            }
            if (e.tipo == T::implosione && !s.implosioni_annunciate) {
                s.implosioni_annunciate = true;
                annuncia(m, s.unita, msg_sottomarino::IMPLOSIONI, false);
            }
        }
        if (b.perso() || sul_fondo_perso) s.distrutto = true;
    }
}

void DanniSottomarini::in_superficie(SottomarinoColpito& s) {
    s.superficie = std::make_unique<DanniNave>(scafi_.at(s.classe), caso_.prossimo());
    auto& d = *s.superficie;
    const auto& corpo = d.corpi().front();
    auto indice = [&](const std::string& nome) {
        for (std::size_t j = 0; j < corpo.celle.size(); ++j) if (corpo.celle[j].dati.nome == nome) return static_cast<int>(j);
        return -1;
    };
    auto falla_sul_fondo = [&](int j, double area) {   // nella stazione di mezzo della cella, mezzo metro sopra la chiglia
        const auto& ce = corpo.celle[static_cast<std::size_t>(j)];
        const auto& st = corpo.stazioni[static_cast<std::size_t>(ce.stazioni[ce.stazioni.size() / 2])];
        d.aggiungi_falla({j, st.x, std::clamp(0.0, ce.dati.y0, ce.dati.y1), st.z_chiglia + 0.5, area, false, false});
    };
    const auto& b = *s.danni;
    for (const auto& c : b.compartimenti())   // acqua dei compartimenti: in m^3; il fuoco e il carico vivo (fase 4)
        if (const int j = indice(c.nome); j >= 0) {
            d.imposta_acqua(static_cast<std::size_t>(j), c.acqua);
            d.imposta_fuoco(static_cast<std::size_t>(j), c.fuoco);
            if (c.uso == "deposito") d.imposta_carico(static_cast<std::size_t>(j), c.carica_kg, c.propellente_kg);
        }
    for (const auto& k : b.casse())           // casse: la stessa frazione piena (in superficie l'aria e' a 1 bar)
        if (const int j = indice(k.nome); j >= 0) {
            const auto& ce = corpo.celle[static_cast<std::size_t>(j)];
            const double piena = k.volume > 0 ? std::clamp(1.0 - k.aria_Nm3 / k.volume, 0.0, 1.0) : 0.0;
            d.imposta_acqua(static_cast<std::size_t>(j), piena * ce.volume * ce.dati.permeabilita);
            if (k.squarciata) falla_sul_fondo(j, 0.5);
        }
    for (const auto& f : b.falle())
        if (f.area > 0)
            if (const int j = indice(b.compartimenti()[static_cast<std::size_t>(f.compartimento)].nome); j >= 0) falla_sul_fondo(j, f.area);
    s.falle_passate = b.falle().size();
}

void DanniSottomarini::sincronizza(Mondo& mondo) const {
    for (const auto& s : sub_) {
        if (s->distrutto) continue;
        const EmersioneViolenta* e = s->emersione >= 0 ? &emersioni_[static_cast<std::size_t>(s->emersione)] : nullptr;
        const bool salto = e && tempo_ <= e->fine();
        if (!salto && !s->superficie) continue;
        for (auto& n : mondo.navi())
            if (n.id == s->unita) n.posa = salto ? posa_emersione(*e, tempo_) : s->superficie->posa();
    }
}

bool DanniSottomarini::menomato(const SottomarinoColpito& s) {
    const auto& b = *s.danni;
    double volume = 0;
    for (const auto& c : b.compartimenti()) volume += c.volume;
    return s.emerso_danneggiato || b.limiti().velocita < 0.3 || b.acqua_imbarcata_m3() > 0.1 * volume;
}

void DanniSottomarini::lascia_segni(SottomarinoColpito& s, double tempo) {
    if (s.segni_fatti) return;
    s.segni_fatti = true;
    // gasolio dei diesel: chiazza piu' grande e piu' lunga (dossier 07 §8.3)
    bool nucleare = false;
    for (const auto& c : s.danni->compartimenti()) nucleare |= c.uso == "reattore";
    const double k = nucleare ? 1.0 : 1.5, ore = 3600.0;
    using T = SegnoSuperficie::Tipo;
    if (s.quota > ROTTURA_PROFONDA) {   // rottura profonda: nafta dopo 10-60 minuti, rottami scarsi
        const double inizio = tempo + caso_.uniforme(600.0, 3600.0);
        segni_.push_back({T::nafta, s.unita, s.pos, inizio, 6 * ore * k, std::min(1.0, 0.4 * k)});
        segni_.push_back({T::rottami, s.unita, s.pos, inizio, ore, 0.15});
    } else {                            // vicino alla superficie o sul fondo: aria, nafta e rottami abbondanti subito
        segni_.push_back({T::aria, s.unita, s.pos, tempo, 300.0, 1.0});
        segni_.push_back({T::nafta, s.unita, s.pos, tempo, 6 * ore * k, std::min(1.0, 0.7 * k)});
        segni_.push_back({T::rottami, s.unita, s.pos, tempo, ore, 0.6});
    }
}

}  // namespace m688
