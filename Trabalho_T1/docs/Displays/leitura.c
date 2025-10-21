#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"

#define UART_PORT_NUM      UART_NUM_0
#define BUF_SIZE           1024


#define LOW 0
#define HIGH 1


// Para ler apenas um caracter
int leiaB()
{
    int c;
    uart_read_bytes(UART_NUM_0, &c, 1, portMAX_DELAY);
    return c;
}
// Para ler uma string
void leiaString (char *st)
{
    int x=0;
    char c;

    while (1) {

        const int rxBytes = uart_read_bytes(UART_NUM_0, &c, 1, 100);
        if (rxBytes > 0) {
            if (c==13)
            {
                st[x]=0;
                            return;

            }
            else
            {
                uart_write_bytes(UART_NUM_0, &c, 1);
                st[x] = c;
                x++;
            }
        }
    }
}
//Para ler um numero
int leiaInteiro (void)
{
    int x=0;
    char c;
    char st[21];
    int valor;

    while (1) {

        const int rxBytes = uart_read_bytes(UART_NUM_0, &c, 1, 20);
        if (rxBytes > 0) {
            if (c==13)
            {
                st[x]=0;
                sscanf(st,"%d",&valor);
                return (valor);

            }
            else
            {
                uart_write_bytes(UART_NUM_0, &c, 1);

                st[x] = c;
                x++;
            }
        }
    }
}



void app_main() {

    // Configura a porta serial
    const uart_config_t uart_config = {
           .baud_rate = 115200,
           .data_bits = UART_DATA_8_BITS,
           .parity    = UART_PARITY_DISABLE,
           .stop_bits = UART_STOP_BITS_1,
           .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_driver_install(UART_PORT_NUM, BUF_SIZE, 0, 0, NULL, 0);
    uart_param_config(UART_PORT_NUM, &uart_config);
    // ----------------------------------------------------------



}
