#include "partita.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <ctime>
#include <filesystem>
#include <optional>
#include <stdexcept>
#include <utility>

#include "missioni/pacchetto_missione.hpp"
#include "missioni/traccia_missione.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/geometria.hpp"
#include "nucleo/strumenti_carta.hpp"
#include "nucleo/testi.hpp"

namespace m688 {

void sincronizza(const Missione& mis, Mondo& mondo, int id_proprio) {
    const Nave* prima = mondo.trova(id_proprio);
    const std::optional<Nave> proprio = prima ? std::optional<Nave>(*prima) : std::nullopt;
    mis.sincronizza(mondo);
    if (!mondo.trova(id_proprio) && proprio) {
        Nave n = *proprio;
        n.pos_prec = n.pos; n.rotta_prec = n.rotta; n.velocita = n.velocita_ordinata = 0.0;
        mondo.aggiungi(n);
    }
}

Partita::Partita(const Risorse& r, const ImpostazioniPartita& o, Effetti& effetti)
    : r_(r), effetti_(effetti), cartella_dati_(o.dati) {
    // le missioni importate sono file di terzi: i loro script girano coi limiti (un ciclo senza fine o una tabella
    // enorme finiscono in errori() e la partita continua); le nostre senza
    LimitiScript limiti;
    std::error_code ec;
    if (std::filesystem::equivalent(o.cartella_missioni, cartella_missioni_importate(), ec)) {
        limiti.istruzioni_per_chiamata = 10'000'000;
        limiti.memoria_byte = std::size_t{64} << 20;
    }
    mis_ = std::make_unique<Missione>(o.cartella_missioni + "/" + o.missione + ".lua", o.seme, limiti);
    std::map<std::string, double> pescaggi;
    for (const auto& [id, c] : r.classi) pescaggi[id] = c.pescaggio_m;
    mis_->imposta_fondale(std::make_shared<const Fondale>(Fondale::carica(o.dati + "/teatri/" + mis_->teatro() + ".fdl")),
                          std::move(pescaggi));
    if (o.regola == RegolaDanni::simulazione) {   // la traccia del replay (solo Enhanced: il classico non cambia)
        traccia_ = std::make_unique<RegistratoreTraccia>(o.missione, o.seme, o.ripresa_da);
        const std::time_t ora = std::time(nullptr);
        char oggi[16];
        std::strftime(oggi, sizeof oggi, "%Y-%m-%d", std::localtime(&ora));
        traccia_->metadato("data_reale", oggi);
        traccia_->metadato("teatro", mis_->teatro());   // per lo schermo del replay: il fondale e il battello
        if (const auto* g = mis_->giocatore()) traccia_->metadato("giocatore", std::to_string(g->id));
    }
    testi_ = carica_testi(o.dati + "/testi/messaggi_bordo.tsv");
    if (!mis_->giocatore()) throw std::runtime_error(o.missione + ": nessuna unita' del giocatore");
    // carte d'epoca delle due parti (N5, docs/08 §5): solo per le stazioni, la simulazione resta sul fondale vero
    if (o.regola == RegolaDanni::simulazione && o.carta == "epoca")
        for (const Parte p : {Parte::nato, Parte::urss})
            if (const std::string f = percorso_carta(o.dati, mis_->teatro(), p); std::filesystem::exists(f))
                mis_->imposta_fondale_carta(p, std::make_shared<const Fondale>(Fondale::carica(f)));
    // la sala radio dell'Enhanced (DTG e finestre) parte dall'ora del pannello
    mis_->imposta_data_ora(o.inizio.giorno, o.inizio.mese, o.inizio.anno, o.inizio.ore_utc);
    // la sala radio nasce sempre prima del primo passo (il briefing la legge, e partita e replay restano uguali)
    if (o.regola == RegolaDanni::simulazione) mis_->radio_enhanced();
    id_proprio_ = mis_->giocatore()->id;
    sincronizza(*mis_, mondo_, id_proprio_);
    // danni, relitti e reazioni nelle missioni (fase D5)
    dm_ = std::make_unique<DanniMissione>(r.classi, o.dati, o.seme, o.regola);
    // danni dei sottomarini, giocatore compreso (fase D6, solo con la regola di simulazione)
    if (o.regola == RegolaDanni::simulazione) {
        ds_ = std::make_unique<DanniSottomarini>(r.classi, o.dati, o.seme);
        const auto* g = mis_->giocatore();
        ds_->prepara(*mis_, g->id);   // quadro danni visibile prima del primo colpo
        if (!o.urto_giocatore.empty()) {
            double kg, dist, gradi_urto;
            if (std::sscanf(o.urto_giocatore.c_str(), "%lf,%lf,%lf", &kg, &dist, &gradi_urto) != 3)
                throw std::runtime_error("urto non valido (KG,DIST,GRADI): " + o.urto_giocatore);
            ds_->urto(*mis_, g->id, kg, dist, radianti(gradi_urto));
        }
        if (!o.urto_unita.empty()) {
            int id;
            double kg, dist, gradi_urto;
            if (std::sscanf(o.urto_unita.c_str(), "%d,%lf,%lf,%lf", &id, &kg, &dist, &gradi_urto) != 4)
                throw std::runtime_error("urto non valido (ID,KG,DIST,GRADI): " + o.urto_unita);
            ds_->prepara(*mis_, id);
            ds_->urto(*mis_, id, kg, dist, radianti(gradi_urto));
        }
    }
    // stazioni (fasi C3-C4): tubi, lancio e guida a filo, sonar; nell'Enhanced guasti e limiti dal battello danneggiato
    stazioni_ = std::make_unique<StazioniMissione>(*mis_, static_cast<std::uint32_t>(o.seme));
    stazioni_->usa_danni(ds_.get());
    stazioni_->raccogli_tma(o.regola == RegolaDanni::simulazione);   // Enhanced: rilevamenti per la TMA
    const auto& g = *mis_->giocatore();
    posto_.imposta_telegrafo(PostoDiComando::posizione_da_velocita(g.velocita_ordinata / std::max(g.avanti_tutta, 0.1)));
}

// ASW aereo nella vista (A5, docs/13): a ogni passo gli aerei controllati (hover e spruzzi, cavo del sonar a immersione,
// MAD rimorchiato, luci) e le boe coi loro tempi; una volta i marcatori lanciati e i siluri aviolanciati col paracadute.
// Le bombe di profondita' non hanno paracadute: lo scoppio lo fa la fisica delle esplosioni.
void Partita::effetti_asw(const Missione& mis) {
    const auto voli = mis.voli_asw();
    if (voli.empty() && mis.boe().empty() && mis.marcatori_asw().empty() && mis.lanci_aerei().empty()) return;
    if (!asw_) {
        try {
            asw_ = carica_asw_aereo(cartella_dati_ + "/asw_aereo.toml");
        } catch (const std::exception&) {
            asw_.emplace();
        }
    }
    const auto mezzo = [&](int id) -> const DatiMezzo* {
        const auto it = mis.unita().find(id);
        return it != mis.unita().end() ? asw_->mezzo(it->second.classe) : nullptr;
    };
    std::vector<AereoAsw> aerei;
    for (const VoloAsw& v : voli) {
        const auto it = mis.unita().find(v.unita);
        if (it == mis.unita().end() || it->second.distrutta || it->second.persa) continue;
        const UnitaClassica& u = it->second;
        const DatiMezzo* d = asw_->mezzo(v.classe);
        const auto ic = r_.classi.find(v.classe);
        const double largo = ic != r_.classi.end() ? ic->second.larghezza_m : 10.0;
        AereoAsw a;
        a.id = v.unita;
        a.pos = u.pos;
        a.quota_m = v.quota_m;
        a.rotta = u.rotta;
        a.velocita = mis.metri_al_secondo(u.velocita);
        a.elicottero = d && d->elicottero;
        a.hover = v.comando.hover;
        a.rotore_m = a.elicottero ? largo : 0.0;   // per gli elicotteri la larghezza di classi.toml e' il rotore
        a.cavo_m = v.comando.cavo_m;
        a.mad_m = d && d->mad ? d->mad->cavo_m : 0.0;
        a.apertura_m = largo;
        a.luci = v.comando.luci;
        aerei.push_back(a);
    }
    // gli avvisi al giocatore (380-389): il testo coi rilevamento misurato al posto di {RIL}
    for (; avvisi_testo_visti_ < mis.messaggi_asw().size(); ++avvisi_testo_visti_) {
        const AvvisoAsw& a = mis.messaggi_asw()[avvisi_testo_visti_];
        const auto it = testi_.find(a.codice);
        if (it == testi_.end()) continue;
        std::string s = componi_messaggio(it->second, false);
        if (const auto k = s.find("{RIL}"); k != std::string::npos) {
            char ril[8];
            std::snprintf(ril, sizeof ril, "%03.0f", a.rilevamento ? normalizza_360(*a.rilevamento) : 0.0);
            if (a.rilevamento) s.replace(k, 5, ril);
            else s.erase(s.rfind(',', k), k + 5 - s.rfind(',', k));   // senza rilevamento: via «, rilevamento {RIL}»
        }
        avvisi_testo_.emplace_back(a.tempo, s);
    }
    effetti_.aerei_asw(aerei);
    effetti_.boe_asw(mis.boe(), mis.tempo());
    for (; marcatori_mostrati_ < mis.marcatori_asw().size(); ++marcatori_mostrati_) {
        const MarcatoreAereo& m = mis.marcatori_asw()[marcatori_mostrati_];
        if (m.fine <= mis.tempo() || m.tipo == "mk84") continue;   // il SUS e' solo acustico
        const bool notte = luce_ < 0.3;
        const MarcatoreAsw tipo = m.tipo == "mk58"   ? MarcatoreAsw::mk58
                                  : m.tipo == "omab" ? (notte ? MarcatoreAsw::omab_notte : MarcatoreAsw::omab_giorno)
                                                     : MarcatoreAsw::mk25;
        effetti_.marcatore_asw(tipo, m.pos, m.fine - mis.tempo());
    }
    for (; lanci_aerei_mostrati_ < mis.lanci_aerei().size(); ++lanci_aerei_mostrati_) {
        const LancioAereo& l = mis.lanci_aerei()[lanci_aerei_mostrati_];
        if (l.tipo.rfind("plab", 0) == 0) continue;
        const auto it = mis.unita().find(l.aereo);
        const double v = it != mis.unita().end() ? mis.metri_al_secondo(it->second.velocita) : 50.0;
        const DatiMezzo* d = mezzo(l.aereo);
        effetti_.arma_paracadute(l.pos, l.quota_sgancio_m, l.rotta, v, d && d->parte == ParteAsw::urss);
    }
}

// Un avviso composto: {COMANDANTE} per la parte del giocatore e il rilevamento al posto di {RIL}
void Partita::avviso(int codice, double tempo, std::optional<double> ril) {
    const auto it = testi_.find(codice);
    if (it == testi_.end()) return;
    const auto* g = mis_->giocatore();
    std::string s = componi_messaggio(it->second, g && g->classe == "alfa");
    if (const auto k = s.find("{RIL}"); k != std::string::npos) {
        char x[8];
        std::snprintf(x, sizeof x, "%03.0f", ril ? normalizza_360(*ril) : 0.0);
        if (ril) s.replace(k, 5, x);
        else s.erase(s.rfind(',', k), k + 5 - s.rfind(',', k));
    }
    avvisi_testo_.emplace_back(tempo, s);
}

// Artiglieria navale (docs/16, G6; dossier 22 §9.6-9.7): gli eventi della missione agli effetti (colonne, vampe,
// illuminanti) e quel che l'equipaggio vede o sente. Col battello esposto (albero o periscopio su, o in superficie) la
// vedetta riporta le vampe, le colonne vicine e gli illuminanti; in immersione il sonarista sente gli scoppi. I colpi
// sul battello: la parte colpita (periscopi, alberi, vela)
void Partita::artiglieria(const Missione& mis) {
    const UnitaClassica* g = mis.giocatore();
    const bool esposto = g && ((g->flag54 & 6) || g->quota <= 2.0);
    for (; eventi_art_visti_ < mis.eventi_artiglieria().size(); ++eventi_art_visti_) {
        const EventoArtiglieria& e = mis.eventi_artiglieria()[eventi_art_visti_];
        effetti_.artiglieria(e, mis.tempo());
        if (!g || (e.bersaglio >= 0 && e.bersaglio != g->id && e.tipo != TipoEventoArtiglieria::illuminante)) continue;
        const double d = distanza(g->pos, e.pos), ril = rilevamento(g->pos, e.pos), t = e.tempo_s;
        switch (e.tipo) {
            case TipoEventoArtiglieria::partenza: {   // le vampe: una volta al minuto per nave
                auto& u = ultima_vampa_.try_emplace(e.tiratore, -1e9).first->second;
                if (esposto && d <= 25000.0 && t - u >= 60.0) { avviso(421, t, ril); u = t; }
                break;
            }
            case TipoEventoArtiglieria::scoppio_superficie:
            case TipoEventoArtiglieria::scoppio_aria:
            case TipoEventoArtiglieria::raffica:
                if (!esposto && d <= 15000.0 && t - ultimo_sonar_art_ >= 120.0) { avviso(420, t, ril); ultimo_sonar_art_ = t; }
                break;
            case TipoEventoArtiglieria::illuminante:
                if (esposto && d <= 15000.0 && t - ultimo_illuminante_ >= 120.0) {   // sopra di noi: senza rilevamento
                    avviso(429, t, d >= 1000.0 ? std::optional<double>(ril) : std::nullopt);
                    ultimo_illuminante_ = t;
                }
                break;
            default: break;
        }
    }
    for (; impatti_art_visti_ < mis.impatti_artiglieria().size(); ++impatti_art_visti_) {
        const ImpattoArtiglieria& i = mis.impatti_artiglieria()[impatti_art_visti_];
        if (!g || i.bersaglio != g->id) continue;
        const double t = i.tempo_impatto_s;
        if (i.avvertimento) {   // le bombe d'avvertimento delle acque territoriali hanno il loro messaggio (455, runtime)
            if (i.pezzo != "bomba_avvertimento" && t - ultimo_avvertimento_ >= 300.0) { avviso(423, t); ultimo_avvertimento_ = t; }
            continue;
        }
        using E = ImpattoArtiglieria::Esito;
        using P = ParteEsposta;
        if (i.esito == E::diretto || i.esito == E::schegge) {
            int codice = 0;
            switch (i.parte) {
                case P::periscopio_attacco: codice = i.esito == E::diretto ? 424 : 426; break;
                case P::periscopio_ricerca: codice = i.esito == E::diretto ? 425 : 426; break;
                case P::albero_esm: case P::albero_radio: case P::radar: case P::snorkel: codice = 427; break;
                case P::falsatorre: case P::plancia: case P::scafo: codice = i.esito == E::diretto ? 428 : 0; break;
                case P::nessuna: break;
            }
            // lo stesso rapporto al piu' ogni 30 s: sotto una raffica l'equipaggio non lo ripete a ogni colpo
            if (codice) {
                auto& u = ultimo_colpo_.try_emplace(codice, -1e9).first->second;
                if (t - u >= 30.0) { avviso(codice, t); u = t; }
            }
            if (i.parte != P::nessuna) continue;
        }
        if (esposto && distanza(g->pos, i.punto) <= 150.0 && t - ultima_colonna_ >= 60.0) { avviso(422, t); ultima_colonna_ = t; }
    }
}

// Collisioni (dossier 16 §2-3, P4): sui contatti che l'equipaggio ha, la rotta di collisione (avvicinamento con il
// passaggio previsto entro 300 m nei prossimi 3 minuti, STIMA) e il contatto vicinissimo (sotto 500 m), solo se le
// quote si incrociano: una nave conta quando siamo sopra la sua chiglia con un margine di 30 m, un sottomarino entro
// 60 m di quota. Poi le quasi collisioni registrate dalla missione, con o senza contatto
// Lo spettatore in partita (docs/18, fase 5): dei fatti fra unita' dell'IA l'equipaggio riporta solo quelli su cui ha
// un contatto (noto). Il sonarista sente il lancio di un'arma non diretta a noi, i colpi d'artiglieria, l'urto fra
// due scafi, le esche di un sottomarino e chi accelera per allontanarsi; con l'albero o il periscopio su la vedetta
// vede le vampe. Un avviso per tipo ogni 30 s
void Partita::interazioni_ia(const Missione& mis) {
    const auto& ev = mis.registro().eventi();
    const UnitaClassica* g = mis.giocatore();
    if (!g || !ds_) { eventi_ia_visti_ = ev.size(); return; }   // solo l'Enhanced
    const bool esposto = (g->flag54 & 6) || g->quota <= 2.0;
    for (; eventi_ia_visti_ < ev.size(); ++eventi_ia_visti_) {
        const EventoMissione& e = ev[eventi_ia_visti_];
        if (!e.noto) continue;
        int codice = 0;
        switch (e.tipo) {
            case TipoEvento::lancio_ia: if (e.altra != g->id) codice = 460; break;   // quelli su di noi hanno i loro avvisi
            case TipoEvento::artiglieria_ia: codice = esposto ? 462 : 461; break;
            case TipoEvento::collisione_ia: codice = 463; break;
            case TipoEvento::esca_ia: codice = 464; break;                       // fase 4 A: esche di un sottomarino
            case TipoEvento::ritirata_ia: codice = 465; break;                  // chi si ritira, anche da noi
            case TipoEvento::reazione_ia:   // fase 4 C: chi ci segue (in pace, il pedinamento del 24 §8.5)
                if (e.altra == g->id && e.classe_arma == "pedina") codice = 466;
                break;
            case TipoEvento::incsea: codice = 467; break;                       // fase 4 D: una nave troppo vicina
            default: break;
        }
        if (!codice) continue;
        double& u = ultimo_ia_.try_emplace(codice, -1e9).first->second;
        if (e.tempo - u < 30.0) continue;
        avviso(codice, e.tempo, rilevamento(g->pos, e.pos));
        u = e.tempo;
    }
}

void Partita::allarmi_collisione(const Missione& mis) {
    const UnitaClassica* g = mis.giocatore();
    if (!g || !ds_) return;   // solo l'Enhanced
    const double t = mis.tempo();
    const Vec2 vg = vettore(g->rotta, mis.metri_al_secondo(g->velocita));
    for (const auto& [id, c] : g->contatti) {
        const auto it = mis.unita().find(id);
        if (it == mis.unita().end() || it->second.distrutta) continue;
        const UnitaClassica& u = it->second;
        if (u.categoria != "nave" && u.categoria != "sottomarino") continue;
        if (u.categoria == "nave") {   // la schiera trainata (S10): il cavo entro 300 m, a ogni quota (STIMA)
            const double dc = mis.distanza_dal_cavo(g->id, id);
            auto& uc = ultimo_cavo_.try_emplace(id, -1e9).first->second;
            if (dc >= 0.0 && dc < 300.0 && t - uc >= 120.0) { avviso(435, t, rilevamento(g->pos, u.pos)); uc = t; }
        }
        const bool incrocia = u.categoria == "nave" ? g->quota <= mis.pescaggio(u) + 30.0 : std::fabs(u.quota - g->quota) <= 60.0;
        if (!incrocia) continue;
        const Vec2 r{u.pos.x - g->pos.x, u.pos.y - g->pos.y};
        const Vec2 vu = vettore(u.rotta, mis.metri_al_secondo(u.velocita));
        const Vec2 v{vu.x - vg.x, vu.y - vg.y};
        const double d = std::hypot(r.x, r.y), v2 = v.x * v.x + v.y * v.y;
        const double tcpa = v2 > 1e-6 ? -(r.x * v.x + r.y * v.y) / v2 : -1.0;
        const double cpa = std::hypot(r.x + v.x * tcpa, r.y + v.y * tcpa);
        const double ril = rilevamento(g->pos, u.pos);
        if (d < 500.0) {
            auto& ul = ultimo_vicino_.try_emplace(id, -1e9).first->second;
            if (t - ul >= 60.0) { avviso(432, t, ril); ul = t; }
        } else if (d < 3000.0 && tcpa > 0.0 && tcpa < 180.0 && cpa < 300.0) {
            auto& ul = ultimo_cpa_.try_emplace(id, -1e9).first->second;
            if (t - ul >= 120.0) { avviso(431, t, ril); ul = t; }
        }
    }
    for (; cavi_visti_ < mis.incidenti_cavo().size(); ++cavi_visti_) {
        const auto& k = mis.incidenti_cavo()[cavi_visti_];
        if (k.sottomarino == g->id) avviso(k.impigliato ? 436 : 437, k.tempo_s);
    }
    for (; quasi_viste_ < mis.quasi_collisioni().size(); ++quasi_viste_) {
        const auto& q = mis.quasi_collisioni()[quasi_viste_];
        if (q.a == g->id) avviso(q.a_sapeva ? 434 : 433, q.tempo_s);
    }
}

double Partita::x_albero(const std::string& classe) {
    const auto it = alberi_x_.find(classe);
    if (it != alberi_x_.end()) return it->second;
    double x = 0.0, z = -1e9;
    if (const auto im = r_.modelli.find(classe); im != r_.modelli.end())
        for (const auto& t : im->second.triangoli())
            for (const auto& v : t.v)
                if (v[2] > z) { z = v[2]; x = v[0]; }
    return alberi_x_[classe] = x;
}

void Partita::imposta_carico(CatalogoCarico catalogo, const CaricoPartenza& carico) {
    catalogo_ = std::make_unique<CatalogoCarico>(std::move(catalogo));
    carico_ = carico;
    ordini_.imposta_carico(carico);   // nello storico: il replay riparte con lo stesso carico
    stazioni_->imposta_carico(*mis_, *carico_, *catalogo_);
}

void Partita::avanza(double dt) {
    Missione& mis = *mis_;
    for (const auto& ord : ordini_.consegna(mis.tempo())) {
        posto_.osserva(ord);
        if (osserva) osserva(ord);
        if (stazioni_->rifiuto_per_guasto(mis, ord)) {   // sistema guasto: rifiutato con il suo messaggio
            stazioni_->consegna(mis, ord);
            if (ord.tipo == TipoOrdine::telegrafo && mis.giocatore())   // il telegrafo torna dov'era
                posto_.imposta_telegrafo(PostoDiComando::posizione_da_velocita(mis.giocatore()->velocita_ordinata /
                                                                                std::max(mis.giocatore()->avanti_tutta, 0.1)));
            continue;
        }
        if (ds_ && ord.tipo == TipoOrdine::emersione_emergenza && mis.giocatore())   // Enhanced: esaurimento delle casse
            ds_->esaurisci(mis, mis.giocatore()->id);
        if (ds_ && ds_->consegna(mis, ord)) continue;   // Alfa: sfoghi, compensazione e Tan alle casse (runtime)
        if (stazioni_->consegna(mis, ord) || stazioni_->riguarda(ord)) continue;   // stazioni: rifiuti con il loro messaggio
        if (ord.tipo == TipoOrdine::pilota_automatico && ord.n != 0) {   // senza punti di rotta
            posto_.pilota_disinserito();
            if (const auto* g = mis.giocatore()) equipaggio_.push_back({mis.tempo(), g->id, MSG_NESSUN_PUNTO});
        } else {
            std::fprintf(stderr, "ordine non applicabile al livello classico: %s\n", std::string(nome_ordine(ord.tipo)).c_str());
        }
    }
    mis.avanza(dt);
    stazioni_->dopo_passo(mis);
    if (traccia_ && traccia_->serve(mis.tempo(), mis.eventi_missione().size())) {   // la traccia del replay
        Fotogramma f = fotogramma_da(mis, stazioni_.get(), dati_app ? dati_app() : DatiApp{}, traccia_->ultimo_evento(), &raccolta_);
        if (dopo_fotogramma) dopo_fotogramma(f);
        traccia_->aggiungi(std::move(f), mis.eventi_missione().size());
    }
    if (dopo_passo) dopo_passo();
    if (posto_.pilota_automatico() && !mis.pilota_automatico()) posto_.pilota_disinserito();
    dm_->avanza(mis, dt);
    if (ds_) {
        ds_->avanza(mis, dt);
        mis.raccogli_danni(ds_.get(), dm_.get());   // il registro degli eventi (M1): rapporti, emersioni, navi affondate
        // segni in superficie dei sottomarini colpiti: compaiono al loro inizio, per la durata che resta
        segni_mostrati_.resize(ds_->segni().size(), false);   // anche futuri, non in ordine di inizio
        for (std::size_t i = 0; i < ds_->segni().size(); ++i) {
            const auto& sg = ds_->segni()[i];
            if (segni_mostrati_[i] || sg.inizio > mis.tempo()) continue;
            segni_mostrati_[i] = true;
            if (sg.inizio + sg.durata > mis.tempo())
                effetti_.segno_superficie(static_cast<int>(sg.tipo), sg.pos, sg.inizio + sg.durata - mis.tempo(), sg.intensita);
        }
        // emersione violenta dell'IA dopo l'esaurimento delle casse (E1, docs/10 §10.14): acqua, spruzzi, schiuma
        for (; emersioni_mostrate_ < ds_->emersioni().size(); ++emersioni_mostrate_) {
            const auto& e = ds_->emersioni()[emersioni_mostrate_];
            effetti_.emersione_violenta(e.pos, e.rotta, e.velocita, e.angolo, e.lunghezza, e.fuori, e.salita_s + e.caduta_s);
        }
    } else {
        mis.raccogli_danni(nullptr, dm_.get());
    }
    sincronizza(mis, mondo_, id_proprio_);
    dm_->sincronizza(mondo_);
    if (ds_) ds_->sincronizza(mondo_);   // posa dei sottomarini emersi danneggiati (fisica delle navi, D6)
    for (; navi_seguite_ < dm_->navi().size(); ++navi_seguite_) {
        const auto& n = *dm_->navi()[navi_seguite_];
        const auto im = r_.modelli.find(n.classe);
        effetti_.segui(n.id_mondo, n.danni.get(), im != r_.modelli.end() ? &im->second : nullptr);
    }
    // i sottomarini emersi danneggiati (D6): passano alla fisica delle navi, e gli effetti seguono fuoco e carico come per
    // le navi (danni critici fase 4 del kit); l'id nel mondo e' quello dell'unita'
    if (ds_)
        for (const auto& s : ds_->sottomarini())
            if (s->superficie && sottomarini_seguiti_.insert(s->unita).second) {
                const auto im = r_.modelli.find(s->classe);
                effetti_.segui(s->unita, s->superficie.get(), im != r_.modelli.end() ? &im->second : nullptr);
            }
    for (; colpi_mostrati_ < dm_->altri_colpi().size(); ++colpi_mostrati_) {   // sottomarini, esche: solo lo scoppio
        const Colpo& c = dm_->altri_colpi()[colpi_mostrati_];
        if (c.quota <= 0.0 || c.tipo == Colpo::Tipo::attacco_casuale) continue;
        const double kg = c.tipo == Colpo::Tipo::attacco ? 135.0 : c.classe_arma.find("mk48") != std::string::npos ? 544.0 : 300.0;
        effetti_.esplosione_subacquea(c.pos, c.quota, kg, mis.profondita(c.pos));
    }
    // missili lanciati (E2, docs/10 §10.15): dai sottomarini capsula o booster che esce, fiammata, fumo; dalle navi la
    // vampa e la nube del booster sul ponte, poi la stessa traiettoria
    for (; ds_ && lanci_mostrati_ < mis.lanci_missili().size(); ++lanci_mostrati_) {
        const LancioMissile& l = mis.lanci_missili()[lanci_mostrati_];
        const bool nave = l.categoria_lanciatore == "nave";
        if (!nave && l.categoria_lanciatore != "sottomarino") continue;   // gli aerei: non ancora
        // il profilo di lancio per arma (la stazione di tiro: harpoon, tasm, tomahawk, p500...) o dalla classe (gli
        // script dell'originale); il TASM e' un Tomahawk, i missili grandi delle navi sovietiche come il Granit
        // (gli id del modello hanno la variante: harpoon_84c, tlam_c...; si guarda l'inizio)
        const auto inizia = [&](const char* p) { return l.arma.rfind(p, 0) == 0; };
        std::string tipo = inizia("harpoon")                                         ? "harpoon"
                           : inizia("tasm") || inizia("tlam") || inizia("tomahawk")  ? "tomahawk"
                           : inizia("p500") || inizia("p700") || inizia("p1000") || inizia("granit") ? "granit"
                                                                                       : l.arma;
        if (tipo != "harpoon" && tipo != "tomahawk" && tipo != "granit") {
            const std::string& c = l.classe_lanciatore;
            tipo = c == "oscar" ? "granit" : c == "charlie2" ? "malakhit" : c == "charlie" ? "ametist"
                 : c == "los_angeles" || c == "sturgeon" || c == "trafalgar" ? "harpoon" : nave ? "malakhit" : "vodopad";
        }
        effetti_.lancio_missile(l.pos, l.rotta, nave ? 6.0 : l.quota, tipo, nave);   // dalle navi: dal ponte, a 6 m
    }
    if (ds_) effetti_asw(mis);
    artiglieria(mis);
    allarmi_collisione(mis);
    interazioni_ia(mis);
    for (; avvisi_sonarista_visti_ < mis.avvisi_sonarista().size(); ++avvisi_sonarista_visti_) {   // il sonarista dell'equipaggio
        const auto& a = mis.avvisi_sonarista()[avvisi_sonarista_visti_];
        avvisi_testo_.emplace_back(a.tempo, a.testo_it);
    }
    if (ds_)   // piuma di periscopio e antenna dei sottomarini dell'IA a quota periscopica (E2, docs/10 §10.15)
        for (const auto& [id, u] : mis.unita())
            if (!u.giocatore && u.categoria == "sottomarino" && (u.flag54 & 6) && u.quota <= 20.0 && !u.distrutta) {
                const double x = x_albero(u.classe), a = radianti(u.rotta);   // gli alberi sono nella vela
                effetti_.piuma(id, {u.pos.x + x * std::sin(a), u.pos.y + x * std::cos(a)}, u.rotta, u.velocita * NODO,
                               (u.flag54 & 4) ? 3.0 : 2.0, dt);
            }
    if (mis.tempo() >= prossima_traccia_) {   // le tracce del debriefing (solo osservazione)
        prossima_traccia_ = mis.tempo() + 20.0;
        for (const auto& [id, u] : mis.unita())
            if (!u.distrutta) tracce_.push_back({mis.tempo(), id, u.pos});
    }
    if (!effetti_per_fotogramma) anima_effetti(dt);
    // le fotografie per le minicartoline: gli eventi nuovi che si vedono, poi lo scatto al loro istante
    const auto& eventi = mis.eventi_missione();
    for (; eventi_visti_ < eventi.size(); ++eventi_visti_) {
        const EventoMissione& e = eventi[eventi_visti_];
        // il missile fermato dalla difesa della nave (T13 del kit): lo scoppio in aria, alla quota e con l'esplosivo
        if (e.tipo == TipoEvento::fine_missile && e.codice == 2) effetti_.esplosione_aerea(e.pos, e.quota, e.valore, false);
        if (!e.inquadratura.visibile || e.inquadratura.soggetto < 0) continue;
        if (!scelta_fotografie_.scatta(e, fotografie_.size() + da_fotografare_.size())) continue;
        const bool dopo = e.tipo == TipoEvento::colpo || e.tipo == TipoEvento::affondamento ||
                          e.tipo == TipoEvento::lancio_giocatore || e.tipo == TipoEvento::lancio_nemico;
        da_fotografare_.emplace_back(e.tempo + (dopo ? 4.0 : 0.0), e.id);
    }
    for (auto it = da_fotografare_.begin(); it != da_fotografare_.end();)
        if (mis.tempo() >= it->first) {
            fotografie_.emplace(it->second, Fotografia{mis.tempo(), mondo_, effetti_});
            it = da_fotografare_.erase(it);
        } else {
            ++it;
        }
}

void Partita::anima_effetti(double dt) {
    for (double resto = dt; resto > 1e-9;) {
        const double h = std::min(resto, 1.0 / 30.0);
        effetti_.avanza(h, mondo_);
        resto -= h;
    }
    for (auto& n : mondo_.navi())
        if (n.id >= DanniMissione::ID_IMMAGINE) effetti_.decora(n.id, n.posa);   // alberi caduti
}

std::vector<const std::vector<Messaggio>*> Partita::liste_messaggi() const {
    static const std::vector<Messaggio> nessuno;
    return {&mis_->messaggi(), &mis_->messaggi_sonar(), &mis_->messaggi_radio(), &mis_->messaggi_esm(), &dm_->messaggi(),
            ds_ ? &ds_->messaggi() : &nessuno, &stazioni_->messaggi(), &equipaggio_};
}

std::string Partita::ultimo_messaggio() const {
    if (mis_->esito()) {   // prima riga del rapporto ("CODICE: 1-5-ACP"); il testo intero va nel debriefing
        const std::string t = mis_->testo_esito(*mis_->esito());
        return "Missione conclusa  " + (t.empty() ? "esito " + std::to_string(*mis_->esito()) : t.substr(0, t.find('\n')));
    }
    // l'ultimo con un testo fra i messaggi degli script e quelli del motore (codici 200+)
    const Messaggio* ultimo = nullptr;
    for (const auto* lista : liste_messaggi())
        for (const auto& g : *lista) {
            if (lista == &mis_->messaggi() && StazioniMissione::nascosto(g.codice)) continue;   // al loro posto 10 + arma
            if (testo_di(g.codice) && (!ultimo || g.tempo >= ultimo->tempo)) ultimo = &g;
        }
    if (!avvisi_testo_.empty() && (!ultimo || avvisi_testo_.back().first >= ultimo->tempo)) return avvisi_testo_.back().second;
    return ultimo ? *testo_di(ultimo->codice) : std::string{};
}

// Il testo di un messaggio di bordo: la tabella globale, o per i codici da 1000 quello della missione (E6)
std::optional<std::string> Partita::testo_di(int codice) const {
    if (codice >= 1000) {
        if (const TestoRadio* t = mis_->testo_messaggio_missione(codice)) return componi_messaggio(t->it, false);
        return std::nullopt;
    }
    const auto it = testi_.find(codice);
    if (it == testi_.end()) return std::nullopt;
    return componi_messaggio(it->second, false);
}

std::vector<std::pair<double, std::string>> Partita::storico_messaggi() const {
    std::vector<std::pair<double, std::string>> v;
    const auto* g = mis_->giocatore();
    if (!g) return v;
    for (const auto* lista : liste_messaggi())
        for (const auto& x : *lista) {
            const auto testo = testo_di(x.codice);
            if (x.unita != g->id || !testo) continue;
            if (lista == &mis_->messaggi() && StazioniMissione::nascosto(x.codice)) continue;
            v.emplace_back(x.tempo, *testo);
        }
    v.insert(v.end(), avvisi_testo_.begin(), avvisi_testo_.end());
    std::stable_sort(v.begin(), v.end(), [](const auto& a, const auto& b) { return a.first < b.first; });
    return v;
}

void Partita::stampa_resoconto() const {
    const auto testo = [&](int codice) { return testo_di(codice).value_or(std::string{}); };
    for (const auto& g : stazioni_->messaggi()) std::printf("  t=%5.0f s  armi     %3d  %s\n", g.tempo, g.codice, testo(g.codice).c_str());
    for (const auto& g : mis_->messaggi_sonar()) std::printf("  t=%5.0f s  sonar    %3d  %s\n", g.tempo, g.codice, testo(g.codice).c_str());
    for (const auto& g : mis_->messaggi_radio()) std::printf("  t=%5.0f s  radio    %3d  %s\n", g.tempo, g.codice, testo(g.codice).c_str());
    for (const auto& g : mis_->messaggi_esm()) std::printf("  t=%5.0f s  esm      %3d  %s\n", g.tempo, g.codice, testo(g.codice).c_str());
    for (const auto& g : mis_->messaggi()) std::printf("  t=%5.0f s  messaggio %3d  %s\n", g.tempo, g.codice, testo(g.codice).c_str());
    for (const auto& [t, x] : avvisi_testo_) std::printf("  t=%5.0f s  avviso   %s\n", t, x.c_str());
    for (const auto& x : mis_->foto_periscopio())
        std::printf("  t=%5.0f s  foto     %s %03.0f°, %zu unita' in vista\n", x.tempo, x.periscopio.c_str(), x.rilevamento, x.in_vista.size());
    for (const auto& g : mis_->messaggi_periscopi()) std::printf("  t=%5.0f s  periscopio %3d  %s\n", g.tempo, g.codice, testo(g.codice).c_str());
    std::printf("  eventi del registro: %zu\n", mis_->eventi_missione().size());
    if (traccia_) std::printf("  traccia del replay: %zu fotogrammi, %zu byte\n", traccia_->fotogrammi(), traccia_->dati().size());
    if (ds_)
        for (const auto& g : ds_->messaggi())
            std::printf("  t=%5.0f s  battello %3d  %s (unita' %d)\n", g.tempo, g.codice, testo(g.codice).c_str(), g.unita);
    for (const auto& g : dm_->messaggi())
        std::printf("  t=%5.0f s  motore   %3d  %s (unita' %d)\n", g.tempo, g.codice, testo(g.codice).c_str(), g.unita);
    for (const auto& c : mis_->colpi())
        std::printf("  t=%5.0f s  colpo su %d (%s) da %s di %d (%s): danno %d%s%s\n", c.tempo, c.vittima, c.classe_vittima.c_str(),
                    c.classe_arma.empty() ? "attacco" : c.classe_arma.c_str(), c.lanciatore, c.classe_lanciatore.c_str(), c.danno,
                    c.riuscito ? "" : " (mancato)", c.distrutta ? ", distrutta" : "");
    for (const auto& n : dm_->navi())
        for (const auto& riga : n->danni->registro()) std::printf("  [%s %d] %s\n", n->classe.c_str(), n->unita, riga.c_str());
    for (const auto& e : mis_->errori()) std::printf("  ERRORE %s\n", e.c_str());
    if (mis_->esito()) std::printf("  ESITO %d\n%s\n", *mis_->esito(), mis_->testo_esito(*mis_->esito()).c_str());
}

}  // namespace m688
