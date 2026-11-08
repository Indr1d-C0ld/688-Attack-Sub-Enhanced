# Riferimento del rilievo del classico

`rilievo_missioni.tsv`: le 110 partite delle originali col giocatore passivo (`strumenti/rilievo_missioni`, sul fondale
reale). Il controllo è `tools/regressione/rilievo_classico.sh`: deve dare «0 partite diverse su 110».

## Aggiornamenti

- 28/10/2026 (runtime, deciso dalla principale): **SCENE0A, semi 2, 4 e 5**. `Missione::colloca` sposta all'avvio il
  battello del giocatore solo sotto l'acqua minima (pescaggio + 0,5 m) o in acqua chiusa, non più sotto la comoda
  dell'IA: nella 0A il 688 parte dove lo mette l'originale (15-16,5 m di fondo), invece che a 10-35 km. Correzione di
  fedeltà: l'originale il fondale non ce l'ha. Esiti e totali uguali (142, 117/170, 79); cambiano solo:
  - seme 2: t_scoperto 60 → 57;
  - semi 4 e 5: t_scoperto 58 → 540, scopritori 3 → 2.
