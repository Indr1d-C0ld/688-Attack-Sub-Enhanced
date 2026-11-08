#include "stazione_radio.hpp"

#include <algorithm>
#include <cctype>
#include <cstdio>

#include "grafica/lamiera.hpp"
#include "grafica/palette.hpp"
#include "grafica/periscopio.hpp"
#include "grafica/tema_stazioni.hpp"
#include "ripetitori.hpp"

namespace m688 {

namespace {

constexpr int RIGA = 11;   // passo delle righe di testo alla scala 1

std::string maiuscolo(std::string s) {
    for (auto& c : s) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return s;
}

// «241530Z OCT 89» -> «1530Z»: l'ora del giorno per le righe corte
std::string ora_dtg(const std::string& dtg) { return dtg.size() >= 7 ? dtg.substr(2, 5) : dtg; }

const std::string& testo(const TestoRadio& t) { return t.it.empty() ? t.en : t.it; }

std::uint8_t colore_ricezione(Ricezione r) {
    return static_cast<std::uint8_t>(r == Ricezione::si ? colori::VERDE_CHIARO : r == Ricezione::meta ? colori::GIALLO : colori::GRIGIO);
}

const char* nome_ricezione(Ricezione r) { return r == Ricezione::si ? "SI" : r == Ricezione::meta ? "50 PER CENTO" : "NO"; }

// Riquadro con il bordo grigio
void riquadro(Tela& t, int x, int y, int w, int h) {
    t.rettangolo_pieno(x, y, w, h, colori::NERO);
    t.linea_orizzontale(x, x + w - 1, y, colori::GRIGIO_SCURO);
    t.linea_orizzontale(x, x + w - 1, y + h - 1, colori::GRIGIO_SCURO);
    t.linea_verticale(x, y, y + h - 1, colori::GRIGIO_SCURO);
    t.linea_verticale(x + w - 1, y, y + h - 1, colori::GRIGIO_SCURO);
}

}  // namespace

std::vector<std::string> StazioneRadio::a_capo(const Font& f, const std::string& testo, int larghezza) {
    std::vector<std::string> righe;
    std::size_t inizio = 0;
    while (inizio <= testo.size()) {   // paragrafo per paragrafo
        const std::size_t fine = std::min(testo.find('\n', inizio), testo.size());
        const std::string paragrafo = testo.substr(inizio, fine - inizio);
        std::string riga;
        std::size_t i = 0;
        while (i < paragrafo.size()) {
            const std::size_t j = std::min(paragrafo.find(' ', i), paragrafo.size());
            const std::string parola = paragrafo.substr(i, j - i);
            const std::string prova = riga.empty() ? parola : riga + " " + parola;
            if (!riga.empty() && f.larghezza(prova, 1) > larghezza) {
                righe.push_back(riga);
                riga = parola;
            } else {
                riga = prova;
            }
            i = j + 1;
        }
        righe.push_back(riga);
        inizio = fine + 1;
    }
    return righe;
}

std::vector<const Ricevuto*> StazioneRadio::archivio(const SalaRadio& r) const {
    std::vector<const Ricevuto*> v;
    if (solo_ordini_) v = r.ordini();
    else
        for (const auto& x : r.ricevuti()) v.push_back(&x);
    std::reverse(v.begin(), v.end());   // il piu' recente per primo
    return v;
}

void StazioneRadio::disegna(Tela& t, const Font& f, const Missione& m) const {
    t.rettangolo_pieno(0, 0, LARGHEZZA_TELA, ALTEZZA_VISTA, colori::NERO);
    const SalaRadio* r = m.radio();
    const auto* g = m.giocatore();
    if (!r || !g) {
        f.scrivi(t, 12, 8, "SALA RADIO NON DISPONIBILE", colori::GRIGIO, 1);
        return;
    }
    const double ora = m.tempo();
    char riga[200];
    // intestazione: data-ora e prossima finestra di comunicazione
    std::snprintf(riga, sizeof riga, "SALA RADIO   %s", r->dtg(ora).c_str());
    f.scrivi(t, 12, 4, riga, colori::CIANO_CHIARO, 1);
    std::snprintf(riga, sizeof riga, "PROSSIMA FINESTRA %s (PIU' O MENO %.0f MIN)", ora_dtg(r->dtg(r->prossima_finestra(ora))).c_str(),
                  r->tolleranza_finestra_s() / 60.0);
    f.scrivi(t, LARGHEZZA_TELA - 12 - f.larghezza(riga, 1), 4, riga, colori::CIANO_CHIARO, 1);
    // antenne e ricezione per banda
    const bool albero = (g->flag54 & 4) != 0 || (g->flag54 & 2) != 0;
    const char* filo = r->filo_rotto() ? "ROTTO" : r->filo() ? "FILATO" : r->filo_in_filata() ? "IN FILATA" : "RECUPERATO";
    std::snprintf(riga, sizeof riga, "ALBERO %s   FILO GALLEGGIANTE %s%s", albero ? "SU" : "GIU'", filo, r->elf() ? "   RICEVITORE ELF" : "");
    f.scrivi(t, 12, 20, riga, colori::BIANCO, 1);
    // boe SLOT: rimaste, ritardo scelto e trasmissione in attesa
    if (const auto attesa = m.slot_in_attesa())
        std::snprintf(riga, sizeof riga, "BOE SLOT %d  RITARDO %.0f MIN  IN ATTESA: TRASMETTE ALLE %s", m.boe_slot(), ritardo_s_ / 60.0,
                      ora_dtg(r->dtg(*attesa)).c_str());
    else std::snprintf(riga, sizeof riga, "BOE SLOT %d  RITARDO %.0f MIN", m.boe_slot(), ritardo_s_ / 60.0);
    f.scrivi(t, LARGHEZZA_TELA - 12 - f.larghezza(riga, 1), 20, riga, m.boe_slot() ? colori::BIANCO : colori::GRIGIO, 1);
    int x = 12;
    for (const Banda b : {Banda::elf, Banda::vlf, Banda::hf, Banda::uhf}) {
        if (b == Banda::elf && !r->elf()) continue;
        const Ricezione z = r->ricezione(b);
        std::snprintf(riga, sizeof riga, "%s %s", nome_banda(b), nome_ricezione(z));
        x = f.scrivi(t, x, 32, riga, colore_ricezione(z), 1) + 18;
    }
    // broadcast: in onda, in programma, buchi nella serie
    if (const auto o = r->in_onda()) {
        std::snprintf(riga, sizeof riga, "IN ONDA  SERIE %d  %c %s  %s-%s  %da MESSA IN ONDA", o->serie, sigla_precedenza(o->precedenza),
                      nome_precedenza(o->precedenza), ora_dtg(r->dtg(o->inizio)).c_str(), ora_dtg(r->dtg(o->fine)).c_str(), o->ripetizione);
        f.scrivi(t, 12, 48, riga, o->precedenza == Precedenza::flash ? colori::GIALLO : colori::BIANCO, 1);
        const double p = r->progresso();   // ricezione della messa in onda in corso
        t.rettangolo_pieno(LARGHEZZA_TELA - 132, 48, 120, 8, colori::GRIGIO_SCURO);
        if (p > 0.0) t.rettangolo_pieno(LARGHEZZA_TELA - 132, 48, std::max(1, static_cast<int>(120.0 * std::min(p, 1.0))), 8, colori::VERDE_CHIARO);
    } else {
        f.scrivi(t, 12, 48, "IN ONDA  -", colori::GRIGIO, 1);
    }
    std::string programma = "IN PROGRAMMA ";
    const auto prossimi = r->in_programma();
    for (std::size_t i = 0; i < std::min<std::size_t>(prossimi.size(), 5); ++i) {
        std::snprintf(riga, sizeof riga, "  %d %c %s", prossimi[i].serie, sigla_precedenza(prossimi[i].precedenza),
                      ora_dtg(r->dtg(prossimi[i].inizio)).c_str());
        programma += riga;
    }
    if (prossimi.empty()) programma += " -";
    f.scrivi(t, 12, 60, programma, colori::GRIGIO, 1);
    // sessioni obbligatorie (il guinzaglio sovietico, R2): quella in corso o la prossima, fatte e mancate
    if (const auto ss = r->sessioni(ora); !ss.empty()) {
        int fatte = 0, mancate = 0;
        const Sessione* prossima = nullptr;
        for (const auto& ses : ss) {
            fatte += ses.stato == Sessione::Stato::fatta;
            mancate += ses.stato == Sessione::Stato::mancata;
            if (!prossima && (ses.stato == Sessione::Stato::in_corso || ses.stato == Sessione::Stato::futura)) prossima = &ses;
        }
        std::string s = "SESSIONI";
        if (prossima)
            s += prossima->stato == Sessione::Stato::in_corso ? "  IN CORSO FINO ALLE " + ora_dtg(r->dtg(prossima->fine))
                                                               : "  PROSSIMA " + ora_dtg(r->dtg(prossima->inizio));
        s += "  FATTE " + std::to_string(fatte) + "  MANCATE " + std::to_string(mancate);
        f.scrivi(t, LARGHEZZA_TELA - 12 - f.larghezza(s, 1), 60, s, mancate ? colori::GIALLO : colori::BIANCO, 1);
    }
    const auto persi = r->persi();
    std::string buchi = persi.empty() ? "SERIE COMPLETA" : "MANCANO NELLA SERIE:";
    for (const int s : persi) buchi += " " + std::to_string(s);
    f.scrivi(t, 12, 72, buchi, persi.empty() ? colori::VERDE : colori::GIALLO, 1);
    // regole d'ingaggio in vigore secondo i messaggi ricevuti (fra parentesi la serie che le ha stabilite)
    const Roe roe = r->roe_ricevute();
    x = f.scrivi(t, 12, 84, "ROE", colori::CIANO_CHIARO, 1) + 10;
    for (int c = 0; c < static_cast<int>(CategoriaRoe::numero); ++c) {
        const StatoRoe st = roe.stato[static_cast<std::size_t>(c)];
        static constexpr const char* BREVI[] = {"SOTT.", "NAVI", "MERC.", "NEUTR.", "AEREI"};
        const int serie = roe.serie[static_cast<std::size_t>(c)];
        const bool dubbia = roe.dubbia[static_cast<std::size_t>(c)];   // da un messaggio che non si autentica
        std::snprintf(riga, sizeof riga, "%s %s%s%s", BREVI[c], maiuscolo(nome_stato_roe(st)).c_str(),
                      serie ? (" (" + std::to_string(serie) + ")").c_str() : "", dubbia ? " DUBBIA" : "");
        x = f.scrivi(t, x, 84, riga,
                     dubbia ? colori::GIALLO : st == StatoRoe::libero ? colori::VERDE_CHIARO : st == StatoRoe::stretto ? colori::GIALLO : colori::ROSSO, 1) + 12;
    }

    // telescrivente: il messaggio in arrivo mentre si riceve, altrimenti quello scelto nell'archivio
    const int bx = 8, by = 96, bw = LARGHEZZA_TELA - 16, bh = 144;
    riquadro(t, bx, by, bw, bh);
    const auto lista = archivio(*r);
    scelto_ = lista.empty() ? 0 : std::clamp(scelto_, 0, static_cast<int>(lista.size()) - 1);
    const int max_righe = (bh - 8) / RIGA;
    std::vector<std::pair<std::string, std::uint8_t>> righe;
    const TestoRadio in_arrivo = r->testo_in_arrivo();
    if (!testo(in_arrivo).empty()) {
        const auto o = r->in_onda();
        std::snprintf(riga, sizeof riga, "TELESCRIVENTE  IN ARRIVO SERIE %d  %.0f PER CENTO", o ? o->serie : 0, 100.0 * r->progresso());
        righe.emplace_back(riga, colori::CIANO_CHIARO);
        for (const auto& l : a_capo(f, maiuscolo(testo(in_arrivo)) + "_", bw - 16)) righe.emplace_back(l, colori::VERDE_CHIARO);
        if (righe.size() > static_cast<std::size_t>(max_righe))   // la telescrivente scorre: le ultime righe
            righe.erase(righe.begin() + 1, righe.begin() + 1 + static_cast<std::ptrdiff_t>(righe.size() - static_cast<std::size_t>(max_righe)));
    } else if (!lista.empty()) {
        const Ricevuto& ric = *lista[static_cast<std::size_t>(scelto_)];
        const MessaggioRadio& msg = ric.messaggio;
        std::snprintf(riga, sizeof riga, "%s  %c %s  %s  %s", msg.serie ? ("SERIE " + std::to_string(msg.serie)).c_str() : "ORDINI DI MISSIONE",
                      sigla_precedenza(msg.precedenza), nome_precedenza(msg.precedenza), msg.dtg.c_str(), maiuscolo(msg.classifica).c_str());
        righe.emplace_back(riga, colori::CIANO_CHIARO);
        righe.emplace_back("DA: " + maiuscolo(msg.da) + "   A: " + maiuscolo(msg.a), colori::BIANCO);
        if (!msg.oggetto.empty()) righe.emplace_back("OGG.: " + maiuscolo(msg.oggetto), colori::BIANCO);
        if (!msg.autenticazione.empty()) {   // il gruppo si confronta con la tabella del giorno solo quando lo si chiede
            const std::string esito = !ric.verificato ? ": DA VERIFICARE (INVIO)" : msg.autentico ? ": VALIDA" : ": NON VALIDA, POSSIBILE INGANNO";
            righe.emplace_back("AUTENTICAZIONE " + maiuscolo(msg.autenticazione) + esito,
                               !ric.verificato ? colori::BIANCO : msg.autentico ? colori::VERDE : colori::GIALLO);
        }
        if (ric.tardi) righe.emplace_back("RICEVUTO DOPO LA SCADENZA DEL COMPITO: NON VALE PIU'", colori::GIALLO);
        for (const auto& l : a_capo(f, maiuscolo(testo(msg.testo)), bw - 16)) righe.emplace_back(l, colori::BIANCO);
        if (righe.size() > static_cast<std::size_t>(max_righe)) {
            righe.resize(static_cast<std::size_t>(max_righe));
            righe.back() = {"...", colori::GRIGIO};
        }
    } else {
        righe.emplace_back(solo_ordini_ ? "NESSUN ORDINE DI MISSIONE" : "NESSUN MESSAGGIO RICEVUTO", colori::GRIGIO);
    }
    for (std::size_t i = 0; i < righe.size(); ++i)
        f.scrivi(t, bx + 8, by + 5 + static_cast<int>(i) * RIGA, righe[i].first, righe[i].second, 1);

    // archivio: i ricevuti, il piu' recente in alto, lo scelto evidenziato
    const int ly = by + bh + 6;
    f.scrivi(t, 12, ly, solo_ordini_ ? "ORDINI DI MISSIONE" : "MESSAGGI RICEVUTI", colori::CIANO_CHIARO, 1);
    const int visibili = (ALTEZZA_VISTA - ly - 16) / RIGA;
    const int primo = std::max(0, scelto_ - visibili + 1);
    for (int i = primo; i < std::min(static_cast<int>(lista.size()), primo + visibili); ++i) {
        const Ricevuto& ric = *lista[static_cast<std::size_t>(i)];
        const MessaggioRadio& msg = ric.messaggio;
        std::snprintf(riga, sizeof riga, "%3d  %c  %-12s  %-4s  %-16.16s  %s", msg.serie, sigla_precedenza(msg.precedenza), msg.dtg.c_str(),
                      nome_banda(ric.banda), maiuscolo(msg.da).c_str(), maiuscolo(msg.oggetto).c_str());
        const int y = ly + 14 + (i - primo) * RIGA;
        if (i == scelto_) t.rettangolo_pieno(8, y - 2, LARGHEZZA_TELA - 16, RIGA, colori::GRIGIO_SCURO);
        f.scrivi(t, 12, y, riga, msg.precedenza == Precedenza::flash ? colori::GIALLO : colori::BIANCO, 1);
    }
}

void StazioneRadio::disegna_sala(Tela& t, const Font& f, const Font& incisa, const Missione& m, lamiera::Scritte sc, bool sovietica) const {
    using ripetitori::largo;
    using ripetitori::scrivi;
    using ripetitori::taglia;
    lamiera::lamiera(t, {0, 0, LARGHEZZA_TELA, ALTEZZA_VISTA}, 7, 5);
    const SalaRadio* r = m.radio();
    const auto* g = m.giocatore();
    if (!r || !g) {
        lamiera::secondaria(t, f, 14, 11, "SALA RADIO NON DISPONIBILE", false, sc);
        return;
    }
    const double ora = m.tempo();
    char riga[200];
    // la fascia di stato: il gruppo data-ora, le antenne e le bande in spie, le ROE, la finestra e la sessione
    lamiera::pannello(t, {4, 4, 632, 56}, 8);
    lamiera::principale(t, incisa, 14, 11, (sovietica ? "RADIORUBKA  " : "SALA RADIO  ") + r->dtg(ora), sc);
    struct Spia { std::string nome; ColoreLampada c; bool accesa; };
    std::vector<Spia> antenne = {
        {"ALBERO", ColoreLampada::verde, (g->flag54 & 4) != 0 || (g->flag54 & 2) != 0},
        // il filo: verde filato, ambra mentre si fila, rosso rotto
        {"FILO", r->filo_rotto() ? ColoreLampada::rossa : r->filo_in_filata() ? ColoreLampada::ambra : ColoreLampada::verde,
         r->filo() || r->filo_in_filata() || r->filo_rotto()}};
    for (const Banda b : {Banda::elf, Banda::vlf, Banda::hf, Banda::uhf}) {   // la ricezione: verde si', ambra a meta'
        if (b == Banda::elf && !r->elf()) continue;
        const Ricezione z = r->ricezione(b);
        antenne.push_back({nome_banda(b), z == Ricezione::meta ? ColoreLampada::ambra : ColoreLampada::verde, z != Ricezione::no});
    }
    int x = 14;
    for (const auto& s : antenne) {
        lamiera::spia_voce(t, {x, 28, 12, 9}, s.c, s.accesa);
        if (s.accesa) lamiera::principale(t, f, x + 16, 29, s.nome, sc);
        else lamiera::secondaria(t, f, x + 16, 29, s.nome, false, sc);
        x += 22 + largo(f, s.nome);
    }
    // le ROE ricevute: rossa trattenuta, ambra stretta o dubbia (col punto di domanda), verde libera
    lamiera::secondaria(t, f, 14, 45, "ROE", false, sc);
    const Roe roe = r->roe_ricevute();
    x = 44;
    for (int c = 0; c < static_cast<int>(CategoriaRoe::numero); ++c) {
        static constexpr const char* BREVI[] = {"SOTT.", "NAVI", "MERC.", "NEUTR.", "AEREI"};
        const StatoRoe st = roe.stato[static_cast<std::size_t>(c)];
        const bool dubbia = roe.dubbia[static_cast<std::size_t>(c)];
        const ColoreLampada col = dubbia || st == StatoRoe::stretto ? ColoreLampada::ambra
                                  : st == StatoRoe::libero        ? ColoreLampada::verde
                                                                  : ColoreLampada::rossa;
        const std::string nome = std::string(BREVI[c]) + (dubbia ? "?" : "");
        lamiera::spia_voce(t, {x, 44, 12, 9}, col, true);
        lamiera::principale(t, f, x + 16, 45, nome, sc);
        x += 22 + largo(f, nome);
    }
    // la prossima finestra: l'ora e la tolleranza nel vetro, la barra del tempo trascorso dalla finestra prima
    lamiera::secondaria(t, f, 384, 13, "PROSSIMA FINESTRA", false, sc);
    lamiera::vetro(t, {384, 24, 240, 14});
    const double finestra = r->prossima_finestra(ora);
    std::snprintf(riga, sizeof riga, "%s +- %.0f MIN", ora_dtg(r->dtg(finestra)).c_str(), r->tolleranza_finestra_s() / 60.0);
    scrivi(t, f, 388, 27, riga, fosforo(6));
    const double periodo = r->parametri().finestra_ogni_s > 0.0 ? r->parametri().finestra_ogni_s : 12.0 * 3600.0;   // USA e URSS: due al giorno
    const double trascorso = std::clamp(1.0 - (finestra - ora) / periodo, 0.0, 1.0);
    t.rettangolo_pieno(514, 28, 104, 6, fosforo(1));
    if (trascorso > 0.0) t.rettangolo_pieno(514, 28, std::max(1, static_cast<int>(104.0 * trascorso)), 6, fosforo(5));
    // la sessione obbligatoria (R2) in corso o la prossima; senza sessioni le boe SLOT, che altrimenti vanno a destra
    const auto attesa = m.slot_in_attesa();
    std::snprintf(riga, sizeof riga, "BOE SLOT %d, RITARDO %.0f MIN%s", m.boe_slot(), ritardo_s_ / 60.0,
                  attesa ? (", ATTESA " + ora_dtg(r->dtg(*attesa))).c_str() : "");
    const std::string slot = riga;
    const auto ss = r->sessioni(ora);
    if (!ss.empty()) {
        int fatte = 0, mancate = 0;
        const Sessione* prossima = nullptr;
        for (const auto& ses : ss) {
            fatte += ses.stato == Sessione::Stato::fatta;
            mancate += ses.stato == Sessione::Stato::mancata;
            if (!prossima && (ses.stato == Sessione::Stato::in_corso || ses.stato == Sessione::Stato::futura)) prossima = &ses;
        }
        std::string s = "SESSIONE";
        if (prossima)
            s += prossima->stato == Sessione::Stato::in_corso ? " FINO " + ora_dtg(r->dtg(prossima->fine)) : " " + ora_dtg(r->dtg(prossima->inizio));
        s += ", FATTE " + std::to_string(fatte) + ", MANCATE " + std::to_string(mancate);
        // le sessioni mancate in giallo inciso sul grigio; sul beige il giallo non si legge: rosso piatto
        if (mancate && sc == lamiera::Scritte::scure) scrivi(t, f, 384, 45, taglia(f, s, 248), colori::ROSSO);
        else if (mancate) lamiera::incisa(t, f, 384, 45, taglia(f, s, 248), colori::GIALLO, colori::NERO);
        else lamiera::secondaria(t, f, 384, 45, taglia(f, s, 248), false, sc);
    } else {
        lamiera::secondaria(t, f, 384, 45, taglia(f, slot, 248), false, sc);
    }

    // la telescrivente: il messaggio in arrivo mentre si riceve, altrimenti quello scelto nell'archivio
    lamiera::pannello(t, {4, 64, 424, ALTEZZA_VISTA - 68}, 8);
    const int passo = f.righe() + 3;
    const Rettangolo a = lamiera::modulo_continuo(t, {12, 72, 408, ALTEZZA_VISTA - 84}, passo);
    const int ultima = a.y + a.h - passo;   // la riga del seguito
    const auto lista = archivio(*r);
    scelto_ = lista.empty() ? 0 : std::clamp(scelto_, 0, static_cast<int>(lista.size()) - 1);
    const TestoRadio in_arrivo = r->testo_in_arrivo();
    const bool arriva = !testo(in_arrivo).empty();
    int y = a.y;
    if (arriva) {
        const auto o = r->in_onda();
        std::snprintf(riga, sizeof riga, "IN ARRIVO  SERIE %d  %c %s  %.0f PER CENTO", o ? o->serie : 0,
                      o ? sigla_precedenza(o->precedenza) : '-', o ? nome_precedenza(o->precedenza) : "", 100.0 * r->progresso());
        scrivi(t, f, a.x, y, taglia(f, riga, a.w), colori::BLU), y += 2 * passo;
        // la telescrivente scorre: le ultime righe arrivate
        const auto righe = a_capo(f, maiuscolo(testo(in_arrivo)) + "_", a.w);
        const int posto = std::max(1, (ultima + passo - y) / passo);
        for (std::size_t i = righe.size() > static_cast<std::size_t>(posto) ? righe.size() - static_cast<std::size_t>(posto) : 0; i < righe.size(); ++i)
            scrivi(t, f, a.x, y, righe[i], colori::NERO), y += passo;
    } else if (!lista.empty()) {
        const Ricevuto& ric = *lista[static_cast<std::size_t>(scelto_)];
        const MessaggioRadio& msg = ric.messaggio;
        // la prima riga: la serie, la precedenza e il gruppo data-ora; il timbro della classifica a destra
        std::snprintf(riga, sizeof riga, "%s  %c %s  %s", msg.serie ? ("SERIE " + std::to_string(msg.serie)).c_str() : "ORDINI",
                      sigla_precedenza(msg.precedenza), nome_precedenza(msg.precedenza), msg.dtg.c_str());
        const std::string classifica = maiuscolo(msg.classifica);
        const int w_timbro = classifica.empty() ? 0 : largo(f, classifica) + 20;
        scrivi(t, f, a.x, y, taglia(f, riga, a.w - w_timbro), colori::NERO);
        if (!classifica.empty()) lamiera::timbro(t, f, a.x + a.w - largo(f, classifica) - 12, y - 2, classifica, colori::ROSSO);
        y += passo;
        std::vector<std::pair<std::string, std::uint8_t>> righe;
        righe.emplace_back("DA: " + maiuscolo(msg.da) + "   A: " + maiuscolo(msg.a), colori::NERO);
        if (!msg.oggetto.empty()) righe.emplace_back("OGG.: " + maiuscolo(msg.oggetto), colori::NERO);
        if (!msg.autenticazione.empty()) {   // il gruppo si confronta con la tabella del giorno solo quando lo si chiede
            const std::string esito = !ric.verificato ? ": DA VERIFICARE (INVIO)" : msg.autentico ? ": VALIDA" : ": NON VALIDA, POSSIBILE INGANNO";
            righe.emplace_back("AUTENTICAZIONE " + maiuscolo(msg.autenticazione) + esito,
                               !ric.verificato ? colori::BLU : msg.autentico ? colori::NERO : colori::ROSSO);
        }
        if (ric.tardi) righe.emplace_back("RICEVUTO DOPO LA SCADENZA DEL COMPITO: NON VALE PIU'", colori::ROSSO);
        righe.emplace_back("", colori::NERO);
        for (const auto& l : a_capo(f, maiuscolo(testo(msg.testo)), a.w)) righe.emplace_back(l, colori::NERO);
        std::size_t i = 0;
        for (; i < righe.size() && y + passo <= ultima; ++i) scrivi(t, f, a.x, y, taglia(f, righe[i].first, a.w), righe[i].second), y += passo;
        if (i < righe.size()) {   // il testo continua: quante righe restano fuori dal foglio
            std::snprintf(riga, sizeof riga, "(SEGUE, ALTRE %zu RIGHE)", righe.size() - i);
            scrivi(t, f, a.x, ultima, riga, colori::BLU);
        }
    } else {
        scrivi(t, f, a.x, y, solo_ordini_ ? "NESSUN ORDINE DI MISSIONE" : "NESSUN MESSAGGIO RICEVUTO", colori::NERO);
    }

    // a destra: in onda, in programma (con i buchi nella serie), i messaggi ricevuti
    lamiera::pannello(t, {432, 64, 204, ALTEZZA_VISTA - 68}, 8);
    const int xd = 440, wd = 188;
    int yd = 72;
    std::vector<std::pair<std::string, std::uint8_t>> onda;
    if (const auto o = r->in_onda()) {
        const std::uint8_t c = o->precedenza == Precedenza::flash ? static_cast<std::uint8_t>(colori::GIALLO) : fosforo(6);
        std::snprintf(riga, sizeof riga, "SERIE %d  %c %s", o->serie, sigla_precedenza(o->precedenza), nome_precedenza(o->precedenza));
        onda.emplace_back(taglia(f, riga, wd - 8), c);
        std::snprintf(riga, sizeof riga, "%s-%s, %dA MESSA", ora_dtg(r->dtg(o->inizio)).c_str(), ora_dtg(r->dtg(o->fine)).c_str(), o->ripetizione);
        onda.emplace_back(taglia(f, riga, wd - 8), c);
        if (r->progresso() > 0.0) {
            std::snprintf(riga, sizeof riga, "RICEZIONE %.0f PER CENTO", 100.0 * std::min(r->progresso(), 1.0));
            onda.emplace_back(riga, fosforo(5));
        }
    } else {
        onda.emplace_back("-", fosforo(3));
    }
    yd += lamiera::campo_vetro(t, f, xd, yd, wd, "IN ONDA", onda, sc) + 4;
    std::vector<std::pair<std::string, std::uint8_t>> programma;
    const auto prossimi = r->in_programma();
    for (std::size_t i = 0; i < std::min<std::size_t>(prossimi.size(), 4); i += 2) {   // due per riga
        std::string s;
        for (std::size_t k = i; k < std::min<std::size_t>({prossimi.size(), i + 2, 4}); ++k) {
            std::snprintf(riga, sizeof riga, "%s%d %c %s", s.empty() ? "" : "   ", prossimi[k].serie, sigla_precedenza(prossimi[k].precedenza),
                          ora_dtg(r->dtg(prossimi[k].inizio)).c_str());
            s += riga;
        }
        programma.emplace_back(s, fosforo(5));
    }
    if (prossimi.empty()) programma.emplace_back("-", fosforo(3));
    const auto persi = r->persi();
    std::string buchi = persi.empty() ? "SERIE COMPLETA" : "MANCANO:";
    for (const int s : persi) buchi += " " + std::to_string(s);
    programma.emplace_back(taglia(f, buchi, wd - 8), persi.empty() ? fosforo(7) : static_cast<std::uint8_t>(colori::GIALLO));
    if (!ss.empty()) programma.emplace_back(taglia(f, slot, wd - 8), fosforo(5));   // la fascia ha la sessione
    yd += lamiera::campo_vetro(t, f, xd, yd, wd, "IN PROGRAMMA", programma, sc) + 4;
    lamiera::secondaria(t, f, xd, yd, solo_ordini_ ? "ORDINI DI MISSIONE" : "MESSAGGI RICEVUTI", false, sc);
    yd += 12;
    // una riga per messaggio, il piu' recente in alto: serie, precedenza, ora, banda, oggetto; quello in arrivo per primo
    std::vector<std::string> voci;
    if (arriva)
        if (const auto o = r->in_onda()) {
            std::snprintf(riga, sizeof riga, "%d %c %s IN ARRIVO", o->serie, sigla_precedenza(o->precedenza), ora_dtg(r->dtg(o->inizio)).c_str());
            voci.emplace_back(riga);
        }
    const int primo_archivio = static_cast<int>(voci.size());
    for (const Ricevuto* ric : lista) {
        const MessaggioRadio& msg = ric->messaggio;
        std::snprintf(riga, sizeof riga, "%d %c %s %s %s", msg.serie, sigla_precedenza(msg.precedenza), ora_dtg(msg.dtg).c_str(),
                      nome_banda(ric->banda), maiuscolo(msg.oggetto).c_str());
        voci.push_back(taglia(f, riga, wd - 18));
    }
    const Rettangolo el{xd, yd, wd, ALTEZZA_VISTA - 12 - yd};
    const int scelta = arriva ? 0 : primo_archivio + scelto_;
    const int visibili = std::max(1, (el.h - 6) / 14);
    lamiera::elenco_vetro(t, el, f, voci, voci.empty() ? -1 : scelta, 14, std::max(0, scelta - visibili + 1));
}

void StazioneRadio::disegna_classico(Tela& t, const Font& f, const Missione& m, const std::vector<RigaStorico>& storico) const {
    t.rettangolo_pieno(0, 0, LARGHEZZA_TELA, ALTEZZA_VISTA, colori::NERO);
    const auto* g = m.giocatore();
    char riga[200];
    std::snprintf(riga, sizeof riga, "SALA RADIO   PERISCOPIO %s   ANTENNA %s", g && (g->flag54 & 2) ? "SU" : "GIU'",
                  g && (g->flag54 & 4) ? "SU" : "GIU'");
    f.scrivi(t, 12, 4, riga, colori::CIANO_CHIARO, 1);
    // il messaggio scelto per intero, poi l'archivio dei messaggi di bordo, il piu' recente in alto
    const int n = static_cast<int>(storico.size());
    scelto_ = n ? std::clamp(scelto_, 0, n - 1) : 0;
    riquadro(t, 8, 20, LARGHEZZA_TELA - 16, 60);
    if (n) {
        const auto& [tempo, testo_scelto] = storico[static_cast<std::size_t>(n - 1 - scelto_)];
        const int s = static_cast<int>(tempo);
        std::snprintf(riga, sizeof riga, "%02d:%02d:%02d", s / 3600, s / 60 % 60, s % 60);
        f.scrivi(t, 16, 25, riga, colori::CIANO_CHIARO, 1);
        const auto righe = a_capo(f, testo_scelto, LARGHEZZA_TELA - 32);
        for (std::size_t i = 0; i < std::min<std::size_t>(righe.size(), 4); ++i)
            f.scrivi(t, 16, 37 + static_cast<int>(i) * RIGA, righe[i], colori::VERDE_CHIARO, 1);
    } else {
        f.scrivi(t, 16, 25, "NESSUN MESSAGGIO", colori::GRIGIO, 1);
    }
    const int ly = 90, visibili = (ALTEZZA_VISTA - ly - 8) / RIGA;
    const int primo = std::max(0, scelto_ - visibili + 1);
    for (int i = primo; i < std::min(n, primo + visibili); ++i) {
        const auto& [tempo, testo_riga] = storico[static_cast<std::size_t>(n - 1 - i)];
        const int s = static_cast<int>(tempo), y = ly + (i - primo) * RIGA;
        std::snprintf(riga, sizeof riga, "%02d:%02d:%02d  %s", s / 3600, s / 60 % 60, s % 60, testo_riga.c_str());
        if (i == scelto_) t.rettangolo_pieno(8, y - 2, LARGHEZZA_TELA - 16, RIGA, colori::GRIGIO_SCURO);
        f.scrivi(t, 12, y, riga, colori::BIANCO, 1);
    }
}

}  // namespace m688
