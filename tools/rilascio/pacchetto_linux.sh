#!/bin/bash
# Pacchetto Linux di anteprima, autonomo: rilascio/melchior688-anteprima-AAAA-MM-GG-linux-x86_64-PRIVATO.tar.xz nella
# radice del progetto, dai binari della build di rilascio (cmake -DM688_RILASCIO=ON, gia' verificati: casi, rilievo, suite).
# Uso: [BASE=si] [MT32=CARTELLA] pacchetto_linux.sh BUILD/melchior688 [AAAA-MM-GG]
# Scritto il 07/11/2026. Il pacchetto e' PRIVATO (scelta dell'utente del 07/11/2026, «B: uno solo privato»): contiene i
# dati ricavati dal gioco originale e la musica resa con le ROM MT-32, per uso personale, da non distribuire.
#
# Estratto, il pacchetto ha tutto: bin/melchior688 parte da solo (il menu), senza lanciatore e senza variabili.
# - bin/: melchior688, serie_sandbox e serie_ab (S e A delle regolazioni della sandbox, che il gioco cerca accanto a se'),
#   dalla stessa build. Non si ricompilano: si tolgono solo i simboli di debug (strip --strip-debug) e lo script controlla
#   che .text, .rodata e .data restino identici. melchior688 deve avere il DT_RPATH $ORIGIN/../lib della build di rilascio;
# - lib/: le sole librerie del sysroot (libSDL3 3.2.10 e libsndio, sua dipendenza), trovate col DT_RPATH. Le altre
#   (glibc, libstdc++, X11, Wayland, PulseAudio, PipeWire, ALSA) sono del sistema;
# - data/motore, data/suoni, data/missioni: il gioco le trova accanto all'eseguibile, in ../data (app/percorsi_pacchetto.hpp).
#   data/missioni/prove: solo vetrini.toml e le missioni coi vetrini «pronto» (la sandbox), non le altre prove;
# - data/mt32: i brani della musica resi con le ROM Roland MT-32 (da $MT32, predefinita ~/.cache/melchior688/mt32), che
#   il gioco cerca prima della cache dell'utente (RegiaAudio::cartella_mt32_per). Le ROM no. Solo nel pacchetto privato;
# - i file derivati dal gioco originale: i «derivati» delle voci di categoria «originale» di crediti.toml. Nel
#   pacchetto privato ci sono, elencati in LICENZE/PROPRIETARI.txt. Con BASE=si
#   restano fuori e il tar perde «-PRIVATO»: e' il pacchetto distribuibile per quando il motore li importera' dalla
#   copia del giocatore. Oggi senza di loro il gioco non parte (palette/giorno.pal), e BASE=si non e' verificato;
# - mai nel pacchetto, in nessun caso: original-game/, extracted/, re/, roland-mt32/, le ROM MT-32 e i sorgenti;
# - LICENZE/: tools/pipeline/licenze.py (sessione dei crediti): CREDITI.txt, PROPRIETARI.txt e le licenze delle librerie;
# - manuale/: i quattro volumi in PDF del kit (rilascio/manuale/manuale_[1-4]_*.pdf), insieme perche' i rimandi fra
#   volumi valgano; il Markdown no. Il colophon rimanda a LICENZE/CREDITI.txt, che licenze.py deve scrivere;
# - 688.sh: facoltativo, lancia bin/melchior688 con le opzioni date.
# Il gioco scrive solo nella casa dell'utente (~/.local/share, ~/.config), mai nel pacchetto.
# xz: -9 a blocchi da 32 MiB, cosi' usa 6 fili (15 s invece di 111 s, stessa dimensione).
# I file di lavoro vanno in rilascio/.lavoro.PID (su disco, non in /tmp) e si tolgono alla fine.
set -euo pipefail
R=$(cd "$(dirname "$0")/../.." && pwd)
B=${1:?uso: [BASE=si] [MT32=CARTELLA] pacchetto_linux.sh BUILD/melchior688 [AAAA-MM-GG]}
DATA=${2:-$(date +%F)}
BASE=${BASE:-no}
SYSROOT=$R/tools/deps/sysroot/usr/lib/x86_64-linux-gnu
NOME=melchior688-anteprima-$DATA-linux-x86_64
[ "$BASE" = si ] || NOME=$NOME-PRIVATO
USCITA=${USCITA:-$R/rilascio}   # la cartella del tar (un tar di prova altrove)
L=$USCITA/.lavoro.$$
P=$L/$NOME
[[ $DATA =~ ^[0-9]{4}-[0-9]{2}-[0-9]{2}$ ]] || { echo "data non valida (AAAA-MM-GG): $DATA"; exit 1; }
file -L "$B" | grep -q "ELF 64-bit.*x86-64" || { echo "non e' un eseguibile x86-64: $B"; exit 1; }
STRUMENTI=$(dirname "$B")
MT32=${MT32:-$HOME/.cache/melchior688/mt32}
for x in serie_sandbox serie_ab; do [ -x "$STRUMENTI/$x" ] || { echo "manca $STRUMENTI/$x (della stessa build)"; exit 1; }; done
readelf -d "$B" | grep -q 'RPATH.*\[\$ORIGIN/../lib\]' || { echo "$B non e' della build di rilascio (cmake -DM688_RILASCIO=ON)"; exit 1; }
[ "$BASE" = si ] || ls "$MT32"/*.wav > /dev/null 2>&1 || { echo "nessun brano MT-32 in $MT32"; exit 1; }
for x in strip objcopy objdump xz tar python3; do command -v $x > /dev/null || { echo "manca $x"; exit 1; }; done
mkdir -p "$P"/{bin,lib,data,LICENZE}
trap 'rm -rf "$L"' EXIT

# --- il binario, senza i simboli di debug, e il controllo che il codice sia lo stesso ---
strip --strip-debug -o "$P/bin/melchior688" "$B"
sezioni() { for s in .text .rodata .data; do objcopy -O binary --only-section=$s "$1" "$L/sezione" && md5sum < "$L/sezione" | cut -c1-32; done; rm -f "$L/sezione"; }
[ "$(sezioni "$B")" = "$(sezioni "$P/bin/melchior688")" ] || { echo "il codice del binario e' cambiato con lo strip"; exit 1; }
echo "binario: $(md5sum < "$B" | cut -c1-32), $(( $(stat -c %s "$B") / 1048576 )) MB -> $(( $(stat -c %s "$P/bin/melchior688") / 1048576 )) MB, .text/.rodata/.data identici"

# --- gli strumenti della sandbox (S e A), accanto al gioco ---
for x in serie_sandbox serie_ab; do
    strip --strip-debug -o "$P/bin/$x" "$STRUMENTI/$x"
    [ "$(sezioni "$STRUMENTI/$x")" = "$(sezioni "$P/bin/$x")" ] || { echo "il codice di $x e' cambiato con lo strip"; exit 1; }
done
echo "strumenti: serie_sandbox $(md5sum < "$STRUMENTI/serie_sandbox" | cut -c1-32), serie_ab $(md5sum < "$STRUMENTI/serie_ab" | cut -c1-32)"
echo "percorsi del progetto nei binari (solo stringhe di diagnostica): $(for x in melchior688 serie_sandbox serie_ab; do echo -n "$x $(strings "$P/bin/$x" | grep -c /home/) "; done)"

# --- le librerie del sysroot ---
cp -L "$SYSROOT/libSDL3.so.0" "$SYSROOT/libsndio.so.7" "$P/lib/"

# --- i dati: l'elenco dei file dell'originale (e la loro scheda per LICENZE), le prove non usate, poi la copia ---
python3 -I - "$R" > "$L/originali.txt" <<'FINE'
import pathlib, sys, tomllib
R = pathlib.Path(sys.argv[1])
crediti = tomllib.loads((R / 'data/motore/crediti.toml').read_text(encoding='utf-8'))
# i file ricavati dal gioco del 1989: i derivati delle voci di categoria «originale» (tools/pipeline/crediti.py)
originali = {d.rstrip('/') for a in crediti['asset'] if a.get('categoria') == 'originale' for d in a['derivati']}
for d in sorted(originali):
    if not (R / d).exists():
        sys.exit(f'derivato elencato ma assente: {d}')
    print(d)
FINE
python3 -I - "$R" > "$L/prove.txt" <<'FINE'
import pathlib, sys, tomllib
R = pathlib.Path(sys.argv[1])
prove = R / 'data/missioni/prove'
v = tomllib.loads((prove / 'vetrini.toml').read_text(encoding='utf-8'))
pronte = {x['missione'] for x in v['vetrino'] if x.get('pronto')}
# fuori le prove che la sandbox non mostra: si raggiungono solo con --missione prove/X
for f in sorted(prove.iterdir()):
    if f.name != 'vetrini.toml' and f.stem not in pronte:
        print(f.relative_to(R))
FINE
if [ "$BASE" = si ]; then cp "$L/originali.txt" "$L/esclusi.txt"; else cp "$L/prove.txt" "$L/esclusi.txt"; fi
[ "$BASE" = si ] && cat "$L/prove.txt" >> "$L/esclusi.txt"
tar -C "$R" -cf - --anchored --no-wildcards --exclude-from="$L/esclusi.txt" \
    data/motore data/suoni data/missioni | tar -C "$P" -xf -
echo "dati: $(du -sm "$P/data" | cut -f1) MB; $(wc -l < "$L/originali.txt") voci dell'originale $([ "$BASE" = si ] && echo fuori || echo dentro); fuori $(wc -l < "$L/prove.txt") prove non usate"

# --- il lanciatore, facoltativo ---
cat > "$P/688.sh" <<'FINE'
#!/bin/sh
# 688 Attack Sub, remake: avvia il gioco (senza opzioni, il menu). Equivale a bin/melchior688, che trova da solo i
# dati e le librerie di questa cartella; le opzioni date qui vanno al gioco (per esempio ./688.sh --missione SCENE1A).
exec "$(dirname "$(readlink -f "$0")")/bin/melchior688" "$@"
FINE
chmod 755 "$P/688.sh" "$P/bin/"*

# --- il manuale e la musica MT-32 ---
MANUALE=$R/rilascio/manuale
mkdir -p "$P/manuale"
for v in 1_uso 2_riferimento 3_storia 4_tattiche; do cp "$MANUALE/manuale_$v.pdf" "$P/manuale/"; done
if [ "$BASE" != si ]; then
    mkdir -p "$P/data/mt32"
    cp "$MT32"/*.wav "$P/data/mt32/"
    echo "mt32: $(ls "$P/data/mt32" | wc -l) brani, $(du -sm "$P/data/mt32" | cut -f1) MB da $MT32"
fi

# --- licenze e crediti: tools/pipeline/licenze.py da crediti.toml (CREDITI.txt, PROPRIETARI.txt, licenze delle librerie) ---
if [ "$BASE" = si ]; then
    "$R/tools/venv/bin/python" -I "$R/tools/pipeline/licenze.py" "$P/LICENZE" --base
else
    "$R/tools/venv/bin/python" -I "$R/tools/pipeline/licenze.py" "$P/LICENZE" --mt32 "$P/data/mt32"
fi
[ -s "$P/LICENZE/CREDITI.txt" ] || { echo "manca LICENZE/CREDITI.txt, a cui rimanda il colophon del manuale"; exit 1; }

# --- LEGGIMI ---
GLIBC=$(objdump -T "$P/bin/melchior688" "$P/lib/"*.so.* | grep -o 'GLIBC_[0-9.]*' | sort -uV | tail -1 | cut -d_ -f2)
GLIBCXX=$(objdump -T "$P/bin/melchior688" | grep -o 'GLIBCXX_[0-9.]*' | sort -uV | tail -1 | cut -d_ -f2)
{
[ "$BASE" = si ] || printf '%s\n\n' "Uso personale, non distribuire: contiene dati ricavati dal gioco originale (Electronic Arts, 1989)."
cat <<FINE
688 Attack Sub - remake (Melchior)    ANTEPRIMA del $DATA, Linux x86-64

E' un'anteprima: non e' la versione finale, puo' avere errori e cambiera'.

REQUISITI
- Linux x86-64 recente: compilato su Debian 13. Servono glibc $GLIBC o piu' recente e libstdc++ con
  GLIBCXX_$GLIBCXX (GCC 13 o piu' recente): Debian 13, Ubuntu 24.04, Fedora 39 o successive.
- Le librerie grafiche e audio del desktop: X11 o Wayland, PulseAudio o PipeWire, ALSA, libdecor, libgbm,
  libxkbcommon. SDL3 e sndio sono nel pacchetto (lib/).

AVVIO
  bin/melchior688      (oppure ./688.sh, che fa lo stesso)
Si apre il menu. Tutto quello che serve e' in questa cartella: dati, musica, librerie SDL3 e sndio (lib/).
Le opzioni date sulla riga di comando vanno al gioco.

MUSICA
FM dal vivo (predefinita), MT-32 (i brani di data/mt32) o i titoli del 1989: si sceglie col tasto M nel menu.

MANUALE
In manuale/, quattro volumi in PDF: 1 uso, 2 riferimento tecnico, 3 storia e unita', 4 tattiche. Vanno tenuti
nella stessa cartella: i rimandi da un volume all'altro li cercano li'.

DOVE SCRIVE
Mai nella cartella del pacchetto. Nella casa dell'utente:
  ~/.local/share/melchior688/   fascicoli, carriera, replay, missioni dell'editor, rapide e importate
                                (o \$XDG_DATA_HOME/melchior688)
  ~/.config/melchior688/        preferenze.toml e tasti.toml

LICENZE
In LICENZE/: le librerie e i crediti di foto, disegni e suoni.
FINE
[ "$BASE" = si ] || echo "In LICENZE/PROPRIETARI.txt: i file ricavati dal gioco originale."
} > "$P/LEGGIMI.txt"

# --- il tar ---
mkdir -p "$USCITA"
T=$USCITA/$NOME.tar.xz
t0=$SECONDS
tar -C "$L" --sort=name --owner=0 --group=0 --numeric-owner -cf - "$NOME" | xz -T6 -9 --block-size=32MiB > "$T.parziale"
mv "$T.parziale" "$T"
echo "tar: $T, $(( $(stat -c %s "$T") / 1048576 )) MB, compresso in $(( SECONDS - t0 )) s"
(cd "$(dirname "$T")" && sha256sum "$(basename "$T")") > "$T.sha256"   # formato di sha256sum: si controlla con sha256sum -c
