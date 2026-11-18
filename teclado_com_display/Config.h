// =============================================================================
//  Configuração central do jogo: regras, dificuldade, teclas e pinagem.
//  Para ajustar o jogo, altere apenas este arquivo.
// =============================================================================
#pragma once

#include <Arduino.h>

namespace config {

// --- Regras ------------------------------------------------------------------
constexpr uint8_t  TAMANHO_SENHA        = 4;     // dígitos da senha
constexpr uint8_t  VIDAS_INICIAIS       = 3;
constexpr uint32_t BLOQUEIO_MS          = 3000;  // teclado travado após errar
constexpr uint32_t PAUSA_CONFERENCIA_MS = 400;   // pausa antes de conferir a senha
constexpr uint32_t TEMPO_RESULTADO_MS   = 2500;  // tempo da tela de acerto/tempo esgotado

// --- Tempo por rodada (diminui a cada nível) ---------------------------------
constexpr uint16_t TEMPO_RODADA_INICIAL_S    = 60;
constexpr uint16_t TEMPO_RODADA_MINIMO_S     = 20;
constexpr uint16_t REDUCAO_TEMPO_POR_NIVEL_S = 5;

// --- Dificuldade: maior número atômico que pode ser sorteado -----------------
//  Nível 1 usa só os 20 primeiros elementos; cada nível libera mais 20.
constexpr uint8_t NUMERO_ATOMICO_NIVEL_1   = 20;
constexpr uint8_t AUMENTO_NUMERO_POR_NIVEL = 20;
constexpr uint8_t NUMERO_ATOMICO_MAXIMO    = 99;  // maior número com 2 dígitos

// --- Pontuação ---------------------------------------------------------------
constexpr uint16_t PONTOS_POR_NIVEL = 10;  // + 1 ponto por segundo restante

// --- Teclas especiais --------------------------------------------------------
constexpr char TECLA_LIMPAR    = '*';  // apaga tudo que foi digitado
constexpr char TECLA_APAGAR    = 'D';  // apaga o último dígito
constexpr char TECLA_CONFIRMAR = '#';  // começa / recomeça a partida

// --- Rede: Wi-Fi do laboratório + broker MQTT da Central (RFC 001) ----------
constexpr uint8_t  MODULO_ID              = 2;  // número do grupo
constexpr char     WIFI_SSID[]            = "iot2022";
constexpr char     WIFI_SENHA[]           = "S3nhab0@";
constexpr char     MQTT_BROKER[]          = "192.168.2.100";
constexpr uint16_t MQTT_PORTA             = 1883;
constexpr uint32_t INTERVALO_RECONEXAO_MS = 5000;

// --- Teclado 4x4 -------------------------------------------------------------
//  Linhas usam INPUT_PULLUP: evitar GPIO 34-39 (sem pull-up) e pinos de boot
//  (0, 2, 12, 15).
constexpr uint8_t LINHAS_TECLADO  = 4;
constexpr uint8_t COLUNAS_TECLADO = 4;
constexpr uint8_t PINOS_LINHAS[LINHAS_TECLADO]   = {13, 17, 21, 22};
constexpr uint8_t PINOS_COLUNAS[COLUNAS_TECLADO] = {25, 26, 27, 32};

// --- Display integrado do TTGO T-Display (ST7789 135x240) --------------------
constexpr int8_t PINO_TFT_MOSI = 19;
constexpr int8_t PINO_TFT_SCLK = 18;
constexpr int8_t PINO_TFT_CS   = 5;
constexpr int8_t PINO_TFT_DC   = 16;
constexpr int8_t PINO_TFT_RST  = 23;
constexpr int8_t PINO_TFT_BL   = 4;

}  // namespace config
