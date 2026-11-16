// =============================================================================
//  Tabela periódica: símbolo de cada elemento a partir do número atômico.
// =============================================================================
#pragma once

#include <Arduino.h>

namespace elementos {

constexpr uint8_t TOTAL = 99;  // elementos de 1 (H) a 99 (Es)

// Símbolo do elemento (ex: 10 -> "Ne"). Retorna "?" fora do intervalo 1..TOTAL.
const char* simbolo(uint8_t numeroAtomico);

}  // namespace elementos
