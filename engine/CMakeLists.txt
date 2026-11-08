cmake_minimum_required(VERSION 3.25)
project(melchior688 VERSION 0.1.0 LANGUAGES C CXX)

# Motore del remake di 688 Attack Sub (docs/00_ROADMAP.md, Fase 1).
#   nucleo      simulazione deterministica a passo fisso, senza grafica ne' SDL
#   grafica     tela 640x480 a 256 colori indicizzati, sprite, font, periscopio (senza SDL)
#   piattaforma finestra, ingresso e presentazione con SDL3
#   melchior688 l'eseguibile; prove: i test (doctest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

option(M688_FINESTRA "Compila la piattaforma SDL3 e l'eseguibile" ON)
# i binari del pacchetto di rilascio (tools/rilascio/pacchetto_linux.sh): melchior688 cerca SDL3 e sndio in ../lib con un
# DT_RPATH $ORIGIN/../lib (vale anche per sndio, che carica SDL3), e niente percorsi del progetto: i dati li trova solo
# accanto all'eseguibile (app/percorsi_pacchetto.hpp). Spenta nella build di sviluppo
option(M688_RILASCIO "Binari per il pacchetto di rilascio" OFF)
set(M688_DATI "${CMAKE_CURRENT_SOURCE_DIR}/../data/motore" CACHE PATH "Cartella dei dati esportati dalla pipeline")

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Wall> $<$<COMPILE_LANGUAGE:CXX>:-Wextra> $<$<COMPILE_LANGUAGE:CXX>:-Wpedantic>
                      $<$<COMPILE_LANGUAGE:CXX>:-Wshadow> $<$<COMPILE_LANGUAGE:CXX>:-Wconversion> $<$<COMPILE_LANGUAGE:CXX>:-Wno-sign-conversion>)
endif()

add_library(nucleo STATIC
  nucleo/src/geometria.cpp
  nucleo/src/sole.cpp
  nucleo/src/testi.cpp
  nucleo/src/mondo.cpp
  nucleo/src/classi.cpp
  nucleo/src/fondale.cpp
  nucleo/src/rotte.cpp
  nucleo/src/scafo.cpp
  nucleo/src/danni.cpp
  nucleo/src/collisioni.cpp
  nucleo/src/fenomeni.cpp
  nucleo/src/battello.cpp
  nucleo/src/ordini.cpp
  nucleo/src/comando.cpp
  nucleo/src/armi_giocatore.cpp
  nucleo/src/carico.cpp
  nucleo/src/compiti_carico.cpp
  nucleo/src/mine.cpp
  nucleo/src/carriera.cpp
  nucleo/src/fascicolo.cpp
  nucleo/src/siluro.cpp
  nucleo/src/acustica.cpp
  nucleo/src/analisi_acustica.cpp
  nucleo/src/paesaggio_sonoro.cpp
  nucleo/src/interferenza.cpp
  nucleo/src/navi_civili.cpp
  nucleo/src/traffico.cpp
  nucleo/src/schiera.cpp
  nucleo/src/intercettazione.cpp
  nucleo/src/rilevatore_righe.cpp
  nucleo/src/storia_cascata.cpp
  nucleo/src/ecoscandaglio.cpp
  nucleo/src/quadro_fondale.cpp
  nucleo/src/ritorni_fondo.cpp
  nucleo/src/confronto_classificazioni.cpp
  nucleo/src/sondaggio_ia.cpp
  nucleo/src/campo_ghiaccio.cpp
  nucleo/src/modifica_profilo.cpp
  nucleo/src/volta_ghiaccio.cpp
  nucleo/src/sonar_giocatore.cpp
  nucleo/src/carta_giocatore.cpp
  nucleo/src/strumenti_carta.cpp
  nucleo/src/luci_navigazione.cpp
  nucleo/src/fari.cpp
  nucleo/src/portata_luci.cpp
  nucleo/src/sagoma_notte.cpp
  nucleo/src/aob_ia.cpp
  nucleo/src/guasti_giocatore.cpp
  nucleo/src/tma.cpp
  nucleo/src/mappa_tasti.cpp
  nucleo/src/dinamica_sottomarino.cpp
  nucleo/src/radio.cpp
  nucleo/src/elettronica.cpp
  nucleo/src/asw_aereo.cpp
  nucleo/src/evasione.cpp
  nucleo/src/artiglieria.cpp
  nucleo/src/periscopi.cpp
  nucleo/src/meteo.cpp
  nucleo/src/acustica_asw.cpp
  nucleo/src/misure_sonar.cpp
  nucleo/src/acint.cpp
  nucleo/src/sonarista.cpp
  nucleo/src/ascolto.cpp
  nucleo/src/mixer_audio.cpp
  nucleo/src/bordo_audio.cpp
  nucleo/src/audio_meteo.cpp
  nucleo/src/transitori_armi.cpp
  nucleo/src/campioni.cpp
  nucleo/src/acustica_artiglieria.cpp
  nucleo/src/acustica_urto.cpp
  nucleo/src/enciclopedia.cpp nucleo/src/albo.cpp nucleo/src/achievement.cpp nucleo/src/albo_onore.cpp nucleo/src/traccia_replay.cpp nucleo/src/statistiche.cpp nucleo/src/tma_ia.cpp nucleo/src/navigazione.cpp nucleo/src/missili.cpp nucleo/src/difese_navi.cpp nucleo/src/manovre_tma.cpp nucleo/src/armi_ia.cpp nucleo/src/suoni_classici.cpp nucleo/src/statistiche_sandbox.cpp
  nucleo/src/musica_fm.cpp nucleo/src/confronto_serie.cpp)
target_include_directories(nucleo PUBLIC nucleo/include)
target_include_directories(nucleo SYSTEM PRIVATE terzi/tomlplusplus)
target_compile_definitions(nucleo PRIVATE "TOML_ASSERT=(void)sizeof")   # un file rovinato: parse_error anche in Debug

# ymfm (sorgenti in terzi/ymfm, Aaron Giles, licenza BSD-3): l'OPL3 della musica FM dal vivo (nucleo/musica_fm)
add_library(ymfm STATIC terzi/ymfm/ymfm_opl.cpp terzi/ymfm/ymfm_adpcm.cpp terzi/ymfm/ymfm_pcm.cpp)
target_include_directories(ymfm SYSTEM PUBLIC terzi/ymfm)
target_compile_options(ymfm PRIVATE -w)
target_link_libraries(nucleo PUBLIC ymfm)

# Lua 5.4.7 (sorgenti in terzi/lua, licenza MIT) per le missioni
file(GLOB LUA_SORGENTI terzi/lua/*.c)
add_library(lua54 STATIC ${LUA_SORGENTI})
set_target_properties(lua54 PROPERTIES LINKER_LANGUAGE C C_STANDARD 99)
target_include_directories(lua54 SYSTEM PUBLIC terzi/lua)
target_compile_definitions(lua54 PRIVATE LUA_USE_POSIX)
target_compile_options(lua54 PRIVATE -w)
target_link_libraries(lua54 PUBLIC m)

add_library(missioni STATIC missioni/src/missione.cpp missioni/src/registro_missione.cpp missioni/src/missione_asw.cpp missioni/src/missione_periscopi.cpp missioni/src/missione_meteo.cpp missioni/src/missione_sonarista.cpp missioni/src/missione_transitori.cpp missioni/src/missione_esche.cpp missioni/src/missione_artiglieria.cpp missioni/src/missione_collisioni.cpp missioni/src/missione_diesel.cpp missioni/src/missione_bqs15.cpp missioni/src/missione_avaria.cpp missioni/src/missione_forze.cpp missioni/src/missione_difesa.cpp missioni/src/missione_armi_ia.cpp missioni/src/missione_reazioni.cpp missioni/src/missione_pedinamento.cpp missioni/src/missione_navigazione.cpp missioni/src/missione_missili.cpp missioni/src/missione_traffico.cpp missioni/src/missione_schiera.cpp missioni/src/missione_vds.cpp missioni/src/missione_intercettazione.cpp missioni/src/missione_fondo.cpp missioni/src/missione_fondale.cpp missioni/src/missione_fondale_ia.cpp missioni/src/missione_ghiaccio.cpp missioni/src/missione_libretto.cpp missioni/src/missione_conti.cpp missioni/src/missione_luci.cpp missioni/src/missione_antisom_navi.cpp missioni/src/missione_interventi.cpp missioni/src/missione_scafo.cpp missioni/src/comandante_ia.cpp missioni/src/sandbox_originali.cpp missioni/src/missione_quadro.cpp missioni/src/danni_missione.cpp missioni/src/stazioni_missione.cpp missioni/src/contatti_giocatore.cpp missioni/src/danni_sottomarini.cpp missioni/src/carriera_missione.cpp missioni/src/achievement_missione.cpp missioni/src/traccia_missione.cpp missioni/src/missione_rapida.cpp missioni/src/scenario_missione.cpp missioni/src/pacchetto_missione.cpp)
target_include_directories(missioni PUBLIC missioni/include)
target_include_directories(missioni SYSTEM PRIVATE terzi/tomlplusplus)   # missioni rapide: data/motore/missioni_rapide.toml
target_compile_definitions(missioni PRIVATE "TOML_ASSERT=(void)sizeof")   # un file rovinato: parse_error anche in Debug
target_link_libraries(missioni PUBLIC nucleo PRIVATE lua54)

add_library(grafica STATIC
  grafica/src/tela.cpp
  grafica/src/palette.cpp
  grafica/src/sprite.cpp
  grafica/src/font.cpp
  grafica/src/periscopio.cpp
  grafica/src/riscala.cpp
  grafica/src/modello.cpp
  grafica/src/effetti.cpp
  grafica/src/effetti_artiglieria.cpp
  grafica/src/spaccato.cpp
  grafica/src/pagina_squadre.cpp
  grafica/src/schermata_comandi.cpp
  grafica/src/testo_font.cpp
  grafica/src/cerimonia.cpp
  grafica/src/decorazioni.cpp
  grafica/src/medaglioni.cpp
  grafica/src/ritratto.cpp
  grafica/src/cornici.cpp
  grafica/src/libretto.cpp
  grafica/src/cartolina.cpp
  grafica/src/giornale.cpp
  grafica/src/sagome_armi.cpp
  grafica/src/oculare.cpp
  grafica/src/cielo.cpp
  grafica/src/moto_navi.cpp
  grafica/src/strati_meteo.cpp
  grafica/src/cascata_firma.cpp
  grafica/src/tema_stazioni.cpp
  grafica/src/consolle.cpp
  grafica/src/sale.cpp
  grafica/src/carta_vga.cpp
  grafica/src/simboli_carta.cpp
  grafica/src/grafici_serie.cpp
  grafica/src/scala_cascata.cpp
  grafica/src/lamiera.cpp
  grafica/src/quadro.cpp)
target_include_directories(grafica PUBLIC grafica/include)
target_include_directories(grafica SYSTEM PRIVATE terzi/tomlplusplus)   # decorazioni_grafica.toml
target_compile_definitions(grafica PRIVATE "TOML_ASSERT=(void)sizeof")   # un file rovinato: parse_error anche in Debug
target_link_libraries(grafica PUBLIC nucleo)

if(M688_FINESTRA)
  find_package(SDL3 3.2 REQUIRED CONFIG COMPONENTS SDL3-shared)
  add_library(piattaforma STATIC piattaforma/src/finestra.cpp piattaforma/src/audio.cpp)
  target_include_directories(piattaforma PUBLIC piattaforma/include)
  target_link_libraries(piattaforma PUBLIC grafica SDL3::SDL3-shared)

  add_executable(melchior688 app/main.cpp app/schermata_modo.cpp app/partita.cpp app/briefing.cpp app/debriefing.cpp app/esito.cpp app/schermata_siluri.cpp app/scelta_missione.cpp app/scena_prova.cpp app/interfaccia.cpp app/schermata_comandi.cpp app/carta.cpp app/sonar_enhanced.cpp app/stazione_radio.cpp app/ripetitori.cpp app/museo.cpp app/albo.cpp app/bacheca.cpp app/coda_messaggi.cpp app/albo_onore.cpp app/editor_missione.cpp app/replay.cpp app/pannello_interventi.cpp app/regia_audio.cpp app/effetti_bordo.cpp app/confronto_ab.cpp app/schermata_confronto.cpp app/replay_affiancati.cpp app/esperimento_ab.cpp app/esperimento.cpp app/statistiche.cpp app/codice_partita.cpp app/musica_titoli.cpp app/regolazioni_sandbox.cpp app/serie_dati.cpp app/schermata_serie.cpp app/stazione_missili.cpp app/pagina_intercettazione.cpp app/cascata_demon.cpp app/righe_automatiche.cpp app/vista_storia.cpp app/vista_fondale.cpp app/video_apertura.cpp)
  target_link_libraries(melchior688 PRIVATE piattaforma missioni)
  target_include_directories(melchior688 SYSTEM PRIVATE terzi/tomlplusplus)   # serie.toml (app/serie_dati)
  target_compile_definitions(melchior688 PRIVATE "TOML_ASSERT=(void)sizeof")   # un file rovinato: parse_error anche in Debug
  if(M688_RILASCIO)
    target_compile_definitions(melchior688 PRIVATE M688_DATI_PREDEFINITI="" M688_MISSIONI_PREDEFINITE="")
    set_target_properties(melchior688 PROPERTIES BUILD_WITH_INSTALL_RPATH ON INSTALL_RPATH "\$ORIGIN/../lib")
    # -rpath-link: sndio, che serve a SDL3, si cerca accanto a SDL3 solo al collegamento; nel binario resta $ORIGIN/../lib
    target_link_options(melchior688 PRIVATE "LINKER:--disable-new-dtags" "LINKER:-rpath-link,$<TARGET_FILE_DIR:SDL3::SDL3-shared>")
  else()
    target_compile_definitions(melchior688 PRIVATE M688_DATI_PREDEFINITI="${M688_DATI}"
                               M688_MISSIONI_PREDEFINITE="${CMAKE_CURRENT_SOURCE_DIR}/../data/missioni")
  endif()

  # la navigazione del Museo con eventi veri: a parte da «prove», che non si lega a SDL
  add_executable(prove_museo test/main.cpp test/prove_museo_navigazione.cpp app/museo.cpp app/carta.cpp app/stazione_radio.cpp app/ripetitori.cpp)
  target_include_directories(prove_museo SYSTEM PRIVATE terzi/doctest terzi/tomlplusplus)
  target_include_directories(prove_museo PRIVATE app)
  target_link_libraries(prove_museo PRIVATE piattaforma missioni)
  target_compile_definitions(prove_museo PRIVATE M688_DATI="${M688_DATI}")
  add_test(NAME prove_museo COMMAND prove_museo)

  # le frecce nelle schermate di contorno, con eventi veri
  add_executable(prove_frecce test/main.cpp test/prove_frecce_schermate.cpp app/schermata_modo.cpp app/scelta_missione.cpp app/albo.cpp app/bacheca.cpp app/briefing.cpp app/schermata_siluri.cpp app/museo.cpp app/carta.cpp app/stazione_radio.cpp app/ripetitori.cpp app/codice_partita.cpp app/esito.cpp app/regolazioni_sandbox.cpp app/pannello_interventi.cpp app/esperimento.cpp)
  target_include_directories(prove_frecce SYSTEM PRIVATE terzi/doctest terzi/tomlplusplus)
  target_include_directories(prove_frecce PRIVATE app)
  target_link_libraries(prove_frecce PRIVATE piattaforma missioni)
  target_compile_definitions(prove_frecce PRIVATE M688_DATI="${M688_DATI}" M688_MISSIONI="${CMAKE_CURRENT_SOURCE_DIR}/../data/missioni")
  add_test(NAME prove_frecce COMMAND prove_frecce)
  # il menu con tasti e clic veri: il modo di gioco, le schede, il modulo del comandante
  add_executable(prove_menu test/main.cpp test/prove_menu_modo.cpp app/schermata_modo.cpp app/scelta_missione.cpp app/albo.cpp app/museo.cpp app/carta.cpp app/stazione_radio.cpp app/ripetitori.cpp app/codice_partita.cpp app/esito.cpp app/regolazioni_sandbox.cpp app/pannello_interventi.cpp app/esperimento.cpp)
  target_include_directories(prove_menu SYSTEM PRIVATE terzi/doctest terzi/tomlplusplus)
  target_include_directories(prove_menu PRIVATE app)
  target_link_libraries(prove_menu PRIVATE piattaforma missioni)
  target_compile_definitions(prove_menu PRIVATE M688_DATI="${M688_DATI}" M688_MISSIONI="${CMAKE_CURRENT_SOURCE_DIR}/../data/missioni")
  add_test(NAME prove_menu COMMAND prove_menu)
endif()

# strumento di verifica del fondale sulle scene originali (non fa parte delle prove: alcuni minuti)
add_executable(verifica_fondale strumenti/verifica_fondale.cpp)
target_link_libraries(verifica_fondale PRIVATE missioni)
target_compile_definitions(verifica_fondale PRIVATE M688_DATI="${M688_DATI}" M688_MISSIONI="${CMAKE_CURRENT_SOURCE_DIR}/../data/missioni")

# rilievo di esiti, colpi e tempi di scoperta sulle scene originali (giocatore passivo)
add_executable(taratura strumenti/taratura.cpp)
target_link_libraries(taratura PRIVATE missioni lua54)
target_compile_definitions(taratura PRIVATE M688_DATI="${M688_DATI}")
# letalita' delle armi sui sottomarini: un colpo con la geometria vera, poi ore di missione (docs/10 §10.17)
add_executable(letalita strumenti/letalita.cpp)
target_link_libraries(letalita PRIVATE missioni lua54)
add_executable(rilievo_missioni strumenti/rilievo_missioni.cpp)
target_link_libraries(rilievo_missioni PRIVATE missioni)
target_compile_definitions(rilievo_missioni PRIVATE M688_DATI="${M688_DATI}" M688_MISSIONI="${CMAKE_CURRENT_SOURCE_DIR}/../data/missioni")
# missioni rapide procedurali (roadmap punto 10): il file da un codice, e la verifica su molti semi
add_executable(missione_rapida strumenti/missione_rapida.cpp)
target_link_libraries(missione_rapida PRIVATE missioni)
target_compile_definitions(missione_rapida PRIVATE M688_DATI="${M688_DATI}")
# libreria di missioni pronte (docs/17_libreria_missioni.md): costruisce i file dell'editor e i pacchetti in data/missioni/libreria
add_executable(libreria_missioni strumenti/libreria_missioni.cpp)
target_link_libraries(libreria_missioni PRIVATE missioni)
target_compile_definitions(libreria_missioni PRIVATE M688_DATI="${M688_DATI}" M688_LIBRERIA="${CMAKE_CURRENT_SOURCE_DIR}/../data/missioni/libreria")
# scene delle interazioni fra unita' dell'IA (docs/18_interazioni_ia.md, fase 5): missioni di prova in data/missioni/prove
# le serie della sandbox (docs/19 §6, V3): la stessa missione su N semi con melchior688 --sandbox, il riassunto in serie.toml
add_executable(serie_sandbox strumenti/serie_sandbox.cpp app/serie_dati.cpp)
target_include_directories(serie_sandbox PRIVATE app)
target_include_directories(serie_sandbox SYSTEM PRIVATE terzi/tomlplusplus)
target_link_libraries(serie_sandbox PRIVATE missioni)
# l'esperimento A/B del vetrino di Dio (docs/28 §5, D2): le due serie, il confronto, le pagine e il rapporto
add_executable(serie_ab strumenti/serie_ab.cpp app/confronto_ab.cpp app/schermata_confronto.cpp app/carta.cpp)
target_include_directories(serie_ab PRIVATE app piattaforma/include)   # solo le struct dei comandi (finestra.hpp), non SDL
target_include_directories(serie_ab SYSTEM PRIVATE terzi/tomlplusplus)
target_link_libraries(serie_ab PRIVATE grafica missioni)
if(M688_RILASCIO)
  target_compile_definitions(serie_ab PRIVATE M688_DATI="")   # i dati del pacchetto accanto all'eseguibile
else()
  target_compile_definitions(serie_ab PRIVATE M688_DATI="${M688_DATI}")
endif()
add_executable(scene_interazioni strumenti/scene_interazioni.cpp)
target_link_libraries(scene_interazioni PRIVATE missioni)
target_compile_definitions(scene_interazioni PRIVATE M688_DATI="${M688_DATI}" M688_MISSIONI="${CMAKE_CURRENT_SOURCE_DIR}/../data/missioni")

# anteprime 640x360 del quadro danni dell'Enhanced con lo spaccato (grafica/spaccato.hpp)
add_executable(anteprima_spaccato strumenti/anteprima_spaccato.cpp)
target_link_libraries(anteprima_spaccato PRIVATE grafica)
# le bozze della pagina SQUADRE della stazione Stato (kit, punto 4 del piano del 06/11/2026)
add_executable(bozze_squadre strumenti/bozze_squadre.cpp)
target_link_libraries(bozze_squadre PRIVATE grafica)
add_executable(bozze_comandi strumenti/bozze_comandi.cpp)
target_include_directories(bozze_comandi SYSTEM PRIVATE terzi/tomlplusplus)
target_link_libraries(bozze_comandi PRIVATE grafica)

# anteprime della grafica attorno alla missione (M4): medaglie, ritratti, cerimonie, fascicolo, debriefing
add_executable(anteprima_cerimonia strumenti/anteprima_cerimonia.cpp)
target_include_directories(anteprima_cerimonia PRIVATE app)
target_link_libraries(anteprima_cerimonia PRIVATE grafica)
add_executable(anteprima_albo_onore strumenti/anteprima_albo_onore.cpp app/albo_onore.cpp)
target_include_directories(anteprima_albo_onore PRIVATE app piattaforma/include)   # Comandi, senza la finestra
target_link_libraries(anteprima_albo_onore PRIVATE grafica)

add_executable(anteprima_sagome strumenti/anteprima_sagome.cpp)
target_link_libraries(anteprima_sagome PRIVATE grafica)
target_compile_definitions(anteprima_sagome PRIVATE M688_DATI="${M688_DATI}")

add_executable(anteprima_periscopio strumenti/anteprima_periscopio.cpp)
target_include_directories(anteprima_periscopio PRIVATE app)
target_link_libraries(anteprima_periscopio PRIVATE grafica)

add_executable(anteprima_meteo strumenti/anteprima_meteo.cpp)
target_include_directories(anteprima_meteo PRIVATE app)
target_include_directories(anteprima_meteo SYSTEM PRIVATE terzi/tomlplusplus)
target_compile_definitions(anteprima_meteo PRIVATE "TOML_ASSERT=(void)sizeof")   # un file rovinato: parse_error anche in Debug
target_link_libraries(anteprima_meteo PRIVATE grafica)

add_executable(anteprima_enciclopedia strumenti/anteprima_enciclopedia.cpp app/carta.cpp)
target_include_directories(anteprima_enciclopedia PRIVATE app)
target_include_directories(anteprima_enciclopedia SYSTEM PRIVATE terzi/tomlplusplus)
target_compile_definitions(anteprima_enciclopedia PRIVATE "TOML_ASSERT=(void)sizeof")   # un file rovinato: parse_error anche in Debug
target_link_libraries(anteprima_enciclopedia PRIVATE grafica missioni)
# tavole delle stazioni della Fase 3: strumenti delle consolle col tema del 688 e dell'Alfa (dossier 19)
add_executable(anteprima_stazioni strumenti/anteprima_stazioni.cpp)
target_link_libraries(anteprima_stazioni PRIVATE grafica)
# tavole della carta di navigazione VGA/SVGA (grafica/carta_vga): la carta di oggi e la nuova affiancate
add_executable(anteprima_carta strumenti/anteprima_carta.cpp app/carta.cpp)
target_include_directories(anteprima_carta PRIVATE app)
target_link_libraries(anteprima_carta PRIVATE grafica missioni)
# le bozze della carta F6 (kit, proposte A e B del 29/10/2026): oggi, la carta pulita, la carta grande
add_executable(bozze_carta strumenti/bozze_carta.cpp app/carta.cpp)
target_include_directories(bozze_carta PRIVATE app)
target_link_libraries(bozze_carta PRIVATE grafica missioni)
# tavola dei simboli delle viste dall'alto (grafica/simboli_carta) con la palette della carta
add_executable(anteprima_simboli strumenti/anteprima_simboli.cpp app/carta.cpp)
target_include_directories(anteprima_simboli PRIVATE app)
target_link_libraries(anteprima_simboli PRIVATE grafica missioni)
# tavola dei grafici della schermata delle serie della sandbox (grafica/grafici_serie)
add_executable(anteprima_serie strumenti/anteprima_serie.cpp app/carta.cpp)
target_include_directories(anteprima_serie PRIVATE app)
target_link_libraries(anteprima_serie PRIVATE grafica missioni)
# bozze della rifinitura estetica (roadmap 33, dossier 25): le schermate nelle varianti A, B, C, fuori dal gioco
add_executable(bozze_estetica strumenti/bozze_estetica.cpp)
target_link_libraries(bozze_estetica PRIVATE grafica)
# bozze del sonar della rifinitura estetica (dossier 25 §9): la scala lunga, il dithering, il fosforo del 1989
add_executable(bozze_sonar strumenti/bozze_sonar.cpp)
target_link_libraries(bozze_sonar PRIVATE grafica)
# le bozze della stazione Sonar in stile cascate SDR (kit, proposta e del 29/10/2026; docs/ricerca/30)
add_executable(bozze_sonar_sdr strumenti/bozze_sonar_sdr.cpp)
target_link_libraries(bozze_sonar_sdr PRIVATE grafica)
# bozze degli strumenti della rifinitura estetica (dossier 25 §8.5): statistiche e replay come schermi di consolle
add_executable(bozze_strumenti strumenti/bozze_strumenti.cpp)
target_link_libraries(bozze_strumenti PRIVATE grafica)
add_executable(libretto_tavole strumenti/libretto_tavole.cpp)
target_link_libraries(libretto_tavole PRIVATE grafica)
target_include_directories(libretto_tavole SYSTEM PRIVATE terzi/tomlplusplus)   # libretto.toml

add_executable(prova_danni strumenti/prova_danni.cpp)
target_link_libraries(prova_danni PRIVATE nucleo)
target_compile_definitions(prova_danni PRIVATE M688_DATI="${M688_DATI}")

enable_testing()
add_executable(prove test/main.cpp test/prove_debriefing_carta.cpp test/prove_coda_messaggi.cpp test/prove_codice_partita.cpp test/prove_testi_spazi.cpp app/coda_messaggi.cpp app/codice_partita.cpp test/prove_nucleo.cpp test/prove_grafica.cpp test/prove_scena.cpp test/prove_missioni.cpp test/prove_danni.cpp test/prove_effetti.cpp test/prove_danni_missione.cpp test/prove_ordini.cpp test/prove_comandi.cpp test/prove_comando.cpp test/prove_battello.cpp test/prove_armi_giocatore.cpp test/prove_sonar_giocatore.cpp test/prove_contatti_giocatore.cpp test/prove_tma.cpp test/prove_enhanced_giocatore.cpp test/prove_danni_sottomarini.cpp test/prove_cerimonia.cpp test/prove_sagome_armi.cpp test/prove_simboli_carta.cpp test/prove_grafici_serie.cpp test/prove_scala_cascata.cpp test/prove_lamiera.cpp test/prove_luci_navigazione.cpp test/prove_quadro.cpp test/prove_effetti_asw.cpp test/prove_vista_periscopio.cpp test/prove_meteo_grafica.cpp test/prove_museo_grafica.cpp test/prove_stazioni_grafica.cpp test/prove_carta_vga.cpp test/prove_artiglieria_grafica.cpp test/prove_collisioni.cpp test/prove_dinamica_sottomarino.cpp test/prove_acustica_missione.cpp test/prove_radio.cpp test/prove_elettronica.cpp test/prove_registro.cpp test/prove_replay.cpp test/prove_siluro.cpp test/prove_acustica.cpp test/prove_analisi_acustica.cpp test/prove_paesaggio_sonoro.cpp test/prove_ricarica.cpp test/prove_carico.cpp test/prove_mine.cpp test/prove_carriera.cpp test/prove_sonar_enhanced.cpp test/prove_stazione_radio.cpp test/prove_ripetitori.cpp test/prove_strumenti_carta.cpp test/prove_carta_enhanced.cpp test/prove_asw_aereo.cpp test/prove_acustica_asw.cpp test/prove_misure_sonar.cpp test/prove_acint.cpp test/prove_sonarista.cpp test/prove_ascolto.cpp test/prove_mixer_audio.cpp test/prove_musica_fm.cpp test/prove_confronto_serie.cpp test/prove_bordo_audio.cpp test/prove_audio_meteo.cpp test/prove_variazioni_acustiche.cpp test/prove_transitori_armi.cpp test/prove_campioni.cpp test/prove_acustica_artiglieria.cpp test/prove_artiglieria.cpp test/prove_collisioni_missione.cpp test/prove_bqs15.cpp test/prove_avaria_missione.cpp test/prove_percezione_ia.cpp test/prove_interventi.cpp test/prove_scafo.cpp test/prove_forze.cpp test/prove_difesa.cpp test/prove_armi_ia_missione.cpp test/prove_navigazione_missione.cpp test/prove_comandante_ia.cpp test/prove_sandbox_v2.cpp test/diag_originali.cpp test/diag_scena_runtime.cpp test/prove_acustica_urto.cpp test/prove_asw_missione.cpp test/prove_periscopi.cpp test/prove_meteo.cpp test/prove_sonarista_missione.cpp test/prove_missione_rapida.cpp test/prove_scenario_missione.cpp test/prove_editor_missione.cpp test/prove_pacchetto_missione.cpp test/prove_libreria_missioni.cpp test/prove_albo.cpp test/prove_achievement.cpp test/prove_traccia_replay.cpp test/prove_alfa_sistemi.cpp test/prove_statistiche.cpp test/prove_replay_schermo.cpp test/prove_pannello_interventi.cpp test/prove_regolazioni_esperimento.cpp test/prove_esperimento.cpp test/prove_regia_audio.cpp test/prove_percorsi_pacchetto.cpp test/prove_confronto_ab.cpp test/prove_statistiche_schermo.cpp test/prove_interazioni_scene.cpp test/prove_navigazione.cpp test/prove_missili.cpp test/prove_difese_navi.cpp test/prove_missili_missione.cpp test/prove_traffico_missione.cpp test/prove_schiera_missione.cpp test/prove_intercettazione_missione.cpp test/prove_fondo_missione.cpp test/prove_fondale_missione.cpp test/prove_ghiaccio_missione.cpp test/prove_libretto_missione.cpp test/prove_luci_missione.cpp test/prove_manovre_tma.cpp test/prove_armi_ia.cpp test/prove_suoni_classici.cpp test/prove_statistiche_sandbox.cpp test/prove_serie_sandbox.cpp test/prove_interferenza.cpp test/prove_navi_civili.cpp test/prove_traffico.cpp test/prove_schiera.cpp test/prove_vds.cpp test/prove_righe_proprie.cpp test/prove_intercettazione.cpp test/prove_rilevatore_righe.cpp test/prove_storia_cascata.cpp test/prove_proprie_missione.cpp test/prove_ecoscandaglio.cpp test/prove_quadro_fondale.cpp test/prove_ritorni_fondo.cpp test/prove_confronto_classificazioni.cpp test/prove_sondaggio_ia.cpp test/prove_campo_ghiaccio.cpp test/prove_modifica_profilo.cpp test/prove_vetrino_ghiaccio.cpp test/prove_volta_ghiaccio.cpp test/prove_portata_luci.cpp test/prove_sagoma_notte.cpp test/prove_aob_ia.cpp test/prove_stazione_missili.cpp test/prove_pagina_intercettazione.cpp test/prove_cascata_demon.cpp test/prove_righe_automatiche.cpp test/prove_vista_storia.cpp test/prove_vista_fondale.cpp test/prove_video_apertura.cpp test/prove_pagina_squadre.cpp test/prove_schermata_comandi.cpp test/prove_comandi_manovra.cpp test/prove_schermata_siluri.cpp app/sonar_enhanced.cpp app/stazione_radio.cpp app/ripetitori.cpp app/carta.cpp app/editor_missione.cpp app/albo_onore.cpp app/replay.cpp app/interfaccia.cpp app/schermata_comandi.cpp app/scena_prova.cpp app/pannello_interventi.cpp app/regolazioni_sandbox.cpp app/regia_audio.cpp app/effetti_bordo.cpp app/confronto_ab.cpp app/schermata_confronto.cpp app/replay_affiancati.cpp app/esperimento_ab.cpp app/esperimento.cpp app/statistiche.cpp app/serie_dati.cpp app/schermata_serie.cpp app/stazione_missili.cpp app/pagina_intercettazione.cpp app/cascata_demon.cpp app/righe_automatiche.cpp app/vista_storia.cpp app/vista_fondale.cpp app/video_apertura.cpp app/schermata_siluri.cpp app/debriefing.cpp app/partita.cpp)
target_include_directories(prove SYSTEM PRIVATE terzi/doctest terzi/tomlplusplus)
target_include_directories(prove PRIVATE app)
target_include_directories(prove PRIVATE piattaforma/include)   # Comandi per le schermate provate senza finestra (editor)
target_link_libraries(prove PRIVATE grafica missioni)
target_compile_definitions(prove PRIVATE M688_DATI="${M688_DATI}" M688_MISSIONI="${CMAKE_CURRENT_SOURCE_DIR}/../data/missioni")
target_compile_definitions(prove PRIVATE "TOML_ASSERT=(void)sizeof")   # un file rovinato: parse_error anche in Debug
add_test(NAME prove COMMAND prove)
