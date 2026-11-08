#pragma once
// Il libretto di riconoscimento al periscopio (docs/25_libretto_riconoscimento.md, scelte dell'utente del 29/10/2026):
// le tavole delle classi come i manuali dell'ONI, inchiostro su carta. Le sagome si fanno dai modelli 3D del kit a
// 0, 45, 90 e 135 gradi sulla prua (genera_sprite, poi l'inchiostro: nero pieno, retino dove la luce batte); la pagina
// e' un documento della variante B, 210x340 circa, nel terzo destro della vista accanto all'oculare.
// Indici: solo gli EGA 0-15 (la carta bianca 15, l'inchiostro nero 0, il rosso 4 della classifica, il blu 1 della
// penna), cosi' la pagina sta sopra la vista senza toccarne la palette.

#include <array>
#include <map>
#include <string>
#include <vector>

#include "grafica/font.hpp"
#include "grafica/modello.hpp"
#include "grafica/palette.hpp"
#include "grafica/sprite.hpp"
#include "grafica/tela.hpp"
#include "nucleo/luci_navigazione.hpp"

namespace m688::libretto {

// Una tavola del libretto: i dati noti nel 1989 (data/motore/libretto.toml, tools/pipeline/libretto.py). Non sono la
// verita' del gioco: l'altezza d'albero e' quella dell'intelligence, con la sua incertezza
struct Voce {
    std::string classe;               // id di classi.toml
    std::string nome;                 // «KRIVAK II»
    std::string sottotitolo;          // «PROGETTO 1135M, FREGATA ANTISOM»
    std::string sezione;              // «NAVI DA GUERRA - URSS»
    std::string parte;                // nato, patto, neutrali, civili
    std::string tipo;                 // fregata, mercantile... (il filtro)
    bool sottomarino = false;         // per i sottomarini solo la vela e gli alberi
    double dislocamento_t = 0.0;
    double lunghezza_m = 0.0;
    double albero_m = 0.0;            // testa d'albero sul galleggiamento, come la sa l'intelligence
    double albero_sigma_m = 0.0;      // la sua incertezza (1 sigma)
    // Il secondo riferimento, per le navi con l'albero oltre il quadrante del Type 2 e del Type 18 (39,62 m): un punto
    // noto entro il quadrante («TETTO DELLA PLANCIA», «FUMAIOLO PRODIERO»), con la sua altezza e la sua incertezza.
    // Nome vuoto e zeri per le altre classi; il telemetro lo usa quando l'albero non ci sta
    std::string riferimento_nome;
    double riferimento_m = 0.0;
    double riferimento_sigma_m = 0.0;
    std::string segni;                // i segni distintivi, una riga
    std::vector<std::string> armi;    // armi visibili
    std::vector<std::string> sensori; // sensori visibili
    std::vector<std::string> emettitori;   // radar per l'ESM (i nomi NATO)
    std::vector<std::string> emettitori_id;   // id di emettitori.toml, per collegarla all'ESM
    std::vector<std::string> vede;    // le etichette del filtro «cosa vedo»: ponte_volo, lanciatori, vela...
    std::string voce_museo;           // «nave.krivak2»
    std::string classifica = "CONFIDENTIAL";
    int ordine = 0;                   // la posizione nel libretto (1 la prima tavola)
    // l'album sovietico dell'Alfa (in_stile_sovietico): il piede «ALBOM SILUETOV» e, al posto del quadrante, il telemetro
    // a reticolo
    bool sovietico = false;
};

// Le tavole di data/motore/libretto.toml nell'ordine del file (navi sovietiche, della NATO, sottomarini, civili).
// Lancia std::runtime_error se il file non si legge
std::vector<Voce> carica(const std::string& percorso);

// L'album dei siluetti sovietico per l'Alfa (roadmap 33, approvato dall'utente il 31/10/2026): prima le navi e i
// sottomarini della NATO, poi quelli sovietici, poi le civili; le testate traslitterate («KORABLI VMS SSHA I NATO»,
// «KORABLI VMF SSSR»...), il timbro «SOV. SEKRETNO», il piede «ALBOM SILUETOV» e il telemetro a reticolo. Il contenuto
// delle tavole resta in italiano, come gli ordini della radio
void in_stile_sovietico(std::vector<Voce>& voci);

// Il filtro delle tavole (docs/25 §3.1): per parte, per tipo e per «cosa vedo». Una tavola passa se ha la parte e il
// tipo scelti (vuoti: tutte) e tutte le etichette di `vede`
struct Filtro {
    std::string parte;                // «patto», «nato», «civili»; vuota = tutte
    std::string tipo;                 // «fregata»...; vuoto = tutti
    std::vector<std::string> vede;    // «lanciatori», «un_fumaiolo»...: devono esserci tutte
    bool attivo() const { return !parte.empty() || !tipo.empty() || !vede.empty(); }
};

// Gli indici in `voci` delle tavole che passano, nell'ordine del libretto
std::vector<int> filtra(const std::vector<Voce>& voci, const Filtro& filtro);

// Gli elenchi per scegliere, senza doppioni. Le parti e i tipi nell'ordine del libretto (i tipi della sola `parte`, se
// non e' vuota); le etichette nell'ordine del vocabolario (ponte di volo, hangar, lanciatori... vela), poi le altre
std::vector<std::string> parti(const std::vector<Voce>& voci);
std::vector<std::string> tipi(const std::vector<Voce>& voci, const std::string& parte = {});
std::vector<std::string> etichette(const std::vector<Voce>& voci);

// Come si scrivono sulla carta: «PATTO», «NAVE SPIA», «PIU' FUMAIOLI», «PLANCIA A PRUA». Le etichette fuori dal
// vocabolario in maiuscolo, coi trattini bassi come spazi
std::string testo_etichetta(const std::string& etichetta);
std::string testo_maiuscolo(const std::string& s);

// La scheda del filtro ha una riga per la parte, una per il tipo, poi una per etichetta: 2 + etichette(voci).size()
int righe_filtro(const std::vector<Voce>& voci);

// Il filtro dopo un comando sulla riga `riga` della scheda: sulla parte e sul tipo `passo` +1 o -1 scorre l'elenco
// (TUTTE, poi le voci, e di nuovo TUTTE), 0 torna a TUTTE; su un'etichetta, qualunque passo la mette o la toglie.
// Cambiata la parte, un tipo che non c'e' piu' torna a TUTTI
Filtro scegli(const std::vector<Voce>& voci, Filtro filtro, int riga, int passo);

// La striscia del filtro, una riga d'inchiostro alta piccolo.righe() + 3: una fascia nera con la carta sopra,
// «FILTRO: PATTO / FREGATA / LANCIATORI» a sinistra e a destra `destra` («12 TAVOLE», o «3/12» nella testata della
// pagina). «FILTRO:» resta fuori se non ci sta tutto; il resto si tronca coi puntini. Senza filtro attivo
// «FILTRO: TUTTE LE TAVOLE»
void striscia(Tela& t, Rettangolo r, const Font& piccolo, const Filtro& filtro, const std::string& destra);

// La scheda del filtro aperta, sulla carta come la pagina (in r, la stessa 210x340): la testata «FILTRO DELLE TAVOLE»
// col numero delle tavole che passano, la PARTE e il TIPO fra le frecce, poi «COSA VEDO» con una casella per etichetta
// e a destra quante tavole resterebbero mettendola (in grigio le etichette che non lascerebbero niente). La riga
// `riga` (come in scegli) e' in negativo. `aiuto`, se c'e', va nel piede in tre righe al piu': i tasti li decide chi la usa
void scheda_filtro(Tela& t, Rettangolo r, const Font& f, const Font& piccolo, const std::vector<Voce>& voci, const Filtro& filtro,
                   int riga, const std::string& aiuto = {});

// La sagoma d'inchiostro della classe vista da `aob` gradi sulla prua (0 prua, 90 profilo, 180 poppa), a `scala_px_m`
// pixel per metro. Pixel 0 l'inchiostro, 15 la carta (il retino della luce), TRASPARENTE fuori; l'ancora e' quella di
// genera_sprite. Con `solo_vela` si tiene solo cio' che sta sopra il dorso (un terzo dell'altezza, almeno 3 m: i
// sottomarini, vela e alberi), ritagliato alla sua larghezza, con l'ancora sul taglio
Sprite sagoma(const Modello& m, double aob, double scala_px_m, const std::array<double, 15>& rampa, bool solo_vela = false);

// La pagina del libretto in r (210x340 circa): la testata con la sezione e il numero, il nome e il sottotitolo, il
// profilo grande, le tre viste piccole (0, 45, 135) con l'angolo sotto, i dati (dislocamento, lunghezza, ALBERO con
// l'incertezza; per le navi oltre il quadrante anche il riferimento, in un riquadro a filo), i segni, le armi e i
// sensori, gli emettitori, il rimando al museo e la classifica.
// `sagome`: profilo, prua, 45, 135 (gia' a scala); `pagina` e `pagine` per «12/36». `f` e' il font del nome e
// dell'albero (originale.fnt), `piccolo` quello del testo: fosforo688_5x7.fnt, che ha trattino, virgola e punto e virgola
// Restituisce falso se il testo non e' entrato tutto (le righe in piu' si tacciono): i dati vanno accorciati.
// Con un `filtro` attivo la testata diventa la striscia del filtro, con «3/12» (pagina e pagine contate fra le tavole
// che passano): la sezione si legge gia' dal nome
bool pagina(Tela& t, Rettangolo r, const Font& f, const Font& piccolo, const Voce& v, const std::array<const Sprite*, 4>& sagome,
            int pagina, int pagine, const Filtro* filtro = nullptr);

// La tavola «luci» (docs/27 §2, scelta dell'utente del 29/10/2026): gli schemi del COLREG come li vede la vedetta di
// notte, su otto riquadri neri dalle configurazioni vere di luci.toml.
// - il mercantile da prora, da dritta e da poppa;
// - alla fonda;
// - il peschereccio a strascico in pesca;
// - la manovra limitata;
// - senza governo;
// - il sottomarino americano col faro ambra.
// Ognuno ha lo scafo e gli alberi in grigio, i fanali accesi nel loro settore e due righe di testo. In r (la stessa
// 210x340 della pagina); `pagina` e `pagine` per il numero in alto. Servono le classi mercantile, peschereccio e
// los_angeles: se ne manca una il suo riquadro resta vuoto e la funzione restituisce falso
bool pagina_luci(Tela& t, Rettangolo r, const Font& f, const Font& piccolo,
                 const std::map<std::string, luci::Configurazione>& configurazioni, int pagina, int pagine);

// La seconda tavola «luci»: l'AOB dalle luci (docs/27 §2). I colori dei laterali (solo verde, solo rosso, tutti e due,
// un bianco solo) con quello che dicono dell'angolo sulla prora; i due bianchi d'albero del mercantile visti a 0, 20,
// 45 e 90 gradi a dritta; la regola AOB = arcsen(D / L), con L la distanza fra gli alberi della tavola. Falso se manca
// il mercantile in `configurazioni`
bool pagina_luci_aob(Tela& t, Rettangolo r, const Font& f, const Font& piccolo,
                     const std::map<std::string, luci::Configurazione>& configurazioni, int pagina, int pagine);

}  // namespace m688::libretto
