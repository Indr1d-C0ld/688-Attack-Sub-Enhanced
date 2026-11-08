#include "nucleo/confronto_classificazioni.hpp"

#include <algorithm>
#include <cctype>
#include <map>

namespace m688 {

const char* nome_esito(EsitoClassificazione e) {
    switch (e) {
        case EsitoClassificazione::assente: return "assente";
        case EsitoClassificazione::giusta: return "giusta";
        case EsitoClassificazione::famiglia: return "della famiglia";
        case EsitoClassificazione::solo_categoria: return "solo la categoria";
        case EsitoClassificazione::sbagliata: return "sbagliata";
    }
    return "";
}

std::string famiglia_classe(const std::string& classe) {
    std::size_t fine = classe.find("_flight");
    for (std::size_t i = 0; i < std::min(fine, classe.size()); ++i)
        if (std::isdigit(static_cast<unsigned char>(classe[i]))) { fine = i; break; }
    std::string f = classe.substr(0, std::min(fine, classe.size()));
    while (!f.empty() && f.back() == '_') f.pop_back();
    return f;
}

namespace {

EsitoClassificazione esito_di(const VoceClassificazione& v) {
    if (v.livello < 2) return v.giusta ? EsitoClassificazione::solo_categoria : EsitoClassificazione::sbagliata;
    if (v.giusta) return EsitoClassificazione::giusta;
    const std::string a = famiglia_classe(v.classe_creduta), b = famiglia_classe(v.classe_vera);
    return !a.empty() && a == b ? EsitoClassificazione::famiglia : EsitoClassificazione::sbagliata;
}

double punti(EsitoClassificazione e) {
    switch (e) {
        case EsitoClassificazione::giusta: return 1.0;
        case EsitoClassificazione::famiglia:
        case EsitoClassificazione::solo_categoria: return 0.5;
        default: return 0.0;
    }
}

}  // namespace

ConfrontoClassificazioni confronta_classificazioni(const std::vector<VoceClassificazione>& voci) {
    ConfrontoClassificazioni out;
    std::vector<const VoceClassificazione*> ordinate;
    for (const auto& v : voci) ordinate.push_back(&v);
    std::stable_sort(ordinate.begin(), ordinate.end(), [](const auto* a, const auto* b) { return a->t < b->t; });
    std::map<int, ConfrontoContatto> per;
    for (const VoceClassificazione* v : ordinate) {
        if (v->chi == VoceClassificazione::Chi::automatica) {
            ++out.automatiche;
            continue;
        }
        ConfrontoContatto& c = per[v->contatto];
        c.contatto = v->contatto;
        if (!v->classe_vera.empty()) c.classe_vera = v->classe_vera;
        ConfrontoContatto::Parte& p = v->chi == VoceClassificazione::Chi::visiva ? c.visiva : c.acustica;
        p.esito = esito_di(*v);   // l'ultima vale
        p.classe = v->classe_creduta;
        p.t = v->t;
        if (p.t_prima_giusta < 0.0 && punti(p.esito) > 0.0) p.t_prima_giusta = v->t;
    }
    for (auto& [id, c] : per) {
        const double a = c.acustica.t_prima_giusta, b = c.visiva.t_prima_giusta;
        if (a >= 0.0 && (b < 0.0 || a < b)) c.prima = ConfrontoContatto::Prima::acustica;
        else if (b >= 0.0 && (a < 0.0 || b < a)) c.prima = ConfrontoContatto::Prima::visiva;
        else if (a >= 0.0) c.prima = ConfrontoContatto::Prima::insieme;
        if (c.acustica.esito != EsitoClassificazione::assente) ++out.acustiche, out.acustiche_giuste += punti(c.acustica.esito);
        if (c.visiva.esito != EsitoClassificazione::assente) ++out.visive, out.visive_giuste += punti(c.visiva.esito);
        out.prima_acustica += c.prima == ConfrontoContatto::Prima::acustica ? 1 : 0;
        out.prima_visiva += c.prima == ConfrontoContatto::Prima::visiva ? 1 : 0;
        out.contatti.push_back(c);
    }
    return out;
}

}  // namespace m688
