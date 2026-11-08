// Prove della navigazione stimata (nucleo/navigazione.hpp; roadmap 26, dossier 06 §2, §6 e §7.3)
#include <doctest.h>

#include <cmath>
#include <string>

#include "nucleo/navigazione.hpp"

using namespace m688;

namespace {

constexpr double NM = 1852.0;

const CatalogoNavigazione& cat() {
    static const CatalogoNavigazione c = carica_navigazione(M688_DATI "/navigazione.toml");
    return c;
}

// l'errore radiale rms (nm) di `n` battelli dopo `ore` senza punti nave, e la sigma media che il filtro dichiara
struct Deriva {
    double vero_nm = 0.0, dichiarato_nm = 0.0;
};
Deriva deriva(const std::string& classe, const std::string& teatro, double ore, double latitudine, bool ins = true,
              int n = 200) {
    double q = 0.0, s = 0.0;
    for (int k = 0; k < n; ++k) {
        NavigazioneStimata nav(cat(), classe, teatro, 1000u + static_cast<std::uint64_t>(k), 0.0);
        for (double t = 0.0; t < ore * 3600.0; t += 60.0) nav.avanza(60.0, latitudine, ins);
        const Vec2 e = nav.errore();
        q += e.x * e.x + e.y * e.y;
        s += nav.sigma_m();
    }
    return {std::sqrt(q / n) / NM, s / n / NM};
}

CondizioniPuntoNave a_quota_periscopica() {
    CondizioniPuntoNave c;
    c.teatro = "faroe";
    c.quota_m = 18.0;
    c.velocita_kn = 5.0;
    c.albero = true;
    return c;
}

}  // namespace

TEST_CASE("navigazione: il catalogo e i sistemi per classe") {
    const auto& c = cat();
    CHECK(c.sistemi.size() == 7);
    CHECK(c.punti.size() == 12);
    CHECK(c.teatri.size() == 7);
    CHECK(c.sistema_di("los_angeles").id == "dmins");
    CHECK(c.sistema_di("los_angeles_688i").id == "wsn3");
    CHECK(c.sistema_di("los_angeles_flight3").id == "dmins");   // senza voce: si toglie il suffisso
    CHECK(c.sistema_di("alfa").deriva_nm_giorno == doctest::Approx(3.0));
    CHECK(c.sistema_di("akula").id == "medvedica");
    CHECK(c.sistema_di("kilo").id == "stimata");
    CHECK_FALSE(c.sistema_di("peschereccio").inerziale());     // il predefinito
    CHECK(c.corrente_kn("kunashir") == doctest::Approx(0.8));
    CHECK(c.corrente_kn("sconosciuto", 0.5) == doctest::Approx(0.5));
    for (const auto& [id, p] : c.punti) {
        CHECK_MESSAGE(!p.fonte.empty(), id);
        CHECK_MESSAGE(!p.confidenza.empty(), id);
    }
    for (const auto& [id, s] : c.sistemi) CHECK_MESSAGE(!s.fonte.empty(), id);
}

TEST_CASE("navigazione: le condizioni dei punti nave") {
    const auto& c = cat();
    const DefPuntoNave& transit = *c.punto("transit");
    auto v = valuta_punto_nave(transit, a_quota_periscopica());
    REQUIRE(v.possibile);
    CHECK(v.sigma_nm == doctest::Approx(0.075));
    CHECK(v.albero);
    CHECK(v.durata_s == doctest::Approx(120.0));
    auto x = a_quota_periscopica();
    x.errore_velocita_kn = 0.5;   // l'errore di velocita' propria pesa sul Transit
    CHECK(valuta_punto_nave(transit, x).sigma_nm == doctest::Approx(0.175));
    x = a_quota_periscopica(), x.quota_m = 30.0;
    CHECK(valuta_punto_nave(transit, x).motivo == MotivoPuntoNave::quota);
    x = a_quota_periscopica(), x.velocita_kn = 10.0;
    CHECK(valuta_punto_nave(transit, x).motivo == MotivoPuntoNave::velocita);
    x = a_quota_periscopica(), x.albero = false;
    CHECK(valuta_punto_nave(transit, x).motivo == MotivoPuntoNave::albero);
    x = a_quota_periscopica(), x.marina = "vmf";
    CHECK(valuta_punto_nave(transit, x).motivo == MotivoPuntoNave::marina);
    CHECK(valuta_punto_nave(*c.punto("parus"), x).possibile);
    x.marina = "rn";   // le marine occidentali usano i sistemi della NATO
    CHECK(valuta_punto_nave(transit, x).possibile);

    // GPS: solo con l'opzione, e dal 1990
    x = a_quota_periscopica();
    CHECK(valuta_punto_nave(*c.punto("gps"), x).motivo == MotivoPuntoNave::anno);
    x.anno = 1990;
    CHECK(valuta_punto_nave(*c.punto("gps"), x).motivo == MotivoPuntoNave::opzione);
    x.gps = true;
    CHECK(valuta_punto_nave(*c.punto("gps"), x).possibile);

    // copertura del Loran-C
    x = a_quota_periscopica();
    CHECK(valuta_punto_nave(*c.punto("loran_c"), x).possibile);
    x.teatro = "gibilterra";
    CHECK(valuta_punto_nave(*c.punto("loran_c"), x).motivo == MotivoPuntoNave::copertura);

    // Omega col filo a 50 m, l'astronomico col cielo e la notte, il radar con la costa
    x = a_quota_periscopica(), x.quota_m = 50.0, x.albero = false;
    CHECK(valuta_punto_nave(*c.punto("omega"), x).motivo == MotivoPuntoNave::filo);
    x.filo = true;
    CHECK(valuta_punto_nave(*c.punto("omega"), x).possibile);
    CHECK(valuta_punto_nave(*c.punto("omega"), x).corsia_nm == doctest::Approx(8.0));
    x = a_quota_periscopica();
    CHECK(valuta_punto_nave(*c.punto("astronomico"), x).motivo == MotivoPuntoNave::notte);
    x.notte = true, x.sereno = false;
    CHECK(valuta_punto_nave(*c.punto("astronomico"), x).motivo == MotivoPuntoNave::cielo);
    x.sereno = true;
    CHECK(valuta_punto_nave(*c.punto("astronomico"), x).possibile);
    // i fari (il kit, fari.toml): di notte, con almeno due luci in vista
    x = a_quota_periscopica(), x.notte = true, x.fari_in_vista = 1;
    REQUIRE(c.punto("fari") != nullptr);
    CHECK(valuta_punto_nave(*c.punto("fari"), x).motivo == MotivoPuntoNave::fari);
    x.fari_in_vista = 2;
    CHECK(valuta_punto_nave(*c.punto("fari"), x).possibile);
    x.notte = false;
    CHECK(valuta_punto_nave(*c.punto("fari"), x).motivo == MotivoPuntoNave::notte);
    x = a_quota_periscopica(), x.distanza_costa_km = 50.0;
    CHECK(valuta_punto_nave(*c.punto("radar_costa"), x).motivo == MotivoPuntoNave::costa);
    x.distanza_costa_km = 30.0;
    v = valuta_punto_nave(*c.punto("radar_costa"), x);
    CHECK(v.possibile);
    CHECK(v.emissione);

    // batimetrico: un quarto della spaziatura delle isobate, piu' debole lungo l'isobata, con l'errore della carta
    x = CondizioniPuntoNave{};
    x.quota_m = 200.0, x.spaziatura_isobate_nm = 1.0, x.direzione_debole_deg = 30.0, x.errore_carta_nm = 0.5;
    v = valuta_punto_nave(*c.punto("batimetrico"), x);
    REQUIRE(v.possibile);
    CHECK(v.sigma_nm == doctest::Approx(0.25));
    CHECK(v.sigma_debole_nm == doctest::Approx(0.75));
    CHECK(v.sistematico_nm == doctest::Approx(0.5));
    CHECK(v.emissione);
    CHECK_FALSE(v.albero);
    CHECK(valuta_punto_nave(transit, a_quota_periscopica()).sistematico_nm == 0.0);   // il Transit non usa la carta
}

TEST_CASE("navigazione: i passaggi dei satelliti") {
    const DefPuntoNave& transit = *cat().punto("transit");
    CHECK(attesa_satellite_min(transit, 30.0) == doctest::Approx(100.0));
    CHECK(attesa_satellite_min(transit, 50.0) == doctest::Approx(80.0));
    CHECK(attesa_satellite_min(transit, -75.0) == doctest::Approx(40.0));
    CHECK(attesa_satellite_min(*cat().punto("omega"), 50.0) < 0.0);
    // deterministico, sempre dopo l'istante chiesto, e piu' frequente a nord
    CHECK(prossimo_passaggio_s(transit, 5000.0, 62.0, 7) == prossimo_passaggio_s(transit, 5000.0, 62.0, 7));
    double attesa_40 = 0.0, attesa_70 = 0.0;
    int n = 0;
    for (double t = 0.0; t < 30.0 * 86400.0; t += 3637.0, ++n) {
        const double p40 = prossimo_passaggio_s(transit, t, 40.0, 11), p70 = prossimo_passaggio_s(transit, t, 70.0, 11);
        CHECK(p40 >= t);
        attesa_40 += p40 - t, attesa_70 += p70 - t;
    }
    attesa_40 /= n * 60.0, attesa_70 /= n * 60.0;
    MESSAGE("attesa media del Transit: " << attesa_40 << " min a 40°, " << attesa_70 << " min a 70°");
    CHECK(attesa_70 < attesa_40);
    CHECK(attesa_40 > 40.0);
    CHECK(attesa_40 < 120.0);
}

TEST_CASE("navigazione: la deriva per sistema dopo un giorno") {
    // l'errore vero e quello che il filtro dichiara crescono insieme come la deriva del sistema
    const Deriva dmins = deriva("los_angeles", "faroe", 24.0, 62.0);
    const Deriva wsn3 = deriva("los_angeles_688i", "faroe", 24.0, 62.0);
    const Deriva soz = deriva("alfa", "barents", 24.0, 72.0);
    MESSAGE("DMINS " << dmins.vero_nm << " (" << dmins.dichiarato_nm << "), WSN-3 " << wsn3.vero_nm << " ("
                     << wsn3.dichiarato_nm << "), Soz " << soz.vero_nm << " (" << soz.dichiarato_nm << ") nm");
    CHECK(dmins.dichiarato_nm == doctest::Approx(1.5).epsilon(0.05));
    CHECK(dmins.vero_nm == doctest::Approx(1.5).epsilon(0.2));
    CHECK(wsn3.vero_nm == doctest::Approx(0.5).epsilon(0.2));   // con Schuler sopra: 0,15 nm
    CHECK(soz.vero_nm == doctest::Approx(3.0).epsilon(0.2));
    // oltre 80°N la deriva raddoppia
    const Deriva polo = deriva("los_angeles", "barents", 24.0, 85.0);
    CHECK(polo.vero_nm == doctest::Approx(3.0).epsilon(0.2));
    CHECK(polo.dichiarato_nm == doctest::Approx(3.0).epsilon(0.05));
}

TEST_CASE("navigazione: a inerziale guasto la corrente del teatro") {
    // 6 ore a stima: l'errore e' circa la corrente per il tempo (un po' meno: la corrente cambia in 12 ore)
    const Deriva kuna = deriva("los_angeles", "kunashir", 6.0, 44.5, false);
    const Deriva balt = deriva("kilo", "baltico", 6.0, 56.0);
    MESSAGE("a stima 6 h: Kunashir " << kuna.vero_nm << " (" << kuna.dichiarato_nm << "), Baltico " << balt.vero_nm << " nm");
    CHECK(kuna.vero_nm == doctest::Approx(0.8 * 6.0 * 0.92).epsilon(0.2));
    CHECK(kuna.dichiarato_nm == doctest::Approx(kuna.vero_nm).epsilon(0.2));
    CHECK(balt.vero_nm == doctest::Approx(0.2 * 6.0 * 0.92).epsilon(0.2));
    // l'inerziale e' molto meglio della stima nello stesso teatro
    CHECK(deriva("los_angeles", "kunashir", 6.0, 44.5).vero_nm < kuna.vero_nm / 5.0);
}

TEST_CASE("navigazione: un punto nave riporta la stima e l'ellisse a posto") {
    const auto v = valuta_punto_nave(*cat().punto("transit"), a_quota_periscopica());
    // NEES: l'errore vero al quadrato sulla varianza dichiarata, 1 se il filtro crede quanto sbaglia
    auto prova = [&](const std::string& classe, double& rms, double& nees) {
        double q = 0.0, r = 0.0;
        const int n = 300;
        for (int k = 0; k < n; ++k) {
            NavigazioneStimata nav(cat(), classe, "faroe", 77u + static_cast<std::uint64_t>(k));
            for (int i = 0; i < 24 * 60; ++i) nav.avanza(60.0, 62.0);
            const double prima = nav.sigma_m();
            nav.punto_nave(v);
            CHECK(nav.sigma_m() < prima / 5.0);
            const Vec2 e = nav.errore();
            const EllisseErrore el = nav.ellisse();
            q += e.x * e.x + e.y * e.y;
            r += (e.x * e.x + e.y * e.y) / (el.maggiore_m * el.maggiore_m + el.minore_m * el.minore_m);
        }
        rms = std::sqrt(q / n) / NM, nees = r / n;
    };
    double rms = 0.0, nees = 0.0;
    prova("kilo", rms, nees);   // a stima, senza Schuler: il filtro e' coerente
    MESSAGE("dopo il Transit, a stima: " << rms << " nm rms, NEES " << nees);
    CHECK(rms < 0.15);
    CHECK(nees > 0.85);
    CHECK(nees < 1.15);
    // a inerziale l'ellisse tiene in media l'oscillazione di Schuler (0,15 nm), che il fix ha appena tolto: subito
    // dopo il filtro e' un po' pessimista, mai troppo sicuro
    prova("los_angeles", rms, nees);
    MESSAGE("dopo il Transit, DMINS: " << rms << " nm rms, NEES " << nees);
    CHECK(rms < 0.15);
    CHECK(nees > 0.4);
    CHECK(nees < 1.15);
}

TEST_CASE("navigazione: due punti nave stimano anche la deriva") {
    NavigazioneStimata nav(cat(), "alfa", "barents", 5);
    const auto c = a_quota_periscopica();
    CondizioniPuntoNave cs = c;
    cs.marina = "vmf", cs.teatro = "barents";
    const auto v = valuta_punto_nave(*cat().punto("parus"), cs);
    REQUIRE(v.possibile);
    const double v0 = nav.sigma_velocita_kn();
    for (int i = 0; i < 12 * 60; ++i) nav.avanza(60.0, 72.0);
    nav.punto_nave(v);
    for (int i = 0; i < 12 * 60; ++i) nav.avanza(60.0, 72.0);
    nav.punto_nave(v);
    MESSAGE("errore di velocita': " << v0 << " -> " << nav.sigma_velocita_kn() << " kn");
    CHECK(nav.sigma_velocita_kn() < v0 * 0.5);
}

TEST_CASE("navigazione: il batimetrico allunga l'ellisse e la carta degradata inganna") {
    CondizioniPuntoNave c;
    c.quota_m = 150.0, c.spaziatura_isobate_nm = 2.0, c.direzione_debole_deg = 45.0;
    const auto buono = valuta_punto_nave(*cat().punto("batimetrico"), c);
    NavigazioneStimata nav(cat(), "kilo", "baltico", 3);
    for (int i = 0; i < 24 * 60; ++i) nav.avanza(60.0, 56.0);
    nav.punto_nave(buono);
    const EllisseErrore el = nav.ellisse();
    MESSAGE("ellisse dopo il batimetrico: " << el.maggiore_m << " x " << el.minore_m << " m, " << el.orientamento_deg << "°");
    CHECK(el.maggiore_m > 2.0 * el.minore_m);
    CHECK(el.orientamento_deg == doctest::Approx(45.0).epsilon(0.05));

    // la carta sbaglia di 1 nm: la stima va dove dice la carta, e il filtro se ne fida
    c.errore_carta_nm = 1.0, c.spaziatura_isobate_nm = 0.4;
    const auto cattivo = valuta_punto_nave(*cat().punto("batimetrico"), c);
    double q = 0.0, s = 0.0;
    const int n = 100;
    for (int k = 0; k < n; ++k) {
        NavigazioneStimata x(cat(), "kilo", "baltico", 900u + static_cast<std::uint64_t>(k));
        for (int i = 0; i < 6 * 60; ++i) x.avanza(60.0, 56.0);
        x.punto_nave(cattivo);
        const Vec2 e = x.errore();
        q += e.x * e.x + e.y * e.y;
        s += x.sigma_m();
    }
    MESSAGE("carta degradata: errore " << std::sqrt(q / n) / NM << " nm, dichiarato " << s / n / NM << " nm");
    CHECK(std::sqrt(q / n) / NM > 0.8);
    CHECK(s / n / NM < 0.4);
}

TEST_CASE("navigazione: l'Omega sbaglia corsia se la stima e' troppo lontana") {
    CondizioniPuntoNave c;
    c.teatro = "faroe", c.quota_m = 40.0, c.filo = true, c.qualita = 0.0;
    const auto omega = valuta_punto_nave(*cat().punto("omega"), c);
    REQUIRE(omega.possibile);
    auto prova = [&](double ore, const std::string& teatro) {
        int sbagliati = 0;
        const int n = 200;
        for (int k = 0; k < n; ++k) {
            NavigazioneStimata nav(cat(), "kilo", teatro, 4000u + static_cast<std::uint64_t>(k));
            for (int i = 0; i < static_cast<int>(ore * 60.0); ++i) nav.avanza(60.0, 50.0);
            nav.punto_nave(omega);
            const Vec2 e = nav.errore();
            if (std::hypot(e.x, e.y) > 6.0 * NM) ++sbagliati;   // ben oltre l'errore dell'Omega: una corsia sbagliata
        }
        return static_cast<double>(sbagliati) / n;
    };
    const double vicino = prova(3.0, "baltico"), lontano = prova(18.0, "gibilterra");
    MESSAGE("corsia sbagliata: " << vicino * 100 << "% dopo 3 h nel Baltico, " << lontano * 100 << "% dopo 18 h a Gibilterra");
    CHECK(vicino < 0.03);
    CHECK(lontano > 0.3);
}

TEST_CASE("navigazione: deterministica dal seme, e il cambio di modo") {
    NavigazioneStimata a(cat(), "akula", "barents", 42), b(cat(), "akula", "barents", 42);
    for (int i = 0; i < 500; ++i) a.avanza(30.0, 70.0, i < 250), b.avanza(30.0, 70.0, i < 250);
    CHECK(a.errore().x == b.errore().x);
    CHECK(a.errore().y == b.errore().y);
    CHECK_FALSE(a.a_inerziale());
    a.avanza(30.0, 70.0, true);
    CHECK(a.a_inerziale());
    NavigazioneStimata kilo(cat(), "kilo", "barents", 1);
    CHECK_FALSE(kilo.a_inerziale());
    kilo.avanza(60.0, 70.0, true);   // senza inerziale il flag non conta
    CHECK_FALSE(kilo.a_inerziale());
    // la posizione stimata e' la vera piu' l'errore
    const Vec2 v{1000.0, 2000.0}, s = a.stimata(v);
    CHECK(s.x - v.x == doctest::Approx(a.errore().x));
    CHECK(s.y - v.y == doctest::Approx(a.errore().y));
}
