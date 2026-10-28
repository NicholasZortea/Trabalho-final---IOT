// Placa: TTGO T-Display (ESP32, display ST7789 135x240)
// Keypad, de Mark Stanley
// Adafruit ST7735 and ST7789 Library + Adafruit GFX, da Adafruit


#include <Keypad.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

// Pinos do display integrado do T-Display
#define TFT_MOSI 19
#define TFT_SCLK 18
#define TFT_CS    5
#define TFT_DC   16
#define TFT_RST  23
#define TFT_BL    4

const byte LINHAS = 4, COLUNAS = 4;
char teclas[LINHAS][COLUNAS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
// Linhas usam INPUT_PULLUP: evitar GPIO 34-39 (sem pull-up) e pinos de boot (0, 2, 12, 15)
byte pinosLinhas[LINHAS]   = {13, 17, 21, 22};
byte pinosColunas[COLUNAS] = {25, 26, 27, 32};

Keypad teclado = Keypad(makeKeymap(teclas), pinosLinhas, pinosColunas, LINHAS, COLUNAS);
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

const String SENHA = "1234";
String entrada = "";

void telaInicial() {
  entrada = "";
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 20);
  tft.print("Digite a senha:");
}

void setup() {
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);  // liga a luz de fundo

  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);  // pinos SPI do T-Display, antes do init
  tft.init(135, 240);
  tft.setRotation(1);  // horizontal: 240x135
  telaInicial();
}

void loop() {
  char tecla = teclado.getKey();
  if (!tecla) return;

  if (tecla == '*') {  // '*' apaga a entrada
    telaInicial();
    return;
  }

  entrada += tecla;
  tft.setTextSize(4);
  tft.setCursor(10, 60);
  tft.print(entrada);  // mostra a tecla clicada

  if (entrada.length() == 4) {
    delay(500);
    bool correta = entrada == SENHA;
    tft.fillScreen(correta ? ST77XX_GREEN : ST77XX_RED);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.setCursor(10, 50);
    tft.print(correta ? "Senha correta" : "Senha errada");
    delay(2000);
    telaInicial();
  }
}
