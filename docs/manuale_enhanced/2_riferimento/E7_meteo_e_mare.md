# E7. Meteo e mare

*Autore: kit, per il runtime. Revisione del 27/10/2026. Stato: rivisto (runtime, 27/10/2026), curatore: kit.*

## 1. A che serve

Decide il tempo che fa nel teatro della missione e come cambia ora per ora: vento, onde, nubi, pioggia e neve,
nebbia, temporali, ghiaccio. Da un solo stato fisico vengono tutte le conseguenze: il rumore del mare al sonar, le
onde che coprono la finestra del periscopio, la quota periscopica difficile da tenere, la vela che affiora, il radar
che non vede una testa fra le creste, le vedette nella nebbia, gli elicotteri che non calano il sonar, le navi
piccole che rallentano. Il maltempo copre il battello e insieme gli toglie la quota periscopica. Il comando trasmette
bollettini e avvisi di burrasca, con gli errori delle previsioni del 1989.

## 2. Il modello

Il codice sta in tre parti:
- `nucleo/meteo.hpp` e `nucleo/src/meteo.cpp`: il generatore (`Meteo`), pezzo M1 di `docs/15`. Modulo puro e
  deterministico: un generatore casuale per processo, così aggiungerne uno non cambia gli altri;
- `nucleo/onde.hpp`: l'elevazione del mare in un punto nel tempo, per la testa del periscopio e per la vista;
- `missioni/src/missione_meteo.cpp`: il meteo nella missione, i bollettini, la vela che affiora, la velocità delle
  navi col mare (pezzo M2).

I dati sono in `data/motore/meteo.toml` (da `tools/pipeline/meteo.py`, pezzo M0 del kit): la sezione `[generale]`
con le costanti fisiche, e per ognuno dei sette teatri la climatologia mensile ERA5 1981-1990, le stagioni, i regimi,
gli eventi rari e i punti ERA5 del riquadro. Ogni voce porta fonte, confidenza e l'elenco delle stime. La tabella
completa è nell'appendice G10.

### 2.1 La scala unica

Lo stato del mare è uno solo per tutto il gioco (dossier 21 §8 punto 2): la classe di Douglas (WMO 3700) dall'altezza
significativa `Hs`, non dal vento. Il vento ha la sua scala, Beaufort.

| Mare (Douglas) | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |
|---|---|---|---|---|---|---|---|---|---|---|
| Hs da (m) | — | oltre 0 | 0,1 | 0,5 | 1,25 | 2,5 | 4 | 6 | 9 | 14 |
| Hs al centro della classe (m) | 0 | 0,05 | 0,3 | 0,875 | 1,875 | 3,25 | 5 | 7,5 | 11,5 | 14 |

```
Hs = √(Hs_vento² + Hs_swell²)        stato_mare = classe_douglas(Hs)
```

Beaufort dai limiti in m/s 0,3, 1,6, 3,4, 5,5, 8, 10,8, 13,9, 17,2, 20,8, 24,5, 28,5, 32,7 (`beaufort`). Le soglie
sono costanti in `nucleo/src/meteo.cpp` (`classe_douglas`, `centro_douglas`, `beaufort`). Le stesse sono in
`meteo.toml [generale]` (`douglas_limiti_m`, `douglas_centri_m`, `beaufort_limiti_m_s`): il file le carica ma il
codice non le usa.

### 2.2 Il generatore

`Meteo` genera uno stato per ogni ora di missione e lo interpola al secondo (la visibilità sul logaritmo). Il seme
mescola teatro, missione, data e seme della partita: lo stesso codice partita ridà lo stesso tempo. Le ore si
generano in avanti a richiesta, anche dalla previsione, e se ne tengono 200 indietro.

Ogni ora, in quest'ordine (dossier 21 §4):

1. **Il regime sinottico.** Una catena semi-Markov fra i regimi del teatro: per il Nord Atlantico anticiclonico,
   prefrontale, settore caldo, fronte freddo, postfrontale, bloccato; per Gibilterra levante, poniente, vendaval,
   calma, cut-off; per Otranto bora chiara e scura, scirocco, maestrale, calma, temporale autunnale; per Kunashir
   monsone, ciclone esplosivo, flusso nebbioso, calma. La durata è gamma di forma 3 attorno a quella del regime,
   almeno 2 ore (STIMA). Il regime dopo si sceglie fra i suoi `prossimi`, con un salto verso un regime qualunque nel 10% dei casi
   (STIMA). I pesi delle transizioni si tarano all'avvio perché il tempo passato in ogni regime sia la quota della
   stagione. Il passaggio da un regime all'altro dura 3 ore (o l'esordio del regime).
2. **Il vento.** Un processo AR(1) a due stadi nello spazio gaussiano, uno veloce di 4 ore e uno lento con la
   persistenza misurata della stagione (`tau_vento_h`), più l'anomalia del regime con peso 0,6 (`BETA_REGIME`). Il
   valore passa per la distribuzione vera dell'anomalia e poi per la Weibull del mese, con la coda tarata sulle ore
   di forza 7 e 8 dei dati (ERA5 sottostima le burrasche: `correzione_burrasche`). Si aggiunge un ciclo diurno debole.
   La direzione è quella del regime (o della rosa dei venti del mese) più un AR(1) di 6 ore con σ = 25° (STIMA).
3. **Gli eventi rari.** Per ogni evento del teatro nei suoi mesi, un tiro orario col tasso mensile diviso per 720
   ore (× 1,5 nel mese di picco, × 0,9 negli altri); un evento alla volta. Polar low alle Fær Øer e nel Barents, fumo
   di mare denso nel Barents, tempesta di forza 10 alle Fær Øer e nel Mar Celtico, medicane e trombe marine nel canale
   d'Otranto, cut-off e trombe marine a Gibilterra, nebbia persistente, ciclone esplosivo e spruzzo gelante forte a
   Kunashir, temporale estivo nel Baltico. L'evento si sovrappone al regime per la sua durata: con la sua rampa alza
   vento e mare, e porta i suoi effetti (neve, pioggia, nubi, pressione, nebbia).
4. **Le onde.** Il mare da vento rilassa verso l'equilibrio (formule del Coastal Engineering Manual):

   ```
   Cd = 0,001 · (1,1 + 0,035 U)          u* = U · √Cd
   Hs_PM    = 0,22 · U² / g  · (10 / U)^0,4 sopra i 10 m/s
   Hs_fetch = 0,0413 · u* · √(F / g)
   Hs_eq    = min(Hs_PM, Hs_fetch) · scala
   Hs(t+1h) = Hs_eq + (Hs(t) − Hs_eq) · e^(−1/τ)
   ```

   `U` è il vento in m/s, `F` il fetch in m del settore da cui soffia (`fetch_km`, zero = illimitato), `g` = 9,81
   m/s². `τ` vale 6 ore quando il mare cresce e almeno 18 ore quando cala (0,6 volte la persistenza ERA5 del mare).
   Il periodo di picco è `Tp = 5 √Hs`. L'onda lunga (swell) è un AR(1) lognormale di 48 ore con σ = 0,3. Il
   correttivo sopra i 10 m/s viene dalle Hs mediane per classe di vento (STIMA): sopra quella velocità il mare non fa
   in tempo a svilupparsi.
5. **Nubi e precipitazione.** Le nubi seguono il regime con un AR(1) di 6 ore. Le ore bagnate sono una catena
   asciutto/bagnato con la quota del regime e una persistenza di 0,6 (STIMA); l'intensità è gamma. Sotto 1 °C
   d'aria la pioggia è neve; sotto 2 ottavi di nubi non piove. Il plafond è 300 m col coperto e la pioggia, 600 m col
   coperto asciutto, 1200 m da 3 ottavi, 3000 m sotto.
6. **Temperature e pressione.** AR(1) di 72 ore per l'aria, 240 per il mare, 24 per la pressione, attorno ai valori
   del mese; aria e pressione anche con l'anomalia del regime. La tendenza barometrica è quella delle ultime 3 ore.
7. **Ghiaccio e spruzzo gelante.** La frazione di ghiaccio del mese, almeno 0,3 nel Baltico in febbraio e marzo degli
   inverni rigidi (1985, 1986, 1987). Il ghiaccio smorza il mare: Hs × (1 − ghiaccio). Lo spruzzo gelante segue
   Overland: `PPR = U (Tf − Ta) / (1 + 0,3 (Tw − Tf))`, con Tf = −1,7 °C, nelle classi 0,7, 2 e 4 cm/h.
   Dove la frazione di ghiaccio c'è, il campo procedurale della banchisa (`nucleo/campo_ghiaccio`) la trasforma in pack
   vero: lastroni, polynye, nilas e chiglie che derivano col vento (capitolo E11).
8. **Nebbia e visibilità.** La nebbia è d'avvezione, di radiazione (solo fra le 22 e le 9), da precipitazione, o fumo
   di mare quando il mare è almeno 9 °C più caldo dell'aria (strato di 5-50 m). Viene nei regimi che la portano, col
   vento fino a 10 m/s; resta o cambia con probabilità 0,25 l'ora (STIMA). La visibilità segue Koschmieder:

   ```
   V = 3,912 / σ        σ = σ_foschia + σ_pioggia + σ_neve + σ_spruzzi + σ_nebbia     [1/km]
   ```

   La foschia di fondo è quella del teatro per stagione; la pioggia aggiunge 2,24 · C^0,75 con l'acqua `C = 0,072 ·
   R^0,88` (R in mm/h); gli spruzzi da forza 9; la nebbia 3,912 diviso 100-400 m, più fitta di notte (× 1,2) e meno
   nel pomeriggio (× 0,8). La visibilità resta fra pochi metri e 60 km; nel polar low al più 1,5 km.
9. **Le celle temporalesche.** Nascono nei regimi col temporale, coi giorni di temporale della stagione (due celle
   per giorno, STIMA), in un riquadro di ±100 km. Hanno un raggio di 5-15 km, vivono 1-3 ore, fanno 1-4 lampi al
   minuto e vanno col vento a 0,6 volte la sua velocità (STIMA).

All'avvio una corsa breve del generatore tara la scala del mare da vento e lo swell, perché la Hs media del mese sia
quella di ERA5, e scala nebbia e ore bagnate sulle quote dei dati.

**Esempio.** Canale d'Otranto, vento da est a 15 m/s (forza 7). In mare aperto l'equilibrio sarebbe Hs_PM = 0,22 ·
225 / 9,81 · (10/15)^0,4 = 4,3 m, mare 6. Ma da est il fetch è di 80 km: Cd = 0,0016, u* = 0,60 m/s, Hs_fetch =
0,0413 · 0,60 · √(80 000 / 9,81) = 2,3 m, mare 4. Il mare corto dell'Adriatico vince sul vento, prima della scala del
teatro e dello swell.

### 2.3 Il meteo in un punto

`Meteo::in(pos)` parte dallo stato del centro del riquadro e lo corregge:
- **i punti ERA5** del teatro (4-5 per teatro), pesati con `1 / (d² + 25)` (d in km): rapporti di vento e di Hs col
  centro, scarto di temperatura, ghiaccio (il più alto fra il mese e i punti);
- **gli stretti**: entro 30 km dal centro del teatro il vento si incanala del fattore del teatro e del regime (STIMA);
- **le celle** sopra il punto: lampi, rovescio fino a 20 mm/h al centro, raffiche, coperto con plafond a 500 m,
  visibilità ridotta dalla pioggia;
- **il ghiaccio** del punto smorza il mare solo per quello che ha in più del teatro, perché lo stato del teatro è già
  smorzato dal generatore. Se il ghiaccio del punto è maggiore di quello del teatro, Hs del vento e dello swell si
  moltiplicano per (1 − ghiaccio del punto) / (1 − ghiaccio del teatro); se no restano quelle del teatro. Esempio:
  teatro 0,3, punto 0,6: il mare del teatro scende di 0,4 / 0,7 = 0,57. In tutto il punto ha il mare senza ghiaccio
  per (1 − 0,6), una volta sola.

Nella missione (`Missione::passo_meteo`, ogni secondo) il meteo si calcola nel punto del giocatore (`meteo_qui_`).
Chi ascolta lontano dal giocatore usa il meteo di una cella di 5 km, aggiornato ogni 10 s (`meteo_ascolto`,
`METEO_CELLA_M`, `METEO_CELLA_S`).

### 2.4 La climatologia dei teatri

Valori del punto centrale, gennaio e luglio (`meteo.toml [teatri.*.mesi]`, ERA5 1981-1990):

| Teatro | Vento medio gen / lug (m/s) | Ore di forza 8 a gennaio | Hs media gen / lug (m) | Mare gen / lug (°C) | Nebbia a luglio (ore con UR ≥ 97%) |
|---|---|---|---|---|---|
| Fær Øer | 10,7 / 6,6 | 8,4% | 4,2 / 1,6 | 7,4 / 10,1 | 7,9% |
| Barents | 9,2 / 6,3 | 0,6% | 2,4 / 1,1 | 2,8 / 7,4 | 8,5% |
| Mar Celtico | 10,2 / 6,1 | 5,2% | 4,0 / 1,5 | 11,7 / 16,3 | 2,4% |
| Baltico | 9,2 / 6,4 | 1,0% | 1,5 / 0,7 | 3,1 / 16,2 | 0,4% |
| Gibilterra | 7,2 / 6,1 | 0,2% | 1,0 / 0,7 | 15,9 / 21,1 | 1,9% |
| Otranto | 6,8 / 5,4 | 0,6% | 0,9 / 0,5 | 14,5 / 24,3 | 0,9% |
| Kunashir | 9,5 / 5,2 | 1,4% | 2,3 / 0,8 | 1,1 / 10,5 | 33,4% |

Il ghiaccio: nel Barents il pack sta fuori dal riquadro (solo baie e fiordi); a Kunashir il pack copre il lato
dell'Okhotsk da febbraio a metà aprile, con lingue fra i passi; nel Baltico solo negli inverni rigidi. Gli altri
teatri non hanno ghiaccio. Lo swell oceanico (1-4 m alle Fær Øer e nel Celtico) tiene il mare sopra la calma anche
col vento debole; nel Baltico e a Otranto lo swell è quasi assente.

### 2.5 Le onde nel punto della testa

`nucleo/onde.hpp` dà l'elevazione del mare in un punto nel tempo, da `Hs` e dal periodo di picco: cinque componenti
attorno al picco (0,8, 0,9, 1, 1,15 e 1,35 volte la frequenza di picco), con la varianza `(Hs/4)²`. È la stessa per
la simulazione e per la vista. Il modello dei periscopi la usa per decidere se un'onda copre la finestra (§2.8).

### 2.6 I lampi

`Meteo::lampi(t0, t1)` dà i lampi deterministici di un intervallo, dalle celle vive: per ogni minuto di vita di una
cella un tiro di Poisson coi suoi lampi al minuto. Un lampo ha 1 colpo nel 30% dei casi, altrimenti 2 più un
Poisson: in media 3,67 colpi per lampo in tutto (`lampi_colpi_media`), a 57 ms l'uno dall'altro nei transitori del
sonar (`lampi_intervallo_ms`). Il 25% tocca il mare (STIMA). Lo stesso minuto dà gli stessi lampi comunque si chieda
l'intervallo.

Nella missione i lampi nube-mare entro 40 km dal giocatore diventano transitori per il sonar (`transitori_lampo`,
dossier 21 §6.2). Il livello e la forma sono nel capitolo C3 §2.5.

### 2.7 La previsione e i bollettini

`Meteo::previsione(emessa, valida)` dà il bollettino emesso a un'ora per un'ora futura, con gli errori del 1989
(dossier 21 §5.1, `[generale] previsione_*`):

| Orizzonte | 12 h | 24 h | 48 h | 72 h |
|---|---|---|---|---|
| errore sul passaggio dei fronti (σ, ore) | 2 | 4 | 10 | 18 |
| errore sul vento (σ, m/s) | 1,5 | 2,5 | 3,5 | 4,5 |
| errore su Hs (σ, m) | 0,3 | 0,5 | 0,7 | 0,9 |

- L'errore sui fronti sposta tutto il bollettino nel tempo. Da 48 ore in su, una volta su quattro il bollettino
  anticipa il tempo di 6-12 ore: la burrasca arriva dopo il previsto (`previsione_burrasche_in_ritardo`).
- La previsione parte dallo stato senza eventi rari: polar low, getti, temporali e trombe marine non si prevedono mai.
  Gli altri eventi entrano se il teatro li dà come prevedibili.
- I temporali singoli non ci sono: «possibili rovesci». La nebbia diventa «possibili banchi», con la visibilità
  almeno a 1 km.
- L'errore è deterministico per bollettino: lo stesso bollettino dice sempre la stessa cosa.

**I bollettini alla sala radio** (dossier 21 §5.2-5.3; `Missione::bollettini_meteo`):
- alle 00, 06, 12 e 18 UTC il WEAX (per il 688, da NAVEASTOCEANCEN) o la «meteosvodka» (метеосводка) del servizio
  idrometeorologico della flotta per l'Alfa: vento, forza, mare e visibilità a 12 e 24 ore, con la tendenza;
- la visibilità in parole: buona oltre 10 km, moderata oltre 4, scarsa oltre 1, nebbia sotto;
- la tendenza: in peggioramento con la pressione che cala più di 4 hPa o il vento che sale più di 3 m/s fra 12 e 24
  ore, in miglioramento al contrario, altrimenti stazionaria;
- se la previsione a 6, 12, 18 o 24 ore vede vento da 34 nodi, l'avviso di burrasca (GALE WARNING), da 48 nodi
  l'avviso di tempesta (STORM WARNING); per l'Alfa lo «shtormovoe preduprezhdenie» (штормовое предупреждение). Hanno
  precedenza prioritaria e si ripetono solo per una burrasca nuova;
- la previsione chiesta col rapporto di tipo «previsione» va in coda mezzo ciclo del broadcast dopo la richiesta, e
  la trasmissione si può intercettare (capitolo C11).

Chi non ascolta la radio li perde. Col meteo fisso non ci sono bollettini, e la previsione chiesta non ha risposta.

### 2.8 Gli effetti su ogni sistema

Il meteo nel punto del giocatore (o di chi ascolta) entra in ogni modello che lo usa:

| Sistema | Che cosa conta | Dove | Capitolo |
|---|---|---|---|
| Rumore ambiente | vento (Wenz, Knudsen), pioggia, ghiaccio; col meteo dinamico il mare dalla Hs vera | `Missione::ambiente_acustico` | C3 |
| Sensori vicino alla superficie | il mare grosso sui sonar di scafo, sulle boe e sulle schiere | acustica | C1, C3 |
| Propagazione | l'effetto pomeriggio dal vento e dalle nubi, ogni 10 minuti | `passo_pomeriggio` | C2 |
| Fulmini al sonar | transitori nube-mare entro 40 km | `passo_meteo` | C3 |
| Periscopio | onde sulla finestra, errore di quota, tremolio, vista ridotta | `nucleo/periscopi` | C9 |
| Vela che affiora | rischio al minuto col mare grosso | `passo_meteo` | qui sotto |
| Radar | portata contro teste e alberi per stato del mare, pioggia in banda X | `nucleo/elettronica` | C10 |
| Vedette e periscopi dell'IA | mare, visibilità, nebbia, nubi di notte | `nucleo/periscopi` | C9, E8 |
| ASW aereo | boe oltre il loro mare, dip, MAD, siluri aviolanciati, temporali | `missione_asw` | E5 |
| Navi | velocità sostenibile col mare di prua | `velocita_col_mare` | qui sotto |
| Artiglieria | precisione col mare, piuma vista fino a mare 4 | `missione_artiglieria` | E4 |
| Navigazione | il punto nave astronomico solo con poche nubi (fino a 3 ottavi) | `missione_navigazione` | C12 |
| Audio e radio | tuono, colpi di mare, scariche HF | `nucleo/audio_meteo` | C8, C11 |

**Il periscopio col mare** (dossier 21 §7.3 e §8 punti 8 e 11):
- l'errore di quota è un AR(1) di 20 s con σ = 0,4 volte la tabella del 688: 0,3, 0,4, 0,5, 0,6, 1,5, 2,5, 4, 6 m da
  mare 0 a 7 (`sigma_quota_m`). Il fattore 0,4 (il comandante tiene la testa alta) viene dalla taratura M6: con la
  tabella intera la testa coperta era al 14% a mare 4 e al 19% a mare 5, contro l'1-3% e il 7-15% del dossier;
- la finestra è coperta quando l'onda nel punto supera l'altezza vera della testa meno la finestra;
- il mare scuote la testa: il tremolio sale di 0,08 per ogni grado oltre mare 2 (STIMA);
- la portata a vista su una testa e sulla piuma si moltiplica per 1,3, 1,3, 1, 0,8, 0,5, 0,3, 0,2, 0,1 da mare 0 a 7
  (STIMA su IAMSAR, `fattore_mare`), e non supera la visibilità meteo. Nella nebbia di radiazione il termico vede fino
  a 4 volte l'occhio, ma non sotto i 300 m.

**La vela che affiora** (dossier 21 §7.3). A quota periscopica (dalla superficie a 20 ft più 1,5 m) il cavo d'onda
può scoprire la vela. Il rischio al
minuto è 0,2% a mare 4, 1% a mare 5, 5% a mare 6, 10% oltre. Vale per intero sotto i 4 nodi o col mare al traverso
(entro 30° dal traverso rispetto al vento), altrimenti un terzo (STIMA). L'affioramento dura 15 s: il battello è
visibile al radar come in superficie, la vela entra nella sagoma dell'artiglieria (capitolo E4), e l'equipaggio
avverte (messaggio 408).

**Il radar contro teste e alberi** (`r50_m`, dossier 21 §6.1). Col mare calmo la portata è migliore (× 1,3 a mare 0,
× 1,15 a mare 1 di quella a mare 2); fra 2 e 4 segue i dati del radar; da mare 5 si annulla. Il codice ha un ramo per
i radar fatti per i periscopi, che scenderebbero a 0,4, 0,2 e 0,1 della portata a mare 2. Vale per chi a mare 4 tiene
almeno 0,55 della portata a mare 2: nei dati nessuno, nemmeno l'APS-116 e l'APS-137 (capitolo C10 §7). La pioggia
attenua da 4 GHz in su, in banda X circa 0,0101 · R^1,276 dB/km, contati due volte per l'andata e il ritorno; la sua
eco nasconde le teste piccole (STIMA).

**Le navi col mare di prua** (dossier 21 §7.2, `velocita_col_mare`). Da mare 4 la velocità ha un tetto che dipende
dalla lunghezza (130 m se la classe non la dà), interpolato fra:

| Lunghezza | 60 m | 130 m | 170 m | 250 m | 330 m |
|---|---|---|---|---|---|
| mare 4 | 28 | 29 | 32 | 31 | 30 |
| mare 5 | 18 | 25 | 28 | 30 | 30 |
| mare 6 | 12 | 18 | 22 | 26 | 28 |
| mare 7 | 8 | 12 | 15 | 20 | 24 |
| mare 8 e oltre | 5 | 8 | 10 | 14 | 18 |

Velocità in nodi. Col mare al traverso o in poppa (oltre 60° dalla direzione del vento) il tetto è 1,25 volte più alto
(STIMA).

### 2.9 Tre modi

- **Semplificato** (`ModoMeteo::semplificato`, il predefinito dell'Enhanced): mare 3, sereno, vento di 5 m/s,
  visibilità 30 km (`meteo_semplificato`). Fisso per tutta la missione, senza bollettini e senza lampi. Col
  semplificato, e con ogni meteo fisso, l'acustica resta col mare dal vento, come prima del meteo.
- **Realistico** (`ModoMeteo::realistico`): il generatore del teatro, del mese e dell'ora della missione.
- **Fissato dall'autore**: la chiave `meteo = { fisso = { ... } }` della missione (regime, vento, direzione, raffica,
  stato del mare, swell, nubi, plafond, pioggia, neve, visibilità, nebbia, lampi, temperature, pressione, ghiaccio).
  I campi dati vanno sopra il meteo di partenza (quello del teatro, o il semplificato) e restano fermi per tutta la
  missione (`meteo_fisso_della_missione`).

Col teatro che non c'è vale il semplificato. Senza il file dei dati il generatore non nasce: resta lo stato di base
(mare 3, vento di 5 m/s, visibilità 30 km), senza bollettini.

## 3. Fonti e confidenza

| Parametro | Fonte | Confidenza |
|---|---|---|
| Climatologia per teatro e mese (vento, Hs, temperature, nubi, precipitazione, nebbia, ghiaccio) | ERA5 1981-1990 via Open-Meteo, dossier 21 §3, nota A §2 | alta (dati); ERA5 sottostima le burrasche |
| Regimi, quote e durate | dossier 21 §3-4, nota A §6-7 | media; forma gamma 3, salto 10% **STIMA** |
| Persistenza del vento e del mare | ERA5, nota A §6.3 | alta |
| Mare da vento (Pierson-Moskowitz, CEM, fetch) | dossier 21 §2 e §4 | media; correttivo sopra 10 m/s e fetch per settore **STIMA** |
| Douglas e Beaufort | WMO, dossier 21 §8 | alta |
| Visibilità di Koschmieder, estinzioni | dossier 21 §2, nota B §3 | media |
| Nebbia e fumo di mare | dossier 21 §3, nota A | media; persistenza e visibilità nella nebbia **STIMA** |
| Spruzzo gelante (Overland) | dossier 21 §7.2 | media |
| Celle e lampi | dossier 21 §2 e §6.2, nota D B-bis | media (colpi a 57 ms); celle e lampi al minuto **STIMA** |
| Previsione del 1989 | dossier 21 §5.1, nota A §8 | media; σ per orizzonte **STIMA** |
| Bollettini, avvisi di burrasca, nomi sovietici | dossier 21 §5.2-5.3, nota D §A1-A2 | alta |
| Effetti per stato del mare | dossier 21 §6.1 (nota B §8.1) | da alta (rumore) a **STIMA** (vista) |
| Quota periscopica, affioramento | dossier 21 §7.3 e §8 punto 8 | media; affioramento e fattore 0,4 **STIMA** |
| Velocità delle navi | dossier 21 §7.2 (nota C §12.4) | media |

## 4. Classico ed Enhanced

Il classico non ha meteo: lo stato del mare e il tempo non sono nel modello dell'originale, e la missione classica
non crea il generatore. Il rilievo delle partite classiche resta identico, e con il meteo spento la vista è identica
pixel per pixel.

Nell'Enhanced il meteo si accende con la regola di simulazione (`DanniMissione::avanza` chiama
`Missione::meteo_enhanced`), e l'app lo crea prima del briefing per mostrarlo. Il modo predefinito è il semplificato;
il realistico si sceglie nel menu.

## 5. Come si vede in gioco

- **Volume 1**, cap. 9: §9.1 (W sceglie il meteo nella scelta della missione: fisso o del teatro e della stagione),
  §9.2 (lo stato del mare e i suoi effetti), §9.3 (che cosa si vede e si sente), §9.5 (sotto il ghiaccio), §9.6 (il
  meteo via radio e la previsione chiesta, tasto 6 alla Radio, poi 7).
- **Briefing**: il meteo all'inizio e la previsione a 12 ore (WEAX o PROGNOZ), con l'errore del 1989.
- **Debriefing**: la pagina dell'esito riporta vento, forza, mare, visibilità, temporale, pioggia o nebbia a fine
  missione (capitolo F1 §2.6).
- **Al periscopio**: il mare con le creste e la schiuma (Monahan, per forza Beaufort), le nubi col plafond (in prospettiva; oltre i 60 km sfumano: il coperto resta un velo uniforme fino all'orizzonte, le nubi sparse spariscono nel cielo), la
  foschia, la nebbia, il fumo di mare, la pioggia e la neve, i lampi, le navi che rollano e beccheggiano col periodo
  d'incontro, le creste vicine che coprono l'orizzonte e le navi (capitolo E9).
- **Sulla carta**: una barba del vento ogni 192 px, lunga 24 px, il vento e il mare nella legenda, le celle, il ghiaccio, la
  nebbia (livello `meteo`).
- **Messaggi di bordo**: 408 «la vela affiora»; 409 il sonar che sente un transitorio senza contatto (il fulmine), al
  più una volta al minuto; i bollettini e gli avvisi nella sala radio.
- **Al sonar e a bordo**: i fulmini nella cascata e lo schiocco in cuffia; a bordo il tuono (solo in superficie) e i
  colpi di mare da mare 4 (capitolo C8).
- Il giorno, la notte e la luce sotto le nubi sono nel capitolo E8.

## 6. Taratura e prove

**Cento anni sintetici** per teatro e mese contro le tabelle ERA5: medie e quantili di vento e Hs, ore di forza 7 e
8, nebbia, ore bagnate, entro il ±20% (`test/prove_meteo.cpp`). **Taratura M6**: l'errore di quota del periscopio
ridotto al fattore 0,4, che riporta la testa coperta all'1-3% a mare 4 e al 7-15% a mare 5 del dossier 21 §8.

Prove automatiche:
- `test/prove_meteo.cpp`: «meteo: scala unica di Douglas e Beaufort», «i dati dei sette teatri», «deterministico, un
  generatore per processo», «cento anni sintetici contro le tabelle ERA5», «nebbia, ghiaccio e mari chiusi per
  teatro», «eventi rari nella loro stagione, polar low nel Barents», «celle temporalesche e lampi deterministici», «la
  previsione del 1989 sbaglia di piu' piu' e' lontana, niente polar low», «nel punto, stretti e meteo fisso»; «meteo
  missione: il generatore del teatro nell'Enhanced col realistico, niente nel classico, semplificato predefinito»,
  «col mare grosso le navi rallentano e la vela affiora», «WEAX e avvisi di burrasca sul broadcast, la previsione
  chiesta, i nomi sovietici»;
- `test/prove_periscopi.cpp`: «periscopi: l'onda e l'errore di quota coprono la finestra col mare grosso, di rado col
  mare calmo», «tremolio, piuma e danno con la velocita' e il mare», «portate visive per testa, piuma, mare, notte e
  osservatore»;
- `test/prove_vista_periscopio.cpp`: «onde: varianza (Hs/4)^2 e periodo di picco di Pierson-Moskowitz», l'acqua sulla
  finestra, il mare mosso;
- `test/prove_acustica.cpp`: «acustica: il mare grosso sui sensori vicino alla superficie, uguale per tutti»;
- `test/prove_asw_aereo.cpp`: «asw aereo: il mare del meteo sul MAD e sul sonar a immersione (dossier 21 §8)»;
- `test/prove_audio_meteo.cpp`: fulmini come transitori, tuono, colpi di mare, scariche in sala radio;
- `test/prove_meteo_grafica.cpp`: «meteo spento: la vista e' quella di oggi, pixel per pixel», nubi, foschia, fulmini,
  fumo di mare, pioggia, moto delle navi, strati sulla carta;
- `test/prove_missioni.cpp`: «E6: il meteo fissato dall'autore e i messaggi di bordo propri della missione».

## 7. Limiti noti e in arrivo

- **Effetti non ancora collegati.** `docs/15` dà il pezzo M2 per fatto, ma nel codice il meteo non arriva ancora a:
  la ricarica allo snorkel dei diesel, i siluri e i missili del battello vicino alla superficie (tetto, affioramento,
  guasti), le mine ormeggiate col mare grosso, i danni e il soccorso in superficie (allagamento, zattere), lo slamming e
  il sonar di prua delle navi, l'appontaggio degli elicotteri (dossier 21 §7.1-7.4).
- **Lo spruzzo gelante** si calcola ma nessun sistema lo usa.
- **Il meteo è uno per il riquadro**: il punto corregge vento, mare e temperatura coi punti ERA5 e con le celle, ma il
  regime e le nubi sono gli stessi in tutto il teatro.
- **Le celle** nascono e si muovono col vento, ma gli aerei non le aggirano (capitolo E5).
- **Il periodo della vista e della simulazione** usa `Tp = 5 √Hs` del mare formato anche per lo swell; il commento di
  `onde.hpp` cita la forma `Hs = 0,0246 U²`, il generatore usa `0,22 U²/g` col correttivo sopra 10 m/s.
- **Il mare al traverso** per l'affioramento si misura rispetto al vento, non allo swell.
- **Previsione col meteo fisso.** La «previsione» chiesta alla radio parte, ma la risposta non arriva (capitolo C11).
- **Le missioni rapide** scelgono solo fra semplificato e realistico; il meteo fissato non è un loro parametro
  (capitolo F6).

## 8. Riferimenti

- Codice: `nucleo/meteo.hpp`, `nucleo/src/meteo.cpp`, `nucleo/onde.hpp`, `missioni/src/missione_meteo.cpp`,
  `missioni/src/missione.cpp` (`ambiente_acustico`, `meteo_ascolto`, `passo_pomeriggio`), `nucleo/src/periscopi.cpp`
  (`sigma_quota_m`, `fattore_mare`, `portata_meteo`), `nucleo/src/elettronica.cpp` (`r50_m`, pioggia),
  `nucleo/audio_meteo.hpp`.
- Dati: `data/motore/meteo.toml` (da `tools/pipeline/meteo.py`), messaggi 408 e 409 in
  `data/motore/testi/messaggi_bordo.tsv`.
- Ricerca: dossier 21 (`docs/ricerca/21_meteo_e_mare.md`) §2-§8 e §11, note in `docs/ricerca/21_note/`.
- Progetto: `docs/15_meteo_e_mare.md` (pezzi M0-M6), `docs/00_ROADMAP.md` punto 7, `docs/09_schema_missioni.md`
  (chiave `meteo`).
- Capitoli: C2 (propagazione), C3 (rumore ambiente e fulmini), C8 (audio), C9 (periscopi), C10 (radar ed ESM), C11
  (radio), C12 (navigazione), E4 (artiglieria), E5 (ASW aereo), E8 (giorno, notte e cielo), E9 (grafica), G10 (tabella
  del meteo), volume 1 cap. 9.
