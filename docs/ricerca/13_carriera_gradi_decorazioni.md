# 13 — Carriera, gradi e decorazioni (dossier per il remake di *688 Attack Sub*)

Progetto: remake realistico di *688 Attack Sub* (EA 1989). Ambientazione 1989 per entrambe le parti: comandante di un
SSN classe 688 della US Navy oppure di un SSN classe Alfa (progetto 705 «Lira») della VMF sovietica.
Scopo: base di ricerca per la **carriera** (requisito 15 di `docs/00_ROADMAP.md`), le **promozioni**, le
**decorazioni**, il **fascicolo personale** e la **cerimonia di decorazione** in stile *F-19* / *F-117A* (Fase 5),
collegata al debriefing (requisito 16, Fase 4). Solo ricerca: nessun codice. Compilato il 24 ottobre 2026.

---

## 0. Convenzioni, rimandi e punto di partenza

**Confidenza** (come `02` §0): **[alta]** fonte primaria o autorevole verificata; **[media]** fonte aperta credibile
ma secondaria (enciclopedie, siti di falleristica), oppure dato noto e coerente fra più fonti ma non letto sul
regolamento; **[bassa]** ricordo non riverificato, forum, giochi, interpolazione. **STIMA** = valore proposto per il
gioco; **PROPOSTA** = scelta di design. «estratto» = pagina letta solo attraverso il riassunto di una ricerca.
Le sigle tra parentesi quadre rimandano alla §7. Le voci [bassa] sono **parametri di taratura**, non dati.

**Epoca.** Tutto è riferito al **1989**. I nomi delle decorazioni sono quelli **dell'epoca**: per esempio la «Navy
Commendation Medal» e la «Navy Achievement Medal» prendono il nome «Navy *and Marine Corps*…» solo nel 1994
[alta] ([WP‑NCM], [WP‑NAM]); il grado O‑7 della US Navy si chiama «Rear Admiral (lower half)» dal 1985, prima
«Commodore» [alta] ([WP‑USNRANK]). Le decorazioni nate dopo il 1989 (per esempio la Navy Arctic Service *Medal* del
2024, il dispositivo «C» del 2016) non si usano.

**Non ripetuto qui:** ROE, serie radio e funzione `debriefing()` (`10` §§ su R1–R2); codici d'esito ≥ 100 delle
missioni originali (`docs/04_vm_missioni.md` §5); blocco `roe` e struttura del file di missione
(`docs/09_schema_missioni.md` §2, §5.1); stati di danno e perdita del battello (`docs/10_danni_affondamento_esplosioni.md`
§2.7).

### 0.1 Cosa dice già il gioco originale

Le pagine d'esito delle 10 missioni del 1989 (`docs/originale/briefing_EN.txt`, [ORIG‑BR]) parlano già di carriera,
con tono a volte serio e a volte comico. Sono il punto di partenza naturale: il remake deve renderle **coerenti**
con un sistema di gradi e decorazioni, non buttarle.

| Esito nell'originale | Cosa promette | Commento storico |
|---|---|---|
| Missioni 7 e 10, lato USA, esiti migliori | **Navy Cross** al comandante, **Navy Unit Commendation** all'equipaggio; nella 10 anche la **Medal of Honor** | Plausibile solo in **guerra aperta** (la Navy Cross e la MOH richiedono il combattimento) [alta, §3.1] |
| Missione 7, esito ottimo | l'ammiraglio vorrebbe promuoverlo **Commodore** al comando di una squadriglia | Coerente: un capitano di vascello (O‑6) comanda squadriglie, non battelli (§1.4) |
| Varie, esito buono | «commendation», rapporto nel **permanent record** | Corrisponde a una Navy Commendation Medal o a una lettera d'encomio [media] |
| Missione 4, esito mediocre | trasferimento a terra in un posto ridicolo | Equivale a una rimozione «morbida» dal comando (§2.1) |
| Missione 8, esito con vittime civili | il comandante è **sollevato dall'incarico** in attesa di processo | Coerente col caso reale del *George Washington* (1981, §2.1) [alta] |
| Lato sovietico, esiti ottimi | «decorazione di Lenin», titolo di **Eroe dell'URSS** | Ordine di Lenin e Stella d'oro esistono; l'Eroe riceve sempre anche l'Ordine di Lenin (§3.4) [alta] |
| Lato sovietico, disastri | il **zampolit** prende il comando; gulag, «armadietto del ghiaccio» | Caricatura: nella realtà il comando passa al **primo ufficiale (starpom)**, il zampolit non è nella linea di comando (§1.8) [media] |

PROPOSTA: nel livello **classico** questi testi restano identici; nel livello **simulazione** la frase d'esito
diventa l'**intestazione** del debriefing e la decorazione promessa si assegna davvero solo se il sistema del §3.7
la conferma (altrimenti il testo si adegua: «raccomandato per…»).

---

## 1. Gradi e percorso di carriera (1989)

### 1.1 US Navy: gradi degli ufficiali

| Paga | Grado (abbr.) | Italiano (equivalente MMI) | Gallone sulla manica dell'uniforme blu | Distintivo sul colletto (kaki) | NATO |
|---|---|---|---|---|---|
| O‑1 | Ensign (ENS) | guardiamarina | 1 gallone medio (½″) | barretta d'oro | OF‑1 |
| O‑2 | Lieutenant junior grade (LTJG) | sottotenente di vascello | 1 medio + 1 sottile (¼″) sopra | barretta d'argento | OF‑1 |
| O‑3 | Lieutenant (LT) | tenente di vascello | 2 medi | due barrette d'argento | OF‑2 |
| O‑4 | Lieutenant Commander (LCDR) | capitano di corvetta | medio, sottile, medio | foglia di quercia d'oro | OF‑3 |
| O‑5 | Commander (CDR) | capitano di fregata | 3 medi | foglia di quercia d'argento | OF‑4 |
| O‑6 | Captain (CAPT) | capitano di vascello | 4 medi | aquila d'argento | OF‑5 |
| O‑7 | Rear Admiral (lower half) (RDML), «Commodore» fino al 1985 | contrammiraglio | 1 gallone largo (2″) | 1 stella d'argento | OF‑6 |
| O‑8 | Rear Admiral (RADM) | contrammiraglio / ammiraglio di divisione | largo + 1 medio | 2 stelle | OF‑7 |
| O‑9 | Vice Admiral (VADM) | ammiraglio di squadra | largo + 2 medi | 3 stelle | OF‑8 |
| O‑10 | Admiral (ADM) | ammiraglio | largo + 3 medi | 4 stelle | OF‑9 |

Nomi e paghe [alta] ([WP‑USNRANK]); stella dell'ufficiale di linea sopra i galloni e sulle controspalline [alta]
([WP‑USNRANK]); larghezze e combinazioni dei galloni e distintivi del colletto [media] (dal regolamento delle uniformi,
noti e coerenti nelle fonti divulgative, non letti sul testo del 1989). Fleet Admiral esiste solo in guerra [alta].
Il **titolo** di «Captain» spetta a bordo a chiunque comandi la nave, qualunque sia il grado: il comandante CDR di un
688 è «the Captain» per l'equipaggio [media]. Lo usano anche i testi dell'originale.

### 1.2 US Navy: il percorso dell'ufficiale sommergibilista

| Tappa | Dove / come (anni '80) | Durata tipica | Anni dalla nomina (YCS) | Fonte e confidenza |
|---|---|---|---|---|
| Nomina | Accademia navale (USNA), NROTC, OCS, programma NUPOC; laurea con analisi e fisica | — | 0 | [WP‑NPS] [alta] sui requisiti |
| Colloqui a Naval Reactors | colloqui tecnici con lo staff e, fino al 1982, con l'ammiraglio Rickover in persona | qualche giorno | prima della nomina | [USNI92‑Rick], [USSR‑rcl] [media] (estratto) |
| Nuclear Power School | Orlando (Florida) dal 1976 al 1998 | ~6 mesi | 0–0,5 | [WP‑NPS] [alta] sul luogo; durata [media] |
| Prototipo | reattori a terra: Idaho (S1W, A1W, S5G), Windsor CT (S1C), West Milton NY (Kesselring) | ~6 mesi | 0,5–1 | [WP‑NPS] [alta] sui siti; durata [media] |
| Submarine Officer Basic Course | Naval Submarine School, Groton (CT) | ~3 mesi | ~1–1,5 | [media] |
| **Ufficiale di divisione** (division officer) | a bordo: 2–3 incarichi di divisione; guardie di macchina (EOOW), poi di plancia (OOD); **qualifica in sommergibili** e dolphins d'oro | ~3 anni | 1,5–4,5 | [NSL91] [alta sul periodo], [NPC‑TACAID] [media] |
| Esame da **ingegnere** | esame scritto e orale a Naval Reactors (Washington): abilita al ruolo di direttore di macchina | — | ~3–4 | [USSR‑rcl] [media] (singola testimonianza) |
| Primo incarico a terra | staff, istruttore, master | ~2 anni | 4,5–6,5 | [NSL91] [media] |
| Submarine Officer Advanced Course | Groton | ~6 mesi | ~6,5–7 | [NSL91] [alta] |
| **Capo servizio** (department head) | direttore di macchina (Engineer), navigatore/operazioni, armi | ~3 anni | 7–10 | [NSL91] [alta] |
| Qualifica al comando | esame davanti a una commissione; abilita a fare il secondo | — | ~9–10 | [NPC‑TACAID] [media] |
| **Ufficiale in seconda** (XO) | spesso subito dopo, o dopo un turno a terra | ~2 anni | 11–14 | [NSL91], [NPC‑TACAID] [media] |
| Selezione per il comando e corso PCO | commissioni di selezione; corso del futuro comandante: parte tecnica a Naval Reactors (circa sei mesi in una testimonianza del 1980) e parte tattica | ~6 mesi | 14–16 | [USSR‑rcl] [media], [NPC‑TACAID] |
| **Comandante** (CO) di un SSN | grado Commander (O‑5) | ~3 anni | 15–19 | [NSL91]: l'autore prende il comando del *Philadelphia* nel maggio 1989 [alta] |
| Dopo il comando | staff, comando di squadriglia (CAPT, «Commodore»), comandi maggiori, ammiragliato | — | 19+ | [media] |

**Selezione a imbuto.** Un ufficiale scrive su *Proceedings* che, storicamente, solo circa il 60 % dei capi servizio
diventa XO e circa il 60 % degli XO arriva al comando [media] ([USNI24], estratto): su 10 capi servizio, circa 3–4
comandano un battello. Il sistema di gioco deve rendere il comando **una conquista**, non un automatismo.

### 1.3 «Qualified in Submarines» e le dolphins

- Il distintivo di sommergibilista (le «dolphins») nasce nel 1924: la prua di un sommergibile in superficie con i
  timoni di prua aperti, fra due delfini araldici; **d'oro per gli ufficiali**, d'argento per gli equipaggi [alta]
  ([WP‑SWI]).
- Si porta sul petto sinistro **sopra i nastrini**; sulle uniformi di lavoro in versione ricamata [alta] ([WP‑SWI]).
- Percorso dell'ufficiale: prima le guardie di macchina (EOOW, EDO), poi quelle tattiche (Contact Manager, DOOW, OOD,
  SDO); commissione presieduta dal comandante, osservazione di un ufficiale di grado da comando, raccomandazione al
  comandante di squadriglia che conferisce la qualifica [alta] ([WP‑SWI], procedura attuale; negli anni '80 la
  sostanza era la stessa [media]).
- Tempo a bordo per qualificarsi: circa un anno per gli equipaggi [alta] ([WP‑SWI]); 12–18 mesi per gli ufficiali
  [media].

### 1.4 Chi comanda un 688

| Incarico a bordo di un 688 (1989) | Grado tipico | Confidenza |
|---|---|---|
| Comandante (CO) | **Commander (O‑5)** | [alta] ([NSL91]; prassi costante) |
| Ufficiale in seconda (XO) | Lieutenant Commander (O‑4) | [media] |
| Capi servizio (Engineer, Navigator/Ops, Weapons) | Lieutenant o LCDR | [media] |
| Ufficiali di divisione | Ensign, LTJG, LT | [media] |
| Capo dell'equipaggio (COB) | Master Chief (E‑9) | [media] |

L'equipaggio conta circa 12–14 ufficiali e 110–120 fra sottufficiali e marinai [media]. Il **Captain (O‑6)**
comanda una **squadriglia** di sottomarini (il «Commodore» dei testi dell'originale), un tender o una base [media].
Un 688 comandato da un O‑6 è un'eccezione [media].

### 1.5 VMF: gradi degli ufficiali di marina (корабельные звания)

| Russo | Traslitterazione | Italiano | Spalline (controspalline) | NATO |
|---|---|---|---|---|
| младший лейтенант | mladšij lejtenant | sottotenente (raro in marina) | 1 striscia, 1 stella piccola | OF‑1 |
| лейтенант | lejtenant | tenente | 1 striscia, 2 stelle piccole | OF‑1 |
| старший лейтенант | staršij lejtenant | tenente anziano | 1 striscia, 3 stelle piccole | OF‑1 |
| капитан-лейтенант | kapitan‑lejtenant | capitano‑tenente | 1 striscia, 4 stelle piccole | OF‑2 |
| капитан 3-го ранга | kapitan 3‑go ranga | capitano di 3° rango | 2 strisce, 1 stella grande | OF‑3 |
| капитан 2-го ранга | kapitan 2‑go ranga | capitano di 2° rango | 2 strisce, 2 stelle grandi | OF‑4 |
| капитан 1-го ранга | kapitan 1‑go ranga | capitano di 1° rango | 2 strisce, 3 stelle grandi | OF‑5 |
| контр-адмирал | kontr‑admiral | contrammiraglio | campo a zig‑zag, 1 stella ricamata | OF‑6 |
| вице-адмирал | vice‑admiral | viceammiraglio | 2 stelle ricamate | OF‑7 |
| адмирал | admiral | ammiraglio | 3 stelle ricamate | OF‑8 |
| адмирал флота | admiral flota | ammiraglio della flotta (dal 1962) | 1 stella grande | OF‑9 |
| Адмирал Флота Советского Союза | Admiral Flota Sovetskogo Sojuza | ammiraglio della flotta dell'Unione Sovietica | stella grande con stemma | (OF‑10) |

Elenco, ordine e storia dei gradi superiori [alta] ([WP‑SovRanks], [WP‑MilRanks]). Struttura delle controspalline
(una «striscia», просвет, per gli ufficiali inferiori e due per i superiori; stelle piccole e grandi) [media]:
coerente con la regola attuale russa trovata nelle ricerche e con le immagini, ma non letta su un testo sovietico.
**Il colore delle stelle** e del campo sulle diverse uniformi va verificato su foto d'epoca [bassa] (§4.3).
I gradi di marina valgono per gli ufficiali imbarcati; i reparti a terra e i servizi usano i gradi dell'esercito
(maggiore, tenente colonnello…) [media].

### 1.6 VMF: il percorso dell'ufficiale sommergibilista

| Tappa | Contenuto | Durata / età tipica | Fonte e confidenza |
|---|---|---|---|
| Scuola navale superiore (VVMU) | 5 anni; per esempio ВВМУ «Frunze» (Leningrado), ВВМУ di navigazione subacquea «Komsomol leninista» (Leningrado), Tichookeanskoe di Vladivostok; ingegneri: ВВМИУ «Dzeržinskij», Sebastopoli | 17–22 anni | [FBIS‑Suchkov], [WP‑Bessonov] [alta] sui nomi; durata [media] |
| Nomina a **lejtenant** e assegnazione | comandante di **gruppo** (команди́р гру́ппы) in un servizio (БЧ): per esempio il gruppo siluri della БЧ‑3 | 22 anni | [FBIS‑Suchkov] [alta] |
| Comandante di **servizio** (командир БЧ) | БЧ‑1 navigazione, БЧ‑2 missili, БЧ‑3 mine e siluri, БЧ‑4 comunicazioni, БЧ‑5 elettromeccanico (macchina), servizio radiotecnico | dopo 1–2 anni | [FBIS‑Suchkov] [alta]; elenco dei servizi [CIA‑BU83] [media] (estratto) |
| **Pomoščnik** (assistente del comandante) | terzo nella linea di comando | ~25 anni | [FBIS‑Suchkov] [alta] |
| **Staršij pomoščnik** («starpom», primo ufficiale) | il secondo; prende il comando se il comandante manca | ~25–30 anni | [FBIS‑Suchkov], [WP‑Bessonov], [WP‑Zhiltsov] [alta] |
| Corsi superiori speciali (ВСОК ВМФ) | corso per futuri comandanti, Leningrado | ~1 anno | [FBIS‑Suchkov]: diploma nel 1978, poi il comando [alta] |
| Ammissione al comando autonomo | esami (зачёты) davanti allo staff di brigata/divisione: «допуск к самостоятельному управлению» | — | [media] (prassi nota, nessun testo trovato) |
| **Comandante** del sottomarino | kapitan 2‑go o 1‑go ranga | **~31 anni**, 9 anni dopo la scuola | [FBIS‑Suchkov] [alta] |
| Accademia navale, poi stati maggiori | capo di stato maggiore di brigata, di squadra, comandante di squadra, flotta | 38+ anni | [FBIS‑Suchkov], [WP‑Zhiltsov] [alta] |

**Esempio reale completo** ([FBIS‑Suchkov], [alta]): G. A. Sučkov, diplomato «Frunze» nel 1969; comandante del gruppo
siluri (1969), della БЧ‑3 (1970), assistente e poi starpom (1972), ВСОК e **comandante** (1978, a 31 anni),
Accademia (1985), capo di stato maggiore di brigata e di squadra, poi viceammiraglio e comandante della flotta del
Pacifico. Decorazioni: Ordine della Bandiera Rossa, Ordine «Per il servizio alla Patria» di 3ª classe, medaglie.
Altri esempi: L. M. Žil'cov, primo ufficiale del primo SSN sovietico (1954), comandante del K‑3 da kapitan
2‑go ranga (1959) [alta] ([WP‑Zhiltsov]); V. B. Bessonov, starpom del K‑8 (1968), comandante da kapitan 2‑go ranga
(1970) [alta] ([WP‑Bessonov]).

**Il contrasto con la US Navy** è il dato più utile per il gioco: il sovietico arriva al comando in **8–10 anni**
dalla nomina, l'americano in **15–17** [media, confronto fra le due tabelle]. Il sovietico passa tutta la carriera
nella «linea di comando» di bordo; l'americano alterna mare e terra e passa per l'ingegneria nucleare.

### 1.7 L'Alfa: un equipaggio di ufficiali

- Equipaggio dell'Alfa: circa **31 persone**, quasi tutti ufficiali (27 ufficiali e 4 michmany nel testo di
  [WP‑Alfa]; cifre leggermente diverse in altre fonti; 8 ufficiali nella guardia di combattimento in centrale)
  [media] ([WP‑Alfa], [GS‑705] estratto).
- Comandanti: **kapitan 1‑go ranga** o **2‑go ranga**; per esempio il K‑123 (705K) è dato con un comandante di
  1° rango [media] ([GS‑705], estratto).
- STIMA per il gioco: comandante kapitan 2‑go ranga (alla nomina) che diventa 1‑go ranga dopo un turno di comando
  riuscito; starpom kapitan 2‑go o 3‑go ranga; comandanti di servizio kapitan 3‑go ranga o kapitan‑lejtenant;
  ufficiali di guardia staršij lejtenant e lejtenant [bassa].
- Nella VMF ogni incarico ha un **grado massimo previsto** (штатная категория): il comando di un sottomarino
  nucleare, nave di 1° rango, «vale» kapitan 1‑go ranga [media]. Il grado segue l'incarico più che l'anzianità
  (§2.2).

### 1.8 Il zampolit

- Dal 1942 il commissario diventa **заместитель командира по политической части** (zampolit), **subordinato** al
  comandante; gli ufficiali politici restano in tutte le forze armate sovietiche fino al 1991 [alta] ([WP‑PC]).
- A bordo: kapitan 3‑go o 2‑go ranga, formato alla Scuola navale politica superiore di Kiev; risponde anche alla
  catena politica (dipartimento politico della formazione, fino alla Direzione politica principale) [media].
- Scrive la **caratteristica politica** usata nelle attestazioni e firma le proposte di ricompensa [media].
- Non è nella linea di comando: in caso di rimozione del comandante subentra lo **starpom** [media]. PROPOSTA: nel
  livello simulazione il zampolit compare come **personaggio** del fascicolo (il suo giudizio pesa
  sull'attestazione, §2.3) e nella cerimonia (legge il decreto davanti all'equipaggio); nel classico resta la
  caricatura dell'originale.

### 1.9 Equivalenze fra le due marine

| NATO | US Navy | VMF | Ruolo tipico a bordo (1989) |
|---|---|---|---|
| OF‑1 | Ensign / LTJG | mladšij lejtenant / lejtenant / staršij lejtenant | ufficiale di divisione / comandante di gruppo |
| OF‑2 | Lieutenant | kapitan‑lejtenant | ufficiale di divisione anziano / comandante di servizio |
| OF‑3 | Lieutenant Commander | kapitan 3‑go ranga | XO (USA) / comandante di servizio o starpom (URSS) |
| OF‑4 | **Commander** | **kapitan 2‑go ranga** | **comandante di un SSN** (entrambi) |
| OF‑5 | Captain | kapitan 1‑go ranga | squadriglia (USA) / comandante anziano di SSN o di brigata (URSS) |
| OF‑6 | Rear Admiral (lower half) | kontr‑admiral | gruppo, flottiglia |

Codici NATO [media] (STANAG 2116, citato da [WP‑SovRanks]; tabella dell'autore). La differenza da ricordare: alla
pari del grado, il comandante sovietico è **più giovane** di 5–8 anni.

---

## 2. Criteri di promozione

### 2.1 US Navy

- **Legge DOPMA** (1980, in vigore dal 1981): obiettivi di promozione per anni di servizio e percentuale di scelta:
  O‑2 a 1,5–2 anni (tutti i pienamente qualificati); O‑3 a 3,5–4 (95 %); O‑4 a 9–11 (80 %); O‑5 a 15–17 (70 %);
  O‑6 a 21–23 (50 %); permanenza minima nel grado precedente di 18 mesi per O‑2, 2 anni per O‑3, 3 anni per O‑4,
  O‑5 e O‑6 [media] ([WP‑DOPMA]: la tabella è quella attuale; la legge è del 1980, le percentuali sono obiettivi e
  non obblighi).
- **Commissioni di selezione** (selection boards) annuali per classe di nomina: **in zona** (la maggior parte),
  **sotto zona** (pochi eccellenti, 1–2 anni prima), **sopra zona** (seconda occasione) [alta] ([WP‑DOPMA]).
- **«Up or out»**: due mancate promozioni consecutive portano al congedo, o alla pensione se maturata (20 anni) [alta]
  ([WP‑DOPMA]).
- **Note caratteristiche** (fitness reports): dal 1954 sono lo strumento principale per promozioni, selezioni e
  incarichi [media] ([USNI60], estratto). Negli anni '80 il modulo NAVPERS 1611/1 conteneva voti, una **graduatoria**
  fra gli ufficiali pari grado del comandante («1 di N») e una raccomandazione di promozione [bassa]; il sistema
  attuale (BUPERSINST 1610.10) è del 1996 [media].
- **Comando**: oltre alla promozione servono le **commissioni di selezione per il comando** (XO e CO) e le
  qualifiche del §1.2 [media] ([NPC‑TACAID]).
- **Rimozione dal comando** («detachment for cause», «loss of confidence»): esempio reale del 9 aprile 1981, lo SSBN
  *George Washington* a quota periscopica urta e affonda il mercantile giapponese *Nissho Maru* (due morti); la Marina
  solleva dall'incarico e ammonisce **il comandante e l'ufficiale di guardia** e si assume la responsabilità [alta]
  ([WP‑GW]). Per incidenti più gravi: commissione d'inchiesta e corte marziale [media].

### 2.2 VMF

- **Anzianità minima nel grado** (сроки выслуги): per il sistema sovietico degli anni '80 nessun testo è stato
  trovato in rete. Valori attuali russi, ereditati in larga parte: staršij lejtenant 3 anni (2 per chi esce da un
  corso lungo), kapitan(‑lejtenant) 4 anni [media]; per gli altri gradi la serie usuale è mladšij lejtenant 2,
  lejtenant 2–3, kapitan 3‑go ranga 4, kapitan 2‑go ranga 5, nessun termine fisso per kapitan 1‑go ranga [bassa].
  Un rapporto CIA del 1954 conferma che i termini c'erano e che furono allungati in quell'anno [media] (estratto).
- **Il grado segue l'incarico**: si promuove solo se il posto occupato lo consente (штатная категория); per salire
  bisogna essere **nominati** a un incarico superiore [media]. Promozione **anticipata** (досрочно) per meriti
  particolari [media].
- **Attestazione** (аттестация) periodica: il superiore diretto scrive un giudizio, una commissione conclude
  («adeguato all'incarico», «merita la promozione a…», «non adeguato») [media] (struttura attuale dei moduli; per
  il periodo sovietico nessun testo trovato).
- **Partito**: quasi tutti gli ufficiali di carriera erano membri del PCUS; la **caratteristica politica** del
  zampolit e del dipartimento politico entrava nell'attestazione e nelle proposte di ricompensa [media].
- **Chi conferisce i gradi**: fino ai gradi intermedi i comandanti di flotta e il ministero; i gradi d'ammiraglio un
  decreto del Consiglio dei ministri (es. Matuškin, viceammiraglio nel 1976) [media] ([RIAMO‑Matushkin]).
- **Punizioni reali dopo i disastri**: K‑429 (1983, affondato in prova a Kamčatka, 16 morti): il comandante,
  kapitan 1‑go ranga N. Suvorov, condannato a **10 anni** [alta] ([WP‑K429]); K‑219 (1986): il comandante, kapitan
  2‑go ranga I. Britanov, **congedato** e accusato di negligenza e sabotaggio; accuse ritirate nel 1987 [alta]
  ([WP‑Britanov]). Al contrario, morte eroica: Bessonov (K‑8, 1970) **Eroe dell'Unione Sovietica** postumo [alta]
  ([WP‑Bessonov]); l'equipaggio del *Komsomolets* (1989) decorato con l'Ordine della Bandiera Rossa [media]
  ([WP‑K278]).

### 2.3 Proposta per il gioco: punteggio, promozioni, rimozione

#### 2.3.1 Punteggio di missione (STIMA)

Il punteggio nasce dal **registro della partita** (lo stesso del debriefing), quindi è deterministico e ripetibile.

| Voce | Misura | Punti (STIMA) |
|---|---|---|
| **R** Riuscita | obiettivi primari raggiunti (frazione pesata) | 0–400 |
| **C** Completezza | obiettivi secondari; contatti classificati correttamente e con anticipo; rapporti radio inviati quando richiesti | 0–150 |
| **S** Discrezione | assenza di contro‑scoperta: ping subiti, fix DF/ESM, avvistamenti, transitori evitabili, boe sopra di sé | 0–150 |
| **O** Ordini e ROE | ordini eseguiti, ROE ricevute e rispettate, serie radio copiate (`10`, `debriefing()`) | 0–100 |
| **E** Efficienza | colpi a segno su lanci, armi coerenti col bersaglio, carico scelto bene (Fase 4) | 0–100 |
| **D** Danni e perdite | danni subiti, feriti e morti a bordo, armi perse | da 0 a −300 |
| **V** Violazioni | neutrali o amici colpiti, fuoco senza autorizzazione, rotta in acque vietate | da −200 a squalifica |

**Moltiplicatore di rischio** M = M_situazione × M_avversario × M_realismo, sul modello del fattore di *F‑19*
(fino a circa 9 ai livelli più difficili; [FSB‑F19] [alta sul gioco]):

- M_situazione (dichiarato dalla missione): pace 1,0; crisi 1,3; guerra 1,6 (STIMA).
- M_avversario: impreparato 0,8; regolare 1,0; veterano 1,25; élite 1,5 (STIMA).
- M_realismo: ogni aiuto acceso lo riduce (verità sulla carta, TMA automatica, fondale perfetto…), fino a 0,5 (STIMA).
- In più un **indice di pericolo** misurato nella partita (minima distanza da siluri nemici, tempo passato in
  contatto con unità ASW, profondità vicino al collasso): fino a +25 % (STIMA).

Punteggio di missione P = (R + C + S + O + E + D + V) × M, mai sotto zero. Ogni missione produce anche una
**nota caratteristica** (USA) o una **riga d'attestazione** (URSS): un giudizio a parole scelto da P e dalle voci
più deboli («ottima discrezione, uso delle armi da migliorare»).

#### 2.3.2 Carriera compressa: dai gradi bassi al comando (PROPOSTA)

Il giocatore **comanda** sempre il battello, ma il requisito 15 chiede la scala completa da Ensign/lejtenant.
Proposta: i gradi bassi sono le **missioni di addestramento e qualifica** (tutorial) con un pretesto credibile, e il
comando arriva dopo; i gradi alti allargano il compito. Una missione fa avanzare il calendario (mesi), così anni di
servizio e commissioni emergono da soli.

| Fase | USA (grado, pretesto) | URSS (grado, pretesto) | Missioni minime (STIMA) | Condizione d'uscita (STIMA) |
|---|---|---|---|---|
| 1 | ENS: scuola e prototipo (tutorial di sistemi) | lejtenant: comandante di gruppo siluri | 2 | missioni completate |
| 2 | LTJG: ufficiale di guardia sotto supervisione | staršij lejtenant: comandante della БЧ‑3 | 3 | **dolphins** / esami di guardia superati |
| 3 | LT: OOD e ufficiale di guardia in immersione, prove di qualifica | kapitan‑lejtenant: pomoščnik | 4 | media P ≥ 250 |
| 4 | LCDR: XO, comando temporaneo (il CO è a terra o ferito) | kapitan 3‑go ranga: starpom; ВСОК | 4 | esame di comando (missione‑esame) |
| 5 | **CDR: comandante del 688** (le 10 missioni originali, campagne, generate) | **kapitan 2‑go ranga: comandante dell'Alfa** | 10 | commissione O‑6 / attestazione |
| 6 | CAPT: Commodore di squadriglia (missioni con più battelli, oppure resta in mare per scelta) | kapitan 1‑go ranga: comandante anziano o di brigata | — | fine carriera: pensione, ammiragliato onorario |

La strada sovietica è **più corta** (meno missioni nelle fasi 1–4, STIMA: 1, 2, 3, 3), come quella reale (§1.6).
Chi vuole solo comandare sceglie «Carriera breve»: parte da CDR o da kapitan 2‑go ranga con un fascicolo
precompilato coerente con la tabella del §1.2 o del §1.6.

#### 2.3.3 Commissioni e attestazioni simulate (PROPOSTA)

- **USA**: ogni 3–4 missioni (o quando il calendario raggiunge gli anni del §2.1) si riunisce una commissione. Si
  calcola la **media delle note** (P normalizzato) delle ultime 6 missioni: ≥ soglia «sotto zona» → promozione
  anticipata; ≥ soglia «in zona» → promozione; sotto → «non selezionato», seconda occasione alla commissione
  successiva; **due mancate** → pensione con la cerimonia di congedo (fine onorevole della carriera). Soglie STIMA
  per O‑4/O‑5/O‑6: in zona 300/350/400, sotto zona 450/500/550.
- **URSS**: l'attestazione arriva quando scade il termine del grado; servono **termine trascorso**, **giudizio
  positivo** (media P sopra soglia) e **incarico adeguato**. La caratteristica del zampolit aggiunge o toglie fino al
  10 % (STIMA): pesa il rispetto degli ordini e delle ROE, non i colpi a segno. Promozione **anticipata** per una
  decorazione dal livello dell'Ordine della Stella Rossa in su (STIMA).
- Determinismo: nessun tiro di dado; a parità di partite, stessa carriera.

#### 2.3.4 Rimozione dal comando, degradazione, morte (PROPOSTA)

| Evento | USA | URSS |
|---|---|---|
| Neutrale o amico affondato per colpa del giocatore | sollevato dal comando (caso *George Washington*); corte marziale se ci sono morti e violazioni delle ROE | rimosso; procedimento; possibile perdita di un grado |
| Battello perso, equipaggio salvo (emersione e abbandono) | commissione d'inchiesta: esito secondo il registro (colpa / fatalità) | commissione; colpa: congedo (caso Britanov) o condanna (caso Suvorov) |
| Battello perso con tutto l'equipaggio | fine della carriera; pagina «in memoria», decorazione postuma se l'azione era valorosa | idem; Eroe postumo possibile (caso Bessonov) |
| Incaglio grave, danno nucleare, fuoco senza ordine in pace | rimozione e richiamo; carriera ferma (nessuna promozione per 2 commissioni) | biasimo di partito, attestazione negativa |
| Esito mediocre ripetuto (3 missioni sotto soglia) | trasferimento a terra (come nella missione 4 originale) | rimosso e assegnato a uno stato maggiore |

Livelli di indulgenza (PROPOSTA): **realistico** (sopra), **indulgente** (una «seconda possibilità» per carriera),
**arcade** (nessuna rimozione, solo malus). La schermata è una **commissione d'inchiesta** con il registro degli
eventi decisivi (riusa il debriefing, Fase 4).

#### 2.3.5 Missioni rapide e dell'editor (PROPOSTA, come da requisito 15)

- Contano per la carriera **solo se la missione lo dichiara** con un blocco `carriera = { ... }` nel file di
  missione (`docs/09`), con situazione (pace/crisi/guerra), obiettivi primari e secondari e difficoltà dichiarata.
- Il motore **ricalcola** la difficoltà dai dati (minacce, unità ASW, ambiente acustico): se quella dichiarata è più
  alta, vale la calcolata (contro le medaglie regalate).
- Le missioni **rapide** generate dal gioco contano se generate con seme registrato e non modificate; le missioni
  dell'editor contano nella carriera «aperta», separata da quella «ufficiale» (stessi gradi, fascicolo marcato).
- **Rendimenti decrescenti**: rigiocare la stessa missione conta al 50 %, poi al 25 %, poi zero (come le soglie
  raddoppiate di *F‑19* per una seconda medaglia uguale, [FSB‑F19]).
- Le missioni fuori carriera danno comunque achievement e statistiche.

---

## 3. Decorazioni (1989)

### 3.1 US Navy: decorazioni individuali e di reparto in ordine di precedenza

Ordine di precedenza dalla tabella generale delle forze armate (varia di poco fra le armi) [alta] ([WP‑AWD]);
istituzione e criteri dalle pagine di ogni decorazione; nastrini da [MOU] e dalle pagine dedicate. «Pace» e
«guerra» indicano la plausibilità per il **comandante di un SSN nel 1989** (giudizio dell'autore, [media]).

| # | Decorazione (nome 1989) | Istituita | Criterio in breve | Medaglia | Nastrino | CO SSN in pace | CO SSN in guerra |
|---|---|---|---|---|---|---|---|
| 1 | **Medal of Honor** (versione Navy) | 1861–62; criterio unico dal 1963 | valore in combattimento a rischio della vita, oltre il dovere | stella capovolta di ottone ossidato, punte a trifoglio, Minerva che respinge la Discordia; appesa a un'àncora | al collo, seta azzurra con 13 stelle bianche; nastrino azzurro con stelle bianche | no | eccezionale (7 comandanti di sommergibile nella II guerra mondiale [media]) |
| 2 | **Navy Cross** | 1919 | eroismo straordinario in combattimento, sotto la MOH | croce patente di bronzo con estremità arrotondate, caravella sulle onde, foglie d'alloro | blu marina con striscia bianca centrale | no | **sì**: la decorazione tipica di un attacco riuscito (anche nell'originale) |
| 3 | Defense Distinguished Service Medal | 1970 | servizio eccezionale in incarichi interforze di altissimo livello | — | — | no | no |
| 4 | **Navy Distinguished Service Medal** | 1919 | servizio eccezionalmente meritorio in incarichi di grande responsabilità; di norma ammiragli | medaglione dorato con aquila in anello di smalto blu, stella bianca con àncora sopra | blu scuro con striscia gialla centrale | no (fine carriera da ammiraglio) | raro |
| 5 | **Silver Star** | 1932/1942 | valore in azione contro un nemico, breve periodo | stella dorata con alloro e stellina d'argento al centro | centro rosso, poi bianco e blu, bordi a righine bianche e blu | no | **sì** |
| 6 | Defense Superior Service Medal | 1976 | servizio superiore in incarichi interforze | — | — | no | no |
| 7 | **Legion of Merit** | 1942 | condotta eccezionalmente meritoria in servizi di grande rilievo; in pace incarichi speciali o difficilissimi; di norma O‑6 e oltre; «V» di combattimento ammesso in Marina fino al 2017 | stella bianca bordata di cremisi su corona d'alloro verde, centro blu con 13 stelle | cremisi con bordi bianchi | **sì**: fine di un comando eccellente o operazione speciale | sì (con «V») |
| 8 | Distinguished Flying Cross | 1926 | eroismo in volo | — | — | no (solo aviazione) | no |
| 9 | Navy and Marine Corps Medal | 1942 | eroismo **non in combattimento** a rischio della vita | — | — | sì: incendio o falla a bordo, salvataggio | sì |
| 10 | **Bronze Star Medal** | 1944 | eroismo o merito **in zona di combattimento**; in Marina il «V» indica valore o merito in combattimento | stella di bronzo con stellina centrale | rosso scarlatto, bordi bianchi, sottile riga centrale blu bordata di bianco | no | **sì** (con «V») |
| 11 | **Purple Heart** | 1782/1932 | feriti o uccisi dal nemico | cuore di smalto viola bordato d'oro, profilo di Washington | viola con bordi bianchi | no | sì, se ferito |
| 12 | Defense Meritorious Service Medal | 1977 | merito in incarichi interforze | — | — | no | no |
| 13 | **Meritorious Service Medal** | 1969 | servizio meritorio, sotto la Legion of Merit; in Marina di norma O‑5/O‑6 dopo un comando riuscito | bronzo, aquila su rami d'alloro davanti a una stella raggiata | cremisi con due strisce bianche | **sì**: decorazione tipica di fine comando | sì |
| 14 | Air Medal | 1942 | in volo | — | — | no | no |
| 15 | Joint Service Commendation Medal | 1963 | incarichi interforze | — | — | no | no |
| 16 | **Navy Commendation Medal** | 1943 (nome dal 1960) | merito, risultato eccezionale, coraggio; «V» se in combattimento | medaglia esagonale di bronzo [bassa] | verde scuro con strisce bianche presso i bordi | sì (da ufficiale in seconda o capo servizio) | sì (con «V») |
| 17 | Joint Service Achievement Medal | 1983 | interforze | — | — | no | no |
| 18 | **Navy Achievement Medal** | 1961 (nome dal 1967) | merito o risultato, fino a LCDR | piccola medaglia di bronzo con àncora [bassa] | verde con strisce arancioni ai bordi | sì (gradi bassi) | sì |
| 19 | **Combat Action Ribbon** | 1969 | partecipazione attiva a combattimento di superficie o terrestre, sotto il fuoco | solo nastrino | giallo con sottili righe centrali blu‑bianco‑rosso, un bordo blu e uno rosso | no | **sì** (PROPOSTA, §3.7) |
| U1 | **Presidential Unit Citation** (Navy) | 1942 (per fatti dal 7/12/1941) | eroismo straordinario del reparto, al livello della Navy Cross | — | **righe orizzontali** blu, giallo, rosso (unico nastrino della Marina a righe orizzontali) | sì, per operazioni speciali (*Parche*, *Halibut*) | sì |
| U2 | Joint Meritorious Unit Award | 1981 | reparti interforze | — | — | no | no |
| U3 | **Navy Unit Commendation** | 1944 | eroismo in azione (livello Silver Star) o servizio non di combattimento eccezionale (livello Legion of Merit) | — | verde con bordi blu, giallo e rosso | **sì** | sì (anche nell'originale) |
| U4 | **Meritorious Unit Commendation** | 1967 | reparto distinto in combattimento o no | — | verde con righe centrali blu, giallo e rosso | sì | sì |

Fonti per riga: Medal of Honor [alta] ([WP‑MOH]); Navy Cross [alta] ([WP‑NC]); DSM [alta] ([WP‑NDSM]); Silver Star
[alta] ([WP‑SS]); Legion of Merit [alta] ([WP‑LOM]); Bronze Star [alta] ([WP‑BSM]); Purple Heart nastrino [media]
([MOU]); MSM [alta] ([WP‑MSM]) e nastrino [media] ([MOU]); Commendation [alta] sul nome e sulla storia ([WP‑NCM]),
nastrino [media] ([MOU]); Achievement [alta] sul nome ([WP‑NAM]), nastrino [media] ([MOU]); CAR [alta] su date e
criteri ([WP‑CAR]), nastrino [media] ([MOU]); PUC [alta] ([WP‑PUC]); NUC [alta] su criterio e data ([WP‑NUC]),
nastrino [media] (le fonti commerciali non concordano sulla disposizione delle righe); MUC [media] ([MOU]). Le
decorazioni interforze (righe 3, 6, 12, 15, 17, U2) sono elencate solo per completezza: non vanno nel gioco.
Date delle interforze [bassa].

**Dispositivi** [media]: stellina d'oro da 5/16″ per ogni conferimento successivo, d'argento per cinque; «V» di
bronzo per il combattimento. **Barretta dei nastrini**: file di tre, la decorazione più alta in alto e verso il
centro del petto, l'eventuale fila incompleta in alto e centrata [media].

### 3.2 US Navy: distintivi e nastrini di servizio utili al gioco

| Voce | Data | Criterio | Aspetto | Uso nel gioco | Fonte |
|---|---|---|---|---|---|
| **Dolphins d'oro** | 1924 | qualifica in sommergibili | §1.3 | conquista della fase 2 | [WP‑SWI] [alta] |
| **Command at Sea** | 1960 | comandante (in carica o già stato) di nave o sottomarino, da O‑3 a O‑6 | spilla con sei stelle (le prime sei fregate della Marina); in carica sul **petto destro**, dopo il comando sul sinistro | si appunta al primo comando | [WP‑CAS] [alta]; posizione [media] ([NPC‑5201], estratto) |
| SSBN Deterrent Patrol | per pattuglie dal 1961 | pattuglie di deterrenza degli SSBN | sagoma d'argento di un SSBN con missile Polaris e orbite di elettroni; cartiglio con stelle, d'oro dopo 20 pattuglie | **no** per un SSN (solo per carriere con un passato su SSBN) | [WP‑DPI] [alta] |
| **Submarine Combat Patrol** | II guerra mondiale | pattuglie di combattimento in guerra dichiarata; stelle sul cartiglio (oro per ogni pattuglia riuscita dopo la prima, argento per cinque) | sagoma di un sommergibile classe Gato di profilo | **guerra aperta**: PROPOSTA di riattivarla come distintivo di pattuglia | [WP‑SCP] [alta] |
| Navy «E» Ribbon | 1976 | equipaggio di una nave vincitrice della gara d'efficienza (Battle «E») | blu scuro con bordi giallo e bianco; «E» sul nastrino | premio di **flottiglia** dopo una serie di missioni ottime | [MOU] [media] |
| Navy Expeditionary Medal | 1936 | operazioni speciali senza medaglia di campagna | blu con due strisce gialle | **operazioni segrete in pace** (il *Parche* ne ha 13) | [MOU], [WP‑Parche] [media] |
| National Defense Service Medal | 1953 | servizio attivo nei periodi 1950–54, 1961–74, dal 1990 | rosso con striscia centrale gialla bordata di rosso, bianco e blu | il CO del 1989 (nomina ~1972) **ce l'ha**; un ufficiale nominato dopo il 1974 **no** | [MOU] [alta] sul nastrino; periodi [media] |
| Sea Service Deployment Ribbon | 1980 (retroattiva al 1974) | 90 giorni consecutivi di schieramento | blu scuro, rosso, giallo, azzurro | cresce con le missioni (stelline) | [WP‑SSDR] [alta] su date; [MOU] [media] sul nastrino |
| Navy Arctic Service Ribbon | 1986 (retroattiva al 1982) | 28 giorni oltre il Circolo polare; vale l'emersione attraverso il ghiaccio e le operazioni segrete sotto il pack di 7+ giorni | colori da verificare [bassa] | missioni artiche (Kola, Mare di Barents) | [WP‑NASR] [alta] sui criteri |

### 3.3 US Navy: esempi reali

- **Nautilus, 1958**: il comandante W. R. Anderson riceve la **Legion of Merit** dal presidente Eisenhower per il
  passaggio sotto il Polo; l'equipaggio porta la **PUC con la «N» d'oro** [alta] ([WP‑Anderson], [WP‑PUC]).
- **Triton, 1960**: E. L. Beach, **Legion of Merit** per la prima circumnavigazione in immersione; PUC con il globo
  d'oro all'equipaggio [alta] ([UCSB‑Beach], [WP‑PUC]).
- **Halibut**: PUC nel 1968 (ricerca nazionale, legata al K‑129) e nel 1972 con motivazione volutamente vaga
  («eroismo straordinario» senza nominare l'operazione) [media] ([WP‑Halibut]).
- **Parche** (SSN‑683, operazioni sui cavi sovietici dal 1979): **9 PUC, 10 NUC, 13 Navy Expeditionary Medal**,
  la nave più decorata della storia americana [media] ([WP‑Parche]; le fonti più vecchie danno cifre minori).
- **II guerra mondiale**: Navy Cross per pattuglie di guerra (es. B. E. Bacon, *Pickerel*, 1941–42) [alta]
  ([MT‑Bacon]).
- **Lezione per il gioco**: in **pace** (Guerra fredda) il comandante di un SSN riceve Legion of Merit, MSM o
  Commendation, e l'equipaggio citazioni di reparto e Navy Expeditionary Medal, spesso con motivazioni **segrete**;
  le decorazioni di **valore** (Navy Cross, Silver Star, Bronze Star con «V») richiedono la **guerra**.

### 3.4 VMF: ordini e medaglie in ordine di precedenza

Ordine di precedenza sovietico degli anni '80 [media] (ricostruito dall'autore dalle pagine dei singoli ordini e
dalla prassi; non letto su un testo ufficiale). «Pace» e «guerra» come in §3.1.

| # | Decorazione | Istituita | Criterio in breve | Aspetto | Nastrino | Come si porta | Pace | Guerra |
|---|---|---|---|---|---|---|---|---|
| 1 | **Eroe dell'Unione Sovietica**, medaglia «Stella d'oro» | 1934 / 1939 | impresa eroica; sempre con l'**Ordine di Lenin** e un diploma del Presidium | stella d'oro a cinque punte senza smalto, appesa a un piccolo rettangolo rosso [media] | rosso | in alto a sinistra, sopra tutto [media] | eccezionale: Polo, transiti artici, «incarichi speciali» | sì |
| 2 | **Ordine di Lenin** | 1930 | meriti eccezionali verso lo Stato, servizio esemplare | tondo d'oro, profilo di Lenin in platino su disco grigio, spighe, bandiera rossa «LENIN», stella rossa | rosso con coppie di strisce gialle ai bordi | petto sinistro, appeso al nastro | raro (anche nell'originale) | sì |
| 3 | Ordine della Rivoluzione d'Ottobre | 1967 | meriti per il socialismo e la difesa | — | — | sinistra | raro | raro |
| 4 | **Ordine della Bandiera Rossa** | 1918 / 1924 | coraggio e meriti militari; anche a navi e flotte | stella rossa e falce e martello d'oro su corona d'alloro bianca, bandiera rossa col motto | rosso con bordi bianchi e larga striscia bianca centrale | sinistra | sì: incarico speciale riuscito (Sučkov) o perdita eroica (*Komsomolets*) | **sì** |
| 5 | Ordini di Ušakov e di Nachimov | 1944 | ordini navali di guerra per comandanti di navi e formazioni | — | — | — | no (non conferiti dopo la guerra [media]) | PROPOSTA: tornano in guerra aperta [bassa] |
| 6 | **Ordine della Stella Rossa** | 1930 | coraggio, comando efficace; in pace «grande contributo alla prontezza di combattimento», padronanza di nuove armi | stella a cinque punte di smalto rosso, al centro un soldato col fucile in argento ossidato | rosso scuro con striscia centrale d'argento | **petto destro**, con vite | **sì**: la decorazione «alta» più comune in pace | sì |
| 7 | **Ordine «Per il servizio alla Patria nelle Forze armate dell'URSS»**, 3 classi | 1974 | addestramento al combattimento e politico, prontezza, nuove tecniche, compiti speciali, coraggio; si sale dalla 3ª alla 1ª | due stelle a quattro punte incrociate, quella dietro di smalto azzurro con due razzi d'argento; medaglione con stella e corona di quercia; 1ª classe dorata, 2ª mista, 3ª d'argento | blu con 1 (1ª), 2 (2ª) o 3 (3ª) strisce gialle | destra [media] | **sì**: la progressione tipica di un comandante | sì |
| 8 | Ordine «Per il coraggio personale» | **28 dicembre 1988** | coraggio nel salvare vite, beni dello Stato, catastrofi | stella d'argento dorata | da verificare [bassa] | — | sì: incendio o falla a bordo (1989) | sì |
| 9 | Medaglie di Ušakov, «Per il coraggio», di Nachimov | 1944 / 1938 | valore personale (marinai e sottufficiali; «Per il coraggio» anche ufficiali) | — | — | sinistra | rare | sì (equipaggio) |
| 10 | **Medaglia «Per merito in combattimento»** (За боевые заслуги) | 1938 | difesa delle frontiere, azioni riuscite, ottimi risultati nell'addestramento | tondo d'argento con la scritta [media] | grigio con bordi gialli [bassa] | sinistra | sì (gradi bassi) | sì |
| 11 | **Medaglia «Per distinzione nel servizio militare»**, 2 classi | 1974 | eccellenza nell'addestramento al combattimento e politico, esercitazioni | stella a cinque punte di 38 mm con soldato, marinaio e aviatore; 1ª ottone, 2ª cupronichel | rosso con due strisce verdi presso i bordi | sinistra | sì | — |
| 12 | Medaglie giubilari | 1978, **1988** | «60 anni» e «70 anni delle Forze armate dell'URSS»: a chi era in servizio | — | — | sinistra | automatiche [media] | — |
| 13 | «Veterano delle Forze armate dell'URSS» | 1976 | lunga carriera senza macchia | — | — | sinistra | a fine carriera [media] | — |
| 14 | **«Per il servizio impeccabile»**, 3 classi | 1958 | 20 (1ª), 15 (2ª), 10 (3ª) anni | tondo di 32 mm, stella con falce e martello in corona d'alloro | rosso con bordi verdi e 1, 2 o 3 strisce gialle al centro | sinistra | automatica con gli anni | — |

Fonti per riga: Eroe [alta] ([WP‑HSU]) salvo l'aspetto [media]; Lenin [alta] ([WP‑Lenin]); Bandiera Rossa [alta]
([WP‑RB]); Stella Rossa [alta] ([WP‑RS]); «Per il servizio alla Patria» [alta] ([WP‑SH]); «Per il coraggio personale»
[alta] su data e criteri ([WP‑FPC]); «Per merito in combattimento» [alta] su data e criteri ([WP‑BM]), aspetto e
nastrino [bassa/media]; «Per distinzione» [alta] ([WP‑DMS]); «Per il servizio impeccabile» [alta] ([WP‑IS]);
medaglie giubilari, Veterano, Ušakov e Nachimov [media] (prassi nota, nessuna pagina letta).

**Barrette** (орденские планки): sull'uniforme ordinaria si portano i **nastrini** su barrette rettangolari a
sinistra, gli ordini prima delle medaglie; sull'uniforme da parata le insegne vere, con gli ordini «a vite» (Stella
Rossa, Servizio alla Patria) a destra [media]. Numero di nastrini per fila: da verificare su foto d'epoca [bassa].

### 3.5 VMF: distintivi da petto

| Distintivo | Data | Criterio | Aspetto | Fonte |
|---|---|---|---|---|
| **«Comandante di sottomarino»** (Командир подводной лодки) | 1942; nuovo disegno 1943; dal 1987 variante «sottomarino» del distintivo «Comandante di nave» | consegnato **una volta**, in forma solenne, alla prima nomina stabile al comando | sagoma di un sommergibile in argento ossidato con stella di smalto rosso, circa 68 × 23 mm; fissato a vite | [RUWP‑KPL] [alta] |
| Rombo del diploma della scuola superiore (ВВМУ) | — | laurea di una scuola militare superiore | rombo di smalto bianco o azzurro con emblema; petto destro | [media] |
| «Per un lungo viaggio» (За дальний поход) | anni '60–'70 [bassa] | crociere oceaniche lunghe | da verificare | [RUWP‑KPL] lo nomina solo [bassa] |
| Classe di specializzazione, «Eccellente della Marina» | — | qualifiche tecniche, risultati | piccoli distintivi smaltati | [bassa] |

### 3.6 VMF: esempi reali

- **K‑3, 1962**: Žil'cov porta il primo SSN sovietico al Polo Nord; **Eroe dell'Unione Sovietica** il 20 luglio 1962
  (Ordine di Lenin già nel 1959 per l'entrata in servizio del battello) [alta] ([WP‑Zhiltsov]).
- **1964**: primo transito artico di gruppo da Zapadnaja Lica alla Kamčatka (K‑178): Eroi I. R. Dubjaga e
  A. P. Michajlovskij [media] (estratto, [NM‑K178]).
- **1982**: L. Matuškin, già viceammiraglio, **Eroe** per coraggio e audacia **«negli incarichi speciali del
  comando»**: era stato comandante di SSBN, aveva fatto 12 pattuglie di cui 7 sotto il ghiaccio ed era emerso al Polo
  (K‑245) [media] ([RIAMO‑Matushkin]).
- **Senza Stella d'oro**: il transito sotto il Polo del K‑325 (1978, kapitan 2‑go ranga V. P. Lušin, promosso al
  1° rango nel 1980) [media] ([CYCLO‑Lushin], estratto) e l'operazione «Atrina» (1987, cinque Victor III in
  Atlantico) [media] ([WP‑Atrina]): nessuna ricompensa trovata. Sučkov (§1.6) mostra il profilo **normale**.
- **K‑8, 1970**: Bessonov, Bandiera Rossa per prove di armi, poi **Eroe postumo** dopo la perdita del battello nel
  golfo di Biscaglia [alta] ([WP‑Bessonov]).
- **K‑219, 1986**: il marinaio S. Preminin, morto spegnendo il reattore, **Ordine della Stella Rossa** postumo
  [alta] ([WP‑RS]).
- **Lezione per il gioco**: in pace la scala realistica per il comandante di un Alfa è **«Per distinzione» → «Per il
  servizio alla Patria» (3ª, 2ª) → Stella Rossa → Bandiera Rossa**; l'Ordine di Lenin e la Stella d'oro sono per
  imprese eccezionali (artico, «incarichi speciali») o per la guerra.

### 3.7 Proposta: assegnazione delle decorazioni nel gioco (STIMA / PROPOSTA)

**Principi** (PROPOSTA):

1. Una sola decorazione **personale** al comandante per missione (la più alta meritata), più eventualmente una
   **di reparto** al battello; come in *F‑19*, la decorazione dipende dalla **singola missione** ([FSB‑F19]).
2. La **situazione** della missione filtra le decorazioni: in pace e in crisi non esistono le decorazioni di valore
   in combattimento; in guerra sì.
3. Una seconda decorazione uguale richiede un punteggio più alto (×1,5 per la seconda, ×2 per la terza, STIMA; in
   *F‑19* ×2 e ×3), e si mostra con la stellina (USA) o con il numero di conferimento (URSS).
4. **Decorazioni di fine comando**: dopo 6–10 missioni al comando, una decorazione di carriera secondo la media
   (USA: Navy Commendation, MSM o Legion of Merit; URSS: «Per il servizio alla Patria» della classe successiva).
5. **Decorazioni automatiche** di servizio: Sea Service Deployment Ribbon e stelline (USA), medaglie giubilari e
   «Per il servizio impeccabile» (URSS), secondo il calendario della carriera; nastrini speciali per i teatri artici e
   le operazioni segrete.

**Soglie per missione** (P del §2.3.1; STIMA da tarare sul gioco):

| P (singola missione) | USA in pace o crisi | USA in guerra | URSS in pace o crisi | URSS in guerra |
|---|---|---|---|---|
| ≥ 250 | Navy Achievement (solo ≤ LCDR) | Navy Commendation con «V» | Medaglia «Per distinzione» 2ª cl. | Medaglia «Per merito in combattimento» |
| ≥ 400 | Navy Commendation | Bronze Star con «V» | «Per il servizio alla Patria» 3ª cl. | Stella Rossa |
| ≥ 600 | Meritorious Service Medal | Silver Star | Stella Rossa | Bandiera Rossa |
| ≥ 850 | Legion of Merit | **Navy Cross** | Bandiera Rossa | Ušakov 2ª cl. (PROPOSTA) |
| ≥ 1100 e pericolo massimo | Legion of Merit (non c'è valore senza combattimento) | **Medal of Honor** (solo se il battello ha corso un rischio mortale e il CO non ha violato ROE) | Ordine di Lenin («incarico speciale») | **Eroe dell'Unione Sovietica** con Ordine di Lenin |

**Di reparto** (STIMA): P ≥ 500 e equipaggio senza vittime per colpa propria → Meritorious Unit Commendation (USA);
P ≥ 700 → Navy Unit Commendation; P ≥ 1000 in guerra o operazione speciale → Presidential Unit Citation. Per la VMF
non c'è un equivalente stretto: PROPOSTA di un **encomio del Comandante in capo** nel fascicolo e, per imprese
eccezionali, l'**Ordine della Bandiera Rossa al battello** (gli ordini venivano conferiti anche a navi e flotte,
[WP‑RB]).

**Altre regole** (PROPOSTA): Combat Action Ribbon alla prima missione di guerra con attacco subito; Purple Heart se
la centrale subisce danni con feriti e il comandante è fra questi (evento di danno, `docs/10`); Navy and Marine Corps
Medal o Ordine «Per il coraggio personale» per un salvataggio dopo incendio o falla gestiti in pace; nessuna
decorazione se la missione ha violazioni (V < 0), qualunque sia P.

---

## 4. Uniformi: descrizioni per il ritratto

Solo descrizioni in parole, per disegnare un ritratto d'epoca; nessuna immagine copiata.

### 4.1 US Navy: Service Dress Blue (ufficiale, 1989)

- **Giacca** blu molto scuro (quasi nero), doppio petto, sei bottoni dorati con àncora, risvolti a punta; camicia
  bianca, cravatta nera [media].
- **Galloni d'oro** in fondo alle maniche secondo il grado (§1.1), con la **stella a cinque punte** d'oro sopra per
  gli ufficiali di linea: per un CDR tre galloni medi [alta] sulla stella ([WP‑USNRANK]), [media] sul resto.
- **Berretto** («combination cover»): calotta bianca (o blu in inverno, a seconda dell'epoca [bassa]), fascia nera,
  visiera nera lucida con **una fila di foglie di quercia ricamate d'oro** per CDR e CAPT (due file per gli
  ammiragli), sottogola dorato, fregio con aquila d'argento su ancore incrociate d'oro e scudo [media].
- **Petto sinistro**: in alto le **dolphins d'oro**, sotto la barretta dei nastrini in file di tre [alta]
  ([WP‑SWI]); eventuale distintivo secondario (pattuglia) sotto i nastrini [alta] ([WP‑DPI]).
- **Petto destro**: la spilla **Command at Sea** finché è al comando, dopo passa a sinistra [alta] ([WP‑CAS]); la
  regola attuale la pone circa ¼″ sopra la targhetta o la tasca [media] ([NPC‑5201]).
- **Targhetta col nome**: oggi prevista su molte uniformi; nelle **foto ufficiali** degli anni '80 di solito assente
  [bassa]. PROPOSTA: nel ritratto della cerimonia niente targhetta; nel fascicolo il nome è nella didascalia.
- **Full Dress Blue** (cerimonie): stessa giacca con le **medaglie** al posto dei nastrini, guanti bianchi, spada da
  ufficiale nelle cerimonie maggiori [media]. Per la cerimonia di decorazione basta la Service Dress Blue con i
  nastrini, e la medaglia nuova appuntata appena sopra [bassa].
- **Foto ufficiale** d'epoca: mezzo busto, senza berretto, bandiera o sfondo neutro [bassa].

### 4.2 US Navy: kaki e uniforme di bordo

- **Kaki** (service khaki / working khaki): camicia e pantaloni kaki, **gradi metallici su entrambe le punte del
  colletto** (per un CDR le foglie di quercia d'argento), dolphins d'oro sopra la tasca sinistra, bustina kaki o
  berretto con copertura kaki [alta] sui gradi al colletto ([WP‑USNRANK]); resto [media].
- **A bordo**: tuta blu da lavoro con dolphins ricamate e targhetta in stoffa [media] (ricamo [alta], [WP‑SWI]).
- Uso nel gioco: il kaki per le schermate di briefing e debriefing; la Service Dress Blue per la cerimonia.

### 4.3 VMF: uniforme da ufficiale

- **Ordinaria** (повседневная): **тужурка**, giacca nera a doppio petto aperta sul davanti con due file di bottoni
  dorati con àncora, camicia bianca, cravatta nera con fermacravatta; pantaloni neri [media].
- **Controspalline** (§1.5): per l'uniforme scura gallone dorato con strisce nere (просветы) e stelle; per la
  camicia controspalline nere con strisce gialle [bassa]: fra le fonti trovate non c'è una descrizione affidabile del
  colore delle stelle e del campo; va verificato su foto d'epoca. I **galloni sulle maniche** esistono anche nella
  VMF [media] ([WP‑MilRanks] cita le modifiche del decennio dopo il 1955).
- **Berretto** (фуражка): calotta nera (bianca d'estate), fascia nera, visiera nera; fregio ovale con àncora e
  stella rossa dentro una corona di foglie ricamate d'oro (il «краб», il «granchio»); cordoncino dorato dal 1975
  [media] (un venditore descrive il cordoncino [bassa]); foglie di quercia ricamate sulla visiera per gli ufficiali
  superiori [bassa].
- **Petto sinistro**: barrette dei nastrini; **petto destro**: rombo del diploma, distintivo «Comandante di
  sottomarino» e gli ordini a vite (Stella Rossa) [media].
- **Parata** (парадная): giacca chiusa (китель) o tužurka con le **insegne vere**, guanti bianchi, **pugnale da
  ufficiale** (кортик) al fianco sinistro; d'estate giacca bianca [media]. La grande riforma delle uniformi sovietiche
  del 1969 non ha toccato la Marina nel modello dell'esercito [media] ([IWM69], [GS‑uniform], estratti).
- **A bordo**: tuta da lavoro azzurra o blu con il **basco** o il berretto, distintivo di bordo; d'inverno giubbotto
  imbottito [bassa].

### 4.4 Il ritratto nel gioco (PROPOSTA)

- Ritratto a **strati** in stile VGA 320×200 a 256 colori, come le schermate MicroProse: busto + volto (alcune facce
  scelte alla creazione del personaggio) + **uniforme del grado** (maniche o controspalline) + **barretta generata**
  dai dati (§5.4) + distintivi (dolphins, Command at Sea; rombo e «Comandante di sottomarino»).
- Il ritratto **invecchia** con gli anni di servizio (capelli brizzolati dopo 15 anni, rughe dopo 20).
- Due fondali per marina: banchina con il battello ormeggiato e ufficio del comandante di squadriglia (USA);
  banchina di Zapadnaja Lica con la bandiera navale e sala del circolo ufficiali (URSS).

---

## 5. Cerimonia e schermate

### 5.1 Come sono scritte le motivazioni americane

Struttura costante nelle motivazioni pubblicate [alta] ([MT‑Bacon], [UCSB‑Beach]; altre in [MT]):

1. **Apertura**: chi conferisce («The President of the United States takes pleasure in presenting…» per Navy Cross,
   Silver Star e altre di valore; il Segretario della Marina o un comandante per le minori), la decorazione, grado,
   nome e «United States Navy».
2. **Formula del criterio**: «for extraordinary heroism…» (Navy Cross), «for conspicuous gallantry and intrepidity…»
   (Silver Star), «for exceptionally meritorious conduct in the performance of outstanding services…» (Legion of
   Merit), «for meritorious service/achievement…» (MSM, Commendation, Achievement) [media] sulle formule minori.
3. **Incarico e periodo**: «as Commanding Officer of USS …» e le date, oppure «during the … war patrol».
4. **Corpo**: 3–5 frasi sui fatti (pattuglie, navi affondate, condotta sotto attacco, guida dell'equipaggio), con
   **dettagli omessi** se l'operazione è segreta (motivazioni vaghe come quella del *Halibut*).
5. **Chiusura**: la condotta del decorato fu all'altezza delle «highest traditions of the United States Naval
   Service» [alta] ([MT‑Bacon]); per le minori «reflected great credit upon himself».
6. **Firma**: il presidente, il Segretario della Marina o l'ammiraglio che conferisce.

Esempio di testo di gioco (in italiano, scritto dall'autore sul modello): «Il Presidente degli Stati Uniti ha il
piacere di conferire la Navy Cross al capitano di fregata …, Marina degli Stati Uniti, per lo straordinario eroismo
dimostrato come comandante dell'USS *Los Angeles* (SSN 688) durante operazioni contro forze nemiche nel Mare di
Norvegia dal … al …. Con abile manovra eluse … e affondò …. Il suo coraggio, il suo giudizio e la sua dedizione al
dovere furono all'altezza delle più alte tradizioni della Marina degli Stati Uniti.»

### 5.2 Come sono scritti i decreti sovietici

Struttura dei decreti di conferimento [media] (dalle notizie di decreti pubblicati, es. [RIAMO‑Matushkin]; nessun
testo integrale letto):

1. Intestazione: **Decreto del Presidium del Soviet Supremo dell'URSS**, data, titolo «Sul conferimento…».
2. **Motivazione** in una formula breve e fissa: «per il coraggio e l'audacia dimostrati nell'esecuzione di incarichi
   speciali del comando» (Matuškin, 1982), «per il coraggio e l'eroismo dimostrati nell'adempimento del dovere
   militare», «per i successi nell'addestramento al combattimento e politico», «per la padronanza della nuova tecnica».
3. Dispositivo: «conferire il titolo di Eroe dell'Unione Sovietica con la consegna dell'Ordine di Lenin e della
   medaglia Stella d'oro» oppure «decorare con l'Ordine della Stella Rossa», seguito da grado, nome e patronimico.
4. Firme del presidente e del segretario del Presidium [media]. Nel 1989 presiedeva Gorbačëv [media].
5. Molti decreti per operazioni militari erano **chiusi** (non pubblicati) [media]: nel gioco la cerimonia sovietica
   per una missione segreta si svolge a bordo o al circolo, con il decreto letto ma «non per la stampa».

Il diploma (грамота) del Presidium descrive l'impresa [alta] ([WP‑HSU]). La consegna avveniva in una cerimonia
davanti all'equipaggio schierato, spesso con il comandante della flotta [media].

Esempio di testo di gioco: «Decreto del Presidium del Soviet Supremo dell'URSS. Per il coraggio e l'audacia dimostrati
nell'esecuzione di un incarico speciale del comando, decorare con l'Ordine della Stella Rossa il capitano di 2° rango
…, comandante dell'incrociatore subacqueo K‑…. Mosca, Cremlino, … 1989.»

### 5.3 Il modello *F‑19* / *F‑117A* (dalle fonti pubbliche)

- **Promozioni** in *F‑19*: tre condizioni insieme, missioni volate, punteggio totale e media per missione; scala da
  Second Lieutenant a Brigadier General (99 missioni per l'ultimo grado) [alta sul gioco] ([FSB‑F19] 07_3).
- **Decorazioni** in *F‑19*: dipendono **solo dalla singola missione**: Airman's Medal, DFC, Silver Star, Air Force
  Cross e Medal of Honor a soglie crescenti; una seconda medaglia uguale chiede il doppio, una terza il triplo; a
  parità di soglia vince la più alta [alta sul gioco] ([FSB‑F19] 07_4; i valori differiscono fra le versioni, 06_10).
- **Moltiplicatore** del punteggio fino a circa 9 secondo regione, tensione (guerra fredda, limitata, convenzionale),
  qualità del nemico, realismo, distanza e tipo di missione [alta sul gioco] ([FSB‑F19] 06_10).
- *F‑117A* (1991): 99 missioni, poi la pensione «a una scrivania» [media] ([MR‑F117]); *Silent Service II* (1990):
  carriera di guerra a pattuglie con promozioni ed encomi [media] ([WP‑SS2]).
- **Aspetto delle schermate** (comandante in uniforme, medaglia nel riquadro, motivazione, nastrino che si aggiunge):
  è la descrizione del committente nel requisito 15 ([ROAD]); nessuna fonte aperta trovata la descrive in dettaglio
  [bassa]: va confrontata su una copia propria dei due giochi.

### 5.4 Le schermate del remake (PROPOSTA)

**A. Cerimonia di decorazione** (dopo il debriefing, solo se c'è una decorazione o una promozione):

1. Fondale (§4.4), musica di cerimonia originale o di pubblico dominio verificato (es. inni ottocenteschi; da
   controllare caso per caso).
2. A sinistra il **ritratto** del comandante in Service Dress Blue o in tužurka; a destra la **medaglia in un
   riquadro** (cornice dorata o rossa), disegnata a grandezza grande con il nastro.
3. Sotto, la **motivazione** che si scrive a righe come una telescrivente (stesso effetto del sonar e della radio),
   generata da modelli (§5.1, §5.2) riempiti con i dati della missione: nave, teatro, date, fatti chiave scelti dal
   debriefing (eventi a punteggio più alto).
4. Animazione finale: il **nastrino vola** sulla barretta del ritratto e si inserisce al suo posto in ordine di
   precedenza; per una decorazione ripetuta compare la stellina o il numero.
5. Promozione: le maniche (o le controspalline) cambiano e il nuovo grado si legge nella didascalia; per la VMF il
   decreto del ministero.
6. Tasti: «Continua», «Leggi la motivazione intera», «Al fascicolo».

**B. Costruttore della barretta** (motore, non schermata): dai dati del fascicolo calcola la barretta secondo la
precedenza della marina, file di tre (USA) o di N (URSS, da verificare), dispositivi; serve al ritratto, al
fascicolo e al museo. Una tabella dati per marina con: id, nome d'epoca, precedenza, colori del nastrino a righe (in
unità di larghezza), dispositivi ammessi, criteri del gioco.

**C. Fascicolo personale** («Officer Service Record» / «личное дело»):

- Copertina con timbro **CONFIDENTIAL** o **СЕКРЕТНО**, foto ufficiale (ritratto senza berretto), dati anagrafici
  inventati, provenienza (USNA/NROTC/OCS; nome della VVMU).
- **Stato di servizio**: tabella di incarichi (battello, incarico, dal‑al), qualifiche con data (dolphins, ingegnere,
  comando; esami di guardia, ВСОК, ammissione al comando).
- **Note caratteristiche** o **attestazioni**: una riga per missione con il giudizio a parole (§2.3.1), il voto e la
  graduatoria fra i pari grado simulati; per la VMF anche la caratteristica politica del zampolit.
- **Decorazioni** in ordine di precedenza, ognuna apribile con la motivazione intera e la data.
- **Provvedimenti**: rimozioni, commissioni d'inchiesta, biasimi.
- Chiusura: congedo, pensione o pagina «in memoria»; la carriera finita resta consultabile nell'albo (statistiche e
  achievement, requisito 15).

**D. Commissione di selezione / attestazione** (breve): verbale con il risultato («selezionato», «non
selezionato», «promosso in anticipo»), le medie e le note migliori e peggiori.

**E. Commissione d'inchiesta** (dopo un disastro): eventi decisivi dal registro, verdetto e conseguenze (§2.3.4).

---

## 6. Lacune note

1. **Tempi esatti della carriera USN negli anni '80** (durate di Nuclear Power School, prototipo, SOBC; anni medi a
   XO e CO): fonti attuali e testimonianze; manca un documento d'epoca (es. «Submarine Officer Career Path» del 1985).
2. **Note caratteristiche** del 1989 (NAVPERS 1611/1): forma, voti, graduatorie non lette.
3. **Anzianità minime e attestazione sovietiche**: il «Положение о прохождении воинской службы офицерским составом»
   del 1985 non trovato; i rapporti CIA del 1954 irraggiungibili (sito in manutenzione).
4. **Precedenza sovietica** del 1989, nastrini per fila, posizione dei distintivi, **uniforme navale** (colore di
   stelle e campo delle controspalline, galloni, visiera): ricostruiti dall'autore, da verificare su foto d'epoca.
5. **Nastrini** di NUC (disposizione), Navy Arctic Service Ribbon, «Per merito in combattimento», «Per il coraggio
   personale»: descrizioni discordanti o assenti (servono Institute of Heraldry e un catalogo sovietico).
6. **Esempi 1980–1989**: ricompense ai comandanti di SSN americani per operazioni speciali e ai comandanti di Alfa e
   Victor III non trovate; il comandante del K‑123 e il titolo di Eroe attribuitogli vanno verificati.
7. **Schermate di *F‑19* e *F‑117A***: nessuna descrizione aperta della cerimonia; serve un confronto sui giochi.

---

## 7. Fonti

### Progetto

- `[ROAD]` `docs/00_ROADMAP.md`, requisiti 15 e 16, Fasi 4 e 5.
- `[ORIG‑BR]` `docs/originale/briefing_EN.txt`, pagine d'esito delle 10 missioni del 1989.

### Web (consultate il 24 ottobre 2026)

**US Navy: carriera e promozioni**
- `[WP‑USNRANK]` «United States Navy officer rank insignia», Wikipedia: https://en.wikipedia.org/wiki/United_States_Navy_officer_rank_insignia
- `[WP‑DOPMA]` «Defense Officer Personnel Management Act», Wikipedia: https://en.wikipedia.org/wiki/Defense_Officer_Personnel_Management_Act
- `[WP‑NPS]` «Naval Nuclear Power School», Wikipedia: https://en.wikipedia.org/wiki/Naval_Nuclear_Power_School
- `[NSL91]` *The Submarine Review*, luglio 1991, articolo di un comandante del *Philadelphia* sulla carriera: https://archive.navalsubleague.org/?p=1664
- `[NPC‑TACAID]` MyNavy HR, «Submarine Officer Career Path and Milestones» (TACAID, attuale): https://www.mynavyhr.navy.mil/Portals/55/Career/Detailing/Officer/SubmarineNuclear/Career/Submarine%20Officer%20Career%20Path%20and%20Milestones%20TACAID%20CH-3.pdf (estratto)
- `[USNI24]` «The submarine community needs a new officer career path», *Proceedings*, luglio 2024: https://www.usni.org/magazines/proceedings/2024/july/submarine-community-needs-new-officer-career-path (estratto)
- `[USNI92‑Rick]` J. W. Crawford, «Passing Rickover's muster», *Naval History*, marzo 1992: https://www.usni.org/magazines/naval-history-magazine/1992/march/passing-rickovers-muster (estratto)
- `[USSR‑rcl]` Testimonianze su Rickover (esame da ingegnere, corso PCO del 1980), USS Rickover Association: https://ussrickover.org/?p=920 ; https://ussrickover.org/rcl-john-sheehan/ (estratti)
- `[USNI60]` «Officer evaluation and promotion», *Proceedings*, marzo 1960: https://www.usni.org/magazines/proceedings/1960/march/officer-evaluation-and-promotion (estratto); BUPERSINST 1610.10: https://www.mynavyhr.navy.mil/Portals/55/Reference/Instructions/BUPERS/BUPERSINST%201610.10.pdf (estratto)
- `[WP‑GW]` «USS George Washington (SSBN‑598)», Wikipedia: https://en.wikipedia.org/wiki/USS_George_Washington_(SSBN-598)

**US Navy: decorazioni e distintivi**
- `[WP‑AWD]` «Awards and decorations of the United States Armed Forces», Wikipedia: https://en.wikipedia.org/wiki/Awards_and_decorations_of_the_United_States_Armed_Forces
- `[WP‑MOH]` https://en.wikipedia.org/wiki/Medal_of_Honor ; `[WP‑NC]` https://en.wikipedia.org/wiki/Navy_Cross ; `[WP‑NDSM]` https://en.wikipedia.org/wiki/Navy_Distinguished_Service_Medal
- `[WP‑SS]` https://en.wikipedia.org/wiki/Silver_Star ; `[WP‑LOM]` https://en.wikipedia.org/wiki/Legion_of_Merit ; `[WP‑BSM]` https://en.wikipedia.org/wiki/Bronze_Star_Medal
- `[WP‑MSM]` https://en.wikipedia.org/wiki/Meritorious_Service_Medal_(United_States) ; `[WP‑NCM]` https://en.wikipedia.org/wiki/Navy_and_Marine_Corps_Commendation_Medal ; `[WP‑NAM]` https://en.wikipedia.org/wiki/Navy_and_Marine_Corps_Achievement_Medal
- `[WP‑CAR]` https://en.wikipedia.org/wiki/Combat_Action_Ribbon ; `[WP‑PUC]` https://en.wikipedia.org/wiki/Presidential_Unit_Citation_(United_States) ; `[WP‑NUC]` https://en.wikipedia.org/wiki/Navy_Unit_Commendation
- `[WP‑SSDR]` https://en.wikipedia.org/wiki/Sea_Service_Deployment_Ribbon ; `[WP‑NASR]` https://en.wikipedia.org/wiki/Navy_Arctic_Service_Ribbon
- `[MOU]` medals.org.uk, schede USA (nastrini, date): https://medals.org.uk/usa/usa-text.htm ; schede usa011, usa049, usa053, usa059, usa065, usa070, usa073, usa079, usa097, usa134, usa144 (stesso sito)
- `[WP‑SWI]` «Submarine Warfare insignia», Wikipedia: https://en.wikipedia.org/wiki/Submarine_Warfare_insignia
- `[WP‑CAS]` «Command at Sea insignia», Wikipedia: https://en.wikipedia.org/wiki/Command_at_Sea_insignia ; `[NPC‑5201]` Uniform Regulations, art. 5201: https://www.mynavyhr.navy.mil/References/US-Navy-Uniforms/Uniform-Regulations/Chapter-5/5201-Breast-Insignia-2/ (estratto)
- `[WP‑DPI]` https://en.wikipedia.org/wiki/SSBN_Deterrent_Patrol_insignia ; `[WP‑SCP]` https://en.wikipedia.org/wiki/Submarine_Combat_Patrol_insignia
- `[WP‑Parche]` https://en.wikipedia.org/wiki/USS_Parche_(SSN-683) ; `[WP‑Halibut]` https://en.wikipedia.org/wiki/USS_Halibut_(SSGN-587)
- `[WP‑Anderson]` https://en.wikipedia.org/wiki/William_R._Anderson ; `[UCSB‑Beach]` American Presidency Project, citazione della Legion of Merit a E. L. Beach (1960): https://www.presidency.ucsb.edu/node/234325
- `[MT‑Bacon]` Military Times Hall of Valor, Navy Cross a B. E. Bacon Jr. (*Pickerel*): https://valor.militarytimes.com/?p=185707 ; `[MT]` altre motivazioni sullo stesso sito (es. ?p=346145, ?p=198118) (estratti)

**VMF**
- `[WP‑SovRanks]` «Ranks and insignia of the Soviet Armed Forces 1955–1991», Wikipedia: https://en.wikipedia.org/wiki/Ranks_and_insignia_of_the_Soviet_Armed_Forces_1955–1991 ; `[WP‑MilRanks]` https://en.wikipedia.org/wiki/Military_ranks_of_the_Soviet_Union
- `[FBIS‑Suchkov]` FBIS, scheda biografica del viceammiraglio G. A. Sučkov (2002), FAS: https://irp.fas.org/world/russia/fbis/SUCHKOVBiography.html
- `[WP‑Zhiltsov]` https://en.wikipedia.org/wiki/Lev_Zhiltsov ; `[WP‑Bessonov]` https://en.wikipedia.org/wiki/Vsevolod_Bessonov
- `[RIAMO‑Matushkin]` RIAMO, targa in memoria di L. Matuškin (2018): https://riamo.ru/news/obschestvo/v-moskve-otkryli-memorialnuyu-dosku-v-chest-podvodnika-lva-matushkina-xl/
- `[CYCLO‑Lushin]` Cyclowiki, V. P. Lušin: https://cyclowiki.org/wiki/Владимир_Петрович_Лушин (estratto) ; `[NM‑K178]` Museo navale centrale, transito del K‑178: https://eng.navalmuseum.ru/news/memorable?id=907 (estratto)
- `[WP‑K429]` https://en.wikipedia.org/wiki/Soviet_submarine_K-429 ; `[WP‑Britanov]` https://en.wikipedia.org/wiki/Igor_Britanov ; `[WP‑K278]` https://en.wikipedia.org/wiki/Soviet_submarine_K-278_Komsomolets ; `[WP‑Atrina]` https://en.wikipedia.org/wiki/Operation_Atrina
- `[WP‑Alfa]` https://en.wikipedia.org/wiki/Alfa-class_submarine ; `[GS‑705]` GlobalSecurity, elenco dei 705: https://www.globalsecurity.org/military/world/russia/705-list.htm (estratto)
- `[WP‑PC]` «Political commissar», Wikipedia: https://en.wikipedia.org/wiki/Political_commissar
- `[CIA‑BU83]` CIA, traduzione del regolamento di combattimento della Marina sovietica (1983): https://www.cia.gov/readingroom/node/2062266 (estratto)
- `[WP‑HSU]` https://en.wikipedia.org/wiki/Hero_of_the_Soviet_Union ; `[WP‑Lenin]` https://en.wikipedia.org/wiki/Order_of_Lenin ; `[WP‑RB]` https://en.wikipedia.org/wiki/Order_of_the_Red_Banner ; `[WP‑RS]` https://en.wikipedia.org/wiki/Order_of_the_Red_Star
- `[WP‑SH]` https://en.wikipedia.org/wiki/Order_%22For_Service_to_the_Homeland_in_the_Armed_Forces_of_the_USSR%22 ; `[WP‑FPC]` https://en.wikipedia.org/wiki/Order_%22For_Personal_Courage%22
- `[WP‑BM]` https://en.wikipedia.org/wiki/Medal_%22For_Battle_Merit%22 ; `[WP‑DMS]` https://en.wikipedia.org/wiki/Medal_%22For_Distinction_in_Military_Service%22 ; `[WP‑IS]` https://en.wikipedia.org/wiki/Medal_%22For_Impeccable_Service%22
- `[RUWP‑KPL]` «Знак "Командир подводной лодки"», Wikipedia in russo: https://ru.wikipedia.org/wiki/Знак_«Командир_подводной_лодки»
- `[GS‑uniform]` GlobalSecurity, uniformi russe e sovietiche: https://www.globalsecurity.org/military/world/russia/uniform.htm (estratto) ; `[IWM69]` Imperial War Museum, scheda sulla riforma delle uniformi del 1969: https://www.iwm.org.uk/collections/item/object/30096349 (estratto)

**Giochi**
- `[FSB‑F19]` *The Official F‑19 Stealth Fighter Handbook*, flightsimbooks.com: https://www.flightsimbooks.com/f19stealthfighter/06_10_Scoring_System.php ; …/07_3_Promotions.php ; …/07_4_Decorations_for_Valor.php
- `[MR‑F117]` Macintosh Repository, *F‑117A Nighthawk Stealth Fighter 2.0*: https://www.macintoshrepository.org/5190-f-117a-nighthawk-stealth-fighter-2-0 (estratto)
- `[WP‑SS2]` «Silent Service II», Wikipedia: https://en.wikipedia.org/wiki/Silent_Service_II (estratto)
