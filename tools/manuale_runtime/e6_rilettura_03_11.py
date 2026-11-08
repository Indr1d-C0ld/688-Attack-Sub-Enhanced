"""Rilettura del runtime di E6 «IA» (03/11/2026): le correzioni dei punti «da rivedere dal runtime», dal controllo
col codice. Uso: python3 -I tools/manuale_runtime/e6_rilettura_03_11.py (una volta: ogni sostituzione deve combaciare)."""
import re
import sys

P = 'docs/manuale_enhanced/2_riferimento/E6_ia.md'
s = open(P, encoding='utf-8').read()


def r(old, new):
    global s
    n = s.count(old)
    if n != 1:
        sys.exit('non combacia (%d): %r' % (n, old[:90]))
    s = s.replace(old, new, 1)


SEGNO = re.compile(r'\s*\*Da rivedere dal runtime\.?\*')

# 123: la tonale del sensore migliore
r("Nelle navi con più sonar la tonale è quella del sensore che la vede meglio (dal 30/10/2026;\n  *da rivedere dal runtime*);",
  "Con più sonar la tonale è quella del sensore che la vede meglio, per ogni unità e anche per il giocatore\n  (dal 30/10/2026);")
# 135: lo specchio della schiera
r("Lo risolve un'accostata di\nalmeno 30° (**STIMA**; il rilevamento vero si sposta in modo coerente con la manovra, lo specchio no) o un sensore senza\nspecchio, la sfera o un'eco. Se il lato creduto era quello falso, la TMA ricomincia e la posizione decade (§2.5). La\n*Da rivedere dal runtime.*",
  "Lo risolve un'accostata di\nalmeno 30° dalla rotta del primo rilevamento (**STIMA**; il rilevamento vero si sposta in modo coerente con la\nmanovra, lo specchio no), o un ascolto in cui il sensore migliore non ha specchio, come la sfera. L'eco del ping dà la\nposizione, ma non risolve il lato. Se il lato creduto era quello falso, la TMA ricomincia e la posizione decade (§2.5).")
# 138: giusto
r("restano il VDS e gli idrofoni d'intercettazione. *Da rivedere dal runtime.*", "restano il VDS e gli idrofoni d'intercettazione.")
# 178: il periscopio dell'IA
r("Fuori dall'acqua vede le navi\n  dell'altra parte entro 15 km di giorno, non oltre la visibilità (`NAVE_VISTA_GIORNO_M`, `missione_periscopi.cpp`;\n  **STIMA**). Di notte, dal 29/10/2026, vede fino alla portata notturna di quella nave (`Missione::portata_notte`): la\n  più lunga fra i fanali accesi verso di lui e la sagoma contro il cielo, col binocolo. I 3 km fissi\n  (`NAVE_VISTA_NOTTE_M`) restano solo per le classi senza configurazione delle luci. Anche la vista automatica di notte\n  vale solo entro la portata notturna (`vista_automatica_di_notte`). *Da rivedere dal runtime*;",
  "Fuori dall'acqua vede le navi\n  dell'altra parte entro la portata a vista (`Missione::portata_vista`, `missione_luci.cpp`): la sagoma con la luce del\n  momento, la foschia e l'orizzonte, col binocolo 7×; di notte anche i fanali accesi verso di lui. Le portate fisse di\n  15 e 3 km non ci sono più: una classe senza configurazione delle luci ha la sagoma dalla categoria e dalla lunghezza.\n  La vista automatica di notte vale solo entro la portata notturna (`vista_automatica_di_notte`);")
# 184: giusto
r("  vede le luci, ma non dà la classe. *Da rivedere dal runtime*;", "  vede le luci, ma non dà la classe;")
# 187: l'AOB al periscopio
r("con σ da 10°\n  da vicino a 30° al limite della portata (**STIMA**). Entra nella TMA come una rotta osservata (`aggiungi_rotta`,\n  §2.5); `Missione::tma_dell_ia(id, contatto)` la mostra. *Da rivedere dal runtime*;",
  "con σ da 10°\n  da vicino a 20° al limite della portata; di notte ×1,5 coi fanali e ×2 sulla sola sagoma, ×1,5 sulla sola vela, al\n  più 60° (`nucleo/aob_ia.cpp`, **STIMA**). Entra nella TMA come una rotta osservata (`aggiungi_rotta`, §2.5);\n  `Missione::tma_dell_ia(id, contatto)` la mostra;")
# 192: giusto
r("al posto di quella del bersaglio.\n  *Da rivedere dal runtime*;", "al posto di quella del bersaglio;")
# 195: i transitori dello scafo
r("su un ostile alzano l'allerta come un POSSUB (§2.6). Un'unità dell'IA\n  allertata da A2 in su ne fa un quarto (×0,25), come il battello del giocatore ai posti di combattimento. *La parte della missione è da rivedere dal runtime*;",
  "su un ostile alzano l'allerta come un POSSUB, o come un\n  PROBSUB se il contatto c'era già o se è una soffiata d'emergenza (§2.6). Un'unità dell'IA allertata da A2 in su ne fa\n  un quarto (×0,25), come il battello del giocatore in silenzio o ai posti di combattimento;")
# 240: giusto
r("  classificare. *Da rivedere dal runtime*;", "  classificare;")
# 273 e 288: il libretto
r("Su un contatto visto, o al radar entro\nla portata della vista (una vela emersa entro metà), l'IA dà una classe.",
  "Su un contatto visto, o al radar entro\nla portata della vista (la vela di un sottomarino emerso ha la sua sagoma, più bassa), l'IA dà una classe.")
r("  preferisce a quello del sonarista. *Da rivedere dal runtime.*".lstrip(), "preferisce a quello del sonarista.")
# 291: giusto
r("nemmeno con una classificazione sbagliata. *Da rivedere dal runtime.*", "nemmeno con una classificazione sbagliata.")
# 366: giusto
r("dice al lancio quanto è fresca la posizione (§2.13). *Da rivedere dal runtime.*", "dice al lancio quanto è fresca la posizione (§2.13).")
# 410: possub, albero in pace
r("Un transitorio di scafo sentito su un ostile vale un `possub`\n(§2.3). Un albero visto in pace porta al più alla vigilanza (A1, **STIMA**). *Da rivedere dal runtime.*",
  "Un transitorio di scafo sentito su un ostile vale un `possub`,\no un `probsub` se il contatto c'era già o se è una soffiata (§2.3). Un albero visto da un neutrale, o da una parte in\npace con quella del giocatore, porta al più alla vigilanza (A1, `ALBERO_PACE_MAX`, **STIMA**).")
# 603: giusto
r("I neutrali non pedinano. *Da rivedere dal runtime.*", "I neutrali non pedinano.")
# 608-618: il pedinamento ravvicinato
r("un sottomarino, di qualunque parte, si mette nel cono sordo\ndi un altro sottomarino, a `distanza_m` dietro di lui",
  "un sottomarino, di qualunque parte, si mette dritto a poppa\ndi un altro sottomarino, sulla rotta che gli vede, a `distanza_m` dietro di lui")
r("- Il cono è il più stretto fra i baffles della classe pedinata e l'endfire della sua schiera: 45° per l'Alfa, 35° per\n  il 688, 25° per il 688 con la schiera filata (**STIMA**); 60° per un pedinato senza sensori in ascolto.",
  "- Dritto a poppa si sta dentro ogni cono sordo. `cono_sordo_gradi` calcola il cono (il più stretto fra i baffles della\n  classe pedinata e l'endfire della sua schiera: 45° per l'Alfa, 35° per il 688, 25° per il 688 con la schiera filata,\n  60° per un pedinato senza sensori in ascolto; **STIMA**), ma il pilota non lo usa ancora.")
r("cambia quota\n  di 40 m.", "accosta via verso un\n  punto a 2 km, ad almeno 5 nodi, e va 40 m sopra la quota che vede (sotto, se `sopra_m` è negativo).")
r("(la regola `R_pedina_vicino`). *Da rivedere dal runtime.*", "(la regola `R_pedina_vicino`).")
# 699, 709: giusti
open(P, 'w', encoding='utf-8').write(s)
print('parte 1 fatta')
