"""Gradi, fasi della carriera, decorazioni, soglie, giudizi e motivazioni (Fase 4a, M3) -> data/motore/carriera.toml.
Lo legge nucleo/carriera.hpp (carica_carriera). Dati e proposte dal dossier docs/ricerca/13_carriera_gradi_decorazioni.md
(gradi §1, criteri §2, decorazioni §3, motivazioni §5.1-5.2); i numeri del punteggio sono STIME di taratura (§2.3).
I colori dei nastrini sono approssimati dalle descrizioni del dossier: righe da sinistra a destra, larghezza in
trentaseiesimi del nastrino (orizzontali solo per la Presidential Unit Citation).
Uso: carriera.py <radice del progetto>   (lo chiama anche esporta_motore.py)"""
import os, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from siluri import valore   # noqa: E402

# --- gradi (§1.1, §1.5): id, sigla, nome d'epoca, nome italiano, NATO ---
GRADI = {
    'usn': [('O-1', 'ENS', 'Ensign', 'guardiamarina', 'OF-1'),
            ('O-2', 'LTJG', 'Lieutenant junior grade', 'sottotenente di vascello', 'OF-1'),
            ('O-3', 'LT', 'Lieutenant', 'tenente di vascello', 'OF-2'),
            ('O-4', 'LCDR', 'Lieutenant Commander', 'capitano di corvetta', 'OF-3'),
            ('O-5', 'CDR', 'Commander', 'capitano di fregata', 'OF-4'),
            ('O-6', 'CAPT', 'Captain', 'capitano di vascello', 'OF-5'),
            ('O-7', 'RDML', 'Rear Admiral (lower half)', 'contrammiraglio', 'OF-6')],
    'vmf': [('lt', 'л-т', 'lejtenant', 'tenente', 'OF-1'),
            ('st_lt', 'ст. л-т', 'staršij lejtenant', 'tenente anziano', 'OF-1'),
            ('kap_lt', 'к-л', 'kapitan-lejtenant', 'capitano-tenente', 'OF-2'),
            ('kap3', 'к3р', 'kapitan 3-go ranga', 'capitano di 3° rango', 'OF-3'),
            ('kap2', 'к2р', 'kapitan 2-go ranga', 'capitano di 2° rango', 'OF-4'),
            ('kap1', 'к1р', 'kapitan 1-go ranga', 'capitano di 1° rango', 'OF-5'),
            ('kontr_adm', 'к-адм', 'kontr-admiral', 'contrammiraglio', 'OF-6')],
}

# --- fasi della carriera compressa (§2.3.2): grado, incarico, missioni minime, uscita ---
# uscita: "missioni" (basta il numero), "qualifica" (media >= media_minima e la qualifica), "media" (media),
# "esame" (l'ultima missione della fase sopra media_minima), "commissione" (commissione di selezione / attestazione)
FASI = {
    'usn': [dict(grado='O-1', incarico='scuola e prototipo', incarico_en='school and prototype', missioni=2,
                 uscita='missioni'),
            dict(grado='O-2', incarico='ufficiale di guardia in addestramento', incarico_en='officer of the deck under instruction',
                 missioni=3, uscita='qualifica', media_minima=150, qualifica='dolphins'),
            dict(grado='O-3', incarico='ufficiale di guardia in immersione', incarico_en='officer of the deck',
                 missioni=4, uscita='commissione', in_zona=250, sotto_zona=400),
            dict(grado='O-4', incarico='ufficiale in seconda', incarico_en='executive officer', missioni=4,
                 uscita='commissione', in_zona=300, sotto_zona=450, qualifica='comando'),
            dict(grado='O-5', incarico='comandante', incarico_en='commanding officer', missioni=10,
                 uscita='commissione', in_zona=400, sotto_zona=550),
            dict(grado='O-6', incarico='comandante di squadriglia', incarico_en='squadron commander', missioni=0,
                 uscita='fine')],
    'vmf': [dict(grado='lt', incarico='comandante del gruppo siluri', incarico_en='torpedo group commander',
                 missioni=1, uscita='missioni'),
            dict(grado='st_lt', incarico='comandante della БЧ-3', incarico_en='BCh-3 department head', missioni=2,
                 uscita='qualifica', media_minima=150, qualifica='esami_guardia'),
            dict(grado='kap_lt', incarico='pomoščnik', incarico_en='assistant commander', missioni=3,
                 uscita='commissione', in_zona=250, sotto_zona=400),
            dict(grado='kap3', incarico='starpom', incarico_en='first officer', missioni=3, uscita='commissione',
                 in_zona=300, sotto_zona=450, qualifica='vsok'),
            dict(grado='kap2', incarico='comandante', incarico_en='commanding officer', missioni=10,
                 uscita='commissione', in_zona=400, sotto_zona=550),
            dict(grado='kap1', incarico='comandante anziano di brigata', incarico_en='senior brigade commander',
                 missioni=0, uscita='fine')],
}

# --- regole generali (STIME §2.3) ---
REGOLE = dict(
    mesi_per_missione=2, mesi_per_missione_al_comando=3, media_su=6, missioni_fra_commissioni=3,
    mancate_per_congedo=2, anni_pensione=20, soglia_mediocre=150, mediocri_per_trasferimento=3,
    rendimenti=[1.0, 0.5, 0.25, 0.0], ripetizione=[1.0, 1.5, 2.0], zampolit=0.10,
    data_inizio='1989-06', anno_nomina_breve_usn=1973, anno_nomina_breve_vmf=1978,
)
PUNTEGGIO = dict(
    riuscita=400.0, completezza=150.0, discrezione=150.0, ordini=100.0, efficienza=100.0, danni_min=-300.0,
    violazioni_min=-200.0, squalifica_sotto=-400.0,
    peso_completezza=0.5, peso_classificazioni=0.3, peso_rapporti=0.2,
    esposizione_piena=100.0,   # esposizione pesata che azzera la discrezione
    infrazione_roe=30.0, peso_ordini=0.6, peso_serie=0.4,
    peso_colpi=0.7, peso_carico=0.3, senza_lanci=0.7, arma_incoerente=10.0,
    danno_scafo=150.0, ferito=20.0, morto=50.0, arma_persa=10.0, battello_perso=-300.0,
    neutrale=150.0, amico=200.0, fuoco_senza_ordine=100.0, zona_vietata=50.0, fuoco_senza_ordine_ordini=50.0,
    # acque territoriali straniere in pace e in crisi (UNCLOS art. 20: in superficie e con la bandiera, dossier 22;
    # S-363, «U-137», incagliato a Karlskrona nel 1981, dossier 23 E14 [WP-U137]): ogni entrata pesa sugli ordini come
    # un'infrazione delle ROE; farsi scoprire dentro
    # e' un incidente diplomatico, come colpire un neutrale. Nelle operazioni speciali l'entrata e' l'ordine, la scoperta
    # vale la meta'. In guerra niente. STIMA del gioco
    acque_territoriali=30.0, acque_scoperto=150.0,
    pericolo_max=0.25, siluro_vicino_m=5000.0, contatto_asw_s=1800.0, margine_collasso=0.2,
)
# docs/26 §4 (29/10/2026): anche l'ecoscandaglio (un ping verso il fondo, meno del sonar attivo), il radar proprio acceso
# e il lancio di un missile (la vampa e la scia in superficie). STIMA di gioco
# 31/10/2026 (secondo giro, approvato): le luci di navigazione accese in superficie di notte, solo in crisi e in guerra
# (in pace sono la regola, COLREG: le conta l'adattatore, carriera_missione.cpp)
ESPOSIZIONE = dict(ping=5.0, fix=25.0, avvistamento=20.0, esm=15.0, transitorio=2.0, boa=10.0, ecoscandaglio=3.0, radar=8.0,
                   missile=15.0, luci=10.0)
MOLTIPLICATORI = dict(
    situazione=dict(pace=1.0, crisi=1.3, guerra=1.6),
    avversario=dict(impreparato=0.8, regolare=1.0, veterano=1.25, elite=1.5),
    realismo_min=0.5,
)

# --- colori dei nastrini ---
BLU, AZZ, BIA, ROS, CRE, ORO, VER, ARA, VIO, BLS = ('#1f2f5c', '#5b9bd5', '#f2f2f2', '#c8102e', '#9b1b30', '#ffc72c',
                                                    '#1e6b3a', '#e87722', '#5b2c83', '#0b1a3a')
ARG, GRI, ROSC = '#c0c0c0', '#8a8d8f', '#8b0000'


def _rig(*parti):
    assert abs(sum(w for _, w in parti) - 36) < 1e-9, parti
    return [list(p) for p in parti]


# id: campi. tipo: personale | reparto | servizio | distintivo. situazioni: dove la si assegna per merito ("" =
# automatica o fuori dalle soglie). dispositivi: "stella" (USA, conferimenti successivi), "v", "numero" (URSS).
DECORAZIONI = {
    # --- US Navy (§3.1-3.2) ---
    'medal_of_honor': dict(marina='usn', nome='Medal of Honor', nome_it='Medal of Honor', precedenza=1,
                           righe=_rig((AZZ, 36)), motivo='stelle_bianche', fonte='§3.1 [alta]'),
    'navy_cross': dict(marina='usn', nome='Navy Cross', nome_it='Navy Cross', precedenza=2,
                       righe=_rig((BLU, 16), (BIA, 4), (BLU, 16)), fonte='§3.1 [alta]'),
    'navy_dsm': dict(marina='usn', nome='Navy Distinguished Service Medal', nome_it='Navy Distinguished Service Medal',
                     precedenza=4, righe=_rig((BLS, 15), (ORO, 6), (BLS, 15)), fonte='§3.1 [alta]'),
    'silver_star': dict(marina='usn', nome='Silver Star', nome_it='Silver Star', precedenza=5,
                        righe=_rig((BLU, 4), (BIA, 1), (BLU, 1), (BIA, 1), (BLU, 3), (BIA, 3), (ROS, 10), (BIA, 3),
                                   (BLU, 3), (BIA, 1), (BLU, 1), (BIA, 1), (BLU, 4)), fonte='§3.1 [alta]'),
    'legion_of_merit': dict(marina='usn', nome='Legion of Merit', nome_it='Legion of Merit', precedenza=7,
                            righe=_rig((BIA, 2), (CRE, 32), (BIA, 2)), dispositivi=['stella', 'v'], fonte='§3.1 [alta]'),
    'navy_marine_corps_medal': dict(marina='usn', nome='Navy and Marine Corps Medal',
                                    nome_it='Navy and Marine Corps Medal', precedenza=9,
                                    righe=_rig((BLU, 12), (ORO, 12), (ROS, 12)), confidenza='bassa',
                                    fonte='§3.1; nastrino non descritto nel dossier'),
    'bronze_star': dict(marina='usn', nome='Bronze Star Medal', nome_it='Bronze Star Medal', precedenza=10,
                        righe=_rig((BIA, 1), (ROS, 15), (BIA, 1), (BLU, 2), (BIA, 1), (ROS, 15), (BIA, 1)),
                        dispositivi=['stella', 'v'], fonte='§3.1 [alta]'),
    'purple_heart': dict(marina='usn', nome='Purple Heart', nome_it='Purple Heart', precedenza=11,
                         righe=_rig((BIA, 2), (VIO, 32), (BIA, 2)), fonte='§3.1 [media]'),
    'meritorious_service': dict(marina='usn', nome='Meritorious Service Medal', nome_it='Meritorious Service Medal',
                                precedenza=13, righe=_rig((CRE, 6), (BIA, 4), (CRE, 16), (BIA, 4), (CRE, 6)),
                                fonte='§3.1 [alta]'),
    'navy_commendation': dict(marina='usn', nome='Navy Commendation Medal', nome_it='Navy Commendation Medal',
                              precedenza=16, righe=_rig((VER, 3), (BIA, 2), (VER, 26), (BIA, 2), (VER, 3)),
                              dispositivi=['stella', 'v'], fonte='§3.1 [alta]'),
    'navy_achievement': dict(marina='usn', nome='Navy Achievement Medal', nome_it='Navy Achievement Medal',
                             precedenza=18, righe=_rig((ARA, 3), (VER, 30), (ARA, 3)), grado_max='O-4',
                             fonte='§3.1 [alta]'),
    'combat_action_ribbon': dict(marina='usn', nome='Combat Action Ribbon', nome_it='Combat Action Ribbon',
                                 precedenza=19, tipo='servizio',
                                 righe=_rig((BLU, 3), (ORO, 14), (BLU, 1), (BIA, 1), (ROS, 1), (BIA, 1), (BLU, 1),
                                            (ORO, 11), (ROS, 3)), fonte='§3.1 [media]'),
    'presidential_unit_citation': dict(marina='usn', nome='Presidential Unit Citation (Navy)',
                                       nome_it='Presidential Unit Citation', precedenza=20, tipo='reparto',
                                       righe=_rig((BLU, 12), (ORO, 12), (ROS, 12)), orizzontale=True,
                                       fonte='§3.1 [alta]'),
    'navy_unit_commendation': dict(marina='usn', nome='Navy Unit Commendation', nome_it='Navy Unit Commendation',
                                   precedenza=21, tipo='reparto',
                                   righe=_rig((BLU, 1), (ORO, 1), (ROS, 1), (VER, 30), (ROS, 1), (ORO, 1), (BLU, 1)),
                                   fonte='§3.1 [media]'),
    'meritorious_unit_commendation': dict(marina='usn', nome='Meritorious Unit Commendation',
                                          nome_it='Meritorious Unit Commendation', precedenza=22, tipo='reparto',
                                          righe=_rig((VER, 15), (ROS, 1), (ORO, 1), (BLU, 2), (ORO, 1), (ROS, 1),
                                                     (VER, 15)), fonte='§3.1 [media]'),
    'navy_e_ribbon': dict(marina='usn', nome='Navy "E" Ribbon', nome_it='Navy «E» Ribbon', precedenza=23,
                          tipo='servizio', righe=_rig((BLS, 30), (ORO, 3), (BIA, 3)), fonte='§3.2 [media]'),
    'navy_expeditionary': dict(marina='usn', nome='Navy Expeditionary Medal', nome_it='Navy Expeditionary Medal',
                               precedenza=24, tipo='servizio', righe=_rig((BLU, 12), (ORO, 4), (BLU, 4), (ORO, 4), (BLU, 12)),
                               fonte='§3.2 [media]'),
    'national_defense': dict(marina='usn', nome='National Defense Service Medal',
                             nome_it='National Defense Service Medal', precedenza=25, tipo='servizio',
                             righe=_rig((ROS, 14), (ORO, 1), (ROS, 1), (BIA, 1), (BLU, 2), (BIA, 1), (ROS, 1), (ORO, 1),
                                        (ROS, 14)), fonte='§3.2 [alta]'),
    'sea_service_deployment': dict(marina='usn', nome='Sea Service Deployment Ribbon',
                                   nome_it='Sea Service Deployment Ribbon', precedenza=26, tipo='servizio',
                                   righe=_rig((BLS, 9), (ROS, 3), (ORO, 3), (AZZ, 6), (ORO, 3), (ROS, 3), (BLS, 9)),
                                   fonte='§3.2 [media]'),
    'arctic_service': dict(marina='usn', nome='Navy Arctic Service Ribbon', nome_it='Navy Arctic Service Ribbon',
                           precedenza=27, tipo='servizio', righe=_rig((BIA, 12), (AZZ, 12), (BIA, 12)),
                           confidenza='bassa', fonte='§3.2; colori da verificare'),
    'dolphins': dict(marina='usn', nome='Submarine Warfare Insignia (officer)', nome_it='dolphins d\'oro', precedenza=90,
                     tipo='distintivo', fonte='§1.3 [alta]'),
    'command_at_sea': dict(marina='usn', nome='Command at Sea Insignia', nome_it='distintivo Command at Sea',
                           precedenza=91, tipo='distintivo', fonte='§3.2 [alta]'),
    'submarine_combat_patrol': dict(marina='usn', nome='Submarine Combat Patrol Insignia',
                                    nome_it='distintivo di pattuglia di combattimento', precedenza=92,
                                    tipo='distintivo', fonte='§3.2 [alta]; PROPOSTA per la guerra'),
    # --- VMF (§3.4-3.5) ---
    'eroe_urss': dict(marina='vmf', nome='Hero of the Soviet Union', nome_it='Eroe dell\'Unione Sovietica',
                      precedenza=1, righe=_rig((ROS, 36)), con='lenin', fonte='§3.4 [alta]'),
    'lenin': dict(marina='vmf', nome='Order of Lenin', nome_it='Ordine di Lenin', precedenza=2,
                  righe=_rig((ORO, 1), (ROS, 1), (ORO, 1), (ROS, 30), (ORO, 1), (ROS, 1), (ORO, 1)), fonte='§3.4 [alta]'),
    'bandiera_rossa': dict(marina='vmf', nome='Order of the Red Banner', nome_it='Ordine della Bandiera Rossa',
                           precedenza=4, righe=_rig((BIA, 2), (ROS, 13), (BIA, 6), (ROS, 13), (BIA, 2)),
                           fonte='§3.4 [alta]'),
    'usakov_2': dict(marina='vmf', nome='Order of Ushakov, 2nd class', nome_it='Ordine di Ušakov di 2ª classe',
                     precedenza=5, righe=_rig((BIA, 6), (AZZ, 24), (BIA, 6)), confidenza='bassa',
                     fonte='§3.4; PROPOSTA per la guerra, nastrino da verificare'),
    'stella_rossa': dict(marina='vmf', nome='Order of the Red Star', nome_it='Ordine della Stella Rossa', precedenza=6,
                         righe=_rig((ROSC, 15), (ARG, 6), (ROSC, 15)), fonte='§3.4 [alta]'),
    'servizio_patria_1': dict(marina='vmf', nome='Order "For Service to the Homeland", 1st class',
                              nome_it='Ordine «Per il servizio alla Patria» di 1ª classe', precedenza=7,
                              righe=_rig((AZZ, 16), (ORO, 4), (AZZ, 16)), fonte='§3.4 [alta]'),
    'servizio_patria_2': dict(marina='vmf', nome='Order "For Service to the Homeland", 2nd class',
                              nome_it='Ordine «Per il servizio alla Patria» di 2ª classe', precedenza=8,
                              righe=_rig((AZZ, 13), (ORO, 3), (AZZ, 4), (ORO, 3), (AZZ, 13)), successiva='servizio_patria_1',
                              fonte='§3.4 [alta]'),
    'servizio_patria_3': dict(marina='vmf', nome='Order "For Service to the Homeland", 3rd class',
                              nome_it='Ordine «Per il servizio alla Patria» di 3ª classe', precedenza=9,
                              righe=_rig((AZZ, 10), (ORO, 3), (AZZ, 3), (ORO, 4), (AZZ, 3), (ORO, 3), (AZZ, 10)),
                              successiva='servizio_patria_2', fonte='§3.4 [alta]'),
    'coraggio_personale': dict(marina='vmf', nome='Order "For Personal Courage"',
                               nome_it='Ordine «Per il coraggio personale»', precedenza=10,
                               righe=_rig((ROS, 14), (BIA, 8), (ROS, 14)), confidenza='bassa',
                               fonte='§3.4; dal 28/12/1988, nastrino da verificare'),
    'merito_combattimento': dict(marina='vmf', nome='Medal "For Battle Merit"',
                                 nome_it='Medaglia «Per merito in combattimento»', precedenza=12,
                                 righe=_rig((ORO, 3), (GRI, 30), (ORO, 3)), confidenza='bassa', fonte='§3.4'),
    'distinzione_1': dict(marina='vmf', nome='Medal "For Distinction in Military Service", 1st class',
                          nome_it='Medaglia «Per distinzione nel servizio militare» di 1ª classe', precedenza=13,
                          righe=_rig((ROS, 3), (VER, 4), (ROS, 22), (VER, 4), (ROS, 3)), fonte='§3.4 [alta]'),
    'distinzione_2': dict(marina='vmf', nome='Medal "For Distinction in Military Service", 2nd class',
                          nome_it='Medaglia «Per distinzione nel servizio militare» di 2ª classe', precedenza=14,
                          righe=_rig((ROS, 3), (VER, 4), (ROS, 22), (VER, 4), (ROS, 3)), successiva='distinzione_1',
                          fonte='§3.4 [alta]'),
    'giubilare_70': dict(marina='vmf', nome='Jubilee Medal "70 Years of the Armed Forces of the USSR"',
                         nome_it='Medaglia «70 anni delle Forze armate dell\'URSS»', precedenza=20, tipo='servizio',
                         righe=_rig((ROS, 10), (ORO, 4), (ROS, 8), (ORO, 4), (ROS, 10)), confidenza='bassa',
                         fonte='§3.4 [media]'),
    'servizio_impeccabile_3': dict(marina='vmf', nome='Medal "For Impeccable Service", 3rd class',
                                   nome_it='Medaglia «Per il servizio impeccabile» di 3ª classe', precedenza=23,
                                   tipo='servizio', righe=_rig((VER, 4), (ROS, 12), (ORO, 4), (ROS, 12), (VER, 4)),
                                   anni=10, fonte='§3.4 [alta]'),
    'servizio_impeccabile_2': dict(marina='vmf', nome='Medal "For Impeccable Service", 2nd class',
                                   nome_it='Medaglia «Per il servizio impeccabile» di 2ª classe', precedenza=22,
                                   tipo='servizio', righe=_rig((VER, 4), (ROS, 11), (ORO, 2), (ROS, 2), (ORO, 2), (ROS, 11), (VER, 4)),
                                   anni=15, fonte='§3.4 [alta]'),
    'servizio_impeccabile_1': dict(marina='vmf', nome='Medal "For Impeccable Service", 1st class',
                                   nome_it='Medaglia «Per il servizio impeccabile» di 1ª classe', precedenza=21,
                                   tipo='servizio', righe=_rig((VER, 4), (ROS, 8), (ORO, 2), (ROS, 2), (ORO, 2), (ROS, 2), (ORO, 2),
                                                               (ROS, 10), (VER, 4)),
                                   anni=20, fonte='§3.4 [alta]'),
    'encomio_comandante': dict(marina='vmf', nome='Commendation of the Commander-in-Chief of the Navy',
                               nome_it='encomio del Comandante in capo della Marina', precedenza=40, tipo='reparto',
                               fonte='§3.7 PROPOSTA (senza nastrino)'),
    'bandiera_rossa_battello': dict(marina='vmf', nome='Order of the Red Banner (ship)',
                                    nome_it='Ordine della Bandiera Rossa al battello', precedenza=41, tipo='reparto',
                                    righe=_rig((BIA, 2), (ROS, 13), (BIA, 6), (ROS, 13), (BIA, 2)),
                                    fonte='§3.7 PROPOSTA'),
    'comandante_sottomarino': dict(marina='vmf', nome='Submarine Commander badge',
                                   nome_it='distintivo «Comandante di sottomarino»', precedenza=90, tipo='distintivo',
                                   fonte='§3.5 [alta]'),
}
BASE_DECORAZIONE = dict(tipo='personale', righe=[], orizzontale=False, motivo='', dispositivi=['stella'], con='',
                        successiva='', grado_max='', anni=0, confidenza='media', fonte='')

# --- soglie per missione (§3.7): P minimo, decorazione, dispositivo, condizione ---
# condizione: "" | "pericolo" (pericolo massimo e nessuna infrazione) | "speciale" (operazione speciale)
SOGLIE = {
    ('usn', 'pace'): [(250, 'navy_achievement', ''), (400, 'navy_commendation', ''), (600, 'meritorious_service', ''),
                      (850, 'legion_of_merit', '')],
    ('usn', 'guerra'): [(250, 'navy_commendation', 'v'), (400, 'bronze_star', 'v'), (600, 'silver_star', ''),
                        (850, 'navy_cross', ''), (1100, 'medal_of_honor', '', 'pericolo')],
    ('vmf', 'pace'): [(250, 'distinzione_2', ''), (400, 'servizio_patria_3', ''), (600, 'stella_rossa', ''),
                      (850, 'bandiera_rossa', ''), (1100, 'lenin', '', 'speciale')],
    ('vmf', 'guerra'): [(250, 'merito_combattimento', ''), (400, 'stella_rossa', ''), (600, 'bandiera_rossa', ''),
                        (850, 'usakov_2', ''), (1100, 'eroe_urss', '', 'pericolo')],
}
REPARTO = {   # (marina): [(P, decorazione, condizione)]; "guerra_speciale" = in guerra o in un'operazione speciale
    'usn': [(500, 'meritorious_unit_commendation', ''), (700, 'navy_unit_commendation', ''),
            (1000, 'presidential_unit_citation', 'guerra_speciale')],
    'vmf': [(700, 'encomio_comandante', ''), (1000, 'bandiera_rossa_battello', 'guerra_speciale')],
}
FINE_COMANDO = {   # media delle missioni al comando (§3.7 punto 4)
    'usn': [(250, 'navy_commendation'), (400, 'meritorious_service'), (600, 'legion_of_merit')],
    'vmf': [(250, 'servizio_patria_3'), (400, 'servizio_patria_2'), (600, 'servizio_patria_1')],
}
# incendio o falla domati in pace; dal 31/10/2026 anche un soccorso riuscito in pace (uomini in mare salvati), con la
# formula `soccorso` delle motivazioni
CORAGGIO = {'usn': 'navy_marine_corps_medal', 'vmf': 'coraggio_personale'}

# --- giudizi a parole (§2.3.1) ---
LIVELLI = [(850, 'eccezionale', 'outstanding'), (600, 'ottimo', 'excellent'), (400, 'buono', 'good'),
           (250, 'adeguato', 'satisfactory'), (150, 'mediocre', 'marginal'), (0, 'insufficiente', 'unsatisfactory')]
VOCI = {   # voce: (forte it, debole it, forte en, debole en)
    'riuscita': ('obiettivi pienamente raggiunti', 'obiettivi mancati', 'objectives fully achieved', 'objectives missed'),
    'completezza': ('quadro tattico completo', 'quadro tattico lacunoso', 'complete tactical picture',
                    'incomplete tactical picture'),
    'discrezione': ('ottima discrezione', 'discrezione da migliorare', 'excellent stealth', 'stealth needs improvement'),
    'ordini': ('ordini e regole d\'ingaggio rispettati', 'ordini e regole d\'ingaggio trascurati',
               'orders and rules of engagement followed', 'orders and rules of engagement neglected'),
    'efficienza': ('uso esemplare delle armi', 'uso delle armi da migliorare', 'exemplary use of weapons',
                   'weapons employment needs improvement'),
}

# --- motivazioni (§5.1-5.2): modelli scritti per il gioco ---
# {da}: chi conferisce; {decorazione}, {grado}, {nome}, {incarico}, {battello}, {teatro}, {periodo}, {fatti}
MOTIVAZIONI = {
    'usn': dict(
        conferisce_it={'alta': 'Il Presidente degli Stati Uniti ha il piacere di conferire',
                       'media': 'Il Segretario della Marina ha il piacere di conferire',
                       'bassa': 'Il Comandante della Flotta ha il piacere di conferire'},
        conferisce_en={'alta': 'The President of the United States takes pleasure in presenting',
                       'media': 'The Secretary of the Navy takes pleasure in presenting',
                       'bassa': 'The Commander in Chief of the Fleet takes pleasure in presenting'},
        formula_it={'valore': 'per lo straordinario eroismo dimostrato', 'merito': 'per la condotta eccezionalmente meritoria',
                    'servizio': 'per il servizio meritorio prestato',
                    'soccorso': 'per l\'eroismo dimostrato nel soccorso di uomini in mare'},
        formula_en={'valore': 'for extraordinary heroism', 'merito': 'for exceptionally meritorious conduct',
                    'servizio': 'for meritorious service', 'soccorso': 'for heroism in the rescue of men at sea'},
        apertura_it='{da} la {decorazione} al {grado} {nome}, Marina degli Stati Uniti,',
        apertura_en='{da} the {decorazione} to {grado} {nome}, United States Navy,',
        incarico_it='{formula} come {incarico} dell\'USS {battello} durante operazioni nel {teatro}, {periodo}.',
        incarico_en='{formula} as {incarico} of USS {battello} during operations in the {teatro}, {periodo}.',
        chiusura_it='Il suo coraggio, il suo giudizio e la sua dedizione al dovere furono all\'altezza delle più alte '
                    'tradizioni della Marina degli Stati Uniti.',
        chiusura_en='The courage, judgment and devotion to duty displayed were in keeping with the highest traditions '
                    'of the United States Naval Service.',
        reparto_it='{da} la {decorazione} all\'equipaggio dell\'USS {battello} {formula} durante operazioni nel '
                   '{teatro}, {periodo}.',
        reparto_en='{da} the {decorazione} to the crew of USS {battello} {formula} during operations in the {teatro}, '
                   '{periodo}.',
        postuma_it='alla memoria', postuma_en='posthumously',
        segreto_it='I particolari dell\'operazione restano coperti dal segreto.',
        segreto_en='The details of the operation remain classified.',
    ),
    'vmf': dict(
        intestazione_it='Decreto del Presidium del Soviet Supremo dell\'URSS.',
        intestazione_en='Decree of the Presidium of the Supreme Soviet of the USSR.',
        formula_it={'valore': 'Per il coraggio e l\'eroismo dimostrati nell\'adempimento del dovere militare',
                    'speciale': 'Per il coraggio e l\'audacia dimostrati nell\'esecuzione di un incarico speciale del comando',
                    'merito': 'Per i successi nell\'addestramento al combattimento e politico',
                    'servizio': 'Per il servizio esemplare',
                    'soccorso': 'Per il coraggio e l\'abnegazione dimostrati nel salvataggio di vite umane in mare'},
        formula_en={'valore': 'For courage and heroism displayed in the performance of military duty',
                    'speciale': 'For courage and daring displayed in carrying out a special assignment of the command',
                    'merito': 'For success in combat and political training',
                    'servizio': 'For exemplary service',
                    'soccorso': 'For courage and selflessness displayed in saving human lives at sea'},
        dispositivo_it='{formula}, decorare con {decorazione} il {grado} {nome}, {incarico} del {battello}.',
        dispositivo_en='{formula}, to award the {decorazione} to {grado} {nome}, {incarico} of {battello}.',
        eroe_it='{formula}, conferire il titolo di Eroe dell\'Unione Sovietica con la consegna dell\'Ordine di Lenin e '
                'della medaglia Stella d\'oro al {grado} {nome}, {incarico} del {battello}.',
        eroe_en='{formula}, to confer the title of Hero of the Soviet Union, with the Order of Lenin and the Gold Star '
                'medal, on {grado} {nome}, {incarico} of {battello}.',
        reparto_it='{formula}, decorare con {decorazione} il sottomarino {battello}.',
        reparto_en='{formula}, to award the {decorazione} to the submarine {battello}.',
        postuma_it='alla memoria', postuma_en='posthumously',
        chiusura_it='Mosca, Cremlino, {data}.', chiusura_en='Moscow, the Kremlin, {data}.',
        segreto_it='Non per la stampa.', segreto_en='Not for publication.',
    ),
}
# livello di chi conferisce (USA) e formula per decorazione
FORMULE = {
    'medal_of_honor': ('alta', 'valore'), 'navy_cross': ('alta', 'valore'), 'silver_star': ('alta', 'valore'),
    'legion_of_merit': ('alta', 'merito'), 'navy_marine_corps_medal': ('media', 'valore'),
    'bronze_star': ('media', 'valore'), 'meritorious_service': ('media', 'servizio'),
    'navy_commendation': ('media', 'servizio'), 'navy_achievement': ('bassa', 'servizio'),
    'presidential_unit_citation': ('alta', 'valore'), 'navy_unit_commendation': ('media', 'merito'),
    'meritorious_unit_commendation': ('media', 'servizio'),
    'eroe_urss': ('', 'valore'), 'lenin': ('', 'speciale'), 'bandiera_rossa': ('', 'valore'),
    'usakov_2': ('', 'valore'), 'stella_rossa': ('', 'merito'), 'servizio_patria_1': ('', 'merito'),
    'servizio_patria_2': ('', 'merito'), 'servizio_patria_3': ('', 'merito'), 'coraggio_personale': ('', 'valore'),
    'merito_combattimento': ('', 'valore'), 'distinzione_1': ('', 'merito'), 'distinzione_2': ('', 'merito'),
    'bandiera_rossa_battello': ('', 'valore'),
}

# --- missioni originali (§0.1; le C sono le stesse missioni a due giocatori) ---
# situazione, avversario, difficolta 1-5, operazione speciale, artico
_ORIG = {
    'SCENE0': ('guerra', 'regolare', 4, True, False),    # Colpisci e fuggi: Tomahawk sull'entroterra
    'SCENE1': ('pace', 'impreparato', 1, False, False),  # TORPEX '89
    'SCENE2': ('crisi', 'veterano', 2, False, False),    # Scrollatevelo di dosso
    'SCENE3': ('pace', 'regolare', 2, False, True),      # Operazioni subacquee, Poljarnyj
    'SCENE4': ('crisi', 'regolare', 3, True, False),     # Mumar Cadaver
    'SCENE5': ('crisi', 'veterano', 3, False, False),    # Fuga
    'SCENE6': ('crisi', 'regolare', 3, False, False),    # Gulasch
    'SCENE7': ('guerra', 'veterano', 4, False, False),   # Passerella
    'SCENE8': ('guerra', 'regolare', 4, False, True),    # Festa a sorpresa, Mar di Norvegia
    'SCENE9': ('guerra', 'elite', 5, False, False),      # Ritorno a casa
}


# Esiti dell'originale che dicono un fuoco senza provocazione o senza ordine (i testi del briefing: «ha lanciato armi
# senza alcuna provocazione», «ha sparato sui sottomarini sovietici senza provocazione», «ha oltrepassato i propri
# poteri»): nelle missioni senza la sala radio le ROE le fanno rispettare gli script, e la carriera li conta come un
# fuoco senza ordine (punteggio O e V, richiamo)
ESITI_FUOCO = {'SCENE2A': [3, 7], 'SCENE2B': [3], 'SCENE2C': [4, 18], 'SCENE5A': [10]}


def _tabella(d):
    return '{ ' + ', '.join(f'{k} = {_val(v)}' for k, v in d.items()) + ' }'


def _val(v):
    if isinstance(v, dict): return _tabella(v)
    if isinstance(v, (list, tuple)): return '[' + ', '.join(_val(x) for x in v) + ']'
    return valore(v)


def esporta_carriera(radice):
    r = ['# Gradi, carriera, decorazioni, soglie e motivazioni (generato da tools/pipeline/carriera.py: non '
         'modificare a mano).', '']
    r += ['[regole]', *(f'{k} = {_val(v)}' for k, v in REGOLE.items()), '']
    r += ['[punteggio]', *(f'{k} = {_val(v)}' for k, v in PUNTEGGIO.items()), '']
    r += ['[esposizione]', *(f'{k} = {_val(v)}' for k, v in ESPOSIZIONE.items()), '']
    r += ['[moltiplicatori]', *(f'{k} = {_val(v)}' for k, v in MOLTIPLICATORI.items()), '']
    for m, gradi in GRADI.items():
        for i, (gid, sigla, nome, it, nato) in enumerate(gradi):
            r += [f'[gradi.{m}."{gid}"]', f'ordine = {i}', f'sigla = {valore(sigla)}', f'nome = {valore(nome)}',
                  f'nome_it = {valore(it)}', f'nato = {valore(nato)}', '']
    for m, fasi in FASI.items():
        ids = [g[0] for g in GRADI[m]]
        for i, f in enumerate(fasi):
            assert f['grado'] in ids, f
            r += [f'[[fasi.{m}]]', *(f'{k} = {_val(v)}' for k, v in f.items()), '']
    for d, campi in DECORAZIONI.items():
        ignoti = set(campi) - set(BASE_DECORAZIONE) - {'marina', 'nome', 'nome_it', 'precedenza'}
        assert not ignoti, (d, ignoti)
        dec = {**BASE_DECORAZIONE, **campi}
        assert not dec['successiva'] or dec['successiva'] in DECORAZIONI, d
        r += [f'[decorazioni.{d}]', *(f'{k} = {_val(v)}' for k, v in dec.items()), '']
    for (m, sit), soglie in SOGLIE.items():
        for s in soglie:
            p, d, disp = s[:3]
            cond = s[3] if len(s) > 3 else ''
            assert DECORAZIONI[d]['marina'] == m, d
            r += [f'[[soglie.{m}.{sit}]]', f'punti = {p}', f'decorazione = {valore(d)}', f'dispositivo = {valore(disp)}',
                  f'condizione = {valore(cond)}', '']
    for m, soglie in REPARTO.items():
        for p, d, cond in soglie:
            r += [f'[[reparto.{m}]]', f'punti = {p}', f'decorazione = {valore(d)}', f'condizione = {valore(cond)}', '']
    for m, soglie in FINE_COMANDO.items():
        for p, d in soglie:
            r += [f'[[fine_comando.{m}]]', f'media = {p}', f'decorazione = {valore(d)}', '']
    r += ['[coraggio]', *(f'{m} = {valore(d)}' for m, d in CORAGGIO.items()), '']
    for p, it, en in LIVELLI:
        r += ['[[giudizi.livelli]]', f'punti = {p}', f'it = {valore(it)}', f'en = {valore(en)}', '']
    for v, (fi, di, fe, de) in VOCI.items():
        r += [f'[giudizi.voci.{v}]', f'forte_it = {valore(fi)}', f'debole_it = {valore(di)}',
              f'forte_en = {valore(fe)}', f'debole_en = {valore(de)}', '']
    for m, t in MOTIVAZIONI.items():
        r += [f'[motivazioni.{m}]', *(f'{k} = {_val(v)}' for k, v in t.items()), '']
    r += ['[formule]', *(f'{d} = {_val(list(v))}' for d, v in FORMULE.items()), '']
    for base, (sit, avv, diff, spec, art) in _ORIG.items():
        for suff in 'ABC':
            nome = base + suff
            if not os.path.exists(os.path.join(radice, 'data', 'missioni', nome + '.lua')): continue
            r += [f'[missioni_originali.{nome}]', f'situazione = {valore(sit)}', f'avversario = {valore(avv)}',
                  f'difficolta = {diff}', f'operazione_speciale = {valore(spec)}', f'artico = {valore(art)}',
                  f'esiti_fuoco_senza_ordine = {_val(ESITI_FUOCO.get(nome, []))}', '']
    uscita = os.path.join(radice, 'data', 'motore', 'carriera.toml')
    open(uscita, 'w').write('\n'.join(r))
    return sum(len(g) for g in GRADI.values()), len(DECORAZIONI), sum(len(s) for s in SOGLIE.values())


if __name__ == '__main__':
    print('gradi, decorazioni, soglie:', esporta_carriera(sys.argv[1] if len(sys.argv) > 1 else '.'))
