// =============================================================================
//  Máquina de estados do jogo.
//
//   MENU ──#──> DIGITANDO ──4 dígitos──> CONFERINDO ──certa──> RODADA_VENCIDA
//                 ▲   │                      │                      │
//                 │   └─tempo acabou─┐     errada                próximo nível
//                 │                  ▼       ▼                      │
//                 │          TEMPO_ESGOTADO◄─BLOQUEADO              │
//                 └──────────────────┴───────┴──────────────────────┘
//
//   O relógio continua correndo durante o BLOQUEADO; se acabar ali, a
//   rodada vai para TEMPO_ESGOTADO.
//   Errar a senha ou deixar o tempo acabar custa 1 vida. Sem vidas, a
//   partida vai para FIM_DE_JOGO; '#' volta ao MENU.
//   Todas as esperas usam millis(): o loop nunca fica travado em delay().
// =============================================================================
#pragma once

#include <Arduino.h>
#include "Config.h"
#include "Desafio.h"
#include "Tela.h"

class Jogo {
 public:
  void iniciar(uint32_t agora);
  void atualizar(uint32_t agora);
  void tratarTecla(char tecla, uint32_t agora);

 private:
  enum class Estado {
    MENU,
    DIGITANDO,
    CONFERINDO,
    BLOQUEADO,
    RODADA_VENCIDA,
    TEMPO_ESGOTADO,
    FIM_DE_JOGO
  };

  // Transições
  void irParaMenu(uint32_t agora);
  void novaPartida(uint32_t agora);
  void novaRodada(uint32_t agora);
  void conferirSenha(uint32_t agora);
  void venceuRodada(uint32_t agora);
  void errouSenha(uint32_t agora);
  void esgotouTempo(uint32_t agora);
  void fimDeJogo(uint32_t agora);
  void voltarADigitar(uint32_t agora);

  // Entrada do jogador
  void digitar(char tecla, uint32_t agora);
  void apagarUltimoDigito();
  void limparEntrada();

  // Tempo da rodada
  void atualizarCronometro(uint32_t agora);
  uint16_t segundosRestantes(uint32_t agora) const;
  uint16_t duracaoRodadaSegundos() const;
  uint8_t  maiorNumeroAtomico() const;

  void mudarEstado(Estado novo, uint32_t agora);
  bool esperou(uint32_t agora, uint32_t duracaoMs) const;
  Placar placar(uint32_t agora) const;
  void mostrarRodada(const char* mensagem, uint16_t corFundo, uint32_t agora);

  Estado   estado_ = Estado::MENU;
  uint32_t inicioEstadoMs_ = 0;
  uint32_t fimRodadaMs_ = 0;
  uint16_t ultimoSegundoMostrado_ = 0;
  uint16_t segundosQuandoVenceu_ = 0;

  Desafio  desafio_{};
  char     entrada_[config::TAMANHO_SENHA + 1] = "";
  uint8_t  digitados_ = 0;

  uint8_t  nivel_ = 1;
  uint8_t  vidas_ = config::VIDAS_INICIAIS;
  uint32_t pontos_ = 0;
};
