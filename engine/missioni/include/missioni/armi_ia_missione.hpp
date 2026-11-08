#pragma once
// Stato interno delle armi dell'IA dell'Enhanced (fase 4 B delle interazioni fra IA, docs/18_interazioni_ia.md; nota
// condivisa ia-fase4-contratto.md): carico e tubi dei sottomarini dell'IA, manovre di TMA, guide a filo. Lo usano
// soltanto missione.cpp e missione_armi_ia.cpp; l'interfaccia pubblica e' in missione.hpp (armi_rimaste, ...).

#include <map>
#include <vector>

#include "missioni/missione.hpp"
#include "nucleo/armi_ia.hpp"
#include "nucleo/carico.hpp"
#include "nucleo/manovre_tma.hpp"

namespace m688 {

struct Missione::StatoArmiIa {
    CatalogoCarico cat;
    CatalogoInterazioni ia;
    bool originale = false;   // una delle missioni originali: i loro script tengono i loro tempi (niente TMA a gambe)
    struct Battello {
        CaricoIa carico;
        std::map<int, double> ricarica;   // tubo vuoto -> quando e' di nuovo pronto
        std::map<int, std::string> ultima;   // tubo -> la variante che c'era (si ricarica la stessa, se resta)
    };
    std::map<int, Battello> battelli;
    std::set<int> considerati;
    struct Tma {
        ManovreTma m;
        Vec2 verso;
        double velocita_kn = 0.0;
        int gambe = 0;
        bool attiva = false;           // la gamba comanda il battello in questo istante (non se lo script ha ordinato)
        double velocita_prima = -1.0;  // quella dello script, rimessa alla fine
    };
    std::map<std::pair<int, int>, Tma> tma;   // (sottomarino, bersaglio)
    double prossima_tma = 0.0;
    std::map<int, GuidaFiloIa> filo;          // siluro -> la sua guida
    std::map<int, VincoliFilo> vincoli;       // lanciatore -> i vincoli del filo sulla sua manovra, rifatti a ogni passo
    std::size_t eventi_siluri_visti = 0;
    std::vector<EventoTmaIa> eventi_tma;
    struct AttaccoForzato { int bersaglio = -1; bool senza_soluzione = false; };
    std::map<int, AttaccoForzato> forzati;    // m688.attacca_unita: sottomarino -> l'attacco che aspetta il lancio
};

}  // namespace m688
