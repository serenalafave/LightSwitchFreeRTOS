
// --- Custom wifi functionality ---

// Standard C
#include <string.h>

// ESP-IDF
#include "esp_log.h"
#include "esp_wifi.h"

// Project
#include "MyWifiData.h"


// TAG for logging
static const char *TAG = "my_wifi";

// Handles wifi events
void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
   switch (event_id) {
      case WIFI_EVENT_STA_START:
         ESP_LOGI(TAG, "Station connection initiated...");
         break;

      case WIFI_EVENT_STA_CONNECTED:
         ESP_LOGI(TAG, "Station connected!");
         break;

      case WIFI_EVENT_STA_DISCONNECTED:
         ESP_LOGI(TAG, "Station disconnected!");
         break;

      case IP_EVENT_STA_GOT_IP:
         ESP_LOGI(TAG, "Station got IP.");
         break;
         
      default:
         ESP_LOGI(TAG, "Other event. ID:%d", (int)event_id);
         break;
   }
}

// Initiates wifi connection
void wifi_connect_as_station(void) {
   
   // Init wifi
   esp_netif_init();
   esp_event_loop_create_default();
   esp_netif_create_default_wifi_sta();
   
   wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
   esp_wifi_init(&wifi_initiation);
   
   // Configure wifi
   esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
   esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
   
   // Set SSID and password
   wifi_config_t wifi_configuration = {};
   strcpy((char *)wifi_configuration.sta.ssid, SSID);
   strcpy((char *)wifi_configuration.sta.password, PASSWORD);

   esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);

   // Set mode to station
   esp_wifi_set_mode(WIFI_MODE_STA);
   
   // Start and connect
   esp_wifi_start();
   esp_wifi_connect();
}
