#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "grafica/cartolina.hpp"
#include "missioni/danni_missione.hpp"
#include "missioni/registro_missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/geometria.hpp"
#include "nucleo/mondo.hpp"

namespace m688 {

// La figurina di ogni evento del debriefing, come in F-19 (scelta dell'utente del 07/11/2026): mai un riquadro vuoto. La
// scena, dalla fotografia se c'e' e se no dalle pose dell'evento, che restano anche dopo che l'unita' e' uscita dal gioco;
// l'icona per cio' che non si vede; quella di riserva, scelta dal tipo, se non c'e' nulla da inquadrare.
enum class Figurina { scena, icona, riserva };

inline const PosaEvento* posa_evento(const EventoMissione& e, int id) {
    for (const PosaEvento& p : e.pose)
        if (p.unita == id) return &p;
    return nullptr;
}

inline Nave nave_da_posa(const PosaEvento& p) {
    Nave n;
    n.id = p.unita;
    n.classe = p.classe;
    n.pos = n.pos_prec = p.pos;
    n.rotta = n.rotta_prec = n.rotta_ordinata = p.rotta;
    // gli aerei: nel mondo la quota e' negativa, in volo a -quota metri (STIMA: 150 m se la posa non la dice)
    n.quota = p.categoria == "aereo" ? -(p.quota > 0.0 ? p.quota : 150.0) : p.quota;
    return n;
}

// l'unita' nella fotografia: se stessa, o l'immagine che i danni disegnano al suo posto
inline const Nave* nave_in_foto(const Mondo& m, int id) {
    if (const Nave* n = m.trova(id)) return n;
    return m.trova(DanniMissione::ID_IMMAGINE + id);
}

// Cio' che nella scena non si disegna: un'arma in acqua, un contatto biologico o un rumore senza unita'
inline bool senza_sagoma(const PosaEvento& p) { return p.categoria == "nessuna" || (p.categoria == "arma" && p.quota > 0.0); }

// Chi si inquadra: il soggetto, ma un siluro in acqua non ha sagoma (la scena sarebbe solo mare), e allora chi lo riguarda, purche'
// si veda: l'esca che ha sedotto un siluro non si vede nemmeno lei
inline int soggetto_scena(const EventoMissione& e) {
    const int s = e.inquadratura.soggetto;
    const PosaEvento* p = posa_evento(e, s);
    if (!p || p->categoria != "arma" || p->quota <= 0.0) return s;
    for (const int id : {e.inquadratura.secondo, e.unita, e.altra, e.giocatore})
        if (id != s && id >= 0)
            if (const PosaEvento* altro = posa_evento(e, id); altro && !senza_sagoma(*altro)) return id;
    return s;
}

// L'altra unita' da tenere in campo, solo se e' vicina: per due unita' a miglia di distanza la camera si allontana di
// decine di chilometri e restano due puntini (STIMA: 500 m, la scena del 07/11/2026). Nel campo stretto (senza alzo) anche 400 m
// portano la camera a 9 km: la coppia ci sta da 1500 m, l'ultimo stacco, solo sotto i 70 m
inline constexpr double COPPIA_IN_CAMPO_M = 500.0, COPPIA_CAMPO_STRETTO_M = 70.0;
inline const Nave* secondo_in_campo(const Mondo& m, const Nave& soggetto, int id_soggetto, int secondo, double alzo_deg) {
    if (secondo < 0 || secondo == id_soggetto) return nullptr;
    const Nave* n = nave_in_foto(m, secondo);
    return n && distanza(n->pos, soggetto.pos) <= (alzo_deg < 1.2 ? COPPIA_CAMPO_STRETTO_M : COPPIA_IN_CAMPO_M) ? n : nullptr;
}

// Lo stacco della camera: dal registro verrebbe da lontano e dall'alto, e una Krivak a 3700 m con 5 gradi d'alzo sono 30
// pixel; a 1500 m a pelo d'acqua 2000 (STIMA: oltre 1000 m senza alzo, che apre il campo). Chi ha preso il posto di un
// siluro non eredita i suoi 200 m, dove uno scafo e' una fascia grigia: da 800 m (STIMA). L'alzo sta sotto i 5 gradi, quello che
// il registro da' di solito: nel campo largo il soggetto cade alla riga 180 + tan(alzo) x 1146, e la vista finisce a 360, quindi a
// 10 gradi (comandante_ia) restava il mare vuoto
inline constexpr double ALZO_MASSIMO_SCENA = 5.0;
struct StaccoScena {
    double distanza_m = 0.0, elevazione_deg = 0.0;
};
inline StaccoScena stacco_scena(const EventoMissione& e) {
    const Inquadratura& q = e.inquadratura;
    if (soggetto_scena(e) != q.soggetto) return {800.0, 0.0};
    double d = std::min(q.distanza_m, 1500.0);
    // nel campo stretto il ritaglio arriva a 1,2 gradi sull'orizzonte: un aereo a 20 m (mondo_scena) ci sta tutto da 1500 m
    if (const PosaEvento* p = posa_evento(e, q.soggetto); p && p->categoria == "aereo" && q.elevazione_deg < 1.2) d = 1500.0;
    return {d, d > 1000.0 ? 0.0 : std::min(q.elevazione_deg, ALZO_MASSIMO_SCENA)};
}

// L'affondamento (l'utente, 07/11/2026: una scena che lo mostri). Al momento dell'evento la fisica non ha ancora fatto quasi
// nulla (la petroliera di S5 4 s dopo: 1,4 gradi) o il battello e' a 200 m: la scena non si fotografa, si compone. Quello che
// e' a galla si vede che va giu', dal suo modello; di quello che era sotto o in volo restano i segni sul mare.
enum class ScenaAffondamento { nessuna, scafo, segni };

inline constexpr double QUOTA_A_GALLA_M = 3.0;   // oltre, dello scafo non affiora nulla (grafica/periscopio.cpp)

inline ScenaAffondamento scena_affondamento(const EventoMissione& e) {
    if (e.tipo != TipoEvento::affondamento) return ScenaAffondamento::nessuna;
    const PosaEvento* p = posa_evento(e, soggetto_scena(e));
    if (!p) return ScenaAffondamento::nessuna;
    if (p->categoria == "nave") return ScenaAffondamento::scafo;
    if (p->categoria == "sottomarino") return p->quota > QUOTA_A_GALLA_M ? ScenaAffondamento::segni : ScenaAffondamento::scafo;
    return p->categoria == "aereo" ? ScenaAffondamento::segni : ScenaAffondamento::nessuna;
}

// La lunghezza per comporre la scena, dalle classi; senza, quella del registro per la categoria
inline double lunghezza_affondamento(const std::map<std::string, ClasseNave>& classi, const EventoMissione& e) {
    const PosaEvento* p = posa_evento(e, soggetto_scena(e));
    if (p)
        if (const auto it = classi.find(p->classe); it != classi.end() && it->second.lunghezza_m > 0.0) return it->second.lunghezza_m;
    return p && p->categoria == "sottomarino" ? 100.0 : 130.0;
}

// La posa di chi va giu', sopra quella della fisica: falle, fuliggine e rottura restano le sue. Il modo viene dall'unita', sempre
// lo stesso per la stessa: di prua, di poppa o sul fianco, il verso del fianco da come sbandava. Tutti gli angoli sono STIMA, una
// scelta di aspetto: l'assetto di 16 gradi con un quinto della nave oltre il perno sott'acqua lascia fuori l'estremita' con le
// eliche, il fianco di 30 gradi mostra la coperta
inline PosaNave posa_affondamento(const EventoMissione& e, PosaNave p, double lunghezza_m) {
    constexpr double ASSETTO = 16.0, FIANCO = 30.0, QUOTA_ROTTURA_M = 6.0;
    // un battello ha poco sopra l'acqua: solo di poppa, con la prua alta (di prua affiorerebbe la sola coda, sul fianco nulla)
    const int modo = e.categoria == "sottomarino" ? 1 : (e.unita < 0 ? 0 : e.unita) % 3;
    const double verso = p.prodiero.sbandamento < 0.0 ? -1.0 : 1.0;
    p.danneggiata = true;
    if (p.spezzata) {   // due tronconi a V, che si appoggiano alla rottura
        p.prodiero.x_perno = p.poppiero.x_perno = p.x_rottura;
        p.prodiero.assetto = ASSETTO;
        p.poppiero.assetto = -ASSETTO;
        p.prodiero.immersione = p.poppiero.immersione = QUOTA_ROTTURA_M;
        return p;
    }
    p.prodiero.x_perno = 0.0;
    p.prodiero.dx = p.prodiero.dy = 0.0;
    if (modo == 2) {
        p.prodiero.sbandamento = verso * FIANCO;
        p.prodiero.assetto = -4.0;
        p.prodiero.immersione = 0.03 * lunghezza_m;
    } else {
        p.prodiero.assetto = modo == 0 ? -ASSETTO : ASSETTO;
        p.prodiero.sbandamento = verso * 6.0;
        p.prodiero.immersione = (e.categoria == "sottomarino" ? 0.05 : 0.2) * lunghezza_m * std::sin(radianti(ASSETTO));
    }
    return p;
}

// La camera: a pelo d'acqua si vedrebbe solo lo scafo, e i segni sul mare resterebbero una riga; un po' dall'alto, nel campo
// largo, con tutta la nave dentro e un quinto d'aria per il fumo (STIMA). Sopra 7 gradi il punto esce dalla vista
inline StaccoScena stacco_affondamento(ScenaAffondamento s, double lunghezza_m) {
    if (s == ScenaAffondamento::segni) return {500.0, 5.0};
    return {std::max(lunghezza_m * 3.4, 350.0), 3.0};
}

// Il fumo, il fuoco e il mare attorno: effetti di questa scena sola, dallo stesso motore; dei tempi della partita non serve nulla.
// Seme dall'evento, per avere sempre la stessa figurina. Una bolla fiacca a ogni scoppio fa la colonna, l'ultimo e' ancora acceso
// nell'istante della scena (STIMA: tutte scelte di aspetto)
inline Effetti effetti_affondamento(const EventoMissione& e, ScenaAffondamento s, Vec2 pos, double lunghezza_m, const Mondo& mondo) {
    Effetti fx(static_cast<std::uint64_t>(e.id) + 1);
    const bool sotto = s == ScenaAffondamento::segni;
    for (const Vec2 d : {Vec2{0.0, 0.0}, Vec2{25.0, 10.0}, Vec2{-25.0, -10.0}})
        fx.segno_superficie(0, {pos.x + d.x, pos.y + d.y}, 600.0, 0.5);   // la schiuma e le bolle
    fx.segno_superficie(1, pos, 60.0, 1.0);   // la nafta
    fx.segno_superficie(2, pos, 600.0, 1.0);   // i rottami
    const double bolla = std::clamp(0.3 * lunghezza_m, 8.0, 80.0);
    for (int k = 0; k < 230; ++k) {
        if (!sotto && k % 55 == 0) fx.esplosione_aerea(pos, 6.0, bolla, false);
        if (sotto && k == 170) fx.esplosione_subacquea(pos, 12.0, 80.0);   // la colonna, ancora alta nell'istante della scena
        fx.avanza(1.0 / 30.0, mondo);
    }
    return fx;
}

inline Figurina figurina_evento(const EventoMissione& e, const Mondo* foto) {
    if (!e.inquadratura.visibile) return Figurina::icona;
    const int s = soggetto_scena(e);
    if (s < 0) return Figurina::riserva;
    if (const PosaEvento* p = posa_evento(e, s); p && senza_sagoma(*p)) return Figurina::riserva;
    return (foto && nave_in_foto(*foto, s)) || posa_evento(e, s) ? Figurina::scena : Figurina::riserva;
}

// Il mondo della scena: la fotografia con il soggetto e il secondo rimessi dalle pose se non ci sono piu', o le sole pose
inline Mondo mondo_scena(const EventoMissione& e, const Mondo* foto, double lunghezza_m = 130.0) {
    Mondo m = foto ? *foto : Mondo{};
    if (foto) {
        for (const int id : {soggetto_scena(e), e.inquadratura.secondo})
            if (id >= 0 && !nave_in_foto(m, id))
                if (const PosaEvento* p = posa_evento(e, id)) m.aggiungi(nave_da_posa(*p));
    } else {
        for (const PosaEvento& p : e.pose) m.aggiungi(nave_da_posa(p));
    }
    // un battello immerso non si vedrebbe, e a meta' affiora solo la vela (a 1 m, a 400 m: nessun pixel in campo; a galla
    // 922): nella sola scena sta a galla, con tutto lo scafo. Un aereo a 150 m esce dal campo stretto: a 20 m (STIMA)
    for (const int id : {soggetto_scena(e), e.inquadratura.secondo})
        for (Nave& n : m.navi())
            if (n.id == id || n.id == DanniMissione::ID_IMMAGINE + id) n.quota = n.quota > 0.0 ? 0.0 : std::max(n.quota, -20.0);
    const ScenaAffondamento fine = scena_affondamento(e);
    const int chi = soggetto_scena(e);
    const auto e_lui = [&](const Nave& n) { return n.id == chi || n.id == DanniMissione::ID_IMMAGINE + chi; };
    if (fine == ScenaAffondamento::segni) std::erase_if(m.navi(), e_lui);
    if (fine == ScenaAffondamento::scafo)
        for (Nave& n : m.navi())
            if (e_lui(n)) n.posa = posa_affondamento(e, n.posa, lunghezza_m);
    return m;
}

// L'icona di riserva: quello che l'evento racconta, senza unita' da inquadrare (STIMA: una scelta di aspetto)
inline Icona icona_di_riserva(TipoEvento t) {
    switch (t) {
        case TipoEvento::colpo:
        case TipoEvento::affondamento:
        case TipoEvento::incaglio:
        case TipoEvento::collisione:
        case TipoEvento::collisione_ia:
        case TipoEvento::emersione_emergenza: return Icona::danni;
        case TipoEvento::scoperta:
        case TipoEvento::scoperta_ia:
        case TipoEvento::classificazione_ia:
        case TipoEvento::tma_ia: return Icona::cascata;
        case TipoEvento::comandante_ia:
        case TipoEvento::reazione_ia:
        case TipoEvento::ingaggio_ia: return Icona::telescrivente;
        case TipoEvento::foto_periscopio: return Icona::periscopio;
        default: return Icona::sonar;
    }
}

}  // namespace m688
