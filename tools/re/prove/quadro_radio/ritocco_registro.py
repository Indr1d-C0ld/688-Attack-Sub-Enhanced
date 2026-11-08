p='missioni/src/registro_missione.cpp'
s=open(p,encoding='utf-8').read()
a='''            e.testo_it = c + (r == "pedina" ? " pedina " :'''
assert s.count(a)==1
n='''            // il quadro radio ai sottomarini dell'IA, per lo spettatore
            if (r == "ascolto_broadcast" || r == "dato_radio_usato" || r == "dato_radio_lasciato") {
                e.testo_it = r == "ascolto_broadcast" ? c + " sale ad ascoltare il broadcast" : r == "dato_radio_usato" ? c + " va sul dato radio di " + b : c + " lascia il dato radio di " + b;
                e.testo_en = r == "ascolto_broadcast" ? ce + " comes up to copy the broadcast" : r == "dato_radio_usato" ? ce + " heads for the radio report of " + be : ce + " drops the radio report of " + be;
                q.soggetto = e.unita; q.secondo = e.altra; q.distanza_m = 800.0; q.elevazione_deg = 4.0; icona("radio");
                break;
            }
'''
s=s.replace(a,n+a)
open(p,'w',encoding='utf-8').write(s)
