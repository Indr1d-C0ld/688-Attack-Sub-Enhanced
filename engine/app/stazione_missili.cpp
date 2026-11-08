#include "stazione_missili.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>

#include "grafica/consolle.hpp"
#include "grafica/palette.hpp"
#include "grafica/simboli_carta.hpp"
#include "nucleo/geometria.hpp"
#include "nucleo/missili.hpp"

namespace m688 {

namespace {

constexpr int N_SCALE = static_cast<int>(std::size(StazioneMissili::SCALE_KM));
constexpr int ROSSO_CHIARO = 12;   // EGA 12: i bloccanti, come i tubi armati del pannello Armi

std::vector<AreaAttiva>* aree_ = nullptr;
void area(int x, int y, int w, int h, std::string comando) {
    if (aree_) aree_->push_back({x, y, w, h, std::move(comando)});
}

// il battello come lo crede il giocatore (la carta e il piano usano la stima)
Vec2 battello(const Missione& m) {
    const auto* g = m.giocatore();
    if (!g) return {};
    return m.con_navigazione() ? m.posizione_stimata(g->id) : g->pos;
}

Vec2 lungo(Vec2 da, double rotta, double d) {
    const double a = radianti(rotta);
    return {da.x + d * std::sin(a), da.y + d * std::cos(a)};
}

bool antinave(const DatiMissile& d) { return d.ruolo == RuoloMissile::antinave; }

bool c_e(const PianoTiro& p, Avviso a) { return std::find(p.avvisi.begin(), p.avvisi.end(), a) != p.avvisi.end(); }

Ordine ordine_campo(double t, int tubo, CampoMissile c, double valore) {
    return {t, TipoOrdine::imposta_missile, valore, static_cast<double>(c), tubo};
}

const char* nome_fonte(FonteDati f) {
    switch (f) {
        case FonteDati::tma: return "TMA";
        case FonteDati::periscopio: return "PERISCOPIO";
        case FonteDati::esm: return "ESM";
        case FonteDati::radio: return "RAPPORTO RADIO";
    }
    return "?";
}

const char* nome_taglia(Taglia g) { return g == Taglia::piccola ? "PICCOLA" : g == Taglia::media ? "MEDIA" : "GRANDE"; }

std::string tempo_mm_ss(double s) {
    char r[16];
    const int x = std::max(0, static_cast<int>(std::lround(s)));
    std::snprintf(r, sizeof r, "%d:%02d", x / 60, x % 60);
    return r;
}

std::string km(double m) {
    char r[24];
    std::snprintf(r, sizeof r, m < 10000.0 ? "%.1f KM" : "%.0f KM", m / 1000.0);
    for (char* c = r; *c; ++c) if (*c == '.') *c = ',';
    return r;
}

std::string virgola_s(std::string s) {
    for (char& c : s) if (c == '.') c = ',';
    return s;
}

std::string virgola_nm(double m) {   // le miglia con un decimale
    char r[24];
    std::snprintf(r, sizeof r, "%.1f NM", m / MIGLIO);
    return virgola_s(r);
}

std::string nm(double m) {
    char r[24];
    std::snprintf(r, sizeof r, "%.0f NM", m / MIGLIO);
    return r;
}

// la distanza in uso per il bersaglio (quella dello stadimetro col periscopio), dal battello stimato
std::optional<double> distanza_bersaglio(const Missione& m, const Missione::TiroMissile& r) {
    if (!r.bersaglio || !r.bersaglio->distanza_nota) return std::nullopt;
    return distanza(battello(m), r.bersaglio->pos);
}

std::string testata(ContestoMissili::Consolle c) {
    switch (c) {
        case ContestoMissili::Consolle::mk117: return "MK 117 FIRE CONTROL";
        case ContestoMissili::Consolle::ccs_mk1: return "CCS MK 1";
        case ContestoMissili::Consolle::bsy1: return "AN/BSY-1";
    }
    return "";
}

}  // namespace

// --- righe e comandi ---

std::vector<StazioneMissili::Riga> StazioneMissili::righe(const Missione& m, int tubo) const {
    const auto r = m.tiro_missile(tubo);
    if (!r || !r->dati) return {};
    if (!antinave(*r->dati)) return {Riga::missione, Riga::preparazione};   // TLAM: la missione della libreria di bordo
    std::vector<Riga> v{Riga::fonte};
    if (r->fonte == FonteDati::periscopio) v.push_back(Riga::distanza);
    v.push_back(Riga::modo);
    if (r->imp.modo == ModoLancio::rbl) v.push_back(Riga::ricerca);
    else v.insert(v.end(), {Riga::srch, Riga::dstr});
    v.push_back(Riga::preparazione);
    return v;
}

std::optional<StazioneMissili::Riga> StazioneMissili::riga(const Missione& m, int tubo) const {
    const auto v = righe(m, tubo);
    if (v.empty()) return std::nullopt;
    return v[static_cast<std::size_t>(std::clamp(riga_, 0, static_cast<int>(v.size()) - 1))];
}

void StazioneMissili::su_giu(const Missione& m, int tubo, int verso) {
    const int n = static_cast<int>(righe(m, tubo).size());
    if (n == 0) return;
    riga_ = std::clamp(std::clamp(riga_, 0, n - 1) + (verso > 0 ? 1 : -1), 0, n - 1);
}

std::vector<Ordine> StazioneMissili::cambia(const Missione& m, int tubo, int verso, double t) {
    const auto r = m.tiro_missile(tubo);
    const auto quale = riga(m, tubo);
    if (!r || !quale || verso == 0) return {};
    const int s = verso > 0 ? 1 : -1;
    const DatiMissile& d = *r->dati;
    // SRCH e DSTR a passi di un miglio; 0 e' il valore predefinito (accensione del modello, fino alla portata)
    const auto passo_miglia = [&](double impostato, double in_uso, double massimo) {
        const double da = impostato > 0.0 ? impostato : in_uso;
        double x = (std::round(da / MIGLIO) + s) * MIGLIO;
        if (x < MIGLIO * 0.5) x = 0.0;
        if (x >= massimo) x = s > 0 && impostato <= 0.0 && in_uso >= massimo ? 0.0 : std::min(x, massimo);
        return x;
    };
    switch (*quale) {
        case Riga::fonte:
            return {ordine_campo(t, tubo, CampoMissile::fonte, static_cast<double>((static_cast<int>(r->fonte) + s + 4) % 4))};
        case Riga::distanza: {
            const double da = distanza_bersaglio(m, *r).value_or(5000.0);
            const double x = std::max(PASSO_DISTANZA_M, std::round(da / PASSO_DISTANZA_M) * PASSO_DISTANZA_M + s * PASSO_DISTANZA_M);
            return {ordine_campo(t, tubo, CampoMissile::distanza_m, x)};
        }
        case Riga::modo:
            return {ordine_campo(t, tubo, CampoMissile::modo, r->imp.modo == ModoLancio::rbl ? 1.0 : 0.0)};
        case Riga::ricerca: {
            const int g = std::clamp(static_cast<int>(r->imp.taglia) + s, 0, 2);
            if (g == static_cast<int>(r->imp.taglia)) return {};
            return {ordine_campo(t, tubo, CampoMissile::taglia, g)};
        }
        case Riga::srch: {
            const double x = passo_miglia(r->imp.srch_m, r->piano.accensione_m, d.portata_m);
            return x == r->imp.srch_m ? std::vector<Ordine>{} : std::vector<Ordine>{ordine_campo(t, tubo, CampoMissile::srch_m, x)};
        }
        case Riga::dstr: {
            const double x = passo_miglia(r->imp.dstr_m, r->piano.fine_m > 0.0 ? r->piano.fine_m : d.portata_m, d.portata_m);
            return x == r->imp.dstr_m ? std::vector<Ordine>{} : std::vector<Ordine>{ordine_campo(t, tubo, CampoMissile::dstr_m, x)};
        }
        case Riga::missione: {   // a giro sulla libreria di bordo
            const int n = static_cast<int>(m.libreria_tlam().size());
            if (n == 0) return {};
            const int i = r->missione_tlam < 0 ? (s > 0 ? 0 : n - 1) : (r->missione_tlam + s + n) % n;
            return {ordine_campo(t, tubo, CampoMissile::missione_tlam, i)};
        }
        case Riga::preparazione:
            return {{t, TipoOrdine::prepara_missile, s > 0 ? 1.0 : 0.0, 0.0, tubo}};
    }
    return {};
}

std::optional<std::string> StazioneMissili::rifiuto_lancio() const {
    if (!coperchio_) return std::string("COPERCHIO CHIUSO");
    return std::nullopt;
}

void StazioneMissili::zoom(int verso) {
    const int da = scala_ < 0 ? scala_auto_ : scala_;
    scala_ = std::clamp(da - (verso > 0 ? 1 : -1), 0, N_SCALE - 1);
}

void StazioneMissili::centra() { centro_ = centro_ == Centro::bersaglio ? Centro::lancio : Centro::bersaglio; }

void StazioneMissili::cambio_tubo() {
    riga_ = 0;
    coperchio_ = false;
    scala_ = -1;
    centro_ = Centro::automatico;
}

std::vector<Ordine> StazioneMissili::ordini_soluzione(double t, int tubo, double distanza_m, double rotta, double velocita_kn,
                                                      double sigma_distanza_m, double sigma_trasversale_m) {
    return {ordine_campo(t, tubo, CampoMissile::distanza_m, distanza_m), ordine_campo(t, tubo, CampoMissile::rotta, rotta),
            ordine_campo(t, tubo, CampoMissile::velocita_kn, velocita_kn),
            ordine_campo(t, tubo, CampoMissile::sigma_distanza_m, sigma_distanza_m),
            ordine_campo(t, tubo, CampoMissile::sigma_trasversale_m, sigma_trasversale_m)};
}

// --- la carta di tiro ---

VistaCarta StazioneMissili::vista(const Missione& m, int tubo) const {
    VistaCarta v;
    v.x = v.y = 0;
    v.w = LARGHEZZA_TELA;
    v.h = ALTEZZA_CARTA;
    const Vec2 io = battello(m);
    const auto r = m.tiro_missile(tubo);
    // i punti da inquadrare: il battello, il previsto e l'area di ricerca, il bersaglio, i missili in volo
    std::vector<Vec2> punti{io};
    std::optional<Vec2> bersaglio;
    if (r && r->bersaglio) {
        if (r->bersaglio->distanza_nota) bersaglio = posizione_a(*r->bersaglio, m.tempo());
        if (bersaglio) punti.push_back(*bersaglio);
        if (r->piano.distanza_m > 0.0 || !r->piano.area.contorno.empty()) {
            punti.push_back(r->piano.previsto);
            punti.insert(punti.end(), r->piano.area.contorno.begin(), r->piano.area.contorno.end());
            punti.insert(punti.end(), r->piano.area.percorso.begin(), r->piano.area.percorso.end());
            if (r->piano.previsto.x != 0.0 || r->piano.previsto.y != 0.0) bersaglio = bersaglio.value_or(r->piano.previsto);
        }
    }
    if (r && r->tlam) {   // TLAM: l'area di lancio col cerchio di VALIDA; il landfall e' lontano (la tratta sul mare), la sua
        const Vec2 c = r->tlam->plp;   // rotta esce dal bordo
        for (const Vec2 d : {Vec2{1, 0}, Vec2{-1, 0}, Vec2{0, 1}, Vec2{0, -1}})
            punti.push_back({c.x + d.x * TLAM_VALIDA_M, c.y + d.y * TLAM_VALIDA_M});
        bersaglio = c;
    }
    for (const auto& x : m.missili_in_volo()) punti.push_back(x.stima.pos);
    double x0 = io.x, x1 = io.x, y0 = io.y, y1 = io.y;
    for (const auto& p : punti) {
        x0 = std::min(x0, p.x), x1 = std::max(x1, p.x);
        y0 = std::min(y0, p.y), y1 = std::max(y1, p.y);
    }
    // la scala automatica: la piu' piccola delle fisse in cui tutto sta con un margine
    const double margine = 1.15;
    // senza bersaglio ne' missili in volo solo il battello: la scala di partenza (20 km), non la piu' stretta
    scala_auto_ = punti.size() == 1 ? SCALA_PARTENZA : N_SCALE - 1;
    for (int i = 0; i < N_SCALE && punti.size() > 1; ++i) {
        const double mpp = SCALE_KM[i] * 1000.0 / v.w;
        if ((x1 - x0) * margine <= v.w * mpp && (y1 - y0) * margine <= v.h * mpp) { scala_auto_ = i; break; }
    }
    const int s = scala_ < 0 ? scala_auto_ : scala_;
    v.metri_per_pixel = SCALE_KM[s] * 1000.0 / v.w;
    switch (centro_) {
        case Centro::automatico: v.centro = {(x0 + x1) / 2.0, (y0 + y1) / 2.0}; break;
        case Centro::bersaglio: v.centro = bersaglio.value_or(io); break;
        case Centro::lancio: v.centro = io; break;
    }
    return v;
}

void StazioneMissili::disegna_carta(Tela& t, const Font& f, const Missione& m, const CartaGiocatore& c,
                                    const std::vector<SegnoContatto>& segni, const ContestoMissili& k) const {
    const VistaCarta v = vista(m, k.tubo);
    ::m688::disegna_carta(t, f, m, c, segni, k.bersaglio, std::nullopt, &v);
    t.imposta_ritaglio({v.x, v.y, v.w, v.h});
    const bool vga = !c.restyle || c.stile != StileCarta::foglio;
    const auto inchiostro = static_cast<std::uint8_t>(colori_carta::MATITA + (vga ? 5 : 0));   // bianco sulla VGA, nero sul foglio
    const auto giallo = static_cast<std::uint8_t>(colori_carta::MATITA + 4);
    const auto rosso = static_cast<std::uint8_t>(colori_carta::MATITA + 1);
    const auto area_col = static_cast<std::uint8_t>(colori_carta::MAGENTA);
    const auto q = [&](Vec2 p) { return v.a_tela(p); };
    const double mpp = v.metri_per_pixel;
    const Vec2 io = battello(m);
    const auto r = m.tiro_missile(k.tubo);
    simboli::Pennello pn;
    if (r && r->dati) {
        const PianoTiro& p = r->piano;
        const bool con_piano = p.distanza_m > 0.0 || !p.area.contorno.empty();
        // il datum del lancio: dove arrivera' l'ASW
        simboli::disegna_ellisse(pn, q(io), DATUM_M / mpp, DATUM_M / mpp, 0.0, simboli::VERDE_PROPRIO_SCURO);
        // la stima del bersaglio: l'ellisse al 95% che si gonfia dall'ora della posizione, o il solo rilevamento
        if (r->bersaglio) {
            const BersaglioStimato& b = *r->bersaglio;
            const auto col = static_cast<std::uint8_t>(b.fonte == FonteDati::radio ? colori::CIANO_CHIARO : inchiostro);
            if (b.distanza_nota) {
                const EllisseErrore e = ellisse_a(b, m.tempo());
                simboli::disegna_ellisse(pn, q(posizione_a(b, m.tempo())), std::max(2.0, simboli::K_ELLISSE_95 * e.maggiore_m / mpp),
                                         std::max(2.0, simboli::K_ELLISSE_95 * e.minore_m / mpp), e.orientamento_deg, col);
            } else {
                simboli::disegna_rilevamento(pn, q(io), q(lungo(io, b.rilevamento, r->dati->portata_m)), col);
            }
        }
        if (con_piano) {
            // la rotta del missile, fino al previsto (RBL) o alla fine della ricerca (BOL); il TASM poi la serpentina
            const Vec2 fine = p.area.forma == AreaRicerca::Forma::serpentina && !p.area.percorso.empty() ? p.area.percorso.front()
                              : r->imp.modo == ModoLancio::rbl ? p.previsto : lungo(p.lancio, p.rotta, p.fine_m);
            pn.linea(q(p.lancio), q(fine), inchiostro);
            for (std::size_t i = 1; i < p.area.percorso.size(); ++i) pn.linea(q(p.area.percorso[i - 1]), q(p.area.percorso[i]), area_col, 3);
            // l'area di ricerca
            const auto& a = p.area.contorno;
            for (std::size_t i = 0; i < a.size(); ++i) pn.linea(q(a[i]), q(a[(i + 1) % a.size()]), area_col);
            // il punto previsto col vettore dalla stima, e l'ellisse all'arrivo (RBL)
            if (r->imp.modo == ModoLancio::rbl) {
                if (r->bersaglio && r->bersaglio->distanza_nota) pn.linea(q(posizione_a(*r->bersaglio, m.tempo())), q(p.previsto), giallo, 2);
                simboli::disegna_croce_tma(pn, q(p.previsto), giallo);
            }
            // l'accensione del seeker
            const Vec2 a_on = q(p.accensione);
            pn.rettangolo(static_cast<int>(a_on.x) - 1, static_cast<int>(a_on.y) - 1, 3, 3, giallo);
            // l'orizzonte radar del bersaglio: dove il missile in crociera si fa vedere
            if (antinave(*r->dati) && p.orizzonte_radar_m > 0.0)
                simboli::disegna_ellisse(pn, q(p.previsto), p.orizzonte_radar_m / mpp, p.orizzonte_radar_m / mpp, 0.0, rosso);
        }
    }
    std::vector<std::pair<Vec2, std::string>> scritte;
    // TLAM (§4, area di lancio): il PLP coi cerchi di VALIDA (10 nm) e RIPIANIFICA (40 nm), la rotta d'uscita fino al
    // landfall e la prima mappa TERCOM (il quadrato di mezzo lato H)
    if (r && r->tlam) {
        const MissioneTlam& mt = *r->tlam;
        const Vec2 plp = q(mt.plp), lf = q(mt.landfall);
        simboli::disegna_ellisse(pn, plp, TLAM_VALIDA_M / mpp, TLAM_VALIDA_M / mpp, 0.0, static_cast<std::uint8_t>(colori::VERDE_CHIARO));
        simboli::disegna_ellisse(pn, plp, TLAM_RIPIANIFICA_M / mpp, TLAM_RIPIANIFICA_M / mpp, 0.0, giallo);
        simboli::disegna_croce_tma(pn, plp, inchiostro);
        pn.linea(plp, lf, inchiostro, 3);
        const int h = std::max(2, static_cast<int>(mt.mezza_mappa_m / mpp));
        pn.rettangolo(static_cast<int>(lf.x) - h, static_cast<int>(lf.y) - h, 2 * h + 1, 1, giallo);
        pn.rettangolo(static_cast<int>(lf.x) - h, static_cast<int>(lf.y) + h, 2 * h + 1, 1, giallo);
        pn.rettangolo(static_cast<int>(lf.x) - h, static_cast<int>(lf.y) - h, 1, 2 * h + 1, giallo);
        pn.rettangolo(static_cast<int>(lf.x) + h, static_cast<int>(lf.y) - h, 1, 2 * h + 1, giallo);
        scritte.push_back({{plp.x + 6.0, plp.y + 4.0}, "PLP " + mt.id});
        scritte.push_back({{lf.x + h + 3.0, lf.y - 4.0}, "LANDFALL"});
    }
    // i missili in volo: solo la stima dal tempo di volo, tratteggiata e marcata STIMA (decisione 4 del §10)
    for (const auto& x : m.missili_in_volo()) {
        const Vec2 a = q(x.stima.pos);
        pn.linea(q(x.piano.lancio), a, inchiostro, 2);
        simboli::disegna_oggetto(pn, a, simboli::Oggetto::missile, inchiostro, x.piano.rotta);
        scritte.push_back({{a.x + 6.0, a.y - 3.0}, "STIMA"});
    }
    pn.stendi(t);
    // i contatti nell'area di ricerca, nell'ordine in cui il seeker li vedrebbe
    if (r && !r->piano.in_area.empty()) {
        const auto sulla_carta = segni_su_carta(v, m, segni);
        for (std::size_t i = 0; i < r->piano.in_area.size(); ++i)
            for (const auto& s : sulla_carta)
                if (s.segno.id == r->piano.in_area[i]) {
                    simboli::Pennello cornice;
                    cornice.rettangolo(static_cast<int>(s.q.x) - 8, static_cast<int>(s.q.y) - 8, 17, 1, simboli::EVIDENZA);
                    cornice.rettangolo(static_cast<int>(s.q.x) - 8, static_cast<int>(s.q.y) + 8, 17, 1, simboli::EVIDENZA);
                    cornice.rettangolo(static_cast<int>(s.q.x) - 8, static_cast<int>(s.q.y) - 8, 1, 17, simboli::EVIDENZA);
                    cornice.rettangolo(static_cast<int>(s.q.x) + 8, static_cast<int>(s.q.y) - 8, 1, 17, simboli::EVIDENZA);
                    cornice.stendi(t);
                    // il numero d'ordine, con la N dei neutrali creduti (il rischio della scatola: docs/21 §3.2)
                    const auto& n = r->piano.neutrali_in_area;
                    const bool neutrale = std::find(n.begin(), n.end(), s.segno.id) != n.end();
                    scritte.push_back({{s.q.x + 10.0, s.q.y - 8.0}, std::to_string(i + 1) + (neutrale ? " N" : "")});
                }
    }
    for (const auto& [a, s] : scritte)
        simboli::scrivi(t, f, static_cast<int>(a.x), static_cast<int>(a.y), s,
                        s.size() > 2 && s.substr(s.size() - 2) == " N" ? simboli::indice_parte(simboli::Parte::neutrale) : inchiostro, 1);
    // la scala in basso a destra
    char riga[32];
    std::snprintf(riga, sizeof riga, "%s%.0f KM", scala_ < 0 ? "AUTO " : "", SCALE_KM[scala_ < 0 ? scala_auto_ : scala_]);
    simboli::scrivi(t, f, v.x + v.w - 8 - f.larghezza(riga, 1), v.y + v.h - 12, riga, inchiostro, 1);
    t.togli_ritaglio();
}

// --- il pannello ---

void StazioneMissili::disegna_pannello(Tela& t, const Font& f, const Missione& m, const ContestoMissili& k,
                                       std::vector<AreaAttiva>* aree) const {
    aree_ = aree;
    const int y0 = ALTEZZA_VISTA;
    t.rettangolo_pieno(0, y0, LARGHEZZA_TELA, ALTEZZA_TELA - y0, colori::GRIGIO_SCURO);
    t.linea_orizzontale(0, LARGHEZZA_TELA - 1, y0, colori::GRIGIO);
    char buf[200];
    const auto r = m.tiro_missile(k.tubo);
    std::snprintf(buf, sizeof buf, "ARMI - MISSILI  TUBO %d  %s", k.tubo, r && r->dati ? r->dati->nome.c_str() : "");
    f.scrivi(t, 12, y0 + 4, buf, colori::CIANO_CHIARO, 1);
    const int ore = static_cast<int>(k.ora_s / 3600.0) % 24, minuti = static_cast<int>(k.ora_s / 60.0) % 60,
              secondi = static_cast<int>(k.ora_s) % 60;
    std::snprintf(buf, sizeof buf, "ora %02d:%02d:%02dZ  tempo x%g", ore, minuti, secondi, k.compressione);
    f.scrivi(t, LARGHEZZA_TELA - 12 - f.larghezza(buf, 1), y0 + 4, buf, colori::CIANO_CHIARO, 1);

    // la consolle di tiro, a sinistra: testo al fosforo su vetro nero, una riga per parametro
    const Rettangolo vetro{8, y0 + 14, 404, 92};
    t.rettangolo_pieno(vetro.x, vetro.y, vetro.w, vetro.h, colori::NERO);
    t.imposta_ritaglio(vetro);
    constexpr int PASSO = 9;   // dieci righe nel vetro: la testata, otto parametri, gli avvisi
    int y = vetro.y + 1;
    const auto testo = [&](int x, const std::string& s, int colore) { return f.scrivi(t, x, y, s, static_cast<std::uint8_t>(colore), 1); };
    f.scrivi(t, vetro.x + 4, y, testata(k.consolle), colori::VERDE, 1);
    y += PASSO;
    if (!r || !r->dati) {
        testo(vetro.x + 4, "NESSUN MISSILE NEL TUBO", colori::VERDE_CHIARO);
    } else {
        const DatiMissile& d = *r->dati;
        const PianoTiro& p = r->piano;
        const auto scelte = righe(m, k.tubo);
        const auto attuale = riga(m, k.tubo);
        // una riga che si sceglie: il cursore, l'area per il clic
        const auto riga_scelta = [&](Riga q, const std::string& nome, const std::string& valore) {
            const auto it = std::find(scelte.begin(), scelte.end(), q);
            const bool su = attuale && *attuale == q;
            if (su) t.rettangolo_pieno(vetro.x + 2, y - 1, vetro.w - 4, PASSO, colori::VERDE);
            const int c = su ? colori::NERO : colori::VERDE_CHIARO;
            testo(vetro.x + 14, nome, c);
            testo(vetro.x + 110, valore, c);
            if (it != scelte.end()) area(vetro.x, y - 1, vetro.w, PASSO, "missili_riga_" + std::to_string(it - scelte.begin()));
            y += PASSO;
        };
        const auto riga_letta = [&](const std::string& nome, const std::string& valore, int colore = colori::VERDE_CHIARO) {
            testo(vetro.x + 14, nome, colori::VERDE);
            testo(vetro.x + 110, valore, colore);
            y += PASSO;
        };
        if (antinave(d)) {
            std::string b = k.bersaglio < 0 ? "NESSUNO" : "N. " + std::to_string(k.bersaglio);
            if (r->bersaglio) {   // rilevamento, distanza, rotta e velocita' della fonte
                const double ril = r->bersaglio->distanza_nota ? rilevamento(battello(m), r->bersaglio->pos) : r->bersaglio->rilevamento;
                std::snprintf(buf, sizeof buf, "  %03d°", static_cast<int>(std::lround(normalizza_360(ril))) % 360);
                b += buf;
                if (const auto dist = distanza_bersaglio(m, *r)) b += "  " + km(*dist);
                if (r->bersaglio->moto_noto) {
                    std::snprintf(buf, sizeof buf, "  %03d°/%.0f KN", static_cast<int>(std::lround(normalizza_360(r->bersaglio->rotta))) % 360,
                                  r->bersaglio->velocita_m_s / NODO);
                    b += buf;
                }
            }
            riga_letta("BERSAGLIO", b);
            riga_scelta(Riga::fonte, "FONTE", nome_fonte(r->fonte));
            if (r->fonte == FonteDati::periscopio) {
                const auto dist = distanza_bersaglio(m, *r);
                std::snprintf(buf, sizeof buf, dist ? "%.0f M" : "MANCA LO STADIMETRO", dist.value_or(0.0));
                riga_scelta(Riga::distanza, "DISTANZA", buf);
            }
            riga_scelta(Riga::modo, "MODO", r->imp.modo == ModoLancio::rbl ? "RBL" : "BOL");
            if (r->imp.modo == ModoLancio::rbl) {
                riga_scelta(Riga::ricerca, "RICERCA", nome_taglia(r->imp.taglia));
            } else {
                riga_scelta(Riga::srch, "SRCH", (r->imp.srch_m > 0.0 ? nm(r->imp.srch_m) : "PREDEF.") + (p.accensione_m > 0.0 ? "  (" + km(p.accensione_m) + ")" : ""));
                riga_scelta(Riga::dstr, "DSTR", r->imp.dstr_m > 0.0 ? nm(r->imp.dstr_m) : "FINO ALLA PORTATA");
            }
            std::string volo = d.terminale == Terminale::pop_up ? "POP-UP" : "RADENTE";
            if (p.tempo_volo_s > 0.0) volo += "  VOLO " + tempo_mm_ss(p.tempo_volo_s) + "  " + km(p.distanza_m);
            riga_letta("TERMINALE", volo);
            if (r->bersaglio && (p.distanza_m > 0.0 || !p.area.contorno.empty())) {
                std::snprintf(buf, sizeof buf, "%.2f  (AREA %.2f  PRIMO %.2f)", p.p_successo, p.p_area, p.p_primo);
                for (char* c = buf; *c; ++c) if (*c == '.') *c = ',';
                riga_letta("P SUCCESSO", buf, p.p_successo >= 0.4 ? colori::VERDE_CHIARO : colori::GIALLO);
            } else {
                riga_letta("P SUCCESSO", "-");
            }
            // i controlli: verde se va, ambra se e' un avviso, rosso se blocca
            testo(vetro.x + 14, "CONTROLLI", colori::VERDE);
            int x = vetro.x + 110;
            const auto controllo = [&](const char* nome, bool rosso_, bool ambra) {
                x = testo(x, nome, rosso_ ? ROSSO_CHIARO : ambra ? colori::GIALLO : colori::VERDE_CHIARO) + 8;
            };
            controllo("QUOTA", c_e(p, Avviso::quota), c_e(p, Avviso::quota_bassa));
            controllo("VEL", false, c_e(p, Avviso::velocita));
            controllo("MARE", false, c_e(p, Avviso::mare));
            controllo("DIST", c_e(p, Avviso::fuori_portata) || c_e(p, Avviso::troppo_vicino), false);
            y += PASSO;
        } else {   // TLAM (§3.4): la missione della libreria, lo stato sulla posizione stimata, l'ordine d'esecuzione
            const auto ora_utc = [&](double t_missione) {   // l'ora del giorno di un istante della missione
                const double x = std::fmod(k.ora_s + (t_missione - m.tempo()) + 86400.0 * 4, 86400.0);
                std::snprintf(buf, sizeof buf, "%02d:%02dZ", static_cast<int>(x / 3600.0), static_cast<int>(std::fmod(x, 3600.0) / 60.0));
                return std::string(buf);
            };
            std::string mis = m.libreria_tlam().empty() ? "LIBRERIA VUOTA" : "NESSUNA   (DESTRA SCEGLIE)";
            if (r->tlam) {
                std::snprintf(buf, sizeof buf, "%s  TOT %s +-%.0f MIN", r->tlam->id.c_str(), ora_utc(r->tlam->tot_s).c_str(), r->tlam->finestra_s / 60.0);
                mis = buf;
            }
            riga_scelta(Riga::missione, "MISSIONE", mis);
            if (r->tlam && r->stato_tlam) {
                const StatoTlam& st = *r->stato_tlam;
                const int col = st.validita == ValiditaTlam::valida ? colori::VERDE_CHIARO
                                : st.validita == ValiditaTlam::ripianifica ? colori::GIALLO : ROSSO_CHIARO;
                std::string stato = std::string(nome_validita(st.validita)) + "  PLP " + virgola_nm(st.distanza_plp_m);
                if (st.validita == ValiditaTlam::ripianifica) stato += "  +" + tempo_mm_ss(st.ripianifica_s);
                riga_letta("STATO", stato, col);
                std::snprintf(buf, sizeof buf, "P TERCOM %.2f", st.p_landfall);
                riga_letta("NAVIGAZIONE", virgola_s(buf), st.p_landfall >= 0.8 ? colori::VERDE_CHIARO : colori::GIALLO);
                riga_letta("LANCIO", ora_utc(st.ora_lancio_s) + "  VOLO " + tempo_mm_ss(st.tempo_volo_s));
            } else {
                riga_letta("STATO", "-");
            }
            // l'ordine: ricevuto, oppure arrivato solo con l'autenticazione che non torna (I12: non arma la missione)
            riga_letta("ORDINE",
                       r->ordine_esecuzione      ? "ESECUZIONE RICEVUTA"
                       : r->ordine_non_autentico ? "NON AUTENTICATO: NON ESEGUIRE"
                                                 : "IN ATTESA DELL'ORDINE",
                       r->ordine_esecuzione ? colori::VERDE_CHIARO : r->ordine_non_autentico ? ROSSO_CHIARO : colori::GIALLO);
            // i controlli del lancio: la quota (rosso oltre la massima) e la velocita' (ambra oltre la massima)
            const auto* g = m.giocatore();
            const bool profondo = g && g->quota > d.quota_lancio_max_m, veloce = g && std::fabs(g->velocita) > d.velocita_lancio_max_kn;
            testo(vetro.x + 14, "CONTROLLI", colori::VERDE);
            int x = vetro.x + 110;
            x = testo(x, "QUOTA", profondo ? ROSSO_CHIARO : colori::VERDE_CHIARO) + 8;
            testo(x, "VEL", veloce ? colori::GIALLO : colori::VERDE_CHIARO);
            y += PASSO;
        }
        std::string prep = r->fase == FasePreparazione::pronto ? "PRONTO" : r->fase == FasePreparazione::spento ? "SPENTO   (DESTRA AVVIA)" : "";
        if (prep.empty()) prep = std::string(r->fase == FasePreparazione::accensione ? "ACCENSIONE " : "ALLINEAMENTO ") + tempo_mm_ss(r->residuo_s);
        riga_scelta(Riga::preparazione, "PREPARAZIONE", prep);
        // gli avvisi del piano, i bloccanti per primi, in fondo al vetro
        std::string avvisi;
        int col = colori::VERDE_CHIARO;
        for (const Avviso a : p.avvisi) {
            if (!avvisi.empty()) avvisi += "  ";
            avvisi += nome_avviso(a);
            col = bloccante(p.avvisi.front()) ? ROSSO_CHIARO : colori::GIALLO;
        }
        if (avvisi.empty()) avvisi = p.valido ? "DATI DI TIRO VALIDI" : "";
        while (f.larghezza(avvisi, 1) > vetro.w - 8 && avvisi.size() > 3) avvisi.pop_back();
        y = vetro.y + vetro.h - PASSO;
        testo(vetro.x + 4, avvisi, col);
    }
    t.togli_ritaglio();

    // la Weapons Launch Console, a destra: per tubo con un missile le spie (al piu' cinque righe), a lato il FIRE col
    // coperchio
    const int wx = 420;
    f.scrivi(t, wx, y0 + 15, "WEAPONS LAUNCH CONSOLE", colori::GRIGIO, 1);
    static constexpr const char* SPIE[] = {"PWR", "ALN", "RDY", "DOOR", "SLIDE"};
    for (int i = 0; i < 5; ++i)   // le maiuscole piccole: le scritte stanno sopra le lampade
        f.scrivi(t, WLC_X + i * WLC_PASSO + WLC_W / 2 - f.larghezza(SPIE[i], 1, 1, false) / 2, y0 + 26, SPIE[i], colori::GRIGIO, 1, 1, false);
    int n = 0;
    for (const auto& tb : k.tubi) {
        const auto x = m.tiro_missile(tb.n);
        if (!x || n == 5) continue;
        const int ty = y0 + WLC_Y + n++ * WLC_RIGA;
        std::snprintf(buf, sizeof buf, "T%d", tb.n);
        f.scrivi(t, wx, ty, buf, tb.n == k.tubo ? colori::BIANCO : colori::GRIGIO, 1);
        const bool accese[] = {x->pwr, x->aln, x->rdy, tb.porta, x->slide_libera};
        for (int i = 0; i < 5; ++i)
            lampada_rettangolare(t, {WLC_X + i * WLC_PASSO, ty, WLC_W, WLC_H}, i == 3 ? ColoreLampada::ambra : ColoreLampada::verde, accese[i]);
    }
    const Rettangolo fuoco{LARGHEZZA_TELA - 54, y0 + 40, 40, 40};
    const bool pronto = r && r->rdy && r->slide_libera;
    f.scrivi(t, fuoco.x + fuoco.w / 2 - f.larghezza("FIRE", 1) / 2, y0 + 28, "FIRE", pronto ? colori::BIANCO : colori::GRIGIO, 1);
    pulsante_coperto(t, fuoco, coperchio_, false);
    const char* stato_coperchio = coperchio_ ? "ALZATO" : "COPERTO";
    f.scrivi(t, fuoco.x + fuoco.w / 2 - f.larghezza(stato_coperchio, 1) / 2, fuoco.y + fuoco.h + 3, stato_coperchio,
             coperchio_ ? colori::GIALLO : colori::GRIGIO, 1);
    area(fuoco.x, fuoco.y, fuoco.w, fuoco.h, "missili_fuoco");

    // ultima riga: il messaggio di bordo, o l'aiuto dei tasti
    if (k.messaggio.empty())
        f.scrivi(t, 12, y0 + 108, "FRECCE RIGA E VALORE   MAIUSC+L COPERCHIO  L LANCIA   I O SCALA  C CENTRO  G BERSAGLIO  TAB SILURI",
                 colori::GRIGIO, 1);
    else
        f.scrivi(t, 12, y0 + 108, k.messaggio, colori::VERDE_CHIARO, 1);
    aree_ = nullptr;
}

}  // namespace m688
