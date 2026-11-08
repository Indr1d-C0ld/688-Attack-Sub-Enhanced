-- Riassunto delle 22 missioni convertite per scrivere i loro contenuti radio (R3, docs/11):
--   tools/lua/src/lua tools/radio/riassunto_missioni.lua data/missioni > riassunto.txt
-- Titolo, teatro, giocatore, briefing inglese, esiti, unita' della prima variante e obiettivi M1.
local cartella = arg[1] or "data/missioni"
local function carica(f)
  local fn = loadfile(f, "t", setmetatable({}, { __index = _G }))
  if not fn then return nil end
  local ok, t = pcall(fn)
  return ok and t or nil
end
local function riga(s) return (tostring(s or ""):gsub("\n", " / ")) end
local ids = {}
for _, n in ipairs({ "0A", "1A", "1B", "1C", "2A", "2B", "2C", "3B", "4A", "5A", "5B", "5C", "6A", "7A", "7B", "7C", "8A", "8B", "8C", "9A", "9B", "9C" }) do
  ids[#ids + 1] = "SCENE" .. n
end
for _, id in ipairs(ids) do
  local m = carica(cartella .. "/" .. id .. ".lua")
  print("==== " .. id .. "  " .. riga(m.titolo) .. "  teatro " .. riga(m.teatro))
  if m.briefing and m.briefing.en then
    for _, r in ipairs(m.briefing.en) do print("  B: " .. riga(r)) end
  end
  if m.esiti then
    local codici = {}
    for k in pairs(m.esiti) do codici[#codici + 1] = k end
    table.sort(codici)
    for _, k in ipairs(codici) do
      local e = m.esiti[k]
      print(string.format("  E%d: %s", k, riga(e.en):sub(1, 220)))
    end
  end
  local v = m.varianti and m.varianti[1]
  if v then
    local u = {}
    for _, x in ipairs(v.unita) do
      u[#u + 1] = string.format("%d:%s/%s%s", x.id, x.classe, x.schieramento or "?", x.giocatore and "*" or "")
    end
    print("  U: " .. table.concat(u, " ") .. "  (varianti " .. #m.varianti .. ")")
  end
  if m.dotazione then
    local d = {}
    for k, n in pairs(m.dotazione) do d[#d + 1] = k .. "=" .. n end
    table.sort(d)
    print("  D: " .. table.concat(d, " "))
  end
  local o = carica(cartella .. "/obiettivi/" .. id .. ".lua")
  if o and o.obiettivi then
    for _, x in ipairs(o.obiettivi) do
      print(string.format("  O: %s %s %s", x.id or "?", x.tipo or "?", riga(x.testo and x.testo.it or "")))
    end
  end
  if o and o.categorie_esiti then
    local c = {}
    for k, s in pairs(o.categorie_esiti) do c[#c + 1] = k .. "=" .. s end
    table.sort(c)
    print("  C: " .. table.concat(c, " "))
  end
end
