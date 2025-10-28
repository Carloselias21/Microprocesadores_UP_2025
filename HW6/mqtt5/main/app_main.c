#include <time.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include <stdlib.h> // Para rand() y srand()

// =================================================================
// 1. CONFIGURACIÓN (ACTUALIZA ESTOS VALORES)
// =================================================================


#define WIFI_SSID       "S25 Ultra de Carlos"      // << Reemplaza con tu SSID
#define WIFI_PASS       "carlos21"  // << Reemplaza con tu Contraseña

#define MQTT_BROKER_URI "mqtt://io.adafruit.com:1883"
#define MQTT_USERNAME   "Carlos321"   // << Tu usuario de Adafruit IO   
#define MQTT_PASSWORD   " "        // << Tu AIO Key

// Prefijo único: u21
#define AIO_USER        "Carlos321"   // Usado para concatenar los tópicos
#define UNIQUE_PREFIX   "u21"

// Definición de tópicos (Feeds)
#define MQTT_TOPIC_TEMP  AIO_USER "/feeds/" UNIQUE_PREFIX ".temperature"
#define MQTT_TOPIC_GPS   AIO_USER "/feeds/" UNIQUE_PREFIX ".gps"

static const char *TAG = "HW6_ESP32";

// =================================================================
// 2. DATOS DE SIMULACIÓN GPS (Ruta de Guadalajara)
// =================================================================
// Array con las coordenadas solicitadas en el formato "lat, lon"
const char *GPS_ROUTE[] = {
    // 1. Vértice Superior (Norte)
    "{\"lat\":20.741111, \"lon\":-103.400000}", 
    // 2. Conecta a Vértice Inferior Derecho (Salta una punta)
    "{\"lat\":20.742222, \"lon\":-103.400833}", 
    // 3. Conecta a Vértice Superior Izquierdo (Salta una punta)
    "{\"lat\":20.741111, \"lon\":-103.401389}", 
    // 4. Conecta a Vértice Inferior Izquierdo (Salta una punta)
    "{\"lat\":20.741944, \"lon\":-103.399722}", 
    // 5. Conecta a Vértice Superior Derecho (Salta una punta)
    "{\"lat\":20.741944, \"lon\":-103.401667}", 
    // 6. Cierra el ciclo en el Vértice Superior (de nuevo el punto 1)
    "{\"lat\":20.741111, \"lon\":-103.400000}"  
};

// Cálculo del número de elementos en el array GPS (seis puntos)
const int GPS_ROUTE_SIZE = sizeof(GPS_ROUTE) / sizeof(GPS_ROUTE[0]);

// =================================================================
// 3. TAREA DE PUBLICACIÓN
// =================================================================
static void mqtt_publish_task(void *pvParameters)
{
    esp_mqtt_client_handle_t client = (esp_mqtt_client_handle_t)pvParameters;
    int msg_id;
    char temp_payload[10];
    
    // Índice estático para recorrer las coordenadas GPS
    static int gps_index = 0; 
    
    srand(time(NULL));

    while (1)
    {
        // --- 3.a. Publicar Temperatura (Random 20-40) ---
        // Genera un flotante aleatorio entre 20.0 y 40.0
        float temperature = 20.0 + (float)rand() / ((float)RAND_MAX / 20.0);
        
        // Formatea el valor flotante
        snprintf(temp_payload, sizeof(temp_payload), "%.2f", temperature);

        // Publica la temperatura
        msg_id = esp_mqtt_client_publish(client, MQTT_TOPIC_TEMP, temp_payload, 0, 1, 0);
        ESP_LOGI(TAG, "Sent TEMP message, msg_id=%d, payload=%s", msg_id, temp_payload);

        // --- 3.b. Publicar GPS (Coordenadas secuenciales) ---
        const char *gps_payload = GPS_ROUTE[gps_index];

        // Publica la coordenada GPS
        msg_id = esp_mqtt_client_publish(client, MQTT_TOPIC_GPS, gps_payload, 0, 1, 0);
        ESP_LOGI(TAG, "Sent GPS message, msg_id=%d, payload=%s", msg_id, gps_payload);

        // Mueve el índice a la siguiente coordenada, ciclando al inicio
        gps_index = (gps_index + 1) % GPS_ROUTE_SIZE;
        
        // Evitar publicar muy rápido para no saturar Adafruit IO (máx 1 msg/s) 
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}


// El resto del código permanece igual (mqtt_event_handler, mqtt_app_start, etc.)

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            // Se elimina la subscripción de ejemplo, ya que solo necesitamos publicar en esta tarea
            // esp_mqtt_client_subscribe(client, MQTT_TOPIC, 0); 

            // Inicia la tarea de publicación al conectarse
            xTaskCreate(mqtt_publish_task, "mqtt_publish_task", 4096, client, 5, NULL);
            break;

        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("Received data on topic: %.*s\r\n", event->topic_len, event->topic);
            printf("Message: %.*s\r\n", event->data_len, event->data);
            break;

        default:
            ESP_LOGI(TAG, "Other MQTT event id:%d", event->event_id);
            break;
    }
}

void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker = {
            .address = {
                .uri = MQTT_BROKER_URI,
            },
        },
        .credentials = {
            .username = MQTT_USERNAME,
            .authentication = {
                .password = MQTT_PASSWORD,
            },
        },
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);

    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();

    } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
        ESP_LOGI(TAG, "Retrying Wi-Fi connection...");

    } else if (event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI(TAG, "Wi-Fi connected!");
        mqtt_app_start(); // Start MQTT after Wi-Fi is connected
    }
}

void wifi_init_sta(void)
{
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_t *netif = esp_netif_create_default_wifi_sta();
    assert(netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    // Register Wi-Fi event handler
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    esp_event_handler_instance_register(WIFI_EVENT,
                                        ESP_EVENT_ANY_ID,
                                        &wifi_event_handler,
                                        NULL,
                                        &instance_any_id);
    esp_event_handler_instance_register(IP_EVENT,
                                        IP_EVENT_STA_GOT_IP,
                                        &wifi_event_handler,
                                        NULL,
                                        &instance_got_ip);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();
}

void app_main(void)
{
    // Initialize NVS (Non-Volatile Storage)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated, erase and retry
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize Wi-Fi
    wifi_init_sta();
}
