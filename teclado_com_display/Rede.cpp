#include "Rede.h"

#include <PubSubClient.h>
#include <WiFi.h>
#include "Config.h"

namespace {

WiFiClient   wifi;
PubSubClient mqtt(wifi);

char topicoStatus[32];
char topicoEvento[32];
char topicoDicaAleatoria[32];
char prefixoDica[32];  // "jogo/modulo/<ID>/dica/"
char assinaturaDica[32];
char idCliente[16];

rede::EstadoPartida estado = rede::EstadoPartida::ESPERA;
bool     vez        = false;
bool     resolvido  = false;
char     serialBomba[8] = "-----";
uint16_t tempo      = 0;

bool    temDica   = false;
uint8_t origemDica = 0;
char    dica[64]  = "";

uint32_t ultimaTentativaMs = 0;

bool partidaNaMinhaVez() {
  return mqtt.connected() && estado == rede::EstadoPartida::RODANDO && vez;
}

void aoReceber(char* topico, byte* payload, unsigned int tamanho) {
  char msg[sizeof(dica)];
  if (tamanho > sizeof(msg) - 1) tamanho = sizeof(msg) - 1;
  memcpy(msg, payload, tamanho);
  msg[tamanho] = '\0';
  while (tamanho > 0 && isspace((unsigned char)msg[tamanho - 1])) msg[--tamanho] = '\0';

  if (strcmp(topico, "jogo/estado") == 0) {
    if (strcmp(msg, "START") == 0) {
      estado    = rede::EstadoPartida::RODANDO;
      resolvido = false;
      temDica   = false;
    } else if (strcmp(msg, "RESET") == 0) {
      estado    = rede::EstadoPartida::ESPERA;
      vez       = false;
      resolvido = false;
      temDica   = false;
    } else if (strcmp(msg, "VICTORY") == 0) {
      estado = rede::EstadoPartida::VITORIA;
      vez    = false;
    } else if (strcmp(msg, "DEFEAT") == 0) {
      estado = rede::EstadoPartida::DERROTA;
      vez    = false;
    }
  } else if (strcmp(topico, "jogo/modulo/ativo") == 0) {
    vez = atoi(msg) == config::MODULO_ID;
  } else if (strcmp(topico, "jogo/serial") == 0) {
    strlcpy(serialBomba, msg, sizeof(serialBomba));
  } else if (strcmp(topico, "jogo/tempo") == 0) {
    tempo = atoi(msg);
  } else {
    // jogo/modulo/<ID>/dica/<ORIGEM>
    const size_t n = strlen(prefixoDica);
    int origem = 0, pos = 0;
    if (strncmp(topico, prefixoDica, n) == 0 &&
        sscanf(topico + n, "%d%n", &origem, &pos) == 1 && topico[n + pos] == '\0') {
      origemDica = origem;
      strlcpy(dica, msg, sizeof(dica));
      temDica = true;
    }
  }
}

void conectarMqtt() {
  // LWT: se a placa cair, o broker publica OFFLINE (retain) por nós
  if (!mqtt.connect(idCliente, topicoStatus, 0, true, "OFFLINE")) return;

  mqtt.publish(topicoStatus, "ONLINE", true);
  mqtt.subscribe("jogo/estado");
  mqtt.subscribe("jogo/serial");
  mqtt.subscribe("jogo/modulo/ativo");
  mqtt.subscribe("jogo/tempo");
  mqtt.subscribe(assinaturaDica);
}

}  // namespace

namespace rede {

void iniciar() {
  const int id = config::MODULO_ID;
  snprintf(topicoStatus,        sizeof(topicoStatus),        "jogo/modulo/%d/status", id);
  snprintf(topicoEvento,        sizeof(topicoEvento),        "jogo/modulo/%d/evento", id);
  snprintf(topicoDicaAleatoria, sizeof(topicoDicaAleatoria), "jogo/modulo/%d/dica", id);
  snprintf(prefixoDica,         sizeof(prefixoDica),         "jogo/modulo/%d/dica/", id);
  snprintf(assinaturaDica,      sizeof(assinaturaDica),      "jogo/modulo/%d/dica/+", id);
  snprintf(idCliente,           sizeof(idCliente),           "Modulo_%d", id);

  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(config::WIFI_SSID, config::WIFI_SENHA);

  mqtt.setServer(config::MQTT_BROKER, config::MQTT_PORTA);
  mqtt.setCallback(aoReceber);
  mqtt.setSocketTimeout(2);  // limita quanto o connect() pode travar o loop

  ultimaTentativaMs = millis() - config::INTERVALO_RECONEXAO_MS;  // tenta já
}

void atualizar(uint32_t agora) {
  if (WiFi.status() != WL_CONNECTED) return;  // o Wi-Fi reconecta sozinho

  if (mqtt.connected()) {
    mqtt.loop();
    return;
  }
  if (agora - ultimaTentativaMs < config::INTERVALO_RECONEXAO_MS) return;
  ultimaTentativaMs = agora;
  conectarMqtt();
}

bool          conectado()     { return mqtt.connected(); }
EstadoPartida estadoPartida() { return estado; }
bool          minhaVez()      { return vez; }
const char*   serial()        { return serialBomba; }
uint16_t      tempoCentral()  { return tempo; }

bool novaDica(uint8_t& origem, const char*& conteudo) {
  if (!temDica) return false;
  temDica  = false;
  origem   = origemDica;
  conteudo = dica;
  return true;
}

void publicarResolvido() {
  if (!partidaNaMinhaVez() || resolvido) return;
  resolvido = true;
  mqtt.publish(topicoEvento, "SOLVED");
}

void publicarErro() {
  if (!partidaNaMinhaVez() || resolvido) return;
  mqtt.publish(topicoEvento, "ERROR");
}

void enviarDica(uint8_t destino, const char* conteudo) {
  if (!mqtt.connected() || estado != EstadoPartida::RODANDO) return;
  char topico[36];
  snprintf(topico, sizeof(topico), "jogo/modulo/%d/dica/%d", destino, config::MODULO_ID);
  mqtt.publish(topico, conteudo);
}

void enviarDicaAleatoria(const char* conteudo) {
  if (!mqtt.connected() || estado != EstadoPartida::RODANDO) return;
  mqtt.publish(topicoDicaAleatoria, conteudo);
}

}  // namespace rede
