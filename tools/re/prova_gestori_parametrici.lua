-- Prova dei gestori del giocatore parametrici (docs/11_comandi_e_stazioni.md, fase C1): il primo argomento
-- dell'istruzione che riceve i parametri prende il valore del comando; senza parametri resta quello dell'originale.
--   lua prova_gestori_parametrici.lua SCENE*.lua
local chiamate
local m688 = setmetatable({}, { __index = function(_, nome)
  return function(_, ...) chiamate[#chiamate + 1] = { nome, ... } return false end
end })
local casi = {   -- gestore, parametri, funzione attesa, argomenti attesi
  { "telegrafo", { 0.75 }, "telegrafo", { 0.75 } },
  { "telegrafo", { "arresto_rapido" }, "telegrafo", { "arresto_rapido" } },
  { "timone", { -3.6 }, "rateo_virata", { -3.6 } },
  { "quota", { 45.72, 1 }, "quota", { 45.72, 1 } },
  { "periscopio", { 1 }, "periscopio", { 1 } },
  { "antenna", { 1 }, "antenna", { 1 } },
  { "designa_bersaglio", { 7 }, "bersaglio", { 7 } },
  { "punto_di_rotta", { 1000, -2000 }, "vai_a", { 1000, -2000 } },
  { "posizione", { 10, 20 }, "posizione", { 10, 20 } },
  { "guida_e2", { 500, 600 }, "vai_a", { 500, 600 } },
}
local errori = 0
for _, file in ipairs(arg) do
  local f = assert(loadfile(file, "t", setmetatable({ m688 = m688 }, { __index = _G })))
  local mis = f()
  for _, c in ipairs(casi) do
    local etichetta = mis.gestori_giocatore[c[1]]
    local g = etichetta and mis.script[etichetta]
    if g then
      for _, con in ipairs({ true, false }) do
        chiamate = {}
        g(0, con and c[2] or nil)
        local trovata
        for _, x in ipairs(chiamate) do if x[1] == c[3] then trovata = x break end end
        local ok = trovata ~= nil
        if ok and con then for i, v in ipairs(c[4]) do ok = ok and trovata[i + 1] == v end end
        if ok and not con then ok = trovata[2] ~= nil end   -- valore originale presente
        if not ok then errori = errori + 1 print(file, c[1], con and "con parametri" or "senza", "ERRATO") end
      end
    end
  end
end
print("errori: " .. errori)
os.exit(errori == 0 and 0 or 1)
