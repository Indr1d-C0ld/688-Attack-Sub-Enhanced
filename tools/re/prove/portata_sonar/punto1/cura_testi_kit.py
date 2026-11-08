#!/usr/bin/env python3
"""I testi di chiusura della portata in C4, C1 §2.7, E10, capitolo sonar ed E6 (kit, 06/11/2026). Ogni testo vecchio deve esserci una volta sola."""
D = 'docs/manuale_enhanced/'
def cambia(f, vecchio, nuovo):
    t = open(D + f, encoding='utf-8').read()
    assert t.count(vecchio) == 1, (f, vecchio[:50], t.count(vecchio))
    open(D + f, 'w', encoding='utf-8').write(t.replace(vecchio, nuovo))

C4 = '2_riferimento/C4_firme_acustiche.md'
cambia(C4, '''viene dalla firma.

### 2.2 La cavitazione''', '''viene dalla firma.

Le navi civili (`nazionalita = "civile"`: le quattro firme e il traffico generato) hanno lo spettro 17 dB più basso
(`SPETTRO_CIVILI_SOTTO_DB`). I loro `punti` sono la banda misurata (McKenna 2012, Veirs 2016), e integrando lo spettro su
5-200 Hz si otterrebbero 17 dB in più. Le navi da guerra e i sottomarini no: le loro portate sono tarate su questo spettro.

### 2.2 La cavitazione''')
cambia(C4, '''Ogni firma ha la **tonale** (la riga più forte, `tonale_hz`, `tonale_db`) e un elenco di **righe** relative a quella:
`[Hz, dB relativi, tipo, da nodi, a nodi]`.''', '''Ogni firma ha la **tonale** (di norma la riga più forte, `tonale_hz`, `tonale_db`; nelle navi civili a diesel lento lo
sono le righe del diesel, +12, +10, +8 sulla tonale) e un elenco di **righe** relative a quella:
`[Hz, dB relativi, tipo, da nodi, a nodi]`.''')
cambia(C4, '''**Le righe legate all'asse** (blocco 2 del sonar''', '''**Le tonali delle navi civili** seguono lo spettro: mercantile 138 dB, cargo 140, petroliera 143, peschereccio 135, e nel
traffico generato la tonale è 35 dB sotto la banda (era 18). Con la riga a 250-300 Hz più alta, il LOFAR tratteneva il contatto
a 100 km anche con lo spettro abbassato. Le righe del diesel, a 7-28 Hz, servono all'analizzatore a distinguere le classi, e
stanno nel rumore del mare: non allungano la portata.

**Le righe legate all'asse** (blocco 2 del sonar''')

cambia('2_riferimento/C1_equazione_del_sonar.md', '''| Alfa a 20 kn | 175 km (135) |
''', '''| Alfa a 20 kn | 175 km (135) |

Questi esempi usano il livello A, senza raggi: non dipendono dalla griglia, che nella missione arriva a 300 km (capitolo C2 §7).
''')

E10 = '2_riferimento/E10_traffico_civile.md'
cambia(E10, '''- **Sonar:** tracce di mercantili sulla cascata e righe diverse sul LOFAR. Le navi si classificano con la libreria:
  i tipi civili sono voci con gli indizi.
''', '''- **Sonar:** tracce di mercantili sulla cascata e righe diverse sul LOFAR. Le navi si classificano con la libreria:
  i tipi civili sono voci con gli indizi. L'analizzatore di firma riconosce di rado il traffico per classe: da 3 a 4 casi
  su 78 fra 3 e 20 km, con una media di 3-5 righe viste. Oltre 30 km il traffico resta al livello A (capitolo C2 §7).
''')

cambia('1_uso/03_sonar.md', '''Tutti i comandi della stazione restano attivi anche sulla pagina.
''', '''Tutti i comandi della stazione restano attivi anche sulla pagina.

Con la TB-16 l'analizzatore riconosce mercantile, cargo e petroliera fino a 20 km se vanno a 4-8 nodi. A 12 nodi il
mercantile si riconosce fino a 10 km, cargo e petroliera fino a 20 km; a 16 nodi non si riconoscono. Il peschereccio
resta incerto: l'ipotesi giusta arriva, con una confidenza di 0,36. Sul traffico civile il riconoscimento è raro.
''')

E6 = '2_riferimento/E6_ia.md'
cambia(E6, '''- In una scena di prova il comandante IA si è fidato di una soluzione sbagliata e ha lanciato molto oltre la portata
  del siluro.
''', '''- Il comandante IA si fida della soluzione TMA e non controlla la distanza vera: lancia il Mk 48 quando la soluzione lo
  mette entro 15 km, anche se il bersaglio è molto più lontano. Su 10 semi: M3, in 5 semi, a 92-105 km fra 545 e 2430 s, 0 a segno su
  12; M4, in 1 seme, a 95 km, 0 su 2. Riguarda solo il vetrino: le armi dell'IA controllano la portata, e in una
  partita umana il comandante non c'è.
''')
cambia(E6, '''Che cosa vuol dire per la regola è nel §7.
''', '''Che cosa vuol dire per la regola è nel §7. Queste misure sono di prima della griglia a 300 km e dello spettro più basso delle
navi civili: i tempi di oggi sono nel §7.
''')
cambia(E6, '''Il Victor sente il convoglio da solo: a 40 km ha il contatto solido a 83 s, e al
  rilevamento più favorevole (90°) anche a 85-95 km entro 600 s; a 100 km, in giugno, in 3000 s su 2 semi non ha contatto. La regola orienta solo chi
''', '''Il Victor sente il convoglio da solo: nella scena, a 40 km, il primo contatto solido arriva a 85-119 s,
  e sul mercantile a 114-236 s (10 semi). In corse di 3000 s al rilevamento 90°, in giugno, su 3 semi: a 99 km solido sulla petroliera a 264-432 s e sul mercantile a 840-1650 s, con
  perdite di contatto; a 130 km sulla petroliera in 3 semi, sul mercantile in 1. La regola orienta solo chi
''')
