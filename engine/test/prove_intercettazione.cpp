#include <doctest.h>

#include <algorithm>
#include <cmath>

#include "nucleo/intercettazione.hpp"
#include "nucleo/transitori_armi.hpp"

using namespace m688;

// Il ricevitore d'intercettazione e l'allarme siluro (blocco 2 del sonar, E; docs/22_sonar_blocco2.md §2)

namespace {

const CatalogoAcustico& acustica() {
    static const CatalogoAcustico c = carica_acustica(M688_DATI "/acustica.toml");
    return c;
}
const CatalogoIntercettazione& catalogo() {
    static const CatalogoIntercettazione c = carica_intercettazione(M688_DATI "/acustica.toml");
    return c;
}
Ambiente calmo() {
    Ambiente a;
    a.fondale_m = 3000.0;
    a.vento_m_s = 4.0;
    a.traffico = 0.4;
    return a;
}
Ricevitore il_688(const char* sensore = "wlr9") { return Ricevitore{{0.0, 0.0}, 150.0, 0.0, 5.0, acustica().sensore(sensore)}; }

// una testa che parte a `da` metri e corre a 45 kn verso il punto `verso` (o via), coi ping alla cadenza della missione
struct Corsa {
    std::vector<Allarme> allarmi;
    std::vector<double> distanze;
    std::vector<int> doppler;
};
Corsa corsa(RicevitoreIntercettazione& ric, double da, bool avvicina, double fino_a) {
    Corsa c;
    const Ricevitore r = il_688();
    double prossimo = 0.0;
    for (double t = 0.0; t < 400.0; t += 0.1) {
        const double d = avvicina ? da - 45.0 * NODO * t : da + 45.0 * NODO * t;
        if (d < fino_a) break;
        std::vector<EmissioneAttiva> e;
        if (t >= prossimo) {
            EmissioneAttiva x;
            x.emettitore = 7;
            x.chiave = "ping_testa_mk48";
            x.pos = {0.0, d};
            x.quota_m = 150.0;
            x.rotta = avvicina ? 180.0 : 0.0;
            x.velocita_kn = 45.0;
            x.t = t;
            x.sl_db = 210.0;
            e.push_back(x);
            prossimo = t + intervallo_ping_testa_s(d, 1500.0);
        }
        ric.ascolta(e, r, calmo(), t, 0.1);
        c.allarmi.push_back(ric.allarme());
        c.distanze.push_back(d);
        if (!ric.tracce().empty()) c.doppler.push_back(ric.tracce().front().doppler);
    }
    return c;
}

}  // namespace

TEST_CASE("intercettazione: il catalogo dei ricevitori e delle emissioni") {
    REQUIRE(catalogo().intercettatore_di("los_angeles") != nullptr);
    CHECK(catalogo().intercettatore_di("los_angeles")->id == "wlr9");
    CHECK(catalogo().intercettatore_di("victor3")->id == "skat_ogs");
    CHECK(catalogo().intercettatore_di("alfa")->id == "rubin");
    CHECK(catalogo().intercettatore_di("perry") == nullptr);
    REQUIRE(catalogo().emissione("ping_testa_mk48") != nullptr);
    CHECK(catalogo().emissione("ping_testa_mk48")->tipo == TipoEmissione::testa);
    CHECK(catalogo().emissione("ping_testa_mk48")->f_hz == doctest::Approx(30000.0));
    CHECK(catalogo().emissione("sqs53_attivo")->fm);
}

TEST_CASE("intercettazione: il sonar di una fregata a 20 km, misurato e classificato, senza allarme") {
    RicevitoreIntercettazione ric(*catalogo().intercettatore_di("los_angeles"), catalogo(), 1);
    std::vector<Intercetto> tutti;
    for (int k = 0; k < 4; ++k) {
        EmissioneAttiva x;
        x.emettitore = 3;
        x.chiave = "sqs53_attivo";
        x.pos = {14000.0, 14000.0};   // a 045, 19,8 km
        x.quota_m = 7.0;
        x.t = 25.0 * k;
        x.sl_db = 235.0;
        for (const auto& i : ric.ascolta({x}, il_688(), calmo(), x.t, 25.0)) tutti.push_back(i);
    }
    REQUIRE(tutti.size() == 4);
    for (const Intercetto& i : tutti) {
        CHECK(i.banda == 0);
        CHECK(i.f_hz == doctest::Approx(3300.0).epsilon(0.05));
        CHECK(i.durata_s == doctest::Approx(1.5).epsilon(0.2));
        CHECK(std::fabs(normalizza_180(i.rilevamento - 45.0)) < 18.0);
        CHECK(i.fm == -1);   // il WLR-9 non distingue CW e FM (STIMA)
    }
    REQUIRE(ric.tracce().size() == 1);
    const TracciaIntercetto& tr = ric.tracce().front();
    CHECK(tr.classe == TipoEmissione::scafo);
    CHECK(tr.pri_s == doctest::Approx(25.0).epsilon(0.01));
    CHECK(ric.allarme() == Allarme::nessuno);
    // fuori dalle bande niente
    EmissioneAttiva basso;
    basso.chiave = "sqs53_attivo";
    basso.f_hz = 500.0;
    basso.pos = {1000.0, 0.0};
    basso.sl_db = 235.0;
    CHECK(ric.ascolta({basso}, il_688(), calmo(), 200.0, 1.0).empty());
}

TEST_CASE("intercettazione: una testa che arriva, SILURO IN ACQUA e poi IN ACQUISIZIONE; una che se ne va no") {
    RicevitoreIntercettazione ric(*catalogo().intercettatore_di("los_angeles"), catalogo(), 2);
    const Corsa c = corsa(ric, 4000.0, true, 200.0);
    const auto primo = [&](Allarme a) {
        for (std::size_t i = 0; i < c.allarmi.size(); ++i)
            if (c.allarmi[i] == a) return c.distanze[i];
        return -1.0;
    };
    const double in_acqua = primo(Allarme::siluro_in_acqua), acquisizione = primo(Allarme::siluro_in_acquisizione);
    MESSAGE("in acqua a " << in_acqua << " m, in acquisizione a " << acquisizione << " m");
    CHECK(in_acqua > 3000.0);                // la testa si sente da lontano
    CHECK(acquisizione > 300.0);             // l'acquisizione arriva prima dell'impatto,
    CHECK(acquisizione < 1600.0);            // dopo che la testa ha cominciato a stringere
    CHECK(std::count(c.doppler.begin(), c.doppler.end(), 1) > static_cast<long>(c.doppler.size()) / 2);   // si avvicina
    // la stessa testa che si allontana: in acqua, mai in acquisizione, Doppler negativo
    RicevitoreIntercettazione via(*catalogo().intercettatore_di("los_angeles"), catalogo(), 3);
    const Corsa v = corsa(via, 600.0, false, 0.0);
    CHECK(std::find(v.allarmi.begin(), v.allarmi.end(), Allarme::siluro_in_acquisizione) == v.allarmi.end());
    CHECK(std::find(v.allarmi.begin(), v.allarmi.end(), Allarme::siluro_in_acqua) != v.allarmi.end());
    CHECK(std::count(v.doppler.begin(), v.doppler.end(), -1) > static_cast<long>(v.doppler.size()) / 2);
}

TEST_CASE("intercettazione: lo Skat misura CW e FM, il Rubin no e ha i falsi allarmi") {
    EmissioneAttiva x;
    x.emettitore = 4;
    x.chiave = "sqs53_attivo";
    x.pos = {0.0, 15000.0};
    x.quota_m = 7.0;
    x.sl_db = 235.0;
    RicevitoreIntercettazione skat(catalogo().intercettatori.at("skat_ogs"), catalogo(), 5), rubin(catalogo().intercettatori.at("rubin"), catalogo(), 5);
    Ricevitore r{{0.0, 0.0}, 150.0, 0.0, 5.0, acustica().sensore("intercettazione_sov")};
    const auto a = skat.ascolta({x}, r, calmo(), 0.0, 1.0), b = rubin.ascolta({x}, r, calmo(), 0.0, 1.0);
    REQUIRE(a.size() == 1);
    REQUIRE(b.size() == 1);
    CHECK(a.front().fm == 1);
    CHECK(a.front().durata_s > 0.0);
    CHECK(b.front().fm == -1);
    CHECK(b.front().durata_s < 0.0);
    // dieci ore di mare vuoto: il Rubin ne inventa circa 2 all'ora, il WLR-9 quasi nessuno
    RicevitoreIntercettazione r2(catalogo().intercettatori.at("rubin"), catalogo(), 9), w(*catalogo().intercettatore_di("los_angeles"), catalogo(), 9);
    int falsi_r = 0, falsi_w = 0;
    for (double t = 0.0; t < 36000.0; t += 1.0) {
        for (const auto& i : r2.ascolta({}, r, calmo(), t, 1.0)) falsi_r += i.spurio;
        for (const auto& i : w.ascolta({}, il_688(), calmo(), t, 1.0)) falsi_w += i.spurio;
    }
    MESSAGE("falsi allarmi in 10 h: Rubin " << falsi_r << ", WLR-9 " << falsi_w);
    CHECK(falsi_r >= 10);
    CHECK(falsi_r <= 32);
    CHECK(falsi_w <= 4);
}

TEST_CASE("intercettazione: ripetibile dal seme") {
    const auto prova = [](std::uint64_t seme) {
        RicevitoreIntercettazione ric(*catalogo().intercettatore_di("los_angeles"), catalogo(), seme);
        EmissioneAttiva x;
        x.emettitore = 3;
        x.chiave = "aqs13b";
        x.pos = {3000.0, 2000.0};
        x.quota_m = 60.0;
        x.sl_db = 211.0;
        x.t = 10.0;
        return ric.ascolta({x}, il_688(), calmo(), 10.0, 1.0);
    };
    const auto a = prova(11), b = prova(11), c = prova(12);
    REQUIRE(a.size() == 1);
    CHECK(a.front().rilevamento == b.front().rilevamento);
    CHECK(a.front().f_hz == b.front().f_hz);
    CHECK(a.front().rilevamento != c.front().rilevamento);
    // il sonar a immersione si riconosce
    RicevitoreIntercettazione ric(*catalogo().intercettatore_di("los_angeles"), catalogo(), 1);
    for (int k = 0; k < 3; ++k) {
        EmissioneAttiva x;
        x.emettitore = 3;
        x.chiave = "aqs13b";
        x.pos = {3000.0, 2000.0};
        x.quota_m = 60.0;
        x.sl_db = 211.0;
        x.t = 8.0 * k;
        ric.ascolta({x}, il_688(), calmo(), x.t, 8.0);
    }
    REQUIRE(ric.tracce().size() == 1);
    CHECK(ric.tracce().front().classe == TipoEmissione::immersione);
}

TEST_CASE("intercettazione: una boa attiva non e' mai una testa per un ricevitore che misura la durata; il Rubin si'") {
    // il battello a 6 kn passa a 200 m da una SSQ-47B (16 kHz come le teste, ma 0,1 s ogni 10 s), per 20 minuti
    const auto passa = [](const char* id, const char* sensore, std::uint64_t seme) {
        RicevitoreIntercettazione ric(catalogo().intercettatori.at(id), catalogo(), seme);
        Ricevitore r{{0.0, 0.0}, 100.0, 0.0, 6.0, acustica().sensore(sensore)};
        Allarme peggiore = Allarme::nessuno;
        double prossimo = 0.0;
        for (double t = 0.0; t < 1200.0; t += 0.5) {
            r.pos = {200.0, -3600.0 + 6.0 * NODO * t};
            std::vector<EmissioneAttiva> e;
            if (t >= prossimo) {
                EmissioneAttiva x;
                x.emettitore = 9;
                x.chiave = "ssq47b";
                x.pos = {0.0, 0.0};
                x.quota_m = 60.0;
                x.t = t;
                x.sl_db = 205.0;
                e.push_back(x);
                prossimo = t + 10.0;
            }
            ric.ascolta(e, r, calmo(), t, 0.5);
            for (const TracciaIntercetto& tr : ric.tracce())
                if (!tr.intercetti.empty() && tr.intercetti.back().emettitore == 9 && static_cast<int>(tr.allarme) > static_cast<int>(peggiore)) peggiore = tr.allarme;
        }
        return peggiore;
    };
    for (std::uint64_t seme = 1; seme <= 5; ++seme) {
        CHECK(passa("wlr9", "wlr9", seme) == Allarme::nessuno);
        CHECK(passa("skat_ogs", "intercettazione_sov", seme) == Allarme::nessuno);
        CHECK(passa("rubikon", "intercettazione_sov", seme) == Allarme::nessuno);
    }
    // il Rubin vede solo la frequenza: SILURO IN ACQUA, e da vicino anche l'acquisizione (il falso allarme voluto)
    CHECK(passa("rubin", "intercettazione_sov", 1) != Allarme::nessuno);
}
