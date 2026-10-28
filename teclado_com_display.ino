// Grupo 2 - Senha de segurança
// Placa: TTGO T-Display (ESP32, display ST7789 135x240)
// Keypad, de Mark Stanley
// Adafruit ST7735 and ST7789 Library + Adafruit GFX, da Adafruit
// PubSubClient, de Nick O'Leary
// Protocolo MQTT: RFC 001 do Grupo 1 (https://github.com/GroosL/KTNE-ESP32)


#include <WiFi.h>
#include <PubSubClient.h>
#include <Keypad.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

#define MODULO_ID "2"

constexpr char* WIFI_SSID     = "iot2026";
constexpr char* WIFI_PASSWORD = "Senhab0@";
constexpr char* MQTT_BROKER   = "10.150.207.154";
constexpr int   MQTT_PORT     = 1883;

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
WiFiClient espClient;
PubSubClient client(espClient);

// Senha usada enquanto nenhum módulo enviar dica em jogo/modulo/2/dica
const String SENHA_PADRAO = "1234";
const unsigned long BLOQUEIO_MS = 3000;  // bloqueio após erro

String senhaBase = SENHA_PADRAO;
String entrada = "";
char serialBomba[8] = "-----";
bool desafioAtivo = false;
bool resolvido = false;

// Regra do manual (RFC 3.2): se o último dígito do serial for ímpar, soma 2 ao primeiro dígito
String senhaAtual() {
  String senha = senhaBase;
  char ultimo = serialBomba[4];
  if (isdigit(ultimo) && (ultimo - '0') % 2 == 1 && senha.length() && isdigit(senha[0])) {
    senha[0] = '0' + (senha[0] - '0' + 2) % 10;
  }
  return senha;
}

void mensagem(const char* texto, uint16_t fundo) {
  tft.fillScreen(fundo);
  tft.setTextColor(ST77XX_WHITE, fundo);
  tft.setTextSize(2);
  tft.setCursor(10, 5);
  tft.print("SN: ");
  tft.print(serialBomba);
  tft.setCursor(10, 40);
  tft.print(texto);
}

void telaInicial() {
  entrada = "";
  if (resolvido) mensagem("Desarmado", ST77XX_GREEN);
  else if (desafioAtivo) mensagem("Digite a senha:", ST77XX_BLACK);
  else mensagem("Aguardando...", ST77XX_BLACK);
}

void callbackMqtt(char* topic, byte* payload, unsigned int length) {
  char msg[length + 1];
  memcpy(msg, payload, length);
  msg[length] = '\0';

  if (strcmp(topic, "jogo/estado") == 0) {
    if (strcmp(msg, "START") == 0) {
      desafioAtivo = true;
      resolvido = false;
      senhaBase = SENHA_PADRAO;
    } else {  // RESET, VICTORY ou DEFEAT travam o teclado
      desafioAtivo = false;
    }
    telaInicial();
  }
  else if (strcmp(topic, "jogo/serial") == 0) {
    strncpy(serialBomba, msg, sizeof(serialBomba) - 1);
    telaInicial();
  }
  else if (strcmp(topic, "jogo/modulo/" MODULO_ID "/dica") == 0) {
    senhaBase = msg;  // ex: Grupo 3 envia "8310" ao resolver
  }
}

void reconectarMQTT() {
  while (!client.connected()) {
    if (client.connect("Modulo_" MODULO_ID)) {
      client.subscribe("jogo/estado");
      client.subscribe("jogo/serial");
      client.subscribe("jogo/modulo/" MODULO_ID "/dica");
    } else {
      delay(1000);
    }
  }
}

void setup() {
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);  // liga a luz de fundo

  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);  // pinos SPI do T-Display, antes do init
  tft.init(135, 240);
  tft.setRotation(1);  // horizontal: 240x135
  mensagem("Conectando...", ST77XX_BLACK);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) delay(250);

  client.setServer(MQTT_BROKER, MQTT_PORT);
  client.setCallback(callbackMqtt);
  reconectarMQTT();  // estado e serial chegam logo (retain)
  telaInicial();
}

void loop() {
  if (!client.connected()) reconectarMQTT();
  client.loop();

  char tecla = teclado.getKey();
  if (!tecla || !desafioAtivo || resolvido) return;

  if (tecla == '*') {  // '*' apaga a entrada
    telaInicial();
    return;
  }

  entrada += tecla;
  tft.setTextSize(4);
  tft.setCursor(10, 80);
  tft.print(entrada);  // mostra a tecla clicada

  String senha = senhaAtual();
  if (entrada.length() == senha.length()) {
    delay(500);
    if (entrada == senha) {
      resolvido = true;
      client.publish("jogo/modulo/" MODULO_ID "/evento", "SOLVED");
      telaInicial();
    } else {
      client.publish("jogo/modulo/" MODULO_ID "/evento", "ERROR");
      mensagem("Senha errada", ST77XX_RED);
      delay(BLOQUEIO_MS);
      client.loop();  // não perder mensagens durante o bloqueio
      telaInicial();
    }
  }
}
