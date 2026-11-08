#include "codice_partita.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iterator>
#include <random>

namespace m688 {

namespace {

constexpr const char* ALFABETO = "0123456789ABCDEFGHJKMNPQRSTVWXYZ";   // Crockford, come il codice delle rapide
constexpr int BIT_MASSIMI = 150;
constexpr int CARATTERI = 18, BIT_DATI = 78;                       // codice partita: 90 bit
constexpr int CARATTERI_SANDBOX = 30, BIT_DATI_SANDBOX = 138;      // codice sandbox: 150 bit

struct Bit {
    std::array<std::uint8_t, BIT_MASSIMI> b{};
    int n = 0;
    void scrivi(std::uint64_t v, int k) {
        for (int i = k - 1; i >= 0; --i) b[static_cast<std::size_t>(n++)] = static_cast<std::uint8_t>((v >> i) & 1u);
    }
    std::uint64_t leggi(int& pos, int k) const {
        std::uint64_t v = 0;
        for (int i = 0; i < k; ++i) v = (v << 1) | b[static_cast<std::size_t>(pos++)];
        return v;
    }
};

// CRC-12 (x^12 + x^11 + x^3 + x^2 + x + 1): vede ogni errore entro 12 bit di fila, quindi un carattere sbagliato
// (5 bit) e lo scambio di due caratteri vicini (10 bit)
int crc12(const Bit& d, int n) {
    int r = 0;
    for (int i = 0; i < n; ++i) {
        const int alto = ((r >> 11) & 1) ^ d.b[static_cast<std::size_t>(i)];
        r = (r << 1) & 0xFFF;
        if (alto) r ^= 0x80F;
    }
    return r;
}

// i 77 bit comuni ai due codici: versione, origine, impronta, seme e le opzioni del menu
void scrivi_base(Bit& d, const CodicePartita& c) {
    d.scrivi(VERSIONE_CODICE_PARTITA, 3);
    d.scrivi(static_cast<std::uint64_t>(c.origine), 2);
    d.scrivi(c.impronta, 32);
    d.scrivi(c.seme & 0x7FFFFFFFu, 31);   // versione 2: 31 bit di seme e il GPS
    d.scrivi(c.gps ? 1 : 0, 1);
    d.scrivi(c.anni_89 ? 1 : 0, 1);
    d.scrivi(c.carta_epoca ? 1 : 0, 1);
    d.scrivi(c.meteo_realistico ? 1 : 0, 1);
    d.scrivi(static_cast<std::uint64_t>(c.asw & 3), 2);
    d.scrivi(static_cast<std::uint64_t>(c.sonarista & 3), 2);
    d.scrivi(c.navigazione ? 1 : 0, 1);
}

std::optional<CodicePartita> leggi_base(const Bit& d, int& pos) {
    const int versione = static_cast<int>(d.leggi(pos, 3));
    if (versione != 1 && versione != VERSIONE_CODICE_PARTITA) return std::nullopt;
    CodicePartita c;
    c.origine = static_cast<OrigineCodice>(d.leggi(pos, 2));
    c.impronta = static_cast<std::uint32_t>(d.leggi(pos, 32));
    if (versione == 1) {   // la versione 1: il seme a 32 bit, senza GPS
        c.seme = static_cast<std::uint32_t>(d.leggi(pos, 32));
    } else {
        c.seme = static_cast<std::uint32_t>(d.leggi(pos, 31));
        c.gps = d.leggi(pos, 1) != 0;
    }
    c.anni_89 = d.leggi(pos, 1) != 0;
    c.carta_epoca = d.leggi(pos, 1) != 0;
    c.meteo_realistico = d.leggi(pos, 1) != 0;
    c.asw = static_cast<int>(d.leggi(pos, 2));
    c.sonarista = static_cast<int>(d.leggi(pos, 2));
    c.navigazione = d.leggi(pos, 1) != 0;
    return c;
}

// controllo, poi i caratteri a gruppi di sei
std::string caratteri(Bit& d, int bit_dati, int n_caratteri) {
    d.scrivi(static_cast<std::uint64_t>(crc12(d, bit_dati)), 12);
    std::string s;
    int pos = 0;
    for (int i = 0; i < n_caratteri; ++i) {
        if (i && i % 6 == 0) s += '-';
        s += ALFABETO[d.leggi(pos, 5)];
    }
    return s;
}

// i bit di un codice letto, col controllo giusto; nullopt se la lunghezza o il controllo non tornano
std::optional<Bit> bit_di(std::string_view codice, int n_caratteri, int bit_dati) {
    std::string s;
    for (char c : codice) {
        if (c == '-' || c == ' ') continue;
        if (c >= 'a' && c <= 'z') c = static_cast<char>(c - 'a' + 'A');
        if (c == 'O') c = '0';
        if (c == 'I' || c == 'L') c = '1';
        s += c;
    }
    if (static_cast<int>(s.size()) != n_caratteri) return std::nullopt;
    Bit d;
    for (const char c : s) {
        const char* p = std::strchr(ALFABETO, c);
        if (!p || c == '\0') return std::nullopt;
        d.scrivi(static_cast<std::uint64_t>(p - ALFABETO), 5);
    }
    int pos = bit_dati;
    if (static_cast<int>(d.leggi(pos, 12)) != crc12(d, bit_dati)) return std::nullopt;
    return d;
}

}  // namespace

const char* nome_origine(OrigineCodice o) {
    switch (o) {
        case OrigineCodice::originale: return "originale";
        case OrigineCodice::libreria: return "libreria";
        case OrigineCodice::editor: return "editor";
        case OrigineCodice::importata: return "importata";
    }
    return "originale";
}

std::string scrivi_codice_partita(const CodicePartita& c) {
    Bit d;
    scrivi_base(d, c);
    d.scrivi(c.senza_traffico ? 1 : 0, 1);
    return caratteri(d, BIT_DATI, CARATTERI);
}

std::optional<CodicePartita> leggi_codice_partita(std::string_view codice) {
    const auto d = bit_di(codice, CARATTERI, BIT_DATI);
    if (!d) return std::nullopt;
    int pos = 0;
    auto c = leggi_base(*d, pos);
    if (!c) return std::nullopt;
    c->senza_traffico = d->leggi(pos, 1) != 0;
    return c;
}

std::string scrivi_codice_sandbox(const CodiceSandbox& k) {
    Bit d;
    scrivi_base(d, k.partita);
    const RegolazioniSandbox& r = k.regolazioni;
    for (const SceltaParteSandbox* p : {&r.nato, &r.patto, &r.neutrali}) {
        d.scrivi(static_cast<std::uint64_t>(p->situazione) & 3, 2);
        d.scrivi(static_cast<std::uint64_t>(p->roe) & 3, 2);
        d.scrivi(static_cast<std::uint64_t>(p->livello) & 3, 2);
        d.scrivi(static_cast<std::uint64_t>(std::clamp(p->allerta + 1, 0, 4)), 3);
        d.scrivi(p->carico_per_classe ? 1 : 0, 1);
    }
    d.scrivi(static_cast<std::uint64_t>(std::clamp(r.durata_min / 15, 1, 96)), 7);
    for (const bool b : {r.ferma_all_esito, r.difesa, r.armi, r.reazioni, r.forze, r.esm_ia, r.sonaristi_ia}) d.scrivi(b ? 1 : 0, 1);
    d.scrivi(r.ferma_eventi ? 1 : 0, 1);   // dalla riserva: i codici di prima lo leggono spento
    d.scrivi(r.traffico ? 0 : 1, 1);       // dalla riserva: i codici di prima lo leggono acceso
    d.scrivi(0, BIT_DATI_SANDBOX - d.n);   // riserva
    return caratteri(d, BIT_DATI_SANDBOX, CARATTERI_SANDBOX);
}

std::optional<CodiceSandbox> leggi_codice_sandbox(std::string_view codice) {
    const auto d = bit_di(codice, CARATTERI_SANDBOX, BIT_DATI_SANDBOX);
    if (!d) return std::nullopt;
    int pos = 0;
    const auto base = leggi_base(*d, pos);
    if (!base) return std::nullopt;
    CodiceSandbox k;
    k.partita = *base;
    RegolazioniSandbox& r = k.regolazioni;
    for (SceltaParteSandbox* p : {&r.nato, &r.patto, &r.neutrali}) {
        const auto sit = d->leggi(pos, 2), roe = d->leggi(pos, 2), liv = d->leggi(pos, 2), all = d->leggi(pos, 3);
        if (roe > 2 || all > 4) return std::nullopt;
        p->situazione = static_cast<SituazioneScelta>(sit);
        p->roe = static_cast<RoeScelta>(roe);
        p->livello = static_cast<LivelloScelta>(liv);
        p->allerta = static_cast<int>(all) - 1;
        p->carico_per_classe = d->leggi(pos, 1) != 0;
    }
    const int quarti = static_cast<int>(d->leggi(pos, 7));
    if (quarti < 1 || quarti > 96) return std::nullopt;
    r.durata_min = quarti * 15;
    for (bool* b : {&r.ferma_all_esito, &r.difesa, &r.armi, &r.reazioni, &r.forze, &r.esm_ia, &r.sonaristi_ia}) *b = d->leggi(pos, 1) != 0;
    r.ferma_eventi = d->leggi(pos, 1) != 0;
    r.traffico = d->leggi(pos, 1) == 0;
    k.partita.senza_traffico = !r.traffico;
    if (d->leggi(pos, BIT_DATI_SANDBOX - pos) != 0) return std::nullopt;   // riserva
    // le opzioni di base valgono anche per il vetrino
    r.meteo_realistico = base->meteo_realistico;
    r.navigazione_realistica = base->navigazione;
    r.anni_89 = base->anni_89;
    r.seme = base->seme;
    return k;
}

std::uint32_t impronta_missione(const std::string& id, const std::filesystem::path& file) {
    std::ifstream f(file, std::ios::binary);
    if (!f) return 0;
    const std::string testo{std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};
    std::uint32_t h = 2166136261u;
    const auto mescola = [&h](unsigned char c) { h = (h ^ c) * 16777619u; };
    for (const char c : id) mescola(static_cast<unsigned char>(c));
    mescola(0);
    for (const char c : testo)
        if (c != '\r') mescola(static_cast<unsigned char>(c));   // lo stesso file con i ritorni di Windows
    return h ? h : 1u;
}

std::optional<MissioneDelCodice> trova_missione(std::uint32_t impronta, const std::vector<std::filesystem::path>& cartelle) {
    for (const auto& cartella : cartelle) {
        std::error_code ec;
        for (const auto& e : std::filesystem::directory_iterator(cartella, ec)) {
            if (e.path().extension() != ".lua") continue;
            const std::string id = e.path().stem().string();
            if (impronta_missione(id, e.path()) == impronta) return MissioneDelCodice{id, e.path()};
        }
    }
    return std::nullopt;
}

std::uint32_t seme_nuovo() {
    std::random_device rd;
    std::uint64_t x = (static_cast<std::uint64_t>(rd()) << 32) ^ rd() ^
                      static_cast<std::uint64_t>(std::chrono::steady_clock::now().time_since_epoch().count());
    x ^= x >> 33;
    x *= 0xff51afd7ed558ccdull;
    x ^= x >> 33;
    const auto s = static_cast<std::uint32_t>(x) & 0x7FFFFFFFu;   // 31 bit: quanti ne tiene il codice partita
    return s ? s : 1u;
}

}  // namespace m688
