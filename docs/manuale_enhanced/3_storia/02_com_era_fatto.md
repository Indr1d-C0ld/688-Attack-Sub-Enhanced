# 2. Com'era fatto

*Quello che il reverse engineering di questo progetto ha trovato dentro il gioco del 1989. I dettagli tecnici, coi
formati e gli indirizzi, sono in `docs/01_analisi_tecnica_originale.md`, `docs/04_vm_missioni.md` e `docs/originale/`.*

## 2.1 I file

Il gioco stava in due dischi. Sul disco fisso, dopo l'installazione:

| File | Che cos'è |
|---|---|
| `688.BAT` | lancia `TITLE.EXE`, poi `GAME n` secondo il codice d'uscita: la scelta della missione |
| `TITLE.EXE` | titoli e scelta della missione (17 KB) |
| `GAME.EXE` | il gioco (105 KB compressi) |
| `CALLS.EXE` | il lettore della musica dei titoli (65 KB) |
| `DISK1.MLB`, `DISK2.MLB` | gli archivi delle risorse: 205 file in tutto |
| `688.PAK`, `ALFA.PAK`, `TITLE.PAK` | schermate VGA a 320×200, con le loro palette |
| `MODEM.688` | la stringa d'inizializzazione del modem |

I tre eseguibili sono scritti in **Turbo C 2.0** di Borland (1988) e compressi con **EXEPACK** di Microsoft. Per
studiarli il progetto ha scritto da zero un decompressore EXEPACK: senza, Ghidra analizzava codice ancora compresso.
`TITLE.EXE` conserva perfino i simboli di debug, coi nomi dei moduli: `TITLE.C`, `DECODE.ASM`, `FBLOAD.ASM`,
`SETEGA.ASM`, `HERC_CGA.ASM`, `PALETTE.ASM`.

## 2.2 La grafica

- **Schermate** a 320×200 e 256 colori (il modo 13h della VGA), con le palette a 8 bit per colore. Una palette per il
  giorno e una per la notte (`DAY.PAL`, `NIGHT.PAL`).
- **Sprite** compressi a corse (RLE), quattro bit per pixel su un «banco» di palette: il banco 0 per l'interfaccia (i 16
  colori EGA), il banco 144 per le foto digitalizzate in 16 grigi (navi, elicotteri, missili, fumo, equipaggio). Le
  navi hanno cinque viste, la *Nimitz* otto.
- **Compressione** LZW a 9-12 bit senza codice di reset, per le immagini più grandi: le centrali, le **carte
  batimetriche dei sette teatri** (`SURFA` … `SURFG`, sedici livelli di profondità), la scrivania e le cartoline di
  fine missione.
- Le versioni per **EGA, CGA, Hercules e Tandy** avevano i loro moduli grafici.

## 2.3 Il suono

- **La musica dei titoli**, di Rob Hubbard, in tre arrangiamenti diversi nello stesso `CALLS.EXE`:
  - **AdLib**: nove voci FM (OPL2), 177 secondi;
  - **Tandy/PCjr**: due voci a onda quadra, una di rumore e campioni a 4 bit, 155 secondi;
  - **Covox Speech Thing**: una voce campionata con cinque strumenti, 78 secondi.

  Nel rifacimento suonano tutte e tre nel menu (vol. 1 §1.6): l'AdLib con ymfm, che emula il chip OPL2; il Tandy e il
  Covox con gli emulatori del progetto (`tools/re/tandy_musica.py` e `covox_musica.py`).
- **Sei campioni** a 4 bit per gli effetti: il ping, due eliche, il siluro, l'esplosione, la balena. Suonavano
  sull'altoparlante del PC, sul Tandy o sul Covox, con un ciclo d'attesa tarato sulla velocità della CPU misurata
  all'avvio. Ogni suono fermava il gioco finché non era finito. Il classico del rifacimento li suona agli stessi
  eventi, ma senza fermare il gioco.
- **ANALYZE**, l'analizzatore della sala sonar, ricampionava i suoni delle eliche e della balena a una frequenza che
  dipendeva dal tipo d'unità: era la «firma» di ogni classe. Anche il grafico dell'analizzatore era una firma fissa: sei
  barre che scrivevano in binario il numero del tipo.

## 2.4 Le missioni: una macchina virtuale

Le missioni non sono dati: ogni scena (`SCENE1A.TOK` e le altre 21) è un **programma** per una piccola macchina
virtuale dentro `GAME.EXE`, con 118 istruzioni. Lo stesso programma:

- crea le unità e ne guida il comportamento: ogni unità ha tre script, all'arrivo, al bersaglio e al bersaglio perso, e
  se li riassegna da sola, quindi ogni comportamento è una piccola macchina a stati;
- comanda perfino il **sottomarino del giocatore**: un tasto diventa un codice di comando, e il dispatcher scrive i
  parametri (la posizione del telegrafo, l'angolo di barra) direttamente negli argomenti della prima istruzione del
  gestore scritto nello script;
- gestisce i **timer** (un tick ogni 2 secondi), le variabili di scena, la casualità (un generatore congruenziale a un
  byte), i messaggi di bordo e l'esito: i codici di messaggio da 100 in su sono le pagine di fine missione.

Il briefing e i testi stanno nei file `SCENE*.LZA`, compressi, con un piccolo linguaggio a token per le righe, i colori e
i valori della missione.

Le dieci missioni hanno varianti per il 688 (A) e per l'Alfa (B); le scene C hanno in testa tutti e due i sottomarini
e sono probabilmente la partita a due via modem (volume 2, capitolo A3): 22 scene in tutto. Lo «0» è la decima.

## 2.5 Come «sentiva» il sonar

Il modello di scoperta, ricostruito istruzione per istruzione:

- **un solo osservatore per passo**, nove passi al secondo, a turno nella lista delle unità; le navi non combattenti non
  osservano;
- **solo nel settore di prua**: ±30° col sonar di scafo, ±90° con l'antenna rimorchiata. Per questo quasi ogni script mette
  la prua sul rilevamento del bersaglio;
- la **portata** è la somma di tre termini: il guadagno del settore, il rumore del bersaglio (che cresce con la sua
  velocità, triplo oltre «avanti tutta») moltiplicato per la sensibilità del sonar e per la capacità d'ascolto
  dell'osservatore (che cala con la sua velocità), e gli eventi acustici in corso (ping, lanci, esplosioni);
- **ogni strato termico** fra le due quote divide il guadagno per due e il rumore per quattro;
- oltre 20 miglia non si sente niente. Un'Alfa ferma sentiva un 688 fermo davanti a sé fino a circa 17 miglia; con uno
  strato in mezzo, 4.

La capacità d'ascolto conta più del rumore del bersaglio: chi va piano sente. È la stessa lezione dell'Enhanced, con un
modello molto più semplice.

## 2.6 Il tempo e la scala

Gli autori scelsero di non usare le distanze vere (capitolo 1, §1.4). Il moto del gioco è **3,46 volte** quello vero: un
nodo mostrato vale 0,048 unità di gioco al secondo invece di 0,014. Senza quel fattore i tempi delle missioni (i 15
minuti di TORPEX, le corse dei siluri, i timer degli script) non tornano con le distanze. L'Enhanced, per restare fedele,
tiene quel moto nelle dieci missioni originali, in tutti e due i livelli: posate sulle carte vere, corrono da 2 a 4,3
volte più del vero secondo il teatro. Le missioni rapide, della libreria e dell'editor vanno invece a velocità vere
(volume 2, capitolo A2).

## 2.7 La protezione anticopia

Nella sala radio, il comando SEND CODE. Il gioco sceglie a caso una delle 89 domande: il nome di un battello e un
frammento di frase. Il giocatore cerca nel manuale il paragrafo segnato con quel battello e scrive le prime tre lettere
della parola che segue. Le domande stanno in un file chiamato `HELICOP.ENC`: un nome scelto, a quanto pare, per sviare
(interpretazione). Il progetto le ha verificate tutte e 89 su una copia del manuale.

## 2.8 I resti

- `TARGETS.LZW`, un elenco di 41 pagine coi nomi delle classi a telescrivente, non è caricato da nessun eseguibile: il
  residuo di una versione precedente.
- `DIALOG.CLS` contiene i messaggi di una versione dimostrativa.
- Due paragrafi dei briefing sono troncati nell'originale; nella traduzione sono completati al minimo.
