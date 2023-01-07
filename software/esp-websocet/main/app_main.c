#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "esp_system.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"

#define WEB_SERVER	"192.168.0.13"
#define WEB_PORT	8080

#define WIFI_SSID	"2.4G-Vectra-WiFi-744724"
#define WIFI_PASS	"cx3g0skljckkegc8"

static const char *TAG = "esp_tcp_socket";

typedef struct {
	float x, y, z;
} vector3f_t;

typedef struct {
	vector3f_t attitude;
	float throttle;

	uint8_t cs;
} controls_t;

typedef struct {
	struct {
		vector3f_t accel;
		vector3f_t gyro;
		vector3f_t mag;
		float baro;
		float temp[3];
		float current;
		float voltage;
	} readings;
	
	struct {
		vector3f_t attitude;
		float altitude;
	} estiamtes;

	struct {
		float throttle;
		float fins[4];
	} steering;

	uint8_t cs;
} telemetry_t;

void send_task();

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id) {
        case SYSTEM_EVENT_STA_START:
            ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");
            ESP_ERROR_CHECK(esp_wifi_connect());
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
            ESP_LOGI(TAG, "Got IP: '%s'", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
			xTaskCreate(send_task, "send_task", 8192, NULL, 5, NULL);
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED");
            ESP_ERROR_CHECK(esp_wifi_connect());
            break;
        default:
            break;
    }
    return ESP_OK;
}

static void initialise_wifi(void)
{
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void send_task(void *pvParameters) {
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(WEB_SERVER);
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(WEB_PORT);

	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
		vTaskDelete(NULL);
		return;
	}
	ESP_LOGI(TAG, "Socket created");

	int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
	if (err != 0) {
		ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
		vTaskDelete(NULL);
		return;
	}
	ESP_LOGI(TAG, "Successfully connected");

	while (1) {
		// Generate random float between 0 and 1
		float rand_float = (float)rand() / (float)RAND_MAX;

		int err = send(sock, &rand_float, sizeof(float), 0);
		if (err < 0) {
			ESP_LOGE(TAG, "Error occured during sending: errno %d", errno);
			break;
		}
		ESP_LOGI(TAG, "Message sent: %f", rand_float);

		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}

	if (sock != -1) {
		ESP_LOGE(TAG, "Shutting down socket and restarting...");
		shutdown(sock, 0);
		close(sock);
	}

	vTaskDelete(NULL);
}

static void blink(void *param) {
	(void)param;

	gpio_config_t led = {
		.pin_bit_mask = GPIO_Pin_2,
		.mode = GPIO_MODE_OUTPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_DISABLE
	};

	gpio_config(&led);

	while(1) {

		gpio_set_level(GPIO_NUM_2, 1);
		vTaskDelay(100);

		gpio_set_level(GPIO_NUM_2, 0);
		vTaskDelay(100);
	}
}

void app_main() {

	ESP_ERROR_CHECK(nvs_flash_init());

	initialise_wifi();
	
	xTaskCreate(blink, "blink", 3000, NULL, 5, NULL);
}
