# 28 — Intercettazione acustica, emissioni attive e righe del proprio battello (dossier di riferimento)

Progetto: remake di *688 Attack Sub* (EA 1989), blocco 2 del sonar (`docs/22_sonar_blocco2.md`). Il dossier serve al
ricevitore d'intercettazione con l'allarme siluro (punto E) e alle righe del proprio battello sul LOFAR (punto F).

Compilato il 28 ottobre 2026 dal disassemblatore con una ricerca su fonti aperte: solo pagine lette, nessun file
scaricato. Gli appunti, con le fonti riga per riga e le etichette [alta]/[media]/[bassa]/STIMA, sono in
`28_note/intercettazione_righe_proprie.md`:
- **§1:** WLR-9 e successori, e il modello di gioco proposto;
- **§2:** i ricevitori sovietici, «Svet», «Rubin», «Skat», da Kazakov 2019;
- **§3:** le emissioni attive del 1985-91, la cadenza delle teste e la logica d'allarme;
- **§4:** le righe del proprio battello e il catalogo STIMA di un 688;
- **§5:** le vie d'accoppiamento verso sfera, schiera e intercettatori;
- **§6:** le lacune.

**Decisione dell'utente (28/10/2026):** approvato il piano di `docs/22` così com'è, comprese le 2-3 righe deboli di
turbina e riduttore legate all'asse per i sottomarini nucleari. Si fa prima F (le righe proprie), poi E
(l'intercettazione).

## Le scelte principali

- **WLR-9:** nessun numero aperto. Le bande B1-B4 (1-6, 6-15, 15-45, 45-100 kHz), la precisione del rilevamento e la
  soglia sono STIMA, ricavate dalle emissioni vere e dallo «Svet» sovietico.
- **Allarme siluro:** si basa su banda, cadenza, livello, deriva del rilevamento e Doppler. Viene dai brevetti USN
  (teste a 20-80 kHz, ping di 2-17 ms, cadenza che raddoppia sotto i 400 m); la regola è STIMA.
- **Ricevitori sovietici:**
  - lo «Skat» (Victor III, Akula, Typhoon) misura tutto;
  - il «Rubin» (Victor I e II, Charlie) solo frequenza e rilevamento, con falsi allarmi;
  - l'Alfa si tratta come il «Rubin» (STIMA), finché non si chiarisce l'«Enisej».
- **Righe proprie:**
  - rete a 60 o 50 Hz con le armoniche, pompe del primario a due velocità, riduttore e turbina coi giri;
  - si accoppiano per via strutturale alla sfera (isolamento STIMA 30 dB) e per l'acqua alla schiera, vicino al fuoco
    anteriore;
  - nessuna frequenza di macchina del 688 è pubblica.

## Incongruenze da sistemare

- **«Enisej» (Alfa):** è un intercettatore nel dossier 06 e una stazione passiva nel 19.
- **MT-70 (Akula):** è un intercettatore per en.wikipedia e un sonar da ghiaccio negli appunti del 18.
- **WLR-9 nel dossier 09:** la banda di «3,5-25 kHz» non ha fonte.
