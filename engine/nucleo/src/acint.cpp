#include "nucleo/acint.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>

namespace m688 {

namespace {

// soglie della scala NATO sulla confidenza del confronto per rapporti (STIMA DI GIOCO, dossier 09 §2.9)
constexpr double CERTSUB_CONFIDENZA = 0.8, PROBSUB_CONFIDENZA = 0.55;
constexpr int CERTSUB_RIGHE = 5, PROBSUB_RIGHE = 3;
constexpr int MINIMO_RIGHE = 2;   // con una riga sola concorde non si classifica (resta POSSUB)

bool tipo_sottomarino(const std::string& t) { return t == "ssn" || t == "ssgn" || t == "ssbn" || t == "ssk"; }

std::string ora(double t) {
    const long s = std::lround(std::max(0.0, t));
    char b[32];
    std::snprintf(b, sizeof b, "%02ld:%02ld", s / 3600, (s / 60) % 60);
    return b;
}
std::string numero(double x, int decimali, bool virgola) {
    char b[32];
    std::snprintf(b, sizeof b, "%.*f", decimali, x);
    std::string s = b;
    if (virgola) std::replace(s.begin(), s.end(), '.', ',');
    return s;
}

}  // namespace

const char* nome_confidenza(Confidenza c) {
    switch (c) {
        case Confidenza::nonsub: return "NONSUB";
        case Confidenza::possub: return "POSSUB";
        case Confidenza::probsub: return "PROBSUB";
        case Confidenza::certsub: return "CERTSUB";
    }
    return "?";
}

// --- registratore ---

void Registratore::registra(int contatto, const CampioneContatto& c) {
    auto& v = storie_[contatto];
    if (v.empty()) ordine_.push_back(contatto);
    v.push_back(c);
    if (v.size() <= MASSIMO) return;
    // la prima meta' della storia a un campione su due
    std::vector<CampioneContatto> nuova;
    nuova.reserve(v.size());
    const std::size_t meta = v.size() / 2;
    for (std::size_t i = 0; i < v.size(); ++i)
        if (i >= meta || i % 2 == 0) nuova.push_back(v[i]);
    v.swap(nuova);
}

const std::vector<CampioneContatto>* Registratore::storia(int contatto) const {
    const auto it = storie_.find(contatto);
    return it == storie_.end() ? nullptr : &it->second;
}

std::vector<int> Registratore::contatti() const { return ordine_; }

void Registratore::dimentica(int contatto) {
    storie_.erase(contatto);
    ordine_.erase(std::remove(ordine_.begin(), ordine_.end(), contatto), ordine_.end());
}

// --- righe stabili ---

std::vector<RigaStabile> righe_stabili(const std::vector<CampioneContatto>& storia, double tolleranza_rel, double presenza_min) {
    struct Punto { double hz, snr; std::size_t campione; };
    std::vector<Punto> punti;
    for (std::size_t i = 0; i < storia.size(); ++i)
        for (const auto& r : storia[i].righe) punti.push_back({r.hz, r.snr_db, i});
    std::sort(punti.begin(), punti.end(), [](const Punto& a, const Punto& b) { return a.hz < b.hz; });
    std::vector<RigaStabile> out;
    std::size_t i = 0;
    while (i < punti.size()) {
        // un gruppo: finche' la riga successiva sta entro la tolleranza dalla media del gruppo
        double somma = punti[i].hz;
        std::size_t j = i + 1;
        while (j < punti.size() && punti[j].hz - somma / static_cast<double>(j - i) <= tolleranza_rel * punti[j].hz) somma += punti[j++].hz;
        std::vector<std::size_t> campioni;
        double snr = 0.0, lo = punti[i].hz, hi = punti[i].hz;
        for (std::size_t k = i; k < j; ++k) {
            campioni.push_back(punti[k].campione);
            snr += punti[k].snr;
            lo = std::min(lo, punti[k].hz);
            hi = std::max(hi, punti[k].hz);
        }
        std::sort(campioni.begin(), campioni.end());
        campioni.erase(std::unique(campioni.begin(), campioni.end()), campioni.end());
        RigaStabile r;
        r.hz = somma / static_cast<double>(j - i);
        r.snr_db = snr / static_cast<double>(j - i);
        r.campioni = static_cast<int>(campioni.size());
        r.presenza = storia.empty() ? 0.0 : static_cast<double>(campioni.size()) / static_cast<double>(storia.size());
        r.deriva_hz = hi - lo;
        if (r.presenza >= presenza_min) out.push_back(r);
        i = j;
    }
    return out;
}

// --- classificazione e rapporto ---

namespace {
// le righe osservate distinte fra le concordi: una riga che concorda con due righe del catalogo (120 Hz con 120 e 119,8)
// conta una volta. Contando le coppie, la sola riga a 120 Hz di un 688 non mascherata bastava per «kidd» su ogni
// contatto di M1
int osservate_concordi(const ConfrontoRapporti& m) {
    std::vector<double> hz;
    for (const auto& [o, l] : m.concordi)
        if (std::find(hz.begin(), hz.end(), o.hz) == hz.end()) hz.push_back(o.hz);
    return static_cast<int>(hz.size());
}
}  // namespace

Classificazione classifica_righe(const std::vector<RigaOsservata>& righe, std::optional<int> pale, const CatalogoAcustico& c) {
    Classificazione k;   // senza altro: una prima rilevazione subacquea e' POSSUB (§2.9)
    if (righe.empty()) return k;
    k.ipotesi = confronta_rapporti(righe, c, pale);
    if (k.ipotesi.size() > 3) k.ipotesi.resize(3);
    if (k.ipotesi.empty() || osservate_concordi(k.ipotesi.front()) < MINIMO_RIGHE) return k;
    const ConfrontoRapporti& m = k.ipotesi.front();
    k.classe = m.id;
    const auto voce = voce_libreria(c, m.id);
    k.sottomarino = voce && tipo_sottomarino(voce->tipo_unita);
    const int concordi = osservate_concordi(m);
    if (!k.sottomarino) k.confidenza = Confidenza::nonsub;
    else if (m.confidenza >= CERTSUB_CONFIDENZA && concordi >= CERTSUB_RIGHE) k.confidenza = Confidenza::certsub;
    else if (m.confidenza >= PROBSUB_CONFIDENZA && concordi >= PROBSUB_RIGHE) k.confidenza = Confidenza::probsub;
    return k;
}

VoceAcint classifica_contatto(int contatto, const std::vector<CampioneContatto>& storia, const CatalogoAcustico& c) {
    VoceAcint v;
    v.contatto = contatto;
    if (storia.empty()) return v;
    v.t_primo = storia.front().t;
    v.t_ultimo = storia.back().t;
    v.rilevamento_primo = storia.front().rilevamento;
    v.rilevamento_ultimo = storia.back().rilevamento;
    std::map<int, int> pale;
    for (const auto& s : storia) {
        v.livello_massimo_db = std::max(v.livello_massimo_db, s.livello_db);
        if (s.demon && s.demon->pale > 0) ++pale[s.demon->pale];
    }
    if (!pale.empty())
        v.pale = std::max_element(pale.begin(), pale.end(), [](const auto& a, const auto& b) { return a.second < b.second; })->first;
    v.righe = righe_stabili(storia);
    std::vector<RigaOsservata> osservate;
    for (const auto& r : v.righe) osservate.push_back({r.hz, r.snr_db});
    const Classificazione k = classifica_righe(osservate, v.pale, c);
    v.ipotesi = k.ipotesi;
    v.classe = k.classe;
    v.sottomarino = k.sottomarino;
    v.confidenza = k.confidenza;
    if (!v.classe.empty()) {
        const ConfrontoRapporti& m = v.ipotesi.front();
        for (const auto& r : v.righe) {
            const bool spiegata = std::any_of(m.concordi.begin(), m.concordi.end(), [&](const auto& x) {
                return std::fabs(x.first.hz - r.hz) <= 0.004 * r.hz;
            });
            if (!spiegata) v.nuove.push_back(r);
        }
    } else {
        v.nuove = v.righe;   // nessuna classe: tutte le righe sono materiale nuovo
    }
    // il testo
    const auto voce = v.classe.empty() ? std::nullopt : voce_libreria(c, v.classe);
    const std::string nome = voce ? voce->nome : std::string("sconosciuto");
    const std::string nome_en = voce ? voce->nome : std::string("unknown");
    const long minuti = std::lround((v.t_ultimo - v.t_primo) / 60.0);
    std::string it = "S" + std::to_string(contatto) + ": dalle " + ora(v.t_primo) + " alle " + ora(v.t_ultimo) + " (" +
                     std::to_string(minuti) + " min), rilevamento " + numero(normalizza_360(v.rilevamento_primo), 0, true) +
                     " -> " + numero(normalizza_360(v.rilevamento_ultimo), 0, true) + "; " + nome + ", " +
                     nome_confidenza(v.confidenza);
    std::string en = "S" + std::to_string(contatto) + ": " + ora(v.t_primo) + " to " + ora(v.t_ultimo) + " (" +
                     std::to_string(minuti) + " min), bearing " + numero(normalizza_360(v.rilevamento_primo), 0, false) +
                     " -> " + numero(normalizza_360(v.rilevamento_ultimo), 0, false) + "; " + nome_en + ", " +
                     nome_confidenza(v.confidenza);
    if (!v.ipotesi.empty() && !v.classe.empty()) {
        const auto& m = v.ipotesi.front();
        it += " (confidenza " + numero(m.confidenza, 2, true) + ", " + std::to_string(m.concordi.size()) + " righe concordi)";
        en += " (confidence " + numero(m.confidenza, 2, false) + ", " + std::to_string(m.concordi.size()) + " matching lines)";
    }
    if (v.pale) {
        it += "; pale " + std::to_string(*v.pale);
        en += "; " + std::to_string(*v.pale) + " blades";
    }
    v.testo_it.push_back(it + ".");
    v.testo_en.push_back(en + ".");
    if (!v.nuove.empty()) {
        std::string ni = "  Righe non in libreria:", ne = "  Lines not in the library:";
        for (std::size_t i = 0; i < v.nuove.size() && i < 8; ++i) {
            ni += (i ? ", " : " ") + numero(v.nuove[i].hz, 1, true) + " Hz";
            ne += (i ? ", " : " ") + numero(v.nuove[i].hz, 1, false) + " Hz";
        }
        v.testo_it.push_back(ni + ".");
        v.testo_en.push_back(ne + ".");
    }
    return v;
}

RapportoAcint rapporto_acint(const Registratore& r, const CatalogoAcustico& c, double durata_min_s) {
    RapportoAcint out;
    for (const int id : r.contatti()) {
        const auto* s = r.storia(id);
        if (!s || s->empty() || s->back().t - s->front().t < durata_min_s) continue;
        out.voci.push_back(classifica_contatto(id, *s, c));
    }
    int nuove = 0;
    for (const auto& v : out.voci) nuove += static_cast<int>(v.nuove.size());
    out.testo_it.push_back("RAPPORTO ACINT: " + std::to_string(out.voci.size()) + " contatti registrati, " +
                           std::to_string(nuove) + " righe non in libreria.");
    out.testo_en.push_back("ACINT REPORT: " + std::to_string(out.voci.size()) + " contacts recorded, " +
                           std::to_string(nuove) + " lines not in the library.");
    for (const auto& v : out.voci) {
        out.testo_it.insert(out.testo_it.end(), v.testo_it.begin(), v.testo_it.end());
        out.testo_en.insert(out.testo_en.end(), v.testo_en.begin(), v.testo_en.end());
    }
    return out;
}

}  // namespace m688
