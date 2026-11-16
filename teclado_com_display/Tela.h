// =============================================================================
//  Desenho no display (240x135, horizontal).
//
//    y=4   nível, vidas e tempo restante
//    y=30  dicas: símbolos dos elementos sorteados
//    y=62  mensagem de status
//    y=86  senha sendo digitada
//    y=124 serial da bomba e pontuação
// =============================================================================
#pragma once

#include <Arduino.h>
#include <Adafruit_ST7789.h>
#include "Desafio.h"

struct Placar {
  uint8_t  nivel;
  uint8_t  vidas;
  uint16_t segundosRestantes;
  uint32_t pontos;
};

namespace tela {

constexpr uint16_t COR_NORMAL  = ST77XX_BLACK;
constexpr uint16_t COR_ACERTO  = 0x03E0;  // verde escuro, legível com texto branco
constexpr uint16_t COR_ERRO    = ST77XX_RED;
constexpr uint16_t COR_ALERTA  = 0x7800;  // vermelho escuro

void iniciar();

void menu(uint32_t recorde);

// Redesenha a tela inteira da rodada.
void rodada(const Desafio& desafio, const Placar& placar,
            const char* mensagem, uint16_t corFundo);

// Atualizações parciais, sem piscar a tela.
void tempo(uint16_t segundosRestantes);
void entrada(const char* digitado);

void fimDeJogo(uint32_t pontos, uint32_t recorde, bool novoRecorde,
               const char* ultimaSenha);

}  // namespace tela
