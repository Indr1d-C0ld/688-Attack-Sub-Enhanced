// Anteprime del meteo nella vista del periscopio dell'Enhanced (docs/15_meteo_e_mare.md, M3). Una scena di prova (una
// Perry a 3 km, una Krivak II a 7 km, un SH-3H in hover col sonar calato a 900 m; a 6x) col clima del teatro e del mese letto
// da meteo.toml (vento, mare, nubi, pioggia o neve, foschia della stagione) e un modo:
//   giorno, notte, temporale (cumulonembi, pioggia forte, un fulmine vicino e un bagliore lontano), nebbia (fumo di mare
//   nel Barents e a Kunashir d'inverno, nebbia d'avvezione altrove);
//   dinamico[:ore] (il generatore del runtime, nucleo/meteo, dopo tante ore: la vista con applica_meteo),
//   carta[:ore] (gli strati meteo su una carta schematica di 190 km col generatore)
// Uso: anteprima_meteo <dati> <teatro> <mese> <modo> <uscita.bmp> [classico]

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <memory>
#include <string>

#include <toml.hpp>

#include "grafica/cielo.hpp"
#include "grafica/effetti.hpp"
#include "grafica/oculare.hpp"
#include "grafica/periscopio.hpp"
#include "grafica/strati_meteo.hpp"
#include "nucleo/meteo.hpp"
#include "risorse.hpp"

using namespace m688;

namespace {

Nave nave(int id, const char* classe, double rilevamento_gradi, double distanza_m, double rotta, double velocita, double quota = 0.0) {
    Nave n;
    n.id = id;
    n.classe = classe;
    n.pos = {distanza_m * std::sin(radianti(rilevamento_gradi)), distanza_m * std::cos(radianti(rilevamento_gradi))};
    n.pos_prec = n.pos;
    n.rotta = n.rotta_prec = rotta;
    n.velocita = velocita;
    n.quota = quota;
    return n;
}

const char* stagione(int mese) {
    if (mese == 12 || mese <= 2) return "inverno";
    if (mese <= 5) return "primavera";
    if (mese <= 8) return "estate";
    return "autunno";
}

}  // namespace

int main(int argc, char** argv) {
    if (argc < 6) {
        std::fprintf(stderr, "uso: anteprima_meteo <dati> <teatro> <mese> <modo> <uscita.bmp> [classico]\n");
        return 1;
    }
    const std::string dati = argv[1], teatro = argv[2], modo = argv[4], uscita = argv[5];
    const int mese = std::stoi(argv[3]);
    const bool classico = argc > 6 && std::string(argv[6]) == "classico";
    const auto ris = carica_risorse(dati);
    const auto meteo = toml::parse_file(dati + "/meteo.toml");
    const auto& t = *meteo["teatri"][teatro].as_table();
    const auto& m = *t["mesi"][std::to_string(mese)].as_table();
    auto num = [](const toml::table& tb, const char* k) { return tb[k].value<double>().value_or(0.0); };
    const int si = mese == 12 || mese <= 2 ? 0 : mese <= 5 ? 1 : mese <= 8 ? 2 : 3;

    VistaPeriscopio v;
    v.rilevamento = 2.0;
    v.quota_occhio_m = 1.5;
    v.tempo_s = 40.0;
    v.altezza_onde_m = num(m, "hs_media_m");
    v.visibilita_m = 1000.0 * (*t["foschia_km"].as_array())[static_cast<std::size_t>(si)].value<double>().value_or(30.0);
    if (!classico) {
        v.oculare.attivo = true;
        v.oculare.campo_gradi = 8.0;   // 6x, sulla Perry
        v.oculare.passo_gradi = 1.0;
        v.oculare.passo_fine_gradi = 0.25;
        v.oculare.altezza_testa_m = 1.5;
        v.oculare.vento_kn = num(m, "vento_media_m_s") / 0.5144;
    }
    MeteoVista& mv = v.meteo;
    mv.attivo = true;
    mv.vento_m_s = num(m, "vento_media_m_s");
    // direzione: il settore piu' frequente della rosa del mese
    const auto& rosa = *m["rosa_pct"].as_array();
    std::size_t s_max = 0;
    for (std::size_t k = 1; k < rosa.size(); ++k)
        if (rosa[k].value<double>().value_or(0) > rosa[s_max].value<double>().value_or(0)) s_max = k;
    mv.vento_da_gradi = mv.mare_da_gradi = 45.0 * static_cast<double>(s_max);
    v.oculare.direzione_mare_gradi = mv.mare_da_gradi;
    mv.tp_s = num(m, "tp_media_s") * 1.25;   // periodo di picco ~ 1,25 il medio (STIMA)
    mv.nubi = num(m, "nubi_pct") / 100.0;
    mv.plafond_m = num(m, "nubi_basse_pct") > 45.0 ? 500.0 : 1200.0;
    const bool freddo = num(m, "t_aria_c") < 1.0;
    if (num(m, "precipitazione_pct") > 30.0) {
        if (freddo) mv.neve_mm_h = 1.0;
        else mv.pioggia_mm_h = std::max(1.0, num(m, "intensita_mm_h") * 3.0);
        v.visibilita_m = std::min(v.visibilita_m, freddo ? 2000.0 : 8000.0);
    }
    v.luce = 1.0;
    std::string titolo = std::string(teatro) + " - mese " + argv[3] + " (" + stagione(mese) + ")";
    if (modo == "notte") {
        v.luce = 0.06;
        titolo += " - NOTTE";
    } else if (modo == "temporale") {
        mv.temporale = 1.0;
        mv.nubi = 1.0;
        mv.plafond_m = 600.0;
        mv.pioggia_mm_h = freddo ? 0.0 : 20.0;
        mv.neve_mm_h = freddo ? 3.0 : 0.0;
        v.visibilita_m = 3000.0;
        v.luce = 0.12;
        mv.lampi.push_back({v.tempo_s - 0.01, v.rilevamento - 1.5, 6000.0, 4, true, 7});
        mv.lampi.push_back({v.tempo_s - 0.012, v.rilevamento + 2.5, 40000.0, 3, false, 9});
        titolo += " - TEMPORALE, FULMINE A 6 KM";
    } else if (modo == "nebbia") {
        if (freddo && (teatro == "barents" || teatro == "kunashir" || teatro == "baltico")) {
            mv.nebbia = NebbiaVista::fumo_di_mare;
            mv.strato_nebbia_m = 25.0;
            v.visibilita_m = 20000.0;
            mv.nubi = 0.1;
            mv.neve_mm_h = 0.0;
            titolo += " - FUMO DI MARE (STRATO 25 M)";
        } else {
            mv.nebbia = NebbiaVista::avvezione;
            v.visibilita_m = 1500.0;
            mv.pioggia_mm_h = 0.0;
            titolo += " - NEBBIA D'AVVEZIONE (1,5 KM)";
        }
    } else {
        titolo += " - GIORNO";
    }

    // col generatore del runtime: la vista dallo stato, o gli strati sulla carta
    const auto due_punti = modo.find(':');
    const std::string base = modo.substr(0, due_punti);
    const double ore = due_punti == std::string::npos ? 30.0 : std::stod(modo.substr(due_punti + 1));
    std::unique_ptr<DatiMeteo> dm;
    std::unique_ptr<Meteo> gen;
    if (base == "dinamico" || base == "carta") {
        dm = std::make_unique<DatiMeteo>(carica_meteo(dati + "/meteo.toml"));
        gen = std::make_unique<Meteo>(*dm, teatro, mese, 6.0, 1989u * 31u + static_cast<unsigned>(mese));
        gen->avanza(ore * 3600.0);
    }
    if (base == "carta") {
        Tela tela;
        tela.riempi(colori::BLU);
        StratiMeteo s;
        constexpr double M_PX = 300.0;
        s.a_tela = [](Vec2 w) { return Vec2{320.0 + w.x / M_PX, 200.0 - w.y / M_PX}; };
        s.a_mondo = [](Vec2 q) { return Vec2{(q.x - 320.0) * M_PX, (200.0 - q.y) * M_PX}; };
        s.area = {0, 0, LARGHEZZA_TELA, 400};
        s.vento = [&](Vec2 w, double& u, double& da) { const StatoMeteo st = gen->in(w); u = st.vento_m_s; da = st.vento_da_gradi; };
        s.stato_mare = [&](Vec2 w) { return gen->in(w).stato_mare(); };
        s.ghiaccio = [&](Vec2 w) { return gen->in(w).ghiaccio; };
        s.nebbia = [&](Vec2 w) { return gen->in(w).visibilita_m < 1000.0; };
        for (const auto& c : gen->celle(gen->tempo())) s.celle.push_back({c.pos, c.raggio_m});
        disegna_strati_meteo(tela, ris.font, s);
        const StatoMeteo st = gen->stato();
        char riga[200];
        std::snprintf(riga, sizeof riga, "%s MESE %d ORA %.0f: %s  VENTO %.0f KN DA %03.0f  MARE %d  VIS %.1f KM  CELLE %zu",
                      teatro.c_str(), mese, ore, nome_regime(st.regime), st.vento_m_s / 0.5144, st.vento_da_gradi, st.stato_mare(),
                      st.visibilita_m / 1000.0, s.celle.size());
        ris.font.scrivi(tela, 8, 410, riga, colori::BIANCO, 1);
        ris.font.scrivi(tela, 8, 424, "CARTA SCHEMATICA DI 190 KM: BARBE DEL VENTO, STATO DEL MARE, CELLE, GHIACCIO, NEBBIA", colori::GRIGIO, 1);
        tela.salva_bmp(uscita, ris.giorno);
        return 0;
    }
    if (base == "dinamico") {
        const StatoMeteo st = gen->stato();
        mv = MeteoVista{};
        applica_meteo(v, st, gen->lampi(gen->tempo() - 60.0, gen->tempo() + 1.0), Vec2{0.0, 0.0});
        v.oculare.altezza_testa_m = 1.5;
        v.luce = 1.0;
        titolo = std::string(teatro) + " - mese " + argv[3] + ", dopo " + std::to_string(static_cast<int>(ore)) + " h: " +
                 nome_regime(st.regime);
    }

    Periscopio per(ris.font, ris.classi, ris.sprite);
    per.usa_modelli(ris.modelli, rampa_grigi(ris.giorno));
    per.usa_palette(ris.giorno);
    Mondo mondo;
    mondo.aggiungi(nave(1, "perry", 2.0, 3000.0, 300.0, 8.0));
    mondo.aggiungi(nave(2, "krivak2", -1.0, 7000.0, 40.0, 7.0));
    mondo.aggiungi(nave(3, "sh3h", 4.0, 900.0, mv.vento_da_gradi, 0.0, -15.0));   // in hover a 15 m, muso al vento
    Effetti e(5);
    AereoAsw a;
    a.id = 3; a.pos = mondo.navi().back().pos; a.quota_m = 15.0; a.elicottero = true; a.hover = true;
    a.rotore_m = 18.9; a.cavo_m = 60.0; a.rotta = mv.vento_da_gradi; a.luci = v.luce < 0.3;
    a.vento_m_s = mv.vento_m_s; a.vento_da_gradi = mv.vento_da_gradi;
    for (double tt = 0; tt < v.tempo_s; tt += 0.1) {
        e.aerei_asw({a});
        e.avanza(0.1, mondo);
    }
    Nave occhio;
    occhio.id = -1;
    v.luce = cielo::luce_sotto_le_nubi(v.luce, mv);
    Palette pal = ris.giorno.illuminata(v.luce, INDICI_VISTA);
    const double lampo = cielo::lampo(mv, v.tempo_s, v.luce);
    if (lampo > 0) pal = pal.schiarita(0.8 * lampo, INDICI_VISTA);
    oculare::palette(pal, v.oculare);
    Tela tela;
    tela.riempi(colori::NERO);
    per.disegna(tela, mondo, occhio, v, &e);
    ris.font.scrivi(tela, 16, ALTEZZA_VISTA + 14, titolo, colori::BIANCO, 1);
    char riga[160];
    std::snprintf(riga, sizeof riga, "VENTO %.0f M/S DA %03.0f  HS %.1f M  NUBI %.0f%%  VIS %.1f KM  PIOGGIA %.1f  NEVE %.1f MM/H",
                  mv.vento_m_s, mv.vento_da_gradi, v.altezza_onde_m, 100 * mv.nubi, v.visibilita_m / 1000.0, mv.pioggia_mm_h, mv.neve_mm_h);
    ris.font.scrivi(tela, 16, ALTEZZA_VISTA + 30, riga, colori::GRIGIO, 1);
    tela.salva_bmp(uscita, pal);
    return 0;
}
