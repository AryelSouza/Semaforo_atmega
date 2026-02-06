# 🚦 Projeto: Semáforo com Interrupção – ATmega328P (SimulIDE)
## 📌 Descrição geral

Este projeto implementa um semáforo veicular com sinalização para pedestres, desenvolvido no Microchip Studio e simulado no SimulIDE, utilizando o microcontrolador ATmega328P.

### O sistema simula um cenário real de trânsito urbano, incluindo:

Semáforo veicular (verde, amarelo e vermelho)

Semáforo de pedestre

Botão de solicitação de travessia

Efeito de fade (PWM) no sinal amarelo

Pisca do pedestre nos últimos segundos

Aviso sonoro com buzzer antes da abertura do sinal

### O projeto utiliza:

GPIO

Timers

Controlador de interrupções

PWM

## 🧠 Funcionalidades
### 🚗 Semáforo veicular

Verde: fluxo normal de veículos

Amarelo: transição com efeito de fade (PWM)

Vermelho: veículos parados

### 🚶 Semáforo de pedestre

Acende junto com o vermelho veicular

Permanece aceso durante a travessia

Pisca nos últimos 2 segundos, indicando o fim do tempo

### 🔘 Botão de pedestre

Conectado a uma interrupção externa (INT0)

Permite solicitar a travessia

Só é considerado quando o semáforo está verde

### 🔊 Buzzer

Ativado durante o piscar final do pedestre

Simula aviso sonoro de abertura/fechamento do sinal

### ⚙️ Periféricos utilizados
GPIO

Controle de LEDs (verde, amarelo, vermelho e pedestre)

Controle do buzzer

Leitura do botão

Timers

Timer0:

Geração de PWM para o LED amarelo (fade de intensidade)

Timer1:

Base de tempo de 1 segundo

Controle das durações dos estados do semáforo

Interrupções

INT0 (PD2):

Botão de pedestre

Evita polling e melhora resposta do sistema

Interrupção do Timer1:

Contagem precisa de segundos

## 🔌 Componentes utilizados

ATmega328P

LEDs:

Verde (veicular)

Amarelo (veicular – PWM)

Vermelho (veicular)

Pedestre

Resistores para LEDs

Botão de pressão

Buzzer


SimulIDE

## 🔗 Mapeamento de pinos
Função	Pino ATmega328P
LED Verde	PB0
LED Vermelho	PB7
LED Amarelo (PWM)	PD6 (OC0A)
LED Pedestre	PB3
Buzzer	PB2
Botão de pedestre	PD2 (INT0)
VCC	VCC
GND	GND
## 🧩 Lógica de funcionamento

O sistema inicia com o sinal verde aceso

Ao pressionar o botão:

O pedido é registrado por interrupção

O sinal verde apaga

O amarelo acende com fade (PWM crescente)

Ao atingir brilho máximo:

Amarelo apaga

Vermelho acende

Pedestre acende

Nos últimos 2 segundos:

LED do pedestre enfraquece

Buzzer emite aviso sonoro

O sistema retorna ao estado inicial (verde)

Todo o controle é feito por uma máquina de estados, garantindo clareza, organização e comportamento previsível.
