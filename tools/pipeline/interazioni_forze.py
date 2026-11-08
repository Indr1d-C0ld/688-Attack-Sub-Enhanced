"""Sezioni del runtime in data/motore/interazioni.toml (fase 3 di docs/18_interazioni_ia.md; dossier
docs/ricerca/24_interazioni_fra_unita.md §8.1-8.3, §8.7): allerta della forza, messaggi di contatto fra unita' dell'IA,
zone dei sottomarini amici e fuoco amico, regole d'ingaggio dell'IA. Le chiama tools/pipeline/interazioni.py, che
scrive il file; il loader e' in engine/missioni/src/missione_forze.cpp. Ogni valore ha fonte e confidenza."""

# --- allerta della forza (24 §8.1): A0 routine, A1 vigilanza, A2 minaccia, A3 combattimento. Un evento alza il livello
# della formazione che lo vive (le unita' della parte entro raggio_formazione_m da chi lo vive) e, se lo dice la
# tabella, quello del resto della forza (subito per i FLASH, altrimenti col messaggio della fase B). passo: quanto sale;
# fino_a: il livello almeno raggiunto (-1: nessuno); max: il tetto del solo passo. discesa_s: -1 ogni tanto senza
# eventi, fino al valore iniziale e mai sotto i minimi della situazione.
ALLERTA = dict(
    raggio_formazione_m=20000.0, discesa_s=2700.0, minimo_crisi=1, minimo_guerra=2,
    falsi_contatti=[1.0, 1.5, 2.0, 3.0], posto_combattimento=2, emcon_combattimento=3,
    fonte='24 §8.1 (tabella degli eventi, discesa 30-60 min, falsi contatti x1-x3); raggio della formazione STIMA',
    confidenza='bassa')
EVENTI_ALLERTA = {
    'possub': dict(formazione=1, formazione_max=2, forza=0, forza_max=0, formazione_fino_a=-1, forza_fino_a=-1),
    'probsub': dict(formazione=1, formazione_max=3, forza=1, forza_max=2, formazione_fino_a=-1, forza_fino_a=-1),
    'albero': dict(formazione=1, formazione_max=3, forza=0, forza_max=0, formazione_fino_a=-1, forza_fino_a=-1),
    'flash': dict(formazione=1, formazione_max=3, forza=1, forza_max=3, formazione_fino_a=-1, forza_fino_a=-1),
    'ping': dict(formazione=1, formazione_max=3, forza=0, forza_max=0, formazione_fino_a=-1, forza_fino_a=-1),
    'arma': dict(formazione=0, formazione_max=3, forza=1, forza_max=3, formazione_fino_a=3, forza_fino_a=-1),
    'affondato': dict(formazione=0, formazione_max=3, forza=0, forza_max=3, formazione_fino_a=3, forza_fino_a=2),
}

# --- messaggi di contatto fra unita' dell'IA (24 §8.2, §4.5, §3.4). ritardi in s [minimo, massimo]; perdita: P che il
# messaggio non arrivi; i ritardi fra formazioni sovietiche x1,5 (centralizzazione). ellisse: r(t) = r0 + u*t (17 §5.2),
# u la velocita' massima plausibile della categoria creduta; oltre ellisse_max_m resta il solo rilevamento.
MESSAGGI = {
    'usa': dict(formazione_s=[30.0, 120.0], formazione_perdita=0.02, aereo_nave_s=[60.0, 180.0], aereo_nave_riuscita=1.0,
                teatro_flash_s=[300.0, 600.0], teatro_immediate_s=[900.0, 1800.0], teatro_perdita=0.05,
                fonte='24 §8.2 (voce e Link 11, FLASH e IMMEDIATE nella scala del gioco)', confidenza='media'),
    'urss': dict(formazione_s=[60.0, 180.0], formazione_perdita=0.05, aereo_nave_s=[720.0, 720.0], aereo_nave_riuscita=0.16,
                 teatro_flash_s=[450.0, 900.0], teatro_immediate_s=[1350.0, 2700.0], teatro_perdita=0.05,
                 fonte='24 §8.2 e §4.5: KPUG; Be-12 -> Ka-25 12 min, riuscita 12/74 [ART-Be12]; teatro x1,5',
                 confidenza='media'),
}
MESSAGGI_COMUNI = dict(
    riferire_ogni_s=600.0, broadcast_ciclo_s=[600.0, 900.0], r0_m=1852.0, ellisse_max_m=10000.0,
    velocita_kn=dict(sottomarino=20.0, nave=30.0, aereo=0.0, sconosciuto=20.0),
    fonte='24 §8.2 (ciclo di broadcast 10-15 min, 10 §0); r0, velocita\' e soglia dell\'ellisse STIMA', confidenza='bassa')

# --- zone e fuoco amico (24 §8.3). Area d'azione mobile di un sottomarino amico, nella scala del gioco (17 §5.4: 10-20
# nm; storica 50/100/15 [WP-Dorado]). Dentro, navi e aerei della parte non usano armi ASW sui contatti subacquei.
ZONE = dict(
    avanti_nm=15.0, indietro_nm=30.0, lato_nm=5.0, dopo_attacco_s=3600.0, fuoco_amico_usa=0.05, fuoco_amico_urss=0.15,
    fonte='24 §8.3: zona mobile 10-20 nm nella scala del gioco, P di fuoco amico nella prima ora dopo un attacco',
    confidenza='bassa')

# --- regole d'ingaggio dell'IA (24 §4.6, §8.7; inventario §12): pace mai, crisi solo contro la parte che ha provocato,
# guerra contro le unita' militari ostili; i neutrali mai, salvo ad A3 su un contatto non classificato (P 0,3).
ROE_IA = dict(
    neutrali_a3=0.3, linea_neutrale_nm=2.0,
    fonte='24 §4.6 (legittima difesa contro chi ha fatto l\'atto), §8.7 (neutrali, P 0,3 ad A3)', confidenza='media')


def righe_forze(valore):
    """Le righe TOML delle sezioni del runtime; valore() e' quello di interazioni.py."""
    righe = ['# Fase 3 (runtime): allerta della forza, messaggi fra unita\' dell\'IA, zone e fuoco amico, ROE dell\'IA.', '']
    righe += ['[allerta]', *(f'{k} = {valore(v)}' for k, v in ALLERTA.items()), '']
    chiavi = None
    for e, d in EVENTI_ALLERTA.items():
        assert chiavi is None or set(d) == chiavi, e
        chiavi = set(d)
        righe += [f'[allerta.eventi.{e}]', *(f'{k} = {valore(v)}' for k, v in d.items()), '']
    chiavi = None
    for p, d in MESSAGGI.items():
        assert chiavi is None or set(d) == chiavi, p
        chiavi = set(d)
        righe += [f'[messaggi.{p}]', *(f'{k} = {valore(v)}' for k, v in d.items()), '']
    righe += ['[messaggi.comuni]', *(f'{k} = {valore(v)}' for k, v in MESSAGGI_COMUNI.items()), '']
    righe += ['[zone]', *(f'{k} = {valore(v)}' for k, v in ZONE.items()), '']
    righe += ['[roe_ia]', *(f'{k} = {valore(v)}' for k, v in ROE_IA.items()), '']
    return righe
