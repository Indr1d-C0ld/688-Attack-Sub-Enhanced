#pragma once

#include <algorithm>
#include <functional>
#include <string>
#include <vector>

namespace m688 {

// Le etichette degli eventi sulla carta del debriefing (scelta dell'utente del 07/11/2026): in S5 fino a 336 sulla stessa
// carta. Quelle che si toccherebbero, alla scala di adesso, diventano una sola: «12-18» se i numeri sono di seguito, se no
// «12+6», il primo e quanti altri (10-241 direbbe 232 eventi, e nel gruppo ce ne sono una ventina). Avvicinandosi tornano
// separate. Il colore e' quello dell'evento piu' interessante del gruppo.
struct PuntoEtichetta {
    int x = 0, y = 0;
    int numero = 0;      // dell'evento nell'elenco
    int interesse = 0;
};

struct EtichettaCarta {
    double x = 0.0, y = 0.0;   // il centro: la media dei suoi eventi
    int primo = 0, ultimo = 0, quanti = 1;
    int capo = 0;              // il numero dell'evento piu' interessante
    int interesse = 0;
    std::string testo() const {
        if (primo == ultimo) return std::to_string(primo);
        return std::to_string(primo) + (ultimo - primo + 1 == quanti ? "-" + std::to_string(ultimo) : "+" + std::to_string(quanti - 1));
    }
};

inline constexpr int ALTEZZA_ETICHETTA = 11;

inline std::vector<EtichettaCarta> raccogli_etichette(const std::vector<PuntoEtichetta>& punti, const std::function<int(const std::string&)>& largo) {
    std::vector<EtichettaCarta> v;
    v.reserve(punti.size());
    for (const PuntoEtichetta& p : punti) {
        EtichettaCarta e;
        e.x = p.x;
        e.y = p.y;
        e.primo = e.ultimo = e.capo = p.numero;
        e.interesse = p.interesse;
        v.push_back(e);
    }
    const auto si_toccano = [&](const EtichettaCarta& a, const EtichettaCarta& b) {
        const double w = (largo(a.testo()) + largo(b.testo())) / 2.0 + 4.0;
        return std::abs(a.x - b.x) < w + 1.0 && std::abs(a.y - b.y) < ALTEZZA_ETICHETTA + 1.0;
    };
    // un gruppo piu' largo puo' toccarne altri: si ripete finche' non se ne fondono piu'
    for (bool fusi = true; fusi;) {
        fusi = false;
        for (std::size_t i = 0; i < v.size(); ++i)
            for (std::size_t j = i + 1; j < v.size();) {
                if (!si_toccano(v[i], v[j])) {
                    ++j;
                    continue;
                }
                EtichettaCarta& a = v[i];
                const EtichettaCarta& b = v[j];
                const double n = a.quanti + b.quanti;
                a.x = (a.x * a.quanti + b.x * b.quanti) / n;
                a.y = (a.y * a.quanti + b.y * b.quanti) / n;
                a.primo = std::min(a.primo, b.primo);
                a.ultimo = std::max(a.ultimo, b.ultimo);
                a.quanti += b.quanti;
                if (b.interesse > a.interesse) a.interesse = b.interesse, a.capo = b.capo;
                v.erase(v.begin() + static_cast<std::ptrdiff_t>(j));
                fusi = true;
            }
    }
    return v;
}

}  // namespace m688
