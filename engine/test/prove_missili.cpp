// Prove dei missili lanciati dal sottomarino (nucleo/missili.hpp; docs/21_stazione_missili.md §11, dossier 27)
#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

#include "nucleo/missili.hpp"

using namespace m688;

namespace {

const CatalogoMissili& cat() {
    static const CatalogoMissili c = carica_missili(M688_DATI "/missili.toml");
    return c;
}

const DatiMissile& harpoon() { return *cat().missile("harpoon_84c"); }

// un bersaglio della TMA a `distanza` verso nord-est, fermo o in moto, con un'ellisse di `sigma` metri
BersaglioStimato bersaglio(double distanza, double sigma, bool moto = false, int dim = 1) {
    BersaglioStimato b;
    b.contatto = 7;
    b.dimensione = dim;
    b.pos = {distanza * std::sin(radianti(45.0)), distanza * std::cos(radianti(45.0))};
    b.ellisse = {sigma, sigma, 0.0};
    b.moto_noto = moto;
    b.rotta = 210.0;
    b.velocita_m_s = moto ? 18.0 * NODO : 0.0;
    return b;
}

StatoLanciatore battello() {
    StatoLanciatore l;
    l.quota_m = 25.0;
    l.velocita_kn = 6.0;
    l.navigazione = {500.0, 500.0, 0.0};
    return l;
}

Vec2 avanti_di(Vec2 p, double rotta, double d) { return {p.x + d * std::sin(radianti(rotta)), p.y + d * std::cos(radianti(rotta))}; }

EcoNave nave(int id, Vec2 pos, int dim = 1) {
    EcoNave n;
    n.id = id;
    n.pos = pos;
    n.dimensione = dim;
    return n;
}

// il volo fino alla fine, con le navi ferme; ritorna la nave colpita o -1
int vola(Missile& mi, const std::vector<EcoNave>& navi, int mare = 3) {
    for (int k = 0; k < 20000 && mi.attivo(); ++k) mi.avanza(0.5, navi, mare);
    return mi.fase() == FaseMissile::impatto ? mi.bersaglio() : -1;
}

}  // namespace

TEST_CASE("missili: il catalogo si carica, con fonte e confidenza, e le armi del carico hanno la loro classe per anno") {
    REQUIRE(cat().missili.size() >= 8);
    for (const auto& [id, d] : cat().missili) {
        CAPTURE(id);
        CHECK(!d.fonte.empty());
        CHECK(!d.confidenza.empty());
        CHECK(d.velocita_m_s > 0.0);
        CHECK(d.portata_m > 0.0);
        CHECK(d.affidabilita > 0.5);
        CHECK(d.affidabilita <= 1.0);
    }
    CHECK(harpoon().terminale == Terminale::radente);
    CHECK(cat().missile("harpoon_84a")->terminale == Terminale::pop_up);
    CHECK(cat().missile("tasm")->esm);
    CHECK(cat().missile("tasm")->uscita == UscitaMissile::booster_subacqueo);
    CHECK(cat().missile("tlam_c")->ruolo == RuoloMissile::attacco_terra);
    REQUIRE(cat().per_arma("harpoon", 1981));
    CHECK(cat().per_arma("harpoon", 1981)->id == "harpoon_84a");
    CHECK(cat().per_arma("harpoon", 1989)->id == "harpoon_84c");
    CHECK(cat().per_arma("tasm", 1989)->id == "tasm");
    CHECK(cat().per_arma("tlam_c", 1985) == nullptr);
    CHECK(cat().per_arma("mk48", 1989) == nullptr);
    // il missile antinave delle classi dell'IA, per il MISSILE degli script col modello (docs/21 §7)
    REQUIRE(cat().per_classe("oscar", 1989));
    CHECK(cat().per_classe("oscar", 1989)->id == "p700");
    CHECK(cat().per_classe("slava", 1989)->id == "p500");
    CHECK(cat().per_classe("charlie2", 1989)->id == "p120");
    CHECK(cat().per_classe("perry", 1980)->id == "harpoon_84a");
    CHECK(cat().per_classe("perry", 1989)->id == "harpoon_84c");
    CHECK(cat().per_classe("slava", 1980) == nullptr);   // prima dell'entrata in servizio
    CHECK(cat().per_classe("kara", 1989) == nullptr);    // l'SS-N-14 e' antisom: fuori dal catalogo
    CHECK(cat().per_classe("victor3", 1989) == nullptr);
}

TEST_CASE("missili: l'ellisse del bersaglio cresce piano col moto noto e come velocita' massima x eta' senza") {
    BersaglioStimato b;
    b.ellisse = {2.0 * MIGLIO, 2.0 * MIGLIO, 0.0};
    b.velocita_m_s = 24.0 * NODO;
    b.velocita_max_m_s = 30.0 * NODO;
    b.rotta = 320.0;
    b.sigma_velocita_m_s = 2.0 * NODO;
    b.sigma_rotta_deg = 10.0;
    const double t = 70.0 * 60.0;   // l'esempio del Kirov degli appunti OTH §3.2
    b.moto_noto = true;
    const EllisseErrore a = ellisse_a(b, t);
    b.moto_noto = false;
    const EllisseErrore c = ellisse_a(b, t);
    MESSAGE("moto noto: " << a.maggiore_m / MIGLIO << " x " << a.minore_m / MIGLIO << " mi; ignoto: " << c.maggiore_m / MIGLIO << " mi");
    CHECK(a.maggiore_m * a.minore_m < c.maggiore_m * c.minore_m / 4.0);   // un dato con rotta vale piu' di uno senza
    CHECK(c.maggiore_m == doctest::Approx(c.minore_m).epsilon(0.01));      // senza rotta e' un cerchio
    CHECK(c.maggiore_m == doctest::Approx(std::hypot(2.0 * MIGLIO, 30.0 * NODO * t / 2.0)).epsilon(0.01));
    // l'asse maggiore col moto noto sta attraverso la rotta (l'errore di rotta pesa piu' di quello di velocita')
    const double asse = std::fmod(std::abs(a.orientamento_deg - 50.0), 180.0);   // 320 + 90 = 410 -> asse 50-230
    CHECK(std::min(asse, 180.0 - asse) < 5.0);
    // prima dell'ora della posizione resta quella data; la posizione segue il moto
    CHECK(ellisse_a(b, -10.0).maggiore_m == doctest::Approx(2.0 * MIGLIO));
    b.moto_noto = true;
    CHECK(distanza(posizione_a(b, 600.0), b.pos) == doctest::Approx(24.0 * NODO * 600.0));
    // la somma delle covarianze: due cerchi da 3 e 4 fanno un cerchio da 5
    const EllisseErrore s = somma({3000.0, 3000.0, 0.0}, {4000.0, 4000.0, 0.0});
    CHECK(s.maggiore_m == doctest::Approx(5000.0));
    // un rapporto radio senza rotta: geografico, rotta ignota
    const BersaglioStimato r = da_rapporto(3, {1000.0, 2000.0}, 50.0, 4000.0, std::nullopt, 9.0, 15.0);
    CHECK(r.fonte == FonteDati::radio);
    CHECK_FALSE(r.relativo);
    CHECK_FALSE(r.moto_noto);
}

TEST_CASE("missili: la scatola RBL sta sul previsto e scala con la sigma; fuori portata e troppo vicino bloccano") {
    const StatoLanciatore l = battello();
    const PianoTiro p = pianifica(harpoon(), {}, bersaglio(38200.0, 1000.0), l, {}, 0.0);
    CHECK(p.valido);
    CHECK(p.area.forma == AreaRicerca::Forma::scatola);
    CHECK(p.area.contorno.size() == 4);
    CHECK(p.rotta == doctest::Approx(45.0).epsilon(0.001));
    CHECK(p.distanza_m == doctest::Approx(38200.0).epsilon(0.001));
    CHECK(p.tempo_volo_s == doctest::Approx(harpoon().salita_s + 38200.0 / harpoon().velocita_m_s).epsilon(0.01));
    CHECK(p.accensione_m == doctest::Approx(38200.0 - 10000.0).epsilon(0.001));   // media: il minimo di 10 km
    CHECK(p.p_area > 0.95);
    // con piu' incertezza la scatola si allunga
    const PianoTiro q = pianifica(harpoon(), {}, bersaglio(38200.0, 8000.0), l, {}, 0.0);
    CHECK(q.accensione_m < p.accensione_m - 5000.0);
    CHECK(q.p_area < p.p_area);
    // la grande e' piu' larga della piccola
    ImpostazioniMissile piccola, grande;
    piccola.taglia = Taglia::piccola;
    grande.taglia = Taglia::grande;
    CHECK(pianifica(harpoon(), piccola, bersaglio(38200.0, 1000.0), l, {}, 0.0).accensione_m >
          pianifica(harpoon(), grande, bersaglio(38200.0, 1000.0), l, {}, 0.0).accensione_m);
    // il bersaglio in moto: il previsto e' dove sara' all'arrivo
    const PianoTiro m = pianifica(harpoon(), {}, bersaglio(38200.0, 1000.0, true), l, {}, 0.0);
    CHECK(distanza(m.previsto, bersaglio(38200.0, 1000.0).pos) == doctest::Approx(18.0 * NODO * m.tempo_volo_s).epsilon(0.02));
    // i blocchi
    const PianoTiro lontano = pianifica(harpoon(), {}, bersaglio(200000.0, 1000.0), l, {}, 0.0);
    CHECK_FALSE(lontano.valido);
    CHECK(lontano.avvisi.front() == Avviso::fuori_portata);
    CHECK(lontano.p_successo == 0.0);
    CHECK_FALSE(pianifica(harpoon(), {}, bersaglio(3000.0, 300.0), l, {}, 0.0).valido);
    StatoLanciatore fondo = l;
    fondo.quota_m = 120.0;
    CHECK_FALSE(pianifica(harpoon(), {}, bersaglio(38200.0, 1000.0), fondo, {}, 0.0).valido);
    BersaglioStimato senza = bersaglio(38200.0, 1000.0);
    senza.distanza_nota = false;
    const PianoTiro rbl = pianifica(harpoon(), {}, senza, l, {}, 0.0);
    CHECK_FALSE(rbl.valido);
    CHECK(rbl.avvisi.front() == Avviso::solo_bol);
}

TEST_CASE("missili: sotto la quota minima dell'Harpoon un avviso che non blocca e piu' guasti in salita; il TLAM non ne ha") {
    StatoLanciatore alto = battello();
    alto.quota_m = 12.0;
    const PianoTiro p = pianifica(harpoon(), {}, bersaglio(38200.0, 1000.0), alto, {}, 0.0);
    CHECK(p.valido);
    CHECK(std::find(p.avvisi.begin(), p.avvisi.end(), Avviso::quota_bassa) != p.avvisi.end());
    CHECK_FALSE(bloccante(Avviso::quota_bassa));
    CHECK(std::string(nome_avviso(Avviso::quota_bassa)) == "QUOTA DI LANCIO TROPPO BASSA");
    CHECK(harpoon().quota_lancio_min_m == doctest::Approx(20.0));
    CHECK(cat().missile("tlam_c")->quota_lancio_min_m == 0.0);
    CHECK(cat().missile("tasm")->quota_lancio_min_m == 0.0);
    // in salita: con la capsula a 12 m si guastano di piu' che a 25 m (affidabilita' 1 per isolare la salita)
    DatiMissile m = harpoon();
    m.affidabilita = 1.0;
    int guasti_alto = 0, guasti_giusto = 0;
    for (std::uint64_t seme = 1; seme <= 400; ++seme) {
        Missile a(m, {}, p, p.lancio, 12.0, 6.0, 0, seme), b(m, {}, p, p.lancio, 25.0, 6.0, 0, seme);
        vola(a, {});
        vola(b, {});
        guasti_alto += a.fase() == FaseMissile::guasto;
        guasti_giusto += b.fase() == FaseMissile::guasto;
    }
    CHECK(guasti_giusto == 0);
    CHECK(guasti_alto / 400.0 == doctest::Approx(0.10).epsilon(0.4));
}

TEST_CASE("missili: il ventaglio BOL si allarga dal punto d'accensione; la serpentina del TASM copre l'area") {
    ImpostazioniMissile bol;
    bol.modo = ModoLancio::bol;
    bol.srch_m = 20000.0;
    BersaglioStimato b = bersaglio(38200.0, 3000.0);
    b.distanza_nota = false;
    b.rilevamento = 45.0;
    const PianoTiro p = pianifica(harpoon(), bol, b, battello(), {}, 0.0);
    CHECK(p.valido);
    CHECK(p.area.forma == AreaRicerca::Forma::ventaglio);
    CHECK(p.accensione_m == doctest::Approx(20000.0));
    CHECK(p.fine_m == doctest::Approx(harpoon().portata_m));
    // stretto vicino al punto d'accensione, largo piu' avanti
    const auto largo = [&](double r) {
        double w = 0.0;
        for (const Vec2& q : p.area.contorno) {
            double s, x;
            const double a = radianti(p.rotta);
            s = (q.x - p.accensione.x) * std::sin(a) + (q.y - p.accensione.y) * std::cos(a);
            x = (q.x - p.accensione.x) * std::cos(a) - (q.y - p.accensione.y) * std::sin(a);
            if (std::abs(s - r) < 2500.0) w = std::max(w, std::abs(x));
        }
        return w;
    };
    CHECK(largo(2000.0) < largo(30000.0));

    const DatiMissile& tasm = *cat().missile("tasm");
    BersaglioStimato lontano = bersaglio(300000.0, 15000.0);
    lontano.fonte = FonteDati::radio;
    lontano.relativo = false;
    const PianoTiro s = pianifica(tasm, {}, lontano, battello(), {}, 0.0);
    CHECK(s.valido);
    CHECK(s.area.forma == AreaRicerca::Forma::serpentina);
    CHECK(s.area.percorso.size() > 4);
    CHECK(s.accensione_m == doctest::Approx(300000.0 - SERPENTINA_SALITA_M).epsilon(0.001));
    CHECK(s.p_area > 0.5);
    CHECK(std::find(s.avvisi.begin(), s.avvisi.end(), Avviso::rotta_ignota) != s.avvisi.end());
}

TEST_CASE("missili: contatti nell'area in ordine dal piu' vicino; un neutrale avvisa; la probabilita' scende") {
    const BersaglioStimato b = bersaglio(38200.0, 1000.0);
    const auto davanti = [&](double d, double lato) {   // un contatto lungo la rotta del missile
        const double a = radianti(45.0);
        return Vec2{d * std::sin(a) + lato * std::cos(a), d * std::cos(a) - lato * std::sin(a)};
    };
    std::vector<ContattoNoto> k = {{7, b.pos, 1, false}, {11, davanti(34000.0, 1500.0), 1, true}, {12, davanti(31000.0, -800.0), 0, false},
                                   {13, davanti(60000.0, 0.0), 2, false}};
    const PianoTiro solo = pianifica(harpoon(), {}, b, battello(), {}, 0.0);
    const PianoTiro p = pianifica(harpoon(), {}, b, battello(), k, 0.0);
    REQUIRE(p.in_area.size() == 3);   // il 13 e' oltre la scatola
    CHECK(p.in_area[0] == 12);
    CHECK(p.in_area[1] == 11);
    CHECK(p.in_area[2] == 7);
    CHECK(std::find(p.avvisi.begin(), p.avvisi.end(), Avviso::neutrali_in_area) != p.avvisi.end());
    CHECK(p.neutrali_in_area == std::vector<int>{11});   // per la carta: quale dei contatti dell'area
    CHECK(solo.neutrali_in_area.empty());
    CHECK(p.valido);   // l'avviso non blocca
    CHECK(p.p_primo < solo.p_primo);
    CHECK(p.p_successo < solo.p_successo);
    CHECK(solo.p_successo == doctest::Approx(solo.p_area * harpoon().affidabilita));
    CHECK(p.contatto == 7);
    // l'amico creduto nell'area: un avviso come il neutrale, non blocca; il bersaglio stesso non si conta
    CHECK(p.amici_in_area.empty());
    k[2].amico_creduto = true;
    k[0].amico_creduto = true;
    const PianoTiro a = pianifica(harpoon(), {}, b, battello(), k, 0.0);
    CHECK(a.amici_in_area == std::vector<int>{12});
    CHECK(std::find(a.avvisi.begin(), a.avvisi.end(), Avviso::amici_in_area) != a.avvisi.end());
    CHECK_FALSE(bloccante(Avviso::amici_in_area));
    CHECK(a.valido);
    CHECK(std::string(nome_avviso(Avviso::amici_in_area)).find("AMICI") != std::string::npos);
}

TEST_CASE("missili: la preparazione accende PWR e poi ALN; il TLAM, il primo in 15 minuti e gli altri in pochi") {
    PreparazioneMissile p(harpoon());
    CHECK(p.fase() == FasePreparazione::spento);
    CHECK_FALSE(p.pwr());
    p.avvia();
    CHECK(p.fase() == FasePreparazione::accensione);
    p.avanza(harpoon().accensione_s + 1.0);
    CHECK(p.pwr());
    CHECK_FALSE(p.aln());
    p.avanza(harpoon().allineamento_s);
    CHECK(p.aln());
    CHECK(p.fase() == FasePreparazione::pronto);
    p.spegni();
    CHECK_FALSE(p.pwr());
    const DatiMissile& tlam = *cat().missile("tlam_c");
    CHECK(preparazione_tlam_s(tlam, 1) == doctest::Approx(tlam.preparazione_primo_s));
    CHECK(preparazione_tlam_s(tlam, 4) == doctest::Approx(tlam.preparazione_primo_s + 3.0 * tlam.preparazione_altri_s));
    // il TLAM: il primo di bordo 15 minuti (un terzo accensione, due terzi allineamento), gli altri piu' in fretta
    PreparazioneMissile t(tlam);
    t.avvia();
    CHECK(t.fase() == FasePreparazione::accensione);
    t.avanza(tlam.preparazione_primo_s / 3.0 + 1.0);
    CHECK(t.pwr());
    CHECK_FALSE(t.aln());
    t.avanza(tlam.preparazione_primo_s * 2.0 / 3.0);
    CHECK(t.aln());
    PreparazioneMissile altro(tlam, false);
    altro.avvia();
    altro.avanza(tlam.preparazione_altri_s + 1.0);
    CHECK(altro.aln());
    CHECK(tlam.preparazione_altri_s < tlam.preparazione_primo_s / 2.0);
}

TEST_CASE("missili: in volo arriva sul previsto; uno scarto di navigazione sposta tutto e la nave vera resta fuori") {
    DatiMissile m = harpoon();
    m.affidabilita = 1.0;
    const BersaglioStimato b = bersaglio(38200.0, 1000.0);
    const PianoTiro p = pianifica(m, {}, b, battello(), {}, 0.0);
    {
        Missile mi(m, {}, p, p.lancio, 25.0, 6.0, 0, 1);
        CHECK(vola(mi, {nave(7, b.pos)}) == 7);
        CHECK(distanza(mi.pos(), b.pos) < 1.0);
    }
    {
        // il battello crede di essere 12 km piu' a ovest: il missile vero parte da 12 km piu' a est e la scatola vera
        // lascia la nave fuori
        Missile mi(m, {}, p, {p.lancio.x + 12000.0, p.lancio.y}, 25.0, 6.0, 0, 1);
        CHECK(vola(mi, {nave(7, b.pos)}) == -1);
        CHECK(mi.fase() == FaseMissile::esaurito);
    }
    // senza navi esaurisce alla fine della scatola
    Missile vuoto(m, {}, p, p.lancio, 25.0, 6.0, 0, 1);
    vola(vuoto, {});
    CHECK(vuoto.fase() == FaseMissile::esaurito);
    CHECK(vuoto.percorso_m() == doctest::Approx(p.fine_m).epsilon(0.02));
    // la stima per la carta segue il piano, non la verita'
    const StimaVolo s = stima_volo(p, {}, m, p.tempo_volo_s);
    CHECK(distanza(s.pos, p.previsto) < 1500.0);
    CHECK(s.fase == FaseMissile::ricerca);
}

TEST_CASE("missili: il seeker prende il primo eco valido della scatola, anche se neutrale; il TASM preferisce chi emette") {
    DatiMissile m = harpoon();
    m.affidabilita = 1.0;
    const BersaglioStimato b = bersaglio(38200.0, 1000.0);
    const PianoTiro p = pianifica(m, {}, b, battello(), {}, 0.0);
    const double a = radianti(45.0);
    const Vec2 prima{33000.0 * std::sin(a) + 900.0 * std::cos(a), 33000.0 * std::cos(a) - 900.0 * std::sin(a)};
    // un mercantile grande davanti al bersaglio medio: e' il piu' grande e il piu' vicino, lo prende sempre
    for (std::uint64_t seme = 1; seme <= 5; ++seme) {
        Missile mi(m, {}, p, p.lancio, 25.0, 6.0, 0, seme);
        CHECK(vola(mi, {nave(7, b.pos), nave(99, prima, 2)}) == 99);
    }
    // una motovedetta davanti a un incrociatore: la prende solo con p_acquisizione_scorta, se no passa oltre
    int presa = 0;
    for (std::uint64_t seme = 1; seme <= 200; ++seme) {
        Missile mi(m, {}, p, p.lancio, 25.0, 6.0, 0, seme);
        const int colpita = vola(mi, {nave(7, b.pos, 2), nave(98, prima, 0)});
        CHECK((colpita == 98 || colpita == 7));
        presa += colpita == 98;
    }
    CHECK(presa / 200.0 == doctest::Approx(m.p_acquisizione_scorta).epsilon(0.25));
    // il TASM con l'ESM: una nave che emette il radar programmato attira il missile anche se non e' la piu' vicina
    DatiMissile t = *cat().missile("tasm");
    t.affidabilita = 1.0;
    BersaglioStimato lontano = bersaglio(150000.0, 5000.0, false, 2);
    const PianoTiro s = pianifica(t, {}, lontano, battello(), {}, 0.0);
    EcoNave kirov = nave(20, avanti_di(s.previsto, s.rotta + 90.0, 15000.0), 2);   // di lato, col Top Sail acceso
    kirov.radar_accesi = {"top_sail"};
    EcoNave mercantile = nave(21, avanti_di(s.accensione, s.rotta, 8000.0), 2);      // sulla rotta, piu' vicino
    Missile mi(t, {}, s, s.lancio, 25.0, 6.0, 0, 3);
    CHECK(vola(mi, {mercantile, kirov}) == 20);
}

TEST_CASE("missili: TLAM, le quattro validita', la presa della prima mappa e il «sicuro di se' e sbagliato»") {
    const DatiMissile& m = *cat().missile("tlam_c");
    MissioneTlam t;
    t.plp = {0.0, 0.0};
    t.landfall = {0.0, 250000.0};
    t.tratta_terra_m = 200000.0;
    t.tot_s = 4.0 * 3600.0;
    const double volo = valuta_tlam(m, t, t.plp, 0.0, 0.0).tempo_volo_s;
    const double ora = t.tot_s - volo;
    CHECK(valuta_tlam(m, t, t.plp, 500.0, ora).validita == ValiditaTlam::valida);
    const Vec2 indietro{0.0, -20.0 * MIGLIO};   // 20 miglia dal PLP: si lancia 10 minuti piu' tardi per il ricalcolo
    const double ora_indietro = valuta_tlam(m, t, indietro, 500.0, 0.0).ora_lancio_s - TLAM_RIPIANIFICA_S;
    const StatoTlam r = valuta_tlam(m, t, indietro, 500.0, ora_indietro);
    CHECK(r.validita == ValiditaTlam::ripianifica);
    CHECK(r.ripianifica_s == doctest::Approx(TLAM_RIPIANIFICA_S));
    CHECK(valuta_tlam(m, t, {0.0, -60.0 * MIGLIO}, 500.0, ora).validita == ValiditaTlam::non_valida);
    CHECK(valuta_tlam(m, t, t.plp, 500.0, ora + 3600.0).validita == ValiditaTlam::fuori_finestra);
    // P(landfall) = 1 - exp(-H^2 / 2 sigma^2), con la sigma del battello che domina
    const StatoTlam a = valuta_tlam(m, t, t.plp, 2.0 * MIGLIO, ora);
    CHECK(a.p_landfall == doctest::Approx(1.0 - std::exp(-2.5 * 2.5 / (2.0 * 2.0 * 2.0))).epsilon(0.05));
    CHECK(valuta_tlam(m, t, t.plp, 0.5 * MIGLIO, ora).p_landfall > 0.99);
    // stimata valida con l'ellisse piccola, vera lontana: il pannello dice 99%, l'esito si perde spesso
    const StatoTlam stimato = valuta_tlam(m, t, t.plp, 0.5 * MIGLIO, ora);
    const StatoTlam vero = valuta_tlam(m, t, {4.0 * MIGLIO, 0.0}, 4.0 * MIGLIO, ora);
    CHECK(stimato.validita == ValiditaTlam::valida);
    int arrivati = 0;
    for (std::uint64_t seme = 1; seme <= 200; ++seme) arrivati += esito_tlam(m, t, vero, ora, seme).arrivato;
    CHECK(arrivati < 100);
    CHECK(esito_tlam(m, t, vero, ora, 1).t_arrivo == doctest::Approx(ora + vero.tempo_volo_s));
}

TEST_CASE("missili: deterministici dal seme") {
    DatiMissile m = harpoon();
    const BersaglioStimato b = bersaglio(38200.0, 3000.0);
    const PianoTiro p = pianifica(m, {}, b, battello(), {}, 0.0);
    const std::vector<EcoNave> navi = {nave(7, b.pos, 2), nave(8, {b.pos.x - 3000.0, b.pos.y - 3000.0}, 0)};
    for (std::uint64_t seme : {1u, 2u, 77u}) {
        Missile x(m, {}, p, p.lancio, 25.0, 6.0, 0, seme), y(m, {}, p, p.lancio, 25.0, 6.0, 0, seme);
        CHECK(vola(x, navi) == vola(y, navi));
        CHECK(x.tempo_s() == y.tempo_s());
    }
}
