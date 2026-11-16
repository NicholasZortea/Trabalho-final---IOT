// =============================================================================
//  Grupo 2 - Senha Química (versão standalone)
// -----------------------------------------------------------------------------
//  Placa: TTGO T-Display (ESP32, display ST7789 135x240) + teclado 4x4
//
//  Bibliotecas:
//    - Keypad, de Mark Stanley
//    - Adafruit ST7735 and ST7789 Library + Adafruit GFX, da Adafruit
//
//  Como jogar: veja o README.md. Regras e ajustes ficam em Config.h.
//
//  Organização:
//    Config.h      regras, dificuldade, teclas e pinos
//    Elementos.*   tabela periódica (número atômico -> símbolo)
//    Desafio.*     sorteio dos elementos e cálculo da senha
//    Tela.*        tudo que é desenhado no display
//    Recorde.*     recorde salvo na flash
//    Jogo.*        máquina de estados que liga tudo
// =============================================================================

#include <Keypad.h>
#include "Config.h"
#include "Jogo.h"
#include "Tela.h"

namespace {

char MAPA_TECLAS[config::LINHAS_TECLADO][config::COLUNAS_TECLADO] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// A biblioteca Keypad exige ponteiros não constantes
byte pinosLinhas[config::LINHAS_TECLADO];
byte pinosColunas[config::COLUNAS_TECLADO];

Keypad* teclado = nullptr;
Jogo    jogo;

void iniciarTeclado() {
  memcpy(pinosLinhas,  config::PINOS_LINHAS,  sizeof(pinosLinhas));
  memcpy(pinosColunas, config::PINOS_COLUNAS, sizeof(pinosColunas));

  static Keypad instancia(makeKeymap(MAPA_TECLAS), pinosLinhas, pinosColunas,
                          config::LINHAS_TECLADO, config::COLUNAS_TECLADO);
  teclado = &instancia;
}

}  // namespace

void setup() {
  randomSeed(esp_random());  // sorteio diferente a cada vez que liga
  tela::iniciar();
  iniciarTeclado();
  jogo.iniciar(millis());
}

void loop() {
  const uint32_t agora = millis();

  const char tecla = teclado->getKey();
  if (tecla) jogo.tratarTecla(tecla, agora);

  jogo.atualizar(agora);
}
