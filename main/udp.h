#ifndef __UDP_H
#define __UDP_H

int udp_init_socket(void);
void udp_send_packet(int udp_sock, const char *udp_tx_buffer, int tx_len);

#endif