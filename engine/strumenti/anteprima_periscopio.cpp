// Anteprime dell'oculare dei periscopi dell'Enhanced (docs/14_periscopi.md, P3): una scena di prova (una Perry a 3 km,
// una Krivak II piu' lontana, un SH-60B in volo) vista coi periscopi e i modi di periscopi.toml. Senza finestra.
// Uso: anteprima_periscopio <dati> <cosa> <uscita.bmp>
//   cosa: classico, type2_15, type2_6, type18_12, elevazione, stadimetro, filtro_giallo, type18_notte, type18_tv,
//         alfa_intensificatore, alfa_tv1, alfa_verticale, alfa_verticale_ghiaccio, trafalgar_termico, tremolio, sott_acqua,
//         mare_2, mare_5, mare_7, onde (sei momenti di un'onda sulla finestra, mare 5), pioggia,
//         beaufort (la schiuma da forza 3 a 10: creste, strisce, spruzzaglia, mare bianco), creste (sei momenti a mare 6
//         con la testa a 1 m: la Perry a 3 km dietro le creste vicine, l'orizzonte frastagliato),
//         artiglieria (sei cadute: 5", 76 mm, 16", VT, rimbalzo, AK-630), artiglieria_notte (vampa, lampo, traccianti),
//         artiglieria_vetro (acqua delle colonne, vetro incrinato, immagine spenta), artiglieria_illuminante,
//         luci_notte, luci_foschia, luci_6x (docs/27: i fanali di un mercantile, un peschereccio in pesca, una nave alla
//         fonda, una Perry e un Los Angeles col faro ambra, di notte; con la foschia a 3 km; a 6x sul mercantile),
//         luci_calma, luci_mosso (le stesse col mare calmo e mosso: il riflesso dei fanali),
//         gruppo_mare5, gruppo_mare5_6x (la S17: il gruppo portaerei di notte col mare 5; le variabili d'ambiente
//         ANTEPRIMA_TEMPO, ANTEPRIMA_TESTA e ANTEPRIMA_ONDE cambiano l'istante, la testa sopra l'acqua e l'onda),
//         ghiaccio_polynya, ghiaccio_6x, ghiaccio_notte, ghiaccio_nebbia, ghiaccio_margine, ghiaccio_nubi_margine,
//         ghiaccio_nubi_canale (la banchisa), costa_fiordo, costa_fiordo_6x, costa_notte, costa_arcipelago,
//         costa_lontana (la terraferma), classe:<nome>
//         (una classe sola a 1,5 km, Type 2 6x: il foglio di controllo dei modelli, tools/navi/foglio_controllo.py)
// ANTEPRIMA_TEMPI stampa il costo del disegno.

#include <algorithm>
#include <cmath>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <optional>
#include <string>

#include "grafica/effetti.hpp"
#include "grafica/oculare.hpp"
#include "grafica/periscopio.hpp"
#include "nucleo/campo_ghiaccio.hpp"
#include "nucleo/luci_navigazione.hpp"
#include "nucleo/onde.hpp"
#include "risorse.hpp"

using namespace m688;

namespace {

Nave nave(int id, const char* classe, double rilevamento_gradi, double distanza_m, double rotta, double quota = 0.0) {
    Nave n;
    n.id = id;
    n.classe = classe;
    n.pos = {distanza_m * std::sin(radianti(rilevamento_gradi)), distanza_m * std::cos(radianti(rilevamento_gradi))};
    n.pos_prec = n.pos;
    n.rotta = n.rotta_prec = rotta;
    n.quota = quota;
    return n;
}

// Campo e reticolo dal modello (periscopi.toml) per un ingrandimento
ParametriOculare modello(const std::string& id, int ingrandimento) {
    ParametriOculare p;
    p.attivo = true;
    if (id == "type2" || id == "type18" || id == "ch84") {
        const double campi[] = {32, 8, 4}, passi[] = {4, 1, 0.5}, fini[] = {1, 0.25, 0.125};
        p.campo_gradi = campi[ingrandimento];
        p.passo_gradi = passi[ingrandimento];
        p.passo_fine_gradi = fini[ingrandimento];
        p.reticolo = id == "ch84" ? Reticolo::britannico : Reticolo::americano;
    } else {   // sovietici: 1,5x/40 gradi e 6x/10 gradi, griglia a 5 e 1 gradi (1 e 0,2 a 6x)
        const double campi[] = {40, 10}, passi[] = {5, 1}, fini[] = {1, 0.2};
        p.campo_gradi = campi[ingrandimento];
        p.passo_gradi = passi[ingrandimento];
        p.passo_fine_gradi = fini[ingrandimento];
        p.reticolo = Reticolo::sovietico;
    }
    return p;
}

}  // namespace

int main(int argc, char** argv) {
    if (argc < 4) {
        std::fprintf(stderr, "uso: anteprima_periscopio <dati> <cosa> <uscita.bmp>\n");
        return 1;
    }
    const std::string dati = argv[1], cosa = argv[2], uscita = argv[3];
    const auto ris = carica_risorse(dati);
    Periscopio per(ris.font, ris.classi, ris.sprite);
    per.usa_modelli(ris.modelli, rampa_grigi(ris.giorno));
    per.usa_palette(ris.giorno);

    Mondo m;
    m.aggiungi(nave(1, "perry", 2.0, 3000.0, 300.0));
    m.aggiungi(nave(2, "krivak2", -9.0, 9000.0, 40.0));
    m.aggiungi(nave(3, "sh60b", 8.0, 420.0, 250.0, -150.0));   // in volo a 150 m: 19,6 gradi sopra l'orizzonte
    Nave occhio;
    occhio.id = -1;
    occhio.pos = {0, 0};

    VistaPeriscopio v;
    v.rilevamento = 2.0;
    v.quota_occhio_m = 1.5;
    v.tempo_s = 12.0;
    v.altezza_onde_m = 1.0;
    std::string titolo;
    std::optional<CampoGhiaccio> campo;   // le scene del ghiaccio
    double quota = 0, ghiaccio = 0;
    bool verticale = false;

    if (cosa == "classico") {
        titolo = "CLASSICO: VISTA RETTANGOLARE 1,5X";
    } else if (cosa == "type2_15") {
        v.oculare = modello("type2", 0);
        titolo = "TYPE 2 (ATTACCO) 1,5X - CAMPO 32 GRADI";
    } else if (cosa == "type2_6") {
        v.oculare = modello("type2", 1);
        titolo = "TYPE 2 (ATTACCO) 6X - CAMPO 8 GRADI";
    } else if (cosa == "type18_12") {
        v.oculare = modello("type18", 2);
        titolo = "TYPE 18 (RICERCA) 12X - CAMPO 4 GRADI";
    } else if (cosa == "elevazione") {
        v.oculare = modello("type2", 0);
        v.oculare.elevazione_gradi = 14.0;
        v.rilevamento = 8.0;
        titolo = "TYPE 2 1,5X ALZATO A 14 GRADI (FERMO): SH-60B A 150 M";
    } else if (cosa == "stadimetro") {
        v.oculare = modello("type2", 1);
        v.oculare.sdoppiamento_gradi = 0.55;
        titolo = "TYPE 2 6X: STADIMETRO, IMMAGINE FANTASMA";
    } else if (cosa == "filtro_giallo") {
        v.oculare = modello("type2", 0);
        v.oculare.filtro = Filtro::giallo;
        titolo = "TYPE 2 1,5X CON IL FILTRO GIALLO";
    } else if (cosa == "type18_notte") {
        v.oculare = modello("type18", 1);
        v.oculare.modo = ModoOculare::bassa_luminosita;
        v.luce = 0.04;
        titolo = "TYPE 18 6X DI NOTTE: BASSA LUMINOSITA'";
    } else if (cosa == "type18_tv") {
        v.oculare = modello("type18", 1);
        v.oculare.modo = ModoOculare::tv;
        titolo = "TYPE 18 6X: TV AL MONITOR DELLA CENTRALE";
    } else if (cosa == "alfa_intensificatore") {
        v.oculare = modello("signal", 0);
        v.oculare.modo = ModoOculare::intensificatore;
        v.luce = 0.04;
        titolo = "ALFA, SIGNAL 1,5X DI NOTTE: INTENSIFICATORE";
    } else if (cosa == "alfa_tv1") {
        v.oculare = modello("signal", 1);
        v.oculare.modo = ModoOculare::tv;
        titolo = "ALFA, SIGNAL 6X: TV-1 AL PULTO DEL COMANDANTE";
    } else if (cosa == "alfa_verticale" || cosa == "alfa_verticale_ghiaccio") {
        v.oculare.attivo = true;
        v.oculare.modo = ModoOculare::tv;
        verticale = true;
        quota = cosa == "alfa_verticale" ? 25.0 : 40.0;
        ghiaccio = cosa == "alfa_verticale" ? 0.0 : 0.8;
        titolo = cosa == "alfa_verticale" ? "ALFA: TELECAMERA VERTICALE DELLA VELA A 25 M"
                                          : "ALFA: TELECAMERA VERTICALE SOTTO IL PACK A 40 M";
    } else if (cosa == "trafalgar_termico") {
        v.oculare = modello("ch84", 0);
        v.oculare.modo = ModoOculare::termico;
        v.luce = 0.02;
        titolo = "TRAFALGAR, CH84 1,5X DI NOTTE: TERMICO";
    } else if (cosa == "tremolio") {
        v.oculare = modello("type2", 1);
        v.oculare.tremolio = 1.0;
        v.tempo_s = 12.13;
        titolo = "TYPE 2 6X A 10 NODI: TREMOLIO PIENO";
    } else if (cosa == "mare_2" || cosa == "mare_5" || cosa == "mare_7") {
        // mare 2 (Hs 0,5 m), 5 (3 m, vento 22 kn), 7 (7 m, vento 45 kn): onde da 300 gradi
        v.oculare = modello("type2", 0);
        v.oculare.direzione_mare_gradi = 300.0;
        v.altezza_onde_m = cosa == "mare_2" ? 0.5 : cosa == "mare_5" ? 3.0 : 7.0;
        v.oculare.vento_kn = cosa == "mare_2" ? 8.0 : cosa == "mare_5" ? 22.0 : 45.0;
        v.visibilita_m = cosa == "mare_7" ? 6000.0 : 30000.0;
        v.quota_occhio_m = 1.5;
        titolo = cosa == "mare_2" ? "MARE 2 (HS 0,5 M, VENTO 8 KN)" : cosa == "mare_5" ? "MARE 5 (HS 3 M, VENTO 22 KN)"
                                                                   : "MARE 7 (HS 7 M, VENTO 45 KN, VISIBILITA' 6 KM)";
    } else if (cosa == "pioggia") {
        v.oculare = modello("type2", 1);
        v.oculare.altezza_testa_m = 1.5;
        v.oculare.pioggia = 1.0;
        v.luce = 0.6;
        titolo = "TYPE 2 6X SOTTO LA PIOGGIA: GOCCE SULLA FINESTRA";
    } else if (cosa == "beaufort" || cosa == "creste") {
        // sei riquadri: per forza del vento (vento e mare formato, la Perry nel campo) o sei momenti di mare 6 a 6x
        const bool bf = cosa == "beaufort";
        const double venti[6] = {9, 19, 30, 37, 44, 52}, onde_bf[6] = {0.6, 2.0, 4.0, 5.5, 7.0, 9.0};
        const char* nomi_bf[6] = {"FORZA 3", "FORZA 5", "FORZA 7: STRISCE", "FORZA 8: SPRUZZAGLIA", "FORZA 9", "FORZA 10: MARE BIANCO"};
        Palette pal = ris.giorno;
        Tela t;
        t.riempi(colori::NERO);
        for (int i = 0; i < 6; ++i) {
            VistaPeriscopio w = v;
            w.oculare = modello("type2", bf ? 0 : 1);
            w.oculare.direzione_mare_gradi = 300.0;
            w.oculare.vento_kn = bf ? venti[i] : 27.0;
            w.altezza_onde_m = bf ? onde_bf[i] : 4.0;
            w.oculare.altezza_testa_m = bf ? 3.0 : 1.0;   // Beaufort: il periscopio di ricerca alto, la finestra fuori
            w.visibilita_m = bf && i >= 4 ? 8000.0 : 25000.0;
            w.tempo_s = bf ? 20.0 : 20.0 + i * 1.7;
            Tela grande;
            grande.riempi(colori::NERO);
            per.disegna(grande, m, occhio, w);
            const int cx = (i % 3) * 213, cy = (i / 3) * 200;
            for (int y = 0; y < 180; ++y)
                for (int x = 0; x < 212; ++x) t.pixel(cx + x, cy + y, grande.at(140 + x * 360 / 212, 30 + y * 300 / 180 > 359 ? 359 : 30 + y * 300 / 180));
            char riga[64];
            if (bf) std::snprintf(riga, sizeof riga, "%s (%.0f KN, HS %.1f M)", nomi_bf[i], venti[i], onde_bf[i]);
            else std::snprintf(riga, sizeof riga, "MARE 6, TESTA A 1 M, T %.1f S", w.tempo_s);
            ris.font.scrivi(t, cx + 4, cy + 184, riga, colori::BIANCO, 1);
        }
        t.salva_bmp(uscita, pal);
        return 0;
    } else if (cosa == "onde") {
        // sei momenti col mare 5 (Hs 3 m) e la finestra a 1 m: libera, l'acqua sale, sommersa, il velo che scola,
        // le gocce, gli spruzzi
        const double hs = 3.0, testa = 1.0;
        auto cop = [&](double tt) { return std::clamp((elevazione_onda_m(hs, tt) - testa + oculare::FINESTRA_M) / (2 * oculare::FINESTRA_M), 0.0, 1.0); };
        double momenti[6] = {-1, -1, -1, -1, -1, -1};
        for (double tt = 5.0; tt < 400.0 && momenti[5] < 0; tt += 0.01) {
            const double c = cop(tt);
            if (momenti[0] < 0 && elevazione_onda_m(hs, tt) < testa - 1.5) momenti[0] = tt;
            if (momenti[1] < 0 && c > 0.4 && c < 0.6 && cop(tt + 0.05) > c) momenti[1] = tt;
            if (momenti[1] > 0 && momenti[2] < 0 && c >= 1.0) momenti[2] = tt;
            if (momenti[2] > 0 && momenti[3] < 0 && c <= 0.0) momenti[3] = tt + 0.25, momenti[4] = tt + 2.0;
            if (momenti[3] > 0 && tt > momenti[4] + 3 && momenti[5] < 0 && c <= 0.0 &&
                elevazione_onda_m(hs, tt) > testa - oculare::FINESTRA_M - 0.3 && cop(tt - 2.0) <= 0.0 && cop(tt - 4.0) <= 0.0 && cop(tt - 6.0) <= 0.0 && cop(tt - 8.0) <= 0.0)
                momenti[5] = tt;
        }
        const char* nomi[6] = {"LIBERA", "L'ACQUA SALE", "SOMMERSA", "IL VELO SCOLA", "GOCCE DOPO 2 S", "SPRUZZI"};
        Palette pal = ris.giorno;
        Tela t;
        t.riempi(colori::NERO);
        for (int i = 0; i < 6; ++i) {
            VistaPeriscopio w = v;
            w.oculare = modello("type2", 1);
            w.oculare.altezza_testa_m = testa;
            w.altezza_onde_m = hs;
            w.tempo_s = momenti[i];
            Tela grande;
            grande.riempi(colori::NERO);
            per.disegna(grande, m, occhio, w);
            const int cx = (i % 3) * 213, cy = (i / 3) * 200;
            for (int y = 0; y < 180; ++y)
                for (int x = 0; x < 212; ++x) t.pixel(cx + x, cy + y, grande.at(140 + x * 360 / 212, 30 + y * 300 / 180 > 359 ? 359 : 30 + y * 300 / 180));
            char riga[64];
            std::snprintf(riga, sizeof riga, "%s (T %.1f S)", nomi[i], momenti[i]);
            ris.font.scrivi(t, cx + 4, cy + 184, riga, colori::BIANCO, 1);
        }
        t.salva_bmp(uscita, pal);
        return 0;
    } else if (cosa == "artiglieria" || cosa == "artiglieria_notte" || cosa == "artiglieria_vetro") {
        // sei riquadri, il Type 2 a 6x verso la Perry (che spara): gli eventi della missione e gli effetti fino all'istante
        const bool notte = cosa == "artiglieria_notte";
        const auto punto = [](double ril, double d) { return Vec2{d * std::sin(radianti(ril)), d * std::cos(radianti(ril))}; };
        const auto evento = [&](TipoEventoArtiglieria tipo, Vec2 pos, double t, double calibro, double carica) {
            EventoArtiglieria e;
            e.tipo = tipo; e.pos = pos; e.tempo_s = t; e.calibro_mm = calibro; e.carica_kg = carica; e.tiratore = 1; e.bersaglio = -1;
            return e;
        };
        struct Riquadro {
            const char* nome;
            std::vector<EventoArtiglieria> eventi;
            double istante;
            double spruzzi = 0, incrinato = 0, spento = 0;
        };
        std::vector<Riquadro> r;
        using T = TipoEventoArtiglieria;
        if (cosa == "artiglieria") {
            r.push_back({"5 POLLICI HE A 800 M, 2 S", {evento(T::scoppio_superficie, punto(1.5, 800), 0, 127, 3.5)}, 2.0});
            r.push_back({"76 MM, TRE COLPI A 1,2 KM", {evento(T::scoppio_superficie, punto(0.5, 1200), 0, 76, 0.75), evento(T::scoppio_superficie, punto(2.2, 1150), 0.75, 76, 0.75),
                                                     evento(T::scoppio_superficie, punto(3.0, 1300), 1.5, 76, 0.75)}, 2.6});
            r.push_back({"16 POLLICI (IOWA) A 2,5 KM, 4 S", {evento(T::scoppio_superficie, punto(2.0, 2500), 0, 406, 70)}, 4.0});
            auto vt = evento(T::scoppio_aria, punto(2.0, 700), 0, 127, 3.5);
            vt.quota_m = 12;
            r.push_back({"VT: SCOPPIO A 12 M, 1,5 S", {vt}, 1.5});
            r.push_back({"RIMBALZO A 500 M, 1,2 S", {evento(T::rimbalzo, punto(2.0, 500), 0, 127, 0)}, 1.2});
            auto raffica = evento(T::raffica, punto(2.0, 600), 0, 30, 0.05);
            raffica.colpi = 60;
            raffica.intervallo_s = 0.013;
            r.push_back({"AK-630 A 600 M, 0,6 S", {raffica}, 0.6});
        } else if (notte) {
            auto partenza = evento(T::partenza, Vec2{3000.0 * std::sin(radianti(2.0)), 3000.0 * std::cos(radianti(2.0))}, 0, 76, 0);
            r.push_back({"VAMPA DI BOCCA SULLA PERRY", {partenza}, 0.06});
            r.push_back({"5 POLLICI: IL LAMPO ALLA BASE", {evento(T::scoppio_superficie, punto(1.5, 900), 0, 127, 3.5)}, 0.05});
            auto raffica = evento(T::raffica, punto(2.0, 700), 0, 30, 0.05);
            raffica.colpi = 80;
            raffica.intervallo_s = 0.013;
            r.push_back({"AK-630: I TRACCIANTI", {raffica}, 0.5});
            r.push_back({"5 POLLICI A 900 M, 3 S", {evento(T::scoppio_superficie, punto(1.5, 900), 0, 127, 3.5)}, 3.0});
            r.push_back({"VAMPA E FUMO, 2 S", {partenza}, 2.0});
            r.push_back({"76 MM A 1,2 KM, 1,5 S", {evento(T::scoppio_superficie, punto(2.5, 1200), 0, 76, 0.75)}, 1.5});
        } else {
            r.push_back({"ACQUA DELLA COLONNA: 0,4", {}, 1.0, 0.4});
            r.push_back({"ACQUA DELLA COLONNA: 0,9", {}, 1.0, 0.9});
            r.push_back({"VETRO INCRINATO: 0,4", {}, 1.0, 0, 0.4});
            r.push_back({"VETRO INCRINATO: 1", {}, 1.0, 0, 1.0});
            r.push_back({"IMMAGINE CHE SI SPEGNE: 0,5", {}, 1.0, 0, 0, 0.5});
            r.push_back({"IMMAGINE SPENTA", {}, 1.0, 0, 0, 1.0});
        }
        Palette pal = ris.giorno.illuminata(notte ? 0.05 : v.luce, INDICI_VISTA);
        Tela t;
        t.riempi(colori::NERO);
        for (std::size_t i = 0; i < r.size(); ++i) {
            Effetti e(7 + i);
            e.imposta_vento(250.0, 7.0);
            for (const auto& ev : r[i].eventi) e.artiglieria(ev, 0.0);
            for (double tt = 0; tt < r[i].istante; tt += 0.02) e.avanza(0.02, m);
            VistaPeriscopio w = v;
            w.oculare = modello("type2", 1);
            w.oculare.spruzzi_proietti = r[i].spruzzi;
            w.oculare.incrinato = r[i].incrinato;
            w.oculare.spento = r[i].spento;
            w.oculare.seme_colpo = 17;
            if (notte) w.luce = 0.05;
            Tela grande;
            grande.riempi(colori::NERO);
            per.disegna(grande, m, occhio, w, &e);
            const int cx = static_cast<int>(i % 3) * 213, cy = static_cast<int>(i / 3) * 200;
            for (int y = 0; y < 180; ++y)
                for (int x = 0; x < 212; ++x) t.pixel(cx + x, cy + y, grande.at(140 + x * 360 / 212, 30 + y * 300 / 180 > 359 ? 359 : 30 + y * 300 / 180));
            ris.font.scrivi(t, cx + 4, cy + 184, r[i].nome, colori::BIANCO, 1);
        }
        t.salva_bmp(uscita, pal);
        return 0;
    } else if (cosa == "artiglieria_illuminante") {
        // di notte, un illuminante aperto a 500 m sopra un punto a 1,5 km, dopo 10 s: la luce sul mare
        Effetti e(3);
        EventoArtiglieria il;
        il.tipo = TipoEventoArtiglieria::illuminante;
        il.pos = {1500.0 * std::sin(radianti(4.0)), 1500.0 * std::cos(radianti(4.0))};
        il.quota_m = 500.0;
        il.calibro_mm = 127.0;
        e.artiglieria(il, 0.0);
        for (double tt = 0; tt < 10.0; tt += 0.05) e.avanza(0.05, m);
        v.oculare = modello("type2", 0);
        v.luce = std::min(1.0, 0.05 + e.luce_illuminanti(Vec2{0, 1500}));
        v.rilevamento = 3.0;
        v.oculare.elevazione_gradi = 6.0;
        Palette pal = ris.giorno.illuminata(v.luce, INDICI_VISTA);
        Tela t;
        t.riempi(colori::NERO);
        per.disegna(t, m, occhio, v, &e);
        char riga[96];
        std::snprintf(riga, sizeof riga, "ILLUMINANTE A 500 M SU 1,5 KM, DOPO 10 S: LUCE DELLA VISTA %.2f", v.luce);
        ris.font.scrivi(t, 16, ALTEZZA_VISTA + 20, riga, colori::BIANCO, 1);
        t.salva_bmp(uscita, pal);
        return 0;
    } else if (cosa == "sott_acqua") {
        v.oculare = modello("type2", 0);
        v.oculare.sott_acqua = true;
        titolo = "TYPE 2 IN ALZATA: LA TESTA E' ANCORA SOTT'ACQUA";
    } else if (cosa == "luci_notte" || cosa == "luci_foschia" || cosa == "luci_6x" || cosa == "luci_calma" || cosa == "luci_mosso" ||
               cosa == "gruppo_mare5" || cosa == "gruppo_mare5_6x") {
        // la scena delle luci: il mercantile che viene verso di noi, il peschereccio a strascico, il cargo alla fonda,
        // la Perry che passa, il Los Angeles emerso col faro ambra; lo stato e' quello predefinito (pace)
        m = Mondo();
        constexpr double NODO = 1852.0 / 3600.0;
        struct Voce { int id; const char* classe; double ril, dist, rotta, nodi; };
        const std::vector<Voce> scena = cosa.rfind("gruppo", 0) == 0
            // la LIB-S17: il gruppo portaerei di notte col mare 5, la portaerei a 15 nodi in accostata verso di noi
            ? std::vector<Voce>{{20, "nimitz", -3.0, 5500.0, 150.0, 15.0}, {21, "knox", 8.0, 3500.0, 120.0, 15.0},
                                {22, "spruance", -12.0, 7500.0, 140.0, 15.0}}
            : std::vector<Voce>{{10, "mercantile", -4.0, 6500.0, 170.0, 12.0}, {11, "peschereccio", 5.0, 3200.0, 95.0, 3.0},
                                {12, "cargo", 1.0, 11000.0, 60.0, 0.0},        {13, "perry", 11.0, 7500.0, 280.0, 18.0},
                                {14, "los_angeles", -12.0, 2200.0, 35.0, 10.0}};
        static const auto configurazioni = luci::carica(dati + "/luci.toml");
        for (const auto& x : scena) {
            Nave n = nave(x.id, x.classe, x.ril, x.dist, x.rotta);
            n.velocita = x.nodi * NODO;
            m.aggiungi(n);
            if (const auto c = configurazioni.find(x.classe); c != configurazioni.end())
                v.luci.stati[x.id] = luci::stato_predefinito(c->second.tipo, n.velocita, false, false);
        }
        v.luci.configurazioni = &configurazioni;
        v.luce = 0.04;
        v.tempo_s = 0.2;   // il faro ambra nel lampo
        v.rilevamento = 0.0;
        v.oculare = modello("type2", cosa == "luci_6x" || cosa == "gruppo_mare5_6x" ? 1 : 0);
        if (cosa.rfind("gruppo", 0) == 0) {   // mare 5 col meteo: le navi beccheggiano, la testa sale e scende con l'onda
            v.altezza_onde_m = std::getenv("ANTEPRIMA_ONDE") ? std::atof(std::getenv("ANTEPRIMA_ONDE")) : 3.0;
            v.oculare.vento_kn = 22.0;
            v.oculare.altezza_testa_m = std::getenv("ANTEPRIMA_TESTA") ? std::atof(std::getenv("ANTEPRIMA_TESTA")) : 1.5;
            v.meteo.attivo = true;
            v.meteo.nubi = 0.9;
            v.meteo.plafond_m = 600.0;
            v.meteo.vento_m_s = 11.0;
            v.meteo.vento_da_gradi = 200.0;
            v.meteo.mare_da_gradi = 200.0;
            v.visibilita_m = 15000.0;
            v.tempo_s = std::getenv("ANTEPRIMA_TEMPO") ? std::atof(std::getenv("ANTEPRIMA_TEMPO")) : 37.2;
        }
        if (cosa == "luci_6x") v.rilevamento = -4.0;
        if (cosa == "luci_foschia") v.visibilita_m = 3000.0;
        if (cosa == "luci_calma") v.altezza_onde_m = 0.1;   // il riflesso sul mare (vento ~2 m/s): la scia stretta e piena
        if (cosa == "luci_mosso") v.altezza_onde_m = 2.5;   // vento ~10 m/s: la scia lunga e rada
        titolo = cosa == "luci_notte"     ? "TYPE 2 1,5X DI NOTTE: I FANALI (MERCANTILE, PESCA, FONDA, PERRY, 688 COL FARO AMBRA)"
                 : cosa == "luci_foschia" ? "LE STESSE LUCI CON LA FOSCHIA A 3 KM: SI SPENGONO LE LONTANE, ALONI SULLE VICINE"
                 : cosa == "gruppo_mare5" ? "S17: IL GRUPPO PORTAEREI DI NOTTE COL MARE 5 (NIMITZ, KNOX, SPRUANCE A 15 NODI)"
                 : cosa == "gruppo_mare5_6x" ? "S17 A 6X: LA PORTAEREI COI FANALI FRA LE ONDE"
                 : cosa == "luci_calma"   ? "MARE CALMO (VENTO 2 M/S): IL RIFLESSO DEI FANALI, UNA SCIA STRETTA SOTTO OGNI LUCE"
                 : cosa == "luci_mosso"   ? "MARE MOSSO (VENTO 10 M/S): LA SCIA SI ALLUNGA E SI SPARGE IN SCINTILLE"
                                          : "TYPE 2 6X SUL MERCANTILE: I DUE BIANCHI D'ALBERO, IL ROSSO E IL VERDE";
    } else if (cosa.rfind("ghiaccio", 0) == 0) {
        // la banchisa (oculare::banchisa): dentro una polynya di 100 m nel pack pieno (ghiaccio_polynya a 1,5x,
        // ghiaccio_6x, ghiaccio_notte, ghiaccio_nebbia; ghiaccio_emerso con l'occhio a 12 m, il battello emerso nella
        // polynya), o il margine a 1,2 km visto dal mare aperto con un Udaloy davanti (ghiaccio_margine); il campo del
        // ghiaccio e' quello della missione, col seme fisso. Sotto il cielo coperto (plafond 300 m): il margine a 4 km
        // a 1,5x (ghiaccio_nubi_margine), la base delle nubi chiara sopra il pack sotto i 4 gradi; dalla polynya un canale
        // a 1,5-2,5 km (ghiaccio_nubi_canale), il cielo d'acqua scuro fra 7 e 11 gradi
        const bool margine = cosa == "ghiaccio_margine" || cosa == "ghiaccio_nubi_margine";
        const bool canale = cosa == "ghiaccio_nubi_canale", nubi = canale || cosa == "ghiaccio_nubi_margine";
        const double y_margine = nubi ? 4000.0 : 1200.0;
        campo.emplace(7, ParametriGhiaccio{}, [margine, canale, y_margine](Vec2 q) {
            if (margine) return q.y > y_margine ? 1.0 : 0.0;
            if (canale && std::abs(q.y - 2000.0) < 500.0) return 0.0;
            return std::hypot(q.x, q.y - 40.0) < 100.0 ? 0.0 : 1.0;
        });
        m = Mondo();
        if (cosa == "ghiaccio_margine") m.aggiungi(nave(1, "udaloy", 2.0, 900.0, 280.0));
        v.rilevamento = 0.0;
        v.altezza_onde_m = margine ? 1.0 : 0.3;
        v.oculare = modello("type2", cosa == "ghiaccio_6x" || cosa == "ghiaccio_margine" ? 1 : 0);   // il margine a 1,5x e' sotto il pixel
        if (nubi) {
            v.meteo.attivo = true;
            v.meteo.nubi = 1.0;
            v.meteo.plafond_m = 300.0;
        }
        v.ghiaccio.attivo = true;
        v.ghiaccio.copertura = margine ? 0.0 : 1.0;
        v.ghiaccio.campione = [&campo](Vec2 q) {
            const PuntoGhiaccio g = campo->in(q, 0.0);
            oculare::CampioneGhiaccio c;
            c.ghiaccio = g.ghiaccio;
            c.nilas = g.nilas;
            c.spessore_m = g.spessore_m;
            c.cresta_m = g.chiglia ? g.pescaggio_m / 4.5 : 0.0;
            return c;
        };
        if (cosa == "ghiaccio_notte") v.luce = 0.15;
        if (cosa == "ghiaccio_emerso") v.quota_occhio_m = 12.0;
        if (cosa == "ghiaccio_nebbia") v.visibilita_m = 1200.0;
        titolo = cosa == "ghiaccio_polynya"   ? "TYPE 2 1,5X IN UNA POLYNYA DEL PACK: LASTRE, CRESTE DI PRESSIONE, CANALI"
                 : cosa == "ghiaccio_6x"      ? "TYPE 2 6X: LE CRESTE DI PRESSIONE SUL BORDO DELLA POLYNYA"
                 : cosa == "ghiaccio_notte"   ? "LA STESSA POLYNYA DI NOTTE (LUCE 0,15)"
                 : cosa == "ghiaccio_nebbia"  ? "LA STESSA POLYNYA NELLA NEBBIA (1,2 KM)"
                 : cosa == "ghiaccio_margine" ? "6X: IL MARGINE DEL PACK A 1,2 KM DAL MARE APERTO, UN UDALOY DAVANTI, IL RIFLESSO NEL CIELO"
                 : cosa == "ghiaccio_emerso"  ? "EMERSI NELLA POLYNYA: L'OCCHIO A 12 M"
                 : cosa == "ghiaccio_nubi_margine" ? "1,5X, CIELO COPERTO A 300 M, IL PACK A 4 KM: IL SUO RIFLESSO SULLA BASE DELLE NUBI"
                 : cosa == "ghiaccio_nubi_canale"  ? "1,5X DALLA POLYNYA, CIELO COPERTO: IL CIELO D'ACQUA SOPRA UN CANALE A 1,5-2,5 KM"
                                              : "GHIACCIO";
    } else if (cosa.rfind("costa", 0) == 0) {
        // la terraferma (oculare::costa): un fiordo largo 800 m con le pareti fino a 400 m, che si apre a 3 km
        // (costa_fiordo, costa_notte, costa_fiordo_6x); un arcipelago basso con una Koni dietro un'isola
        // (costa_arcipelago); le montagne lontane a 30 km oltre il mare (costa_lontana). Quote sintetiche
        m = Mondo();
        const bool arcipelago = cosa == "costa_arcipelago", lontana = cosa == "costa_lontana";
        if (arcipelago) m.aggiungi(nave(1, "koni", 6.0, 2600.0, 270.0));
        if (!arcipelago && !lontana) m.aggiungi(nave(1, "cargo", 1.0, 2400.0, 90.0));
        v.costa.attiva = true;
        v.costa.cella_m = 200.0;
        v.costa.quota = [arcipelago, lontana](Vec2 q) {
            auto collina = [](Vec2 q, double cx, double cy, double r, double h) {
                const double d2 = ((q.x - cx) * (q.x - cx) + (q.y - cy) * (q.y - cy)) / (r * r);
                return h * std::exp(-d2);
            };
            if (lontana) return q.y > 30000.0 ? 600.0 * (1.0 - std::exp(-(q.y - 30000.0) / 4000.0)) * (0.7 + 0.3 * std::sin(q.x / 2500.0)) : -50.0;
            if (arcipelago) {
                const double z = collina(q, 300.0, 1800.0, 450.0, 35.0) + collina(q, -900.0, 2600.0, 600.0, 25.0) +
                                 collina(q, 1500.0, 3800.0, 700.0, 45.0) + collina(q, -2500.0, 1200.0, 500.0, 20.0);
                return z - 8.0;
            }
            // il fiordo: verso nord, le pareti a 400 m dal centro, l'imboccatura a 3 km
            const double lato = std::abs(q.x) - 400.0 - 0.04 * std::max(0.0, q.y);
            if (q.y < -300.0 || q.y > 3000.0) return lato > 0.0 ? 60.0 * (1.0 - std::exp(-lato / 300.0)) - 5.0 : -40.0;
            return lato > 0.0 ? 400.0 * (1.0 - std::exp(-lato / 90.0)) + 30.0 * std::sin(q.y / 400.0) : -80.0;
        };
        v.rilevamento = 0.0;
        v.altezza_onde_m = 0.3;
        v.oculare = modello("type2", cosa == "costa_fiordo_6x" ? 1 : 0);
        if (cosa == "costa_notte") v.luce = 0.12;
        if (arcipelago) v.visibilita_m = 12000.0;
        titolo = cosa == "costa_fiordo"       ? "LA COSTA: IN UN FIORDO LARGO 800 M, LE PARETI FINO A 400 M, UN CARGO A 2,4 KM"
                 : cosa == "costa_fiordo_6x"  ? "6X NEL FIORDO: IL CARGO SOTTO LE PARETI"
                 : cosa == "costa_notte"      ? "IL FIORDO DI NOTTE (LUCE 0,12)"
                 : cosa == "costa_arcipelago" ? "UN ARCIPELAGO BASSO NELLA FOSCHIA: UNA KONI DIETRO UN'ISOLA"
                 : cosa == "costa_lontana"    ? "LE MONTAGNE A 30 KM OLTRE IL MARE"
                                              : "COSTA";
    } else if (cosa.rfind("classe:", 0) == 0) {   // foglio di controllo dei modelli: una classe sola a 1,5 km, quasi
        const std::string c = cosa.substr(7);      // al traverso (angolo sulla prua 70 gradi), Type 2 a 6x
        m = Mondo();
        m.aggiungi(nave(1, c.c_str(), 0.0, 1500.0, 250.0));
        v.rilevamento = 0.0;
        v.oculare = modello("type2", 1);
        titolo = "TYPE 2 6X, 1,5 KM: " + c;
    } else {
        std::fprintf(stderr, "cosa sconosciuta: %s\n", cosa.c_str());
        return 1;
    }

    Palette pal = ris.giorno.illuminata(v.luce, INDICI_VISTA);
    oculare::palette(pal, v.oculare);
    Tela t;
    t.riempi(colori::NERO);
    if (verticale) {
        t.imposta_ritaglio({0, 0, LARGHEZZA_TELA, ALTEZZA_VISTA});
        oculare::telecamera_verticale(t, oculare::area(v.oculare), quota, 1.0, ghiaccio, v.tempo_s);
        oculare::maschera(t, v.oculare);
        t.togli_ritaglio();
    } else {
        const auto t0 = std::chrono::steady_clock::now();
        per.disegna(t, m, occhio, v);
        if (std::getenv("ANTEPRIMA_TEMPI"))   // il costo del disegno della vista, per le prove di velocita'
            std::fprintf(stderr, "disegno: %.1f ms\n", std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count());
    }
    ris.font.scrivi(t, 16, ALTEZZA_VISTA + 20, titolo, colori::BIANCO, 1);
    t.salva_bmp(uscita, pal);
    return 0;
}
