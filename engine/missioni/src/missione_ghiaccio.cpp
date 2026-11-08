// Sotto il ghiaccio nell'Enhanced (opzione A di sotto-ghiaccio-piano.md, approvata il 29/10/2026; docs/08 §7; moduli
// nucleo/campo_ghiaccio e volta_ghiaccio, del disassemblatore). Qui il collegamento:
// - il campo del ghiaccio vero, procedurale dal seme della missione sulla copertura del meteo (StatoMeteo::ghiaccio,
//   per cella d'ascolto), pluriennale oltre gli 80 gradi; non si salva (si rifa' dal seme);
// - per il giocatore e per i sottomarini dell'IA col sonar del fondo (simmetria): un QuadroVolta con il sondatore verso
//   l'alto (col BQS-15 acceso per il giocatore; da solo per l'IA, salvo in silenzio) e le spazzate sulle chiglie
//   (spazzata_volta ogni 5 s: il BQS-15 acceso dei Los Angeles, il cui impulso lo emette gia' spazzata_bqs15; l'MG-70
//   dei sovietici, che emette qui). Ogni ping del sondatore va agli intercettatori come il fathometer;
// - gli avvisi dell'equipaggio dalle sole misure, come per il fondo: 486-489 (volta bassa o che scende, chiglia a prua,
//   acqua libera sopra, alberi bloccati) e 484 (troppo veloci) gia' del fondo;
// - l'IA sotto la volta: scende sotto le chiglie misurate a prua e rallenta alla velocita' sicura;
// - l'urto della vela con il ghiaccio vero (CampoGhiaccio::pescaggio_max lungo il passo), per tutti i sottomarini: un
//   Urto con un ostacolo fisso (unita' -1, la massa della chiglia), valutato con valuta_urto e messo in urti_ (i danni li
//   applica DanniSottomarini, il registro lo scrive); il battello resta sotto la chiglia e perde la velocita'.
// Le regole di bordo (niente alberi ne' periscopio, la radio solo ELF, niente punto nave che chieda un'antenna) sono
// righe nei file del runtime e della principale: sotto_banchisa() le dice. Il classico non lo usa.

#include <algorithm>
#include <cmath>
#include <set>

#include "missioni/collisioni_missione.hpp"
#include "missioni/missione.hpp"
#include "nucleo/acustica_urto.hpp"

namespace m688 {

namespace {

constexpr double SONDATORE_OGNI_S = 2.0;        // il sondatore alto: una misura ogni 2 s nella missione (1 Hz vero, STIMA)
constexpr double AVVISI_OGNI_S = 2.0;
constexpr double SPAZZATA_IA_S = 5.0;           // le spazzate sulla volta (BQS-15 e MG-70)
constexpr double LAT_PLURIENNALE = 80.0;        // oltre, il pack pluriennale (Sharpe 1988)
constexpr double METRI_GRADO = 111320.0;
constexpr double MARGINE_IA_M = 25.0;           // l'IA tiene la cima della vela 25 m sotto le chiglie misurate (STIMA)
constexpr double AVANTI_IA_M = 2000.0;          // e guarda 2 km a prua nel suo quadro
constexpr double QUOTA_MAX_IA_M = 300.0;        // senza mai scendere oltre (il fondo lo guarda il sondaggio del fondo)
constexpr double VELOCITA_MIN_IA_KN = 4.0;
constexpr double MG70_SL_DB = 210.0, MG70_HZ = 20000.0;   // l'impulso dell'MG-70 (emissioni.mg70 di acustica.toml, STIMA)
constexpr double MG70_SETTORE_DEG = 60.0, MG70_LOBI_DB = 20.0;
constexpr double BQS8_SL_DB = 205.0, BQS8_HZ = 40000.0;   // il BQS-8 degli Sturgeon (emissioni.bqs8, docs/ricerca/34: STIMA)
constexpr double MARGINE_CHIGLIA_M = 10.0;      // la chiglia-ostacolo: fino a 10 m sopra la cima della vela
constexpr double VOLTA_BASSA_M = 15.0;          // l'avviso: la volta a meno di 15 m sopra la cima della vela (STIMA)
constexpr double URTO_DI_NUOVO_S = 20.0;        // un urto per chiglia: il prossimo dopo 20 s
constexpr double SOTTO_CHIGLIA_M = 2.0;         // dopo l'urto il battello resta 2 m sotto la chiglia
constexpr double VELOCITA_DOPO_URTO = 0.2;      // e tiene un quinto della velocita' (STIMA)
constexpr double DENSITA_GHIACCIO_T_M3 = 0.9, POROSITA_CHIGLIA = 0.7;   // la massa della chiglia: pescaggio^2 x 60 m
constexpr int MSG_TROPPO_VELOCE = 484;          // del fondo (missione_fondale.cpp)
constexpr int MSG_VOLTA_BASSA = 486, MSG_CHIGLIA = 487, MSG_ACQUA_LIBERA = 488, MSG_ALBERI_GHIACCIO = 489;
// il carattere del pack per l'acustica (dossier 06 §5.1, STIMA): le creste sono la frazione di celle con una chiglia in un
// quadrato di 2 km attorno al punto, piena al 30%; le fessure termiche crescono col freddo dell'aria sotto i -10 gradi,
// piene a -30; la rugosita' della volta e' 2,5 m nel pack pluriennale, 1 nel primo anno
constexpr double CARATTERE_CELLA_M = 5000.0, CARATTERE_OGNI_S = 600.0, CARATTERE_LATO_M = 2000.0;
constexpr int CARATTERE_PUNTI = 5;
constexpr double CRESTE_PIENE = 0.3, FESSURE_DA_C = -10.0, FESSURE_PIENE_C = -30.0;
constexpr double RUGOSITA_PRIMO_ANNO_M = 1.0, RUGOSITA_PLURIENNALE_M = 2.5, SPESSORE_PLURIENNALE_M = 2.0;
// i falsi echi per le teste (STIMA): il ventaglio verticale della testa al piu' 20 gradi; una chiglia e' muta (60 dB, sotto
// ogni ascolto), ferma (la testa la rifiuta col Doppler come un'esca, a volte) e lunga come una cresta, alla quota del suo
// pescaggio: la testa la vede solo sotto il suo soffitto (15 m di serie), quindi le creste profonde del pack pluriennale,
// o un soffitto abbassato per inseguire sotto la volta. L'id e' negativo e fisso per cella, perche' la testa la riconosca
// da un passo all'altro (il motore toglie il siluro che scoppia su un id negativo, come su un'esca)
constexpr double ELEVAZIONE_TESTA_DEG = 20.0, LIVELLO_CHIGLIA_DB = 60.0, LUNGHEZZA_CHIGLIA_M = 30.0, CELLA_ECO_M = 200.0;
constexpr int ID_CHIGLIE = -1000000;
// i falsi echi per l'attivo del giocatore (STIMA): le chiglie entro 8 km fuori dai baffles del sonar, ogni 100 m lungo i
// raggi, nel ventaglio di 20 gradi; ognuna passa per l'equazione dell'attivo con la sua forza di bersaglio (una firma muta)
constexpr double PORTATA_ECHI_M = 8000.0, PASSO_ECHI_M = 100.0, ELEVAZIONE_ATTIVO_DEG = 20.0;
// solo le creste che l'operatore vede come echi a se': almeno 6 m di pescaggio (le piu' piccole sono la volta, gia' nella
// sua riverberazione), una per cella di 200 m (la piu' forte: due raggi vicini vedono la stessa cresta) (STIMA)
constexpr double PESCAGGIO_ECO_MIN_M = 6.0;
constexpr int URTI_DA = 940000, URTI_PASSO = 25, URTI_QUANTI = 400;   // come missione_collisioni.cpp

// C'e' una volta da misurare? Sotto una polynya aperta dal vetrino di Dio la copertura e' 0, ma sonar e sondatore hanno ancora
// da vedere l'acqua libera sopra e le chiglie attorno, e il ping ne ha gli echi. Senza cambi il campo non si guarda: la copertura, bit per bit
bool volta_da_misurare(const CampoGhiaccio* g, double copertura, Vec2 p, double t) {
    return copertura > 0.0 || (g && g->modificato() && g->in(p, t).aperta);
}

}  // namespace

bool Missione::prepara_ghiaccio() {
    if (ghiaccio_) return true;
    if (ghiaccio_tentato_ || !catalogo_ || !meteo_) return false;
    ghiaccio_tentato_ = true;
    const bool pluriennale = latitudine_ >= LAT_PLURIENNALE;
    ghiaccio_.emplace(seme_ ^ 0x61ACC10ull, pluriennale ? ghiaccio_pluriennale() : ParametriGhiaccio{},
                      [this](Vec2 p) { return meteo_ascolto(p).ghiaccio; });   // Missione non si copia: this resta valido
    return true;
}

const CampoGhiaccio* Missione::ghiaccio() const { return ghiaccio_ ? &*ghiaccio_ : nullptr; }

CampoGhiaccio* Missione::ghiaccio_da_cambiare() { return prepara_ghiaccio() ? &*ghiaccio_ : nullptr; }

double Missione::copertura_ghiaccio(Vec2 p) const {
    return ghiaccio_ && ghiaccio_->modificato() ? ghiaccio_->copertura_in(p, tempo_) : meteo_ascolto(p).ghiaccio;
}

bool Missione::sotto_banchisa(int id) const {
    const auto it = unita_.find(id);
    if (!ghiaccio_ || it == unita_.end() || it->second.categoria != "sottomarino") return false;
    const UnitaClassica* u = &it->second;
    if (copertura_ghiaccio(u->pos) <= 0.0) return false;   // la via breve: niente ghiaccio nella cella
    const PuntoGhiaccio p = ghiaccio_->in(u->pos, tempo_);
    return p.ghiaccio && !p.aperta;   // anche il nilas: periscopio e alberi non lo sfondano
}

const QuadroVolta* Missione::quadro_volta(int id) const {
    const auto it = volta_.find(id);
    return it != volta_.end() ? &it->second.quadro : nullptr;
}

Missione::VoltaUnita* Missione::volta_di(const UnitaClassica& u) {
    if (!prepara_ghiaccio()) return nullptr;
    if (auto it = volta_.find(u.id); it != volta_.end()) return &it->second;
    const auto* g = giocatore();
    VoltaUnita v{QuadroVolta(ghiaccio_->parametri().cella_m), Casuale(seme_ ^ 0x601AA1ull ^ (static_cast<std::uint64_t>(u.id) * 0x9E37ull)),
                 std::nullopt, false, 0.0, 0.0, 0.0, false, {}};
    if (g && g->id == u.id) {
        v.bqs = par_bqs_fondo_;
    } else {
        // senza il sonar del fondo (Kilo, Foxtrot) il quadro ha solo il sondatore verso l'alto: le polynye che passano sopra,
        // per il rifugio dei diesel sotto la banchisa (runtime, «l'IA sotto il pack»); niente spazzate sulle chiglie
        try { v.bqs = bqs15_fondo_di_classe(acustica_toml_, u.classe); } catch (const std::exception&) {}
    }
    v.migliorato = u.classe.find("flight3") != std::string::npos || u.classe.find("688i") != std::string::npos;
    return &volta_.emplace(u.id, std::move(v)).first->second;
}

void Missione::spazzata_volta(const UnitaClassica& u) {
    if (u.quota < 5.0 || !volta_da_misurare(ghiaccio(), copertura_ghiaccio(u.pos), u.pos, tempo_)) return;
    VoltaUnita* v = volta_di(u);
    if (!v || !v->bqs) return;
    const double kn = std::fabs(u.velocita);
    const double portata = portata_chiglia_m(*v->bqs, v->migliorato);
    // l'ostacolo e' la chiglia che arriva alla cima della vela meno 10 m (il sonar sta alla quota della chiglia)
    const double vela = chiglia_vera(u) - cima_vela(u);
    v->quadro.aggiungi_spazzata(tempo_, portata_effettiva(portata, kn, *v->bqs),
                                spazza_ghiaccio(*ghiaccio_, u.pos, posizione_stimata(u.id), chiglia_vera(u), u.rotta, kn, tempo_, *v->bqs,
                                                v->caso, portata, MARGINE_CHIGLIA_M + vela));
}

void Missione::passo_ghiaccio(double dt) {
    if (!prepara_ghiaccio()) return;
    const auto* g = giocatore();
    for (auto& [id, u] : unita_) {
        if (u.distrutta || u.persa || u.categoria != "sottomarino") continue;
        const bool del_giocatore = g && g->id == id;
        urto_ghiaccio(u, dt);
        if (!volta_da_misurare(ghiaccio(), copertura_ghiaccio(u.pos), u.pos, tempo_) || u.quota < 5.0) continue;
        VoltaUnita* v = volta_di(u);
        if (!v) continue;
        // il sondatore alto: il giocatore col BQS-15 acceso; l'IA salvo in silenzio (a contatto col nemico, in allarme)
        const bool sonda = del_giocatore ? bqs15_.count(id) > 0 : allerta_unita(id).value_or(0) < 3;
        if (sonda && par_eco_ && tempo_ + 1e-9 >= v->prossimo_sondaggio) {
            v->prossimo_sondaggio = tempo_ + SONDATORE_OGNI_S;
            const SondaggioAlto s = sonda_alto(*ghiaccio_, u.pos, posizione_stimata(id), chiglia_vera(u), tempo_, ParametriSondatoreAlto{}, v->caso);
            v->quadro.aggiungi(s);
            emetti_impulso(id, "fathometer", par_eco_->sl_bassa_db, par_eco_->f_hz);
            if (del_giocatore && s.eco) {   // l'acqua libera sopra, quando lo diventa
                if (s.libera && !v->libera_sopra) messaggio_di_bordo(id, MSG_ACQUA_LIBERA);
                v->libera_sopra = s.libera;
            }
        }
        // le chiglie: il BQS-15 acceso dei Los Angeles (l'impulso lo emette spazzata_bqs15); il BQS-8 dello Sturgeon del
        // giocatore quando l'ha acceso (docs/ricerca/34); l'MG-70 dei sovietici e il BQS-8 degli Sturgeon dell'IA salvo in
        // silenzio. L'impulso, fuori dai Los Angeles, qui: quello del sistema
        const bool los_angeles = ha_bqs15(u.classe);
        const bool a_comando = los_angeles || (del_giocatore && v->bqs && !v->bqs->mine);
        if (v->bqs && (a_comando ? bqs15_.count(id) > 0 : sonda) && tempo_ + 1e-9 >= v->prossima_spazzata) {   // col sonar del fondo
            v->prossima_spazzata = tempo_ + SPAZZATA_IA_S;
            spazzata_volta(u);
            if (!los_angeles) {
                const bool bqs8 = v->bqs->emissione == "bqs8";
                emetti_impulso(id, v->bqs->emissione, bqs8 ? BQS8_SL_DB : MG70_SL_DB, bqs8 ? BQS8_HZ : MG70_HZ,
                               bqs8 ? v->bqs->settore_az_deg : MG70_SETTORE_DEG, MG70_LOBI_DB);
            }
        }
        if (tempo_ < v->prossimi_avvisi) continue;
        v->prossimi_avvisi = tempo_ + AVVISI_OGNI_S;
        const Vec2 stimata = posizione_stimata(id);
        const double kn = std::fabs(u.velocita);
        // la volta bassa: meno di 15 m sopra la cima della vela
        const auto avvisi = v->quadro.avvisi(stimata, chiglia_vera(u), u.rotta, kn, tempo_, 180.0, VOLTA_BASSA_M + chiglia_vera(u) - cima_vela(u));
        if (del_giocatore) {   // ognuno detto una volta quando diventa vero, di nuovo dopo che e' tornato falso
            std::set<int> attivi;
            for (const AvvisoVolta& a : avvisi) {
                const int codice = a.tipo == AvvisoVolta::Tipo::chiglia_a_prua      ? MSG_CHIGLIA
                                   : a.tipo == AvvisoVolta::Tipo::velocita_eccessiva ? MSG_TROPPO_VELOCE
                                                                                     : MSG_VOLTA_BASSA;   // bassa o in discesa
                attivi.insert(codice);
                if (!v->avvisi.count(codice)) messaggio_di_bordo(id, codice);
            }
            v->avvisi = std::move(attivi);
            continue;
        }
        // l'IA: sotto le chiglie misurate a prua, alla velocita' sicura; solo verso il prudente
        bool scendi = false;
        double sicura = 1e9;
        for (const AvvisoVolta& a : avvisi) {
            if (a.tipo == AvvisoVolta::Tipo::velocita_eccessiva) sicura = std::min(sicura, a.valore);
            else scendi = true;
        }
        if (scendi) {
            const double r = radianti(u.rotta);
            const double chiglie = v->quadro.pescaggio_max(stimata, {stimata.x + AVANTI_IA_M * std::sin(r), stimata.y + AVANTI_IA_M * std::cos(r)});
            const double vela = chiglia_vera(u) - cima_vela(u);   // dalla chiglia alla cima della vela
            const double serve = std::min(QUOTA_MAX_IA_M, chiglie + vela + MARGINE_IA_M);
            if (serve > u.quota_ordinata) u.quota_ordinata = serve;
        }
        if (sicura < std::fabs(u.velocita_ordinata))
            u.velocita_ordinata = std::copysign(std::max(VELOCITA_MIN_IA_KN, sicura), u.velocita_ordinata);
    }
}

void Missione::carattere_ghiaccio(Ambiente& amb, Vec2 p, double t_aria_c) const {
    if (!ghiaccio_ || amb.ghiaccio <= 0.0) return;
    const std::pair<int, int> k{static_cast<int>(std::floor(p.x / CARATTERE_CELLA_M)), static_cast<int>(std::floor(p.y / CARATTERE_CELLA_M))};
    CarattereGhiaccio& c = carattere_celle_[k];
    if (tempo_ - c.t >= CARATTERE_OGNI_S || tempo_ < c.t) {
        const Vec2 centro{(k.first + 0.5) * CARATTERE_CELLA_M, (k.second + 0.5) * CARATTERE_CELLA_M};
        int chiglie = 0, punti = 0;
        for (int i = 0; i < CARATTERE_PUNTI; ++i)
            for (int j = 0; j < CARATTERE_PUNTI; ++j) {
                const double fx = (i + 0.5) / CARATTERE_PUNTI - 0.5, fy = (j + 0.5) / CARATTERE_PUNTI - 0.5;
                const PuntoGhiaccio x = ghiaccio_->in({centro.x + fx * CARATTERE_LATO_M, centro.y + fy * CARATTERE_LATO_M}, tempo_);
                if (!x.ghiaccio) continue;
                ++punti;
                chiglie += x.chiglia;
            }
        c.t = tempo_;
        c.creste = punti > 0 ? std::min(1.0, static_cast<double>(chiglie) / punti / CRESTE_PIENE) : 0.0;
    }
    amb.ghiaccio_creste = c.creste;
    amb.ghiaccio_fessure = std::clamp((FESSURE_DA_C - t_aria_c) / (FESSURE_DA_C - FESSURE_PIENE_C), 0.0, 1.0);
    amb.rugosita_ghiaccio_m = ghiaccio_->parametri().spessore_livellato_m >= SPESSORE_PLURIENNALE_M ? RUGOSITA_PLURIENNALE_M
                                                                                                   : RUGOSITA_PRIMO_ANNO_M;
}

bool Missione::echi_chiglie_siluro(const Siluro& s, const std::vector<SorgenteAcustica>& sorgenti,
                                   std::vector<SorgenteAcustica>& con_chiglie) const {
    const DatiSiluro& d = s.dati();
    if (!ghiaccio_ || !d.attivo || d.segui_scia || copertura_ghiaccio(s.pos()) <= 0.0) return false;
    const auto echi = echi_chiglie(*ghiaccio_, s.pos(), s.quota(), s.rotta(), 2.0 * d.semiapertura_gradi, d.acquisizione_attiva_m,
                                   std::min(d.semiapertura_gradi, ELEVAZIONE_TESTA_DEG), tempo_);
    if (echi.empty()) return false;
    con_chiglie = sorgenti;
    for (const EcoChiglia& e : echi) {
        SorgenteAcustica x;
        const auto ix = static_cast<long long>(std::floor(e.pos.x / CELLA_ECO_M)) & 0x3FF, iy = static_cast<long long>(std::floor(e.pos.y / CELLA_ECO_M)) & 0x3FF;
        x.id = ID_CHIGLIE - static_cast<int>((ix << 10) | iy);
        x.pos = e.pos;
        x.quota = e.pescaggio_m;   // l'eco dal fondo della chiglia: sopra il soffitto della testa (15 m di serie) non conta
        x.livello_db = LIVELLO_CHIGLIA_DB;
        x.forza_bersaglio_db = e.ts_db;
        x.lunghezza_m = LUNGHEZZA_CHIGLIA_M;
        x.esca = true;
        con_chiglie.push_back(x);
    }
    return true;
}

void Missione::echi_chiglie_ping(const UnitaClassica& p, const Sensore& sa) { echi_falsi_ = echi_chiglie_di(p, sa); }

std::vector<Missione::EcoFalso> Missione::echi_chiglie_di(const UnitaClassica& p, const Sensore& sa) {
    std::vector<EcoFalso> echi_falsi;
    if (!ghiaccio_ || !catalogo_ || !volta_da_misurare(ghiaccio(), copertura_ghiaccio(p.pos), p.pos, tempo_)) return echi_falsi;
    const double settore = std::clamp(360.0 - 2.0 * sa.baffles_gradi, 0.0, 360.0);
    const auto echi = echi_chiglie(*ghiaccio_, p.pos, chiglia_vera(p), p.rotta, settore, PORTATA_ECHI_M, ELEVAZIONE_ATTIVO_DEG, tempo_, PASSO_ECHI_M,
                                   PESCAGGIO_ECO_MIN_M);
    if (echi.empty()) return echi_falsi;
    const bool nella_polynya = copertura_ghiaccio(p.pos) <= 0.0;   // qui, con copertura 0, solo in una zona forzata
    FirmaRumore muta;   // niente rumore, solo l'eco: la forza di bersaglio della chiglia di prua (senza il lobo del traverso)
    muta.punti = {{0.0, 0.0}, {40.0, 0.0}};
    muta.tonale_db = 0.0;
    const Ricevitore rc = ricevitore_acustico(p, &sa);
    for (const EcoChiglia& e : echi) {
        muta.ts_prua_db = muta.ts_traverso_db = e.ts_db;
        Sorgente s;
        s.firma = &muta;
        s.pos = e.pos;
        s.quota_m = e.pescaggio_m;
        s.rotta = rilevamento(e.pos, p.pos);
        s.velocita_kn = 0.0;
        // l'ambiente guarda il primo punto e vale per tutta la via: da una polynya, la via alla chiglia corre sotto il pack
        const Rilevazione x = ::m688::valuta(s, rc, nella_polynya ? ambiente_acustico(e.pos, p.pos) : ambiente_acustico(p.pos, e.pos), raggi_, tempo_);
        if (x.se_db < 0.0) continue;
        const auto stessa = std::find_if(echi_falsi.begin(), echi_falsi.end(), [&](const EcoFalso& f) {
            return std::floor(f.punto.x / CELLA_ECO_M) == std::floor(e.pos.x / CELLA_ECO_M) && std::floor(f.punto.y / CELLA_ECO_M) == std::floor(e.pos.y / CELLA_ECO_M);
        });
        if (stessa != echi_falsi.end()) {   // la stessa cresta da due raggi: la piu' forte
            if (x.se_db > stessa->eccesso_db) stessa->eccesso_db = x.se_db;
            continue;
        }
        echi_falsi.push_back({tempo_, normalizza_360(rilevamento(p.pos, e.pos) + gauss()), e.distanza_m * (1.0 + 0.01 * gauss()), x.se_db,
                              e.pos, e.pescaggio_m});
    }
    return echi_falsi;
}

double Missione::cima_vela(const UnitaClassica& u) const {
    double alto = 10.0;   // senza la classe: diametro e vela d'un 688 (STIMA)
    if (collisioni_)
        if (const auto c = collisioni_->classi.find(u.classe); c != collisioni_->classi.end())
            alto = c->second.larghezza_m + c->second.altezza_vela_m;
    return chiglia_vera(u) - alto;
}

void Missione::urto_ghiaccio(UnitaClassica& u, double dt) {
    // dove era e a che quota al passo prima (la prima volta: qui)
    const auto pp = passo_ghiaccio_prec_.try_emplace(u.id, std::pair<Vec2, double>{u.pos, u.quota}).first;
    const Vec2 da = pp->second.first;
    const double quota_prima = pp->second.second;
    pp->second = {u.pos, u.quota};
    if (!collisioni_ || dt <= 0.0 || copertura_ghiaccio(u.pos) <= 0.0) return;
    if (const auto f = urto_ghiaccio_fino_.find(u.id); f != urto_ghiaccio_fino_.end() && tempo_ < f->second) return;
    const double cima = cima_vela(u);
    const double pescaggio = ghiaccio_->pescaggio_max(da, u.pos, tempo_);
    if (pescaggio <= std::max(cima, 0.0)) return;
    // il ghiaccio arriva sotto la cima della vela: il nilas (5-15 cm) lo sfonda anche un Flight I (dossier 06 §1.3)
    const PuntoGhiaccio qui = ghiaccio_->in(u.pos, tempo_);
    if (qui.nilas && pescaggio < 0.2) return;
    const auto& S = *collisioni_;
    Urto x;
    x.tempo_s = tempo_;
    CorpoUrto& a = x.a;
    a.unita = u.id;
    a.classe = u.classe;
    a.sottomarino = true;
    const auto m = S.masse.find(u.classe);
    double massa = m != S.masse.end() ? m->second.second : 0.0;
    if (const auto c = S.classi.find(u.classe); massa <= 0.0 && c != S.classi.end())
        massa = 1.025 * PI * c->second.larghezza_m * c->second.larghezza_m / 4.0 * c->second.lunghezza_m * 0.75;
    a.massa_t = massa > 0.0 ? massa : 5000.0;
    a.pos = u.pos;
    a.quota_m = chiglia_vera(u);
    a.rotta_gradi = u.rotta;
    a.velocita_m_s = u.velocita * m_s_per_nodo_;
    a.rateo_verticale_m_s = (u.quota - quota_prima) / dt;
    a.periscopio_alzato = (u.flag54 & 2) != 0;
    a.alberi_alzati = (u.flag54 & 4) != 0;
    CorpoUrto& b = x.b;   // la chiglia: un ostacolo fisso con la sua massa (STIMA: pescaggio^2 x 60 m di ghiaccio poroso)
    b.massa_t = DENSITA_GHIACCIO_T_M3 * POROSITA_CHIGLIA * pescaggio * pescaggio * 60.0;
    b.pos = u.pos;
    b.quota_m = 0.0;
    x.quota_punto_m = std::max(cima, 0.0);
    // salendo sotto la volta l'urto e' dal basso (la normale in su), altrimenti di prua lungo la rotta
    const bool dal_basso = a.rateo_verticale_m_s < -0.2 && ghiaccio_->in(da, tempo_).pescaggio_m > cima;
    const double r = radianti(u.rotta);
    // il punto sulla vela (valuta_urto sceglie la parte anche dalla posizione lungo lo scafo; kit, 29/10/2026): il bordo
    // d'attacco andando avanti, il centro della vela salendo
    double lungo = 0.0;
    if (const auto c = S.classi.find(u.classe); c != S.classi.end())
        lungo = dal_basso ? 0.5 * (c->second.vela_x_m[0] + c->second.vela_x_m[1]) : c->second.vela_x_m[1];
    x.punto = {u.pos.x + lungo * std::sin(r), u.pos.y + lungo * std::cos(r)};
    x.normale[0] = dal_basso ? 0.0 : std::sin(r) * (u.velocita >= 0.0 ? 1.0 : -1.0);
    x.normale[1] = dal_basso ? 0.0 : std::cos(r) * (u.velocita >= 0.0 ? 1.0 : -1.0);
    x.normale[2] = dal_basso ? 1.0 : 0.0;
    const std::uint64_t seme = seme_ ^ (static_cast<std::uint64_t>(urti_.size() + 1) * 0x61ACC10Bull);
    EsitoUrto e = valuta_urto(x, S.classi, S.cartella_dati, seme);
    if (catalogo_) {
        const int base = URTI_DA + URTI_PASSO * collisioni_->prossimo_transitorio;
        collisioni_->prossimo_transitorio = (collisioni_->prossimo_transitorio + 1) % URTI_QUANTI;
        for (auto& t : transitori_urto(x, e, base, seme)) transitori_armi_.push_back(std::move(t));
    }
    urti_.push_back({x, e});
    urto_ghiaccio_fino_[u.id] = tempo_ + URTO_DI_NUOVO_S;
    // il battello resta sotto la chiglia e perde la velocita'
    const double sotto = pescaggio + (chiglia_vera(u) - cima) + SOTTO_CHIGLIA_M;
    if (u.quota < sotto) u.quota = sotto;
    u.velocita *= VELOCITA_DOPO_URTO;
    pp->second = {u.pos, u.quota};
}

}  // namespace m688
