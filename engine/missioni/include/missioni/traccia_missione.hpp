#pragma once
// Il fotogramma della traccia del replay (nucleo/traccia_replay.hpp) da una missione in corso: la verita' di tutte le
// unita', i contatti del giocatore con la classe che ha dato (dal registro M1), le linee di rilevamento e il bersaglio
// della TMA (StazioniMissione). Il sonar, la stazione e la soluzione proposta li passa l'app in DatiApp. Legge
// soltanto: la missione non cambia.
//
// Uso nell'app, dopo ogni passo:
//   if (reg.serve(m.tempo(), m.eventi_missione().size()))
//       reg.aggiungi(fotogramma_da(m, &stazioni, dati_app, reg.ultimo_evento(), &raccolta), m.eventi_missione().size());

#include "missioni/missione.hpp"
#include "missioni/stazioni_missione.hpp"
#include "nucleo/traccia_replay.hpp"

namespace m688 {

// I fatti dell'IA che il registro M1 non ha, per le statistiche della sandbox (record I della traccia v2, nota
// condivisa sandbox-statistiche-contratto.md):
// - con Missione::registra_statistiche (R1-R6 del runtime): le scoperte al passo esatto, il primo istante possibile, i
//   lanci con la stima, la stima della TMA alla soluzione, le evasioni con la minaccia, le emissioni, i punti nave
//   dell'IA;
// - senza: i contatti di tutte le unita' dai cambi delle loro liste (nuovo, traccia solida, perso dopo
//   ASSENZA_PERSO_S d'assenza: un contatto che va e torna prima non fa righe);
// - sempre: gli eventi dei siluri del modello, e le righe dei contatti di ogni unita' (record K, la lettura «unita'»
//   della sandbox): ai cambi di stato subito, alle variazioni di posizione e rilevamento al piu' ogni RINNOVO_K_S.
// Solo con l'acustica dell'Enhanced; le armi non osservano (i loro agganci sono negli eventi dei siluri).
// Legge soltanto: basta chiamarla a ogni fotogramma, perche' le liste hanno il loro tempo.
class RaccoltaTraccia {
public:
    static constexpr double ASSENZA_PERSO_S = 30.0;
    static constexpr double SOLIDO = 800.0;   // la traccia solida dell'originale (qualita')
    static constexpr double RINNOVO_K_S = 15.0, SPOSTA_K_M = 100.0, RUOTA_K_GRADI = 1.0, QUALITA_K = 100.0;
    std::vector<EventoIa> nuovi(const Missione& m);
    // le righe K nuove (dopo nuovi(), nello stesso fotogramma)
    std::vector<ContattoUnita> contatti(const Missione& m);
    // i quadri di parte cambiati (Missione::quadro_parte), guardati ogni PASSO_P_S
    static constexpr double PASSO_P_S = 5.0, SPOSTA_P_M = 200.0;
    std::vector<QuadroParte> quadri(const Missione& m);
    // gli interventi del vetrino di Dio (record X): le fasi nuove dall'ultimo fotogramma, ciascuna col suo tempo
    std::vector<RecordIntervento> interventi(const Missione& m);

private:
    struct Visto {
        bool solido = false;
        double sparito = -1.0;   // >= 0: da quando non c'e' piu'
    };
    std::map<std::pair<int, int>, Visto> contatti_;   // (osservatore, bersaglio): per gli eventi senza R1
    std::map<std::pair<int, int>, ContattoUnita> scritte_;   // l'ultima riga K di ogni coppia
    std::map<std::pair<int, int>, double> assenti_;          // da quando una coppia scritta non c'e' piu'
    std::map<std::pair<int, int>, std::string> credute_;     // la classe detta (registro)
    std::size_t siluri_visti_ = 0, r1_ = 0, r2_ = 0, r3_ = 0, tma_ = 0, r4_ = 0, r5_ = 0, r6_ = 0, registro_ = 0;
    std::vector<std::size_t> radar_aperti_;          // emissioni radar copiate ancora accese (durata -1)
    std::map<int, QuadroParte> quadri_scritti_;      // l'ultimo quadro scritto per lato
    double prossimo_p_ = 0.0;
    std::map<int, int> fasi_interventi_;             // id dell'intervento -> l'ultima fase scritta
};

// `dopo_evento`: copia solo gli eventi del registro con id maggiore (RegistratoreTraccia::ultimo_evento(), per non
// ricopiarli tutti a ogni fotogramma); con il valore predefinito li copia tutti e il registratore scrive i nuovi.
// `raccolta`: i fatti dell'IA nuovi in Fotogramma::eventi_ia (nullptr: nessuno, come la traccia v1)
Fotogramma fotogramma_da(const Missione& m, const StazioniMissione* st = nullptr, const DatiApp& app = {},
                         int dopo_evento = -1, RaccoltaTraccia* raccolta = nullptr);

// I conti del giocatore che la traccia non campiona (Missione::ContiGiocatore, il soccorso, gli urti col ghiaccio, gli avvisi
// di chiglia, le luci, chi ci ha classificati), come metadati «conti.<nome>» per le statistiche (nucleo/statistiche,
// report_missione). Si chiama una volta, prima di salvare la traccia
void metadati_conti(const Missione& m, RegistratoreTraccia& r);

}  // namespace m688
