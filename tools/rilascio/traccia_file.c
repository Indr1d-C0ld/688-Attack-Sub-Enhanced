// Traccia dei file che il gioco apre, per la verifica del pacchetto autonomo (tools/rilascio/autonomia.sh), al posto di
// strace che sulla macchina non c'e'. Si carica con LD_PRELOAD e scrive una riga «chiamata<TAB>percorso» per ogni apertura,
// stat, accesso, cartella, dlopen ed exec in $TRACCIA_FILE (in coda: anche i processi figli, che ereditano l'ambiente).
// Vede le chiamate del gioco, di libstdc++, SDL e Lua, non quelle interne di glibc. Scritto il 07/11/2026.
// Compilazione: gcc -O2 -shared -fPIC -o traccia_file.so traccia_file.c -ldl
#define _GNU_SOURCE
#include <dirent.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <spawn.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>

static int registro = -1;

static void annota(const char* chiamata, const char* percorso) {
    if (registro < 0) {
        const char* f = getenv("TRACCIA_FILE");
        if (!f) return;
        registro = (int)syscall(SYS_openat, AT_FDCWD, f, O_WRONLY | O_CREAT | O_APPEND | O_CLOEXEC, 0644);
        if (registro < 0) return;
    }
    char riga[4352];
    const int n = snprintf(riga, sizeof riga, "%s\t%s\n", chiamata, percorso ? percorso : "(null)");
    if (n > 0) (void)!write(registro, riga, (size_t)(n < (int)sizeof riga ? n : (int)sizeof riga - 1));
}

// relativo a dirfd: la cartella corrente si scrive davanti, cosi' l'elenco ha solo percorsi assoluti
static void annota_at(const char* chiamata, int dirfd, const char* percorso) {
    if (percorso && percorso[0] != '/' && dirfd == AT_FDCWD) {
        char c[4096], p[4352];
        if (getcwd(c, sizeof c)) {
            snprintf(p, sizeof p, "%s/%s", c, percorso);
            annota(chiamata, p);
            return;
        }
    }
    annota(chiamata, percorso);
}

#define VERA(tipo, nome, ...) static tipo (*vera)(__VA_ARGS__); if (!vera) vera = (tipo (*)(__VA_ARGS__))dlsym(RTLD_NEXT, nome)

static mode_t modo(int flags, va_list a) { return (flags & (O_CREAT | O_TMPFILE)) ? (mode_t)va_arg(a, int) : 0; }

int open(const char* p, int flags, ...) { va_list a; va_start(a, flags); mode_t m = modo(flags, a); va_end(a); VERA(int, "open", const char*, int, ...); annota_at("open", AT_FDCWD, p); return vera(p, flags, m); }
int open64(const char* p, int flags, ...) { va_list a; va_start(a, flags); mode_t m = modo(flags, a); va_end(a); VERA(int, "open64", const char*, int, ...); annota_at("open", AT_FDCWD, p); return vera(p, flags, m); }
int openat(int d, const char* p, int flags, ...) { va_list a; va_start(a, flags); mode_t m = modo(flags, a); va_end(a); VERA(int, "openat", int, const char*, int, ...); annota_at("openat", d, p); return vera(d, p, flags, m); }
int openat64(int d, const char* p, int flags, ...) { va_list a; va_start(a, flags); mode_t m = modo(flags, a); va_end(a); VERA(int, "openat64", int, const char*, int, ...); annota_at("openat", d, p); return vera(d, p, flags, m); }
int __open_2(const char* p, int flags) { VERA(int, "__open_2", const char*, int); annota_at("open", AT_FDCWD, p); return vera(p, flags); }
int __open64_2(const char* p, int flags) { VERA(int, "__open64_2", const char*, int); annota_at("open", AT_FDCWD, p); return vera(p, flags); }
int __openat_2(int d, const char* p, int flags) { VERA(int, "__openat_2", int, const char*, int); annota_at("openat", d, p); return vera(d, p, flags); }
FILE* fopen(const char* p, const char* m) { VERA(FILE*, "fopen", const char*, const char*); annota_at("fopen", AT_FDCWD, p); return vera(p, m); }
FILE* fopen64(const char* p, const char* m) { VERA(FILE*, "fopen64", const char*, const char*); annota_at("fopen", AT_FDCWD, p); return vera(p, m); }
FILE* freopen(const char* p, const char* m, FILE* f) { VERA(FILE*, "freopen", const char*, const char*, FILE*); annota_at("fopen", AT_FDCWD, p); return vera(p, m, f); }
int stat(const char* p, struct stat* s) { VERA(int, "stat", const char*, struct stat*); annota_at("stat", AT_FDCWD, p); return vera(p, s); }
int stat64(const char* p, struct stat64* s) { VERA(int, "stat64", const char*, struct stat64*); annota_at("stat", AT_FDCWD, p); return vera(p, s); }
int lstat(const char* p, struct stat* s) { VERA(int, "lstat", const char*, struct stat*); annota_at("stat", AT_FDCWD, p); return vera(p, s); }
int lstat64(const char* p, struct stat64* s) { VERA(int, "lstat64", const char*, struct stat64*); annota_at("stat", AT_FDCWD, p); return vera(p, s); }
int fstatat(int d, const char* p, struct stat* s, int f) { VERA(int, "fstatat", int, const char*, struct stat*, int); annota_at("stat", d, p); return vera(d, p, s, f); }
int fstatat64(int d, const char* p, struct stat64* s, int f) { VERA(int, "fstatat64", int, const char*, struct stat64*, int); annota_at("stat", d, p); return vera(d, p, s, f); }
int statx(int d, const char* p, int f, unsigned m, struct statx* s) { VERA(int, "statx", int, const char*, int, unsigned, struct statx*); annota_at("stat", d, p); return vera(d, p, f, m, s); }
int access(const char* p, int m) { VERA(int, "access", const char*, int); annota_at("access", AT_FDCWD, p); return vera(p, m); }
int faccessat(int d, const char* p, int m, int f) { VERA(int, "faccessat", int, const char*, int, int); annota_at("access", d, p); return vera(d, p, m, f); }
DIR* opendir(const char* p) { VERA(DIR*, "opendir", const char*); annota_at("opendir", AT_FDCWD, p); return vera(p); }
ssize_t readlink(const char* p, char* b, size_t n) { VERA(ssize_t, "readlink", const char*, char*, size_t); annota_at("readlink", AT_FDCWD, p); return vera(p, b, n); }
void* dlopen(const char* p, int f) { VERA(void*, "dlopen", const char*, int); if (p) annota("dlopen", p); return vera(p, f); }
int execve(const char* p, char* const a[], char* const e[]) { VERA(int, "execve", const char*, char* const[], char* const[]); annota_at("execve", AT_FDCWD, p); return vera(p, a, e); }
int execv(const char* p, char* const a[]) { VERA(int, "execv", const char*, char* const[]); annota_at("execve", AT_FDCWD, p); return vera(p, a); }
int execvp(const char* p, char* const a[]) { VERA(int, "execvp", const char*, char* const[]); annota("execvp", p); return vera(p, a); }
int system(const char* c) { VERA(int, "system", const char*); annota("system", c); return vera(c); }
int posix_spawn(pid_t* pid, const char* p, const posix_spawn_file_actions_t* fa, const posix_spawnattr_t* at, char* const a[], char* const e[]) {
    VERA(int, "posix_spawn", pid_t*, const char*, const posix_spawn_file_actions_t*, const posix_spawnattr_t*, char* const[], char* const[]);
    annota_at("posix_spawn", AT_FDCWD, p);
    return vera(pid, p, fa, at, a, e);
}
int posix_spawnp(pid_t* pid, const char* p, const posix_spawn_file_actions_t* fa, const posix_spawnattr_t* at, char* const a[], char* const e[]) {
    VERA(int, "posix_spawnp", pid_t*, const char*, const posix_spawn_file_actions_t*, const posix_spawnattr_t*, char* const[], char* const[]);
    annota("posix_spawnp", p);
    return vera(pid, p, fa, at, a, e);
}
