// kp_eventi.c: i tasti del tastierino come eventi veri di SDL3, su una finestra NASCOSTA (non si vede, non prende il fuoco, Bloc Num dell'utente non si tocca).
// Stampa l'id della finestra X, poi per 6 s ogni evento di tasto.
#include <SDL3/SDL.h>
#include <stdio.h>
int main(void) {
    if (!SDL_Init(SDL_INIT_VIDEO)) return 1;
    SDL_Window* w = SDL_CreateWindow("kp", 64, 64, SDL_WINDOW_HIDDEN);
    if (!w) { printf("finestra: %s\n", SDL_GetError()); return 1; }
    const long id = (long)SDL_GetNumberProperty(SDL_GetWindowProperties(w), SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
    printf("%ld\n", id); fflush(stdout);
    const Uint64 fine = SDL_GetTicks() + 6000;
    SDL_Event e;
    while (SDL_GetTicks() < fine) {
        while (SDL_PollEvent(&e))
            if (e.type == SDL_EVENT_KEY_DOWN) { printf("giu scancode %-12s keycode %-12s (%#x) mod %#x\n", SDL_GetScancodeName(e.key.scancode), SDL_GetKeyName(e.key.key), (unsigned)e.key.key, (unsigned)e.key.mod); fflush(stdout); }
        SDL_Delay(10);
    }
    SDL_Quit();
    return 0;
}
