#pragma once
// Il disegno del periscopio costa piu' di un fotogramma: le righe (o le colonne) di una tela si possono fare su piu' fili, perche'
// ognuna scrive solo i suoi pixel e il colore dipende solo dal punto. Stessi byte con un filo o con piu'.

#include <algorithm>
#include <future>
#include <system_error>
#include <thread>
#include <vector>

namespace m688 {

template <class F>
void per_righe(int da, int a, const F& fa) {
    const int fili = std::clamp(static_cast<int>(std::thread::hardware_concurrency()), 1, 4);
    const int quante = a - da;
    const auto striscia = [&](int k) {
        for (int i = da + quante * k / fili; i < da + quante * (k + 1) / fili; ++i) fa(i);
    };
    std::vector<std::future<void>> altri;
    int fatte = 1;
    try {
        for (; fatte < fili; ++fatte) altri.push_back(std::async(std::launch::async, striscia, fatte));
    } catch (const std::system_error&) {   // niente fili in piu': fa il resto chi c'e'
    }
    striscia(0);
    for (auto& f : altri) f.get();
    for (int k = fatte; k < fili; ++k) striscia(k);
}

}  // namespace m688
