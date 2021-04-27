 
#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"
 
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
 
 
#define HOST_IP_ADDR "255.255.255.255"
#define PORT         8080
int sock = 0;
struct sockaddr_in dest_addr;

int udp_init_socket(void)
{
 
    sock = socket(AF_INET,SOCK_DGRAM,0);
     printf("sock:%d\n",sock);
    if(sock < 0){
       printf( "Unable to create socket: errno %d", errno);
       return -1;
    }
    printf("Socket created, sending to %s:%d", HOST_IP_ADDR, PORT);
   
    dest_addr.sin_addr.s_addr = inet_addr(HOST_IP_ADDR); //设置目标IP地址 
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT);//目标端口
    struct sockaddr_in Loacl_addr; 
    Loacl_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    Loacl_addr.sin_family = AF_INET;
    Loacl_addr.sin_port = htons(54321); //设置本地端口
    uint8_t res = 0;
    res = bind(sock,(struct sockaddr *)&Loacl_addr,sizeof(Loacl_addr));
    if(res != 0){
        printf("bind error\n");
        return -1;
    }
    return sock;
}

void udp_send_packet(int udp_sock, const char *udp_tx_buffer, int tx_len)
{
    int err = sendto(udp_sock, udp_tx_buffer, tx_len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
}
 