#include "missioni/contatti_giocatore.hpp"

#include <algorithm>
#include <cmath>
#include <iterator>

namespace m688 {

std::vector<SegnoContatto> segni_contatti(const Missione& m, bool filtro) {
    std::vector<SegnoContatto> segni;
    const auto* g = m.giocatore();
    if (!g) return segni;
    std::vector<std::pair<std::uint64_t, int>> ordine;   // la lista dell'osservatore e' in ordine di creazione
    for (const auto& [id, c] : g->contatti) ordine.emplace_back(c.ordine, id);
    std::sort(ordine.begin(), ordine.end());
    for (const auto& [_, id] : ordine) {
        const auto it = m.unita().find(id);
        if (it == m.unita().end()) continue;   // orfano: 990C lo salta
        const UnitaClassica& u = it->second;
        const Contatto& c = g->contatti.at(id);
        if (!c.visibile()) continue;           // traccia debole nascosta dal lampeggio (996F)
        SegnoContatto s;
        s.id = id;
        s.lettera = c.lettera;
        if (c.posizione_nota) s.indicatori |= SEGNO_TRACCIA;   // nel classico e' la traccia solida (qualita' >= 800)
        if (c.durata > 800.0) s.indicatori |= SEGNO_CLASSIFICATO;
        if (c.durata >= 1400.0) s.indicatori |= SEGNO_CLASSE;
        if (u.flag & 6) s.indicatori |= SEGNO_NAVE_AEREO;
        if (u.flag & 0x20) s.indicatori |= SEGNO_ARMA;
        if (!(u.flag & 0x10)) s.indicatori |= SEGNO_OCCIDENTALE;
        if (u.collegata == g->id && (u.classe == "mk48" || u.classe == "type53")) s.indicatori |= SEGNO_FILOGUIDATO;
        if (filtro && !(s.indicatori & SEGNO_ARMA)) continue;
        s.rilevamento = c.rilevamento;   // misurato (nel classico e' quello vero)
        s.specchio = c.rilevamento_specchio;
        s.sensore = c.sensore;
        s.eccesso_db = c.eccesso_db;
        s.pos_stimata = c.pos_stimata;
        s.quota_stimata = c.quota_stimata;
        segni.push_back(s);
    }
    return segni;
}

std::uint8_t colore_segno(const SegnoContatto& s) {
    if (s.indicatori & SEGNO_FILOGUIDATO) return 14;
    if (!s.traccia()) return 14;                          // la X
    if (!(s.indicatori & SEGNO_CLASSIFICATO)) return 13;  // traccia solida, durata <= 800
    int c = (s.indicatori & SEGNO_NAVE_AEREO) ? 2 : 1;
    if (s.indicatori & SEGNO_CLASSE) c = ((s.indicatori & SEGNO_ARMA) ? 4 : c) + 8;
    return static_cast<std::uint8_t>(c);
}

BarraBersaglio barra_bersaglio(const Missione& m, int bersaglio) {
    BarraBersaglio b;
    const auto* g = m.giocatore();
    const auto it = m.unita().find(bersaglio);
    if (!g || it == m.unita().end()) return b;
    const auto c = g->contatti.find(bersaglio);
    if (c == g->contatti.end()) return b;
    const UnitaClassica& u = it->second;
    b.lettera = c->second.lettera;
    if (c->second.durata >= 1400.0) {
        b.titolo = BarraBersaglio::Titolo::classe;
        b.classe = u.classe;
        b.tipo_originale = u.tipo_originale;
    } else if (c->second.durata > 800.0) {   // secondo la quota vera
        b.titolo = u.quota > 0.0 ? BarraBersaglio::Titolo::sconosciuto_immersione : BarraBersaglio::Titolo::sconosciuto_superficie;
    } else {
        b.titolo = BarraBersaglio::Titolo::sconosciuto;
    }
    b.rilevamento_relativo = normalizza_360(c->second.rilevamento - g->rotta);   // misurato (classico: vero)
    b.sensore = c->second.sensore;
    b.eccesso_db = c->second.eccesso_db;
    b.rotta = normalizza_360(u.rotta);
    const double dx = (u.pos.x - g->pos.x) / MIGLIO, dy = (u.pos.y - g->pos.y) / MIGLIO;
    if (!c->second.posizione_nota || std::fabs(dx) >= 20.0 || std::fabs(dy) >= 20.0) return b;
    b.dati = true;
    b.distanza_decimi = static_cast<int>(std::hypot(dx, dy) * 10.0);
    b.velocita_kn = u.velocita;
    b.quota_ft = u.quota / 0.3048;
    b.superficie = u.quota <= 0.0;
    return b;
}

std::string_view nome_tipo_originale(int tipo) {
    static constexpr std::string_view NOMI[] = {
        "CACCIATORPEDINIERE CLASSE FORREST SHERMAN", "NAVE MERCANTILE", "NAVE DA CARICO", "", "",
        "INCROCIATORE LANCIAMISSILI CLASSE KARA", "KIROV", "FREGATA CLASSE KRIVAK II", "FREGATA CLASSE KONI",
        "INCROCIATORE CLASSE SLAVA", "PORTAEREI CLASSE NIMITZ", "CORAZZATA CLASSE IOWA",
        "CACCIATORPEDINIERE LANCIAMISSILI CLASSE KIDD", "FREGATA CLASSE OLIVER HAZARD PERRY",
        "RIFORNITRICE CLASSE SACRAMENTO", "BERSAGLIO PER MISSILI DA CROCIERA", "PETROLIERA",
        "ELICOTTERO KAMOV KA-25 HORMONE", "ELICOTTERO SEAHAWK SH-60B LAMPS III", "MSS",
        "SOTTOMARINO D'ATTACCO USA CLASSE 688", "SOTTOMARINO D'ATTACCO SOVIETICO CLASSE ALFA", "SILURO MK 48",
        "SOTTOMARINO D'ATTACCO CLASSE VICTOR III", "AEREO P-3C ORION", "BIOLOGICO",
        "SOTTOMARINO D'ATTACCO USA CLASSE 688", "SOTTOMARINO CLASSE STURGEON (SSN 676)",
        "SOTTOMARINO LANCIAMISSILI CLASSE OSCAR (SSGN)", "SOTTOMARINO D'ATTACCO CLASSE AKULA",
        "SOTTOMARINO LANCIAMISSILI CLASSE CHARLIE II", "SOTTOMARINO BALISTICO CLASSE TYPHOON",
        "SOTTOMARINO DIESEL CLASSE KILO", "SOTTOMARINO DIESEL CLASSE FOXTROT", "SOTTOMARINO D'ATTACCO CLASSE TRAFALGAR",
        "MISSILE TOMAHAWK", "SILURO SEA LANCE", "SILURO DA 533 MM", "SILURO DA 450 MM", "SILURO SPEARFISH",
        "MISSILE SS-N-19", "MISSILE HARPOON", "ESCA ACUSTICA", "BOA MARCATRICE SONAR", "SILURO MK 50", "SILURO DA 650 MM",
    };
    return tipo >= 0 && tipo < static_cast<int>(std::size(NOMI)) ? NOMI[tipo] : std::string_view{};
}

}  // namespace m688
