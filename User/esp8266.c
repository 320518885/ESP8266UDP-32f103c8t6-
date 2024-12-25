#include "esp8266.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "core_json.h"

/*
*************************************
�궨��
*************************************
*/
#define WIFI_SSID        "1034"
#define WIFI_PASSWD      "Liziming1314520"

#define MQTT_CLIENT_ID   "k0rr6LTAkT9.ESP8266DUAN|securemode=2\\,signmethod=hmacsha256\\,timestamp=1706078869541|"   
#define MQTT_USER_NAME   "ESP8266DUAN&k0rr6LTAkT9"
#define MQTT_PASSWD      "01c444c0f9ecc3f6ccb36ca40650f6e9c64e5a26c7670c56ee71cc23291718b8"
#define BROKER_ASDDRESS  "k0rr6LTAkT9.iot-as-mqtt.cn-shanghai.aliyuncs.com"
#define SUB_TOPIC        "/sys/k0rr6LTAkT9/ESP8266DUAN/thing/service/property/set"
#define PUB_TOPIC        "/k0rr6LTAkT9/ESP8266DUAN/user/ESP8266DUAN"
#define JSON_FORMAT      "{\\\"x\\\":%f\\\,\\\"y\\\":%f}"  //*****�иĶ�

/*
*************************************
�궨��
*************************************
*/
/*
*************************************
��������
*************************************
*/
/*
*************************************
��������
*************************************
*/
unsigned char receive_buf[512];	  //����2���ջ�������
unsigned char receive_start = 0;	//����2���տ�ʼ��־λ
unsigned char receive_data = 0;
uint16_t receive_count = 0;	      //����2�������ݼ�����
uint16_t receive_finish = 0;	    //����2���ս�����־λ 
uint8_t msg_body[128] = {0};

float x=1.11;
float y=1.11;


/**
  * @brief          ����json����
  * @param[in]      json_msg:json����,json_len:json���ݳ���
  * @retval         ����0�ҵ�ָ��json���ݣ����򷵻�1
  */
uint8_t parse_json_msg(uint8_t *json_msg,uint8_t json_len)
{
  uint8_t retval =0;
  
  JSONStatus_t result;
  char query[] = "params.light";
  size_t queryLength = sizeof( query ) - 1;
  char * value;
  size_t valueLength;
  result = JSON_Validate((const char *)json_msg, json_len);
  if( result == JSONSuccess)
  {
    result = JSON_Search((char *)json_msg, json_len, query, queryLength,&value, &valueLength );
    if( result == JSONSuccess)
    {
      char save = value[valueLength];
      value[valueLength] = '\0';
      printf("Found: %s %d-> %s\n", query, valueLength,value);
      value[valueLength] = save;
      retval = 0;
    }
    else
    {
      retval = 1;
    }
  }
  else
  {
    retval = 1;
  }  
  return retval;
}
/**
  * @brief          ����2���ݽ��մ�������
  * @param[in]      none
  * @retval         none
  */
void uart2_receiver_handle(void)
{
     
  if(__HAL_UART_GET_FLAG(&huart2,UART_FLAG_RXNE) != RESET)
  {
    HAL_UART_Receive(&huart2, &receive_data, 1, 1000);//����2����1λ����
    receive_buf[receive_count++] = receive_data;
    receive_start = 1;	                              //����2�������ݿ�ʼ��־λ��1
    receive_finish = 0;	                              //����2����������ɱ�־λ��0
  }
}
/**
  * @brief          ����2���ݽ�����0����
  * @param[in]      len:��յ����ݳ���
  * @retval         none
  */
void uart2_receiver_clear(uint16_t len)	
{
	memset(receive_buf,0x00,len);
	receive_count = 0;
	receive_start = 0;
	receive_finish = 0;
}
/**
  * @brief          esp8266���������
  * @param[in]      cmd:���͵�����,len:����ĳ���,rec_data:������������
  * @retval         none
  */
uint8_t esp8266_send_cmd(unsigned char *cmd,unsigned char len,char *rec_data)	
{
  unsigned char retval =0;
  unsigned int count = 0;

  HAL_UART_Transmit(&huart2, cmd, len, 1000);	                                   
  while((receive_start == 0)&&(count<1000))
  {
    count++;
    HAL_Delay(1);
  }

  if(count >= 1000)	
  {
    retval = 1;	
  }
  else	
  {
    do
    {
      receive_finish++;
      HAL_Delay(1);
    }
    while(receive_finish < 500);
    retval = 2;
    if(strstr((const char*)receive_buf, rec_data))	
    {
      retval = 0;	
    }
  }
  uart2_receiver_clear(receive_count);
  return retval;
}
/**
  * @brief          esp8266����wifi����
  * @param[in]      none
  * @retval         �������óɹ�����0,���򷵻�1
  */
uint8_t esp8266_config_network(void)
{
	uint8_t retval =0;
	uint16_t count = 0;
	
	HAL_UART_Transmit(&huart2, (unsigned char *)"AT+CWJAP=\""WIFI_SSID"\",\""WIFI_PASSWD"\"\r\n",strlen("AT+CWJAP=\""WIFI_SSID"\",\""WIFI_PASSWD"\"\r\n"), 1000);
	
	while((receive_start == 0)&&(count<1000))
	{
		count++;
		HAL_Delay(1);
	}
	
	if(count >= 1000)	
	{
		retval = 1;	
	}
	else
	{
		HAL_Delay(8000);
		if(strstr((const char*)receive_buf, "OK"))	
		{
			retval = 0;	
		}
    else
    {
      retval = 1;
    }
	}
  uart2_receiver_clear(receive_count);
	return retval;
}
/**
  * @brief          esp8266���ӷ���
  * @param[in]      none
  * @retval         ���ӳɹ�����0,���򷵻�1
  */
uint8_t esp8266_connect_server(void)
{
	uint8_t retval=0;
	uint16_t count = 0;

	HAL_UART_Transmit(&huart2, (unsigned char *)"AT+MQTTCONN=0,\""BROKER_ASDDRESS"\",1883,0\r\n",strlen("AT+MQTTCONN=0,\""BROKER_ASDDRESS"\",1883,0\r\n"), 1000);	
	while((receive_start == 0)&&(count<1000))	
	{
		count++;
		HAL_Delay(1);
	}
	
	if(count >= 1000)	
	{
		retval = 1;	
	}
	else	
	{
		HAL_Delay(5000);
		if(strstr((const char*)receive_buf, "OK"))	
		{
			retval = 0;	
		}
    else
    {
      retval = 1;	
    }
	}
  uart2_receiver_clear(receive_count);	
	return retval;
}
/**
  * @brief          esp8266��λ
  * @param[in]      none
  * @retval         ����0��λ�ɹ�,����1��λʧ��
  */
uint8_t esp8266_reset(void)
{
	uint8_t retval =0;
	uint16_t count = 0;
	
	HAL_UART_Transmit(&huart2, (unsigned char *)"AT+RST\r\n",8, 1000);
	while((receive_start == 0)&&(count<2000))	
	{
		count++;
		HAL_Delay(1);
	}
	if(count >= 2000)	
	{
		retval = 1;	
	}
	else	
	{
		HAL_Delay(5000);
		if(strstr((const char*)receive_buf, "OK"))	
		{
			retval = 0;	
		}
    else
    {
      retval = 1;	
    }
	}
  uart2_receiver_clear(receive_count);	
	return retval;
}
/**
  * @brief          esp8266��������
  * @param[in]      none
  * @retval         ����0�������ݳɹ�,����1��������ʧ��
  */
uint8_t esp8266_send_msg(void)	
{
	uint8_t retval =0;	
  uint16_t count = 0;			
	static uint8_t error_count=0;
	unsigned char msg_buf[256];
  
  sprintf((char *)msg_buf,"AT+MQTTPUB=0,\""PUB_TOPIC"\",\""JSON_FORMAT"\",1,0\r\n",x,y);
	HAL_UART_Transmit(&huart2, (unsigned char *)msg_buf,strlen((const char *)msg_buf), 1000);	
    HAL_UART_Transmit(&huart1, (unsigned char *)msg_buf,strlen((const char *)msg_buf), 1000);	
	while((receive_start == 0)&&(count<500))	
	{
		count++;
		HAL_Delay(1);
	}
	if(count >= 500)	
	{
		retval = 1;	
	}
	else	
	{
		HAL_Delay(50);
		if(strstr((const char*)receive_buf, "OK"))	
		{
			retval = 0;	
			error_count=0;
		}
		else 
		{
			error_count++;
			if(error_count==5)
			{
				error_count=0;
        printf("RECONNECT MQTT BROKER!!!\r\n");
				esp8266_init();
			}
		}
	}
  uart2_receiver_clear(receive_count);	
	return retval;
}
/**
  * @brief          esp8266��������
  * @param[in]      none
  * @retval         ����0������������,����1���������쳣��������
  */
uint8_t esp8266_receive_msg(void)	
{
  uint8_t retval =0;
	int msg_len=0;
	
  
	if(receive_start == 1)	
	{
		do
    {
			receive_finish++;
			HAL_Delay(1);
		}
    while(receive_finish < 5);	
		
		if(strstr((const char*)receive_buf,"+MQTTSUBRECV:"))
		{
			sscanf((const char *)receive_buf,"+MQTTSUBRECV:0,\""SUB_TOPIC"\",%d,%s",&msg_len,msg_body);
			
			
			printf("len:%d,msg:%s\r\n",msg_len,msg_body);
			if(strlen((const char*)msg_body)== msg_len)
			{
        retval = parse_json_msg(msg_body,msg_len);
			}
      else
      {
        retval = 1;
      }
		}
    else 
    {
      retval = 1;
    }
	}
  else
  {
    retval = 1;
  }
  uart2_receiver_clear(receive_count);	
  return retval;
}
/**
  * @brief          esp8266��ʼ��
  * @param[in]      none
  * @retval         none
  */
void esp8266_init(void)
{
	__HAL_UART_ENABLE_IT(&huart2,UART_IT_RXNE);           											//�򿪴���2�����ж�
	
	printf("1.SETTING STATION MODE\r\n");
                    
	while(esp8266_send_cmd((uint8_t *)"AT+CWMODE=1\r\n",strlen("AT+CWMODE=1\r\n"),"OK")!=0)
	{
		HAL_Delay(1000);
	}
	
	printf("2.CLOSE ESP8266 ECHO\r\n");
	
	while(esp8266_send_cmd((uint8_t *)"ATE0\r\n",strlen("ATE0\r\n"),"OK")!=0)
	{
		HAL_Delay(1000);
	}
	
	printf("3.NO AUTO CONNECT WIFI\r\n"); 
	
	while(esp8266_send_cmd((uint8_t *)"AT+CWAUTOCONN=0\r\n",strlen("AT+CWAUTOCONN=0\r\n"),"OK")!=0)
	{
		HAL_Delay(1000);
	}
	
	printf("4.RESET ESP8266\r\n");
	
	while(esp8266_reset() != 0)
	{
		HAL_Delay(5000);
	}
	
	printf("5.CONFIG WIFI NETWORK\r\n");
	
	while(esp8266_config_network() != 0)
	{
		HAL_Delay(8000);
	}
	
	printf("6.MQTT USER CONFIG\r\n");
	
	while(esp8266_send_cmd((uint8_t *)"AT+MQTTUSERCFG=0,1,\""MQTT_CLIENT_ID"\",\""MQTT_USER_NAME"\",\""MQTT_PASSWD"\",0,0,\"\"\r\n",
                          strlen("AT+MQTTUSERCFG=0,1,\""MQTT_CLIENT_ID"\",\""MQTT_USER_NAME"\",\""MQTT_PASSWD"\",0,0,\"\"\r\n"),"OK")!=0)
	{
		HAL_Delay(2000);
	}
	
	printf("7.CONNECT MQTT BROKER\r\n");
	
	while(esp8266_connect_server() != 0)
	{
		HAL_Delay(8000);
	}
	
	printf("8.SUBSCRIBE TOPIC\r\n");

	while(esp8266_send_cmd((uint8_t *)"AT+MQTTSUB=0,\""SUB_TOPIC"\",0\r\n",strlen("AT+MQTTSUB=0,\""SUB_TOPIC"\",0\r\n"),"OK")!=0)
	{
		HAL_Delay(2000);
	}
	printf("9.ESP8266 INIT OK!!!\r\n");
}




























