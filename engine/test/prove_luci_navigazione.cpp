// Le luci di navigazione (docs/27): i dati di luci.toml rispettano il COLREG, i settori, i lampi del faro ambra e i
// fanali accesi per stato
#include <doctest.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <string>

#include "grafica/font.hpp"
#include "grafica/periscopio.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/geometria.hpp"
#include "nucleo/fari.hpp"
#include "nucleo/luci_navigazione.hpp"
#include "nucleo/periscopi.hpp"
#include "nucleo/portata_luci.hpp"

#ifndef M688_DATI
#define M688_DATI "../data/motore"
#endif

using namespace m688;

namespace {
const luci::Fanale* trova(const luci::Configurazione& c, const std::string& ruolo) {
    for (const auto& f : c.fanali)
        if (f.ruolo == ruolo) return &f;
    return nullptr;
}
bool ha(const std::vector<const luci::Fanale*>& v, const std::string& ruolo) {
    return std::any_of(v.begin(), v.end(), [&](const luci::Fanale* f) { return f->ruolo == ruolo; });
}
}  // namespace

TEST_CASE("luci: i settori del COLREG, compreso il bordo e il coronamento che passa per la poppa") {
    luci::Fanale albero, sinistro, dritto, coronamento, tutto;
    albero.settore_da = -112.5, albero.settore_a = 112.5;
    sinistro.settore_da = -112.5, sinistro.settore_a = 0.0;
    dritto.settore_da = 0.0, dritto.settore_a = 112.5;
    coronamento.settore_da = 112.5, coronamento.settore_a = -112.5;
    CHECK(luci::nel_settore(albero, 0.0));
    CHECK(luci::nel_settore(albero, 112.5));
    CHECK_FALSE(luci::nel_settore(albero, 150.0));
    CHECK(luci::nel_settore(sinistro, -45.0));
    CHECK_FALSE(luci::nel_settore(sinistro, 45.0));   // il rosso non si vede da dritta
    CHECK(luci::nel_settore(dritto, 45.0));
    CHECK_FALSE(luci::nel_settore(dritto, -45.0));
    CHECK(luci::nel_settore(coronamento, 180.0));
    CHECK(luci::nel_settore(coronamento, -150.0));
    CHECK(luci::nel_settore(coronamento, 540.0));     // gli angoli si normalizzano
    CHECK_FALSE(luci::nel_settore(coronamento, 90.0));
    for (const double a : {-180.0, -90.0, 0.0, 77.0, 180.0}) CHECK(luci::nel_settore(tutto, a));
}

TEST_CASE("luci: il faro ambra fa tre lampi e tre secondi di buio, in un ciclo di 6 s") {
    int lampi = 0;
    bool prima = false;
    for (int k = 0; k < 600; ++k) {   // 6 s a passi di 10 ms
        const bool ora = luci::lampo_acceso(k * 0.01);
        if (ora && !prima) ++lampi;
        prima = ora;
        if (k * 0.01 >= 3.0) CHECK_FALSE(ora);
    }
    CHECK(lampi == 3);
    CHECK(luci::lampo_acceso(6.2) == luci::lampo_acceso(0.2));
    CHECK(luci::stato_da_nome("pesca") == luci::Stato::pesca);
    // lo stato predefinito: spente in guerra per le militari, pesca per i pescherecci lenti, fonda se ferma
    constexpr double NODO = 1852.0 / 3600.0;
    CHECK(luci::stato_predefinito("mercantile", 12 * NODO, false, false) == luci::Stato::moto);
    CHECK(luci::stato_predefinito("mercantile", 0.0, false, false) == luci::Stato::fonda);
    CHECK(luci::stato_predefinito("mercantile", 12 * NODO, true, false) == luci::Stato::moto);   // in guerra il mercantile no
    CHECK(luci::stato_predefinito("militare", 20 * NODO, true, false) == luci::Stato::spente);
    CHECK(luci::stato_predefinito("militare", 20 * NODO, false, false) == luci::Stato::moto);    // in pace accese
    CHECK(luci::stato_predefinito("sottomarino", 10 * NODO, true, false) == luci::Stato::spente);
    CHECK(luci::stato_predefinito("peschereccio", 3 * NODO, false, false) == luci::Stato::pesca);
    CHECK(luci::stato_predefinito("peschereccio", 10 * NODO, false, false) == luci::Stato::moto);
    CHECK(luci::stato_predefinito("mercantile", 12 * NODO, false, true) == luci::Stato::spente);
    bool ok = true;
    CHECK(luci::stato_da_nome("nessuna", &ok) == luci::Stato::spente);
    CHECK_FALSE(ok);
}

TEST_CASE("luci: i fanali delle classi rispettano il COLREG e si accendono per stato" *
          doctest::skip(!std::filesystem::exists(std::string(M688_DATI) + "/luci.toml"))) {
    const auto tutte = luci::carica(std::string(M688_DATI) + "/luci.toml");
    REQUIRE(tutte.size() >= 30);
    for (const auto& [classe, c] : tutte) {
        CAPTURE(classe);
        const auto* albero = trova(c, "albero");
        REQUIRE(albero);
        CHECK(trova(c, "laterale_sinistro"));
        CHECK(trova(c, "laterale_dritto"));
        CHECK(trova(c, "coronamento"));
        CHECK(trova(c, "laterale_sinistro")->colore == luci::Colore::rosso);
        CHECK(trova(c, "laterale_dritto")->colore == luci::Colore::verde);
        CHECK(albero->portata_m >= 3.0 * 1852.0 - 1.0);
        // il secondo fanale d'albero solo da 50 m in su, piu' alto di almeno 4,5 m e a poppavia
        if (const auto* p = trova(c, "albero_poppiero")) {
            CHECK(c.lunghezza_m >= 50.0);
            CHECK(p->pos_m[2] >= albero->pos_m[2] + 4.4);
            CHECK(p->pos_m[0] < albero->pos_m[0]);
        }
        // in moto: albero, laterali e coronamento; spente: niente; alla fonda: il fanale di prora e non i laterali
        const auto moto = luci::accesi(c, luci::Stato::moto, 10.0);
        CHECK(ha(moto, "albero"));
        CHECK(ha(moto, "coronamento"));
        CHECK(luci::accesi(c, luci::Stato::spente, 10.0).empty());
        const auto fonda = luci::accesi(c, luci::Stato::fonda, 10.0);
        CHECK(ha(fonda, "fonda_prora"));
        CHECK_FALSE(ha(fonda, "laterale_dritto"));
        // senza governo: i due rossi, uno sopra l'altro
        const auto governo = luci::accesi(c, luci::Stato::non_governa, 10.0);
        CHECK(ha(governo, "governo_rosso_alto"));
        CHECK(ha(governo, "governo_rosso_basso"));
        CHECK_FALSE(ha(governo, "albero"));   // senza governo niente fanale d'albero (regola 27 a)
    }
    // i pescherecci: in pesca verde su bianco (strascico) o rosso su bianco (cianciolo), e le luci da lavoro
    const auto& pesch = tutte.at("peschereccio");
    const auto pesca = luci::accesi(pesch, luci::Stato::pesca, 0.0);
    CHECK(ha(pesca, "pesca_alto"));
    CHECK(ha(pesca, "coperta"));
    CHECK(trova(pesch, "pesca_alto")->colore == luci::Colore::verde);
    CHECK(trova(pesch, "pesca_alto")->pos_m[2] > trova(pesch, "pesca_basso")->pos_m[2]);
    CHECK(trova(tutte.at("cianciolo"), "pesca_alto")->colore == luci::Colore::rosso);
    // il faro ambra solo sui sottomarini americani, e lampeggia
    const auto& la = tutte.at("los_angeles");
    REQUIRE(trova(la, "faro_ambra"));
    CHECK(trova(la, "faro_ambra")->lampi_7077);
    CHECK(ha(luci::accesi(la, luci::Stato::moto, 0.2), "faro_ambra"));
    CHECK_FALSE(ha(luci::accesi(la, luci::Stato::moto, 4.0), "faro_ambra"));
    CHECK_FALSE(trova(tutte.at("alfa"), "faro_ambra"));
    CHECK_FALSE(trova(tutte.at("trafalgar"), "faro_ambra"));
    // il mercantile da 170 m ha i due fanali d'albero
    CHECK(trova(tutte.at("mercantile"), "albero_poppiero"));
}

TEST_CASE("luci: la legge di Allard, la foschia e il giorno") {
    const double sei_miglia = 6.0 * 1852.0;
    // la portata nominale: alla soglia con la visibilita' dell'Annesso I (K 0,8 per miglio, Koschmieder 0,02)
    const double vis_annesso = 3.912 / (-std::log(0.8) / 1852.0);
    CHECK(luce_fanale(sei_miglia, sei_miglia, vis_annesso, 0.0) == doctest::Approx(1.0).epsilon(0.02));
    CHECK(luce_fanale(sei_miglia, 3000.0, 18520.0, 0.0) > luce_fanale(sei_miglia, 6000.0, 18520.0, 0.0));
    CHECK(luce_fanale(sei_miglia, 3000.0, 3000.0, 0.0) < luce_fanale(sei_miglia, 3000.0, 18520.0, 0.0) / 10.0);
    CHECK(luce_fanale(sei_miglia, 3000.0, 18520.0, 0.08) == doctest::Approx(luce_fanale(sei_miglia, 3000.0, 18520.0, 0.0)));
    CHECK(luce_fanale(sei_miglia, 6000.0, 18520.0, 1.0) < 1.0);   // di giorno non si vede
    CHECK(luce_fanale(0.0, 100.0, 18520.0, 0.0) == 0.0);
}

TEST_CASE("luci: nella vista i fanali del mercantile, coi settori; di giorno, spente o senza dati niente" *
          doctest::skip(!std::filesystem::exists(std::string(M688_DATI) + "/luci.toml"))) {
    const std::string dati = M688_DATI;
    const Font f = Font::carica(dati + "/font/originale.fnt");
    const auto tutte = carica_classi(dati + "/classi.toml");
    std::map<std::string, ClasseNave> classi{{"mercantile", tutte.at("mercantile")}};
    std::map<std::string, InsiemeSprite> sprite{{"mercantile", InsiemeSprite::carica(dati + "/" + classi.at("mercantile").sprite)}};
    const auto configurazioni = luci::carica(dati + "/luci.toml");
    Periscopio p(f, classi, sprite);
    Nave occhio;
    occhio.id = -1;
    // il mercantile a 3 km a nord; `rotta` 180 viene verso di noi (prora), 0 si allontana (poppa)
    const auto conta = [&](double rotta, luci::Stato stato, double luce, bool con_dati) {
        Mondo m;
        Nave n;
        n.id = 7, n.classe = "mercantile", n.pos = {0.0, 3000.0}, n.rotta = rotta, n.velocita = 6.0;
        m.aggiungi(n);
        VistaPeriscopio v;
        v.luce = luce;
        v.altezza_onde_m = 0.0;
        if (con_dati) {
            v.luci.configurazioni = &configurazioni;
            v.luci.stati[7] = stato;
        }
        Tela t;
        t.riempi(0);
        p.disegna(t, m, occhio, v);
        std::map<int, int> c;
        for (int y = 0; y < ALTEZZA_VISTA; ++y)
            for (int x = 0; x < LARGHEZZA_TELA; ++x) ++c[t.at(x, y)];
        return c;
    };
    const auto prora = conta(180.0, luci::Stato::moto, 0.04, true);
    CHECK(prora.count(12));   // il rosso
    CHECK(prora.count(10));   // il verde
    const auto poppa = conta(0.0, luci::Stato::moto, 0.04, true);
    CHECK_FALSE(poppa.count(12));   // da poppa niente laterali: il coronamento solo
    CHECK_FALSE(poppa.count(10));
    const auto senza = conta(180.0, luci::Stato::moto, 0.04, false);
    const auto spente = conta(180.0, luci::Stato::spente, 0.04, true);
    CHECK(spente == senza);   // spente o senza dati: la vista di oggi
    const auto giorno = conta(180.0, luci::Stato::moto, 1.0, true);
    CHECK_FALSE(giorno.count(12));
    CHECK_FALSE(giorno.count(10));
}

TEST_CASE("luci: di notte i fanali vicini si riflettono sul mare sotto la loro linea d'acqua; di giorno no" *
          doctest::skip(!std::filesystem::exists(std::string(M688_DATI) + "/luci.toml"))) {
    const std::string dati = M688_DATI;
    const Font f = Font::carica(dati + "/font/originale.fnt");
    const auto tutte = carica_classi(dati + "/classi.toml");
    std::map<std::string, ClasseNave> classi{{"mercantile", tutte.at("mercantile")}};
    std::map<std::string, InsiemeSprite> sprite{{"mercantile", InsiemeSprite::carica(dati + "/" + classi.at("mercantile").sprite)}};
    const auto configurazioni = luci::carica(dati + "/luci.toml");
    Periscopio p(f, classi, sprite);
    Nave occhio;
    occhio.id = -1;
    // il mercantile a 1 km che viene verso di noi, mare calmo: il riflesso dei laterali e dell'albero scende sotto la
    // linea d'acqua (riga 182 circa) verso lo specchio, a circa (z + h) / r di depressione (riga 193 per 10 m)
    const auto riflessi = [&](double luce, bool con_dati, double tempo) {
        Mondo m;
        Nave n;
        n.id = 7, n.classe = "mercantile", n.pos = {0.0, 1000.0}, n.rotta = 180.0, n.velocita = 6.0;
        m.aggiungi(n);
        VistaPeriscopio v;
        v.luce = luce;
        v.altezza_onde_m = 0.0;
        v.tempo_s = tempo;
        if (con_dati) {
            v.luci.configurazioni = &configurazioni;
            v.luci.stati[7] = luci::Stato::moto;
        }
        Tela t;
        t.riempi(0);
        p.disegna(t, m, occhio, v);
        int c = 0;
        for (int y = 186; y < ALTEZZA_VISTA; ++y)
            for (int x = 0; x < LARGHEZZA_TELA; ++x)
                if (const auto i = t.at(x, y); i == 12 || i == 4 || i == 10 || i == 2) ++c;
        return c;
    };
    CHECK(riflessi(0.04, true, 0.0) > 0);
    CHECK(riflessi(0.04, false, 0.0) == 0);
    CHECK(riflessi(1.0, true, 0.0) == 0);
    // le scintille cambiano col tempo (un decimo di secondo), il riflesso resta
    CHECK(riflessi(0.04, true, 5.0) > 0);
}

TEST_CASE("luci: di notte il fanale resta almeno un pixel fino alla sua portata, anche oltre l'orizzonte, a 1,5x e a 6x" *
          doctest::skip(!std::filesystem::exists(std::string(M688_DATI) + "/luci.toml"))) {
    const std::string dati = M688_DATI;
    const Font f = Font::carica(dati + "/font/originale.fnt");
    const auto tutte = carica_classi(dati + "/classi.toml");
    std::map<std::string, ClasseNave> classi{{"mercantile", tutte.at("mercantile")}, {"peschereccio", tutte.at("peschereccio")}};
    std::map<std::string, InsiemeSprite> sprite;
    for (const auto& [id, c] : classi) sprite.emplace(id, InsiemeSprite::carica(dati + "/" + c.sprite));
    const auto configurazioni = luci::carica(dati + "/luci.toml");
    Periscopio p(f, classi, sprite);
    Nave occhio;
    occhio.id = -1;
    // la nave che viene verso di noi a `r` metri: quanti pixel rossi (il laterale) e bianchi (l'albero); notte limpida,
    // mare calmo, la testa a 1,5 m
    const auto pixel = [&](const std::string& classe, double r, bool alto) {
        Mondo m;
        Nave n;
        n.id = 7, n.classe = classe, n.pos = {0.0, r}, n.rotta = 180.0, n.velocita = 6.0;
        m.aggiungi(n);
        VistaPeriscopio v;
        v.luce = 0.04;
        v.altezza_onde_m = 0.0;
        v.oculare.attivo = true;
        v.oculare.campo_gradi = alto ? 8.0 : 32.0;
        v.luci.configurazioni = &configurazioni;
        v.luci.stati[7] = luci::Stato::moto;
        Tela t;
        t.riempi(0);
        p.disegna(t, m, occhio, v);
        int rossi = 0, bianchi = 0;
        for (int y = 0; y < ALTEZZA_VISTA; ++y)
            for (int x = 0; x < LARGHEZZA_TELA; ++x) {
                const int c = t.at(x, y);
                rossi += c == 12 || c == 4;
                bianchi += c == 15 || c == 7;
            }
        return std::pair{rossi, bianchi};
    };
    // fin dove il fanale si vede: la luce sopra la soglia (visibilita' di 30 km, come la vista) e sopra l'orizzonte
    // geometrico, con la testa a 1,5 m
    const auto portata = [&](const std::string& classe, const char* ruolo) {
        const luci::Fanale* fa = nullptr;
        for (const auto& x : configurazioni.at(classe).fanali)
            if (x.ruolo == ruolo) fa = &x;
        REQUIRE(fa);
        const double dh = distanza_orizzonte(1.5), re = RAGGIO_TERRA_RIFRAZIONE;
        double r = 500.0;
        for (;; r += 100.0) {
            const double d = r + 100.0, oltre = std::max(0.0, d - dh);
            if (luce_fanale(fa->portata_m, d, 30000.0, 0.04) < 1.0 || fa->pos_m[2] <= oltre * oltre / (2.0 * re)) break;
        }
        return r;
    };
    for (const std::string classe : {"mercantile", "peschereccio"}) {
        const double rosso = portata(classe, "laterale_sinistro"), bianco = portata(classe, "albero");
        CAPTURE(classe);
        CAPTURE(rosso);
        CAPTURE(bianco);
        for (const bool alto : {false, true}) {
            CAPTURE(alto);
            for (double r = 1000.0; r <= bianco - 200.0; r += 250.0) {
                CAPTURE(r);
                const auto [rs, bs] = pixel(classe, r, alto);
                // il laterale fino alla sua portata; a 1,5x, da lontano, quello di un peschereccio cade nello stesso
                // pixel dell'albero (3 m sotto, a 5 km: un terzo di pixel) e la luce resta, bianca
                if (r <= rosso - 200.0) CHECK((alto ? rs : rs + bs) > 0);
                CHECK(bs > 0);                           // l'albero fino alla sua, anche oltre l'orizzonte (4,7 km)
            }
        }
    }
}

TEST_CASE("luci: l'ottica del periscopio a 6x porta i fanali piu' lontano nella foschia; a 1,5x quasi niente" *
          doctest::skip(!std::filesystem::exists(std::string(M688_DATI) + "/luci.toml"))) {
    const std::string dati = M688_DATI;
    const Font f = Font::carica(dati + "/font/originale.fnt");
    const auto tutte = carica_classi(dati + "/classi.toml");
    std::map<std::string, ClasseNave> classi{{"cargo", tutte.at("cargo")}};
    std::map<std::string, InsiemeSprite> sprite{{"cargo", InsiemeSprite::carica(dati + "/" + classi.at("cargo").sprite)}};
    const auto configurazioni = luci::carica(dati + "/luci.toml");
    Periscopio p(f, classi, sprite);
    Nave occhio;
    occhio.id = -1;
    // la I15 (trailer-1, 29/10/2026): il cargo senza governo a 6 km, foschia a 15 km, notte; i due rossi e il laterale
    const auto rossi = [&](double guadagno) {
        Mondo m;
        Nave n;
        n.id = 7, n.classe = "cargo", n.pos = {0.0, 6000.0}, n.rotta = 270.0, n.velocita = 1.0;
        m.aggiungi(n);
        VistaPeriscopio v;
        v.luce = 0.0;
        v.visibilita_m = 15000.0;
        v.altezza_onde_m = 0.0;
        v.oculare.attivo = true;
        v.oculare.campo_gradi = 8.0;
        v.oculare.guadagno_luci = guadagno;
        v.luci.configurazioni = &configurazioni;
        v.luci.stati[7] = luci::Stato::non_governa;
        Tela t;
        t.riempi(0);
        p.disegna(t, m, occhio, v);
        int c = 0;
        for (int y = 0; y < ALTEZZA_VISTA; ++y)
            for (int x = 0; x < LARGHEZZA_TELA; ++x) c += t.at(x, y) == 12 || t.at(x, y) == 4;
        return c;
    };
    const double g6 = luci::guadagno_ottico(30.0, 0.3, 6.0), g15 = luci::guadagno_ottico(30.0, 0.3, 1.5);
    CHECK(g6 > 4.0);
    CHECK(g15 < 1.0);
    CHECK(rossi(1.0) == 0);   // a occhio nudo, oltre i 4,4 km della foschia, niente
    CHECK(rossi(g6) > 0);     // a 6x si', fino a circa 7 km
    CHECK(rossi(g15) == 0);
    // il periscopio dei dati: il Type 2 a 6x ha quel guadagno
    const auto per = carica_periscopi(dati + "/periscopi.toml");
    CHECK(per.periscopi.at("type2").obiettivo_mm == doctest::Approx(30.0));
}

// I fari della costa (C9 §2.15, kit 31/10/2026): i lampi dalla caratteristica, il file di Wikidata, la vista di notte
TEST_CASE("fari: i lampi della caratteristica, il file dei teatri e il faro nella vista di notte") {
    Faro f;
    f.id = "Q0";
    f.periodo_s = 20.0;
    f.lampi = {2.0, 1.5, 2.0, 14.5};   // LFl(2) 20s
    int accesi = 0;
    for (int k = 0; k < 2000; ++k) accesi += faro_acceso(f, k * 0.01 * 20.0 / 20.0);   // 20 s a passi di 0,01
    CHECK(accesi == doctest::Approx(400).epsilon(0.02));   // 4 s accesi su 20
    Faro fisso;
    CHECK(faro_acceso(fisso, 123.4));                     // luce fissa: sempre accesa
    Faro altro = f;
    altro.id = "Q1";
    int diversi = 0;
    for (int k = 0; k < 200; ++k) diversi += faro_acceso(f, k * 0.1) != faro_acceso(altro, k * 0.1);
    CHECK(diversi > 0);                                    // due fari uguali non lampeggiano in fase
    if (std::filesystem::exists(std::string(M688_DATI) + "/fari.toml")) {
        const auto tutti = carica_fari(std::string(M688_DATI) + "/fari.toml");
        CHECK(tutti.size() > 20);
        const auto faroe = carica_fari(std::string(M688_DATI) + "/fari.toml", "faroe");
        CHECK_FALSE(faroe.empty());
        for (const auto& x : tutti) {
            CHECK(x.portata_m > 0.0);
            CHECK(x.quota_m > 0.0);
            CHECK(x.anno <= 1989);   // solo quelli accesi nel 1989 (0: anno ignoto)
        }
    }
    // nella vista: un faro bianco a 8 km a nord, di notte; spento o di giorno niente
    const std::string dati = M688_DATI;
    const Font fnt = Font::carica(dati + "/font/originale.fnt");
    std::map<std::string, ClasseNave> classi;
    std::map<std::string, InsiemeSprite> sprite;
    Periscopio p(fnt, classi, sprite);
    Nave occhio;
    occhio.id = -1;
    const auto bianchi = [&](bool acceso, double luce) {
        Mondo m;
        VistaPeriscopio v;
        v.luce = luce;
        v.altezza_onde_m = 0.0;
        v.oculare.attivo = true;   // i fari sono dell'Enhanced: l'oculare del 6x
        v.oculare.campo_gradi = 8.0;
        v.oculare.passo_gradi = 1.0;
        v.oculare.passo_fine_gradi = 0.25;
        VistaPeriscopio::FaroVista fv;
        fv.pos = {0.0, 8000.0};
        fv.quota_m = 40.0;
        fv.portata_m = 20.0 * 1852.0;
        fv.acceso = acceso;
        v.fari.push_back(fv);
        Tela t;
        t.riempi(0);
        p.disegna(t, m, occhio, v);
        return t;
    };
    const auto pixel_diversi = [](const Tela& a, const Tela& b) {
        int n = 0;
        for (int y = 0; y < ALTEZZA_VISTA; ++y)
            for (int x = 0; x < LARGHEZZA_TELA; ++x) n += a.at(x, y) != b.at(x, y);
        return n;
    };
    CHECK(pixel_diversi(bianchi(true, 0.04), bianchi(false, 0.04)) > 0);   // di notte il lampo si vede, nel buio no
    CHECK(pixel_diversi(bianchi(true, 1.0), bianchi(false, 1.0)) == 0);    // di giorno non si vede
}

TEST_CASE("fari: la terra fra l'occhio e la lanterna copre la luce") {
    // il faro a 20 km verso nord, la lanterna a 30 m; l'occhio del periscopio a 1,5 m
    const Vec2 occhio{0, 0}, faro{0, 20000};
    const auto mare = [](Vec2) { return -50.0; };
    CHECK_FALSE(faro_coperto(mare, 200.0, occhio, 1.5, faro, 30.0));
    // un'isola alta 40 m a meta' strada: la copre
    const auto isola = [](Vec2 p) { return std::hypot(p.x, p.y - 10000.0) < 800.0 ? 40.0 : -50.0; };
    CHECK(faro_coperto(isola, 200.0, occhio, 1.5, faro, 30.0));
    // la stessa isola, bassa (3 m): la retta a meta' strada sta a ~15 m, meno il rigonfiamento della Terra (~6 m): passa
    const auto bassa = [](Vec2 p) { return std::hypot(p.x, p.y - 10000.0) < 800.0 ? 3.0 : -50.0; };
    CHECK_FALSE(faro_coperto(bassa, 200.0, occhio, 1.5, faro, 30.0));
    // il capo su cui sta il faro (alto 60 m entro 200 m dalla lanterna) non conta
    const auto capo = [&](Vec2 p) { return std::hypot(p.x - faro.x, p.y - faro.y) < 200.0 ? 60.0 : -50.0; };
    CHECK_FALSE(faro_coperto(capo, 200.0, occhio, 1.5, faro, 30.0));
}
