#include <string.h>
#include <errno.h>
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_event_loop.h"
#include "wifi.h"

/* FreeRTOS event group to signal when we are connected to WiFi and ready to start UDP test*/
EventGroupHandle_t udp_event_group;

tcpip_adapter_ip_info_t local_ip;

static esp_err_t event_handler(void *ctx, system_event_t *event) {
	switch (event->event_id) {
	case SYSTEM_EVENT_STA_START:
		esp_wifi_connect();
		break;
	case SYSTEM_EVENT_STA_DISCONNECTED:
		esp_wifi_connect();
		xEventGroupClearBits(udp_event_group, WIFI_CONNECTED_BIT);
		break;
	case SYSTEM_EVENT_STA_CONNECTED:
		break;
	case SYSTEM_EVENT_STA_GOT_IP:
		ESP_LOGI(TAG, "event_handler:SYSTEM_EVENT_STA_GOT_IP!");
		ESP_LOGI(TAG, "got ip:%s\n",
				ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
		xEventGroupSetBits(udp_event_group, WIFI_CONNECTED_BIT);
		break;
	case SYSTEM_EVENT_AP_STACONNECTED:
		ESP_LOGI(TAG, "station:"MACSTR" join,AID=%d\n",
				MAC2STR(event->event_info.sta_connected.mac),
				event->event_info.sta_connected.aid);
		xEventGroupSetBits(udp_event_group, WIFI_CONNECTED_BIT);
		break;
	case SYSTEM_EVENT_AP_STADISCONNECTED:
		ESP_LOGI(TAG, "station:"MACSTR"leave,AID=%d\n",
				MAC2STR(event->event_info.sta_disconnected.mac),
				event->event_info.sta_disconnected.aid);
		xEventGroupSetBits(udp_event_group, UDP_CONNCETED_SUCCESS);
		xEventGroupClearBits(udp_event_group, WIFI_CONNECTED_BIT);
		break;
	default:
		break;
	}
	return ESP_OK;
}


void wifi_init_softap() 
{
    esp_err_t res_ap_set;
	udp_event_group = xEventGroupCreate();

	tcpip_adapter_init();
	ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	wifi_config_t wifi_config = { .ap = { .ssid = AP_SSID, .ssid_len = 0,
			.max_connection = EXAMPLE_MAX_STA_CONN, .password = AP_PAW,
			.authmode = WIFI_AUTH_WPA_WPA2_PSK }, };
	if (strlen(AP_SSID) == 0) {
		wifi_config.ap.authmode = WIFI_AUTH_OPEN;
	}

    /*write */
    IP4_ADDR(&local_ip.ip, 192, 168 , 0, 1);
    IP4_ADDR(&local_ip.gw, 192, 168 , 0, 1);
    IP4_ADDR(&local_ip.netmask, 255, 255 , 255, 0);

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());

	ESP_LOGI(TAG, " Wifi_init_softap finished the SSID: %s password:%s \n",
	AP_SSID, AP_PAW);
    
    tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP); 
    res_ap_set = tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &local_ip);
    if(res_ap_set == ESP_OK)
        ESP_LOGI(TAG,"set ip success\n");
    else
        ESP_LOGI(TAG,"set ip failed\n");

    /*restart adapter dhcps*/
    tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP);

}

//wifi初始化，连接路由器
void wifi_init_sta() 
{
    udp_event_group = xEventGroupCreate();
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t wifi_config = { 
    .sta = { .ssid = GATEWAY_SSID, 
     .password = GATEWAY_PASSWORD } };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");
    ESP_LOGI(TAG, "connect to ap SSID:%s password:%s \n",
    GATEWAY_SSID, GATEWAY_PASSWORD);
}