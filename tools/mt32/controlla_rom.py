"""Le ROM dell'MT-32 restano dell'utente (roadmap 38, E; nota condivisa disassemblatore-audio-inventario-proposta.md §4):
il controllo che nessun loro derivato finisca nel progetto distribuibile.

Le ROM stanno in ~/.local/share/melchior688/mt32/ (o nella cartella di M688_MT32), i brani resi con esse in
~/.cache/melchior688/mt32/ (tools/mt32/rendi_mt32). Qui si fanno le impronte SHA-1 di tutti quei file e si cercano,
per impronta e per nome, nelle cartelle del progetto che si distribuiscono o da cui si genera (data/, docs/, engine/
fuori dalle build, tools/pipeline/). Si cercano anche nella cartella delle ROM stessa i file che non sono ROM ne' gli
archivi dell'utente (un derivato messo li' per sbaglio). Una sola cartella e' ammessa nel progetto: quella in cui
l'utente ha messo l'archivio (roland-mt32/), che non va mai distribuita.
Uso: controlla_rom.py <radice del progetto>   (esce con 1 se trova qualcosa)"""
import hashlib, os, sys

ROM = os.environ.get('M688_MT32') or os.path.expanduser('~/.local/share/melchior688/mt32')
CACHE = os.path.expanduser('~/.cache/melchior688/mt32')
CARTELLE = ('data', 'docs', 'engine', os.path.join('tools', 'pipeline'))
SALTA = ('build', '.git')
CONSENTITA = 'roland-mt32'   # dove l'utente ha messo l'archivio: nel progetto, ma mai distribuita
ESTENSIONI_ROM = ('.rom', '.ic8', '.ic21', '.ic22', '.ic26', '.ic27', '.ic28', '.ic37')


def impronta(p):
    h = hashlib.sha1()
    with open(p, 'rb') as f:
        for blocco in iter(lambda: f.read(1 << 20), b''):
            h.update(blocco)
    return h.hexdigest()


def file_in(cartella):
    for radice, dirs, files in os.walk(cartella):
        dirs[:] = [d for d in dirs if not d.startswith(SALTA)]
        for f in files:
            yield os.path.join(radice, f)


def controlla(radice):
    protette = {}
    for c in (ROM, CACHE):
        if os.path.isdir(c):
            for p in file_in(c):
                protette.setdefault(impronta(p), p)
    errori = []
    # nella cartella delle ROM: solo ROM e archivi
    if os.path.isdir(ROM):
        for p in file_in(ROM):
            nome = p.lower()
            if not (nome.endswith(ESTENSIONI_ROM) or nome.endswith('.bin') or nome.endswith(('.zip', '.txt', '.md')) or '.ic' in os.path.basename(nome)):
                errori.append(f'nella cartella delle ROM un file che non e\' una ROM: {p}')
    # nel progetto: niente per impronta, niente col nome d'una ROM
    dimensioni = {os.path.getsize(p) for p in protette.values()}
    for c in CARTELLE:
        base = os.path.join(radice, c)
        if not os.path.isdir(base):
            continue
        for p in file_in(base):
            rel = os.path.relpath(p, radice)
            if os.path.basename(p).lower().endswith(ESTENSIONI_ROM) or os.path.basename(p).lower().startswith(('ctrl_mt32', 'pcm_mt32', 'ctrl_cm32', 'pcm_cm32')):
                errori.append(f'un file col nome d\'una ROM dell\'MT-32 nel progetto: {rel}')
                continue
            try:
                if os.path.getsize(p) in dimensioni and impronta(p) in protette:
                    errori.append(f'{rel} e\' uguale a {protette[impronta(p)]}')
            except OSError:
                pass
    avvisi = []
    if os.path.isdir(os.path.join(radice, CONSENTITA)):
        avvisi.append(f'{CONSENTITA}/ contiene le ROM dell\'utente dentro il progetto: va esclusa da ogni pacchetto')
    return errori, avvisi, len(protette)


if __name__ == '__main__':
    errori, avvisi, n = controlla(sys.argv[1] if len(sys.argv) > 1 else '.')
    for a in avvisi:
        print('avviso:', a)
    for e in errori:
        print('ERRORE:', e)
    print(f'ROM e brani dell\'utente controllati: {n}; problemi: {len(errori)}')
    sys.exit(1 if errori else 0)
