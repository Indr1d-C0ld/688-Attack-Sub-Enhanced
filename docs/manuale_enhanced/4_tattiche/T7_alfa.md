# T7. Combattere con l'Alfa

*Autore: kit. Revisione del 03/11/2026. Stato: bozza. Le portate sono misurate col motore
(`tools/re/prove/suite_acustica/sonda_tattiche/kit_t1_t7.cpp`, uscita in `kit_t1_t7.txt`): le Fær Øer a febbraio,
battelli a 150 m, vento 7 m/s. Le corse dei siluri vengono da `data/motore/siluri.toml`; i conti della fuga sono
fatti a mano su quei numeri.*

L'Alfa non vince la gara d'ascolto: è rumorosa e il suo sonar è di seconda generazione. Vince con quello che il 688
non ha: 41 nodi, un raggio di virata da tre lunghezze, il governo automatico. Si combatte di velocità e di quota,
scegliendo quando farsi sentire.

## 1. Le regole pratiche

1. **Ti sentono prima che tu senta loro.** A 5 nodi la sfera di un 688 sente l'Alfa a 17 km, la sua schiera filata a
   27. L'Alfa sente un 688 a 5 nodi solo entro un chilometro. Non provare a scoprire per primo un 688 in ascolto:
   lascia che ti scopra dove vuoi tu.
2. **Lenta o velocissima, mai a metà.** Già a 15 nodi l'Alfa si sente a 60 km, e da 20 nodi per tutto il mare;
   intanto il suo sonar non sente quasi più niente (un 688 a 5 nodi solo entro 200 m). A 15 nodi paghi tutto il rumore
   senza la velocità che ti salva. Si va piano ad ascoltare, oppure al massimo per arrivare o scappare.
3. **La velocità contro i siluri leggeri.** A 41 nodi un Mk 46 o uno Sting Ray (45 nodi, 11 km di corsa) recupera solo
   4 nodi: in tutta la corsa guadagna meno di un chilometro. Lanciato da più di un chilometro dietro di te, finisce la
   corsa prima di prenderti. Contro il Mk 48 (55 nodi, 38 km) e l'ADCAP (60 nodi, 32 km) la velocità non basta: ti
   raggiungono se parti a meno di 10 km.
4. **La virata stretta.** Tre lunghezze di diametro tattico contro le cinque del 688: in un inseguimento ravvicinato
   l'Alfa gira dentro il cerchio dell'altro. Una virata stretta a tutta forza spezza la soluzione TMA di chi ti segue,
   e il siluro che hai in coda deve rifare la ricerca.
5. **La quota.** A 400 m (la quota di collaudo dell'Alfa nel gioco) stai quasi sempre sotto lo strato, e chi ti cerca
   dall'alto ti sente male (capitolo T2). Non sei sotto i siluri della NATO: il Mk 46 arriva a 450 m, il Mk 48 a 760, lo
   Sting Ray a 800.
6. **Niente schiera filata.** L'Alfa non ha la schiera: il cono sordo di poppa è grande e nessuno lo copre. Ogni tanto
   accosta di 90° per ascoltare dietro (il «Crazy Ivan», capitolo T8).
7. **I siluri sovietici.** Il 53-65K segue la scia: contro una nave in moto non si inganna con le esche. Il SET-65 e il
   SAET-60M sono a testa passiva e senza filo: si lanciano su una soluzione buona, poi si lascia fare la testa. Il
   SAET-60M non scende sotto i 200 m: contro un battello profondo serve il SET-65.
8. **Il reticolo e il Bukhta.** Al periscopio la distanza si legge col reticolo graduato dall'altezza d'albero del
   libretto. L'ESM Bukhta (2-18 GHz) sente i radar prima che loro vedano te: alzalo appena sali a quota periscopica.

## 2. Il perché

- La firma dell'Alfa, rumorosa a ogni velocità: volume 2, C4. Il suo sonar di seconda generazione: C1 e la suite
  in `data/motore/acustica.toml` (`sovietico_2gen`).
- Il diametro tattico e la cavitazione (3 lunghezze, 4 nodi a quota zero, contro le 5 lunghezze e i 5 nodi del 688):
  B1.
- Il governo automatico (Boksit, Tan, ПОКС): volume 1, §2.6.
- I siluri, la scia e le teste: D1; le esche (l'MG-74 dell'Alfa): D2.
- Il reticolo del periscopio Signal: C9 §2.12 e volume 1, cap. 4; il Bukhta: C10.

## 3. I numeri

**L'Alfa ascoltata dal 688** (che va a 5 nodi), e **il 688 ascoltato dall'Alfa** (il 688 a 5 nodi), in km. Fra
parentesi la distanza più lontana nelle zone di convergenza; «oltre 120» vuol dire per tutta la prova.

| Nodi dell'Alfa | 5 | 10 | 15 | 20 | 25 | 30 | 35 | 40 |
|---|---|---|---|---|---|---|---|---|
| la sente il 688, sfera | 17,0 (24) | 27,5 (55) | 59,8 (100) | oltre 120 | oltre 120 | oltre 120 | oltre 120 | oltre 120 |
| la sente il 688, schiera | 27,2 (55) | 61,5 (100) | 100 (111) | oltre 120 | oltre 120 | oltre 120 | oltre 120 | oltre 120 |
| l'Alfa sente il 688 | 1,0 | 0,5 | 0,2 | 0,2 | — | — | — | — |

**La fuga dai siluri a 41 nodi** (dalle corse di `siluri.toml`, con il siluro che insegue in linea retta):

| Siluro | Velocità (nodi) | Corsa (km) | Guadagna sull'Alfa in tutta la corsa |
|---|---|---|---|
| Mk 46, Sting Ray | 45 | 11 | 1,0 km |
| Mk 48 | 55 | 38 | 9,7 km |
| Mk 48 ADCAP | 60 | 32 | 10,1 km |

Se il siluro parte più lontano di quanto guadagna, finisce la corsa prima di arrivare. Nella pratica la testa deve
anche trovarti, e la virata ti costa velocità: i numeri sono il meglio che si possa sperare.

## 4. Una scena per provarlo

Una missione rapida con l'Alfa contro un 688 (capitolo 11 del volume 1):
1. a 5 nodi, guarda quando ti scopre il 688 (con la verità accesa nella sandbox, la linea di rilevamento lo dice);
2. rifallo con lo stesso seme a 18 nodi, e guarda che sei tu a non sentire più niente;
3. da un elicottero ASW che lancia un Mk 46, scappa a 41 nodi: se il siluro cade a più di un chilometro, non ti prende.
