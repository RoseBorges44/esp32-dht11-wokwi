# ESP32-S3 + DHT11 - Leitura de Temperatura e Umidade

Projeto da disciplina de IA Embarcada (Pós-graduação).

## Descrição

Aplicação embarcada que realiza a leitura de temperatura e umidade utilizando o sensor DHT11 conectado ao ESP32-S3, simulado no Wokwi.

## Circuito

| ESP32-S3 | DHT11 |
|----------|-------|
| 3V3      | VCC   |
| GND      | GND   |
| GPIO 4   | DATA  |

## Como executar

1. Abrir o projeto no VS Code com a extensão ESP-IDF instalada
2. Definir o target: `ESP-IDF: Set Espressif Device Target` -> ESP32-S3
3. Compilar: `ESP-IDF: Build your Project`
4. Simular: `Wokwi: Start Simulator`

## Resultado esperado

O monitor serial exibe leituras de temperatura e umidade a cada 2 segundos:

```
I (XXX) DHT11: === Sensor DHT11 com ESP32-S3 ===
I (XXX) DHT11: Iniciando leitura do sensor...
I (XXX) DHT11: Temperatura: 24.0 C | Umidade: 65.0 %
```
