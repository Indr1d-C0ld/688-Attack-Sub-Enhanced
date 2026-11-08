# il rimbalzo della salita: un tentativo per ciclo (via della principale, 05/11)
import pathlib
R = pathlib.Path('/home/randolph/Claude/melchior-688')
def r(f, a, b):
    p = R / f; t = p.read_text(encoding='utf-8')
    assert t.count(a) == 1, a
    p.write_text(t.replace(a, b), encoding='utf-8')
r('engine/missioni/include/missioni/forze_missione.hpp',
  '        double salita_da = 0.0, ciclo = 0.0, quota_prima = 0.0, velocita_prima = 0.0, velocita_data = 0.0;\n',
  '        double salita_da = 0.0, ciclo = 0.0, quota_prima = 0.0, velocita_prima = 0.0, velocita_data = 0.0;\n'
  '        double ciclo_tentato = -1.0;    // il ciclo dell\'ultima salita: chiusa da altri, non si ritenta prima del ciclo dopo\n')
F = 'engine/missioni/src/missione_forze.cpp'
r(F, '                        R.ciclo = prossimo;   // arrivato tardi: il ciclo dopo\n',
     '                        R.ciclo = R.ciclo_tentato = prossimo;   // arrivato tardi: il ciclo dopo\n')
r(F, '                   prossimo >= R.ultimo_ascolto + (a >= 3 ? ASCOLTO_A3_OGNI_S : ASCOLTO_OGNI_S) &&\n',
     '                   prossimo != R.ciclo_tentato && prossimo >= R.ultimo_ascolto + (a >= 3 ? ASCOLTO_A3_OGNI_S : ASCOLTO_OGNI_S) &&\n')
r(F, '            R.ciclo = prossimo;\n            R.quota_prima = u.quota_ordinata;\n',
     '            R.ciclo = R.ciclo_tentato = prossimo;\n            R.quota_prima = u.quota_ordinata;\n')
r('engine/test/prove_forze.cpp',
  'TEST_CASE("quadro ai sottomarini: col contatto ostile l\'esperto rimanda',
  '''TEST_CASE("quadro ai sottomarini: una salita disturbata non si ritenta prima del ciclo dopo" * doctest::skip(!fs::exists(M688_DATI "/classi.toml"))) {
    const auto f = scena_quadro("disturbata", "guerra", {{3, 0, 60000, 100, 3}});
    ProvaQuadro p(f);
    p.cieco(3);
    // un altro comandante riprende il battello a ogni secondo: la salita si chiude al passo dopo
    std::set<double> cicli;
    for (int k = 0; k < 4000 && p.m.tempo() < 4000.0; ++k) {
        p.avanza(1.0);
        p.m.trova(3)->velocita_ordinata = 3.0;
    }
    const auto salite = p.reazioni("ascolto_broadcast");
    REQUIRE_FALSE(salite.empty());
    const double ciclo = primo_ciclo();
    MESSAGE("salite disturbate: ", salite.size(), " in 4000 s, il primo ciclo a ", ciclo, " s");
    for (std::size_t i = 1; i < salite.size(); ++i) CHECK(salite[i].t - salite[i - 1].t > 0.5 * ciclo);
    CHECK(p.m.unita().at(3).quota > 90.0);
    fs::remove(f);
}

TEST_CASE("quadro ai sottomarini: col contatto ostile l'esperto rimanda''')
g = R / 'tools/re/prove/quadro_radio/guasti/applica.py'
t = g.read_text(encoding='utf-8')
a = "r('situazione_di(lu, altra) != Situazione::pace &&'"
assert t.count(a) == 1
t = t.replace(a, "r('prossimo != R.ciclo_tentato && ','(GU(87) || prossimo != R.ciclo_tentato) && ')\n" + a)
g.write_text(t, encoding='utf-8')
c = R / 'tools/re/prove/quadro_radio/guasti/corri.sh'
t = c.read_text(encoding='utf-8')
a = '81 *sale alle finestre*\n'
assert t.count(a) == 1
c.write_text(t.replace(a, '87 *salita disturbata*\n' + a), encoding='utf-8')
