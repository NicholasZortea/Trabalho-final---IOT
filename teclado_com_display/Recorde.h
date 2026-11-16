// =============================================================================
//  Recorde de pontos, salvo na memória flash (sobrevive a desligar a placa).
// =============================================================================
#pragma once

#include <Arduino.h>

namespace recorde {

uint32_t carregar();

// Salva se for maior que o recorde atual. Retorna true se bateu o recorde.
bool registrar(uint32_t pontos);

}  // namespace recorde
