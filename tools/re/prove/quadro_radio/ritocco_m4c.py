p='strumenti/scene_interazioni.cpp'
s=open(p,encoding='utf-8').read()
i=s.index('ScenarioMissione m4(const DatiScenario& d)')
j=s.index('std::vector<ScenarioMissione> scene_missili',i)
t=s[i:j]
def r(a,b):
    global t
    assert t.count(a)==1,a
    t=t.replace(a,b)
a=t.index('    const Vec2 pattuglia =')
b=t.index('    const char* const CLASSI[]')
t=t[:a]+'''    // il primo rilevamento, di fianco al convoglio, con acqua a 40 km esatti: la distanza e' la scena
    Vec2 pattuglia{};
    bool trovata = false;
    for (const double r : {90.0, 75.0, 105.0, 60.0, 120.0, 135.0, 150.0, 165.0, 180.0, 195.0, 210.0, 225.0, 240.0, 255.0, 270.0, 285.0, 300.0, 315.0})
        if (const Vec2 q = polare(c0, r, 40000.0); !trovata && f.dentro(q) && f.profondita(q) >= 400.0) {
            pattuglia = q;
            trovata = true;
            std::printf("M4: il Victor a rilevamento %.0f, fondale %.0f m\\n", r, f.profondita(q));
        }
    if (!trovata) throw std::runtime_error("M4: niente acqua a 40 km dal convoglio");
'''+t[b:]
open(p,'w',encoding='utf-8').write(s[:i]+t+s[j:])
