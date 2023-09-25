
// --- Custom GPIO functionality ---

#include "GeneralGPIO.h"

// Configures pin for basic output LED
void config_led(gpio_num_t led_pin) {
   gpio_config_t ioConfig = {};

   ioConfig.intr_type = GPIO_INTR_DISABLE;
   ioConfig.mode = GPIO_MODE_OUTPUT;
   ioConfig.pin_bit_mask = 1ULL << led_pin;
   ioConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
   ioConfig.pull_up_en = GPIO_PULLUP_DISABLE;
   
   gpio_config(&ioConfig);
}

// Configures pin for pushbutton input with:
//    - Pullup resistor enabled
//    - Negative edge interrupt
void config_push_button(gpio_num_t button_pin) {
   gpio_config_t ioConfig = {};

   ioConfig.intr_type = GPIO_INTR_NEGEDGE;
   ioConfig.mode = GPIO_MODE_INPUT;
   ioConfig.pin_bit_mask = 1ULL << button_pin;
   ioConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
   ioConfig.pull_up_en = GPIO_PULLUP_ENABLE;
   
   gpio_config(&ioConfig);
}
