// =============================================================================
//  Conexão Wi-Fi + MQTT com a Central (protocolo: RFC 001 do Grupo 1,
//  https://github.com/GroosL/KTNE-ESP32).
//
//  Ao conectar: publica ONLINE (retain) em jogo/modulo/<ID>/status, com LWT
//  OFFLINE, e assina jogo/estado, jogo/serial, jogo/modulo/ativo, jogo/tempo
//  e jogo/modulo/<ID>/dica/+.
//
//  Nada aqui usa delay() em loop: se a rede cair, o jogo continua rodando e a
//  reconexão é tentada a cada config::INTERVALO_RECONEXAO_MS.
// =============================================================================
#pragma once

#include <Arduino.h>

namespace rede {

// Último valor recebido em jogo/estado. ESPERA = lobby ou RESET.
enum class EstadoPartida : uint8_t { ESPERA, RODANDO, VITORIA, DERROTA };

void iniciar();
void atualizar(uint32_t agora);  // chamar a cada loop()

bool          conectado();
EstadoPartida estadoPartida();
bool          minhaVez();          // jogo/modulo/ativo == config::MODULO_ID
const char*   serial();            // "-----" até a Central mandar um
uint16_t      tempoCentral();      // segundos restantes (0 se a Central não mandar)

// true uma vez por dica recebida; preenche quem mandou e o conteúdo.
bool novaDica(uint8_t& origem, const char*& conteudo);

// Só publicam com a partida rodando e na vez deste módulo (a Central ignora
// o resto). SOLVED é enviado no máximo uma vez por partida.
void publicarResolvido();
void publicarErro();

void enviarDica(uint8_t destino, const char* conteudo);  // direto a um grupo
void enviarDicaAleatoria(const char* conteudo);          // Central sorteia

}  // namespace rede
