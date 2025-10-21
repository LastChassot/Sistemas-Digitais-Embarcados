#include "DisplayManager.h"
#include "ContadorController.h"

extern "C" void app_main(void)
{
    DisplayManager::inicializar();

    ContadorController contador;
    contador.inicializar();

    DisplayManager::iniciar();
    contador.iniciar();
}
