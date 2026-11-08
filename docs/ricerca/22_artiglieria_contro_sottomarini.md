# 22 — Artiglieria navale contro sottomarini (dossier per il remake di *688 Attack Sub*)

> Ambientazione: **1989**. Domanda: è realistico che le navi di superficie **sparino col cannone** al nostro battello
> quando lo scoprono **in superficie** o **a quota periscopica** (periscopio o albero fuori)? Vale per entrambi i lati:
> le navi NATO contro l'Alfa, quelle sovietiche contro il 688.
> Scopo: dottrina e casi storici; i cannoni delle classi di `classi.toml`; i tempi dalla scoperta al primo colpo; gli
> effetti sul battello; le colonne d'acqua viste al periscopio; che cosa si sente al sonar e dentro lo scafo; una
> proposta di gioco.
> Compilato: 25 ottobre 2026, da una sola ricerca, con calcoli propri. Gli script di calcolo non sono stati conservati
> (stavano nello scratchpad volatile); le formule e i parametri sono riportati nei §3.4, §5.2, §6.1 e §7.1.

---

## 0. Convenzioni e punto di partenza

**Confidenza.**
- **alta:** fonte primaria o ufficiale (rapporto di guerra del Bureau of Ships, rapporto OEG, documento NHHC, NavWeaps
  quando cita la scheda ufficiale);
- **media:** fonte secondaria seria (Wikipedia con fonti, NavWeaps su armi sovietiche, sintesi di un EIS della US Navy
  lette solo di seconda mano);
- **bassa:** forum, testimonianze isolate, estrapolazioni fuori dal campo di validità;
- **n.v.:** non trovato.

**Marcature:** **STIMA** = numero proposto, da tarare; **[GIOCO]** = proposta; **[CALC]** = calcolo da una formula
citata.

**Sigle delle fonti** tra parentesi quadre; la tabella con gli URL è al §11.

**Che cosa c'è già altrove** (non si ripete):
- dossier 07: fisica delle esplosioni subacquee (Cole, bolla, colonne di Swisdak), raggi letali delle cariche WWII;
- dossier 11 §0.2 e §5: orizzonte radar, sezione radar di periscopi e alberi, probabilità cumulativa di scoperta;
- dossier 17: ASW aereo, ROE di pace e di guerra per gli aerei, SUS Mk 84;
- dossier 18 §6: la funzione di trasferimento acqua → scafo → interno, K(f);
- dossier 20 §5 e §7: quanto sporge il periscopio, piuma, tempi d'esposizione;
- dossier 05: Phalanx e AK-630 come difesa antimissile.

**Il gioco originale.** Nessuna nave spara col cannone al sottomarino. Il manuale elenca i cannoni nelle schede delle
classi, ma li descrive solo come difesa contro missili e siluri: «If missile defense is unsuccessful, guns will be
used» `[MAN688]`. Gli script dell'IA delle navi ASW non hanno alcun attacco d'artiglieria: «fanno comparire un siluro
da 450 mm vicino al bersaglio» per rappresentare ASROC e SS-N-14 e usano un `ATTACK` a 300 di portata contro i siluri
in arrivo `[IA-ORIG]`. L'emersione d'emergenza esiste, ma in superficie si rischia solo di essere visti dal radar. È
quindi una **novità dell'Enhanced**; il classico resta identico.

---

## 1. Sintesi

1. **Sì, è realistico, ma solo in guerra e solo in due situazioni.**
   - **Battello in superficie** entro la portata dei cannoni: qui il cannone è **l'arma principale**, perché i siluri
     leggeri e le armi ASW sono fatti per bersagli immersi. Lo dice già il rapporto OEG del 1946: le cariche servono a
     costringere il sottomarino a emergere, «dove può essere affondato col cannone o con lo sperone» `[OEG51]`.
   - **Periscopio o albero visti a distanza ravvicinata** (entro 2-5 km) da una nave già in allarme: si apre il fuoco
     **insieme** all'attacco ASW, per accecare il battello e costringerlo a scendere. Precedenti: USS *Ward* 1941, che
     colpisce la falsa torre di un sommergibile tascabile semi-immerso `[WP-Ward]`; la dottrina USA del 1917, per la
     quale distruggere i periscopi rende il battello «inoperativo, salvo in superficie» `[NHHC17]`.
2. **In pace e in crisi no**, salvo i **colpi d'avvertimento** e i **segnali esplosivi** non letali: Cuba 1962 con le
   bombe di profondità da esercitazione `[NSA75]` e forse i traccianti di un aereo `[WP-B59]`; la Svezia, con i colpi
   d'avvertimento degli elicotteri nel 1980 e le bombe di profondità nel 1982-84 `[WP-SWE]`. Non risulta alcun caso
   della Guerra Fredda di **cannonate contro un periscopio**.
3. **Il cannone non raggiunge un battello immerso.** I proietti esplodono all'impatto con l'acqua, rimbalzano se
   arrivano radenti, oppure si capovolgono e si fermano in pochi metri `[Okun]` `[NG-S3]`. Uno scoppio da 3,5 kg in
   superficie (5" o 130 mm) ha un raggio d'avaria contro uno scafo resistente di **circa 3 m** [CALC]. **Lo scafo di un
   battello a quota periscopica è al sicuro.** In pericolo restano solo **le teste dei periscopi, gli alberi e la parte
   alta della falsa torre**.
4. **In superficie il battello è vulnerabile, ma non è una nave.** Nella Seconda guerra mondiale un 5" inesploso
   non perforò lo scafo resistente del *Nautilus*; un 5,5" sulle guide dei periscopi del *Rock* ne frantumò i prismi;
   un 8" inesploso aprì lo scafo del *Bergall* `[WDR58]`. Per il gioco: alberi e falsa torre fuori uso in pochi colpi;
   lo scafo si danneggia, ma **di solito ci si può ancora immergere**.
5. **I cannoni in gioco:**
   - USA: Mk 75 da 76 mm (Perry), Mk 45 da 5" (Spruance, Kidd), 5"/38 e 16"/50 (Iowa);
   - URSS: AK-100 (Krivak II, Kirov), AK-130 (Slava), AK-726 (Kara, Koni), AK-725 da 57 mm (Nanuchka I), AK-630 e
     AK-230 da 30 mm.

   Il **Phalanx del 1989 non ha un modo contro bersagli di superficie**: arriva solo col Block 1B, nel 1999 `[WP-Phal]`.
   L'**AK-630** invece è dato anche contro «forze leggere di superficie» fino a ~5 km `[RU-AK630]`.
6. **Effetti visivi:**
   - colonne d'acqua di ~2-5 m per il 30 mm, ~10-15 m per il 76 mm, ~20-30 m per il 5" e il 130 mm, ~60-100 m per il
     16" (STIMA da [CALC]), che durano pochi secondi;
   - a corta distanza i colpi corti **rimbalzano** e fanno una fila di spruzzi bassi;
   - di notte: vampa di bocca, illuminanti da ~50 s `[NW-538]` e traccianti dell'AK-630.
7. **Acustica:**
   - lo scoppio di un proietto si sente al sonar **a molti km**, come una carica SUS: è un **«crack» secco**, non il
     tonfo profondo di una bomba di profondità, perché la superficie cancella le basse frequenze [CALC];
   - **dentro lo scafo** immerso si sente a orecchio solo entro **~1-2 km**;
   - in superficie, in plancia, si sente il colpo di partenza: a 5 km la colonna d'acqua si vede **~7 s prima** che
     arrivi il boato.

---

## 2. Dottrina e casi storici

### 2.1 Fino al 1945: il cannone finisce il lavoro

- **OEG Report 51** (Sternhell e Thorndike, 1946), cap. 11 «Attacks by Surface Craft»:
  - l'azione in superficie contro un sottomarino «è poco diversa da qualunque altra azione di superficie» e il
    rapporto non la analizza;
  - il compito delle cariche è danneggiare lo scafo abbastanza da far emergere il battello, «dove può essere affondato
    col cannone o con lo sperone» `[OEG51]` (alta).
- **1917-18.** Il comando della Nantucket Division Patrol Force ordina di **non speronare** se si è visto solo un
  periscopio: i tedeschi mettevano finti periscopi sulle mine. Aggiunge che un battello può essere reso «inoperativo,
  salvo in superficie, distruggendone i periscopi o crivellandone la falsa torre» `[NHHC17]` (alta).
- **Rischio del fuoco libero.** Una lettera a *Proceedings* del 1941 ricorda che autorizzare i serventi a sparare
  subito su ogni periscopio, provato nel 1917-18, si rivelò pericoloso soprattutto **per i cacciatorpediniere della
  scorta** `[USNI41]` (media). Per il gioco: il tiro al periscopio dentro un convoglio rischia il fuoco amico.
- **USS *Ward*, 7 dicembre 1941** (alta per i fatti):
  - le vedette vedono un periscopio che segue la nave *Antares*; il *Ward* apre il fuoco con i 4"/50;
  - il pezzo n. 3 colpisce la falsa torre di un sommergibile tascabile Ko-hyoteki; poi il *Ward* lancia le bombe di
    profondità;
  - nel 2002 il relitto mostra **un solo foro di proietto** sul fianco destro della falsa torre; il battello affondò
    per l'acqua entrata da lì, mentre le bombe non fecero danni strutturali apparenti `[WP-Ward]`.

  È il caso tipico: un bersaglio quasi in superficie, a distanza brevissima, colpito di punteria diretta.
- **Danni da cannone sui sommergibili USA** (Bureau of Ships, *War Damage Report 58*, appendice I) `[WDR58]` (alta):

| Battello, data | Proietto | Dove | Effetto |
|---|---|---|---|
| *Grampus*, 5/1942 | 3" o 4,7" | parte poppiera della carenatura della falsa torre | resta in pattugliamento |
| *Nautilus*, 11/1943 | 5", inesploso, più un 5" vicino | sovrastruttura dietro la falsa torre | «lo scafo resistente non fu perforato»; allagamento interno da una valvola del portavoce; resta in pattugliamento |
| *Bowfin*, 11/1943 | 3" o 4,7" | sovrastruttura | scafo non perforato; schegge nelle tubature; tappati i fori, può navigare immerso; rientra |
| *Rock*, 2/1944 | 5,5" | guide dei periscopi, del radar, carenatura | **«i prismi superiori di entrambi i periscopi andarono in frantumi»**; scafo non perforato; rientra |
| *Bergall*, 12/1944 | 8", inesploso | struttura del portello di caricamento siluri prodiero | **squarcio nello scafo resistente**; rientra in superficie |

  Nello stesso rapporto le bombe d'aereo vicine allagano spesso i periscopi (*Trout*, *Stingray*, *Runner*, *Tuna*).
  La **testa del periscopio è il punto più fragile** del battello.

### 2.2 Il dopoguerra USA e NATO: il cannone diventa marginale

- Le armi ASW della Guerra Fredda (ASROC, siluri Mk 46 dai tubi Mk 32, elicotteri) sono fatte per un bersaglio
  immerso. In superficie il battello torna un bersaglio da cannone o da missile antinave.
- **Proietti antisommergibile per cannone.** Ci furono due tentativi, entrambi rimasti senza seguito:
  - l'**EX-30** per il 5"/38: proietto lungo e stabilizzato da alette, da 34 kg, a 90-150 m/s, contro sommergibili
    entro 2000 yd. Le prove del 1952 riuscirono, ma lo sviluppo si fermò `[NW-538]` (alta);
  - nel **1986** fu autorizzato un progetto per dare una funzione antisommergibile ad alcuni proietti HE-ET/PT Mk 145
    da 16". Non se ne sanno i dettagli; **alcuni furono sparati dall'*Iowa* nel 1989** `[NW-1650]` (media).

  Per il gioco basta una nota d'enciclopedia: nessuno dei due ha un effetto pratico nel 1989.
- **Phalanx.** Il Block 0 ha una «capacità marginale contro bersagli di superficie»; il modo di superficie (Block 1B
  PSuM, con FLIR e puntamento dell'operatore) è del **1999** `[WP-Phal]` (media). Nel 1989 il Phalanx **non spara a un
  periscopio**.
- **Lezione delle Falkland, 25 aprile 1982:** l'ARA *Santa Fe*, sorpreso in superficie, fu attaccato **solo dagli
  elicotteri**:
  - bombe di profondità del Wessex;
  - un Mk 46 del Lynx, che manca il bersaglio;
  - mitragliatrici da 7,62 mm;
  - missili AS.12 dei Wasp, che **passano da parte a parte** la falsa torre in vetroresina.

  Le navi (*Antrim*, *Plymouth*) usarono i cannoni solo per intimare la resa alla guarnigione `[WP-SF]` (media).
  Lezione: nel 1982 un battello in superficie vicino a un gruppo navale viene attaccato da tutto ciò che è più rapido;
  il cannone serve se la nave è la più vicina.

### 2.3 La dottrina sovietica

- **Fonti aperte: n.v.** Non si è trovato un testo sovietico (regolamento o manuale di tiro) sul tiro al periscopio o
  al sottomarino in superficie. Il regolamento di combattimento del 1983, tradotto dalla CIA (dossier 17
  `[CIA-REG86]`), non è stato riletto per questa voce.
- **Indizi dalle armi:**
  - l'AK-726, dei Kara e dei Koni, «poteva essere usato anche contro bersagli navali fino a 8,2 km» `[NW-726]` (media);
  - l'AK-630 è dato contro «forze leggere di superficie» fino a ~5 km `[RU-AK630]` (media).
  - Un periscopio o un sottomarino in superficie sono, per la direzione del tiro, un bersaglio di superficie piccolo.
- **Inferenza (media):**
  - in guerra una nave sovietica si comporta come una NATO: cannone contro il battello in superficie e contro il
    periscopio a corta distanza, insieme alle armi ASW (RBU-6000, siluri, SS-N-14);
  - in pace segue il quadro politico.
- **Legge sovietica sul confine di Stato** del 24/11/1982 (Ведомости ВС СССР 1982, n. 48, art. 891): disciplina l'uso
  delle armi contro chi viola il confine. Il testo degli articoli non è stato letto (**n.v.**).

### 2.4 Pace, crisi, guerra: le regole d'ingaggio

| Situazione | Che cosa è plausibile | Precedente | Conf. |
|---|---|---|---|
| **Pace, acque internazionali** | Niente fuoco. Pedinamento, sonar attivo, segnali. Sparare su un periscopio è un atto di guerra | Prassi della Guerra Fredda; dossier 17 §5.5 | media |
| **Pace, acque territoriali** | La UNCLOS (art. 20) chiede ai sottomarini in mare territoriale di navigare **in superficie e con la bandiera**. Lo Stato costiero può reagire: avvertimenti, poi bombe e mine | Svezia: colpi d'avvertimento degli elicotteri presso Utö (1980); 44 bombe di profondità e 4 mine a Hårsfjärden (1982); bombe a Karlskrona (1983-84) `[WP-SWE]`. Regolamento IKFN 1982:756; dal 1983 ordini di «sparare per uccidere» secondo la stampa `[CSM83]`; nelle acque interne il comandante supremo può ordinare l'attacco senza preavviso `[USNI91]` | alta (fatti), media (regole) |
| **Crisi** | Segnali esplosivi non letali per far emergere il battello; colpi d'avvertimento, luci, illuminanti | Cuba 1962: «quattro o cinque segnali sonori esplosivi innocui» (bombe da esercitazione grandi come bombe a mano) per far emergere i battelli; i sovietici dicono di non aver mai ricevuto il messaggio `[NSA75]`. Sul B-59 emerso un ufficiale ricorda un aereo che «sparò traccianti davanti al battello»; altre fonti parlano di lampi per le fotografie `[WP-B59]` | alta (bombe), bassa (traccianti) |
| **Guerra** | Cannone contro il battello in superficie entro la portata; contro il periscopio a corta distanza; insieme alle armi ASW | §2.1; Falkland 1982 `[WP-SF]` | media |

### 2.5 Cannone o arma ASW? [sintesi, media]

| Situazione | Arma preferita | Ruolo del cannone |
|---|---|---|
| Battello immerso sotto ~15 m | ASROC, Mk 46, RBU, SS-N-14, elicottero | nessuno |
| Periscopio visto **oltre ~5 km** | Elicottero, ASROC o SS-N-14 sul datum | nessuno: il colpo isolato a quella distanza è sprecato e rivela la nave |
| Periscopio visto **entro ~2-5 km** | Arma ASW sul punto **più** cannone sulla testa del periscopio | accecare e far scendere il battello; segnare il punto con le colonne d'acqua per gli altri |
| Periscopio **entro ~1 km** | Cannone, AK-630, RBU-6000 a tiro diretto; speronamento solo se c'è il battello sotto | arma principale nei primi secondi |
| Battello **in superficie** | **Cannone** (e missili antinave se la distanza è grande) | arma principale: i siluri leggeri e le bombe sono fatti per l'immerso |
| Battello danneggiato che emerge | Cannone | finire o costringere alla resa (OEG 51) |
| Crisi o pace | Segnali (bombe da esercitazione, SUS), colpi d'avvertimento lontani dal bersaglio | solo avvertimento |

---

## 3. I cannoni delle classi in gioco

### 3.1 Tabella per classe (dotazione 1989)

Le dotazioni vengono dal dossier 01 §6, i dati dei cannoni da NavWeaps. Le distanze «efficaci» contro una nave sono
quelle delle schede; contro un periscopio valgono molto meno (§3.6).

| Classe (lato) | Cannone | Direzione del tiro | Cadenza | Gittata max / efficace contro nave | Elevazione | Proietto / carica | Fonte | Conf. |
|---|---|---|---|---|---|---|---|---|
| **Perry** FFG-7 (USA) | 1 × Mk 75 76/62 (OTO Compact) | Mk 92 (CAS + STIR) | 80-85 colpi/min; 80 pronti nell'affusto, senza serventi | 18,4 km / **8 km** | −15/+85° | 6,3 kg; HE 0,75 kg | `[NW-Mk75]` | alta |
| **Spruance** DD-963, **Kidd** DDG-993 (USA) | 2 × Mk 45 5"/54 | Mk 86 (SPQ-9A per la superficie, SPG-60) | 20/min (16 col graduatore di spoletta) | 23,1 km / **15 km** | −15/+65°; brandeggio 30°/s, elevazione 20°/s | 31 kg; 3,5 kg di Comp A-3 | `[NW-Mk45]` | alta |
| **Iowa** BB (USA) | 6 × 2 5"/38 Mk 28; 9 × 16"/50 Mk 7 | Mk 37 (5"); Mk 38 e Mk 160 (16") | 15-22/min per canna (5"); 2/min (16") | 5": ~16-18 km; 16": 38 km | 5": −15/+85°; 16": **−2/+45°** (torre II 0°) | 5" AAC 25 kg, 3,3 kg; 16" HC 862 kg, **69,7 kg** | `[NW-538]` `[NW-1650]` | alta |
| **Nimitz** CVN, **Sacramento** AOE (USA) | Solo Phalanx (e Sea Sparrow) | — | — | — | — | — | dossier 01 | media |
| **Krivak II** 1135M (URSS) | 2 × 1 AK-100 100/70 | MR-184 «Lev» (Kite Screech), dossier 11 | 50-60/min | 21 km / n.v. (STIMA ~10 km) | −10/+85°, 30°/s | 15,6 kg; 1,53 kg; ZS-58P di prossimità | `[NW-AK100]` | media |
| **Kirov** 1144 (URSS) | 2 × 1 AK-100 (Kirov); 1 × 2 AK-130 (Frunze, Kalinin); 8 × AK-630 | Lev / MR-184; MR-123 Vympel (Bass Tilt) | come sopra | come sopra | come sopra | come sopra | dossier 01; `[NW-AK130]` | media |
| **Slava** 1164 (URSS) | 1 × 2 AK-130 130/70; 6 × AK-630 | MR-184 «Lev» | 10-40/min per canna | 23 km / n.v. (STIMA ~12 km) | −12/+80°, 25°/s; 150-180 colpi pronti | 33,4 kg; 3,56 kg | `[NW-AK130]` | media |
| **Kara** 1134B (URSS) | 2 × 2 AK-726 76/59; 4 × AK-630 | MR-105 «Turel» (Owl Screech) | 45/min per affusto; raffreddamento dopo 40-45 colpi | 15,7 km / **8,2 km** | a mano −5/+85°; idraulica **−2/+84°** | 5,9 kg; OF-62 0,4 kg; ZS-63 0,48 kg | `[NW-726]` | media |
| **Koni** 1159 (URSS) | 2 × 2 AK-726; 2 × 2 AK-230 30 mm | Hawk Screech / Drum Tilt (MR-104) | come sopra | come sopra | come sopra | come sopra | dossier 01, 11 | media |
| **Nanuchka I** 1234 (URSS) | 1 × 2 AK-725 57/75 | MR-103 «Bars» (Muff Cob) | 100-120/min cicl., 70 sost. | ~8 km / **6 km col radar, 4 km all'ottica** | −4/+85° (affusto terrestre; navale n.v.) | 2,85 kg; 0,154 kg HE-T | `[WP-AK725]` | media-bassa |
| **AK-630** (Kara, Kirov, Slava) | 6 canne da 30 mm | MR-123 Vympel; mira ottica di riserva (n.v.) | 4000-5000/min a raffiche | ~4 km aereo, **~5 km forze leggere di superficie** | −12/+88° | 0,39 kg; OF-84 48,5 g; OR-84 tracciante | `[RU-AK630]` `[NW-AK630]` | media |
| **Phalanx** Block 0/1 (Perry, Kidd, Iowa, Nimitz, Sacramento) | 20 mm, 3000-4500/min | radar proprio, solo aereo | — | **nessun modo di superficie nel 1989** | Block 0 −10°, Block 1 −20° | — | `[WP-Phal]` `[NW-Phal]` | media |

Note:
- **Spruance:** la dotazione 1989 ha i due Mk 45 come i Kidd (dossier 01).
- **Kirov:** nel 1989 sono in servizio *Kirov* (AK-100), *Frunze* e *Kalinin* (AK-130); `classi.toml` ha una sola
  classe, per cui si sceglie l'unità nella missione.
- **AK-726, elevazione −2° con l'idraulica:** è il dato NavWeaps, sorprendente; non incide molto sulle distanze (§3.3).
- **Mercantili, petroliere, portacontainer:** disarmati.

### 3.2 Il cannone «ultimo» contro il periscopio: CIWS e 30 mm

- **Phalanx:** escluso nel 1989 (§2.2). È **un'asimmetria reale**: il 688 vicino a una scorta USA non rischia il
  CIWS; l'Alfa vicino a un Kara o a uno Slava sì.
- **AK-630 e AK-230:** sono comandati a distanza dal radar MR-123 o MR-104. L'AK-630 è dato contro le forze leggere di
  superficie `[RU-AK630]`.
  - Contro un periscopio a 1-2 km una raffica di 1-3 s mette in aria centinaia di proietti da 30 mm.
  - Il bersaglio è minuscolo, ma la raffica copre un'area: per la testa di un periscopio basta una scheggia nel vetro.
  - **STIMA:** è l'arma più pericolosa per l'albero entro 2 km, se la nave l'ha agganciato al radar o a vista.
  - Documentazione d'uso reale contro un periscopio: **n.v.**

### 3.3 Depressione e distanza minima [CALC]

Distanza minima per colpire il pelo dell'acqua: `d_min = h / tan(depressione)`, con h l'altezza dell'asse del pezzo.
Rollio e beccheggio la cambiano di volta in volta.

| Pezzo | h (STIMA) | Depressione | d_min |
|---|---|---|---|
| Mk 75, Mk 45, AK-100, AK-130, 5"/38 | 6-10 m | −10/−15° | **25-55 m** |
| AK-726 (idraulica) | ~8 m | −2° | **~230 m** (−5° a mano: ~90 m) |
| AK-725 | ~6 m | −4° (n.v. per il navale) | ~85 m |
| AK-630 | 6-12 m | −12° | 30-55 m |
| 16"/50 (Iowa) | ~10-12 m | −2° | **~300-350 m** |

In pratica nessuno è cieco a distanze utili, tranne il 16". Del resto **nessuno sparerebbe il 16" a un periscopio**:
2 colpi al minuto e una dispersione da tiro a 30 km.

### 3.4 Balistica a corta distanza [CALC, media-bassa]

Integrazione numerica con resistenza quadratica (Cd ≈ 0,28 supersonico, 0,33-0,38 transonico, 0,20 subsonico),
pezzo a 8 m sull'acqua, aria a livello del mare. Va bene per l'ordine di grandezza e per i tempi; non sostituisce le
tavole di tiro.

| Distanza | Mk 75 76 mm: volo / angolo di caduta / v d'arrivo | Mk 45 5": come sopra | AK-130: come sopra | AK-725 57 mm: come sopra |
|---|---|---|---|---|
| 1 km | 1,1 s / 0,8° / 820 m/s | 1,3 s / 0,9° / 750 | 1,2 s / 0,9° / 790 | 1,1 s / 0,8° / 860 |
| 2 km | 2,4 s / 1,1° / 720 | 2,7 s / 1,3° / 700 | 2,5 s / 1,2° / 740 | 2,3 s / 1,1° / 740 |
| 3 km | 3,9 s / 1,8° / 640 | 4,1 s / 1,9° / 655 | 3,9 s / 1,7° / 690 | 3,8 s / 1,7° / 630 |
| 5 km | 7,5 s / 4,0° / 500 | 7,4 s / 3,6° / 570 | 7,0 s / 3,2° / 605 | 7,5 s / 4,3° / 460 |
| 8 km | 15 s / 12° / 320 | 13 s / 7,6° / 455 | 12,6 s / 6,7° / 490 | 16 s / 15° / 290 |
| 10 km | 22 s / 21° / 280 | 18 s / 12° / 390 | 17 s / 10° / 420 | 25 s / 27° / 250 |

Conseguenze:
- **Fino a 5-6 km il tiro è radente** (angolo di caduta sotto i 5°): i colpi corti toccano l'acqua quasi di piatto e
  **rimbalzano** (§5.1).
- **Spazio pericoloso** (*danger space*, la fascia di distanza in cui il proietto passa sotto l'altezza del bersaglio):
  per un albero alto 1,5 m a 3 km con caduta di 1,8° vale circa 48 m. L'errore in distanza conta poco; conta l'errore
  **in direzione** contro una testa larga 10-25 cm.
- **Il proietto è più veloce del suono fino a ~8 km** (con l'eccezione del 76 mm). Chi sta in plancia vede la colonna
  d'acqua **prima** di sentire il colpo di partenza (§6.5).

### 3.5 Spolette e proietti

- **USA, 5":** spolette a percussione (PD), a tempo meccanico (MT), di prossimità (VT, CVT), infrarosse; il CVT si arma
  solo pochi secondi prima del bersaglio, per ridurre gli scoppi prematuri da pioggia e mare `[NW-Mk45]` `[NW-538]`
  (alta).
- **Prossimità sopra l'acqua** (dati delle spolette VT dei 5"/38 della Seconda guerra mondiale) `[BP-VT]` (media):
  - Mk 32 senza soppressione dell'onda: scoppi alti, in media **~40 m (130 ft) a 11 km**, molto variabili;
  - Mk 32 Mod 40 con soppressione: **3-15 m (10-50 ft)**;
  - Mk 58: **1,5-4,5 m (5-15 ft)**, utile «contro aerei bassi e mezzi di superficie».
- **OTO 76 mm DART** (moderno, solo per confronto): spoletta di prossimità che funziona «entro 2 m dall'acqua»
  `[NW-Mk75]`.
- **URSS:** ZS-58P di prossimità per l'AK-100 `[NW-AK100]`; ZS-44P per l'AK-130 `[NW-AK130]`. L'altezza di scoppio
  sull'acqua è n.v.
- **Per il gioco:**
  - contro **un periscopio** la spoletta di prossimità è peggiore della percussione: scoppia alta sopra l'acqua, oppure
    il mare la innesca prima del bersaglio. STIMA: si usa la **PD**;
  - contro **un battello in superficie** la VT con soppressione, a 3-15 m, investe di schegge la falsa torre e gli
    uomini in plancia. STIMA: buona contro il personale e gli alberi, inutile contro lo scafo.

### 3.6 Precisione [dati scarsi; il resto STIMA]

- **Fatti:**
  - il Mk 75 di prima serie ebbe **problemi d'affidabilità e precisione**: a Dahlgren non riusciva a tenere una
    raffica di 20 colpi in un bersaglio di 6 × 6 m a 460 m `[NW-Mk75]` (alta);
  - l'OTO SR moderno dichiara una deviazione standard **< 0,3 mrad** a 1000 m per raffiche di 10 colpi (solo
    dispersione della canna) `[NW-Mk75]` (media, dato del costruttore);
  - i Mk 92 e Mk 86 inseguono bersagli di superficie; le loro precisioni sono riservate (n.v.).
- **Modello proposto (STIMA)** in millesimi di radiante (mrad):
  - errore totale **in direzione** σ_d = 1-2 mrad: inseguimento radar di un bersaglio piccolo nel clutter, moto
    proprio, rollio;
  - errore **in distanza** σ_r = 15-30 m, più 0,5% della distanza;
  - la dispersione della canna (0,3-1 mrad) è compresa.
- **Probabilità per colpo** (STIMA, mare 2-3, bersaglio fermo o lento, nave a meno di 15 nodi):

| Bersaglio | Area utile | 1 km | 3 km | 5 km | 8 km |
|---|---|---|---|---|---|
| Testa di periscopio (danno per schegge entro r_eff ≈ 2 m col 76 mm, 4 m col 5"/130) | cerchio di r_eff | 30-50% | 5-12% | 2-5% | < 1% |
| Insieme degli alberi e della parte alta della falsa torre fuori dall'acqua (3-4 m × 2 m) | rettangolo + spazio pericoloso | 50-70% | 15-25% | 6-12% | 2-4% |
| Battello in superficie **di traverso** (110 m × ~3 m di bordo libero, falsa torre 6-8 m) | rettangolo | ~95% | 60-80% | 40-60% | 20-35% |
| Battello in superficie **di prora** (10 m × 3 m più la falsa torre) | rettangolo | 70-90% | 25-40% | 12-25% | 5-12% |

- **Correzioni:**
  - mare 4: × 0,6; mare 5: × 0,3. A mare 5 un periscopio è quasi invisibile al radar (dossier 11 §5.2);
  - nave sopra i 20 nodi o in accostata: × 0,5;
  - primo colpo: × 0,5 (manca la correzione sul punto di caduta);
  - di notte senza radar di tiro: × 0,3;
  - con illuminante o radar: × 0,8.
- **Controllo di plausibilità.** Un Mk 75 a 3 km spara 10 colpi in ~8 s: la probabilità di danneggiare la testa del
  periscopio è ~40-70%. Un periscopio alzato per 10 s davanti a una fregata già in allarme e già puntata è a rischio
  vero. Un periscopio che compare di sorpresa non fa in tempo a essere colpito (§4).

---

## 4. Scoperta e tempi di reazione

### 4.1 Portate di scoperta che contano per il tiro

Il modello radar ed ESM sta nel dossier 11; qui solo gli ordini di grandezza per l'artiglieria.

| Bersaglio | Radar di superficie della nave (antenna a 20-30 m) | A vista, di giorno, col binocolo | Di notte | Fonte |
|---|---|---|---|---|
| Battello in superficie (100-1000 m²) | fino all'orizzonte radar contro la falsa torre: **~15-25 km** | 10-20 km con la falsa torre sopra l'orizzonte | radar come di giorno; a vista 1-3 km senza luna | dossier 11 §0.2, §5.2; STIMA |
| Periscopio o albero (0,1-1 m²) | **4-5 nm** dal punto di vista sovietico, mare 2-3; molto meno con mare ≥ 4 | 1-3 km la testa; piuma visibile «per miglia» a 10 nodi | quasi zero; piuma fosforescente a poche centinaia di m (bassa) | dossier 11 §5; dossier 20 §5 `[RW-OBN]` |
| Snorkel con scarico | 12-15 nm (URSS) | pennacchio di fumo, scia | odore e scarico a breve distanza | dossier 20 §5 |

### 4.2 Dalla scoperta al primo colpo (STIMA, nessun dato pubblico trovato)

Prontezza navale:
- **posto di combattimento**: tutti ai pezzi;
- **condizione III** (crociera di guerra): circa un terzo dell'armamento armato;
- **crociera di pace**: pezzi scarichi o non armati.

| Passo | Posto di combattimento | Condizione III | Pace |
|---|---|---|---|
| Classificazione (periscopio o albero, non un'onda, un rottame o una boa) | 5-20 s | 10-40 s | — |
| Designazione al sistema di tiro (radar di tiro agganciato o mira ottica) | 5-15 s | 15-60 s | — |
| Pezzo pronto e brandeggiato | 2-5 s (Mk 75 e AK-630 senza serventi nell'affusto) | 30-120 s (il Mk 45 ha 6 uomini sotto coperta; l'AK-726 va armato) | minuti; ordine del comandante e autorizzazione politica |
| **Totale fino al primo colpo** | **~15-40 s** | **~1-3 min** | non spara |

Fatti che sostengono le stime:
- il Mk 75 ha 80 colpi pronti nell'affusto e nessun servente `[NW-Mk75]`;
- il Mk 45 ha 6 uomini sotto coperta `[NW-Mk45]`;
- l'AK-130 ha 150-180 colpi pronti `[NW-AK130]`;
- un periscopio disciplinato sta fuori **meno di 10 s** in attacco (dossier 11 §5.3).

**Conseguenza per il gioco:** il tiro al periscopio è un **rischio da comportamento sbagliato**: periscopio alzato a
lungo, vicino a una scorta già in allarme, oppure piuma a velocità. Non è una punizione automatica.

---

## 5. Effetti sul sottomarino

### 5.1 Che cosa fa un proietto quando tocca l'acqua

- **Rimbalzo.** Per una sfera la legge empirica dell'angolo critico è **θ_c ≈ 18°/√σ**, con σ il peso specifico del
  proiettile (Birkhoff 1944, Johnson e Reid 1975) `[JR75]` (alta per le sfere).
  - Un proietto HE da 5" ha una densità media di ~4-5 [CALC]: θ_c ≈ **8-9°**.
  - Per un corpo allungato e rotante il numero cambia (Hutchings 1976 aggiunge l'effetto della rotazione), ma l'ordine
    di grandezza resta qualche grado.
  - Gli esperimenti moderni su proiettili veloci trovano un angolo critico che cresce con la velocità e la scarsa
    densità `[APS08]` (media).
  - **Con gli angoli di caduta del §3.4, fino a 5-8 km i colpi corti rimbalzano**, a meno che la spoletta PD non
    funzioni all'urto.
- **Spoletta all'impatto.** Le spolette a percussione dei proietti HE di solito esplodono all'urto con l'acqua; quelle
  delle granate perforanti hanno un ritardo breve (0,02-0,05 s) ed esplodono poco dopo l'ingresso `[NG-S3]` `[Okun]`
  (alta). Negli impatti molto radenti una spoletta a percussione può non funzionare e il proietto rimbalza intatto
  (forum, bassa).
- **Traiettoria in acqua.** Le ogive lunghe e appuntite, buone in aria, in acqua fanno **capovolgere** il proietto, che
  smette di avanzare di punta e si ferma dopo un breve tratto `[Okun]` (alta).
  - Solo i giapponesi progettarono proietti per colpire sott'acqua (Type 91, con l'ogiva che si stacca e lascia la
    faccia piatta del cappuccio).
  - L'unico successo noto fu un 8" nel deposito munizioni prodiero del *Boise* a Capo Esperance; gli USA provarono un
    proietto simile e lo giudicarono inutile `[NG-S3]` (alta).
- **Per il gioco:** un proietto da 76-130 mm **non arriva a un battello immerso**. Esplode in superficie, rimbalza o si
  ferma entro pochi metri (STIMA: 2-5 m sotto la superficie, poi affonda inerte).

### 5.2 A che profondità il battello è al sicuro [CALC, media]

Dati di partenza:
- le cariche WWII (dossier 07 §8.1): con **~100 kg di TNT** contro un sottomarino da 1000 t, raggio letale **3-4 m**,
  raggio d'avaria **8-10 m**;
- si scala con W^(1/3), in **acqua libera**, quindi per eccesso. Lo scoppio in superficie scarica gran parte
  dell'energia in aria e la superficie riflette l'onda d'urto invertita (§7.1).

| Proietto | Carica (≈ TNT) | Raggio letale (acqua libera) | Raggio d'avaria (acqua libera) |
|---|---|---|---|
| 30 mm OF-84 | 0,05 kg | 0,2-0,3 m | 0,6-0,8 m |
| 57 mm | 0,15 kg | 0,3-0,5 m | 0,9-1,2 m |
| 76 mm (AK-726 / Mk 75) | 0,4 / 0,75 kg | 0,5-0,8 m | 1,3-2,0 m |
| 100 mm AK-100 | 1,5 kg | 0,7-1,0 m | 2,0-2,5 m |
| 5" Mk 45 / 130 mm AK-130 | 3,5 kg | 1,0-1,3 m | 2,6-3,3 m |
| 16" HC | 70 kg | 2,7-3,5 m | 7-9 m |

- **Il 688 a quota periscopica** (chiglia a ~60 ft, 18 m; diametro 10 m) ha il dorso dello scafo resistente a
  **~8 m**. Nessun proietto da 76-130 mm che scoppia in superficie lo raggiunge. Per l'Alfa (diametro ~9,5 m) vale lo
  stesso.
- **In pericolo** restano:
  - la **testa del periscopio**, gli alberi (ESM, radio, radar, snorkel) e la parte alta della **falsa torre**, se
    affiora;
  - in pratica, tutto ciò che sta sopra o entro ~1 m sotto la superficie.
- **Battello che sta scendendo** (STIMA):
  - la falsa torre è sott'acqua di 2-3 m dopo 15-30 s;
  - da lì in poi il cannone non conta più e conta solo l'arma ASW.

### 5.3 Periscopi e alberi

- **Fatti WWII** `[WDR58]` (alta):
  - un 5,5" sulle guide dei periscopi frantumò i prismi superiori di entrambi i periscopi del *Rock*;
  - le bombe vicine allagarono le canne dei periscopi o ne ruppero i vetri della testa in sette casi.
- **Danni proposti (STIMA)** per una testa colpita direttamente o da uno scoppio entro r_eff (2 m col 76 mm, 4 m col
  5"/130, 1 m col 30 mm):

| Esito | Probabilità | Effetto in gioco |
|---|---|---|
| Vetro della testa rotto | 50% | periscopio cieco; allagamento lento della canna; non si può più abbassare del tutto, oppure va abbassato e chiuso |
| Testa o tubo piegati | 30% | periscopio bloccato, non si alza né si abbassa: rumore e piuma se resta su |
| Solo graffi o schegge | 20% | immagine degradata (macchie, fessure) |

- **Altri alberi colpiti:** ESM fuori uso; antenna radio fuori uso (niente comunicazioni a quota periscopica); radar
  del battello fuori uso; snorkel dell'Alfa: l'Alfa è nucleare, quindi non conta.
- **Effetto tattico:** perdere il periscopio d'attacco lascia quello di ricerca; perderli entrambi rende il battello
  «inoperativo, salvo in superficie» `[NHHC17]`.

### 5.4 Battello in superficie

- **Che cosa c'è da colpire:**
  - falsa torre e alberi;
  - plancia, con le vedette e l'ufficiale di guardia;
  - timoni della falsa torre (688);
  - parte alta dello scafo;
  - casse di zavorra a prora e a poppa (il 688 è a scafo semplice, con casse libere solo alle estremità);
  - cupola del sonar di prora in vetroresina (sott'acqua, di solito fuori tiro);
  - portelli e passaggi dello scafo.
- **Precedenti WWII** `[WDR58]` (alta), su scafi resistenti di ~22 mm:
  - un 5" inesploso non perforò lo scafo del *Nautilus*;
  - i 3" e i 4,7" fecero danni solo alla sovrastruttura;
  - un 8" inesploso squarciò lo scafo del *Bergall*.

  Lo scafo resistente del 688 e quello in titanio dell'Alfa sono molto più spessi; i valori esatti sono riservati
  (n.v.).
- **Valutazione (STIMA):**

| Calibro | Contro la falsa torre e gli alberi | Contro lo scafo resistente (688 o Alfa) | Personale in plancia |
|---|---|---|---|
| 30 mm | distrugge antenne, periscopi e vetri; fori nella carenatura | nessuna perforazione; danni ai rivestimenti anecoici | letale |
| 57-76 mm | carenatura squarciata; alberi e periscopi distrutti | ammaccature, nessuna falla (STIMA 95%) | letale |
| 100-130 mm, 5" | carenatura e timoni della falsa torre danneggiati; un colpo nel punto giusto blocca i timoni | falla piccola possibile in un passaggio dello scafo o in un portello (STIMA 10-20% per colpo sullo scafo) | letale |
| 16" HC | falsa torre distrutta | falla certa; il battello non si immerge più | — |

- **Conseguenza per il gioco:**
  - in superficie, sotto il fuoco del 76-130 mm, il battello **di solito può ancora immergersi**: rapporto WDR 58,
    Bowfin che tappa i fori e naviga immerso;
  - ogni colpo sulla parte alta aggiunge un rischio di falla nei portelli o nei passaggi, e quindi un'infiltrazione
    in immersione (dossier 07);
  - il pericolo vero per il 688 in superficie è **il tempo**: elicotteri, siluri e missili antinave arrivano mentre il
    cannone lo tiene in superficie.

---

## 6. Effetti visivi

### 6.1 Colonne d'acqua per calibro [CALC con le formule di Swisdak, poi STIMA]

- **Base del calcolo:** la formula per il getto da esplosione molto bassa (dossier 07 §2.4 b, Milligan e Young 1954):
  `H/W^(1/3) = 32,4·(d/W^(1/4))^0,1`, con d = 0,1-0,3 m, e il diametro della colonna
  `D/W^(1/3) = 3,71·(d/W^(1/3))^0,166`.
- **Correzione:** la spoletta PD scoppia **sulla** superficie e non sotto: gran parte dell'energia va in aria. Si
  prende quindi il **50-70%** del valore calcolato (STIMA, bassa).
- **Per le cariche piccole** (sotto 1 kg) la formula è estrapolata. A questi calibri l'energia cinetica del proietto
  (per esempio 5,4 kJ/kg a 1000 m/s) pesa quanto l'esplosivo, e la colonna è soprattutto un **getto di spruzzo**.

| Proietto | Getto calcolato | **Colonna in gioco (STIMA)** | Diametro | Durata visibile (salita + caduta + nebbia) | Note |
|---|---|---|---|---|---|
| 30 mm AK-630 / AK-230 | 10-11 m (estrapolato) | **2-5 m**, a decine, in fila | 0,5-1 m | 1-2 s | «ribollire» bianco intorno al punto mirato; traccianti |
| 57 mm AK-725 | 14-16 m | **5-8 m** | 1-1,5 m | 2-3 s | |
| 76 mm AK-726 / Mk 75 | 19-26 m | **10-15 m** | 2-2,5 m | 3-5 s | a 80 colpi/min del Mk 75: una colonna ogni 0,75 s |
| 100 mm AK-100 | 29-33 m | **15-20 m** | ~3 m | 4-6 s | |
| 5" Mk 45, 5"/38, 130 mm AK-130 | 38-42 m | **20-30 m** | ~4 m | 5-8 s | AK-130 binato: due colonne vicine |
| 16" HC | 95-106 m | **60-100 m** | ~9 m | 10-15 s | colonne colorate dalla tintura (Iowa arancio, New Jersey blu, Missouri rosso, Wisconsin verde) `[NW-1650]` |
| Proietto inerte che rimbalza | — | spruzzo basso e lungo (1-3 m), poi un secondo e un terzo spruzzo **più avanti** sulla linea di tiro | — | 1-2 s ciascuno | rimbalzo come nel §5.1 |
| Scoppio di prossimità sopra l'acqua | — | sbuffo grigio-nero a 3-15 m d'altezza, poi una **chiazza di spruzzi** sull'acqua sotto | 10-20 m | sbuffo 5-15 s col vento | VT `[BP-VT]` |

Tempi della colonna (STIMA, dossier 07 §2.4 c):
- salita in ≈ √(2H/g): **2 s per 20 m**, 4,5 s per 100 m;
- ricaduta di 2-5 s;
- spruzzo sospeso per 5-15 s secondo il vento.

Con mare formato la colonna si stacca meno dal fondo bianco delle creste.

### 6.2 Dove si vedono

- **Al periscopio** (occhio a ~1,5 m sull'acqua): una colonna di 20 m si vede sopra l'orizzonte fino a
  ~4,12·(√1,5 + √20) ≈ **23 km**; una di 10 m fino a ~18 km [CALC, dossier 11 §0.2].
  - Di fatto il limite è la visibilità: con l'ingrandimento 6×, le colonne del 5" a 5-10 km sono ben chiare.
  - È anche **un modo per capire che si spara a qualcun altro**: colonne attorno a un compagno, a un mercantile, o a
    un falso bersaglio.
- **Vicino al battello:**
  - entro ~200-300 m le colonne del 5" coprono una parte del campo a basso ingrandimento (32°);
  - sopra la testa del periscopio piovono acqua e schegge;
  - lo spruzzo sulla finestra va resa con lo stesso effetto dell'acqua sulla lente del dossier 21.
- **Il «danger space» visto da sotto** (STIMA): i colpi corti a pochi metri dal periscopio sono rimbalzi radenti; si
  vedono come una fila di spruzzi bassi che corre verso di noi.

### 6.3 Di notte

- **Vampa di bocca:** un lampo arancio-bianco di una frazione di secondo, visibile all'orizzonte della nave (decine di
  km). È il primo segnale che si spara; il colpo arriva dopo il tempo di volo.
- **Illuminanti:** il 5"/38 ha l'Illum Mark 30, che brucia **~50 s** sotto il paracadute `[NW-538]` (alta); il 5"/54
  ha il Mk 91 Illum-MT, a 823 m/s, di cui non si sono trovate intensità e durata (n.v.). I proietti illuminanti da
  mortaio di confronto danno 0,3-1 milione di candele per 50-60 s `[GS-M91]` (media).
  - Nel gioco: una luce bianca che scende lentamente, che illumina il mare per un raggio di 1-2 km (STIMA) e rende
    visibili a occhio la piuma e la falsa torre.
  - Per i sovietici: illuminanti per AK-100 e AK-130 (n.v.); si propongono identici.
- **Traccianti:**
  - l'AK-630 spara l'OR-84 tracciante `[RU-AK630]`: un flusso continuo rosso-arancio, raffiche di 1-3 s (dossier 05
    §8);
  - lo scoppio dell'OF-84 sull'acqua fa una nube di piccoli lampi;
  - i traccianti del 57 mm sono a colpo singolo, 2 al secondo;
  - il Phalanx non spara alla superficie (§3.2).
- **Lampi degli scoppi in superficie:** un lampo rossastro alla base della colonna, poi la colonna appena visibile
  contro il cielo. Senza luna e senza illuminanti si vede quasi solo il lampo.

### 6.4 Cronologia di una salva vista dalla plancia del battello in superficie [CALC]

Esempio: Mk 45 a 5 km; suono in aria 340 m/s, in acqua 1500 m/s.

| t (s) | Che cosa |
|---|---|
| 0 | vampa di bocca e fumo sul cannone |
| ~3,3 | il sonar sente il **colpo di partenza** trasmesso dall'acqua (§7.3) |
| ~7,4 | **arriva il proietto**: colonna d'acqua; se passa vicino, nello stesso istante lo «schiocco» supersonico |
| ~7,4 + d/340 | il botto dello scoppio, con d la distanza in m fra il punto di caduta e la plancia (quasi subito se cade vicino; +3 s a 1 km) |
| ~14,7 | **arriva il boato del colpo di partenza** |
| 3 s | ciclo del Mk 45 (20 colpi/min): la colonna successiva |

Con il Mk 75 a 3 km le colonne arrivano dopo ~4 s, una ogni 0,75 s; il boato del primo colpo arriva quando sono già
caduti 8-10 colpi.

---

## 7. Acustica

### 7.1 Lo scoppio del proietto in acqua, al sonar [CALC, media-bassa]

- **Livello in acqua libera.** La legge di Cole (dossier 07 §1.2): `P_m = 52,4·(W^(1/3)/R)^1,13` MPa, con W in kg di
  TNT e R in m. Oltre una certa distanza la si estrapola fuori dal campo di validità (P_m < 3,4 MPa).
- **Specchio di superficie.** Lo scoppio sta a ~0,5 m di profondità o meno. La superficie riflette l'onda invertita
  (specchio di Lloyd). Con battello a 18 m di profondità e distanza R, l'onda riflessa arriva dopo
  `Δt = 2·d_s·d_r/(R·c)`. L'impulso esponenziale di costante θ (Cole: `θ = 0,084·W^(1/3)·(W^(1/3)/R)^−0,23` ms) si
  riduce a circa **P·(1 − e^(−Δt/θ))**.
- **Conseguenza:**
  - le **basse frequenze si cancellano**: il segnale che arriva è un **«crack» corto e secco**, ricco di alte
    frequenze;
  - è diverso dal tonfo profondo delle bombe di profondità e dei siluri, che scoppiano in profondità, hanno una bolla
    che pulsa e non sono tagliate dallo specchio;
  - i proietti **non hanno pulsazioni di bolla**: la bolla sfiata subito in aria (dossier 07 §2.1).

| Proietto (≈ TNT) | 0,5 km | 1 km | 3 km | 5 km | 10 km |
|---|---|---|---|---|---|
| 30 mm (0,05 kg), libero / con lo specchio | 203 / **186** | 197 / **172** | 186 / **150** | 181 / **140** | 174 / **126** |
| 76 mm Mk 75 (0,75 kg) | 212 / **189** | 206 / **175** | 195 / **153** | 190 / **143** | 183 / **128** |
| 5" o 130 mm (3,5 kg) | 217 / **191** | 211 / **177** | 200 / **155** | 195 / **144** | 188 / **130** |
| 16" HC (70 kg) | 227 / **194** | 220 / **180** | 210 / **158** | 205 / **147** | 198 / **133** |
| Confronto: SUS Mk 61 (0,82 kg TNT), scoppio a profondità | 213 | 206 | 195 | 190 | 183 |

Valori di picco in dB re 1 µPa, battello a 18 m.

Lettura:
- il rumore ambiente con mare 3 sta intorno ai **100-110 dB** a banda larga (dossier 02). **Ogni colpo che scoppia
  entro 10 km è sentito chiaramente dal sonar**, anche il 30 mm; sono transitori di qualche millisecondo;
- un battello più profondo li sente **più forti** a parità di distanza, perché Δt cresce con d_r;
- un SSN in ascolto sente le cannonate di un'azione di superficie a decine di km, come si sentivano le cariche
  esplosive del sistema Julie (dossier 02).
- **Classificazione per il sonarista** (STIMA): «esplosioni in superficie, ripetute, a cadenza regolare» vuol dire
  tiro d'artiglieria.
  - Una cadenza di 0,75 s è un Mk 75; 3 s un Mk 45; due colpi quasi insieme ogni 1,5-6 s un AK-130 o un AK-726
    binato.
  - Un «ronzio» di micro-scoppi per 1-3 s è un AK-630.

### 7.2 Il tonfo del proietto inerte e del rimbalzo

- Un proietto che entra in acqua senza esplodere fa un «tonfo» d'impatto più la cavità che collassa. Livelli misurati:
  **n.v.** (gli EIS della US Navy trattano l'impatto dei proietti da esercitazione non esplosivi, ma i documenti non si
  sono potuti scaricare: risposta 403).
- **STIMA:** è 30-50 dB sotto lo scoppio dello stesso calibro e si sente al sonar entro 1-3 km. È il suono dei colpi
  rimbalzati e dei proietti d'esercitazione.

### 7.3 Il colpo di partenza, dalla nave che spara

- **Fonti della US Navy** (sintesi degli EIS, lette di seconda mano) `[NWTT20]` `[GOA11]` (media):
  - il colpo di partenza di un 5" dà **~200 dB re 1 µPa di picco** in acqua, **sotto la bocca**, a 1,5 m di
    profondità;
  - a 100 m lungo la traiettoria, meno di **186 dB**;
  - il suono in aria entra in acqua solo in un **cono stretto sotto la bocca e sotto la traiettoria** (finestra di
    Snell, ~13° dalla verticale, dossier 18 §6.4); il resto si riflette;
  - la parte trasmessa in acqua dalle vibrazioni dello scafo è solo il **6%** di quella del colpo di partenza.
- **Kahoolawe 1980** (Friedl e Thompson, NOSC TR 732, 1981; solo il riassunto) `[NOSC732]` (bassa-media): il livello
  di sorgente in acqua delle cannonate stimato è **~10 dB sotto quello del canto delle megattere**, con il massimo
  d'energia **vicino a 70 Hz**.
- **Modello per il gioco (STIMA):**
  - livello equivalente in acqua lontano dal cono **~165-180 dB re 1 µPa a 1 m**, energia a 40-150 Hz, impulso di
    ~50-100 ms con il rimbombo dello scafo;
  - a 5 km arrivano ~95-105 dB, appena sopra il rumore ambiente a quelle frequenze: un **tonfo sordo**, ritmico, alla
    cadenza del pezzo;
  - si sente al sonar passivo entro **~5-10 km**, prima che arrivi il proietto;
  - è un buon indizio: «la nave X sta sparando», anche se non a noi.

### 7.4 Dentro lo scafo

**Battello immerso.** Si applica il K(f) del dossier 18 §6.2: livello interno [dB re 20 µPa] ≈ livello esterno
[dB re 1 µPa] − K, con K ≈ 92 dB sotto i 500 Hz, ~96 a 1 kHz e ~101 a 2 kHz. I valori del §7.1 danno [CALC]:

| Scoppio di un 5" o 130 mm | Esterno (picco, 18 m) | Interno (−92…−100) | Che cosa si sente in centrale (fondo 60-65 dBA, molto meno in assetto silenzioso) |
|---|---|---|---|
| 0,5 km | ~191 dB | ~90-99 dB | un **colpo secco sullo scafo**, «martellata» |
| 1 km | ~177 dB | ~77-85 dB | uno **schiocco** chiaro |
| 3 km | ~155 dB | ~55-63 dB | appena, a battello silenzioso |
| 5 km | ~144 dB | ~45-52 dB | non si sente: solo in cuffia |

- Per il 76 mm si tolgono 2 dB; per il 30 mm 5 dB.
- Il segnale è ad alta frequenza (lo specchio taglia il basso) e K cresce con la frequenza. **La soglia pratica
  d'udibilità a orecchio è ~1-2 km** (STIMA, media-bassa).
- Le testimonianze WWII parlano delle bombe di profondità come «un immenso martello» e di quelle lontane come «un
  debole rimbombo» (dossier 18 §6.1). Per i proietti in superficie **non si è trovata una testimonianza di prima mano**
  (n.v.).
- **A quota periscopica la testa del periscopio colpita** si sente certamente in centrale: urto metallico trasmesso
  per il tubo, vetri, poi l'acqua che entra nella canna (STIMA).

**Battello in superficie.**
- **In plancia (all'aperto):** il colpo di partenza di un 5" a 5 km è un **boato** forte, come un tuono vicino (STIMA:
  110-120 dB di picco re 20 µPa, se sono ~175-180 dB a 10 m dalla bocca, n.v.). Poi lo «schiocco» del proietto
  supersonico che passa e lo scoppio delle colonne vicine.
  - Per confronto, il 16" misurato da Yagla nel 1989 dà fino a ~14 kPa (2 psi) entro 60 m dalla bocca `[Yagla89]`
    (media).
- **Dentro lo scafo coi portelli chiusi:** il suono in aria passa a fatica. Restano i **tonfi** dei colpi vicini,
  trasmessi dall'acqua attorno allo scafo che è ancora immerso per 2/3. STIMA: i colpi entro 1 km si sentono come
  colpi sordi; quelli che toccano lo scafo come uno schianto metallico.
- **Portello della plancia aperto** (vedette fuori): il boato e gli scoppi entrano nella falsa torre e in centrale,
  attenuati di 20-30 dB (STIMA).

---

## 8. Il gioco originale

- **688 Attack Sub (1989):** nessuna cannonata contro il battello (§0). I cannoni compaiono solo nelle schede delle
  classi, come difesa antimissile `[MAN688]` `[IA-ORIG]`.
- **Altri simulatori:** non verificati per questa voce (n.v.).

---

## 9. Proposte per il gioco [GIOCO]

### 9.1 Il principio

- **Solo nell'Enhanced, solo con la regola di simulazione; il classico resta IDENTICO.**
- Il cannone è un **rischio di contorno**:
  - punisce il periscopio alzato troppo a lungo vicino a una scorta in allarme e il battello sorpreso in superficie;
  - dà vita alle **battaglie di superficie** che il giocatore vede e sente da lontano.
- Non deve diventare il modo normale di uccidere il battello.

### 9.2 Quando l'IA apre il fuoco

Condizione comune: il contatto è **classificato** come sottomarino (in superficie, periscopio o albero) da radar o
vedetta, con traccia solida, ed è **entro la portata del pezzo** (tabella §9.3).

| ROE della missione (dossier 10) | Battello in superficie | Periscopio o albero | Note |
|---|---|---|---|
| **Pace / esercitazione** | Nessun fuoco. Segnali: luci, VHF, sirena; la nave si avvicina e pedina | Nessun fuoco. Sonar attivo e pedinamento | Come oggi |
| **Pace in acque territoriali** (solo missioni scritte apposta: Svezia, Norvegia, Kurili) | Colpi d'avvertimento a 300-500 m dal bersaglio, poi intimazione | Bombe d'avvertimento (granate, SUS) a 500-1000 m; dopo un tempo di missione, bombe vere vicino | Precedenti svedesi `[WP-SWE]` `[USNI91]` |
| **Crisi** | Colpi d'avvertimento, illuminanti, traccianti lontani; mai sul bersaglio | Segnali esplosivi non letali (Cuba 1962) | Fa emergere il battello o lo scaccia; registrato nel debriefing |
| **Guerra** | **Fuoco al bersaglio** con tutti i pezzi che battono, entro la portata efficace; missili antinave se oltre | **Fuoco al periscopio** se entro **5 km** (130 mm, 5", 100 mm), **4 km** (76 mm), **3 km** (57 mm), **2 km** (30 mm), **insieme** all'arma ASW sul punto | Nessun fuoco se una nave amica è entro 15° dalla linea di tiro (fuoco amico, `[USNI41]`) |

Regole aggiuntive (STIMA):
- prima del fuoco al periscopio la nave deve essere **in allarme** (posto di combattimento): un periscopio che compare
  di sorpresa a una nave in crociera fa partire la reazione del §4.2, cioè 1-3 min;
- il fuoco cessa **5 s dopo** che il bersaglio sparisce (periscopio abbassato), con un ultimo «colpo sul punto»;
- **lato sovietico:**
  - stesse regole;
  - l'**AK-630 spara al periscopio entro 2 km** se il radar MR-123 ha agganciato il bersaglio;
  - il Nanuchka ha solo il 57 mm (portata 6 km col radar).
- **lato NATO:**
  - niente CIWS;
  - l'**Iowa spara solo i 5"/38**; il 16" mai al periscopio, e contro un battello in superficie solo oltre i 2 km
    (opzione STIMA: «colpo di 16" al battello in superficie a 10 km» come evento raro e spettacolare).

### 9.3 Dati per classe (proposta di `data/motore/artiglieria.toml`)

```toml
# Schizzo, non definitivo. Ogni voce porta fonte e confidenza come in periscopi.toml;
# le portate contro il periscopio e il modello di tiro sono STIMA.
[pezzo.mk75]
calibro_mm = 76
cadenza_min = 80
colpi_pronti = 80
v0 = 925
carica_kg = 0.75
depressione_gradi = -15
portata_superficie_m = 8000
portata_periscopio_m = 4000
reazione_s = { combattimento = 15, condizione3 = 60 }
sigma_direzione_mrad = 1.5
sigma_distanza_m = 20
sigma_distanza_frazione = 0.005
fonte = "NW-Mk75"
confidenza = "alta (dati), STIMA (tiro)"

[pezzo.mk45]          # Spruance, Kidd: 2 affusti
calibro_mm = 127
cadenza_min = 20
v0 = 808
carica_kg = 3.5
portata_superficie_m = 15000
portata_periscopio_m = 5000

# [pezzo.mk12_5_38]   Iowa: 6 affusti binati
# [pezzo.ak100]       Krivak II, Kirov
# [pezzo.ak130]       Slava, Frunze/Kalinin (binato)
# [pezzo.ak726]       Kara, Koni (binato), depressione -2
# [pezzo.ak725]       Nanuchka I (binato), 57 mm
# [pezzo.ak630]       Kara, Kirov, Slava: raffiche, portata_periscopio_m = 2000
# [pezzo.ak230]       Koni

[classe.perry]
pezzi = ["mk75"]
ciws_superficie = false

[classe.slava]
pezzi = ["ak130", "ak630", "ak630", "ak630", "ak630", "ak630", "ak630"]
ciws_superficie = true
```

### 9.4 Modello di tiro

1. **A ogni colpo:**
   - punto mirato = posizione stimata del bersaglio (traccia radar o vedetta) più l'anticipo calcolato col tempo di
     volo del §3.4;
   - si estrae l'errore: direzione N(0, σ_d·R) e distanza N(0, σ_r);
   - le correzioni del §3.6 (mare, velocità, notte, primo colpo) moltiplicano le σ (÷ P).
2. **Traiettoria semplificata:** tempo di volo e angolo di caduta da tabella per pezzo (§3.4, interpolati).
3. **Impatto:**
   - punto di caduta dentro la sagoma esposta (spazio pericoloso compreso) → **colpo diretto**;
   - altrimenti, sull'acqua: con caduta < θ_c (8°) e spoletta PD, **50% scoppia** e **50% rimbalza**. Il rimbalzo
     prosegue di 50-300 m con angolo e velocità ridotti e può colpire il bersaglio dietro (STIMA);
   - VT: scoppio a 3-15 m sopra l'acqua, schegge in un cono verso il basso.
4. **Danno da schegge:** entro r_eff (§5.3) danno alla testa del periscopio o agli alberi, con le probabilità del §5.3;
   per la plancia, perdite tra le vedette.
5. **Raffiche (AK-630, AK-230):** una raffica di 1-3 s si tratta come N colpi con σ_d più grande (2-3 mrad) e
   r_eff 1 m.
6. **Costo per la nave che spara:**
   - il cannone rivela la nave (vampa, suono);
   - consuma munizioni (Mk 45: 20 colpi nel tamburo, poi rifornimento dal deposito; AK-726: raffreddamento dopo
     40-45 colpi `[NW-726]`).

### 9.5 Danni (si collega al modello di danno del dossier 07 e di `docs/10`)

| Bersaglio | Effetto |
|---|---|
| Testa del periscopio d'attacco (Type 2) o di ricerca (Type 18) | §5.3: cieco, bloccato o degradato; con il Type 18 si perde anche l'ESM nella testa (dossier 20) |
| Albero ESM o radio | fuori uso: niente ESM o niente radio a quota periscopica |
| Falsa torre (in superficie) | per colpo: 10-25% di timoni della falsa torre bloccati (5" e 130 mm); 5-15% di falla piccola a un passaggio (infiltrazione in immersione, pompe di sentina) |
| Plancia (in superficie, vedette fuori) | perdite di personale; il comandante perde la vista diretta finché non si scende |
| Scafo resistente | per colpo da 100-130 mm: 5-10% di falla piccola; 76 mm: 1-2%; 30 mm: 0%. Un 16": falla certa, il battello non si immerge più |
| Battello immerso sotto ~3 m dalla testa della falsa torre | nessun danno dal cannone |

### 9.6 Effetti visivi (Enhanced)

- **Colonne d'acqua** per calibro (§6.1), con il colore della tintura per l'Iowa. Un piccolo **lampo rossastro** alla
  base per gli HE; sbuffo in aria per i VT.
- **Rimbalzi:** fila di spruzzi bassi lungo la linea di tiro.
- **Vampe di bocca** sulle navi del campo visivo, con il fumo grigio che resta 5-10 s.
- **Notte:** illuminanti a paracadute (~50 s, discesa lenta, ombre lunghe sul mare); traccianti rossi dell'AK-630.
- **Al periscopio:**
  - spruzzi e acqua sulla finestra quando le colonne sono entro ~100 m;
  - per il colpo diretto: immagine che **si spegne** di colpo, o vetro incrinato con una macchia d'acqua che sale;
  - l'avviso «periscopio danneggiato» da parte dell'ufficiale di rotta.
- **Sulla carta:** i punti di caduta come piccole croci, che il giocatore vede solo se li osserva (periscopio o radar).

### 9.7 Audio

- **Sonar** (la sorgente transitoria del modello acustico, `acustica.toml`):
  - colpo di partenza: tonfo a 40-150 Hz, ~170 dB a 1 m equivalente, 50-100 ms, alla cadenza del pezzo;
  - scoppio in superficie: impulso a banda larga tagliato in basso dallo specchio (§7.1);
  - raffica di micro-scoppi per l'AK-630;
  - tonfo dell'inerte.
- **A bordo senza cuffie (immerso):** «schiocco» sullo scafo per gli scoppi entro ~1-2 km (§7.4); urto metallico e
  gorgoglio quando la testa del periscopio è colpita.
- **In superficie (plancia):** boato del colpo di partenza col ritardo giusto (R/340 s), sibilo e schiocco del
  proietto, scoppi vicini, spruzzi che ricadono.
- **Voci dell'equipaggio:**
  - sonarista: «Transitori, esplosioni in superficie, rilevamento 040, cadenza tre secondi: tiro d'artiglieria»;
  - vedetta o ufficiale di guardia: «Vampe a dritta!», «Colonne d'acqua a poppa, vicine!»;
  - ufficiale di rotta: «Il periscopio d'attacco non risponde».

### 9.8 Che cosa può fare il giocatore

- **Abbassare il periscopio** (6-8 s, dossier 20 §7): toglie il bersaglio. L'IA cessa il fuoco dopo 5 s.
- **Immersione rapida** dalla superficie o dalla quota periscopica: la falsa torre scende sotto ~3 m in 15-30 s
  (STIMA); da lì il cannone non conta più, contano le armi ASW sul punto.
- **Restare a quota periscopica con gli alberi abbassati:** al sicuro dal cannone, non dal sonar.
- **In superficie, se è necessario restarci** (avaria): virare per presentare la prora; liberare la plancia;
  allontanarsi alla massima velocità; chiedere aiuto via radio (dossier 10).
- **Controffensiva:** il battello in superficie non ha armi sopra coperta. In superficie può ancora lanciare siluri e
  missili dai tubi; nel gioco va permesso, come oggi.
- **Uso tattico del rumore altrui:** le cannonate tra navi (per esempio una battaglia NATO contro sovietici in una
  missione) sono **lontane e udibili** e coprono il proprio rumore per qualche secondo (STIMA, piccolo).

### 9.9 Difficoltà

| Livello | Effetto |
|---|---|
| Principiante | Fuoco al periscopio disattivato; contro la superficie, P × 0,5 e reazione × 2. In linea con l'originale, dove al principiante il periscopio «si nota meno» al radar `[MAN688]` |
| Standard | Valori di questo dossier |
| Esperto | Reazione × 0,7; l'IA spara anche sulla piuma senza periscopio visibile, entro 1 km (STIMA) |

### 9.10 Ordine di lavoro proposto

1. **A1 Dati:** `artiglieria.toml` dai §3.1-3.6, con fonti e confidenza; le tavole di volo del §3.4 precalcolate.
2. **A2 IA:** condizioni del §9.2 nella dottrina delle navi (accanto ad ASROC, RBU e SS-N-14); prontezza (posto di
   combattimento o condizione III); fuoco amico.
3. **A3 Tiro e danni:** §9.4-9.5; collegamento al modello di danno del periscopio (dossier 20) e del battello
   (dossier 07).
4. **A4 Grafica:** colonne, rimbalzi, vampe, illuminanti e traccianti nella vista Enhanced e nell'oculare.
5. **A5 Audio:** sorgenti transitorie del §9.7 nel modello acustico; suoni a bordo col K(f) del dossier 18.
6. **A6 Taratura:** tre missioni di prova:
   - periscopio alzato per 30 s a 3 km da una Perry in allarme: deve essere colpito circa una volta su due;
     **da rivedere (28/10/2026, taratura del runtime):** il «circa una volta su due» contava le granate già in volo
     quando il periscopio scende. Ora l'esito si verifica all'arrivo, e quelle granate cadono in acqua: con il primo
     colpo fra 16 e 31 s su un'esposizione di 30 s, la testa è colpita 0,41 volte su 100 semi (era 0,63). È la fisica
     giusta e non si insegue lo 0,5: l'atteso per 30 s di esposizione va abbassato, o la prova va fatta con
     un'esposizione più lunga del primo colpo;
   - emersione a 8 km da un Kara: il battello deve potersi immergere prima di subire danni gravi nella maggior parte
     dei casi;
   - battaglia di superficie lontana sentita al sonar.

---

## 10. Riepilogo dei numeri per il motore

| Grandezza | Valore | Conf. |
|---|---|---|
| Portata contro un periscopio (130/127/100 mm) | 5 km | STIMA |
| idem 76 mm / 57 mm / 30 mm | 4 / 3 / 2 km | STIMA |
| Portata contro il battello in superficie | portata efficace contro nave (8-15 km), ridotta a 10 km per il 5" e il 130 mm | media / STIMA |
| Reazione fino al primo colpo | 15-40 s al posto di combattimento; 1-3 min in condizione III | STIMA |
| Errore in direzione | 1-2 mrad (30 mm in raffica 2-3) | STIMA |
| Errore in distanza | 15-30 m + 0,5% R | STIMA |
| Angolo critico di rimbalzo | ~8° | [CALC] da `[JR75]` |
| Profondità di sicurezza dal cannone | tutto ciò che sta 3 m o più sotto la superficie | [CALC] da dossier 07 |
| Colonna d'acqua 30 / 57 / 76 / 100 / 127-130 mm / 16" | 2-5 / 5-8 / 10-15 / 15-20 / 20-30 / 60-100 m | STIMA da [CALC] |
| Scoppio di un 5" al sonar, battello a 18 m | 177 dB a 1 km; 155 a 3 km; 144 a 5 km (picco) | [CALC] |
| Udibile dentro lo scafo immerso | entro ~1-2 km | STIMA |
| Colpo di partenza in acqua | ~200 dB sotto la bocca; ~165-180 dB a 1 m equivalente lontano, 40-150 Hz | media / STIMA |
| Illuminante | ~50 s | alta (5"/38) |

---

## 11. Fonti

| Sigla | Fonte | URL | Conf. |
|---|---|---|---|
| `[MAN688]` | Manuale di *688 Attack Sub* (EA 1989), testo in `docs/originale/manuale_EN.txt` (schede delle classi, livelli di gioco) | locale | alta |
| `[IA-ORIG]` | Comportamenti dell'IA ricostruiti dagli script originali, `docs/originale/comportamenti_ia.md` §3.B | locale | alta |
| `[OEG51]` | Sternhell e Thorndike, *Antisubmarine Warfare in World War II*, OEG Report 51 (1946), cap. 11 | https://www.ibiblio.org/hyperwar/USN/rep/ASW-51/ASW-11.html | alta |
| `[WDR58]` | Bureau of Ships, *Submarine Report*, War Damage Report 58, sez. VII e appendice I | https://ibiblio.org/hyperwar/USN/rep/WDR/WDR58/WDR58-I.html | alta |
| `[NHHC17]` | Cdr. N. C. Twining, Nantucket Division Patrol Force, a tutte le unità, aprile 1917 (NHHC, documenti della Prima guerra mondiale) | https://history.navy.mil/research/publications/documentary-histories/wwi/april-1917/commander-nathan-c-t.html | alta |
| `[USNI41]` | *Proceedings*, giugno 1941, «Discussions, Comments and Notes» | https://www.usni.org/magazines/proceedings/1941/june/discussions-comments-and-notes | media |
| `[WP-Ward]` | Wikipedia, USS *Ward* (DD-139) | https://en.wikipedia.org/wiki/USS_Ward_(DD-139) | media |
| `[WP-SF]` | Wikipedia, ARA *Santa Fe* (S-21) | https://en.wikipedia.org/wiki/ARA_Santa_Fe_(S-21) | media |
| `[NSA75]` | National Security Archive, Electronic Briefing Book 75 (sottomarini sovietici, Cuba 1962) e cronologia | https://nsarchive2.gwu.edu/NSAEBB/NSAEBB75/ | alta |
| `[WP-B59]` | Wikipedia, sottomarino sovietico B-59 (testimonianza Mikhailov sui traccianti) | https://en.wikipedia.org/wiki/Soviet_submarine_B-59 | bassa-media |
| `[WP-SWE]` | Wikipedia, *Swedish submarine incidents* | https://en.wikipedia.org/wiki/Swedish_submarine_incidents | media |
| `[CSM83]` | *Christian Science Monitor*, 26/9/1983, «Public outcry prompts Swedish Navy to get tough with intruders» | https://csmonitor.com/1983/0926/092648.html | media |
| `[USNI91]` | *Proceedings*, marzo 1991, «Meeting the Submarine Threat» (intervista a un alto ufficiale svedese) | https://www.usni.org/magazines/proceedings/1991/march/meeting-submarine-threat | media |
| `[NW-Mk45]` | NavWeaps, 5"/54 Mark 45 | https://www.navweaps.com/Weapons/WNUS_5-54_mk45.php | alta |
| `[NW-Mk75]` | NavWeaps, 76 mm/62 Mark 75 (OTO Melara) | https://www.navweaps.com/Weapons/WNUS_3-62_mk75.php | alta |
| `[NW-538]` | NavWeaps, 5"/38 Mark 12 (con EX-30 antisommergibile e illuminanti) | https://www.navweaps.com/Weapons/WNUS_5-38_mk12.php | alta |
| `[NW-1650]` | NavWeaps, 16"/50 Mark 7 (con la funzione antisommergibile del Mk 145, 1986-89) | https://www.navweaps.com/Weapons/WNUS_16-50_mk7.php | alta (dati), media (Mk 145) |
| `[NW-AK130]` | NavWeaps, 130 mm/70 AK-130 | https://www.navweaps.com/Weapons/WNRussian_51-70_ak130.php | media |
| `[NW-AK100]` | NavWeaps, 100 mm/70 AK-100 (A-214) | https://www.navweaps.com/Weapons/WNRussian_39-70_ak100.php | media |
| `[NW-726]` | NavWeaps, 76,2 mm/59 AK-726 | https://navweaps.com/Weapons/WNRussian_3-60_ak726.php | media |
| `[NW-AK630]` `[NW-Phal]` | NavWeaps, AK-630 e Phalanx (come nel dossier 05) | https://www.navweaps.com/Weapons/WNRussian_30mm-54_ak-630.php ; https://www.navweaps.com/Weapons/WNUS_Phalanx.php | alta |
| `[RU-AK630]` | ru.wikipedia, АК-630 (bersagli, munizioni OF-84 e OR-84) | https://ru.wikipedia.org/wiki/АК-630 | media |
| `[WP-AK725]` | Wikipedia, AK-725 / S-60 57 mm (dati del proietto e portate) | https://en.wikipedia.org/wiki/AK-725 | media-bassa |
| `[WP-Phal]` | Wikipedia, Phalanx CIWS (Block 1B PSuM del 1999, elevazioni) | https://en.wikipedia.org/wiki/Phalanx_CIWS | media |
| `[Okun]` | N. Okun, «Underwater hits», NavWeaps History and Technology | https://navweaps.com/index_tech/tech-041.php | alta |
| `[NG-S3]` | Naval Gazing, «Shells Part 3» (Type 91, spolette, ricerca sui colpi sott'acqua) | https://www.navalgazing.net/Shells-Part-3 | media |
| `[NG-Spot]` | Naval Gazing, «Spotting» (formazione delle colonne, radar che vede le colonne) | https://navalgazing.net/Spotting | media |
| `[JR75]` | W. Johnson e S. R. Reid, «Ricochet of spheres off water», *J. Mech. Eng. Sci.* 17(2), 1975, 71-81 (con Birkhoff et al. 1944; Hutchings 1976 per la rotazione); legge θ_c = 18/√σ citata da arXiv physics/0411125 | https://arxiv.org/pdf/physics/0411125 | alta (legge), media (applicazione ai proietti) |
| `[APS08]` | «Shallow-angle water entry of ballistic projectiles», APS DFD 2008 (riassunto) | https://meetings-archive.aps.org/dfd/2008/ak/2/ | media |
| `[BP-VT]` | bulletpicker.com, spolette di prossimità Mk 32 Mod 40 e Mk 58 (altezze di scoppio sull'acqua) | https://bulletpicker.com/proximity_-mk-32-mod-0-40.html ; https://bulletpicker.com/proximity_-mk-58-mod-0.html | media |
| `[GS-M91]` | GlobalSecurity e FAS, illuminanti da mortaio M91, M853A1, M721 (solo confronto) | https://www.globalsecurity.org/military/systems/munitions/m91.htm | media |
| `[NWTT20]` | US Navy, *Northwest Training and Testing Final SEIS* (2020), cap. 3.0, tab. 3.0-6 «Example Weapon Noise» (letta solo nella sintesi della ricerca: il PDF risponde 403) | https://www.nepa.navy.mil/Portals/20/Documents/Pacific%20Fleet/NWTT/files/final_seis/section/NWTT_Final_SEIS_Sept2020_3.0_Affected_Environment.pdf | media |
| `[GOA11]` | US Navy, *Gulf of Alaska Final EIS* (2011), sez. 3.7 (misure del 2000 sul colpo di partenza del 5": ~200 dB all'interfaccia, < 186 dB a 100 m; vibrazione dello scafo 6%) | https://www.nepa.navy.mil/Portals/20/Documents/Pacific%20Fleet/GOA/files/EIS/Final_EIS_2011/sections/GOA_FEIS_3_7_Sea_Turtles.pdf | media |
| `[NOSC732]` | W. A. Friedl e P. O. Thompson, *Measuring Acoustic Noise Around Kahoolawe Island*, NOSC TR 732 (1981), ADA109485 (solo riassunto) | https://apps.dtic.mil/sti/pdfs/ADA109485.pdf | bassa-media |
| `[Yagla89]` | J. J. Yagla, *16 Inch Gunblast Experiments*, NSWC, febbraio 1989 (riassunto) | https://www.academia.edu/85290230/16_Inch_Gunblast_Experiments | media |
| `[RW-OBN]` `[CIA-REG86]` | come nei dossier 20 e 17 | — | — |
| `[Sw78]` `[Cole48]` `[Snay56]` | come nel dossier 07 (Swisdak 1978, Cole 1948, Snay 1956) | — | alta |
| [CALC] | Calcoli di questo dossier: balistica con resistenza quadratica (§3.4), legge di Cole con lo specchio di Lloyd (§7.1), scala dei raggi letali con W^(1/3) (§5.2), getti di Milligan e Young (§6.1), orizzonte (§6.2) | — | come indicato |

---

## 12. Lacune

1. **Dottrina sovietica scritta** sul tiro contro il periscopio e il sottomarino in superficie (Боевой устав ВМФ,
   Правила артиллерийской стрельбы): n.v. Da cercare nella traduzione CIA del 1986 (capitolo sull'ASW) e nelle
   memorie di comandanti di unità ASW.
2. **Testo della legge sovietica sul confine del 1982** (articoli sull'uso delle armi in mare): non letto.
3. **Dottrina USN degli anni '80** sul tiro contro sottomarini (NWP di ASW, istruzioni del Mk 86 e del Mk 92):
   riservata o n.v.
4. **Precisione reale** del Mk 86, del Mk 92 e dei sistemi Lev e Turel contro bersagli piccoli: solo l'aneddoto di
   Dahlgren sul Mk 75.
5. **Altezza e durata delle colonne d'acqua** per calibro: nessuna tabella trovata; i valori del §6.1 sono calcoli
   con una formula per cariche statiche. Da tarare su filmati (prove di tiro del Mk 45 e dell'AK-130, Iowa 1984-91).
6. **Livelli acustici misurati** di proietti che scoppiano in superficie e di proietti inerti che entrano in acqua: i
   documenti degli EIS della US Navy (tabelle delle «bin» esplosive, modello a 1 m di profondità) non si sono potuti
   scaricare (403); Kahoolawe solo il riassunto.
7. **Testimonianze di prima mano** di sommergibilisti che hanno sentito cannonate in immersione: n.v.
8. **Spessore dello scafo resistente** del 688 e dell'Alfa: riservato; i danni del §5.4 sono STIMA.
9. **Casi della Guerra Fredda** con colpi d'avvertimento a periscopi (Norvegia, Giappone, Corea): non cercati a fondo.
10. **Illuminanti sovietici** da 100 e 130 mm e Mk 91 americano: intensità e durata n.v.

**Rimandi interni:**
- dossier 01 §6 (dotazioni);
- 05 §8 (CIWS);
- 07 §1-2, §8 (esplosioni, raggi letali, colonne);
- 10 (ROE e radio);
- 11 §0.2, §5 (orizzonte e scoperta degli alberi);
- 17 §5.5 (ROE degli aerei);
- 18 §6 (suono a bordo);
- 20 §5, §7 (periscopio: esposizione e dottrina);
- 21 (stato del mare, acqua sulla lente).
