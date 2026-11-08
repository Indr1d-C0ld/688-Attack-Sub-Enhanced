"""Il manuale dell'Enhanced in PDF, un file per volume, per il pacchetto di rilascio (rilascio/manuale/).

Markdown → HTML con tools/pdf/manuale_html.mjs (markdown-it, in tools/pdf/node_modules) → PDF con il Chromium di sistema.
Il Markdown non si tocca. Ogni volume ha copertina, indice coi numeri di pagina, segnalibri, testatina col volume e il
capitolo, numeri di pagina, rimandi cliccabili («capitolo F1 §2.5» porta lì, anche in un altro volume), sillabazione
italiana coi pattern di LibreOffice che porta liblouis (/usr/share/liblouis/tables/hyph_it_IT.dic).

I passi:
1. figure: le schermate a pixel ingrandite a fattore intero col vicino più prossimo, così nel PDF restano nitide; foto e
   SVG come sono (tools/pdf/build/figure, rifatte solo se il sorgente è più nuovo);
2. HTML dei quattro volumi (manuale_html.mjs);
3. prima stampa con i numeri dell'indice a «000»; dalle destinazioni del PDF (pdfinfo -dests) la pagina di ogni titolo;
4. seconda stampa coi numeri veri: la colonna dei numeri ha larghezza fissa, quindi le pagine non si spostano
   (si controlla: stesse pagine e stesse destinazioni delle due stampe);
5. i link verso un altro volume, che Chromium scrive come URI file:// assoluti, diventano azioni GoToR col nome del file e
   la destinazione, riscritte nello stesso numero di byte (gli spazi riempiono): nessun offset del PDF si sposta.

Uso: manuale_pdf.py [--volume N ...] [--uscita CARTELLA]
"""
import argparse
import datetime
import json
import math
import os
import pathlib
import re
import shutil
import subprocess
import sys

from PIL import Image

RADICE = pathlib.Path(__file__).resolve().parents[2]
MANUALE = RADICE / "docs" / "manuale_enhanced"
PDF = RADICE / "tools" / "pdf"
BUILD = PDF / "build"
NODE = RADICE / "tools" / "node" / "bin" / "node"
SILLABE = pathlib.Path("/usr/share/liblouis/tables/hyph_it_IT.dic")
CHARIS = PDF / "caratteri" / "charis-7.000"   # i quattro stili di Charis 7.000 e il suo OFL.txt
USCITA = RADICE / "rilascio" / "manuale"

# il file di ogni volume e la schermata della copertina (scelte del kit)
VOLUMI = {
    1: ("manuale_1_uso.pdf", "centrale_688.png"),
    2: ("manuale_2_riferimento.pdf", "sonar_688.png"),
    3: ("manuale_3_storia.pdf", "periscopio_688.png"),
    4: ("manuale_4_tattiche.pdf", "carta_688_mate.png"),
}
# le figure a pixel stanno a 0,25 mm per pixel del gioco, al più 165 mm (manuale_html.mjs): il fattore intero che dà 300 punti
# per pollice a quella misura (3 per una schermata 640 x 480, 2 per le figure larghe)
PUNTI_PER_POLLICE, MM_PER_PIXEL, LARGHEZZA_MM = 300, 0.25, 165
MESI = "gennaio febbraio marzo aprile maggio giugno luglio agosto settembre ottobre novembre dicembre".split()


def volumi_dal_readme():
    """Titolo, per chi e contenuto dalla tabella del README del manuale: | **1. Manuale d'uso** | `1_uso/` | … |"""
    v = {}
    for riga in (MANUALE / "README.md").read_text(encoding="utf-8").splitlines():
        m = re.match(r"\|\s*\*\*(\d)\.\s*(.+?)\*\*\s*\|\s*`(\w+)/`\s*\|\s*(.+?)\s*\|\s*(.+?)\s*\|$", riga)
        if m:
            n = int(m.group(1))
            contenuto = re.sub(r"\s*\(indice in `[^`]+`\)", "", m.group(5))
            v[n] = dict(n=n, titolo=m.group(2), cartella=m.group(3), per_chi=m.group(4), contenuto=contenuto)
    if sorted(v) != [1, 2, 3, 4]:
        sys.exit(f"README del manuale: volumi trovati {sorted(v)}, ne servono quattro")
    return v


def prepara_figure():
    """Le figure in tools/pdf/build/figure e la loro scheda per il convertitore."""
    scheda = {}
    for sorg in sorted((MANUALE / "figure").rglob("*")):
        if not sorg.is_file():
            continue
        rel = sorg.relative_to(MANUALE / "figure").as_posix()
        dest = BUILD / "figure" / rel
        dest.parent.mkdir(parents=True, exist_ok=True)
        aggiorna = not dest.exists() or dest.stat().st_mtime < sorg.stat().st_mtime
        if sorg.suffix == ".svg":
            testo = sorg.read_text(encoding="utf-8")
            m = re.search(r"viewBox=['\"][\d.]+ [\d.]+ ([\d.]+) ([\d.]+)['\"]", testo)
            ow, oh = (float(m.group(1)), float(m.group(2))) if m else (760.0, 440.0)
            if aggiorna:
                shutil.copyfile(sorg, dest)
            scheda[rel] = dict(tipo="svg", ow=ow, oh=oh, w=ow, h=oh)
            continue
        with Image.open(sorg) as im:
            ow, oh = im.size
            if im.format == "PNG":   # schermate e tavole a pixel: fattore intero, vicino più prossimo
                mm = min(ow * MM_PER_PIXEL, LARGHEZZA_MM)
                k = max(1, math.ceil(mm / 25.4 * PUNTI_PER_POLLICE / ow - 1e-9))
                if not aggiorna:
                    with Image.open(dest) as fatta:
                        aggiorna = fatta.size != (ow * k, oh * k)
                if aggiorna:
                    im.resize((ow * k, oh * k), Image.NEAREST).save(dest, optimize=True)
                scheda[rel] = dict(tipo="pixel", ow=ow, oh=oh, w=ow * k, h=oh * k)
            else:
                if aggiorna:
                    shutil.copyfile(sorg, dest)
                scheda[rel] = dict(tipo="foto", ow=ow, oh=oh, w=ow, h=oh)
    return scheda


def stampa(html, pdf, profilo):
    """Una stampa di Chromium, senza finestra; i segnalibri dai titoli."""
    cmd = ["nice", "-n", "19", "chromium", "--headless=new", "--no-sandbox", "--disable-gpu", "--no-first-run",
           f"--user-data-dir={profilo}", "--no-pdf-header-footer", "--generate-pdf-document-outline",
           "--virtual-time-budget=60000",   # i caratteri caricati e le sillabe fatte prima di stampare
           f"--print-to-pdf={pdf}", html.as_uri()]
    r = subprocess.run(cmd, capture_output=True, text=True, timeout=3600)
    if r.returncode != 0 or not pdf.exists():
        sys.exit(f"Chromium non ha stampato {html.name}:\n{r.stderr[-2000:]}")


def destinazioni(pdf):
    """{id: pagina} dalle destinazioni del PDF."""
    r = subprocess.run(["pdfinfo", "-dests", str(pdf)], capture_output=True, text=True, check=True)
    d = {}
    for riga in r.stdout.splitlines():
        m = re.match(r'\s*(\d+)\s+\[.*\]\s+"(.*)"$', riga)
        if m:
            d[m.group(2)] = int(m.group(1))
    return d


def pagine(pdf):
    r = subprocess.run(["pdfinfo", str(pdf)], capture_output=True, text=True, check=True)
    return int(re.search(r"^Pages:\s+(\d+)", r.stdout, re.M).group(1))


def link_fra_volumi(pdf, nomi):
    """/S /URI /URI (file:///…/altro.pdf#id) → /S /GoToR /F (altro.pdf) /D (id), stessi byte."""
    dati = pdf.read_bytes()
    nomi_re = b"|".join(re.escape(n.encode()) for n in nomi)
    schema = re.compile(rb"/S /URI\n/URI \(file://[^()]*?/(" + nomi_re + rb")#([^()]*)\)")
    fatti = 0

    def sostituisci(m):
        nonlocal fatti
        nuovo = b"/S /GoToR\n/F (" + m.group(1) + b") /D (" + m.group(2) + b")"
        if len(nuovo) > len(m.group(0)):
            sys.exit(f"{pdf.name}: percorso troppo corto per riscrivere un link fra volumi")
        fatti += 1
        return nuovo + b" " * (len(m.group(0)) - len(nuovo))

    dati = schema.sub(sostituisci, dati)
    pdf.write_bytes(dati)
    return fatti


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--volume", type=int, action="append", choices=sorted(VOLUMI), help="solo questi volumi (anche più volte)")
    ap.add_argument("--uscita", type=pathlib.Path, default=USCITA)
    a = ap.parse_args()
    scelti = sorted(a.volume or VOLUMI)
    for cosa in (NODE, PDF / "node_modules" / "markdown-it", SILLABE, CHARIS / "Charis-Regular.ttf"):
        if not cosa.exists():
            sys.exit(f"manca {cosa}")
    if not shutil.which("chromium") or not shutil.which("pdfinfo"):
        sys.exit("servono chromium e pdfinfo (poppler-utils)")

    BUILD.mkdir(parents=True, exist_ok=True)
    print("figure…", flush=True)
    scheda = prepara_figure()
    oggi = datetime.date.today()
    readme = volumi_dal_readme()
    volumi = []
    for n, (nome, copertina) in VOLUMI.items():
        volumi.append(dict(readme[n], pdf=nome, copertina=f"figure/{copertina}"))
    conf = dict(radice=str(MANUALE), uscita=str(BUILD), data=f"{oggi.day} {MESI[oggi.month - 1]} {oggi.year}",
                sillabe=str(SILLABE), figure=scheda, volumi=volumi)
    (BUILD / "configurazione.json").write_text(json.dumps(conf, ensure_ascii=False), encoding="utf-8")
    for f in ("manuale.css", "sillabe_finali.js"):
        shutil.copyfile(PDF / f, BUILD / f)
    (BUILD / "caratteri").mkdir(exist_ok=True)
    for stile in ("Regular", "Italic", "Bold", "BoldItalic"):   # Charis (SIL, OFL 1.1): il testo dei volumi
        shutil.copyfile(CHARIS / f"Charis-{stile}.ttf", BUILD / "caratteri" / f"Charis-{stile}.ttf")
    print("HTML…", flush=True)
    subprocess.run([str(NODE), str(PDF / "manuale_html.mjs"), str(BUILD / "configurazione.json")], check=True)

    a.uscita.mkdir(parents=True, exist_ok=True)
    nomi = [nome for nome, _ in VOLUMI.values()]
    profilo = BUILD / "profilo_chromium"
    for n in scelti:
        html = BUILD / f"volume_{n}.html"
        testo = html.read_text(encoding="utf-8")
        prova = BUILD / f"volume_{n}_prova.pdf"
        print(f"volume {n}: prima stampa…", flush=True)
        stampa(html, prova, profilo)
        dest = destinazioni(prova)
        mancanti = []

        def numero(m):
            p = dest.get(m.group(1))
            if p is None:
                mancanti.append(m.group(1))
                return m.group(0)
            return f'<span class="pn" data-d="{m.group(1)}">{p}</span>'

        testo = re.sub(r'<span class="pn" data-d="([^"]+)">000</span>', numero, testo)
        if mancanti:
            sys.exit(f"volume {n}: titoli senza pagina: {mancanti[:10]}")
        finale = BUILD / f"volume_{n}_finale.html"
        finale.write_text(testo, encoding="utf-8")
        pdf = BUILD / nomi[n - 1]
        print(f"volume {n}: seconda stampa…", flush=True)
        stampa(finale, pdf, profilo)
        if pagine(pdf) != pagine(prova) or destinazioni(pdf) != dest:
            sys.exit(f"volume {n}: la seconda stampa ha spostato le pagine")
        fra = link_fra_volumi(pdf, nomi)
        shutil.copyfile(pdf, a.uscita / pdf.name)
        prova.unlink()
        peso = (a.uscita / pdf.name).stat().st_size
        print(f"volume {n}: {a.uscita / pdf.name}, {pagine(pdf)} pagine, {peso / 1e6:.1f} MB, {fra} link verso altri volumi",
              flush=True)


if __name__ == "__main__":
    main()
