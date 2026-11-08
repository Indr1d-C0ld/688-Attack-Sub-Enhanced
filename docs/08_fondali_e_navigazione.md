# Fondali, scansione e navigazione: specifica di progetto

Stato: 23/10/2026, aggiornata con il dossier `docs/ricerca/06_fondali_e_navigazione.md` (fonti e gradi di
affidabilità; tabelle complete dei parametri nel suo §7). I valori segnati **stima** sono stime di gioco
del dossier, da tarare in prova.

## 1. Com'era nell'originale (1989) e cosa cambia

- **Contour Imaging Display** (solo 688): un "sonar a scansione laterale" ad alta frequenza disegnava in
  prospettiva il fondale attorno alla prua, a sinistra e a dritta (tasti P, F, S), con i contatti come
  ovali e senza alcuna penalità d'uso ("virtualmente non rilevabile").
- Mappa dall'alto con sfumature di profondità, HUD con quota, fondo sotto la chiglia e strati termici;
  incagli possibili, acque basse attraversabili solo in superficie.

**Correzione storica (dossier 06 §1.4).** Un 688 del 1989 non aveva sonar a scansione laterale né un'immagine
3D del fondale: quei sistemi esistevano solo su mezzi per missioni speciali (NR-1, Parche). A bordo c'erano:
- un ecoscandaglio verso il basso a fascio singolo;
- l'**AN/BQS-15**, sonar ad alta frequenza a corto raggio per ostacoli, mine e ghiaccio, con trasduttore
  nella vela e sondatori verso l'alto e verso il basso;
- la carta nautica;
- il navigatore inerziale.

Il display di contorno viene quindi **sostituito** da un quadro composito "carta + misure" (§4). La vista 3D
resta, ma mostra solo ciò che è stato davvero misurato.

## 2. Principi del rifacimento

1. **Il fondale vero è la batimetria reale**: heightmap dei teatri (EMODnet, ETOPO, celle da 200 m),
   eventualmente arricchita con rilievi procedurali più piccoli della cella (pinnacoli, monti minori)
   dove i dati sono grossolani.
2. **La carta non è il terreno.** Ogni parte naviga su una propria carta d'epoca, derivata dal fondale
   vero e degradata (§5). Le carte sono due, NATO e sovietica, con qualità diverse per teatro.
3. **Si vede solo ciò che si è sondato**, con l'incertezza resa visibile.
4. **Ogni emissione ha un prezzo.** Ecoscandaglio e BQS-15 sono sonar attivi intercettabili (§3).
5. **Anche la propria posizione è incerta**: l'inerziale deriva, e ogni punto nave costa esposizione (§6).
6. **Gli incagli si calcolano sulla posizione vera**, mai su quella stimata; gli avvisi dell'equipaggio
   nascono solo da misure (§7).

## 3. Sensori

Parametri completi nel dossier 06 §7.1. In sintesi (portate e intercettazione sono **stime**, perché
frequenza e potenza del BQS-15 sono classificate):

| Sensore | Geometria e portata | Uso | Intercettabile da |
|---|---|---|---|
| Ecoscandaglio | cono verticale di 30°, fino a 6000 m sotto la chiglia, ±1% | ping singolo su ordine o periodico (1–30 min, scelta del giocatore); 2% di falsi fondi (strato diffondente, bolle, scia) | 15–20 km a piena potenza; 5–8 km a bassa potenza (portata ≤ 2000 m) |
| BQS-15, canale di prua | ±60° in azimut, da −30° a +45° in elevazione, spazzata completa ogni 8–10 s | mine 1 km, chiglie di ghiaccio 2,5 km, pareti di roccia 3 km, degradati da velocità e riverberazione | 8–10 km nel fascio, 2–4 km fuori |
| BQS-15, sondatori su e giù | fascio di 10–15°, 1000 m, 1 Hz | distanza dalla superficie o dal ghiaccio e dal fondo | 2–4 km |
| BQQ-5 attivo (dossier 02) | sfera, settore di prua | eco di un rilievo come "contatto di fondo", senza profilo | decine di miglia |
| Periscopio | visivo | costa, ghiaccio, acqua scolorita (di giorno, su bassi fondali) | sezione radar dell'albero, vista |
| Radar di navigazione BPS-15 | in superficie o a quota periscopica | costa a 10–20 nm, punto nave radar | ESM nemico |

- **Livelli di emissione** comuni a giocatore e IA: SILENZIO, RIDOTTO, NAVIGAZIONE, SICUREZZA.
  Un ping intercettato dà al nemico un contatto con il rilevamento, e una distanza grezza se ascolta
  da più punti. Un brevetto della US Navy del 1959 per un "ecoscandaglio segreto" conferma il problema.
- **Velocità sicura**: con il BQS-15 che vede 1–3 km, in acque mal cartografate la velocità prudente è
  `portata / tempo di reazione`. Caso di taratura: USS San Francisco, 2005. A circa 33 nodi, con sondaggi
  ogni 15 minuti (circa 8 nm fra due misure) e una carta DMA del 1989 mai aggiornata, urtò un monte
  sottomarino che sulla carta non c'era.
- **688I** (in servizio a fine 1989: San Juan, Pasadena, Topeka): BQS-15 migliorato, +20% di portata sul
  ghiaccio, stima dello spessore del ghiaccio ±50%.
- **Lato sovietico** (dossier 06 §6): l'Alfa ha il sonar per mine MG-24 "Luč"; Victor e Akula hanno
  il sonar anti-ghiaccio e anti-mine MT-70/MG-70. Mancano dati pubblicati sulle prestazioni:
  si usano gli stessi modelli con le portate del BQS-15 ridotte (**stima**).

## 4. Quadro del fondale: carta + misure

Struttura dati e aggiornamento nel dossier 06 §7.2. Per ciascuna cella della griglia (100–250 m):
quota e incertezza dalla carta, quota misurata (fusione delle misure con un filtro di Kalman scalare),
istante e fonte della misura, presenza di ostacoli (pinnacolo, relitto, chiglia di ghiaccio) e tipo di fondo.

- **Ecoscandaglio**: proietta il cono sulla griglia nella posizione *stimata*. Nella sua impronta il
  fondo risulta "almeno a questa profondità"; lungo la rotta si forma un nastro di celle note.
- **BQS-15**: ogni ritorno aggiorna celle e ostacoli. L'assenza di eco entro la portata utile libera le
  celle ("niente fino a X m"), con una confidenza minore.
- **Il mosaico scorre**: ogni misura è archiviata con la posizione stimata del momento. Quando arriva
  un punto nave, la correzione si distribuisce sulle misure prese dall'ultimo punto, come in una SLAM
  semplificata, e il quadro misurato si sposta sulla carta. È visivamente efficace e insegna qualcosa.
- **Viste** (estetica VGA a 256 colori):
  - carta con isobate e annotazioni: "ED" (esistenza dubbia), "PA" (posizione approssimata),
    "acqua scolorita", anno del rilievo;
  - **vista 3D in prospettiva** del fondale, erede del display originale: rende solo la griglia fusa,
    con colore per la quota e trasparenza per la confidenza. Le zone non misurate sono tratteggiate o
    marcate "da carta";
  - sezione verticale di prua: profilo previsto dalla carta, ritorni del BQS-15, quota propria e margine
    di sicurezza;
  - sotto il ghiaccio, profilo della volta dai sondatori verso l'alto e polynye candidate.
- I contatti compaiono nel quadro (lettera, come gli ovali dell'originale) solo se un'eco attiva li ha
  davvero illuminati.
- **Deciso (23/10/2026)**: nessuna opzione "display classico". Il gioco ha solo il quadro realistico
  carta + misure.

## 5. Carta contro realtà

Procedura nel dossier 06 §7.4. Per ciascuna parte si genera una carta:
1. **Rilievo sintetico**: linee di rilievo virtuali con la spaziatura del teatro, campionate con un
   ecoscandaglio a cono, più qualche sondaggio sparso.
2. **Errore di posizione per campagna di rilievo**: spostamenti orizzontali correlati su blocchi di 20–50 nm.
3. **Errore di profondità**: scarto della velocità del suono di ±1–3% per campagna, più un rumore dello 0,5%.
4. **Ricostruzione per interpolazione**: i rilievi più stretti della spaziatura spariscono o si spostano,
   e le pendenze si ammorbidiscono.
5. **Annotazioni**: con una certa probabilità un rilievo mancante compare solo come nota su un'*altra*
   carta della stessa zona, come nel caso del San Francisco.
6. **Generalizzazione** delle isobate secondo la scala.

Qualità per teatro (dossier 06 §3.3, interamente **stima**), dal punto di vista NATO:

| Teatro | Spaziatura dei rilievi | Spostamento tipico |
|---|---|---|
| Faroe / GIUK | 1–2 km sulle soglie, 5–10 km nei bacini | 0,1–0,3 nm |
| Barents e approcci di Kola | 2–5 km | 0,3–1 nm |
| Gibilterra | 0,5–1 km | < 0,1 nm |
| Otranto | 0,5–2 km | 0,1–0,3 nm |
| Kunashir / Okhotsk | 3–10 km | 0,5–1 nm |
| Piattaforma Celtica | 1 km sulla piattaforma, 3–5 km sulla scarpata (canyon mal risolti) | 0,1–0,3 nm |
| Baltico | 0,2–1 km | < 0,1 nm a ovest, 0,3 nm a est |

Per la carta sovietica la tabella si inverte nelle acque di casa (Barents, Okhotsk, Baltico orientale:
carte ottime). Contesto storico: i sondaggi dei sottomarini USA e britannici fra il 1958 e il 1988 erano
classificati; le carte occidentali sbagliavano la posizione della Dorsale di Lomonosov, che le prime
carte sovietiche riportavano correttamente.

**Fatto (24/10/2026).** Le carte sono generate da `tools/pipeline/carta_epoca.py`, con i parametri in
`tools/pipeline/carte_epoca.yaml`. Vanno in `data/motore/teatri/<teatro>_carta_<nato|urss>.fdl` e si caricano come
`Fondale`; la missione le tiene con `Missione::imposta_fondale_carta`. Formato, algoritmo, esiti e prove sono in
`engine/README.md`, «Carte d'epoca e georeferenziazione». Rispetto all'elenco sopra:
- le zone «non rilevate» non vengono dalla fonte della batimetria (EMODnet in tutti i teatri europei, ETOPO in tutta
  Kunashir) ma dal rilievo sintetico: campagne mancanti e fascia costiera avversaria;
- le annotazioni (punto 5) stanno sulla carta stessa, in posizione approssimata;
- la generalizzazione (punto 6) è un passa-basso che tiene i bassi fondali; la scelta delle isobate per scala resta
  al disegno della carta.

Zone per teatro:
- Barents: Varanger norvegese e resto (approcci di Kola);
- Otranto: costa albanese, dove la NATO ha rilievi d'anteguerra e l'URSS quelli della base di Valona fino al 1961;
- Kunashir: acque giapponesi, Mare di Okhotsk, Curili meridionali sul lato del Pacifico;
- Baltico: acque del Patto di Varsavia e stretti danesi con il Baltico occidentale;
- Faroe, Gibilterra, Celtica: una zona ciascuno.

## 6. Navigazione e incertezza di posizione

Modello nel dossier 06 §7.3: posizione stimata con ellisse di covarianza. La posizione vera non viene mai
mostrata durante la missione, ma compare nel replay "verità".

- **Deriva dell'inerziale** (**stima**):
  - DMINS (AN/WSN-1), primi 688: 1,5 nm/giorno;
  - navigatore a giroscopi elettrostatici (AN/WSN-3), 688 con capacità Tomahawk: 0,5;
  - Medvedica (Victor III, Akula): 1,5–2;
  - Sož (Alfa) e Sigma (Victor I/II): 3.

  A questo si aggiunge l'oscillazione di Schuler (84 minuti, 0,15 nm); oltre 80°N la deriva raddoppia.
  Con l'inerziale guasto si naviga con solcometro e girobussola, con un errore di corrente di 0,2–1 nodi.
- **Punti nave** (precisione a 1σ e costo):

| Punto nave | Precisione | Costo ed esposizione |
|---|---|---|
| Transit | 0,05–0,1 nm, più 0,2 nm per nodo di errore di velocità | quota periscopica, antenna fuori per circa 2 minuti, attesa del passaggio (ogni 1–2 h alle medie latitudini), velocità ≤ 8 nodi |
| GPS | 0,01 nm | **assente per default** (primo Block II nel febbraio 1989); opzione "1990+" |
| Loran-C | 0,1–0,25 nm, solo dove c'è copertura | quota periscopica con albero, 2–5 minuti |
| Omega | 2–4 nm | antenna galleggiante, senza salire; ambiguità di corsia se l'incertezza supera 4 nm |
| Astronomico | 1–3 nm | periscopio per 5–10 minuti, cielo sereno, crepuscolo o notte (legato a meteo e ciclo giorno/notte) |
| Radar sulla costa | 0,05–0,2 nm | emissione radar (ESM nemico), entro 20 nm dalla costa |
| Visivo su punti cospicui | 0,05–0,1 nm | periscopio, costa in vista |
| Batimetrico | il maggiore fra ¼ della spaziatura delle isobate e 100 yarde, **più l'errore della carta in quel punto** | serie di sondaggi (emissioni) su rotta costante, pendenze di 1–4° (Bowditch cap. 26) |

- Il punto nave batimetrico usa la carta degradata e ne eredita gli errori: un equipaggio può risultare
  "sicuro di sé e sbagliato" senza alcuna regola apposita.
- Lato sovietico: RSDN-20 "Alfa" (VLF, 2,5–7 km), Čajka (l'equivalente del Loran-C), satelliti Parus
  con prestazioni simili al Transit (**stima**).
- Negli stretti (Gibilterra, Kunashir) il rischio principale è l'errore di posizione più che la carta,
  e serve una cadenza più alta di punti nave.

## 7. Incagli, fondo e ghiaccio

Regole nel dossier 06 §7.5:
- **Urto**: il volume del battello (scafo e vela) interseca la batimetria vera o un ostacolo. Il danno è
  proporzionale all'energia sulla componente normale (**stima**):
  - sotto 3 nodi: graffio;
  - 3–10 nodi: duomo sonar e una cassa zavorra prodiera;
  - 10–20 nodi: sfera sonar persa, feriti;
  - oltre 20 nodi: scenario San Francisco, con lo scafo resistente integro salvo urti frontali su roccia
    oltre i 30 nodi.

  Si integra con il modello dei danni (roadmap, punto 17).
- **Strisciamento**: timoni ed elica. Per gli SSN entrano sedimenti nelle prese del condensatore,
  con perdita di potenza progressiva.
- **Posarsi sul fondo**: solo per i convenzionali (Kilo, Foxtrot, Whiskey). Per gli SSN aumenta il rischio
  sulle prese a mare.
- **Schiera rimorchiata** contro il fondo: docs/07 §3.3.
- **Ghiaccio** (solo nella variante settentrionale del Barents: la costa di Kola è libera dai ghiacci e il
  bordo invernale sta a 74–77°N):
  - chiglie di 3,8 m in media, fino a circa 20 m; iceberg con pescaggio di 30–100 m (**stima**);
  - il sonar verso l'alto non distingue bene il ghiaccio sottile e liscio dall'acqua libera;
  - il sonar attivo ad alta potenza è quasi inutile sotto la banchisa;
  - rumore molto basso sotto la banchisa compatta, forte e impulsivo al margine.

  Emersione nel ghiaccio solo per il **688I** (vela rinforzata, timoni di prua retrattili): i 688
  Flight I/II hanno i timoni sulla vela e non possono farlo.
- Gli **avvisi dell'equipaggio** ("fondo in salita", "ostacolo a prua") nascono solo da misure recenti,
  mai dal terreno vero.

## 8. Tattica e IA

- **Vicino al fondo**: il battello si confonde con la riverberazione del fondo contro i sonar attivi.
  Il rilievo fa ombra fra il battello e il sensore. Canyon e fosse offrono acqua più fredda e cammini
  acustici diversi. Il fondo duro riflette e riverbera; quello molle assorbe, ma rende l'ecoscandaglio
  meno affidabile.
- **IA NATO**: nelle zone con carta scarsa limita la velocità e accetta di emettere. Nei bastioni sovietici
  preferisce sondaggi rari a bassa potenza, punti nave batimetrici e salite pianificate per il Transit.
- **IA sovietica**: con carte buone in casa naviga veloce e vicina al fondo. I convenzionali si posano
  sul fondo in acque basse.
- **Rifugi**: celle con alta riverberazione e un rilievo fra sé e la minaccia, per l'IA come per il giocatore.
- **Prudenza nelle emissioni** come parametro di difficoltà dell'IA.

## 9. Statistiche e achievement

Statistiche: tempo vicino al fondo, distanza minima dal fondo, ping emessi e intercettati, punti nave presi
e loro errore reale (noto solo nel debriefing), incagli e loro cause, celle sondate.

Esempi di achievement:
- attraversare uno stretto in immersione senza punti nave;
- correggere la posizione con la sola navigazione batimetrica;
- seminare un inseguitore in un canyon;
- concludere una missione nel Barents senza alcuna emissione attiva.

## 10. Lacune

Dossier 06 §7.7:
- frequenza, potenza, settore e portata del BQS-15;
- designazione e modalità dell'ecoscandaglio dei 688 nel 1989 (AN/BQN-17?);
- deriva reale di DMINS e WSN-3, e date d'installazione;
- prestazioni dei sistemi sovietici;
- statistiche degli errori delle carte;
- dottrina sul margine sotto la chiglia.

Fonti da cercare offline: Friedman (*Naval Institute Guide to World Naval Weapons Systems*), Polmar &
Moore (*Cold War Submarines*), Cohen (*Bathymetric Navigation and Charting*, 1970), deepstorm.ru.
