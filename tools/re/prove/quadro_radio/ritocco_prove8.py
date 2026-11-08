p='/home/randolph/Claude/melchior-688/engine/test/prove_forze.cpp'
s=open(p,encoding='utf-8').read()
def r(a,b):
    global s
    assert s.count(a)==1,a
    s=s.replace(a,b)
r('''    const auto lanci = [](const ProvaQuadro& p) {
        return std::count_if(p.m.unita().begin(), p.m.unita().end(), [](const auto& v) { return v.second.lanciatore == 3 && (v.second.flag & 0x20); });
    };
''','''    const auto lanci = [](ProvaQuadro& p, double s) {   // guardando ogni 10 s: l'arma a fine corsa esce dalle unita'
        std::set<int> armi;
        for (int k = 0; k < static_cast<int>(s / 10.0); ++k) {
            p.avanza(10.0);
            for (const auto& [id, u] : p.m.unita())
                if (u.lanciatore == 3 && (u.flag & 0x20)) armi.insert(id);
        }
        return armi.size();
    };
''')
r('''        p.spia();
        p.avanza(1800.0);
        REQUIRE_FALSE(p.reazioni("dato_radio_usato").empty());
''','''        p.spia();
        const auto n = lanci(p, 1800.0);
        REQUIRE_FALSE(p.reazioni("dato_radio_usato").empty());
''')
r('''        CHECK(lanci(p) == 0);
''','''        CHECK(n == 0);
''')
r('''        p.avanza(1800.0);
        CHECK(lanci(p) >= 1);''','''        CHECK(lanci(p, 1800.0) >= 1);''')
if '#include <set>' not in s:
    r('#include <string>\n','#include <set>\n#include <string>\n')
open(p,'w',encoding='utf-8').write(s)
