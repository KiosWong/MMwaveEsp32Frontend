#ifndef __WIFI_H
#define __WIFI_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

//路由器的名字
#define GATEWAY_SSID "AliyunOnlyTest"
//连接的路由器密码
#define GATEWAY_PASSWORD "aliyun#123456"

//数据包大小
#define EXAMPLE_DEFAULT_PKTSIZE 1024

/*
 * 自己作为AP热点时候，配置信息如下
 */
//ssid
#define AP_SSID "esp32_mmwave_host"
//密码
#define AP_PAW "12345678"
//最大连接数
#define EXAMPLE_MAX_STA_CONN 1

#define TAG "esp32_mmwave_host :"

extern EventGroupHandle_t udp_event_group;
#define WIFI_CONNECTED_BIT BIT0
#define UDP_CONNCETED_SUCCESS BIT1

void wifi_init_softap();
void wifi_init_sta();


#endif