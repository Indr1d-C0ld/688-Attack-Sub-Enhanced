#include "missioni/stazioni_missione.hpp"

#include "missioni/danni_sottomarini.hpp"
#include "nucleo/comando.hpp"

#include <algorithm>
#include <cmath>
#include <set>

namespace m688 {

namespace {

constexpr double AKKORD_SOGLIA = 0.5;   // efficienza della direzione del tiro sotto cui Akkord e' guasto (STIMA)
bool alfa(const Missione& m) { return m.giocatore() && m.giocatore()->classe == "alfa"; }
}  // namespace

StazioniMissione::StazioniMissione(const Missione& m, std::uint32_t seme)
    : armi_(m.tubi(), m.dotazione(), alfa(m) ? 6 : 7, alfa(m) ? 6 : 4), guasti_(seme),
      integrita_iniziale_(m.giocatore() ? std::max(1, static_cast<int>(m.giocatore()->integrita)) : 1),
      rumore_(seme ^ 0x7a7au) {}

void StazioniMissione::imposta_carico(Missione& m, const CaricoPartenza& c, const CatalogoCarico& cat) {
    catalogo_ = &cat;
    armi_.imposta_carico(c, cat, parametri_ricarica(m.giocatore() ? m.giocatore()->classe : std::string{}));
    std::vector<std::string> famiglie;
    for (int t = 1; t <= armi_.numero_tubi(); ++t) famiglie.emplace_back(nome_arma(armi_.arma(t)));
    m.imposta_tubi(famiglie);
}

int StazioniMissione::percento_operativo(const Missione& m) const {
    const auto* g = m.giocatore();
    return g ? std::max(1, static_cast<int>(g->integrita * 100 / integrita_iniziale_)) : 1;
}

bool StazioniMissione::fuori_uso(const Missione& m, SistemaClassico s) const {
    if (!danni_) return guasti_.guasto(s);
    const auto* g = m.giocatore();
    if (!g) return false;
    const LimitiBattello l = danni_->limiti(g->id);
    switch (s) {
        case SistemaClassico::periscopio: if (!l.periscopio) return true; break;
        case SistemaClassico::antenna: if (!l.antenna) return true; break;
        case SistemaClassico::schiera: if (!l.schiera) return true; break;
        case SistemaClassico::calcolatore_sonar:
        case SistemaClassico::sonar_di_tiro: if (!l.sonar) return true; break;
        case SistemaClassico::motori: break;
    }
    const auto* sistemi = danni_->sistemi(g->id);   // i primi sei hanno l'ordine dell'originale
    const auto i = static_cast<std::size_t>(s);
    return sistemi && i < sistemi->size() && (*sistemi)[i].efficienza <= 0.0;
}

int StazioniMissione::analizza(const Missione& m) const {
    if (fuori_uso(m, SistemaClassico::calcolatore_sonar)) return GuastiGiocatore::annuncio(SistemaClassico::calcolatore_sonar);
    return sonar_.analizza(situazione_sonar(m));
}

int StazioniMissione::rifiuto_per_guasto(const Missione& m, const Ordine& o) const {
    auto se = [&](SistemaClassico s) { return fuori_uso(m, s) ? GuastiGiocatore::annuncio(s) : 0; };
    if (danni_ && m.giocatore()) {   // Enhanced: limiti del battello danneggiato
        const LimitiBattello l = danni_->limiti(m.giocatore()->id);
        switch (o.tipo) {
            case TipoOrdine::timone:
            case TipoOrdine::barra:
            case TipoOrdine::accosta_per: if (!l.timone) return MSG_TIMONE_BLOCCATO; break;
            case TipoOrdine::quota:
            case TipoOrdine::quota_periscopica:
            case TipoOrdine::quota_150_piedi:
            case TipoOrdine::quota_con_angolo:
            case TipoOrdine::angolo_immersione: if (!l.idroplani) return MSG_IDROPLANI_BLOCCATI; break;
            case TipoOrdine::giri:   // i giri sono circa la frazione della velocita' massima
                if (o.a > l.velocita + 1e-9) return GuastiGiocatore::annuncio(SistemaClassico::motori);
                break;
            case TipoOrdine::telegrafo:   // oltre la frazione della massima (MAX, 1,09 di avanti tutta) che resta
                if (o.a > l.velocita * PostoDiComando::potenza(PostoDiComando::TELEGRAFO_POSIZIONI - 1) + 1e-9)
                    return GuastiGiocatore::annuncio(SistemaClassico::motori);
                break;
            default: break;
        }
    }
    switch (o.tipo) {
        case TipoOrdine::filtro_sonar: return se(SistemaClassico::calcolatore_sonar);
        case TipoOrdine::impulso_sonar: return se(SistemaClassico::sonar_di_tiro);
        case TipoOrdine::schiera_cala:
        case TipoOrdine::schiera_recupera: return se(SistemaClassico::schiera);
        case TipoOrdine::periscopio: return se(SistemaClassico::periscopio);   // ne' su ne' giu'
        case TipoOrdine::antenna: return se(SistemaClassico::antenna);
        case TipoOrdine::radio_trasmetti:   // Enhanced: si trasmette e si chiama il satellite con l'antenna
        case TipoOrdine::radio_ssixs: return se(SistemaClassico::antenna);
        case TipoOrdine::telegrafo:   // MAX e arresto rapido (voce DS:028C), solo nel livello classico
            return !danni_ && guasti_.guasto(SistemaClassico::motori) && (o.n == 1 || o.a > 1.0) ? MSG_MOTORI_NON_REGGONO : 0;
        default: return 0;
    }
}

SituazioneArmi StazioniMissione::situazione_armi(const Missione& m) const {
    SituazioneArmi s;
    const auto* g = m.giocatore();
    if (!g) return s;
    s.quota = g->quota;
    if (const auto it = m.unita().find(armi_.bersaglio()); it != m.unita().end()) {
        const auto& b = it->second;
        s.bersaglio_presente = true;
        s.bersaglio_nave = b.categoria == "nave";
        if (const auto c = g->contatti.find(b.id); c != g->contatti.end()) {
            s.contatto = true;
            s.traccia_solida = c->second.posizione_nota;   // classico: qualita' >= 800
        }
        s.distanza_m = distanza(g->pos, b.pos);
    }
    if (const auto it = m.unita().find(armi_.arma_selezionata()); it != m.unita().end()) {
        const auto& a = it->second;
        s.arma_presente = true;
        if (const Siluro* sil = m.siluro(a.id)) s.arma_guidata = sil->filo() && a.lanciatore == g->id;   // Enhanced
        else s.arma_guidata = a.collegata == g->id && (a.classe == "mk48" || a.classe == "type53");
    }
    if (m.con_missili())
        for (int t = 1; t < static_cast<int>(s.missile_tubo.size()); ++t)
            if (const auto tm = m.tiro_missile(t)) s.missile_tubo[static_cast<std::size_t>(t)] = tm->rdy && tm->slide_libera ? 2 : 1;
    return s;
}

SituazioneSonar StazioniMissione::situazione_sonar(const Missione& m) const {
    SituazioneSonar s;
    const auto* g = m.giocatore();
    if (!g) return s;
    s.velocita_ordinata = g->velocita_ordinata;
    s.avanti_tutta = g->avanti_tutta;
    if (const auto it = m.unita().find(armi_.bersaglio()); it != m.unita().end()) {
        s.bersaglio = true;
        s.contatto = g->contatti.count(it->first) > 0;
        s.aereo = it->second.categoria == "aereo";
    }
    return s;
}

int StazioniMissione::contatto_in_vista(const Missione& m, double rilevamento_vista, double semiampiezza) const {
    const auto* g = m.giocatore();
    if (!g) return -1;
    int scelto = -1;
    double migliore = semiampiezza;
    for (const auto& [id, c] : g->contatti) {
        const auto it = m.unita().find(id);
        if (it == m.unita().end() || it->second.lanciatore == g->id) continue;   // orfani e proprie armi
        const double scarto = std::fabs(normalizza_180(rilevamento(g->pos, it->second.pos) - rilevamento_vista));
        if (scarto <= migliore) { migliore = scarto; scelto = id; }
    }
    return scelto;
}

int StazioniMissione::prossimo_siluro(const Missione& m) const {
    const auto* g = m.giocatore();
    if (!g) return -1;
    int primo = -1;
    for (const auto& [id, u] : m.unita()) {
        if (u.collegata != g->id || (u.classe != "mk48" && u.classe != "type53")) continue;
        if (primo < 0) primo = id;
        if (id > armi_.arma_selezionata()) return id;
    }
    return primo;
}

bool StazioniMissione::filo(const Ordine& o) {
    return o.tipo == TipoOrdine::comando_filo || o.tipo == TipoOrdine::spegni_siluro || o.tipo == TipoOrdine::taglia_filo;
}

bool StazioniMissione::dinamica(const Ordine& o) {
    switch (o.tipo) {
        case TipoOrdine::barra:
        case TipoOrdine::accosta_per:
        case TipoOrdine::giri:
        case TipoOrdine::quota_con_angolo:
        case TipoOrdine::angolo_immersione: return true;
        default: return false;
    }
}

EsitoStazione StazioniMissione::posarsi(const Missione& m, double quota_collaudo, double pendenza_max) const {
    EsitoStazione e;
    const auto* g = m.giocatore();
    if (!g || !m.con_dinamica() || !m.dinamica(g->id)) return e;
    if (sonar_.schiera_esposta()) { e.messaggio = MSG_POSARSI_SCHIERA; return e; }
    const double fondo = m.profondita(g->pos);
    if (!std::isfinite(fondo) || (quota_collaudo > 0.0 && fondo > quota_collaudo)) { e.messaggio = MSG_POSARSI_PROFONDO; return e; }
    constexpr double PASSO = 50.0;   // pendenza dal fondale attorno al battello
    const double gx = (m.profondita({g->pos.x + PASSO, g->pos.y}) - m.profondita({g->pos.x - PASSO, g->pos.y})) / (2 * PASSO);
    const double gy = (m.profondita({g->pos.x, g->pos.y + PASSO}) - m.profondita({g->pos.x, g->pos.y - PASSO})) / (2 * PASSO);
    if (!std::isfinite(gx) || !std::isfinite(gy) || gradi(std::atan(std::hypot(gx, gy))) > pendenza_max) {
        e.messaggio = MSG_POSARSI_RIPIDO;
        return e;
    }
    // a macchine ferme gli idroplani non portano e senza casse di assetto il battello non scende: si scende piano
    // con l'abbrivio e si fermano le macchine vicino al fondo (dopo_passo). L'angolo negativo dell'ordine di quota
    // dice alla consegna che e' la discesa per posarsi
    const double t = m.tempo();
    const double giri = m.dinamica(g->id)->giri_per_velocita(3.0);
    e.alla_missione = {{t, TipoOrdine::giri, giri}, {t, TipoOrdine::quota_con_angolo, std::max(0.0, fondo - 1.0), -10.0}};
    e.messaggio = MSG_POSATI;
    return e;
}

// Enhanced: l'ordine cambia una parte dello stato dei comandi, che va alla dinamica intero; nel classico non vale
bool StazioniMissione::consegna_dinamica(Missione& m, const Ordine& o) {
    const auto* g = m.giocatore();
    const DinamicaSottomarino* d = g && m.con_dinamica() ? m.dinamica(g->id) : nullptr;
    if (!d) return false;
    if (!comandi_) {   // si parte da com'e' il battello, cosi' il primo ordine non cambia il resto
        comandi_ = ComandiSottomarino{};
        comandi_->barra_deg = d->barra_deg();
        comandi_->giri = d->giri();
        comandi_->quota_m = d->quota_m();
    }
    ComandiSottomarino& c = *comandi_;
    // un ordine di macchina o di quota dopo essersi posati: si scaricano le casse d'assetto
    if (appesantito_ && danni_ && (o.tipo == TipoOrdine::giri || o.tipo == TipoOrdine::quota_con_angolo ||
                                   o.tipo == TipoOrdine::angolo_immersione || o.tipo == TipoOrdine::emersione_emergenza ||
                                   o.tipo == TipoOrdine::immersione_emergenza)) {
        danni_->alleggerisci(g->id);
        appesantito_ = false;
    }
    // il timone (barra o accostata) disinserisce il pilota automatico, come nel classico
    if ((o.tipo == TipoOrdine::barra || o.tipo == TipoOrdine::accosta_per) && m.pilota_automatico())
        m.applica({o.tempo, TipoOrdine::pilota_automatico, 0, 0, 0});
    switch (o.tipo) {
        case TipoOrdine::barra: c.barra_deg = std::clamp(o.a, -DinamicaSottomarino::BARRA_MAX, DinamicaSottomarino::BARRA_MAX); c.rotta.reset(); break;
        case TipoOrdine::accosta_per: c.rotta = normalizza_360(o.a); if (o.b > 0.0) c.barra_max_deg = o.b; break;
        case TipoOrdine::giri: c.giri = std::clamp(o.a, DinamicaSottomarino::GIRI_INDIETRO, 1.0); break;
        case TipoOrdine::quota_con_angolo:   // b < 0: discesa per posarsi, con angolo |b|
            c.quota_m = std::max(0.0, o.a);
            if (o.b != 0.0) c.angolo_max_deg = std::fabs(o.b);
            if (o.b < 0.0) posarsi_a_ = c.quota_m;
            else posarsi_a_.reset();
            break;
        case TipoOrdine::angolo_immersione: c.quota_m.reset(); c.angolo_deg = o.a; break;
        case TipoOrdine::emersione_emergenza: c.quota_m = 0.0; c.angolo_max_deg = 30.0; break;   // e l'esaurimento (kit)
        case TipoOrdine::immersione_emergenza:   // avanti tutta e piani a scendere (dossier 16 §4.4)
            c.quota_m = m.quota_immersione_emergenza();
            c.angolo_max_deg = 30.0;
            c.giri = 1.0;
            break;
        default: return false;
    }
    m.comandi_sottomarino(g->id, c);
    // i giri danno la velocita' ordinata, quella che vedono il pilota automatico, il pannello e la schiera
    if (o.tipo == TipoOrdine::giri || o.tipo == TipoOrdine::immersione_emergenza)
        if (auto* u = m.trova(g->id)) u->velocita_ordinata = c.giri * d->parametri().velocita_max_kn;
    return true;
}

// --- Boksit (Alfa) ---

namespace {
bool stesso_punto(Vec2 a, Vec2 b) { return std::fabs(a.x - b.x) < 0.5 && std::fabs(a.y - b.y) < 0.5; }
}  // namespace

bool StazioniMissione::consegna_boksit(Missione& m, const Ordine& o) {
    const auto* g = m.giocatore();
    if (!g || !alfa(m) || !m.con_dinamica()) return false;
    if (o.tipo == TipoOrdine::programma_tappa) {   // quota e velocita' della tappa n (punto di rotta n, 1-9)
        const auto& punti = m.punti_di_rotta();
        if (o.n < 1 || o.n > static_cast<int>(punti.size())) return false;
        const Vec2 p = punti[static_cast<std::size_t>(o.n - 1)];
        std::erase_if(tappe_, [&](const TappaBoksit& t) { return stesso_punto(t.punto, p); });
        TappaBoksit t;
        t.punto = p;
        if (o.a >= 0.0) t.quota_m = o.a;
        if (o.b >= 0.0) t.velocita_kn = o.b;
        tappe_.push_back(t);
        return true;
    }
    const int asse = o.n == 0 ? 0 : 1;
    const auto modo = static_cast<ModoBoksit>(std::clamp(static_cast<int>(std::lround(o.a)), 0, 2));
    if (asse == 0 && modo == ModoBoksit::programma) {   // la rotta a programma: il pilota sui punti della carta
        if (m.punti_di_rotta().empty()) {
            aggiungi_messaggio(m, MSG_NESSUN_PUNTO);
            return false;
        }
        if (!m.pilota_automatico()) {
            if (!m.applica({o.tempo, TipoOrdine::pilota_automatico, 0.0, 0.0, 1})) return false;
            m.comandi_del_timoniere(g->id);
            comandi_.reset();
        }
        programma_in_corso_ = true;
        tappa_applicata_.reset();
        tappe_programma_ = static_cast<int>(m.punti_di_rotta().size());
    } else if (asse == 0 && modi_boksit_[0] == ModoBoksit::programma && m.pilota_automatico()) {
        m.applica({o.tempo, TipoOrdine::pilota_automatico, 0.0, 0.0, 0});   // si lascia il programma
        programma_in_corso_ = false;
    }
    modi_boksit_[static_cast<std::size_t>(asse)] = modo;
    return true;
}

int StazioniMissione::tappa_boksit(const Missione& m) const {
    if (!programma_in_corso_ || m.punti_di_rotta().empty()) return 0;
    // il pilota va verso il primo punto rimasto (quelli raggiunti si tolgono): la tappa e' quante ne sono passate + 1
    return std::max(1, tappe_programma_ - static_cast<int>(m.punti_di_rotta().size()) + 1);
}

// A ogni passo col programma: al punto nuovo la sua quota (se l'asse della quota e' a programma) e la sua velocita',
// come gli ordini dell'originale (il timoniere del pilota li segue); a programma finito, il messaggio
void StazioniMissione::passo_boksit(Missione& m) {
    if (!programma_in_corso_) return;
    const auto* g = m.giocatore();
    if (!g) return;
    if (!m.pilota_automatico() || m.punti_di_rotta().empty()) {   // arrivato, o il giocatore ha toccato il timone
        if (m.punti_di_rotta().empty()) aggiungi_messaggio(m, MSG_PROGRAMMA_FINITO);
        programma_in_corso_ = false;
        modi_boksit_[0] = ModoBoksit::automatico;
        tappa_applicata_.reset();
        return;
    }
    const Vec2 p = m.punti_di_rotta().front();
    if (tappa_applicata_ && stesso_punto(*tappa_applicata_, p)) return;
    if (tappa_applicata_) aggiungi_messaggio(m, MSG_TAPPA_BOKSIT);   // il punto di prima e' raggiunto
    tappa_applicata_ = p;
    const auto t = std::find_if(tappe_.begin(), tappe_.end(), [&](const TappaBoksit& x) { return stesso_punto(x.punto, p); });
    if (t == tappe_.end()) return;   // senza quota e velocita': quelle correnti
    if (t->quota_m && modi_boksit_[1] == ModoBoksit::programma) m.applica({m.tempo(), TipoOrdine::quota, *t->quota_m, 1.0});
    if (t->velocita_kn && g->avanti_tutta > 0.0)
        m.applica({m.tempo(), TipoOrdine::telegrafo, std::clamp(*t->velocita_kn / g->avanti_tutta, 0.0, 1.09)});
}

bool StazioniMissione::consegna(Missione& m, const Ordine& o) {
    if (const int r = rifiuto_per_guasto(m, o)) {
        aggiungi_messaggio(m, r);
        return false;
    }
    if (boksit(o)) return consegna_boksit(m, o);
    if (dinamica(o)) return consegna_dinamica(m, o);
    if (filo(o)) {   // Enhanced: comandi via filo al siluro del modello (n = siluro; b = tipo, a = valore)
        if (!m.con_siluri_enhanced()) return false;
        ComandoFilo c;
        if (o.tipo == TipoOrdine::spegni_siluro) c.tipo = ComandoFiloTipo::spegni;
        else if (o.tipo == TipoOrdine::taglia_filo) c.tipo = ComandoFiloTipo::taglia;
        else {
            c.tipo = static_cast<ComandoFiloTipo>(std::lround(o.b));
            c.valore = o.a;
            c.velocita = static_cast<VelocitaSiluro>(std::clamp(static_cast<int>(std::lround(o.a)), 0, 1));
            c.modo = static_cast<ModoTesta>(std::clamp(static_cast<int>(std::lround(o.a)), 0, 2));
            c.sorgente = static_cast<int>(std::lround(o.a));
        }
        const Siluro* s = m.siluro(o.n);
        if (!s || !s->filo() || !m.comanda_siluro(o.n, c)) {
            aggiungi_messaggio(m, s ? MSG_NON_GUIDATA : MSG_NESSUNA_ARMA);
            return false;
        }
        if (c.tipo == ComandoFiloTipo::spegni) aggiungi_messaggio(m, MSG_SILURO_SPENTO);
        return true;
    }
    if (o.tipo == TipoOrdine::pilota_automatico && o.n != 0 && comandi_ && m.giocatore()) {
        // Enhanced: col pilota inserito comanda il timoniere automatico verso i punti di rotta; i comandi del
        // giocatore ripartono dal battello com'e' al prossimo ordine di manovra
        const bool ok = m.applica(o);
        if (ok) { m.comandi_del_timoniere(m.giocatore()->id); comandi_.reset(); }
        return ok;
    }
    // col modello dinamico anche come primo ordine: l'emergenza sale e scende col massimo assetto (30°), qualunque sia
    // l'ordine di prima
    if (o.tipo == TipoOrdine::emersione_emergenza || o.tipo == TipoOrdine::immersione_emergenza) consegna_dinamica(m, o);
    if (o.tipo == TipoOrdine::radio_verifica) {   // Enhanced: il gruppo d'autenticazione del messaggio, a richiesta
        if (!m.radio()) return false;
        m.radio_verifica(o.n);
        return true;
    }
    if (o.tipo == TipoOrdine::radio_slot) {   // Enhanced: boa SLOT, trasmette dal punto di lancio dopo il ritardo
        const bool partita = m.radio() && m.radio_slot(o.a);
        if (!partita) aggiungi_messaggio(m, MSG_NESSUNA_BOA);
        return partita;
    }
    if (o.tipo == TipoOrdine::radio_filo || o.tipo == TipoOrdine::radio_trasmetti || o.tipo == TipoOrdine::radio_ssixs) {
        // Enhanced: sala radio della missione; l'esito della trasmissione (fix nemico) il comandante non lo sa
        if (!m.radio()) return false;
        if (o.tipo == TipoOrdine::radio_filo) {
            m.radio_filo(o.n != 0);
            return true;
        }
        const bool fatta = o.tipo == TipoOrdine::radio_ssixs ? m.radio_ssixs().fatta
                                                             : m.radio_trasmetti(static_cast<TipoTrasmissione>(std::clamp(o.n, 0, static_cast<int>(TipoTrasmissione::previsione)))).fatta;
        if (!fatta) aggiungi_messaggio(m, MSG_SERVE_ALBERO);
        return fatta;
    }
    EsitoStazione e;
    if (armi_.riguarda(o)) e = armi_.consegna(o, situazione_armi(m));
    else if (sonar_.riguarda(o)) e = sonar_.consegna(o, situazione_sonar(m));
    else return m.applica(o);
    bool applicato = !e.alla_missione.empty() || e.messaggio != 0;
    for (const auto& c : e.alla_missione) {
        // Enhanced: il siluro parte con le impostazioni del tubo, se il giocatore le ha toccate (altrimenti quelle
        // ricavate dal LAUNCH dell'originale)
        if (c.tipo == TipoOrdine::lancia && m.con_siluri_enhanced() && armi_.impostato(c.n))
            m.imposta_siluro_giocatore(armi_.impostazioni(c.n), armi_.campi_impostati(c.n));
        if (c.tipo == TipoOrdine::lancia && catalogo_) {   // carico M2: la famiglia del tubo e la variante uscita
            std::vector<std::string> famiglie;
            for (int t = 1; t <= armi_.numero_tubi(); ++t) famiglie.emplace_back(nome_arma(armi_.arma(t)));
            m.imposta_tubi(famiglie);
            const SchedaArma* a = catalogo_->arma(armi_.lanciata(c.n));   // le mine: il tipo per il campo minato
            // le mine e le esche semoventi da tubo (famiglia esca_mobile: MG-74, MOSS, [esche.*] di siluri.toml) col loro
            // id; gli altri col modello del siluro
            const bool per_id = a && (a->tipo == TipoArmaCarico::mina || a->famiglia == "esca_mobile");
            m.variante_tubo(c.n, !a ? std::string{} : per_id ? a->id : a->siluro);
        }
        applicato = m.applica(c) && applicato;
    }
    if (e.messaggio) aggiungi_messaggio(m, e.messaggio);
    return applicato;
}

void StazioniMissione::aggiungi_messaggio(const Missione& m, int codice) {
    if (m.giocatore()) messaggi_.push_back({m.tempo(), m.giocatore()->id, codice});
}

void StazioniMissione::dopo_passo(Missione& m) {
    const auto* g = m.giocatore();
    if (!g) return;
    // Enhanced: ricarica realistica (dossier 14 §5.4), attivata quando la missione passa ai siluri dell'Enhanced
    if (m.con_siluri_enhanced() && !armi_.ricarica_realistica()) armi_.attiva_ricarica(parametri_ricarica(g->classe));
    if (catalogo_) {   // la famiglia di ogni tubo anche quando cambia arma (carico, ricarica): la legge tiro_missile
        std::vector<std::string> famiglie;
        for (int t = 1; t <= armi_.numero_tubi(); ++t) famiglie.emplace_back(nome_arma(armi_.arma(t)));
        if (famiglie != m.tubi()) m.imposta_tubi(famiglie);
    }
    // Enhanced, Alfa: Akkord (dossier 19 §3.4): l'avaria dai danni (la direzione del tiro sotto il 50%, STIMA) blocca
    // tutte le armi; i bersagli usciti dal quadro lasciano il loro posto
    if (alfa(m) && m.con_dinamica()) passo_boksit(m);
    if (m.con_siluri_enhanced() && g->classe == "alfa") {
        armi_.attiva_akkord();
        const auto* sistemi = danni_ ? danni_->sistemi(g->id) : nullptr;
        const auto i = static_cast<std::size_t>(Sistema::sonar_tiro);
        const bool guasto = sistemi && i < sistemi->size() && (*sistemi)[i].efficienza < AKKORD_SOGLIA;
        if (const int c = armi_.imposta_guasto_akkord(guasto)) messaggi_.push_back({m.tempo(), g->id, c});
        std::set<int> presenti;
        for (const auto& [id, c] : g->contatti) presenti.insert(id);
        armi_.aggiorna_akkord(presenti);
    }
    {
        const EsitoRicarica r = armi_.avanza(m.tempo());
        for (const auto& c : r.alla_missione) m.applica(c);   // armamento dei tubi appena ricaricati
        for (const int c : r.messaggi) messaggi_.push_back({m.tempo(), g->id, c});
        for (int k = 0; k < r.transitori; ++k) m.evento_acustico(g->id, armi_.parametri_ricarica().colpo_s);
    }
    for (; letti_ < m.messaggi().size(); ++letti_) {
        const Messaggio x = m.messaggi()[letti_];
        if (x.unita != g->id) continue;
        sonar_.messaggio_missione(x.codice);
        if (const int i = danni_ ? -1 : guasti_.messaggio(x.codice); i >= 0) {   // 8B39: gli effetti del guasto (classico)
            const double t = m.tempo();
            switch (static_cast<SistemaClassico>(i)) {
                case SistemaClassico::calcolatore_sonar: sonar_.guasto_calcolatore(); break;
                case SistemaClassico::schiera:
                    if (sonar_.schiera_esposta())
                        for (const auto& c : sonar_.consegna(StazioneSonar::taglio(t), {}).alla_missione) m.applica(c);
                    break;
                case SistemaClassico::periscopio:
                    if (g->flag54 & 2) m.applica({t, TipoOrdine::periscopio, 0, 0, 0});
                    break;
                case SistemaClassico::antenna:
                    if (g->flag54 & 4) m.applica({t, TipoOrdine::antenna, 0, 0, 0});
                    break;
                case SistemaClassico::motori:   // da MAX a TUTTA (comando 6)
                    if (g->velocita_ordinata > g->avanti_tutta + 1e-9) m.applica({t, TipoOrdine::telegrafo, 1.0});
                    break;
                case SistemaClassico::sonar_di_tiro: break;
            }
            messaggi_.push_back({x.tempo, x.unita, GuastiGiocatore::annuncio(static_cast<SistemaClassico>(i))});
        }
        if (!nascosto(x.codice)) continue;
        if (const int c = armi_.messaggio_missione(x.codice)) messaggi_.push_back({x.tempo, x.unita, c});
    }
    // posarsi: la quota di arrivo segue il fondo sotto il battello (che con l'abbrivio si sposta); a 3 m dal fondo
    // si fermano le macchine e il battello si adagia
    if (posarsi_a_ && comandi_) {
        if (const DinamicaSottomarino* d = m.con_dinamica() ? m.dinamica(g->id) : nullptr) {
            const double fondo = m.profondita(g->pos);
            if (std::isfinite(fondo)) comandi_->quota_m = std::max(0.0, fondo - 1.0);
            if (d->quota_m() >= *comandi_->quota_m - 3.0) {
                comandi_->giri = 0.0;
                posarsi_a_.reset();
                if (danni_) {   // un po' pesante: si appoggia (2 t dalle valvole in circa 40 s)
                    danni_->appesantisci(m, g->id, 2.0);
                    appesantito_ = true;
                }
            }
            m.comandi_sottomarino(g->id, *comandi_);
        }
    }
    if (tma_attiva_ && m.tempo() >= prossimo_tma_) {   // TMA: un rilevamento ogni 5 s sul bersaglio col contatto
        prossimo_tma_ = m.tempo() + 5.0;
        const int b = armi_.bersaglio();
        if (b != bersaglio_tma_) { tma_.azzera(); bersaglio_tma_ = b; }
        const auto it = m.unita().find(b);
        if (it != m.unita().end() && g->contatti.count(b)) {
            const Contatto& c = g->contatti.at(b);
            if (m.con_acustica()) {   // il rilevamento misurato dal sonar, con il suo errore (se il contatto c'e' adesso)
                if (c.visibile()) tma_.aggiungi({m.tempo(), g->pos, normalizza_360(c.rilevamento)});
            } else {
                double z = 0.0;   // senza acustica: il vero con uno scarto gaussiano (somma di 12 uniformi)
                for (int k = 0; k < 12; ++k) z += rumore_() / 32767.0;
                tma_.aggiungi({m.tempo(), g->pos, normalizza_360(rilevamento(g->pos, it->second.pos) + (z - 6.0) * rumore_tma_)});
            }
        }
    }
    // Enhanced: rapporti sui siluri del giocatore dagli eventi del modello, e l'avviso del filo che sta per finire
    for (; eventi_letti_ < m.eventi_siluri().size(); ++eventi_letti_) {
        const auto& ev = m.eventi_siluri()[eventi_letti_];
        if (ev.lanciatore != g->id) continue;
        int codice = 0;
        switch (ev.evento.tipo) {
            case EventoSiluro::Tipo::attivazione: codice = MSG_TESTA_ATTIVA; break;
            case EventoSiluro::Tipo::aggancio: codice = MSG_SILURO_AGGANCIO; break;
            case EventoSiluro::Tipo::perdita: codice = MSG_SILURO_PERDITA; break;
            case EventoSiluro::Tipo::filo_rotto: codice = 23; break;   // «guidance cable cut»
            case EventoSiluro::Tipo::esaurito: codice = 24; break;     // fine corsa
            case EventoSiluro::Tipo::esplosione: codice = 19; break;   // esplosione subacquea
            default: break;
        }
        if (codice) messaggi_.push_back({ev.evento.tempo, g->id, codice});
    }
    for (const auto& [id, u] : m.unita()) {
        if (u.lanciatore != g->id || filo_avvisato_.count(id)) continue;
        if (const Siluro* s = m.siluro(id); s && s->filo() && s->filo_a_rischio()) {
            filo_avvisato_.insert(id);
            messaggi_.push_back({m.tempo(), g->id, 39});   // «il siluro sta per uscire dalla portata del filo»
        }
    }
    std::set<int> lanciate;
    for (const auto& [id, u] : m.unita())
        if (u.lanciatore == g->id && id != g->id && !m.da_mina(id)) lanciate.insert(id);   // non le armi delle mine
    armi_.unita_lanciate(lanciate);
    // il carico vivo del deposito (danni critici, fase 3; runtime): l'esplosivo che resta nei tubi e in rastrelliera
    if (danni_ && catalogo_) danni_->carico_vivo(g->id, esplosivo_a_bordo(armi_, *catalogo_, m.catalogo_missili(), m.limiti_carico().anno));
}

}  // namespace m688
