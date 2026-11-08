#include "nucleo/radio.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>

namespace m688 {

namespace {

constexpr double ORA = 3600.0, GIORNO = 24.0 * ORA, MEZZA_GIORNATA = 12.0 * ORA;
constexpr std::array<const char*, 12> MESI = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};

int ripetizioni(Precedenza p) {   // dossier §9.3: FLASH e IMMEDIATE 3 cicli, PRIORITY 2, ROUTINE 1
    switch (p) {
        case Precedenza::flash:
        case Precedenza::immediate: return 3;
        case Precedenza::priority: return 2;
        case Precedenza::routine: return 1;
    }
    return 1;
}

int giorni_del_mese(int mese, int anno) {
    static constexpr std::array<int, 12> G = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    const bool bisestile = (anno % 4 == 0 && anno % 100 != 0) || anno % 400 == 0;
    return mese == 2 && bisestile ? 29 : G[static_cast<std::size_t>(mese - 1)];
}

Ricezione migliore(Ricezione a, Ricezione b) { return static_cast<int>(a) >= static_cast<int>(b) ? a : b; }

struct Modello {
    const char* oggetto;
    const char* it;
    const char* en;
};

// traffico di fondo (STIMA): bollettini che danno vita alla sala radio e numeri di serie al broadcast
constexpr std::array<Modello, 5> FONDO = {{
    {"WX", "Bollettino meteorologico: mare forza 4, vento da nordovest a 20 nodi, visibilita' buona.",
     "WEATHER BULLETIN. SEA STATE 4. WIND NW 20 KTS. VISIBILITY GOOD."},
    {"INTSUM", "Sintesi d'intelligence: nessuna variazione significativa nell'attivita' navale avversaria nell'area.",
     "INTELLIGENCE SUMMARY. NO SIGNIFICANT CHANGE IN OPPOSING NAVAL ACTIVITY IN AREA."},
    {"ORDINI", "Nessuna variazione agli ordini in vigore.", "NO CHANGE TO ORDERS IN EFFECT."},
    {"NAVAREA", "Avviso ai naviganti: esercitazione di tiro nella zona occidentale, tenersi alla larga.",
     "NAVAREA WARNING. GUNNERY EXERCISE IN WESTERN ZONE. KEEP CLEAR."},
    {"PROVA", "Prova di ricezione del broadcast. Nessuna risposta richiesta.", "BROADCAST TEST. NO REPLY REQUIRED."},
}};

}  // namespace

const char* nome_banda(Banda b) {
    switch (b) {
        case Banda::elf: return "ELF";
        case Banda::vlf: return "VLF";
        case Banda::hf: return "HF";
        case Banda::uhf: return "UHF";
    }
    return "";
}

const char* nome_precedenza(Precedenza p) {
    switch (p) {
        case Precedenza::flash: return "FLASH";
        case Precedenza::immediate: return "IMMEDIATE";
        case Precedenza::priority: return "PRIORITY";
        case Precedenza::routine: return "ROUTINE";
    }
    return "";
}

char sigla_precedenza(Precedenza p) { return "ZOPR"[static_cast<int>(p)]; }

const char* nome_trasmissione(TipoTrasmissione t) {
    switch (t) {
        case TipoTrasmissione::contatto: return "contatto";
        case TipoTrasmissione::attacco: return "attacco";
        case TipoTrasmissione::situazione: return "situazione";
        case TipoTrasmissione::conferma: return "conferma";
        case TipoTrasmissione::richiesta: return "richiesta";
        case TipoTrasmissione::previsione: return "previsione";
    }
    return "";
}

SalaRadio::SalaRadio(LatoRadio lato, std::uint64_t seme, double ora_inizio_s, int giorno, int mese, int anno, ParametriRadio p)
    : lato_(lato), caso_(seme ^ 0x5AD10ull), ora_inizio_s_(ora_inizio_s), giorno_(giorno), mese_(mese), anno_(anno), p_(p),
      elf_(lato == LatoRadio::urss) {   // ZEVS sempre nella Flotta del Nord; Clam Lake pieno solo da fine 1989
    prossimo_fondo_ = caso_.uniforme() * p_.fondo_ogni_s;
}

std::string SalaRadio::dtg(double t) const {
    double s = ora_inizio_s_ + std::max(t, 0.0);
    int g = giorno_, m = mese_, a = anno_;
    while (s >= GIORNO) {
        s -= GIORNO;
        if (++g > giorni_del_mese(m, a)) { g = 1; if (++m > 12) { m = 1; ++a; } }
    }
    const int ore = static_cast<int>(s / ORA), minuti = static_cast<int>(std::fmod(s, ORA) / 60.0);
    char b[32];
    std::snprintf(b, sizeof b, "%02d%02d%02dZ %s %02d", g, ore, minuti, MESI[static_cast<std::size_t>(m - 1)], a % 100);
    return b;
}

void SalaRadio::ordini_di_missione(MessaggioRadio m) {
    m.serie = 0;
    if (m.dtg.empty()) m.dtg = dtg(0.0);
    ricevuti_.push_back({m, 0.0, Banda::vlf});
}

int SalaRadio::programma(MessaggioRadio m, double dal_s) {
    m.serie = ++serie_;
    if (m.da.empty()) m.da = lato_ == LatoRadio::usa ? "COMSUBLANT" : "SHTAB SF";
    if (m.autenticazione.empty()) {
        const char a = static_cast<char>('A' + static_cast<int>(caso_.uniforme() * 26.0));
        const char b = static_cast<char>('A' + static_cast<int>(caso_.uniforme() * 26.0));
        m.autenticazione = std::string{a, b};
    }
    InCoda q;
    q.rimaste = m.ripetizioni > 0 ? m.ripetizioni : ripetizioni(m.precedenza);
    q.dovuto = dal_s;
    q.m = std::move(m);
    coda_.push_back(std::move(q));
    return serie_;
}

bool SalaRadio::annulla(const std::string& id) {
    if (id.empty()) return false;
    bool fatto = false;
    for (std::size_t i = 0; i < coda_.size(); ++i) {
        InCoda& q = coda_[i];
        if (q.m.id != id || q.trasmesso >= 0.0 || static_cast<int>(i) == corrente_) continue;
        q.rimaste = 0;
        fatto = true;
    }
    return fatto;
}

bool SalaRadio::in_coda(const std::string& id) const {
    for (const auto& q : coda_)
        if (q.m.id == id && q.rimaste > 0 && !q.ricevuto) return true;
    return false;
}

double SalaRadio::durata_in_onda(const MessaggioRadio& m) const {
    const int c = m.caratteri > 0 ? m.caratteri
                                  : static_cast<int>(m.testo.en.size() + m.oggetto.size() + m.da.size() + m.a.size()) + 60;
    return p_.acquisizione_vlf_s + c / p_.caratteri_s_vlf;
}

void SalaRadio::evento(EventoRadio::Tipo tipo, double t, int serie, Precedenza p) { eventi_.push_back({tipo, t, serie, p}); }

Ricezione SalaRadio::ricezione(Banda b) const {
    const StatoAntenne& s = stato_;
    const Ricezione albero = s.albero && s.quota_m <= p_.quota_albero_m
                                 ? (s.velocita_kn <= p_.velocita_albero_kn ? Ricezione::si : Ricezione::meta)
                                 : Ricezione::no;
    switch (b) {
        case Banda::vlf: {
            Ricezione filo = Ricezione::no;
            if (filo_ == Filo::filato && !s.ghiaccio && s.quota_m <= p_.quota_filo_m && !(filo_cieco_fino_ > t_))
                filo = s.velocita_kn <= p_.filo_pieno_kn ? Ricezione::si : s.velocita_kn <= p_.filo_meta_kn ? Ricezione::meta : Ricezione::no;
            return migliore(albero, filo);
        }
        case Banda::hf: return albero;
        case Banda::uhf: return lato_ == LatoRadio::usa && std::fabs(s.latitudine) <= p_.latitudine_satellite ? albero : Ricezione::no;
        case Banda::elf:
            return elf_ && (filo_ == Filo::filato || albero != Ricezione::no) && s.quota_m <= p_.quota_elf_m &&
                           s.velocita_kn <= p_.velocita_elf_kn
                       ? Ricezione::si
                       : Ricezione::no;
    }
    return Ricezione::no;
}

void SalaRadio::cala_filo(double t) {
    if (filo_ != Filo::recuperato) return;
    filo_ = Filo::in_filata;
    filo_pronto_ = t + p_.filo_filata_s;
}

void SalaRadio::recupera_filo() {
    if (filo_ == Filo::filato || filo_ == Filo::in_filata) filo_ = Filo::recuperato;
}

void SalaRadio::ricevi(InCoda& q, double t, Banda b) {
    if (q.ricevuto) return;
    q.ricevuto = true;
    q.ricevuto_t = t;
    const bool tardi = q.m.scade_s >= 0.0 && t > q.dovuto + q.m.scade_s;   // il compito e' gia' scaduto
    ricevuti_.push_back({q.m, t, b, false, tardi});
    if (!tardi && q.m.autentico) nuovi_.push_back(q.m);   // un inganno non esegue il suo gestore
    evento(EventoRadio::Tipo::ricevuto, t, q.m.serie, q.m.precedenza);
    if (q.m.roe || q.m.zone) evento(EventoRadio::Tipo::ordini_cambiati, t, q.m.serie, q.m.precedenza);
    // contatto d'intelligence sulla carta, al posto di quelli piu' vecchi sulle stesse unita'
    if (q.m.intelligence) {
        const auto& u = q.m.intelligence->unita;
        std::erase_if(intelligence_, [&](const ContattoIntelligence& c) {
            return std::any_of(c.dati.unita.begin(), c.dati.unita.end(), [&](int x) { return std::find(u.begin(), u.end(), x) != u.end(); });
        });
        intelligence_.push_back({q.m.serie, *q.m.intelligence, t});
        evento(EventoRadio::Tipo::intelligence, t, q.m.serie, q.m.precedenza);
    }
    // buchi nella serie che ora si vedono: messaggi gia' finiti in onda senza essere ricevuti
    for (auto& x : coda_)
        if (!x.ricevuto && x.rimaste == 0 && x.m.serie < q.m.serie && !x.perso_avvisato) {
            x.perso_avvisato = true;
            evento(EventoRadio::Tipo::perso, t, x.m.serie, x.m.precedenza);
        }
}

// il traffico di fondo si programma in anticipo di un intervallo: la stazione vede sempre il prossimo broadcast
void SalaRadio::fondo(double t) {
    while (prossimo_fondo_ <= t + p_.fondo_ogni_s) {
        const Modello& f = FONDO[static_cast<std::size_t>(caso_.uniforme() * FONDO.size()) % FONDO.size()];
        MessaggioRadio m;
        m.oggetto = f.oggetto;
        m.testo = {f.it, f.en};
        m.classifica = "UNCLASSIFIED";
        programma(std::move(m), prossimo_fondo_);
        prossimo_fondo_ += p_.fondo_ogni_s * (0.5 + caso_.uniforme());
    }
}

void SalaRadio::prossimo_spazio(double t) {
    corrente_ = -1;
    for (std::size_t i = 0; i < coda_.size(); ++i) {
        const InCoda& q = coda_[i];
        if (q.rimaste <= 0 || q.dovuto > t) continue;
        if (corrente_ < 0) { corrente_ = static_cast<int>(i); continue; }
        const InCoda& c = coda_[static_cast<std::size_t>(corrente_)];
        if (q.m.precedenza < c.m.precedenza || (q.m.precedenza == c.m.precedenza && q.dovuto < c.dovuto)) corrente_ = static_cast<int>(i);
    }
    if (corrente_ < 0) return;
    InCoda& q = coda_[static_cast<std::size_t>(corrente_)];
    if (q.fatte == 0 && q.m.dtg.empty()) q.m.dtg = dtg(t);
    if (q.trasmesso < 0.0) {
        q.trasmesso = t;
        if (!q.m.annulla.empty()) {   // R3: il «disregard» toglie il messaggio annullato, se non e' ancora andato
            const std::string a = q.m.annulla;
            annulla(a);
        }
    }
    inizio_ = t;
    fine_ = t + durata_in_onda(q.m);
    interrotto_ = al_50_ = false;
    copiato_ = 0.0;
    if (!q.ricevuto && ricezione(Banda::vlf) != Ricezione::no) evento(EventoRadio::Tipo::in_arrivo, t, q.m.serie, q.m.precedenza);
}

void SalaRadio::avanza(double t, double dt, const StatoAntenne& s) {
    stato_ = s;
    t_ = t;
    // filo galleggiante: filata, cecita' dopo le virate, rottura oltre la velocita' di traino
    if (filo_ == Filo::in_filata && t >= filo_pronto_) filo_ = Filo::filato;
    if (filo_ == Filo::filato) {
        if (std::fabs(s.rateo_virata_deg_s) > p_.filo_virata_deg_s) filo_cieco_fino_ = t + p_.filo_cieco_dopo_virata_s;
        oltre_rottura_s_ = s.velocita_kn > p_.filo_rottura_kn ? oltre_rottura_s_ + dt : 0.0;
        if (oltre_rottura_s_ > 60.0 && caso_.uniforme() < 1.0 - std::pow(1.0 - p_.filo_rottura_al_minuto, dt / 60.0)) {
            filo_ = Filo::rotto;
            evento(EventoRadio::Tipo::filo_rotto, t);
        }
    }
    if (filo_ == Filo::filato) {
        const bool senza = ricezione(Banda::vlf) == Ricezione::no;
        if (senza && !senza_segnale_avvisato_) evento(EventoRadio::Tipo::filo_senza_segnale, t);
        senza_segnale_avvisato_ = senza;
    } else {
        senza_segnale_avvisato_ = false;
    }
    fondo(t);
    passo_sessioni(t);
    // compiti scaduti senza essere stati ricevuti in tempo
    for (auto& q : coda_)
        if (q.m.scade_s >= 0.0 && !q.scadenza_avvisata && t > q.dovuto + q.m.scade_s) {
            q.scadenza_avvisata = true;
            if (!q.ricevuto) evento(EventoRadio::Tipo::scaduto, t, q.m.serie, q.m.precedenza);
        }
    // richiesta al satellite: scarica cio' che e' gia' andato in onda (persi compresi), se l'albero resta su
    if (ssixs_fino_ >= 0.0) {
        if (ricezione(Banda::uhf) == Ricezione::no) {
            ssixs_fino_ = -1.0;
            evento(EventoRadio::Tipo::satellite_assente, t);
        } else if (t >= ssixs_fino_) {
            ssixs_fino_ = -1.0;
            for (auto& q : coda_) if (q.fatte > 0) ricevi(q, t, Banda::uhf);
            evento(EventoRadio::Tipo::ssixs, t);
        }
    }
    // spazio del broadcast: fine di quello in corso, poi il prossimo
    if (corrente_ >= 0 && t >= fine_) {
        InCoda& q = coda_[static_cast<std::size_t>(corrente_)];
        ++q.fatte;
        --q.rimaste;
        if (!q.ricevuto && !interrotto_ && (!al_50_ || caso_.uniforme() < 0.5))
            ricevi(q, fine_, ricezione(Banda::hf) != Ricezione::no ? Banda::hf : Banda::vlf);
        if (q.rimaste > 0) q.dovuto = inizio_ + p_.ciclo_s;
        corrente_ = -1;
    }
    if (corrente_ < 0) prossimo_spazio(t);
    if (corrente_ >= 0) {
        const Ricezione r = ricezione(Banda::vlf);
        if (r == Ricezione::no) interrotto_ = true;
        else { if (r == Ricezione::meta) al_50_ = true; copiato_ += dt; }
    }
    // la chiamata ELF dei FLASH appena programmati
    for (auto& q : coda_)
        if (q.m.precedenza == Precedenza::flash && !q.campanello && q.dovuto <= t && ricezione(Banda::elf) == Ricezione::si) {
            q.campanello = true;
            evento(EventoRadio::Tipo::campanello, t, q.m.serie, q.m.precedenza);
        }
}

double RegolaDF::probabilita(double durata_s) const { return p_rete * (1.0 - std::exp(-durata_s / tau_s)); }

RegolaDF regola_df(Banda banda, const ContestoDF& df) {
    RegolaDF r;
    if (banda == Banda::hf) {
        r.p_rete = df.rete_nemica ? 0.9 : 0.2;
        r.tau_s = 1.5;
        r.errore_m = 65000.0;
        r.ritardo_min_s = ORA;
        r.ritardo_extra_s = 2.0 * ORA;
    } else if (df.esm_vicino) {
        r.p_rete = 0.6;
        r.tau_s = 3.0;
        r.errore_m = 0.0;   // un solo sensore: rilevamento senza distanza
    } else {
        r.p_rete = 0.05;
        r.tau_s = 3.0;
        r.errore_m = 185000.0;
        r.ritardo_min_s = 3.0 * ORA;
        r.ritardo_extra_s = 3.0 * ORA;
    }
    return r;
}

EsitoTrasmissione SalaRadio::trasmetti(TipoTrasmissione tipo, double t, const StatoAntenne& s, const ContestoDF& df) {
    EsitoTrasmissione e;
    if (!s.albero || s.quota_m > p_.quota_albero_m) return e;
    static constexpr std::array<int, 6> CARATTERI = {300, 400, 1000, 100, 50, 150};
    const int c = CARATTERI[static_cast<std::size_t>(tipo)];
    const bool satellite = lato_ == LatoRadio::usa && std::fabs(s.latitudine) <= p_.latitudine_satellite;
    e.fatta = true;
    if (lato_ == LatoRadio::urss) {   // burst «Akula»: 0,72 s, ma Bullseye lo localizzava (dossier §6.4)
        e.banda = Banda::hf;
        e.durata_s = 0.72;
    } else if (satellite) {
        e.banda = Banda::uhf;
        e.durata_s = 1.0 + c / p_.caratteri_s_uhf;
    } else {
        e.banda = Banda::hf;
        e.durata_s = 60.0 + c / p_.caratteri_s_hf;
    }
    const RegolaDF df_regola = regola_df(e.banda, df);
    e.errore_m = df_regola.errore_m;
    e.ritardo_s = df_regola.ritardo_min_s + (df_regola.ritardo_extra_s > 0.0 ? df_regola.ritardo_extra_s * caso_.uniforme() : 0.0);
    for (auto& r : ricevuti_)   // R3: una trasmissione vale come ricevuta per i messaggi che la chiedevano
        if (r.messaggio.ricevuta && !r.ricevuta_inviata && r.tempo <= t) r.ricevuta_inviata = true;
    e.probabilita_fix = df_regola.probabilita(e.durata_s);
    e.localizzato = caso_.uniforme() < e.probabilita_fix;
    if (tipo == TipoTrasmissione::richiesta) {
        if (satellite) ssixs_fino_ = t + p_.acquisizione_uhf_s;
        else evento(EventoRadio::Tipo::satellite_assente, t);
    }
    evento(EventoRadio::Tipo::trasmissione, t);
    if (!sessioni_.empty() && sessioni_.back().stato == Sessione::Stato::in_corso) sessioni_.back().ricevuta_inviata = true;
    return e;
}

std::vector<int> SalaRadio::persi() const {
    int ultimo = 0;
    for (const auto& r : ricevuti_) ultimo = std::max(ultimo, r.messaggio.serie);
    std::vector<int> v;
    for (const auto& q : coda_)
        if (!q.ricevuto && q.rimaste == 0 && q.m.serie < ultimo) v.push_back(q.m.serie);
    std::sort(v.begin(), v.end());
    return v;
}

std::optional<MessaInOnda> SalaRadio::in_onda() const {
    if (corrente_ < 0) return std::nullopt;
    const InCoda& q = coda_[static_cast<std::size_t>(corrente_)];
    return MessaInOnda{q.m.serie, q.m.precedenza, inizio_, fine_, q.fatte + 1};
}

double SalaRadio::progresso() const {
    if (corrente_ < 0 || interrotto_ || fine_ <= inizio_) return 0.0;
    return std::clamp(copiato_ / (fine_ - inizio_), 0.0, 1.0);
}

TestoRadio SalaRadio::testo_in_arrivo() const {
    if (corrente_ < 0 || interrotto_) return {};
    const InCoda& q = coda_[static_cast<std::size_t>(corrente_)];
    if (q.ricevuto) return {};
    // l'acquisizione (sincronismo, cifra) non stampa niente; poi i caratteri a velocita' costante
    const double testo = std::max(0.0, copiato_ - p_.acquisizione_vlf_s) / std::max(fine_ - inizio_ - p_.acquisizione_vlf_s, 1.0);
    const auto parte = [&](const std::string& s) {
        std::size_t n = static_cast<std::size_t>(std::clamp(testo, 0.0, 1.0) * static_cast<double>(s.size()));
        while (n > 0 && n < s.size() && (static_cast<unsigned char>(s[n]) & 0xC0) == 0x80) --n;   // UTF-8 intero
        return s.substr(0, n);
    };
    return {parte(q.m.testo.it), parte(q.m.testo.en)};
}

std::vector<MessaInOnda> SalaRadio::in_programma() const {
    std::vector<MessaInOnda> v;
    for (std::size_t i = 0; i < coda_.size(); ++i) {
        const InCoda& q = coda_[i];
        if (q.rimaste <= 0 || static_cast<int>(i) == corrente_) continue;
        const double da = std::max(q.dovuto, corrente_ >= 0 ? fine_ : q.dovuto);
        v.push_back({q.m.serie, q.m.precedenza, da, da + durata_in_onda(q.m), q.fatte + 1});
    }
    std::sort(v.begin(), v.end(), [](const MessaInOnda& a, const MessaInOnda& b) {
        return a.inizio != b.inizio ? a.inizio < b.inizio : a.precedenza < b.precedenza;
    });
    return v;
}

std::vector<const Ricevuto*> SalaRadio::ordini() const {
    std::vector<const Ricevuto*> v;
    for (const auto& r : ricevuti_)
        if (r.messaggio.serie == 0 || !r.messaggio.ordine.empty() || r.messaggio.roe) v.push_back(&r);
    return v;
}

double SalaRadio::tolleranza_finestra_s() const {
    if (p_.tolleranza_finestra_s > 0.0) return p_.tolleranza_finestra_s;
    return lato_ == LatoRadio::usa ? 2.0 * ORA : 1800.0;
}

std::optional<MessaInOnda> SalaRadio::prossimo_broadcast() const {
    const auto v = in_programma();
    if (v.empty()) return std::nullopt;
    return v.front();
}

double SalaRadio::prossima_finestra(double t) const {
    if (p_.finestra_ogni_s > 0.0) {   // orologio di missione
        const double k = std::max(0.0, std::ceil((t - tolleranza_finestra_s() - p_.finestra_primo_s) / p_.finestra_ogni_s));
        return p_.finestra_primo_s + k * p_.finestra_ogni_s;
    }
    // USA alle 00Z e alle 12Z; URSS alle 00:00 e alle 12:00 di Mosca (21Z e 09Z)
    const double scarto = lato_ == LatoRadio::usa ? 0.0 : 9.0 * ORA;
    const double a = ora_inizio_s_ + t;
    const double k = std::ceil((a - tolleranza_finestra_s() - scarto) / MEZZA_GIORNATA);
    return scarto + k * MEZZA_GIORNATA - ora_inizio_s_;
}

std::vector<EventoRadio> SalaRadio::eventi() {
    std::vector<EventoRadio> v;
    v.swap(eventi_);
    return v;
}

std::vector<MessaggioRadio> SalaRadio::nuovi() {
    std::vector<MessaggioRadio> v;
    v.swap(nuovi_);
    return v;
}

// --- R1: regole d'ingaggio, intelligence, debriefing ---

ParametriRadio parametri_di_missione(double ciclo_s) {
    ParametriRadio p;
    p.ciclo_s = ciclo_s;
    p.fondo_ogni_s = ciclo_s / 3.0;
    // finestre: ogni due cicli dal primo ciclo, +-un terzo di ciclo (STIMA: in missioni di 15-70 minuti le finestre
    // del rapporto vero, ogni 6 cicli, non arriverebbero quasi mai)
    p.finestra_ogni_s = 2.0 * ciclo_s;
    p.finestra_primo_s = ciclo_s;
    p.tolleranza_finestra_s = ciclo_s / 3.0;
    return p;
}

const char* nome_categoria(CategoriaRoe c) {
    switch (c) {
        case CategoriaRoe::sottomarini: return "sottomarini";
        case CategoriaRoe::navi: return "navi";
        case CategoriaRoe::mercantili: return "mercantili";
        case CategoriaRoe::neutrali: return "neutrali";
        case CategoriaRoe::aerei: return "aerei";
        case CategoriaRoe::numero: break;
    }
    return "";
}

const char* nome_stato_roe(StatoRoe s) {
    switch (s) {
        case StatoRoe::libero: return "libero";
        case StatoRoe::stretto: return "stretto";
        case StatoRoe::trattenuto: return "trattenuto";
    }
    return "";
}

std::optional<CategoriaRoe> categoria_da_nome(const std::string& s) {
    for (std::size_t i = 0; i < CATEGORIE_ROE; ++i)
        if (s == nome_categoria(static_cast<CategoriaRoe>(i))) return static_cast<CategoriaRoe>(i);
    return std::nullopt;
}

std::optional<StatoRoe> stato_roe_da_nome(const std::string& s) {
    for (const StatoRoe x : {StatoRoe::libero, StatoRoe::stretto, StatoRoe::trattenuto})
        if (s == nome_stato_roe(x)) return x;
    return std::nullopt;
}

void Roe::applica(const CambioRoe& c, int da_serie, bool da_dubbio) {
    for (std::size_t i = 0; i < CATEGORIE_ROE; ++i)
        if (c[i]) { stato[i] = *c[i]; serie[i] = da_serie; dubbia[i] = da_dubbio; }
}

Vec2 ContattoIntelligence::centro(double t) const {
    const double d = dati.velocita_kn * dati.m_s_per_nodo * std::max(eta(t), 0.0);
    const double r = dati.rotta * 3.14159265358979323846 / 180.0;
    return {dati.pos.x + d * std::sin(r), dati.pos.y + d * std::cos(r)};
}

double ContattoIntelligence::semiasse_rotta(double t) const {
    return dati.errore_m + dati.velocita_err_kn * dati.m_s_per_nodo * std::max(eta(t), 0.0);
}

double ContattoIntelligence::semiasse_traverso(double t) const {
    // rotta del tutto ignota (incertezza di 180°, i rapporti della forza): l'ellisse cresce come un cerchio, con la
    // velocita' incerta
    if (dati.rotta_err_deg >= 180.0) return semiasse_rotta(t);
    const double v = dati.velocita_kn * dati.m_s_per_nodo;
    return dati.errore_m + v * std::sin(dati.rotta_err_deg * 3.14159265358979323846 / 180.0) * std::max(eta(t), 0.0);
}

void SalaRadio::roe_iniziali(const CambioRoe& c) { roe_iniziali_ = c; }

// un messaggio vale per il giocatore se l'ha ricevuto (con `solo_autentiche` solo se e' autentico; altrimenti anche
// falso, marcato dubbio) o, per il comando, se e' andato in onda entro t ed e' autentico (un inganno non e' del comando)
static bool vale(bool solo_ricevuti, double t, bool solo_autentiche, bool ricevuto, double trasmesso, bool autentico) {
    if (!solo_ricevuti) return autentico && trasmesso >= 0.0 && trasmesso <= t;
    return ricevuto && (!solo_autentiche || autentico);
}

Roe SalaRadio::roe_con(bool solo_ricevuti, double t, bool solo_autentiche) const {
    Roe r;
    r.applica(roe_iniziali_, 0);
    for (const auto& q : coda_)   // in ordine di serie
        if (q.m.roe && vale(solo_ricevuti, t, solo_autentiche, q.ricevuto, q.trasmesso, q.m.autentico))
            r.applica(*q.m.roe, q.m.serie, verificato_falso(q.m.serie));
    return r;
}

Roe SalaRadio::roe_trasmesse(double t) const { return roe_con(false, t); }
Roe SalaRadio::roe_ricevute(bool solo_autentiche) const { return roe_con(true, 0.0, solo_autentiche); }

// --- R2: zone, inganno, sessioni ---

const char* nome_tipo_zona(TipoZona t) {
    switch (t) {
        case TipoZona::azione: return "azione";
        case TipoZona::libera_asw: return "libera_asw";
        case TipoZona::protetta: return "protetta";
        case TipoZona::protetta_mobile: return "protetta_mobile";
    }
    return "";
}

std::optional<TipoZona> tipo_zona_da_nome(const std::string& s) {
    for (const TipoZona t : {TipoZona::azione, TipoZona::libera_asw, TipoZona::protetta, TipoZona::protetta_mobile})
        if (s == nome_tipo_zona(t)) return t;
    if (s == "haven_mobile") return TipoZona::protetta_mobile;   // nome del dossier 10 §4.3
    return std::nullopt;
}

std::vector<Vec2> poligono_zona(const Zona& z, std::optional<PosaUnita> posa) {
    if (z.tipo != TipoZona::protetta_mobile) return z.vertici;
    if (!posa) return {};
    // rettangolo orientato sulla rotta: avanti e dietro lungo la rotta, lato a dritta e a sinistra
    const double r = posa->rotta * 3.14159265358979323846 / 180.0;
    const Vec2 a{std::sin(r), std::cos(r)}, d{std::cos(r), -std::sin(r)};
    const auto p = [&](double lungo, double traverso) {
        return Vec2{posa->pos.x + a.x * lungo + d.x * traverso, posa->pos.y + a.y * lungo + d.y * traverso};
    };
    return {p(z.avanti_m, z.lato_m), p(z.avanti_m, -z.lato_m), p(-z.dietro_m, -z.lato_m), p(-z.dietro_m, z.lato_m)};
}

bool dentro_zona(const Zona& z, Vec2 p, std::optional<PosaUnita> posa) {
    if (z.centro && z.tipo != TipoZona::protetta_mobile) return distanza(p, *z.centro) <= z.raggio_m;
    const auto v = poligono_zona(z, posa);
    bool dentro = false;   // pari e dispari dei lati attraversati
    for (std::size_t i = 0, j = v.empty() ? 0 : v.size() - 1; i < v.size(); j = i++)
        if ((v[i].y > p.y) != (v[j].y > p.y) && p.x < (v[j].x - v[i].x) * (p.y - v[i].y) / (v[j].y - v[i].y) + v[i].x) dentro = !dentro;
    return dentro;
}

void SalaRadio::zone_iniziali(std::vector<Zona> z) { zone_iniziali_ = std::move(z); }

std::vector<Zona> SalaRadio::zone_con(bool solo_ricevuti, double t, bool solo_autentiche) const {
    std::vector<Zona> v = zone_iniziali_;
    for (const auto& q : coda_) {
        if (!q.m.zone || !vale(solo_ricevuti, t, solo_autentiche, q.ricevuto, q.trasmesso, q.m.autentico)) continue;
        for (const auto& id : q.m.zone->togli) std::erase_if(v, [&](const Zona& z) { return z.id == id; });
        for (Zona z : q.m.zone->aggiungi) {   // con lo stesso id: la sposta
            std::erase_if(v, [&](const Zona& x) { return x.id == z.id; });
            z.serie = q.m.serie;
            z.dubbia = verificato_falso(q.m.serie);
            v.push_back(std::move(z));
        }
    }
    return v;
}

std::vector<Zona> SalaRadio::zone_trasmesse(double t) const { return zone_con(false, t, true); }
std::vector<Zona> SalaRadio::zone_ricevute(bool solo_autentiche) const { return zone_con(true, 0.0, solo_autentiche); }

void SalaRadio::aggrava_ultimo_lancio(GiudizioRoe::Esito e, const std::string& zona) {
    if (lanci_.empty() || static_cast<int>(e) <= static_cast<int>(lanci_.back().esito)) return;
    lanci_.back().esito = e;
    lanci_.back().zona = zona;
}

const MessaggioRadio* SalaRadio::messaggio(int serie) const {
    for (const auto& q : coda_) if (q.m.serie == serie) return &q.m;
    return nullptr;
}

bool SalaRadio::verificato_falso(int serie) const {
    for (const auto& r : ricevuti_) if (r.messaggio.serie == serie) return r.verificato && !r.messaggio.autentico;
    return false;
}

bool SalaRadio::verifica(int serie) {
    for (auto& r : ricevuti_)
        if (r.messaggio.serie == serie) { r.verificato = true; return r.messaggio.autentico; }
    return true;
}

std::vector<Sessione> SalaRadio::sessioni(double t) const {
    std::vector<Sessione> v = sessioni_;
    if (p_.finestra_ogni_s > 0.0) {
        const double c = prossima_finestra(t), tol = tolleranza_finestra_s();
        if (v.empty() || v.back().inizio < c - tol - 1e-6) v.push_back({c - tol, c + tol, false, false, Sessione::Stato::futura});
    }
    return v;
}

void SalaRadio::passo_sessioni(double t) {
    if (p_.finestra_ogni_s <= 0.0) return;
    const double tol = tolleranza_finestra_s();
    // la finestra in corso nasce quando si entra nella sua tolleranza
    const double c = prossima_finestra(t);
    if (t >= c - tol && (sessioni_.empty() || sessioni_.back().inizio < c - tol - 1e-6))
        sessioni_.push_back({c - tol, c + tol, false, false, Sessione::Stato::in_corso});
    if (sessioni_.empty()) return;
    Sessione& s = sessioni_.back();
    if (s.stato != Sessione::Stato::in_corso) return;
    if (ricezione(Banda::vlf) != Ricezione::no) s.copiata = true;
    if (t < s.fine) return;
    const bool fatta = s.copiata && (!p_.ricevuta_richiesta || s.ricevuta_inviata);
    s.stato = fatta ? Sessione::Stato::fatta : Sessione::Stato::mancata;
    if (!p_.sessioni_obbligatorie) return;
    if (fatta) { sessioni_saltate_ = 0; return; }
    evento(EventoRadio::Tipo::sessione_mancata, t);
    if (++sessioni_saltate_ == p_.saltate_max) evento(EventoRadio::Tipo::guinzaglio, t);
}

double SalaRadio::prossimo_inizio_ciclo(double t) const {
    double prossimo = -1.0;
    for (const auto& m : in_programma())
        if (m.inizio >= t && (prossimo < 0.0 || m.inizio < prossimo)) prossimo = m.inizio;
    return prossimo;
}

GiudizioRoe SalaRadio::giudica_lancio(int bersaglio, CategoriaRoe c, bool identificato, double t,
                                      std::optional<CategoriaRoe> creduta, const std::string& classe_creduta) {
    const auto permesso = [&](StatoRoe s) { return s == StatoRoe::libero || (s == StatoRoe::stretto && identificato); };
    const Roe ricevute = roe_ricevute(true), ora = roe_trasmesse(t), un_ciclo_fa = roe_trasmesse(t - p_.ciclo_s);
    GiudizioRoe g;
    g.tempo = t;
    g.bersaglio = bersaglio;
    g.categoria = c;
    g.ricevute = ricevute[c];
    g.trasmesse = ora[c];
    g.serie = ora.serie[static_cast<std::size_t>(c)];
    g.categoria_creduta = creduta;
    g.classe_creduta = classe_creduta;
    // dossier 10 §4.2: le ricevute violate sono un'infrazione piena; le trasmesse da almeno un ciclo e non copiate
    // un'infrazione («dovevi copiare il broadcast»); quelle di meno di un ciclo solo una nota
    g.ordine_falso_creduto = permesso(roe_ricevute(false)[c]) && !permesso(g.ricevute);   // «ordine non autenticato, creduto»
    if (!permesso(g.ricevute)) g.esito = GiudizioRoe::Esito::infrazione;
    else if (!permesso(un_ciclo_fa[c])) g.esito = GiudizioRoe::Esito::infrazione_non_copiato;
    else if (!permesso(g.trasmesse)) g.esito = GiudizioRoe::Esito::nota;
    lanci_.push_back(g);
    return g;
}

Debriefing SalaRadio::debriefing() const {
    Debriefing d;
    for (const auto& q : coda_) {
        if (q.trasmesso < 0.0) continue;
        const Debriefing::Voce v{q.m, q.trasmesso, q.ricevuto ? q.ricevuto_t : -1.0};
        d.trasmessi.push_back(v);
        if (!q.ricevuto) d.persi.push_back(v);
    }
    d.lanci = lanci_;
    d.ingressi = ingressi_;
    for (const auto& r : ricevuti_)
        if (r.messaggio.ricevuta && !r.ricevuta_inviata) d.ricevute_mancate.push_back(r.messaggio.serie);
    return d;
}

std::vector<std::pair<int, RichiestaIntelligence>> SalaRadio::da_compilare(double t) const {
    std::vector<std::pair<int, RichiestaIntelligence>> v;
    for (const auto& q : coda_)
        if (q.m.richiesta && !q.m.intelligence && q.dovuto <= t && q.fatte == 0 && q.rimaste > 0) v.push_back({q.m.serie, *q.m.richiesta});
    return v;
}

void SalaRadio::compila(int serie, const DatiIntelligence& d) {
    for (auto& q : coda_)
        if (q.m.serie == serie) q.m.intelligence = d;
}

std::vector<ContattoIntelligence> SalaRadio::intelligence(double t) const {
    std::vector<ContattoIntelligence> v;
    for (const auto& c : intelligence_)
        if (t - c.ricevuto <= p_.vita_intelligence_cicli * p_.ciclo_s) v.push_back(c);
    return v;
}

}  // namespace m688
