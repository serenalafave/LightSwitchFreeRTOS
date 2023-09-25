
// --- REMOTE LIGHT SWITCH RECEIVER ---
// This program works as a remote light switch receiver. Its connected LED can
// be toggled using a remote pushbutton or from an html page using wifi and
// MQTT communication.

// FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// ESP-IDF
#include "esp_event.h"
#include "nvs_flash.h"

// Project
#include "Wifi.h"
#include "Mqtt.h"
#include "GeneralGPIO.h"

#define LED_PIN            GPIO_NUM_5
#define WIFI_WAIT_TIME_MS  15000
#define MQTT_WAIT_TIME_MS  5000

TaskHandle_t receive_remote_msg_task_handle;

// LED control task that triggers (is moved from suspend to resume for one loop)
// when a remote msg is received from the controller. Toggles the LED.
void receive_remote_msg_task(void *arg) {
   bool led_state = false;

   while(pdTRUE) {
      // Suspend until remote msg pushes task into resume state
      vTaskSuspend(receive_remote_msg_task_handle);

      // Toggle LED once
      led_state = !led_state;
      gpio_set_level(LED_PIN, led_state);
   }
}

void app_main(void) {
   
   // Init flash
   esp_err_t flashStatus = nvs_flash_init();

   if (flashStatus == ESP_ERR_NVS_NO_FREE_PAGES || flashStatus == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      flashStatus = nvs_flash_init();
   }

   ESP_ERROR_CHECK(flashStatus);

   // Start wifi connection
   wifi_connect_as_station();
   vTaskDelay(WIFI_WAIT_TIME_MS / portTICK_PERIOD_MS);
   printf("\nWifi probably ready\n");

   // Config GPIO
   config_led(LED_PIN);

   // Create receive task
   xTaskCreate(receive_remote_msg_task, "receive_remote_msg_task", 2048, NULL, 10, &receive_remote_msg_task_handle);

   // Start MQTT connection
   mqtt_app_start(receive_remote_msg_task_handle);
   vTaskDelay(MQTT_WAIT_TIME_MS / portTICK_PERIOD_MS);
   printf("\nMQTT probably ready\n");

   // Connect to controlling device
   connect_to_controller();
}
