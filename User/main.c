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

/******************************* 全局变量声明 ************************************/
extern u8 ov_sta;	//在exit.c里面定义
extern u8 ov_frame;	//在time.c里面定义


/**************************** 任务句柄 ********************************/
/* 
 * 任务句柄是一个指针，用于指向一个任务，当任务创建好之后，它就具有了一个任务句柄
 * 以后我们要想操作这个任务都需要通过这个任务句柄，如果是自身的任务操作自己，那么
 * 这个句柄可以为NULL。
 */
static TaskHandle_t AppTaskCreate_Handle = NULL;
extern TaskHandle_t camera_refresh_Handle;
extern TaskHandle_t discern_Handle;
extern TaskHandle_t photo_Handle;
static TaskHandle_t LEDsysRunning_Handle = NULL;
static TaskHandle_t LV_Task_Handle = NULL;
extern TaskHandle_t lv_key_scan_Handle;

/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void AppTaskCreate(void);/* 用于创建任务 */
static void lv_task(void); /*lvgl任务刷新*/
extern void camera_refresh(void);//用于刷新屏幕
extern void discern(void);//用于识别
static void LEDsysRunning(void);//用于指示系统处于运行


/********************************** 内核对象句柄 *********************************/
/*
 * 信号量，消息队列，事件标志组，软件定时器这些都属于内核的对象，要想使用这些内核
 * 对象，必须先创建，创建成功之后会返回一个相应的句柄。实际上就是一个指针，后续我
 * 们就可以通过这个句柄操作这些内核对象。
 *
 * 内核对象说白了就是一种全局的数据结构，通过这些数据结构我们可以实现任务间的通信，
 * 任务间的事件同步等各种功能。至于这些功能的实现我们是通过调用这些内核对象的函数
 * 来完成的
 * 
 */

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
    printf("创建LEDsysRunning任务成功!\r\n");
	 
	xReturn = xTaskCreate((TaskFunction_t )camera_refresh,  /* 任务入口函数 */
                        (const char*    )"camera_refresh",/* 任务名字 */
                        (uint16_t       )0x100,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )1, /* 任务的优先级 */
                        (TaskHandle_t*  )&camera_refresh_Handle);/* 任务控制块指针 */ 		
  if(pdPASS == xReturn)
    printf("创建camera_refresh任务成功!\r\n");

	  xReturn = xTaskCreate((TaskFunction_t )photo,  /* 任务入口函数 */
                        (const char*    )"photo",/* 任务名字 */
                        (uint16_t       )0x800,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )11, /* 任务的优先级 */
                        (TaskHandle_t*  )&photo_Handle);/* 任务控制块指针 */ 
		
  if(pdPASS == xReturn)
    printf("创建photo任务成功!\r\n");
	
	 xReturn = xTaskCreate((TaskFunction_t )discern,  /* 任务入口函数 */
                        (const char*    )"discern",/* 任务名字 */
                        (uint16_t       )0x800,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )10, /* 任务的优先级 */
                        (TaskHandle_t*  )&discern_Handle);/* 任务控制块指针 */ 
		
  if(pdPASS == xReturn)
    printf("创建discern任务成功!\r\n");
	
//	xReturn = xTaskCreate((TaskFunction_t )lv_task,  /* 任务入口函数 */
//				(const char*    )"lv_task",/* 任务名字 */
//				(uint16_t       )0x200,  /* 任务栈大小 */
//				(void*          )NULL,/* 任务入口函数参数 */
//				(UBaseType_t    )30, /* 任务的优先级 */
//				(TaskHandle_t*  )&LV_Task_Handle);/* 任务控制块指针 */ 
//	if(pdPASS == xReturn)
//    printf("创建lv_task任务成功!\r\n");	

	
	lv_init();
	printf("lvgl初始化成功!\r\n");
	lv_port_disp_init();
	printf("lvgl屏幕初始化成功!\r\n");
//	lv_port_indev_init();
//	printf("lvgl设备初始化成功!\r\n");
//	lv_port_fs_init();
//	printf("lvgl文件系统初始化成功!\r\n");
	App_Init_Wrapper();

  vTaskDelete(NULL); //删除AppTaskCreate任务
	
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
		xTaskNotifyGive(lv_key_scan_Handle); // 键盘扫描任务
	  lv_task_handler();
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

//const u8*LMODE_TBL[5]={"Auto","Sunny","Cloudy","Office","Home"};
//const u8*EFFECTS_TBL[7]={"Normal","Negative","B&W","Redish","Greenish","Bluish","Antique"};	//7种特效 

int main()
{
	u8 i=0;
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
	
	SysTick_Init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);  //中断优先级分组 分4组，4位用于强占优先级最大15
	LED_Init();
	USART1_Init(9600);
	TFTLCD_Init();			//LCD初始化
	KEY_Init();
	EN25QXX_Init();				//初始化EN25Q128
	
	FRONT_COLOR=RED;//设置字体为红色 
	BACK_COLOR=BLACK;

  LCD_Display_Dir(0);
	LCD_Clear(BLACK);	
	LCD_ShowFont12Char(10, 10, "普中科技");
	LCD_ShowFont12Char(10, 30, "www.prechin.net");    
	LCD_ShowFont12Char(10, 50, "摄像头应用--OV7670");
	i=OV7670_Init();
	printf("i=%d\n",i);
	while(OV7670_Init())//初始化OV7670
	{
		LCD_ShowString(10,80,tftlcd_data.width,tftlcd_data.height,16,"OV7670 Error!");
		delay_ms(200);
		LCD_Fill(10,80,239,206,WHITE);
		delay_ms(200);
	}
 	LCD_ShowString(10,80,tftlcd_data.width,tftlcd_data.height,16,"OV7670 OK!     ");
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
	
	xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* 任务入口函数 */
                        (const char*    )"AppTaskCreate",/* 任务名字 */
                        (uint16_t       )512,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )1, /* 任务的优先级 */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* 任务控制块指针 */ 
  /* 启动任务调度 */           
  if(pdPASS == xReturn)
	{
		printf("开始调度\r\n");
    vTaskStartScheduler();   /* 启动任务，开启调度 */	 
	}
  else
    return -1;  
    while(1);
/* 以下部分运行成功后不会执行	*/
	/*
	while(1)
	{
		key=KEY_Scan(0);
		if(key)count=20;
		switch(key)
		{
			case KEY_UP:           //灯光模式设置
				lightmode++;
				if(lightmode>4)lightmode=0;
				OV7670_Light_Mode(lightmode);
				sprintf((char*)sbuf,"%s",LMODE_TBL[lightmode]);
				break;
			case KEY_DOWN:         //饱和度
				saturation++;
				if(saturation>4)saturation=0;
				OV7670_Color_Saturation(saturation);
				sprintf((char*)sbuf,"Saturation:%d",(char)saturation-2);
				break;	
			case KEY_LEFT:        //亮度
				brightness++;
				if(brightness>4)brightness=0;
				OV7670_Brightness(brightness);
				sprintf((char*)sbuf,"Brightness:%d",(char)brightness-2);
				break;
			case KEY_RIGHT:     //对比度
				contrast++;
				if(contrast>4)contrast=0;
				OV7670_Contrast(contrast);
				sprintf((char*)sbuf,"Contrast:%d",(char)contrast-2);
				break;
		}
		if(count)
		{
			count--;
			LCD_ShowString((tftlcd_data.width-240)/2+30,(tftlcd_data.height-320)/2+60,200,16,16,sbuf);
		}
//		camera_refresh();//更新显示
		i++;
		if(i%20==0)
		{
			led1=!led1;
		}
		//delay_ms(5);	
	}
	*/
}
