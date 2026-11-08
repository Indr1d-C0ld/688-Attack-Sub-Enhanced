// Le luci di navigazione e di fonda nella missione dell'Enhanced (docs/27_luci_navigazione.md, approvato dall'utente il
// 29/10/2026), sopra il modulo puro del kit (nucleo/luci_navigazione: i fanali di ogni classe, i settori del COLREG, il
// faro ambra dei sottomarini USN, lo stato predefinito):
// - lo stato delle luci di ogni unita': la chiave `luci` dello scenario, poi «senza governo» per una nave ferma per danni
//   (avaria o incagliata), poi per il giocatore il suo ordine, altrimenti lo stato predefinito dal tipo, dalla velocita' e
//   dalla guerra fra la sua parte e le altre (le militari e i sottomarini spengono in guerra);
// - l'ordine `luci_giocatore` (a = 1 accese, 0 spente): fanali e faro ambra del 688, o fanali dell'Alfa; ogni cambio va
//   nel registro (evento `luci`);
// - la vedetta, per la simmetria: di notte una nave illuminata si vede fino alla portata dei suoi fanali con la foschia,
//   e il giocatore illuminato in superficie si fa vedere dalle navi e dagli aerei dell'altra parte, con la legge dei
//   fanali del disassemblatore (nucleo/portata_luci).
// Senza luci_enhanced (il classico) non succede niente: nessuna unita' ha luci.

#include <algorithm>
#include <cmath>
#include <filesystem>

#include "missioni/missione.hpp"
#include "nucleo/portata_luci.hpp"
#include "nucleo/sagoma_notte.hpp"

namespace m688 {

namespace {

constexpr double NOTTE = 0.2;                  // luce sotto cui e' notte (come l'artiglieria e il libretto)
constexpr double OCCHIO_NAVE_M = 20.0;         // vedette sul ponte (come missione_periscopi)
constexpr double OCCHIO_PERISCOPIO_M = 1.5;    // la testa del periscopio alzato

bool arma(const UnitaClassica& u) { return (u.flag & 0x20) != 0 || u.categoria == "arma"; }
bool stesso_lato(const UnitaClassica& a, const UnitaClassica& b) { return ((a.flag ^ b.flag) & 0x10) == 0; }

}  // namespace

// Idempotente: il file si legge una volta (DanniMissione la chiama a ogni passo, come gli altri *_enhanced). Senza
// luci.toml (prove senza i dati) niente luci, come nel classico
void Missione::luci_enhanced(const std::string& cartella_dati) {
    if (luci_attive_) return;
    const std::string f = cartella_dati + "/luci.toml";
    if (!std::filesystem::exists(f)) return;
    luci_conf_ = luci::carica(f);
    luci_attive_ = true;
}

void Missione::leggi_luci_unita(int id, const std::string& nome) {
    if (nome.empty()) return;
    bool ok = false;
    const luci::Stato s = luci::stato_da_nome(nome, &ok);
    if (ok) luci_chiave_[id] = s;
}

luci::Stato Missione::stato_luci(int id) const {
    if (!luci_attive_) return luci::Stato::spente;
    const auto it = unita_.find(id);
    if (it == unita_.end()) return luci::Stato::spente;
    const UnitaClassica& u = it->second;
    if (u.distrutta || u.persa || arma(u)) return luci::Stato::spente;
    const auto conf = luci_conf_.find(u.classe);
    if (conf == luci_conf_.end()) return luci::Stato::spente;   // aerei e classi senza fanali
    const double v = u.velocita * NODO;
    if (u.giocatore && luci_giocatore_) return *luci_giocatore_ ? luci::stato_predefinito(conf->second.tipo, v, false, false) : luci::Stato::spente;
    if (const auto k = luci_chiave_.find(id); k != luci_chiave_.end()) return k->second;
    if (u.incagliata || in_avaria(id)) return luci::Stato::non_governa;
    // in guerra con almeno un'altra parte (USA, URSS, luogo): i neutrali non sono mai in guerra
    const int lato = u.flag & 0x18;
    bool guerra = false;
    if (lato != 0)
        for (const int altro : {0x08, 0x10, 0x18})
            if (altro != lato && situazione_fra(lato, altro) == Situazione::guerra) guerra = true;
    return luci::stato_predefinito(conf->second.tipo, v, guerra, false);
}

bool Missione::luci_giocatore_accese() const {
    const auto* g = giocatore();
    return g && stato_luci(g->id) != luci::Stato::spente;
}

bool Missione::ordine_luci(const Ordine& o) {
    const auto* g = giocatore();
    if (!luci_attive_ || !g) return false;
    const luci::Stato prima = stato_luci(g->id);
    luci_giocatore_ = o.a != 0.0;
    const luci::Stato dopo = stato_luci(g->id);
    if (dopo != prima) cambi_luci_.push_back({tempo_, g->id, dopo});
    return true;
}

// Le condizioni della vista nel punto dato (come la testa del giocatore in missione_periscopi)
CondizioniVista Missione::condizioni_luci(Vec2 p) const {
    const StatoMeteo qui = meteo_ascolto(p);
    CondizioniVista c;
    c.stato_mare = qui.stato_mare();
    c.luce = luce_periscopi();
    c.visibilita_m = qui.visibilita_m;
    c.nubi_ottavi = qui.nubi_ottavi;
    c.nebbia_radiazione = qui.nebbia == Nebbia::radiazione;
    return c;
}

// Il cielo della notte in `p` per le sagome: con la data della missione (data e ora_utc) e il fondale georeferenziato
// (GEO1) quello di nucleo/sagoma_notte con la Luna vera (fase, altezza, estinzione e controluce) nel punto, all'ora della
// partita; senza, quello di prima dalla luce del Sole e dalle nubi, senza Luna (principale, 30/10/2026). Lo stesso
// cielo va alla vista del periscopio (prepara_vista), cosi' il giocatore vede quello che vede l'IA
CieloNotte Missione::cielo_notte_in(Vec2 p) const {
    const CondizioniVista c = condizioni_luci(p);
    if ((data_dal_file_ || data_imposta_) && fondale_ && fondale_->georeferenziato()) {
        const Geografiche g = fondale_->geografiche(p);
        // oltre la mezzanotte il giorno dopo (la Luna si sposta di 12 gradi al giorno): le formule sono lineari nel giorno
        const double s = ora_utc_s_ + tempo_, giorni = std::floor(s / 86400.0);
        const DataOra t{anno_, mese_data_, giorno_ + static_cast<int>(giorni), (s - giorni * 86400.0) / 3600.0};
        return cielo_notte(t, g.lat, g.lon, c.nubi_ottavi);
    }
    return cielo_notte(c.luce, c.nubi_ottavi);
}

// La portata dei fanali d'un'unita' per un osservatore (nucleo/portata_luci del disassemblatore: Allard con la foschia,
// la luce, l'allerta e il binocolo, mai oltre l'orizzonte del fanale); 0 senza luci accese verso di lui
double Missione::portata_luci(int id, Vec2 da, const Osservatore& o) const {
    const luci::Stato s = stato_luci(id);
    if (s == luci::Stato::spente) return 0.0;
    const UnitaClassica& u = unita_.at(id);
    const double relativo = normalizza_180(rilevamento(u.pos, da) - u.rotta);
    return luci::vista_luci(luci_conf_.at(u.classe), s, relativo, o, condizioni_luci(u.pos), luci::GUADAGNO_BINOCOLO).portata_m;
}

// La portata di notte d'un'unita' per un osservatore in `da`: il massimo fra le luci accese verso di lui e la sagoma contro il
// cielo (nucleo/sagoma_notte del disassemblatore, col binocolo; la Luna con la data e il GEO1, cielo_notte_in). Al
// posto dei 3 km fissi di NAVE_VISTA_NOTTE_M (periscopio dell'IA) e PORTATA_NOTTE_M (libretto dell'IA). Negativa per una
// classe senza configurazione (aerei, armi): resta la portata di prima
double Missione::portata_notte(int osservato, Vec2 da, const Osservatore& o) const {
    const UnitaClassica& u = unita_.at(osservato);
    const auto conf = luci_conf_.find(u.classe);
    if (!luci_attive_ || conf == luci_conf_.end()) return -1.0;
    const CondizioniVista c = condizioni_luci(da);
    const double aspetto = normalizza_180(rilevamento(u.pos, da) - u.rotta);
    const double sagoma = portata_sagoma_notte_m(sagoma_da_tipo(conf->second.tipo, conf->second.lunghezza_m), aspetto, o, c,
                                                 cielo_notte_in(da), rilevamento(da, u.pos), INGRANDIMENTO_BINOCOLO);
    return std::max(sagoma, portata_luci(osservato, da, o));
}

// La portata a vista d'un'unita' per un osservatore in `da`, a ogni luce (nucleo/sagoma_notte vale dal buio al pieno giorno,
// con la foschia e il tetto del contrasto; disassemblatore, 29/10/2026): la sagoma dalla configurazione delle luci della
// classe, se c'e', se no dalla categoria e dalla lunghezza di classi.toml; di notte il massimo coi fanali accesi. Al posto
// delle portate fisse di giorno e di notte del periscopio e del libretto dell'IA. Negativa per chi non ha sagoma (aerei,
// armi)
double Missione::portata_vista(int osservato, Vec2 da, const Osservatore& o, double ingrandimento) const {
    const double sagoma = portata_sagoma(osservato, da, o, ingrandimento);
    if (sagoma < 0.0) return sagoma;
    const UnitaClassica& u = unita_.at(osservato);
    if (luce_periscopi() < NOTTE && luci_attive_ && luci_conf_.count(u.classe)) return std::max(sagoma, portata_luci(osservato, da, o));
    return sagoma;
}

// la sagoma sola (portata_vista senza i fanali): la classe si riconosce solo da qui (passo_libretto_ia)
double Missione::portata_sagoma(int osservato, Vec2 da, const Osservatore& o, double ingrandimento) const {
    const UnitaClassica& u = unita_.at(osservato);
    if (u.categoria != "nave" && u.categoria != "sottomarino") return -1.0;
    std::string tipo;
    double lunghezza = 0.0;
    const auto conf = luci_conf_.find(u.classe);
    if (conf != luci_conf_.end()) {
        tipo = conf->second.tipo;
        lunghezza = conf->second.lunghezza_m;
    } else {
        tipo = u.categoria == "sottomarino" ? "sottomarino" : classe_civile(u.classe) ? "mercantile" : "militare";
        if (const auto d = dimensioni_classi_.find(u.classe); d != dimensioni_classi_.end()) lunghezza = d->second.lunghezza_m;
    }
    if (lunghezza <= 0.0) lunghezza = u.categoria == "sottomarino" ? 100.0 : 130.0;
    const CondizioniVista c = condizioni_luci(da);
    const double aspetto = normalizza_180(rilevamento(u.pos, da) - u.rotta);
    return portata_sagoma_notte_m(sagoma_da_tipo(tipo, lunghezza), aspetto, o, c, cielo_notte_in(da), rilevamento(da, u.pos),
                                  ingrandimento);
}

// I fanali che arrivano all'osservatore (accesi, nel settore verso di lui, entro la loro portata col binocolo) e quello
// che dicono, coi ruoli di luci.toml (COLREG 1972):
// - lo stato: i fanali da pesca, di fonda, di governo (i due rossi), di manovra limitata (rosso-bianco-rosso), se no
//   quelli di via (albero, laterali, coronamento) per «in moto»;
// - il tipo: peschereccio dai fanali da pesca, sottomarino dal faro ambra USN;
// - l'aspetto: rosso e verde insieme di prua (sigma 5 gradi); il solo rosso dal lato sinistro, il solo verde dal dritto
//   (sigma 30, dentro i 112,5 gradi dei laterali); il solo coronamento di poppa (sigma 20). STIMA
Missione::LetturaFanali Missione::leggi_fanali(int osservato, Vec2 da, const Osservatore& o) const {
    LetturaFanali l;
    const luci::Stato s = stato_luci(osservato);
    if (s == luci::Stato::spente) return l;
    const UnitaClassica& u = unita_.at(osservato);
    const auto& conf = luci_conf_.at(u.classe);
    const CondizioniVista c = condizioni_luci(u.pos);
    const double relativo = normalizza_180(rilevamento(u.pos, da) - u.rotta), d = distanza(u.pos, da);
    bool pesca = false, fonda = false, governo = false, manovra = false, via = false, rosso = false, verde = false, poppa = false, ambra = false;
    for (const luci::Fanale* f : luci::accesi(conf, s, tempo_)) {
        if (f->ruolo == "coperta" || !luci::nel_settore(*f, relativo)) continue;
        if (luci::portata_fanale_m(*f, o, c, luci::GUADAGNO_BINOCOLO) < d) continue;
        ++l.visti;
        const std::string& r = f->ruolo;
        pesca = pesca || r.rfind("pesca", 0) == 0 || r == "albero_pesca";
        fonda = fonda || r.rfind("fonda", 0) == 0;
        governo = governo || r.rfind("governo", 0) == 0;
        manovra = manovra || r.rfind("manovra", 0) == 0;
        rosso = rosso || r == "laterale_sinistro";
        verde = verde || r == "laterale_dritto";
        poppa = poppa || r == "coronamento";
        ambra = ambra || r == "faro_ambra";
        via = via || r == "albero" || r == "albero_poppiero" || r == "laterale_sinistro" || r == "laterale_dritto" || r == "coronamento";
    }
    if (l.visti == 0) return l;
    if (pesca) l.stato = luci::Stato::pesca;
    else if (governo) l.stato = luci::Stato::non_governa;
    else if (manovra) l.stato = luci::Stato::manovra_limitata;
    else if (fonda) l.stato = luci::Stato::fonda;
    else if (via) l.stato = luci::Stato::moto;
    if (pesca) l.tipo = "peschereccio";
    else if (ambra) l.tipo = "sottomarino";
    if (rosso && verde) { l.aspetto = 0.0; l.sigma_aspetto = 5.0; }
    else if (rosso) { l.aspetto = -56.25; l.sigma_aspetto = 30.0; }
    else if (verde) { l.aspetto = 56.25; l.sigma_aspetto = 30.0; }
    else if (poppa) { l.aspetto = 180.0; l.sigma_aspetto = 20.0; }
    return l;
}

const Missione::FanaliVisti* Missione::fanali_visti(int osservatore, int contatto) const {
    const auto it = fanali_ia_.find({osservatore, contatto});
    return it == fanali_ia_.end() ? nullptr : &it->second;
}

// La vedetta e le luci, di notte: il giocatore illuminato in superficie visto dalle navi e dagli aerei dell'altra parte
// con la legge dei fanali (probabilita_vista_luci: binocolo, allerta, settori, faro a lampi). Le navi illuminate viste
// dal periscopio dell'IA le aggancia missione_periscopi (portata_luci al posto della portata notturna fissa)
void Missione::vedetta_luci(double passo) {
    if (!luci_attive_ || luce_periscopi() >= NOTTE) return;
    const auto* gc = giocatore();
    if (!gc) return;
    UnitaClassica& g = unita_.at(gc->id);
    if (g.quota > 1.0) return;   // in superficie
    const luci::Stato s = stato_luci(g.id);
    if (s == luci::Stato::spente) return;
    const auto& conf = luci_conf_.at(g.classe);
    const CondizioniVista c = condizioni_luci(g.pos);
    if (!caso_luci_) caso_luci_.emplace(seme_ ^ 0x10C1ull);
    for (auto& [id, u] : unita_) {
        if (id == g.id || u.distrutta || u.persa || arma(u) || stesso_lato(u, g)) continue;
        Osservatore o;
        if (aereo(u)) {
            o.tipo = TipoOsservatore::aereo;
            o.altezza_occhio_m = quota_aerea(u);
        } else if (u.categoria == "nave") {
            o.altezza_occhio_m = OCCHIO_NAVE_M;
        } else {
            continue;
        }
        const auto ci = u.contatti.find(g.id);
        o.allertato = ci != u.contatti.end();
        const double relativo = normalizza_180(rilevamento(g.pos, u.pos) - g.rotta);
        const double p = luci::probabilita_vista_luci(conf, s, relativo, o, c, distanza(u.pos, g.pos), passo, luci::GUADAGNO_BINOCOLO);
        if (p > 0.0) possibile(id, g.id, "vista", 0.0);
        if (p <= 0.0 || caso_luci_->uniforme() >= p) continue;
        Contatto* k = ci != u.contatti.end() ? &ci->second : nuovo_contatto(u, g.id);
        if (!k) continue;
        k->qualita = std::max(k->qualita, 800.0);
        k->lampeggio = 0;
        k->ultima_scoperta = tempo_;
        k->rilevamento = rilevamento(u.pos, g.pos);
        k->pos_stimata = g.pos;
        k->quota_stimata = g.quota;
        k->posizione_nota = true;
        k->t_posizione = tempo_;
        k->sensore = "vista";
        evento_allerta(id, "albero");   // come la testa vista: la vedetta ha visto il battello
        scelta_bersaglio(u, g, *k);
    }
}

// La scoperta automatica «a vista» del modello originale (Missione::valuta: navi e aerei su un'unita' in superficie, il
// periscopio alzato sulle navi, entrambi entro 9 miglia) di notte vale solo entro la portata notturna: luci o sagoma.
// Di giorno, senza luci_enhanced (il classico) o per un'unita' senza configurazione resta com'era (trailer-1)
bool Missione::vista_automatica_di_notte(const UnitaClassica& o, const UnitaClassica& b) const {
    if (!luci_attive_ || luce_periscopi() >= NOTTE) return true;
    Osservatore oss;
    if (aereo(o)) {
        oss.tipo = TipoOsservatore::aereo;
        oss.altezza_occhio_m = quota_aerea(o);
    } else if (o.categoria == "nave") {
        oss.altezza_occhio_m = OCCHIO_NAVE_M;
    } else {
        oss.tipo = TipoOsservatore::periscopio;
        oss.altezza_occhio_m = OCCHIO_PERISCOPIO_M;
    }
    oss.allertato = o.contatti.count(b.id) != 0;
    const double portata = portata_notte(b.id, o.pos, oss);
    return portata < 0.0 || distanza(o.pos, b.pos) <= portata;
}

}  // namespace m688
