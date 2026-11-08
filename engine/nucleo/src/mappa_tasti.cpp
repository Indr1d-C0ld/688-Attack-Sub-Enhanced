#include "nucleo/mappa_tasti.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <toml.hpp>

namespace m688 {

namespace {
struct Predefinito {
    std::string_view comando, tasto;
};

// Tasti del manuale dell'originale [r. 3274-3366] e ordini rapidi del §5
constexpr Predefinito PREDEFINITI[] = {
    {"esci", "Esc"}, {"conferma", "Invio"}, {"conferma", "Tastierino Invio"},
    {"zona_avanti", "Tab"}, {"zona_indietro", "Maiusc+Tab"},
    {"cambia_ingrandimento", "Su"}, {"cambia_ingrandimento", "Giu"}, {"ingrandisci", "I"}, {"riduci", "O"},
    {"compressione_piu", "Pag Su"}, {"compressione_meno", "Pag Giu"},
    {"verita", "V"}, {"istantanea", "F12"}, {"uscita_schermo", "F10"},
    {"stazione_radio", "F2"}, {"stazione_stato", "F3"}, {"stazione_manovra", "F4"}, {"stazione_periscopio", "F5"}, {"stazione_navigazione", "F6"}, {"stazione_armi", "F7"},
    {"stazione_sonar", "F8"}, {"stazione_centrale", "F9"}, {"pausa", "P"},
    {"tubo_1", "R"}, {"tubo_5", "A"}, {"tubo_7", "M"},
    {"pulsante_1", "1"}, {"pulsante_2", "2"}, {"pulsante_3", "3"}, {"pulsante_4", "4"}, {"pulsante_5", "5"},
    {"pulsante_6", "6"}, {"pulsante_7", "7"}, {"pulsante_8", "8"}, {"pulsante_9", "9"},
    {"lancia", "L"}, {"bersaglio", "G"}, {"siluro", "S"}, {"cerca", "C"}, {"detona", "D"},
    {"punto_di_rotta", "W"}, {"cancella_punto", "K"}, {"proiezione", "J"}, {"sfondo", "B"}, {"mate", "T"},
    {"filo_sinistra", "Z"}, {"filo_dritta", "X"}, {"taglia_filo", "Y"}, {"variante_arma", "Ctrl+V"}, {"enciclopedia", "F1"},
    {"valore_meno", "["}, {"valore_piu", "]"},
    {"telegrafo_avanti", "+"}, {"telegrafo_avanti", "Tastierino +"}, {"telegrafo_avanti", "="},
    {"telegrafo_indietro", "-"}, {"telegrafo_indietro", "Tastierino -"},
    {"timone_sinistra", "Tastierino 4"}, {"timone_dritta", "Tastierino 6"}, {"timone_centro", "Tastierino 5"},
    {"barra_su", "Tastierino 8"}, {"barra_giu", "Tastierino 2"}, {"barra_zero", "Tastierino 0"},
    {"pilota_automatico", "."}, {"pilota_automatico", "Tastierino ."},
    {"periscopio", "U"}, {"antenna", "N"}, {"stadimetro", "H"},
    {"quota_periscopica", "Ctrl+P"}, {"quota_150_piedi", "Ctrl+1"}, {"emersione_emergenza", "Ctrl+E"},
    {"posati", "Ctrl+F"}, {"immersione_emergenza", "Ctrl+D"},
    // l'Alfa nell'Enhanced: Boksit, ПОКС e Akkord (anche dai pulsanti dei pulti)
    {"boksit_rotta", "Ctrl+B"}, {"boksit_quota", "Ctrl+U"}, {"boksit_tappa", "Ctrl+T"}, {"tan", "Ctrl+N"},
    {"sfoga", "Ctrl+G"}, {"compensa_piu", "Ctrl+K"}, {"compensa_meno", "Ctrl+J"},
    {"akkord_designa", "Ctrl+A"}, {"akkord_1", "Ctrl+5"}, {"akkord_2", "Ctrl+6"}, {"akkord_3", "Ctrl+7"},
    {"bqs15", "Ctrl+M"}, {"radar_giocatore", "Ctrl+R"}, {"punto_nave", "Ctrl+O"}, {"aiuto_comandi", "F11"}, {"muto", "Ctrl+Q"}, {"registro_messaggi", "Ctrl+L"}, {"guida_punto", "Ctrl+I"}, {"intercettazione", "Ctrl+W"}, {"quadro_fondale", "Ctrl+H"},
    {"libretto", "Ctrl+Z"}, {"osserva_aob", "Spazio"},
    {"silenzio_a_bordo", "Ctrl+S"}, {"carta_evento", "E"}, {"carta_aggancia", "F"}, {"carta_ekelund", "Q"}, {"carta_pulisci", "Ctrl+X"},
};
}  // namespace

const std::vector<VoceComando>& comandi_rimappabili() {
    static const std::vector<VoceComando> v = {
        {"esci", "esce dal gioco; in una scelta la annulla"},
        {"conferma", "conferma la scelta in corso"},
        {"zona_avanti", "scelta: zona successiva; Armi dell'Enhanced: campo successivo"},
        {"zona_indietro", "scelta: zona precedente"},
        {"cambia_ingrandimento", "periscopio: 1,5x o 6x"},
        {"ingrandisci", "periscopio: 6x; carta: zoom in avanti"},
        {"riduci", "carta: zoom all'indietro"},
        {"compressione_piu", "raddoppia la compressione del tempo"},
        {"compressione_meno", "dimezza la compressione del tempo"},
        {"verita", "diagnosi: classe, distanza e AOB veri"},
        {"istantanea", "salva la schermata in BMP"},
        {"uscita_schermo", "uscita a schermo: nitida, sharp bilinear, sharp bilinear col CRT (E9 §2.1)"},
        {"stazione_radio", "stazione Radio (archivio dei messaggi; nell'Enhanced la sala radio)"},
        {"stazione_stato", "stazione Stato (quadro danni)"},
        {"stazione_manovra", "stazione Manovra"},
        {"stazione_periscopio", "stazione Periscopio (Enhanced: i periscopi veri)"},
        {"stazione_navigazione", "stazione Navigazione (carta)"},
        {"stazione_armi", "stazione Armi"},
        {"stazione_sonar", "stazione Sonar"},
        {"stazione_centrale", "Centrale: stato rapido e menu (nel classico ferma il tempo)"},
        {"pausa", "pausa (Enhanced; nel classico ferma il tempo anche la Centrale)"},
        {"tubo_1", "tubo 1"}, {"tubo_5", "tubo 5 (primo ausiliario)"}, {"tubo_7", "tubo 7 (missili)"},
        {"tubo_2", "tubo 2 (pannello Armi della Fase 3)"}, {"tubo_3", "tubo 3"}, {"tubo_4", "tubo 4"}, {"tubo_6", "tubo 6"},
        {"pulsante_1", "pulsante 1 della stazione"}, {"pulsante_2", "pulsante 2"}, {"pulsante_3", "pulsante 3"},
        {"pulsante_4", "pulsante 4"}, {"pulsante_5", "pulsante 5"}, {"pulsante_6", "pulsante 6"},
        {"pulsante_7", "pulsante 7"}, {"pulsante_8", "pulsante 8"}, {"pulsante_9", "pulsante 9"},
        {"lancia", "lancio dal tubo selezionato"},
        {"bersaglio", "TARGET: scelta del bersaglio"},
        {"siluro", "siluro filoguidato successivo"},
        {"cerca", "classico: CERCA; Enhanced: testa attiva"},
        {"detona", "classico: DETONATE; Enhanced: spegnimento"},
        {"punto_di_rotta", "carta: nuovo punto di rotta"},
        {"cancella_punto", "carta: cancella l'ultimo punto"},
        {"proiezione", "carta: proiezione a N minuti"},
        {"sfondo", "carta: sfondo senza fondale (Enhanced: stile della carta)"},
        {"mate", "carta: strumento MATE della TMA"},
        {"filo_sinistra", "siluro via filo: rotta -10 gradi"},
        {"filo_dritta", "siluro via filo: rotta +10 gradi"},
        {"taglia_filo", "siluro: taglia il filo"},
        {"variante_arma", "Armi dell'Enhanced: arma successiva della rastrelliera nel tubo scelto"},
        {"enciclopedia", "il Museo dal menu; in missione la scheda dell'enciclopedia (Enhanced)"},
        {"valore_meno", "carta: minuti -1; Armi dell'Enhanced: valore precedente"},
        {"valore_piu", "carta: minuti +1; Armi dell'Enhanced: valore successivo"},
        {"telegrafo_avanti", "telegrafo avanti di una posizione"},
        {"telegrafo_indietro", "telegrafo indietro di una posizione"},
        {"timone_sinistra", "timone uno scatto a sinistra"},
        {"timone_dritta", "timone uno scatto a dritta"},
        {"timone_centro", "timone al centro"},
        {"barra_su", "barra di immersione su (Enhanced: quota -15 m)"},
        {"barra_giu", "barra di immersione giu' (Enhanced: quota +15 m)"},
        {"barra_zero", "barra a zero: tiene la quota"},
        {"pilota_automatico", "pilota automatico"},
        {"periscopio", "periscopio su o giu'"},
        {"antenna", "antenna su o giu'"},
        {"stadimetro", "periscopio: distanza del bersaglio dall'altezza dell'albero"},
        {"quota_periscopica", "quota periscopica"},
        {"quota_150_piedi", "150 piedi"},
        {"emersione_emergenza", "emersione d'emergenza"},
        {"posati", "Enhanced: posarsi sul fondo"},
        {"immersione_emergenza", "Enhanced: immersione d'emergenza, avanti tutta alla quota di sicurezza"},
        {"boksit_rotta", "Alfa: Boksit, modo della rotta (manuale, automatico, programma)"},
        {"boksit_quota", "Alfa: Boksit, modo della quota (manuale, automatico, programma)"},
        {"boksit_tappa", "Alfa: Boksit, il programma dai punti di rotta della carta"},
        {"tan", "Alfa: Tan, assetto automatico inserito o disinserito"},
        {"sfoga", "Alfa: ПОКС, apre gli sfoghi delle casse principali"},
        {"compensa_piu", "Alfa: ПОКС, imbarca mezzo metro cubo nelle casse d'assetto"},
        {"compensa_meno", "Alfa: ПОКС, espelle mezzo metro cubo dalle casse d'assetto"},
        {"akkord_designa", "Alfa: Akkord, designa il bersaglio corrente in uno slot libero"},
        {"akkord_1", "Alfa: Akkord, sceglie il bersaglio 1"},
        {"akkord_2", "Alfa: Akkord, sceglie il bersaglio 2"},
        {"akkord_3", "Alfa: Akkord, sceglie il bersaglio 3"},
        {"bqs15", "688: BQS-15, il sonar per le mine acceso o spento"},
        {"radar_giocatore", "Enhanced: il radar del battello, emerso o con l'albero su a quota periscopica"},
        {"punto_nave", "carta, navigazione realistica: il menu dei punti nave (Transit, Omega, stelle...)"},
        {"aiuto_comandi", "la schermata dei comandi, da ogni stazione"},
        {"muto", "classico: suoni spenti o accesi, come Ctrl-Q nel 1989 (Ctrl+N: senza campioni)"},
        {"registro_messaggi", "Enhanced: il registro dei messaggi di bordo, da ogni stazione"},
        {"intercettazione", "Enhanced: la pagina dell'intercettazione del sonar (WLR-9), da ogni stazione"},
        {"quadro_fondale", "Enhanced: la pagina del quadro del fondale alla Navigazione (ecoscandaglio, sezione di prua, ritorni), da ogni stazione"},
        {"libretto", "Enhanced: il libretto di riconoscimento accanto alla vista del Periscopio, da ogni stazione (Tab sfoglia, Invio identifica il bersaglio, [ ] l'altezza d'albero a mano, Z X l'AOB)"},
        {"osserva_aob", "Enhanced, libretto aperto: l'angolo sulla prora impostato va alla TMA, a periscopio alzato"},
        {"guida_punto", "carta: DIRECT, il siluro filoguidato scelto verso un punto"},
        {"silenzio_a_bordo", "Enhanced: silenzio a bordo (ricariche piu' lente e senza rumore)"},
        {"carta_evento", "carta dell'Enhanced: evento con l'ora sulla posizione del battello"},
        {"carta_aggancia", "carta dell'Enhanced: aggancia l'estremo del righello al battello o al contatto piu' vicino"},
        {"carta_ekelund", "carta dell'Enhanced: regolo di Ekelund"},
        {"carta_pulisci", "carta dell'Enhanced: pulisce il lucido (tratti, linee, eventi, datum)"},
    };
    return v;
}

MappaTasti MappaTasti::predefinita() {
    MappaTasti m;
    for (const auto& p : PREDEFINITI) m.lega(std::string(p.tasto), std::string(p.comando));
    return m;
}

void MappaTasti::lega(const std::string& tasto, const std::string& comando) {
    if (!tasto.empty()) tasto_comando_[tasto] = comando;   // un tasto ha un comando solo: l'ultimo vince
}

std::string MappaTasti::comando(const std::string& tasto) const {
    const auto it = tasto_comando_.find(tasto);
    return it == tasto_comando_.end() ? std::string{} : it->second;
}

std::vector<std::string> MappaTasti::tasti(std::string_view comando) const {
    std::vector<std::string> r;
    for (const auto& [t, c] : tasto_comando_)
        if (c == comando) r.push_back(t);
    return r;
}

void MappaTasti::applica_testo(const std::string& testo) {
    toml::table radice;
    try {
        radice = toml::parse(testo);
    } catch (const toml::parse_error& e) {
        throw std::runtime_error(std::string("tasti: ") + std::string(e.description()));
    }
    const auto* t = radice["tasti"].as_table();
    if (!t) return;
    for (const auto& [chiave, valore] : *t) {
        const std::string comando(chiave.str());
        bool noto = false;
        for (const auto& v : comandi_rimappabili()) noto = noto || v.id == comando;
        if (!noto) throw std::runtime_error("tasti: comando sconosciuto «" + comando + "»");
        std::vector<std::string> nuovi;
        if (const auto* s = valore.as_string()) nuovi.push_back(s->get());
        else if (const auto* a = valore.as_array()) {
            for (const auto& x : *a) {
                const auto* s2 = x.as_string();
                if (!s2) throw std::runtime_error("tasti: «" + comando + "» vuole nomi di tasti fra virgolette");
                nuovi.push_back(s2->get());
            }
        } else {
            throw std::runtime_error("tasti: «" + comando + "» vuole un tasto o un elenco di tasti");
        }
        for (const auto& vecchio : tasti(comando)) tasto_comando_.erase(vecchio);
        for (const auto& n : nuovi) lega(n, comando);
    }
}

void MappaTasti::carica(const std::string& percorso) {
    std::ifstream f(percorso);
    if (!f) throw std::runtime_error("tasti: impossibile aprire " + percorso);
    std::stringstream ss;
    ss << f.rdbuf();
    applica_testo(ss.str());
}

std::string MappaTasti::come_toml() const {
    std::string r = "# Tasti di 688 Attack Sub - Enhanced (melchior688 --tasti FILE). Per ogni comando un tasto o un elenco;\n"
                    "# \"\" toglie il comando. Nomi: lettere e cifre come sono, F1-F12, Tastierino 0-9 + - . Invio,\n"
                    "# Su Giu Sinistra Destra, Pag Su, Pag Giu, Invio, Esc, Tab, Spazio, con Ctrl+ e Maiusc+ davanti.\n\n[tasti]\n";
    for (const auto& v : comandi_rimappabili()) {
        const auto t = tasti(v.id);
        std::string valore;
        if (t.size() == 1) valore = "\"" + t.front() + "\"";
        else {
            valore = "[";
            for (std::size_t i = 0; i < t.size(); ++i) valore += (i ? ", \"" : "\"") + t[i] + "\"";
            valore += "]";
        }
        if (t.empty()) valore = "\"\"";
        r += std::string(v.id) + " = " + valore + "   # " + std::string(v.descrizione) + "\n";
    }
    return r;
}

}  // namespace m688
