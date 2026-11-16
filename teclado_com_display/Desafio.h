// =============================================================================
//  Desafio de uma rodada: elementos sorteados e a senha.
//
//  Regra da senha: junte, em ordem, os números atômicos dos elementos
//  mostrados (ex: Ne C B -> 10 6 5 -> "1065").
// =============================================================================
#pragma once

#include <Arduino.h>
#include "Config.h"

struct Desafio {
  uint8_t numerosAtomicos[config::TAMANHO_SENHA];  // no pior caso, 1 elemento por dígito
  uint8_t quantidadeElementos;
  char    senha[config::TAMANHO_SENHA + 1];
};

// Sorteia um desafio usando elementos de 1 até maiorNumeroAtomico.
Desafio gerarDesafio(uint8_t maiorNumeroAtomico);

// Calcula a senha a partir dos elementos (função pura, sem sorteio).
void calcularSenha(const uint8_t* numerosAtomicos, uint8_t quantidade, char* senhaSaida);
