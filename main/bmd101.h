#ifndef __BMD101_H
#define __BMD101_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "freertos/portable.h"
#include "freertos/semphr.h"


#define BM101_BUF_SIZE (256)

extern int udp_sock;
extern volatile int upload_flag; 

extern signed short bmd101_advalue_buf[128];
extern int bmd101_advalue_cnt;
extern SemaphoreHandle_t xSemaphoreUpload;

void bmd101_ble_uart_init(void);
void bmd101_parse_packet(const char* bmd101_recv_packet, int bmd101_packet_len);

#endif