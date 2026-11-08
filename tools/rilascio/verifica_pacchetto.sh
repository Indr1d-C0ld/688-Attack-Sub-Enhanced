#!/bin/bash
# Verifica del pacchetto autonomo (tools/rilascio/pacchetto_linux.sh): lo estrae in una cartella pulita su disco e lo fa
# girare isolato, con /home/randolph vuota (un tmpfs con dentro solo la cartella di verifica: niente progetto, niente
# cache, ROM o preferenze dell'utente), una casa di prova vuota e l'ambiente minimo. Uso: verifica_pacchetto.sh TAR [CARTELLA]
# (predefinita ~/Claude/.verifica-pacchetto-688, tolta e rifatta). Scritto il 07/11/2026 (sessione del pacchetto).
#   1. ldd dei tre binari: niente «not found», SDL3 e sndio da lib/;
#   2. casi.sh col binario del pacchetto, diff con tools/regressione/riferimento.txt: diverse solo la riga del manuale
#      (manuale.py sta nel progetto, che e' nascosto);
#   3. sotto traccia_file.so (LD_PRELOAD, al posto di strace che manca): le schermate senza finestra (video, menu,
#      Museo ed enciclopedia, classico, Enhanced con briefing, sala siluri, debriefing ed esito, sandbox col vetrino di
#      Dio, rapide, editor, albo, statistiche) e le serie S (serie_sandbox) e A (serie_ab) come le lancia il gioco;
#   4. l'elenco dei file aperti fuori dal pacchetto, esclusi il sistema (/usr /lib /etc /dev /proc /sys /run) e la casa
#      di prova: deve essere vuoto. A parte i percorsi del pacchetto cercati e non trovati (spesso facoltativi).
# La partita sotto X con la musica FM e MT-32 si fa a parte con partita_pacchetto.py, che usa la stessa isola e la stessa
# traccia (CARTELLA/traccia.log): poi `verifica_pacchetto.sh --solo-elenco CARTELLA` rifa' l'elenco.
set -u
R=$(cd "$(dirname "$0")/../.." && pwd)
QUI=$(cd "$(dirname "$0")" && pwd)
if [ "${1:-}" = --solo-elenco ]; then SOLO=1; V=${2:?CARTELLA}; else SOLO=0; T=${1:?uso: verifica_pacchetto.sh TAR [CARTELLA]}; V=${2:-$HOME/Claude/.verifica-pacchetto-688}; fi
case "$V" in /tmp/*) echo "non in /tmp (tmpfs): $V"; exit 1;; esac

elenco() {
    local P; P=$(ls -d "$V"/estratto/*/ | head -1); P=${P%/}
    echo "=== file aperti fuori dal pacchetto (escluso il sistema e la casa di prova); deve essere vuoto:"
    awk -F'\t' '$1 != "system" {print $2}' "$V/traccia.log" | sort -u | grep -v -E "^(/usr|/lib|/lib64|/etc|/dev|/proc|/sys|/run|/var/cache/libx11|/tmp/\.X11-unix|/tmp/\.ICE-unix)(/|$)" \
        | grep -v -F -e "$P" -e "$V/casa" -e "$V/uscite" -e "$V/xauth" | grep -v -x -F "$V" | grep -v -E '^(\(null\)|/|/tmp|/home|/home/randolph|/home/randolph/Claude)$' \
        | grep -v -E '^[a-z]' | tee "$V/fuori.txt"
    echo "=== ($(wc -l < "$V/fuori.txt") voci; i comandi di system() si leggono in $V/traccia.log)"
    echo "=== percorsi del pacchetto cercati e non trovati (facoltativi o mancanti):"
    awk -F'\t' '$1 != "system" {print $2}' "$V/traccia.log" | grep -F "$P/" | sort -u | while read -r f; do [ -e "$f" ] || echo "  ${f#$P/}"; done | head -40
}
[ $SOLO = 1 ] && { elenco; exit 0; }

rm -rf "$V"; mkdir -p "$V"/{estratto,casa,uscite,casi} || exit 1
tar -C "$V/estratto" -xJf "$T" || exit 1
P=$(ls -d "$V"/estratto/*/ | head -1); P=${P%/}
gcc -O2 -shared -fPIC -o "$V/traccia_file.so" "$QUI/traccia_file.c" -ldl || exit 1
cp "$R/tools/regressione/casi.sh" "$R/tools/regressione/riferimento.txt" "$V/"
cp "${XAUTHORITY:-$HOME/.Xauthority}" "$V/xauth" 2>/dev/null
A=/tmp/m688-isola-$$; mkdir -p "$A"
# l'isola: /home/randolph e' un tmpfs con dentro solo la cartella di verifica (e una cartella engine/ vuota, per il cd di
# casi.sh); la casa di prova e l'ambiente minimo
cat > "$V/isola.sh" <<FINE
#!/bin/sh
mount --bind "$V" "$A" && mount -t tmpfs isola /home/randolph && mkdir -p "$V" "$R/engine" && mount --bind "$A" "$V" || exit 1
cd "$V/uscite" || exit 1   # la cartella corrente: la shell di system() ne fa stat all'avvio
exec env -i HOME="$V/casa" PATH=/usr/bin:/bin DISPLAY="\${DISPLAY:-}" XAUTHORITY="$V/xauth" XDG_RUNTIME_DIR="\${XDG_RUNTIME_DIR:-}" \\
    \${TRACCIA:+LD_PRELOAD="$V/traccia_file.so" TRACCIA_FILE="$V/traccia.log"} \${SDL_AUDIO_DRIVER:+SDL_AUDIO_DRIVER="\$SDL_AUDIO_DRIVER"} \\
    \${SDL_AUDIO_DISK_OUTPUT_FILE:+SDL_AUDIO_DISK_OUTPUT_FILE="\$SDL_AUDIO_DISK_OUTPUT_FILE"} SDL_VIDEO_X11_XINPUT2=0 "\$@"
FINE
chmod +x "$V/isola.sh"
# SOLO_ISOLA=1: solo il pacchetto estratto e l'isola, per una partita con partita_pacchetto.py (schermate del README)
[ "${SOLO_ISOLA:-}" = 1 ] && { echo "isola pronta: $V/isola.sh (punto di montaggio $A: rmdir alla fine)"; exit 0; }
isola() { unshare -rm "$V/isola.sh" "$@"; }

echo "=== 1. ldd (dentro l'isola, senza LD_LIBRARY_PATH)"
for b in melchior688 serie_sandbox serie_ab; do
    echo "  $b: $(isola ldd "$P/bin/$b" | grep -c 'not found') mancanti; dal pacchetto: $(isola ldd "$P/bin/$b" | grep -oE "$P/bin/\.\./lib/[^ ]+|$P/lib/[^ ]+" | xargs -r -n1 basename | tr '\n' ' ')"
done

echo "=== 2. casi.sh col binario del pacchetto"
t=$SECONDS
isola sh -c "CARTELLA='$V/casi' bash '$V/casi.sh' '$P/bin/melchior688'" > "$V/casi.txt" 2>&1
diff "$V/riferimento.txt" "$V/casi.txt"; echo "  $(diff "$V/riferimento.txt" "$V/casi.txt" | grep -c '^>') righe diverse, $(( SECONDS - t )) s"

echo "=== 3. schermate e serie sotto la traccia"
U=$V/uscite
schermata() { local n=$1; shift; TRACCIA=1 isola "$P/bin/melchior688" "$@" --istantanea "$U/$n.bmp" > "$U/$n.txt" 2>&1; echo "  $n: rc $?$( [ -s "$U/$n.bmp" ] || echo ', NESSUNA ISTANTANEA')"; }
schermata apertura --schermata apertura --secondi 20 --audio-wav "$U/apertura.wav"
schermata menu --menu
schermata rapide --menu --schermata scelta-rapida
schermata editor_scelta --menu --schermata scelta-editor
schermata museo --schermata museo
schermata enciclopedia --schermata museo --voce sottomarino.los_angeles --evento 1
schermata albo --schermata albo
schermata fascicolo --schermata fascicolo
schermata statistiche_cumulate --schermata statistiche-cumulate
schermata classico --missione SCENE1A --secondi 120 --stazione 8
schermata classico_alfa --missione SCENE3B --secondi 60 --stazione 4
schermata briefing --missione SCENE5A --regola simulazione --schermata briefing
schermata enhanced --missione SCENE5A --regola simulazione --secondi 600 --stazione 8 --traffico si
schermata siluri --missione SCENE1A --regola simulazione --schermata siluri
schermata debriefing --missione SCENE1A --regola simulazione --secondi 300 --schermata debriefing
schermata esito --missione SCENE1A --regola simulazione --secondi 300 --schermata esito
schermata sandbox --sandbox --esperimento "$P/data/motore/esperimenti/burrasca.m688v" --seme 1 --secondi 600
schermata sandbox_vivo --sandbox --esperimento "$P/data/motore/esperimenti/burrasca.m688v" --seme 1 --secondi 300 --schermata vivo
schermata scena_audio --audio-wav "$U/scena.wav" --colpo siluro_chiglia --secondi 10
t=$SECONDS
TRACCIA=1 isola "$P/bin/serie_sandbox" --codice 8FZD3Q-4G0000-04K000-0000HZ-G002QH --semi 2 --lavori 2 --secondi 1800 \
    --motore "$P/bin/melchior688" --uscita "$U/serieS" > "$U/serieS.txt" 2>&1
echo "  S (serie_sandbox, 2 semi): rc $?, $(( SECONDS - t )) s, $(ls "$U"/serieS/*.m688tr 2>/dev/null | wc -l) tracce, serie.toml $([ -s "$U/serieS/serie.toml" ] && echo si || echo NO)"
schermata serie_schermata --serie "$U/serieS/serie.toml" --schermata serie
t=$SECONDS
TRACCIA=1 isola "$P/bin/serie_ab" --esperimento "$P/data/motore/esperimenti/burrasca.m688v" --semi 1 --lavori 2 \
    --motore "$P/bin/melchior688" --serie "$P/bin/serie_sandbox" --uscita "$U/serieA" > "$U/serieA.txt" 2>&1
echo "  A (serie_ab, 1 seme): rc $?, $(( SECONDS - t )) s, $(find "$U/serieA" -name '*.m688tr' | wc -l) tracce, $(find "$U/serieA" -name '*.bmp' -o -name '*.png' | wc -l) pagine"
rmdir "$A" 2>/dev/null
echo "=== 4."
elenco
