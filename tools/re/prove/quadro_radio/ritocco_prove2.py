p='/home/randolph/Claude/melchior-688/engine/test/prove_forze.cpp'
s=open(p,encoding='utf-8').read()
def r(a,b):
    global s
    assert s.count(a)==1,a
    s=s.replace(a,b)
r('''scena_quadro("rimanda", "crisi", {{3, 0, 6000, 100, 3}},''','''scena_quadro("rimanda", "crisi", {{3, 0, 3000, 100, 3}},''')
r('''    // il gestore di bersaglio lancia appena ha un contatto; la Perry scappa a 25 nodi, e il dato della spia resta fresco
''','''    // il gestore di bersaglio lancia appena ha la traccia e la posizione, come R_sottomarino; la Perry scappa a 25 nodi, e il dato
    // della spia resta fresco
''')
r('''"T = function(u) m688.bersaglio(u, 2) m688.memorizza_bersaglio(u)''','''"T = function(u) if not (m688.traccia_solida(u) and m688.soluzione(u)) then m688.comportamento(u, 'T') m688.salva_modello(u) return end"
                                " m688.bersaglio(u, 2) m688.memorizza_bersaglio(u)''')
r('''        p.avanza(600.0);
        CHECK(lanci(p) >= 1);''','''        p.avanza(1800.0);
        CHECK(lanci(p) >= 1);''')
open(p,'w',encoding='utf-8').write(s)
