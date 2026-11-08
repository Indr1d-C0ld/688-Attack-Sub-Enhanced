-- Modelli del livello classico per classe, dalle 22 missioni convertite (data/missioni/SCENE*.lua), per le missioni
-- rapide (engine/missioni/src/missione_rapida.cpp). Per ogni classe tiene il modello piu' frequente fra le unita'
-- delle varianti; scrive la sezione [modelli] di data/motore/missioni_rapide.toml.
--   tools/lua/src/lua tools/rapide/estrai_modelli.lua data/missioni/SCENE*.lua > /tmp/modelli.toml

local CAMPI = { "massima_kn", "virata_gradi_s", "accel_kn_s", "quota_m_s", "quota_max_m", "integrita", "rumore", "sonar",
                "difesa" }

local conti = {}      -- classe -> firma -> { n, unita }
local function firma(u)
  local m = u.modello or {}
  local t = { u.categoria or "", tostring(u.tipo or 1), tostring(m.schiera_trainata or false),
              tostring(m.controllo_fondale or false), tostring(m.periodo_controlli or 1) }
  for _, c in ipairs(CAMPI) do t[#t + 1] = tostring(m[c]) end
  return table.concat(t, "|")
end

for i = 1, #arg do
  m688 = setmetatable({}, { __index = function() return function() end end })
  local ok, mis = pcall(dofile, arg[i])
  if ok and type(mis) == "table" then
    for _, v in ipairs(mis.varianti or {}) do
      for _, u in ipairs(v.unita or {}) do
        if u.classe and u.modello and not u.giocatore then
          conti[u.classe] = conti[u.classe] or {}
          local f = firma(u)
          local c = conti[u.classe][f] or { n = 0, unita = u }
          c.n = c.n + 1
          conti[u.classe][f] = c
        end
      end
    end
    -- il giocatore a parte: il suo modello serve per il 688 e per l'Alfa
    for _, v in ipairs(mis.varianti or {}) do
      for _, u in ipairs(v.unita or {}) do
        if u.giocatore and u.modello then
          local k = "giocatore_" .. u.classe
          conti[k] = conti[k] or {}
          local f = firma(u)
          local c = conti[k][f] or { n = 0, unita = u }
          c.n = c.n + 1
          conti[k][f] = c
        end
      end
    end
  end
end

local classi = {}
for k in pairs(conti) do classi[#classi + 1] = k end
table.sort(classi)
local function numero(x)
  if x == math.floor(x) then return string.format("%d", x) end
  return string.format("%.4f", x):gsub("0+$", "")
end
print("# Modelli del livello classico per classe (tools/rapide/estrai_modelli.lua dalle 22 missioni convertite)")
for _, k in ipairs(classi) do
  local migliore, firma_m
  for f, c in pairs(conti[k]) do
    if not migliore or c.n > migliore.n or (c.n == migliore.n and f < firma_m) then migliore, firma_m = c, f end
  end
  local u = migliore.unita
  local m = u.modello
  print("")
  print(string.format("[modelli.%s]", k))
  print(string.format('categoria = "%s"', u.categoria))
  print(string.format("flag = %d", u.flag or 0))
  print(string.format("tipo = %d", u.tipo or 1))
  for _, c in ipairs(CAMPI) do
    if m[c] ~= nil then print(string.format("%s = %s", c, numero(m[c]))) end
  end
  print(string.format("schiera_trainata = %s", tostring(m.schiera_trainata or false)))
  print(string.format("controllo_fondale = %s", tostring(m.controllo_fondale or false)))
  print(string.format("periodo_controlli = %d", m.periodo_controlli or 1))
end
