#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "missioni/danni_missione.hpp"
#include "missioni/missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/schiera.hpp"
#include "nucleo/sonarista.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Il sonar fuori scafo delle navi (docs/ricerca/33; nucleo/schiera, missione_vds.cpp, missione_schiera.cpp): il pesce del
// VDS come schiera corta al cavo, sotto lo strato; la SQR-19 e la SQR-18A dal pesce; le regole dell'IA (filare, recuperare
// prima di accelerare, sprint and drift); il registro; niente di tutto questo nel classico.

namespace {

const CatalogoAcustico& catalogo() {
    static const CatalogoAcustico c = carica_acustica(M688_DATI "/acustica.toml");
    return c;
}

bool c_e() { return fs::exists(M688_DATI "/acustica.toml"); }

const std::string NAVE = "modello = { massima_kn = 30, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 0, integrita = 100, rumore = 10, sonar = 20 }";
const std::string SUB = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200, rumore = 8, sonar = 19 }";

// il 688 fermo lontano e una nave dell'IA verso nord, alle Faer Oer (ottobre: lo strato a circa 80 m), in 3.000 m d'acqua
std::string scena(const std::string& classe, double nodi, bool sovietica = false) {
    const auto f = fs::temp_directory_path() / ("m688_prova_vds_" + classe + ".lua");
    std::ofstream(f) << "return { id = 'PROVA_VDS', teatro = 'faroe', lato = 'usa', data = '1989-10-15', ora_utc = 12, paesaggio_sonoro = false,"
                        " script = {}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = -60000 },"
                        "  quota_m = 150, rotta = 0, velocita_kn = 0, " << SUB << " },\n"
                        "{ id = 2, classe = '" << classe << "', categoria = 'nave', flag = " << (sovietica ? 26 : 10)
                     << ", posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 0, velocita_kn = " << nodi << ", " << NAVE << " },\n"
                        "}, timer = {} } } }\n";
    return f.string();
}

struct Partita {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    Partita(const std::string& file, bool enhanced = true, double fondale_m = 3000.0)
        : m(file, 1, false), dm(classi, M688_DATI, 1, enhanced ? RegolaDanni::simulazione : RegolaDanni::classica) {
        m.imposta_fondale(std::make_shared<const Fondale>(400, 400, 1000.0, 1.0, -fondale_m, std::vector<std::uint16_t>(400 * 400, 0)));
    }
    void avanza(double s) { for (long i = 0; i < static_cast<long>(s * 9.0); ++i) { m.avanza(1.0 / 9.0); dm.avanza(m, 1.0 / 9.0); } }
    const UnitaClassica& nave() const { return m.unita().at(2); }
};

int quanti_traini(const Missione& m, bool filato) {
    return static_cast<int>(std::count_if(m.eventi_traino().begin(), m.eventi_traino().end(), [&](const Missione::EventoTraino& e) { return e.filato == filato; }));
}

}  // namespace

TEST_CASE("vds: il pesce dell'SQS-35 al cavo, la quota voluta e la velocita' che lo fa risalire" * doctest::skip(!c_e())) {
    const DatiSchiera* d = catalogo().schiera("sqs35_vds");
    REQUIRE(d != nullptr);
    // a 10 nodi, 120 m voluti: il cavo che ce lo porta
    const double c = cavo_per_quota_m(*d, 10.0, 0.0, 120.0);
    CHECK(c > 0.0);
    CHECK(c < d->cavo_max_m);
    CHECK(forma_schiera(*d, 10.0, 0.0, c).quota_centro_m == doctest::Approx(120.0).epsilon(0.02));
    // a 15 nodi i 180 m non ci arrivano nemmeno col cavo intero: il pesce risale
    CHECK(cavo_per_quota_m(*d, 15.0, 0.0, 180.0) == doctest::Approx(d->cavo_max_m));
    const double q15 = forma_schiera(*d, 15.0, 0.0, d->cavo_max_m).quota_centro_m, q5 = forma_schiera(*d, 5.0, 0.0, d->cavo_max_m).quota_centro_m;
    MESSAGE("SQS-35 col cavo intero: a 5 nodi ", q5, " m, a 15 nodi ", q15, " m");
    CHECK(q15 < 150.0);
    CHECK(q5 == doctest::Approx(d->quota_max_m));   // mai oltre i 180 m (Knox-class: 600 ft)
    // il Vega del Kara: mai oltre i 100 m
    CHECK(forma_schiera(*catalogo().schiera("vega_vds"), 3.0, 0.0, 150.0).quota_centro_m <= 100.0 + 1e-9);
    // sopra il punto di traino: niente cavo
    CHECK(cavo_per_quota_m(*d, 10.0, 0.0, -5.0) == 0.0);
}

TEST_CASE("vds: le suite delle navi col sonar fuori scafo (docs/ricerca/33)" * doctest::skip(!c_e())) {
    const auto ha = [](const char* classe, const char* sensore) {
        const auto s = catalogo().sensori_di(classe);
        return std::any_of(s.begin(), s.end(), [&](const Sensore* x) { return x->id == sensore; });
    };
    CHECK(ha("spruance", "sqr19"));
    CHECK(ha("perry", "sqr19"));
    CHECK(ha("ticonderoga", "sqr19"));
    CHECK(ha("knox", "sqs35_vds"));
    CHECK(ha("knox", "sqr18a"));
    CHECK(ha("udaloy", "polinom_vds"));
    CHECK(ha("kara", "vega_vds"));
    CHECK(ha("krivak2", "vega_vds"));
    CHECK(ha("grisha3", "shelon_vds"));
    CHECK(ha("type22b2", "type2031z"));         // la 2031Z solo sul Batch 2 (docs/ricerca/04 §4)
    CHECK_FALSE(ha("type22", "type2031z"));
    CHECK(catalogo().schiera("type2031z") != nullptr);
    // il CSU-83 / DBQS-21 della Type 206 e del Näcken (docs/ricerca/35 §4), al posto del passivo sovietico di ripiego
    CHECK(ha("type206", "csu83"));
    CHECK(ha("nacken", "fas3"));
    CHECK_FALSE(ha("type206", "sovietico_2gen"));
    // le navi senza sonar hanno la suite dichiarata vuota (nessun ripiego: Missione::sensori_acustici)
    for (const char* c : {"nimitz", "kitty_hawk", "iowa", "sacramento", "hauk", "cargo"}) {
        CHECK_MESSAGE(catalogo().suite.count(c) == 1, c);
        CHECK_MESSAGE(catalogo().sensori_di(c).empty(), c);
    }
    CHECK(catalogo().sensore("bronza_vds") != nullptr);   // il Krivak III non e' nel gioco: il sensore c'e'
    // nessuna schiera lunga sovietica di superficie nel 1989
    for (const char* c : {"udaloy", "kara", "krivak2", "grisha3", "slava", "kirov"})
        for (const Sensore* s : catalogo().sensori_di(c)) CHECK_MESSAGE(s->antenna != Antenna::lineare, c);
}

TEST_CASE("vds: sotto lo strato il Horse Tail sente il Victor III che la cupola di prua non sente" * doctest::skip(!c_e())) {
    static const ClimaTeatro clima = carica_clima(M688_DATI "/teatri/faroe.suono.toml");
    Ambiente amb;
    amb.profilo = &clima.profilo(10);
    amb.fondale_m = 3000.0;
    amb.vento_m_s = 7.0;
    amb.traffico = 0.4;
    const double sld = amb.profilo->sld_m;
    REQUIRE(sld > 30.0);
    REQUIRE(sld < 150.0);
    UnitaAcustica u;
    u.id = 1;
    u.classe = "victor3";
    u.categoria = "sottomarino";
    u.quota_m = 250.0;
    u.velocita_kn = 6.0;
    u.pos = {7000.0, 0.0};
    const Sorgente victor = sorgente_acustica(catalogo(), u);
    const auto se = [&](const char* sensore, double quota) {
        Ricevitore r;
        r.quota_m = quota;
        r.velocita_kn = 8.0;
        r.sensore = catalogo().sensore(sensore);
        return valuta(victor, r, amb).se_db;
    };
    const double vds = se("polinom_vds", sld + 30.0), vds_su = se("polinom_vds", 7.0), cupola = se("polinom", 7.0);
    MESSAGE("strato a ", sld, " m; Victor III a 250 m e 7 km: Horse Tail a ", sld + 30.0, " m ", vds, " dB, alla quota dello scafo ", vds_su,
            ", cupola ", cupola);
    CHECK(vds > vds_su + 3.0);    // la quota conta: sotto lo strato
    CHECK(vds > cupola);          // e la cupola grande alla quota dello scafo
}

TEST_CASE("vds nella missione: l'Udaloy cala il Horse Tail sotto lo strato e il registro lo dice" * doctest::skip(!c_e())) {
    const std::string f = scena("udaloy", 10.0, true);
    Partita p(f);
    p.avanza(900.0);
    const auto s = p.m.stato_vds(2);
    REQUIRE(s.has_value());
    const ClimaTeatro clima = carica_clima(M688_DATI "/teatri/faroe.suono.toml");
    const double sld = clima.profilo(10).sld_m;
    MESSAGE("Udaloy a 10 nodi: pesce a ", s->quota_m, " m (voluta ", s->quota_ordinata_m, ", strato ", sld, "), cavo ", s->cavo_filato_m, " m");
    CHECK(s->in_acqua);
    CHECK(s->quota_ordinata_m == doctest::Approx(sld + 30.0));
    CHECK(s->quota_m > sld);
    CHECK(s->quota_m == doctest::Approx(s->quota_ordinata_m).epsilon(0.2));   // con lo sprint and drift la quota va e viene
    CHECK(quanti_traini(p.m, true) >= 1);
    CHECK(std::any_of(p.m.registro().eventi().begin(), p.m.registro().eventi().end(),
                      [](const EventoMissione& e) { return e.tipo == TipoEvento::sonar_traino && e.livello == 1 && e.valore > 0.0; }));
    fs::remove(f);
}

TEST_CASE("vds nella missione: la Spruance fila la SQR-19 e la recupera prima di accelerare oltre la velocita' di traino" * doctest::skip(!c_e())) {
    const std::string f = scena("spruance", 12.0);
    Partita p(f);
    p.avanza(60.0);
    REQUIRE((p.nave().flag54 & 1) != 0);
    const auto s0 = p.m.stato_schiera(2);
    REQUIRE(s0.has_value());
    CHECK(s0->cavo_filato_m > 800.0);   // gia' fuori all'inizio, accorciato per la deriva a 6 nodi (quota massima)
    // lo script vuole 30 nodi: la nave recupera alla velocita' per filare e accelera col cavo dentro
    p.m.trova(2)->velocita_ordinata = 30.0;
    double v_col_cavo = 0.0;
    for (int k = 0; k < 50 * 60 && (p.nave().flag54 & 1); ++k) {
        p.avanza(1.0);
        v_col_cavo = std::max(v_col_cavo, p.nave().velocita);
    }
    CHECK((p.nave().flag54 & 1) == 0);
    MESSAGE("col cavo fuori al piu' ", v_col_cavo, " nodi; recuperato dopo ", p.m.tempo() / 60.0, " minuti");
    CHECK(v_col_cavo <= catalogo().schiera("sqr19")->v_manovra_kn + 0.6);
    CHECK_FALSE(p.m.schiera_persa(2));
    p.avanza(300.0);
    CHECK(p.nave().velocita > 25.0);
    CHECK(quanti_traini(p.m, false) == 1);
    fs::remove(f);
}

TEST_CASE("vds nella missione: sulla Knox la SQR-18A scende dietro il pesce dell'SQS-35" * doctest::skip(!c_e())) {
    const std::string f = scena("knox", 10.0);
    Partita p(f);
    p.avanza(1800.0);
    const auto pesce = p.m.stato_vds(2);
    const auto schiera = p.m.stato_schiera(2);
    REQUIRE(pesce.has_value());
    REQUIRE(schiera.has_value());
    MESSAGE("Knox a 10 nodi: pesce a ", pesce->quota_m, " m e ", pesce->dietro_m, " m dietro; SQR-18A a ", schiera->quota_centro_m,
            " m e ", schiera->dietro_m, " m dietro");
    CHECK(pesce->in_acqua);
    CHECK(schiera->cavo_filato_m > 0.0);
    CHECK(schiera->quota_centro_m > pesce->quota_m);
    CHECK(schiera->dietro_m > pesce->dietro_m);
    CHECK_FALSE(schiera->persa);
    CHECK_FALSE(schiera->oltre_quota_max);
    fs::remove(f);
}

TEST_CASE("vds nella missione: il Grisha ferma cala lo Shelon e lo tira su prima di partire" * doctest::skip(!c_e())) {
    const std::string f = scena("grisha3", 0.0, true);
    Partita p(f);
    p.avanza(300.0);
    const auto s = p.m.stato_vds(2);
    REQUIRE(s.has_value());
    MESSAGE("Grisha ferma: Shelon a ", s->quota_m, " m");
    CHECK(s->in_acqua);
    CHECK(s->quota_m > 20.0);
    p.m.trova(2)->velocita_ordinata = 20.0;
    double v_col_pesce = 0.0;
    for (int k = 0; k < 600 && p.m.stato_vds(2)->in_acqua; ++k) {
        p.avanza(1.0);
        v_col_pesce = std::max(v_col_pesce, p.nave().velocita);
    }
    CHECK_FALSE(p.m.stato_vds(2)->in_acqua);
    CHECK(v_col_pesce <= catalogo().schiera("shelon_vds")->v_manovra_kn + 0.6);
    CHECK_FALSE(p.m.stato_vds(2)->persa);
    fs::remove(f);
}

TEST_CASE("vds nella missione: la Perry fa sprint and drift con la media dello script" * doctest::skip(!c_e())) {
    const std::string f = scena("perry", 12.0);
    Partita p(f);
    p.avanza(60.0);
    REQUIRE((p.nave().flag54 & 1) != 0);
    const double y0 = p.nave().pos.y, t0 = p.m.tempo();
    double vmin = 1e9, vmax = 0.0;
    for (int k = 0; k < 45 * 60; ++k) {
        p.avanza(1.0);
        vmin = std::min(vmin, p.nave().velocita);
        vmax = std::max(vmax, p.nave().velocita);
    }
    const double media = (p.nave().pos.y - y0) / (p.m.tempo() - t0) / 0.514444;
    MESSAGE("Perry a 12 nodi con la SQR-19: fra ", vmin, " e ", vmax, " nodi, media ", media);
    CHECK(vmin < 8.0);
    CHECK(vmax > 14.0);
    CHECK(vmax <= catalogo().schiera("sqr19")->v_max_kn + 0.6);
    CHECK(media == doctest::Approx(12.0).epsilon(0.1));
    // la deriva a 6 nodi non porta la SQR-19 oltre la sua quota massima: il cavo e' accorciato
    const auto s = p.m.stato_schiera(2);
    REQUIRE(s.has_value());
    CHECK_FALSE(s->persa);
    CHECK_FALSE(s->danneggiata);
    CHECK(s->quota_centro_m <= catalogo().schiera("sqr19")->quota_max_m);
    fs::remove(f);
}

// la velocita' voluta che salta (l'accostata del classico: un quarto e indietro ogni pochi secondi, la libreria del
// 30/10/2026) non fa andare il pesce su e giu': un evento per manovra, e si cala di nuovo solo con la voluta stabile
TEST_CASE("vds nella missione: con la voluta che salta la Kara non va su e giu', un evento per manovra" * doctest::skip(!c_e())) {
    const std::string f = scena("kara", 8.0, true);
    Partita p(f);
    p.avanza(120.0);
    REQUIRE(p.m.stato_vds(2).has_value());
    REQUIRE(p.m.stato_vds(2)->in_acqua);
    CHECK(quanti_traini(p.m, true) == 1);   // calato all'inizio, in quota
    for (int k = 0; k < 15 * 60 / 3; ++k) {   // un quarto d'ora: 8 e 25 nodi ogni 3 secondi
        p.m.trova(2)->velocita_ordinata = (k % 2) ? 25.0 : 8.0;
        p.avanza(3.0);
    }
    MESSAGE("dopo il quarto d'ora a scatti: ", quanti_traini(p.m, true), " filato, ", quanti_traini(p.m, false), " recuperato");
    CHECK(quanti_traini(p.m, true) == 1);
    CHECK(quanti_traini(p.m, false) == 1);   // tirato su una volta, e non calato di nuovo
    CHECK_FALSE(p.m.stato_vds(2)->in_acqua);
    // la voluta stabile a 8 nodi: dopo un minuto lo cala di nuovo
    p.m.trova(2)->velocita_ordinata = 8.0;
    p.avanza(600.0);
    CHECK(p.m.stato_vds(2)->in_acqua);
    CHECK(quanti_traini(p.m, true) == 2);
    CHECK(quanti_traini(p.m, false) == 1);
    fs::remove(f);
}

// la rivalutazione del sonarista (nucleo/sonarista, 30/10/2026): la Perry che tiene l'Alfa per mezz'ora con la SQR-19 non
// lo lascia NONSUB per sempre. Dal 03/11/2026 le righe del cavo della schiera sono proprie e la maschera le toglie: della
// sorgente la Perry vede solo 8,5 Hz a 0 dB, quindi senza righe ne' DEMON la traccia resta in analisi (POSSUB senza
// classe, §2.7 di C5). Prima arrivava a «POSSUB» votando le righe del proprio cavo
TEST_CASE("vds nella missione: la Perry che tiene l'Alfa mezz'ora non lo dice NONSUB" * doctest::skip(!c_e())) {
    const auto f = fs::temp_directory_path() / "m688_prova_vds_rivaluta.lua";
    std::ofstream(f) << "return { id = 'PROVA_VDS_RIVALUTA', teatro = 'faroe', lato = 'usa', data = '1989-10-15', ora_utc = 2, paesaggio_sonoro = false,"
                        " script = {}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = -60000, y = -60000 },"
                        "  quota_m = 150, rotta = 0, velocita_kn = 0, " << SUB << " },\n"
                        "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 0,"
                        "  velocita_kn = 8, " << NAVE << " },\n"
                        "{ id = 3, classe = 'alfa', categoria = 'sottomarino', flag = 17, posizione = { x = 2500, y = 0 }, quota_m = 150, rotta = 0,"
                        "  velocita_kn = 8, " << SUB << " },\n"
                        "}, timer = {} } } }\n";
    Partita p(f.string());
    p.avanza(1800.0);
    const Sonarista* s = p.m.sonarista_ia(2);
    REQUIRE(s != nullptr);
    const TracciaSonar* x = s->traccia_di(3);
    REQUIRE(x != nullptr);
    const auto k = p.m.unita().at(2).contatti.find(3);
    MESSAGE("la Perry sull'Alfa dopo mezz'ora: ", x->classe.empty() ? std::string("senza classe") : x->classe, ", confidenza ",
            static_cast<int>(x->confidenza), ", qualita' ", k != p.m.unita().at(2).contatti.end() ? k->second.qualita : -1.0);
    CHECK((x->stato == TracciaSonar::Stato::in_analisi || x->stato == TracciaSonar::Stato::classificata));
    CHECK(static_cast<int>(x->confidenza) >= static_cast<int>(Confidenza::possub));
    fs::remove(f);
}

TEST_CASE("vds nella missione: nel classico niente pesce e niente schiera del modello sulle navi" * doctest::skip(!c_e())) {
    const std::string f = scena("knox", 10.0);
    Partita p(f, false);
    p.avanza(300.0);
    CHECK_FALSE(p.m.stato_vds(2).has_value());
    CHECK_FALSE(p.m.stato_schiera(2).has_value());
    CHECK((p.nave().flag54 & 1) == 0);
    CHECK(p.m.eventi_traino().empty());
    fs::remove(f);
}

TEST_CASE("vds nella missione: in acqua bassa la nave non cala niente" * doctest::skip(!c_e())) {
    const std::string f = scena("udaloy", 10.0, true);
    Partita p(f, true, 40.0);
    p.avanza(600.0);
    const auto s = p.m.stato_vds(2);
    CHECK((!s || !s->in_acqua));
    fs::remove(f);
}
