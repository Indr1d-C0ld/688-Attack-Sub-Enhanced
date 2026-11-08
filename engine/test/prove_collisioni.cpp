// Danni da collisione (nucleo/collisioni.hpp; dossier docs/ricerca/16_collisioni.md, citato «16»): le energie del §2.1 e
// i casi storici del §1 e del §4.7, ciascuno con la tolleranza scritta accanto. Masse e forme: le classi del gioco;
// dove la classe vera manca (Delta, Sierra, Vanguard, Ehime Maru, Ticonderoga) la forma di una classe vicina e la massa
// vera nell'Urto.
#include <doctest.h>

#include <cmath>
#include <filesystem>
#include <string>

#include "nucleo/battello.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/collisioni.hpp"
#include "nucleo/danni.hpp"
#include "nucleo/scafo.hpp"

using namespace m688;
namespace fs = std::filesystem;

namespace {


bool ci_sono_dati() { return fs::exists(std::string(M688_DATI) + "/classi.toml"); }

const std::map<std::string, ClasseNave>& classi() {
    static const auto c = carica_classi(std::string(M688_DATI) + "/classi.toml");
    return c;
}

CorpoUrto corpo(const std::string& classe, double massa_t, bool sottomarino, double quota, double rotta, double v,
                double rateo = 0.0) {
    CorpoUrto c;
    c.unita = sottomarino ? 2 : 1;
    c.classe = classe;
    c.sottomarino = sottomarino;
    c.massa_t = massa_t;
    c.quota_m = quota;
    c.rotta_gradi = rotta;
    c.velocita_m_s = v;
    c.rateo_verticale_m_s = rateo;
    return c;
}

// punto del mondo dalle coordinate di uno scafo (x a prua, y a sinistra)
Vec2 nel_mondo(const CorpoUrto& c, double x, double y) {
    const double r = radianti(c.rotta_gradi);
    return {c.pos.x + x * std::sin(r) - y * std::cos(r), c.pos.y + x * std::cos(r) + y * std::sin(r)};
}

// mette `c` in modo che il suo punto (x, y) cada su `p`
void posa(CorpoUrto& c, Vec2 p, double x, double y = 0.0) {
    c.pos = {0, 0};
    const Vec2 q = nel_mondo(c, x, y);
    c.pos = {p.x - q.x, p.y - q.y};
}

Urto urto(CorpoUrto a, CorpoUrto b, Vec2 punto, double quota, double nx, double ny, double nz) {
    Urto u;
    u.a = std::move(a);
    u.b = std::move(b);
    u.punto = punto;
    u.quota_punto_m = quota;
    const double n = std::sqrt(nx * nx + ny * ny + nz * nz);
    u.normale[0] = nx / n;
    u.normale[1] = ny / n;
    u.normale[2] = nz / n;
    return u;
}

double perdita_di(const DanniUrtoUnita& d, Sistema s) {
    double p = 0.0;
    for (const auto& [k, v] : d.sistemi)
        if (k == s) p = std::max(p, v);
    return p;
}

const double MEZZO_688 = 110.3 / 2;

// 688 di prua (rotta `rotta`) sul fianco sinistro di un Victor III fermo diretto a nord, entrambi con la chiglia a 100 m
EsitoUrto prua_su_victor(double nodi, double rotta) {
    CorpoUrto b = corpo("victor3", 7250, true, 100, 0, 0);
    const double meta_b = classi().at("victor3").larghezza_m / 2;
    const Vec2 p = nel_mondo(b, 0, meta_b);
    CorpoUrto a = corpo("los_angeles", 6927, true, 100, rotta, nodi * NODO);
    posa(a, p, MEZZO_688);
    return valuta_urto(urto(a, b, p, 95, 1, 0, 0), classi(), M688_DATI, 7);
}

}  // namespace

TEST_CASE("collisioni: energie del dossier 16 §2.1") {
    if (!ci_sono_dati()) return;
    // 688 (6.927 t) di prua sul fianco di un Victor III (7.250 t): 2,5 / 16 / 62 / 250 MJ a 2 / 5 / 10 / 20 kn, +-10%
    const double attese[4][2] = {{2, 2.5}, {5, 16}, {10, 62}, {20, 250}};
    for (const auto& [nodi, mj] : attese) {
        const EsitoUrto e = prua_su_victor(nodi, 90);
        CHECK(e.energia_normale_mj == doctest::Approx(mj).epsilon(0.10));
        CHECK(e.a.parte == ParteUrto::duomo);
        CHECK(e.b.parte == ParteUrto::fianco);
        CHECK_FALSE(e.radente);
    }
    // radente a 20 gradi: il dossier da' 1,8 / 7,3 MJ con la massa ridotta dell'urto di punta; con la massa aggiunta
    // nella direzione vera (la normale e' quasi di traverso per l'urtante) viene di piu': fra 1,8 e 2,8 / 7,3 e 11 MJ
    const EsitoUrto r5 = prua_su_victor(5, 20), r10 = prua_su_victor(10, 20);
    CHECK(r5.energia_normale_mj >= 1.8);
    CHECK(r5.energia_normale_mj <= 2.8);
    CHECK(r10.energia_normale_mj >= 7.3);
    CHECK(r10.energia_normale_mj <= 11.0);
    CHECK(r5.radente);
    CHECK(r5.energia_tangente_mj > r5.energia_normale_mj);   // il grosso e' strisciamento: graffi e piastrelle
}

TEST_CASE("collisioni: Grayling e Delta IV (1993), radente di prua") {
    if (!ci_sono_dati()) return;
    // Grayling (Sturgeon, 4.800 t) a 12 kn verso nord, il Delta IV (forma dell'Oscar, 18.200 t) a 16 kn verso sud:
    // si toccano le prue a 10 gradi. Atteso (16 §4.7): graffi, duomo del piu' piccolo, nessuna falla nello scafo
    // resistente, ammaccatura < 0,1
    CorpoUrto a = corpo("sturgeon", 4800, true, 80, 0, 12 * NODO), b = corpo("oscar", 18200, true, 80, 180, 16 * NODO);
    const Vec2 p{5, 0};
    posa(a, p, classi().at("sturgeon").lunghezza_m / 2);
    posa(b, p, classi().at("oscar").lunghezza_m / 2);
    const EsitoUrto e = valuta_urto(urto(a, b, p, 75, std::cos(radianti(10)), std::sin(radianti(10)), 0), classi(), M688_DATI, 3);
    CHECK(e.radente);
    CHECK(e.a.parte == ParteUrto::duomo);
    CHECK(perdita_di(e.a, Sistema::sonar_prua) > 0.5);
    CHECK(e.a.falla_resistente_m2 == 0.0);
    CHECK(e.b.falla_resistente_m2 == 0.0);
    CHECK(e.a.ammaccatura < 0.1);
    CHECK(e.b.ammaccatura < 0.1);
    CHECK(e.a.piastrelle_m2 > 10.0);   // il grande graffio
}

TEST_CASE("collisioni: Tautog e K-108 (1970), il Victor che scende sulla vela") {
    if (!ci_sono_dati()) return;
    // il 688 lento a 120 m di chiglia; il Victor scende a 2 m/s e lo tocca con l'elica sulla cima della vela.
    // Atteso: vela del 688 piegata, periscopio e antenna persi (>= 0,5); elica del Victor (velocita' massima <= 0,8,
    // rumore >= +3 dB); nessuna falla nello scafo resistente
    const auto& la = classi().at("los_angeles");
    CorpoUrto b = corpo("los_angeles", 6927, true, 120, 0, 1.5);
    const double cima = la.larghezza_m + la.altezza_vela_m;
    const Vec2 p = nel_mondo(b, 23, 0);
    CorpoUrto a = corpo("victor3", 7250, true, 120 - cima, 0, 1.5, 2.0);
    posa(a, p, -classi().at("victor3").lunghezza_m / 2 + 1.0);
    const EsitoUrto e = valuta_urto(urto(a, b, p, 120 - cima, 0, 0, -1), classi(), M688_DATI, 5);
    CHECK(e.v_n_m_s == doctest::Approx(2.0));
    CHECK(e.b.parte == ParteUrto::vela);
    CHECK(e.a.parte == ParteUrto::eliche);
    CHECK(perdita_di(e.b, Sistema::periscopio) >= 0.5);
    CHECK(perdita_di(e.b, Sistema::antenna) >= 0.5);
    CHECK(e.a.velocita_max <= 0.8);
    CHECK(e.a.rumore_db >= 3.0);
    CHECK(e.a.falla_resistente_m2 == 0.0);
    CHECK(e.b.falla_resistente_m2 == 0.0);
}

TEST_CASE("collisioni: Baton Rouge e Kostroma (1992), il Sierra che risale sotto la poppa") {
    if (!ci_sono_dati()) return;
    // il 688 con la chiglia a 30 m; il Sierra (forma del Victor III, 8.000 t) risale a 1,5 m/s e 7 kn e lo prende con
    // la vela sotto la cassa zavorra poppiera di sinistra. Atteso: cassa forata 0,5-3 m^2, piastrelle, niente falla
    // nello scafo resistente; la vela del Sierra ammaccata
    const auto& vi = classi().at("victor3");
    CorpoUrto b = corpo("los_angeles", 6927, true, 30, 0, 0);
    const Vec2 p = nel_mondo(b, -35, 3);
    CorpoUrto a = corpo("victor3", 8000, true, 30 + vi.larghezza_m + vi.altezza_vela_m, 0, 7 * NODO, -1.5);
    posa(a, p, 10);
    const EsitoUrto e = valuta_urto(urto(a, b, p, 30, 0, 0, 1), classi(), M688_DATI, 11);
    CHECK(e.a.parte == ParteUrto::vela);
    CHECK(e.b.parte == ParteUrto::chiglia);
    CHECK(e.b.falla_cassa_m2 >= 0.5);
    CHECK(e.b.falla_cassa_m2 <= 3.0);
    CHECK(e.b.piastrelle_m2 > 0.0);
    CHECK(e.b.falla_resistente_m2 == 0.0);
    CHECK(e.a.energia_mj > 1.0);
    CHECK(perdita_di(e.a, Sistema::periscopio) > 0.0);
}

TEST_CASE("collisioni: Vanguard e Triomphant (2009), due SSBN lenti e obliqui") {
    if (!ci_sono_dati()) return;
    // il Triomphant (forma dell'Oscar, 14.300 t) a 4 kn verso nord-ovest prende di prua il fianco dritto del Vanguard
    // (15.900 t) a 3 kn verso nord. Atteso: duomo dell'urtante (sonar di prua >= 0,5), casing dell'urtato; niente falle
    // nello scafo resistente
    const auto& os = classi().at("oscar");
    CorpoUrto b = corpo("oscar", 15900, true, 150, 0, 1.5);
    const Vec2 p = nel_mondo(b, 0, -os.larghezza_m / 2);
    CorpoUrto a = corpo("oscar", 14300, true, 150, 315, 2.0);
    posa(a, p, os.lunghezza_m / 2);
    const EsitoUrto e = valuta_urto(urto(a, b, p, 150 - os.larghezza_m / 2, -1, 0, 0), classi(), M688_DATI, 13);
    CHECK(e.a.parte == ParteUrto::duomo);
    CHECK(e.b.parte == ParteUrto::fianco);
    CHECK(perdita_di(e.a, Sistema::sonar_prua) >= 0.5);
    CHECK(e.a.falla_resistente_m2 == 0.0);
    CHECK(e.b.falla_resistente_m2 == 0.0);
    CHECK(e.b.ammaccatura == 0.0);
}

TEST_CASE("collisioni: Sceptre e K-211 (1981), di prua sulle eliche di un SSBN fermo") {
    if (!ci_sono_dati()) return;
    // lo Sceptre (forma del Trafalgar, 4.900 t) a 7 kn sulla poppa del Delta III (forma dell'Oscar, 13.250 t) quasi
    // fermo. Atteso: prua dell'urtante distrutta (sonar di prua perso); elica e velocita' dell'urtato; falla piccola
    // 0,01-0,1 m^2 nello scafo resistente dell'urtante (l'elica che lo incide)
    CorpoUrto b = corpo("oscar", 13250, true, 80, 0, 0.2);
    const Vec2 p = nel_mondo(b, -classi().at("oscar").lunghezza_m / 2 + 1.0, 0);
    CorpoUrto a = corpo("trafalgar", 4900, true, 80, 0, 7 * NODO);
    posa(a, p, classi().at("trafalgar").lunghezza_m / 2);
    const EsitoUrto e = valuta_urto(urto(a, b, p, 75, 0, 1, 0), classi(), M688_DATI, 17);
    CHECK(e.a.parte == ParteUrto::duomo);
    CHECK(e.b.parte == ParteUrto::eliche);
    CHECK(perdita_di(e.a, Sistema::sonar_prua) >= 0.9);
    CHECK(e.b.velocita_max < 1.0);
    CHECK(e.b.rumore_db > 0.0);
    CHECK(e.a.falla_resistente_m2 >= 0.01);
    CHECK(e.a.falla_resistente_m2 <= 0.1);
    CHECK(e.b.falla_resistente_m2 == 0.0);
}

TEST_CASE("collisioni: Greeneville ed Ehime Maru (2001), l'emersione d'emergenza sotto una nave piccola") {
    if (!ci_sono_dati()) return;
    // il 688 risale a 2,5 m/s e 10 kn verso nord; la nave (forma del Nanuchka, 900 t) va a sud a 11 kn: la vela le
    // prende la chiglia. Atteso: la nave con falla >= 2 m^2 (con la fisica delle navi affonda in minuti); il 688 con
    // piastrelle e niente falla nello scafo resistente
    const auto& la = classi().at("los_angeles");
    const double T = classi().at("nanuchka").pescaggio_m;
    CorpoUrto b = corpo("nanuchka", 900, false, 0, 180, 11 * NODO);
    const Vec2 p{0, 0};
    posa(b, p, 10);
    CorpoUrto a = corpo("los_angeles", 6927, true, T + la.larghezza_m + la.altezza_vela_m, 0, 10 * NODO, -2.5);
    posa(a, p, 23);
    const EsitoUrto e = valuta_urto(urto(a, b, p, T, 0, 0, 1), classi(), M688_DATI, 19);
    CHECK(e.a.parte == ParteUrto::vela);
    CHECK(e.b.parte == ParteUrto::chiglia);
    CHECK(e.b.falla_nave_m2 >= 2.0);
    CHECK(e.a.falla_resistente_m2 == 0.0);
    CHECK(e.a.piastrelle_m2 > 0.0);
    CHECK(perdita_integrita_urto(e.b) > 0.1);
    // con la fisica delle navi (lo scafo del Nanuchka): abbandono entro 10 minuti, a fondo entro 30 (Ehime Maru: 5-10)
    DanniNave n(DatiScafo::carica(std::string(M688_DATI) + "/" + classi().at("nanuchka").scafo), 1);
    n.urto_meccanico(e.b);
    double abbandono = -1.0;
    for (int k = 0; k < 1800 && !n.affondata(); ++k) {
        n.avanza(1.0);
        if (abbandono < 0 && n.condannata()) abbandono = n.tempo();
    }
    MESSAGE("abbandono a ", abbandono, " s, a fondo a ", n.tempo(), " s");
    CHECK(abbandono >= 0.0);
    CHECK(abbandono < 600.0);
    CHECK(n.affondata());
}

TEST_CASE("collisioni: Hartford e New Orleans (2009), preso al traverso a quota periscopica") {
    if (!ci_sono_dati()) return;
    // il 688 a quota periscopica (chiglia a 18 m) verso nord a 4 kn; una nave da 25.000 t (forma del Sacramento) a
    // 10 kn verso est lo prende di prua sulla vela, a 6 m. Atteso: rollio >= 45 gradi, vela (periscopio) e piani; la
    // nave con falla 5-40 m^2; niente falla nello scafo resistente del 688
    CorpoUrto b = corpo("los_angeles", 6927, true, 18, 0, 2.0);
    const Vec2 p = nel_mondo(b, 23, 1.5);
    CorpoUrto a = corpo("sacramento", 25000, false, 0, 90, 10 * NODO);
    posa(a, p, classi().at("sacramento").lunghezza_m / 2);
    const EsitoUrto e = valuta_urto(urto(a, b, p, 6, 1, 0, 0), classi(), M688_DATI, 23);
    CHECK(e.a.parte == ParteUrto::prua);
    CHECK(e.b.parte == ParteUrto::vela);
    CHECK(e.b.rollio_gradi >= 45.0);
    CHECK(perdita_di(e.b, Sistema::periscopio) >= 0.5);
    CHECK(perdita_di(e.b, Sistema::idroplani) > 0.0);
    CHECK(e.a.falla_nave_m2 >= 5.0);
    CHECK(e.a.falla_nave_m2 <= 40.0);
    CHECK(e.b.falla_resistente_m2 == 0.0);
}

TEST_CASE("collisioni: S-178 e Refrizherator-13 (1981), in superficie a 25 gradi") {
    if (!ci_sono_dati()) return;
    // il sottomarino convenzionale in superficie (forma del Foxtrot, 1.050 t come il Whiskey), fermo verso nord; il
    // frigorifero (forma del mercantile, 5.000 t) a 8 kn lo prende di prua sul fianco sinistro a 25 gradi, poppavia.
    // Atteso: falla 1-4 m^2 (16: ~2 m^2); con le porte aperte il compartimento si allaga in secondi
    const auto& fx = classi().at("foxtrot");
    CorpoUrto b = corpo("foxtrot", 1050, true, fx.pescaggio_m, 0, 0);
    b.in_superficie = true;
    const Vec2 p = nel_mondo(b, -20, fx.larghezza_m / 2);
    CorpoUrto a = corpo("mercantile", 5000, false, 0, 25, 8 * NODO);
    posa(a, p, classi().at("mercantile").lunghezza_m / 2);
    const EsitoUrto e = valuta_urto(urto(a, b, p, 1.5, 1, 0, 0), classi(), M688_DATI, 29);
    CHECK(e.angolo_gradi == doctest::Approx(25.0).epsilon(0.05));
    CHECK(e.b.parte == ParteUrto::fianco);
    CHECK(e.b.falla_nave_m2 >= 1.0);
    CHECK(e.b.falla_nave_m2 <= 4.0);
    CHECK(e.b.falla_resistente_m2 >= 1.0);   // la stessa falla per il modello d'immersione
}

TEST_CASE("collisioni: San Francisco (2005), contro un monte a 33 kn") {
    if (!ci_sono_dati()) return;
    // ostacolo fisso: ~1 GJ (+-20%) assorbito dalla prua, duomo perso, scafo resistente senza falla ma ammaccato
    CorpoUrto a = corpo("los_angeles", 6927, true, 160, 0, 33 * NODO);
    const Vec2 p{0, 0};
    posa(a, p, MEZZO_688);
    CorpoUrto roccia;
    roccia.pos = {0, 50};
    const EsitoUrto e = valuta_urto(urto(a, roccia, p, 155, 0, 1, 0), classi(), M688_DATI, 31);
    CHECK(e.a.energia_mj == doctest::Approx(1000.0).epsilon(0.20));
    CHECK(e.a.parte == ParteUrto::duomo);
    CHECK(perdita_di(e.a, Sistema::sonar_prua) == doctest::Approx(1.0));
    CHECK(e.a.falla_resistente_m2 == 0.0);
    CHECK(e.a.ammaccatura > 0.0);
    CHECK(e.a.scossa_g > 1.0);   // l'equipaggio scagliato in avanti (un morto, 97 feriti)
}

TEST_CASE("collisioni: Mar Nero (1988), l'urto voluto radente sopra il galleggiamento") {
    if (!ci_sono_dati()) return;
    // la Bezzavetnyj (Krivak II, 3.200 t) accosta contro l'anca sinistra dello Yorktown (forma dello Slava, 9.600 t), a
    // 3 m sopra l'acqua. Atteso: solo sovrastrutture; nessuna falla sotto il galleggiamento
    CorpoUrto b = corpo("slava", 9600, false, 0, 0, 2.0);
    const Vec2 p = nel_mondo(b, -60, classi().at("slava").larghezza_m / 2);
    CorpoUrto a = corpo("krivak2", 3200, false, 0, 5, 3.5);   // la raggiunge da poppavia
    posa(a, p, classi().at("krivak2").lunghezza_m / 2);
    const EsitoUrto e = valuta_urto(urto(a, b, p, -3, 1, 0, 0), classi(), M688_DATI, 37);
    CHECK(e.radente);
    CHECK(e.b.parte == ParteUrto::sovrastrutture);
    CHECK(e.b.sovrastrutture);
    CHECK(e.a.falla_nave_m2 == 0.0);
    CHECK(e.b.falla_nave_m2 == 0.0);
}

TEST_CASE("collisioni: deterministico; i danni arrivano ai modelli") {
    if (!ci_sono_dati()) return;
    const EsitoUrto x = prua_su_victor(10, 90), y = prua_su_victor(10, 90);
    CHECK(x.a.falla_cassa_m2 == y.a.falla_cassa_m2);
    CHECK(x.b.falla_cassa_m2 == y.b.falla_cassa_m2);
    CHECK(x.a.sistemi.size() == y.a.sistemi.size());
    CHECK(x.livello_sorgente_db == y.livello_sorgente_db);
    CHECK(x.livello_sorgente_db > 170.0);
    // lo Sceptre: la falla dell'elica nel suo scafo resistente, l'ammaccatura, il sonar di prua
    CorpoUrto b = corpo("oscar", 13250, true, 80, 0, 0.2);
    const Vec2 p = nel_mondo(b, -classi().at("oscar").lunghezza_m / 2 + 1.0, 0);
    CorpoUrto a = corpo("trafalgar", 4900, true, 80, 0, 7 * NODO);
    posa(a, p, classi().at("trafalgar").lunghezza_m / 2);
    const EsitoUrto e = valuta_urto(urto(a, b, p, 75, 0, 1, 0), classi(), M688_DATI, 17);
    BattelloDanni t(DatiScafo::carica(std::string(M688_DATI) + "/" + classi().at("trafalgar").scafo), 1);
    const double collasso = t.quota_collasso_m();
    t.urto_meccanico(e.a);
    CHECK(t.falle().size() == 1);
    CHECK(t.quota_collasso_m() < collasso);
    CHECK(t.sistema(Sistema::sonar_prua).efficienza < 0.2);
    CHECK(t.eventi().front().tipo == EventoBattello::Tipo::scossa);
}

TEST_CASE("collisioni: i pescherecci nuovi (agi, peschereccio) galleggiano; la vela di un 688 li allaga") {
    if (!ci_sono_dati() || !classi().count("agi") || !classi().count("peschereccio")) return;
    for (const std::string c : {"agi", "peschereccio"}) {
        CAPTURE(c);
        const auto& k = classi().at(c);
        DanniNave intatta(DatiScafo::carica(std::string(M688_DATI) + "/" + k.scafo), 1);
        for (int i = 0; i < 120; ++i) intatta.avanza(1.0);
        CHECK_FALSE(intatta.affondata());
        CHECK(std::fabs(intatta.sbandamento_gradi()) < 2.0);
        // il 688 che risale sotto a 2,5 m/s (come il Greeneville) e taglia la chiglia a poppavia del mezzo: squarcio su
        // due compartimenti, acqua per oltre meta' del dislocamento. Restano a galla (la compartimentazione, STIMA,
        // regge due compartimenti); l'Ehime Maru, piu' piccolo, affondo'
        CorpoUrto b = corpo(c, c == "agi" ? 760 : 2000, false, 0, 180, 5.0);
        const Vec2 p{0, 0};
        posa(b, p, -5);
        const auto& la = classi().at("los_angeles");
        CorpoUrto a = corpo("los_angeles", 6927, true, k.pescaggio_m + la.larghezza_m + la.altezza_vela_m, 0, 10 * NODO, -2.5);
        posa(a, p, 23);
        const EsitoUrto e = valuta_urto(urto(a, b, p, k.pescaggio_m, 0, 0, 1), classi(), M688_DATI, 41);
        CHECK(e.b.falla_nave_m2 > 1.0);
        CHECK(e.b.squarcio_m > 5.0);
        DanniNave n(DatiScafo::carica(std::string(M688_DATI) + "/" + k.scafo), 1);
        n.urto_meccanico(e.b);
        for (int i = 0; i < 1800 && !n.affondata(); ++i) n.avanza(1.0);
        CHECK(n.acqua_imbarcata_t() > 0.5 * n.dislocamento_intatto_t());
    }
}
