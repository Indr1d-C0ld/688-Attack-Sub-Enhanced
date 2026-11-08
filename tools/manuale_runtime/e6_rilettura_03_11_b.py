"""Rilettura del runtime di E6 «IA» (03/11/2026), seconda parte: dal §2.12 al §7. Uso: python3 -I
tools/manuale_runtime/e6_rilettura_03_11_b.py (una volta: ogni sostituzione deve combaciare)."""
import sys

P = 'docs/manuale_enhanced/2_riferimento/E6_ia.md'
s = open(P, encoding='utf-8').read()


def r(old, new):
    global s
    n = s.count(old)
    if n != 1:
        sys.exit('non combacia (%d): %r' % (n, old[:90]))
    s = s.replace(old, new, 1)


# lo stato in testa
r("Stato: bozza; le parti segnate «da rivedere dal runtime» aspettano la sua rilettura (l'ultima\nrevisione completa è del runtime, 27/10/2026).",
  "Stato: bozza, riletta dal runtime il 03/11/2026 sui punti nuovi, confrontati col codice (l'ultima\nrevisione completa è del runtime, 27/10/2026).")
# 701, 711, 810, 849, 939, 970: giusti
r("Dal 30/10/2026 (*da rivedere dal runtime*):", "Dal 30/10/2026:")
r("il tiro: immergersi lo affonderebbe. Il giocatore sceglie da sé (capitolo E2). *Da rivedere dal runtime.*",
  "il tiro: immergersi lo affonderebbe. Il giocatore sceglie da sé (capitolo E2).")
r("1989; *da rivedere dal runtime*). Le classi della tabella", "1989). Le classi della tabella")
r("Dal 29/10/2026 (`passo_missili_ia`, `missione_missili.cpp`; *da rivedere dal runtime*).", "Dal 29/10/2026 (`passo_missili_ia`, `missione_missili.cpp`).")
r("**Il pilota sulla stima** (dal 29/10/2026, `missione.cpp`; *da rivedere dal runtime*).", "**Il pilota sulla stima** (dal 29/10/2026, `missione.cpp`).")
r("  nessuno vira verso una secca (§2.14). *Da rivedere dal runtime.*", "  nessuno vira verso una secca (§2.14).")
# 757: il lancio sul punto d'incontro
r("Dal 31/10/2026, con una soluzione osservabile della TMA (§2.5), la rotta va al punto d'incontro col moto\n  stimato (`stima_tma_ia`), non alla posizione di adesso (*da rivedere dal runtime*);",
  "Dal 31/10/2026, con l'ultima soluzione osservabile della TMA (§2.5), la rotta e la distanza d'armamento vanno\n  al punto d'incontro col moto stimato (`stima_tma_ia`), non alla posizione di adesso;")
# 765: quando l'IA non lancia
r("**Quando l'IA non lancia** (dal 31/10/2026, `missione_armi_ia.cpp`, **STIMA**; *da rivedere dal runtime*):",
  "**Quando l'IA non lancia** (dal 31/10/2026, `missione_armi_ia.cpp`, **STIMA**):")
r("  Il dato è l'ultima soluzione osservabile della TMA, o l'ultima scoperta per un'eco, una vista o il radar.",
  "  Il dato è l'ultima soluzione osservabile della TMA; per una posizione da un'eco, una vista o il radar è l'ultima\n  scoperta del contatto, che però anche l'ascolto passivo rinnova (`Contatto::ultima_scoperta`; un limite, §7).")
# 825: le salve
r("- Le salve della dotazione si contano anche qui, a ogni chiamata di `m688.attacca`.",
  "- Le salve della dotazione contano solo per l'attacco a dado: chi ha l'arma vera usa i suoi tubi e la sua riserva\n  (dal 03/11/2026; prima le salve si consumavano anche mentre il siluro vero aspettava).")
# 831: l'elicottero imbarcato
r("**L'elicottero imbarcato** (dal 30/10/2026, **STIMA**; *da rivedere dal runtime*). Se il contatto è oltre i tubi ed\nentro 60 km, la nave fa decollare il suo elicottero,",
  "**L'elicottero imbarcato** (dal 30/10/2026, **STIMA**). Se, a una chiamata di `m688.attacca`, il contatto è oltre i\ntubi ed entro 60 km, e la nave potrebbe tirare (un tubo carico, nessuna arma in corsa), fa decollare il suo elicottero,")
# 905-912: le mine e il varco
r("rotta passerebbe a meno di 300 m da una mina vista nell'ultima ora entro 2 km davanti, accosta quanto basta dal lato\n   opposto (`evita_mine`, capitolo D4 §2.8). Il sonar si accende da solo nei campi noti entro 5 km\n   (`SituazioneFondo::campo_minato`),",
  "rotta passerebbe a meno di 300 m da una mina vista nell'ultima ora entro 2 km, prende la rotta libera più vicina\n   (`evita_mine`, capitolo D4 §2.8) e rallenta a 5 nodi; dentro i 300 m va via dalla mina, e chiuso da ogni parte torna\n   indietro. Il sonar si accende da solo, salvo in silenzio (A3, o un ostile entro 15 km), entro 5 km dalle mine della\n   sua parte o da quelle già viste (`SituazioneFondo::campo_minato`),")
r("Fra più mine cerca il varco: la rotta più vicina a quella voluta che passa a 300 m da\n   tutte per 2 km, a passi di 5° (`PASSO_VARCO_GRADI`); nella prova passa a 326 m. *Da rivedere dal runtime.*",
  "La rotta libera è quella più vicina alla voluta che passa a 300 m da tutte le mine\n   per 2 km, provata a passi di 5° (`PASSO_VARCO_GRADI`), prima a dritta; nella prova passa a 326 m.")
# 924: l'inerziale
r("L'inerziale dell'IA non si guasta;", "L'inerziale dell'IA si guasta coi danni, come quello del\n  giocatore (`inerziale_ia_giu_`, da `DanniSottomarini`);")
# 935-937: le navi col navigatore
r("(Loran-C, Transit, Čajka; il GPS dal 1990)", "(Loran-C, Transit, Čajka; il GPS con la chiave `navigazione.gps` o con l'opzione «1990+», come il\ngiocatore, dal 03/11/2026)")
r("una Perry sbaglia di 387 m, una Krivak II di 2450 m. *Da rivedere dal runtime.*", "una Perry sbaglia di 387 m, una Krivak II di 2450 m.")
# 955-962: che cosa resta vero dello script
r("la meta di `vai_a`,\n  il primo punto", "la meta di `vai_a` (vera se\n  l'arrivo dà un esito), il primo punto")
r("risulta spostato del suo errore. *Da rivedere dal runtime.*", "risulta spostato del suo errore.")
# 981: il diesel sotto il pack
r("- **Il diesel sotto il pack** (dal 29/10/2026, `missione_diesel.cpp`, **STIMA**; *da rivedere dal runtime*).",
  "- **Il diesel sotto il pack** (dal 29/10/2026, `missione_diesel.cpp`, **STIMA**).")
r("se no il margine del pack che conosce, dove la copertura scende sotto 0,3, entro 100 km.",
  "se no il punto più vicino del margine del pack, dove la copertura vera scende a 0,3 o meno, entro 100 km (il motore\n  lo prende dal ghiaccio vero, non da quello che il battello ha visto: un limite).")
# §7: le salve corrette, il limite della posizione fresca
r("- Le salve della dotazione (§2.13) si contano a ogni `m688.attacca`, anche per le navi col siluro vero e anche quando\n  aspettano la cadenza o la posizione: uno script che chiama l'attacco ogni secondo può finirle prima del primo siluro.\n  *Da verificare col runtime.*",
  "- Al lancio, la «freschezza» di una posizione da eco, vista o radar è l'ultima scoperta del contatto, che anche\n  l'ascolto passivo rinnova: una posizione vecchia può sembrare fresca finché il contatto si sente (§2.13). Serve il\n  tempo della posizione nel contatto, un cambio del nucleo della missione.")
open(P, 'w', encoding='utf-8').write(s)
print('parte 2 fatta')
