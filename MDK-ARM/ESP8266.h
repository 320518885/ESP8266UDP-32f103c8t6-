#ifndef __ESP8266_H__
#define __ESP8266_H__

#include "main.h"

void ESP8266_init(void);

uint8_t parse_json_msg(uint8_t *json_msg,uint8_t json_len);

void uart2_receiver_handle(void);
void uart2_receiver_clear(uint16_t len)	;

uint8_t ESP8266_send_cmd(unsigned char *cmd,unsigned char len,char *rec_data)	;
uint8_t ESP8266_reset(void);
uint8_t ESP8266_receive_msg(void);


#endif
