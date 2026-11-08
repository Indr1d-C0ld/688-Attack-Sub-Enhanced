#!/usr/bin/env python3
# applica.py <cartella engine> [--controlla]: il passo 1 del punto 4 (priorita' delle squadre e isolamento), come sostituzioni esatte: ogni
# testo vecchio deve esserci una volta sola, altrimenti non si scrive niente. Si prova sulla copia privata (engine/build_danni/albero_lavoro) e poi
# si applica una volta ai sorgenti condivisi, header del nucleo compresi (la finestra della principale).
import os
import sys

P = []


def sost(file, vecchio, nuovo):
    P.append((file, vecchio, nuovo))


# ---- nucleo: il modello
H = 'nucleo/include/nucleo/battello.hpp'
sost(H, """//   volume d'aria dei compartimenti raggiunti) e appiccano l'incendio. Batterie allagate: cloro (fumo tossico).
// Tutto il caso viene dal seme.
""", """//   volume d'aria dei compartimenti raggiunti) e appiccano l'incendio. Batterie allagate: cloro (fumo tossico).
// - Isolare un compartimento (docs/10 §10.19): le paratie sono gia' chiuse, quindi non ferma l'acqua e non cambia l'ossigeno
//   che il fuoco consuma. Le squadre escono e non rientrano, il fuoco passa meno ai vicini, il ramo dell'aria si chiude se
//   non e' gia' rotto; chi resta dentro muore se il compartimento diventa letale prima che si riapra. Solo per ordine.
// Tutto il caso viene dal seme.
""")
sost(H, """    bool reagito = false;
    double aria() const { return volume - acqua; }
""", """    bool reagito = false;
    // isolamento (BattelloDanni::isola): porte_s e' l'istante in cui le porte si chiudono (< 0: nessuno sgombero in corso),
    // dentro gli uomini rimasti chiusi
    bool isolato = false;
    double porte_s = -1.0;
    int dentro = 0;
    bool senza_squadre() const { return isolato || porte_s >= 0.0; }
    double aria() const { return volume - acqua; }
""")
sost(H, """                      incendio, esplosione, incendio_spento, cloro, aria_nell_incendio, cassa_squarciata };
""", """                      incendio, esplosione, incendio_spento, cloro, aria_nell_incendio, cassa_squarciata,
                      sgombero, isolamento, riapertura, caduti };
""")
sost(H, """    int sistema = -1;         // guasti e riparazioni
    std::string testo;
""", """    int sistema = -1;         // guasti e riparazioni
    int uomini = 0;           // caduti: quanti
    std::string testo;
""")
sost(H, """    void priorita(Sistema s);    // il giocatore porta un sistema in cima alle riparazioni
""", """    void priorita(Sistema s);    // il giocatore porta un sistema in cima alle riparazioni
    const std::vector<Sistema>& ordine_riparazioni() const { return priorita_; }
    // Isolare un compartimento (docs/10 §10.19). isola: le squadre escono subito e le porte si chiudono dopo lo sgombero
    // (SGOMBERO_S; senza personale subito); chiudi_subito, durante lo sgombero: chi non e' uscito resta dentro; riapri: chi
    // e' dentro torna fuori, e a fuoco vivo l'ossigeno risale e il fumo si mescola coi vicini. Il posto di comando e i
    // compartimenti perduti non si isolano (isolabile).
    bool isolabile(int compartimento) const;
    void isola(int compartimento);
    void chiudi_subito(int compartimento);
    void riapri(int compartimento);
    // Compartimenti tecnici senza personale e macchine comandate dal posto centrale (Alfa, dossier 19 §3.1-3.2): si
    // chiudono subito, nessuno resta dentro, il motore non ne risente
    void senza_personale(bool si) { senza_personale_ = si; }
    int caduti() const { return caduti_; }   // uomini morti nei compartimenti isolati
""")
sost(H, """    void passo_incendi(double h, double quota);
""", """    void passo_incendi(double h, double quota);
    void passo_isolamento();
    int uomini_in(const CompartimentoBattello& c) const;   // di guardia nel compartimento (STIMA)
""")
sost(H, """    std::vector<Contenitori> contenitori_;
};
""", """    std::vector<Contenitori> contenitori_;
    bool senza_personale_ = false;
    int caduti_ = 0;
};
""")

C = 'nucleo/src/battello.cpp'
sost(C, """constexpr double BATTERIA_S = 7200.0;              // la batteria di riserva di un nucleare: 2 ore al motore d'emergenza
""", """constexpr double BATTERIA_S = 7200.0;              // la batteria di riserva di un nucleare: 2 ore al motore d'emergenza
// isolamento di un compartimento (STIMA, docs/10 §10.19)
constexpr double SGOMBERO_S = 60.0;                // fuori tutti, poi le porte
constexpr double CONTAGIO_ISOLATO = 0.25;          // il fuoco passa meno ai vicini, ma passa (Komsomolets, 1989)
constexpr double OSSIGENO_RIAPERTO = 0.16;         // riaprendo a fuoco vivo
constexpr int UOMINI_MACCHINE = 19, UOMINI_LOCALI = 10;   // di guardia, sui 129 del 688 (dossier 01); reattore e batterie vuoti
""")
sost(C, """        if (c.fuoco <= 0.0 || c.imploso) {   // a fuoco spento: ventilazione e ossigeno che torna
            if (!(c.uso == "batterie" && c.acqua > 0.05 * c.volume)) c.fumo = std::max(0.0, c.fumo - c.fumo * h / VENTILAZIONE_S);
""", """        if (c.fuoco <= 0.0 || c.imploso) {   // a fuoco spento: ventilazione e ossigeno che torna
            if (c.isolato && !c.imploso) continue;   // finche' non si riapre
            if (!(c.uso == "batterie" && c.acqua > 0.05 * c.volume)) c.fumo = std::max(0.0, c.fumo - c.fumo * h / VENTILAZIONE_S);
""")
sost(C, """        if (c.fuoco > 0.7 && !c.aria_nell_incendio && caso_.uniforme() < h / 1200.0) incendio(ic, c.fuoco, true);
""", """        // (a porte chiuse il ramo dell'aria e' chiuso: se non e' gia' rotto non si rompe)
        if (c.fuoco > 0.7 && !c.aria_nell_incendio && caso_.uniforme() < h / 1200.0 && !c.isolato) incendio(ic, c.fuoco, true);
""")
sost(C, """                if (comp_[static_cast<std::size_t>(v)].fuoco <= 0.0 && caso_.uniforme() < h / 900.0) incendio(v, 0.1);
""", """                if (comp_[static_cast<std::size_t>(v)].fuoco <= 0.0 &&
                    caso_.uniforme() < (c.isolato || comp_[static_cast<std::size_t>(v)].isolato ? CONTAGIO_ISOLATO : 1.0) * h / 900.0) incendio(v, 0.1);
""")
sost(C, """void BattelloDanni::priorita(Sistema s) {
    priorita_.erase(std::remove(priorita_.begin(), priorita_.end(), s), priorita_.end());
    priorita_.insert(priorita_.begin(), s);
}
""", """void BattelloDanni::priorita(Sistema s) {
    priorita_.erase(std::remove(priorita_.begin(), priorita_.end(), s), priorita_.end());
    priorita_.insert(priorita_.begin(), s);
}

int BattelloDanni::uomini_in(const CompartimentoBattello& c) const {
    if (senza_personale_ || c.uso == "reattore" || c.uso == "batterie") return 0;
    return c.uso == "macchine" ? UOMINI_MACCHINE : UOMINI_LOCALI;
}

bool BattelloDanni::isolabile(int compartimento) const {
    if (compartimento < 0 || compartimento >= static_cast<int>(comp_.size()) || perso_) return false;
    const auto& c = comp_[static_cast<std::size_t>(compartimento)];
    return compartimento != compartimento_di(Sistema::inerziale) && !c.imploso && !c.distrutto;
}

void BattelloDanni::isola(int compartimento) {
    if (!isolabile(compartimento)) return;
    auto& c = comp_[static_cast<std::size_t>(compartimento)];
    if (c.senza_squadre()) return;
    if (senza_personale_) {
        c.isolato = true;
        evento(EventoBattello::Tipo::isolamento, compartimento, "compartimento " + c.nome + " isolato");
    } else {
        c.porte_s = tempo_ + SGOMBERO_S;
        evento(EventoBattello::Tipo::sgombero, compartimento, "sgombero del compartimento " + c.nome + ": porte chiuse fra un minuto");
    }
}

void BattelloDanni::chiudi_subito(int compartimento) {
    if (!isolabile(compartimento)) return;
    auto& c = comp_[static_cast<std::size_t>(compartimento)];
    if (c.porte_s < 0.0) return;
    c.dentro = static_cast<int>(std::ceil(uomini_in(c) * std::max(c.porte_s - tempo_, 0.0) / SGOMBERO_S));   // escono a ritmo costante
    c.porte_s = -1.0;
    c.isolato = true;
    char t[96];
    std::snprintf(t, sizeof t, "compartimento %s isolato, %d uomini dentro", c.nome.c_str(), c.dentro);
    evento(EventoBattello::Tipo::isolamento, compartimento, t);
}

void BattelloDanni::riapri(int compartimento) {
    if (compartimento < 0 || compartimento >= static_cast<int>(comp_.size())) return;
    auto& c = comp_[static_cast<std::size_t>(compartimento)];
    if (!c.senza_squadre()) return;
    if (c.isolato && c.fuoco > 0.0) {
        c.ossigeno = std::max(c.ossigeno, OSSIGENO_RIAPERTO);
        for (const int v : vicini(compartimento)) {
            auto& w = comp_[static_cast<std::size_t>(v)];
            c.fumo = w.fumo = (c.fumo + w.fumo) / 2;
        }
    }
    c.isolato = false;
    c.porte_s = -1.0;
    c.dentro = 0;
    evento(EventoBattello::Tipo::riapertura, compartimento, "compartimento " + c.nome + " riaperto");
}

// Le porte alla fine dello sgombero; chi e' rimasto dentro muore quando il compartimento diventa letale
void BattelloDanni::passo_isolamento() {
    for (std::size_t i = 0; i < comp_.size(); ++i) {
        auto& c = comp_[i];
        if (c.porte_s >= 0.0 && tempo_ >= c.porte_s) {
            c.porte_s = -1.0;
            c.isolato = true;
            evento(EventoBattello::Tipo::isolamento, static_cast<int>(i), "compartimento " + c.nome + " isolato");
        }
        if (c.dentro > 0 && (c.fumo >= 0.95 || c.ossigeno < OSSIGENO_MINIMO || c.acqua > 0.5 * c.volume || c.imploso || c.distrutto)) {
            char t[96];
            std::snprintf(t, sizeof t, "perduti i %d uomini chiusi nel compartimento %s", c.dentro, c.nome.c_str());
            evento(EventoBattello::Tipo::caduti, static_cast<int>(i), t);
            eventi_.back().uomini = c.dentro;
            caduti_ += c.dentro;
            c.dentro = 0;
        }
    }
}
""")
sost(C, """    if (!l.energia) l.velocita = 0.0;   // reattore fermo e batteria scarica: niente motore d'emergenza
""", """    if (!l.energia) l.velocita = 0.0;   // reattore fermo e batteria scarica: niente motore d'emergenza
    if (!senza_personale_)   // macchine chiuse e abbandonate: si va col motore d'emergenza
        for (const auto& c : comp_)
            if (c.isolato && c.uso == "macchine") l.velocita = std::min(l.velocita, VELOCITA_EMERGENZA);
""")
sost(C, """    auto abitabile = [](const CompartimentoBattello& c) {
        return !c.imploso && !c.distrutto && c.acqua < 0.5 * c.volume && c.fumo < 0.95;
    };
""", """    auto abitabile = [](const CompartimentoBattello& c) {
        return !c.imploso && !c.distrutto && c.acqua < 0.5 * c.volume && c.fumo < 0.95 && !c.senza_squadre();
    };
""")
sost(C, """        if (f.area <= 0 || f.area > AREA_TAMPONABILE) continue;
        f.area -= TAMPONATURA""", """        if (f.area <= 0 || f.area > AREA_TAMPONABILE || comp_[static_cast<std::size_t>(f.compartimento)].senza_squadre()) continue;
        f.area -= TAMPONATURA""")
sost(C, """    for (const auto& c : comp_) focolai += c.fuoco > 0.0 && !c.imploso && !c.distrutto;
""", """    for (const auto& c : comp_) focolai += c.fuoco > 0.0 && !c.imploso && !c.distrutto && !c.senza_squadre();
""")
sost(C, """        if (c.fuoco <= 0.0 || c.imploso || c.distrutto) continue;
        const int k = std::max(1, squadre / focolai);
""", """        if (c.fuoco <= 0.0 || c.imploso || c.distrutto || c.senza_squadre()) continue;
        const int k = std::max(1, squadre / focolai);
""")
sost(C, """        passo_incendi(h, std::max(moto.quota_m, 0.0));
        equipaggio(h, std::max(moto.quota_m, 0.0));
""", """        passo_incendi(h, std::max(moto.quota_m, 0.0));
        passo_isolamento();
        equipaggio(h, std::max(moto.quota_m, 0.0));
""")

# ---- nucleo: gli ordini
sost('nucleo/include/nucleo/ordini.hpp', """    luci_giocatore,         // le luci di navigazione del giocatore (docs/27): a = 1 accese, 0 spente
};
""", """    luci_giocatore,         // le luci di navigazione del giocatore (docs/27): a = 1 accese, 0 spente
    // il controllo danni dell'Enhanced (docs/10 §10.19; BattelloDanni)
    isola_compartimento,    // n: compartimento, a: 0 sgombero e porte, 1 chiude subito, 2 riapre
    priorita_sistema,       // n: Sistema da portare in cima alle riparazioni
};
""")
sost('nucleo/src/ordini.cpp', 'constexpr std::array<std::pair<TipoOrdine, std::string_view>, 64> NOMI = {{\n', 'constexpr std::array<std::pair<TipoOrdine, std::string_view>, 66> NOMI = {{\n')
sost('nucleo/src/ordini.cpp', """    {TipoOrdine::luci_giocatore, "luci_giocatore"},
""", """    {TipoOrdine::luci_giocatore, "luci_giocatore"},
    {TipoOrdine::isola_compartimento, "isola_compartimento"},
    {TipoOrdine::priorita_sistema, "priorita_sistema"},
""")

# ---- missione: la consegna, i messaggi, i caduti
sost('missioni/src/missione.cpp', """        case TipoOrdine::compensa:
            return false;
""", """        case TipoOrdine::compensa:
        case TipoOrdine::isola_compartimento:   // il controllo danni: DanniSottomarini::consegna
        case TipoOrdine::priorita_sistema:
            return false;
""")
D = 'missioni/src/danni_sottomarini.cpp'
sost(D, """constexpr int MSG_SFOGHI_APERTI = 449;
""", """constexpr int MSG_SFOGHI_APERTI = 449;
constexpr int MSG_SGOMBERO = 415, MSG_ISOLATO = 416, MSG_RIAPERTO = 417, MSG_COMANDO_NON_ISOLABILE = 418, MSG_UOMINI_PERDUTI = 419;
""")
sost(D, """bool DanniSottomarini::consegna(const Missione& m, const Ordine& o) {
    if (o.tipo != TipoOrdine::sfoga""", """bool DanniSottomarini::consegna(const Missione& m, const Ordine& o) {
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
        } else {
            b.isola(o.n);
        }
        return true;
    }
    if (o.tipo != TipoOrdine::sfoga""")
sost(D, """    s->danni->imposta_tempo(tempo);   // eventi e segni nel tempo di missione
""", """    s->danni->imposta_tempo(tempo);   // eventi e segni nel tempo di missione
    if (u.classe == "alfa") s->danni->senza_personale(true);   // dossier 19 §3.1-3.2
""")
sost(D, """                case T::aria_nell_incendio: codice = msg_sottomarino::ARIA_NELL_INCENDIO; break;
""", """                case T::aria_nell_incendio: codice = msg_sottomarino::ARIA_NELL_INCENDIO; break;
                case T::sgombero: codice = MSG_SGOMBERO; break;
                case T::isolamento: codice = MSG_ISOLATO; break;
                case T::riapertura: codice = MSG_RIAPERTO; break;
                case T::caduti: codice = MSG_UOMINI_PERDUTI; break;
""")
R = 'missioni/src/registro_missione.cpp'
sost(R, """        case T::cassa_squarciata: return "ballast tank ruptured";
    }
    return "damage";
""", """        case T::cassa_squarciata: return "ballast tank ruptured";
        case T::sgombero: return "compartment evacuated";
        case T::isolamento: return "compartment isolated";
        case T::riapertura: return "compartment reopened";
        case T::caduti: return "men lost in an isolated compartment";
    }
    return "damage";
""")
sost(R, """        case T::cassa_squarciata: return 0.5;   // l'aria della cassa che non tiene: il battello pesa (kit)
""", """        case T::cassa_squarciata: return 0.5;   // l'aria della cassa che non tiene: il battello pesa (kit)
        case T::caduti: return 0.8;
""")
sost(R, """        e.testo_en = std::string{nome_tipo_battello(r.tipo)};
        danno_massimo_ = std::max(danno_massimo_, e.valore);
""", """        e.testo_en = std::string{nome_tipo_battello(r.tipo)};
        if (r.tipo == T::caduti) e.livello = r.uomini;
        danno_massimo_ = std::max(danno_massimo_, e.valore);
""")
sost(R, """            case TipoEvento::scram: r.danno_nucleare = true; break;
""", """            case TipoEvento::scram: r.danno_nucleare = true; break;
            case TipoEvento::danno_subito:   // i soli caduti che il modello conosce: chi e' rimasto in un compartimento isolato
                if (e.codice == static_cast<int>(EventoBattello::Tipo::caduti)) r.morti += e.livello;
                break;
""")

# ---- interfaccia: F3
IH = 'app/interfaccia.hpp'
sost(IH, """    int dettaglio_danni_ = -1;                     // compartimento ingrandito: -1 il piu' in pericolo, o scelto con Tab
""", """    int dettaglio_danni_ = -1;                     // compartimento ingrandito: -1 il piu' in pericolo, o scelto con Tab
    // il controllo danni (docs/10 §10.19): 1 isola, chiude subito o riapre il compartimento mostrato, 5 la pagina delle squadre,
    // e li' 2 e 3 scelgono un sistema, 4 lo porta in cima alle riparazioni
    bool pagina_squadre_ = false;
    int sistema_scelto_ = 0;
    int compartimento_danni() const;               // quello mostrato: dettaglio_danni_ o il piu' in pericolo
    void pulsante_squadre(int n, double t);
""")
I = 'app/interfaccia.cpp'
sost(I, """#include "grafica/palette.hpp"
""", """#include "grafica/pagina_squadre.hpp"
#include "grafica/palette.hpp"
""")
sost(I, """        const int n = spaccato_->compartimenti();
        dettaglio_danni_ = dettaglio_danni_ + 1 >= n ? -1 : dettaglio_danni_ + 1;
""", """        const int n = spaccato_->compartimenti();
        if (c.tab > 0) dettaglio_danni_ = dettaglio_danni_ + 1 >= n ? -1 : dettaglio_danni_ + 1;
        else dettaglio_danni_ = dettaglio_danni_ < 0 ? n - 1 : dettaglio_danni_ - 1;
""")
sost(I, """            if (n == 4) misure_fondale_ = !misure_fondale_;
        } else {
            cifra(n);
""", """            if (n == 4) misure_fondale_ = !misure_fondale_;
        } else if (stazione_ == 3 && st_.danni() && n >= 1 && n <= 5) {
            pulsante_squadre(n, t);
        } else {
            cifra(n);
""")
sost(I, """// Quadro danni (D6d): spaccato dei compartimenti con l'acqua imbarcata e l'elenco dei rapporti dell'equipaggio
void InterfacciaMissione::disegna_danni(Tela& t, const Font& f) const {
""", """int InterfacciaMissione::compartimento_danni() const {
    if (dettaglio_danni_ >= 0) return dettaglio_danni_;
    int scelto = -1;
    if (const auto* comp = st_.danni()->compartimenti(m_.giocatore()->id)) {
        double peggio = -1.0;
        for (std::size_t i = 0; i < comp->size(); ++i) {
            const auto& c = (*comp)[i];
            const double rischio = (c.volume > 0.0 ? c.acqua / c.volume : 0.0) + c.fuoco + 0.5 * c.fumo + (c.distrutto ? 2.0 : 0.0);
            if (rischio > peggio) { peggio = rischio; scelto = static_cast<int>(i); }
        }
    }
    return scelto;
}

void InterfacciaMissione::pulsante_squadre(int n, double t) {
    const SottomarinoColpito* sc = st_.danni()->sottomarino(m_.giocatore()->id);
    if (!sc || !sc->danni) return;
    const BattelloDanni& b = *sc->danni;
    const int ultimo = static_cast<int>(b.ordine_riparazioni().size()) - 1;
    if (n == 5) {
        pagina_squadre_ = !pagina_squadre_;
    } else if (n == 1) {
        const int k = compartimento_danni();
        if (k < 0) return;
        dettaglio_danni_ = k;   // il secondo comando resta su questo compartimento, anche se un altro diventa il piu' colpito
        const auto& c = b.compartimenti()[static_cast<std::size_t>(k)];
        ordina({t, TipoOrdine::isola_compartimento, c.isolato ? 2.0 : c.porte_s >= 0.0 ? 1.0 : 0.0, 0.0, k});
    } else if (pagina_squadre_) {
        if (n == 2) sistema_scelto_ = std::max(sistema_scelto_ - 1, 0);
        if (n == 3) sistema_scelto_ = std::min(sistema_scelto_ + 1, ultimo);
        if (n == 4) {
            ordina({t, TipoOrdine::priorita_sistema, 0.0, 0.0, static_cast<int>(b.ordine_riparazioni()[static_cast<std::size_t>(sistema_scelto_)])});
            sistema_scelto_ = 0;
        }
    }
}

namespace {
// I dati della pagina delle squadre e dei pulsanti (grafica/pagina_squadre del kit), dallo stato del battello
DatiPaginaSquadre dati_squadre(const BattelloDanni& b, int scelto, int cursore, bool alfa) {
    static constexpr const char* OTSEK[] = {"I TORP.", "II AKK.", "III TSP", "IV REA.", "V TURB.", "VI VSP."};
    DatiPaginaSquadre d;
    d.alfa = alfa;
    d.cursore = cursore;
    d.scelto = std::max(scelto, 0);
    d.squadre = d.libere = b.squadre_disponibili();
    for (const auto& q : b.squadre()) d.libere -= q.numero;
    for (const Sistema s : b.ordine_riparazioni()) {
        RigaSquadre r;
        r.nome = nome_sistema(s);
        r.efficienza = b.sistema(s).efficienza;
        r.massimo = b.sistema(s).massimo;
        for (const auto& q : b.squadre())
            if (q.compito == SquadraBattello::Compito::riparare && q.sistema == static_cast<int>(s)) r.squadre += q.numero;
        d.righe.push_back(std::move(r));
    }
    const int comando = b.compartimento_di(Sistema::inerziale);
    for (std::size_t i = 0; i < b.compartimenti().size(); ++i) {
        const auto& c = b.compartimenti()[i];
        PortaOtsek p;
        if (alfa && i < std::size(OTSEK)) {
            p.nome = OTSEK[i];
        } else {
            p.nome = c.nome + " " + (static_cast<int>(i) == comando ? "COMANDO" : c.uso);
            for (auto& ch : p.nome) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        }
        using S = PortaOtsek::Stato;
        p.stato = c.isolato ? S::chiuso : c.porte_s >= 0.0 ? S::in_chiusura : static_cast<int>(i) == comando ? S::comando : S::aperto;
        if (c.porte_s >= 0.0) p.conto_s = static_cast<int>(std::ceil(c.porte_s - b.tempo()));
        p.uomini_dentro = c.dentro;
        p.acqua = c.volume > 0.0 ? c.acqua / c.volume : 0.0;
        p.fuoco = c.fuoco;
        p.ossigeno = c.ossigeno;
        for (const auto& q : b.squadre())
            if (q.compartimento == static_cast<int>(i)) p.squadre += q.numero;
        d.porte.push_back(std::move(p));
    }
    return d;
}
}  // namespace

// Quadro danni (D6d): spaccato dei compartimenti con l'acqua imbarcata e l'elenco dei rapporti dell'equipaggio
void InterfacciaMissione::disegna_danni(Tela& t, const Font& f) const {
""")
sost(I, """        spaccato_->disegna_didascalie(t, f, intero, didascalie, stato);
""", """        spaccato_->disegna_didascalie(t, f, intero, didascalie, stato);
        const int scelto = compartimento_danni();
        const DatiPaginaSquadre squadre = dati_squadre(*sc->danni, scelto, sistema_scelto_, alfa());
        const Font& piccolo = piccolo_ ? *piccolo_ : f;
        disegna_porte_spaccato(t, f, *spaccato_, intero, squadre);
""")
sost(I, """        int scelto = dettaglio_danni_;
        if (scelto < 0 && stato.compartimenti) {
            double peggio = -1.0;
            for (std::size_t i = 0; i < stato.compartimenti->size(); ++i) {
                const auto& c = (*stato.compartimenti)[i];
                const double rischio = (c.volume > 0.0 ? c.acqua / c.volume : 0.0) + c.fuoco + 0.5 * c.fumo + (c.distrutto ? 2.0 : 0.0);
                if (rischio > peggio) { peggio = rischio; scelto = static_cast<int>(i); }
            }
        }
        double x0 = 0.0, x1 = 0.0;
        if (scelto >= 0 && spaccato_->estensione(scelto, x0, x1)) {
            spaccato_->disegna_dettaglio(t, {4, 164, LARGHEZZA_TELA - 8, 148}, stato, x0, x1);
""", """        double x0 = 0.0, x1 = 0.0;
        if (scelto >= 0 && spaccato_->estensione(scelto, x0, x1)) {
            if (pagina_squadre_) disegna_pagina_squadre(t, piccolo, {4, 164, LARGHEZZA_TELA - 8, 148}, squadre);
            else spaccato_->disegna_dettaglio(t, {4, 164, LARGHEZZA_TELA - 8, 148}, stato, x0, x1);
""")
sost(I, """            f.scrivi(t, 8, 314, riga, colori::BIANCO, 1);
        }
        const auto& r = st_.danni()->rapporti(id);   // gli ultimi tre rapporti
""", """            f.scrivi(t, 8, 314, riga, colori::BIANCO, 1);
        }
        std::vector<AreaPulsante> tasti;
        disegna_tasti_squadre(t, piccolo, LARGHEZZA_TELA - 4, 312, squadre, pagina_squadre_, &tasti);
        for (const auto& [r, n] : tasti) aree_vista_.push_back({r.x, r.y, r.w, r.h, "pulsante_" + std::to_string(n)});
        const auto& r = st_.danni()->rapporti(id);   // gli ultimi tre rapporti
""")

# ---- le prove
sost('test/prove_battello.cpp', """TEST_CASE("battello: danni critici, fase 3 (carico vivo""", """TEST_CASE("battello: isolare un compartimento e le priorita' delle squadre (docs/10 §10.19)" * doctest::skip(!ci_sono_sottomarini())) {
    const auto d688 = DatiScafo::carica(SCAFI_SUB + "los_angeles.toml");
    const auto dalfa = DatiScafo::carica(SCAFI_SUB + "alfa.toml");
    const auto avanza = [](BattelloDanni& b, int secondi) { for (int t = 0; t < secondi; ++t) b.avanza(1.0, {100.0, 5.0}); };
    SUBCASE("il posto di comando non si isola") {
        BattelloDanni b(d688, 1);
        CHECK_FALSE(b.isolabile(0));
        CHECK(b.isolabile(1));
        CHECK(b.isolabile(2));
        b.isola(0);
        CHECK_FALSE(b.compartimenti()[0].senza_squadre());
        BattelloDanni a(dalfa, 1);
        a.senza_personale(true);
        CHECK_FALSE(a.isolabile(2));
        for (const int i : {0, 1, 3, 4, 5}) CHECK(a.isolabile(i));
    }
    SUBCASE("688: le squadre escono subito, le porte dopo un minuto, poi il motore al 15%") {
        BattelloDanni b(d688, 1);
        b.incendio(2, 0.8);
        avanza(b, 1);
        bool dentro = false;
        for (const auto& l : b.squadre()) dentro |= l.compartimento == 2 && l.compito == SquadraBattello::Compito::spegnere;
        CHECK(dentro);
        b.isola(2);
        avanza(b, 1);
        for (const auto& l : b.squadre()) CHECK(l.compartimento != 2);
        CHECK(b.squadre_disponibili() == 2);
        CHECK(b.limiti().velocita > 0.15);   // sgombero in corso
        CHECK_FALSE(b.compartimenti()[2].isolato);
        avanza(b, 60);
        CHECK(b.compartimenti()[2].isolato);
        CHECK(b.compartimenti()[2].dentro == 0);
        CHECK(b.limiti().velocita <= 0.15);
        b.riapri(2);
        CHECK_FALSE(b.compartimenti()[2].senza_squadre());
        CHECK(b.caduti() == 0);
    }
    SUBCASE("Alfa: si chiude subito, nessuno dentro, il motore non ne risente") {
        BattelloDanni b(dalfa, 1);
        b.senza_personale(true);
        b.isola(4);
        CHECK(b.compartimenti()[4].isolato);
        CHECK(b.compartimenti()[4].dentro == 0);
        CHECK(b.limiti().velocita == 1.0);
    }
    SUBCASE("a porte chiuse il fuoco si spegne senza ossigeno, l'aria non torna finche' non si riapre") {
        BattelloDanni b(d688, 3);
        b.incendio(2, 0.8);
        b.isola(2);
        avanza(b, 1800);
        const auto& c = b.compartimenti()[2];
        CHECK(c.fuoco == 0.0);
        CHECK(c.ossigeno < 0.13);
        const double fumo = c.fumo;
        avanza(b, 300);
        CHECK(b.compartimenti()[2].ossigeno < 0.13);
        CHECK(b.compartimenti()[2].fumo == fumo);
        const double ossigeno = b.compartimenti()[2].ossigeno;
        b.riapri(2);
        avanza(b, 300);
        CHECK(b.compartimenti()[2].ossigeno > ossigeno);
        CHECK(b.compartimenti()[2].fumo < fumo);
    }
    SUBCASE("riaprire a fuoco vivo: l'ossigeno risale al 16%, il fumo passa ai vicini") {
        BattelloDanni b(d688, 3);
        b.incendio(2, 0.8);
        b.isola(2);
        avanza(b, 600);
        REQUIRE(b.compartimenti()[2].fuoco > 0.0);
        REQUIRE(b.compartimenti()[2].ossigeno < 0.16);
        const double fumo = b.compartimenti()[2].fumo, vicino = b.compartimenti()[1].fumo;
        b.riapri(2);
        CHECK(b.compartimenti()[2].ossigeno == doctest::Approx(0.16));
        CHECK(b.compartimenti()[2].fumo < fumo);
        CHECK(b.compartimenti()[1].fumo > vicino);
    }
    SUBCASE("le porte chiuse in tempo fermano il tubo dell'aria, il contagio resta possibile a un quarto") {
        int tubi[2] = {0, 0}, contagi[2] = {0, 0};
        for (std::uint64_t seme = 1; seme <= 40; ++seme)
            for (const int isola : {0, 1}) {
                BattelloDanni b(dalfa, seme);
                b.senza_personale(true);
                b.incendio(4, 1.0);
                if (isola) b.isola(4);
                avanza(b, 1200);
                for (const auto& e : b.eventi()) {
                    tubi[isola] += e.tipo == EventoBattello::Tipo::aria_nell_incendio;
                    contagi[isola] += e.tipo == EventoBattello::Tipo::incendio && e.compartimento != 4;
                }
            }
        CHECK(tubi[0] > 0);
        CHECK(tubi[1] == 0);
        CHECK(contagi[1] < contagi[0]);
    }
    SUBCASE("contro l'acqua isolare non ferma niente") {
        double acqua[2] = {0.0, 0.0};
        for (const int isola : {0, 1}) {
            BattelloDanni b(d688, 1);
            b.aggiungi_falla(2, 0.02);
            if (isola) b.isola(2);
            avanza(b, 600);
            acqua[isola] = b.compartimenti()[2].acqua;
        }
        CHECK(acqua[1] == doctest::Approx(acqua[0]).epsilon(0.01));
    }
    SUBCASE("chi resta dentro: muore quando il compartimento e' letale, torna fuori se si riapre prima") {
        for (const bool riapre : {false, true}) {
            BattelloDanni b(d688, 1);
            b.incendio(2, 0.8);
            b.isola(2);
            avanza(b, 30);
            b.chiudi_subito(2);
            CHECK(b.compartimenti()[2].isolato);
            CHECK(b.compartimenti()[2].dentro == doctest::Approx(10).epsilon(0.11));   // 19 uomini, meta' sgombero
            const int dentro = b.compartimenti()[2].dentro;
            if (riapre) { avanza(b, 60); b.riapri(2); }
            avanza(b, 1200);
            CHECK(b.caduti() == (riapre ? 0 : dentro));
            int eventi = 0;
            for (const auto& e : b.eventi()) if (e.tipo == EventoBattello::Tipo::caduti) { ++eventi; CHECK(e.uomini == dentro); }
            CHECK(eventi == (riapre ? 0 : 1));
        }
    }
    SUBCASE("le priorita': una squadra sola va sul sistema in cima") {
        auto d = d688;
        d.squadre = 1;
        for (const bool sonar : {false, true}) {
            BattelloDanni b(d, 1);
            b.guasto_sistema(Sistema::timone, 0.5, "prova");
            b.guasto_sistema(Sistema::sonar_prua, 0.5, "prova");
            if (sonar) b.priorita(Sistema::sonar_prua);
            CHECK((b.ordine_riparazioni().front() == Sistema::sonar_prua) == sonar);
            avanza(b, 60);
            CHECK((b.sistema(Sistema::sonar_prua).efficienza > 0.5) == sonar);
            CHECK((b.sistema(Sistema::timone).efficienza > 0.5) == !sonar);
        }
    }
}

TEST_CASE("battello: danni critici, fase 3 (carico vivo""")
sost('test/prove_danni_sottomarini.cpp', """TEST_CASE("sottomarini: casse d'assetto per posarsi sul fondo\"""", """TEST_CASE("sottomarini: isolare un compartimento, l'ordine del giocatore (docs/10 §10.19)" * doctest::skip(!ci_sono_dati())) {
    const auto classi = carica_classi(std::string(M688_DATI) + "/classi.toml");
    CHECK(ordine_da_nome(nome_ordine(TipoOrdine::isola_compartimento)) == TipoOrdine::isola_compartimento);
    CHECK(ordine_da_nome(nome_ordine(TipoOrdine::priorita_sistema)) == TipoOrdine::priorita_sistema);
    const auto c_e_msg = [](const std::vector<Messaggio>& v, int codice) {
        return std::any_of(v.begin(), v.end(), [&](const Messaggio& x) { return x.codice == codice; });
    };
    SUBCASE("688: rifiuto del comando, sgombero, porte, caduti nell'esito") {
        const auto f = missione_sub("{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
                                    "  posizione = { x = 0, y = 0 }, quota_m = 100, rotta = 0, velocita_kn = 5, " + MODELLO + "160 } },", "isola");
        Missione m(f, 1, false);
        m.impatti_dell_originale(false);
        DanniSottomarini ds(classi, M688_DATI, 1);
        ds.prepara(m, 0);
        CHECK(ds.consegna(m, {0.0, TipoOrdine::isola_compartimento, 0.0, 0.0, 0}));
        gioca(m, ds, 1);
        CHECK(c_e_msg(ds.messaggi(), 418));
        CHECK_FALSE((*ds.compartimenti(0))[0].senza_squadre());
        ds.incendio(m, 0, 2, 0.8);
        CHECK(ds.consegna(m, {0.0, TipoOrdine::isola_compartimento, 0.0, 0.0, 2}));
        CHECK((*ds.compartimenti(0))[2].porte_s >= 0.0);
        gioca(m, ds, 30);
        CHECK(c_e_msg(ds.messaggi(), 415));
        CHECK(ds.consegna(m, {0.0, TipoOrdine::isola_compartimento, 1.0, 0.0, 2}));
        const int dentro = (*ds.compartimenti(0))[2].dentro;
        CHECK(dentro > 0);
        for (int i = 0; i < 1200; ++i) {
            gioca(m, ds, 1);
            m.raccogli_danni(&ds, nullptr);
        }
        CHECK(c_e_msg(ds.messaggi(), 416));
        CHECK(c_e_msg(ds.messaggi(), 419));
        CHECK(ds.sottomarino(0)->danni->caduti() == dentro);
        CHECK(m.esito_dettagliato().morti == dentro);
        CHECK(ds.consegna(m, {0.0, TipoOrdine::isola_compartimento, 2.0, 0.0, 2}));
        gioca(m, ds, 1);
        CHECK(c_e_msg(ds.messaggi(), 417));
        CHECK(ds.consegna(m, {0.0, TipoOrdine::priorita_sistema, 0.0, 0.0, static_cast<int>(Sistema::sonar_prua)}));
        CHECK(ds.sottomarino(0)->danni->ordine_riparazioni().front() == Sistema::sonar_prua);
        fs::remove(f);
    }
    SUBCASE("l'Alfa si chiude subito, senza conto") {
        const auto f = missione_sub("{ id = 0, classe = 'alfa', categoria = 'sottomarino', flag = 9, giocatore = true,"
                                    "  posizione = { x = 0, y = 0 }, quota_m = 100, rotta = 0, velocita_kn = 5, " + MODELLO + "160 } },", "isola_alfa");
        Missione m(f, 1, false);
        m.impatti_dell_originale(false);
        DanniSottomarini ds(classi, M688_DATI, 1);
        ds.prepara(m, 0);
        CHECK(ds.consegna(m, {0.0, TipoOrdine::isola_compartimento, 0.0, 0.0, 4}));
        CHECK((*ds.compartimenti(0))[4].isolato);
        gioca(m, ds, 1);
        CHECK(c_e_msg(ds.messaggi(), 416));
        CHECK_FALSE(c_e_msg(ds.messaggi(), 415));
        fs::remove(f);
    }
}

TEST_CASE("sottomarini: casse d'assetto per posarsi sul fondo\"""")

# ---- i messaggi (fuori dalla cartella engine: solo con --tutto, nelle copie private tools e' il collegamento a quello vero)
E = '../tools/pipeline/esporta_motore.py'
sost(E, """    460: 'Sonar: un\\'arma in acqua a rilevamento {RIL}, non è diretta a noi.',""", """    # il controllo danni dell'Enhanced (docs/10 §10.19; DanniSottomarini)
    415: 'Sgombero del compartimento: si chiudono le porte fra un minuto, {COMANDANTE}.',
    416: 'Compartimento isolato: porte chiuse.',
    417: 'Compartimento riaperto, le squadre possono rientrare.',
    418: 'Il posto di comando non si isola, {COMANDANTE}.',
    419: 'Gli uomini chiusi nel compartimento sono perduti, {COMANDANTE}.',
    460: 'Sonar: un\\'arma in acqua a rilevamento {RIL}, non è diretta a noi.',""")


sost('../docs/10_danni_affondamento_esplosioni.md', """Prove: `test/prove_danni_sottomarini.cpp`, i due casi «Alfa, casse».
""", """Prove: `test/prove_danni_sottomarini.cpp`, i due casi «Alfa, casse».

### 10.19 Priorita' delle squadre e isolamento di un compartimento (06/11/2026)

Proposta con le misure e le fonti: nota condivisa `punto4-isolamento-proposta-06-11.md`; misure in `tools/re/prove/isola/`. Solo nell'Enhanced e, per
ora, solo il giocatore (decisione dell'utente: l'IA in un secondo passo, docs/18). Una partita che non usa questi ordini resta identica.

- **Perche' serve:** un incendio esteso le squadre non lo accorciano (688 macchine, fuoco 0,8: 1656 s con le squadre, 1618 s senza), ma ci restano tutte e
  nessuno ripara altro. Uno piccolo lo vincono (66 s contro 1967): isolare troppo presto e' un errore.
- **`BattelloDanni::isola`**: le squadre escono subito e il compartimento non conta piu' come abitabile; le porte si chiudono dopo 60 s (STIMA). `chiudi_subito`,
  durante il conto: chi non e' uscito resta dentro. `riapri`: chi e' dentro torna fuori; a fuoco vivo l'ossigeno risale al 16% (STIMA) e il fumo si mescola coi vicini.
- **A porte chiuse:** nessuna squadra, ne' tamponatura ne' riparazioni; il fuoco consuma l'ossigeno come prima; contagio ai vicini a un quarto (STIMA); il ramo
  dell'aria si chiude, se non e' gia' rotto; fumo e ossigeno non si ripristinano; le pompe continuano. **Paratie, falle e pressione non cambiano: contro l'acqua
  isolare non ferma niente.**
- **Il posto di comando non si isola** (`compartimento_di(Sistema::inerziale)`: C1 del 688, III dell'Alfa), rifiutato col messaggio 418.
- **688 e Alfa:** sul 688 19 uomini in macchine, nessuno nel reattore (STIMA); le macchine chiuse limitano il motore al 15%. Sull'Alfa (`senza_personale`,
  dossier 19 §3.1-3.2) i compartimenti tecnici si chiudono subito, nessuno dentro, nessun limite.
- **Chi resta dentro** muore quando il compartimento diventa letale (fumo pieno, ossigeno sotto il 12%, acqua oltre meta'), e conta in `EsitoDettagliato::morti`:
  sono i soli caduti che il modello conosce, perche' l'equipaggio uomo per uomo non c'e'.
- **Ordini:** `isola_compartimento` (n compartimento, a: 0 sgombero, 1 chiude subito, 2 riapre), `priorita_sistema` (n: Sistema). Messaggi 415 sgombero, 416 isolato,
  417 riaperto, 418 rifiuto, 419 uomini perduti.
- **Stazione F3:** 1 agisce sul compartimento mostrato, 5 apre la pagina delle squadre (`grafica/pagina_squadre`, disegno del kit), e li' 2 e 3 scelgono un
  sistema, 4 lo porta in cima alle riparazioni.

Prove: `test/prove_battello.cpp`, «isolare un compartimento»; `test/prove_danni_sottomarini.cpp`, «isolare un compartimento».
""")


sost('../data/motore/testi/messaggi_bordo.tsv', """460\tSonar: un'arma in acqua a rilevamento {RIL}, non è diretta a noi.\n""", """415\tSgombero del compartimento: si chiudono le porte fra un minuto, {COMANDANTE}.
416\tCompartimento isolato: porte chiuse.
417\tCompartimento riaperto, le squadre possono rientrare.
418\tIl posto di comando non si isola, {COMANDANTE}.
419\tGli uomini chiusi nel compartimento sono perduti, {COMANDANTE}.
460\tSonar: un'arma in acqua a rilevamento {RIL}, non è diretta a noi.\n""")
sost('../docs/11_comandi_e_stazioni.md', """controllo danni al lavoro arriveranno dal kit.
""", """controllo danni al lavoro arriveranno dal kit. Dal 06/11/2026 (docs/10 §10.19): il pulsante 1 isola, chiude subito o riapre il
compartimento mostrato; il 5 apre la pagina delle squadre, dove 2 e 3 scelgono un sistema e 4 lo porta in cima alle riparazioni.
""")


def main():
    radice = os.path.abspath(sys.argv[1])
    controlla = '--controlla' in sys.argv
    testi = {}
    errori = []
    for file, vecchio, nuovo in P:
        if file.startswith('../') and '--tutto' not in sys.argv:
            continue
        percorso = os.path.join(radice, file)
        if percorso not in testi:
            testi[percorso] = open(percorso, encoding='utf-8').read()
        n = testi[percorso].count(vecchio)
        if n != 1:
            errori.append(f'{file}: {n} occorrenze di: {vecchio[:70]!r}')
            continue
        testi[percorso] = testi[percorso].replace(vecchio, nuovo)
    if errori:
        print('\n'.join(errori))
        sys.exit(1)
    if controlla:
        print('tutte le sostituzioni si applicano:', len(P))
        return
    for percorso, testo in testi.items():
        open(percorso, 'w', encoding='utf-8').write(testo)
    print('scritti:', len(testi), 'file,', len(P), 'sostituzioni')


if __name__ == '__main__':
    main()
