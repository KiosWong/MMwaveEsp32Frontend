/*Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "freertos/portable.h"

#include "driver/uart.h"

#include "esp32/rom/ets_sys.h"
#include "driver/spi_slave.h"
#include "esp_heap_caps.h"
#include "esp_heap_caps_init.h"

#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_wifi.h"

#include "esp_event_loop.h"
#include "nvs_flash.h"
#include <stdlib.h>

#include "driver/gpio.h"

#include "adc.h"
#include "wifi.h"
#include "udp.h"
#include "bmd101.h"


#define BLINK_GPIO  GPIO_NUM_4

volatile int upload_flag;
signed int bmd101_upload_value = 0;

SemaphoreHandle_t xSemaphoreUpload;
TimerHandle_t xTimerUser; // 定义句柄

static void vAdcTimerCallBack();
static void bmd101_uart_recv_task(void *arg);
static void upload_task(void *arg);

void app_main() {

	//硬件延时
	vTaskDelay(10);
	xSemaphoreUpload = xSemaphoreCreateBinary();

	gpio_pad_select_gpio(GPIO_NUM_12);//Modoul_NRST
	/* Set the GPIO as a push/pull output */
	gpio_set_direction(GPIO_NUM_12, GPIO_MODE_OUTPUT);
	gpio_set_level(GPIO_NUM_12, 1);

	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	adc_init();
	bmd101_ble_uart_init();

	tcpip_adapter_init();
	wifi_init_softap();
	udp_sock = udp_init_socket();

	xTaskCreate(bmd101_uart_recv_task, "bmd101_uart_recv_task", 2048, NULL, 5, NULL);
	// xTaskCreate(upload_task, "upload_task", 2048, NULL, 10, NULL);
}

static void bmd101_uart_recv_task(void *arg)
{
	uint8_t *data = (uint8_t *) malloc(BM101_BUF_SIZE);
    while (1) {
        // Read data from the UART
        int len = uart_read_bytes(UART_NUM_1, data, BM101_BUF_SIZE, 5);
		if(len > 0) {
			bmd101_parse_packet((const char *)data, len);
		}
		
		vTaskDelay(1);
    }
}
