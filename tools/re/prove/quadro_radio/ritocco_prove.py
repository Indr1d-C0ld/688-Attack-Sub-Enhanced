p='/home/randolph/Claude/melchior-688/engine/test/prove_forze.cpp'
s=open(p,encoding='utf-8').read()
def blocco(inizio, fine, nuovo):
    global s
    a=s.index(inizio); b=s.index(fine,a+1) if fine else len(s)
    s=s[:a]+nuovo+s[b:]
def r(a,b):
    global s
    assert s.count(a)==1,a
    s=s.replace(a,b)
T='TEST_CASE("quadro ai sottomarini: '
SK=' * doctest::skip(!fs::exists(M688_DATI "/classi.toml"))) {\n'
# prova 1 e 7: anche lo Slava riceve come prima
r('''"{ id = 4, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = 90000 }, quota_m = 0, rotta = 90, velocita_kn = 3, " + NAVE + " },\\n");
    ProvaQuadro p(f);
    p.spia();
    p.avanza(5.0);
    CHECK(p.m.contatto_radio(4, 2).has_value());      // la nave: la spia diretta, come prima
''','''"{ id = 4, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = 90000 }, quota_m = 0, rotta = 90, velocita_kn = 3, " + NAVE + " },\\n"
                                "{ id = 6, classe = 'slava', categoria = 'nave', flag = 18, posizione = { x = 20000, y = 90000 }, quota_m = 0, rotta = 90, velocita_kn = 3, " + NAVE + " },\\n");
    ProvaQuadro p(f);
    p.spia();
    p.avanza(5.0);
    CHECK(p.m.contatto_radio(4, 2).has_value());      // le navi: la spia diretta, come prima
    CHECK(p.m.contatto_radio(6, 2).has_value());
''')
# prova 5: anche le ROE si fermano alla banchisa
r('''    REQUIRE(p.m.sotto_banchisa(3));
    CHECK(p.reazioni("ascolto_broadcast").empty());
    CHECK_FALSE(p.m.contatto_radio(3, 2).has_value());
''','''    REQUIRE(p.m.sotto_banchisa(3));
    CHECK(p.reazioni("ascolto_broadcast").empty());
    CHECK_FALSE(p.m.contatto_radio(3, 2).has_value());
    // lo stesso predicato per gli ordini radio: a 5 m ma sotto il ghiaccio le ROE nuove non arrivano
    REQUIRE(p.m.armi_libere_ia(3, 2) == std::optional<bool>(true));
    Intervento x;
    x.tipo = "roe";
    x.testi = {{"parte", "urss"}, {"categoria", "navi"}, {"regola", "trattenute"}};
    REQUIRE(p.m.intervieni(x).ok);
    p.m.trova(3)->quota = p.m.trova(3)->quota_ordinata = 5.0;
    p.avanza(5.0);
    CHECK(p.m.armi_libere_ia(3, 2) == std::optional<bool>(true));
''')
# prova 6: con ritardo x3 il dato arriva 3 volte piu' tardi, lo dice il registro dei messaggi (lo stesso seme estrae lo stesso ritardo)
blocco(T+'il disturbo della parte vale', T+'sale alle finestre', T+'''il disturbo della parte vale come per il giocatore, al teatro; e per la spia"'''+SK+'''    const auto krivak = "{ id = 4, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = 4000 }, quota_m = 0, rotta = 90, velocita_kn = 3, " + NAVE + " },\\n";
    const auto f = scena_quadro("disturbo", "pace", {{3, 0, 60000, 6, 3}}, krivak);
    struct Esito { double arrivo, viaggio; };   // il primo rapporto al battello, il suo viaggio fino al teatro
    const auto prova = [&](double perdita, double ritardo) {
        ProvaQuadro p(f);
        if (perdita > 0.0 || ritardo > 1.0) {
            Intervento x;
            x.tipo = "disturbo_radio";
            x.testi = {{"parte", "urss"}};
            x.parametri = {{"perdita", perdita}, {"ritardo", ritardo}};
            REQUIRE(p.m.intervieni(x).ok);
        }
        Esito e{-1.0, -1.0};
        for (int k = 0; k < 800 && e.arrivo < 0.0; ++k) {
            p.avanza(10.0);
            e.arrivo = p.arrivo(3, 2);
        }
        for (const auto& m : p.m.messaggi_ia())
            if (m.canale == Missione::MessaggioIa::Canale::teatro && m.da == 4 && m.su == 2 && !m.perso) {
                e.viaggio = m.t_arrivo - m.t_partenza;
                break;
            }
        return e;
    };
    const auto libero = prova(0.0, 1.0);
    MESSAGE("il rapporto al battello senza disturbo a ", libero.arrivo, " s, viaggio ", libero.viaggio, " s");
    REQUIRE(libero.arrivo > 0.0);
    REQUIRE(libero.viaggio > 0.0);
    const auto triplo = prova(0.0, 3.0);
    CHECK(triplo.viaggio == doctest::Approx(3.0 * libero.viaggio).epsilon(0.001));
    CHECK(triplo.arrivo > libero.arrivo);
    CHECK(prova(1.0, 1.0).arrivo < 0.0);
    // la spia verso i sottomarini: perdita piena, non arriva niente; la nave la riceve lo stesso
    {
        const auto g = scena_quadro("disturbo_spia", "pace", {{3, 0, 60000, 6, 3}}, "{ id = 4, classe = 'krivak2', categoria = 'nave', flag = 18, posizione = { x = 0, y = 90000 }, quota_m = 0, rotta = 90, velocita_kn = 3, " + NAVE + " },\\n");
        ProvaQuadro p(g);
        Intervento x;
        x.tipo = "disturbo_radio";
        x.testi = {{"parte", "urss"}};
        x.parametri = {{"perdita", 1.0}};
        REQUIRE(p.m.intervieni(x).ok);
        p.spia();
        p.avanza(905.0);
        CHECK_FALSE(p.m.contatto_radio(3, 2).has_value());
        CHECK(p.m.contatto_radio(4, 2).has_value());
        fs::remove(g);
    }
    fs::remove(f);
}

''')
# prova 7: col la spia, in profondita' niente finche' non sale
r('''    cr.cieco(3);
    cr.spia();
    cr.avanza(3200.0);
    CHECK_FALSE(cr.reazioni("ascolto_broadcast").empty());
''','''    cr.cieco(3);
    cr.spia();
    for (int k = 0; k < 3000 && cr.reazioni("ascolto_broadcast").empty(); ++k) cr.avanza(1.0);
    REQUIRE_FALSE(cr.reazioni("ascolto_broadcast").empty());
    CHECK_FALSE(cr.m.contatto_radio(3, 2).has_value());   // la spia gira da un pezzo: finche' non sale, niente
    cr.avanza(3200.0 - cr.m.tempo());
''')
# prova 8: la Perry a 12 nodi passa a 6 km dal Victor mentre si apre la finestra: traccia solida, solo col rilevamento (recluta: non conta)
blocco(T+"col contatto ostile l'esperto rimanda", T+"il dato fresco", T+'''col contatto ostile l'esperto rimanda, la recluta sale"'''+SK+'''    const auto f = scena_quadro("rimanda", "crisi", {{3, 0, 6000, 100, 3}}, "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = -12000, y = 0 }, quota_m = 0, rotta = 90, velocita_kn = 12, " + NAVE + " },\\n", "", "", true, false);
    const auto salite = [&](Esperienza e) {
        ProvaQuadro p(f);
        p.livello(e);
        p.avanza(1950.0);   // la prima finestra si apre a circa 1940 s
        const auto& c = p.m.unita().at(3).contatti;
        const auto k = c.find(2);
        REQUIRE(k != c.end());
        MESSAGE("contatto sulla Perry: qualita' ", k->second.qualita, ", posizione ", k->second.posizione_nota);
        CHECK(k->second.qualita >= 800.0);
        CHECK_FALSE(k->second.posizione_nota);
        return p.reazioni("ascolto_broadcast").size();
    };
    CHECK(salite(Esperienza::recluta) == 1);
    CHECK(salite(Esperienza::esperto) == 0);
    fs::remove(f);
}

''')
# prova 9: l'accostata di 180 gradi a 3 nodi dura circa 500 s
r('''        p.avanza(600.0);   // prima l'accostata, dalla rotta opposta
        const auto& u = p.m.unita().at(3);
        CHECK(u.velocita_ordinata == doctest::Approx(kn));
        CHECK(distanza(u.pos, p.m.unita().at(2).pos) < prima - 0.4 * kn * 0.514 * 600.0);
''','''        p.avanza(1500.0);   // prima l'accostata, dalla rotta opposta: a poca velocita' circa 500 s
        const auto& u = p.m.unita().at(3);
        CHECK(u.velocita_ordinata == doctest::Approx(kn));
        CHECK(distanza(u.rotta, 180.0) < 10.0);
        CHECK(distanza(u.pos, p.m.unita().at(2).pos) < prima - 0.4 * kn * 0.514 * 1500.0);
''')
blocco('''    {   // la spia per due minuti, il punto a 6 km''','''    {   // lontano: l'ellisse passa''','''    {   // la spia per due minuti, il punto a 6 km: un sottomarino che se ne va, e che sul punto nessuno sente. Lo riapre solo un rapporto piu' nuovo
        const auto f = scena_quadro("a_vuoto", "guerra", {{3, 0, 6000, 6, 3}}, "{ id = 2, classe = 'los_angeles', categoria = 'sottomarino', flag = 9, posizione = { x = 0, y = 0 }, quota_m = 200, rotta = 180, velocita_kn = 18, " + SUB + " },\\n", "", "", true, false);
        ProvaQuadro p(f);
        p.cieco(3);
        p.spia(120.0);
        p.avanza(3000.0);
        const auto l = p.reazioni("dato_radio_lasciato");
        REQUIRE_FALSE(l.empty());
        CHECK(l.front().evento == "a_vuoto");
        CHECK(p.reazioni("dato_radio_usato").size() == 1);
        CHECK(p.m.unita().at(3).velocita_ordinata == doctest::Approx(3.0).epsilon(0.05));
        p.spia(120.0, p.m.tempo() + 10.0);
        p.avanza(1100.0);
        CHECK(p.reazioni("dato_radio_usato").size() == 2);
        fs::remove(f);
    }
''')
blocco('''    {
        const auto f = scena_quadro("contatto", "guerra"''','''    {
        const auto gestore = "V = function''','''    {   // la spia ha dato un punto vecchio; la Perry viene incontro e il Victor la sente prima di arrivarci
        const auto f = scena_quadro("contatto", "guerra", {{3, 0, 15000, 6, 3}}, "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 0, velocita_kn = 12, " + NAVE + " },\\n", "", "", true, false);
        ProvaQuadro p(f);
        p.cieco(3, t1 + 30.0);
        p.spia(120.0);
        p.avanza(t1 + 1500.0);
        REQUIRE_FALSE(p.reazioni("dato_radio_usato").empty());
        const auto l = p.reazioni("dato_radio_lasciato");
        REQUIRE_FALSE(l.empty());
        CHECK(l.front().evento == "contatto");
        CHECK(p.reazioni("dato_radio_usato").size() == 1);
        fs::remove(f);
    }
''')
# prova 10 e 12, e via la sonda
blocco('TEST_CASE("SONDA quadro"', None, T+'''nessun siluro col solo dato radio; col contatto proprio lo stesso gestore lancia"'''+SK+'''    // il gestore di bersaglio lancia appena ha un contatto; la Perry scappa a 25 nodi, e il dato della spia resta fresco
    const auto lanci = [](const ProvaQuadro& p) {
        return std::count_if(p.m.eventi_missione().begin(), p.m.eventi_missione().end(), [](const EventoMissione& e) { return e.tipo == TipoEvento::lancio_ia && e.unita == 3; });
    };
    const std::string gestore = "T = function(u) m688.bersaglio(u, 2) m688.memorizza_bersaglio(u) m688.modello_azzera(u) m688.modello_classe(u, 'type53')"
                                " m688.modello_flag(u, 49) m688.lancia(u, '', '') m688.modello_avanti_tutta(u, 45.7) m688.modello_integrita(u, 20)"
                                " m688.modello_quota_max(u, 548) m688.telegrafo(u, 1) m688.navigazione(u, 1) m688.collega(u, 1) m688.nuova_unita(u) end";
    const auto scena_t = [&](const std::string& nome, double y, double kn) {
        return scena_quadro(nome, "guerra", {}, "{ id = 2, classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = 0, y = 0 }, quota_m = 0, rotta = 180, velocita_kn = " + std::to_string(kn) + ", " + NAVE + " },\\n"
                            "{ id = 3, classe = 'victor3', categoria = 'sottomarino', flag = 17, posizione = { x = 0, y = " + std::to_string(y) + " }, quota_m = 6, rotta = 180, velocita_kn = 3, gestori = { bersaglio = 'T' }, " + SUB + " },\\n", gestore, "", true, false);
    };
    {
        const auto f = scena_t("solo_radio", 12000.0, 25.0);
        ProvaQuadro p(f);
        p.cieco(3);
        p.spia();
        p.avanza(1800.0);
        REQUIRE_FALSE(p.reazioni("dato_radio_usato").empty());
        REQUIRE(p.m.contatto_radio(3, 2).has_value());
        CHECK(p.m.unita().at(3).contatti.count(2) == 0);
        CHECK(lanci(p) == 0);
        fs::remove(f);
    }
    {   // il controllo: il gestore e' vivo, col contatto proprio lancia
        const auto f = scena_t("col_contatto", 3000.0, 12.0);
        ProvaQuadro p(f);
        p.avanza(600.0);
        CHECK(lanci(p) >= 1);
        fs::remove(f);
    }
}

'''+T+'''nelle originali e con l'interruttore spento non cambia nulla, stesso registro"'''+SK+'''    const auto impronta = [](const std::string& file, bool acceso, bool sale) {
        ProvaQuadro p(file);
        p.m.usa_quadro_sottomarini(acceso);
        p.spia();
        p.avanza(2600.0);
        std::string s;
        for (const auto& e : p.m.eventi_missione()) s += std::to_string(static_cast<int>(e.tipo)) + "," + std::to_string(e.unita) + ";";
        for (const auto& r : p.m.reazioni_ia()) s += r.reazione + "," + std::to_string(r.t) + ";";
        for (const auto& [id, u] : p.m.unita()) s += std::to_string(u.pos.x) + "," + std::to_string(u.pos.y) + "," + std::to_string(u.quota) + ";";
        CHECK(p.reazioni("ascolto_broadcast").empty() != sale);
        return s;
    };
    const auto o = scena_quadro("uguali_o", "guerra", {{3, 0, 60000, 100, 3}}, "", "", "", false);
    CHECK(impronta(o, true, false) == impronta(o, false, false));
    fs::remove(o);
    // fuori dalle originali: spento non sale e la partita e' un'altra da quella accesa
    const auto e = scena_quadro("uguali_e", "guerra", {{3, 0, 60000, 100, 3}});
    CHECK(impronta(e, false, false) != impronta(e, true, true));
    fs::remove(e);
}
''')
open(p,'w',encoding='utf-8').write(s)
