#include "nucleo/suoni_classici.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <stdexcept>

#include <toml.hpp>

namespace m688 {

const CampioneClassico* DatiSuoniClassici::campione(const std::string& nome) const {
    const auto it = campioni.find(nome);
    return it == campioni.end() ? nullptr : &it->second;
}

DatiSuoniClassici carica_suoni_classici(const std::string& percorso_toml, const std::string& radice_dati) {
    DatiSuoniClassici out;
    if (!std::filesystem::exists(percorso_toml)) return out;
    const toml::table t = toml::parse_file(percorso_toml);
    const auto* tab = t["campioni"].as_table();
    if (!tab) return out;
    for (const auto& [chiave, nodo] : *tab) {
        const auto* x = nodo.as_table();
        if (!x) continue;
        CampioneClassico c;
        c.nome = std::string(chiave.str());
        c.origine = (*x)["origine"].value_or(std::string{});
        const std::string file = (*x)["file"].value_or(std::string{});
        std::ifstream f(radice_dati + "/" + file, std::ios::binary);
        if (!f) throw std::runtime_error(percorso_toml + ": campione '" + c.nome + "' illeggibile: " + file);
        const std::vector<char> b((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
        c.nibble.reserve(2 * b.size());
        for (const char v : b) {   // due campioni per byte, prima il nibble alto
            c.nibble.push_back(static_cast<std::uint8_t>(static_cast<unsigned char>(v) >> 4));
            c.nibble.push_back(static_cast<std::uint8_t>(static_cast<unsigned char>(v) & 15));
        }
        out.campioni[c.nome] = std::move(c);
    }
    return out;
}

void SuoniClassici::campione(const char* nome, int hz, int ripetizioni, const char* causa) {
    if (modo_ != ModoSuoniClassici::tutti) return;   // 407E esce con DS:03E2 != 0
    SuonoClassico s;
    s.campione = nome; s.frequenza_hz = hz; s.ripetizioni = ripetizioni; s.causa = causa;
    uscita_.push_back(std::move(s));
}

void SuoniClassici::bip(int divisore, int giri, const char* causa) {
    if (modo_ == ModoSuoniClassici::muti) return;    // BD3F esce solo con DS:03E2 = 2
    SuonoClassico s;
    s.tipo = SuonoClassico::Tipo::bip;
    s.frequenza_hz = static_cast<int>(std::lround(static_cast<double>(PIT_HZ) / divisore));
    s.durata_s = giri * SECONDI_PER_GIRO;
    s.volume = VOLUME_BIP;
    s.causa = causa;
    uscita_.push_back(std::move(s));
}

// Il gestore dei messaggi (1000:8AB0): i casi con un suono, poi il testo con il bip di 0ED8
void SuoniClassici::messaggio(int codice) {
    soppresso_ = false;
    switch (codice) {
        case 8:   // «active sonar»: il PING di un'altra unita' (8DCC)
            if (ultimo_ == 8) {   // ripetuto: solo un bip grave, niente testo, DS:03C8 resta 8
                bip(5000, 1500, "ping_ripetuto");
                soppresso_ = true;
                return;
            }
            campione("SONAR1", 6200, 1, "ping_altrui");
            bip(3000, 2000, "testo");
            ultimo_ = 8;
            return;
        case 7:
        case 50:   // un'arma a segno (8C7F): con la vista del periscopio il suono arriva con l'animazione
            bip(3000, 2000, "testo");
            if (vista_) { ultimo_ = codice; return; }
            campione("EXP1", 5000, 1, "colpo");
            ultimo_ = 0;
            return;
        case 19:
        case 20:
        case 26:
        case 30:   // esplosione, carica di profondita', colpiti, fondo raschiato (8B39, 8C36)
            bip(3000, 2000, "testo");
            campione("EXP1", 4000, 1, "danno");
            ultimo_ = 0;
            return;
        default:
            bip(3000, 2000, "testo");
            ultimo_ = codice;
            return;
    }
}

void SuoniClassici::testo_in_barra() { bip(3000, 2000, "testo"); }

void SuoniClassici::ping_proprio() {   // 3D26 «sending active sonar ping captain», poi 3D4E
    bip(3000, 2000, "testo");
    campione("SONAR1", 5500, 1, "ping_proprio");
}

void SuoniClassici::lancio_proprio() { campione("TORPRAMP", 6500, 1, "lancio"); }   // 7A73

void SuoniClassici::analizza(const ContattoAnalizzato& c) {   // 3F60 «analyzing contact», poi 3F65-4049
    bip(3000, 2000, "testo");
    if (c.nave) campione("PROP2", 2500 + 50 * c.tipo, 4, "analizza");
    else if (c.arma) campione("PROPV2", 5500 + 10 * c.tipo, 5, "analizza");
    else if (c.tipo == 0x19) {
        const int hz = 2500 + static_cast<int>(caso_.prossimo() % 4000);   // sempre estratto, come rand() in 3FDE
        campione("WHALE2", hz, 2, "analizza");
    } else campione("PROPV2", 2000 + 30 * c.tipo, 4, "analizza");
}

void SuoniClassici::tasto_modo() {   // DEAB, DECC: il bip c'e' sia entrando sia uscendo dal muto
    const auto m = modo_;
    modo_ = ModoSuoniClassici::tutti;
    bip(3000, 3000, "tasto");
    modo_ = m;
}

int SuoniClassici::stato_colpo(int unita) const {
    const auto it = stati_.find(unita);
    return it == stati_.end() ? 0 : it->second;
}

void SuoniClassici::fotogramma_periscopio(const std::vector<int>& in_vista) {
    if (!vista_) return;
    bool scoppio = false;   // DS:03D8
    for (const int u : in_vista) {
        const auto it = stati_.find(u);
        if (it == stati_.end() || it->second == 0) continue;
        int& s = it->second;
        // 322A: il fotogramma dell'esplosione col valore di prima dell'incremento
        int k = 0;
        if (s > 0 && s < 5) k = s;
        if (s > 200 && s < 0xCD) k = s - 200;
        if (s > 0xD2 && s < 0xD7) k = s - 0xD2;
        if (k == 4) scoppio = true;
        // 97BD: +5E avanza di uno; FFh resta, C8h -> A0h, CFh -> A1h
        int v = (s + 1) & 0xFF;
        if (v == 0) v = 0xFF;
        else if (v == 0xC8) v = 0xA0;
        else if (v == 0xCF) v = 0xA1;
        s = v;
    }
    if (scoppio) campione("EXP1", 5000, 1, "colpo_periscopio");   // 2A95-2AA8, dopo il ridisegno
}

std::vector<SuonoClassico> SuoniClassici::prendi() {
    std::vector<SuonoClassico> v;
    v.swap(uscita_);
    return v;
}

std::vector<float> SuoniClassici::pcm(const CampioneClassico& c, int ripetizioni, float volume) {
    std::vector<float> v;
    v.reserve(c.nibble.size() * static_cast<std::size_t>(std::max(ripetizioni, 0)));
    for (int r = 0; r < ripetizioni; ++r)
        for (const std::uint8_t n : c.nibble) v.push_back(volume * (static_cast<float>(n) - 7.5f) / 7.5f);
    return v;
}

std::vector<float> SuoniClassici::pcm_bip(const SuonoClassico& s, int campionamento_hz) {
    std::vector<float> v;
    if (s.frequenza_hz <= 0 || campionamento_hz <= 0) return v;
    const auto n = static_cast<std::size_t>(std::lround(s.durata_s * campionamento_hz));
    v.reserve(n);
    for (std::size_t i = 0; i < n; ++i) {
        const double fase = std::fmod(static_cast<double>(i) * s.frequenza_hz / campionamento_hz, 1.0);
        v.push_back(fase < 0.5 ? s.volume : -s.volume);
    }
    return v;
}

}  // namespace m688
