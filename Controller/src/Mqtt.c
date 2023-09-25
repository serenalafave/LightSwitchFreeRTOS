
// --- Custom MQTT functionality ---

// ESP-IDF
#include "esp_log.h"
#include "mqtt_client.h"

// Project
#include "Config.h"


// TAG for logging
const char *TAG = "my_mqtt";

const char *remote_switch_topic = "remote_switch_topic";
const char *remote_switch_msg = "ToggleLED";
const char *broker_uri = "mqtt://mqtt.eclipseprojects.io";

// Global MQTT client handle
esp_mqtt_client_handle_t client;

// Handles MQTT events
void mqtt_event_handler(void *task_handle, esp_event_base_t base, int32_t event_id, void *event_data) {
   
   esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;

   TaskHandle_t task = (TaskHandle_t)task_handle;
   
   switch (event_id) {
      case MQTT_EVENT_CONNECTED:
         ESP_LOGI(TAG, "Connected.");
         break;

      case MQTT_EVENT_DISCONNECTED:
         ESP_LOGI(TAG, "Disconnected!");
         break;

      case MQTT_EVENT_SUBSCRIBED:
         ESP_LOGI(TAG, "Subscribed, msg_id=%d", event->msg_id);
         break;

      case MQTT_EVENT_UNSUBSCRIBED:
         ESP_LOGI(TAG, "Unsubscribed, msg_id=%d", event->msg_id);
         break;

      case MQTT_EVENT_PUBLISHED:
         ESP_LOGI(TAG, "Published, msg_id=%d", event->msg_id);
         break;

      case MQTT_EVENT_DATA:
     
      #if RECEIVER
         // Resume led control task
         if (task) {
            vTaskResume(task);
         }
         else {
            ESP_LOGE(TAG, "NO TASK PASSED TO EVENT DATA");
         }
      #endif

         ESP_LOGI(TAG, "Incoming data");
         printf("\n   TOPIC = %.*s\n   DATA = %.*s\n", event->topic_len, event->topic, event->data_len, event->data);
         break;

      case MQTT_EVENT_ERROR:
         ESP_LOGE(TAG, "ERROR!");
         break;
         
      default:
         ESP_LOGI(TAG, "Other event, ID:%d", (int)event_id);
         break;
   }
}

// Initiates MQTT connection
void mqtt_app_start(TaskHandle_t task_handle) {
   esp_mqtt_client_config_t mqtt_cfg = {};
   mqtt_cfg.broker.address.uri = broker_uri;

   client = esp_mqtt_client_init(&mqtt_cfg);
   esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, task_handle);
   esp_mqtt_client_start(client);
}

#if CONTROLLER

// Publishes message to MQTT topic that receiver is subscribed to
void send_remote_msg(void) {
   esp_mqtt_client_publish(client, remote_switch_topic, remote_switch_msg, 0, 1, 0);
}

#elif RECEIVER

// Subscribes to MQTT topic that controller publishes to
void connect_to_controller(void) {
   esp_mqtt_client_subscribe(client, remote_switch_topic, 0);
}

#endif // #if CONTROLLER #elif RECEIVER
