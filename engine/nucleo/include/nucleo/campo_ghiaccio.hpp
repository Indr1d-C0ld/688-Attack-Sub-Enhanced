#pragma once
// Il campo del ghiaccio per le operazioni sotto la banchisa (opzione A di sotto-ghiaccio-piano.md, approvata il
// 29/10/2026; docs/08 §7; dossier docs/ricerca/06 §5). Procedurale e deterministico dal seme, sulla copertura del
// meteo (StatoMeteo::ghiaccio, 0..1, che il runtime da' per punto):
// - le celle (`cella_m`, come il fondale) hanno uno spessore livellato (primo anno ~1 m, pluriennale ~3 m: STIMA) e,
//   con probabilita' dalle creste per km, una chiglia col pescaggio lognormale di Ervik 2023 (Barents SW, primo anno:
//   media 3,8 m, 99° percentile 9,4 m, massimo ~20; nel pack pluriennale creste fino a 50 m, Sharpe 1988);
// - l'acqua libera (polynye e canali) a blocchi di `scala_aperta_m`: a copertura piena il 10% (CNR08), meta' dei
//   blocchi richiusi da ghiaccio nuovo sottile (nilas, 5-15 cm), che e' quello che un 688 Flight I/II puo' sfondare;
// - il ghiaccio deriva rigido (sotto 1 nodo): in(p, t) guarda il punto p - deriva x t del campo.
// Le chiglie qui sono per cella: il pescaggio massimo della cella. Per la carta e la volta del quadro bastano; la
// forma fine di una cresta (lunga, stretta) e' una semplificazione. Modulo puro.

#include <cstdint>
#include <functional>
#include <vector>

#include "nucleo/geometria.hpp"

namespace m688 {

struct ParametriGhiaccio {
    double cella_m = 200.0;
    double spessore_livellato_m = 1.0;      // primo anno (STIMA); il pluriennale 3
    double variazione_spessore = 0.3;       // +-30% fra le celle (STIMA)
    // le chiglie: lognormale con media e 99° percentile (Ervik 2023), troncata al massimo
    double chiglia_media_m = 3.8, chiglia_p99_m = 9.4, chiglia_max_m = 20.0;
    double creste_per_km = 4.0;             // STIMA: creste attraversate per km di rotta
    double frazione_aperta = 0.10;          // a copertura piena: polynye e canali (CNR08)
    double frazione_nilas = 0.5;            // di quell'acqua: richiusa da ghiaccio nuovo sottile
    double nilas_min_m = 0.05, nilas_max_m = 0.15;
    double scala_aperta_m = 1500.0;         // il blocco d'acqua libera
    Vec2 deriva_m_s{0.1, 0.05};             // la deriva del pack (sotto 1 nodo, STIMA)
};
// il pack pluriennale dell'Artico centrale (Sharpe 1988, dossier 06 §5): 3 m livellati, creste fino a 50 m
ParametriGhiaccio ghiaccio_pluriennale();

struct PuntoGhiaccio {
    bool ghiaccio = false;       // c'e' ghiaccio sopra (anche il nilas)
    bool aperta = false;         // acqua libera
    bool nilas = false;          // ghiaccio nuovo sottile in una polynya o un canale richiuso
    double spessore_m = 0.0;     // livellato (o del nilas)
    double pescaggio_m = 0.0;    // la parte sotto il pelo dell'acqua: la chiglia se c'e', se no 0,9 dello spessore
    bool chiglia = false;
};

// Una zona forzata (il vetrino di Dio, D4; docs/28 §3.3): un cerchio fermo nel mondo dove il ghiaccio cambia.
// - apri: una polynya che si apre dal centro in `transizione_s`, acqua libera;
// - chiudi: chi e' acqua libera diventa nilas che ingrossa (da nilas_min_m a nilas_max_m) durante la transizione, poi
//   ghiaccio livellato; il ghiaccio che c'era resta. Chi e' emerso non resta preso all'istante.
// Con `durata_s` > 0, dopo la transizione e la durata si torna indietro nello stesso tempo; con 0 resta per sempre
struct ZonaGhiaccio {
    Vec2 centro;
    double raggio_m = 0.0;
    bool apri = true;
    double t0 = 0.0, transizione_s = 0.0, durata_s = 0.0;
};

class CampoGhiaccio {
public:
    // `copertura`: la frazione di ghiaccio nel punto (0..1), nel sistema del mondo; vuota: piena ovunque
    CampoGhiaccio(std::uint64_t seme, ParametriGhiaccio p = {}, std::function<double(Vec2)> copertura = {});
    PuntoGhiaccio in(Vec2 p, double t) const;
    // La deriva a tratti (il vetrino di Dio): da `t` il pack va a `v`, senza salti del campo. Il bordo del pack gli va
    // dietro: la copertura si guarda nel punto spostato di quanto la deriva e' cambiata rispetto a quella dei parametri.
    // Gli istanti non decrescenti: uno piu' vecchio dell'ultimo si ignora
    void cambia_deriva(double t, Vec2 v_m_s);
    void forza_zona(const ZonaGhiaccio& z) { zone_.push_back(z); }
    Vec2 spostamento(double t) const;   // quanto ha derivato il campo dall'inizio
    // La copertura nel punto con la deriva cambiata e le zone forzate: quella che devono leggere tutti (l'acustica, chi
    // guarda se c'e' ghiaccio sopra). Senza cambi e senza zone e' `copertura`, bit per bit
    double copertura_in(Vec2 p, double t) const;
    bool modificato() const { return !tratti_.empty() || !zone_.empty(); }
    // il pescaggio massimo lungo il segmento a -> b (a passi di mezza cella), per l'urto della vela e le avvertenze
    double pescaggio_max(Vec2 a, Vec2 b, double t) const;
    const ParametriGhiaccio& parametri() const { return p_; }

private:
    double u01(std::int64_t i, std::int64_t j, std::uint64_t sale) const;   // un numero 0..1 per cella e sale
    PuntoGhiaccio in_campo(Vec2 p, double t) const;                         // senza le zone forzate
    double spessore_cella(Vec2 q) const;
    std::uint64_t seme_;
    ParametriGhiaccio p_;
    std::function<double(Vec2)> copertura_;
    struct TrattoDeriva { double t; Vec2 v; };
    std::vector<TrattoDeriva> tratti_;
    std::vector<ZonaGhiaccio> zone_;
    double mu_ = 0.0, sigma_ = 0.0;   // della lognormale delle chiglie
};

}  // namespace m688
