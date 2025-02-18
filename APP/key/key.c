#include "key.h"
#include "SysTick.h"
#include "lvgl/lvgl.h"
#include <stdbool.h>
#include "usart.h"	 

/*******************************************************************************
* �� �� ��         : KEY_Init
* ��������		   : ������ʼ��
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //����ṹ�����	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=KEY_UP_Pin;	   //ѡ����Ҫ���õ�IO��
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;//��������  
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	   //���ô�������
	GPIO_Init(KEY_UP_Port,&GPIO_InitStructure);		  /* ��ʼ��GPIO */
	
	GPIO_InitStructure.GPIO_Pin=KEY_DOWN_Pin|KEY_LEFT_Pin|KEY_RIGHT_Pin;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;	//��������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(KEY_Port,&GPIO_InitStructure);
	
	UDEBUG("KEY��ʼ����ɣ�\r\n");
}

/*******************************************************************************
* �� �� ��         : KEY_Scan
* ��������		   : ����ɨ����
* ��    ��         : mode=0:���ΰ��°���
					 mode=1���������°���
* ��    ��         : 0��δ�а�������
					 KEY_UP��K_UP������
					 KEY_DOWN��K_DOWN������
					 KEY_LEFT��K_LEFT������
					 KEY_RIGHT��K_RIGHT������
*******************************************************************************/
u8 KEY_Scan(u8 mode)
{
	static u8 key=1;
	if(key==1&&(K_UP==1||K_DOWN==0||K_LEFT==0||K_RIGHT==0)) //����һ����������
	{
		delay_ms(10);  //����
		key=0;
		if(K_UP==1)
		{
			return KEY_UP; 
		}
		else if(K_DOWN==0)
		{
			return KEY_DOWN; 
		}
		else if(K_LEFT==0)
		{
			return KEY_LEFT; 
		}
		else
		{
			return KEY_RIGHT;
		}
	}
	else if(K_UP==0&&K_DOWN==1&&K_LEFT==1&&K_RIGHT==1)    //�ް�������
	{
		key=1;
	}
	if(mode==1) //������������
	{
		key=1;
	}
	return 0;
}




static u32 lastkey = NULL;

u32 last_key(void)
{
	return lastkey;	
}

TaskHandle_t lv_key_scan_Handle = NULL;/* ���������� */

void lv_key_scan(void)
{
	TickType_t xLastWakeTime;
	const TickType_t xPeriod = pdMS_TO_TICKS( 10 );
	xLastWakeTime = xTaskGetTickCount();
	while(1)
	{
		ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		
		if(K_UP==1||K_DOWN==0||K_LEFT==0||K_RIGHT==0) //����һ����������
		{
			xTaskDelayUntil(&xLastWakeTime,xPeriod);  //����

			if(K_UP==1)
			{
				lastkey = KEY_UP; 
			}
			else if(K_DOWN==0)
			{
				lastkey = KEY_DOWN; 
			}
			else if(K_LEFT==0)
			{
				lastkey = KEY_LEFT; 
			}
			else
			{
				lastkey = KEY_RIGHT;
			}
		}
	}
}

bool key_pressed(void)
{	
  switch (lastkey)
	{
		case KEY_UP :
					return K_UP? true : false;
    case KEY_DOWN :
					return K_DOWN? false : true;
    case KEY_LEFT :
					return K_LEFT? false : true;
    case KEY_RIGHT :
					return K_RIGHT? false : true;
    default :
			return false;
	}
}

int16_t enc_diff = NULL;

int16_t get_new_moves(void)
{
	int16_t diff = enc_diff;
	enc_diff = 0;
	return diff;
}

void update_enc_diff(void)
{
	if(lastkey == KEY_UP)
	{
		enc_diff += 1;
	}
  else if(lastkey == KEY_DOWN)
	{
		enc_diff -= 1;
	}
}
