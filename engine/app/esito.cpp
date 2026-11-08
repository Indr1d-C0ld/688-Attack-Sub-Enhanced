#include "esito.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <ctime>
#include <exception>

#include "grafica/cerimonia.hpp"
#include "grafica/cornici.hpp"
#include "grafica/giornale.hpp"
#include "grafica/medaglioni.hpp"
#include "missioni/carriera_missione.hpp"
#include "missioni/achievement_missione.hpp"
#include "nucleo/albo.hpp"
#include "nucleo/albo_onore.hpp"

namespace m688 {

using namespace cerimonia;

namespace {
constexpr int PIEDE_Y = 428;   // il piede di cerimonia (grafica/cornici), come l'albo

std::string maiuscolo(std::string s) {
    for (auto& c : s) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return s;
}

// Paragrafo a capo sulle parole; restituisce la y dopo l'ultima riga (si ferma a y_max)
int paragrafo(Tela& t, const Font& f, int x, int y, int w, const std::string& testo, int col, int y_max = 1 << 30) {
    std::string riga, parola;
    const auto emetti = [&] {
        if (y <= y_max) {   // oltre il limite il resto si tace
            f.scrivi(t, x, y, riga, static_cast<std::uint8_t>(col), 1);
            y += 11;
        }
        riga.clear();
    };
    for (std::size_t i = 0; i <= testo.size(); ++i) {
        const char c = i < testo.size() ? testo[i] : ' ';
        if (c != ' ' && c != '\n') {
            parola += c;
            continue;
        }
        if (!parola.empty()) {
            const std::string prova = riga.empty() ? parola : riga + " " + parola;
            if (f.larghezza(prova, 1) > w && !riga.empty()) {
                emetti();
                riga = parola;
            } else {
                riga = prova;
            }
            parola.clear();
        }
        if (c == '\n' && !riga.empty()) emetti();
    }
    if (!riga.empty()) emetti();
    return y;
}

void scrivania(Tela& t) {
    for (int y = 0; y < ALTEZZA_TELA; ++y)
        for (int x = 0; x < LARGHEZZA_TELA; ++x)
            t.pixel(x, y, static_cast<std::uint8_t>(LEGNO + retina(2.2 + 1.2 * std::sin((x + 9 * std::sin(y * 0.05)) * 0.05) +
                                                                       0.5 * caso(x / 5, y, 2), x, y, 7)));
}

void tenda(Tela& t) {   // il fondale della cerimonia: tenda di velluto a pieghe
    for (int y = 0; y < ALTEZZA_TELA; ++y)
        for (int x = 0; x < LARGHEZZA_TELA; ++x) {
            const double piega = 0.5 + 0.5 * std::sin(x * 0.09 + 0.6 * std::sin(x * 0.023));
            t.pixel(x, y, static_cast<std::uint8_t>(BANDIERA + retina(1.0 + 4.5 * piega * (0.75 + 0.25 * y / 480.0), x, y, 7)));
        }
}

std::string punti(double v) {
    char b[24];
    std::snprintf(b, sizeof b, "%+.0f", std::abs(v) < 0.5 ? 0.0 : v);
    if (std::abs(v) < 0.5) return "0";
    return b;
}

std::string decimale(double v) {   // 381,5: la somma esatta, che le voci arrotondate non danno
    char b[24];
    std::snprintf(b, sizeof b, "%.1f", v);
    std::string s = b;
    std::replace(s.begin(), s.end(), '.', ',');
    return s;
}

std::string per(double v) {   // 1,30
    char b[24];
    std::snprintf(b, sizeof b, "%.2f", v);
    std::string s = b;
    std::replace(s.begin(), s.end(), '.', ',');
    return s;
}

// «1989-10» -> «OTTOBRE 1989»
std::string data_estesa_(const std::string& d) {
    static constexpr const char* MESI[] = {"GENNAIO", "FEBBRAIO", "MARZO", "APRILE", "MAGGIO", "GIUGNO", "LUGLIO",
                                           "AGOSTO", "SETTEMBRE", "OTTOBRE", "NOVEMBRE", "DICEMBRE"};
    if (d.size() < 7) return d;
    const int m = std::atoi(d.substr(5, 2).c_str());
    return m >= 1 && m <= 12 ? std::string(MESI[m - 1]) + " " + d.substr(0, 4) : d;
}

std::string unisci(const std::vector<std::string>& v) {
    std::string s;
    for (const auto& r : v) s += (s.empty() ? "" : "\n") + r;
    return s;
}

}  // namespace

std::string data_estesa(const std::string& d) { return data_estesa_(d); }

ParametriRitratto ritratto_comandante(const RegoleCarriera& r, const Fascicolo& f) {
    ParametriRitratto p;
    p.marina = f.marina;
    const Grado* g = r.grado(f.marina, f.grado);
    // US Navy 1-10 per O-1...O-10; VMF 1-12 nell'ordine del dossier 13 §1.5 (il tenente e' il secondo)
    p.grado = g ? g->ordine + (f.marina == Marina::usn ? 1 : 2) : 5;
    p.berretto = f.marina == Marina::vmf;
    p.qualificato = f.ha_qualifica("dolphins") || f.marina == Marina::vmf;
    p.comando = f.ha_qualifica("comando") || f.ha_qualifica("vsok");
    p.nastrini = Carriera(r, f).barretta();
    const int anno = f.data.size() >= 4 ? std::atoi(f.data.substr(0, 4).c_str()) : 1989;
    p.volto.seme = f.seme;
    p.volto.eta = std::clamp(22 + anno - f.anno_nomina, 22, 60);
    return p;
}

std::string nome_grado_comandante(const RegoleCarriera& r, const Fascicolo& f) {
    const Grado* g = r.grado(f.marina, f.grado);
    return g ? maiuscolo(g->nome_it) : maiuscolo(f.grado);
}

Fascicolo fascicolo_comandante(const RegoleCarriera& r, Marina m) {
    // il comandante in carica dell'albo, poi il primo in servizio della marina, infine uno nuovo («carriera breve»)
    if (const auto id = comandante_in_carica(m)) {
        try {
            return leggi_fascicolo(percorso_fascicolo(*id));
        } catch (const std::exception&) {
        }
    }
    for (const VoceAlbo& v : leggi_albo())
        if (v.f.marina == m && caricabile(v.f)) return v.f;
    const std::string nome = m == Marina::usn ? "J. R. Halloran" : "V. A. Suchkov";
    Fascicolo nuovo = Carriera::nuovo(r, m, nome, true, Indulgenza::realistica);
    nuovo.id = id_libero(nuovo.id);   // un omonimo caduto o congedato resta nell'albo col suo fascicolo
    return nuovo;
}

// Achievement e albo d'onore (roadmap 15; nucleo/achievement, nucleo/albo_onore): le misure della missione, gli
// sblocchi del comandante (solo se la missione conta per la carriera) e globali, la missione nell'albo d'onore e, a
// carriera chiusa, la carriera. Le riprese non arrivano qui (main le tiene fuori carriera). Un file che manca non
// ferma la chiusura
namespace {
void achievement_e_onore(const Partita& p, const RegoleCarriera& r, MissioneChiusa& c, const std::string& dati,
                         const std::string& codice, int righe_acint) {
    try {
        static const CatalogoAchievement cat = carica_achievement(dati + "/achievement.toml");
        OpzioniMisure o;
        o.fascicolo = &c.dopo;
        o.fascicolo_prima = &c.prima;   // la carriera conta solo quel che la missione ha portato
        o.regole = &r;
        const StazioneArmi& a = p.stazioni().armi();   // le armi rimaste: scorte e tubi carichi, esche escluse
        int armi = 0;
        for (int x = 0; x <= static_cast<int>(Arma::esca_mobile); ++x)
            if (const Arma w = static_cast<Arma>(x); w != Arma::esca && w != Arma::esca_mobile && w != Arma::nessuna) armi += a.scorte(w);
        for (int t = 1; t <= a.numero_tubi(); ++t)
            if (a.stato(t) != StatoTubo::vuoto && a.arma(t) != Arma::esca && a.arma(t) != Arma::esca_mobile) ++armi;
        o.armi_a_bordo = armi;
        Misure m = misure_achievement(p.missione(), c.contesto, o);
        m.v["righe_nuove_acint"] = righe_acint;   // il rapporto ACINT e' della stazione Sonar dell'app
        c.achievement = registra_achievement(cat, m, c.dopo.id, c.esito.contata, c.contesto.ripresa, c.dopo.data, c.contesto.missione);
        // per la pagina: uno per achievement, quelli del comandante prima; i globali soli se la missione non contava
        const auto aggiungi = [&](const Sblocco& s, bool globale) {
            for (const auto& x : c.riconoscimenti)
                if (x.id == s.id) return;
            const DefAchievement* d = cat.cerca(s.id);
            if (!d) return;
            c.riconoscimenti.push_back({s.id, d->icona, d->categoria, d->nome_it, d->descrizione_it, s.livello, s.prima, globale});
        };
        for (const auto& s : c.achievement.comandante) aggiungi(s, false);
        for (const auto& s : c.achievement.globali) aggiungi(s, true);
        const std::time_t ora = std::time(nullptr);
        char oggi[16];
        std::strftime(oggi, sizeof oggi, "%Y-%m-%d", std::localtime(&ora));
        const VoceOnoreMissione v{c.esito.punteggio.totale, c.contesto.missione, c.contesto.origine, codice, c.dopo.data, oggi,
                                  c.dopo.id, c.dopo.nome, c.dopo.marina == Marina::vmf ? "vmf" : "usn", c.prima.grado};
        c.posto_onore = registra_missione_onore(v, c.contesto.ripresa, c.contesto.realismo);
        if (c.esito.fine_carriera) registra_carriera_onore(c.dopo, &r);
    } catch (const std::exception& e) {
        std::fprintf(stderr, "achievement e albo d'onore non registrati: %s\n", e.what());
    }
}
}  // namespace

MissioneChiusa chiudi_partita(const Partita& p, const RegoleCarriera& r, Fascicolo& f, const std::string& battello,
                              const std::string& teatro, const std::string& origine, std::uint64_t seme,
                              const std::string& dati, const std::string& codice, const RealismoPartita& realismo, int righe_acint) {
    const Missione& m = p.missione();
    MissioneChiusa c;
    c.titolo = m.titolo();
    c.dettaglio = m.esito_dettagliato();
    OpzioniRiassunto o;
    if (p.carico() && p.catalogo_carico()) {
        o.carico = p.carico();
        o.catalogo = p.catalogo_carico();
        if (const auto* loc = o.catalogo->locale(m.giocatore() ? m.giocatore()->classe : "", m.limiti_carico().variante_battello))
            o.locale = loc->id;
        o.tipo_missione = m.limiti_carico().tipo_missione;
    }
    c.riassunto = riassumi(m, o);
    c.contesto = contesto_carriera(m, r, origine, seme);
    c.contesto.battello = battello;
    c.contesto.teatro = c.contesto.teatro_en = teatro;
    c.contesto.realismo = realismo.valore;
    c.aiuti = realismo.aiuti;
    c.prima = f;
    Carriera k(r, f);
    c.esito = k.chiudi_missione(c.contesto, c.riassunto);
    c.barretta = k.barretta();
    f = k.fascicolo();
    c.dopo = f;
    if (!dati.empty()) achievement_e_onore(p, r, c, dati, codice, righe_acint);
    return c;
}

SchermateEsito::SchermateEsito(const MissioneChiusa& c, const RegoleCarriera& r, const TavolaDecorazioni& tav)
    : c_(c), r_(r), tav_(tav), marina_(c.dopo.marina) {
    pagine_.push_back({Tipo::esito});
    if (fa_notizia()) pagine_.push_back({Tipo::giornale});
    for (int i = 0; i < static_cast<int>(c.riconoscimenti.size()); i += RICONOSCIMENTI_PER_PAGINA)
        pagine_.push_back({Tipo::riconoscimenti, nullptr, i});
    if (c.esito.personale) pagine_.push_back({Tipo::cerimonia, &*c.esito.personale});
    if (c.esito.reparto) pagine_.push_back({Tipo::cerimonia, &*c.esito.reparto});
    if (c.esito.promozione) pagine_.push_back({Tipo::promozione});
    if (c.esito.provvedimento) pagine_.push_back({Tipo::provvedimento});
    if (c.esito.stato == "morto") pagine_.push_back({Tipo::memoria});
}

void SchermateEsito::vai(int i) { pagina_ = std::clamp(i, 0, pagine() - 1); }

SchermateEsito::Uscita SchermateEsito::comandi(const Comandi& c) {
    for (const auto& [x, y] : c.clic)   // i due tasti del piede di cerimonia: indietro (o il debriefing), avanti
        switch (tasto_cerimonia(x, y, PIEDE_Y, 2)) {
            case 0:
                if (pagina_ == 0) return Uscita::debriefing;
                vai(pagina_ - 1);
                return Uscita::nessuna;
            case 1: return pagina_ + 1 < pagine() ? (vai(pagina_ + 1), Uscita::nessuna) : Uscita::fine;
            default: break;
        }
    if (c.invio) {
        if (pagina_ + 1 >= pagine()) return Uscita::fine;
        vai(pagina_ + 1);
    }
    if (c.esc) {
        if (pagina_ == 0) return Uscita::debriefing;
        vai(pagina_ - 1);
    }
    return Uscita::nessuna;
}

std::string SchermateEsito::nome_grado(const Fascicolo& f) const {
    const Grado* g = r_.grado(f.marina, f.grado);
    return g ? maiuscolo(g->nome_it) : maiuscolo(f.grado);
}

ParametriRitratto SchermateEsito::ritratto(const Fascicolo& f) const {
    ParametriRitratto p;
    p.marina = f.marina;
    const Grado* g = r_.grado(f.marina, f.grado);
    // US Navy 1-10 per O-1...O-10; VMF 1-12 nell'ordine del dossier 13 §1.5 (il tenente e' il secondo)
    p.grado = g ? g->ordine + (f.marina == Marina::usn ? 1 : 2) : 5;
    p.berretto = f.marina == Marina::vmf;
    p.qualificato = f.ha_qualifica("dolphins") || f.marina == Marina::vmf;
    p.comando = f.ha_qualifica("comando") || f.ha_qualifica("vsok");
    p.nastrini = c_.barretta;
    const int anno = f.data.size() >= 4 ? std::atoi(f.data.substr(0, 4).c_str()) : 1989;
    p.volto.seme = f.seme;
    p.volto.eta = std::clamp(22 + anno - f.anno_nomina, 22, 60);
    return p;
}

void SchermateEsito::palette(Palette& p) const {
    prepara_palette_ritratto(p, ritratto(c_.dopo), &tav_);
}

void SchermateEsito::disegna(Tela& t, const Font& f) const {
    if (!cache_ || pagina_cache_ != pagina_) {
        cache_ = std::make_unique<Tela>(t.larghezza(), t.altezza());
        componi(*cache_, f);
        pagina_cache_ = pagina_;
    }
    t = *cache_;
}

void SchermateEsito::componi(Tela& t, const Font& f) const {
    const Pagina& p = pagine_[static_cast<std::size_t>(pagina_)];
    switch (p.tipo) {
        case Tipo::esito: disegna_esito(t, f); break;
        case Tipo::giornale: disegna_giornale_esito(t, f); break;
        case Tipo::riconoscimenti: disegna_riconoscimenti(t, f, p.da); break;
        case Tipo::memoria: disegna_memoria(t, f); break;
        case Tipo::cerimonia: disegna_cerimonia(t, f, *p.conferimento); break;
        case Tipo::promozione: disegna_promozione(t, f); break;
        case Tipo::provvedimento: disegna_provvedimento(t, f); break;
    }
    // il piede comune della cerimonia (scelta dell'utente del 29/10/2026), come l'albo: la pagina e i due tasti d'ottone
    static constexpr const char* NOMI[] = {"IL RAPPORTO DI PATTUGLIA", "IL GIORNALE", "I RICONOSCIMENTI", "LA DECORAZIONE",
                                           "LA PROMOZIONE", "IL PROVVEDIMENTO", "IN MEMORIA"};   // nell'ordine di Tipo
    std::string aiuto = "PAGINA " + std::to_string(pagina_ + 1) + " DI " + std::to_string(pagine()) + ": " +
                        NOMI[static_cast<int>(p.tipo)];
    if (p.tipo == Tipo::riconoscimenti && static_cast<int>(c_.riconoscimenti.size()) > RICONOSCIMENTI_PER_PAGINA)
        aiuto += " " + std::to_string(p.da / RICONOSCIMENTI_PER_PAGINA + 1) + " DI " +
                 std::to_string((static_cast<int>(c_.riconoscimenti.size()) + RICONOSCIMENTI_PER_PAGINA - 1) / RICONOSCIMENTI_PER_PAGINA);
    piede_cerimonia(t, f, PIEDE_Y, aiuto,
                    {{pagina_ == 0 ? "ESC  DEBRIEFING" : "ESC  INDIETRO"}, {pagina_ + 1 < pagine() ? "INVIO  CONTINUA" : "INVIO  FINE"}}, 1);
}

void SchermateEsito::disegna_esito(Tela& t, const Font& f) const {
    const bool usn = marina_ == Marina::usn;
    const auto inchiostro = static_cast<std::uint8_t>(NERO + 1), tenue = static_cast<std::uint8_t>(NERO + 3),
               rosso = static_cast<std::uint8_t>(TIMBRO + 2);
    scrivania(t);
    const Rettangolo foglio{40, 12, 560, 412};   // fino a 424: sotto c'e' il piede di cerimonia
    carta(t, foglio, 7);
    const int x0 = foglio.x + 24, x1 = foglio.x + foglio.w - 24;
    f.scrivi(t, x0, foglio.y + 14, usn ? "PATROL REPORT" : "DONESENIE O POKHODE", inchiostro, 2);
    f.scrivi(t, x0, foglio.y + 34, usn ? "RAPPORTO DI PATTUGLIA" : "RAPPORTO DI CAMPAGNA", tenue, 1);
    timbro(t, f, x1 - 60, foglio.y + 26, usn ? "CONFIDENTIAL" : "CEKPETHO", -8, 2, 3);
    char riga[200];
    int y = foglio.y + 52;
    std::snprintf(riga, sizeof riga, "MISSIONE: %s", maiuscolo(c_.titolo).c_str());
    f.scrivi(t, x0, y, riga, inchiostro, 1);
    y += 12;
    std::snprintf(riga, sizeof riga, "COMANDANTE: %s %s", nome_grado(c_.prima).c_str(), maiuscolo(c_.prima.nome).c_str());
    f.scrivi(t, x0, y, riga, inchiostro, 1);
    y += 12;
    y = paragrafo(t, f, x0, y, x1 - x0, "BATTELLO: " + maiuscolo(c_.contesto.battello), inchiostro);
    std::snprintf(riga, sizeof riga, "DATA DI SERVIZIO: %s   SITUAZIONE: %s", data_estesa_(c_.prima.data).c_str(),
                  maiuscolo(std::string(nome_situazione(c_.contesto.situazione))).c_str());
    f.scrivi(t, x0, y, riga, inchiostro, 1);
    y += 16;

    // la frase d'esito (quella dell'originale per le sue missioni)
    using C = EsitoDettagliato::Categoria;
    const C cat = c_.dettaglio.categoria;
    std::string nome = nome_categoria_esito(cat);
    std::replace(nome.begin(), nome.end(), '_', ' ');
    f.scrivi(t, x0, y, "ESITO: " + maiuscolo(nome), cat == C::successo ? inchiostro : rosso, 1);
    y += 12;
    if (!c_.dettaglio.testo_it.empty()) y = paragrafo(t, f, x0, y, x1 - x0, c_.dettaglio.testo_it, tenue, y + 6 * 11);
    // il motivo piu' evidente del fallimento (da Falcon 3.0)
    std::string motivo;
    if (c_.dettaglio.battello_perso) motivo = "battello perduto";
    else if (c_.dettaglio.amici_colpiti > 0) motivo = "unita' amica colpita";
    else if (c_.dettaglio.neutrali_colpiti > 0) motivo = "neutrale colpito";
    else if (c_.dettaglio.fuoco_senza_ordine > 0) motivo = "armi usate senza autorizzazione";
    else if (cat != C::successo)
        for (const auto& o : c_.dettaglio.obiettivi)
            if (o.primario && o.stato == Obiettivo::Stato::fallito) {
                motivo = "obiettivo primario non raggiunto: " + o.testo_it;
                break;
            }
    if (!motivo.empty()) y = paragrafo(t, f, x0, y, x1 - x0, maiuscolo("motivo: " + motivo), rosso);
    y += 6;

    // le voci del punteggio (dossier 13 §2.3.1)
    const Punteggio& pt = c_.esito.punteggio;
    const int xp = x0 + 340;
    const auto voce = [&](const char* sigla, const char* nome_voce, double v) {
        f.scrivi(t, x0, y, sigla, tenue, 1);
        f.scrivi(t, x0 + 20, y, nome_voce, inchiostro, 1);
        const std::string s = punti(v);
        f.scrivi(t, xp - f.larghezza(s, 1), y, s, v < 0 ? rosso : inchiostro, 1);
        y += 11;
    };
    voce("R", "RIUSCITA", pt.riuscita);
    voce("C", "COMPLETEZZA, CLASSIFICAZIONI E RAPPORTI", pt.completezza);
    voce("S", "DISCREZIONE", pt.discrezione);
    voce("O", "ORDINI E SERIE RADIO", pt.ordini);
    voce("E", "EFFICIENZA DELLE ARMI E DEL CARICO", pt.efficienza);
    voce("D", "DANNI, FERITI E ARMI PERSE", pt.danni);
    voce("V", "VIOLAZIONI", pt.violazioni);
    t.linea_orizzontale(x0 + 20, xp, y, tenue);
    y += 4;
    const double somma = pt.riuscita + pt.completezza + pt.discrezione + pt.ordini + pt.efficienza + pt.danni + pt.violazioni;
    {   // la somma esatta: le voci sopra sono arrotondate all'unita'
        const std::string s = (somma > 0.0 ? "+" : "") + decimale(somma);
        f.scrivi(t, x0 + 20, y, "SOMMA", inchiostro, 1);
        f.scrivi(t, xp - f.larghezza(s, 1), y, s, somma < 0 ? rosso : inchiostro, 1);
        y += 11;
    }
    // la catena del moltiplicatore, fattore per fattore fino al totale, e gli aiuti accesi col loro peso: il giocatore
    // vede quanto costa ogni aiuto
    static constexpr const char* AVVERSARI[] = {"IMPREPARATO", "REGOLARE", "VETERANO", "ELITE"};
    const int avv = std::clamp(std::min(c_.contesto.avversario, c_.contesto.avversario_calcolato), 0, 3);
    const std::string catena = decimale(somma) + " X SITUAZIONE " + per(pt.m_situazione) + " (" +
                               maiuscolo(std::string(nome_situazione(c_.contesto.situazione))) + ") X AVVERSARIO " +
                               per(pt.m_avversario) + " (" + AVVERSARI[avv] + ") X REALISMO " + per(pt.m_realismo) +
                               " X PERICOLO " + per(1.0 + pt.pericolo) + " = " + punti(pt.totale).substr(pt.totale >= 0.5 ? 1 : 0);
    y = paragrafo(t, f, x0 + 20, y, x1 - x0 - 20, catena, tenue);
    std::string aiuti;
    for (const AiutoPartita& a : c_.aiuti) aiuti += (aiuti.empty() ? "" : ", ") + a.nome + " -" + per(a.peso);
    if (!aiuti.empty() && pt.m_realismo <= 0.5 + 1e-9) aiuti += " (AL MINIMO 0,50)";
    y = paragrafo(t, f, x0 + 20, y, x1 - x0 - 20, aiuti.empty() ? "REALISMO PIENO: NESSUN AIUTO ACCESO" : "AIUTI: " + aiuti, tenue);

    // il totale in un riquadro, e il giudizio
    const int ty = y + 6;
    t.linea_orizzontale(x0, x0 + 160, ty, inchiostro);
    t.linea_orizzontale(x0, x0 + 160, ty + 30, inchiostro);
    t.linea_verticale(x0, ty, ty + 30, inchiostro);
    t.linea_verticale(x0 + 160, ty, ty + 30, inchiostro);
    f.scrivi(t, x0 + 8, ty + 4, "TOTALE P", tenue, 1);
    std::snprintf(riga, sizeof riga, "%.0f", pt.totale);
    f.scrivi(t, x0 + 152 - f.larghezza(riga, 2), ty + 10, riga, inchiostro, 2);
    if (pt.squalifica) timbro(t, f, x0 + 80, ty + 15, "SQUALIFICA", 10, 2, 11);
    int gy = ty;
    std::string peso;
    if (!c_.esito.contata) peso = "LA MISSIONE NON CONTA PER LA CARRIERA";
    else if (c_.esito.peso < 1.0) {
        std::snprintf(riga, sizeof riga, "MISSIONE RIPETUTA: VALE %s, %.0f PUNTI", per(c_.esito.peso).c_str(), c_.esito.punti);
        peso = riga;
    }
    gy = paragrafo(t, f, x0 + 176, gy, x1 - x0 - 176, maiuscolo(pt.giudizio_it), inchiostro);
    if (!peso.empty()) gy = paragrafo(t, f, x0 + 176, gy, x1 - x0 - 176, peso, tenue);
    y = std::max(ty + 38, gy + 4);

    // equipaggio e decorazioni automatiche
    std::string equipaggio = c_.dettaglio.battello_perso ? (c_.dettaglio.equipaggio_salvo ? "BATTELLO PERDUTO, EQUIPAGGIO SALVO"
                                                                                          : "BATTELLO PERDUTO CON L'EQUIPAGGIO")
                                                         : "EQUIPAGGIO: ";
    if (!c_.dettaglio.battello_perso) {
        if (c_.dettaglio.feriti == 0 && c_.dettaglio.morti == 0) equipaggio += "NESSUNA PERDITA";
        else
            equipaggio += std::to_string(c_.dettaglio.feriti) + " FERITI, " + std::to_string(c_.dettaglio.morti) + " CADUTI" +
                          (c_.dettaglio.stimati ? " (STIMA)" : "");
    }
    y = paragrafo(t, f, x0, y, x1 - x0, equipaggio, inchiostro);
    std::string aut;
    for (const auto& k : c_.esito.automatiche)
        if (const Decorazione* d = r_.decorazione(k.decorazione)) aut += (aut.empty() ? "" : ", ") + maiuscolo(d->nome_it);
    if (!aut.empty()) y = paragrafo(t, f, x0, y, x1 - x0, "ANCHE: " + aut, tenue, foglio.y + foglio.h - 14);
    if (c_.esito.commissione) {
        std::string e = c_.esito.commissione->esito;
        std::replace(e.begin(), e.end(), '_', ' ');
        paragrafo(t, f, x0, y, x1 - x0, "COMMISSIONE DI SELEZIONE: " + maiuscolo(e), tenue, foglio.y + foglio.h - 14);
    }
}

void SchermateEsito::disegna_cerimonia(Tela& t, const Font& f, const Conferimento& k) const {
    const bool usn = marina_ == Marina::usn;
    const Decorazione* d = r_.decorazione(k.decorazione);
    const bool reparto = d && d->tipo == "reparto";
    tenda(t);
    std::string chi;
    if (const auto it = r_.formule.find(k.decorazione); it != r_.formule.end()) chi = it->second.first;
    const char* sotto = !usn       ? (reparto ? "AL SOMMERGIBILE E AL SUO EQUIPAGGIO" : "DECRETO DEL PRESIDIUM DEL SOVIET SUPREMO DELL'URSS")
                        : reparto ? "IL SEGRETARIO DELLA MARINA, AL BATTELLO E AL SUO EQUIPAGGIO"
                        : chi == "alta" ? "IL PRESIDENTE DEGLI STATI UNITI HA IL PIACERE DI CONFERIRE"
                                        : "IL SEGRETARIO DELLA MARINA HA IL PIACERE DI CONFERIRE";
    std::string titolo = maiuscolo(d ? d->nome_it : k.decorazione);
    if (k.numero > 1) titolo += " (" + std::to_string(k.numero) + ")";
    if (k.postuma) titolo += " - ALLA MEMORIA";
    intestazione(t, f, {0, 0, 640, 38}, titolo, sotto, marina_, false);
    cornice(t, {18, 50, 252, 326}, usn ? StileCornice::oro : StileCornice::rossa, 8);
    if (reparto) {   // l'equipaggio schierato davanti alla vela, con i feriti e i vuoti dei caduti
        ParametriEquipaggio eq;
        eq.marina = marina_;
        eq.seme = c_.dopo.seme;
        eq.grado_comandante = ritratto(c_.dopo).grado;
        const double a_bordo = usn ? 129.0 : 31.0;   // 688 e Alfa: la foto ne mostra 18
        const auto in_foto = [&](int n) { return n <= 0 ? 0 : std::max(1, static_cast<int>(std::lround(n * 18.0 / a_bordo))); };
        eq.caduti = in_foto(c_.dettaglio.morti);
        eq.feriti = in_foto(c_.dettaglio.feriti);
        disegna_equipaggio(t, f, {26, 58, 236, 310}, eq);
    } else {
        disegna_ritratto(t, {26, 58, 236, 310}, ritratto(c_.dopo), &tav_);
    }
    const Rettangolo astuccio{300, 50, 150, 210};
    cornice(t, astuccio, StileCornice::legno, 9);
    for (int y = astuccio.y + 9; y < astuccio.y + astuccio.h - 9; ++y)
        for (int x = astuccio.x + 9; x < astuccio.x + astuccio.w - 9; ++x)
            t.pixel(x, y, static_cast<std::uint8_t>(VELLUTO + retina(2.0 + 3.5 * (1.0 - std::hypot(x - 375.0, y - 140.0) / 120.0) +
                                                                        0.6 * caso(x, y, 9), x, y, 7)));
    tav_.disegna_medaglia(t, {astuccio.x + 14, astuccio.y + 14, astuccio.w - 28, astuccio.h - 28}, k.decorazione);
    f.scrivi(t, 470, 60, "NASTRINO", static_cast<std::uint8_t>(ORO + 6), 1);
    tav_.disegna_nastrino(t, 470, 76, 70, 20, VoceBarretta{k.decorazione, k.dispositivo, k.numero});
    paragrafo(t, f, 470, 106, 160, nome_grado(c_.dopo), ORO + 6);
    f.scrivi(t, 470, 130, maiuscolo(c_.dopo.nome), 15, 1);
    paragrafo(t, f, 470, 142, 160, maiuscolo(c_.contesto.battello), BIANCO + 2);
    f.scrivi(t, 470, 180, data_estesa_(k.data), static_cast<std::uint8_t>(BIANCO + 1), 1);
    const Rettangolo foglio{290, 256, 336, 168};
    carta(t, foglio, 7);
    const std::string mot = k.motivazione_it.empty() ? "Per il servizio reso durante la missione." : unisci(k.motivazione_it);
    paragrafo(t, f, foglio.x + 12, foglio.y + 12, foglio.w - 24, mot, NERO + 1, foglio.y + foglio.h - 16);
    timbro(t, f, foglio.x + foglio.w - 70, foglio.y + foglio.h - 28, usn ? "CLASSIFIED" : "CEKPETHO", -12, 2, 5);
}

void SchermateEsito::disegna_promozione(Tela& t, const Font& f) const {
    const bool usn = marina_ == Marina::usn;
    tenda(t);
    intestazione(t, f, {0, 0, 640, 38}, "PROMOZIONE",
                 usn ? "IL SEGRETARIO DELLA MARINA" : "ORDINE DEL MINISTRO DELLA DIFESA DELL'URSS", marina_, false);
    cornice(t, {18, 50, 252, 326}, usn ? StileCornice::oro : StileCornice::rossa, 8);
    disegna_ritratto(t, {26, 58, 236, 310}, ritratto(c_.dopo), &tav_);
    const ParametriRitratto prima = ritratto(c_.prima), dopo = ritratto(c_.dopo);
    cornice(t, {300, 60, 150, 96}, StileCornice::console, 5);
    disegna_insegna_grado(t, {305, 65, 140, 86}, marina_, prima.grado);
    cornice(t, {470, 60, 150, 96}, usn ? StileCornice::oro : StileCornice::rossa, 5);
    disegna_insegna_grado(t, {475, 65, 140, 86}, marina_, dopo.grado);
    for (int k = 0; k < 10; ++k) t.pixel(455 + k, 108, static_cast<std::uint8_t>(ORO + 6));
    paragrafo(t, f, 300, 162, 150, nome_grado(c_.prima), BIANCO + 2);
    paragrafo(t, f, 470, 162, 150, nome_grado(c_.dopo), ORO + 7);
    const Rettangolo foglio{300, 220, 320, 120};
    carta(t, foglio, 13);
    std::string testo = maiuscolo(c_.dopo.nome) + " e' promosso " + nome_grado(c_.dopo) + " con decorrenza " +
                        data_estesa_(c_.dopo.data) + ".";
    if (c_.esito.commissione && c_.esito.commissione->esito == "anticipata") testo += " Promozione anticipata, sotto zona.";
    paragrafo(t, f, foglio.x + 12, foglio.y + 12, foglio.w - 24, testo, NERO + 1);
    timbro(t, f, foglio.x + foglio.w - 60, foglio.y + foglio.h - 24, usn ? "APPROVED" : "UTVERZHDAYU", 6, 2, 9);
}

void SchermateEsito::disegna_memoria(Tela& t, const Font& f) const {
    const bool usn = marina_ == Marina::usn;
    tenda(t);
    intestazione(t, f, {0, 0, 640, 38}, "IN MEMORIAM", usn ? "ON ETERNAL PATROL" : "VECHNAYA PAMYAT", marina_, false);
    cornice(t, {18, 50, 252, 326}, usn ? StileCornice::oro : StileCornice::rossa, 8);
    disegna_ritratto(t, {26, 58, 236, 310}, ritratto(c_.prima), &tav_);
    // il nastro nero del lutto attraverso l'angolo in alto a destra della cornice
    t.togli_ritaglio();
    for (int i = 0; i < 80; ++i)
        for (int k = 0; k < 20; ++k) {
            const int x = 190 + i + k, y = 50 + i;
            if (x < 270 && y < 376) t.pixel(x, y, static_cast<std::uint8_t>(k < 2 || k > 17 ? colori::GRIGIO_SCURO : colori::NERO));
        }
    const Rettangolo foglio{300, 60, 320, 300};
    carta(t, foglio, 29);
    const std::string testo =
        (usn ? "Il comandante " + c_.prima.nome + ", " + nome_grado(c_.prima) + ", e gli uomini dell'equipaggio del " +
                   c_.contesto.battello + " sono caduti in servizio nella zona di " + c_.contesto.teatro + ", " +
                   data_estesa_(c_.dopo.data) + ".\nLa Marina degli Stati Uniti ne onora la memoria."
             : "Il comandante " + c_.prima.nome + ", " + nome_grado(c_.prima) + ", e l'equipaggio del sommergibile " +
                   c_.contesto.battello + " sono caduti nell'adempimento del dovere militare nella zona di " +
                   c_.contesto.teatro + ", " + data_estesa_(c_.dopo.data) + ".\nGloria eterna agli eroi.");
    paragrafo(t, f, foglio.x + 16, foglio.y + 20, foglio.w - 32, maiuscolo(testo), NERO + 1, foglio.y + foglio.h - 60);
    std::string servizio = "MISSIONI: " + std::to_string(c_.dopo.missioni.size()) + "   DECORAZIONI: " +
                           std::to_string(c_.dopo.decorazioni.size());
    f.scrivi(t, foglio.x + 16, foglio.y + foglio.h - 40, servizio, static_cast<std::uint8_t>(NERO + 2), 1);
}

// Nuovi riconoscimenti (achievement, roadmap 15): dopo l'esito e prima delle cerimonie, sei per pagina, ognuno col
// medaglione del kit al livello raggiunto, il nome, il passaggio di livello e la condizione
void SchermateEsito::disegna_riconoscimenti(Tela& t, const Font& f, int da) const {
    static constexpr const char* LIVELLI[] = {"", "BRONZO", "ARGENTO", "ORO"};
    tenda(t);
    intestazione(t, f, {0, 0, 640, 38}, "NUOVI RICONOSCIMENTI",
                 marina_ == Marina::usn ? "ACHIEVEMENTS" : "DOSTIZHENIYA", marina_, false);
    const int n = std::min<int>(RICONOSCIMENTI_PER_PAGINA, static_cast<int>(c_.riconoscimenti.size()) - da);
    for (int i = 0; i < n; ++i) {
        const RiconoscimentoNuovo& r = c_.riconoscimenti[static_cast<std::size_t>(da + i)];
        const Rettangolo riga{40, 52 + i * 62, 560, 56};
        carta(t, riga, static_cast<std::uint32_t>(41 + da + i));
        disegna_medaglione(t, {riga.x + 6, riga.y + 5, 48, 48}, r.icona, livello_medaglione(r.livello), true, false,
                           TavolozzaMedaglione::cerimonia, r.categoria);
        const int x = riga.x + 66;
        f.scrivi(t, x, riga.y + 7, maiuscolo(r.nome), static_cast<std::uint8_t>(NERO + 1), 2);
        std::string livello = r.prima ? std::string(LIVELLI[std::clamp(r.prima, 0, 3)]) + " -> " + LIVELLI[std::clamp(r.livello, 0, 3)]
                                      : std::string(LIVELLI[std::clamp(r.livello, 0, 3)]);
        if (r.solo_globale) livello += "   (FUORI CARRIERA)";
        f.scrivi(t, riga.x + riga.w - 12 - f.larghezza(livello, 1), riga.y + 10, livello, static_cast<std::uint8_t>(ORO + 2), 1);
        paragrafo(t, f, x, riga.y + 30, riga.w - 78, maiuscolo(r.descrizione), NERO + 3, riga.y + 44);
    }
    // il numero della pagina dei riconoscimenti sta nella riga d'aiuto del piede
}

bool SchermateEsito::fa_notizia() const {
    if (c_.contesto.operazione_speciale) return false;   // «non per la stampa» (dossier 13 §5.2)
    return c_.dettaglio.battello_perso || c_.dettaglio.neutrali_colpiti > 0 || c_.dettaglio.amici_colpiti > 0;
}

void SchermateEsito::disegna_giornale_esito(Tela& t, const Font& f) const {
    const bool usn = marina_ == Marina::usn;
    const EsitoDettagliato& d = c_.dettaglio;
    const std::string teatro = c_.contesto.teatro, battello = c_.contesto.battello;
    std::string titolo, testo;
    if (d.battello_perso && usn) {
        titolo = d.equipaggio_salvo ? "SALVATO L'EQUIPAGGIO DEL SOTTOMARINO " + maiuscolo(battello)
                                    : "LA MARINA: \"SOTTOMARINO IN RITARDO\"";
        testo = d.equipaggio_salvo
                    ? "Il comando delle forze subacquee dell'Atlantico ha confermato la perdita del sottomarino d'attacco " +
                          battello + " nella zona di " + teatro + ". L'equipaggio e' stato recuperato. La Marina ha "
                          "aperto un'inchiesta e non rilascia altre dichiarazioni."
                    : "Il comando delle forze subacquee dell'Atlantico ha comunicato che il sottomarino d'attacco " + battello +
                          " non ha trasmesso il rapporto previsto e viene considerato in ritardo. Navi e aerei da "
                          "pattugliamento sono impegnati nelle ricerche nella zona di " + teatro + ".";
    } else if (d.battello_perso) {
        titolo = "SOOBSHCHENIE TASS";
        testo = "Durante la navigazione nella zona di " + teatro + " a bordo di un sommergibile sovietico a propulsione "
                "nucleare si e' verificato un incidente. Sono state prese misure per l'assistenza all'equipaggio. Non c'e' "
                "alcun pericolo di contaminazione dell'ambiente. Una commissione governativa ne studia le cause.";
    } else if (usn) {
        const bool amico = d.amici_colpiti > 0 && d.neutrali_colpiti == 0;
        titolo = amico ? "INCIDENTE IN MARE: COLPITA UNA NAVE ALLEATA" : "PROTESTA DIPLOMATICA PER UNA NAVE COLPITA";
        testo = amico ? "Una nave dell'Alleanza e' stata colpita durante operazioni nella zona di " + teatro +
                            ". Il Pentagono parla di un tragico errore e annuncia un'inchiesta. La Marina non commenta "
                            "le operazioni dei sottomarini."
                      : "Il governo interessato ha convocato l'ambasciatore degli Stati Uniti dopo l'attacco a una nave "
                        "in acque internazionali nella zona di " + teatro + ". Fonti della Marina non confermano la "
                        "presenza di sottomarini americani nell'area. Il Congresso chiede chiarimenti.";
    } else {
        titolo = "SOOBSHCHENIE TASS";
        testo = "Sono prive di ogni fondamento le notizie diffuse dalla stampa occidentale su un presunto incidente nella "
                "zona di " + teatro + " che coinvolgerebbe un sommergibile sovietico. Si tratta di una nuova provocazione "
                "dei circoli militaristi della NATO.";
    }
    scrivania(t);
    const Rettangolo r = usn ? Rettangolo{120, 10, 400, 414} : Rettangolo{170, 10, 300, 414};
    disegna_giornale(t, f, r, marina_, usn ? "The Norfolk Herald" : "PRAVDA", titolo, testo, data_estesa_(c_.dopo.data), c_.dopo.seme);
}

void SchermateEsito::disegna_provvedimento(Tela& t, const Font& f) const {
    const bool usn = marina_ == Marina::usn;
    const Provvedimento& pv = *c_.esito.provvedimento;
    scrivania(t);
    const Rettangolo foglio{90, 30, 460, 380};
    carta(t, foglio, 21);
    std::string tipo = pv.tipo;
    std::replace(tipo.begin(), tipo.end(), '_', ' ');
    f.scrivi(t, foglio.x + 24, foglio.y + 20, usn ? "COMMANDER SUBMARINE FORCE" : "KOMANDUYUSHCHIY FLOTOM",
             static_cast<std::uint8_t>(NERO + 1), 2);
    f.scrivi(t, foglio.x + 24, foglio.y + 44, "PROVVEDIMENTO: " + maiuscolo(tipo), static_cast<std::uint8_t>(TIMBRO + 2), 1);
    int y = foglio.y + 64;
    y = paragrafo(t, f, foglio.x + 24, y, foglio.w - 48,
                  "Al comandante " + c_.prima.nome + ", " + nome_grado(c_.prima) + ", " + c_.contesto.battello + ".", NERO + 1);
    y += 8;
    y = paragrafo(t, f, foglio.x + 24, y, foglio.w - 48, pv.motivo.empty() ? std::string("Vedi il rapporto di pattuglia.") : pv.motivo,
                  NERO + 1);
    if (!c_.esito.stato.empty() && c_.esito.stato != "servizio")
        paragrafo(t, f, foglio.x + 24, y + 8, foglio.w - 48, "STATO: " + maiuscolo(c_.esito.stato), TIMBRO + 2);
    timbro(t, f, foglio.x + foglio.w - 90, foglio.y + foglio.h - 50, usn ? "CONFIDENTIAL" : "CEKPETHO", -10, 2, 17);
}

}  // namespace m688
