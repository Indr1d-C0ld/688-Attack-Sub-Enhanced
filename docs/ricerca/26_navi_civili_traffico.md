# 26 — Navi civili e traffico (dossier di riferimento)

Progetto: remake di *688 Attack Sub* (EA 1989), blocco 1 del sonar (`docs/20_sonar_blocco1.md`).
Il dossier serve a due cose:
- generare le navi civili per individuo, con la firma acustica per LOFAR e DEMON (parte A);
- far passare il traffico mercantile, i traghetti e la pesca come contatti veri nei sette teatri (parte B).

Compilato il 28 ottobre 2026 dal disassemblatore, con due ricerche su fonti aperte. Sono state solo lette pagine e
articoli open access, senza scaricare file. Gli appunti completi, con le fonti riga per riga e le etichette
[alta]/[media]/[bassa]/STIMA, sono in:
- `26_note/rumore_navi.md`: modelli di livello, struttura delle righe, propulsione per tipo negli anni '80, innesco
  della cavitazione, ricetta per il generatore, lacune;
- `26_note/rotte_traffico.md`: per ogni teatro corsie, densità, pesca, traghetti e ghiaccio; regole del generatore,
  tabella riassuntiva, fonti, lacune.

## 1. Rumore delle navi civili: le scelte

**Livello.** Il modello JOMOPANS-ECHO (MacGillivray e de Jong 2021, *J. Mar. Sci. Eng.* 9:369, CC BY) per classe:
- velocità `60 log(V / V_C)` e lunghezza `20 log(l / 91,44 m)`, col picco sotto 100 Hz delle classi da carico;
- integrato su 5-200 Hz come le firme del motore (`nucleo/navi_civili`);
- scarto dell'individuo 4 dB (STIMA: ECHO dà 6 dB comprese le misure);
- correzione d'epoca da −3 a +3 dB per tipo, che porta la media sulle misure (STIMA); il rimorchiatore +13, perché ECHO
  lo misura a 3,7 kn in porto e qui naviga a 11-15 kn.

I controlli sono McKenna et al. 2012 (container 184-188 dB, bulker 184-187) e Veirs et al. 2016:

| Tipo | Veirs 2016 |
|---|---|
| rimorchiatori | 170 ± 5 |
| navi da ricerca | 167 ± 5 |
| traghetti e passeggeri | 166 ± 8 |
| pesca | 164 ± 9 |

**Cavitazione.**
- Sotto l'innesco il livello scende di 6 dB (STIMA).
- Il minimo è 30 dB sotto il livello di servizio: macchine e flusso.
- Velocità d'innesco per tipo:
  - portarinfuse e cisterne: 7-10 kn;
  - portacontainer: 9-12 kn;
  - pescherecci al traino: 3-5 kn.

**Righe** (ITTC 2014; Arveson e Vendittis 2000; appunti §2):
- **elica:** asse e pala;
- **diesel lento a 2 tempi in presa diretta:** accensioni a `cilindri × giri / 60`, legate all'asse. Il rapporto con
  la pala è fisso a ogni velocità, ed è un indizio per classificare;
- **diesel medio a 4 tempi con riduttore:** accensioni a `cilindri × giri / 120`, più il riduttore. Sono fisse con
  l'elica a passo variabile, e allora asse e pala non dicono la velocità;
- **diesel-elettrico:** generatori a giri fissi;
- **turbina:** rotore e ruota del riduttore legati all'asse, più il turbogeneratore;
- **generatori di servizio:** a giri fissi (`120 f / poli`), con la rete a 50 o 60 Hz secondo la bandiera e le sue
  armoniche;
- **pompe:** motori asincroni.

L'unico livello di riga misurato è quello dell'*Overseas Harriette* (pala 174 dB a 16 kn, generatore 179 dB a
24 Hz): gli altri livelli sono STIMA.

**Tipi** (`tools/pipeline/civili.py`):
- **merci:** mercantile da carico generale, portacontainer, portarinfuse;
- **cisterne:** petroliera grande a turbina, petroliera costiera e chimichiera;
- **traghetti e servizi:** traghetto ro-ro e passeggeri, rimorchiatore d'altura, rompighiaccio diesel-elettrico,
  nave oceanografica;
- **pesca:** peschereccio a rampa, BMRT sovietico, peschereccio costiero, cianciolo.

## 2. Traffico nei teatri: le scelte

Le rotte, le densità e le zone di pesca per teatro sono in `26_note/rotte_traffico.md`. Ne vengono i file
`data/motore/teatri/<teatro>.traffico.toml` (parte B, `tools/pipeline/traffico.py`). Le densità degli anni '80 sono
quasi tutte STIMA: i conteggi moderni sono riportati indietro di ×0,5-0,8, come spiega il §0 degli appunti.

## 3. Lacune

Le principali (elenco completo in fondo agli appunti):
- il testo integrale di Arveson e Vendittis;
- i livelli delle righe per tipo;
- le statistiche di flotta 1985-91 (la quota di VLCC a turbina, le pale per tipo, la rete per bandiera);
- le coordinate ufficiali degli schemi di separazione di Gibilterra e di Ouessant negli anni '80;
- i conteggi di traffico degli anni '80;
- gli orari dei traghetti dell'epoca;
- i pescherecci per flotta.
