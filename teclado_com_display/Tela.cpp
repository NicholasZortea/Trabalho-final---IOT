#include "Tela.h"

#include <SPI.h>
#include <Adafruit_GFX.h>
#include "Config.h"
#include "Elementos.h"

namespace tela {

namespace {

constexpr uint16_t LARGURA = 240;
constexpr uint16_t ALTURA  = 135;
constexpr uint16_t MARGEM  = 6;

constexpr uint16_t Y_PLACAR   = 4;
constexpr uint16_t Y_DICAS    = 30;
constexpr uint16_t Y_MENSAGEM = 62;
constexpr uint16_t Y_ENTRADA  = 86;
constexpr uint16_t Y_RODAPE   = 124;

constexpr uint16_t X_VIDAS       = 80;
constexpr uint16_t RAIO_VIDA     = 6;
constexpr uint16_t ESPACO_VIDAS  = 18;
constexpr uint16_t X_TEMPO       = 180;
constexpr uint16_t ALERTA_TEMPO_S = 10;

constexpr uint16_t COR_TEXTO     = ST77XX_WHITE;
constexpr uint16_t COR_VIDA      = ST77XX_RED;
constexpr uint16_t COR_VIDA_VAZIA = 0x4208;  // cinza

Adafruit_ST7789 tft(config::PINO_TFT_CS, config::PINO_TFT_DC, config::PINO_TFT_RST);
uint16_t corFundoAtual = COR_NORMAL;

void limpar(uint16_t corFundo) {
  corFundoAtual = corFundo;
  tft.fillScreen(corFundo);
  tft.setTextColor(COR_TEXTO, corFundo);
}

void escrever(uint16_t x, uint16_t y, uint8_t tamanho, const char* texto) {
  tft.setTextSize(tamanho);
  tft.setCursor(x, y);
  tft.print(texto);
}

void desenharVidas(uint8_t vidas) {
  for (uint8_t i = 0; i < config::VIDAS_INICIAIS; i++) {
    const uint16_t cor = (i < vidas) ? COR_VIDA : COR_VIDA_VAZIA;
    tft.fillCircle(X_VIDAS + i * ESPACO_VIDAS, Y_PLACAR + RAIO_VIDA + 1, RAIO_VIDA, cor);
  }
}

void desenharPlacar(const Placar& placar) {
  char texto[8];
  snprintf(texto, sizeof(texto), "Nv%u", (unsigned)placar.nivel);
  escrever(MARGEM, Y_PLACAR, 2, texto);
  desenharVidas(placar.vidas);
  tempo(placar.segundosRestantes);
}

void desenharDicas(const Desafio& desafio) {
  tft.setTextSize(3);
  tft.setCursor(MARGEM, Y_DICAS);
  for (uint8_t i = 0; i < desafio.quantidadeElementos; i++) {
    tft.print(elementos::simbolo(desafio.numerosAtomicos[i]));
    tft.print(' ');
  }
}

void desenharRodape(uint32_t pontos) {
  char texto[24];
  snprintf(texto, sizeof(texto), "Pontos: %lu", (unsigned long)pontos);
  escrever(MARGEM, Y_RODAPE, 1, texto);
}

}  // namespace

void iniciar() {
  pinMode(config::PINO_TFT_BL, OUTPUT);
  digitalWrite(config::PINO_TFT_BL, HIGH);  // liga a luz de fundo

  // Pinos SPI do T-Display precisam ser definidos antes do init
  SPI.begin(config::PINO_TFT_SCLK, -1, config::PINO_TFT_MOSI, config::PINO_TFT_CS);
  tft.init(ALTURA, LARGURA);
  tft.setRotation(1);  // horizontal: 240x135
}

void menu(uint32_t recorde) {
  limpar(COR_NORMAL);
  escrever(MARGEM, 6, 3, "SENHA");
  escrever(MARGEM, 34, 3, "QUIMICA");

  escrever(MARGEM, 70, 1, "Senha = numeros atomicos em ordem");
  escrever(MARGEM, 82, 1, "Ex: Ne C B -> 1065");
  escrever(MARGEM, 94, 1, "* limpa   D apaga");

  char texto[24];
  snprintf(texto, sizeof(texto), "Recorde: %lu", (unsigned long)recorde);
  escrever(MARGEM, 110, 1, texto);
  escrever(MARGEM, Y_RODAPE, 1, "Aperte # para jogar");
}

void rodada(const Desafio& desafio, const Placar& placar,
            const char* mensagem, uint16_t corFundo) {
  limpar(corFundo);
  desenharPlacar(placar);
  desenharDicas(desafio);
  escrever(MARGEM, Y_MENSAGEM, 2, mensagem);
  desenharRodape(placar.pontos);
}

void tempo(uint16_t segundosRestantes) {
  const bool acabando = segundosRestantes <= ALERTA_TEMPO_S;
  tft.setTextColor(acabando ? ST77XX_YELLOW : COR_TEXTO, corFundoAtual);

  char texto[6];
  snprintf(texto, sizeof(texto), "%3us", (unsigned)segundosRestantes);
  escrever(X_TEMPO, Y_PLACAR, 2, texto);

  tft.setTextColor(COR_TEXTO, corFundoAtual);
}

void entrada(const char* digitado) {
  // Completa com espaços para apagar dígitos removidos com a tecla D
  char texto[config::TAMANHO_SENHA + 1];
  snprintf(texto, sizeof(texto), "%-*s", config::TAMANHO_SENHA, digitado);
  escrever(MARGEM, Y_ENTRADA, 4, texto);
}

void fimDeJogo(uint32_t pontos, uint32_t recorde, bool novoRecorde,
               const char* ultimaSenha) {
  limpar(COR_ALERTA);
  escrever(MARGEM, 6, 3, "BOOM!");

  char texto[32];
  snprintf(texto, sizeof(texto), "Senha era %s", ultimaSenha);
  escrever(MARGEM, 40, 2, texto);

  snprintf(texto, sizeof(texto), "Pontos: %lu", (unsigned long)pontos);
  escrever(MARGEM, 66, 2, texto);

  if (novoRecorde) {
    escrever(MARGEM, 90, 2, "Novo recorde!");
  } else {
    snprintf(texto, sizeof(texto), "Recorde: %lu", (unsigned long)recorde);
    escrever(MARGEM, 90, 2, texto);
  }
  escrever(MARGEM, Y_RODAPE, 1, "Aperte # para voltar");
}

}  // namespace tela
