// Il manuale Enhanced in HTML da stampare, un file per volume (lo chiama tools/pipeline/manuale_pdf.py, che fa i PDF).
// Legge il Markdown di docs/manuale_enhanced e non lo cambia: aggiunge le ancore dei titoli, i rimandi cliccabili scritti
// nel testo («capitolo F1 §2.5», «volume 1, cap. 10»), la sillabazione italiana coi trattini morbidi, le figure a misura e
// l'indice con i numeri di pagina da riempire (segnaposto «000», poi li mette lo script Python dalla prima stampa).
//
// Uso: node manuale_html.mjs <configurazione.json>
//   { "radice": cartella del manuale, "uscita": cartella degli HTML, "data": "7 novembre 2026",
//     "sillabe": file dei pattern (hyph_it_IT.dic), "figure": { "unita/x.png": {w, h, ow, oh, tipo} },
//     "volumi": [ { "n": 1, "cartella": "1_uso", "titolo": "Manuale d'uso", "per_chi": "...", "contenuto": "...",
//                   "pdf": "688_manuale_1_uso.pdf", "copertina": "figure/x.png" } ] }
// Scrive <uscita>/volume_<n>.html e <uscita>/rimandi.json (rimandi trovati e non risolti, per il controllo).

import fs from "node:fs";
import path from "node:path";
import MarkdownIt from "markdown-it";

const conf = JSON.parse(fs.readFileSync(process.argv[2], "utf8"));

// ---------------------------------------------------------------- sillabazione (Liang, pattern di LibreOffice/liblouis)
const pattern = new Map();
let pattern_max = 0;
for (const riga of fs.readFileSync(conf.sillabe, "latin1").split(/\r?\n/).slice(1)) {
    const p = riga.trim();
    if (!p || /^[A-Z]/.test(p) || p.startsWith("%")) continue;
    const lettere = p.replace(/\d/g, "");
    const punti = new Array(lettere.length + 1).fill(0);
    let k = 0;
    for (const c of p) {
        if (/\d/.test(c)) punti[k] = Number(c);
        else ++k;
    }
    pattern.set(lettere, punti);
    pattern_max = Math.max(pattern_max, lettere.length);
}
const SHY = "­", SINISTRA = 2, DESTRA = 3;   // almeno due lettere prima del trattino e tre dopo (STIMA: scelta tipografica)
const sillabate = new Map();
function sillaba(parola) {
    if (parola.length < 6 || /[A-Z].*[A-Z]/.test(parola)) return parola;   // sigle, nomi in maiuscolo misto: interi
    const fatta = sillabate.get(parola);
    if (fatta !== undefined) return fatta;
    const w = "." + parola.toLowerCase() + ".";
    const punti = new Array(w.length + 1).fill(0);
    for (let i = 0; i < w.length; ++i)
        for (let j = i + 1; j <= Math.min(w.length, i + pattern_max); ++j) {
            const p = pattern.get(w.slice(i, j));
            if (p) for (let k = 0; k < p.length; ++k) punti[i + k] = Math.max(punti[i + k], p[k]);
        }
    let r = "";
    for (let i = 0; i < parola.length; ++i) {
        if (i >= SINISTRA && parola.length - i >= DESTRA && punti[i + 1] % 2 === 1) r += SHY;
        r += parola[i];
    }
    sillabate.set(parola, r);
    return r;
}
const sillaba_testo = (s) => s.replace(/\p{L}+/gu, sillaba);

const esc = (s) => s.replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;").replace(/"/g, "&quot;");
const testo_semplice = (figli) => figli.map((t) => (t.children ? testo_semplice(t.children) : t.content || "")).join("");

// ---------------------------------------------------------------- i capitoli dei volumi, i titoli e le ancore
const md = new MarkdownIt({ html: true, typographer: false, linkify: false });

function chiave_file(vol, nome) {   // l'ordine e la chiave del capitolo dal nome del file
    const b = nome.replace(/\.md$/, "");
    let m;
    if (b === "00_indice") return { k: "presentazione", ord: [-1] };
    if (b === "SCHEMA_AUTORI") return { k: "schema", ord: [99] };
    if (b === "appendici") return { k: "app", ord: [98] };
    if ((m = /^([A-Z])(\d+)_/.exec(b))) return { k: m[1] + m[2], ord: [m[1].charCodeAt(0), Number(m[2])], lettera: m[1] };
    if ((m = /^(\d+)_/.exec(b))) return { k: "c" + Number(m[1]), ord: [Number(m[1])], numero: Number(m[1]) };
    throw new Error(`capitolo senza ordine: ${vol.cartella}/${nome}`);
}
const confronta = (a, b) => {
    for (let i = 0; i < Math.max(a.length, b.length); ++i) if ((a[i] ?? -1) !== (b[i] ?? -1)) return (a[i] ?? -1) - (b[i] ?? -1);
    return 0;
};
// «2.1 Il registro», «3. Sonar», «A.17 …», «A. Tutti i tasti» (una lettera sola solo col punto: «A che serve» non è un numero)
const numero_titolo = (s) => (/^((?:[A-Z]\.)?\d+(?:\.\d+)*)\.?\s+\S/.exec(s) || /^([A-Z])\.\s+\S/.exec(s) || [])[1];

// ---------------------------------------------------------------- i testi di lavoro, fuori dai PDF di rilascio
// Il Markdown resta com'è; qui, in memoria, si tolgono le note per chi scrive il manuale: chi ha scritto e rivisto ogni
// capitolo, gli stati di lavorazione, gli indici «approvati», la colonna degli autori, le appendici solo pianificate e i
// nomi dei file. Quello che si toglie finisce in tolti.json, per il controllo.
const tolti = [];
const SEZIONI_DI_LAVORO = /^## (Stato|Stato dei capitoli|Appendici generate dai dati|Appendici)\s*$/;
function frasi_senza_lavoro(testo) {   // le frasi «Autore: …», «Revisione del …», «Stato: …» di un paragrafo
    const frasi = testo.split(/(?<=\.)\s+(?=[A-ZÀ-Ý«(`*])/);
    const resto = frasi.filter((f) => !/^(Autor[ei]|Revisione|Stato|Curatore|Bozza del)\b/.test(f));
    return { resto: resto.join(" "), via: frasi.filter((f) => !resto.includes(f)) };
}
function per_il_rilascio(nome, ch, s) {
    const via = (cosa) => tolti.push(`${nome}: ${cosa.replace(/\s+/g, " ").slice(0, 160)}`);
    const righe = s.split("\n");
    // la riga di testa del capitolo: il primo paragrafo dopo il titolo, se comincia con «*Autore» o «*Autori»
    const h1 = righe.findIndex((r) => /^# /.test(r));
    let i = h1 + 1;
    while (i < righe.length && !righe[i].trim()) ++i;
    if (h1 >= 0 && /^\*Autor[ei]:/.test(righe[i] || "")) {
        let j = i;
        while (j < righe.length && righe[j].trim()) ++j;
        const par = righe.slice(i, j).join(" ").trim().replace(/^\*/, "").replace(/\*$/, "");
        const { resto, via: frasi } = frasi_senza_lavoro(par);
        frasi.forEach(via);
        righe.splice(i, j - i, ...(resto ? [`*${resto}*`] : []));
    }
    if (ch.k !== "presentazione") return righe.join("\n");
    // le presentazioni dei volumi
    const fuori = [];
    let sezione = false, autore = -1;
    for (const r of righe) {
        if (/^## /.test(r)) {
            sezione = SEZIONI_DI_LAVORO.test(r);
            if (sezione) via(`sezione «${r.slice(3)}»`);
        }
        if (sezione) continue;
        let x = r;
        // «*Indice approvato (…). Il manuale …*»: se il corsivo continua, il suo asterisco d'apertura resta
        const app = /(\*?)Indice approvato \([^)]*\)\.(\*?)\s*/.exec(x);
        if (app) {
            via(app[0].trim());
            x = x.replace(app[0], app[1] && !app[2] ? "*" : "");
        }
        for (const re of [/\*Indice del [^*]*Curatore: kit\.\*\s*/,
                          /Le tabelle copiate dal volume 2 si rigenerano [^.]*\.\s*/, /\s*Da ricercare con le fonti dell'epoca\./]) {
            const m = re.exec(x);
            if (m) {
                via(m[0].trim());
                x = x.replace(re, "");
            }
        }
        // «…della simulazione, sullo / schema di `SCHEMA_AUTORI.md`: a che serve…»: lo schema è fuori dal PDF
        if (/, sullo\s*$/.test(x)) x = x.replace(/, sullo\s*$/, ":");
        if (/^schema di `SCHEMA_AUTORI\.md`: /.test(x)) {
            via("rimando a `SCHEMA_AUTORI.md`");
            x = x.replace(/^schema di `SCHEMA_AUTORI\.md`: /, "");
        }
        if (/^La colonna «autore» dice chi scrive il capitolo/.test(x)) {
            via(x);
            continue;
        }
        // la colonna «Autore» delle tabelle dei capitoli
        if (/^\|/.test(x)) {
            const celle = x.split("|");
            if (celle.some((c) => c.trim() === "Autore")) {
                autore = celle.findIndex((c) => c.trim() === "Autore");
                via("colonna «Autore» di una tabella");
            }
            if (autore > 0) {
                celle.splice(autore, 1);
                x = celle.join("|");
            }
        } else autore = -1;
        const file = x.match(/ \(`[\w.]+\.md`\)/g);
        if (file) {
            file.forEach((f) => via(`nome del file${f}`));
            x = x.replace(/ \(`[\w.]+\.md`\)/g, "");
        }
        fuori.push(x);
    }
    return fuori.join("\n");
}

const volumi = conf.volumi.map((v) => ({ ...v, capitoli: [], sezioni_tutte: new Map(), per_numero: new Map() }));
const per_lettera = new Map();   // «F1» → [volume, capitolo]: le sigle sono uniche in tutto il manuale
for (const vol of volumi) {
    const cartella = path.join(conf.radice, vol.cartella);
    for (const nome of fs.readdirSync(cartella).filter((f) => f.endsWith(".md"))) {
        const ch = chiave_file(vol, nome);
        if (ch.k === "schema") {   // lo schema per gli autori del volume 2 è roba di lavoro
            tolti.push(`${vol.cartella}/${nome}: tutto il file`);
            continue;
        }
        const sorgente = per_il_rilascio(`${vol.cartella}/${nome}`, ch, fs.readFileSync(path.join(cartella, nome), "utf8"));
        const env = {};
        const token = md.parse(sorgente, env);
        const cap = { ...ch, nome, token, id: `v${vol.n}-${ch.k}`, sezioni: new Map(), titoli: [], titolo: "" };
        let n_senza = 0;
        for (let i = 0; i < token.length; ++i) {
            const t = token[i];
            if (t.type !== "heading_open") continue;
            const livello = Number(t.tag.slice(1));
            const testo = testo_semplice(token[i + 1].children).trim();
            let id;
            if (livello === 1 && !cap.titolo) {
                cap.titolo = testo;
                id = cap.id;
            } else {
                const num = numero_titolo(testo);
                if (num && !cap.sezioni.has(num)) {
                    id = `${cap.id}-s${num.replace(/\./g, "-")}`;
                    cap.sezioni.set(num, id);
                    if (!vol.sezioni_tutte.has(num)) vol.sezioni_tutte.set(num, []);
                    vol.sezioni_tutte.get(num).push(id);
                } else id = `${cap.id}-t${++n_senza}`;
            }
            t.attrSet("id", id);
            cap.titoli.push({ livello, testo, id, figli: token[i + 1].children });
        }
        vol.capitoli.push(cap);
        if (ch.lettera || /^T\d+$/.test(ch.k)) per_lettera.set(ch.k, [vol, cap]);
        if (ch.numero) vol.per_numero.set(ch.numero, cap);
    }
    vol.capitoli.sort((a, b) => confronta(a.ord, b.ord));
    for (const cap of vol.capitoli) if (!cap.titolo) throw new Error(`capitolo senza titolo: ${vol.cartella}/${cap.nome}`);
}
const vol_n = (n) => volumi.find((v) => v.n === n);
const per_file = new Map();   // «T1_principi.md» → [volume, capitolo], per i link fra i file
for (const vol of volumi) for (const cap of vol.capitoli) per_file.set(`${vol.cartella}/${cap.nome}`, [vol, cap]);

// ---------------------------------------------------------------- i rimandi scritti nel testo
const CH = String.raw`(?:[A-F](?:1[0-3]|[1-9])|T(?:1[0-2]|[1-9]))`;
const SEC = String.raw`§\s?(?:[A-Z]\.)?\d+(?:\.\d+)*`;
const RIMANDO = new RegExp(
    [
        String.raw`(?<capw>\b[Cc]apitol[oi]|\bcap\.)\s+(?<capc>${CH})\b(?:\s*,?\s*(?<caps>${SEC}))?`,
        String.raw`(?<capw2>\b[Cc]apitol[oi]|\bcap\.)\s+(?<capn>\d{1,2}(?:\.\d+)*)(?![\d])(?:\s*,?\s*(?<capns>${SEC}))?(?<delvol>\s+del\s+volume\s+(?<dv>[1-4]))?`,
        String.raw`\b(?<chc>${CH})\s*,?\s*(?<chs>${SEC})`,
        String.raw`(?<sec>${SEC})`,
        String.raw`\b[Vv]olume\s+(?<vol>[1-4])\b`,
        String.raw`\b[Aa]ppendice\s+(?<app>[A-Z])\b`,
        String.raw`(?<![\w.])(?<ax>A\.\d{1,2})(?![\w])`,
        String.raw`\b(?<bare>${CH})\b`,
    ].join("|"),
    "gu",
);
const rapporto = { risolti: 0, fuori_volume: 0, esterni: 0, non_risolti: [], cliccabili: [] };
const richiesti = new Map();   // volume → ancore chieste da altri volumi (servono le destinazioni anche lì)

function sezione(vol, cap, num) {
    num = num.replace(/^§\s?/, "");
    if (cap && cap.sezioni.has(num)) return cap.sezioni.get(num);
    // nei volumi 2 e 4 la numerazione ricomincia a ogni capitolo: un «§» da solo vale solo per il suo capitolo
    if (cap && !vol.per_numero.size) return null;
    const tutte = vol.sezioni_tutte.get(num);
    return tutte && tutte.length === 1 ? tutte[0] : null;
}
function href(env, vol, id) {
    if (!id) return null;
    if (vol.n === env.vol.n) return "#" + id;
    if (!richiesti.has(vol.n)) richiesti.set(vol.n, new Set());
    richiesti.get(vol.n).add(id);
    ++rapporto.fuori_volume;
    return `${vol.pdf}#${id}`;
}
function risolvi(m, s, env, precedente, catena) {
    const g = m.groups;
    const ora = env.vol, cap_ora = env.cap;
    env.contesto = null;
    if (g.capc || g.chc) {
        const voce = per_lettera.get(g.capc || g.chc);
        if (!voce) return null;
        const [v, c] = voce, num = g.caps || g.chs;
        env.contesto = { vol: v, cap: c };
        return href(env, v, num ? sezione(v, c, num) || c.id : c.id);
    }
    if (g.capn) {
        // il volume: «del volume 1» dopo, «volume 1, cap. 10» poco prima; se no il volume stesso, se ha capitoli numerati
        let n = g.dv ? Number(g.dv) : null;
        const prima = /[Vv]olume\s+([1-4])\b[^.;)]{0,12}$/.exec(s.slice(Math.max(0, m.index - 30), m.index));
        if (!n && prima) n = Number(prima[1]);
        if (!n && ora.per_numero.size) n = ora.n;
        if (!n) return null;
        const v = vol_n(n);
        env.contesto = { vol: v, cap: null };
        if (g.capn.includes(".")) return href(env, v, sezione(v, null, g.capn));
        const c = v.per_numero.get(Number(g.capn));
        if (!c) return null;
        env.contesto.cap = c;
        return href(env, v, g.capns ? sezione(v, c, g.capns) || c.id : c.id);
    }
    if (g.sec) {
        const prima = s.slice(0, m.index);
        const v = /[Vv]olume\s+([1-4])\s*[,(]\s*$/.exec(prima);   // «volume 1, §2.6», «il volume 1 (§3.8»
        if (v) {
            env.contesto = { vol: vol_n(Number(v[1])), cap: null };
            return href(env, env.contesto.vol, sezione(env.contesto.vol, null, g.sec));
        }
        const c = new RegExp(String.raw`\b(${CH})\s*\(\s*$`).exec(prima);   // «C9 (§2.4»
        if (c && per_lettera.has(c[1])) {
            const [vv, cc] = per_lettera.get(c[1]);
            env.contesto = { vol: vv, cap: cc };
            return href(env, vv, sezione(vv, cc, g.sec));
        }
        if (catena) {   // «capitolo C12 §2.3 e §2.11», «volume 1, §4.2 e §4.5»: lo stesso capitolo o volume
            env.contesto = catena;
            return href(env, catena.vol, sezione(catena.vol, catena.cap, g.sec));
        }
        return href(env, ora, sezione(ora, cap_ora, g.sec));
    }
    if (g.vol) return Number(g.vol) === ora.n ? null : href(env, vol_n(Number(g.vol)), `v${g.vol}`);
    if (g.app || g.ax) {
        const v = vol_n(1), c = v.capitoli.find((x) => x.k === "app");
        return c ? href(env, v, sezione(v, c, g.app || g.ax) || c.id) : null;
    }
    if (g.bare) {   // una sigla da sola solo dentro un elenco di capitoli: «capitoli C1, C2 e C4»
        if (!precedente || !/^(\s*,\s*|\s+e\s+|\s+ed\s+|\s*\/\s*)$/.test(s.slice(precedente, m.index))) return undefined;
        const voce = per_lettera.get(g.bare);
        return voce ? href(env, voce[0], voce[1].id) : null;
    }
    return null;
}
// un «§» che non è del manuale: dei dossier di ricerca, di un file, di una legge, o dopo un numero («13 §3.1» in una colonna
// delle fonti); e quelli che lo seguono in catena («dossier 01 §2 e §8.1»)
const ESTERNO_PRIMA = [
    /(?:[Dd]ossier|appunti|progetto|vol\.|U\.S\.C\.)\s*[\d.]*\s*,?\s*$/,
    /\.(?:md|toml|cpp|hpp|lua|py|txt|json)`?:?\s*$/,
    /docs\/\w+`?\s*$/,
    /(?:^|[^\w.§])\d+\s*$/,
    /\b[a-z]\d+\s*$/,   // le fonti di ricerca: «r7 §1.2»
];
// la frase del «§» esterno finisce a un punto e virgola, a un punto seguito da spazio o a una parentesi chiusa aperta prima
// di lui; una parentesi aperta e chiusa nel mezzo («dossier 11 §1.1 (nota), §3.2») non la chiude
function fine_frase(g) {
    if (/;|\.\s/.test(g)) return true;
    let aperte = 0;
    for (const c of g) {
        if (c === "(") ++aperte;
        else if (c === ")" && --aperte < 0) return true;
    }
    return false;
}
const CATENA = /^(?:[-–]\d+(?:\.\d+)*)?(?:\s*[,;]\s*|\s+e\s+|\s+ed\s+)$/;
function testo_con_rimandi(s, env, dopo_codice) {
    if (env.titolo) return esc(s);
    if (env.link) return esc(sillaba_testo(s));
    let r = "", da = 0, precedente = null, esterno = null, contesto = null;
    for (const m of s.matchAll(RIMANDO)) {
        if (m.groups.sec) {
            const prima = s.slice(0, m.index);
            // dopo un «§» esterno, i «§» della stessa frase sono suoi («26 §2.1 Overseas Harriette, §3 portarinfuse»)
            const stessa_frase = esterno !== null && m.index - esterno < 80 && !fine_frase(s.slice(esterno, m.index));
            const fuori = stessa_frase || (esterno !== null && CATENA.test(s.slice(esterno, m.index))) ||
                ESTERNO_PRIMA.some((e) => e.test(prima)) || (dopo_codice && !prima.trim());
            if (fuori) {
                esterno = m.index + m[0].length;
                ++rapporto.esterni;
                continue;
            }
        }
        esterno = null;
        const catena = contesto && CATENA.test(s.slice(contesto.fine, m.index)) ? contesto : null;
        const h = risolvi(m, s, env, precedente, catena);
        if (h === undefined) continue;   // non è un rimando: resta testo
        r += esc(sillaba_testo(s.slice(da, m.index)));
        const pezzo = esc(sillaba_testo(m[0]));
        contesto = env.contesto ? { ...env.contesto, fine: m.index + m[0].length } : null;
        if (h) {
            r += `<a class="rif" href="${esc(h)}">${pezzo}</a>`;
            ++rapporto.risolti;
            rapporto.cliccabili.push(`${env.vol.cartella}/${env.cap ? env.cap.nome : "?"}: ${m[0]} → ${h} | ${s.slice(Math.max(0, m.index - 40), m.index)}`);
            precedente = m.index + m[0].length;
        } else {
            r += pezzo;
            rapporto.non_risolti.push(`${env.vol.cartella}/${env.cap ? env.cap.nome : "?"}: ${m[0]} | ${s.slice(Math.max(0, m.index - 40), m.index)}`);
            precedente = null;
        }
        da = m.index + m[0].length;
    }
    return r + esc(sillaba_testo(s.slice(da)));
}

// ---------------------------------------------------------------- regole di resa
md.renderer.rules.text = (t, i, o, env) =>
    testo_con_rimandi(t[i].content, env, i > 0 && t[i - 1].type === "code_inline" && /[./_]/.test(t[i - 1].content));
md.renderer.rules.code_inline = (t, i) => {
    const c = esc(t[i].content);
    return `<code>${t[i].content.length > 14 ? c.replace(/([_/.])/g, "$1<wbr>") : c}</code>`;
};
md.renderer.rules.fence = (t, i) => {
    const righe = t[i].content.replace(/\n$/, "").split("\n");
    const lunga = Math.max(20, ...righe.map((r) => [...r].length));
    const pt = Math.max(6.2, Math.min(8.2, 470 / (lunga * 0.6)));   // 470 pt di colonna, 0,6 em per carattere
    return `<pre class="codice" style="font-size:${pt.toFixed(2)}pt"><code>${esc(righe.join("\n"))}</code></pre>\n`;
};
const link_open = md.renderer.rules.link_open || ((t, i, o, e, self) => self.renderToken(t, i, o));
md.renderer.rules.link_open = (t, i, o, env, self) => {
    ++env.link;
    const h = t[i].attrGet("href") || "";
    if (/^https?:/.test(h)) t[i].attrSet("class", "web");
    else if (/\.md(#.*)?$/.test(h)) {   // un altro file del manuale: il suo capitolo
        const dest = path.posix.normalize(path.posix.join(env.vol.cartella, h.replace(/#.*$/, "")));
        const voce = per_file.get(dest);
        if (voce) t[i].attrSet("href", href(env, voce[0], voce[1].id));
        else rapporto.non_risolti.push(`${env.vol.cartella}/${env.cap.nome}: link ${h}`);
        t[i].attrSet("class", "rif");
    }
    return link_open(t, i, o, env, self);
};
md.renderer.rules.link_close = (t, i, o, env, self) => {
    --env.link;
    return self.renderToken(t, i, o);
};
md.renderer.rules.table_open = (t, i) => {
    const cl = t[i].attrGet("class");
    return `<table class="${cl}">\n`;
};
md.renderer.rules.html_block = (t, i) => (/^<!--/.test(t[i].content.trim()) ? "" : esc(t[i].content));
md.renderer.rules.html_inline = (t, i) => (/^<!--/.test(t[i].content) ? "" : esc(t[i].content));
md.renderer.rules.heading_open = (t, i, o, env, self) => {
    env.titolo = true;
    return self.renderToken(t, i, o);
};
md.renderer.rules.heading_close = (t, i, o, env, self) => {
    env.titolo = false;
    return self.renderToken(t, i, o);
};

// le tabelle: quante colonne (la misura del carattere) e se sono corte (non si spezzano)
function segna_tabelle(token) {
    for (let i = 0; i < token.length; ++i) {
        if (token[i].type !== "table_open") continue;
        let colonne = 0, righe = 0;
        for (let j = i + 1; j < token.length && token[j].type !== "table_close"; ++j) {
            if (token[j].type === "th_open") ++colonne;
            if (token[j].type === "tr_open") ++righe;
        }
        token[i].attrSet("class", `tab c${Math.min(colonne, 11)}${righe <= 14 ? " corta" : ""}`);
    }
}

// ---------------------------------------------------------------- le figure
const MM_PER_PIXEL = 0.25, LARGHEZZA_MM = 165, SPAZIO_MM = 4;
function misura(src) {   // la misura in millimetri: le schermate a 0,25 mm per pixel del gioco, le foto a 110 mm
    const f = conf.figure[src];
    if (!f) throw new Error(`figura mancante: ${src}`);
    let w, h;
    if (f.tipo === "foto") [w, h] = [110, (110 * f.oh) / f.ow];
    else if (f.tipo === "svg") [w, h] = [150, (150 * f.oh) / f.ow];
    else [w, h] = [f.ow * MM_PER_PIXEL, f.oh * MM_PER_PIXEL];
    const k = Math.min(1, LARGHEZZA_MM / w, (f.tipo === "foto" ? 120 : 200) / h);
    return { w: w * k, h: h * k, f };
}
const solo_immagini = (t) =>
    t && t.type === "inline" && t.children.some((c) => c.type === "image") &&
    t.children.every((c) => c.type === "image" || c.type === "softbreak" || (c.type === "text" && !c.content.trim()));
const didascalia = (t) =>
    t && t.type === "inline" && t.children.length > 2 && t.children[0].type === "em_open" && t.children.at(-1).type === "em_close";
function sorgente_figura(env, src) {
    const p = path.posix.normalize(path.posix.join(env.vol.cartella, src));
    if (!p.startsWith("figure/")) throw new Error(`figura fuori da figure/: ${src}`);
    return p.slice("figure/".length);
}
function img_html(im, env, k = 1) {
    const src = sorgente_figura(env, im.attrGet("src"));
    const m = misura(src);
    const alt = esc(testo_semplice(im.children || []) || im.content || "");
    const cl = m.f.tipo === "pixel" ? ' class="pixel"' : "";
    return `<img${cl} src="figure/${esc(src)}" alt="${alt}" style="width:${(m.w * k).toFixed(1)}mm;height:${(m.h * k).toFixed(1)}mm">`;
}
function figura(immagini, cap, env) {
    const misure = immagini.map((im) => misura(sorgente_figura(env, im.attrGet("src"))));
    const totale = misure.reduce((a, m) => a + m.w, 0) + SPAZIO_MM * (misure.length - 1);
    const k = Math.min(1, LARGHEZZA_MM / totale);
    const didasc = cap ? `<figcaption>${cap}</figcaption>` : "";
    return `<figure>${immagini.map((im) => img_html(im, env, k)).join("")}${didasc}</figure>\n`;
}
md.renderer.rules.image = (t, i, o, env) => img_html(t[i], env);   // un'immagine dentro un elenco o nel testo

// ---------------------------------------------------------------- un capitolo, un volume
function capitolo_html(vol, cap) {
    const env = { vol, cap, link: 0, titolo: false };
    const t = cap.token;
    segna_tabelle(t);
    const out = [];
    let i = 0;
    const e_figura = (j) => t[j] && t[j].type === "paragraph_open" && t[j].level === 0 && solo_immagini(t[j + 1]);
    while (i < t.length) {
        if (e_figura(i)) {
            const immagini = [];
            let j = i;
            while (e_figura(j)) {
                immagini.push(...t[j + 1].children.filter((c) => c.type === "image"));
                j += 3;
            }
            let cap_html = null;
            if (t[j] && t[j].type === "paragraph_open" && didascalia(t[j + 1])) {
                cap_html = md.renderer.renderInline(t[j + 1].children.slice(1, -1), md.options, env);
                j += 3;
            }
            out.push(figura(immagini, cap_html, env));
            i = j;
            continue;
        }
        let j = i + 1;
        while (j < t.length && !e_figura(j)) ++j;
        out.push(md.renderer.render(t.slice(i, j), md.options, env));
        i = j;
    }
    return out.join("");
}

const css_stringa = (s) => '"' + s.replace(/\\/g, "\\\\").replace(/"/g, '\\"') + '"';
const pagina_nome = (cap) => "p-" + cap.k;

function parti_del_volume(vol) {   // le parti del volume 2: «## Parte A — Fondamenti» nella presentazione
    const pres = vol.capitoli.find((c) => c.k === "presentazione");
    const parti = new Map();
    for (const t of pres ? pres.titoli : []) {
        const m = /^Parte\s+([A-Z])\s+[—-]\s+(.+)$/.exec(t.testo);
        if (m) parti.set(m[1], m[2]);
    }
    return parti;
}

function indice_html(vol, parti) {
    const righe = [];
    let parte = null;
    for (const cap of vol.capitoli) {
        if (cap.lettera && parti.has(cap.lettera) && cap.lettera !== parte) {
            parte = cap.lettera;
            righe.push(`<li class="parte"><a href="#v${vol.n}-parte-${parte}"><span class="t">Parte ${parte} — ${esc(parti.get(parte))}</span><span class="riempi"></span><span class="pn" data-d="v${vol.n}-parte-${parte}">000</span></a></li>`);
        }
        const env = { vol, cap, link: 1, titolo: true };
        const voce = (livello, testo_html, id) =>
            righe.push(`<li class="l${livello}"><a href="#${id}"><span class="t">${testo_html}</span><span class="riempi"></span><span class="pn" data-d="${id}">000</span></a></li>`);
        voce(1, md.renderer.renderInline(cap.titoli[0].figli, md.options, env), cap.id);
        const sotto = cap.titoli.some((x) => x.livello === 2) ? 2 : 3;
        for (const x of cap.titoli.slice(1)) if (x.livello === sotto) voce(2, md.renderer.renderInline(x.figli, md.options, env), x.id);
    }
    return `<nav class="indice" style="page: p-indice"><h1 id="v${vol.n}-indice">Indice</h1><ol>${righe.join("\n")}</ol></nav>\n`;
}

function copertina_html(vol) {
    const img = vol.copertina ? `<img class="pixel" src="${esc(vol.copertina)}" alt="">` : "";
    return `<section class="copertina" id="v${vol.n}">
<p class="testata">688 ATTACK SUB · EDIZIONE ENHANCED</p>
<p class="vol">Volume ${vol.n}</p>
<h1 class="titolo-vol">${esc(vol.titolo)}</h1>
<p class="per-chi">Per ${esc(vol.per_chi)}: ${esc(vol.contenuto)}</p>
<div class="quadro">${img}</div>
<p class="piede">Manuale dell'edizione Enhanced · ${esc(conf.data)}</p>
</section>
<section class="colophon">
<p><b>688 Attack Sub, edizione Enhanced.</b> Manuale, volume ${vol.n} di 4: ${esc(vol.titolo)}.</p>
<p>Gli altri volumi: ${volumi.filter((v) => v.n !== vol.n).map((v) => `<a class="rif" href="${esc(v.pdf)}#v${v.n}">${v.n}. ${esc(v.titolo)}</a>`).join(" · ")}.</p>
<p>Composto ${/^(1|8|11)\b/.test(conf.data) ? "l'" : "il "}${esc(conf.data)}. I rimandi fra capitoli e volumi sono cliccabili, se i quattro volumi stanno nella stessa
cartella. I crediti delle foto sono nelle didascalie e nel Museo del gioco; quelli completi, con le licenze di dati, suoni e
caratteri, nel file <code>LICENZE/CREDITI.txt</code> del pacchetto.</p>
</section>\n`;
}

const corpi = new Map();
for (const vol of volumi) {
    const parti = parti_del_volume(vol);
    const pezzi = [];
    let parte = null;
    const pagine = [];
    for (const cap of vol.capitoli) {
        if (cap.lettera && parti.has(cap.lettera) && cap.lettera !== parte) {
            parte = cap.lettera;
            pezzi.push(`<section class="pagina-parte" id="v${vol.n}-parte-${parte}"><p class="parte-n">Parte ${parte}</p><p class="parte-t">${esc(parti.get(parte))}</p></section>\n`);
        }
        const classe = cap.k === "presentazione" ? "cap presentazione" : "cap";
        pezzi.push(`<section class="${classe}" style="page: ${pagina_nome(cap)}">\n${capitolo_html(vol, cap)}</section>\n`);
        const titolo = cap.k === "presentazione" ? "Presentazione" : cap.titolo;
        pagine.push(`@page ${pagina_nome(cap)} { @top-left { content: ${css_stringa(`Volume ${vol.n} · ${vol.titolo}`)}; } ` +
            `@top-right { content: ${css_stringa(titolo)}; } }`);
    }
    corpi.set(vol.n, { pezzi, pagine, parti });
}

for (const vol of volumi) {
    const { pezzi, pagine, parti } = corpi.get(vol.n);
    // le destinazioni che gli altri volumi chiamano: Chromium le scrive solo se un link del documento le usa
    const ancore = [`v${vol.n}`, ...(richiesti.get(vol.n) || [])].map((id) => `<a href="#${esc(id)}">.</a>`).join("");
    const testata = `Volume ${vol.n} · ${vol.titolo}`;
    const html = `<!doctype html>
<html lang="it"><head><meta charset="utf-8">
<title>688 Attack Sub Enhanced — Volume ${vol.n}: ${esc(vol.titolo)}</title>
<link rel="stylesheet" href="manuale.css">
<style>
@page p-indice { @top-left { content: ${css_stringa(testata)}; } @top-right { content: "Indice"; } }
${pagine.join("\n")}
</style></head><body>
<div class="ancore" aria-hidden="true">${ancore}</div>${copertina_html(vol)}${indice_html(vol, parti)}${pezzi.join("")}
<script src="sillabe_finali.js"></script>
</body></html>
`;
    fs.writeFileSync(path.join(conf.uscita, `volume_${vol.n}.html`), html);
}
fs.writeFileSync(path.join(conf.uscita, "rimandi.json"), JSON.stringify(rapporto, null, 1));
fs.writeFileSync(path.join(conf.uscita, "tolti.json"), JSON.stringify(tolti, null, 1));
console.log(`rimandi: ${rapporto.risolti} cliccabili (${rapporto.fuori_volume} verso un altro volume), ${rapporto.esterni} § di dossier e file lasciati, ${rapporto.non_risolti.length} senza destinazione`);
