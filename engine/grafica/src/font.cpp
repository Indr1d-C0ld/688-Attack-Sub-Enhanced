#include "grafica/font.hpp"

#include <algorithm>
#include <stdexcept>

#include "grafica/tela.hpp"
#include "lettore.hpp"

namespace m688 {

Font Font::carica(const std::string& percorso) {
    const auto d = interno::leggi_file(percorso);
    interno::Lettore l(d, percorso);
    Font f;
    if (d.size() >= 4 && std::string(d.begin(), d.begin() + 4) == "FNT2") {
        // FNT2: u16 glifi, u8 righe, u8 spazio fra le lettere; per glifo u32 codepoint, u8 larghezza (<= 16), righe da
        // u32 (2 bit per colonna, colonna 0 nei bit alti)
        l.firma("FNT2");
        f.formato_ = 2;
        const auto n = l.leggi<std::uint16_t>();
        f.righe_ = l.leggi<std::uint8_t>();
        f.spazio_ = l.leggi<std::uint8_t>();
        for (int i = 0; i < n; ++i) {
            const auto cp = static_cast<char32_t>(l.leggi<std::uint32_t>());
            Glifo g;
            g.w = l.leggi<std::uint8_t>();
            if (g.w > 16) throw std::runtime_error(percorso + ": glifo piu' largo di 16 colonne");
            for (int r = 0; r < f.righe_; ++r) g.righe.push_back(l.leggi<std::uint32_t>());
            f.glifi_.emplace(cp, std::move(g));
        }
        if (!f.glifi_.count(U'?')) throw std::runtime_error(percorso + ": manca il glifo '?'");
        return f;
    }
    l.firma("FNT1");
    const auto n = l.leggi<std::uint16_t>();
    f.righe_ = l.leggi<std::uint8_t>();
    for (int i = 0; i < n; ++i) {
        const auto cp = static_cast<char32_t>(l.leggi<std::uint32_t>());
        Glifo g;
        g.w = l.leggi<std::uint8_t>();
        const auto* r = l.blocco(static_cast<std::size_t>(f.righe_));
        for (int k = 0; k < f.righe_; ++k) {   // un bit per colonna -> livello 3 (tratto pieno)
            std::uint32_t v = 0;
            for (int c = 0; c < 8; ++c)
                if (r[k] & (0x80 >> c)) v |= 3u << (30 - 2 * c);
            g.righe.push_back(v);
        }
        // set delle maiuscole piccole (cifre, punteggiatura, maiuscole, accentate maiuscole): alto 5 righe,
        // scende di 2 sulla base delle maiuscole grandi; i segni che stanno in alto per natura restano dove sono
        const bool piccolo = (cp >= 0x20 && cp < 0x61) || (cp >= 0x7b && cp <= 0x7f) || (cp >= 0xc0 && cp <= 0xde);
        const bool in_alto = cp == U'\'' || cp == U'"' || cp == U'`' || cp == U'^' || cp == U'°';
        g.discesa = piccolo && !in_alto ? 2 : 0;
        f.glifi_.emplace(cp, std::move(g));
    }
    if (!f.glifi_.count(U'?')) throw std::runtime_error(percorso + ": manca il glifo '?'");
    return f;
}

std::vector<char32_t> Font::decodifica_utf8(std::string_view s) {
    std::vector<char32_t> out;
    for (std::size_t i = 0; i < s.size();) {
        const auto c = static_cast<unsigned char>(s[i]);
        int n = c < 0x80 ? 1 : (c >> 5) == 0x6 ? 2 : (c >> 4) == 0xe ? 3 : (c >> 3) == 0x1e ? 4 : 0;
        if (n == 0 || i + n > s.size()) { out.push_back(U'?'); ++i; continue; }   // sequenza non valida
        char32_t cp = n == 1 ? c : c & (0x7f >> n);
        for (int k = 1; k < n; ++k) cp = (cp << 6) | (static_cast<unsigned char>(s[i + k]) & 0x3f);
        out.push_back(cp);
        i += static_cast<std::size_t>(n);
    }
    return out;
}

const Font::Glifo* Font::trova(char32_t cp, bool grandi) const {
    if (formato_ == 2) {   // solo maiuscole: minuscole e accentate minuscole come maiuscole
        if ((cp >= U'a' && cp <= U'z') || (cp >= 0xe0 && cp <= 0xfe && cp != 0xf7)) {
            const auto su = glifi_.find(cp - 0x20);
            if (su != glifi_.end()) return &su->second;
        }
    } else if (grandi && ((cp >= U'A' && cp <= U'Z') || (cp >= 0xc0 && cp <= 0xde && cp != 0xd7))) {
        cp += 0x20;
    }
    auto it = glifi_.find(cp);
    return it != glifi_.end() ? &it->second : nullptr;
}

const Font::Glifo& Font::glifo(char32_t cp, bool grandi) const {
    const Glifo* g = trova(cp, grandi);
    return g ? *g : glifi_.at(U'?');
}

namespace {

// La traslitterazione latina di riserva delle lettere che un font non ha (europee e del glossario della traduzione)
struct Riserva {
    char32_t cp;
    const char* testo;
};
constexpr Riserva RISERVE[] = {
    {U'À', "A"}, {U'Á', "A"}, {U'Â', "A"}, {U'Ã', "A"}, {U'Ä', "A"}, {U'Å', "A"}, {U'Æ', "AE"}, {U'Ç', "C"},
    {U'È', "E"}, {U'É', "E"}, {U'Ê', "E"}, {U'Ë', "E"}, {U'Ì', "I"}, {U'Í', "I"}, {U'Î', "I"}, {U'Ï', "I"},
    {U'Ð', "D"}, {U'Ñ', "N"}, {U'Ò', "O"}, {U'Ó', "O"}, {U'Ô', "O"}, {U'Õ', "O"}, {U'Ö', "O"}, {U'Ø', "O"},
    {U'Ù', "U"}, {U'Ú', "U"}, {U'Û', "U"}, {U'Ü', "U"}, {U'Ý', "Y"}, {U'Þ', "TH"}, {U'ß', "ss"},
    {U'à', "a"}, {U'á', "a"}, {U'â', "a"}, {U'ã', "a"}, {U'ä', "a"}, {U'å', "a"}, {U'æ', "ae"}, {U'ç', "c"},
    {U'è', "e"}, {U'é', "e"}, {U'ê', "e"}, {U'ë', "e"}, {U'ì', "i"}, {U'í', "i"}, {U'î', "i"}, {U'ï', "i"},
    {U'ð', "d"}, {U'ñ', "n"}, {U'ò', "o"}, {U'ó', "o"}, {U'ô', "o"}, {U'õ', "o"}, {U'ö', "o"}, {U'ø', "o"},
    {U'ù', "u"}, {U'ú', "u"}, {U'û', "u"}, {U'ü', "u"}, {U'ý', "y"}, {U'þ', "th"}, {U'ÿ', "y"},
    {U'Ą', "A"}, {U'ą', "a"}, {U'Ć', "C"}, {U'ć', "c"}, {U'Č', "C"}, {U'č', "c"}, {U'Ď', "D"}, {U'ď', "d"},
    {U'Ę', "E"}, {U'ę', "e"}, {U'Ě', "E"}, {U'ě', "e"}, {U'Ğ', "G"}, {U'ğ', "g"}, {U'İ', "I"}, {U'ı', "i"},
    {U'Ł', "L"}, {U'ł', "l"}, {U'Ń', "N"}, {U'ń', "n"}, {U'Ň', "N"}, {U'ň', "n"}, {U'Ō', "O"}, {U'ō', "o"},
    {U'Ő', "O"}, {U'ő', "o"}, {U'Œ', "OE"}, {U'œ', "oe"}, {U'Ř', "R"}, {U'ř', "r"}, {U'Ś', "S"}, {U'ś', "s"},
    {U'Ş', "S"}, {U'ş', "s"}, {U'Š', "S"}, {U'š', "s"}, {U'Ť', "T"}, {U'ť', "t"}, {U'Ū', "U"}, {U'ū', "u"},
    {U'Ů', "U"}, {U'ů', "u"}, {U'Ű', "U"}, {U'ű', "u"}, {U'Ź', "Z"}, {U'ź', "z"}, {U'Ż', "Z"}, {U'ż', "z"},
    {U'Ž', "Z"}, {U'ž', "z"},
    // la tipografia delle decorazioni e delle motivazioni (le virgolette basse, gli ordinali «1ª CLASSE»): i font del
    // 1989 non le hanno
    {U'«', "\""}, {U'»', "\""}, {U'“', "\""}, {U'”', "\""}, {U'„', "\""}, {U'‘', "'"}, {U'’', "'"},
    {U'ª', "A"}, {U'º', "O"}, {U'–', "-"}, {U'—', "-"}, {U'…', "..."},
    // i segni delle schede e delle note che i font non hanno
    {U'≥', ">="}, {U'≤', "<="}, {U'±', "+-"}, {U'²', "2"}, {U'³', "3"}, {U'€', "EUR"}, {U'·', "."}};

}  // namespace

std::vector<char32_t> Font::caratteri(std::string_view testo, bool grandi) const {
    std::vector<char32_t> out;
    for (char32_t cp : decodifica_utf8(testo)) {
        if (cp < 0x80 || trova(cp, grandi)) {
            out.push_back(cp);
            continue;
        }
        const auto r = std::find_if(std::begin(RISERVE), std::end(RISERVE), [&](const Riserva& x) { return x.cp == cp; });
        if (r == std::end(RISERVE)) {
            out.push_back(cp);   // nessuna riserva: glifo() scrive «?»
            continue;
        }
        for (const char* p = r->testo; *p; ++p) out.push_back(static_cast<char32_t>(*p));
    }
    return out;
}

int Font::larghezza(std::string_view testo, int scala, int spazio, bool grandi) const {
    int w = 0, n = 0;
    for (char32_t cp : caratteri(testo, grandi)) { w += glifo(cp, grandi).w + spazio; ++n; }
    return n ? (w - spazio) * scala : 0;
}

int Font::scrivi(Tela& t, int x, int y, std::string_view testo, std::uint8_t colore, int scala, int spazio,
                 bool grandi) const {
    return disegna(t, x, y, testo, {-1, colore, colore}, scala, spazio, grandi);
}

int Font::scrivi_toni(Tela& t, int x, int y, std::string_view testo, const std::array<int, 3>& toni, int scala,
                      int spazio) const {
    return disegna(t, x, y, testo, toni, scala, spazio, false);
}

int Font::disegna(Tela& t, int x, int y, std::string_view testo, const std::array<int, 3>& toni, int scala, int spazio,
                  bool grandi) const {
    for (char32_t cp : caratteri(testo, grandi)) {
        const Glifo& g = glifo(cp, grandi);
        const int dy = allinea_base_ ? g.discesa * scala : 0;
        for (int r = 0; r < righe_; ++r)
            for (int c = 0; c < 16; ++c) {
                const int lv = g.livello(r, c);
                if (lv && toni[static_cast<std::size_t>(lv - 1)] >= 0)
                    t.rettangolo_pieno(x + c * scala, y + dy + r * scala, scala, scala,
                                       static_cast<std::uint8_t>(toni[static_cast<std::size_t>(lv - 1)]));
            }
        x += (g.w + spazio) * scala;
    }
    return x;
}

}  // namespace m688
