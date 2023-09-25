
// --- Header file for custom GPIO functionality ---

#ifndef GENERAL_GPIO_H
#define GENERAL_GPIO_H

#include "driver/gpio.h"

// ------------- "PUBLIC" -------------

// If the last time the button was pressed is >= the debounce delay, the signal is considered stable
#define BUTTON_SIGNAL_IS_STABLE(current_time, last_press_time) (((current_time) - (last_press_time)) >= (DEBOUNCE_TIME_MS / portTICK_PERIOD_MS))

// Configures pin for basic output LED
void config_led(gpio_num_t led_pin);

// Configures pin for pushbutton input with:
//    - Pullup resistor enabled
//    - Negative edge interrupt
void config_push_button(gpio_num_t button_pin);

// ------------- "PRIVATE" -------------

#define DEBOUNCE_TIME_MS      200

#endif // #ifndef GENERAL_GPIO_H
