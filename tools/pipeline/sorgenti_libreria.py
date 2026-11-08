"""Libreria del paesaggio sonoro (Fase 3, S5c): sorgenti non navali per [sorgenti.*] di data/motore/acustica.toml.
Valori dal dossier docs/ricerca/09_sonar_sistemi_e_interfacce.md §5.3-5.6 (fonti e confidenza per voce); dove il
dossier non da' un numero il valore e' una STIMA DI GIOCO, scritta nella confidenza. Schema del §6.2, letto da
nucleo/acustica.hpp (SorgenteLibreria): livelli in dB re 1 uPa @1 m, "pp" convertiti in rms dal motore (-9 dB).
La importa tools/pipeline/acustica.py."""

ATLANTICO = ['faroe', 'celtica', 'gibilterra']
TUTTI = []   # tutti i teatri
INVERNO, ESTATE = [10, 11, 12, 1, 2, 3], [5, 6, 7, 8, 9]


def banda(f_lo, f_hi, rel=0.0, picco=0.0, pendenza=0.0):
    return dict(tipo='banda', f_lo=f_lo, f_hi=f_hi, picco_hz=picco, pendenza_db_ott=pendenza, livello_rel_db=rel)


def impulso(f_lo, f_hi, durata, rel=0.0, picco=0.0, pendenza=0.0):
    return dict(tipo='impulso', f_lo=f_lo, f_hi=f_hi, durata_s=durata, picco_hz=picco, pendenza_db_ott=pendenza,
                livello_rel_db=rel)


def sweep(f_da, f_a, durata, rel=0.0):
    return dict(tipo='sweep', f_da=f_da, f_a=f_a, durata_s=durata, livello_rel_db=rel)


def tonale(hz, rel=0.0, durata=0.0):
    return dict(tipo='tonale', hz=hz, livello_rel_db=rel, **({'durata_s': durata, 'ritmata': True} if durata else {}))


def famiglia(f1, n, decad=0.0, rel=0.0, durata=0.0):
    return dict(tipo='famiglia', hz=f1, armoniche=n, decadimento_db=decad, livello_rel_db=rel,
                **({'durata_s': durata, 'ritmata': True} if durata else {}))


def periodico(a, b, gruppi=(1, 1), interno=(0, 0)):
    return dict(processo='periodico', intervallo_s=[a, b], gruppi=list(gruppi), intervallo_interno_s=list(interno))


def treno(a, b, serie_min, pausa_s, gruppi=(1, 1), interno=(0, 0)):
    return dict(processo='treno', intervallo_s=[a, b], gruppi=list(gruppi), intervallo_interno_s=list(interno),
                serie_min=serie_min, pausa_s=pausa_s)


def poisson(tasso_h, gruppi=(1, 1), interno=(0, 0)):
    return dict(processo='poisson', tasso_h=tasso_h, gruppi=list(gruppi), intervallo_interno_s=list(interno))


S = {}


def voce(id, nome, breve, categoria, sotto, livello, componenti, ritmo=None, teatri=TUTTI, mesi=(), ore=(),
         densita=0.1, profondita=(5, 50), velocita=(0, 0), estensione=0.0, tipo_livello='rms', ts=-10.0,
         diurno=None, indizi=(), confonde=(), aurale='', fonte='', confidenza='media'):
    S[id] = dict(nome=nome, nome_breve=breve, categoria=categoria, sottocategoria=sotto, teatri=list(teatri),
                 mesi=list(mesi), ore=list(ore), densita=densita, profondita_m=list(profondita),
                 velocita_kn=list(velocita), estensione_m=estensione, livello_db=livello, livello_tipo=tipo_livello,
                 ts_db=ts, componenti=componenti, ritmo=ritmo or dict(processo='continuo'),
                 indizi=list(indizi), confonde_con=list(confonde), aurale=aurale, fonte=fonte, confidenza=confidenza,
                 **({'diurno_db': diurno[0], 'diurno_ore': diurno[1]} if diurno else {}))


# --- biologici (09 §5.3) ---
voce('balenottera_comune', 'Balenottera comune (Balaenoptera physalus)', 'Comune', 'biologico', 'misticeto', 185.0,
     [sweep(40, 15, 0.9)], treno(12, 25, 12, 135, (1, 2), (6, 12)), ATLANTICO + ['otranto'], INVERNO,
     densita=0.5, profondita=(10, 60), velocita=(1, 6), ts=10.0,
     indizi=['impulsi discendenti a ~20 Hz molto regolari', 'doppiette e terzine', 'nessuna riga d\'elica', 'nessun DEMON'],
     confonde=['sottomarino_generico'], aurale='tonfi profondi e regolari (accelerare x10)',
     fonte='09 §5.3: DOSITS, Watkins 1987, Miksis-Olds 2019', confidenza='alta')
voce('balenottera_azzurra', 'Balenottera azzurra (Balaenoptera musculus)', 'Azzurra', 'biologico', 'misticeto', 189.0,
     [sweep(28, 16, 17)], treno(60, 90, 60, 600), ['faroe'], [9, 10, 11, 12, 1, 2], densita=0.05,
     profondita=(10, 50), velocita=(1, 5), ts=12.0, indizi=['unita\' lunghe 15-20 s fra 15 e 40 Hz', 'frasi ripetute'],
     fonte='09 §5.3: DOSITS', confidenza='alta')
voce('megattera', 'Megattera (Megaptera novaeangliae)', 'Megattera', 'biologico', 'misticeto', 137.0,
     [sweep(550, 450, 2.5), impulso(30, 8000, 1.0, rel=-6, picco=400, pendenza=3)], periodico(4, 10),
     ['faroe', 'barents'], ESTATE, densita=0.3, profondita=(5, 40), velocita=(1, 5), ts=10.0,
     indizi=['chiamate ritmiche di ~2,5 s a ~500 Hz (alimentazione)', 'canto d\'inverno raro'],
     aurale='richiami lamentosi', fonte='09 §5.3: DOSITS, Fournet 2018', confidenza='media')
voce('balenottera_minore', 'Balenottera minore (Balaenoptera acutorostrata)', 'Minore', 'biologico', 'misticeto', 165.0,
     [impulso(50, 300, 0.25, picco=120, pendenza=3)], treno(0.9, 1.1, 1, 300), ATLANTICO + ['barents'],
     [9, 10, 11, 12, 1, 2, 3, 4, 5], densita=0.3, profondita=(5, 50), velocita=(1, 6), ts=5.0,
     indizi=['treni d\'impulsi a ~1 s'], fonte='09 §5.3: Minke 2014; livello STIMA', confidenza='bassa')
voce('balenottera_minore_pacifica', 'Balenottera minore del Pacifico settentrionale (Balaenoptera acutorostrata)',
     'Minore (boing)', 'biologico', 'misticeto', 150.0, [impulso(1300, 8000, 2.6, picco=1400, pendenza=6)],
     periodico(240, 480), ['kunashir'], INVERNO, densita=0.2, profondita=(5, 50), velocita=(1, 6), ts=5.0,
     indizi=['«boing» di ~2,5 s a 1,4 kHz con armoniche', 'Pacifico settentrionale, d\'inverno'], aurale='un boing metallico',
     fonte='M, 29_note: banda e durata misurate sulla registrazione SWFSC (Rankin e Barlow 2005); cadenza e livello STIMA',
     confidenza='bassa')
voce('capodoglio', 'Capodoglio (Physeter macrocephalus)', 'Capodoglio', 'biologico', 'odontoceto', 230.0,
     [impulso(5000, 25000, 0.01, picco=12000, pendenza=3)], treno(0.5, 1.5, 40, 600),
     ['faroe', 'gibilterra', 'otranto', 'kunashir'], densita=0.1, profondita=(300, 1000), velocita=(1, 4),
     tipo_livello='pp', ts=10.0, indizi=['clic regolari 5-25 kHz', 'creak alla cattura', 'immersioni di 40 min'],
     aurale='ticchettio regolare come un orologio', fonte='09 §5.3: DOSITS; 02 §4.5', confidenza='alta')
voce('orca', 'Orca (Orcinus orca)', 'Orca', 'biologico', 'odontoceto', 155.0,
     [impulso(1000, 6000, 1.0, picco=2500, pendenza=3)], poisson(600, (1, 3), (1, 4)), ['faroe', 'kunashir', 'barents'],
     densita=0.1, profondita=(5, 50), velocita=(2, 8), estensione=300.0,
     indizi=['chiamate impulsive 1-6 kHz in gruppi'], fonte='09 §5.3: Holt 2011 (135-176 dB)', confidenza='media')
voce('delfini', 'Branco di delfini (delfino comune, globicefali)', 'Delfini', 'biologico', 'odontoceto', 160.0,
     [impulso(2000, 20000, 0.5, picco=8000, pendenza=3)], poisson(3600, (1, 3), (0.2, 1)),
     ['celtica', 'gibilterra', 'faroe', 'otranto'], densita=1.0, profondita=(2, 30), velocita=(4, 15), estensione=500.0,
     indizi=['fischi 2-20 kHz', 'clic oltre la banda', 'branchi in moto veloce'],
     aurale='fischi e cinguettii', fonte='09 §5.3: ADA037620; livello dei fischi STIMA', confidenza='media')
voce('foca_barbata', 'Foca barbata (Erignathus barbatus)', 'Foca barbata', 'biologico', 'pinnipede', 170.0,
     [sweep(4000, 400, 60)], periodico(90, 240), ['barents', 'kunashir'], [3, 4, 5, 6], densita=0.3,
     profondita=(5, 30), velocita=(0, 2), indizi=['trilli discendenti lunghi (10 s-3 min)', 'primavera'],
     aurale='trillo che scende', fonte='09 §5.3: Frouin 2016; livello STIMA', confidenza='media')
voce('tricheco', 'Tricheco (Odobenus rosmarus)', 'Tricheco', 'biologico', 'pinnipede', 170.0,
     [impulso(240, 2000, 0.03, picco=500, pendenza=3)], treno(0.2, 0.3, 10, 300), ['barents'], [1, 2, 3, 4],
     densita=0.1, profondita=(5, 40), velocita=(0, 2),
     indizi=['colpi regolari a ~4 al secondo fra 0,2 e 2 kHz', 'serie lunghe d\'inverno (il canto dei maschi)'],
     aurale='un picchiettio di nocche sul legno',
     fonte='M, 29_note: banda e cadenza misurate sulla registrazione AFSC; livello e stagione STIMA', confidenza='bassa')
voce('merluzzo', 'Merluzzo (Gadus morhua) in riproduzione', 'Merluzzo', 'biologico', 'pesce', 123.0,
     [famiglia(50, 2, decad=6, durata=0.2)], poisson(360), ['barents', 'faroe'], [2, 3, 4], densita=1.0,
     profondita=(50, 200), estensione=1000.0, indizi=['grugniti a 50 Hz con la seconda armonica'],
     confonde=['rete 50 Hz'], fonte='09 §5.3: Cod 2011', confidenza='alta')
voce('eglefino', 'Eglefino (Melanogrammus aeglefinus): coro', 'Eglefino', 'biologico', 'pesce', 115.0,
     [banda(30, 400, picco=150, pendenza=3)], None, ['barents', 'faroe', 'celtica'], [2, 3, 4, 5], densita=0.5,
     profondita=(50, 200), estensione=2000.0, diurno=(12.0, 'notte'), indizi=['coro notturno 30-400 Hz (+12 dB)'],
     fonte='09 §5.3: Haddock; livello STIMA', confidenza='media')
voce('aringa', 'Aringa (Clupea harengus): banco', 'Aringa', 'biologico', 'pesce', 140.0,
     [impulso(1700, 22000, 4.0, picco=4000, pendenza=2)], poisson(120), ['faroe', 'barents'], densita=0.5,
     profondita=(20, 200), velocita=(0, 3), estensione=1000.0, diurno=(6.0, 'notte'),
     indizi=['treni a larga banda 1,7-22 kHz di notte'], fonte='09 §5.3: Wilson 2004; livello STIMA', confidenza='media')
voce('coro_pesci', 'Coro di pesci (sciaenidi)', 'Coro', 'biologico', 'pesce', 110.0,
     [banda(100, 800, picco=400, pendenza=4)], None, ['otranto', 'gibilterra'], ESTATE, densita=1.0,
     profondita=(5, 50), estensione=5000.0, diurno=(20.0, 'crepuscolo'),
     indizi=['fondo +20-30 dB in 100-800 Hz al tramonto', 'settore largo verso costa'],
     fonte='09 §5.3: Hild09, Chorus', confidenza='alta')
voce('gamberi', 'Gamberi schioccatori (Alpheidae): colonia', 'Gamberi', 'biologico', 'crostaceo', 160.0,
     [banda(2000, 20000, picco=4000, pendenza=3)], None, ['otranto', 'gibilterra'], densita=0.5, profondita=(2, 40),
     estensione=3000.0, diurno=(3.0, 'notte'), indizi=['crepitio continuo 2-20 kHz in acqua bassa'],
     aurale='friggere di pancetta', fonte='09 §5.3: Hild09 (183-189 dB pp per schiocco); colonia STIMA', confidenza='media')
# --- geofisici (09 §5.4) ---
voce('t_phase_tettonico', 'T-phase di un terremoto', 'T-phase', 'geofisico', 'sismica', 185.0,
     [impulso(2.5, 20, 60, picco=5, pendenza=6)], poisson(0.2), ['faroe', 'kunashir', 'otranto', 'gibilterra'],
     densita=0.2, profondita=(800, 1200), indizi=['fuso lungo decine di secondi sotto 20 Hz', 'nessuna riga'],
     fonte='09 §5.4: Fox, Volc99; livello alla distanza di gioco STIMA', confidenza='bassa')
voce('t_phase_vulcanico', 'T-phase vulcanico', 'T-phase vulc.', 'geofisico', 'vulcano', 180.0,
     [impulso(3, 15, 20, picco=6, pendenza=6)], poisson(0.5), ['kunashir', 'otranto'], densita=0.1,
     profondita=(500, 1500), indizi=['impulsi piu\' brevi e acuti, picco a ~6 Hz'], fonte='09 §5.4: Volc99, Monowai16',
     confidenza='bassa')
voce('vulcano_sottomarino', 'Vulcano sottomarino attivo', 'Vulcano', 'geofisico', 'vulcano', 170.0,
     [tonale(125), famiglia(50, 6, decad=2, rel=-3), banda(500, 5000, rel=-10, picco=2000)], None, ['kunashir'],
     densita=0.05, profondita=(200, 800), indizi=['rombo con tono a 125 Hz e armoniche di 50 Hz', 'sibili'],
     confonde=['rete 50 Hz'], fonte='09 §5.4: Barcena', confidenza='media')
voce('distacco_iceberg', 'Distacco di un iceberg', 'Distacco', 'geofisico', 'ghiaccio', 220.0,
     [impulso(1, 440, 5, picco=50, pendenza=3)], poisson(0.3), ['barents'], ESTATE, densita=0.2, profondita=(0, 20),
     tipo_livello='pp', indizi=['colpo a larga banda 1-440 Hz, fortissimo'], fonte='09 §5.4: PMEL-ice, Podolskiy22',
     confidenza='alta')
voce('tremore_iceberg', 'Tremore armonico d\'iceberg', 'Tremore', 'geofisico', 'ghiaccio', 180.0,
     [famiglia(40, 8, decad=2, durata=300)], poisson(2), ['barents'], [4, 5, 6, 7, 8, 9, 10], densita=0.2,
     profondita=(20, 150), indizi=['famiglia armonica perfetta a 40 Hz', 'episodi di minuti'],
     confonde=['sottomarino_generico'], fonte='09 §5.4: PMEL-ice; livello STIMA', confidenza='alta')
voce('incaglio_iceberg', 'Iceberg che s\'incaglia', 'Incaglio', 'geofisico', 'ghiaccio', 175.0,
     [sweep(60, 20, 420)], poisson(0.2), ['barents'], ESTATE, densita=0.1, profondita=(50, 200),
     indizi=['suono che scende di frequenza in ~7 minuti'], fonte='09 §5.4: PMEL-ice; livello STIMA', confidenza='alta')
voce('fessurazione_ghiaccio', 'Fessurazione del ghiaccio', 'Fessure', 'geofisico', 'ghiaccio', 150.0,
     [impulso(40, 2000, 0.2, picco=300, pendenza=3)], poisson(600), ['barents', 'kunashir', 'baltico'],
     [12, 1, 2, 3, 4], densita=1.0, profondita=(0, 5), estensione=5000.0, diurno=(6.0, 'notte'),
     indizi=['crepitii impulsivi sopra 40 Hz, piu\' forti nelle notti fredde'], fonte='09 §5.4; 02 §4.6', confidenza='media')
voce('fusione_ghiaccio', 'Ghiaccio glaciale che fonde', 'Fusione', 'geofisico', 'ghiaccio', 140.0,
     [banda(1000, 3000, picco=1800, pendenza=3)], None, ['barents'], ESTATE, densita=0.3, profondita=(0, 10),
     estensione=3000.0, indizi=['frizzare continuo 1-3 kHz'], fonte='09 §5.4: IGF; livello STIMA', confidenza='media')
# --- meteo e mare (09 §5.5; vento e pioggia sono gia' nel rumore ambiente) ---
voce('risacca', 'Risacca e frangenti su costa e bassifondi', 'Risacca', 'meteo', 'costa', 165.0,
     [banda(50, 1000, picco=150, pendenza=3)], None, densita=0.2, profondita=(0, 5), estensione=5000.0,
     indizi=['fondo a larga banda dal settore della costa'], fonte='09 §5.5; livello STIMA', confidenza='bassa')
# --- attivita' umane (09 §5.2 e §5.6) ---
voce('airgun', 'Nave sismica con cannoni ad aria', 'Airgun', 'industriale', 'sismica', 250.0,
     [impulso(5, 300, 0.1, picco=50, pendenza=3)], periodico(10, 20), ['faroe', 'celtica'], [4, 5, 6, 7, 8, 9, 10],
     densita=0.05, profondita=(5, 10), velocita=(4, 6), indizi=['colpi ogni 10-20 s per giorni', 'udibili a migliaia di km'],
     fonte='09 §5.6: Hild09 (260 dB equivalente; STIMA 250 rms)', confidenza='alta')
voce('battitura_pali', 'Battitura di pali (cantiere portuale o piattaforma)', 'Pali', 'industriale', 'cantiere', 220.0,
     [impulso(100, 1000, 0.04, picco=300, pendenza=3)], treno(2, 4, 120, 3600), ['baltico', 'celtica', 'otranto'],
     densita=0.05, profondita=(5, 20), indizi=['impulsi di 30-50 ms ogni 2-4 s', 'fermi di notte'],
     fonte='09 §5.6: Hild09 (205,9 + 10,2 log kJ)', confidenza='alta')
voce('ecoscandaglio_profondo', 'Ecoscandaglio multifascio di una nave oceanografica', 'Multifascio', 'civile',
     'ecoscandaglio', 230.0, [impulso(11500, 12500, 0.01)], periodico(5, 15), densita=0.05, profondita=(5, 10),
     velocita=(8, 12), indizi=['impulsi brevi a 12 kHz'], fonte='09 §5.2: Hild09 (245 dB, fascio verso il fondo)',
     confidenza='alta')
voce('profilatore', 'Profilatore del sottofondo', 'Profilatore', 'civile', 'ecoscandaglio', 220.0,
     [impulso(3000, 7000, 0.01, picco=5000)], periodico(1, 2), densita=0.05, profondita=(5, 10), velocita=(4, 8),
     indizi=['impulsi brevi regolari a 3-7 kHz'], fonte='09 §5.2: Hild09 (230 dB)', confidenza='alta')
voce('peschereccio_strascico', 'Peschereccio a strascico al lavoro', 'Strascico', 'civile', 'pesca', 160.0,
     [famiglia(25, 6, decad=2), banda(50, 2000, rel=-6, picco=200, pendenza=3)], None,
     ['celtica', 'otranto', 'kunashir', 'faroe', 'barents'], densita=1.0, profondita=(0, 2), velocita=(2, 4),
     indizi=['righe del diesel a 25 Hz', 'rumore di strascico (verricelli, cavi, rete)', 'elica a passo variabile'],
     confonde=['mercantile'], fonte='09 §5.2: Hild09, RN-aural; livelli STIMA', confidenza='bassa')
# la rete a strascico di un peschereccio al lavoro, sul fondo dietro la nave: la missione la mette alla classe
# «peschereccio» fra 2 e 5,5 kn (densita' 0: non entra nel paesaggio da sola). Divergenti che strisciano, rulli e
# catene della lima da piombo che urtano i sassi: un fruscio largo con i colpi, piu' i cavi che vibrano. STIMA
voce('rete_strascico', 'Rete a strascico sul fondo', 'Rete', 'civile', 'pesca', 152.0,
     [banda(100, 5000, picco=500, pendenza=3), impulso(80, 4000, 0.03, rel=8, picco=600, pendenza=3),
      famiglia(14, 3, decad=4, rel=-10)], poisson(1800, gruppi=(1, 3), interno=(0.05, 0.2)), densita=0.0,
     profondita=(0, 0), velocita=(2, 5.5),
     indizi=['fruscio largo 100 Hz-5 kHz con colpi fitti, sul fondo', 'centinaia di metri dietro un peschereccio lento'],
     confonde=['peschereccio_strascico'],
     fonte='09 §5.2: Hild09, RN-aural (attrezzi da fondo); livelli e ritmo STIMA', confidenza='bassa')
voce('peschereccio_piccolo', 'Peschereccio piccolo in moto', 'Peschereccio', 'civile', 'pesca', 150.0,
     [famiglia(18, 5, decad=2), banda(50, 1000, rel=-3, picco=100, pendenza=3)], None, densita=1.5, profondita=(0, 2),
     velocita=(6, 10), indizi=['armoniche del motore 10-50 Hz', 'continuo sopra 50 Hz dall\'elica'],
     fonte='09 §5.2: Hild09, Suisan95', confidenza='media')
voce('fuoribordo', 'Imbarcazione con fuoribordo', 'Fuoribordo', 'civile', 'diporto', 165.0,
     [banda(1000, 5000, picco=3000, pendenza=3)], None, ['otranto', 'gibilterra', 'baltico'], ESTATE, densita=0.5,
     profondita=(0, 1), velocita=(15, 25), indizi=['ronzio 1-5 kHz veloce, molto locale'], fonte='09 §5.2: Hild09',
     confidenza='alta')
voce('piattaforma', 'Piattaforma di perforazione fissa', 'Piattaforma', 'industriale', 'petrolio', 184.0,
     [banda(700, 1400, picco=1000, pendenza=2)], None, ['faroe', 'celtica'], densita=0.02, profondita=(0, 30),
     indizi=['continuo a 700-1400 Hz, fermo'], fonte='09 §5.2: Hild09', confidenza='alta')
voce('esplosione', 'Esplosione d\'esercitazione', 'Esplosione', 'industriale', 'esplosivo', 280.0,
     [impulso(10, 200, 1.0, picco=50, pendenza=1)], poisson(0.05), densita=0.02, profondita=(10, 100),
     indizi=['un colpo fortissimo 10-200 Hz'], fonte='09 §5.1: SL = 269 + 7,53 log lb (Hild09)', confidenza='alta')
