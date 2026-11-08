import re, sys
p = sys.argv[1]
t = open(p, encoding='utf-8').read()
def sost(vecchio, nuovo):
    global t
    assert t.count(vecchio) == 1, vecchio[:60]
    t = t.replace(vecchio, nuovo)

# 1. il Polinom: la cupola di prua torna al rumore delle navi sovietiche, il «Horse Tail» e' un sensore a parte
sost("""    # MGK-355 «Polinom» dell'Udaloy (docs/ricerca/31 §4): la cupola «Horse Jaw» di prua lunga circa 30 m, a bassa frequenza, e
    # il «Horse Tail» a profondita' variabile. Il motore non ha il VDS (il sensore sta alla quota della nave): il pesce
    # lontano dalle turbine e' reso alla buona col rumore proprio piu' basso. Frequenze e livelli STIMA (non trovati)
    'polinom': dict(modo='passivo', antenna='sfera', dimensione_m=7.0, bande_hz=[300, 1000, 3500], integrazione_s=10,
                    indice_rilevazione=14, nl_macchina_db=56, nl_flusso_db=54, v0_flusso_kn=10, velocita_max_kn=30,
                    baffles_gradi=30, baffles_db=15, lofar_bin_hz=0.5, lofar_integrazione_s=60,
                    nota='Udaloy: MGK-355 Polinom in passivo, cupola di prua a bassa frequenza e VDS (STIMA)'),
    'polinom_attivo': dict(modo='attivo', antenna='sfera', dimensione_m=7.0, bande_hz=[3500], indice_rilevazione=14,
                           nl_macchina_db=56, nl_flusso_db=54,""",
"""    # MGK-355 «Polinom» dell'Udaloy (docs/ricerca/31 §4): la cupola «Horse Jaw» di prua lunga circa 30 m, a bassa frequenza;
    # il «Horse Tail» a profondita' variabile e' polinom_vds (docs/ricerca/33), quindi la cupola ha il rumore proprio delle
    # navi sovietiche. Frequenze e livelli STIMA (non trovati)
    'polinom': dict(modo='passivo', antenna='sfera', dimensione_m=7.0, bande_hz=[300, 1000, 3500], integrazione_s=10,
                    indice_rilevazione=14, nl_macchina_db=64, nl_flusso_db=58, v0_flusso_kn=10, velocita_max_kn=30,
                    baffles_gradi=30, baffles_db=15, lofar_bin_hz=0.5, lofar_integrazione_s=60,
                    nota='Udaloy: MGK-355 Polinom in passivo, cupola di prua «Horse Jaw» a bassa frequenza (STIMA)'),
    'polinom_attivo': dict(modo='attivo', antenna='sfera', dimensione_m=7.0, bande_hz=[3500], indice_rilevazione=14,
                           nl_macchina_db=64, nl_flusso_db=58,""")

# 2. i VDS delle navi (docs/ricerca/33): il pesce lontano dalle macchine, senza via strutturale (isolamento 45 dB) e col
# macchinario 10 dB sotto il sonar di prua; il flusso del pesce; sordo oltre la velocita' di traino. "vds" nell'id: il
# motore lo riconosce cosi' (il duomo perso non lo tocca, il pesce dentro lo spegne). Il cavo in [schiere.*] del passivo
VDS = '''    # Sonar a profondita' variabile delle navi (docs/ricerca/33): il pesce al cavo, alla quota di nucleo/schiera
    # ([schiere.<passivo>]); lontano dalle macchine: niente via strutturale (isolamento 45 dB), macchinario 10 dB sotto
    # il sonar di prua, il flusso del pesce, sordo oltre la velocita' di traino. "vds" nell'id: il motore lo riconosce
    # cosi'. Tutto STIMA salvo le quote massime e le frequenze dell'SQS-35
    'sqs35_vds': dict(modo='passivo', antenna='sfera', dimensione_m=4.8, bande_hz=[3500, 10000], integrazione_s=10,
                      indice_rilevazione=14, nl_macchina_db=50, nl_flusso_db=52, v0_flusso_kn=10, velocita_max_kn=20,
                      baffles_gradi=0, isolamento_proprio_db=45,
                      nota='Knox: AN/SQS-35 IVDS in ascolto («listen»), pesce di 4,8 m fino a 180 m (33 §1)'),
    'sqs35_vds_attivo': dict(modo='attivo', antenna='sfera', dimensione_m=4.8, bande_hz=[10000], indice_rilevazione=14,
                             nl_macchina_db=50, nl_flusso_db=52, v0_flusso_kn=10, velocita_max_kn=20, baffles_gradi=0,
                             sl_attivo_db=220, impulso_s=0.5,
                             nota='Knox: AN/SQS-35 attivo a 13 kHz (banda di 10 kHz), SL 220 (STIMA)'),
    'sqr18a': dict(modo='passivo', antenna='lineare', dimensione_m=200, bande_hz=[50, 150, 300, 1000], integrazione_s=30,
                   indice_rilevazione=14, nl_macchina_db=42, nl_flusso_db=50, v0_flusso_kn=12, velocita_max_kn=20,
                   baffles_gradi=0, virata_max_deg_s=0.3, raddrizzamento_s=150, endfire_gradi=25, conico=True,
                   lofar_bin_hz=0.1, lofar_integrazione_s=120, cavo_diametro_m=0.02,
                   nota='Knox: AN/SQR-18A TACTAS trainata dal pesce dell\\'SQS-35 (33 §1); misure STIMA'),
    'sqr19': dict(modo='passivo', antenna='lineare', dimensione_m=242, bande_hz=[50, 150, 300, 1000], integrazione_s=30,
                  indice_rilevazione=14, nl_macchina_db=40, nl_flusso_db=48, v0_flusso_kn=12, velocita_max_kn=25,
                  baffles_gradi=0, virata_max_deg_s=0.3, raddrizzamento_s=150, endfire_gradi=25, conico=True,
                  lofar_bin_hz=0.05, lofar_integrazione_s=120, cavo_diametro_m=0.0254,
                  nota='Spruance, Perry, Ticonderoga: AN/SQR-19 TACTAS dell\\'SQQ-89, 1.700-1.800 m di cavo, fino a '
                       '335-360 m (33 §1); rumore e LOFAR come le schiere USA, un po\\' sopra la TB-16 (STIMA)'),
    'polinom_vds': dict(modo='passivo', antenna='sfera', dimensione_m=4.0, bande_hz=[1000, 3500], integrazione_s=10,
                        indice_rilevazione=14, nl_macchina_db=54, nl_flusso_db=54, v0_flusso_kn=10, velocita_max_kn=20,
                        baffles_gradi=0, isolamento_proprio_db=45, lofar_bin_hz=0.5, lofar_integrazione_s=60,
                        nota='Udaloy: «Horse Tail», il VDS del Polinom (33 §1), fino a 200 m (STIMA)'),
    'polinom_vds_attivo': dict(modo='attivo', antenna='sfera', dimensione_m=4.0, bande_hz=[3500], indice_rilevazione=14,
                               nl_macchina_db=54, nl_flusso_db=54, v0_flusso_kn=10, velocita_max_kn=20, baffles_gradi=0,
                               sl_attivo_db=226, impulso_s=1.0, nota='Udaloy: «Horse Tail» attivo (STIMA)'),
    'vega_vds': dict(modo='passivo', antenna='sfera', dimensione_m=2.5, bande_hz=[3500, 10000], integrazione_s=10,
                     indice_rilevazione=14, nl_macchina_db=54, nl_flusso_db=55, v0_flusso_kn=10, velocita_max_kn=18,
                     baffles_gradi=0, isolamento_proprio_db=45,
                     nota='Kara, Krivak II: MG-325 «Vega» («Mare Tail»), a poppa, fino a 100 m (33 §1)'),
    'vega_vds_attivo': dict(modo='attivo', antenna='sfera', dimensione_m=2.5, bande_hz=[3500], indice_rilevazione=14,
                            nl_macchina_db=54, nl_flusso_db=55, v0_flusso_kn=10, velocita_max_kn=18, baffles_gradi=0,
                            sl_attivo_db=222, impulso_s=0.5, nota='MG-325 «Vega» attivo, media frequenza (STIMA)'),
    'bronza_vds': dict(modo='passivo', antenna='sfera', dimensione_m=3.0, bande_hz=[1000, 3500], integrazione_s=10,
                       indice_rilevazione=14, nl_macchina_db=54, nl_flusso_db=55, v0_flusso_kn=10, velocita_max_kn=18,
                       baffles_gradi=0, isolamento_proprio_db=45,
                       nota='Krivak III: MG-345 «Bronza» rimorchiata, come VDS (STIMA dichiarata, 33 §1: le fonti '
                            'discordano); nessuna classe del gioco per ora'),
    'shelon_vds': dict(modo='passivo', antenna='sfera', dimensione_m=1.5, bande_hz=[3500, 10000], integrazione_s=10,
                       indice_rilevazione=14, nl_macchina_db=50, nl_flusso_db=55, v0_flusso_kn=3, velocita_max_kn=3,
                       baffles_gradi=0, isolamento_proprio_db=45,
                       nota='Grisha III: «Shelon» («Elk Tail»), calabile a nave ferma (31 §5), fino a 150 m (STIMA)'),
    'shelon_vds_attivo': dict(modo='attivo', antenna='sfera', dimensione_m=1.5, bande_hz=[10000], indice_rilevazione=14,
                              nl_macchina_db=50, nl_flusso_db=55, v0_flusso_kn=3, velocita_max_kn=3, baffles_gradi=0,
                              sl_attivo_db=215, impulso_s=0.1, nota='«Shelon» attivo (STIMA)'),
'''
sost("""                                'cupola grande (STIMA; «90 km» nel capitolato)'),
}""", """                                'cupola grande (STIMA; «90 km» nel capitolato)'),
""" + VDS + "}")

# 3. le suite
sost("""    'ticonderoga': ['sqs53', 'sqs53_attivo'], 'knox': ['sqs53', 'sqs53_attivo'], 'type22': ['sqs53', 'sqs53_attivo'],
    'udaloy': ['polinom', 'polinom_attivo'], 'grisha3': ['nave_sovietica', 'titan2_attivo'],   # Platina del Grisha: come il Titan-2
    'perry': ['sqs53', 'sqs56_attivo'], 'kidd': ['sqs53', 'sqs53_attivo'], 'spruance': ['sqs53', 'sqs53_attivo'],
    'krivak2': ['nave_sovietica', 'titan2_attivo'], 'kara': ['nave_sovietica', 'titan2_attivo'],""",
"""    # i sensori fuori scafo delle navi (docs/ricerca/33): la SQR-19 sulle navi dell'SQQ-89, il VDS e la SQR-18A sulla Knox,
    # i VDS sovietici (Horse Tail, Vega, Shelon); a tutta la classe (STIMA: le date per unita' non si sono trovate)
    'ticonderoga': ['sqs53', 'sqs53_attivo', 'sqr19'], 'type22': ['sqs53', 'sqs53_attivo'],
    'knox': ['sqs53', 'sqs53_attivo', 'sqs35_vds', 'sqs35_vds_attivo', 'sqr18a'],
    'udaloy': ['polinom', 'polinom_attivo', 'polinom_vds', 'polinom_vds_attivo'],
    'grisha3': ['nave_sovietica', 'titan2_attivo', 'shelon_vds', 'shelon_vds_attivo'],   # Platina del Grisha: come il Titan-2
    'perry': ['sqs53', 'sqs56_attivo', 'sqr19'], 'kidd': ['sqs53', 'sqs53_attivo'], 'spruance': ['sqs53', 'sqs53_attivo', 'sqr19'],
    'krivak2': ['nave_sovietica', 'titan2_attivo', 'vega_vds', 'vega_vds_attivo'],
    'kara': ['nave_sovietica', 'titan2_attivo', 'vega_vds', 'vega_vds_attivo'],""")

# 4. le schiere: i pesci dei VDS come schiere corte al cavo (un solo modello), il peso del pesce spalmato sul cavo
sost("""                  fonte='04 §1.5 e §4 (S): cavo da 1 pollice, 380 m di discesa a 10 kn'),
}""", """                  fonte='04 §1.5 e §4 (S): cavo da 1 pollice, 380 m di discesa a 10 kn'),
    'sqr18a': dict(schiera_m=200, cavo_max_m=1500, cavo_diametro_m=0.02, peso_cavo_n_m=10.0, cn=1.2, v_flessione_kn=1.2,
                   filata_m_s=1.0, recupero_m_s=0.8, v_manovra_kn=12, v_max_kn=20, quota_max_m=360,
                   fonte='33 §1 e §2 (S): trainata dal pesce dell\\'SQS-35, misure non trovate'),
    # i VDS delle navi (33 §2): il pesce e' una schiera corta al cavo; il peso del pesce e del depressore e' spalmato sul
    # cavo (peso equivalente, STIMA), cosi' la legge di Pode da' la quota che sale con la velocita'. Le quote massime
    # dalle fonti (SQS-35 180 m, Vega 100 m), il resto STIMA
    'sqs35_vds': dict(schiera_m=4.8, cavo_max_m=250, cavo_diametro_m=0.04, peso_cavo_n_m=280.0, cn=1.0, v_flessione_kn=0.1,
                      filata_m_s=1.0, recupero_m_s=0.8, v_manovra_kn=12, v_max_kn=20, quota_max_m=180,
                      fonte='33 §1 (Knox-class: fino a 600 ft, 180 m); cavo, peso e velocita\\' (S)'),
    'polinom_vds': dict(schiera_m=5.0, cavo_max_m=300, cavo_diametro_m=0.04, peso_cavo_n_m=250.0, cn=1.0, v_flessione_kn=0.1,
                        filata_m_s=1.0, recupero_m_s=0.8, v_manovra_kn=12, v_max_kn=20, quota_max_m=200,
                        fonte='33 §1 e §2 (S): «Horse Tail», nessun numero trovato'),
    'vega_vds': dict(schiera_m=3.0, cavo_max_m=150, cavo_diametro_m=0.04, peso_cavo_n_m=150.0, cn=1.0, v_flessione_kn=0.1,
                     filata_m_s=1.0, recupero_m_s=0.8, v_manovra_kn=12, v_max_kn=18, quota_max_m=100,
                     fonte='33 §1 (Kara-class: fino a 100 m); cavo, peso e velocita\\' (S)'),
    'bronza_vds': dict(schiera_m=4.0, cavo_max_m=200, cavo_diametro_m=0.04, peso_cavo_n_m=200.0, cn=1.0, v_flessione_kn=0.1,
                       filata_m_s=1.0, recupero_m_s=0.8, v_manovra_kn=12, v_max_kn=18, quota_max_m=150,
                       fonte='33 §1 (S): come VDS per decisione della principale, le fonti discordano'),
    'shelon_vds': dict(schiera_m=1.5, cavo_max_m=200, cavo_diametro_m=0.03, peso_cavo_n_m=400.0, cn=1.0, v_flessione_kn=0.1,
                       filata_m_s=1.5, recupero_m_s=1.2, v_manovra_kn=0.5, v_max_kn=3, quota_max_m=150,
                       fonte='31 §5 e 33 §1 (S): calabile a nave ferma'),
}""")

# 5. le emissioni dei VDS attivi (per l'intercettazione)
sost("""    'titan2_attivo': dict(tipo='scafo', f_hz=5000, durata_s=0.5, fm=False, pri_s=15),     # STIMA 3-8 kHz
""", """    'titan2_attivo': dict(tipo='scafo', f_hz=5000, durata_s=0.5, fm=False, pri_s=15),     # STIMA 3-8 kHz
    'sqs35_vds_attivo': dict(tipo='scafo', f_hz=13000, durata_s=0.5, fm=False, pri_s=15),  # 13 kHz (33 §1)
    'polinom_vds_attivo': dict(tipo='scafo', f_hz=3000, durata_s=1.0, fm=False, pri_s=25), # STIMA
    'vega_vds_attivo': dict(tipo='scafo', f_hz=7000, durata_s=0.5, fm=False, pri_s=15),    # STIMA, media frequenza
    'shelon_vds_attivo': dict(tipo='scafo', f_hz=10000, durata_s=0.1, fm=False, pri_s=10), # STIMA, come i calabili
""")
open(p, 'w', encoding='utf-8').write(t)
print('ok')
