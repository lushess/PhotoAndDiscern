#include "picture_tool.h"
#include "ean13_barcode.h"
#include "tftlcd.h"
#include "flash.h"
#include "usart.h"
#include "Config/Config.h"
#include <stdlib.h>
#include <string.h>

#ifdef USE_FREERTOS_MALLOCATOR
#include "freeRTOS.h"
#endif //USE_FREERTOS_MALLOCATOR

void BarcodeAjust(vu8 *BarcodeImage)
{
  vu8 *pBarcodeImage=BarcodeImage;
	vu8 xdst=0,averxdst=0;
	vu16 i,j;
	vu32 dstaddup=0;
	volatile int8_t *offset=(int8_t *)Memalloc(320),*poffset=offset;

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

	Memfree((void *)offset);

}

vu8 BarcodeDenoiseThreshold=10;
void BarcodeDenoise(vu8 *BarcodeImage) //�����뽵�봦��ͨ����Ҫ����бУ�������һ��
{
  vu8 *pBarcodeImage=BarcodeImage;
  vu16 i,j;
  
	vu16 *pProjective_H=(vu16 *)Memalloc(240*sizeof(vu16));
	vu16 *pProjective_V=(vu16 *)Memalloc(320*sizeof(vu16));
	vu16 *pProjective_Htmp=pProjective_H,*pProjective_Vtmp=pProjective_V;
	
	Projective_H(pProjective_Htmp,pBarcodeImage);
	Projective_V(pProjective_Vtmp,pBarcodeImage);
	
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

  Memfree((void *)pProjective_H);
	Memfree((void *)pProjective_V);
}	



