# I commenti degli esperimenti della spia e i due nuovi della M4 (via della principale del 05/11/2026)
D='data/motore/esperimenti/'
def r(f,a,b):
    s=open(D+f,encoding='utf-8').read()
    assert s.count(a)==1,a
    open(D+f,'w',encoding='utf-8').write(s.replace(a,b))
r('nave_spia.m688v','''# minuto e per tutta la partita, il quadro vero della NATO: dove sono il convoglio e la scorta.
''','''# minuto e per tutta la partita, il quadro vero della NATO: dove sono il convoglio e la scorta. Verso i sottomarini passa dal
# canale, come ogni rapporto (tools/re/prove/quadro_radio/proposta.md): il Victor lo copia solo a quota d'antenna, al ciclo.
''')
r('nave_spia.m688v','''mostra = "Il Victor riceve ogni minuto il quadro vero della NATO: la posizione di ogni nave del convoglio e della scorta"''',
  '''mostra = "La spia ha il quadro vero della NATO, ma il Victor lo copia solo quando sale a quota d'antenna, al ciclo del broadcast"''')
r('nave_spia.m688v','''ipotesi = "Sapendo dov'e' la scorta il Victor attacca prima e da dove le Perry non ci sono: lo scoprono piu' tardi e il convoglio perde di piu'"''',
  '''ipotesi = "Il primo lancio non cambia, perche' il primo ascolto viene dopo; il Victor sale, si espone e ha il limite delle salve: il convoglio perde poco di piu'"''')
r('spia_oltre_orizzonte.m688v','''# il quadro vero della NATO. La S5 non misura l'informazione: il Victor attacca coi siluri, e il quadro lo leggono i missili.
''','''# il quadro vero della NATO. Slava e Krivak lo ricevono subito; verso i sottomarini passa dal canale (nave_spia.m688v), e qui
# l'unico sottomarino e' il giocatore: per il quadro ai sottomarini questa scena fa da controllo. La caccia sul dato si misura nella M4.
''')
for nome,titolo,mostra,ipotesi,interventi in [
 ('quadro_m4_spia','Il Victor e la spia','Un Victor senza contatti, a 40 km da un convoglio con una Perry: la spia da\' il quadro, e lui lo copia alle finestre d\'ascolto',
  'Salito ad ascoltare, il Victor va sul dato, trova la Perry coi suoi sensori e lancia; senza dato resta dov\'e\'',
  '''tempo = 0.0
tipo = "spia"
durata_s = 0.0
testi = { parte = "urss", altra = "usa" }
'''),
 ('quadro_m4_krivak','Il Victor e la Krivak che riferisce','Una Krivak vede il convoglio e lo riferisce al teatro: il Victor riceve il rapporto dal canale vero, con la posizione gia\' vecchia',
  'Fra il Victor senza dati e quello con la spia: il dato arriva sempre vecchio, e il Victor lo segue fino a 30 km d\'ellisse',
  '''tempo = 0.0
tipo = "unita_nuova"
punto = { x = -4500.0, y = -130000.0 }
parametri = { rotta = 0.0, velocita_kn = 6.0 }
testi = { classe = "krivak2", parte = "urss" }
'''),
]:
    open(D+nome+'.m688v','w',encoding='utf-8').write(f'''# Esperimento per il quadro radio ai sottomarini (tools/re/prove/quadro_radio/proposta.md §4, M4). La scena M4: un Victor III (6)
# di pattuglia, senza limite di salve, e a 40 km un convoglio (2, 3, 4) con una Perry (5), in guerra.
[esperimento]
titolo = "{titolo}"
mostra = "{mostra}"
ipotesi = "{ipotesi}"
codice = "878RQJ-ER0000-04K000-0000HZ-G003Y0"
semi = 10
da = 1

[[intervento]]
{interventi}''')
