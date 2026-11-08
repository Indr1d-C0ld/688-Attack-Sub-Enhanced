import sys
p='missioni/src/missione_forze.cpp'
s=open(p,encoding='utf-8').read()
def r(a,b):
    global s
    assert s.count(a)==1,a
    s=s.replace(a,b)
r("""        const auto fine_ascolto = [&, id = id, &u = u, &R = R](bool ripristina) {
            R.salita = false;
            if (R.antenna_nostra) u.flag54 &= ~4;
            R.antenna_nostra = false;
            if (!ripristina) return;
            u.quota_ordinata = R.quota_prima;
            if (const auto n = missili_ia_.find(id); n != missili_ia_.end() && n->second.in_manovra) n->second.quota_prima = R.quota_prima;
            velocita_di_prima(R.velocita_prima);
        };
""","""        const auto fine_ascolto = [&, id = id, &u = u] {
            const bool quota_sua = std::fabs(u.quota_ordinata - QUOTA_ORDINE_ANTENNA_M) > 1e-9;   // lo script gliene ha data un'altra
            const bool velocita_sua = std::fabs(u.velocita_ordinata - R.velocita_data) > 1e-9;
            R.salita = false;
            if (R.antenna_nostra) u.flag54 &= ~4;
            R.antenna_nostra = false;
            if (!quota_sua) u.quota_ordinata = R.quota_prima;
            if (const auto n = missili_ia_.find(id); n != missili_ia_.end() && n->second.in_manovra) n->second.quota_prima = R.quota_prima;
            if (!velocita_sua) velocita_di_prima(R.velocita_prima);
        };
""")
r("""            const bool altra_quota = std::fabs(u.quota_ordinata - QUOTA_ORDINE_ANTENNA_M) > 1e-9;   // lo script gliene ha data un'altra
            if (altra_quota || script_ora) {
                fine_ascolto(false);
            } else if (occupato || contatto_ostile(RIMANDA_M[livello])) {
                fine_ascolto(true);
            } else if (!esposto && tempo_ - R.salita_da > ASCOLTO_MAX_S) {
                fine_ascolto(true);
                R.ultimo_ascolto = tempo_;
""","""            if (script_ora || occupato || contatto_ostile(RIMANDA_M[livello]) || std::fabs(u.quota_ordinata - QUOTA_ORDINE_ANTENNA_M) > 1e-9 ||
                std::fabs(u.velocita_ordinata - R.velocita_data) > 1e-9) {
                fine_ascolto();
            } else if (!esposto && tempo_ - R.salita_da > ASCOLTO_MAX_S) {
                fine_ascolto();
                R.ultimo_ascolto = tempo_;
""")
r("""                    if (R.esposto_da >= 0.0 && R.ciclo - R.esposto_da >= COPIA_S) {
                        fine_ascolto(true);
""","""                    if (R.esposto_da >= 0.0 && R.ciclo - R.esposto_da >= COPIA_S) {
                        fine_ascolto();
""")
r("""            u.velocita_ordinata = std::min(std::fabs(u.velocita_ordinata), KN_A_QUOTA[livello]);
            reazioni_ia_""","""            u.velocita_ordinata = R.velocita_data = std::min(std::fabs(u.velocita_ordinata), KN_A_QUOTA[livello]);
            reazioni_ia_""")
r("""        const auto usabile = [&, id = id, &u = u, &R = R](int rb, const ContattoRadio& cr) {""","""        const auto usabile = [&, id = id, &u = u](int rb, const ContattoRadio& cr) {""")
r("""        const auto lascia = [&, id = id, &u = u, &R = R](const char* perche, bool ripristina) {
            reazioni_ia_.push_back({tempo_, id, R.caccia, u.categoria, "dato_radio_lasciato", perche});
            R.caccia = -1;
            if (!ripristina) return;
            velocita_di_prima(R.velocita_prima_caccia);
            if (!u.navigazione) u.rotta_ordinata = R.rotta_prima_caccia;
        };
""","""        const auto lascia = [&, id = id, &u = u](const char* perche, bool rotta) {
            reazioni_ia_.push_back({tempo_, id, R.caccia, u.categoria, "dato_radio_lasciato", perche});
            R.caccia = -1;
            velocita_di_prima(R.velocita_prima_caccia);
            if (rotta && !u.navigazione) u.rotta_ordinata = R.rotta_prima_caccia;
        };
""")
r("""            } else if (occupato || contatto_suo) {
                lascia("contatto", false);
                velocita_di_prima(R.velocita_prima_caccia);
            } else {""","""            } else if (occupato || contatto_suo) {
                lascia("contatto", false);
            } else {""")
r("""        if (R.caccia < 0 && !occupato && !contatto_suo && !R.salita && !script_ora) {""","""        if (R.caccia < 0 && !occupato && !contatto_suo && !R.salita && !script_ora && tempo_ >= R.caccia_non_prima) {""")
# lo script nell'istante chiude la caccia a ogni passo
r("""    auto& F = *forze_;
    if (tempo_ >= F.prossimo_giro) {   // i messaggi di contatto, una volta al secondo""","""    auto& F = *forze_;
    // l'ordine dello script chiude la caccia sul dato nello stesso passo: la sua velocita' e la sua rotta restano
    if (F.quadro_sottomarini)
        for (auto& [id, R] : F.radio_sottomarini) {
            if (R.caccia < 0) continue;
            const auto os = ordini_script_.find(id);
            if (os == ordini_script_.end() || os->second < tempo_) continue;
            if (const auto* u = trova(id)) reazioni_ia_.push_back({tempo_, id, R.caccia, u->categoria, "dato_radio_lasciato", "script"});
            R.caccia = -1;
            R.caccia_non_prima = tempo_ + CACCIA_DOPO_SCRIPT_S;
        }
    if (tempo_ >= F.prossimo_giro) {   // i messaggi di contatto, una volta al secondo""")
r("""constexpr double COPIA_S = 80.0;""","""constexpr double CACCIA_DOPO_SCRIPT_S = 60.0;   // il dato radio torna buono solo un minuto dopo un ordine dello script (STIMA)
constexpr double COPIA_S = 80.0;""")
open(p,'w',encoding='utf-8').write(s)
p='missioni/include/missioni/forze_missione.hpp'
s=open(p,encoding='utf-8').read()
r("""        double salita_da = 0.0, ciclo = 0.0, quota_prima = 0.0, velocita_prima = 0.0;
""","""        double salita_da = 0.0, ciclo = 0.0, quota_prima = 0.0, velocita_prima = 0.0, velocita_data = 0.0;
""")
r("""        double velocita_caccia = 0.0, velocita_prima_caccia = 0.0, rotta_prima_caccia = 0.0;
""","""        double velocita_caccia = 0.0, velocita_prima_caccia = 0.0, rotta_prima_caccia = 0.0, caccia_non_prima = 0.0;
""")
open(p,'w',encoding='utf-8').write(s)
