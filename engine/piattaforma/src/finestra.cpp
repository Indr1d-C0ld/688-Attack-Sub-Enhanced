#include "piattaforma/finestra.hpp"

#include <SDL3/SDL.h>

#include <algorithm>
#include <cstdlib>
#include <map>
#include <stdexcept>

#include "grafica/etichetta_tasti.hpp"
#include "grafica/palette.hpp"
#include "grafica/tela.hpp"

namespace m688 {

namespace {
[[noreturn]] void errore_sdl(const char* cosa) { throw std::runtime_error(std::string(cosa) + ": " + SDL_GetError()); }
}  // namespace

std::string forma_tasti_valore();

Finestra::Finestra(const std::string& titolo, int scala_iniziale)
    : argb_(static_cast<std::size_t>(LARGHEZZA_TELA) * ALTEZZA_TELA) {
    if (!SDL_Init(SDL_INIT_VIDEO)) errore_sdl("SDL_Init");
    if (!std::getenv("M688_TASTI_VALORE")) tasti_valore() = forma_tasti_valore();
    finestra_ = SDL_CreateWindow(titolo.c_str(), LARGHEZZA_TELA * scala_iniziale, ALTEZZA_TELA * scala_iniziale,
                                 SDL_WINDOW_RESIZABLE);
    if (!finestra_) errore_sdl("SDL_CreateWindow");
    renderer_ = SDL_CreateRenderer(finestra_, nullptr);
    if (!renderer_) errore_sdl("SDL_CreateRenderer");
    SDL_SetRenderVSync(renderer_, 1);
    if (!SDL_SetRenderLogicalPresentation(renderer_, LARGHEZZA_TELA, ALTEZZA_TELA,
                                          SDL_LOGICAL_PRESENTATION_INTEGER_SCALE))
        errore_sdl("SDL_SetRenderLogicalPresentation");
    trama_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, LARGHEZZA_TELA,
                               ALTEZZA_TELA);
    if (!trama_) errore_sdl("SDL_CreateTexture");
    SDL_SetTextureScaleMode(trama_, SDL_SCALEMODE_NEAREST);
}

Finestra::~Finestra() {
    if (crt_) SDL_DestroyTexture(crt_);
    if (intermedia_) SDL_DestroyTexture(intermedia_);
    if (trama_) SDL_DestroyTexture(trama_);
    if (renderer_) SDL_DestroyRenderer(renderer_);
    if (finestra_) SDL_DestroyWindow(finestra_);
    SDL_Quit();
}

namespace {
// Nome italiano del tasto premuto, come nel file dei tasti: "Tastierino 4", "Ctrl+P", "F7", "W", "[" ...
std::string nome_tasto(SDL_Keycode k, SDL_Keymod mod) {
    std::string n;
    if (k >= SDLK_KP_1 && k <= SDLK_KP_9) n = "Tastierino " + std::to_string(static_cast<int>(k - SDLK_KP_1) + 1);
    else if (k >= SDLK_F1 && k <= SDLK_F12) n = "F" + std::to_string(static_cast<int>(k - SDLK_F1) + 1);
    else {
        switch (k) {
            case SDLK_KP_0: n = "Tastierino 0"; break;
            case SDLK_KP_PLUS: n = "Tastierino +"; break;
            case SDLK_KP_MINUS: n = "Tastierino -"; break;
            case SDLK_KP_PERIOD: n = "Tastierino ."; break;
            case SDLK_KP_ENTER: n = "Tastierino Invio"; break;
            case SDLK_RETURN: n = "Invio"; break;
            case SDLK_ESCAPE: n = "Esc"; break;
            case SDLK_TAB: n = "Tab"; break;
            case SDLK_SPACE: n = "Spazio"; break;
            case SDLK_UP: n = "Su"; break;
            case SDLK_DOWN: n = "Giu"; break;
            case SDLK_LEFT: n = "Sinistra"; break;
            case SDLK_RIGHT: n = "Destra"; break;
            case SDLK_PAGEUP: n = "Pag Su"; break;
            case SDLK_PAGEDOWN: n = "Pag Giu"; break;
            default:
                if (k >= SDLK_A && k <= SDLK_Z) n = std::string(1, static_cast<char>('A' + (k - SDLK_A)));
                else if (k >= 32 && k < 127) n = std::string(1, static_cast<char>(k));   // cifre e segni come sono
                else n = SDL_GetKeyName(k);
        }
    }
    const bool lettera_o_cifra = n.size() == 1;
    if ((mod & SDL_KMOD_SHIFT) && !lettera_o_cifra) n = "Maiusc+" + n;   // Maiuscolo+lettera resta la lettera
    if (mod & SDL_KMOD_CTRL) n = "Ctrl+" + n;
    return n;
}
}  // namespace

// La scritta dei tasti di `[` e `]` col layout attivo: "[ ]" se sono diretti, o se il layout li fa in un altro modo; se no AltGr piu' i
// tasti che li producono, come sono incisi: "ALTGR+È/+" sulla tastiera italiana. Fuori da finestra.hpp: la usano le prove
std::string forma_tasti_valore() {
    int meno = -1, piu = -1;
    bool diretti_meno = false, diretti_piu = false;
    for (int s = 0; s < SDL_SCANCODE_COUNT; ++s) {
        const auto sc = static_cast<SDL_Scancode>(s);
        diretti_meno = diretti_meno || SDL_GetKeyFromScancode(sc, SDL_KMOD_NONE, false) == '[';
        diretti_piu = diretti_piu || SDL_GetKeyFromScancode(sc, SDL_KMOD_NONE, false) == ']';
        // se due tasti lo fanno, quello fuori dalla fila dei numeri: sull'italiana X11 anche AltGr+8, ma e' sull'è che c'e' inciso
        const bool cifra = s >= SDL_SCANCODE_1 && s <= SDL_SCANCODE_0;
        const auto prendi = [&](int& q) { if (q < 0 || (q >= SDL_SCANCODE_1 && q <= SDL_SCANCODE_0 && !cifra)) q = s; };
        if (SDL_GetKeyFromScancode(sc, SDL_KMOD_MODE, false) == '[') prendi(meno);
        if (SDL_GetKeyFromScancode(sc, SDL_KMOD_MODE, false) == ']') prendi(piu);
    }
    if ((diretti_meno && diretti_piu) || meno < 0 || piu < 0) return "[ ]";
    const auto inciso = [](int s) {   // il tasto com'e' inciso: maiuscolo, e latin-1 in UTF-8, come lo vuole il font
        const SDL_Keycode k = SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(s), SDL_KMOD_NONE, false);
        if (k >= 'a' && k <= 'z') return std::string(1, static_cast<char>(k - 'a' + 'A'));
        if (k >= 32 && k < 127) return std::string(1, static_cast<char>(k));
        if (k >= 0xE0 && k <= 0xFE && k != 0xF7) {   // à-þ: la maiuscola sta 0x20 sotto
            const unsigned u = k - 0x20;
            return std::string{static_cast<char>(0xC0 | (u >> 6)), static_cast<char>(0x80 | (u & 0x3F))};
        }
        return std::string("?");
    };
    return "ALTGR+" + inciso(meno) + "/" + inciso(piu);
}

// Il comando di un tasto premuto. SDL dice il tasto senza i modificatori: su una tastiera italiana il tasto di `[` e' sempre «è», con
// o senza AltGr (misura in tools/re/prove/menu/tastiera/). Vale quindi anche il carattere di punteggiatura che quel tasto produce
// davvero: con AltGr per primo (AltGr++ e' `]`, anche se il + ha un suo comando), e con Maiusc gli stessi (Maiusc e' il passo lungo,
// non un altro carattere); con la sola Maiusc solo se il tasto non ha un comando. Ctrl senza AltGr non conta. Dove AltGr non produce
// niente di suo, come sulla tastiera americana, il tasto resta quello. Fuori da finestra.hpp: la usano le prove
std::string comando_del_tasto(const MappaTasti& tasti, int scancode, int tasto, int mod_tasto) {
    const auto mod = static_cast<SDL_Keymod>(mod_tasto);
    const std::string base = tasti.comando(nome_tasto(static_cast<SDL_Keycode>(tasto), mod));
    const bool altgr = (mod & (SDL_KMOD_MODE | SDL_KMOD_RALT)) != 0;
    if (!altgr && (!base.empty() || (mod & SDL_KMOD_CTRL) || !(mod & SDL_KMOD_SHIFT))) return base;
    const SDL_Keymod maiusc = mod & SDL_KMOD_SHIFT, livello = altgr ? static_cast<SDL_Keymod>(SDL_KMOD_MODE) : static_cast<SDL_Keymod>(SDL_KMOD_NONE);
    for (const SDL_Keymod m : {static_cast<SDL_Keymod>(maiusc | livello), livello}) {
        if (m == SDL_KMOD_NONE) continue;
        const SDL_Keycode k = SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(scancode), m, false);
        if (k < 33 || k >= 127 || (k >= '0' && k <= '9') || (k >= 'a' && k <= 'z') || (k >= 'A' && k <= 'Z')) continue;
        const std::string c = tasti.comando(std::string(1, static_cast<char>(k)));
        if (!c.empty()) return c;
    }
    return base;
}

void Finestra::imposta_tasti(MappaTasti m) { tasti_ = std::move(m); }

// Il comando del tasto nei campi di Comandi (nomi in nucleo/mappa_tasti.cpp)
void applica(const std::string& k, Comandi& c) {
    if (k.empty()) return;
    static const std::map<std::string, Azione> AZIONI = {
        {"telegrafo_avanti", Azione::telegrafo_avanti}, {"telegrafo_indietro", Azione::telegrafo_indietro},
        {"timone_sinistra", Azione::timone_sinistra}, {"timone_dritta", Azione::timone_dritta},
        {"timone_centro", Azione::timone_centro}, {"barra_su", Azione::barra_su}, {"barra_giu", Azione::barra_giu},
        {"barra_zero", Azione::barra_zero}, {"pilota_automatico", Azione::pilota_automatico},
        {"periscopio", Azione::periscopio}, {"antenna", Azione::antenna},
        {"quota_periscopica", Azione::quota_periscopica}, {"quota_150_piedi", Azione::quota_150_piedi},
        {"emersione_emergenza", Azione::emersione_emergenza}, {"posati", Azione::posati},
        {"immersione_emergenza", Azione::immersione_emergenza},
    };
    if (const auto it = AZIONI.find(k); it != AZIONI.end()) { c.azioni.push_back(it->second); return; }
    if (k.rfind("pulsante_", 0) == 0) { c.cifre.push_back(std::stoi(k.substr(9))); return; }
    if (k == "esci") c.esc = true;
    else if (k == "conferma") c.invio = true;
    else if (k == "zona_avanti") c.tab = +1;
    else if (k == "zona_indietro") c.tab = -1;
    else if (k == "cambia_ingrandimento") c.cambia_ingrandimento = true;
    else if (k == "ingrandisci") c.ingrandisci = true;
    else if (k == "riduci") c.riduci = true;
    else if (k == "compressione_piu") c.compressione = +1;
    else if (k == "compressione_meno") c.compressione = -1;
    else if (k == "verita") c.verita = true;
    else if (k == "istantanea") c.istantanea = true;
    else if (k == "stazione_radio") c.stazione = 2;
    else if (k == "stazione_stato") c.stazione = 3;
    else if (k == "stazione_manovra") c.stazione = 4;
    else if (k == "stazione_periscopio") c.stazione = 5;
    else if (k == "stazione_navigazione") c.stazione = 6;
    else if (k == "stazione_armi") c.stazione = 7;
    else if (k == "stazione_sonar") c.stazione = 8;
    else if (k == "stazione_centrale") c.stazione = 9;
    else if (k == "pausa") c.pausa = true;
    else if (k == "stadimetro") c.stadimetro = true;
    else if (k == "carta_evento") c.evento = true;
    else if (k == "carta_aggancia") c.aggancia = true;
    else if (k == "carta_ekelund") c.ekelund = true;
    else if (k == "carta_pulisci") c.pulisci = true;
    else if (k == "silenzio_a_bordo") c.silenzio = true;
    else if (k.rfind("tubo_", 0) == 0) c.tubi.push_back(std::stoi(k.substr(5)));   // tubo_1 ... tubo_7
    else if (k == "lancia") c.lancia = true;
    else if (k == "bersaglio") c.bersaglio = true;
    else if (k == "siluro") c.siluro = true;
    else if (k == "cerca") c.cerca = true;
    else if (k == "detona") c.detona = true;
    else if (k == "punto_di_rotta") c.punto = true;
    else if (k == "cancella_punto") c.cancella_punto = true;
    else if (k == "proiezione") c.proiezione = true;
    else if (k == "sfondo") c.sfondo = true;
    else if (k == "mate") c.tma = true;
    else if (k == "filo_sinistra") c.filo_rotta = -1;
    else if (k == "filo_dritta") c.filo_rotta = +1;
    else if (k == "taglia_filo") c.taglia_filo = true;
    else if (k == "variante_arma") c.variante_arma = true;
    else if (k == "enciclopedia") c.enciclopedia = true;
    else if (k == "valore_meno") c.minuti = -1;
    else if (k == "valore_piu") c.minuti = +1;
    else c.nominati.push_back(k);   // li interpreta la stazione (boksit_*, akkord_*, sfoga, compensa_*, tan)
}

void Finestra::modo_testo(bool si) {
    if (si == testo_) return;
    testo_ = si;
    if (si) SDL_StartTextInput(finestra_);
    else SDL_StopTextInput(finestra_);
}

Comandi Finestra::eventi() {
    Comandi c;
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT) c.esci = true;
        if (e.type == SDL_EVENT_MOUSE_WHEEL) {
            const float y = e.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -e.wheel.y : e.wheel.y;
            c.rotella += y > 0.0f ? 1 : y < 0.0f ? -1 : 0;
        }
        if (e.type == SDL_EVENT_KEYMAP_CHANGED && !std::getenv("M688_TASTI_VALORE")) tasti_valore() = forma_tasti_valore();
        if ((e.type == SDL_EVENT_MOUSE_BUTTON_DOWN || e.type == SDL_EVENT_MOUSE_BUTTON_UP) &&
            (e.button.button == SDL_BUTTON_LEFT || e.button.button == SDL_BUTTON_RIGHT)) {   // in pixel della tela
            float x = 0, y = 0;
            SDL_RenderCoordinatesFromWindow(renderer_, e.button.x, e.button.y, &x, &y);
            const std::pair<int, int> p{static_cast<int>(x), static_cast<int>(y)};
            if (e.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                if (e.button.button == SDL_BUTTON_LEFT) c.rilascio.push_back(p);
            } else {
                (e.button.button == SDL_BUTTON_LEFT ? c.clic : c.clic_destro).push_back(p);
            }
            continue;
        }
        if (e.type == SDL_EVENT_TEXT_INPUT) {
            c.testo += e.text.text;
            continue;
        }
        if (e.type != SDL_EVENT_KEY_DOWN || (e.key.repeat && e.key.key != SDLK_BACKSPACE)) continue;
        if (e.key.key == SDLK_BACKSPACE) c.cancella = true;
        if (e.key.scancode == SDL_SCANCODE_UP) ++c.passi_su_giu;
        else if (e.key.scancode == SDL_SCANCODE_DOWN) --c.passi_su_giu;
        else if (e.key.scancode == SDL_SCANCODE_RIGHT) ++c.passi_ruota;
        else if (e.key.scancode == SDL_SCANCODE_LEFT) --c.passi_ruota;
        if (e.key.key >= SDLK_A && e.key.key <= SDLK_Z && !(e.key.mod & SDL_KMOD_CTRL))   // lettere per TARGET
            c.lettere.push_back(static_cast<char>('A' + (e.key.key - SDLK_A)));
        applica(comando_del_tasto(tasti_, e.key.scancode, static_cast<int>(e.key.key), e.key.mod), c);
    }
    {   // il puntatore e il tasto sinistro tenuto, per i trascinamenti
        float wx = 0, wy = 0, x = 0, y = 0;
        const SDL_MouseButtonFlags b = SDL_GetMouseState(&wx, &wy);
        SDL_RenderCoordinatesFromWindow(renderer_, wx, wy, &x, &y);
        if (x >= 0 && y >= 0 && x < LARGHEZZA_TELA && y < ALTEZZA_TELA) c.mouse = {static_cast<int>(x), static_cast<int>(y)};
        c.premuto = (b & SDL_BUTTON_LMASK) != 0;
    }
    const bool* tasti = SDL_GetKeyboardState(nullptr);
    c.ruota = (tasti[SDL_SCANCODE_RIGHT] ? 1 : 0) - (tasti[SDL_SCANCODE_LEFT] ? 1 : 0);
    c.su_giu = (tasti[SDL_SCANCODE_UP] ? 1 : 0) - (tasti[SDL_SCANCODE_DOWN] ? 1 : 0);
    c.veloce = tasti[SDL_SCANCODE_LSHIFT] || tasti[SDL_SCANCODE_RSHIFT];
    // F10: il giro delle uscite a schermo, qui per tutte le schermate; la scelta va salvata e mostrata
    if (const auto it = std::find(c.nominati.begin(), c.nominati.end(), "uscita_schermo"); it != c.nominati.end()) {
        c.nominati.erase(it);
        Uscita u = Uscita::nitida;
        bool crt = false;
        if (uscita_ == Uscita::nitida) u = Uscita::sharp_bilinear;
        else if (!con_crt_) { u = Uscita::sharp_bilinear; crt = true; }
        imposta_uscita(u, crt);
        if (salva_uscita_) salva_uscita_(u, crt);
        avviso_ = u == Uscita::nitida ? "USCITA: NITIDA" : crt ? "USCITA: SHARP BILINEAR + CRT" : "USCITA: SHARP BILINEAR";
        avviso_fino_ = secondi() + 2.0;
    }
    return c;
}

void Finestra::imposta_uscita(Uscita u, bool crt) {
    uscita_ = u;
    con_crt_ = crt;
    // nitida: multipli interi con le bande nere (come sempre); sharp bilinear: la finestra intera in 4:3
    SDL_SetRenderLogicalPresentation(renderer_, LARGHEZZA_TELA, ALTEZZA_TELA,
                                     u == Uscita::nitida ? SDL_LOGICAL_PRESENTATION_INTEGER_SCALE
                                                         : SDL_LOGICAL_PRESENTATION_LETTERBOX);
}

namespace {
// Il CRT leggero (E9 §2.1): per ogni riga della tela tre sottorighe, l'ultima scura (la riga di scansione); per ogni
// colonna tre sottocolonne appena tinte rosso, verde e blu (la maschera). Si moltiplica sopra l'immagine.
constexpr int CRT_SOTTO = 3;
constexpr std::uint8_t CRT_RIGA = 168, CRT_MASCHERA = 228;

std::vector<std::uint32_t> pixel_crt() {
    const int w = LARGHEZZA_TELA * CRT_SOTTO, h = ALTEZZA_TELA * CRT_SOTTO;
    std::vector<std::uint32_t> px(static_cast<std::size_t>(w) * h);
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            const double riga = y % CRT_SOTTO == CRT_SOTTO - 1 ? CRT_RIGA / 255.0 : 1.0;
            const int k = x % CRT_SOTTO;
            auto c = [&](int canale) { return static_cast<std::uint32_t>((canale == k ? 255.0 : CRT_MASCHERA) * riga); };
            px[static_cast<std::size_t>(y) * w + x] = 0xFF000000u | (c(0) << 16) | (c(1) << 8) | c(2);
        }
    return px;
}
}  // namespace

void Finestra::presenta(const Tela& t, const Palette& p) {
    std::uint32_t lut[256];
    for (int i = 0; i < 256; ++i) lut[i] = p.argb(i);
    const std::uint8_t* src = t.dati();
    for (std::size_t i = 0; i < argb_.size(); ++i) argb_[i] = lut[src[i]];
    SDL_UpdateTexture(trama_, nullptr, argb_.data(), LARGHEZZA_TELA * 4);
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);
    if (uscita_ == Uscita::nitida) {
        SDL_RenderTexture(renderer_, trama_, nullptr, nullptr);
    } else {
        // sharp bilinear: la tela al multiplo intero che copre la finestra, nitida, poi lineare fino alla finestra
        int w = 0, h = 0;
        SDL_GetCurrentRenderOutputSize(renderer_, &w, &h);
        const int k = std::clamp((std::max(h, 1) + ALTEZZA_TELA - 1) / ALTEZZA_TELA, 1, 8);
        if (!intermedia_ || k != scala_intermedia_) {
            if (intermedia_) SDL_DestroyTexture(intermedia_);
            intermedia_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET,
                                            LARGHEZZA_TELA * k, ALTEZZA_TELA * k);
            scala_intermedia_ = k;
            if (intermedia_) SDL_SetTextureScaleMode(intermedia_, SDL_SCALEMODE_LINEAR);
        }
        if (intermedia_ && SDL_SetRenderTarget(renderer_, intermedia_)) {
            SDL_RenderTexture(renderer_, trama_, nullptr, nullptr);
            SDL_SetRenderTarget(renderer_, nullptr);
            SDL_RenderTexture(renderer_, intermedia_, nullptr, nullptr);
        } else {
            SDL_RenderTexture(renderer_, trama_, nullptr, nullptr);   // senza trame di destinazione: come la nitida
        }
    }
    if (con_crt_) {
        if (!crt_) {
            crt_w_ = LARGHEZZA_TELA * CRT_SOTTO;
            crt_h_ = ALTEZZA_TELA * CRT_SOTTO;
            crt_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, crt_w_, crt_h_);
            if (crt_) {
                const auto px = pixel_crt();
                SDL_UpdateTexture(crt_, nullptr, px.data(), crt_w_ * 4);
                SDL_SetTextureBlendMode(crt_, SDL_BLENDMODE_MOD);
                SDL_SetTextureScaleMode(crt_, SDL_SCALEMODE_LINEAR);
            }
        }
        if (crt_) SDL_RenderTexture(renderer_, crt_, nullptr, nullptr);
    }
    if (!avviso_.empty() && secondi() < avviso_fino_) {   // l'avviso del tasto F10, in alto a sinistra sopra tutto
        const float w = 8.0f * static_cast<float>(avviso_.size()) + 12.0f;
        const SDL_FRect fondo{6.0f, 6.0f, w, 20.0f};
        SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer_, &fondo);
        SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
        SDL_RenderDebugText(renderer_, 12.0f, 12.0f, avviso_.c_str());
    }
    const double prima = secondi();
    SDL_RenderPresent(renderer_);
    ultimo_present_s_ = secondi() - prima;
}

std::string Finestra::descrizione() const {
    const char* video = SDL_GetCurrentVideoDriver();
    const char* nome = SDL_GetRendererName(renderer_);
    int vsync = 0, w = 0, h = 0;
    SDL_GetRenderVSync(renderer_, &vsync);
    SDL_GetWindowSizeInPixels(finestra_, &w, &h);
    return std::string("video ") + (video ? video : "?") + ", renderer " + (nome ? nome : "?") + ", vsync " + std::to_string(vsync) + ", finestra " + std::to_string(w) + "x" + std::to_string(h);
}

double Finestra::secondi() { return static_cast<double>(SDL_GetTicksNS()) * 1e-9; }

}  // namespace m688
