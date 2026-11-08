# una copia di scene_interazioni.cpp con la distanza e la durata della M4 prese dall'ambiente: solo per scegliere la scena, fuori da engine/
import pathlib
R = pathlib.Path('/home/randolph/Claude/melchior-688')
t = (R / 'engine/strumenti/scene_interazioni.cpp').read_text(encoding='utf-8')
i = t.index('ScenarioMissione m4(const DatiScenario& d) {')
j = t.index('std::vector<ScenarioMissione> scene_missili(const DatiScenario& d) {')
m = t[i:j]
for a, b in [('s.durata_s = 7200.0;', 's.durata_s = std::atof(std::getenv("M4_DUR"));'),
             ('for (const double r : {90.0, 75.0, 105.0, 60.0, 120.0, 135.0, 150.0, 165.0, 180.0, 195.0, 210.0, 225.0, 240.0, 255.0, 270.0, 285.0, 300.0, 315.0})',
              'for (const double r : {std::atof(std::getenv("M4_RIL"))})'),
             ('polare(c0, r, 40000.0)', 'polare(c0, r, std::atof(std::getenv("M4_KM")) * 1000.0)')]:
    assert m.count(a) == 1, a
    m = m.replace(a, b)
t = t[:i] + m + t[j:]
t = t.replace('#include "nucleo/fondale.hpp"', '#include "nucleo/fondale.hpp"\n#include <cstdlib>', 1)
(R / 'engine/build_radio/sonda_scena/scene.cpp').write_text(t, encoding='utf-8')
