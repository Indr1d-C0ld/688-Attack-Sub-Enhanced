-- Verifica delle missioni convertite (docs/09_schema_missioni.md §7).
-- Uso: tools/lua/src/lua tools/re/verifica_missioni.lua data/missioni/*.lua
-- Ogni file gira in un ambiente isolato con un m688 finto: le condizioni rispondono a caso, le altre
-- funzioni non fanno nulla. Si controllano i riferimenti agli script e si esegue ogni script 200 volte.

local errori_totali = 0
math.randomseed(688)

local function sandbox()
  local chiamate = {}
  local m688 = setmetatable({}, { __index = function(_, nome)
    return function(u, ...)
      chiamate[nome] = (chiamate[nome] or 0) + 1
      if nome == "confronta_variabili" then return math.random(-1, 1) end
      return math.random() < 0.5
    end
  end })
  local env = { m688 = m688, math = math, string = string, table = table, pairs = pairs, ipairs = ipairs,
                type = type, tostring = tostring, tonumber = tonumber, select = select }
  return env, chiamate
end

for _, percorso in ipairs(arg) do
  local env, chiamate = sandbox()
  local errori, avvisi = {}, {}
  local function errore(m) errori[#errori + 1] = m end
  local blocco, msg = loadfile(percorso, "t", env)
  if not blocco then errore("caricamento: " .. msg) else
    local ok, m = pcall(blocco)
    if not ok then errore("esecuzione: " .. tostring(m)) else
      local s = m.script
      local function esiste(nome, dove)
        if nome ~= nil and s[nome] == nil then errore(dove .. ": script " .. nome .. " assente") end
      end
      if m.formato ~= 1 then errore("formato diverso da 1") end
      local somma = 0
      for i, v in ipairs(m.varianti) do
        somma = somma + v.probabilita
        for _, u in ipairs(v.unita) do
          if not u.classe or not u.posizione then errore("variante " .. i .. ": unità senza classe o posizione") end
          esiste(u.gestori.bersaglio, "unità " .. u.id); esiste(u.gestori.reazione, "unità " .. u.id); esiste(u.gestori.arrivo, "unità " .. u.id)
        end
        for _, t in ipairs(v.timer) do esiste(t.gestore, "timer") end
      end
      if math.abs(somma - 1) > 1e-6 then errore(string.format("probabilità: somma %.6f", somma)) end
      for nome, a in pairs(m.armi) do esiste(a.gestore, "arma " .. nome) end
      for nome, g in pairs(m.gestori_giocatore) do esiste(g, "gestore " .. nome) end
      -- esecuzione degli script con un limite di istruzioni (i cicli dell'originale non sono errori)
      local n, cicli = 0, 0
      for nome, f in pairs(s) do
        n = n + 1
        for _ = 1, 200 do
          local co = coroutine.create(f)
          debug.sethook(co, function() error("__limite__") end, "", 100000)
          local ok2, e = coroutine.resume(co, {})
          if not ok2 then
            if tostring(e):find("__limite__") then cicli = cicli + 1 break
            else errore(nome .. ": " .. tostring(e)) break end
          end
        end
      end
      local distinte = 0; for _ in pairs(chiamate) do distinte = distinte + 1 end
      print(string.format("%-28s script %4d  varianti %3d  funzioni m688 %3d  cicli lunghi %d  errori %d",
        percorso:match("[^/]+$"), n, #m.varianti, distinte, cicli, #errori))
    end
  end
  for _, e in ipairs(errori) do print("   ERRORE " .. e) end
  errori_totali = errori_totali + #errori
end
print("errori totali: " .. errori_totali)
os.exit(errori_totali == 0 and 0 or 1)
