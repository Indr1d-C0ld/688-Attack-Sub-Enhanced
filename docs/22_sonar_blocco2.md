# 22 — Sonar, blocco 2: intercettazione e allarme siluro, righe del proprio battello (piano)

*Disassemblatore, 28/10/2026. Stato: approvato dall'utente il 28/10/2026 (tramite la principale), comprese le righe di turbina e riduttore legate all'asse dei nucleari. Ordine: F, poi E.*

Sono i punti 8 e 4 della nota `sonar-confronto-27-10.md`, dopo il blocco 1 (`docs/20`):
- **E:** il ricevitore d'intercettazione, coi ping altrui e i siluri attivi come frequenza e tempo, e l'allarme
  siluro;
- **F:** le righe del proprio battello sul LOFAR, che cambiano coi giri e con l'assetto.

Il classico non cambia: rilievo 0/110 e casi classici identici. La scoperta (`valuta`) resta com'è; E e F cambiano
quello che il sonarista vede, sente e capisce.

Ricerca: `docs/ricerca/28_intercettazione_righe_proprie.md`, con gli appunti in `28_note/`.

## 1. Che cosa c'è oggi

- **Ping altrui:**
  - i sonar attivi delle navi e il BQS-15 sono `Sorgente::ping_*`;
  - la missione li fa sentire ai sensori passivi dell'altro (`intercetta_impulso`): un contatto col rilevamento, il
    messaggio «ci pingano», la reazione dell'IA;
  - le frequenze sono quasi tutte 3,5 kHz, la banda dei sensori attivi del catalogo.
- **Teste dei siluri:** sono transitori (`ping_testa_*`, 25-35 kHz, STIMA). La cadenza della missione accelera con la
  distanza (`intervallo_ping_testa_s`); i messaggi sono 263-266 (testa attiva, aggancio, perdita).
- **Sensori d'intercettazione:** ci sono nelle suite (`wlr9`, `intercettazione_sov`: 3,5, 10 e 25 kHz), ma
  partecipano solo alla scoperta del contatto.
- **Manca:**
  - una vista in frequenza e nel tempo;
  - le misure per emettitore (frequenza, durata, cadenza, livello, CW o FM);
  - le tracce;
  - la logica dell'allarme siluro, che oggi passa solo da testi.
- **Rumore proprio:** è solo a banda larga (`rumore_proprio`, macchine e flusso). Le righe del proprio battello non ci
  sono, salvo la vibrazione del cavo della schiera (blocco 1, D).

## 2. E — Il ricevitore d'intercettazione

**Ricerca** (dossier 28 §1-3):
- **WLR-9:** nessun numero aperto, né bande né precisione. Si sa che classificava per frequenza, durata, livello e
  rilevamento, con idrofoni nella vela e sulla chiglia (Military Periscope, media).
- **Lato sovietico:** è meglio documentato (Kazakov 2019, alta sul testo).
  - Lo «Skat» (Victor III, Akula, Typhoon) misurava portante, deviazione FM, durata, cadenza e livello, con un
    registratore ad anello di 5 s.
  - Il «Rubin» (Victor I e II, Charlie) aveva 2 bande e tanti falsi allarmi.
- **Emissioni:**
  - SQS-53C: 2,6-3,3 kHz, FM di 1-2 s ogni 20-30 s, 235 dB;
  - AQS-13: 9,2-10,8 kHz, 3,5 o 35 ms;
  - SSQ-47: 13-19 kHz, CW ogni 10 s;
  - DICASS: 6,5-9,5 kHz;
  - teste dei siluri a 20-80 kHz, ping di 2-17 ms, cadenza che raddoppia sotto i 400 m (brevetti USN, alta);
  - le frequenze sovietiche non sono note: STIMA.

**Due incongruenze nei dossier, da sistemare:** l'«Enisej» dell'Alfa è un intercettatore nel 06 e una stazione
passiva nel 19; l'MT-70 dell'Akula è un intercettatore per Wikipedia e un sonar da ghiaccio nel 18. Propongo di
trattare l'Alfa al livello del «Rubin» (STIMA) e l'Akula con lo «Skat-3».

**Modello: modulo puro `nucleo/intercettazione.hpp` (mio).**
- **`EmissioneAttiva`:** chi pinga, pos, quota, istante, frequenza, durata, FM o CW, SL, se il ricevitore è nel fascio.
  La compone il runtime dai ping che ha già (sonar di scafo, a immersione, boe attive, teste, BQS-15).
- **`RicevitoreIntercettazione`**, per unità, coi dati del sensore. Le bande proposte per il WLR-9 (STIMA dalle
  emissioni vere):
  - B1, 1-6 kHz: rilevamento ±10-15°;
  - B2, 6-15 kHz;
  - B3, 15-45 kHz;
  - B4, 45-100 kHz, «grezza».

  Da B2 a B4 il rilevamento è ±5-8°. La risoluzione in frequenza è di 100 Hz, la soglia circa 10 dB in 1 kHz. Le
  generazioni sovietiche: «Skat», tutte le misure; «Rubin», solo frequenza e rilevamento, più i falsi allarmi dagli
  impulsi del proprio battello.
- **`ascolta(emissioni, ricevitore, ambiente, t)`:** l'impulso a sola andata con la perdita del livello A alla sua
  frequenza, contro il rumore della banda. Dà gli **intercetti**: ora, banda, frequenza misurata (col Doppler a sola
  andata, `v/c`), durata, livello sopra soglia, rilevamento con l'errore della banda, CW o FM.
- **Tracce per emettitore:** gli intercetti si legano per rilevamento, frequenza e tempo.
  - Per ogni traccia: la cadenza stimata, l'andamento del livello, la deriva del rilevamento, il segno del Doppler.
  - La classificazione: sonar di piattaforma, sonar a immersione, boa attiva, testa d'arma.
- **Allarme siluro** (dossier 28 §3.3, STIMA dai brevetti):
  - testa d'arma = banda B3 o B4, impulso di 50 ms al più, cadenza di 3 s al più;
  - **SILURO IN ACQUA** = una testa d'arma;
  - **SILURO IN ACQUISIZIONE** = in più tre cadenze in calo di oltre il 15%, il livello che sale di 3 dB in 10 s, il
    rilevamento fermo (meno di 2° al minuto) e il Doppler positivo.
- **Storia per la vista:** gli intercetti degli ultimi N minuti (la cascata frequenza-tempo, frequenza in log da 1 a
  100 kHz) e l'elenco delle tracce, come le righe a fosforo del dossier: «BRG 047 · 3.5 kHz · 1.0 s · PRI 24 s ·
  +18 dB · FM».
- **In cuffia:** il ping intercettato trasposto a 1-3 kHz con la cadenza vera (dossier 28 §3.3), nel mixer dell'audio.

**Dati (miei):**
- `[sensori.*]` attivi con frequenza, durata, FM o CW e cadenza per sistema: SQS-53, SQS-56, SQS-26, il BQQ-5 attivo,
  l'AQS-13, le boe, i sovietici di scafo e a immersione (STIMA);
- `[transitori.ping_testa_*]` con la frequenza per testa;
- le bande e le capacità dei ricevitori (`wlr9`, `skat`, `rubin`).

**Contratto col runtime:**
- compone le `EmissioneAttiva` di ogni passo da quello che già emette: il ping di un sonar attivo, il BQS-15, le teste
  coi tempi di `intervallo_ping_testa_s`, le boe e i sonar a immersione dell'ASW aereo;
- fa ascoltare il ricevitore del giocatore e quelli dell'IA (simmetria);
- usa gli allarmi per i messaggi (263-266 e uno nuovo per l'acquisizione), per la reazione dell'IA (oggi
  `stimola_evasione` sul ping) e per il registro;
- la scoperta del contatto da `intercetta_impulso` resta quella di oggi.

**Contratto con la vista (principale):**
- la pagina dell'intercettatore: cascata frequenza-tempo, tracce, lampade gialla e rossa;
- il suono d'allarme, che non è documentato: lo sceglie la principale.

Il modulo dà solo dati.

## 3. F — Le righe del proprio battello sul LOFAR

**Ricerca** (dossier 28 §4-5):
- **Righe e assetto:** rete a 60 Hz (USA) o 50 Hz (URSS) con le armoniche fino alla quinta. Le pompe del primario a
  due velocità sono la sorgente più temuta. Le righe del riduttore vanno coi giri. Il «Quiet Ship Bill» decide quali
  macchine girano a ogni velocità.
- **Vie d'ingresso nei sensori:**
  - la sfera prende le righe per via strutturale, sotto 250 Hz, a tutti i rilevamenti;
  - la schiera trainata le prende per l'acqua, vicino al fuoco anteriore. In acqua profonda c'è anche un'eco a
    10-90° dal fuoco.
- **Numeri:** non ci sono numeri del 688; quelli d'isolamento sono navali (25-35 dB).

**Modello (mio, nei moduli di sempre):**
- **`Ricevitore::proprio`:** la Sorgente del proprio battello, con firma, velocità e variazioni (regime delle pompe,
  giri). Puntatore di default `nullptr`: con `nullptr` tutto è uguale a prima, bit per bit.
- **Righe proprie:** sono le `righe_emesse` della propria sorgente, quindi cambiano da sole coi giri, col regime delle
  pompe (lente e veloci) e con lo snorkel dei diesel.
  - **Sfera e sensori di scafo:** entrano in tutti i fasci con l'isolamento `I` (STIMA 30 dB per la sfera, 22 per i
    sensori conformi), 6 dB in più nei baffles.
  - **Schiera trainata:** entrano solo nei fasci entro ±30° dal fuoco anteriore, con la perdita del cavo
    (`20 log L`) e la direttività. In acqua profonda c'è un'eco più debole su un rilevamento falso a 10-90° (STIMA).
- **Negli spettri:** `spettro_fascio`, il verniero, `righe_ricevute` (sorgente −3, «proprio») e la cuffia.
  `valuta` no: la scoperta resta col rumore proprio a banda larga.
- **Catalogo del rumore proprio:** `righe_proprie(ricevitore)` dà le righe del momento, con macchina, frequenza e
  livello, per la pagina della vista e per la maschera. `righe_osservate` e `confronta_firma` ricevono le righe da
  togliere, entro una tolleranza. Senza maschera il sonarista sbaglia come dal vero: scambia una riga propria per un
  contatto (dossier 09 §3.9).
- **Cosa decidere:** le righe di turbina e riduttore dei sottomarini nucleari oggi sono fisse. Propongo di aggiungerne
  2-3 legate all'asse (`multiplo_asse`, blocco 1), deboli, da −15 a −20 dB sulla tonale. Si vedrebbero salire coi
  giri sul proprio LOFAR e anche su quello degli altri. Cambia di poco la firma dei sottomarini; la classificazione
  resta sulle righe fisse, quelle legate all'asse vanno negli indizi.
- **IA (simmetria):** il sonarista dell'IA (`missione_sonarista`) vede le stesse righe proprie. Lo «Skat-3» ha i
  falsi allarmi da sorgenti di bordo (Kazakov), e il sonarista sovietico mediocre maschera peggio.

**Contratto:**
- **runtime:** riempie `Ricevitore::proprio` per i sensori del giocatore e dell'IA. Il regime delle pompe e il
  silenzio a bordo ci sono già (R6).
- **vista (principale):** marca le righe proprie sul LOFAR, la pagina del catalogo con «maschera sì/no», l'avviso
  quando entra una riga nuova fuori catalogo.

## 4. File

**I miei:**
- moduli nuovi: `nucleo/intercettazione.{hpp,cpp}`;
- moduli esistenti: modifiche in `nucleo/acustica` (`Ricevitore::proprio`, sensori attivi), `nucleo/analisi_acustica`
  (righe proprie e maschera), `nucleo/ascolto` (cuffia);
- dati: `tools/pipeline/acustica.py` e `data/motore/acustica.toml` (emissioni, ricevitori);
- prove: `test/prove_intercettazione.cpp`, `test/prove_righe_proprie.cpp`;
- dossier 28 e le parti del manuale (C5, C7, C8; il kit cura).

**Condivisi, da annunciare:** `engine/CMakeLists.txt`, `engine/README.md`. `Ricevitore` cambia layout: tutti
ricompilano.

**Non miei:** `missione*` (runtime); `app/sonar_enhanced` e la pagina nuova (principale); il manuale come cura (kit).

## 5. Ordine, prove, costi

1. **F, righe proprie:** prima perché è più piccolo e dà la base del rumore di bordo. Costo S-M: pochi giorni miei,
   pochi giorni per la vista. Prove:
   - con `proprio == nullptr` tutto uguale;
   - le righe proprie della sfera uguali in tutti i fasci e più forti nei baffles;
   - quelle legate all'asse che salgono coi giri;
   - il passaggio delle pompe a 15 kn;
   - la schiera che le sente solo verso il fuoco anteriore;
   - la maschera che le toglie da `righe_osservate` e da `confronta_firma`, e senza maschera una riga propria che
     sbaglia la classificazione.
2. **E, intercettazione:** costo M, 1-2 settimane mie; poi l'aggancio del runtime (giorni) e la vista (1-2
   settimane). Prove:
   - l'SQS-53 intercettato in B1 col rilevamento dentro l'errore e misure giuste;
   - fuori banda niente;
   - Doppler positivo per chi si avvicina;
   - una testa che si avvicina passa da nulla a SILURO IN ACQUA a SILURO IN ACQUISIZIONE coi tempi del brevetto;
   - una testa che si allontana non dà acquisizione;
   - un sonar di nave non dà mai allarme siluro;
   - lo «Skat» misura CW e FM, il «Rubin» no e ha falsi allarmi al tasso previsto;
   - ripetibile dal seme;
   - classico identico e rilievo 0/110.

Stima complessiva: 2-3 settimane mie, più runtime e vista.
