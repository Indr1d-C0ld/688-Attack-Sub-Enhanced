p='strumenti/scene_interazioni.cpp'
s=open(p,encoding='utf-8').read()
i=s.index('ScenarioMissione m4(const DatiScenario& d)')
j=s.index('std::vector<ScenarioMissione> scene_missili',i)
t=s[i:j]
def r(a,b):
    global t
    assert t.count(a)==1,a
    t=t.replace(a,b)
r('const double rotta = 45.0;','const double rotta = 0.0;')
r('const Vec2 pattuglia = acqua(f, polare(c0, rotta + 90.0, 40000.0), 800.0);','const Vec2 pattuglia = acqua(f, polare(c0, rotta + 90.0, 40000.0), 400.0);')
open(p,'w',encoding='utf-8').write(s[:i]+t+s[j:])
