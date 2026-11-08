#pragma once
// Il piano del comandante IA per le missioni originali nella sandbox (docs/19_sandbox.md, V3; nota condivisa
// sandbox-v3-originali-contratto.md). Gli obiettivi delle originali dicono come si vince, non dove andare: il piano lo
// dice, dal briefing e dagli script (data/motore/sandbox_originali.toml, da tools/pipeline/sandbox_originali.py, che
// porta la fonte di ogni voce). Solo cio' che il comandante sa dagli ordini: mai le posizioni dei nemici.

#include <optional>
#include <string>
#include <vector>

#include "nucleo/carico.hpp"
#include "nucleo/geometria.hpp"

namespace m688 {

struct PianoOriginale {
    enum class Condotta { transito, caccia, scorta, pedinamento, ricognizione, minamento, evasione, attacco_terra };
    // libere; roe: quelle della sala radio; solo_difesa: libere dopo la prima arma nemica in acqua; classi: solo le
    // classi del piano, le altre come solo_difesa
    enum class Armi { libere, roe, solo_difesa, classi };
    struct Cerchio { Vec2 centro; double raggio_m = 0.0; };
    struct Rettangolo {
        double x0 = 0, y0 = 0, x1 = 0, y1 = 0;   // ovest, nord, est, sud (come in_rettangolo)
        bool dentro(Vec2 p, double margine = 0.0) const {
            return p.x >= x0 + margine && p.x <= x1 - margine && p.y <= y0 - margine && p.y >= y1 + margine;
        }
    };
    struct Pim { std::vector<Vec2> punti; double velocita_kn = 0.0; };   // la rotta pianificata del convoglio

    Condotta condotta = Condotta::caccia;
    std::string fonte;
    Armi armi = Armi::roe;
    std::vector<std::string> classi;   // i bersagli, o la classe da pedinare
    double quota_m = 0.0, velocita_kn = 0.0;   // 0: la dottrina del comandante
    bool seminare = false;
    std::vector<Vec2> punti;           // la rotta del transito
    std::optional<Cerchio> area, uscita;
    std::optional<Rettangolo> limite;  // fuori, l'originale chiude la missione
    std::optional<Pim> pim;
    double avanti_m = 0.0;             // scorta: la stazione davanti al convoglio
    double distanza_m = 0.0;           // pedinamento: entro questa distanza
    double ping_m = 0.0;               // pedinamento: un impulso del sonar sotto questa distanza
    double attesa_s = 0.0;             // prima, resta sul posto
    std::vector<std::string> tubi;     // le armi del catalogo del carico nei tubi 1..n alla partenza (SCENE0A: i Tomahawk)
};

const char* nome_condotta(PianoOriginale::Condotta c);

// Il piano della missione `id_missione` da <cartella_dati>/sandbox_originali.toml; nullopt se la missione non c'e' (o
// il file manca). std::runtime_error se la voce c'e' ma e' sbagliata
std::optional<PianoOriginale> piano_originale(const std::string& cartella_dati, const std::string& id_missione);

// Il carico di partenza col piano, prima del primo passo: le armi di `tubi` nei tubi da siluro 1..n (dove il locale non
// le accetta nei tubi, nei pozzi: il Flight II), l'arma che c'era in rastrelliera se c'e' posto. Restituisce i motivi
// delle armi che non ha potuto mettere (vuoto: tutto fatto, o il piano non chiede tubi)
std::vector<std::string> applica_tubi_piano(SalaSiluri& s, const PianoOriginale& p);

}  // namespace m688
