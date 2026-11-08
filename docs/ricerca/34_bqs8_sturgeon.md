# 34 — Il sonar da ghiaccio degli Sturgeon: BQS-8, non BQS-14

Disassemblatore, 30/10/2026. L'ha chiesto la principale. Nel motore uno Sturgeon sotto il pack non vedeva né le chiglie
né le mine, perché il sonar del fondo e del ghiaccio era solo dei Los Angeles (BQS-15). Il nome atteso era «BQS-14/14A».

**Le fonti.**
- Solo pagine aperte, senza file scaricati.
- Confidenza: **alta** quando le fonti concordano, **media** con una sola fonte, **bassa** quando è debole o in disaccordo.
- Le scelte di gioco sono **STIMA**.

## 1. Chi aveva che cosa

| Sistema | Su quali battelli | Fonte | Confidenza |
|---|---|---|---|
| AN/BQS-8 | Gli Sturgeon: SSN 637-639, 646-653, 660-670, 672-684, 686-687, e la Narwhal (SSN 671), insieme al BQQ-2 o al BQQ-5 e al BQS-12/13 | Navypedia, «us_ss_sturgeon» (tabella per gruppi di scafi) | media |
| AN/BQS-14 | La sola Glenard P. Lipscomb (SSN 685), con il BQQ-2 | Navypedia | media |
| AN/BQS-14A | La Parche, per le missioni speciali: «a further development of the BQS-14» | Military Periscope, BQS-14/14A/15/MIDAS (anteprima) | media |
| AN/BQS-15 | I Los Angeles (e le Ohio): sonar per contatti vicini e navigazione sotto il ghiaccio, sopra i 10 kHz, «in or near minefields» | Nuclear Companion; FAS; dossier 06 §1.1 | media |
| AN/BQR-19 «Top Hat» | Le Ohio: corto raggio, ghiaccio verso l'alto e mine | Military Periscope | media |

**Il BQS-14 non era degli Sturgeon di serie.** Il sonar da ghiaccio degli Sturgeon è il **BQS-8**, che guarda in alto per
la volta. Frequenza, portata e settore non sono pubblicati. Il dossier 01 lo dava già come «BQS-8/12/13 per il ghiaccio»,
senza verifica: ora la tabella di Navypedia lo conferma per gruppi di scafi.

**Le mine.** Nessuna fonte aperta dice che il BQS-8 vedesse le mine. La ricerca di mine è attribuita al BQS-14/14A, al
BQS-15 e al BQR-19. Quindi nel gioco lo Sturgeon ha il ghiaccio e non le mine.

## 2. Nel gioco (STIMA salvo dove detto)

Il BQS-8 è una voce di `bqs15_fondo` (acustica.py, `BQS15_FONDO['bqs8']`), con `mine = false` ed emissione `bqs8`:

| Parametro | BQS-8 | Il BQS-15, per confronto |
|---|---|---|
| Settore | ±45° | ±60° |
| Elevazione | da 0° (orizzonte) a +60° | da -30° a +45° |
| Fondo e pareti sotto | no | sì |
| Portata | metà (fattore 0,5: le chiglie a circa 1.250 m, le pareti davanti a 1.600 m) | piena |
| Mine | no | sì |
| Impulso | 40 kHz, SL 205 dB | 25 kHz |

Le regole:
- **L'IA:** uno Sturgeon dell'IA sotto la banchisa tiene il BQS-8 acceso salvo in silenzio, come il BQS-15 (I17) e
  l'MG-70. Vede le chiglie e scende sotto quelle misurate, ma non vede le mine.
- **Il giocatore:** con uno Sturgeon, lo stesso ordine del BQS-15 accende il BQS-8 («BQS-8 acceso»), che fa solo le
  spazzate della volta.
- **La Lipscomb e la Parche** non sono classi del gioco.

## 3. Cosa non si è trovato

- La frequenza, la portata e il settore del BQS-8.
- Se il BQS-8 vedesse ostacoli davanti, oltre alla volta.
- Le date dei montaggi per scafo.
- I dati del BQS-14 della Lipscomb.
