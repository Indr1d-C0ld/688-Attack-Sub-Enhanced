#include "missioni/carriera_missione.hpp"

#include <algorithm>
#include <set>
#include <tuple>

#include "missioni/registro_missione.hpp"

namespace m688 {

namespace {

constexpr double ARTICO_BANCHISA_S = 600.0;   // dieci minuti sotto la banchisa: una missione artica (STIMA, §1.2)

std::string categoria_di(const Missione& m, const EventoMissione& e, int id) {
    for (const auto& p : e.pose)
        if (p.unita == id) return p.categoria;
    const auto it = m.unita().find(id);
    return it == m.unita().end() ? std::string{} : it->second.categoria;
}

// un'arma contro un bersaglio che non puo' colpire bene: missili antinave contro un sottomarino, siluri contro aerei
bool incoerente(const std::string& arma, const std::string& bersaglio) {
    if ((arma == "harpoon" || arma == "tomahawk") && bersaglio == "sottomarino") return true;
    if ((arma == "mk48" || arma == "type53") && bersaglio == "aereo") return true;
    return false;
}

std::string frase(std::string s) {
    if (!s.empty() && s.back() != '.') s += '.';
    return s;
}

}  // namespace

std::pair<int, int> minaccia_calcolata(const Missione& m) {
    const UnitaClassica* g = m.giocatore();
    if (!g) return {1, 3};
    int punti = 0;
    for (const auto& [id, u] : m.unita()) {
        if (id == g->id || (u.flag & 0x18) == 0 || ((u.flag ^ g->flag) & 0x18) != 0x18) continue;   // solo gli ostili
        if (u.categoria == "sottomarino") punti += 3;
        else if (u.categoria == "nave" || u.categoria == "aereo") punti += 2;
    }
    const int avversario = punti < 3 ? 0 : punti < 8 ? 1 : punti < 14 ? 2 : 3;
    return {avversario, std::clamp(1 + punti / 5, 1, 5)};
}

EsitoPerCarriera riassumi(const Missione& m, const OpzioniRiassunto& o) {
    const EsitoDettagliato x = m.esito_dettagliato();
    EsitoPerCarriera e;
    e.riuscita = x.riuscita;
    e.completezza = x.completezza;
    e.classificazioni_giuste = x.classificazioni_giuste;
    e.classificazioni_errate = x.classificazioni_errate;
    e.anticipo_medio_s = x.anticipo_medio_s;
    e.rapporti_richiesti = x.rapporti_richiesti;
    e.rapporti_inviati = x.rapporti_inviati;
    e.esposizioni = {{"ping", x.ping_subiti}, {"fix", x.fix_subiti}, {"avvistamento", x.avvistamenti},
                     {"esm", x.esm_rischio_alto}, {"transitorio", x.transitori}, {"boa", x.boe_vicine}};
    // docs/26 §4: i ping dell'ecoscandaglio, il radar proprio acceso, i lanci di missili (la vampa e la scia)
    int eco = 0, missili = 0;
    for (const EventoMissione& ev : m.eventi_missione()) eco += ev.tipo == TipoEvento::ping_ecoscandaglio;
    if (const auto* g = m.giocatore())
        for (const auto& l : m.lanci_missili()) missili += l.unita == g->id;
    e.esposizioni["ecoscandaglio"] = eco;
    e.esposizioni["radar"] = m.conti_giocatore().accensioni_radar;
    e.esposizioni["missile"] = missili;
    // le luci proprie accese emerso di notte (nota disassemblatore-achievement-31-10-proposta.md §5.2): gli episodi; in pace
    // non pesano (le luci sono la regola), e la situazione la sa la carriera (ContestoMissione)
    e.esposizioni["luci"] = m.conti_giocatore().luci_accese_notte;
    // il soccorso (m688.salvati): in pace la carriera ne fa la decorazione al coraggio (carriera.cpp, formula `soccorso`)
    e.soccorso = m.salvati().salvati > 0;
    e.ordini = x.ordini;
    e.ordini_eseguiti = x.ordini_eseguiti;
    e.serie_copiate = x.serie_copiate;
    e.serie_perse = x.serie_perse;
    e.infrazioni_roe = static_cast<int>(x.infrazioni.size());
    // armi d'attacco: le esche e il TLAM-C non hanno un bersaglio da colpire; le mine non si lanciano contro
    // qualcuno, ma i loro colpi contano
    for (const auto& [arma, k] : x.per_arma) {
        if (arma == "esca" || arma == "tlam_c") continue;
        const bool mina = arma == "mk67" || arma == "mk60" || arma == "pmr1" || arma == "pmr2" || arma == "mdt";
        if (!mina) e.lanci += k.lanci;
        e.colpi += k.colpi;
    }
    e.danni = x.danno_massimo;
    e.feriti = x.feriti;
    e.morti = x.morti;
    e.comandante_ferito = x.comandante_ferito;
    e.battello_perso = x.battello_perso;
    e.equipaggio_salvo = x.equipaggio_salvo;
    e.neutrali_colpiti = x.neutrali_colpiti;
    e.amici_colpiti = x.amici_colpiti;
    e.fuoco_senza_ordine = x.fuoco_senza_ordine;
    e.zone_vietate = x.zone_vietate;
    e.incaglio = x.incaglio;
    e.danno_nucleare = x.danno_nucleare;
    e.salvataggio_a_bordo = x.danni_domati > 0;
    e.distanza_min_siluro_m = x.distanza_min_siluro_m;
    e.tempo_contatto_asw_s = x.tempo_contatto_asw_s;
    e.margine_collasso = x.margine_collasso;
    e.attaccato = x.colpi_subiti > 0 || x.siluri_evitati > 0;
    const UnitaClassica* g = m.giocatore();
    for (const auto& ev : m.eventi_missione()) {
        if (ev.tipo == TipoEvento::lancio_giocatore && ev.altra >= 0 && incoerente(ev.classe_arma, categoria_di(m, ev, ev.altra)))
            ++e.armi_incoerenti;
        if (ev.tipo == TipoEvento::lancio_nemico && g && ev.altra == g->id) e.attaccato = true;
        if (ev.tipo == TipoEvento::camera_di_lancio_persa) e.armi_perse = std::max(0, o.armi_a_bordo);
    }
    if (o.carico && o.catalogo) e.scarto_carico = scarto_dal_standard(*o.carico, *o.catalogo, o.locale, o.tipo_missione);
    // acque territoriali straniere: le entrate, il tempo dentro e se ci hanno scoperti dentro (un intervallo senza
    // uscita dura fino alla fine)
    {
        static const std::set<TipoEvento> SCOPERTA = {TipoEvento::avvistato, TipoEvento::fix_radiogoniometria, TipoEvento::ping_subito,
                                                      TipoEvento::attacco_simulato, TipoEvento::artiglieria_salva,
                                                      TipoEvento::artiglieria_vicino, TipoEvento::artiglieria_colpo};
        std::vector<std::pair<double, double>> dentro;
        for (const auto& ev : m.eventi_missione()) {
            if (ev.tipo != TipoEvento::acque_territoriali) continue;
            if (ev.livello == 0) {
                ++e.acque_territoriali;
                dentro.emplace_back(ev.tempo, 1e18);
            } else if (!dentro.empty() && dentro.back().second > 1e17) {
                dentro.back().second = ev.tempo;
            }
        }
        for (const auto& [da, a] : dentro) e.tempo_in_acque_s += std::max(0.0, std::min(a, m.tempo()) - da);
        for (const auto& ev : m.eventi_missione())
            if (SCOPERTA.count(ev.tipo))
                for (const auto& [da, a] : dentro)
                    if (ev.tempo >= da && ev.tempo <= a) e.scoperto_in_acque = true;
    }

    // i fatti della motivazione: fra i momenti salienti, quelli d'azione noti allora, i piu' interessanti
    static const std::set<TipoEvento> AZIONE = {TipoEvento::colpo, TipoEvento::affondamento, TipoEvento::siluro_evitato,
                                                TipoEvento::emersione_emergenza, TipoEvento::danno_domato, TipoEvento::esca_sedotta};
    std::vector<const EventoMissione*> fatti;
    for (const EventoMissione* ev : m.registro().momenti_salienti(12))
        if (AZIONE.count(ev->tipo) && ev->noto && !ev->testo_it.empty()) fatti.push_back(ev);
    std::stable_sort(fatti.begin(), fatti.end(), [](const auto* a, const auto* b) { return a->interesse > b->interesse; });
    if (static_cast<int>(fatti.size()) > o.fatti_max) fatti.resize(static_cast<std::size_t>(std::max(0, o.fatti_max)));
    std::stable_sort(fatti.begin(), fatti.end(), [](const auto* a, const auto* b) { return a->tempo < b->tempo; });
    for (const auto* ev : fatti) {
        e.eventi_chiave.push_back(ev->id);
        e.fatti_it.push_back(frase(ev->testo_it));
        e.fatti_en.push_back(frase(ev->testo_en.empty() ? ev->testo_it : ev->testo_en));
    }
    return e;
}

ContestoMissione contesto_carriera(const Missione& m, const RegoleCarriera& r, const std::string& origine, std::uint64_t seme) {
    ContestoMissione c;
    c.missione = m.id();
    c.origine = origine;
    c.seme = seme;
    if (const auto it = r.originali.find(m.id()); it != r.originali.end()) {
        c.situazione = it->second.situazione;
        c.avversario = it->second.avversario;
        c.difficolta = it->second.difficolta;
        c.operazione_speciale = it->second.speciale;
        c.artico = it->second.artico;
        const auto& f = it->second.esiti_fuoco;
        c.fuoco_senza_ordine = m.esito() && std::find(f.begin(), f.end(), *m.esito()) != f.end();
    }
    c.richiamato = m.esito_dettagliato().categoria == EsitoDettagliato::Categoria::richiamato;
    if (const auto& b = m.blocco_carriera()) {   // rapide ed editor: il blocco `carriera` della missione
        c.blocco_carriera = true;
        c.situazione = situazione_da_nome(b->situazione).value_or(Situazione::pace);
        c.avversario = std::max(0, avversario_da_nome(b->avversario));
        c.difficolta = std::clamp(b->difficolta, 1, 5);
        c.operazione_speciale = b->operazione_speciale;
        c.artico = b->artico;
    }
    // l'Artico anche dal tempo vero sotto la banchisa (nota disassemblatore-achievement-31-10-proposta.md §1.2)
    c.artico = c.artico || m.conti_giocatore().sotto_banchisa_s >= ARTICO_BANCHISA_S;
    std::tie(c.avversario_calcolato, c.difficolta_calcolata) = minaccia_calcolata(m);
    const UnitaClassica* g = m.giocatore();
    c.battello = g && g->classe == "los_angeles" ? "Los Angeles (SSN 688)" : g && g->classe == "alfa" ? "Alfa" : g ? g->classe : "";
    c.teatro = c.teatro_en = m.teatro();
    return c;
}

}  // namespace m688
