#include "nucleo/musica_fm.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iterator>
#include <stdexcept>

#include <toml.hpp>

#include "ymfm_opl.h"

namespace m688 {

namespace {

constexpr std::uint32_t CLOCK_OPL3 = 14318180;   // il quarzo della SB16: 14,318 MHz / 288 = 49.716 Hz
constexpr int VOCI = 18;
constexpr int OFFSET_MOD[9] = {0, 1, 2, 8, 9, 10, 16, 17, 18};
constexpr double CODA_S = 3.0;                   // a brano finito senza loop: il rilascio delle ultime note
constexpr double GUADAGNO = 1.4;                 // STIMA: il livello d'uscita, con 6-8 voci a velocita' piena sotto 1

class Interfaccia : public ymfm::ymfm_interface {};

OperatoreFm operatore(const toml::table* t) {
    OperatoreFm o;
    if (!t) return o;
    const auto& x = *t;
    o.mult = x["mult"].value_or(o.mult);
    o.tl = x["tl"].value_or(o.tl);
    o.ar = x["ar"].value_or(o.ar);
    o.dr = x["dr"].value_or(o.dr);
    o.sl = x["sl"].value_or(o.sl);
    o.rr = x["rr"].value_or(o.rr);
    o.onda = x["onda"].value_or(o.onda);
    o.ksl = x["ksl"].value_or(o.ksl);
    o.vib = x["vib"].value_or(o.vib);
    o.am = x["am"].value_or(o.am);
    o.tenuta = x["tenuta"].value_or(o.tenuta);
    o.ksr = x["ksr"].value_or(o.ksr);
    return o;
}

TimbroFm timbro(const toml::table& t) {
    TimbroFm x;
    x.nome = t["nome"].value_or(std::string{});
    x.mod = operatore(t["mod"].as_table());
    x.por = operatore(t["por"].as_table());
    x.retro = t["retro"].value_or(0);
    x.connessione = t["connessione"].value_or(0);
    x.nota_fissa = t["nota_fissa"].value_or(-1);
    return x;
}

std::uint32_t vlq(const std::vector<std::uint8_t>& b, std::size_t& i) {
    std::uint32_t v = 0;
    for (int k = 0; k < 4; ++k) {
        if (i >= b.size()) throw std::runtime_error("MIDI troncato");
        const std::uint8_t c = b[i++];
        v = (v << 7) | (c & 0x7F);
        if (!(c & 0x80)) return v;
    }
    throw std::runtime_error("MIDI: lunghezza variabile oltre 4 byte");
}

bool spegnimento(const std::array<std::uint8_t, 3>& d) {
    return (d[0] & 0xF0) == 0x80 || ((d[0] & 0xF0) == 0x90 && d[2] == 0);
}

}  // namespace

BancoFm carica_banco_fm(const std::string& percorso) {
    toml::table t;
    try {
        t = toml::parse_file(percorso);
    } catch (const toml::parse_error& e) {
        throw std::runtime_error(percorso + ": " + std::string(e.description()));
    }
    BancoFm b;
    b.programma_riserva = t["programma_riserva"].value_or(b.programma_riserva);
    for (const char* sezione : {"programmi", "ritmo"})
        if (const auto* s = t[sezione].as_table())
            for (const auto& [k, v] : *s) {
                const auto* r = v.as_table();
                if (!r) continue;
                const int n = std::stoi(std::string(k.str()));
                (std::string(sezione) == "programmi" ? b.programmi : b.ritmo)[n] = timbro(*r);
            }
    if (b.programmi.empty()) throw std::runtime_error(percorso + ": nessun timbro");
    return b;
}

BranoMidi leggi_midi(const std::vector<std::uint8_t>& b) {
    if (b.size() < 14 || std::string(b.begin(), b.begin() + 4) != "MThd") throw std::runtime_error("non e' un file MIDI");
    const int ntracce = (b[10] << 8) | b[11], divisione = (b[12] << 8) | b[13];
    if (divisione & 0x8000) throw std::runtime_error("MIDI a tempo SMPTE: non gestito");
    if (divisione == 0) throw std::runtime_error("MIDI senza divisione");
    // tick e ordine d'arrivo; il tipo: 0 tempo, 1 marcatore, 2 nota o controllo
    struct Grezzo {
        long tick;
        int priorita, ordine;
        std::array<std::uint8_t, 3> dati;
        std::uint32_t tempo = 0;
        int marcatore = 0;   // 1 inizio del loop, 2 fine
    };
    std::vector<Grezzo> g;
    std::size_t i = 8 + ((b[4] << 24) | (b[5] << 16) | (b[6] << 8) | b[7]);
    int ordine = 0;
    for (int k = 0; k < ntracce && i + 8 <= b.size(); ++k) {
        if (std::string(b.begin() + static_cast<long>(i), b.begin() + static_cast<long>(i) + 4) != "MTrk")
            throw std::runtime_error("MIDI: traccia attesa");
        const std::size_t lung = (static_cast<std::size_t>(b[i + 4]) << 24) | (b[i + 5] << 16) | (b[i + 6] << 8) | b[i + 7];
        std::size_t j = i + 8;
        const std::size_t fine = std::min(b.size(), j + lung);
        long tick = 0;
        std::uint8_t stato = 0;
        while (j < fine) {
            tick += static_cast<long>(vlq(b, j));
            if (j >= fine) break;
            const std::uint8_t c = b[j];
            if (c == 0xFF) {
                if (j + 2 > fine) break;
                const std::uint8_t tipo = b[j + 1];
                j += 2;
                const std::uint32_t n = vlq(b, j);
                if (j + n > fine) throw std::runtime_error("MIDI: meta troncato");
                if (tipo == 0x51 && n == 3) {
                    Grezzo x{tick, 0, ordine++, {}};
                    x.tempo = (static_cast<std::uint32_t>(b[j]) << 16) | (b[j + 1] << 8) | b[j + 2];
                    g.push_back(x);
                } else if (tipo == 0x01 || tipo == 0x06) {
                    const std::string testo(b.begin() + static_cast<long>(j), b.begin() + static_cast<long>(j + n));
                    const int m = testo == "loop_inizio" || testo == "loopStart" ? 1 : testo == "loop_fine" || testo == "loopEnd" ? 2 : 0;
                    if (m) {
                        Grezzo x{tick, 1, ordine++, {}};
                        x.marcatore = m;
                        g.push_back(x);
                    }
                } else if (tipo == 0x2F) {
                    j += n;
                    break;
                }
                j += n;
                continue;
            }
            if (c == 0xF0 || c == 0xF7) {
                ++j;
                j += vlq(b, j);
                continue;
            }
            if (c & 0x80) {
                stato = c;
                ++j;
            }
            if (!(stato & 0x80)) throw std::runtime_error("MIDI: dati senza stato");
            const int n = (stato & 0xF0) == 0xC0 || (stato & 0xF0) == 0xD0 ? 1 : 2;
            if (j + static_cast<std::size_t>(n) > fine) break;
            Grezzo x{tick, 2, ordine++, {stato, b[j], static_cast<std::uint8_t>(n > 1 ? b[j + 1] : 0)}};
            j += static_cast<std::size_t>(n);
            if (spegnimento(x.dati)) x.priorita = 1;   // a pari tempo gli spegnimenti prima delle accensioni
            g.push_back(x);
        }
        i = i + 8 + lung;
    }
    std::stable_sort(g.begin(), g.end(), [](const Grezzo& a, const Grezzo& c) {
        return a.tick != c.tick ? a.tick < c.tick : a.priorita < c.priorita;
    });
    BranoMidi out;
    double us_semiminima = 500000.0, t = 0.0;
    long tick_prec = 0;
    for (const auto& x : g) {
        t += static_cast<double>(x.tick - tick_prec) * us_semiminima / divisione / 1e6;
        tick_prec = x.tick;
        if (x.priorita == 0) {
            if (x.tempo > 0) us_semiminima = x.tempo;
            continue;
        }
        if (x.marcatore == 1) out.loop_inizio_s = t;
        else if (x.marcatore == 2) out.loop_fine_s = t;
        else out.eventi.push_back({t, x.dati});
        out.durata_s = std::max(out.durata_s, t);
    }
    return out;
}

BranoMidi leggi_midi_file(const std::string& percorso) {
    std::ifstream in(percorso, std::ios::binary);
    if (!in) throw std::runtime_error(percorso + ": non si apre");
    const std::vector<std::uint8_t> b((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    try {
        return leggi_midi(b);
    } catch (const std::runtime_error& e) {
        throw std::runtime_error(percorso + ": " + e.what());
    }
}

// --- il suonatore ---

struct SuonatoreFm::Stato {
    struct Voce {
        int canale = -1, nota = -1;
        long inizio = 0;
        bool accesa = false;
        const TimbroFm* timbro = nullptr;
        double att_base = 0.0;   // dB dalla velocita'
    };
    BancoFm banco;
    Interfaccia intf;
    ymfm::ymf262 chip{intf};
    double fs = 49716.0;
    Voce voci[VOCI];
    int bloc[VOCI] = {};
    int volume[16], espressione[16], programma[16];
    BranoMidi brano;
    bool attivo = false, loop = false;
    std::size_t prossimo = 0;
    long campioni = 0;        // generati dall'inizio del brano (per l'eta' delle voci)
    double t = 0.0;           // nel brano
    double fine_coda = -1.0;  // senza loop: il brano e' finito, la coda fino a qui
    float vol = 1.0f;
    double dissolvenza = 1.0, passo_dissolvenza = 0.0;

    explicit Stato(BancoFm b) : banco(std::move(b)) {
        fs = static_cast<double>(chip.sample_rate(CLOCK_OPL3));
        azzera_chip();
    }
    void scrivi(int reg, int val) {
        if (reg & 0x100) chip.write_address_hi(static_cast<std::uint8_t>(reg & 0xFF));
        else chip.write_address(static_cast<std::uint8_t>(reg & 0xFF));
        chip.write_data(static_cast<std::uint8_t>(val));
    }
    void azzera_chip() {
        chip.reset();
        scrivi(0x105, 0x01);   // modo OPL3
        scrivi(0x104, 0x00);   // niente 4 operatori
        scrivi(0x01, 0x20);
        scrivi(0xBD, 0xC0);    // vibrato e tremolo profondi
        for (int c = 0; c < 16; ++c) volume[c] = 100, espressione[c] = 127, programma[c] = 0;
        for (auto& v : voci) v = Voce{};
        for (int& x : bloc) x = 0;
    }
    static int base(int voce) { return voce < 9 ? 0 : 0x100; }
    static int ch(int voce) { return voce % 9; }
    void op(int voce, int quale, const OperatoreFm& o, int tl) {
        const int r = base(voce) + OFFSET_MOD[ch(voce)] + (quale ? 3 : 0);
        scrivi(0x20 + r, (o.am ? 0x80 : 0) | (o.vib ? 0x40 : 0) | (o.tenuta ? 0x20 : 0) | (o.ksr ? 0x10 : 0) | (o.mult & 15));
        scrivi(0x40 + r, ((o.ksl & 3) << 6) | std::clamp(tl, 0, 63));
        scrivi(0x60 + r, ((o.ar & 15) << 4) | (o.dr & 15));
        scrivi(0x80 + r, ((o.sl & 15) << 4) | (o.rr & 15));
        scrivi(0xE0 + r, o.onda & 7);
    }
    void carica(int voce, const TimbroFm& tb) {
        op(voce, 0, tb.mod, tb.mod.tl);
        op(voce, 1, tb.por, tb.por.tl);
        scrivi(base(voce) + 0xC0 + ch(voce), 0x30 | ((tb.retro & 7) << 1) | (tb.connessione & 1));
        bloc[voce] = 0;
    }
    void livello(int voce) {
        const Voce& v = voci[voce];
        const double gc = (volume[v.canale] / 127.0) * (espressione[v.canale] / 127.0);
        const double att = v.att_base - 20.0 * std::log10(std::max(gc, 1e-3));
        const int passi = static_cast<int>(std::lround(att / 0.75));
        const int r = base(voce) + OFFSET_MOD[ch(voce)] + 3;
        scrivi(0x40 + r, ((v.timbro->por.ksl & 3) << 6) | std::clamp(v.timbro->por.tl + passi, 0, 63));
        if (v.timbro->connessione) {   // additiva: anche il modulatore si sente
            const int rm = base(voce) + OFFSET_MOD[ch(voce)];
            scrivi(0x40 + rm, ((v.timbro->mod.ksl & 3) << 6) | std::clamp(v.timbro->mod.tl + passi, 0, 63));
        }
    }
    void frequenza(int voce, int nota) {
        const double f = 440.0 * std::pow(2.0, (nota - 69) / 12.0);
        int blocco = 0;
        double fn = f * (1 << 20) / fs;
        while (fn >= 1024.0 && blocco < 7) {
            fn /= 2.0;
            ++blocco;
        }
        const int fnum = std::clamp(static_cast<int>(std::lround(fn)), 0, 1023);
        scrivi(base(voce) + 0xA0 + ch(voce), fnum & 0xFF);
        bloc[voce] = (blocco << 2) | (fnum >> 8);
    }
    void chiave(int voce, bool su) { scrivi(base(voce) + 0xB0 + ch(voce), bloc[voce] | (su ? 0x20 : 0)); }
    const TimbroFm* timbro_per(int canale, int nota) const {
        if (canale == 9) {
            const auto it = banco.ritmo.find(nota);
            return it == banco.ritmo.end() ? nullptr : &it->second;
        }
        auto it = banco.programmi.find(programma[canale]);
        if (it == banco.programmi.end()) it = banco.programmi.find(banco.programma_riserva);
        if (it == banco.programmi.end()) it = banco.programmi.begin();
        return &it->second;
    }
    void accendi(int c, int n, int vel) {
        const TimbroFm* tb = timbro_per(c, n);
        if (!tb) return;
        int scelta = -1;
        for (int i = 0; i < VOCI && scelta < 0; ++i)
            if (!voci[i].accesa && voci[i].timbro == tb) scelta = i;
        for (int i = 0; i < VOCI && scelta < 0; ++i)
            if (!voci[i].accesa) scelta = i;
        if (scelta < 0) {   // la piu' vecchia cede
            scelta = 0;
            for (int i = 1; i < VOCI; ++i)
                if (voci[i].inizio < voci[scelta].inizio) scelta = i;
        }
        Voce& v = voci[scelta];
        chiave(scelta, false);
        if (v.timbro != tb) carica(scelta, *tb);
        v.canale = c;
        v.nota = n;
        v.inizio = campioni;
        v.timbro = tb;
        v.accesa = true;
        v.att_base = -20.0 * std::log10(std::pow(std::max(1, vel) / 127.0, 1.5));
        livello(scelta);
        frequenza(scelta, tb->nota_fissa >= 0 ? tb->nota_fissa : n);
        chiave(scelta, true);
    }
    void spegni(int c, int n) {
        for (int i = 0; i < VOCI; ++i)
            if (voci[i].accesa && voci[i].canale == c && voci[i].nota == n) {
                voci[i].accesa = false;
                chiave(i, false);
            }
    }
    void tutte_spente() {
        for (int i = 0; i < VOCI; ++i)
            if (voci[i].accesa) {
                voci[i].accesa = false;
                chiave(i, false);
            }
    }
    void evento(const EventoMidi& e) {
        const std::uint8_t st = e.dati[0];
        const int c = st & 0x0F;
        switch (st & 0xF0) {
            case 0x90:
                if (e.dati[2]) accendi(c, e.dati[1], e.dati[2]);
                else spegni(c, e.dati[1]);
                break;
            case 0x80: spegni(c, e.dati[1]); break;
            case 0xB0:
                if (e.dati[1] == 7) volume[c] = e.dati[2];
                if (e.dati[1] == 11) espressione[c] = e.dati[2];
                if (e.dati[1] == 7 || e.dati[1] == 11)
                    for (int i = 0; i < VOCI; ++i)
                        if (voci[i].accesa && voci[i].canale == c) livello(i);
                if (e.dati[1] == 123) tutte_spente();
                break;
            case 0xC0: programma[c] = e.dati[1]; break;
            default: break;
        }
    }
    double fine_loop() const { return brano.loop_fine_s >= 0.0 ? brano.loop_fine_s : brano.durata_s; }
    // gli eventi fino al tempo t; col loop, alla fine si torna all'inizio del loop
    void avanza_eventi() {
        if (loop && fine_loop() > brano.loop_inizio_s && t >= fine_loop()) {
            tutte_spente();
            t = brano.loop_inizio_s + (t - fine_loop());
            prossimo = static_cast<std::size_t>(std::lower_bound(brano.eventi.begin(), brano.eventi.end(), brano.loop_inizio_s,
                                                                 [](const EventoMidi& e, double x) { return e.t_s < x; }) -
                                                brano.eventi.begin());
        }
        while (prossimo < brano.eventi.size() && brano.eventi[prossimo].t_s <= t) evento(brano.eventi[prossimo++]);
        if (!loop && prossimo >= brano.eventi.size() && fine_coda < 0.0) fine_coda = t + CODA_S;
    }
};

SuonatoreFm::SuonatoreFm(BancoFm banco) : s_(std::make_unique<Stato>(std::move(banco))) {}
SuonatoreFm::~SuonatoreFm() = default;

void SuonatoreFm::suona(const BranoMidi& brano, bool in_loop) {
    s_->azzera_chip();
    s_->brano = brano;
    s_->loop = in_loop;
    s_->attivo = true;
    s_->prossimo = 0;
    s_->campioni = 0;
    s_->t = 0.0;
    s_->fine_coda = -1.0;
    s_->dissolvenza = 1.0;
    s_->passo_dissolvenza = 0.0;
}

void SuonatoreFm::ferma(double dissolvenza_s) {
    if (!s_->attivo) return;
    if (dissolvenza_s <= 0.0) {
        s_->tutte_spente();
        s_->attivo = false;
        return;
    }
    s_->passo_dissolvenza = s_->dissolvenza / (dissolvenza_s * s_->fs);
}

bool SuonatoreFm::suona_ancora() const { return s_->attivo; }
double SuonatoreFm::tempo_s() const { return s_->t; }
void SuonatoreFm::volume(float v) { s_->vol = std::clamp(v, 0.0f, 1.0f); }
int SuonatoreFm::campionamento_hz() const { return static_cast<int>(std::lround(s_->fs)); }

std::vector<float> SuonatoreFm::genera(std::size_t n) {
    std::vector<float> out(n, 0.0f);
    Stato& s = *s_;
    if (!s.attivo) return out;
    const double dt = 1.0 / s.fs;
    for (std::size_t k = 0; k < n; ++k) {
        s.avanza_eventi();
        ymfm::ymf262::output_data o;
        s.chip.generate(&o);
        const double x = 0.5 * (o.data[0] + o.data[1]) / 32768.0 * GUADAGNO;
        out[k] = static_cast<float>(std::clamp(x * s.dissolvenza * s.vol, -1.0, 1.0));
        s.t += dt;
        ++s.campioni;
        if (s.passo_dissolvenza > 0.0) {
            s.dissolvenza -= s.passo_dissolvenza;
            if (s.dissolvenza <= 0.0) {
                s.tutte_spente();
                s.attivo = false;
                break;
            }
        }
        if (s.fine_coda >= 0.0 && s.t >= s.fine_coda) {
            s.attivo = false;
            break;
        }
    }
    return out;
}

}  // namespace m688
