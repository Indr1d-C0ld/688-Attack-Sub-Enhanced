# 10 — Danni, affondamento ed esplosioni (progetto)

Stato: **in corso** (24/10/2026). Decisa la resa grafica delle navi danneggiate (§5.1); fatte le fasi D2 (modello
3D nel motore), D3 (fisica dei danni delle navi di superficie), D4 (effetti visivi e deformazioni, §5.8), D5
(danni nelle missioni, §7.1) e D6 (sottomarini colpiti, §10, con il battello emerso sulla fisica delle navi). Sviluppa i punti 11 (danni e riparazioni) e 17 (effetti e danni
visivi) della roadmap. I numeri vengono da `docs/ricerca/02_acustica_oceanografia_dottrina.md` §9 e dal dossier
`docs/ricerca/07_danni_affondamento_esplosioni.md`, con livelli di fiducia e fonti.

## 0. Principi

1. **Una sola fisica, tre uscite.** La stessa simulazione decide:
   - la **meccanica**: falle, compartimenti, sistemi che non funzionano più, velocità e manovra ridotte, reazioni
     dell'IA;
   - l'**immagine**: la nave che sbanda, si apprua, si spezza, brucia, si capovolge e affonda;
   - il **suono**: esplosione, pulsazioni della bolla, rumori di rottura.

   Quello che si vede è sempre quello che è successo nella simulazione.
2. **Niente animazioni a copione.** Il modo di affondare non si sceglie da una lista: emerge da dove e come la nave
   è stata colpita, da com'è fatta e da cosa trasporta. Il caso conta solo dove conta nella realtà:
   - punto d'impatto;
   - paratie e porte che cedono;
   - esplosioni secondarie;
   - dettagli visivi (forma del fumo, detriti).
3. **Determinismo.** Tutto il caso viene dal seme della missione: stessa partita, stesso affondamento. Serve per
   replay, statistiche e codici missione.
4. **Realismo del 1989.** Armi, spolette, esplosivi e dottrina di controllo dei danni dell'epoca. Ogni parametro
   ha una fonte o è dichiarato come stima di gioco.
5. **Leggibilità.** Gli effetti sono veri anche a pochi pixel, a 10 km di distanza e a basso ingrandimento, e
   restano nello stile VGA: 256 colori, retinatura e animazioni a palette.

## 1. La catena di un colpo

```
arma ─► spoletta ─► punto di scoppio ─► carichi ─► danni locali ─► risposta della nave ─► stato ─► effetti
        contatto     (x, y, z rispetto    onda d'urto  falle          allagamento         assetto    immagine
        influenza     allo scafo)         bolla        compartimenti  incendi             stabilità  suono
        prossimità                        getto        sistemi        controllo danni     trave      messaggi
        ritardo                           schegge      struttura                          esito      IA
                                          fuoco
```

### 1.1 Armi e spolette

Le armi entrano nel motore come dati, insieme alle armi delle missioni che mancano: Mk 48, Type 53, Spearfish,
Sea Lance, Type 40.

| Arma | Carica | Spoletta e punto di scoppio tipico | Effetto dominante |
|---|---|---|---|
| Siluro pesante a influenza (Mk 48, Tigerfish, TEST-71) | ≈ 300 kg (Mk 48: 295 kg di PBXN-103 ≈ 544 kg di TNT per la bolla) | sotto la chiglia, verso mezzanave (la testa attiva mira al centro dell'eco) | bolla e getto: la chiglia si solleva e poi si spezza; poche falle, struttura distrutta |
| Siluro pesante a contatto | ≈ 300 kg | sulla murata, 3–6 m sotto il galleggiamento | grande falla, 1–3 compartimenti allagati, shock |
| Siluro segui-scia (53-65K, 65-76) | 300–560 kg | da poppa, lungo la scia | eliche, timoni e sale macchine: nave ferma e appoppata |
| Siluro leggero (Mk 46, AT-1) | 44–70 kg | contro sottomarini; su una nave fa poco | falla piccola, shock locale |
| Missile antinave (Harpoon, TASM, P-700, P-70) | 221–1.000 kg | dentro lo scafo sopra il galleggiamento (spoletta ritardata) | incendio con il combustibile residuo, sovrastrutture e sistemi |
| Bomba di profondità, razzo RBU | 25–200 kg | a quota preimpostata o a contatto | shock contro i sottomarini |
| Esplosione dei depositi (secondaria) | tonnellate | interna | catastrofica |

Il **punto d'impatto** si estrae (con il seme) da una distribuzione che dipende dalla guida:
- segui-scia: verso poppa;
- testa attiva: verso il centro della sagoma acustica;
- missile: verso il centro della sagoma radar, quindi verso la sovrastruttura.

Su questo punto la pagina della nave decide quali compartimenti, sistemi e parti del modello vengono colpiti.

### 1.2 Carichi

Dal dossier 07 (§1 e formule in §11.1):
- **Onda d'urto:** similitudine di Cole, `P = 52,4·(W^(1/3)/R)^1,13` MPa.
- **Bolla:** raggio massimo, periodo di Willis `T₁ = 2,11·W^(1/3)/(d+10)^(5/6)` s, migrazione verso l'alto e
  pulsazioni successive.
- **Getto** della bolla che collassa vicino allo scafo.
- **Schegge e fuoco** per i missili.

Per la bolla si usa il TNT equivalente per la bolla: Mk 48 ≈ 1,84 volte la massa, Torpex e HBX ≈ 1,5. Contro i sottomarini si
usa il fattore d'urto dello scafo (`HSF`, `KSF`, dossier 02 §9.4). Lo scafo è più fragile in profondità.

## 2. Il modello fisico della nave (nucleo, `nucleo/danni`)

### 2.1 Dati di classe nuovi

Si aggiungono a `classi.toml`, con fonte e affidabilità come gli altri campi:
- **pesi e forma:** dislocamento a pieno carico, pescaggio (c'è già), altezza del ponte, GM intatto;
- **compartimentazione:** posizioni delle paratie stagne. Vengono da piani pubblici, oppure da regole per tipo,
  per esempio la regola dei due compartimenti per le navi militari e uno per i mercantili;
- **trave di scafo:** resistenza a flessione stimata con le formule di regolamento, ridotta dove lo scafo è
  danneggiato;
- **locali critici:**
  - depositi munizioni (e cosa contengono: missili, proiettili, siluri);
  - cisterne di combustibile;
  - sale macchine e caldaie;
  - hangar con elicottero e carburante avio;
- **propulsione:** turbine a gas, vapore, nucleare o diesel. Conta per l'immagine (vapore bianco dalle caldaie
  allagate) e per il rumore;
- **carico:** petroliera carica o in zavorra (vapori nelle cisterne), portacontainer, nave rifornimento con
  munizioni e combustibile;
- **controllo danni:** qualità di equipaggio e dottrina (USN, RN, sovietica, mercantile). Cambia velocità e
  riuscita delle riparazioni.

### 2.2 Idrostatica per stazioni

- **Stazioni:** lo scafo del kit si divide in circa 40 stazioni lungo la lunghezza. Per ogni assetto
  (immersione, sbandamento, assetto longitudinale) si calcolano la sezione immersa di ogni stazione, la spinta e
  il suo centro.
- **Opera viva:** lo scafo del kit oggi arriva solo al galleggiamento. Va esteso sotto l'acqua (§5.1), e serve
  anche per vedere la carena quando la nave sbanda.
- **Pesi:** la nave più l'acqua imbarcata in ogni compartimento, con l'effetto delle **superfici libere** (l'acqua
  che si sposta nel compartimento allagato riduce la stabilità).
- **Dinamica:** tre gradi di libertà (sussulto, rollio, beccheggio), con masse aggiunte e smorzamento. La nave non
  salta da un assetto all'altro: si inclina gradualmente, oscilla e, se la stabilità residua si annulla, si
  capovolge.

### 2.3 Allagamento

- **Portata:** per ogni falla, dalla formula dell'efflusso (dossier 02 §9.1) con l'altezza d'acqua sopra il foro
  nell'assetto del momento. Una falla che emerge smette di imbarcare; una che affonda imbarca di più.
- **Allagamento progressivo**, verso i compartimenti vicini:
  - paratie danneggiate;
  - porte stagne aperte (con una probabilità che dipende dallo stato di allerta);
  - condotte di ventilazione;
  - ponte che va sott'acqua (*downflooding*).
- **Controllo danni:** tamponature e pompe riducono la portata con tempi realistici e con priorità
  (dossier 02 §9.2).

### 2.4 Trave di scafo e rottura

- **Momento flettente:** lungo la nave si integra la differenza fra pesi e spinta. Lo spezzamento avviene quando
  il momento supera la resistenza residua.
- **Le tre cause tipiche:**
  - la bolla sotto la chiglia: prima solleva la nave al centro, poi la lascia cadere nel vuoto della bolla
    (*sagging* e *hogging* in rapida successione);
  - la poppa o la prua che si alzano durante l'affondamento, come il Titanic;
  - lo scafo indebolito dai danni.
- **Quasi statico più whipping** (dossier 07 §3.3–3.4):
  - Contro corvette e fregate (frequenza a 2 nodi di 1,5–2,4 Hz) la bolla di un siluro pesante (periodo 1–1,5 s)
    spezza la trave soprattutto con il sollevamento, la caduta nel vuoto della bolla e il getto, più che per
    risonanza.
  - La risonanza (*whipping*) conta di più con le navi grandi o con cariche più piccole e profonde: il dossier
    propone un moltiplicatore di severità che cresce quando periodo della bolla e periodo della trave si avvicinano.
  - **Taratura sui casi:** carica sotto la chiglia entro ±0,25 L dal centro.
    - Sotto 130 m di lunghezza la nave si spezza con 150 kg di TNT per la bolla: Cheonan 88 m, Torrens 112 m,
      Ingraham 138 m.
    - Fra 130 e 200 m è probabile con 400 kg.
    - Oltre 200 m o 20.000 t è improbabile.
    - Fuori dal tratto centrale la carica asporta la prua o la poppa invece di spezzare la nave.
  - In fonte aperta non esistono soglie di rottura delle navi da guerra: è una calibrazione, dichiarata come tale.
- **Dopo la rottura:** due corpi rigidi, ognuno con le sue stazioni, i suoi compartimenti e la sua acqua. Ogni
  troncone galleggia, si inclina e affonda per conto proprio: uno può impennarsi e scendere in verticale, l'altro
  può restare a galla capovolto per ore.

### 2.5 Incendi ed esplosioni secondarie

- **Focolai:** per compartimento, con crescita, propagazione, temperatura, fumo e lotta del controllo danni.
- **Combustibili:**
  - il carburante residuo di un missile o il JP-5 dell'hangar dà fumo nero e denso;
  - gasolio e nafta danno fumo nero;
  - le vernici danno fumo grigio;
  - le caldaie e le tubolature del vapore danno vapore bianco.
- **Munizioni:** l'esplosione dei depositi (*cook-off*) ha una probabilità che cresce con temperatura e tempo.
  Se avviene è catastrofica: la nave si apre e affonda in pochi minuti.
- **Petroliere** (dossier 07 §5):
  - **carica:** difficilissima da affondare, perché il greggio è più leggero dell'acqua. Nella guerra delle
    petroliere (1980–88) circa 451 attacchi affondarono 11 navi e ne resero irrecuperabili circa altre 36.
    - Brucia, perde greggio in mare (chiazza che brucia a 3–4 mm/min) e può restare a galla per giorni.
  - **in zavorra, senza gas inerte:** i vapori nelle cisterne possono esplodere (stima del 20–40% per colpo nella
    zona del carico).

### 2.6 Sistemi e parti

- **Posizione:** ogni parte del modello del kit (radar, lanciatori, torri, alberi, elicottero) e ogni sistema
  interno (motori, timone, sonar, generatori) ha una posizione.
- **Danni graduali:** shock e schegge **degradano** le prestazioni invece di spegnere tutto: radar con portata
  ridotta, una sola linea d'assi, timone bloccato.
- **Conseguenze:** il danno a una parte toglie ciò che la parte fa, quindi cambiano le capacità dell'IA e il
  rumore irradiato (falle, macchinari danneggiati).

### 2.7 Stati ed esito

| Stato | Quando | Effetti di gioco |
|---|---|---|
| danneggiata | danni senza perdita di capacità essenziali | combatte, l'IA può ritirarsi |
| menomata (*mission kill*) | armi, sensori o propulsione principali fuori uso | non combatte più; conta come colpita negli script |
| immobilizzata | nessuna propulsione o timone | alla deriva |
| condannata | la simulazione prevede la perdita: spinta o stabilità residue insufficienti, incendio senza controllo | abbandono nave (zattere), richiesta di soccorso via radio |
| in affondamento (modo) | assetto oltre i limiti, spinta in esaurimento | animazione finale |
| affondata | sotto la superficie | relitto: rumori di implosione, chiazza, detriti |

Per stimare se una nave è **condannata**, si fa avanzare la simulazione di qualche minuto senza controllo dei danni.

## 3. Catalogo dei modi di affondare (emergenti)

Nessuno di questi modi è scritto a mano: sono gli esiti che il modello deve saper produrre. I tempi e i casi storici
sono nel dossier 07 (§7, tabella dei casi).

| Modo | Cosa lo causa nella simulazione | Come appare | Tempi tipici e casi |
|---|---|---|---|
| **In assetto, lento** | allagamento distribuito e simmetrico, compartimenti che cedono uno dopo l'altro | la nave si abbassa, la coperta va sott'acqua, sbuffi d'aria dagli osteriggi, scompare quasi orizzontale | da 30 min a giorni |
| **Di prua o di poppa** | falla a un'estremità, allagamento longitudinale | appruamento crescente, eliche e timone fuori dall'acqua, l'altra estremità a 30–90°, scivolata finale rapida | 10 min – ore, scivolata 30 s – 2 min (Gustloff, Indianapolis 12 min) |
| **Capovolgimento** | allagamento asimmetrico, superfici libere, stabilità che si annulla | sbandamento a scatti (5°, 15°, 45°), pausa, rotazione rapida con l'opera viva in vista; lo scafo rovesciato galleggia, poi affonda | 3–20 min (Royal Oak 13 min, Coventry < 20, Barham ~4) |
| **Spezzata in due** | bolla sotto la chiglia o trave sovraccarica | lampo, colonna d'acqua, nave a V; la poppa con le macchine affonda per prima, la prua resta in verticale o capovolta | poppa 0,3–5 min, prua 2 min – 2 h (Cheonan, Torrens, Racine, Ingraham) |
| **Esplosione dei depositi** | munizioni colpite o *cook-off* | fiammata verticale, fungo di fumo, frammenti, la nave scompare | 3 s – 4 min (Juneau 20 s, Hood ~3 min) |
| **Brucia e resta a galla** | incendio senza perdita di spinta (petroliere cariche, navi grandi) | fumo nero alto, poi relitto annerito alla deriva; spesso affonda sotto rimorchio | ore – 6 giorni (Sheffield, Atlantic Conveyor, petroliere) |
| **Prua (o poppa) asportata** | esplosione a un'estremità con le paratie vicine che reggono | moncone con la paratia esposta, nave appruata che galleggia e può salvarsi | salva (Samuel B. Roberts, Tripoli; Belgrano al primo colpo) |
| **Disintegrazione** | carica grande contro una nave piccola (Nanuchka, 600 t) | la nave scompare nella colonna d'acqua; restano detriti | secondi |
| **Nave grande che non affonda** | portaerei, Iowa: un siluro non basta | danni, sbandamento corretto con controallagamento; servono molti colpi | portaerei con 3–4 siluri: 4–8 h (Shinano 7 h 42 min) |

Il dossier 07 §7.3 ha 21 casi storici completi (dislocamento, arma, punto d'impatto, modo, tempo): diventano le prove
di taratura della fase D3.

## 4. Il nostro sottomarino e i sottomarini nemici

Progetto della realizzazione (fase D6), con i confini fra moduli e le decisioni: §10.

- **Colpi vicini:** l'onda d'urto (`KSF`) guasta i sistemi, come l'originale (§9.4 di
  `docs/originale/sensori_e_navigazione.md`: sonar, schiera, periscopio, motori, antenna) e come il dossier 02 §9.4.
  - Le luci tremolano (palette), la vista sobbalza, l'equipaggio riferisce i danni.
  - Una falla imbarca con una portata che dipende dalla quota.
  - Sotto la quota di collasso lo scafo implode: Thresher a circa 730 m in un decimo di secondo, Scorpion a circa
    470 m (dossier 07 §8).
  - Resta l'emersione d'emergenza.
- **Sottomarini nemici colpiti:**
  - **sonar:** rumori di rottura e compartimenti che implodono mentre il battello affonda;
  - **superficie:** bolle, gasolio, detriti; il periscopio li vede se si è vicini;
  - **emersione:** un battello danneggiato può emergere d'emergenza.
- **Detonazione volontaria del nostro siluro: non esisteva nella realtà del 1989** (fiducia media: nessuna fonte
  aperta la cita per i siluri convenzionali, e tutte vanno nella stessa direzione).
  - Il dossier 07 §10 la trova solo sul Mk 45 ASTOR, il siluro nucleare che esplodeva unicamente per comando via
    filo.
  - Per il Mk 48 il filo serve a guida, cambio di quota e velocità, abilitazione della ricerca e **spegnimento**.
    Un ex sommergibilista scrive che un siluro non si può far esplodere a comando: la scena di *Caccia a Ottobre
    Rosso* è un errore cinematografico.
  - Nessuna fonte per Tigerfish e TEST-71.
  - **Il gioco del 1989 invece l'aveva:** pulsante DETONATE della stazione armi, gestore `guida_e0` degli script,
    che toglie il siluro con il messaggio 19 senza fare danni. Il manuale dice invece che danneggia
    (`docs/11_comandi_e_stazioni.md` §1.3). Il livello classico la mantiene com'era.
  - **Enhanced (deciso il 24/10/2026):** via filo solo i comandi reali, spegnimento compreso. La detonazione è
    eliminata, nemmeno come opzione (decisione 4). In ogni caso, quando un siluro esplode vicino alla superficie e nel campo del periscopio,
    si vede (§5.5).

## 5. Immagine

### 5.1 Decisione chiave: il modello 3D nel motore (decisa il 23/10/2026)

**Decisione:** le navi intatte restano con gli sprite di oggi; per le navi danneggiate gli sprite si generano al volo
dal modello del kit, con lo stesso aspetto (foto proiettate, 16 grigi, retinatura).

Gli sprite precalcolati (74 viste per classe) non bastano:
- Una nave che sbanda, si apprua, si spezza e si capovolge richiederebbe migliaia di viste per classe e per
  scenario.
- Le 74 viste delle navi grandi occupano già 9–15 MB per classe. Una griglia di 7 sbandamenti × 7 appruamenti le
  moltiplica per 49, fino a 400–750 MB per classe.
- Le trasformazioni 2D di uno sprite sono esatte solo in tre casi: affondamento in assetto, appruamento visto di
  fianco, sbandamento visto di prua o di poppa. Nelle viste oblique reggono fino a 10–15°.
- Oltre quegli angoli lo sprite non contiene ciò che diventa visibile: coperta vista dall'alto, carena, eliche,
  timoni.

Le trasformazioni 2D restano comunque utili per gli assetti piccoli. Lo schema:
- **Navi intatte:** gli sprite di oggi, che restano la resa migliore.
- **Navi danneggiate o in affondamento:** il motore disegna il **modello 3D del kit** con un piccolo rasterizzatore
  software (z-buffer, 2.000–10.000 triangoli per nave: pochissimi per una CPU moderna).
  - Il colore di ogni triangolo viene «cotto» dalla stessa proiezione delle foto che produce gli sprite e
    esportato dal kit in una texture.
  - Il risultato passa nella stessa rampa di 16 grigi, con la stessa retinatura di Bayer e la stessa luce del Sole.
- **Prova di coerenza automatica:** a nave intatta, il disegno del modello e lo sprite devono coincidere quasi
  pixel per pixel, così il passaggio dall'uno all'altro non si vede.
- **Lavoro sul kit:**
  - **opera viva:** carena sotto il galleggiamento, chiglia, eliche, timoni, cupola del sonar, colore
    antivegetativo;
  - **esportazione:** mesh, texture cotta, parti nominate e punti d'aggancio per fuoco e fumo (camini, hangar,
    depositi).

### 5.2 Deformazioni del modello

- **Parti asportate:** alberi che cadono (con una fisica semplice), torri e lanciatori che saltano, l'elicottero che
  scivola fuori dall'hangar.
- **Falle:** macchie nere con il bordo frastagliato, nel punto d'impatto della simulazione.
- **Annerimento da incendio:** la texture vicino ai focolai scende verso i grigi scuri.
- **Rottura:** un piano di taglio con il bordo frastagliato procedurale divide la mesh in due tronconi, ognuno con
  la sua posizione e il suo assetto.

### 5.3 Acqua attorno alla nave

- **Galleggiamento sul modello inclinato:** la parte immersa non si disegna; sulla linea d'acqua compare un orlo di
  schiuma, più marcato dove la nave si muove o dove entra acqua.
- **Spruzzi:** quando un troncone ricade o un albero colpisce l'acqua.
- **Dopo l'affondamento:**
  - ribollio d'aria;
  - un vortice di schiuma che si allarga e si spegne;
  - una chiazza di nafta: una macchia più scura e piatta che il mare deforma;
  - detriti e zattere che vanno alla deriva col vento.

### 5.4 Fuoco, fumo ed esplosioni nell'aria

Sono particelle e *billboard* nel mondo 3D, non sullo schermo, quindi seguono distanza, orizzonte e ingrandimento
come le navi.
- **Fuoco:** una rampa di palette **emissiva**. Di notte non si scurisce con la luce ambiente (resta fuori da
  `INDICI_VISTA`) e illumina dal basso il fumo che la sovrasta. La fiamma si anima ciclando la palette, nello stile
  VGA.
- **Fumo:** nero (gasolio, carburante dei missili), grigio (vernici, materiali), bianco (vapore).
  - Sale, si allarga e si piega col vento del teatro.
  - Una nave che brucia lascia una colonna inclinata visibile oltre l'orizzonte.
- **Esplosione di un missile o dei depositi:**
  - un lampo di un fotogramma (palette più chiara, soprattutto di notte);
  - palla di fuoco;
  - schegge e detriti in traiettorie balistiche;
  - colonna di fumo.

### 5.5 Esplosioni subacquee viste dalla superficie

La sequenza nel tempo, le dimensioni e la durata vengono dalla fisica (dossier 07 §2) in funzione di carica e
profondità.
1. **Slick:** un anello scuro e velocissimo, che è l'onda d'urto sulla superficie.
2. **Cupola di spruzzi:** si alza e si sfrangia.
3. **Pennacchi:** colonne che escono quando la bolla raggiunge la superficie. Non ci sono se la carica è troppo
   profonda: allora restano solo la cupola e un rigonfiamento.
4. **Ricaduta e onda di base:** l'acqua ricade e un'onda di base si allarga.
5. **Schiuma:** una chiazza bianca che dura minuti.
6. **Pulsazioni successive:** cupole più piccole.

Il seme varia numero, forma e inclinazione dei pennacchi, così nessuna esplosione è identica a un'altra.

**Il regime dipende dalla profondità relativa al raggio massimo della bolla** (`d/A_max`, dossier 07 §2.1):

| d/A_max | Che cosa si vede |
|---|---|
| < 1 | la bolla sfiata: colonna cava con in cima una nube scura di gas e acqua, nessuna pulsazione |
| 1–3 | cupola, poi, all'istante del primo impulso della bolla, pennacchi bianchi che la bucano |
| 3–5 | cupola bassa e una chiazza |
| > 5 | solo una gobba e una chiazza di acqua schiumosa |

**Ordini di grandezza** (dossier 07 §2.6 e §11.2, fiducia bassa: le misure reali variano di ±50%):

| Caso | Pennacchio: altezza × diametro | Comparsa del pennacchio | Visibile per |
|---|---|---|---|
| Mk 48 sotto la chiglia (8–12 m) | 160–215 × 180–210 m | 1,3–1,5 s | 20–40 s |
| Siluro sovietico o Tigerfish | 110–180 × 150–200 m | 1,0–1,3 s | 20–35 s |
| Bomba di profondità a 15 m | 85–115 × 120–170 m | 0,7–0,9 s | 10–25 s |
| Bomba di profondità a 30 m | 30–60 × 30–80 m | 0,5–0,6 s | 5–15 s |
| Bomba di profondità a 50–100 m | gobba di 2–5 m, chiazza di 30–60 m | — | chiazza 30–120 s |
| Razzo RBU a 10 m / oltre 50 m | ≈ 50 × 70 m / quasi invisibile | 0,5 s | 5–10 s |
| Mina da fondo in 20 m d'acqua | 145–180 × 210–250 m, acqua marrone di fango | ≈ 1 s | chiazza marrone per minuti |

- **Slick:** l'anello scuro corre a `r = sqrt((1500·t)² − d²)` e dura meno di 0,3 s, un lampo.
- **Verifica:** il pennacchio dell'ex-HMAS *Torrens* (Mk 48, 1999) è riportato di circa 150 m, coerente con le
  tabelle.

**Casi da prevedere:**
- siluro sotto la chiglia: la nave sollevata nella cupola, poi la rottura;
- bombe di profondità e salve RBU: gruppi di colonne quasi contemporanee;
- siluro contro un'esca;
- il nostro siluro fatto esplodere;
- mine.

### 5.6 Visibilità

Gli effetti passano dalla stessa proiezione del periscopio delle navi:
- **orizzonte:** un pennacchio di 150–200 m si vede anche quando la nave colpita è già oltre l'orizzonte;
- **ingrandimento e luce;**
- **mare:** le onde nascondono la base;
- **notte:** si vedono solo fuochi e lampi;
- **foschia.**

Dimensione in pixel di un oggetto alto 100 m (32° o 8° su 640 pixel):

| Distanza | 1,5× (0,05° per pixel) | 6× (0,0125° per pixel) |
|---|---|---|
| 2 km | 57 px | 229 px |
| 5 km | 23 px | 92 px |
| 10 km | 11 px | 46 px |
| 20 km | 6 px | 23 px |

Gli effetti si disegnano quindi a più livelli di dettaglio, da una manciata di pixel retinati a sequenze con
centinaia di particelle.

### 5.7 Palette

- **Rampe già presenti:** fuoco (175–178 e 89–95, dal rosso al giallo), grigi (80–88), bianchi azzurrati
  (160–167).
- **Posti liberi:** 190, 192–194 e 196–198, oggi segnaposto magenta.
- **Proposta:**
  - una rampa di **fuoco emissivo**, con 8 posti presi da quelli liberi e dalle rampe esistenti;
  - una rampa di **fumo** da 8 posti, dal nero al grigio caldo;
  - spruzzi e schiuma con i grigi e i bianchi già presenti.
- **Esclusioni:** la rampa del fuoco va tolta da quelle che la luce ambiente scurisce.

### 5.8 Realizzazione (fase D4)

- **Fatto:** §5.3 (orlo di schiuma, spruzzi, ribollio, vortice, nafta, rottami), §5.4 (fiamme, fumo, lampo, palla
  di fuoco, schegge, fungo dei depositi), §5.5 (tutta la sequenza e i regimi, con i numeri del dossier), §5.6
  (stessa proiezione delle navi, orizzonte, notte) e §5.7 (palette).
- **Diverso dal progetto:**
  - La fiamma tremola con un hash per particella e per dodicesimo di secondo, non ciclando la palette: l'effetto
    è lo stesso e la palette resta libera per il lampo.
  - La cupola è limitata a metà del pennacchio. Per le cariche a pochi metri la formula di Kolsky darebbe
    centinaia di metri, mentre lì domina la colonna.
- **Aggiunto dopo (24/10/2026):**
  - §5.2: falle nere, fuliggine, alberi che cadono, con lo spruzzo quando finiscono fuori bordo;
  - onde che nascondono la base e foschia (§5.6);
  - nave sollevata dalla bolla e inarcata, poi insellata (dossier 07 §3.3).
- **Rinviato:**
  - parti asportate che volano via (torri, lanciatori);
  - elicottero che scivola fuori dall'hangar;
  - alberi che scivolano in mare.

## 6. Suono

Rimanda al punto della roadmap sulle firme acustiche:
- esplosione e pulsazioni della bolla, diverse per carica e profondità;
- rumori di rottura e compartimenti che implodono;
- sibilo del vapore;
- eliche che si fermano.

Al sonar un colpo andato a segno si **sente** prima di vederlo, e spesso si sente soltanto.

## 7. Livello classico e missioni

- **Gli script restano fedeli.** Nell'originale una nave distrutta sparisce dopo 18 visite (§9.4 del documento
  sull'originale) e gli script controllano `unita_colpita` e `unita_esiste`. Il livello classico mantiene la
  stessa logica: la nave conta come distrutta quando lo dice la regola dell'originale.
- **L'immagine resta realistica.** Il motore tiene la nave nel mondo come **relitto in affondamento**: visibile al
  periscopio, udibile al sonar e ignorato dagli script. Così le missioni restano identiche nei tempi, ma quello
  che si vede è realistico.
- **Versione Enhanced:** decide la simulazione (§2.7). «Menomata» e «condannata» sono gli eventi che le missioni
  nuove possono usare.

### 7.1 Realizzazione (fase D5, 24/10/2026)

- **La regola d'impatto dell'originale** (`docs/originale/sensori_e_navigazione.md` §9.8), ricavata dal
  disassemblato, e' nel runtime delle missioni. Prima i siluri non colpivano mai. Insieme sono arrivate le
  correzioni a `ATTACK`, `ATTACK_RND`, `SELECT_LINKED`, all'opcode 37h e all'inseguimento in quota.
- **Livello classico:** decide l'integrita' dell'originale; l'immagine e' la fisica. La nave distrutta diventa un
  relitto staccato dagli script che affonda davvero: l'equipaggio la abbandona e, se serve, falle distribuite sul
  fondo.
- **Impatti** (24/10/2026): nel livello classico solo nelle coppie dei sensori, come l'originale; nell'Enhanced su ogni
  coppia a ogni passo.
- **Enhanced** (`--regola simulazione`): decide la fisica, oppure l'integrita' esaurita con la nave menomata.
  Reazioni dell'IA: fermata, velocita' ridotta, ritirata, soccorso delle amiche.
- **Messaggi del motore** (200+): cio' che sonar e radio sentono entro 20 nm.
- **Dettagli:** `engine/README.md`, «Danni nelle missioni».

## 8. Piano di lavoro

Ogni fase si chiude con prove automatiche e istantanee.

| Fase | Contenuto | Verifica |
|---|---|---|
| D0 | Dossier quantitativo `docs/ricerca/07` (fatto il 23/10/2026; lacune nel suo §13) | fonti e livelli di fiducia |
| D1 | Dati: armi e spolette, dati di classe del §2.1 | campi completi per le 35 classi |
| D2 | Kit: opera viva, esportazione di mesh e texture cotta; rasterizzatore nel motore (**fatto il 23/10/2026**: 26 classi, `docs/06_kit_navi.md`, `engine/README.md`) | coerenza sprite/modello a nave intatta: sagoma 0,98–1,00, il 96–100% dei pixel entro un livello di grigio |
| D3 | Fisica: idrostatica per stazioni, allagamento, stabilità, trave, incendi, controllo danni (**fatto il 23/10/2026**: `nucleo/danni.hpp`, 15 navi di superficie; `engine/README.md`, «Fisica dei danni») | i casi storici del dossier: modo e ordine di grandezza dei tempi (tabella nel README) |
| D4 | Effetti: particelle, fuoco e fumo, fenomeni delle esplosioni subacquee, acqua attorno alla nave (**fatto il 24/10/2026**: `nucleo/fenomeni.hpp`, `grafica/effetti.hpp`; `engine/README.md`, «Effetti visivi») | sequenze per carica e profondità confrontate con le misure (prove `fenomeni:` ed `effetti:`) |
| D5 | Missioni e IA: stati, relitti, messaggi, reazioni (ritirata, soccorso, abbandono) (**fatto il 24/10/2026**: §7.1, `missioni/danni_missione.hpp`) | 22 scene senza errori; 110 partite di `verifica_fondale` senza urti (6 esiti diversi per scontri diversi) |
| D6 | Il nostro sottomarino: shock, guasti, falle, emersione d'emergenza (lo spegnimento via filo dei siluri e' nella fase C6 di `docs/11`; la detonazione volontaria resta solo nel livello classico) (**fatto il 24/10/2026**: sezioni, compartimenti e scafo resistente degli 11 sottomarini; battello danneggiato in immersione, urto e sistemi, esiti e segni in superficie, dati per la stazione Stato; §10, `engine/README.md`) | prove con le soglie del dossier 02 §9.4 |

D2 e D3 sono indipendenti e possono procedere in parallelo; D4 ha bisogno di D2 per agganciare gli effetti al
modello.

## 9. Decisioni aperte

1. ~~Modello 3D nel motore~~ **deciso** (§5.1): sprite generati al volo dal modello per le navi danneggiate.
2. **La fisica decide il modo di affondare** (consigliato) oppure archetipi scelti per probabilità.
3. **Livello classico:** relitti che affondano in modo realistico anche nelle missioni originali (consigliato).
4. ~~Detonazione volontaria~~ **decisa il 24/10/2026:** non esisteva per i siluri veri del 1989 (§4). L'Enhanced ha
   solo i comandi via filo reali (guida, quota, velocità, ricerca, spegnimento), senza detonazione nemmeno come
   opzione; il livello classico tiene il DETONATE del gioco originale.
5. **Tono:** zattere e scialuppe come piccole sagome, nessun dettaglio crudo sull'equipaggio (consigliato).
6. ~~Fase D6~~ **decise il 24/10/2026** le dieci decisioni sul sottomarino colpito del §10.3, tutte secondo le
   raccomandazioni (la decima, lo spegnimento via filo, va nella fase C6 dei comandi).

## 10. Progetto della fase D6: il sottomarino colpito (24/10/2026)

Stato: **decisioni prese il 24/10/2026** (§10.3, tutte come le raccomandazioni); **D6a e D6b fatte** il 24/10/2026
(§10.6, §10.7); **D6c, D6e e D6d fatte** (§10.8–§10.10): D6 chiusa il 24/10/2026, poi la verifica storica
(§10.11), il battello emerso sulla fisica delle navi con la verifica visiva (§10.12), incendi ed esplosioni
interne (§10.13). Coordinato con la dinamica del sottomarino della Fase 2
(`docs/00_ROADMAP.md`) e con il posto di comando e il pannello Manovra della fase C2
(`docs/11_comandi_e_stazioni.md` §3.1 e §8).

### 10.1 Cosa c'è già

- **Dati dei battelli** (`scafi/<classe>.toml` degli 11 sottomarini, `docs/06_kit_navi.md` «Sottomarini»):
  - sezioni dello scafo esterno, compartimenti fra paratie, casse zavorra di prua e di poppa, intercapedine dei
    doppi scafi;
  - scafo resistente per stazione (raggio, quota, due scafi affiancati nel Typhoon);
  - quote di collaudo e di collasso, dislocamenti pubblicati, riserva di spinta tarata, zavorra iniziale.
- **Fisica in emersione:** quella delle navi (D3), con portelli stagni, casse che non bruciano ed esaurimento
  d'emergenza quando il battello imbarca acqua (prova `danni: sottomarini in emersione`).
- **Moto oggi:** quello del livello classico, cinematico (`UnitaClassica`: quota ordinata e rateo, rotta e rateo
  di virata, velocità ordinata). Il posto di comando di C2 lo comanda con:
  - la barra di immersione a 9 scatti;
  - le quote automatiche (periscopica, 150 ft);
  - l'emersione d'emergenza, che oggi è solo un rateo di quota verso la superficie.
- **Danni del giocatore nel livello classico:** quelli dell'originale (`docs/originale/sensori_e_navigazione.md`
  §9.4). Con i messaggi 20, 26 e 30, la metà delle volte si guasta uno di sei sistemi; nessuna riparazione.
- **Non c'è ancora:** la dinamica della Fase 2, cioè spinta, resistenza, timone e piani, assetto e cavitazione
  (C6 in `docs/11`).

### 10.2 Chi fa cosa: un solo proprietario per ogni grandezza

| Modulo | Possiede | Riceve | Fornisce |
|---|---|---|---|
| **Ordini e posto di comando** (C1–C2) | posizioni di telegrafo, timone, barra, quote automatiche | tasti e pannelli | ordini (con storico e replay) |
| **Dinamica del sottomarino** (Fase 2, C6) | posizione, velocità, rotta, quota, assetto longitudinale | ordini; **forze idrostatiche** e **limiti** da D6 | stato del moto |
| **Battello danneggiato** (D6, modulo nuovo del nucleo) | acqua nei compartimenti e nelle casse, aria delle bombole, integrità dello scafo resistente, stato dei sistemi, lavoro dell'equipaggio | urti (esplosioni), stato del moto, ordini di D6 (esaurimento, isolamento, priorità) | peso netto e momento d'assetto; limiti (velocità massima, timoni bloccati, niente periscopio); eventi (messaggi, rumori, implosioni) |
| **Stazioni e messaggi** (C2–C4, Fase 3) | disegno | stato dei sistemi, eventi | — |

- **D6 non muove il battello.** Calcola il peso netto e il momento d'assetto che l'acqua imbarcata, le casse e
  l'aria producono; chi integra il moto li usa.
- **In superficie** il battello passa alla fisica delle navi (`DanniNave`), con la stessa acqua nelle stesse
  celle; tornando sotto, la riprende D6.

### 10.3 Decisioni (prese il 24/10/2026)

**1. Chi usa il modello D6.**
- Possibilità:
  - (a) solo il nostro battello nell'Enhanced;
  - (b) anche i sottomarini dell'IA nell'Enhanced;
  - (c) anche il livello classico.
- **Decisione (raccomandata): (a) + (b).** Il livello classico tiene i danni dell'originale, come per le navi (D5: regola
  fedele contro regola di simulazione). I battelli dell'IA usano lo stesso modello, con l'equipaggio automatico.

**2. Accoppiamento con il moto prima che esista la dinamica della Fase 2.**
- Possibilità:
  - (a) D6 aspetta la dinamica;
  - (b) D6 espone subito forze (peso netto, momento), e un adattatore provvisorio le traduce nel moto cinematico
    di oggi;
  - (c) D6 scrive direttamente quota e rateo.
- **Decisione (raccomandata): (b).** L'interfaccia a forze è quella che servirà alla Fase 2, quindi niente da rifare dopo.
- L'adattatore cambia il rateo di quota: un battello pesante scende alla velocità di regime della resistenza,
  `w = sign(F)·sqrt(|F| / (½ ρ C_d A_pianta))`. Gli idroplani ne compensano una parte, che cresce con il
  quadrato della velocità:
  - portanza `L = ½ ρ V² S C_L`;
  - sul 688 a 10 m/s dell'ordine di 100 t (stima: due piani da circa 12 m², C_L 0,8; più la portanza dello scafo
    con l'assetto).
- È la «portanza dinamica più rapida della zavorra» della dottrina (dossier 02 §9.2): a velocità alta un battello
  allagato si tiene con i piani, da fermo affonda.
- Tocca `missione.cpp`, quindi va fatto insieme alla sessione che lavora sul runtime delle missioni.

**3. Allagamento in immersione.**
- Possibilità:
  - (a) le celle a tutta sezione di D3;
  - (b) un modello dedicato: compartimenti = scafo resistente fra paratie (dai raggi per stazione), casse =
    intercapedine ed estremità, il resto allagabile.
- **Decisione (raccomandata): (b)** in immersione, (a) in superficie, con la stessa acqua.
- Portata `Q = C_d A sqrt(2 g h)` dalla quota della falla, come nella tabella del dossier 02 §9.1: un foro da 5"
  imbarca 0,16 m³/s a 20 m e 0,60 m³/s a 300 m.
- L'acqua sta sul fondo del compartimento, con il pelo libero orizzontale. Pesa e sposta l'assetto, appruando o
  appoppando il battello.
- Paratie stagne fino a una differenza di quota (stima: la quota di collaudo per le paratie dei compartimenti di
  salvataggio sovietici; molto meno per le altre).

**4. Casse, aria e esaurimento d'emergenza.**
- Possibilità:
  - (a) esaurimento istantaneo;
  - (b) aria delle bombole limitata, con la legge di Boyle e valvole a portata finita.
- **Decisione (raccomandata): (b).** Il volume che una massa d'aria svuota si riduce come `1 / (1 + h/10,33)`: a 300 m
  1/30 di quello in superficie (dossier 02 §9.1). In risalita l'aria si espande e la salita si autoalimenta.
- Riserva d'aria per classe in m³ a 1 bar (stima: quanto basta a svuotare 2–3 volte le casse in superficie;
  affidabilità bassa).
- **Intatto** il battello è in assetto neutro; casse d'assetto e di compensazione non si simulano.
- **Ctrl+E**, l'emersione d'emergenza del pannello Manovra di C2, nell'Enhanced diventa l'esaurimento: rumore
  forte, salita difficile da fermare e, oltre una certa velocità di salita, assetto incontrollato.
- Valvole ghiacciate come nel Thresher: non si simulano.

**5. Urto e guasti dei sistemi.**
- Possibilità:
  - (a) i 6 sistemi dell'originale, rotti o funzionanti;
  - (b) sistemi graduati (0–100%, come il «% operativo» della stazione Stato dell'originale), con riparazioni.
- **Decisione (raccomandata): (b)**, con le soglie del fattore d'urto del dossier 02 §9.4:

  | KSF | Effetto |
  |---|---|
  | 0,05–0,1 | strumenti e luci: la palette tremola, la vista sobbalza, guasti lievi e brevi |
  | 0,1–0,3 | apparecchiature danneggiate; perdite da tenute e tubolature (fori da 1–2"); possibile SCRAM: propulsione persa per 5–20 min, poi motore d'emergenza a bassa velocità |
  | 0,3–0,6 | allagamenti da rotture localizzate (fori da 2–5" o maggiori) |
  | > 0,6–1 | rottura dello scafo resistente: perdita del battello |

  - La soglia si riduce con la quota: × (1 − 0,5·h/h_collaudo).
  - **Sistemi:** i sei dell'originale (calcolatore del sonar, schiera, periscopio, sonar di tiro, motori, antenna)
    più idroplani, timone, reattore e propulsione, aria ad alta pressione, pompe, sonar di prua.
  - Ogni guasto ha un tempo di riparazione (stima) e limita un ordine. Esempi: motori al 75% danno 3/4 della
    velocità massima, come nell'originale; idroplani bloccati tengono l'angolo che avevano.

**6. Collasso dello scafo.**
- Possibilità:
  - (a) collasso alla quota nominale;
  - (b) quota casuale dal seme fra 1,0 e 1,4 volte la nominale (Rule: il collasso vero supera il previsto in
    media del 44%, dossier 07 §8.2).
- **Decisione (raccomandata): (b).** La quota si abbassa con i danni allo scafo resistente.
- **Corretto con la verifica storica (§10.11):** il fattore e' 1,1–1,8, con media 1,45. L'intervallo 1,0–1,4
  aveva media 1,2 e non rispettava la regola di Rule che lo motivava; il K‑8 (x 1,62) ne restava fuori.
- Il collasso implode i compartimenti asciutti uno dopo l'altro: ciascuno in 0,1–0,3 s, a 1–30 s di distanza,
  mentre il relitto scende a 5–15 m/s (dossier 07 §9.4).
- Energia di ciascuno `E = p·V`, in TNT `W = p V / 4,184·10⁶` kg. Serve ai rumori e ai fenomeni (D4).

**7. Controllo danni del nostro battello.**
- Possibilità:
  - (a) tutto automatico;
  - (b) automatico con le priorità del dossier 02 §9.2, che il giocatore può cambiare;
  - (c) tutto a mano.
- **Decisione (raccomandata): (b).** L'equipaggio:
  1. dà l'allarme;
  2. chiude le valvole e isola il compartimento;
  3. ripara: falle, incendi, propulsione, governo, sensori.
- Il giocatore:
  - sposta le priorità;
  - ordina l'esaurimento e l'isolamento di un compartimento, anche con dentro l'equipaggio (la scelta dura dei
    sottomarini);
  - vede tutto nella stazione Stato e danni: minima nelle fasi C, completa nella Fase 3.

**8. Messaggi ed effetti a bordo.**
- **Decisione (raccomandata):**
  - **rapporti dell'equipaggio**, con priorità, nella coda dei messaggi di C1: «Allagamento nel locale macchine»,
    «SCRAM del reattore», «Piani poppieri bloccati a 10 in salita»;
  - **luci e vista:** la palette tremola e la vista sobbalza con il fattore d'urto;
  - **al sonar:** i rumori di rottura dei bersagli colpiti (dossier 07 §9.3–9.4), quando arriva l'acustica.

**9. Sottomarini nemici colpiti.**
- **Decisione (raccomandata):** stesso modello (decisione 1), con questi esiti:
  - **implosione e affondamento:** rumori, poi olio e detriti in superficie dopo 10–60 minuti se la rottura è
    profonda; aria, olio e detriti abbondanti se è vicina alla superficie (dossier 07 §8.3);
  - **emersione d'emergenza:** riuscita nel 30–70% dei casi se la quota è sotto i 150 m e la falla piccola
    (dossier 07 §8.4). In superficie diventa una nave di D3, visibile con l'opera viva e le appendici di poppa.
- Nel livello classico restano gli script dell'originale.

**10. ~~Spegnimento via filo dei siluri~~ deciso il 24/10/2026:** sta nella fase C6 dei comandi (`docs/11` §8),
con le impostazioni reali dei siluri; non fa parte di D6.

### 10.4 Interfaccia (bozza)

- `BattelloDanni(const DatiScafo&, seme)`, nel nucleo, file nuovi `nucleo/battello.hpp/.cpp`.
- `urto(const Esplosione&, distanza_m, angolo)`: fattore d'urto, guasti, falle, collasso.
- `avanza(dt, const StatoMoto&)`, dove `StatoMoto` = quota, velocità, assetto, sbandamento. Restituisce
  `ForzeIdrostatiche{peso_netto_N, momento_assetto_Nm}`.
- **Ordini di D6:** `esaurisci()`, `isola(compartimento)`, `priorita(...)`.
- **Letture:** `sistemi()`, `limiti()` (velocità massima, timone e piani, periscopio, antenna), `eventi()`
  (messaggi, rumori, implosioni, con l'istante).
- **Determinismo:** tutto il caso viene dal seme, come per le navi; gli ordini passano dalla coda di C1, quindi il
  replay ridà la stessa partita.

### 10.5 Piano di lavoro

| Sottofase | Contenuto | Verifica | File |
|---|---|---|---|
| D6a | modulo `BattelloDanni`: allagamento per quota, paratie, casse e aria con Boyle, peso netto e momento, collasso con implosioni | intatto neutro; portate della tabella del dossier 02 §9.1; esaurimento che riesce a 100 m e non a 400 m con la stessa falla; collasso fra 1,1 e 1,8 volte la quota prevista (era 1,0–1,4, corretto con la verifica storica del §10.11) | `nucleo/battello.*`, prove; una riga nel `CMakeLists.txt` condiviso |
| D6b | urto e sistemi graduati, riparazioni, SCRAM e motore d'emergenza | soglie del dossier 02 §9.4: Mk 48 a 10 m (HSF 0,77) letale o quasi; carica da 135 kg a 30 m: guasti senza falle | `nucleo/battello.*` |
| D6c | adattatore cinematico (decisione 2) e passaggio fra superficie (D3) e immersione; Ctrl+E come esaurimento nell'Enhanced | battello allagato che a velocità si tiene con i piani e da fermo affonda; emersione d'emergenza riuscita o no secondo i casi del dossier 07 §8.4 | `missione.cpp`, `comando.*`: insieme alla sessione dei comandi |
| D6d | stazione Stato e danni minima, rapporti dell'equipaggio, luci e vista | istantanee; messaggi nell'ordine giusto | stazioni (fasi C) |
| D6e | sottomarini dell'IA colpiti: esiti, segni in superficie, emersione | casi del dossier 07 §8 | `danni_missione.*` |

D6a e D6b non toccano file di altre sessioni, a parte una riga nel `CMakeLists.txt`; da D6c in poi serve il
lavoro comune con la sessione dei comandi (C2–C6). Quando arriva la dinamica della Fase 2, l'adattatore si
toglie: le forze di D6 restano le stesse.

**D6c concordata con la sessione dei comandi** (24/10/2026):
- **sessione dei danni** (questa): file nuovo `missioni/danni_sottomarini.*` e le sue prove.
  - Un `BattelloDanni` per ogni sottomarino colpito nell'Enhanced.
  - `urto` dai colpi al posto del guasto 1/2 del classico.
  - Ctrl+E come `esaurisci()`.
  - Messaggi degli eventi con i codici **270–289**.
  - `perso()` per l'esito e le implosioni per gli eventi acustici.
  - Interfaccia `sistemi(id)` e `limiti(id)` per le stazioni.
- **sessione dei comandi:** `StazioniMissione::guasti()` dall'Enhanced, telegrafo limitato, rifiuti
  dell'equipaggio (codici 213 e 220–262 sono suoi).
- **moto in `missione.cpp`** (rateo di quota da `rateo_verticale_regime`, piani dalla barra): chi tiene il file in
  quel momento, a turno.
- **Quando:** dopo la correzione della convenzione degli angoli nelle missioni, che tocca lo stesso file.

### 10.6 Realizzazione di D6a (24/10/2026)

`nucleo/battello.hpp` (`BattelloDanni`), prova `battello: sottomarino danneggiato in immersione`.
- **Compartimenti:** lo scafo resistente (raggi per stazione di `scafi/<classe>.toml`) dentro le celle non di
  zavorra, per la permeabilita'.
  - 688: tre compartimenti, prodiero 2066 m³, reattore 359, macchine 1321;
  - Typhoon: compartimenti propri e uguali nei due scafi affiancati.
- **Casse:** l'intercapedine e le estremita' fuori dallo scafo resistente (688: 1018 m³).
- **Allagamento:** fino all'equilibrio fra il mare e l'aria compressa del compartimento (96,8% a 300 m).
  - Portate della tabella del dossier 02 §9.1: un foro da 5" imbarca 0,155 m³/s a 20 m e 0,602 a 300 m.
  - Paratie che cedono a 0,3 volte la pressione di collaudo (688: circa 87 m), poi l'acqua passa finche' le
    pressioni si pareggiano.
- **Esaurimento:** aria delle bombole pari a 2,5 volte le casse a 1 bar, che le svuota in 30 s in superficie;
  volume utile p_atm/p; l'aria si espande in risalita ed esce dagli ombrinali. Le casse squarciate non tengono
  l'aria.
- **Collasso:** alla quota di collasso per un fattore fra 1,1 e 1,8 dal seme (1,0–1,4 fino alla verifica storica del
  §10.11); poi implodono i compartimenti
  asciutti, a 1–30 s l'uno dall'altro, con l'energia p V (688 a circa 530 m: circa 0,5 t di TNT il reattore).
- **Forze:** peso netto e momento d'assetto, dall'acqua nei compartimenti e dall'aria nelle casse (intatto:
  neutro).
- **Adattatore cinematico:** `rateo_verticale_regime(peso, pianta, velocita', piani)`.
  - Velocita' verticale di regime dalla resistenza sulla pianta (Cd 1).
  - Portanza dei piani e dello scafo 0,5 ρ v² (3% della pianta) 0,8: sul 688 circa 100 t a 10 m/s.
  - Piani +1 tutti in salita, 0 per tenere la quota, −1 tutti in discesa.
- **Esiti sul 688, con l'esaurimento dopo 30 s:**

  | Falla | Fermo | 10 m/s e barra in salita |
  |---|---|---|
  | 5" a 100–200 m | riemerge in 1,5–3 min | riemerge in 1–2 min |
  | 5" a 300–400 m | perso (collasso) | riemerge in 2,5–3,5 min |
  | 10" a 100–200 m | perso | riemerge in 1–2 min |
  | 10" a 300–400 m | perso | perso |

  - Come la dottrina del dossier 02 §9.2: la portanza e' piu' rapida della zavorra.
  - Come la lezione del Thresher: in profondita' l'aria rende poco.
- **Stime da confermare:** resistenza delle paratie, riserva d'aria e portata delle valvole, portanza.

### 10.7 Realizzazione di D6b (24/10/2026)

`BattelloDanni::urto(tnt, distanza, angolo, x, quota)`, prova `battello: urto, guasti dei sistemi e riparazioni`.
- **Fattore d'urto:** `KSF = HSF (1 + sin θ) / 2` con `HSF = sqrt(W lb) / R ft`, diviso per la riduzione della
  soglia con la quota, `1 − 0,5 h / h_collaudo` (al minimo 0,25). Un Mk 48 a 33 ft da sotto da' 0,77, come nel
  dossier 02 §9.4.
- **Effetti per fasce:**

  | KSF efficace | Effetto |
  |---|---|
  | da 0,05 | scossa (evento con l'intensita': luci, vista); guasti con probabilita' (k − 0,05)/0,8 per sistema |
  | da 0,1 | perdite da 1–2" (una, due da 0,2); SCRAM del reattore fino all'80% |
  | da 0,3 | falle da 2–5" e oltre; casse squarciate entro 15 m; danno allo scafo resistente, che abbassa la quota di collasso |
  | da 0,6 | rottura dello scafo resistente (falla da 0,3–1 m²), certa a 1 |

- **Sistemi:** i sei dell'originale piu' idroplani, timone, propulsione, aria, pompe, sonar di prua, ciascuno da
  0 a 100%.
  - Una perdita oltre il 70% lascia un danno permanente: le riparazioni arrivano al massimo a 1,3 − perdita, mai
    sotto il 50%.
  - `limiti()`: velocita' (il peggiore fra motori e propulsione; 15% durante lo SCRAM), timone e idroplani sotto
    il 30%, periscopio e antenna sotto il 50%, sonar, schiera.
- **Equipaggio** (squadre e qualita' della classe):
  - tampona le perdite fino a circa 2,4" (piu' lentamente in profondita');
  - pompa i compartimenti senza falle aperte (le pompe non rendono piu' a 200 m);
  - ripara i sistemi in 15 minuti da zero per squadra, nell'ordine del dossier 02 §9.2: propulsione, governo,
    aria e pompe, sensori, comunicazioni. `priorita(sistema)` sposta un sistema in cima.
  - Dopo lo SCRAM il reattore torna critico in 5–20 minuti.
- **Esiti sul 688** (50 semi; dopo l'urto avanti tutta, barra in salita ed esaurimento):

  | Urto | KSF | Esito |
  |---|---|---|
  | Mk 48 a 10 m sotto, a 100 m | 0,94 | perso 45 volte su 50 («letale o quasi») |
  | Mk 48 a 10 m sotto, a 250 m | 1,37 | sempre perso |
  | 135 kg a 30 m di fianco, a 50 m | 0,10 | nessuna falla, 0,7 guasti per urto |
  | la stessa carica a 250 m | 0,15 | perdite da 1–2", 1,6 guasti; riemerge |
  | 135 kg a 15 m sotto, o RBU da 25 kg a 5 m, a 100 m | 0,40–0,42 | falle, 5–6 guasti, SCRAM in 4 casi su 5; riemerge |

- **Diesel** (Kilo, Foxtrot): nessuno SCRAM.
- **Stime da confermare:** fasce di guasto e loro gravita', diametri delle perdite e delle falle, tempi di
  tamponatura e riparazione, portata delle pompe, velocita' del motore d'emergenza.

### 10.8 D6c: sottomarini colpiti nelle missioni (24/10/2026)

`missioni/danni_sottomarini.hpp` (`DanniSottomarini`), solo con la regola di simulazione; prova `danni dei
sottomarini nelle missioni`.
- **Fatto:**
  - un `BattelloDanni` per ogni sottomarino colpito, affidato alla fisica (`Missione::affida_alla_fisica`);
  - colpi dalla regola d'impatto:
    - Mk 48 d'influenza 3–8 m sotto lo scafo;
    - siluri da 533 mm a contatto sul fianco; siluri leggeri a contatto;
    - cariche e razzi a 5–25 m;
    - missili solo su un battello in superficie;
  - velocita' massima dai limiti dei sistemi (`OrdineMotore::velocita_max_kn`);
  - l'IA tenta l'emersione d'emergenza dopo un allagamento sopra i 150 m nel 30–70% dei casi;
  - il collasso distrugge l'unita' (`Missione::distruggi`);
  - messaggi 270–281: rapporti dell'equipaggio del giocatore (allagamento, avarie, SCRAM, reattore, riparazione,
    falla tamponata, paratia, esaurimento, aria finita, rottura dello scafo); rumori di rottura e implosioni dei
    sottomarini nemici entro 20 nm;
  - `esaurisci(id)` per Ctrl+E; `sistemi(id)`, `limiti(id)` per le stazioni; `rateo_quota(id)` per il moto.
  - **Moto** (`missione.cpp`): con `OrdineMotore::quota_fisica` la quota segue
    `rateo_verticale_regime(peso netto, pianta, velocita', piani)`, con i piani nel verso della quota ordinata,
    al posto della barra. Si attiva oltre circa 5 t di peso netto. In superficie il battello leggero galleggia; il
    livello classico non cambia (rilievo identico alla base).
  - Esempi della prova: Ctrl+E a 120 m porta il 688 intatto in superficie in meno di 5 minuti con la quota
    ordinata ancora a 120 m; un Victor III colpito da un Mk 48 a 100 m non resta alla sua quota.
  - **Sessione dei comandi** (`main.cpp`, stazioni):
    - con `--regola simulazione` crea `DanniSottomarini` dopo `DanniMissione`;
    - Ctrl+E chiama anche `esaurisci`;
    - la stazione Stato (F3) mostra i 12 sistemi;
    - timone e idroplani bloccati danno i messaggi 214–215;
    - il telegrafo e' rifiutato oltre `limiti().velocita` (frazione della massima) con il messaggio 64.
- **Superficie:** il battello emerso danneggiato passa alla fisica delle navi (§10.12).

### 10.9 D6e: esiti dei sottomarini colpiti e segni in superficie (24/10/2026)

Sempre in `DanniSottomarini`; prova `sottomarini colpiti: esiti e segni in superficie`.
- **Esiti:**
  - **perso:** per collasso; quando tutti i compartimenti sono allagati (nessuno a bordo si salva); o quando
    resta 10 minuti pesante sul fondo con l'aria delle bombole finita (l'IA esaurisce le casse da sola appena
    tocca il fondo, il giocatore solo se lo ordina);
  - **emerso danneggiato:** l'IA resta in superficie a 3 nodi al massimo;
  - **fuori combattimento** (solo l'IA, come le navi di D5): integrita' dell'originale finita e battello
    menomato (emerso danneggiato, propulsione sotto il 30% o piu' del 10% dei compartimenti allagato).
- **Segni in superficie** (`segni()`, dossier 07 §8.3) dove il battello si perde:
  - rottura profonda (oltre 300 m): nafta (6 ore) e pochi rottami (1 ora) dopo 10–60 minuti;
  - vicino alla superficie o sul fondo: aria per 5 minuti, nafta e rottami abbondanti subito;
  - il gasolio dei diesel fa una chiazza una volta e mezza piu' grande e piu' lunga.
  - Li disegna l'app con gli effetti gia' esistenti (nafta, schiuma, rottami): parte della sessione dei comandi.
- **Messaggi nuovi:**
  - 282: «siamo posati sul fondo e pesanti», al giocatore;
  - 283: «un sottomarino nemico esaurisce le casse», dal sonar entro 20 nm.
- **Esempi della prova:**
  - un Mk 48 sotto lo scafo di un Victor III a 400 m lo perde; la nafta affiora 10–60 minuti dopo il collasso,
    entro 500 m dal punto;
  - su un fondale a 150 m un Victor III colpito da un 533 esaurisce le casse, resta sul fondo e si perde, in 4
    semi su 4;
  - il 688 del giocatore nello stesso caso riceve il rapporto 282 e si perde solo quando ha tutti i
    compartimenti allagati.

### 10.10 D6d: dati per la stazione Stato e danni (24/10/2026)

Parte dei dati, in `DanniSottomarini`; prova `sottomarini colpiti: dati per la stazione Stato`. L'interfaccia
(pannello Stato con diagramma e rapporti, luci che tremolano, vista che sobbalza) e' della sessione dei comandi.
- `prepara(m, id)`: modello di un battello intatto (il giocatore all'avvio), cosi' diagramma e sistemi si vedono
  prima del primo colpo.
- `rapporti(id)`: eventi del battello in ordine di tempo di missione (testo, tipo, sistema, compartimento).
- `scossa(id)`: istante e fattore d'urto efficace dell'ultima scossa; la decadenza visiva la fa l'app.
- `compartimenti(id)`: nome, uso, x0 e x1 lungo lo scafo, volume, acqua, imploso.
- `BattelloDanni::imposta_tempo`: gli eventi usano il tempo di missione dalla creazione del modello.
- `appesantisci(m, id, t)`, `alleggerisci(id)`: casse d'assetto per posarsi sul fondo (`docs/11` §6).
  - L'acqua entra dalle valvole a 0,05 m³/s (3 t al minuto) ed esce con le pompe, che a 200 m non rendono piu'.
  - Il peso passa alla dinamica con `OrdineMotore::peso_netto_N`; non conta come acqua imbarcata.
- `urto(m, id, tnt, distanza, angolo)`: urto di prova come un colpo della missione, per le istantanee
  dell'interfaccia (per esempio una carica da 135 kg a 15 m sotto lo scafo, fattore d'urto circa 0,4).
- Per la dinamica dell'Enhanced (C6a, sessione sulla cadenza) `DanniSottomarini` riempie anche
  `OrdineMotore::peso_netto_N`, `momento_assetto_Nm`, `timone` e `idroplani`.
  - Con la dinamica attiva la quota la integra il suo modello, con assetto e portanza propri.
    `rateo_verticale_regime` serve solo per la resistenza verticale (velocita' 0, per non contare due volte la
    portanza dei piani).
  - Esempio della C6a: con 100 t d'acqua il 688 tiene la quota a 15 nodi con circa 11° di prua in su; fermo
    affonda a 1,5 m/s.
  - Il ramo `quota_fisica` resta come ripiego quando le forze mancano.

### 10.11 Verifica storica del sottomarino danneggiato (24/10/2026)

Prova `battello: casi storici` (`engine/test/prove_battello.cpp`): il moto verticale e' quello dell'adattatore
cinematico, senza dinamica.

| Caso | Fonte | Prova | Esito del modello |
|---|---|---|---|
| **Thresher** (1963): allagamento in sala macchine da un foro di 2–5" a circa 300 m, propulsione persa, esaurimento inutile; implosione a ~730 m contro i ~590 previsti | dossier 07 §8.2, dossier 02 §9.1; [WP‑Thr], [USNI21] | Sturgeon (stessa quota di collaudo, 400 m), foro da 5" in sala macchine a 300 m, fermo, esaurimento dopo 60 s | perso in 10 semi su 10 (20 su 20 nel giro esplorativo); collasso fra 660 e 1080 m. Con un foro da 2" si salva |
| **San Francisco** (2005): monte sottomarino a ~160 m, casse prodiere squarciate, esaurimento riuscito | dossier 07 §8.4 (da verificare) | 688 a 160 m, cassa di prua squarciata, esaurimento | emerge |
| **Kursk** (2000): siluro esploso nel compartimento 1, seconda esplosione 135,8 s dopo; battello sul fondo a ~108 m, superstiti per ore nel 9° compartimento | dossier 07 §8.2; [NORSAR02] | Oscar II a quota periscopica, falla da 1 m² nel C1, poi 3 m² in C1 e C2, fondo a 108 m, niente esaurimento | sul fondo, nessun collasso, prua allagata oltre l'80%, compartimento di poppa asciutto |
| **Cariche della WWII**: 130 kg spaccano uno scafo resistente a ~6 m e costringono all'emersione a ~12 m; ~100 kg contro 1000 t: letale entro 3–4 m, avaria entro 8–10 m | dossier 07 §8.1; [WP‑DC] | Foxtrot a 60 m, 130 kg a 45° | a 6 m scafo rotto in circa meta' dei casi (24 su 40), a 4 m sempre; a 12 m falle senza rotture, sempre riemerso con barra in salita ed esaurimento |
| **Regola di Rule**: collasso reale in media +44% sul previsto (Thresher x 1,24; K‑8 x 1,62) | dossier 07 §8.2; [Rule] | quota di collasso su 20 semi | 1,1–1,8 volte la prevista (media 1,45) |
| **Emersione d'emergenza** riuscita nel 30–70% dei casi sopra i 150 m con falla sotto 10 cm (stima di gioco) | dossier 07 §8.4 | 688 fermo, falla da 5–10 cm a 100–150 m | la fisica da sola riemerge sempre. La probabilita' del dossier resta come probabilita' che l'IA ci provi (§10.8) |

- **Cambiato:** il fattore della quota di collasso, da 1,0–1,4 (media 1,2: contro la stessa regola di Rule che lo
  motivava) a 1,1–1,8. Il K‑8 ora e' dentro.
- **Confermate senza cambiamenti:**
  - paratie a 0,3 volte la pressione di collaudo: al Kursk a 108 m (11 bar) reggono e la poppa resta asciutta;
  - aria pari a 2,5 volte le casse: basta al San Francisco e non al Thresher;
  - portanza dei piani (3% della pianta, Cl 0,8);
  - fasce dei guasti del dossier 02 §9.4.
- **Aggiunti con il §10.13:** Kursk con le esplosioni interne, Komsomolets (incendio) e Bonefish (batterie).
- **Non coperto:** il ghiaccio nelle valvole dell'aria (Thresher); l'affondamento del Komsomolets dopo 5 ore in
  superficie (§10.13).

### 10.12 Battello emerso sulla fisica delle navi, geometria dello scoppio, verifica visiva (24/10/2026)

- **Battello emerso danneggiato** (`DanniSottomarini::in_superficie`): quando un sottomarino colpito riemerge passa
  a `DanniNave` (D3) con lo stesso `scafi/<classe>.toml`.
  - L'acqua dei compartimenti e la frazione piena delle casse passano per nome di cella
    (`DanniNave::imposta_acqua`, nuovo).
  - Le falle aperte e le casse squarciate diventano falle sul fondo delle celle.
  - Da li' puo' sbandare, appruarsi, capovolgersi e affondare in superficie (se affonda e' perso, con i segni);
    resta in superficie e non si immerge piu'.
  - `sincronizza(mondo)` passa la posa al disegno: lo sprite si genera dal modello 3D, con le appendici di poppa.
    La chiamata in `main.cpp` e' della sessione dei comandi.
  - Esempi: un Kilo colpito da 300 kg a 10 m di fianco a 60 m riemerge e in superficie sbanda di circa 34°; un
    Typhoon nello stesso caso di 26°. Prova `sottomarini colpiti: emersione con la fisica delle navi e geometria
    dello scoppio`.
- **Geometria vera dello scoppio** (siluri del modello dell'Enhanced, C6b): `Colpo::geometria_vera`, `distanza_m`
  (dal centro della vittima) e `dz_m` (positivo sotto lo scafo), riempiti in `missione.cpp` quando scatta la
  spoletta.
  - `DanniSottomarini` usa come distanza dallo scafo quella dal centro meno meta' larghezza.
  - L'incidenza e' `asin(|dz| / distanza)`.
  - Senza geometria vera restano le stime per arma.
- **Verifica visiva** (`extracted/navi/_verifica/sottomarino_danneggiato_688.png`,
  `sottomarini_danneggiati_poppa.png`): sprite dal modello di 688, Kilo, Victor III, Oscar II e Typhoon intatti,
  appruati di 12°, sbandati di 35°, capovolti e spezzati.
  - Timoni superiori e inferiori, piani poppieri, piastre del 688 ed eliche si vedono attaccati allo scafo e
    nelle proporzioni attese.
  - Il Typhoon appruato mostra soprattutto il timone alto; i piani a poppa delle eliche si vedono di scorcio.
  - Le misure restano le stime di `docs/06_kit_navi.md` («Formato MDL2 e appendici di poppa dei sottomarini»);
    nessuna correzione necessaria a vista.
- **Livello classico:** rilievo identico alla base (0 partite diverse su 110).

### 10.13 Incendi ed esplosioni interne del sottomarino (24/10/2026)

`BattelloDanni::incendio`, `esplosione_interna`; in `DanniSottomarini` le stesse per le prove e gli scenari.
- **Incendio** (per compartimento: `fuoco` 0–1, `fumo` 0–1, `ossigeno`, combustibile per uso):
  - il fuoco raddoppia in 2 minuti finche' c'e' ossigeno. In un compartimento chiuso lo consuma (dal 21% al 12% in
    circa 10 minuti in 1000 m³) e sotto il 12% si spegne;
  - a fuoco forte puo' rompersi una tubolatura dell'aria ad alta pressione (in media in 20 minuti). L'aria delle
    bombole lo tiene vivo e si consuma: meno aria per l'esaurimento (Komsomolets);
  - il fumo riempie il compartimento (1000 m³ in 3 minuti a fuoco pieno), passa dalle paratie cedute e si dirada
    a fuoco spento (10 minuti);
  - il fuoco forte brucia le tenute dello scafo (una perdita da 2" ogni 15 minuti) e passa ai compartimenti
    vicini;
  - danneggia i sistemi del compartimento: sonar a prua, governo in centrale, propulsione nel reattore (o nelle
    macchine dei diesel), motori, aria e pompe nelle macchine, schiera a poppa;
  - **squadre:** dopo le falle, tutte sugli incendi, divise fra i focolai. Nel fumo, con le maschere EAB, rendono
    la meta'; su un fuoco pieno meno di un terzo, perche' un incendio esteso si isola e si lascia senza ossigeno;
  - le squadre lavorano solo nella parte abitabile del battello (non allagata, non sventrata, non piena di fumo).
- **Esplosioni interne:**
  - testate nell'incendio del locale siluri: 300 kg, 0,6% al minuto a fuoco pieno, come i depositi delle navi;
  - idrogeno delle batterie in fiamme: 30 kg;
  - scenari (Kursk).
  - **Effetti:**
    - la sovrappressione `0,4 E / V` sull'aria dei compartimenti raggiunti fa cedere le paratie a cascata oltre
      0,45 volte la loro resistenza (tarato sul Kursk);
    - oltre 50 kg lo scafo resistente si squarcia, con una falla di diametro `W^(1/3)` m;
    - i compartimenti raggiunti prendono fuoco, quello dello scoppio e' sventrato e i sistemi sono gravemente
      danneggiati.
- **Cloro:** le batterie allagate lo producono, come fumo tossico.
- **IA:** emerge dopo 5 minuti di incendio fuori controllo (oltre 0,6) o di fumo denso (oltre 0,8), a qualunque
  quota. Un battello emerso con incendio, fumo o compartimenti sventrati passa alla fisica delle navi (§10.12).
- **Messaggi 284–289:** incendio, esplosione a bordo, incendio spento, cloro, aria nell'incendio, esplosione a bordo
  di un sottomarino nemico (dal sonar).
- **Per la stazione Stato:** `CompartimentoBattello::fuoco`, `fumo`, `ossigeno`, `distrutto`.
- **Verifica storica**, prove `battello: casi storici` e `sottomarini: incendi a bordo, casi storici`:

  | Caso | Fonte | Esito del modello |
  |---|---|---|
  | **Kursk** (2000): siluro esploso nel compartimento 1 a quota periscopica, testate 135 s dopo sul fondo a ~108 m; superstiti nel 9° | dossier 07 §8.2; [NORSAR02] | Oscar II: 150 kg non sfondano la paratia; a fondo in ~35 s; 2 t sfondano le paratie fino al 5° compartimento; dal reattore (6°) a poppa asciutti; nessun collasso |
  | **Komsomolets** (1989): incendio a lampo nel 7° compartimento a ~380 m, tubolatura dell'aria rotta, emerso in ~13 minuti, a galla ~5 ore, poi affondato | dossier 07 §8.4 | Akula (doppio scafo sovietico simile): emerge in circa 9 minuti, l'incendio si spegne quando l'aria finisce, resta a galla. L'affondamento dopo 5 ore non si riproduce: le tenute bruciate aprono falle troppo piccole |
  | **Bonefish** (1988, SS-582): incendio delle batterie in immersione, fumo tossico, emerso e abbandonato | da verificare | Kilo: l'incendio delle batterie consuma l'ossigeno e si spegne in ~5 minuti, il fumo costringe all'emersione in ~6 minuti |
  | Incendio in un compartimento chiuso | fisica | si spegne in meno di 15 minuti con l'ossigeno sotto il 14%; con l'aria ad alta pressione resta acceso e consuma le bombole |

- **Stime da confermare:** crescita, consumo d'ossigeno e fumo; probabilita' della tubolatura rotta e delle
  testate; rendimento delle squadre; soglia delle paratie per le esplosioni (tarata solo sul Kursk).


### 10.14 Emersione violenta di un sottomarino nemico (E1, 24/10/2026)

Un battello dell'IA che si salva con l'esaurimento d'emergenza delle casse arriva in superficie a qualche metro al
secondo: la prua esce dall'acqua con un angolo forte, ricade sul mare, il battello rolla e si assesta. Si vede al
periscopio; dopo restano la schiuma e il battello emerso danneggiato (§10.12).

- **Riferimenti:** le dimostrazioni dell'esaurimento d'emergenza dei 688 della US Navy. Nelle foto la prua esce per
  circa un terzo della lunghezza, con 20-30 gradi di assetto (stima dalle foto). Il caso del Greeneville (9 febbraio
  2001, collisione con l'Ehime Maru) e' proprio una di queste manovre.
- **Quando:** `DanniSottomarini::avanza`, per un battello dell'IA (non il giocatore). Servono tre condizioni:
  - un esaurimento delle casse fra gli eventi del battello;
  - il passaggio da sotto a sopra 0,5 m di quota;
  - una salita mediata oltre 1 m/s, dal rateo della dinamica (`Missione::dinamica`) o dalla quota fra due passi.

  Al massimo una volta per battello. Un'emersione normale degli script non la provoca.
- **Quanto** (`EmersioneViolenta`), dalla velocita' di salita `w` (m/s), dall'assetto della dinamica (15 gradi se
  manca) e dalla velocita' sul fondo `v`, misurata sullo spostamento vero:
  - angolo al culmine: `assetto + 2,5 w`, fra 10 e 35 gradi;
  - frazione della lunghezza fuori: `0,08 + 0,04 w + 0,006 v`, fra 0,12 e 0,40;
  - durata della salita: `1 + 0,4 w` s, fra 1,5 e 4;
  - ricaduta: 1,4 volte la caduta libera dall'altezza della prua.

  Il Victor III che esaurisce a 100 m arriva a 4,8-6,4 m/s, con 31-35 gradi e il 30-38% della lunghezza fuori.
- **Posa** (`posa_emersione`): rotazione attorno al punto che resta in acqua (`x_perno = L/2 - fuori L`):
  - la prua sale con un quarto di seno, mentre lo scafo esce di 4 m;
  - poi ricade con una legge quadratica, cioe' accelerando;
  - poi per 30 s oscillazioni smorzate di assetto (7 s), sussulto (5 s) e rollio (4 gradi, 9 s).

  `sincronizza` la usa durante l'emersione; dopo torna la posa della fisica delle navi (§10.12).
- **Acqua e schiuma:** `Effetti::emersione_violenta(pos, rotta, velocita, angolo, lunghezza, fuori, ricaduta)`.
  Gli effetti seguono l'avanzata del battello:
  - acqua che cola dalla prua mentre esce, dalla punta verso il perno;
  - una colonna alla prua;
  - schiuma bianca lungo lo scafo per 3-5 minuti;
  - al colpo della prua, con il ritardo della ricaduta: spruzzi laterali, nebbia e bolle.
- **Gancio nell'app:** in `main.cpp`, dopo `ds->avanza`, per ogni voce nuova di `ds->emersioni()` si chiama
  `effetti.emersione_violenta(e.pos, e.rotta, e.velocita, e.angolo, e.lunghezza, e.fuori, e.salita_s + e.caduta_s)`.
- **Verifica a vista:** `extracted/navi/_verifica/emersione_violenta_periscopio.png` e `..._sequenza.png`.
  - Scena: un Victor III a 800 m, colpito da 150 kg a 15 m e fatto esaurire subito.
  - Vista: periscopio ad alto ingrandimento puntato su di lui (`--periscopio-su 2 --alto`).
  - Sequenza da t+0,5 a t+59 s: la prua esce a 35 gradi, ricade a t+6,4 s con gli spruzzi alla prua, poi si
    assesta; a poppa resta fuori la gondola dell'antenna rimorchiata.
- **Prove** (`sottomarini colpiti: emersione violenta dopo l'esaurimento delle casse`): fasi della posa, effetto
  (altezze, posizione degli spruzzi, schiuma dopo 2 minuti), innesco sul Victor III con la posa al culmine nel
  mondo e il ritorno alla posa della fisica delle navi, nessuna emersione violenta per il giocatore.
- **Stime da confermare:** coefficienti di angolo e frazione fuori, tempi delle oscillazioni.

### 10.15 Effetti dei sottomarini visibili al periscopio: lanci di missili e piuma (E2, 24/10/2026)

Punto 17 della roadmap. Numeri dal dossier 05 (§1.2, §1.8, §2.2, §3, §6.1) e dal dossier 02 (§7.7). Quasi tutto
l'aspetto visivo e' una **STIMA**: le fonti danno la sequenza, non le misure.

- **Lancio di un missile dall'immersione:** `Effetti::lancio_missile(pos, rotta, quota, missile)`. Restituisce i
  secondi dal lancio all'uscita dall'acqua. Il missile esce piu' avanti del punto di lancio, di
  `quota / tan(angolo di salita)`: 45 gradi per la capsula dell'Harpoon, 55 per il Tomahawk, 38,5-70 per i
  sovietici. All'uscita ci sono ribollio, spruzzo e vapore del getto sul mare. Il corpo e la fiammata seguono la
  traiettoria a tratti; il fumo bianco resta sospeso 25-50 s e si allarga. Il booster esaurito ricade.

  | Missile | Sott'acqua | Volo (STIME) |
  |---|---|---|
  | `harpoon` (UGM-84) | capsula a 3 m/s (2-5 da 20-60 m: 10-30 s); sporge 1,5 s, poi saltano i tappi | booster 3 s a 35→25 gradi, 20→80 m/s (getto di fumo di 50-150 m); turbogetto senza fumo che accelera a 240 m/s e scende a 10 m |
  | `tomahawk` (UGM-109) | booster Mk 106 acceso sott'acqua: esce gia' spinto a ~12 m/s | 12 s di fumo, 55→10 gradi, 25→150 m/s, fino a ~470 m; sgancio del booster, crociera a 30 m e 220 m/s |
  | `granit` (SS-N-19, Oscar) | contenitore inclinato, 15 m/s | booster 4 s a 60 gradi fino a 250 m/s, poi salita senza fumo a 600 m/s verso la quota di crociera (oltre 6 km in 25 s) |
  | `malakhit`, `ametist` (SS-N-9, SS-N-7, Charlie) | lancio umido a 38,5 gradi | boost di 7 s a 40→5 gradi, crociera a 60 m e 300 m/s |
  | `vodopad` (SS-N-15/16, missili-siluro) | 20 m/s, ripido | 6 s di fumo a 65→45 gradi, poi la parabola in alto |

- **Piuma:** `Effetti::piuma(id, pos, rotta, velocita, altezza, dt)`, da chiamare a ogni passo per ogni albero
  alzato.
  - Davanti all'albero c'e' uno spruzzo alto circa `1,2 v^2/2g` (STIMA dall'onda di prua di un cilindro): 0,1 m a
    3 nodi, 1 m a 8, 3 m a 15.
  - Dietro resta una scia di schiuma lunga `v (4 + 1,5 v)` m: circa 10 m a 3 nodi, 40 m a 8, 120 m a 15.
  - Coerenza con il dossier 02 §7.7: la piuma si vede sopra 5-8 nodi, e da 8 nodi in su la scia e' abbastanza
    lunga per un elicottero.
- **Agganci in `main.cpp`** (solo con la regola di simulazione):
  - per i sottomarini dell'IA con periscopio o antenna alzati (+54, bit 02h e 04h) e quota fino a 20 m si chiama
    `piuma`;
  - il punto dell'albero e' il punto piu' alto del modello 3D della classe: Victor III +12,7 m dal centro, 688
    +22 m (periscopio d'attacco), Oscar +37 m;
  - altezza 2 m per il periscopio, 3 m per l'antenna.
  - **Lanci:** `lancio_missile` (opcode 5Ah) registra anche un `LancioMissile` in `Missione::lanci_missili()`, con
    chi lancia, posizione, quota e rotta verso il bersaglio. Per il giocatore c'e' anche l'arma: la voce di `armi`
    il cui gestore sta girando (`Contesto::gestore`), "harpoon" o "tomahawk". Il livello classico non ne crea
    unita', quindi il rilievo resta identico.
  - Per ogni lancio nuovo da un sottomarino `main.cpp` chiama `lancio_missile`. Il tipo e' l'arma del giocatore;
    per l'IA dipende dalla classe: Oscar → granit, Charlie II → malakhit, Charlie → ametist, 688/Sturgeon/
    Trafalgar → harpoon, gli altri → vodopad.
  - I lanci dalle navi per ora non hanno effetto.
  - Caso reale: in SCENE1B con il seme 2 il 688 alleato (unita' 7) lancia due missili da 6,1 m a 466 e 468 s.
- **Battelli immersi al periscopio:** prima `Missione::sincronizza` metteva nel mondo anche le unita' immerse, e il
  periscopio le disegnava a galla (un Victor III a 100 m si vedeva intero). Ora `Nave` ha `quota` e
  `alberi_alzati`, riempiti da `sincronizza` con la quota e i bit 02h e 04h di +54. `periscopio.cpp` decide cosi':
  - fino a 0,5 m il battello si disegna come in superficie;
  - fino a 3 m il modello 3D affonda gradualmente;
  - fino a 20 m escono solo gli alberi alzati, con il punto piu' alto del modello 2,5 m sul mare (STIMA);
  - altrimenti, e con gli alberi giu', non si vede niente. Vale anche per siluri e battelli profondi.
- **Verifica a vista** (`extracted/navi/_verifica/`), Victor III a 600 m:
  - `lancio_harpoon_sequenza.png`: lancio da 30 m; capsula, booster e getto di fumo che resta;
  - `lancio_tomahawk_granit.png`: colonne di fumo dei due booster;
  - `piuma_periscopio.png`: 4, 8 e 15 nodi; a 4 nodi solo l'albero, a 15 un pennacchio e la scia.
- **Prove:**
  - `effetti: missili lanciati da un sottomarino in immersione`: tempi di uscita, tappi, fiammata, quote del fumo e
    del corpo, durata;
  - `effetti: piuma del periscopio che cresce con la velocita'`: altezza dello spruzzo e lunghezza della scia a 3,
    8 e 15 nodi; niente da fermo.
  - `missioni: lanci di missili registrati per gli effetti`:
    - l'Harpoon dal tubo 7 di SCENE1A ha arma, bersaglio e rotta giusti;
    - il lancio dello script del 688 di SCENE1B non ha arma.
  - `periscopio: sottomarini immersi, solo gli alberi alzati`: Victor III a 600 m. In superficie 15541 pixel; a
    6 m con gli alberi alzati 28 pixel, cioe' solo gli alberi; a 1,5 m mezzo scafo; con gli alberi giu' e a 100 m
    niente.

### 10.16 Quadro danni dell'Enhanced: spaccato del battello (24/10/2026)

**Fatto, lato grafica** (`grafica/spaccato.{hpp,cpp}`; l'integrazione in `disegna_danni` la fa la sessione
principale). È lo spaccato del classico (`SUB1`: prua a destra, interni ciano, macchinari chiari, elica rossa), più
dettagliato e con i colori a rampe.

**Profilo e interni:**
- il profilo è quello vero: i triangoli del modello 3D proiettati sul piano x-z danno scafo, vela, timoni, elica in
  bronzo e alberi;
- lo scafo resistente (`scafi/<classe>.toml`) è tagliato con il bordo chiaro; fra i due scafi ci sono le casse;
- interni per classe:
  - 688 e Alfa a mano: ponti e paratie con le porte stagne; sfera BQQ-5 o schiera di prua; tubi e rastrelliere di
    siluri; sonar, centrale, radio, alloggi e mensa; ausiliari; reattore con generatori di vapore e galleria; turbine,
    condensatore, riduttore, turbogeneratori e asse; verricello della schiera; capsula dell'Alfa;
  - le altre classi (Sturgeon, Trafalgar) con un allestimento generico dagli usi dei compartimenti.

**Stato dei danni** (`StatoSpaccato::da(BattelloDanni, quota, assetto, tempo)`):
- acqua per compartimento come segmento di cerchio con la frazione d'acqua del kit, inclinata con l'assetto, con il
  pelo increspato e le caustiche; macchinari sommersi;
- falle con lo squarcio, il getto e le bolle;
- fiamme sul pavimento più basso sopra l'acqua, col bagliore attorno ai focolai;
- fumo dal soffitto alto quanto il fumo del kit;
- compartimenti sventrati (bruciati, scafo aperto) e implosi (schiacciati);
- paratie cedute;
- casse con l'aria e le bolle dell'esaurimento;
- luci d'emergenza nel reattore dopo lo SCRAM;
- sistemi guasti col contorno che pulsa in rosso (l'evidenziazione del classico);
- **squadre di controllo danni** (`BattelloDanni::squadre()`): figurine blu sulle falle e alle riparazioni, ambra
  con maschera, lancia e getto nebulizzato agli incendi; manichetta gialla delle pompe.

**Vista e palette:**
- quadro intero, didascalie per compartimento (nome, barra dell'acqua, stato) e dettaglio ingrandito di un
  compartimento (`disegna_dettaglio`);
- palette: 15 rampe negli indici 96–143 e 199–247 (`Spaccato::imposta_palette`, solo con F3 aperto), retinatura di
  Bayer.

**Squadre nel kit** (`SquadraBattello`: compito tamponare, spegnere, riparare o pompare; compartimento; numero; falla
o sistema; portata delle pompe):
- registrate a ogni passo da `BattelloDanni::equipaggio`, con `squadre_disponibili()`;
- pompare non toglie squadre (le pompe sono macchine);
- la simulazione non cambia.

**Costo:** il disegno di base va in cache per rettangolo e vista (35 ms la prima volta); poi intero più dettaglio
costano 3,1 ms a fotogramma a battello intatto e 4,1 ms con un incendio.

Anteprime: `engine/<cartella di build>/anteprima_spaccato <dati> <classe> <integro|danneggiato|grave> <uscita.bmp>`, in
`extracted/navi/_verifica/spaccato/`.

Prove:
- `test/prove_danni_sottomarini.cpp`: palette, profilo, acqua nel compartimento giusto, fuoco e bagliore, squadre,
  contorno dei guasti, determinismo e dettaglio;
- `test/prove_battello.cpp`: squadre su falla e incendio, pompe senza squadre, riparazioni dopo un urto.

### 10.17 Letalità dei siluri sui sottomarini (25/10/2026)

Verifica chiesta dalla taratura dell'ASW aereo. Un colpo di Mk 46 o di AT-1 non affondava né un Victor III né un
Foxtrot entro 4 ore, e nemmeno due colpi bastavano.

**Causa** (`missioni/src/danni_sottomarini.cpp`, `DanniSottomarini::colpisci`):
- con la geometria vera la distanza dello scoppio era quella dal **centro** della vittima meno la mezza larghezza;
- la spoletta del modello scatta a 10-15 m dal centro (siluri.toml `spoletta_m` 5-8, più 5 m sui sottomarini; `nucleo/siluro.cpp`), che su un battello di 70-110 m è spesso a contatto della prua o della poppa;
- il calcolo dava invece 5-35 m dallo scafo: fattore d'urto 0,07-0,4, solo scosse e falle da 1-2";
- le teste contavano in kg di esplosivo e non in TNT equivalente: il Mk 46 44 kg invece di 108, il Mk 48 295 invece di 544. L'AT-1 aveva la testa del Mk 46.

**Correzione:**
- **Riferimento della vittima:** lo scoppio si porta lungo l'asse, di traverso e in quota. La distanza è quella da un cilindro lungo quanto lo scafo, e il punto lungo lo scafo è quello dello scoppio, non più a caso. I colpi senza una posizione coerente con la distanza registrata restano come prima.
- **Doppio scafo:** l'intercapedine fra lo scafo leggero e quello resistente (`[sottomarino] intercapedine_m`) si aggiunge alla distanza. Victor III 0,82 m, Alfa 1,0, Foxtrot 1,04, Kilo 1,21; 0 sui 688.
- **Teste in TNT equivalente:** Mk 48 544 kg (295 di PBXN-103) e Mk 46 108 kg (44 di PBXN-103), dossier 07 §1.5, [NW-US], [Brain16]. AT-1 105 kg: 70 kg di miscela alluminizzata × 1,5, STIMA (nessuna tabella pubblica, 07 §13 punto 3).
- La spoletta che misura la distanza dal centro è del runtime (`nucleo/siluro.cpp`). Il calcolo dei danni ora regge comunque; la distanza dallo scafo nella spoletta è passata al runtime.

**Misure** (`engine/<cartella di build>/letalita <dati> <arma> victor3,alfa <modo> 12 4`):
- un colpo, 12 semi, poi 4 ore di missione con la regola di simulazione;
- vittima in moto a 5 nodi;
- «fuori combattimento» è `DanniSottomarini::menomato` o perso.

| Arma e geometria | Fattore d'urto | Victor III 50 m | 150 m | 250 m | Alfa 50 m | 150 m | 250 m |
|---|---|---|---|---|---|---|---|
| Mk 46 a contatto | 1,6–2,3 | 11 persi, 1 emerso | 12 persi | 12 persi | 11 persi, 1 emerso | 12 persi | 12 persi |
| AT-1 a contatto | 1,6–2,2 | come il Mk 46 | | | | | |
| Mk 46 di traverso a 7 m | 0,31–0,43 | 5 persi, 7 emersi | 12 persi | 12 persi | 5 persi, 7 emersi | 12 persi | 12 persi |
| Mk 48 a contatto | 4,2–5,1 | come il Mk 46 a contatto | | | | | |
| Mk 48 6 m sotto la chiglia | 1,8–2,3 | come il Mk 46 a contatto | | | | | |

- **Tempi alla perdita:**
  - a contatto 1,5-3 minuti a 50 m, meno di un minuto e mezzo in profondità (l'Alfa, più piccolo, in 40-90 s);
  - di traverso a 7 m 4-12 minuti: le falle sono di 2-5" in 1-2 compartimenti, con le casse vicine squarciate.
- Fuori combattimento sempre, in tutte le prove.
- **Con la spoletta del modello** (scoppio a 12 m dal centro, da una direzione a caso; 12 semi):
  - a 50 m Victor III 7 persi e 5 emersi, Foxtrot 5 persi, 4 emersi e 3 rimasti in quota con piccole falle;
  - a 150 e 250 m tutti persi.
- **Prima della correzione:** nessun affondamento in 4 ore.

**Confronto con le fonti:**
- **P di uccisione:** il dossier 17 §6.7 dà per il Mk 46 0,5-0,7 per lancio contro un SSN non allertato, cioè 0,7-0,9 col colpo a segno. Qui, col colpo, il fuori combattimento è certo e l'affondamento va dal 40-90% vicino alla superficie al 100% sotto i 150 m. È nella forbice per un bersaglio in profondità, un po' sopra per uno vicino alla superficie.
- **Doppio scafo:** dà il 25-45% di riserva di spinta contro il 9-15% di uno scafo singolo e protegge dalle armi (USNI Proceedings, aprile 1991). Qui conta l'intercapedine: con 3-4 m (Oscar, Typhoon) un Mk 46 a contatto resta sotto la soglia di rottura. Il Mk 50 nasce proprio contro i sottomarini «veloci, profondi, a doppio scafo» (Wikipedia: Mark 50; StrategyPage 2004).
- **Foxtrot e Victor:** il Foxtrot è anch'esso a doppio scafo, ma più vulnerabile. Ha riserva 0,27 contro 0,47, collaudo a 280 m contro 600 (la soglia dell'urto si riduce prima) e si perde prima.

**La leva dei sopravvissuti in profondità:**
- è la regola del dossier 07 §8.4: emersione d'emergenza riuscita al 30-70% solo sopra i 150 m e con falle sotto i 10 cm;
- sotto i 150 m decide quasi da sola che il battello colpito vada perso;
- la fonte è **[bassa]** (un solo caso che la sfiora, il *San Francisco* a ~160 m): resta com'è finché non ce n'è una migliore.

**Limite noto:**
- oltre la soglia di rottura (fattore d'urto 0,6-1) la falla dello scafo resistente è di 0,3-1 m², qualunque sia la testa;
- così un Mk 48 a contatto (fattore d'urto 5) fa quanto un Mk 46 (2), e a 50 m un battello su 12 riemerge anche dopo un Mk 48;
- il dossier 07 §11.3 dà per le lamiere delle navi una falla di diametro `1,25·W^(1/3)`. Scalare l'area della falla con `W^(2/3)` (rispetto ai 108 kg del Mk 46) darebbe al Mk 48 una falla tre volte più grande;
- da fare solo con una fonte sullo scafo resistente, che è molto più spesso del fasciame delle navi.

Prove:
- `test/prove_danni_sottomarini.cpp`: Mk 46 a contatto della poppa di un Kilo, con la distanza dallo scafo e non dal centro; Mk 48 e mina a influenza con l'intercapedine;
- classico identico (rilievo delle 110 partite).

### 10.18 Le casse dell'Alfa: ПОКС e Tan (26/10/2026)

Dossier 19 §3.1-3.2 e §6.2; proposta della sessione dell'acustica (nota condivisa `alfa-boksit-tan-poks-akkord-proposta.md`).
Solo nell'Enhanced e solo per l'Alfa del giocatore; gli altri battelli restano neutri come prima.

- **SFOGA** (`BattelloDanni::sfoga`): ferma l'esaurimento e apre gli sfoghi delle casse principali. Una cassa piena d'aria
  si svuota in 15 s (STIMA 10-20 s), poi gli sfoghi si richiudono.
- **COMPENSA** (`compensa(m3)`): ±0,5 m³ a pulsante sull'obiettivo delle casse d'assetto, alle portate di sempre
  (valvole 0,05 m³/s, pompe 0,03 m³/s fino a 200 m). Sotto il neutro si espelle la riserva delle casse di
  compensazione, fino a 15 m³ (STIMA).
- **Peso variabile** (`varia_peso`, `abilita_compressione`):
  - ogni arma che esce dal tubo alleggerisce la prua: 1,9 t un siluro, 0,3 t un'esca, 1 t una mina (STIMA);
  - lo scafo si comprime con la quota, 0,5 t ogni 100 m (STIMA: qualche tonnellata a 600 m). Conta dalla quota di
    partenza, dove l'equipaggio ha messo il battello in assetto: sotto pesa di più, sopra di meno.
- **Tan** (`tan`): porta l'obiettivo delle casse d'assetto a compensare peso variabile e acqua imbarcata, entro la
  riserva. Un allagamento grande lo vince. Si disinserisce da solo con le pompe sotto il 50%.
- **Guasto delle casse** (`guasto_casse`, la finestrella rossa): aria o pompe sotto il 50%, o una cassa squarciata.
- **Ordini**: `DanniSottomarini::consegna` prende `sfoga`, `compensa` (a = m³) e `tan` (n = 1/0) del giocatore. Rifiuta
  COMPENSA con Tan inserito, perché compensa lui, e Tan con le pompe sotto il 50%.
- **Messaggi**: 443 Tan inserito, 444 disinserito, 445 fuori servizio, 446 avaria alle casse, 449 sfoghi aperti.

Prove: `test/prove_danni_sottomarini.cpp`, i due casi «Alfa, casse».

### 10.19 Priorita' delle squadre e isolamento di un compartimento (06/11/2026)

Proposta con le misure e le fonti: nota condivisa `punto4-isolamento-proposta-06-11.md`; misure in `tools/re/prove/isola/`. Solo nell'Enhanced e
solo il giocatore: l'IA non isola, limite scritto per decisione dell'utente del 06/11/2026 (misura e motivo in docs/18, Stato). Una partita che non usa questi ordini resta identica.

- **Perche' serve:** un incendio esteso le squadre non lo accorciano (688 macchine, fuoco 0,8: 1656 s con le squadre, 1618 s senza), ma ci restano tutte e
  nessuno ripara altro. Uno piccolo lo vincono (66 s contro 1967): isolare troppo presto e' un errore.
- **`BattelloDanni::isola`**: le squadre escono subito e il compartimento non conta piu' come abitabile; le porte si chiudono dopo 60 s (STIMA). `chiudi_subito`,
  durante il conto: chi non e' uscito resta dentro. `riapri`: chi e' dentro torna fuori; a fuoco vivo l'ossigeno risale al 16% (STIMA) e il fumo si mescola coi vicini.
- **A porte chiuse:** nessuna squadra, ne' tamponatura ne' riparazioni; il fuoco consuma l'ossigeno come prima; contagio ai vicini a un quarto (STIMA); il ramo
  dell'aria si chiude, se non e' gia' rotto; fumo e ossigeno non si ripristinano; le pompe continuano. **Paratie, falle e pressione non cambiano: contro l'acqua
  isolare non ferma niente.**
- **Il posto di comando non si isola** (`compartimento_di(Sistema::inerziale)`: C1 del 688, III dell'Alfa), rifiutato col messaggio 418.
- **688 e Alfa:** sul 688 19 uomini in macchine, nessuno nel reattore (STIMA); le macchine chiuse limitano il motore al 15%. Sull'Alfa (`senza_personale`,
  dossier 19 §3.1-3.2) i compartimenti tecnici si chiudono subito, nessuno dentro, nessun limite.
- **Chi resta dentro** muore quando il compartimento diventa letale (fumo pieno, ossigeno sotto il 12%, acqua oltre meta'), e conta in `EsitoDettagliato::morti`:
  sono i soli caduti che il modello conosce, perche' l'equipaggio uomo per uomo non c'e'.
- **Ordini:** `isola_compartimento` (n compartimento, a: 0 sgombero, 1 chiude subito, 2 riapre), `priorita_sistema` (n: Sistema). Messaggi 415 sgombero, 416 isolato,
  417 riaperto, 418 rifiuto del posto di comando, 419 uomini perduti, 468 rifiuto di un compartimento gia' perduto.
- **Stazione F3:** 1 agisce sul compartimento mostrato, 5 apre la pagina delle squadre (`grafica/pagina_squadre`, disegno del kit), e li' 2 e 3 scelgono un
  sistema, 4 lo porta in cima alle riparazioni.

Prove: `test/prove_battello.cpp`, «isolare un compartimento»; `test/prove_danni_sottomarini.cpp`, «isolare un compartimento».
