#include "Desafio.h"

namespace {

constexpr uint8_t MAIOR_NUMERO_UM_DIGITO = 9;

uint8_t quantidadeDigitos(uint8_t numero) {
  return numero >= 10 ? 2 : 1;
}

// Sorteia elementos até completar TAMANHO_SENHA dígitos.
// Faltando só 1 dígito, sorteia apenas entre os elementos de 1 dígito.
void sortearElementos(Desafio& desafio, uint8_t maiorNumeroAtomico) {
  uint8_t digitos = 0;
  desafio.quantidadeElementos = 0;

  while (digitos < config::TAMANHO_SENHA) {
    const uint8_t faltam = config::TAMANHO_SENHA - digitos;
    const uint8_t limite = (faltam >= 2) ? maiorNumeroAtomico : MAIOR_NUMERO_UM_DIGITO;
    const uint8_t numero = random(1, limite + 1);

    desafio.numerosAtomicos[desafio.quantidadeElementos++] = numero;
    digitos += quantidadeDigitos(numero);
  }
}

}  // namespace

void calcularSenha(const uint8_t* numerosAtomicos, uint8_t quantidade, char* senhaSaida) {
  senhaSaida[0] = '\0';
  for (uint8_t i = 0; i < quantidade; i++) {
    char numero[4];
    snprintf(numero, sizeof(numero), "%u", (unsigned)numerosAtomicos[i]);
    strcat(senhaSaida, numero);
  }
}

Desafio gerarDesafio(uint8_t maiorNumeroAtomico) {
  Desafio desafio;
  sortearElementos(desafio, maiorNumeroAtomico);
  calcularSenha(desafio.numerosAtomicos, desafio.quantidadeElementos, desafio.senha);
  return desafio;
}
