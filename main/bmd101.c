#include "bmd101.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "udp.h"
#include "adc.h"

#define BMD101_UART_TXD  (GPIO_NUM_1)
#define BMD101_UART_RXD  (GPIO_NUM_3)
#define BMD101_UART_RTS  (UART_PIN_NO_CHANGE)
#define BMD101_UART_CTS  (UART_PIN_NO_CHANGE)


signed short bmd101_advalue_buf[128] = {0};
int bmd101_advalue_cnt = 0;

typedef enum {
	parse_idle = 0,
	parse_sync_1,
	parse_sync_2,
	parse_plength,
	parse_code,
	parse_vlength,
	parse_value_h,
	parse_value_l,
	parse_checksum
}bmd101_parse_state_t;

static int bmd101_parse_ch(char bmd101_recv_ch, signed short *bmd101_ad_value)
{
	static bmd101_parse_state_t bmd101_parse_state = parse_idle;

	switch (bmd101_parse_state) {
	case parse_idle:
		if (bmd101_recv_ch == (char)0xAA)
			bmd101_parse_state = parse_sync_1;
		else
			bmd101_parse_state = parse_idle;
		break;
	case parse_sync_1:
		if (bmd101_recv_ch == (char)0xAA)
			bmd101_parse_state = parse_sync_2;
		else
			bmd101_parse_state = parse_idle;
		break;
	case parse_sync_2:
		if (bmd101_recv_ch > 170)
			bmd101_parse_state = parse_idle;
		else 
			bmd101_parse_state = parse_plength;
		break;
	case parse_plength:
		bmd101_parse_state = parse_code;
		break;
	case parse_code:
		bmd101_parse_state = parse_vlength;
		break;
	case parse_vlength:
		*bmd101_ad_value = bmd101_recv_ch;
		*bmd101_ad_value <<= 8;
		bmd101_parse_state = parse_value_h;
		break;
	case parse_value_h:
		*bmd101_ad_value |= bmd101_recv_ch;
		bmd101_parse_state = parse_value_l;
		break;
	case parse_value_l:
		bmd101_parse_state = parse_idle;
		return 1;
	default:
		break;
	}
	return 0;
}

void bmd101_ble_uart_init()
{
    uart_config_t uart_config = {
        .baud_rate = 57600,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    uart_driver_install(UART_NUM_1, BM101_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, BMD101_UART_TXD, BMD101_UART_RXD, BMD101_UART_RTS, BMD101_UART_CTS);
}

void bmd101_parse_packet(const char* bmd101_recv_packet, int bmd101_packet_len)
{
	signed short bmd101_ad_value = 0;
	signed int bmd101_upload_value = 0;
	int adc_val = 0;
	for (int i = 0; i < bmd101_packet_len; i++) {
		if (bmd101_parse_ch(bmd101_recv_packet[i], &bmd101_ad_value)) {
			bmd101_advalue_buf[bmd101_advalue_cnt++] = bmd101_ad_value;
			if(bmd101_advalue_cnt == 5) {
				bmd101_upload_value = 0;
				for(int i = 0; i < bmd101_advalue_cnt; i++)
					bmd101_upload_value += bmd101_advalue_buf[i];
				bmd101_upload_value = bmd101_upload_value / bmd101_advalue_cnt;
				bmd101_advalue_cnt = 0;
				adc_val = getadc();
				udp_data_upload(adc_val, bmd101_upload_value);
			}
		} 
	}
}
