# i totali delle serie S5 e M3 come nel T12 §10 (solo lettura)
import tomllib, pathlib, sys
R = pathlib.Path('/home/randolph/Claude/melchior-688/engine')
for nome, p in [('dopo/s5', 'build_difese/tmp/serie/dopo/s5'), ('s5', 'build_radio/serie/s5'), ('s5_spia', 'build_radio/serie/s5_spia'), ('dopo/m3', 'build_difese/tmp/serie/dopo/m3'), ('m3', 'build_radio/serie/m3'), ('m4_a', 'build_radio/serie/m4_a'), ('m4_b', 'build_radio/serie/m4_b'), ('m4_c', 'build_radio/serie/m4_c')]:
    t = tomllib.loads((R / p / 'serie.toml').read_text())
    P = t['partita']
    s = lambda parte, c: sum(x[parte][c] for x in P)
    print(f"{nome:8} navi perse {s('nato','perse'):4.0f} | patto: lanci {s('patto','lanci'):4.0f} a segno {s('patto','a_segno'):4.0f} perse {s('patto','perse'):3.0f} radio {s('patto','radio'):5.0f} | nato: lanci {s('nato','lanci'):4.0f} a segno {s('nato','a_segno'):4.0f} | perse nato per seme {[int(x['nato']['perse']) for x in P]} patto {[int(x['patto']['perse']) for x in P]}")
