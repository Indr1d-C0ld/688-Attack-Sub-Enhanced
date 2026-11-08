p='/home/randolph/Claude/melchior-688/engine/test/prove_forze.cpp'
s=open(p,encoding='utf-8').read()
def r(a,b):
    global s
    assert s.count(a)==1,a
    s=s.replace(a,b)
r('''scena_quadro("rimanda", "crisi", {{3, 0, 3000, 100, 3}},''','''scena_quadro("rimanda", "crisi", {{3, 0, 2000, 100, 1}},''')
r('''    const auto salite = [&](Esperienza e) {
        ProvaQuadro p(f);
''','''    const auto salite = [&](Esperienza e, double dopo_s = 0.0) {
        ProvaQuadro p(f);
''')
r('''        CHECK_FALSE(k->second.posizione_nota);
        return p.reazioni("ascolto_broadcast").size();
''','''        CHECK_FALSE(k->second.posizione_nota);
        p.avanza(dopo_s);
        return p.reazioni("ascolto_broadcast").size();
''')
r('''    CHECK(salite(Esperienza::esperto) == 0);
    fs::remove(f);
''','''    CHECK(salite(Esperienza::esperto) == 0);
    CHECK(salite(Esperienza::esperto, 2500.0) >= 1);   // rimanda, non rinuncia: passata la Perry sale
    fs::remove(f);
''')
r('''        CHECK(p.m.unita().at(3).contatti.count(2) == 0);
        CHECK(lanci(p) == 0);
''','''        const auto& c = p.m.unita().at(3).contatti;
        CHECK((c.count(2) == 0 || !c.at(2).posizione_nota));   // al piu' l'ESM, quando a quota d'antenna la Perry irradia
        CHECK(lanci(p) == 0);
''')
open(p,'w',encoding='utf-8').write(s)
