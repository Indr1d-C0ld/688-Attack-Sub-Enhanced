#pragma once
// Il registro dei tempi per fotogramma, per trovare gli scatti: MELCHIOR_REGISTRO_TEMPI=file lo accende, nel film e in partita.
// Spento costa un confronto e non cambia nulla. Una riga per fotogramma, i tempi in ms; le colonne sono nell'intestazione del file.
// Tocca solo gli orologi: le partite non lo vedono.

#include <time.h>

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string>

namespace m688 {

struct RigaTempi {
    double eventi = 0.0, passi = 0.0, passi_cpu = 0.0, passo_max = 0.0;   // passi_cpu: del solo thread; il resto e' attesa
    double disegno = 0.0, cuffia = 0.0, produci = 0.0, presenta = 0.0, present = 0.0;   // present: il solo SDL_RenderPresent, dentro presenta
    double sonar = 0.0, sonar_scena = 0.0, sonar_spettri = 0.0;   // la riga della cascata, dentro i passi; di cui la lettura della scena con la cascata, e gli spettri del LOFAR
    int passi_n = 0;
    long coda_min = -1;     // campioni nella coda d'uscita al momento di riempirla, il minimo del fotogramma
    long accodati = 0;      // campioni aggiunti alla coda
    long buco_cuffia = 0, buco_bordo = 0;   // campioni chiesti al mixer oltre quelli del canale, mentre il canale e' vivo
    double filo = 0.0;      // la chiamata piu' lunga del filo dell'audio
    long mancanti = 0;      // campioni che il dispositivo ha aspettato oltre il doppio del suo periodo
    long bordo_attesa = 0;   // lo stesso per il bordo
    long film_anticipo = 0, film_silenzio = 0;   // il film: i fotogrammi gia' pronti, i campioni di silenzio dati perche' la coda era vuota
    long cuffia_attesa = 0;  // campioni fermi nella coda della cuffia (mezzo secondo vuol dire mezzo secondo di ritardo)
};

class RegistroTempi {
public:
    using Riga = RigaTempi;

    static RegistroTempi* attivo() {
        static RegistroTempi* r = apri();
        return r;
    }
    static double cpu_s() {
        timespec t{};
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t);
        return static_cast<double>(t.tv_sec) + static_cast<double>(t.tv_nsec) * 1e-9;
    }
    static double ora_s() { return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count(); }

    void nota(const std::string& testo) {   // una riga di commento: renderer, driver, inizio di una scena
        std::fprintf(f_, "# %s\n", testo.c_str());
        std::fflush(f_);
    }
    // Il fotogramma e' finito: una riga. `scena`: l'inquadratura del film, la stazione in partita
    void chiudi(const char* luogo, int scena, double missione_s) {
        const double t = ora_s();
        if (prec_ < 0.0) prec_ = inizio_;
        const auto ms = [](double s) { return s * 1000.0; };
        std::fprintf(f_, "%.3f %s %d %.1f %.1f %.1f %.1f %.1f %.1f %d %.1f %.1f %.1f %.1f %.1f %ld %ld %ld %ld %.1f %.1f %.1f %ld %ld %ld %.1f %ld %ld\n", t - inizio_, luogo, scena, missione_s, ms(t - prec_),
                     ms(riga.eventi), ms(riga.passi), ms(riga.passi_cpu), ms(riga.passo_max), riga.passi_n, ms(riga.disegno), ms(riga.cuffia), ms(riga.produci),
                     ms(riga.presenta), ms(riga.present), riga.coda_min, riga.accodati, riga.buco_cuffia, riga.mancanti, ms(riga.sonar), ms(riga.sonar_scena), ms(riga.sonar_spettri), riga.cuffia_attesa, riga.bordo_attesa, riga.buco_bordo, ms(riga.filo), riga.film_anticipo, riga.film_silenzio);
        prec_ = t;
        riga = Riga{};
    }

    static inline thread_local RigaTempi riga;   // una per filo: il film si prepara su un filo suo

private:
    explicit RegistroTempi(std::FILE* f) : f_(f), inizio_(ora_s()) {
        // le righe restano in memoria: un fflush a fotogramma col disco occupato fermava il gioco per un secondo. Vanno su file
        // con le note, a buffer pieno e all'uscita
        static char buffer[1 << 22];
        std::setvbuf(f_, buffer, _IOFBF, sizeof buffer);
        char quando[32] = "";
        const std::time_t ora = std::time(nullptr);
        std::strftime(quando, sizeof quando, "%Y-%m-%d %H:%M:%S", std::localtime(&ora));
        std::fprintf(f_, "# avvio %s\n", quando);
        std::fprintf(f_, "# t_s luogo scena missione_s fotogramma_ms eventi_ms passi_ms passi_cpu_ms passo_max_ms passi_n disegno_ms cuffia_ms produci_ms presenta_ms present_ms coda_min accodati buco_cuffia mancanti sonar_ms sonar_scena_ms sonar_spettri_ms cuffia_attesa bordo_attesa buco_bordo filo_ms film_anticipo film_silenzio\n");
    }
    static RegistroTempi* apri() {
        const char* p = std::getenv("MELCHIOR_REGISTRO_TEMPI");
        if (!p || !*p) return nullptr;
        std::FILE* f = std::fopen(p, "a");   // in aggiunta: un secondo lancio non cancella il primo
        if (!f) {
            std::fprintf(stderr, "registro dei tempi: non posso scrivere %s\n", p);
            return nullptr;
        }
        return new RegistroTempi(f);   // vive fino all'uscita, che scarica il buffer
    }
    std::FILE* f_;
    double inizio_;
    static inline thread_local double prec_ = -1.0;
};

// Il tempo fino a ferma() o alla fine del blocco, sommato a un campo della riga
class Cronometro {
public:
    explicit Cronometro(double RegistroTempi::Riga::*campo) : r_(RegistroTempi::attivo()), campo_(campo), da_(r_ ? RegistroTempi::ora_s() : 0.0) {}
    ~Cronometro() { ferma(); }
    Cronometro(const Cronometro&) = delete;
    Cronometro& operator=(const Cronometro&) = delete;
    void ferma() {
        if (!r_) return;
        r_->riga.*campo_ += RegistroTempi::ora_s() - da_;
        r_ = nullptr;
    }

private:
    RegistroTempi* r_;
    double RegistroTempi::Riga::*campo_;
    double da_;
};

// Un passo di partita: tempo vero, tempo di calcolo del thread, il passo piu' lungo
class CronometroPasso {
public:
    CronometroPasso() : r_(RegistroTempi::attivo()), da_(r_ ? RegistroTempi::ora_s() : 0.0), cpu_(r_ ? RegistroTempi::cpu_s() : 0.0) {}
    ~CronometroPasso() {
        if (!r_) return;
        const double s = RegistroTempi::ora_s() - da_;
        r_->riga.passi += s;
        r_->riga.passi_cpu += RegistroTempi::cpu_s() - cpu_;
        r_->riga.passo_max = std::max(r_->riga.passo_max, s);
        ++r_->riga.passi_n;
    }
    CronometroPasso(const CronometroPasso&) = delete;
    CronometroPasso& operator=(const CronometroPasso&) = delete;

private:
    RegistroTempi* r_;
    double da_, cpu_;
};

}  // namespace m688
