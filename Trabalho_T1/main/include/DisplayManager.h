#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"

class DisplayManager {
public:
    static void inicializar();
    static void iniciar();
    static void atualizarValor(int novoValor);

private:
    static void displayTask(void *parameter);
    static void writeToDecoder(int number);

    static volatile int32_t valorExibido;
    static SemaphoreHandle_t valorMutex;
};

#endif
