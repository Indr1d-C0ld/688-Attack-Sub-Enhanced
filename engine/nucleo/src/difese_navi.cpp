#include "nucleo/difese_navi.hpp"

#include <algorithm>

namespace m688 {

namespace {

// impegna il primo libero all'ora t fino a `fino`; falso se sono tutti impegnati
bool prendi(std::vector<double>& fino_a, int quanti, double t, double fino) {
    if (fino_a.size() < static_cast<std::size_t>(quanti)) fino_a.resize(static_cast<std::size_t>(quanti), 0.0);
    const auto libero = std::find_if(fino_a.begin(), fino_a.end(), [t](double x) { return x <= t; });
    if (libero == fino_a.end()) return false;
    *libero = fino;
    return true;
}

}  // namespace

double fattore_allerta(std::optional<int> livello) {
    if (!livello) return 1.0;
    return *livello <= 0 ? FATTORE_A0 : *livello == 1 ? FATTORE_A1 : 1.0;
}

int byte_in_allerta(int byte, double fattore) { return static_cast<int>(byte * fattore); }

double pk_phalanx(const DifesaClasse& d, int anno, double velocita_m_s, double fattore) {
    const double pk = d.block1_dal > 0 && anno >= d.block1_dal ? PK_PHALANX_BLOCK1 : PK_PHALANX_BLOCK0;
    return pk * fattore * (velocita_m_s > SUPERSONICO_M_S ? FATTORE_SUPERSONICO : 1.0);
}

EsitoDifesa ImpegnoDifese::arrivo(const DifesaClasse* classe, int byte, double fattore, double t, double velocita_m_s, int anno, Casuale& caso) {
    const int r = static_cast<int>(caso.prossimo() >> 56);   // sempre: la sequenza non dipende dai canali
    if (!classe) return r < byte ? EsitoDifesa::fermato_byte : EsitoDifesa::colpo;
    if (prendi(canali_, classe->canali, t, t + CANALE_IMPEGNATO_S) && r < byte_in_allerta(byte, fattore)) return EsitoDifesa::fermato_byte;
    if (classe->phalanx <= 0 || !prendi(affusti_, classe->phalanx, t, t + PORTATA_PHALANX_M / velocita_m_s)) return EsitoDifesa::colpo;
    return caso.uniforme() < pk_phalanx(*classe, anno, velocita_m_s, fattore) ? EsitoDifesa::fermato_phalanx : EsitoDifesa::colpo;
}

}  // namespace m688
