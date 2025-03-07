#include "user_task.h"
#include "Config/Config.h"
//#include "lvgl/lvgl.h"
#include "font_show.h"
#include <stdlib.h>
#include "ov7670.h"
#include "tftlcd.h"
#include "exti.h"
#include "picture_tool.h"
#include "characterdiscern.h"
#include "ean13_barcode.h"
//#include "Utils/lv_ext/lv_obj_ext_func.h"
//#include "Utils/lv_ext/lv_anim_timeline_wrapper.h"
#include "FATFS_APP/fatfs_app.h"
#include "usart.h"
//#include "Utils/PageManager/PageManager.h"

TaskHandle_t camera_refresh_Handle = NULL;
TaskHandle_t discern_Handle = NULL;
TaskHandle_t photo_Handle = NULL;

//__align(32) PageManager::Switch_Info_t *swinfo __INRAM;
extern FATFS *fs[_VOLUMES];  
extern FIL *file;	 
extern FIL *ftemp;	 
extern UINT br,bw;
extern FILINFO *fileinfo;
extern DIR *dir;
extern u8 fs_ready_status[_VOLUMES];//����״̬��Ϊ0��ʾ����
extern u8 *fatbuf;//���ݻ���������СΪ1024���ֽ�

//#define ANIM_DEF(start_time, obj, attr, start, end) \
//    {start_time, obj, LV_ANIM_EXEC(attr), start, end, 500, lv_anim_path_ease_out, true}

//#define ANIM_OPA_DEF(start_time, obj) \
//    ANIM_DEF(start_time, obj, opa_scale, LV_OPA_COVER,LV_OPA_TRANSP) //͸���ȴ�255��0

void camera_refresh(void)
{
	u32 j;
	u16 i;
 	u16 color;
	u32 current_event=0,last_event=ENREFRESH_EVENT;

  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
	while(1)
	{
    xReturn = xTaskNotifyWait(0x0,		//���뺯����ʱ���������bit
                            0xffffffffUL,	   //�˳�������ʱ���������bit
                            &current_event,		  //��������ֵ֪ͨ                    
                            portMAX_DELAY);	//����ʱ��
		if( pdTRUE == xReturn )
		{ 
				last_event|=current_event;
				if(current_event&DISREFRESH_EVENT) 
				{
					last_event=0;
					ov_sta=0;
					EXTI_ITConfig(EXTI_Line7,DISABLE);
				}
				else if(current_event&ENREFRESH_EVENT&&ov_sta==0)
				{
				  EXTI_ITConfig(EXTI_Line7,ENABLE);
				}
				
				if(last_event&(ENREFRESH_EVENT|REFRESH_EVENT)&&ov_sta == 0xff)
				{
						last_event&=~REFRESH_EVENT;
					
				    LCD_Display_Dir(1);
						LCD_Set_Window(0,(tftlcd_data.height-240)/2,320-1,240-1);//����ʾ�������õ���Ļ����
					
						EXTI_ITConfig(EXTI_Line7,DISABLE);
						GPIOF_Pin0_5_BeUsedFor_OV7670();//����SRAMEX��OV7670��FIFO�в���GPIO���ã�����GPIOF��Pin0_5����FIFO

						OV7670_RRST=0;				//��ʼ��λ��ָ�� 
						OV7670_RCK_L;
						OV7670_RCK_H;
						OV7670_RCK_L;
						OV7670_RRST=1;				//��λ��ָ����� 
						OV7670_RCK_H;
						for(j=0;j<76800;j++)   //���ַ�ʽ�����TFT�ڲ���ʾ������ƼĴ���ֵ �ٶȽϿ�
						{
							OV7670_RCK_L;
							color=GPIOF->IDR&0XFF;	//������
							OV7670_RCK_H; 
							color<<=8;  
							OV7670_RCK_L;
							color|=GPIOF->IDR&0XFF;	//������
							OV7670_RCK_H; 
							LCD_WriteData_Color(color); 
						}   							  	

					  LCD_Display_Dir(0);	
            ov_sta=0;						
					  GPIOF_Pin0_5_BeUsedFor_SRAMEX();//����SRAMEX��OV7670��FIFO�в���GPIO���ã�����GPIOF��Pin0_5����SRAMEX
					  EXTI_ITConfig(EXTI_Line7,ENABLE);
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
		taskHead:
		ulTaskNotifyTake(pdTRUE,portMAX_DELAY); //�ȴ�����֪ͨ
		LCD_Display_Dir(0);
    LCD_Set_Window(0,0,240-1,320-1);		

    path = (char *)Memalloc(64*sizeof(char));
		
		snprintf(path,64*sizeof(char),"%s%s%d",SDCARD_PHOTO_PATH_DEFAULT,"/",photocount);
		if(f_open(file,path,FA_CREATE_ALWAYS|FA_WRITE) == FR_OK)
		{
		   f_close(file);
			 f_open(file,path,FA_OPEN_ALWAYS|FA_WRITE);
		}
		else 
		{
			snprintf(path,64*sizeof(char),"%s%s%d",FLASH_PHOTO_PATH_DEFAULT,"/",photocount);
			if(f_open(file,path,FA_CREATE_ALWAYS|FA_WRITE) == FR_OK)
			{
				f_close(file);
				f_open(file,path,FA_OPEN_ALWAYS|FA_WRITE);
			}
			else {
				UDEBUG("�ļ���ʧ��");
				Memfree((void *)path);
				goto taskHead;
			}
		}
		
    Memfree((void *)path);

		f_write(file,&bmpinfo,sizeof(bmpinfo_t),&bw);
		if(bw) UDEBUG("���ļ�д��BMPͷ�ļ���Ϣ,���ݹ�%d�ֽ�",bw);

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

void discern(void)//ʶ��
{	
	u32 current_event=0;
	BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
	
////	LV_FONT_DECLARE(font_discern);
////	
////	lv_obj_t * bar = lv_bar_create(lv_scr_act());
////  lv_obj_set_size(bar,80, 2);
////  lv_obj_align(bar,LV_ALIGN_CENTER,-74,140);
////  lv_obj_set_style_bg_color(bar,lv_color_hex(0xa01eff),LV_PART_INDICATOR);
////  lv_bar_set_value(bar,0,LV_ANIM_OFF);
////  lv_obj_set_style_anim_time(bar,200,0);
////  lv_obj_set_style_opa(bar,LV_OPA_0,0);
////	lv_obj_refresh_style(bar,0,LV_STYLE_BG_OPA);//ˢ����ʽ
////	
////  lv_obj_t * label = lv_label_create(lv_scr_act());
////  lv_obj_set_size(label,50,12);
////  lv_obj_align_to(label,bar,LV_ALIGN_OUT_TOP_LEFT,0,-4);
////  lv_obj_set_style_text_font(label,&font_discern,0);
////  lv_obj_set_style_text_color(label,lv_color_hex(0x804040),0);
////  lv_label_set_text(label,"����ʶ��...");
////  lv_obj_set_style_opa(label,LV_OPA_0,0);
////	lv_obj_refresh_style(label,0,LV_STYLE_BG_OPA);//ˢ����ʽ
	
//	lv_anim_timeline_t* anim_timeline = lv_anim_timeline_create(); //��������
//	
//	lv_anim_timeline_wrapper_t wrapper[] =
//	{
//			ANIM_OPA_DEF(0,bar),
//			ANIM_OPA_DEF(0,label),
//			LV_ANIM_TIMELINE_WRAPPER_END
//	};
//	
//  lv_anim_timeline_add_wrapper(anim_timeline, wrapper);

	imageBufStat_Init(); //��ʼ��image�ڴ滺����״̬�ṹ��;
	while(1)
	{
		xReturn = xTaskNotifyWait(0xffffffffUL,		//���뺯����ʱ���������bit
                            0xffffffffUL,	   //�˳�������ʱ���������bit
                            &current_event,		  //��������ֵ֪ͨ                    
                            portMAX_DELAY);	//����ʱ��
		
		extern volatile uint32_t lastkey;
		lastkey = NULL;
		if(pdTRUE == xReturn)
		{
				GPIOF_Pin0_5_BeUsedFor_SRAMEX();

				vu8 *image=(vu8 *)Memalloc(240*320*sizeof(vu8)),*Tmpimage=image;
				if(image!=NULL)	UDEBUG("image�ڴ����ɹ�,���ַΪ:%p\r\n",image);		
				else UDEBUG("image�ڴ����ʧ��\r\n");			
				vu16 character[CharacterNumToCapture];
				vu16 *Tmpcharacter=character;
			
			  /* ��ʾUI */			  
//			  lv_obj_set_style_opa(bar,LV_OPA_100,0);
//			  lv_obj_set_style_opa(label,LV_OPA_100,0);
//			  lv_obj_refresh_style(bar,0,LV_STYLE_BG_OPA);//ˢ����ʽ
//			  lv_obj_refresh_style(label,0,LV_STYLE_BG_OPA);//ˢ����ʽ
////			  lv_bar_set_value(bar,0,LV_ANIM_OFF);
////			  lv_obj_fade_in(bar,500,0);
////			  lv_obj_fade_in(label,500,0);
//			  lv_anim_timeline_set_reverse(anim_timeline,true); //������ת
//        lv_anim_timeline_start(anim_timeline);
			
				CapLCDToGray(image);                    //��ȡLCD�������洢��SRAM
				UDEBUG("��ȡLCD��ɣ���ת��Ϊ�Ҷ�ͼ");
				u8 MedianfilterCout;
				MedianfilterCout=3;
				while(MedianfilterCout--)
				{
//					MedianFilter(image);                   //�Ҷ�ͼ��ֵ�˲�
				}
				ShowGrayToLCD(image);                  //��ӡ�Ҷ�ͼ��LCD��
				GrayEven(image); 											//�Ҷ�ͼ���Ȼ�
				GrayToBinary(image);                    //�Ҷ�ͼת��Ϊ��ֵͼ 
		//		MedianfilterCout=3;
		//		while(MedianfilterCout--)
		//		{
		//			image=Tmpimage;
		//			MedianFilter(image);                   //��ֵͼ��ֵ�˲�
		//		}		
		//		MedianfilterCout=3;
		//		while(MedianfilterCout--)
		//		{
		//			image=Tmpimage;
		//			CloseOperation(image);                 //������                 
		//		}		

				ShowBinaryToLCD(image);               //�Ѷ�ֵͼ��ӡ��LCD��
//        lv_bar_set_value(bar,80,LV_ANIM_ON);
				if(current_event&CHARA_DISCERN)
				{
					CharacterCompareFromExflash(image,character); //���ⲿFLASH��ȡ(16*16)��ģ���бȶԣ��洢�ַ����뵽character����
					//LCD_ShowFont12Char_FromBuf(10,360, (u8*)character,CharacterNumToCapture);

				  /* ����UI */
//					lv_obj_set_style_opa(bar,LV_OPA_0,0);
//					lv_obj_set_style_opa(label,LV_OPA_0,0);
//					lv_obj_refresh_style(bar,0,LV_STYLE_BG_OPA);//ˢ����ʽ
//					lv_obj_refresh_style(label,0,LV_STYLE_BG_OPA);//ˢ����ʽ	
////          lv_obj_fade_out(bar,500,0);
////			    lv_obj_fade_out(label,500,0);					
//					lv_anim_timeline_set_reverse(anim_timeline,false); //��������
//          lv_anim_timeline_start(anim_timeline);
					UDEBUG("ʶ��ɹ����ַ�Ϊ:%s\r\n",(char*)character);
				}	
				else if(current_event&EAN13_DISCERN)
				{
				  //TODO:
				
				}			

				UDEBUG("image�ڴ����ͷ�,���ַΪ:%p\r\n",image);
				Memfree((void *)image);
				
				//MemInfo_UDEBUG(); //����ڴ���Ϣ
				
        //GPIOF_Pin0_5_BeUsedFor_OV7670();//����SRAMEX��OV7670��FIFO�в���GPIO���ã�����GPIOF��Pin0_5����FIFO
				xTaskNotify((TaskHandle_t	)camera_refresh_Handle,
                        (uint32_t	)ENREFRESH_EVENT,
                        (eNotifyAction)eSetBits);
		}							
	}
}


