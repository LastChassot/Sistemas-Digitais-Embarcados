#ifndef CONTADOR_CONTROLLER_H
#define CONTADOR_CONTROLLER_H

#include "freertos/FreeRTOS.h"

class ContadorController {
public:
    static void inicializar();
    void iniciar(); // cria a task principal
    static void controlTask(void *parameter);

private:
    static void configurarUART();
    static void showMenu();
    static int readInteger();
};

#endif
