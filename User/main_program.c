#include "main_program.h"
#include <stdio.h>
#include "time_handle.h"
#include "ESP8266.h"


/*
*************************************
宏定义
*************************************
*/

/*
*************************************
变量声明
*************************************
*/
extern uint16_t index_send;
/*
*************************************
函数声明
*************************************
*/

/*
*************************************
变量定义
*************************************
*/
uint8_t led_status =0;
/*
*************************************
函数定义
*************************************
*/

/**
  * @brief          初始化函数,相当于Arduino的setup()函数,只在初始的时候执行一次
  * @param[in]      none
  * @retval         none
  */
void user_init_program(void)
{
	ESP8266_init();
	time_slot_start();
}
/**
  * @brief          初始化函数,相当于Arduino的loop()函数,一直执行该函数
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

