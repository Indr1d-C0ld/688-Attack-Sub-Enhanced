// Dopo l'impaginazione: i trattini morbidi che non vanno a capo si tolgono, quelli dove la parola si spezza diventano un
// trattino vero. Così nel PDF il testo si cerca e si copia intero («Portacontainer», non «Porta­container»), come in
// ogni libro sillabato. La pagina sta già larga quanto la colonna di stampa (manuale.css, @media screen), quindi le righe
// sono quelle della stampa; un trattino morbido spezzato ha la stessa larghezza del trattino che lo sostituisce
// (hyphenate-character). Si aspetta che Charis sia caricato: misurare col carattere di riserva darebbe altre righe.
document.fonts.ready.then(() => {
    const SHY = "­";
    const giro = document.createTreeWalker(document.body, NodeFilter.SHOW_TEXT,
        { acceptNode: (n) => (n.data.includes(SHY) ? NodeFilter.FILTER_ACCEPT : NodeFilter.FILTER_REJECT) });
    const nodi = [];
    while (giro.nextNode()) nodi.push(giro.currentNode);
    const r = document.createRange();
    const da_cambiare = nodi.map((n) => {
        const spezzati = new Set();
        for (let i = n.data.indexOf(SHY); i >= 0; i = n.data.indexOf(SHY, i + 1)) {
            r.setStart(n, i);
            r.setEnd(n, i + 1);
            const box = r.getBoundingClientRect();
            if (box.width > 0.5) spezzati.add(i);   // disegnato: è a fine riga
        }
        return spezzati;
    });
    // prima si misura tutto, poi si cambia: cambiare un nodo non deve spostare le misure degli altri
    nodi.forEach((n, k) => {
        let s = "";
        for (let i = 0; i < n.data.length; ++i) {
            const c = n.data[i];
            if (c !== SHY) s += c;
            else if (da_cambiare[k].has(i)) s += "-";
        }
        n.data = s;
    });
    document.documentElement.dataset.sillabe = "fatte";
});
