// =============================================================================
//  Desafio de uma rodada: elementos sorteados, serial da bomba e a senha.
//
//  Regras da senha:
//    1. Junte, em ordem, os números atômicos dos elementos mostrados
//       (ex: Ne C B -> 10 6 5 -> "1065").
//    2. Se o último dígito do serial for ímpar, some 2 ao primeiro dígito
//       da senha, sem "vai um" (ex: 9 + 2 = 1).
// =============================================================================
#pragma once

#include <Arduino.h>
#include "Config.h"

constexpr uint8_t TAMANHO_SERIAL = 5;

struct Desafio {
  uint8_t numerosAtomicos[config::TAMANHO_SENHA];  // no pior caso, 1 elemento por dígito
  uint8_t quantidadeElementos;
  char    serial[TAMANHO_SERIAL + 1];
  char    senha[config::TAMANHO_SENHA + 1];
};

// Sorteia um desafio usando elementos de 1 até maiorNumeroAtomico.
Desafio gerarDesafio(uint8_t maiorNumeroAtomico);

// Calcula a senha a partir dos elementos e do serial (função pura, sem sorteio).
void calcularSenha(const uint8_t* numerosAtomicos, uint8_t quantidade,
                   const char* serial, char* senhaSaida);
