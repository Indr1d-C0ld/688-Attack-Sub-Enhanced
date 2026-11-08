#include "nucleo/ordini.hpp"

#include <algorithm>
#include <array>
#include <map>
#include <cstdio>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace m688 {

namespace {

constexpr std::array<std::pair<TipoOrdine, std::string_view>, 66> NOMI = {{
    {TipoOrdine::telegrafo, "telegrafo"},
    {TipoOrdine::timone, "timone"},
    {TipoOrdine::quota, "quota"},
    {TipoOrdine::quota_periscopica, "quota_periscopica"},
    {TipoOrdine::quota_150_piedi, "quota_150_piedi"},
    {TipoOrdine::emersione_emergenza, "emersione_emergenza"},
    {TipoOrdine::punto_di_rotta, "punto_di_rotta"},
    {TipoOrdine::cancella_punto, "cancella_punto"},
    {TipoOrdine::pilota_automatico, "pilota_automatico"},
    {TipoOrdine::periscopio, "periscopio"},
    {TipoOrdine::antenna, "antenna"},
    {TipoOrdine::schiera_cala, "schiera_cala"},
    {TipoOrdine::schiera_recupera, "schiera_recupera"},
    {TipoOrdine::schiera_taglia, "schiera_taglia"},
    {TipoOrdine::impulso_sonar, "impulso_sonar"},
    {TipoOrdine::filtro_sonar, "filtro_sonar"},
    {TipoOrdine::designa_bersaglio, "designa_bersaglio"},
    {TipoOrdine::carica_tubo, "carica_tubo"},
    {TipoOrdine::lancia, "lancia"},
    {TipoOrdine::seleziona_siluro, "seleziona_siluro"},
    {TipoOrdine::guida_punto, "guida_punto"},
    {TipoOrdine::guida_cerca, "guida_cerca"},
    {TipoOrdine::guida_detona, "guida_detona"},
    {TipoOrdine::spegni_siluro, "spegni_siluro"},
    {TipoOrdine::taglia_filo, "taglia_filo"},
    {TipoOrdine::compressione_tempo, "compressione_tempo"},
    {TipoOrdine::barra, "barra"},
    {TipoOrdine::accosta_per, "accosta_per"},
    {TipoOrdine::giri, "giri"},
    {TipoOrdine::quota_con_angolo, "quota_con_angolo"},
    {TipoOrdine::angolo_immersione, "angolo_immersione"},
    {TipoOrdine::imposta_siluro, "imposta_siluro"},
    {TipoOrdine::comando_filo, "comando_filo"},
    {TipoOrdine::radio_filo, "radio_filo"},
    {TipoOrdine::radio_trasmetti, "radio_trasmetti"},
    {TipoOrdine::radio_ssixs, "radio_ssixs"},
    {TipoOrdine::radio_slot, "radio_slot"},
    {TipoOrdine::silenzio_a_bordo, "silenzio_a_bordo"},
    {TipoOrdine::radio_verifica, "radio_verifica"},
    {TipoOrdine::scegli_arma_tubo, "scegli_arma_tubo"},
    {TipoOrdine::classifica_contatto, "classifica_contatto"},
    {TipoOrdine::scegli_periscopio, "scegli_periscopio"},
    {TipoOrdine::immersione_emergenza, "immersione_emergenza"},
    {TipoOrdine::elevazione_periscopio, "elevazione_periscopio"},
    {TipoOrdine::foto_periscopio, "foto_periscopio"},
    {TipoOrdine::modo_boksit, "modo_boksit"},
    {TipoOrdine::programma_tappa, "programma_tappa"},
    {TipoOrdine::tan, "tan"},
    {TipoOrdine::sfoga, "sfoga"},
    {TipoOrdine::compensa, "compensa"},
    {TipoOrdine::akkord_designa, "akkord_designa"},
    {TipoOrdine::akkord_scegli, "akkord_scegli"},
    {TipoOrdine::bqs15, "bqs15"},
    {TipoOrdine::radar, "radar"},
    {TipoOrdine::punto_nave, "punto_nave"},
    {TipoOrdine::imposta_missile, "imposta_missile"},
    {TipoOrdine::prepara_missile, "prepara_missile"},
    {TipoOrdine::ecoscandaglio_ping, "ecoscandaglio_ping"},
    {TipoOrdine::ecoscandaglio_periodo, "ecoscandaglio_periodo"},
    {TipoOrdine::ecoscandaglio_potenza, "ecoscandaglio_potenza"},
    {TipoOrdine::identifica_visiva, "identifica_visiva"},
    {TipoOrdine::osserva_aob, "osserva_aob"},
    {TipoOrdine::misura_telemetro, "misura_telemetro"},
    {TipoOrdine::luci_giocatore, "luci_giocatore"},
    {TipoOrdine::isola_compartimento, "isola_compartimento"},
    {TipoOrdine::priorita_sistema, "priorita_sistema"},
}};

}  // namespace

std::string_view nome_ordine(TipoOrdine t) {
    for (const auto& [k, v] : NOMI)
        if (k == t) return v;
    return "?";
}

std::optional<TipoOrdine> ordine_da_nome(std::string_view nome) {
    for (const auto& [k, v] : NOMI)
        if (v == nome) return k;
    return std::nullopt;
}

void RegistroOrdini::ordina(const Ordine& o) { coda_.push_back({o, sequenza_++}); }

std::vector<Ordine> RegistroOrdini::consegna(double t) {
    std::vector<InCoda> pronti;
    std::erase_if(coda_, [&](const InCoda& c) {
        if (c.o.tempo > t) return false;
        pronti.push_back(c);
        return true;
    });
    std::sort(pronti.begin(), pronti.end(), [](const InCoda& x, const InCoda& y) {
        return x.o.tempo != y.o.tempo ? x.o.tempo < y.o.tempo : x.sequenza < y.sequenza;
    });
    std::vector<Ordine> out;
    out.reserve(pronti.size());
    for (auto& c : pronti) {
        // un ordine arrivato in ritardo (istante gia' passato) vale ora: lo storico registra l'istante effettivo, cosi'
        // il replay lo consegna allo stesso passo
        if (c.o.tempo <= ultimo_) c.o.tempo = t;
        out.push_back(c.o);
        storico_.push_back(c.o);
    }
    ultimo_ = t;
    return out;
}

namespace {

// righe "carico ..." dello storico (RegistroOrdini::serializza)
std::string righe_carico(const CaricoPartenza& c) {
    std::string s;
    for (const auto& [campo, v] : {std::pair<const char*, const std::vector<std::string>*>{"posizioni", &c.posizioni}, {"tipi", &c.tipi}}) {
        s += std::string("carico ") + campo + " " + std::to_string(v->size());
        for (const auto& x : *v) s += " " + (x.empty() ? std::string("-") : x);
        s += "\n";
    }
    for (const auto& [campo, m] : {std::pair<const char*, const std::map<std::string, int>*>{"rastrelliera", &c.rastrelliera},
                                   {"pozzi", &c.pozzi}, {"esche", &c.esche}}) {
        s += std::string("carico ") + campo;
        for (const auto& [arma, n] : *m) s += " " + arma + "=" + std::to_string(n);
        s += "\n";
    }
    return s;
}

// una riga "carico <campo> ..." (senza la parola iniziale) nel carico; falso se non e' valida
bool leggi_carico(std::istringstream& campi, CaricoPartenza& c) {
    std::string campo;
    if (!(campi >> campo)) return false;
    if (campo == "posizioni" || campo == "tipi") {
        auto& v = campo == "posizioni" ? c.posizioni : c.tipi;
        std::size_t n = 0;
        if (!(campi >> n)) return false;
        v.clear();
        for (std::size_t i = 0; i < n; ++i) {
            std::string x;
            if (!(campi >> x)) return false;
            v.push_back(x == "-" ? std::string{} : x);
        }
        return true;
    }
    auto* m = campo == "rastrelliera" ? &c.rastrelliera : campo == "pozzi" ? &c.pozzi : campo == "esche" ? &c.esche : nullptr;
    if (!m) return false;
    m->clear();
    for (std::string x; campi >> x;) {
        const auto uguale = x.find('=');
        if (uguale == std::string::npos || uguale == 0) return false;
        try {
            (*m)[x.substr(0, uguale)] = std::stoi(x.substr(uguale + 1));
        } catch (const std::exception&) {
            return false;
        }
    }
    return true;
}

}  // namespace

Ordine ordine_classifica(double t, int contatto, const std::string& classe) {
    Ordine o{t, TipoOrdine::classifica_contatto, 0.0, 0.0, contatto, classe};
    std::replace(o.testo.begin(), o.testo.end(), ' ', '_');
    return o;
}

Ordine ordine_misura_telemetro(double t, int contatto, double letta_m, double altezza_m) {
    return {t, TipoOrdine::misura_telemetro, letta_m, altezza_m, contatto};
}

Ordine ordine_identifica_visiva(double t, int contatto, const std::string& classe) {
    Ordine o = ordine_classifica(t, contatto, classe);
    o.tipo = TipoOrdine::identifica_visiva;
    return o;
}

std::string RegistroOrdini::serializza() const {
    std::string s = carico_ ? righe_carico(*carico_) : std::string{};
    char riga[160];
    for (const auto& o : storico_) {
        std::snprintf(riga, sizeof riga, "%.17g %s %.17g %.17g %d", o.tempo, std::string(nome_ordine(o.tipo)).c_str(), o.a, o.b, o.n);
        s += riga;
        if (!o.testo.empty()) s += ' ' + o.testo;
        s += '\n';
    }
    return s;
}

RegistroOrdini RegistroOrdini::da_testo(const std::string& testo) {
    RegistroOrdini r;
    std::istringstream in(testo);
    std::string riga;
    int numero = 0;
    while (std::getline(in, riga)) {
        ++numero;
        if (riga.empty() || riga[0] == '#') continue;
        std::istringstream campi(riga);
        if (riga.rfind("carico ", 0) == 0) {
            std::string parola;
            campi >> parola;
            if (!r.carico_) r.carico_ = CaricoPartenza{};
            if (!leggi_carico(campi, *r.carico_)) throw std::runtime_error("ordini: carico non valido alla riga " + std::to_string(numero));
            continue;
        }
        Ordine o;
        std::string nome;
        if (!(campi >> o.tempo >> nome >> o.a >> o.b >> o.n)) throw std::runtime_error("ordini: riga " + std::to_string(numero) + " non valida");
        campi >> o.testo;   // facoltativo
        const auto t = ordine_da_nome(nome);
        if (!t) throw std::runtime_error("ordini: tipo sconosciuto alla riga " + std::to_string(numero) + ": " + nome);
        o.tipo = *t;
        r.ordina(o);
    }
    return r;
}

}  // namespace m688
