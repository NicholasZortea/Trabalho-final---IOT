# Trabalho final IoT — Senha Química (standalone)

Jogo de desarmar uma "bomba" descobrindo uma senha de 4 dígitos a partir de
símbolos da tabela periódica. Roda sozinho em um **TTGO T-Display (ESP32)**
com um **teclado matricial 4x4**, sem Wi-Fi nem MQTT.

## Como jogar

1. Na tela inicial, aperte `#`.
2. O display mostra símbolos de elementos químicos, por exemplo `Ne C B`.
3. A senha é a junção, em ordem, dos **números atômicos**:
   `Ne C B` → `10` `6` `5` → **`1065`**.
4. Olhe o **serial (SN)** no rodapé. Se o **último dígito for ímpar**, some
   **2 ao primeiro dígito** da senha, sem "vai um" (`9 + 2 = 1`).
   Ex.: `SN: K7QA3` → `1065` vira **`3065`**.
5. Digite os 4 dígitos. A senha é conferida automaticamente.

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
- Core ESP32 (Espressif) — `Preferences` já vem com ele

## Organização do código

| Arquivo                     | Responsabilidade                              |
|-----------------------------|-----------------------------------------------|
| `teclado_com_display.ino`   | `setup`/`loop`: liga teclado, tela e jogo      |
| `Config.h`                  | regras, dificuldade, teclas e pinos            |
| `Elementos.*`               | tabela periódica (número atômico → símbolo)   |
| `Desafio.*`                 | sorteio dos elementos/serial e cálculo da senha |
| `Tela.*`                    | tudo que é desenhado no display               |
| `Recorde.*`                 | recorde persistido na flash                   |
| `Jogo.*`                    | máquina de estados do jogo (sem `delay`)      |
