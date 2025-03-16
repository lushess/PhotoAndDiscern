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
*                             ��������
*************************************************************************
*/
static void AppTaskCreate(void);/* ���ڴ������� */
static void lv_task(void); /*lvgl����ˢ��*/
static void LEDsysRunning(void);//����ָʾϵͳ��������

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
    UDEBUG("����LEDsysRunning����ɹ�!\r\n");
	 else UDEBUG("����LEDsysRunning����ʧ��,�������:%d\r\n",(int)xReturn);
			 
	 xReturn = xTaskCreate((TaskFunction_t )camera_refresh,  /* ������ں��� */
													(const char*    )"camera_refresh",/* �������� */
													(uint16_t       )0x400,  /* ����ջ��С */
													(void*          )NULL,/* ������ں������� */
													(UBaseType_t    )7, /* ��������ȼ� */
													(TaskHandle_t*  )&camera_refresh_Handle);/* ������ƿ�ָ�� */ 		
	 if(pdPASS == xReturn)
			UDEBUG("����camera_refresh����ɹ�!\r\n");
	 else UDEBUG("����camera_refresh����ʧ��,�������:%d\r\n",(int)xReturn);
	 
	 xReturn = xTaskCreate((TaskFunction_t )photo,  /* ������ں��� */
                        (const char*    )"photo",/* �������� */
                        (uint16_t       )0x400,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )11, /* ��������ȼ� */
                        (TaskHandle_t*  )&photo_Handle);/* ������ƿ�ָ�� */ 
		
  if(pdPASS == xReturn)
    UDEBUG("����photo����ɹ�!\r\n");
	else UDEBUG("����photo����ʧ��,�������:%d\r\n",(int)xReturn);
	
	 xReturn = xTaskCreate((TaskFunction_t )discern,  /* ������ں��� */
                        (const char*    )"discern",/* �������� */
                        (uint16_t       )0x400,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )10, /* ��������ȼ� */
                        (TaskHandle_t*  )&discern_Handle);/* ������ƿ�ָ�� */ 
		
  if(pdPASS == xReturn)
    UDEBUG("����discern����ɹ�!\r\n");
	else UDEBUG("����discern����ʧ��,�������:%d\r\n",(int)xReturn);
		
	xReturn = xTaskCreate((TaskFunction_t )lv_task,  /* ������ں��� */
				(const char*    )"lv_task",/* �������� */
				(uint16_t       )0x200,  /* ����ջ��С */
				(void*          )NULL,/* ������ں������� */
				(UBaseType_t    )30, /* ��������ȼ� */
				(TaskHandle_t*  )&LV_Task_Handle);/* ������ƿ�ָ�� */ 
	if(pdPASS == xReturn)
    UDEBUG("����lv_task����ɹ�!\r\n");	

	
	lv_init();
	UDEBUG("lvgl��ʼ���ɹ�!\r\n");
	lv_port_disp_init();
	UDEBUG("lvgl��Ļ��ʼ���ɹ�!\r\n");
	lv_port_indev_init();
	UDEBUG("lvgl�豸��ʼ���ɹ�!\r\n");
	lv_port_fs_init();
	UDEBUG("lvgl�ļ�ϵͳ��ʼ���ɹ�!\r\n");
	App_Init_Wrapper();

  vTaskDelete(NULL); //ɾ��AppTaskCreate����
	UDEBUG("����������ɾ��!\r\n");
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
		xTaskNotifyGive(key_scan_task_Handle); // ����ɨ������
	  lv_task_handler(); //lvglˢ��
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

int main()
{
	u8 i=0;
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
	
	SysTick_Init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);  //�ж����ȼ�����4��4λ������ռ���ȼ����15
//	USART1_Init();
	LED_Init();
	TFTLCD_Init();			//LCD��ʼ��
	KEY_Init();
	EN25QXX_Init();				//��ʼ��EN25Q128
	
//	char *ExramTest = malloc(sizeof("EXRAM����"));
//	if (NULL == ExramTest) UDEBUG("EXRAM���ڴ����ʧ��");
//	snprintf(ExramTest, sizeof("EXRAM����"), "EXRAM����");
//	UDEBUG("EXRAM���Ե�ַ:%p;����:%s",ExramTest,ExramTest); // ����EXRAM�Ƿ���ȷ��д
//	free ((void*)ExramTest);
	
//	UDEBUG("LVGL��ʼ��ַLV_MEM_ADR:0x%lX",(unsigned long)LV_MEM_ADR);
//	UDEBUG("LVGL������ַLV_MEM_ADR:0x%lX",(unsigned long)(LV_MEM_ADR+LV_MEM_SIZE));

	FRONT_COLOR=RED;//��������Ϊ��ɫ 
	BACK_COLOR=BLACK;

  LCD_Display_Dir(0);
	LCD_Clear(BLACK);	
	
	GPIOF_Pin0_5_BeUsedFor_OV7670();
	while(OV7670_Init()&&i!=0xff)i++;
	i=OV7670_Init();
	if (0 == i) UDEBUG("OV7670��ʼ�����\r\n");
	else	UDEBUG("OV7670��ʼ��ʧ��\r\n");
 
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
		
	//MemInfo_UDEBUG(); //����ڴ���Ϣ
		
	xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* ������ں��� */
                        (const char*    )"AppTaskCreate",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )1, /* ��������ȼ� */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* ������ƿ�ָ�� */ 
  /* ����������� */           
  if(pdPASS == xReturn)
	{
		UDEBUG("��ʼ����\r\n");
    vTaskStartScheduler();   /* �������񣬿������� */	 
	}
  else
    return -1;  
    while(1);
}
