#include "interfaccia.hpp"
#include "registro_tempi.hpp"

#include "missioni/danni_missione.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstring>

#include "carta.hpp"
#include "effetti_bordo.hpp"
#include "nucleo/tma_ia.hpp"
#include "stima_mate.hpp"
#include "vista_ghiaccio.hpp"
#include "grafica/pagina_squadre.hpp"
#include "grafica/palette.hpp"
#include "grafica/simboli_carta.hpp"
#include "missioni/asw_missione.hpp"
#include "missioni/contatti_giocatore.hpp"
#include "nucleo/strumenti_carta.hpp"
#include "missioni/danni_sottomarini.hpp"
#include "missioni/periscopi_missione.hpp"
#include "nucleo/dinamica_sottomarino.hpp"
#include "nucleo/siluro.hpp"
#include "nucleo/armi_giocatore.hpp"
#include "nucleo/sonar_giocatore.hpp"

namespace m688 {

namespace {
constexpr double STIMA_TMA_OGNI_S = 30.0;
constexpr int MSG_PUNTI_FINITI = 254, MSG_PUNTO_CANCELLATO = 255, MSG_NESSUN_PUNTO_CARTA = 256;
constexpr int MSG_SCEGLI_PUNTO = 257, MSG_PUNTO_IMPOSTATO = 258, MSG_PUNTO_E_PILOTA = 259;
constexpr int MSG_SCEGLI_BERSAGLIO = 260, MSG_SCELTA_ANNULLATA = 261, MSG_PRIMA_L_IMMAGINE = 262;
constexpr int MSG_SCEGLI_PUNTO_SILURO = 438, MSG_SILURO_DIRETTO = 439;   // DIRECT dalla carta (C4)
constexpr double CURSORE_PX_S = 60.0;   // pixel della tela al secondo; dopo mezzo secondo quattro volte tanto

double semicampo(const VistaPeriscopio& v) { return (v.alto_ingrandimento ? CAMPO_ALTO_GRADI : CAMPO_BASSO_GRADI) / 2.0; }

// la divisione piccola del reticolo all'ingrandimento scelto (telemetro a reticolo dei sovietici)
double passo_fine_reticolo(const PostoPeriscopi& pp) {
    const auto& v = pp.dati()->reticolo_fine_gradi;
    if (v.empty()) return 1.0;
    return v[static_cast<std::size_t>(std::clamp(pp.stato().ingrandimento, 0, static_cast<int>(v.size()) - 1))];
}

// per Arma, anche le famiglie del carico dell'Enhanced (mina, attacco a terra): il pannello Armi e il suo ripetitore
constexpr const char* NOMI_ARMI[] = {"MK48", "TYPE53", "SEA LANCE", "TOMAHAWK", "HARPOON", "ESCA", "-", "MINA", "TLAM", "ESCA MOB."};
static_assert(std::size(NOMI_ARMI) == static_cast<std::size_t>(Arma::esca_mobile) + 1);

std::string maiuscole(std::string s) {
    for (auto& ch : s) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    return s;
}
// i numeri con la virgola, come le scritte delle stazioni
std::string decimale(double x, int cifre = 1) {
    char b[32];
    std::snprintf(b, sizeof b, "%.*f", cifre, x);
    std::string s = b;
    for (auto& ch : s) if (ch == '.') ch = ',';
    return s;
}
}  // namespace

std::string nome_periscopio_pulsante(const std::string& nome) {
    static const char* const CIRILLICO[32] = {"A", "B", "V", "G", "D", "E", "ZH", "Z", "I", "Y", "K", "L", "M", "N", "O", "P",
                                              "R", "S", "T", "U", "F", "KH", "TS", "CH", "SH", "SHCH", "", "Y", "'", "E", "YU", "YA"};
    const std::string s = nome.substr(0, nome.find(" ("));
    std::string r;
    for (std::size_t i = 0; i < s.size();) {   // А-Я (D0 90 - D0 AF), а-п (D0 B0 - D0 BF), р-я (D1 80 - D1 8F)
        const int b0 = static_cast<unsigned char>(s[i]), b1 = i + 1 < s.size() ? static_cast<unsigned char>(s[i + 1]) : 0;
        int k = -1;
        if (b0 == 0xD0 && b1 >= 0x90 && b1 <= 0xAF) k = b1 - 0x90;
        else if (b0 == 0xD0 && b1 >= 0xB0 && b1 <= 0xBF) k = b1 - 0xB0;
        else if (b0 == 0xD1 && b1 >= 0x80 && b1 <= 0x8F) k = b1 - 0x80 + 16;
        if (k >= 0) {
            r += CIRILLICO[k];
            i += 2;
        } else if (b0 == 0xC2 && (b1 == 0xAB || b1 == 0xBB)) {   // le virgolette basse di «Сигнал»: sul pulsante no
            i += 2;
        } else {
            r += s[i++];
        }
    }
    return maiuscole(std::move(r));
}

InterfacciaMissione::InterfacciaMissione(Missione& m, StazioniMissione& st, PostoDiComando& posto, RegistroOrdini& ordini,
                                         std::vector<Messaggio>& equipaggio, const std::map<std::string, ClasseNave>& classi,
                                         int stazione)
    : m_(m), st_(st), posto_(posto), ordini_(ordini), equipaggio_(equipaggio), classi_(classi), stazione_(stazione) {
    std::map<std::string, std::string> nomi;   // i nomi delle classi per la libreria del sonar
    for (const auto& [id, c] : classi_) nomi[id] = nome_classe(id);
    sonar_enh_.imposta_nomi(std::move(nomi));
    sonar_enh_.usa_scala_lunga(true);   // la stazione sonar occupa la vista (anche nella sala: la cornice usa il tema, 16-47)
    soluzione_.velocita_m_s = m_.metri_al_secondo(5.0);   // il MATE parte da 5 nodi di gioco (moto classico nelle originali)
}

void InterfacciaMissione::messaggio(int codice) {
    if (const auto* g = m_.giocatore()) equipaggio_.push_back({m_.tempo(), g->id, codice});
    if (EffettiDiBordo* e = effetti_di_bordo(); e && codice != MSG_ANALISI) e->messaggio_equipaggio(codice);   // roadmap 38
}

// Il registro dei messaggi (punto 21): gli ultimi in fondo, con l'ora di missione; quelli critici in giallo
void InterfacciaMissione::disegna_registro(Tela& t, const Font& f) const {
    const Rettangolo r{16, 24, LARGHEZZA_TELA - 32, ALTEZZA_VISTA - 40};
    t.rettangolo_pieno(r.x, r.y, r.w, r.h, colori::NERO);
    t.linea_orizzontale(r.x, r.x + r.w - 1, r.y, colori::GRIGIO);
    t.linea_orizzontale(r.x, r.x + r.w - 1, r.y + r.h - 1, colori::GRIGIO);
    t.linea_verticale(r.x, r.y, r.y + r.h - 1, colori::GRIGIO);
    t.linea_verticale(r.x + r.w - 1, r.y, r.y + r.h - 1, colori::GRIGIO);
    f.scrivi(t, r.x + 8, r.y + 6, "REGISTRO DEI MESSAGGI   (CTRL+L O ESC CHIUDE)", colori::CIANO_CHIARO, 1);
    const int righe = (r.h - 26) / 10;
    const int da = std::max(0, static_cast<int>(registro_.size()) - righe);
    for (int i = da; i < static_cast<int>(registro_.size()); ++i) {
        const auto& [tm, testo] = registro_[static_cast<std::size_t>(i)];
        const int s = static_cast<int>(tm);
        char ora[16];
        std::snprintf(ora, sizeof ora, "%02d:%02d:%02d", s / 3600, s / 60 % 60, s % 60);
        const int y = r.y + 22 + (i - da) * 10;
        f.scrivi(t, r.x + 8, y, ora, colori::GRIGIO, 1);
        std::string x = testo;
        while (!x.empty() && f.larghezza(x, 1) > r.w - 90) {   // tagliato al margine: il testo intero e' nella barra
            while (x.size() > 1 && (static_cast<unsigned char>(x.back()) & 0xC0) == 0x80) x.pop_back();
            x.pop_back();
        }
        f.scrivi(t, r.x + 74, y, x, x.find('!') != std::string::npos ? colori::GIALLO : colori::VERDE_CHIARO, 1);
    }
}

// «Sistemi di bordo operativi al N per cento»: nel classico l'integrita' dello scafo come l'originale, nell'Enhanced
// la media dell'efficienza dei tredici sistemi (un urto che lascia la schiera al 68% non da' piu' 100)
static int percento_sistemi(const StazioniMissione& st, const Missione& m) {
    const auto* g = m.giocatore();
    const auto* sistemi = st.danni() && g ? st.danni()->sistemi(g->id) : nullptr;
    if (!sistemi || sistemi->empty()) return st.percento_operativo(m);
    double somma = 0.0;
    for (const auto& x : *sistemi) somma += x.efficienza;
    return std::max(1, static_cast<int>(std::lround(somma * 100.0 / static_cast<double>(sistemi->size()))));
}

// L'antenna rimorchiata nella suite della classe del giocatore (Enhanced): un sensore passivo lineare
bool InterfacciaMissione::ha_schiera() const {
    const CatalogoAcustico* cat = m_.catalogo_acustico();
    const auto* g = m_.giocatore();
    if (!cat || !g) return true;
    for (const Sensore* x : cat->sensori_di(g->classe))
        if (x->modo == ModoSensore::passivo && x->antenna == Antenna::lineare) return true;
    return false;
}

bool InterfacciaMissione::silenzio() const { return st_.armi().silenzio(); }

DatiApp InterfacciaMissione::dati_app() const {
    DatiApp d;
    if (m_.con_acustica()) {
        d.modo_sonar = sonar_enh_.modo() == ModoSonar::anni_89 ? "1989" : "moderno";
        d.sensore = sonar_enh_.sensore();
        d.fascio = sonar_enh_.fascio();
    }
    d.filtro = st_.sonar().filtro();
    d.stazione = stazione_;
    d.silenzio = st_.armi().silenzio();
    d.bersaglio = st_.armi().bersaglio();
    d.soluzione = tma_vista_;   // la soluzione proposta dal giocatore allo strumento MATE
    d.proposta = soluzione_;
    return d;
}

std::vector<SegnoContatto> InterfacciaMissione::segni() const { return segni_contatti(m_, st_.sonar().filtro()); }

std::string InterfacciaMissione::nome_classe(const std::string& classe) const {
    const auto it = classi_.find(classe);
    std::string n = it != classi_.end() && !it->second.nome.empty() ? it->second.nome : classe;
    for (auto& ch : n) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    return n;
}

// Telegrafo con la schiera fuori o in manovra: la domanda prima dell'ordine (§10.4)
namespace {
// La quota a schermo. chiglia_vera resta ferma alla quota periscopica della classe per tutto il tratto della missione fra 6,1 m
// e quella quota (688: 18,3 m), e li' il periscopio e' ancora rifiutato: chi risaliva leggeva «18 M» per 12 m. Sullo schermo
// la quota scende senza scalini da RACCORDO_M in su; le collisioni tengono chiglia_vera
constexpr double RACCORDO_M = 60.0;
double quota_a_schermo(const Missione& m, UnitaClassica u) {
    const double q = u.quota, chiglia = m.chiglia_vera(u);
    if (q <= QUOTA_PERISCOPICA_MISSIONE_M || q >= RACCORDO_M) return chiglia;
    u.quota = QUOTA_PERISCOPICA_MISSIONE_M;
    const double periscopica = m.chiglia_vera(u);
    return periscopica + (q - QUOTA_PERISCOPICA_MISSIONE_M) * (RACCORDO_M - periscopica) / (RACCORDO_M - QUOTA_PERISCOPICA_MISSIONE_M);
}
constexpr double VELOCITA_PER_SALIRE_KN = 3.0;   // sotto, i piani non portano su: Ctrl+P ci mette troppo (misura del 06/11/2026)
constexpr int MSG_SERVE_VELOCITA = 469, MSG_SOLO_PERISCOPICA_COME = 500;
}  // namespace

void InterfacciaMissione::azione_posto(Azione a) {
    const auto* g = m_.giocatore();
    if (a == Azione::immersione_emergenza) {   // Enhanced (P4): avanti tutta e piani a scendere sotto le chiglie vicine
        PostoEnhanced* p = posto_enhanced();
        if (!p) return;   // nel classico l'ordine non c'e', come nell'originale
        ordina({m_.tempo(), TipoOrdine::immersione_emergenza, 0.0, 0.0, 0});
        p->imposta(PostoEnhanced::CAMPANE - 1, p->barra(), m_.quota_immersione_emergenza());   // il pannello Manovra
        messaggio(MSG_IMMERSIONE_EMERGENZA);
        return;
    }
    if (a == Azione::posati) {   // Enhanced: posarsi sul fondo (nel classico l'ordine non c'e', come nell'originale)
        const EsitoStazione e = st_.posarsi(m_, quota_collaudo_);
        for (const auto& o : e.alla_missione) ordina(o);
        if (e.messaggio) messaggio(e.messaggio);
        return;
    }
    // Enhanced con la dinamica: timone, telegrafo e quote vanno al posto dell'Enhanced (niente domanda della schiera,
    // che e' una regola del classico); il resto (pilota, periscopio, antenna, ping) resta quello del classico
    const PostoPeriscopi* pp = m_.con_periscopi() ? m_.posto_periscopi() : nullptr;
    const bool periscopio_su = pp ? pp->stato().comandato_su : (g->flag54 & 2) != 0;   // coi periscopi veri: l'ordine
    if (PostoEnhanced* p = posto_enhanced())
        if (const auto o = p->azione(a, {m_.tempo(), g->quota, periscopio_su, (g->flag54 & 4) != 0})) {
            if (const DinamicaSottomarino* d = m_.dinamica(g->id);
                a == Azione::quota_periscopica && d && d->velocita_kn() < VELOCITA_PER_SALIRE_KN && g->quota > PostoDiComando::QUOTA_PERISCOPICA + 0.5)
                messaggio(MSG_SERVE_VELOCITA);
            ordina(*o);
            return;
        }
    const int prima = posto_.telegrafo();
    const auto risposta = posto_.azione(a, {m_.tempo(), g->quota, periscopio_su, (g->flag54 & 4) != 0});
    if (risposta.ordine && st_.sonar().chiede_conferma(*risposta.ordine)) {
        telegrafo_sospeso_ = risposta.ordine;
        telegrafo_prima_ = prima;
        scelta_ = Scelta::conferma_taglio;
        messaggio(MSG_CONFERMA_TAGLIO);
        return;
    }
    if (risposta.ordine) ordina(*risposta.ordine);
    else if (risposta.rifiuto) messaggio(enhanced_ && risposta.rifiuto == MSG_SOLO_PERISCOPICA ? MSG_SOLO_PERISCOPICA_COME : risposta.rifiuto);
}

// L'Alfa nell'Enhanced (dossier 19 §6; ordini del disassemblatore): Boksit a tre modi per asse e il programma dai punti
// di rotta, Tan, gli sfoghi e la compensazione di ПОКС, i tre bersagli di Akkord. Tutto passa dagli ordini, quindi
// entra nello storico, nel replay e nella ripresa
void InterfacciaMissione::comando_alfa(const std::string& k) {
    if (!alfa() || !m_.con_dinamica()) return;
    const double t = m_.tempo();
    if (k == "boksit_rotta" || k == "boksit_quota") {   // manuale -> automatico -> programma
        const int asse = k == "boksit_quota" ? 1 : 0;
        const int modo = (static_cast<int>(st_.modo_boksit(asse)) + 1) % 3;
        ordina({t, TipoOrdine::modo_boksit, static_cast<double>(modo), 0.0, asse});
    } else if (k == "boksit_tappa") {   // il programma: i punti di rotta della carta, con quota e velocita' correnti
        const int n = std::min<int>(9, static_cast<int>(m_.punti_di_rotta().size()));
        for (int p = 1; p <= n; ++p) ordina({t, TipoOrdine::programma_tappa, -1.0, -1.0, p});
        if (n) ordina({t, TipoOrdine::modo_boksit, 2.0, 0.0, 0});
    } else if (k == "tan") {
        if (const DanniSottomarini* ds = st_.danni()) tan_ = ds->tan(m_.giocatore()->id);   // lo stato vero delle casse
        tan_ = !tan_;
        ordina({t, TipoOrdine::tan, 0.0, 0.0, tan_ ? 1 : 0});
    } else if (k == "sfoga") {
        ordina({t, TipoOrdine::sfoga, 0.0, 0.0, 0});
    } else if (k == "compensa_piu" || k == "compensa_meno") {   // mezzo metro cubo per pressione
        ordina({t, TipoOrdine::compensa, k == "compensa_piu" ? 0.5 : -0.5, 0.0, 0});
    } else if (k == "akkord_designa") {
        if (const int b = st_.armi().bersaglio(); b >= 0) ordina({t, TipoOrdine::akkord_designa, 0.0, 0.0, b});
    } else if (k == "akkord_1" || k == "akkord_2" || k == "akkord_3") {
        ordina({t, TipoOrdine::akkord_scegli, 0.0, 0.0, k.back() - '0'});
    }
}

// Pulsanti numerati della stazione: tubi alle Armi, FILTRO, impulso, schiera e ANALIZZA al Sonar
void InterfacciaMissione::cifra(int n) {
    const double t = m_.tempo();
    if (stazione_ == 5 && m_.con_periscopi()) {
        cifra_periscopi(n, t);
        return;
    }
    if (stazione_ == 7 && n >= 8 && m_.con_siluri_enhanced()) {   // 8 e 9: quota del siluro scelto via filo, -/+15 m
        const int s = st_.armi().arma_selezionata();
        if (const Siluro* sil = m_.siluro(s))
            ordina({t, TipoOrdine::comando_filo, std::max(0.0, sil->quota() + (n == 8 ? -15.0 : 15.0)),
                    static_cast<double>(ComandoFiloTipo::quota), s});
        return;
    }
    if (stazione_ == 2) {   // Radio: nel classico periscopio, antenna e rilettura; nell'Enhanced la sala radio
        const SalaRadio* r = m_.radio();
        if (n == 3) radio_.precedente();
        if (n == 4) radio_.successivo();
        if (!r) {
            if (n == 1) azione_posto(Azione::periscopio);
            if (n == 2) azione_posto(Azione::antenna);
            return;
        }
        if (n == 1) azione_posto(Azione::antenna);
        if (n == 2 && !r->filo_rotto()) ordina({t, TipoOrdine::radio_filo, 0, 0, r->filo() || r->filo_in_filata() ? 0 : 1});
        if (n == 5) radio_.commuta_ordini();
        if (n == 6) radio_.tipo_successivo();
        if (n == 7) ordina({t, TipoOrdine::radio_trasmetti, 0, 0, static_cast<int>(radio_.tipo())});
        if (n == 8) ordina({t, TipoOrdine::radio_ssixs});
        if (n == 9) ordina({t, TipoOrdine::radio_slot, radio_.ritardo_s()});   // boa SLOT col ritardo scelto
        return;
    }
    if (stazione_ == 7) {
        const EsitoStazione e = st_.armi().clic_tubo(n, t);
        for (const auto& o : e.alla_missione) ordina(o);
        if (e.messaggio) messaggio(e.messaggio);
    } else if (stazione_ == 6) {   // pulsanti del Plotting: ALL TRACKS, TARGET TRACK, PLOT PROJ
        if (n == 1) carta_.tutte_le_scie = !carta_.tutte_le_scie;
        if (n == 2) carta_.scia_bersaglio = !carta_.scia_bersaglio;
        if (n == 3) carta_.proiezione = !carta_.proiezione;
        if (carta_.restyle) {   // strumenti dell'Enhanced (N2): righello, linee di rilevamento, anelli, CPA
            auto& r = carta_.righello;
            if (n == 4) {
                r.aperto = !r.aperto;
                if (r.aperto) {   // A sul battello, B a 3 nm sulla prua
                    r.a_battello = true;
                    r.sposta_b = true;
                    r.a = m_.giocatore()->pos;
                    const Vec2 d = vettore(m_.giocatore()->rotta, 3.0 * MIGLIO);
                    r.b = {r.a.x + d.x, r.a.y + d.y};
                }
            }
            if (n == 5) carta_.linee = (carta_.linee + 1) % 3;
            if (n == 6) carta_.anelli = (carta_.anelli + 1) % 3;
            if (n == 8) carta_.cpa = !carta_.cpa;
            if (n == 7) {   // modo segno: il cursore parte dal battello
                carta_.segni.aperto = !carta_.segni.aperto;
                carta_.segni.cursore = m_.giocatore()->pos;
                if (carta_.segni.aperto) r.aperto = carta_.ekelund.aperto = false;
            }
            if (n == 9) carta_.tempo_rilevamento = !carta_.tempo_rilevamento;
        }
    } else if (stazione_ == 8) {
        const StazioneSonar& s = st_.sonar();
        if (m_.con_acustica()) {   // Enhanced: sensore, fascio e analizzatore della stazione completa
            if (n == 1) sonar_enh_.commuta_filtro();
            if (n == 4) {
                messaggio(sonar_enh_.analizza(m_));
                classifica_dai_tracker(t);
                return;
            }
            if (n == 5) { sonar_enh_.sensore_successivo(); return; }
            if (n == 6) { sonar_enh_.fascio_successivo(); return; }
            if (n == 7) { sonar_enh_.commuta_integrazione(); return; }
            if (n == 8) { sonar_enh_.automatico(); return; }
            if (n == 9) { sonar_enh_.cursore_successivo(); return; }
            if (n == 3 && !ha_schiera()) return;   // l'Alfa e i diesel non hanno l'antenna rimorchiata
        }
        switch (n) {
            case 1: ordina({t, TipoOrdine::filtro_sonar, 0, 0, s.filtro() ? 0 : 1}); break;
            case 2:
                ordina({t, TipoOrdine::impulso_sonar});
                if (mixer_) bordo_.ping_proprio(225.0, 3500.0, 0.5);   // il BQQ-5 attivo, sentito nei locali prodieri
                if (!m_.con_acustica()) suoni_classici_.ping_proprio();   // il classico: bip e sonar1 a 5500 Hz
                break;
            case 3: {   // il riquadro mostra l'azione possibile: FILA con la schiera dentro, RECUPERA con la schiera fuori
                const bool recupera = s.manovra() == ManovraSchiera::recuperando ||
                                      (s.manovra() == ManovraSchiera::nessuna && s.schiera() == StatoSchiera::filata);
                ordina({t, recupera ? TipoOrdine::schiera_recupera : TipoOrdine::schiera_cala});
                break;
            }
            case 4: {
                const int r = st_.analizza(m_);
                messaggio(r);
                if (r == MSG_ANALISI && !m_.con_acustica())   // il classico: la firma della categoria del bersaglio
                    if (const auto it = m_.unita().find(st_.armi().bersaglio()); it != m_.unita().end())
                        suoni_classici_.analizza(ContattoAnalizzato::da_flag(it->second.tipo_originale, it->second.flag));
                break;
            }
            default: break;
        }
    }
}

// TARGET (802A, numeri_eseguibile.md §6): ferma il tempo e fa scegliere fra i segni dell'ultima vista. Una lettera
// prende l'ultima voce con quella lettera; Tab (Maiuscolo+Tab all'indietro) scorre tutte le zone, X comprese (le X non
// si digitano); Invio conferma
void InterfacciaMissione::inizia_bersaglio(const VistaPeriscopio& v) {
    candidati_.clear();
    posizioni_.clear();
    scelto_ = -1;
    zona_ = -1;
    const auto lista = segni();
    auto zona = [&](const SegnoContatto& s, Vec2 q) {
        candidati_.emplace_back(s.traccia() && s.lettera ? s.lettera : '\0', s.id);
        posizioni_.push_back(q);
    };
    if (stazione_ == 6) {
        for (const auto& [s, q] : segni_su_carta(vista_carta(m_, carta_), m_, lista)) zona(s, q);
    } else {
        for (const auto& s : lista) {
            const double d = normalizza_180(s.rilevamento - v.rilevamento);
            if (std::fabs(d) <= semicampo(v))
                zona(s, {LARGHEZZA_TELA / 2.0 + d / (2.0 * semicampo(v)) * LARGHEZZA_TELA, 44.0});
        }
    }
    scelta_ = Scelta::bersaglio;
    messaggio(MSG_SCEGLI_BERSAGLIO);
}

// L'altezza d'albero sul quadrante del telemetro per il bersaglio (docs/25 §3.3): quella che il giocatore ha impostato,
// dal libretto con IDENTIFICA o a mano; altrimenti, solo nella difficolta' facile, «il timoniere legge la tavola» della
// classe vera se il contatto e' noto da 1400 s; altrimenti 30 m. Sempre entro i limiti del periscopio
InterfacciaMissione::Quadrante InterfacciaMissione::quadrante(int b) const {
    Quadrante q;
    if (const auto iq = quadrante_.find(b); iq != quadrante_.end()) q = iq->second;
    else if (m_.difficolta_asw() == DifficoltaAsw::facile)
        if (const auto* g = m_.giocatore())
            if (const auto c = g->contatti.find(b); c != g->contatti.end() && c->second.durata >= 1400.0)
                if (const auto u = m_.unita().find(b); u != m_.unita().end()) {
                    if (const libretto::Voce* v = voce_libretto(u->second.classe); v && v->albero_m > 1.0) q = quadrante_tavola(*v);
                    else if (const auto ia = alberi_.find(u->second.classe); ia != alberi_.end() && ia->second > 1.0) q = {ia->second, true};
                }
    if (const PostoPeriscopi* pp = m_.con_periscopi() ? m_.posto_periscopi() : nullptr; pp && pp->dati()->telemetro_altezza_max_m > 0.0)
        q.altezza_m = std::clamp(q.altezza_m, pp->dati()->telemetro_altezza_min_m, pp->dati()->telemetro_altezza_max_m);
    return q;
}

// Stadimetro (S2): al periscopio alzato, il bersaglio di superficie nel campo; la distanza dall'angolo fra la testa
// d'albero e il galleggiamento, con l'altezza vera se la classe e' nota (durata >= 1400) e 30 m altrimenti, e un
// errore di lettura di circa il 3% a 6x e l'8% a 1,5x. Coi periscopi veri l'altezza e' quella del quadrante
void InterfacciaMissione::misura_stadimetro(const VistaPeriscopio& v) {
    constexpr int MSG_STADIMETRO = 267, MSG_STADIMETRO_NO = 268, MSG_STADIMETRO_ALLINEA = 395, MSG_RETICOLO_ALLINEA = 492;
    const auto* g = m_.giocatore();
    const int b = st_.armi().bersaglio();
    const auto it = m_.unita().find(b);
    // Enhanced coi periscopi veri e telemetro sdoppiato (Type 2 e Type 18): H apre l'immagine fantasma, il giocatore la
    // sposta con [ ] finche' la testa d'albero fantasma tocca il galleggiamento, H legge la distanza dall'angolo:
    // R = altezza / tan(sdoppiamento). La precisione e' quella dell'allineamento, senza estrazioni a caso
    // Coi periscopi sovietici (telemetro a reticolo, kit 31/10/2026) lo stesso giro: H mette il cursore sulla scala
    // verticale, [ ] lo spostano di un decimo di divisione (Maiuscolo una divisione) finche' sta sulla testa d'albero col
    // galleggiamento sulla linea di mezzo, H legge R = altezza / tan(angolo del cursore)
    if (const PostoPeriscopi* pp = m_.con_periscopi() ? m_.posto_periscopi() : nullptr;
        pp && (pp->dati()->telemetro == "sdoppiato" || pp->dati()->telemetro == "reticolo")) {
        const StatoPeriscopio& s = pp->stato();
        if (!s.su) { messaggio(MSG_STADIMETRO_NO); return; }
        const Quadrante q = quadrante(b);   // l'altezza impostata sul quadrante: dal libretto, a mano o presunta
        const double altezza = q.altezza_m;
        const bool nota = q.tavola;
        if (s.sdoppiamento_gradi <= 0.0) {   // si apre a 3000 m per l'altezza impostata
            stadimetro_.reset();
            const bool reticolo = pp->dati()->telemetro == "reticolo";
            double angolo = gradi(std::atan(altezza / 3000.0));
            if (reticolo) {   // sul reticolo: a decimi di divisione
                const double decimo = passo_fine_reticolo(*pp) / 10.0;
                angolo = std::max(decimo, std::round(angolo / decimo) * decimo);
            }
            m_.vista_periscopio(s.ingrandimento, s.modo, s.filtro, angolo, s.telecamera_verticale);
            messaggio(reticolo ? MSG_RETICOLO_ALLINEA : MSG_STADIMETRO_ALLINEA);
            return;
        }
        stadimetro_ = Stadimetro{b, altezza / std::tan(radianti(s.sdoppiamento_gradi)), altezza, nota, q.punto};
        // la lettura va al motore (registro, debriefing, obiettivi «telemetro»): la distanza vera la mette lui
        if (g && g->contatti.count(b)) ordina(ordine_misura_telemetro(m_.tempo(), b, stadimetro_->distanza_m, altezza));
        m_.vista_periscopio(s.ingrandimento, s.modo, s.filtro, 0.0, s.telecamera_verticale);
        messaggio(MSG_STADIMETRO);
        return;
    }
    stadimetro_.reset();
    if (!g || !(g->flag54 & 2) || it == m_.unita().end() || it->second.quota > 1.0) { messaggio(MSG_STADIMETRO_NO); return; }
    const auto& u = it->second;
    if (std::fabs(normalizza_180(rilevamento(g->pos, u.pos) - v.rilevamento)) > semicampo(v)) { messaggio(MSG_STADIMETRO_NO); return; }
    const auto ia = alberi_.find(u.classe);
    const double vera = ia != alberi_.end() && ia->second > 1.0 ? ia->second : 30.0;
    const auto c = g->contatti.find(b);
    const bool nota = c != g->contatti.end() && c->second.durata >= 1400.0;
    const double usata = nota ? vera : 30.0;
    const double sigma = v.alto_ingrandimento ? 0.03 : 0.08;
    const double errore = std::normal_distribution<double>(0.0, sigma)(lettura_);
    stadimetro_ = Stadimetro{b, distanza(g->pos, u.pos) * usata / vera * (1.0 + errore), usata, nota};
    messaggio(MSG_STADIMETRO);
}

void InterfacciaMissione::chiudi_bersaglio(bool annulla) {
    scelta_ = Scelta::nessuna;
    if (annulla) scelto_ = -1;
    if (scelto_ >= 0 || annulla) ordina({m_.tempo(), TipoOrdine::designa_bersaglio, 0, 0, scelto_});
    messaggio(scelto_ >= 0 ? MSG_BERSAGLIO_ACQUISITO : MSG_NESSUN_BERSAGLIO);
}

void InterfacciaMissione::imposta_comandi(const std::string& dati, const MappaTasti& tasti, bool suggerimento) {
    try {
        testo_comandi_ = std::make_unique<TestoComandi>(dati);
        font_comandi_ = Font::carica(dati + "/font/fosforo688_5x7.fnt");
        targa_comandi_ = Font::carica(dati + "/font/incisione688_7.fnt");
    } catch (const std::exception&) {
        testo_comandi_.reset();   // senza il file F11 non fa niente, e la partita va avanti
    }
    tasti_comandi_ = tasti;
    suggerimento_comandi_ = suggerimento;
}

bool InterfacciaMissione::comandi_schermata(const Comandi& c) {
    if (!testo_comandi_) return false;
    const bool premuto = std::find(c.nominati.begin(), c.nominati.end(), "aiuto_comandi") != c.nominati.end();
    if (!schermata_aperta_) {
        if (!premuto) return false;
        schermata_aperta_ = true;
        scheda_comandi_ = testo_comandi_->scheda_di(stazione_);
        pagina_comandi_ = 0;
        return true;
    }
    if (premuto || c.esc) {
        schermata_aperta_ = false;
        return true;
    }
    const int n = testo_comandi_->schede();
    const auto scheda = [&](int k) { scheda_comandi_ = (k % n + n) % n; pagina_comandi_ = 0; };
    if (c.stazione) scheda(testo_comandi_->scheda_di(c.stazione));
    if (c.tab) scheda(scheda_comandi_ + c.tab);
    if (c.passi_ruota) scheda(scheda_comandi_ + c.passi_ruota);
    if (c.passi_su_giu) pagina_comandi_ = std::clamp(pagina_comandi_ - c.passi_su_giu, 0, std::max(pagine_comandi_ - 1, 0));
    for (const auto& [x, y] : c.clic)
        for (const AreaComando& a : aree_comandi_) {
            if (x < a.r.x || y < a.r.y || x >= a.r.x + a.r.w || y >= a.r.y + a.r.h) continue;
            if (a.chiudi) schermata_aperta_ = false;
            if (a.scheda >= 0) scheda(a.scheda);
            if (a.pagina >= 0) pagina_comandi_ = a.pagina;
            break;
        }
    return true;
}

bool InterfacciaMissione::comandi(const Comandi& c, VistaPeriscopio& vista, double dt) {
    if (comandi_schermata(c)) return false;
    if (c.esci) return true;
    if (stazione_ == 6 && carta_.segni.scrivendo) {   // nota sulla carta: i caratteri vanno nella scritta del segno
        if (Segno* sg = carta_.lucido.ultimo_segno()) {
            for (const char ch : c.testo)   // maiuscole, come le scritte dell'originale; il resto dell'ASCII com'e'
                if (sg->testo.size() < static_cast<std::size_t>(LucidoCarta::MAX_TESTO) && static_cast<unsigned char>(ch) >= 32 &&
                    static_cast<unsigned char>(ch) < 127)
                    sg->testo += static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
            if (c.cancella && !sg->testo.empty()) sg->testo.pop_back();
        }
        if (c.invio || c.esc) carta_.segni.scrivendo = false;
        return false;
    }
    const double t = m_.tempo();
    switch (scelta_) {
        case Scelta::bersaglio:
            for (const char l : c.lettere)
                for (std::size_t i = 0; i < candidati_.size(); ++i)
                    if (candidati_[i].first == l) { scelto_ = candidati_[i].second; zona_ = static_cast<int>(i); }
            for (const auto& [x, y] : c.clic)   // mouse: il segno piu' vicino entro 10 pixel, e la scelta si chiude
                for (std::size_t i = 0; i < posizioni_.size(); ++i)
                    if (std::hypot(posizioni_[i].x - x, posizioni_[i].y - y) <= 10.0) {
                        zona_ = static_cast<int>(i);
                        scelto_ = candidati_[i].second;
                        chiudi_bersaglio(false);
                        return false;
                    }
            if (c.tab && !candidati_.empty()) {
                const int n = static_cast<int>(candidati_.size());
                zona_ = zona_ < 0 ? (c.tab > 0 ? 0 : n - 1) : ((zona_ + c.tab) % n + n) % n;
                scelto_ = candidati_[static_cast<std::size_t>(zona_)].second;
            }
            if (c.invio || c.esc || c.stazione) chiudi_bersaglio(c.esc);
            if (c.stazione) stazione_ = c.stazione;
            return false;
        case Scelta::direct:
        case Scelta::punto: {
            const int dx = c.ruota, dy = c.su_giu;
            tenuto_ = dx || dy ? tenuto_ + dt : 0.0;
            const double passo = CURSORE_PX_S * (tenuto_ > 0.5 ? 4.0 : 1.0) * dt * vista_carta(m_, carta_).metri_per_pixel;
            cursore_.x += dx * passo;
            cursore_.y += dy * passo;
            bool clic = false;
            for (const auto& [x, y] : c.clic)   // mouse: il punto e' dove si clicca sulla carta
                if (y < ALTEZZA_VISTA) {
                    cursore_ = vista_carta(m_, carta_).a_mondo({static_cast<double>(x), static_cast<double>(y)});
                    clic = true;
                }
            if ((c.invio || clic) && scelta_ == Scelta::direct) {   // DIRECT: il siluro filoguidato scelto verso il punto
                const int s = st_.armi().arma_selezionata();
                if (m_.con_siluri_enhanced()) {   // Enhanced: la rotta via filo verso il punto
                    if (const Siluro* sil = m_.siluro(s))
                        ordina({t, TipoOrdine::comando_filo, rilevamento(sil->pos(), cursore_), static_cast<double>(ComandoFiloTipo::rotta), s});
                } else {   // classico: guida_e2 col punto, come l'originale
                    ordina({t, TipoOrdine::guida_punto, cursore_.x, cursore_.y, s});
                }
                messaggio(MSG_SILURO_DIRETTO);
                scelta_ = Scelta::nessuna;
            } else if (c.invio || clic) {   // il primo punto accende anche il pilota automatico (2Ch)
                const bool primo = m_.punti_di_rotta().empty();
                ordina({t, TipoOrdine::punto_di_rotta, cursore_.x, cursore_.y});
                if (primo) ordina({t, TipoOrdine::pilota_automatico, 0, 0, 1});
                messaggio(primo ? MSG_PUNTO_E_PILOTA : MSG_PUNTO_IMPOSTATO);
                scelta_ = Scelta::nessuna;
            } else if (c.esc) {
                messaggio(MSG_SCELTA_ANNULLATA);
                scelta_ = Scelta::nessuna;
            }
            return false;
        }
        case Scelta::conferma_taglio:
            if (c.invio) {
                ordina(StazioneSonar::taglio(t));
                ordina(*telegrafo_sospeso_);
                scelta_ = Scelta::nessuna;
            } else if (c.esc) {
                posto_.imposta_telegrafo(telegrafo_prima_);
                scelta_ = Scelta::nessuna;
            }
            return false;
        case Scelta::nessuna: break;
    }
    if (registro_aperto_ && c.esc) {   // Esc chiude il registro dei messaggi, non il gioco
        registro_aperto_ = false;
        return false;
    }
    if (stazione_ == 6 && menu_punto_nave_ && (c.esc || c.stazione)) {   // Esc chiude il menu dei punti nave, non il gioco
        menu_punto_nave_ = false;
        if (c.esc) return false;
    }
    if (stazione_ == 8 && pagina_intercetti_ && c.esc) {   // Esc chiude la pagina dell'intercettazione, non il gioco
        pagina_intercetti_ = false;
        return false;
    }
    if (stazione_ == 6 && pagina_fondale_ && c.esc) {   // Esc chiude la pagina del fondale, non il gioco
        pagina_fondale_ = false;
        return false;
    }
    if (stazione_ == 5 && libretto_aperto_ && c.esc) {   // Esc chiude la scheda del filtro, poi il libretto, non il gioco
        if (scheda_filtro_) scheda_filtro_ = false;
        else libretto_aperto_ = false;
        return false;
    }
    if (stazione_ == 8 && sonar_enh_.pagina_analisi() && c.esc) {   // Esc chiude la pagina dell'analizzatore, non il gioco
        sonar_enh_.chiudi_pagina_analisi();
        return false;
    }
    if (stazione_ == 8 && sonar_enh_.libreria_aperta() && c.esc) {   // Esc chiude la libreria del sonar, non il gioco
        sonar_enh_.commuta_libreria();
        return false;
    }
    if (c.pausa) pausa_ = !pausa_;
    if (c.esc) {   // in pausa Esc la toglie invece di uscire
        if (!pausa_) return true;
        pausa_ = false;
        return false;
    }
    if (c.stazione) stazione_ = c.stazione;
    if (m_.con_acustica() && std::find(c.nominati.begin(), c.nominati.end(), "intercettazione") != c.nominati.end()) {
        if (stazione_ == 8 && pagina_intercetti_) pagina_intercetti_ = false;   // Ctrl+W: la pagina, da ogni stazione
        else stazione_ = 8, pagina_intercetti_ = true;
    }
    if (m_.giocatore() && m_.quadro_fondale(m_.giocatore()->id) &&
        std::find(c.nominati.begin(), c.nominati.end(), "quadro_fondale") != c.nominati.end()) {
        if (stazione_ == 6 && pagina_fondale_) pagina_fondale_ = false;   // Ctrl+H: la pagina del fondale, da ogni stazione
        else stazione_ = 6, pagina_fondale_ = true, tma_vista_ = false;
    }
    if (libretto_possibile() && std::find(c.nominati.begin(), c.nominati.end(), "libretto") != c.nominati.end()) {
        if (stazione_ == 5 && libretto_aperto_) libretto_aperto_ = false;   // Ctrl+Z: il libretto, da ogni stazione
        else stazione_ = 5, libretto_aperto_ = true;
    }
    if (stazione_ == 5 && libretto_aperto_ && libretto_possibile()) comandi_libretto(c, t);
    if (stazione_ == 3 && spaccato_ && c.tab) {   // quadro danni: Tab sceglie il compartimento ingrandito, poi l'automatico
        const int n = spaccato_->compartimenti();
        if (c.tab > 0) dettaglio_danni_ = dettaglio_danni_ + 1 >= n ? -1 : dettaglio_danni_ + 1;
        else dettaglio_danni_ = dettaglio_danni_ < 0 ? n - 1 : dettaglio_danni_ - 1;
    }
    if (stazione_ == 6 && carta_.restyle && !tma_vista_ && m_.giocatore()) {   // carta dell'Enhanced: strumenti (N2)
        const auto* g = m_.giocatore();
        auto& r = carta_.righello;
        Comandi resto = c;
        if (c.evento)   // dove il giocatore crede di essere (con la navigazione realistica, la stima)
            carta_.lucido.evento(Evento::Tipo::segno, m_.con_navigazione() ? m_.posizione_stimata(g->id) : g->pos, m_.tempo());
        if (c.pulisci) carta_.lucido.pulisci();
        resto.evento = resto.pulisci = resto.ekelund = false;
        const VistaCarta vc = vista_carta(m_, carta_);
        auto& ek = carta_.ekelund;
        auto& sm = carta_.segni;
        if (c.ekelund) {   // Q: il regolo sulle linee del bersaglio, o della fonte con l'ultima linea
            ek.aperto = !ek.aperto;
            if (ek.aperto && !carta_.lucido.linee().empty()) {
                ek.fonte = carta_.lucido.linee().back().fonte;
                if (ultimo_bersaglio_ && carta_.lucido.linee_di(ultimo_bersaglio_->fonte).size() >= 2) ek.fonte = ultimo_bersaglio_->fonte;
                const auto ultima = carta_.lucido.linee_di(ek.fonte);
                if (!ultima.empty()) ek.rotta = normalizza_360(ultima.back().rilevamento + 90.0);
            }
        }
        if (ek.aperto) {   // frecce: rotta e distanza; [ ] velocita'; Invio la soluzione al MATE
            ek.rotta = normalizza_360(ek.rotta + c.ruota * (c.veloce ? 60.0 : 15.0) * dt);
            ek.distanza_m = std::clamp(ek.distanza_m * (1.0 + c.su_giu * (c.veloce ? 1.0 : 0.3) * dt), 200.0, 100000.0);
            if (c.minuti) ek.velocita_kn = std::clamp(ek.velocita_kn + 0.5 * c.minuti, 0.0, 45.0);
            if (c.invio) soluzione_ = {ek.distanza_m, ek.rotta, m_.metri_al_secondo(ek.velocita_kn)};
            resto.ruota = resto.su_giu = resto.minuti = 0;
            resto.invio = false;
            return comandi_stazioni(resto, vista, dt);
        }
        if (sm.aperto) {   // modo segno: frecce o clic il cursore, Tab l'affiliazione, [ ] il tipo, Invio mette, K toglie
            if (sm.tipo == 5 && c.veloce && c.su_giu)
                sm.raggio_zona_m = std::clamp(sm.raggio_zona_m * (1.0 + c.su_giu * 0.8 * dt), 0.1 * MIGLIO, 30.0 * MIGLIO);
            else {
                const double passo = (c.veloce ? 160.0 : 40.0) * dt * vc.metri_per_pixel;
                sm.cursore.x += c.ruota * passo;
                sm.cursore.y += c.su_giu * passo;
            }
            for (const auto& [px, py] : c.clic)
                if (px >= vc.x && py >= vc.y && px < vc.x + vc.w && py < vc.y + vc.h) sm.cursore = vc.a_mondo({px + 0.5, py + 0.5});
            if (c.tab) sm.affiliazione = static_cast<Segno::Affiliazione>((static_cast<int>(sm.affiliazione) + c.tab + 4) % 4);
            if (c.minuti) sm.tipo = (sm.tipo + c.minuti + 6) % 6;
            if (c.invio) {
                if (sm.tipo == 5) carta_.lucido.aggiungi_zona({sm.cursore, sm.raggio_zona_m, true});
                else {
                    carta_.lucido.aggiungi_segno({sm.cursore, sm.affiliazione, static_cast<Segno::Dominio>(sm.tipo), {}, m_.tempo()});
                    sm.scrivendo = true;   // la scritta: la finestra passa i caratteri, Invio chiude
                }
            }
            if (c.cancella_punto) {
                if (sm.tipo == 5) carta_.lucido.togli_zona();
                else carta_.lucido.togli_segno();
            }
            resto.ruota = resto.su_giu = resto.minuti = resto.tab = 0;
            resto.invio = resto.cancella_punto = false;
            resto.clic.clear();
            return comandi_stazioni(resto, vista, dt);
        }
        if (r.aperto) {   // frecce sull'estremo attivo, Tab cambia estremo, F aggancia, Invio fissa il tratto, K lo toglie
            const VistaCarta v = vista_carta(m_, carta_);
            if (c.tab) r.sposta_b = !r.sposta_b;
            if (!r.sposta_b && r.a_battello && (c.ruota || c.su_giu || !c.clic.empty())) {
                r.a = g->pos;   // l'estremo A si stacca dal battello quando lo si muove
                r.a_battello = false;
            }
            Vec2& e = r.sposta_b ? r.b : r.a;
            const double passo = (c.veloce ? 160.0 : 40.0) * dt * v.metri_per_pixel;
            e.x += c.ruota * passo;
            e.y += c.su_giu * passo;
            for (const auto& [px, py] : c.clic)
                if (px >= v.x && py >= v.y && px < v.x + v.w && py < v.y + v.h) e = v.a_mondo({px + 0.5, py + 0.5});
            if (c.aggancia) {   // il battello o il contatto con la posizione piu' vicino all'estremo
                Vec2 meglio = g->pos;
                double dmin = distanza(e, g->pos);
                for (const auto& s : segni())
                    if (s.traccia() && distanza(e, s.pos_stimata) < dmin) {
                        dmin = distanza(e, s.pos_stimata);
                        meglio = s.pos_stimata;
                    }
                e = meglio;
                if (!r.sposta_b) r.a_battello = distanza(meglio, g->pos) < 1e-6;
            }
            if (c.invio) {
                const Vec2 a = r.a_battello ? g->pos : r.a;
                carta_.lucido.aggiungi_tratto({a, r.b, testo_misura(misura(a, r.b), 0.0, carta_.anni_89, carta_.scala())});
            }
            if (c.cancella_punto) carta_.lucido.togli_tratto();
            resto.tab = resto.ruota = resto.su_giu = 0;
            resto.invio = resto.cancella_punto = resto.aggancia = false;
            resto.clic.clear();
        }
        return comandi_stazioni(resto, vista, dt);
    }
    if (stazione_ == 8 && m_.con_acustica() && pagina_intercetti_) {   // la pagina dell'intercettazione (docs/22, E)
        if (const RicevitoreIntercettazione* r = m_.intercettatore_giocatore()) {
            if (c.su_giu && c.su_giu != prec_su_giu_int_) intercetti_.su_giu(*r, -c.su_giu);   // su: la traccia sopra
            for (const auto& k : c.nominati)
                if (k.rfind("intercetto_riga_", 0) == 0) {   // il clic su una riga sceglie la sua traccia
                    const auto& tr = r->tracce();
                    int da = 0;
                    for (std::size_t j = 0; j < tr.size(); ++j)
                        if (tr[j].id == intercetti_.traccia_scelta()) da = static_cast<int>(j);
                    intercetti_.su_giu(*r, std::atoi(k.c_str() + 16) - da);
                }
        }
        if (c.ruota && c.ruota != prec_ruota_int_) intercetti_.finestra(c.ruota);
        for (const auto& k : c.nominati) {
            if (k == "intercetto_finestra_piu") intercetti_.finestra(+1);
            if (k == "intercetto_finestra_meno") intercetti_.finestra(-1);
        }
        prec_su_giu_int_ = c.su_giu;
        prec_ruota_int_ = c.ruota;
        Comandi resto = c;   // il resto dei comandi senza quelli presi dalla pagina
        resto.su_giu = resto.ruota = resto.tab = 0;
        resto.nominati.clear();
        resto.stazione = 0;
        return comandi_stazioni(resto, vista, dt);
    }
    if (stazione_ == 8 && m_.con_acustica()) {   // Sonar dell'Enhanced: presentazione, cursori, zoom, tracker, libreria
        if (c.sfondo) sonar_enh_.commuta_libreria();
        if (sonar_enh_.libreria_aperta()) {   // libreria: Tab la scheda, frecce la voce, Invio la sovrappone al LOFAR
            if (c.tab) sonar_enh_.scheda_successiva(c.tab);
            voce_sonar_ += c.su_giu * -8.0 * dt;
            const int passi = static_cast<int>(voce_sonar_);
            voce_sonar_ -= passi;
            if (passi) sonar_enh_.voce_successiva(passi);
            if (c.invio) sonar_enh_.sovrapponi();
            Comandi resto;   // con la libreria aperta passano solo cambio di stazione, cifre e compressione
            resto.cifre = c.cifre;
            resto.compressione = c.compressione;
            resto.istantanea = c.istantanea;
            resto.verita = c.verita;
            return comandi_stazioni(resto, vista, dt);
        }
        if (c.invio) sonar_enh_.togli_sovrapposizione();
        if (c.punto) sonar_enh_.annota();
        if (c.proiezione) sonar_enh_.togli_annotazione();
        if (c.siluro) sonar_enh_.salva_slot();
        if (c.taglia_filo) sonar_enh_.slot_successivo();
        if (c.cerca) sonar_enh_.confronto_successivo();
        if (c.tab) sonar_enh_.campo_successivo(c.tab);
        if (c.minuti) {
            if (sonar_enh_.cursore() == SonarEnhanced::Cursore::storia) sonar_enh_.storia_risoluzione(c.minuti);
            else sonar_enh_.cambia_valore(c.minuti);
        }
        if (c.ingrandisci) sonar_enh_.zoom(+1);
        if (c.riduci) sonar_enh_.zoom(-1);
        if (c.tma) sonar_enh_.nuovo_tracker();
        if (c.cancella_punto) sonar_enh_.togli_tracker();
        // S5d: F aggancia (o sgancia) il verniero alla riga del cursore, E registra l'ascolto del fascio in un WAV
        const bool demon = sonar_enh_.cursore() == SonarEnhanced::Cursore::demon;   // il modo DEMON del cursore (9)
        const bool righe_auto = sonar_enh_.cursore() == SonarEnhanced::Cursore::righe;   // il modo RIGHE (moderno)
        const bool storia = sonar_enh_.cursore() == SonarEnhanced::Cursore::storia;      // il modo STORIA
        if (c.aggancia && demon) {
            sonar_enh_.demon_aggancia();   // F: la serie trovata (moderno), poi la successiva
        } else if (c.aggancia && storia) {   // F: quando e' comparsa la riga del cursore (o la riga automatica scelta)
            sonar_enh_.storia_cerca();
        } else if (c.aggancia && righe_auto) {   // F: il verniero sulla riga automatica scelta
            messaggio(sonar_enh_.righe_aggancia() ? MSG_VERNIERO_AGGANCIATO : MSG_VERNIERO_CURSORE);
        } else if (c.aggancia) {
            const bool aveva = sonar_enh_.riga_misurata().has_value();
            sonar_enh_.aggancia_riga();
            messaggio(aveva ? MSG_VERNIERO_SGANCIATO : sonar_enh_.riga_misurata() ? MSG_VERNIERO_AGGANCIATO : MSG_VERNIERO_CURSORE);
        }
        if (c.evento) messaggio(sonar_enh_.registra_ascolto(m_).empty() ? MSG_ASCOLTO_FALLITO : MSG_ASCOLTO_REGISTRATO);
        if (c.ekelund) sonar_enh_.modo_cuffia_successivo();   // Q: il modo della cuffia
        if (storia) {   // su e giu' il cursore del tempo (Maiuscolo a dieci righe); [ e ] la risoluzione
            if (c.su_giu && c.su_giu != prec_su_giu_demon_) sonar_enh_.storia_cursore(-c.su_giu, c.veloce);
            prec_su_giu_demon_ = c.su_giu;
        } else if (righe_auto) {   // su e giu' scelgono la riga automatica, un passo per pressione
            if (c.su_giu && c.su_giu != prec_su_giu_demon_) sonar_enh_.righe_su_giu(-c.su_giu);
            prec_su_giu_demon_ = c.su_giu;
        } else if (demon) {   // i cursori armonici del DEMON: sinistra e destra l'asse (Maiuscolo a passo grosso), su e giu' le pale
            if (c.ruota && c.ruota != prec_ruota_demon_) sonar_enh_.demon_asse(c.ruota, !c.veloce);
            if (c.su_giu && c.su_giu != prec_su_giu_demon_) sonar_enh_.demon_pale(c.su_giu);
            prec_ruota_demon_ = c.ruota;
            prec_su_giu_demon_ = c.su_giu;
        } else {
            cursore_sonar_ += c.su_giu * -15.0 * dt;   // righe al secondo: su va verso il presente
            const int righe = static_cast<int>(cursore_sonar_);
            cursore_sonar_ -= righe;
            const double hz_s = (c.veloce ? 40.0 : 8.0) / sonar_enh_.zoom();
            if (c.ruota || righe) sonar_enh_.muovi_cursore(c.ruota * hz_s * dt, righe);
        }
        for (const int k : sonar_enh_.messaggi()) messaggio(k);
        Comandi resto = c;   // il resto dei comandi senza quelli presi dalla stazione
        resto.tab = resto.minuti = resto.ruota = resto.su_giu = 0;
        resto.ingrandisci = resto.riduci = resto.tma = resto.cancella_punto = resto.cambia_ingrandimento = false;
        resto.invio = resto.punto = resto.proiezione = resto.siluro = resto.taglia_filo = resto.cerca = resto.detona = resto.sfondo = false;
        resto.stazione = 0;
        return comandi_stazioni(resto, vista, dt);
    }
    if (stazione_ == 7) {   // la pagina di tiro dei missili (docs/21 §3.5): si apre da sola sul tubo col missile
        const int tubo = st_.armi().selezionato();
        if (tubo != tubo_missili_ || c.stazione == 7) {
            if (tubo != tubo_missili_) missili_.cambio_tubo();
            tubo_missili_ = tubo;
            pagina_missili_ = tubo_con_missile(tubo);
        }
        if (pagina_missili_ && !tubo_con_missile(tubo)) pagina_missili_ = false;   // il tubo ha cambiato arma
    }
    if (stazione_ == 7 && pagina_missili_ && scelta_ == Scelta::nessuna) {
        const int tubo = tubo_missili_;
        const auto manda = [&](const std::vector<Ordine>& v) { for (const auto& o : v) ordina(o); };
        if (c.su_giu && c.su_giu != prec_su_giu_missili_) missili_.su_giu(m_, tubo, -c.su_giu);   // su: la riga sopra
        if (c.ruota && c.ruota != prec_ruota_missili_) manda(missili_.cambia(m_, tubo, c.ruota, t));
        prec_su_giu_missili_ = c.su_giu;
        prec_ruota_missili_ = c.ruota;
        bool fuoco = c.lancia && !c.veloce;
        if (c.lancia && c.veloce) missili_.coperchio();   // Maiusc+L: il coperchio del FIRE
        for (const auto& k : c.nominati) {   // i clic sulle righe della consolle e sul FIRE
            if (k == "missili_fuoco") fuoco = true;
            else if (k.rfind("missili_riga_", 0) == 0) {
                const auto righe = missili_.righe(m_, tubo);
                const int i = std::atoi(k.c_str() + 13);
                const auto ora = missili_.riga(m_, tubo);
                int da = 0;
                for (std::size_t j = 0; j < righe.size(); ++j)
                    if (ora && righe[j] == *ora) da = static_cast<int>(j);
                if (i == da) manda(missili_.cambia(m_, tubo, +1, t));   // un clic sulla riga scelta cambia il valore
                else missili_.su_giu(m_, tubo, i - da);
            }
        }
        if (fuoco) {
            if (const auto rifiuto = missili_.rifiuto_lancio()) {
                testo_missili_ = *rifiuto;
                testo_missili_fino_ = t + 8.0;
            } else {
                ordina(st_.armi().lancio(t));
                missili_.lanciato();
            }
        }
        if (c.ingrandisci) missili_.zoom(+1);
        if (c.riduci) missili_.zoom(-1);
        if (c.cerca) missili_.centra();
        if (c.tab) pagina_missili_ = false;   // Tab: la pagina dei siluri
        Comandi resto = c;   // il resto dei comandi senza quelli presi dalla pagina
        resto.tab = resto.su_giu = resto.ruota = resto.minuti = 0;
        resto.lancia = resto.ingrandisci = resto.riduci = resto.cerca = resto.cambia_ingrandimento = false;
        resto.nominati.clear();
        return comandi_stazioni(resto, vista, dt);
    }
    return comandi_stazioni(c, vista, dt);
}

StileIntercettazione InterfacciaMissione::stile_intercetti() const {
    StileIntercettazione s;
    s.modo_1989 = sonar_enh_.modo() == ModoSonar::anni_89;
    s.fosforo = sonar_enh_.con_fosforo();
    s.scala = sonar_enh_.scala_lunga() ? cascate::SCALA_LUNGA : cascate::SCALA_CORTA;
    s.persistenza = sonar_enh_.persistenza();
    if (const auto* g = m_.giocatore()) s.rotta = g->rotta;
    return s;
}

// L'allarme siluro dell'intercettatore (docs/22, E): il suono non e' documentato, quindi e' nostro. Siluro in acqua:
// tre bip a 900 Hz; in acquisizione: due toni alternati 700/1100 Hz per due secondi
void InterfacciaMissione::suona_allarme(Allarme a) {
    if (!mixer_ || a == Allarme::nessuno) return;
    constexpr int FS = 22050;
    std::vector<float> v;
    const auto tono = [&](double hz, double s, double ampiezza) {
        const int n = static_cast<int>(s * FS);
        for (int i = 0; i < n; ++i) {
            const double inviluppo = std::min({1.0, i / (0.005 * FS), (n - i) / (0.005 * FS)});   // niente clic
            v.push_back(static_cast<float>(ampiezza * inviluppo * std::sin(2.0 * PI * hz * i / FS)));
        }
    };
    if (a == Allarme::siluro_in_acqua)
        for (int k = 0; k < 3; ++k) tono(900.0, 0.18, 0.5), tono(0.0, 0.12, 0.0);
    else
        for (int k = 0; k < 8; ++k) tono(k % 2 ? 1100.0 : 700.0, 0.125, 0.6);
    mixer_->aggiungi(Canale::interfaccia, v, FS);
}

ContestoMissili InterfacciaMissione::contesto_missili() const {
    ContestoMissili k;
    const StazioneArmi& a = st_.armi();
    k.tubo = tubo_missili_;
    for (int n = 1; n <= a.numero_tubi(); ++n)
        k.tubi.push_back({n, std::string(nome_arma(a.arma(n))), a.stato(n) == StatoTubo::armato});
    k.bersaglio = a.bersaglio();
    k.consolle = ContestoMissili::Consolle::mk117;   // il Flight I del 1989 (il CCS MK 1 coi pozzi: col carico, piu' avanti)
    k.modo_1989 = carta_.anni_89;
    if (m_.tempo() < testo_missili_fino_) k.messaggio = testo_missili_;
    else k.messaggio = messaggio_barra_;
    k.ora_s = m_.tempo();
    return k;
}

bool InterfacciaMissione::comandi_stazioni(const Comandi& c, VistaPeriscopio& vista, double dt) {
    const double t = m_.tempo();
    if (m_.con_periscopi()) periscopi_vista(c, vista, dt);
    if (c.tma && stazione_ == 6) tma_vista_ = !tma_vista_;
    if (tma_vista_ && stazione_ == 6) {   // MATE: frecce per rotta e distanza, Maiuscolo per la velocita'
        soluzione_.rotta = normalizza_360(soluzione_.rotta + c.ruota * 30.0 * dt);
        // la velocita' in nodi del giocatore: nelle originali il moto classico corre k volte il vero (metri_al_secondo),
        // come il regolo di Ekelund
        if (c.veloce) {
            const double nodo = m_.metri_al_secondo(1.0);
            soluzione_.velocita_m_s = m_.metri_al_secondo(std::clamp(soluzione_.velocita_m_s / nodo + c.su_giu * 4.0 * dt, 0.0, 45.0));
        }
        else soluzione_.distanza_m = std::clamp(soluzione_.distanza_m * (1.0 + c.su_giu * 0.5 * dt), 200.0, 150000.0);
        // Invio: la proposta dal CPA del verniero sul bersaglio (blocco 3 del sonar, L): al CPA il rilevamento e'
        // perpendicolare al moto relativo, e ne esce una soluzione intera da caricare nel MATE
        if (c.invio)
            if (const auto& rm = sonar_enh_.riga_misurata(); rm && rm->cpa.valida && rm->contatto == st_.armi().bersaglio())
                if (const auto p = soluzione_da_cpa(st_.tma(), rm->cpa)) soluzione_ = p->soluzione;
        // la rotta stimata per gli obiettivi (rotta_stimata con da_tma, Enhanced; app/stima_mate.hpp)
        if (m_.con_acustica() && m_.tempo() >= prossima_stima_tma_) {
            prossima_stima_tma_ = m_.tempo() + STIMA_TMA_OGNI_S;
            if (stima_mate_valida(st_.tma(), soluzione_, m_.tempo())) m_.stima_tma_giocatore(st_.bersaglio_tma(), soluzione_.rotta);
        }
    }
    // il contatto del bersaglio e' sparito: si deseleziona da solo (DS:0368 = -1)
    if (const int b = st_.armi().bersaglio();
        b >= 0 && b != deselezionato_ && barra_bersaglio(m_, b).titolo == BarraBersaglio::Titolo::nessuno) {
        if (carta_.restyle && ultimo_bersaglio_ && ultimo_bersaglio_->id == b) {   // contatto perso: il datum sulla carta
            carta_.lucido.aggiungi_datum({ultimo_bersaglio_->pos, ultimo_bersaglio_->tempo, m_.metri_al_secondo(30.0), ultimo_bersaglio_->fonte});
            carta_.lucido.evento(Evento::Tipo::datum, ultimo_bersaglio_->pos, ultimo_bersaglio_->tempo, ultimo_bersaglio_->fonte);
        }
        ordina({t, TipoOrdine::designa_bersaglio, 0, 0, -1});
        deselezionato_ = b;   // una volta sola, finche' l'ordine non arriva alla stazione
    }
    // coi periscopi veri gli ingrandimenti del modello, a giro; alla stazione Periscopio le frecce sono l'elevazione
    // (periscopi_vista), e l'ingrandimento resta su I e sul pulsante 3
    if (m_.con_periscopi() && stazione_ != 6 && ((c.cambia_ingrandimento && stazione_ != 5) || c.ingrandisci)) {
        cifra_periscopi(3, t);
    } else if (c.cambia_ingrandimento && !(tma_vista_ && stazione_ == 6)) {
        vista.alto_ingrandimento = !vista.alto_ingrandimento;
    }
    if ((c.ingrandisci || c.riduci) && !(m_.con_periscopi() && stazione_ != 6)) {
        if (stazione_ == 6) {
            if (const int r = c.ingrandisci ? carta_.ingrandisci() : carta_.riduci()) messaggio(r);
        } else if (c.ingrandisci) {
            vista.alto_ingrandimento = !vista.alto_ingrandimento;
        }
    }
    if (!m_.giocatore()) return false;
    for (const Azione a : c.azioni) {
        azione_posto(a);
        if (in_pausa()) return false;
    }
    for (const auto& k : c.nominati) {
        if (k == "registro_messaggi") {
            registro_aperto_ = !registro_aperto_;
        } else if (k == "guida_punto") {   // DIRECT dalla carta: il cursore parte dal siluro filoguidato scelto
            const int s = st_.armi().arma_selezionata();
            const auto u = m_.unita().find(s);
            if (s < 0 || u == m_.unita().end() || u->second.distrutta) messaggio(MSG_NESSUN_SILURO);
            else if (stazione_ != 6) messaggio(MSG_PRIMA_L_IMMAGINE);
            else {
                cursore_ = u->second.pos;
                tenuto_ = 0.0;
                scelta_ = Scelta::direct;
                messaggio(MSG_SCEGLI_PUNTO_SILURO);
            }
        } else if (k == "muto" || (k == "tan" && !m_.con_acustica())) {   // classico: Ctrl-Q muto, Ctrl-N senza campioni
            if (!m_.con_acustica()) {
                const ModoSuoniClassici voluto = k == "muto" ? ModoSuoniClassici::muti : ModoSuoniClassici::senza_campioni;
                suoni_classici_.modo(suoni_classici_.modo() == voluto ? ModoSuoniClassici::tutti : voluto);
                suoni_classici_.tasto_modo();
            }
        } else if (k == "punto_nave") {   // navigazione realistica: il menu dei punti nave, alla carta
            if (m_.con_navigazione()) {
                if (stazione_ != 6) messaggio(MSG_PRIMA_L_IMMAGINE);
                else menu_punto_nave_ = !menu_punto_nave_;
            }
        } else if (k == "radar_giocatore") {   // il radar del battello (runtime, docs/18 fase 2): emerso o con l'albero su
            if (m_.giocatore() && m_.con_acustica())
                ordina({m_.tempo(), TipoOrdine::radar, 0.0, 0.0, m_.radar_giocatore_acceso() ? 0 : 1});
        } else if (k == "bqs15") {   // il sonar per le mine del 688 (runtime, S06) o il BQS-8 dello Sturgeon: acceso o spento;
                                     // chi non ne ha uno se lo vede rifiutare dalla missione (Missione::bqs15)
            if (const auto* g = m_.giocatore(); g && m_.con_acustica())
                ordina({m_.tempo(), TipoOrdine::bqs15, 0.0, 0.0, m_.bqs15_acceso(g->id) ? 0 : 1});
        } else {
            comando_alfa(k);
        }
    }
    StazioneArmi& armi = st_.armi();
    for (const int tubo : c.tubi) {
        const EsitoStazione e = armi.clic_tubo(tubo, t);
        for (const auto& o : e.alla_missione) ordina(o);
        if (e.messaggio) messaggio(e.messaggio);
    }
    for (const int n : c.cifre)
        if (stazione_ == 6 && menu_punto_nave_) {   // la cifra sceglie il punto nave; non possibile: resta aperto
            const auto possibili = m_.punti_nave_possibili();
            if (n >= 1 && n <= static_cast<int>(possibili.size()) && possibili[static_cast<std::size_t>(n - 1)].second.possibile) {
                ordina({t, TipoOrdine::punto_nave, 0.0, 0.0, 0, possibili[static_cast<std::size_t>(n - 1)].first});
                menu_punto_nave_ = false;
            }
        } else if (stazione_ == 6 && pagina_fondale_) {   // la pagina del fondale: l'ecoscandaglio, le misure sulla carta
            const auto e = m_.ecoscandaglio(m_.giocatore()->id);
            if (n == 1) ordina({t, TipoOrdine::ecoscandaglio_ping});
            if (n == 2 && e) {   // il periodo successivo: a comando, 1, 2, 5, 10, 15, 30 minuti, di nuovo a comando
                static constexpr double PERIODI[] = {0.0, 1.0, 2.0, 5.0, 10.0, 15.0, 30.0};
                double prossimo = 0.0;
                for (const double p : PERIODI)
                    if (p > e->periodo_min + 0.01) { prossimo = p; break; }
                ordina({t, TipoOrdine::ecoscandaglio_periodo, prossimo});
            }
            if (n == 3 && e) ordina({t, TipoOrdine::ecoscandaglio_potenza, 0.0, 0.0, e->potenza == PotenzaEco::piena ? 1 : 0});
            if (n == 4) misure_fondale_ = !misure_fondale_;
        } else if (stazione_ == 3 && st_.danni() && n >= 1 && n <= 5) {
            pulsante_squadre(n, t);
        } else {
            cifra(n);
        }
    if (c.lancia) ordina(armi.lancio(t));
    if (c.silenzio && armi.ricarica_realistica()) ordina({t, TipoOrdine::silenzio_a_bordo, 0, 0, armi.silenzio() ? 0 : 1});
    if (c.siluro) {
        const int s = st_.prossimo_siluro(m_);
        if (s >= 0) ordina({t, TipoOrdine::seleziona_siluro, 0, 0, s});
        else messaggio(MSG_NESSUN_SILURO);
    }
    if (stazione_ == 8 && m_.con_acustica() && c.filo_rotta) sonar_enh_.sposta_fascio(c.filo_rotta);   // Z X: il fascio
    else if (stazione_ == 5 && libretto_aperto_ && c.filo_rotta) {   // Z X: l'AOB del libretto (comandi_libretto)
    } else if (m_.con_siluri_enhanced()) {   // Enhanced: comandi via filo al siluro scelto (niente detonazione)
        const int s = armi.arma_selezionata();
        if (c.cerca) ordina({t, TipoOrdine::comando_filo, 0, static_cast<double>(ComandoFiloTipo::abilita), s});
        if (c.detona) ordina({t, TipoOrdine::spegni_siluro, 0, 0, s});
        if (c.taglia_filo) ordina({t, TipoOrdine::taglia_filo, 0, 0, s});
        if (c.filo_rotta)
            if (const Siluro* sil = m_.siluro(s))
                ordina({t, TipoOrdine::comando_filo, normalizza_360(sil->rotta() + 10.0 * c.filo_rotta),
                        static_cast<double>(ComandoFiloTipo::rotta), s});
        // Ctrl+V: nel tubo scelto l'arma successiva fra quelle in rastrelliera (non le esche); la stazione decide se si
        // puo' (un tubo armato va prima disarmato) e lo scambio costa una ricarica intera
        if (stazione_ == 7 && c.variante_arma && !armi.varianti().empty()) {
            const int tubo = armi.selezionato();
            const auto& vv = armi.varianti();
            const auto n = static_cast<int>(vv.size());
            int da = armi.indice_variante(armi.variante(tubo));
            const CatalogoCarico* cat = catalogo_carico_ ? catalogo_carico_() : nullptr;
            for (int k = 1; k <= n; ++k) {
                const std::string& id = vv[static_cast<std::size_t>(((da < 0 ? -1 : da) + k + n) % n)];
                const SchedaArma* sa = cat ? cat->arma(id) : nullptr;
                const auto r = armi.rastrelliera().find(id);
                if ((sa && sa->tipo == TipoArmaCarico::esca) || r == armi.rastrelliera().end() || r->second <= 0) continue;
                if (id != armi.variante(tubo)) ordina(armi.scegli_arma(tubo, id, t));
                break;
            }
        }
        // impostazioni del tubo selezionato: Tab sceglie il campo, [ e ] cambiano il valore
        if (stazione_ == 7 && c.tab) campo_siluro_ = (campo_siluro_ + c.tab + 10) % 10;
        if (stazione_ == 7 && c.minuti) {
            const int tubo = armi.selezionato();
            const ImpostazioniSiluro& imp = armi.impostazioni(tubo);
            double v = 0.0;
            const int d = c.minuti;
            switch (static_cast<CampoSiluro>(campo_siluro_)) {
                case CampoSiluro::rotta: v = normalizza_360(imp.rotta + 10.0 * d); break;
                case CampoSiluro::modo: v = (static_cast<int>(imp.modo) + d + 3) % 3; break;
                case CampoSiluro::velocita: v = imp.velocita == VelocitaSiluro::alta ? 0 : 1; break;
                case CampoSiluro::rte: v = std::max(0.0, imp.rte_m + 500.0 * d); break;
                case CampoSiluro::quota_corsa: v = std::max(0.0, imp.quota_corsa_m + 15.0 * d); break;
                case CampoSiluro::quota_ricerca: v = std::max(0.0, imp.quota_ricerca_m + 15.0 * d); break;
                case CampoSiluro::soffitto: v = std::max(0.0, imp.soffitto_m + 5.0 * d); break;
                case CampoSiluro::pavimento: v = std::max(0.0, imp.pavimento_m + 50.0 * d); break;
                case CampoSiluro::schema: v = (static_cast<int>(imp.schema) + d + 3) % 3; break;
                case CampoSiluro::anti_circolare: v = imp.anti_circolare ? 0 : 1; break;
            }
            ordina({t, TipoOrdine::imposta_siluro, v, static_cast<double>(campo_siluro_), tubo});
        }
    } else {
        if (c.cerca) ordina({t, TipoOrdine::guida_cerca});
        if (c.detona) ordina({t, TipoOrdine::guida_detona});
    }
    // carta
    if (c.proiezione) carta_.proiezione = !carta_.proiezione;
    if (c.sfondo) {   // B: nel classico il riempimento uniforme; nell'Enhanced lo stile (VGA 1989, carta nautica, foglio)
        if (carta_.restyle) carta_.stile_successivo();
        else carta_.sfondo_uniforme = !carta_.sfondo_uniforme;
    }
    if (c.minuti && stazione_ == 2 && m_.radio()) radio_.cambia_ritardo(c.minuti);   // Radio: ritardo della boa SLOT
    if (c.invio && stazione_ == 2 && m_.radio())   // Radio: verifica dell'autenticazione del messaggio scelto
        if (const int serie = radio_.serie_scelta(*m_.radio()); serie > 0) ordina({t, TipoOrdine::radio_verifica, 0, 0, serie});
    // stadimetro sdoppiato aperto (periscopi veri): [ ] spostano l'immagine fantasma, del 2 per cento (Maiuscolo 10)
    const PostoPeriscopi* pps = m_.con_periscopi() ? m_.posto_periscopi() : nullptr;
    const bool sdoppiato = pps && pps->stato().sdoppiamento_gradi > 0.0;
    if (sdoppiato && c.minuti) {
        const StatoPeriscopio& s = pps->stato();
        const double massimo = std::max(pps->dati()->telemetro_angolo_max_gradi, 0.01);
        double nuovo;
        if (pps->dati()->telemetro == "reticolo") {   // il cursore del reticolo: un decimo di divisione (Maiuscolo una)
            const double fine = passo_fine_reticolo(*pps);
            nuovo = std::clamp(s.sdoppiamento_gradi + c.minuti * (c.veloce ? fine : fine / 10.0), fine / 10.0, massimo);
        } else {
            nuovo = std::clamp(s.sdoppiamento_gradi * (1.0 + c.minuti * (c.veloce ? 0.10 : 0.02)), 0.005, massimo);
        }
        m_.vista_periscopio(s.ingrandimento, s.modo, s.filtro, nuovo, s.telecamera_verticale);
    }
    const bool quadrante_a_mano = stazione_ == 5 && libretto_aperto_;   // [ ] l'altezza del quadrante (comandi_libretto)
    if (c.minuti > 0 && stazione_ != 7 && stazione_ != 2 && !sdoppiato && !quadrante_a_mano) carta_.minuti_piu();
    if (c.minuti < 0 && stazione_ != 7 && stazione_ != 2 && !sdoppiato && !quadrante_a_mano) carta_.minuti_meno();
    if (c.cancella_punto) {   // CLEAR: l'ultimo punto; senza punti il pilota si spegne
        if (m_.punti_di_rotta().empty()) messaggio(MSG_NESSUN_PUNTO_CARTA);
        else {
            ordina({t, TipoOrdine::cancella_punto});
            messaggio(MSG_PUNTO_CANCELLATO);
        }
    }
    if (c.punto) {   // SET: solo con la carta disegnata
        if (m_.punti_di_rotta().size() >= 9) messaggio(MSG_PUNTI_FINITI);
        else if (stazione_ != 6) messaggio(MSG_PRIMA_L_IMMAGINE);
        else {
            cursore_ = m_.punti_di_rotta().empty() ? m_.giocatore()->pos : m_.punti_di_rotta().back();
            tenuto_ = 0.0;
            scelta_ = Scelta::punto;
            messaggio(MSG_SCEGLI_PUNTO);
            return false;
        }
    }
    if (c.bersaglio) inizia_bersaglio(vista);
    if (c.stadimetro) misura_stadimetro(vista);
    if (c.enciclopedia && enhanced_) enciclopedia_ = true;
    return false;
}

void InterfacciaMissione::imposta_libretto(const std::string& dati, const std::map<std::string, Modello>* modelli,
                                           const std::array<double, 15>& rampa) {
    try {
        libretto_ = libretto::carica(dati + "/libretto.toml");
        if (alfa()) libretto::in_stile_sovietico(libretto_);   // l'album dei siluetti sovietico (kit, 31/10/2026)
        piccolo_ = Font::carica(dati + "/font/fosforo688_5x7.fnt");
    } catch (const std::exception& e) {
        std::fprintf(stderr, "libretto di riconoscimento non caricato: %s\n", e.what());
        libretto_.clear();
        return;
    }
    modelli_ = modelli;
    rampa_ = rampa;
}

// Le navi con l'albero oltre il quadrante (Sacramento, Nimitz, Kirov...): si mira al secondo riferimento della tavola,
// un punto piu' basso e noto (il tetto della plancia, i fumaioli), come si faceva coi periscopi veri
InterfacciaMissione::Quadrante InterfacciaMissione::quadrante_tavola(const libretto::Voce& v) const {
    const PostoPeriscopi* pp = m_.con_periscopi() ? m_.posto_periscopi() : nullptr;
    const double massimo = pp ? pp->dati()->telemetro_altezza_max_m : 0.0;
    if (massimo > 0.0 && v.albero_m > massimo && v.riferimento_m > 1.0) return {v.riferimento_m, true, v.riferimento_nome};
    return {v.albero_m, true};
}

void InterfacciaMissione::imposta_luci(const std::string& dati) {
    try {
        luci_ = luci::carica(dati + "/luci.toml");
    } catch (const std::exception& e) {
        std::fprintf(stderr, "luci di navigazione non caricate: %s\n", e.what());
        luci_.clear();
    }
    try {   // i fari della costa del teatro (C9 §2.15, kit): luci anche loro
        fari_ = carica_fari(dati + "/fari.toml", m_.teatro());
    } catch (const std::exception& e) {
        std::fprintf(stderr, "fari non caricati: %s\n", e.what());
        fari_.clear();
    }
}

const libretto::Voce* InterfacciaMissione::voce_libretto(const std::string& classe) const {
    for (const auto& v : libretto_)
        if (v.classe == classe) return &v;
    return nullptr;
}

std::vector<int> InterfacciaMissione::tavole_filtrate() const {
    std::vector<int> v = libretto::filtra(libretto_, filtro_);
    if (v.empty() && !filtro_.attivo())
        for (int i = 0; i < static_cast<int>(libretto_.size()); ++i) v.push_back(i);
    return v;
}

// I comandi col libretto aperto alla stazione Periscopio (docs/25 §3.1-3.4)
void InterfacciaMissione::comandi_libretto(const Comandi& c, double t) {
    if (c.aggancia) scheda_filtro_ = !scheda_filtro_;   // F: la scheda del filtro
    if (scheda_filtro_) {   // la scheda: frecce su e giu' la riga, sinistra e destra il valore, una volta per pressione
        const int righe = libretto::righe_filtro(libretto_);
        if (c.su_giu && c.su_giu != prec_su_giu_lib_) riga_filtro_ = std::clamp(riga_filtro_ - c.su_giu, 0, righe - 1);
        if (c.ruota && c.ruota != prec_ruota_lib_) filtro_ = libretto::scegli(libretto_, filtro_, riga_filtro_, c.ruota);
        const bool spazio = std::find(c.nominati.begin(), c.nominati.end(), "osserva_aob") != c.nominati.end();
        if (c.invio || spazio) filtro_ = libretto::scegli(libretto_, filtro_, riga_filtro_, +1);
        if (c.cancella) filtro_ = libretto::scegli(libretto_, filtro_, riga_filtro_, 0);
        prec_su_giu_lib_ = c.su_giu;
        prec_ruota_lib_ = c.ruota;
        return;
    }
    const std::vector<int> lista = tavole_filtrate();
    if (lista.empty()) return;
    auto pos = std::find(lista.begin(), lista.end(), tavola_);
    if (pos == lista.end()) tavola_ = lista.front(), pos = lista.begin();   // la tavola aperta non passa il filtro
    if (c.tab) {
        const int n = static_cast<int>(lista.size());
        tavola_ = lista[static_cast<std::size_t>(((pos - lista.begin() + c.tab) % n + n) % n)];
    }
    if (c.filo_rotta) {   // Z sinistra, X dritta, a passi di 10 gradi; 180 e' la poppa
        aob_ += 10 * c.filo_rotta;
        if (aob_ > 180) aob_ -= 360;
        if (aob_ <= -180) aob_ += 360;
    }
    const auto* g = m_.giocatore();
    const int b = st_.armi().bersaglio();
    if (!g || b < 0 || !g->contatti.count(b)) return;   // senza un bersaglio designato si sfoglia soltanto
    const libretto::Voce& v = libretto_[static_cast<std::size_t>(tavola_)];   // quella mostrata (tavola_ e' nella lista)
    if (c.invio) {   // IDENTIFICA: la classe della tavola al contatto, e la sua altezza d'albero al quadrante
        ordina(ordine_identifica_visiva(t, b, v.classe));
        quadrante_[b] = v.albero_m > 1.0 ? quadrante_tavola(v) : Quadrante{quadrante(b).altezza_m, false};
    }
    const PostoPeriscopi* pp = m_.posto_periscopi();
    if (c.minuti && pp->stato().sdoppiamento_gradi <= 0.0) {   // [ ] l'altezza a mano, 1 m (Maiuscolo 5): non e' piu' la tavola
        const DatiPeriscopio& d = *pp->dati();
        double h = std::round(quadrante(b).altezza_m) + c.minuti * (c.veloce ? 5.0 : 1.0);
        if (d.telemetro_altezza_max_m > 0.0) h = std::clamp(h, d.telemetro_altezza_min_m, d.telemetro_altezza_max_m);
        quadrante_[b] = {h, false};
    }
    // Spazio: l'osservazione completa alla TMA, a periscopio alzato; l'incertezza e' il passo, piu' larga a basso
    // ingrandimento
    if (std::find(c.nominati.begin(), c.nominati.end(), "osserva_aob") != c.nominati.end() && pp->stato().su)
        ordina({t, TipoOrdine::osserva_aob, static_cast<double>(aob_), pp->stato().ingrandimento > 0 ? 10.0 : 15.0, b});
}

namespace {
// la sagoma del libretto alla scala data, ridotta se e' piu' larga di `largo_max` (come strumenti/libretto_tavole)
Sprite sagoma_libretto(const Modello& m, double aob, double scala, int largo_max, const std::array<double, 15>& rampa, bool vela) {
    Sprite s = libretto::sagoma(m, aob, scala, rampa, vela);
    if (s.w > largo_max) s = libretto::sagoma(m, aob, scala * largo_max / s.w, rampa, vela);
    return s;
}
}  // namespace

// La pagina del libretto nel terzo destro della vista, e sotto la riga del quadrante e dell'AOB
void InterfacciaMissione::disegna_libretto(Tela& t, const Font& f) const {
    const Rettangolo r{422, 8, 210, 340};
    if (scheda_filtro_) {
        libretto::scheda_filtro(t, r, f, *piccolo_, libretto_, filtro_, riga_filtro_,
                                "FRECCE SCELGONO, INVIO O SPAZIO CAMBIA, BACKSPACE TUTTE, F O ESC CHIUDE");
        return;
    }
    const std::vector<int> lista = tavole_filtrate();
    if (lista.empty()) {   // il filtro non lascia tavole: la scheda lo dice
        libretto::scheda_filtro(t, r, f, *piccolo_, libretto_, filtro_, riga_filtro_, "NESSUNA TAVOLA: F RIAPRE IL FILTRO");
        return;
    }
    const auto pos = std::find(lista.begin(), lista.end(), tavola_);
    const int tavola = pos == lista.end() ? lista.front() : tavola_;
    const int numero = pos == lista.end() ? 1 : static_cast<int>(pos - lista.begin()) + 1;
    const libretto::Voce& v = libretto_[static_cast<std::size_t>(tavola)];
    auto is = sagome_libretto_.find(v.classe);
    if (is == sagome_libretto_.end()) {   // le scale delle tavole del kit: il profilo in 194 px, le viste piccole in 62
        SagomeTavola sg;
        if (const auto im = modelli_ ? modelli_->find(v.classe) : std::map<std::string, Modello>::const_iterator{};
            modelli_ && im != modelli_->end()) {
            const Modello& m = im->second;
            const double l = std::max<double>(m.lunghezza_m(), 1.0), h = std::max<double>(m.quota_max_m(), 1.0);
            const double sp = v.sottomarino ? 38.0 / h : std::min(194.0 / l, 38.0 / h);
            const double sv = std::min(sp, 28.0 / h);
            const double s3 = v.sottomarino ? sv * 0.8 : std::min(sv, 62.0 / (0.78 * l));
            sg = {sagoma_libretto(m, 90.0, sp, 194, rampa_, v.sottomarino), sagoma_libretto(m, 0.0, sv, 62, rampa_, v.sottomarino),
                  sagoma_libretto(m, 45.0, s3, 62, rampa_, v.sottomarino), sagoma_libretto(m, 135.0, s3, 62, rampa_, v.sottomarino)};
        }
        is = sagome_libretto_.emplace(v.classe, std::move(sg)).first;
    }
    const SagomeTavola& sg = is->second;
    const auto o = [](const Sprite& x) { return x.w > 0 ? &x : nullptr; };
    libretto::pagina(t, r, f, *piccolo_, v, {o(sg.profilo), o(sg.prua), o(sg.tre_quarti), o(sg.poppa)}, numero,
                     static_cast<int>(lista.size()), filtro_.attivo() ? &filtro_ : nullptr);
    // la riga del quadrante: l'AOB impostato e l'altezza sul telemetro per il bersaglio; IDENT. se la tavola aperta e'
    // la classe dichiarata per il bersaglio
    const int b = st_.armi().bersaglio();
    const auto* g = m_.giocatore();
    char riga[64];
    const int a = std::abs(aob_);
    const char* lato = a == 0 || a == 180 ? "" : aob_ > 0 ? " DR" : " SN";
    int n = std::snprintf(riga, sizeof riga, "AOB %d%s", a, lato);
    if (b >= 0) {
        const Quadrante q = quadrante(b);
        // il punto in breve, che la riga sta in 35 caratteri: «TETTO DELLA PLANCIA» PLANCIA, «PRIMO FUMAIOLO» FUMAIOLO
        std::string punto = q.punto.substr(0, 10);
        for (const char* k : {"PLANCIA", "ISOLA", "FUMAIOLO"})
            if (q.punto.find(k) != std::string::npos) punto = k;
        n += std::snprintf(riga + n, sizeof riga - static_cast<std::size_t>(n), "  %s %.0f M%s", punto.c_str(), q.altezza_m, q.tavola ? "" : " ?");
        if (const auto* iv = g ? m_.identificazione_visiva(g->id, b) : nullptr; iv && iv->classe == v.classe)
            std::snprintf(riga + n, sizeof riga - static_cast<std::size_t>(n), "  IDENT.");
    }
    t.rettangolo_pieno(r.x, r.y + r.h + 1, r.w, 10, colori::NERO);
    piccolo_->scrivi(t, r.x + 3, r.y + r.h + 2, riga, colori::BIANCO, 1);
}

void InterfacciaMissione::imposta_consolle(const std::string& dati) {
    const auto* g = m_.giocatore();
    if (!g) return;
    try {
        consolle_ = std::make_unique<FontConsolle>(FontConsolle::carica(dati, g->classe == "alfa" ? BattelloTema::alfa : BattelloTema::usa688));
        if (g->classe == "alfa") schermo_ripetitori_ = Font::carica(dati + "/font/fosforo688_5x7.fnt");   // i ripetitori (gruppo 5)
    } catch (const std::exception& e) {
        std::fprintf(stderr, "consolle non disponibili: %s\n", e.what());
        consolle_.reset();
    }
}

void InterfacciaMissione::pannello(StatoPannello& p) const {
    p.consolle = consolle_.get();
    if (const auto* g = m_.giocatore()) p.battello = g->classe == "alfa" ? BattelloTema::alfa : BattelloTema::usa688;
    // la vista ha pezzi di consolle: la centrale, la cornice del periscopio, il Sozh dell'Alfa attorno alla carta
    p.consolle_vista = consolle_ && (stazione_ == 9 || (stazione_ == 5 && m_.con_periscopi()) || (stazione_ == 6 && alfa()) ||
                                   (stazione_ == 8 && m_.con_acustica()));
    p.armi.reset();
    p.sonar.reset();
    p.navigazione.reset();
    p.stato.reset();
    p.centrale.reset();
    p.periscopi.reset();
    p.barra_titolo.clear();
    p.barra_dati.clear();
    const auto* g = m_.giocatore();
    if (!g) return;
    // barra del bersaglio (8213); con il filtro solo per le armi
    const int b = st_.armi().bersaglio();
    bool passa = true;
    if (st_.sonar().filtro())
        if (const auto it = m_.unita().find(b); it != m_.unita().end()) passa = (it->second.flag & 0x20) != 0;
    const BarraBersaglio bb = barra_bersaglio(m_, b);
    if (passa && bb.titolo != BarraBersaglio::Titolo::nessuno) {
        std::string titolo;
        switch (bb.titolo) {
            case BarraBersaglio::Titolo::sconosciuto: titolo = "CONTATTO SCONOSCIUTO"; break;
            case BarraBersaglio::Titolo::sconosciuto_superficie: titolo = "CONTATTO DI SUPERFICIE SCONOSCIUTO"; break;
            case BarraBersaglio::Titolo::sconosciuto_immersione: titolo = "CONTATTO IN IMMERSIONE SCONOSCIUTO"; break;
            case BarraBersaglio::Titolo::classe:   // il nome dell'originale per tipo, altrimenti quello della classe
                titolo = nome_tipo_originale(bb.tipo_originale).empty() ? nome_classe(bb.classe)
                                                                        : std::string(nome_tipo_originale(bb.tipo_originale));
                break;
            case BarraBersaglio::Titolo::nessuno: break;
        }
        p.barra_titolo = (bb.lettera ? std::string(1, bb.lettera) + "  " : std::string{}) + titolo;
        if (barra_.bersaglio != b) barra_ = {b};
        if (bb.dati) {   // velocita' +-v/16 e quota +-50 ft (non in superficie), estratti solo quando il valore cambia
            barra_.validi = true;
            barra_.distanza_decimi = bb.distanza_decimi;
            if (bb.velocita_kn != barra_.velocita_vera) {
                barra_.velocita_vera = bb.velocita_kn;
                barra_.velocita = bb.velocita_kn * (1.0 + std::uniform_real_distribution<double>(-1.0 / 16, 1.0 / 16)(rumore_));
            }
            if (bb.quota_ft != barra_.quota_vera) {
                barra_.quota_vera = bb.quota_ft;
                barra_.quota = bb.superficie ? 0.0 : std::max(0.0, bb.quota_ft + std::uniform_int_distribution<int>(-50, 49)(rumore_));
            }
        }
        char riga[160], dati[96] = "";
        if (barra_.validi)
            std::snprintf(dati, sizeof dati, "   DISTANZA %d,%d NM   VELOCITÀ %.0f NODI   QUOTA %.0f FT", barra_.distanza_decimi / 10,
                          barra_.distanza_decimi % 10, barra_.velocita, barra_.quota);
        std::snprintf(riga, sizeof riga, "RIL. %03.0f°   ROTTA %03.0f°%s", bb.rilevamento_relativo, bb.rotta, dati);
        p.barra_dati = riga;
        if (stadimetro_ && stadimetro_->bersaglio == b) {   // l'ultima misura dello stadimetro su questo bersaglio
            char st[96];
            std::snprintf(st, sizeof st, "   STADIMETRO %.1f NM (%s %s%.0f M)", stadimetro_->distanza_m / MIGLIO, stadimetro_->punto.c_str(),
                          stadimetro_->classe_nota ? "" : "PRESUNTO ", stadimetro_->altezza_m);
            p.barra_dati += st;
        }
        if (const auto* g = m_.giocatore(); g && enhanced_)   // la classe identificata a vista dal libretto (docs/25 §3.2)
            if (const auto* iv = m_.identificazione_visiva(g->id, b)) {
                const libretto::Voce* v = voce_libretto(iv->classe);
                p.barra_dati += "   A VISTA " + (v ? v->nome : nome_classe(iv->classe));
            }
        if (!bb.sensore.empty()) {   // Enhanced: chi lo sente e con che margine
            std::string sens = bb.sensore;
            for (auto& ch : sens) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
            char e[48];
            std::snprintf(e, sizeof e, "   %s %+.0f DB", sens.c_str(), bb.eccesso_db);
            p.barra_titolo += e;
        }
    }
    if (stazione_ == 7) {
        const auto& NOMI = NOMI_ARMI;
        const StazioneArmi& a = st_.armi();
        StatoArmi s;
        // la ricarica dura i secondi della classe, allungati dal silenzio: da li' quanto l'arma e' gia' scivolata nel tubo
        const ParametriRicarica pr = parametri_ricarica(g->classe);
        const double durata = pr.secondi * (a.silenzio() ? pr.fattore_silenzio : 1.0);
        for (int t = 1; t <= a.numero_tubi(); ++t) {
            StatoArmi::Tubo tb{NOMI[static_cast<int>(a.arma(t))], static_cast<int>(a.stato(t)), a.secondi_ricarica(t), a.in_coda(t), {}};
            // il nome della variante del carico (MK 48 ADCAP, MK 67 SLMM, ADC MK 2...) al posto della famiglia
            if (const CatalogoCarico* c = catalogo_carico_ ? catalogo_carico_() : nullptr)
                if (const SchedaArma* sa = c->arma(a.variante(t))) {
                    tb.arma = sa->nome + (sa->what_if ? " (WHAT-IF)" : "");   // la MOSS del modo moderno
                    for (auto& ch : tb.arma) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
                }
            SagomaInTubo& z = tb.sezione;
            z.tempo_s = m_.tempo();
            switch (a.stato(t)) {
                case StatoTubo::vuoto: z.stato = FaseTubo::vuoto; break;
                case StatoTubo::in_ricarica:
                    z.stato = FaseTubo::ricarica;
                    z.ricarica = tb.coda ? 0.0 : std::clamp(1.0 - tb.secondi / std::max(durata, 1.0), 0.0, 1.0);
                    tb.ricarica = z.ricarica;
                    break;
                case StatoTubo::carico: z.stato = FaseTubo::carico; break;
                case StatoTubo::in_armamento: z.stato = FaseTubo::armamento; z.allagato = true; break;
                case StatoTubo::armato: z.stato = FaseTubo::armato; z.allagato = z.porta_aperta = true; break;
            }
            if (z.stato != FaseTubo::vuoto) z.arma = sagoma_arma(a.variante(t), a.arma(t));
            s.tubi.push_back(std::move(tb));
        }
        s.ricarica = a.ricarica_realistica();
        s.silenzio = a.silenzio();
        if (alfa()) {   // Akkord: i tre bersagli designati con la lettera del contatto, lo slot scelto, l'avaria
            for (std::size_t k = 0; k < s.akkord.size() && k < a.bersagli_akkord().size(); ++k)
                if (const int id = a.bersagli_akkord()[k]; id >= 0) {
                    const auto ct = g->contatti.find(id);
                    s.akkord[k] = ct != g->contatti.end() && ct->second.lettera ? std::string(1, ct->second.lettera) : "X";
                }
            s.akkord_scelto = a.bersaglio_scelto();
            s.akkord_guasto = a.akkord_guasto();
        }
        s.selezionato = a.selezionato();
        for (const auto& [nome, n] : m_.dotazione())
            if (const Arma x = arma_da_nome(nome); x != Arma::nessuna) s.scorte.emplace_back(NOMI[static_cast<int>(x)], a.scorte(x));
        s.in_acqua = a.in_acqua();
        s.limite = a.limite_in_acqua();
        if (const auto it = m_.unita().find(a.arma_selezionata()); it != m_.unita().end()) {
            const auto c = g->contatti.find(it->first);
            char d[64];
            std::snprintf(d, sizeof d, "%c %03.0f° %s", c != g->contatti.end() && c->second.lettera ? c->second.lettera : '?',
                          normalizza_360(rilevamento(g->pos, it->second.pos)), it->second.collegata == g->id ? "FILO" : "LIBERO");
            s.siluro = d;
        }
        if (m_.con_siluri_enhanced()) {   // impostazioni del tubo selezionato, il campo scelto fra parentesi
            static constexpr const char* MODI[] = {"PASSIVO", "ATTIVO", "MISTO"};
            static constexpr const char* SCHEMI[] = {"SERPENTINA", "CIRCOLARE", "DRITTO"};
            const ImpostazioniSiluro& i = a.impostazioni(a.selezionato());
            char v[10][32];
            std::snprintf(v[0], 32, "ROTTA %03.0f", i.rotta);
            std::snprintf(v[1], 32, "%s", MODI[static_cast<int>(i.modo)]);
            std::snprintf(v[2], 32, "VEL. %s", i.velocita == VelocitaSiluro::alta ? "ALTA" : "BASSA");
            std::snprintf(v[3], 32, "RTE %.1f KM", i.rte_m / 1000.0);
            std::snprintf(v[4], 32, "CORSA %.0f M", i.quota_corsa_m);
            std::snprintf(v[5], 32, "RICERCA %.0f M", i.quota_ricerca_m);
            std::snprintf(v[6], 32, "SOFF. %.0f M", i.soffitto_m);
            std::snprintf(v[7], 32, "PAV. %.0f M", i.pavimento_m);
            std::snprintf(v[8], 32, "%s", SCHEMI[static_cast<int>(i.schema)]);
            std::snprintf(v[9], 32, "ACR %s", i.anti_circolare ? "SI" : "NO");
            std::string r = a.impostato(a.selezionato()) ? "" : "(DAL LANCIO) ";
            for (int k = 0; k < 10; ++k) r += (k == campo_siluro_ ? "[" : "") + std::string(v[k]) + (k == campo_siluro_ ? "] " : " ");
            s.impostazioni = r;
            if (const Siluro* sil = m_.siluro(a.arma_selezionata()))
                s.filo = std::string(nome_fase(sil->fase())) + (sil->filo() ? "  FILO " + std::to_string(static_cast<int>(sil->filo_siluro_frazione() * 100)) +
                                                                                    (sil->filo_a_rischio() ? " A RISCHIO" : "")
                                                                              : "  SENZA FILO");
        }
        p.armi = s;
    } else if (stazione_ == 8) {
        const StazioneSonar& so = st_.sonar();
        StatoSonar s;
        s.schiera = so.schiera() == StatoSchiera::persa || st_.fuori_uso(m_, SistemaClassico::schiera) ? "PERSA"
                    : so.schiera() == StatoSchiera::filata                                     ? "RECUPERA"
                                                                                                : "FILA";
        if (so.manovra() == ManovraSchiera::filando) { s.schiera = "RECUPERA"; s.manovra = "IN FILATURA"; }
        if (so.manovra() == ManovraSchiera::recuperando) { s.schiera = "FILA"; s.manovra = "IN RECUPERO"; }
        s.ascolto_schiera = (g->flag54 & 1) != 0;
        if (m_.con_acustica() && !ha_schiera()) { s.schiera = "SENZA SCHIERA"; s.manovra.clear(); }   // Enhanced: la suite non l'ha
        // Enhanced con la schiera del modello (docs/20 §7, D): filata, la quota vera del centro, o il fondo e i danni;
        // persa anche quando la perde il modello (fondo, reti, eccesso di velocita')
        if (m_.con_acustica() && ha_schiera())
            if (const auto sc = m_.stato_schiera(g->id)) {
                if (sc->persa) {
                    s.schiera = "PERSA";
                    s.manovra.clear();
                } else if (so.manovra() == ManovraSchiera::nessuna && so.schiera() == StatoSchiera::filata) {
                    s.manovra = sc->danneggiata ? std::string("DANNEGGIATA")
                                : sc->fondo     ? std::string("SUL FONDO")
                                                : std::to_string(static_cast<int>(std::lround(sc->quota_centro_m))) + " M";
                }
            }
        s.filtro = so.filtro();
        s.enhanced = m_.con_acustica();
        s.slot = sonar_enh_.slot_disponibili();
        p.sonar = s;
    } else if (stazione_ == 2) {
        StatoRadio s;
        s.periscopio = (g->flag54 & 2) != 0;
        s.antenna = (g->flag54 & 4) != 0;
        s.radar = m_.radar_giocatore_acceso();
        if (const SalaRadio* r = m_.radio()) {
            s.enhanced = true;
            s.filo = r->filo_rotto() ? "FILO ROTTO" : r->filo() ? "RECUPERA FILO" : r->filo_in_filata() ? "FILO IN FILATA" : "CALA FILO";
            std::string tipo = nome_trasmissione(radio_.tipo());
            for (auto& ch : tipo) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
            s.trasmissione = tipo;
            s.solo_ordini = radio_.solo_ordini();
            if (consolle_) {   // Fase 3: il pannello radio/ESM del kit
                s.filo_breve = r->filo_rotto() ? "ROTTO" : r->filo() ? "RECUP." : r->filo_in_filata() ? "FILATA" : "FILO";
                s.filo_filato = r->filo();
                s.ritardo_min = static_cast<int>(std::lround(radio_.ritardo_s() / 60.0));
                // la telescrivente: gli ultimi messaggi ricevuti (serie, precedenza, oggetto) e quello in onda
                static constexpr char PRECEDENZA[] = {'Z', 'O', 'P', 'R'};
                const auto& ricevuti = r->ricevuti();
                for (std::size_t i = ricevuti.size() > 5 ? ricevuti.size() - 5 : 0; i < ricevuti.size(); ++i) {
                    const MessaggioRadio& x = ricevuti[i].messaggio;
                    std::string oggetto = x.oggetto.substr(0, 26);
                    for (auto& ch : oggetto) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
                    char riga[64];
                    std::snprintf(riga, sizeof riga, "%03d %c %s", x.serie, PRECEDENZA[static_cast<int>(x.precedenza) & 3], oggetto.c_str());
                    s.telescrivente.push_back(riga);
                }
                if (const auto o = r->in_onda(); o && r->progresso() > 0.0) {
                    char riga[64];
                    std::snprintf(riga, sizeof riga, "%03d %c IN RICEZIONE %d%%", o->serie, PRECEDENZA[static_cast<int>(o->precedenza) & 3],
                                  static_cast<int>(r->progresso() * 100.0));
                    s.telescrivente.push_back(riga);
                }
            }
        }
        if (consolle_ && s.enhanced && (g->flag54 & 6)) {   // ESM con l'albero su: la panoramica per banda e le intercettazioni
            static constexpr const char BANDE[] = "BCDEFGHIJ";
            s.panoramica.assign(90, 0.0f);
            for (std::size_t k = 0; k < s.panoramica.size(); ++k)   // il fondo del ricevitore, fermo
                s.panoramica[k] = 0.05f + 0.03f * static_cast<float>((k * 7919u) % 5u) / 4.0f;
            for (const auto& x : m_.intercettazioni()) {
                const char* banda = x.banda.empty() ? nullptr : std::strchr(BANDE, x.banda[0]);
                if (!banda) continue;
                const int c = static_cast<int>(banda - BANDE) * 10 + 5;   // il centro della banda
                const float picco = static_cast<float>(std::clamp(x.ampiezza, 1, 5)) / 5.0f;
                for (int k = c - 3; k <= c + 3; ++k)
                    if (k >= 0 && k < 90)
                        s.panoramica[static_cast<std::size_t>(k)] =
                            std::max(s.panoramica[static_cast<std::size_t>(k)], picco * (1.0f - static_cast<float>(std::abs(k - c)) / 4.0f));
                std::string identita = x.identita.substr(0, 8);
                for (auto& ch : identita) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
                char riga[48];
                std::snprintf(riga, sizeof riga, "%03.0f %s %s %d", normalizza_360(x.rilevamento), x.banda.c_str(), identita.c_str(), x.ampiezza);
                s.intercettazioni.push_back(riga);
                if (x.rischio == Rischio::probabile || x.rischio == Rischio::scoperto) s.minaccia = true;
            }
        }
        if (s.radar) s.intercettazioni.insert(s.intercettazioni.begin(), "PROPRIO RADAR ACCESO");   // ci sentono
        p.radio = s;
    } else if (stazione_ == 5 && m_.con_periscopi() && m_.posto_periscopi()) {   // Periscopio coi periscopi veri (P4)
        const PostoPeriscopi& pp = *m_.posto_periscopi();
        const StatoPeriscopio& st = pp.stato();
        const DatiPeriscopio& d = *pp.dati();
        StatoPeriscopi s;
        for (const auto& x : pp.periscopi()) {
            s.nomi.push_back(nome_periscopio_pulsante(x.nome));
        }
        s.scelto = pp.indice();
        char corsa[32];
        if (st.danneggiato) std::snprintf(corsa, sizeof corsa, "AVARIA");
        else if (st.su) std::snprintf(corsa, sizeof corsa, "SU");
        else if (st.comandato_su) std::snprintf(corsa, sizeof corsa, "IN SALITA %d", static_cast<int>(st.alzata * 100.0));
        else if (st.alzata > 0.0) std::snprintf(corsa, sizeof corsa, "IN DISCESA %d", static_cast<int>(st.alzata * 100.0));
        else std::snprintf(corsa, sizeof corsa, "GIU'");
        s.corsa = corsa;
        if (static_cast<std::size_t>(st.ingrandimento) < d.ingrandimenti.size()) {
            char x[16];
            std::snprintf(x, sizeof x, "%gX", d.ingrandimenti[static_cast<std::size_t>(st.ingrandimento)]);
            s.ingrandimento = x;
            for (auto& ch : s.ingrandimento) if (ch == '.') ch = ',';
        }
        static constexpr const char* MODI[] = {"GIORNO", "BASSA LUM.", "INTENSIF.", "TV", "TERMICO"};
        s.modo = MODI[static_cast<int>(st.modo)];
        if (st.filtro >= 0 && static_cast<std::size_t>(st.filtro) < d.filtri.size()) {
            s.filtro = d.filtri[static_cast<std::size_t>(st.filtro)];
            for (auto& ch : s.filtro) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        }
        s.elevazione = st.elevazione;
        s.esposto_s = st.esposto_s;
        s.tremolio = st.tremolio;
        s.su = st.comandato_su;
        s.finestra_coperta = st.finestra_coperta;
        s.telecamera = alfa();
        s.telecamera_accesa = st.telecamera_verticale;
        s.foto = d.foto && st.su;
        s.danneggiato = st.danneggiato;
        s.nwp = !carta_.anni_89;
        s.giri = static_cast<int>(giro_gradi_ / 360.0);
        s.aria = aria_;
        p.periscopi = s;
    } else if (stazione_ == 9) {   // Centrale: stato rapido e menu delle stazioni
        StatoCentrale s;
        s.quota_m = g->quota;
        s.rotta = g->rotta;
        s.velocita_kn = g->velocita;
        if (const DinamicaSottomarino* d = m_.con_dinamica() ? m_.dinamica(g->id) : nullptr) {
            s.quota_m = d->quota_m();
            s.rotta = d->rotta();
            s.velocita_kn = d->velocita_kn();
        }
        const auto lista = segni();
        s.contatti = static_cast<int>(lista.size());
        for (const auto& x : lista) s.tracce += x.traccia() ? 1 : 0;
        s.armi_in_acqua = st_.armi().in_acqua();
        for (int i = 0; i < SISTEMI; ++i) s.guasti += st_.fuori_uso(m_, static_cast<SistemaClassico>(i)) ? 1 : 0;
        s.percento = percento_sistemi(st_, m_);
        s.pausa = in_pausa();
        s.periscopio = (g->flag54 & 2) != 0;
        s.antenna = (g->flag54 & 4) != 0;
        s.schiera = st_.sonar().schiera_esposta();
        s.radar = m_.radar_giocatore_acceso();
        p.centrale = s;
    } else if (stazione_ == 3) {   // quadro di stato: i sei sistemi e l'integrita' dello scafo
        static constexpr const char* SISTEMI_NOMI[] = {"CALCOLATORE SONAR", "ANTENNA RIMORCHIATA", "PERISCOPIO",
                                                       "SONAR DI PUNTAMENTO", "MOTORI", "ANTENNA"};
        StatoSistemi s;
        const auto* sistemi = st_.danni() ? st_.danni()->sistemi(g->id) : nullptr;
        if (st_.danni()) {   // Enhanced: i tredici sistemi del battello con l'efficienza (BattelloDanni)
            for (int i = 0; i < static_cast<int>(Sistema::numero); ++i) {
                std::string nome = nome_sistema(static_cast<Sistema>(i));
                for (auto& ch : nome) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
                const double e = sistemi && static_cast<std::size_t>(i) < sistemi->size() ? (*sistemi)[static_cast<std::size_t>(i)].efficienza : 1.0;
                s.guasti.push_back({nome, e < 0.5, static_cast<int>(std::lround(e * 100.0))});
            }
        } else {
            for (int i = 0; i < SISTEMI; ++i) s.guasti.push_back({SISTEMI_NOMI[i], st_.fuori_uso(m_, static_cast<SistemaClassico>(i))});
        }
        s.percento = percento_sistemi(st_, m_);
        for (const auto& x : m_.messaggi()) s.scafo_violato = s.scafo_violato || (x.unita == g->id && x.codice == 53);
        p.stato = s;
    } else if (stazione_ == 6) {
        StatoNavigazione n;
        n.punti = static_cast<int>(m_.punti_di_rotta().size());
        n.pilota = m_.pilota_automatico();
        n.minuti = carta_.minuti();
        n.proiezione = carta_.proiezione;
        n.fattore = carta_.fattore();
        n.strumenti = carta_.restyle;
        n.tutte_le_scie = carta_.tutte_le_scie;
        n.scia_bersaglio = carta_.scia_bersaglio;
        if (pagina_fondale_) {   // la riga dell'ecoscandaglio
            n.fondale = true;
            n.fondale_riga = "ECOSCANDAGLIO";
            if (const auto e = m_.ecoscandaglio(g->id)) {
                n.fondale_riga += e->periodo_min > 0.0 ? "  OGNI " + std::to_string(static_cast<int>(e->periodo_min)) + " MIN" : std::string("  A COMANDO");
                n.fondale_riga += e->potenza == PotenzaEco::piena ? "  POTENZA PIENA" : "  POTENZA BASSA";
                if (e->ultimo)
                    n.fondale_riga += e->ultimo->eco ? "  SOTTO LA CHIGLIA " + std::to_string(static_cast<int>(std::lround(e->ultimo->sotto_chiglia_m))) + " M"
                                                     : std::string("  NESSUN ECO");
            }
            n.fondale_riga += misure_fondale_ ? "  MISURE SULLA CARTA SI" : "  MISURE SULLA CARTA NO";
        }
        if (tma_vista_) {
            const Tma& tma = st_.tma();
            n.tma = true;
            n.tma_distanza_nm = soluzione_.distanza_m / MIGLIO;
            n.tma_rotta = soluzione_.rotta;
            n.tma_velocita_kn = soluzione_.velocita_m_s / m_.metri_al_secondo(1.0);   // nodi del giocatore (moto classico)
            n.tma_scarto = tma.scarto_quadratico_medio(soluzione_);
            n.tma_rilevamenti = static_cast<int>(tma.rilevamenti().size());
            n.tma_rateo = tma.rateo(m_.tempo() - 120.0, m_.tempo());
        }
        p.navigazione = n;
    }
}

namespace {
void linea(Tela& t, Vec2 a, Vec2 b, std::uint8_t c) {   // Bresenham semplice (il ritaglio lo fa Tela::pixel)
    const int passi = static_cast<int>(std::max(std::fabs(b.x - a.x), std::fabs(b.y - a.y))) + 1;
    for (int i = 0; i <= passi && i < 4000; ++i) {
        const double k = static_cast<double>(i) / passi;
        t.pixel(static_cast<int>(a.x + (b.x - a.x) * k), static_cast<int>(a.y + (b.y - a.y) * k), c);
    }
}
}  // namespace

// MATE sulla carta: le linee di rilevamento, le posizioni della soluzione agli stessi istanti e la striscia degli scarti
namespace {
// i nomi dei punti nave (data/motore/navigazione.toml) e perche' uno non si puo' fare adesso
std::string nome_punto_nave(const std::string& id) {
    static const std::map<std::string, std::string> NOMI = {
        {"transit", "TRANSIT"}, {"gps", "GPS"}, {"loran_c", "LORAN-C"}, {"cajka", "CAJKA"}, {"omega", "OMEGA"},
        {"rsdn20", "RSDN-20"}, {"parus", "PARUS"}, {"astronomico", "ASTRONOMICO"}, {"radar_costa", "RADAR SULLA COSTA"},
        {"visivo", "VISIVO SULLA COSTA"}, {"batimetrico", "BATIMETRICO"}};
    const auto it = NOMI.find(id);
    if (it != NOMI.end()) return it->second;
    std::string s = id;
    for (auto& ch : s) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch == '_' ? ' ' : ch)));
    return s;
}
const char* motivo_punto_nave(MotivoPuntoNave m) {
    switch (m) {
        case MotivoPuntoNave::nessuno: return "";
        case MotivoPuntoNave::marina: return "NON DELLA NOSTRA MARINA";
        case MotivoPuntoNave::anno: return "NON ANCORA IN SERVIZIO";
        case MotivoPuntoNave::opzione: return "SOLO CON L'OPZIONE 1990+";
        case MotivoPuntoNave::copertura: return "FUORI COPERTURA";
        case MotivoPuntoNave::quota: return "TROPPO PROFONDI";
        case MotivoPuntoNave::velocita: return "TROPPO VELOCI";
        case MotivoPuntoNave::albero: return "SERVE L'ANTENNA SU";
        case MotivoPuntoNave::filo: return "SERVE IL FILO";
        case MotivoPuntoNave::cielo: return "CIELO COPERTO";
        case MotivoPuntoNave::notte: return "SERVE IL CREPUSCOLO O LA NOTTE";
        case MotivoPuntoNave::costa: return "COSTA TROPPO LONTANA";
        case MotivoPuntoNave::fari: return "NESSUN FARO IN VISTA";
    }
    return "";
}
}  // namespace

// Il menu dei punti nave (navigazione realistica, Ctrl+O alla carta): quelli della propria marina con la precisione, la
// durata dell'esposizione e se emettono; quelli che non si possono fare adesso in grigio, col motivo
void InterfacciaMissione::disegna_menu_punto_nave(Tela& t, const Font& f) const {
    const auto possibili = m_.punti_nave_possibili();
    const int x0 = 16, y0 = 40, w = 400, h = 30 + 11 * static_cast<int>(possibili.size());
    t.rettangolo_pieno(x0, y0, w, h, colori::NERO);
    t.linea_orizzontale(x0, x0 + w - 1, y0, colori::GRIGIO);
    t.linea_orizzontale(x0, x0 + w - 1, y0 + h - 1, colori::GRIGIO);
    t.linea_verticale(x0, y0, y0 + h - 1, colori::GRIGIO);
    t.linea_verticale(x0 + w - 1, y0, y0 + h - 1, colori::GRIGIO);
    f.scrivi(t, x0 + 6, y0 + 5, "PUNTO NAVE: LA CIFRA LO ORDINA, ESC CHIUDE", colori::CIANO_CHIARO, 1);
    int y = y0 + 18;
    char riga[160];
    for (std::size_t i = 0; i < possibili.size() && i < 9; ++i, y += 11) {
        const auto& [id, v] = possibili[i];
        const std::uint8_t col = v.possibile ? (v.emissione ? colori::GIALLO : colori::BIANCO) : colori::GRIGIO;
        std::snprintf(riga, sizeof riga, "%zu %s", i + 1, nome_punto_nave(id).c_str());
        f.scrivi(t, x0 + 6, y, riga, col, 1);
        if (v.possibile)
            std::snprintf(riga, sizeof riga, "+-%.2f NM   %.0f S%s", v.sigma_nm, v.durata_s, v.emissione ? "   EMETTE" : "");
        else
            std::snprintf(riga, sizeof riga, "%s", motivo_punto_nave(v.motivo));
        f.scrivi(t, x0 + 160, y, riga, col, 1);
    }
}

void InterfacciaMissione::disegna_tma(Tela& t, const Font& f) const {
    const Tma& tma = st_.tma();
    const VistaCarta v = vista_carta(m_, carta_);
    t.imposta_ritaglio({v.x, v.y, v.w, v.h});
    const auto& r = tma.rilevamenti();
    for (std::size_t i = 0; i < r.size(); i += 3) {
        const double b = radianti(r[i].rilevamento), l = soluzione_.distanza_m * 1.3;
        linea(t, v.a_tela(r[i].proprio), v.a_tela({r[i].proprio.x + l * std::sin(b), r[i].proprio.y + l * std::cos(b)}),
              colori::GRIGIO);
        if (const auto p = tma.posizione(soluzione_, r[i].tempo)) {
            const Vec2 q = v.a_tela(*p);
            t.rettangolo_pieno(static_cast<int>(q.x) - 1, static_cast<int>(q.y) - 1, 3, 3, colori::GIALLO);
        }
    }
    if (const auto p = tma.posizione(soluzione_, m_.tempo())) {
        const Vec2 q = v.a_tela(*p);
        if (carta_.restyle) simboli::disegna_croce_tma(t, q, simboli::indice_croce_tma(carta_.stile == StileCarta::vga));
        else f.scrivi(t, static_cast<int>(q.x) - 3, static_cast<int>(q.y) - 3, "+", 12, 1);
    }
    // striscia degli scarti: un punto per rilevamento, +-5 gradi a tutta altezza
    const int x0 = 20, y0 = v.y + v.h - 50, w = v.w - 40, h = 40;
    t.rettangolo_pieno(x0, y0, w, h, colori::NERO);
    t.linea_orizzontale(x0, x0 + w - 1, y0 + h / 2, colori::GRIGIO_SCURO);
    const auto e = tma.scarti(soluzione_);
    for (std::size_t i = 0; i < e.size(); ++i) {
        const int x = x0 + static_cast<int>(i * static_cast<std::size_t>(w - 1) / std::max<std::size_t>(e.size(), 1));
        const int y = y0 + h / 2 - static_cast<int>(std::clamp(e[i], -5.0, 5.0) / 5.0 * (h / 2 - 1));
        t.pixel(x, y, std::fabs(e[i]) < 1.0 ? colori::VERDE_CHIARO : colori::GIALLO);
    }
    f.scrivi(t, x0 + 4, y0 + 2, "SCARTI MATE +-5°", colori::GRIGIO, 1);
    // l'ultima osservazione al periscopio dell'angolo sulla prora del bersaglio (docs/25 §3.4): la rotta che ne esce
    // contro quella della soluzione
    for (auto it = m_.osservazioni_aob().rbegin(); it != m_.osservazioni_aob().rend(); ++it)
        if (it->contatto == st_.armi().bersaglio()) {
            const double scarto = normalizza_180(it->rotta - soluzione_.rotta);
            const int sec = static_cast<int>(carta_.ora_inizio_s + it->t) % 86400;   // l'ora UTC, come l'orologio
            char ora[16], riga[128];
            std::snprintf(ora, sizeof ora, "%02d:%02d:%02dZ", sec / 3600, sec / 60 % 60, sec % 60);
            std::snprintf(riga, sizeof riga, "PERISCOPIO %s: AOB %.0f%s +-%.0f  ROTTA %03.0f   SOLUZIONE %03.0f  SCARTO %+.0f°",
                          ora, std::fabs(it->aob_gradi), it->aob_gradi > 0.0 ? " DR" : it->aob_gradi < 0.0 ? " SN" : "",
                          it->incertezza_gradi, it->rotta, soluzione_.rotta, scarto);
            f.scrivi(t, x0 + 160, y0 + 2, riga, std::fabs(scarto) <= it->incertezza_gradi ? colori::VERDE_CHIARO : colori::GIALLO, 1);
            break;
        }
    // S5d: il CPA misurato dal verniero sul bersaglio (curva a S) contro quello che la soluzione prevede
    if (const auto& rm = sonar_enh_.riga_misurata(); rm && rm->cpa.valida && rm->contatto == st_.armi().bersaglio())
        if (const auto sc = confronta_cpa(tma, soluzione_, rm->cpa)) {
            char riga[160];
            std::snprintf(riga, sizeof riga, "CPA MISURATO %.1f KM %.0f KN   SOLUZIONE %.1f KM %.0f KN   SCARTO %+.0f S  %s",
                          rm->cpa.distanza_cpa_m / 1000.0, rm->cpa.velocita_relativa_m_s / 0.5144, sc->distanza_prevista_m / 1000.0,
                          sc->velocita_prevista_m_s / 0.5144, sc->scarto_t_s, sc->coerente ? "COERENTE" : "NON COERENTE");
            f.scrivi(t, x0 + 4, y0 + h - 10, riga, sc->coerente ? colori::VERDE_CHIARO : colori::GIALLO, 1);
            if (const auto p = soluzione_da_cpa(tma, rm->cpa)) {   // la proposta che Invio carica, con le sue sigma
                std::snprintf(riga, sizeof riga, "INVIO: PROPOSTA DAL CPA  %.1f KM +-%.1f  ROTTA %03.0f +-%.0f  %.0f KN +-%.0f",
                              p->soluzione.distanza_m / 1000.0, p->sigma_distanza_m / 1000.0, p->soluzione.rotta, p->sigma_rotta_deg,
                              p->soluzione.velocita_m_s / m_.metri_al_secondo(1.0), p->sigma_velocita_m_s / m_.metri_al_secondo(1.0));
                f.scrivi(t, x0 + 4, y0 - 10, riga, colori::CIANO_CHIARO, 1);
            }
        }
    t.togli_ritaglio();
}

PostoEnhanced* InterfacciaMissione::posto_enhanced() {
    const auto* g = m_.giocatore();
    const DinamicaSottomarino* d = g && m_.con_dinamica() ? m_.dinamica(g->id) : nullptr;
    if (!d) return nullptr;
    if (!posto_enh_) {
        const int id = g->id;
        posto_enh_.emplace([this, id](double kn) { const auto* x = m_.dinamica(id); return x ? x->giri_per_velocita(kn) : 0.0; },
                           d->parametri().velocita_max_kn);
        posto_enh_->imposta(PostoEnhanced::STOP, 0, d->quota_m());
    }
    return &*posto_enh_;
}

void InterfacciaMissione::ricostruzione(bool si) {
    ricostruzione_ = si;
    if (si) return;
    colpi_sentiti_ = m_.colpi().size();
    art_sentiti_ = m_.eventi_artiglieria().size();
    if (navi_)
        for (const auto& n : navi_->navi()) navi_sentite_[n->unita] = {n->danni->scoppi().size(), n->danni->spezzata()};
    tuoni_fino_ = m_.tempo();
}

void InterfacciaMissione::dopo_passo() {
    // il ripetitore di manovra (gruppo 5): un campione della quota della chiglia ogni 10 s e lo strato del momento.
    // Solo nell'Enhanced con la dinamica: il classico resta com'era
    if (const auto* gq = m_.giocatore(); gq && enhanced_ && m_.con_dinamica() && m_.tempo() + 1e-9 >= prossimo_campione_quota_) {
        UnitaClassica u = *gq;
        if (const DinamicaSottomarino* d = m_.dinamica(gq->id)) u.quota = d->quota_m();
        storia_quota_.emplace_back(m_.tempo(), quota_a_schermo(m_, u));
        while (storia_quota_.size() > CAMPIONI_QUOTA) storia_quota_.pop_front();
        prossimo_campione_quota_ = (std::floor(m_.tempo() / PASSO_QUOTA_S + 1e-9) + 1.0) * PASSO_QUOTA_S;
        if (m_.con_acustica()) {   // la SLD del profilo del suono nella posizione del battello (0: nessuno strato)
            const auto scena = m_.scena_acustica(gq->id, std::string());
            strato_m_ = scena.ambiente.profilo ? scena.ambiente.profilo->sld_m : 0.0;
        }
    }
    // gli urti del battello (valuta_urto, dossier 16 §2.6): il rollio provocato fa inclinare la vista e torna smorzato
    if (const auto* gi = m_.giocatore(); gi && m_.con_acustica())   // solo nell'Enhanced: il classico resta com'era
        for (; urti_visti_ < m_.urti().size(); ++urti_visti_) {
            const Missione::UrtoMissione& u = m_.urti()[urti_visti_];
            const DanniUrtoUnita* proprio = u.urto.a.unita == gi->id ? &u.esito.a : u.urto.b.unita == gi->id ? &u.esito.b : nullptr;
            if (proprio && proprio->rollio_gradi > 0.5) {
                rollio_ampiezza_ = proprio->rollio_gradi;
                rollio_da_ = u.urto.tempo_s;
            }
        }
    if (!m_.con_acustica() && suoni_classici_.vista_periscopio())   // classico: le animazioni dei colpi nel campo
        if (const auto* g0 = m_.giocatore()) {
            std::vector<int> in_vista;
            for (const auto& [id, u] : m_.unita())
                if (id != g0->id && !u.distrutta &&
                    std::fabs(normalizza_180(rilevamento(g0->pos, u.pos) - ril_vista_)) <= semicampo_vista_)
                    in_vista.push_back(id);
            suoni_classici_.fotogramma_periscopio(in_vista);
        }
    if (const RicevitoreIntercettazione* r = m_.con_acustica() ? m_.intercettatore_giocatore() : nullptr) {
        const Allarme a = r->allarme();   // l'allarme che sale suona; quando cala, si riarma
        if (!ricostruzione_ && static_cast<int>(a) > static_cast<int>(ultimo_allarme_)) suona_allarme(a);
        ultimo_allarme_ = a;
    }
    if (!ricostruzione_ && m_.con_missili() && !m_.sandbox() && !st_.tma().rilevamenti().empty()) {
        // la soluzione del MATE va ai tubi coi missili quando cambia (fonte TMA del piano di tiro, docs/21 §5): come
        // ordini, cosi' il replay la ripete. Sigma: trasversale dallo scarto dei rilevamenti, in distanza il 20% (STIMA,
        // dossier 05 §1.4: errore 10-20%)
        const double d = soluzione_.distanza_m, kn = soluzione_.velocita_m_s / 0.5144;
        const double sigma_t = d * radianti(st_.tma().scarto_quadratico_medio(soluzione_)), sigma_d = 0.2 * d;
        // arrotondata (100 m, 1 grado, mezzo nodo, sigma a 100 m) e al piu' ogni 10 s: il registro degli ordini resta corto
        const auto tondo = [](double v, double passo) { return std::round(v / passo) * passo; };
        const std::array<double, 5> ora{tondo(d, 100.0), tondo(soluzione_.rotta, 1.0), tondo(kn, 0.5), tondo(sigma_d, 100.0),
                                        tondo(sigma_t, 100.0)};
        if (ora != soluzione_missili_ && m_.tempo() >= prossima_soluzione_missili_) {
            soluzione_missili_ = ora;
            prossima_soluzione_missili_ = m_.tempo() + 10.0;
            for (int tubo = 1; tubo <= st_.armi().numero_tubi(); ++tubo)
                if (tubo_con_missile(tubo))
                    for (const auto& o : StazioneMissili::ordini_soluzione(m_.tempo(), tubo, ora[0], ora[1], ora[2], ora[3], ora[4])) ordina(o);
        }
    }
    if (!ricostruzione_) {
        {
            Cronometro cr(&RegistroTempi::Riga::sonar);
            sonar_enh_.aggiorna(m_, st_.armi().bersaglio());
        }
        for (const int k : sonar_enh_.messaggi()) messaggio(k);   // rapporti dei tracker
    }
    const auto* g = m_.giocatore();
    // artiglieria (docs/16, G6): le cadute viste dal battello esposto, entro 10 km, diventano croci ostili sulla carta
    // (le ultime 40)
    for (; g && impatti_art_visti_ < m_.impatti_artiglieria().size(); ++impatti_art_visti_) {
        const ImpattoArtiglieria& i = m_.impatti_artiglieria()[impatti_art_visti_];
        if (((g->flag54 & 6) || g->quota <= 2.0) && distanza(g->pos, i.punto) <= 10000.0) {
            carta_.cadute.push_back({i.punto, Segno::Affiliazione::ostile, Segno::Dominio::croce, {}, i.tempo_impatto_s});
            if (carta_.cadute.size() > 40) carta_.cadute.erase(carta_.cadute.begin());
        }
    }
    if (!carta_.restyle || !g) return;
    // ASW aereo (docs/13, A5): quel che il battello ha sentito o visto diventa una linea di rilevamento sul lucido, dalla
    // posizione del momento, come la segnerebbe il plotter: tonfi, SUS, ping di boa attiva, sonar a immersione, siluro
    // aviolanciato, fumogeno al periscopio, radar d'aereo all'ESM. Mai la boa stessa: la sua posizione resta ignota.
    for (; asw_sulla_carta_ < m_.messaggi_asw().size(); ++asw_sulla_carta_) {
        const AvvisoAsw& a = m_.messaggi_asw()[asw_sulla_carta_];
        if (!a.rilevamento) continue;
        static const std::map<int, std::pair<const char*, int>> LINEE = {   // fonte e matita (1 rosso, 3 verde, 4 giallo)
            {MSG_ASW_TONFO, {"TONFO", 4}},   {MSG_ASW_SUS, {"SUS", 4}},       {MSG_ASW_MARCATORE, {"FUMOG.", 4}},
            {MSG_ASW_DIP, {"DIP", 1}},       {MSG_ASW_BOA_ATTIVA, {"BOA ATT.", 1}}, {MSG_ASW_SILURO, {"SIL.AER.", 1}},
            {MSG_ASW_PERISCOPIO, {"ESM AER.", 3}},
        };
        if (const auto it = LINEE.find(a.codice); it != LINEE.end())
            carta_.lucido.aggiungi_linea({g->pos, *a.rilevamento, a.tempo, it->second.first, it->second.second});
    }
    // carta (N2): l'ultima posizione nota del bersaglio (per il datum) e le linee di rilevamento, segnate ogni 30 s
    // nel moderno e ogni 2 minuti dal plotter del 1989
    const int b = st_.armi().bersaglio();
    const auto ct = b >= 0 ? g->contatti.find(b) : g->contatti.end();
    std::string fonte_b = "B";
    if (ct != g->contatti.end()) {
        if (ct->second.lettera) fonte_b = std::string(1, ct->second.lettera);
        if (ct->second.posizione_nota || ct->second.qualita >= 800.0) ultimo_bersaglio_ = UltimoBersaglio{b, ct->second.pos_stimata, m_.tempo(), fonte_b};
    }
    // contorno di sicurezza: la quota del battello piu' 30 m di margine (STIMA, docs/ricerca/12 §5.2 punto 11); nel
    // moderno l'allarme se la rotta dei prossimi 5 minuti tocca il contorno sulla carta o entra in una zona da evitare.
    // La carta e' quella della parte: con la carta d'epoca l'allarme si fida degli stessi errori del giocatore.
    carta_.contorno_sicurezza_m = g->quota + 30.0;
    if (!carta_.anni_89 && m_.tempo() >= prossimo_allarme_carta_) {
        const Fondale* carta = m_.fondale_carta(carta_.urss ? Parte::urss : Parte::nato);
        const Vec2 v = vettore(g->rotta, m_.metri_al_secondo(g->velocita));
        bool fondo = false, zona = false;
        for (int k = 1; k <= 10; ++k) {
            const Vec2 p{g->pos.x + v.x * 30.0 * k, g->pos.y + v.y * 30.0 * k};
            if (carta && carta->profondita(p) < carta_.contorno_sicurezza_m) fondo = true;
            for (const auto& z : carta_.lucido.zone())
                if (z.esclusione && distanza(p, z.centro) < z.raggio_m) zona = true;
            for (const auto& z : m_.zone_ricevute())   // le zone protette della radio (R2)
                if (z.zona.tipo == TipoZona::protetta || z.zona.tipo == TipoZona::protetta_mobile) {
                    if (!z.vertici.empty() && dentro_poligono(z.vertici, p)) zona = true;
                    if (z.vertici.empty() && z.centro && distanza(p, *z.centro) < z.raggio_m) zona = true;
                }
        }
        if (fondo) messaggio(MSG_FONDO_SULLA_ROTTA);
        if (zona) messaggio(MSG_ZONA_SULLA_ROTTA);
        prossimo_allarme_carta_ = m_.tempo() + (fondo || zona ? 60.0 : 10.0);
    }
    if (!carta_.linee) return;
    const double cadenza = carta_.anni_89 ? 120.0 : 30.0, ora = m_.tempo();
    const auto segna = [&](const std::string& fonte, double ril, int colore) {
        const LineaRilevamento* u = carta_.lucido.ultima_linea(fonte);
        if (!u || ora - u->tempo >= cadenza) carta_.lucido.aggiungi_linea({g->pos, ril, ora, fonte, colore});
    };
    if (ct != g->contatti.end()) segna(fonte_b, ct->second.rilevamento, 1);
    if (carta_.linee >= 2)
        for (const auto& tr : sonar_enh_.tracker())
            if (!tr.perso) segna("S" + std::to_string(tr.numero), tr.rilevamento, 2 + tr.numero % 3);
}

// La pagina del quadro del fondale (docs/24 §4): la prospettiva dei ritorni, la striscia dell'ecoscandaglio, la sezione
// di prua; tutto dalla posizione stimata e dalla chiglia vera, mai dal fondale vero
void InterfacciaMissione::disegna_pagina_fondale(Tela& t, const Font& f) const {
    t.rettangolo_pieno(0, 0, LARGHEZZA_TELA, ALTEZZA_VISTA, colori::NERO);
    const auto* g = m_.giocatore();
    const QuadroFondale* q = g ? m_.quadro_fondale(g->id) : nullptr;
    if (!q) {
        f.scrivi(t, 12, 12, "NESSUN QUADRO DEL FONDALE", colori::GRIGIO, 1);
        return;
    }
    const Vec2 pos = m_.con_navigazione() ? m_.posizione_stimata(g->id) : g->pos;
    UnitaClassica u = *g;
    if (const DinamicaSottomarino* d = m_.con_dinamica() ? m_.dinamica(g->id) : nullptr) u.quota = d->quota_m();
    const double chiglia = m_.chiglia_vera(u), ora = m_.tempo();
    // sotto il ghiaccio (bozza del kit, scelta dell'utente del 29/10/2026): la volta nei tre schermi, in due fasce
    if (const QuadroVolta* v = m_.quadro_volta(g->id); v && v->celle() > 0) {
        disegna_pagina_volta(t, consolle_ ? fonti_ripetitori(f).schermo : piccolo_ ? *piccolo_ : f, *v, q, battello_volta(), ora);
        return;
    }
    StatoEcoscandaglio se;
    if (alfa()) se.nome = "NEL-5";   // l'ecoscandaglio sovietico dell'Alfa (STIMA del modello, kit 31/10/2026)
    if (const auto e = m_.ecoscandaglio(g->id)) {
        se.periodo_min = e->periodo_min;
        se.potenza_piena = e->potenza == PotenzaEco::piena;
    }
    disegna_ritorni_prospettiva(t, f, {6, 6, 400, 214}, *q, pos, g->rotta, chiglia, ora);
    disegna_ecoscandaglio(t, f, {412, 6, 222, 214}, *q, ora, se);
    disegna_sezione_prua(t, f, {6, 226, LARGHEZZA_TELA - 12, ALTEZZA_VISTA - 232}, *q, pos, g->rotta, chiglia, 30.0, ora);
}

// Il battello per le viste della volta: la posizione stimata, la chiglia vera e la cima della vela (classi.toml)
BattelloVolta InterfacciaMissione::battello_volta() const {
    const auto* g = m_.giocatore();
    BattelloVolta b;
    if (!g) return b;
    b.pos = m_.con_navigazione() ? m_.posizione_stimata(g->id) : g->pos;
    b.rotta = g->rotta, b.velocita_kn = g->velocita;
    UnitaClassica u = *g;
    if (const DinamicaSottomarino* d = m_.con_dinamica() ? m_.dinamica(g->id) : nullptr) u.quota = d->quota_m();
    b.chiglia_m = m_.chiglia_vera(u);
    b.cima_vela_m = b.chiglia_m;
    if (const auto it = classi_.find(g->classe); it != classi_.end())
        b.cima_vela_m = b.chiglia_m - it->second.larghezza_m - it->second.altezza_vela_m;
    return b;
}

void InterfacciaMissione::palette(Palette& p) const {
    if (schermata_aperta_) {   // la schermata dei comandi copre tutto: la lamiera delle pagine dell'Enhanced
        applica_tema(p, TemaStazione{});
        return;
    }
    if (stazione_ == 8 && m_.con_acustica()) sonar_enh_.applica_palette(p);
    if (stazione_ == 7 && pagina_missili_) {   // la carta di tiro, poi il tema delle spie e del FIRE (16-47)
        applica_palette_carta(p, carta_.stile);
        applica_palette_rilievo(p, carta_.stile);
        applica_tema(p, tema_consolle(alfa() ? BattelloTema::alfa : BattelloTema::usa688, 1.0));
    }
    if (stazione_ == 6 && carta_.restyle) {
        applica_palette_carta(p, carta_.stile);
        applica_palette_rilievo(p, carta_.stile);   // la terra della carta della Fase 3 (48-79)
        if (pagina_fondale_) applica_tema(p, tema_consolle(alfa() ? BattelloTema::alfa : BattelloTema::usa688, 1.0));   // il fosforo e la carta termica
    }
    if (stazione_ == 3 && spaccato_ && st_.danni()) Spaccato::imposta_palette(p);
}

void InterfacciaMissione::osserva(const Ordine& o) {
    if (o.tipo == TipoOrdine::lancia && !m_.con_acustica()) suoni_classici_.lancio_proprio();   // classico: TORPRAMP
    // Enhanced: la soluzione del MATE del giocatore al lancio, per le statistiche tecniche (errore della TMA al lancio)
    if (o.tipo == TipoOrdine::lancia && m_.con_acustica() && !m_.sandbox() && !st_.tma().rilevamenti().empty())
        if (const auto pos = st_.tma().posizione(soluzione_, m_.tempo())) {
            StimaTma s;
            s.soluzione = soluzione_;
            s.posizione = *pos;
            s.osservabile = true;
            s.scarto_deg = st_.tma().scarto_quadratico_medio(soluzione_);
            m_.soluzione_giocatore(st_.armi().bersaglio(), s);
        }
    if (StazioniMissione::dinamica(o) || o.tipo == TipoOrdine::emersione_emergenza)
        if (PostoEnhanced* p = posto_enhanced()) p->osserva(o);
}

void InterfacciaMissione::manovra_enhanced(StatoManovra& s) const {
    const auto* g = m_.giocatore();
    const DinamicaSottomarino* d = g && m_.con_dinamica() ? m_.dinamica(g->id) : nullptr;
    if (!d) return;
    s.enhanced = true;
    s.campana = posto_enh_ ? posto_enh_->campana() : PostoEnhanced::STOP;
    s.barra_gradi = d->barra_deg();
    s.giri = d->giri();
    s.rotta = d->rotta();
    s.velocita_kn = d->velocita_kn();
    s.quota_m = d->quota_m();
    s.assetto = d->assetto_deg();
    s.cavita = d->cavita();
    s.velocita_cavitazione_kn = d->velocita_cavitazione_kn();
    if (const auto& c = st_.comandi_dinamica(); c && c->quota_m) s.quota_ordinata_m = *c->quota_m;
    s.cambia_quota = std::fabs(s.quota_ordinata_m - s.quota_m) > 0.5;
    if (quota_collaudo_ > 0.0) s.quota_max_m = quota_collaudo_;
    // i timoni orizzontali non sono nella dinamica: la loro posizione stimata dalla quota da prendere (STIMA, ±20°)
    s.piani_gradi = s.cambia_quota ? std::clamp((s.quota_ordinata_m - s.quota_m) * 0.5, -20.0, 20.0) : 0.0;
    // la quota vera della chiglia (Missione::chiglia_vera; scelta dell'utente del 28/10/2026): la quota periscopica della
    // missione (6,1 m; Ctrl+P ne prende 5,5) e' quella della classe (688: 18,3 m), sopra si interpola dal pescaggio
    {
        UnitaClassica u = *g;
        u.quota = s.quota_m;
        const double quota_missione = s.quota_m;
        s.quota_m = quota_a_schermo(m_, u);
        u.quota = s.quota_ordinata_m;
        s.quota_ordinata_m = quota_a_schermo(m_, u);
        // la spia QUOTA PERISCOPICA: dove il periscopio si alza davvero (la stessa soglia del rifiuto: con la spia fino a 7,1 m il
        // battello fermo a 7 m restava con la spia accesa e il rifiuto), non in superficie
        if (!alfa()) s.quota_periscopica = quota_missione > 1.0 && quota_missione <= PostoDiComando::QUOTA_PERISCOPICA + 0.5 ? 1 : 0;
    }
    if (!alfa() && ha_schiera()) {   // la spia ARRAY: spenta con la schiera a posto (o dentro)
        s.schiera = 0;
        if (const auto sc = m_.stato_schiera(g->id)) s.schiera = sc->persa ? 3 : sc->danneggiata ? 2 : sc->fondo ? 1 : 0;
    }
    if (alfa()) {   // Boksit: i modi e il programma a tappe
        s.modo_rotta = static_cast<int>(st_.modo_boksit(0));
        s.modo_quota = static_cast<int>(st_.modo_boksit(1));
        s.tappa = st_.tappa_boksit(m_);
        s.tappe = st_.tappe_boksit();
        if (const DanniSottomarini* ds = st_.danni()) {   // le casse (runtime): Tan, sfoghi aperti, guasto
            s.tan = ds->tan(g->id);
            s.guasto_casse = ds->guasto_casse(g->id);
            if (const SottomarinoColpito* sc = ds->sottomarino(g->id); sc && sc->danni) s.sfoga = sc->danni->sfogo();
        }
    }
}

double InterfacciaMissione::scossa_attuale() const {
    if (!st_.danni() || !m_.giocatore()) return 0.0;
    const ScossaBattello s = st_.danni()->scossa(m_.giocatore()->id);
    const double dt = m_.tempo() - s.tempo;
    return s.tempo < 0.0 || dt < 0.0 || dt > 1.5 ? 0.0 : s.intensita * (1.0 - dt / 1.5);
}

double InterfacciaMissione::rollio_attuale() const {
    // un'oscillazione di 7 s che si smorza in una decina di secondi (STIMA), finita dopo 30 s
    const double dt = m_.tempo() - rollio_da_;
    if (rollio_da_ < 0.0 || dt < 0.0 || dt > 30.0) return 0.0;
    return rollio_ampiezza_ * std::exp(-dt / 10.0) * std::sin(2.0 * PI * dt / 7.0);
}

double InterfacciaMissione::tremolio() const {
    const double k = std::min(scossa_attuale() * 3.0, 1.0);
    if (k <= 0.0) return 0.0;
    const auto fase = static_cast<long>((tempo_disegno_ >= 0.0 ? tempo_disegno_ : m_.tempo()) * 25.0);   // a scatti, come lampade che vanno e vengono
    return (fase * 2654435761L >> 7) % 3 == 0 ? 0.7 * k : 0.15 * k;
}

// I suoni del classico come nel 1989 (nucleo/suoni_classici, docs/originale/suoni.md): i messaggi della missione in
// barra col loro campione e il bip del testo, gli altri testi col bip, i colpi animati al periscopio; tutto in coda sul
// canale dell'interfaccia, uno dopo l'altro come nell'originale (che fermava il gioco: qui il gioco va avanti)
void InterfacciaMissione::audio_classico() {
    const auto* g = m_.giocatore();
    if (!g || !dati_classici_) return;
    for (; msg_classici_visti_ < m_.messaggi().size(); ++msg_classici_visti_) {
        const Messaggio& x = m_.messaggi()[msg_classici_visti_];
        if (x.unita == g->id && x.codice < 100 && !StazioniMissione::nascosto(x.codice)) suoni_classici_.messaggio(x.codice);
    }
    for (; testi_classici_visti_ < equipaggio_.size(); ++testi_classici_visti_)   // risposte delle stazioni, rifiuti
        if (equipaggio_[testi_classici_visti_].codice != MSG_ANALISI) suoni_classici_.testo_in_barra();
    for (; colpi_classici_visti_ < m_.colpi().size(); ++colpi_classici_visti_) {
        const Colpo& c = m_.colpi()[colpi_classici_visti_];
        const auto it = m_.unita().find(c.vittima);
        suoni_classici_.colpo(c.vittima, it != m_.unita().end() ? it->second.colpo : 0xD3);
    }
    suoni_classici_.vista_periscopio(stazione_ != 6 && (g->flag54 & 2));
    constexpr int FS_BIP = 22050;
    constexpr float LIVELLO = 0.5f;   // sotto il fondo scala: i campioni a 4 bit vanno da un capo all'altro
    for (SuonoClassico s : suoni_classici_.prendi()) {
        s.volume *= LIVELLO;
        if (s.tipo == SuonoClassico::Tipo::bip) {
            mixer_->aggiungi(Canale::interfaccia, SuoniClassici::pcm_bip(s, FS_BIP), FS_BIP);
        } else if (const CampioneClassico* c = dati_classici_->campione(s.campione)) {
            mixer_->aggiungi(Canale::interfaccia, SuoniClassici::pcm(*c, s.ripetizioni, s.volume), s.frequenza_hz);
        }
    }
}

void InterfacciaMissione::audio(double dt_reale, double compressione) {
    const auto* g = m_.giocatore();
    if (!mixer_ || !g || in_pausa()) return;
    if (!m_.con_acustica()) {   // il classico: i campioni e i bip del 1989
        audio_classico();
        return;
    }
    const bool suona = compressione <= 8.0;
    // cuffia e bordo si generano per il tempo vero, e l'uscita ne prende 0,1 s al massimo a fotogramma: l'avanzo di un fotogramma
    // lungo resterebbe in coda, col suono sempre piu' indietro (7,5 s in 94 s alla cascata). Mai oltre 0,2 s qui, piu' la coda d'uscita
    constexpr double TETTO_CODA_S = 0.2;
    const auto fino_al_tetto = [&](Canale c) {
        const double ferma_s = static_cast<double>(mixer_->in_attesa(c)) / mixer_->parametri().campionamento_hz;
        if (anticipo_s_ > 0.0 && suona) return std::max(0.0, anticipo_s_ - ferma_s);   // il filo dell'audio consuma a tempo vero: si riporta la coda all'anticipo
        return std::clamp(dt_reale, 0.0, std::max(0.0, TETTO_CODA_S - ferma_s));
    };
    // la cuffia del sonarista, solo alla stazione Sonar
    if (stazione_ == 8) {
        const auto v = sonar_enh_.ascolta(m_, fino_al_tetto(Canale::cuffia));
        if (suona && !v.empty()) mixer_->aggiungi(Canale::cuffia, v, SonarEnhanced::CUFFIA_HZ);
    } else {
        sonar_enh_.tace();
    }
    // bordo: gli urti nuovi, lo schianto e lo stridio a bordo se e' il battello, altrimenti dall'acqua alla loro distanza
    for (; urti_sentiti_ < m_.urti().size(); ++urti_sentiti_) {
        const Missione::UrtoMissione& u = m_.urti()[urti_sentiti_];
        const DanniUrtoUnita* proprio = u.urto.a.unita == g->id ? &u.esito.a : u.urto.b.unita == g->id ? &u.esito.b : nullptr;
        bordo_.urto(u.esito, proprio, proprio ? 0.0 : distanza(g->pos, u.urto.punto));
    }
    // bordo: gli scoppi nuovi (cariche, testate, mine) alla loro distanza, poi il suono della stazione
    for (; colpi_sentiti_ < m_.colpi().size(); ++colpi_sentiti_) {
        const Colpo& c = m_.colpi()[colpi_sentiti_];
        if (c.tipo == Colpo::Tipo::attacco_casuale) continue;   // i missili dell'originale: niente in acqua
        const double kg = c.tipo == Colpo::Tipo::attacco ? 135.0 : c.classe_arma.find("mk48") != std::string::npos ? 544.0 : 300.0;
        bordo_.scoppio(distanza(g->pos, c.pos), kg, std::max(c.quota, 1.0));
    }
    // bordo: il deposito delle navi colpite che salta, coi suoi kg, e lo scafo che si spezza (T14; il rombo della rottura
    // come una carica di ROTTURA_KG sotto la chiglia, STIMA)
    if (navi_) {
        constexpr double ROTTURA_KG = 50.0, QUOTA_DEPOSITO_M = 3.0, QUOTA_ROTTURA_M = 30.0;
        for (const auto& n : navi_->navi()) {
            auto& [scoppi, rottura] = navi_sentite_[n->unita];
            const auto& s = n->danni->scoppi();
            for (; scoppi < s.size(); ++scoppi)
                if (s[scoppi].tipo == Esplosione::Tipo::deposito) bordo_.scoppio(distanza(g->pos, n->pos), s[scoppi].tnt_kg, QUOTA_DEPOSITO_M);
            if (!rottura && n->danni->spezzata()) {
                rottura = true;
                bordo_.scoppio(distanza(g->pos, n->pos), ROTTURA_KG, QUOTA_ROTTURA_M);
            }
        }
    }
    scena_bordo_s_ += dt_reale;
    if (!scena_bordo_ || scena_bordo_s_ >= 0.25) {
        const auto& sensori = sonar_enh_.sensori();
        scena_bordo_ = m_.scena_acustica(g->id, sensori.empty() ? std::string("bqq5_sfera") : sensori.front());
        proprio_bordo_ = m_.sorgente_giocatore();   // R3b: le macchine del battello, la firma che ascolta il nemico
        scena_bordo_s_ = 0.0;
        if (const Meteo* me = m_.meteo()) bordo_.meteo(me->in(g->pos));   // colpi di mare, acqua sul ponte (M5)
    }
    // il tuono dei lampi entro 20 km (il ritardo del suono e la superficie li decide il bordo)
    if (const Meteo* me = m_.meteo(); me && m_.tempo() > tuoni_fino_) {
        for (const Lampo& l : me->lampi(tuoni_fino_, m_.tempo()))
            if (const double d = distanza(g->pos, l.pos); d <= 20000.0)
                bordo_.tuono(d, l.nube_mare, me->in(g->pos).t_aria_c);
        tuoni_fino_ = m_.tempo();
    }
    // l'artiglieria (docs/16, G5-G6): ogni evento al suo istante; il ritardo in aria e lo scafo li decide il bordo
    for (; art_sentiti_ < m_.eventi_artiglieria().size(); ++art_sentiti_) {
        const Meteo* me = m_.meteo();
        bordo_.artiglieria(m_.eventi_artiglieria()[art_sentiti_], g->pos, g->quota, me ? me->in(g->pos).t_aria_c : 10.0);
    }
    // in superficie la stazione Periscopio e' la plancia: boati, sibili e tuoni all'aperto, senza lo scafo
    const StazioneBordo dove = stazione_ == 5 && g->quota <= 2.0 ? StazioneBordo::plancia
                               : stazione_ == 8                  ? StazioneBordo::sonar
                               : stazione_ == 7                  ? StazioneBordo::siluri
                                                                 : StazioneBordo::centrale;
    const RegimeBordo regime = st_.armi().silenzio()                 ? RegimeBordo::silenzioso
                               : g->velocita >= 0.8 * g->avanti_tutta ? RegimeBordo::avanti_tutta
                                                                      : RegimeBordo::normale;
    const auto v = bordo_.passo(scena_bordo_->sorgenti, g->pos, g->quota, scena_bordo_->ambiente, dove, regime, fino_al_tetto(Canale::bordo),
                                proprio_bordo_ ? &*proprio_bordo_ : nullptr);
    if (suona && !v.empty()) mixer_->aggiungi(Canale::bordo, v, bordo_.parametri().campionamento_hz);
}

// Dopo ANALIZZA: un tracker arrivato a PROBSUB o CERTSUB e' la classificazione che la stazione da' al comando. Va nello storico (ordine classifica_contatto, per il replay e il
// registro M1) sul contatto del giocatore piu' vicino al rilevamento del tracker, entro 5 gradi, una volta per classe.
void InterfacciaMissione::classifica_dai_tracker(double t) {
    const auto* g = m_.giocatore();
    if (!g) return;
    for (const TrackerSonar& tr : sonar_enh_.tracker()) {
        if (tr.perso || tr.classe.empty()) continue;
        if (tr.passo != PassoClassifica::probsub && tr.passo != PassoClassifica::certsub) continue;
        int scelto = -1;
        double scarto = 5.0;
        for (const auto& [id, c] : g->contatti) {
            const auto it = m_.unita().find(id);
            if (it == m_.unita().end() || it->second.distrutta || it->second.persa) continue;
            const double d = std::fabs(normalizza_180(rilevamento(g->pos, it->second.pos) - tr.rilevamento));
            if (d <= scarto) scarto = d, scelto = id;
        }
        if (scelto < 0) continue;
        if (const auto k = classificati_.find(scelto); k != classificati_.end() && k->second == tr.classe) continue;
        classificati_[scelto] = tr.classe;
        ordina(ordine_classifica(t, scelto, tr.classe));
    }
}

// Stazione Periscopio coi periscopi veri (docs/14 P4). 1 il periscopio successivo della classe (sale quando l'altro e'
// sceso), 2 su o giu', 3 ingrandimento, 4 modo (giorno, bassa luminosita', intensificatore, TV), 5 foto (solo chi ce
// l'ha, alzato), 6 filtro. Scelta, alzata, elevazione e foto sono ordini (cambiano la simulazione e il replay);
// ingrandimento, modo e filtro sono della vista
void InterfacciaMissione::cifra_periscopi(int n, double t) {
    const PostoPeriscopi* pp = m_.posto_periscopi();
    if (!pp) return;
    const StatoPeriscopio& s = pp->stato();
    const DatiPeriscopio& d = *pp->dati();
    switch (n) {
        case 1:
            if (pp->periscopi().size() > 1)
                ordina({t, TipoOrdine::scegli_periscopio, 0, 0, (pp->indice() + 1) % static_cast<int>(pp->periscopi().size())});
            break;
        case 2: azione_posto(Azione::periscopio); break;
        case 3:
            if (!d.ingrandimenti.empty())
                m_.vista_periscopio((s.ingrandimento + 1) % static_cast<int>(d.ingrandimenti.size()), s.modo, s.filtro,
                                    s.sdoppiamento_gradi, s.telecamera_verticale);
            break;
        case 4: {
            if (d.modi.empty()) break;
            const auto it = std::find(d.modi.begin(), d.modi.end(), s.modo);
            const auto k = it == d.modi.end() ? 0 : (static_cast<std::size_t>(it - d.modi.begin()) + 1) % d.modi.size();
            m_.vista_periscopio(s.ingrandimento, d.modi[k], s.filtro, s.sdoppiamento_gradi, s.telecamera_verticale);
            break;
        }
        case 5:
            if (d.foto && s.su) ordina({t, TipoOrdine::foto_periscopio, ultimo_rilevamento_, 0, s.ingrandimento});
            break;
        case 6:
            if (!d.filtri.empty()) {
                const int k = s.filtro + 1 >= static_cast<int>(d.filtri.size()) ? -1 : s.filtro + 1;
                m_.vista_periscopio(s.ingrandimento, s.modo, k, s.sdoppiamento_gradi, s.telecamera_verticale);
            }
            break;
        case 7:   // la telecamera verticale della vela (Alfa; PostoPeriscopi la tiene spenta sulle altre classi): solo la vista
            m_.vista_periscopio(s.ingrandimento, s.modo, s.filtro, s.sdoppiamento_gradi, !s.telecamera_verticale);
            break;
        default: break;
    }
}

// A ogni fotogramma coi periscopi veri: l'elevazione con le frecce alla stazione Periscopio (20 gradi al secondo, si
// ferma ai fermi a molla del modello, un ordine al piu' ogni 0,2 s), la vista dal modello (oculare, onde della
// simulazione) e la guardata per la lista NWP (rotazione accumulata, ricerca aerea)
void InterfacciaMissione::periscopi_vista(const Comandi& c, VistaPeriscopio& vista, double dt) {
    const PostoPeriscopi* pp = m_.posto_periscopi();
    if (!pp) return;
    const StatoPeriscopio& s = pp->stato();
    const DatiPeriscopio& d = *pp->dati();
    const double t = m_.tempo();
    if (stazione_ == 5 && c.su_giu && !frecce_al_libretto()) {
        const double prima = elevazione_voluta_;
        double e = std::clamp(prima - c.su_giu * 20.0 * dt, d.elevazione_min, d.elevazione_max);   // su: in alto
        for (const double fermo : d.fermi_gradi)   // il fermo a molla: ci si ferma, poi si riparte con un'altra pressione
            if ((prima < fermo && e > fermo) || (prima > fermo && e < fermo)) e = fermo;
        elevazione_voluta_ = e;
    }
    if (std::fabs(elevazione_voluta_ - s.elevazione) >= 0.5 && t >= prossima_elevazione_) {
        ordina({t, TipoOrdine::elevazione_periscopio, elevazione_voluta_});
        prossima_elevazione_ = t + 0.2;
    }
    prepara_vista(vista);
    ultimo_rilevamento_ = vista.rilevamento;
    // la guardata: rotazione e ricerca aerea mentre la testa e' fuori; si azzera quando torna sotto
    if (s.esposto_s > 0.0) {
        if (giro_prec_ >= 0.0) giro_gradi_ += std::fabs(normalizza_180(vista.rilevamento - giro_prec_));
        giro_prec_ = vista.rilevamento;
        aria_ = aria_ || s.elevazione >= (d.fermi_gradi.empty() ? 14.0 : d.fermi_gradi.front()) - 0.5;
    } else {
        giro_gradi_ = 0.0;
        giro_prec_ = -1.0;
        aria_ = false;
    }
}

void InterfacciaMissione::prepara_vista(VistaPeriscopio& vista) const {
    ril_vista_ = vista.rilevamento;
    semicampo_vista_ = semicampo(vista);
    // il periscopio vero: l'oculare del modello col suo stato
    const PostoPeriscopi* pp = m_.con_periscopi() ? m_.posto_periscopi() : nullptr;
    if (pp) {
        vista.oculare = parametri_oculare(*pp->dati(), pp->stato());
        // fuori dalla stazione Periscopio l'immagine arriva solo dal monitor TV della centrale (Type 18, «Сигнал» con
        // la TV-1): monocroma, con la grana del video
        if (stazione_ != 5 && pp->dati()->tv && pp->stato().su) vista.oculare.modo = ModoOculare::tv;
        vista.altezza_onde_m = altezza_significativa_m(STATO_MARE_PERISCOPI);
    }
    // il meteo della missione (Enhanced, M3) dopo l'oculare: cielo, nubi, nebbia, pioggia, onde, mare e vento
    // dell'oculare e i lampi dell'ultimo mezzo minuto
    if (const Meteo* me = m_.meteo(); me && m_.giocatore()) {
        const double t = m_.tempo();
        applica_meteo(vista, m_.meteo_qui(), me->lampi(std::max(0.0, t - 30.0), t), m_.giocatore()->pos);
    }
    // il cielo della notte che vede l'IA (Enhanced, runtime col via della principale): la Luna con la data della
    // missione e il GEO1 del fondale (Missione::cielo_notte_in), cosi' il giocatore vede la luce delle vedette nemiche
    vista.cielo_lunare = enhanced_ && m_.giocatore();
    vista.cielo = vista.cielo_lunare ? m_.cielo_notte_in(m_.giocatore()->pos) : CieloNotte{};
    // la banchisa attorno alla testa (Enhanced): dal campo del ghiaccio della missione, che segue il meteo (anche il
    // margine visto da fuori); la vela della cresta e' il pescaggio della chiglia diviso 4,5 (STIMA, rapporto tipico)
    // la terraferma (Enhanced, coll'oculare): coste, isole e fiordi dal fondale della missione; il classico non la vede
    vista.costa = {};
    vista.fari.clear();
    if (const Fondale* fo = m_.fondale(); fo && enhanced_ && pp) {
        vista.costa.attiva = true;
        vista.costa.cella_m = fo->cella_m();
        vista.costa.quota = [fo](Vec2 p) { return fo->quota(p); };
        // i fari (kit): nel piano della missione col fondale georeferenziato, col loro lampo di adesso
        if (fo->georeferenziato())
            for (std::size_t i = 0; i < fari_.size(); ++i) {
                const Faro& f = fari_[i];
                VistaPeriscopio::FaroVista fv;
                fv.pos = fo->proiettata({f.lat, f.lon});
                fv.quota_m = f.quota_m;
                fv.portata_m = f.portata_m;
                fv.colore = f.colore;
                fv.acceso = faro_acceso(f, m_.tempo());
                // la terra fra l'occhio e la lanterna la copre (kit, 03/11/2026): solo per i fari in portata
                if (const auto* g = m_.giocatore(); g && fv.acceso && std::hypot(fv.pos.x - g->pos.x, fv.pos.y - g->pos.y) < 1.3 * f.portata_m &&
                    faro_coperto([fo](Vec2 p) { return fo->quota(p); }, fo->cella_m(), g->pos, vista.quota_occhio_m, fv.pos, f.quota_m))
                    fv.acceso = false;
                fv.seme = static_cast<int>(7919 * (i + 1));
                vista.fari.push_back(fv);
            }
    }
    vista.ghiaccio = vista_ghiaccio(m_);
    if (!pp) return;
    if (enhanced_ && vista.oculare.telecamera_verticale) {
        const DatiTelecamera c = dati_telecamera(m_);
        vista.oculare.quota_m = c.quota_m;
        vista.oculare.ghiaccio = c.ghiaccio;
        vista.oculare.modo = ModoOculare::tv;   // la palette del video, come il monitor della centrale
    }
    // la testa colpita dall'artiglieria: lo stato dei danni (G3) del periscopio in uso, attacco o ricerca dal nome
    if (const DanniSottomarini* ds = st_.danni(); ds && m_.giocatore())
        if (const DanniArtiglieriaBattello* a = ds->artiglieria(m_.giocatore()->id)) {
            const std::string& nome = pp->dati()->nome;
            const bool ricerca = nome.find("ricerca") != std::string::npos || (nome.find("attacco") == std::string::npos && pp->indice() > 0);
            const TestaColpita& c = ricerca ? a->ricerca : a->attacco;
            if (c.tempo >= 0.0) {   // l'immagine si spegne in 1,5 s, le crepe e l'acqua salgono in 20 s
                const double dt = m_.tempo() - c.tempo;
                vista.oculare.spento = c.spento * std::clamp(dt / 1.5, 0.0, 1.0);
                vista.oculare.incrinato = c.incrinato * std::clamp(dt / 20.0, 0.05, 1.0);
                vista.oculare.seme_colpo = c.seme;
            }
        }
    vista.tempo_s = m_.tempo();
    vista.alto_ingrandimento = pp->stato().ingrandimento > 0;   // la rotazione rallenta con l'ingrandimento
    // le luci di navigazione (docs/27): lo stato di ogni nave dal motore (Missione::stato_luci: la chiave dello
    // scenario, le avarie, la guerra; spente senza luci_enhanced)
    vista.luci.stati.clear();
    vista.luci.configurazioni = luci_.empty() ? nullptr : &luci_;
    if (!luci_.empty())
        for (const auto& [id, u] : m_.unita())
            if (!u.giocatore && luci_.count(u.classe)) vista.luci.stati[id] = m_.stato_luci(id);
}

std::vector<std::string> InterfacciaMissione::enciclopedia_chiesta() {
    std::vector<std::string> v;
    if (!enciclopedia_) return v;
    enciclopedia_ = false;
    const auto* g = m_.giocatore();
    const auto classe = [&](const UnitaClassica& u) {   // la stessa classe sotto le categorie possibili
        for (const char* cat : {"sottomarino", "nave", "aereo", "elicottero", "bersaglio"}) v.push_back(std::string(cat) + "." + u.classe);
    };
    if (stazione_ == 7) {   // l'arma del tubo scelto, dal catalogo del carico (siluro, missile, mina, esca)
        const std::string var = st_.armi().variante(st_.armi().selezionato());
        if (!var.empty())
            for (const char* cat : {"siluro", "missile", "mina", "esca"}) v.push_back(std::string(cat) + "." + var);
    }
    if (stazione_ == 5)
        if (const PostoPeriscopi* pp = m_.con_periscopi() ? m_.posto_periscopi() : nullptr) v.push_back("periscopio." + pp->dati()->id);
    if (const auto it = m_.unita().find(st_.armi().bersaglio()); it != m_.unita().end()) classe(it->second);
    if (g) classe(*g);
    return v;
}

// La sagoma dell'arma per il kit: dal catalogo del carico se c'e' (misure vere), altrimenti la forma della famiglia
SagomaArma InterfacciaMissione::sagoma_arma(const std::string& variante, Arma famiglia) const {
    if (const CatalogoCarico* c = catalogo_carico_ ? catalogo_carico_() : nullptr)
        if (const SchedaArma* a = c->arma(variante)) return sagoma_da_scheda(*a);
    SagomaArma s;
    s.id = !variante.empty() ? variante : std::string(nome_arma(famiglia));
    s.sovietica = famiglia == Arma::type53;
    s.tipo = e_missile(famiglia) ? TipoArmaCarico::missile : famiglia == Arma::mina ? TipoArmaCarico::mina
             : famiglia == Arma::esca ? TipoArmaCarico::esca : TipoArmaCarico::siluro;
    return s;
}

int InterfacciaMissione::compartimento_danni() const {
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
    const int id = m_.giocatore()->id;
    const SottomarinoColpito* sc = st_.danni()->sottomarino(id);
    if (spaccato_ && sc && sc->danni) {   // lo spaccato del kit: intero, didascalie, dettaglio, ultimi rapporti
        const auto* g = m_.giocatore();
        const DinamicaSottomarino* din = m_.con_dinamica() ? m_.dinamica(id) : nullptr;
        const double assetto = din ? radianti(din->assetto_deg()) : 0.0;
        StatoSpaccato stato = StatoSpaccato::da(*sc->danni, g->quota, assetto, m_.tempo());
        if (catalogo_carico_ && catalogo_carico_()) {   // le rastrelliere col carico vero, un'arma per volta
            rastrelliera_.clear();
            const StazioneArmi& armi = st_.armi();
            for (const auto& [var, n] : armi.rastrelliera())
                for (int k = 0; k < n; ++k) rastrelliera_.push_back(sagoma_arma(var, arma_da_nome(var)));
            stato.rastrelliera = &rastrelliera_;
        }
        t.rettangolo_pieno(0, 0, LARGHEZZA_TELA, ALTEZZA_VISTA, colori::NERO);
        char riga[96];
        f.scrivi(t, 8, 4, alfa() ? "PULT ZHIVUCHESTI" : "QUADRO DANNI", colori::CIANO_CHIARO, 1);   // l'Alfa: kit, 31/10/2026
        std::snprintf(riga, sizeof riga, "QUOTA %.0f M  ASSETTO %+.0f", g->quota, din ? din->assetto_deg() : 0.0);
        f.scrivi(t, LARGHEZZA_TELA / 2 - f.larghezza(riga, 1) / 2, 4, riga, colori::GRIGIO, 1);
        std::string classe = nome_classe(g->classe);
        f.scrivi(t, LARGHEZZA_TELA - 8 - f.larghezza(classe, 1), 4, classe, colori::CIANO_CHIARO, 1);
        const Rettangolo intero{4, 20, LARGHEZZA_TELA - 8, 108}, didascalie{4, 128, LARGHEZZA_TELA - 8, 34};
        spaccato_->disegna(t, intero, stato);
        spaccato_->disegna_didascalie(t, f, intero, didascalie, stato);
        const int scelto = compartimento_danni();
        const DatiPaginaSquadre squadre = dati_squadre(*sc->danni, scelto, sistema_scelto_, alfa());
        const Font& piccolo = piccolo_ ? *piccolo_ : f;
        disegna_porte_spaccato(t, f, *spaccato_, intero, squadre);
        // dettaglio: il compartimento scelto con Tab, o il piu' in pericolo (acqua, fuoco, fumo)
        double x0 = 0.0, x1 = 0.0;
        if (scelto >= 0 && spaccato_->estensione(scelto, x0, x1)) {
            if (pagina_squadre_) disegna_pagina_squadre(t, piccolo, {4, 164, LARGHEZZA_TELA - 8, 148}, squadre);
            else spaccato_->disegna_dettaglio(t, {4, 164, LARGHEZZA_TELA - 8, 148}, stato, x0, x1);
            // nel quadro intero la parentesi del dettaglio, sul fondo del rettangolo
            t.linea_orizzontale(spaccato_->x_tela(intero, x0), spaccato_->x_tela(intero, x1), intero.y + intero.h - 1, colori::GIALLO);
            static constexpr const char* ROMANI[] = {"I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X"};
            if (alfa() && scelto < 10)   // gli otsek sovietici, numerati dalla prua in cifre romane
                std::snprintf(riga, sizeof riga, "OTSEK %s%s   TAB SCEGLIE", ROMANI[scelto], dettaglio_danni_ < 0 ? " (IL PIU' COLPITO)" : "");
            else
                std::snprintf(riga, sizeof riga, "COMPARTIMENTO %d%s   TAB SCEGLIE", scelto + 1, dettaglio_danni_ < 0 ? " (IL PIU' COLPITO)" : "");
            f.scrivi(t, 8, 314, riga, colori::BIANCO, 1);
        }
        std::vector<AreaPulsante> tasti;
        disegna_tasti_squadre(t, piccolo, LARGHEZZA_TELA - 4, 312, squadre, pagina_squadre_, &tasti);
        for (const auto& [r, n] : tasti) aree_vista_.push_back({r.x, r.y, r.w, r.h, "pulsante_" + std::to_string(n)});
        const auto& r = st_.danni()->rapporti(id);   // gli ultimi tre rapporti
        const std::size_t n = std::min<std::size_t>(r.size(), 3);
        for (std::size_t i = 0; i < n; ++i) {
            const auto& e = r[r.size() - n + i];
            std::string testo = e.testo;
            for (std::size_t k; (k = testo.find('%')) != std::string::npos;) testo.replace(k, 1, " PER CENTO");
            const int s = static_cast<int>(e.tempo);
            std::snprintf(riga, sizeof riga, "%02d:%02d  %s", s / 60, s % 60, testo.c_str());
            f.scrivi(t, 8, 325 + static_cast<int>(i) * 11, riga, i + 1 == n ? colori::GIALLO : colori::BIANCO, 1);
        }
        if (r.empty()) f.scrivi(t, 8, 325, "NESSUN DANNO SEGNALATO", colori::VERDE_CHIARO, 1);
        return;
    }
    t.rettangolo_pieno(0, 0, LARGHEZZA_TELA, ALTEZZA_VISTA, colori::GRIGIO_SCURO);
    f.scrivi(t, 16, 10, alfa() ? "PULT ZHIVUCHESTI" : "QUADRO DANNI", colori::CIANO_CHIARO);
    const auto* comp = st_.danni()->compartimenti(id);
    if (comp && !comp->empty()) {
        double x0 = 1e9, x1 = -1e9;
        for (const auto& c : *comp) { x0 = std::min(x0, c.x0); x1 = std::max(x1, c.x1); }
        const int sx = 40, sw = LARGHEZZA_TELA - 80, sy = 60, sh = 90;
        const auto px = [&](double x) { return sx + static_cast<int>((x - x0) / std::max(x1 - x0, 1.0) * sw); };
        for (const auto& c : *comp) {   // prua a destra
            const int a = px(c.x0), b = px(c.x1), w = std::max(b - a, 2);
            t.rettangolo_pieno(a, sy, w, sh, colori::NERO);
            const double pieno = c.volume > 0.0 ? std::clamp(c.acqua / c.volume, 0.0, 1.0) : 0.0;
            const int h = static_cast<int>(pieno * (sh - 2));
            // fumo dall'alto (grigio, a puntini se rado), fuoco sopra l'acqua (arancio), acqua dal basso
            const int hf = static_cast<int>(std::clamp(c.fumo, 0.0, 1.0) * (sh - 2 - h));
            for (int yy = sy + 1; yy < sy + 1 + hf; ++yy)
                for (int xx = a + 1; xx < a + w - 1; ++xx)
                    if (c.fumo >= 0.95 || ((xx + yy) & 1)) t.pixel(xx, yy, colori::GRIGIO);
            if (c.fuoco > 0.0) {
                const int hfu = std::max(3, static_cast<int>(std::clamp(c.fuoco, 0.0, 1.0) * (sh - 2 - h) * 0.6));
                t.rettangolo_pieno(a + 2, sy + sh - 1 - h - hfu, w - 4, hfu, static_cast<std::uint8_t>(colori::FUOCO[3]));
            }
            if (h > 0) t.rettangolo_pieno(a + 1, sy + sh - 1 - h, w - 2, h, c.imploso ? colori::ROSSO : colori::BLU);
            if (c.distrutto) {   // sventrato da un'esplosione: una croce
                for (int k = 0; k < w; ++k) {
                    t.pixel(a + k, sy + k * (sh - 1) / std::max(w - 1, 1), colori::ROSSO);
                    t.pixel(a + k, sy + sh - 1 - k * (sh - 1) / std::max(w - 1, 1), colori::ROSSO);
                }
            }
            t.linea_verticale(a, sy, sy + sh - 1, colori::GRIGIO);
            f.scrivi(t, a + 3, sy + 4, c.nome, colori::BIANCO, 1);
            f.scrivi(t, a + 3, sy + 14, c.uso, colori::GRIGIO, 1);
            if (pieno > 0.0) f.scrivi(t, a + 3, sy + 24, std::to_string(static_cast<int>(std::lround(pieno * 100))), colori::CIANO_CHIARO, 1);
            if (c.fuoco > 0.0) f.scrivi(t, a + 3, sy + 34, "FUOCO", colori::GIALLO, 1);
            else if (c.fumo >= 0.95) f.scrivi(t, a + 3, sy + 34, "FUMO", colori::BIANCO, 1);
            if (c.ossigeno < 0.17) {   // aria povera: sotto il 12% il fuoco si spegne, e non si respira
                char o2[16];
                std::snprintf(o2, sizeof o2, "O2 %.0f", c.ossigeno * 100.0);
                f.scrivi(t, a + 3, sy + 44, o2, colori::GIALLO, 1);
            }
        }
        t.linea_orizzontale(sx, sx + sw, sy - 1, colori::GRIGIO);
        t.linea_orizzontale(sx, sx + sw, sy + sh, colori::GRIGIO);
        t.linea_verticale(sx + sw, sy, sy + sh - 1, colori::GRIGIO);
        f.scrivi(t, sx + sw - f.larghezza("PRUA", 1), sy - 12, "PRUA", colori::GRIGIO, 1);
    }
    // rapporti: gli ultimi, il piu' recente in basso
    const auto& r = st_.danni()->rapporti(id);
    const std::size_t n = std::min<std::size_t>(r.size(), 14);
    for (std::size_t i = 0; i < n; ++i) {
        const auto& e = r[r.size() - n + i];
        char riga[200];
        const int s = static_cast<int>(e.tempo);
        std::string testo = e.testo;   // il font non ha il segno di percentuale
        for (std::size_t k; (k = testo.find('%')) != std::string::npos;) testo.replace(k, 1, " PER CENTO");
        std::snprintf(riga, sizeof riga, "%02d:%02d  %s", s / 60, s % 60, testo.c_str());
        f.scrivi(t, 40, 170 + static_cast<int>(i) * 12, riga, i + 1 == n ? colori::GIALLO : colori::BIANCO, 1);
    }
    if (r.empty()) f.scrivi(t, 40, 170, "NESSUN DANNO SEGNALATO", colori::VERDE_CHIARO, 1);
}

void InterfacciaMissione::disegna(Tela& t, const Font& f, const VistaPeriscopio& v) const {
    aree_vista_.clear();
    if (!m_.giocatore()) return;
    if (stazione_ == 3 && st_.danni()) disegna_danni(t, f);
    const auto lista = segni();
    TemaStazione tema = tema_consolle(alfa() ? BattelloTema::alfa : BattelloTema::usa688, v.luce);
    if (stazione_ == 8 && !alfa()) tema.luce = LuceSala::blu;   // come tema_pannello: la sala sonar del 688
    const bool centrale = stazione_ == 9 && consolle_;
    if (centrale) {   // Fase 3: la sala al posto della vista, coi posti che aprono le stazioni
        std::vector<AreaPulto> aree;
        vista_centrale(t, tema, *consolle_, m_.tempo(), &aree);
        for (const auto& a : aree)
            if (!a.comando.empty()) aree_vista_.push_back({a.r.x, a.r.y, a.r.w, a.r.h, a.comando});
    } else if (stazione_ == 3 && st_.danni()) {
    } else if (stazione_ == 7 && pagina_missili_) {   // la pagina di tiro: la carta sopra, la consolle e la WLC sotto
        const ContestoMissili k = contesto_missili();
        CartaGiocatore carta = carta_;
        carta.livelli.intestazione = false;   // la scala e il centro li scrive la pagina
        missili_.disegna_carta(t, f, m_, carta, lista, k);
        aree_missili_.clear();
        missili_.disegna_pannello(t, f, m_, k, &aree_missili_);
        aree_vista_.insert(aree_vista_.end(), aree_missili_.begin(), aree_missili_.end());
    } else if (stazione_ == 8 && m_.con_acustica() && pagina_intercetti_) {   // la pagina dell'intercettazione
        const Rettangolo area = consolle_ && !alfa() ? cornice_sala_sonar(t, tema, *consolle_) : Rettangolo{0, 0, LARGHEZZA_TELA, ALTEZZA_VISTA};
        std::vector<AreaAttiva> aree;
        intercetti_.disegna(t, f, area, m_.intercettatore_giocatore(), m_.tempo(), stile_intercetti(), &aree);
        aree_vista_.insert(aree_vista_.end(), aree.begin(), aree.end());
    } else if (stazione_ == 8 && m_.con_acustica()) {
        if (sonar_enh_.colore_rilevamento()) ril_visto_ = true;   // l'aiuto RIL guardato davvero (realismo, docs/26 §1)
        if (consolle_ && alfa()) sonar_enh_.disegna_okean(t, f, *consolle_);   // Fase 3: Okean con lo schermo di Enisej
        else if (consolle_) sonar_enh_.disegna(t, f, cornice_sala_sonar(t, tema, *consolle_));   // la sala sonar del 688
        else sonar_enh_.disegna(t, f);
    } else if (stazione_ == 2) {
        if (m_.radio() && stazioni_lamiera()) radio_.disegna_sala(t, f, consolle_->targa, m_, scritte(), alfa());   // gruppo 5
        else if (m_.radio()) radio_.disegna(t, f, m_);
        else radio_.disegna_classico(t, f, m_, storico_);
    } else if (ripetitore()) {   // gruppo 5: Manovra e Armi col periscopio giu'
        if (stazione_ == 4) disegna_ripetitore_manovra(t, f);
        else disegna_ripetitore_armi(t, f);
    } else if (stazione_ == 6 && pagina_fondale_) {
        disegna_pagina_fondale(t, f);
    } else if (stazione_ == 6) {
        disegna_carta(t, f, m_, carta_, lista, st_.armi().bersaglio(),
                      scelta_ == Scelta::punto || scelta_ == Scelta::direct ? std::optional<Vec2>(cursore_) : std::nullopt);
        if (misure_fondale_)   // le misure del quadro del fondale sulla carta (4 nella pagina del fondale)
            if (const QuadroFondale* q = m_.quadro_fondale(m_.giocatore()->id)) disegna_misure_carta(t, vista_carta(m_, carta_), *q, m_.tempo());
        // la volta del ghiaccio misurata: sempre, sotto la banchisa (un'informazione di sicurezza; la principale, 29/10/2026)
        if (const QuadroVolta* v = m_.giocatore() ? m_.quadro_volta(m_.giocatore()->id) : nullptr; v && v->celle() > 0)
            disegna_volta_carta(t, vista_carta(m_, carta_), piccolo_ ? *piccolo_ : f, *v, battello_volta(), m_.tempo());
        if (m_.giocatore() && !sonar_enh_.echi_falsi().empty()) {   // gli echi falsi delle chiglie dell'ultimo ping attivo
            std::vector<EcoCarta> echi;
            for (const auto& e : sonar_enh_.echi_falsi()) echi.push_back({e.t, e.rilevamento, e.distanza_m, e.eccesso_db});
            disegna_echi_carta(t, vista_carta(m_, carta_), echi, battello_volta().pos, m_.tempo(), SonarEnhanced::ECO_DURATA_S);
        }
        if (consolle_ && alfa()) {   // Fase 3: il plottatore Sozh, col pennino sul battello
            const Rettangolo carta{8, 8, LARGHEZZA_TELA - 16, ALTEZZA_VISTA - 16};
            const auto* g = m_.giocatore();   // il pennino sulla posizione stimata, con la navigazione realistica
            const Vec2 b = vista_carta(m_, carta_).a_tela(m_.con_navigazione() ? m_.posizione_stimata(g->id) : g->pos);
            cornice_sozh(t, carta, tema, *consolle_, b.x - carta.x, b.y - carta.y);
        }
        if (tma_vista_) disegna_tma(t, f);
        if (menu_punto_nave_) disegna_menu_punto_nave(t, f);
    } else {   // al periscopio, X o lettera dei contatti nella vista (4777), in alto
        const double campo = 2.0 * semicampo(v);
        for (const auto& s : lista)
            for (const std::optional<double>& ril : {std::optional<double>(s.rilevamento), s.traccia() ? std::optional<double>{} : s.specchio}) {
                if (!ril) continue;   // la seconda X e' l'ambiguita' della schiera
                const double d = normalizza_180(*ril - v.rilevamento);
                if (std::fabs(d) > campo / 2.0) continue;
                const int x = static_cast<int>(LARGHEZZA_TELA / 2.0 + d / campo * LARGHEZZA_TELA);
                const std::string segno = s.traccia() && s.lettera ? std::string(1, s.lettera) : "X";
                if (s.id == st_.armi().bersaglio()) t.rettangolo_pieno(x - 6, 38, 13, 13, colori::NERO);
                // come l'originale (29F4, 2A0F): la X gialla col contorno rosso scuro, la lettera bianca col contorno nero
                // (nel classico; l'Enhanced tiene il colore del contatto)
                const bool x_segno = segno == "X";
                const std::uint8_t col = x_segno || m_.con_acustica() ? colore_segno(s) : static_cast<std::uint8_t>(colori::BIANCO);
                scrivi_contornato(t, f, x - f.larghezza(segno, 1) / 2, 41, segno, col,
                                  x_segno ? static_cast<std::uint8_t>(colori::ROSSO) : static_cast<std::uint8_t>(colori::NERO));
            }
    }
    // Fase 3: alla stazione Periscopio le maniglie e la targhetta del periscopio in uso (Type 2 o 18, Signal), o il
    // monitor della TV
    if (stazione_ == 5 && consolle_ && m_.con_periscopi())
        if (const PostoPeriscopi* pp = m_.posto_periscopi())
            cornice_periscopio(t, tema, *consolle_, v.oculare.modo == ModoOculare::tv, pp->indice() + 1);
    // ESM (244C): con periscopio o antenna su, riquadri accesi se 9k < livello (688: 9, Alfa: 10), colori di DS:029B
    if (!centrale && stazione_ != 6 && stazione_ != 2 && !(stazione_ == 3 && st_.danni()) && !(stazione_ == 8 && m_.con_acustica()) &&
        !ripetitore() && (m_.giocatore()->flag54 & 6)) {   // non sopra le stazioni che coprono la vista (carta, radio, danni, sonar, ripetitori)
        static constexpr std::uint8_t COLORI_ESM[] = {9, 9, 9, 10, 10, 10, 14, 14, 12, 12};
        const bool alfa = m_.giocatore()->classe == "alfa";
        const int riquadri = alfa ? 10 : 9, livello = m_.livello_esm();
        f.scrivi(t, 12, 326, "ESM", colori::BIANCO, 1);
        for (int k = 0; k < riquadri; ++k)
            t.rettangolo_pieno(40 + k * 16, 324, 13, 11, 9 * k < livello ? COLORI_ESM[k] : colori::GRIGIO_SCURO);
        // Enhanced: le intercettazioni dell'operatore ESM (rilevamento, banda, identita', ampiezza, tendenza, rischio)
        const auto& esm = m_.intercettazioni();
        const int n = static_cast<int>(std::min<std::size_t>(esm.size(), 5));
        const bool corte = stazione_ == 5 && libretto_aperto_ && libretto_possibile() && !registro_aperto_;   // accanto alla pagina
        if (n) t.rettangolo_pieno(200, ALTEZZA_VISTA - 4 - n * 11, corte ? 218 : LARGHEZZA_TELA - 204, n * 11 + 2, colori::NERO);
        for (int i = 0; i < n; ++i) {
            const auto& x = esm[static_cast<std::size_t>(i)];
            std::string identita = x.identita.substr(0, corte ? 12 : 34);
            for (auto& ch : identita) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
            const char* tendenza = x.tendenza == Tendenza::avvicinamento ? "AVV." : x.tendenza == Tendenza::allontanamento ? "ALL." : "";
            char riga[160];
            if (corte) std::snprintf(riga, sizeof riga, "%03.0f° %-2s %-12s A%d", normalizza_360(x.rilevamento), x.banda.c_str(), identita.c_str(), x.ampiezza);
            else std::snprintf(riga, sizeof riga, "%03.0f° %-2s %-34s A%d %-4s %s", normalizza_360(x.rilevamento), x.banda.c_str(), identita.c_str(),
                               x.ampiezza, tendenza, nome_rischio(x.rischio));
            const auto col = static_cast<std::uint8_t>(x.rischio == Rischio::sicuro ? colori::VERDE_CHIARO
                                                       : x.rischio == Rischio::attenzione ? colori::GIALLO : 12);
            f.scrivi(t, 204, ALTEZZA_VISTA - 2 - (n - i) * 11, riga, col, 1);
        }
    }
    if (registro_aperto_) disegna_registro(t, f);
    if (schermata_aperta_ && testo_comandi_) {
        DatiSchermataComandi d = testo_comandi_->dati(enhanced_, alfa(), tasti_comandi_);
        d.scheda = scheda_comandi_;
        d.pagina = pagina_comandi_;
        pagine_comandi_ = disegna_schermata_comandi(t, f, *font_comandi_, *targa_comandi_, d, &aree_comandi_);
    }
    if (suggerimento_comandi_ && enhanced_ && !schermata_aperta_ && m_.tempo() < 20.0) {
        const auto k = tasti_comandi_.tasti("aiuto_comandi");
        if (!k.empty()) {
            const std::string s = k.front() + ": I COMANDI";
            const int w = f.larghezza(s, 1);
            t.rettangolo_pieno(LARGHEZZA_TELA - w - 12, 4, w + 8, 12, colori::NERO);
            f.scrivi(t, LARGHEZZA_TELA - w - 8, 6, s, colori::GIALLO, 1);
        }
    }
    // l'allarme siluro dell'intercettatore in ogni stazione (docs/22, E): una targa lampeggiante in alto, Ctrl+W apre la
    // pagina dell'intercettazione
    if (const RicevitoreIntercettazione* r = m_.con_acustica() ? m_.intercettatore_giocatore() : nullptr; r && r->allarme() != Allarme::nessuno) {
        const bool acquisizione = r->allarme() == Allarme::siluro_in_acquisizione;
        const double ora = tempo_disegno_ >= 0.0 ? tempo_disegno_ : m_.tempo();
        const bool acceso = static_cast<long>(ora * (acquisizione ? 4.0 : 2.0)) % 2 == 0;
        std::string testo = nome_allarme(r->allarme());
        for (auto& ch : testo) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        testo += "   CTRL+W";
        const int w = f.larghezza(testo, 1) + 16, x = (LARGHEZZA_TELA - w) / 2;
        t.rettangolo_pieno(x, 2, w, 13, acceso ? (acquisizione ? colori::ROSSO : colori::GIALLO) : colori::NERO);
        f.scrivi(t, x + 8, 5, testo, acceso ? colori::NERO : (acquisizione ? colori::ROSSO : colori::GIALLO), 1);
    }
    if (scelta_ == Scelta::bersaglio)   // le lettere che si possono scegliere
        for (std::size_t i = 0; i < candidati_.size(); ++i) {
            const std::string r(1, candidati_[i].first ? candidati_[i].first : 'X');
            const bool scelto = static_cast<int>(i) == zona_;
            t.rettangolo_pieno(8 + static_cast<int>(i) * 18, 340, 14, 14, scelto ? colori::GIALLO : colori::NERO);
            f.scrivi(t, 12 + static_cast<int>(i) * 18, 343, r, scelto ? colori::NERO : colori::BIANCO, 1);
        }
    // scossa (D6d): la vista sobbalza di qualche pixel, a scatti, mentre l'urto si spegne
    if (const double k = scossa_attuale(); k > 0.0) {
        const int d = std::clamp(static_cast<int>(std::lround(k * 30.0)), 1, 8) * ((static_cast<long>((tempo_disegno_ >= 0.0 ? tempo_disegno_ : m_.tempo()) * 30.0) % 2) ? 1 : -1);
        std::vector<std::uint8_t> copia(t.dati(), t.dati() + static_cast<std::size_t>(LARGHEZZA_TELA) * ALTEZZA_VISTA);
        for (int y = 0; y < ALTEZZA_VISTA; ++y) {
            const int sorgente = std::clamp(y - d, 0, ALTEZZA_VISTA - 1);
            std::copy_n(copia.data() + static_cast<std::size_t>(sorgente) * LARGHEZZA_TELA, LARGHEZZA_TELA, t.riga(y));
        }
    }
    // il rollio dopo un urto, al periscopio: la vista s'inclina (uno scorrimento verticale per colonna, per angoli piccoli
    // come una rotazione) e torna
    if (stazione_ == 5)
        if (const double r = rollio_attuale(); std::fabs(r) > 0.2) {
            const double k = std::tan(radianti(r));
            std::vector<std::uint8_t> copia(t.dati(), t.dati() + static_cast<std::size_t>(LARGHEZZA_TELA) * ALTEZZA_VISTA);
            for (int x = 0; x < LARGHEZZA_TELA; ++x) {
                const int d = static_cast<int>(std::lround(k * (x - LARGHEZZA_TELA / 2)));
                for (int y = 0; y < ALTEZZA_VISTA; ++y)
                    t.riga(y)[x] = copia[static_cast<std::size_t>(std::clamp(y - d, 0, ALTEZZA_VISTA - 1)) * LARGHEZZA_TELA + static_cast<std::size_t>(x)];
            }
        }
    // il libretto di riconoscimento accanto alla vista (docs/25 §3.1), dopo il rollio: la pagina e' in mano, non s'inclina
    if (stazione_ == 5 && libretto_aperto_ && libretto_possibile() && !registro_aperto_) disegna_libretto(t, f);
}

// --- gruppo 5: i ripetitori di Manovra e Armi col periscopio giu' (bozze in docs/ricerca/25_bozze/stazioni) ---

bool InterfacciaMissione::copre_la_vista() const {
    if (!m_.giocatore() || (stazione_ == 7 && pagina_missili_)) return false;
    if (stazione_ == 8) return m_.con_acustica() && !pagina_intercetti_;
    if (stazione_ == 6) return !pagina_fondale_;
    return stazione_ == 2 || (stazione_ == 9 && consolle_) || (stazione_ == 3 && st_.danni()) || ripetitore();
}

bool InterfacciaMissione::ripetitore() const {
    const auto* g = m_.giocatore();
    if (!g || !stazioni_lamiera() || (g->flag54 & 2)) return false;   // col periscopio su torna la vista
    if (stazione_ == 4) return m_.con_dinamica() && m_.dinamica(g->id);   // il pannello Manovra dell'Enhanced
    return stazione_ == 7 && !pagina_missili_;
}

void InterfacciaMissione::disegna_ripetitore_manovra(Tela& t, const Font& f) const {
    const auto* g = m_.giocatore();
    ripetitori::DatiManovra dm;
    // gli stessi valori del pannello: la chiglia vera, la barra, la stima dei piani, i giri e la cavitazione
    StatoManovra s;
    s.rotta = g->rotta;
    s.quota_m = g->quota;
    s.quota_ordinata_m = g->quota_ordinata;
    manovra_enhanced(s);
    // il profilo: i campioni della storia, il fondo dai sondaggi del quadro del fondale nello stesso passo di 10 s
    quadro::ProfiloQuota& pq = dm.profilo;
    std::vector<double> tempi;
    for (const auto& [tc, quota] : storia_quota_) tempi.push_back(tc), pq.quota_m.push_back(quota);
    if (pq.quota_m.empty()) tempi.push_back(m_.tempo()), pq.quota_m.push_back(s.quota_m);
    if (pq.quota_m.size() == 1) tempi.push_back(tempi.back()), pq.quota_m.push_back(pq.quota_m.back());   // una linea piatta
    pq.durata_s = std::max(PASSO_QUOTA_S, tempi.back() - tempi.front());
    const int minuti = std::max(1, static_cast<int>(std::lround(pq.durata_s / 60.0)));
    dm.titolo = (alfa() ? "PROFIL GLUBINY, ULTIMI " : "PROFILO DI QUOTA, ULTIMI ") + std::to_string(minuti) + " MIN";
    dm.alfa = alfa();
    // il fondo a ogni campione: fra due echi a meno di 3 minuti l'uno dall'altro la retta che li unisce (l'ecoscandaglio
    // a periodo), attorno a un eco isolato mezzo minuto; altrove non misurato (0)
    pq.fondo_m.assign(pq.quota_m.size(), 0.0);
    const QuadroFondale* qf = m_.quadro_fondale(g->id);
    std::vector<std::pair<double, double>> echi;   // (s, fondo m), dal piu' vecchio
    if (qf)
        for (auto it = qf->sondaggi().rbegin(); it != qf->sondaggi().rend() && it->t > tempi.front() - 180.0; ++it)
            if (it->eco) echi.emplace_back(it->t, it->fondo_m);
    std::reverse(echi.begin(), echi.end());
    for (std::size_t k = 0; k < tempi.size() && !echi.empty(); ++k) {
        const double tk = tempi[k];
        const auto dopo = std::upper_bound(echi.begin(), echi.end(), tk, [](double x, const auto& e) { return x < e.first; });
        const auto* prima = dopo != echi.begin() ? &*(dopo - 1) : nullptr;
        const auto* poi = dopo != echi.end() ? &*dopo : nullptr;
        if (prima && poi && poi->first - prima->first <= 180.0)
            pq.fondo_m[k] = prima->second + (poi->second - prima->second) * (tk - prima->first) / std::max(poi->first - prima->first, 1e-6);
        else if (prima && tk - prima->first <= 30.0) pq.fondo_m[k] = prima->second;
        else if (poi && poi->first - tk <= 30.0) pq.fondo_m[k] = poi->second;
    }
    // lo strato del profilo climatologico solo se c'e' acqua per averlo: non sotto il fondo dell'ultimo eco
    const auto eco = m_.ecoscandaglio(g->id);
    const bool sotto_il_fondo = eco && eco->ultimo && eco->ultimo->eco && strato_m_ >= eco->ultimo->fondo_m;
    pq.strato_m = sotto_il_fondo ? 0.0 : strato_m_;
    {   // la quota periscopica della classe: la chiglia alla quota periscopica della missione
        UnitaClassica u = *g;
        u.quota = QUOTA_PERISCOPICA_MISSIONE_M;
        pq.periscopica_m = m_.chiglia_vera(u);
    }
    // la scala: la traccia, lo strato e la quota periscopica con un po' d'acqua sotto; il fondo se non schiaccia la traccia
    double profondo = std::max({pq.strato_m, pq.periscopica_m, 40.0});
    for (const double q : pq.quota_m) profondo = std::max(profondo, q);
    double scala = profondo * 1.3;
    double fondo_alto = 0.0;
    for (const double x : pq.fondo_m)
        if (x > 0.0) fondo_alto = fondo_alto > 0.0 ? std::min(fondo_alto, x) : x;
    if (fondo_alto > 0.0 && fondo_alto * 1.1 <= std::max(3.0 * scala, 300.0)) scala = std::max(scala, fondo_alto * 1.1);
    pq.scala_m = std::ceil(scala / 50.0) * 50.0;

    // gli ordini contro lo stato
    const auto& c = st_.comandi_dinamica();
    const auto gradi = [](double x) {
        const long v = std::lround(x);
        char b[16];
        std::snprintf(b, sizeof b, v == 0 ? "0" : "%+ld", v);
        return std::string(b);
    };
    char b1[32], b2[32];
    std::snprintf(b2, sizeof b2, "%03ld", std::lround(normalizza_360(s.rotta)) % 360);
    if (c && c->rotta) std::snprintf(b1, sizeof b1, "%03ld", std::lround(normalizza_360(*c->rotta)) % 360);
    dm.righe.push_back({"ROTTA", c && c->rotta ? b1 : "-", b2});
    dm.righe.push_back({"VELOCITA'", posto_enh_ ? decimale(posto_enh_->nodi_campana(posto_enh_->campana()), 0) + " KN" : "-",
                        decimale(s.velocita_kn) + " KN"});
    std::snprintf(b1, sizeof b1, "%.0f M", s.quota_ordinata_m);
    std::snprintf(b2, sizeof b2, "%.0f M", s.quota_m);
    dm.righe.push_back({"QUOTA", c && c->quota_m ? b1 : "-", b2});
    dm.righe.push_back({"TIMONE", !c ? "-" : c->rotta ? "ROTTA" : gradi(c->barra_deg), gradi(s.barra_gradi)});
    dm.righe.push_back({"PIANI", "-", gradi(s.piani_gradi)});   // i piani non sono nella dinamica: la stima del pannello
    // le campane: i nomi del posto (688) o del telegrafo sovietico, traslitterati come sul pulto (Alfa)
    static constexpr const char* CAMPANE_ALFA[PostoEnhanced::CAMPANE] = {"PN", "SN", "MN", "STOP", "SMV", "MV", "SV", "PV", "SPV"};
    const auto campana = [&](int c) {
        return alfa() ? std::string(CAMPANE_ALFA[std::clamp(c, 0, PostoEnhanced::CAMPANE - 1)]) : std::string(PostoEnhanced::nome_campana(c));
    };
    dm.righe.push_back({"MACCHINA", posto_enh_ ? campana(posto_enh_->campana()) : "-",
                        "GIRI " + std::to_string(static_cast<int>(std::lround(s.giri * 100.0)))});
    dm.righe.push_back({"ZAVORRA", "-", "AUTO"});   // l'assetto lo tiene il modello: nessun comando delle casse sul 688
    // l'acqua sotto la chiglia dall'ultimo sondaggio dell'ecoscandaglio e il fondo dei due ultimi echi
    dm.chiglia = "NESSUNA MISURA (CTRL+H)";
    dm.chiglia_misurata = false;
    if (eco && eco->ultimo) {
        const Sondaggio& u = *eco->ultimo;
        dm.chiglia = u.eco ? std::to_string(static_cast<int>(std::lround(u.sotto_chiglia_m))) + " M" : std::string("NESSUN ECO");
        dm.chiglia_misurata = u.eco;
        if (u.eco && qf) {
            const Sondaggio* prima = nullptr;
            bool ultimo = true;
            for (auto it = qf->sondaggi().rbegin(); it != qf->sondaggi().rend(); ++it) {
                if (!it->eco) continue;
                if (ultimo) { ultimo = false; continue; }
                prima = &*it;
                break;
            }
            if (prima) dm.chiglia += u.fondo_m < prima->fondo_m - 2.0 ? ", IN SALITA" : u.fondo_m > prima->fondo_m + 2.0 ? ", IN DISCESA" : ", PIANO";
        }
        if (const double eta = m_.tempo() - u.t; eta >= 60.0) dm.chiglia += ", DA " + std::to_string(static_cast<int>(eta / 60.0)) + " MIN";
    }
    dm.cavitazione = (s.cavita ? "CAVITA! OLTRE " : "CAVITAZIONE A ") + decimale(s.velocita_cavitazione_kn) + " KN";
    ripetitori::manovra(t, fonti_ripetitori(f), dm);
}

void InterfacciaMissione::disegna_ripetitore_armi(Tela& t, const Font& f) const {
    const auto* g = m_.giocatore();
    const StazioneArmi& a = st_.armi();
    const CatalogoCarico* cat = catalogo_carico_ ? catalogo_carico_() : nullptr;
    // il nome breve di una variante del carico (o della famiglia), come nel pannello
    const auto nome_variante = [&](const std::string& variante, Arma famiglia) {
        if (cat)
            if (const SchedaArma* sa = cat->arma(variante)) return maiuscole(sa->nome);
        return std::string(NOMI_ARMI[static_cast<int>(famiglia)]);
    };
    // il nome corto di un siluro per la colonna: «MK 48 MOD 4» -> «MK48», «MK 48 ADCAP» -> «ADCAP»
    const auto corto = [](std::string n) {
        n = maiuscole(n);
        if (n.find("ADCAP") != std::string::npos) return std::string("ADCAP");
        if (const auto k = n.find(" MOD"); k != std::string::npos) n.erase(k);
        if (n.rfind("MK ", 0) == 0) n.erase(2, 1);
        return n;
    };
    const auto km = [](double m) { return decimale(m / 1000.0) + " KM"; };
    const auto stato_testa = [](FaseSiluro fs) {
        switch (fs) {
            case FaseSiluro::corsa: return "CORSA";
            case FaseSiluro::ricerca: return "RICERCA";
            case FaseSiluro::verifica: return "VERIFICA";
            case FaseSiluro::inseguimento: return "AGGANCIO";
            case FaseSiluro::riacquisizione: return "RIACQUIS.";
            case FaseSiluro::spento: return "SPENTO";
            case FaseSiluro::esaurito: return "ESAURITO";
            case FaseSiluro::esploso: return "ESPLOSO";
        }
        return "-";
    };
    static constexpr const char* MODI[] = {"PASSIVO", "ATTIVO", "MISTO"};
    ripetitori::DatiArmiRipetitore d;
    d.alfa = alfa();
    // le armi in acqua lanciate dal giocatore, nell'ordine dei lanci; il bersaglio e' quello che la testa riporta lungo il filo
    int n = 0, fili = 0;
    bool filoguidate = false;   // qualche arma in acqua col filo (anche tagliato)
    for (const auto& [id, u] : m_.unita()) {
        if (u.lanciatore != g->id || id == g->id || u.distrutta || u.persa || m_.da_mina(id)) continue;
        // solo le armi: nelle missioni originali anche un battello puo' avere il +62 del giocatore (SCENE2B, il 688)
        if (!m_.siluro(id) && u.categoria != "arma" && !(u.flag & 0x20)) continue;
        ripetitori::ArmaInAcqua r;
        r.numero = std::to_string(++n);
        r.arma = maiuscole(u.classe);
        r.corsa = r.filo = r.modo = r.stato = r.bersaglio = "-";
        if (const Siluro* sil = m_.siluro(id)) {
            r.arma = corto(sil->dati().nome);
            r.corsa = km(sil->percorso_m());
            if (sil->dati().filoguidato)
                r.filo = sil->filo() ? km(std::max(0.0, sil->dati().filo_siluro_m - sil->filo_siluro_svolto_m())) : std::string("TAGLIATO");
            fili += sil->filo();
            filoguidate = filoguidate || sil->dati().filoguidato;
            r.modo = sil->dati().segui_scia ? "SCIA" : MODI[static_cast<int>(sil->impostazioni().modo)];   // 53-65K: a scia
            r.stato = stato_testa(sil->fase());
            if (sil->bersaglio() >= 0) {
                const auto ct = g->contatti.find(sil->bersaglio());
                r.bersaglio = !sil->filo() ? "?" : ct != g->contatti.end() && ct->second.lettera ? std::string(1, ct->second.lettera) : "X";
            }
            if (id == a.arma_selezionata()) {   // l'arma scelta
                d.scelta_titolo = "L'ARMA SCELTA: " + r.numero + " " + maiuscole(sil->dati().nome);
                char riga[96];
                std::snprintf(riga, sizeof riga, "RILEVAMENTO %03.0f, DISTANZA STIMATA %s", normalizza_360(rilevamento(g->pos, u.pos)),
                              km(distanza(g->pos, u.pos)).c_str());
                d.scelta.push_back(riga);
                std::snprintf(riga, sizeof riga, "QUOTA %.0f M, VELOCITA' %s, RICERCA %.0f M", sil->quota(),
                              sil->impostazioni().velocita == VelocitaSiluro::alta ? "ALTA" : "BASSA", sil->impostazioni().quota_ricerca_m);
                d.scelta.push_back(riga);
                d.scelta.push_back(std::string("TESTA: ") + stato_testa(sil->fase()) + ", AUTONOMIA " + km(sil->autonomia_m()));
                if (!sil->dati().filoguidato) d.scelta.push_back("SENZA FILO: AUTONOMO DAL LANCIO");
                else if (!sil->filo()) d.scelta.push_back("FILO TAGLIATO: AUTONOMO");
                else
                    d.scelta.push_back("FILO: " + km(std::max(0.0, sil->dati().filo_battello_m - sil->filo_battello_svolto_m())) +
                                       " SULLA BOBINA DEL BATTELLO" + (sil->filo_a_rischio() ? ", A RISCHIO" : ""));
            }
        }
        d.in_acqua.push_back(std::move(r));
    }
    d.fili = std::to_string(fili) + (fili == 1 ? " FILO SU " : " FILI SU ") + std::to_string(a.limite_in_acqua());
    if (alfa() && !filoguidate) d.fili = "SENZA FILO";   // le armi dell'Alfa (SET-65, 53-65K) corrono da sole
    // i tubi: l'arma e lo stato; il tubo scelto evidenziato
    for (int tb = 1; tb <= a.numero_tubi(); ++tb) {
        ripetitori::TuboRipetitore r;
        r.numero = std::to_string(tb);
        if (alfa() && a.arma(tb) == Arma::esca) r.numero = "E";   // il lanciatore delle esche, non un tubo da siluri
        r.scelto = tb == a.selezionato();
        const StatoTubo st = a.stato(tb);
        r.arma = st == StatoTubo::vuoto ? std::string("VUOTO") : nome_variante(a.variante(tb), a.arma(tb));
        switch (st) {
            case StatoTubo::vuoto: r.stato = "-"; break;
            case StatoTubo::in_ricarica: r.stato = a.in_coda(tb) ? "IN CODA" : "RICARICA"; break;
            case StatoTubo::carico: r.stato = "CARICO"; break;
            case StatoTubo::in_armamento: r.stato = "ALLAGATO"; break;
            case StatoTubo::armato: r.stato = "PRONTO"; break;
        }
        d.tubi.push_back(std::move(r));
    }
    // il carico: la rastrelliera, i pozzi verticali e le esche del carico della sala siluri, o le scorte per famiglia
    char riga[96];
    if (a.con_carico()) {
        for (const auto& [v, k] : a.rastrelliera())
            std::snprintf(riga, sizeof riga, "%-12.12s %2d", nome_variante(v, Arma::nessuna).c_str(), k), d.carico.emplace_back(riga);
        for (const auto& [v, k] : a.pozzi())
            std::snprintf(riga, sizeof riga, "%-12.12s %2d VLS", nome_variante(v, Arma::nessuna).c_str(), k), d.carico.emplace_back(riga);
        for (const auto& [v, k] : a.esche())
            std::snprintf(riga, sizeof riga, "%-12.12s %2d", nome_variante(v, Arma::nessuna).c_str(), k), d.carico.emplace_back(riga);
    } else {
        for (const auto& [x, k] : a.scorte_tutte())
            std::snprintf(riga, sizeof riga, "%-12.12s %2d", NOMI_ARMI[static_cast<int>(x)], k), d.carico.emplace_back(riga);
    }
    if (d.carico.empty()) d.carico.emplace_back("NESSUNA SCORTA");
    // la ricarica: i tubi in ricarica e i minuti che mancano (in coda: aspettano un posto)
    for (int tb = 1; tb <= a.numero_tubi(); ++tb) {
        if (a.stato(tb) != StatoTubo::in_ricarica) continue;
        const std::string arma = nome_variante(a.variante(tb), a.arma(tb));
        if (a.in_coda(tb)) std::snprintf(riga, sizeof riga, "TUBO %d: %s, IN CODA", tb, arma.c_str());
        else std::snprintf(riga, sizeof riga, "TUBO %d: %s, %d MIN", tb, arma.c_str(), static_cast<int>(std::ceil(a.secondi_ricarica(tb) / 60.0)));
        d.ricarica.emplace_back(riga);
    }
    if (d.ricarica.empty()) d.ricarica.emplace_back(a.ricarica_realistica() ? "NESSUNA" : "-");
    if (a.silenzio()) d.ricarica.emplace_back("SILENZIO: RICARICHE LENTE");
    ripetitori::armi(t, fonti_ripetitori(f), d);
}

}  // namespace m688
