# 38 — La Type 22 nel 1989: il Lynx HAS.2/HAS.3, lo Stingray e i tubi STWS Mk 2

Disassemblatore, 31/10/2026. L'ha chiesto la principale: nel gioco la Type 22 non ha né siluri né elicottero, e il suo
sonar era l'SQS-53 di ripiego.

**Le fonti.**
- Solo pagine aperte, senza file scaricati.
- Confidenza: **alta** quando le fonti concordano, **media** con una sola fonte, **bassa** quando è debole o in disaccordo.
- Le scelte di gioco sono **STIMA**.

## 1. Le navi nel 1989

| Dato | Fonte | Confidenza |
|---|---|---|
| Batch 1 (Broadsword, Battleaxe, Brilliant, Brazen, 1979-82): 2 Lynx; due lanciasiluri tripli STWS Mk 2 da 324 mm (sulle prime due montati nel 1980-82); sonar di scafo Type 2016, sostituito dalla Type 2050 su Broadsword e Brilliant a fine anni '80 e sulla Battleaxe nei primi '90 | Navypedia, Broadsword; Wikipedia, Type 22 frigate («1 or 2 × Westland Lynx») | alta per tubi e sonar; media per il numero di Lynx |
| Batch 2 (Boxer, Beaver, Brave, London, Sheffield, Coventry, 1983-88): 2 Lynx, o dalla Brave in poi un Sea King o un Merlin; STWS Mk 2; Type 2016 e la schiera rimorchiata Type 2031Z | Navypedia, Boxer; Wikipedia, Type 22 | alta |
| Batch 3 (Cornwall 1988, Campbeltown e Cumberland 1989): Type 2050 e 2031Z, STWS Mk 2 | Wikipedia, Type 22 | alta (non è una classe del gioco) |
| Su tutte: il Type 162M per i bersagli sul fondo e il Type 2008 per le comunicazioni subacquee | Wikipedia, Type 22 | media |

## 2. Il Lynx HAS.2 / HAS.3

| Dato | Fonte | Confidenza |
|---|---|---|
| Il ruolo antisom è quello del Wasp prima di lui (il concetto MATCH): la nave scopre col suo sonar e dirige l'elicottero, che porta l'arma sul punto | forum (War Thunder) e Navy Lookout; la tattica, non i dati | media |
| Nessun sonar a immersione sui Lynx britannici (l'avevano i francesi, gli olandesi, i tedeschi e altri); nessuna boa | Wikipedia, Westland Lynx | media (la fonte non lo dice per il negativo, ma elenca il sonar solo per le altre marine) |
| Il MAD c'era, per precisare il contatto prima dell'attacco | Navy Lookout (didascalia); Naval Technology («può essere dotato») | media; il tipo non è detto |
| Radar Seaspray nel muso; dalla HAS.3 l'ESM «Orange Crop» | Wikipedia, Westland Lynx; Seaspray | alta |
| Armi antisom: 2 siluri (Mk 46 o Stingray) o 2 bombe di profondità; contro le navi 4 Sea Skua | Wikipedia; scheda del kit Revell (HAS.3) | alta per le armi; media per il carico misto |
| La HAS.3: motori Gem 42-1, 23 nuove (1982-85) e 53 HAS.2 convertite entro il 1989; la RN ne aveva 77 a metà anni '80 | scheda Revell; Navy Lookout | media |

**Velocità e autonomia della HAS.3:** non le ho trovate per questa versione. Le cifre in rete (282 km/h, 5 h 20 con
serbatoi) sono della successiva HMA.8.

## 3. Le armi

| Dato | Fonte | Confidenza |
|---|---|---|
| **Stingray** Mod 0: in servizio dal 1983 (1982 per le prime, affrettate per le Falkland); 324 mm, 2,6 m, 267 kg; carica cava di 45 kg di Torpex; 45 kn; 8-11 km; fino a 800 m; ricerca attiva e poi autonomo, attivo e passivo | seaforces; Naval Technology; Navy Lookout; UK Defence Journal | alta per peso, carica e anno; media per velocità, portata e quota (una fonte) |
| Lo Stingray non entra nei tubi **STWS Mk 1**, fatti per il Mk 46: serve lo **STWS Mk 2** | Wikipedia, Sting Ray (ripreso da UK Defence Journal) | media |
| Lo STWS Mk 2 della Type 22 lanciava Mk 44 e Mk 46; lo Stingray dalla nave «dopo il 1989» | Hazegray, Type 22 | bassa (una fonte, la data è vaga) |
| La **bomba di profondità Mk 11**: era sul Wasp (2 bombe). La Mk XI aviolanciata del 1942 pesa 111,6 kg con 77 kg di Torpex, profondità 7,6 m; quella del dopoguerra (Mod 3) non l'ho trovata | globalmilitary (Wasp); NavWeaps, armi antisom britanniche | bassa |

## 4. Nel gioco (STIMA salvo dove detto)

**Il Lynx** (`asw_aereo.py`, mezzo `lynx`): elicottero senza boe né sonar, col MAD come quello dell'SH-2F, il radar
Seaspray quando ci sarà fra gli emettitori, e due carichi:
- `asw` (predefinito): 2 Stingray;
- `mk46`: 2 Mk 46, il carico di prima dello Stingray.

Per ora non ha le bombe Mk 11: in missione servirebbe il trattamento delle bombe che hanno le PLAB e la Sjunkbomb 51.

Nel controllore il Lynx non ha né boe né dip: lavora sui cue (il contatto passato dalla nave, come nel MATCH) e sul
MAD, e attacca col fix del MAD.

**Lo Stingray** (`siluri.py`, `stingray`):
- 45/45 kn, 11 km, 800 m (da seaforces);
- 45 kg di carica cava, a contatto (la carica cava è fatta per gli scafi doppi sovietici; il modello dei danni non
  la distingue ancora);
- testa attiva e passiva; acquisizione 1.000/1.500 m (STIMA, appena sopra il Mk 46 Mod 5);
- affidabilità 0,8 (STIMA, come il Mk 46); fondale minimo 30 m (STIMA, fatto anche per le acque basse);
- il ping della testa (`ping_testa_stingray`) a 32 kHz (STIMA).

**Il sonar Type 2016** (`acustica.py`, `type2016` e `type2016_attivo`): cilindrico a 64 doghe, di sorveglianza e
d'attacco (Military Periscope). La frequenza non è pubblicata: nel gioco come un sonar di scafo di media frequenza,
nella banda dei 10 kHz come l'SQS-56 (STIMA), con SL 228 dB.

**Le suite:** la Type 22 Batch 1 ha `type2016`; il Batch 2 `type2016` e `type2031z`. Prima avevano l'SQS-53 di
ripiego.

**I tubi STWS Mk 2 e il Lynx imbarcato** sono nella missione (`missione_antisom_navi.cpp`, del runtime): 6 tubi, 2
Lynx. Nel 1989 lo Stingray sulla nave è incerto (§3), quindi la proposta è il Mk 46 dai tubi e lo Stingray dal Lynx.

## 5. Cosa non si è trovato

- La frequenza e il livello della Type 2016.
- Velocità, autonomia e quote d'impiego della Lynx HAS.3.
- Il tipo del MAD del Lynx.
- La Mk 11 del dopoguerra (peso, carica, quote).
- L'anno in cui lo Stingray entrò nei tubi delle Type 22.
