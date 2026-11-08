#!/usr/bin/env python3
# Copie private per le immagini e i conti (non e' il gioco): sonar_enhanced.cpp con CASCATA_A (la cascata di prima) e
# CASCATA_LOG (i tracker contro le unita' vere); missione.cpp con CASCATA_OLTRE (i raggi anche al traffico oltre i 30 km).
import sys
R = "/home/randolph/Claude/melchior-688/engine/"
out = sys.argv[1]
s = open(R + "app/sonar_enhanced.cpp").read()
a = "&grana_, &scena.campi));"
assert s.count(a) == 1
s = s.replace(a, '&grana_, std::getenv("CASCATA_A") ? nullptr : &scena.campi));')
a = "    aggiorna_tracker(b);\n"
assert s.count(a) == 1
s = s.replace(a, a + r'''    if (const char* fl = std::getenv("CASCATA_LOG")) {
        static double prossima = 0.0;
        if (tempo_ >= prossima) {
            prossima = tempo_ + 20.0;
            FILE* o = std::fopen(fl, "a");
            const bool vecchia = std::getenv("CASCATA_A") != nullptr;
            struct Una { int id; std::string classe; double km; std::vector<float> db; };
            std::vector<Una> une;
            for (std::size_t i = 0; i < scena.sorgenti.size(); ++i) {
                const Sorgente& so = scena.sorgenti[i];
                if (!so.firma && !so.fonte) continue;
                std::vector<Sorgente> sola{so};
                std::vector<const CampoPerdita*> campo{vecchia ? nullptr : scena.campi[i]};
                const auto u = m.unita().find(so.id);
                une.push_back({so.id, u != m.unita().end() ? u->second.classe : (so.firma ? so.firma->classe : so.fonte->id),
                               distanza(scena.ricevitore.pos, so.pos) / 1000.0,
                               cascata(sola, scena.ricevitore, scena.ambiente, 72, 0.0, nullptr, &campo).db});
            }
            int sopra3 = 0, sopra8 = 0, unita3 = 0;
            for (const Una& u : une) {
                const float x = *std::max_element(u.db.begin(), u.db.end());
                if (x >= 3.0f) { ++sopra3; if (m.unita().count(u.id)) ++unita3; }
                if (x >= 8.0f) ++sopra8;
            }
            int solidi = 0;
            for (const auto& [id, c] : g->contatti) if (c.qualita >= 1000.0) ++solidi;
            int attivi = 0;
            for (const auto& t : tracker_) if (!t.perso) ++attivi;
            int fasci3 = 0, fasci8 = 0, fasci = 0;
            for (std::size_t i = 0; i < b.db.size(); ++i) {
                if (i < b.cieco.size() && b.cieco[i]) continue;
                ++fasci; if (b.db[i] >= 3.0f) ++fasci3; if (b.db[i] >= 8.0f) ++fasci8;
            }
            std::fprintf(o, "R %.0f attivi %d nati %d sorgenti %zu sopra3 %d sopra8 %d unita3 %d contatti %zu solidi %d fasci %d fasci3 %d fasci8 %d\n", tempo_, attivi,
                         prossimo_numero_ - 1, une.size(), sopra3, sopra8, unita3, g->contatti.size(), solidi, fasci, fasci3, fasci8);
            for (std::size_t iu = 0; iu < une.size(); ++iu) {
                const Una& u = une[iu];
                if (!m.unita().count(u.id)) continue;
                const float x = *std::max_element(u.db.begin(), u.db.end());
                const auto co = g->contatti.find(u.id);
                if (x < 3.0f && co == g->contatti.end()) continue;
                const std::size_t k = static_cast<std::size_t>(std::max_element(u.db.begin(), u.db.end()) - u.db.begin());
                std::fprintf(o, "U %.0f %d %s km %.1f fascio %zu db_solo %.1f qualita %.0f\n", tempo_, u.id, u.classe.c_str(), u.km, k * 5, x, co != g->contatti.end() ? co->second.qualita : -1.0);
            }
            for (const auto& t : tracker_) {
                if (t.perso) continue;
                const int fa = static_cast<int>(std::lround(normalizza_360(t.rilevamento) / 5.0)) % 72;
                const Una* meglio = nullptr; double mx = -1.0;
                for (const Una& u : une)
                    for (int d = -1; d <= 1; ++d) {
                        const double x = u.db[static_cast<std::size_t>((fa + d + 72) % 72)];
                        if (x > mx) { mx = x; meglio = &u; }
                    }
                std::fprintf(o, "T %.0f S%d ril %.0f db %.1f auto %d mancati %d | %d %s km %.1f db_solo %.1f contatto %d\n", tempo_, t.numero, t.rilevamento, t.livello_db, t.automatico ? 1 : 0, t.mancati,
                             meglio ? meglio->id : -1, meglio ? meglio->classe.c_str() : "-", meglio ? meglio->km : 0.0, mx, meglio && g->contatti.count(meglio->id) ? 1 : 0);
            }
            std::fclose(o);
        }
    }
''')
open(out + "/sonar_enhanced_v.cpp", "w").write('#include <cstdio>\n#include <cstdlib>\n' + s)
s = open(R + "missioni/src/missione.cpp").read()
a = "sc.campi.push_back(col_campo ? raggi_.in_vigore(s.id) : nullptr);"
assert s.count(a) == 1
s = s.replace(a, '''static const bool oltre = std::getenv("CASCATA_OLTRE") != nullptr;
        if (oltre && b != unita_.end() && !col_campo) { sc.campi.push_back(const_cast<CacheCampiAsincrona&>(raggi_).campo(s.id, s.pos, s.quota_m, sc.ambiente, tempo_)); continue; }
        ''' + a)
open(out + "/missione_v.cpp", "w").write('#include <cstdlib>\n' + s)
