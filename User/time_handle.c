#include "time_handle.h"
#include "main.h"
#include "tim.h"
#include <stdio.h>

/*
*************************************
�궨��
*************************************
*/
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
static uint16_t index_10ms = 0;
uint16_t index_send = 0;

/**
  * @brief          ��ʱ��2�жϷ�����,10msһ���ж�
  * @param[in]      htim:��ʱ��
  * @retval         none
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)	
{
	
}
/**
  * @brief          ������ѯʱ��Ƭ,������ʱ��2
  * @param[in]      none
  * @retval         none
  */
void time_slot_start(void)
{
  HAL_TIM_Base_Start_IT(&htim2);	//�򿪶�ʱ��2�ж�
}

	

