p='/home/randolph/Claude/melchior-688/engine/test/prove_forze.cpp'
s=open(p,encoding='utf-8').read()
a='''    p.m.trova(3)->quota = p.m.trova(3)->quota_ordinata = 5.0;
    p.avanza(5.0);
    CHECK(p.m.armi_libere_ia(3, 2) == std::optional<bool>(true));
'''
b='''    Intervento o;
    o.tipo = "ordine_falso";
    o.testi = {{"parte", "urss"}, {"ordine", ORDINE}};
    o.parametri = {{"autenticazione", 1.0}};
    REQUIRE(p.m.intervieni(o).ok);
    p.m.trova(3)->quota = p.m.trova(3)->quota_ordinata = 5.0;
    p.avanza(905.0);   // e neanche il quadro, a un ciclo intero
    REQUIRE(p.m.unita().at(3).quota <= 6.5);
    CHECK(p.m.armi_libere_ia(3, 2) == std::optional<bool>(true));
    CHECK_FALSE(p.m.contatto_radio(3, 2).has_value());
    CHECK(std::none_of(p.m.reazioni_ia().begin(), p.m.reazioni_ia().end(), [](const Missione::ReazioneIa& r) { return r.evento == "ordine_falso" && r.unita == 3; }));
'''
assert s.count(a)==1
s=s.replace(a,b)
open(p,'w',encoding='utf-8').write(s)
