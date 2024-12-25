#include "ESP8266.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "core_json.h"




/*             宏定义         */

#define WIFINAME     "1034"
#define WIFIPASS     "Liziming1314520"

#define IPPC    "192.168.4.2"  
#define PORTPC  "8080"

#define IPESP   "192.168.4.1"
#define PORTESP "2345"



/*
*************************************
变量定义
*************************************
*/
unsigned char receive_buf[512];	  //串口2接收缓存数组
unsigned char receive_start = 0;	//串口2接收开始标志位
unsigned char receive_data = 0;
uint16_t receive_count = 0;	      //串口2接收数据计数器
uint16_t receive_finish = 0;	    //串口2接收结束标志位 
uint8_t msg_body[128] = {0};
int left,right,LCl,LCr,RCl,RCr,button;

/**
  * @brief          解析json数据
  * @param[in]      json_msg:json数据,json_len:json数据长度
  * @retval         返回0找到指定json数据，否则返回1
  */
uint8_t parse_json_msg(uint8_t *json_msg,uint8_t json_len)
{
  uint8_t retval =0;
  
  JSONStatus_t result;
  char query[] = "]";
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
  * @brief          串口2数据接收处理函数
  * @param[in]      none
  * @retval         none
  */
void uart2_receiver_handle(void)
{
     
  if(__HAL_UART_GET_FLAG(&huart2,UART_FLAG_RXNE) != RESET)
  {
    HAL_UART_Receive(&huart2, &receive_data, 1, 1000);//串口2接收1位数据
    receive_buf[receive_count++] = receive_data;
    receive_start = 1;	                              //串口2接收数据开始标志位置1
    receive_finish = 0;	                              //串口2接收数据完成标志位清0
  }
}


/**
  * @brief          串口2数据接收清0函数
  * @param[in]      len:清空的数据长度
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
  * @brief          esp8266发送命令函数
  * @param[in]      cmd:发送的命令,len:命令的长度,rec_data:期望接收数据
  * @retval         none
  */
uint8_t ESP8266_send_cmd(unsigned char *cmd,unsigned char len,char *rec_data)	
{
  unsigned char retval =0;//返回的值，若是0则代表指令发送成功
  unsigned int count = 0;

  HAL_UART_Transmit(&huart2, cmd, len, 1000);	                                   
  while((receive_start == 0)&&(count<3000))
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



//暂时用不到
///**
//  * @brief          esp8266发送数据
//  * @param[in]      none
//  * @retval         返回0发送数据成功,返回1发送数据失败
//  */
//uint8_t ESP8266_send_msg(void)	
//{
//	uint8_t retval =0;	
//  uint16_t count = 0;			
//	static uint8_t error_count=0;
//	unsigned char msg_buf[256];
//  
//	sprintf((char *)msg_buf,"data:%d %d\r\n",x,y);
//	HAL_UART_Transmit(&huart2, (unsigned char *)msg_buf,strlen((const char *)msg_buf), 1000);	
//    HAL_UART_Transmit(&huart1, (unsigned char *)msg_buf,strlen((const char *)msg_buf), 1000);	
//	while((receive_start == 0)&&(count<500))	
//	{
//		count++;
//		HAL_Delay(1);
//	}
//	if(count >= 500)	
//	{
//		retval = 1;	
//	}
//	else	
//	{
//		HAL_Delay(50);
//		if(strstr((const char*)receive_buf, "OK"))	
//		{
//			retval = 0;	
//			error_count=0;
//		}
//		else 
//		{
//			error_count++;
//			if(error_count==5)
//			{
//				error_count=0;
//        printf("RECONNECT BROKER!!!\r\n");
//				ESP8266_init();
//			}
//		}
//	}               
//  uart2_receiver_clear(receive_count);	
//	return retval;
//}



/**
  * @brief          ESP8266接收数据
  * @param[in]      none
  * @retval         返回0接收数据正常,返回1接收数据异常或无数据
  */
uint8_t ESP8266_receive_msg(void)	
{
  uint8_t retval =0;
	//int msg_len=0;
	
  
	if(receive_start == 1)	
	{
		do
		{
			receive_finish++;
			HAL_Delay(1);
		}
		while(receive_finish < 5);	
		
		if(strstr((const char*)receive_buf,"]"))
		{
			sscanf((const char*)receive_buf, "[%d,%d,%d,%d,%d,%d,%d]", &left, &right,&LCr,&LCl,&RCr,&RCl,&button);
			
			printf("Parsed numbers: %d and %d and %d and %d and %d and %d and %d\r\n", left, right,LCr,LCl,RCr,RCl,button);
			
			retval = 2;
			
			uart2_receiver_clear(receive_count);
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
  * @brief          ESP8266初始化
  * @param[in]      none
  * @retval         none
  */
void ESP8266_init(void)
{
	__HAL_UART_ENABLE_IT(&huart2,UART_IT_RXNE);
	
	printf("0.+++\r\n");
	
	ESP8266_send_cmd((uint8_t *)"+++",strlen("+++"),"OK");
	
	printf("0.RESET\r\n");
	
	while(ESP8266_send_cmd((uint8_t *)"AT+RST\r\n",strlen("AT+RST\r\n"),"OK")!=0)
	{
		HAL_Delay(1000);
	}

	
	printf("1.SETTING AP MODE\r\n");
	
	while(ESP8266_send_cmd((uint8_t *)"AT+CWMODE=2\r\n",strlen("AT+CWMODE=2\r\n"),"OK")!=0)
	{
		HAL_Delay(1000);
	}
	
	printf("2.SET WIFI\r\n");
	
	while(ESP8266_send_cmd((uint8_t *)"AT+CWSAP=\""WIFINAME"\",\""WIFIPASS"\",1,0\r\n",strlen("AT+CWSAP=\""WIFINAME"\",\""WIFIPASS"\",1,0\r\n"),"\"192.168.4.2\"")!=0)
	{
		HAL_Delay(1000);
	}
	
	printf("3.UDP CONFIG\r\n");
	
	while(ESP8266_send_cmd((uint8_t *)"AT+CIPSTART=\"UDP\",\""IPPC"\",8080,2345,0\r\n",strlen("AT+CIPSTART=\"UDP\",\""IPPC"\",8080,2345,0\r\n"),"OK")!=0)
	{
		HAL_Delay(1000);
	}
	
	
	printf("4.Transparrent transmisstion mode\r\n");
	
	while(ESP8266_send_cmd((uint8_t *)"AT+CIPMODE=1\r\n",strlen("AT+CIPMODE=1\r\n"),"OK")!=0)
	{
		HAL_Delay(1000);
	}
	
	printf("5. DATA Transparrent transmisstion\r\n");
	
	while(ESP8266_send_cmd((uint8_t *)"AT+CIPSEND\r\n",strlen("AT+CIPSEND\r\n"),"OK")!=0)
	{
		HAL_Delay(1000);
	}
	


	printf("6.ESP8266 INIT OK!!!\r\n");
}
