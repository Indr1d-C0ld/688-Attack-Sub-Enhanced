#include "nucleo/sonarista.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <set>

namespace m688 {

namespace {

constexpr double ISTERESI_DB = 3.0;      // una traccia si tiene fino a 3 dB sotto la soglia di scoperta
constexpr double INCROCIO_DEG = 2.0;     // due tracce a meno di 2 gradi si incrociano
constexpr double SEPARATE_DEG = 5.0;     // e l'incrocio finisce quando si separano oltre 5 gradi
constexpr double UDITO_ANALISI = 1.5;     // passo B: la classificazione a orecchio dopo 1,5 volte il tempo d'analisi (STIMA)
constexpr double UDITO_ERRORE = 0.5;      // e sbaglia la categoria con meta' dell'errore di classe del profilo (STIMA)
constexpr int UDITO_PALE_SOTTOMARINO = 6;
constexpr double UDITO_ASSE_SOTTOMARINO_HZ = 2.6, UDITO_ASSE_VELOCE_HZ = 1.8, UDITO_ASSE_MERCANTILE_HZ = 1.5;
// la rivalutazione (il collaudo della libreria, 30/10/2026: un Alfa detto NONSUB a 76 s restava NONSUB per sempre, finche'
// la traccia non si perdeva): ogni RIVALUTA_S una traccia classificata che ha righe viste si riguarda col classificatore,
// senza lo sbaglio voluto (la seconda ipotesi: lo sbaglio e' del primo colpo d'occhio), e si accetta solo in meglio
constexpr double RIVALUTA_S = 300.0;      // STIMA
constexpr double SCAFO_RECENTE_S = 60.0;   // STIMA: un transitorio di scafo vale per il contatto nuovo che nasce entro tanto
// il voto della natura senza classe (una riga sola concorde): decide solo se le ipotesi dei battelli pesano almeno tanto
// (STIMA). Le armoniche di rete (50, 60 Hz e multipli) stanno su venti navi e dieci sottomarini con pesi di pochi
// centesimi: 250 Hz votava «nave» con mk46 0,08, agi 0,04, akula 0,04, e un Victor III vero a 3,7 km diventava NONSUB
// (nota del runtime, 03/11/2026). Sotto la soglia decide il DEMON a orecchio, e senza DEMON resta POSSUB (§2.9)
constexpr double VOTO_MINIMO = 0.3;

bool di_sottomarino(const CatalogoAcustico* c, const std::string& classe) {
    const auto voce = c ? voce_libreria(*c, classe) : std::nullopt;
    return voce && (voce->tipo_unita == "ssn" || voce->tipo_unita == "ssgn" || voce->tipo_unita == "ssbn" || voce->tipo_unita == "ssk");
}

// una voce che e' un battello, nave o sottomarino: le sole che votano la natura di un contatto senza classe. Un siluro,
// un'esca, un pesce, il ghiaccio o un vulcano non dicono se il contatto e' una nave
bool di_battello(const CatalogoAcustico* c, const std::string& classe) {
    const auto voce = c ? voce_libreria(*c, classe) : std::nullopt;
    if (!voce) return false;
    const std::string& t = voce->tipo_unita;
    return t != "siluro" && t != "esca" && t != "biologico" && t != "pesce" && t != "ghiaccio" && t != "vulcano";
}

// meglio di prima: una classe dove non c'era, o una confidenza piu' alta (NONSUB, POSSUB, PROBSUB, CERTSUB); mai una classe
// tolta ne' un passo indietro
bool migliore(const std::string& classe, Confidenza conf, const std::string& prima, Confidenza conf_prima) {
    if (!prima.empty() && classe.empty()) return false;
    if (prima.empty() && !classe.empty()) return static_cast<int>(conf) >= static_cast<int>(conf_prima) || conf_prima == Confidenza::nonsub;
    return static_cast<int>(conf) > static_cast<int>(conf_prima);
}

double gauss(Casuale& caso) {
    const double u1 = std::max(caso.uniforme(), 1e-12), u2 = caso.uniforme();
    return std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * PI * u2);
}

std::string tre_cifre(double rilevamento) {
    char b[8];
    std::snprintf(b, sizeof b, "%03ld", std::lround(normalizza_360(rilevamento)) % 360);
    return b;
}

std::string confidenza_it(Confidenza c) {
    switch (c) {
        case Confidenza::nonsub: return "non sottomarino";
        case Confidenza::possub: return "possibile sottomarino";
        case Confidenza::probsub: return "probabile sottomarino";
        case Confidenza::certsub: return "sottomarino certo";
    }
    return "";
}
std::string confidenza_en(Confidenza c) {
    switch (c) {
        case Confidenza::nonsub: return "non-submarine";
        case Confidenza::possub: return "possible submarine";
        case Confidenza::probsub: return "probable submarine";
        case Confidenza::certsub: return "certain submarine";
    }
    return "";
}

}  // namespace

const char* nome_esperienza(Esperienza e) {
    switch (e) {
        case Esperienza::recluta: return "recluta";
        case Esperienza::medio: return "medio";
        case Esperienza::esperto: return "esperto";
    }
    return "?";
}

std::optional<Esperienza> esperienza_da_nome(std::string_view s) {
    if (s == "recluta") return Esperienza::recluta;
    if (s == "medio") return Esperienza::medio;
    if (s == "esperto") return Esperienza::esperto;
    return std::nullopt;
}

Esperienza esperienza_ia(int avversario, std::string_view sovrascrittura) {
    if (const auto e = esperienza_da_nome(sovrascrittura)) return *e;
    return avversario <= 0 ? Esperienza::recluta : avversario == 1 ? Esperienza::medio : Esperienza::esperto;
}

// splitmix64 del seme della missione con l'unita': semi lontani anche per unita' vicine
std::uint64_t seme_sonarista_ia(std::uint64_t seme_missione, int unita) {
    std::uint64_t z = seme_missione ^ 0x50AA50AAull ^ (static_cast<std::uint64_t>(static_cast<std::uint32_t>(unita)) * 0x9E3779B97F4A7C15ull);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
    return z ^ (z >> 31);
}

ClassificazioneUdito classifica_a_orecchio(const std::vector<LetturaDemon>& serie) {
    ClassificazioneUdito out;
    if (serie.empty() || serie.front().asse_hz <= 0.0) return out;
    const LetturaDemon& d = serie.front();   // la serie piu' forte
    out.valida = true;
    out.sottomarino = d.pale >= UDITO_PALE_SOTTOMARINO + 1 || (d.pale >= UDITO_PALE_SOTTOMARINO && d.asse_hz >= UDITO_ASSE_VELOCE_HZ) ||
                      d.asse_hz > UDITO_ASSE_SOTTOMARINO_HZ;
    if (!out.sottomarino) {
        if (d.asse_hz >= UDITO_ASSE_VELOCE_HZ) out.tipo_it = "nave da guerra veloce", out.tipo_en = "fast warship";
        else if (d.asse_hz < UDITO_ASSE_MERCANTILE_HZ) out.tipo_it = "mercantile", out.tipo_en = "merchant";
    }
    return out;
}

bool riga_propria_tolta(double hz, std::uint64_t seme, double maschera) {
    std::uint64_t z = (static_cast<std::uint64_t>(std::llround(hz * 10.0)) ^ seme) * 0x9E3779B97F4A7C15ull;
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ull;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebull;
    return static_cast<double>((z ^ (z >> 31)) >> 11) * 0x1.0p-53 < maschera;
}

// STIMA DI GIOCO (dossier 09 §3.9): la scoperta e il passo 2 (30-45 s, §2.9) dell'operatore medio, piu' lenti e
// piu' sbagliati per la recluta, piu' rapidi per l'esperto
ProfiloSonarista profilo_sonarista(Esperienza e) {
    ProfiloSonarista p;
    switch (e) {
        case Esperienza::recluta:
            p = {3.0, 60.0, 90.0, 0.30, 0.25, 2.0, 60.0, 0.40, 6.0, 0.6, 0.5, 0.3};
            break;
        case Esperienza::medio:
            p = {1.0, 30.0, 45.0, 0.15, 0.10, 1.0, 120.0, 0.20, 3.0, 1.0, 0.0, 0.6};
            break;
        case Esperienza::esperto:
            p = {0.0, 15.0, 30.0, 0.05, 0.03, 0.5, 180.0, 0.05, 1.5, 1.0, 0.0, 0.9};
            break;
    }
    return p;
}

const TracciaSonar* Sonarista::traccia_di(int sorgente) const {
    for (const auto& x : tracce_)
        if (x.sorgente == sorgente) return &x;
    return nullptr;
}

std::vector<RapportoSonar> Sonarista::passo(double t, const std::vector<AscoltoSonar>& ascolti, Casuale& caso) {
    std::vector<RapportoSonar> out;
    if (!seme_preso_) {   // da una copia: la sequenza del generatore resta quella di prima
        Casuale copia = caso;
        seme_maschera_ = copia.prossimo();
        seme_preso_ = true;
    }
    const auto rapporto = [&](RapportoSonar::Tipo tipo, const TracciaSonar& x, std::string it, std::string en) {
        out.push_back({tipo, x.numero, t, "Comandante, sonar: " + std::move(it), "Conn, sonar: " + std::move(en)});
    };
    std::map<int, const AscoltoSonar*> per_sorgente;
    for (const auto& a : ascolti) {
        per_sorgente[a.sorgente] = &a;
        if (a.transitorio_scafo) scafo_sentito_[a.sorgente] = t;
    }
    // le righe comuni: la stessa frequenza (entro 0,05 Hz) vista sopra la soglia su due o piu' sorgenti in questo istante
    std::vector<double> comuni;
    if (profilo_.riconosce_comuni > 0.0 && ascolti.size() > 1) {
        std::map<long, std::set<int>> chi;   // per decimo di Hz: le sorgenti che la vedono
        for (const auto& a : ascolti)
            for (const auto& rg : a.righe)
                if (rg.snr_db >= profilo_.soglia_righe_db) chi[std::lround(rg.hz * 10.0)].insert(a.sorgente);
        for (const auto& [k, s] : chi) {
            std::set<int> vicine = s;   // il decimo accanto: la stessa riga a cavallo di due decimi
            if (const auto d = chi.find(k + 1); d != chi.end()) vicine.insert(d->second.begin(), d->second.end());
            const double hz = static_cast<double>(k) / 10.0;
            if (vicine.size() >= 2 && riga_propria_tolta(hz, seme_maschera_ ^ 0xC0A1u, profilo_.riconosce_comuni)) comuni.push_back(hz);
        }
    }
    // un transitorio dello scafo su una traccia che c'e' (31/10/2026): un colpo metallico vuol dire sottomarino, PROBSUB
    // subito, senza aspettare l'analisi (la classe resta quella che c'era)
    for (auto& x : tracce_) {
        const auto it = per_sorgente.find(x.sorgente);
        if (it == per_sorgente.end() || !it->second->transitorio_scafo || x.stato == TracciaSonar::Stato::persa) continue;
        if (static_cast<int>(x.confidenza) >= static_cast<int>(Confidenza::probsub)) continue;
        x.confidenza = Confidenza::probsub;
        x.tipo.clear();
        x.errata = !it->second->sottomarino;
        x.stato = TracciaSonar::Stato::classificata;
        x.t_classificata = t;
        rapporto(RapportoSonar::Tipo::classificato, x,
                 "transitorio metallico su Sierra " + std::to_string(x.numero) + ", classificato " + confidenza_it(x.confidenza) + ".",
                 "metallic transient on Sierra " + std::to_string(x.numero) + ", classified " + confidenza_en(x.confidenza) + ".");
    }
    // il falso contatto (D3): l'eco d'esca di un ping lo smaschera subito; altrimenti, a ogni classificazione o
    // rivalutazione, con P = (1 - fedelta') x smaschera_falsi la firma non torna (anche senza classificazione: il falso
    // scadente, sotto FEDELTA_SENZA_PALE, non ha pale ne' abbastanza righe e resterebbe in analisi per sempre).
    // Smascherato: NONSUB «falso contatto»
    const auto smaschera = [&](TracciaSonar& x, const char* perche_it, const char* perche_en) {
        x.classe.clear();
        x.confidenza = Confidenza::nonsub;
        x.tipo = "falso contatto";
        x.errata = false;
        x.stato = TracciaSonar::Stato::classificata;
        x.t_classificata = t;
        rapporto(RapportoSonar::Tipo::classificato, x, "Sierra " + std::to_string(x.numero) + ": " + perche_it + ", probabile falso contatto.",
                 "Sierra " + std::to_string(x.numero) + ": " + perche_en + ", probable false contact.");
    };
    for (auto& x : tracce_) {
        const auto it = per_sorgente.find(x.sorgente);
        if (it == per_sorgente.end() || !it->second->eco_esca || x.stato == TracciaSonar::Stato::persa || x.tipo == "falso contatto") continue;
        // ogni contatto, anche creduto NONSUB senza classe: l'eco puntiforme e' troppo piccola anche per una nave
        // (03/11/2026: saltandoli, il falso Victor preso per nave non si smascherava col ping)
        smaschera(x, "eco troppo piccola per la classe", "echo too small for the class");
    }
    // tracce esistenti: aggiornate, perse, ritrovate
    for (auto& x : tracce_) {
        const auto it = per_sorgente.find(x.sorgente);
        const bool sentita = it != per_sorgente.end() && it->second->se_db >= profilo_.soglia_db - ISTERESI_DB;
        if (sentita) {
            const AscoltoSonar& a = *it->second;
            x.rilevamento = normalizza_360(a.rilevamento + std::hypot(a.errore_deg, profilo_.sigma_extra_deg) * gauss(caso));
            x.se_db = a.se_db;
            x.t_visto = t;
            if (x.stato == TracciaSonar::Stato::persa) {
                x.stato = x.classe.empty() && x.confidenza == Confidenza::possub ? TracciaSonar::Stato::in_analisi
                                                                                  : TracciaSonar::Stato::classificata;
                rapporto(RapportoSonar::Tipo::ritrovato, x, "riacquisito Sierra " + std::to_string(x.numero) + ", rilevamento " +
                         tre_cifre(x.rilevamento) + ".", "regained Sierra " + std::to_string(x.numero) + ", bearing " +
                         tre_cifre(x.rilevamento) + ".");
            }
            // classificazione dopo il tempo d'analisi, con le righe che l'operatore vede (la soglia della sua esperienza);
            // senza nessuna riga non si classifica, si aspetta che ne arrivino (prima diventava POSSUB per difetto, anche
            // per le navi vicine)
            std::vector<RigaOsservata> viste;
            for (const auto& rg : a.righe) if (rg.snr_db >= profilo_.soglia_righe_db) viste.push_back(rg);
            // le righe proprie che l'operatore toglie col catalogo: per ogni riga la stessa scelta a ogni passo (dalla sua
            // frequenza e dal seme della partita), la recluta ne lascia circa il 40%
            std::vector<double> maschera;
            for (double hz : a.proprie)
                if (riga_propria_tolta(hz, seme_maschera_, profilo_.maschera_proprie)) maschera.push_back(hz);
            togli_righe(viste, maschera, 0.05);
            togli_righe(viste, comuni, 0.1);
            // il classificatore sulle righe viste: la classe e la confidenza (con una riga sola niente classe, ma la natura
            // che le ipotesi migliori votano, pesate con la loro confidenza: la rete a 300 Hz sta sia sulle navi sia sui
            // sottomarini USA, una nave resta NONSUB se le navi pesano di piu')
            const auto classifica = [&](Classificazione& k, std::string& classe, Confidenza& conf) {
                if (catalogo_) k = classifica_righe(viste, a.pale, *catalogo_);
                classe = k.classe;
                conf = k.confidenza;
                if (classe.empty() && !k.ipotesi.empty() && !k.ipotesi.front().concordi.empty() && catalogo_) {
                    double sub = 0.0, tot = 0.0;
                    for (const auto& h : k.ipotesi) {
                        if (h.concordi.empty() || !di_battello(catalogo_, h.id)) continue;
                        sub += di_sottomarino(catalogo_, h.id) ? h.confidenza : 0.0;
                        tot += h.confidenza;
                    }
                    if (tot >= VOTO_MINIMO) conf = sub < 0.5 * tot ? Confidenza::nonsub : Confidenza::possub;
                    else if (const ClassificazioneUdito u = classifica_a_orecchio(a.demon); u.valida)
                        conf = u.sottomarino ? Confidenza::possub : Confidenza::nonsub;
                    else conf = Confidenza::possub;
                }
            };
            const auto nome_classe = [&](const std::string& classe, bool en) -> std::string {
                if (classe.empty() || !catalogo_) return "";
                const auto v = voce_libreria(*catalogo_, classe);
                return v ? (en ? ", class " : ", classe ") + v->nome : "";
            };
            if (x.stato == TracciaSonar::Stato::in_analisi && t - x.t_primo >= profilo_.ritardo_s + profilo_.analisi_s &&
                (!viste.empty() || a.biologico)) {
                Classificazione k;
                std::string classe;
                Confidenza conf = Confidenza::possub;
                classifica(k, classe, conf);
                bool errata = false;
                if (k.ipotesi.size() > 1 && !classe.empty() && caso.uniforme() < profilo_.errore_classe) {
                    // la seconda ipotesi: classe sbagliata, con la sua natura
                    classe = k.ipotesi[1].id;
                    conf = di_sottomarino(catalogo_, classe) ? Confidenza::possub : Confidenza::nonsub;
                    errata = true;
                }
                if (a.biologico && (conf == Confidenza::nonsub || classe.empty()) && caso.uniforme() < profilo_.biologico_come_sub) {
                    classe.clear();
                    conf = Confidenza::possub;   // il canto preso per una tonale (§3.9)
                    errata = true;
                }
                x.classe = classe;
                x.confidenza = conf;
                if (!a.classe_vera.empty())   // dalla verita': la classe detta, o senza classe la categoria
                    errata = !classe.empty() ? classe != a.classe_vera : (conf == Confidenza::nonsub ? a.sottomarino : !a.sottomarino);
                if (a.falso) errata = true;   // il falso contatto preso per vero
                x.errata = errata;
                x.stato = TracciaSonar::Stato::classificata;
                x.t_classificata = t;
                if (a.falso && caso.uniforme() < (1.0 - std::clamp(a.fedelta, 0.0, 1.0)) * profilo_.smaschera_falsi)
                    smaschera(x, "firma incoerente", "inconsistent signature");
                else
                    rapporto(RapportoSonar::Tipo::classificato, x,
                             "Sierra " + std::to_string(x.numero) + " classificato " + confidenza_it(conf) + nome_classe(classe, false) + ".",
                             "Sierra " + std::to_string(x.numero) + " classified " + confidenza_en(conf) + nome_classe(classe, true) + ".");
            }
            // il falso contatto preso per vero: a ogni rivalutazione si riprova a smascherarlo (anche senza righe nuove)
            else if (a.falso && x.stato == TracciaSonar::Stato::classificata && x.tipo != "falso contatto" &&
                     t - x.t_classificata >= RIVALUTA_S) {
                x.t_classificata = t;
                if (caso.uniforme() < (1.0 - std::clamp(a.fedelta, 0.0, 1.0)) * profilo_.smaschera_falsi)
                    smaschera(x, "firma incoerente", "inconsistent signature");
            }
            // la rivalutazione: ogni RIVALUTA_S, con righe viste, solo in meglio (niente caso: la sequenza non cambia)
            else if (x.stato == TracciaSonar::Stato::classificata && !viste.empty() && !a.biologico &&
                     t - x.t_classificata >= RIVALUTA_S) {
                x.t_classificata = t;
                Classificazione k;
                std::string classe;
                Confidenza conf = Confidenza::possub;
                classifica(k, classe, conf);
                if (migliore(classe, conf, x.classe, x.confidenza)) {
                    x.classe = classe;
                    x.confidenza = conf;
                    x.tipo.clear();
                    x.errata = !a.classe_vera.empty() ? (!classe.empty() ? classe != a.classe_vera
                                                                         : (conf == Confidenza::nonsub ? a.sottomarino : !a.sottomarino))
                                                      : false;
                    rapporto(RapportoSonar::Tipo::classificato, x,
                             "Sierra " + std::to_string(x.numero) + " riclassificato " + confidenza_it(conf) + nome_classe(classe, false) + ".",
                             "Sierra " + std::to_string(x.numero) + " reclassified " + confidenza_en(conf) + nome_classe(classe, true) + ".");
                }
            }
            // passo B: nessuna riga utile anche dopo un'analisi piu' lunga, ma il DEMON si legge: la categoria a orecchio
            else if (x.stato == TracciaSonar::Stato::in_analisi && viste.empty() && !a.demon.empty() && !a.biologico &&
                     t - x.t_primo >= profilo_.ritardo_s + UDITO_ANALISI * profilo_.analisi_s) {
                if (const ClassificazioneUdito u = classifica_a_orecchio(a.demon); u.valida) {
                    bool sub = u.sottomarino;
                    if (caso.uniforme() < UDITO_ERRORE * profilo_.errore_classe) sub = !sub;   // la categoria sbagliata
                    x.classe.clear();
                    x.confidenza = sub ? Confidenza::possub : Confidenza::nonsub;
                    x.tipo = sub ? std::string() : u.tipo_it;
                    x.errata = sub != a.sottomarino;
                    x.stato = TracciaSonar::Stato::classificata;
                    x.t_classificata = t;
                    const std::string ti = !sub && !u.tipo_it.empty() ? ", " + u.tipo_it : "", te = !sub && !u.tipo_en.empty() ? ", " + u.tipo_en : "";
                    rapporto(RapportoSonar::Tipo::classificato, x,
                             "Sierra " + std::to_string(x.numero) + " classificato " + confidenza_it(x.confidenza) + " a orecchio" + ti + ".",
                             "Sierra " + std::to_string(x.numero) + " classified " + confidenza_en(x.confidenza) + " by ear" + te + ".");
                }
            }
            // il falso contatto che non si classifica (ne' righe utili ne' DEMON: la fedelta' sotto FEDELTA_SENZA_PALE): dopo
            // il tempo del passo B, e poi ogni RIVALUTA_S, la stessa prova della firma incoerente. Prima del 03/11/2026 un
            // falso a fedelta' 0,7 o meno restava in analisi per 55 minuti senza mai essere smascherato, piu' sicuro di uno
            // buono (nota del runtime, §4.3). Solo i falsi: la sequenza del caso delle partite senza falsi non cambia
            else if (a.falso && x.stato == TracciaSonar::Stato::in_analisi && viste.empty() && a.demon.empty() && !a.biologico &&
                     t - x.t_primo >= profilo_.ritardo_s + UDITO_ANALISI * profilo_.analisi_s &&
                     (x.t_classificata <= x.t_primo || t - x.t_classificata >= RIVALUTA_S)) {
                x.t_classificata = t;
                if (caso.uniforme() < (1.0 - std::clamp(a.fedelta, 0.0, 1.0)) * profilo_.smaschera_falsi)
                    smaschera(x, "firma incoerente", "inconsistent signature");
            }
        } else if (x.stato != TracciaSonar::Stato::persa && t - x.t_visto > profilo_.tenuta_s) {
            x.stato = TracciaSonar::Stato::persa;
            rapporto(RapportoSonar::Tipo::perso, x, "perso il contatto con Sierra " + std::to_string(x.numero) +
                     ", ultimo rilevamento " + tre_cifre(x.rilevamento) + ".", "lost contact Sierra " + std::to_string(x.numero) +
                     ", last bearing " + tre_cifre(x.rilevamento) + ".");
        }
    }
    // sorgenti nuove: dall'attesa al rapporto
    for (const auto& a : ascolti) {
        if (traccia_di(a.sorgente)) continue;
        if (a.se_db < profilo_.soglia_db) {
            in_attesa_.erase(a.sorgente);
            continue;
        }
        const auto it = in_attesa_.emplace(a.sorgente, t).first;
        if (t - it->second < profilo_.ritardo_s) continue;
        TracciaSonar x;
        x.numero = prossimo_++;
        x.sorgente = a.sorgente;
        x.rilevamento = normalizza_360(a.rilevamento + std::hypot(a.errore_deg, profilo_.sigma_extra_deg) * gauss(caso));
        x.se_db = a.se_db;
        x.t_primo = it->second;
        x.t_visto = t;
        in_attesa_.erase(it);
        tracce_.push_back(x);
        rapporto(RapportoSonar::Tipo::nuovo, tracce_.back(), "nuovo contatto, rilevamento " + tre_cifre(x.rilevamento) +
                 ", designato Sierra " + std::to_string(x.numero) + ".", "new contact, bearing " + tre_cifre(x.rilevamento) +
                 ", designated Sierra " + std::to_string(x.numero) + ".");
        // un transitorio dello scafo sentito da poco su questa sorgente: POSSUB subito
        if (const auto s = scafo_sentito_.find(a.sorgente); s != scafo_sentito_.end() && t - s->second <= SCAFO_RECENTE_S) {
            TracciaSonar& n = tracce_.back();
            n.confidenza = Confidenza::possub;
            n.errata = !a.sottomarino;
            n.stato = TracciaSonar::Stato::classificata;
            n.t_classificata = t;
            rapporto(RapportoSonar::Tipo::classificato, n,
                     "Sierra " + std::to_string(n.numero) + ": transitorio metallico, " + confidenza_it(n.confidenza) + ".",
                     "Sierra " + std::to_string(n.numero) + ": metallic transient, " + confidenza_en(n.confidenza) + ".");
        }
    }
    // incroci: due tracce sentite a meno di INCROCIO_DEG; una volta per incrocio, a volte si scambiano le sorgenti
    for (std::size_t i = 0; i < tracce_.size(); ++i)
        for (std::size_t j = i + 1; j < tracce_.size(); ++j) {
            TracciaSonar& a = tracce_[i];
            TracciaSonar& b = tracce_[j];
            if (a.stato == TracciaSonar::Stato::persa || b.stato == TracciaSonar::Stato::persa) continue;
            const auto chiave = std::make_pair(a.numero, b.numero);
            // la geometria dell'incrocio: i rilevamenti veri delle sorgenti seguite (il tracker leviga il rumore)
            const auto pa = per_sorgente.find(a.sorgente), pb = per_sorgente.find(b.sorgente);
            if (pa == per_sorgente.end() || pb == per_sorgente.end()) continue;
            const double d = std::fabs(normalizza_180(pa->second->rilevamento - pb->second->rilevamento));
            bool& in_incrocio = vicine_[chiave];
            if (in_incrocio) {
                if (d > SEPARATE_DEG) in_incrocio = false;
                continue;
            }
            if (d >= INCROCIO_DEG) continue;
            in_incrocio = true;
            if (caso.uniforme() < profilo_.scambio) {
                std::swap(a.sorgente, b.sorgente);
                ++scambi_;
            }
        }
    return out;
}

}  // namespace m688
