#include "main_program.h"
#include <stdio.h>
#include "time_handle.h"
#include "ESP8266.h"


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
extern uint16_t index_send;
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
uint8_t led_status =0;
/*
*************************************
��������
*************************************
*/

/**
  * @brief          ��ʼ������,�൱��Arduino��setup()����,ֻ�ڳ�ʼ��ʱ��ִ��һ��
  * @param[in]      none
  * @retval         none
  */
void user_init_program(void)
{
	ESP8266_init();
	time_slot_start();
}
/**
  * @brief          ��ʼ������,�൱��Arduino��loop()����,һֱִ�иú���
  * @param[in]      none
  * @retval         none
  */
void user_main_program(void)
{
  
	  if(ESP8266_receive_msg()!=1 && ESP8266_receive_msg()!=0)
	  {
		  printf("SUCCESS!");
	  }
  
}

