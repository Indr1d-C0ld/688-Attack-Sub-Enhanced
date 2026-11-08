# 26 — Achievement, statistiche e carriera: l'aggiornamento (proposta)

Richiesta dell'utente del 29/10/2026: tenere aggiornati achievement, voci delle statistiche, gradi e promozioni con le
novità integrate. `data/motore/achievement.toml` (41 voci) e `data/motore/carriera.toml` sono del 26/10, prima di
libretto, telemetro, AOB, missili antinave, intercettazione, quadro del fondale, navigazione incerta e GPS, ghiaccio,
mine, meteo, collisioni, artiglieria, autenticazione radio e confronto delle classificazioni.

## 1. Un difetto da correggere comunque

`ContestoMissione::realismo` (nucleo/carriera.hpp) non viene mai impostato: vale sempre 1. Gli aiuti quindi non
abbassano il punteggio, anche se `carriera.toml` prevede il moltiplicatore da 1 a 0,5. Da collegare alle opzioni della
partita:

| Aiuto acceso | Peso (STIMA) |
|---|---|
| Navigazione esatta (senza posizione incerta) | −0,10 |
| GPS «1990+», con la navigazione incerta | −0,05 |
| Difficoltà ASW facile (con l'aiuto del telemetro) | −0,10 |
| Verità dei contatti (V) accesa almeno una volta, scie di tutte le unità | −0,05 l'uno |
| Sonarista esperto, invece di normale | −0,05 |
| Sonar moderno: LOFAR colorato per rilevamento (RIL) guardato acceso alla stazione Sonar | −0,05 |
| Senza variazioni acustiche (R6) | −0,05 |

Le opzioni più dure non alzano oltre 1: tolgono solo la riduzione. Il classico resta com'è.

Fatto il 29/10 (`realismo_partita` in app/main.cpp, passato a `chiudi_partita`). Rispetto alla prima stesura la
navigazione esatta pesa più del GPS, che dà solo punti nave, e il MATE non conta: è uno strumento, non un aiuto.
Il rapporto di pattuglia mostra la catena intera (somma esatta × situazione × avversario × realismo × pericolo =
totale) e gli aiuti accesi col loro peso.

## 2. Achievement nuovi (14)

| Id | Nome | Condizione | Ambito |
|---|---|---|---|
| occhio_libretto | Occhio al libretto | identificazioni visive giuste: 1, 10, 50 | somma |
| telemetrista | Telemetrista | una misura del telemetro entro il 5% della distanza vera | missione |
| angolo_giusto | Angolo sulla prora | un AOB entro 10° del vero, mandato alla TMA | missione |
| prima_la_vista | Prima la vista | un contatto identificato a vista prima del sonar, giusto | missione |
| oltre_orizzonte | Oltre l'orizzonte | un bersaglio affondato con un missile antinave oltre l'orizzonte | missione |
| scatola_pulita | Scatola pulita | una salva di missili con un neutrale nella scatola, senza colpirlo | missione |
| allarme_siluro | Allarme siluro | un siluro evitato dopo l'allarme dell'intercettatore | somma: 1, 5 |
| sotto_chiglia | Sotto la chiglia | navigare oltre 10 minuti con meno di 20 m d'acqua sotto la chiglia, senza toccare | missione |
| punto_nave | Punto nave | finire con la navigazione incerta e l'errore sotto i 500 m, senza GPS | missione |
| non_autentico | Non autenticato | un ordine radio non autenticato riconosciuto e non eseguito | missione |
| campo_minato | Campo minato | un campo di mine attraversato col BQS-15, senza danni | missione |
| tempesta | Nella tempesta | una missione compiuta con mare 6 o più | missione |
| sotto_il_fuoco | Sotto il fuoco | missione compiuta dopo essere stati presi a cannonate | missione |
| novita_acint | Per l'intelligence | una riga nuova nel rapporto ACINT, non spiegata dalla libreria | somma: 1, 10 |

Le missioni nuove della libreria (25) entrano in «Biblioteca», da riallineare al conto attuale.

## 3. Voci nuove delle statistiche (cumulate e per missione)

- **Identificazioni visive:** giuste e sbagliate; il confronto sonar contro vista, con le percentuali del debriefing.
- **Telemetro:** misure fatte ed errore medio in percentuale.
- **AOB:** osservazioni ed errore medio in gradi.
- **Missili:** lanciati, a segno, fuori bersaglio, neutrali nella scatola.
- **Ecoscandaglio:** ping, a parte da quelli del sonar attivo.
- **Navigazione:** errore medio e massimo della posizione stimata, punti nave fatti.
- **Ghiaccio:** tempo sotto il pack.
- **Intercettazione:** allarmi di siluro, siluri evitati dopo l'allarme.
- **Mine:** scoperte col BQS-15, attraversamenti.
- **Radio:** messaggi verificati, non autentici riconosciuti.
- **Urti:** collisioni e quasi collisioni; colpi d'artiglieria subiti.

## 4. Carriera e punteggio

- **I gradi e la progressione** (USN da O-1 a O-7, VMF da tenente a contrammiraglio) restano come sono: le novità
  cambiano il punteggio delle missioni, non la struttura delle promozioni.
- **Esposizione:** tre voci nuove in `[esposizione]`:
  - ecoscandaglio (un ping verso il fondo, meno del sonar attivo);
  - radar proprio acceso;
  - lancio di un missile (la vampa e la scia in superficie).
- **Classificazioni:** `peso_classificazioni` conta anche quelle a vista. Un neutrale identificato male e poi colpito
  resta la penalità «neutrale» di oggi; il registro dice che era un errore d'identificazione.
- **Realismo:** il moltiplicatore del §1.

## 5. Chi fa cosa

- **Disassemblatore:** `tools/pipeline/achievement.py` e `carriera.py`, i dati e le prove.
- **Runtime:**
  - gli eventi che mancano al registro: la misura del telemetro come ordine, che il replay vede; l'allarme
    dell'intercettatore col suo esito; il tempo sotto il ghiaccio e sotto chiglia; gli attraversamenti di mine;
  - il conto delle statistiche.
- **Principale:** il realismo dalle opzioni della partita, le voci nuove nelle schermate delle statistiche e nel
  debriefing.
- **Kit:**
  - le icone degli achievement nuovi;
  - il manuale: achievement, statistiche e punteggio.

## 6. Decisione dell'utente (29/10/2026)

Approvato tutto: i 14 achievement, le voci delle statistiche, le tre voci d'esposizione e il realismo dagli aiuti coi
pesi della tabella del §1.
