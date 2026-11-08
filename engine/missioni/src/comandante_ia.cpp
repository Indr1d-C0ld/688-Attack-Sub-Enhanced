// Il comandante IA del battello del giocatore (sandbox, docs/19 §5, V1): vedi missioni/comandante_ia.hpp. Numeri STIMA
// DI GIOCO, dalla dottrina gia' usata dall'IA (docs/18, fase 4) e dal dossier 02 §5.

#include "missioni/comandante_ia.hpp"

#include <algorithm>
#include <cmath>

#include "missioni/contatti_giocatore.hpp"
#include "missioni/stazioni_missione.hpp"
#include "nucleo/armi_giocatore.hpp"
#include "nucleo/siluro.hpp"

namespace m688 {

namespace {

constexpr double PASSO_S = 1.0;
constexpr double VELOCITA_TRANSITO_KN = 8.0, VELOCITA_PATTUGLIA_KN = 5.0, VELOCITA_SPRINT_KN = 14.0;
constexpr double SPRINT_S = 300.0, DERIVA_S = 900.0;           // sprint and drift (dossier 02 §5)
// la quota periscopica della missione (20 ft, come StazioneArmi::QUOTA_PERISCOPICA e controlli_giocatore: piu' giu'
// periscopio e antenna si ammainano da soli e i missili non partono); la dinamica dell'Enhanced usa la stessa scala
constexpr double QUOTA_PATTUGLIA_M = 120.0, QUOTA_PERISCOPICA_M = 6.096, SALTO_QUOTA_M = 60.0;
constexpr double QUOTA_ORDINE_PERISCOPICA_M = 5.5;
constexpr double MARGINE_CHIGLIA_M = 5.0;   // la chiglia vera dal fondo
constexpr double PASSAGGIO_M = 2.0;         // per passare in superficie: la chiglia (il pescaggio) con 2 m sotto
constexpr double RIPIANO_S = 600.0;         // la rotta per acqua navigabile si rifa' ogni 10 minuti   // l'ordine un poco sopra: col mare e l'assetto resta entro i 20 ft
constexpr double ARRIVO_AREA_M = 3000.0, LATO_PATTUGLIA_M = 8000.0;
constexpr double RILEVAMENTO_OGNI_S = 20.0, SOLUZIONE_OGNI_S = 60.0, SIGMA_RILEVAMENTO_DEG = 1.5;
constexpr double DISTANZA_LANCIO_M = 15000.0, LANCI_PER_BERSAGLIO = 2, TRA_LANCI_S = 120.0;   // Mk 48: 38 km ad alta velocita'
constexpr double VELOCITA_AVVICINAMENTO_KN = 12.0;
constexpr double DIFESA_S = 180.0, MINACCIA_M = 5000.0;
constexpr double SIGMA_PUNTO_NAVE_M = 1852.0;
constexpr double ROTTA_TOLLERANZA_DEG = 5.0, ROTTA_OGNI_S = 30.0;
constexpr double TMA_PRIMA_DEL_PING_S = 300.0, PING_OGNI_S = 120.0;   // la distanza dall'eco, se la TMA non basta
constexpr double PORTATA_MISSILE_M = 100000.0;   // Harpoon UGM-84A ~120 km: da qui in su contro le navi, oltre i siluri
constexpr double BARRA_FILO_DEG = 5.0;           // la barra mentre guida a filo: accosta piano, il filo regge
constexpr double RAPPORTO_OGNI_S = 600.0, ASCOLTO_PRIMA_S = 120.0, VELOCITA_QUOTA_PERISCOPICA_KN = 5.0;
constexpr double CORSA_MINA_M = 12000.0, TRA_MINE_S = 60.0;   // Mk 67: corsa fino a 13 km (carico.toml); una al minuto
// V3, il piano delle originali
constexpr double PUNTO_RAGGIUNTO_M = 2000.0, MARGINE_LIMITE_M = 3000.0, PREVISIONE_LIMITE_M = 4000.0;
constexpr double SCATTO_S = 240.0, DERIVA_SEMINARE_S = 360.0, VELOCITA_SCATTO_KN = 20.0, VELOCITA_DERIVA_KN = 4.0;
constexpr double TRA_SCATTI_S = 600.0;              // evasione senza contatto: uno scatto ogni 10 minuti (pulizia del cono)
constexpr double QUOTA_ALTA_M = 60.0, QUOTA_BASSA_M = 200.0, SOPRA_FONDO_M = 25.0;   // le due parti dello strato; l'acqua sotto lo scatto
constexpr double STAZIONE_M = 3000.0, VELOCITA_STAZIONE_KN = 16.0;
constexpr double KN = 1852.0 / 3600.0;
constexpr double CERCA_MISSILI_S = 900.0;
constexpr double ANGOLO_QUOTA_DEG = 15.0;
constexpr double TRA_MISSILI_TERRA_S = 10.0;          // SCENE0A: i quattro Tomahawk entro 300 s (L_02fb)
constexpr double ASCOLTO_OGNI_S = 1800.0;              // il broadcast: una finestra ogni mezz'ora, non tutte              // l'angolo massimo della quota, come PostoEnhanced::ANGOLO_MAX           // il bersaglio dei missili non si vede: poi la fuga
bool arma_da_armare(Arma a) { return a != Arma::nessuna && a != Arma::attacco_terra; }
bool missile(Arma a) { return a == Arma::harpoon || a == Arma::tomahawk; }
// i rilevamenti (uno ogni 20 s) prima di fidarsi della soluzione: la recluta aspetta 6 minuti, l'esperto 2 e mezzo
std::size_t rilevamenti_minimi(Esperienza e) { return e == Esperienza::recluta ? 18 : e == Esperienza::esperto ? 8 : 12; }

double normalizza(double a) { a = std::fmod(a, 360.0); return a < 0.0 ? a + 360.0 : a; }
double scarto(double a, double b) { const double d = normalizza(a - b); return d > 180.0 ? 360.0 - d : d; }
double rilevamento_da(Vec2 da, Vec2 a) { return normalizza(std::atan2(a.x - da.x, a.y - da.y) * 180.0 / 3.14159265358979323846); }
Vec2 lungo(Vec2 p, double rotta_deg, double d) {
    const double r = rotta_deg * 3.14159265358979323846 / 180.0;
    return {p.x + d * std::sin(r), p.y + d * std::cos(r)};
}

}  // namespace

const char* nome_fase_comandante(StatoComandante::Fase f) {
    switch (f) {
        case StatoComandante::Fase::transito: return "transito";
        case StatoComandante::Fase::pattuglia: return "pattuglia";
        case StatoComandante::Fase::tma: return "tma";
        case StatoComandante::Fase::attacco: return "attacco";
        case StatoComandante::Fase::difesa: return "difesa";
        case StatoComandante::Fase::punto_nave: return "punto_nave";
        case StatoComandante::Fase::ritirata: return "ritirata";
        case StatoComandante::Fase::radio: return "radio";
        case StatoComandante::Fase::mine: return "mine";
        case StatoComandante::Fase::scorta: return "scorta";
        case StatoComandante::Fase::pedinamento: return "pedinamento";
        case StatoComandante::Fase::seminare: return "seminare";
        case StatoComandante::Fase::attesa: return "attesa";
        case StatoComandante::Fase::attacco_terra: return "attacco_terra";
    }
    return "?";
}

ComandanteIa::ComandanteIa(const Missione& m, OpzioniComandante o, std::uint64_t seme) : o_(o), caso_(seme ^ 0xC0DA17E5ull) {
    quota_crociera_ = QUOTA_PATTUGLIA_M;
    if (const auto* g = m.giocatore()) {
        lato_ = g->flag & 0x18;
        partenza_ = g->pos;
        pescaggio_ = m.pescaggio(*g);   // la chiglia in superficie e a quota periscopica, per la quota sopra il fondo
        UnitaClassica a_quota = *g;
        a_quota.quota = QUOTA_PERISCOPICA_M;
        chiglia_periscopica_ = m.chiglia_vera(a_quota);
    }
}

void ComandanteIa::cambia(double t, StatoComandante::Fase f, const std::string& motivo, Missione& m) {
    if (stato_.fase == f && stato_.motivo == motivo) return;
    stato_.fase = f;
    stato_.motivo = motivo;
    storia_.emplace_back(t, stato_);
    m.nota_comandante(t, nome_fase_comandante(f), motivo, stato_.bersaglio);
}

void ComandanteIa::rotta(std::vector<Ordine>& v, double t, double r, double kn, const UnitaClassica& g) {
    if (vincoli_.attivi) kn = std::min(kn, vincoli_.velocita_max_kn);   // mentre guida a filo
    // il rettangolo del piano (fuori, l'originale chiude la missione): se la rotta ne esce, verso il centro. Anche nella
    // fuga da un siluro (fuori la missione e' persa comunque); non verso un'uscita che sta fuori
    if (o_.piano && o_.piano->limite && !verso_uscita_) {
        const auto& l = *o_.piano->limite;
        if (!l.dentro(lungo(io_, r, PREVISIONE_LIMITE_M), MARGINE_LIMITE_M) && l.dentro(io_))
            r = rilevamento_da(io_, {(l.x0 + l.x1) / 2.0, (l.y0 + l.y1) / 2.0});
    }
    // le secche: se lungo la rotta, entro 2 km, l'acqua non basta alla chiglia in superficie, a scarti di 15° dal lato
    // gia' scelto o da quello con piu' acqua (come Missione::evita_secche per l'IA)
    if (missione_) {
        const double serve = pescaggio_ + PASSAGGIO_M;
        const auto acqua = [&](double rr) {   // la meno profonda lungo la rotta, entro 2 km
            double minima = 1e9;
            for (const double d : {500.0, 1000.0, 1500.0, 2000.0})
                if (const double f = missione_->profondita(lungo(io_, rr, d)); std::isfinite(f)) minima = std::min(minima, f);
            return minima;
        };
        if (acqua(r) < serve) {
            if (lato_secche_ == 0)
                lato_secche_ = missione_->profondita(lungo(io_, r + 45.0, 1500.0)) >= missione_->profondita(lungo(io_, r - 45.0, 1500.0)) ? 1 : -1;
            // la prima libera; se non ce n'e' (acqua bassa tutto intorno), quella con piu' acqua
            double meglio = r, acqua_meglio = acqua(r);
            for (int passo = 1, trovata = 0; passo <= 12 && !trovata; ++passo)
                for (const int l : {lato_secche_, -lato_secche_}) {
                    const double rr = normalizza(r + l * 15.0 * passo), a = acqua(rr);
                    if (a >= serve) { meglio = rr; trovata = 1; break; }
                    if (a > acqua_meglio + 0.5) meglio = rr, acqua_meglio = a;
                }
            r = meglio;
        } else {
            lato_secche_ = 0;
        }
    }
    bool ripeti = false;   // ogni ROTTA_OGNI_S rotta e giri di nuovo: un ordine perso (al primo passo, prima della
                           // dinamica dell'Enhanced) non lascia il battello fermo
    if (rotta_data_ < 0.0 || scarto(r, rotta_data_) > ROTTA_TOLLERANZA_DEG || t >= prossima_rotta_) {
        v.push_back({t, TipoOrdine::accosta_per, r, vincoli_.attivi ? BARRA_FILO_DEG : 15.0, 0});
        ripeti = t >= prossima_rotta_;
        rotta_data_ = r;
        prossima_rotta_ = t + ROTTA_OGNI_S;
    }
    // con la dinamica dell'Enhanced i giri per quella velocita' (come il telegrafo del giocatore), nel classico la
    // frazione. Lontani dalla velocita' voluta, come il giocatore, qualche nodo in piu' finche' non ci si arriva: ai
    // giri bassi la spinta e' poca e il battello ci mette minuti
    const double massima = std::max(1.0, g.avanti_tutta);
    const double spinta = dinamica_ && g.velocita < kn - 2.0 ? std::min(massima, kn + 2.0 * (kn - g.velocita)) : kn;
    if (std::fabs(spinta - velocita_data_) > 0.5 || ripeti) {
        const double giri = dinamica_ ? dinamica_->giri_per_velocita(spinta) : kn / massima;
        v.push_back({t, TipoOrdine::giri, std::clamp(giri, 0.05, 1.0), 0.0, 0});
        velocita_data_ = spinta;
    }
}

// la quota piu' profonda con la chiglia vera (Missione::chiglia_vera) a MARGINE_CHIGLIA_M dal fondo: sotto la quota
// periscopica la chiglia e' quella della classe (688: 18,3 m), sopra si interpola fino al pescaggio in superficie. In
// 15-16 m di fondo (la partenza di SCENE0A) resta in superficie; i missili partono anche da li'
double ComandanteIa::quota_massima() const {
    if (!std::isfinite(fondo_) || fondo_ <= 0.0) return 1e9;
    const double c = fondo_ - MARGINE_CHIGLIA_M;
    if (c >= chiglia_periscopica_) return c;
    if (c <= pescaggio_) return 0.0;
    return QUOTA_PERISCOPICA_M * (c - pescaggio_) / std::max(1.0, chiglia_periscopica_ - pescaggio_);
}

// la prossima tappa verso `meta` per acqua navigabile (nucleo/rotte, come il pilota dell'IA): la chiglia in superficie
// con PASSAGGIO_M per passare, quella a quota periscopica per stare comodi. Rifatta ogni RIPIANO_S o se la meta cambia;
// senza fondale, la meta
Vec2 ComandanteIa::verso(const Missione& m, double t, Vec2 io, Vec2 meta) {
    const Fondale* f = m.fondale();
    if (!f) return meta;
    if (!acque_) acque_ = std::make_shared<const AcqueNavigabili>(*f, pescaggio_ + PASSAGGIO_M);
    if (!meta_rotta_ || distanza(*meta_rotta_, meta) > PUNTO_RAGGIUNTO_M || t >= ripiano_) {
        percorso_ = rotta_navigabile(*acque_, io, meta, chiglia_periscopica_ + MARGINE_CHIGLIA_M).punti;
        meta_rotta_ = meta;
        ripiano_ = t + RIPIANO_S;
    }
    while (!percorso_.empty() && distanza(io, percorso_.front()) <= PUNTO_RAGGIUNTO_M) percorso_.erase(percorso_.begin());
    return percorso_.empty() ? meta : percorso_.front();
}

void ComandanteIa::quota(std::vector<Ordine>& v, double t, double q) {
    quota_voluta_ = q;
    q = std::min(q, quota_massima());   // la chiglia sopra il fondo (al passo il fondale cambia: si rifa' da passo)
    if (quota_data_ >= 0.0 && std::fabs(q - quota_data_) < 2.0) return;   // -1: mai data
    // con la dinamica dell'Enhanced la quota col suo angolo massimo, come la stazione del giocatore (PostoEnhanced);
    // l'ordine di quota del classico la' non vale
    if (dinamica_) v.push_back({t, TipoOrdine::quota_con_angolo, q, ANGOLO_QUOTA_DEG, 0});
    else v.push_back({t, TipoOrdine::quota, q, 0.0, 0});
    quota_data_ = q;
}

std::vector<ComandanteIa::Contatto> ComandanteIa::contatti(const Missione& m) const {
    std::vector<Contatto> v;
    const auto* g = m.giocatore();
    if (!g) return v;
    for (const auto& s : segni_contatti(m, false)) {
        Contatto c;
        c.id = s.id;
        c.rilevamento = s.rilevamento;
        c.posizione = s.traccia();
        c.pos = s.pos_stimata;
        c.arma = (s.indicatori & SEGNO_ARMA) != 0;
        if (const auto* u = m.unita().count(s.id) ? &m.unita().at(s.id) : nullptr) c.nostra = c.arma && u->lanciatore == g->id;   // le proprie armi le sa
        // la parte dalla classe nota: quella delle unita' della missione di quella classe (come l'IA)
        const BarraBersaglio b = barra_bersaglio(m, s.id);
        if (b.titolo == BarraBersaglio::Titolo::classe && !b.classe.empty()) {
            c.classe = b.classe;
            c.classificato = true;
            for (const auto& [k, u] : m.unita())
                if (u.classe == b.classe) {
                    const int l = u.flag & 0x18;
                    c.ostile = l != 0 && l != 0x18 && l != lato_;
                    c.sottomarino = u.categoria == "sottomarino";
                    c.categoria = m.categoria_roe(u);
                    break;
                }
        } else if (c.arma && !c.nostra) {
            c.ostile = true;   // un'arma che non e' nostra
        } else if (!c.arma) {
            // non ancora classificato (la classe arriva dopo 1400 s di contatto): la carta dice gia' la parte (il colore
            // occidentale) e se e' in superficie o in aria. Per la NATO ogni contatto non occidentale e' ostile; per il
            // Patto un occidentale e' ostile solo se sta sotto (i mercantili neutrali hanno il colore occidentale)
            const bool occidentale = (s.indicatori & SEGNO_OCCIDENTALE) != 0;
            c.sottomarino = (s.indicatori & SEGNO_NAVE_AEREO) == 0;
            c.ostile = lato_ == 0x08 ? !occidentale : occidentale && c.sottomarino;
            c.categoria = c.sottomarino ? CategoriaRoe::sottomarini : CategoriaRoe::navi;
        }
        v.push_back(c);
    }
    return v;
}

bool ComandanteIa::della_lista(const Contatto& c) const {
    return !c.classe.empty() && std::find(classi_obiettivo_.begin(), classi_obiettivo_.end(), c.classe) != classi_obiettivo_.end();
}

bool ComandanteIa::armi_libere(const Missione& m, const Contatto& c) const {
    if (!c.ostile || c.arma) return false;
    if (o_.piano) {   // V3: le armi del piano
        const bool attaccati = !minacce_.empty() || colpito_;   // un'arma nemica in acqua: «a meno che non sparino»
        switch (o_.piano->armi) {
            case PianoOriginale::Armi::libere: return true;
            case PianoOriginale::Armi::solo_difesa: return attaccati;
            case PianoOriginale::Armi::classi: return attaccati || della_lista(c);
            case PianoOriginale::Armi::roe: break;
        }
    }
    if (const SalaRadio* r = m.radio()) {
        const StatoRoe s = r->roe_ricevute(true)[c.categoria];
        return c.classificato ? s != StatoRoe::trattenuto : s == StatoRoe::libero;   // stretto: solo gli identificati
    }
    return m.situazione_fra(lato_, lato_ == 0x08 ? 0x10 : 0x08) == Situazione::guerra;
}

std::vector<Ordine> ComandanteIa::passo(Missione& m, const StazioniMissione& st) {
    std::vector<Ordine> v;
    const double t = m.tempo();
    if (t < prossimo_) return v;
    prossimo_ = t + PASSO_S;
    const auto* g = m.giocatore();
    if (!g || g->distrutta || g->persa) return v;
    dinamica_ = m.con_dinamica() ? m.dinamica(g->id) : nullptr;
    if (dinamica_ && !dinamica_vista_) {   // la dinamica nasce al primo passo: gli ordini di prima non valgono, di nuovo
        dinamica_vista_ = true;
        quota_data_ = velocita_data_ = rotta_data_ = -1.0;
    }
    // un ordine di quota perso: di nuovo (nel classico; con la dinamica ogni ordine arriva)
    if (!dinamica_ && std::fabs(g->quota_ordinata - quota_data_) > 2.0) quota_data_ = -1.0;
    const Vec2 io = m.posizione_stimata(g->id);
    io_ = io;
    missione_ = &m;
    fondo_ = m.profondita(g->pos);   // dal sonar di prora (l'ecoscandaglio): il battello lo sa
    if (quota_voluta_ > 0.0) quota(v, t, quota_voluta_);
    const auto& armi = st.armi();
    const PianoOriginale* piano = o_.piano ? &*o_.piano : nullptr;

    if (!iniziato_) {
        iniziato_ = true;
        // l'obiettivo: il primo primario con un'area, se no il primo con delle classi; senza, la pattuglia qui
        for (const auto& a : m.registro().aree_obiettivi()) {
            if (!a.primario) continue;
            if (a.centro && !area_) {
                area_ = a.centro;
                raggio_area_m_ = a.raggio_m;
                stato_.obiettivo = a.id;
            }
            if (a.tipo == Obiettivo::Tipo::minare && a.centro) {   // le mine: l'area del campo
                area_ = a.centro;
                raggio_area_m_ = a.raggio_m;
                stato_.obiettivo = a.id;
                minare_ = true;
            }
            if (!a.classi.empty() && classi_obiettivo_.empty() &&
                (a.tipo == Obiettivo::Tipo::affondare || a.tipo == Obiettivo::Tipo::colpire || a.tipo == Obiettivo::Tipo::scoprire)) {
                classi_obiettivo_ = a.classi;
                if (stato_.obiettivo.empty()) stato_.obiettivo = a.id;
            }
        }
        if (piano) {   // V3: area, bersagli e dottrina dal piano della missione originale
            stato_.obiettivo = std::string("piano: ") + nome_condotta(piano->condotta);
            area_.reset();
            raggio_area_m_ = 0.0;
            if (piano->area) area_ = piano->area->centro, raggio_area_m_ = piano->area->raggio_m;
            classi_obiettivo_ = piano->classi;
            if (piano->quota_m > 0.0) quota_crociera_ = piano->quota_m;
            velocita_crociera_ = piano->velocita_kn;
        }
        quota(v, t, quota_crociera_);
        if (g->classe == "los_angeles" || g->classe.rfind("los_angeles", 0) == 0) v.push_back({t, TipoOrdine::schiera_cala, 0.0, 0.0, 0});
        // i tubi carichi li arma il passo, qui sotto (carica_tubo e' un interruttore: due nello stesso passo disarmano)
        if (!piano)
            cambia(t, area_ ? StatoComandante::Fase::transito : StatoComandante::Fase::pattuglia,
                   area_ ? "verso l'area dell'obiettivo" : "pattuglia sul posto", m);
        else
            cambia(t, StatoComandante::Fase::transito, std::string("piano della missione: ") + nome_condotta(piano->condotta), m);
    }

    // colpito: ritirata verso la partenza
    for (; colpi_visti_ < m.colpi().size(); ++colpi_visti_)
        if (m.colpi()[colpi_visti_].vittima == g->id) colpito_ = true;
    // i siluri nei tubi e in rastrelliera; i tubi vuoti si ricaricano (la ricarica realistica: 300 s sul 688)
    int siluri = armi.scorte(Arma::mk48) + armi.scorte(Arma::type53);
    for (int k = 1; k <= armi.numero_tubi(); ++k) {
        siluri += e_siluro(armi.arma(k)) && armi.stato(k) != StatoTubo::vuoto;
        if (armi.stato(k) == StatoTubo::vuoto && arma_da_armare(armi.arma(k)) && armi.scorte(armi.arma(k)) > 0 &&
            t - ricarica_chiesta_[k] >= 30.0) {   // una richiesta ogni 30 s
            v.push_back({t, TipoOrdine::carica_tubo, 0.0, 0.0, k});
            ricarica_chiesta_[k] = t;
        }
        if (armi.stato(k) == StatoTubo::carico && arma_da_armare(armi.arma(k)) && armi.arma(k) != Arma::mina) v.push_back({t, TipoOrdine::carica_tubo, 0.0, 0.0, k});
    }
    const auto cc = contatti(m);
    guida_fili(m, v, t, io, cc);
    // il rapporto radio: il primo ostile dell'obiettivo (o un sottomarino ostile) che si vede per classe
    for (const auto& c : cc)
        if (c.ostile && !c.arma && !da_riferire_ && !riferiti_.count(c.id) && !(piano && piano->armi == PianoOriginale::Armi::solo_difesa) &&
            (c.sottomarino || std::find(classi_obiettivo_.begin(), classi_obiettivo_.end(), c.classe) != classi_obiettivo_.end()))
            da_riferire_ = c.id;

    // difesa: un'arma ostile in avvicinamento (un contatto nuovo d'arma non nostra) prevale su tutto per DIFESA_S. Con i
    // Tomahawk del piano ancora da lanciare (SCENE0A: tutti entro 300 s) le esche partono, ma il battello resta a quota
    // periscopica e finisce i lanci: la fuga viene dopo
    const bool terra = piano && piano->condotta == PianoOriginale::Condotta::attacco_terra && !missili_terra_finiti_;
    for (const auto& c : cc) {
        // un'arma con la posizione nota la si fugge solo entro MINACCIA_M (quelle sul convoglio non ci cercano); senza
        // posizione, appena la si sente
        if (!c.arma || c.nostra || (c.posizione && distanza(io, c.pos) > MINACCIA_M)) continue;
        const bool nuova = minacce_.insert(c.id).second;
        if (terra) {   // durante i lanci: l'esca dal tubo, e si resta a lanciare
            if (nuova)
                for (int k = 1; k <= armi.numero_tubi(); ++k)
                    if (armi.arma(k) == Arma::esca && armi.stato(k) == StatoTubo::armato) { v.push_back({t, TipoOrdine::lancia, 0.0, 0.0, k}); break; }
            continue;
        }
        if (!fuggite_.insert(c.id).second) continue;   // una fuga per arma (anche per quelle viste durante i lanci)
        stato_.bersaglio = -1;
        cambia(t, StatoComandante::Fase::difesa, "siluro in arrivo: fuga e cambio di quota", m);
        fine_fase_ = t + DIFESA_S;
        rotta_data_ = -1.0;
        rotta(v, t, normalizza(c.rilevamento + 180.0), g->avanti_tutta, *g);
        const double q = g->quota + SALTO_QUOTA_M <= g->quota_max - 20.0 ? g->quota + SALTO_QUOTA_M : std::max(30.0, g->quota - SALTO_QUOTA_M);
        quota(v, t, q);
        // l'esca semovente di traverso alla minaccia, a un'altra quota, e l'esca fissa dal tubo
        for (int k = 1; k <= armi.numero_tubi(); ++k)
            if (armi.arma(k) == Arma::esca_mobile && armi.stato(k) == StatoTubo::armato) {
                v.push_back({t, TipoOrdine::imposta_siluro, normalizza(c.rilevamento + 90.0), static_cast<double>(CampoSiluro::rotta), k});
                v.push_back({t, TipoOrdine::imposta_siluro, g->quota, static_cast<double>(CampoSiluro::quota_corsa), k});
                v.push_back({t, TipoOrdine::lancia, 0.0, 0.0, k});
                break;
            }
        for (int k = 1; k <= armi.numero_tubi(); ++k)
            if (armi.arma(k) == Arma::esca && armi.stato(k) == StatoTubo::armato) { v.push_back({t, TipoOrdine::lancia, 0.0, 0.0, k}); break; }
        return v;
    }
    if (stato_.fase == StatoComandante::Fase::difesa) {
        if (t < fine_fase_) return v;
        quota(v, t, quota_crociera_);
        cambia(t, StatoComandante::Fase::pattuglia, "fine della fuga", m);
    }
    if (colpito_ || (siluri == 0 && !classi_obiettivo_.empty())) {
        cambia(t, StatoComandante::Fase::ritirata, colpito_ ? "colpiti: rientro" : "senza siluri: rientro", m);
        rotta(v, t, rilevamento_da(io, partenza_), 0.7 * g->avanti_tutta, *g);
        return v;
    }

    if (stato_.fase != StatoComandante::Fase::attacco && radio(m, v, t, *g)) return v;

    // V3: l'attesa sul posto, e i Tomahawk prima di tutto il resto
    if (piano && t < piano->attesa_s) {
        cambia(t, StatoComandante::Fase::attesa, "attesa sul posto, come da ordini", m);
        rotta(v, t, rotta_data_ >= 0.0 ? rotta_data_ : g->rotta, VELOCITA_DERIVA_KN, *g);
        return v;
    }
    if (piano && piano->condotta == PianoOriginale::Condotta::attacco_terra && !missili_terra_finiti_ && attacco_terra(m, v, t, cc, *g, st))
        return v;

    // il bersaglio: un ostile su cui le armi sono libere, prima quelli dell'obiettivo, poi il piu' vicino per segnale
    // con le mine da posare il campo viene prima: niente attacchi (resta la difesa), si passa in silenzio
    const Contatto* scelto = nullptr;
    for (const auto& c : cc) {
        if (minare_) break;
        if (!armi_libere(m, c) || lanci_su_[c.id] >= LANCI_PER_BERSAGLIO) continue;
        const bool dell_obiettivo = std::find(classi_obiettivo_.begin(), classi_obiettivo_.end(), c.classe) != classi_obiettivo_.end();
        if (!scelto || (dell_obiettivo && std::find(classi_obiettivo_.begin(), classi_obiettivo_.end(), scelto->classe) == classi_obiettivo_.end()))
            scelto = &c;
    }
    if (scelto) {
        const Contatto& c = *scelto;
        std::optional<Vec2> dove;
        if (tma(m, v, t, io, c, *g, true, dove)) return v;
        const double dist = distanza(io, *dove);
        // una nave oltre la portata dei siluri ed entro quella del missile: Harpoon o TASM dal tubo armato
        if (!c.sottomarino && dist > DISTANZA_LANCIO_M && dist <= PORTATA_MISSILE_M && t - ultimo_lancio_ >= TRA_LANCI_S) {
            int tubo = -1;
            for (int k = 1; k <= armi.numero_tubi() && tubo < 0; ++k)
                if (missile(armi.arma(k)) && armi.stato(k) == StatoTubo::armato) tubo = k;
            // il pannello, sul bersaglio designato: con neutrali o amici creduti nella scatola non tira, come le navi
            // dell'IA, e si avvicina per il siluro. Nell'attacco_terra e nei lanci chiesti dalla missione no
            bool scatola_sporca = false;
            if (tubo >= 0 && g->bersaglio == c.id)
                if (const auto tm = m.tiro_missile(tubo))
                    scatola_sporca = std::any_of(tm->piano.avvisi.begin(), tm->piano.avvisi.end(),
                                                 [](Avviso a) { return a == Avviso::neutrali_in_area || a == Avviso::amici_in_area; });
            if (tubo >= 0 && !scatola_sporca) {   // il missile parte solo da quota periscopica (StazioneArmi::lancia)
                cambia(t, StatoComandante::Fase::attacco, "missile sulla nave", m);
                quota(v, t, QUOTA_ORDINE_PERISCOPICA_M);
                rotta(v, t, rotta_data_ >= 0.0 ? rotta_data_ : g->rotta, VELOCITA_QUOTA_PERISCOPICA_KN, *g);
                if (g->bersaglio != c.id) {
                    v.push_back({t, TipoOrdine::designa_bersaglio, 0.0, 0.0, c.id});
                } else if (g->quota <= QUOTA_PERISCOPICA_M) {
                    v.push_back({t, TipoOrdine::lancia, 0.0, 0.0, tubo});
                    ultimo_lancio_ = t;
                    ++lanci_su_[c.id];
                    missile_lanciato_ = true;
                }
                return v;
            }
        }
        if (missile_lanciato_) {   // di nuovo giu', alla quota di pattuglia
            missile_lanciato_ = false;
            quota(v, t, quota_crociera_);
        }
        if (dist > DISTANZA_LANCIO_M) {   // avvicinamento sulla soluzione
            cambia(t, StatoComandante::Fase::attacco, "avvicinamento alla distanza di lancio", m);
            rotta(v, t, rilevamento_da(io, *dove), VELOCITA_AVVICINAMENTO_KN, *g);
            return v;
        }
        // il lancio: un tubo armato con un siluro, designazione, impostazioni dalla soluzione
        if (t - ultimo_lancio_ >= TRA_LANCI_S) {
            int tubo = -1;
            for (int k = 1; k <= armi.numero_tubi() && tubo < 0; ++k)
                if (e_siluro(armi.arma(k)) && armi.stato(k) == StatoTubo::armato) tubo = k;
            if (tubo < 0) {
                for (int k = 1; k <= armi.numero_tubi(); ++k)
                    if (e_siluro(armi.arma(k)) && armi.stato(k) == StatoTubo::carico) { v.push_back({t, TipoOrdine::carica_tubo, 0.0, 0.0, k}); break; }
            } else {
                cambia(t, StatoComandante::Fase::attacco, "lancio sulla soluzione", m);
                const double r = rilevamento_da(io, *dove);
                v.push_back({t, TipoOrdine::designa_bersaglio, 0.0, 0.0, c.id});
                v.push_back({t, TipoOrdine::imposta_siluro, r, static_cast<double>(CampoSiluro::rotta), tubo});
                v.push_back({t, TipoOrdine::imposta_siluro, std::max(500.0, 0.7 * dist), static_cast<double>(CampoSiluro::rte), tubo});
                v.push_back({t, TipoOrdine::imposta_siluro, c.sottomarino ? g->quota : 15.0, static_cast<double>(CampoSiluro::quota_ricerca), tubo});
                // la sua soluzione per le statistiche della sandbox (R3: l'errore al lancio); con la posizione del contatto
                // la stima la prende la missione dal contatto
                if (const auto it = stime_.find(c.id); !c.posizione && it != stime_.end()) m.soluzione_giocatore(c.id, it->second);
                v.push_back({t, TipoOrdine::lancia, 0.0, 0.0, tubo});
                ultimo_lancio_ = t;
                ++lanci_su_[c.id];
            }
        }
        return v;
    }
    stato_.bersaglio = -1;

    // il punto nave, con la navigazione incerta e la stima cresciuta
    if (m.con_navigazione()) {
        const auto sigma = m.sigma_navigazione_m(g->id);
        const auto in_corso = m.punto_nave_in_corso();
        // sotto la banchisa non si sale: niente punto nave con l'antenna (missione_ghiaccio.cpp)
        if (sigma && *sigma > SIGMA_PUNTO_NAVE_M && in_corso && !in_corso->in_corso && !m.sotto_banchisa(g->id)) {
            cambia(t, StatoComandante::Fase::punto_nave, "la stima e' cresciuta: punto nave", m);
            quota(v, t, QUOTA_ORDINE_PERISCOPICA_M);
            rotta(v, t, g->rotta, 4.0, *g);
            if (g->quota <= QUOTA_PERISCOPICA_M) {
                if ((g->flag54 & 4) == 0 && t - ultima_antenna_ >= 10.0) {
                    v.push_back({t, TipoOrdine::antenna, 0.0, 0.0, 1});
                    antenna_su_ = true;
                    ultima_antenna_ = t;
                }
                std::optional<std::pair<std::string, double>> meglio;
                for (const auto& [id, val] : m.punti_nave_possibili())
                    if (val.possibile && !val.emissione && (!meglio || val.sigma_nm < meglio->second)) meglio = std::make_pair(id, val.sigma_nm);
                if (meglio) v.push_back({t, TipoOrdine::punto_nave, 0.0, 0.0, 0, meglio->first});
            }
            return v;
        }
        if (stato_.fase == StatoComandante::Fase::punto_nave && in_corso && !in_corso->in_corso) {
            if (antenna_su_) { v.push_back({t, TipoOrdine::antenna, 0.0, 0.0, 0}); antenna_su_ = false; }
            quota(v, t, quota_crociera_);
        }
        if (in_corso && in_corso->in_corso) return v;
    }

    // le mine (Mk 67, semoventi): da entro la corsa, una per tubo armato con rotta e corsa su un punto del campo,
    // sparpagliate sul cerchio dell'obiettivo; il canale sorvegliato resta lontano
    if (minare_ && area_ && distanza(io, *area_) <= CORSA_MINA_M) {
        int tubo = -1, da_armare = -1, in_tubo = 0;
        for (int k = 1; k <= armi.numero_tubi(); ++k) {
            if (armi.arma(k) != Arma::mina) continue;
            in_tubo += armi.stato(k) != StatoTubo::vuoto;
            if (armi.stato(k) == StatoTubo::armato && tubo < 0) tubo = k;
            if (armi.stato(k) == StatoTubo::carico && da_armare < 0) da_armare = k;
        }
        if (in_tubo == 0 && armi.scorte(Arma::mina) == 0) {
            minare_ = false;
            cambia(t, StatoComandante::Fase::pattuglia, "mine posate", m);
        } else {
            cambia(t, StatoComandante::Fase::mine, "posa delle mine da " + std::to_string(static_cast<int>(distanza(io, *area_) / 1000.0)) + " km", m);
            rotta(v, t, rilevamento_da(io, *area_), VELOCITA_PATTUGLIA_KN, *g);
            if (tubo >= 0 && t - ultimo_lancio_ >= TRA_MINE_S) {
                // il punto: sul cerchio del campo, a ventaglio attorno al centro
                const double r = std::min(raggio_area_m_ > 0.0 ? raggio_area_m_ : 1000.0, 1500.0) * 0.6;
                const Vec2 punto = mine_posate_ == 0 ? *area_ : lungo(*area_, 72.0 * (mine_posate_ % 5) + 36.0 * (mine_posate_ / 5 % 2), r);
                v.push_back({t, TipoOrdine::imposta_siluro, rilevamento_da(io, punto), static_cast<double>(CampoSiluro::rotta), tubo});
                v.push_back({t, TipoOrdine::imposta_siluro, distanza(io, punto), static_cast<double>(CampoSiluro::rte), tubo});
                v.push_back({t, TipoOrdine::lancia, 0.0, 0.0, tubo});
                ultimo_lancio_ = t;
                ++mine_posate_;
            } else if (tubo < 0 && da_armare >= 0) {
                v.push_back({t, TipoOrdine::carica_tubo, 0.0, 0.0, da_armare});
            }
            return v;
        }
    }

    // V3: la condotta del piano (transito, scorta, pedinamento, evasione, l'uscita a missione compiuta)
    if (piano && condotta(m, v, t, io, cc, *g)) return v;

    // transito verso l'area, poi pattuglia con gli sprint and drift
    if (area_ && distanza(io, *area_) > std::max(ARRIVO_AREA_M, 0.5 * raggio_area_m_)) {
        cambia(t, StatoComandante::Fase::transito, "verso l'area dell'obiettivo", m);
        rotta(v, t, rilevamento_da(io, verso(m, t, io, *area_)), velocita_crociera_ > 0.0 ? std::max(velocita_crociera_, VELOCITA_TRANSITO_KN) : VELOCITA_TRANSITO_KN, *g);
        return v;
    }
    cambia(t, StatoComandante::Fase::pattuglia, area_ ? "pattuglia nell'area" : "pattuglia sul posto", m);
    const double ciclo = SPRINT_S + DERIVA_S;
    const double nel_ciclo = std::fmod(t, ciclo);
    const Vec2 centro = area_.value_or(partenza_);
    if (nel_ciclo < PASSO_S + 1e-9 || rotta_data_ < 0.0) ++gamba_pattuglia_;
    const Vec2 punto = lungo(centro, 90.0 * (gamba_pattuglia_ % 4), 0.5 * LATO_PATTUGLIA_M);
    rotta(v, t, rilevamento_da(io, punto), nel_ciclo < SPRINT_S ? VELOCITA_SPRINT_KN : velocita_crociera_ > 0.0 ? velocita_crociera_ : VELOCITA_PATTUGLIA_KN, *g);
    return v;
}

// la TMA sul contatto (vedi l'header): un rilevamento ogni 20 s, una soluzione al minuto; senza posizione le gambe della
// dottrina della sua marina e, se `ping` e la TMA non basta, un impulso del sonar per la distanza
bool ComandanteIa::tma(Missione& m, std::vector<Ordine>& v, double t, Vec2 io, const Contatto& c, const UnitaClassica& g, bool ping,
                       std::optional<Vec2>& dove) {
    if (stato_.bersaglio != c.id) {
        stato_.bersaglio = c.id;
        manovre_.reset();
        stato_.gambe = 0;
        inizio_tma_ = t;
    }
    // la propria TMA: un rilevamento ogni 20 s, una soluzione al minuto
    auto& traccia = tma_[c.id];
    if (t - ultimo_ril_[c.id] >= RILEVAMENTO_OGNI_S || ultimo_ril_[c.id] == 0.0) {
        traccia.aggiungi({t, io, c.rilevamento});
        ultimo_ril_[c.id] = t;
    }
    if (t - ultima_soluzione_[c.id] >= SOLUZIONE_OGNI_S) {
        ultima_soluzione_[c.id] = t;
        // il livello: quanti rilevamenti prima di fidarsi della soluzione
        if (traccia.rilevamenti().size() >= rilevamenti_minimi(o_.livello))
            if (const auto s = risolvi_tma(traccia, t, SIGMA_RILEVAMENTO_DEG); s && s->osservabile) {
                stime_[c.id] = *s;
                m.stima_tma_giocatore(c.id, s->soluzione.rotta);   // per gli obiettivi con da_tma, come il MATE del giocatore
            }
    }
    if (c.posizione) dove = c.pos;
    else if (const auto it = stime_.find(c.id); it != stime_.end() && t - ultima_soluzione_[c.id] < 2.0 * SOLUZIONE_OGNI_S)
        dove = it->second.posizione;
    if (!dove) {   // le gambe di TMA
        if (!manovre_) manovre_.emplace(m.dottrina_tma_ia(g.id), caso_.prossimo());   // la dottrina della sua marina
        SituazioneTma x;
        x.t = t;
        x.pos = io;
        x.rotta = g.rotta;
        x.velocita_kn = g.velocita;
        x.rilevamento = c.rilevamento;
        if (const auto it = stime_.find(c.id); it != stime_.end()) x.stima = it->second;
        x.schiera = (g.flag54 & 1) != 0;
        x.quota_m = g.quota;
        x.fondale_m = m.profondita(g.pos);
        const DecisioneTma d = manovre_->passo(x);
        stato_.gambe = d.gambe;
        // la TMA non basta (o il tempo stringe): un impulso del sonar per la distanza
        if (ping && (t - inizio_tma_ >= TMA_PRIMA_DEL_PING_S || d.gambe >= 2) && t - ultimo_ping_ >= PING_OGNI_S) {
            v.push_back({t, TipoOrdine::impulso_sonar, 0.0, 0.0, 0});
            ultimo_ping_ = t;
        }
        cambia(t, StatoComandante::Fase::tma, "TMA a gambe sul contatto", m);
        if (d.rinuncia == DecisioneTma::Rinuncia::no && !d.soluzione_pronta) rotta(v, t, d.rotta, d.velocita_kn, g);
        return true;
    }
    return false;
}

// V3: la posizione della PIM (la rotta pianificata del convoglio) al tempo t, `avanti_m` piu' avanti lungo la rotta;
// in `rotta` la direzione della PIM la'. Alla fine della rotta il convoglio e' arrivato e la PIM resta li'
Vec2 ComandanteIa::posizione_pim(double t, double avanti_m, double* rotta_pim) const {
    const auto& pim = *o_.piano->pim;
    double resto = std::max(0.0, pim.velocita_kn * KN * t + avanti_m);
    Vec2 p = pim.punti.front();
    double r = pim.punti.size() > 1 ? rilevamento_da(pim.punti[0], pim.punti[1]) : 0.0;
    for (std::size_t k = 1; k < pim.punti.size(); ++k) {
        const double tratto = distanza(pim.punti[k - 1], pim.punti[k]);
        r = rilevamento_da(pim.punti[k - 1], pim.punti[k]);
        if (resto <= tratto) { p = lungo(pim.punti[k - 1], r, resto); resto = 0.0; break; }
        resto -= tratto;
        p = pim.punti[k];
    }
    if (rotta_pim) *rotta_pim = r;
    return p;
}

// V3: seminare chi pedina. Un ostile sottomarino in contatto (o, con `sempre`, ogni TRA_SCATTI_S anche senza): uno
// scatto di SCATTO_S lontano da lui e dall'altra parte dello strato, poi la deriva lenta e silenziosa di traverso.
// Vero se occupa il passo
bool ComandanteIa::semina(Missione& m, std::vector<Ordine>& v, double t, const std::vector<Contatto>& cc, const UnitaClassica& g,
                          bool sempre, std::optional<double> lungo_rotta) {
    const Contatto* coda = nullptr;
    for (const auto& c : cc)
        if (c.ostile && c.sottomarino && !c.arma) coda = &c;
    const auto nuova_quota = [&] {
        const double fondo = m.profondita(g.pos) - SOPRA_FONDO_M;
        const double q = g.quota < 0.5 * (QUOTA_ALTA_M + QUOTA_BASSA_M) ? QUOTA_BASSA_M : QUOTA_ALTA_M;
        return std::clamp(q, QUOTA_ALTA_M, std::max(QUOTA_ALTA_M, std::min(fondo, g.quota_max - 20.0)));
    };
    if (t >= fine_deriva_ && (coda || (sempre && t >= prossimo_scatto_))) {
        // lontano da lui, con un lato a caso (la virata a gomito), o di traverso alla rotta senza contatto
        const double lato = caso_.uniforme() < 0.5 ? -45.0 : 45.0;
        // in transito lo scatto resta sulla rotta (a gomito di 45°, dal lato lontano da lui se e' davanti): si semina
        // andando avanti, non tornando indietro
        if (lungo_rotta) {
            const bool davanti = coda && scarto(coda->rilevamento, *lungo_rotta) < 60.0;
            const double via = coda ? normalizza(coda->rilevamento - *lungo_rotta) : 0.0;   // da che lato sta
            rotta_scatto_ = normalizza(*lungo_rotta + (davanti ? (via < 180.0 ? -60.0 : 60.0) : lato));
        } else {
            rotta_scatto_ = normalizza((coda ? coda->rilevamento + 180.0 : g.rotta + 90.0) + lato);
        }
        // non verso la costa: lo scatto dal lato con l'acqua piu' profonda a 3 km (il fondale della carta)
        const auto fondo_a = [&](double r) { const double f = m.profondita(lungo(g.pos, r, 3000.0)); return std::isfinite(f) ? f : 0.0; };
        if (fondo_a(rotta_scatto_) < QUOTA_ALTA_M + SOPRA_FONDO_M) {
            const double base = lungo_rotta ? *lungo_rotta : g.rotta;
            double meglio = rotta_scatto_, fondo_meglio = fondo_a(rotta_scatto_);
            for (const double d : {-60.0, -30.0, 0.0, 30.0, 60.0})
                if (const double f = fondo_a(normalizza(base + d)); f > fondo_meglio) meglio = normalizza(base + d), fondo_meglio = f;
            rotta_scatto_ = meglio;
        }
        rotta_deriva_ = lungo_rotta ? *lungo_rotta : normalizza(rotta_scatto_ + 90.0);
        fine_scatto_ = t + SCATTO_S;
        fine_deriva_ = fine_scatto_ + DERIVA_SEMINARE_S;
        prossimo_scatto_ = fine_deriva_ + TRA_SCATTI_S;
        rotta_data_ = -1.0;
        quota(v, t, nuova_quota());
    }
    if (t < fine_scatto_) {
        cambia(t, StatoComandante::Fase::seminare, coda ? "seminare: scatto oltre lo strato" : "pulizia del cono: scatto", m);
        rotta(v, t, rotta_scatto_, VELOCITA_SCATTO_KN, g);
        return true;
    }
    if (t < fine_deriva_) {
        cambia(t, StatoComandante::Fase::seminare, "seminare: deriva silenziosa", m);
        rotta(v, t, rotta_deriva_, VELOCITA_DERIVA_KN, g);
        return true;
    }
    return false;
}

// V3: i Tomahawk da quota periscopica sul bersaglio dei missili (la classe del piano), finche' ce ne sono; il TLAM-C
// (attacco_terra) parte senza bersaglio. Vero se occupa il passo
bool ComandanteIa::attacco_terra(Missione& m, std::vector<Ordine>& v, double t, const std::vector<Contatto>& cc, const UnitaClassica& g,
                                 const StazioniMissione& st) {
    const auto& armi = st.armi();
    if (inizio_terra_ < 0.0) inizio_terra_ = t;
    int tubo = -1, rimasti = armi.scorte(Arma::tomahawk) + armi.scorte(Arma::attacco_terra);
    for (int k = 1; k <= armi.numero_tubi(); ++k) {
        const Arma a = armi.arma(k);
        if (a != Arma::tomahawk && a != Arma::attacco_terra) continue;
        rimasti += armi.stato(k) != StatoTubo::vuoto;
        if (armi.stato(k) == StatoTubo::armato && tubo < 0) tubo = k;   // i tubi carichi li arma il passo (carica_tubo e' un interruttore)
    }
    if (rimasti == 0 || t >= CERCA_MISSILI_S + inizio_terra_) {
        missili_terra_finiti_ = true;
        if (g.flag54 & 2) v.push_back({t, TipoOrdine::periscopio, 0.0, 0.0, 0});
        if (antenna_su_) { v.push_back({t, TipoOrdine::antenna, 0.0, 0.0, 0}); antenna_su_ = false; }
        quota(v, t, quota_crociera_ > QUOTA_PERISCOPICA_M + 10.0 ? quota_crociera_ : QUOTA_PATTUGLIA_M);
        return false;
    }
    cambia(t, StatoComandante::Fase::attacco_terra, "Tomahawk da quota periscopica", m);
    quota(v, t, QUOTA_ORDINE_PERISCOPICA_M);
    rotta(v, t, rotta_data_ >= 0.0 ? rotta_data_ : g.rotta, VELOCITA_DERIVA_KN, g);   // piano: lontano dalla costa il punto si perde
    // il periscopio: il punto di partenza dei missili si vede da li' (il manuale della 0A: «raise your periscope... select TARGET»)
    if (g.quota <= QUOTA_PERISCOPICA_M && (g.flag54 & 2) == 0 && t - ultimo_periscopio_ >= 10.0) {
        v.push_back({t, TipoOrdine::periscopio, 0.0, 0.0, 1});
        ultimo_periscopio_ = t;
    }
    if (g.quota > QUOTA_PERISCOPICA_M || tubo < 0 || t - ultimo_lancio_ < TRA_MISSILI_TERRA_S) return true;
    if (armi.arma(tubo) == Arma::tomahawk) {   // il missile antinave vuole un bersaglio: quello dei missili
        // la classe del piano; se non e' classificato, il punto preimpostato: il contatto con la posizione, ne' ostile ne'
        // arma ne' classificato, piu' vicino (sulla carta e' la lettera del punto, dal periscopio)
        const Contatto* c = nullptr;
        for (const auto& k : cc)
            if (!k.arma && della_lista(k)) c = &k;
        if (!c)
            for (const auto& k : cc)
                if (!k.arma && !k.ostile && k.posizione && k.classe.empty() && (!c || distanza(io_, k.pos) < distanza(io_, c->pos))) c = &k;
        if (!c) return true;
        // il missile del modello (Enhanced, docs/21): il piano sul bersaglio designato, il BOL senza la distanza; si
        // lancia col piano valido e il missile pronto (quello della partenza lo e' gia')
        if (const auto tm = m.tiro_missile(tubo)) {
            if (g.bersaglio != c->id) {
                v.push_back({t, TipoOrdine::designa_bersaglio, 0.0, 0.0, c->id});
                return true;
            }
            if (tm->fase == FasePreparazione::spento) {
                v.push_back({t, TipoOrdine::prepara_missile, 1.0, 0.0, tubo});
                return true;
            }
            if (tm->bersaglio && !tm->bersaglio->distanza_nota && tm->imp.modo != ModoLancio::bol)
                v.push_back({t, TipoOrdine::imposta_missile, 1.0, static_cast<double>(CampoMissile::modo), tubo});
            if (!tm->rdy || !tm->slide_libera) return true;
        } else {
            v.push_back({t, TipoOrdine::designa_bersaglio, 0.0, 0.0, c->id});
        }
    }
    if (armi.arma(tubo) == Arma::attacco_terra)   // il TLAM del modello: la prima missione della libreria, poi l'ordine
        if (const auto tm = m.tiro_missile(tubo)) {
            if (tm->missione_tlam < 0 && !m.libreria_tlam().empty()) {
                v.push_back({t, TipoOrdine::imposta_missile, 0.0, static_cast<double>(CampoMissile::missione_tlam), tubo});
                return true;
            }
            if (!tm->rdy || !tm->slide_libera) return true;   // fuori area, fuori finestra o senza l'ordine d'esecuzione
        }
    v.push_back({t, TipoOrdine::lancia, 0.0, 0.0, tubo});
    ultimo_lancio_ = t;
    return true;
}

// V3: la condotta del piano, dopo attacchi, punto nave e mine. Vero se occupa il passo; falso: la pattuglia nell'area
bool ComandanteIa::condotta(Missione& m, std::vector<Ordine>& v, double t, Vec2 io, const std::vector<Contatto>& cc, const UnitaClassica& g) {
    const PianoOriginale& p = *o_.piano;
    const double crociera = velocita_crociera_ > 0.0 ? velocita_crociera_ : VELOCITA_TRANSITO_KN;
    // l'uscita a missione compiuta (o subito, se il piano non ha altro): li' si resta
    const auto verso_uscita = [&](const char* motivo) {
        if (!p.uscita) return false;
        verso_uscita_ = !p.limite || !p.limite->dentro(p.uscita->centro);
        if (distanza(io, p.uscita->centro) <= std::max(PUNTO_RAGGIUNTO_M, 0.5 * p.uscita->raggio_m)) uscito_ = true;
        if (uscito_) {
            cambia(t, StatoComandante::Fase::pattuglia, "arrivati all'uscita", m);
            rotta(v, t, rotta_data_ >= 0.0 ? rotta_data_ : g.rotta, VELOCITA_DERIVA_KN, g);
            return true;
        }
        cambia(t, StatoComandante::Fase::transito, motivo, m);
        rotta(v, t, rilevamento_da(io, verso(m, t, io, p.uscita->centro)), crociera, g);
        return true;
    };
    switch (p.condotta) {
        case PianoOriginale::Condotta::attacco_terra:
            return missili_terra_finiti_ && verso_uscita("missili lanciati: verso l'uscita");
        case PianoOriginale::Condotta::transito: {
            while (punto_ < p.punti.size() && distanza(io, p.punti[punto_]) <= PUNTO_RAGGIUNTO_M) ++punto_;
            const std::optional<Vec2> prossimo = punto_ < p.punti.size() ? std::optional<Vec2>(p.punti[punto_])
                                                 : p.uscita ? std::optional<Vec2>(p.uscita->centro) : std::nullopt;
            if (p.seminare && !uscito_ && semina(m, v, t, cc, g, false, prossimo ? std::optional<double>(rilevamento_da(io, *prossimo)) : std::nullopt))
                return true;
            if (punto_ < p.punti.size()) {
                verso_uscita_ = p.limite && !p.limite->dentro(p.punti[punto_]);
                cambia(t, StatoComandante::Fase::transito, "transito sui punti del piano", m);
                rotta(v, t, rilevamento_da(io, verso(m, t, io, p.punti[punto_])), crociera, g);
                return true;
            }
            return verso_uscita("verso l'uscita");
        }
        case PianoOriginale::Condotta::evasione:
            if (semina(m, v, t, cc, g, true, std::nullopt)) return true;
            cambia(t, StatoComandante::Fase::seminare, "lento e silenzioso", m);
            rotta(v, t, rotta_data_ >= 0.0 ? rotta_data_ : g.rotta, VELOCITA_DERIVA_KN, g);
            return true;
        case PianoOriginale::Condotta::scorta: {
            double r = 0.0;
            const Vec2 stazione = posizione_pim(t, p.avanti_m, &r);
            const double d = distanza(io, stazione);
            const double pim_kn = p.pim->velocita_kn;
            if (d > STAZIONE_M) {   // verso la stazione, piu' svelti del convoglio
                cambia(t, StatoComandante::Fase::scorta, "verso la stazione davanti al convoglio", m);
                rotta(v, t, rilevamento_da(io, verso(m, t, io, stazione)), std::min(VELOCITA_STAZIONE_KN, pim_kn + 2.0 + d / 1000.0), g);
            } else {                // in stazione: la rotta del convoglio alla sua velocita'
                cambia(t, StatoComandante::Fase::scorta, "in stazione davanti al convoglio", m);
                rotta(v, t, r, std::max(VELOCITA_DERIVA_KN, pim_kn), g);
            }
            return true;
        }
        case PianoOriginale::Condotta::pedinamento: {
            // la classe del piano, o prima della classificazione il sottomarino ostile (il colore sulla carta)
            auto c = std::find_if(cc.begin(), cc.end(), [&](const Contatto& k) { return !k.arma && della_lista(k); });
            if (c == cc.end())
                c = std::find_if(cc.begin(), cc.end(), [&](const Contatto& k) { return !k.arma && !k.classificato && k.ostile && k.sottomarino; });
            if (c == cc.end()) {
                if (!ultima_vista_) return false;   // mai visto: la ricerca nell'area
                if (distanza(io, *ultima_vista_) <= PUNTO_RAGGIUNTO_M) { ultima_vista_.reset(); return false; }
                cambia(t, StatoComandante::Fase::pedinamento, "contatto perso: verso l'ultima posizione", m);
                rotta(v, t, rilevamento_da(io, *ultima_vista_), crociera, g);
                return true;
            }
            if (designato_ != c->id) {   // il bersaglio designato: le originali contano il pedinamento solo cosi' (L_025b)
                v.push_back({t, TipoOrdine::designa_bersaglio, 0.0, 0.0, c->id});
                designato_ = c->id;
            }
            std::optional<Vec2> dove;
            if (tma(m, v, t, io, *c, g, false, dove)) return true;   // senza impulsi: il pedinamento resta nascosto
            ultima_vista_ = *dove;
            const double d = distanza(io, *dove);
            if (p.ping_m > 0.0 && d <= p.ping_m && !ping_fatto_) {   // «move close to the intruder and activate sonar»
                v.push_back({t, TipoOrdine::impulso_sonar, 0.0, 0.0, 0});
                ping_fatto_ = true;
            }
            const double vicino = p.ping_m > 0.0 && !ping_fatto_ ? 0.8 * p.ping_m : 0.4 * p.distanza_m;
            if (d > vicino) {
                cambia(t, StatoComandante::Fase::pedinamento, "pedinamento: avvicinamento", m);
                rotta(v, t, rilevamento_da(io, *dove), d > 0.8 * p.distanza_m ? VELOCITA_AVVICINAMENTO_KN : crociera, g);
            } else {
                const auto s = stime_.find(c->id);
                const double r = s != stime_.end() ? s->second.soluzione.rotta : rilevamento_da(io, *dove);
                cambia(t, StatoComandante::Fase::pedinamento, "pedinamento: in coda", m);
                rotta(v, t, r, VELOCITA_PATTUGLIA_KN, g);
            }
            return true;
        }
        case PianoOriginale::Condotta::caccia:
        case PianoOriginale::Condotta::ricognizione:
        case PianoOriginale::Condotta::minamento:
            return false;
    }
    return false;
}

// la guida a filo dei propri siluri: la dottrina dell'IA del livello, coi comandi via filo del giocatore
void ComandanteIa::guida_fili(const Missione& m, std::vector<Ordine>& v, double t, Vec2 io, const std::vector<Contatto>& cc) {
    const auto* g = m.giocatore();
    for (const auto& [id, u] : m.unita())
        if (u.lanciatore == g->id && !fili_.count(id))
            if (const Siluro* s = m.siluro(id); s && s->filo()) fili_.emplace(id, GuidaFiloIa(m.dottrina_filo_ia(o_.livello)));
    std::map<int, std::vector<EventoSiluro>> eventi;
    const auto& es = m.eventi_siluri();
    for (; eventi_siluri_visti_ < es.size(); ++eventi_siluri_visti_) eventi[es[eventi_siluri_visti_].siluro].push_back(es[eventi_siluri_visti_].evento);
    vincoli_ = {};
    for (auto it = fili_.begin(); it != fili_.end();) {
        const Siluro* s = m.siluro(it->first);
        const auto w = m.unita().find(it->first);
        if (!s || !s->filo() || it->second.tagliato() || w == m.unita().end()) { it = fili_.erase(it); continue; }
        SituazioneFilo x;
        x.t = t;
        x.siluro = s;
        x.eventi = eventi[it->first];
        x.pos_lanciatore = io;
        x.lanciatore_evade = stato_.fase == StatoComandante::Fase::difesa;
        // il bersaglio designato al lancio, come lo vede lui: la posizione del contatto, la soluzione o il rilevamento
        const int b = w->second.inseguito >= 0 ? w->second.inseguito : w->second.bersaglio;
        if (const auto c = std::find_if(cc.begin(), cc.end(), [&](const Contatto& k) { return k.id == b; }); c != cc.end()) {
            if (c->posizione) x.bersaglio_stimato = c->pos;
            else if (const auto st = stime_.find(b); st != stime_.end()) x.bersaglio_stimato = st->second.posizione;
            else x.rilevamento = c->rilevamento;
        }
        for (const ComandoFilo& k : it->second.passo(x)) {
            if (k.tipo == ComandoFiloTipo::spegni) { v.push_back({t, TipoOrdine::spegni_siluro, 0.0, 0.0, it->first}); continue; }
            if (k.tipo == ComandoFiloTipo::taglia) { v.push_back({t, TipoOrdine::taglia_filo, 0.0, 0.0, it->first}); continue; }
            // il valore nel formato dell'ordine del giocatore (StazioniMissione): a, il tipo in b
            const double a = k.tipo == ComandoFiloTipo::velocita ? static_cast<double>(k.velocita)
                           : k.tipo == ComandoFiloTipo::modo ? static_cast<double>(k.modo)
                           : k.tipo == ComandoFiloTipo::rifiuta || k.tipo == ComandoFiloTipo::assegna ? static_cast<double>(k.sorgente) : k.valore;
            v.push_back({t, TipoOrdine::comando_filo, a, static_cast<double>(static_cast<int>(k.tipo)), it->first});
        }
        if (const VincoliFilo f = it->second.vincoli(); f.attivi) {
            vincoli_.velocita_max_kn = vincoli_.attivi ? std::min(vincoli_.velocita_max_kn, f.velocita_max_kn) : f.velocita_max_kn;
            vincoli_.virata_max_deg_s = vincoli_.attivi ? std::min(vincoli_.virata_max_deg_s, f.virata_max_deg_s) : f.virata_max_deg_s;
            vincoli_.attivi = true;
        }
        ++it;
    }
}

// la radio: il rapporto di contatto e l'ascolto del broadcast, a quota periscopica con l'antenna. Vero se occupa il passo
bool ComandanteIa::radio(Missione& m, std::vector<Ordine>& v, double t, const UnitaClassica& g) {
    const SalaRadio* r = m.radio();
    if (!r) return false;
    const bool a_quota = g.quota <= QUOTA_PERISCOPICA_M;
    const auto su = [&] {
        quota(v, t, QUOTA_ORDINE_PERISCOPICA_M);
        rotta(v, t, rotta_data_ >= 0.0 ? rotta_data_ : g.rotta, VELOCITA_QUOTA_PERISCOPICA_KN, g);
        // l'antenna, di nuovo se non e' fuori (l'Enhanced la ammaina da se', per la quota o il mare)
        if (a_quota && (g.flag54 & 4) == 0 && t - ultima_antenna_ >= 10.0) {
            v.push_back({t, TipoOrdine::antenna, 0.0, 0.0, 1});
            antenna_su_ = true;
            ultima_antenna_ = t;
        }
        return a_quota && (g.flag54 & 4) != 0;   // l'antenna e' fuori
    };
    if (da_riferire_ && t - ultimo_rapporto_ >= RAPPORTO_OGNI_S) {
        cambia(t, StatoComandante::Fase::radio, "rapporto di contatto: quota periscopica", m);
        if (su()) {
            v.push_back({t, TipoOrdine::designa_bersaglio, 0.0, 0.0, *da_riferire_});
            v.push_back({t, TipoOrdine::radio_trasmetti, 0.0, 0.0, static_cast<int>(TipoTrasmissione::contatto)});
            riferiti_.insert(*da_riferire_);
            da_riferire_.reset();
            ultimo_rapporto_ = t;
        }
        return true;
    }
    const double f = r->prossima_finestra(t);
    if (stato_.fase != StatoComandante::Fase::tma && f >= 0.0 && t >= f - ASCOLTO_PRIMA_S && t <= f + r->tolleranza_finestra_s() &&
        (ascolto_ || t - ultimo_ascolto_ >= ASCOLTO_OGNI_S)) {
        ascolto_ = true;
        cambia(t, StatoComandante::Fase::radio, "ascolto del broadcast", m);
        su();
        return true;
    }
    if (ascolto_ || (antenna_su_ && stato_.fase != StatoComandante::Fase::punto_nave)) {
        if (ascolto_) ultimo_ascolto_ = t;
        ascolto_ = false;
        if (antenna_su_) { v.push_back({t, TipoOrdine::antenna, 0.0, 0.0, 0}); antenna_su_ = false; }
        quota(v, t, quota_crociera_);
    }
    return false;
}

}  // namespace m688
