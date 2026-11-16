#include "Desafio.h"

namespace {

constexpr uint8_t MAIOR_NUMERO_UM_DIGITO = 9;
constexpr uint8_t SOMA_SERIAL_IMPAR      = 2;

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

// Serial no formato "XXXXd": 4 caracteres alfanuméricos + 1 dígito final,
// usado pela regra do dígito ímpar.
void sortearSerial(char* serial) {
  static const char ALFANUMERICOS[] = "ABCDEFGHJKLMNPQRSTUVWXYZ0123456789";
  const uint8_t quantidade = sizeof(ALFANUMERICOS) - 1;

  for (uint8_t i = 0; i < TAMANHO_SERIAL - 1; i++) {
    serial[i] = ALFANUMERICOS[random(quantidade)];
  }
  serial[TAMANHO_SERIAL - 1] = '0' + random(10);
  serial[TAMANHO_SERIAL]     = '\0';
}

bool ultimoDigitoImpar(const char* serial) {
  const char ultimo = serial[strlen(serial) - 1];
  return isdigit(ultimo) && (ultimo - '0') % 2 == 1;
}

}  // namespace

void calcularSenha(const uint8_t* numerosAtomicos, uint8_t quantidade,
                   const char* serial, char* senhaSaida) {
  senhaSaida[0] = '\0';
  for (uint8_t i = 0; i < quantidade; i++) {
    char numero[4];
    snprintf(numero, sizeof(numero), "%u", (unsigned)numerosAtomicos[i]);
    strcat(senhaSaida, numero);
  }

  if (ultimoDigitoImpar(serial)) {
    const uint8_t primeiro = senhaSaida[0] - '0';
    senhaSaida[0] = '0' + (primeiro + SOMA_SERIAL_IMPAR) % 10;
  }
}

Desafio gerarDesafio(uint8_t maiorNumeroAtomico) {
  Desafio desafio;
  sortearElementos(desafio, maiorNumeroAtomico);
  sortearSerial(desafio.serial);
  calcularSenha(desafio.numerosAtomicos, desafio.quantidadeElementos,
                desafio.serial, desafio.senha);
  return desafio;
}
