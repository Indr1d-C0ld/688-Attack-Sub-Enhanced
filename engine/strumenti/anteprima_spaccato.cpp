// Anteprima del quadro danni dell'Enhanced con lo spaccato (grafica/spaccato.hpp): immagini 640x360 senza finestra
// (extracted/navi/_verifica/spaccato/), con l'impaginazione della vista F3.
// Uso: anteprima_spaccato <dati> <classe> <scenario: integro|danneggiato|grave> <uscita.bmp> [secondi] [carico]
// (carico: un tipo di missione di carico.toml, per le rastrelliere col carico standard: antisom, minamento, ...)

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

#include "grafica/font.hpp"
#include "grafica/modello.hpp"
#include "grafica/palette.hpp"
#include "grafica/spaccato.hpp"
#include "grafica/tela.hpp"
#include "nucleo/battello.hpp"
#include "nucleo/carico.hpp"
#include "nucleo/scafo.hpp"

using namespace m688;

int main(int argc, char** argv) {
    if (argc < 5) {
        std::fprintf(stderr, "uso: %s <dati> <classe> <integro|danneggiato|grave> <uscita.bmp> [secondi] [carico]\n", argv[0]);
        return 2;
    }
    const std::string dati = argv[1], classe = argv[2], scenario = argv[3], uscita = argv[4];
    const double secondi = argc > 5 ? std::stod(argv[5]) : 0.0;
    const auto modello = Modello::carica(dati + "/modelli/" + classe + ".mdl");
    const auto scafo = DatiScafo::carica(dati + "/scafi/" + classe + ".toml");
    auto pal = Palette::carica(dati + "/palette/giorno.pal");
    const auto font = Font::carica(dati + "/font/originale.fnt");
    Spaccato::imposta_palette(pal);
    Spaccato sp(classe, modello, scafo);

    BattelloDanni b(scafo, 7);
    const double quota = 150.0;
    double assetto = 0.0;
    const int n = static_cast<int>(b.compartimenti().size());
    const int prua = 0, poppa = n - 1, mezzo = n / 2;
    StatoMoto moto{quota, 6.0};
    auto avanza = [&](double t) {
        for (double x = 0; x < t; x += 0.5) b.avanza(0.5, moto);
    };
    // la simulazione da' guasti e rapporti; lo stato dei compartimenti si fissa a mano per mostrare ogni caso
    if (scenario == "danneggiato") {
        b.urto(135, 15, 1.57, b.compartimenti()[static_cast<std::size_t>(poppa)].xc, quota);
        b.incendio(prua, 0.7);
        avanza(secondi > 0 ? secondi : 60);
    } else if (scenario == "grave") {
        b.esplosione_interna(poppa, 120);
        b.incendio(mezzo, 0.9, true);
        avanza(secondi > 0 ? secondi : 90);
        b.esaurisci();
        avanza(4);
    }
    auto comp = b.compartimenti();
    auto falle = b.falle();
    auto paratie = b.paratie();
    auto casse = b.casse();
    bool esaurimento = false;
    auto pieno = [&](int i, double f) { comp[static_cast<std::size_t>(i)].acqua = f * comp[static_cast<std::size_t>(i)].volume; };
    for (auto& c : comp) { c.acqua = 0; c.fuoco = 0; c.fumo = 0; c.ossigeno = 0.21; c.distrutto = false; c.imploso = false; }
    for (auto& p : paratie) p.ceduta = false;
    falle.clear();
    auto& cp = comp[static_cast<std::size_t>(prua)];
    auto& cq = comp[static_cast<std::size_t>(poppa)];
    auto& cm = comp[static_cast<std::size_t>(n - 2)];
    std::vector<SquadraBattello> squadre;
    auto squadra = [&](SquadraBattello::Compito compito, int compartimento, int quante, int falla = -1, int sistema = -1) {
        SquadraBattello l;
        l.compito = compito;
        l.compartimento = compartimento;
        l.numero = quante;
        l.falla = falla;
        l.sistema = sistema;
        squadre.push_back(l);
    };
    using C = SquadraBattello::Compito;
    if (scenario == "danneggiato") {
        squadra(C::tamponare, poppa, 1, 0);
        squadra(C::spegnere, prua, 2);
        squadra(C::riparare, poppa, 1, -1, static_cast<int>(Sistema::motori));
        // siluro a poppa: falla e acqua nelle macchine; incendio a prua con il fumo che scende
        pieno(poppa, 0.32);
        cq.fumo = 0.15;
        cp.fuoco = 0.65;
        cp.fumo = 0.55;
        cp.ossigeno = 0.18;
        falle.push_back({poppa, 0.05});
        assetto = -0.05;
    } else if (scenario == "grave") {
        // macchine sventrate e allagate, paratia ceduta, incendio nel compartimento vicino, casse in esaurimento
        pieno(poppa, 1.0);
        cq.distrutto = true;
        pieno(n - 2, 0.55);
        cm.fumo = 0.9;
        cm.fuoco = 0.3;
        cm.ossigeno = 0.14;
        pieno(prua, 0.06);
        cp.fumo = 0.4;
        for (auto& p : paratie)
            if ((p.a == poppa && p.b == n - 2) || (p.b == poppa && p.a == n - 2)) p.ceduta = true;
        falle.push_back({poppa, 1.2});
        falle.push_back({n - 2, 0.03});
        falle.push_back({prua, 0.01});
        for (auto& k : casse) k.aria_Nm3 = 0.55 * k.volume * (1 + quota / 10.06);
        // le casse alle estremita' squarciate dal cannone: non tengono l'aria
        for (auto* k : {&casse.front(), &casse.back()}) { k->squarciata = true; k->aria_Nm3 = 0.0; }
        squadra(C::spegnere, n - 2, 1);
        squadra(C::tamponare, prua, 1, 2);
        squadra(C::pompare, prua, 0);
        esaurimento = true;
        assetto = 0.14;
    }

    Tela t;
    t.riempi(96);
    const int H = 360;
    // cornice e intestazione
    t.rettangolo_pieno(0, 0, 640, 20, 104 + 1);
    t.linea_orizzontale(0, 639, 20, 112 + 1);
    font.scrivi(t, 8, 3, "QUADRO DANNI", 112 + 3, 1);
    std::string nome = classe == "los_angeles" ? "CLASSE LOS ANGELES (688)" : classe == "alfa" ? "PROGETTO 705 LIRA (ALFA)" : classe;
    for (auto& c : nome) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    font.scrivi(t, 640 - 8 - font.larghezza(nome, 1), 3, nome, 112 + 2, 1);
    char riga[96];
    std::snprintf(riga, sizeof riga, "QUOTA %.0f M  ASSETTO %+.0f", quota, assetto * 57.3);
    font.scrivi(t, 260, 3, riga, 112 + 1, 1);

    const Rettangolo rs{4, 20, 632, 108};
    StatoSpaccato stato = StatoSpaccato::da(b, quota, assetto, b.tempo() + 0.37);
    stato.compartimenti = &comp;
    stato.falle = &falle;
    stato.paratie = &paratie;
    stato.casse = &casse;
    stato.esaurimento = esaurimento;
    stato.squadre = &squadre;
    // rastrelliere col carico standard del tipo di missione, un'arma per posto
    std::vector<SagomaArma> rastrelliera;
    if (argc > 6) {
        const auto cat = carica_carico(dati + "/carico.toml");
        const auto* loc = cat.locale(classe);
        if (loc) {
            const auto& std_ = cat.carichi.at(loc->id).at(argv[6]);
            for (const auto& [id, quante] : std_.rastrelliera)
                for (int k = 0; k < quante; ++k) rastrelliera.push_back(sagoma_da_scheda(*cat.arma(id)));
            while (static_cast<int>(rastrelliera.size()) < loc->rastrelliera) rastrelliera.push_back({});
            stato.rastrelliera = &rastrelliera;
        }
    }
    sp.disegna(t, rs, stato);
    sp.disegna_didascalie(t, font, rs, Rettangolo{rs.x, rs.y + rs.h, rs.w, 34}, stato);

    // dettaglio: il compartimento piu' colpito (o quello di prua)
    int scelto = 0;
    double peggio = -1;
    for (int i = 0; i < n; ++i) {
        const auto& c = comp[static_cast<std::size_t>(i)];
        const double v = c.fuoco + c.fumo + (c.volume > 0 ? c.acqua / c.volume : 0) + (c.distrutto ? 2 : 0);
        if (v > peggio + 1e-9) { peggio = v; scelto = i; }
    }
    double x0 = 0, x1 = 0;
    sp.estensione(scelto, x0, x1);
    const Rettangolo rd{4, 164, 632, 160};
    sp.disegna_dettaglio(t, rd, stato, x0, x1);
    t.linea_orizzontale(rd.x, rd.x + rd.w - 1, rd.y - 1, 112 + 1);
    t.linea_orizzontale(rd.x, rd.x + rd.w - 1, rd.y + rd.h, 112 + 1);
    // nel quadro intero, la parentesi del dettaglio
    const int a0 = sp.x_tela(rs, x0), a1 = sp.x_tela(rs, x1);
    t.linea_orizzontale(a0, a1, rs.y + rs.h - 2, 112 + 3);
    std::snprintf(riga, sizeof riga, "COMPARTIMENTO %d", scelto + 1);
    t.rettangolo_pieno(rd.x, rd.y + rd.h - 13, font.larghezza(riga, 1) + 8, 13, 104 + 1);
    font.scrivi(t, rd.x + 4, rd.y + rd.h - 12, riga, 112 + 2, 1);

    // rapporti dell'equipaggio (i sistemi restano nel pannello sotto la vista)
    const auto& ev = b.eventi();
    const std::size_t k = std::min<std::size_t>(ev.size(), 3);
    for (std::size_t i = 0; i < k; ++i) {
        const auto& e = ev[ev.size() - k + i];
        std::string testo = e.testo;
        for (std::size_t p; (p = testo.find('%')) != std::string::npos;) testo.replace(p, 1, " PER CENTO");
        const int sec = static_cast<int>(e.tempo);
        std::snprintf(riga, sizeof riga, "%02d:%02d  ", sec / 60, sec % 60);
        std::string r = riga + testo;
        while (font.larghezza(r, 1) > 620) r.pop_back();
        font.scrivi(t, 8, 327 + static_cast<int>(i) * 11, r, i + 1 == k ? 136 + 2 : 124 + 6, 1);
    }
    if (ev.empty()) font.scrivi(t, 8, 327, "NESSUN DANNO SEGNALATO", 136 + 3, 1);
    (void)H;

    Tela ritaglio(640, 360);
    for (int y = 0; y < 360; ++y) std::copy(t.riga(y), t.riga(y) + 640, ritaglio.riga(y));
    ritaglio.salva_bmp(uscita, pal);
    return 0;
}
