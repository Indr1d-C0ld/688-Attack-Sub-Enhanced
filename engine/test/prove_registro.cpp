#include <doctest.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "missioni/danni_sottomarini.hpp"
#include "missioni/missione.hpp"
#include "nucleo/classi.hpp"
#include "missioni/registro_missione.hpp"
#include "nucleo/ordini.hpp"

using namespace m688;
namespace fs = std::filesystem;

// Registro degli eventi di missione ed esito dettagliato (M1, missioni/registro_missione.hpp).

namespace {

constexpr double DT = 1.0 / 9.0;
const std::string MISSIONI = M688_MISSIONI;
bool c_e(const std::string& id) { return fs::exists(MISSIONI + "/" + id + ".lua"); }

int quanti(const Missione& m, TipoEvento t) {
    int n = 0;
    for (const auto& e : m.eventi_missione()) n += e.tipo == t;
    return n;
}

const EventoMissione* primo(const Missione& m, TipoEvento t) {
    for (const auto& e : m.eventi_missione()) if (e.tipo == t) return &e;
    return nullptr;
}

// il 688 in superficie, una fregata ostile (2) a `distanza` m, una nave neutrale (3) lontana; obiettivi nella missione
std::string scena(double distanza, double esito_dopo_s = -1.0) {
    const auto f = fs::temp_directory_path() / "m688_prova_registro.lua";
    const std::string modello = "modello = { massima_kn = 30, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 100 }";
    std::ofstream o(f);
    o << "return { id = 'PROVA_REGISTRO', teatro = 'faroe', paesaggio_sonoro = false,\n"
         "esiti = { [6] = { it = 'Bene.' }, [7] = { it = 'Male.' } },\n"
         "categorie_esiti = { [6] = 'successo', [7] = 'fallimento' },\n"
         "obiettivi = {\n"
         "  { id = 'fregata', tipo = 'affondare', unita = { 2 }, testo = { it = 'Affondare la fregata', en = 'Sink the frigate' } },\n"
         "  { id = 'neutrale', tipo = 'proteggere', unita = { 3 }, primario = false, testo = { it = 'Non toccare la nave neutrale' } },\n"
         "  { id = 'vivo', tipo = 'sopravvivere' },\n"
         "},\n"
         "script = { FINE = function(u) m688.messaggio(u, 106) end },\n"
         "varianti = { { probabilita = 1, unita = {\n"
         "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
         "  posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 0, velocita_kn = 4, " << modello << " },\n"
         "{ id = 2, classe = 'krivak', categoria = 'nave', flag = 18, posizione = { x = 0, y = " << distanza << " },"
         "  quota_m = 0, rotta = 90, velocita_kn = 10, " << modello << " },\n"
         "{ id = 3, classe = 'mercantile', categoria = 'nave', flag = 2, posizione = { x = 60000, y = 60000 },"
         "  quota_m = 0, rotta = 0, velocita_kn = 10, " << modello << " },\n"
         "}";
    if (esito_dopo_s >= 0.0) o << ", timer = { { dopo_s = " << esito_dopo_s << ", unita = 0, gestore = 'FINE' } }";
    o << " } } }\n";
    return f.string();
}

}  // namespace

TEST_CASE("registro: scoperte, contro-scoperta, esposizione e obiettivi della missione") {
    Missione m(scena(3000.0), 1, false);
    for (int i = 0; i < 9 * 120; ++i) m.avanza(DT);
    REQUIRE(m.errori().empty());
    REQUIRE_FALSE(m.eventi_missione().empty());
    CHECK(m.eventi_missione().front().tipo == TipoEvento::inizio);
    // la fregata vede il battello in superficie entro 9 nm: contro-scoperta, una volta sola nei primi due minuti
    const auto* a = primo(m, TipoEvento::avvistato);
    REQUIRE(a);
    CHECK(a->unita == 2);
    CHECK(a->lato == Lato::ostile);
    CHECK_FALSE(a->noto);
    CHECK(a->inquadratura.soggetto == 2);
    CHECK(a->inquadratura.secondo == 0);
    CHECK(quanti(m, TipoEvento::avvistato) == 1);
    CHECK_FALSE(a->testo_it.empty());
    // ogni evento ha un id distinto, in ordine di tempo
    for (std::size_t i = 1; i < m.eventi_missione().size(); ++i)
        CHECK(m.eventi_missione()[i - 1].tempo <= m.eventi_missione()[i].tempo);

    auto e = m.esito_dettagliato();
    CHECK(e.categoria == EsitoDettagliato::Categoria::in_corso);
    REQUIRE(e.obiettivi.size() == 3);
    CHECK(e.obiettivi[0].id == "fregata");
    CHECK(e.obiettivi[0].stato == Obiettivo::Stato::in_corso);
    CHECK(e.obiettivi[0].richiesti == 1);
    CHECK(e.obiettivi[1].primario == false);
    CHECK(e.avvistamenti == 1);
    CHECK(e.tempo_esposto_s > 100.0);   // in superficie
    CHECK(e.tempo_contatto_asw_s > 100.0);

    // la fregata affondata (dal motore): obiettivo raggiunto, completezza parziale finche' la missione e' aperta
    m.distruggi(2);
    for (int i = 0; i < 9 * 120; ++i) m.avanza(DT);
    const auto* af = primo(m, TipoEvento::affondamento);
    REQUIRE(af);
    CHECK(af->unita == 2);
    CHECK(af->classe == "krivak");
    CHECK(af->interesse >= 70);
    e = m.esito_dettagliato();
    CHECK(e.obiettivi[0].stato == Obiettivo::Stato::raggiunto);
    CHECK(e.obiettivi[0].quota() == doctest::Approx(1.0));
    CHECK(e.obiettivi[1].stato == Obiettivo::Stato::in_corso);
    CHECK(e.completezza == doctest::Approx(1.0));   // l'unico obiettivo attivo; il vincolo non e' violato
    CHECK(e.riuscita == doctest::Approx(1.0));
    CHECK(e.per_bersaglio.at(2).affondato);
    CHECK_FALSE(e.per_bersaglio.at(2).dal_giocatore);
    CHECK(e.affondati_ostili == 0);   // non dal giocatore
}

TEST_CASE("registro: l'esito dell'originale chiude la missione con la sua categoria") {
    Missione m(scena(30000.0, 60.0), 1, false);
    for (int i = 0; i < 9 * 90; ++i) m.avanza(DT);
    REQUIRE(m.esito());
    CHECK(*m.esito() == 6);
    const auto* f = primo(m, TipoEvento::fine);
    REQUIRE(f);
    CHECK(f->codice == 6);
    CHECK(f->interesse == 100);
    const auto e = m.esito_dettagliato();
    CHECK(e.categoria == EsitoDettagliato::Categoria::successo);
    CHECK(e.testo_it == "Bene.");
    CHECK(e.obiettivi[0].stato == Obiettivo::Stato::fallito);    // la fregata e' viva
    CHECK(e.obiettivi[1].stato == Obiettivo::Stato::raggiunto);  // la neutrale e' intatta
    CHECK(e.obiettivi[2].stato == Obiettivo::Stato::raggiunto);  // il battello e' vivo
    // momenti salienti: la fine c'e' sempre, in ordine di tempo
    const auto s = m.momenti_salienti(3);
    REQUIRE_FALSE(s.empty());
    CHECK(s.back()->tipo == TipoEvento::fine);
    for (std::size_t i = 1; i < s.size(); ++i) CHECK(s[i - 1]->tempo <= s[i]->tempo);
}

TEST_CASE("registro: lancio del giocatore dalla missione originale, con l'arma e il bersaglio" * doctest::skip(!c_e("SCENE1A"))) {
    Missione m(MISSIONI + "/SCENE1A.lua", 1);
    int bersaglio = -1;
    for (const auto& [id, u] : m.unita()) if (u.categoria == "nave") { bersaglio = id; break; }
    REQUIRE(bersaglio >= 0);
    m.avanza(2.0);
    CHECK(m.applica({0, TipoOrdine::designa_bersaglio, 0, 0, bersaglio}));
    CHECK(m.applica({0, TipoOrdine::lancia, 0, 0, 1}));
    m.avanza(4.0);
    const auto* l = primo(m, TipoEvento::lancio_giocatore);
    REQUIRE(l);
    CHECK(l->unita == m.giocatore()->id);
    CHECK(l->altra == bersaglio);
    CHECK(l->classe_arma == "mk48");
    CHECK(l->arma >= 0);
    CHECK(l->inquadratura.soggetto == l->arma);
    CHECK(l->testo_it.find("Mk 48") != std::string::npos);
    const auto e = m.esito_dettagliato();
    CHECK(e.lanci == 1);
    CHECK(e.per_arma.at("mk48").lanci == 1);
}

TEST_CASE("registro: deterministico, e il classico non cambia" * doctest::skip(!c_e("SCENE4A"))) {
    const auto gira = [](std::vector<EventoMissione>& ev, std::vector<Messaggio>& msg) {
        Missione m(MISSIONI + "/SCENE4A.lua", 7);
        for (int i = 0; i < 9 * 1800; ++i) m.avanza(DT);
        ev = m.eventi_missione();
        msg = m.messaggi();
        CHECK(m.errori().empty());
    };
    std::vector<EventoMissione> a, b;
    std::vector<Messaggio> ma, mb;
    gira(a, ma);
    gira(b, mb);
    REQUIRE(a.size() == b.size());
    CHECK(a.size() > 1);
    for (std::size_t i = 0; i < a.size(); ++i) {
        CHECK(a[i].id == b[i].id);
        CHECK(a[i].tipo == b[i].tipo);
        CHECK(a[i].tempo == b[i].tempo);
        CHECK(a[i].unita == b[i].unita);
        CHECK(a[i].testo_it == b[i].testo_it);
        CHECK(a[i].interesse == b[i].interesse);
    }
    REQUIRE(ma.size() == mb.size());
    // ogni evento ha il suo nome, testo e interesse
    for (const auto& e : a) {
        CHECK(std::string(nome_evento(e.tipo)) != "?");
        CHECK_FALSE(e.testo_it.empty());
        CHECK_FALSE(e.testo_en.empty());
        CHECK(e.interesse >= 0);
        CHECK(e.interesse <= 100);
    }
}

TEST_CASE("registro: danni subiti dai rapporti del battello, con la gravita' e senza minicartolina" * doctest::skip(!fs::exists(M688_DATI "/classi.toml"))) {
    const auto classi = carica_classi(std::string(M688_DATI) + "/classi.toml");
    Missione m(scena(30000.0), 1, false);
    DanniSottomarini ds(classi, M688_DATI, 1);
    ds.prepara(m, 0);
    const auto gioca = [&](double s) {
        for (double t = 0; t < s; t += DT) { m.avanza(DT); ds.avanza(m, DT); m.raccogli_danni(&ds, nullptr); }
    };
    gioca(20);
    ds.urto(m, 0, 135, 15, 1.5707963);   // carica da 135 kg a 15 m sotto lo scafo
    gioca(60);
    const auto* d = primo(m, TipoEvento::danno_subito);
    REQUIRE(d);
    CHECK_FALSE(d->inquadratura.visibile);
    CHECK(d->inquadratura.icona == "danni");
    CHECK_FALSE(d->testo_it.empty());
    CHECK(d->valore > 0.0);
    for (std::size_t i = 1; i < m.eventi_missione().size(); ++i)
        CHECK(m.eventi_missione()[i - 1].tempo <= m.eventi_missione()[i].tempo);
    const auto e = m.esito_dettagliato();
    CHECK(e.danno_massimo > 0.0);
    CHECK(e.margine_collasso == doctest::Approx(1.0));   // in superficie
    CHECK(e.stimati);
    CHECK(e.feriti == 0);
}

// «scoprire» conta anche la classificazione giusta fatta a mano dalla stazione, ma solo al primo tentativo (trailer-1,
// 31/10/2026: lo Sturgeon silenzioso della S24 che il sonarista non classifica mai): giusta subito, l'obiettivo e'
// raggiunto; sbagliata e poi giusta, no
TEST_CASE("registro: «scoprire» con la classificazione a mano giusta al primo tentativo") {
    const auto gioca = [](const std::vector<std::string>& classi) {
        const auto f = fs::temp_directory_path() / "m688_prova_registro_scoprire.lua";
        const std::string modello = "modello = { massima_kn = 30, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 100 }";
        std::ofstream(f) << "return { id = 'PROVA_SCOPRIRE', teatro = 'faroe', paesaggio_sonoro = false,\n"
                            "obiettivi = { { id = 'intruso', tipo = 'scoprire', unita = { 2 }, testo = { it = 'Classificare il sottomarino' } } },\n"
                            "script = {}, varianti = { { probabilita = 1, unita = {\n"
                            "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true,"
                            "  posizione = { x = 0, y = 0 }, quota_m = 100, rotta = 0, velocita_kn = 4, " << modello << " },\n"
                            "{ id = 2, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 20000 },"
                            "  quota_m = 100, rotta = 90, velocita_kn = 5, " << modello << " },\n"
                            "} } } }\n";
        Missione m(f.string(), 1, false);
        for (int i = 0; i < 9 * 10; ++i) m.avanza(DT);
        for (const auto& c : classi) m.classifica_contatto(0, 2, c);
        for (int i = 0; i < 9 * 10; ++i) m.avanza(DT);
        fs::remove(f);
        const auto e = m.esito_dettagliato();
        REQUIRE_FALSE(e.obiettivi.empty());
        return e.obiettivi[0].stato == Obiettivo::Stato::raggiunto;
    };
    CHECK(gioca({"victor3"}));
    CHECK_FALSE(gioca({"akula", "victor3"}));
    CHECK_FALSE(gioca({"akula"}));
}
