#ifndef DISPLAY_CONFIG_H
#define DISPLAY_CONFIG_H

#include "driver/gpio.h"

// Decodificador CD4511
constexpr gpio_num_t DECODER_D0 = GPIO_NUM_18;
constexpr gpio_num_t DECODER_D1 = GPIO_NUM_19;
constexpr gpio_num_t DECODER_D2 = GPIO_NUM_5;
constexpr gpio_num_t DECODER_D3 = GPIO_NUM_17;

// Transistores (seleção de displays)
constexpr gpio_num_t DISPLAY_1_ENABLE = GPIO_NUM_23;
constexpr gpio_num_t DISPLAY_2_ENABLE = GPIO_NUM_22;

#endif
