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

/******************************* ȫ�ֱ������� ************************************/
extern u8 ov_sta;	//��exit.c���涨��
extern u8 ov_frame;	//��time.c���涨��


/**************************** ������ ********************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
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
*                             ��������
*************************************************************************
*/
static void AppTaskCreate(void);/* ���ڴ������� */
static void lv_task(void); /*lvgl����ˢ��*/
extern void camera_refresh(void);//����ˢ����Ļ
extern void discern(void);//����ʶ��
static void LEDsysRunning(void);//����ָʾϵͳ��������


/********************************** �ں˶����� *********************************/
/*
 * �ź�������Ϣ���У��¼���־�飬�����ʱ����Щ�������ں˵Ķ���Ҫ��ʹ����Щ�ں�
 * ���󣬱����ȴ����������ɹ�֮��᷵��һ����Ӧ�ľ����ʵ���Ͼ���һ��ָ�룬������
 * �ǾͿ���ͨ��������������Щ�ں˶���
 *
 * �ں˶���˵���˾���һ��ȫ�ֵ����ݽṹ��ͨ����Щ���ݽṹ���ǿ���ʵ��������ͨ�ţ�
 * �������¼�ͬ���ȸ��ֹ��ܡ�������Щ���ܵ�ʵ��������ͨ��������Щ�ں˶���ĺ���
 * ����ɵ�
 * 
 */

static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  
  taskENTER_CRITICAL();           //�����ٽ���	
		
	xReturn = xTaskCreate((TaskFunction_t )LEDsysRunning,  /* ������ں��� */
                        (const char*    )"LEDsysRunning",/* �������� */
                        (uint16_t       )0x80,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )2, /* ��������ȼ� */
                        (TaskHandle_t*  )&LEDsysRunning_Handle);/* ������ƿ�ָ�� */ 
	 if(pdPASS == xReturn)
    printf("����LEDsysRunning����ɹ�!\r\n");
	 
	xReturn = xTaskCreate((TaskFunction_t )camera_refresh,  /* ������ں��� */
                        (const char*    )"camera_refresh",/* �������� */
                        (uint16_t       )0x100,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )1, /* ��������ȼ� */
                        (TaskHandle_t*  )&camera_refresh_Handle);/* ������ƿ�ָ�� */ 		
  if(pdPASS == xReturn)
    printf("����camera_refresh����ɹ�!\r\n");

	  xReturn = xTaskCreate((TaskFunction_t )photo,  /* ������ں��� */
                        (const char*    )"photo",/* �������� */
                        (uint16_t       )0x800,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )11, /* ��������ȼ� */
                        (TaskHandle_t*  )&photo_Handle);/* ������ƿ�ָ�� */ 
		
  if(pdPASS == xReturn)
    printf("����photo����ɹ�!\r\n");
	
	 xReturn = xTaskCreate((TaskFunction_t )discern,  /* ������ں��� */
                        (const char*    )"discern",/* �������� */
                        (uint16_t       )0x800,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )10, /* ��������ȼ� */
                        (TaskHandle_t*  )&discern_Handle);/* ������ƿ�ָ�� */ 
		
  if(pdPASS == xReturn)
    printf("����discern����ɹ�!\r\n");
	
//	xReturn = xTaskCreate((TaskFunction_t )lv_task,  /* ������ں��� */
//				(const char*    )"lv_task",/* �������� */
//				(uint16_t       )0x200,  /* ����ջ��С */
//				(void*          )NULL,/* ������ں������� */
//				(UBaseType_t    )30, /* ��������ȼ� */
//				(TaskHandle_t*  )&LV_Task_Handle);/* ������ƿ�ָ�� */ 
//	if(pdPASS == xReturn)
//    printf("����lv_task����ɹ�!\r\n");	

	
	lv_init();
	printf("lvgl��ʼ���ɹ�!\r\n");
	lv_port_disp_init();
	printf("lvgl��Ļ��ʼ���ɹ�!\r\n");
//	lv_port_indev_init();
//	printf("lvgl�豸��ʼ���ɹ�!\r\n");
//	lv_port_fs_init();
//	printf("lvgl�ļ�ϵͳ��ʼ���ɹ�!\r\n");
	App_Init_Wrapper();

  vTaskDelete(NULL); //ɾ��AppTaskCreate����
	
  taskEXIT_CRITICAL();            //�˳��ٽ���
}

static void lv_task(void)
{
	TickType_t xLastWakeTime;
	const TickType_t xPeriod = pdMS_TO_TICKS( 5 );
	xLastWakeTime = xTaskGetTickCount();
	while(1)
	{		
		xTaskDelayUntil(&xLastWakeTime,xPeriod);
		xTaskNotifyGive(lv_key_scan_Handle); // ����ɨ������
	  lv_task_handler();
	}
}

static void LEDsysRunning(void)//����ָʾϵͳ��������
{
	while(1)
	{
	 led1=!led1;
	 vTaskDelay(1000);
	}
}

//const u8*LMODE_TBL[5]={"Auto","Sunny","Cloudy","Office","Home"};
//const u8*EFFECTS_TBL[7]={"Normal","Negative","B&W","Redish","Greenish","Bluish","Antique"};	//7����Ч 

int main()
{
	u8 i=0;
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
	
	SysTick_Init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);  //�ж����ȼ����� ��4�飬4λ����ǿռ���ȼ����15
	LED_Init();
	USART1_Init(9600);
	TFTLCD_Init();			//LCD��ʼ��
	KEY_Init();
	EN25QXX_Init();				//��ʼ��EN25Q128
	
	FRONT_COLOR=RED;//��������Ϊ��ɫ 
	BACK_COLOR=BLACK;

  LCD_Display_Dir(0);
	LCD_Clear(BLACK);	
	LCD_ShowFont12Char(10, 10, "���пƼ�");
	LCD_ShowFont12Char(10, 30, "www.prechin.net");    
	LCD_ShowFont12Char(10, 50, "����ͷӦ��--OV7670");
	i=OV7670_Init();
	printf("i=%d\n",i);
	while(OV7670_Init())//��ʼ��OV7670
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
	OV7670_Window_Set(12,176,240,320);	//���ô���	
//	OV7670_Window_Set(0,0,240,320);	//���ô���	
	LCD_Clear(BLACK);
	
	GPIOF_Pin0_5_BeUsedFor_SRAMEX();
	
	xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* ������ں��� */
                        (const char*    )"AppTaskCreate",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )1, /* ��������ȼ� */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* ������ƿ�ָ�� */ 
  /* ����������� */           
  if(pdPASS == xReturn)
	{
		printf("��ʼ����\r\n");
    vTaskStartScheduler();   /* �������񣬿������� */	 
	}
  else
    return -1;  
    while(1);
/* ���²������гɹ��󲻻�ִ��	*/
	/*
	while(1)
	{
		key=KEY_Scan(0);
		if(key)count=20;
		switch(key)
		{
			case KEY_UP:           //�ƹ�ģʽ����
				lightmode++;
				if(lightmode>4)lightmode=0;
				OV7670_Light_Mode(lightmode);
				sprintf((char*)sbuf,"%s",LMODE_TBL[lightmode]);
				break;
			case KEY_DOWN:         //���Ͷ�
				saturation++;
				if(saturation>4)saturation=0;
				OV7670_Color_Saturation(saturation);
				sprintf((char*)sbuf,"Saturation:%d",(char)saturation-2);
				break;	
			case KEY_LEFT:        //����
				brightness++;
				if(brightness>4)brightness=0;
				OV7670_Brightness(brightness);
				sprintf((char*)sbuf,"Brightness:%d",(char)brightness-2);
				break;
			case KEY_RIGHT:     //�Աȶ�
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
//		camera_refresh();//������ʾ
		i++;
		if(i%20==0)
		{
			led1=!led1;
		}
		//delay_ms(5);	
	}
	*/
}
