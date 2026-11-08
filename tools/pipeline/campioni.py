"""Libreria di campioni registrati (pezzo R4 del dossier docs/ricerca/18_audio_reale.md §8): manifesto, scarico e
controllo. L'elenco e le licenze vengono da docs/ricerca/18_note/campioni_r4.md (solo opere federali USA di pubblico
dominio, 17 U.S.C. §105; CC0 e CC BY ammessi dalla scelta dell'utente del 25/10/2026). I tre file con la licenza da
verificare (autore non federale o non dichiarato) sono segnati `licenza = "PD-§105?"` ed esclusi dopo la verifica del
25/10/2026 (il pubblico dominio non si dimostra: il motivo e la pagina in `escluso`).

I file grezzi vanno in tools/campioni/grezzi/ (dati non fidati: si leggono, non si eseguono) col manifesto delle
impronte in tools/campioni/manifesto.json. L'analisi e la conversione sono in campioni_analisi.py.

Uso:  campioni.py <radice> scarica     scarica quelli che mancano e controlla le dimensioni
      campioni.py <radice> controlla   rifa' le impronte e le confronta col manifesto
"""
import hashlib, json, os, sys, time, urllib.error, urllib.request

SS = 'https://storage.googleapis.com/noaa-passive-bioacoustic/sanctsound/products/sound_clips/'
PMEL = 'https://www.pmel.noaa.gov/acoustics/'
NPSA = 'https://www.nps.gov/nps-audiovideo/audiovideo/'
RUS = 'https://www.pmel.noaa.gov/rusalca/sites/default/files/atoms/audio/'

A_SS = ('NOAA Office of National Marine Sanctuaries and U.S. Navy. 2021. SanctSound Raw Passive Acoustic Data. '
        'NOAA National Centers for Environmental Information. https://doi.org/10.25921/saca-sp25')
A_PMEL = 'NOAA PMEL Acoustics Program, {pagina}'
A_NPS = 'National Park Service, Glacier Bay National Park & Preserve ({autore}), https://www.nps.gov/glba/learn/nature/soundclips.htm'
A_NMFS = ('NOAA Fisheries, Northeast Fisheries Science Center, Passive Acoustics Group, '
          'https://www.fisheries.noaa.gov/new-england-mid-atlantic/science-data/fish-and-invertebrate-acoustics')
A_RUS = 'NOAA PMEL, RUSALCA (Russian-American Long-term Census of the Arctic), https://www.pmel.noaa.gov/rusalca/sounds-arctic-ocean/'

PMEL_SISMICA = 'https://www.pmel.noaa.gov/acoustics/sounds_seismic.html'
PMEL_GHIACCIO = 'https://www.pmel.noaa.gov/acoustics/sounds_cryogenic.html'
PMEL_AMBIENTE = 'https://www.pmel.noaa.gov/acoustics/sounds_enviro.html'


def C(n, voce, url, byte, attribuzione, licenza='PD-§105', fattore=1.0, note='', escluso=''):
    """Una voce del manifesto: n dell'elenco curato, voce del catalogo, URL, dimensione attesa, credito, licenza,
    fattore d'accelerazione gia' applicato dalla fonte (PMEL), note."""
    return dict(n=n, voce=voce, url=url, file=os.path.basename(url), byte=byte, attribuzione=attribuzione,
                licenza=licenza, fattore=fattore, note=note, escluso=escluso)


ESCLUSO_RUS = ('licenza: la pagina https://www.pmel.noaa.gov/rusalca/sounds-arctic-ocean/ non dice autore ne\' '
               'licenza; gli ormeggi dello Stretto di Bering sono del gruppo di K. Stafford (APL-UW, non federale)')

MANIFESTO = [
    # biologici
    C(1, 'balenottera_comune', SS + 'sb03/sanctsound_sb03_05_sound_clips/data/SanctSound_SB03_05_finwhale_20190923T074930Z.wav', 5760046, A_SS),
    C(2, 'megattera', NPSA + 'cde82ded-68b8-4c67-b6c3-9ab190908357.mp3', 703236, A_NPS.format(autore='NPS'),
      note='humpback whale feeding call, Glacier Bay, luglio 2000'),
    C(3, 'capodoglio', SS + 'hi03/sanctsound_hi03_02_sound_clips/data/SanctSound_HI03_02_spermwhale2_20191216T115715Z.wav', 1054776, A_SS,
      licenza='PD-§105?', note='autori del pacchetto Lynker LLC e University of Hawaii: copertura del §105 da verificare',
      escluso='licenza: i metadati (' + SS + 'hi03/sanctsound_hi03_02_sound_clips/metadata/'
              'sanctsound_hi03_02_sound_clips-metadata.json) danno autori Lynker LLC e University of Hawaii, non '
              'federali; la scheda NCEI non dichiara licenza'),
    C(4, 'orca', NPSA + 'e60488c2-30fb-414b-bc9f-ee16f886a897.mp3', 681481, A_NPS.format(autore='C. Gabriele / NPS'),
      note='Resident Killer Whale Vocalizations'),
    C(5, 'delfini', SS + 'ci04/sanctsound_ci04_05_sound_clips/data/SanctSound_CI04_05_dolphins_20200921T232749Z.wav', 5760280, A_SS),
    C(6, 'foca_barbata', RUS + 'BeringStfeb2010_beardedseal1.mp3', 451396, A_RUS, licenza='PD-§105?',
      note='pagina NOAA senza autore (probabilmente APL-UW): copertura del §105 da verificare',
      escluso=ESCLUSO_RUS),
    C(7, 'merluzzo', SS + 'sb01/sanctsound_sb01_01_sound_clips/data/SanctSound_SB01_01_atlanticcod_20181113T024154Z.wav', 480200, A_SS),
    C(8, 'eglefino', 'https://www.fisheries.noaa.gov/s3/2023-04/Meae-knock-NOAA-PAGroup-01-haddock-clip.mp3', 91197, A_NMFS),
    C(9, 'coro_pesci', SS + 'gr01/sanctsound_gr01_02_sound_clips/data/SanctSound_GR01_02_fishchorus_20190902T004059Z.wav', 480228, A_SS),
    C(10, 'gamberi', SS + 'ci01/sanctsound_ci01_03_sound_clips/data/SanctSound_CI01_03_snappingshrimp_20190906T230827Z.wav', 5760236, A_SS),
    # geofisici e ghiaccio (PMEL gia' accelerati)
    C(11, 't_phase_tettonico', PMEL + 'pioneer/sounds/pio_eq_244_1159.wav', 291244, A_PMEL.format(pagina=PMEL_SISMICA), fattore=10.0),
    C(12, 't_phase_vulcanico', PMEL + 'sounds/whistle16x.wav', 827430, A_PMEL.format(pagina=PMEL_SISMICA), fattore=16.0,
      note='la pagina dice 10x, il nome del file 16x: da verificare sullo spettrogramma'),
    C(13, 'distacco_iceberg', PMEL + 'sounds/A53JD33Calving_3x.wav', 4515910, A_PMEL.format(pagina=PMEL_GHIACCIO), fattore=3.0,
      note='60 s attorno al colpo piu\' forte'),
    C(14, 'tremore_iceberg', PMEL + 'sounds/HarmonicTremor2006_215_09_20UsedOnBloopWebsite.wav', 277492,
      A_PMEL.format(pagina=PMEL_GHIACCIO), fattore=3.0),
    C(15, 'incaglio_iceberg', PMEL + 'sounds/noise97139.wav', 289628, A_PMEL.format(pagina=PMEL_GHIACCIO), fattore=16.0),
    C(16, 'fessurazione_ghiaccio', RUS + 'BeringStfeb2010_icenoise.mp3', 223399, A_RUS, licenza='PD-§105?',
      note='come la foca barbata', escluso=ESCLUSO_RUS),
    C(17, 'fusione_ghiaccio', 'https://www.nps.gov/nps-audiovideo/legacy/mp3/akr/avElement/glba-seltzer.mp3', 503505,
      A_NPS.format(autore='H. Lentfer / NPS'), note='Bergy Seltzer; idrofono o microfono in aria da verificare all\'ascolto'),
    # tessiture d'ambiente
    C(18, 'pioggia', SS + 'mb01/sanctsound_mb01_05_sound_clips/data/SanctSound_MB01_05_rain_20200816T093204Z_28dBgain.wav', 5760204, A_SS,
      note='+28 dB di guadagno digitale gia\' applicato'),
    C(19, 'vento', SS + 'ci02/sanctsound_ci02_03_sound_clips/data/SanctSound_CI02_03_wind_20191026T155434Z.wav', 2880140, A_SS),
    # navi civili
    C(20, 'mercantile', SS + 'ci05/sanctsound_ci05_03_sound_clips/data/SanctSound_CI05_03_largeship_20190925T135956Z.wav', 960224, A_SS,
      note='a large commercial ship, est di Anacapa'),
    C(21, 'mercantile', SS + 'oc02/sanctsound_oc02_01_sound_clips/data/SanctSound_OC02_01_ship_20190419T233040Z.wav', 5760232, A_SS,
      note='traffico della corsia, Neah Bay'),
    C(22, 'mercantile', NPSA + '7dd6d0fe-831f-4a11-9837-1b4f629d429b.mp3', 297794, A_NPS.format(autore='C. Gabriele / NPS'),
      note='Freighter approaching the Bartlett Cove dock'),
    # pesca, diporto e industria
    C(23, 'peschereccio_piccolo', NPSA + 'cb678cb6-5627-4544-9fbb-c8e3884f0827.mp3', 250356, A_NPS.format(autore='C. Gabriele / NPS'),
      note='Small Diesel Engine'),
    C(24, 'fuoribordo', NPSA + '39f7562b-1d7a-4fe4-a969-6be98e7d4886.mp3', 78157, A_NPS.format(autore='C. Gabriele / NPS'),
      note='Outboard Engine (60hp) at 20 Knots'),
    C(25, 'airgun', PMEL + 'env-noise/sounds/airguns_00_03_05_marCE-Airguns-10x.wav', 828364, A_PMEL.format(pagina=PMEL_AMBIENTE), fattore=10.0),
    C(26, 'ecoscandaglio_profondo', SS + 'ci05/sanctsound_ci05_03_sound_clips/data/SanctSound_CI05_03_echosounder_20191003T132915Z_36dBgain.wav',
      960332, A_SS, note='+36 dB di guadagno digitale gia\' applicato'),
    C(27, 'esplosione', SS + 'fk03/sanctsound_fk03_01_sound_clips/data/SanctSound_FK03_01_explosion_20190131T220530Z.wav', 480230, A_SS,
      note='Florida Keys, sorgente ignota'),
]


# R8: aerei ed elicotteri (elenco in docs/ricerca/18_note/campioni_r8.md, scelto dall'utente il 25/10/2026: tutti e 14).
# Quasi tutti video di Wikimedia Commons: si scarica la transcodifica 240p (VP9 + Opus) di quelli grandi e l'originale
# degli altri; se ne usa solo l'audio. Licenze: PD-§105, CC0 o PD dell'autore, CC BY 3.0 / 4.0 (attribuzione);
# «CC BY 4.0?» = licenza da verificare (U13, «License review needed» su Commons): esclusa dopo la verifica
U = 'https://upload.wikimedia.org/wikipedia/commons/'
COMMONS = 'https://commons.wikimedia.org/wiki/File:'
A_MIL = ('Ministry of Defence of the Russian Federation (mil.ru), CC BY 4.0 (https://creativecommons.org/licenses/by/4.0/), '
         'via Wikimedia Commons, {pagina}. Modificato: estratto audio, sbiancato e ricolorato.')
A_KRM = 'Presidential Press and Information Office, www.kremlin.ru, CC BY 4.0, via Wikimedia Commons, {pagina}. Modificato.'
A_YT = '"{titolo}" di {autore} (YouTube {id}), CC BY 3.0 (https://creativecommons.org/licenses/by/3.0/), via Wikimedia Commons, {pagina}. Modificato.'
A_BY4 = '"{titolo}" di {autore}, CC BY 4.0, via Wikimedia Commons, {pagina}. Modificato.'
A_USV = '{forza} video by {autore}, via DVIDS / Wikimedia Commons, {pagina}'
A_0 = '{autore}, via Wikimedia Commons, {pagina}'

MANIFESTO_R8 = [
    C(101, 'aereo_p3c', U + '9/93/Fat_Albert_low_pass.ogv', 2293021,
      A_USV.format(forza='U.S. Marine Corps', autore='SSgt Oscar L. Olive IV', pagina=COMMONS + 'Fat_Albert_low_pass.ogv'),
      note='C-130T con i T56 del P-3; passaggio basso'),
    C(102, 'aereo_p3c', U + '2/2a/US_Navy_Lockheed_WP-3D_Orion_recon_aircraft.ogv', 2098554,
      A_0.format(autore='John Veit (CC0)', pagina=COMMONS + 'US_Navy_Lockheed_WP-3D_Orion_recon_aircraft.ogv'), licenza='CC0',
      note='WP-3D Orion, 8 s'),
    C(103, 'aereo_il38', U + 'transcoded/b/b4/Antonov_An-12_taking_off_from_Malta_International_Airport.ogv/'
      'Antonov_An-12_taking_off_from_Malta_International_Airport.ogv.240p.vp9.webm', 2794898,
      A_YT.format(titolo='Antonov An-12 taking off from Malta International Airport', autore='Joluqa', id='blzV1AvMPoY',
                  pagina=COMMONS + 'Antonov_An-12_taking_off_from_Malta_International_Airport.ogv'), licenza='CC BY 3.0',
      note='An-12 con gli AI-20M dell\'Il-38'),
    C(104, 'aereo_be12', U + '5/59/Sound_of_AN-24_airplane.ogg', 491365,
      A_0.format(autore='Grachev (pubblico dominio)', pagina=COMMONS + 'Sound_of_AN-24_airplane.ogg'), licenza='PD-autore',
      note='An-24, bimotore AI-24'),
    C(105, 'aereo_tu142', U + '5/57/%D0%9F%D0%BE%D0%BB%D0%B5%D1%82%D1%8B_%D0%A2%D1%83-95%D0%9C%D0%A1_%D0%B2_%D1%83%D1%81%D0%BB%D0%BE%D0%B2%D0%B8%D1%8F%D1%85_%D0%BD%D0%B8%D0%B7%D0%BA%D0%B8%D1%85_%D1%82%D0%B5%D0%BC%D0%BF%D0%B5%D1%80%D0%B0%D1%82%D1%83%D1%80.webm',
      9465148, A_MIL.format(pagina=COMMONS + 'Полеты_Ту-95МС_в_условиях_низких_температур.webm'), licenza='CC BY 4.0',
      note='Tu-95MS a Engels (NK-12, eliche controrotanti)'),
    C(106, 'aereo_tu142', U + '0/09/Tupolev_Tu-95.ogv', 1744717, A_KRM.format(pagina=COMMONS + 'Tupolev_Tu-95.ogv'),
      licenza='CC BY 4.0', note='parata del 2008: anche i getti dell\'Il-78 e dei MiG-31',
      escluso='telecronaca su tutto il tratto (spettrogramma del 25/10/2026)'),
    C(107, 'aereo_s3', U + "transcoded/c/c2/A-10%27s_perform_highway_landing_in_Estonia_-_160801-Z-QO433-001.webm/"
      "A-10%27s_perform_highway_landing_in_Estonia_-_160801-Z-QO433-001.webm.240p.vp9.webm", 4014837,
      A_USV.format(forza='Iowa Air National Guard', autore='SSgt David Asbra',
                   pagina=COMMONS + "A-10's_perform_highway_landing_in_Estonia_-_160801-Z-QO433-001.webm"),
      note='A-10, turbofan TF34 della famiglia dell\'S-3', escluso='voci su tutto il tratto (spettrogramma del 25/10/2026)'),
    C(108, 'elicottero_sh60', U + 'transcoded/8/85/JMSDF_SH-60J_Sapporo_Airshow_Okadama_Airport.ogv/'
      'JMSDF_SH-60J_Sapporo_Airshow_Okadama_Airport.ogv.240p.vp9.webm', 8084033,
      A_YT.format(titolo='JMSDF SH-60J Sapporo Airshow Okadama Airport', autore='RUNWAY FUN Aviation & Military & plus',
                  id='zCGzVZeXj6s', pagina=COMMONS + 'JMSDF_SH-60J_Sapporo_Airshow_Okadama_Airport.ogv'), licenza='CC BY 3.0',
      note='SH-60J in arrivo'),
    C(109, 'elicottero_sh60', U + '7/74/BlackhawksLilburnGeorgia.webm', 5702745,
      A_BY4.format(titolo='BlackhawksLilburnGeorgia', autore='BeeboMan', pagina=COMMONS + 'BlackhawksLilburnGeorgia.webm'),
      licenza='CC BY 4.0', note='quattro UH-60 in passaggio'),
    C(110, 'elicottero_sh3', U + 'transcoded/0/0d/President_Clinton_Departing_the_White_House_via_Marine_One_%281998%29.webm/'
      'President_Clinton_Departing_the_White_House_via_Marine_One_%281998%29.webm.240p.vp9.webm', 15306445,
      A_USV.format(forza='White House Television', autore='Courtesy; William J. Clinton Presidential Library',
                   pagina=COMMONS + 'President_Clinton_Departing_the_White_House_via_Marine_One_(1998).webm'),
      note='Marine One 1998, probabile VH-3D; voci e folla'),
    C(111, 'elicottero_sh3', U + '0/0b/Desert_Duck_744_Accident.webm', 1330485,
      A_USV.format(forza='U.S. Navy', autore='U.S. Navy', pagina=COMMONS + 'Desert_Duck_744_Accident.webm'),
      note='UH-3H in appontaggio, filmato di un incidente: si usa solo l\'avvicinamento'),
    C(112, 'elicottero_ka27', U + 'transcoded/8/80/%D0%9F%D0%BE%D0%BB%D1%91%D1%82%D1%8B_%D0%B8_%D1%81%D1%82%D1%80%D0%B5%D0%BB%D1%8C%D0%B1%D0%B0_%D0%B2%D0%B5%D1%80%D1%82%D0%BE%D0%BB%D0%B5%D1%82%D0%BE%D0%B2_%D0%9A%D0%B0-52%D0%9A_%D0%B8_%D0%9A%D0%B0-29_%D0%B8%D0%B7_%D1%81%D0%BE%D1%81%D1%82%D0%B0%D0%B2%D0%B0_%D0%BF%D0%B0%D0%BB%D1%83%D0%B1%D0%BD%D0%BE%D0%B9_%D0%B0%D0%B2%D0%B8%D0%B0%D1%86%D0%B8%D0%B8_%D0%A2%D0%90%D0%92%D0%9A%D0%A0_%C2%AB%D0%90%D0%B4%D0%BC%D0%B8%D1%80%D0%B0%D0%BB_%D0%9A%D1%83%D0%B7%D0%BD%D0%B5%D1%86%D0%BE%D0%B2%C2%BB.ogv/%D0%9F%D0%BE%D0%BB%D1%91%D1%82%D1%8B_%D0%B8_%D1%81%D1%82%D1%80%D0%B5%D0%BB%D1%8C%D0%B1%D0%B0_%D0%B2%D0%B5%D1%80%D1%82%D0%BE%D0%BB%D0%B5%D1%82%D0%BE%D0%B2_%D0%9A%D0%B0-52%D0%9A_%D0%B8_%D0%9A%D0%B0-29_%D0%B8%D0%B7_%D1%81%D0%BE%D1%81%D1%82%D0%B0%D0%B2%D0%B0_%D0%BF%D0%B0%D0%BB%D1%83%D0%B1%D0%BD%D0%BE%D0%B9_%D0%B0%D0%B2%D0%B8%D0%B0%D1%86%D0%B8%D0%B8_%D0%A2%D0%90%D0%92%D0%9A%D0%A0_%C2%AB%D0%90%D0%B4%D0%BC%D0%B8%D1%80%D0%B0%D0%BB_%D0%9A%D1%83%D0%B7%D0%BD%D0%B5%D1%86%D0%BE%D0%B2%C2%BB.ogv.240p.vp9.webm',
      6742214, A_MIL.format(pagina=COMMONS + 'Полёты_и_стрельба_вертолетов_Ка-52К_и_Ка-29_(Адмирал_Кузнецов).ogv'),
      licenza='CC BY 4.0', note='Ka-29 e Ka-52K sulla Kuznetsov, rotori coassiali; ci sono anche spari',
      escluso='musica (accordi ritmati nello spettrogramma del 25/10/2026)'),
    C(113, 'elicottero_ka27', U + 'transcoded/a/ae/-%D0%9D%D0%B0_%D0%9A%D0%B0%D0%BC%D1%87%D0%B0%D1%82%D0%BA%D0%B5_%D0%BB%D0%B5%D1%82%D1%87%D0%B8%D0%BA%D0%B8_%D0%BC%D0%BE%D1%80%D1%81%D0%BA%D0%BE%D0%B9_%D0%B0%D0%B2%D0%B8%D0%B0%D1%86%D0%B8%D0%B8_%D0%B8_%D0%B1%D0%BE%D0%B9%D1%86%D1%8B_%D0%BF%D1%80%D0%BE%D1%82%D0%B8%D0%B2%D0%BE%D0%B4%D0%B8%D0%B2%D0%B5%D1%80%D1%81%D0%B8%D0%BE%D0%BD%D0%BD%D0%BE%D0%B3%D0%BE_%D0%BE%D1%82%D1%80%D1%8F%D0%B4%D0%B0_%D0%A2%D0%B8%D1%85%D0%BE%D0%BE%D0%BA%D0%B5%D0%B0%D0%BD%D1%81%D0%BA%D0%BE%D0%B3%D0%BE_%D1%84%D0%BB%D0%BE%D1%82%D0%B0_%D0%BF%D1%80%D0%BE%D0%B2%D0%B5%D0%BB%D0%B8_%D0%BA%D0%BE%D0%BC%D0%BF%D0%BB%D0%B5%D0%BA%D1%81%D0%BD%D0%BE%D0%B5_%D1%83%D1%87%D0%B5%D0%BD%D0%B8%D0%B5.webm/transcode.240p.vp9.webm',
      8770551, A_MIL.format(pagina=COMMONS + '-На_Камчатке_летчики_морской_авиации…комплексное_учение.webm'),
      licenza='CC BY 4.0?', note='Ka-27PS in Kamchatka; «License review needed» su Commons',
      escluso='licenza: la revisione di Commons non e\' fatta e la fonte https://new.mil.ru/news/'
              '4089362c-64ee-429c-b8e0-8fe2a03a9368 non e\' raggiungibile per vedere la CC BY 4.0'),
    C(114, 'hover_spruzzi', U + '9/9c/U_S_Navy_search_and_rescue_operations_for_missing_F_A_18_pilot.webm', 10313240,
      A_USV.format(forza='U.S. Navy', autore='U.S. Navy',
                   pagina=COMMONS + 'U_S_Navy_search_and_rescue_operations_for_missing_F_A_18_pilot.webm'),
      note='MH-60S basso sul Pacifico', escluso='saturato: 20% dei campioni a fondo scala'),
]


# M: biologici e ghiaccio del blocco 3 del sonar (elenco in docs/ricerca/29_note/registrazioni_biologiche.md, scelto
# dall'utente il 28/10/2026: nove voci). Scaricati a mano in tools/dl/29_biologici/<voce>/ (una cartella nuova per
# voce) e copiati nei grezzi; scarica() li trova gia' della dimensione giusta. Licenze: PD-§105 (SWFSC, SanctSound),
# PD-§105 presunto (la tavola sonora dell'AFSC non accredita i singoli clip), CC BY 4.0 (AWI, Polar Sounds)
AB = 'https://audioboom.com/posts/'
A_AWI = ('Registrazione: Alfred Wegener Institute, Helmholtz Centre for Polar and Marine Research (AWI), 2022, CC BY 4.0 '
         '(https://creativecommons.org/licenses/by/4.0/), dal progetto Polar Sounds (AWI, HIFMB, Cities and Memory), {pagina}. '
         'Modificato: sbiancato e ricolorato dal motore del gioco.')
A_SWFSC = 'NOAA Fisheries, Southwest Fisheries Science Center, Marine Mammal and Turtle Division, {via}'
A_AFSC = ('NOAA Fisheries, Alaska Fisheries Science Center, Marine Mammal Laboratory, '
          'https://apps-afsc.fisheries.noaa.gov/Audio/Alaska-Marine-Mammal-Acoustic-Studies/index.html')
WB_SWFSC = 'https://web.archive.org/web/20111016181104id_/http://swfsc.noaa.gov/uploadedFiles/Divisions/PRD/Sounds/'

MANIFESTO_M = [
    C(201, 'capodoglio', U + '2/29/Sperm_Whale_Normal_Clicks.ogg', 1484800,
      A_SWFSC.format(via='via Wikimedia Commons, ' + COMMONS + 'Sperm_Whale_Normal_Clicks.ogg'),
      note='click regolari d\'ecolocalizzazione (SWFSC, «Cetacean Sounds»)'),
    C(202, 'capodoglio', U + 'd/dd/Sperm_Whale_Rapid_Clicks_and_Coda.ogg', 4408011,
      A_SWFSC.format(via='via Wikimedia Commons, ' + COMMONS + 'Sperm_Whale_Rapid_Clicks_and_Coda.ogg'),
      note='intitolato «rapid clicks and coda»',
      escluso='nessuna coda riconoscibile: e\' la stessa registrazione dei «rapid clicks» (stessa impronta dei WAV '
              'SWFSC nella Wayback), raffiche irregolari di piu\' animali; un solo gruppo regolare isolato (4 click a '
              '47,9 s), non basta (tools/re/prove/m_coda/clic.py). La coda resta al modello'),
    C(203, 'foca_barbata', AB + '8232592.mp3', 1973784,
      A_AWI.format(pagina=AB + '8232592-bearded-seal-erignathus-barbatus'), licenza='CC BY 4.0',
      note='trilli discendenti sotto la banchisa artica'),
    C(204, 'fessurazione_ghiaccio', AB + '8230903.mp3', 1958840,
      A_AWI.format(pagina=AB + '8230903-rubbing-sea-ice-recording-2'), licenza='CC BY 4.0',
      note='lastroni che si sfregano e si piegano (pressione), Oceano Australe'),
    C(205, 'fessurazione_ghiaccio', AB + '8231522.mp3', 1056683,
      A_AWI.format(pagina=AB + '8231522-singing-sea-ice-recording-1'), licenza='CC BY 4.0',
      note='«singing sea ice», Oceano Australe'),
    C(206, 'tricheco', 'https://apps-afsc.fisheries.noaa.gov/Audio/Alaska-Marine-Mammal-Acoustic-Studies/assets/walrus-knocks.mp3',
      1789172, A_AFSC, licenza='PD-§105 presunto', note='colpi regolari (~4 al secondo)'),
    C(207, 'balenottera_minore_pacifica', WB_SWFSC + 'MinkWhaBoin.wav', 6633372,
      A_SWFSC.format(via='copia della Wayback Machine del 16/10/2011 (' + WB_SWFSC + 'MinkWhaBoin.wav)'),
      note='boing (Rankin e Barlow 2005); impronta SHA-1 uguale a quella della Wayback'),
    C(208, 'balenottera_minore_antartica', AB + '8227325.mp3', 4810918,
      A_AWI.format(pagina=AB + '8227325-the-antarctic-minke-whale-balaenoptera-bonaerensis'), licenza='CC BY 4.0',
      note='bioduck, Antartide',
      escluso='nessun teatro antartico nel gioco: senza una sorgente della libreria a cui legarlo'),
    C(209, 'orca', SS + 'mb02/sanctsound_mb02_04_sound_clips/data/SanctSound_MB02_04_killerwhales_20200414T081659Z.wav', 718434, A_SS,
      note='chiamate sociali a larga banda, Monterey Canyon (J. Joseph, T. Margolina, Naval Postgraduate School)'),
]

def cartelle(radice):
    base = os.path.join(radice, 'tools', 'campioni')
    return base, os.path.join(base, 'grezzi'), os.path.join(base, 'manifesto.json')


def impronta(percorso):
    h = hashlib.sha256()
    with open(percorso, 'rb') as f:
        for blocco in iter(lambda: f.read(1 << 20), b''): h.update(blocco)
    return h.hexdigest()


def scarica(radice):
    base, grezzi, manifesto = cartelle(radice)
    os.makedirs(grezzi, exist_ok=True)
    registro = json.load(open(manifesto)) if os.path.exists(manifesto) else {}
    for c in MANIFESTO + MANIFESTO_R8 + MANIFESTO_M:
        dst = os.path.join(grezzi, f"{c['n']:02d}_{c['file'][:80]}")
        if not os.path.exists(dst) or os.path.getsize(dst) != c['byte']:
            # Wikimedia chiede un User-Agent descrittivo e un ritmo moderato: una pausa fra i file e, sul 429, l'attesa
            # che chiede (Retry-After) o crescente
            richiesta = urllib.request.Request(c['url'], headers={
                'User-Agent': 'melchior688-pipeline/1.0 (remake amatoriale di 688 Attack Sub; campioni audio con licenza libera)'})
            for tentativo in range(6):
                try:
                    with urllib.request.urlopen(richiesta, timeout=120) as r, open(dst + '.parziale', 'wb') as f:
                        while True:
                            blocco = r.read(1 << 16)
                            if not blocco: break
                            f.write(blocco)
                    break
                except urllib.error.HTTPError as e:
                    if e.code != 429 or tentativo == 5: raise
                    attesa = int(e.headers.get('Retry-After') or 0) or 15 * 2 ** tentativo
                    print(f"   429: attendo {attesa} s", flush=True)
                    time.sleep(attesa)
            os.replace(dst + '.parziale', dst)
            time.sleep(3)
        n = os.path.getsize(dst)
        stato = 'ok' if n == c['byte'] else f'DIMENSIONE {n} invece di {c["byte"]}'
        registro[os.path.basename(dst)] = {**c, 'sha256': impronta(dst), 'byte_scaricati': n}
        print(f"{c['n']:2d} {c['voce']:24s} {n:9d} byte  {stato}")
    json.dump(registro, open(manifesto, 'w'), ensure_ascii=False, indent=1)
    print('totale', sum(v['byte_scaricati'] for v in registro.values()), 'byte in', len(registro), 'file')


def controlla(radice):
    _, grezzi, manifesto = cartelle(radice)
    registro = json.load(open(manifesto))
    errori = [k for k, v in registro.items() if impronta(os.path.join(grezzi, k)) != v['sha256']]
    print('impronte diverse:', errori or 'nessuna')
    return not errori


if __name__ == '__main__':
    radice, comando = sys.argv[1], sys.argv[2]
    {'scarica': scarica, 'controlla': controlla}[comando](radice)
