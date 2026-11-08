#include "ripetitori.hpp"

#include <algorithm>

#include "grafica/lamiera.hpp"
#include "grafica/palette.hpp"
#include "grafica/periscopio.hpp"
#include "grafica/tema_stazioni.hpp"

namespace m688::ripetitori {

namespace q = m688::quadro;

int largo(const Font& f, std::string_view s) {
    return f.larghezza(s, 1, f.formato() == 2 ? f.spazio_predefinito() : 1, f.formato() != 2);
}

int scrivi(Tela& t, const Font& f, int x, int y, std::string_view s, std::uint8_t c) {
    return f.scrivi(t, x, y, s, c, 1, f.formato() == 2 ? f.spazio_predefinito() : 1, f.formato() != 2);
}

std::string taglia(const Font& f, std::string s, int w) {
    while (!s.empty() && largo(f, s) > w) {   // un carattere intero alla volta (UTF-8: le continuazioni, poi l'iniziale)
        while (!s.empty() && (static_cast<unsigned char>(s.back()) & 0xC0) == 0x80) s.pop_back();
        if (!s.empty()) s.pop_back();
    }
    return s;
}

namespace {
constexpr int PASSO = 11;   // le righe degli schermi a fosforo
const char* const VISTA = "PERISCOPIO SU: LA VISTA";
}  // namespace

// la manovra: a sinistra il profilo di quota, a destra gli ordini contro lo stato e l'acqua sotto la chiglia
void manovra(Tela& t, const Fonti& f, const DatiManovra& d) {
    const Font& g = f.schermo;
    lamiera::lamiera(t, {0, 0, LARGHEZZA_TELA, ALTEZZA_VISTA}, 7, 6);
    const Rettangolo a = q::schermo(t, {4, 4, 420, ALTEZZA_VISTA - 8}, g, d.titolo, "PERISCOPIO GIU'");
    q::profilo_quota(t, a, g, d.profilo);
    const Rettangolo b = q::schermo(t, {428, 4, 208, ALTEZZA_VISTA - 8}, g, d.alfa ? "ZADANO / FAKT" : "ORDINATO / ATTUALE");
    int y = b.y + 4;
    const std::vector<int> col = {96, 152};
    y += q::intestazione(t, b, y, g, "", col, {"ORD.", "ORA"});
    int k = 0;
    for (const auto& r : d.righe) q::riga(t, b, y, g, r.nome, col, {r.ordinato, r.attuale}, k++ % 2 == 1), y += PASSO;
    y += 8;
    y += q::intestazione(t, b, y, g, "SOTTO LA CHIGLIA");
    scrivi(t, g, b.x + 4, y, taglia(g, d.chiglia, b.w - 8), d.chiglia_misurata ? fosforo(6) : fosforo(4)), y += PASSO;
    if (!d.cavitazione.empty()) scrivi(t, g, b.x + 4, y, taglia(g, d.cavitazione, b.w - 8), fosforo(5));
    scrivi(t, g, b.x + 4, b.y + b.h - 12, VISTA, fosforo(3));
}

// le armi: a sinistra le armi in acqua e quella scelta, a destra i tubi, il carico e la ricarica
void armi(Tela& t, const Fonti& f, const DatiArmiRipetitore& d) {
    const Font& g = f.schermo;
    lamiera::lamiera(t, {0, 0, LARGHEZZA_TELA, ALTEZZA_VISTA}, 7, 6);
    const Rettangolo a = q::schermo(t, {4, 4, 420, ALTEZZA_VISTA - 8}, g, d.alfa ? "ORUZHIE V VODE" : "ARMI IN ACQUA", d.fili);
    int y = a.y + 4;
    if (d.in_acqua.empty()) {
        q::stato_vuoto(t, {a.x, a.y, a.w, a.h - 16}, g, "NESSUNA ARMA IN ACQUA",
                       {"I SILURI LANCIATI COMPAIONO QUI", "CON LA CORSA, IL FILO E LO STATO DELLA TESTA"});
    } else {
        const std::vector<int> col = {20, 70, 130, 186, 244, 322};
        y += q::intestazione(t, a, y, g, "N.", col, {"ARMA", "CORSA", "FILO", "MODO", "STATO", "BERS."});
        // le righe che stanno sopra l'arma scelta (quattro righe e la sua intestazione) e la riga in fondo
        const int max_righe = std::max(1, (a.y + a.h - 12 - 8 - 5 * PASSO - 8 - y) / PASSO);
        int k = 0;
        for (const auto& r : d.in_acqua) {
            if (k >= max_righe) break;
            q::riga(t, a, y, g, r.numero, col, {taglia(g, r.arma, 48), r.corsa, r.filo, r.modo, r.stato, r.bersaglio}, k++ % 2 == 1);
            y += PASSO;
        }
        if (!d.scelta_titolo.empty()) {
            y += 8;
            y += q::intestazione(t, a, y, g, d.scelta_titolo);
            for (const auto& r : d.scelta) scrivi(t, g, a.x + 4, y, taglia(g, r, a.w - 8), fosforo(6)), y += PASSO;
        }
    }
    scrivi(t, g, a.x + 4, a.y + a.h - 12, VISTA, fosforo(3));
    const Rettangolo b = q::schermo(t, {428, 4, 208, ALTEZZA_VISTA - 8}, g, d.alfa ? "TORPEDNYE APPARATY" : "TUBI");
    y = b.y + 4;
    const std::vector<int> ct = {24, 92};
    y += q::intestazione(t, b, y, g, "T.", ct, {"ARMA", "STATO"});
    for (const auto& r : d.tubi) {
        q::riga(t, b, y, g, r.numero, ct, {taglia(g, r.arma, 66), taglia(g, r.stato, b.w - 96)}, r.scelto);
        y += PASSO;
    }
    y += 8;
    y += q::intestazione(t, b, y, g, "CARICO");
    const int fondo = b.y + b.h - 4;
    // il carico lascia posto alla ricarica (la sua intestazione e almeno una riga)
    for (std::size_t i = 0; i < d.carico.size() && y + PASSO <= fondo - 8 - 5 - g.righe() - PASSO; ++i)
        scrivi(t, g, b.x + 4, y, taglia(g, d.carico[i], b.w - 8), fosforo(6)), y += PASSO;
    y += 8;
    y += q::intestazione(t, b, y, g, "RICARICA");
    for (const auto& r : d.ricarica) {
        if (y + g.righe() > fondo) break;
        scrivi(t, g, b.x + 4, y, taglia(g, r, b.w - 8), fosforo(5)), y += PASSO;
    }
}

// la carta: la lamiera, la riga di stato incisa, lo stato in spie e vetri, il bersaglio, i comandi come tasti e la
// riga dell'equipaggio
void pannello_carta(Tela& t, const Fonti& f, const DatiPannelloCarta& d) {
    const Font& o = f.orig;
    const Rettangolo p{0, ALTEZZA_VISTA, LARGHEZZA_TELA, ALTEZZA_TELA - ALTEZZA_VISTA};
    lamiera::lamiera(t, p, 7, 8);
    const lamiera::Scritte sc = f.scritte;
    // la riga di stato: incisa in ciano sul grigio, nera e piatta sul beige
    const auto stato = [&](int x, const std::string& s) {
        if (sc == lamiera::Scritte::scure) scrivi(t, o, x, p.y + 6, s, colori::NERO);
        else lamiera::incisa(t, o, x, p.y + 6, s, colori::CIANO_CHIARO);
    };
    stato(8, d.titolo);
    stato(LARGHEZZA_TELA - 8 - largo(o, d.ora), d.ora);
    // lo stato: le spie del pilota, della proiezione e delle scie; i vetri dei punti di rotta e della scala
    const Rettangolo ps{6, p.y + 20, 300, 44};
    lamiera::pannello(t, ps, 8);
    const std::pair<std::string, bool> spie[] = {
        {"PILOTA", d.pilota}, {"PROIEZ. " + std::to_string(d.minuti) + " MIN", d.proiezione}, {"SCIE", d.scie}};
    int x = ps.x + 8;
    for (const auto& [nome, accesa] : spie) {
        lamiera::spia_voce(t, {x, ps.y + 8, 12, 9}, ColoreLampada::verde, accesa);
        lamiera::secondaria(t, o, x + 16, ps.y + 9, nome, false, sc);
        x += 26 + largo(o, nome);
    }
    lamiera::secondaria(t, o, ps.x + 8, ps.y + 26, "PUNTI DI ROTTA", false, sc);
    lamiera::vetro(t, {ps.x + 112, ps.y + 23, 44, 14});
    scrivi(t, o, ps.x + 118, ps.y + 26, std::to_string(d.punti) + "/" + std::to_string(d.punti_max), fosforo(6));
    lamiera::secondaria(t, o, ps.x + 166, ps.y + 26, "SCALA", false, sc);
    lamiera::vetro(t, {ps.x + 214, ps.y + 23, 30, 14});
    const std::string scala = std::to_string(d.scala);
    scrivi(t, o, ps.x + 214 + (30 - largo(o, scala)) / 2, ps.y + 26, scala, fosforo(6));
    // il bersaglio: il titolo della barra e, se ci stanno, i suoi dati
    const Rettangolo pb{310, p.y + 20, 324, 44};
    lamiera::pannello(t, pb, 8);
    lamiera::secondaria(t, o, pb.x + 8, pb.y + 7, "BERSAGLIO", false, sc);
    const Rettangolo vb{pb.x + 8, pb.y + 19, pb.w - 16, 17};
    lamiera::vetro(t, vb);
    if (d.bersaglio.empty()) {
        scrivi(t, o, pb.x + 14, pb.y + 24, "NESSUNO", fosforo(4));
    } else {
        const Rettangolo prima = t.ritaglio();
        t.imposta_ritaglio({vb.x + 2, vb.y, vb.w - 4, vb.h});
        const int xd = scrivi(t, o, pb.x + 14, pb.y + 24, d.bersaglio, fosforo(6));
        if (!d.bersaglio_dati.empty()) scrivi(t, o, xd + 12, pb.y + 24, d.bersaglio_dati, fosforo(5));
        t.imposta_ritaglio(prima);
    }
    // i comandi: il tasto disegnato e la parola, a capo a 632; MATE e fondale: le loro righe in un vetro
    if (!d.modo.empty()) {
        lamiera::vetro(t, {4, p.y + 68, 632, 32});
        int yr = p.y + 72;
        for (const auto& [s, c] : d.modo) scrivi(t, o, 10, yr, taglia(o, s, 620), c), yr += o.righe() + 3;
    } else {
        int xc = 8, yc = p.y + 70;
        for (const auto& [tasto, parola] : d.comandi) {
            const int w = 14 + 4 + largo(o, parola) + 12;
            if (xc + w > 632) xc = 8, yc += 18;
            lamiera::tasto_tastiera(t, {xc, yc, 14, 12}, tasto, o);
            lamiera::secondaria(t, o, xc + 18, yc + 2, parola, false, sc);
            xc += w;
        }
    }
    // la riga dell'equipaggio
    lamiera::vetro(t, {4, ALTEZZA_TELA - 18, 632, 15});
    if (!d.messaggio.empty()) {
        std::string m = d.messaggio;
        if (largo(o, m) > 620) m = taglia(o, m, 620 - largo(o, "...")) + "...";
        scrivi(t, o, 10, ALTEZZA_TELA - 14, m, fosforo(6));
    }
}

}  // namespace m688::ripetitori
