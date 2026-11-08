// melchior688: Fase 1 del motore. Apre la scena di prova al periscopio.
//
//   melchior688 [--dati CARTELLA] [--rilevamento GRADI] [--alto] [--verita]
//               [--ora HH.hh UTC] [--data AAAA-MM-GG] [--lat GRADI] [--lon GRADI]
//   melchior688 --missione SCENE1A [--seme N] [--regola classica|simulazione]
//                                                    missione originale convertita (data/missioni); regola dei danni:
//                                                    classica (script fedeli, relitti realistici) o simulazione
//   melchior688 --cartella-missioni DIR --missione X    una missione di prova fuori da data/missioni
//   melchior688 --codice XXXX-XXXX-XXXXX [...]          una missione rapida dal suo codice (missioni/missione_rapida)
//   melchior688 --codice XXXXXX-XXXXXX-XXXXXX [--seme N]  una partita dal codice partita (18 caratteri) o sandbox (30): la
//               missione, il seme e le opzioni, e per la sandbox le regolazioni per parte, la durata e gli interruttori
//               dell'IA (docs/19); --seme vince sul seme del codice (le serie: stesso vetrino, N semi)
//   melchior688 --sandbox --missione X [--seme N] [--secondi S] [--salva-traccia F] --istantanea X   il vetrino senza
//               finestra, con le regolazioni predefinite (per altre: --codice); finisce alla durata o all'esito
//   melchior688 --sandbox ... --schermata vivo|statistiche-sandbox [--pagina K] --istantanea X   le sue schermate
//   melchior688 --sandbox --esperimento F.m688v --seme N [--senza-interventi] ...   il vetrino di Dio (docs/28): il
//               codice sandbox del file e i suoi interventi programmati (senza: la partita A del confronto)
//   melchior688 --serie CARTELLA/serie.toml [--schermata serie --pagina K --istantanea X]   il riassunto di una serie
//               della sandbox (strumenti/serie_sandbox); in finestra un clic su una partita ne apre il replay
//   melchior688 --missione X --ordini FILE --riprendi-da T [...]   «Riprendi da qui»: gli ordini fino a T s, poi i propri
//   melchior688 --immagine TITLE [--nitido]          schermata originale 320x200 portata a 640x480
//   melchior688 --istantanea FILE.bmp [--secondi N] [...]   senza finestra: simula N secondi e salva la tela
//   melchior688 --colpo siluro_chiglia|siluro_fianco|siluro_prua|siluro_poppa|due_siluri|missile [--bersaglio perry]
//               |deposito|coperta|carico_in_fiamme   lo scoppio secondario subito, il carico della petroliera in fiamme
//   melchior688 ... --audio-wav FILE [--ascolto centrale|plancia]   la scena di prova col suono di bordo (scoppi, colpo)
//               [--seme N]                       fisica dei danni su una nave della scena di prova, con fuoco e fumo
//   melchior688 --scoppio KG,PROF,DIST,RIL[,FONDALE]   esplosione subacquea di prova (TNT per la bolla, m, m, gradi)
//   melchior688 --aerea KG,QUOTA,DIST,RIL[,deposito]    esplosione nell'aria di prova
//   melchior688 --vento DA,M/S                          vento del teatro per fumo e nebbia (predefinito 250,7)
//   melchior688 --onde M --visibilita KM                altezza significativa delle onde e portata visiva (1 m, 30 km)
//   melchior688 --missione X --ordini FILE              ripete gli ordini del giocatore salvati (replay)
//   melchior688 --missione X --regola simulazione --sonar 1989|moderno   aspetto e aiuti della stazione Sonar (S5a)
//   melchior688 --missione X --regola simulazione --meteo semplificato|realistico   meteo fisso (mare 3, sereno) o dinamico
//   melchior688 --missione X --regola simulazione --asw facile|normale|realistico|minaccia   difficolta' dell'ASW aereo
//   melchior688 --menu --menu-prova FILE   i comandi di ogni fotogramma da un file, per provare il giro dei menu (leggi_passi_prova)
//   melchior688 --menu --regola classica|simulazione   col menu vince sulle preferenze ([avvio] modo), senza scriverle
//   melchior688 --menu --senza-apertura   al menu senza il video d'apertura, qualunque sia la preferenza (A nel menu)
//   melchior688 --schermata apertura --secondi T --istantanea FILE [--istantanee T1,T2,...] [--audio-wav F]   il video d'apertura
//               (docs/29) senza finestra: il fotogramma al secondo T del film; col WAV, il suono del film fin li'
//   melchior688 --schermata museo [--voce ID] [--evento N] --istantanea FILE   il Museo: una voce, l'immagine N
//   melchior688 --missione X ... --istantanea FILE --istantanee T1,T2,...   anche le istantanee FILE_ttttt.bmp agli istanti dati
//   melchior688 --missione X ... --istantanea FILE --tasti-a-tempo F   comandi dei tasti a tempo: righe «secondi comando...»
//              (nomi di nucleo/mappa_tasti, piu' su, giu', sinistra e destra per le frecce tenute un passo)
//   melchior688 --missione X --regola simulazione --sonarista nessuno|recluta|medio|esperto   il sonarista dell'equipaggio
//   melchior688 --missione X --regola simulazione --navigazione perfetta|realistica   posizione propria incerta, punti nave
//   melchior688 ... --navigazione realistica --gps   anche il GPS fra i punti nave (l'opzione «1990+»)
//   melchior688 --missione X --regola simulazione --traffico si|no   il traffico civile (di norma si; mai nelle scene di prova)
//   melchior688 --audio piena|sound_blaster|originale|spento   resa dell'audio (requisito 27, R1); senza dispositivo, silenzio
//   melchior688 --missione X --regola simulazione --stile-carta vga|nautica|foglio   stile della carta (N1; B in gioco)
//   melchior688 --missione X --regola simulazione --carta epoca|vera   carta d'epoca della propria parte (predefinita) o fondale vero
//   melchior688 --menu                                  le schermate: scelta della missione, briefing, missione (Enhanced)
//   melchior688 --menu --uscita nitida|sharp [--crt|--senza-crt]   l'uscita a schermo (se no da preferenze.toml [schermo])
//   melchior688 --missione X --regola simulazione --briefing   la sala briefing prima della missione (Fase 4a, docs/12)
//   melchior688 --missione X --regola simulazione --schermata briefing --istantanea FILE   la schermata senza finestra
//   melchior688 --missione X --regola simulazione --secondi N --schermata debriefing [--evento K] --istantanea FILE (K 0 l'esito, -1 e -2 le pagine dopo)
//                                                       il debriefing dopo N secondi, sull'evento K (0: l'esito)
//   melchior688 --missione X --regola simulazione --schermata siluri [--arma ID] --istantanea FILE   la sala siluri
//   melchior688 --missione X --regola simulazione --secondi N --schermata esito [--pagina K] --istantanea FILE
//                                                       esito e cerimonie (su un fascicolo nuovo, che non si salva)
//   melchior688 --schermata replay --replay FILE [--secondi T] [--pagina 2] --istantanea X   lo schermo del replay
//   melchior688 --schermata statistiche --replay FILE --istantanea X   le statistiche della missione della traccia
//   melchior688 --schermata statistiche-cumulate [--voce ID] [--replay FILE] [--pagina N] --istantanea X   le cumulate
//                                                       all'istante T (pagina 2: la verita' invece del quadro)
//   melchior688 --missione X --registra FILE            salva gli ordini del giocatore all'uscita
//   melchior688 --missione X --stazione 3|4|6|7|8       pannello iniziale: Stato, Manovra, Navigazione, Armi, Sonar
//   melchior688 --missione X --scie tutte|bersaglio     carta con le scie accese (ALL TRACKS o TARGET TRACK)
//   melchior688 --missione X --regola simulazione --mate   carta con lo strumento MATE della TMA
//   melchior688 --missione X --regola simulazione --urto-giocatore KG,DIST,GRADI   urto di prova sul battello del
//                                                    giocatore all'avvio (quadro danni, scossa)
//   melchior688 --missione X --regola simulazione --urto-unita ID,KG,DIST,GRADI    lo stesso su un sottomarino qualsiasi
//   melchior688 --missione X --periscopio-su ID         periscopio puntato sull'unita' ID (anche con --istantanea)
//   melchior688 --tasti FILE                            tasti rimappati (predefinito: ~/.config/melchior688/tasti.toml)
//   melchior688 --scrivi-tasti FILE                     scrive il modello del file dei tasti ed esce

#include <atomic>
#include <mutex>
#include <thread>
#include <cctype>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <deque>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <limits>
#include <map>
#include <sstream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include <toml.hpp>

#include "grafica/effetti.hpp"
#include "grafica/font.hpp"
#include "grafica/modello.hpp"
#include "grafica/cielo.hpp"
#include "grafica/palette.hpp"
#include "grafica/periscopio.hpp"
#include "grafica/riscala.hpp"
#include "grafica/sprite.hpp"
#include "grafica/tela.hpp"
#include "missioni/griglia_ghiaccio.hpp"
#include "missioni/stazioni_missione.hpp"
#include "missioni/danni_missione.hpp"
#include "missioni/danni_sottomarini.hpp"
#include "missioni/missione.hpp"
#include "missioni/traccia_missione.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/comando.hpp"
#include "nucleo/danni.hpp"
#include "nucleo/fondale.hpp"
#include "nucleo/mappa_tasti.hpp"
#include "nucleo/mondo.hpp"
#include "nucleo/ordini.hpp"
#include "nucleo/orologio.hpp"
#include "nucleo/sole.hpp"
#include "nucleo/statistiche.hpp"
#include "nucleo/testi.hpp"
#include "nucleo/ascolto.hpp"
#include "missioni/comandante_ia.hpp"
#include "missioni/sandbox_originali.hpp"
#include "nucleo/mixer_audio.hpp"
#include "piattaforma/audio.hpp"
#include "piattaforma/finestra.hpp"
#include "briefing.hpp"
#include "data_missione.hpp"
#include "debriefing.hpp"
#include "esito.hpp"
#include "schermata_siluri.hpp"
#include "scelta_missione.hpp"
#include "interfaccia.hpp"
#include "museo.hpp"
#include "replay.hpp"
#include "statistiche.hpp"
#include "albo.hpp"
#include "schermata_modo.hpp"
#include "albo_onore.hpp"
#include "bacheca.hpp"
#include "coda_messaggi.hpp"
#include "codice_partita.hpp"
#include "musica_titoli.hpp"
#include "percorsi_pacchetto.hpp"
#include "effetti_bordo.hpp"
#include "regia_audio.hpp"
#include "regolazioni_sandbox.hpp"
#include "registro_tempi.hpp"
#include "video_apertura.hpp"
#include "schermata_serie.hpp"
#include "replay.hpp"
#include "editor_missione.hpp"
#include "esperimento.hpp"
#include "esperimento_ab.hpp"
#include "partita.hpp"
#include "risorse.hpp"
#include "scena_prova.hpp"

using namespace m688;

namespace {

struct Opzioni {
    std::string dati = cartella_dati(eseguibile_in_corso(), "data/motore", M688_DATI_PREDEFINITI);   // il pacchetto, poi la build
    std::string istantanea;
    std::string immagine;
    std::string missione;
    std::string cartella_missioni = cartella_dati(eseguibile_in_corso(), "data/missioni", M688_MISSIONI_PREDEFINITE);
    std::uint64_t seme = 1;
    std::string colpo, bersaglio = "perry";
    RegolaDanni regola = RegolaDanni::classica;
    bool regola_data = false;          // --regola: col menu vince sulle preferenze ([avvio] modo)
    std::string menu_prova;            // --menu-prova FILE: i comandi di ogni fotogramma da un file, per le prove del menu (leggi_passi_prova)
    std::vector<std::string> scoppi, aeree;
    std::string ordini, registra;   // replay degli ordini del giocatore e loro registrazione
    int stazione = 4;               // pannello in missione: 3 Stato, 4 Manovra, 6 Navigazione, 7 Armi, 8 Sonar
    std::string scie;               // carta: "tutte" o "bersaglio"
    bool mate = false;              // carta: strumento MATE
    std::string urto_giocatore;     // urto di prova all'avvio: KG,DIST,GRADI (regola di simulazione)
    std::string urto_unita;         // ID,KG,DIST,GRADI: su un sottomarino qualsiasi
    std::string tasti, scrivi_tasti;   // file dei tasti rimappati; modello da scrivere
    std::string uscita;                // --uscita nitida|sharp: l'uscita a schermo (vuota: dalle preferenze)
    int crt = -1;                      // --crt / --senza-crt (-1: dalle preferenze)
    int periscopio_su = -1;         // unita' su cui tenere puntato il periscopio (-1: nessuna)
    std::string sonar = "moderno";  // stazione Sonar dell'Enhanced: 1989 o moderno
    std::string audio = "piena";
    bool tutti = false;             // debriefing senza finestra: tutti gli eventi (come Tab)
    std::string voce;               // Museo senza finestra: la voce dell'enciclopedia da aprire
    std::string replay;             // --schermata replay: la traccia (.m688tr) da aprire
    std::string salva_traccia;      // senza finestra: la traccia del replay della partita in questo file (prove)
    std::string serie;              // --serie: il riassunto di una serie della sandbox (serie.toml) da mostrare
    std::string audio_wav;          // senza finestra: l'audio della partita (per ora i suoni del classico) in questo WAV
    std::string ascolto = "centrale";   // la scena di prova con --audio-wav: dove si ascolta (centrale, immersi; plancia, sul mare)
    bool senza_apertura = false;    // --senza-apertura: al menu senza il video, qualunque sia la preferenza
    bool sandbox = false;           // il vetrino dell'IA (docs/19_sandbox.md): nessun giocatore, fuori carriera
    RegolazioniSandbox regolazioni; // le sue regolazioni (V dalla scelta della missione)
    std::vector<double> istantanee;  // senza finestra: altre istantanee agli istanti dati, dalla stessa partita
    std::string tasti_a_tempo;      // senza finestra: file dei comandi dei tasti a tempo
    std::string meteo = "semplificato";   // Enhanced: meteo fisso o dinamico del teatro e della stagione
    std::string asw = "normale";          // Enhanced: difficolta' dell'ASW aereo (dossier 17 §10.3)
    std::string navigazione = "perfetta";   // Enhanced: perfetta o realistica (posizione propria incerta, punti nave)
    bool gps = false;                       // Enhanced, navigazione realistica: il GPS fra i punti nave (opzione «1990+»)
    bool verita_usata = false;              // la verita' dei contatti (V) accesa almeno una volta: un aiuto (docs/26 §1)
    bool traffico = true;                   // Enhanced: il traffico civile (docs/20 §3); mai nelle scene di prova
    std::string sonarista = "medio";      // Enhanced: il sonarista dell'equipaggio (nessuno, recluta, medio, esperto)    // resa dell'audio: piena, sound_blaster, originale o spento
    std::string stile_carta = "vga";   // carta dell'Enhanced: vga, nautica o foglio
    std::string carta = "epoca";       // carta dell'Enhanced: la carta d'epoca della parte o il fondale vero
    bool briefing = false;             // Enhanced: la sala briefing prima della missione (Fase 4a)
    bool menu = false;                 // avvio dalle schermate: scelta della missione, briefing, missione (Enhanced)
    std::string schermata;             // con --istantanea: «briefing» disegna la schermata invece della missione
    int evento = 1;                    // --schermata debriefing: l'evento da mostrare (0: l'esito)
    std::string arma;                  // --schermata siluri: l'arma scelta nel deposito
    int pagina = 1;                    // --schermata esito: la pagina (1 il rapporto, poi le cerimonie)
    std::optional<bool> variazioni;    // Enhanced: variazioni acustiche R6 (si/no; senza: il predefinito della missione)
    std::string codice;                // una missione rapida dal suo codice (senza menu)
    std::string esperimento;           // il vetrino di Dio (docs/28, D2): un file .m688v, codice sandbox piu' interventi
    bool senza_interventi = false;     // la partita A dell'esperimento: lo stesso codice, senza gli interventi
    bool consolle = true;              // Enhanced: i pannelli delle consolle della Fase 3 (--consolle no: quelli di prima)
    double riprendi_da = -1.0;         // con --ordini: «Riprendi da qui» all'istante dato (s), gli ordini dopo si scartano
    double vento_da = 250.0, vento = 7.0;
    bool nitido = false;
    double secondi = 0.0;
    VistaPeriscopio vista;
    // teatro della scena di prova: a sud delle Faroe (passaggio GIUK), meta' ottobre 1989
    double latitudine = 61.5, longitudine = -7.0;
    bool luogo_dato = false;           // --lat/--lon: senza, l'Enhanced prende il centro del teatro della missione
    DataOra inizio{1989, 10, 15, 9.0};
    bool data_data = false;            // --data/--ora: senza, le originali dell'Enhanced partono dalla data dei loro ordini
    bool seme_dato = false;            // --seme vince sul seme del --codice (le serie della sandbox: stesso codice, N semi)
};

Opzioni leggi_opzioni(int argc, char** argv) {
    Opzioni o;
    // lanciato da solo dal pacchetto di rilascio: il menu (nella build di sviluppo resta la scena di prova)
    if (argc == 1 && !cartella_accanto(eseguibile_in_corso(), "data/motore").empty()) o.menu = true;
    for (int i = 1; i < argc; ++i) {
        const std::string a = argv[i];
        auto valore = [&]() -> std::string {
            if (i + 1 >= argc) throw std::runtime_error("manca il valore di " + a);
            return argv[++i];
        };
        if (a == "--dati") o.dati = valore();
        else if (a == "--istantanea") o.istantanea = valore();
        else if (a == "--immagine") o.immagine = valore();
        else if (a == "--nitido") o.nitido = true;
        else if (a == "--missione") o.missione = valore();
        else if (a == "--cartella-missioni") o.cartella_missioni = valore();   // missioni di prova fuori da data/missioni
        else if (a == "--seme") o.seme = std::stoull(valore()), o.seme_dato = true;
        else if (a == "--colpo") o.colpo = valore();
        else if (a == "--bersaglio") o.bersaglio = valore();
        else if (a == "--regola") {
            const std::string v = valore();
            o.regola_data = true;
            if (v == "classica") o.regola = RegolaDanni::classica;
            else if (v == "simulazione") o.regola = RegolaDanni::simulazione;
            else throw std::runtime_error("regola dei danni sconosciuta (classica, simulazione): " + v);
        }
        else if (a == "--scoppio") o.scoppi.push_back(valore());
        else if (a == "--onde") o.vista.altezza_onde_m = std::stod(valore());
        else if (a == "--ordini") o.ordini = valore();
        else if (a == "--riprendi-da") o.riprendi_da = std::stod(valore());
        else if (a == "--consolle") o.consolle = valore() != "no";
        else if (a == "--codice") o.codice = valore();
        else if (a == "--esperimento") o.esperimento = valore();
        else if (a == "--senza-interventi") o.senza_interventi = true;
        else if (a == "--variazioni") o.variazioni = valore() != "no";
        else if (a == "--briefing") o.briefing = true;
        else if (a == "--menu") o.menu = true;
        else if (a == "--menu-prova") o.menu_prova = valore();
        else if (a == "--schermata") o.schermata = valore();
        else if (a == "--evento") o.evento = std::stoi(valore());
        else if (a == "--arma") o.arma = valore();
        else if (a == "--pagina") o.pagina = std::stoi(valore());
        else if (a == "--carta") {
            o.carta = valore();
            if (o.carta != "epoca" && o.carta != "vera") throw std::runtime_error("--carta vuole epoca o vera");
        } else if (a == "--stile-carta") {
            o.stile_carta = valore();
            if (o.stile_carta != "vga" && o.stile_carta != "nautica" && o.stile_carta != "foglio")
                throw std::runtime_error("--stile-carta vuole vga, nautica o foglio");
        } else if (a == "--meteo") {
            o.meteo = valore();
            if (o.meteo != "semplificato" && o.meteo != "realistico") throw std::runtime_error("--meteo vuole semplificato o realistico");
        } else if (a == "--sonarista") {
            o.sonarista = valore();
            if (o.sonarista != "nessuno" && o.sonarista != "recluta" && o.sonarista != "medio" && o.sonarista != "esperto")
                throw std::runtime_error("--sonarista vuole nessuno, recluta, medio o esperto");
        } else if (a == "--asw") {
            o.asw = valore();
            if (o.asw != "facile" && o.asw != "normale" && o.asw != "realistico" && o.asw != "minaccia")
                throw std::runtime_error("--asw vuole facile, normale, realistico o minaccia");
        } else if (a == "--istantanee") {
            std::stringstream ss(valore());
            for (std::string x; std::getline(ss, x, ',');) o.istantanee.push_back(std::stod(x));
            std::sort(o.istantanee.begin(), o.istantanee.end());
        } else if (a == "--tasti-a-tempo") {
            o.tasti_a_tempo = valore();
        } else if (a == "--replay") {
            o.replay = valore();
        } else if (a == "--salva-traccia") {
            o.salva_traccia = valore();
        } else if (a == "--voce") {
            o.voce = valore();
        } else if (a == "--tutti") {
            o.tutti = true;
        } else if (a == "--audio") {
            o.audio = valore();
            if (o.audio != "piena" && o.audio != "sound_blaster" && o.audio != "originale" && o.audio != "spento")
                throw std::runtime_error("--audio vuole piena, sound_blaster, originale o spento");
        } else if (a == "--sonar") {
            o.sonar = valore();
            if (o.sonar != "1989" && o.sonar != "moderno") throw std::runtime_error("--sonar vuole 1989 o moderno");
        }
        else if (a == "--registra") o.registra = valore();
        else if (a == "--stazione") o.stazione = std::atoi(valore().c_str());
        else if (a == "--scie") o.scie = valore();
        else if (a == "--mate") o.mate = true;
        else if (a == "--urto-giocatore") o.urto_giocatore = valore();
        else if (a == "--urto-unita") o.urto_unita = valore();
        else if (a == "--tasti") o.tasti = valore();
        else if (a == "--uscita") o.uscita = valore();
        else if (a == "--crt") o.crt = 1;
        else if (a == "--senza-crt") o.crt = 0;
        else if (a == "--periscopio-su") o.periscopio_su = std::atoi(valore().c_str());
        else if (a == "--scrivi-tasti") o.scrivi_tasti = valore();
        else if (a == "--visibilita") o.vista.visibilita_m = std::stod(valore()) * 1000.0;
        else if (a == "--aerea") o.aeree.push_back(valore());
        else if (a == "--vento") {
            const std::string w = valore();
            if (std::sscanf(w.c_str(), "%lf,%lf", &o.vento_da, &o.vento) != 2) throw std::runtime_error("vento non valido (DA,M/S): " + w);
        }
        else if (a == "--secondi") o.secondi = std::stod(valore());
        else if (a == "--rilevamento") o.vista.rilevamento = std::stod(valore());
        else if (a == "--alto") o.vista.alto_ingrandimento = true;
        else if (a == "--verita") o.vista.mostra_verita = true;
        else if (a == "--navigazione") o.navigazione = valore();
        else if (a == "--gps") o.gps = true;
        else if (a == "--traffico") {
            const std::string v = valore();
            if (v != "si" && v != "no") throw std::runtime_error("--traffico vuole si o no");
            o.traffico = v == "si";
        }
        else if (a == "--audio-wav") o.audio_wav = valore();
        else if (a == "--senza-apertura") o.senza_apertura = true;
        else if (a == "--ascolto") o.ascolto = valore();
        else if (a == "--serie") o.serie = valore();
        else if (a == "--sandbox") o.sandbox = true, o.regola = RegolaDanni::simulazione;   // senza menu: le regolazioni predefinite, nell'Enhanced come dal menu
        else if (a == "--ora") o.inizio.ore_utc = std::stod(valore()), o.data_data = true;
        else if (a == "--lat") o.latitudine = std::stod(valore()), o.luogo_dato = true;
        else if (a == "--lon") o.longitudine = std::stod(valore()), o.luogo_dato = true;
        else if (a == "--data") {
            const std::string d = valore();
            if (std::sscanf(d.c_str(), "%d-%d-%d", &o.inizio.anno, &o.inizio.mese, &o.inizio.giorno) != 3)
                throw std::runtime_error("data non valida (AAAA-MM-GG): " + d);
            o.data_data = true;
        } else throw std::runtime_error("opzione sconosciuta: " + a);
    }
    return o;
}

// Disegna la scena e restituisce la palette con la luce del momento.
Palette componi(Tela& t, const Risorse& r, const Periscopio& p, const Mondo& m, VistaPeriscopio v,
                const Opzioni& o, StatoPannello s, int id_proprio, double tempo_s, const Effetti* effetti,
                std::vector<AreaAttiva>* aree = nullptr, bool con_vista = true) {
    DataOra ora = o.inizio;
    ora.ore_utc = std::fmod(o.inizio.ore_utc + tempo_s / 3600.0, 24.0);
    s.tempo_s = ora.ore_utc * 3600.0;
    s.elevazione_sole = elevazione_sole(ora, o.latitudine, o.longitudine);
    const Nave* np = m.trova(id_proprio);
    if (!np) throw std::runtime_error("il sottomarino del giocatore non c'e' piu'");
    const Nave& proprio = *np;
    v.luce = cielo::luce_sotto_le_nubi(luce_ambiente(s.elevazione_sole), v.meteo);   // il coperto toglie luce (meteo)
    // Enhanced: la luna schiarisce la notte quanto la vede l'IA (i lux del suo cielo, con la fase e le nubi, resi come la
    // luce del crepuscolo con lo stesso illuminamento); il classico non ha il cielo lunare
    if (v.cielo_lunare) v.luce = std::max(v.luce, cielo::luce_della_notte(v.cielo.lux));
    if (effetti) {   // artiglieria (docs/16, G6): l'acqua delle colonne vicine sul vetro e la luce degli illuminanti
        v.oculare.spruzzi_proietti = std::max(v.oculare.spruzzi_proietti, effetti->acqua_sul_vetro(proprio.pos));
        double illuminanti = 0.0;   // sul mare dove guarda il periscopio
        for (const double d : {300.0, 1000.0, 2500.0, 5000.0}) {
            const Vec2 q = vettore(v.rilevamento, d);
            illuminanti = std::max(illuminanti, effetti->luce_illuminanti({proprio.pos.x + q.x, proprio.pos.y + q.y}));
        }
        v.luce = std::min(1.0, v.luce + illuminanti);
    }
    v.tempo_s = tempo_s;
    if (con_vista) p.disegna(t, m, proprio, v, effetti);
    else t.rettangolo_pieno(0, 0, LARGHEZZA_TELA, ALTEZZA_VISTA, colori::NERO);   // la stazione la copre tutta
    disegna_pannello(t, r.font, proprio, v, s, aree);
    Palette pal = r.giorno.illuminata(v.luce, INDICI_VISTA);
    oculare::palette(pal, v.oculare);   // Enhanced: la rampa 160-174 dei modi monocromi e i filtri dell'oculare
    if (const double l = cielo::lampo(v.meteo, tempo_s, v.luce); l > 0.0) pal = pal.schiarita(l, INDICI_VISTA);   // fulmini
    if (effetti && effetti->lampo() > 0) pal = pal.schiarita(0.6 * effetti->lampo(), INDICI_VISTA);   // lampo di un'esplosione
    if (const auto tema = tema_pannello(s, v.luce)) applica_tema(pal, *tema);   // Fase 3: le consolle, dopo la luce della vista
    return pal;
}

// Schermata originale 320x200 (Scale2x e 5:6) con la sua palette.
Palette mostra_immagine(Tela& t, const Opzioni& o) {
    const auto im = porta_a_640x480(ImmagineIndicizzata::carica(o.dati + "/immagini/" + o.immagine + ".img"), o.nitido);
    for (int y = 0; y < im.h; ++y)
        for (int x = 0; x < im.w; ++x) t.pixel(x, y, im.pix[static_cast<std::size_t>(y) * im.w + x]);
    return Palette::carica(o.dati + "/immagini/" + o.immagine + ".pal");
}

// Il realismo della partita per il punteggio della carriera (docs/26 §1): 1 senza aiuti, meno il peso di ogni aiuto
// acceso, fino a 0,5. Il classico resta a 1. La navigazione esatta pesa piu' del GPS, che da' solo punti nave
// Con l'elenco degli aiuti accesi e il loro peso, che il rapporto di pattuglia mostra
RealismoPartita realismo_partita(const Opzioni& o, bool colore_rilevamento = false) {
    RealismoPartita r;
    if (o.regola != RegolaDanni::simulazione) return r;
    const auto aiuto = [&](bool acceso, const char* nome, double peso) {
        if (acceso) r.aiuti.push_back({nome, peso});
    };
    aiuto(o.navigazione != "realistica", "NAVIGAZIONE ESATTA", 0.10);
    aiuto(o.navigazione == "realistica" && o.gps, "GPS 1990+", 0.05);
    aiuto(o.asw == "facile", "DIFFICOLTA' FACILE", 0.10);   // l'aiuto del telemetro e l'ASW aereo piu' blando
    aiuto(o.verita_usata, "VERITA' DEI CONTATTI", 0.05);
    aiuto(o.scie == "tutte", "SCIE DI TUTTE LE UNITA'", 0.05);
    aiuto(o.sonarista == "esperto", "SONARISTA ESPERTO", 0.05);
    aiuto(o.variazioni && !*o.variazioni, "SENZA VARIAZIONI ACUSTICHE", 0.05);
    aiuto(colore_rilevamento, "SONAR: COLORE PER RILEVAMENTO", 0.05);   // acceso almeno una volta (sonar moderno)
    for (const auto& a : r.aiuti) r.valore -= a.peso;
    r.valore = std::max(r.valore, 0.5);
    return r;
}

// --periscopio-su: il periscopio segue un'unita' (istantanee degli effetti, verifiche a vista)
void punta_periscopio(const Mondo& mondo, int id_proprio, Opzioni& o) {
    if (o.periscopio_su < 0) return;
    const Nave* p = mondo.trova(id_proprio);
    const Nave* b = mondo.trova(o.periscopio_su);
    if (p && b) o.vista.rilevamento = normalizza_360(rilevamento(p->pos, b->pos));
}

// Tasti: quelli predefiniti, poi il file indicato o quello dell'utente in ~/.config/melchior688/tasti.toml
MappaTasti carica_tasti(const std::string& file) {
    MappaTasti m = MappaTasti::predefinita();
    std::string percorso = file;
    if (percorso.empty())
        if (const char* casa = std::getenv("HOME")) {
            const std::string utente = std::string(casa) + "/.config/melchior688/tasti.toml";
            if (std::ifstream(utente)) percorso = utente;
        }
    if (!percorso.empty()) m.carica(percorso);
    return m;
}

// Le preferenze dell'utente fra le partite, in ~/.config/melchior688/preferenze.toml accanto ai tasti: per ora il grado
// della persistenza del fosforo del sonar del 1989 (roadmap 33, scelta dell'utente del 28/10/2026). 1 (normale) se
// il file non c'e'
std::filesystem::path percorso_preferenze() {
    const char* casa = std::getenv("HOME");
    return casa ? std::filesystem::path(casa) / ".config" / "melchior688" / "preferenze.toml" : std::filesystem::path{};
}

int leggi_persistenza_sonar() {
    try {
        const auto p = percorso_preferenze();
        if (p.empty() || !std::filesystem::exists(p)) return 1;
        const toml::table t = toml::parse_file(p.string());
        return std::clamp(static_cast<int>(t["sonar"]["persistenza"].value_or(1)), 0, 2);
    } catch (const std::exception&) {
        return 1;
    }
}

// L'uscita a schermo (E9 §2.1, kit 31/10/2026): [schermo] uscita = "nitida" (predefinita) o "sharp_bilinear", crt = false
struct UscitaSchermo {
    Finestra::Uscita uscita = Finestra::Uscita::nitida;
    bool crt = false;
};

UscitaSchermo leggi_uscita_schermo() {
    UscitaSchermo u;
    try {
        const auto p = percorso_preferenze();
        if (p.empty() || !std::filesystem::exists(p)) return u;
        const toml::table t = toml::parse_file(p.string());
        if (t["schermo"]["uscita"].value_or(std::string{}) == "sharp_bilinear") u.uscita = Finestra::Uscita::sharp_bilinear;
        u.crt = t["schermo"]["crt"].value_or(false);
    } catch (const std::exception&) {
    }
    return u;
}

// L'audio (roadmap 38): [audio] musica = 0..1, il volume della musica (predefinito 0,7); musica_uscita, la voce di M
// nel menu: "fm", "mt32" (i WAV resi dalle ROM dell'utente nella sua cache, dove ci sono), "adlib", "tandy", "covox"
// (i titoli di Hubbard del 1989) o "spenta". Senza la voce, la scelta vecchia di musica.txt (app/musica_titoli), se c'e'
const char* const USCITE_MUSICA[] = {"fm", "mt32", "adlib", "tandy", "covox", "spenta"};   // nell'ordine di M
struct PreferenzeAudio {
    float musica = 0.7f;
    std::string uscita = "fm";
};

PreferenzeAudio leggi_preferenze_audio() {
    PreferenzeAudio a;
    bool con_uscita = false;
    try {
        const auto p = percorso_preferenze();
        if (!p.empty() && std::filesystem::exists(p)) {
            const toml::table t = toml::parse_file(p.string());
            a.musica = static_cast<float>(std::clamp(t["audio"]["musica"].value_or(0.7), 0.0, 1.0));
            const std::string u = t["audio"]["musica_uscita"].value_or(std::string{});
            for (const char* x : USCITE_MUSICA)
                if (u == x) a.uscita = u, con_uscita = true;
        }
    } catch (const std::exception&) {
    }
    if (!con_uscita && std::filesystem::exists(cartella_fascicoli().parent_path() / "musica.txt")) {   // la migrazione
        const VersioneMusica v = leggi_preferenza_musica();
        a.uscita = v == VersioneMusica::adlib ? "adlib" : v == VersioneMusica::tandy ? "tandy" : v == VersioneMusica::covox ? "covox" : "spenta";
    }
    return a;
}

// Il video d'apertura (docs/29): [avvio] video = "sempre" (predefinito), "prima" (solo la prima volta) o "mai"; visto, se e'
// gia' passato una volta
const char* const QUANDO_VIDEO[] = {"sempre", "prima", "mai"};   // nell'ordine di A nel menu
struct PreferenzeAvvio {
    int video = 0;
    bool visto = false;
    int modo = -1;   // [avvio] modo: -1 mai scelto, 0 enhanced, 1 classico
};

PreferenzeAvvio leggi_preferenze_avvio() {
    PreferenzeAvvio a;
    try {
        const auto p = percorso_preferenze();
        if (p.empty() || !std::filesystem::exists(p)) return a;
        const toml::table t = toml::parse_file(p.string());
        const std::string v = t["avvio"]["video"].value_or(std::string{});
        for (int k = 0; k < 3; ++k)
            if (v == QUANDO_VIDEO[k]) a.video = k;
        a.visto = t["avvio"]["visto"].value_or(false);
        const std::string m = t["avvio"]["modo"].value_or(std::string{});
        if (m == "enhanced") a.modo = 0;
        if (m == "classico") a.modo = 1;
    } catch (const std::exception&) {
    }
    return a;
}

// il file si riscrive intero: il sonar, lo schermo, l'audio e l'avvio insieme (`audio` e `avvio` vuoti: quelli del file)
void scrivi_preferenze(int grado, const UscitaSchermo& u, const std::optional<PreferenzeAudio>& audio = std::nullopt,
                       const std::optional<PreferenzeAvvio>& avvio = std::nullopt) {
    const PreferenzeAvvio av = avvio ? *avvio : leggi_preferenze_avvio();   // prima di aprire il file, che si svuota
    const auto p = percorso_preferenze();
    if (p.empty()) return;
    std::error_code e;
    std::filesystem::create_directories(p.parent_path(), e);
    std::ofstream f(p);
    f << "# preferenze di melchior688\n[sonar]\npersistenza = " << grado << "   # 0 breve, 1 normale, 2 lunga\n";
    f << "[schermo]\nuscita = \"" << (u.uscita == Finestra::Uscita::sharp_bilinear ? "sharp_bilinear" : "nitida")
      << "\"   # nitida o sharp_bilinear\ncrt = " << (u.crt ? "true" : "false") << "\n";
    const PreferenzeAudio a = audio ? *audio : leggi_preferenze_audio();
    f << "[audio]\nmusica = " << a.musica << "   # il volume della musica, da 0 a 1\nmusica_uscita = \"" << a.uscita
      << "\"   # fm, mt32 (i WAV resi con l'MT-32 nella cache dell'utente), adlib, tandy, covox (1989) o spenta\n";
    f << "[avvio]\nvideo = \"" << QUANDO_VIDEO[std::clamp(av.video, 0, 2)] << "\"   # il video d'apertura: sempre, prima (la prima volta) o mai\nvisto = "
      << (av.visto ? "true" : "false") << "\n";
    if (av.modo >= 0) f << "modo = \"" << (av.modo == 1 ? "classico" : "enhanced") << "\"   # classico o enhanced: si cambia nel menu col tasto O\n";
}

void scrivi_persistenza_sonar(int grado) { scrivi_preferenze(grado, leggi_uscita_schermo()); }

// le preferenze dello schermo, con --uscita e --crt sopra
void applica_uscita(Finestra& f, const Opzioni& o) {
    UscitaSchermo u = leggi_uscita_schermo();
    if (o.uscita == "sharp" || o.uscita == "sharp_bilinear") u.uscita = Finestra::Uscita::sharp_bilinear;
    else if (o.uscita == "nitida") u.uscita = Finestra::Uscita::nitida;
    if (o.crt >= 0) u.crt = o.crt == 1;
    if (u.uscita != Finestra::Uscita::nitida || u.crt) f.imposta_uscita(u.uscita, u.crt);
    // F10 cambia l'uscita in ogni schermata: la scelta resta nelle preferenze
    f.al_cambio_uscita([](Finestra::Uscita nuova, bool crt) { scrivi_preferenze(leggi_persistenza_sonar(), {nuova, crt}); });
}

// «Riprendi da qui» (roadmap 16, debriefing): la stessa partita (missione, seme, opzioni) rigiocata con lo storico
// degli ordini fino all'istante scelto, col carico di partenza; da li' comanda di nuovo il giocatore
struct Ripresa {
    RegistroOrdini ordini;
    double tempo = 0.0;   // s di missione
    std::vector<m688::Intervento> interventi;   // il vetrino di Dio (docs/28): quelli tenuti, agli istanti in cui scattarono
};

Ripresa ripresa_da(const RegistroOrdini& storico, double tempo) {
    Ripresa r;
    r.tempo = std::max(0.0, tempo);
    if (storico.carico()) r.ordini.imposta_carico(*storico.carico());
    for (const Ordine& x : storico.storico())
        if (x.tempo < r.tempo) r.ordini.ordina(x);
    return r;
}

// La stessa da un file di ordini salvato (--ordini con --riprendi-da): le righe d'ordine da `tempo` in poi si scartano,
// carico e commenti restano
Ripresa ripresa_da_testo(const std::string& testo, double tempo) {
    std::string tenuto;
    std::istringstream in(testo);
    for (std::string riga; std::getline(in, riga);) {
        std::istringstream campi(riga);
        double t = 0.0;
        if (!riga.empty() && riga[0] != '#' && (campi >> t) && t >= tempo) continue;
        tenuto += riga + "\n";
    }
    return {RegistroOrdini::da_testo(tenuto), std::max(0.0, tempo), {}};
}

// L'audio del gioco con la finestra (roadmap 38, app/regia_audio): una regia e un'uscita per tutto il processo, aperte
// con la prima finestra. Ogni schermata presenta col suo brano (vuoto: silenzio; lo stesso continua); i clic, Invio ed
// Esc fanno il clic dell'interfaccia, tranne in mare nel classico; Ctrl+Q (muto) spegne tutto, tranne in mare nel
// classico, dove resta il muto del 1989 (app/interfaccia)
std::unique_ptr<RegiaAudio> regia;
constexpr double ANTICIPO_CANALI_S = 0.1;   // cuffia e bordo, prodotti a fotogramma: un fotogramma piu' lungo lascia un buco in loro, non nella musica
// Il film in onda: il suo suono viene dalla coda (gioca_apertura)
std::mutex film_mutex;
CodaFilm* film_in_onda = nullptr;
std::unique_ptr<UscitaAudio> uscita_gioco;
bool classico_in_mare = false;

// I buchi dei canali continui, col registro dei tempi acceso: campioni chiesti oltre la coda, se il canale ha suonato da poco (il filo dell'audio)
std::atomic<long> buco_cuffia_filo{0}, buco_bordo_filo{0};
void conta_buchi(Canale c, std::size_t n, long& vivo, std::atomic<long>& buco) {
    constexpr long RESTA_VIVO = 24000;
    const auto attesa = regia->mixer().in_attesa(c);
    if (attesa > 0) vivo = RESTA_VIVO;
    else vivo = std::max(0L, vivo - static_cast<long>(n));
    if (vivo > 0 && n > attesa) buco.fetch_add(static_cast<long>(n - attesa), std::memory_order_relaxed);
}

void apri_audio(const Opzioni& o) {
    if (regia) return;
    if (o.audio != "spento") {
        std::string motivo;
        uscita_gioco = UscitaAudio::apri(48000, &motivo);
        if (!uscita_gioco) std::fprintf(stderr, "audio non disponibile: %s\n", motivo.c_str());
        // all'uscita da main la finestra ha gia' chiuso SDL (SDL_Quit): il flusso audio non va piu' distrutto, lo fa il sistema.
        // Senza, il distruttore di questo globale libera due volte («double free» all'uscita). atexit gira prima di quel distruttore
        // Anche la regia: il filo dell'audio la usa finche' SDL non si chiude
        if (uscita_gioco) std::atexit([] {
            (void)uscita_gioco.release();
            (void)regia.release();
        });
    }
    regia = std::make_unique<RegiaAudio>(o.dati, uscita_gioco ? uscita_gioco->campionamento_hz() : 48000);
    if (uscita_gioco) uscita_gioco->tira([](std::size_t n) {
        if (RegistroTempi::attivo()) {   // solo sul filo: nessun lucchetto in comune col thread dei fotogrammi per piu' di una somma
            static long viva_cuffia = 0, vivo_bordo = 0;
            conta_buchi(Canale::cuffia, n, viva_cuffia, buco_cuffia_filo);
            conta_buchi(Canale::bordo, n, vivo_bordo, buco_bordo_filo);
        }
        {
            std::lock_guard blocco(film_mutex);
            if (film_in_onda) return film_in_onda->campioni(n);
        }
        return regia->produci_dal_filo(n);
    });
    const PreferenzeAudio a = leggi_preferenze_audio();
    regia->volume_musica(a.musica);
    regia->uscita_musica(a.uscita == "mt32" ? UscitaMusica::mt32 : UscitaMusica::fm);
}

// La musica del menu per la voce di M (USCITE_MUSICA): il tema dell'Enhanced con l'FM o l'MT-32, i titoli di Hubbard
// del 1989 (data/motore/musica/titolo_*.wav, una volta, come all'avvio del gioco originale) o niente. Il brano da
// presentare
std::string musica_del_menu(const Opzioni& o, int voce) {
    if (!regia) return {};
    if (voce == 0 || voce == 1) {
        regia->uscita_musica(voce == 1 ? UscitaMusica::mt32 : UscitaMusica::fm);
        return "menu";
    }
    if (voce < 2 || voce > 4) return {};
    static constexpr const char* FILE[] = {"titolo_adlib.wav", "titolo_tandy.wav", "titolo_covox.wav"};
    const std::string nome = std::string("titoli_") + USCITE_MUSICA[voce];
    if (regia->brano() == nome) return nome;
    std::vector<float> x;
    int fs = 0;
    if (!leggi_wav(o.dati + "/musica/" + FILE[voce - 2], x, fs) || fs <= 0) return {};
    regia->musica_pcm(nome, std::move(x), fs);
    return nome;
}

// Con la finestra il suono lo chiede il filo dell'audio: al thread dei fotogrammi resta il cambio di brano, che legge i file
void alimenta_audio() {
    if (regia && uscita_gioco) regia->aggiorna_brano();
}

// Una riga del registro dei tempi, se e' acceso; il primo fotogramma scrive anche com'e' fatta l'uscita
void chiudi_fotogramma(const Finestra& w, const char* luogo, int scena, double missione_s) {
    RegistroTempi* rt = RegistroTempi::attivo();
    if (!rt) return;
    static bool intestato = false;
    static std::uint64_t ritardo_prima = 0, tirati_prima = 0;
    if (!std::exchange(intestato, true)) {
        rt->nota(w.descrizione());
        if (uscita_gioco) {
            rt->nota("audio " + uscita_gioco->driver() + ", dispositivo " + std::to_string(uscita_gioco->campioni_del_dispositivo()) + " campioni a " + std::to_string(uscita_gioco->campionamento_hz()) + " Hz");
        }
    }
    rt->riga.present = w.ultimo_present_s();
    if (uscita_gioco && regia) {
        const std::uint64_t m = uscita_gioco->in_ritardo(), c = uscita_gioco->tirati();
        rt->riga.mancanti = static_cast<long>(m - ritardo_prima);
        rt->riga.accodati = static_cast<long>(c - tirati_prima);
        ritardo_prima = m, tirati_prima = c;
        rt->riga.filo = uscita_gioco->filo_massimo_s();
        rt->riga.buco_cuffia = buco_cuffia_filo.exchange(0, std::memory_order_relaxed);
        rt->riga.buco_bordo = buco_bordo_filo.exchange(0, std::memory_order_relaxed);
        rt->riga.cuffia_attesa = static_cast<long>(regia->mixer().in_attesa(Canale::cuffia));
        rt->riga.bordo_attesa = static_cast<long>(regia->mixer().in_attesa(Canale::bordo));
    }
    rt->chiudi(luogo, scena, missione_s);
}

// --menu-prova FILE: un passo per fotogramma, una riga per passo, per provare il giro dei menu senza mani (SDL_VIDEODRIVER=dummy).
// Parole: su giu sinistra destra, lettera:X, testo:abc, clic:X,Y, foto:FILE (lo schermo di questo passo), chiudi; ogni altra e' un
// comando dei tasti (conferma, esci, zona_avanti, pulsante_2, cerca). Una riga vuota: un fotogramma senza niente. Finite le righe, la
// finestra si chiude
struct PassoProva {
    Comandi c;
    std::string foto;
};
std::deque<PassoProva> passi_prova;
bool prova_attiva = false;
std::string foto_prova;

void leggi_passi_prova(const std::string& percorso) {
    std::ifstream in(percorso);
    if (!in) throw std::runtime_error("--menu-prova: non trovo " + percorso);
    prova_attiva = true;
    for (std::string riga; std::getline(in, riga);) {
        if (!riga.empty() && riga[0] == '#') continue;
        PassoProva p;
        std::istringstream parole(riga);
        for (std::string w; parole >> w;) {
            if (w == "su") p.c.su_giu = 1;
            else if (w == "giu") p.c.su_giu = -1;
            else if (w == "sinistra") p.c.ruota = -1;
            else if (w == "destra") p.c.ruota = 1;
            else if (w == "chiudi") p.c.esci = true;
            else if (w.rfind("lettera:", 0) == 0) p.c.lettere.push_back(w[8]);
            else if (w.rfind("testo:", 0) == 0) p.c.testo += w.substr(6);
            else if (w.rfind("foto:", 0) == 0) p.foto = w.substr(5);
            else if (w.rfind("clic:", 0) == 0) {
                int x = 0, y = 0;
                if (std::sscanf(w.c_str() + 5, "%d,%d", &x, &y) == 2) p.c.clic.push_back({x, y});
            } else applica(w, p.c);
        }
        passi_prova.push_back(std::move(p));
    }
}

void presenta(Finestra& w, const Tela& t, const Palette& p, const std::string& brano, bool in_loop = true) {
    if (!foto_prova.empty()) {
        t.salva_bmp(foto_prova, p);
        foto_prova.clear();
    }
    if (regia) regia->musica(brano, in_loop);
    alimenta_audio();
    Cronometro cr(&RegistroTempi::Riga::presenta);
    w.presenta(t, p);
}

Comandi leggi_eventi(Finestra& w) {
    Comandi c = w.eventi();
    if (prova_attiva) {
        c = {};
        if (passi_prova.empty()) c.esci = true;
        else {
            c = std::move(passi_prova.front().c);
            foto_prova = std::move(passi_prova.front().foto);
            passi_prova.pop_front();
        }
    }
    if (regia && !classico_in_mare) {
        if (!c.clic.empty()) regia->effetto("clic_interfaccia", 0.8f);
        else if (c.invio || c.esc) regia->effetto("clic_interfaccia", 0.5f);
        if (std::find(c.nominati.begin(), c.nominati.end(), "muto") != c.nominati.end()) regia->muto(!regia->muto());
    }
    return c;
}

// Il video d'apertura (docs/29; app/video_apertura): recitato dal motore, si salta con un tasto qualunque o un clic. Falso se la
// finestra si chiude. Senza la regia o le scene si va al menu, col motivo su stderr.
// Non si calcola sul fotogramma che si mostra: un filo lo prepara in anticipo come il ramo senza finestra, a passi fissi, immagine
// e suono insieme, e qui si mostra quello che l'audio ha raggiunto. Il periscopio costa piu' di un fotogramma e una riga del
// sonar anche: la coda li assorbe, e lo schermo non li sente
bool gioca_apertura(Finestra& w, Tela& tela, const Risorse& r, const Opzioni& o) {
    constexpr int FPS = 30, HZ = 48000, A_FOTOGRAMMA = HZ / FPS;
    constexpr std::size_t MASSIMO = 20 * FPS, AVVIO = 3 * FPS;   // 20 s in coda, 190 MB; si parte con 3 s pronti
    bool chiusa = false;
    try {
        tela.riempi(0);
        presenta(w, tela, r.giorno, "");
        RegiaApertura rg = carica_regia_apertura(o.dati + "/apertura.toml");
        Opzioni ov = o;
        ov.inizio = rg.inizio, ov.latitudine = rg.latitudine, ov.longitudine = rg.longitudine;
        CodaFilm coda(A_FOTOGRAMMA, MASSIMO, AVVIO);
        std::string errore;
        double ritardo = 0.0;
        RegistroTempi* rt = RegistroTempi::attivo();
        std::thread fabbrica([&, rg = std::move(rg)]() mutable {
            try {
                const std::string brano = rg.brano;
                RegiaAudio suono(o.dati, HZ);
                const PreferenzeAudio a = leggi_preferenze_audio();
                suono.volume_musica(a.musica);
                suono.uscita_musica(a.uscita == "mt32" ? UscitaMusica::mt32 : UscitaMusica::fm);
                VideoApertura v(r, o.dati, std::move(rg),
                                [&r, ov](Tela& t, const Periscopio& p, const Mondo& m, const VistaPeriscopio& vp, const StatoPannello& s, int id,
                                         double tempo, const Effetti* e, bool con_vista) { return componi(t, r, p, m, vp, ov, s, id, tempo, e, nullptr, con_vista); });
                v.imposta_audio(&suono);
                suono.musica(brano, false);
                Tela t;
                while (v.avanza(1.0 / FPS, 0.0)) {
                    Cronometro cr_disegno(&RegistroTempi::Riga::disegno);
                    const Palette pal = v.disegna(t);
                    cr_disegno.ferma();
                    if (!coda.metti(t, pal, v.inquadratura() + 1, suono.produci(A_FOTOGRAMMA))) break;
                    if (rt) rt->chiudi("film_prep", v.inquadratura() + 1, v.tempo_missione(false));
                }
                ritardo = v.ritardo_massimo();
            } catch (const std::exception& e) {
                errore = e.what();
            }
            coda.finita();
        });
        struct Fine {   // anche se qui si esce con un'eccezione: il filo si ferma prima che coda e fotogrammi spariscano
            CodaFilm& coda;
            std::thread& fabbrica;
            ~Fine() {
                coda.ferma();
                if (fabbrica.joinable()) fabbrica.join();
                std::lock_guard blocco(film_mutex);
                film_in_onda = nullptr;
            }
        } fine{coda, fabbrica};
        {
            std::lock_guard blocco(film_mutex);
            film_in_onda = &coda;
        }
        if (rt) rt->nota("film: la preparazione comincia");
        Palette pal = r.giorno;
        std::uint64_t mostrati = 0, tirati_prima = uscita_gioco ? uscita_gioco->tirati() : 0;
        const double t_inizio = Finestra::secondi();
        double t_prec = t_inizio, t_tirati = t_inizio, t_zero = t_inizio, t_partita = -1.0, ultimo_s = 0.0;
        for (;;) {
            Cronometro cr_eventi(&RegistroTempi::Riga::eventi);
            const Comandi c = w.eventi();
            cr_eventi.ferma();
            if (c.esci) {
                chiusa = true;
                break;
            }
            if (c.esc || c.invio || c.tab || c.pausa || !c.clic.empty() || !c.clic_destro.empty() || !c.lettere.empty() || !c.cifre.empty() ||
                !c.nominati.empty() || c.stazione != 0) {
                if (rt) rt->nota("film: saltato a " + std::to_string(ultimo_s) + " s");
                break;
            }
            const double t_ora = Finestra::secondi();
            // senza dispositivo, o se il dispositivo non chiede piu' niente, l'orologio e' il tempo vero
            const std::uint64_t tirati = uscita_gioco ? uscita_gioco->tirati() : 0;
            if (tirati != tirati_prima) tirati_prima = tirati, t_tirati = t_ora;
            if (!uscita_gioco || t_ora - t_tirati > 0.5) (void)coda.campioni(static_cast<std::size_t>((t_ora - t_prec) * HZ));
            t_prec = t_ora;
            if (t_partita < 0.0 && coda.partita()) {
                t_partita = t_ora;
                if (rt) rt->nota("film: comincia dopo " + std::to_string(t_ora - t_inizio) + " s di preparazione");
            }
            // l'orologio dei fotogrammi: il tempo vero, tenuto piano sull'audio, che avanza a blocchi: seguirlo a scatti farebbe cadere
            // i fotogrammi a intervalli diversi. Se l'audio si ferma, o salta, ci si riaggancia subito
            const double suonato = static_cast<double>(coda.dati()) / HZ, scarto = suonato - (t_ora - t_zero);
            t_zero = std::fabs(scarto) > 0.25 ? t_ora - suonato : t_zero - 0.01 * scarto;
            auto f = coda.prendi(static_cast<std::uint64_t>(std::max(t_ora - t_zero, 0.0) * HZ));
            if (f) {
                tela = std::move(f->tela);
                pal = std::move(f->palette);
                ultimo_s = static_cast<double>(f->numero) / FPS;
                ++mostrati;
            }
            {
                Cronometro cr(&RegistroTempi::Riga::presenta);
                w.presenta(tela, pal);
            }
            if (f) {
                if (rt) {
                    RegistroTempi::riga.film_anticipo = static_cast<long>(coda.anticipo());
                    RegistroTempi::riga.film_silenzio = static_cast<long>(coda.silenzio());
                }
                chiudi_fotogramma(w, "film", f->inquadratura, ultimo_s);
            }
            if (coda.conclusa()) {
                if (rt) rt->nota("film: finito");
                break;
            }
        }
        coda.ferma();
        fabbrica.join();
        if (!errore.empty()) std::fprintf(stderr, "video d'apertura non disponibile: %s\n", errore.c_str());
        else if (t_partita >= 0.0) {
            const double durata = std::max(0.1, Finestra::secondi() - t_partita);
            std::fprintf(stderr, "video d'apertura: %.1f s di film in %.1f s, dopo %.1f s di preparazione, %.1f fotogrammi al secondo, %.2f s di silenzio per la coda vuota, missioni in ritardo di %.1f s al massimo\n",
                         ultimo_s, durata, t_partita - t_inizio, static_cast<double>(mostrati) / durata, static_cast<double>(coda.silenzio()) / HZ, ritardo);
        }
    } catch (const std::exception& e) {
        std::fprintf(stderr, "video d'apertura non disponibile: %s\n", e.what());
    }
    if (chiusa) return false;
    if (regia) regia->musica("");
    if (uscita_gioco) uscita_gioco->svuota();
    return true;
}

// L'albo dei comandanti con la finestra (Fase 4a): la scelta di chi va in mare, le carriere nuove, i fascicoli.
// Falso se la finestra si chiude
bool mostra_albo(Finestra& w, Tela& tela, const Risorse& r, const RegoleCarriera& regole, const TavolaDecorazioni& tav,
                 const std::string& dati, std::optional<Marina>* marina = nullptr) {
    SchermataAlbo albo(regole, tav);
    for (;;) {
        const Comandi c = leggi_eventi(w);
        if (c.esci) return false;
        const auto u = albo.comandi(c);
        w.modo_testo(albo.vuole_testo());
        if (marina && albo.marina_scelta()) *marina = albo.marina_scelta();
        if (u == SchermataAlbo::Uscita::menu) break;
        if (u == SchermataAlbo::Uscita::statistiche) {   // le statistiche cumulate del comandante scelto, o le globali
            try {
                const FileStatistiche fs = leggi_statistiche();
                const Fascicolo* f = albo.scelto();
                const auto it = f ? fs.comandanti.find(f->id) : fs.comandanti.end();
                SchermataStatisticheCumulate st(dati, it != fs.comandanti.end() ? it->second : fs.globali,
                                                it != fs.comandanti.end() ? f->nome : std::string("globali"));
                for (double t_st = Finestra::secondi();;) {
                    const Comandi cs = leggi_eventi(w);
                    if (cs.esci) return false;
                    const double ora = Finestra::secondi();
                    if (st.comandi(cs, ora - t_st)) break;
                    t_st = ora;
                    st.disegna(tela, r.font);
                    Palette pal = r.giorno;
                    st.palette(pal);
                    presenta(w, tela, pal, "albo");
                }
            } catch (const std::exception& e) {
                std::fprintf(stderr, "statistiche non disponibili: %s\n", e.what());
            }
            continue;
        }
        if (u == SchermataAlbo::Uscita::onore) {   // l'albo d'onore del kit: migliori missioni, carriere e storici
            SchermataAlboOnore onore(regole, dati);
            for (;;) {
                const Comandi co = leggi_eventi(w);
                if (co.esci) return false;
                if (onore.comandi(co) == SchermataAlboOnore::Uscita::menu) break;
                onore.disegna(tela, r.font);
                Palette pal = r.giorno;
                onore.palette(pal);
                presenta(w, tela, pal, "albo");
            }
            continue;
        }
        if (u == SchermataAlbo::Uscita::bacheca) {   // i riconoscimenti del comandante scelto (o i globali)
            try {
                static const CatalogoAchievement cat = carica_achievement(dati + "/achievement.toml");
                SchermataBacheca b(cat, tav, albo.scelto() ? std::optional<Fascicolo>(*albo.scelto()) : std::nullopt);
                for (;;) {
                    const Comandi cb = leggi_eventi(w);
                    if (cb.esci) return false;
                    if (b.comandi(cb)) break;
                    b.disegna(tela, r.font);
                    Palette pal = r.giorno;
                    b.palette(pal);
                    presenta(w, tela, pal, "albo");
                }
            } catch (const std::exception& e) {
                std::fprintf(stderr, "bacheca non disponibile: %s\n", e.what());
            }
            continue;
        }
        if (u == SchermataAlbo::Uscita::fascicolo && albo.scelto()) {
            SchermataFascicolo fascicolo(regole, tav, *albo.scelto());
            for (;;) {
                const Comandi cf = leggi_eventi(w);
                if (cf.esci) return false;
                if (fascicolo.comandi(cf)) break;
                if (const auto p = fascicolo.replay_chiesto()) {   // il replay di una missione del fascicolo
                    try {
                        SchermataReplay rp(dati, carica_traccia(*p));
                        for (double t_rp = Finestra::secondi();;) {
                            const Comandi cr = leggi_eventi(w);
                            if (cr.esci) return false;
                            const double ora = Finestra::secondi();
                            if (rp.comandi(cr, ora - t_rp)) break;   // Invio: qui non si riprende, la missione non c'e'
                            t_rp = ora;
                            rp.disegna(tela, r.font);
                            Palette pal = r.giorno;
                            rp.palette(pal);
                            presenta(w, tela, pal, "");
                        }
                    } catch (const std::exception& e) {
                        std::fprintf(stderr, "replay non disponibile: %s\n", e.what());
                    }
                    continue;
                }
                fascicolo.disegna(tela, r.font);
                Palette pal = r.giorno;
                fascicolo.palette(pal);
                presenta(w, tela, pal, "albo");
            }
            continue;
        }
        albo.disegna(tela, r.font);
        Palette pal = r.giorno;
        albo.palette(pal);
        presenta(w, tela, pal, "albo");
    }
    w.modo_testo(false);
    return true;
}

// Chi va in mare per le due parti, per la scelta della missione: sigla del grado e nome
// Il codice partita della missione nelle opzioni (app/codice_partita.hpp), o quello sandbox col vetrino; vuoto per il
// classico, i semi oltre 32 bit e le missioni senza file
std::string codice_della_partita(const Opzioni& o, bool dalla_libreria, bool dall_editor) {
    if (o.missione.empty() || o.regola != RegolaDanni::simulazione || o.seme > 0x7FFFFFFFull) return {};   // 31 bit di seme
    const std::filesystem::path cartella(o.cartella_missioni);
    CodicePartita k;
    k.origine = dalla_libreria ? OrigineCodice::libreria
              : !dall_editor ? OrigineCodice::originale
              : cartella.lexically_normal() == cartella_missioni_importate().lexically_normal() ? OrigineCodice::importata
                                                                                                  : OrigineCodice::editor;
    // l'id col solo nome del file, come lo trova trova_missione (--missione prove/X da riga di comando)
    k.impronta = impronta_missione(std::filesystem::path(o.missione).filename().string(), cartella / (o.missione + ".lua"));
    k.seme = static_cast<std::uint32_t>(o.seme);
    k.anni_89 = o.sonar == "1989";
    k.carta_epoca = o.carta == "epoca";
    k.meteo_realistico = o.meteo == "realistico";
    k.asw = o.asw == "facile" ? 0 : o.asw == "realistico" ? 2 : o.asw == "minaccia" ? 3 : 1;
    k.sonarista = o.sonarista == "nessuno" ? 0 : o.sonarista == "recluta" ? 1 : o.sonarista == "esperto" ? 3 : 2;
    k.navigazione = o.navigazione == "realistica";
    k.gps = o.gps && k.navigazione;
    k.senza_traffico = !(o.sandbox ? o.regolazioni.traffico : o.traffico);
    if (!k.impronta) return {};
    return o.sandbox ? scrivi_codice_sandbox({k, o.regolazioni}) : scrivi_codice_partita(k);   // il vetrino: le regolazioni
}

// La schermata di una serie della sandbox (app/schermata_serie) e, da un clic su una partita, il suo replay; falso se
// la finestra si chiude
bool mostra_serie(Finestra& w, Tela& tela, const Risorse& r, const std::string& dati, const std::filesystem::path& riassunto) {
    std::optional<SchermataSerie> sr;
    try {
        sr.emplace(dati, riassunto);
    } catch (const std::exception& e) {
        std::fprintf(stderr, "serie non disponibile: %s\n", e.what());
        return true;
    }
    for (double t_s = Finestra::secondi();;) {
        const Comandi c = leggi_eventi(w);
        if (c.esci) return false;
        const double ora = Finestra::secondi();
        const bool fine = sr->comandi(c, ora - t_s);
        t_s = ora;
        if (const auto p = sr->traccia_scelta()) {   // il replay della partita cliccata, poi di nuovo la serie
            try {
                SchermataReplay rp(dati, carica_traccia(*p));
                for (double t_rp = Finestra::secondi();;) {
                    const Comandi cr = leggi_eventi(w);
                    if (cr.esci) return false;
                    const double adesso = Finestra::secondi();
                    if (rp.comandi(cr, adesso - t_rp)) break;
                    t_rp = adesso;
                    rp.disegna(tela, r.font);
                    Palette pal = r.giorno;
                    rp.palette(pal);
                    presenta(w, tela, pal, "");
                }
            } catch (const std::exception& e) {
                std::fprintf(stderr, "replay non disponibile: %s\n", e.what());
            }
            sr.emplace(dati, riassunto);   // da capo, senza la scelta
            continue;
        }
        if (fine) return true;
        sr->disegna(tela, r.font);
        Palette pal = r.giorno;
        sr->palette(pal);
        presenta(w, tela, pal, "albo");
    }
}

// S dalle regolazioni: una serie del vetrino su `semi` semi con strumenti/serie_sandbox (accanto all'eseguibile), in un
// filo a parte mentre la finestra mostra quante partite sono fatte; il riassunto, o vuoto se non e' riuscita
std::optional<std::filesystem::path> gioca_serie(Finestra& w, Tela& tela, const Risorse& r, const std::string& codice, int semi,
                                                 const std::string& missione, bool& chiusa) {
    std::error_code ec;
    const auto se = std::filesystem::read_symlink("/proc/self/exe", ec);
    const std::filesystem::path motore = ec ? std::filesystem::path("melchior688") : se, strumento = motore.parent_path() / "serie_sandbox";
    char quando[32];
    const std::time_t ora = std::time(nullptr);
    std::strftime(quando, sizeof quando, "%Y%m%d_%H%M%S", std::localtime(&ora));
    const auto uscita = cartella_replay(std::string()).parent_path().parent_path() / "serie" / (missione + "_" + quando);
    std::filesystem::create_directories(uscita, ec);
    const auto tra_apici = [](const std::string& s) {
        std::string x = "'";
        for (const char ch : s) x += ch == '\'' ? std::string("'\\''") : std::string(1, ch);
        return x + "'";
    };
    const int lavori = std::clamp(static_cast<int>(std::thread::hardware_concurrency()) - 1, 1, 4);
    const std::string cmd = tra_apici(strumento.string()) + " --codice " + tra_apici(codice) + " --semi " + std::to_string(semi) +
                            " --lavori " + std::to_string(lavori) + " --secondi 90000 --motore " + tra_apici(motore.string()) +
                            " --uscita " + tra_apici(uscita.string()) + " > " + tra_apici((uscita / "serie.txt").string()) + " 2>&1";
    std::atomic<bool> fatto{false};
    std::thread filo([&] {
        if (std::system(cmd.c_str()) != 0) std::fprintf(stderr, "serie: lo strumento non e' riuscito (%s)\n", (uscita / "serie.txt").c_str());
        fatto = true;
    });
    while (!fatto) {   // la finestra resta viva; la serie non si interrompe
        if (leggi_eventi(w).esci) chiusa = true;
        int fatte = 0;
        for (const auto& e : std::filesystem::directory_iterator(uscita, ec))
            if (e.path().extension() == ".m688tr") ++fatte;
        tela.riempi(colori::NERO);
        const std::string a = "SERIE DEL VETRINO: " + std::to_string(fatte) + " PARTITE FATTE SU " + std::to_string(semi),
                          b = "OGNI PARTITA SI GIOCA SENZA FINESTRA, " + std::to_string(lavori) + " ALLA VOLTA. ATTENDERE.";
        r.font.scrivi(tela, LARGHEZZA_TELA / 2 - r.font.larghezza(a, 1) / 2, 200, a, colori::GIALLO, 1);
        r.font.scrivi(tela, LARGHEZZA_TELA / 2 - r.font.larghezza(b, 1) / 2, 220, b, colori::GRIGIO, 1);
        presenta(w, tela, r.giorno, "albo");
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    filo.join();
    if (!std::filesystem::exists(uscita / "serie.toml")) return std::nullopt;
    return uscita / "serie.toml";
}

void comandanti_in_carica(SchermataScelta& s, const RegoleCarriera& regole) {
    const auto nome = [&](Marina m) -> std::string {
        const auto id = comandante_in_carica(m);
        if (!id) return {};
        try {
            const Fascicolo f = leggi_fascicolo(percorso_fascicolo(*id));
            const Grado* g = regole.grado(f.marina, f.grado);
            return testo_per_font((g ? g->sigla + " " : std::string()) + f.nome);   // i gradi sovietici in cirillico, traslitterati
        } catch (const std::exception&) {
            return {};
        }
    };
    const std::string usn = nome(Marina::usn), vmf = nome(Marina::vmf);
    s.imposta_comandanti(usn, vmf);
    if (usn.empty() && !vmf.empty()) s.vai_al_battello(true);   // c'e' solo un comandante della VMF: il menu parte dall'Alfa
}

// La prima volta, senza preferenze: classico o Enhanced. Vuota se la finestra si chiude o con Esc
std::optional<bool> chiedi_modo(Finestra& w, Tela& tela, const Risorse& r) {
    SchermataModo modo;
    for (;;) {
        bool esci = false;
        const Comandi c = leggi_eventi(w);
        const auto scelto = modo.comandi(c, esci);
        if (esci) return std::nullopt;
        if (scelto) return scelto;
        modo.disegna(tela, r.font);
        Palette pal = r.giorno;
        modo.palette(pal);
        presenta(w, tela, pal, "menu");
    }
}

}  // namespace

int main(int argc, char** argv) {
    try {
        Opzioni o = leggi_opzioni(argc, argv);
        if (!o.menu_prova.empty()) leggi_passi_prova(o.menu_prova);
        if (!o.scrivi_tasti.empty()) {   // il modello del file dei tasti, con quelli in uso (predefiniti o del file)
            std::ofstream f(o.scrivi_tasti);
            if (!f) throw std::runtime_error("impossibile scrivere " + o.scrivi_tasti);
            f << carica_tasti(o.tasti).come_toml();
            std::printf("tasti scritti in %s\n", o.scrivi_tasti.c_str());
            return 0;
        }
        Tela tela;

        if (!o.immagine.empty()) {
            const Palette pal = mostra_immagine(tela, o);
            if (!o.istantanea.empty()) { tela.salva_bmp(o.istantanea, pal); return 0; }
            Finestra finestra("688 Attack Sub - Enhanced");
            for (;;) {
                const Comandi c = leggi_eventi(finestra);
                if (c.esci || c.esc) break;
                presenta(finestra, tela, pal, "");
            }
            return 0;
        }

        const Risorse r = carica_risorse(o.dati);
        // i numeri delle isobate della carta della Fase 3: incisione sulle carte NATO, stampino sulle sovietiche
        std::optional<Font> font_isobate_nato, font_isobate_urss;
        try {
            font_isobate_nato = Font::carica(o.dati + "/font/incisione688_5.fnt");
            font_isobate_urss = Font::carica(o.dati + "/font/stampino_urss_7.fnt");
            imposta_font_isobate(&*font_isobate_nato, &*font_isobate_urss);
            imposta_cartella_dati_carta(o.dati);
        } catch (const std::exception& e) {
            std::fprintf(stderr, "font della carta non disponibili: %s\n", e.what());
        }
        if (o.schermata == "apertura" && !o.istantanea.empty()) {   // il video d'apertura senza finestra: al passo di 1/30 s
            RegiaApertura rg = carica_regia_apertura(o.dati + "/apertura.toml");
            Opzioni ov = o;
            ov.inizio = rg.inizio, ov.latitudine = rg.latitudine, ov.longitudine = rg.longitudine;
            VideoApertura v(r, o.dati, std::move(rg),
                            [&r, ov](Tela& t, const Periscopio& p, const Mondo& m, const VistaPeriscopio& vp, const StatoPannello& s, int id,
                                     double tempo, const Effetti* e, bool con_vista) { return componi(t, r, p, m, vp, ov, s, id, tempo, e, nullptr, con_vista); });
            std::vector<float> campioni;
            std::map<int, std::array<double, 3>> costo;   // per inquadratura: s di missioni, s di disegno e audio, fotogrammi
            if (!o.audio_wav.empty()) {
                regia = std::make_unique<RegiaAudio>(o.dati, 48000);
                v.imposta_audio(regia.get());
                regia->musica(v.regia().brano, false);
            }
            std::vector<double> istanti = o.istantanee;
            std::sort(istanti.begin(), istanti.end());
            istanti.push_back(o.secondi);
            const std::filesystem::path base(o.istantanea);
            for (std::size_t k = 0; k < istanti.size(); ++k) {
                bool ancora = true;
                while (v.tempo() + 1.0 / 30.0 <= istanti[k] && ancora) {
                    const double t0 = Finestra::secondi();
                    ancora = v.avanza(1.0 / 30.0, 0.0);
                    const double t1 = Finestra::secondi();
                    if (regia) {
                        (void)v.disegna(tela);   // la stazione in vista fa il suo audio
                        const auto x = regia->produci(1600);
                        campioni.insert(campioni.end(), x.begin(), x.end());
                    }
                    auto& c = costo[v.inquadratura()];
                    c[0] += t1 - t0, c[1] += Finestra::secondi() - t1, c[2] += 1.0;
                }
                const Palette pal = v.disegna(tela);
                char nome[32];
                std::snprintf(nome, sizeof nome, "_%05d", static_cast<int>(istanti[k]));
                const std::string f = k + 1 == istanti.size() ? o.istantanea : (base.parent_path() / (base.stem().string() + nome + base.extension().string())).string();
                tela.salva_bmp(f, pal);
                std::printf("apertura: %.1f s di film, inquadratura %d, missione %.1f e %.1f s, %s\n", v.tempo(), v.inquadratura() + 1,
                            v.tempo_missione(false), v.tempo_missione(true), f.c_str());
            }
            if (!o.audio_wav.empty()) {
                Suono s;
                s.campionamento_hz = 48000;
                s.campioni.reserve(campioni.size());
                for (const float x : campioni) s.campioni.push_back(static_cast<std::int16_t>(std::clamp(x, -1.0f, 1.0f) * 32767.0f));
                scrivi_wav(o.audio_wav, s);
                for (const auto& [q, c] : costo)   // il conto per il tempo reale: ogni fotogramma ha 33 ms
                    std::printf("inquadratura %2d: %5.1f ms di missioni e %5.1f di disegno a fotogramma\n", q + 1, 1e3 * c[0] / c[2], 1e3 * c[1] / c[2]);
            }
            return 0;
        }
        if (o.schermata == "museo" && !o.istantanea.empty()) {   // il Museo senza finestra: una voce, un'immagine
            const Enciclopedia enc = carica_enciclopedia(o.dati + "/enciclopedia.toml");
            SchermataMuseo museo(enc, r, o.dati, o.voce);
            if (o.evento > 0) museo.immagine(o.evento - 1);
            museo.disegna(tela, r.font);
            Palette pal = r.giorno;
            museo.palette(pal);
            tela.salva_bmp(o.istantanea, pal);
            std::printf("schermata museo in %s (%s)\n", o.istantanea.c_str(), museo.voce() ? museo.voce()->id.c_str() : "-");
            return 0;
        }
        if (!o.serie.empty()) {   // il riassunto di una serie della sandbox: senza finestra una pagina, in finestra tutto
            if (!o.istantanea.empty()) {
                SchermataSerie sr(o.dati, o.serie);
                sr.pagina(std::clamp(o.pagina - 1, 0, SchermataSerie::PAGINE - 1));
                Palette pal = r.giorno;
                sr.disegna(tela, r.font);
                sr.palette(pal);
                tela.salva_bmp(o.istantanea, pal);
                std::printf("schermata serie in %s (pagina %d, %d partite)\n", o.istantanea.c_str(), sr.pagina() + 1, sr.serie().semi);
                return 0;
            }
            Finestra w("688 Attack Sub - Serie della sandbox");
            mostra_serie(w, tela, r, o.dati, o.serie);
            return 0;
        }
        if (o.schermata == "replay" && !o.istantanea.empty()) {   // senza finestra: lo schermo del replay all'istante T
            SchermataReplay replay(o.dati, carica_traccia(o.replay));
            if (o.pagina == 2) replay.lettura(SchermataReplay::Lettura::verita);
            replay.vai(o.secondi);
            Palette pal = r.giorno;
            replay.disegna(tela, r.font);
            replay.palette(pal);
            tela.salva_bmp(o.istantanea, pal);
            std::printf("schermata replay in %s (%s, teatro %s, %.0f s)\n", o.istantanea.c_str(), replay.traccia().missione.c_str(),
                        replay.teatro().c_str(), replay.tempo());
            return 0;
        }
        if (o.schermata == "statistiche" && !o.istantanea.empty()) {   // senza finestra: le statistiche di una missione dalla traccia
            const Traccia traccia = carica_traccia(o.replay);
            const CatalogoAcustico acu = carica_acustica(o.dati + "/acustica.toml");
            SchermataStatistiche st(o.dati, report_missione(traccia, &acu), traccia);
            Palette pal = r.giorno;
            st.disegna(tela, r.font);
            st.palette(pal);
            tela.salva_bmp(o.istantanea, pal);
            std::printf("schermata statistiche in %s (%s, teatro %s)\n", o.istantanea.c_str(), traccia.missione.c_str(), st.teatro().c_str());
            return 0;
        }
        if (o.schermata == "statistiche-cumulate" && !o.istantanea.empty()) {   // le globali, o un comandante con --voce ID
            const FileStatistiche file = leggi_statistiche();
            StatisticheCumulate c = file.globali;
            std::string chi = "globali";
            if (!o.voce.empty()) {
                const auto it = file.comandanti.find(o.voce);
                c = it != file.comandanti.end() ? it->second : StatisticheCumulate{};
                chi = o.voce;
            }
            if (c.missioni == 0 && !o.replay.empty()) {   // nessun dato: le riempie la traccia data (per le istantanee)
                const CatalogoAcustico acu = carica_acustica(o.dati + "/acustica.toml");
                accumula(c, report_missione(carica_traccia(o.replay), &acu));
            }
            SchermataStatisticheCumulate st(o.dati, c, chi);
            st.scegli_teatro(o.pagina > 0 ? o.pagina - 1 : 0);
            Palette pal = r.giorno;
            st.disegna(tela, r.font);
            st.palette(pal);
            tela.salva_bmp(o.istantanea, pal);
            std::printf("schermata statistiche cumulate in %s (%d missioni, teatro %s)\n", o.istantanea.c_str(), c.missioni, st.teatro().c_str());
            return 0;
        }
        if (o.schermata == "bacheca" && !o.istantanea.empty()) {   // senza finestra: la bacheca dei riconoscimenti globali
            const RegoleCarriera regole = carica_carriera(o.dati + "/carriera.toml");
            const TavolaDecorazioni tav = TavolaDecorazioni::carica(regole, o.dati + "/decorazioni_grafica.toml");
            const CatalogoAchievement cat = carica_achievement(o.dati + "/achievement.toml");
            SchermataBacheca b(cat, tav, std::nullopt);
            b.scegli(o.pagina - 1, std::max(0, o.evento - 1));
            Palette pal = r.giorno;
            b.disegna(tela, r.font);
            b.palette(pal);
            tela.salva_bmp(o.istantanea, pal);
            std::printf("schermata bacheca in %s\n", o.istantanea.c_str());
            return 0;
        }
        if ((o.schermata == "albo" || o.schermata == "fascicolo") && !o.istantanea.empty()) {   // senza finestra
            const RegoleCarriera regole = carica_carriera(o.dati + "/carriera.toml");
            const TavolaDecorazioni tav = TavolaDecorazioni::carica(regole, o.dati + "/decorazioni_grafica.toml");
            SchermataAlbo albo(regole, tav);
            if (o.evento > 0) albo.scegli(o.evento - 1);
            Palette pal = r.giorno;
            if (o.schermata == "fascicolo" && albo.scelto()) {
                SchermataFascicolo fascicolo(regole, tav, *albo.scelto());
                fascicolo.vai(o.pagina - 1);
                fascicolo.disegna(tela, r.font);
                fascicolo.palette(pal);
            } else {
                albo.disegna(tela, r.font);
                albo.palette(pal);
            }
            tela.salva_bmp(o.istantanea, pal);
            std::printf("schermata %s in %s\n", o.schermata.c_str(), o.istantanea.c_str());
            return 0;
        }
        // avvio dalle schermate (Fase 4a): la scelta della missione decide missione e opzioni, poi il briefing
        std::unique_ptr<Finestra> finestra_menu;
        std::optional<MissioneRapida> rapida;   // la missione rapida scelta nel menu: codice e seme per la carriera
        bool dall_editor = false;               // la missione e' dell'editor: conta solo nella carriera aperta
        bool dalla_libreria = false;            // ... della libreria (data/missioni/libreria): origine «libreria»
        if (o.schermata == "editor" && !o.istantanea.empty()) {   // l'editor senza finestra: nuova, o la rapida di --codice
            std::optional<ScenarioMissione> iniziale;
            if (const auto p = leggi_codice(o.codice)) iniziale = genera_missione_rapida(*p, o.dati).scenario;
            SchermataEditor editor(r, o.dati, iniziale);
            if (o.pagina > 1) editor.scheda(static_cast<SchermataEditor::Scheda>(std::clamp(o.pagina - 1, 0, 7)));
            editor.disegna(tela, r.font);
            Palette pal = r.giorno;
            editor.palette(pal);
            tela.salva_bmp(o.istantanea, pal);
            std::printf("schermata editor in %s\n", o.istantanea.c_str());
            return 0;
        }
        if (o.schermata == "regolazioni" && !o.istantanea.empty()) {   // le regolazioni della sandbox, senza finestra
            SchermataRegolazioni reg(o.missione.empty() ? "SCENE1A" : o.missione, o.regolazioni);
            reg.disegna(tela, r.font);
            Palette pal = r.giorno;
            reg.palette(pal);
            tela.salva_bmp(o.istantanea, pal);
            std::printf("schermata regolazioni in %s\n", o.istantanea.c_str());
            return 0;
        }
        if (o.menu || o.schermata == "scelta" || o.schermata == "scelta-rapida" || o.schermata == "scelta-editor") {
            SchermataScelta scelta(elenco_missioni(o.cartella_missioni), o.dati);
            scelta.imposta_cartella_missioni(o.cartella_missioni);
            if (o.regola_data) scelta.imposta_classico(o.regola == RegolaDanni::classica);
            if (!o.istantanea.empty()) {   // la schermata senza finestra
                if (o.schermata == "scelta-editor") scelta.scheda_editor();
                if (o.schermata == "scelta-rapida") {
                    scelta.scheda_rapida(true);
                    if (const auto p = leggi_codice(o.codice)) scelta.imposta_rapida(*p);
                }
                scelta.disegna(tela, r.font);
                Palette pal = r.giorno;
                scelta.palette(pal);
                tela.salva_bmp(o.istantanea, pal);
                std::printf("schermata scelta in %s\n", o.istantanea.c_str());
                return 0;
            }
            finestra_menu = std::make_unique<Finestra>("688 Attack Sub - Enhanced");
            finestra_menu->imposta_tasti(carica_tasti(o.tasti));
            applica_uscita(*finestra_menu, o);
            apri_audio(o);
            PreferenzeAvvio avvio = leggi_preferenze_avvio();
            scelta.imposta_video(avvio.video);
            static bool apertura_fatta = false;   // una volta per avvio: dal debriefing si torna qui senza rivederlo
            if (!std::exchange(apertura_fatta, true) && !o.senza_apertura && (avvio.video == 0 || (avvio.video == 1 && !avvio.visto))) {
                if (!gioca_apertura(*finestra_menu, tela, r, o)) return 0;
                if (!avvio.visto) {
                    avvio.visto = true;
                    scrivi_preferenze(leggi_persistenza_sonar(), leggi_uscita_schermo(), std::nullopt, avvio);
                }
            }
            // classico o Enhanced: dalla riga di comando, dalle preferenze, o si chiede la prima volta
            bool classico = false;
            if (o.regola_data || o.sandbox) {
                classico = o.regola == RegolaDanni::classica;
            } else if (avvio.modo >= 0) {
                classico = avvio.modo == 1;
            } else {
                const auto scelto = chiedi_modo(*finestra_menu, tela, r);
                if (!scelto) return 0;
                classico = *scelto;
                avvio.modo = classico ? 1 : 0;
                scrivi_preferenze(leggi_persistenza_sonar(), leggi_uscita_schermo(), std::nullopt, avvio);
            }
            scelta.imposta_classico(classico);
            // l'albo dei comandanti (C): la prima volta, senza carriere, si apre da solo (come in F-19); non nel classico
            std::optional<RegoleCarriera> regole_albo;
            std::optional<TavolaDecorazioni> tavola_albo;
            try {
                regole_albo = carica_carriera(o.dati + "/carriera.toml");
                tavola_albo = TavolaDecorazioni::carica(*regole_albo, o.dati + "/decorazioni_grafica.toml");
                std::optional<Marina> marina;
                if (!classico && leggi_albo().empty() && !mostra_albo(*finestra_menu, tela, r, *regole_albo, *tavola_albo, o.dati, &marina)) return 0;
                comandanti_in_carica(scelta, *regole_albo);
                if (marina) scelta.vai_al_battello(*marina == Marina::vmf);
            } catch (const std::exception& e) {
                std::fprintf(stderr, "albo non disponibile: %s\n", e.what());
                regole_albo.reset();
            }
            // la musica del menu (roadmap 38): M fa il giro FM, MT-32 (il tema dell'Enhanced), 1989 AdLib, Tandy, Covox (i
            // titoli di Hubbard), spenta; la scelta resta in preferenze.toml ([audio] musica_uscita)
            apri_audio(o);
            {
                const PreferenzeAudio a = leggi_preferenze_audio();
                for (int k = 0; k < 6; ++k)
                    if (a.uscita == USCITE_MUSICA[k]) scelta.imposta_musica(k);
            }
            int musica_scelta = scelta.musica();
            std::string brano_menu = musica_del_menu(o, musica_scelta);
            for (;;) {
                if (scelta.musica() != musica_scelta) {   // M: si cambia, si ricorda
                    musica_scelta = scelta.musica();
                    PreferenzeAudio a = leggi_preferenze_audio();
                    a.uscita = USCITE_MUSICA[std::clamp(musica_scelta, 0, 5)];
                    scrivi_preferenze(leggi_persistenza_sonar(), leggi_uscita_schermo(), a);
                    brano_menu = musica_del_menu(o, musica_scelta);
                }
                if (scelta.classico() != classico) {   // O: si cambia, si ricorda; la prima volta nell'Enhanced si apre l'albo
                    classico = scelta.classico();
                    avvio.modo = classico ? 1 : 0;
                    scrivi_preferenze(leggi_persistenza_sonar(), leggi_uscita_schermo(), std::nullopt, avvio);
                    if (!classico && regole_albo && tavola_albo && leggi_albo().empty()) {
                        std::optional<Marina> marina;
                        if (!mostra_albo(*finestra_menu, tela, r, *regole_albo, *tavola_albo, o.dati, &marina)) return 0;
                        comandanti_in_carica(scelta, *regole_albo);
                        if (marina) scelta.vai_al_battello(*marina == Marina::vmf);
                    }
                }
                if (scelta.video() != avvio.video) {   // A: si cambia, si ricorda
                    avvio.video = scelta.video();
                    scrivi_preferenze(leggi_persistenza_sonar(), leggi_uscita_schermo(), std::nullopt, avvio);
                }
                alimenta_audio();
                bool esci = false;
                auto s = scelta.comandi(leggi_eventi(*finestra_menu), esci);
                finestra_menu->modo_testo(scelta.vuole_testo());   // il codice (P) prende i caratteri
                if (esci) return 0;
                if (scelta.editor_chiesto()) {   // l'editor di missione (app/editor_missione); «Prova in mare» la gioca
                    try {
                        SchermataEditor editor(r, o.dati, scelta.scenario_per_editor());
                        double t0 = Finestra::secondi();
                        bool gioca = false;
                        for (;;) {
                            const Comandi ce = leggi_eventi(*finestra_menu);
                            if (ce.esci) return 0;
                            const double t1 = Finestra::secondi();
                            const auto u = editor.comandi(ce, t1 - t0);
                            t0 = t1;
                            finestra_menu->modo_testo(editor.vuole_testo());
                            if (u == SchermataEditor::Uscita::menu) break;
                            if (u == SchermataEditor::Uscita::gioca) {
                                gioca = true;
                                break;
                            }
                            editor.disegna(tela, r.font);
                            Palette pe = r.giorno;
                            editor.palette(pe);
                            presenta(*finestra_menu, tela, pe, "editor");
                        }
                        finestra_menu->modo_testo(false);
                        if (gioca) {   // la missione salvata dall'editor, caricata come le rapide
                            s = scelta.opzioni_partita();
                            s->missione = editor.id();
                            s->meteo_realistico = editor.meteo_realistico();
                            o.cartella_missioni = editor.cartella().string();
                            o.seme = editor.seme();
                            o.inizio = editor.inizio();
                            if (editor.scenario().centro_lat != 0.0 || editor.scenario().centro_lon != 0.0) {   // il sole e la luce sul teatro vero
                                o.latitudine = editor.scenario().centro_lat;
                                o.longitudine = editor.scenario().centro_lon;
                            }
                            dall_editor = true;
                        }
                    } catch (const std::exception& e) {
                        std::fprintf(stderr, "editor non disponibile: %s\n", e.what());
                    }
                    if (!s) continue;
                }
                if (scelta.albo_chiesto() && regole_albo && tavola_albo) {
                    std::optional<Marina> marina;
                    if (!mostra_albo(*finestra_menu, tela, r, *regole_albo, *tavola_albo, o.dati, &marina)) return 0;
                    comandanti_in_carica(scelta, *regole_albo);
                    if (marina) scelta.vai_al_battello(*marina == Marina::vmf);
                    continue;
                }
                if (scelta.museo_chiesto()) {   // F1: il Museo, poi di nuovo la scelta
                    const Enciclopedia enc = carica_enciclopedia(o.dati + "/enciclopedia.toml");
                    SchermataMuseo museo(enc, r, o.dati);
                    double t0 = Finestra::secondi();
                    for (;;) {
                        const Comandi cm = leggi_eventi(*finestra_menu);
                        if (cm.esci) return 0;
                        const double t1 = Finestra::secondi();
                        if (museo.comandi(cm, t1 - t0)) break;
                        t0 = t1;
                        museo.disegna(tela, r.font);
                        Palette pm = r.giorno;
                        museo.palette(pm);
                        presenta(*finestra_menu, tela, pm, "museo");
                    }
                    continue;
                }
                const Opzioni o_prima = o;   // se dalle regolazioni della sandbox si torna indietro
                if (s && s->rapida) {   // la missione rapida: il file generato accanto ai fascicoli, il suo seme e la sua ora
                    const auto cartella = cartella_missioni_rapide();
                    std::filesystem::create_directories(cartella);
                    std::ofstream(cartella / (s->rapida->id + ".lua"), std::ios::binary) << s->rapida->lua;
                    o.cartella_missioni = cartella.string();
                    o.seme = s->rapida->parametri.seme;
                    o.inizio = {s->rapida->anno, s->rapida->mese, s->rapida->giorno, s->rapida->ora_utc};
                    if (s->rapida->scenario.centro_lat != 0.0 || s->rapida->scenario.centro_lon != 0.0) {   // il sole e la luce sul teatro vero
                        o.latitudine = s->rapida->scenario.centro_lat;
                        o.longitudine = s->rapida->scenario.centro_lon;
                    }
                    rapida = s->rapida;
                }
                if (s && s->scenario) {   // una missione della libreria o del giocatore, giocata subito (G)
                    o.cartella_missioni = s->cartella_scenario;
                    dalla_libreria = std::filesystem::path(s->cartella_scenario).filename() == "libreria";
                    o.seme = s->seme ? *s->seme : seme_nuovo();   // ogni partita la sua, o quella del codice partita
                    o.inizio = {s->scenario->anno, s->scenario->mese, s->scenario->giorno, s->scenario->ora_utc};
                    if (s->scenario->centro_lat != 0.0 || s->scenario->centro_lon != 0.0) {
                        o.latitudine = s->scenario->centro_lat;
                        o.longitudine = s->scenario->centro_lon;
                    }
                    dall_editor = true;
                }
                if (s && s->classico) {   // come --missione X --regola classica, con varianti e dadi nuovi a ogni partita
                    o.missione = s->missione;
                    o.regola = RegolaDanni::classica;
                    o.seme = seme_nuovo();
                    o.briefing = false;
                    break;
                }
                if (s) {
                    o.missione = s->missione;
                    o.regola = RegolaDanni::simulazione;
                    // le originali come nel 1989: varianti e dadi nuovi a ogni partita, o quelli del codice partita
                    if (!s->rapida && !dall_editor) o.seme = s->seme ? *s->seme : seme_nuovo();
                    o.sonar = s->anni_89 ? "1989" : "moderno";
                    o.carta = s->carta_epoca ? "epoca" : "vera";
                    o.meteo = s->meteo_realistico ? "realistico" : "semplificato";
                    static constexpr const char* ASW[] = {"facile", "normale", "realistico", "minaccia"};
                    o.asw = ASW[std::clamp(s->asw, 0, 3)];
                    static constexpr const char* SONARISTA[] = {"nessuno", "recluta", "medio", "esperto"};
                    o.sonarista = SONARISTA[std::clamp(s->sonarista, 0, 3)];
                    o.navigazione = s->navigazione_realistica ? "realistica" : "perfetta";
                    o.gps = s->navigazione_realistica && s->gps;
                    o.briefing = true;
                    if (s->sandbox) {   // V: le regolazioni del vetrino, poi la partita senza giocatore (docs/19_sandbox.md)
                        SchermataRegolazioni reg(s->missione, s->regolazioni.value_or(o.regolazioni));   // dal codice: compilate
                        // il vetrino di Dio (docs/28, D2): la pagina degli interventi programmati vuole le unita' della
                        // missione e il codice sandbox delle regolazioni; la missione si carica una volta, solo per l'elenco
                        const auto codice_vetrino = [&](const RegolazioniSandbox& rg) {
                            Opzioni os = o;
                            os.sandbox = true;
                            os.regolazioni = rg;
                            os.meteo = rg.meteo_realistico ? "realistico" : "semplificato";
                            os.navigazione = rg.navigazione_realistica ? "realistica" : "perfetta";
                            os.sonar = rg.anni_89 ? "1989" : "moderno";
                            if (os.seme > 0xFFFFFFFFull) os.seme = 1;
                            return codice_della_partita(os, dalla_libreria, dall_editor);
                        };
                        try {
                            Effetti eff(o.seme);
                            Partita prova(r, ImpostazioniPartita{o.dati, o.cartella_missioni, o.missione, o.seme, RegolaDanni::simulazione,
                                                                 o.carta, o.inizio, {}, {}, -1.0},
                                          eff);
                            std::map<std::string, std::string> nomi;
                            std::vector<std::pair<std::string, std::string>> classi;
                            for (const auto& [k, c] : carica_classi(o.dati + "/classi.toml")) {
                                nomi[k] = c.nome.empty() ? k : c.nome;
                                if (c.pescaggio_m > 0.0) classi.emplace_back(k, nomi[k]);
                            }
                            std::sort(classi.begin(), classi.end(), [](const auto& a, const auto& b) { return a.second < b.second; });
                            std::vector<UnitaEsperimento> unita;
                            for (const auto& [id, u] : prova.missione().unita()) {
                                if (u.flag & 0x20) continue;   // le armi
                                const int parte = u.flag & 0x18;
                                const auto n = nomi.find(u.classe);
                                unita.push_back({UnitaPannello{id, (n != nomi.end() ? n->second : u.classe) + " (" + std::to_string(id) + ")",
                                                               parte == 0 ? "NEUTRALI" : parte == 0x08 ? "NATO" : "PATTO", u.categoria},
                                                 u.pos});
                            }
                            reg.imposta_missione(std::move(unita), std::move(classi));
                        } catch (const std::exception& e) {
                            std::fprintf(stderr, "interventi programmati: la missione non si carica (%s)\n", e.what());
                        }
                        reg.imposta_codice(codice_vetrino);
                        reg.imposta_cartelle({}, std::filesystem::path(o.dati) / "esperimenti");
                        SchermataRegolazioni::Uscita u = SchermataRegolazioni::Uscita::nessuna;
                        double t_reg = Finestra::secondi();
                        while (u == SchermataRegolazioni::Uscita::nessuna) {
                            finestra_menu->modo_testo(reg.vuole_testo());
                            const Comandi cr = leggi_eventi(*finestra_menu);
                            if (cr.esci) return 0;
                            alimenta_audio();
                            const double t_ora_reg = Finestra::secondi();
                            u = reg.comandi(cr, std::clamp(t_ora_reg - t_reg, 0.0, 0.25));
                            t_reg = t_ora_reg;
                            if (u == SchermataRegolazioni::Uscita::esperimento) {   // A: il confronto A/B (serie_ab di trailer-1)
                                finestra_menu->modo_testo(false);
                                std::error_code ec;
                                const auto se = std::filesystem::read_symlink("/proc/self/exe", ec);
                                const std::filesystem::path motore = ec ? std::filesystem::path("melchior688") : se;
                                char quando[32];
                                const std::time_t adesso = std::time(nullptr);
                                std::strftime(quando, sizeof quando, "%Y%m%d_%H%M%S", std::localtime(&adesso));
                                const auto uscita = cartella_esperimenti() / (s->missione + "_" + quando);
                                std::filesystem::create_directories(uscita, ec);
                                const auto file = uscita / "esperimento.m688v";
                                bool chiusa = false;
                                try {
                                    scrivi_esperimento(file, reg.esperimento());
                                    AnelloFinestra w{[&] { return leggi_eventi(*finestra_menu); },
                                                     [&](const Tela& t, const Palette& p, const std::string& b) { presenta(*finestra_menu, t, p, b); },
                                                     [] { return Finestra::secondi(); }};
                                    const auto confronto = gioca_esperimento(w, tela, r.font, r.giorno, motore, file, uscita, chiusa);
                                    if (chiusa) return 0;
                                    if (confronto && !mostra_confronto(w, tela, r.font, r.giorno, o.dati, *confronto)) return 0;
                                } catch (const std::exception& e) {
                                    std::fprintf(stderr, "esperimento: %s\n", e.what());
                                }
                                u = SchermataRegolazioni::Uscita::nessuna;   // di nuovo le regolazioni
                                t_reg = Finestra::secondi();
                                continue;
                            }
                            if (u == SchermataRegolazioni::Uscita::serie) {   // S: 10 semi con queste regolazioni, poi il riassunto
                                Opzioni os = o;
                                os.sandbox = true;
                                os.regolazioni = reg.regolazioni();
                                os.meteo = os.regolazioni.meteo_realistico ? "realistico" : "semplificato";
                                os.navigazione = os.regolazioni.navigazione_realistica ? "realistica" : "perfetta";
                                os.sonar = os.regolazioni.anni_89 ? "1989" : "moderno";
                                if (os.seme > 0xFFFFFFFFull) os.seme = 1;   // il seme del codice non conta: la serie ha i suoi
                                const std::string cs = codice_della_partita(os, dalla_libreria, dall_editor);
                                bool chiusa = false;
                                const auto riassunto = cs.empty() ? std::nullopt : gioca_serie(*finestra_menu, tela, r, cs, 10, s->missione, chiusa);
                                if (chiusa) return 0;
                                if (riassunto && !mostra_serie(*finestra_menu, tela, r, o.dati, *riassunto)) return 0;
                                u = SchermataRegolazioni::Uscita::nessuna;   // di nuovo le regolazioni
                                continue;
                            }
                            reg.disegna(tela, r.font);
                            Palette pr = r.giorno;
                            reg.palette(pr);
                            presenta(*finestra_menu, tela, pr, brano_menu);
                        }
                        finestra_menu->modo_testo(false);
                        if (u == SchermataRegolazioni::Uscita::indietro) {   // di nuovo la scelta, come prima
                            o = o_prima;
                            rapida.reset();
                            dall_editor = dalla_libreria = false;
                            continue;
                        }
                        const RegolazioniSandbox& rg = reg.regolazioni();
                        o.sandbox = true;
                        o.regolazioni = rg;
                        o.briefing = false;
                        o.meteo = rg.meteo_realistico ? "realistico" : "semplificato";
                        o.navigazione = rg.navigazione_realistica ? "realistica" : "perfetta";
                        o.sonar = rg.anni_89 ? "1989" : "moderno";
                        if (rg.seme) o.seme = *rg.seme;
                    }
                    break;
                }
                scelta.disegna(tela, r.font);
                Palette pal = r.giorno;
                scelta.palette(pal);
                presenta(*finestra_menu, tela, pal, brano_menu);
            }
        }
        // --esperimento: il codice sandbox del file fa da --codice; gli interventi si programmano prima del primo passo
        std::vector<m688::Intervento> interventi_esperimento;
        if (!o.esperimento.empty()) {
            const Esperimento es = leggi_esperimento(o.esperimento);
            o.codice = es.codice;
            if (!o.senza_interventi) interventi_esperimento = es.interventi;
        }
        if (!o.codice.empty() && !rapida && !leggi_codice(o.codice)) {   // --codice di una partita: la missione, il seme, le opzioni
            std::optional<CodicePartita> k = leggi_codice_partita(o.codice);
            if (!k)   // un codice sandbox: la partita del vetrino con le sue regolazioni
                if (const auto ks = leggi_codice_sandbox(o.codice)) k = ks->partita, o.sandbox = true, o.regolazioni = ks->regolazioni;
            if (!k) throw std::runtime_error("codice non valido: " + o.codice);
            const std::filesystem::path base(o.cartella_missioni);
            const auto m = trova_missione(k->impronta, {base, base / "libreria", base / "prove", cartella_missioni_editor(),
                                                              cartella_missioni_importate()});   // prove: i vetrini pronti
            if (!m) throw std::runtime_error("la missione del codice non c'e', o e' cambiata: " + o.codice);
            o.missione = m->id;
            o.cartella_missioni = m->file.parent_path().string();
            if (!o.seme_dato) o.seme = k->seme;
            o.gps = k->gps;
            o.regola = RegolaDanni::simulazione;
            o.sonar = k->anni_89 ? "1989" : "moderno";
            o.carta = k->carta_epoca ? "epoca" : "vera";
            o.meteo = k->meteo_realistico ? "realistico" : "semplificato";
            static constexpr const char* ASW[] = {"facile", "normale", "realistico", "minaccia"};
            o.asw = ASW[std::clamp(k->asw, 0, 3)];
            static constexpr const char* SONARISTA[] = {"nessuno", "recluta", "medio", "esperto"};
            o.sonarista = SONARISTA[std::clamp(k->sonarista, 0, 3)];
            o.navigazione = k->navigazione ? "realistica" : "perfetta";
            o.traffico = !k->senza_traffico;
            if (k->origine != OrigineCodice::originale) {   // libreria, editor, importata: data, ora e teatro dallo scenario
                std::ifstream f(m->file, std::ios::binary);
                if (const auto sc = leggi_scenario(std::string{std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()})) {
                    o.inizio = {sc->anno, sc->mese, sc->giorno, sc->ora_utc};
                    if (sc->centro_lat != 0.0 || sc->centro_lon != 0.0) o.latitudine = sc->centro_lat, o.longitudine = sc->centro_lon;
                }
                dall_editor = true;
                dalla_libreria = k->origine == OrigineCodice::libreria;
            }
            o.codice.clear();
        }
        if (!o.codice.empty() && !rapida) {   // --codice: la missione rapida senza menu, come dalla scheda
            const auto p = leggi_codice(o.codice);
            if (!p) throw std::runtime_error("codice missione non valido: " + o.codice);
            const MissioneRapida m = genera_missione_rapida(*p, o.dati);
            const auto cartella = cartella_missioni_rapide();
            std::filesystem::create_directories(cartella);
            std::ofstream(cartella / (m.id + ".lua"), std::ios::binary) << m.lua;
            o.cartella_missioni = cartella.string();
            o.missione = m.id;
            o.seme = m.parametri.seme;
            o.inizio = {m.anno, m.mese, m.giorno, m.ora_utc};
            if (m.scenario.centro_lat != 0.0 || m.scenario.centro_lon != 0.0) {   // il sole e la luce sul teatro vero
                o.latitudine = m.scenario.centro_lat;
                o.longitudine = m.scenario.centro_lon;
            }
            if (m.parametri.meteo_realistico) o.meteo = "realistico";
            rapida = m;
        }
        // le originali dell'Enhanced partono dalla data e dall'ora dei loro ordini, le scene di prova e la libreria da riga di
        // comando da quella del loro file (app/data_missione.hpp): sole, luce, meteo della stagione, profilo del suono e DTG
        // coerenti con quel che si legge. --data e --ora prevalgono; rapide, editor e libreria dal menu hanno la loro
        if (!o.missione.empty() && o.regola == RegolaDanni::simulazione && !o.data_data && !rapida && !dall_editor)
            if (const auto d = data_della_missione(o.cartella_missioni, o.missione)) o.inizio = *d;
        // il codice partita (app/codice_partita.hpp) da mostrare e da registrare: la stessa missione, lo stesso seme, le
        // stesse opzioni. Le rapide hanno il loro codice; il classico e i semi oltre 32 bit (--seme) non ne hanno
        std::string codice_partita;
        if (rapida) {
            codice_partita = rapida->codice;
        } else {
            codice_partita = codice_della_partita(o, dalla_libreria, dall_editor);
        }
        if (!codice_partita.empty()) std::printf("codice partita: %s\n", codice_partita.c_str());
        // la partita; «Riprendi da qui» dal debriefing la ricomincia uguale e la rigioca fino all'istante scelto
        for (std::optional<Ripresa> ripresa;;) {
            // effetti visivi: fuoco e fumo delle navi colpite, scoppi, emersioni; li fa avanzare la partita o la scena di prova
            Effetti effetti(o.seme);
            effetti.imposta_vento(o.vento_da, o.vento);
            // missione originale convertita (Partita: missione, danni, stazioni, ordini, mondo) o scena di prova
            Mondo mondo_prova;
            std::unique_ptr<Partita> partita;
            if (!o.missione.empty())
                partita = std::make_unique<Partita>(r, ImpostazioniPartita{o.dati, o.cartella_missioni, o.missione, o.seme, o.regola, o.carta,
                                                                            o.inizio, o.urto_giocatore, o.urto_unita,
                                                                            ripresa ? ripresa->tempo : -1.0},
                                                    effetti);
            else
                crea_scena_prova(mondo_prova, r.classi);
            Missione* mis = partita ? &partita->missione() : nullptr;
            std::optional<ComandanteIa> comandante;   // il vetrino: il battello del giocatore comandato dall'IA
            std::optional<PianoOriginale> piano_vetrino;   // V3: il piano delle originali (sandbox_originali.toml)
            // il sole e la luce sul teatro vero anche per le originali (rapide, editor e libreria hanno il centro dello
            // scenario): il centro della batimetria georeferenziata, come il modello dei periscopi
            if (mis && o.regola == RegolaDanni::simulazione && !o.luogo_dato && !rapida && !dall_editor && mis->fondale() &&
                mis->fondale()->georeferenziato())
                o.latitudine = mis->fondale()->centro().lat, o.longitudine = mis->fondale()->centro().lon;
            if (mis && o.regola == RegolaDanni::simulazione) {   // prima del primo passo: meteo e difficolta' dell'ASW
                mis->usa_meteo(o.meteo == "realistico" ? Missione::ModoMeteo::realistico : Missione::ModoMeteo::semplificato);
                mis->usa_navigazione(o.navigazione == "realistica");   // la posizione propria incerta (prima del primo passo)
                if (o.luogo_dato) mis->luogo_del_sole(o.latitudine, o.longitudine);   // --lat/--lon: anche il Sole della missione
                mis->usa_gps(o.gps);   // l'opzione «1990+»: il GPS fra i punti nave (con la navigazione realistica)
                // il traffico civile (docs/20 §3): mai nelle scene di prova, che restano controllate
                const bool di_prova = o.missione.rfind("prove/", 0) == 0 || o.missione.rfind("PROVA_", 0) == 0 ||
                                      std::filesystem::path(o.cartella_missioni).filename() == "prove";
                mis->usa_traffico((o.sandbox ? o.regolazioni.traffico : o.traffico) && !di_prova);
                if (o.sandbox) {   // il vetrino (docs/19_sandbox.md): niente giocatore, le regolazioni per parte, gli interruttori
                    const RegolazioniSandbox& rg = o.regolazioni;
                    mis->senza_giocatore(true);
                    mis->registra_statistiche(true);   // le liste per le statistiche tecniche (R1-R6 del runtime)
                    Missione::InterruttoriIa ii;
                    ii.difesa = rg.difesa, ii.armi = rg.armi, ii.reazioni = rg.reazioni, ii.forze = rg.forze;
                    ii.esm = rg.esm_ia, ii.sonaristi = rg.sonaristi_ia;
                    mis->interruttori_ia(ii);
                    const auto parte = [](const SceltaParteSandbox& s) {
                        Missione::RegolazioniParte p;
                        if (s.situazione != SituazioneScelta::missione)
                            p.situazione = s.situazione == SituazioneScelta::pace    ? Situazione::pace
                                           : s.situazione == SituazioneScelta::crisi ? Situazione::crisi : Situazione::guerra;
                        if (s.roe != RoeScelta::missione) {   // tutte trattenute o tutte libere
                            std::map<CategoriaRoe, bool> roe;
                            for (int k = 0; k < static_cast<int>(CategoriaRoe::numero); ++k)
                                roe[static_cast<CategoriaRoe>(k)] = s.roe == RoeScelta::libere;
                            p.roe = roe;
                        }
                        if (s.livello != LivelloScelta::missione)   // V2: comandanti e sonaristi dell'IA
                            p.livello = s.livello == LivelloScelta::recluta  ? Esperienza::recluta
                                        : s.livello == LivelloScelta::esperto ? Esperienza::esperto : Esperienza::medio;
                        if (s.allerta >= 0) p.allerta = s.allerta;
                        p.carico_per_classe = s.carico_per_classe;
                        return p;
                    };
                    mis->regolazioni({{0x08, parte(rg.nato)}, {0x10, parte(rg.patto)}, {0, parte(rg.neutrali)}});
                    OpzioniComandante oc;   // il comandante del battello ha il livello dell'IA della sua parte
                    const UnitaClassica* g = mis->giocatore();
                    if (const auto lv = parte(g && (g->flag & 0x10) ? rg.patto : rg.nato).livello) oc.livello = *lv;
                    try {
                        oc.piano = piano_vetrino = piano_originale(o.dati, mis->id());
                    } catch (const std::exception& e) {
                        std::fprintf(stderr, "piano della missione non valido: %s\n", e.what());
                    }
                    comandante.emplace(*mis, oc, o.seme);
                }
                mis->imposta_difficolta_asw(o.asw == "facile" ? DifficoltaAsw::facile : o.asw == "realistico" ? DifficoltaAsw::realistico
                                            : o.asw == "minaccia" ? DifficoltaAsw::minaccia_percepita : DifficoltaAsw::normale);
                mis->meteo_enhanced(o.dati);   // gia' adesso (idempotente): il briefing mostra il meteo e la previsione
                // il ghiaccio per lo spettatore nel replay da file (solo il vetrino col ghiaccio)
                if (const std::string griglia = griglia_ghiaccio_missione(*mis); !griglia.empty() && partita->traccia())
                    partita->traccia()->metadato(METADATO_GRIGLIA_GHIACCIO, griglia);
                if (o.variazioni) mis->usa_variazioni_acustiche(*o.variazioni);   // R6 (opzione di realismo)
            if (o.sonarista != "nessuno")
                    mis->imposta_sonarista(o.sonarista == "recluta" ? Esperienza::recluta : o.sonarista == "esperto" ? Esperienza::esperto
                                                                                                            : Esperienza::medio);
                // l'equipaggio del battello (i transitori dello scafo): la carriera non ha ancora un livello suo, vale
                // quello scelto per il sonarista
                mis->equipaggio_giocatore(o.sonarista == "recluta" ? Esperienza::recluta : o.sonarista == "esperto" ? Esperienza::esperto
                                                                                                                 : Esperienza::medio);
                // il vetrino di Dio (docs/28): gli interventi vogliono il vetrino pronto (il catalogo acustico lo controlla la
                // missione all'applicazione). Rigiocato senza l'ultimo, gli altri agli stessi istanti; l'esperimento
                // (--esperimento), i programmati del file
                if (mis->sandbox()) {
                    const auto& programmati = ripresa ? ripresa->interventi : interventi_esperimento;
                    for (const auto& x : programmati)
                        if (const auto e = mis->intervieni(x); !e.ok)
                            std::fprintf(stderr, "intervento %s rifiutato: %s\n", x.tipo.c_str(), e.motivo_it.c_str());
                }
            }
            Mondo& mondo = partita ? partita->mondo() : mondo_prova;
            const int id_proprio = partita ? partita->id_proprio() : ID_PROPRIO;
            std::unique_ptr<InterfacciaMissione> interfaccia;   // creata piu' sotto; osserva gli ordini e i passi della partita
            CodaMessaggi coda_messaggi;                          // Enhanced: la barra dei messaggi (punto 21)
            std::size_t eventi_compressione = 0;                // eventi gia' visti per fermare la compressione
            const int stazione = o.stazione;
            Periscopio periscopio(r.font, r.classi, r.sprite);
            periscopio.usa_modelli(r.modelli, rampa_grigi(r.giorno));
            periscopio.usa_palette(r.giorno);   // le luminanze per i modi monocromi dell'oculare (TV, notte, termico)
            periscopio.usa_filo_di_lavoro(o.istantanea.empty());   // le navi vicine a 1 grado: in finestra senza scatti
            // fisica dei danni su una nave della scena di prova: la prima della classe scelta, colpita all'inizio
            std::unique_ptr<DanniNave> danni;
            if (!o.colpo.empty()) {
                const auto ic = r.classi.find(o.bersaglio);
                if (ic == r.classi.end() || ic->second.scafo.empty()) throw std::runtime_error(o.bersaglio + ": nessuno scafo per i danni");
                danni = std::make_unique<DanniNave>(DatiScafo::carica(o.dati + "/" + ic->second.scafo), o.seme);
                colpo_di_prova(*danni, o.colpo);
            }
            if (danni)
                for (const auto& n : mondo.navi())
                    if (n.classe == o.bersaglio) {
                        const auto im = r.modelli.find(n.classe);
                        effetti.segui(n.id, danni.get(), im != r.modelli.end() ? &im->second : nullptr);
                        break;
                    }
            for (const auto& sc : o.scoppi) {
                double kg, prof, dist, ril, fondale = std::numeric_limits<double>::infinity();
                if (std::sscanf(sc.c_str(), "%lf,%lf,%lf,%lf,%lf", &kg, &prof, &dist, &ril, &fondale) < 4)
                    throw std::runtime_error("scoppio non valido (KG,PROF,DIST,RIL[,FONDALE]): " + sc);
                effetti.esplosione_subacquea({dist * std::sin(radianti(ril)), dist * std::cos(radianti(ril))}, prof, kg, fondale);
            }
            for (const auto& sc : o.aeree) {
                double kg, quota, dist, ril;
                char tipo[16] = "";
                if (std::sscanf(sc.c_str(), "%lf,%lf,%lf,%lf,%15s", &kg, &quota, &dist, &ril, tipo) < 4)
                    throw std::runtime_error("esplosione non valida (KG,QUOTA,DIST,RIL[,deposito]): " + sc);
                effetti.esplosione_aerea({dist * std::sin(radianti(ril)), dist * std::cos(radianti(ril))}, quota, kg,
                                         std::string(tipo) == "deposito");
            }
            std::size_t eventi = 0;
            auto anima = [&](Mondo& m, double dt) {
                effetti.avanza(dt, m);
                if (!danni) return;
                danni->avanza(dt);
                for (; eventi < danni->registro().size(); ++eventi) std::printf("%s\n", danni->registro()[eventi].c_str());
                for (auto& n : m.navi())
                    if (n.classe == o.bersaglio) {   // colpita: niente propulsione, si ferma
                        n.velocita = n.velocita_ordinata = 0.0;
                        n.posa = danni->posa();
                        effetti.decora(n.id, n.posa);   // alberi caduti
                        break;
                    }
            };
            // ordini del giocatore (docs/11_comandi_e_stazioni.md, fase C1): consegnati all'inizio del passo, registrati
            // «Riprendi da qui»: lo storico fino all'istante scelto (dal debriefing, o da --ordini con --riprendi-da)
            if (partita && (ripresa || !o.ordini.empty())) {
                if (!ripresa) {
                    std::ifstream f(o.ordini);
                    if (!f) throw std::runtime_error("impossibile aprire " + o.ordini);
                    const std::string testo{std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};
                    if (o.riprendi_da >= 0.0) ripresa = ripresa_da_testo(testo, o.riprendi_da);
                    else partita->ordini() = RegistroOrdini::da_testo(testo);
                }
                if (ripresa) partita->ordini() = ripresa->ordini;
                // replay: lo storico porta il carico scelto alla partenza (Sala siluri), da applicare prima del primo passo
                if (const auto carico = partita->ordini().carico(); carico && o.regola == RegolaDanni::simulazione)
                    partita->imposta_carico(carica_carico(o.dati + "/carico.toml"), *carico);
            }
            auto salva_ordini = [&]() {
                if (o.registra.empty() || !partita) return;
                std::ofstream f(o.registra);
                if (!f) throw std::runtime_error("impossibile scrivere " + o.registra);
                f << "# ordini del giocatore: istante (s) tipo a b n (docs/11_comandi_e_stazioni.md)\n" << partita->ordini().serializza();
            };
            auto avanza_missione = [&](double dt) {
                if (comandante)   // il vetrino: gli ordini del comandante IA passano dal registro, come i tasti
                    for (const Ordine& ord : comandante->passo(partita->missione(), partita->stazioni())) partita->ordini().ordina(ord);
                partita->avanza(dt);
            };
            auto stato_manovra = [&]() -> std::optional<StatoManovra> {
                const auto* g = mis ? mis->giocatore() : nullptr;
                if (!g) return std::nullopt;
                StatoManovra m;
                const PostoDiComando& posto = partita->posto();
                m.telegrafo = posto.telegrafo(); m.timone = posto.timone(); m.barra = posto.barra();
                m.pilota = posto.pilota_automatico(); m.punti = static_cast<int>(mis->punti_di_rotta().size());
                m.periscopio = (g->flag54 & 2) != 0; m.antenna = (g->flag54 & 4) != 0;
                m.radar = mis->radar_giocatore_acceso();
                m.rotta = g->rotta; m.velocita_kn = g->velocita; m.quota_m = g->quota; m.quota_ordinata_m = g->quota_ordinata;
                m.cambia_quota = g->rateo_quota > 0.0 && std::fabs(g->quota_ordinata - g->quota) > 0.3;
                if (interfaccia) interfaccia->manovra_enhanced(m);   // Enhanced: posto e letture della dinamica
                return m;
            };
            // le missioni avanzano a passi fissi di 1/9 s, la cadenza della VM dell'originale (missione.cpp, PASSO_S): con un
            // passo piu' corto le regole a passi (visita dei sensori, contatori) correrebbero piu' veloci che nel 1989 e che
            // nel rilievo delle 110 partite. Il disegno interpola fra gli ultimi due passi, gli effetti girano a ogni
            // fotogramma (Partita::anima_effetti). La scena di prova resta a 30 passi al secondo
            const double passo_sim = mis ? 1.0 / 9.0 : 1.0 / 30.0;
            Orologio orologio(passo_sim);
            // «Riprendi da qui»: la partita rigiocata fino all'istante `fino`. La cascata del sonar (solo interfaccia, il
            // costo maggiore) resta ferma fino agli ultimi 10 minuti, che le ridanno la sua storia; `avanzamento` ogni
            // tanto, per la schermata d'attesa (falso: la finestra si chiude, e la ricostruzione con lei)
            const auto ricostruisci = [&](double fino, const std::function<bool(double)>& avanzamento) {
                if (interfaccia) interfaccia->ricostruzione(true);
                bool cascata = false;
                for (long k = 0; mis->tempo() < fino - 1e-9; ++k) {
                    if (interfaccia && !cascata && mis->tempo() >= fino - 600.0) interfaccia->ricostruzione(false), cascata = true;
                    DataOra ora = o.inizio;
                    ora.ore_utc = std::fmod(o.inizio.ore_utc + mis->tempo() / 3600.0, 24.0);
                    partita->imposta_luce(luce_ambiente(elevazione_sole(ora, o.latitudine, o.longitudine)));
                    avanza_missione(passo_sim);
                    if (avanzamento && k % 200 == 0 && !avanzamento(mis->tempo())) return false;
                }
                if (interfaccia) interfaccia->ricostruzione(false);   // l'audio riparte da adesso
                return true;
            };
            StatoPannello pannello;
            // interfaccia del giocatore (fasi C2-C4): tasti -> ordini, scelte che fermano il tempo, pannelli e carta
            if (mis && mis->giocatore())
                interfaccia = std::make_unique<InterfacciaMissione>(*mis, partita->stazioni(), partita->posto(), partita->ordini(),
                                                                    partita->equipaggio(), r.classi, stazione);
            if (interfaccia) {
                partita->osserva = [&](const Ordine& ord) { interfaccia->osserva(ord); };
                partita->dopo_passo = [&]() { interfaccia->dopo_passo(); };
                partita->dati_app = [&]() { return interfaccia->dati_app(); };   // per la traccia del replay
                interfaccia->sonar().imposta_modo(o.sonar == "1989" ? ModoSonar::anni_89 : ModoSonar::moderno);
                interfaccia->sonar().imposta_persistenza(static_cast<cascate::Persistenza>(leggi_persistenza_sonar()));
                interfaccia->carta().stile = o.stile_carta == "nautica" ? StileCarta::nautica : o.stile_carta == "foglio" ? StileCarta::foglio : StileCarta::vga;
                interfaccia->carta().anni_89 = o.sonar == "1989";   // le stazioni dell'Enhanced nel modo d'epoca
                interfaccia->carta().ora_inizio_s = o.inizio.ore_utc * 3600.0;
                interfaccia->carta().urss = mis->giocatore()->classe == "alfa";   // l'Alfa naviga sulla carta sovietica
                interfaccia->carta().tutte_le_scie = o.scie == "tutte";
                // il livello meteo della carta (vento, mare, celle, nebbia, fulmini): solo nella simulazione
                interfaccia->carta().livelli.meteo = o.regola == RegolaDanni::simulazione;
                interfaccia->carta().livelli.mobilia = o.regola == RegolaDanni::simulazione;   // Fase 3: carta nautica
                interfaccia->carta().scia_bersaglio = o.scie == "bersaglio";
                interfaccia->mostra_mate(o.mate);
                interfaccia->imposta_enhanced(o.regola == RegolaDanni::simulazione);
                if (o.regola == RegolaDanni::simulazione && o.consolle) interfaccia->imposta_consolle(o.dati);   // Fase 3
                interfaccia->imposta_comandi(o.dati, carica_tasti(o.tasti), o.istantanea.empty());   // F11; il suggerimento solo in finestra
                interfaccia->sonar().imposta_missione(o.missione);
                interfaccia->imposta_catalogo_carico([&partita] { return partita->catalogo_carico(); });
                std::map<std::string, double> alberi;   // altezza della testa d'albero per lo stadimetro
                for (const auto& [id, mod] : r.modelli) alberi[id] = mod.quota_max_m();
                interfaccia->imposta_alberi(std::move(alberi));
                if (o.regola == RegolaDanni::simulazione) {   // il libretto (docs/25) e le luci di navigazione (docs/27)
                    interfaccia->imposta_libretto(o.dati, &r.modelli, rampa_grigi(r.giorno));
                    interfaccia->imposta_luci(o.dati);
                    interfaccia->imposta_navi(&partita->danni());
                }
                if (const auto ic = r.classi.find(mis->giocatore()->classe); ic != r.classi.end() && !ic->second.scafo.empty())
                {
                    const DatiScafo scafo = DatiScafo::carica(o.dati + "/" + ic->second.scafo);
                    interfaccia->imposta_quota_collaudo(scafo.sottomarino.quota_collaudo_m);
                    // Enhanced: lo spaccato del quadro danni dal modello 3D e dai compartimenti della classe (kit)
                    if (const auto im = r.modelli.find(ic->first); im != r.modelli.end() && o.regola == RegolaDanni::simulazione)
                        interfaccia->imposta_spaccato(std::make_unique<Spaccato>(ic->first, im->second, scafo));
                }
            }

            const bool urss = mis && mis->giocatore()->classe == "alfa";
            const StileCarta stile_carta = o.stile_carta == "nautica" ? StileCarta::nautica : o.stile_carta == "foglio" ? StileCarta::foglio
                                                                                                                       : StileCarta::vga;
            // la Sala siluri (M2): il catalogo e la sala della classe del giocatore, se il catalogo ne ha il locale
            std::optional<CatalogoCarico> catalogo;
            std::optional<SalaSiluri> sala;
            std::map<std::string, CompitoCarico> compiti_carico;
            if ((o.briefing || o.schermata == "siluri") && mis && o.regola == RegolaDanni::simulazione && !partita->carico()) {
                try {
                    catalogo = carica_carico(o.dati + "/carico.toml");
                    compiti_carico = carica_compiti(o.dati + "/carico.toml");
                    sala.emplace(*catalogo, mis->giocatore()->classe, mis->limiti_carico(), false, o.sonar == "moderno");   // il modo moderno consente la MOSS (what-if)
                    sala->carico_iniziale();
                } catch (const std::exception& e) {
                    std::fprintf(stderr, "sala siluri non disponibile: %s\n", e.what());
                    sala.reset();
                }
            }
            // il vetrino: il carico per classe e anno del battello, se la sua parte lo chiede (le altre unita' lo prende
            // la missione del runtime); altrimenti quello della missione, come senza Sala siluri. Il piano delle
            // originali puo' chiedere armi nei tubi (SCENE0A: i Tomahawk), sopra l'uno o l'altro
            if (o.sandbox && mis && mis->giocatore() && !partita->carico()) {
                const int lato = mis->giocatore()->flag & 0x18;
                const SceltaParteSandbox& sp = lato == 0x10 ? o.regolazioni.patto : lato == 0x08 ? o.regolazioni.nato : o.regolazioni.neutrali;
                const bool tubi_piano = piano_vetrino && !piano_vetrino->tubi.empty();
                if (sp.carico_per_classe || tubi_piano) try {
                    CatalogoCarico cat = carica_carico(o.dati + "/carico.toml");
                    SalaSiluri sb(cat, mis->giocatore()->classe, mis->limiti_carico(), false, o.sonar == "moderno");
                    if (sp.carico_per_classe) sb.carico_standard();
                    else sb.carico_iniziale();
                    if (tubi_piano)
                        for (const auto& x : applica_tubi_piano(sb, *piano_vetrino)) std::fprintf(stderr, "carico del piano: %s\n", x.c_str());
                    const CaricoPartenza car = sb.carico();
                    partita->imposta_carico(std::move(cat), car);
                } catch (const std::exception& e) {
                    std::fprintf(stderr, "carico per classe non disponibile: %s\n", e.what());
                }
            }
            // il nome del battello, preso all'avvio: un battello perduto esce dalla missione prima del debriefing
            const std::string classe_giocatore = mis ? mis->giocatore()->classe : std::string();
            // le righe nuove del rapporto ACINT della stazione Sonar, per l'achievement «Per l'intelligence» (docs/26)
            const auto righe_acint = [&] {
                int n = 0;
                if (interfaccia)
                    if (const auto ra = interfaccia->sonar().rapporto_acint())
                        for (const auto& v : ra->voci) n += static_cast<int>(v.nuove.size());
                return n;
            };
            const auto nome_battello = [&] {
                const auto ic = r.classi.find(classe_giocatore);
                return ic != r.classi.end() && !ic->second.nome.empty() ? ic->second.nome : classe_giocatore;
            };
            if (!o.istantanea.empty() && o.schermata == "siluri" && sala) {   // la sala siluri, senza finestra
                SchermataSiluri s(*sala, nome_battello(), mis->limiti_carico().anno, mis->limiti_carico().tipo_missione, compiti_carico);
                if (o.arma == "elenco") s.apri_elenco();   // l'elenco dei carichi
                else if (!o.arma.empty()) s.scegli(o.arma);
                Palette pal = r.giorno;
                s.palette(pal);
                s.disegna(tela, r.font);
                tela.salva_bmp(o.istantanea, pal);
                std::printf("schermata siluri in %s\n", o.istantanea.c_str());
                return 0;
            }
            if (!o.istantanea.empty() && o.schermata.rfind("briefing", 0) == 0 && mis) {   // la sala briefing, senza finestra
                SchermataBriefing b(*mis, urss, o.sonar == "1989", stile_carta, o.dati);
                b.imposta_codice(codice_partita);
                b.completa();
                b.mostra_ordini(o.schermata != "briefing-carta");   // «briefing-carta»: la carta con gli ordini chiusi
                b.disegna(tela, r.font, *mis);
                Palette pal = r.giorno;
                b.palette(pal);
                tela.salva_bmp(o.istantanea, pal);
                std::printf("schermata briefing in %s\n", o.istantanea.c_str());
                return 0;
            }
            if (!o.istantanea.empty()) {   // modalita' senza finestra: verifiche e immagini per la documentazione
                const auto passi = static_cast<long>(std::lround(o.secondi / passo_sim));
                // con --briefing la partita parte col carico iniziale della Sala siluri, come dopo le schermate
                if (sala && o.briefing) partita->imposta_carico(std::move(*catalogo), sala->carico());
                // un'istantanea della vista e del pannello a questo istante (la stessa del finale, sotto)
                const auto scatta = [&](const std::string& file) {
                    const double t_sim = mis ? mis->tempo() : mondo.tempo();
                    if (mis) {
                        pannello.missione = mis->titolo();
                        pannello.manovra = stato_manovra();
                        if (interfaccia) interfaccia->pannello(pannello);
                        pannello.messaggio = partita->ultimo_messaggio();
                    }
                    punta_periscopio(mondo, id_proprio, o);
                    if (interfaccia) interfaccia->prepara_vista(o.vista);
                    Palette pal = componi(tela, r, periscopio, mondo, o.vista, o, pannello, id_proprio, t_sim, &effetti, nullptr, !(interfaccia && interfaccia->copre_la_vista()));
                    if (interfaccia) interfaccia->disegna(tela, r.font, o.vista);
                    if (interfaccia) interfaccia->palette(pal);
                    if (interfaccia && interfaccia->tremolio() > 0.0) pal = pal.scurita(interfaccia->tremolio(), {{0, 255}});
                    tela.salva_bmp(file, pal);
                    std::printf("istantanea %s dopo %.0f s simulati (impronta %016llx, contatti %016llx)\n", file.c_str(), t_sim,
                                static_cast<unsigned long long>(mondo.impronta()), static_cast<unsigned long long>(mis ? mis->impronta_contatti() : 0));
                };
                // i comandi dei tasti a tempo: «secondi comando [comando...]»
                std::vector<std::pair<double, std::vector<std::string>>> tasti;
                if (!o.tasti_a_tempo.empty()) {
                    std::ifstream in(o.tasti_a_tempo);
                    if (!in) throw std::runtime_error("--tasti-a-tempo: non trovo " + o.tasti_a_tempo);
                    for (std::string riga; std::getline(in, riga);) {
                        std::istringstream campi(riga);
                        double tt = 0.0;
                        if (riga.empty() || riga[0] == '#' || !(campi >> tt)) continue;
                        std::vector<std::string> nomi;
                        for (std::string x; campi >> x;) nomi.push_back(x);
                        tasti.emplace_back(tt, std::move(nomi));
                    }
                    std::stable_sort(tasti.begin(), tasti.end(), [](const auto& a, const auto& b) { return a.first < b.first; });
                }
                std::size_t prossimo_tasto = 0, prossima_istantanea = 0;
                const std::filesystem::path base(o.istantanea);
                if (mis) {
                    // --audio-wav: il mixer senza dispositivo, a ogni passo quanto basta, e alla fine il WAV (prove)
                    std::optional<Mixer> mixer_prova;
                    std::vector<float> audio_prova;
                    static const DatiSuoniClassici suoni_prova = [&] {
                        try {
                            return carica_suoni_classici(o.dati + "/suoni_classici.toml", std::filesystem::path(o.dati).parent_path().string());
                        } catch (const std::exception&) {
                            return DatiSuoniClassici{};
                        }
                    }();
                    // --sandbox --schermata vivo: la vista dal vivo alimentata come in finestra, disegnata alla fine
                    std::optional<SchermataReplay> vivo_prova;
                    if (o.sandbox && o.schermata == "vivo" && partita) {
                        Traccia vuota;
                        vuota.missione = o.missione;
                        vuota.seme = o.seme;
                        if (const auto* g = mis->giocatore()) vuota.metadati["giocatore"] = std::to_string(g->id);
                        vivo_prova.emplace(o.dati, std::move(vuota), mis->teatro());
                        vivo_prova->dal_vivo(true);
                        vivo_prova->griglia_ghiaccio(griglia_ghiaccio_missione(*mis));
                        partita->dopo_fotogramma = [&, suono = false](const Fotogramma& f) mutable {
                            vivo_prova->aggiungi(f);
                            if (!suono && mis->profilo_in({}, {})) suono = true, vivo_prova->mese_acustica(mis->mese_acustica());
                        };
                    }
                    if (!o.audio_wav.empty() && interfaccia) {
                        ParametriMixer pm;
                        pm.campionamento_hz = 22050;
                        pm.resa = o.audio == "sound_blaster" ? Resa::sound_blaster : o.audio == "originale" ? Resa::originale : Resa::piena;
                        mixer_prova.emplace(pm);
                        interfaccia->imposta_mixer(&*mixer_prova);
                        if (o.regola == RegolaDanni::classica) interfaccia->imposta_suoni_classici(&suoni_prova);
                    }
                    long i = 0;
                    if (ripresa) {   // «Riprendi da qui» senza finestra: la ricostruzione, poi gli stessi passi fino a N s
                        ricostruisci(ripresa->tempo, nullptr);
                        i = std::lround(mis->tempo() / passo_sim);
                        std::printf("ripresa da %.0f s (%zu ordini prima)\n", mis->tempo(), partita->ordini().storico().size());
                        while (prossimo_tasto < tasti.size() && tasti[prossimo_tasto].first < mis->tempo()) ++prossimo_tasto;
                        while (prossima_istantanea < o.istantanee.size() && o.istantanee[prossima_istantanea] < mis->tempo()) ++prossima_istantanea;
                    }
                    for (; i < passi; ++i) {
                        // il vetrino senza finestra finisce come in finestra: alla durata, o all'esito se richiesto (le serie)
                        if (o.sandbox && (mis->tempo() >= o.regolazioni.durata_min * 60.0 || (o.regolazioni.ferma_all_esito && mis->esito())))
                            break;
                        for (; prossimo_tasto < tasti.size() && tasti[prossimo_tasto].first <= mis->tempo() + 1e-9; ++prossimo_tasto)
                            if (interfaccia) {
                                Comandi c;
                                for (const auto& k : tasti[prossimo_tasto].second) {
                                    if (k == "su") c.su_giu = +1;
                                    else if (k == "giu") c.su_giu = -1;
                                    else if (k == "sinistra") c.ruota = -1;
                                    else if (k == "destra") c.ruota = +1;
                                    else applica(k, c);
                                }
                                interfaccia->comandi(c, o.vista, orologio.passo());
                            }
                        avanza_missione(passo_sim);
                        if (mixer_prova) {
                            interfaccia->audio(passo_sim, 1.0);
                            const auto pezzo = mixer_prova->produci(static_cast<std::size_t>(std::lround(22050 * passo_sim)));
                            audio_prova.insert(audio_prova.end(), pezzo.begin(), pezzo.end());
                        }
                        for (; prossima_istantanea < o.istantanee.size() && o.istantanee[prossima_istantanea] <= mis->tempo() + 1e-9; ++prossima_istantanea) {
                            char nome[32];
                            std::snprintf(nome, sizeof nome, "_%05d", static_cast<int>(std::lround(o.istantanee[prossima_istantanea])));
                            scatta((base.parent_path() / (base.stem().string() + nome + base.extension().string())).string());
                        }
                    }
                    if (o.sandbox && o.schermata == "statistiche-sandbox" && partita->traccia()) {   // la schermata, senza finestra
                        const Traccia tc = leggi_traccia(partita->traccia()->dati());
                        SchermataStatisticheSandbox sb(o.dati, tc);
                        sb.pagina(std::max(0, o.pagina - 1));
                        sb.disegna(tela, r.font);
                        Palette ps = r.giorno;
                        sb.palette(ps);
                        tela.salva_bmp(o.istantanea, ps);
                        std::printf("statistiche della sandbox in %s (pagina %d)\n", o.istantanea.c_str(), std::max(1, o.pagina));
                        return 0;
                    }
                    if (vivo_prova) {
                        vivo_prova->adesso(mis->tempo(), 1.0, false);
                        vivo_prova->disegna(tela, r.font);
                        Palette pv = r.giorno;
                        vivo_prova->palette(pv);
                        tela.salva_bmp(o.istantanea, pv);
                        std::printf("vista dal vivo in %s dopo %.0f s\n", o.istantanea.c_str(), mis->tempo());
                        partita->dopo_fotogramma = nullptr;
                        return 0;
                    }
                    if (mixer_prova) {
                        Suono s;
                        s.campionamento_hz = 22050;
                        for (const float x : audio_prova)
                            s.campioni.push_back(static_cast<std::int16_t>(std::lround(std::clamp(x, -1.0f, 1.0f) * 32767.0f)));
                        scrivi_wav(o.audio_wav, s);
                        std::printf("audio in %s (%.1f s)\n", o.audio_wav.c_str(), static_cast<double>(s.campioni.size()) / 22050.0);
                        interfaccia->imposta_mixer(nullptr);
                    }
                    pannello.missione = mis->titolo();
                    pannello.manovra = stato_manovra();
                    if (interfaccia && interfaccia->stazione() == 2 && !mis->radio()) interfaccia->storico(partita->storico_messaggi());
                    if (interfaccia) interfaccia->pannello(pannello);
                    pannello.messaggio = partita->ultimo_messaggio();
                    if (o.regola == RegolaDanni::simulazione) {   // Enhanced: due righe e il registro (punto 21)
                        pannello.due_righe = true;
                        if (interfaccia && interfaccia->registro_aperto()) {
                            interfaccia->registro(partita->storico_messaggi());
                            interfaccia->pannello(pannello);
                        }
                    }
                    partita->stampa_resoconto();
                    if (!o.salva_traccia.empty() && partita->traccia()) {
                        metadati_conti(*mis, *partita->traccia());   // come la traccia della partita vera
                        partita->traccia()->salva(o.salva_traccia);
                    }
                    if (o.schermata == "debriefing") {   // il debriefing dopo N secondi, senza finestra
                        SchermataDebriefing d(*partita, urss, stile_carta, &periscopio, &r.giorno);
                        d.imposta_dati(o.dati);
                        d.imposta_codice(codice_partita);
                        if (interfaccia) d.imposta_acint(interfaccia->sonar().rapporto_acint());
                        d.mostra_verita(o.vista.mostra_verita);
                        if (o.tutti) d.mostra_tutti(true);
                        d.vai(o.evento <= 0 ? d.eventi() - o.evento : o.evento - 1);   // -1, -2: le pagine dopo l'esito
                        d.disegna(tela, r.font);
                        Palette pal = r.giorno;
                        d.palette(pal);
                        tela.salva_bmp(o.istantanea, pal);
                        std::printf("schermata debriefing in %s (%d eventi)\n", o.istantanea.c_str(), d.eventi());
                        return 0;
                    }
                    if (o.schermata == "esito") {   // esito e cerimonie su un fascicolo nuovo (non si salva)
                        const RegoleCarriera regole = carica_carriera(o.dati + "/carriera.toml");
                        const TavolaDecorazioni tavola = TavolaDecorazioni::carica(regole, o.dati + "/decorazioni_grafica.toml");
                        const Marina marina = urss ? Marina::vmf : Marina::usn;
                        Fascicolo f = Carriera::nuovo(regole, marina, marina == Marina::usn ? "J. R. Halloran" : "V. A. Suchkov", true,
                                                      Indulgenza::realistica);
                        const MissioneChiusa chiusa = chiudi_partita(*partita, regole, f, nome_battello(), mis->teatro(), "originale", 0, "", "",
                                                                     realismo_partita(o, interfaccia && interfaccia->ril_visto()));
                        SchermateEsito e(chiusa, regole, tavola);
                        e.vai(o.pagina - 1);
                        e.disegna(tela, r.font);
                        Palette pal = r.giorno;
                        e.palette(pal);
                        tela.salva_bmp(o.istantanea, pal);
                        std::printf("schermata esito in %s (pagina %d di %d, punteggio %.0f)\n", o.istantanea.c_str(), e.pagina() + 1,
                                    e.pagine(), chiusa.esito.punteggio.totale);
                        return 0;
                    }
                } else {
                    // R7 (il trailer): con --audio-wav la scena di prova col suono di bordo, come in missione. Gli
                    // scoppi in acqua e in aria e il colpo sul bersaglio partono all'inizio, come gli effetti; si
                    // ascoltano dalla centrale (il battello immerso a quota periscopica) o dalla plancia (sul mare)
                    std::optional<AmbienteBordo> bordo_prova;
                    std::vector<float> audio_prova;
                    const bool plancia = o.ascolto == "plancia";
                    const double quota_ascolto = plancia ? 0.0 : 18.0;
                    if (!o.audio_wav.empty()) {
                        bordo_prova.emplace();
                        for (const auto& sc : o.scoppi) {
                            double kg = 0, prof = 0, dist = 0, ril = 0;
                            if (std::sscanf(sc.c_str(), "%lf,%lf,%lf", &kg, &prof, &dist) == 3) bordo_prova->scoppio(dist, kg, prof);
                            (void)ril;
                        }
                        for (const auto& sc : o.aeree) {
                            double kg = 0, quota = 0, dist = 0, ril = 0;
                            if (std::sscanf(sc.c_str(), "%lf,%lf,%lf,%lf", &kg, &quota, &dist, &ril) < 4) continue;
                            EventoArtiglieria e;
                            e.tipo = TipoEventoArtiglieria::scoppio_aria;
                            e.pos = {dist * std::sin(radianti(ril)), dist * std::cos(radianti(ril))};
                            e.quota_m = quota;
                            e.carica_kg = kg;
                            bordo_prova->artiglieria(e, {0.0, 0.0}, quota_ascolto);
                        }
                        if (!o.colpo.empty())   // il colpo sul bersaglio: il siluro sotto lo scafo, il missile sopra l'acqua
                            for (const auto& n : mondo.navi())
                                if (n.classe == o.bersaglio) {
                                    const double d = std::hypot(n.pos.x, n.pos.y);
                                    if (o.colpo == "missile") {
                                        EventoArtiglieria e;
                                        e.tipo = TipoEventoArtiglieria::scoppio_superficie;
                                        e.pos = n.pos;
                                        e.carica_kg = 165.0;
                                        bordo_prova->artiglieria(e, {0.0, 0.0}, quota_ascolto);
                                    } else {
                                        bordo_prova->scoppio(d, o.colpo == "siluro_chiglia" ? 544.0 : 300.0, 6.0);
                                        if (o.colpo == "due_siluri") bordo_prova->scoppio(d + 20.0, 300.0, 6.0);
                                    }
                                    break;
                                }
                    }
                    for (long i = 0; i < passi; ++i) {
                        anima(mondo, orologio.passo());
                        mondo.passo(orologio.passo());
                        if (bordo_prova) {
                            const auto v = bordo_prova->passo({}, {0.0, 0.0}, quota_ascolto, Ambiente{}, plancia ? StazioneBordo::plancia : StazioneBordo::centrale,
                                                              RegimeBordo::normale, orologio.passo());
                            audio_prova.insert(audio_prova.end(), v.begin(), v.end());
                        }
                    }
                    if (bordo_prova) {
                        Suono s;
                        s.campionamento_hz = bordo_prova->parametri().campionamento_hz;
                        for (const float x : audio_prova)
                            s.campioni.push_back(static_cast<std::int16_t>(std::lround(std::clamp(x, -1.0f, 1.0f) * 32767.0f)));
                        scrivi_wav(o.audio_wav, s);
                        std::printf("audio in %s (%.1f s, %s)\n", o.audio_wav.c_str(), static_cast<double>(s.campioni.size()) / s.campionamento_hz,
                                    plancia ? "plancia" : "centrale");
                    }
                }
                const double t_sim = mis ? mis->tempo() : mondo.tempo();
                punta_periscopio(mondo, id_proprio, o);
                if (interfaccia) interfaccia->prepara_vista(o.vista);
                Palette pal = componi(tela, r, periscopio, mondo, o.vista, o, pannello, id_proprio, t_sim, &effetti, nullptr, !(interfaccia && interfaccia->copre_la_vista()));
                if (interfaccia) interfaccia->messaggio_barra(pannello.messaggio);
                if (interfaccia) interfaccia->disegna(tela, r.font, o.vista);
                if (interfaccia) interfaccia->palette(pal);
                if (interfaccia && interfaccia->tremolio() > 0.0) pal = pal.scurita(interfaccia->tremolio(), {{0, 255}});
                std::printf("effetti: %zu particelle\n", effetti.numero());
                tela.salva_bmp(o.istantanea, pal);
                std::printf("istantanea %s dopo %.0f s simulati (impronta %016llx, contatti %016llx)\n", o.istantanea.c_str(), t_sim,
                            static_cast<unsigned long long>(mondo.impronta()), static_cast<unsigned long long>(mis ? mis->impronta_contatti() : 0));
                if (mis) salva_ordini();
                return 0;
            }

            if (!finestra_menu) {
                finestra_menu = std::make_unique<Finestra>("688 Attack Sub - Enhanced");
                finestra_menu->imposta_tasti(carica_tasti(o.tasti));
                applica_uscita(*finestra_menu, o);
            }
            Finestra& finestra = *finestra_menu;
            if (interfaccia && o.regola == RegolaDanni::simulazione)   // il quaderno del sonarista delle missioni precedenti
                interfaccia->sonar().carica_quaderno(SonarEnhanced::percorso_quaderno());
            // audio (R1): solo con la finestra; senza dispositivo si gioca in silenzio. Il mixer della regia (roadmap 38)
            // somma i canali (cuffia del sonar, bordo, interfaccia) che i produttori riempiono a ogni passo, la musica e
            // gli effetti; qui si tiene piena la coda
            apri_audio(o);
            UscitaAudio* uscita_audio = uscita_gioco.get();
            regia->resa(o.audio == "sound_blaster" ? Resa::sound_blaster : o.audio == "originale" ? Resa::originale : Resa::piena);
            Mixer& mixer = regia->mixer();
            classico_in_mare = o.regola == RegolaDanni::classica;
            if (interfaccia && uscita_audio) {
                interfaccia->imposta_mixer(&mixer);
                interfaccia->imposta_anticipo(ANTICIPO_CANALI_S);
            }
            // gli allarmi e i rumori di bordo dell'Enhanced (app/effetti_bordo): non nel classico ne' nel vetrino
            std::optional<EffettiDiBordo> effetti_bordo;
            if (uscita_audio && mis && !classico_in_mare && !o.sandbox) effetti_bordo.emplace(*regia, urss);
            imposta_effetti_di_bordo(effetti_bordo ? &*effetti_bordo : nullptr);
            // i campioni del 1989 per il classico (nucleo/suoni_classici): caricati una volta, anche se poi non servono
            static const DatiSuoniClassici suoni_classici = [&] {
                try {   // i file indicati sono relativi a data/, la cartella sopra data/motore
                    return carica_suoni_classici(o.dati + "/suoni_classici.toml", std::filesystem::path(o.dati).parent_path().string());
                } catch (const std::exception& e) {
                    std::fprintf(stderr, "suoni del classico non disponibili: %s\n", e.what());
                    return DatiSuoniClassici{};
                }
            }();
            if (interfaccia && uscita_audio && o.regola == RegolaDanni::classica) interfaccia->imposta_suoni_classici(&suoni_classici);
            // R5: i campioni reali per la cuffia del sonar, la stessa libreria della missione (una per processo)
            if (interfaccia && uscita_audio)
                if (const LibreriaCampioni* c = mis ? mis->campioni() : nullptr) interfaccia->sonar().imposta_campioni(c);
            const auto suona = [&](bool pausa) {
                if (!uscita_audio) return;
                uscita_audio->pausa(pausa);
                if (!pausa) regia->aggiorna_brano();
            };
            std::vector<AreaAttiva> aree;   // aree attive del pannello dell'ultimo fotogramma (mouse)
            double t_prec = Finestra::secondi();
            if (o.briefing && !ripresa && mis && o.regola == RegolaDanni::simulazione) {   // sala briefing (Fase 4a): Invio in mare, Esc rifiuta
                SchermataBriefing b(*mis, urss, o.sonar == "1989", stile_carta, o.dati);
                b.imposta_codice(codice_partita);
                for (;;) {
                    const Comandi c = leggi_eventi(finestra);
                    if (c.esci) return 0;
                    const double t_ora = Finestra::secondi(), dt = t_ora - t_prec;
                    t_prec = t_ora;
                    const auto u = b.comandi(c, dt);
                    if (u == SchermataBriefing::Uscita::rifiuta) return 0;
                    if (u == SchermataBriefing::Uscita::in_mare) break;
                    if (u == SchermataBriefing::Uscita::armi && sala) {   // la Sala siluri: Esc torna qui, Invio in mare
                        SchermataSiluri s(*sala, nome_battello(), mis->limiti_carico().anno, mis->limiti_carico().tipo_missione, compiti_carico);
                        Palette pal_s = r.giorno;
                        s.palette(pal_s);
                        bool in_mare = false;
                        for (;;) {
                            const Comandi cs = leggi_eventi(finestra);
                            if (cs.esci) return 0;
                            const double t1 = Finestra::secondi(), dt1 = t1 - t_prec;
                            t_prec = t1;
                            const auto us = s.comandi(cs, dt1);
                            if (us == SchermataSiluri::Uscita::briefing) break;
                            if (us == SchermataSiluri::Uscita::in_mare) {
                                in_mare = true;
                                break;
                            }
                            s.disegna(tela, r.font);
                            presenta(finestra, tela, pal_s, "briefing");
                        }
                        if (in_mare) break;
                        continue;
                    }
                    b.disegna(tela, r.font, *mis);
                    Palette pal = r.giorno;
                    b.palette(pal);
                    presenta(finestra, tela, pal, "briefing");
                }
                if (sala) partita->imposta_carico(std::move(*catalogo), sala->carico());   // prima del primo passo
                t_prec = Finestra::secondi();
            }
            if (ripresa && mis) {   // «Riprendi da qui»: la ricostruzione, con l'attesa a schermo
                const auto ora = [](double t) {
                    const int x = static_cast<int>(std::max(0.0, t));
                    char e[32];
                    std::snprintf(e, sizeof e, "T+%02d:%02d:%02d", x / 3600, x / 60 % 60, x % 60);
                    return std::string(e);
                };
                const bool fatta = ricostruisci(ripresa->tempo, [&](double t) {
                    if (leggi_eventi(finestra).esci) return false;
                    tela.riempi(colori::NERO);
                    const std::string righe[] = {"RICOSTRUZIONE DELLA SITUAZIONE", "RIPRESA DA " + ora(ripresa->tempo), ora(t)};
                    for (int k = 0; k < 3; ++k)
                        r.font.scrivi(tela, LARGHEZZA_TELA / 2 - r.font.larghezza(righe[k], 1) / 2, 190 + k * 16, righe[k],
                                      k == 0 ? colori::GIALLO : colori::BIANCO, 1);
                    const int bw = 400, bx = (LARGHEZZA_TELA - bw) / 2, by = 250;
                    tela.rettangolo_pieno(bx, by, bw, 10, colori::GRIGIO_SCURO);
                    tela.rettangolo_pieno(bx, by, static_cast<int>(bw * std::clamp(t / std::max(ripresa->tempo, 1.0), 0.0, 1.0)), 10,
                                          colori::VERDE_CHIARO);
                    presenta(finestra, tela, r.giorno, "");
                    return true;
                });
                if (!fatta) return 0;
                t_prec = Finestra::secondi();
            }
            int istantanee = 0;
            // il vetrino (docs/19_sandbox.md): la vista dal vivo di app/replay al posto delle stazioni. Riceve ogni
            // fotogramma registrato nella traccia; il tempo lo fa avanzare main, da 1x a 256x
            std::optional<SchermataReplay> vivo;
            bool pausa_vivo = false;
            std::optional<Ripresa> riavvio_vetrino;   // l'annulla di un intervento gia' applicato: si rigioca senza
            if (o.sandbox && mis && partita) {
                Traccia vuota;
                vuota.missione = o.missione;
                vuota.seme = o.seme;
                if (const auto* g = mis->giocatore()) vuota.metadati["giocatore"] = std::to_string(g->id);
                vivo.emplace(o.dati, std::move(vuota), mis->teatro());
                vivo->dal_vivo(true);
                vivo->griglia_ghiaccio(griglia_ghiaccio_missione(*mis));   // griglia e mese dell'acqua: solo alla vista dello spettatore
                partita->dopo_fotogramma = [&, suono = false](const Fotogramma& f) mutable {
                    vivo->aggiungi(f);
                    // il clima nasce al primo passo
                    if (!suono && mis->profilo_in({}, {})) suono = true, vivo->mese_acustica(mis->mese_acustica());
                };
            }
            for (;;) {
                if (vivo) {   // il vetrino: lo spettatore guarda dalla vista dal vivo, il motore va avanti da solo
                    const Comandi cv = leggi_eventi(finestra);
                    if (cv.esci) return 0;
                    const double t_ora = Finestra::secondi(), dt = t_ora - t_prec;
                    t_prec = t_ora;
                    const RichiesteVivo rq = vivo->comandi_vivo(cv, dt);
                    if (rq.fine) break;
                    if (rq.pausa) pausa_vivo = !pausa_vivo;
                    if (rq.compressione > 0) pannello.compressione = std::min(pannello.compressione * 2.0, 256.0);
                    if (rq.compressione < 0) pannello.compressione = std::max(pannello.compressione / 2.0, 1.0);
                    // il vetrino di Dio (docs/28, D1): gli interventi del pannello alla missione, che li applica all'inizio del
                    // passo; l'annulla toglie l'ultimo se non e' ancora scattato, se no si rigioca dall'inizio senza
                    for (const auto& x : rq.interventi)
                        if (const auto e = mis->intervieni(x); !e.ok) vivo->avviso("INTERVENTO RIFIUTATO: " + e.motivo_it);
                    if (rq.annulla_ultimo && !mis->interventi().empty()) {
                        const int ultimo = mis->interventi().back().id;
                        if (!mis->annulla_intervento(ultimo)) {
                            Ripresa rr = ripresa_da(partita->ordini(), mis->tempo());
                            const auto& stati = mis->stati_interventi();
                            for (std::size_t k = 0; k < mis->interventi().size(); ++k) {
                                m688::Intervento y = mis->interventi()[k];
                                if (y.id == ultimo || stati[k].fallito) continue;
                                if (stati[k].applicato) y.tempo = stati[k].inizio;
                                y.id = -1;
                                rr.interventi.push_back(std::move(y));
                            }
                            riavvio_vetrino = std::move(rr);
                            break;
                        }
                    }
                    const int passi = pausa_vivo ? 0 : orologio.avanza(dt, pannello.compressione);
                    DataOra ora = o.inizio;
                    ora.ore_utc = std::fmod(o.inizio.ore_utc + mis->tempo() / 3600.0, 24.0);
                    partita->imposta_luce(luce_ambiente(elevazione_sole(ora, o.latitudine, o.longitudine)));
                    partita->effetti_per_fotogramma = true;
                    for (int i = 0; i < passi; ++i) avanza_missione(passo_sim);
                    // «tempo compresso agli eventi» (docs/19 §3): anche dalla vista dal vivo si torna a 1x sui fatti dell'IA,
                    // come nel vetrino delle stazioni piu' sotto
                    for (const auto& ev = mis->eventi_missione(); eventi_compressione < ev.size(); ++eventi_compressione)
                        if (o.regolazioni.ferma_eventi) switch (ev[eventi_compressione].tipo) {
                            case TipoEvento::lancio_giocatore: case TipoEvento::lancio_nemico: case TipoEvento::lancio_ia:
                            case TipoEvento::colpo: case TipoEvento::affondamento: case TipoEvento::emersione_emergenza:
                            case TipoEvento::mina_esplosa:
                                pannello.compressione = 1.0;
                                break;
                            default: break;
                        }
                    partita->anima_effetti(pausa_vivo ? 0.0 : std::clamp(dt, 0.0, 0.25) * pannello.compressione);
                    vivo->adesso(mis->tempo(), pannello.compressione, pausa_vivo);
                    vivo->disegna(tela, r.font);
                    Palette pv = r.giorno;
                    vivo->palette(pv);
                    presenta(finestra, tela, pv, "");
                    if (mis->tempo() >= o.regolazioni.durata_min * 60.0 || (o.regolazioni.ferma_all_esito && mis->esito())) break;
                    continue;
                }
                Cronometro cr_eventi(&RegistroTempi::Riga::eventi);
                Comandi c = leggi_eventi(finestra);
                cr_eventi.ferma();
                if (o.sandbox) {   // il vetrino: lo spettatore guarda, non comanda (pausa, tempo, stazioni, zoom, uscita)
                    Comandi sp;
                    sp.esci = c.esci, sp.esc = c.esc, sp.pausa = c.pausa, sp.compressione = c.compressione;
                    sp.stazione = c.stazione, sp.verita = c.verita, sp.ingrandisci = c.ingrandisci, sp.riduci = c.riduci;
                    c = sp;
                }
                if (interfaccia && interfaccia->comandi_schermata(c)) {   // la schermata dei comandi aperta: alla partita non passa niente
                    const bool esci = c.esci;
                    c = Comandi{};
                    c.esci = esci;
                }
                {   // mouse: un clic su un'area attiva del pannello vale il suo comando, gli altri vanno alla vista
                    std::vector<std::pair<int, int>> altri;
                    for (const auto& [x, y] : c.clic) {
                        bool preso = false;
                        for (const auto& a : aree)
                            if (a.contiene(x, y)) { applica(a.comando, c); preso = true; break; }
                        if (!preso) altri.emplace_back(x, y);
                    }
                    c.clic = std::move(altri);
                }
                const double t_ora = Finestra::secondi(), dt = t_ora - t_prec;
                t_prec = t_ora;

                if (interfaccia) {
                    if (interfaccia->comandi(c, o.vista, dt)) break;
                    if (mis) mis->silenzio_giocatore(interfaccia->silenzio());   // i transitori dello scafo, piu' rari in silenzio
                    // il grado della persistenza del fosforo resta fra le partite: si salva quando cambia
                    static int persistenza_salvata = leggi_persistenza_sonar();
                    if (const int g = static_cast<int>(interfaccia->sonar().persistenza()); g != persistenza_salvata) {
                        scrivi_persistenza_sonar(g);
                        persistenza_salvata = g;
                    }
                    finestra.modo_testo(interfaccia->vuole_testo());   // la nota sulla carta prende i caratteri
                } else {
                    if (c.esci || c.esc) break;
                    if (c.cambia_ingrandimento || c.ingrandisci) o.vista.alto_ingrandimento = !o.vista.alto_ingrandimento;
                }
                if (c.verita) o.vista.mostra_verita = !o.vista.mostra_verita;
                if (o.vista.mostra_verita) o.verita_usata = true;
                if (c.compressione > 0) pannello.compressione = std::min(pannello.compressione * 2.0, o.sandbox ? 256.0 : 64.0);
                if (c.compressione < 0) pannello.compressione = std::max(pannello.compressione / 2.0, 1.0);
                // rotazione del periscopio: 20 gradi/s (rapida 60), un quarto ad alto ingrandimento
                const double velocita_rot = (c.veloce ? 60.0 : 20.0) * (o.vista.alto_ingrandimento ? 0.25 : 1.0);
                if (!interfaccia || !interfaccia->frecce_al_libretto())   // la scheda del filtro del libretto tiene le frecce
                    o.vista.rilevamento = normalizza_360(o.vista.rilevamento + c.ruota * velocita_rot * dt);
                punta_periscopio(mondo, id_proprio, o);

                const int passi = interfaccia && interfaccia->in_pausa() ? 0 : orologio.avanza(dt, pannello.compressione);
                if (mis) {
                    DataOra ora = o.inizio;   // la luce per gli effetti che cambiano col giorno (marcatori ASW)
                    ora.ore_utc = std::fmod(o.inizio.ore_utc + mis->tempo() / 3600.0, 24.0);
                    partita->imposta_luce(luce_ambiente(elevazione_sole(ora, o.latitudine, o.longitudine)));
                    partita->effetti_per_fotogramma = true;
                    for (int i = 0; i < passi; ++i) {
                        CronometroPasso cp;
                        avanza_missione(passo_sim);
                    }
                    // fumo, spruzzi, scie: a ogni fotogramma, col tempo simulato
                    partita->anima_effetti(interfaccia && interfaccia->in_pausa() ? 0.0 : std::clamp(dt, 0.0, 0.25) * pannello.compressione);
                    pannello.missione = mis->titolo();
                    pannello.messaggio = partita->ultimo_messaggio();
                    if (o.regola == RegolaDanni::simulazione && !mis->esito()) {   // Enhanced: la coda dei messaggi (punto 21)
                        // la compressione del tempo si ferma da sola a ogni evento importante (docs/11 §3.4): contatto
                        // nuovo, siluro in acqua, colpo, avvistamento, danno, ping subito, e ogni messaggio critico
                        const auto& ev = mis->eventi_missione();
                        for (; eventi_compressione < ev.size(); ++eventi_compressione)
                            if (o.sandbox) {   // il vetrino si ferma solo se richiesto (docs/19 §3), sui fatti dell'IA
                                if (o.regolazioni.ferma_eventi) switch (ev[eventi_compressione].tipo) {
                                    case TipoEvento::lancio_giocatore: case TipoEvento::lancio_nemico: case TipoEvento::lancio_ia:
                                    case TipoEvento::colpo: case TipoEvento::affondamento: case TipoEvento::emersione_emergenza:
                                    case TipoEvento::mina_esplosa:
                                        pannello.compressione = 1.0;
                                        break;
                                    default: break;
                                }
                            } else switch (ev[eventi_compressione].tipo) {
                                case TipoEvento::scoperta: case TipoEvento::lancio_nemico: case TipoEvento::colpo:
                                case TipoEvento::avvistato: case TipoEvento::danno_subito: case TipoEvento::ping_subito:
                                case TipoEvento::aereo_in_sorvolo: case TipoEvento::artiglieria_salva:
                                    pannello.compressione = 1.0;
                                    break;
                                default: break;
                            }
                        const std::string prima = coda_messaggi.corrente();
                        const auto storico = partita->storico_messaggi();
                        coda_messaggi.aggiorna(storico, Finestra::secondi());
                        if (!o.sandbox && coda_messaggi.corrente() != prima && CodaMessaggi::critico(coda_messaggi.corrente()))
                            pannello.compressione = 1.0;
                        if (!coda_messaggi.corrente().empty()) pannello.messaggio = coda_messaggi.corrente();
                        pannello.due_righe = true;
                        if (interfaccia && interfaccia->registro_aperto()) interfaccia->registro(storico);
                    }
                    if (comandante) {   // il vetrino: cosa sta facendo il comandante IA, e perche'
                        const StatoComandante& sc = comandante->stato();
                        std::string fase_ia = std::string("COMANDANTE IA: ") + nome_fase_comandante(sc.fase) + (sc.motivo.empty() ? "" : " - " + sc.motivo);
                        for (auto& ch : fase_ia) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
                        pannello.messaggio = fase_ia;
                    }
                    pannello.manovra = stato_manovra();
                    if (interfaccia && interfaccia->stazione() == 2 && !mis->radio()) interfaccia->storico(partita->storico_messaggi());
                    if (interfaccia) interfaccia->pannello(pannello);
                    // il vetrino finisce da solo: alla durata massima, o all'esito della missione se lo si e' chiesto
                    if (o.sandbox && (mis->tempo() >= o.regolazioni.durata_min * 60.0 || (o.regolazioni.ferma_all_esito && mis->esito())))
                        break;
                } else {
                    for (int i = 0; i < passi; ++i) {
                        anima(mondo, orologio.passo());
                        mondo.passo(orologio.passo());
                    }
                }

                // disegno sullo stato interpolato tra gli ultimi due passi: moto fluido a ogni ritmo di fotogrammi
                Cronometro cr_disegno(&RegistroTempi::Riga::disegno);
                const Mondo vista = mondo.interpolato(orologio.alfa());
                if (interfaccia && mis) interfaccia->imposta_tempo_disegno(mis->tempo() + orologio.alfa() * passo_sim);
                if (interfaccia) interfaccia->prepara_vista(o.vista);
                Palette pal = componi(tela, r, periscopio, vista, o.vista, o, pannello, id_proprio,
                                            mis ? mis->tempo() : mondo.tempo(), &effetti, &aree, !(interfaccia && interfaccia->copre_la_vista()));
                if (interfaccia) interfaccia->messaggio_barra(pannello.messaggio);
                if (interfaccia) interfaccia->disegna(tela, r.font, o.vista);
                if (interfaccia && interfaccia->copre_pannello()) aree.clear();   // la pagina di tiro copre il pannello Armi
                if (interfaccia) aree.insert(aree.end(), interfaccia->aree_vista().begin(), interfaccia->aree_vista().end());
                if (interfaccia) interfaccia->palette(pal);
                if (interfaccia && interfaccia->tremolio() > 0.0) pal = pal.scurita(interfaccia->tremolio(), {{0, 255}});   // luci che tremolano
                cr_disegno.ferma();
                if (c.istantanea) {
                    char nome[64];
                    std::snprintf(nome, sizeof nome, "istantanea_%03d.bmp", ++istantanee);
                    tela.salva_bmp(nome, pal);
                }
                if (interfaccia)   // F1: la scheda dell'enciclopedia pertinente, a tempo fermo (si torna dov'eri con Esc)
                    if (const auto voci = interfaccia->enciclopedia_chiesta(); !voci.empty()) {
                        static std::optional<Enciclopedia> enc;
                        if (!enc) {
                            try {
                                enc = carica_enciclopedia(o.dati + "/enciclopedia.toml");
                            } catch (const std::exception&) {
                                enc.emplace();
                            }
                        }
                        std::string id;
                        for (const auto& x : voci)
                            if (id.empty() && enc->voce(x)) id = x;
                        SchermataMuseo museo(*enc, r, o.dati, id);
                        if (uscita_audio) uscita_audio->pausa(true);
                        double t0 = Finestra::secondi();
                        for (;;) {
                            const Comandi cm = leggi_eventi(finestra);
                            if (cm.esci) break;
                            const double t1 = Finestra::secondi();
                            if (museo.comandi(cm, t1 - t0)) break;
                            t0 = t1;
                            museo.disegna(tela, r.font);
                            Palette pal_museo = r.giorno;
                            museo.palette(pal_museo);
                            presenta(finestra, tela, pal_museo, "");
                        }
                        t_prec = Finestra::secondi();   // il tempo passato nel Museo non conta per la missione
                        continue;
                    }
                if (interfaccia) {
                    Cronometro cr(&RegistroTempi::Riga::cuffia);
                    interfaccia->audio(dt, pannello.compressione);
                }
                if (effetti_bordo) effetti_bordo->aggiorna(*mis);
                suona(interfaccia && interfaccia->in_pausa());
                presenta(finestra, tela, pal, "");
                chiudi_fotogramma(finestra, "partita", interfaccia ? interfaccia->stazione() : 0, mis ? mis->tempo() : mondo.tempo());
            }
            if (uscita_audio) uscita_audio->svuota(), uscita_audio->pausa(false);   // fuori dal mare: la musica del debriefing
            regia->resa(Resa::piena);
            classico_in_mare = false;
            imposta_effetti_di_bordo(nullptr);
            if (riavvio_vetrino) {   // l'annulla del vetrino di Dio: la stessa partita da capo, ricostruita fino all'istante
                ripresa = std::move(riavvio_vetrino);
                continue;
            }
            // il quaderno del sonarista: gli slot pieni restano per la prossima missione (solo in gioco, non nelle istantanee)
            if (interfaccia && o.regola == RegolaDanni::simulazione) interfaccia->sonar().salva_quaderno(SonarEnhanced::percorso_quaderno());
            if (mis) salva_ordini();
            // la traccia del replay (nucleo/traccia_replay): in carriera nella cartella del comandante, col nome nel
            // fascicolo; fuori carriera (riprese, missioni che non contano, partite senza esito) fra le ultime 20 libere
            const std::string origine = rapida ? "rapida" : dalla_libreria ? "libreria" : dall_editor ? "editor" : "originale";
            bool traccia_salvata = false;
            const auto salva_traccia = [&](Fascicolo* f) {
                RegistratoreTraccia* tr = partita ? partita->traccia() : nullptr;
                if (!tr || !mis || traccia_salvata) return;
                traccia_salvata = true;
                try {
                    tr->metadato("origine", origine);
                    if (!codice_partita.empty()) tr->metadato("codice", codice_partita);
                    if (f) tr->metadato("comandante", f->id);
                    const auto p = percorso_replay(f ? f->id : std::string(), mis->id(), f ? f->data : std::string());
                    metadati_conti(*mis, *tr);   // le grandezze continue (ghiaccio, coda, luci, salvati) per le statistiche
                    tr->salva(p);
                    if (f && !f->missioni.empty()) f->missioni.back().replay = p.filename().string();
                    if (!f) pulisci_replay_liberi(20);
                    // le statistiche (roadmap 6) dalla stessa traccia: le globali sempre, il comandante se la missione
                    // conta per la carriera; una ripresa non conta per nessuno; il vetrino e' fuori da tutto (docs/19:
                    // l'IA contro l'IA non e' una partita del giocatore)
                    if (!o.sandbox)
                        registra_statistiche(report_missione(leggi_traccia(tr->dati()), mis->catalogo_acustico()), f ? f->id : std::string(),
                                             f != nullptr, ripresa.has_value());
                } catch (const std::exception& e) {
                    std::fprintf(stderr, "traccia del replay non salvata: %s\n", e.what());
                }
            };
            if (ripresa || !(o.briefing || o.menu)) salva_traccia(nullptr);
            if ((o.briefing || o.menu) && mis && o.regola == RegolaDanni::simulazione) {   // debriefing ed esito (Fase 4a, M5)
                finestra.modo_testo(false);
                SchermataDebriefing d(*partita, urss, stile_carta, &periscopio, &r.giorno);
                // la musica del debriefing: riuscito col successo, anche parziale, fallito altrimenti (roadmap 38)
                const auto cat_esito = partita->missione().esito_dettagliato().categoria;
                const std::string brano_debriefing = cat_esito == EsitoDettagliato::Categoria::successo ||
                                                             cat_esito == EsitoDettagliato::Categoria::successo_parziale ||
                                                             cat_esito == EsitoDettagliato::Categoria::in_corso
                                                         ? "debriefing_riuscito"
                                                         : "debriefing_fallito";
                d.imposta_dati(o.dati);
                d.imposta_codice(codice_partita);
                if (o.sandbox) d.mostra_tutti(true), d.mostra_verita(true);   // lo spettatore vede tutto
                if (interfaccia) d.imposta_acint(interfaccia->sonar().rapporto_acint());
                if (ripresa) d.imposta_ripresa(ripresa->tempo);
                // la carriera si chiude una volta sola, alla prima uscita dal debriefing, e il fascicolo si salva subito.
                // «Riprendi da qui» prima la chiude con la missione cosi' com'e'; una ripresa non conta per la carriera
                std::optional<RegoleCarriera> regole;
                std::optional<TavolaDecorazioni> tavola;
                std::optional<MissioneChiusa> chiusa;
                std::unique_ptr<SchermateEsito> esito;
                for (bool fine = false; !fine;) {
                    for (;;) {
                        const Comandi c = leggi_eventi(finestra);
                        if (c.esci) return 0;
                        const double t_ora = Finestra::secondi(), dt = t_ora - t_prec;
                        t_prec = t_ora;
                        if (d.comandi(c, dt)) {
                            if (d.statistiche_chieste()) {   // le statistiche della missione dalla traccia in memoria
                                if (RegistratoreTraccia* tr = partita->traccia()) {
                                    try {
                                        const Traccia tc = leggi_traccia(tr->dati());
                                        if (o.sandbox) {   // il vetrino: le statistiche tecniche (docs/19 §4), e dal clic il replay li'
                                            SchermataStatisticheSandbox sb(o.dati, tc);
                                            for (double t_sb = Finestra::secondi();;) {
                                                const Comandi cs = leggi_eventi(finestra);
                                                if (cs.esci) return 0;
                                                const double ora = Finestra::secondi();
                                                if (sb.comandi(cs, ora - t_sb)) break;
                                                t_sb = ora;
                                                sb.disegna(tela, r.font);
                                                Palette pal = r.giorno;
                                                sb.palette(pal);
                                                presenta(finestra, tela, pal, "albo");
                                            }
                                            if (const auto t0 = sb.replay_da()) {
                                                SchermataReplay rp(o.dati, tc, mis->teatro());
                                                rp.vai(*t0);
                                                for (double t_rp = Finestra::secondi();;) {
                                                    const Comandi cr = leggi_eventi(finestra);
                                                    if (cr.esci) return 0;
                                                    const double ora = Finestra::secondi();
                                                    if (rp.comandi(cr, ora - t_rp)) break;
                                                    t_rp = ora;
                                                    rp.disegna(tela, r.font);
                                                    Palette pal = r.giorno;
                                                    rp.palette(pal);
                                                    presenta(finestra, tela, pal, "");
                                                }
                                            }
                                            t_prec = Finestra::secondi();
                                            continue;
                                        }
                                        SchermataStatistiche st(o.dati, report_missione(tc, mis->catalogo_acustico()), tc);
                                        for (double t_st = Finestra::secondi();;) {
                                            const Comandi cs = leggi_eventi(finestra);
                                            if (cs.esci) return 0;
                                            const double ora = Finestra::secondi();
                                            if (st.comandi(cs, ora - t_st)) break;
                                            t_st = ora;
                                            st.disegna(tela, r.font);
                                            Palette pal = r.giorno;
                                            st.palette(pal);
                                            presenta(finestra, tela, pal, "albo");
                                        }
                                    } catch (const std::exception& e) {
                                        std::fprintf(stderr, "statistiche non disponibili: %s\n", e.what());
                                    }
                                }
                                t_prec = Finestra::secondi();
                                continue;
                            }
                            if (!d.replay_chiesto()) break;
                            // il replay completo dalla traccia in memoria; Invio sceglie l'istante per «Riprendi da qui»
                            if (RegistratoreTraccia* tr = partita->traccia()) {
                                try {
                                    SchermataReplay rp(o.dati, leggi_traccia(tr->dati()), mis->teatro());
                                    for (double t_rp = Finestra::secondi();;) {
                                        const Comandi cr = leggi_eventi(finestra);
                                        if (cr.esci) return 0;
                                        const double ora = Finestra::secondi();
                                        if (rp.comandi(cr, ora - t_rp)) break;
                                        t_rp = ora;
                                        rp.disegna(tela, r.font);
                                        Palette pal = r.giorno;
                                        rp.palette(pal);
                                        presenta(finestra, tela, pal, "");
                                    }
                                    if (const auto t = rp.riprendi_da()) {
                                        d.riprendi(*t);
                                        break;
                                    }
                                } catch (const std::exception& e) {
                                    std::fprintf(stderr, "replay non disponibile: %s\n", e.what());
                                }
                            }
                            t_prec = Finestra::secondi();
                            continue;
                        }
                        d.disegna(tela, r.font);
                        Palette pal = r.giorno;
                        d.palette(pal);
                        presenta(finestra, tela, pal, brano_debriefing);
                    }
                    if (ripresa) break;   // fuori carriera: niente esito ne' cerimonie
                    if (o.sandbox) {   // il vetrino e' fuori carriera: il replay fra i liberi, niente esito ne' cerimonie
                        salva_traccia(nullptr);
                        break;
                    }
                    if (!esito) {
                        try {
                            regole = carica_carriera(o.dati + "/carriera.toml");
                            tavola = TavolaDecorazioni::carica(*regole, o.dati + "/decorazioni_grafica.toml");
                            Fascicolo f = fascicolo_comandante(*regole, urss ? Marina::vmf : Marina::usn);
                            // una rapida conta col suo seme, se lo chiede e il comandante non ha gia' giocato il codice
                            const bool conta = rapida && rapida->conta_carriera && !codice_giocato(f.id, rapida->codice);
                            chiusa = chiudi_partita(*partita, *regole, f, nome_battello(), mis->teatro(), origine,
                                                    conta ? rapida->parametri.seme : 0, o.dati, codice_partita, realismo_partita(o, interfaccia && interfaccia->ril_visto()),
                                                    righe_acint());
                            salva_traccia(chiusa->esito.contata ? &f : nullptr);
                            salva_fascicolo(f, percorso_fascicolo(f.id));
                            if (conta) registra_codice(f.id, rapida->codice);
                            if (caricabile(f)) imposta_in_carica(f.marina, f.id);   // l'albo: chi e' andato in mare resta in carica
                            esito = std::make_unique<SchermateEsito>(*chiusa, *regole, *tavola);
                        } catch (const std::exception& e) {
                            std::fprintf(stderr, "carriera non disponibile: %s\n", e.what());
                            salva_traccia(nullptr);
                            break;
                        }
                    }
                    if (d.riprendi_da()) break;   // la missione e' agli atti: si riprende
                    for (;;) {
                        const Comandi c = leggi_eventi(finestra);
                        if (c.esci) return 0;
                        const auto u = esito->comandi(c);
                        if (u == SchermateEsito::Uscita::debriefing) break;
                        if (u == SchermateEsito::Uscita::fine) {
                            fine = true;
                            break;
                        }
                        esito->disegna(tela, r.font);
                        Palette pal = r.giorno;
                        esito->palette(pal);
                        presenta(finestra, tela, pal, "cerimonia", false);
                    }
                    t_prec = Finestra::secondi();
                }
                if (const auto t = d.riprendi_da()) {
                    ripresa = ripresa_da(partita->ordini(), *t);
                    continue;
                }
            }
            return 0;
        }
    } catch (const std::exception& e) {
        std::fprintf(stderr, "melchior688: %s\n", e.what());
        return 1;
    }
}
