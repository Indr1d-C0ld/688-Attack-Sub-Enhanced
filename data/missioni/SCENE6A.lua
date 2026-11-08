-- SCENE6A: missione originale convertita da tools/re/missioni_lua.py (non modificare a mano).
-- Formato: docs/09_schema_missioni.md. Comportamenti al livello classico (API m688 = operazioni della VM).

local script = {}

-- parametro i del comando del giocatore (p), oppure il valore scritto nello script originale
local function par(p, i, predefinito) if p ~= nil and p[i] ~= nil then return p[i] end return predefinito end

local missione = {
  formato = 1,
  id = "SCENE6A",
  missione = 6,
  titolo = { it = "Gulasch", en = "GOULASH" },
  lato = "usa",
  teatro = "otranto",
  posa = { metodo = "georef", centro = { lat = 40.4355, lon = 18.6780 }, metri_per_unita = { x = 21.522, y = 21.703 }, moto_classico_m_s_per_nodo = 1.0388, affidabilita = "media", nota = "carta originale collocata sul riquadro trovato da georef_surf.py" },
  data = nil, ora_utc = nil,
  ambiente = { termoclini_m = { 76.2, 152.4, 243.84 } },
  dotazione = { mk48 = 20, harpoon = 4, esca = 6 },
  tubi = { "mk48", "mk48", "mk48", "mk48", "esca", "esca", "harpoon" },
  armi = {
    mk48 = { classe = "mk48", velocita_kn = 50.2857, corsa_s = 280, filo_s = 190, danno = 75, rumore = 18, testa = 17, virata_gradi_s = 9, quota_max_m = 548.64, gestore = "L_05f5" },
    harpoon = { gestore = "L_0588" },   -- senza modello di corsa (missile o esca: vedi lo script)
    esca = { gestore = "L_06a0" },   -- senza modello di corsa (missile o esca: vedi lo script)
  },
  gestori_giocatore = {
    punto_di_rotta = "L_0703",
    telegrafo = "L_0715",
    quota = "L_071a",
    timone = "L_0721",
    pilota_automatico_off = "L_077d",
    periscopio = "L_0785",
    carica_tubo_1 = "L_0729",
    carica_tubo_2 = "L_0732",
    carica_tubo_3 = "L_073b",
    carica_tubo_4 = "L_0744",
    carica_tubo_5 = "L_074d",
    carica_tubo_6 = "L_0756",
    carica_tubo_7 = "L_075f",
    schiera_cala = "L_0789",
    schiera_recupera = "L_0799",
    schiera_persa = "L_07a5",
    designa_bersaglio = "L_07af",
    posizione = "L_07b4",
    antenna = "L_078f",
    guida_e0 = "L_06d6",
    guida_e1 = "L_06db",
    guida_e2 = "L_06e5",
  },
  copertura = 1,
  varianti = {
    { probabilita = 1, probabilita_originale = "1", unita = {
      { id = 0, classe = "los_angeles", tipo_originale = 20, nome_originale = "US 688 CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "usa", flag = 9, tipo = 1, giocatore = true,
        posizione = { x = 8558.4459, y = -53652.3958 }, originale = { x = 5792, y = 3616 },
        quota_m = 6.096, rotta = 90, velocita_kn = 0,
        modello = { massima_kn = 37.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 160, rumore = 10, sonar = 22, difesa = nil, schiera_trainata = false, controllo_fondale = true, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 1, classe = "biologico", tipo_originale = 25, nome_originale = "BIOLOGICAL", categoria = "nessuna", schieramento = "neutrale", flag = 192, tipo = 1, attributi = { "non_combattente", "falso_bersaglio" },
        posizione = { x = -54835.5022, y = -35394.6957 }, originale = { x = 2880, y = 4448 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 1.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 5.4864, quota_max_m = 457.2, integrita = 10, rumore = 25, sonar = 22, difesa = nil, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 1 },
        gestori = { bersaglio = nil, reazione = nil, arrivo = nil } },
      { id = 2, classe = "alfa", tipo_originale = 21, nome_originale = "SOVIET ALFA CLASS ATTACK SUBMARINE", categoria = "sottomarino", schieramento = "urss", flag = 17, tipo = 1,
        posizione = { x = 37942.3574, y = -66063.0093 }, originale = { x = 7136, y = 3040 },
        quota_m = 45.72, rotta = 0, velocita_kn = 0,
        modello = { massima_kn = 42.0, virata_gradi_s = 3.6, accel_kn_s = 0.257, quota_m_s = 6.858, quota_max_m = 548.64, integrita = 175, rumore = 12, sonar = 17, difesa = nil, schiera_trainata = true, controllo_fondale = true, periodo_controlli = 1 },
        rotta_punti = { { x = 17584.4512, y = -32798.5666 }, { x = 5746.6299, y = -28649.5858 }, { x = -516.4781, y = -46711.4338 }, { x = 17584.4512, y = -32798.5666 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0208", reazione = nil, arrivo = "L_0431" } },
      { id = 3, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 20, attributi = { "non_combattente" },
        posizione = { x = -1214.842, y = -52268.0594 }, originale = { x = 5344, y = 3680 },
        quota_m = 0, rotta = 0, velocita_kn = 8.2286,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 110, rumore = 18, sonar = 0, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 5 },
        rotta_punti = { { x = 876.136, y = -27262.5449 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_04ec" } },
      { id = 4, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 20, attributi = { "non_combattente" },
        posizione = { x = 879.1743, y = -52962.6917 }, originale = { x = 5440, y = 3648 },
        quota_m = 0, rotta = 0, velocita_kn = 8.2286,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 110, rumore = 18, sonar = 0, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 5 },
        rotta_punti = { { x = 876.136, y = -27262.5449 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_04ec" } },
      { id = 5, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 20, attributi = { "non_combattente" },
        posizione = { x = -1913.3553, y = -54351.665 }, originale = { x = 5312, y = 3584 },
        quota_m = 0, rotta = 0, velocita_kn = 8.2286,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 110, rumore = 18, sonar = 0, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 5 },
        rotta_punti = { { x = 876.136, y = -27262.5449 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_04ec" } },
      { id = 6, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 20, attributi = { "non_combattente" },
        posizione = { x = 1577.6587, y = -55046.3267 }, originale = { x = 5472, y = 3552 },
        quota_m = 0, rotta = 0, velocita_kn = 8.2286,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 110, rumore = 18, sonar = 0, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 5 },
        rotta_punti = { { x = 876.136, y = -27262.5449 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_04ec" } },
      { id = 7, classe = "cargo", tipo_originale = 2, nome_originale = "CARGO SHIP", categoria = "nave", schieramento = "usa", flag = 74, tipo = 20, attributi = { "non_combattente" },
        posizione = { x = -517.1531, y = -56435.6394 }, originale = { x = 5376, y = 3488 },
        quota_m = 0, rotta = 0, velocita_kn = 8.2286,
        modello = { massima_kn = 25.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 110, rumore = 18, sonar = 0, difesa = 0, schiera_trainata = false, controllo_fondale = false, periodo_controlli = 5 },
        rotta_punti = { { x = 876.136, y = -27262.5449 } }, rotta_ciclica = false,
        gestori = { bersaglio = nil, reazione = nil, arrivo = "L_04ec" } },
      { id = 8, classe = "kirov", tipo_originale = 6, nome_originale = "KIROV", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 40047.608, y = -67441.148 }, originale = { x = 7232, y = 2976 },
        quota_m = 0, rotta = 0, velocita_kn = 20.1143,
        modello = { massima_kn = 30.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 200, rumore = 24, sonar = 18, difesa = 90, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 17584.4512, y = -32798.5666 }, { x = 5746.6299, y = -28649.5858 }, { x = -516.4781, y = -46711.4338 }, { x = 17584.4512, y = -32798.5666 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0315", reazione = nil, arrivo = "L_0431" } },
      { id = 9, classe = "slava", tipo_originale = 9, nome_originale = "SLAVA CLASS CRUISER", categoria = "nave", schieramento = "urss", flag = 18, tipo = 2,
        posizione = { x = 39355.5198, y = -68834.0027 }, originale = { x = 7200, y = 2912 },
        quota_m = 0, rotta = 0, velocita_kn = 20.1143,
        modello = { massima_kn = 30.0, virata_gradi_s = 1.44, accel_kn_s = 0.051, quota_m_s = 0.6706, quota_max_m = 548.64, integrita = 175, rumore = 22, sonar = 18, difesa = 90, schiera_trainata = true, controllo_fondale = false, periodo_controlli = 1 },
        rotta_punti = { { x = 17584.4512, y = -32798.5666 }, { x = 5746.6299, y = -28649.5858 }, { x = -516.4781, y = -46711.4338 }, { x = 17584.4512, y = -32798.5666 } }, rotta_ciclica = true,
        gestori = { bersaglio = "L_0315", reazione = nil, arrivo = "L_0431" } },
      },
      timer = { { dopo_s = 200, unita = 0, gestore = "L_01c6" }, { dopo_s = 240, assoluto = true, unita = 0, gestore = "L_01b8" }, { dopo_s = 30, unita = 1, gestore = "L_0112" }, { dopo_s = 50, unita = 2, gestore = "L_01ad" }, { dopo_s = 30, unita = 2, gestore = "L_02f2" }, { dopo_s = 50, unita = 8, gestore = "L_01a6" }, { dopo_s = 70, unita = 9, gestore = "L_01a6" } },
      variabili = { [1] = 0, [2] = 0, [3] = 0 },
    },
  },
  briefing = {
    it = { "ORDINI DI MISSIONE", "DA: CINCLANTFLT", "A: USS LOS ANGELES (SSN 688)", "Comandante, la NATO ha risposto positivamente alla richiesta di aiuto del governo jugoslavo. Possiamo procedere con i nostri piani per fornire aiuti militari agli jugoslavi. Al momento i russi hanno solo due unità da guerra nella zona: secondo l'intelligence stanno pattugliando l'imboccatura dell'Adriatico. Il suo compito è scortare le navi da carico verso nord fino a Tivat e proteggerle dall'aggressione sovietica con ogni mezzo a sua disposizione. Se incontra le unità da guerra, le raccomandiamo di impiegare più missili Harpoon: questo dovrebbe limitare la loro capacità di chiedere supporto aereo. Buona fortuna, e che Dio la benedica." },
    en = { "MISSION ORDERS", "FM: CINCLANTFLT", "TO: USS LOS ANGELES (SSN 688)", "CAPTAIN, NATO HAS ANSWERED POSITIVELY TO THE YUGOSLAVIAN GOVERNMENT'S PLEA FOR ASSISTANCE. WE MAY PROCEED WITH OUR PLANS TO PROVIDE MILITARY AID TO THE YUGOSLAV'S. AT THIS MOMENT THE RUSSIANS HAVE ONLY TWO WARSHIPS IN THE AREA. INTELLIGENCE REPORTS THAT THE WARSHIPS ARE CURRENTLY PATROLLING THE MOUTH OF THE ADRIATIC. YOUR ASSIGNMENT IS TO ESCORT THE CARGO SHIPS NORTH TO TRIVAT AND PROTECT THEM FROM SOVIET AGGRESSION BY WHATEVER MEANS AVAILABLE TO YOU. SHOULD YOU ENCOUNTER THE WARSHIPS, WE RECOMMEND THE USE OF MULTIPLE HARPOON MISSILES. THIS SHOULD LIMIT THE WARSHIP'S ABILITY TO CALL FOR AIRBORNE ASSISTANCE. GOOD LUCK AND GOD BLESS." },
  },
  esiti = {
    [1] = { it = "CODICE: 6-1-ACP\nL'USS Los Angeles è andato distrutto durante una manovra. Sembra che il Los Angeles si sia incagliato e che l'intero equipaggio sia andato perduto. Abbiamo avviato un'indagine completa sull'incidente. Le nostre condoglianze vanno ai familiari e agli amici dell'equipaggio del Los Angeles.", en = "CODE: 6-1-ACP\nTHE USS LOS ANGELES WAS DESTROYED WHILE ON MANEUVERS. IT APPEARS THE LOS ANGELES RAN AGROUND AND ALL HANDS WERE LOST. WE HAVE INITIATED A FULL INVESTIGATION OF THE INCIDENT. OUR CONDOLENCES GO OUT TO FAMILY AND FRIENDS OF THE CREW OF THE LOS ANGELES." },
    [2] = { it = "CODICE: 6-2-ACP\nL'USS Los Angeles è andato perduto nel Mare Adriatico, al largo delle coste italiane. Sono autorizzato a rivelare che il Los Angeles era impegnato in un pattugliamento di routine e non aveva ricevuto l'ordine di entrare nella zona di guerra dichiarata dai sovietici. A tutti coloro che conoscevano l'equipaggio del Los Angeles va la nostra più sentita solidarietà.", en = "CODE: 6-2-ACP\nTHE USS LOS ANGELES WAS LOST IN THE ADRIATIC SEA OFF THE COAST OF ITALY. AT THIS TIME I AM AUTHORIZED TO DISCLOSE THAT THE LOS ANGELES WAS ON A ROUTINE PATROL AND WAS NOT INSTRUCTED TO ENTER THE SOVIET'S DECLARED WAR ZONE. TO ALL THOSE WHO KNEW THE CREW OF THE LOS ANGELES, WE EXTEND OUR MOST HEARTFELT SYMPATHY." },
    [3] = { it = "CODICE: 6-3-ACP\nComandante, rientri immediatamente in porto e fornisca a questo ufficio un rapporto completo entro le prossime 24 ore. Lei e i suoi ufficiali sarete miei ospiti mentre mi spiegherete esattamente cosa stavate cercando di combinare là fuori. Dove stavate andando? Si porti dietro la sua roba, perché resterà qui per il prossimo mese.", en = "CODE: 6-3-ACP\nCAPTAIN, YOU ARE TO RETURN TO PORT IMMEDIATELY AND PROVIDE THIS OFFICE WITH A FULL REPORT WITHIN THE NEXT 24 HOURS. CAPTAIN, YOU AND YOUR OFFICERS WILL BE MY GUESTS AS YOU EXPLAIN TO ME EXACTLY WHAT YOU WERE TRYING TO PULL OFF OUT THERE. WHERE WERE YOU GOING? BE SURE TO BRING YOUR GEAR WITH YOU, BECAUSE YOU WILL BE HERE FOR THE NEXT MONTH." },
    [4] = { it = "CODICE: 6-4-ACP\nBel lavoro, Comandante. La sua prestazione in questa missione sarà annotata nel suo stato di servizio. Grazie ai nostri sforzi, il Fronte Rivoluzionario Armato Jugoslavo (YARF) ha i mezzi per affrontare la forza d'invasione sovietica. Rientri immediatamente in porto: abbiamo un altro carico da affidarle.", en = "CODE: 6-4-ACP\nFINE WORK, CAPTAIN. YOUR PERFORMANCE ON THIS MISSION SHALL GO DOWN ON YOUR SERVICE RECORD. BECAUSE OF OUR EFFORTS THE YUGOSLAVIAN ARMED REVOLUTIONARY FRONT (YARF) IS EQUIPPED TO DEAL WITH THE SOVIET INVASION FORCE. RETURN TO PORT IMMEDIATELY, WE HAVE ANOTHER SHIPMENT FOR YOU TO HANDLE." },
    [5] = { it = "CODICE: 6-5-ACP\nBel lavoro, Comandante. La sua prestazione in questa missione sarà annotata nel suo stato di servizio. Grazie ai nostri sforzi, il Fronte Rivoluzionario Armato Jugoslavo (YARF) ha i mezzi per affrontare la forza d'invasione sovietica. Rientri immediatamente in porto: abbiamo un altro carico da affidarle.", en = "CODE: 6-5-ACP\nFINE WORK, CAPTAIN. YOUR PERFORMANCE ON THIS MISSION SHALL GO DOWN ON YOUR SERVICE RECORD. BECAUSE OF OUR EFFORTS THE YUGOSLAVIAN ARMED REVOLUTIONARY FRONT (YARF) IS EQUIPPED TO DEAL WITH THE SOVIET INVASION FORCE. RETURN TO PORT IMMEDIATELY, WE HAVE ANOTHER SHIPMENT FOR YOU TO HANDLE." },
    [7] = { it = "CODICE: 6-7-ACP\nBel lavoro, Comandante, ma non è bastato a impedire alla minaccia sovietica di distruggere i nostri regali. Sembra che un rimorchiatore sarebbe stato altrettanto efficace come scorta per quelle navi da carico. La prossima volta che le affido una scorta, farà bene a portare le navi in porto sane e salve, o farò in modo che finisca sommerso fino alle mutande di bucce di patate!", en = "CODE: 6-7-ACP\nFINE WORK CAPTAIN, BUT IT WASN'T ENOUGH TO KEEP THE SOVIET MENACE FROM DESTROYING OUR GIFTS. IT SEEMS THAT A TUG BOAT WOULD HAVE BEEN AS EFFECTIVE IN ESCORTING THOSE CARGO SHIPS. THE NEXT TIME I HAVE YOU ESCORT OUR SHIPS, YOU HAD BETTER SEE THEM INTO PORT SAFELY OR I'LL SEE THAT YOU'RE UP TO YOUR SKIVVIES IN POTATO PEELINGS!" },
    [8] = { it = "CODICE: 6-8-ACP\nBel lavoro, Comandante. La sua prestazione in questa missione sarà annotata nel suo stato di servizio. Grazie ai nostri sforzi, il Fronte Rivoluzionario Armato Jugoslavo (YARF) ha i mezzi per affrontare la forza d'invasione sovietica. Rientri immediatamente in porto: abbiamo un altro carico da affidarle. Ho anche una decorazione da consegnare ai suoi uomini.", en = "CODE: 6-8-ACP\nFINE WORK, CAPTAIN. YOUR PERFORMANCE ON THIS MISSION SHALL GO DOWN ON YOUR SERVICE RECORD. BECAUSE OF OUR EFFORTS THE YUGOSLAVIAN ARMED REVOLUTIONARY FRONT (YARF) IS EQUIPPED TO DEAL WITH THE SOVIET INVASION FORCE. RETURN TO PORT IMMEDIATELY, WE HAVE ANOTHER SHIPMENT FOR YOU TO HANDLE. I ALSO HAVE AN AWARD PRESENTATION TO MAKE TO YOUR MEN." },
    [9] = { it = "CODICE: 6-9-ACP\nBel lavoro, Comandante. La sua prestazione in questa missione sarà annotata nel suo stato di servizio. Grazie ai nostri sforzi, il Fronte Rivoluzionario Armato Jugoslavo (YARF) ha i mezzi per affrontare la forza d'invasione sovietica. Rientri immediatamente in porto: abbiamo un altro carico da affidarle. Ho anche una decorazione da consegnare ai suoi uomini.", en = "CODE: 6-9-ACP\nFINE WORK, CAPTAIN. YOUR PERFORMANCE ON THIS MISSION SHALL GO DOWN ON YOUR SERVICE RECORD. BECAUSE OF OUR EFFORTS THE YUGOSLAVIAN ARMED REVOLUTIONARY FRONT (YARF) IS EQUIPPED TO DEAL WITH THE SOVIET INVASION FORCE. RETURN TO PORT IMMEDIATELY, WE HAVE ANOTHER SHIPMENT FOR YOU TO HANDLE. I ALSO HAVE AN AWARD PRESENTATION TO MAKE TO YOUR MEN." },
    [19] = { it = "CODICE: 6-19-ACP\n********** EMERGENZA **********\nInformazioni recenti indicano che il comandante è responsabile di un attacco a unità amiche. Il comandante in seconda assuma il comando del battello e rientri immediatamente in porto. Comandante, lei è tenuto a fornire a questo ufficio un rapporto completo entro le prossime 24 ore. Sgomberi il suo alloggio, Comandante. Non tollererò che lei comandi uno dei miei sottomarini!", en = "CODE: 6-19-ACP\n********** EMERGENCY **********\nRECENT INFORMATION INDICATES THAT THE CAPTAIN WAS RESPONSIBLE FOR AN ATTACK ON FRIENDLY VESSELS. THE XO IS TO TAKE COMMAND OF THE SHIP, AND RETURN TO PORT IMMEDIATELY. CAPTAIN, YOU ARE TO PROVIDE THIS OFFICE WITH A FULL REPORT WITHIN THE NEXT 24 HOURS. CLEAN OUT YOUR QUARTERS CAPTAIN. I WILL NOT HAVE YOU COMMANDING ONE OF MY SUBS!" },
  },
  classi_mancanti = { "biologico", "mk48" },
  -- punti spostati nell'acqua piu' vicina (fondale >= 10 m, sottomarini 25 m) e quote ridotte a fondale - 10 m
  correzioni = { unita = 0, punti = 0, spostamento_max_m = 0, non_risolte = 0 },
  script = script,
}

script.L_0112 = function(u)
  ::L_0112::
  m688.nuovo_caso(u)
  m688.posizione(u, 23912.3293, -52229.6448)
  m688.rotta_ora(u, 22)
  if m688.caso(u, 0.082) then goto L_019f end
  m688.posizione(u, 19040.6952, -57800.4242)
  m688.rotta_ora(u, 64)
  if m688.caso(u, 0.1602) then goto L_019f end
  m688.posizione(u, 27483.8758, -74443.8454)
  m688.rotta_ora(u, 84)
  if m688.caso(u, 0.2383) then goto L_019f end
  m688.posizione(u, 40066.2077, -70913.9551)
  m688.rotta_ora(u, 104)
  if m688.caso(u, 0.3164) then goto L_019f end
  m688.posizione(u, 20416.7389, -50156.3111)
  m688.rotta_ora(u, 124)
  if m688.caso(u, 0.3945) then goto L_019f end
  m688.posizione(u, 5768.1641, -56433.4182)
  m688.rotta_ora(u, 145)
  if m688.caso(u, 0.4727) then goto L_019f end
  m688.posizione(u, 1579.4224, -63381.2604)
  m688.rotta_ora(u, 165)
  if m688.caso(u, 0.5508) then goto L_019f end
  m688.posizione(u, 21816.3696, -51541.5105)
  m688.rotta_ora(u, 185)
  if m688.caso(u, 0.6289) then goto L_019f end
  m688.posizione(u, 14866.3022, -66144.8693)
  m688.rotta_ora(u, 205)
  if m688.caso(u, 0.707) then goto L_019f end
  m688.posizione(u, 26023.2265, -57084.667)
  m688.rotta_ora(u, 225)
  if m688.caso(u, 0.7852) then goto L_019f end
  m688.posizione(u, 16228.5402, -49472.0626)
  m688.rotta_ora(u, 245)
  if m688.caso(u, 0.8633) then goto L_019f end
  m688.posizione(u, 5072.6291, -60601.3985)
  m688.rotta_ora(u, 300)
  ::L_019f::
  m688.salva_modello(u)
  m688.dopo(u, 280, "L_0112")
  do return end
end

script.L_01a6 = function(u)
  ::L_01a6::
  m688.marca_contatti(u)
  m688.dopo(u, 20, "L_01a6")
  do return end
end

script.L_01ad = function(u)
  ::L_01ad::
  m688.comportamento(u, "L_0208")
  m688.salva_modello(u)
  m688.marca_contatti(u)
  m688.dopo(u, 20, "L_01ad")
  do return end
end

script.L_01b8 = function(u)
  ::L_01b8::
  if m688.manomesso(u) then goto L_01bc end
  do return end
  ::L_01bc::
  m688.messaggio(u, 57)
  m688.modello_sonar(u, 1)
  m688.modello_rumore(u, 255)
  m688.modello_integrita(u, 10)
  m688.salva_modello(u)
  do return end
end

script.L_01c6 = function(u)
  ::L_01c6::
  if m688.unita_esiste(u, 2) then goto L_01d8 end
  if m688.unita_esiste(u, 8) then goto L_01d8 end
  if m688.unita_esiste(u, 9) then goto L_01d8 end
  goto L_04ec
  ::L_01d8::
  if m688.unita_esiste(u, 3) then goto L_01f4 end
  if m688.unita_esiste(u, 4) then goto L_01f4 end
  if m688.unita_esiste(u, 5) then goto L_01f4 end
  if m688.unita_esiste(u, 6) then goto L_01f4 end
  if m688.unita_esiste(u, 7) then goto L_01f4 end
  m688.messaggio(u, 107)
  do return end
  ::L_01f4::
  if m688.in_rettangolo(u, -103192.8186, -7089.5866, 92430.9261, -118376.7851) then goto L_0202 end
  m688.messaggio(u, 103)
  do return end
  ::L_0202::
  m688.dopo(u, 20, "L_01c6")
  do return end
  ::L_04ec::
  m688.variabile(u, 3, 0)
  if m688.unita_esiste(u, 2) then goto L_04f9 end
  m688.variabile_piu(u, 3)
  ::L_04f9::
  if m688.unita_esiste(u, 8) then goto L_0501 end
  m688.variabile_piu(u, 3)
  ::L_0501::
  if m688.unita_esiste(u, 9) then goto L_0509 end
  m688.variabile_piu(u, 3)
  ::L_0509::
  if m688.variabile_uguale(u, 3, 0) then goto L_0521 end
  if m688.variabile_uguale(u, 3, 1) then goto L_0524 end
  if m688.variabile_uguale(u, 3, 2) then goto L_0527 end
  m688.messaggio(u, 109)
  do return end
  ::L_0521::
  m688.messaggio(u, 104)
  do return end
  ::L_0524::
  m688.messaggio(u, 105)
  do return end
  ::L_0527::
  m688.messaggio(u, 108)
  do return end
end

script.L_0208 = function(u)
  ::L_0208::
  if m688.bersaglio_id(u, 0) then goto L_027b end
  if m688.bersaglio_arma(u) then goto L_0215 end
  ::L_0210::
  m688.comportamento(u, "L_0208")
  m688.salva_modello(u)
  do return end
  ::L_0215::
  if m688.bersaglio_mi_punta(u) then goto L_021b end
  goto L_0210
  ::L_021b::
  if m688.traccia_solida(u) then goto L_022a end
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, 0.0391)
  m688.ingaggia(u, 20, "L_021b", "L_0210")
  m688.salva_modello(u)
  do return end
  ::L_022a::
  if m688.bersaglio_vicino(u, 3) then goto L_024b end
  ::L_022f::
  m688.annulla_timer(u, "L_02f2")
  m688.annulla_timer(u, "L_0305")
  m688.ferma_navigazione(u)
  m688.quota(u, 274.32, 1)
  m688.accosta_opposto(u)
  m688.telegrafo(u, 1.0547)
  m688.dopo(u, 240, "L_02f2")
  m688.dopo(u, 60, "L_0210")
  m688.salva_modello(u)
  do return end
  ::L_024b::
  if m688.flag_scena_bit(u, 16) then goto L_022f end
  m688.flag_scena_accendi(u, 16)
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_025b")
  do return end
  ::L_027b::
  if m688.traccia_solida(u) then goto L_02a0 end
  m688.annulla_timer(u, "L_02f2")
  m688.annulla_timer(u, "L_0305")
  m688.ferma_navigazione(u)
  m688.azzera_rateo_quota(u)
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, 0.5)
  m688.ingaggia(u, 20, "L_027b", "L_0292")
  m688.salva_modello(u)
  do return end
  ::L_02a0::
  m688.ferma_navigazione(u)
  m688.annulla_timer(u, "L_02f2")
  m688.annulla_timer(u, "L_0305")
  m688.uguaglia_velocita(u)
  m688.uguaglia_quota(u)
  m688.accosta_verso_bersaglio(u)
  m688.ingaggia(u, 10, "L_027b", "L_0292")
  if m688.bersaglio_vicino(u, 9) then goto L_02b8 end
  goto L_02e6
  ::L_02b8::
  if m688.variabile_uguale(u, 3, 4) then goto L_02e6 end
  m688.marca_contatti(u)
  m688.salva_modello(u)
  m688.variabile_piu(u, 3)
  script.L_0595(u)
  m688.dopo(u, 240, "L_03e4")
  do return end
  ::L_02e6::
  if m688.bersaglio_vicino(u, 1.2) then goto L_02ed end
  m688.salva_modello(u)
  do return end
  ::L_02ed::
  m688.telegrafo(u, 0.0391)
  m688.salva_modello(u)
  do return end
end

script.L_0210 = function(u)
  ::L_0210::
  m688.comportamento(u, "L_0208")
  m688.salva_modello(u)
  do return end
end

script.L_021b = function(u)
  ::L_021b::
  if m688.traccia_solida(u) then goto L_022a end
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, 0.0391)
  m688.ingaggia(u, 20, "L_021b", "L_0210")
  m688.salva_modello(u)
  do return end
  ::L_022a::
  if m688.bersaglio_vicino(u, 3) then goto L_024b end
  ::L_022f::
  m688.annulla_timer(u, "L_02f2")
  m688.annulla_timer(u, "L_0305")
  m688.ferma_navigazione(u)
  m688.quota(u, 274.32, 1)
  m688.accosta_opposto(u)
  m688.telegrafo(u, 1.0547)
  m688.dopo(u, 240, "L_02f2")
  m688.dopo(u, 60, "L_0210")
  m688.salva_modello(u)
  do return end
  ::L_024b::
  if m688.flag_scena_bit(u, 16) then goto L_022f end
  m688.flag_scena_accendi(u, 16)
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_025b")
  do return end
end

script.L_025b = function(u)
  ::L_025b::
  m688.telegrafo(u, 1.0547)
  m688.quota(u, 182.88, 1)
  m688.salva_modello(u)
  m688.dopo(u, 80, "L_0276")
  m688.dopo(u, 120, "L_0272")
  goto L_06a0
  ::L_06a0::
  m688.messaggio_al_giocatore(u, 43)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_flag(u, 193)
  m688.lancia(u, "L_069c", "L_0679")
  m688.modello_tipo(u, 1)
  m688.modello_rumore(u, 0)
  m688.velocita_ora(u, 0.9143)
  m688.modello_avanti_tutta(u, 0.9143)
  m688.modello_integrita(u, 2)
  m688.modello_4c(u, 1)
  m688.navigazione(u, 0)
  m688.collega(u, 1)
  m688.collega(u, 0)
  m688.flag_scena(u, 0)
  m688.nuova_unita(u)
  m688.dopo(u, 2, "L_06cd")
  m688.dopo(u, 160, "L_06d2")
  do return end
end

script.L_0272 = function(u)
  ::L_0272::
  m688.flag_scena_spegni(u, 16)
  m688.salva_modello(u)
  do return end
end

script.L_0276 = function(u)
  ::L_0276::
  m688.comportamento(u, "L_0208")
  m688.salva_modello(u)
  do return end
end

script.L_027b = function(u)
  ::L_027b::
  if m688.traccia_solida(u) then goto L_02a0 end
  m688.annulla_timer(u, "L_02f2")
  m688.annulla_timer(u, "L_0305")
  m688.ferma_navigazione(u)
  m688.azzera_rateo_quota(u)
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, 0.5)
  m688.ingaggia(u, 20, "L_027b", "L_0292")
  m688.salva_modello(u)
  do return end
  ::L_02a0::
  m688.ferma_navigazione(u)
  m688.annulla_timer(u, "L_02f2")
  m688.annulla_timer(u, "L_0305")
  m688.uguaglia_velocita(u)
  m688.uguaglia_quota(u)
  m688.accosta_verso_bersaglio(u)
  m688.ingaggia(u, 10, "L_027b", "L_0292")
  if m688.bersaglio_vicino(u, 9) then goto L_02b8 end
  goto L_02e6
  ::L_02b8::
  if m688.variabile_uguale(u, 3, 4) then goto L_02e6 end
  m688.marca_contatti(u)
  m688.salva_modello(u)
  m688.variabile_piu(u, 3)
  script.L_0595(u)
  m688.dopo(u, 240, "L_03e4")
  do return end
  ::L_02e6::
  if m688.bersaglio_vicino(u, 1.2) then goto L_02ed end
  m688.salva_modello(u)
  do return end
  ::L_02ed::
  m688.telegrafo(u, 0.0391)
  m688.salva_modello(u)
  do return end
end

script.L_0292 = function(u)
  ::L_0292::
  m688.telegrafo(u, 0.5)
  m688.riprendi_navigazione(u)
  m688.comportamento(u, "L_0208")
  m688.salva_modello(u)
  m688.dopo(u, 120, "L_02f2")
  do return end
end

script.L_02f2 = function(u)
  ::L_02f2::
  m688.ferma_navigazione(u)
  m688.telegrafo(u, 0.0195)
  m688.virata_relativa(u, -180)
  m688.quota(u, 274.32, 0.3906)
  m688.salva_modello(u)
  m688.dopo(u, 120, "L_0305")
  do return end
end

script.L_0305 = function(u)
  ::L_0305::
  m688.riprendi_navigazione(u)
  m688.telegrafo(u, 0.5)
  m688.quota(u, 30.48, 0.5859)
  m688.salva_modello(u)
  m688.dopo(u, 320, "L_02f2")
  do return end
end

script.L_0315 = function(u)
  ::L_0315::
  if m688.bersaglio_id(u, 0) then goto L_0387 end
  if m688.bersaglio_nave(u) then goto L_03e8 end
  if m688.bersaglio_arma(u) then goto L_0325 end
  ::L_0320::
  m688.comportamento(u, "L_0315")
  m688.salva_modello(u)
  do return end
  ::L_0325::
  if m688.variabile_uguale(u, 1, 0) then goto L_0366 end
  if m688.flag_scena_bit(u, 1) then goto L_0366 end
  m688.nuovo_caso(u)
  m688.vai_a(u, -517.0567, -55046.4762, 30, "L_043c")
  if m688.caso(u, 0.2539) then goto L_0361 end
  m688.vai_a(u, -1214.0486, -47405.9434, 30, "L_043c")
  if m688.caso(u, 0.5039) then goto L_0361 end
  m688.vai_a(u, -2606.4116, -39765.0414, 30, "L_043c")
  if m688.caso(u, 0.7539) then goto L_0361 end
  m688.vai_a(u, -5388.5835, -32817.5098, 30, "L_043c")
  ::L_0361::
  m688.telegrafo(u, 1.0547)
  m688.flag_scena_accendi(u, 1)
  ::L_0366::
  if m688.bersaglio_mi_punta(u) then goto L_036c end
  goto L_0320
  ::L_036c::
  if m688.traccia_solida(u) then goto L_0374 end
  ::L_036f::
  m688.comportamento(u, "L_0315")
  m688.salva_modello(u)
  do return end
  ::L_0374::
  if m688.bersaglio_vicino(u, 0.8) then goto L_037c end
  goto L_036f
  ::L_037c::
  m688.comportamento(u, "L_0315")
  m688.salva_modello(u)
  m688.messaggio(u, 19)
  m688.attacca(u, 6, 16)
  do return end
  ::L_0387::
  if m688.flag_scena_bit(u, 1) then goto L_0398 end
  m688.annulla_timer(u, "L_03e1")
  m688.dopo(u, 240, "L_03e1")
  m688.ferma_navigazione(u)
  m688.telegrafo(u, 1.0547)
  m688.accosta_verso_bersaglio(u)
  ::L_0398::
  if m688.variabile_uguale(u, 3, 4) then goto L_03a2 end
  goto L_03bc
  ::L_03a2::
  if m688.traccia_solida(u) then goto L_03ab end
  m688.comportamento(u, "L_0315")
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  do return end
  ::L_03ab::
  if m688.bersaglio_vicino(u, 3) then goto L_03b3 end
  goto L_0320
  ::L_03b3::
  m688.comportamento(u, "L_0315")
  m688.salva_modello(u)
  m688.attacca(u, 6, 13)
  do return end
  ::L_03bc::
  m688.variabile_piu(u, 3)
  m688.comportamento(u, "L_0315")
  m688.salva_modello(u)
  m688.marca_contatti(u)
  if m688.bersaglio_vicino(u, 5) then goto L_03d5 end
  script.L_05a0(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 240, "L_03e4")
  do return end
  ::L_03d5::
  script.L_0595(u)
  m688.seleziona(u, 0)
  m688.dopo(u, 240, "L_03e4")
  do return end
  ::L_03e8::
  if m688.bersaglio_nemico(u) then goto L_03ee end
  goto L_0320
  ::L_03ee::
  if m688.traccia_solida(u) then goto L_03f9 end
  m688.ingaggia(u, 30, "L_03e8", "L_0320")
  m688.salva_modello(u)
  do return end
  ::L_03f9::
  if m688.bersaglio_vicino(u, 9) then goto L_0406 end
  m688.ingaggia(u, 30, "L_03e8", "L_0320")
  m688.salva_modello(u)
  do return end
  ::L_0406::
  m688.comportamento(u, "L_0315")
  m688.salva_modello(u)
  if m688.variabile_uguale(u, 2, 1) then goto L_0418 end
  m688.messaggio(u, 52)
  m688.variabile(u, 2, 1)
  ::L_0418::
  m688.seleziona_bersaglio(u)
  if m688.flag_scena_uguale(u, 1) then goto L_0428 end
  m688.flag_scena(u, 1)
  m688.salva_modello(u)
  m688.collega(u, 1)
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_0429")
  ::L_0428::
  do return end
end

script.L_0320 = function(u)
  ::L_0320::
  m688.comportamento(u, "L_0315")
  m688.salva_modello(u)
  do return end
end

script.L_03e1 = function(u)
  ::L_03e1::
  m688.riprendi_navigazione(u)
  m688.salva_modello(u)
  do return end
end

script.L_03e4 = function(u)
  ::L_03e4::
  m688.variabile_meno(u, 3)
  do return end
end

script.L_03e8 = function(u)
  ::L_0320::
  m688.comportamento(u, "L_0315")
  m688.salva_modello(u)
  do return end
  ::L_03e8::
  if m688.bersaglio_nemico(u) then goto L_03ee end
  goto L_0320
  ::L_03ee::
  if m688.traccia_solida(u) then goto L_03f9 end
  m688.ingaggia(u, 30, "L_03e8", "L_0320")
  m688.salva_modello(u)
  do return end
  ::L_03f9::
  if m688.bersaglio_vicino(u, 9) then goto L_0406 end
  m688.ingaggia(u, 30, "L_03e8", "L_0320")
  m688.salva_modello(u)
  do return end
  ::L_0406::
  m688.comportamento(u, "L_0315")
  m688.salva_modello(u)
  if m688.variabile_uguale(u, 2, 1) then goto L_0418 end
  m688.messaggio(u, 52)
  m688.variabile(u, 2, 1)
  ::L_0418::
  m688.seleziona_bersaglio(u)
  if m688.flag_scena_uguale(u, 1) then goto L_0428 end
  m688.flag_scena(u, 1)
  m688.salva_modello(u)
  m688.collega(u, 1)
  m688.salva_modello(u)
  m688.dopo(u, 2, "L_0429")
  ::L_0428::
  do return end
end

script.L_0429 = function(u)
  ::L_0429::
  m688.attacco_casuale(u, 50)
  m688.dopo(u, 10, "L_0429")
  do return end
end

script.L_0431 = function(u)
  ::L_0431::
  m688.vai_a(u, 5746.6299, -28649.5858, 30, "L_043c")
  m688.salva_modello(u)
  do return end
end

script.L_043c = function(u)
  ::L_043c::
  m688.vai_a(u, -516.4781, -46711.4338, 30, "L_0447")
  m688.salva_modello(u)
  do return end
end

script.L_0447 = function(u)
  ::L_0447::
  m688.vai_a(u, 17584.4512, -32798.5666, 30, "L_0431")
  m688.salva_modello(u)
  do return end
end

script.L_04ec = function(u)
  ::L_04ec::
  m688.variabile(u, 3, 0)
  if m688.unita_esiste(u, 2) then goto L_04f9 end
  m688.variabile_piu(u, 3)
  ::L_04f9::
  if m688.unita_esiste(u, 8) then goto L_0501 end
  m688.variabile_piu(u, 3)
  ::L_0501::
  if m688.unita_esiste(u, 9) then goto L_0509 end
  m688.variabile_piu(u, 3)
  ::L_0509::
  if m688.variabile_uguale(u, 3, 0) then goto L_0521 end
  if m688.variabile_uguale(u, 3, 1) then goto L_0524 end
  if m688.variabile_uguale(u, 3, 2) then goto L_0527 end
  m688.messaggio(u, 109)
  do return end
  ::L_0521::
  m688.messaggio(u, 104)
  do return end
  ::L_0524::
  m688.messaggio(u, 105)
  do return end
  ::L_0527::
  m688.messaggio(u, 108)
  do return end
end

script.L_0588 = function(u)
  ::L_0588::
  m688.lancio_missile(u)
  m688.messaggio_al_giocatore(u, 49)
  m688.seleziona_collegata(u)
  m688.dopo(u, 20, "L_0592")
  do return end
end

script.L_0592 = function(u)
  ::L_0592::
  m688.attacco_casuale(u, 75)
  do return end
end

script.L_0595 = function(u)
  ::L_0595::
  m688.rumore_transitorio(u)
  script.L_05c5(u)
  m688.nuova_unita(u)
  m688.dopo(u, 240, "L_069c")
  do return end
end

script.L_05a0 = function(u)
  ::L_05a0::
  m688.lancio_missile(u)
  m688.teletrasporta_al_bersaglio(u)
  script.L_05c5(u)
  m688.teletrasporta_al_bersaglio(u)
  m688.modello_classe(u, "type40")
  m688.modello_difesa(u, 40)
  m688.comportamento(u, "L_064d")
  m688.marca_contatti(u)
  m688.telegrafo(u, 0.7812)
  m688.rateo_virata(u, -1.8)
  m688.rotta(u, nil)
  m688.collega(u, 0)
  m688.nuova_unita(u)
  m688.dopo(u, 10, "L_0695")
  m688.dopo(u, 240, "L_069c")
  do return end
end

script.L_05c5 = function(u)
  ::L_05c5::
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "type53")
  m688.modello_flag(u, 49)
  m688.lancia(u, "L_069c", "L_0679")
  m688.modello_tipo(u, 1)
  m688.modello_rateo_quota(u, 10.287)
  m688.modello_virata(u, 9)
  m688.modello_accelerazione(u, 1.0286)
  m688.modello_avanti_tutta(u, 50.2857)
  m688.modello_rumore(u, 18)
  m688.modello_sonar(u, 15)
  m688.modello_integrita(u, 20)
  m688.modello_difesa(u, 100)
  m688.modello_quota_max(u, 548.64)
  m688.modello_4c(u, 1)
  m688.telegrafo(u, 1)
  m688.navigazione(u, 1)
  m688.collega(u, 1)
  m688.flag_scena(u, 0)
  do return end
end

script.L_05f5 = function(u)
  ::L_05f5::
  m688.rumore_transitorio(u)
  m688.variabile_piu(u, 1)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_classe(u, "mk48")
  m688.modello_flag(u, 41)
  m688.lancia(u, "L_069c", "L_0679")
  script.L_061c(u)
  m688.dopo(u, 170, "L_0619")
  m688.dopo(u, 190, "L_0642")
  m688.dopo(u, 280, "L_069c")
  m688.messaggio_al_giocatore(u, 2)
  do return end
end

script.L_0619 = function(u)
  ::L_0619::
  m688.messaggio_al_giocatore(u, 39)
  do return end
end

script.L_061c = function(u)
  ::L_061c::
  m688.modello_tipo(u, 1)
  m688.modello_rateo_quota(u, 10.287)
  m688.modello_virata(u, 9)
  m688.modello_accelerazione(u, 0.7714)
  m688.modello_avanti_tutta(u, 50.2857)
  m688.modello_rumore(u, 18)
  m688.modello_sonar(u, 17)
  m688.modello_integrita(u, 20)
  m688.modello_difesa(u, 75)
  m688.modello_quota_max(u, 548.64)
  m688.modello_4c(u, 1)
  m688.telegrafo(u, 1)
  m688.navigazione(u, 1)
  m688.collega(u, 1)
  m688.flag_scena(u, 1)
  m688.nuova_unita(u)
  do return end
end

script.L_0642 = function(u)
  ::L_0642::
  m688.annulla_timer(u, "L_0642")
  m688.collega(u, 0)
  m688.modello_sonar(u, 13)
  m688.messaggio_al_giocatore(u, 23)
  m688.salva_modello(u)
  do return end
end

script.L_064d = function(u)
  ::L_064d::
  if m688.traccia_solida(u) then goto L_0670 end
  ::L_0650::
  if m688.flag_scena_uguale(u, 1) then goto L_065e end
  if m688.bersaglio_id(u, 0) then goto L_065e end
  ::L_0659::
  m688.comportamento(u, "L_064d")
  m688.salva_modello(u)
  do return end
  ::L_065e::
  m688.telegrafo(u, 1)
  m688.annulla_timer(u, "L_068d")
  m688.annulla_timer(u, "L_0679")
  m688.insegui(u, 0, "L_069c", "L_0679")
  m688.salva_modello(u)
  do return end
  ::L_0670::
  if m688.bersaglio_vicino(u, 14) then goto L_0650 end
  goto L_0659
end

script.L_0679 = function(u)
  ::L_0679::
  m688.telegrafo(u, 0.7812)
  m688.comportamento(u, "L_064d")
  m688.salva_modello(u)
  m688.impulso_sonar(u)
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_0695")
  m688.dopo(u, 10, "L_068d")
  do return end
end

script.L_068d = function(u)
  ::L_068d::
  m688.rateo_virata(u, -3.6)
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_0695 = function(u)
  ::L_0695::
  m688.impulso_sonar(u)
  m688.dopo(u, 10, "L_0695")
  do return end
end

script.L_069c = function(u)
  ::L_069c::
  m688.messaggio_al_giocatore(u, 24)
  m688.togli_unita(u)
  do return end
end

script.L_06a0 = function(u)
  ::L_06a0::
  m688.messaggio_al_giocatore(u, 43)
  m688.memorizza_bersaglio(u)
  m688.modello_azzera(u)
  m688.modello_flag(u, 193)
  m688.lancia(u, "L_069c", "L_0679")
  m688.modello_tipo(u, 1)
  m688.modello_rumore(u, 0)
  m688.velocita_ora(u, 0.9143)
  m688.modello_avanti_tutta(u, 0.9143)
  m688.modello_integrita(u, 2)
  m688.modello_4c(u, 1)
  m688.navigazione(u, 0)
  m688.collega(u, 1)
  m688.collega(u, 0)
  m688.flag_scena(u, 0)
  m688.nuova_unita(u)
  m688.dopo(u, 2, "L_06cd")
  m688.dopo(u, 160, "L_06d2")
  do return end
end

script.L_06cd = function(u)
  ::L_06cd::
  m688.modello_rumore(u, 40)
  m688.salva_modello(u)
  m688.devia_riferimenti(u)
  do return end
end

script.L_06d2 = function(u)
  ::L_06d2::
  m688.messaggio_al_giocatore(u, 44)
  m688.togli_unita(u)
  do return end
end

script.L_06d6 = function(u)
  ::L_06d6::
  m688.seleziona_bersaglio(u)
  m688.messaggio(u, 19)
  m688.togli_unita(u)
  do return end
end

script.L_06db = function(u)
  ::L_06db::
  m688.messaggio_al_giocatore(u, 41)
  m688.seleziona_bersaglio(u)
  m688.marca_contatti(u)
  m688.dopo(u, 2, "L_0679")
  do return end
end

script.L_06e5 = function(u, p)
  ::L_06e5::
  m688.seleziona_bersaglio(u)
  m688.vai_a(u, par(p, 1, -116846.1684), par(p, 2, 7009.9259), 20, "L_06f4")
  m688.telegrafo(u, 1)
  m688.salva_modello(u)
  do return end
end

script.L_06f4 = function(u)
  ::L_06f4::
  m688.telegrafo(u, 0)
  m688.comportamento(u, "L_064d")
  m688.salva_modello(u)
  m688.marca_contatti(u)
  if m688.flag_scena_uguale(u, 1) then goto L_0702 end
  m688.messaggio_al_giocatore(u, 42)
  ::L_0702::
  do return end
end

script.L_0703 = function(u, p)
  ::L_0703::
  m688.vai_a(u, par(p, 1, -116846.1684), par(p, 2, 7009.9259), 30, "L_070e")
  m688.salva_modello(u)
  do return end
end

script.L_070e = function(u)
  ::L_070e::
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 4)
  do return end
end

script.L_0715 = function(u, p)
  ::L_0715::
  m688.telegrafo(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_071a = function(u, p)
  ::L_071a::
  m688.quota(u, par(p, 1, 0), par(p, 2, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0721 = function(u, p)
  ::L_0721::
  m688.rateo_virata(u, par(p, 1, 0))
  m688.rotta(u, nil)
  m688.salva_modello(u)
  do return end
end

script.L_0729 = function(u)
  ::L_0729::
  m688.annulla_timer(u, "L_0768")
  m688.dopo(u, 20, "L_0768")
  do return end
end

script.L_0732 = function(u)
  ::L_0732::
  m688.annulla_timer(u, "L_076b")
  m688.dopo(u, 20, "L_076b")
  do return end
end

script.L_073b = function(u)
  ::L_073b::
  m688.annulla_timer(u, "L_076e")
  m688.dopo(u, 20, "L_076e")
  do return end
end

script.L_0744 = function(u)
  ::L_0744::
  m688.annulla_timer(u, "L_0771")
  m688.dopo(u, 20, "L_0771")
  do return end
end

script.L_074d = function(u)
  ::L_074d::
  m688.annulla_timer(u, "L_0774")
  m688.dopo(u, 20, "L_0774")
  do return end
end

script.L_0756 = function(u)
  ::L_0756::
  m688.annulla_timer(u, "L_0777")
  m688.dopo(u, 20, "L_0777")
  do return end
end

script.L_075f = function(u)
  ::L_075f::
  m688.annulla_timer(u, "L_077a")
  m688.dopo(u, 20, "L_077a")
  do return end
end

script.L_0768 = function(u)
  ::L_0768::
  m688.messaggio_al_giocatore(u, 10)
  do return end
end

script.L_076b = function(u)
  ::L_076b::
  m688.messaggio_al_giocatore(u, 11)
  do return end
end

script.L_076e = function(u)
  ::L_076e::
  m688.messaggio_al_giocatore(u, 12)
  do return end
end

script.L_0771 = function(u)
  ::L_0771::
  m688.messaggio_al_giocatore(u, 13)
  do return end
end

script.L_0774 = function(u)
  ::L_0774::
  m688.messaggio_al_giocatore(u, 14)
  do return end
end

script.L_0777 = function(u)
  ::L_0777::
  m688.messaggio_al_giocatore(u, 15)
  do return end
end

script.L_077a = function(u)
  ::L_077a::
  m688.messaggio_al_giocatore(u, 16)
  do return end
end

script.L_077d = function(u)
  ::L_077d::
  m688.ferma_navigazione(u)
  m688.rateo_virata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 6)
  do return end
end

script.L_0785 = function(u, p)
  ::L_0785::
  m688.periscopio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0789 = function(u)
  ::L_0789::
  m688.dopo(u, 60, "L_0793")
  do return end
end

script.L_078f = function(u, p)
  ::L_078f::
  m688.antenna(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_0793 = function(u)
  ::L_0793::
  m688.schiera_trainata(u, 1)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 17)
  do return end
end

script.L_0799 = function(u)
  ::L_0799::
  m688.dopo(u, 40, "L_079f")
  do return end
end

script.L_079f = function(u)
  ::L_079f::
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  m688.messaggio_al_giocatore(u, 18)
  do return end
end

script.L_07a5 = function(u)
  ::L_07a5::
  m688.annulla_timer(u, "L_0793")
  m688.annulla_timer(u, "L_079f")
  m688.schiera_trainata(u, 0)
  m688.salva_modello(u)
  do return end
end

script.L_07af = function(u, p)
  ::L_07af::
  m688.bersaglio(u, par(p, 1, 0))
  m688.salva_modello(u)
  do return end
end

script.L_07b4 = function(u, p)
  ::L_07b4::
  m688.posizione(u, par(p, 1, -116846.1684), par(p, 2, 7009.9259))
  m688.salva_modello(u)
  do return end
end

return missione
