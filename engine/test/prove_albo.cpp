#include <doctest.h>

#include <cstdlib>
#include <filesystem>

#include "nucleo/albo.hpp"
#include "nucleo/carriera.hpp"
#include "nucleo/fascicolo.hpp"

using namespace m688;
namespace fs = std::filesystem;

namespace {
// L'albo in una cartella di prova: XDG_DATA_HOME vale solo dentro il blocco
struct CartellaProva {
    fs::path dati = fs::temp_directory_path() / "m688_prova_albo";
    CartellaProva() {
        fs::remove_all(dati);
        setenv("XDG_DATA_HOME", dati.c_str(), 1);
    }
    ~CartellaProva() {
        unsetenv("XDG_DATA_HOME");
        fs::remove_all(dati);
    }
};
}  // namespace

TEST_CASE("albo dei comandanti: in carica per marina, caduti consultabili ma non caricabili, id liberi, eliminazione") {
    const CartellaProva c;
    static const RegoleCarriera r = carica_carriera(M688_DATI "/carriera.toml");
    CHECK(leggi_albo().empty());
    CHECK_FALSE(comandante_in_carica(Marina::usn));

    Fascicolo a = Carriera::nuovo(r, Marina::usn, "J. R. Halloran", true, Indulgenza::realistica);
    Fascicolo b = Carriera::nuovo(r, Marina::vmf, "V. A. Suchkov", true, Indulgenza::realistica);
    b.stato = "morto";
    salva_fascicolo(a, percorso_fascicolo(a.id));
    salva_fascicolo(b, percorso_fascicolo(b.id));
    imposta_in_carica(Marina::usn, a.id);
    imposta_in_carica(Marina::vmf, b.id);

    const auto albo = leggi_albo();
    REQUIRE(albo.size() == 2);
    for (const auto& v : albo) {
        if (v.f.id == a.id) CHECK(v.in_carica);
        if (v.f.id == b.id) CHECK_FALSE(v.in_carica);   // caduto: resta nell'albo, non va in mare
    }
    CHECK(comandante_in_carica(Marina::usn) == a.id);
    CHECK_FALSE(comandante_in_carica(Marina::vmf));
    CHECK(caricabile(a));
    CHECK_FALSE(caricabile(b));

    // un omonimo ha un id suo; l'albo si ferma a ALBO_MAX
    CHECK(id_libero(a.id) == a.id + "_2");
    for (int k = 0; k < ALBO_MAX + 2; ++k) {
        Fascicolo x = Carriera::nuovo(r, Marina::usn, "Comandante " + std::to_string(k), false, Indulgenza::indulgente);
        x.id = id_libero(x.id);
        salva_fascicolo(x, percorso_fascicolo(x.id));
    }
    CHECK(static_cast<int>(leggi_albo().size()) == ALBO_MAX);

    // i codici delle rapide contano una volta per comandante: registrarli non tocca chi e' in carica
    CHECK_FALSE(codice_giocato(a.id, "7KQ2-M9A3-PX4F1"));
    registra_codice(a.id, "7KQ2-M9A3-PX4F1");
    registra_codice(a.id, "7KQ2-M9A3-PX4F1");
    CHECK(codice_giocato(a.id, "7KQ2-M9A3-PX4F1"));
    CHECK_FALSE(codice_giocato(b.id, "7KQ2-M9A3-PX4F1"));
    CHECK(comandante_in_carica(Marina::usn) == a.id);

    // eliminato: il file non c'e' piu' e il posto in carica si libera
    elimina_dall_albo(a.id);
    CHECK_FALSE(fs::exists(percorso_fascicolo(a.id)));
    CHECK_FALSE(comandante_in_carica(Marina::usn));
    CHECK_FALSE(codice_giocato(a.id, "7KQ2-M9A3-PX4F1"));
}
