#pragma once
// Che cosa le parti credono di chi le inganna, dalla sola traccia (D4, docs/28 §3.5): il cambio di parte, finche' non lo
// sanno, e la false flag, dalla colpa data in poi. Per il replay e le sue prove (trailer-1)
#include <map>
#include <set>
#include <string>
#include "nucleo/traccia_replay.hpp"

namespace m688 {

struct CambioCreduto {
    double t = 0.0;
    int prima = 0, nuova = 0;        // i bit della parte
    bool sa_nato = false, sa_patto = false;   // tutta la parte (reazioni cambio_saputo_usa, cambio_saputo_urss)
    std::set<int> sanno;             // le unita' che lo sanno prima della loro parte: chi e' stato attaccato
};

inline std::string nome_parte_creduta(int lato) {
    return lato == 0x08 ? "NATO" : lato == 0x10 ? "PATTO" : lato == 0x18 ? "DEL LUOGO" : "NEUTRALE";
}

// i cambi di parte applicati fino a `t`, per unita'; dell'ultimo, se un'unita' ne ha piu' d'uno. La traccia scrive il flag
// di un'unita' una volta sola, quella d'inizio partita: la parte nuova e' nel record X
inline std::map<int, CambioCreduto> cambi_creduti(const Traccia& tr, double t) {
    std::map<int, CambioCreduto> m;
    for (const RecordIntervento& r : tr.interventi) {
        if (r.tipo != "cambia_parte" || r.fase != RecordIntervento::FASE_APPLICATO || r.t > t) continue;
        const auto s = tr.unita.find(r.unita);
        if (s == tr.unita.end() || s->second.empty()) continue;
        const auto gia = m.find(r.unita);
        CambioCreduto c;
        c.t = r.t;
        c.prima = gia != m.end() ? gia->second.nuova : s->second.front().second.flag & 0x18;
        const std::string parte = leggi_chiavi(r.chiavi)["parte"];
        c.nuova = parte == "usa" ? 0x08 : parte == "urss" ? 0x10 : 0;
        m[r.unita] = c;
    }
    if (m.empty()) return m;
    constexpr double TEMPI_EVENTI_S = 0.1;   // il registro scrive i tempi al decimo: l'evento puo' cadere appena prima del record X
    for (const EventoTraccia& e : tr.registro) {
        if (e.tempo > t || e.tipo != "reazione_ia") continue;
        if (e.classe_arma == "cambio_saputo_usa" || e.classe_arma == "cambio_saputo_urss") {
            const auto c = m.find(e.unita);
            if (c != m.end() && e.tempo + TEMPI_EVENTI_S >= c->second.t) (e.classe_arma == "cambio_saputo_usa" ? c->second.sa_nato : c->second.sa_patto) = true;
        } else if (e.classe_arma == "cambio_saputo") {
            const auto c = m.find(e.altra);
            if (c != m.end() && e.tempo + TEMPI_EVENTI_S >= c->second.t) c->second.sanno.insert(e.unita);
        }
    }
    return m;
}

// la parte di `unita` per chi guarda: una parte intera (`lato`), o una sua unita' (`chi`, altrimenti -1); `vera` senza cambi
inline int parte_creduta_da(const std::map<int, CambioCreduto>& cambi, int unita, int lato, int chi, int vera) {
    const auto it = cambi.find(unita);
    if (it == cambi.end()) return vera;
    const CambioCreduto& c = it->second;
    const bool sa = chi == unita || c.sanno.count(chi) || (lato == 0x08 && c.sa_nato) || (lato == 0x10 && c.sa_patto);
    return sa ? c.nuova : c.prima;
}

// la scritta per lo spettatore; vuota quando lo sanno tutte e due le parti
inline std::string etichetta_creduta(const CambioCreduto& c) {
    if (c.sa_nato && c.sa_patto) return {};
    const std::string base = "CREDUTA " + nome_parte_creduta(c.prima);
    // chi non lo sa e' la parte di prima, o nessuno e' informato: basta la parte creduta. Altrimenti si dice chi ci crede
    if ((!c.sa_nato && !c.sa_patto) || (c.prima == 0x08 && !c.sa_nato) || (c.prima == 0x10 && !c.sa_patto)) return base;
    return base + (c.sa_nato ? " DAL PATTO" : " DALLA NATO");
}

// la false flag: chi ha lanciato davvero -> la scritta, dal colpo (la parte colpita da' la colpa tutta insieme). Niente
// scritta se la colpa cade sulla parte giusta, su nessuno, o se l'arma non ha lanciatore
inline std::map<int, std::string> colpe_credute(const Traccia& tr, double t) {
    std::map<int, std::string> m;
    const auto cambi = cambi_creduti(tr, t);
    for (const EventoTraccia& e : tr.registro) {
        if (e.tempo > t || e.tipo != "reazione_ia" || e.altra < 0 || e.classe_arma.rfind("attribuisce_", 0) != 0) continue;
        const std::string a = e.classe_arma.substr(12);
        const int creduta = a == "usa" ? 0x08 : a == "urss" ? 0x10 : a == "neutrale" ? 0 : a == "luogo" ? 0x18 : -1;
        const auto s = tr.unita.find(e.altra);
        if (creduta < 0 || s == tr.unita.end() || s->second.empty()) continue;
        const auto c = cambi.find(e.altra);
        if ((c != cambi.end() ? c->second.nuova : s->second.front().second.flag & 0x18) == creduta) m.erase(e.altra);
        else m[e.altra] = "CREDUTA " + nome_parte_creduta(creduta);
    }
    return m;
}

}  // namespace m688
