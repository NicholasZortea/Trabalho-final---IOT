#include "Jogo.h"

#include "Recorde.h"

// =============================================================================
//  Interface pública
// =============================================================================

void Jogo::iniciar(uint32_t agora) {
  irParaMenu(agora);
}

void Jogo::atualizar(uint32_t agora) {
  switch (estado_) {
    case Estado::DIGITANDO:
      atualizarCronometro(agora);
      break;

    case Estado::CONFERINDO:
      if (esperou(agora, config::PAUSA_CONFERENCIA_MS)) conferirSenha(agora);
      break;

    case Estado::BLOQUEADO:
      atualizarCronometro(agora);  // o relógio continua correndo no bloqueio
      if (estado_ == Estado::BLOQUEADO && esperou(agora, config::BLOQUEIO_MS)) {
        voltarADigitar(agora);
      }
      break;

    case Estado::RODADA_VENCIDA:
      if (esperou(agora, config::TEMPO_RESULTADO_MS)) {
        nivel_++;
        novaRodada(agora);
      }
      break;

    case Estado::TEMPO_ESGOTADO:
      if (esperou(agora, config::TEMPO_RESULTADO_MS)) {
        if (vidas_ > 0) novaRodada(agora);
        else            fimDeJogo(agora);
      }
      break;

    case Estado::MENU:
    case Estado::FIM_DE_JOGO:
      break;  // só reagem a teclas
  }
}

void Jogo::tratarTecla(char tecla, uint32_t agora) {
  switch (estado_) {
    case Estado::MENU:
      if (tecla == config::TECLA_CONFIRMAR) novaPartida(agora);
      break;

    case Estado::FIM_DE_JOGO:
      if (tecla == config::TECLA_CONFIRMAR) irParaMenu(agora);
      break;

    case Estado::DIGITANDO:
      if      (tecla == config::TECLA_LIMPAR) limparEntrada();
      else if (tecla == config::TECLA_APAGAR) apagarUltimoDigito();
      else if (isdigit(tecla))                digitar(tecla, agora);
      break;

    default:
      break;  // teclado travado nos demais estados
  }
}

// =============================================================================
//  Transições
// =============================================================================

void Jogo::irParaMenu(uint32_t agora) {
  mudarEstado(Estado::MENU, agora);
  tela::menu(recorde::carregar());
}

void Jogo::novaPartida(uint32_t agora) {
  nivel_  = 1;
  vidas_  = config::VIDAS_INICIAIS;
  pontos_ = 0;
  novaRodada(agora);
}

void Jogo::novaRodada(uint32_t agora) {
  desafio_     = gerarDesafio(maiorNumeroAtomico());
  fimRodadaMs_ = agora + duracaoRodadaSegundos() * 1000UL;
  voltarADigitar(agora);
}

void Jogo::voltarADigitar(uint32_t agora) {
  limparEntrada();
  mudarEstado(Estado::DIGITANDO, agora);
  mostrarRodada("Digite a senha:", tela::COR_NORMAL, agora);
}

void Jogo::conferirSenha(uint32_t agora) {
  if (strcmp(entrada_, desafio_.senha) == 0) venceuRodada(agora);
  else                                       errouSenha(agora);
}

void Jogo::venceuRodada(uint32_t agora) {
  segundosQuandoVenceu_ = segundosRestantes(agora);
  pontos_ += nivel_ * config::PONTOS_POR_NIVEL + segundosQuandoVenceu_;

  mudarEstado(Estado::RODADA_VENCIDA, agora);
  mostrarRodada("Desarmado!", tela::COR_ACERTO, agora);
  tela::entrada(entrada_);
}

void Jogo::errouSenha(uint32_t agora) {
  vidas_--;
  if (vidas_ == 0) {
    fimDeJogo(agora);
    return;
  }
  mudarEstado(Estado::BLOQUEADO, agora);
  mostrarRodada("Senha errada!", tela::COR_ERRO, agora);
  tela::entrada(entrada_);
}

void Jogo::esgotouTempo(uint32_t agora) {
  vidas_--;
  mudarEstado(Estado::TEMPO_ESGOTADO, agora);

  char mensagem[24];
  snprintf(mensagem, sizeof(mensagem), "Tempo! Era %s", desafio_.senha);
  mostrarRodada(mensagem, tela::COR_ERRO, agora);
}

void Jogo::fimDeJogo(uint32_t agora) {
  mudarEstado(Estado::FIM_DE_JOGO, agora);
  const bool novoRecorde = recorde::registrar(pontos_);
  tela::fimDeJogo(pontos_, recorde::carregar(), novoRecorde, desafio_.senha);
}

// =============================================================================
//  Entrada do jogador
// =============================================================================

void Jogo::digitar(char tecla, uint32_t agora) {
  if (digitados_ >= config::TAMANHO_SENHA) return;

  entrada_[digitados_++] = tecla;
  entrada_[digitados_]   = '\0';
  tela::entrada(entrada_);

  if (digitados_ == config::TAMANHO_SENHA) mudarEstado(Estado::CONFERINDO, agora);
}

void Jogo::apagarUltimoDigito() {
  if (digitados_ == 0) return;
  entrada_[--digitados_] = '\0';
  tela::entrada(entrada_);
}

void Jogo::limparEntrada() {
  digitados_  = 0;
  entrada_[0] = '\0';
  if (estado_ == Estado::DIGITANDO) tela::entrada(entrada_);
}

// =============================================================================
//  Tempo da rodada e dificuldade
// =============================================================================

void Jogo::atualizarCronometro(uint32_t agora) {
  const uint16_t restantes = segundosRestantes(agora);
  if (restantes == 0) {
    esgotouTempo(agora);
    return;
  }
  if (restantes != ultimoSegundoMostrado_) {
    ultimoSegundoMostrado_ = restantes;
    tela::tempo(restantes);
  }
}

uint16_t Jogo::segundosRestantes(uint32_t agora) const {
  // Subtração com sinal: funciona mesmo quando millis() dá a volta
  const int32_t faltamMs = (int32_t)(fimRodadaMs_ - agora);
  if (faltamMs <= 0) return 0;
  return (faltamMs + 999) / 1000;  // arredonda para cima: "1s" até o fim
}

uint16_t Jogo::duracaoRodadaSegundos() const {
  const uint16_t reducao = (nivel_ - 1) * config::REDUCAO_TEMPO_POR_NIVEL_S;
  if (reducao >= config::TEMPO_RODADA_INICIAL_S - config::TEMPO_RODADA_MINIMO_S) {
    return config::TEMPO_RODADA_MINIMO_S;
  }
  return config::TEMPO_RODADA_INICIAL_S - reducao;
}

uint8_t Jogo::maiorNumeroAtomico() const {
  const uint16_t maior = config::NUMERO_ATOMICO_NIVEL_1 +
                         (nivel_ - 1) * config::AUMENTO_NUMERO_POR_NIVEL;
  return maior < config::NUMERO_ATOMICO_MAXIMO ? maior : config::NUMERO_ATOMICO_MAXIMO;
}

// =============================================================================
//  Auxiliares
// =============================================================================

void Jogo::mudarEstado(Estado novo, uint32_t agora) {
  estado_         = novo;
  inicioEstadoMs_ = agora;
}

bool Jogo::esperou(uint32_t agora, uint32_t duracaoMs) const {
  return agora - inicioEstadoMs_ >= duracaoMs;
}

Placar Jogo::placar(uint32_t agora) const {
  const uint16_t segundos = (estado_ == Estado::RODADA_VENCIDA)
                              ? segundosQuandoVenceu_
                              : segundosRestantes(agora);
  return {nivel_, vidas_, segundos, pontos_};
}

void Jogo::mostrarRodada(const char* mensagem, uint16_t corFundo, uint32_t agora) {
  const Placar atual = placar(agora);
  ultimoSegundoMostrado_ = atual.segundosRestantes;
  tela::rodada(desafio_, atual, mensagem, corFundo);
}
