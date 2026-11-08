#include "righe_automatiche.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>

#include "grafica/palette.hpp"

namespace m688 {

namespace {

std::vector<AreaAttiva>* aree_ = nullptr;
void area(int x, int y, int w, int h, std::string comando) {
    if (aree_) aree_->push_back({x, y, w, h, std::move(comando)});
}

std::string virgola(std::string s) {
    for (char& c : s) if (c == '.') c = ',';
    return s;
}

}  // namespace

std::vector<RigaAutomatica> VistaRigheAutomatiche::in_ordine(const std::vector<RigaAutomatica>& righe) {
    std::vector<RigaAutomatica> v;
    for (const auto& r : righe)
        if (r.confermata) v.push_back(r);
    std::sort(v.begin(), v.end(), [](const RigaAutomatica& a, const RigaAutomatica& b) { return a.hz < b.hz || (a.hz == b.hz && a.id < b.id); });
    return v;
}

void VistaRigheAutomatiche::su_giu(const std::vector<RigaAutomatica>& righe, int verso) {
    const auto v = in_ordine(righe);
    if (v.empty()) { scelta_ = -1; return; }
    int i = -1;
    for (std::size_t k = 0; k < v.size(); ++k)
        if (v[k].id == scelta_) i = static_cast<int>(k);
    i = i < 0 ? 0 : std::clamp(i + (verso > 0 ? 1 : -1), 0, static_cast<int>(v.size()) - 1);
    scelta_ = v[static_cast<std::size_t>(i)].id;
}

std::optional<RigaAutomatica> VistaRigheAutomatiche::riga_scelta(const std::vector<RigaAutomatica>& righe) const {
    for (const auto& r : righe)
        if (r.id == scelta_ && r.confermata) return r;
    return std::nullopt;
}

std::string VistaRigheAutomatiche::riga(const RigaAutomatica& r, double ora) {
    char b[96];
    const int eta = std::max(0, static_cast<int>(r.eta_s(ora)));
    std::snprintf(b, sizeof b, "R%d  %.1f HZ  %+.2f HZ/MIN  %.0f DB  %d:%02d", r.id, r.hz, r.deriva_hz_min, r.snr_db, eta / 60, eta % 60);
    return virgola(b);
}

int VistaRigheAutomatiche::x_di(Rettangolo lofar, const StileRighe& s, double hz) {
    return lofar.x + static_cast<int>((hz - s.f0_hz) / (s.f1_hz - s.f0_hz) * lofar.w);
}

void VistaRigheAutomatiche::disegna_marche(Tela& t, const Font& f, Rettangolo lofar, const std::vector<RigaAutomatica>& righe,
                                           const StileRighe& s, std::vector<AreaAttiva>* aree) const {
    if (s.f1_hz <= s.f0_hz) return;
    aree_ = aree;
    const Rettangolo prima = t.ritaglio();
    t.imposta_ritaglio(lofar);
    const int y = lofar.y;
    // le etichette, come i segnalibri delle SDR (docs/ricerca/30, n. 8): sulla prima riga libera da sinistra, al massimo
    // ETICHETTE_RIGHE; quelle che non trovano posto diventano un «+n» sull'ultima etichetta della riga piu' bassa
    struct Etichetta { int id, x, riga, piu; std::string testo; std::uint8_t col; };
    std::vector<Etichetta> et;
    std::vector<int> fine_riga;                   // la x dove finisce l'ultima etichetta di ogni riga
    std::vector<std::size_t> ultima_riga;         // e quale e'
    const auto fine = [&](const Etichetta& e) {
        return e.x + 5 + f.larghezza(e.piu ? e.testo + " +" + std::to_string(e.piu) : e.testo, 1);
    };
    for (const auto& r : in_ordine(righe)) {
        const auto col = static_cast<std::uint8_t>(r.id == scelta_ ? colori::BIANCO : colori::VERDE_CHIARO);
        if (r.hz < s.f0_hz || r.hz > s.f1_hz) {   // fuori finestra: una freccetta sul bordo dalla sua parte
            const int x = r.hz < s.f0_hz ? lofar.x + 1 : lofar.x + lofar.w - 2;
            const int d = r.hz < s.f0_hz ? 1 : -1;
            for (int k = 0; k < 3; ++k) t.linea_verticale(x + d * k, y + 2 - k, y + 2 + k, col);
            continue;
        }
        const int x = x_di(lofar, s, r.hz);
        for (int k = 0; k < 4; ++k) t.linea_orizzontale(x - 3 + k, x + 3 - k, y + k, col);   // il triangolino verso il basso
        std::string n = std::to_string(r.id);
        if (r.deriva_hz_min > DERIVA_FRECCIA_HZ_MIN) n += ">";
        else if (r.deriva_hz_min < -DERIVA_FRECCIA_HZ_MIN) n = "<" + n;
        std::size_t k = 0;
        while (k < fine_riga.size() && fine_riga[k] + ETICHETTA_STACCO_PX > x + 5) ++k;
        if (k >= static_cast<std::size_t>(ETICHETTE_RIGHE)) {   // nessuna riga libera: conta sull'ultima della piu' bassa
            Etichetta& u = et[ultima_riga.back()];
            ++u.piu;
            fine_riga.back() = fine(u);
            area(x - 4, y, 8, 8, "riga_auto_marca_" + std::to_string(r.id));   // il triangolino resta cliccabile
            continue;
        }
        et.push_back({r.id, x, static_cast<int>(k), 0, n, col});
        if (k == fine_riga.size()) { fine_riga.push_back(0); ultima_riga.push_back(0); }
        fine_riga[k] = fine(et.back());
        ultima_riga[k] = et.size() - 1;
    }
    // prima le linee a puntini fino alla riga di ognuna, poi le etichette sul fondo nero: una linea non taglia il testo
    for (const auto& e : et)
        for (int yy = y + 5; yy < y + e.riga * ETICHETTA_PASSO_PX + 4; yy += 2) t.pixel(e.x, yy, e.col);
    for (const auto& e : et) {
        const int ye = y + e.riga * ETICHETTA_PASSO_PX, w = f.larghezza(e.testo, 1);
        const std::string piu = e.piu ? " +" + std::to_string(e.piu) : "";
        // il fondo sulle righe disegnate dei caratteri (dalla 3 alla 11 della cella del font originale), non sulla cella intera
        t.rettangolo_pieno(e.x + 4, ye + 3, w + f.larghezza(piu, 1) + 2, ETICHETTA_PASSO_PX, colori::NERO);
        f.scrivi(t, e.x + 5, ye, e.testo, e.col, 1);
        if (e.piu) f.scrivi(t, e.x + 5 + w, ye, piu, colori::VERDE_CHIARO, 1);   // il conto delle nascoste, mai nel bianco della scelta
        area(e.x - 4, ye, 8 + w + 4, 8, "riga_auto_marca_" + std::to_string(e.id));
    }
    t.imposta_ritaglio(prima);
    aree_ = nullptr;
}

void VistaRigheAutomatiche::disegna_lista(Tela& t, const Font& f, Rettangolo r, const std::vector<RigaAutomatica>& righe,
                                          double ora, std::vector<AreaAttiva>* aree) const {
    aree_ = aree;
    const Rettangolo prima = t.ritaglio();
    t.imposta_ritaglio(r);
    t.rettangolo_pieno(r.x, r.y, r.w, r.h, colori::NERO);
    t.linea_orizzontale(r.x, r.x + r.w - 1, r.y, colori::GRIGIO);
    t.linea_orizzontale(r.x, r.x + r.w - 1, r.y + r.h - 1, colori::GRIGIO);
    t.linea_verticale(r.x, r.y, r.y + r.h - 1, colori::GRIGIO);
    t.linea_verticale(r.x + r.w - 1, r.y, r.y + r.h - 1, colori::GRIGIO);
    f.scrivi(t, r.x + 4, r.y + 3, "RIGHE AUTOMATICHE", colori::VERDE_CHIARO, 1);
    const auto v = in_ordine(righe);
    const int n = (r.h - 16) / RIGA_PX;
    int primo = 0;   // la scelta sempre in vista
    for (std::size_t k = 0; k < v.size(); ++k)
        if (v[k].id == scelta_) primo = std::max(0, static_cast<int>(k) - n + 1);
    for (int i = primo; i < static_cast<int>(v.size()) && i < primo + n; ++i) {
        const RigaAutomatica& x = v[static_cast<std::size_t>(i)];
        const int y = r.y + 14 + (i - primo) * RIGA_PX;
        const bool su = x.id == scelta_;
        if (su) t.rettangolo_pieno(r.x + 2, y - 1, r.w - 4, RIGA_PX, colori::GRIGIO_SCURO);
        const int col = su ? colori::BIANCO : x.eta_s(ora) < GIOVANE_S ? colori::GRIGIO : colori::VERDE_CHIARO;
        f.scrivi(t, r.x + 4, y, riga(x, ora), static_cast<std::uint8_t>(col), 1);
        area(r.x, y - 1, r.w, RIGA_PX, "riga_auto_" + std::to_string(i));
    }
    if (v.empty()) f.scrivi(t, r.x + 4, r.y + 14, "NESSUNA RIGA CONFERMATA", colori::GRIGIO, 1);
    t.imposta_ritaglio(prima);
    aree_ = nullptr;
}

}  // namespace m688
