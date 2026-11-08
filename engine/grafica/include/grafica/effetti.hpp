#pragma once
// Effetti visivi di danni ed esplosioni (docs/10_danni_affondamento_esplosioni.md §5.3-5.6, fase D4): particelle
// nel mondo 3D, non sullo schermo, quindi seguono distanza, orizzonte e ingrandimento come le navi.
// - Esplosioni subacquee: anello scuro, cupola di spruzzi, pennacchi (o colonna con la corona scura, o solo una
//   gobba), ricaduta e base surge, nebbia, chiazza di schiuma o di fango, pulsazioni; numeri da nucleo/fenomeni.hpp.
//   Il seme varia numero, forma e inclinazione dei pennacchi.
// - Esplosioni nell'aria: lampo sulla palette, palla di fuoco, schegge balistiche, sbuffo o fungo di fumo.
// - Navi seguite (nucleo/danni.hpp): fiamme e fumo dai locali che bruciano (colonna di Briggs piegata dal vento,
//   colore dal combustibile), vapore dove l'acqua spegne il fuoco, orlo di schiuma sul galleggiamento inclinato,
//   ribollio d'aria sopra le falle che imbarcano acqua, spruzzi quando lo scafo si muove in fretta o si spezza;
//   dopo l'affondamento ribollio, vortice di schiuma, chiazza di nafta e rottami alla deriva; alberi che cadono
//   (asta rigida attorno alla base) per uno scoppio vicino, la frusta della bolla o la rottura dello scafo.
// - ASW aereo (docs/13_asw_aereo.md, pezzo A4; dossier 17 §4.3 e §8.1): anello di spruzzi sotto l'elicottero in hover,
//   cavo e trasduttore del sonar a immersione (tonfo quando entra in acqua, gocce quando esce), MAD rimorchiato, luci
//   anticollisione e di navigazione; boe che scendono col paracadute, tonfo, galleggiante con l'antenna e macchia di
//   colorante; siluro col paracadute e tonfo; marcatori Mk 25, Mk 58 e OMAB. Lo stato arriva dalla missione (aerei e
//   boe a ogni passo, marcatori e armi una volta): nessuna dipendenza dal controllore dell'IA.
// - Artiglieria navale (docs/16_artiglieria.md, pezzo G4; dossier 22 §6 e §9.6): dagli EventoArtiglieria della missione
//   colonne d'acqua per calibro col lampo alla base (il 16" dell'Iowa tinto d'arancio), sbuffo dei VT e spruzzi sotto,
//   tonfo dell'inerte, rimbalzi lungo la linea di tiro, raffiche dell'AK-630 coi traccianti, vampe e fumo sulle navi
//   che sparano, illuminanti a paracadute con la luce sul mare; per l'oculare l'acqua delle colonne vicine sul vetro.
// Tutto e' deterministico: stesso seme, stessa sequenza.

#include <cstdint>
#include <limits>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "grafica/modello.hpp"
#include "nucleo/acustica_artiglieria.hpp"
#include "nucleo/asw_aereo.hpp"
#include "nucleo/casuale.hpp"
#include "nucleo/danni.hpp"
#include "nucleo/geometria.hpp"
#include "nucleo/mondo.hpp"

namespace m688 {

class Tela;

enum class Materia : std::uint8_t {
    fuoco,          // fiamme e palla di fuoco: rampa emissiva
    fumo,           // dal nero al grigio secondo `tono`; illuminato dal basso di notte
    vapore,         // bianco, si dissolve in fretta
    spruzzo,        // acqua: cupole, pennacchi, ricadute
    nebbia,         // spruzzi sospesi dopo la ricaduta
    corona,         // gas grigio-nero sopra la colonna di sfiato
    fango,          // acqua marrone delle mine da fondo
    detrito,        // schegge e rottami
    schiuma,        // sul mare (piatta) o bolle che scoppiano
    anello_scuro,   // "slick" dell'onda d'urto: scurisce il mare
    nafta,          // chiazza di nafta: scurisce il mare per ore
    colorante,      // macchia di colorante sul mare (fluoresceina delle boe e degli OMAB): verde, solo di giorno
    luce,           // luce di posizione: un punto che non si spegne di notte; colore in `tono` (0 rosso, 0,5 verde, 1 bianco)
    cavo,           // filo verticale lungo `H` metri sotto la particella (cavo del sonar a immersione, antenna delle boe)
    tinta,          // acqua tinta delle colonne del 16" (Iowa: arancio, dossier 22 §6.1)
    tracciante,     // proietto tracciante in volo: un punto rosso-arancio col suo tratto (moto rettilineo)
    illuminante,    // illuminante a paracadute: nucleo bianco con l'alone, scende piano col vento
};

struct Particella {
    Materia materia = Materia::spruzzo;
    double x = 0, y = 0, z = 0;          // est, nord, quota sul mare (m)
    double vx = 0, vy = 0, vz = 0;       // m/s
    double eta = 0, vita = 1;            // s
    double ritardo = 0;                  // s prima di comparire (sequenze delle esplosioni)
    double r = 1, dr = 0;                // raggio (m) e crescita (m/s)
    double r0 = 0, r_max = 1e9;          // raggio alla base della colonna di fumo; raggio massimo
    double opacita = 1;
    double calore = 0;                   // fuoco: posizione nella rampa (1 bianco-giallo, 0 brace); fumo: luce del fuoco
    double tono = 0.5;                   // fumo: 0 nero, 1 grigio chiaro
    // salita prescritta: z = z0 + H (1 - (1 - t/ta)^2) per t < ta, poi caduta balistica fino a v_limite
    double z0 = 0, H = 0, ta = 0, v_limite = 30;
    double F = 0, percorso = 0;          // fumo: flusso di galleggiamento (Briggs) e distanza percorsa nell'aria
    bool piatta = false;                 // sul mare: chiazze, schiuma, anelli
    bool galleggia = false;              // rottami: restano a galla e vanno alla deriva
    bool propulsa = false;               // missili e fiamme dei booster: moto rettilineo, senza gravita' ne' vento
    std::uint32_t seme = 0;              // tremolio delle fiamme
};

// Un mezzo aereo ASW visto dagli effetti, a ogni passo (dalla missione: posizione e ComandoVolo)
struct AereoAsw {
    int id = 0;
    Vec2 pos;
    double quota_m = 150.0;
    double rotta = 0.0, velocita = 0.0;   // gradi veri, m/s
    bool elicottero = false, hover = false;
    double rotore_m = 0.0;     // diametro del rotore principale: l'anello di spruzzi in hover (SH-3H 18,9, Ka-27 15,9)
    double cavo_m = 0.0;       // sonar a immersione: cavo filato dall'elicottero (ComandoVolo::cavo_m); 0 = a bordo
    double mad_m = 0.0;        // MAD rimorchiato: lunghezza del cavo (180 ft = 55 m); 0 = niente MAD rimorchiato
    double apertura_m = 10.0;  // fra le luci di navigazione: apertura alare, o larghezza della fusoliera per gli elicotteri
    bool luci = true;          // anticollisione e navigazione; spente in caccia (§8.1)
    // vento al mare (docs/15, M3): il cavo del sonar a immersione pende sottovento e l'anello di spruzzi si sposta;
    // 0 = calma (come prima)
    double vento_m_s = 0.0, vento_da_gradi = 0.0;
};

enum class MarcatoreAsw : std::uint8_t {
    mk25,          // fiamma gialla e fumo bianco, 10-20 min
    mk58,          // due candele, 40-60 min
    omab_giorno,   // OMAB-25-12D: macchia di fluoresceina
    omab_notte,    // OMAB-25-8N: fiamma di acetilene
};

class Effetti {
public:
    explicit Effetti(std::uint64_t seme = 1);

    // Vento del teatro: direzione da cui soffia (gradi veri) e velocita' (m/s). Predefinito: da 250 gradi a 7 m/s.
    void imposta_vento(double da_gradi, double velocita);

    // W: TNT equivalente per la bolla (kg); profondita' della carica (m); fondale nel punto (m).
    void esplosione_subacquea(Vec2 pos, double profondita, double W,
                              double fondale = std::numeric_limits<double>::infinity());
    // Testata di un missile (deposito = false) o deposito di munizioni, alla quota z sul mare.
    void esplosione_aerea(Vec2 pos, double z, double W, bool deposito);
    // Segni in superficie di un sottomarino colpito (fase D6e, docs/10 §10): 0 aria (schiuma e bolle), 1 nafta
    // (chiazza scura), 2 rottami alla deriva; `durata` in s, `intensita` 0..1 per estensione e quantita'
    void segno_superficie(int tipo, Vec2 pos, double durata, double intensita);
    // Emersione violenta di un sottomarino (E1, docs/10 §10.14): pos = centro del battello, rotta (gradi bussola),
    // velocita' (m/s), angolo della prua al culmine (gradi), lunghezza (m), fuori = frazione della lunghezza fuori
    // dall'acqua al culmine, ricaduta = s dall'arrivo in superficie al colpo della prua sul mare. Acqua che cola dalla
    // prua che esce, colonna alla prua, schiuma lungo lo scafo (minuti); al colpo spruzzi laterali, nebbia e schiuma.
    void emersione_violenta(Vec2 pos, double rotta, double velocita, double angolo, double lunghezza, double fuori,
                            double ricaduta);
    // Missile lanciato da un sottomarino in immersione (E2, docs/10 §10.15; dossier 05 §1.2, §1.8, §2.2, §3, §6.1):
    // pos = punto di lancio, rotta del missile (gradi bussola), quota del lancio (m). `missile`: "harpoon" (capsula
    // che affiora, tappi, booster di 3 s), "tomahawk" (booster acceso sott'acqua, 12 s), "granit" (SS-N-19),
    // "malakhit" (SS-N-9), "ametist" (SS-N-7), "vodopad" (SS-N-15/16); altri nomi: come il Malakhit.
    // Ribollio e spruzzo dove esce, fiammata, scia di fumo bianco che resta e si allarga, corpo del missile che sale,
    // si livella o sparisce in quota. Restituisce i secondi dal lancio all'uscita dall'acqua.
    // `da_nave`: il lancio dal ponte di una nave (contenitori o rampe a `quota` metri sul mare): niente risalita, ne'
    // capsula ne' spruzzo; la fiammata, una nube di fumo sul ponte e la stessa traiettoria. Restituisce 0.
    double lancio_missile(Vec2 pos, double rotta, double quota, const std::string& missile, bool da_nave = false);
    // "Piuma" di un albero (periscopio o antenna) che sporge dall'acqua (E2): da chiamare a ogni passo per ogni
    // albero alzato. pos dell'albero, rotta e velocita' (m/s) del battello, altezza dell'albero sul mare (m). Spruzzo
    // davanti all'albero alto circa v^2/2g e scia di schiuma che si allunga con la velocita'. `id` tiene il conto
    // delle particelle frazionarie per albero.
    void piuma(int id, Vec2 pos, double rotta, double velocita, double altezza, double dt);

    // --- ASW aereo ---
    // Gli aerei ASW del passo (sostituisce l'elenco precedente): luci, anello di spruzzi, cavo, trasduttore, MAD
    void aerei_asw(const std::vector<AereoAsw>& aerei);
    // Le boe della missione al tempo di missione t (Sonoboa di nucleo/asw_aereo.hpp, coi loro tempi): espulsione,
    // discesa col paracadute, tonfo, colorante (boe USA), galleggiante con l'antenna fino a t_fine
    void boe_asw(const std::vector<Sonoboa>& boe, double t_missione);
    // Marcatore lanciato in pos, acceso per `durata_s`
    void marcatore_asw(MarcatoreAsw tipo, Vec2 pos, double durata_s);
    // Siluro aereo (Mk 46, AT-1M, UMGT-1) sganciato da quota_m alla velocita' dell'aereo (m/s): scende col paracadute,
    // tonfo e paracadute che resta sull'acqua. Restituisce i secondi fino all'impatto.
    double arma_paracadute(Vec2 pos, double quota_m, double rotta, double velocita, bool sovietica);

    // --- artiglieria navale (G4) ---
    // Un evento della missione (nucleo/acustica_artiglieria.hpp) al tempo di missione t_missione: si accoda e parte al
    // suo tempo_s. La posizione del tiratore (vampe, traccianti, linea di tiro dei rimbalzi) viene dal Mondo in avanza.
    // Il colpo in testa e il passaggio non hanno effetti nel mondo (l'oculare e l'audio li fanno da se').
    void artiglieria(const EventoArtiglieria& e, double t_missione);
    // Illuminante a paracadute aperto a quota_m sopra pos; la durata (~50 s) dal calibro (dossier 22 §6.3)
    void illuminante(Vec2 pos, double quota_m, double calibro_mm);
    // La luce in piu' sul mare in un punto per gli illuminanti accesi, 0-1 (da sommare alla luce della vista)
    double luce_illuminanti(Vec2 pos) const;
    // L'acqua delle colonne cadute vicino a un occhio (periscopio), 0-1: per ParametriOculare::spruzzi_proietti
    double acqua_sul_vetro(Vec2 occhio) const;
    // Le misure della colonna di un proietto per calibro (dossier 22 §6.1, STIMA): altezza e diametro (m), durata
    // visibile (s); carica_kg 0 = inerte (solo il getto)
    struct MisureColonna {
        double altezza_m = 0, diametro_m = 0, durata_s = 0;
    };
    static MisureColonna colonna_proietto(double calibro_mm, double carica_kg);

    // La nave `id` del mondo e' danneggiata: gli effetti seguono la sua fisica (i puntatori devono restare validi).
    // Con il modello della classe anche gli alberi possono cadere.
    void segui(int id, const DanniNave* danni, const Modello* modello = nullptr);
    // Aggiunge alla posa della nave le parti mosse dagli effetti (alberi caduti o in caduta).
    void decora(int id, PosaNave& posa) const;

    void avanza(double dt, const Mondo& mondo);

    double tempo() const { return tempo_; }
    double lampo() const { return lampo_; }   // 0-1: schiarisce la palette della vista (un fotogramma)
    const std::vector<Particella>& particelle() const { return particelle_; }
    // rifatte a ogni passo: schiuma sul galleggiamento, oggetti ASW appesi (cavi, luci, galleggianti, boe che scendono)
    const std::vector<Particella>& orli() const { return orli_; }
    std::size_t numero() const { return particelle_.size(); }

    // Disegna una particella gia' proiettata: centro (x, y) in pixel, `px_m` pixel per metro, `limite` prima riga
    // nascosta dal mare (oltre l'orizzonte), `quota_occhio` e `distanza` per lo schiacciamento delle chiazze.
    void disegna(Tela& t, const Particella& p, double x, double y, double px_m, int limite, double luce,
                 double quota_occhio, double distanza, double foschia = 0.0) const;

private:
    struct Albero {   // gruppo di parti del modello che cade insieme attorno alla base della parte piu' bassa
        std::vector<int> parti;
        double px = 0, py = 0, pz = 0, altezza = 1;
        double ax = 1, ay = 0, angolo = 0, omega = 0;
        bool cade = false, a_terra = false;
    };
    struct Seguita {
        int id = 0;
        const Modello* modello = nullptr;
        std::vector<Albero> alberi;
        const DanniNave* danni = nullptr;
        std::size_t scoppi = 0, corpi = 1;
        std::vector<bool> affondati;
        std::map<std::string, double> accumulo;      // emissioni frazionarie per sorgente
        std::map<std::string, double> acqua, fuoco;  // stato precedente delle celle (corpo:cella)
    };
    struct Emettitore {   // sorgente temporanea: ribollio d'aria, o il carico che brucia sulla chiazza (fuoco)
        double x = 0, y = 0, raggio = 10, t0 = 0, durata = 60, ritmo = 5, accumulo = 0;
        bool fango = false, fuoco = false;
    };

    struct MarcatoreAcceso {
        MarcatoreAsw tipo = MarcatoreAsw::mk25;
        double x = 0, y = 0, t0 = 0, durata = 600, accumulo = 0;
    };
    void segui_nave(Seguita& s, const Nave& n, double dt);
    void avanza_asw(double dt);
    void avanza_artiglieria(const Mondo& mondo);
    void emetti(const EventoArtiglieria& e, const Mondo& mondo);
    void colonna(double x, double y, double calibro_mm, double carica_kg, double ritardo, bool tinta);
    Particella& appeso(Materia m, double x, double y, double z, double r);   // particella di orli_ per un passo
    void affondamento(const Seguita& s, const Nave& n, const Corpo& c);
    void bolle(double x, double y, double raggio, int n, bool fango, double alto);
    void abbatti(Albero& a, double dx, double dy);
    void spruzzi(double x, double y, double z, int n, double alto, double largo);
    Particella& nuova(Materia m);

    Casuale caso_;
    double tempo_ = 0.0, lampo_ = 0.0;
    double vento_x_ = 0.0, vento_y_ = 0.0, vento_ = 7.0;
    std::vector<Particella> particelle_, orli_;
    std::vector<Seguita> seguite_;
    std::vector<Emettitore> emettitori_;
    std::map<int, double> piume_;   // particelle frazionarie della piuma per albero
    std::vector<AereoAsw> aerei_;
    std::map<int, double> cavo_prec_, spruzzi_acc_;   // per aereo: cavo del passo prima, spruzzi frazionari
    std::vector<Sonoboa> boe_;
    double t_boe_ = 0.0;
    std::set<int> boe_espulse_, boe_cadute_;
    std::vector<MarcatoreAcceso> marcatori_;
    // artiglieria: eventi in coda (al tempo degli effetti), impatti recenti (acqua sul vetro), illuminanti accesi
    struct EventoInCoda {
        EventoArtiglieria e;
        double t = 0;
    };
    struct Impatto {
        double x = 0, y = 0, t = 0, H = 0;
    };
    struct Illuminante {
        double x = 0, y = 0, z = 0, t0 = 0, durata = 50;
    };
    std::vector<EventoInCoda> coda_artiglieria_;
    std::vector<Impatto> impatti_;
    std::vector<Illuminante> illuminanti_;
};

}  // namespace m688
