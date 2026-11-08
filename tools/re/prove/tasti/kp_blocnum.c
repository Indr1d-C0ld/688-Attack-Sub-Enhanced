// kp_blocnum.c: che keycode da' SDL3 ai tasti del tastierino con Bloc Num acceso e spento, sul server X vero. Apre solo il video, nessuna finestra, nessun tasto premuto:
// e' la funzione che SDL usa per gli eventi (SDL_GetKeyFromScancode con key_event = true).
#include <SDL3/SDL.h>
#include <stdio.h>
int main(void) {
    if (!SDL_Init(SDL_INIT_VIDEO)) { printf("init: %s\n", SDL_GetError()); return 1; }
    printf("driver %s, hint KEYCODE_OPTIONS=%s, modi ora %x\n", SDL_GetCurrentVideoDriver(), SDL_GetHint(SDL_HINT_KEYCODE_OPTIONS) ? SDL_GetHint(SDL_HINT_KEYCODE_OPTIONS) : "(predefinito)", (unsigned)SDL_GetModState());
    static const SDL_Scancode K[] = {SDL_SCANCODE_KP_0, SDL_SCANCODE_KP_1, SDL_SCANCODE_KP_2, SDL_SCANCODE_KP_3, SDL_SCANCODE_KP_4, SDL_SCANCODE_KP_5, SDL_SCANCODE_KP_6, SDL_SCANCODE_KP_7, SDL_SCANCODE_KP_8, SDL_SCANCODE_KP_9, SDL_SCANCODE_KP_PERIOD, SDL_SCANCODE_KP_ENTER, SDL_SCANCODE_KP_PLUS, SDL_SCANCODE_KP_MINUS};
    for (size_t i = 0; i < sizeof K / sizeof *K; ++i) {
        const SDL_Keycode acceso = SDL_GetKeyFromScancode(K[i], SDL_KMOD_NUM, true), spento = SDL_GetKeyFromScancode(K[i], SDL_KMOD_NONE, true);
        printf("%-12s BlocNum acceso: %-14s (%#x)  spento: %-14s (%#x)\n", SDL_GetScancodeName(K[i]), SDL_GetKeyName(acceso), (unsigned)acceso, SDL_GetKeyName(spento), (unsigned)spento);
    }
    SDL_Quit();
    return 0;
}
