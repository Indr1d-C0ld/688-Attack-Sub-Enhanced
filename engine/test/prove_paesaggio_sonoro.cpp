#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <set>
#include <string>

#include "nucleo/acustica.hpp"
#include "nucleo/analisi_acustica.hpp"

using namespace m688;

// Paesaggio sonoro: sorgenti della libreria (Fase 3, S5c; dossier 09 §5 e §6.2)

namespace {

const CatalogoAcustico& catalogo() {
    static const CatalogoAcustico c = carica_acustica(M688_DATI "/acustica.toml");
    return c;
}

Sorgente sorgente(const char* id, Vec2 pos, double quota, double tempo, double finestra, std::uint64_t seme = 1) {
    IstanzaSorgente i;
    i.id = 100000;
    i.sorgente = id;
    i.pos = pos;
    i.quota_m = quota;
    i.seme = seme;
    return sorgente_da_istanza(catalogo(), i, tempo, finestra);
}

}  // namespace

TEST_CASE("paesaggio: la libreria delle sorgenti si carica con categorie, componenti e livelli rms") {
    const auto& c = catalogo();
    CHECK(c.sorgenti.size() >= 30);
    const std::set<std::string> categorie = {"biologico", "geofisico", "meteo", "civile", "industriale"};
    for (const auto& [id, f] : c.sorgenti) {
        CHECK_MESSAGE(categorie.count(f.categoria) == 1, id);
        CHECK_MESSAGE(!f.componenti.empty(), id);
        CHECK_MESSAGE(!f.fonte.empty(), id);
    }
    CHECK(c.sorgente("capodoglio")->livello_db == doctest::Approx(221.0));   // 230 dB pp -> rms
    for (const char* id : {"balenottera_comune", "megattera", "capodoglio", "orca", "foca_barbata", "merluzzo", "eglefino",
                           "coro_pesci", "gamberi", "t_phase_tettonico", "tremore_iceberg", "airgun", "battitura_pali",
                           "peschereccio_strascico"})
        CHECK_MESSAGE(c.sorgente(id) != nullptr, id);
    CHECK(std::string(fascia_oraria(12.0)) == "giorno");
    CHECK(std::string(fascia_oraria(19.0)) == "crepuscolo");
    CHECK(std::string(fascia_oraria(2.0)) == "notte");
}

TEST_CASE("paesaggio: ritmo degli eventi (periodico, treni, Poisson) ripetibile dal seme") {
    const auto& c = catalogo();
    // cannoni ad aria: 0,1 s ogni 10-20 s, quindi circa lo 0,7% del tempo
    const SorgenteLibreria& ag = *c.sorgente("airgun");
    const double a = attivita(ag, ag.componenti[0], 7, 7200.0, 3600.0);
    CHECK(a == doctest::Approx(0.1 / 15.0).epsilon(0.25));
    CHECK(attivita(ag, ag.componenti[0], 7, 7200.0, 3600.0) == a);   // ripetibile
    // un altro seme, altri istanti (in un'ora i colpi sono comunque gli stessi in numero)
    int diversi = 0;
    for (int k = 0; k < 500; ++k)
        diversi += (attivita(ag, ag.componenti[0], 7, k * 0.5, 0.5) > 0.0) != (attivita(ag, ag.componenti[0], 8, k * 0.5, 0.5) > 0.0);
    CHECK(diversi > 0);
    // con una finestra di 0,2 s l'impulso c'e' o non c'e'
    int accesi = 0;
    for (int k = 0; k < 2000; ++k) accesi += attivita(ag, ag.componenti[0], 7, k * 0.2, 0.2) > 0.0;
    CHECK(accesi > 5);
    CHECK(accesi < 100);
    // balenottera comune: impulsi di 0,9 s ogni 12-25 s, a gruppi di 1-2, in serie di 12 min con pause di 135 s
    const SorgenteLibreria& bc = *c.sorgente("balenottera_comune");
    CHECK(attivita(bc, bc.componenti[0], 3, 36000.0, 7200.0) == doctest::Approx(0.9 * 1.5 / 18.5 * 720.0 / 855.0).epsilon(0.3));
    // Poisson: il merluzzo grugnisce 360 volte all'ora per 0,2 s
    const SorgenteLibreria& me = *c.sorgente("merluzzo");
    CHECK(attivita(me, me.componenti[0], 5, 36000.0, 7200.0) == doctest::Approx(360.0 * 0.2 / 3600.0).epsilon(0.2));
    // le componenti continue suonano sempre
    const SorgenteLibreria& ct = *c.sorgente("piattaforma");
    CHECK(attivita(ct, ct.componenti[0], 1, 100.0, 2.0) == 1.0);
}

TEST_CASE("paesaggio: spettro delle sorgenti (banda normalizzata, righe, sweep, diurno)") {
    // una banda continua: l'integrale dello spettro vale il livello della sorgente
    const Sorgente p = sorgente("piattaforma", {}, 10.0, 0.0, 2.0);
    double e = 0.0;
    for (double f = 300.0; f < 3000.0; f += 1.0) e += std::pow(10.0, livello_fonte(p, f, true) / 10.0);
    CHECK(10.0 * std::log10(e) == doctest::Approx(184.0).epsilon(0.01));
    CHECK(livello_banda_larga(p) == doctest::Approx(184.0));
    CHECK(livello_fonte(p, 1000.0, true) > livello_fonte(p, 3000.0, true) + 20.0);   // fuori banda cala
    // famiglia armonica del tremore d'iceberg: righe a 40, 80, ... Hz quando suona
    const Sorgente t = sorgente("tremore_iceberg", {}, 50.0, 36000.0, 36000.0);
    const auto righe = righe_fonte(t);
    REQUIRE(righe.size() == 8);
    CHECK(righe[1].first == doctest::Approx(80.0));
    CHECK(righe[1].second < righe[0].second);
    // lo sweep della balenottera comune mette energia fra 15 e 40 Hz, non a 100 Hz
    const Sorgente b = sorgente("balenottera_comune", {}, 30.0, 36000.0, 3600.0);
    CHECK(livello_fonte(b, 20.0, false) > -100.0);
    CHECK(livello_fonte(b, 100.0, false) < -200.0);
    // il coro di pesci sale di 20 dB al crepuscolo
    Sorgente coro = sorgente("coro_pesci", {}, 20.0, 0.0, 2.0);
    coro.ora_locale = 12.0;
    const double giorno = livello_fonte(coro, 400.0, true);
    coro.ora_locale = 19.0;
    CHECK(livello_fonte(coro, 400.0, true) == doctest::Approx(giorno + 20.0));
}

TEST_CASE("paesaggio: popolazione per teatro, mese e area, e moto delle istanze") {
    const auto& c = catalogo();
    const auto faroe = popola_sorgenti(c, "faroe", 2, -1.0, {0.0, 0.0}, 100000.0, 42);
    CHECK_FALSE(faroe.empty());
    CHECK(popola_sorgenti(c, "faroe", 2, -1.0, {0.0, 0.0}, 100000.0, 42).size() == faroe.size());   // ripetibile
    std::set<std::string> tipi;
    for (const auto& i : faroe) {
        tipi.insert(i.sorgente);
        CHECK(distanza(i.pos, {0.0, 0.0}) <= 100000.0);
        CHECK(i.id >= 100000);
        const SorgenteLibreria& f = *c.sorgente(i.sorgente);
        CHECK(i.quota_m >= f.quota_min_m - 1e-9);
        CHECK(i.quota_m <= f.quota_max_m + 1e-9);
    }
    CHECK(tipi.count("foca_barbata") == 0);   // non alle Faroe
    CHECK(tipi.count("coro_pesci") == 0);     // solo nel Mediterraneo d'estate
    CHECK(tipi.count("peschereccio_piccolo") == 1);   // densita' 1,5 per 1000 nm^2: decine in 9000 nm^2
    for (const auto& i : popola_sorgenti(c, "otranto", 7, -1.0, {0.0, 0.0}, 100000.0, 42))
        CHECK_MESSAGE(i.sorgente != "merluzzo", i.sorgente);
    // l'ora filtra le sorgenti che dichiarano le fasce (nessuna oggi): con ora ignota non si filtra
    CHECK(popola_sorgenti(c, "faroe", 2, 3.0, {0.0, 0.0}, 100000.0, 42).size() == faroe.size());
    // moto: in linea retta fino al cambio di rotta dei 10 minuti
    IstanzaSorgente m;
    m.sorgente = "balenottera_comune";
    m.rotta = 90.0;
    m.velocita_kn = 4.0;
    m.seme = 11;
    muovi_sorgente(m, 300.0);
    CHECK(m.pos.x == doctest::Approx(4.0 * NODO * 300.0));
    CHECK(m.rotta == 90.0);
    muovi_sorgente(m, 400.0);
    CHECK(m.rotta != 90.0);
}

TEST_CASE("paesaggio: le sorgenti della libreria nei sensori, nel LOFAR e nella cascata") {
    static const ClimaTeatro clima = carica_clima(M688_DATI "/teatri/faroe.suono.toml");
    Ambiente amb;
    amb.profilo = &clima.profilo(2);
    amb.fondale_m = 3000.0;
    amb.vento_m_s = 7.0;
    Ricevitore r;
    r.quota_m = 150.0;
    r.velocita_kn = 5.0;
    r.sensore = catalogo().sensore("tb16");
    // un tremore d'iceberg a 10 km si sente, e nel LOFAR mostra la famiglia a 40 Hz
    const Sorgente t = sorgente("tremore_iceberg", {10000.0, 0.0}, 50.0, 36000.0, 36000.0);
    CHECK(valuta(t, r, amb).se_db > 0.0);
    const auto righe = righe_osservate(spettro_fascio({t}, r, amb, 90.0, 10.0, 410.0, 0.1));
    int famiglia = 0;
    for (const auto& x : righe)
        if (std::fabs(x.hz / 40.0 - std::round(x.hz / 40.0)) < 0.01) ++famiglia;
    CHECK(famiglia >= 3);
    // nessun DEMON: non c'e' elica
    CHECK_FALSE(leggi_demon(demon(t, r, amb)));
    // una sorgente diffusa (coro di gamberi) occupa sulla cascata un settore piu' largo di una puntiforme
    Ricevitore sfera = r;
    sfera.sensore = catalogo().sensore("bqq5_sfera");
    Sorgente g = sorgente("gamberi", {2000.0, 0.0}, 20.0, 0.0, 2.0);   // 2-20 kHz: la cascata a 3,5 kHz
    const auto larghezza = [&](const Sorgente& s) {
        const RigaCascata c = cascata({s}, sfera, amb, 72, 3500.0);
        const float massimo = *std::max_element(c.db.begin(), c.db.end());
        return std::count_if(c.db.begin(), c.db.end(), [&](float x) { return x > massimo - 6.0f; });
    };
    const auto diffusa = larghezza(g);
    g.estensione_m = 0.0;
    CHECK(diffusa > larghezza(g));
    // per le teste dei siluri i biologici sono falsi bersagli
    CHECK(sorgente_per_siluri(sorgente("balenottera_comune", {}, 30.0, 0.0, 2.0)).esca);
}
