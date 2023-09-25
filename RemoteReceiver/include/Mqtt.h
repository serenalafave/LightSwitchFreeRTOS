
// --- Header file for custom MQTT functionality ---

#ifndef MQTT_H
#define MQTT_H

#include "Config.h"

// ------------- "PUBLIC" -------------

// Initiates MQTT connection
void mqtt_app_start(TaskHandle_t task_handle);

#if CONTROLLER

// Publishes a message to MQTT topic that receiver is subscribed to
void send_remote_msg(void);

#elif RECEIVER

// Subscribes to MQTT topic that controller publishes to
void connect_to_controller(void);

#endif // #if CONTROLLER #elif RECEIVER

// ------------- "PRIVATE" -------------

// Handles MQTT events
void mqtt_event_handler(void *task_handle, esp_event_base_t base, int32_t event_id, void *event_data);

#endif // #ifndef MQTT_H
