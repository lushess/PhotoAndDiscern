#include "key.h"
#include "SysTick.h"
#include "lvgl/lvgl.h"
#include <stdbool.h>
#include "usart.h"	 

/*******************************************************************************
* 函 数 名         : KEY_Init
* 函数功能		   : 按键初始化
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //定义结构体变量	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=KEY_UP_Pin;	   //选择你要设置的IO口
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;//下拉输入  
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	   //设置传输速率
	GPIO_Init(KEY_UP_Port,&GPIO_InitStructure);		  /* 初始化GPIO */
	
	GPIO_InitStructure.GPIO_Pin=KEY_DOWN_Pin|KEY_LEFT_Pin|KEY_RIGHT_Pin;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;	//上拉输入
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(KEY_Port,&GPIO_InitStructure);
	
	UDEBUG("KEY初始化完成！\r\n");
}

/*******************************************************************************
* 函 数 名         : KEY_Scan
* 函数功能		   : 按键扫描检测
* 输    入         : mode=0:单次按下按键
					 mode=1：连续按下按键
* 输    出         : 0：未有按键按下
					 KEY_UP：K_UP键按下
					 KEY_DOWN：K_DOWN键按下
					 KEY_LEFT：K_LEFT键按下
					 KEY_RIGHT：K_RIGHT键按下
*******************************************************************************/
u8 KEY_Scan(u8 mode)
{
	static u8 key=1;
	if(key==1&&(K_UP==1||K_DOWN==0||K_LEFT==0||K_RIGHT==0)) //任意一个按键按下
	{
		delay_ms(10);  //消抖
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
	else if(K_UP==0&&K_DOWN==1&&K_LEFT==1&&K_RIGHT==1)    //无按键按下
	{
		key=1;
	}
	if(mode==1) //连续按键按下
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

TaskHandle_t lv_key_scan_Handle = NULL;/* 创建任务句柄 */

void lv_key_scan(void)
{
	TickType_t xLastWakeTime;
	const TickType_t xPeriod = pdMS_TO_TICKS( 10 );
	xLastWakeTime = xTaskGetTickCount();
	while(1)
	{
		ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		
		if(K_UP==1||K_DOWN==0||K_LEFT==0||K_RIGHT==0) //任意一个按键按下
		{
			xTaskDelayUntil(&xLastWakeTime,xPeriod);  //消抖

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
