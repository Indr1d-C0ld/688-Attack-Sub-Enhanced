"""Achievement e albo d'onore (roadmap punto 15; dossier docs/ricerca/15 §7.10, 13 §3.3 e §3.6) ->
data/motore/achievement.toml e data/motore/albo_onore_storico.toml.
Li leggono nucleo/achievement.hpp (carica_achievement) e nucleo/albo_onore.hpp (carica_storici); il significato dei
campi e' in DefAchievement e ImpresaStorica. Le misure delle condizioni sono quelle di
missioni/achievement_missione.cpp (misure_achievement): il nome di una misura che non c'e' e' un errore al caricamento.
Uso: achievement.py <radice del progetto>   (lo chiama anche esporta_motore.py)"""
import os, sys

CATEGORIE = ['tattico', 'stealth', 'tecnico', 'storico', 'carriera', 'editor', 'nascosto']
AMBITI = ['missione', 'somma', 'carriera']   # una missione; somma sulle missioni; dal fascicolo
OPERATORI = ['>=', '>', '<=', '<', '==', '!=']

# Le misure che il nucleo calcola (missioni/achievement_missione.cpp): numeri, e le chiavi (testi) per i conti distinti
MISURE = {
    # esito
    'riuscita', 'completezza', 'successo', 'battello_perso', 'avversario',
    'affondati_ostili', 'sottomarini_affondati', 'navi_affondate', 'doppietta',
    'lanci', 'lanci_a_segno', 'tutti_a_segno', 'siluri_evitati', 'colpi_subiti', 'distanza_min_siluro_m',
    'mine_posate', 'mine_inerti',
    # esposizione
    'fantasma', 'trasmissioni', 'serie_copiate', 'serie_perse', 'rapporti_mancati', 'avvistamenti',
    'boe_vicine', 'ping_attivi', 'sottomarino_vicino', 'esposizione_periscopio_s', 'quasi_collisione_ignota',
    # tecnica
    'classificazioni_giuste', 'classificazioni_errate', 'anticipo_medio_s', 'danni_domati', 'margine_collasso',
    'foto_periscopio', 'ordini', 'ordini_mancati', 'ordini_rispettati', 'incaglio', 'armi_a_bordo',
    # contesto
    'originale', 'rapida', 'editor', 'libreria', 'artico', 'pace', 'crisi', 'guerra', 'usn', 'vmf', 'realismo',
    # fascicolo
    'qualifica_sommergibilista', 'grado_ordine', 'decorazioni_personali', 'decorazione_massima', 'missioni_carriera',
    'pensione',
    # sempre 1 (la misura dei conti di missioni)
    'uno',
    # docs/26 §2 (29/10/2026): libretto, telemetro, AOB, missili, intercettazione, fondale, navigazione, radio, mine,
    # meteo, artiglieria, ACINT
    'visive_giuste', 'telemetro_entro_5', 'aob_entro_10', 'prima_la_vista', 'affondati_missile_oth',
    'salve_neutrale_illeso', 'siluri_evitati_allarme', 'sotto_chiglia_20_s', 'navigazione_incerta', 'gps',
    'errore_finale_m', 'non_autentici_respinti', 'campi_minati_attraversati', 'mare_max', 'colpi_artiglieria_subiti',
    'righe_nuove_acint',
    # secondo giro (31/10/2026, approvato dall'utente): ghiaccio, notte, soccorso, pedinamento, mine, elicotteri, VDS,
    # libreria storica
    'emersioni_ghiaccio', 'urti_ghiaccio', 'sotto_banchisa_s', 'visive_fanali_giuste', 'visive_luci_spente',
    'tutti_salvati', 'in_coda_s', 'entro_500yd_s', 'collisioni', 'mine_scoperte', 'mine_aggirate', 'sorvoli_elicottero',
    'navi_traino_vicine', 'libreria_storica',
}
# classi_giuste: a piu' valori (le classi separate da '|'), solo per `distinti`
CHIAVI = {'missione', 'teatro', 'marina', 'codice', 'classi_giuste'}

RIUSCITA = [['successo', '>=', 1]]
SALVO = [['battello_perso', '==', 0]]


def A(nome_it, nome_en, desc_it, desc_en, categoria, ambito, misura, livelli, condizioni=(), distinti='', icona='',
      nascosto=False):
    return dict(nome_it=nome_it, nome_en=nome_en, descrizione_it=desc_it, descrizione_en=desc_en, categoria=categoria,
                ambito=ambito, misura=misura, livelli=list(livelli), condizioni=[list(c) for c in condizioni],
                distinti=distinti, icona=icona, nascosto=nascosto)


# id: scheda. livelli: una soglia (solo oro) o tre (bronzo, argento, oro) sulla misura; condizioni: tutte vere nella
# stessa missione; distinti: per l'ambito somma, conta i valori diversi di quella chiave invece di sommare la misura.
# Le soglie sono PROPOSTA di gioco (approvata dalla principale il 26/10/2026)
ACHIEVEMENT = {
    # --- tattici ---
    'primo_sangue': A('Primo sangue', 'First blood', 'Affondare un\'unita\' ostile', 'Sink a hostile unit',
                      'tattico', 'missione', 'affondati_ostili', [1]),
    'cacciatore': A('Cacciatore di sottomarini', 'Submarine hunter', 'Sottomarini ostili affondati',
                    'Hostile submarines sunk', 'tattico', 'somma', 'sottomarini_affondati', [1, 5, 20]),
    'flotta': A('Contro la flotta', 'Against the fleet', 'Navi ostili affondate', 'Hostile ships sunk',
                'tattico', 'somma', 'navi_affondate', [3, 15, 50]),
    'un_colpo': A('Un siluro, un bersaglio', 'One fish, one target', 'Tutti i lanci di una missione a segno',
                  'Every launch of a patrol on target', 'tattico', 'missione', 'tutti_a_segno', [3, 5, 8]),
    'doppietta': A('Doppietta', 'Double', 'Due unita\' ostili affondate in un minuto',
                   'Two hostile units sunk within a minute', 'tattico', 'missione', 'doppietta', [1]),
    'elusione': A('Elusione', 'Evasion', 'Siluri evitati senza colpi subiti', 'Torpedoes evaded without being hit',
                  'tattico', 'missione', 'siluri_evitati', [1, 3, 5], [['colpi_subiti', '==', 0]]),
    'completa': A('Missione compiuta', 'Mission accomplished',
                  'Tutti gli obiettivi contro un avversario regolare, veterano o d\'elite',
                  'Every objective against a regular, veteran or elite opponent', 'tattico', 'missione', 'avversario',
                  [1, 2, 3], [['completezza', '>=', 1]]),
    'posa': A('Posa di precisione', 'Precision lay', 'Almeno quattro mine posate, nessuna inerte',
              'At least four mines laid, none inert', 'tattico', 'missione', 'mine_posate', [4],
              [['mine_inerti', '==', 0]]),
    # --- stealth ---
    'fantasma': A('Fantasma', 'Ghost',
                  'Riuscita senza fix, avvistamenti ne\' ping subiti; poi senza contatto ASW; poi senza transitori ne\' '
                  'ping propri', 'Success with no fix, sighting or ping received; then no ASW contact; then no '
                  'transient or own ping', 'stealth', 'missione', 'fantasma', [1, 2, 3], RIUSCITA),
    'silenzio_radio': A('Silenzio radio', 'Radio silence', 'Riuscita senza trasmettere, con almeno una serie copiata',
                        'Success without transmitting, with at least one broadcast copied', 'stealth', 'missione',
                        'serie_copiate', [1], RIUSCITA + [['trasmissioni', '==', 0]]),
    'fra_le_boe': A('Fra le boe', 'Among the buoys', 'Boe ostili vicine, senza mai essere avvistati',
                    'Hostile buoys close aboard, never sighted', 'stealth', 'missione', 'boe_vicine', [1, 3, 6],
                    [['avvistamenti', '==', 0]] + SALVO),
    'solo_orecchie': A('Solo orecchie', 'Ears only', 'Un affondamento senza nessun ping attivo',
                       'A kill without a single active ping', 'stealth', 'missione', 'affondati_ostili', [1],
                       [['ping_attivi', '==', 0]]),
    'sovereign': A('Passaggio ravvicinato', 'Close pass', 'A meno di 900 m da un sottomarino ostile, senza essere scoperti',
                   'Within 900 m of a hostile submarine, undetected', 'stealth', 'missione', 'sottomarino_vicino', [1],
                   [['avvistamenti', '==', 0]]),
    'testa_bassa': A('Testa bassa', 'Head down', 'Missioni riuscite con meno di un minuto di periscopio fuori',
                     'Successful patrols with under a minute of periscope exposure', 'stealth', 'somma', 'uno',
                     [3, 10, 25], RIUSCITA + [['esposizione_periscopio_s', '<', 60]]),
    # --- tecnici ---
    'orecchio_fino': A('Orecchio fino', 'Golden ear', 'Classificazioni giuste, nessuna sbagliata',
                       'Correct classifications, none wrong', 'tecnico', 'missione', 'classificazioni_giuste',
                       [3, 6, 10], [['classificazioni_errate', '==', 0]]),
    'anticipo': A('Prima di tutti', 'First to know', 'Classificazione in meno di due minuti in media (almeno tre)',
                  'Classified in under two minutes on average (at least three)', 'tecnico', 'missione',
                  'classificazioni_giuste', [3], [['anticipo_medio_s', '<', 120]]),
    'controllo_danni': A('Controllo danni', 'Damage control', 'Danni domati con il battello salvo',
                         'Damage brought under control, boat saved', 'tecnico', 'somma', 'danni_domati', [1, 5, 15],
                         SALVO),
    'quota_limite': A('Al limite', 'On the edge', 'A meno del 5% dalla quota di collasso, e ritorno',
                      'Within 5% of crush depth, and back', 'tecnico', 'missione', 'uno', [1],
                      [['margine_collasso', '<', 0.05]] + SALVO),
    'fotografo': A('Fotografo', 'Photographer', 'Foto col periscopio Type 18', 'Type 18 periscope photographs',
                   'tecnico', 'somma', 'foto_periscopio', [5, 20, 60]),
    'telegrafista': A('Telegrafista', 'Radioman', 'Tutte le serie copiate e tutti i rapporti inviati',
                      'Every broadcast copied and every report sent', 'tecnico', 'missione', 'serie_copiate',
                      [2, 4, 6], [['serie_perse', '==', 0], ['rapporti_mancati', '==', 0]]),
    'alla_lettera': A('Alla lettera', 'By the book', 'Missioni con tutti gli ordini eseguiti e le ROE rispettate',
                      'Patrols with every order carried out and the ROE respected', 'tecnico', 'somma', 'uno',
                      [3, 10, 25], [['ordini', '>=', 1], ['ordini_mancati', '==', 0], ['ordini_rispettati', '>=', 1]]),
    # --- storici ---
    'originali': A('Le missioni del 1989', 'The 1989 patrols', 'Missioni originali diverse riuscite',
                   'Different original missions completed', 'storico', 'somma', 'uno', [5, 12, 22],
                   RIUSCITA + [['originale', '>=', 1]], distinti='missione'),
    'barents': A('Silenzio nel Barents', 'Silence in the Barents', 'Riuscita nel Barents senza ping ne\' trasmissioni',
                 'Success in the Barents with no ping and no transmission', 'storico', 'missione', 'uno', [1],
                 RIUSCITA + [['teatro', '==', 'barents'], ['ping_attivi', '==', 0], ['trasmissioni', '==', 0]]),
    'ghiaccio': A('Sotto il ghiaccio', 'Under the ice', 'Missioni artiche riuscite', 'Arctic patrols completed',
                  'storico', 'somma', 'uno', [1, 3, 6], RIUSCITA + [['artico', '>=', 1]]),
    'crisi': A('Sangue freddo', 'Cool head', 'Una crisi riuscita senza aprire il fuoco senza ordine',
               'A crisis handled without firing unprovoked', 'storico', 'missione', 'uno', [1],
               RIUSCITA + [['crisi', '>=', 1], ['ordini_rispettati', '>=', 1]]),
    'guerra': A('Guerra fredda, guerra calda', 'Cold war, hot war', 'Missioni di guerra riuscite',
                'Wartime patrols completed', 'storico', 'somma', 'uno', [1, 5, 10], RIUSCITA + [['guerra', '>=', 1]]),
    'due_marine': A('Dalle due parti', 'Both sides', 'Missioni riuscite col 688 e con l\'Alfa',
                    'Patrols completed with the 688 and the Alfa', 'storico', 'somma', 'uno', [2], RIUSCITA,
                    distinti='marina'),
    # --- carriera (dal fascicolo) ---
    'dolphins': A('Dolphins', 'Dolphins', 'La qualifica di sommergibilista (dolphins o VSOK)',
                  'Submarine qualification (dolphins or VSOK)', 'carriera', 'carriera', 'qualifica_sommergibilista', [1]),
    'al_comando': A('Al comando', 'In command', 'Commander o kapitan 2-go ranga', 'Commander or kapitan 2-go ranga',
                    'carriera', 'carriera', 'grado_ordine', [4]),
    'quattro_galloni': A('Quattro galloni', 'Four stripes', 'Captain o kapitan 1-go ranga',
                         'Captain or kapitan 1-go ranga', 'carriera', 'carriera', 'grado_ordine', [5]),
    'decorato': A('Decorato', 'Decorated', 'Decorazioni personali', 'Personal decorations', 'carriera', 'carriera',
                  'decorazioni_personali', [1, 5, 12]),
    'massima': A('Al valore', 'For valor',
                 'Silver Star o Bandiera Rossa; Navy Cross o Ordine di Lenin; Medal of Honor o Eroe dell\'URSS',
                 'Silver Star or Red Banner; Navy Cross or Order of Lenin; Medal of Honor or Hero of the USSR',
                 'carriera', 'carriera', 'decorazione_massima', [1, 2, 3]),
    'veterano': A('Veterano', 'Veteran', 'Missioni in carriera', 'Career patrols', 'carriera', 'carriera',
                  'missioni_carriera', [15, 30, 60]),
    'pensione': A('Fine servizio', 'Retired', 'Una carriera chiusa in pensione', 'A career ended in retirement',
                  'carriera', 'carriera', 'pensione', [1]),
    # --- editor e libreria ---
    'autore': A('Autore', 'Author', 'Missioni dell\'editor portate a termine', 'Editor missions played to the end',
                'editor', 'somma', 'uno', [1, 5, 20], [['editor', '>=', 1]]),
    'codici': A('Codici condivisi', 'Shared codes', 'Codici di missioni rapide diversi giocati',
                'Different quick-mission codes played', 'editor', 'somma', 'uno', [5, 20, 50], [['rapida', '>=', 1]],
                distinti='codice'),
    'biblioteca': A('Biblioteca', 'Library', 'Missioni della libreria diverse riuscite (41 in tutto)',
                    'Different library missions completed (41 in all)', 'editor', 'somma', 'uno', [5, 20, 41],
                    RIUSCITA + [['libreria', '>=', 1]], distinti='missione'),
    # --- docs/26 §2 (29/10/2026, approvati dall'utente): le novita' del libretto, dei missili, del fondale e della radio ---
    'occhio_libretto': A('Occhio al libretto', 'Eye on the book', 'Identificazioni visive giuste al periscopio',
                         'Correct visual identifications at the periscope', 'tecnico', 'somma', 'visive_giuste', [1, 10, 50]),
    'telemetrista': A('Telemetrista', 'Rangefinder', 'Una misura del telemetro entro il 5% della distanza vera',
                      'A rangefinder reading within 5% of the true range', 'tecnico', 'missione', 'telemetro_entro_5', [1]),
    'angolo_giusto': A('Angolo sulla prora', 'Angle on the bow', 'Un AOB entro 10 gradi del vero, mandato alla TMA',
                       'An AOB within 10 degrees of the truth, sent to the TMA', 'tecnico', 'missione', 'aob_entro_10', [1]),
    'prima_la_vista': A('Prima la vista', 'Eyes first', 'Un contatto identificato a vista prima del sonar, giusto',
                        'A contact correctly identified by eye before the sonar', 'tecnico', 'missione', 'prima_la_vista', [1]),
    'oltre_orizzonte': A('Oltre l\'orizzonte', 'Over the horizon', 'Un bersaglio affondato con un missile oltre l\'orizzonte',
                         'A target sunk with an over-the-horizon missile', 'tattico', 'missione', 'affondati_missile_oth', [1]),
    'scatola_pulita': A('Scatola pulita', 'Clean box', 'Una salva con un neutrale nella scatola, senza colpirlo',
                        'A salvo with a neutral in the box, left untouched', 'tattico', 'missione', 'salve_neutrale_illeso', [1]),
    'allarme_siluro': A('Allarme siluro', 'Torpedo alarm', 'Siluri evitati dopo l\'allarme dell\'intercettatore',
                        'Torpedoes evaded after the intercept alarm', 'tattico', 'somma', 'siluri_evitati_allarme', [1, 3, 5]),
    'sotto_chiglia': A('Sotto la chiglia', 'Under the keel', 'Oltre 10 minuti con meno di 20 m sotto la chiglia, senza toccare',
                       'Over 10 minutes with less than 20 m under the keel, without touching', 'stealth', 'missione',
                       'sotto_chiglia_20_s', [600], [['incaglio', '==', 0]]),
    'punto_nave': A('Punto nave', 'Fix', 'Finire con la navigazione incerta e l\'errore sotto i 500 m, senza GPS',
                    'Finish with uncertain navigation, error under 500 m, without GPS', 'tecnico', 'missione', 'uno', [1],
                    [['navigazione_incerta', '>=', 1], ['gps', '==', 0], ['errore_finale_m', '<', 500]]),
    'non_autentico': A('Non autenticato', 'Not authenticated', 'Un ordine radio non autenticato riconosciuto e non eseguito',
                       'An unauthenticated radio order recognised and not carried out', 'tecnico', 'missione',
                       'non_autentici_respinti', [1]),
    'campo_minato': A('Campo minato', 'Minefield', 'Un campo di mine attraversato col sonar da mine (BQS-15 o MG-70), senza danni',
                      'A minefield crossed with the mine-hunting sonar (BQS-15 or MG-70), unharmed', 'tattico', 'missione', 'campi_minati_attraversati', [1]),
    'tempesta': A('Nella tempesta', 'In the storm', 'Una missione compiuta con mare 6 o piu\'',
                  'A patrol completed in sea state 6 or more', 'tattico', 'missione', 'uno', [1],
                  RIUSCITA + [['mare_max', '>=', 6]]),
    'sotto_il_fuoco': A('Sotto il fuoco', 'Under fire', 'Missione compiuta dopo essere stati presi a cannonate',
                        'A patrol completed after taking gunfire', 'tattico', 'missione', 'uno', [1],
                        RIUSCITA + [['colpi_artiglieria_subiti', '>=', 1]]),
    'novita_acint': A('Per l\'intelligence', 'For the intelligence', 'Righe nuove nel rapporto ACINT, non spiegate dalla libreria',
                      'New lines in the ACINT report, not explained by the library', 'tecnico', 'somma', 'righe_nuove_acint',
                      [1, 5, 10]),
    # --- secondo giro (31/10/2026, approvati dall'utente): ghiaccio, notte, soccorso, pedinamento, mine, ASW aereo ---
    'polynya': A('Polynya', 'Polynya', 'Emergere nella banchisa, in acqua libera o nel ghiaccio sottile, senza urti col ghiaccio',
                 'Surface inside the pack, in open water or thin ice, without striking the ice', 'tecnico', 'missione',
                 'emersioni_ghiaccio', [1], [['urti_ghiaccio', '==', 0]]),
    'banchisa': A('Tetto bianco', 'White ceiling', 'Ore sotto la banchisa senza urti col ghiaccio',
                  'Hours under the pack without striking the ice', 'storico', 'somma', 'sotto_banchisa_s',
                  [3600, 18000, 72000], [['urti_ghiaccio', '==', 0]]),
    'fanali': A('Leggere i fanali', 'Reading the lights',
                'Identificazioni visive giuste di notte coi soli fanali, oltre la portata della sagoma',
                'Correct visual identifications at night from the lights alone, beyond silhouette range', 'tecnico',
                'somma', 'visive_fanali_giuste', [1, 5, 15]),
    'luci_spente': A('A luci spente', 'Darkened ship', 'Di notte, una nave a luci spente identificata giusta',
                     'At night, a darkened ship correctly identified', 'tecnico', 'missione', 'visive_luci_spente', [1]),
    'soccorso': A('Soccorso', 'Rescue', 'Tutti gli uomini in acqua salvati', 'Every man in the water rescued',
                  'storico', 'missione', 'tutti_salvati', [1]),
    'in_coda': A('Nei suoi baffles', 'In his baffles',
                 'Minuti in coda a un sottomarino (entro 3 nm, a poppa, col contatto), senza che lui ci classifichi',
                 'Minutes trailing a submarine (within 3 nm, astern, holding contact) without being classified',
                 'stealth', 'missione', 'in_coda_s', [1800, 3600, 7200]),
    'cinquecento': A('Cinquecento iarde', 'Five hundred yards',
                     'Cinque minuti entro 500 iarde da un sottomarino straniero, senza urto e senza essere scoperti',
                     'Five minutes within 500 yards of a foreign submarine, no collision, undetected', 'nascosto',
                     'missione', 'entro_500yd_s', [300], [['collisioni', '==', 0], ['avvistamenti', '==', 0]],
                     nascosto=True),
    'occhio_mine': A('Occhio alle mine', 'Mine eyes', 'Mine scoperte col sonar da mine, senza colpi subiti',
                     'Mines detected with the mine-hunting sonar, without being hit', 'tattico', 'somma', 'mine_scoperte',
                     [5, 20, 50], [['colpi_subiti', '==', 0]] + SALVO),
    'aggirate': A('Aggirate', 'Steered clear', 'Almeno tre mine scoperte, nessuna avvicinata a meno di 200 m dopo',
                  'At least three mines detected, none approached within 200 m afterwards', 'tattico', 'missione',
                  'mine_aggirate', [3], [['colpi_subiti', '==', 0]]),
    'sotto_rotore': A('Sotto il rotore', 'Under the rotor', 'Sorvoli di elicotteri ASW, senza avvistamenti ne\' colpi',
                      'ASW helicopter overflights, never sighted or hit', 'stealth', 'missione', 'sorvoli_elicottero',
                      [1, 3, 6], [['avvistamenti', '==', 0], ['colpi_subiti', '==', 0]]),
    'fra_le_code': A('Fra le code', 'Between the tails',
                     'Navi col VDS o la schiera filati passate entro 5 km, senza essere scoperti',
                     'Ships streaming VDS or towed array passed within 5 km, undetected', 'stealth', 'missione',
                     'navi_traino_vicine', [1, 3, 6], [['avvistamenti', '==', 0]]),
    'catalogo': A('Catalogo', 'Catalogue', 'Classi diverse classificate giuste, in tutte le partite',
                  'Different classes correctly classified, across all patrols', 'tecnico', 'somma', 'uno', [10, 30, 60],
                  distinti='classi_giuste'),
    'storie_vere': A('Le storie vere', 'True stories', 'Missioni storiche diverse della libreria riuscite (24 in tutto)',
                     'Different historical library missions completed (24 in all)', 'storico', 'somma', 'uno',
                     [5, 12, 24], RIUSCITA + [['libreria_storica', '>=', 1]], distinti='missione'),
    # --- nascosti ---
    'sasso': A('Sasso nella scarpa', 'Pebble in the shoe', 'Un incaglio, e la missione riesce lo stesso',
               'Ran aground, and still succeeded', 'nascosto', 'missione', 'incaglio', [1], RIUSCITA, nascosto=True),
    'un_pelo': A('Per un pelo', 'By a whisker', 'Un siluro ostile passato a meno di 50 m',
                 'A hostile torpedo passed within 50 m', 'nascosto', 'missione', 'uno', [1],
                 [['distanza_min_siluro_m', '<', 50]] + SALVO, nascosto=True),
    'ultimo_siluro': A('L\'ultimo siluro', 'The last fish', 'Riuscita con nessuna arma rimasta a bordo',
                       'Success with no weapon left aboard', 'nascosto', 'missione', 'uno', [1],
                       RIUSCITA + [['armi_a_bordo', '==', 0]], nascosto=True),
    'pendolare': A('Pendolare', 'Commuter', 'Un passaggio ravvicinato con un\'unita\' che non sapevamo li\'',
                   'A near miss with a unit we did not know was there', 'nascosto', 'missione',
                   'quasi_collisione_ignota', [1], nascosto=True),
}

# Comandanti storici (dossier 15 §7.10): imprese reali documentate dal dossier 13 §3.3 e §3.6, senza punteggi e senza
# frasi attribuite alle persone; solo fatti, con fonte e confidenza
STORICI = {
    'nautilus_1958': dict(marina='usn', anno=1958, battello='USS Nautilus (SSN-571)', comandante='W. R. Anderson',
                          impresa_it='Primo passaggio sotto il Polo Nord', impresa_en='First transit under the North Pole',
                          decorazione_it='Legion of Merit; all\'equipaggio la Presidential Unit Citation',
                          fonte='dossier 13 §3.3 [WP-Anderson], [WP-PUC]', confidenza='alta'),
    'triton_1960': dict(marina='usn', anno=1960, battello='USS Triton (SSRN-586)', comandante='E. L. Beach',
                        impresa_it='Prima circumnavigazione del globo in immersione',
                        impresa_en='First submerged circumnavigation',
                        decorazione_it='Legion of Merit; all\'equipaggio la Presidential Unit Citation',
                        fonte='dossier 13 §3.3 [UCSB-Beach], [WP-PUC]', confidenza='alta'),
    'halibut_1968': dict(marina='usn', anno=1968, battello='USS Halibut (SSN-587)', comandante='',
                         impresa_it='Ricerche e operazioni speciali; motivazione volutamente vaga nel 1972',
                         impresa_en='Search and special operations; deliberately vague citation in 1972',
                         decorazione_it='Presidential Unit Citation (1968 e 1972)',
                         fonte='dossier 13 §3.3 [WP-Halibut]', confidenza='media'),
    'parche_1979': dict(marina='usn', anno=1979, battello='USS Parche (SSN-683)', comandante='',
                        impresa_it='Operazioni speciali dal 1979: la nave piu\' decorata della storia americana',
                        impresa_en='Special operations from 1979: the most decorated US ship',
                        decorazione_it='9 Presidential Unit Citation, 10 Navy Unit Commendation, 13 Navy Expeditionary Medal',
                        fonte='dossier 13 §3.3 [WP-Parche]', confidenza='media'),
    'k3_1962': dict(marina='vmf', anno=1962, battello='K-3 «Leninskij Komsomol»', comandante='L. M. Žil\'cov',
                    impresa_it='Il primo sottomarino nucleare sovietico al Polo Nord',
                    impresa_en='First Soviet nuclear submarine at the North Pole',
                    decorazione_it='Eroe dell\'Unione Sovietica',
                    fonte='dossier 13 §3.6 [WP-Zhiltsov]', confidenza='alta'),
    'k178_1964': dict(marina='vmf', anno=1964, battello='K-178', comandante='I. R. Dubjaga, A. P. Michajlovskij',
                      impresa_it='Primo transito artico di gruppo da Zapadnaja Lica alla Kamčatka',
                      impresa_en='First group Arctic transit from Zapadnaya Litsa to Kamchatka',
                      decorazione_it='Eroi dell\'Unione Sovietica', fonte='dossier 13 §3.6 [NM-K178]',
                      confidenza='media'),
    'k325_1978': dict(marina='vmf', anno=1978, battello='K-325', comandante='V. P. Lušin',
                      impresa_it='Transito sotto il Polo; nessuna ricompensa trovata',
                      impresa_en='Transit under the Pole; no award found', decorazione_it='',
                      fonte='dossier 13 §3.6 [CYCLO-Lushin]', confidenza='media'),
    'k8_1970': dict(marina='vmf', anno=1970, battello='K-8', comandante='V. B. Bessonov',
                    impresa_it='Prove di armi; il battello si perde nel golfo di Biscaglia',
                    impresa_en='Weapons trials; the boat was lost in the Bay of Biscay',
                    decorazione_it='Bandiera Rossa; Eroe dell\'Unione Sovietica postumo',
                    fonte='dossier 13 §3.6 [WP-Bessonov]', confidenza='alta'),
    # dalle missioni storiche della libreria (31/10/2026): le fonti sono le pagine di Wikipedia lette quel giorno
    'k219_1986': dict(marina='vmf', anno=1986, battello='K-219', comandante='I. A. Britanov',
                      impresa_it='Dopo l\'esplosione nel tubo 6 il marinaio S. A. Preminin spegne a mano il secondo reattore '
                                 'e muore nel compartimento; il battello affonda il 6 ottobre',
                      impresa_en='After the explosion in tube 6 seaman S. A. Preminin shuts down the second reactor by hand '
                                 'and dies in the compartment; the boat sinks on 6 October',
                      decorazione_it='A Preminin la Stella Rossa postuma (1987) e, nel 1997, Eroe della Federazione Russa',
                      fonte='[WP-K219], [WP-Preminin]', confidenza='alta'),
    'k278_1989': dict(marina='vmf', anno=1989, battello='K-278 «Komsomolec»', comandante='E. A. Vanin',
                      impresa_it='Incendio a poppa a 335 m: il battello emerge e affonda dopo circa quattro ore; 27 salvati '
                                 'su 69, molti morti di freddo nell\'acqua a 2 °C',
                      impresa_en='Fire aft at 335 m: the boat surfaced and sank about four hours later; 27 of 69 saved, '
                                 'many died of cold in water at 2 °C',
                      decorazione_it='Ordine della Bandiera Rossa all\'equipaggio', fonte='[WP-K278]', confidenza='alta'),
}


def valore(v):
    if isinstance(v, bool):
        return 'true' if v else 'false'
    if isinstance(v, (int, float)):
        return repr(float(v)) if isinstance(v, float) else str(v)
    if isinstance(v, str):
        return '"' + v.replace('\\', '\\\\').replace('"', '\\"') + '"'
    if isinstance(v, list):
        return '[' + ', '.join(valore(x) for x in v) + ']'
    raise TypeError(v)


def esporta_achievement(radice):
    righe = ['# Achievement del remake (generato da tools/pipeline/achievement.py: non modificare a mano).', '']
    for a, s in ACHIEVEMENT.items():
        assert s['categoria'] in CATEGORIE and s['ambito'] in AMBITI, a
        assert s['misura'] in MISURE, (a, s['misura'])
        assert len(s['livelli']) in (1, 3) and s['livelli'] == sorted(s['livelli']), a
        assert not s['distinti'] or (s['distinti'] in CHIAVI and s['ambito'] == 'somma'), a
        for m, op, v in s['condizioni']:
            assert op in OPERATORI and (m in MISURE or (m in CHIAVI and isinstance(v, str))), (a, m)
        righe.append(f'[achievement.{a}]')
        for k, v in s.items():
            if k == 'icona':
                v = v or a   # l'id del medaglione (grafica): di norma l'id dell'achievement
            righe.append(f'{k} = {valore(v)}')
        righe.append('')
    open(os.path.join(radice, 'data', 'motore', 'achievement.toml'), 'w').write('\n'.join(righe))
    righe = ['# Comandanti storici dell\'albo d\'onore (generato da tools/pipeline/achievement.py: non modificare a '
             'mano). Fatti documentati, senza punteggi (dossier 15 §7.10).', '']
    for i, s in STORICI.items():
        righe += [f'[storici.{i}]', *(f'{k} = {valore(v)}' for k, v in s.items()), '']
    open(os.path.join(radice, 'data', 'motore', 'albo_onore_storico.toml'), 'w').write('\n'.join(righe))
    return len(ACHIEVEMENT), len(STORICI)


if __name__ == '__main__':
    print('achievement, comandanti storici:', esporta_achievement(sys.argv[1] if len(sys.argv) > 1 else '.'))
