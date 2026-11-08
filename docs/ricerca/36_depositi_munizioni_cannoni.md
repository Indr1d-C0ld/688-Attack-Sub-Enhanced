# 36 — I depositi delle munizioni dei cannoni navali (classi del gioco, 1989)

trailer-1, 31/10/2026, per il runtime: quanti colpi ha ogni pezzo pronti all'affusto, quanti in deposito, a che cadenza
li tira, quanto ci vuole a ricaricare il caricatore e se lo si fa in combattimento. Completa il dossier 22 (artiglieria
contro sottomarini, §3.1) e `data/motore/artiglieria.toml`, che oggi ha `colpi_pronti` e `ricarica_s` ma non il deposito.

**Le fonti.**
- Solo pagine aperte, senza file scaricati.
- Confidenza: **alta** quando la fonte riporta la scheda ufficiale o due fonti concordano; **media** con una sola fonte
  seria (NavWeaps sulle armi sovietiche, ru.wikipedia, Navypedia); **bassa** con fonti deboli o in disaccordo.
- **STIMA** = numero proposto per il gioco dove le fonti tacciono, da tarare; **[CALC]** = conto da numeri citati.
- Le sigle fra parentesi quadre sono nella tabella del §6.

---

## 1. Sintesi

- **I pezzi occidentali hanno un caricatore piccolo e un deposito grande.** Il Mk 75 ha 80 colpi pronti, il Mk 45 20
  nel tamburo, il Mk 42 40 nei due tamburi; in deposito 475-600 colpi per pezzo da 127 mm. Il caricatore si rifornisce
  dal deposito sotto coperta, a mano.
  - Nel **Mk 45** il rifornimento è continuo: sei serventi caricano il tamburo mentre il pezzo spara, quindi la cadenza
    sostenuta è quella nominale finché dura il deposito.
  - Nel **Mk 75** il tamburo da 70 si ricarica a mano da due-tre serventi; il tempo non è pubblicato.
- **I pezzi sovietici sono a caricamento automatico dal deposito** (AK-100, AK-726 coi montacarichi, AK-176), ma hanno
  limiti di raffica per il calore: l'AK-726 tira 40-45 colpi per canna, poi 3 minuti di raffreddamento; l'AK-176 70
  colpi, poi 25-30 minuti; l'AK-725 100 colpi.
- **I CIWS sovietici hanno il nastro sull'affusto e basta:** l'AK-630 2000 colpi, cioè meno di mezzo minuto di fuoco
  continuo, in raffiche da 200 o 400; la versione M ha in più 1000 colpi di riserva. Rifare il nastro è un lavoro a mano,
  non da combattimento (STIMA).
- **I nordici:**
  - il Bofors 57 Mk 2 della Stockholm ha 120 colpi pronti, che si ricaricano in 2 minuti dai montacarichi;
  - il Bofors 40/70 delle Oslo e delle Hauk ha 18 colpi nel caricatore automatico e 96 sull'affusto, in piastrine da 4
    caricate a mano anche durante il tiro;
  - il 76/50 Mk 33 binato delle Oslo è caricato da 11 serventi: 200-300 colpi nei depositi pronti e 1200 in deposito
    per canna.
- **Nel 1989 tre Oslo su cinque hanno ancora due binati da 76.** La Trondheim (11/1987) e la Narvik (10/1988) hanno già
  un binato e il Bofors 40/70; la Stavanger lo riceve nel giugno 1989 `[NP-Oslo]`.
- **Non ci sono pezzi da 76 svedesi** fra le classi del gioco: la Stockholm ha il 57 e il 40. **L'AK-176** non è su
  nessuna classe del gioco (va sulle Nanuchka III, Grisha V, Tarantul), ma lo riporto perché l'ha chiesto la principale.

---

## 2. Tabella per pezzo

| Pezzo | Colpi pronti all'affusto | Deposito | Cadenza | Ricarica del caricatore | In combattimento | Fonte | Conf. |
|---|---|---|---|---|---|---|---|
| **Mk 75 76/62** (OTO Compact) | **80**: tamburo da 70, montacarichi a vite da 6, tamburo di culatta da 4 | n.v. per la Perry; **STIMA 600** (come i 127 mm USA) | 80-85/min automatico | 2-3 serventi sotto coperta ricaricano il tamburo a mano; tempo n.v.; **STIMA 3 min** per 70 colpi | n.v.; **STIMA sì**, a pezzo fermo (il tamburo gira col tiro) | `[NW-Mk75]` `[GC-Mk75]` | alta (affusto), STIMA (deposito, ricarica) |
| **Mk 42 5"/54** (Knox, Mod 9) | **40**: due tamburi da 20, ciascuno con un tamburo cartocci e uno proietti | **600 per pezzo** (Knox) | 28/min dopo il 1968 (34 coi due lati, 17 con un lato) | i serventi mettono a mano proietti e cartocci nei tamburi; 13 uomini nel Mod 9 | sì: è l'unico passaggio manuale del ciclo, continuo durante il tiro (STIMA) | `[NW-Mk42]` | alta |
| **Mk 45 5"/54** (Spruance, Kidd, Ticonderoga) | **20** nel tamburo di caricamento | **600 per pezzo** (Ticonderoga); **475-500** le altre classi (Spruance e Kidd per esclusione) | 20/min nominale, 16 col graduatore di spoletta | continua: 4 caricatori, un capopezzo e un operatore sotto coperta, «flusso ininterrotto» | **sì**, durante il tiro | `[NW-Mk45]` | alta (Tico), media (Spruance, Kidd) |
| **3"/50 Mk 33** binato (Oslo) | 200-300 per canna nei depositi pronti | **1200 per canna** in deposito | 45-50/min per canna | caricatore automatico alimentato a mano dai due lati; 11 serventi (4 serventi ai proietti, 4 porgitori) | **sì**, a mano durante il tiro | `[NW-Mk33]` | alta (dati USA), media (Oslo) |
| **AK-100 100/70** (Krivak II, Udaloy, Kirov) | n.v.; **STIMA 174** (il dato «munizioni immagazzinate» di `[AC-AK100]`, forse nell'affusto) | **350 per pezzo** (fregata di 4000 t, Neustrashimy) | 50-60/min | automatica dal deposito con l'elevatore; 5-6 serventi | **sì** (automatica); canna raffreddata ad acqua di mare continua, nessun limite di raffica pubblicato | `[NW-AK100]` `[WP-AK100]` `[RU-AK100]` `[AC-AK100]` | media (cadenza, deposito), bassa (pronti) |
| **AK-726 76/59** binato (Kara, Koni) | n.v.: i montacarichi salgono dai depositi sotto la torre | n.v.; **STIMA 1000 per affusto** | 90-107/min per canna `[RU-726]`; 45/min nella scheda `[NW-726]` | montacarichi caricati a mano da 4 dei 9 serventi, nel locale sotto la torre | sì; ma **raffiche di 40-45 colpi, poi 3 minuti** di raffreddamento ad acqua di mare (2-3 min per `[NW-726]`); vita della canna 3000 colpi | `[RU-726]` `[NW-726]` | media |
| **AK-176 76/59** (non nel gioco) | **152** sull'affusto: due trasportatori da 76 | n.v. | 30, 60 o 120/min | continua dai due lati con trasportatori ed elevatori a catena; 2 serventi, 4 a mano | sì; **raffica massima 70 colpi, poi 25-30 minuti** di raffreddamento | `[RU-176]` | media |
| **AK-725 57/75** binato (Nanuchka I, Grisha III) | n.v. (nastro) | **1000 per affusto** | 200/min per canna, 400 l'affusto | a nastro; 1 servente | sì; raffica massima 100 colpi, canne ad acqua di mare | `[RU-725]` | media |
| **AK-630 30 mm** (Kara, Kirov, Slava, Udaloy, Grisha III) | **2000** nel nastro (AK-630); l'AK-630M ha in più un serbatoio di riserva da **1000** | quello che c'è sull'affusto; **STIMA 2000** di scorta a bordo per rifare il nastro | 4000-5000/min | a mano, a pezzo fermo; tempo n.v.; **STIMA 15 min** | **no** (STIMA): 6 raffiche da 400 con 5 s di pausa, o 6 da 200 con 1 s, poi il nastro è finito | `[RU-630]` `[NW-630]` | media (nastro, raffiche), STIMA (ricarica) |
| **AK-230 30 mm** binato (Koni) | **500 per canna** | **STIMA 500 per canna** di scorta | ~1000/min per canna | tamburo a 4 camere; a mano, tempo n.v. | **no** (STIMA) | `[RU-230]` | media-bassa |
| **Bofors 57/70 Mk 2** (Stockholm) | **120** pronti, più fino a 40 nei due montacarichi | n.v.; **STIMA 1000** | 220/min ciclica | dal deposito con due montacarichi: **120 colpi in 2 minuti** | **sì** | `[NW-57]` `[NP-Stockholm]` | alta (affusto), STIMA (deposito) |
| Bofors 57/70 Mk 1 (Spica; non nel gioco) | 40 pronti, più 128 nelle rastrelliere della torre | n.v. | 200/min ciclica | a mano dalle rastrelliere | sì (STIMA) | `[NW-57]` | media |
| **Bofors 40/70** SAK 315 e 350 (Hauk; Stockholm; Oslo dal 1987) | **18** nel caricatore automatico, **96** sull'affusto | n.v.; **STIMA 800** | 240/min | piastrine da 4 caricate a mano nel caricatore | **sì**, a mano: il fuoco sostenuto è limitato dal ritmo dei serventi (come i Mk 10-12 britannici) | `[NW-40]` `[NP-Oslo]` `[NP-Stockholm]` `[WP-Hauk]` | alta (affusto), STIMA (deposito) |

Note:
- **Mk 75, 80 o 76 colpi pronti.** NavWeaps dà 80 (70 + 6 + 4). Un'altra descrizione dà due anelli concentrici da 35
  più i 6 della vite, cioè 76, più i 4 del tamburo di culatta: torna 80 `[GC-Mk75]`. Con 80 colpi a 80-85 al minuto, il
  pezzo vuota l'affusto in poco meno di un minuto [CALC].
- **Mk 45:** quello che limita il fuoco non è il tamburo da 20 ma il deposito. Il gioco oggi ha `ricarica_s = 30` dopo
  20 colpi (STIMA del dossier 22): con il rifornimento continuo andrebbe **0**.
- **AK-726, la cadenza.** ru.wikipedia dà 90-107 colpi al minuto per canna; NavWeaps 45 al minuto, senza dire se per
  canna o per affusto. Il dossier 22 ha preso 45 per affusto. Con la raffica di 40-45 colpi e 3 minuti di
  raffreddamento, la cadenza sostenuta è comunque bassa: circa **20-30 colpi al minuto per affusto** [CALC].
- **AK-176, i 25-30 minuti** di raffreddamento dopo 70 colpi vengono da ru.wikipedia, su una revisione non verificata.
  Sono molto più lunghi di quelli dell'AK-726; li segnalo con confidenza bassa.
- **AK-630, «боекомплект на ствол 2000».** È il nastro dell'affusto. Il serbatoio di riserva da 1000 c'è solo
  nell'AK-630M; le classi del gioco nel 1989 hanno in prevalenza la versione base (STIMA).
- **3"/50 Mk 33, i dati sono quelli della US Navy.** La dotazione norvegese non si trova. Con 11 serventi in
  combattimento il pezzo spara finché ci sono colpi nei depositi pronti; poi il rifornimento dal deposito è lento
  (STIMA).

---

## 3. Tabella per classe (dotazione 1989)

| Classe | Pezzi | Colpi pronti | Deposito | Note | Conf. |
|---|---|---|---|---|---|
| **Perry** | 1 × Mk 75 | 80 | STIMA 600 | il caricatore si rifà a mano in circa 3 minuti (STIMA) | alta / STIMA |
| **Knox** | 1 × Mk 42 Mod 9 | 40 | 600 | | alta |
| **Spruance, Kidd** | 2 × Mk 45 | 20 per pezzo | 475-500 per pezzo | rifornimento continuo | media |
| **Ticonderoga** | 2 × Mk 45 Mod 0 | 20 per pezzo | 600 per pezzo | | alta |
| **Oslo** (Bergen, Oslo, Stavanger fino al 6/1989) | 2 × 2 3"/50 Mk 33 | 200-300 per canna | 1200 per canna | | media |
| **Oslo** (Trondheim dal 11/1987, Narvik dal 10/1988, Stavanger dal 6/1989) | 1 × 2 3"/50 Mk 33, 1 × Bofors 40/70 SAK 350 | come sopra; 96 il Bofors | come sopra; STIMA 800 il Bofors | | media |
| **Hauk** | 1 × Bofors 40/70 (più un 20 mm Rheinmetall, fonti deboli) | 96 | STIMA 800 | | media |
| **Stockholm** | 1 × Bofors 57 Mk 2, 1 × Bofors 40/70 SAK 315 | 120; 96 | STIMA 1000; STIMA 800 | la Malmö ha forse il Mk 1 (fonti in disaccordo, §5) | media |
| **Krivak II** | 2 × AK-100 | STIMA 174 per pezzo | 350 per pezzo | caricamento automatico | media-bassa |
| **Udaloy** | 2 × AK-100, 4 × AK-630 | come sopra; 2000 | 350; STIMA 2000 | | media-bassa |
| **Kirov** (*Kirov*) | 2 × AK-100, 8 × AK-630 | come sopra | come sopra | *Frunze* e *Kalinin* hanno l'AK-130 (dossier 22 §3.1) | media-bassa |
| **Kara** | 2 × 2 AK-726, 4 × AK-630 | n.v.; 2000 | STIMA 1000 per affusto; STIMA 2000 | raffiche AK-726 di 40-45 colpi | media |
| **Koni** | 2 × 2 AK-726, 2 × 2 AK-230 | n.v.; 500 per canna | STIMA 1000 per affusto; STIMA 500 per canna | | media |
| **Nanuchka I** | 1 × 2 AK-725 | n.v. | 1000 | | media |
| **Grisha III** | 1 × 2 AK-725, 1 × AK-630 | n.v.; 2000 | 1000; STIMA 2000 | | media |
| **Slava** | 1 × 2 AK-130, 6 × AK-630 | 150-180 l'AK-130 (dossier 22) | n.v. | fuori dalla richiesta, per completezza | media |
| Nimitz, Kitty Hawk, Sacramento, mercantili | nessun cannone (Phalanx o niente) | — | — | | — |

---

## 4. Proposte per il gioco [GIOCO]

Campi nuovi in `artiglieria.toml` per pezzo (STIMA dove non c'è fonte):

| Pezzo | `colpi_pronti` | `deposito` | `ricarica_s` | `ricarica_in_combattimento` | `raffica_max` / `raffreddamento_s` |
|---|---|---|---|---|---|
| mk75 | 80 | 600 | 180 | sì, a pezzo fermo | — |
| mk42 | 40 | 600 | 0 (continua) | sì | — |
| mk45 | 20 | 500 (Spruance, Kidd), 600 (Ticonderoga) | **0** (continua; oggi 30) | sì | — |
| mk33 (per canna) | 250 | 1200 | 0 finché ci sono colpi pronti, poi lento | sì | — |
| ak100 | 174 | 350 | 0 (automatica) | sì | — |
| ak726 (per affusto) | 90 (40-45 per canna) | 1000 | 0 | sì | 45 per canna / 180 |
| ak176 | 152 | — | 0 | sì | 70 / 1500 |
| ak725 (per affusto) | 200 (100 per canna) | 1000 | 0 | sì | 100 / STIMA 120 |
| ak630 | 2000 (3000 per l'M) | 2000 | 900 | **no** | 400 / 5 (6 raffiche) |
| ak230 (per canna) | 500 | 500 | 900 | no | — |
| bofors57_mk2 | 120 | 1000 | 120 | sì | — |
| bofors40 | 96 (18 nel caricatore) | 800 | 0 (piastrine a mano) | sì | — |

Il principio: un pezzo occidentale finisce il caricatore e poi spara al ritmo del rifornimento; un pezzo sovietico ha il
caricamento automatico ma deve fermarsi per il calore; un CIWS finisce il nastro e tace. In una partita di 2 ore solo
l'AK-630 rischia davvero di restare senza colpi.

---

## 5. Quello che non si è trovato

- I depositi per nave della Perry e delle classi sovietiche, a parte i 350 colpi per pezzo dell'AK-100.
- I colpi pronti dell'AK-100 e dell'AK-726 (montacarichi dal deposito: è il deposito a contare).
- Il tempo di ricarica del tamburo del Mk 75 e del nastro dell'AK-630.
- Il Mk 1 o il Mk 2 sulla Malmö: Navypedia dà il Mk 2 per tutta la classe `[NP-Stockholm]`; Military Wiki dice che la
  Malmö (1984) aveva il Mk 1 `[MW-57]`.
- Il 20 mm Rheinmetall della Hauk: solo fonti deboli.
- La dotazione norvegese del 3"/50: si usano i dati USA.

---

## 6. Fonti

| Sigla | Fonte | URL | Conf. |
|---|---|---|---|
| `[NW-Mk75]` | NavWeaps, 76 mm/62 Mark 75 (OTO Melara Compact e SR) | https://www.navweaps.com/Weapons/WNUS_3-62_mk75.php | alta |
| `[GC-Mk75]` | guns.com, «Goodbye Mk 75» (2024): 80 colpi compresi i 4 del tamburo di culatta; gli anelli da 35 | https://www.guns.com/news/2024/10/25/goodbye-mk-75-cold-war-classic-76mm-gun-system-retired-in-us-use | media |
| `[NW-Mk42]` | NavWeaps, 5"/54 Mark 42 | https://www.navweaps.com/Weapons/WNUS_5-54_mk42.php | alta |
| `[NW-Mk45]` | NavWeaps, 5"/54 Mark 45 | https://www.navweaps.com/Weapons/WNUS_5-54_mk45.php | alta |
| `[NW-Mk33]` | NavWeaps, 3"/50 Marks 27, 33 e 34 | https://www.navweaps.com/Weapons/WNUS_3-50_mk27-33-34.php | alta |
| `[NW-AK100]` | NavWeaps, 100 mm/70 AK-100 | https://www.navweaps.com/Weapons/WNRussian_39-70_ak100.php | media |
| `[WP-AK100]` | Wikipedia, AK-100 (naval gun) | https://en.wikipedia.org/wiki/AK-100_(naval_gun) | media |
| `[RU-AK100]` | ru.wikipedia, АК-100 | https://ru.wikipedia.org/wiki/АК-100 | media |
| `[AC-AK100]` | armedconflicts.com, SOV - AK-100 (ZIF-91) | https://www.armedconflicts.com/topic/view/46377 | bassa |
| `[NW-726]` | NavWeaps, 76,2 mm/59 AK-726 | https://navweaps.com/Weapons/WNRussian_3-60_ak726.php | media |
| `[RU-726]` | ru.wikipedia, АК-726 | https://ru.wikipedia.org/wiki/АК-726 | media |
| `[RU-176]` | ru.wikipedia, АК-176 (revisione non verificata) | https://ru.wikipedia.org/wiki/АК-176 | media-bassa |
| `[RU-725]` | ru.wikipedia, АК-725 | https://ru.wikipedia.org/wiki/АК-725 | media |
| `[NW-630]` | NavWeaps, 30 mm AK-630 | https://www.navweaps.com/Weapons/WNRussian_30mm-54_ak-630.php | media |
| `[RU-630]` | ru.wikipedia, АК-630 | https://ru.wikipedia.org/wiki/АК-630 | media |
| `[RU-230]` | ru.wikipedia, АК-230 | https://ru.wikipedia.org/wiki/АК-230 | media-bassa |
| `[NW-57]` | NavWeaps, Bofors 57 mm/70 Marks 1, 2, 3 | https://www.navweaps.com/Weapons/WNSweden_57-70_mk123.php | alta |
| `[MW-57]` | Military Wiki, Bofors 57 mm Naval Automatic Gun L/70 (Mk 2 sulla Stockholm dal 1985, Mk 1 sulla Malmö) | https://military-history.fandom.com/wiki/Bofors_57_mm_Naval_Automatic_Gun_L/70 | bassa |
| `[NW-40]` | NavWeaps, Bofors 40 mm/70 Model 1948 e affusti navali | https://www.navweaps.com/Weapons/WNSweden_4cm-70_m1948.php | alta |
| `[NP-Oslo]` | Navypedia, classe Oslo (dotazioni e lavori per nave) | https://navypedia.org/ships/norway/no_es_oslo.htm | media |
| `[NP-Stockholm]` | Navypedia, classe Stockholm | https://navypedia.org/ships/sweden/sw_cf_stockholm.htm | media |
| `[WP-Hauk]` | Wikipedia, Hauk-class patrol boat | https://en.wikipedia.org/wiki/Hauk-class_patrol_boat | media |
