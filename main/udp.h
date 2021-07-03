#ifndef __UDP_H
#define __UDP_H

extern int udp_sock;

int udp_init_socket(void);
void udp_send_packet(int udp_sock, const char *udp_tx_buffer, int tx_len);
void udp_data_upload(int mmwave_ad_value, signed short bmd101_ad_value);

#endif