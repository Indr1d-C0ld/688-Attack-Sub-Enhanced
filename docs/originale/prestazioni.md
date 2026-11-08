# Prestazioni delle unità e delle armi nell'originale

Generato da `tools/re/prestazioni.py` dagli script delle 22 scene (`.TOK`). Per ogni classe il modello usato nel
maggior numero di scene; *modelli* = quanti modelli diversi compaiono. Velocità alla massima forza (telegrafo 280/256), virata standard,
accelerazione, rateo di quota a pieno comando. Conversioni nell'intestazione dello strumento.

## Unità

La quota massima delle navi di superficie è un residuo del modello precedente e non conta. *P colpita* è la
probabilità di subire un attacco a esito casuale (missili): (256 − difesa 3D) / 256.

| Tipo | Classe | max kn | manuale kn | quota max ft | manuale ft | virata °/s | acc. kn/s | quota ft/s | integrità | rumore 3A | sonar 3B | difesa 3D | P colpita | rumore nel manuale | modelli |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| 0x00 | Forrest Sherman Class Destroyer | 25.0 | 20 | 1500 | — | 1.44 | 0.051 | 2.2 | 100 | 12 | 11 | 0 | 100% | alto | 3 |
| 0x01 | Merchant Ship | 25.0 | 20 | 1800 | — | 1.44 | 0.051 | 2.2 | 50 | 15 | 0 | 0 | 100% | alto | 6 |
| 0x02 | Cargo Ship | 15.0 | 20 | 1800 | — | 1.44 | 0.051 | 2.2 | 75 | 15 | 0 | — | — | molto alto | 3 |
| 0x05 | Kara Class Guided Missile Cruiser | 30.0 | 34 | 1800 | — | 1.44 | 0.051 | 2.2 | 150 | 24 | 18 | 90 | 65% | superiore alla media | 1 |
| 0x06 | Kirov | 30.0 | 33 | 1800 | — | 1.44 | 0.051 | 2.2 | 200 | 24 | 18 | 90 | 65% | alto | 2 |
| 0x07 | Krivak Ii | 30.0 | — | 1800 | — | 1.44 | 0.051 | 2.2 | 100 | 24 | 18 | 90 | 65% | — | 1 |
| 0x08 | Koni Class Frigate | 25.0 | 29 | 600 | — | 1.44 | 0.051 | 2.2 | 50 | 20 | 18 | 5 | 98% | nella media | 2 |
| 0x09 | Slava Class Cruiser | 30.0 | 12 | 1800 | — | 1.44 | 0.051 | 2.2 | 175 | 22 | 18 | 90 | 65% | superiore alla media | 3 |
| 0x0a | Nimitz Class Aircraft Carrier | 15.0 | 33 | 1800 | — | 1.44 | 0.051 | 2.2 | 220 | 25 | 4 | 15 | 94% | alto | 2 |
| 0x0b | Iowa Class Battleship | 15.0 | 35 | 1800 | — | 1.44 | 0.051 | 2.2 | 175 | 15 | 8 | 17 | 93% | alto | 2 |
| 0x0c | Kidd Class Guided Missile Destroyer | 33.0 | 33 | 1800 | — | 1.44 | 0.051 | 2.2 | 150 | 15 | 8 | 16 | 94% | superiore alla media | 2 |
| 0x0d | Oliver Hazard Perry Class Frigate | 29.0 | 29 | 1800 | — | 1.44 | 0.051 | 2.2 | 100 | 15 | 8 | 15 | 94% | superiore alla media | 5 |
| 0x0e | Sacramento Class Oiler | 15.0 | 26 | 1800 | — | 1.44 | 0.051 | 2.2 | 75 | 22 | 5 | 5 | 98% | alto | 2 |
| 0x0f | Cruise Missile Target | 1.0 | — | 1500 | — | 3.6 | 0.257 | 18.0 | 10 | 0 | 18 | — | — | — | 1 |
| 0x10 | Tanker | 15.0 | 16 | 600 | — | 1.44 | 0.051 | 2.2 | 75 | 20 | 0 | — | — | molto alto | 1 |
| 0x12 | Seahawk-60B Lamps Iii Helicoptor | 70.0 | 126 | 1800 | — | 9.0 | 0.771 | 1.1 | 10 | 0 | 18 | — | — | — | 3 |
| 0x14 | Us 688 Class Attack Submarine | 37.0 | 37 | 1500 | 1500 | 3.6 | 0.257 | 18.0 | 160 | 6 | 18 | — | — | basso | 12 |
| 0x15 | Soviet Alfa Class Attack Submarine | 42.0 | 42 | 1800 | 1800 | 3.6 | 0.257 | 22.5 | 175 | 12 | 19 | — | — | superiore alla media | 12 |
| 0x17 | Victor Iii Class Fast Attack Sub | 42.0 | 32 | 1800 | 1800 | 3.6 | 0.257 | 22.5 | 160 | 10 | 18 | — | — | inferiore alla media | 3 |
| 0x19 | Biological | 1.0 | — | 1500 | — | 3.6 | 0.257 | 18.0 | 10 | 25 | 18 | — | — | — | 13 |
| 0x1c | Oscar Class (Ssgn) Cruise Missile Sub | 42.0 | 35 | 1800 | 1800 | 3.6 | 0.257 | 22.5 | 175 | 14 | 18 | 128 | 50% | nella media | 2 |
| 0x1d | Akula Class Fast Attack | 42.0 | 45 | 1800 | 1800 | 3.6 | 0.257 | 22.5 | 255 | 10 | 18 | — | — | superiore alla media | 2 |
| 0x1e | Charlie Ii Class Guided Missile Sub | 42.0 | 24 | 1800 | 1800 | 3.6 | 0.257 | 22.5 | 175 | 10 | 18 | — | — | nella media | 1 |
| 0x1f | Typhoon Class Ssbn | 42.0 | 24 | 1800 | 1800 | 3.6 | 0.257 | 22.5 | 175 | 14 | 19 | — | — | superiore alla media | 1 |
| 0x21 | Foxtrot Class Diesel Sub | 20.0 | 16 | 600 | 800 | 3.6 | 0.129 | 22.5 | 110 | 5 | 23 | — | — | basso | 1 |
| 0x22 | Trafalgar Class Fast Attack | 37.0 | 32 | 1500 | 1500 | 3.6 | 0.257 | 18.0 | 180 | 8 | 22 | — | — | basso | 1 |

## Armi del giocatore (gestori di lancio HANDLERS_C)

| Voce | Arma | Scene | corsa kn | durata s | portata nm | danno 3D | rumore 3A | testa 3B | virata °/s | quota ft/s | eventi durante la corsa |
|---|---|---|---|---|---|---|---|---|---|---|---|
| 0 (Mk 48 ADCAP) | Mark 48 Torpedo | 17 | 50.3 | 280 | 13.5 | 75 | 18 | 17 | 9.0 | 33.8 | +170 s: MSG_IF_PLAYER 39; +190 s: CANCEL, LINK 0, P_3B 13 |
| 0 (Mk 48 ADCAP) | Mark 48 Torpedo | 2 | 50.3 | 240 | 11.6 | 75 | 18 | 17 | 9.0 | 33.8 | — |
| 0 (Mk 48 ADCAP) | Mark 48 Torpedo | 1 | 54.9 | 280 | 14.8 | 75 | 18 | 17 | 9.0 | 33.8 | +170 s: MSG_IF_PLAYER 39; +190 s: CANCEL, LINK 0, P_3B 13 |
| 0 (Mk 48 ADCAP) | Mark 48 Torpedo | 1 | 50.3 | 280 | 13.5 | 75 | 18 | 17 | 9.0 | 33.8 | +170 s: MSG_IF_PLAYER 39; +190 s: CANCEL, LINK 0, P_3B 16 |
| 0 (Mk 48 ADCAP) | Spearfish Torpedo | 1 | 50.3 | 240 | 11.6 | 75 | 18 | 17 | 9.0 | 33.8 | +60 s: CANCEL, LINK 0, P_3B 13 |
| 1 (Type 53) | 533 Mm Torpedo | 11 | 45.7 | 240 | 10.5 | 100 | 18 | 15 | 9.0 | 33.8 | +150 s: MSG_IF_PLAYER 39; +170 s: CANCEL, LINK 0, P_3B 13 |
| 1 (Type 53) | 533 Mm Torpedo | 4 | 50.3 | 240 | 11.6 | 100 | 18 | 15 | 9.0 | 33.8 | — |
| 1 (Type 53) | 533 Mm Torpedo | 2 | 45.7 | 280 | 12.3 | 100 | 18 | 15 | 9.0 | 33.8 | +170 s: MSG_IF_PLAYER 39; +190 s: CANCEL, LINK 0, P_3B 13 |
| 1 (Type 53) | 533 Mm Torpedo | 2 | 50.3 | 240 | 11.6 | 100 | 18 | 15 | 9.0 | 33.8 | +150 s: MSG_IF_PLAYER 39; +170 s: CANCEL, LINK 0, P_3B 13 |
| 1 (Type 53) | 533 Mm Torpedo | 1 | 45.7 | 240 | 10.5 | 100 | 18 | 15 | 9.0 | 33.8 | +60 s: CANCEL, LINK 0, P_3B 16 |
| 1 (Type 53) | 533 Mm Torpedo | 1 | 45.7 | 200 | 8.8 | 100 | 18 | 15 | 9.0 | 33.8 | — |
| 1 (Type 53) | 450 Mm Torpedo | 1 | 45.7 | 240 | 10.5 | 40 | 18 | 15 | 9.0 | 33.8 | +60 s: CANCEL, LINK 0, P_3B 13 |
| 2 (Sea Lance) | Mark 48 Torpedo | 11 | 50.3 | 280 | 13.5 | 75 | 18 | 17 | 9.0 | 33.8 | +170 s: MSG_IF_PLAYER 39; +190 s: CANCEL, LINK 0, P_3B 13 |
| 2 (Sea Lance) | Spearfish Torpedo | 2 | 39.3 | 240 | 9.1 | 40 | 18 | 17 | 9.0 | 33.8 | +20 s: LINK 0, GOTO; +10 s: PING, AT 5 |
| 2 (Sea Lance) | Mark 50 Torpedo | 2 | 39.3 | 240 | 9.1 | 40 | 18 | 17 | 9.0 | 33.8 | +20 s: LINK 0, GOTO; +10 s: PING, AT 5 |
| 2 (Sea Lance) | Mark 48 Torpedo | 2 | 50.3 | 240 | 11.6 | 75 | 18 | 17 | 9.0 | 33.8 | — |
| 2 (Sea Lance) | Mark 48 Torpedo | 1 | 54.9 | 280 | 14.8 | 75 | 18 | 17 | 9.0 | 33.8 | +170 s: MSG_IF_PLAYER 39; +190 s: CANCEL, LINK 0, P_3B 13 |
| 2 (Sea Lance) | Mark 48 Torpedo | 1 | 50.3 | 280 | 13.5 | 75 | 18 | 17 | 9.0 | 33.8 | +170 s: MSG_IF_PLAYER 39; +190 s: CANCEL, LINK 0, P_3B 16 |
| 2 (Sea Lance) | Spearfish Torpedo | 1 | 50.3 | 240 | 11.6 | 75 | 18 | 17 | 9.0 | 33.8 | +60 s: CANCEL, LINK 0, P_3B 13 |
| 2 (Sea Lance) | Sealance Torpedo | 1 | 39.3 | 240 | 9.1 | 40 | 18 | 17 | 9.0 | 33.8 | +20 s: LINK 0, GOTO; +10 s: PING, AT 5 |
| 2 (Sea Lance) | Spearfish Torpedo | 1 | 39.3 | 240 | 9.1 | 40 | 18 | 17 | 9.0 | 33.8 | +20 s: LINK 0, GOTO; +10 s: PING, AT 8 |
| 3 (Tomahawk) | Mark 48 Torpedo | 17 | 50.3 | 280 | 13.5 | 75 | 18 | 17 | 9.0 | 33.8 | +170 s: MSG_IF_PLAYER 39; +190 s: CANCEL, LINK 0, P_3B 13 |
| 3 (Tomahawk) | Mark 48 Torpedo | 2 | 50.3 | 240 | 11.6 | 75 | 18 | 17 | 9.0 | 33.8 | — |
| 3 (Tomahawk) | — | 1 | — | — | — | — | — | — | — | — | — |
| 3 (Tomahawk) | Mark 48 Torpedo | 1 | 50.3 | 280 | 13.5 | 75 | 18 | 17 | 9.0 | 33.8 | +170 s: MSG_IF_PLAYER 39; +190 s: CANCEL, LINK 0, P_3B 16 |
| 3 (Tomahawk) | Spearfish Torpedo | 1 | 50.3 | 240 | 11.6 | 75 | 18 | 17 | 9.0 | 33.8 | +60 s: CANCEL, LINK 0, P_3B 13 |
| 4 (Harpoon) | — | 13 | — | — | — | — | — | — | — | — | +20 s: ATTACK_RND 75 |
| 4 (Harpoon) | — | 5 | — | — | — | — | — | — | — | — | +10 s: ATTACK_RND 75 |
| 4 (Harpoon) | — | 4 | — | — | — | — | — | — | — | — | +10 s: ATTACK_RND 100 |
| 5 (esca acustica) | — | 17 | 0.9 | — | — | — | 0 | — | — | — | +2 s: P_3A 40, STORE, RETARGET; +160 s: MSG_IF_PLAYER 44, SELECT_LAST |
| 5 (esca acustica) | — | 4 | 0.9 | — | — | — | 0 | — | — | — | +2 s: P_3A 40, STORE, RETARGET; +60 s: MSG_IF_PLAYER 44, SELECT_LAST |
| 5 (esca acustica) | Us 688 Class Attack Submarine | 1 | 0.9 | — | — | — | 0 | — | — | — | +2 s: P_3A 40, STORE, RETARGET; +160 s: MSG_IF_PLAYER 44, SELECT_LAST |
