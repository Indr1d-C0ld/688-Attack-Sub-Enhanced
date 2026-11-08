#pragma once
// Ambiente sonoro di bordo (requisito 27, pezzo R3; dossier docs/ricerca/18_audio_reale.md §6): cio' che si sente in
// una stazione senza cuffie. Le stesse sorgenti della scena acustica (Missione::scena_acustica) arrivano allo scafo con
// la loro perdita di trasmissione e il Doppler, attraversano lo scafo con K(f) (STIMA di §6.2: passa basso, 91 dB sotto
// 250 Hz, 96 a 1 kHz, 107 a 4 kHz, 113 a 8 kHz, meno il riverbero del locale), si sommano al fondo della stazione
// (centrale, sonar, siluri, macchine, alloggi) secondo il regime, e si tengono solo sopra la soglia d'udibilita'.
// Gli aerei non si sentono a bordo (scelta dell'utente: solo al sonar).
//
// Eventi brevi, a campione esatto:
// - i ping degli altri (transitori «ping» della scena) arrivano come ticchettio o ghiaia sullo scafo, cioe' il
//   transitorio d'inizio e la risposta dello scafo a 1-4 kHz, non il tono (§6.1);
// - i tonfi (boe, siluri, trasduttori) quando passano la soglia, cioe' vicinissimi;
// - il ping proprio, forte nei locali prodieri (93-108 dB misurati sulla Tigrone, §6.1);
// - gli scoppi (cariche, siluri, mine): onda d'urto, pulsazioni della bolla e rombo, col «scossone» per la vista;
// - il maltempo (M5, docs/ricerca/21_note/e_audio_meteo.md §3 e §5.4, citato «21e»): i colpi di mare sullo scafo al
//   periodo d'onda, che calano con la quota come il moto orbitale (in profondita' niente), l'acqua sul ponte in
//   superficie, il tuono in superficie dopo distanza / c, e i fulmini caduti vicino come tonfi (netti entro ~100 m: a 200 m ~52 dB dentro). La pioggia e il vento
//   dentro lo scafo non si sentono (21e §3.2: 45 dB d'isolamento a 100 Hz, 65 a 1 kHz).
//
// Il modulo e' puro e deterministico. Unita' dei campioni: 0 dBFS rms = `riferimento_db_spl` dB SPL (100 dB: la sala
// macchine a ~87 dBA sta a -13 dBFS, la centrale silenziosa a ~-50).

#include <cstdint>
#include <map>
#include <optional>
#include <vector>

#include "nucleo/ascolto.hpp"
#include "nucleo/acustica_artiglieria.hpp"
#include "nucleo/acustica_urto.hpp"
#include "nucleo/meteo.hpp"

namespace m688 {

// plancia: all'aperto col battello in superficie (G5): niente scafo, fondo di vento e mare; l'acqua non vi arriva
enum class StazioneBordo : std::uint8_t { centrale, sonar, siluri, macchine, alloggi, plancia };
enum class RegimeBordo : std::uint8_t { silenzioso, normale, avanti_tutta };
const char* nome_stazione_bordo(StazioneBordo s);

struct ParametriBordo {
    int campionamento_hz = 22050;
    double riferimento_db_spl = 100.0;
    std::uint64_t seme = 1;
};

// Il fondo della stazione (dBA) per regime (STIMA da [Cla92] e [Har71], dossier 18 §6.3)
double fondo_stazione_dba(StazioneBordo s, RegimeBordo r);
// K(f): dal livello esterno (dB re 1 uPa) a quello interno (dB re 20 uPa), per la stazione (§6.2)
double trasferimento_scafo_db(double f_hz, StazioneBordo s);
// soglia d'udibilita' (dB SPL, ISO 226 approssimata, da verificare: dossier 18 §6.3)
double soglia_udibilita_db(double f_hz);

class AmbienteBordo {
public:
    explicit AmbienteBordo(const ParametriBordo& p = {});
    const ParametriBordo& parametri() const { return p_; }

    // `dt_s` di suono per la stazione: le sorgenti della scena (senza il proprio battello), dove sta lo scafo (posizione
    // e quota), l'ambiente; continuo fra un passo e l'altro come la cuffia
    // R3b: con `proprio` (la sorgente acustica del battello, quella della scena) la stazione sente le sue macchine (le
    // righe di righe_emesse, coerenti con R6, e il fischio della turbina che sale coi giri), i suoi transitori (le voci
    // «scafo» di R7 entro 50 m, per via aerea, forti) e la ventilazione; gli alloggi ovattati. Senza, come prima
    std::vector<float> passo(const std::vector<Sorgente>& sorgenti, Vec2 pos, double quota_m, const Ambiente& amb,
                             StazioneBordo stazione, RegimeBordo regime, double dt_s, const Sorgente* proprio = nullptr);
    StazioneBordo stazione() const { return stazione_; }   // per la diagnosi: l'ultima stazione e l'ultimo regime
    RegimeBordo regime() const { return regime_; }

    // uno scoppio a `distanza_m` (carica di profondita', testata, mina) di `tnt_kg` alla profondita' `quota_m`
    void scoppio(double distanza_m, double tnt_kg, double quota_m);
    // il ping del proprio sonar attivo (livello di sorgente, frequenza, durata)
    void ping_proprio(double sl_db, double hz, double durata_s);
    // il meteo nel punto del battello (Meteo::in): mare, swell e periodo per i colpi di mare; tiene fino al prossimo
    void meteo(const StatoMeteo& s);
    // un tuono: il lampo a `distanza_m` (nella nube: 10 dB sotto e piu' lungo), il suono parte dopo distanza / c con c
    // dalla temperatura dell'aria; oltre 20 km niente (rifrazione, 21e §2). Si sente solo col battello in superficie
    // quando arriva: dentro lo scafo 45 dB sotto e passa basso a 200 Hz (21e §5.3-5.4)
    void tuono(double distanza_m, bool nube_mare, double t_aria_c = 10.0);
    // L'artiglieria (G5, dossier 22 §7.4 e §9.7), all'istante dell'evento; i ritardi in aria (R / c) li mette lui.
    // Immerso: lo schiocco sullo scafo degli scoppi (picco_scoppio_db meno K), udibile entro 1-2 km; l'urto metallico
    // col gorgoglio quando e' colpita la testa del periscopio. In superficie: il boato della partenza (e il colpo sordo
    // dell'illuminante che si apre), lo scoppio,
    // lo schiocco supersonico e il sibilo del proietto che passa, gli spruzzi che ricadono; pieni in plancia, 45 dB
    // sotto e cupi nelle stazioni interne (come il tuono), piu' i tonfi che arrivano dall'acqua. STIMA quasi tutto
    void artiglieria(const EventoArtiglieria& e, Vec2 pos, double quota_m, double t_aria_c = 10.0);
    // Una collisione (dossier 16 §2.6), all'istante dell'urto. Con `proprio` (la parte di EsitoUrto del battello) e'
    // il nostro scafo: arriva per la struttura, forte in ogni stazione e in plancia (STIMA: 115 dB a 1 MJ assorbito,
    // +10 per decade, fra 90 e 135; lo stridio 60 dB sotto il suo livello in acqua), col colpo squillante se c'e' di
    // mezzo una lama e lo scossone dalla scossa_g. Senza, l'urto di altri a `distanza_m` dall'acqua, come uno scoppio
    void urto(const EsitoUrto& e, const DanniUrtoUnita* proprio, double distanza_m = 0.0);
    // intensita' dell'ultimo scoppio per lo scossone della vista (0..1, cala in un secondo)
    double scossone() const { return scossone_; }
    void azzera();

private:
    struct Evento {
        enum class Tipo : std::uint8_t { ticchettio, tonfo, ping, scoppio, tuono, onda, urto_testa, passaggio, spruzzi, raffica, urto, stridio } tipo = Tipo::tonfo;
        std::uint64_t inizio = 0;           // campione
        double ampiezza = 0.0;              // picco, unita' d'uscita
        double durata_s = 0.1;
        double hz = 0.0;                    // ping: il tono
        double bolla_s = 0.0;               // scoppio: periodo della bolla
        Casuale caso{1};                    // il rumore dell'evento
        double filtro = 0.0, filtro2 = 0.0; // passa basso del rumore
        double filtro3 = 0.0, filtro4 = 0.0;
        double taglio_hz = 200.0;           // tuono: il passa basso della distanza e dello scafo
        bool vicino = false;                // tuono: lo schianto iniziale (sotto 3 km); onda: in superficie; urto: squilla
        std::vector<std::pair<double, double>> colpi;   // tuono: i rimbombi secondari (s, ampiezza relativa)
        double intervallo_s = 0.0;          // raffica: fra un colpo e l'altro
    };
    void prepara(const std::vector<Sorgente>& sorgenti, Vec2 pos, double quota_m, const Ambiente& amb, StazioneBordo s, RegimeBordo r,
                 const Sorgente* proprio);
    double campione_evento(Evento& e, std::uint64_t k);

    ParametriBordo p_;
    GeneratoreSpettrale g_;
    double resto_ = 0.0, dal_aggiornamento_ = 0.0;
    bool pronto_ = false;
    std::uint64_t campione_ = 0;             // campioni prodotti
    StazioneBordo stazione_ = StazioneBordo::centrale;
    RegimeBordo regime_ = RegimeBordo::normale;
    std::vector<Evento> eventi_;
    std::map<int, bool> attivi_;             // transitori della scena gia' sentiti (sorgente -> attivo)
    double scossone_ = 0.0;
    std::uint64_t prossimo_seme_ = 1;
    // il maltempo
    std::optional<StatoMeteo> meteo_;
    double quota_m_ = 0.0;                   // l'ultima letta: la superficie per il tuono e i colpi di mare
    std::uint64_t prossima_onda_ = 0;        // campione del prossimo colpo di mare possibile
    Casuale onde_{0x0dde};
    void colpo_di_mare(std::uint64_t k);
};

}  // namespace m688
