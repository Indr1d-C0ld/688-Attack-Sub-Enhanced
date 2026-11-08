#include "scena_prova.hpp"

#include <cmath>
#include <functional>
#include <algorithm>
#include <cstdio>
#include <stdexcept>

#include "grafica/etichetta_tasti.hpp"
#include "grafica/palette.hpp"
#include "ripetitori.hpp"
#include "nucleo/comando.hpp"
#include "nucleo/sole.hpp"

namespace m688 {

namespace {

struct Disposizione {
    const char* classe;
    double rilevamento, distanza_mn, rotta, nodi;
};

// Un gruppo sovietico che incrocia verso ovest a nord del sottomarino e traffico NATO a sud
constexpr Disposizione DISPOSIZIONE[] = {
    {"kirov", 20, 3.8, 250, 16},      {"krivak2", 5, 2.4, 255, 16},   {"slava", 45, 6.0, 250, 16},
    {"kara", 62, 9.5, 245, 16},       {"nanuchka", 330, 3.2, 200, 24}, {"koni", 290, 5.5, 120, 14},
    {"perry", 160, 2.2, 80, 12},      {"kidd", 185, 3.5, 75, 14},     {"sacramento", 205, 5.0, 75, 14},
    {"nimitz", 215, 7.5, 75, 14},     {"iowa", 140, 6.5, 80, 14},     {"spruance", 175, 2.8, 75, 14},
    // traffico civile
    {"petroliera", 100, 3.0, 20, 13}, {"mercantile", 245, 2.0, 160, 14}, {"cargo", 120, 5.5, 200, 16},
};

}  // namespace

void crea_scena_prova(Mondo& mondo, const std::map<std::string, ClasseNave>& classi) {
    Nave proprio;
    proprio.id = ID_PROPRIO;
    proprio.classe = "688";
    proprio.rotta = proprio.rotta_ordinata = 0.0;
    proprio.velocita = proprio.velocita_ordinata = 3.0 * NODO;   // velocita' di quota periscopica
    mondo.aggiungi(proprio);

    int id = 1;
    for (const auto& d : DISPOSIZIONE) {
        const auto it = classi.find(d.classe);
        if (it == classi.end()) continue;
        Nave n;
        n.id = id++;
        n.classe = d.classe;
        const double r = d.distanza_mn * MIGLIO, b = radianti(d.rilevamento);
        n.pos = {r * std::sin(b), r * std::cos(b)};
        n.rotta = n.rotta_ordinata = d.rotta;
        n.velocita = n.velocita_ordinata = std::min(d.nodi, it->second.velocita_max_nodi) * NODO;
        mondo.aggiungi(n);
    }
}

void colpo_di_prova(DanniNave& n, const std::string& caso) {
    const auto& st = n.corpi().front().stazioni;
    const double x0 = st.front().x, x1 = st.back().x, L = x1 - x0, xm = (x0 + x1) / 2;
    const double chiglia = st[st.size() / 2].z_chiglia;
    auto siluro = [&](double x, double z, bool contatto, double kg) {
        Esplosione e;
        e.x = x; e.y = contatto ? -1.0 : 0.0; e.z = z; e.contatto = contatto; e.tnt_kg = kg;
        n.colpo(e);
    };
    if (caso == "siluro_chiglia") siluro(xm, chiglia - 6.0, false, 544);
    else if (caso == "siluro_fianco") siluro(xm - 0.08 * L, chiglia * 0.7, true, 300);
    else if (caso == "siluro_prua") siluro(x1 - 0.1 * L, chiglia * 0.7, true, 300);
    else if (caso == "siluro_poppa") siluro(x0 + 0.08 * L, chiglia * 0.6, true, 300);
    else if (caso == "due_siluri") { siluro(xm + 0.15 * L, chiglia * 0.7, true, 300); siluro(xm - 0.15 * L, chiglia * 0.7, true, 300); }
    else if (caso == "missile") {
        Esplosione e;
        e.tipo = Esplosione::Tipo::missile; e.x = xm + 0.05 * L; e.y = -1.0; e.z = 2.5; e.tnt_kg = 165;
        n.colpo(e);
    } else if (caso == "deposito" || caso == "coperta") {   // lo scoppio secondario subito (danni critici, fase 4)
        if (!n.esplosione_di_prova(caso == "coperta")) throw std::runtime_error("la nave non ha " + std::string(caso == "coperta" ? "lanciatori carichi in coperta" : "depositi"));
    } else if (caso == "carico_in_fiamme") {   // il carico liquido che brucia, poi due siluri: la chiazza in fiamme
        for (std::size_t j = 0; j < n.corpi().front().celle.size(); ++j)
            if (n.corpi().front().celle[j].dati.uso == "carico") n.imposta_fuoco(j, 0.9);
        siluro(xm + 0.15 * L, chiglia * 0.7, true, 300);
        siluro(xm - 0.15 * L, chiglia * 0.7, true, 300);
    } else throw std::runtime_error("colpo sconosciuto (siluro_chiglia, siluro_fianco, siluro_prua, siluro_poppa, due_siluri, missile, "
                                    "deposito, coperta, carico_in_fiamme): " + caso);
}

namespace {

std::vector<AreaAttiva>* aree_ = nullptr;   // aree attive del pannello in disegno (mouse)
void area(int x, int y, int w, int h, std::string comando) {
    if (aree_) aree_->push_back({x, y, w, h, std::move(comando)});
}

// Pannello Manovra: telegrafo a 7 posizioni, timone e barra a scatti, pilota automatico, stato del battello
// Manovra dell'Enhanced (C6a, C6c): campane del telegrafo, barra in gradi, assetto, giri e cavitazione
void disegna_manovra_enhanced(Tela& t, const Font& f, const StatoPannello& s, const StatoManovra& m) {
    const int y0 = ALTEZZA_VISTA;
    char riga[200];
    for (int p = 0; p < PostoEnhanced::CAMPANE; ++p) {
        const int x = 12 + p * 69;
        const bool attiva = p == m.campana;
        t.rettangolo_pieno(x, y0 + 20, 67, 17, attiva ? colori::GIALLO : colori::NERO);
        const std::string_view n = PostoEnhanced::nome_campana(p);
        f.scrivi(t, x + 33 - f.larghezza(n, 1) / 2, y0 + 25, n, attiva ? colori::NERO : colori::GRIGIO, 1);
    }
    // barra del timone: -35 ... +35 a scatti di 5, l'indice sull'angolo vero
    f.scrivi(t, 12, y0 + 46, "BARRA", colori::GRIGIO, 1);
    const int xs = 70, passo = 14, pos = static_cast<int>(std::lround(m.barra_gradi / 5.0));
    for (int k = -7; k <= 7; ++k) {
        const int x = xs + (k + 7) * passo;
        t.linea_verticale(x, y0 + 46 + (k == 0 ? 0 : 3), y0 + 56, colori::GRIGIO);
        if (k == pos) t.rettangolo_pieno(x - 3, y0 + 45, 7, 13, k == 0 ? colori::BIANCO : colori::GIALLO);
    }
    std::snprintf(riga, sizeof riga, "%+.0f°   GIRI %.0f%%   ASSETTO %+.1f°", m.barra_gradi, m.giri * 100.0, m.assetto);
    for (char* c = riga; *c; ++c) if (*c == '%') *c = ' ';   // il font non ha il segno di percentuale
    f.scrivi(t, 300, y0 + 46, riga, colori::BIANCO, 1);
    std::snprintf(riga, sizeof riga, "CAVITAZIONE OLTRE %.0f NODI%s", m.velocita_cavitazione_kn, m.cavita ? "   CAVITA!" : "");
    f.scrivi(t, 300, y0 + 58, riga, m.cavita ? colori::GIALLO : colori::GRIGIO, 1);
    char quota[48];
    if (m.cambia_quota) std::snprintf(quota, sizeof quota, "%.0f M -> %.0f M", m.quota_m, m.quota_ordinata_m);
    else std::snprintf(quota, sizeof quota, "%.0f M", m.quota_m);
    std::snprintf(riga, sizeof riga, "ROTTA %03d°   VELOCITÀ %.1f NODI   QUOTA %s%s%s",
                  static_cast<int>(std::lround(normalizza_360(m.rotta))) % 360, m.velocita_kn, quota,
                  m.periscopio ? "   PERISCOPIO SU" : "", m.antenna ? (m.radar ? "   ANTENNA SU, RADAR ACCESO" : "   ANTENNA SU") : "");
    f.scrivi(t, 12, y0 + 74, riga, colori::BIANCO, 1);
    const bool aiuto = s.messaggio.empty();
    const std::string ultima = aiuto ? "TASTIERINO 4/6 BARRA 5°, 5 CENTRO, 8/2 QUOTA 15 M, 0 TIENE   +/- CAMPANE   CTRL+P PERISCOPICA"
                                     : s.messaggio;
    const int scala_ultima = !aiuto && f.larghezza(ultima, 2) <= LARGHEZZA_TELA - 24 ? 2 : 1;
    f.scrivi(t, 12, y0 + 96, ultima, aiuto ? colori::GRIGIO : colori::VERDE_CHIARO, scala_ultima, 1, !aiuto);
}

void disegna_manovra(Tela& t, const Font& f, const VistaPeriscopio& v, const StatoPannello& s, const StatoManovra& m) {
    const int y0 = ALTEZZA_VISTA;
    t.rettangolo_pieno(0, y0, LARGHEZZA_TELA, ALTEZZA_TELA - y0, colori::GRIGIO_SCURO);
    t.linea_orizzontale(0, LARGHEZZA_TELA - 1, y0, colori::GRIGIO);
    char riga[160];
    const int ore = static_cast<int>(s.tempo_s / 3600.0) % 24, minuti = static_cast<int>(s.tempo_s / 60.0) % 60,
              secondi = static_cast<int>(s.tempo_s) % 60;
    std::snprintf(riga, sizeof riga, "%s  %s", s.missione.c_str(), v.alto_ingrandimento ? "6x" : "1,5x");
    f.scrivi(t, 12, y0 + 5, riga, colori::CIANO_CHIARO, 1);
    std::snprintf(riga, sizeof riga, "ora %02d:%02d:%02dZ  tempo x%g", ore, minuti, secondi, s.compressione);
    f.scrivi(t, LARGHEZZA_TELA - 12 - f.larghezza(riga, 1), y0 + 5, riga, colori::CIANO_CHIARO, 1);
    if (m.enhanced) { disegna_manovra_enhanced(t, f, s, m); return; }
    // telegrafo
    f.scrivi(t, 12, y0 + 27, "MACCHINA", colori::GRIGIO, 1);
    for (int p = 0; p < PostoDiComando::TELEGRAFO_POSIZIONI; ++p) {
        const int x = 88 + p * 58;
        const bool attiva = p == m.telegrafo;
        t.rettangolo_pieno(x, y0 + 22, 56, 17, attiva ? colori::GIALLO : colori::NERO);
        const std::string_view n = PostoDiComando::nome_telegrafo(p);
        f.scrivi(t, x + 28 - f.larghezza(n, 1) / 2, y0 + 27, n, attiva ? colori::NERO : colori::GRIGIO, 1);
    }
    std::snprintf(riga, sizeof riga, "PILOTA %s  PUNTI %d", m.pilota ? "ON" : "OFF", m.punti);
    f.scrivi(t, 504, y0 + 27, riga, m.pilota ? colori::VERDE_CHIARO : colori::GRIGIO, 1);
    // timone e barra: scale a scatti con l'indice sulla posizione corrente
    auto scala = [&](int x0, int y, const char* nome, int scatti, int pos) {
        f.scrivi(t, x0, y, nome, colori::GRIGIO, 1);
        const int xs = x0 + 70, passo = 16;
        for (int k = -scatti; k <= scatti; ++k) {
            const int x = xs + (k + scatti) * passo;
            t.linea_verticale(x, y + (k == 0 ? 0 : 3), y + 10, colori::GRIGIO);
            if (k == pos) t.rettangolo_pieno(x - 3, y - 1, 7, 13, k == 0 ? colori::BIANCO : colori::GIALLO);
        }
    };
    scala(12, y0 + 50, "TIMONE", PostoDiComando::TIMONE_MAX, m.timone);
    scala(330, y0 + 50, "BARRA", PostoDiComando::BARRA_MAX, m.barra);
    // stato del battello (quote in piedi, come gli strumenti dell'originale)
    char quota[48];
    if (m.cambia_quota) std::snprintf(quota, sizeof quota, "%.0f FT -> %.0f FT", m.quota_m / 0.3048, m.quota_ordinata_m / 0.3048);
    else std::snprintf(quota, sizeof quota, "%.0f FT", m.quota_m / 0.3048);
    std::snprintf(riga, sizeof riga, "ROTTA %03d°   VELOCITÀ %.0f NODI   QUOTA %s%s%s",
                  static_cast<int>(std::lround(normalizza_360(m.rotta))) % 360, m.velocita_kn, quota,
                  m.periscopio ? "   PERISCOPIO SU" : "", m.antenna ? (m.radar ? "   ANTENNA SU, RADAR ACCESO" : "   ANTENNA SU") : "");
    f.scrivi(t, 12, y0 + 74, riga, colori::BIANCO, 1);
    // ultima riga: il messaggio di bordo o l'aiuto dei tasti
    const bool aiuto = s.messaggio.empty();
    const std::string ultima = aiuto ? "TASTIERINO 4/6 TIMONE, 5 CENTRO, 8/2 BARRA, 0 QUOTA   +/- MACCHINA   . PILOTA   "
                                       "CTRL+P PERISCOPICA   PAG SU/GIU TEMPO"
                                     : s.messaggio;
    const int scala_ultima = !aiuto && f.larghezza(ultima, 2) <= LARGHEZZA_TELA - 24 ? 2 : 1;
    f.scrivi(t, 12, y0 + 96, ultima, aiuto ? colori::GRIGIO : colori::VERDE_CHIARO, scala_ultima, 1, !aiuto);
}

// Intestazione comune dei pannelli delle stazioni: nome, missione, ora e compressione
void intestazione(Tela& t, const Font& f, const VistaPeriscopio& v, const StatoPannello& s, const char* stazione) {
    const int y0 = ALTEZZA_VISTA;
    t.rettangolo_pieno(0, y0, LARGHEZZA_TELA, ALTEZZA_TELA - y0, colori::GRIGIO_SCURO);
    t.linea_orizzontale(0, LARGHEZZA_TELA - 1, y0, colori::GRIGIO);
    char riga[160];
    const int ore = static_cast<int>(s.tempo_s / 3600.0) % 24, minuti = static_cast<int>(s.tempo_s / 60.0) % 60,
              secondi = static_cast<int>(s.tempo_s) % 60;
    std::snprintf(riga, sizeof riga, "%s  %s  %s", stazione, s.missione.c_str(), v.alto_ingrandimento ? "6x" : "1,5x");
    f.scrivi(t, 12, y0 + 5, riga, colori::CIANO_CHIARO, 1);
    std::snprintf(riga, sizeof riga, "ora %02d:%02d:%02dZ  tempo x%g", ore, minuti, secondi, s.compressione);
    f.scrivi(t, LARGHEZZA_TELA - 12 - f.larghezza(riga, 1), y0 + 5, riga, colori::CIANO_CHIARO, 1);
}

// Barra del bersaglio: titolo e, con la traccia solida, distanza, rotta, velocita' e quota
void disegna_barra(Tela& t, const Font& f, const StatoPannello& s, int y) {
    t.rettangolo_pieno(8, y - 2, LARGHEZZA_TELA - 16, 26, colori::NERO);
    f.scrivi(t, 14, y + 1, s.barra_titolo.empty() ? "NESSUN BERSAGLIO" : s.barra_titolo, colori::CIANO_CHIARO, 1);
    if (!s.barra_dati.empty()) f.scrivi(t, 14, y + 12, s.barra_dati, colori::BIANCO, 1);
}

// Ultima riga: il messaggio di bordo
void disegna_messaggio(Tela& t, const Font& f, const StatoPannello& s, int y) {
    if (s.messaggio.empty()) return;
    const int w = LARGHEZZA_TELA - 24;
    if (s.due_righe && f.larghezza(s.messaggio, 1) > w) {   // Enhanced: a capo sulla parola piu' vicina alla meta'
        std::size_t taglio = std::string::npos;
        for (std::size_t i = s.messaggio.find(' '); i != std::string::npos; i = s.messaggio.find(' ', i + 1))
            if (f.larghezza(s.messaggio.substr(0, i), 1) <= w) taglio = i;
        if (taglio != std::string::npos) {
            f.scrivi(t, 12, y - 2, s.messaggio.substr(0, taglio), colori::VERDE_CHIARO, 1, 1, true);
            f.scrivi(t, 12, y + 8, s.messaggio.substr(taglio + 1), colori::VERDE_CHIARO, 1, 1, true);
            return;
        }
    }
    const int scala = f.larghezza(s.messaggio, 2) <= w ? 2 : 1;
    f.scrivi(t, 12, y, s.messaggio, colori::VERDE_CHIARO, scala, 1, true);
}

// Pannelli delle consolle della Fase 3 (Enhanced, docs/ricerca/19): l'intestazione di oggi, il pannello del kit in
// 88 righe, sotto il messaggio di bordo o l'aiuto; le aree del kit con un comando diventano aree attive
void consolle(Tela& t, const Font& f, const VistaPeriscopio& v, const StatoPannello& s, const char* nome, const char* aiuto,
              const std::function<void(Rettangolo, std::vector<AreaPulto>*)>& pannello) {
    const int y0 = ALTEZZA_VISTA;
    intestazione(t, f, v, s, nome);
    std::vector<AreaPulto> aree;
    pannello({0, y0 + 14, LARGHEZZA_TELA, 88}, &aree);
    for (const auto& a : aree)
        if (!a.comando.empty()) area(a.r.x, a.r.y, a.r.w, a.r.h, a.comando);
    if (s.messaggio.empty()) f.scrivi(t, 12, y0 + 106, aiuto, colori::GRIGIO, 1);
    else disegna_messaggio(t, f, s, y0 + 103);   // a scala 2 arriva al fondo della tela
}

void disegna_manovra_consolle(Tela& t, const Font& f, const VistaPeriscopio& v, const StatoPannello& s, const StatoManovra& m,
                              const TemaStazione& tema) {
    DatiGoverno d;
    d.rotta = d.rotta_ordinata = m.rotta;
    d.quota_m = m.quota_m;
    d.quota_ordinata_m = m.cambia_quota ? m.quota_ordinata_m : m.quota_m;
    d.quota_max_m = m.quota_max_m;
    d.velocita_kn = m.velocita_kn;
    d.giri = m.giri;
    d.barra_gradi = m.barra_gradi;
    d.piani_gradi = m.piani_gradi;
    d.assetto_gradi = m.assetto;
    d.telegrafo = m.telegrafo;
    d.campana = m.campana;
    d.pilota = m.pilota;
    d.cavita = m.cavita;
    d.cambia_quota = m.cambia_quota;
    d.periscopio = m.periscopio;
    d.antenna = m.antenna;
    d.radar = m.radar;
    d.punti_rotta = m.punti;
    d.modo_rotta = static_cast<DatiGoverno::Modo>(std::clamp(m.modo_rotta, 0, 2));   // Alfa: Boksit
    d.modo_quota = static_cast<DatiGoverno::Modo>(std::clamp(m.modo_quota, 0, 2));
    d.tan = m.tan;
    d.sfoga = m.sfoga;
    d.guasto_casse = m.guasto_casse;
    d.quota_periscopica = m.quota_periscopica;
    d.schiera = m.schiera;
    consolle(t, f, v, s, "MANOVRA", "TASTIERINO 4/6 BARRA, 5 CENTRO, 8/2 QUOTA, 0 TIENE   +/- CAMPANE   CTRL+P PERISCOPICA   CLIC SUI COMANDI",
             [&](Rettangolo r, std::vector<AreaPulto>* a) { pannello_governo(t, r, tema, *s.consolle, d, a); });
}

void disegna_armi_consolle(Tela& t, const Font& f, const VistaPeriscopio& v, const StatoPannello& s, const StatoArmi& a,
                           const TemaStazione& tema) {
    DatiArmi d;
    for (const auto& tb : a.tubi) {   // stati: 0 vuoto, 2 in armamento, 3 armato, 4 in ricarica, 5 carico
        DatiArmi::Tubo x;
        if (tb.stato != 0) x.arma = tb.arma;
        x.pronto = x.porta = tb.stato == 3;
        x.allagato = tb.stato == 2 || tb.stato == 3;
        x.ricarica = tb.stato == 4 ? std::max(tb.ricarica, 0.0) : -1.0;
        x.sezione = tb.sezione;
        d.tubi.push_back(std::move(x));
    }
    d.selezionato = a.selezionato - 1;
    // lo schermo del Mk 81 o di Akkord: impostazioni del tubo, siluro filoguidato, bersaglio, armi in acqua
    if (!a.impostazioni.empty()) d.schermo.push_back(a.impostazioni);
    if (!a.siluro.empty()) d.schermo.push_back("SILURO " + a.siluro + (a.filo.empty() ? "" : "  " + a.filo));
    if (!s.barra_titolo.empty()) d.schermo.push_back(s.barra_titolo);
    d.schermo.push_back("IN ACQUA " + std::to_string(a.in_acqua) + "/" + std::to_string(a.limite) + (a.silenzio ? "  SILENZIO" : ""));
    d.bersagli_akkord = static_cast<int>(std::count_if(a.akkord.begin(), a.akkord.end(), [](const std::string& x) { return !x.empty(); }));
    d.bersaglio_scelto = a.akkord_scelto - 1;   // Alfa: lo slot di Akkord scelto (-1 nessuno)
    d.akkord_guasto = a.akkord_guasto;
    consolle(t, f, v, s, "ARMI", ("CLIC TUBO: RICARICA/ARMA/DISARMA   CTRL+V ARMA   TAB " + tasti_valore() + " IMPOSTA   L LANCIA   S C D Z X FILO").c_str(),
             [&](Rettangolo r, std::vector<AreaPulto>* ar) { pannello_armi(t, r, tema, *s.consolle, d, ar); });
}

// Pannello Armi: i tubi in sezione con l'arma (sagome del kit), sotto il nome e lo stato nei colori dell'originale
// (2 giallo, 3 rosso chiaro; Enhanced: in ricarica ciano, carico grigio), il selezionato bordato di bianco; scorte,
// armi in acqua, bersaglio e siluro filoguidato
void disegna_armi(Tela& t, const Font& f, const VistaPeriscopio& v, const StatoPannello& s, const StatoArmi& a) {
    constexpr int ROSSO_CHIARO = 12;   // colore EGA 12, quello dei tubi armati nell'originale
    const int y0 = ALTEZZA_VISTA;
    intestazione(t, f, v, s, "ARMI");
    char riga[160];
    for (std::size_t i = 0; i < a.tubi.size(); ++i) {
        const int x = 12 + static_cast<int>(i) * 88, y = y0 + 18;
        const auto& tb = a.tubi[i];
        const auto colore = static_cast<std::uint8_t>(tb.stato == 3 ? ROSSO_CHIARO : tb.stato == 2 ? colori::GIALLO
                                                      : tb.stato == 4 ? colori::CIANO : tb.stato == 5 ? colori::GRIGIO : colori::GRIGIO_SCURO);
        if (static_cast<int>(i) + 1 == a.selezionato) t.rettangolo_pieno(x - 2, y - 2, 86, 34, colori::BIANCO);
        t.rettangolo_pieno(x, y, 82, 30, colori::NERO);
        area(x, y, 82, 30, "pulsante_" + std::to_string(i + 1));
        disegna_tubo(t, {x, y, 82, 13}, tb.sezione);
        std::snprintf(riga, sizeof riga, "%zu %s", i + 1, tb.arma.c_str());
        std::string nome = riga;   // il nome dell'arma del carico puo' essere lungo (UGM-84 HARPOON)
        while (f.larghezza(nome, 1) > 80 && nome.size() > 3) nome.pop_back();
        f.scrivi(t, x + 41 - f.larghezza(nome, 1) / 2, y + 14, nome, tb.stato == 0 ? colori::GRIGIO : colori::BIANCO, 1);
        char conto[24];
        std::snprintf(conto, sizeof conto, "RICARICA %d:%02d", static_cast<int>(tb.secondi) / 60, static_cast<int>(tb.secondi) % 60);
        const std::string stato = tb.stato == 3 ? "ARMATO" : tb.stato == 2 ? "ARMAMENTO" : tb.stato == 5 ? "CARICO"
                                  : tb.stato == 4 ? (tb.coda ? std::string("IN CODA") : std::string(conto)) : "VUOTO";
        f.scrivi(t, x + 41 - f.larghezza(stato, 1) / 2, y + 21, stato, colore, 1);
    }
    std::string scorte = "SCORTE";
    for (const auto& [nome, n] : a.scorte) scorte += "  " + nome + " " + std::to_string(n);
    std::snprintf(riga, sizeof riga, "%s     IN ACQUA %d/%d%s", scorte.c_str(), a.in_acqua, a.limite, a.silenzio ? "   SILENZIO A BORDO" : "");
    f.scrivi(t, 12, y0 + 50, riga, colori::BIANCO, 1);
    std::snprintf(riga, sizeof riga, "SILURO %s%s%s", a.siluro.empty() ? "-" : a.siluro.c_str(), a.filo.empty() ? "" : "  ",
                  a.filo.c_str());
    f.scrivi(t, LARGHEZZA_TELA - 12 - f.larghezza(riga, 1), y0 + 50, riga, colori::BIANCO, 1);
    if (!a.impostazioni.empty()) {   // Enhanced: le impostazioni del tubo al posto della barra
        f.scrivi(t, 12, y0 + 64, a.impostazioni, colori::GIALLO, 1);
        f.scrivi(t, 12, y0 + 82, (tasti_valore() == "[ ]" ? std::string("CLIC TUBO: RICARICA/ARMA/DISARMA") : std::string("CLIC TUBO")) +   // col tasto lungo, per starci
                                     "  CTRL+V ARMA  TAB " + tasti_valore() + " IMPOSTA  L LANCIA  S C D Z X 8 9 Y FILO  CTRL+S SILENZIO", colori::GRIGIO, 1);
    } else {
        disegna_barra(t, f, s, y0 + 64);
        f.scrivi(t, 12, y0 + 82, "1-7/R/A/M TUBO (2 CLIC ARMA)  L LANCIA  G BERSAGLIO  S SILURO  C CERCA  D DETONA", colori::GRIGIO, 1);
    }
    disegna_messaggio(t, f, s, y0 + 96);
}

// Pannello Sonar: schiera (azione possibile, come il riquadro del 688), filtro, impulso, ANALIZZA
void disegna_sonar(Tela& t, const Font& f, const VistaPeriscopio& v, const StatoPannello& s, const StatoSonar& so) {
    const int y0 = ALTEZZA_VISTA;
    intestazione(t, f, v, s, "SONAR");
    auto pulsante = [&](int x, const char* tasto, const std::string& testo, bool acceso) {
        t.rettangolo_pieno(x, y0 + 20, 150, 21, acceso ? colori::GIALLO : colori::NERO);
        area(x, y0 + 20, 150, 21, std::string("pulsante_") + tasto);
        const std::string r = std::string(tasto) + " " + testo;
        f.scrivi(t, x + 75 - f.larghezza(r, 1) / 2, y0 + 27, r, acceso ? colori::NERO : colori::GRIGIO, 1);
    };
    pulsante(12, "1", so.filtro ? "PASSA ALTO" : "PASSA BANDA", so.filtro);
    pulsante(170, "2", "IMPULSO ATTIVO", false);
    pulsante(328, "3", so.schiera + (so.manovra.empty() ? "" : " (" + so.manovra + ")"), so.ascolto_schiera);
    pulsante(486, "4", "ANALIZZA", false);
    disegna_barra(t, f, s, y0 + 50);
    f.scrivi(t, 12, y0 + 82,
             so.enhanced ? "4 ANALIZZA 6 FASCIO 7 INT 8 AUTO 9 CURS TAB " + tasti_valore() + (so.slot ? " B LIBRERIA W J NOTE S Y C SLOT T K TRACKER I O" : " B LIBRERIA W J NOTE T K TRACKER I O")
                         : "1 FILTRO 2 IMPULSO 3 SCHIERA 4 ANALIZZA G BERSAGLIO",
             colori::GRIGIO, 1);
    disegna_messaggio(t, f, s, y0 + 96);
}

// Pannello Periscopio dell'Enhanced (docs/14 P4): i pulsanti (periscopio, su/giu', ingrandimento, modo, foto, filtro),
// la corsa e l'elevazione, il contatore dei secondi d'esposizione (giallo oltre gli 8 s di un giro, rosso oltre i 30) e,
// nel modo moderno, la lista NWP della guardata: tre giri rapidi, la ricerca aerea
void disegna_periscopi(Tela& t, const Font& f, const VistaPeriscopio& v, const StatoPannello& s, const StatoPeriscopi& p) {
    const int y0 = ALTEZZA_VISTA;
    intestazione(t, f, v, s, "PERISCOPIO");
    auto pulsante = [&](int x, const char* tasto, const std::string& testo, bool acceso, bool spento = false) {
        t.rettangolo_pieno(x, y0 + 18, 101, 19, acceso ? colori::GIALLO : colori::NERO);
        area(x, y0 + 18, 101, 19, std::string("pulsante_") + tasto);
        const std::string q = std::string(tasto) + " " + testo;
        f.scrivi(t, x + 50 - f.larghezza(q, 1) / 2, y0 + 24, q,
                 acceso ? colori::NERO : spento ? colori::GRIGIO_SCURO : colori::GRIGIO, 1);
    };
    const std::string nome = p.nomi.empty() ? std::string("-") : p.nomi[static_cast<std::size_t>(p.scelto)];
    pulsante(8, "1", nome, false, p.nomi.size() < 2);
    pulsante(113, "2", p.su ? "GIU'" : "SU", p.su);
    pulsante(218, "3", p.ingrandimento, false);
    pulsante(323, "4", p.modo, p.modo != "GIORNO");
    pulsante(428, "5", "FOTO", false, !p.foto);
    pulsante(533, "6", p.filtro.empty() ? "FILTRO" : p.filtro, !p.filtro.empty());
    char riga[160];
    std::snprintf(riga, sizeof riga, "%s   RIL. %03.0f°   ELEV. %+.0f°%s%s%s", p.corsa.c_str(), normalizza_360(v.rilevamento),
                  p.elevazione, p.finestra_coperta ? "   ONDA SULLA FINESTRA" : "",
                  p.tremolio > 0.5 ? "   TREMA: TROPPO VELOCI" : p.tremolio > 0.05 ? "   TREMA" : "",
                  p.telecamera_accesa ? "   TELECAMERA DELLA VELA" : "");
    f.scrivi(t, 12, y0 + 46, riga, p.danneggiato ? colori::ROSSO : colori::BIANCO, 1);
    // il contatore dell'esposizione: la regola dei pochi secondi
    const int sec = static_cast<int>(p.esposto_s);
    std::snprintf(riga, sizeof riga, "ESPOSTI %02d:%02d", sec / 60, sec % 60);
    const auto col = static_cast<std::uint8_t>(p.esposto_s >= 30.0 ? colori::ROSSO : p.esposto_s > 8.0 ? colori::GIALLO
                                               : p.esposto_s > 0.0 ? colori::VERDE_CHIARO : colori::GRIGIO);
    f.scrivi(t, LARGHEZZA_TELA - 12 - f.larghezza(riga, 2), y0 + 44, riga, col, 2);
    if (p.nwp) {
        std::snprintf(riga, sizeof riga, "NWP:  GIRI RAPIDI %d/3   RICERCA AEREA %s   POI GIU'", std::min(p.giri, 3), p.aria ? "FATTA" : "-");
        f.scrivi(t, 12, y0 + 62, riga, p.giri >= 3 && p.aria ? colori::VERDE_CHIARO : colori::GIALLO, 1);
    }
    f.scrivi(t, 12, y0 + 82, p.telecamera ? "1 PERISC. 2 SU/GIU' 3 INGR. 4 MODO 5 FOTO 6 FILTRO 7 TELECAMERA  FRECCE ELEV.  < > RUOTA"
                                          : "1 PERISCOPIO 2 SU/GIU' 3 INGR. 4 MODO 5 FOTO 6 FILTRO  FRECCE SU/GIU' ELEVAZIONE  < > RUOTA",
             colori::GRIGIO, 1);
    disegna_messaggio(t, f, s, y0 + 96);
}

// Pannello Radio: due file di pulsanti; il contenuto (archivio, telescrivente, broadcast) e' nella vista
void disegna_radio(Tela& t, const Font& f, const VistaPeriscopio& v, const StatoPannello& s, const StatoRadio& r) {
    const int y0 = ALTEZZA_VISTA;
    intestazione(t, f, v, s, "RADIO");
    auto pulsante = [&](int x, int y, int h, const char* tasto, const std::string& testo, bool acceso) {
        t.rettangolo_pieno(x, y, 150, h, acceso ? colori::GIALLO : colori::NERO);
        area(x, y, 150, h, std::string("pulsante_") + tasto);
        const std::string q = std::string(tasto) + " " + testo;
        f.scrivi(t, x + 75 - f.larghezza(q, 1) / 2, y + (h - 7) / 2, q, acceso ? colori::NERO : colori::GRIGIO, 1);
    };
    if (r.enhanced) {
        pulsante(12, y0 + 18, 19, "1", r.antenna ? "ANTENNA GIU'" : "ANTENNA SU", r.antenna);
        pulsante(170, y0 + 18, 19, "2", r.filo, false);
        pulsante(328, y0 + 18, 19, "3", "PREC.", false);
        pulsante(486, y0 + 18, 19, "4", "SUCC.", false);
        pulsante(12, y0 + 41, 19, "5", "ORDINI", r.solo_ordini);
        pulsante(170, y0 + 41, 19, "6", "TIPO " + r.trasmissione, false);
        pulsante(328, y0 + 41, 19, "7", "TRASMETTI", false);
        pulsante(486, y0 + 41, 19, "8", "SATELLITE", false);
        f.scrivi(t, 12, y0 + 70, "9 BOA SLOT  " + tasti_valore() + " RITARDO DELLA BOA   TRASMETTERE ESPONE ALLA RADIOGONIOMETRIA NEMICA",
                 colori::GRIGIO, 1);
    } else {
        pulsante(12, y0 + 20, 21, "1", r.periscopio ? "PERISCOPIO GIU'" : "PERISCOPIO SU", r.periscopio);
        pulsante(170, y0 + 20, 21, "2", r.antenna ? "ANTENNA GIU'" : "ANTENNA SU", r.antenna);
        pulsante(328, y0 + 20, 21, "3", "PREC.", false);
        pulsante(486, y0 + 20, 21, "4", "SUCC.", false);
        disegna_barra(t, f, s, y0 + 50);
    }
    f.scrivi(t, 12, y0 + 82, r.enhanced ? "" : "1 PERISCOPIO  2 ANTENNA  3 PREC.  4 SUCC.  (SOLO A QUOTA PERISCOPICA)", colori::GRIGIO, 1);
    disegna_messaggio(t, f, s, y0 + 96);
}

// Radio dell'Enhanced con le consolle: telescrivente, ESM (WLR-8 o Bukhta) e i nove pulsanti della sala radio,
// sugli stessi tasti 1-9 di sempre
void disegna_radio_consolle(Tela& t, const Font& f, const VistaPeriscopio& v, const StatoPannello& s, const StatoRadio& r,
                            const TemaStazione& tema) {
    DatiRadioEsm d;
    d.righe_telescrivente = r.telescrivente;
    d.panoramica = r.panoramica;
    d.intercettazioni = r.intercettazioni;
    d.minaccia = r.minaccia;
    std::string tipo = r.trasmissione.substr(0, 6);
    d.pulsanti = {{r.antenna ? "ANT GIU" : "ANT SU", "pulsante_1", r.antenna},
                  {r.filo_breve, "pulsante_2", r.filo_filato},
                  {"PREC", "pulsante_3"},
                  {"SUCC", "pulsante_4"},
                  {"ORDINI", "pulsante_5", r.solo_ordini},
                  {tipo, "pulsante_6"},
                  {"TRASM", "pulsante_7"},
                  {"SAT", "pulsante_8"},
                  {"SLOT" + std::to_string(r.ritardo_min), "pulsante_9"}};
    consolle(t, f, v, s, "RADIO", ("1-9 PULSANTI   " + tasti_valore() + " RITARDO SLOT   INVIO AUTENTICA   " +
                                   (tasti_valore() == "[ ]" ? "TRASMETTERE ESPONE ALLA RADIOGONIOMETRIA" : "TRASMETTERE ESPONE AL DF")).c_str(),
             [&](Rettangolo q, std::vector<AreaPulto>* a) { pannello_radio_esm(t, q, tema, *s.consolle, d, a); });
}

// Pannello Navigazione: la carta e' sopra; qui punti di rotta, pilota, proiezione e barra del bersaglio
void disegna_navigazione(Tela& t, const Font& f, const VistaPeriscopio& v, const StatoPannello& s, const StatoNavigazione& n) {
    const int y0 = ALTEZZA_VISTA;
    intestazione(t, f, v, s, "NAVIGAZIONE");
    char riga[160];
    if (n.tma) {
        char rateo[32] = "-";
        if (n.tma_rateo) std::snprintf(rateo, sizeof rateo, "%+.2f°/MIN", *n.tma_rateo);
        std::snprintf(riga, sizeof riga, "MATE  DISTANZA %.1f NM  ROTTA %03.0f°  VELOCITÀ %.1f NODI  SCARTO %.2f°  RATEO %s  (%d RIL.)",
                      n.tma_distanza_nm, n.tma_rotta, n.tma_velocita_kn, n.tma_scarto, rateo, n.tma_rilevamenti);
    } else if (n.fondale) {
        std::snprintf(riga, sizeof riga, "%s", n.fondale_riga.c_str());
    } else {
        std::snprintf(riga, sizeof riga, "PUNTI DI ROTTA %d/9  PILOTA %s  PROIEZIONE %02d MIN %s  SCIE %s  INGRANDIMENTO %d", n.punti,
                      n.pilota ? "ON" : "OFF", n.minuti, n.proiezione ? "ON" : "OFF",
                      n.scia_bersaglio ? "BERSAGLIO" : n.tutte_le_scie ? "TUTTE" : "OFF", n.fattore);
    }
    f.scrivi(t, 12, y0 + 24, riga, n.tma ? colori::GIALLO : colori::BIANCO, 1);
    disegna_barra(t, f, s, y0 + 44);
    f.scrivi(t, 12, y0 + 76, n.tma ? "MATE: FRECCE SINISTRA/DESTRA ROTTA, SU/GIU DISTANZA, MAIUSC+SU/GIU VELOCITÀ  T CHIUDE"
                                   : n.fondale ? "FONDALE: 1 PING  2 OGNI N MINUTI  3 POTENZA  4 MISURE SULLA CARTA  CTRL+H O ESC CHIUDE"
                                   : n.strumenti ? "4 RIGHELLO 5 LINEE 6 ANELLI 7 SEGNI 8 CPA 9 T-RIL. Q EKELUND E EVENTO F AGGANCIA B STILE T MATE"
                                   : "1 TUTTE LE SCIE  2 SCIA BERSAGLIO  3/J PROIEZIONE  [ ] MINUTI  W PUNTO  K CANCELLA  I O ZOOM  T MATE",
             colori::GRIGIO, 1);
    disegna_messaggio(t, f, s, y0 + 96);
}

// Pannello Navigazione dell'Enhanced con le consolle (gruppo 5, bozze approvate il 29/10/2026): la lamiera con lo
// stato in spie e vetri, il bersaglio, i comandi della carta come tasti della tastiera e la riga dell'equipaggio. Col
// MATE e la pagina del fondale le loro righe al posto dei tasti. Sull'Alfa il Sozh: il beige e le scritte scure
void disegna_navigazione_lamiera(Tela& t, const Font& f, const VistaPeriscopio& v, const StatoPannello& s, const StatoNavigazione& n) {
    ripetitori::DatiPannelloCarta d;
    char riga[200];
    const bool alfa = s.battello == BattelloTema::alfa;   // il plottatore Sozh
    d.titolo = std::string(alfa ? "SOZH  " : "NAVIGAZIONE  ") + s.missione + "  " + (v.alto_ingrandimento ? "6X" : "1,5X");
    const int ore = static_cast<int>(s.tempo_s / 3600.0) % 24, minuti = static_cast<int>(s.tempo_s / 60.0) % 60,
              secondi = static_cast<int>(s.tempo_s) % 60;
    std::snprintf(riga, sizeof riga, "ORA %02d:%02d:%02dZ  TEMPO X%g", ore, minuti, secondi, s.compressione);
    d.ora = riga;
    d.pilota = n.pilota;
    d.proiezione = n.proiezione;
    d.scie = n.tutte_le_scie || n.scia_bersaglio;
    d.minuti = n.minuti;
    d.punti = n.punti;
    d.scala = n.fattore;
    d.bersaglio = s.barra_titolo;
    d.bersaglio_dati = s.barra_dati;
    d.messaggio = s.messaggio;
    if (n.tma) {
        char rateo[32] = "-";
        if (n.tma_rateo) std::snprintf(rateo, sizeof rateo, "%+.2f°/MIN", *n.tma_rateo);
        std::snprintf(riga, sizeof riga, "MATE  DISTANZA %.1f NM  ROTTA %03.0f°  VELOCITÀ %.1f NODI  SCARTO %.2f°  RATEO %s  (%d RIL.)",
                      n.tma_distanza_nm, n.tma_rotta, n.tma_velocita_kn, n.tma_scarto, rateo, n.tma_rilevamenti);
        d.modo = {{riga, fosforo(6)}, {"FRECCE SINISTRA/DESTRA ROTTA, SU/GIU DISTANZA, MAIUSC+SU/GIU VELOCITÀ  T CHIUDE", fosforo(4)}};
    } else if (n.fondale) {
        d.modo = {{n.fondale_riga, fosforo(6)}, {"1 PING  2 OGNI N MINUTI  3 POTENZA  4 MISURE SULLA CARTA  CTRL+H O ESC CHIUDE", fosforo(4)}};
    } else {
        d.comandi = {{"4", "RIGHELLO"}, {"5", "LINEE"},   {"6", "ANELLI"},  {"7", "SEGNI"}, {"8", "CPA"},  {"9", "T-RIL."},
                     {"Q", "EKELUND"},  {"E", "EVENTO"},  {"F", "AGGANCIA"}, {"B", "STILE"}, {"T", "MATE"}};
    }
    ripetitori::pannello_carta(t, {f, s.consolle->targa, s.consolle->schermo, lamiera::scritte_per(s.battello)}, d);
}

// Quadro di stato: «SISTEMA OK» o il danno, e l'integrita' dello scafo
void disegna_stato(Tela& t, const Font& f, const VistaPeriscopio& v, const StatoPannello& s, const StatoSistemi& st) {
    const int y0 = ALTEZZA_VISTA;
    intestazione(t, f, v, s, "STATO");
    // 13 sistemi (col navigatore inerziale) stanno in cinque righe di 8 pixel, sopra la riga dello scafo
    const bool fitto = st.guasti.size() > 12;
    const int colonne = 3, passo_x = 210, passo_y = fitto ? 8 : st.guasti.size() > 6 ? 9 : 14, y_primo = fitto ? 16 : 18;
    for (std::size_t i = 0; i < st.guasti.size(); ++i) {
        const auto& voce = st.guasti[i];
        const int x = 12 + static_cast<int>(i % static_cast<std::size_t>(colonne)) * passo_x,
                  y = y0 + y_primo + static_cast<int>(i / static_cast<std::size_t>(colonne)) * passo_y;
        const std::string r = voce.nome + (voce.percento >= 0 ? " " + std::to_string(voce.percento) : voce.guasto ? ": GUASTO" : ": OK");
        f.scrivi(t, x, y, r, voce.guasto ? colori::GIALLO : colori::VERDE_CHIARO, 1);
    }
    const std::string scafo = st.scafo_violato ? "SCAFO SQUARCIATO" : "SISTEMI DI BORDO OPERATIVI AL " + std::to_string(st.percento) + " PER CENTO";
    // con 13 voci la quinta riga ha solo il navigatore: lo scafo le sta accanto, nella seconda colonna
    f.scrivi(t, fitto ? 12 + passo_x : 12, fitto ? y0 + y_primo + 4 * passo_y : y0 + 55, scafo, st.scafo_violato || st.percento < 50 ? colori::GIALLO : colori::BIANCO, 1);
    disegna_barra(t, f, s, y0 + 66);
    disegna_messaggio(t, f, s, y0 + 96);
}

// Stato dell'Alfa con le consolle: il pulto di Ritm. I tredici sistemi nei tre sinottici (GAMMA reattore e
// propulsione, TEMBR rete ed elettronica, TAKT-T impianti), lampada verde da 90, ambra da 50, rossa sotto o guasta; le
// finestrelle d'avaria coi sistemi guasti; potenza dai giri, temperatura del primario STIMA (270-300 °C)
void disegna_stato_consolle(Tela& t, const Font& f, const VistaPeriscopio& v, const StatoPannello& s, const StatoSistemi& st,
                            const TemaStazione& tema) {
    static constexpr int GRUPPO[] = {1, 1, 1, 1, 0, 1, 2, 2, 0, 2, 2, 1, 1};   // per Sistema, nell'ordine di battello.hpp
    DatiStato d;
    d.sinottici = {{"GAMMA", {}, {}}, {"TEMBR", {}, {}}, {"TAKT-T", {}, {}}};
    for (std::size_t i = 0; i < st.guasti.size() && i < std::size(GRUPPO); ++i) {
        const auto& x = st.guasti[i];
        const int e = x.percento >= 0 ? x.percento : x.guasto ? 0 : 100;
        auto& g = d.sinottici[static_cast<std::size_t>(GRUPPO[i])];
        g.lampade.push_back(x.guasto || e < 50 ? ColoreLampada::rossa : e < 90 ? ColoreLampada::ambra : ColoreLampada::verde);
        g.accese.push_back(true);
        if (x.guasto && d.allarmi.size() < 4) d.allarmi.push_back(x.nome);
    }
    const double giri = s.manovra ? std::clamp(std::fabs(s.manovra->giri), 0.0, 1.0) : 0.0;
    d.potenza_reattore = giri;
    d.temperatura = 270.0 + 30.0 * giri;
    consolle(t, f, v, s, "STATO", st.scafo_violato ? "SCAFO SQUARCIATO" : "RITM: SINOTTICI GAMMA, TEMBR E TAKT-T",
             [&](Rettangolo r, std::vector<AreaPulto>* a) { pannello_stato_alfa(t, r, tema, *s.consolle, d, a); });
}

// Centrale: il battello in una riga, i contatti, le armi, i guasti, e il menu delle stazioni
void disegna_centrale(Tela& t, const Font& f, const VistaPeriscopio& v, const StatoPannello& s, const StatoCentrale& c) {
    const int y0 = ALTEZZA_VISTA;
    intestazione(t, f, v, s, c.pausa ? "CENTRALE  (PAUSA)" : "CENTRALE");
    char riga[200];
    std::snprintf(riga, sizeof riga, "QUOTA %.0f M (%.0f FT)   ROTTA %03.0f°   VELOCITÀ %.1f NODI%s%s%s", c.quota_m, c.quota_m / 0.3048,
                  normalizza_360(c.rotta), c.velocita_kn, c.periscopio ? "   PERISCOPIO SU" : "",
                  c.antenna ? (c.radar ? "   ANTENNA SU, RADAR ACCESO" : "   ANTENNA SU") : c.radar ? "   RADAR ACCESO" : "",
                  c.schiera ? "   SCHIERA FUORI" : "");
    f.scrivi(t, 12, y0 + 20, riga, colori::BIANCO, 1);
    std::snprintf(riga, sizeof riga, "CONTATTI %d (%d CON POSIZIONE)   ARMI IN ACQUA %d   SISTEMI GUASTI %d   SCAFO %d PER CENTO", c.contatti,
                  c.tracce, c.armi_in_acqua, c.guasti, c.percento);
    f.scrivi(t, 12, y0 + 32, riga, c.guasti ? colori::GIALLO : colori::VERDE_CHIARO, 1);
    disegna_barra(t, f, s, y0 + 46);
    static constexpr std::pair<const char*, const char*> MENU[] = {
        {"F2 RADIO", "stazione_radio"}, {"F3 STATO", "stazione_stato"}, {"F4 MANOVRA", "stazione_manovra"},
        {"F6 NAVIGAZ.", "stazione_navigazione"}, {"F7 ARMI", "stazione_armi"}, {"F8 SONAR", "stazione_sonar"}, {"P PAUSA", "pausa"}};
    for (std::size_t i = 0; i < std::size(MENU); ++i) {   // menu delle stazioni: anche col mouse
        const int x = 12 + static_cast<int>(i) * 89;
        t.rettangolo_pieno(x, y0 + 75, 85, 15, colori::NERO);
        f.scrivi(t, x + 42 - f.larghezza(MENU[i].first, 1) / 2, y0 + 79, MENU[i].first, colori::GRIGIO, 1);
        area(x, y0 + 75, 85, 15, MENU[i].second);
    }
    disegna_messaggio(t, f, s, y0 + 96);
}

}  // namespace

TemaStazione tema_consolle(BattelloTema b, double luce) {
    static const double NOTTE = luce_ambiente(-6.0);
    TemaStazione t;
    t.battello = b;
    t.luce = b == BattelloTema::usa688 && luce < NOTTE ? LuceSala::rossa : LuceSala::bianca;
    return t;
}

bool navigazione_lamiera(const StatoPannello& s) {
    return s.consolle && s.navigazione && s.navigazione->strumenti;
}

std::optional<TemaStazione> tema_pannello(const StatoPannello& s, double luce) {
    if (!s.consolle) return std::nullopt;
    const bool altro = s.sonar || s.navigazione || s.stato || s.centrale || s.radio || s.periscopi;
    const bool pannello = s.armi || (s.stato && s.battello == BattelloTema::alfa) || (s.radio && s.radio->enhanced) ||
                          (!altro && s.manovra && s.manovra->enhanced) || navigazione_lamiera(s);
    if (!pannello && !s.consolle_vista) return std::nullopt;
    TemaStazione t = tema_consolle(s.battello, luce);
    if (s.sonar && s.battello == BattelloTema::usa688) t.luce = LuceSala::blu;   // la sala sonar del 688, luce blu fissa
    return t;
}

void disegna_pannello(Tela& t, const Font& f, const Nave& proprio, const VistaPeriscopio& v, const StatoPannello& s,
                      std::vector<AreaAttiva>* aree) {
    if (aree) aree->clear();
    aree_ = aree;
    const auto tema = tema_pannello(s, v.luce);
    if (s.armi && tema) { disegna_armi_consolle(t, f, v, s, *s.armi, *tema); return; }
    if (s.armi) { disegna_armi(t, f, v, s, *s.armi); return; }
    if (s.sonar) { disegna_sonar(t, f, v, s, *s.sonar); return; }
    if (navigazione_lamiera(s) && tema) { disegna_navigazione_lamiera(t, f, v, s, *s.navigazione); return; }
    if (s.navigazione) { disegna_navigazione(t, f, v, s, *s.navigazione); return; }
    if (s.stato && tema && s.battello == BattelloTema::alfa) { disegna_stato_consolle(t, f, v, s, *s.stato, *tema); return; }
    if (s.stato) { disegna_stato(t, f, v, s, *s.stato); return; }
    if (s.centrale) { disegna_centrale(t, f, v, s, *s.centrale); return; }
    if (s.radio && s.radio->enhanced && tema) { disegna_radio_consolle(t, f, v, s, *s.radio, *tema); return; }
    if (s.radio) { disegna_radio(t, f, v, s, *s.radio); return; }
    if (s.periscopi) { disegna_periscopi(t, f, v, s, *s.periscopi); return; }
    if (s.manovra && tema) { disegna_manovra_consolle(t, f, v, s, *s.manovra, *tema); return; }
    if (s.manovra) { disegna_manovra(t, f, v, s, *s.manovra); return; }
    const int y0 = ALTEZZA_VISTA;
    t.rettangolo_pieno(0, y0, LARGHEZZA_TELA, ALTEZZA_TELA - y0, colori::GRIGIO_SCURO);
    t.linea_orizzontale(0, LARGHEZZA_TELA - 1, y0, colori::GRIGIO);
    char riga[128];
    const int ore = static_cast<int>(s.tempo_s / 3600.0) % 24, minuti = static_cast<int>(s.tempo_s / 60.0) % 60,
              secondi = static_cast<int>(s.tempo_s) % 60;

    if (!s.missione.empty())
        std::snprintf(riga, sizeof riga, "%s  %s", s.missione.c_str(), v.alto_ingrandimento ? "6x" : "1,5x");
    else
        std::snprintf(riga, sizeof riga, "periscopio  %s", v.alto_ingrandimento ? "6x" : "1,5x");
    f.scrivi(t, 16, y0 + 8, riga, colori::CIANO_CHIARO);
    std::snprintf(riga, sizeof riga, "ora %02d:%02d:%02dZ", ore, minuti, secondi);
    f.scrivi(t, LARGHEZZA_TELA - 16 - f.larghezza(riga), y0 + 8, riga, colori::CIANO_CHIARO);

    std::snprintf(riga, sizeof riga, "rilevamento %03d°", static_cast<int>(std::lround(normalizza_360(v.rilevamento))) % 360);
    f.scrivi(t, 16, y0 + 36, riga, colori::BIANCO);
    std::snprintf(riga, sizeof riga, "rotta %03d°  %.0f nodi", static_cast<int>(std::lround(proprio.rotta)) % 360,
                  proprio.velocita / NODO);
    f.scrivi(t, 16, y0 + 64, riga, colori::BIANCO);

    std::snprintf(riga, sizeof riga, "sole %+.0f°  tempo x%g", s.elevazione_sole, s.compressione);
    f.scrivi(t, LARGHEZZA_TELA - 16 - f.larghezza(riga), y0 + 64, riga, colori::GIALLO);
    // ultima riga: il messaggio di bordo (in missione) o l'aiuto; scala 2 se entra, altrimenti 1 (sempre per intero)
    const bool aiuto = s.messaggio.empty();
    const std::string ultima = aiuto ? "FRECCE: RUOTA, ZOOM   PAG SU/GIU: TEMPO   V: VERITÀ" : s.messaggio;
    const bool grandi = !aiuto;   // l'aiuto in maiuscole piccole, i messaggi in maiuscole grandi
    const int scala_ultima = f.larghezza(ultima, 2, 1, grandi) <= LARGHEZZA_TELA - 32 ? 2 : 1;
    f.scrivi(t, 16, y0 + 92, ultima, aiuto ? colori::GRIGIO : colori::VERDE_CHIARO, scala_ultima, 1, grandi);
}

}  // namespace m688
