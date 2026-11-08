#pragma once
// Stato interno delle collisioni dell'Enhanced nella missione (docs/ricerca/16_collisioni.md, P1, P3, P6): lo usano
// soltanto missione.cpp e missione_collisioni.cpp. L'interfaccia pubblica e' in missione.hpp (collisioni_enhanced,
// urti, quasi_collisioni, manovre_crazy_ivan).

#include <map>
#include <set>
#include <string>
#include <utility>

#include "missioni/missione.hpp"
#include "nucleo/casuale.hpp"
#include "nucleo/classi.hpp"

namespace m688 {

struct Missione::StatoCollisioniMissione {
    std::map<std::string, ClasseNave> classi;
    std::string cartella_dati;
    std::map<std::string, std::pair<double, double>> masse;   // classe -> dislocamento emerso e immerso (t)
    Casuale caso{0xC0111510u};
    struct Coppia {
        bool contatto = false;
        double ultimo_urto = -1e18;
        bool vicini = false;                 // dentro la soglia della quasi collisione
        double minimo = 1e18, t_minimo = 0.0;
        Vec2 pos_minimo;
        double quota_a = 0.0, quota_b = 0.0;
        bool urtati = false;                 // nel passaggio c'e' stato un urto: niente quasi collisione
    };
    std::map<std::pair<int, int>, Coppia> coppie;   // (minore, maggiore)
    std::map<int, double> quote_prec;                // per il rateo verticale vero
    struct Ivan {
        double prossimo = -1.0;              // s di missione della prossima manovra
        double fine_virata = -1.0, fine = -1.0;   // la virata, poi l'ascolto fino a `fine`
        double rotta = 0.0, velocita = 0.0, quota = 0.0;
        double rotta_vista = 0.0;            // per riconoscere un cambio di rotta
        bool vista = false;
        int fatti = 0;
    };
    std::map<int, Ivan> ivan;
    int prossimo_transitorio = 0;            // id 940000 + 25 n
    // cavi delle schiere trainate (S10): la scia della poppa di ogni nave che fila, la piu' recente in testa
    std::map<int, std::vector<Vec2>> scie;
    std::map<int, double> ultima_scia;
    std::set<std::pair<int, int>> impigliati, toccati;   // (sottomarino, nave)
};

}  // namespace m688
