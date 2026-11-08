# MT-32 facoltativo (roadmap 38, E)

Le musiche del remake (`data/motore/musica/fm/*.mid`) le suona l'OPL3 del motore (`nucleo/musica_fm`). Se l'utente ha
le ROM di un Roland MT-32, questo strumento rende gli stessi brani con l'MT-32 emulato. Il gioco usa quei WAV se li
trova, altrimenti l'OPL3.

- **Le ROM** restano dell'utente, fuori dal progetto: `~/.local/share/melchior688/mt32/rom` (o `$M688_MT32/rom`).
  Servono il controllo 1.07 (`ctrl_mt32_1_07.rom`) e la PCM (`pcm_mt32.rom`).
- **I brani resi** vanno in `~/.cache/melchior688/mt32/<brano>.wav`, anche questi fuori dal progetto.
- **Munt** (libmt32emu 2.8.3, LGPL-2.1+): il sorgente è in `munt/`, il pacchetto scaricato
  (`libmt32emu_2_8_3.tar.gz`) qui accanto. Entra solo in questo strumento, mai nell'eseguibile del gioco.
- **Il controllo** `controlla_rom.py` cerca nel progetto, per impronta e per nome, qualunque derivato delle ROM o dei
  brani resi. Lo chiama anche `tools/pipeline/esporta_motore.py`, che si ferma se ne trova uno.
- **I timbri:** l'MT-32 non è General MIDI. `rendi_mt32.cpp` porta i programmi GM delle partiture sui timbri
  preimpostati dell'MT-32 e i canali 1-8 sui 2-9 (il ritmo resta sul 10).

## Compilare (da fare con il permesso dell'utente: è codice esterno)

```
cmake -S munt/munt-libmt32emu_2_8_3/mt32emu -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -Dlibmt32emu_SHARED=OFF -DCMAKE_INSTALL_PREFIX=$PWD/installato
ninja -C build install
c++ -O2 -std=c++20 -I installato/include -I ../../engine/nucleo/include -isystem ../../engine/terzi/tomlplusplus rendi_mt32.cpp ../../engine/build_acustica/libnucleo.a ../../engine/build_acustica/libymfm.a installato/lib/libmt32emu.a -o rendi_mt32
./rendi_mt32 ../../data/motore/musica/fm
```

Nota storica: il 688 del 1989 non aveva il driver MT-32, solo AdLib, Tandy e Covox. L'MT-32 è un'aggiunta del
remake, dichiarata come tale.
