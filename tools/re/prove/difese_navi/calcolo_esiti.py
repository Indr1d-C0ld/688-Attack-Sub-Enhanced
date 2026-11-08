#!/usr/bin/env python3
"""Esiti attesi delle tre regole proposte per le difese delle navi (proposta.md), calcolati esatti, senza il motore.

Per ogni missile, nell'ordine d'arrivo: prima il byte della nave se c'e' un canale libero (scalato con l'allerta), poi il
Phalanx se c'e' un affusto libero (scalato con l'allerta e, contro un supersonico, per 0,5). Solo il primo missile vede
l'allerta di partenza: il suo esito, colpo o missile fermato, porta la nave ad A3 (evento «arma»). Uso:

    python3 tools/re/prove/difese_navi/calcolo_esiti.py
"""
import math

F_ALLERTA = {0: 0.5, 1: 0.75, 2: 1.0, 3: 1.0}
PK_PHALANX = {0: 0.35, 1: 0.45}
PORTATA_PHALANX_M = 1490.0
FINESTRA_CANALE_S = 30.0
SUPERSONICO_M_S = 340.0

# classe: byte, canali, affusti, blocco del Phalanx nel 1989
NAVI = {
    'krivak2 (90)':     (90, 2, 0, 0),
    'kara (90)':        (90, 4, 0, 0),
    'slava (90)':       (90, 3, 0, 0),
    'kirov (90)':       (90, 3, 0, 0),
    'nanuchka (40)':    (40, 1, 0, 0),
    'koni (40)':        (40, 1, 0, 0),
    'perry (15)':       (15, 1, 1, 0),
    'perry (128)':      (128, 1, 1, 0),
    'knox (128)':       (128, 1, 1, 0),
    'spruance (0)':     (0, 1, 2, 0),
    'kidd (16)':        (16, 2, 2, 0),
    'nimitz (15)':      (15, 3, 3, 0),
    'iowa (17)':        (17, 1, 4, 1),
    'sacramento (5)':   (5, 1, 2, 0),
    'mercantile (0)':   (0, 0, 0, 0),
}
# salve: gli istanti d'arrivo. Il giocatore da quattro tubi: la slide 4 s sull'altro lato e 20 s sullo stesso (D3 §2.9);
# l'IA a 4 s l'uno dall'altro (FRA_MISSILI_SALVA_S)
SALVE = {'1': [0.0], '2 a 4 s': [0.0, 4.0], '4 a 4 s': [0.0, 4.0, 8.0, 12.0], '4 giocatore': [0.0, 4.0, 20.0, 24.0]}
MISSILI = {'Harpoon 240 m/s': 240.0, 'P-120 300 m/s': 300.0, 'P-500 450 m/s': 450.0}


def p_oggi(byte):
    return (256 - byte) / 256.0


def salva(nave, v, tempi, allerta):
    """Colpi attesi e P(almeno uno), esatti: si ramifica su ogni esito, perche' un affusto si impegna solo sul missile che
    il byte non ha fermato."""
    byte, canali, affusti, blocco = nave

    def ramo(i, occ_c, occ_a):   # -> (colpi attesi da qui, P(nessun colpo) da qui)
        if i == len(tempi):
            return 0.0, 1.0
        t = tempi[i]
        f = F_ALLERTA[allerta] if i == 0 else 1.0
        p_byte = 0.0
        c2 = occ_c
        if not canali:
            p_byte = math.floor(byte * f) / 256.0
        elif any(x <= t for x in occ_c):
            k = next(j for j, x in enumerate(occ_c) if x <= t)
            c2 = occ_c[:k] + (t + FINESTRA_CANALE_S,) + occ_c[k + 1:]
            p_byte = math.floor(byte * f) / 256.0
        p_ciws = 0.0
        a2 = occ_a
        if any(x <= t for x in occ_a):
            k = next(j for j, x in enumerate(occ_a) if x <= t)
            a2 = occ_a[:k] + (t + PORTATA_PHALANX_M / v,) + occ_a[k + 1:]
            p_ciws = PK_PHALANX[blocco] * f * (0.5 if v > SUPERSONICO_M_S else 1.0)
        # fermato dal byte; fermato dal Phalanx; a segno
        e1, n1 = ramo(i + 1, c2, occ_a)
        e2, n2 = ramo(i + 1, c2, a2)
        p_f1 = p_byte
        p_f2 = (1.0 - p_byte) * p_ciws
        p_hit = (1.0 - p_byte) * (1.0 - p_ciws)
        e3, n3 = (e2, n2) if p_ciws > 0.0 or a2 != occ_a else (e1, n1)
        return p_f1 * e1 + p_f2 * e2 + p_hit * (1.0 + e3), p_f1 * n1 + p_f2 * n2

    colpi, nessuno = ramo(0, (0.0,) * canali, (0.0,) * affusti)
    return colpi, 1.0 - nessuno


def main():
    print('## P(colpo) del singolo missile, per allerta: oggi -> A0 / A1 / A2-A3 (Harpoon; tra parentesi P-500)\n')
    print('| Nave (byte) | Oggi | A0 | A1 | A2-A3 | A2-A3 contro P-500 |')
    print('|---|---|---|---|---|---|')
    for n, d in NAVI.items():
        r = [salva(d, 240.0, [0.0], al)[0] for al in (0, 1, 2)]
        s = salva(d, 450.0, [0.0], 2)[0]
        print(f'| {n} | {p_oggi(d[0]):.0%} | {r[0]:.0%} | {r[1]:.0%} | {r[2]:.0%} | {s:.0%} |')
    print('\n## Colpi attesi per salva, nave ad A2 (oggi -> proposta)\n')
    print('| Nave (byte) | ' + ' | '.join(f'Harpoon {k}' for k in SALVE) + ' | P-500 4 a 4 s |')
    print('|---|' + '---|' * (len(SALVE) + 1))
    for n, d in NAVI.items():
        cel = []
        for k, t in SALVE.items():
            cel.append(f'{len(t) * p_oggi(d[0]):.2f} -> {salva(d, 240.0, t, 2)[0]:.2f}')
        cel.append(f'{4 * p_oggi(d[0]):.2f} -> {salva(d, 450.0, SALVE["4 a 4 s"], 2)[0]:.2f}')
        print(f'| {n} | ' + ' | '.join(cel) + ' |')
    print('\n## La sorpresa: Harpoon 2 a 4 s, P(almeno un colpo) e colpi attesi per allerta di partenza\n')
    print('| Nave (byte) | A0 | A1 | A2 |')
    print('|---|---|---|---|')
    for n in ('krivak2 (90)', 'slava (90)', 'perry (128)', 'kidd (16)'):
        cel = []
        for al in (0, 1, 2):
            c, p1 = salva(NAVI[n], 240.0, SALVE['2 a 4 s'], al)
            cel.append(f'{p1:.0%}, {c:.2f}')
        print(f'| {n} | ' + ' | '.join(cel) + ' |')


if __name__ == '__main__':
    main()
