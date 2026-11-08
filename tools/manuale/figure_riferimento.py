"""Figure del volume 2 del manuale che vengono dalle formule del gioco, senza simulazioni (kit, 03/11/2026):
- B1 §2.5: la soglia di cavitazione con la quota, V_inc(z) = V_inc0 √((10,33 + z) / 10,33), per 688, Victor III e Alfa;
- B3 §2.3: la portata di una falla con la quota, Q = C_d A √(2 Δp / ρ), per quattro falle (l'aria a 1 bar);
- C10 §2.3: l'orizzonte radar, d = 1,23 (√h_r + √h_t) miglia con le altezze in piedi, contro l'altezza dell'antenna;
- C12 §2.6: l'errore di posizione senza punti nave, per navigatore: l'errore di velocita' di Gauss-Markov (correlazione
  72 ore con l'inerziale, 12 a stima), tarato sulla deriva di un giorno; a stima la corrente non nota del teatro.
Nello stile di figure_acustica.py; scrivono in docs/manuale_enhanced/figure/riferimento.
    tools/venv/bin/python tools/manuale/figure_riferimento.py
"""
import math
import pathlib
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import figure_acustica as fa   # noqa: E402
from figure_acustica import Grafico, num   # noqa: E402

fa.USCITA = fa.RADICE / "docs/manuale_enhanced/figure/riferimento"

ATM_ACQUA, RHO, G, P_ATM, CD = 10.33, 1025.0, 9.81, 101325.0, 0.62


def cavitazione():
    g = Grafico("La soglia di cavitazione con la quota", "V_inc(z) = V_inc0 · √((10,33 + z) / 10,33); sopra la curva l'elica cavita.",
                0, 400, 0, 35)
    g.assi(range(0, 401, 50), range(0, 36, 5), "quota (m)", "giri in nodi equivalenti")
    for i, (nome, v0) in enumerate((("688", 5.0), ("Victor III", 4.5), ("Alfa", 4.0))):
        g.linea([(z, v0 * math.sqrt((ATM_ACQUA + z) / ATM_ACQUA)) for z in range(0, 401, 5)], i)
    g.legenda(["688, 5,0 nodi\na quota zero", "Victor III, 4,5", "Alfa, 4,0"])
    g.salva("b1_cavitazione.svg", "La soglia di cavitazione con la quota per il 688, il Victor III e l'Alfa")


def falle():
    # la portata in m³ al minuto a compartimento ancora asciutto (aria a 1 bar), su scala logaritmica
    g = Grafico("Quanto entra da una falla", "Portata a compartimento asciutto, Q = 0,62 · A · √(2 Δp / ρ), in m³ al minuto.",
                0, 400, 0, 4.0)
    g.assi(range(0, 401, 50), [0, 1, 2, 3, 4], "quota (m)", "portata (m³/min, scala logaritmica)",
           yfmt=lambda y: {0: "1", 1: "10", 2: "100", 3: "1000", 4: "10000"}[int(y)])
    casi = (("tenuta, 2 pollici", math.pi * (0.0254) ** 2), ("5 pollici", math.pi * (2.5 * 0.0254) ** 2),
            ("0,1 m²", 0.1), ("1 m²", 1.0))
    for i, (nome, area) in enumerate(casi):
        punti = []
        for z in range(5, 401, 5):
            q = CD * area * math.sqrt(2 * RHO * G * z / RHO) * 60.0
            punti.append((z, math.log10(max(q, 1.0))))
        g.linea(punti, i)
    g.legenda([c[0] for c in casi])
    g.salva("b3_falle.svg", "La portata di una falla con la quota per quattro aree, da una tenuta a un metro quadrato")


def orizzonte():
    piedi = lambda m: m / 0.3048
    d_km = lambda hr, ht: 1.23 * (math.sqrt(piedi(hr)) + math.sqrt(piedi(ht))) * 1.852
    g = Grafico("L'orizzonte radar", "d = 1,23 (√h_antenna + √h_bersaglio) miglia, con le altezze in piedi.",
                0, 3.7, 0, 300)
    g.assi([0, 1, 2, 3], range(0, 301, 50), "altezza dell'antenna (m, scala logaritmica)", "distanza dell'orizzonte (km)",
           xfmt=lambda x: {0: "1", 1: "10", 2: "100", 3: "1000"}[int(x)])
    bersagli = (("periscopio, 1,5 m", 1.5), ("albero, 2 m", 2.0), ("battello emerso, 6 m", 6.0), ("nave, 20 m", 20.0))
    for i, (nome, ht) in enumerate(bersagli):
        g.linea([(e / 10, d_km(10 ** (e / 10), ht)) for e in range(0, 38)], i)
    g.legenda([b[0] for b in bersagli])
    for x, testo in ((math.log10(30), "nave, 30 m"), (math.log10(150), "elicottero"), (math.log10(1500), "P-3C")):
        g.verticale(x, testo)
    g.salva("c10_orizzonte.svg", "L'orizzonte radar contro l'altezza dell'antenna, per periscopio, albero, battello emerso e nave")


def errore_navigazione():
    # varianza della posizione da un errore di velocita' di Gauss-Markov: 2 σv² τ² (t/τ − 1 + e^(−t/τ)), per asse
    gm = lambda t, tau: 2 * tau * tau * (t / tau - 1 + math.exp(-t / tau))
    g = Grafico("L'errore di posizione senza punti nave", "Errore radiale a 1σ; l'inerziale tarato sulla deriva di un giorno, la stima sulla corrente non nota.",
                0, 48, 0, 14)
    g.assi(range(0, 49, 6), range(0, 15, 2), "ore dall'ultimo punto nave", "errore radiale (miglia, 1σ)")
    casi = (("ESGN (688 Flight II), 0,5 nm/giorno", 0.5, 72.0, None), ("DMINS (688), 1,5 nm/giorno", 1.5, 72.0, None),
            ("Soz (Alfa), 3 nm/giorno", 3.0, 72.0, None), ("stima, corrente 0,4 kn (Fær Øer)", None, 12.0, 0.4))
    for i, (nome, deriva, tau, corrente) in enumerate(casi):
        if corrente is None:   # σv radiale tale che a 24 ore l'errore radiale sia la deriva
            k = deriva / math.sqrt(gm(24.0, tau))
        else:                  # la velocita' radiale rms e' la corrente non nota
            k = corrente
        g.linea([(t, k * math.sqrt(gm(t, tau))) for t in [x * 0.5 for x in range(0, 97)]], i)
    g.legenda(["ESGN, 0,5 nm\nal giorno (688 FII)", "DMINS, 1,5 (688)", "Soz, 3 (Alfa)", "stima, corrente\n0,4 kn (Fær Øer)"])
    g.salva("c12_errore.svg", "L'errore di posizione senza punti nave col tempo, per tre navigatori inerziali e per la stima")


def main():
    cavitazione()
    falle()
    orizzonte()
    errore_navigazione()


if __name__ == "__main__":
    main()
