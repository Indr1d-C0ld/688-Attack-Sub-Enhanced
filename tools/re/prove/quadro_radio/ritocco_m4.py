p='strumenti/scene_interazioni.cpp'
s=open(p,encoding='utf-8').read()
i=s.index('ScenarioMissione m4(const DatiScenario& d)')
j=s.index('std::vector<ScenarioMissione> scene_missili',i)
t=s[i:j]
def r(a,b):
    global t
    assert t.count(a)==1,a
    t=t.replace(a,b)
r('Fondale::carica(DATI + "/teatri/celtica.fdl")','Fondale::carica(DATI + "/teatri/faroe.fdl")')
r('"PROVA_M4_QUADRO", "celtica", false','"PROVA_M4_QUADRO", "faroe", false')
r('s.giorno = 21, s.mese = 10, s.ora_utc = 14.0;','s.giorno = 12, s.mese = 6, s.ora_utc = 11.0;')
r('const Vec2 c0 = acqua(f, f.proiettata({47.0, -10.0}), 2000.0);','const Vec2 c0 = acqua(f, f.proiettata({60.5, -7.0}), 800.0);')
r('const double rotta = 260.0;','const double rotta = 45.0;')
r('const Vec2 pattuglia = acqua(f, polare(c0, rotta - 90.0, 40000.0), 2000.0);','const Vec2 pattuglia = acqua(f, polare(c0, rotta + 90.0, 40000.0), 800.0);')
r('m.punti = {polare(p, rotta, 120000.0)};','m.punti = {acqua(f, polare(p, rotta, 40000.0), 300.0)};')
r('e.punti = {polare(e.pos, rotta, 120000.0)};','e.punti = {acqua(f, polare(e.pos, rotta, 40000.0), 300.0)};')
r('acqua(f, polare(c0, rotta + 270.0, 120000.0), 2000.0)','acqua(f, polare(c0, rotta + 270.0, 120000.0), 300.0)')
r('// in guerra,\n','// in guerra,\n') if False else None
t=t.replace('in guerra,\n// in acqua fonda: un Victor III','in guerra,\n// nel canale Faroe-Shetland: un Victor III')
open(p,'w',encoding='utf-8').write(s[:i]+t+s[j:])
