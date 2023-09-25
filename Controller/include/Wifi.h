
// --- Header file for custom wifi functionality ---

#ifndef WIFI_H
#define WIFI_H

// ------------- "PUBLIC" -------------

// Initiates wifi connection
void wifi_connect_as_station(void);

// ------------- "PRIVATE" -------------

// Handles wifi events
void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

#endif // #ifndef WIFI_H
