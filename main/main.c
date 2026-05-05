#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "rom/ets_sys.h"

#define DHT11_PIN GPIO_NUM_4

static const char *TAG = "DHT11";

typedef struct {
    float temperatura;
    float umidade;
    bool valido;
} dht11_leitura_t;

static void dht11_enviar_start(void)
{
    gpio_set_direction(DHT11_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(DHT11_PIN, 0);
    ets_delay_us(20000); // 20ms low
    gpio_set_level(DHT11_PIN, 1);
    ets_delay_us(40);    // 40us high
    gpio_set_direction(DHT11_PIN, GPIO_MODE_INPUT);
}

static int dht11_esperar_nivel(int nivel, int timeout_us)
{
    int contador = 0;
    while (gpio_get_level(DHT11_PIN) == nivel) {
        if (contador >= timeout_us) {
            return -1;
        }
        ets_delay_us(1);
        contador++;
    }
    return contador;
}

static dht11_leitura_t dht11_ler(void)
{
    dht11_leitura_t resultado = { .valido = false };
    uint8_t dados[5] = {0};

    dht11_enviar_start();

    // Esperar resposta do sensor: 80us LOW + 80us HIGH
    if (dht11_esperar_nivel(0, 100) < 0) return resultado;
    if (dht11_esperar_nivel(1, 100) < 0) return resultado;

    // Ler 40 bits (5 bytes)
    for (int i = 0; i < 40; i++) {
        // Cada bit comeca com ~50us LOW
        if (dht11_esperar_nivel(0, 70) < 0) return resultado;

        // Duracao do HIGH determina se eh 0 ou 1
        int duracao = dht11_esperar_nivel(1, 100);
        if (duracao < 0) return resultado;

        // Se HIGH > 40us, eh bit 1
        if (duracao > 40) {
            dados[i / 8] |= (1 << (7 - (i % 8)));
        }
    }

    // Verificar checksum
    uint8_t checksum = dados[0] + dados[1] + dados[2] + dados[3];
    if (checksum != dados[4]) {
        ESP_LOGW(TAG, "Checksum invalido: esperado %d, recebido %d", checksum, dados[4]);
        return resultado;
    }

    resultado.umidade = (float)dados[0] + (float)dados[1] * 0.1f;
    resultado.temperatura = (float)dados[2] + (float)dados[3] * 0.1f;
    resultado.valido = true;

    return resultado;
}

void app_main(void)
{
    ESP_LOGI(TAG, "=== Sensor DHT11 com ESP32-S3 ===");
    ESP_LOGI(TAG, "Iniciando leitura do sensor...");

    // Configurar GPIO com pull-up
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << DHT11_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLUP_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);

    while (1) {
        dht11_leitura_t leitura = dht11_ler();

        if (leitura.valido) {
            ESP_LOGI(TAG, "Temperatura: %.1f C | Umidade: %.1f %%",
                     leitura.temperatura, leitura.umidade);
        } else {
            ESP_LOGW(TAG, "Falha na leitura do sensor");
        }

        vTaskDelay(pdMS_TO_TICKS(2000)); // Ler a cada 2 segundos
    }
}
