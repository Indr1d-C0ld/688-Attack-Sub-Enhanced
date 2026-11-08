#!/bin/bash
# Controllo di sintassi di alcuni sorgenti con i comandi della build (compile_commands.json), senza produrre oggetti:
# utile mentre un'altra sessione sta cambiando un header condiviso e non si deve ancora compilare davvero.
# uso: controlla_sintassi.sh <cartella di build> <sorgente>...
set -e
build=$1; shift
for f in "$@"; do
    abs=$(realpath "$f")
    cmd=$(python3 -I -c "
import json,sys,shlex
for e in json.load(open(sys.argv[1])):
    if e['file']==sys.argv[2]:
        a=shlex.split(e['command']); i=a.index('-o'); del a[i:i+2]
        a=[x for x in a if x!='-c']
        print(shlex.join(a+['-fsyntax-only']), '&&cd', e['directory']); break
" "$build/compile_commands.json" "$abs")
    [ -z "$cmd" ] && { echo "non trovato: $f"; continue; }
    dir=${cmd##*&&cd }; cmd=${cmd% &&cd *}
    (cd "$dir" && eval "$cmd") && echo "ok: $f"
done
