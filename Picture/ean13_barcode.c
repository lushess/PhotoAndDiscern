#include "picture_tool.h"
#include "ean13_barcode.h"
#include "tftlcd.h"
#include "flash.h"
#include "usart.h"
#include "Config/Config.h"
#ifdef CONFIG_LVGL_USE_MALLOC
    #include <stdlib.h>
		#include <string.h>
#else
		#include "lvgl/lvgl.h"	 
#endif /*CONFIG_LVGL_USE_MALLOC*/

void BarcodeAjust(vu8 *BarcodeImage)
{
  vu8 *pBarcodeImage=BarcodeImage;
	vu8 xdst=0,averxdst=0;
	vu16 i,j;
	vu32 dstaddup=0;
#ifdef CONFIG_LVGL_USE_MALLOC
	__IO int8_t *offset=(int8_t *)malloc(320),*poffset=offset;
#else
	__IO int8_t *offset=(int8_t *)lv_malloc(320),*poffset=offset;
#endif /*CONFIG_LVGL_USE_MALLOC*/
	for(j=1;j<=320;j++)
	{
		pBarcodeImage=BarcodeImage+240*(j-1); //ָ��ÿ���׵�ַ
    for(i=1;i<=240;i++)
		{
      if(*pBarcodeImage==0)
			{
			  dstaddup+=xdst;
			  break;  
			}
			pBarcodeImage++;
			xdst++;
		}	
    xdst=0;		
	}
	
  averxdst=dstaddup/320; //�õ�ÿ���׸������ص���߽��ƽ������
	
	for(j=1;j<=320;j++)
	{
		pBarcodeImage=BarcodeImage+240*(j-1); //ָ��ÿ���׵�ַ
    for(i=1;i<=240;i++)
		{
      if(*pBarcodeImage==0)
			{
			  dstaddup+=xdst;
			  break;  
			}
			pBarcodeImage++;
			xdst++;
		}	
		*poffset++=averxdst-xdst;
    xdst=0;		
	}
		
  pBarcodeImage=BarcodeImage;
	poffset=offset;
  ImageAjust_Horizontal(pBarcodeImage,poffset);

#ifdef CONFIG_LVGL_USE_MALLOC
	free((void *)offset);
#else
	lv_free((void *)offset);
#endif /*CONFIG_LVGL_USE_MALLOC*/
}

vu8 BarcodeDenoiseThreshold=10;
void BarcodeDenoise(vu8 *BarcodeImage) //�����뽵�봦��ͨ����Ҫ����бУ�������һ��
{
  vu8 *pBarcodeImage=BarcodeImage;
  vu16 *pProjective_H=Projective_H(pBarcodeImage),*pProjective_V=Projective_V(pBarcodeImage),\
	     *pProjective_Htmp=pProjective_H,*pProjective_Vtmp=pProjective_V;
  vu16 i,j;
  
	for(j=1;j<=320;j++)
	{
		pBarcodeImage=BarcodeImage+(j-1)*240;//ָ��ÿ���׵�ַ
	  if(BarcodeDenoiseThreshold>=*pProjective_Vtmp) //ͶӰС�ڻ������ֵ����ʾ��������ձ���
		{
		  for(i=1;i<=240;i++)*pBarcodeImage++=255;	
		}	
		pProjective_Vtmp++;
	}
	
	for(j=1;j<=240;j++)
	{
		pBarcodeImage=BarcodeImage+j-1;//ָ��ÿ���׵�ַ
	  if(BarcodeDenoiseThreshold>=*pProjective_Htmp) //ͶӰС�ڻ������ֵ����ʾ��������ձ���
		{
		  for(i=1;i<=320;i++)
			{
				*pBarcodeImage=255;
				pBarcodeImage+=240;  //����ָ��ָ�����ص�ͬһx�������һ������
			}				
		}	
		pProjective_Htmp++;
	}

#ifdef CONFIG_LVGL_USE_MALLOC
  free((void *)pProjective_H);
	free((void *)pProjective_V);
#else
  lv_free((void *)pProjective_H);
	lv_free((void *)pProjective_V);			 
#endif /*CONFIG_LVGL_USE_MALLOC*/
}	



