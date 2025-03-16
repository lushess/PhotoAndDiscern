#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"
#include "tftlcd.h"
#include <stdlib.h>
#include "key.h"
#include "sd.h"
#include "flash.h"
#include "ff.h" 
#include "FATFS/FATFS_APP/fatfs_app.h"
#include "beep.h"
#include "font_show.h"
#include "exti.h"
#include "time.h" 
#include "string.h"		
#include "math.h"	 
#include "ov7670.h"
#include "sram.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "user_task.h"
#include "lv_port_indev.h"
#include "lv_port_disp.h"
#include "lv_port_fs_Fatfs.h"
#include "User/App/App.h"

static TaskHandle_t AppTaskCreate_Handle = NULL;
extern TaskHandle_t camera_refresh_Handle;
extern TaskHandle_t discern_Handle;
extern TaskHandle_t photo_Handle;
static TaskHandle_t LEDsysRunning_Handle = NULL;
static TaskHandle_t key_press_task_Handle = NULL;
static TaskHandle_t LV_Task_Handle = NULL;
extern TaskHandle_t lv_key_scan_Handle;

/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void AppTaskCreate(void);/* 用于创建任务 */
static void lv_task(void); /*lvgl任务刷新*/
static void LEDsysRunning(void);//用于指示系统处于运行

static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
  
  taskENTER_CRITICAL();           //进入临界区	
		
	xReturn = xTaskCreate((TaskFunction_t )LEDsysRunning,  /* 任务入口函数 */
                        (const char*    )"LEDsysRunning",/* 任务名字 */
                        (uint16_t       )0x80,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )2, /* 任务的优先级 */
                        (TaskHandle_t*  )&LEDsysRunning_Handle);/* 任务控制块指针 */ 
	 if(pdPASS == xReturn)
    UDEBUG("创建LEDsysRunning任务成功!\r\n");
	 else UDEBUG("创建LEDsysRunning任务失败,错误代码:%d\r\n",(int)xReturn);
			 
	 xReturn = xTaskCreate((TaskFunction_t )camera_refresh,  /* 任务入口函数 */
													(const char*    )"camera_refresh",/* 任务名字 */
													(uint16_t       )0x400,  /* 任务栈大小 */
													(void*          )NULL,/* 任务入口函数参数 */
													(UBaseType_t    )7, /* 任务的优先级 */
													(TaskHandle_t*  )&camera_refresh_Handle);/* 任务控制块指针 */ 		
	 if(pdPASS == xReturn)
			UDEBUG("创建camera_refresh任务成功!\r\n");
	 else UDEBUG("创建camera_refresh任务失败,错误代码:%d\r\n",(int)xReturn);
	 
	 xReturn = xTaskCreate((TaskFunction_t )photo,  /* 任务入口函数 */
                        (const char*    )"photo",/* 任务名字 */
                        (uint16_t       )0x400,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )11, /* 任务的优先级 */
                        (TaskHandle_t*  )&photo_Handle);/* 任务控制块指针 */ 
		
  if(pdPASS == xReturn)
    UDEBUG("创建photo任务成功!\r\n");
	else UDEBUG("创建photo任务失败,错误代码:%d\r\n",(int)xReturn);
	
	 xReturn = xTaskCreate((TaskFunction_t )discern,  /* 任务入口函数 */
                        (const char*    )"discern",/* 任务名字 */
                        (uint16_t       )0x400,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )10, /* 任务的优先级 */
                        (TaskHandle_t*  )&discern_Handle);/* 任务控制块指针 */ 
		
  if(pdPASS == xReturn)
    UDEBUG("创建discern任务成功!\r\n");
	else UDEBUG("创建discern任务失败,错误代码:%d\r\n",(int)xReturn);
		
	xReturn = xTaskCreate((TaskFunction_t )lv_task,  /* 任务入口函数 */
				(const char*    )"lv_task",/* 任务名字 */
				(uint16_t       )0x200,  /* 任务栈大小 */
				(void*          )NULL,/* 任务入口函数参数 */
				(UBaseType_t    )30, /* 任务的优先级 */
				(TaskHandle_t*  )&LV_Task_Handle);/* 任务控制块指针 */ 
	if(pdPASS == xReturn)
    UDEBUG("创建lv_task任务成功!\r\n");	

	
	lv_init();
	UDEBUG("lvgl初始化成功!\r\n");
	lv_port_disp_init();
	UDEBUG("lvgl屏幕初始化成功!\r\n");
	lv_port_indev_init();
	UDEBUG("lvgl设备初始化成功!\r\n");
	lv_port_fs_init();
	UDEBUG("lvgl文件系统初始化成功!\r\n");
	App_Init_Wrapper();

  vTaskDelete(NULL); //删除AppTaskCreate任务
	UDEBUG("启动任务已删除!\r\n");
  taskEXIT_CRITICAL();            //退出临界区
}

static void lv_task(void)
{
	TickType_t xLastWakeTime;
	const TickType_t xPeriod = pdMS_TO_TICKS( 5 );
	xLastWakeTime = xTaskGetTickCount();
	while(1)
	{		
		xTaskDelayUntil(&xLastWakeTime,xPeriod);
		xTaskNotifyGive(key_scan_task_Handle); // 键盘扫描任务
	  lv_task_handler(); //lvgl刷新
	}
}

static void LEDsysRunning(void)//用于指示系统处于运行
{
	while(1)
	{
	 led1=!led1;
	 vTaskDelay(1000);
	}
}

int main()
{
	u8 i=0;
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
	
	SysTick_Init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);  //中断优先级分组4，4位用于抢占优先级最大15
//	USART1_Init();
	LED_Init();
	TFTLCD_Init();			//LCD初始化
	KEY_Init();
	EN25QXX_Init();				//初始化EN25Q128
	
//	char *ExramTest = malloc(sizeof("EXRAM测试"));
//	if (NULL == ExramTest) UDEBUG("EXRAM堆内存分配失败");
//	snprintf(ExramTest, sizeof("EXRAM测试"), "EXRAM测试");
//	UDEBUG("EXRAM测试地址:%p;内容:%s",ExramTest,ExramTest); // 测试EXRAM是否正确读写
//	free ((void*)ExramTest);
	
//	UDEBUG("LVGL起始地址LV_MEM_ADR:0x%lX",(unsigned long)LV_MEM_ADR);
//	UDEBUG("LVGL结束地址LV_MEM_ADR:0x%lX",(unsigned long)(LV_MEM_ADR+LV_MEM_SIZE));

	FRONT_COLOR=RED;//设置字体为红色 
	BACK_COLOR=BLACK;

  LCD_Display_Dir(0);
	LCD_Clear(BLACK);	
	
	GPIOF_Pin0_5_BeUsedFor_OV7670();
	while(OV7670_Init()&&i!=0xff)i++;
	i=OV7670_Init();
	if (0 == i) UDEBUG("OV7670初始化完成\r\n");
	else	UDEBUG("OV7670初始化失败\r\n");
 
	delay_ms(1500);	 
	OV7670_Light_Mode(0);
	OV7670_Color_Saturation(2);
	OV7670_Brightness(2);
	OV7670_Contrast(2);
 	OV7670_Special_Effects(0);
	
	OV7670_CS=0;	
	OV7670_Window_Set(12,176,240,320);	//设置窗口	
//	OV7670_Window_Set(0,0,240,320);	//设置窗口	
	LCD_Clear(BLACK);
	
	GPIOF_Pin0_5_BeUsedFor_SRAMEX();
		
	//MemInfo_UDEBUG(); //输出内存信息
		
	xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* 任务入口函数 */
                        (const char*    )"AppTaskCreate",/* 任务名字 */
                        (uint16_t       )512,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )1, /* 任务的优先级 */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* 任务控制块指针 */ 
  /* 启动任务调度 */           
  if(pdPASS == xReturn)
	{
		UDEBUG("开始调度\r\n");
    vTaskStartScheduler();   /* 启动任务，开启调度 */	 
	}
  else
    return -1;  
    while(1);
}
