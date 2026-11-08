#include "replay.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <functional>
#include <map>
#include <regex>
#include <set>

#include "carta.hpp"
#include "grafica/lamiera.hpp"
#include "grafica/quadro.hpp"
#include "grafica/simboli_carta.hpp"
#include "grafica/strati_meteo.hpp"
#include "grafica/tema_stazioni.hpp"
#include "missioni/missione_rapida.hpp"
#include "missioni/pacchetto_missione.hpp"
#include "missioni/scenario_missione.hpp"
#include "missioni/griglia_ghiaccio.hpp"
#include "nucleo/campo_ghiaccio.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/geometria.hpp"
#include "parti_credute.hpp"
#include "segni_ambiente.hpp"
#include "trascina_carta.hpp"

namespace m688 {

namespace {

// la disposizione della tela 640x480
constexpr int CARTA_W = 468;                             // la prima scala: trenta miglia attorno al battello su tanti pixel
constexpr int PIEDE_Y = 428;                             // la variante C: il piede comune (tasti a 450-476)
constexpr int COLONNA_W = 168;                           // dal vivo: lo schermo del registro a destra (bozza vivo_C del kit)
constexpr int SINISTRA_W = 190;                          // dal vivo: il testo a sinistra nella fascia, perche' la barra stia ferma
constexpr double SCIA_S = 300.0, PROFILO_S = 600.0;
constexpr double VELOCITA_MAX = 64.0;

void linea(Tela& t, Vec2 a, Vec2 b, std::uint8_t c, int tratto = 0) {
    const int n = static_cast<int>(std::max(std::fabs(b.x - a.x), std::fabs(b.y - a.y)));
    for (int i = 0; i <= n; ++i) {
        if (tratto > 0 && (i / tratto) % 2) continue;
        const double s = n ? static_cast<double>(i) / n : 0.0;
        t.pixel(static_cast<int>(std::lround(a.x + (b.x - a.x) * s)), static_cast<int>(std::lround(a.y + (b.y - a.y) * s)), c);
    }
}

Vec2 avanti(Vec2 p, double rotta, double d) { return {p.x + d * std::sin(radianti(rotta)), p.y + d * std::cos(radianti(rotta))}; }

std::string orologio(double s) {
    const int x = static_cast<int>(std::max(0.0, s));
    char b[16];
    std::snprintf(b, sizeof b, "%d:%02d:%02d", x / 3600, x / 60 % 60, x % 60);
    return b;
}

std::string maiu(std::string s) {
    for (auto& c : s) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return s;
}

// il nome della classe per la carta: maiuscolo, senza i trattini bassi delle chiavi (los_angeles)
std::string nome_classe(std::string s) {
    for (auto& c : s) c = c == '_' ? ' ' : static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return s;
}

std::string taglia(const Font& f, std::string s, int w) {
    while (!s.empty() && f.larghezza(s, 1) > w) s.pop_back();
    return s;
}

// i testi sul fosforo (font a celle variabili, formato 2), come grafica/quadro
int largo_q(const Font& f, std::string_view s) { return f.larghezza(s, 1, f.formato() == 2 ? f.spazio_predefinito() : 1, f.formato() != 2); }
void scrivi_q(Tela& t, const Font& f, int x, int y, std::string_view s, std::uint8_t c) {
    f.scrivi(t, x, y, s, c, 1, f.formato() == 2 ? f.spazio_predefinito() : 1, f.formato() != 2);
}
std::string taglia_q(const Font& f, std::string s, int w) {
    while (!s.empty() && largo_q(f, s) > w) s.pop_back();
    return s;
}

bool arma(const StatoUnita& u) {
    return u.categoria != "sottomarino" && u.categoria != "nave" && u.categoria != "aereo";
}

// il falso contatto della D3 (docs/28 §3.5): per il motore un'«arma» senza lanciatore nella fascia d'id dei falsi
// (missione_interventi.cpp, 990000-994999), con la classe imitata e la parte creduta nel flag
constexpr int FALSI_DA = 990000, FALSI_FINO = 995000;
bool falso(const StatoUnita& u) { return u.categoria == "arma" && u.lanciatore < 0 && u.id >= FALSI_DA && u.id < FALSI_FINO; }
// il siluro «di nessuno» dell'incidente provocato (D3): un siluro in acqua senza lanciatore, fuori dalla fascia dei falsi
bool di_nessuno(const StatoUnita& u) {
    return u.categoria == "arma" && u.lanciatore < 0 && !falso(u) && simboli::oggetto_da_classe(u.categoria, u.classe) == simboli::Oggetto::siluro;
}

// il colore del segno di un evento sulla timeline
std::uint8_t colore_evento(const std::string& tipo) {
    const auto uno_di = [&](std::initializer_list<const char*> tipi) {
        return std::any_of(tipi.begin(), tipi.end(), [&](const char* x) { return tipo == x; });
    };
    // le interazioni fra unita' dell'IA (docs/18, fase 5) coi colori degli eventi del giocatore. Rosso cio' che colpisce,
    // fa danno o e' un'arma che parte
    if (uno_di({"lancio_giocatore", "lancio_nemico", "lancio_ia", "colpo", "affondamento", "artiglieria_ia", "collisione_ia", "danno_subito",
                "scram", "camera_di_lancio_persa", "infrazione_roe", "incaglio", "fuoco_amico", "mina_posata", "mina_esplosa",
                "artiglieria_salva", "artiglieria_colpo", "collisione", "cavo_impigliato", "avaria_ia", "colpo_esercitazione", "provocazione",
                "fuoco_amico_ia", "esito_tlam"}))
        return colori::ROSSO;
    if (uno_di({"scoperta", "classificazione", "scoperta_ia", "ingaggio_ia", "mina_scoperta"})) return colori::CIANO_CHIARO;
    // giallo il pericolo, scampato o in corso
    if (uno_di({"avvistato", "fix_radiogoniometria", "evasione_ia", "emersione_emergenza", "siluro_evitato", "esca_sedotta", "danno_domato",
                "posti_di_combattimento", "ingresso_zona", "esm_rischio_alto", "ping_subito", "boa_vicina", "aereo_in_sorvolo",
                "ping_sonar_immersione", "marcatore_avvistato", "attacco_simulato", "esposizione_lunga", "artiglieria_vicino", "quasi_collisione",
                "emersione_forzata", "cavo_toccato", "acque_territoriali", "esca_ia", "ritirata_ia", "incsea", "allarme_siluro",
                "attraversamento_mine"}))
        return colori::GIALLO;
    if (tipo == "fine" || tipo == "inizio") return colori::BIANCO;
    // il resto non colpisce e non minaccia: radio, routine dell'IA, misure. L'intervento ha gia' il suo triangolo sopra la barra
    return colori::GRIGIO;
}

// --- il vetrino di Dio (docs/28 §6): gli interventi sulla linea del tempo e sulla carta ---
constexpr double TOLTA_S = 120.0;          // la croce di un'unita' tolta resta due minuti

// la famiglia di un tipo (il gruppo del catalogo) e il suo colore (grafica/simboli_carta del kit)
simboli::FamigliaIntervento famiglia_intervento(const std::string& tipo) {
    const VoceCatalogoIntervento* v = voce_intervento(tipo);
    return simboli::famiglia_da_gruppo(v ? v->gruppo : std::string("FORZE"));
}

// Il profilo del suono nella scheda dello spettatore: la velocita' verso destra, la profondita' verso il basso fino a scala_m; quello
// di adesso in ciano e, se un intervento l'ha cambiato, quello del mese piu' scuro; lo strato a tratti in giallo; a sinistra una
// tacca alla quota dell'unita' (sotto 0: nessuna)
void curva_suono(Tela& t, Rettangolo r, const ProfiloSuono& ora, const ProfiloSuono& base, double scala_m, double quota_m, std::uint8_t col_quota) {
    if (r.w < 6 || r.h < 6) return;
    double c0 = 1.0e9, c1 = -1.0e9;
    const auto z_di = [&](int y) { return scala_m * y / (r.h - 1); };
    for (int y = 0; y < r.h; ++y)
        for (const ProfiloSuono* p : {&ora, &base}) c0 = std::min(c0, p->c(z_di(y))), c1 = std::max(c1, p->c(z_di(y)));
    if (c1 - c0 < 2.0) c0 -= 1.0, c1 += 1.0;
    const auto curva = [&](const ProfiloSuono& p, std::uint8_t col) {
        std::optional<Vec2> prima;
        for (int y = 0; y < r.h; ++y) {
            const Vec2 q{r.x + 3 + (p.c(z_di(y)) - c0) / (c1 - c0) * (r.w - 4), static_cast<double>(r.y + y)};
            if (prima) linea(t, *prima, q, col);
            prima = q;
        }
    };
    for (int x = r.x; x < r.x + r.w; x += 3) t.pixel(x, r.y, static_cast<std::uint8_t>(colori::GRIGIO_SCURO));   // la superficie
    if (ora.z_m != base.z_m || ora.c_m_s != base.c_m_s) curva(base, static_cast<std::uint8_t>(colori::GRIGIO));
    curva(ora, static_cast<std::uint8_t>(colori::CIANO_CHIARO));
    if (ora.sld_m > 0.0 && ora.sld_m < scala_m) {
        const int y = r.y + static_cast<int>(std::lround(ora.sld_m / scala_m * (r.h - 1)));
        for (int x = r.x; x < r.x + r.w; x += 4) t.linea_orizzontale(x, x + 1, y, static_cast<std::uint8_t>(colori::GIALLO));
    }
    if (quota_m >= 0.0) {
        const int y = r.y + static_cast<int>(std::lround(std::min(quota_m, scala_m) / scala_m * (r.h - 1)));
        t.linea_orizzontale(r.x, r.x + 3, y, col_quota);
    }
}

// La polynya sulla carta dello spettatore (polynye_a), sopra il fondo. Che si apre: l'acqua libera a puntini fino al raggio a
// cui e' arrivata, col suo bordo, solo dove c'era il pack (`nel_pack`; vuota, nelle tracce senza griglia: ovunque). Che si chiude: il tratteggio obliquo del ghiaccio della carta (grafica/strati_meteo), rado
// mentre e' nilas, fitto a ghiaccio fatto
void disegna_polynya(Tela& t, const GeometriaCarta& g, const PolynyaVista& z, double mpp, const std::function<bool(Vec2)>& nel_pack) {
    const Vec2 c = g.a_tela(z.centro);
    const double rr = (z.apri ? z.raggio_m * z.fase : z.raggio_m) / mpp;
    const int passo = z.fase >= 1.0 ? 6 : static_cast<int>(std::lround(12.0 - 4.0 * z.fase));
    const int x0 = std::max(g.x, static_cast<int>(std::floor(c.x - rr))), x1 = std::min(g.x + g.w - 1, static_cast<int>(std::ceil(c.x + rr)));
    const int y0 = std::max(g.y, static_cast<int>(std::floor(c.y - rr))), y1 = std::min(g.y + g.h - 1, static_cast<int>(std::ceil(c.y + rr)));
    for (int py = y0; py <= y1; ++py)
        for (int px = x0; px <= x1; ++px) {
            if (std::hypot(px - c.x, py - c.y) > rr) continue;
            if (z.apri && nel_pack && px % 4 == 0 && py % 4 == 0 && !nel_pack(g.a_mondo({static_cast<double>(px), static_cast<double>(py)}))) continue;
            if (z.apri ? px % 4 == 0 && py % 4 == 0 : (px + py) % passo == 0)
                t.pixel(px, py, static_cast<std::uint8_t>(z.apri ? colori::CIANO_CHIARO : colori::BIANCO));
        }
    if (!z.apri) return;
    const std::uint8_t col = colore_intervento("polynya");
    for (int k = 0; k < 48; ++k) {
        const double a0 = k * 2.0 * PI / 48.0, a1 = (k + 1) * 2.0 * PI / 48.0, am = (a0 + a1) / 2.0;
        if (nel_pack && !nel_pack(g.a_mondo({c.x + rr * std::cos(am), c.y + rr * std::sin(am)}))) continue;
        linea(t, {c.x + rr * std::cos(a0), c.y + rr * std::sin(a0)}, {c.x + rr * std::cos(a1), c.y + rr * std::sin(a1)}, col);
    }
}

// gli interventi all'istante t dai record X: per ogni id l'ultimo record fino a t, se e' l'applicazione
std::vector<const RecordIntervento*> interventi_a(const Traccia& tr, double t) {
    std::map<int, const RecordIntervento*> ultimo;
    for (const RecordIntervento& x : tr.interventi)
        if (x.t <= t + 1e-6) ultimo[x.id] = &x;
    std::vector<const RecordIntervento*> v;
    for (const auto& [id, x] : ultimo)
        if (x->fase == RecordIntervento::FASE_APPLICATO) v.push_back(x);
    return v;
}

// un triangolo in giu' di 5 pixel col vertice in (x, y): il segno di un intervento sopra la linea del tempo
void triangolo(Tela& t, int x, int y, std::uint8_t c, bool pieno = true) {
    for (int k = 0; k < 3; ++k) {
        if (pieno || k == 2) t.linea_orizzontale(x - k, x + k, y - k, c);
        else t.pixel(x - k, y - k, c), t.pixel(x + k, y - k, c);
    }
}

// il gruppo del filtro del registro dal vivo: 1 scoperte, 2 armi, 3 manovre, 4 allerta e radio, 5 comandante (0 altro)
int gruppo_di(const std::string& tipo) {
    const auto ha = [&](const char* x) { return tipo.find(x) != std::string::npos; };
    if (ha("comandante")) return 5;
    if (ha("lancio") || ha("colpo") || ha("affondamento") || ha("siluro") || ha("esca") || ha("artiglieria") || ha("mina")) return 2;
    if (ha("scoperta") || ha("classificazione") || ha("contatto") || ha("avvistato") || ha("fix_")) return 1;
    if (ha("evasione") || ha("ritirata") || ha("ingaggio") || ha("reazione") || ha("tma") || ha("collisione") || ha("incaglio") ||
        ha("pedina"))
        return 3;
    if (ha("allerta") || ha("messaggio") || ha("radio") || ha("ping") || ha("incsea") || ha("punto_nave")) return 4;
    return 0;
}

const char* const NOMI_PARTE[] = {"TUTTE", "NATO", "PATTO", "NEUTRALI"};
const char* const NOMI_TIPO[] = {"TUTTI", "SCOPERTE", "ARMI", "MANOVRE", "ALLERTA E RADIO", "COMANDANTE"};
constexpr double STIMA_MAX_S = 6.0;   // dal vivo: per quanto un'unita' prosegue a stima dall'ultimo campione

}  // namespace

std::vector<quadro::TaccaTempo> tacche_eventi(const std::vector<EventoTraccia>& segni, double fine, bool solo_noti) {
    std::vector<quadro::TaccaTempo> out;
    for (const auto& e : segni)
        if (!solo_noti || e.noto) out.push_back({e.tempo / fine, colore_evento(e.tipo), e.interesse});
    return out;
}

std::uint8_t colore_intervento(const std::string& tipo) { return simboli::indice_famiglia(famiglia_intervento(tipo)); }

std::string teatro_dal_file(const std::string& dati, const std::string& missione, std::vector<double>& termoclini) {
    if (missione.empty()) return {};
    const std::filesystem::path missioni = std::filesystem::path(dati) / ".." / "missioni";
    std::vector<std::filesystem::path> cartelle = {missioni, missioni / "libreria"};
    for (const auto& f : {&cartella_missioni_rapide, &cartella_missioni_editor, &cartella_missioni_importate}) {
        try {
            cartelle.push_back(f());
        } catch (const std::exception&) {
        }
    }
    static const std::regex teatro("teatro\\s*=\\s*\"([a-z_]+)\""), strati("termoclini_m\\s*=\\s*\\{([^}]*)\\}");
    for (const auto& c : cartelle) {
        std::ifstream in(c / (missione + ".lua"));
        if (!in) continue;
        std::string trovato;
        for (std::string riga; std::getline(in, riga);) {
            std::smatch m;
            if (trovato.empty() && std::regex_search(riga, m, teatro)) trovato = m[1];
            if (termoclini.empty() && std::regex_search(riga, m, strati)) {
                const std::string v = m[1];
                for (std::size_t i = 0; i < v.size();) {
                    std::size_t letti = 0;
                    try {
                        termoclini.push_back(std::stod(v.substr(i), &letti));
                    } catch (const std::exception&) {
                        break;
                    }
                    i += letti;
                    while (i < v.size() && (v[i] == ',' || v[i] == ' ')) ++i;
                }
            }
            if (!trovato.empty() && !termoclini.empty()) break;
        }
        if (!trovato.empty()) return trovato;
    }
    return {};
}

SchermataReplay::SchermataReplay(std::string dati, Traccia traccia, std::string teatro)
    : dati_(std::move(dati)), teatro_(std::move(teatro)), tr_(std::move(traccia)) {
    if (teatro_.empty())
        if (const auto it = tr_.metadati.find("teatro"); it != tr_.metadati.end()) teatro_ = it->second;
    {
        const std::string dal_file = teatro_dal_file(dati_, tr_.missione, termoclini_);
        if (teatro_.empty()) teatro_ = dal_file;
    }
    if (!teatro_.empty()) {
        try {
            fondale_ = std::make_unique<Fondale>(Fondale::carica(dati_ + "/teatri/" + teatro_ + ".fdl"));
        } catch (const std::exception&) {
            fondale_.reset();
        }
    }
    if (const auto it = tr_.metadati.find("giocatore"); it != tr_.metadati.end()) giocatore_id_ = std::atoi(it->second.c_str());
    if (const auto it = tr_.metadati.find(METADATO_GRIGLIA_GHIACCIO); it != tr_.metadati.end()) griglia_ghiaccio(it->second);
    else if (tr_.metadati.count(METADATO_MESE_ACUSTICA)) griglia_ghiaccio(std::string());   // il vetrino di oggi senza griglia: niente ghiaccio
    if (const auto it = tr_.metadati.find(METADATO_MESE_ACUSTICA); it != tr_.metadati.end()) mese_acustica(std::atoi(it->second.c_str()));
    try {
        targa_ = Font::carica(dati_ + "/font/incisione688_7.fnt");
        fosforo_ = Font::carica(dati_ + "/font/fosforo688_5x7.fnt");
    } catch (const std::exception&) {
        targa_.reset();
        fosforo_.reset();
    }
    segni_ = segnalibri(tr_, 0, false);
    aggiorna_registro();
    // la prima vista: il battello con trenta miglia attorno
    mpp_ = 30.0 * MIGLIO * 2.0 / CARTA_W;
    segui();
}

void SchermataReplay::griglia_ghiaccio(const std::string& griglia) {
    campo_.reset();
    griglia_.reset();
    if (auto g = leggi_griglia(griglia)) griglia_ = std::make_shared<const GrigliaGhiaccio>(std::move(*g));
    else if (griglia.empty()) {   // il teatro non ha ghiaccio: si sa, e una polynya che si apre non ha niente da aprire
        GrigliaGhiaccio vuota;
        vuota.nx = vuota.ny = 1;
        vuota.celle = {0};
        griglia_ = std::make_shared<const GrigliaGhiaccio>(std::move(vuota));
    }
}

void SchermataReplay::mese_acustica(int mese) {
    suono_base_.reset();
    if (mese < 1 || mese > 12 || teatro_.empty()) return;
    try {
        suono_base_ = std::make_shared<const ProfiloSuono>(carica_clima(dati_ + "/teatri/" + teatro_ + ".suono.toml").profilo(mese));
    } catch (const std::exception&) {   // senza il clima del teatro niente profilo: il replay si apre lo stesso
    }
}

const CampoGhiaccio* SchermataReplay::campo_ghiaccio() const {
    if (!griglia_) return nullptr;
    if (!campo_ || campo_record_ != tr_.interventi.size()) {   // dal vivo i record crescono
        campo_ = std::make_shared<const CampoGhiaccio>(campo_spettatore(griglia_, tr_.interventi));
        campo_record_ = tr_.interventi.size();
    }
    return campo_.get();
}

void SchermataReplay::vai(double t) {
    t_ = std::clamp(t, 0.0, std::max(0.0, tr_.durata));
    if (segue_) segui();
}

const StatoUnita* SchermataReplay::giocatore(const IstanteTraccia& i) const {
    for (const auto& u : i.unita)
        if (u.id == giocatore_id_) return &u;
    return nullptr;
}

void SchermataReplay::segui() {
    segue_ = true;
    const IstanteTraccia i = istante_vista();
    if (scelta_ >= 0) {
        for (const auto& u : i.unita)
            if (u.id == scelta_) {
                centro_ = u.pos;
                return;
            }
    }
    if (const auto* g = giocatore(i)) centro_ = g->pos;
}

int SchermataReplay::lato(const StatoUnita& u) const { return lato_flag(u.flag); }

int SchermataReplay::lato_flag(int flag) const {
    int mio = 0x08;   // il 688 se il battello non c'e' (gia' uscito)
    if (const auto it = tr_.unita.find(giocatore_id_); it != tr_.unita.end() && !it->second.empty()) mio = it->second.front().second.flag & 0x18;
    const int suo = flag & 0x18;
    if (suo == 0) return 3;
    return suo == mio ? 0 : 1;
}

Rettangolo SchermataReplay::carta() const { return carta_c_; }
Rettangolo SchermataReplay::linea_tempo() const { return barra_; }

void SchermataReplay::disegna_carta_in(Tela& t, const Font& f, Rettangolo r) const {
    carta_c_ = r;   // carta() fuori dal vivo: quella dell'ultimo disegno
    disegna_carta(t, f, istante_vista());
}

GeometriaCarta SchermataReplay::geometria() const {
    const Rettangolo c = carta();
    GeometriaCarta g;
    g.x = c.x;
    g.y = c.y;
    g.w = c.w;
    g.h = c.h;
    g.centro = centro_;
    g.metri_per_pixel = mpp_;
    return g;
}

double SchermataReplay::fine_linea() const {   // dal vivo anche gli interventi programmati, oltre adesso
    if (!vivo_) return tr_.durata;
    double fine = std::max(tr_.durata, adesso_);
    for (const auto& q : richieste_)
        if (!q.subito) fine = std::max(fine, q.tempo + 60.0);
    return fine;
}

double SchermataReplay::x_di(double t) const {
    const double fine = fine_linea();
    const Rettangolo b = linea_tempo();
    return b.x + (fine > 0.0 ? t / fine : 0.0) * b.w;
}
double SchermataReplay::t_di(double x) const {
    const Rettangolo b = linea_tempo();
    return std::clamp((x - b.x) / std::max(1, b.w - 1), 0.0, 1.0) * fine_linea();
}

// --- dal vivo ---

void SchermataReplay::dal_vivo(bool si) {
    vivo_ = si;
    indietro_ = false;
    pausa_ = false;
    if (si) lettura_ = Lettura::verita;
    if (si && fosforo_) {   // i rettangoli della carta, del registro e della barra prima del primo disegno
        Tela prova;
        cornice_vivo(prova, *fosforo_);
        disegna_timeline(prova, *fosforo_);
    }
}

void SchermataReplay::aggiungi(const Fotogramma& f) {
    const bool prima = tr_.unita.empty();
    const std::size_t eventi_prima = tr_.registro.size();
    aggiungi_fotogramma(tr_, f);
    if (tr_.registro.size() != eventi_prima) segni_ = segnalibri(tr_, 0, false);
    aggiorna_registro();
    if (prima && segue_) segui();   // il primo fotogramma: la carta va sul battello
}

void SchermataReplay::adesso(double t, double compressione, bool pausa) {
    adesso_ = std::max(adesso_, t);
    velocita_ = compressione;
    pausa_ = pausa;
    if (!indietro_) {
        t_ = adesso_;
        if (segue_) segui();
    }
}

std::string SchermataReplay::nome_unita(int id) const {
    const auto it = tr_.unita.find(id);
    if (it == tr_.unita.end() || it->second.empty()) return id >= 0 ? "#" + std::to_string(id) : std::string("?");
    return nome_classe(it->second.front().second.classe);
}

int SchermataReplay::flag_unita(int id) const {
    const auto it = tr_.unita.find(id);
    return it == tr_.unita.end() || it->second.empty() ? -1 : it->second.front().second.flag;
}

std::string SchermataReplay::categoria_imitata(const std::string& classe) const {
    if (!sottomarini_) {   // come il runtime: una classe con la vela e' un sottomarino
        sottomarini_.emplace();
        try {
            for (const auto& [k, c] : carica_classi(dati_ + "/classi.toml"))
                if (c.altezza_vela_m > 0.1) sottomarini_->push_back(k);
        } catch (const std::exception&) {
        }
    }
    return std::find(sottomarini_->begin(), sottomarini_->end(), classe) != sottomarini_->end() ? "sottomarino" : "nave";
}

void SchermataReplay::aggiorna_registro() {
    const double ultimo = righe_.empty() ? -1e18 : righe_.back().t;
    bool fuori_ordine = false;
    const auto metti = [&](RigaRegistro r) {
        // lo stesso fatto della stessa unita' entro un minuto dall'ultimo uguale: si conta sulla riga di prima
        for (auto it = righe_.rbegin(); it != righe_.rend() && r.t - it->t < 60.0; ++it)
            if (it->testo == r.testo && it->unita == r.unita && !r.dove) {
                ++it->volte;
                it->t = std::max(it->t, r.t);   // la riga va all'ultima volta: il registro la riordina
                fuori_ordine = fuori_ordine || it != righe_.rbegin();
                return;
            }
        fuori_ordine = fuori_ordine || r.t < ultimo;
        righe_.push_back(std::move(r));
    };
    for (; registro_visti_ < tr_.registro.size(); ++registro_visti_) {
        const EventoTraccia& e = tr_.registro[registro_visti_];
        RigaRegistro r;
        r.t = e.tempo;
        r.testo = maiu(e.testo_it.empty() ? e.tipo : e.testo_it);
        r.colore = colore_evento(e.tipo);
        r.unita = e.unita;
        r.altra = e.altra;
        r.gruppo = gruppo_di(e.tipo);
        metti(std::move(r));
    }
    // dai record I: le tracce solide, i contatti persi e i momenti dei siluri (il resto e' gia' nel registro)
    for (; ia_visti_ < tr_.eventi_ia.size(); ++ia_visti_) {
        const EventoIa& e = tr_.eventi_ia[ia_visti_];
        RigaRegistro r;
        r.t = e.t;
        r.unita = e.unita;
        r.altra = e.altra;
        char km[24];
        std::snprintf(km, sizeof km, "%.1f KM", e.a / 1000.0);
        if (e.tipo == "contatto_solido") {
            r.testo = nome_unita(e.unita) + ": TRACCIA SOLIDA SU " + nome_unita(e.altra) + " (" + maiu(e.s) + ", " + km + ")";
            r.colore = colori::CIANO_CHIARO;
            r.gruppo = 1;
        } else if (e.tipo == "contatto_perso") {
            r.testo = nome_unita(e.unita) + " PERDE " + nome_unita(e.altra);
            r.colore = colori::GRIGIO;
            r.gruppo = 1;
        } else if (e.tipo == "siluro" && (e.s == "aggancio" || e.s == "esca_sedotta" || e.s == "esplosione" || e.s == "perdita")) {
            const std::string chi = "SILURO DI " + nome_unita(e.altra) + ": ";
            r.testo = chi + (e.s == "aggancio" ? "AGGANCIA" : e.s == "esca_sedotta" ? "SEDOTTO DA UN'ESCA" : e.s == "esplosione" ? "ESPLODE" : "PERDE IL BERSAGLIO");
            r.colore = e.s == "esplosione" ? colori::ROSSO : colori::GIALLO;
            r.gruppo = 2;
            r.dove = Vec2{e.a, e.b};
        } else {
            continue;
        }
        metti(std::move(r));
    }
    if (fuori_ordine)
        std::stable_sort(righe_.begin(), righe_.end(), [](const RigaRegistro& a, const RigaRegistro& b) { return a.t < b.t; });
}

bool SchermataReplay::passa(const RigaRegistro& r) const {
    if (filtro_tipo_ > 0 && r.gruppo != filtro_tipo_) return false;
    if (vivo_ && lettura_ == Lettura::unita) {   // la lettura dell'unita': solo i fatti suoi
        const int quale = scelta_ >= 0 ? scelta_ : giocatore_id_;
        if (r.unita != quale && r.altra != quale) return false;
    }
    if (filtro_parte_ > 0) {
        const int f = flag_unita(r.unita);
        const int lato = f < 0 ? -1 : (f & 0x18) == 0x08 ? 1 : (f & 0x18) ? 2 : 3;
        if (lato != filtro_parte_) return false;
    }
    return true;
}

std::size_t SchermataReplay::righe_registro() const {
    return static_cast<std::size_t>(std::count_if(righe_.begin(), righe_.end(), [&](const RigaRegistro& r) {
        return r.t <= t_ + 0.5 && passa(r);
    }));
}

IstanteTraccia SchermataReplay::istante_vista() const {
    IstanteTraccia i = tr_.istante(t_);
    if (!vivo_ || indietro_) return i;
    for (auto& u : i.unita) {   // oltre l'ultimo campione: a stima con la rotta e la velocita' di allora
        const auto it = tr_.unita.find(u.id);
        if (it == tr_.unita.end() || it->second.empty()) continue;
        const auto& [tc, ultimo] = it->second.back();
        if (tc >= t_ || ultimo.distrutta || ultimo.sul_fondo) continue;
        const double dt = std::min(t_ - tc, STIMA_MAX_S);
        u.pos = avanti(ultimo.pos, ultimo.rotta, ultimo.velocita_kn * NODO * dt);
    }
    return i;
}

void SchermataReplay::apri_pannello() {
    if (!pannello_) {   // le classi che si possono inserire: navi e sottomarini (col pescaggio), per nome
        std::vector<std::pair<std::string, std::string>> classi;
        try {
            for (const auto& [k, c] : carica_classi(dati_ + "/classi.toml"))
                if (c.pescaggio_m > 0.0) classi.emplace_back(k, c.nome.empty() ? k : c.nome);
        } catch (const std::exception&) {
        }
        std::sort(classi.begin(), classi.end(), [](const auto& a, const auto& b) { return a.second < b.second; });
        pannello_.emplace(catalogo_pannello(catalogo_interventi(), classi));
    }
    std::vector<UnitaPannello> v;
    for (const auto& u : istante_vista().unita) {
        if (arma(u) || u.distrutta || u.persa || u.rimossa) continue;
        const int parte = u.flag & 0x18;
        v.push_back({u.id, nome_unita(u.id) + " (" + std::to_string(u.id) + ")", parte == 0 ? "NEUTRALI" : parte == 0x08 ? "NATO" : "PATTO",
                     u.categoria});
    }
    std::sort(v.begin(), v.end(), [](const UnitaPannello& a, const UnitaPannello& b) { return a.id < b.id; });
    pannello_->apri(adesso_, std::move(v));
}

void SchermataReplay::disegna_avviso(Tela& t, const Font& f) const {   // in alto al centro della carta, su un riquadro
    if (avviso_resta_ <= 0.0 || avviso_.empty()) return;
    const Rettangolo c = carta();
    const std::string testo = taglia(f, maiu(avviso_), c.w - 24);
    const int w = f.larghezza(testo, 1) + 12, h = f.righe() + 8, x = c.x + (c.w - w) / 2, y = c.y + 8;
    t.rettangolo_pieno(x, y, w, h, colori::ROSSO);
    t.rettangolo_pieno(x + 1, y + 1, w - 2, h - 2, colori::NERO);
    f.scrivi(t, x + 6, y + 4, testo, colori::GIALLO, 1);
}

RichiesteVivo SchermataReplay::comandi_vivo(const Comandi& c, double dt) {
    avviso_resta_ = std::max(0.0, avviso_resta_ - dt);
    RichiesteVivo r;
    if (c.esci) {
        r.fine = true;
        return r;
    }
    if (pannello_aperto()) {   // il pannello prende tastiera e clic; il tempo resta allo spettatore
        r.pausa = c.pausa;
        r.compressione = c.compressione;
        for (const auto& [x, y] : c.clic)
            if (pannello_->aspetta_punto() && x >= carta_c_.x && x < carta_c_.x + carta_c_.w && y >= carta_c_.y && y < carta_c_.y + carta_c_.h)
                pannello_->punto_scelto(geometria().a_mondo({static_cast<double>(x), static_cast<double>(y)}));
        if (auto q = pannello_->comandi(c, dt)) {
            richieste_.push_back(*q);
            r.interventi.push_back(intervento_di(*q));
        }
        return r;
    }
    if (c.esc) {
        r.fine = true;
        return r;
    }
    if (c.cancella && !richieste_.empty()) {   // annulla l'ultimo
        richieste_.pop_back();
        r.annulla_ultimo = true;
    }
    const auto torna = [&] {
        indietro_ = false;
        t_ = adesso_;
        if (segue_) segui();
    };
    const auto vai_indietro = [&](double t) {
        if (t >= adesso_ - 0.5) {
            torna();
            return;
        }
        indietro_ = true;
        t_ = std::clamp(t, 0.0, adesso_);
        if (segue_) segui();
    };
    if (c.invio) torna();
    r.pausa = c.pausa;
    r.compressione = c.compressione;
    for (const Azione a : c.azioni) r.compressione += a == Azione::telegrafo_avanti ? 1 : a == Azione::telegrafo_indietro ? -1 : 0;
    if (c.verita) {   // V: verita' -> parte NATO -> parte Patto -> unita'; il filtro del registro segue la parte
        lettura_ = lettura_ == Lettura::verita ? Lettura::parte_nato : lettura_ == Lettura::parte_nato ? Lettura::parte_patto
                 : lettura_ == Lettura::parte_patto ? Lettura::unita : Lettura::verita;
        filtro_parte_ = lettura_ == Lettura::parte_nato ? 1 : lettura_ == Lettura::parte_patto ? 2 : 0;
    }
    if (c.ingrandisci) mpp_ = std::max(10.0, mpp_ / 1.5);
    if (c.riduci) mpp_ = std::min(2000.0, mpp_ * 1.5);
    rotella_carta(c, carta_c_, centro_, mpp_);
    // Tab: l'unita' successiva (Maiuscolo+Tab la precedente) fra quelle in gioco, armi escluse
    if (c.tab != 0) {
        std::vector<int> ids;
        for (const auto& u : tr_.istante(t_).unita)
            if (!arma(u)) ids.push_back(u.id);
        std::sort(ids.begin(), ids.end());
        if (!ids.empty()) {
            auto it = std::find(ids.begin(), ids.end(), scelta_);
            if (it == ids.end()) scelta_ = c.tab > 0 ? ids.front() : ids.back();
            else if (c.tab > 0) scelta_ = std::next(it) == ids.end() ? ids.front() : *std::next(it);
            else scelta_ = it == ids.begin() ? ids.back() : *std::prev(it);
        }
    }
    for (const char l : c.lettere) {
        if (l == 'G' || l == 'g') apri_pannello();
        if ((l == 'S' || l == 's') && scelta_ >= 0) segui();
        if (l == 'C' || l == 'c') confronto_ = !confronto_;   // la parte: la verita' degli altri in grigio sotto
    }
    // le frecce: l'evento precedente o successivo del registro filtrato (uno subito, poi uno ogni 0,2 s)
    salto_attesa_ -= dt;
    const int verso = verso_freccia(c.ruota, c.passi_ruota);
    if (verso != salto_verso_) salto_attesa_ = 0.0;
    salto_verso_ = verso;
    if (verso != 0 && salto_attesa_ <= 0.0) {
        salto_attesa_ = 0.2;
        const double ora = t_;
        std::optional<double> meta;
        if (verso > 0) {
            for (const auto& x : righe_)
                if (x.t > ora + 0.5 && x.t <= adesso_ && passa(x)) { meta = x.t; break; }
            if (meta) vai_indietro(*meta);
            else torna();   // dopo l'ultimo evento: di nuovo al vivo
        } else {
            for (auto it = righe_.rbegin(); it != righe_.rend(); ++it)
                if (it->t < ora - 0.5 && passa(*it)) { meta = it->t; break; }
            if (meta) vai_indietro(*meta);
        }
    }
    const Rettangolo b = barra_, rg = registro_c_, fa = fascia_c_;
    for (const auto& [x, y] : c.clic) {
        const bool nel_registro = x >= rg.x && x < rg.x + rg.w;
        if (y >= fa.y && y < fa.y + fa.h && x >= b.x - 4 && x <= b.x + b.w + 4) {
            vai_indietro(t_di(x));
        } else if (y >= fa.y && y < fa.y + fa.h && x > b.x + b.w + 4 && x < fa.x + fa.w) {
            torna();   // ADESSO, a destra nella fascia
        } else if (y >= PIEDE_Y + 22 && y < PIEDE_Y + 48) {   // i tre tasti del piede, larghi 202 da x 8, 218, 428
            if (x < 214) r.fine = true;
            else if (x < 424) confronto_ = !confronto_;
            else torna();
        } else if (nel_registro && y >= rg.y && y < rg.y + 12) {   // la riga dei filtri: a sinistra la parte, a destra il tipo
            if (x < rg.x + rg.w / 2) filtro_parte_ = (filtro_parte_ + 1) % 4;
            else filtro_tipo_ = (filtro_tipo_ + 1) % 6;
        } else if (nel_registro && y >= scheda_y_ && y < scheda_y_ + 10) {
            segui();   // il titolo della scheda: segui l'unita' scelta (o il battello)
        } else {
            for (const auto& z : zone_righe_) {
                if (x < z.r.x || x >= z.r.x + z.r.w || y < z.r.y || y >= z.r.y + z.r.h || z.riga >= righe_.size()) continue;
                const RigaRegistro& riga = righe_[z.riga];
                if (c.veloce) vai_indietro(riga.t);   // Maiuscolo: anche il tempo va li'
                std::optional<Vec2> p = riga.dove;
                if (!p) {
                    for (const auto& u : tr_.istante(riga.t).unita)
                        if (u.id == riga.unita) p = u.pos;
                }
                if (p) {
                    centro_ = *p;
                    segue_ = false;
                }
                if (riga.unita >= 0) scelta_ = riga.unita;
                break;
            }
        }
    }
    // trascinare la carta la sposta (e la carta non segue piu')
    const auto [mx, my] = c.mouse;
    const bool sulla_carta = mx >= carta_c_.x && mx < carta_c_.x + carta_c_.w && my >= carta_c_.y && my < carta_c_.y + carta_c_.h;
    if (c.premuto && (trascina_ || sulla_carta)) {
        if (trascina_ && (mx != trascina_->first || my != trascina_->second)) {
            centro_.x -= (mx - trascina_->first) * mpp_;
            centro_.y += (my - trascina_->second) * mpp_;
            segue_ = false;
        }
        trascina_ = std::make_pair(mx, my);
    } else {
        trascina_.reset();
    }
    return r;
}

bool SchermataReplay::comandi(const Comandi& c, double dt) {
    avviso_resta_ = std::max(0.0, avviso_resta_ - dt);
    if (c.esci || c.esc) return true;
    if (c.invio) {   // «Riprendi da qui»
        riprendi_da_ = t_;
        return true;
    }
    if (c.pausa) pausa_ = !pausa_;
    if (c.verita) lettura_ = lettura_ == Lettura::verita ? Lettura::quadro : Lettura::verita;
    // la velocita': Pag Su e Pag Giu' (la compressione del tempo), e anche + e - (anche del tastierino), che in
    // missione muovono il telegrafo e qui non hanno altro da fare
    int piu = c.compressione;
    for (const Azione a : c.azioni) piu += a == Azione::telegrafo_avanti ? 1 : a == Azione::telegrafo_indietro ? -1 : 0;
    for (; piu > 0; --piu) velocita_ = std::min(VELOCITA_MAX, velocita_ * 2.0);
    for (; piu < 0; ++piu) velocita_ = std::max(1.0, velocita_ / 2.0);
    if (c.ingrandisci) mpp_ = std::max(10.0, mpp_ / 1.5);
    if (c.riduci) mpp_ = std::min(2000.0, mpp_ * 1.5);
    rotella_carta(c, carta_c_, centro_, mpp_);
    // le frecce: un salto di 30 s subito, poi uno ogni 0,2 s finche' restano premute
    salto_attesa_ -= dt;
    const int verso = verso_freccia(c.ruota, c.passi_ruota);
    if (verso != salto_verso_) salto_attesa_ = 0.0;   // lasciata, o girata dall'altra parte: si ricomincia
    salto_verso_ = verso;
    if (verso != 0 && salto_attesa_ <= 0.0) {
        vai(t_ + 30.0 * verso);
        salto_attesa_ = 0.2;
    }
    // Tab: l'evento successivo (Maiuscolo+Tab il precedente) fra quelli della lettura
    if (c.tab != 0) {
        const bool solo_noti = lettura_ == Lettura::quadro;
        if (c.tab > 0) {
            for (const auto& e : segni_)
                if (e.tempo > t_ + 0.5 && (!solo_noti || e.noto)) { vai(e.tempo); break; }
        } else {
            for (auto it = segni_.rbegin(); it != segni_.rend(); ++it)
                if (it->tempo < t_ - 0.5 && (!solo_noti || it->noto)) { vai(it->tempo); break; }
        }
    }
    // G: l'intervento del vetrino di Dio successivo (Maiuscolo+G il precedente)
    for (const char l : c.lettere) {
        if (l != 'G' && l != 'g') continue;
        if (!c.veloce) {
            for (const RecordIntervento& x : tr_.interventi)
                if (x.t > t_ + 1e-3 && x.fase != RecordIntervento::FASE_FINITO) { vai(x.t); break; }
        } else {
            for (auto it = tr_.interventi.rbegin(); it != tr_.interventi.rend(); ++it)
                if (it->t < t_ - 1e-3 && it->fase != RecordIntervento::FASE_FINITO) { vai(it->t); break; }
        }
    }
    const Rettangolo b = linea_tempo(), cr = carta();
    for (const auto& [x, y] : c.clic) {
        if (y >= b.y - 4 && y < b.y + b.h + 4 && x >= b.x - 4 && x < b.x + b.w + 4) {
            vai(t_di(x));
        } else if (y >= PIEDE_Y + 22 && y < PIEDE_Y + 48) {   // i tre tasti del piede, larghi 202 da x 8, 218, 428
            if (x < 214) return true;
            if (x < 424) lettura_ = lettura_ == Lettura::verita ? Lettura::quadro : Lettura::verita;
            else {
                riprendi_da_ = t_;
                return true;
            }
        } else if (y >= cr.y - 14 && y < cr.y && x >= cr.x + cr.w / 2) {
            segui();   // la testata dello schermo, a destra: SEGUI IL BATTELLO
        }
    }
    // trascinare la carta la sposta (e la carta non segue piu' il battello)
    const auto [mx, my] = c.mouse;
    const bool sulla_carta = mx >= cr.x && mx < cr.x + cr.w && my >= cr.y && my < cr.y + cr.h;
    if (c.premuto && (trascina_ || sulla_carta)) {
        if (trascina_ && (mx != trascina_->first || my != trascina_->second)) {
            centro_.x -= (mx - trascina_->first) * mpp_;
            centro_.y += (my - trascina_->second) * mpp_;
            segue_ = false;
        }
        trascina_ = std::make_pair(mx, my);
    } else {
        trascina_.reset();
    }
    if (!pausa_) {
        vai(t_ + dt * velocita_);
        if (t_ >= tr_.durata) pausa_ = true;   // a fine traccia si ferma
    }
    return false;
}

void SchermataReplay::palette(Palette& p) const {
    applica_palette_carta(p, StileCarta::vga);
    applica_palette_rilievo(p, StileCarta::vga);
    applica_tema(p, TemaStazione{});   // 16-47: la lamiera e il fosforo della variante C
}

void SchermataReplay::disegna(Tela& t, const Font& f) const {
    t.riempi(colori::NERO);
    const IstanteTraccia i = istante_vista();
    if (vivo_) {   // dal vivo in variante C, come il replay (bozza vivo_C del kit, scelta dell'utente del 03/11/2026): la
        // consolle, la carta grande con la testata, il registro a destra coi filtri e la scheda, la fascia del tempo con
        // ADESSO, il piede comune
        const Font& g = fosforo_ ? *fosforo_ : f;
        cornice_vivo(t, f);
        disegna_carta(t, f, i);
        disegna_colonna(t, g, i);
        disegna_timeline(t, g);
        lamiera::piede(t, f, PIEDE_Y, "P PAUSA  + - VELOCITA'  FRECCE EVENTO  TAB UNITA'  S SEGUI  V LETTURA  I O ZOOM  G INTERVENTI",
                       {{"ESC  FINE", ColoreLampada::rossa},
                        {confronto_ ? "C  CONFRONTO: SI'" : "C  CONFRONTO", ColoreLampada::ambra},
                        {"INVIO  ADESSO", ColoreLampada::verde}},
                       2);
        if (const auto it = tr_.metadati.find("codice"); it != tr_.metadati.end() && !it->second.empty()) {
            const std::string codice = "CODICE " + it->second;
            f.scrivi(t, 632 - 6 - f.larghezza(codice, 1, 1, false), PIEDE_Y + (13 - f.righe()) / 2, codice, smalto(11), 1, 1, false);
        }
        const Rettangolo c = carta_c_;
        if (pannello_aperto()) pannello_->disegna(t, g, {c.x + 6, c.y + 6, 250, c.h - 12});
        disegna_avviso(t, f);
        return;
    }
    // il replay in variante C (roadmap 33, grafica/quadro; scelta dell'utente del 28/10/2026): la consolle della sala
    // analisi, la carta grande nello schermo, la legenda e la lettura in riquadri sopra la carta, la fascia del tempo e
    // il piede comune
    namespace q = quadro;
    const Font& g = fosforo_ ? *fosforo_ : f;
    const Rettangolo u = q::consolle(t, targa_ ? *targa_ : f, "SALA ANALISI - REPLAY");
    const int bh = g.righe() + 6, by = PIEDE_Y - 3 - bh;
    std::string titolo = maiu(tr_.missione.empty() ? std::string("MISSIONE") : tr_.missione);
    if (const auto it = tr_.metadati.find("comandante"); it != tr_.metadati.end()) titolo += " - " + maiu(it->second);
    titolo += lettura_ == Lettura::verita        ? "  VERITA'"
              : lettura_ == Lettura::parte_nato  ? "  PARTE NATO"
              : lettura_ == Lettura::parte_patto ? "  PARTE PATTO"
              : lettura_ == Lettura::unita       ? "  UNITA' " + nome_unita(scelta_ >= 0 ? scelta_ : giocatore_id_)
                                                 : "  QUADRO DEL GIOCATORE";
    const std::string segue = segue_ ? "SEGUE IL BATTELLO" : "SEGUI IL BATTELLO (CLIC)";
    carta_c_ = q::schermo(t, {u.x, u.y, u.w, by - 3 - u.y}, g, taglia_q(g, titolo, u.w - 24 - largo_q(g, segue)), segue);
    const Rettangolo c = carta_c_;
    disegna_carta(t, f, i);
    // la legenda coi simboli veri (grafica/simboli_carta del kit), in basso a sinistra sulla carta
    {
        namespace sc = simboli;
        using V = sc::VoceLegenda;
        std::vector<V> voci;
        if (lettura_ == Lettura::verita) {
            voci = {{V::Tipo::proprio, sc::Parte::amico, sc::Dominio::subacqueo, sc::Oggetto::siluro, sc::Emissione::ping, 0, "IL BATTELLO"},
                    {V::Tipo::unita, sc::Parte::amico, sc::Dominio::superficie, sc::Oggetto::siluro, sc::Emissione::ping, 0, "AMICI"},
                    {V::Tipo::unita, sc::Parte::ostile, sc::Dominio::subacqueo, sc::Oggetto::siluro, sc::Emissione::ping, 0, "OSTILI"},
                    {V::Tipo::unita, sc::Parte::neutrale, sc::Dominio::superficie, sc::Oggetto::siluro, sc::Emissione::ping, 0, "NEUTRALI"}};
        } else {
            voci = {{V::Tipo::unita, sc::Parte::ignoto, sc::Dominio::ignoto, sc::Oggetto::siluro, sc::Emissione::ping, 0, "I CONTATTI"},
                    {V::Tipo::croce_tma, sc::Parte::amico, sc::Dominio::superficie, sc::Oggetto::siluro, sc::Emissione::ping,
                     sc::indice_croce_tma(true), "SOLUZIONE TMA"},
                    {V::Tipo::oggetto, sc::Parte::amico, sc::Dominio::superficie, sc::Oggetto::siluro, sc::Emissione::ping, 0, "LE TUE ARMI"},
                    {V::Tipo::proprio, sc::Parte::amico, sc::Dominio::subacqueo, sc::Oggetto::siluro, sc::Emissione::ping, 0, "IL BATTELLO"}};
        }
        const int hl = g.righe() + 8 + 14 * static_cast<int>(voci.size());   // la testata del riquadro e una riga di 14 per voce
        const Rettangolo l = q::riquadro(t, {c.x + 6, c.y + c.h - 6 - hl, 112, hl}, g,
                                         lettura_ == Lettura::verita ? "TUTTE LE UNITA'" : "CIO' CHE SAPEVI");
        sc::disegna_legenda(t, g, l.x, l.y - 1, voci, fosforo(5));
    }
    // la lettura, in alto a destra sulla carta: gli ultimi eventi e la quota degli ultimi 10 minuti
    disegna_lettura(t, g, q::riquadro(t, {c.x + c.w - 6 - 256, c.y + 6, 256, 112}, g,
                                      lettura_ == Lettura::verita ? "LETTURA: VERITA'" : "LETTURA: QUADRO"));
    // la fascia del tempo: l'istante e lo stato a sinistra, l'evento corrente a destra, i segni degli eventi (colore e
    // altezza: quadro::fascia_tempo), la ripresa in verde
    {
        const double durata = std::max(1e-6, tr_.durata);
        const std::vector<q::TaccaTempo> tacche = tacche_eventi(segni_, durata, lettura_ == Lettura::quadro);
        const EventoTraccia* vicino = nullptr;
        for (const auto& e : segni_) {
            if (lettura_ == Lettura::quadro && !e.noto) continue;
            if (e.tempo <= t_ + 0.5) vicino = &e;
        }
        const std::string sinistra = orologio(t_) + " / " + orologio(tr_.durata) + "  X" + std::to_string(static_cast<int>(velocita_)) +
                                     (pausa_ ? "  PAUSA" : "");
        const std::string destra =
            vicino ? taglia_q(g, orologio(vicino->tempo).substr(2, 5) + " " + maiu(vicino->testo_it.empty() ? vicino->tipo : vicino->testo_it), 250)
                   : std::string();
        barra_ = q::fascia_tempo(t, {u.x, by, u.w, bh}, g, sinistra, destra, t_ / durata, tacche);
        for (const RecordIntervento& x : tr_.interventi)   // gli interventi del vetrino di Dio, sopra la barra (G: il seguente)
            if (x.fase != RecordIntervento::FASE_FINITO)
                triangolo(t, barra_.x + static_cast<int>(x.t / durata * barra_.w), barra_.y - 2,
                          x.fase == RecordIntervento::FASE_FALLITO ? std::uint8_t{colori::ROSSO} : colore_intervento(x.tipo));
        if (tr_.ripresa_da >= 0.0) {   // la partita ripresa: da li' il giocatore comandava di nuovo
            const int x = static_cast<int>(x_di(tr_.ripresa_da));
            t.linea_verticale(x, barra_.y, barra_.y + barra_.h - 1, colori::VERDE_CHIARO);
        }
    }
    // il piede comune; il codice partita (app/codice_partita: la missione, il seme e le opzioni, per rigiocarla uguale)
    // in fondo alla riga d'aiuto
    lamiera::piede(t, f, PIEDE_Y, "P GIOCA/PAUSA  + - VELOCITA'  FRECCE 30 S  TAB EVENTO  I O ZOOM  CLIC: SALTA",
                   {{"ESC  ESCI", ColoreLampada::rossa},
                    {lettura_ == Lettura::verita ? "V  LETTURA: QUADRO" : "V  LETTURA: VERITA'", ColoreLampada::ambra},
                    {"INVIO  RIPRENDI DA QUI", ColoreLampada::verde}},
                   2);
    if (const auto it = tr_.metadati.find("codice"); it != tr_.metadati.end() && !it->second.empty()) {
        const std::string codice = "CODICE " + it->second;
        f.scrivi(t, 632 - 6 - f.larghezza(codice, 1, 1, false), PIEDE_Y + (13 - f.righe()) / 2, codice, smalto(11), 1, 1, false);
    }
}

void SchermataReplay::cornice_vivo(Tela& t, const Font& f) const {
    const Font& g = fosforo_ ? *fosforo_ : f;
    std::string titolo = maiu(tr_.missione.empty() ? std::string("MISSIONE") : tr_.missione) + "  ";
    titolo += lettura_ == Lettura::verita       ? "VERITA'"
              : lettura_ == Lettura::parte_nato  ? "PARTE NATO"
              : lettura_ == Lettura::parte_patto ? "PARTE PATTO"
              : lettura_ == Lettura::unita       ? "UNITA' " + nome_unita(scelta_ >= 0 ? scelta_ : giocatore_id_)
                                                 : "QUADRO DEL BATTELLO";
    const std::string stato = (indietro_ ? "INDIETRO " + orologio(t_) : "DAL VIVO " + orologio(adesso_)) + "  X" +
                              std::to_string(static_cast<int>(velocita_)) + (pausa_ ? "  PAUSA" : "");
    const int wc = LARGHEZZA_TELA - 16 - COLONNA_W - 4;   // la carta: la larghezza utile della consolle meno il registro
    const std::string filtri = filtro_parte_ || filtro_tipo_
                                   ? std::string(filtro_parte_ ? NOMI_PARTE[filtro_parte_] : "") + (filtro_parte_ && filtro_tipo_ ? " " : "") +
                                         (filtro_tipo_ ? NOMI_TIPO[filtro_tipo_] : "")
                                   : "TUTTI";
    // i pezzi del kit (grafica/quadro, gruppo 6): la consolle, lo schermo della carta e quello del registro
    const quadro::DisposizioneVivo d = quadro::consolle_vivo(t, targa_ ? *targa_ : f, g, taglia_q(g, titolo, wc - 24 - largo_q(g, stato)), stato,
                                                             taglia_q(g, filtri, COLONNA_W - 70), COLONNA_W, PIEDE_Y);
    carta_c_ = d.carta;
    registro_c_ = d.registro;
    fascia_c_ = d.fascia;
}

void SchermataReplay::disegna_colonna(Tela& t, const Font& g, const IstanteTraccia& i) const {
    const Rettangolo rg = registro_c_;
    const int x0 = rg.x + 2, w = rg.w - 4, wt = w - largo_q(g, "00:00") - 6;   // la larghezza del testo di quadro::riga_registro
    // i filtri (un clic li fa scorrere: a sinistra la parte, a destra il tipo)
    scrivi_q(t, g, x0, rg.y + 2, std::string("PARTE ") + NOMI_PARTE[filtro_parte_], filtro_parte_ ? colori::GIALLO : fosforo(4));
    const std::string tipo = std::string("TIPO ") + NOMI_TIPO[filtro_tipo_];
    scrivi_q(t, g, x0 + w - largo_q(g, tipo), rg.y + 2, tipo, filtro_tipo_ ? colori::GIALLO : fosforo(4));
    t.linea_orizzontale(x0, x0 + w - 1, rg.y + 12, fosforo(2));
    // la scheda dell'unita' scelta in fondo, o come sceglierla
    const StatoUnita* u = nullptr;
    for (const auto& x : i.unita)
        if (x.id == scelta_) u = &x;
    // nel vetrino la scheda ha anche il profilo del suono nel punto dell'unita': due righe e il grafico
    const std::optional<ProfiloSuono> suono = u && suono_base_ ? std::optional<ProfiloSuono>(profilo_spettatore(*suono_base_, tr_.interventi, u->pos, t_)) : std::nullopt;
    scheda_y_ = rg.y + rg.h - (u ? 72 + (suono ? 68 : 0) : 26);
    // il registro fino all'istante mostrato, il piu' recente in basso; ogni evento su una o due righe
    const int y0 = rg.y + 16, y1 = scheda_y_ - 4;
    const int max_righe = std::max(0, (y1 - y0) / 10);
    zone_righe_.clear();
    struct Pezzo {
        std::vector<std::string> righe;
        std::uint8_t col;
        std::size_t k;
    };
    std::vector<Pezzo> pezzi;
    int usate = 0;
    const std::size_t corrente = [&] {   // indietro: l'evento all'istante mostrato
        std::size_t c = righe_.size();
        for (std::size_t k = 0; k < righe_.size(); ++k)
            if (righe_[k].t <= t_ + 0.5 && passa(righe_[k])) c = k;
        return c;
    }();
    for (std::size_t k = righe_.size(); k-- > 0 && usate < max_righe;) {
        const RigaRegistro& r = righe_[k];
        if (r.t > t_ + 0.5 || !passa(r)) continue;
        std::vector<std::string> righe{orologio(r.t).substr(2, 5)};   // la prima voce: l'ora
        std::string riga, parola;
        for (const char ch : r.testo + (r.volte > 1 ? " (X" + std::to_string(r.volte) + ")" : std::string()) + " ") {
            if (ch != ' ') {
                parola += ch;
                continue;
            }
            if (parola.empty()) continue;
            if (!riga.empty() && largo_q(g, riga + parola) > wt) {
                righe.push_back(riga);
                riga.clear();
                if (righe.size() == 3) break;
            }
            riga += parola + " ";
            parola.clear();
        }
        if (righe.size() < 3) righe.push_back(taglia_q(g, riga, wt));
        const int n = static_cast<int>(righe.size()) - 1;
        if (usate + n > max_righe) break;
        usate += n;
        // i colori degli eventi sul fosforo: il grigio dei fatti minori diventa il verde del vetro
        pezzi.push_back({std::move(righe), r.colore == colori::GRIGIO ? fosforo(5) : r.colore, k});
    }
    int y = y0 + (max_righe - usate) * 10;
    for (auto it = pezzi.rbegin(); it != pezzi.rend(); ++it) {
        const int n = static_cast<int>(it->righe.size()) - 1;
        zone_righe_.push_back({{x0, y, w, n * 10}, it->k});
        const bool scelta = indietro_ && it->k == corrente;   // indietro: l'evento all'istante mostrato, in negativo
        for (int k = 1; k <= n; ++k) {
            quadro::riga_registro(t, rg, y, g, k == 1 ? it->righe.front() : std::string(), it->righe[static_cast<std::size_t>(k)], it->col, scelta);
            y += 10;
        }
    }
    if (pezzi.empty()) scrivi_q(t, g, x0, y0, "NESSUN EVENTO", fosforo(3));
    t.linea_orizzontale(x0, x0 + w - 1, scheda_y_ - 3, fosforo(2));
    if (!u) {
        scrivi_q(t, g, x0, scheda_y_, segue_ ? "SEGUE IL BATTELLO" : "SEGUI IL BATTELLO (CLIC)", segue_ ? fosforo(5) : fosforo(7));
        scrivi_q(t, g, x0, scheda_y_ + 11, "TAB SCEGLI UN'UNITA'", fosforo(5));
        return;
    }
    const auto riga_scheda = [&](int k, const std::string& testo, std::uint8_t col) { scrivi_q(t, g, x0, scheda_y_ + 10 * k, taglia_q(g, testo, w), col); };
    riga_scheda(0, nome_classe(u->classe) + " #" + std::to_string(u->id) + (segue_ ? "  SEGUITA" : "  (S: SEGUI)"),
                static_cast<std::uint8_t>(colori_carta::NTDS + lato(*u)));
    char riga[64];
    std::snprintf(riga, sizeof riga, "%d M  %.0f KN  ROTTA %03d", static_cast<int>(std::lround(u->quota)), u->velocita_kn,
                  static_cast<int>(std::lround(normalizza_360(u->rotta))) % 360);
    riga_scheda(1, riga, fosforo(7));
    std::string stato = u->allerta >= 0 ? "ALLERTA A" + std::to_string(u->allerta) : std::string("ALLERTA -");
    if (u->sigma_nav_m > 0.0) {
        std::snprintf(riga, sizeof riga, "  STIMA +-%.0f M", 2.0 * u->sigma_nav_m);
        stato += riga;
    }
    riga_scheda(2, stato, fosforo(5));
    // la TMA in corso e le armi a bordo (versione 2, Missione::tma_ia e tubi_carichi del runtime)
    std::string tma = "TMA -";
    if (u->tma_bersaglio >= 0)
        tma = "TMA " + nome_unita(u->tma_bersaglio) + " " + std::to_string(u->tma_gambe) + "G" +   // gambe, soluzione
              (u->tma_soluzione ? (u->tma_osservabile ? " SOL" : " SOL?") : "");
    riga_scheda(3, tma, u->tma_soluzione ? std::uint8_t{colori::GIALLO} : fosforo(5));
    std::string armi = u->armi_rimaste >= 0 ? "ARMI " + std::to_string(u->armi_rimaste) : std::string("ARMI -");
    if (u->tubi_carichi >= 0) armi += "  TUBI CARICHI " + std::to_string(u->tubi_carichi);
    riga_scheda(4, armi, fosforo(5));
    std::string extra = u->distrutta ? "DISTRUTTA" : u->sul_fondo ? "SUL FONDO" : u->incagliata ? "INCAGLIATA" : "IN AZIONE";
    if (u->alberi & 0x02) extra += "  PERISCOPIO";
    if (u->alberi & 0x04) extra += "  ANTENNA";
    if (u->alberi & 0x01) extra += "  SCHIERA";
    if (u->emissioni & StatoUnita::EMESSO_PING) extra += "  PING";
    if (u->emissioni & StatoUnita::EMESSO_RADAR) extra += "  RADAR";
    if (u->emissioni & StatoUnita::EMESSO_RADIO) extra += "  RADIO";
    riga_scheda(5, extra, fosforo(5));
    // la reazione in corso e, per il battello, la fase del comandante IA: gli ultimi eventi del registro che lo riguardano
    std::string ultima;
    for (const auto& e : tr_.registro) {
        if (e.tempo > t_ + 0.5 || e.unita != u->id) continue;
        if (e.tipo == "reazione_ia" || (u->id == giocatore_id_ && e.tipo == "comandante_ia")) ultima = maiu(e.testo_it);
    }
    riga_scheda(6, ultima.empty() ? maiu(u->categoria) : ultima, colori::CIANO_CHIARO);
    if (suono) {
        riga_scheda(7, riga_strato(*suono, *suono_base_), colori::GIALLO);
        riga_scheda(8, riga_asse(*suono, *suono_base_), fosforo(5));
        const double quota = u->categoria == "aereo" ? -1.0 : std::max(0.0, u->quota);
        const double scala = scala_suono(*suono, *suono_base_, quota);
        const Rettangolo r{x0, scheda_y_ + 91, w - largo_q(g, "0000M") - 2, rg.y + rg.h - scheda_y_ - 93};
        curva_suono(t, r, *suono, *suono_base_, scala, quota, static_cast<std::uint8_t>(colori_carta::NTDS + lato(*u)));
        const std::string fondo = std::to_string(static_cast<int>(scala)) + "M";
        scrivi_q(t, g, x0 + w - largo_q(g, fondo), r.y + r.h - g.righe(), fondo, fosforo(4));
    }
}

void SchermataReplay::disegna_carta(Tela& t, const Font& f, const IstanteTraccia& i) const {
    namespace sc = simboli;
    const GeometriaCarta g = geometria();
    if (fondale_) {
        OpzioniCarta o;
        o.stile = StileCarta::vga;
        vga_.disegna(t, g, *fondale_, o);
    } else {
        t.rettangolo_pieno(g.x, g.y, g.w, g.h, static_cast<std::uint8_t>(colori_carta::ACQUA + 30));
    }
    t.imposta_ritaglio({g.x, g.y, g.w, g.h});
    // il pack per lo spettatore, col tratteggio e il bordo della carta del giocatore, ma col bordo vero e non sulla terra; solo il
    // vetrino ha la griglia
    if (const CampoGhiaccio* c = campo_ghiaccio()) {
        StratiMeteo s;
        s.a_tela = [&g](Vec2 p) { return g.a_tela(p); };
        s.a_mondo = [&g](Vec2 q) { return g.a_mondo(q); };
        s.area = {g.x, g.y, g.w, g.h};
        s.ghiaccio = [c, tempo = t_, fondo = fondale_.get()](Vec2 p) { return fondo && fondo->profondita(p) <= 0.0 ? 0.0 : c->copertura_in(p, tempo); };
        s.colori.ghiaccio = colori::BIANCO;
        disegna_strati_meteo(t, f, s);
        t.imposta_ritaglio({g.x, g.y, g.w, g.h});   // gli strati tolgono il loro ritaglio
    }
    const std::vector<PolynyaVista> polynye = polynye_a(tr_.interventi, t_);
    std::function<bool(Vec2)> nel_pack;
    if (const CampoGhiaccio* c = campo_ghiaccio())
        nel_pack = [this, c, tempo = t_](Vec2 p) { return pack_senza_zone(*griglia_, *c, p, tempo) >= 0.5; };
    for (const PolynyaVista& z : polynye) disegna_polynya(t, g, z, mpp_, nel_pack);
    const auto ntds = [](int k) { return static_cast<std::uint8_t>(colori_carta::NTDS + k); };
    // le etichette non si sovrappongono: scendono di una riga finche' trovano posto, altrimenti si tacciono
    std::vector<Rettangolo> occupate;
    const auto etichetta = [&](Vec2 q, const std::string& testo, std::uint8_t col) {
        int x = static_cast<int>(q.x), y = static_cast<int>(q.y);
        const int w = f.larghezza(testo, 1) + 2, h = 9;
        for (int prova = 0; prova < 4; ++prova, y += h) {
            const bool libera = std::none_of(occupate.begin(), occupate.end(), [&](const Rettangolo& r) {
                return x < r.x + r.w && r.x < x + w && y < r.y + r.h && r.y < y + h;
            });
            if (libera) {
                occupate.push_back({x, y, w, h});
                sc::scrivi(t, f, x, y, testo, col);
                return;
            }
        }
    };
    // il simbolo APP-6 della parte e del dominio (grafica/simboli_carta del kit)
    const auto simbolo = [&](Vec2 q, const std::string& categoria, const std::string& classe, sc::Parte p, std::uint8_t c) {
        sc::disegna_simbolo_unita(t, q, p, sc::dominio_da_categoria(categoria, classe), c);
    };
    const auto parte_di = [](int l) { return static_cast<sc::Parte>(l); };   // 0 amico, 1 ostile, 2 ignoto, 3 neutrale
    // un'arma in acqua (siluro, missile, esca, mina, boa, biologico) nel colore della parte di chi l'ha lanciata, col filo
    // il cambio di parte (D4): il flag d'oggi di chi l'ha cambiata (flag_unita da' quello d'inizio partita), e la parte
    // che crede chi guarda
    const auto cambi = cambi_creduti(tr_, t_);
    const auto flag_ora = [&](int id) {
        const int fl = flag_unita(id);
        const auto c = cambi.find(id);
        return fl < 0 || c == cambi.end() ? fl : (fl & ~0x18) | c->second.nuova;
    };
    const auto oggetto = [&](const StatoUnita& u, Vec2 q, const std::map<int, Vec2>& dove) {
        // senza lanciatore (il siluro «di nessuno» della D3) la parte creduta, nel suo flag
        const int fl = u.lanciatore >= 0 ? flag_ora(u.lanciatore) : (u.flag & 0x18) ? u.flag : -1;
        const auto col = sc::indice_parte(parte_di(fl < 0 ? 2 : lato_flag(fl)));
        sc::disegna_oggetto(t, q, sc::oggetto_da_classe(u.categoria, u.classe), col, u.rotta);
        if (u.collegata >= 0)
            if (const auto it = dove.find(u.collegata); it != dove.end()) sc::disegna_filo(t, q, g.a_tela(it->second), col);
    };
    // la navigazione (versione 2): la posizione che l'unita' crede di avere, col filo verso la vera e l'ellisse al 95%
    const auto stima = [&](const StatoUnita& u, Vec2 q, std::uint8_t col) {
        const double maggiore = sc::K_ELLISSE_95 * u.sigma_nav_m / mpp_;
        if (u.sigma_nav_m <= 0.0 || maggiore < 2.0) return;
        const bool proprio = u.id == giocatore_id_;
        const auto tenue = proprio ? sc::VERDE_PROPRIO : static_cast<std::uint8_t>(colori_carta::NTDS + 4 + lato(u));
        const Vec2 s = g.a_tela(u.pos_stimata);
        linea(t, q, s, tenue, 1);
        linea(t, {s.x - 3, s.y}, {s.x + 3, s.y}, proprio ? sc::VERDE_PROPRIO : col);
        linea(t, {s.x, s.y - 3}, {s.x, s.y + 3}, proprio ? sc::VERDE_PROPRIO : col);
        sc::disegna_ellisse(t, s, maggiore, sc::K_ELLISSE_95 * (u.sigma_min_m > 0.0 ? u.sigma_min_m : u.sigma_nav_m) / mpp_, u.orientamento_deg, tenue);
    };
    const auto croce_tma = [&](Vec2 pos, double rotta, double velocita_m_s, bool certa) {
        const Vec2 q = g.a_tela(pos);
        const auto m = sc::indice_croce_tma(true);
        sc::disegna_croce_tma(t, q, m);
        linea(t, q, g.a_tela(avanti(pos, rotta, velocita_m_s * 120.0)), m, certa ? 0 : 2);
    };
    // il falso contatto (D3): il segno dell'inganno accanto; dove lo si crede, la doppia freccia dalla sorgente vera
    const auto falso_di = [&](int id) -> const StatoUnita* {
        for (const auto& u : i.unita)
            if (u.id == id) return falso(u) ? &u : nullptr;
        return nullptr;
    };
    const auto inganno = simboli::FamigliaIntervento::inganno;
    const auto col_falso = simboli::indice_famiglia(inganno);
    const auto segno_falso = [&](Vec2 q) { simboli::disegna_intervento(t, {q.x - 12.0, q.y - 10.0}, inganno, col_falso); };
    const auto creduto = [&](int bersaglio, Vec2 dove) {   // un contatto di chi guarda, nella posizione stimata
        const StatoUnita* x = falso_di(bersaglio);
        if (!x) return;
        sc::disegna_doppia_freccia(t, g.a_tela(x->pos), g.a_tela(dove), col_falso);
        segno_falso(g.a_tela(dove));
    };
    const auto creduto_su = [&](int bersaglio, Vec2 origine, double rilevamento) {   // col solo rilevamento: alla distanza vera
        if (const StatoUnita* x = falso_di(bersaglio)) creduto(bersaglio, avanti(origine, rilevamento, distanza(origine, x->pos)));
    };
    const StatoUnita* g0 = giocatore(i);
    if (lettura_ == Lettura::verita) {
        // le scie: i campioni degli ultimi 5 minuti
        for (const auto& [id, campioni] : tr_.unita) {
            Vec2 prima{};
            bool c_e = false;
            std::uint8_t col = ntds(3);
            for (const auto& [tc, u] : campioni) {
                if (tc > t_) break;
                if (tc < t_ - SCIA_S || u.rimossa) continue;
                col = id == giocatore_id_ ? sc::VERDE_PROPRIO : static_cast<std::uint8_t>(colori_carta::NTDS + 4 + lato_flag(flag_ora(id)));   // attenuato
                const Vec2 q = g.a_tela(u.pos);
                if (c_e) linea(t, prima, q, col, 2);
                prima = q;
                c_e = true;
            }
        }
        std::map<int, Vec2> dove;
        for (const auto& u : i.unita) dove[u.id] = u.pos;
        const auto colpe = colpe_credute(tr_, t_);
        for (const auto& u : i.unita) {
            const Vec2 q = g.a_tela(u.pos);
            const int lato_vero = cambi.count(u.id) ? lato_flag(flag_ora(u.id)) : lato(u);   // chi ha cambiato parte: quella nuova
            const std::uint8_t col = ntds(lato_vero);
            if (falso(u)) {   // il falso contatto: la classe imitata nella parte creduta, col segno dell'inganno
                simbolo(q, categoria_imitata(u.classe), u.classe, parte_di(lato(u)), col);
                segno_falso(q);
                linea(t, q, g.a_tela(avanti(u.pos, u.rotta, std::max(500.0, u.velocita_kn * NODO * 120.0))), col);
                etichetta({q.x + 7, q.y - 3}, "FALSO " + nome_classe(u.classe), col_falso);
                continue;
            }
            if (arma(u)) {   // l'arma in acqua, col filo verso chi la guida
                oggetto(u, q, dove);
                if (di_nessuno(u)) {   // il siluro dell'incidente provocato: il segno dell'inganno e la parte creduta
                    segno_falso(q);
                    etichetta({q.x + 7, q.y - 3}, "SILURO DI NESSUNO", col_falso);
                } else if (const int fl = flag_ora(u.lanciatore);   // la false flag (D4): la parte che sembra non e' di chi lancia
                           u.lanciatore >= 0 && (u.flag & 0x20) && fl >= 0 && (u.flag & 0x18) != (fl & 0x18)) {
                    segno_falso(q);
                    etichetta({q.x + 7, q.y - 3}, "FALSE FLAG: CREDUTA " + nome_parte_creduta(u.flag & 0x18), col_falso);
                    if (const auto vera = dove.find(u.lanciatore); vera != dove.end())   // da chi ha lanciato davvero
                        sc::disegna_doppia_freccia(t, g.a_tela(vera->second), q, col_falso);
                }
                continue;
            }
            stima(u, q, col);
            // le emissioni: l'anello del ping, il ventaglio del radar, la radio
            if (u.emissioni & StatoUnita::EMESSO_PING) sc::disegna_emissione(t, q, sc::Emissione::ping, t_);
            if (u.emissioni & StatoUnita::EMESSO_RADAR) sc::disegna_emissione(t, q, sc::Emissione::radar, t_);
            if (u.emissioni & StatoUnita::EMESSO_RADIO) sc::disegna_emissione(t, q, sc::Emissione::radio, t_);
            if (u.id == scelta_) sc::disegna_scelta(t, q);
            if (u.id == giocatore_id_) sc::disegna_proprio(t, q);
            else simbolo(q, u.categoria, u.classe, parte_di(lato_vero), col);
            linea(t, q, g.a_tela(avanti(u.pos, u.rotta, std::max(500.0, u.velocita_kn * NODO * 120.0))), col);   // due minuti di rotta
            if (u.alberi & 0x06) linea(t, {q.x, q.y - 6.0}, {q.x, q.y - 10.0}, static_cast<std::uint8_t>(colori::BIANCO));   // periscopio, antenna
            if (u.alberi & 0x01) linea(t, q, g.a_tela(avanti(u.pos, u.rotta + 180.0, 1500.0)), col, 1);                     // schiera filata
            if (u.distrutta) sc::disegna_distrutta(t, q);
            etichetta({q.x + 7, q.y - 3}, nome_classe(u.classe) + (u.allerta >= 0 ? " A" + std::to_string(u.allerta) : std::string()), col);
            std::string creduta;   // chi inganna: la parte che gli altri credono, finche' non lo sanno
            if (const auto c = cambi.find(u.id); c != cambi.end()) creduta = etichetta_creduta(c->second);
            if (const auto c = colpe.find(u.id); creduta.empty() && c != colpe.end()) creduta = c->second;
            if (!creduta.empty()) {
                segno_falso(q);
                etichetta({q.x + 7, q.y + 7}, creduta, col_falso);
            }
        }
    } else if (lettura_ == Lettura::quadro) {
        // il quadro: le linee della TMA degli ultimi 10 minuti, i contatti, la soluzione proposta, il battello
        for (const auto& r : i.rilevamenti) {
            if (r.tempo < t_ - 600.0) continue;
            linea(t, g.a_tela(r.proprio), g.a_tela(avanti(r.proprio, r.rilevamento, 30000.0)), static_cast<std::uint8_t>(colori_carta::MATITA + 2), 2);
        }
        for (const auto& c : i.contatti) {
            const std::string nome = std::string(1, c.lettera ? c.lettera : '?') + (c.classe_creduta.empty() ? "" : " " + nome_classe(c.classe_creduta));
            if (c.posizione_nota) {
                const Vec2 q = g.a_tela(c.pos_stimata);
                simbolo(q, "", c.classe_creduta, sc::Parte::ignoto, ntds(2));
                etichetta({q.x + 6, q.y - 3}, nome, ntds(2));
                creduto(c.id, c.pos_stimata);
            } else if (g0) {   // la sola linea di rilevamento (e lo specchio della schiera, tratteggiato)
                const Vec2 a = g.a_tela(g0->pos), b = g.a_tela(avanti(g0->pos, c.rilevamento, 20000.0));
                sc::disegna_rilevamento(t, a, b, ntds(2));
                etichetta({b.x + 2, b.y - 3}, nome, ntds(2));
                if (c.specchio) sc::disegna_rilevamento(t, a, g.a_tela(avanti(g0->pos, *c.specchio, 20000.0)), ntds(2), true);
                creduto_su(c.id, g0->pos, c.rilevamento);
            }
        }
        if (i.app.soluzione && !i.rilevamenti.empty()) {   // la soluzione proposta: il bersaglio lungo l'ultimo rilevamento
            const Rilevamento& r = i.rilevamenti.back();
            const Vec2 b0 = avanti(r.proprio, r.rilevamento, i.app.proposta.distanza_m);
            const Vec2 b = avanti(b0, i.app.proposta.rotta, i.app.proposta.velocita_m_s * (t_ - r.tempo));
            croce_tma(b, i.app.proposta.rotta, i.app.proposta.velocita_m_s, true);
        }
        // le proprie armi in acqua col filo: il giocatore le conosce
        std::map<int, Vec2> dove;
        for (const auto& u : i.unita) dove[u.id] = u.pos;
        for (const auto& u : i.unita) {
            if (!arma(u) || u.lanciatore != giocatore_id_) continue;
            oggetto(u, g.a_tela(u.pos), dove);
        }
    } else {
        // la parte (NATO o Patto) o l'unita' scelta: cio' che sa
        const int mio = lettura_ == Lettura::parte_nato ? 0x08 : lettura_ == Lettura::parte_patto ? 0x10 : -1;
        const int quale = scelta_ >= 0 ? scelta_ : giocatore_id_;
        std::map<int, Vec2> dove;
        for (const auto& u : i.unita) dove[u.id] = u.pos;
        const int flag_quale = flag_ora(quale);
        for (const auto& u : i.unita) {
            const int fu = mio >= 0 ? parte_creduta_da(cambi, u.id, mio, -1, u.flag & 0x18) : u.flag & 0x18;   // chi non lo sa la crede ancora dei suoi
            const int suo = fu ? (fu == 0x08 ? 0x08 : 0x10) : 0;
            const bool dentro = mio >= 0 ? suo == mio : u.id == quale ||
                                (arma(u) && u.lanciatore == quale);   // l'unita' e le sue armi in acqua
            if (!dentro || falso(u)) continue;   // il falso contatto non e' di nessuno
            const Vec2 q = g.a_tela(u.pos);
            if (arma(u)) {
                oggetto(u, q, dove);
                continue;
            }
            const std::uint8_t col = ntds(0);   // dalla parte di chi guarda: amica
            stima(u, q, col);
            if (u.id == giocatore_id_) sc::disegna_proprio(t, q, u.id == quale);
            else simbolo(q, u.categoria, u.classe, sc::Parte::amico, col);
            if (u.id == quale && mio < 0) sc::disegna_scelta(t, q);
            linea(t, q, g.a_tela(avanti(u.pos, u.rotta, std::max(500.0, u.velocita_kn * NODO * 120.0))), col);
            etichetta({q.x + 7, q.y - 3}, nome_classe(u.classe) + (u.allerta >= 0 ? " A" + std::to_string(u.allerta) : std::string()), col);
        }
        const auto ellisse = [&](Vec2 c, double a1, double b1, double orient, std::uint8_t col) {   // al 95%
            const double a = sc::K_ELLISSE_95 * a1 / mpp_;
            if (a1 <= 0.0 || a < 2.0) return;
            sc::disegna_ellisse(t, g.a_tela(c), a, sc::K_ELLISSE_95 * (b1 > 0.0 ? b1 : a1) / mpp_, orient, col);
        };
        const auto eta = [](double s) { return s < 60.0 ? std::to_string(static_cast<int>(s)) + " S" : std::to_string(static_cast<int>(s / 60.0)) + " MIN"; };
        if (mio >= 0) {   // il quadro della parte (record P): una voce per bersaglio col dato migliore
            if (confronto_)   // la verita' degli altri, in grigio sotto
                for (const auto& u : i.unita) {
                    const int fv = cambi.count(u.id) ? flag_ora(u.id) : u.flag;
                    const int suo = fv & 0x18 ? ((fv & 0x18) == 0x08 ? 0x08 : 0x10) : 0;
                    if (suo == mio || arma(u)) continue;
                    simbolo(g.a_tela(u.pos), u.categoria, u.classe, parte_di(lato_flag(fv)), static_cast<std::uint8_t>(colori::GRIGIO_SCURO));
                }
            const QuadroParte* qp = tr_.quadro_parte(mio, t_);
            if (qp)
                for (const VoceParte& v : qp->voci) {
                    const double vecchia = t_ - v.t_dato;
                    // con la classe la parte si sa (amica, ostile o neutrale rispetto a chi guarda); senza, ignoto
                    const int fb = parte_creduta_da(cambi, v.bersaglio, mio, -1, flag_unita(v.bersaglio) & 0x18);
                    const sc::Parte pv = v.classe.empty() ? sc::Parte::ignoto : fb == 0 ? sc::Parte::neutrale
                                       : ((fb == 0x08) == (mio == 0x08)) ? sc::Parte::amico : sc::Parte::ostile;
                    const std::uint8_t col = sc::indice_parte_eta(pv, vecchia);
                    const std::string nome = (v.classe.empty() ? (v.categoria.empty() ? std::string("?") : maiu(v.categoria)) : nome_classe(v.classe)) +
                                             " " + eta(vecchia) + " " + nome_classe(v.sensore);
                    if (v.posizione) {
                        const Vec2 q = g.a_tela(v.pos);
                        ellisse(v.pos, v.semiasse_maggiore_m, v.semiasse_minore_m, v.orientamento_deg, col);
                        simbolo(q, v.categoria, v.classe, pv, col);
                        etichetta({q.x + 7, q.y - 3}, nome, col);
                        creduto(v.bersaglio, v.pos);
                    } else {
                        const Vec2 a = g.a_tela(v.origine), b = g.a_tela(avanti(v.origine, v.rilevamento, 20000.0));
                        sc::disegna_rilevamento(t, a, b, col);
                        etichetta({b.x + 2, b.y - 3}, nome, col);
                        creduto_su(v.bersaglio, v.origine, v.rilevamento);
                    }
                }
            else if (tr_.quadri_parte.empty())
                testo_con_alone(t, g.x + 8, g.y + 6, "QUADRO DI PARTE NON REGISTRATO", f, colori::GRIGIO, colori::NERO);
        } else {   // i contatti dell'unita' (record K): posizione stimata o linea di rilevamento, piu' vivi se solidi
            Vec2 da{};
            for (const auto& u : i.unita)
                if (u.id == quale) da = u.sigma_nav_m > 0.0 ? u.pos_stimata : u.pos;
            for (const ContattoUnita& k : tr_.quadro_unita(quale, t_)) {
                const int fk = parte_creduta_da(cambi, k.bersaglio, flag_quale & 0x18, quale, flag_unita(k.bersaglio) & 0x18);
                const sc::Parte pk = k.classe_creduta.empty() ? sc::Parte::ignoto : parte_di(lato_flag(fk));
                const std::uint8_t col = sc::indice_parte(pk, !k.solido);
                const std::string nome = (k.classe_creduta.empty() ? std::string("?") : nome_classe(k.classe_creduta)) + " " +
                                         std::to_string(static_cast<int>(std::lround(k.qualita))) + " " + nome_classe(k.sensore);
                if (k.posizione_nota) {
                    const Vec2 q = g.a_tela(k.pos_stimata);
                    simbolo(q, "", k.classe_creduta, pk, col);
                    etichetta({q.x + 6, q.y - 3}, nome, col);
                    creduto(k.bersaglio, k.pos_stimata);
                } else {
                    const Vec2 a = g.a_tela(da), b = g.a_tela(avanti(da, k.rilevamento, 20000.0));
                    sc::disegna_rilevamento(t, a, b, col);
                    etichetta({b.x + 2, b.y - 3}, nome, col);
                    creduto_su(k.bersaglio, da, k.rilevamento);
                }
            }
            for (const auto& u : i.unita)   // la soluzione della sua TMA: la croce e due minuti di rotta
                if (u.id == quale && u.tma_soluzione) croce_tma(u.tma_pos, u.tma_rotta, u.tma_velocita_kn * NODO, u.tma_osservabile);
            if (tr_.contatti_unita.empty())
                testo_con_alone(t, g.x + 8, g.y + 6, "CONTATTI DELL'UNITA' NON REGISTRATI", f, colori::GRIGIO, colori::NERO);
        }
    }
    if (g0 && (lettura_ == Lettura::verita || lettura_ == Lettura::quadro)) {   // il battello, sempre sopra: il simbolo con un anello bianco, che si veda su ogni fondale
        const Vec2 q = g.a_tela(g0->pos);
        sc::disegna_proprio(t, q, true);
        linea(t, q, g.a_tela(avanti(g0->pos, g0->rotta, std::max(500.0, g0->velocita_kn * NODO * 120.0))), ntds(0));
        if (g0->alberi & 0x06) linea(t, {q.x, q.y - 6.0}, {q.x, q.y - 10.0}, static_cast<std::uint8_t>(colori::BIANCO));
    }
    // gli interventi del vetrino di Dio attivi a quest'istante, coi segni del kit: l'icona della famiglia accanto
    // all'unita' (una per famiglia, in fila verso sinistra), il «vai a» tratteggiato, la zona; l'unita' tolta con una
    // croce per due minuti; dal vivo i programmati col punto, l'icona con gli angoli e l'ora
    {
        const auto pos_unita = [&](int id) -> std::optional<Vec2> {
            for (const auto& u : i.unita)
                if (u.id == id) return u.pos;
            return std::nullopt;
        };
        std::map<int, std::vector<simboli::FamigliaIntervento>> famiglie;   // unita' -> le famiglie dei suoi interventi
        for (const RecordIntervento* x : interventi_a(tr_, t_)) {
            const std::uint8_t col = colore_intervento(x->tipo);
            if (x->tipo == "togli") {
                if (t_ - x->t > TOLTA_S) continue;
                const auto it = tr_.unita.find(x->unita);
                if (it == tr_.unita.end() || it->second.empty()) continue;
                const Vec2 q = g.a_tela(it->second.back().second.pos);
                linea(t, {q.x - 4, q.y - 4}, {q.x + 4, q.y + 4}, col);
                linea(t, {q.x - 4, q.y + 4}, {q.x + 4, q.y - 4}, col);
                testo_con_alone(t, static_cast<int>(q.x) + 6, static_cast<int>(q.y) - 3, "TOLTA", f, col, colori::NERO);
                continue;
            }
            if (x->unita >= 0) {
                auto& v = famiglie[x->unita];
                if (std::find(v.begin(), v.end(), famiglia_intervento(x->tipo)) == v.end()) v.push_back(famiglia_intervento(x->tipo));
                if (const auto p = pos_unita(x->unita); p && x->ha_punto && x->tipo != "incidente")   // il «vai a» dell'ordine
                    linea(t, g.a_tela(*p), g.a_tela(x->punto), col, 3);
            }
            if (x->raggio_m > 0.0 && x->ha_punto) {   // la zona (il meteo): un cerchio
                const Vec2 c = g.a_tela(x->punto);
                const double rr = x->raggio_m / mpp_;
                for (int k = 0; k < 48; ++k) {
                    const double a0 = k * 2.0 * PI / 48.0, a1 = (k + 1) * 2.0 * PI / 48.0;
                    linea(t, {c.x + rr * std::cos(a0), c.y + rr * std::sin(a0)}, {c.x + rr * std::cos(a1), c.y + rr * std::sin(a1)}, col);
                }
            }
        }
        for (const auto& [id, v] : famiglie)
            if (const auto p = pos_unita(id)) {
                const Vec2 q = g.a_tela(*p);
                for (std::size_t k = 0; k < v.size(); ++k)
                    simboli::disegna_intervento(t, {q.x - 12.0 - 12.0 * static_cast<double>(k), q.y - 10.0}, v[k], simboli::indice_famiglia(v[k]));
            }
        if (vivo_)
            for (const auto& q : richieste_) {
                if (q.subito || q.tempo <= adesso_ || !q.punto) continue;
                const Vec2 c = g.a_tela(*q.punto);
                const std::uint8_t col = colore_intervento(q.tipo);
                simboli::disegna_intervento(t, c, famiglia_intervento(q.tipo), col, true);
                testo_con_alone(t, static_cast<int>(c.x) + 9, static_cast<int>(c.y) - 3, orologio(q.tempo), f, col, colori::NERO);
            }
        // la D3, comunicazioni e inganno: in alto a sinistra gli interventi su tutta una parte (il disturbo senza zona,
        // l'ordine falso col suo «vai a», la spia) e per due minuti l'incidente provocato, con la stella dello scoppio o
        // il segno sul punto del siluro «di nessuno»; sotto il simbolo di ogni unita' che l'ha ricevuto come ha preso
        // l'ordine falso; le localizzazioni radiogoniometriche degli ultimi due minuti, tratteggiate da chi localizza a
        // chi trasmette
        int yb = g.y + 6;
        bool ordine_attivo = false;
        for (const RecordIntervento* x : interventi_a(tr_, t_)) {
            const bool zona = x->raggio_m > 0.0 && x->ha_punto;
            const bool incidente = (x->tipo == "incidente" || x->tipo == "false_flag") && t_ - x->t <= TOLTA_S;
            if (incidente && x->tipo == "incidente" && x->ha_punto) {
                const Vec2 c = g.a_tela(x->punto);
                const std::uint8_t col = colore_intervento(x->tipo);
                if (leggi_chiavi(x->chiavi)["incidente"] == "scoppio")   // la stella: otto raggi
                    for (int k = 0; k < 8; ++k) {
                        const double a = k * PI / 4.0;
                        linea(t, {c.x + 3.0 * std::cos(a), c.y + 3.0 * std::sin(a)}, {c.x + 8.0 * std::cos(a), c.y + 8.0 * std::sin(a)}, col);
                    }
                else
                    simboli::disegna_intervento(t, c, famiglia_intervento(x->tipo), col);
                testo_con_alone(t, static_cast<int>(c.x) + 10, static_cast<int>(c.y) - 3, "INCIDENTE", f, col, colori::NERO);
            }
            // l'ambiente della D4: la scritta nella zona (il cerchio e' qui sopra), o in alto se vale per tutto il teatro;
            // il pack che deriva con la freccia della rotta
            if (const std::string amb = riga_ambiente(*x, t_); !amb.empty()) {
                const std::uint8_t col = colore_intervento(x->tipo);
                if (zona) {
                    const Vec2 c = g.a_tela(x->punto);
                    simboli::disegna_intervento(t, c, famiglia_intervento(x->tipo), col);
                    testo_con_alone(t, static_cast<int>(c.x) + 8, static_cast<int>(c.y) - 3, amb, f, col, colori::NERO);
                    continue;
                }
                simboli::disegna_intervento(t, {g.x + 10.0, yb + 3.0}, famiglia_intervento(x->tipo), col);
                testo_con_alone(t, g.x + 18, yb, amb, f, col, colori::NERO);
                if (x->tipo == "ghiaccio_deriva") {
                    const double a = std::strtod(leggi_chiavi(x->chiavi)["#rotta"].c_str(), nullptr) * PI / 180.0;
                    const Vec2 c{g.x + 18.0 + largo_q(f, amb) + 10.0, yb + 3.0};
                    const Vec2 punta{c.x + 6.0 * std::sin(a), c.y - 6.0 * std::cos(a)};
                    linea(t, {c.x - 6.0 * std::sin(a), c.y + 6.0 * std::cos(a)}, punta, col);
                    linea(t, punta, {punta.x - 3.0 * std::sin(a - 0.5), punta.y + 3.0 * std::cos(a - 0.5)}, col);
                    linea(t, punta, {punta.x - 3.0 * std::sin(a + 0.5), punta.y + 3.0 * std::cos(a + 0.5)}, col);
                }
                yb += 12;
                continue;
            }
            if (x->tipo != "ordine_falso" && x->tipo != "spia" && !incidente && (x->tipo != "disturbo_radio" || zona)) continue;
            const std::uint8_t col = colore_intervento(x->tipo);
            ordine_attivo = ordine_attivo || x->tipo == "ordine_falso";
            simboli::disegna_intervento(t, {g.x + 10.0, yb + 3.0}, famiglia_intervento(x->tipo), col);
            testo_con_alone(t, g.x + 18, yb, taglia(f, maiu(x->riga_it), g.w / 2), f, col, colori::NERO);
            yb += 12;
            if (x->tipo == "ordine_falso" && x->ha_punto) {   // il «vai a» dell'ordine falso
                const Vec2 c = g.a_tela(x->punto);
                simboli::disegna_intervento(t, c, famiglia_intervento(x->tipo), col);
                testo_con_alone(t, static_cast<int>(c.x) + 8, static_cast<int>(c.y) - 3, "VAI A (FALSO)", f, col, colori::NERO);
            }
        }
        // la polynya dopo la fine dell'intervento: il ghiaccio torna com'era nella stessa transizione, fuori da interventi_a
        for (const PolynyaVista& z : polynye) {
            if (!z.ritorno) continue;
            const std::uint8_t col = colore_intervento("polynya");
            const Vec2 c = g.a_tela(z.centro);
            const double rr = z.raggio_m / mpp_;
            for (int k = 0; k < 48; ++k) {
                const double a0 = k * 2.0 * PI / 48.0, a1 = (k + 1) * 2.0 * PI / 48.0;
                linea(t, {c.x + rr * std::cos(a0), c.y + rr * std::sin(a0)}, {c.x + rr * std::cos(a1), c.y + rr * std::sin(a1)}, col);
            }
            simboli::disegna_intervento(t, c, famiglia_intervento("polynya"), col);
            testo_con_alone(t, static_cast<int>(c.x) + 8, static_cast<int>(c.y) - 3, z.apri ? "POLYNYA SI RICHIUDE" : "POLYNYA SI RIAPRE", f, col, colori::NERO);
        }
        std::map<int, std::string> presa;   // l'ultima reazione di ogni unita' all'ordine falso
        std::map<int, std::string> attribuito;   // la false flag: a chi la parte colpita da' la colpa (due minuti)
        std::vector<const EventoTraccia*> localizzazioni;
        std::set<int> persi;   // chi ha perso un messaggio nel disturbo, nell'ultimo minuto
        for (const EventoTraccia& e : tr_.registro) {
            if (e.tempo > t_ + 0.5 || e.tipo != "reazione_ia") continue;
            if (e.classe_arma == "messaggio_perso" && t_ - e.tempo <= 60.0) persi.insert(e.unita);
            else if (e.classe_arma.rfind("ordine_", 0) == 0) presa[e.unita] = e.classe_arma;
            else if (e.classe_arma == "localizza" && t_ - e.tempo <= TOLTA_S) localizzazioni.push_back(&e);
            else if (e.classe_arma.rfind("attribuisce_", 0) == 0 && t_ - e.tempo <= TOLTA_S) attribuito[e.unita] = e.classe_arma.substr(12);
        }
        for (const auto& [id, a] : attribuito)
            if (const auto p = pos_unita(id)) {
                const Vec2 q = g.a_tela(*p);
                const std::string chi = a == "usa" ? "NATO" : a == "urss" ? "PATTO" : a == "ignoto" ? "NESSUNO" : maiu(a);
                testo_con_alone(t, static_cast<int>(q.x) - 10, static_cast<int>(q.y) + 17, "DA' LA COLPA A " + chi, f, col_falso, colori::NERO);
            }
        if (ordine_attivo)
            for (const auto& [id, r] : presa) {
                const auto p = pos_unita(id);
                if (!p) continue;
                const auto [testo, col] = r == "ordine_creduto"       ? std::pair{"CREDE", std::uint8_t{12}}   // 12: rosso chiaro
                                          : r == "ordine_ignorato"     ? std::pair{"IGNORA", std::uint8_t{colori::VERDE_CHIARO}}
                                          : r == "ordine_verifica"     ? std::pair{"VERIFICA", std::uint8_t{colori::GIALLO}}
                                          : r == "ordine_smascherato"  ? std::pair{"SMASCHERA", std::uint8_t{colori::VERDE_CHIARO}}
                                                                       : std::pair{"NON RICEVUTO", std::uint8_t{colori::GRIGIO}};
                const Vec2 q = g.a_tela(*p);
                testo_con_alone(t, static_cast<int>(q.x) - 10, static_cast<int>(q.y) + 7, testo, f, col, colori::NERO);
            }
        for (const int id : persi)
            if (const auto p = pos_unita(id)) {
                const Vec2 q = g.a_tela(*p);
                testo_con_alone(t, static_cast<int>(q.x) - 10, static_cast<int>(q.y) + 27, "MESSAGGIO PERSO", f, colori::GRIGIO, colori::NERO);
            }
        for (const EventoTraccia* e : localizzazioni) {
            const auto a = pos_unita(e->unita), b = pos_unita(e->altra);
            if (e->unita < 0 && b) {   // la rete strategica: niente linea, il suo fix sbaglia di 65-185 km; il segno sull'unita'
                const Vec2 qb = g.a_tela(*b);
                testo_con_alone(t, static_cast<int>(qb.x) - 10, static_cast<int>(qb.y) - 16, "DF DELLA RETE", f, colori::GIALLO, colori::NERO);
                continue;
            }
            if (!a || !b) continue;
            const Vec2 qa = g.a_tela(*a), qb = g.a_tela(*b);
            linea(t, qa, qb, colori::GIALLO, 2);
            testo_con_alone(t, static_cast<int>((qa.x + qb.x) / 2) + 4, static_cast<int>((qa.y + qb.y) / 2) - 3, "DF", f, colori::GIALLO, colori::NERO);
        }
    }
    // la scala: 10 miglia, in basso a sinistra (nel replay a destra: a sinistra c'e' la legenda)
    const double px = 10.0 * MIGLIO / mpp_;
    const int sx = vivo_ ? g.x + 8 : g.x + g.w - 8 - static_cast<int>(px), sy = g.y + g.h - 8;
    if (px < g.w - 20) {
        t.linea_orizzontale(sx, sx + static_cast<int>(px), sy, colori::BIANCO);
        testo_con_alone(t, sx, sy - 10, "10 NM", f, colori::BIANCO, colori::NERO);
    }
    t.togli_ritaglio();
}

void SchermataReplay::disegna_lettura(Tela& t, const Font& g, Rettangolo d) const {
    // gli ultimi quattro eventi fino all'istante (nel quadro solo quelli che il giocatore sapeva), l'ultimo in negativo
    std::vector<const EventoTraccia*> ultimi;
    for (const auto& e : segni_)
        if (e.tempo <= t_ + 0.5 && (lettura_ == Lettura::verita || e.noto)) ultimi.push_back(&e);
    if (ultimi.size() > 4) ultimi.erase(ultimi.begin(), ultimi.end() - 4);
    int y = d.y;
    for (const EventoTraccia* e : ultimi) {
        const bool qui = e == ultimi.back();
        if (qui) t.rettangolo_pieno(d.x, y - 1, d.w, g.righe() + 2, fosforo(1));
        const bool rosso = colore_evento(e->tipo) == colori::ROSSO;
        scrivi_q(t, g, d.x + 2, y, orologio(e->tempo).substr(2, 5), qui ? fosforo(7) : fosforo(4));
        scrivi_q(t, g, d.x + 36, y, taglia_q(g, maiu(e->testo_it.empty() ? e->tipo : e->testo_it), d.w - 38),
                 rosso ? std::uint8_t{12} : qui ? fosforo(7) : fosforo(5));   // 12: il rosso chiaro di lanci, colpi e danni
        y += 10;
    }
    if (ultimi.empty()) scrivi_q(t, g, d.x + 2, y, "NESSUN EVENTO", fosforo(3));
    y = d.y + 43;
    // la quota degli ultimi 10 minuti: il battello (verde) e il bersaglio (rosso), i termoclini a tratti, il fondale
    // sotto il battello (solo dove entra nella scala); la scala segue le quote, non il fondo
    const IstanteTraccia ora = tr_.istante(t_);
    const int bersaglio = ora.app.bersaglio >= 0 ? ora.app.bersaglio : ora.bersaglio_tma;
    const auto campioni = [&](int id) -> const std::vector<std::pair<double, StatoUnita>>* {
        const auto it = tr_.unita.find(id);
        return it == tr_.unita.end() ? nullptr : &it->second;
    };
    const auto* gc = campioni(giocatore_id_);
    const auto* b = bersaglio >= 0 && bersaglio != giocatore_id_ ? campioni(bersaglio) : nullptr;
    scrivi_q(t, g, d.x + 2, y, "QUOTA 10 MIN", fosforo(4));
    // nel vetrino il profilo del suono qui sotto il battello, a destra, sulla stessa scala delle quote
    const StatoUnita* con_suono = suono_base_ ? giocatore(ora) : nullptr;
    const std::optional<ProfiloSuono> suono = con_suono ? std::optional<ProfiloSuono>(profilo_spettatore(*suono_base_, tr_.interventi, con_suono->pos, t_)) : std::nullopt;
    {
        std::string noi, suo;
        if (const auto* gi = giocatore(ora)) noi = "NOI " + std::to_string(static_cast<int>(std::lround(gi->quota))) + " M";
        if (b)
            for (const auto& x : ora.unita)
                if (x.id == bersaglio) suo = "BERS. " + std::to_string(static_cast<int>(std::lround(x.quota)));
        const int ws = suo.empty() ? 0 : largo_q(g, suo);
        if (!suo.empty()) scrivi_q(t, g, d.x + d.w - 2 - ws, y, suo, 12);
        if (!noi.empty()) scrivi_q(t, g, d.x + d.w - 2 - ws - (ws ? largo_q(g, "  ") : 0) - largo_q(g, noi), y, noi, colori::VERDE_CHIARO);
        if (suono) {   // fra il titolo e le quote, quanto ci sta
            const int da = d.x + 2 + largo_q(g, "QUOTA 10 MIN  "), a_x = d.x + d.w - 2 - ws - (ws ? largo_q(g, "  ") : 0) - largo_q(g, noi) - largo_q(g, " ");
            std::string riga = riga_strato(*suono, *suono_base_);
            if (largo_q(g, riga) > a_x - da) riga = riga.substr(0, riga.find(" ("));   // senza com'era, se non ci sta
            scrivi_q(t, g, da, y, taglia_q(g, riga, a_x - da), colori::GIALLO);
        }
    }
    y += 11;
    const int striscia = suono ? 48 : 0;
    const Rettangolo p{d.x + 30, y + 3, d.w - 34 - striscia, d.y + d.h - y - 6};
    if (p.w <= 2 || p.h <= 2) return;
    // la scala: 1,4 volte la quota piu' profonda della finestra, almeno 100 m, arrotondata a 50
    double massima = 70.0;
    for (const auto* v : {gc, b})
        if (v)
            for (const auto& [tc, x] : *v)
                if (tc >= t_ - PROFILO_S && tc <= t_ && x.categoria != "aereo") massima = std::max(massima, x.quota);
    const double fondo = std::ceil(massima * 1.4 / 50.0) * 50.0;
    const auto y_di = [&](double m) { return p.y + std::clamp(m / fondo, 0.0, 1.0) * (p.h - 1); };
    const auto x_t = [&](double tc) { return p.x + (tc - (t_ - PROFILO_S)) / PROFILO_S * (p.w - 1); };
    // il fondale sotto il battello, una colonna per pixel (interpolando fra i campioni)
    if (gc && fondale_)
        for (int x = 0; x < p.w; ++x) {
            const double tc = t_ - PROFILO_S + PROFILO_S * x / (p.w - 1);
            if (tc < 0.0) continue;
            const auto dopo = std::lower_bound(gc->begin(), gc->end(), tc, [](const auto& c, double v) { return c.first < v; });
            if (dopo == gc->end() || dopo == gc->begin()) continue;
            const auto prima = std::prev(dopo);
            const double s = (tc - prima->first) / std::max(1e-6, dopo->first - prima->first);
            const Vec2 pos{prima->second.pos.x + (dopo->second.pos.x - prima->second.pos.x) * s,
                           prima->second.pos.y + (dopo->second.pos.y - prima->second.pos.y) * s};
            const double fd = fondale_->profondita(pos);
            if (!std::isfinite(fd) || fd <= 0.0 || fd >= fondo) continue;
            t.linea_verticale(p.x + x, static_cast<int>(y_di(fd)), p.y + p.h - 1, fosforo(1));
        }
    // la superficie e il fondo della scala, a punti, con le etichette dentro il riquadro
    for (const double m : {0.0, fondo}) {
        const int yy = static_cast<int>(y_di(m));
        for (int x = p.x; x < p.x + p.w; x += 3) t.pixel(x, yy, fosforo(2));
        const std::string n = std::to_string(static_cast<int>(m)) + (m > 0.0 ? "M" : " M");
        scrivi_q(t, g, d.x + 2, m > 0.0 ? yy - g.righe() + 1 : yy, n, fosforo(3));
    }
    for (const double m : termoclini_)
        if (m < fondo) {
            const int yy = static_cast<int>(y_di(m));
            for (int x = p.x; x < p.x + p.w; x += 4) t.linea_orizzontale(x, x + 1, yy, fosforo(3));
        }
    const auto curva = [&](const std::vector<std::pair<double, StatoUnita>>* v, std::uint8_t col) {
        if (!v) return;
        std::optional<Vec2> prima;
        for (const auto& [tc, x] : *v) {
            if (tc < t_ - PROFILO_S || tc > t_ || x.categoria == "aereo") continue;
            const Vec2 qq{x_t(tc), y_di(std::max(0.0, x.quota))};
            if (prima) linea(t, *prima, qq, col);
            prima = qq;
        }
    };
    curva(b, 12);
    curva(gc, static_cast<std::uint8_t>(colori::VERDE_CHIARO));
    if (suono) curva_suono(t, {p.x + p.w + 2, p.y, striscia - 2, p.h}, *suono, *suono_base_, fondo, con_suono->quota, static_cast<std::uint8_t>(colori::VERDE_CHIARO));
}

void SchermataReplay::disegna_timeline(Tela& t, const Font& g) const {   // dal vivo (il replay ha la sua fascia)
    // la fascia del tempo del kit: a sinistra l'istante e l'evento piu' vicino (a larghezza fissa, la barra non si
    // sposta), a destra ADESSO; le tacche degli eventi (colore e altezza: quadro::fascia_tempo)
    const double fine = std::max(1e-6, fine_linea());
    const EventoTraccia* vicino = nullptr;
    for (const auto& e : segni_)
        if (e.tempo <= t_ + 0.5 && (!vicino || e.tempo > vicino->tempo)) vicino = &e;
    std::string sinistra = orologio(t_);
    if (vicino) sinistra += " " + maiu(vicino->testo_it.empty() ? vicino->tipo : vicino->testo_it);
    sinistra = taglia_q(g, sinistra, SINISTRA_W);
    for (int k = 0; k < 200 && largo_q(g, sinistra + " ") <= SINISTRA_W; ++k) sinistra += ' ';
    barra_ = quadro::fascia_tempo(t, fascia_c_, g, sinistra, "ADESSO", t_ / fine, tacche_eventi(segni_, fine, false));
    const Rettangolo b = barra_;
    if (tr_.ripresa_da >= 0.0) {   // la partita ripresa: da li' il giocatore comandava di nuovo
        const int x = static_cast<int>(x_di(tr_.ripresa_da));
        t.linea_verticale(x, b.y, b.y + b.h - 1, colori::VERDE_CHIARO);
    }
    if (indietro_) {   // guardando indietro: dov'e' adesso
        const int x = static_cast<int>(x_di(adesso_));
        t.linea_verticale(x, b.y, b.y + b.h - 1, colori::GIALLO);
    }
    // gli interventi sopra la barra: il triangolo all'applicazione (rosso se e' fallito), la durata nella riga sotto;
    // i programmati (dal pannello, non ancora applicati) col triangolo vuoto
    for (const RecordIntervento& x : tr_.interventi) {
        const int px = static_cast<int>(x_di(x.t));
        if (x.fase == RecordIntervento::FASE_APPLICATO) {
            const std::uint8_t col = colore_intervento(x.tipo);
            triangolo(t, px, b.y - 2, col);
            if (x.durata_s > 0.0) t.linea_orizzontale(px, static_cast<int>(x_di(std::min(x.t + x.durata_s, fine))), b.y + b.h - 1, col);
        } else if (x.fase == RecordIntervento::FASE_FALLITO) {
            triangolo(t, px, b.y - 2, colori::ROSSO);
        }
    }
    for (const auto& q : richieste_)
        if (!q.subito && q.tempo > adesso_) triangolo(t, static_cast<int>(x_di(q.tempo)), b.y - 2, colore_intervento(q.tipo), false);
}

}  // namespace m688
