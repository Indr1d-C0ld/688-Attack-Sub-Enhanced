p='/home/randolph/Claude/melchior-688/engine/test/prove_forze.cpp'
s=open(p,encoding='utf-8').read()
a=s.index('TEST_CASE("quadro ai sottomarini: oltre 10 nodi')
b=s.index('TEST_CASE("quadro ai sottomarini: sotto la banchisa')
n='''namespace {

// otto navi e la spia, un ciclo solo (dura fra 600 e 900 s): i messaggi copiati a quella velocita', a quota d'antenna
int copiati_quadro(double kn, int semi) {
    std::string navi;
    for (int i = 0; i < 8; ++i)
        navi += "{ id = " + std::to_string(10 + i) + ", classe = 'perry', categoria = 'nave', flag = 10, posizione = { x = " + std::to_string(2000 * i) + ", y = 0 }, quota_m = 0, rotta = 0, velocita_kn = 0, " + NAVE + " },\\n";
    const auto f = scena_quadro("veloce", "pace", {{3, 0, 80000, 6, kn}}, navi, "", "", true, false);
    int ricevuti = 0;
    for (int s = 1; s <= semi; ++s) {
        ProvaQuadro p(f, static_cast<std::uint64_t>(s));
        p.spia();
        p.avanza(905.0);
        ricevuti += static_cast<int>(p.m.quadro_radio(3).size());
    }
    fs::remove(f);
    return ricevuti;
}

}  // namespace

TEST_CASE("quadro ai sottomarini: oltre 10 nodi a quota d'antenna copia circa la meta' dei messaggi" * doctest::skip(!fs::exists(M688_DATI "/classi.toml"))) {
    CHECK(copiati_quadro(8.0, 1) == 8);
    const int veloci = copiati_quadro(15.0, 6);
    MESSAGE("copiati a 15 nodi: ", veloci, " su 48");
    CHECK(veloci >= 14);
    CHECK(veloci <= 34);
}

// la misura lunga, da lanciare a mano (--no-skip): 1600 messaggi in 200 semi, circa 20 minuti
TEST_CASE("quadro ai sottomarini: 200 semi oltre 10 nodi" * doctest::skip()) {
    const int veloci = copiati_quadro(15.0, 200);
    MESSAGE("copiati a 15 nodi: ", veloci, " su 1600");
    CHECK(veloci >= 720);
    CHECK(veloci <= 880);
}

'''
s=s[:a]+n+s[b:]
open(p,'w',encoding='utf-8').write(s)
