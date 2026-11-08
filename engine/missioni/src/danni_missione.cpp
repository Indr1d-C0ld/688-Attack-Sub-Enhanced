#include "missioni/danni_missione.hpp"

#include <algorithm>
#include <cmath>

#include "nucleo/geometria.hpp"

namespace m688 {

namespace {

constexpr double ASCOLTO_M = 20.0 * MIGLIO;   // sonar e radio del giocatore: 20 nm, il limite di scoperta della VM
constexpr double RITIRATA_S = 1200.0;         // la nave colpita si allontana da chi l'ha attaccata per 20 minuti
constexpr double SOCCORSO_S = 3600.0;
constexpr double SOCCORSO_KN = 15.0;

// TNT della testata dei missili: per l'arma, se si sa (il Tomahawk antinave TASM 454 kg, l'Harpoon 221), se no per
// classe del lanciatore (dossier 07 §4.1 e doc 05 §3.1): P-700 Granit 750 kg, P-500 Bazalt 1000, P-120 Malakhit 500,
// Harpoon 221; altrimenti 500 (Styx).
double testata_missile(const std::string& lanciatore, const std::string& arma = {}) {
    if (arma == "tomahawk") return 454.0;
    if (arma == "harpoon") return 221.0;
    if (lanciatore == "oscar" || lanciatore == "kirov") return 750.0;
    if (lanciatore == "slava") return 1000.0;
    if (lanciatore == "charlie2") return 500.0;
    if (lanciatore == "los_angeles" || lanciatore == "sturgeon" || lanciatore == "trafalgar") return 221.0;
    return 500.0;
}

}  // namespace

DanniMissione::DanniMissione(const std::map<std::string, ClasseNave>& classi, std::string cartella_dati,
                             std::uint64_t seme, RegolaDanni regola)
    : classi_(classi), dati_(std::move(cartella_dati)), caso_(seme ^ 0xd5d5u), regola_(regola) {}

bool DanniMissione::ha_fisica(const UnitaClassica& u) const {
    if (u.categoria != "nave") return false;
    const auto it = classi_.find(u.classe);
    return it != classi_.end() && !it->second.scafo.empty();
}

const NaveColpita* DanniMissione::nave(int unita) const {
    for (const auto& n : navi_) if (n->unita == unita) return n.get();
    return nullptr;
}

NaveColpita& DanniMissione::prendi(const UnitaClassica& u) {
    for (auto& n : navi_) if (n->unita == u.id) return *n;
    auto& sc = scafi_[u.classe];
    if (sc.stazioni.empty()) sc = DatiScafo::carica(dati_ + "/" + classi_.at(u.classe).scafo);
    auto n = std::make_unique<NaveColpita>();
    n->unita = u.id;
    n->id_mondo = ID_IMMAGINE + u.id;
    n->classe = u.classe;
    n->danni = std::make_unique<DanniNave>(sc, caso_.prossimo());
    if (u.cisterne.starts_with("zavorra")) n->danni->imposta_zavorra(u.cisterne == "zavorra_inerte");
    n->pos = u.pos;
    n->rotta = u.rotta;
    navi_.push_back(std::move(n));
    return *navi_.back();
}

void DanniMissione::colpisci(NaveColpita& n, const Colpo& c) {
    // la testata dell'arma, in un punto dello scafo scelto dal seme (la missione sa solo che il colpo e' a segno)
    const auto& st = n.danni->corpi().front().stazioni;
    const double x0 = st.front().x, x1 = st.back().x, L = x1 - x0, xm = (x0 + x1) / 2;
    const double chiglia = st[st.size() / 2].z_chiglia;
    Esplosione e;
    switch (c.tipo) {
        case Colpo::Tipo::arma:
            if (c.influenza) {   // mina a influenza (M2b): sotto la chiglia, alla distanza vera se c'e'
                e.tipo = Esplosione::Tipo::mina;
                e.x = xm + caso_.uniforme(-0.2, 0.2) * L;
                if (c.geometria_vera) {
                    const double prof = std::max(c.dz_m, 0.0), lato = std::sqrt(std::max(0.0, c.distanza_m * c.distanza_m - prof * prof));
                    e.y = caso_.uniforme() < 0.5 ? -lato : lato;
                    e.z = std::min(-prof, chiglia - 0.5);
                } else {
                    e.y = 0.0;
                    e.z = chiglia - caso_.uniforme(5.0, 15.0);
                }
                e.contatto = false;
                e.tnt_kg = 150.0;
            } else if (c.classe_arma.find("mk48") != std::string::npos) {   // ADCAP: spoletta d'influenza sotto la chiglia
                e.x = xm + caso_.uniforme(-0.15, 0.15) * L; e.y = 0.0;
                e.z = chiglia - caso_.uniforme(3.0, 6.0); e.contatto = false; e.tnt_kg = 544.0;
            } else {   // siluri sovietici da 533 mm: a contatto sul fianco
                e.x = xm + caso_.uniforme(-0.3, 0.3) * L; e.y = caso_.uniforme() < 0.5 ? -1.0 : 1.0;
                e.z = 0.6 * chiglia; e.contatto = true; e.tnt_kg = 300.0;
            }
            if (c.tnt_kg > 0) e.tnt_kg = c.tnt_kg;   // la carica vera (mine dell'Enhanced)
            break;
        case Colpo::Tipo::attacco:   // carica di profondita' o razzo sotto la nave
            if (!c.riuscito) return;
            e.x = xm + caso_.uniforme(-0.3, 0.3) * L; e.y = 0.0; e.z = chiglia - 8.0; e.contatto = false; e.tnt_kg = 135.0;
            break;
        case Colpo::Tipo::attacco_casuale:   // missile antinave sopra il galleggiamento
            e.tipo = Esplosione::Tipo::missile;
            e.x = xm + caso_.uniforme(-0.25, 0.25) * L; e.y = caso_.uniforme() < 0.5 ? -1.0 : 1.0; e.z = 2.5;
            e.tnt_kg = testata_missile(c.classe_lanciatore, c.classe_arma);
            break;
    }
    if (c.lanciatore >= 0) n.lanciatore = c.lanciatore;
    n.danni->colpo(e);
}

bool DanniMissione::menomata(const DanniNave& d) {
    double fuoco = 0.0;
    for (const auto& c : d.corpi())
        for (const auto& ce : c.celle) fuoco += ce.fuoco;
    for (const auto& z : d.zone()) fuoco += z.fuoco;
    return !d.energia() || d.spezzata() || fuoco >= 1.5 || d.acqua_imbarcata_t() > 0.05 * d.dislocamento_intatto_t() ||
           std::fabs(d.sbandamento_gradi()) > 8.0;
}

void DanniMissione::annuncia(const Missione& m, const NaveColpita& n, int codice) {
    const auto* g = m.giocatore();
    if (g && distanza(g->pos, n.pos) <= ASCOLTO_M) messaggi_.push_back({m.tempo(), n.unita, codice});
}

void DanniMissione::reazioni(Missione& m, NaveColpita& n) {
    const auto it = m.unita().find(n.unita);
    if (it == m.unita().end()) return;
    const UnitaClassica& u = it->second;
    const DanniNave& d = *n.danni;
    // velocita' consentita dalla fisica: ferma senza energia, piano con sbandamento o molta acqua a bordo
    const double acqua = d.acqua_imbarcata_t() / std::max(d.dislocamento_intatto_t(), 1.0);
    const double sb = std::fabs(d.sbandamento_gradi());
    double vmax = -1.0;
    if (!d.energia() || d.condannata()) vmax = 0.0;
    else if (acqua > 0.15 || sb > 15.0) vmax = 5.0;
    else if (acqua > 0.05 || sb > 5.0) vmax = 12.0;
    if (n.velocita_urto < 1.0) {   // l'elica piegata da una collisione
        const double v = u.avanti_tutta * std::max(n.velocita_urto, 0.0);
        vmax = vmax >= 0.0 ? std::min(vmax, v) : v;
    }
    OrdineMotore o = u.ordine;
    o.velocita_max_kn = vmax;
    if (n.timone_bloccato) o.timone = false;   // dopo una collisione
    // ritirata: lontano da chi ha attaccato, alla velocita' che resta
    if (n.fine_ritirata < 0.0 && n.lanciatore >= 0 && vmax != 0.0) {
        if (const auto il = m.unita().find(n.lanciatore); il != m.unita().end()) {
            const double r = radianti(rilevamento(il->second.pos, u.pos));
            o.meta = true;
            o.verso = {u.pos.x + 20000.0 * std::sin(r), u.pos.y + 20000.0 * std::cos(r)};
            o.velocita_kn = vmax > 0 ? std::min(u.avanti_tutta, vmax) : u.avanti_tutta;
            o.fino_a = m.tempo() + RITIRATA_S;
            n.fine_ritirata = o.fino_a;
        }
    }
    if (vmax == 0.0) o.meta = false;
    m.ordina(n.unita, o);
    // abbandono nave: le navi amiche vicine e illese vanno in soccorso
    if (d.condannata() && !n.soccorso) {
        n.soccorso = true;
        bool qualcuna = false;
        for (const auto& [id, v] : m.unita()) {
            if (id == n.unita || v.categoria != "nave" || v.colpo != 0 || v.affondamento != 0) continue;
            if ((v.flag & 0x18) != (u.flag & 0x18) || distanza(v.pos, u.pos) > ASCOLTO_M) continue;
            OrdineMotore s;
            s.meta = true; s.verso = u.pos; s.velocita_kn = std::min(v.avanti_tutta, SOCCORSO_KN);
            s.fino_a = m.tempo() + SOCCORSO_S;
            m.ordina(id, s);
            qualcuna = true;
        }
        if (qualcuna) annuncia(m, n, 206);
    }
}

void DanniMissione::avanza(Missione& m, double dt) {
    tempo_ = m.tempo();
    if (regola_ == RegolaDanni::simulazione) {
        m.impatti_dell_originale(false);   // un'arma che passa vicino colpisce, chiunque osservi in quel passo
        m.dinamica_sottomarini(classi_, dati_);   // i sottomarini con la dinamica dell'Enhanced (fase C6a)
        m.acustica_enhanced(dati_);   // scoperta sonar con l'equazione del sonar (nucleo/acustica.hpp)
        m.radio_enhanced();   // sala radio del giocatore (nucleo/radio.hpp, pezzo S4)
        m.meteo_enhanced(dati_);   // meteo e mare del teatro (nucleo/meteo.hpp, docs/15 M2): prima di chi lo usa
        m.elettronica_enhanced(dati_);   // radar dell'IA con l'EMCON ed ESM del giocatore (nucleo/elettronica.hpp)
        m.siluri_enhanced(dati_);   // i siluri con il modello dell'Enhanced (fase C6b)
        m.asw_enhanced(dati_);   // ASW aereo: controllori, boe e sonar a immersione (docs/13_asw_aereo.md, A3)
        m.periscopi_enhanced(dati_);   // periscopi: posto del giocatore, teste viste, guardate dell'IA (docs/14, P2)
        m.luci_enhanced(dati_);   // le luci di navigazione e di fonda: stato per unita', ordine del giocatore, vedetta (docs/27)
        m.artiglieria_enhanced(dati_);   // artiglieria navale contro il sottomarino esposto (docs/16, G2)
        m.forze_enhanced(dati_);   // allerta della forza, situazione per coppia di parti, ROE dell'IA (docs/18, fase 3)
        m.collisioni_enhanced(classi_, dati_);   // urti fra unita' e Crazy Ivan (docs/ricerca/16_collisioni.md)
        m.batterie_enhanced();   // batteria e snorkel dei diesel dell'IA (M6 del dossier 23)
        for (const auto& [id, u] : m.unita())
            if (ha_fisica(u) && !m.alla_fisica(id)) m.affida_alla_fisica(id);
    }
    // colpi nuovi
    const auto& colpi = m.colpi();
    for (; colpi_visti_ < colpi.size(); ++colpi_visti_) {
        const Colpo& c = colpi[colpi_visti_];
        const auto it = m.unita().find(c.vittima);
        const bool fisica = it != m.unita().end() ? ha_fisica(it->second) : false;
        if (!fisica) { altri_.push_back(c); continue; }
        NaveColpita& n = prendi(it->second);
        if (!n.relitto) colpisci(n, c);
    }
    // collisioni (nucleo/collisioni.hpp): la nave di un urto con un sottomarino. Con lo scafo per i danni la falla e le
    // parti perse (DanniNave::urto_meccanico); senza, l'integrita' dell'originale in proporzione (STIMA). I sottomarini,
    // anche in superficie, li fa DanniSottomarini.
    const auto& urti = m.urti();
    for (; urti_visti_ < urti.size(); ++urti_visti_) {
        const auto& x = urti[urti_visti_];
        for (const auto* c : {&x.urto.a, &x.urto.b}) {
            if (c->sottomarino || c->unita < 0) continue;
            const auto it = m.unita().find(c->unita);
            if (it == m.unita().end() || it->second.distrutta || it->second.persa) continue;
            const DanniUrtoUnita& d = c == &x.urto.a ? x.esito.a : x.esito.b;
            if (d.duomo_perso) m.perdi_sonar_scafo(c->unita);   // il duomo del sonar strappato
            if (ha_fisica(it->second)) {
                NaveColpita& n = prendi(it->second);
                if (!n.relitto) n.danni->urto_meccanico(d);
                n.velocita_urto = std::min(n.velocita_urto, d.velocita_max);   // la applica reazioni()
                n.timone_bloccato = n.timone_bloccato || d.timone_bloccato;
            } else if (UnitaClassica* u = m.trova(c->unita)) {
                if (d.velocita_max < 1.0 || d.timone_bloccato) {   // l'elica piegata, il timone bloccato: per sempre
                    OrdineMotore o = u->ordine;
                    if (d.velocita_max < 1.0) {
                        const double v = u->avanti_tutta * std::max(d.velocita_max, 0.0);
                        o.velocita_max_kn = o.velocita_max_kn >= 0.0 ? std::min(o.velocita_max_kn, v) : v;
                    }
                    if (d.timone_bloccato) o.timone = false;
                    m.ordina(c->unita, o);
                }
                const int perdita = static_cast<int>(std::lround(perdita_integrita_urto(d) * u->integrita_max));
                if (perdita > 0) {
                    u->integrita -= perdita;
                    if (u->integrita <= 0 && u->affondamento == 0) m.distruggi(c->unita);
                }
            }
        }
    }
    // artiglieria fra navi (G3, battaglie di superficie con artiglieria.fra_navi): i colpi diretti sullo scafo e sulle
    // sovrastrutture (la «falsatorre» delle navi) come colpi sopra il galleggiamento, con meno incendi di un missile;
    // le schegge non aprono falle. Senza scafo per i danni, l'integrita' dell'originale ~ 2,6 W^0,6 punti (W kg di
    // carica; STIMA: 5" circa 6, 16" circa 33). I sottomarini li fa DanniSottomarini.
    const auto& impatti = m.impatti_artiglieria();
    for (; impatti_visti_ < impatti.size(); ++impatti_visti_) {
        const ImpattoArtiglieria& i = impatti[impatti_visti_];
        if (i.avvertimento || i.esito != ImpattoArtiglieria::Esito::diretto || i.carica_kg <= 0.0) continue;
        if (i.parte != ParteEsposta::scafo && i.parte != ParteEsposta::falsatorre) continue;
        const auto it = m.unita().find(i.bersaglio);
        if (it == m.unita().end() || it->second.categoria != "nave" || it->second.distrutta || it->second.persa) continue;
        const UnitaClassica& u = it->second;
        if (ha_fisica(u)) {
            NaveColpita& n = prendi(u);
            if (n.relitto) continue;
            const double r = radianti(u.rotta), dx = i.punto.x - u.pos.x, dy = i.punto.y - u.pos.y;
            Esplosione e;
            e.tipo = Esplosione::Tipo::proietto;
            e.x = dx * std::sin(r) + dy * std::cos(r);       // lungo lo scafo, prua positiva
            e.y = -dx * std::cos(r) + dy * std::sin(r);      // a sinistra positiva (coordinate del kit)
            e.z = std::max(1.0, i.quota_m);   // sopra il galleggiamento (i colpi al pelo dell'acqua scoppiano fuori)
            e.tnt_kg = i.carica_kg;
            e.contatto = true;
            n.danni->colpo(e);
            if (i.tiratore >= 0) n.lanciatore = i.tiratore;
        } else if (UnitaClassica* v = m.trova(i.bersaglio)) {
            const int perdita = static_cast<int>(std::lround(2.6 * std::pow(i.carica_kg, 0.6)));
            if (perdita > 0) {
                v->integrita -= perdita;
                if (v->integrita <= 0 && v->affondamento == 0) m.distruggi(i.bersaglio);
            }
        }
    }
    for (auto& p : navi_) {
        NaveColpita& n = *p;
        DanniNave& d = *n.danni;
        d.avanza(dt);
        const auto it = m.unita().find(n.unita);
        const bool viva = it != m.unita().end() && !it->second.distrutta && !it->second.persa;
        if (!n.relitto && viva) {
            n.pos = it->second.pos; n.rotta = it->second.rotta; n.velocita = it->second.velocita;
            if (regola_ == RegolaDanni::simulazione) {
                reazioni(m, n);
                for (const auto& z : d.zone())
                    if (z.elicottero_perso) m.perdi_elicottero(n.unita);
                const bool persa = d.condannata() || d.affondata() || (it->second.integrita <= 0 && menomata(d));
                if (persa && it->second.affondamento == 0) m.distruggi(n.unita);
            }
        }
        // relitto: la missione la da' per distrutta, o la fisica l'ha affondata
        if (!n.relitto && (!viva || it->second.affondamento > 0 || d.affondata())) {
            n.relitto = true;
            n.velocita = 0.0;
            if (regola_ == RegolaDanni::classica && !d.condannata() && !d.affondata() && !n.colpo_di_grazia) {
                // livello classico: la regola dell'originale l'ha distrutta, la fisica ancora no. Falle sul fondo di tutti i
                // locali, in tutto 1 cm^2 per tonnellata (0,4 m^2 per una fregata, 5 per una portacontainer), equipaggio che
                // abbandona la nave: la compartimentazione non basta piu' e affonda, nel modo che la fisica sceglie
                n.colpo_di_grazia = true;
                d.abbandona();   // nave perduta: niente piu' tamponature ne' pompe
                const auto& c = d.corpi().front();
                const double area = 1e-4 * d.dislocamento_intatto_t() / static_cast<double>(std::max<std::size_t>(c.celle.size(), 1));
                for (std::size_t j = 0; j < c.celle.size(); ++j) {
                    const auto& cd = c.celle[j].dati;
                    const auto& st = c.stazioni[static_cast<std::size_t>(c.celle[j].stazioni[c.celle[j].stazioni.size() / 2])];
                    n.danni->aggiungi_falla({static_cast<int>(j), (cd.x0 + cd.x1) / 2, std::clamp(0.0, cd.y0, cd.y1), st.z_chiglia,
                                             area, false, false});
                }
            }
        }
        // cio' che il sonar e la radio del giocatore riferiscono
        for (; n.scoppi < d.scoppi().size(); ++n.scoppi)
            if (d.scoppi()[n.scoppi].tipo == Esplosione::Tipo::deposito) annuncia(m, n, 201);
        // il deposito in fiamme e l'allagamento d'emergenza (danni critici, T15): solo con la regola di simulazione, una
        // volta per nave. La soglia del fuoco e' quella da cui parte l'allagamento (danni.cpp)
        if (regola_ == RegolaDanni::simulazione)
            for (const auto& c : d.corpi())
                for (const auto& ce : c.celle) {
                    if (ce.dati.uso != "deposito") continue;
                    if (!n.deposito_brucia && ce.fuoco > 0.2) { n.deposito_brucia = true; annuncia(m, n, 207); }
                    if (!n.deposito_allagato && ce.allagata_per_incendio == 0.0) {
                        n.deposito_allagato = true;
                        annuncia(m, n, 208);
                    }
                }
        if (!n.spezzata && d.spezzata()) { n.spezzata = true; annuncia(m, n, 200); }
        if (!n.sbanda && (std::fabs(d.sbandamento_gradi()) > 10.0 || std::fabs(d.appoppamento_gradi()) > 3.0)) {
            n.sbanda = true;
            annuncia(m, n, 203);
        }
        if (regola_ == RegolaDanni::simulazione && !n.ferma && !d.energia()) { n.ferma = true; annuncia(m, n, 202); }
        if (!n.abbandono && d.condannata()) { n.abbandono = true; annuncia(m, n, 205); }
        if (!n.affondata && d.affondata()) {
            n.affondata = true;
            if (!n.spezzata) annuncia(m, n, 200);   // rumori di rottura e compartimenti che cedono mentre scende
            annuncia(m, n, 204);
        }
    }
}

void DanniMissione::sincronizza(Mondo& mondo) const {
    auto& navi = mondo.navi();
    for (const auto& p : navi_) {
        const NaveColpita& n = *p;
        // l'unita' degli script lascia il posto alla sua immagine (stessa posizione finche' non e' un relitto)
        Nave immagine;
        bool trovata = false;
        for (auto it = navi.begin(); it != navi.end(); ++it)
            if (it->id == n.unita) { immagine = *it; trovata = true; navi.erase(it); break; }
        if (!trovata || n.relitto) {
            immagine.classe = n.classe;
            immagine.pos = immagine.pos_prec = n.pos;
            immagine.rotta = immagine.rotta_prec = n.rotta;
            immagine.velocita = immagine.velocita_ordinata = 0.0;
        }
        immagine.id = n.id_mondo;
        immagine.posa = n.danni->posa();   // affondata: tutta sotto il pelo dell'acqua, non si disegna
        bool aggiornata = false;
        for (auto& v : navi)
            if (v.id == n.id_mondo) { v = immagine; aggiornata = true; break; }
        if (!aggiornata) mondo.aggiungi(immagine);
    }
}

}  // namespace m688
