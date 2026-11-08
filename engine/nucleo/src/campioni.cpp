#include "nucleo/campioni.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <stdexcept>

#include <toml.hpp>

namespace m688 {

namespace {

std::uint32_t le32(const unsigned char* p) { return p[0] | (p[1] << 8) | (p[2] << 16) | (static_cast<std::uint32_t>(p[3]) << 24); }
std::uint16_t le16(const unsigned char* p) { return static_cast<std::uint16_t>(p[0] | (p[1] << 8)); }

}  // namespace

bool leggi_wav(const std::string& percorso, std::vector<float>& campioni, int& campionamento_hz) {
    std::ifstream f(percorso, std::ios::binary);
    if (!f) return false;
    std::vector<unsigned char> d((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    if (d.size() < 12 || std::memcmp(d.data(), "RIFF", 4) != 0 || std::memcmp(d.data() + 8, "WAVE", 4) != 0) return false;
    int canali = 0, bit = 0;
    std::size_t p = 12;
    while (p + 8 <= d.size()) {
        const std::uint32_t n = le32(d.data() + p + 4);
        const unsigned char* c = d.data() + p + 8;
        if (p + 8 + n > d.size()) return false;
        if (std::memcmp(d.data() + p, "fmt ", 4) == 0 && n >= 16) {
            if (le16(c) != 1) return false;   // solo PCM
            canali = le16(c + 2);
            campionamento_hz = static_cast<int>(le32(c + 4));
            bit = le16(c + 14);
        } else if (std::memcmp(d.data() + p, "data", 4) == 0) {
            if (bit != 16 || canali < 1) return false;
            const std::size_t fotogrammi = n / (2u * static_cast<std::size_t>(canali));
            campioni.assign(fotogrammi, 0.0f);
            for (std::size_t i = 0; i < fotogrammi; ++i) {
                double s = 0.0;
                for (int k = 0; k < canali; ++k)
                    s += static_cast<std::int16_t>(le16(c + 2 * (i * static_cast<std::size_t>(canali) + static_cast<std::size_t>(k))));
                campioni[i] = static_cast<float>(s / canali / 32768.0);
            }
            return true;
        }
        p += 8 + n + (n & 1u);
    }
    return false;
}

LibreriaCampioni carica_campioni(const std::string& percorso_toml, const std::string& radice_dati) {
    LibreriaCampioni out;
    if (!std::filesystem::exists(percorso_toml)) return out;
    const toml::table t = toml::parse_file(percorso_toml);
    const auto* tab = t["campioni"].as_table();
    if (!tab) return out;
    for (const auto& [chiave, nodo] : *tab) {
        const auto* x = nodo.as_table();
        if (!x) continue;
        Campione c;
        c.id = std::string(chiave.str());
        c.voce = (*x)["voce"].value_or(c.id);
        c.fattore = (*x)["fattore"].value_or(1.0);
        c.tessitura_rms = (*x)["tessitura_rms"].value_or(0.25);
        c.f_min_hz = (*x)["f_min_valida_hz"].value_or(0.0);
        c.f_max_hz = (*x)["f_max_valida_hz"].value_or(0.0);
        c.demon_hz = (*x)["demon_hz"].value_or(0.0);
        c.demon_db = (*x)["demon_db"].value_or(0.0);
        c.passo_s = (*x)["passo_inviluppo_s"].value_or(0.25);
        c.licenza = (*x)["licenza"].value_or(std::string{});
        c.stato = (*x)["stato"].value_or(std::string{});
        c.attribuzione = (*x)["attribuzione"].value_or(std::string{});
        if (const auto* a = (*x)["inviluppo_tempo"].as_array())
            for (const auto& banda : *a) {
                std::vector<float> v;
                if (const auto* b = banda.as_array())
                    for (const auto& e : *b) v.push_back(static_cast<float>(e.value_or(0.0)));
                c.inviluppo_tempo.push_back(std::move(v));
            }
        if (const auto* a = (*x)["bande_tempo_hz"].as_array())
            for (const auto& banda : *a)
                if (const auto* b = banda.as_array(); b && b->size() == 2)
                    c.bande_hz.emplace_back((*b)[0].value_or(0.0), (*b)[1].value_or(0.0));
        const std::string file = (*x)["file"].value_or(std::string{});
        if (!leggi_wav(radice_dati + "/" + file, c.campioni, c.campionamento_hz))
            throw std::runtime_error(percorso_toml + ": campione '" + c.id + "' illeggibile: " + file);
        prepara_inviluppo(c);
        out.per_voce[c.voce].push_back(out.campioni.size());
        out.campioni.push_back(std::move(c));
    }
    return out;
}

std::size_t indice_campione(std::uint64_t seme, std::size_t quanti) {
    if (quanti == 0) return 0;
    std::uint64_t z = seme ^ 0x6a09e667f3bcc909ull;
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ull;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebull;
    return static_cast<std::size_t>((z ^ (z >> 31)) % quanti);
}

const Campione* LibreriaCampioni::scegli(const std::string& voce, std::uint64_t seme) const {
    const auto it = per_voce.find(voce);
    if (it == per_voce.end() || it->second.empty()) return nullptr;
    return &campioni[it->second[indice_campione(seme, it->second.size())]];
}

double posizione_campione_s(const Campione& c, std::uint64_t seme, double tempo_s) {
    const double durata = c.durata_s();
    if (durata <= 0.0) return 0.0;
    const double scarto = static_cast<double>(seme % 1000003u) / c.campionamento_hz;
    const double p = std::fmod(scarto + tempo_s / c.fattore, durata);
    return p < 0.0 ? p + durata : p;
}

void prepara_inviluppo(Campione& c) {
    c.prefisso_lineare.assign(c.inviluppo_tempo.size(), {});
    c.media_lineare.assign(c.inviluppo_tempo.size(), 0.0);
    for (std::size_t b = 0; b < c.inviluppo_tempo.size(); ++b) {
        const auto& v = c.inviluppo_tempo[b];
        auto& p = c.prefisso_lineare[b];
        p.assign(v.size() + 1, 0.0);
        for (std::size_t i = 0; i < v.size(); ++i) p[i + 1] = p[i] + std::pow(10.0, v[i] / 10.0);
        if (!v.empty()) c.media_lineare[b] = p.back() / static_cast<double>(v.size());
    }
}

double modulazione_campione(const Campione& c, std::uint64_t seme, double tempo_s, double finestra_s, double f_hz) {
    if (c.inviluppo_tempo.empty()) return 1.0;
    std::size_t b = 0;   // la banda di f (Hz veri)
    for (std::size_t k = 0; k < c.bande_hz.size() && k < c.inviluppo_tempo.size(); ++k)
        if (f_hz >= c.bande_hz[k].first) b = k;
    while (b > 0 && c.inviluppo_tempo[b].empty()) --b;
    const std::vector<float>& v = c.inviluppo_tempo[b];
    if (v.empty()) return 1.0;
    if (c.prefisso_lineare.size() != c.inviluppo_tempo.size() || c.prefisso_lineare[b].size() != v.size() + 1)
        throw std::logic_error("modulazione_campione: inviluppo di '" + c.id + "' non preparato (prepara_inviluppo)");
    const std::vector<double>& p = c.prefisso_lineare[b];
    const auto n = static_cast<long long>(v.size());
    // la finestra: gli indici da fine - quanti + 1 a fine, ad anello (giri interi piu' il resto)
    const long long fine = static_cast<long long>(std::floor(posizione_campione_s(c, seme, tempo_s) / c.passo_s)) % n;
    const auto quanti = static_cast<long long>(std::max(1.0, std::floor(std::max(finestra_s, 0.0) / c.fattore / c.passo_s)));
    double somma = static_cast<double>(quanti / n) * p[static_cast<std::size_t>(n)];
    if (const long long resto = quanti % n; resto > 0) {
        const long long inizio = fine - resto + 1;
        somma += inizio >= 0 ? p[static_cast<std::size_t>(fine + 1)] - p[static_cast<std::size_t>(inizio)]
                             : p[static_cast<std::size_t>(fine + 1)] + p[static_cast<std::size_t>(n)] - p[static_cast<std::size_t>(n + inizio)];
    }
    return std::clamp(somma / static_cast<double>(quanti) / std::max(c.media_lineare[b], 1e-30), 0.001, 30.0);
}

void collega_campioni(CatalogoAcustico& cat, const LibreriaCampioni& l) {
    for (auto& [id, f] : cat.sorgenti) {
        f.campioni.clear();
        f.attivita_media.clear();
        const auto it = l.per_voce.find(id);
        if (it == l.per_voce.end()) continue;
        for (const std::size_t i : it->second) f.campioni.push_back(&l.campioni[i]);
        // l'attivita' media di lungo periodo (20 ore) di ogni componente: il livello medio non cambia
        for (const Componente& c : f.componenti) f.attivita_media.push_back(attivita(f, c, 12345u, 72000.0, 72000.0));
    }
}

std::string voce_campione(const Sorgente& s) {
    if (s.fonte) return s.fonte->id == "elicottero_ka25" ? "elicottero_ka27" : s.fonte->id;   // R8: il Ka-25 col Ka-27
    if (s.firma) {
        const std::string& c = s.firma->classe;
        if (c == "mercantile" || c == "cargo" || c == "petroliera") return "mercantile";
        return c;
    }
    return {};
}

}  // namespace m688
