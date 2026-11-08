#include "nucleo/navi_civili.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>

#include "nucleo/casuale.hpp"

namespace m688 {

namespace {

// JOMOPANS-ECHO (MacGillivray e de Jong 2021, eq. 9-11): spettro base e picco delle classi da carico sotto 100 Hz
constexpr double ECHO_K = 191.0, ECHO_D = 3.0, ECHO_K_LF = 208.0, ECHO_L0_M = 91.44;
constexpr double SCARTO_INDIVIDUO_DB = 4.0, SCARTO_MAX_DB = 8.0;   // STIMA (ECHO: 6 dB comprese le misure)
constexpr double SOTTO_CAVITAZIONE_DB = 6.0;                       // STIMA
// La velocita': il livello di ECHO alla velocita' della sua classe (V_C, la media delle navi misurate) e' quello della
// nave alla sua velocita' di servizio; attorno, 40 log V (STIMA: ECHO e RANDI 60 log V sulla flotta, Veirs 2016 circa
// 1 dB/kn per nave)
constexpr double PENDENZA_VELOCITA_DB = 40.0;
constexpr double MINIMO_MACCHINE_DB = 30.0;                        // il livello non scende oltre: macchine e flusso
constexpr double TONALE_SOTTO_LARGA_DB = 35.0;   // la riga piu' forte sotto la banda larga di servizio (come le firme civili, 17 dB piu' giu' dal punto 1 della portata)
// forza di bersaglio: il mercantile di riferimento (firme.mercantile, 170 m) e la scala con la lunghezza (STIMA)
constexpr double TS_TRAVERSO_170_DB = 38.0, TS_PRUA_SOTTO_DB = 13.0;

double densita_echo(const TipoCivile& t, double f) {
    const double vc = std::max(t.echo_vc_kn, 1.0);
    if (t.echo_d_lf > 0.0 && f < 100.0) {
        const double f1 = 600.0 / vc, x = 1.0 - (f / f1) * (f / f1);
        return ECHO_K_LF - 40.0 * std::log10(f1) + 10.0 * std::log10(f) - 10.0 * std::log10(x * x + t.echo_d_lf * t.echo_d_lf);
    }
    const double f1 = 480.0 / vc, x = 1.0 - f / f1;
    return ECHO_K - 20.0 * std::log10(f1) - 10.0 * std::log10(x * x + ECHO_D * ECHO_D);
}

std::uint64_t mescola(std::uint64_t z) {
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ull;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebull;
    return z ^ (z >> 31);
}

}  // namespace

Propulsione propulsione_da_testo(const std::string& testo) {
    if (testo == "diesel_lento") return Propulsione::diesel_lento;
    if (testo == "diesel_medio") return Propulsione::diesel_medio;
    if (testo == "diesel_veloce") return Propulsione::diesel_veloce;
    if (testo == "diesel_elettrico") return Propulsione::diesel_elettrico;
    if (testo == "turbina") return Propulsione::turbina;
    throw std::runtime_error("propulsione ignota '" + testo + "'");
}

double livello_echo_db(const TipoCivile& t, double v, double l) {
    // integrale della densita' su 5-200 Hz (in log f), poi velocita' e lunghezza
    constexpr int PASSI = 200;
    const double a = std::log(5.0), b = std::log(200.0), h = (b - a) / PASSI;
    double somma = 0.0;
    for (int i = 0; i < PASSI; ++i) {
        const double f = std::exp(a + (i + 0.5) * h);
        somma += std::pow(10.0, densita_echo(t, f) / 10.0) * f * h;
    }
    return 10.0 * std::log10(somma) + 60.0 * std::log10(std::max(v, 0.1) / std::max(t.echo_vc_kn, 1.0)) +
           20.0 * std::log10(std::max(l, 1.0) / ECHO_L0_M) + t.epoca_db;
}

IndividuoCivile genera_nave_civile(const TipoCivile& t, std::uint64_t seme) {
    Casuale caso(mescola(seme ^ 0xC1F1ull));
    const auto in = [&](const Intervallo& x) { return caso.uniforme(x.min, std::max(x.max, x.min)); };
    const auto intero = [&](const Intervallo& x) {
        const int a = static_cast<int>(std::lround(x.min)), b = std::max(a, static_cast<int>(std::lround(x.max)));
        return a + static_cast<int>(std::min<std::uint64_t>(caso.prossimo() % static_cast<std::uint64_t>(b - a + 1), static_cast<std::uint64_t>(b - a)));
    };
    const auto normale = [&] {
        const double u1 = std::max(caso.uniforme(), 1e-300), u2 = caso.uniforme();
        return std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * PI * u2);
    };

    IndividuoCivile n;
    n.lunghezza_m = in(t.lunghezza_m);
    n.larghezza_m = n.lunghezza_m * t.larghezza_rel;
    n.pescaggio_m = in(t.pescaggio_m);
    n.velocita_servizio_kn = in(t.velocita_servizio_kn);
    n.velocita_max_kn = n.velocita_servizio_kn * t.velocita_max_rel;
    const double cis = in(t.cavitazione_kn);
    const double scarto = std::clamp(SCARTO_INDIVIDUO_DB * normale(), -SCARTO_MAX_DB, SCARTO_MAX_DB);

    FirmaRumore& f = n.firma;
    f.classe = t.id;
    f.nave = true;
    f.nazionalita = "civile";
    f.tipo_unita = t.categoria;
    f.fonte = "generata dal tipo civile " + t.id + " (" + t.fonte + ")";
    f.v_cavitazione_kn = cis;
    f.v_quieto_kn = std::min(cis, n.velocita_servizio_kn);
    // il livello per velocita': ECHO alla V_C come livello di servizio, 6 dB in meno sotto l'innesco, mai oltre 30 dB
    // sotto il servizio
    const double servizio = livello_echo_db(t, t.echo_vc_kn, n.lunghezza_m) + scarto;
    const auto livello = [&](double v) {
        const double l = servizio + PENDENZA_VELOCITA_DB * std::log10(v / n.velocita_servizio_kn) - (v < cis ? SOTTO_CAVITAZIONE_DB : 0.0);
        return std::max(l, servizio - MINIMO_MACCHINE_DB);
    };
    std::vector<double> velocita = {2.0, 0.5 * cis, 0.98 * cis, cis, n.velocita_servizio_kn, n.velocita_max_kn};
    std::sort(velocita.begin(), velocita.end());
    for (double v : velocita)
        if (f.punti.empty() || v > f.punti.back().first * 1.001) f.punti.emplace_back(v, livello(v));
    f.sl_quieto_db = f.punti.front().second;
    f.tonale_db = servizio - TONALE_SOTTO_LARGA_DB;
    f.ts_traverso_db = TS_TRAVERSO_170_DB + 20.0 * std::log10(n.lunghezza_m / 170.0);
    f.ts_prua_db = f.ts_traverso_db - TS_PRUA_SOTTO_DB;

    // elica
    f.assi = intero(t.assi);
    f.pale = intero(t.pale);
    const double giri_asse = in(t.giri_asse);
    if (t.passo_variabile) f.giri_costanti = giri_asse;
    else f.giri_per_nodo = giri_asse / n.velocita_servizio_kn;
    const double asse_hz = giri_asse / 60.0;   // al servizio, per le righe legate all'asse
    const auto legata = [&](double multiplo, double db, TipoRiga tipo) {
        RigaFirma r;
        r.hz = multiplo * asse_hz;
        r.db_relativo = db;
        r.tipo = tipo;
        if (!t.passo_variabile) r.multiplo_asse = multiplo;   // col passo variabile l'asse gira costante: riga fissa
        f.righe.push_back(r);
    };
    const auto fissa = [&](double hz, double db, TipoRiga tipo) {
        RigaFirma r;
        r.hz = hz;
        r.db_relativo = db;
        r.tipo = tipo;
        f.righe.push_back(r);
    };

    // la rete e i generatori a giri fissi (la riga piu' forte: le accensioni del primo generatore)
    const double rete = caso.uniforme() < t.rete_60 ? 60.0 : 50.0;
    for (const auto& [k, db] : {std::pair{1.0, -6.0}, {2.0, -3.0}, {3.0, -8.0}, {5.0, -12.0}}) fissa(k * rete, db, TipoRiga::rete);
    std::vector<double> giri_rete;   // giri dei generatori sincroni: 120 f / poli
    for (int poli : {4, 6, 8, 10, 12})
        if (const double g = 120.0 * rete / poli; g >= t.giri_generatori.min - 1e-6 && g <= t.giri_generatori.max + 1e-6) giri_rete.push_back(g);
    if (giri_rete.empty()) giri_rete.push_back(120.0 * rete / 8.0);
    const double giri_gen = giri_rete[caso.prossimo() % giri_rete.size()];
    const int gen = std::max(1, intero(t.generatori));
    const int in_moto = std::min(gen, 1 + (caso.uniforme() < 0.5 ? 1 : 0));   // in navigazione uno o due
    const int cil_gen = intero(t.cilindri_generatori);
    for (int g = 0; g < in_moto; ++g) {
        const double giri = giri_gen * (1.0 + 0.002 * normale());   // il carico e il regolatore: battimenti fra due
        fissa(cil_gen * giri / 120.0, g == 0 ? 0.0 : -3.0, TipoRiga::diesel);
        fissa(giri / 60.0, -12.0, TipoRiga::ausiliario);
    }
    // gli ausiliari: pompe con motori asincroni (2 o 4 poli, 2% di scorrimento)
    for (int a = 0; a < 2; ++a) fissa(rete / (caso.uniforme() < 0.5 ? 1.0 : 2.0) * 0.98 * (1.0 + 0.003 * normale()), -12.0, TipoRiga::ausiliario);

    // la propulsione
    switch (t.propulsione) {
        case Propulsione::diesel_lento: {   // 2 tempi in presa diretta: il motore e' l'asse
            const int cil = intero(t.cilindri);
            legata(cil, -2.0, TipoRiga::diesel);
            legata(2.0 * cil, -8.0, TipoRiga::diesel);
            break;
        }
        case Propulsione::diesel_medio:
        case Propulsione::diesel_veloce: {   // 4 tempi col riduttore: accensioni a cilindri x giri / 120
            const int motori = std::max(1, intero(t.motori));
            const double giri = in(t.giri_motore);
            const double rapporto = giri / giri_asse;
            const int cil = intero(t.cilindri);
            const double denti = 25.0 + 15.0 * caso.uniforme();   // il pignone del riduttore (STIMA)
            for (int m = 0; m < motori; ++m) {
                const double scarto_giri = 1.0 + 0.003 * normale();   // motori sullo stesso riduttore, appena diversi
                legata(cil * rapporto / 2.0 * scarto_giri, m == 0 ? -2.0 : -5.0, TipoRiga::diesel);
                legata(rapporto * scarto_giri, -10.0, TipoRiga::diesel);
            }
            legata(rapporto * denti, -10.0, TipoRiga::riduttore);
            break;
        }
        case Propulsione::diesel_elettrico: {   // i motori elettrici sull'asse; i diesel-generatori a giri fissi
            const int motori = std::max(1, intero(t.motori));
            const int cil = intero(t.cilindri);
            const double giri = in(t.giri_motore);
            for (int m = 0; m < motori; ++m) fissa(cil * giri * (1.0 + 0.002 * normale()) / 120.0, m == 0 ? -1.0 : -4.0, TipoRiga::diesel);
            break;
        }
        case Propulsione::turbina: {   // turbina col riduttore: rotore e ruota legati all'asse, il turbogeneratore fisso
            const double giri = in(t.giri_motore);
            legata(giri / giri_asse, -8.0, TipoRiga::turbina);
            legata(150.0 + 100.0 * caso.uniforme(), -6.0, TipoRiga::riduttore);   // denti della ruota (STIMA)
            fissa(rete * (1.0 + 0.002 * normale()), -8.0, TipoRiga::turbina);       // il turbogeneratore a 3000 o 3600
            break;
        }
    }
    // la riga piu' forte e' la tonale del catalogo (per la scoperta col LOFAR)
    const auto forte = std::max_element(f.righe.begin(), f.righe.end(), [](const RigaFirma& a, const RigaFirma& b) { return a.db_relativo < b.db_relativo; });
    f.tonale_hz = forte->hz;
    return n;
}

}  // namespace m688
