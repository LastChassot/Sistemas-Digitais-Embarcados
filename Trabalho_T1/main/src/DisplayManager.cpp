#include "DisplayManager.h"
#include "DisplayConfig.h"
#include "driver/gpio.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp32/rom/ets_sys.h"

volatile int32_t DisplayManager::valorExibido = 99;
SemaphoreHandle_t DisplayManager::valorMutex = nullptr;

static const gpio_num_t decoderPins[] = {DECODER_D0, DECODER_D1, DECODER_D2, DECODER_D3};
static const gpio_num_t displayEnablePins[] = {DISPLAY_1_ENABLE, DISPLAY_2_ENABLE};
static constexpr int NUM_DISPLAYS = 2;
static constexpr TickType_t DISPLAY_DELAY = pdMS_TO_TICKS(5);

void DisplayManager::inicializar()
{
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;

    io_conf.pin_bit_mask = ((1ULL << DECODER_D0) | (1ULL << DECODER_D1) |
                            (1ULL << DECODER_D2) | (1ULL << DECODER_D3));
    gpio_config(&io_conf);

    io_conf.pin_bit_mask = ((1ULL << DISPLAY_1_ENABLE) | (1ULL << DISPLAY_2_ENABLE));
    gpio_config(&io_conf);

    gpio_set_level(DISPLAY_1_ENABLE, 0);
    gpio_set_level(DISPLAY_2_ENABLE, 0);

    valorMutex = xSemaphoreCreateMutex();
    configASSERT(valorMutex != nullptr);
}

void DisplayManager::iniciar()
{
    xTaskCreatePinnedToCore(
        displayTask,
        "Display Task",
        2048,
        nullptr,
        5,
        nullptr,
        1
    );
}

void DisplayManager::atualizarValor(int novoValor)
{
    if (xSemaphoreTake(valorMutex, portMAX_DELAY) == pdTRUE) {
        valorExibido = novoValor;
        xSemaphoreGive(valorMutex);
    }
}

void DisplayManager::writeToDecoder(int number)
{
    for (int i = 0; i < 4; i++) {
        gpio_set_level(decoderPins[i], (number >> i) & 1);
    }
    ets_delay_us(2);
}

void DisplayManager::displayTask(void *parameter)
{
    while (true) {
        int valorLocal = 0;

        if (xSemaphoreTake(valorMutex, portMAX_DELAY) == pdTRUE) {
            valorLocal = valorExibido;
            xSemaphoreGive(valorMutex);
        }

        int digits[2] = {(valorLocal / 10) % 10, valorLocal % 10};

        for (int i = 0; i < NUM_DISPLAYS; i++) {
            for (auto pin : displayEnablePins)
                gpio_set_level(pin, 0);

            writeToDecoder(digits[i]);
            gpio_set_level(displayEnablePins[i], 1);

            vTaskDelay(DISPLAY_DELAY);
        }
    }
}
