# D5. Carico e ricarica

*Autore: disassemblatore. Revisione del 26/10/2026. Stato: rivisto (curatore: kit).*

## 1. A che serve

Decide che cosa porta il battello e quanto ci vuole a rimettere un'arma nel tubo. Prima di partire, nella **sala
siluri**, il comandante sceglie il carico: quali siluri, quanti missili, se portare le mine, entro i posti del locale e
le armi che il deposito della missione concede. In mare, dopo un lancio, il tubo si ricarica in qualche minuto, e la
ricarica fa rumore: il carrello e la culatta si sentono. Con il **silenzio a bordo** la ricarica è muta ma dura il
doppio. È la scelta fra una seconda salva rapida e restare nascosti.

## 2. Il modello

Il codice è in `nucleo/carico.hpp` (il catalogo e la sala siluri) e `nucleo/armi_giocatore.hpp` (`StazioneArmi`, i
tubi e la ricarica); i dati in `data/motore/carico.toml`, da `tools/pipeline/carico.py`. Nella missione li collega
`StazioniMissione::imposta_carico` (`missioni/stazioni_missione.hpp`).

### 2.1 Il catalogo delle armi

Ogni arma (`SchedaArma`) ha:
- guida, velocità, portata, testata, peso, lunghezza e diametro;
- l'anno d'entrata in servizio e di ritiro, i posti che occupa in rastrelliera (le mine corte mezzo posto), se va nei
  pozzi verticali;
- i limiti: «solo IA», «what-if», «solo nel modo moderno», nucleare;
- la **famiglia**, cioè l'arma della stazione Armi e dei gestori delle missioni (`mk48`, `type53`, `harpoon`,
  `tomahawk`, `sea_lance`, `esca`, `mina`, `attacco_terra`, `esca_mobile`), e il modello del siluro (`siluro`, la voce
  di `siluri.toml`, capitolo D1);
- l'**efficacia**, da 0 a 3 stelle contro sottomarini in immersione, navi in superficie, bersagli a terra e come mina:
  una STIMA dalla guida e dalla testata, per la tabella della sala siluri;
- la sagoma per la grafica, la fonte e la confidenza.

Le esche mobili da tubo (MG-74 Korund sovietica, 1974; MOSS Mk 70 americana, 1976) sono siluri per il carico: un posto
in rastrelliera e un tubo.

<!-- generato: armi_carico -->
| Arma | Marina | Famiglia | Guida | Velocità (kn), portata (km), testata (kg) | Posti | In servizio | Contro sottomarini / superficie / terra / come mina | Limiti | Fonte, confidenza |
|---|---|---|---|---|---|---|---|---|---|
| Mk 48 Mod 4 | USA | mk48 | filoguidato, poi testa attiva o passiva | 55, 38, 293 | 1 | dal 1980 | ★★★ / ★★★ / ☆☆☆ / ☆☆☆ | — | dossier 14 §2.1; alta |
| Mk 48 ADCAP | USA | mk48 | filoguidato, testa attiva o passiva a scansione elettronica | 60, 32, 295 | 1 | dal 1988 | ★★★ / ★★★ / ☆☆☆ / ☆☆☆ | al più 6 nei primi due anni | dossier 14 §2.1 e §5.1; media |
| UGM-84 Harpoon | USA | harpoon | inerziale, radar attivo in banda J; capsula dal tubo | 560, 140, 221 | 1 | dal 1981 | ☆☆☆ / ★★★ / ☆☆☆ / ☆☆☆ | — | dossier 14 §2.2; media |
| UGM-109B Tomahawk TASM | USA | tomahawk | inerziale, radar attivo, volo radente | 475, 460, 450 | 1 (pozzo) | dal 1983, ritirata nel 1994 | ☆☆☆ / ★★★ / ☆☆☆ / ☆☆☆ | — | dossier 14 §2.3; alta |
| UGM-109C Tomahawk TLAM-C | USA | attacco_terra | inerziale, TERCOM e DSMAC | 475, 1250, 450 | 1 (pozzo) | dal 1986 | ☆☆☆ / ☆☆☆ / ★★★ / ☆☆☆ | — | dossier 14 §2.3; alta |
| UUM-125 Sea Lance | USA | sea_lance | inerziale; capsula, poi siluro leggero Mk 50 a paracadute | 860, 185, 45 | 1 | dal 0 | ★★★ / ☆☆☆ / ☆☆☆ / ☆☆☆ | what-if | dossier 14 §2.5; bassa |
| Mk 67 SLMM | USA | mina | mina mobile: corre fino a 13 km, poi si posa sul fondo | 20, 13, 150 | 1 | dal 1983 | ★☆☆ / ★★☆ / ☆☆☆ / ★★★ | — | dossier 14 §2.6; media |
| Mk 60 CAPTOR | USA | mina | ancorata: libera un siluro Mk 46 al passaggio di un sottomarino | —, —, 44 | 1 | dal 1979 | ★★☆ / ☆☆☆ / ☆☆☆ / ★★★ | — | dossier 14 §2.7; alta |
| MOSS Mk 70 | USA | esca_mobile | esca semovente da tubo: imita rumore ed eco del battello e si allontana | 10, —, — | 1 | dal 1976 | ☆☆☆ / ☆☆☆ / ☆☆☆ / ☆☆☆ | what-if, solo nel modo moderno | dossier 05 §5.1; bassa |
| ADC Mk 2 | USA | esca | esca acustica a quota fissa, elica intubata | —, —, — | 0 | dal 1980 | ☆☆☆ / ☆☆☆ / ☆☆☆ / ☆☆☆ | — | dossier 14 §2.8, dossier 05 §5.1; media |
| Tigerfish Mk 24 Mod 1 | Regno Unito | mk48 | filoguidato, testa attiva o passiva | 35, 17, 134 | 1 | dal 1987 | ★★☆ / ★★☆ / ☆☆☆ / ☆☆☆ | — | dossier 01 §6.1, §2.3; media |
| SET-65 | URSS | type53 | testa attiva o passiva | 40, 16, 205 | 1 | dal 1965 | ★★★ / ★★☆ / ☆☆☆ / ☆☆☆ | — | dossier 14 §2.9; media |
| 53-65K | URSS | type53 | segui-scia, immune alle esche | 45, 19, 300 | 1 | dal 1969 | ☆☆☆ / ★★★ / ☆☆☆ / ☆☆☆ | — | dossier 14 §2.9; media |
| SAET-60M | URSS | type53 | testa passiva | 40, 15, 300 | 1 | dal 1969 | ★★☆ / ★★☆ / ☆☆☆ / ☆☆☆ | — | dossier 14 §2.9; bassa |
| USET-80 | URSS | type53 | testa attiva o passiva | 45, 18, 250 | 1 | dal 1980 | ★★★ / ★★☆ / ☆☆☆ / ☆☆☆ | — | dossier 14 §2.9; bassa |
| TEST-71M | URSS | type53 | filoguidato, testa attiva o passiva | 40, 15, 205 | 1 | dal 1977 | ★★★ / ★★☆ / ☆☆☆ / ☆☆☆ | — | dossier 14 §2.9; bassa |
| VA-111 Shkval | URSS | type53 | supercavitante a razzo, non guidato | 200, 11, 210 | 1 | dal 1977 | ★★☆ / ★★☆ / ☆☆☆ / ☆☆☆ | solo IA | dossier 14 §2.10; bassa |
| RPK-2 Vyuga (SS-N-15) | URSS | sea_lance | razzo antisom dal tubo, carica di profondità | —, 45, — | 1 | dal 1969 | ★★☆ / ☆☆☆ / ☆☆☆ / ☆☆☆ | solo IA | dossier 14 §2.11; bassa |
| PMR-1 Kal'mar | URSS | mina | mina-razzo antisom ormeggiata: lancia un razzo subacqueo contro un sottomarino sopra di lei | —, —, 200 | 1 | dal 1970 | ★★☆ / ☆☆☆ / ☆☆☆ / ★★★ | — | dossier 14 §2.12 [RuPMR1] [AMPMR2]; media |
| PMR-2 | URSS | mina | mina-razzo antisom ormeggiata a 200-400 m: razzo a 80-100 m/s, attacco in 7 s al più | —, —, 175 | 1 | dal 1973 | ★★★ / ☆☆☆ / ☆☆☆ / ★★★ | — | dossier 14 §2.12 [AMPMR2] [WFPMR2]; media |
| MDT | URSS | mina | mina da fondo acustico-induttiva, fino a 50 m di fondale | —, —, 600 | 0,5 | dal 1953 | ★☆☆ / ★★☆ / ☆☆☆ / ★★★ | — | dossier 14 §2.12 [WPMDT] [AMMDT]; media |
| MG-74 Korund | URSS | esca_mobile | esca semovente da tubo: imita rumore ed eco del sottomarino fino a 250 m | 10, —, — | 1 | dal 1974 | ☆☆☆ / ☆☆☆ / ☆☆☆ / ☆☆☆ | — | dossier 05 §5.2 e §7 [KREM]; bassa |
| MG-84 Korund | URSS | esca | esca che disturba o imita la firma del battello | —, —, — | 0 | dal 1974 | ☆☆☆ / ☆☆☆ / ☆☆☆ / ☆☆☆ | — | dossier 14 §2.13; bassa |
<!-- fine: armi_carico -->

### 2.2 I locali siluri

Per classe e variante (`LocaleSiluri`): tubi e calibro, rastrelliera, pozzi verticali, lanciatori d'esche ed esche,
quante armi in acqua al più, e che cosa c'è in ogni posizione della stazione Armi.

<!-- generato: locali_siluri -->
| Battello | Tubi (calibro) | Rastrelliera | Pozzi verticali | Lanciatori d'esche, esche | Armi in acqua al più | Fonte |
|---|---|---|---|---|---|---|
| Los Angeles (Flight I) | 4 (533 mm) | 21 | — | 2, 6 | 4 | dossier 14 §1.2 |
| Los Angeles (Flight II) | 4 (533 mm) | 21 | 12 | 2, 6 | 4 | dossier 14 §1.2 |
| Los Angeles (688I) | 4 (533 mm) | 21 | 12 | 2, 6 | 4 | dossier 14 §1.2 |
| Alfa (progetto 705) | 5 (533 mm) | 13 | — | 1, 4 | 6 | dossier 14 §1.3 e §4.2 |
<!-- fine: locali_siluri -->

### 2.3 La sala siluri

`SalaSiluri` parte dai limiti della missione (`Missione::limiti_carico()`: deposito, tipi negati, tipo di missione,
variante del battello, ospiti, what-if; `docs/09_schema_missioni.md` §5.3):
- **i posti**: tubi più rastrelliera meno gli ospiti, condivisi da siluri, missili, mine ed esche mobili. I pozzi sono
  a parte e solo per le armi da pozzo; le esche dei lanciatori sono a parte, fino alla scorta del locale;
- **il deposito**: accetta famiglie o varianti. Senza deposito (le missioni originali) vale la `dotazione`
  dell'originale, che è insieme il massimo e il carico predefinito;
- **le varianti**: una variante prende dal deposito della sua famiglia se la scheda lo consente (`da_famiglia`), con
  il tetto dei primi due anni di servizio (l'ADCAP nel 1989: al più 6). Il TLAM-C, l'USET-80, il TEST-71M e le mine ci
  sono solo se la missione li dichiara;
- **i divieti**: `disponibili()` dà per ogni arma della marina quante ne restano e perché no: «in servizio dal …»,
  «ritirata nel …», «negata dalla missione», «what-if», «solo IA», «nucleare», «non al deposito»;
- **il modo della partita**: le what-if marcate «moderno» (la MOSS sul 688) le consente solo il modo moderno. Le altre
  what-if (la Sea Lance) le consente l'opzione del giocatore;
- **i carichi**: `carico_standard(tipo)` adattato al deposito (un'arma che manca lascia il posto alla predefinita della
  famiglia, poi al siluro principale); `carico_predefinito()` come l'originale (sul Flight I il missile del tubo 7 va
  nell'ultimo tubo da siluri, sul Flight II nei pozzi);
- **i compiti**: oltre ai carichi del dossier 14 §5.1 ce ne sono quattro per il 688 (`caccia`, `saturazione`,
  `costiero`, `barriera`) e quattro per l'Alfa (`caccia`, `agguato`, `barriera`, `costiero`). I mix sono **STIMA**: le
  fonti dicono che cosa si può imbarcare, non come si combina per compito. Nome, nota e fonte stanno in `[compiti]` di
  `carico.toml` (`nucleo/compiti_carico.hpp`). Le chiavi non coincidono con un `tipo_missione` delle missioni o
  dell'editor: il carico standard di un'unità dell'IA si sceglie per tipo di missione (capitolo D1), quindi le partite
  non cambiano. Per questo l'Alfa non ha `scorta`: oggi una missione «scorta» le dà il carico antisom;
- **gli avvisi**: tubi vuoti, posti e pozzi liberi, esche mancanti, minamento senza mine, armi what-if;
- `scarto_dal_standard()` misura quanto il carico si scosta dallo standard del tipo, per il debriefing e la carriera.

Dai carichi per compito sono esclusi di proposito la Sea Lance (l'opzione what-if del giocatore è spenta), la MOSS (solo
nel modo moderno, e i carichi del 688 restano uguali nei due modi), Shkval e Vyuga (solo IA), USET-80 e TEST-71M
(sull'Alfa non confermati).

La MG-74 dell'Alfa, storica, entra in rastrelliera prima dei siluri. La MOSS del 688, what-if, resta al deposito: nel
modo moderno la carica il giocatore, nel 1989 è negata.

<!-- generato: carichi_standard -->
| Battello | Missione | Nei tubi | In rastrelliera | Nei pozzi |
|---|---|---|---|---|
| Los Angeles (Flight I) | antisom (predefinito) | 4 Mk 48 Mod 4 | 19 Mk 48 Mod 4, 2 UGM-84 Harpoon | — |
| Los Angeles (Flight I) | antinave | 3 Mk 48 Mod 4, 1 UGM-84 Harpoon | 13 Mk 48 Mod 4, 4 UGM-84 Harpoon, 4 UGM-109B Tomahawk TASM | — |
| Los Angeles (Flight I) | attacco terra | 2 Mk 48 Mod 4, 2 UGM-109C Tomahawk TLAM-C | 13 Mk 48 Mod 4, 6 UGM-109C Tomahawk TLAM-C, 2 UGM-84 Harpoon | — |
| Los Angeles (Flight I) | minamento | 2 Mk 48 Mod 4, 2 Mk 67 SLMM | 9 Mk 48 Mod 4, 10 Mk 67 SLMM, 2 UGM-84 Harpoon | — |
| Los Angeles (Flight I) | scorta | 4 Mk 48 Mod 4 | 17 Mk 48 Mod 4, 4 UGM-84 Harpoon | — |
| Los Angeles (Flight I) | caccia | 4 Mk 48 ADCAP | 2 Mk 48 ADCAP, 19 Mk 48 Mod 4 | — |
| Los Angeles (Flight I) | saturazione | 2 Mk 48 Mod 4, 2 UGM-84 Harpoon | 9 Mk 48 Mod 4, 8 UGM-84 Harpoon, 4 UGM-109B Tomahawk TASM | — |
| Los Angeles (Flight I) | costiero | 3 Mk 48 Mod 4, 1 UGM-84 Harpoon | 11 Mk 48 Mod 4, 4 UGM-84 Harpoon, 6 Mk 67 SLMM | — |
| Los Angeles (Flight I) | barriera | 2 Mk 48 Mod 4, 2 Mk 60 CAPTOR | 9 Mk 48 Mod 4, 10 Mk 60 CAPTOR, 2 UGM-84 Harpoon | — |
| Los Angeles (Flight II) | antisom (predefinito) | 4 Mk 48 Mod 4 | 19 Mk 48 Mod 4, 2 UGM-84 Harpoon | 4 UGM-109B Tomahawk TASM |
| Los Angeles (Flight II) | antinave | 3 Mk 48 Mod 4, 1 UGM-84 Harpoon | 17 Mk 48 Mod 4, 4 UGM-84 Harpoon | 8 UGM-109B Tomahawk TASM |
| Los Angeles (Flight II) | attacco terra | 4 Mk 48 Mod 4 | 19 Mk 48 Mod 4, 2 UGM-84 Harpoon | 12 UGM-109C Tomahawk TLAM-C |
| Los Angeles (Flight II) | minamento | 2 Mk 48 Mod 4, 2 Mk 67 SLMM | 9 Mk 48 Mod 4, 10 Mk 67 SLMM, 2 UGM-84 Harpoon | 4 UGM-109B Tomahawk TASM |
| Los Angeles (Flight II) | scorta | 4 Mk 48 Mod 4 | 17 Mk 48 Mod 4, 4 UGM-84 Harpoon | 4 UGM-109B Tomahawk TASM |
| Los Angeles (Flight II) | caccia | 4 Mk 48 ADCAP | 2 Mk 48 ADCAP, 19 Mk 48 Mod 4 | 4 UGM-109B Tomahawk TASM |
| Los Angeles (Flight II) | saturazione | 2 Mk 48 Mod 4, 2 UGM-84 Harpoon | 13 Mk 48 Mod 4, 8 UGM-84 Harpoon | 12 UGM-109B Tomahawk TASM |
| Los Angeles (Flight II) | costiero | 3 Mk 48 Mod 4, 1 UGM-84 Harpoon | 11 Mk 48 Mod 4, 4 UGM-84 Harpoon, 6 Mk 67 SLMM | 6 UGM-109B Tomahawk TASM, 6 UGM-109C Tomahawk TLAM-C |
| Los Angeles (Flight II) | barriera | 2 Mk 48 Mod 4, 2 Mk 60 CAPTOR | 9 Mk 48 Mod 4, 10 Mk 60 CAPTOR, 2 UGM-84 Harpoon | 4 UGM-109B Tomahawk TASM |
| Los Angeles (688I) | antisom (predefinito) | 4 Mk 48 Mod 4 | 19 Mk 48 Mod 4, 2 UGM-84 Harpoon | 4 UGM-109B Tomahawk TASM |
| Los Angeles (688I) | antinave | 3 Mk 48 Mod 4, 1 UGM-84 Harpoon | 17 Mk 48 Mod 4, 4 UGM-84 Harpoon | 8 UGM-109B Tomahawk TASM |
| Los Angeles (688I) | attacco terra | 4 Mk 48 Mod 4 | 19 Mk 48 Mod 4, 2 UGM-84 Harpoon | 12 UGM-109C Tomahawk TLAM-C |
| Los Angeles (688I) | minamento | 2 Mk 48 Mod 4, 2 Mk 67 SLMM | 9 Mk 48 Mod 4, 10 Mk 67 SLMM, 2 UGM-84 Harpoon | 4 UGM-109B Tomahawk TASM |
| Los Angeles (688I) | scorta | 4 Mk 48 Mod 4 | 17 Mk 48 Mod 4, 4 UGM-84 Harpoon | 4 UGM-109B Tomahawk TASM |
| Los Angeles (688I) | caccia | 4 Mk 48 ADCAP | 2 Mk 48 ADCAP, 19 Mk 48 Mod 4 | 4 UGM-109B Tomahawk TASM |
| Los Angeles (688I) | saturazione | 2 Mk 48 Mod 4, 2 UGM-84 Harpoon | 13 Mk 48 Mod 4, 8 UGM-84 Harpoon | 12 UGM-109B Tomahawk TASM |
| Los Angeles (688I) | costiero | 3 Mk 48 Mod 4, 1 UGM-84 Harpoon | 11 Mk 48 Mod 4, 4 UGM-84 Harpoon, 6 Mk 67 SLMM | 6 UGM-109B Tomahawk TASM, 6 UGM-109C Tomahawk TLAM-C |
| Los Angeles (688I) | barriera | 2 Mk 48 Mod 4, 2 Mk 60 CAPTOR | 9 Mk 48 Mod 4, 10 Mk 60 CAPTOR, 2 UGM-84 Harpoon | 4 UGM-109B Tomahawk TASM |
| Alfa (Progetto 705) | antisom (predefinito) | 3 SET-65, 2 53-65K | 6 SET-65, 5 53-65K, 2 MG-74 Korund | — |
| Alfa (Progetto 705) | antinave | 2 SET-65, 3 53-65K | 6 SET-65, 7 53-65K | — |
| Alfa (Progetto 705) | minamento | 3 SET-65, 2 53-65K | 2 SET-65, 6 PMR-2, 10 MDT | — |
| Alfa (Progetto 705) | caccia | 5 SET-65 | 11 SET-65, 2 MG-74 Korund | — |
| Alfa (Progetto 705) | agguato | 3 SAET-60M, 2 53-65K | 8 SAET-60M, 3 53-65K, 2 MG-74 Korund | — |
| Alfa (Progetto 705) | barriera | 3 SET-65, 2 53-65K | 3 SET-65, 10 PMR-2 | — |
| Alfa (Progetto 705) | costiero | 3 SET-65, 2 53-65K | 2 SET-65, 1 53-65K, 20 MDT | — |
<!-- fine: carichi_standard -->

### 2.4 La ricarica

Ogni tubo d'arma passa per questi stati (`StatoTubo`):

```
vuoto → in ricarica → carico → in armamento (20 s) → armato
```

- **in ricarica**: l'arma va dalla rastrelliera al tubo. Al più due ricariche insieme; una terza va in coda;
- **carico**: l'arma è nel tubo, asciutto. In pattugliamento i tubi partono carichi, presi dalle scorte;
- **in armamento**: i 20 s dell'originale per allagare, equalizzare e aprire la porta;
- dopo un lancio la ricarica parte da sola e, finita, il tubo si arma da solo.

Il clic sul tubo è l'ordine `carica_tubo` di sempre: su un tubo vuoto ricarica (o mette in coda), in ricarica annulla
(l'arma torna in rastrelliera), carico arma, armato o in armamento disarma (l'arma resta nel tubo). `scegli_arma_tubo`
cambia l'arma di un tubo: vuoto la prende alla prossima ricarica, carico la scambia con una ricarica intera, in
ricarica ricomincia con l'arma nuova, armato va prima disarmato.

**Tempi** (`parametri_ricarica(classe)`):

| Classe | Secondi per tubo | Perché |
|---|---|---|
| Alfa | 75 | caricatore automatico |
| Trafalgar, Victor III | 240 | |
| Los Angeles e le classi senza dati | 300 | |
| Sturgeon | 420 | prima del carrello idraulico del 1976 |

**Rumore**: fuori dal silenzio, un colpo ogni 60 s di lavoro, un evento acustico di 2 s che l'acustica vede come
+10 dB sulla sorgente del battello: cinque colpi per un tubo del 688.

**Silenzio a bordo** (ordine `silenzio_a_bordo`): la ricarica dura il doppio (`fattore_silenzio` 2) e non fa colpi.
Il rumore viene dalla velocità del carrello e dai colpi della catena e della culatta, che un movimento lento evita.
Fermarla del tutto renderebbe impossibile un lungo avvicinamento silenzioso dopo una salva.

**Pozzi e casi particolari:**
- la posizione 7 del Flight II sono i pozzi verticali: dopo un lancio la cella dopo è subito carica;
- un missile in un tubo da siluri ha le regole del tubo 7 dell'originale; il TLAM-C vuole la quota periscopica;
- le mine si lanciano dal tubo come le altre armi, ma non contano fra le armi in acqua: la posa e il campo minato sono
  nel capitolo D4;
- l'esca mobile si ricarica, si arma («esca pronta») e si lancia come un siluro, e conta fra le armi in acqua;
- le esche dei lanciatori hanno i loro lanciatori e restano come nel classico.

### 2.5 Nella missione

`StazioniMissione::imposta_carico(m, carico, catalogo)` è chiamata dall'app dopo la sala siluri, prima del primo passo:
- le posizioni partono con le armi scelte, «carico», e la ricarica si accende;
- le scorte si tengono per variante; la famiglia resta quella delle regole dell'originale;
- `Missione::imposta_tubi` dà al gestore di lancio la famiglia di ogni tubo; a ogni lancio `Missione::variante_tubo`
  passa la variante uscita dal tubo, così un ADCAP o un SET-65 corre con il suo modello;
- il TLAM-C lanciato è solo l'evento `lancio_giocatore` nel registro, senza bersaglio in mare;
- senza la sala siluri resta il comportamento di prima: i tubi carichi dalla dotazione.

## 3. Fonti e confidenza

| Parametro | Fonte | Confidenza |
|---|---|---|
| Armi: prestazioni, anni di servizio | dossier 14 §2 (per arma, nella tabella) | per arma |
| Locali, tubi, rastrelliere, pozzi | dossier 14 §1.2 | alta (688), media (Alfa) |
| Carichi standard | dossier 14 §5 | media |
| Carichi per compito | nessuna fonte aperta sui mix | **STIMA** |
| SAET-60M sull'Alfa | dossier 14 §1.3 | bassa |
| Tetto dei primi due anni (ADCAP 6 nel 1989) | dossier 14 §5.1 | **STIMA** |
| Tempi di ricarica per classe | dossier 14 §1.5 e §5.4 | **STIMA** |
| Rumore della ricarica, silenzio a bordo | dossier 14 §5.4 | **STIMA** |
| Efficacia in stelle | dossier 14 §2, dossier 15 §7.5 | **STIMA DI GIOCO** |
| Esche mobili MG-74 e MOSS | dossier 05 §5 | media |

## 4. Classico ed Enhanced

Nel classico il battello parte con la `dotazione` e i `tubi` della missione. La ricarica è quella dell'originale
(`docs/originale/sensori_e_navigazione.md` §10.1): il secondo clic su un tubo vuoto toglie una scorta del tipo d'arma
e il tubo è armato dopo 20 s, in tutte le scene; dopo un lancio il tubo si ricarica da solo nello stesso modo. Non ci
sono sala siluri, varianti, rastrelliera, tempi di ricarica, rumore né silenzio a bordo. Il rilievo delle 110 partite
resta identico.

Nell'Enhanced la ricarica realistica si accende da sola quando la missione passa ai siluri dell'Enhanced, e la sala
siluri c'è con la regola di simulazione.

## 5. Come si vede in gioco

- **Sala siluri** (volume 1, cap. 6): le schede delle armi con le stelle, i posti, i divieti col motivo, i carichi
  standard con l'elenco per compito, gli avvisi.
- **Stazione Armi**: lo stato di ogni tubo, i secondi che mancano, la coda, le ricariche in corso, il silenzio.
- **Messaggi**: 348 ricarica iniziata (solo a mano), 349 in coda, 350 annullata; 351 e 352 inizio e fine del
  silenzio; 353-359 tubo n ricaricato; 360 disarmato; 361 cambio d'arma con ricarica, 362 arma scelta per il tubo,
  363 prima va disarmato, 365 arma non in rastrelliera.
- **Debriefing e carriera**: lo scarto dal carico standard (capitolo F2).

## 6. Taratura e prove

Prove:
- `test/prove_carico.cpp` (`carico:`): il catalogo, i posti, il deposito, le varianti e il tetto dei primi anni, i
  divieti, i carichi standard e predefiniti, gli avvisi, le esche mobili, l'efficacia;
- `test/prove_carico.cpp` (`carichi per compito:`): ognuno sta nel suo locale, con armi del gioco, e con questo deposito
  sale intero; la partenza e l'IA non cambiano, e le chiavi nuove non sono tipi di missione; il deposito e l'anno
  comandano;
- `test/prove_schermata_siluri.cpp` (`sala siluri:`): l'elenco si apre con C e col clic, e il consigliato non si applica
  da solo; dall'elenco si applica coi tasti e col clic, e poi si modifica a mano; l'elenco dice che cosa sale davvero, e
  lo scarto che pesa in carriera; il disegno dell'elenco evidenzia il consigliato e non tocca il carico;
- `test/prove_ricarica.cpp` (`ricarica:`): gli stati, i tempi per classe, le due ricariche e la coda, il silenzio,
  i colpi, i pozzi del Flight II, il cambio d'arma.

## 7. Limiti noti e in arrivo

- Il silenzio a bordo per ora tocca solo le ricariche; più avanti potrà toccare anche il rumore proprio.
- I tempi di ricarica non dipendono dall'equipaggio né dai danni al locale.
- L'IA dell'Enhanced ha i suoi carichi, le varianti e la guida via filo dalla fase 4 delle interazioni fra IA
  (`missioni/src/missione_armi_ia.cpp`, capitolo E6 §2.13). Un tubo dell'IA si ricarica in 300 s fissi (`RICARICA_S`,
  STIMA), senza i tempi per arma del giocatore.

## 8. Riferimenti

- Codice: `nucleo/carico.hpp`, `nucleo/src/carico.cpp`, `nucleo/armi_giocatore.hpp`, `nucleo/src/armi_giocatore.cpp`,
  `missioni/stazioni_missione.hpp`, `app/schermata_siluri.cpp`.
- Dati: `data/motore/carico.toml` (da `tools/pipeline/carico.py`).
- Ricerca: dossier 14 §1, §2, §5; dossier 05 §5; dossier 15 §7.5.
- Progetto: `docs/09_schema_missioni.md` §5.3.
