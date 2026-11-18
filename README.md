# Trabalho final IoT — Senha Química (standalone)

Jogo de desarmar uma "bomba" descobrindo uma senha de 4 dígitos a partir de
símbolos da tabela periódica. Roda sozinho em um **TTGO T-Display (ESP32)**
com um **teclado matricial 4x4**. O módulo também se conecta ao broker MQTT
da Central (Grupo 1) — veja [Rede](#rede-mqtt).

## Como jogar

1. Na tela inicial, aperte `#`.
2. O display mostra símbolos de elementos químicos, por exemplo `Ne C B`.
3. A senha é a junção, em ordem, dos **números atômicos**:
   `Ne C B` → `10` `6` `5` → **`1065`**.
4. Digite os 4 dígitos. A senha é conferida automaticamente.

| Tecla | Função                         |
|-------|--------------------------------|
| `0-9` | digita                         |
| `D`   | apaga o último dígito          |
| `*`   | apaga tudo                     |
| `#`   | começa a partida / volta ao menu |

### Vidas, tempo e pontos

- Você tem **3 vidas**. Errar a senha ou deixar o tempo acabar custa 1 vida.
- Errou: o teclado trava por 3 s (o relógio continua correndo).
- Tempo acabou: a senha certa é revelada e uma nova rodada começa.
- Acertou: ganha `10 × nível + segundos restantes` e sobe de nível.
- A cada nível o tempo cai 5 s (de 60 s até o mínimo de 20 s) e novos
  elementos entram no sorteio (nível 1: H–Ca, até 99 no nível 5).
- O recorde fica salvo na flash e aparece na tela inicial.

Todos esses valores podem ser ajustados em
[`teclado_com_display/Config.h`](teclado_com_display/Config.h).

## Hardware

| Teclado | GPIO            |
|---------|-----------------|
| Linhas  | 13, 17, 21, 22  |
| Colunas | 25, 26, 27, 32  |

O display é o integrado do T-Display (ST7789, 135x240).

## Bibliotecas

- Keypad (Mark Stanley)
- Adafruit ST7735 and ST7789 Library + Adafruit GFX
- PubSubClient (Nick O'Leary)
- Core ESP32 (Espressif) — `Preferences` e `WiFi` já vêm com ele

## Rede (MQTT)

Segue o protocolo do Grupo 1 ([RFC 001](https://github.com/GroosL/KTNE-ESP32)).
Wi-Fi, broker e ID do módulo (`2`) ficam em `Config.h`.

| Tópico                       | Direção  | Uso                                  |
|------------------------------|----------|--------------------------------------|
| `jogo/modulo/2/status`       | publica  | `ONLINE` ao conectar, `OFFLINE` (LWT) |
| `jogo/modulo/2/evento`       | publica  | `SOLVED` / `ERROR`                   |
| `jogo/modulo/<DEST>/dica/2`  | publica  | dica direta para outro grupo         |
| `jogo/modulo/2/dica`         | publica  | dica para a Central sortear destino  |
| `jogo/estado`                | assina   | `START`, `RESET`, `VICTORY`, `DEFEAT` |
| `jogo/modulo/ativo`          | assina   | grupo da vez                         |
| `jogo/serial`                | assina   | serial da bomba (`LL-DD`)            |
| `jogo/tempo`                 | assina   | segundos restantes                   |
| `jogo/modulo/2/dica/+`       | assina   | dicas recebidas de outros grupos     |

Sem rede o jogo continua funcionando; a reconexão é tentada a cada 5 s.

## Organização do código

| Arquivo                     | Responsabilidade                              |
|-----------------------------|-----------------------------------------------|
| `teclado_com_display.ino`   | `setup`/`loop`: liga teclado, tela e jogo      |
| `Config.h`                  | regras, dificuldade, teclas e pinos            |
| `Elementos.*`               | tabela periódica (número atômico → símbolo)   |
| `Desafio.*`                 | sorteio dos elementos e cálculo da senha      |
| `Tela.*`                    | tudo que é desenhado no display               |
| `Recorde.*`                 | recorde persistido na flash                   |
| `Jogo.*`                    | máquina de estados do jogo (sem `delay`)      |
| `Rede.*`                    | Wi-Fi + MQTT com a Central                    |
