#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <string>

#include "nucleo/artiglieria.hpp"
#include "nucleo/geometria.hpp"

using namespace m688;

// Artiglieria navale, G0 e G1 (docs/16_artiglieria.md; dossier 22): dati, sagoma esposta e statistiche del tiro.

namespace {

const TabellaArtiglieria& tabella() {
    static const TabellaArtiglieria t = carica_artiglieria(M688_DATI "/artiglieria.toml");
    return t;
}

// un 688 fermo con la prua a nord: a quota periscopica col periscopio d'attacco alzato di 1,5 m, oppure in superficie
SagomaEsposta battello(Vec2 pos, double quota, double rotta = 0.0) {
    std::vector<AlberoAlzato> alberi;
    if (quota > 1.0) alberi.push_back({ParteEsposta::periscopio_attacco, 1.5, 0.2});
    return sagoma_sottomarino(tabella().sagoma("los_angeles"), 7, pos, rotta, 0.0, 110.0, 10.0, quota, 0.0, alberi);
}

struct Conti {
    int colpi = 0, diretti = 0, schegge = 0, rimbalzi = 0, acqua = 0;
    double p_testa() const { return static_cast<double>(diretti + schegge) / colpi; }
};

// colpi di `pezzo` dalla nave a `distanza` metri a ovest del battello, con la mira sul bersaglio vero
Conti tira(const std::string& pezzo, double distanza, const SagomaEsposta& g, int n, std::uint64_t seme = 1) {
    const DatiPezzo& p = *tabella().pezzo(pezzo);
    Casuale caso(seme);
    Conti k;
    const Vec2 nave{g.pos.x - distanza, g.pos.y};
    for (int i = 0; i < n; ++i) {
        ++k.colpi;
        bool toccato = false, schegge = false;
        for (const auto& x : spara(p, nave, punto_di_mira(g), {0.0, 0.0}, g, 0.0, {}, caso)) {
            if (x.esito == ImpattoArtiglieria::Esito::rimbalzo) ++k.rimbalzi;
            if (x.esito == ImpattoArtiglieria::Esito::acqua) ++k.acqua;
            if (x.esito == ImpattoArtiglieria::Esito::diretto) toccato = true;
            if (x.esito == ImpattoArtiglieria::Esito::schegge) schegge = true;
        }
        k.diretti += toccato;
        k.schegge += !toccato && schegge;
    }
    return k;
}

}  // namespace

TEST_CASE("artiglieria: i dati G0 (pezzi, classi, sagome, tavole di volo)" * doctest::skip(!std::filesystem::exists(M688_DATI "/artiglieria.toml"))) {
    const auto& t = tabella();
    REQUIRE_FALSE(t.vuota());
    const auto perry = t.pezzi_di("perry");
    REQUIRE(perry.size() == 1);
    CHECK(perry[0]->id == "mk75");
    CHECK(t.pezzi_di("kara").size() == 6);   // 2 AK-726 e 4 AK-630
    CHECK(t.pezzi_di("nimitz").empty());     // solo Phalanx: niente contro la superficie
    CHECK(t.pezzi_di("mercantile").empty());
    const DatiPezzo& mk75 = *t.pezzo("mk75");
    CHECK(mk75.tempo_volo_s(3000.0) == doctest::Approx(3.9));
    CHECK(mk75.caduta_gradi(3000.0) == doctest::Approx(1.8));
    CHECK(mk75.tempo_volo_s(4000.0) == doctest::Approx(5.7));   // fra 3 e 5 km
    CHECK(mk75.velocita_arrivo_m_s(0.0) == doctest::Approx(925.0));
    CHECK(t.pezzo("mk7_16")->portata_periscopio_m == 0.0);       // il 16" mai al periscopio
    CHECK(t.pezzo("ak630")->raffica());
    CHECK(mk75.deposito == 600);                  // il deposito (ricerca 36): STIMA per la Perry
    CHECK(t.pezzo("ak100")->deposito == 350);
    CHECK(t.pezzo("mk7_16")->deposito == 0);      // n.v.: senza limite
    // la Knox e le nordiche (ricerca 36, 31/10/2026)
    CHECK(t.pezzi_di("knox").size() == 1);
    CHECK(t.pezzi_di("knox")[0]->id == "mk42");
    CHECK(t.pezzi_di("oslo").size() == 2);        // i due binati da 76 della maggior parte delle Oslo nel 1989
    CHECK(t.pezzo("mk33")->canne == 2);
    CHECK(t.pezzi_di("hauk").size() == 1);
    CHECK(t.pezzi_di("stockholm").size() == 2);
    CHECK(t.pezzi_di("grisha3").size() == 2);     // l'AK-725 binato e l'AK-630
    CHECK(t.pezzo("bofors40")->portata_superficie_m < t.pezzo("bofors57")->portata_superficie_m);
    CHECK(t.sagoma("classe_inesistente").falsatorre_altezza_m > 0.0);
    // un file assente: tutto spento
    CHECK(carica_artiglieria("/percorso/inesistente.toml").vuota());
}

TEST_CASE("artiglieria: la sagoma esposta di un sottomarino" * doctest::skip(!std::filesystem::exists(M688_DATI "/artiglieria.toml"))) {
    const SagomaEsposta su = battello({0, 0}, 0.0);
    CHECK(su.in_superficie);
    CHECK(su.bordo_libero_m > 0.0);
    CHECK(su.falsatorre_altezza_m >= 6.0);
    const SagomaEsposta peri = battello({0, 0}, 18.0);
    CHECK_FALSE(peri.in_superficie);
    CHECK_FALSE(peri.vuota());
    REQUIRE(peri.alberi.size() == 1);
    CHECK(peri.alberi[0].pos.y == doctest::Approx(tabella().sagoma("los_angeles").falsatorre_avanti_m));   // sulla falsatorre, a prora
    CHECK(peri.alberi[0].pos.y > 10.0);
    // sotto, senza alberi: il cannone non serve (22 §5.2)
    CHECK(sagoma_sottomarino(tabella().sagoma("alfa"), 3, {0, 0}, 0.0, 0.0, 81.0, 9.5, 30.0, 0.0, {}).vuota());
    // la vela che affiora col mare grosso
    CHECK_FALSE(sagoma_sottomarino(tabella().sagoma("alfa"), 3, {0, 0}, 0.0, 0.0, 81.0, 9.5, 12.0, 0.8, {}).vuota());
}

TEST_CASE("artiglieria: probabilita' per colpo contro la testa del periscopio e contro il battello in superficie" * doctest::skip(!std::filesystem::exists(M688_DATI "/artiglieria.toml"))) {
    // 22 §3.6: testa del periscopio 30-50% a 1 km, 5-12% a 3 km (schegge entro r_eff comprese)
    const Conti vicino = tira("mk75", 1000.0, battello({0, 0}, 18.0), 2000);
    const Conti medio = tira("mk75", 3000.0, battello({0, 0}, 18.0), 2000);
    CAPTURE(vicino.p_testa());
    CAPTURE(medio.p_testa());
    CHECK(vicino.p_testa() > 0.2);
    CHECK(vicino.p_testa() < 0.65);
    CHECK(medio.p_testa() > 0.03);
    CHECK(medio.p_testa() < 0.2);
    CHECK(vicino.p_testa() > medio.p_testa());
    // battello in superficie a 3 km, di traverso e di prora: 22 §3.6 da' 60-80% e 25-40%, ma col tiro radente lo
    // spazio pericoloso fa assorbire l'errore in distanza ai 110 m di scafo anche di prora
    const Conti traverso = tira("mk45", 3000.0, battello({0, 0}, 0.0, 0.0), 1000);
    const Conti prora = tira("mk45", 3000.0, battello({0, 0}, 0.0, 90.0), 1000);
    CAPTURE(traverso.diretti);
    CAPTURE(prora.diretti);
    CHECK(traverso.diretti > 450);
    CHECK(prora.diretti > 300);
    // tiro radente: meta' dei colpi in acqua rimbalza (22 §5.1)
    CHECK(medio.rimbalzi > medio.colpi / 4);
}

TEST_CASE("artiglieria: avvertimenti lontani, raffiche e eventi per audio e grafica" * doctest::skip(!std::filesystem::exists(M688_DATI "/artiglieria.toml"))) {
    const SagomaEsposta g = battello({0, 0}, 0.0);
    Casuale caso(5);
    for (int i = 0; i < 50; ++i)
        for (const auto& x : spara(*tabella().pezzo("mk75"), {-3000, 0}, g.pos, {0, 0}, g, 10.0, {}, caso, true)) {
            CHECK(x.avvertimento);
            CHECK(x.parte == ParteEsposta::nessuna);
            const double d = distanza(x.punto, g.pos);
            CHECK(d >= 250.0);
            CHECK(d <= 520.0);
        }
    // l'AK-630 a 1,5 km sul periscopio: una raffica (il centro) e qualche proietto sulla testa
    const SagomaEsposta peri = battello({0, 0}, 18.0);
    int con_colpi = 0;
    for (int i = 0; i < 100; ++i) {
        const auto v = spara(*tabella().pezzo("ak630"), {-1500, 0}, punto_di_mira(peri), {0, 0}, peri, 0.0, {}, caso);
        REQUIRE_FALSE(v.empty());
        CHECK(v[0].raffica);
        CHECK(v[0].colpi == 100);
        con_colpi += v.size() > 1;
    }
    CHECK(con_colpi > 10);
    // gli eventi: partenza alla nave, poi lo scoppio al punto di caduta dopo il tempo di volo
    ImpattoArtiglieria x;
    x.calibro_mm = 76.0;
    x.carica_kg = 0.75;
    x.tempo_sparo_s = 5.0;
    x.tempo_impatto_s = 8.9;
    x.punto = {100, 0};
    const auto e = eventi_di(x, {-3000, 0}, true);
    REQUIRE(e.size() == 2);
    CHECK(e[0].tipo == TipoEventoArtiglieria::partenza);
    CHECK(e[0].tempo_s == doctest::Approx(5.0));
    CHECK(e[1].tipo == TipoEventoArtiglieria::scoppio_superficie);
    CHECK(e[1].tempo_s == doctest::Approx(8.9));
    x.esito = ImpattoArtiglieria::Esito::diretto;
    x.parte = ParteEsposta::periscopio_attacco;
    CHECK(eventi_di(x, {-3000, 0}, false).front().tipo == TipoEventoArtiglieria::colpo_testa);
}

// --- G2: la regola di fuoco nella missione ---------------------------------------------------------------------

#include <fstream>
#include <map>
#include <memory>
#include <set>

#include "../app/scelta_fotografie.hpp"
#include "missioni/achievement_missione.hpp"
#include "missioni/carriera_missione.hpp"
#include "missioni/comandante_ia.hpp"
#include "missioni/danni_missione.hpp"
#include "missioni/danni_sottomarini.hpp"
#include "missioni/missione.hpp"
#include "missioni/registro_missione.hpp"
#include "missioni/stazioni_missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/ordini.hpp"

namespace {

namespace fs = std::filesystem;

// il 688 fermo a `quota` e un Krivak II ostile a 4 km a nord che si avvicina piano; `situazione` del blocco carriera
std::string scena_g2(const std::string& nome, const std::string& situazione, double quota, int ora_utc = 12) {
    const auto f = fs::temp_directory_path() / ("m688_prova_artiglieria_" + nome + ".lua");
    const std::string sub = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200, rumore = 8, sonar = 19 }";
    const std::string nave = "modello = { massima_kn = 32, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 0, integrita = 100, rumore = 10, sonar = 20 }";
    std::ofstream(f) << "return { id = 'ART_" << nome << "', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = " << ora_utc << ","
                        " paesaggio_sonoro = false, carriera = { situazione = '" << situazione << "', difficolta = 3 },"
                        " script = {}, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
                        "  posizione = { x = 0, y = 0 }, quota_m = " << quota << ", rotta = 90, velocita_kn = 0, " << sub << " },"
                        "{ id = 2, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = 4000 },"
                        "  quota_m = 0, rotta = 180, velocita_kn = 3, " << nave << " },"
                        "\n}, timer = {} } } }\n";
    return f.string();
}

struct PartitaG2 {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    explicit PartitaG2(const std::string& file) : m(file, 1), dm(classi, M688_DATI, 1, RegolaDanni::simulazione) {}
    void avanza(double s) { for (long i = 0; i < static_cast<long>(s * 9.0); ++i) { m.avanza(1.0 / 9.0); dm.avanza(m, 1.0 / 9.0); } }
};

int quanti(const Missione& m, TipoEvento t) {
    return static_cast<int>(std::count_if(m.registro().eventi().begin(), m.registro().eventi().end(), [&](const EventoMissione& e) { return e.tipo == t; }));
}

}  // namespace

TEST_CASE("artiglieria G2: in guerra il Krivak spara al 688 in superficie; in crisi solo avvertimenti, in pace mai" * doctest::skip(!fs::exists(M688_DATI "/artiglieria.toml"))) {
    PartitaG2 guerra(scena_g2("guerra", "guerra", 0.0));
    guerra.avanza(240.0);
    bool transitorio = false;   // nella scena acustica del giocatore (G5), mentre spara: tiene un quarto del deposito e smette a 295 s
    for (int i = 0; i < 60; ++i) {
        guerra.avanza(1.0);
        for (const auto& s : guerra.m.scena_acustica(0, "bqq5_sfera").sorgenti) transitorio = transitorio || (s.id >= 990000 && s.id <= 999999);
    }
    CHECK(transitorio);
    REQUIRE(guerra.m.con_artiglieria());
    const auto& imp = guerra.m.impatti_artiglieria();
    REQUIRE_FALSE(imp.empty());
    CHECK(imp.front().tiratore == 2);
    CHECK(imp.front().bersaglio == 0);
    CHECK(imp.front().pezzo == "ak100");
    CHECK(std::none_of(imp.begin(), imp.end(), [](const ImpattoArtiglieria& i) { return i.avvertimento; }));
    CHECK(std::any_of(imp.begin(), imp.end(), [](const ImpattoArtiglieria& i) { return i.esito == ImpattoArtiglieria::Esito::diretto; }));
    const auto& ev = guerra.m.eventi_artiglieria();
    CHECK(std::any_of(ev.begin(), ev.end(), [](const EventoArtiglieria& e) { return e.tipo == TipoEventoArtiglieria::partenza; }));
    CHECK(quanti(guerra.m, TipoEvento::artiglieria_salva) == 1);
    CHECK(quanti(guerra.m, TipoEvento::artiglieria_colpo) >= 1);
    CHECK(quanti(guerra.m, TipoEvento::artiglieria_vicino) == 1);

    PartitaG2 crisi(scena_g2("crisi", "crisi", 0.0));
    crisi.avanza(300.0);
    REQUIRE_FALSE(crisi.m.impatti_artiglieria().empty());
    for (const auto& i : crisi.m.impatti_artiglieria()) {
        CHECK(i.avvertimento);
        CHECK(i.parte == ParteEsposta::nessuna);
    }
    CHECK(crisi.m.artiglieria_avvertimento());

    PartitaG2 pace(scena_g2("pace", "pace", 0.0));
    pace.avanza(300.0);
    CHECK(pace.m.impatti_artiglieria().empty());
}

// il deposito (la principale, 31/10/2026: nella S12 due Perry tiravano 7566 colpi in un'ora): il Krivak in guerra sul 688
// in superficie per 15 minuti; i due AK-100 a 50 colpi al minuto vuotano i 350 colpi a testa in circa 7 minuti, poi tacciono
TEST_CASE("artiglieria: finito il deposito il pezzo tace" * doctest::skip(!fs::exists(M688_DATI "/artiglieria.toml"))) {
    PartitaG2 p(scena_g2("deposito", "guerra", 0.0));
    p.avanza(600.0);
    const auto da = [&](double t) {
        return std::count_if(p.m.impatti_artiglieria().begin(), p.m.impatti_artiglieria().end(),
                             [&](const ImpattoArtiglieria& i) { return i.tiratore == 2 && !i.avvertimento && i.tempo_sparo_s >= t; });
    };
    const auto tutti = da(0.0);
    p.avanza(300.0);
    const auto dopo = da(600.0);
    MESSAGE("colpi del Krivak nei primi 10 minuti: ", tutti, ", negli ultimi 5: ", dopo);
    CHECK(tutti > 100);
    CHECK(tutti <= 700);
    CHECK(dopo == 0);
}

namespace {
// una, tre navi a tiro di un battello che non puo' immergersi: i colpi diretti si sommano, fino al fuori combattimento
struct TiroSuBattello {
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    Missione m;
    DanniMissione dm;
    DanniSottomarini ds;
    std::unique_ptr<StazioniMissione> st;
    RegistroOrdini ordini;
    std::unique_ptr<ComandanteIa> comandante;
    explicit TiroSuBattello(const std::string& file, bool con_comandante = false) : m(file, 1, false), dm(classi, M688_DATI, 1, RegolaDanni::simulazione), ds(classi, M688_DATI, 1) {
        if (!con_comandante) return;
        m.senza_giocatore(true);
        ds.prepara(m, 0);
        st = std::make_unique<StazioniMissione>(m, 1u);
        st->usa_danni(&ds);
        comandante = std::make_unique<ComandanteIa>(m, OpzioniComandante{}, 1);
    }
    void passo() {
        if (comandante) {   // si immergerebbe appena puo': qui resta a galla, per contare i colpi
            for (const auto& o : comandante->passo(m, *st)) ordini.ordina(o);
            for (const auto& o : ordini.consegna(m.tempo()))
                if (!ds.consegna(m, o)) st->consegna(m, o);
            UnitaClassica* c = m.trova(0);
            c->quota = c->quota_ordinata = 0.0;
        }
        m.avanza(1.0);
        if (st) st->dopo_passo(m);
        dm.avanza(m, 1.0);
        ds.avanza(m, 1.0);
    }
    long diretti(int b) const {
        return std::count_if(m.impatti_artiglieria().begin(), m.impatti_artiglieria().end(), [&](const ImpattoArtiglieria& i) { return i.bersaglio == b && i.esito == ImpattoArtiglieria::Esito::diretto; });
    }
};
std::string scena_cannone(const std::string& nome, int integrita, bool giocatore, const std::string& classe_nave = "perry", bool tre_navi = true) {
    const auto f = fs::temp_directory_path() / ("m688_prova_cannone_" + nome + ".lua");
    const std::string modello = "modello = { massima_kn = 3, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 0, integrita = " + std::to_string(integrita) + ", rumore = 8, sonar = 19 }";
    const std::string nave = "classe = '" + classe_nave + "', categoria = 'nave', flag = 10, quota_m = 0, velocita_kn = 0, modello = { massima_kn = 29, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 0, integrita = 100, rumore = 10, sonar = 20 }";
    std::ofstream(f) << "return { id = 'PROVA_CANNONE_" << nome << "', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12,"
                        " paesaggio_sonoro = false, carriera = { situazione = 'guerra' }, editor = {}, script = {}, varianti = { { probabilita = 1, unita = {\n"
                     << (giocatore ? "{ id = 0, classe = 'alfa', categoria = 'sottomarino', flag = 17, giocatore = true, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 90, velocita_kn = 3, " + modello + " },\n"
                                   : "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = -90000, y = -80000 }, quota_m = 300, rotta = 90, velocita_kn = 3, modello = { massima_kn = 30, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 100, rumore = 8, sonar = 19 } },\n"
                                     "{ id = 12, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 90, velocita_kn = 3, " + modello + " },\n")
                     << "{ id = 2, posizione = { x = 0, y = 3000 }, rotta = 90, " << nave << " },\n"
                     << (tre_navi ? "{ id = 3, posizione = { x = 3000, y = 0 }, rotta = 0, " + nave + " },\n{ id = 4, posizione = { x = 0, y = -3000 }, rotta = 270, " + nave + " },\n" : std::string{})
                     << "}, timer = {} } } }\n";
    return f.string();
}
}  // namespace

// La S07 del 05/11/2026: 5329 proiettili di AK-630 sul battello a galla, una fotografia a colpo, 10 GB. Il caso peggiore: un Kirov con 8 AK-630
TEST_CASE("artiglieria: i colpi non moltiplicano le fotografie del debriefing, e i momenti salienti restano" * doctest::skip(!fs::exists(M688_DATI "/artiglieria.toml"))) {
    const auto f = scena_cannone("foto", 300000, true, "kirov", false);
    TiroSuBattello p(f);
    for (int k = 0; k < 300; ++k) p.passo();
    fs::remove(f);
    SceltaFotografie scelta;
    std::set<int> fatte;
    long colpi = 0;
    for (const EventoMissione& e : p.m.eventi_missione()) {
        colpi += e.tipo == TipoEvento::artiglieria_colpo;
        if (e.inquadratura.visibile && scelta.scatta(e, fatte.size())) fatte.insert(e.id);
    }
    MESSAGE("colpi del Kirov: ", colpi, ", fotografie: ", fatte.size());
    CHECK(colpi >= 500);
    CHECK(fatte.size() <= 20);
    for (const EventoMissione& e : p.m.eventi_missione())   // ogni colpo mostra una fotografia: nessuna cartolina vuota
        if (e.tipo == TipoEvento::artiglieria_colpo) CHECK(fatte.count(scelta.foto_di(e.id)) == 1);
    const auto salienti = p.m.momenti_salienti();
    CHECK(salienti.size() <= 12);
    CHECK(salienti.size() >= 2);
    for (const EventoMissione* e : salienti)
        if (e->tipo == TipoEvento::artiglieria_colpo) CHECK(fatte.count(scelta.foto_di(e->id)) == 1);
}

// La S07: 5329 proiettili di AK-630 facevano 5329 «colpi subiti», come altrettanti siluri
TEST_CASE("artiglieria: una raffica vale un colpo subito, ogni canna di un binato per se', ogni proiettile resta nel registro, e i riconoscimenti non cambiano" * doctest::skip(!fs::exists(M688_DATI "/artiglieria.toml"))) {
    const auto f = scena_cannone("raffica", 300000, true, "kara", false);
    TiroSuBattello p(f);
    for (int k = 0; k < 300; ++k) p.passo();
    fs::remove(f);
    long eventi = 0;
    std::set<int> nel_registro;
    for (const EventoMissione& e : p.m.eventi_missione())
        if (e.tipo == TipoEvento::artiglieria_colpo) {
            ++eventi;
            CHECK(e.codice > 0);
            nel_registro.insert(e.codice);
        }
    long a_segno = 0;
    std::map<int, std::set<std::string>> pezzi;
    std::map<double, std::set<int>> binati;
    for (const ImpattoArtiglieria& i : p.m.impatti_artiglieria()) {
        if (i.bersaglio != 0 || i.avvertimento) continue;
        CHECK(i.sparo > 0);
        pezzi[i.sparo].insert(i.pezzo);
        if (i.pezzo == "ak726") binati[i.tempo_sparo_s].insert(i.sparo);
        a_segno += i.esito == ImpattoArtiglieria::Esito::diretto || i.esito == ImpattoArtiglieria::Esito::schegge;
    }
    for (const auto& [sparo, q] : pezzi) CHECK(q.size() == 1);
    CHECK(std::any_of(binati.begin(), binati.end(), [](const auto& b) { return b.second.size() >= 2; }));   // lo stesso istante, due canne
    const EsitoDettagliato esito = p.m.esito_dettagliato();
    MESSAGE("eventi ", eventi, ", colpi subiti ", esito.colpi_subiti);
    CHECK(eventi == a_segno);   // nel registro, proiettile per proiettile
    CHECK(eventi > 5 * esito.colpi_subiti);
    CHECK(esito.colpi_subiti == static_cast<int>(nel_registro.size()));
    // i riconoscimenti guardano solo == 0 e >= 1 (achievement.toml), e con un colpo solo non sono zero
    auto misure = misure_achievement(p.m, ContestoMissione{});
    CHECK(misure["colpi_subiti"] == static_cast<double>(esito.colpi_subiti));
    CHECK(misure["colpi_artiglieria_subiti"] == static_cast<double>(esito.colpi_subiti));
    CHECK(misure["colpi_subiti"] >= 1.0);
    CHECK(riassumi(p.m).attaccato);
}

// Dal deposito si scalavano solo i proietti che toccano: contro un periscopio un AK-630 sparava fino a 20 volte i suoi colpi
TEST_CASE("artiglieria: una raffica scala dal deposito i proietti sparati, anche quelli a vuoto" * doctest::skip(!fs::exists(M688_DATI "/artiglieria.toml"))) {
    const auto f = scena_cannone("raffiche", 300000, true, "kirov", false);
    TiroSuBattello p(f);
    for (int k = 0; k < 600; ++k) p.passo();
    fs::remove(f);
    const auto raffiche = std::count_if(p.m.impatti_artiglieria().begin(), p.m.impatti_artiglieria().end(), [](const ImpattoArtiglieria& i) { return i.raffica && i.pezzo == "ak630" && !i.avvertimento; });
    MESSAGE("raffiche di AK-630 del Kirov in 10 minuti: ", raffiche);
    CHECK(raffiche == 8 * 15);   // 8 affusti, 1500 colpi su 2000 contro un sottomarino, 100 a raffica
}

TEST_CASE("artiglieria: i conti degli spari, e una traccia vecchia senza numeri non si riduce a un colpo") {
    ContaSpari vecchia;
    int colpi = 0;
    for (int k = 0; k < 5; ++k) colpi += vecchia.nuovo(0);
    CHECK(colpi == 5);
    ContaSpari nuova;
    colpi = 0;
    for (const int sparo : {7, 7, 7, 8, 9, 9}) colpi += nuova.nuovo(sparo);
    CHECK(colpi == 3);
}

TEST_CASE("artiglieria: il limite delle fotografie vale per tutti, e la fine e gli affondamenti passano") {
    SceltaFotografie scelta;
    const auto evento = [](int id, TipoEvento tipo, int interesse, int unita = 1) {
        EventoMissione e;
        e.id = id;
        e.tipo = tipo;
        e.interesse = interesse;
        e.unita = unita;
        return e;
    };
    std::size_t fatte = 0;
    int id = 0;
    for (int k = 0; k < 12000; ++k) fatte += scelta.scatta(evento(++id, TipoEvento::artiglieria_colpo, 80), fatte);   // un tiratore solo
    CHECK(fatte == 1);
    for (int k = 0; k < 400; ++k) fatte += scelta.scatta(evento(++id, TipoEvento::artiglieria_colpo, 80, 100 + k), fatte);   // tiratori diversi
    CHECK(fatte == SceltaFotografie::LIMITE);
    const int tiratore_senza = id;
    CHECK(scelta.foto_di(tiratore_senza) == tiratore_senza);   // senza fotografia: la cartolina si fa dalle pose (app/figurina_evento.hpp)
    CHECK_FALSE(scelta.scatta(evento(++id, TipoEvento::siluro_evitato, 90), fatte));
    CHECK(scelta.scatta(evento(++id, TipoEvento::affondamento, 100), fatte));
    CHECK(scelta.scatta(evento(++id, TipoEvento::fine, 100), fatte));
    // il tetto di prima: oltre 120 solo gli eventi con interesse da 60
    SceltaFotografie tetto;
    CHECK_FALSE(tetto.scatta(evento(1, TipoEvento::scoperta, 59), SceltaFotografie::TETTO));
    CHECK(tetto.scatta(evento(2, TipoEvento::siluro_evitato, 60), SceltaFotografie::TETTO));
    CHECK(tetto.scatta(evento(3, TipoEvento::scoperta, 59), SceltaFotografie::TETTO - 1));
}

TEST_CASE("artiglieria: i colpi diretti del 76 mm si sommano, e a integrita' zero il battello e' fuori combattimento, anche il giocatore e il comandante IA" * doctest::skip(!fs::exists(M688_DATI "/artiglieria.toml"))) {
    const auto gioca = [](const std::string& nome, bool giocatore, bool con_comandante) {
        const auto f = scena_cannone(nome, 10, giocatore);
        TiroSuBattello p(f, con_comandante);
        const int id = giocatore ? 0 : 12;
        long diretti = 0;
        for (int k = 0; k < 900; ++k) {
            p.passo();
            diretti = p.diretti(id);
            const UnitaClassica* u = p.m.trova(id);
            if (!u || u->affondamento > 0 || u->distrutta) break;
        }
        fs::remove(f);
        const UnitaClassica* u = p.m.trova(id);
        MESSAGE(nome, ": colpi diretti ", diretti, ", affondamento ", u ? u->affondamento : -1);
        CHECK((!u || u->affondamento > 0 || u->distrutta));
        CHECK(diretti >= 50);   // 0,2 per colpo su 10
        CHECK(diretti <= 70);   // anche quelli sugli alberi non contano
        for (const Colpo& c : p.m.colpi()) CHECK(c.vittima != id);   // il cannone, non un Harpoon
    };
    gioca("ia", false, false);
    gioca("giocatore", true, false);
    gioca("comandante", true, true);
}

TEST_CASE("artiglieria: le cannonate a segno contano nei colpi subiti dell'esito, una volta sola, e arrivano a riconoscimenti e carriera" * doctest::skip(!fs::exists(M688_DATI "/artiglieria.toml"))) {
    const auto f = scena_cannone("debriefing", 10, true);
    TiroSuBattello p(f);
    for (int k = 0; k < 900 && p.m.trova(0) && p.m.trova(0)->affondamento == 0; ++k) p.passo();
    fs::remove(f);
    const auto& imp = p.m.impatti_artiglieria();
    const auto a_segno = std::count_if(imp.begin(), imp.end(), [](const ImpattoArtiglieria& i) {
        return i.bersaglio == 0 && !i.avvertimento && (i.esito == ImpattoArtiglieria::Esito::diretto || i.esito == ImpattoArtiglieria::Esito::schegge);
    });
    const EsitoDettagliato esito = p.m.esito_dettagliato();
    MESSAGE("a segno ", a_segno, ", nell'esito ", esito.colpi_subiti);
    CHECK(a_segno >= 50);
    CHECK(esito.colpi_subiti == a_segno);   // la riga del debriefing legge questo
    CHECK(misure_achievement(p.m, ContestoMissione{})["colpi_subiti"] == static_cast<double>(a_segno));
    CHECK(riassumi(p.m).attaccato);   // il Combat Action Ribbon
}

TEST_CASE("artiglieria: i colpi d'avvertimento non contano nei colpi subiti" * doctest::skip(!fs::exists(M688_DATI "/artiglieria.toml"))) {
    PartitaG2 crisi(scena_g2("crisi_conto", "crisi", 0.0));
    crisi.avanza(300.0);
    REQUIRE_FALSE(crisi.m.impatti_artiglieria().empty());
    for (const auto& i : crisi.m.impatti_artiglieria()) CHECK(i.avvertimento);
    CHECK(quanti(crisi.m, TipoEvento::artiglieria_salva) >= 1);
    const EsitoDettagliato esito = crisi.m.esito_dettagliato();
    CHECK(esito.colpi_subiti == 0);
    CHECK(misure_achievement(crisi.m, ContestoMissione{})["colpi_subiti"] == 0.0);
    CHECK_FALSE(riassumi(crisi.m).attaccato);
}

TEST_CASE("artiglieria: sotto i 57 mm i colpi non consumano integrita', e non finiscono un battello gia' a zero per un siluro" * doctest::skip(!fs::exists(M688_DATI "/artiglieria.toml"))) {
    const auto f = scena_cannone("quaranta", 1, true, "hauk");
    TiroSuBattello p(f);
    p.m.trova(0)->integrita = 0;
    for (int k = 0; k < 600; ++k) p.passo();
    fs::remove(f);
    const UnitaClassica* u = p.m.trova(0);
    MESSAGE("colpi diretti da 40 mm: ", p.diretti(0));
    CHECK(p.diretti(0) > 50);
    REQUIRE(u);
    CHECK(u->affondamento == 0);
    CHECK_FALSE(u->distrutta);
}

TEST_CASE("artiglieria: contro un sottomarino la nave tiene un quarto del deposito" * doctest::skip(!fs::exists(M688_DATI "/artiglieria.toml"))) {
    const auto f = scena_cannone("riserva", 30000, false, "perry", false);
    TiroSuBattello p(f);
    for (int k = 0; k < 1500; ++k) p.passo();
    fs::remove(f);
    std::set<int> spari;   // i proietti, non gli impatti: con un rimbalzo un proietto ne fa due
    for (const ImpattoArtiglieria& i : p.m.impatti_artiglieria())
        if (i.tiratore == 2 && i.bersaglio == 12 && i.pezzo == "mk75") spari.insert(i.sparo);
    const auto sparati = spari.size();
    MESSAGE("Mk 75 sparati sul Victor: ", sparati);
    CHECK(sparati > 300);
    CHECK(sparati <= 450);   // 600 in deposito, 150 in riserva
}

TEST_CASE("artiglieria G2: il battello immerso senza alberi e' al sicuro; il classico non spara" * doctest::skip(!fs::exists(M688_DATI "/artiglieria.toml"))) {
    PartitaG2 sotto(scena_g2("sotto", "guerra", 30.0));
    sotto.avanza(300.0);
    CHECK(sotto.m.sagoma_esposta(sotto.m.unita().at(0)).vuota());
    CHECK(sotto.m.impatti_artiglieria().empty());
    Missione classico(scena_g2("classico", "guerra", 0.0), 1);
    for (int i = 0; i < 9 * 300; ++i) classico.avanza(1.0 / 9.0);
    CHECK_FALSE(classico.con_artiglieria());
    CHECK(classico.impatti_artiglieria().empty());
}

TEST_CASE("artiglieria G2: di notte l'illuminante parte da un pezzo pronto, con la sua partenza e il tempo di volo" * doctest::skip(!fs::exists(M688_DATI "/artiglieria.toml"))) {
    PartitaG2 notte(scena_g2("notte", "guerra", 0.0, 23));
    notte.avanza(300.0);
    REQUIRE(notte.m.luce_periscopi() < 0.2);
    const auto& ev = notte.m.eventi_artiglieria();
    const auto ill = std::find_if(ev.begin(), ev.end(), [](const EventoArtiglieria& e) { return e.tipo == TipoEventoArtiglieria::illuminante; });
    REQUIRE(ill != ev.end());
    CHECK(ill->durata_s == doctest::Approx(50.0));
    CHECK(ill->quota_m > 100.0);
    CHECK(ill->tempo_s > 10.0);   // dopo la reazione della nave, non a t = 0
    // prima, la sua partenza dalla nave
    const auto prima = std::find_if(ev.begin(), ill, [&](const EventoArtiglieria& e) {
        return e.tipo == TipoEventoArtiglieria::partenza && e.tiratore == ill->tiratore && e.tempo_s < ill->tempo_s;
    });
    CHECK(prima != ill);
}

TEST_CASE("artiglieria G2: con fra_navi in guerra un Kara e una Perry si cannoneggiano; senza no" * doctest::skip(!fs::exists(M688_DATI "/artiglieria.toml"))) {
    const auto scena = [](const std::string& nome, bool fra_navi) {
        const auto f = fs::temp_directory_path() / ("m688_prova_fra_navi_" + nome + ".lua");
        const std::string sub = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200, rumore = 8, sonar = 19 }";
        const std::string nave = "modello = { massima_kn = 32, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 0, integrita = 100, rumore = 10, sonar = 20 }";
        std::ofstream(f) << "return { id = 'FRA_" << nome << "', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false,"
                            " carriera = { situazione = 'guerra' }, " << (fra_navi ? "artiglieria = { fra_navi = true }, " : "")
                         << "script = { P = function(u) m688.posto_di_combattimento(u) end }, varianti = { { probabilita = 1, unita = {\n"
                            "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = -20000 },"
                            "  quota_m = 100, rotta = 90, velocita_kn = 3, " << sub << " },"
                            "{ id = 2, classe = 'kara', categoria = 'nave', flag = 18, posizione = { x = 0, y = 4000 }, quota_m = 0, rotta = 90, velocita_kn = 10, " << nave << " },"
                            "{ id = 3, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = -3000 }, quota_m = 0, rotta = 90, velocita_kn = 10, " << nave << " },"
                            "\n}, timer = { { dopo_s = 1, unita = 2, gestore = 'P' }, { dopo_s = 1, unita = 3, gestore = 'P' } } } } }\n";
        return f.string();
    };
    // entrambe al posto di combattimento dall'inizio: con i danni G3 la Perry, piu' pronta, rallenterebbe o affonderebbe
    // il Kara prima che finisca la reazione di condizione III (e lo porterebbe oltre gli 8,2 km dell'AK-726)
    PartitaG2 con(scena("con", true));
    con.avanza(400.0);
    const auto& imp = con.m.impatti_artiglieria();
    CHECK(std::any_of(imp.begin(), imp.end(), [](const ImpattoArtiglieria& i) { return i.tiratore == 2 && i.bersaglio == 3; }));
    CHECK(std::any_of(imp.begin(), imp.end(), [](const ImpattoArtiglieria& i) { return i.tiratore == 3 && i.bersaglio == 2; }));
    CHECK(std::none_of(imp.begin(), imp.end(), [](const ImpattoArtiglieria& i) { return i.bersaglio == 0; }));   // il 688 immerso no
    // lo spettatore (docs/18 fase 5): le salve e le scoperte fra unita' dell'IA nel registro, nessuna sul giocatore
    const auto& ev = con.m.registro().eventi();
    const auto c = [&](TipoEvento t, int u, int a) {
        return std::any_of(ev.begin(), ev.end(), [&](const EventoMissione& e) { return e.tipo == t && e.unita == u && e.altra == a; });
    };
    CHECK(c(TipoEvento::artiglieria_ia, 2, 3));
    CHECK(c(TipoEvento::artiglieria_ia, 3, 2));
    CHECK(c(TipoEvento::scoperta_ia, 2, 3));
    CHECK(c(TipoEvento::scoperta_ia, 3, 2));
    CHECK(quanti(con.m, TipoEvento::artiglieria_salva) == 0);
    PartitaG2 senza(scena("senza", false));
    senza.avanza(400.0);
    CHECK(senza.m.impatti_artiglieria().empty());
}

namespace {

// scena libera per le estensioni del G2: il giocatore, le altre unita' e le chiavi in piu' della missione
std::string scena_g8(const std::string& nome, const std::string& situazione, const std::string& giocatore, const std::string& altre,
                     const std::string& chiavi = "", const std::string& script = "", const std::string& timer = "") {
    const auto f = fs::temp_directory_path() / ("m688_prova_artiglieria_g8_" + nome + ".lua");
    std::ofstream(f) << "return { id = 'G8_" << nome << "', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12,"
                        " paesaggio_sonoro = false, carriera = { situazione = '" << situazione << "', difficolta = 3 }, " << chiavi
                     << " script = {" << script << "}, varianti = { { probabilita = 1, unita = {\n" << giocatore << ",\n" << altre
                     << "\n}, timer = {" << timer << "} } } }\n";
    return f.string();
}

const std::string SUB_G8 = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200, rumore = 8, sonar = 19 }";
const std::string NAVE_G8 = "modello = { massima_kn = 32, virata_gradi_s = 2, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 0, integrita = 100, rumore = 10, sonar = 20 }";

std::string giocatore_g8(double x, double y, double quota, double velocita = 0.0) {
    return "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = " + std::to_string(x) +
           ", y = " + std::to_string(y) + " }, quota_m = " + std::to_string(quota) + ", rotta = 90, velocita_kn = " + std::to_string(velocita) + ", " + SUB_G8 + " }";
}

}  // namespace

TEST_CASE("artiglieria: in guerra il Krivak spara anche al sottomarino dell'IA in superficie, che si immerge" * doctest::skip(!fs::exists(M688_DATI "/artiglieria.toml"))) {
    PartitaG2 p(scena_g8("ia", "guerra", giocatore_g8(-20000.0, 0.0, 100.0),
                         "{ id = 2, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = 4000 }, quota_m = 0, rotta = 180, velocita_kn = 3, " + NAVE_G8 + " },"
                         "{ id = 3, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 90, velocita_kn = 0, " + SUB_G8 + " },"));
    p.avanza(300.0);
    const auto& imp = p.m.impatti_artiglieria();
    REQUIRE(std::any_of(imp.begin(), imp.end(), [](const ImpattoArtiglieria& i) { return i.tiratore == 2 && i.bersaglio == 3 && !i.avvertimento; }));
    CHECK(std::none_of(imp.begin(), imp.end(), [](const ImpattoArtiglieria& i) { return i.bersaglio == 0; }));
    CHECK(p.m.unita().at(3).quota_ordinata >= 60.0);   // sotto il tiro si immerge
    CHECK(quanti(p.m, TipoEvento::artiglieria_salva) == 0);   // non e' il giocatore
}

// la cassa zavorra squarciata (il modello del kit; la principale, 31/10/2026): col danno dei sottomarini, la Krivak in
// guerra sul 688 dell'IA in superficie; se un colpo a prua o a poppa squarcia una cassa, il comandante non ordina piu'
// l'immersione (scelta di comando, STIMA), anche sotto il tiro. Su piu' semi, finche' una cassa si squarcia
TEST_CASE("artiglieria: con una cassa zavorra squarciata il sottomarino dell'IA resta a galla" * doctest::skip(!fs::exists(M688_DATI "/artiglieria.toml"))) {
    // di prua verso la Krivak (la dispersione in distanza corre lungo lo scafo e arriva alle estremita'); scende a 0,05
    // m/s: senza la regola, a 300 s sarebbe una decina di metri sotto
    std::string lento = SUB_G8;
    lento.replace(lento.find("quota_m_s = 1"), 13, "quota_m_s = 0.05");
    const auto f = scena_g8("cassa", "guerra", giocatore_g8(-20000.0, 0.0, 100.0),
                            "{ id = 2, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = 2000 }, quota_m = 0, rotta = 180, velocita_kn = 3, " + NAVE_G8 + " },"
                            "{ id = 3, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 0, velocita_kn = 0, " + lento + " },");
    std::map<std::string, ClasseNave> classi = carica_classi(M688_DATI "/classi.toml");
    bool provata = false;
    for (std::uint64_t seme = 1; seme <= 16 && !provata; ++seme) {
        Missione m(f, seme);
        DanniMissione dm(classi, M688_DATI, seme, RegolaDanni::simulazione);
        DanniSottomarini ds(classi, M688_DATI, seme);
        for (long i = 0; i < 300L * 9; ++i) { m.avanza(1.0 / 9.0); dm.avanza(m, 1.0 / 9.0); ds.avanza(m, 1.0 / 9.0); m.raccogli_danni(&ds, &dm); }
        const auto* a = ds.artiglieria(3);
        const auto u = m.unita().find(3);
        const bool vivo = u != m.unita().end() && !u->second.distrutta && !u->second.persa;
        MESSAGE("seme ", seme, ": colpi sul 688 ", a ? a->colpi : 0, ", casse squarciate ", a ? a->casse_squarciate : 0, vivo ? std::string() : std::string(", affondato"));
        if (!a || a->casse_squarciate == 0 || !vivo) continue;
        provata = true;
        MESSAGE("seme ", seme, ": casse squarciate ", a->casse_squarciate, ", quota del 688 dell'IA ", m.unita().at(3).quota, " (ordinata ", m.unita().at(3).quota_ordinata, ")");
        // la scelta di comando: niente immersione ordinata (sotto il tiro sarebbe 60 m); la quota vera la fa la fisica
        // del kit (l'acqua imbarcata puo' appesantirlo comunque)
        CHECK(m.unita().at(3).quota_ordinata == doctest::Approx(0.0));
    }
    CHECK(provata);
}

TEST_CASE("artiglieria: acque territoriali in pace, avvertimenti al battello in superficie e bombe a quello immerso" * doctest::skip(!fs::exists(M688_DATI "/artiglieria.toml"))) {
    const std::string acque = "acque_territoriali = { { nome = 'Svezia', lato = 'neutrale', cerchio = { x = 0, y = 0, r = 12000 } } },";
    const std::string koni = "{ id = 2, classe = 'koni', categoria = 'nave', flag = 26, posizione = { x = 0, y = 3000 }, quota_m = 0, rotta = 180, velocita_kn = 3, " + NAVE_G8 + " },";
    // in superficie: colpi d'avvertimento, mai sul bersaglio
    PartitaG2 sopra(scena_g8("acque_sopra", "pace", giocatore_g8(0.0, 0.0, 0.0), koni, acque));
    sopra.avanza(300.0);
    REQUIRE(sopra.m.acque_territoriali().size() == 1);
    CHECK(sopra.m.acque_territoriali_di(sopra.m.unita().at(0)) == 0);
    const auto& imp = sopra.m.impatti_artiglieria();
    REQUIRE_FALSE(imp.empty());
    for (const auto& i : imp) CHECK(i.avvertimento);
    CHECK(quanti(sopra.m, TipoEvento::acque_territoriali) == 1);
    CHECK(std::any_of(sopra.m.messaggi().begin(), sopra.m.messaggi().end(), [](const Messaggio& x) { return x.codice == 453; }));
    // immerso, con la posizione dal ping della nave: bombe d'avvertimento lontane
    PartitaG2 sotto(scena_g8("acque_sotto", "pace", giocatore_g8(0.0, 0.0, 60.0), koni, acque,
                             "P = function(u) m688.impulso_sonar(u) end", "{ dopo_s = 20, unita = 2, gestore = 'P' }, { dopo_s = 140, unita = 2, gestore = 'P' }"));
    sotto.avanza(300.0);
    const auto& b = sotto.m.impatti_artiglieria();
    REQUIRE_FALSE(b.empty());
    for (const auto& i : b) {
        CHECK(i.pezzo == "bomba_avvertimento");
        CHECK(i.avvertimento);
        CHECK(distanza(i.punto, sotto.m.unita().at(0).pos) > 300.0);
    }
    CHECK(std::any_of(sotto.m.messaggi().begin(), sotto.m.messaggi().end(), [](const Messaggio& x) { return x.codice == 455; }));
    // fuori dalle acque, in pace: niente
    PartitaG2 fuori(scena_g8("acque_fuori", "pace", giocatore_g8(20000.0, 0.0, 0.0), koni, acque));
    fuori.avanza(300.0);
    CHECK(fuori.m.impatti_artiglieria().empty());
    CHECK(quanti(fuori.m, TipoEvento::acque_territoriali) == 0);
}

TEST_CASE("artiglieria: l'esperto spara sulla piuma della testa alzata, entro 1 km e col mare fino a 4" * doctest::skip(!fs::exists(M688_DATI "/artiglieria.toml"))) {
    // il 688 a quota periscopica col periscopio su, una Perry ostile al posto di combattimento accanto (22 §9.9)
    struct Prova { double d, kn; int mare; bool esperto, su; };
    const auto partita = [](const Prova& x) {
        auto p = std::make_unique<PartitaG2>(scena_g8("piuma", "guerra", giocatore_g8(0.0, 0.0, 6.096, x.kn),
                             "{ id = 2, classe = 'perry', categoria = 'nave', flag = 18, posizione = { x = 0, y = " + std::to_string(x.d) +
                             " }, quota_m = 0, rotta = 90, velocita_kn = " + std::to_string(x.kn) + ", " + NAVE_G8 + " },",
                             "meteo = { fisso = { stato_mare = " + std::to_string(x.mare) + " } }, gestori_giocatore = { periscopio = 'PERI' },",
                             "P = function(u) m688.posto_di_combattimento(u) end, PERI = function(u, p) m688.periscopio(u, p[1]); m688.salva_modello(u) end",
                             "{ dopo_s = 1, unita = 2, gestore = 'P' }"));
        if (x.esperto) p->m.imposta_difficolta_asw(DifficoltaAsw::minaccia_percepita);
        p->m.meteo_enhanced(M688_DATI);
        p->avanza(0.5);
        if (x.su) p->m.applica({p->m.tempo(), TipoOrdine::periscopio, 0, 0, 1});
        p->avanza(10.0);
        return p;
    };
    const auto vede = [&](const Prova& x) { auto p = partita(x); return p->m.vede_la_piuma(p->m.unita().at(2)); };
    CHECK(vede({900.0, 7.0, 3, true, true}));
    CHECK_FALSE(vede({900.0, 7.0, 3, false, true}));    // solo l'esperto
    CHECK_FALSE(vede({1500.0, 7.0, 3, true, true}));    // oltre 1 km
    CHECK_FALSE(vede({900.0, 7.0, 5, true, true}));     // col mare 5 i frangenti la nascondono
    CHECK_FALSE(vede({900.0, 2.0, 3, true, true}));     // piano, niente piuma
    CHECK_FALSE(vede({900.0, 7.0, 3, true, false}));    // periscopio giu'
    // e ci spara: il primo colpo arriva prima che al livello normale
    const auto primo = [&](const Prova& x) {
        auto p = partita(x);
        for (int s = 0; s < 120; ++s) {
            for (const auto& i : p->m.impatti_artiglieria()) if (i.bersaglio == 0 && !i.avvertimento) return p->m.tempo();
            p->avanza(1.0);
        }
        return 1e9;
    };
    CHECK(primo({900.0, 7.0, 4, true, true}) < primo({900.0, 7.0, 4, false, true}));
}
