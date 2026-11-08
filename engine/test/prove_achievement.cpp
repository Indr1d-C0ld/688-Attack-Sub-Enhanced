// Achievement e albo d'onore (nucleo/achievement.hpp, nucleo/albo_onore.hpp, missioni/achievement_missione.hpp;
// roadmap 15, dossier 15 §7.10)
#include <doctest.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>

#include "missioni/achievement_missione.hpp"
#include "missioni/missione.hpp"
#include "nucleo/achievement.hpp"
#include "nucleo/albo_onore.hpp"
#include "nucleo/carriera.hpp"
#include "nucleo/classi.hpp"

using namespace m688;
namespace fs = std::filesystem;

namespace {

const CatalogoAchievement& catalogo() {
    static const CatalogoAchievement c = carica_achievement(M688_DATI "/achievement.toml");
    return c;
}

const RegoleCarriera& regole() {
    static const RegoleCarriera r = carica_carriera(M688_DATI "/carriera.toml");
    return r;
}

Misure missione(std::initializer_list<std::pair<const std::string, double>> v, const std::string& id = "SCENE1A") {
    Misure m;
    m.v = v;
    m.v["uno"] = 1.0;
    m.chiavi["missione"] = id;
    m.chiavi["marina"] = "usn";
    return m;
}

int livello_di(const std::vector<Sblocco>& s, const std::string& id) {
    for (const auto& x : s)
        if (x.id == id) return x.livello;
    return 0;
}

Conferimento conferimento(const std::string& id) {
    Conferimento k;
    k.decorazione = id;
    k.data = "1990-01";
    return k;
}

fs::path cartella_prova(const char* nome) {
    const fs::path d = fs::temp_directory_path() / nome;
    fs::remove_all(d);
    fs::create_directories(d);
    return d;
}

}  // namespace

TEST_CASE("achievement: il catalogo, i livelli e le misure note") {
    const auto& c = catalogo();
    CHECK(c.achievement.size() == 68);   // 41, le 14 di docs/26 (29/10/2026) e le 13 del secondo giro (31/10/2026)
    REQUIRE(c.cerca("fantasma"));
    CHECK(c.achievement.front().id == "primo_sangue");   // l'ordine del file
    const DefAchievement& cacciatore = *c.cerca("cacciatore");
    CHECK(cacciatore.ambito == AmbitoAchievement::somma);
    CHECK(cacciatore.livello(0) == 0);
    CHECK(cacciatore.livello(1) == 1);
    CHECK(cacciatore.livello(7) == 2);
    CHECK(cacciatore.livello(20) == 3);
    CHECK(c.cerca("primo_sangue")->livello(1) == 3);   // una soglia sola: l'oro
    int nascosti = 0;
    for (const auto& d : c.achievement) {
        nascosti += d.nascosto;
        CHECK_MESSAGE(misure_achievement_note().count(d.misura), d.id);
        CHECK_MESSAGE(!d.nome_en.empty(), d.id);
    }
    CHECK(nascosti == 5);
    // una misura che il nucleo non calcola e' un errore al caricamento
    const fs::path d = cartella_prova("m688_prova_achievement_catalogo");
    std::ofstream(d / "a.toml") << "[achievement.x]\nambito = \"missione\"\nmisura = \"inventata\"\nlivelli = [1]\n";
    CHECK_THROWS(carica_achievement((d / "a.toml").string()));
    fs::remove_all(d);
}

TEST_CASE("achievement: una missione, le somme, i valori distinti; i livelli non scendono") {
    StatoAchievement s;
    // il fantasma d'argento: nessuna localizzazione, nessun contatto ASW, ma un ping proprio
    auto m = missione({{"successo", 1}, {"fantasma", 2}, {"sottomarini_affondati", 1}, {"affondati_ostili", 1}, {"originale", 1}});
    auto e = valuta_achievement(catalogo(), m, s, "1989-08", "SCENE1A");
    CHECK(livello_di(e, "fantasma") == 2);
    CHECK(livello_di(e, "primo_sangue") == 3);
    CHECK(livello_di(e, "cacciatore") == 1);
    CHECK(s.p.at("fantasma").data == "1989-08");
    CHECK(s.avanzamento(*catalogo().cerca("originali")) == 1.0);
    // la stessa missione di nuovo: le somme crescono, i distinti no; un fantasma peggiore non toglie l'argento
    auto m2 = missione({{"successo", 1}, {"fantasma", 1}, {"sottomarini_affondati", 4}, {"originale", 1}});
    e = valuta_achievement(catalogo(), m2, s, "1989-10", "SCENE1A");
    CHECK(livello_di(e, "fantasma") == 0);
    CHECK(s.livello("fantasma") == 2);
    CHECK(livello_di(e, "cacciatore") == 2);   // 5 in tutto: argento
    CHECK(s.avanzamento(*catalogo().cerca("originali")) == 1.0);
    // senza la condizione (missione fallita) le somme condizionate non contano
    auto fallita = missione({{"successo", 0}, {"originale", 1}}, "SCENE2A");
    valuta_achievement(catalogo(), fallita, s, "1989-12", "SCENE2A");
    CHECK(s.avanzamento(*catalogo().cerca("originali")) == 1.0);
    // le condizioni sulle chiavi: il Barents senza emissioni
    auto barents = missione({{"successo", 1}, {"ping_attivi", 0}, {"trasmissioni", 0}});
    barents.chiavi["teatro"] = "barents";
    CHECK(livello_di(valuta_achievement(catalogo(), barents, s, "1990-02", "X"), "barents") == 3);
    auto norvegia = barents;
    norvegia.chiavi["teatro"] = "faroe";
    StatoAchievement altro;
    CHECK(livello_di(valuta_achievement(catalogo(), norvegia, altro, "1990-02", "X"), "barents") == 0);
}

TEST_CASE("achievement: la chiave a piu' valori (le classi classificate giuste) e le storiche della libreria") {
    StatoAchievement s;
    auto m = missione({{"successo", 1}});
    m.chiavi["classi_giuste"] = "perry|kidd|perry";
    valuta_achievement(catalogo(), m, s, "1989-08", "SCENE1A");
    CHECK(s.avanzamento(*catalogo().cerca("catalogo")) == 2.0);
    m.chiavi["classi_giuste"] = "kidd|kirov|";
    valuta_achievement(catalogo(), m, s, "1989-10", "SCENE2A");
    CHECK(s.avanzamento(*catalogo().cerca("catalogo")) == 3.0);
    // dieci classi diverse: il bronzo
    m.chiavi["classi_giuste"] = "oscar|alfa|akula|typhoon|krivak2|udaloy|slava";
    CHECK(livello_di(valuta_achievement(catalogo(), m, s, "1989-12", "SCENE3A"), "catalogo") == 1);
    // le storiche della libreria: distinte per missione, solo riuscite e solo LIB-S
    StatoAchievement t;
    auto st = missione({{"successo", 1}, {"libreria", 1}, {"libreria_storica", 1}}, "LIB_S23_ACQUA_A_DUE_GRADI");
    valuta_achievement(catalogo(), st, t, "1989-08", "LIB_S23_ACQUA_A_DUE_GRADI");
    valuta_achievement(catalogo(), st, t, "1989-09", "LIB_S23_ACQUA_A_DUE_GRADI");
    auto in = missione({{"successo", 1}, {"libreria", 1}, {"libreria_storica", 0}}, "LIB_I15_FANALI");
    valuta_achievement(catalogo(), in, t, "1989-10", "LIB_I15_FANALI");
    CHECK(t.avanzamento(*catalogo().cerca("storie_vere")) == 1.0);
    CHECK(t.avanzamento(*catalogo().cerca("biblioteca")) == 2.0);
    // il soccorso: un livello solo
    CHECK(catalogo().cerca("soccorso")->livelli.size() == 1);
}

TEST_CASE("achievement: il file, i globali e il comandante, le riprese") {
    const fs::path d = cartella_prova("m688_prova_achievement_file");
    const fs::path p = d / "achievement.toml";
    const auto m = missione({{"successo", 1}, {"affondati_ostili", 1}, {"originale", 1}});
    // una ripresa non conta per nessuno
    CHECK(registra_achievement(catalogo(), m, "halloran", true, true, "1989-08", "SCENE1A", p).globali.empty());
    CHECK_FALSE(fs::exists(p));
    // fuori carriera: solo i globali
    auto e = registra_achievement(catalogo(), m, "halloran", false, false, "1989-08", "SCENE1A", p);
    CHECK(livello_di(e.globali, "primo_sangue") == 3);
    CHECK(e.comandante.empty());
    // in carriera: anche il comandante; i globali li aveva gia'
    e = registra_achievement(catalogo(), m, "halloran", true, false, "1989-09", "SCENE1A", p);
    CHECK(livello_di(e.comandante, "primo_sangue") == 3);
    CHECK(livello_di(e.globali, "primo_sangue") == 0);
    const FileAchievement f = leggi_achievement(p);
    CHECK(f.globali.livello("primo_sangue") == 3);
    CHECK(f.comandanti.at("halloran").livello("primo_sangue") == 3);
    CHECK(f.comandanti.at("halloran").p.at("primo_sangue").missione == "SCENE1A");
    CHECK(f.globali.p.at("originali").chiavi.count("SCENE1A") == 1);
    // un file rovinato riparte da zero
    std::ofstream(p) << "non e' toml [[[";
    CHECK(leggi_achievement(p).globali.p.empty());
    fs::remove_all(d);
}

TEST_CASE("achievement: le misure di una missione vera e del fascicolo") {
    Missione m(std::string(M688_MISSIONI) + "/SCENE1A.lua", 1, false);
    for (int k = 0; k < 30; ++k) m.avanza(1.0);
    ContestoMissione c;
    c.missione = "SCENE1A";
    c.teatro = "faroe";
    c.situazione = Situazione::crisi;
    Fascicolo f = Carriera::nuovo(regole(), Marina::usn, "J. R. Halloran", true, Indulgenza::realistica);   // al comando
    OpzioniMisure prima;
    prima.fascicolo = &f;
    prima.regole = &regole();
    const double personali = misure_achievement(m, c, prima)["decorazioni_personali"];   // quelle del fascicolo precompilato
    f.decorazioni.push_back(conferimento("navy_cross"));
    f.decorazioni.push_back(conferimento("legion_of_merit"));
    OpzioniMisure o;
    o.fascicolo = &f;
    o.regole = &regole();
    o.armi_a_bordo = 21;
    const Misure x = misure_achievement(m, c, o);
    CHECK(x["uno"] == 1.0);
    CHECK(x["originale"] == 1.0);
    CHECK(x["crisi"] == 1.0);
    CHECK(x["usn"] == 1.0);
    CHECK(x["battello_perso"] == 0.0);
    CHECK(x["armi_a_bordo"] == 21.0);
    CHECK(x["grado_ordine"] >= 4.0);
    CHECK(x["decorazione_massima"] == 2.0);
    CHECK(x["decorazioni_personali"] == personali + 2.0);
    CHECK(x.chiavi.at("missione") == "SCENE1A");
    CHECK(x.chiavi.at("teatro") == "faroe");
    CHECK(x.chiavi.at("marina") == "usn");
    CHECK(x.chiavi.count("codice") == 0);
    for (const auto& n : misure_achievement_note()) CHECK_MESSAGE(x.v.count(n) + (n.find("qualifica") == 0 ? 1 : 0) > 0, n);
    // una rapida porta il suo codice
    c.origine = "rapida";
    c.seme = 0xabcdef;
    CHECK(misure_achievement(m, c).chiavi.at("codice") == "0000000000abcdef");
    // la carriera: al comando e decorato (bronzo)
    StatoAchievement s;
    const auto e = valuta_achievement(catalogo(), x, s, "1990-04", "SCENE1A");
    CHECK(livello_di(e, "al_comando") == 3);
    CHECK(livello_di(e, "decorato") == (personali + 2.0 >= 5.0 ? 2 : 1));
    CHECK(livello_di(e, "massima") == 2);
    // col fascicolo di prima: grado e qualifiche d'ingresso della carriera breve non si guadagnano; la Navy Cross si'
    const Fascicolo nuovo = Carriera::nuovo(regole(), Marina::usn, "J. R. Halloran", true, Indulgenza::realistica);
    o.fascicolo_prima = &nuovo;
    StatoAchievement s2;
    const auto e2 = valuta_achievement(catalogo(), misure_achievement(m, c, o), s2, "1990-04", "SCENE1A");
    CHECK(livello_di(e2, "al_comando") == 0);
    CHECK(livello_di(e2, "dolphins") == 0);
    CHECK(livello_di(e2, "massima") == 2);
    const DefAchievement& decorato = *catalogo().cerca("decorato");
    CHECK(livello_di(e2, "decorato") == (decorato.livello(personali + 2.0) > decorato.livello(personali) ? decorato.livello(personali + 2.0) : 0));
    // la stessa missione senza cambi nel fascicolo: niente di carriera
    o.fascicolo = &nuovo;
    StatoAchievement s3;
    for (const auto& x3 : valuta_achievement(catalogo(), misure_achievement(m, c, o), s3, "1990-04", "SCENE1A"))
        CHECK_MESSAGE(catalogo().cerca(x3.id)->ambito != AmbitoAchievement::carriera, x3.id);
}

TEST_CASE("achievement: le misure del secondo giro nella missione: in coda, entro 500 yd, il soccorso (runtime, 31/10/2026)") {
    // il 688 del giocatore 1,5 km in coda a un Victor III alla sua velocita', con un Kilo a 350 m al traverso; lo script
    // dice tre uomini salvati su tre
    const auto f = fs::temp_directory_path() / "m688_prova_achievement_coda.lua";
    const std::string SUB = "modello = { massima_kn = 32, virata_gradi_s = 1, accel_kn_s = 0.5, quota_m_s = 1, quota_max_m = 450, integrita = 200, rumore = 8, sonar = 19 }";
    std::ofstream(f) << "return { id = 'PROVA_ACH_CODA', teatro = 'faroe', lato = 'usa', data = '1989-06-15', ora_utc = 12, paesaggio_sonoro = false,"
                        " script = { S = function(u) m688.salvati(u, 3, 3) end }, varianti = { { probabilita = 1, unita = {\n"
                        "{ id = 0, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, giocatore = true, posizione = { x = 0, y = 0 },"
                        "  quota_m = 150, rotta = 0, velocita_kn = 8, " << SUB << " },\n"
                        "{ id = 2, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = 1500 }, quota_m = 150, rotta = 0,"
                        "  velocita_kn = 8, " << SUB << " },\n"
                        "{ id = 3, classe = 'kilo', categoria = 'sottomarino', flag = 17, posizione = { x = 350, y = 0 }, quota_m = 100, rotta = 0,"
                        "  velocita_kn = 8, " << SUB << " },\n"
                        "}, timer = { { dopo_s = 5, unita = 2, gestore = 'S' } } } } }\n";
    Missione m(f.string(), 1, false);
    m.acustica_enhanced(M688_DATI);
    m.dinamica_sottomarini(carica_classi(M688_DATI "/classi.toml"), M688_DATI);
    for (int i = 0; i < 300 * 9; ++i) m.avanza(1.0 / 9.0);
    const auto& k = m.conti_giocatore();
    MESSAGE("in coda ", k.in_coda_s, " s, entro 500 yd ", k.entro_500yd_s, " s");
    CHECK(k.in_coda_s > 200.0);
    CHECK(k.entro_500yd_s > 200.0);
    ContestoMissione c;
    c.missione = "PROVA_ACH_CODA";
    const Misure x = misure_achievement(m, c, {});
    CHECK(x.v.at("tutti_salvati") == 1.0);
    CHECK(x.v.at("in_coda_s") == doctest::Approx(k.in_coda_s));
    CHECK(x.v.at("emersioni_ghiaccio") == 0.0);
    CHECK(x.v.at("libreria_storica") == 0.0);
    fs::remove(f);
}

TEST_CASE("albo d'onore: migliori missioni senza aiuti, carriere chiuse coi caduti, comandanti storici") {
    AlboOnore a;
    VoceOnoreMissione v;
    v.missione = "SCENE1A";
    v.punteggio = 300;
    CHECK(proponi_missione(a, v, false, 1.0) == 1);
    v.punteggio = 500;
    v.missione = "SCENE2A";
    CHECK(proponi_missione(a, v, false, 1.0) == 1);
    CHECK(proponi_missione(a, v, false, 0.9) == 0);    // con aiuti
    CHECK(proponi_missione(a, v, true, 1.0) == 0);     // ripresa
    v.punteggio = 300;
    v.missione = "SCENE3B";
    CHECK(proponi_missione(a, v, false, 1.0) == 3);    // a parita' resta prima la piu' vecchia
    for (int k = 0; k < ONORE_MISSIONI; ++k) {
        v.punteggio = 1000 + k;
        proponi_missione(a, v, false, 1.0);
    }
    CHECK(static_cast<int>(a.missioni.size()) == ONORE_MISSIONI);
    v.punteggio = 10;
    CHECK(proponi_missione(a, v, false, 1.0) == 0);
    // carriere: in servizio no, caduto si'; lo stesso comandante si aggiorna
    Fascicolo f = Carriera::nuovo(regole(), Marina::vmf, "V. A. Suchkov", true, Indulgenza::realistica);
    CHECK(proponi_carriera(a, f, &regole()) == 0);
    VoceMissione vm;
    vm.punti = 400;
    f.missioni = {vm, vm};
    f.stato = "morto";
    f.decorazioni.push_back(conferimento("stella_rossa"));
    f.decorazioni.push_back(conferimento("bandiera_rossa"));
    CHECK(proponi_carriera(a, f, &regole()) == 1);
    CHECK(a.carriere.front().stato == "morto");
    CHECK(a.carriere.front().punti == doctest::Approx(800));
    CHECK(a.carriere.front().decorazione == regole().decorazione("bandiera_rossa")->nome);   // la piu' alta
    f.missioni.push_back(vm);
    CHECK(proponi_carriera(a, f, &regole()) == 1);
    CHECK(a.carriere.size() == 1);
    // il file
    const fs::path d = cartella_prova("m688_prova_albo_onore");
    salva_albo_onore(a, d / "albo_onore.toml");
    const AlboOnore b = leggi_albo_onore(d / "albo_onore.toml");
    CHECK(b.missioni.size() == a.missioni.size());
    CHECK(b.missioni.front().punteggio == doctest::Approx(a.missioni.front().punteggio));
    CHECK(b.carriere.front().nome == "V. A. Suchkov");
    CHECK(registra_missione_onore(v, false, 0.5, d / "albo_onore.toml") == 0);
    fs::remove_all(d);
    // i comandanti storici: fatti senza punteggi, in ordine d'anno
    const auto st = carica_storici(M688_DATI "/albo_onore_storico.toml");
    REQUIRE(st.size() == 10);
    CHECK(st.front().anno == 1958);
    for (const auto& i : st) {
        CHECK_MESSAGE(!i.fonte.empty(), i.id);
        CHECK_MESSAGE(!i.impresa_it.empty(), i.id);
    }
}
