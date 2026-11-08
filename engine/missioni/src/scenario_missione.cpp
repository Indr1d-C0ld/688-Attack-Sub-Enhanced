#include "missioni/scenario_missione.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <limits>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>

#define TOML_EXCEPTIONS 1
#include <toml.hpp>

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

#include "nucleo/luci_navigazione.hpp"
#include "nucleo/battello.hpp"
#include "nucleo/classi.hpp"
#include "nucleo/fascicolo.hpp"
#include "nucleo/fondale.hpp"
#include "missioni/missione_rapida.hpp"
#include "missioni/registro_missione.hpp"
#include "lua_isolato.hpp"
#include "testo_lua.hpp"

namespace m688 {

namespace {

using testo_lua::esatto;
using testo_lua::num;
using testo_lua::stringa;

constexpr int IT = 0, EN = 1;
constexpr int USA = 0, URSS = 1;
const char* const PARTI[] = {"usa", "urss"};

// Variabili di scena: 1 siluri del giocatore (kit), 10 fase, 11 allarme, 100 + id salve, 200 + n regole scattate,
// 300 + i variabili dell'autore
constexpr int VAR_REGOLE = 200, VAR_AUTORE = 300;

std::string intero(int x) { return std::to_string(x); }

KitScenario leggi_kit(const std::string& percorso) {
    std::ifstream f(percorso, std::ios::binary);
    if (!f) throw std::runtime_error("impossibile aprire " + percorso);
    KitScenario k;
    std::string* sezione = nullptr;
    for (std::string r; std::getline(f, r);) {
        if (r == "--@ gestori_giocatore") sezione = &k.gestori;
        else if (r == "--@ armi") sezione = &k.armi;
        else if (r == "--@ script") sezione = &k.script;
        else if (sezione) *sezione += r + "\n";
    }
    if (k.gestori.empty() || k.armi.empty() || k.script.empty()) throw std::runtime_error(percorso + ": kit incompleto");
    return k;
}

TestiScenario testi_toml(const toml::node_view<const toml::node>& n) {
    return {n["it"].value_or(std::string{}), n["en"].value_or(std::string{})};
}

// Il gestore di lancio di un kit riscritto per l'IA: senza il conto dei siluri del giocatore (variabile 1) e senza il
// messaggio «siluro lanciato»
std::string lancio_ia(const KitScenario& k, const std::string& famiglia, const std::string& nome) {
    const auto riga = k.armi.find("    " + famiglia + " = {");
    if (riga == std::string::npos) throw std::runtime_error("kit senza l'arma " + famiglia);
    const auto g = k.armi.find("gestore = \"", riga);
    const auto fine_g = k.armi.find('"', g + 11);
    const std::string gestore = k.armi.substr(g + 11, fine_g - g - 11);
    const auto inizio = k.script.find("script." + gestore + " = function(u)\n");
    if (inizio == std::string::npos) throw std::runtime_error("kit senza lo script " + gestore);
    const auto fine = k.script.find("\nend\n", inizio);
    std::istringstream in(k.script.substr(inizio, fine - inizio + 5));
    std::string out;
    for (std::string r; std::getline(in, r);) {
        if (r.find("m688.variabile_piu(u, 1)") != std::string::npos || r.find("m688.messaggio_al_giocatore(u, 2)") != std::string::npos) continue;
        if (r == "script." + gestore + " = function(u)") r = "script." + nome + " = function(u)";
        if (r == "  ::" + gestore + "::") continue;
        out += r + "\n";
    }
    return out;
}

// --- comportamenti dell'IA -------------------------------------------------------------------------------------------

// Libreria 1: quella delle missioni rapide v1 (costanti della missione). Non si cambia: i codici gia' condivisi danno
// gli stessi file.
constexpr const char* COMPORTAMENTI_1 = R"(-- Comportamenti dell'IA della missione rapida (API classica, docs/09 §6). Variabili di scena: 1 siluri lanciati dal
-- giocatore (kit), 10 fase della missione (zona raggiunta), 11 allarme, 100 + id salve lanciate da un'unita'.
local function armi_libere(u)
  return ARMI == 2 or (ARMI == 1 and m688.variabile_maggiore(u, 1, 0))
end
local function contatto_buono(u)
  return m688.traccia_solida(u) or (MEDIE and m688.traccia_media(u))
end
local function riprendi_rotta(u)
  local f = script["R_" .. u .. "_p1"]
  if f ~= nil then f(u) end
end

-- nave da guerra: segue la sua rotta; su un contatto nemico accosta, fa ping e, se le regole lo consentono, attacca
-- con le armi antisom (ATTACK: riesce con probabilita' (18 - COLPO) / 18)
script.R_nave = function(u)
  if m688.bersaglio_arma(u) then goto evita end
  if not m688.bersaglio_nemico(u) then goto ignora end
  if m688.bersaglio_aereo(u) then goto ignora end
  if not contatto_buono(u) then goto ignora end
  m688.variabile(u, 11, 1)
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, CACCIA)
  if m688.caso(u, PING) then m688.impulso_sonar(u) end
  if armi_libere(u) and not m688.bersaglio_nave(u) and m688.bersaglio_vicino(u, PORTATA_NAVE) then
    m688.messaggio(u, 19)
    m688.attacca(u, PORTATA_NAVE, COLPO)
  end
  m688.ingaggia(u, AGGANCIO, "R_nave", "R_persa")
  m688.salva_modello(u)
  do return end
  ::evita::
  m688.ferma_navigazione(u)
  m688.accosta_opposto(u)
  m688.telegrafo(u, 1)
  m688.comportamento(u, "R_nave")
  m688.salva_modello(u)
  m688.dopo(u, 90, "R_persa")
  do return end
  ::ignora::
  m688.comportamento(u, "R_nave")
  m688.salva_modello(u)
end

-- sottomarino: pattuglia lento; sul contatto nemico si avvicina (paranoia) e lancia entro la portata, al piu' SALVE
-- volte; davanti a un siluro scappa e scende
script.R_sottomarino = function(u)
  if m688.bersaglio_arma(u) then goto evita end
  if not m688.bersaglio_nemico(u) then goto ignora end
  if m688.bersaglio_aereo(u) then goto ignora end
  if not contatto_buono(u) then goto ignora end
  m688.variabile(u, 11, 1)
  if armi_libere(u) and m688.bersaglio_vicino(u, PORTATA_SILURO) and m688.variabile_minore(u, 100 + u, SALVE) then goto lancia end
  if INSEGUE then
    m688.ferma_navigazione(u)
    m688.accosta_verso_bersaglio(u)
    m688.telegrafo(u, AVVICINAMENTO)
  end
  m688.ingaggia(u, AGGANCIO, "R_sottomarino", "R_persa")
  m688.salva_modello(u)
  do return end
  ::lancia::
  m688.variabile_piu(u, 100 + u)
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, AVVICINAMENTO)
  m688.ingaggia(u, AGGANCIO, "R_sottomarino", "R_persa")
  m688.salva_modello(u)
  -- il gestore di lancio crea il siluro dal modello corrente: dopo non si salva piu' il lanciatore
  script["R_siluro_" .. UNITA[u].parte](u)
  do return end
  ::evita::
  m688.ferma_navigazione(u)
  m688.accosta_opposto(u)
  m688.telegrafo(u, 1)
  m688.quota(u, UNITA[u].profonda, 1)
  m688.comportamento(u, "R_sottomarino")
  m688.salva_modello(u)
  m688.dopo(u, 120, "R_persa")
  do return end
  ::ignora::
  m688.comportamento(u, "R_sottomarino")
  m688.salva_modello(u)
end

-- aereo antisom (nel classico; nell'Enhanced lo guida il controllore di docs/13 e vai_al_bersaglio diventa un datum)
script.R_aereo = function(u)
  if not m688.bersaglio_nemico(u) then goto ignora end
  if m688.bersaglio_nave(u) or m688.bersaglio_aereo(u) or m688.bersaglio_arma(u) then goto ignora end
  if not contatto_buono(u) then goto ignora end
  m688.variabile(u, 11, 1)
  m688.vai_al_bersaglio(u, 60, "R_aereo_sopra")
  m688.telegrafo(u, 1)
  m688.comportamento(u, "R_aereo")
  m688.salva_modello(u)
  do return end
  ::ignora::
  m688.comportamento(u, "R_aereo")
  m688.salva_modello(u)
end

script.R_aereo_sopra = function(u)
  if armi_libere(u) and contatto_buono(u) then
    m688.messaggio(u, 19)
    m688.attacca(u, 1, COLPO)
  end
  m688.telegrafo(u, UNITA[u].crociera)
  riprendi_rotta(u)
  m688.salva_modello(u)
end

-- bersaglio perso: la rotta di prima alla velocita' di crociera
script.R_persa = function(u)
  if UNITA[u].rotta then m688.riprendi_navigazione(u) end
  m688.telegrafo(u, UNITA[u].crociera)
  m688.comportamento(u, UNITA[u].comportamento)
  m688.salva_modello(u)
end

-- in allarme le navi fanno ping
script.R_vigila = function(u)
  if m688.variabile_uguale(u, 11, 1) and m688.caso(u, PING) then m688.impulso_sonar(u) end
  m688.dopo(u, VIGILA, "R_vigila")
end

-- fine della rotta di transito: l'unita' esce dal teatro
script.R_esce = function(u)
  m688.togli_unita(u)
end
)";

// Libreria 2: gli stessi comportamenti, con i parametri per unita' della tabella UNITA (missioni dell'editor)
constexpr const char* COMPORTAMENTI_2 = R"(-- Comportamenti dell'IA (API classica, docs/09 §6), con i parametri per unita' della tabella UNITA. Variabili di scena:
-- 1 siluri lanciati dal giocatore (kit), 10 fase della missione, 11 allarme, 100 + id salve lanciate da un'unita',
-- 200 + n regole scattate, 300 + i variabili dell'autore, 400 + id passaggi d'attesa di una nave dopo una salva,
-- 450 + id passaggi di caccia di una nave (nel classico la prima salva dopo PRIMO_ATTACCO), 460 + id le sue salve
-- dall'ultima ripresa, 470 + id i passaggi d'ascolto dopo il tetto delle salve, 480 + id lo stato del pedinamento
-- ravvicinato (1 in corso, 2 scoperto col ping: non torna).
-- Nell'Enhanced (docs/18 fase 3) le regole d'ingaggio sono quelle dell'IA (m688.armi_libere: crisi solo dopo un atto
-- ostile fra le parti, zone amiche) e l'allerta sale con gli eventi, mai sotto la paranoia dell'editor; nel classico
-- quelle funzioni danno nil e valgono le costanti della missione.
local PING, AGGANCIO, VIGILA = { 0.05, 0.2, 0.4, 0.7 }, { 8, 5, 3, 2 }, { 60, 180, 90, 60 }
local ATTESA_SALVA = 20   -- passaggi sul contatto fra due salve antisom di una nave (riposizionarsi, ricaricare)
-- la caccia fino al PROBSUB: la prima salva di una nave dopo PRIMO_ATTACCO_S secondi sul contatto attuale (Enhanced,
-- m688.tempo_contatto), nel classico dopo PRIMO_ATTACCO passaggi; dopo SALVE_MASSIME salve si torna ad ascoltare e si
-- riattacca dopo RIPRESA passaggi sul contatto (STIME)
local PRIMO_ATTACCO_S, PRIMO_ATTACCO = 780, 330
local SALVE_MASSIME, RIPRESA = 6, 120
local function armi_libere(u)
  local a = m688.armi_libere(u)
  if a ~= nil then return a end
  return ARMI == 2 or (ARMI == 1 and m688.variabile_maggiore(u, 1, 0))
end
local function allerta(u)
  local a = m688.allerta(u)
  if a == nil then return UNITA[u].paranoia end
  return math.max(a, UNITA[u].paranoia)
end
local function contatto_buono(u)
  return m688.traccia_solida(u) or (allerta(u) >= 3 and m688.traccia_media(u))
end
-- nell'Enhanced si lancia su una posizione stimata (eco, vista, radar, TMA), non sul solo rilevamento: finche' non
-- c'e', il motore fa da se' le gambe di TMA (fase 4) e lo script non le disturba con rotta e velocita'
local function soluzione(u)
  return m688.allerta(u) == nil or m688.soluzione(u)
end
-- siluri nei tubi o di riserva (Enhanced); nil nel classico o senza i dati di carico: si lancia come prima
local function ha_armi(u)
  local n = m688.armi_rimaste(u)
  return n == nil or n > 0
end
-- i non combattenti (mercantili, pescherecci, navi spia: flag 40h) non si ingaggiano finche' le armi non sono libere
-- (il peschereccio in pace); in guerra un convoglio nemico resta un bersaglio. Nel classico la funzione da' nil
local function non_combattente(u) return m688.bersaglio_non_combattente(u) == true end
local function caccia_finita(u)
  local t = m688.tempo_contatto(u)
  if t ~= nil then return t >= PRIMO_ATTACCO_S end
  return m688.variabile_maggiore(u, 450 + u, PRIMO_ATTACCO)
end
local function riprendi_rotta(u)
  local f = script["R_" .. u .. "_p1"]
  if f ~= nil then f(u) end
end

-- nave da guerra: segue la sua rotta; su un contatto nemico subacqueo accosta, fa ping e, se le regole lo consentono,
-- attacca con le armi antisom (ATTACK: riesce con probabilita' (18 - COLPO) / 18). Le navi nemiche no: la caccia
-- antisom su una nave di superficie illuminava col ping i sottomarini vicini (il tiro fra navi e' del motore e dei missili)
script.R_nave = function(u)
  if m688.bersaglio_arma(u) then goto evita end
  if not m688.bersaglio_nemico(u) or (non_combattente(u) and not armi_libere(u)) then goto ignora end
  if m688.bersaglio_aereo(u) or m688.bersaglio_nave(u) then goto ignora end
  if not contatto_buono(u) then goto ignora end
  m688.variabile(u, 11, 1)
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, CACCIA)
  if m688.caso(u, PING[allerta(u) + 1]) then m688.impulso_sonar(u) end
  -- una salva, poi ATTESA_SALVA passaggi prima della successiva; nell'Enhanced solo con una posizione stimata
  m688.variabile_piu(u, 450 + u)   -- i passaggi di caccia
  if m688.variabile_maggiore(u, 400 + u, 0) then
    m688.variabile_meno(u, 400 + u)
  elseif m688.variabile_maggiore(u, 470 + u, 0) then
    m688.variabile_meno(u, 470 + u)   -- dopo il tetto delle salve: la caccia ricomincia
  elseif armi_libere(u) and not m688.bersaglio_nave(u) and m688.bersaglio_vicino(u, UNITA[u].portata) and soluzione(u) and
         caccia_finita(u) then
    if not m688.variabile_minore(u, 460 + u, SALVE_MASSIME) then
      -- tante salve senza affondarlo: si torna ad ascoltare, e si riattacca solo dopo RIPRESA passaggi sul contatto
      m688.variabile(u, 460 + u, 0)
      m688.variabile(u, 470 + u, RIPRESA)
      goto ascolta
    end
    m688.messaggio(u, 19)
    m688.attacca(u, UNITA[u].portata, COLPO)
    m688.variabile(u, 400 + u, ATTESA_SALVA)
    m688.variabile_piu(u, 460 + u)
  end
  m688.ingaggia(u, AGGANCIO[allerta(u) + 1], "R_nave", "R_persa")
  m688.salva_modello(u)
  do return end
  ::ascolta::
  if UNITA[u].rotta then m688.riprendi_navigazione(u) end
  m688.telegrafo(u, UNITA[u].crociera)
  m688.comportamento(u, "R_nave")
  m688.salva_modello(u)
  do return end
  ::evita::
  m688.ferma_navigazione(u)
  m688.accosta_opposto(u)
  m688.telegrafo(u, 1)
  m688.comportamento(u, "R_nave")
  m688.salva_modello(u)
  m688.dopo(u, 90, "R_persa")
  do return end
  ::ignora::
  m688.comportamento(u, "R_nave")
  m688.salva_modello(u)
end

-- sottomarino: pattuglia lento; sul contatto nemico si avvicina (se insegue) e lancia entro la portata, al piu' `salve`
-- volte; davanti a un siluro scappa e scende
script.R_sottomarino = function(u)
  if m688.bersaglio_arma(u) then goto evita end
  if not m688.bersaglio_nemico(u) or (non_combattente(u) and not armi_libere(u)) then goto ignora end
  if m688.bersaglio_aereo(u) then goto ignora end
  if not contatto_buono(u) then goto ignora end
  m688.variabile(u, 11, 1)
  if armi_libere(u) and m688.bersaglio_vicino(u, UNITA[u].portata) and m688.variabile_minore(u, 100 + u, UNITA[u].salve) and
     soluzione(u) and ha_armi(u) then goto lancia end
  if allerta(u) >= 1 and soluzione(u) then
    m688.ferma_navigazione(u)
    m688.accosta_verso_bersaglio(u)
    m688.telegrafo(u, AVVICINAMENTO)
  end
  m688.ingaggia(u, AGGANCIO[allerta(u) + 1], "R_sottomarino", "R_persa")
  m688.salva_modello(u)
  do return end
  ::lancia::
  m688.variabile_piu(u, 100 + u)
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  m688.telegrafo(u, AVVICINAMENTO)
  m688.ingaggia(u, AGGANCIO[allerta(u) + 1], "R_sottomarino", "R_persa")
  m688.salva_modello(u)
  -- il gestore di lancio crea il siluro dal modello corrente: dopo non si salva piu' il lanciatore
  script["R_siluro_" .. UNITA[u].parte](u)
  do return end
  ::evita::
  m688.ferma_navigazione(u)
  m688.accosta_opposto(u)
  m688.telegrafo(u, 1)
  m688.quota(u, UNITA[u].profonda, 1)
  m688.comportamento(u, "R_sottomarino")
  m688.salva_modello(u)
  m688.dopo(u, 120, "R_persa")
  do return end
  ::ignora::
  m688.comportamento(u, "R_sottomarino")
  m688.salva_modello(u)
end

-- aereo antisom (nel classico; nell'Enhanced lo guida il controllore di docs/13 e vai_al_bersaglio diventa un datum)
script.R_aereo = function(u)
  if not m688.bersaglio_nemico(u) or (non_combattente(u) and not armi_libere(u)) then goto ignora end
  if m688.bersaglio_nave(u) or m688.bersaglio_aereo(u) or m688.bersaglio_arma(u) then goto ignora end
  if not contatto_buono(u) then goto ignora end
  m688.variabile(u, 11, 1)
  m688.vai_al_bersaglio(u, 60, "R_aereo_sopra")
  m688.telegrafo(u, 1)
  m688.comportamento(u, "R_aereo")
  m688.salva_modello(u)
  do return end
  ::ignora::
  m688.comportamento(u, "R_aereo")
  m688.salva_modello(u)
end

script.R_aereo_sopra = function(u)
  -- sopra il bersaglio: attacca solo se quello che osserva adesso e' ancora un sottomarino nemico (l'osservazione gira
  -- su tutte le unita', anche amiche e navi)
  if armi_libere(u) and m688.bersaglio_nemico(u) and not non_combattente(u) and not (m688.bersaglio_nave(u) or m688.bersaglio_aereo(u) or m688.bersaglio_arma(u)) and
     contatto_buono(u) then
    m688.messaggio(u, 19)
    m688.attacca(u, 1, COLPO)
  end
  m688.telegrafo(u, UNITA[u].crociera)
  riprendi_rotta(u)
  m688.salva_modello(u)
end

-- bersaglio perso: la rotta di prima alla velocita' di crociera
script.R_persa = function(u)
  if UNITA[u].rotta then m688.riprendi_navigazione(u) end
  m688.telegrafo(u, UNITA[u].crociera)
  if UNITA[u].comportamento ~= "" then m688.comportamento(u, UNITA[u].comportamento) end
  m688.salva_modello(u)
end

-- pedinamento ravvicinato (S24): sul giocatore, con una traccia buona, il motore lo tiene nel cono sordo a `distanza`
-- miglia e 20 m sotto la quota che vede (m688.pedina_vicino), e ne segue le manovre con ritardo; se il giocatore pinga
-- entro 3 km smette e non torna, e l'evasione dell'IA lo porta via. Solo il giocatore: lo script non sa l'id del bersaglio
script.R_pedina_vicino = function(u)
  if m688.variabile_uguale(u, 480 + u, 1) and m688.ping_giocatore_entro(u, 3000, 60) then
    m688.pedina_vicino(u, -1)
    m688.variabile(u, 480 + u, 2)
  end
  if m688.variabile_uguale(u, 480 + u, 2) or not m688.bersaglio_id(u, 0) or not contatto_buono(u) then goto ignora end
  if m688.variabile_uguale(u, 480 + u, 0) and m688.pedina_vicino(u, 0, UNITA[u].distanza * 1852, -20) then
    m688.variabile(u, 480 + u, 1)
  end
  m688.ingaggia(u, AGGANCIO[allerta(u) + 1], "R_pedina_vicino", "R_persa")
  m688.salva_modello(u)
  do return end
  ::ignora::
  m688.comportamento(u, "R_pedina_vicino")
  m688.salva_modello(u)
end

-- pedinamento: sul contatto nemico si porta a `distanza` miglia e ci resta, alla stessa velocita', senza farsi notare;
-- spara solo se le regole lo consentono, come il sottomarino; davanti a un siluro scappa
script.R_pedina = function(u)
  if m688.bersaglio_arma(u) then goto evita end
  if not m688.bersaglio_nemico(u) or (non_combattente(u) and not armi_libere(u)) then goto ignora end
  if m688.bersaglio_aereo(u) then goto ignora end
  if not contatto_buono(u) then goto ignora end
  if armi_libere(u) and m688.bersaglio_vicino(u, UNITA[u].portata) and m688.variabile_minore(u, 100 + u, UNITA[u].salve) and
     soluzione(u) and ha_armi(u) then goto lancia end
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  if m688.bersaglio_vicino(u, UNITA[u].distanza) then
    m688.uguaglia_velocita(u)
  else
    m688.telegrafo(u, AVVICINAMENTO * 1.6)
  end
  m688.ingaggia(u, AGGANCIO[allerta(u) + 1], "R_pedina", "R_persa")
  m688.salva_modello(u)
  do return end
  ::lancia::
  m688.variabile_piu(u, 100 + u)
  m688.ferma_navigazione(u)
  m688.accosta_verso_bersaglio(u)
  m688.ingaggia(u, AGGANCIO[allerta(u) + 1], "R_pedina", "R_persa")
  m688.salva_modello(u)
  script["R_siluro_" .. UNITA[u].parte](u)
  do return end
  ::evita::
  m688.ferma_navigazione(u)
  m688.accosta_opposto(u)
  m688.telegrafo(u, 1)
  m688.comportamento(u, "R_pedina")
  m688.salva_modello(u)
  m688.dopo(u, 120, "R_persa")
  do return end
  ::ignora::
  m688.comportamento(u, "R_pedina")
  m688.salva_modello(u)
end

-- in allarme le navi fanno ping
script.R_vigila = function(u)
  if m688.variabile_uguale(u, 11, 1) and m688.caso(u, PING[allerta(u) + 1]) then m688.impulso_sonar(u) end
  m688.dopo(u, VIGILA[allerta(u) + 1], "R_vigila")
end

-- fine della rotta di transito: l'unita' esce dal teatro
script.R_esce = function(u)
  m688.togli_unita(u)
end
)";

constexpr double PING[] = {0.05, 0.2, 0.4, 0.7};
constexpr int AGGANCIO[] = {8, 5, 3, 2};
constexpr int VIGILA[] = {0, 180, 90, 45};
constexpr int COLPO[] = {16, 14, 12, 9, 6};

// Il parcheggio delle unita' di riserva: lontano da ogni sensore, fuori dalla carta (come l'angolo dell'originale)
Vec2 parcheggio(const UnitaScenario& u) { return {u.pos.x, u.pos.y + 2.0e6 + 20000.0 * u.id}; }

int lato(const ScenarioMissione& s) { return s.alfa ? URSS : USA; }

const ModelloClassico& modello_di(const UnitaScenario& u, const DatiScenario& d) {
    const auto it = d.modelli.find(u.giocatore ? "giocatore_" + u.classe : u.classe);
    if (it == d.modelli.end()) throw std::invalid_argument("manca il modello classico di " + u.classe);
    return it->second;
}

double crociera(const UnitaScenario& u, const DatiScenario& d) {   // velocita' come frazione del telegrafo
    return std::clamp(u.velocita * 280.0 / (256.0 * modello_di(u, d).massima), 0.0, 1.0);
}

std::string modello_lua(const UnitaScenario& u, const ScenarioMissione& s, const DatiScenario& d) {
    const ModelloClassico& m = modello_di(u, d);
    int sonar = m.sonar;
    if (!u.giocatore && u.parte == PARTI[1 - lato(s)] && sonar > 0) sonar = std::max(1, sonar + (s.difficolta - 3));
    std::string t = "modello = { massima_kn = " + num(m.massima, 4) + ", virata_gradi_s = " + num(m.virata, 4) +
                    ", accel_kn_s = " + num(m.accel, 4) + ", quota_m_s = " + num(m.quota_m_s, 4) + ", quota_max_m = " +
                    num(m.quota_max, 2) + ", integrita = " + intero(m.integrita) + ", rumore = " + intero(m.rumore) +
                    ", sonar = " + intero(sonar);
    if (m.difesa) t += ", difesa = " + intero(*m.difesa);
    t += std::string(", schiera_trainata = ") + (m.schiera && !u.giocatore ? "true" : "false") +
         ", controllo_fondale = " + (m.fondale || u.giocatore ? "true" : "false") + ", periodo_controlli = " +
         intero(m.periodo) + " }";
    return t;
}

std::string carico_lua(const std::string& chiave, const std::vector<VoceCarico>& v) {
    std::string t = "  " + chiave + " = { ";
    for (std::size_t i = 0; i < v.size(); ++i) t += (i ? ", " : "") + v[i].arma + " = " + intero(v[i].numero);
    return t + " },\n";
}

std::array<double, 4> rettangolo(const CerchioScenario& c) { return {c.x - c.r, c.y + c.r, c.x + c.r, c.y - c.r}; }
std::string in_rettangolo(const CerchioScenario& c) {
    const auto q = rettangolo(c);
    return "m688.in_rettangolo(u, " + num(q[0]) + ", " + num(q[1]) + ", " + num(q[2]) + ", " + num(q[3]) + ")";
}
std::string esistono(const std::vector<int>& ids) {
    std::string t;
    for (const int id : ids) t += (t.empty() ? "" : " or ") + std::string("m688.unita_esiste(u, ") + intero(id) + ")";
    return t.empty() ? "false" : t;
}

std::string chiusura_lua(const ChiusuraScenario& c) {
    switch (c.tipo) {
        case ChiusuraScenario::Tipo::solo_tempo: return "";
        case ChiusuraScenario::Tipo::unita_finita:
            if (c.unita.empty()) return "";
            return "  if not m688.unita_esiste(u, " + intero(c.unita[0]) + ") then goto fine end\n";
        case ChiusuraScenario::Tipo::unita_finite:
            return "  if not (" + esistono(c.unita) + ") then goto fine end\n";
        case ChiusuraScenario::Tipo::dentro_poi_fuori:
            return "  if m688.variabile_uguale(u, 10, 0) and " + in_rettangolo(c.zona) + " then m688.variabile(u, 10, 1) end\n" +
                   "  if m688.variabile_uguale(u, 10, 1) and not " + in_rettangolo(c.fuori) + " then goto fine end\n";
        case ChiusuraScenario::Tipo::dentro_poi_dentro:
            return "  if m688.variabile_uguale(u, 10, 0) and " + in_rettangolo(c.zona) + " then m688.variabile(u, 10, 1) end\n" +
                   "  if m688.variabile_uguale(u, 10, 1) and " + in_rettangolo(c.fuori) + " then goto fine end\n";
    }
    return "";
}

// --- regole -----------------------------------------------------------------------------------------------------------

const ZonaScenario* zona_di(const ScenarioMissione& s, const std::string& id) {
    for (const auto& z : s.zone)
        if (z.id == id) return &z;
    return nullptr;
}
int indice_regola(const ScenarioMissione& s, const std::string& id) {
    for (std::size_t i = 0; i < s.regole.size(); ++i)
        if (s.regole[i].id == id) return static_cast<int>(i);
    return -1;
}
const UnitaScenario* unita_di(const ScenarioMissione& s, int id) {
    for (const auto& u : s.unita)
        if (u.id == id) return &u;
    return nullptr;
}

// L'unita' su cui gira una regola: quella di una condizione `unita_in_zona`, altrimenti il giocatore
int soggetto(const ScenarioMissione& s, const RegolaScenario& r) {
    for (const auto& c : r.quando)
        if (c.tipo == CondizioneScenario::Tipo::unita_in_zona) return c.unita;
    return s.unita.empty() ? 0 : s.unita[0].id;
}
double primo_controllo(const RegolaScenario& r) {
    double t = 10.0;
    for (const auto& c : r.quando)
        if (c.tipo == CondizioneScenario::Tipo::tempo) t = std::max(t, c.valore);
    return t;
}

std::string condizione_lua(const ScenarioMissione& s, const CondizioneScenario& c) {
    using T = CondizioneScenario::Tipo;
    const auto zona = [&](const std::string& id) {
        if (!zona_di(s, id)) throw std::invalid_argument("regola: zona sconosciuta " + id);
        return "m688.in_cerchio(u, table.unpack(ZONE[" + stringa(id) + "]))";   // la zona si puo' spostare
    };
    switch (c.tipo) {
        case T::tempo: return "";   // dal primo controllo della regola
        case T::unita_colpita: return "m688.unita_colpita(u, " + intero(c.unita) + ")";
        case T::unita_fuori: return "not m688.unita_esiste(u, " + intero(c.unita) + ")";
        case T::giocatore_in_zona:
        case T::unita_in_zona: return zona(c.zona);
        case T::giocatore_fuori_zona: return "not " + zona(c.zona);
        case T::allarme: return "m688.variabile_uguale(u, 11, 1)";
        case T::giocatore_ha_lanciato: return "m688.variabile_maggiore(u, 1, 0)";
        case T::variabile: {
            const std::string f = c.confronto == ">" ? "variabile_maggiore" : c.confronto == "<" ? "variabile_minore" : "variabile_uguale";
            return "m688." + f + "(u, " + intero(VAR_AUTORE + c.indice) + ", " + num(c.valore, 0) + ")";
        }
        case T::regola_scattata: {
            const int m = indice_regola(s, c.regola);
            if (m < 0) throw std::invalid_argument("regola: regola sconosciuta " + c.regola);
            return "m688.variabile_uguale(u, " + intero(VAR_REGOLE + m) + ", 1)";
        }
        case T::giocatore_sotto_nodi: return "m688.velocita_sotto(u, " + num(c.valore, 2) + ")";
        case T::giocatore_a_quota: return "m688.quota_entro(u, " + num(c.valore, 1) + ", " + num(c.fino_a, 1) + ")";
        case T::giocatore_in_superficie: return "m688.in_superficie(u)";
        case T::giocatore_alberi_alzati: return "m688.alberi_alzati(u)";
        case T::obiettivo_compiuto: return "m688.obiettivo_compiuto(u, " + stringa(c.obiettivo) + ")";
        case T::contatto_su: return "m688.contatto_da(u, " + intero(c.unita) + ", " + num(c.valore, 1) + ")";
        case T::senza_contatto_su: return "m688.senza_contatto_da(u, " + intero(c.unita) + ", " + num(c.valore, 1) + ")";
        case T::ping_vicino:   // la domanda e' sull'unita' della condizione: la si seleziona e poi si torna alla regola
            return "su_unita(u, " + intero(c.unita) + ", m688.ping_giocatore_entro, " + num(c.valore) +
                   (c.fino_a > 0.0 ? ", " + num(c.fino_a, 0) : std::string()) + ")";
        case T::ostile_ha_lanciato: return "m688.ha_lanciato(u, " + intero(c.unita) + ")";
        case T::vicino_al_cavo:   // -1 se la nave non fila la schiera
            return "(m688.distanza_cavo(u, " + intero(c.unita) + ") >= 0 and m688.distanza_cavo(u, " + intero(c.unita) + ") < " + num(c.valore, 0) + ")";
        case T::unita_in_superficie: return "su_unita(u, " + intero(c.unita) + ", m688.in_superficie)";
        case T::giocatore_sopra_nodi: return "not m688.velocita_sotto(u, " + num(c.valore, 2) + ")";
        case T::unita_in_avaria: return "su_unita(u, " + intero(c.unita) + ", m688.in_avaria)";
        case T::unita_rimorchiata: return "su_unita(u, " + intero(c.unita) + ", m688.rimorchiata)";
        case T::giocatore_ha_trasmesso:   // nil nel classico (niente sala radio): falsa
            return "m688.trasmesso(u, " + stringa(c.trasmissione.empty() ? std::string("contatto") : c.trasmissione) + ", " + num(c.valore, 0) + ")";
        case T::evento: return "m688.evento(u, " + stringa(c.evento) + ", " + num(c.valore, 0) + ")";
        case T::unita_vede:
        case T::unita_non_vede: {   // la domanda e' sull'osservatore, come il ping vicino
            const int b = c.bersaglio >= 0 ? c.bersaglio : s.unita.empty() ? 0 : s.unita[0].id;
            return "su_unita(u, " + intero(c.unita) + ", m688." + (c.tipo == T::unita_vede ? "contatto_da" : "senza_contatto_da") + ", " +
                   intero(b) + ", " + num(c.valore, 1) + ")";
        }
        case T::giocatore_vicino_a:   // nil se l'unita' non c'e' piu': lontana
            return "(m688.distanza_da(u, " + intero(c.unita) + ") or 1e18) < " + num(c.valore, 0);
        case T::unita_classifica: {   // nil nel classico (niente sonaristi): falsa
            const int b = c.bersaglio >= 0 ? c.bersaglio : s.unita.empty() ? 0 : s.unita[0].id;
            return "su_unita(u, " + intero(c.unita) + ", m688.classificato, " + intero(b) + ", " + num(c.valore, 0) + ")";
        }
    }
    return "";
}

// I messaggi di bordo propri della missione (E6, docs/09 §5.1b): le azioni «messaggio» con un testo, numerate da 1 nell'
// ordine delle regole; vanno in onda col codice 1000 + n
std::vector<const AzioneScenario*> messaggi_propri(const ScenarioMissione& s) {
    std::vector<const AzioneScenario*> v;
    for (const auto& r : s.regole)
        for (const auto& a : r.allora)
            if (a.tipo == AzioneScenario::Tipo::messaggio && !a.testo[IT].empty()) v.push_back(&a);
    return v;
}

std::string roe_lua(const RoeScenario& r) {
    std::string t = "{ ";
    for (const auto& [k, v] : r) t += k + " = " + stringa(v) + ", ";
    return t + "}";
}

// la chiave `radio` della missione (docs/09 §5.1, R3 ed E6)
std::string radio_lua(const ScenarioMissione& s) {
    const RadioScenario& r = *s.radio;
    std::string t = "  radio = {\n";
    if (r.ordini) {
        t += "    ordini = { da = " + stringa(r.ordini->da) + ", a = " + stringa(r.ordini->a) + ", oggetto = " + stringa(r.ordini->oggetto) +
             ", classifica = " + stringa(r.ordini->classifica) + ", data = { giorno = " + intero(s.giorno) + ", mese = " + intero(s.mese) +
             ", anno = " + intero(s.anno) + ", ora_utc = " + num(s.ora_utc, 3) + " } },\n";
    }
    std::string neutrali;
    for (const auto& u : s.unita)
        if (u.parte == "neutrale") neutrali += intero(u.id) + ", ";
    const auto zona_radio = [](const ZonaScenario& z) {
        return "{ id = " + stringa(z.id) + ", tipo = " + stringa(z.radio) + ", cerchio = { x = " + num(z.cerchio.x) + ", y = " +
               num(z.cerchio.y) + ", r = " + num(z.cerchio.r) + " } }, ";
    };
    std::string zone;
    for (const auto& z : s.zone)
        if (!z.radio.empty() && !z.da_messaggio) zone += zona_radio(z);
    t += "    stato_iniziale = { roe = " + roe_lua(r.roe) + (neutrali.empty() ? std::string() : ", neutrali = { " + neutrali + "}") +
         (zone.empty() ? std::string() : ", zone = { " + zone + "}") + " },\n";
    for (const auto& m : r.messaggi) {
        t += "    { id = " + stringa(m.id) + (m.su_richiesta ? ", su_richiesta = true" : ", dopo_s = " + num(m.dopo_s, 0)) +
             ", precedenza = " + stringa(m.precedenza) + ", da = " + stringa(m.da) + ", oggetto = " + stringa(m.oggetto) +
             ", classifica = " + stringa(m.classifica) + (m.autentico ? "" : ", autentico = false");
        if (!m.roe.empty()) t += ", roe = " + roe_lua(m.roe);
        if (!m.se_evento.empty() || !m.se_ricevuto.empty() || !m.se_unita_colpita.empty()) {
            t += ", se = { ";
            if (!m.se_evento.empty()) t += "evento = " + stringa(m.se_evento) + ", ";
            if (!m.se_ricevuto.empty()) t += "ricevuto = " + stringa(m.se_ricevuto) + ", ";
            if (!m.se_unita_colpita.empty()) {
                t += "unita_colpita = { ";
                for (const int id : m.se_unita_colpita) t += intero(id) + ", ";
                t += "}, ";
            }
            t += "}";
        }
        if (!m.annulla.empty()) t += ", annulla = " + stringa(m.annulla);
        if (!m.esecuzione_tlam.empty()) t += ", esecuzione_tlam = " + stringa(m.esecuzione_tlam);
        if (!m.zone_aggiungi.empty() || !m.zone_togli.empty()) {
            t += ", zone = { ";
            if (!m.zone_aggiungi.empty()) {
                t += "aggiungi = { ";
                for (const auto& id : m.zone_aggiungi)
                    if (const ZonaScenario* z = zona_di(s, id)) t += zona_radio(*z);
                t += "}, ";
            }
            if (!m.zone_togli.empty()) {
                t += "togli = { ";
                for (const auto& id : m.zone_togli) t += stringa(id) + ", ";
                t += "}, ";
            }
            t += "}";
        }
        if (!m.contatto.empty()) {
            t += ", contatto = { unita = { ";
            for (const int id : m.contatto) t += intero(id) + ", ";
            t += "}, errore_m = " + num(m.contatto_errore_m, 0) + ", sensore = " + stringa(m.contatto_sensore) + " }";
        }
        t += ",\n      testo = { it = " + stringa(m.testo[IT]) + ", en = " + stringa(m.testo[EN]) + " } },\n";
    }
    return t + "  },\n";
}

// --- danni iniziali e sonar speciale (Enhanced, docs/09 §4) ---


// la tabella `danni_iniziali`: `n` formatta i numeri (num nella missione, esatto nel blocco dell'editor)
template <typename N>
std::string danni_iniziali_lua(const DanniInizialiScenario& d, N n) {
    std::string t = "danni_iniziali = { ";
    if (d.urto) t += "urto = { tnt_kg = " + n(d.urto->tnt_kg) + ", distanza_m = " + n(d.urto->distanza_m) + ", angolo_gradi = " + n(d.urto->angolo_gradi) + " }, ";
    if (!d.sistemi.empty()) {
        t += "sistemi = { ";
        for (const auto& [k, v] : d.sistemi) t += k + " = " + n(v) + ", ";
        t += "}, ";
    }
    const auto voci = [&](const char* nome, const char* campo, const std::vector<VoceDanno>& v) {
        if (v.empty()) return;
        t += std::string(nome) + " = { ";
        for (const auto& x : v) t += "{ compartimento = " + stringa(x.compartimento) + ", " + campo + " = " + n(x.quanto) + " }, ";
        t += "}, ";
    };
    voci("acqua", "m3", d.acqua);
    voci("falle", "area_m2", d.falle);
    voci("incendi", "intensita", d.incendi);
    return t + "}";
}

template <typename N>
std::string sonar_speciale_lua(const SonarSpecialeScenario& x, N n) {
    return "sonar_speciale = { sl_db = " + n(x.sl_db) + ", frequenza_hz = " + n(x.frequenza_hz) + ", impulso_s = " + n(x.impulso_s) +
           ", risoluzione_s = " + n(x.risoluzione_s) + ", ogni_s = " + n(x.ogni_s) + ", dimensione_m = " + n(x.dimensione_m) +
           (x.antenna.empty() ? std::string() : ", antenna = " + stringa(x.antenna)) + " }";
}

std::string meteo_lua(const MeteoFissoScenario& m) {
    std::string t = "  meteo = { fisso = { ";
    const std::pair<const char*, const std::optional<double>*> CAMPI[] = {
        {"vento_m_s", &m.vento_m_s}, {"vento_da_gradi", &m.vento_da_gradi}, {"stato_mare", &m.stato_mare}, {"visibilita_m", &m.visibilita_m},
        {"pioggia_mm_h", &m.pioggia_mm_h}, {"neve_mm_h", &m.neve_mm_h}, {"nubi_ottavi", &m.nubi_ottavi}};
    for (const auto& [k, v] : CAMPI)
        if (*v) t += std::string(k) + " = " + num(**v, 1) + ", ";
    if (!m.nebbia.empty()) t += "nebbia = " + stringa(m.nebbia) + ", ";
    return t + "} },\n";
}

// Il corpo di un'azione; `su`: l'unita' che seleziona (-1 nessuna)
std::string corpo_azione_lua(const ScenarioMissione& s, const DatiScenario& d, const AzioneScenario& a, int& su) {
    using T = AzioneScenario::Tipo;
    const auto seleziona = [&](int id) {
        if (!unita_di(s, id)) throw std::invalid_argument("regola: unita' sconosciuta " + intero(id));
        su = id;
        return "    m688.seleziona(u, " + intero(id) + ")\n";
    };
    switch (a.tipo) {
        case T::messaggio: {
            if (a.testo[IT].empty()) return "    m688.messaggio(u, " + intero(a.codice) + ")\n";
            const auto propri = messaggi_propri(s);
            const auto n = std::find(propri.begin(), propri.end(), &a) - propri.begin() + 1;
            return "    m688.messaggio_al_giocatore(u, " + intero(1000 + static_cast<int>(n)) + ")\n";
        }
        case T::fine: return "    m688.messaggio(u, " + intero(100 + a.codice) + ")\n";
        case T::variabile: {
            const std::string v = intero(VAR_AUTORE + a.indice);
            if (a.operazione == "=") return "    m688.variabile(u, " + v + ", " + intero(a.valore) + ")\n";
            std::string t;
            for (int i = 0; i < std::abs(a.valore); ++i) t += "    m688.variabile_" + std::string(a.operazione == "-" ? "meno" : "piu") + "(u, " + v + ")\n";
            return t;
        }
        case T::comportamento: {
            std::string t = seleziona(a.unita);
            if (a.comportamento.empty()) t += "    m688.nessun_comportamento(u)\n";
            else t += "    m688.comportamento(u, " + stringa(a.comportamento) + ")\n";
            t += "    m688.salva_modello(u)\n";
            t += "    UNITA[" + intero(a.unita) + "].comportamento = " + stringa(a.comportamento) + "\n";
            return t;
        }
        case T::vai_a: {
            std::string t = seleziona(a.unita) + "    m688.vai_a(u, " + num(a.punto.x) + ", " + num(a.punto.y) + ", 30, \"\")\n";
            std::string crociera;
            if (a.velocita >= 0.0) {   // la nuova crociera, come frazione del telegrafo (R_persa la riprende)
                UnitaScenario v = *unita_di(s, a.unita);
                v.velocita = a.velocita;
                crociera = num(m688::crociera(v, d), 3);
                t += "    m688.telegrafo(u, " + crociera + ")\n";
            }
            t += "    m688.salva_modello(u)\n    UNITA[" + intero(a.unita) + "].rotta = true\n";
            if (!crociera.empty()) t += "    UNITA[" + intero(a.unita) + "].crociera = " + crociera + "\n";
            return t;
        }
        case T::entra: {
            const UnitaScenario* u = unita_di(s, a.unita);
            std::string t = seleziona(a.unita);
            t += "    m688.posizione(u, " + num(u->pos.x) + ", " + num(u->pos.y) + ")\n";
            t += "    m688.rotta_ora(u, " + num(u->rotta) + ")\n";
            t += "    m688.velocita_ora(u, " + num(u->velocita, 2) + ")\n";
            if (!u->punti.empty()) t += "    script.R_" + intero(u->id) + "_p1(u)\n";
            else t += "    m688.salva_modello(u)\n";
            return t;
        }
        case T::cue_asw:
            return "    m688.cue_asw(u, -1, " + num(a.punto.x) + ", " + num(a.punto.y) + ", " + num(a.raggio) + ", 0)\n";
        case T::allarme: return "    m688.variabile(u, 11, 1)\n";
        case T::sposta_zona: {
            const ZonaScenario* z = zona_di(s, a.zona);
            if (!z) throw std::invalid_argument("regola: zona sconosciuta " + a.zona);
            return "    ZONE[" + stringa(a.zona) + "] = { " + num(a.punto.x) + ", " + num(a.punto.y) + ", " + num(z->cerchio.r) + " }\n";
        }
        case T::radio: return "    m688.radio(u, " + stringa(a.zona) + ")\n";
        case T::posa_mina:
            return seleziona(a.unita) + "    m688.posa_mina(u, " + stringa(a.comportamento) + ", " + intero(std::max(1, a.valore)) + ", " +
                   intero(a.codice > 0 ? a.codice : 60) + ")\n";
        case T::avaria: return seleziona(a.unita) + "    m688.avaria(u" + (a.valore == 0 ? std::string(", false") : std::string()) + ")\n";
        case T::rimorchia:
            if (!unita_di(s, a.bersaglio)) throw std::invalid_argument("regola: unita' sconosciuta " + intero(a.bersaglio));
            return seleziona(a.unita) + "    m688.rimorchia(u, " + intero(a.bersaglio) + ", " + num(a.velocita > 0.0 ? a.velocita : 6.0, 1) +
                   ", " + intero(a.valore > 0 ? a.valore : 300) + ")\n";
        case T::molla_rimorchio: return seleziona(a.unita) + "    m688.molla_rimorchio(u)\n";
        case T::esercitazione: return seleziona(a.unita) + "    m688.esercitazione(u" + (a.valore == 0 ? std::string(", false") : std::string()) + ")\n";
        case T::attacca_unita:
            if (!unita_di(s, a.bersaglio)) throw std::invalid_argument("regola: unita' sconosciuta " + intero(a.bersaglio));
            return seleziona(a.unita) + "    m688.attacca_unita(u, " + intero(a.bersaglio) + (a.valore != 0 ? std::string(", true") : std::string()) + ")\n";
        case T::lancia_missile: {
            const auto g = std::find_if(s.unita.begin(), s.unita.end(), [](const UnitaScenario& v) { return v.giocatore; });
            const int b = a.bersaglio >= 0 ? a.bersaglio : g != s.unita.end() ? g->id : 0;
            if (!unita_di(s, b)) throw std::invalid_argument("regola: unita' sconosciuta " + intero(b));
            std::string t = seleziona(a.unita) + "    m688.bersaglio(u, " + intero(b) + ")\n    m688.salva_modello(u)\n";
            for (int i = 0; i < std::max(1, a.valore); ++i) t += "    m688.lancio_missile(u)\n";
            return t;
        }
        case T::quota:   // la quota va nel buffer dell'unita' selezionata: salva_modello la scrive nell'unita'
            return seleziona(a.unita) + "    m688.quota(u, " + intero(a.valore) + ", 1)\n    m688.salva_modello(u)\n";
        case T::rumore: return seleziona(a.unita) + "    m688.rumore_transitorio(u)\n";
        case T::comanda_mine: return "    m688.comanda_mine(u" + (a.zona.empty() ? std::string() : ", " + stringa(a.zona)) + ")\n";
        case T::togli: return seleziona(a.unita) + "    m688.togli_unita(u)\n";   // l'unita' selezionata, come l'opcode 37h
        case T::salvati: return "    m688.salvati(u, " + intero(a.valore) + ", " + intero(a.indice) + ")\n";   // n su di (achievement, carriera)
        case T::situazione: {   // le armi dell'IA (armi_libere dei comportamenti): 0 pace, 1 crisi, 2 guerra; e al motore
            const std::string nome = a.comportamento == "guerra" ? "guerra" : a.comportamento == "crisi" ? "crisi" : "pace";
            if (!a.fra_a.empty() && !a.fra_b.empty())   // fra due parti sole (M7): le armi di tutti restano come sono
                return "    m688.situazione(u, " + stringa(nome) + ", " + stringa(a.fra_a) + ", " + stringa(a.fra_b) + ")\n";
            return std::string("    ARMI = ") + (nome == "guerra" ? "2" : nome == "crisi" ? "1" : "0") + "\n    m688.situazione(u, " +
                   stringa(nome) + ")\n";
        }
    }
    return "";
}

// L'azione nel testo della regola. Su un'altra unita' solo se c'e' ancora: senza, la selezione non cambierebbe l'unita'
// corrente e l'azione finirebbe su quella della regola (il giocatore)
std::string azione_lua(const ScenarioMissione& s, const DatiScenario& d, const AzioneScenario& a, bool& selezionato) {
    int su = -1;
    const std::string corpo = corpo_azione_lua(s, d, a, su);
    if (su < 0) return corpo;
    selezionato = true;
    std::string t = "    if m688.unita_esiste(u, " + intero(su) + ") then\n";
    for (std::size_t i = 0; i < corpo.size();) {
        const std::size_t j = corpo.find('\n', i);
        t += "  " + corpo.substr(i, j - i + 1);
        i = j + 1;
    }
    return t + "    end\n";
}

std::string regole_lua(const ScenarioMissione& s, const DatiScenario& d) {
    if (s.regole.empty()) return "";
    std::string t = "\n-- regole «quando... allora...» dell'editor: un controllo ogni 10 s sull'unita' della regola\n";
    if (!s.zone.empty()) {   // i cerchi delle zone (x, y, raggio di in_cerchio): le regole possono spostarli
        t += "local ZONE = {\n";
        for (const auto& z : s.zone) t += "  [" + stringa(z.id) + "] = { " + num(z.cerchio.x) + ", " + num(z.cerchio.y) + ", " + num(z.cerchio.r) + " },\n";
        t += "}\n";
    }
    const bool su_altra = std::any_of(s.regole.begin(), s.regole.end(), [](const RegolaScenario& r) {
        return std::any_of(r.quando.begin(), r.quando.end(), [](const CondizioneScenario& c) {
            using T = CondizioneScenario::Tipo;
            return c.tipo == T::ping_vicino || c.tipo == T::unita_vede || c.tipo == T::unita_non_vede || c.tipo == T::unita_in_superficie ||
                   c.tipo == T::unita_in_avaria || c.tipo == T::unita_rimorchiata || c.tipo == T::unita_classifica;
        });
    });
    if (su_altra)   // una domanda fatta su un'altra unita' (che diventa la corrente), poi di nuovo sull'unita' della regola
        t += "local function su_unita(u, id, domanda, ...)\n  m688.seleziona(u, id)\n  local r = domanda(u, ...)\n  m688.seleziona(u, u)\n"
             "  return r\nend\n";
    for (std::size_t n = 0; n < s.regole.size(); ++n) {
        const auto& r = s.regole[n];
        const std::string nome = "R_regola_" + intero(static_cast<int>(n));
        const std::string scattata = intero(VAR_REGOLE + static_cast<int>(n));
        std::string cond = "m688.variabile_uguale(u, " + scattata + ", 0)";
        if (!r.una_volta) cond = "true";
        for (const auto& c : r.quando)
            if (const std::string x = condizione_lua(s, c); !x.empty()) cond += " and " + x;
        t += "-- " + r.id + "\n";
        t += "script." + nome + " = function(u)\n";
        t += "  if " + cond + " then\n";
        t += "    m688.variabile(u, " + scattata + ", 1)\n";
        bool selezionato = false;
        for (const auto& a : r.allora) t += azione_lua(s, d, a, selezionato);
        if (selezionato) t += "    m688.seleziona(u, u)\n";
        if (r.una_volta) t += "    do return end\n";
        t += "  end\n";
        t += "  m688.dopo(u, 10, " + stringa(nome) + ")\n";
        t += "end\n";
    }
    return t;
}

// --- il blocco `editor` -------------------------------------------------------------------------------------------------

std::string testi_lua(const TestiScenario& x) { return "{ it = " + stringa(x[IT]) + ", en = " + stringa(x[EN]) + " }"; }
std::string punto_lua(Vec2 p) { return "{ x = " + esatto(p.x) + ", y = " + esatto(p.y) + " }"; }
std::string cerchio_lua(const CerchioScenario& c) {
    return "{ x = " + esatto(c.x) + ", y = " + esatto(c.y) + ", r = " + esatto(c.r) + " }";
}
std::string interi_lua(const std::vector<int>& v) {
    std::string t = "{ ";
    for (std::size_t i = 0; i < v.size(); ++i) t += (i ? ", " : "") + intero(v[i]);
    return t + " }";
}
std::string stringhe_lua(const std::vector<std::string>& v) {
    std::string t = "{ ";
    for (const auto& x : v) t += stringa(x) + ", ";
    return t + "}";
}
std::string vero(bool b) { return b ? "true" : "false"; }

const char* nome_chiusura(ChiusuraScenario::Tipo t) {
    switch (t) {
        case ChiusuraScenario::Tipo::unita_finita: return "unita_finita";
        case ChiusuraScenario::Tipo::unita_finite: return "unita_finite";
        case ChiusuraScenario::Tipo::dentro_poi_fuori: return "dentro_poi_fuori";
        case ChiusuraScenario::Tipo::dentro_poi_dentro: return "dentro_poi_dentro";
        case ChiusuraScenario::Tipo::solo_tempo: break;
    }
    return "solo_tempo";
}
const char* const CONDIZIONI[] = {"tempo", "unita_colpita", "unita_fuori", "giocatore_in_zona", "giocatore_fuori_zona",
                                  "unita_in_zona", "allarme", "giocatore_ha_lanciato", "variabile", "regola_scattata",
                                  "giocatore_sotto_nodi", "giocatore_a_quota", "giocatore_in_superficie", "giocatore_alberi_alzati",
                                  "obiettivo_compiuto", "contatto_su", "senza_contatto_su", "ping_vicino", "ostile_ha_lanciato",
                                  "unita_vede", "unita_non_vede",
                                  "vicino_al_cavo", "unita_in_superficie", "giocatore_sopra_nodi", "unita_in_avaria",
                                  "unita_rimorchiata", "giocatore_ha_trasmesso", "evento", "unita_classifica",
                                  "giocatore_vicino_a"};
const char* const AZIONI[] = {"messaggio", "fine", "variabile", "comportamento", "vai_a", "entra", "cue_asw", "allarme", "sposta_zona",
                              "radio", "situazione", "posa_mina", "avaria", "rimorchia", "molla_rimorchio",
                              "esercitazione", "attacca_unita", "lancia_missile", "quota", "rumore", "comanda_mine", "togli", "salvati"};

std::string blocco_editor(const ScenarioMissione& s) {
    std::string t = "  editor = { formato_editor = " + intero(FORMATO_EDITOR) + ", scenario = {\n";
    t += "    id = " + stringa(s.id) + ", titolo = " + testi_lua(s.titolo) + ", autore = " + stringa(s.autore) + ",\n";
    t += "    teatro = " + stringa(s.teatro) + ", alfa = " + vero(s.alfa) + ", centro = { lat = " + esatto(s.centro_lat) +
         ", lon = " + esatto(s.centro_lon) + " },\n";
    t += "    data = { giorno = " + intero(s.giorno) + ", mese = " + intero(s.mese) + ", anno = " + intero(s.anno) +
         " }, ora_utc = " + esatto(s.ora_utc) + ", meteo_realistico = " + vero(s.meteo_realistico) + ",\n";
    t += "    situazione = " + stringa(s.situazione) + ", paranoia = " + intero(s.paranoia) + ", difficolta = " +
         intero(s.difficolta) + ", durata_s = " + esatto(s.durata_s) + ", libreria = " + intero(s.libreria) + ",\n";
    const auto voci = [](const std::vector<VoceCarico>& v) {
        std::string x = "{ ";
        for (const auto& c : v) x += "{ " + stringa(c.arma) + ", " + intero(c.numero) + " }, ";
        return x + "}";
    };
    t += "    carico = { dotazione = " + voci(s.carico.dotazione) + ", deposito = " + voci(s.carico.deposito) + ", tubi = { ";
    for (const auto& x : s.carico.tubi) t += stringa(x) + ", ";
    t += "}, tipo_missione = " + stringa(s.carico.tipo_missione) + ", ospiti = " + intero(s.carico.ospiti) + " },\n";
    if (s.carriera)
        t += "    carriera = { situazione = " + stringa(s.carriera->situazione) + ", avversario = " + stringa(s.carriera->avversario) +
             ", difficolta = " + intero(s.carriera->difficolta) + ", operazione_speciale = " + vero(s.carriera->operazione_speciale) +
             ", artico = " + vero(s.carriera->artico) + " },\n";
    t += "    unita = {\n";
    for (const auto& u : s.unita) {
        t += "      { id = " + intero(u.id) + ", classe = " + stringa(u.classe) + ", categoria = " + stringa(u.categoria) +
             ", parte = " + stringa(u.parte) + ", ruolo = " + stringa(u.ruolo) + ", flag = " + intero(u.flag) +
             ", giocatore = " + vero(u.giocatore) + ", non_combattente = " + vero(u.non_combattente) + ", nascosta = " +
             vero(u.nascosta) + ",\n        pos = " + punto_lua(u.pos) + ", quota = " + esatto(u.quota) + ", rotta = " +
             esatto(u.rotta) + ", velocita = " + esatto(u.velocita) + ", punti = { ";
        for (const auto& p : u.punti) t += punto_lua(p) + ", ";
        t += "},\n        ciclica = " + vero(u.ciclica) + ", esce = " + vero(u.esce) + ", comportamento = " + stringa(u.comportamento) +
             ", profonda = " + esatto(u.profonda) + ", entra_con = " + stringa(u.entra_con) + ",\n        parametri = { ";
        if (u.parametri.paranoia) t += "paranoia = " + intero(*u.parametri.paranoia) + ", ";
        if (u.parametri.salve) t += "salve = " + intero(*u.parametri.salve) + ", ";
        if (u.parametri.portata_nm) t += "portata_nm = " + esatto(*u.parametri.portata_nm) + ", ";
        if (u.parametri.distanza_nm) t += "distanza_nm = " + esatto(*u.parametri.distanza_nm) + ", ";
        t += "}" + (u.batteria ? ", batteria = " + esatto(*u.batteria) : std::string()) +
             (u.stato.empty() ? std::string() : ", stato = " + stringa(u.stato)) +
             (u.sonar_speciale ? ", " + sonar_speciale_lua(*u.sonar_speciale, [](double v) { return esatto(v); }) : std::string()) +
             (u.armi_esercitazione ? ", armi_esercitazione = true" : "") + (u.emcon.empty() ? std::string() : ", emcon = " + stringa(u.emcon)) +
             (u.luci.empty() ? std::string() : ", luci = " + stringa(u.luci)) +
             (u.cisterne.empty() ? std::string() : ", cisterne = " + stringa(u.cisterne)) +
             " },\n";
    }
    t += "    },\n    obiettivi = {\n";
    for (const auto& o : s.obiettivi) {
        t += "      { id = " + stringa(o.id) + ", tipo = " + stringa(o.tipo) + ", unita = " + interi_lua(o.unita) + ", minimo = " +
             intero(o.minimo) + (o.cerchio ? ", cerchio = " + cerchio_lua(*o.cerchio) : std::string()) + ", entro_s = " +
             esatto(o.entro_s) + ", primario = " + vero(o.primario) + ", colpire = " + vero(o.colpire) + ", testo = " +
             testi_lua(o.testo) + (o.indice >= 0 ? ", indice = " + intero(o.indice) : std::string()) +
             (o.minuti > 0.0 ? ", minuti = " + esatto(o.minuti) : std::string()) +
             (o.distanza_m > 0.0 ? ", distanza_m = " + esatto(o.distanza_m) : std::string()) +
             (o.parte.empty() ? std::string() : ", parte = " + stringa(o.parte)) +
             (o.missione.empty() ? std::string() : ", missione = " + stringa(o.missione)) + (o.finestra ? std::string() : ", finestra = false") +
             (o.massima.empty() ? std::string() : ", massima = " + stringa(o.massima)) +
             (o.errore > 0.0 ? ", errore = " + esatto(o.errore) : std::string()) + (o.da_tma ? ", da_tma = true" : "") + " },\n";
    }
    t += "    },\n    aerei_asw = {\n";
    for (const auto& a : s.aerei_asw) {
        t += "      { unita = " + intero(a.unita) + ", base_unita = " + intero(a.base_unita) +
             (a.base ? ", base = " + punto_lua(*a.base) : std::string()) + ", situazione = " + stringa(a.situazione) +
             ", contro_giocatore = " + vero(a.contro_giocatore) + ", bersagli = " + interi_lua(a.bersagli) + ", zona = " +
             punto_lua(a.zona) + ", raggio = " + esatto(a.raggio) + " },\n";
    }
    t += "    },\n    briefing = { it = { ";
    for (const auto& r : s.briefing[IT]) t += stringa(r) + ", ";
    t += "}, en = { ";
    for (const auto& r : s.briefing[EN]) t += stringa(r) + ", ";
    t += "} },\n";
    t += "    esito_fine = " + testi_lua(s.esito_fine) + ", esito_tempo = " + testi_lua(s.esito_tempo) + ",\n";
    t += "    chiusura = { tipo = " + stringa(nome_chiusura(s.chiusura.tipo)) + ", unita = " + interi_lua(s.chiusura.unita) +
         ", zona = " + cerchio_lua(s.chiusura.zona) + ", fuori = " + cerchio_lua(s.chiusura.fuori) + " },\n";
    t += "    zone = {\n";
    for (const auto& z : s.zone)
        t += "      { id = " + stringa(z.id) + ", nome = " + testi_lua(z.nome) + ", cerchio = " + cerchio_lua(z.cerchio) +
             (z.radio.empty() ? std::string() : ", radio = " + stringa(z.radio)) + (z.da_messaggio ? ", da_messaggio = true" : "") + " },\n";
    t += "    },\n    regole = {\n";
    for (const auto& r : s.regole) {
        t += "      { id = " + stringa(r.id) + ", una_volta = " + vero(r.una_volta) + ",\n        quando = {\n";
        for (const auto& c : r.quando)
            t += "          { tipo = " + stringa(CONDIZIONI[static_cast<int>(c.tipo)]) + ", valore = " + esatto(c.valore) +
                 ", unita = " + intero(c.unita) + ", indice = " + intero(c.indice) + ", zona = " + stringa(c.zona) +
                 ", regola = " + stringa(c.regola) + ", confronto = " + stringa(c.confronto) +
                 (c.fino_a != 0.0 ? ", fino_a = " + esatto(c.fino_a) : std::string()) +
                 (c.obiettivo.empty() ? std::string() : ", obiettivo = " + stringa(c.obiettivo)) +
                 (c.bersaglio >= 0 ? ", bersaglio = " + intero(c.bersaglio) : std::string()) +
                 (c.trasmissione.empty() ? std::string() : ", trasmissione = " + stringa(c.trasmissione)) +
                 (c.evento.empty() ? std::string() : ", evento = " + stringa(c.evento)) + " },\n";
        t += "        },\n        allora = {\n";
        for (const auto& a : r.allora)
            t += "          { tipo = " + stringa(AZIONI[static_cast<int>(a.tipo)]) + ", codice = " + intero(a.codice) +
                 ", unita = " + intero(a.unita) + ", indice = " + intero(a.indice) + ", valore = " + intero(a.valore) +
                 ", comportamento = " + stringa(a.comportamento) + ", operazione = " + stringa(a.operazione) +
                 ", categoria = " + stringa(a.categoria) + ", zona = " + stringa(a.zona) + ", punto = " + punto_lua(a.punto) + ", raggio = " +
                 esatto(a.raggio) + ", testo = " + testi_lua(a.testo) + (a.velocita >= 0.0 ? ", velocita = " + esatto(a.velocita) : std::string()) +
                 (a.bersaglio >= 0 ? ", bersaglio = " + intero(a.bersaglio) : std::string()) +
                 (a.fra_a.empty() ? std::string() : ", fra_a = " + stringa(a.fra_a) + ", fra_b = " + stringa(a.fra_b)) + " },\n";
        t += "        } },\n";
    }
    t += "    },\n";
    if (s.radio) {
        const RadioScenario& r = *s.radio;
        t += "    radio = { ";
        if (r.ordini)
            t += "ordini = { da = " + stringa(r.ordini->da) + ", a = " + stringa(r.ordini->a) + ", oggetto = " + stringa(r.ordini->oggetto) +
                 ", classifica = " + stringa(r.ordini->classifica) + " }, ";
        t += "roe = " + roe_lua(r.roe) + ", messaggi = {\n";
        for (const auto& m : r.messaggi) {
            t += "      { id = " + stringa(m.id) + ", dopo_s = " + esatto(m.dopo_s) + ", su_richiesta = " + vero(m.su_richiesta) +
                 ", precedenza = " + stringa(m.precedenza) + ", da = " + stringa(m.da) + ", oggetto = " + stringa(m.oggetto) +
                 ", classifica = " + stringa(m.classifica) + ", testo = " + testi_lua(m.testo) + ", autentico = " + vero(m.autentico) +
                 ", roe = " + roe_lua(m.roe) + ", se_evento = " + stringa(m.se_evento) + ", se_ricevuto = " + stringa(m.se_ricevuto) +
                 ", se_unita_colpita = " + interi_lua(m.se_unita_colpita) + ", annulla = " + stringa(m.annulla) +
                 (m.contatto.empty() ? std::string()
                                     : ", contatto = " + interi_lua(m.contatto) + ", contatto_errore_m = " + esatto(m.contatto_errore_m) +
                                           ", contatto_sensore = " + stringa(m.contatto_sensore)) +
                 (m.zone_aggiungi.empty() ? std::string() : ", zone_aggiungi = " + stringhe_lua(m.zone_aggiungi)) +
                 (m.zone_togli.empty() ? std::string() : ", zone_togli = " + stringhe_lua(m.zone_togli)) +
                 (m.esecuzione_tlam.empty() ? std::string() : ", esecuzione_tlam = " + stringa(m.esecuzione_tlam)) + " },\n";
        }
        t += "    } },\n";
    }
    if (!s.mine.empty()) {
        t += "    mine = {\n";
        for (const auto& mn : s.mine)
            t += "      { tipo = " + stringa(mn.tipo) + ", pos = " + punto_lua(mn.pos) + ", lato = " + stringa(mn.lato) + ", armata = " +
                 vero(mn.armata) + (mn.sicura ? ", sicura = true" : "") + (mn.comandata ? ", comandata = true" : "") +
                 (mn.gruppo.empty() ? "" : ", gruppo = " + stringa(mn.gruppo)) + " },\n";
        t += "    },\n";
    }
    if (s.danni_iniziali) t += "    " + danni_iniziali_lua(*s.danni_iniziali, [](double v) { return esatto(v); }) + ",\n";
    if (s.esercitazione) t += "    esercitazione = true,\n";
    if (s.situazione_luogo) t += "    situazione_luogo = { usa = " + stringa(s.situazione_luogo->usa) + ", urss = " + stringa(s.situazione_luogo->urss) + " },\n";
    if (s.navigazione)
        t += "    navigazione = { sigma_iniziale_nm = " + esatto(s.navigazione->sigma_iniziale_nm) +
             (s.navigazione->realistica_di_serie ? ", realistica_di_serie = true" : "") + " },\n";
    if (!s.tlam.empty()) {
        t += "    tlam = {\n";
        for (const auto& x : s.tlam)
            t += "      { id = " + stringa(x.id) + ", bersaglio = " + stringa(x.bersaglio) + ", plp = " + punto_lua(x.plp) + ", landfall = " +
                 punto_lua(x.landfall) + ", tot_s = " + esatto(x.tot_s) + ", finestra_s = " + esatto(x.finestra_s) + ", tratta_terra_m = " +
                 esatto(x.tratta_terra_m) + " },\n";
        t += "    },\n";
    }
    if (s.meteo_fisso) {
        const MeteoFissoScenario& m = *s.meteo_fisso;
        t += "    meteo_fisso = { ";
        const std::pair<const char*, const std::optional<double>*> CAMPI[] = {
            {"vento_m_s", &m.vento_m_s}, {"vento_da_gradi", &m.vento_da_gradi}, {"stato_mare", &m.stato_mare}, {"visibilita_m", &m.visibilita_m},
            {"pioggia_mm_h", &m.pioggia_mm_h}, {"neve_mm_h", &m.neve_mm_h}, {"nubi_ottavi", &m.nubi_ottavi}};
        for (const auto& [k, v] : CAMPI)
            if (*v) t += std::string(k) + " = " + esatto(**v) + ", ";
        t += "nebbia = " + stringa(m.nebbia) + " },\n";
    }
    t += "  } },\n";
    return t;
}

// --- lettura del blocco `editor` (Lua isolato) ----------------------------------------------------------------------

struct Lettore {
    lua_State* L;
    std::string stringa(int t, const char* k, const std::string& d = "") {
        lua_getfield(L, t, k);
        std::string v = lua_type(L, -1) == LUA_TSTRING ? lua_tostring(L, -1) : d;
        lua_pop(L, 1);
        return v;
    }
    double numero(int t, const char* k, double d = 0.0) {
        lua_getfield(L, t, k);
        const double v = lua_isnumber(L, -1) ? lua_tonumber(L, -1) : d;
        lua_pop(L, 1);
        return v;
    }
    int intero(int t, const char* k, int d = 0) { return static_cast<int>(std::lround(numero(t, k, d))); }
    bool booleano(int t, const char* k, bool d = false) {
        lua_getfield(L, t, k);
        const bool v = lua_isboolean(L, -1) ? lua_toboolean(L, -1) != 0 : d;
        lua_pop(L, 1);
        return v;
    }
    bool tabella(int t, const char* k) {   // lascia la tabella sullo stack (o niente) e dice se c'e'
        lua_getfield(L, t, k);
        if (lua_istable(L, -1)) return true;
        lua_pop(L, 1);
        return false;
    }
    template <class F>
    void elenco(int t, const char* k, F f) {   // f(indice della voce sullo stack)
        if (!tabella(t, k)) return;
        const int a = lua_gettop(L);
        for (lua_Integer i = 1; lua_rawgeti(L, a, i) != LUA_TNIL; ++i) {
            f(lua_gettop(L));
            lua_pop(L, 1);
        }
        lua_pop(L, 2);
    }
    TestiScenario testi(int t, const char* k) {
        TestiScenario x;
        if (tabella(t, k)) {
            x = {stringa(lua_gettop(L), "it"), stringa(lua_gettop(L), "en")};
            lua_pop(L, 1);
        }
        return x;
    }
    Vec2 punto(int t, const char* k) {
        Vec2 p;
        if (tabella(t, k)) {
            p = {numero(lua_gettop(L), "x"), numero(lua_gettop(L), "y")};
            lua_pop(L, 1);
        }
        return p;
    }
    CerchioScenario cerchio(int t, const char* k) {
        CerchioScenario c;
        if (tabella(t, k)) {
            const int c0 = lua_gettop(L);
            c = {numero(c0, "x"), numero(c0, "y"), numero(c0, "r")};
            lua_pop(L, 1);
        }
        return c;
    }
    std::vector<int> interi(int t, const char* k) {
        std::vector<int> v;
        elenco(t, k, [&](int i) { v.push_back(static_cast<int>(lua_tointeger(L, i))); });
        return v;
    }
    std::vector<std::string> stringhe(int t, const char* k) {
        std::vector<std::string> v;
        elenco(t, k, [&](int i) { v.push_back(lua_tostring(L, i) ? lua_tostring(L, i) : ""); });
        return v;
    }
};

template <std::size_t N>
int indice_nome(const char* const (&nomi)[N], const std::string& s) {
    for (std::size_t i = 0; i < N; ++i)
        if (s == nomi[i]) return static_cast<int>(i);
    throw std::runtime_error("scenario: valore sconosciuto " + s);
}

ScenarioMissione leggi_tabella(Lettore& l, int t) {
    ScenarioMissione s;
    s.id = l.stringa(t, "id");
    s.titolo = l.testi(t, "titolo");
    s.autore = l.stringa(t, "autore");
    s.teatro = l.stringa(t, "teatro");
    s.alfa = l.booleano(t, "alfa");
    if (l.tabella(t, "centro")) {
        s.centro_lat = l.numero(lua_gettop(l.L), "lat");
        s.centro_lon = l.numero(lua_gettop(l.L), "lon");
        lua_pop(l.L, 1);
    }
    if (l.tabella(t, "data")) {
        const int d = lua_gettop(l.L);
        s.giorno = l.intero(d, "giorno", 1);
        s.mese = l.intero(d, "mese", 1);
        s.anno = l.intero(d, "anno", 1989);
        lua_pop(l.L, 1);
    }
    s.ora_utc = l.numero(t, "ora_utc");
    s.meteo_realistico = l.booleano(t, "meteo_realistico");
    s.situazione = l.stringa(t, "situazione", "guerra");
    s.paranoia = l.intero(t, "paranoia", 1);
    s.difficolta = l.intero(t, "difficolta", 3);
    s.durata_s = l.numero(t, "durata_s", 3.0 * 3600.0);
    s.libreria = l.intero(t, "libreria", 2);
    if (l.tabella(t, "carico")) {
        const int c = lua_gettop(l.L);
        const auto voci = [&](const char* k, std::vector<VoceCarico>& v) {
            l.elenco(c, k, [&](int i) {
                lua_rawgeti(l.L, i, 1);
                lua_rawgeti(l.L, i, 2);
                v.push_back({lua_tostring(l.L, -2) ? lua_tostring(l.L, -2) : "", static_cast<int>(lua_tointeger(l.L, -1))});
                lua_pop(l.L, 2);
            });
        };
        voci("dotazione", s.carico.dotazione);
        voci("deposito", s.carico.deposito);
        s.carico.tubi = l.stringhe(c, "tubi");
        s.carico.tipo_missione = l.stringa(c, "tipo_missione", "antisom");
        s.carico.ospiti = l.intero(c, "ospiti");
        lua_pop(l.L, 1);
    }
    if (l.tabella(t, "carriera")) {
        const int c = lua_gettop(l.L);
        CarrieraScenario k;
        k.situazione = l.stringa(c, "situazione", k.situazione);
        k.avversario = l.stringa(c, "avversario", k.avversario);
        k.difficolta = l.intero(c, "difficolta", k.difficolta);
        k.operazione_speciale = l.booleano(c, "operazione_speciale");
        k.artico = l.booleano(c, "artico");
        s.carriera = k;
        lua_pop(l.L, 1);
    }
    l.elenco(t, "unita", [&](int i) {
        UnitaScenario u;
        u.id = l.intero(i, "id");
        u.classe = l.stringa(i, "classe");
        u.categoria = l.stringa(i, "categoria");
        u.parte = l.stringa(i, "parte");
        u.ruolo = l.stringa(i, "ruolo");
        u.flag = l.intero(i, "flag");
        u.giocatore = l.booleano(i, "giocatore");
        u.non_combattente = l.booleano(i, "non_combattente");
        u.nascosta = l.booleano(i, "nascosta");
        u.pos = l.punto(i, "pos");
        u.quota = l.numero(i, "quota");
        u.rotta = l.numero(i, "rotta");
        u.velocita = l.numero(i, "velocita");
        l.elenco(i, "punti", [&](int k) { u.punti.push_back({l.numero(k, "x"), l.numero(k, "y")}); });
        u.ciclica = l.booleano(i, "ciclica");
        u.esce = l.booleano(i, "esce");
        u.comportamento = l.stringa(i, "comportamento");
        u.profonda = l.numero(i, "profonda");
        u.entra_con = l.stringa(i, "entra_con");
        lua_getfield(l.L, i, "batteria");
        if (lua_isnumber(l.L, -1)) u.batteria = lua_tonumber(l.L, -1);
        lua_pop(l.L, 1);
        u.stato = l.stringa(i, "stato");
        u.armi_esercitazione = l.booleano(i, "armi_esercitazione");
        u.emcon = l.stringa(i, "emcon");
        u.luci = l.stringa(i, "luci");
        u.cisterne = l.stringa(i, "cisterne");
        if (l.tabella(i, "sonar_speciale")) {
            const int k = lua_gettop(l.L);
            SonarSpecialeScenario x;
            x.sl_db = l.numero(k, "sl_db");
            x.frequenza_hz = l.numero(k, "frequenza_hz");
            x.impulso_s = l.numero(k, "impulso_s");
            x.risoluzione_s = l.numero(k, "risoluzione_s");
            x.ogni_s = l.numero(k, "ogni_s");
            x.dimensione_m = l.numero(k, "dimensione_m");
            x.antenna = l.stringa(k, "antenna");
            u.sonar_speciale = x;
            lua_pop(l.L, 1);
        }
        if (l.tabella(i, "parametri")) {
            const int p = lua_gettop(l.L);
            lua_getfield(l.L, p, "paranoia");
            if (lua_isnumber(l.L, -1)) u.parametri.paranoia = static_cast<int>(lua_tointeger(l.L, -1));
            lua_getfield(l.L, p, "salve");
            if (lua_isnumber(l.L, -1)) u.parametri.salve = static_cast<int>(lua_tointeger(l.L, -1));
            lua_getfield(l.L, p, "portata_nm");
            if (lua_isnumber(l.L, -1)) u.parametri.portata_nm = lua_tonumber(l.L, -1);
            lua_getfield(l.L, p, "distanza_nm");
            if (lua_isnumber(l.L, -1)) u.parametri.distanza_nm = lua_tonumber(l.L, -1);
            lua_pop(l.L, 5);
        }
        s.unita.push_back(u);
    });
    l.elenco(t, "obiettivi", [&](int i) {
        ObiettivoScenario o;
        o.id = l.stringa(i, "id");
        o.tipo = l.stringa(i, "tipo");
        o.unita = l.interi(i, "unita");
        o.minimo = l.intero(i, "minimo", -1);
        if (l.tabella(i, "cerchio")) {
            lua_pop(l.L, 1);
            o.cerchio = l.cerchio(i, "cerchio");
        }
        o.entro_s = l.numero(i, "entro_s", -1.0);
        o.primario = l.booleano(i, "primario", true);
        o.colpire = l.booleano(i, "colpire", true);
        o.testo = l.testi(i, "testo");
        o.indice = l.intero(i, "indice", -1);
        o.minuti = l.numero(i, "minuti", 0.0);
        o.distanza_m = l.numero(i, "distanza_m", 0.0);
        o.parte = l.stringa(i, "parte");
        o.missione = l.stringa(i, "missione");
        o.finestra = l.booleano(i, "finestra", true);
        o.massima = l.stringa(i, "massima");
        o.errore = l.numero(i, "errore", 0.0);
        o.da_tma = l.booleano(i, "da_tma", false);
        s.obiettivi.push_back(o);
    });
    l.elenco(t, "aerei_asw", [&](int i) {
        AereoAswScenario a;
        a.unita = l.intero(i, "unita");
        a.base_unita = l.intero(i, "base_unita", -1);
        if (l.tabella(i, "base")) {
            lua_pop(l.L, 1);
            a.base = l.punto(i, "base");
        }
        a.situazione = l.stringa(i, "situazione", "guerra");
        a.contro_giocatore = l.booleano(i, "contro_giocatore", true);
        a.bersagli = l.interi(i, "bersagli");
        a.zona = l.punto(i, "zona");
        a.raggio = l.numero(i, "raggio", 20000.0);
        s.aerei_asw.push_back(a);
    });
    if (l.tabella(t, "briefing")) {
        const int b = lua_gettop(l.L);
        s.briefing[IT] = l.stringhe(b, "it");
        s.briefing[EN] = l.stringhe(b, "en");
        lua_pop(l.L, 1);
    }
    s.esito_fine = l.testi(t, "esito_fine");
    s.esito_tempo = l.testi(t, "esito_tempo");
    if (l.tabella(t, "chiusura")) {
        const int c = lua_gettop(l.L);
        const std::string tipo = l.stringa(c, "tipo", "solo_tempo");
        s.chiusura.tipo = tipo == "unita_finita"        ? ChiusuraScenario::Tipo::unita_finita
                          : tipo == "unita_finite"      ? ChiusuraScenario::Tipo::unita_finite
                          : tipo == "dentro_poi_fuori"  ? ChiusuraScenario::Tipo::dentro_poi_fuori
                          : tipo == "dentro_poi_dentro" ? ChiusuraScenario::Tipo::dentro_poi_dentro
                                                        : ChiusuraScenario::Tipo::solo_tempo;
        s.chiusura.unita = l.interi(c, "unita");
        s.chiusura.zona = l.cerchio(c, "zona");
        s.chiusura.fuori = l.cerchio(c, "fuori");
        lua_pop(l.L, 1);
    }
    l.elenco(t, "zone", [&](int i) {
        s.zone.push_back({l.stringa(i, "id"), l.testi(i, "nome"), l.cerchio(i, "cerchio"), l.stringa(i, "radio"), l.booleano(i, "da_messaggio")});
    });
    l.elenco(t, "regole", [&](int i) {
        RegolaScenario r;
        r.id = l.stringa(i, "id");
        r.una_volta = l.booleano(i, "una_volta", true);
        l.elenco(i, "quando", [&](int k) {
            CondizioneScenario c;
            c.tipo = static_cast<CondizioneScenario::Tipo>(indice_nome(CONDIZIONI, l.stringa(k, "tipo")));
            c.valore = l.numero(k, "valore");
            c.unita = l.intero(k, "unita", -1);
            c.indice = l.intero(k, "indice");
            c.zona = l.stringa(k, "zona");
            c.regola = l.stringa(k, "regola");
            c.confronto = l.stringa(k, "confronto", "=");
            c.fino_a = l.numero(k, "fino_a", 0.0);
            c.obiettivo = l.stringa(k, "obiettivo");
            c.bersaglio = l.intero(k, "bersaglio", -1);
            c.trasmissione = l.stringa(k, "trasmissione");
            c.evento = l.stringa(k, "evento");
            r.quando.push_back(c);
        });
        l.elenco(i, "allora", [&](int k) {
            AzioneScenario a;
            a.tipo = static_cast<AzioneScenario::Tipo>(indice_nome(AZIONI, l.stringa(k, "tipo")));
            a.codice = l.intero(k, "codice");
            a.unita = l.intero(k, "unita", -1);
            a.indice = l.intero(k, "indice");
            a.valore = l.intero(k, "valore");
            a.comportamento = l.stringa(k, "comportamento");
            a.operazione = l.stringa(k, "operazione", "=");
            a.categoria = l.stringa(k, "categoria");
            a.fra_a = l.stringa(k, "fra_a");
            a.fra_b = l.stringa(k, "fra_b");
            a.zona = l.stringa(k, "zona");
            a.punto = l.punto(k, "punto");
            a.raggio = l.numero(k, "raggio", 1852.0);
            a.testo = l.testi(k, "testo");
            a.velocita = l.numero(k, "velocita", -1.0);
            a.bersaglio = l.intero(k, "bersaglio", -1);
            r.allora.push_back(a);
        });
        s.regole.push_back(r);
    });
    const auto roe = [&](int k, RoeScenario& v) {   // una tabella { categoria = stato }
        if (!l.tabella(k, "roe")) return;
        const int r = lua_gettop(l.L);
        for (lua_pushnil(l.L); lua_next(l.L, r); lua_pop(l.L, 1))
            if (lua_type(l.L, -2) == LUA_TSTRING && lua_type(l.L, -1) == LUA_TSTRING) v[lua_tostring(l.L, -2)] = lua_tostring(l.L, -1);
        lua_pop(l.L, 1);
    };
    if (l.tabella(t, "radio")) {
        const int r = lua_gettop(l.L);
        RadioScenario radio;
        if (l.tabella(r, "ordini")) {
            const int o = lua_gettop(l.L);
            radio.ordini = OrdiniScenario{l.stringa(o, "da"), l.stringa(o, "a"), l.stringa(o, "oggetto"), l.stringa(o, "classifica", "SECRET")};
            lua_pop(l.L, 1);
        }
        roe(r, radio.roe);
        l.elenco(r, "messaggi", [&](int k) {
            MessaggioScenario m;
            m.id = l.stringa(k, "id");
            m.dopo_s = l.numero(k, "dopo_s");
            m.su_richiesta = l.booleano(k, "su_richiesta");
            m.precedenza = l.stringa(k, "precedenza", "routine");
            m.da = l.stringa(k, "da");
            m.oggetto = l.stringa(k, "oggetto");
            m.classifica = l.stringa(k, "classifica", "CONFIDENTIAL");
            m.testo = l.testi(k, "testo");
            m.autentico = l.booleano(k, "autentico", true);
            roe(k, m.roe);
            m.se_evento = l.stringa(k, "se_evento");
            m.se_ricevuto = l.stringa(k, "se_ricevuto");
            m.se_unita_colpita = l.interi(k, "se_unita_colpita");
            m.annulla = l.stringa(k, "annulla");
            m.contatto = l.interi(k, "contatto");
            m.contatto_errore_m = l.numero(k, "contatto_errore_m", 5000.0);
            m.contatto_sensore = l.stringa(k, "contatto_sensore", "sosus");
            m.zone_aggiungi = l.stringhe(k, "zone_aggiungi");
            m.zone_togli = l.stringhe(k, "zone_togli");
            m.esecuzione_tlam = l.stringa(k, "esecuzione_tlam");
            radio.messaggi.push_back(m);
        });
        s.radio = radio;
        lua_pop(l.L, 1);
    }
    l.elenco(t, "mine", [&](int k) {
        MinaScenario mn;
        mn.tipo = l.stringa(k, "tipo", "mk67");
        mn.pos = l.punto(k, "pos");
        mn.lato = l.stringa(k, "lato", "urss");
        mn.armata = l.booleano(k, "armata", true);
        mn.sicura = l.booleano(k, "sicura", false);
        mn.comandata = l.booleano(k, "comandata", false);
        mn.gruppo = l.stringa(k, "gruppo");
        s.mine.push_back(mn);
    });
    s.esercitazione = l.booleano(t, "esercitazione");
    if (l.tabella(t, "situazione_luogo")) {
        s.situazione_luogo = SituazioneLuogo{l.stringa(lua_gettop(l.L), "usa", "pace"), l.stringa(lua_gettop(l.L), "urss", "pace")};
        lua_pop(l.L, 1);
    }
    if (l.tabella(t, "navigazione")) {
        s.navigazione = NavigazioneScenario{l.numero(lua_gettop(l.L), "sigma_iniziale_nm"), l.booleano(lua_gettop(l.L), "realistica_di_serie")};
        lua_pop(l.L, 1);
    }
    l.elenco(t, "tlam", [&](int k) {
        TlamScenario x;
        x.id = l.stringa(k, "id");
        x.bersaglio = l.stringa(k, "bersaglio");
        x.plp = l.punto(k, "plp");
        x.landfall = l.punto(k, "landfall");
        x.tot_s = l.numero(k, "tot_s");
        x.finestra_s = l.numero(k, "finestra_s", 600.0);
        x.tratta_terra_m = l.numero(k, "tratta_terra_m");
        s.tlam.push_back(x);
    });
    if (l.tabella(t, "danni_iniziali")) {
        const int k = lua_gettop(l.L);
        DanniInizialiScenario d;
        if (l.tabella(k, "urto")) {
            const int u = lua_gettop(l.L);
            d.urto = UrtoIniziale{l.numero(u, "tnt_kg"), l.numero(u, "distanza_m"), l.numero(u, "angolo_gradi")};
            lua_pop(l.L, 1);
        }
        if (l.tabella(k, "sistemi")) {
            const int m = lua_gettop(l.L);
            for (lua_pushnil(l.L); lua_next(l.L, m); lua_pop(l.L, 1))
                if (lua_type(l.L, -2) == LUA_TSTRING && lua_isnumber(l.L, -1)) d.sistemi[lua_tostring(l.L, -2)] = lua_tonumber(l.L, -1);
            lua_pop(l.L, 1);
        }
        const auto voci = [&](const char* nome, const char* campo, std::vector<VoceDanno>& v) {
            l.elenco(k, nome, [&](int j) { v.push_back({l.stringa(j, "compartimento"), l.numero(j, campo)}); });
        };
        voci("acqua", "m3", d.acqua);
        voci("falle", "area_m2", d.falle);
        voci("incendi", "intensita", d.incendi);
        s.danni_iniziali = d;
        lua_pop(l.L, 1);
    }
    if (l.tabella(t, "meteo_fisso")) {
        const int m = lua_gettop(l.L);
        MeteoFissoScenario mf;
        const auto campo = [&](const char* k, std::optional<double>& v) {
            lua_getfield(l.L, m, k);
            if (lua_isnumber(l.L, -1)) v = lua_tonumber(l.L, -1);
            lua_pop(l.L, 1);
        };
        campo("vento_m_s", mf.vento_m_s);
        campo("vento_da_gradi", mf.vento_da_gradi);
        campo("stato_mare", mf.stato_mare);
        campo("visibilita_m", mf.visibilita_m);
        campo("pioggia_mm_h", mf.pioggia_mm_h);
        campo("neve_mm_h", mf.neve_mm_h);
        campo("nubi_ottavi", mf.nubi_ottavi);
        mf.nebbia = l.stringa(m, "nebbia");
        s.meteo_fisso = mf;
        lua_pop(l.L, 1);
    }
    return s;
}

}  // namespace

DatiScenario carica_dati_scenario(const std::string& cartella) {
    DatiScenario d;
    const std::string percorso = cartella + "/missioni_rapide.toml";
    toml::table tab;
    try {
        tab = toml::parse_file(percorso);
    } catch (const toml::parse_error& e) {
        throw std::runtime_error(percorso + ": " + std::string(e.description()));
    }
    const toml::node_view<const toml::node> t{tab};
    if (const auto* n = t["modelli"].as_table())
        for (auto&& [k, v] : *n) {
            const toml::node_view<const toml::node> m{v};
            ModelloClassico mc;
            mc.massima = m["massima_kn"].value_or(mc.massima);
            mc.virata = m["virata_gradi_s"].value_or(mc.virata);
            mc.accel = m["accel_kn_s"].value_or(mc.accel);
            mc.quota_m_s = m["quota_m_s"].value_or(mc.quota_m_s);
            mc.quota_max = m["quota_max_m"].value_or(mc.quota_max);
            mc.integrita = static_cast<int>(m["integrita"].value_or(std::int64_t{mc.integrita}));
            mc.rumore = static_cast<int>(m["rumore"].value_or(std::int64_t{mc.rumore}));
            mc.sonar = static_cast<int>(m["sonar"].value_or(std::int64_t{mc.sonar}));
            if (const auto x = m["difesa"].value<std::int64_t>()) mc.difesa = static_cast<int>(*x);
            mc.schiera = m["schiera_trainata"].value_or(false);
            mc.fondale = m["controllo_fondale"].value_or(false);
            mc.periodo = static_cast<int>(m["periodo_controlli"].value_or(std::int64_t{1}));
            d.modelli[std::string(k.str())] = mc;
        }
    d.esito_fine = testi_toml(t["obiettivi"]["esito_fine"]);
    d.esito_tempo = testi_toml(t["obiettivi"]["esito_tempo"]);
    d.kit[USA] = leggi_kit(cartella + "/missioni_rapide/kit_usa.lua");
    d.kit[URSS] = leggi_kit(cartella + "/missioni_rapide/kit_urss.lua");
    for (const auto& [id, c] : carica_classi(cartella + "/classi.toml")) d.pescaggi[id] = c.pescaggio_m;
    return d;
}

// I comportamenti delle rapide per versione del generatore (missione_rapida.hpp): la 1 come il 25/10/2026, byte per
// byte; dalla 2 R_nave lascia le navi di superficie (la caccia antisom col ping illuminava i sottomarini vicini)
namespace {
std::string comportamenti_rapide(int versione) {
    std::string s = COMPORTAMENTI_1;
    if (versione <= 1) return s;
    const auto sostituisci = [&s](const std::string& a, const std::string& b) {
        const auto i = s.find(a);
        if (i == std::string::npos || s.find(a, i + 1) != std::string::npos) throw std::logic_error("comportamenti delle rapide: " + a);
        s.replace(i, a.size(), b);
    };
    sostituisci("-- nave da guerra: segue la sua rotta; su un contatto nemico accosta, fa ping e, se le regole lo consentono, attacca\n"
                "-- con le armi antisom (ATTACK: riesce con probabilita' (18 - COLPO) / 18)\n",
                "-- nave da guerra: segue la sua rotta; su un contatto nemico subacqueo accosta, fa ping e, se le regole lo consentono,\n"
                "-- attacca con le armi antisom (ATTACK: riesce con probabilita' (18 - COLPO) / 18). Le navi nemiche no (versione 2)\n");
    sostituisci("script.R_nave = function(u)\n  if m688.bersaglio_arma(u) then goto evita end\n  if not m688.bersaglio_nemico(u) then goto ignora end\n"
                "  if m688.bersaglio_aereo(u) then goto ignora end\n",
                "script.R_nave = function(u)\n  if m688.bersaglio_arma(u) then goto evita end\n  if not m688.bersaglio_nemico(u) then goto ignora end\n"
                "  if m688.bersaglio_aereo(u) or m688.bersaglio_nave(u) then goto ignora end\n");
    return s;
}
}  // namespace

std::string scrivi_missione_lua(const ScenarioMissione& sc, const DatiScenario& d) {
    if (sc.unita.empty() || !sc.unita[0].giocatore) throw std::invalid_argument("scenario senza il giocatore come prima unita'");
    const int lato_ = lato(sc), ostile_ = 1 - lato_;
    const bool v1 = sc.libreria == 1;
    const int par = std::clamp(sc.paranoia, 0, 3), dif = std::clamp(sc.difficolta, 1, 5);
    std::string s;
    // intestazione
    if (sc.rapida) {
        const auto& r = *sc.rapida;
        s += "-- " + sc.id + ": missione rapida generata da engine/missioni/src/missione_rapida.cpp (versione " + intero(r.versione) + ") dal codice " + r.codice + ".\n";
        s += "-- Formato: docs/09_schema_missioni.md. Lo stesso codice rigenera lo stesso file: non serve modificarlo a mano.\n";
        s += "-- Teatro " + sc.teatro + ", tipo " + r.tipo + ", parte " + (sc.alfa ? "Alfa" : "688") + ", forze " + intero(r.forze) +
             ", paranoia " + intero(r.paranoia) + ", difficolta' " + intero(r.difficolta) + ", ASW aereo " + (r.asw_aereo ? "si'" : "no") +
             ", meteo " + (r.meteo_realistico ? "realistico" : "semplificato") + ".\n\n";
    } else {
        s += "-- " + sc.id + ": missione dell'editor (engine/missioni/src/scenario_missione.cpp)" +
             (sc.autore.empty() ? std::string() : ", autore " + sc.autore) + ".\n";
        s += "-- Formato: docs/09_schema_missioni.md. Lo scenario e' nel blocco `editor`: l'editor lo rilegge e riscrive il file.\n\n";
    }
    s += "local script = {}\n\n";
    s += "-- parametro i del comando del giocatore (p), oppure il valore scritto nello script\n";
    s += "local function par(p, i, predefinito) if p ~= nil and p[i] ~= nil then return p[i] end return predefinito end\n\n";
    s += "local missione = {\n";
    s += "  formato = 1,\n";
    s += "  id = " + stringa(sc.id) + ",\n";
    if (sc.rapida) {
        const auto& r = *sc.rapida;
        s += "  codice_rapida = " + stringa(r.codice) + ",\n";
        s += "  rapida = { versione = " + intero(r.versione) + ", seme = " + std::to_string(r.seme) + ", tipo = " + stringa(r.tipo) + ", forze = " +
             intero(r.forze) + ", paranoia = " + intero(r.paranoia) + ", difficolta = " + intero(r.difficolta) + ", asw_aereo = " +
             vero(r.asw_aereo) + ", meteo = " + stringa(r.meteo_realistico ? "realistico" : "semplificato") +
             ", carriera_richiesta = " + vero(r.carriera_richiesta) + " },\n";
    }
    s += "  titolo = { it = " + stringa(sc.titolo[IT]) + ", en = " + stringa(sc.titolo[EN]) + " },\n";
    s += "  lato = " + stringa(PARTI[lato_]) + ",\n";
    s += "  teatro = " + stringa(sc.teatro) + ",\n";
    s += "  posa = { metodo = \"rapida\", centro = { lat = " + num(sc.centro_lat, 4) + ", lon = " + num(sc.centro_lon, 4) +
         " }, metri_per_unita = { x = 37.04, y = 37.04 }, moto_classico_m_s_per_nodo = 0.514444, nota = \"posizioni in metri sul teatro, velocita' vere anche nel classico\" },\n";
    char data[16];
    std::snprintf(data, sizeof data, "%04d-%02d-%02d", sc.anno, sc.mese, sc.giorno);
    s += "  data = " + stringa(data) + ", ora_utc = " + num(sc.ora_utc, 3) + ",\n";
    // armi del giocatore
    const KitScenario& k = d.kit[static_cast<std::size_t>(lato_)];
    s += carico_lua("dotazione", sc.carico.dotazione);
    s += "  tubi = { ";
    for (std::size_t i = 0; i < sc.carico.tubi.size(); ++i) s += (i ? ", " : "") + stringa(sc.carico.tubi[i]);
    s += " },\n";
    if (!sc.carico.deposito.empty()) s += carico_lua("deposito", sc.carico.deposito);
    s += "  tipo_missione = " + stringa(sc.carico.tipo_missione) + ",\n";
    if (sc.carico.ospiti > 0) s += "  ospiti = " + intero(sc.carico.ospiti) + ",\n";
    s += "  armi = {\n" + k.armi + "  },\n";
    s += "  gestori_giocatore = {\n" + k.gestori + "  },\n";
    if (sc.carriera) {
        const auto& c = *sc.carriera;
        s += "  carriera = { situazione = " + stringa(c.situazione) + ", avversario = " + stringa(c.avversario) + ", difficolta = " +
             intero(c.difficolta) + ", operazione_speciale = " + vero(c.operazione_speciale) + ", artico = " + vero(c.artico) + " },\n";
    } else {
        s += "  carriera = false,   -- missione fuori carriera\n";
    }
    if (!sc.aerei_asw.empty()) {
        static const char* const EQUIPAGGIO[] = {"scarso", "scarso", "medio", "esperto", "esperto"};
        s += "  aereo_asw = {\n";
        for (const auto& a : sc.aerei_asw) {
            s += "    { unita = " + intero(a.unita) + ", missione = \"caccia\", situazione = " + stringa(a.situazione) + ", bersagli = ";
            if (a.contro_giocatore) s += "\"giocatore\"";
            else {
                s += "{ ";
                for (std::size_t i = 0; i < a.bersagli.size(); ++i) s += (i ? ", " : "") + intero(a.bersagli[i]);
                s += " }";
            }
            // le rapide v1 tengono le armi solo in guerra; nell'editor le armi ci sono e decide la situazione, che le
            // regole cambiano con m688.situazione (docs/18 fase 3)
            if (sc.libreria == 1) s += std::string(", armi = ") + (a.situazione == "guerra" ? "true" : "false");
            s += ", equipaggio = " + stringa(EQUIPAGGIO[dif - 1]) + ",\n      ";
            if (a.base_unita >= 0) s += "base_unita = " + intero(a.base_unita) + ", ";
            else if (a.base) s += "base = { x = " + num(a.base->x) + ", y = " + num(a.base->y) + " }, ";
            s += "zona = { x = " + num(a.zona.x) + ", y = " + num(a.zona.y) + ", raggio_m = " + num(a.raggio) + " } },\n";
        }
        s += "  },\n";
    }
    // gli esiti delle regole con la categoria
    std::vector<const AzioneScenario*> fini;   // un codice una volta (vale la prima regola che lo usa)
    std::set<int> codici;
    for (const auto& r : sc.regole)
        for (const auto& a : r.allora)
            if (a.tipo == AzioneScenario::Tipo::fine && codici.insert(a.codice).second) fini.push_back(&a);
    bool categorie = false;
    for (const auto* a : fini) categorie = categorie || !a->categoria.empty();
    if (categorie) {
        s += "  categorie_esiti = { ";
        bool primo = true;
        for (const auto* a : fini)
            if (!a->categoria.empty()) {
                s += (primo ? "" : ", ") + std::string("[") + intero(a->codice) + "] = " + stringa(a->categoria);
                primo = false;
            }
        s += " },\n";
    }
    s += "  obiettivi = {\n";
    for (const auto& o : sc.obiettivi) {
        s += "    { id = " + stringa(o.id) + ", tipo = " + stringa(o.tipo);
        if (!o.unita.empty()) {
            s += ", unita = { ";
            for (std::size_t i = 0; i < o.unita.size(); ++i) s += (i ? ", " : "") + intero(o.unita[i]);
            s += " }";
        }
        if (o.minimo >= 0) s += ", minimo = " + intero(o.minimo);
        if (o.cerchio) s += ", cerchio = { x = " + num(o.cerchio->x) + ", y = " + num(o.cerchio->y) + ", r = " + num(o.cerchio->r) + " }";
        if (o.entro_s >= 0.0) s += ", entro_s = " + num(o.entro_s, 0);
        if (!o.primario) s += ", primario = false";
        if (!o.colpire) s += ", colpire = false";
        if (o.tipo == "contatto" || (o.tipo == "zona_inviolata" && o.minuti > 0.0)) s += ", minuti = " + num(o.minuti, 1);
        if (o.distanza_m > 0.0) s += ", distanza_m = " + num(o.distanza_m, 0);
        if (o.tipo == "zona_inviolata" && !o.parte.empty()) s += ", parte = " + stringa(o.parte);
        if (o.tipo == "variabile" && o.indice >= 0) s += ", indice = " + intero(VAR_AUTORE + o.indice) + ", almeno = " + intero(std::max(o.minimo, 1));
        if (o.tipo == "tlam" && !o.missione.empty()) s += ", missione = " + stringa(o.missione);
        if (o.tipo == "tlam" && !o.finestra) s += ", finestra = false";
        if (o.tipo == "scatola" && !o.massima.empty()) s += ", massima = " + stringa(o.massima);
        if ((o.tipo == "telemetro" || o.tipo == "rotta_stimata") && o.errore > 0.0) s += ", errore = " + num(o.errore, 3);
        if (o.tipo == "rotta_stimata" && o.da_tma) s += ", da_tma = true";
        s += ",\n      testo = { it = " + stringa(o.testo[IT]) + ", en = " + stringa(o.testo[EN]) + " } },\n";
    }
    s += "  },\n";
    for (int l = 0; l < 2; ++l) {
        s += std::string(l == IT ? "  briefing = {\n    it = { " : "    en = { ");
        for (std::size_t i = 0; i < sc.briefing[static_cast<std::size_t>(l)].size(); ++i)
            s += (i ? ", " : "") + stringa(sc.briefing[static_cast<std::size_t>(l)][i]);
        s += " },\n";
    }
    s += "  },\n";
    s += "  esiti = {\n    [20] = { it = " + stringa(sc.esito_fine[IT]) + ", en = " + stringa(sc.esito_fine[EN]) + " },\n";
    s += "    [21] = { it = " + stringa(sc.esito_tempo[IT]) + ", en = " + stringa(sc.esito_tempo[EN]) + " },\n";
    for (const auto* a : fini) s += "    [" + intero(a->codice) + "] = { it = " + stringa(a->testo[IT]) + ", en = " + stringa(a->testo[EN]) + " },\n";
    s += "  },\n";
    if (sc.radio) s += radio_lua(sc);
    if (sc.meteo_fisso) s += meteo_lua(*sc.meteo_fisso);
    if (!sc.mine.empty()) {   // un campo gia' posato (Enhanced, docs/09 §5.1c)
        s += "  mine = {\n";
        for (const auto& mn : sc.mine)
            s += "    { tipo = " + stringa(mn.tipo) + ", x = " + num(mn.pos.x) + ", y = " + num(mn.pos.y) + ", lato = " + stringa(mn.lato) +
                 (mn.armata ? "" : ", armata = false") + (mn.sicura ? ", sicura = true" : "") + (mn.comandata ? ", comandata = true" : "") +
                 (mn.gruppo.empty() ? "" : ", gruppo = " + stringa(mn.gruppo)) + " },\n";
        s += "  },\n";
    }
    if (sc.danni_iniziali) s += "  " + danni_iniziali_lua(*sc.danni_iniziali, [](double v) { return num(v, 4); }) + ",\n";
    if (sc.esercitazione) s += "  esercitazione = true,\n";
    if (sc.situazione_luogo)   // M7: la situazione di partenza fra il luogo e le altre due parti
        s += "  situazione_luogo = { usa = " + stringa(sc.situazione_luogo->usa) + ", urss = " + stringa(sc.situazione_luogo->urss) + " },\n";
    if (sc.navigazione)
        s += "  navigazione = { sigma_iniziale_nm = " + num(sc.navigazione->sigma_iniziale_nm) +
             (sc.navigazione->realistica_di_serie ? ", realistica_di_serie = true" : "") + " },\n";
    if (!sc.tlam.empty()) {   // la libreria di bordo dei TLAM (docs/21 §11.8): tot_s dall'inizio della missione
        s += "  tlam = {\n";
        for (const auto& x : sc.tlam)
            s += "    { id = " + stringa(x.id) + ", bersaglio = " + stringa(x.bersaglio) + ", plp = { x = " + num(x.plp.x) + ", y = " +
                 num(x.plp.y) + " }, landfall = { x = " + num(x.landfall.x) + ", y = " + num(x.landfall.y) + " }, tot_s = " + num(x.tot_s) +
                 ", finestra_s = " + num(x.finestra_s) + ", tratta_terra_m = " + num(x.tratta_terra_m) + " },\n";
        s += "  },\n";
    }
    if (const auto propri = messaggi_propri(sc); !propri.empty()) {   // messaggi di bordo propri: codice 1000 + n
        s += "  messaggi = {\n";
        for (std::size_t n = 0; n < propri.size(); ++n)
            s += "    [" + intero(static_cast<int>(n) + 1) + "] = { it = " + stringa(propri[n]->testo[IT]) + ", en = " + stringa(propri[n]->testo[EN]) + " },\n";
        s += "  },\n";
    }
    if (!sc.rapida) s += blocco_editor(sc);
    s += "  script = script,\n";
    // unita'
    s += "  varianti = {\n    { probabilita = 1, unita = {\n";
    for (const auto& u : sc.unita) {
        const Vec2 pos = u.entra_con.empty() ? u.pos : parcheggio(u);
        s += "      { id = " + intero(u.id) + ", classe = " + stringa(u.classe) + ", categoria = " + stringa(u.categoria) +
             ", schieramento = " + stringa(u.parte) + ", flag = " + intero(u.flag) + (u.giocatore ? ", giocatore = true" : "") + ",\n";
        s += "        posizione = { x = " + num(pos.x) + ", y = " + num(pos.y) + " }, quota_m = " + num(u.quota) +
             ", rotta = " + num(u.rotta) + ", velocita_kn = " + num(u.entra_con.empty() ? u.velocita : 0.0, 2) +
             (u.batteria ? ", batteria = " + num(*u.batteria, 3) : std::string()) +
             (u.stato.empty() ? std::string() : ", stato = " + stringa(u.stato)) +
             (u.sonar_speciale ? ", " + sonar_speciale_lua(*u.sonar_speciale, [](double v) { return num(v, 4); }) : std::string()) +
             (u.armi_esercitazione ? ", armi_esercitazione = true" : "") + (u.emcon.empty() ? std::string() : ", emcon = " + stringa(u.emcon)) +
             (u.luci.empty() ? std::string() : ", luci = " + stringa(u.luci)) +
             (u.cisterne.empty() ? std::string() : ", cisterne = " + stringa(u.cisterne)) +
             ",\n";
        s += "        " + modello_lua(u, sc, d) + ",\n";
        const bool rotta = !u.punti.empty() && u.entra_con.empty();   // la riserva parte ferma: la rotta la da' l'ingresso
        if (rotta)
            s += "        rotta_punti = { { x = " + num(u.punti[0].x) + ", y = " + num(u.punti[0].y) + " } }, rotta_ciclica = " +
                 (u.ciclica ? "true" : "false") + ",\n";
        const std::string arrivo = !rotta ? "nil"
                                   : u.punti.size() > 1 ? "\"R_" + intero(u.id) + "_p2\""
                                   : (u.ciclica ? "\"R_" + intero(u.id) + "_p1\"" : u.esce ? "\"R_esce\"" : "nil");
        s += "        gestori = { bersaglio = " + (u.comportamento.empty() || u.giocatore ? std::string("nil") : stringa(u.comportamento)) +
             ", reazione = nil, arrivo = " + arrivo + " } },";
        if (!u.ruolo.empty()) s += "   -- " + u.ruolo + (u.nascosta ? " (non nel briefing)" : "");
        if (!u.entra_con.empty()) s += (u.ruolo.empty() ? "   -- " : ", ") + std::string("di riserva: entra con la regola ") + u.entra_con;
        s += "\n";
    }
    s += "    },\n    timer = {\n";
    s += "      { dopo_s = 20, unita = 0, gestore = \"R_chiusura\" },\n";
    s += "      { dopo_s = " + num(sc.durata_s, 0) + ", unita = 0, gestore = \"R_tempo\" },\n";
    for (const auto& u : sc.unita) {
        const int p = u.parametri.paranoia.value_or(par);
        if (u.comportamento == "R_nave" && (v1 ? par >= 1 : p >= 1))
            s += "      { dopo_s = " + intero(30 + 7 * u.id) + ", unita = " + intero(u.id) + ", gestore = \"R_vigila\" },\n";
    }
    for (std::size_t n = 0; n < sc.regole.size(); ++n)
        s += "      { dopo_s = " + num(primo_controllo(sc.regole[n]), 0) + ", unita = " + intero(soggetto(sc, sc.regole[n])) +
             ", gestore = \"R_regola_" + intero(static_cast<int>(n)) + "\" },\n";
    s += "    },\n    variabili = { [10] = 0, [11] = 0 },\n    },\n  },\n";
    s += "}\n\n";
    s += "-- " + std::string(lato_ == USA ? "kit del 688" : "kit dell'Alfa") + " (data/motore/missioni_rapide/, dalle missioni originali)\n";
    s += k.script;
    s += "-- " + std::string(lato_ == USA ? "kit dell'Alfa" : "kit del 688") + ": i gestori di lancio usati dall'IA\n";
    s += d.kit[static_cast<std::size_t>(ostile_)].script;
    // costanti e comportamenti
    s += "\n-- Costanti della missione: regole d'ingaggio, paranoia (" + intero(par) + ") e difficolta' (" + intero(dif) + ")\n";
    s += "local ARMI = " + std::string(sc.situazione == "guerra" ? "2" : sc.situazione == "crisi" ? "1" : "0") +
         "            -- 0 pace (armi trattenute), 1 crisi (solo dopo un lancio del giocatore), 2 guerra\n";
    if (v1) {
        s += "local PING = " + num(PING[par], 2) + "          -- probabilita' di un ping a ogni valutazione del contatto\n";
        s += "local AGGANCIO = " + intero(AGGANCIO[par]) + "        -- aggiornamenti del contatto fra due valutazioni\n";
        s += std::string("local MEDIE = ") + (par >= 3 ? "true" : "false") + "       -- reagisce anche alle tracce medie\n";
        s += std::string("local INSEGUE = ") + (par >= 1 ? "true" : "false") + "      -- i sottomarini si avvicinano al contatto\n";
        s += "local VIGILA = " + intero(std::max(VIGILA[par], 60)) + "         -- s fra due controlli dell'allarme\n";
    }
    s += "local COLPO = " + intero(COLPO[dif - 1]) + "          -- soglia di ATTACK: riesce con probabilita' (18 - COLPO) / 18\n";
    if (v1) {
        s += "local PORTATA_NAVE = " + intero(2 + (dif >= 3 ? 1 : 0)) + "    -- nm delle armi antisom delle navi\n";
        s += "local PORTATA_SILURO = " + intero(3 + dif / 2) + "  -- nm di lancio dei sottomarini\n";
        s += "local SALVE = " + intero(1 + dif / 2) + "          -- lanci per sottomarino\n";
    }
    s += "local CACCIA = 0.8          -- telegrafo delle navi in caccia\n";
    s += "local AVVICINAMENTO = 0.25  -- telegrafo dei sottomarini in avvicinamento\n";
    if (v1) {
        // la parte vera, anche «neutrale»: chi e' nemico lo decidono i bit di schieramento (m688.bersaglio_nemico), e un
        // neutrale non lo e' per nessuno, quindi non arriva mai al gestore di lancio R_siluro_<parte>
        s += "-- per unita': parte, telegrafo di crociera, comportamento, rotta, quota d'evasione\n";
        s += "local UNITA = {\n";
        for (const auto& u : sc.unita) {
            if (u.giocatore || u.comportamento.empty()) continue;
            s += "  [" + intero(u.id) + "] = { parte = " + stringa(u.parte) + ", crociera = " +
                 num(crociera(u, d), 3) + ", comportamento = " + stringa(u.comportamento) + ", rotta = " +
                 (u.punti.empty() ? "false" : "true") + ", profonda = " + num(u.profonda) + " },\n";
        }
        s += "}\n\n";
        s += comportamenti_rapide(sc.rapida ? sc.rapida->versione : VERSIONE_RAPIDE);
    } else {
        s += "-- per unita': parte, telegrafo di crociera, comportamento, rotta, quota d'evasione e i parametri del\n";
        s += "-- comportamento (paranoia 0-3, il minimo dell'allerta; salve, portata e distanza del pedinamento in nm)\n";
        s += "local UNITA = {\n";
        for (const auto& u : sc.unita) {
            if (u.giocatore) continue;
            const int p = std::clamp(u.parametri.paranoia.value_or(par), 0, 3);
            const double portata = u.parametri.portata_nm.value_or(u.categoria == "sottomarino" ? 3 + dif / 2 : 2 + (dif >= 3 ? 1 : 0));
            s += "  [" + intero(u.id) + "] = { parte = " + stringa(u.parte) + ", crociera = " +
                 num(crociera(u, d), 3) + ", comportamento = " + stringa(u.comportamento) + ", rotta = " +
                 vero(!u.punti.empty()) + ", profonda = " + num(u.profonda) + ",\n      paranoia = " + intero(p) +
                 ", salve = " + intero(u.parametri.salve.value_or(1 + dif / 2)) +
                 ", portata = " + num(portata, 2) + ", distanza = " + num(u.parametri.distanza_nm.value_or(2.0), 2) + " },\n";
        }
        s += "}\n\n";
        s += COMPORTAMENTI_2;
    }
    s += "\n-- lanci dei sottomarini dell'IA: i gestori di lancio dei kit senza il conto del giocatore\n";
    s += lancio_ia(d.kit[USA], "mk48", "R_siluro_usa");
    s += lancio_ia(d.kit[URSS], "type53", "R_siluro_urss");
    if (std::any_of(sc.unita.begin(), sc.unita.end(), [](const UnitaScenario& u) { return u.parte == "luogo"; }))
        s += "script.R_siluro_luogo = script.R_siluro_urss   -- M7: i sottomarini del luogo lanciano il Type 53\n";
    s += "\n-- rotte: all'arrivo a ogni punto il successivo (le pattuglie ricominciano, i transiti escono)\n";
    for (const auto& u : sc.unita) {
        for (std::size_t i = 0; i < u.punti.size(); ++i) {
            const std::string prossimo = i + 1 < u.punti.size() ? "R_" + intero(u.id) + "_p" + intero(static_cast<int>(i) + 2)
                                         : u.ciclica                 ? "R_" + intero(u.id) + "_p1"
                                         : u.esce                    ? "R_esce"
                                                                     : "";
            s += "script.R_" + intero(u.id) + "_p" + intero(static_cast<int>(i) + 1) + " = function(u)\n";
            s += "  m688.vai_a(u, " + num(u.punti[i].x) + ", " + num(u.punti[i].y) + ", 30, " + stringa(prossimo) + ")\n";
            s += "  m688.salva_modello(u)\nend\n";
        }
    }
    s += "\n-- chiusura: codice 120 a missione conclusa, 121 a tempo scaduto (il giudizio lo danno gli obiettivi)\n";
    s += "script.R_chiusura = function(u)\n" + chiusura_lua(sc.chiusura) + "  m688.dopo(u, 20, \"R_chiusura\")\n  do return end\n  ::fine::\n  m688.messaggio(u, 120)\nend\n";
    s += "script.R_tempo = function(u)\n  m688.messaggio(u, 34)\n  m688.messaggio(u, 121)\nend\n";
    s += regole_lua(sc, d);
    s += "\nreturn missione\n";
    return s;
}

std::optional<ScenarioMissione> leggi_scenario(const std::string& testo) {
    lua_isolato::Stato stato;   // ambiente isolato come quello delle missioni, con un tetto di memoria e di istruzioni
    lua_State* L = stato.get();
    try {
        stato.esegui(testo, "=scenario");
    } catch (const std::runtime_error& e) {
        throw std::runtime_error(std::string("scenario: ") + e.what());
    }
    if (!lua_istable(L, -1)) throw std::runtime_error("scenario: il file non restituisce una tabella");
    Lettore l{L};
    const int m = lua_gettop(L);
    if (!l.tabella(m, "editor")) return std::nullopt;
    const int e = lua_gettop(L);
    if (l.intero(e, "formato_editor") > FORMATO_EDITOR)
        throw std::runtime_error("scenario: formato dell'editor " + std::to_string(l.intero(e, "formato_editor")) + ", piu' nuovo di questo gioco");
    if (!l.tabella(e, "scenario")) return std::nullopt;
    return leggi_tabella(l, lua_gettop(L));
}


ScenarioMissione scenario_da_originale(const std::string& cartella, const std::string& id, const std::string& dati, std::vector<std::string>* note) {
    std::vector<std::string> n;
    const auto leggi = [](const std::filesystem::path& p) {
        std::ifstream f(p, std::ios::binary);
        if (!f) throw std::runtime_error("impossibile aprire " + p.string());
        return std::string{std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};
    };
    const DatiScenario d = carica_dati_scenario(dati);
    lua_isolato::Stato stato;
    lua_State* L = stato.get();
    stato.esegui(leggi(std::filesystem::path(cartella) / (id + ".lua")), "=originale");
    if (!lua_istable(L, -1)) throw std::runtime_error(id + ": il file non restituisce una tabella");
    Lettore l{L};
    const int t = lua_gettop(L);
    ScenarioMissione s;
    s.id = "EDITOR_" + l.stringa(t, "id", id);
    s.titolo = l.testi(t, "titolo");
    s.teatro = l.stringa(t, "teatro");
    s.alfa = l.stringa(t, "lato") == "urss";
    if (l.tabella(t, "posa")) {
        if (l.tabella(lua_gettop(L), "centro")) {
            s.centro_lat = l.numero(lua_gettop(L), "lat");
            s.centro_lon = l.numero(lua_gettop(L), "lon");
            lua_pop(L, 1);
        }
        lua_pop(L, 1);
    }
    s.giorno = 15;
    s.mese = 10;
    s.ora_utc = 12.0;
    s.libreria = 2;
    s.esito_fine = d.esito_fine;
    s.esito_tempo = d.esito_tempo;
    if (l.stringa(t, "lato") == "due_giocatori") n.push_back("missione per due giocatori: si gioca col 688");
    // carico: dotazione nell'ordine delle famiglie, tubi come nell'originale
    if (l.tabella(t, "dotazione")) {
        for (const char* arma : {"mk48", "type53", "harpoon", "tomahawk", "sea_lance", "esca"})
            if (const int k = l.intero(lua_gettop(L), arma, 0); k > 0) s.carico.dotazione.push_back({arma, k});
        lua_pop(L, 1);
    }
    s.carico.tubi = l.stringhe(t, "tubi");
    if (s.carico.tubi.empty()) s.carico.tubi = s.alfa ? std::vector<std::string>{"type53", "type53", "type53", "type53", "type53", "esca", "esca"}
                                                      : std::vector<std::string>{"mk48", "mk48", "mk48", "mk48", "esca", "esca", "harpoon"};
    if (l.tabella(t, "briefing")) {
        s.briefing[IT] = l.stringhe(lua_gettop(L), "it");
        s.briefing[EN] = l.stringhe(lua_gettop(L), "en");
        lua_pop(L, 1);
    }
    // la variante piu' probabile
    int scelta = 0, numero = 0;
    double meglio = -1.0;
    l.elenco(t, "varianti", [&](int v) {
        ++numero;
        if (const double p = l.numero(v, "probabilita"); p > meglio) { meglio = p; scelta = numero; }
    });
    if (numero > 1) n.push_back("variante " + std::to_string(scelta) + " di " + std::to_string(numero) + ", la piu' probabile");
    int k = 0;
    std::vector<UnitaScenario> unita;
    l.elenco(t, "varianti", [&](int v) {
        if (++k != scelta) return;
        l.elenco(v, "unita", [&](int i) {
            UnitaScenario u;
            u.id = l.intero(i, "id");
            u.classe = l.stringa(i, "classe");
            u.categoria = l.stringa(i, "categoria");
            u.parte = l.stringa(i, "schieramento", "neutrale");
            u.flag = l.intero(i, "flag");
            u.giocatore = l.booleano(i, "giocatore");
            u.pos = l.punto(i, "posizione");
            u.quota = l.numero(i, "quota_m");
            u.rotta = l.numero(i, "rotta");
            u.velocita = l.numero(i, "velocita_kn");
            l.elenco(i, "rotta_punti", [&](int q) { u.punti.push_back({l.numero(q, "x"), l.numero(q, "y")}); });
            u.ciclica = l.booleano(i, "rotta_ciclica");
            u.non_combattente = (u.flag & 0x40) != 0;
            const bool categoria = u.categoria == "sottomarino" || u.categoria == "nave" || u.categoria == "aereo";
            if (!categoria || (u.flag & 0x80) || !d.modelli.count(u.giocatore ? "giocatore_" + u.classe : u.classe)) {
                n.push_back("unita' " + std::to_string(u.id) + " (" + u.classe + ") lasciata fuori: senza modello per l'editor");
                return;
            }
            if (u.giocatore) {
                u.classe = s.alfa ? "alfa" : "los_angeles";
                u.parte = s.alfa ? "urss" : "usa";
            }
            if (u.categoria == "sottomarino" && !u.giocatore) {
                u.comportamento = "R_sottomarino";
                const double massima = d.modelli.at(u.classe).quota_max;
                u.profonda = std::max(u.quota, std::min(massima * 0.8, u.quota + 150.0));
            } else if (u.categoria == "aereo") {
                u.comportamento = "R_aereo";
            } else if (u.categoria == "nave" && !u.non_combattente) {
                u.comportamento = "R_nave";
            }
            unita.push_back(u);
        });
    });
    // il giocatore per primo (e uno solo)
    std::stable_partition(unita.begin(), unita.end(), [](const UnitaScenario& u) { return u.giocatore; });
    for (std::size_t i = 1; i < unita.size(); ++i)
        if (unita[i].giocatore) {
            unita[i].giocatore = false;
            n.push_back("secondo giocatore (unita' " + std::to_string(unita[i].id) + ") diventato un'unita' dell'IA");
            if (unita[i].categoria == "sottomarino") unita[i].comportamento = "R_sottomarino";
        }
    if (unita.empty() || !unita[0].giocatore) throw std::runtime_error(id + ": nessun battello del giocatore");
    s.unita = std::move(unita);
    std::set<int> ids;
    for (const auto& u : s.unita) ids.insert(u.id);
    // gli obiettivi che non dipendono dagli script
    const auto file_obiettivi = std::filesystem::path(cartella) / "obiettivi" / (id + ".lua");
    if (std::filesystem::exists(file_obiettivi)) {
        lua_isolato::Stato so;
        so.esegui(leggi(file_obiettivi), "=obiettivi");
        Lettore lo{so.get()};
        const int o = lua_gettop(so.get());
        static const std::set<std::string> TIPI = {"affondare", "proteggere", "raggiungere", "scoprire", "non_essere_scoperto", "sopravvivere", "minare",
                                                   "contatto"};
        lo.elenco(o, "obiettivi", [&](int i) {
            ObiettivoScenario x;
            x.id = lo.stringa(i, "id");
            x.tipo = lo.stringa(i, "tipo");
            if (!TIPI.count(x.tipo)) {
                n.push_back("obiettivo «" + x.id + "» lasciato fuori: dipende dagli script originali");
                return;
            }
            for (const int u : lo.interi(i, "unita"))
                if (ids.count(u)) x.unita.push_back(u);
            if ((x.tipo == "affondare" || x.tipo == "proteggere" || x.tipo == "scoprire") && x.unita.empty()) {
                n.push_back("obiettivo «" + x.id + "» lasciato fuori: le sue unita' non sono in questa variante");
                return;
            }
            x.minimo = lo.intero(i, "minimo", -1);
            if (x.minimo > static_cast<int>(x.unita.size())) x.minimo = static_cast<int>(x.unita.size());
            if (lo.tabella(i, "cerchio")) {
                lua_pop(so.get(), 1);
                x.cerchio = lo.cerchio(i, "cerchio");
            }
            x.entro_s = lo.numero(i, "entro_s", -1.0);
            x.primario = lo.booleano(i, "primario", true);
            x.colpire = lo.booleano(i, "colpire", true);
            x.testo = lo.testi(i, "testo");
            s.obiettivi.push_back(x);
        });
    }
    if (std::none_of(s.obiettivi.begin(), s.obiettivi.end(), [](const ObiettivoScenario& o) { return o.tipo == "sopravvivere"; })) {
        ObiettivoScenario vivo;
        vivo.id = "vivo";
        vivo.tipo = "sopravvivere";
        vivo.testo = {"Riportare a casa il battello", "Bring the boat home"};
        s.obiettivi.push_back(vivo);
    }
    for (const auto& o : s.obiettivi)
        if (o.tipo == "affondare") {
            s.chiusura.tipo = ChiusuraScenario::Tipo::unita_finite;
            s.chiusura.unita.insert(s.chiusura.unita.end(), o.unita.begin(), o.unita.end());
        }
    n.insert(n.begin(), "la logica degli script originali non si riprende: comportamenti, durata e fine sono quelli dell'editor");
    if (note) *note = std::move(n);
    return s;
}

std::filesystem::path cartella_missioni_editor() { return cartella_fascicoli().parent_path() / "missioni_editor"; }

std::vector<ScenarioInElenco> elenco_scenari(const std::filesystem::path& cartella) {
    std::vector<ScenarioInElenco> v;
    std::error_code ec;
    for (const auto& e : std::filesystem::directory_iterator(cartella, ec)) {
        if (e.path().extension() != ".lua") continue;
        std::ifstream f(e.path(), std::ios::binary);
        const std::string testo{std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};
        try {
            if (const auto s = leggi_scenario(testo)) v.push_back({e.path(), s->id, s->autore, s->teatro, s->titolo, s->alfa});
        } catch (const std::exception&) {
            // un file rovinato non blocca l'elenco
        }
    }
    std::sort(v.begin(), v.end(), [](const auto& a, const auto& b) { return a.file.filename() < b.file.filename(); });
    return v;
}

// --- verifica ----------------------------------------------------------------------------------------------------------

std::vector<AvvisoScenario> verifica(const ScenarioMissione& s, const Fondale& f, const DatiScenario& d) {
    using G = AvvisoScenario::Gravita;
    std::vector<AvvisoScenario> v;
    const auto avviso = [&](G g, const std::string& codice, const std::string& testo, int unita = -1, const std::string& rif = "") {
        v.push_back({g, codice, testo, unita, rif});
    };
    const double mx = f.larghezza() * f.cella_m() / 2.0, my = f.altezza() * f.cella_m() / 2.0;
    const auto in_carta = [&](Vec2 p) { return std::fabs(p.x) < mx && std::fabs(p.y) < my; };
    const auto minima = [&](const UnitaScenario& u) {
        if (u.categoria == "sottomarino") return std::max(10.0, u.quota + 5.0);   // il Baltico delle originali: 16 m
        const auto it = d.pescaggi.find(u.classe);
        return std::max(8.0, (it == d.pescaggi.end() ? 8.0 : it->second) + 3.0);
    };
    // missione
    if (s.situazione != "pace" && s.situazione != "crisi" && s.situazione != "guerra")
        avviso(G::errore, "situazione", "situazione sconosciuta: " + s.situazione);
    if (s.situazione_luogo)
        for (const std::string* x : {&s.situazione_luogo->usa, &s.situazione_luogo->urss})
            if (*x != "pace" && *x != "crisi" && *x != "guerra") avviso(G::errore, "situazione_luogo", "situazione col luogo sconosciuta: " + *x);
    if (s.paranoia < 0 || s.paranoia > 3) avviso(G::errore, "paranoia", "paranoia fuori da 0-3");
    if (s.difficolta < 1 || s.difficolta > 5) avviso(G::errore, "difficolta", "difficolta' fuori da 1-5");
    if (s.durata_s < 600.0) avviso(G::attenzione, "durata", "la missione dura meno di 10 minuti");
    if (s.carico.tubi.empty()) avviso(G::errore, "tubi", "nessun tubo di lancio");
    // unita'
    int giocatori = 0;
    std::set<int> ids;
    for (std::size_t i = 0; i < s.unita.size(); ++i) {
        const auto& u = s.unita[i];
        if (u.giocatore) {
            ++giocatori;
            if (i != 0) avviso(G::errore, "giocatore_non_primo", "il battello del giocatore deve essere la prima unita'", u.id);
        }
        if (!ids.insert(u.id).second) avviso(G::errore, "id_doppio", "due unita' con l'id " + std::to_string(u.id), u.id);
        if (!d.modelli.count(u.giocatore ? "giocatore_" + u.classe : u.classe))
            avviso(G::errore, "classe", "classe senza modello del livello classico: " + u.classe, u.id);
        if (u.categoria != "sottomarino" && u.categoria != "nave" && u.categoria != "aereo")
            avviso(G::errore, "categoria", "categoria sconosciuta: " + u.categoria, u.id);
        if (u.comportamento != "" && u.comportamento != "R_nave" && u.comportamento != "R_sottomarino" && u.comportamento != "R_aereo" &&
            !((u.comportamento == "R_pedina" || u.comportamento == "R_pedina_vicino") && s.libreria == 2))
            avviso(G::errore, "comportamento", "comportamento sconosciuto: " + u.comportamento, u.id);
        if (u.categoria != "aereo") {
            if (!in_carta(u.pos) || f.profondita(u.pos) < minima(u))
                avviso(G::errore, "terra", u.classe + " " + std::to_string(u.id) + " sulla terra o in acqua troppo bassa", u.id);
            Vec2 a = u.pos;
            for (const auto& p : u.punti) {
                const double l = distanza(a, p);
                const int n = std::max(1, static_cast<int>(l / 1000.0));
                bool libera = true;
                for (int k = 0; k <= n && libera; ++k) {
                    const double t = static_cast<double>(k) / n;
                    const Vec2 q{a.x + (p.x - a.x) * t, a.y + (p.y - a.y) * t};
                    libera = in_carta(q) && f.profondita(q) >= minima(u);
                }
                if (!libera) {
                    avviso(G::attenzione, "rotta_su_terra", "la rotta di " + u.classe + " " + std::to_string(u.id) + " passa sulla terra o nel bassofondo", u.id);
                    break;
                }
                a = p;
            }
        }
        if (!u.entra_con.empty() && indice_regola(s, u.entra_con) < 0)
            avviso(G::errore, "riserva", "unita' di riserva con una regola sconosciuta: " + u.entra_con, u.id);
        if (u.giocatore && !u.entra_con.empty()) avviso(G::errore, "riserva", "il giocatore non puo' essere di riserva", u.id);
        if (!u.stato.empty() && u.stato != "avaria" && u.stato != "incagliata" && u.stato != "relitto")
            avviso(G::errore, "stato", "stato sconosciuto: " + u.stato + " (avaria, incagliata o relitto)", u.id);
        if (!u.stato.empty() && u.giocatore) avviso(G::errore, "stato", "il battello del giocatore non parte in avaria", u.id);
        if (!u.emcon.empty() && u.emcon != "silenzio" && u.emcon != "navigazione" && u.emcon != "ricerca" && u.emcon != "intermittente" &&
            u.emcon != "ingaggio")
            avviso(G::errore, "emcon", "EMCON sconosciuto: " + u.emcon + " (silenzio, navigazione, ricerca, intermittente, ingaggio)", u.id);
        if (!u.luci.empty()) {   // le luci di navigazione (docs/27)
            bool ok = false;
            luci::stato_da_nome(u.luci, &ok);
            if (!ok) avviso(G::errore, "luci", "luci sconosciute: " + u.luci + " (moto, fonda, pesca, manovra_limitata, non_governa, spente)", u.id);
        }
        if (!u.cisterne.empty() && u.cisterne != "zavorra" && u.cisterne != "zavorra_inerte")
            avviso(G::errore, "cisterne", "cisterne sconosciute: " + u.cisterne + " (zavorra, zavorra_inerte; vuoto: cariche)", u.id);
        else if (!u.cisterne.empty() && !u.classe.starts_with("petroliera"))
            avviso(G::attenzione, "cisterne", "le cisterne contano solo per le petroliere", u.id);
        if (!u.giocatore && u.id == 1 && s.libreria == 2)   // missione.cpp: gli id 0 e 1 sono i due giocatori dell'originale
            avviso(G::attenzione, "id_1", "l'id 1 e' del secondo giocatore dell'originale: meglio partire da 2", u.id);
    }
    if (giocatori == 0) avviso(G::errore, "senza_giocatore", "nessun battello del giocatore");
    if (giocatori > 1) avviso(G::errore, "giocatori", "piu' di un battello del giocatore");
    if (!s.unita.empty() && s.unita[0].giocatore && s.unita[0].classe != (s.alfa ? "alfa" : "los_angeles"))
        avviso(G::attenzione, "classe_giocatore", "il battello del giocatore non e' della parte scelta");
    // obiettivi
    static const std::set<std::string> TIPI = {"affondare", "proteggere", "raggiungere", "scoprire", "non_essere_scoperto",
                                               "sopravvivere", "variabile", "esito", "minare", "contatto", "foto", "zona_inviolata",
                                               "colpire", "tlam", "scatola", "identificare", "telemetro", "rotta_stimata"};
    for (const auto& o : s.obiettivi) {
        if (!TIPI.count(o.tipo)) avviso(G::errore, "obiettivo_tipo", "obiettivo di tipo sconosciuto: " + o.tipo, -1, o.id);
        if (o.tipo == "scatola" && !o.massima.empty() && o.massima != "piccola" && o.massima != "media" && o.massima != "grande")
            avviso(G::errore, "obiettivo_scatola", "l'obiettivo " + o.id + " ha una taglia sconosciuta: " + o.massima, -1, o.id);
        if (o.tipo == "tlam" && !o.missione.empty() &&
            std::none_of(s.tlam.begin(), s.tlam.end(), [&](const TlamScenario& x) { return x.id == o.missione; }))
            avviso(G::errore, "obiettivo_tlam", "l'obiettivo " + o.id + " nomina la missione TLAM " + o.missione + " che non e' in libreria", -1, o.id);
        for (const int id : o.unita)
            if (!ids.count(id)) avviso(G::errore, "obiettivo_unita", "l'obiettivo " + o.id + " nomina l'unita' " + std::to_string(id) + " che non c'e'", -1, o.id);
        if ((o.tipo == "affondare" || o.tipo == "proteggere" || o.tipo == "scoprire" || o.tipo == "contatto" || o.tipo == "foto" ||
             o.tipo == "colpire" || o.tipo == "identificare") &&
            o.unita.empty())
            avviso(G::errore, "obiettivo_senza_unita", "l'obiettivo " + o.id + " non ha unita'", -1, o.id);
        if (o.tipo == "raggiungere" && !o.cerchio && (o.unita.empty() || o.distanza_m <= 0.0))   // una zona, o un'unita' in moto
            avviso(G::errore, "obiettivo_senza_zona", "l'obiettivo " + o.id + " non ha la zona ne' l'unita' con la distanza", -1, o.id);
        if (o.tipo == "zona_inviolata" && !o.cerchio) avviso(G::errore, "obiettivo_senza_zona", "l'obiettivo " + o.id + " non ha la zona", -1, o.id);
        if (o.tipo == "zona_inviolata" && !o.parte.empty() && o.parte != "ostile" && o.parte != "amica" && o.parte != "neutrale")
            avviso(G::errore, "obiettivo_parte", "l'obiettivo " + o.id + " ha una parte sconosciuta: " + o.parte, -1, o.id);
        if (o.tipo == "contatto" && o.minuti <= 0.0) avviso(G::errore, "obiettivo_minuti", "l'obiettivo " + o.id + " non dice quanti minuti", -1, o.id);
        // il telemetro e la rotta stimata (docs/25, I13 e I14): l'errore massimo, relativo per la distanza, in gradi per la rotta
        if ((o.tipo == "telemetro" || o.tipo == "rotta_stimata") && o.errore <= 0.0)
            avviso(G::errore, "obiettivo_errore", "l'obiettivo " + o.id + " non dice l'errore massimo", -1, o.id);
        if (o.tipo == "telemetro" && o.errore >= 1.0)
            avviso(G::attenzione, "obiettivo_errore", "l'obiettivo " + o.id + ": l'errore del telemetro e' una frazione (0,1 = 10%)", -1, o.id);
        if (o.tipo == "rotta_stimata" && o.errore > 90.0)
            avviso(G::attenzione, "obiettivo_errore", "l'obiettivo " + o.id + ": l'errore sulla rotta oltre 90 gradi non distingue niente", -1, o.id);
        if ((o.tipo == "identificare" || o.tipo == "telemetro" || o.tipo == "rotta_stimata") && o.minimo == 0)
            avviso(G::attenzione, "obiettivo_minimo", "l'obiettivo " + o.id + " ha minimo 0: e' gia' raggiunto", -1, o.id);
        if (o.tipo == "variabile" && o.indice < 0) avviso(G::errore, "obiettivo_variabile", "l'obiettivo " + o.id + " non dice quale variabile", -1, o.id);
        if (o.cerchio && !in_carta({o.cerchio->x, o.cerchio->y}))
            avviso(G::attenzione, "obiettivo_fuori", "la zona dell'obiettivo " + o.id + " e' fuori dalla carta", -1, o.id);
        if (o.tipo == "minare" && o.cerchio && f.profondita({o.cerchio->x, o.cerchio->y}) < 15.0)
            avviso(G::attenzione, "mine_fondale", "il campo minato " + o.id + " e' in acqua troppo bassa per le mine", -1, o.id);
    }
    // aerei
    for (const auto& a : s.aerei_asw) {
        const UnitaScenario* u = unita_di(s, a.unita);
        if (!u || u->categoria != "aereo") avviso(G::errore, "aereo", "la voce ASW " + std::to_string(a.unita) + " non e' un aereo", a.unita);
        if (a.base_unita >= 0 && !unita_di(s, a.base_unita)) avviso(G::errore, "aereo_base", "nave madre sconosciuta", a.unita);
        if (a.base_unita < 0 && !a.base) avviso(G::attenzione, "aereo_base", "aereo ASW senza base: parte dalla sua posizione", a.unita);
    }
    // zone e regole
    std::set<std::string> zone;
    for (const auto& z : s.zone) {
        if (!zone.insert(z.id).second) avviso(G::errore, "zona_doppia", "due zone con l'id " + z.id, -1, z.id);
        if (!in_carta({z.cerchio.x, z.cerchio.y})) avviso(G::attenzione, "zona_fuori", "la zona " + z.id + " e' fuori dalla carta", -1, z.id);
        if (!z.radio.empty() && z.radio != "azione" && z.radio != "libera_asw" && z.radio != "protetta")
            avviso(G::errore, "zona_radio", "la zona " + z.id + " ha un tipo radio sconosciuto: " + z.radio + " (azione, libera_asw, protetta)", -1, z.id);
        if (!z.radio.empty() && !s.radio) avviso(G::attenzione, "zona_radio", "la zona " + z.id + " e' della radio, ma la missione non ha la radio", -1, z.id);
    }
    std::set<std::string> regole;
    for (const auto& r : s.regole) {
        if (!regole.insert(r.id).second) avviso(G::errore, "regola_doppia", "due regole con l'id " + r.id, -1, r.id);
        if (r.allora.empty()) avviso(G::attenzione, "regola_vuota", "la regola " + r.id + " non fa niente", -1, r.id);
        std::set<int> soggetti;   // le unita' di cui la regola guarda la posizione: una sola (la regola gira su di lei)
        for (const auto& c : r.quando) {
            using T = CondizioneScenario::Tipo;
            if ((c.tipo == T::giocatore_in_zona || c.tipo == T::giocatore_fuori_zona || c.tipo == T::unita_in_zona) && !zona_di(s, c.zona))
                avviso(G::errore, "regola_zona", "la regola " + r.id + " usa la zona sconosciuta " + c.zona, -1, r.id);
            if ((c.tipo == T::unita_colpita || c.tipo == T::unita_fuori || c.tipo == T::unita_in_zona || c.tipo == T::contatto_su ||
                 c.tipo == T::vicino_al_cavo || c.tipo == T::unita_in_superficie || c.tipo == T::unita_in_avaria ||
                 c.tipo == T::unita_rimorchiata ||
                 c.tipo == T::senza_contatto_su || c.tipo == T::ping_vicino || c.tipo == T::unita_vede || c.tipo == T::unita_non_vede ||
                 c.tipo == T::unita_classifica || c.tipo == T::giocatore_vicino_a || (c.tipo == T::ostile_ha_lanciato && c.unita >= 0)) &&
                !ids.count(c.unita))
                avviso(G::errore, "regola_unita", "la regola " + r.id + " usa l'unita' sconosciuta " + std::to_string(c.unita), -1, r.id);
            if ((c.tipo == T::unita_vede || c.tipo == T::unita_non_vede || c.tipo == T::unita_classifica) && c.bersaglio >= 0 &&
                !ids.count(c.bersaglio))
                avviso(G::errore, "regola_unita", "la regola " + r.id + " osserva l'unita' sconosciuta " + std::to_string(c.bersaglio), -1, r.id);
            if (c.tipo == T::variabile && c.confronto != "=" && c.confronto != ">" && c.confronto != "<")   // lo scrittore li farebbe «=»
                avviso(G::errore, "regola_confronto", "la regola " + r.id + " usa il confronto sconosciuto " + c.confronto + " (=, >, <)", -1, r.id);
            if (c.tipo == T::giocatore_ha_trasmesso && !c.trasmissione.empty() && c.trasmissione != "contatto" && c.trasmissione != "attacco" &&
                c.trasmissione != "situazione" && c.trasmissione != "conferma" && c.trasmissione != "richiesta" && c.trasmissione != "previsione")
                avviso(G::errore, "regola_trasmissione", "la regola " + r.id + " aspetta la trasmissione sconosciuta " + c.trasmissione, -1, r.id);
            if (c.tipo == T::evento) {
                bool noto = false;
                for (int e = 0; e < static_cast<int>(TipoEvento::numero); ++e) noto = noto || c.evento == nome_evento(static_cast<TipoEvento>(e));
                if (!noto) avviso(G::errore, "regola_evento", "la regola " + r.id + " aspetta l'evento sconosciuto " + c.evento, -1, r.id);
            }
            if (c.tipo == T::regola_scattata && indice_regola(s, c.regola) < 0)
                avviso(G::errore, "regola_regola", "la regola " + r.id + " aspetta la regola sconosciuta " + c.regola, -1, r.id);
            if (c.tipo == T::unita_in_zona) soggetti.insert(c.unita);
            if (c.tipo == T::obiettivo_compiuto &&
                std::none_of(s.obiettivi.begin(), s.obiettivi.end(), [&](const ObiettivoScenario& o) { return o.id == c.obiettivo; }))
                avviso(G::errore, "regola_obiettivo", "la regola " + r.id + " aspetta l'obiettivo sconosciuto " + c.obiettivo, -1, r.id);
            // le condizioni sul giocatore: la regola deve girare su di lui
            if ((c.tipo == T::giocatore_in_zona || c.tipo == T::giocatore_fuori_zona || c.tipo == T::giocatore_sotto_nodi ||
                 c.tipo == T::giocatore_a_quota || c.tipo == T::giocatore_in_superficie || c.tipo == T::giocatore_alberi_alzati ||
                 c.tipo == T::contatto_su || c.tipo == T::senza_contatto_su || c.tipo == T::vicino_al_cavo ||
                 c.tipo == T::giocatore_sopra_nodi || c.tipo == T::giocatore_vicino_a) && !s.unita.empty())
                soggetti.insert(s.unita[0].id);
        }
        if (soggetti.size() > 1)
            avviso(G::errore, "regola_soggetti", "la regola " + r.id + " guarda unita' diverse (zone o stato del giocatore): dividila in due", -1, r.id);
        for (const auto& a : r.allora) {
            using T = AzioneScenario::Tipo;
            if ((a.tipo == T::comportamento || a.tipo == T::vai_a || a.tipo == T::entra || a.tipo == T::posa_mina || a.tipo == T::avaria ||
                 a.tipo == T::rimorchia || a.tipo == T::molla_rimorchio || a.tipo == T::esercitazione || a.tipo == T::attacca_unita ||
                 a.tipo == T::lancia_missile || a.tipo == T::quota || a.tipo == T::rumore || a.tipo == T::togli) &&
                !ids.count(a.unita))
                avviso(G::errore, "regola_unita", "la regola " + r.id + " muove l'unita' sconosciuta " + std::to_string(a.unita), -1, r.id);
            if (a.tipo == T::salvati && (a.indice <= 0 || a.valore < 0 || a.valore > a.indice))
                avviso(G::errore, "regola_salvati", "la regola " + r.id + " salva " + std::to_string(a.valore) + " uomini su " +
                                                        std::to_string(a.indice) + " (0 <= salvati <= in acqua, in acqua > 0)", -1, r.id);
            if (a.tipo == T::attacca_unita && (!ids.count(a.bersaglio) || a.bersaglio == a.unita))
                avviso(G::errore, "regola_attacco", "la regola " + r.id + " fa attaccare un'unita' sconosciuta o se stessa", -1, r.id);
            if (a.tipo == T::lancia_missile && ((a.bersaglio >= 0 && !ids.count(a.bersaglio)) || a.bersaglio == a.unita))
                avviso(G::errore, "regola_attacco", "la regola " + r.id + " fa tirare un missile su un'unita' sconosciuta o su se stessa", -1, r.id);
            if (a.tipo == T::comanda_mine &&
                std::none_of(s.mine.begin(), s.mine.end(), [&](const MinaScenario& mn) { return mn.comandata && (a.zona.empty() || mn.gruppo == a.zona); }))
                avviso(G::errore, "regola_mine", "la regola " + r.id + " comanda mine che non ci sono (gruppo «" + a.zona + "»)", -1, r.id);
            if (a.tipo == T::rimorchia && (!ids.count(a.bersaglio) || a.bersaglio == a.unita))
                avviso(G::errore, "regola_rimorchio", "la regola " + r.id + " rimorchia un'unita' sconosciuta o se stessa", -1, r.id);
            if (a.tipo == T::entra)
                if (const UnitaScenario* u = unita_di(s, a.unita); u && u->entra_con.empty())
                    avviso(G::attenzione, "regola_entra", "la regola " + r.id + " fa entrare l'unita' " + std::to_string(a.unita) + ", che non e' di riserva", -1, r.id);
            if (a.tipo == T::sposta_zona && !zona_di(s, a.zona))
                avviso(G::errore, "regola_zona", "la regola " + r.id + " sposta la zona sconosciuta " + a.zona, -1, r.id);
            if ((a.tipo == T::comportamento) && (a.comportamento == "R_pedina" || a.comportamento == "R_pedina_vicino") && s.libreria != 2)
                avviso(G::errore, "comportamento", "il pedinamento c'e' solo nella libreria 2", -1, r.id);
            if (a.tipo == T::fine && (a.codice < 30 || a.codice > 99))
                avviso(G::errore, "regola_esito", "gli esiti delle regole vanno da 30 a 99", -1, r.id);
            if (a.tipo == T::radio && (!s.radio || std::none_of(s.radio->messaggi.begin(), s.radio->messaggi.end(),
                                                                 [&](const MessaggioScenario& m) { return m.id == a.zona; })))
                avviso(G::errore, "regola_radio", "la regola " + r.id + " manda il messaggio radio sconosciuto " + a.zona, -1, r.id);
            if (a.tipo == T::situazione && !a.fra_a.empty()) {
                const auto parte = [](const std::string& p) { return p == "usa" || p == "urss" || p == "luogo"; };
                if (!parte(a.fra_a) || !parte(a.fra_b) || a.fra_a == a.fra_b)
                    avviso(G::errore, "regola_situazione", "la regola " + r.id + " cambia la situazione fra parti sconosciute o uguali", -1, r.id);
            }
            if (a.tipo == T::situazione && a.comportamento != "pace" && a.comportamento != "crisi" && a.comportamento != "guerra")
                avviso(G::errore, "regola_situazione", "la regola " + r.id + " cambia la situazione in " + a.comportamento, -1, r.id);
            if (a.tipo == T::messaggio && a.testo[IT].empty() && (a.codice < 0 || a.codice >= 100))
                avviso(G::errore, "regola_messaggio", "i messaggi di bordo vanno da 0 a 99", -1, r.id);
            if (a.tipo == T::variabile && (a.indice < 0 || a.indice > 99))
                avviso(G::errore, "regola_variabile", "le variabili dell'autore vanno da 0 a 99", -1, r.id);
        }
    }
    for (const auto& u : s.unita)
        if (!u.entra_con.empty()) {
            bool entra = false;
            for (const auto& r : s.regole)
                if (r.id == u.entra_con)
                    for (const auto& a : r.allora) entra = entra || (a.tipo == AzioneScenario::Tipo::entra && a.unita == u.id);
            if (!entra) avviso(G::attenzione, "riserva_senza_ingresso", "la regola " + u.entra_con + " non fa entrare l'unita' " + std::to_string(u.id), u.id);
        }
    // campo minato gia' posato: i tipi di carico.toml, in acqua sulla carta (le profondita' del tipo le controlla il motore)
    static const std::set<std::string> MINE = {"mk67", "mk60", "pmr1", "pmr2", "mdt"};
    for (const auto& mn : s.mine) {
        if (!MINE.count(mn.tipo)) avviso(G::errore, "mina_tipo", "mina di tipo sconosciuto: " + mn.tipo);
        if (!in_carta(mn.pos) || f.profondita(mn.pos) <= 0.0) avviso(G::errore, "mina_terra", "una mina " + mn.tipo + " e' sulla terra o fuori dalla carta");
    }
    // danni iniziali: i sistemi di nucleo/battello (le chiavi di sistema_da_chiave), efficienze fra 0 e 1, compartimenti nominati
    if (s.danni_iniziali) {
        const auto& di = *s.danni_iniziali;
        for (const auto& [k, eff] : di.sistemi) {
            if (!sistema_da_chiave(k))
                avviso(G::errore, "danni_sistema", "danni iniziali: sistema sconosciuto " + k);
            if (eff < 0.0 || eff > 1.0) avviso(G::errore, "danni_efficienza", "danni iniziali: l'efficienza di " + k + " va da 0 a 1");
        }
        for (const auto* voci : {&di.acqua, &di.falle, &di.incendi})
            for (const auto& x : *voci)
                if (x.compartimento.empty() || x.quanto <= 0.0) avviso(G::errore, "danni_compartimento", "danni iniziali: una voce senza compartimento o quantita'");
        if (di.urto && (di.urto->tnt_kg <= 0.0 || di.urto->distanza_m <= 0.0)) avviso(G::errore, "danni_urto", "danni iniziali: l'urto vuole carica e distanza");
    }
    for (const auto& u : s.unita) {
        if (!u.sonar_speciale) continue;
        const auto& x = *u.sonar_speciale;
        if (x.sl_db <= 0.0 || x.frequenza_hz <= 0.0) avviso(G::errore, "sonar_speciale", "sonar speciale senza livello o frequenza", u.id);
        if (!x.antenna.empty() && x.antenna != "sfera" && x.antenna != "lineare" && x.antenna != "scafo")
            avviso(G::errore, "sonar_speciale", "antenna del sonar speciale sconosciuta: " + x.antenna + " (sfera, lineare o scafo)", u.id);
    }
    // radio (R3 ed E6): id unici, precedenze e ROE con i nomi della sala radio, riferimenti ad altri messaggi
    if (s.radio) {
        static const std::set<std::string> PRECEDENZE = {"flash", "immediate", "priority", "routine"};
        static const std::set<std::string> CATEGORIE = {"sottomarini", "navi", "mercantili", "neutrali", "aerei"};
        static const std::set<std::string> STATI = {"libero", "stretto", "trattenuto"};
        const auto roe_valide = [&](const RoeScenario& r, const std::string& rif) {
            for (const auto& [k, x] : r)
                if (!CATEGORIE.count(k) || !STATI.count(x)) avviso(G::errore, "radio_roe", "regola d'ingaggio sconosciuta: " + k + " = " + x, -1, rif);
        };
        roe_valide(s.radio->roe, "radio");
        std::set<std::string> id_radio;
        for (const auto& m : s.radio->messaggi)
            if (!id_radio.insert(m.id).second) avviso(G::errore, "radio_id", "due messaggi radio con l'id " + m.id, -1, m.id);
        for (const auto& m : s.radio->messaggi) {
            if (m.id.empty()) avviso(G::errore, "radio_id", "un messaggio radio senza id");
            if (!PRECEDENZE.count(m.precedenza)) avviso(G::errore, "radio_precedenza", "precedenza sconosciuta: " + m.precedenza, -1, m.id);
            roe_valide(m.roe, m.id);
            for (const std::string* x : {&m.se_ricevuto, &m.annulla})
                if (!x->empty() && !id_radio.count(*x)) avviso(G::errore, "radio_riferimento", "il messaggio " + m.id + " nomina il messaggio sconosciuto " + *x, -1, m.id);
            for (const int id : m.se_unita_colpita)
                if (!unita_di(s, id)) avviso(G::errore, "radio_unita", "il messaggio " + m.id + " aspetta l'unita' sconosciuta " + std::to_string(id), -1, m.id);
            if (m.testo[IT].empty()) avviso(G::attenzione, "radio_testo", "il messaggio " + m.id + " non ha testo", -1, m.id);
            for (const auto* elenco : {&m.zone_aggiungi, &m.zone_togli})
                for (const auto& id : *elenco) {
                    const ZonaScenario* z = zona_di(s, id);
                    if (!z || z->radio.empty())
                        avviso(G::errore, "radio_zona", "il messaggio " + m.id + " nomina la zona " + id + ", che non c'e' o non e' della radio", -1, m.id);
                }
        }
    }
    {   // un codice d'esito ha un testo solo (quello della prima regola che lo usa): le altre lo mostrerebbero
        std::map<int, const RegolaScenario*> primo;
        for (const auto& r : s.regole)
            for (const auto& a : r.allora) {
                if (a.tipo != AzioneScenario::Tipo::fine) continue;
                const auto [it, nuovo] = primo.emplace(a.codice, &r);
                if (nuovo) continue;
                const auto& prima = *std::find_if(it->second->allora.begin(), it->second->allora.end(), [&](const AzioneScenario& x) {
                    return x.tipo == AzioneScenario::Tipo::fine && x.codice == a.codice;
                });
                if (prima.testo != a.testo || prima.categoria != a.categoria)
                    avviso(G::errore, "regola_esito_testo", "la regola " + r.id + " usa l'esito " + std::to_string(a.codice) + " della regola " +
                                                              it->second->id + " con un altro testo: si vedrebbe quello di " + it->second->id, -1, r.id);
            }
    }
    return v;
}

}  // namespace m688
