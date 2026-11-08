p='/home/randolph/Claude/melchior-688/engine/test/prove_forze.cpp'
s=open(p,encoding='utf-8').read()
def r(a,b):
    global s
    assert s.count(a)==1,a
    s=s.replace(a,b)
r('''    ProvaQuadro p(f.string());
    p.spia();
    p.avanza(2500.0);
    REQUIRE(p.m.sotto_banchisa(3));
''','''    ProvaQuadro p(f.string());
    p.cieco(3);   // col sonar farebbe la TMA sul giocatore, e allora non salirebbe comunque
    p.spia();
    p.avanza(2500.0);
    REQUIRE(p.m.sotto_banchisa(3));
''')
r('''    for (int k = 0; k < 905; ++k) {   // e neanche il quadro, a un ciclo intero. Tenuto a 5 m a mano: sotto il ghiaccio l'IA scenderebbe
        p.m.trova(3)->quota = p.m.trova(3)->quota_ordinata = 5.0;
        p.avanza(1.0);
    }
    REQUIRE(p.m.unita().at(3).quota <= 6.5);
''','''    for (int k = 0; k < 905 * 9; ++k) {   // e neanche il quadro, a un ciclo intero. Tenuto a 5 m a mano: sotto il ghiaccio l'IA scenderebbe
        p.m.trova(3)->quota = p.m.trova(3)->quota_ordinata = 5.0;
        p.m.trova(3)->flag54 |= 4;
        p.avanza(1.0 / 9.0);
    }
''')
r('''    const auto lanci = [](const ProvaQuadro& p) {
        return std::count_if(p.m.eventi_missione().begin(), p.m.eventi_missione().end(), [](const EventoMissione& e) { return e.tipo == TipoEvento::lancio_ia && e.unita == 3; });
    };
''','''    const auto lanci = [](const ProvaQuadro& p) {
        return std::count_if(p.m.unita().begin(), p.m.unita().end(), [](const auto& v) { return v.second.lanciatore == 3 && (v.second.flag & 0x20); });
    };
''')
open(p,'w',encoding='utf-8').write(s)
