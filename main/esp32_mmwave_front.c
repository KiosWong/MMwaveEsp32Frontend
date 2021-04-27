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


#define BLINK_GPIO  GPIO_NUM_4

int udp_sock;

TimerHandle_t xTimerUser; // 定义句柄

static void vAdcTimerCallBack();

void app_main() {

	//硬件延时
	vTaskDelay(10000 / portTICK_PERIOD_MS);

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
    // 申请定时器， 配置
    xTimerUser = xTimerCreate
                   /*调试用， 系统不用*/
                   ("Timer's name",
                   /*定时溢出周期， 单位是任务节拍数*/
                   10,   
                   /*是否自动重载， 此处设置周期性执行*/
                   pdTRUE,
                   /*记录定时器溢出次数， 初始化零, 用户自己设置*/
                  ( void * ) 0,
                   /*回调函数*/
                  vAdcTimerCallBack);

     if( xTimerUser != NULL ) {
        // 启动定时器， 0 表示不阻塞
        xTimerStart( xTimerUser, 0 );
    }

	tcpip_adapter_init();
	wifi_init_softap();
	udp_sock = udp_init_socket();
    vTaskDelay(1000 / portTICK_PERIOD_MS);

	while(1) {

	}
}


void vAdcTimerCallBack()
{
	int adc_val = getadc();
	unsigned char udp_send_buf[4] = {0};
	printf("adc value:%d\r\n", adc_val);
	udp_send_buf[3] = adc_val&0x000000ff;
	udp_send_buf[2] = (adc_val&0x0000ff00) >> 8;
	udp_send_buf[1] = (adc_val&0x00ff0000) >> 16;
	udp_send_buf[0] = (adc_val&0xff000000) >> 24;
	udp_send_packet(udp_sock, (const char *)udp_send_buf, 4);
}