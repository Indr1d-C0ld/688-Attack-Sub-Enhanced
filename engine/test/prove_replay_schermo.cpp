// Lo schermo del replay completo (app/replay.hpp; roadmap 16) su una traccia vera, registrata da SCENE4A
#include <doctest.h>

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <map>
#include <memory>
#include <set>
#include <string>

#include "../app/replay.hpp"
#include "grafica/tema_stazioni.hpp"
#include "missioni/danni_missione.hpp"
#include "missioni/missione.hpp"
#include "missioni/registro_missione.hpp"
#include "missioni/stazioni_missione.hpp"
#include "missioni/traccia_missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/traccia_replay.hpp"

using namespace m688;
namespace fs = std::filesystem;

namespace {

bool c_e() { return fs::exists(M688_MISSIONI "/SCENE4A.lua") && fs::exists(M688_DATI "/teatri/gibilterra.fdl"); }

// 15 minuti di SCENE4A nel classico, un fotogramma quando serve; la traccia resta anche su disco, per le istantanee
// (melchior688 --schermata replay --replay <TMPDIR>/m688_replay_scene4a.m688tr)
const Traccia& traccia() {
    static const Traccia t = [] {
        Missione m(M688_MISSIONI "/SCENE4A.lua", 1, false);
        StazioniMissione st(m, 1);
        RegistratoreTraccia reg("SCENE4A", 1);
        reg.metadato("codice", "4A0000000000000001");   // un codice partita di 18 caratteri (app/codice_partita)
        for (int k = 0; k < 900 * 9; ++k) {
            m.avanza(1.0 / 9.0);
            if (reg.serve(m.tempo(), m.eventi_missione().size())) {
                DatiApp app;
                app.stazione = 6;
                reg.aggiungi(fotogramma_da(m, &st, app, reg.ultimo_evento()), m.eventi_missione().size());
            }
        }
        reg.salva(fs::temp_directory_path() / "m688_replay_scene4a.m688tr");
        return leggi_traccia(reg.dati());
    }();
    return t;
}

Comandi nessuno() { return {}; }

}  // namespace

TEST_CASE("replay: lo schermo trova il teatro, disegna la carta e ha tutte le parti" * doctest::skip(!c_e())) {
    SchermataReplay r(M688_DATI, traccia());
    CHECK(r.teatro() == "gibilterra");
    CHECK(r.traccia().durata > 800.0);
    CHECK(r.in_pausa());
    CHECK(r.lettura() == SchermataReplay::Lettura::quadro);
    Tela t;
    Palette p;
    r.palette(p);
    for (const auto lettura : {SchermataReplay::Lettura::quadro, SchermataReplay::Lettura::verita}) {
        r.lettura(lettura);
        r.vai(450.0);
        t.riempi(0);
        r.disegna(t, Font::carica(M688_DATI "/font/originale.fnt"));
        // la carta grande (variante C) col fondale, non nera; la fascia del tempo sotto la carta, col cursore acceso
        const Rettangolo c = r.carta(), b = r.linea_tempo();
        CHECK(c.w > 600);
        CHECK(b.y > c.y + c.h);
        CHECK(b.y + b.h < 428);
        int colorati = 0;
        for (int y = c.y + 2; y < c.y + c.h; y += 7)
            for (int x = c.x + 2; x < c.x + c.w; x += 7) colorati += t.dati()[(y) * LARGHEZZA_TELA + (x)] != 0;
        CHECK(colorati > 1000);
        bool cursore = false;
        for (int x = b.x; x < b.x + b.w; ++x) cursore = cursore || t.dati()[(b.y) * LARGHEZZA_TELA + (x)] == fosforo(7);
        CHECK(cursore);
    }
    // il codice partita sta in fondo alla riga d'aiuto del piede, senza toccarla
    CHECK(r.traccia().metadati.at("codice") == "4A0000000000000001");
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    CHECK(14 + f.larghezza("P GIOCA/PAUSA  + - VELOCITA'  FRECCE 30 S  TAB EVENTO  I O ZOOM  CLIC: SALTA", 1, 1, false) + 12 +
              f.larghezza("CODICE 4A0000000000000001", 1, 1, false) <=
          626);
}

TEST_CASE("replay: gioca e pausa, velocita' da 1x a 64x, salti di 30 s, eventi, clic sulla timeline" * doctest::skip(!c_e())) {
    SchermataReplay r(M688_DATI, traccia());
    Comandi c = nessuno();
    // in pausa il tempo non passa
    CHECK_FALSE(r.comandi(c, 1.0));
    CHECK(r.tempo() == 0.0);
    // P: gioca a 1x
    c.pausa = true;
    r.comandi(c, 0.0);
    CHECK_FALSE(r.in_pausa());
    r.comandi(nessuno(), 2.0);
    CHECK(r.tempo() == doctest::Approx(2.0));
    // + fino a 64x, non oltre; - fino a 1x
    Comandi piu = nessuno();
    piu.compressione = 1;
    for (int k = 0; k < 10; ++k) r.comandi(piu, 0.0);
    CHECK(r.velocita() == 64.0);
    r.comandi(nessuno(), 1.0);
    CHECK(r.tempo() == doctest::Approx(66.0));
    Comandi meno = nessuno();
    meno.compressione = -1;
    for (int k = 0; k < 10; ++k) r.comandi(meno, 0.0);
    CHECK(r.velocita() == 1.0);
    // anche + e - (i tasti del telegrafo, qui liberi)
    Comandi telegrafo = nessuno();
    telegrafo.azioni = {Azione::telegrafo_avanti};
    r.comandi(telegrafo, 0.0);
    r.comandi(telegrafo, 0.0);
    CHECK(r.velocita() == 4.0);
    telegrafo.azioni = {Azione::telegrafo_indietro};
    for (int k = 0; k < 5; ++k) r.comandi(telegrafo, 0.0);
    CHECK(r.velocita() == 1.0);
    r.comandi(c, 0.0);   // pausa
    CHECK(r.in_pausa());
    // freccia destra: 30 s avanti, poi uno ogni 0,2 s tenendola
    Comandi destra = nessuno();
    destra.ruota = 1;
    r.comandi(destra, 0.0);
    CHECK(r.tempo() == doctest::Approx(96.0));
    r.comandi(destra, 0.1);
    CHECK(r.tempo() == doctest::Approx(96.0));
    r.comandi(destra, 0.15);
    CHECK(r.tempo() == doctest::Approx(126.0));
    Comandi sinistra = nessuno();
    sinistra.ruota = -1;
    r.comandi(sinistra, 0.0);
    CHECK(r.tempo() == doctest::Approx(96.0));
    // clic a meta' della fascia del tempo: a meta' della durata
    {
        Tela t;
        r.disegna(t, Font::carica(M688_DATI "/font/originale.fnt"));
    }
    const Rettangolo b = r.linea_tempo();
    Comandi clic = nessuno();
    clic.clic = {{b.x + (b.w - 1) / 2, b.y + b.h / 2}};
    r.comandi(clic, 0.0);
    CHECK(r.tempo() == doctest::Approx(r.traccia().durata / 2.0).epsilon(0.01));
    // Tab: l'evento successivo, nella verita' anche quelli che il giocatore non sapeva
    r.lettura(SchermataReplay::Lettura::verita);
    const auto segni = segnalibri(r.traccia());
    REQUIRE(segni.size() >= 2);
    r.vai(0.0);
    Comandi tab = nessuno();
    tab.tab = 1;
    r.comandi(tab, 0.0);
    double primo = -1.0;
    for (const auto& e : segni)
        if (e.tempo > 0.5) { primo = e.tempo; break; }
    CHECK(r.tempo() == doctest::Approx(primo));
    tab.tab = -1;
    r.vai(r.traccia().durata);
    r.comandi(tab, 0.0);
    CHECK(r.tempo() < r.traccia().durata);
    // V: l'altra lettura
    Comandi v = nessuno();
    v.verita = true;
    r.comandi(v, 0.0);
    CHECK(r.lettura() == SchermataReplay::Lettura::quadro);
}

TEST_CASE("replay: Invio sceglie l'istante per «Riprendi da qui», Esc esce senza" * doctest::skip(!c_e())) {
    {
        SchermataReplay r(M688_DATI, traccia());
        r.vai(300.0);
        Comandi c = nessuno();
        c.invio = true;
        CHECK(r.comandi(c, 0.0));
        REQUIRE(r.riprendi_da());
        CHECK(*r.riprendi_da() == doctest::Approx(300.0));
    }
    {
        SchermataReplay r(M688_DATI, traccia());
        Comandi c = nessuno();
        c.esc = true;
        CHECK(r.comandi(c, 0.0));
        CHECK_FALSE(r.riprendi_da());
    }
    // i tasti del piede col clic: V gira la lettura, INVIO riprende da qui
    {
        SchermataReplay r(M688_DATI, traccia());
        r.vai(200.0);
        Comandi c = nessuno();
        c.clic = {{320, 460}};
        CHECK_FALSE(r.comandi(c, 0.0));
        CHECK(r.lettura() == SchermataReplay::Lettura::verita);
        c.clic = {{530, 460}};
        CHECK(r.comandi(c, 0.0));
        REQUIRE(r.riprendi_da());
        CHECK(*r.riprendi_da() == doctest::Approx(200.0));
    }
    // senza teatro (missione sconosciuta) la carta e' il solo mare, e lo schermo funziona lo stesso
    Traccia t = traccia();
    t.missione = "NESSUNA";
    SchermataReplay r(M688_DATI, t);
    CHECK(r.teatro().empty());
    Tela tela;
    r.disegna(tela, Font::carica(M688_DATI "/font/originale.fnt"));
}

// Lo spettatore (docs/18, fase 5): le interazioni fra unita' dell'IA del registro arrivano alla timeline del replay;
// nel quadro del giocatore solo quelle note, nella verita' tutte. La scena S5 delle prove nell'Enhanced: il Victor
// lancia sul convoglio quando la TMA gli da' la soluzione (fase 4), quindi si gioca fino al primo lancio dell'IA, al
// piu' 20 minuti
namespace {
Traccia traccia_s5() {
    const auto classi = carica_classi(M688_DATI "/classi.toml");
    Missione m(M688_MISSIONI "/prove/PROVA_S5_CONVOGLIO.lua", 1);
    std::map<std::string, double> pescaggi;
    for (const auto& [k, c] : classi) pescaggi[k] = c.pescaggio_m;
    m.imposta_fondale(std::make_shared<const Fondale>(Fondale::carica(M688_DATI "/teatri/celtica.fdl")), pescaggi);
    m.radio_enhanced();
    DanniMissione dm(classi, M688_DATI, 1, RegolaDanni::simulazione);
    StazioniMissione st(m, 1);
    RegistratoreTraccia reg("PROVA_S5_CONVOGLIO", 1);
    const auto lanciato = [&] {
        return std::any_of(m.eventi_missione().begin(), m.eventi_missione().end(),
                           [](const EventoMissione& e) { return e.tipo == TipoEvento::lancio_ia; });
    };
    for (int k = 0; k < 1200 * 9 && !(m.tempo() >= 150.0 && lanciato()); ++k) {
        m.avanza(1.0 / 9.0);
        dm.avanza(m, 1.0 / 9.0);
        m.raccogli_danni(nullptr, &dm);
        if (reg.serve(m.tempo(), m.eventi_missione().size()))
            reg.aggiungi(fotogramma_da(m, &st, DatiApp{}, reg.ultimo_evento()), m.eventi_missione().size());
    }
    return leggi_traccia(reg.dati());
}
bool dell_ia(const EventoTraccia& e) { return e.tipo.size() > 3 && e.tipo.compare(e.tipo.size() - 3, 3, "_ia") == 0; }
bool c_e_s5() { return fs::exists(M688_MISSIONI "/prove/PROVA_S5_CONVOGLIO.lua") && fs::exists(M688_DATI "/teatri/celtica.fdl"); }
}  // namespace

TEST_CASE("replay: gli eventi dell'IA (lancio_ia, scoperta_ia...) arrivano alla timeline; Tab li trova nella verita'" * doctest::skip(!c_e_s5())) {
    const Traccia tr = traccia_s5();
    int ia = 0, noti = 0;
    std::set<std::string> tipi;
    for (const auto& e : segnalibri(tr)) {
        if (!dell_ia(e)) continue;
        ++ia;
        noti += e.noto;
        tipi.insert(e.tipo);
        CHECK(!e.testo_it.empty());
    }
    CHECK(tipi.count("lancio_ia"));
    CHECK(tipi.count("scoperta_ia"));
    int nel_quadro = 0;
    for (const auto& e : segnalibri(tr, 0, true)) nel_quadro += dell_ia(e);
    CHECK(nel_quadro == noti);
    // Tab nella verita' passa da un evento dell'IA all'altro
    SchermataReplay r(M688_DATI, tr);
    r.lettura(SchermataReplay::Lettura::verita);
    double primo = -1.0;
    for (const auto& e : segnalibri(tr))
        if (dell_ia(e) && e.tempo > 1.0) { primo = e.tempo; break; }
    REQUIRE(primo > 0.0);
    double prima = 0.0;   // l'evento prima di quello: Tab parte da li'
    for (const auto& e : segnalibri(tr))
        if (e.tempo < primo - 0.5) prima = e.tempo;
    r.vai(prima);
    Comandi tab = nessuno();
    tab.tab = 1;
    r.comandi(tab, 0.0);
    CHECK(r.tempo() == doctest::Approx(primo));
}

// docs/18 §Stato: scoperte, ingaggi ed evasioni dell'IA sono decisioni interne, con noto falso (nel quadro del
// giocatore non si vedono)
TEST_CASE("replay: le scoperte dell'IA non sono note al giocatore" * doctest::skip(!c_e_s5())) {
    for (const auto& e : segnalibri(traccia_s5()))
        if (e.tipo == "scoperta_ia" || e.tipo == "ingaggio_ia" || e.tipo == "evasione_ia") CHECK_FALSE(e.noto);
}

// --- Il modo dal vivo della sandbox (docs/19 §3): S5 nell'Enhanced, i fotogrammi registrati passati anche alla vista ---
namespace {
struct PartitaS5 {
    Traccia dal_file;
    std::unique_ptr<SchermataReplay> vista;
    double fine = 0.0;
};
PartitaS5& partita_s5_dal_vivo() {
    static PartitaS5 p = [] {
        PartitaS5 out;
        const auto classi = carica_classi(M688_DATI "/classi.toml");
        Missione m(M688_MISSIONI "/prove/PROVA_S5_CONVOGLIO.lua", 1);
        std::map<std::string, double> pescaggi;
        for (const auto& [k, c] : classi) pescaggi[k] = c.pescaggio_m;
        m.imposta_fondale(std::make_shared<const Fondale>(Fondale::carica(M688_DATI "/teatri/celtica.fdl")), pescaggi);
        m.radio_enhanced();
        DanniMissione dm(classi, M688_DATI, 1, RegolaDanni::simulazione);
        StazioniMissione st(m, 1);
        RegistratoreTraccia reg("PROVA_S5_CONVOGLIO", 1);
        RaccoltaTraccia raccolta;
        Traccia vuota;
        vuota.missione = "PROVA_S5_CONVOGLIO";
        vuota.seme = 1;
        out.vista = std::make_unique<SchermataReplay>(M688_DATI, vuota, "celtica");
        out.vista->dal_vivo(true);
        const auto lanciato = [&] {
            return std::any_of(m.eventi_missione().begin(), m.eventi_missione().end(),
                               [](const EventoMissione& e) { return e.tipo == TipoEvento::lancio_ia; });
        };
        for (int k = 0; k < 1200 * 9 && !(m.tempo() >= 200.0 && lanciato()); ++k) {
            m.avanza(1.0 / 9.0);
            dm.avanza(m, 1.0 / 9.0);
            m.raccogli_danni(nullptr, &dm);
            if (reg.serve(m.tempo(), m.eventi_missione().size())) {
                const Fotogramma f = fotogramma_da(m, &st, DatiApp{}, reg.ultimo_evento(), &raccolta);
                reg.aggiungi(f, m.eventi_missione().size());
                out.vista->aggiungi(f);
            }
            out.vista->adesso(m.tempo(), 8.0, false);
        }
        out.fine = m.tempo();
        out.dal_file = leggi_traccia(reg.dati());
        return out;
    }();
    return p;
}
int pixel(const Tela& t, int x, int y) { return t.dati()[y * LARGHEZZA_TELA + x]; }
}  // namespace

TEST_CASE("replay dal vivo: la traccia costruita coi fotogrammi e' quella del file" * doctest::skip(!c_e_s5())) {
    PartitaS5& p = partita_s5_dal_vivo();
    const Traccia& vivo = p.vista->traccia();
    CHECK(vivo.unita.size() == p.dal_file.unita.size());
    CHECK(vivo.registro.size() == p.dal_file.registro.size());
    CHECK(vivo.eventi_ia.size() == p.dal_file.eventi_ia.size());
    for (const auto& [id, serie] : p.dal_file.unita) {   // l'ultima posizione di ogni unita' (il file quantizza a 0,5 m)
        REQUIRE(vivo.unita.count(id));
        const auto& a = vivo.unita.at(id).back().second;
        const auto& b = serie.back().second;
        CHECK(distanza(a.pos, b.pos) < 1.0);
    }
}

TEST_CASE("replay dal vivo: adesso, indietro con la timeline e le frecce, Invio torna al vivo; le richieste a main" * doctest::skip(!c_e_s5())) {
    PartitaS5& p = partita_s5_dal_vivo();
    SchermataReplay& v = *p.vista;
    v.adesso(p.fine, 8.0, false);
    CHECK(v.al_vivo());
    CHECK(v.tempo() == doctest::Approx(p.fine));
    // P, + e -, Esc: richieste per main, la vista non cambia tempo
    Comandi c = nessuno();
    c.pausa = true;
    c.compressione = 1;
    RichiesteVivo r = v.comandi_vivo(c, 0.1);
    CHECK(r.pausa);
    CHECK(r.compressione == 1);
    CHECK_FALSE(r.fine);
    CHECK(v.tempo() == doctest::Approx(p.fine));
    c = nessuno();
    c.azioni = {Azione::telegrafo_indietro};
    CHECK(v.comandi_vivo(c, 0.1).compressione == -1);
    // un clic a meta' della timeline: indietro, e la partita che va avanti non lo sposta
    c = nessuno();
    const Rettangolo barra = v.linea_tempo();
    c.clic = {{barra.x + barra.w / 2, barra.y + barra.h / 2}};
    v.comandi_vivo(c, 0.1);
    CHECK_FALSE(v.al_vivo());
    CHECK(v.tempo() == doctest::Approx(p.fine / 2.0).epsilon(0.02));
    const double guardato = v.tempo();
    v.adesso(p.fine + 5.0, 8.0, false);
    CHECK(v.tempo() == doctest::Approx(guardato));
    // la freccia sinistra: l'evento precedente del registro
    c = nessuno();
    c.ruota = -1;
    v.comandi_vivo(c, 0.3);
    CHECK(v.tempo() < guardato);
    // Invio: di nuovo al vivo
    c = nessuno();
    c.invio = true;
    v.comandi_vivo(c, 0.1);
    CHECK(v.al_vivo());
    CHECK(v.tempo() == doctest::Approx(p.fine + 5.0));
    c = nessuno();
    c.esc = true;
    CHECK(v.comandi_vivo(c, 0.1).fine);
}

TEST_CASE("replay dal vivo: il registro coi filtri per parte e per tipo, Tab sceglie un'unita', il disegno" * doctest::skip(!c_e_s5())) {
    PartitaS5& p = partita_s5_dal_vivo();
    SchermataReplay& v = *p.vista;
    v.adesso(p.fine + 5.0, 8.0, false);
    v.filtri(0, 0);
    const std::size_t tutte = v.righe_registro();
    CHECK(tutte > 5);
    v.filtri(0, 2);   // armi: c'e' almeno il lancio del Victor
    const std::size_t armi = v.righe_registro();
    CHECK(armi >= 1);
    CHECK(armi < tutte);
    v.filtri(2, 2);   // le armi del Patto: il Victor
    CHECK(v.righe_registro() >= 1);
    v.filtri(1, 0);   // NATO e Patto insieme non superano il totale
    const std::size_t nato = v.righe_registro();
    v.filtri(2, 0);
    CHECK(nato + v.righe_registro() <= tutte);
    v.filtri(0, 0);
    // Tab: un'unita' scelta, la scheda in fondo alla colonna
    Comandi c = nessuno();
    c.tab = 1;
    v.comandi_vivo(c, 0.1);
    CHECK(v.scelta() >= 0);
    Tela t;
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    v.disegna(t, f);
    if (const char* d = std::getenv("M688_ISTANTANEE")) {   // la vista dal vivo della S5, con l'unita' scelta
        Palette pv = Palette::carica(M688_DATI "/immagini/MAIN.pal");
        v.palette(pv);
        t.salva_bmp(std::string(d) + "/vivo_s5.bmp", pv);
    }
    // lo schermo del registro ha del testo acceso sul vetro (il fosforo dal tono 4, o i colori degli eventi) nelle righe
    // e nella scheda in fondo (sette righe da 10)
    const Rettangolo rg = v.registro();
    const auto acceso = [&](int y0, int y1) {
        int n = 0;
        for (int y = y0; y < y1; ++y)
            for (int x = rg.x; x < rg.x + rg.w; ++x) {
                const int p = pixel(t, x, y);
                n += (p > 0 && p < 16) || p >= fosforo(4);
            }
        return n;
    };
    CHECK(acceso(rg.y + 16, rg.y + rg.h - 76) > 300);
    CHECK(acceso(rg.y + rg.h - 72, rg.y + rg.h) > 100);
    // un clic su una riga del registro porta la carta li' e smette di seguire
    c = nessuno();
    c.clic = {{rg.x + 40, rg.y + rg.h - 90}};
    v.comandi_vivo(c, 0.1);
    CHECK(v.al_vivo());   // senza Maiuscolo il tempo resta
}

TEST_CASE("replay dal vivo: V gira verita', parte NATO, parte Patto e unita'; il registro segue la parte e l'unita'" * doctest::skip(!c_e_s5())) {
    PartitaS5& p = partita_s5_dal_vivo();
    SchermataReplay& v = *p.vista;
    v.adesso(p.fine + 5.0, 8.0, false);
    v.lettura(SchermataReplay::Lettura::verita);
    v.filtri(0, 0);
    const std::size_t tutte = v.righe_registro();
    Comandi c = nessuno();
    c.verita = true;
    v.comandi_vivo(c, 0.1);
    CHECK(v.lettura() == SchermataReplay::Lettura::parte_nato);
    CHECK(v.righe_registro() < tutte);   // solo la NATO
    v.comandi_vivo(c, 0.1);
    CHECK(v.lettura() == SchermataReplay::Lettura::parte_patto);
    v.comandi_vivo(c, 0.1);
    CHECK(v.lettura() == SchermataReplay::Lettura::unita);
    const std::size_t dell_unita = v.righe_registro();   // il battello: i fatti suoi
    CHECK(dell_unita < tutte);
    Tela t;
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    for (int k = 0; k < 4; ++k) {   // ogni lettura si disegna
        v.disegna(t, f);
        int carta = 0;
        for (int y = 30; y < 360; y += 5)
            for (int x = 20; x < 460; x += 5) carta += pixel(t, x, y) != 0;
        CHECK(carta > 1000);
        v.comandi_vivo(c, 0.1);
    }
    CHECK(v.lettura() == SchermataReplay::Lettura::unita);
    v.lettura(SchermataReplay::Lettura::verita);
    v.filtri(0, 0);
}

TEST_CASE("replay dal vivo: G apre il pannello degli interventi, Esc lo chiude; l'intervento a main, Backspace lo annulla" *
          doctest::skip(!c_e_s5())) {
    PartitaS5& p = partita_s5_dal_vivo();
    SchermataReplay& v = *p.vista;
    v.adesso(p.fine + 5.0, 8.0, false);
    const auto premi = [&](auto&& imposta) {
        Comandi c = nessuno();
        imposta(c);
        return v.comandi_vivo(c, 0.1);
    };
    CHECK_FALSE(premi([](Comandi& c) { c.lettere = {'G'}; }).fine);
    REQUIRE(v.pannello_aperto());
    CHECK_FALSE(premi([](Comandi& c) { c.esc = true; }).fine);   // Esc chiude il pannello, non la partita
    CHECK_FALSE(v.pannello_aperto());
    // togli: G, FORZE, il secondo tipo, la prima unita', poi «fra» e APPLICA
    premi([](Comandi& c) { c.lettere = {'G'}; });
    premi([](Comandi& c) { c.invio = true; });
    premi([](Comandi& c) { c.tab = 1; });
    premi([](Comandi& c) { c.invio = true; });
    Tela t;
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    v.disegna(t, f);
    int pannello = 0;   // il pannello sopra la carta, a sinistra
    for (int y = 30; y < 200; ++y)
        for (int x = 12; x < 250; ++x) pannello += pixel(t, x, y) != 0;
    CHECK(pannello > 2000);
    if (const char* d = std::getenv("M688_ISTANTANEE")) {
        Palette pv = Palette::carica(M688_DATI "/immagini/MAIN.pal");
        v.palette(pv);
        t.salva_bmp(std::string(d) + "/vivo_pannello_interventi.bmp", pv);
    }
    premi([](Comandi& c) { c.invio = true; });
    premi([](Comandi& c) { c.tab = 1; });
    const RichiesteVivo r = premi([](Comandi& c) { c.invio = true; });
    REQUIRE(r.interventi.size() == 1);
    CHECK(r.interventi[0].tipo == "togli");
    CHECK(r.interventi[0].unita >= 0);
    CHECK(r.interventi[0].tempo == -1.0);
    CHECK_FALSE(v.pannello_aperto());
    CHECK(v.richieste().size() == 1);
    CHECK(premi([](Comandi& c) { c.cancella = true; }).annulla_ultimo);
    CHECK(v.richieste().empty());
    CHECK_FALSE(premi([](Comandi& c) { c.cancella = true; }).annulla_ultimo);
}

// Le tacche della linea del tempo sullo schermo vero: i colori del registro, gli eventi non noti solo nella verita'
namespace {
// i pixel della barra che non sono ne' del fondo ne' della barra stessa, colonna per colonna: il colore della tacca
std::set<int> colori_tacche(const Tela& t, Rettangolo b) {
    std::set<int> c;
    for (int y = b.y; y < b.y + b.h; ++y)
        for (int x = b.x; x < b.x + b.w; ++x)
            for (const std::uint8_t k : {std::uint8_t{colori::ROSSO}, std::uint8_t{colori::CIANO_CHIARO}, std::uint8_t{colori::GIALLO}, std::uint8_t{colori::BIANCO}, std::uint8_t{colori::GRIGIO}})
                if (t.at(x, y) == k) c.insert(k);
    return c;
}
}  // namespace

namespace {
std::uint8_t colore_del_tipo(const std::string& tipo) {
    EventoTraccia e;
    e.tipo = tipo;
    e.tempo = 1.0;
    return tacche_eventi({e}, 2.0, false).at(0).colore;
}

// La tavola decisa prima del codice (ghiaccio-d4-tacche-cartoline-07-11.md): rosso cio' che colpisce, fa danno o e' un'arma che
// parte; giallo il pericolo; ciano le scoperte
const std::map<std::uint8_t, std::set<std::string>>& colori_attesi() {
    static const std::map<std::uint8_t, std::set<std::string>> tavola = {
        {colori::ROSSO, {"lancio_giocatore", "lancio_nemico", "lancio_ia", "colpo", "affondamento", "artiglieria_ia", "collisione_ia", "danno_subito",
                         "scram", "camera_di_lancio_persa", "infrazione_roe", "incaglio", "fuoco_amico", "mina_posata", "mina_esplosa",
                         "artiglieria_salva", "artiglieria_colpo", "collisione", "cavo_impigliato", "avaria_ia", "colpo_esercitazione", "provocazione",
                         "fuoco_amico_ia", "esito_tlam"}},
        {colori::GIALLO, {"avvistato", "fix_radiogoniometria", "evasione_ia", "emersione_emergenza", "siluro_evitato", "esca_sedotta", "danno_domato",
                          "posti_di_combattimento", "ingresso_zona", "esm_rischio_alto", "ping_subito", "boa_vicina", "aereo_in_sorvolo",
                          "ping_sonar_immersione", "marcatore_avvistato", "attacco_simulato", "esposizione_lunga", "artiglieria_vicino", "quasi_collisione",
                          "emersione_forzata", "cavo_toccato", "acque_territoriali", "esca_ia", "ritirata_ia", "incsea", "allarme_siluro",
                          "attraversamento_mine"}},
        {colori::CIANO_CHIARO, {"scoperta", "classificazione", "scoperta_ia", "ingaggio_ia", "mina_scoperta"}},
        {colori::BIANCO, {"inizio", "fine"}},
        {colori::GRIGIO, {"intervento", "foto_periscopio", "rimorchio", "ordine_ricevuto", "messaggio_ricevuto", "verifica_radio", "allerta_forza",
                          "reazione_ia", "classificazione_ia", "comandante_ia", "tma_ia", "messaggio_perso", "compito_scaduto", "sessione_mancata",
                          "trasmissione", "ping_attivo", "transitorio_proprio", "scambio_tracce", "punto_nave", "fine_missile", "misura_telemetro",
                          "osservazione_aob", "ping_ecoscandaglio", "luci", "punto_nave_ia", "sonar_traino", "messaggio_ia"}},
    };
    return tavola;
}

// il massimo interesse che un tipo puo' raggiungere, passando le varianti che la formula guarda
int interesse_massimo(TipoEvento t) {
    EventoMissione e;
    e.tipo = t;
    e.giocatore = 0;
    int massimo = 0;
    for (const bool primo : {false, true})
        for (const int unita : {0, 5})
            for (const int altra : {0, 5})
                for (const int livello : {0, 1, 2, 3})
                    for (const int codice : {0, 1, 2, 3})
                        for (const double valore : {0.0, 1000.0})
                            for (const bool giusta : {false, true})
                                for (const bool noto : {false, true})
                                    for (const char* categoria : {"sottomarino", "nave"})
                                        for (const char* arma : {"", "attacco", "colpito"}) {
                                            e.unita = unita, e.altra = altra, e.livello = livello, e.codice = codice, e.valore = valore;
                                            e.giusta = giusta, e.noto = noto, e.categoria = categoria, e.classe_arma = arma, e.distanza_m = 1000.0;
                                            massimo = std::max(massimo, interesse(e, primo));
                                        }
    return massimo;
}
}  // namespace

TEST_CASE("replay: ogni tipo del registro ha il colore della tavola, e nessun evento grave resta grigio") {
    std::set<std::string> nel_registro;
    for (int i = 0; i < static_cast<int>(TipoEvento::numero); ++i) nel_registro.insert(nome_evento(static_cast<TipoEvento>(i)));
    std::set<std::string> in_tavola;
    for (const auto& [colore, tipi] : colori_attesi())
        for (const std::string& tipo : tipi) {
            CHECK_MESSAGE(in_tavola.insert(tipo).second, tipo);   // un solo colore per tipo
            CHECK_MESSAGE(colore_del_tipo(tipo) == colore, tipo);
        }
    CHECK(in_tavola == nel_registro);   // un tipo nuovo deve entrare nella tavola
    CHECK(colore_del_tipo("tipo_che_non_esiste") == colori::GRIGIO);
    // i colori chiesti per nome, a prescindere dalla tavola
    for (const char* tipo : {"collisione", "artiglieria_colpo", "incaglio", "danno_subito", "mina_esplosa"}) CHECK_MESSAGE(colore_del_tipo(tipo) == colori::ROSSO, tipo);
    for (const char* tipo : {"artiglieria_vicino", "quasi_collisione", "emersione_forzata", "siluro_evitato"}) CHECK_MESSAGE(colore_del_tipo(tipo) == colori::GIALLO, tipo);
    // grigi e alti: quelli con la loro ragione. A tutta altezza (da 70) solo l'intervento, che ha il triangolo della famiglia sopra la barra;
    // da 50 anche radio, foto, rimorchio e reazioni, che non colpiscono e non minacciano
    const std::set<std::string> grigi_da_50 = {"intervento", "foto_periscopio", "rimorchio", "ordine_ricevuto", "messaggio_ricevuto", "verifica_radio", "allerta_forza", "reazione_ia"};
    for (int i = 0; i < static_cast<int>(TipoEvento::numero); ++i) {
        const auto t = static_cast<TipoEvento>(i);
        const std::string nome = nome_evento(t);
        if (colore_del_tipo(nome) != colori::GRIGIO) continue;
        const int massimo = interesse_massimo(t);
        if (nome == "intervento") CHECK(massimo >= 70);
        else CHECK_MESSAGE(massimo < 70, nome);
        if (massimo >= 50) CHECK_MESSAGE(grigi_da_50.count(nome) == 1, nome);
    }
}

// La riga della lettura: rosso chiaro per tutti i rossi della tavola, anche i nuovi; il pericolo scampato e il resto col fosforo
TEST_CASE("replay: nella lettura la collisione e' scritta in rosso chiaro, il pericolo scampato no" * doctest::skip(!c_e())) {
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    const auto rosso_chiaro = [&](const char* tipo) {
        Traccia tr = traccia();
        EventoTraccia e;
        e.tempo = 450.4;
        e.tipo = tipo;
        e.testo_it = "PROVA";
        const auto dopo = std::find_if(tr.registro.begin(), tr.registro.end(), [&](const EventoTraccia& x) { return x.tempo > e.tempo; });
        tr.registro.insert(dopo, e);
        SchermataReplay r(M688_DATI, tr);
        r.vai(450.0);
        Tela t;
        r.disegna(t, f);
        // le righe degli ultimi eventi, in alto a destra sulla carta: il riquadro della lettura e' largo 256
        const Rettangolo c = r.carta();
        int n = 0;
        for (int y = c.y + 6; y < c.y + 6 + 56; ++y)
            for (int x = c.x + c.w - 6 - 256; x < c.x + c.w - 6; ++x) n += t.at(x, y) == 12;
        return n;
    };
    const int base = rosso_chiaro("trasmissione");
    CHECK(rosso_chiaro("collisione") > base);
    CHECK(rosso_chiaro("artiglieria_colpo") > base);
    CHECK(rosso_chiaro("lancio_giocatore") > base);
    CHECK(rosso_chiaro("quasi_collisione") == base);
    CHECK(rosso_chiaro("emersione_forzata") == base);
}

TEST_CASE("replay: le tacche hanno il colore e l'interesse dell'evento, nel QUADRO senza i non noti" * doctest::skip(!c_e_s5())) {
    const Traccia tr = traccia_s5();
    const auto segni = segnalibri(tr, 0, false);
    REQUIRE(segni.size() > 5);
    const auto tutte = tacche_eventi(segni, tr.durata, false);
    const auto note = tacche_eventi(segni, tr.durata, true);
    REQUIRE(tutte.size() == segni.size());
    std::size_t noti = 0;
    for (const auto& e : segni) noti += e.noto;
    CHECK(note.size() == noti);
    CHECK(note.size() < tutte.size());   // nella S5 le scoperte dell'IA non sono note
    for (std::size_t i = 0; i < segni.size(); ++i) {
        CHECK(tutte[i].u == doctest::Approx(segni[i].tempo / tr.durata));
        CHECK(tutte[i].interesse == segni[i].interesse);
        if (segni[i].tipo == "lancio_ia") CHECK(tutte[i].colore == colori::ROSSO);
        if (segni[i].tipo == "scoperta_ia" || segni[i].tipo == "scoperta") CHECK(tutte[i].colore == colori::CIANO_CHIARO);
    }
    // sullo schermo: nella verita' piu' di un colore, fra i quali il rosso del lancio
    SchermataReplay r(M688_DATI, tr);
    r.lettura(SchermataReplay::Lettura::verita);
    r.vai(0.0);
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    Tela t;
    r.disegna(t, f);
    const std::set<int> colori_v = colori_tacche(t, r.linea_tempo());
    CHECK(colori_v.size() >= 2);
    CHECK(colori_v.count(colori::ROSSO) == 1);
}

TEST_CASE("replay dal vivo: le tacche della linea del tempo hanno i colori del registro" * doctest::skip(!c_e_s5())) {
    PartitaS5& p = partita_s5_dal_vivo();
    SchermataReplay& v = *p.vista;
    v.adesso(p.fine + 5.0, 8.0, false);
    const Font f = Font::carica(M688_DATI "/font/originale.fnt");
    Tela t;
    v.disegna(t, f);
    const std::set<int> c = colori_tacche(t, v.linea_tempo());
    CHECK(c.size() >= 2);
    CHECK(c.count(colori::ROSSO) == 1);
}
