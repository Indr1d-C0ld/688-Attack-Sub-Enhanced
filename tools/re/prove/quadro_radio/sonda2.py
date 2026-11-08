import re
p='test/prove_forze.cpp'
s=open(p,encoding='utf-8').read()
i=s.index('TEST_CASE("SONDA quadro"')
s=s[:i]+r'''TEST_CASE("SONDA quadro" * doctest::skip(!fs::exists(M688_DATI "/classi.toml"))) {
    const auto stampa = [](ProvaQuadro& p, int id) {
        for (const auto& r : p.m.reazioni_ia()) if (r.unita == id) printf("   %6.0f %d %d %s %s %s\n", r.t, r.unita, r.causa, r.ruolo.c_str(), r.reazione.c_str(), r.evento.c_str());
    };
    const auto contatti = [](ProvaQuadro& p, int id) {
        const auto& u = p.m.unita().at(id);
        printf("t %5.0f q %5.1f v %4.1f agg %d:", p.m.tempo(), u.quota, u.velocita, u.agganciato);
        for (const auto& [c, x] : u.contatti) printf(" [%d q=%.0f pos=%d %s t=%.0f]", c, x.qualita, x.posizione_nota, x.sensore.c_str(), x.ultima_scoperta);
        printf("\n");
    };
    puts("=== a_vuoto");
    {
        const auto f = scena_quadro("a_vuoto", "guerra", {{3, 0, 6000, 6, 3}}, "{ id = 2, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, posizione = { x = 0, y = 0 }, quota_m = 200, rotta = 0, velocita_kn = 0, " + SUB + " },\n", "", "", true, false);
        ProvaQuadro p(f);
        p.cieco(3);
        p.spia(120.0);
        for (int k = 0; k < 14; ++k) { p.avanza(200.0); contatti(p, 3); }
        stampa(p, 3);
    }
    puts("=== contatto");
    {
        const auto f = scena_quadro("contatto", "guerra", {{3, 0, 6000, 6, 3}});
        ProvaQuadro p(f);
        p.cieco(3, 900.0);
        p.spia();
        for (int k = 0; k < 14; ++k) { p.avanza(200.0); contatti(p, 3); }
        stampa(p, 3);
    }
    puts("=== rimanda");
    for (const double y : {5000.0, 7000.0, 9000.0}) {
        printf("--- y %.0f\n", y);
        const auto f = scena_quadro("rimanda", "crisi", {{3, 0, y, 100, 3}}, "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = -4000, y = 0 }, quota_m = 0, rotta = 90, velocita_kn = 5, " + NAVE + " },\n", "", "", true, false);
        ProvaQuadro p(f);
        p.livello(Esperienza::esperto);
        for (int k = 0; k < 15; ++k) { p.avanza(200.0); contatti(p, 3); }
        stampa(p, 3);
    }
}
'''
open(p,'w',encoding='utf-8').write(s)
