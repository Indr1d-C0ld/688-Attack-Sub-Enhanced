#pragma once
// Lo stato Lua isolato per leggere scenari e pacchetti dell'editor (anche di terzi): le stesse librerie delle missioni
// (base senza caricamento di codice e file, math, string, table), `m688` finto che non fa niente, un tetto di memoria
// e un limite di istruzioni.
#include <cstdlib>
#include <memory>
#include <stdexcept>

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

namespace m688::lua_isolato {

struct Memoria {
    std::size_t usata = 0, tetto = 0;
};

inline void* alloca_con_tetto(void* ud, void* p, std::size_t vecchia, std::size_t nuova) {
    auto* m = static_cast<Memoria*>(ud);
    if (p == nullptr) vecchia = 0;
    if (nuova == 0) {
        std::free(p);
        m->usata -= vecchia;
        return nullptr;
    }
    if (m->usata - vecchia + nuova > m->tetto) return nullptr;   // memoria esaurita: Lua da' un errore
    void* q = std::realloc(p, nuova);
    if (q) m->usata = m->usata - vecchia + nuova;
    return q;
}

inline int nulla(lua_State*) { return 0; }
inline int indice_finto(lua_State* L) {
    lua_pushcfunction(L, nulla);
    return 1;
}

// Lo stato e la sua memoria: si chiude da solo
class Stato {
public:
    explicit Stato(std::size_t tetto_byte = 64u << 20, int istruzioni = 50000000) {
        memoria_ = std::make_unique<Memoria>();
        memoria_->tetto = tetto_byte;
        L_ = lua_newstate(alloca_con_tetto, memoria_.get());
        if (!L_) throw std::runtime_error("Lua: memoria esaurita");
        luaL_requiref(L_, LUA_GNAME, luaopen_base, 1); lua_pop(L_, 1);
        luaL_requiref(L_, LUA_MATHLIBNAME, luaopen_math, 1); lua_pop(L_, 1);
        luaL_requiref(L_, LUA_STRLIBNAME, luaopen_string, 1); lua_pop(L_, 1);
        luaL_requiref(L_, LUA_TABLIBNAME, luaopen_table, 1); lua_pop(L_, 1);
        for (const char* via : {"dofile", "loadfile", "load", "require", "collectgarbage", "print"}) {
            lua_pushnil(L_);
            lua_setglobal(L_, via);
        }
        lua_newtable(L_);
        lua_newtable(L_);
        lua_pushcfunction(L_, indice_finto);
        lua_setfield(L_, -2, "__index");
        lua_setmetatable(L_, -2);
        lua_setglobal(L_, "m688");
        lua_sethook(L_, [](lua_State* l, lua_Debug*) { luaL_error(l, "troppe istruzioni al caricamento"); }, LUA_MASKCOUNT, istruzioni);
    }
    ~Stato() { if (L_) lua_close(L_); }
    Stato(const Stato&) = delete;
    Stato& operator=(const Stato&) = delete;
    lua_State* get() const { return L_; }
    // Esegue il testo (solo testo, niente bytecode) e lascia sullo stack quello che restituisce
    void esegui(const std::string& testo, const char* nome) {
        if (luaL_loadbufferx(L_, testo.data(), testo.size(), nome, "t") != LUA_OK || lua_pcall(L_, 0, 1, 0) != LUA_OK)
            throw std::runtime_error(lua_tostring(L_, -1) ? lua_tostring(L_, -1) : "errore di caricamento");
    }

private:
    std::unique_ptr<Memoria> memoria_;
    lua_State* L_ = nullptr;
};

}  // namespace m688::lua_isolato
