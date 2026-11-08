p='/home/randolph/Claude/melchior-688/engine/test/prove_forze.cpp'
s=open(p,encoding='utf-8').read()
def r(a,b):
    global s
    assert s.count(a)==1,a
    s=s.replace(a,b)
# banchisa: l'IA sotto il ghiaccio scende da se' (fondale_ia): la si tiene a 5 m a mano
r('''    p.m.trova(3)->quota = p.m.trova(3)->quota_ordinata = 5.0;
    p.avanza(905.0);   // e neanche il quadro, a un ciclo intero
    REQUIRE(p.m.unita().at(3).quota <= 6.5);
''','''    for (int k = 0; k < 905; ++k) {   // e neanche il quadro, a un ciclo intero. Tenuto a 5 m a mano: sotto il ghiaccio l'IA scenderebbe
        p.m.trova(3)->quota = p.m.trova(3)->quota_ordinata = 5.0;
        p.avanza(1.0);
    }
    REQUIRE(p.m.unita().at(3).quota <= 6.5);
''')
# prova 3: messo a quota a mano, 10 s e 100 s prima del ciclo
a=s.index('TEST_CASE("quadro ai sottomarini: salito 10 s prima')
b=s.index('namespace {\n\n// otto navi e la spia')
s=s[:a]+'''TEST_CASE("quadro ai sottomarini: salito 10 s prima del ciclo non copia il messaggio, al ciclo dopo si'; salito 100 s prima lo copia" * doctest::skip(!fs::exists(M688_DATI "/classi.toml"))) {
    const double t1 = primo_ciclo();
    REQUIRE(t1 > 100.0);
    const auto f = scena_quadro("copia", "pace", {{3, 0, 60000, 12, 3}, {5, 20000, 60000, 12, 3}});
    ProvaQuadro p(f);
    p.spia();
    const auto a_quota = [&](int id) { p.m.trova(id)->quota = p.m.trova(id)->quota_ordinata = 5.5; };
    p.avanza(t1 - 100.0);
    a_quota(5);
    p.avanza(90.0);
    a_quota(3);
    p.avanza(15.0);
    CHECK_FALSE(p.m.contatto_radio(3, 2).has_value());
    REQUIRE(p.m.contatto_radio(5, 2).has_value());
    CHECK(p.m.contatto_radio(5, 2)->t_arrivo <= t1 + 2.0);
    p.avanza(t1 + 5.0);
    const auto c = p.m.contatto_radio(3, 2);
    REQUIRE(c.has_value());
    CHECK(c->t_arrivo > 1.9 * t1);
    fs::remove(f);
}

'''+s[b:]
# prova 9: piu' vicino, cosi' dopo il punto il battello esce dal raggio prima che il dato scada, e un ritentativo si vedrebbe
r('''scena_quadro("a_vuoto", "guerra", {{3, 0, 6000, 6, 3}},''','''scena_quadro("a_vuoto", "guerra", {{3, 0, 2000, 6, 3}},''')
r('''// la spia per due minuti, il punto a 6 km: un sottomarino''','''// la spia per due minuti, il punto a pochi km: un sottomarino''')
open(p,'w',encoding='utf-8').write(s)
