# R8 — Elenco curato dei campioni di aerei ed elicotteri (da far approvare prima di scaricare)

> Pezzo R8 del dossier `docs/ricerca/18_audio_reale.md` (§1 «Mezzi aerei», §2.3, §6.4, §8). Voci del catalogo:
> `tools/pipeline/sorgenti_asw.py` (aerei, elicotteri, `hover_spruzzi`). Uso: tessitura sbiancata e ricolorata dal
> modello (§2): conta il timbro di rotore, turboelica o turbofan, non la qualità assoluta.
> Compilato il 25/10/2026. **Nessun file audio o video è stato scaricato.** Ho usato solo: l'API di Wikimedia Commons
> (`list=search`, `list=categorymembers`, `prop=imageinfo` con `extmetadata|metadata`, `prop=videoinfo` con
> `derivatives`, `prop=revisions` per leggere i modelli di licenza), l'API `images-api.nasa.gov`, l'API
> `advancedsearch`/`metadata` di archive.org, richieste HEAD sui file e, per i transcodifiche WebM, **una richiesta
> Range di 8 KB** per leggere l'intestazione Opus (canali, frequenza); nessun campione salvato.

Vincoli applicati: solo CC0, pubblico dominio (opere federali USA, 17 U.S.C. §105; PD dichiarato dall'autore) e
CC BY (3.0 o 4.0); esclusi CC BY-SA, NC, ND e licenze governative «compatibili» ma non CC (Giappone, Taiwan, India).
Esclusi Freesound (scaricamento con account) e BBC. Tutti i file proposti stanno su `upload.wikimedia.org`:
scaricabili senza account (il server ha risposto 429 a raffiche di HEAD: scaricare con calma e con un User-Agent).

**Quasi tutti i candidati sono video** (Commons ha pochissimi file solo audio di aerei con licenze libere): la
traccia audio si estrae dopo il download. Per i video grandi propongo la **transcodifica 240p VP9+Opus** che Commons
genera da sé (stessa traccia audio ricodificata in Opus stereo 48 kHz; è una seconda compressione con perdita, che
per una tessitura sbiancata in 30-2000 Hz non pesa); per i piccoli l'originale. Colonna «Byte» = file da scaricare
(`Content-Length` letto con HEAD o `size` dell'API); tra parentesi l'originale quando si propone la 240p.

Legenda «Licenza»: **PD-§105** = opera federale USA; **PD-autore** = dichiarato di pubblico dominio dall'autore;
**CC0**; **CC BY 3.0/4.0**. «Da ascoltare» = non ho potuto sentire la traccia: musica, commento o spari possibili.

Attribuzioni (stringhe per i crediti; colonna «Attr.»):

- **A-MIL**: «Ministry of Defence of the Russian Federation (mil.ru / минобороны.рф), CC BY 4.0
  (https://creativecommons.org/licenses/by/4.0/), via Wikimedia Commons, <pagina del file>. Modificato: estratto
  audio, sbiancato e ricolorato.» (modello Commons `{{Mil.ru}}` = `{{Cc-by-4.0|Mil.ru}}`: «attribute Mil.ru»).
- **A-KRM**: «Presidential Press and Information Office, www.kremlin.ru, CC BY 4.0 (prima dell'8/4/2015 anche
  CC BY 3.0), via Wikimedia Commons, <pagina>. Modificato.» (modello `{{Kremlin.ru}}`).
- **A-YT**: «"<titolo>" di <autore> (YouTube <id>), CC BY 3.0 (https://creativecommons.org/licenses/by/3.0/), via
  Wikimedia Commons, <pagina>. Modificato.» (licenza CC BY di YouTube verificata dai revisori di Commons).
- **A-BY4**: «"<titolo>" di <autore>, CC BY 4.0, via Wikimedia Commons, <pagina>. Modificato.»
- **A-US**: nessun obbligo; credito di cortesia «<forza armata> video by <autore>, via DVIDS / Wikimedia Commons».
  **Clinton Library**: la pagina chiede il credito «Courtesy; William J. Clinton Presidential Library».
- **A-0**: CC0 o PD-autore, nessun obbligo; credito di cortesia «<autore>, via Wikimedia Commons».

---

## 1. `aereo_p3c` — P-3C Orion (4 × Allison T56, eliche quadripala Hamilton 54H60)

| # | File | Licenza | Attr. | Formato (audio) | Byte | Durata | Affidabilità |
|---|---|---|---|---|---|---|---|
| U1 | [Fat Albert low pass.ogv](https://commons.wikimedia.org/wiki/File:Fat_Albert_low_pass.ogv) — C-130T «Fat Albert» dei Blue Angels (USMC) in passaggio basso sul Silent Drill Platoon, MCAS Yuma, 4/3/2014; DVIDS 322867 | PD-§105 (`{{PD-USGov-Military}}`, SSgt Oscar L. Olive IV, USMC) | A-US | Ogg Theora + **Vorbis 48 kHz stereo, ~224 kb/s** (originale) | 2.293.021 | 12,0 s | **Alta per il motore**: C-130T = T56-A-16 con 54H60, la stessa famiglia del P-3C (T56-A-14, 54H60-77); ripresa da terra di un militare USA, un solo passaggio con Doppler. Corta: basta per un loop di 3-5 s. Da ascoltare (DVIDS a volte aggiunge musica, qui improbabile). |
| U2 | [US Navy Lockheed WP-3D Orion recon aircraft.ogv](https://commons.wikimedia.org/wiki/File:US_Navy_Lockheed_WP-3D_Orion_recon_aircraft.ogv) — P-3 (WP-3D) ripreso nel 2012 | CC0 (`{{self|Cc-zero}}`, John Veit, «own work») | A-0 | Ogg Theora + **Vorbis 44,1 kHz stereo, ~192 kb/s** | 2.098.554 | 8,3 s | **Media**: è davvero una cellula P-3 con T56-A-14 (l'unico P-3 vero trovato con licenza libera); opera propria dichiarata. La descrizione non dice se sia un passaggio, un rullaggio o un motore al minimo: decidere dopo l'ascolto. |

## 2. `aereo_il38` — Il-38 (4 × Ivchenko AI-20M; vanno Il-18 e An-12)

| # | File | Licenza | Attr. | Formato (audio) | Byte | Durata | Affidabilità |
|---|---|---|---|---|---|---|---|
| U3 | [Antonov An-12 taking off from Malta International Airport.ogv](https://commons.wikimedia.org/wiki/File:Antonov_An-12_taking_off_from_Malta_International_Airport.ogv) — An-12 di Volare Air al decollo, Malta | CC BY 3.0 (YouTube `blzV1AvMPoY`, autore «Joluqa», licenza rivista) | A-YT | 240p: WebM VP9 + **Opus stereo 48 kHz** (banda totale ~229 kb/s); originale: Theora + Vorbis 44,1 kHz stereo ~152 kb/s | 2.794.898 (orig. 55.865.095) | 97,8 s | **Alta**: An-12 = 4 × AI-20M con eliche AV-68, gli stessi motori dell'Il-38; ripresa da terra di uno spotter, decollo con allontanamento (salita a piena potenza). Credibile come opera propria. |

## 3. `aereo_be12` — Be-12 (2 × AI-20; turboelica bimotore sovietico)

| # | File | Licenza | Attr. | Formato (audio) | Byte | Durata | Affidabilità |
|---|---|---|---|---|---|---|---|
| U4 | [Sound of AN-24 airplane.ogg](https://commons.wikimedia.org/wiki/File:Sound_of_AN-24_airplane.ogg) — «suono di un An-24 in sorvolo», Mosca, 29/1/2008 | PD-autore (`{{PD-user-w|ru|wikipedia|Grachev}}`: «Released into the public domain (by the author)») | A-0 | **Ogg Vorbis 48 kHz stereo, 128 kb/s** (solo audio) | 491.365 | 46,8 s | **Media-alta**: registrazione propria con dittafono (lo dice l'autore); An-24 = 2 × AI-24, il fratello minore dell'AI-20 (stessa scuola Ivchenko, eliche quadripala): timbro bimotore sovietico vicino al Be-12, giri e potenza diversi. Qualità da dittafono, ma basta per la tessitura. Il tipo non è verificabile. |

Riserva: [Antonov An-26 Taking Off from Leipzig-Halle Airport.ogv](https://commons.wikimedia.org/wiki/File:Antonov_An-26_Taking_Off_from_Leipzig-Halle_Airport.ogv)
(CC BY 3.0, YouTube `h6aASJ4ail4`, «LEJ.approach/dvldi»; 2 × AI-24VT più il getto ausiliario RU-19; 240p 6.041.882 B,
orig. 149.597.038 B; 139 s; ripresa da spotter, buona). URL in fondo (R1).

## 4. `aereo_tu142` — Tu-142 (4 × NK-12, eliche controrotanti; va il Tu-95)

| # | File | Licenza | Attr. | Formato (audio) | Byte | Durata | Affidabilità |
|---|---|---|---|---|---|---|---|
| U5 | [Полеты Ту-95МС в условиях низких температур.webm](https://commons.wikimedia.org/wiki/File:%D0%9F%D0%BE%D0%BB%D0%B5%D1%82%D1%8B_%D0%A2%D1%83-95%D0%9C%D0%A1_%D0%B2_%D1%83%D1%81%D0%BB%D0%BE%D0%B2%D0%B8%D1%8F%D1%85_%D0%BD%D0%B8%D0%B7%D0%BA%D0%B8%D1%85_%D1%82%D0%B5%D0%BC%D0%BF%D0%B5%D1%80%D0%B0%D1%82%D1%83%D1%80.webm) — voli dei Tu-95MS del 184° reggimento a Engels in inverno, 15/1/2020 | CC BY 4.0 (`{{Mil.ru|year=2020}}`) | A-MIL | WebM VP8/9 + **Vorbis 44,1 kHz stereo, ~128 kb/s** (originale) | 9.465.148 | 39,6 s | **Alta per il tipo** (NK-12MP, le stesse eliche controrotanti AV-60 del Tu-142); fonte ufficiale. **Da ascoltare**: i filmati del ministero hanno spesso musica o voce fuori campo. |
| U6 | [Tupolev Tu-95.ogv](https://commons.wikimedia.org/wiki/File:Tupolev_Tu-95.ogv) — sorvolo di un Tu-95 con un Il-78 e due MiG-31 alla parata del 9 maggio 2008, Mosca | CC BY 4.0 / CC BY 3.0 (`{{Kremlin.ru|version=2008}}`) | A-KRM | Theora + **Vorbis 48 kHz stereo, ~64 kb/s** | 1.744.717 | 28,6 s | **Media**: Tu-95 reale ripreso da terra, ma nel suono entrano i getti dell'Il-78 e dei MiG-31 e forse la telecronaca; bitrate basso. Seconda scelta, piccola. |

## 5. `aereo_s3` — S-3 Viking (2 × GE TF34; vanno A-10 o CRJ)

| # | File | Licenza | Attr. | Formato (audio) | Byte | Durata | Affidabilità |
|---|---|---|---|---|---|---|---|
| U7 | [A-10's perform highway landing in Estonia - 160801-Z-QO433-001.webm](https://commons.wikimedia.org/wiki/File:A-10%27s_perform_highway_landing_in_Estonia_-_160801-Z-QO433-001.webm) — B-roll degli A-10 del 442nd FW che atterrano e decollano dalla statale di Jägala, Estonia, 1/8/2016; DVIDS 477603 | PD-§105 (Iowa ANG, SSgt David Asbra) | A-US | 240p: VP9 + **Opus stereo 48 kHz** (banda ~181 kb/s); originale: **Vorbis 48 kHz stereo ~224 kb/s** | 4.014.837 (orig. 31.694.346) | 177,1 s | **Alta per il motore**: A-10 = 2 × TF34-GE-100, la stessa famiglia del TF34-GE-400 dell'S-3; B-roll da terra (di norma solo suono ambiente), più passaggi, atterraggi e decolli. Se il budget lo consente, meglio l'originale. |

Riserva: [HIGH WINDS FullHD Planespotting Compilation Ljubljana Airport](https://commons.wikimedia.org/wiki/File:HIGH_WINDS_FullHD_Planespotting_Compilation_Ljubljana_Airport_(Incl._Saab_2000,_Policia_Helicopter).webm)
(CC BY 3.0, YouTube, «Vienna Plane Spotting»; da 0:50 a 6:35 cinque CRJ-700/900 con CF34, il derivato civile del TF34;
240p 21.645.893 B, orig. 241.060.508 B; 600 s). URL in fondo (R2).

## 6. `elicottero_sh60` — SH-60 / UH-60

| # | File | Licenza | Attr. | Formato (audio) | Byte | Durata | Affidabilità |
|---|---|---|---|---|---|---|---|
| U8 | [JMSDF SH-60J Sapporo Airshow Okadama Airport.ogv](https://commons.wikimedia.org/wiki/File:JMSDF_SH-60J_Sapporo_Airshow_Okadama_Airport.ogv) — arrivo di un SH-60J della Marina giapponese, Okadama, 8/8/2012 | CC BY 3.0 (YouTube `zCGzVZeXj6s`, «RUNWAY FUN Aviation & Military & plus»; licenza rivista) | A-YT | 240p: VP9 + **Opus stereo 48 kHz** (banda ~375 kb/s); originale: Vorbis 44,1 kHz stereo ~500 kb/s | 8.084.033 (orig. 174.839.209) | 172,4 s | **Alta per il tipo**: l'SH-60J è l'SH-60B costruito su licenza (stessi rotori e T700); ripresa da terra di uno spotter, avvicinamento e hover. Possibile speaker della manifestazione. Licenza CC BY dello spotter, non del governo giapponese. |
| U9 | [BlackhawksLilburnGeorgia.webm](https://commons.wikimedia.org/wiki/File:BlackhawksLilburnGeorgia.webm) — quattro UH-60 in volo vicino all'autore, Lilburn (Georgia), 12/7/2025 | CC BY 4.0 («BeeboMan», opera propria) | A-BY4 | WebM + **Opus mono 48 kHz** (originale) | 5.702.745 | 18,3 s | **Alta**: UH-60 confermati dalla categoria e dalla descrizione, registrazione propria da terra (telefono), passaggio di una formazione. |

Riserva: [Blackhawk flyover at MLB Fort Bragg Game.webm](https://commons.wikimedia.org/wiki/File:Blackhawk_flyover_at_MLB_Fort_Bragg_Game.webm)
(PD-§105, U.S. Army, DVIDS 473397, B-roll; Vorbis 48 kHz stereo; orig. 12.566.287 B, 240p 1.054.213 B; 21,9 s; possibile
pubblico dello stadio). URL in fondo (R3).

## 7. `elicottero_sh3` — SH-3 Sea King

| # | File | Licenza | Attr. | Formato (audio) | Byte | Durata | Affidabilità |
|---|---|---|---|---|---|---|---|
| U10 | [President Clinton Departing the White House via Marine One (1998).webm](https://commons.wikimedia.org/wiki/File:President_Clinton_Departing_the_White_House_via_Marine_One_(1998).webm) — girato grezzo della White House Television, 18/8/1998, South Lawn | PD-§105 (`{{PD-USGov-POTUS}}`; Clinton Library: «unrestricted») | A-US (+ «Courtesy; William J. Clinton Presidential Library») | 240p: VP9 + **Opus stereo 48 kHz** (banda ~247 kb/s); originale: Vorbis 44,1 kHz stereo ~128 kb/s | 15.306.445 (orig. 41.029.268) | 496,0 s | **Media**: Marine One nel 1998 era quasi sempre un VH-3D Sea King (T58, rotore a 5 pale come l'SH-3; la compilazione dello stesso archivio dichiara VH-3D per tutte le clip), ma l'HMX-1 usava anche il VH-60N: **verificare a vista**. Messa in moto, decollo e allontanamento da terra; voci e folla da tagliare. |
| U11 | [Desert Duck 744 Accident.webm](https://commons.wikimedia.org/wiki/File:Desert_Duck_744_Accident.webm) — UH-3H Sea King dell'HC-2 in appontaggio sull'USS Cushing, 5/7/2002 | PD-§105 (`PD US Government`, U.S. Navy) | A-US | Theora/VP8 + **Vorbis 44,1 kHz stereo ~224 kb/s** (originale, 320 × 240) | 1.330.485 | 24,9 s | **Alta per il tipo, bassa per l'uso**: è davvero un Sea King, ma il video è un incidente (avaria del rotore di coda e caduta in mare): serve solo l'avvicinamento, se la traccia ha suono di presa diretta. Valutare anche l'opportunità. |

Riserva: [Compilation of Marine One Clips.webm](https://commons.wikimedia.org/wiki/File:Compilation_of_Marine_One_Clips.webm)
(PD-§105, VH-3D dichiarato, 31 min; 240p 55.407.968 B, orig. 172.266.185 B): troppo grande per il budget, ma è la fonte
Sea King più ricca. URL in fondo (R4).

## 8. `elicottero_ka25` / `elicottero_ka27` — rotori coassiali Kamov

| # | File | Licenza | Attr. | Formato (audio) | Byte | Durata | Affidabilità |
|---|---|---|---|---|---|---|---|
| U12 | [Полёты и стрельба вертолетов Ка-52К и Ка-29 … «Адмирал Кузнецов».ogv](https://commons.wikimedia.org/wiki/File:%D0%9F%D0%BE%D0%BB%D1%91%D1%82%D1%8B_%D0%B8_%D1%81%D1%82%D1%80%D0%B5%D0%BB%D1%8C%D0%B1%D0%B0_%D0%B2%D0%B5%D1%80%D1%82%D0%BE%D0%BB%D0%B5%D1%82%D0%BE%D0%B2_%D0%9A%D0%B0-52%D0%9A_%D0%B8_%D0%9A%D0%B0-29_%D0%B8%D0%B7_%D1%81%D0%BE%D1%81%D1%82%D0%B0%D0%B2%D0%B0_%D0%BF%D0%B0%D0%BB%D1%83%D0%B1%D0%BD%D0%BE%D0%B9_%D0%B0%D0%B2%D0%B8%D0%B0%D1%86%D0%B8%D0%B8_%D0%A2%D0%90%D0%92%D0%9A%D0%A0_%C2%AB%D0%90%D0%B4%D0%BC%D0%B8%D1%80%D0%B0%D0%BB_%D0%9A%D1%83%D0%B7%D0%BD%D0%B5%D1%86%D0%BE%D0%B2%C2%BB.ogv) — voli e tiri di Ka-52K e Ka-29 dalla portaerei Kuznetsov, 2017 | CC BY 4.0 (Mil.ru; licenza rivista) | A-MIL | 240p: VP9 + **Opus stereo 48 kHz** (banda ~354 kb/s); originale: Vorbis 44,1 kHz stereo ~500 kb/s | 6.742.214 (orig. 258.142.128) | 152,3 s | **Media**: il Ka-29 è un Ka-27 d'assalto (stesso sistema di rotori e TV3-117), il Ka-52K è un Kamov coassiale; fonte ufficiale. **Da ascoltare**: spari («стрельба») e forse musica; scegliere un tratto di solo rotore. |
| U13 | [-На Камчатке летчики морской авиации … комплексное учение.webm](https://commons.wikimedia.org/wiki/File:-%D0%9D%D0%B0_%D0%9A%D0%B0%D0%BC%D1%87%D0%B0%D1%82%D0%BA%D0%B5_%D0%BB%D0%B5%D1%82%D1%87%D0%B8%D0%BA%D0%B8_%D0%BC%D0%BE%D1%80%D1%81%D0%BA%D0%BE%D0%B9_%D0%B0%D0%B2%D0%B8%D0%B0%D1%86%D0%B8%D0%B8_%D0%B8_%D0%B1%D0%BE%D0%B9%D1%86%D1%8B_%D0%BF%D1%80%D0%BE%D1%82%D0%B8%D0%B2%D0%BE%D0%B4%D0%B8%D0%B2%D0%B5%D1%80%D1%81%D0%B8%D0%BE%D0%BD%D0%BD%D0%BE%D0%B3%D0%BE_%D0%BE%D1%82%D1%80%D1%8F%D0%B4%D0%B0_%D0%A2%D0%B8%D1%85%D0%BE%D0%BE%D0%BA%D0%B5%D0%B0%D0%BD%D1%81%D0%BA%D0%BE%D0%B3%D0%BE_%D1%84%D0%BB%D0%BE%D1%82%D0%B0_%D0%BF%D1%80%D0%BE%D0%B2%D0%B5%D0%BB%D0%B8_%D0%BA%D0%BE%D0%BC%D0%BF%D0%BB%D0%B5%D0%BA%D1%81%D0%BD%D0%BE%D0%B5_%D1%83%D1%87%D0%B5%D0%BD%D0%B8%D0%B5.webm) — esercitazione dell'aviazione navale e dei reparti antisabotaggio della Flotta del Pacifico, gennaio 2025 | CC BY 4.0 (Mil.ru) — **«License review needed»** su Commons | A-MIL | 240p: VP9 + **Opus stereo 48 kHz** (banda ~405 kb/s); originale: Opus 48 kHz stereo | 8.770.551 (orig. 57.674.623) | 173,0 s | **Alta per il tipo** (categoria «Kamov Ka-27PS»), **incerta per la licenza** (fonte `new.mil.ru`, sito nuovo: la sua dicitura CC BY non l'ho potuta verificare, il server rifiuta la connessione) e per il contenuto (spari di AK-74 e PK nelle categorie, forse musica). Forse l'unico Ka-27 sull'acqua: utile anche per `hover_spruzzi` se c'è un calo con fune. |

Riserva: [Ка-50 — танец вертолета.ogv](https://commons.wikimedia.org/wiki/File:%D0%9A%D0%B0-50_%E2%80%94_%D1%82%D0%B0%D0%BD%D0%B5%D1%86_%D0%B2%D0%B5%D1%80%D1%82%D0%BE%D0%BB%D0%B5%D1%82%D0%B0.ogv)
(CC BY 4.0, Mil.ru 2012; esibizione acrobatica del Ka-50, coassiale; 240p 11.794.582 B, orig. 57.751.555 B; 431 s; da
ascoltare, probabile musica). URL in fondo (R5). Esclusi di proposito i video Mil.ru di Ka-52 in Ucraina (2022): stessa
licenza, ma girati di guerra.

## 9. `hover_spruzzi` — elicottero in hover sull'acqua

| # | File | Licenza | Attr. | Formato (audio) | Byte | Durata | Affidabilità |
|---|---|---|---|---|---|---|---|
| U14 | [U S Navy search and rescue operations for missing F A 18 pilot.webm](https://commons.wikimedia.org/wiki/File:U_S_Navy_search_and_rescue_operations_for_missing_F_A_18_pilot.webm) — MH-60S in ricerca sul Pacifico occidentale, 13/9/2014 | CC BY 3.0 su YouTube (`8lTLSPSGxYY`, canale U.S. Navy); di fatto PD-§105 («U.S. Navy video/Released») | A-YT o A-US | WebM + **Vorbis 44,1 kHz stereo ~112 kb/s** (originale) | 10.313.240 | 50,2 s | **Media**: MH-60S (famiglia SH-60) basso sull'oceano; non so se ripreso da una nave o da un altro velivolo né se c'è hover con spruzzi visibili: **da ascoltare e guardare**. Unico candidato «elicottero sul mare» con licenza adatta e dimensione ragionevole. |

Riserva: [Helicopter over a lake.ogg](https://commons.wikimedia.org/wiki/File:Helicopter_over_a_lake.ogg)
(PD-autore, pdsounds.org n. 671, «ezwa»; MP3 d'origine mono 44,1 kHz 128 kb/s ricodificato Ogg; 351.628 B; 28,0 s;
elicottero generico che sorvola un lago, registrato da una barca: acqua e rotore, niente hover). URL in fondo (R6).

---

## Totale da scaricare

**14 file (U1-U14), 79.152.253 byte (≈ 79,2 MB, 75,5 MiB)**, con le transcodifiche 240p per U3, U7, U8, U10, U12, U13
e gli originali per gli altri. Durata complessiva ≈ 25 min di girato (1497 s), di cui forse un terzo utile.

- Volendo restare più larghi sotto gli 80 MB si può togliere U10 (15,3 MB) e tenere solo U11 per il Sea King, oppure
  togliere U6 (seconda scelta Tu-95).
- Volendo gli originali di U7 (A-10, Vorbis 224 kb/s) il totale sale di 27,7 MB.
- Riserve R1-R6: 52.400.272 byte in tutto (R1 6,0 MB, R2 21,6 MB, R3 12,6 MB orig., R5 11,8 MB, R6 0,35 MB; R4 a
  parte, 55,4 MB).

## Note sulle licenze

1. **Video e non audio.** Le licenze coprono il file intero; estrarre e trasformare la traccia è una modifica,
   ammessa da CC BY se la si dichiara («Modificato: …»). Per PD e CC0 nessun vincolo.
2. **Mil.ru e Kremlin.ru (U5, U6, U12, U13).** Commons li accetta come CC BY 4.0 per i modelli `{{Mil.ru}}` e
   `{{Kremlin.ru}}` (lettera di autorizzazione del Cremlino archiviata su Commons). Due cautele: (a) U13 viene dal nuovo
   sito `new.mil.ru` (2025) ed è ancora in «License review needed»; non ho potuto aprire il sito per verificarne la
   dicitura; (b) se nel filmato c'è musica di terzi, quella non è coperta: usare solo tratti senza musica.
3. **YouTube CC BY (U3, U8, U14 e le riserve R1, R2).** La licenza è quella scelta da chi ha caricato, verificata su
   Commons da un revisore o da un bot (`{{LicenseReview}}` / `License reviewed by YouTubeReviewBot`). U3 e U8 sono
   spotter che filmano da sé: credibili. U14 è del canale ufficiale U.S. Navy, quindi è comunque opera federale.
4. **Opere federali USA (U1, U7, U10, U11, U14).** Pubblico dominio per §105. U1, U7 sono B-roll DVIDS ricaricati su
   Commons con l'ID DVIDS; il sito DVIDS non l'ho potuto controllare direttamente (vedi «Fonti»). U10: la Clinton
   Library chiede il credito «Courtesy; William J. Clinton Presidential Library» (cortesia, non licenza).
5. **PD dichiarato dall'autore (U4, R6) e CC0 (U2).** Registrazioni proprie dichiarate tali; nessuna prova contraria.
   U4 viene da ru.wikipedia (2008), trasferito su Commons con il modello `{{PD-user-w}}`.
6. **Scartati per licenza:** CC BY-SA — Ka-27 sull'«Admiral Vinogradov» (unico Ka-27 da vicino), MH-60R «Spinning Up»,
   MH-60S HSC-4, Mi-8MTV-1, Bell 212, «Rescue helicopter take off», Il-18 e Boeing di Beeld en Geluid (CC BY-SA 3.0),
   Tu-114 Polygoon (CC BY-SA 3.0 nl), «Propeller-plane-flying-steady», «Jet airliner overhead»; licenze governative non
   CC — JMSDF (P-3C, SH-60J/K ufficiali), Taiwan (P-3C), Marina indiana (Sea King); «Helicopter flying overhead»
   (licenza generica «Attribution», non CC, ed elicottero non identificato).
7. **Scartati per provenienza:** archive.org «GOLD TAPE» (effetti USC / Sunset Editorial, anni '50-'60: il CC0 lo ha
   messo chi li ha caricati, non il titolare; niente turboeliche, elicotteri generici); i file «FR Lockheed L188
   Electra», «FR Tupolev Tu114», «FR Iliouchine Il62» sono letture di voci di Wikipedia, non suoni.

## Lacune

- **P-3 / Electra veri:** nessuna ripresa da terra lunga con licenza libera; solo gli 8 s di U2. Il C-130T (U1) è il
  sostituto più fedele; l'Electra CC0 di Freesound (HootOwl 464893) resta escluso perché chiede un account.
- **Il-38, Be-12, Tu-142, Ka-25:** nessun file del tipo esatto. Coperti con An-12 (stessi motori), An-24 (motori
  parenti), Tu-95 (stessi motori ed eliche) e Ka-29/Ka-27PS. **Ka-25: nessun candidato proprio**; si usa lo stesso
  materiale del Ka-27 (rotore coassiale Kamov) abbassando la frequenza di pala col modello (11,9 contro 13-14 Hz).
- **S-3 Viking:** nessuna registrazione (solo un video NASA con narrazione e un time-lapse); sostituito con l'A-10.
- **Sea King:** nessuna ripresa pulita; solo girato presidenziale con voci (U10) e un incidente (U11).
- **`hover_spruzzi`:** nessuna registrazione da terra o da una barca di un elicottero in hover sull'acqua con gli
  spruzzi; U14 e U13 sono da verificare all'ascolto. In alternativa la parte «spruzzi» si può sintetizzare (rumore
  rosa filtrato 200-8000 Hz, come già fa il catalogo) e usare il campione solo per il rotore.
- **Contenuto delle tracce:** per nessun video ho potuto sapere se c'è musica, commento o spari; tutti sono marcati
  «da ascoltare». Dopo il download conviene un passaggio di scelta (spettrogramma e ascolto) prima del taglio.
- **Frequenza di pala:** le riprese da terra con camere di consumo tagliano spesso sotto 30-50 Hz (filtri del
  microfono, AGC): le righe sotto i 50 Hz (rotori 11-17 Hz, Tu-142 49 Hz) le deve dare il modello, non il campione.
- **DVIDS diretto:** non verificato (vedi sotto); su DVIDS ci sono certamente altri B-roll di P-3C, MH-60R e SH-60B in
  hover, che potrebbero colmare le lacune P-3 e hover se l'utente li cerca da un browser.

## Fonti

- Wikimedia Commons, API: `https://commons.wikimedia.org/w/api.php` (ricerche `filetype:audio|video`,
  `deepcat:"<categoria>"`; `prop=imageinfo&iiprop=size|url|extmetadata|metadata`; `prop=videoinfo&viprop=derivatives`;
  wikitesto per i modelli di licenza). Modelli: [Template:Mil.ru](https://commons.wikimedia.org/wiki/Template:Mil.ru),
  [Template:Kremlin.ru](https://commons.wikimedia.org/wiki/Template:Kremlin.ru),
  [Commons:YouTube files](https://commons.wikimedia.org/wiki/Commons:YouTube_files).
- Categorie Commons consultate: «Sounds of air transport», «Antonov An-24 sound», «Videos of Tupolev Tu-95», «Videos of
  Kamov helicopters», «Videos of Sikorsky helicopters», «Videos of UH-60 Black Hawk», «Videos of C-130 Hercules»,
  «Fat Albert (aircraft)», «WP-3 Orion», «Videos of Marine One», alberi di «Lockheed P-3 Orion», «Kamov Ka-27»,
  «Sikorsky H-60», «Sikorsky H-3 Sea King», «Lockheed S-3 Viking», «Antonov An-12».
- NASA Image and Video Library API (`https://images-api.nasa.gov/search`, `media_type=audio|video`): nessun audio
  utile (solo podcast); video P-3 solo dall'interno (SnowEx), S-3 solo time-lapse e un servizio narrato.
- archive.org `advancedsearch.php` e `metadata/<id>`: Radio Aporee (elicotteri generici, PDM o CC BY 3.0, tipo non
  identificato), «GOLD TAPE» USC (scartato, vedi sopra).
- DVIDS (`www.dvidshub.net`): la ricerca risponde con una sfida anti-bot (AWS WAF, HTTP 202 vuoto) alle richieste da
  script; non l'ho aggirata. Lo stato PD dei B-roll DVIDS usati qui è quello registrato su Commons con l'ID DVIDS.
- Openverse API: limitata per le richieste anonime; i pochi risultati non-Freesound erano già su Commons.
- Le licenze generali (CC0, PD §105, CC BY) sono quelle di `docs/ricerca/18_audio_reale.md` §1.

### URL diretti

Originali (`O`) e transcodifiche 240p (`T`); in grassetto quello da scaricare.

- U1 **O** https://upload.wikimedia.org/wikipedia/commons/9/93/Fat_Albert_low_pass.ogv
- U2 **O** https://upload.wikimedia.org/wikipedia/commons/2/2a/US_Navy_Lockheed_WP-3D_Orion_recon_aircraft.ogv
- U3 **T** https://upload.wikimedia.org/wikipedia/commons/transcoded/b/b4/Antonov_An-12_taking_off_from_Malta_International_Airport.ogv/Antonov_An-12_taking_off_from_Malta_International_Airport.ogv.240p.vp9.webm
  — O https://upload.wikimedia.org/wikipedia/commons/b/b4/Antonov_An-12_taking_off_from_Malta_International_Airport.ogv
- U4 **O** https://upload.wikimedia.org/wikipedia/commons/5/59/Sound_of_AN-24_airplane.ogg
- U5 **O** https://upload.wikimedia.org/wikipedia/commons/5/57/%D0%9F%D0%BE%D0%BB%D0%B5%D1%82%D1%8B_%D0%A2%D1%83-95%D0%9C%D0%A1_%D0%B2_%D1%83%D1%81%D0%BB%D0%BE%D0%B2%D0%B8%D1%8F%D1%85_%D0%BD%D0%B8%D0%B7%D0%BA%D0%B8%D1%85_%D1%82%D0%B5%D0%BC%D0%BF%D0%B5%D1%80%D0%B0%D1%82%D1%83%D1%80.webm
- U6 **O** https://upload.wikimedia.org/wikipedia/commons/0/09/Tupolev_Tu-95.ogv
- U7 **T** https://upload.wikimedia.org/wikipedia/commons/transcoded/c/c2/A-10%27s_perform_highway_landing_in_Estonia_-_160801-Z-QO433-001.webm/A-10%27s_perform_highway_landing_in_Estonia_-_160801-Z-QO433-001.webm.240p.vp9.webm
  — O https://upload.wikimedia.org/wikipedia/commons/c/c2/A-10%27s_perform_highway_landing_in_Estonia_-_160801-Z-QO433-001.webm
- U8 **T** https://upload.wikimedia.org/wikipedia/commons/transcoded/8/85/JMSDF_SH-60J_Sapporo_Airshow_Okadama_Airport.ogv/JMSDF_SH-60J_Sapporo_Airshow_Okadama_Airport.ogv.240p.vp9.webm
- U9 **O** https://upload.wikimedia.org/wikipedia/commons/7/74/BlackhawksLilburnGeorgia.webm
- U10 **T** https://upload.wikimedia.org/wikipedia/commons/transcoded/0/0d/President_Clinton_Departing_the_White_House_via_Marine_One_%281998%29.webm/President_Clinton_Departing_the_White_House_via_Marine_One_%281998%29.webm.240p.vp9.webm
- U11 **O** https://upload.wikimedia.org/wikipedia/commons/0/0b/Desert_Duck_744_Accident.webm
- U12 **T** https://upload.wikimedia.org/wikipedia/commons/transcoded/8/80/%D0%9F%D0%BE%D0%BB%D1%91%D1%82%D1%8B_%D0%B8_%D1%81%D1%82%D1%80%D0%B5%D0%BB%D1%8C%D0%B1%D0%B0_%D0%B2%D0%B5%D1%80%D1%82%D0%BE%D0%BB%D0%B5%D1%82%D0%BE%D0%B2_%D0%9A%D0%B0-52%D0%9A_%D0%B8_%D0%9A%D0%B0-29_%D0%B8%D0%B7_%D1%81%D0%BE%D1%81%D1%82%D0%B0%D0%B2%D0%B0_%D0%BF%D0%B0%D0%BB%D1%83%D0%B1%D0%BD%D0%BE%D0%B9_%D0%B0%D0%B2%D0%B8%D0%B0%D1%86%D0%B8%D0%B8_%D0%A2%D0%90%D0%92%D0%9A%D0%A0_%C2%AB%D0%90%D0%B4%D0%BC%D0%B8%D1%80%D0%B0%D0%BB_%D0%9A%D1%83%D0%B7%D0%BD%D0%B5%D1%86%D0%BE%D0%B2%C2%BB.ogv/%D0%9F%D0%BE%D0%BB%D1%91%D1%82%D1%8B_%D0%B8_%D1%81%D1%82%D1%80%D0%B5%D0%BB%D1%8C%D0%B1%D0%B0_%D0%B2%D0%B5%D1%80%D1%82%D0%BE%D0%BB%D0%B5%D1%82%D0%BE%D0%B2_%D0%9A%D0%B0-52%D0%9A_%D0%B8_%D0%9A%D0%B0-29_%D0%B8%D0%B7_%D1%81%D0%BE%D1%81%D1%82%D0%B0%D0%B2%D0%B0_%D0%BF%D0%B0%D0%BB%D1%83%D0%B1%D0%BD%D0%BE%D0%B9_%D0%B0%D0%B2%D0%B8%D0%B0%D1%86%D0%B8%D0%B8_%D0%A2%D0%90%D0%92%D0%9A%D0%A0_%C2%AB%D0%90%D0%B4%D0%BC%D0%B8%D1%80%D0%B0%D0%BB_%D0%9A%D1%83%D0%B7%D0%BD%D0%B5%D1%86%D0%BE%D0%B2%C2%BB.ogv.240p.vp9.webm
- U13 **T** https://upload.wikimedia.org/wikipedia/commons/transcoded/a/ae/-%D0%9D%D0%B0_%D0%9A%D0%B0%D0%BC%D1%87%D0%B0%D1%82%D0%BA%D0%B5_%D0%BB%D0%B5%D1%82%D1%87%D0%B8%D0%BA%D0%B8_%D0%BC%D0%BE%D1%80%D1%81%D0%BA%D0%BE%D0%B9_%D0%B0%D0%B2%D0%B8%D0%B0%D1%86%D0%B8%D0%B8_%D0%B8_%D0%B1%D0%BE%D0%B9%D1%86%D1%8B_%D0%BF%D1%80%D0%BE%D1%82%D0%B8%D0%B2%D0%BE%D0%B4%D0%B8%D0%B2%D0%B5%D1%80%D1%81%D0%B8%D0%BE%D0%BD%D0%BD%D0%BE%D0%B3%D0%BE_%D0%BE%D1%82%D1%80%D1%8F%D0%B4%D0%B0_%D0%A2%D0%B8%D1%85%D0%BE%D0%BE%D0%BA%D0%B5%D0%B0%D0%BD%D1%81%D0%BA%D0%BE%D0%B3%D0%BE_%D1%84%D0%BB%D0%BE%D1%82%D0%B0_%D0%BF%D1%80%D0%BE%D0%B2%D0%B5%D0%BB%D0%B8_%D0%BA%D0%BE%D0%BC%D0%BF%D0%BB%D0%B5%D0%BA%D1%81%D0%BD%D0%BE%D0%B5_%D1%83%D1%87%D0%B5%D0%BD%D0%B8%D0%B5.webm/transcode.240p.vp9.webm
  (nome breve `transcode.240p.vp9.webm`: Commons lo usa per i titoli lunghi; verificato con HEAD e Range)
- U14 **O** https://upload.wikimedia.org/wikipedia/commons/9/9c/U_S_Navy_search_and_rescue_operations_for_missing_F_A_18_pilot.webm

Riserve:

- R1 T https://upload.wikimedia.org/wikipedia/commons/transcoded/0/0a/Antonov_An-26_Taking_Off_from_Leipzig-Halle_Airport.ogv/Antonov_An-26_Taking_Off_from_Leipzig-Halle_Airport.ogv.240p.vp9.webm
- R2 T https://upload.wikimedia.org/wikipedia/commons/transcoded/f/f2/HIGH_WINDS_FullHD_Planespotting_Compilation_Ljubljana_Airport_%28Incl._Saab_2000%2C_Policia_Helicopter%29.webm/HIGH_WINDS_FullHD_Planespotting_Compilation_Ljubljana_Airport_%28Incl._Saab_2000%2C_Policia_Helicopter%29.webm.240p.vp9.webm
- R3 O https://upload.wikimedia.org/wikipedia/commons/b/b2/Blackhawk_flyover_at_MLB_Fort_Bragg_Game.webm
- R4 T https://upload.wikimedia.org/wikipedia/commons/transcoded/7/75/Compilation_of_Marine_One_Clips.webm/Compilation_of_Marine_One_Clips.webm.240p.vp9.webm
- R5 T https://upload.wikimedia.org/wikipedia/commons/transcoded/5/5c/%D0%9A%D0%B0-50_%E2%80%94_%D1%82%D0%B0%D0%BD%D0%B5%D1%86_%D0%B2%D0%B5%D1%80%D1%82%D0%BE%D0%BB%D0%B5%D1%82%D0%B0.ogv/%D0%9A%D0%B0-50_%E2%80%94_%D1%82%D0%B0%D0%BD%D0%B5%D1%86_%D0%B2%D0%B5%D1%80%D1%82%D0%BE%D0%BB%D0%B5%D1%82%D0%B0.ogv.240p.vp9.webm
- R6 O https://upload.wikimedia.org/wikipedia/commons/c/c6/Helicopter_over_a_lake.ogg

---

## Esito (25/10/2026)

L'utente ha scelto tutti i 14 file. Scaricati tutti col User-Agent descrittivo e le pause che Wikimedia chiede (risposte
429). Dopo l'analisi e lo spettrogramma (`tools/campioni/spettri/`), quattro sono esclusi nel manifesto
(`tools/pipeline/campioni.py`, campo `escluso`):
- U6 (Tu-95 alla parata): telecronaca su tutto il tratto;
- U7 (A-10 in Estonia): voci su tutto il tratto;
- U12 (Ka-29 e Ka-52K): musica;
- U14 (MH-60S): saturato, il 20% dei campioni a fondo scala.

Gli altri 10 sono in gioco. U13 (licenza da verificare) e U11 (l'avvicinamento dell'UH-3H) sono segnati nei crediti.

**Verifica delle licenze (25/10/2026, sera).** U13 è escluso: la pagina di Commons
(<https://commons.wikimedia.org/wiki/File:-На_Камчатке_летчики_морской_авиации_и_бойцы_противодиверсионного_отряда_Тихоокеанского_флота_провели_комплексное_учение.webm>)
porta {{Mil.ru}} (CC BY 4.0, ticket VRT 2024021510008013) ma è in «License review needed», e la fonte
<https://new.mil.ru/news/4089362c-64ee-429c-b8e0-8fe2a03a9368> non risponde da qui, quindi la licenza sulla pagina
d'origine non si vede. Restano 9 file; Ka-27 e Ka-25 tornano al modello.

