"""Sorgenti dell'ASW aereo (docs/13_asw_aereo.md, pezzo A2) per [transitori.*] di data/motore/acustica.toml: toni di
aerei ed elicotteri (passano in acqua solo nella finestra di Snell, nucleo/acustica_asw.hpp), spruzzi dell'hover,
tonfi, ping di boe e sonar a immersione, esplosive e SUS. Dati dal dossier docs/ricerca/17_asw_aereo.md §4 e §8.3;
i livelli sono quasi tutti STIMA del dossier o del gioco, scritte nella confidenza. Schema di SorgenteLibreria
(nucleo/acustica.hpp), con `vita_s`: la durata del transitorio dal suo istante 0 (0 = finche' la missione lo tiene).
Gli aerei: livello in acqua a 1 m sotto il punto della superficie, con l'aereo a 91 m (300 ft); la quota lo cambia
di -20 log(h / 91). La importa tools/pipeline/acustica.py."""
from sorgenti_libreria import banda, impulso, tonale, famiglia, periodico

SINGOLO = dict(processo='singolo')
T = {}


def voce(id, nome, breve, sotto, livello, componenti, ritmo=None, vita=0.0, profondita=(0, 0), indizi=(), aurale='',
         fonte='', confidenza='STIMA', categoria='asw'):
    T[id] = dict(nome=nome, nome_breve=breve, categoria=categoria, sottocategoria=sotto, densita=0.0,
                 profondita_m=list(profondita), livello_db=livello, componenti=componenti,
                 ritmo=ritmo or dict(processo='continuo'), vita_s=vita, indizi=list(indizi), aurale=aurale,
                 fonte=fonte, confidenza=confidenza)


# --- aerei ed elicotteri (§8.3: toni; livelli STIMA del dossier: P-3 a 300 ft su un battello a 50-150 m, 110-125 dB
# re 1 uPa in banda 50-150 Hz, cioe' circa 158 dB a 1 m sotto la superficie) ---
voce('aereo_p3c', 'P-3C Orion in sorvolo', 'P-3', 'aereo', 158.0, [famiglia(68, 14, 1.5), banda(50, 1000, rel=-8)],
     indizi=['68 Hz (4 pale x 17 Hz) e armoniche fino a ~1 kHz', 'pochi secondi, Doppler +-5 Hz'],
     aurale='rombo grave e breve', fonte='17 §8.3 [AT-FERG19] [FMARS22]; livello STIMA', confidenza='alta per i toni')
voce('aereo_il38', 'Il-38 in sorvolo', 'Il-38', 'aereo', 158.0, [famiglia(70, 14, 1.5), banda(50, 1000, rel=-8)],
     indizi=['~70 Hz e armoniche'], fonte='17 §8.3 [CALC]; livello STIMA', confidenza='media')
voce('aereo_be12', 'Be-12 in sorvolo', 'Be-12', 'aereo', 155.0, [famiglia(70, 12, 1.5), banda(50, 1000, rel=-8)],
     indizi=['~70 Hz e armoniche'], fonte='17 §8.3 [CALC]; livello STIMA', confidenza='media')
voce('aereo_tu142', 'Tu-142 in sorvolo', 'Tu-142', 'aereo', 162.0,
     [famiglia(49, 20, 1.5), tonale(98, rel=-2), banda(30, 1000, rel=-8)],
     indizi=['~49 e 98 Hz (eliche controrotanti)'], fonte='17 §8.3 [CALC]; livello STIMA', confidenza='media')
voce('aereo_s3', 'S-3 Viking in sorvolo', 'S-3', 'aereo', 145.0, [banda(50, 2000, picco=200, pendenza=3)],
     indizi=['turbofan: nessun tonale d\'elica, soffio largo'], fonte='17 §8.3 [CALC]; livello STIMA',
     confidenza='media')
voce('elicottero_sh60', 'SH-60 in volo', 'SH-60', 'elicottero', 150.0, [famiglia(17, 8, 2.0), tonale(80, rel=-3)],
     indizi=['~17 Hz del rotore principale', '80 Hz del rotore di coda'], fonte='17 §8.3; giri e livello STIMA')
voce('elicottero_sh2', 'SH-2F Seasprite in volo', 'SH-2', 'elicottero', 148.0, [famiglia(20, 8, 2.0), tonale(95, rel=-3)],
     indizi=['~20 Hz del rotore principale a 4 pale', 'circa 95 Hz del rotore di coda'],
     fonte='17 §8.3 come l\'SH-60; giri e livello STIMA (la Knox, 30/10/2026)')
voce('elicottero_sh3', 'SH-3 Sea King in volo', 'SH-3', 'elicottero', 150.0, [famiglia(17, 8, 2.0)],
     indizi=['~17 Hz del rotore principale'], fonte='17 §8.3; giri e livello STIMA')
voce('elicottero_ka25', 'Ka-25 in volo', 'Ka-25', 'elicottero', 150.0, [famiglia(11.9, 10, 2.0)],
     indizi=['11,9 Hz, rotori coassiali'], fonte='17 §8.3; livello STIMA')
voce('elicottero_ka27', 'Ka-27 in volo', 'Ka-27', 'elicottero', 152.0, [famiglia(13.0, 10, 2.0), famiglia(14.0, 10, 2.0, rel=-1)],
     indizi=['~13-14 Hz con i battimenti dei rotori coassiali'], fonte='17 §8.3; giri e livello STIMA')
voce('hover_spruzzi', 'Elicottero in hover: rotore e spruzzi', 'Hover', 'elicottero', 136.0,
     [banda(200, 8000, picco=800, pendenza=3)],
     indizi=['rumore di superficie localizzato entro 0,5-1 nm'], aurale='fruscio continuo dalla superficie',
     fonte='17 §8.3 (regola d\'ascolto [GIOCO]); livello STIMA: la sfera del 688 lo sente entro ~1 nm')

# --- tonfi (§8.3: boa A-size 155-165 dB impulsivo in 0,5-10 kHz; Mk 46 170-180 dB) ---
voce('tonfo_boa', 'Tonfo di una sonoboa', 'Tonfo boa', 'tonfo', 160.0, [impulso(500, 10000, 0.2, picco=2000, pendenza=3)],
     SINGOLO, vita=5.0, indizi=['colpo secco in superficie', 'a intervalli regolari: una barriera di boe'],
     fonte='17 §8.3 [ART-Be12] (2,8 km ai sonar sovietici) e [GIOCO]', confidenza='media')
voce('tonfo_trasduttore', 'Tonfo del trasduttore del sonar a immersione', 'Tonfo dip', 'tonfo', 150.0,
     [impulso(300, 5000, 0.3, picco=1000, pendenza=3)], SINGOLO, vita=5.0,
     indizi=['tonfo, poi 1-3 min di silenzio e i ping'], fonte='17 §8.3; livello STIMA')
voce('tonfo_siluro', 'Tonfo di un siluro aviolanciato', 'Tonfo siluro', 'tonfo', 175.0,
     [impulso(200, 10000, 0.2, picco=1500, pendenza=3)], SINGOLO, vita=5.0,
     indizi=['tonfo forte, dopo 5-10 s il motore e la testa: siluro in acqua'], fonte='17 §8.3 [GIOCO]',
     confidenza='media')

# --- ping (§4.1-4.2, §8.3) ---
voce('ping_aqs13b', 'Ping del sonar a immersione AQS-13B', 'AQS-13B', 'ping', 211.0, [impulso(9900, 10100, 0.035)],
     SINGOLO, vita=2.0, indizi=['~10 kHz, ogni 6-10 s, 3-6 ping poi silenzio'], fonte='17 §6.4 e §8.3 [MILAQS13B]',
     confidenza='alta')
voce('ping_aqs13f', 'Ping del sonar a immersione AQS-13F', 'AQS-13F', 'ping', 215.0, [impulso(9900, 10100, 0.035)],
     SINGOLO, vita=2.0, indizi=['~10 kHz, ogni 6-10 s'], fonte='17 §8.3 (215 dB nella tabella dell\'asimmetria)',
     confidenza='media')
voce('ping_vgs3', 'Ping del sonar a immersione VGS-3', 'VGS-3', 'ping', 205.0, [impulso(8400, 8600, 0.1)], SINGOLO,
     vita=2.0, indizi=['raro: un contatto o un sospetto'], fonte='17 §6.5 e §8.3; frequenza e livello STIMA')
voce('ping_vgs2', 'Ping del sonar a immersione VGS-2', 'VGS-2', 'ping', 200.0, [impulso(7900, 8100, 0.1)], SINGOLO,
     vita=2.0, indizi=['Ka-25: raro'], fonte='17 §8.3; frequenza e livello STIMA')
voce('ping_dicass', 'Ping di una boa DICASS (SSQ-62B)', 'DICASS', 'ping', 200.0, [impulso(8400, 8600, 0.25)], SINGOLO,
     vita=2.0, indizi=['6,5-9,5 kHz, CW o FM, a comando'], fonte='17 §4.1 e §8.3 [FASSSQ62]; durata STIMA',
     confidenza='alta')
voce('ping_ssq47b', 'Ping di una boa SSQ-47B', 'SSQ-47B', 'ping', 195.0, [impulso(15900, 16100, 0.1)],
     periodico(10, 10), vita=1800.0, indizi=['13-19 kHz ogni 10 s: inconfondibile'],
     fonte='17 §4.1 e §8.3 [HOLLER]; livello e durata STIMA', confidenza='alta per la cadenza')
voce('ping_rgb55a', 'Ping di una boa RGB-55A', 'RGB-55A', 'ping', 200.0, [impulso(8400, 8600, 0.1)], SINGOLO,
     vita=2.0, indizi=['4 frequenze, a comando'], fonte='17 §4.2 [ART-Tu142]; frequenza e livello STIMA')

# --- esplosive e segnali (§4.2-4.3) ---
voce('esplosione_mgab', 'Esplosiva MGAB', 'MGAB', 'esplosiva', 215.0, [impulso(50, 5000, 0.05, picco=200, pendenza=3)],
     SINGOLO, vita=3.0, indizi=['colpo singolo a 25, 150 o 400 m', '2-3 nello stesso punto a 3-4 min'],
     fonte='17 §4.2 [ART-Tu142]; livello STIMA (100-800 g)', confidenza='media')
voce('treno_mgab', 'Spirale MGAB SZ: treno di colpi', 'MGAB SZ', 'esplosiva', 210.0,
     [impulso(50, 5000, 0.05, picco=200, pendenza=3)], periodico(0.25, 0.25), vita=8.0,
     indizi=['treno di colpi a 4 Hz'], fonte='17 §4.2 [ART-Tu142]; livello e durata STIMA', confidenza='media')
voce('sus_mk84', 'SUS Mk 84', 'SUS', 'segnale', 170.0, [tonale(3000, durata=80.0), tonale(3400, rel=-3, durata=80.0)],
     SINGOLO, vita=80.0, indizi=['toni a ~3 kHz per 45-120 s: attacco simulato o codice d\'ordine'],
     fonte='17 §4.3 [PRC68SUS]; livello STIMA', confidenza='media')


# --- meteo (M5, docs/15_meteo_e_mare.md; dossier 21 §6.2 e note 21_note/e_audio_meteo.md §1 e §5.2) ---
# Un colpo di fulmine che tocca il mare: un Transitorio per colpo (nucleo/audio_meteo.hpp, transitori_lampo), ~60 ms
# l'uno dall'altro. Livello tarato sulle osservazioni, non sui 260 dB di picco di Hill 1985 (30 us, n.v.): con la
# perdita del modello (sorgente a 1 m, il dipolo con la superficie compreso) l'impulso sta ~15 dB sopra il fondo di
# mare 3 a 10 km in 50-300 Hz ([INPEX09], Dubrovsky e Kosterin 1993) e ~+23 a 1 km. Colpo 40-400 Hz con picco a
# 150 Hz e 50 ms, coda di multipercorso 250 ms a -12 dB, schiocco 0,5-5 kHz di 8 ms che conta solo da vicino (STIMA)
voce('fulmine_mare', 'Fulmine che colpisce il mare', 'Fulmine', 'fulmine', 195.0,
     [impulso(40, 400, 0.05, picco=150, pendenza=3), impulso(40, 300, 0.25, rel=-12, picco=120, pendenza=3),
      impulso(500, 5000, 0.008, rel=-18, picco=1500, pendenza=3)],
     SINGOLO, vita=3.0, profondita=(0, 1),
     indizi=['colpo secco e grave da un rilevamento qualsiasi, spesso 3-5 colpi in mezzo secondo',
             'nessuna traccia: dura troppo poco; con un temporale vicino, altri colpi da direzioni diverse'],
     aurale='tonfo cupo e secco, a volte a raffica', categoria='meteo',
     fonte='21e §1 e §5.2: [ABA84] (udibili a 30-40 km), [INPEX09] (+15 dB a 5-10 km), [HIL85] via [DOL09]; '
           'livello e forma STIMA tarate sul modello', confidenza='media per l\'esistenza, STIMA per i livelli')


# --- armi, lancio e scafi (R7; dossier 18 §8 e note 18_note/transitori_armi_r7.md, citate «r7») ---
# I livelli sono quasi tutti STIMA di r7 §6.2 (nessun transitorio di lancio misurato e pubblicato); le sequenze
# (lancio, esplosione con le bolle, implosione coi cedimenti) le compone nucleo/transitori_armi.hpp.
from sorgenti_libreria import sweep, poisson

ARMA = dict(categoria='arma')
SCAFO = dict(categoria='scafo')
# lancio dal tubo (r7 §1: il 688 lancia col pistone ad acqua, RPES; Yang25 per il getto)
voce('tubo_allaga', 'Allagamento del tubo lanciasiluri', 'Allaga tubo', 'lancio', 128.0,
     [banda(100, 3000, picco=400, pendenza=6), banda(200, 800, rel=-6)], SINGOLO, vita=25.0,
     indizi=['gorgoglio di 20-30 s: un tubo si prepara'], fonte='r7 §1.2 e §6.2; livello STIMA', **ARMA)
voce('tubo_equalizza', 'Equalizzazione del tubo', 'Equalizza', 'lancio', 125.0,
     [banda(500, 5000, picco=2000, pendenza=3), impulso(100, 2000, 0.08, rel=6, picco=500, pendenza=3)], SINGOLO, vita=10.0,
     indizi=['sibilo, poi un colpo secco'], fonte='r7 §6.2; STIMA', **ARMA)
voce('porta_tubo', 'Porta esterna del tubo', 'Porta tubo', 'lancio', 145.0,
     [impulso(50, 4000, 0.15, picco=500, pendenza=3), banda(120, 700, rel=-20)], SINGOLO, vita=5.0,
     indizi=['colpo metallico e ronzio idraulico: la porta si apre o si chiude'], fonte='r7 §6.2; STIMA', **ARMA)
voce('sparo_ram', 'Lancio: colpo del pistone ad acqua', 'Lancio', 'lancio', 172.0,
     [impulso(20, 5000, 0.08, picco=250, pendenza=3), tonale(55, rel=-6, durata=1.2), banda(20, 5000, rel=-15)],
     SINGOLO, vita=1.5, indizi=['colpo forte e grave con la risonanza del tubo (~55 Hz): lancio'],
     fonte='r7 §1.2 e §6.2 (risonanza c/4L con L ~7 m); STIMA, udibile a 10-40 km', **ARMA)
voce('ram_ritorno', 'Lancio: ritorno del pistone', 'Ritorno ram', 'lancio', 155.0,
     [impulso(20, 5000, 0.5, picco=250, pendenza=3)], SINGOLO, vita=1.0, fonte='r7 §6.2; STIMA', **ARMA)
voce('avvio_siluro', 'Avvio del motore del siluro', 'Avvio siluro', 'lancio', 155.0,
     [impulso(1000, 10000, 0.03, picco=3000, pendenza=3), banda(1000, 10000, rel=-10)], SINGOLO, vita=2.0,
     indizi=['scoppio breve e acuto, poi il fischio che sale: siluro in acqua'], fonte='r7 §2.5 e §6.2; STIMA', **ARMA)
voce('capsula_harpoon', 'Capsula dell\'Harpoon: apertura dei tappi', 'Tappi capsula', 'lancio', 178.0,
     [impulso(20, 10000, 0.05, picco=500, pendenza=3)], SINGOLO, vita=1.0,
     indizi=['schiocco pirotecnico in superficie, 20-60 s dopo il lancio'], fonte='r7 §1.3; STIMA', **ARMA)
voce('booster_sub', 'Booster del Tomahawk acceso in acqua', 'Booster', 'lancio', 200.0,
     [banda(20, 10000), famiglia(10, 3, decad=6, rel=-10)], SINGOLO, vita=2.5,
     indizi=['ruggito a larga banda di 2 s: missile da sottomarino'], fonte='r7 §1.3: 195-210 dB molto incerti (STIMA)', **ARMA)
voce('vls_portello', 'Portello di un pozzo VLS', 'Portello VLS', 'lancio', 145.0,
     [impulso(50, 4000, 0.15, picco=500, pendenza=3)], SINGOLO, vita=3.0, fonte='r7 §6.2; STIMA', **ARMA)
# teste attive dei siluri (r7 §2.4: frequenze n.v., bande di progetto 20-80 kHz; cadenza in transitori_armi.hpp)
for sigla, nome, hz, sl, durata in (('mk48', 'Mk 48', 30000, 210.0, 0.01), ('mk46', 'Mk 46', 35000, 205.0, 0.01),
                                     ('uset80', 'USET-80', 25000, 210.0, 0.01), ('set65', 'SET-65 / TEST-71', 25000, 205.0, 0.01),
                                     ('at1', 'AT-1', 30000, 200.0, 0.01)):
    voce(f'ping_testa_{sigla}', f'Ping della testa attiva del {nome}', f'Testa {nome}', 'ping', sl,
         [impulso(hz - 500, hz + 500, durata)], SINGOLO, vita=0.5,
         indizi=['ping acuto che accelera man mano che il siluro si avvicina'], fonte='r7 §2.4 e §6.3; STIMA', **ARMA)
voce('ping_testa_adcap', 'Ping della testa attiva del Mk 48 ADCAP (FM)', 'Testa ADCAP', 'ping', 210.0,
     [sweep(20000, 40000, 0.02)], SINGOLO, vita=0.5, indizi=['ping a larga banda FM'], fonte='r7 §6.3; STIMA', **ARMA)
# esplosioni: l'urto di 1 kg di TNT (Cole: 274 dB di picco a 1 m; qui l'rms di 10 ms tarato su ~200 dB di picco a
# 10 km per 250 kg, r7 §3.1), e una pulsazione della bolla; il peso e la quota li mette transitori_esplosione
voce('esplosione_urto', 'Esplosione subacquea: onda d\'urto (1 kg di TNT)', 'Esplosione', 'esplosione', 256.0,
     [impulso(20, 20000, 0.01, picco=2800, pendenza=6)], SINGOLO, vita=3.0,
     indizi=['colpo violento, poi le pulsazioni della bolla'], fonte='r7 §3.1 (Cole, Willis, dossier 07 §1); taratura STIMA', **ARMA)
voce('esplosione_bolla', 'Esplosione subacquea: pulsazione della bolla', 'Bolla', 'esplosione', 242.0,
     [impulso(2, 500, 0.02, picco=20, pendenza=6)], SINGOLO, vita=1.0, fonte='r7 §3.2: la prima bolla ~14 dB sotto l\'urto', **ARMA)
# il proprio battello e gli altri (r7 §5: livelli n.v., stime di progetto)
voce('portello_sbatte', 'Portello che sbatte', 'Portello', 'scafo', 142.0,
     [impulso(50, 5000, 0.2, picco=600, pendenza=3), tonale(700, rel=-6, durata=0.2)], SINGOLO, vita=0.5,
     indizi=['colpo metallico isolato: indisciplina a bordo'], fonte='r7 §5 e §6.2; STIMA', **SCAFO)
voce('attrezzo_cade', 'Attrezzo che cade', 'Attrezzo', 'scafo', 138.0,
     [impulso(200, 8000, 0.2, picco=2000, pendenza=3), tonale(2500, rel=-6, durata=0.2)], SINGOLO, vita=0.5,
     fonte='r7 §6.2; STIMA', **SCAFO)
voce('piani_colpo', 'Colpo dei piani di profondita\'', 'Piani', 'scafo', 128.0,
     [impulso(50, 2000, 0.05, picco=300, pendenza=3), banda(150, 400, rel=-6)], SINGOLO, vita=1.5,
     fonte='r7 §6.2; STIMA', **SCAFO)
voce('pompa_avvio', 'Avvio di una pompa d\'assetto o d\'esaurimento', 'Pompa', 'scafo', 135.0,
     [banda(100, 2000, picco=300, pendenza=3), famiglia(60, 4, decad=3, rel=-3)], SINGOLO, vita=3.0,
     fonte='r7 §6.2; STIMA', **SCAFO)
voce('tdu', 'Espulsione dei rifiuti (TDU)', 'TDU', 'scafo', 148.0,
     [impulso(30, 4000, 0.05, picco=200, pendenza=3), banda(100, 800, rel=-10)], SINGOLO, vita=1.5,
     indizi=['colpo d\'acqua e bolle, spesso al cambio di guardia'], fonte='r7 §6.2; STIMA', **SCAFO)
voce('soffiata_emergenza', 'Soffiata d\'emergenza delle casse', 'Soffiata', 'scafo', 175.0,
     [banda(20, 5000, picco=200, pendenza=3), banda(20, 100, rel=-3)], SINGOLO, vita=30.0,
     indizi=['ruggito di 30 s con bolle grosse: risalita d\'emergenza'], fonte='r7 §4.1 (Thresher a ~30 nmi) e §6.2; STIMA', **SCAFO)
voce('scafo_scricchiola', 'Scafo che scricchiola al cambio di quota', 'Scricchiolii', 'scafo', 128.0,
     [impulso(1000, 5000, 0.05, picco=2500, pendenza=3)], periodico(0.5, 2.0), vita=15.0,
     fonte='r7 §6.2; STIMA', **SCAFO)
# affondamento (r7 §4)
voce('cedimento_minore', 'Cedimento di un compartimento in discesa', 'Cedimento', 'affondamento', 185.0,
     [impulso(10, 2000, 0.3, picco=100, pendenza=3)], SINGOLO, vita=1.0,
     indizi=['schianti cupi uno dopo l\'altro, sempre piu\' profondi'], fonte='r7 §4.1-4.2 (Scorpion: 17 eventi in 200 s); STIMA', **SCAFO)
voce('rottura_nave', 'Nave che si spezza e affonda', 'Rottura', 'affondamento', 160.0,
     [impulso(50, 3000, 0.5, picco=300, pendenza=3)], poisson(360.0), vita=450.0,
     indizi=['strappi, colpi e lamiere per 5-10 minuti'], fonte='r7 §4.3 (MOL Comfort, CTBTO); livelli STIMA', **SCAFO)
voce('allagamento', 'Allagamento di uno scafo', 'Allagamento', 'affondamento', 150.0,
     [banda(50, 5000, picco=300, pendenza=3)] + [tonale(hz, rel=-6 - k) for k, hz in enumerate((47, 83, 131, 177, 233, 311, 389, 457))],
     SINGOLO, vita=1800.0, indizi=['fruscio con molte righe strette: risonanze dello scafo che si riempie'],
     fonte='r7 §4.3 ([Bry18]: oltre 100 righe nel LOFAR); frequenze e livelli STIMA', **SCAFO)
voce('bolle_aria', 'Aria che sfugge dallo scafo', 'Bolle', 'affondamento', 140.0,
     [impulso(30, 700, 0.005, picco=200, pendenza=3)], poisson(36000.0), vita=60.0,
     fonte='r7 §6.2 (bolle di Minnaert); STIMA', **SCAFO)
voce('urto_fondo', 'Scafo che urta il fondo', 'Urto sul fondo', 'affondamento', 190.0,
     [impulso(5, 200, 0.05, picco=40, pendenza=3), banda(5, 200, rel=-12)], SINGOLO, vita=20.0,
     fonte='r7 §6.2; STIMA', **SCAFO)


# --- artiglieria contro il sottomarino (G5; docs/16_artiglieria.md, dossier 22 §7, §9.7 e §10, citato «22») ---
# Riferimenti: lo scoppio di un 5" o di un 130 mm (3,5 kg di TNT) a 0,5 m, che un battello a 18 m sente a 177 dB di
# picco a 1 km (22 §7.1, Cole con lo specchio di superficie): con la perdita del modello (sorgente a 0,5 m) e 6 dB fra
# picco e rms dei 5 ms, 244 dB. Il taglio dello specchio lo fa la perdita del modello; con mare 3 la superficie ruvida
# lo attenua oltre i 3 km (lo scoppio arriva piu' forte della formula a specchio liscio). Il calibro e la carica li
# mette nucleo/acustica_artiglieria.hpp col guadagno.
voce('art_scoppio_superficie', 'Proietto che scoppia sull\'acqua (5", 3,5 kg)', 'Cannonata', 'artiglieria', 244.0,
     [impulso(300, 20000, 0.005, picco=3000, pendenza=3)], SINGOLO, vita=1.0,
     indizi=['crack corto e secco, senza bolla: esplosione in superficie',
             'a cadenza regolare: tiro d\'artiglieria (0,75 s Mk 75, 3 s Mk 45, doppiette AK-130 e AK-726)'],
     aurale='schiocco secco e acuto', fonte='22 §7.1 e §10 (Cole, specchio di Lloyd); taratura sul modello [CALC]',
     confidenza='media-bassa', **ARMA)
voce('art_scoppio_aria', 'Proietto con spoletta di prossimita\' che scoppia in aria', 'Scoppio VT', 'artiglieria', 224.0,
     [impulso(300, 20000, 0.005, picco=2000, pendenza=3)], SINGOLO, vita=1.0,
     indizi=['come la cannonata, piu\' debole: il suono entra in acqua solo sotto lo scoppio (finestra di Snell)'],
     fonte='22 §3.5 e §7.1; 20 dB sotto lo scoppio sull\'acqua (STIMA)', confidenza='STIMA', **ARMA)
voce('art_partenza', 'Colpo di partenza di un cannone navale', 'Partenza', 'artiglieria', 180.0,
     [impulso(40, 150, 0.07, picco=70, pendenza=3)], SINGOLO, vita=1.0,
     indizi=['tonfo sordo e ritmico alla cadenza del pezzo: la nave sta sparando', 'arriva prima del proietto'],
     aurale='tonfo cupo, ritmico', fonte='22 §7.3: 165-180 dB a 1 m equivalente, picco a ~70 Hz [NOSC732]; si tiene il '
     'massimo perche\' con la perdita del modello (sorgente a 3 m) si senta entro 5-10 km come nelle fonti della US Navy',
     confidenza='STIMA', **ARMA)
voce('art_tonfo', 'Proietto inerte o rimbalzato che entra in acqua', 'Tonfo proietto', 'artiglieria', 204.0,
     [impulso(100, 5000, 0.02, picco=800, pendenza=3)], SINGOLO, vita=1.0,
     indizi=['tonfo breve senza esplosione: colpo d\'esercitazione o rimbalzo'],
     fonte='22 §7.2: 30-50 dB sotto lo scoppio dello stesso calibro (STIMA; EIS non scaricabili)', confidenza='STIMA', **ARMA)
voce('art_raffica', 'Raffica di un cannone a canne rotanti (AK-630): mezzo secondo', 'Raffica', 'artiglieria', 240.0,
     [impulso(300, 20000, 0.004, picco=3000, pendenza=3)], periodico(0.0133, 0.0133), vita=0.5,
     indizi=['ronzio di micro-scoppi per 1-3 s: AK-630'],
     fonte='22 §7.1 (30 mm, 48,5 g: 5 dB sotto il 5"); 75 colpi al secondo (4500/min); STIMA', confidenza='STIMA', **ARMA)

# --- collisioni fra unita' (dossier 16 §2.6; i danni in nucleo/collisioni.hpp, le sorgenti in nucleo/acustica_urto.hpp) ---
# Riferimenti: il colpo di 1 MJ a 170 dB re 1 uPa a 1 m (STIMA di 16 §2.6, +10 dB per decade d'energia, fra 150 e 195:
# il livello lo mette EsitoUrto col guadagno), lo stridio di 1 MJ di strisciamento in 1 s a 160 dB (STIMA).
voce('urto_colpo', 'Collisione: scafo contro scafo', 'Collisione', 'collisione', 170.0,
     [impulso(10, 3000, 0.15, picco=120, pendenza=3), banda(20, 1500, picco=150, pendenza=3, rel=-14)], SINGOLO, vita=2.0,
     indizi=['colpo metallico forte e cupo, poi lo scafo che risuona: due scafi si sono toccati',
             'dopo l\'urto entrambi sanno, chiunque fosse il piu\' silenzioso'],
     aurale='schianto sordo che rimbomba', fonte='16 §2.6 (180-190 dB per un urto forte); STIMA', **SCAFO)
voce('urto_lamiera', 'Collisione: vela, timoni o appendici', 'Collisione', 'collisione', 170.0,
     [impulso(50, 8000, 0.08, picco=800, pendenza=3), banda(200, 5000, picco=1200, pendenza=3, rel=-12)], SINGOLO, vita=2.0,
     indizi=['schianto squillante di lamiera: una vela, un timone o un albero colpiti'],
     aurale='clangore acuto', fonte='16 §2.6 e §2.3 (parti a lama); spettro STIMA', **SCAFO)
voce('urto_stridio', 'Collisione: strisciamento di uno scafo sull\'altro', 'Stridio', 'collisione', 160.0,
     [banda(200, 6000, picco=1000, pendenza=4)], SINGOLO, vita=1.0,
     indizi=['stridio metallico continuo dopo il colpo (K-407 e Grayling, 1993)'],
     aurale='stridore di lamiere', fonte='16 §2.6 [WP-Kola93]; livello e spettro STIMA', **SCAFO)
voce('urto_elica', 'Collisione: le pale di un\'elica che colpiscono', 'Elica', 'collisione', 150.0,
     [impulso(30, 3000, 0.02, picco=400, pendenza=3)], periodico(0.07, 0.15), vita=3.0,
     indizi=['colpi rapidi che rallentano: un\'elica ha colpito qualcosa'],
     fonte='16 §2.3 e §2.6; 20 dB sotto il colpo (STIMA)', **SCAFO)
