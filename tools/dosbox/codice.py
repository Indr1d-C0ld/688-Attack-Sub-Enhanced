"""I passi del pilota che impostano le tre lettere del codice di sicurezza e lo inviano (sala radio del gioco DOS).

    python3 codice.py HIG   ->  passi per catture/NOME/comandi.txt
Gruppo 1 della sala radio: < e > dei tre riquadri (partono da A, girano), poi SEND CODE.
"""
import sys

passi = []
for i, c in enumerate(sys.argv[1].upper()[:3]):
    n = ord(c) - ord("A")
    avanti = n <= 13
    passi += ["k:1"] + ["k:Right"] * (2 * i + (1 if avanti else 0)) + ["k:Return"] * (n if avanti else 26 - n)
passi += ["k:1"] + ["k:Right"] * 6 + ["w:0.5", "s", "k:Return"]
print(" ".join(passi))
