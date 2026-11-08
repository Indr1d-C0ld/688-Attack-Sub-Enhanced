#!/bin/bash
# La copia del progetto per il repository GitHub privato «688 Attack Sub Enhanced». Il progetto resta senza git: la copia
# sta in una cartella a parte (predefinita ~/Claude/688-attack-sub-enhanced), ha il suo .git e si riallinea rilanciando
# lo script (rsync --delete: quello che esce dall'elenco sparisce anche dalla copia; .git e i file messi da qui restano).
# Uso: esporta_repo.sh [CARTELLA]. Scritto l'08/11/2026 (sessione del pacchetto).
#
# Entra, con un elenco esplicito (le regole sotto, la prima che combacia vince):
#   - engine/ senza le build: app, grafica, missioni, nucleo, piattaforma, strumenti, terzi, test, CMakeLists.txt,
#     costruisci.sh, README.md;
#   - data/ intera, compresi i file dell'originale (il repo e' privato), senza il riquadro di prova del Baltico;
#   - docs/ senza docs/trailer, senza le trascrizioni dei testi del 1989 (docs/originale/*.txt, docs/traduzione);
#   - gli strumenti nostri di tools/, senza binari compilati, scaricati, risultati e registri; di tools/re e
#     tools/re/prove solo gli script (.py .sh, piu' i pochi .lua .c .cpp che servono a rifare le prove);
#   - in testa: README.md, .gitignore e le immagini del README (da tools/rilascio/repo/), LICENZE/ (licenze.py).
# Esce: build*, toolchain scaricate (jdk, jdk21, ghidra, node, venv, dosbox, rea, deps, dl, *_dl), tools/campioni,
# tools/trailer, original-game/, extracted/, re/, roland-mt32/, rilascio/, .mcp.json, __pycache__. Alla fine controlla
# che non ci siano ROM o brani MT-32 (tools/mt32/controlla_rom.py), file oltre i 50 MiB, eseguibili ELF o link.
set -euo pipefail
R=$(cd "$(dirname "$0")/../.." && pwd)
D=${1:-$HOME/Claude/688-attack-sub-enhanced}
case "$D" in "$R"|"$R"/*) echo "la copia non va dentro il progetto: $D"; exit 1;; esac
mkdir -p "$D"

F=$(mktemp "$R/tools/rilascio/.filtro.XXXXXX"); trap 'rm -f "$F"' EXIT
cat > "$F" <<'FINE'
# protetti nella copia: non vengono dal progetto con lo stesso percorso
P /.git/
P /README.md
P /.gitignore
P /LICENZE/
P /docs/immagini_readme/
# dappertutto
- __pycache__/
- *.pyc
- .filtro.*
- *.o
- *.a
- *.so
# engine/
- /engine/build*/
+ /engine/
+ /engine/app/***
+ /engine/grafica/***
+ /engine/missioni/***
+ /engine/nucleo/***
+ /engine/piattaforma/***
+ /engine/strumenti/***
+ /engine/terzi/***
+ /engine/test/***
+ /engine/CMakeLists.txt
+ /engine/costruisci.sh
+ /engine/README.md
- /engine/*
# data/ e docs/
- /data/teatri/baltico/_riquadro_2026-10-22/
+ /data/***
- /docs/trailer/
- /docs/immagini_readme/
# le trascrizioni dei testi EA del 1989 (manuale, appendici, addendum, briefing, schede sonar) e la traduzione, che li
# riporta in inglese e in italiano: fuori anche dal repo privato. Restano le nostre analisi e le regole della traduzione
- /docs/originale/*.txt
+ /docs/traduzione/
+ /docs/traduzione/README.md
+ /docs/traduzione/glossario.md
- /docs/traduzione/**
+ /docs/***
# tools/: prima le esclusioni dentro gli strumenti che entrano
- /tools/mt32/build/
- /tools/mt32/munt/
- /tools/mt32/installato/
- /tools/mt32/*.tar.gz
- /tools/mt32/rendi_mt32
- /tools/libreria/bin/
- /tools/libreria/prova_uscita/
- /tools/taratura/collaudo_libreria*/
- /tools/taratura/risultati/
- /tools/manuale/figura_firma
- /tools/manuale/sonda_schemi_boe
- /tools/pdf/node_modules/
- /tools/pdf/controllo/
- /tools/toponimi/grezzi/naturalearth/
- /tools/rilascio/crediti_scatti/
- /tools/rilascio/repo/
- /tools/regressione/gruppo5/
- /tools/regressione/libretto_prova/
- /tools/**.log
- /tools/lua/src/lua
- /tools/lua/src/luac
- /tools/ymfm/vgmrender
+ /tools/
+ /tools/pipeline/***
+ /tools/rilascio/***
+ /tools/regressione/***
+ /tools/manuale/***
+ /tools/mt32/***
+ /tools/taratura/***
+ /tools/libreria/***
+ /tools/navi/***
+ /tools/lua/***
+ /tools/ymfm/***
+ /tools/meteo/***
+ /tools/museo/***
+ /tools/estetica/***
+ /tools/schermate/***
+ /tools/rapide/***
+ /tools/radio/***
+ /tools/anteprime/***
+ /tools/toponimi/***
+ /tools/ghidra_scripts/***
+ /tools/manuale_runtime/***
+ /tools/pdf/***
# tools/dosbox: solo i nostri script (xtasti.py serve a guida.py), non DOSBox
+ /tools/dosbox/
+ /tools/dosbox/*.py
- /tools/dosbox/**
# tools/re: solo gli script, niente risultati delle prove
+ /tools/re/
+ /tools/re/*.py
+ /tools/re/*.sh
+ /tools/re/*.lua
+ /tools/re/*.cpp
+ /tools/re/prove/
+ /tools/re/prove/**/
+ /tools/re/prove/**.py
+ /tools/re/prove/**.sh
+ /tools/re/prove/tasti/*.c
+ /tools/re/prove/tasti/*.cpp
- /tools/re/**
- /tools/*
# il resto della radice: original-game, extracted, re, roland-mt32, rilascio, .mcp.json
- /*
FINE

rsync -a --delete --delete-excluded --prune-empty-dirs --filter="merge $F" "$R/" "$D/"

# in testa alla copia
Q=$R/tools/rilascio/repo
cp "$Q/README.md" "$D/README.md"
cp "$Q/gitignore" "$D/.gitignore"
rsync -a --delete "$Q/immagini/" "$D/docs/immagini_readme/"
rm -rf "$D/LICENZE"
"$R/tools/venv/bin/python" -I "$R/tools/pipeline/licenze.py" "$D/LICENZE" > /dev/null

# controlli: niente ROM o brani MT-32, niente file oltre i 50 MiB (avviso di GitHub), niente ELF, niente link
esito=0
grandi=$(find "$D" -path "$D/.git" -prune -o -type f -size +50M -print)
[ -z "$grandi" ] || { echo "oltre 50 MiB:"; echo "$grandi"; esito=1; }
elf=$(find "$D" -path "$D/.git" -prune -o -type f -size +1k -print0 | xargs -0 -r file -N --mime-type | grep -E 'application/x-(executable|sharedlib|pie-executable|object)' || true)
[ -z "$elf" ] || { echo "binari compilati:"; echo "$elf"; esito=1; }
link=$(find "$D" -path "$D/.git" -prune -o -type l -print)
[ -z "$link" ] || { echo "link simbolici:"; echo "$link"; esito=1; }
"$R/tools/venv/bin/python" -I "$R/tools/mt32/controlla_rom.py" "$D" > /dev/null || { echo "controlla_rom.py: derivati MT-32 in data, docs, engine o tools/pipeline"; esito=1; }
# controlla_rom.py guarda solo quattro cartelle: qui tutta la copia, per impronta contro le ROM e i brani resi
"$R/tools/venv/bin/python" -I - "$D" <<'FINE' || esito=1
import hashlib, os, sys
def sha(p):
    h = hashlib.sha1()
    with open(p, 'rb') as f:
        for b in iter(lambda: f.read(1 << 20), b''): h.update(b)
    return h.hexdigest()
fonti = [os.environ.get('M688_MT32') or os.path.expanduser('~/.local/share/melchior688/mt32'), os.path.expanduser('~/.cache/melchior688/mt32')]
impronte = {sha(os.path.join(r, f)) for c in fonti for r, _, fs in os.walk(c) for f in fs}
trovati = []
for r, ds, fs in os.walk(sys.argv[1]):
    ds[:] = [d for d in ds if d != '.git']
    for f in fs:
        p = os.path.join(r, f)
        if f.lower().endswith(('.rom', '.ic8', '.ic21', '.ic22', '.ic26', '.ic27', '.ic28', '.ic37')) or sha(p) in impronte:
            trovati.append(p)
print('\n'.join(['ROM o brani MT-32 nella copia:'] + trovati) if trovati else f'MT-32: nessun derivato ({len(impronte)} impronte)')
sys.exit(1 if trovati else 0)
FINE
echo "copia: $D, $(find "$D" -path "$D/.git" -prune -o -type f -print | wc -l) file, $(du -sh --exclude=.git "$D" | cut -f1)"
exit $esito
