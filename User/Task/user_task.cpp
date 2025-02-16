#include "user_task.h"
#include "lvgl/lvgl.h"
#include "font_show.h"
#include <stdlib.h>
#include "ov7670.h"
#include "tftlcd.h"
#include "exti.h"
#include "picture_tool.h"
#include "characterdiscern.h"
#include "ean13_barcode.h"
#include "Utils/lv_ext/lv_obj_ext_func.h"
#include "Utils/lv_ext/lv_anim_timeline_wrapper.h"
#include "FATFS_APP/fatfs_app.h"
#include "Config/Config.h"
#include "Utils/PageManager/PageManager.h"

TaskHandle_t camera_refresh_Handle = NULL;
TaskHandle_t discern_Handle = NULL;
TaskHandle_t photo_Handle = NULL;

bmpinfo_t bmpinfo __INRAM = {NULL};
__align(32) PageManager::Switch_Info_t *swinfo __INRAM;
extern u8 ov_sta;
extern FATFS *fs[_VOLUMES];  
extern FIL *file;	 
extern FIL *ftemp;	 
extern UINT br,bw;
extern FILINFO *fileinfo;
extern DIR *dir;
extern u8 fs_ready_status[_VOLUMES];//磁盘状态，为0表示就绪
extern u8 *fatbuf;//数据缓存区，大小为1024个字节

#define ANIM_DEF(start_time, obj, attr, start, end) \
    {start_time, obj, LV_ANIM_EXEC(attr), start, end, 500, lv_anim_path_ease_out, true}

#define ANIM_OPA_DEF(start_time, obj) \
    ANIM_DEF(start_time, obj, opa_scale, LV_OPA_COVER,LV_OPA_TRANSP) //透明度从255到0

void camera_refresh(void)
{
	u32 j;
	u16 i;
 	u16 color;
	u32 r_event=0,last_event=0;

  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
	while(1)
	{
    xReturn = xTaskNotifyWait(0x0,		//进入函数的时候不清除任务bit
                            ULONG_MAX,	   //退出函数的时候清除所有bit
                            &r_event,		  //保存任务通知值                    
                            portMAX_DELAY);	//阻塞时间
		if( pdTRUE == xReturn )
		{ 
				last_event|=r_event;
				if(r_event==DISREFRESH_EVENT) 
				{
					last_event=0;
					ov_sta=0;
					EXTI_ITConfig(EXTI_Line7,DISABLE);
				}
				else if(r_event==ENREFRESH_EVENT&&ov_sta==0)
				{
				  EXTI_ITConfig(EXTI_Line7,ENABLE);
				}
				
				if(last_event==(ENREFRESH_EVENT|REFRESH_EVENT)&&ov_sta!=0)
				{
						last_event&=~REFRESH_EVENT;
					
				    LCD_Display_Dir(1);
						//LCD_Set_Window((tftlcd_data.width-320)/2,(tftlcd_data.height-240)/2,320,240-1);//将显示区域设置到屏幕中央
					  taskENTER_CRITICAL();           //进入临界区
						LCD_Set_Window(0,(tftlcd_data.height-240)/2,320-1,240-1);//将显示区域设置到屏幕中央
						
						GPIOF_Pin0_5_BeUsedFor_OV7670();//由于SRAMEX和OV7670的FIFO有部分GPIO共用，设置GPIOF，Pin0_5用于FIFO

						OV7670_RRST=0;				//开始复位读指针 
						OV7670_RCK_L;
						OV7670_RCK_H;
						OV7670_RCK_L;
						OV7670_RRST=1;				//复位读指针结束 
						OV7670_RCK_H;
						for(j=0;j<76800;j++)   //此种方式需清楚TFT内部显示方向控制寄存器值 速度较快
						{
							OV7670_RCK_L;
							color=GPIOF->IDR&0XFF;	//读数据
							OV7670_RCK_H; 
							color<<=8;  
							OV7670_RCK_L;
							color|=GPIOF->IDR&0XFF;	//读数据
							OV7670_RCK_H; 
							LCD_WriteData_Color(color); 
						}   							  	

					  LCD_Display_Dir(0);	
            ov_sta=0;						
					  GPIOF_Pin0_5_BeUsedFor_SRAMEX();
					  EXTI_ITConfig(EXTI_Line7,ENABLE);
						taskEXIT_CRITICAL();            //退出临界区
			  }
		 }
  }
}

void photo(void)
{
	u8 r,g,b;
	char* path;
	u8 *pfatbuf = fatbuf;
	u16 i,j,Imagetmp;
	u32 photocount =0;
	FRESULT res;
	
#if LV_COLOR_DEPTH == 24
	bmpinfo.header.size = 230456;
	bmpinfo.infoheader.biBitCount = 24;
	bmpinfo.infoheader.biSizeImage = 230400;
#endif /*LV_COLOR_DEPTH == 24*/
#if LV_COLOR_DEPTH == 16
	bmpinfo.header.size = 153656;	
	bmpinfo.infoheader.biBitCount = 16;
	bmpinfo.infoheader.biSizeImage = 153600;
#endif /*LV_COLOR_DEPTH == 16*/
	bmpinfo.header.type = 0x424d;
	bmpinfo.header.Offset = 56;
	bmpinfo.infoheader.biSize = 40;
	bmpinfo.infoheader.biWidth = 240;
	bmpinfo.infoheader.biHeight = 320;
	bmpinfo.infoheader.biPlanes = 1;		
	bmpinfo.infoheader.biXPelsPerMeter = 240;
	bmpinfo.infoheader.biYPelsPerMeter = 320;

	while(1)
	{
		ulTaskNotifyTake(pdTRUE,portMAX_DELAY); //等待任务通知
		LCD_Display_Dir(0);
    LCD_Set_Window(0,0,240-1,320-1);		
#ifdef CONFIG_LVGL_USE_MALLOC
    path = (char *)malloc(sizeof(FIL));	
#else
		path = (char *)lv_malloc(128);	 
#endif /*CONFIG_LVGL_USE_MALLOC*/
		sprintf(path,"%s%s%d",SDCARD_PHOTO_PATH_DEFAULT,"/",photocount);
		if(f_open(file,path,FA_CREATE_ALWAYS|FA_WRITE) == FR_OK)
		{
		   f_close(file);
			 f_open(file,path,FA_OPEN_ALWAYS|FA_WRITE);
		}
		else 
		{
			sprintf(path,"%s%s%d",FLASH_PHOTO_PATH_DEFAULT,"/",photocount);
			if(f_open(file,path,FA_CREATE_ALWAYS|FA_WRITE) == FR_OK)
			{
				f_close(file);
				f_open(file,path,FA_OPEN_ALWAYS|FA_WRITE);
			} 
		}
#ifdef CONFIG_LVGL_USE_MALLOC
    free((void *)path);
#else
		lv_free((void *)path);	 
#endif /*CONFIG_LVGL_USE_MALLOC*/
		LV_ASSERT_NULL(file);

		f_write(file,&bmpinfo,sizeof(bmpinfo_t),&bw);

		for(j=1;j<=320;j++)
		{
			pfatbuf = fatbuf;
			for(i=1;i<=240;i++)
			{				
				Imagetmp=TFTLCD->LCD_DATA;
			    LCD_WriteData_Color(TFTLCD->LCD_DATA);
				//Imagetmp=LCD_ReadPoint(i-1,j-1);
#if LV_COLOR_DEPTH == 24
				r=R8FromRGB565(Imagetmp);
				g=G8FromRGB565(Imagetmp);
				b=B8FromRGB565(Imagetmp);
	            
				*pfatbuf++=b;
				*pfatbuf++=g;
				*pfatbuf++=r;
			}
			f_write(file,fatbuf,3*240,&bw);
#endif /*LV_COLOR_DEPTH == 24*/
#if LV_COLOR_DEPTH == 16	            
				*pfatbuf++=(u8)(Imagetmp&0x00ff);
				*pfatbuf++=(u8)(Imagetmp>>8);
			}
			f_write(file,fatbuf,2*240,&bw);
#endif /*LV_COLOR_DEPTH == 16*/
		}
		f_close(file);
	}
}

void discern(void)//识别
{	
	u32 r_event=0;
	BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
	
	LV_FONT_DECLARE(font_discern);
	
	lv_obj_t * bar = lv_bar_create(lv_scr_act());
  lv_obj_set_size(bar,80, 2);
  lv_obj_align(bar,LV_ALIGN_CENTER,-74,140);
  lv_obj_set_style_bg_color(bar,lv_color_hex(0xa01eff),LV_PART_INDICATOR);
  lv_bar_set_value(bar,0,LV_ANIM_OFF);
  lv_obj_set_style_anim_time(bar,200,0);
  lv_obj_set_style_opa(bar,LV_OPA_0,0);
	lv_obj_refresh_style(bar,0,LV_STYLE_BG_OPA);//刷新样式
	
  lv_obj_t * label = lv_label_create(lv_scr_act());
  lv_obj_set_size(label,50,12);
  lv_obj_align_to(label,bar,LV_ALIGN_OUT_TOP_LEFT,0,-4);
  lv_obj_set_style_text_font(label,&font_discern,0);
  lv_obj_set_style_text_color(label,lv_color_hex(0x804040),0);
  lv_label_set_text(label,"正在识别...");
  lv_obj_set_style_opa(label,LV_OPA_0,0);
	lv_obj_refresh_style(label,0,LV_STYLE_BG_OPA);//刷新样式
	
//	lv_anim_timeline_t* anim_timeline = lv_anim_timeline_create(); //创建动画
//	
//	lv_anim_timeline_wrapper_t wrapper[] =
//	{
//			ANIM_OPA_DEF(0,bar),
//			ANIM_OPA_DEF(0,label),
//			LV_ANIM_TIMELINE_WRAPPER_END
//	};
//	
//  lv_anim_timeline_add_wrapper(anim_timeline, wrapper);

	while(1)
	{
		xReturn = xTaskNotifyWait(0x0,		//进入函数的时候不清除任务bit
                            0xfffff,	   //退出函数的时候清除所有bit
                            &r_event,		  //保存任务通知值                    
                            portMAX_DELAY);	//阻塞时间
		
		if(pdTRUE == xReturn)
		{
				GPIOF_Pin0_5_BeUsedFor_SRAMEX();
#ifdef CONFIG_LVGL_USE_MALLOC
				//my_mem_set(malloc_cortol.membase[SRAMEX], 0,256 *1024);//清空内存
			  //my_mem_init(SRAMEX); //初始化内存
		    //vu8 MedianfilterCout;
				vu8 *image=(vu8 *)malloc(76800),*Tmpimage=image;			
				vu16 *character=(vu16 *)malloc(CharacterNumToCapture),*Tmpcharacter=character;
#else
			  vu8 *image=(vu8 *)lv_malloc(76800),*Tmpimage=image;			
				vu16 *character=(vu16 *)lv_malloc(CharacterNumToCapture),*Tmpcharacter=character;
#endif /*CONFIG_LVGL_USE_MALLOC*/
			
			  /* 显示UI */			  
//			  lv_obj_set_style_opa(bar,LV_OPA_100,0);
//			  lv_obj_set_style_opa(label,LV_OPA_100,0);
//			  lv_obj_refresh_style(bar,0,LV_STYLE_BG_OPA);//刷新样式
//			  lv_obj_refresh_style(label,0,LV_STYLE_BG_OPA);//刷新样式
			  lv_bar_set_value(bar,0,LV_ANIM_OFF);
			  lv_obj_fade_in(bar,500,0);
			  lv_obj_fade_in(label,500,0);
//			  lv_anim_timeline_set_reverse(anim_timeline,true); //动画翻转
//        lv_anim_timeline_start(anim_timeline);
			
				CapLCDToGray(image);                    //读取LCD屏，并存储到SRAM
		//		MedianfilterCout=3;
		//		while(MedianfilterCout--)
		//		{
		//			image=Tmpimage;
		//			Medianfilter(image);                   //灰度图中值滤波
		//		}
				ShowGrayToLCD(image);                  //打印灰度图到LCD屏
				GrayEven(image); 
				GrayToBinary(image);                    //灰度图转化为二值图 
		//		MedianfilterCout=3;
		//		while(MedianfilterCout--)
		//		{
		//			image=Tmpimage;
		//			Medianfilter(image);                   //二值图中值滤波
		//		}		
		//		MedianfilterCout=3;
		//		while(MedianfilterCout--)
		//		{
		//			image=Tmpimage;
		//			CloseOperation(image);                 //开运算                 
		//		}		

				ShowBinaryToLCD(image);               //把二值图打印到LCD屏
        lv_bar_set_value(bar,80,LV_ANIM_ON);
				if(r_event==CHARA_DISCERN)
				{
					CharacterCompareFromExflash(image,character); //从外部FLASH读取(16*16)字模进行比对，存储字符编码到character数组
					//LCD_ShowFont12Char_FromBuf(10,360, (u8*)character,CharacterNumToCapture);

				  /* 隐藏UI */
//					lv_obj_set_style_opa(bar,LV_OPA_0,0);
//					lv_obj_set_style_opa(label,LV_OPA_0,0);
//					lv_obj_refresh_style(bar,0,LV_STYLE_BG_OPA);//刷新样式
//					lv_obj_refresh_style(label,0,LV_STYLE_BG_OPA);//刷新样式	
          lv_obj_fade_out(bar,500,0);
			    lv_obj_fade_out(label,500,0);					
//					lv_anim_timeline_set_reverse(anim_timeline,false); //动画正序
//          lv_anim_timeline_start(anim_timeline);
				}	
				else if(r_event==EAN13_DISCERN)
				{
				  //TODO:
				
				}							
#ifdef CONFIG_LVGL_USE_MALLOC
			free((void *)image);
		    free((void *)character);
#else
			lv_free((void *)image);
		    lv_free((void *)character);
#endif /*CONFIG_LVGL_USE_MALLOC*/				
				
        //GPIOF_Pin0_5_BeUsedFor_OV7670();//由于SRAMEX和OV7670的FIFO有部分GPIO共用，设置GPIOF，Pin0_5用于FIFO
				xTaskNotify((TaskHandle_t	)camera_refresh_Handle,
                        (uint32_t	)ENREFRESH_EVENT,
                        (eNotifyAction)eSetBits);
		}							
	}
}

void TaskSwitchTo_cpp(PageManager *_Manager)
{
    BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
    while(1)
    {
        xReturn = xTaskNotifyWait(
														0x0,
														ULONG_MAX,
														(uint32_t *)swinfo,
														portMAX_DELAY
												);
        if(xReturn == pdPASS)
					_Manager->SwitchTo(swinfo->newNode, swinfo->isEnterAct, swinfo->stash);
    }     
}

extern "C" void TaskSwitchTo(void *_Manager)
{
	return TaskSwitchTo_cpp((PageManager *)_Manager);
}


