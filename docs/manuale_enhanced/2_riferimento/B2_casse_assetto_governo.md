# B2. Casse, assetto e governo automatico

*Autore: kit, per il runtime. Revisione del 27/10/2026. Stato: rivisto (runtime, 27/10/2026), curatore: kit.*

## 1. A che serve

Decide quanto pesa il battello e chi lo tiene alla quota giusta. Intatto e in immersione, il battello è neutro: le
casse zavorra sono piene d'acqua e i timoni orizzontali fanno il resto (capitolo B1). Le casse entrano in gioco nei
momenti difficili. Con l'**esaurimento d'emergenza** l'aria delle bombole caccia l'acqua e il battello sale, sempre
più in fretta, ma l'aria è poca e più in basso si è, meno spinge. Con le **casse d'assetto** il battello si appesantisce
quanto basta per **posarsi sul fondo** e sparire dal sonar nemico. Sull'Alfa c'è di più, come nel battello vero: gli
**sfoghi** per fermare una risalita, la **compensazione** a pulsanti, il **Tan** che tiene l'assetto da solo e il
**Boksit** che governa rotta e quota a programma. In questo capitolo stanno anche l'immersione d'emergenza e il pilota
automatico.

## 2. Il modello

Il peso del battello lo calcola `BattelloDanni` (`nucleo/battello.hpp`, `nucleo/src/battello.cpp`), lo stesso modello
dei danni (capitolo B3). Lo integra un passo interno di 0,1 s. Ne escono un peso netto e un momento d'assetto
(`ForzeIdrostatiche`) che la dinamica usa (capitolo B1). Nella missione lo collega `DanniSottomarini`
(`missioni/src/danni_sottomarini.cpp`), che riceve anche gli ordini delle casse. Il governo automatico è in
`StazioniMissione` (`missioni/src/stazioni_missione.cpp`).

Il modello del battello del giocatore nasce all'avvio con la regola di simulazione (`DanniSottomarini::prepara`,
chiamata da `Partita`). Quello di un battello dell'IA nasce al primo colpo, al primo urto o al primo esaurimento.

### 2.1 Le casse zavorra principali

Le casse vengono dallo scafo del kit (`data/motore/scafi/<classe>.toml`): sono le celle con `uso = "zavorra"`. Il
volume di una cassa è la parte della sezione fuori dallo scafo resistente, per la lunghezza della cella e la sua
permeabilità (0,98 nei dati del kit). Ogni cassa (`CassaBattello`) ha la sua posizione lungo lo scafo, che dà il
momento.
- **688**: scafo singolo, due casse, a prua (`Z_PRUA`) e a poppa (`Z_POPPA`).
- **Alfa**: doppio scafo, quattordici casse: quelle di prua e di poppa più due laterali, a dritta e a sinistra, per
  ognuno dei sei compartimenti (da `C1D`, `C1S` a `C6D`, `C6S`).

In immersione le casse sono piene d'acqua: è la condizione neutra, e il peso netto è zero. L'aria che ci entra si
misura in metri cubi normali, cioè a 1 bar (`aria_Nm3`). Alla quota `z` occupa il volume che le dà la legge di Boyle:

```
p(z) = p_atm + ρ g z                   ρ = 1025 kg/m³, p_atm = 101.325 Pa
V_aria = min(V_cassa, aria_Nm3 · p_atm / p(z))
spinta = ρ g V_aria                    momento = ρ g V_aria (x_cassa − x_rif)
```

- `x_rif`: il centro dei volumi dei compartimenti, il riferimento dei momenti. Aria a prua: prua su.
- **Ombrinali**: l'aria che non ci sta più, perché in risalita si è espansa oltre il volume della cassa, esce dal
  fondo. La cassa resta piena d'aria e la spinta smette di crescere.
- **Cassa squarciata** (urti ed esplosioni, capitolo B3): non tiene l'aria.

### 2.2 Soffiare: l'esaurimento d'emergenza

`BattelloDanni::esaurisci` apre l'aria delle bombole verso le casse principali.

```
aria delle bombole  = 2,5 · V_casse            (m³ normali, ARIA_CASSE)
portata             = V_casse / 30 s           (m³ normali al secondo, TEMPO_ESAURIMENTO)
```

- `V_casse`: il volume di tutte le casse principali.
- L'aria va alle casse integre in proporzione all'acqua che hanno ancora.
- Il soffio si chiude quando l'aria finisce (evento «aria delle bombole esaurita») o quando si aprono gli sfoghi.
  Con le casse vuote resta aperto ma non consuma aria; se il battello scende e l'acqua rientra, riprende.
- Le bombole durano al più 75 s di soffio (2,5 volte 30 s). In superficie bastano a svuotare le casse in 30 s.

La quota cambia tutto. Un metro cubo normale, alla quota `z`, occupa `p_atm / p(z)` metri cubi. Senza contare
l'espansione in risalita, le bombole intere svuotano:

| Quota | 20 m | 100 m | 150 m | 300 m |
|---|---|---|---|---|
| Pressione (atmosfere) | 3,0 | 10,9 | 15,9 | 30,8 |
| Parte delle casse che le bombole svuotano | 84% | 23% | 16% | 8% |

*Calcolo dalle formule, con `ARIA_CASSE` = 2,5.*

Per questo un soffio profondo comincia piano. Poi, man mano che il battello sale, l'aria si espande e la spinta cresce
da sola: la risalita si autoalimenta ed è difficile da fermare. È quello che dice la dottrina (dossier 02 §9.1).

**Nella missione:**
- **giocatore**: l'ordine di emersione d'emergenza (Ctrl+E) fa partire il soffio (`Partita::avanza`), e la dinamica
  riceve la quota zero con un angolo fino a 30°, anche se è il primo ordine di manovra: i comandi passano al
  giocatore e il battello sale col massimo assetto, qualunque fosse l'ordine di prima;
- **IA**, dopo un allagamento (più di 1 m³ a bordo): se è sopra i 150 m, tenta l'emersione una volta sola, con una
  probabilità estratta fra il 30 e il 70% (`QUOTA_EMERSIONE_IA`, dossier 07 §8.4);
- **IA**, dopo 5 minuti di incendio fuori controllo o di fumo denso: soffia a qualunque quota, per ventilare;
- **IA**, posata sul fondo e pesante: soffia;
- un battello dell'IA che arriva in superficie dopo un soffio, risalendo a più di 1 m/s, fa l'**emersione violenta**:
  la prua esce dall'acqua e ricade (capitoli B3 ed E1).

### 2.3 Sfogare

`BattelloDanni::sfoga` ferma il soffio e apre gli sfoghi delle casse principali. L'aria esce e rientra l'acqua.

```
dN/dt = − V_cassa · p(z) / p_atm / 15 s        per ogni cassa (SFOGO_S)
```

Una cassa piena d'aria si svuota in 15 s, a qualunque quota. Quando l'aria è uscita da tutte le casse, gli sfoghi si
richiudono da soli. Il comando c'è solo sull'Alfa del giocatore (§2.7).

### 2.4 Casse d'assetto e di compensazione

Oltre alle casse principali c'è un volume d'acqua «di assetto» (`acqua_assetto_m3`), contato oltre il neutro e
posto al centro dello scafo: pesa, ma non dà momento.

- **Obiettivo**: chi governa le casse fissa un obiettivo in metri cubi (`BattelloDanni::assetto`). Positivo vuol dire
  acqua in più, battello pesante.
- **Imbarcare** dalle valvole: 0,05 m³/s (`PORTATA_ASSETTO`), cioè 3 t al minuto.
- **Espellere** con le pompe:

  ```
  portata = max(0,03 · e_pompe · clamp(1 − z / 200, 0, 1), 0,002)     m³/s
  ```

  `e_pompe` è l'efficienza delle pompe (capitolo B3). A 200 m e più in basso le pompe danno solo 0,002 m³/s.
- **Riserva sotto il neutro**: si possono espellere al più 15 m³ (`RISERVA_ASSETTO_M3`), l'acqua delle casse di
  compensazione.

### 2.5 Peso variabile e compressione dello scafo (Alfa)

Sull'Alfa del giocatore il peso cambia durante la missione (`DanniSottomarini::pesa_lanci`):
- ogni arma che esce dal tubo alleggerisce la prua: 1,9 t un siluro (`ARMA_KG`), 0,3 t un'esca (`ESCA_KG`), 1 t una
  mina (`MINA_KG`);
- lo scafo si comprime con la quota (`abilita_compressione`):

  ```
  peso in più = 0,5 t · (z − z_assetto) / 100 m          (COMPRESSIONE_ALFA_T_100M)
  ```

  `z_assetto` è la quota di partenza, dove l'equipaggio ha messo il battello in assetto. Sotto pesa di più, sopra di
  meno.

Gli altri battelli non hanno peso variabile: restano neutri.

### 2.6 Tan, l'assetto automatico (Alfa)

Il Tan (Тан) porta l'obiettivo delle casse d'assetto a compensare il peso variabile e l'acqua imbarcata:

```
obiettivo = clamp(−(P_var / g + ρ A_imbarcata) / ρ, −15, +15)       m³
```

- `P_var`: il peso variabile (§2.5). `A_imbarcata`: l'acqua nei compartimenti (capitolo B3).
- L'obiettivo si raggiunge alle portate di §2.4: il Tan non è istantaneo, e in profondità le pompe lo rallentano.
- Oltre i 15 m³ di riserva il Tan non basta: un allagamento grande lo vince.
- L'obiettivo si ricalcola a ogni passo e prende il posto di qualunque altro. Col Tan inserito le 2 t del posarsi
  (§2.9) tornano subito al neutro: è voluto, perché il Tan è l'assetto automatico. Per posarsi appesantiti si
  disinserisce il Tan.
- Si disinserisce da solo con le pompe sotto il 50% di efficienza (messaggio 445).
- Con il Tan inserito la compensazione a mano è ignorata, perché compensa lui. La compensazione a mano non ha un
  tetto in alto; in basso si ferma alla riserva di 15 m³.

Il **guasto delle casse** (la finestrella rossa) si accende con l'aria o le pompe sotto il 50%, o con una cassa
squarciata (`BattelloDanni::guasto_casse`).

### 2.7 Il pannello ПОКС dell'Alfa

Gli ordini delle casse dell'Alfa (`sfoga`, `compensa`, `tan`) li prende `DanniSottomarini::consegna`. Valgono solo
nell'Enhanced e solo per il giocatore su un'Alfa.
- **PRODUV** (soffia): l'esaurimento d'emergenza di §2.2, come Ctrl+E.
- **VENTIL** (sfoga, Ctrl+G): §2.3.
- **URAV+ e URAV−** (Ctrl+K e Ctrl+J): mezzo metro cubo in più o in meno sull'obiettivo delle casse d'assetto.
- **TAN** (Ctrl+N): §2.6. Se le pompe sono sotto il 50% l'inserimento è rifiutato col messaggio 445.

### 2.8 Immersione d'emergenza

Ctrl+D, solo nell'Enhanced. `Missione::quota_immersione_emergenza` (`missioni/src/missione_collisioni.cpp`) calcola la
quota di sicurezza:

```
q = max(45,72 m, per ogni nave entro 5 km: pescaggio + 10 m + diametro + altezza della vela)
q = min(q, quota massima − 10 m, fondo − 15 m)
q = max(q, 10 m)
```

Il diametro è la larghezza della classe del giocatore; senza i dati delle collisioni valgono 10 m di diametro e 6 di
vela, quelli di un 688. La cima della vela passa così 10 m sotto la chiglia più profonda. Poi la dinamica riceve i
giri al massimo, la quota `q` e un angolo fino a 30°, anche come primo ordine. Le costanti sono `EMERGENZA_MIN_M`,
`EMERGENZA_NAVI_M`, `EMERGENZA_MARGINE_M` ed `EMERGENZA_FRANCO_M`.

### 2.9 Posarsi sul fondo

`StazioniMissione::posarsi` (Ctrl+F, solo nell'Enhanced e con la dinamica). Prima i controlli dell'equipaggio:
1. antenna rimorchiata filata o in manovra: rifiuto, messaggio 217;
2. fondo sconosciuto, o oltre la quota di collaudo dello scafo: rifiuto, messaggio 219;
3. pendenza oltre 5°, misurata sul fondale 50 m attorno al battello: rifiuto, messaggio 218.

Poi la manovra (messaggio 216):
1. i giri per 3 nodi, e la quota ordinata al fondo meno 1 m con un angolo di 10° al più;
2. mentre il battello avanza, la quota ordinata segue il fondo sotto di lui;
3. a 3 m dalla quota d'arrivo le macchine si fermano da sole e il battello si adagia con l'abbrivio;
4. con i danni del battello attivi (regola di simulazione) si imbarcano 2 t nelle casse d'assetto, in circa 40 s alle
   valvole: il battello si appoggia (`DanniSottomarini::appesantisci`). Sull'Alfa col Tan inserito il Tan le riporta
   al neutro (§2.6).

Il primo ordine di macchina, di quota, d'angolo o d'emergenza scarica le casse d'assetto con le pompe
(`DanniSottomarini::alleggerisci`), più lente in profondità. Senza la dinamica dell'Enhanced Ctrl+F non fa nulla.

**Effetti del posato** (`nucleo/src/acustica.cpp`):
- la sorgente del battello posato ha 10 dB in meno (`POSATO_DB`, capitolo C4);
- i suoi sensori non hanno rumore di flusso;
- l'eco del sonar attivo nemico si confonde con la riverberazione del fondo (capitolo C1).

**Pesante sul fondo.** Un battello posato con oltre 5 t di peso netto è pesante. Il giocatore riceve il messaggio 282:
senza soffiare non si risale. Con l'aria delle bombole finita, dopo 10 minuti pesante sul fondo il battello è perso
(`FONDO_SENZA_SCAMPO`).

### 2.10 Boksit, il governo dell'Alfa

`StazioniMissione` tiene, per l'Alfa del giocatore nell'Enhanced, il modo di ciascuno dei due assi, rotta e quota
(`ModoBoksit`):
- **MANUALE** e **AUTOMATICO**: sono un'indicazione del modo sul pulto, non due comportamenti. In entrambi la barra
  e la quota ordinata funzionano come sul 688 (capitolo B1), e la quota e la rotta ordinate le tiene comunque il
  motore. Il volume 1 (§2.6) li descrive come modi diversi: vale questo capitolo;
- **PROGRAMMA**: per la rotta, il pilota automatico sui punti di rotta della carta. A ogni punto raggiunto, Boksit
  prende la quota della tappa (se anche la quota è a programma) e la sua velocità.

All'avvio i due assi sono in AUTOMATICO. Ctrl+B (rotta) e Ctrl+U (quota) passano al modo seguente, nell'ordine
AUTOMATICO, PROGRAMMA, MANUALE.

Le **tappe** (`TappaBoksit`) sono i punti di rotta, fino a nove, ognuno con una quota e una velocità facoltative.
Ctrl+T le carica tutte senza quota né velocità propria e mette la rotta a programma: a ogni punto il battello tiene
quota e velocità che ha. Una tappa con la quota o la velocità si dà con l'ordine `programma_tappa`, che Ctrl+T manda
senza valori: oggi nessun tasto li imposta. La velocità di una tappa diventa un ordine di telegrafo, come frazione di
avanti tutta fra 0 e 1,09.

**Fine del programma:**
- a ogni punto raggiunto: messaggio 447;
- all'ultimo punto: messaggio 448, e la rotta torna in automatico;
- un ordine di barra o di accostata lascia il programma: il pilota si disinserisce e la rotta torna in automatico.

Senza punti di rotta il programma non parte (messaggio 211). Sul 688 e nel classico gli ordini di Boksit non valgono.

### 2.11 Il pilota automatico

- **Classico**: segue i punti di rotta della carta, al più nove, uno dopo l'altro. Toccare il timone lo disinserisce.
- **Enhanced**: inserito, ridà i comandi al timoniere automatico della dinamica (`Missione::comandi_del_timoniere`,
  capitolo B1 §2.7), che accosta verso il punto con al più 20° di barra. Il timoniere prende i giri dalla velocità
  ordinata, che l'ultimo ordine di giri ha aggiornato: inserendo il pilota la velocità resta quella ordinata
  (capitolo B1 §2.7). Un ordine di barra o di accostata lo disinserisce; al primo ordine di manovra i
  comandi del giocatore ripartono dal battello com'è.

## 3. Fonti e confidenza

| Parametro | Fonte | Confidenza |
|---|---|---|
| Casse dallo scafo, battello intatto neutro | scafi del kit, `docs/10` §10.3 (decisione 4) | media |
| Legge di Boyle, aria che si espande in risalita | dossier 02 §9.1 | alta per la fisica |
| Aria delle bombole: 2,5 volte le casse | `docs/10` §10.3 (decisione 4) | **STIMA**, bassa |
| Esaurimento in 30 s in superficie | scelta di modello | **STIMA** |
| Sfoghi: 15 s per cassa | dossier 19 §3.2 (10-20 s) | **STIMA** |
| Valvole 0,05 m³/s, pompe 0,03 m³/s fino a 200 m | scelta di modello | **STIMA** |
| Riserva di compensazione 15 m³ | scelta di modello | **STIMA** |
| Peso delle armi: siluro 1,9 t, esca 0,3 t, mina 1 t | dossier 19 | **STIMA** |
| Compressione 0,5 t ogni 100 m | dossier 19 («qualche tonnellata a 600 m») | **STIMA** |
| Tan, Boksit, ПОКС: esistenza e funzione | dossier 19 §3.1-3.2 [SOB] [PRJ] | alta |
| Modi di Boksit e pulsanti di ПОКС | dossier 19 §6.2 | proposta di gioco |
| L'IA che tenta l'emersione sopra i 150 m, nel 30-70% dei casi | dossier 07 §8.4 | media |
| Immersione d'emergenza: 150 piedi, la vela sotto le chiglie | dossier 16 §4.4 | media |
| Posarsi: rifiuti, discesa lenta | `docs/11` §6 | proposta di gioco |
| 2 t per appoggiarsi, 10 dB in meno da posato | scelta di modello | **STIMA** |

Le STIME delle portate e della riserva d'aria sono tarate su due cose che le fonti dicono: un battello allagato in
profondità non si salva col soffio da solo, e uno vicino alla superficie risale in fretta e male.

## 4. Classico ed Enhanced

Nel classico non ci sono casse, aria né peso. L'emersione d'emergenza (Ctrl+E) è un ordine di quota zero a quattro
volte il rateo di quota del modello (`docs/originale/sensori_e_navigazione.md` §7). Non esistono l'immersione
d'emergenza, il posarsi sul fondo né i sistemi dell'Alfa: i tasti non fanno nulla, come nell'originale. Toccare il
fondo dà i messaggi 30 e 9 dell'originale (capitolo C12). Il pilota automatico segue i punti di rotta come nel 1989.

Nell'Enhanced, con la regola di simulazione:
- Ctrl+E è l'esaurimento vero, con l'aria limitata (§2.2);
- Ctrl+D e Ctrl+F ci sono per tutti i battelli del giocatore;
- l'Alfa del giocatore ha ПОКС, Tan e Boksit, con peso variabile e compressione dello scafo;
- i battelli dell'IA soffiano secondo le regole di §2.2, ma non governano le casse d'assetto.

## 5. Come si vede in gioco

- **Manovra (F4)** del 688 (volume 1, §2.4-2.5): il pannello BALLAST CONTROL con le aperture dello scafo, le casse,
  il pilota automatico e l'esaurimento d'emergenza (EMER BLOW).
- **Manovra (F4)** dell'Alfa (volume 1, §2.6): il pulto Boksit con KURS e GLUB e i tre modi, il pulsante del programma
  con la tappa corrente («P 3/7»), il pannello ПОКС con PRODUV, VENTIL, URAV+, URAV−, TAN e la finestrella AVAR.
- **Quadro danni (F3)** (volume 1, cap. 7): l'acqua nei compartimenti, i sistemi aria e pompe.
- **Tasti**: Ctrl+E emersione d'emergenza, Ctrl+D immersione d'emergenza, Ctrl+F posarsi; sull'Alfa Ctrl+B e Ctrl+U i
  modi di Boksit, Ctrl+T il programma, Ctrl+N il Tan, Ctrl+G gli sfoghi, Ctrl+K e Ctrl+J la compensazione.
- **Messaggi**:
  - 216 ci posiamo; 217, 218 e 219 i rifiuti; 282 posati e pesanti;
  - 277 esaurimento d'emergenza; 278 aria delle bombole esaurita;
  - 430 immersione d'emergenza;
  - 443 Tan inserito, 444 disinserito, 445 fuori servizio; 446 avaria alle casse; 449 sfoghi aperti;
  - 447 tappa del programma raggiunta, 448 programma concluso.

## 6. Taratura e prove

- `test/prove_battello.cpp`: «battello: sottomarino danneggiato in immersione» (compartimenti e casse dallo scafo,
  momento dell'acqua, esaurimento, casse d'assetto che a 300 m le pompe quasi non svuotano).
- `test/prove_danni_sottomarini.cpp`:
  - «sottomarini: casse d'assetto per posarsi sul fondo»: le 2 t arrivano alla dinamica e poi si scaricano;
  - «sottomarini colpiti: emersione violenta dopo l'esaurimento delle casse»;
  - «Alfa, casse (dossier 19): sfoghi, compensazione, peso delle armi, compressione, Tan e guasto»;
  - «Alfa, casse: gli ordini sfoga, compensa e tan arrivano al battello del giocatore».
- `test/prove_enhanced_giocatore.cpp`: «enhanced: posarsi sul fondo, con i rifiuti dell'equipaggio»; «enhanced:
  posato con le casse d'assetto, il battello si appoggia e poi si alleggerisce».
- `test/prove_alfa_sistemi.cpp`: «alfa: Boksit a programma segue i punti di rotta con la quota e la velocita' di ogni
  tappa»; «alfa: Boksit non vale per il 688 ne' nel classico».
- `test/prove_comandi.cpp`: «comandi: punti di rotta e pilota automatico, che il timone disinserisce».
- `test/prove_dinamica_sottomarino.cpp`: «dinamica nella missione: i giri danno la velocita' ordinata; Ctrl+E come
  primo ordine sale a 30 gradi» (assetto oltre 25°, mai oltre 30°).

## 7. Limiti noti e in arrivo

- Il 688 non ha casse d'assetto da governare: le usa solo il posarsi. La compensazione a mano e il peso variabile
  sono solo dell'Alfa; gli altri battelli restano neutri anche dopo i lanci.
- Le casse d'assetto sono un volume solo, al centro: spostare acqua fra prua e poppa per correggere l'assetto non si
  può.
- L'aria delle bombole non si ricarica.
- MANUALE e AUTOMATICO di Boksit non cambiano il comportamento: sono un'indicazione del modo, e decide il tipo
  d'ordine che si dà (decisione del 27/10/2026).
- Il Tan, inserito, rimette ogni passo l'obiettivo delle casse d'assetto al neutro, e annulla le 2 t del posarsi. È
  voluto: il Tan è l'assetto automatico.
- Le tappe con quota e velocità proprie non hanno un tasto: Ctrl+T carica i punti senza.
- Il documento di progetto `docs/11` §6 chiede di scendere sotto 2 nodi; il codice scende a 3.
- `docs/10` §10.3 (decisione 4) dice che le casse d'assetto non si simulano; il codice le ha poi aggiunte per il
  posarsi e per l'Alfa (`docs/10` §10.18).
- **I tipi di fondo** (`docs/11` §6, roadmap punto 30) ci sono, nell'Enhanced, per il giocatore e per l'IA
  (`missioni/src/missione_fondo.cpp`, STIMA DI GIOCO). Il tipo viene dal teatro e dalla profondità (`ClimaTeatro::fondo`).
  - **Fango:** la presa comincia dopo un minuto posato ed è piena in 10 minuti. A lasciare il fondo il battello resta
    trattenuto per presa × 180 s da fermo, la metà a 4 nodi. Messaggio 476.
  - **Roccia:** posato e oltre mezzo nodo, ogni minuto con P = 0,05 + 0,05 v, un guasto del 30% al timone o alla
    cupola del sonar di prua (metà e metà) e un danno allo scafo del 2% dell'integrità massima. Messaggio 477.
  - **Prese a mare:** posato sul fango oltre mezz'ora, ogni minuto con P = 0,1, un guasto del 50% alla propulsione
    (raffreddamento in avaria). Messaggio 478.
  - I guasti passano dai danni del battello (`DanniSottomarini`, capitolo B3); i messaggi arrivano solo al giocatore.

  Resta da fare il tipo di fondo dai raster geologici.

## 8. Riferimenti

- Codice: `nucleo/battello.hpp` e `nucleo/src/battello.cpp` (`BattelloDanni`: `esaurisci`, `sfoga`, `assetto`,
  `compensa`, `varia_peso`, `abilita_compressione`, `tan`, `guasto_casse`, `forze`), `missioni/danni_sottomarini.hpp`
  e `missioni/src/danni_sottomarini.cpp` (`consegna`, `appesantisci`, `alleggerisci`, `pesa_lanci`),
  `missioni/stazioni_missione.hpp` e `missioni/src/stazioni_missione.cpp` (`posarsi`, `consegna_boksit`,
  `passo_boksit`), `missioni/src/missione_collisioni.cpp` (`quota_immersione_emergenza`), `app/interfaccia.cpp`
  (`comando_alfa`, `azione_posto`), `app/partita.cpp` (l'esaurimento all'ordine di emersione).
- Dati: `data/motore/scafi/<classe>.toml` (celle di zavorra, `[sottomarino]`), `data/motore/tasti_predefiniti.toml`,
  `data/motore/testi/messaggi_bordo.tsv`.
- Ricerca: dossier 02 §9.1-9.2; dossier 07 §8.4; dossier 16 §4.4; dossier 19 §3.1-3.2 e §6.2.
- Progetto: `docs/10_danni_affondamento_esplosioni.md` §10.3 e §10.18; `docs/11_comandi_e_stazioni.md` §6 e §10;
  `docs/00_ROADMAP.md` punto 30.
- Volume 1: `1_uso/02_battello_e_stazioni.md` §2.4-§2.6. Volume 2: capitoli B1, B3, C1, C4, E1.
