#include "nucleo/enciclopedia.hpp"

#include <algorithm>
#include <stdexcept>

#include <toml.hpp>

namespace m688 {

std::string ImmagineVoce::testo(const std::string& k, const std::string& predefinito) const {
    const auto it = testi.find(k);
    return it != testi.end() ? it->second : predefinito;
}

double ImmagineVoce::numero(const std::string& k, double predefinito) const {
    const auto it = numeri.find(k);
    return it != numeri.end() ? it->second : predefinito;
}

bool ImmagineVoce::vero(const std::string& k, bool predefinito) const {
    const auto it = flag.find(k);
    return it != flag.end() ? it->second : predefinito;
}

const std::string& VoceEnciclopedia::nome_mostrato(bool inglese) const {
    if (!nome_lat.empty()) return nome_lat;
    return inglese && !nome_en.empty() ? nome_en : nome;
}

const VoceEnciclopedia* Enciclopedia::voce(const std::string& id) const {
    const auto it = voci.find(id);
    return it != voci.end() ? &it->second : nullptr;
}

std::vector<const VoceEnciclopedia*> Enciclopedia::del_gruppo(const std::string& gruppo) const {
    std::vector<const VoceEnciclopedia*> v;
    for (const auto& [id, x] : voci)
        if (x.gruppo == gruppo) v.push_back(&x);
    std::sort(v.begin(), v.end(), [](const auto* a, const auto* b) { return a->nome_mostrato() < b->nome_mostrato(); });
    return v;
}

namespace {

std::string s(const toml::table& t, const char* k) { return t[k].value<std::string>().value_or(""); }

std::vector<std::string> elenco(const toml::table& t, const char* k) {
    std::vector<std::string> v;
    if (const auto* a = t[k].as_array())
        for (const auto& n : *a)
            if (const auto x = n.value<std::string>()) v.push_back(*x);
    return v;
}

}  // namespace

Enciclopedia carica_enciclopedia(const std::string& percorso) {
    toml::table doc;
    try {
        doc = toml::parse_file(percorso);
    } catch (const toml::parse_error& e) {
        throw std::runtime_error("enciclopedia: " + percorso + ": " + std::string(e.description()));
    }
    Enciclopedia en;
    const auto* voci = doc["voci"].as_table();
    if (!voci) throw std::runtime_error("enciclopedia: " + percorso + " senza [voci]");
    for (const auto& [chiave, nodo] : *voci) {
        const auto* t = nodo.as_table();
        if (!t) continue;
        VoceEnciclopedia v;
        v.id = std::string(chiave.str());
        v.nome = s(*t, "nome");
        v.nome_en = s(*t, "nome_en");
        v.nome_lat = s(*t, "nome_lat");
        v.categoria = s(*t, "categoria");
        v.gruppo = s(*t, "gruppo");
        v.gruppo_en = s(*t, "gruppo_en");
        v.nazione = s(*t, "nazione");
        v.testo_it = s(*t, "testo_it");
        v.testo_en = s(*t, "testo_en");
        v.uso_it = s(*t, "uso_it");
        v.uso_en = s(*t, "uso_en");
        v.fonti = elenco(*t, "fonti");
        v.armi_tipiche_altre = elenco(*t, "armi_tipiche_altre");
        if (const auto* a = (*t)["scheda"].as_array())
            for (const auto& n : *a)
                if (const auto* r = n.as_table()) {
                    RigaScheda x;
                    x.campo = s(*r, "campo");
                    x.nome_it = s(*r, "nome_it");
                    x.nome_en = s(*r, "nome_en");
                    x.testo_it = s(*r, "testo_it");
                    x.testo_en = s(*r, "testo_en");
                    x.unita = s(*r, "unita");
                    x.fonte = s(*r, "fonte");
                    x.conf = s(*r, "conf");
                    x.gioco_testo_it = s(*r, "gioco_testo_it");
                    x.gioco_testo_en = s(*r, "gioco_testo_en");
                    x.testo_lat = s(*r, "testo_lat");
                    if (const auto d = (*r)["valore"].value<double>()) x.valore = *d;
                    v.scheda.push_back(std::move(x));
                }
        if (const auto* a = (*t)["riferimenti"].as_array())
            for (const auto& n : *a)
                if (const auto* r = n.as_table()) v.riferimenti.push_back({s(*r, "relazione"), s(*r, "voce")});
        if (const auto* a = (*t)["immagini"].as_array())
            for (const auto& n : *a)
                if (const auto* r = n.as_table()) {
                    ImmagineVoce im;
                    for (const auto& [k, val] : *r) {
                        const std::string nome(k.str());
                        if (nome == "tipo") im.tipo = val.value<std::string>().value_or("");
                        else if (nome == "didascalia_it") im.didascalia_it = val.value<std::string>().value_or("");
                        else if (nome == "didascalia_en") im.didascalia_en = val.value<std::string>().value_or("");
                        else if (val.is_boolean()) im.flag[nome] = val.value<bool>().value_or(false);
                        else if (val.is_number()) im.numeri[nome] = val.value<double>().value_or(0.0);
                        else if (val.is_string()) im.testi[nome] = val.value<std::string>().value_or("");
                    }
                    v.immagini.push_back(std::move(im));
                }
        if (const auto* a = (*t)["crediti"].as_array())
            for (const auto& n : *a)
                if (const auto* r = n.as_table()) v.crediti.push_back({s(*r, "titolo"), s(*r, "autore"), s(*r, "licenza"), s(*r, "url")});
        if (!v.gruppo.empty() && std::find(en.gruppi.begin(), en.gruppi.end(), v.gruppo) == en.gruppi.end()) en.gruppi.push_back(v.gruppo);
        en.voci.emplace(v.id, std::move(v));
    }
    return en;
}

}  // namespace m688
