#include "ContadorController.h"
#include "driver/uart.h"
#include "freertos/task.h"
#include "DisplayManager.h"
#include "esp_log.h"

#include <cstdlib>
#include <cstring>
#include <cctype>

#define UART_NUM UART_NUM_0
#define BUF_SIZE 1024

static const char *TAG = "ContadorController";

void ContadorController::inicializar()
{
    configurarUART();
}

void ContadorController::iniciar()
{
    uart_flush_input(UART_NUM);

    xTaskCreatePinnedToCore(
        ContadorController::controlTask,
        "Control Task",
        4096,
        this,
        5,
        nullptr,
        0
    );
}

void ContadorController::configurarUART()
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    uart_param_config(UART_NUM, &uart_config);
    uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, nullptr, 0);
    uart_set_pin(UART_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE,
                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

void ContadorController::showMenu()
{
    uart_write_bytes(UART_NUM, "\r\n\r\n", 4);
    const char *menu =
        "++---------------------------------------++\r\n"
        "|            MENU DE OPCOES             |\r\n"
        "+---------------------------------------+\r\n"
        "| [1] Incrementa velocidade do contador |\r\n"
        "| [2] Decrementa velocidade do contador |\r\n"
        "| [3] Para o contador                   |\r\n"
        "| [4] Retorna a contagem                |\r\n"
        "| [5] Informa valor inicial da contagem |\r\n"
        "++---------------------------------------++\r\n"
        "Escolha uma opcao: ";
    uart_write_bytes(UART_NUM, menu, strlen(menu));
}

int ContadorController::readInteger()
{
    uint8_t data_char;
    char buf[16] = {0};
    int len = 0;
    TickType_t start = xTaskGetTickCount();

    while (true)
    {
        int read_len = uart_read_bytes(UART_NUM, &data_char, 1, pdMS_TO_TICKS(20));

        if (read_len > 0)
        {
            if (data_char == '\r' || data_char == '\n')
            {
                uart_write_bytes(UART_NUM, "\r\n", 2);
                if (len > 0)
                    break;
            }
            else if (isdigit(data_char) && len < (int)sizeof(buf) - 1)
            {
                buf[len++] = (char)data_char;
                uart_write_bytes(UART_NUM, (const char *)&data_char, 1);
            }
        }

        if (xTaskGetTickCount() - start > pdMS_TO_TICKS(10000))
        {
            uart_write_bytes(UART_NUM, "\r\nTempo excedido.\r\n", 21);
            return -1;
        }
    }

    return strtol(buf, nullptr, 10);
}

void ContadorController::controlTask(void *parameter)
{
    auto *self = static_cast<ContadorController*>(parameter);
    int count = 99;
    int counterSpeed = 500;
    bool isPaused = false;
    TickType_t lastTick = xTaskGetTickCount();
    uint8_t data;
    bool menuMostrado = false;

    while (true)
    {
        if (!isPaused && (xTaskGetTickCount() - lastTick >= pdMS_TO_TICKS(counterSpeed))) {
            count = (count + 1) % 100;
            DisplayManager::atualizarValor(count);
            lastTick += pdMS_TO_TICKS(counterSpeed); // evita drift
        }

        if (!menuMostrado) {
            self->showMenu();
            menuMostrado = true;
        }

        if (uart_read_bytes(UART_NUM, &data, 1, 0) > 0)
        {
            menuMostrado = false;
            switch (data)
            {
                case '1':
                    counterSpeed = (counterSpeed <= 50) ? 50 : counterSpeed - 50;
                    uart_write_bytes(UART_NUM, "\r\nVelocidade incrementada.\r\n", 28);
                    break;

                case '2':
                    counterSpeed = (counterSpeed >= 2000) ? 2000 : counterSpeed + 50;
                    uart_write_bytes(UART_NUM, "\r\nVelocidade decrementada.\r\n", 29);
                    break;

                case '3':
                    isPaused = true;
                    uart_write_bytes(UART_NUM, "\r\nContador parado.\r\n", 20);
                    break;

                case '4':
                    isPaused = false;
                    lastTick = xTaskGetTickCount();
                    uart_write_bytes(UART_NUM, "\r\nContagem retomada.\r\n", 22);
                    break;

                case '5': {
                    isPaused = true;
                    const char* msg = "\r\nDigite o valor inicial (0-99): ";
                    uart_write_bytes(UART_NUM, msg, strlen(msg));

                    int newCount = self->readInteger();
                    if (newCount >= 0 && newCount <= 99) {
                        count = newCount;
                        DisplayManager::atualizarValor(count);
                    }
                    isPaused = false;
                    lastTick = xTaskGetTickCount();
                    break;
                }
                default: ;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
