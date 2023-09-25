
// --- LIGHT SWITCH CONTROLLER ---

// This program works as a light switch controller. It has two connected
// pushbuttons; the first ("lamp button") toggles a local LED, and the second
// ("remote button") sends a remote msg, using wifi and MQTT, to a receiver to
// toggle its LED.

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

#define LED_PIN               GPIO_NUM_7

#define LAMP_BUTTON_PIN       GPIO_NUM_20
#define REMOTE_BUTTON_PIN     GPIO_NUM_22

#define WIFI_WAIT_TIME_MS     15000
#define MQTT_WAIT_TIME_MS     5000

#define ESP_INTR_FLAG_DEFAULT 0

TaskHandle_t lamp_switch_task_handle;
TaskHandle_t remote_switch_task_handle;

volatile TickType_t lamp_last_button_press_time = 0;
volatile TickType_t remote_last_button_press_time = 0;

// ISR for lamp button press. Resumes the lamp switch task, which triggers the
// lamp (local) led to toggle.
void IRAM_ATTR lamp_button_isr_handler(void *arg) {
   TickType_t current_time = xTaskGetTickCountFromISR();

   // If button signal is stable
   if (BUTTON_SIGNAL_IS_STABLE(current_time, lamp_last_button_press_time)) {
      lamp_last_button_press_time = current_time;
      xTaskResumeFromISR(lamp_switch_task_handle);
   }
}

// ISR for remote switch button press. Resumes the remote switch task, which
// triggers an led toggle signal to be sent to the receiver.
void IRAM_ATTR remote_button_isr_handler(void *arg) {
   TickType_t current_time = xTaskGetTickCountFromISR();

   // If button signal is stable (last button press time meets debounce threshold)
   if (BUTTON_SIGNAL_IS_STABLE(current_time, remote_last_button_press_time)) {
      remote_last_button_press_time = current_time;
      xTaskResumeFromISR(remote_switch_task_handle);
   }
}

// Lamp (local) LED control task that triggers (is moved from suspend to resume
// for one loop) when the lamp button ISR is called. Toggles the lamp LED.
void lamp_switch_task(void *arg) {
   bool ledState = false;

   while (pdTRUE) {
      // Suspend until lamp button is pressed, pushing task into resume state
      vTaskSuspend(lamp_switch_task_handle);
      
      // Toggle LED once
      ledState = !ledState;
      gpio_set_level(LED_PIN, ledState);
   }
}

// Remote switch control task that triggers (is moved from suspend to resume
// for one loop) when the remote switch button ISR is called. Sends a msg to
// the receiver, signaling for its LED to be toggled.
void remote_switch_task(void *arg) {
   while(pdTRUE) {
      // Suspend until remote button is pressed, pushing task into resume state
      vTaskSuspend(remote_switch_task_handle);

      // Send msg to trigger LED toggle on receiver
      send_remote_msg();
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

   // Config GPIOs
   config_led(LED_PIN);
   config_push_button(LAMP_BUTTON_PIN);
   config_push_button(REMOTE_BUTTON_PIN);

   // Start MQTT connection
   mqtt_app_start(NULL);
   vTaskDelay(MQTT_WAIT_TIME_MS / portTICK_PERIOD_MS);
   printf("\nMQTT probably ready\n");

   // Enable GPIO ISRs
   gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

   // Add ISR to push buttons
   gpio_isr_handler_add(LAMP_BUTTON_PIN, lamp_button_isr_handler, NULL);
   gpio_isr_handler_add(REMOTE_BUTTON_PIN, remote_button_isr_handler, NULL);

   // Create light switch tasks
   xTaskCreate(lamp_switch_task, "lamp_switch_task", 2048, NULL, 10, &lamp_switch_task_handle);
   xTaskCreate(remote_switch_task, "remote_switch_task", 2048, NULL, 10, &remote_switch_task_handle);
}
