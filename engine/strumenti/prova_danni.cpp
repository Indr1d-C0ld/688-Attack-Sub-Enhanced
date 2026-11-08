// Prova della fisica dei danni (nucleo/danni.hpp) sui casi del dossier 07: stampa assetto, acqua e eventi nel tempo.
//
//   prova_danni CLASSE CASO [SEME] [SECONDI]
//   casi: intatta, siluro_chiglia, siluro_fianco, siluro_prua, siluro_poppa, due_siluri, missile, mina
#include <cstdio>
#include <string>

#include "nucleo/danni.hpp"

using namespace m688;

int main(int argc, char** argv) {
    if (argc < 3) { std::fprintf(stderr, "uso: prova_danni CLASSE CASO [SEME] [SECONDI]\n"); return 1; }
    const std::string classe = argv[1], caso = argv[2];
    const std::uint64_t seme = argc > 3 ? std::stoull(argv[3]) : 1;
    const double durata = argc > 4 ? std::stod(argv[4]) : 7200;
    const DatiScafo d = DatiScafo::carica(std::string(M688_DATI) + "/scafi/" + classe + ".toml");
    DanniNave n(d, seme);
    const double L = d.lunghezza, x0 = n.corpi().front().stazioni.front().x, x1 = n.corpi().front().stazioni.back().x;
    const double xm = (x0 + x1) / 2, keel = n.corpi().front().stazioni[n.corpi().front().stazioni.size() / 2].z_chiglia;
    std::printf("%s: %.0f t (pubblicato %.0f t), L %.1f m\n", classe.c_str(), n.dislocamento_intatto_t(), d.dislocamento_pubblicato_t, L);
    auto siluro = [&](double x, double y, double z, bool contatto, double kg) {
        Esplosione e; e.tipo = Esplosione::Tipo::siluro; e.x = x; e.y = y; e.z = z; e.contatto = contatto; e.tnt_kg = kg; n.colpo(e);
    };
    if (caso == "siluro_chiglia") siluro(xm, 0, keel - 6.0, false, 544);           // Mk 48 ADCAP sotto la chiglia
    else if (caso == "siluro_fianco") siluro(xm - 0.08 * L, -1, keel * 0.7, true, 300);
    else if (caso == "siluro_prua") siluro(x1 - 0.1 * L, -1, keel * 0.7, true, 300);
    else if (caso == "siluro_poppa") siluro(x0 + 0.08 * L, -1, keel * 0.6, true, 300);
    else if (caso == "due_siluri") { siluro(xm + 0.15 * L, -1, keel * 0.7, true, 300); siluro(xm - 0.15 * L, -1, keel * 0.7, true, 300); }
    else if (caso == "missile") { Esplosione e; e.tipo = Esplosione::Tipo::missile; e.x = xm + 0.05 * L; e.y = -1; e.z = 2.5; e.tnt_kg = 165; n.colpo(e); }
    else if (caso == "mina") siluro(xm - 0.1 * L, 0, keel - 3.0, false, 150);
    else if (caso != "intatta") { std::fprintf(stderr, "caso sconosciuto\n"); return 1; }
    std::size_t ev = 0;
    bool affondato_annotato[8] = {};
    for (double t = 0; t <= durata; t += 10) {
        if (t > 0) n.avanza(10);
        for (; ev < n.registro().size(); ++ev) std::printf("    %s\n", n.registro()[ev].c_str());
        const bool stampa = t < 120 ? true : (t < 1800 ? static_cast<int>(t) % 60 == 0 : static_cast<int>(t) % 600 == 0);
        if (stampa || n.affondata()) {
            double fuoco = 0;
            int in_fiamme = 0;
            for (const auto& c : n.corpi()) for (const auto& ce : c.celle) { fuoco = std::max(fuoco, ce.fuoco); in_fiamme += ce.fuoco > 0.05; }
            for (const auto& z : n.zone()) { fuoco = std::max(fuoco, z.fuoco); in_fiamme += z.fuoco > 0.05; }
            std::printf("t=%6.0f  sbandamento %6.1f  assetto %6.1f  immersione %6.2f  acqua %7.0f t  fuoco %.2f in %d  corpi %zu%s%s%s\n", t,
                        n.sbandamento_gradi(), n.appoppamento_gradi(), n.immersione_m(), n.acqua_imbarcata_t(), fuoco, in_fiamme, n.corpi().size(),
                        n.energia() ? "" : "  SENZA ENERGIA", n.condannata() ? "  CONDANNATA" : "", n.affondata() ? "  AFFONDATA" : "");
            for (const auto& c : n.corpi())
                if (n.corpi().size() > 1)
                    std::printf("          troncone x %.0f..%.0f: sb %.1f as %.1f imm %.1f%s\n", c.stazioni.front().x, c.stazioni.back().x,
                                gradi(c.assetto.sbandamento), gradi(c.assetto.appoppamento), c.assetto.immersione, c.affondato ? " AFFONDATO" : "");
        }
        for (std::size_t k = 0; k < n.corpi().size(); ++k)
            if (n.corpi()[k].affondato && !affondato_annotato[k]) {
                affondato_annotato[k] = true;
                std::printf("    troncone %zu (x %.0f..%.0f) affondato a t=%.0f s\n", k, n.corpi()[k].stazioni.front().x,
                            n.corpi()[k].stazioni.back().x, t);
            }
        if (n.affondata()) break;
    }
    return 0;
}
