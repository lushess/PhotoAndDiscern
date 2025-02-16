#include "picture_tool.h"
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

static void fastsort(vu8 *arrayhead,vu8 *arrayend)
{
	vu8 *arrayleft=arrayhead,*arrayright=arrayend,*arraycut;
	vu8 temp=*arrayleft;

	while(arrayhead<arrayend)
	{
	  while(arrayhead<arrayend&&*arrayend<=temp) arrayend--;
	
	  *arrayhead=*arrayend;
		
		while(arrayhead<arrayend&&*arrayhead>=temp) arrayhead++;
		
		*arrayend=*arrayhead;
	}

	*arrayhead=temp;
	
	arraycut=arrayhead;
	
	if(arrayleft<arrayright)
	{
	  fastsort(arrayleft,arraycut);
		fastsort(arraycut,arrayright);
	}
}

static u16 RGB565ToHSV745(vu16 RGB565) //RGB565תHSV745
{                                          
	vu8 max = 0,min = 0;
	vu8 r = 0,g = 0,b = 0;
  vu8 V=0,S=0;//����HSVֵ
  __IO int8_t H=0;
	r=R8FromRGB565(RGB565);g=G8FromRGB565(RGB565);b=B8FromRGB565(RGB565);
	
	max = r;
	min = r;
	
	if(g >= max)max = g;
	if(b >= max)max = b;
	if(g <= min)min = g;
	if(b <= min)min = b;
	
	V = max;                                                    //ת��8λ
	S = (max - min) / max *255;                                 //ת��8λ
	if(max == r)H =(g - b) / (max - min) * 255/60;
	if(max == g)H = 85 + (b - r) / (max - min) * 255/60;
	if(max == b)H = 170 + (r - g) / (max - min) * 255/60;
	if(H<0) H=H+255;
	
	return hsv24Tohsv16(H,S,V) ;
}

void CapLCDToGray(vu8 *GrayImage)//GrayImageΪ����������׵�ַָ�뼴��һ��Ԫ��
{
//	vu8 r=0,g=0,b=0;
	vu16 Imagetmp,i,j;
	vu32 r=0,g=0,b=0;
	LCD_Display_Dir(0);
    LCD_Set_Window(0,0,240-1,320-1);
	for(j=1;j<=320;j++)
	{
		for(i=1;i<=240;i++)
		{
			Imagetmp=TFTLCD->LCD_DATA;
			LCD_WriteData_Color(TFTLCD->LCD_DATA);
			//Imagetmp=LCD_ReadPoint(i-1,j-1);
			r=R8FromRGB565(Imagetmp);
			g=G8FromRGB565(Imagetmp);
			b=B8FromRGB565(Imagetmp);
//			*GrayImage++=(vu8)((0.2126*r)+(0.7152*g)+(0.0722*b));//ȡ�Ҷ�,�洢��GrayImage[76800]���飬���Ȳ�
//			*GrayImage++=(vu8)((r*38+g*75+b*15)>>7);//Gray = R0.299 + G0.587 + B*0.114;�Ŵ�2^7�����,����7λ
			*GrayImage++=(vu8)((r*19595+g*38469+b*7472)>>16);
		}
	}	
	printf("��ȡ�Ҷ�ͼ�ɹ���\r\n");
}
void CapLCDToBinary_ByRG565_HSV745IsTmp(vu8 *BinaryImage)//δʵ��
{
	vu8 r=0,g=0,b=0,s=0,v=0,max=0,min=0;
	int8_t h=0;
	vu8 *TmpBinaryImage=BinaryImage;
	vu16 Imagetmp,i,j;
//	vu16 *hsv=(vu16 *)malloc(240*320);
//	vu16 *phsv=hsv;
	LCD_Display_Dir(0);
  LCD_Set_Window(0,0,240-1,320-1);
	for(j=1;j<=320;j++)
	{
		for(i=1;i<=240;i++)
		{
			Imagetmp=TFTLCD->LCD_DATA;
			r=R8FromRGB565(Imagetmp);g=G8FromRGB565(Imagetmp);b=B8FromRGB565(Imagetmp);
	
			max = r;
			min = r;
			
			if(g >= max)max = g;
			if(b >= max)max = b;
			if(g <= min)min = g;
			if(b <= min)min = b;
			
			v = max;                                                    //ת��8λ
			s = (max - min) / max *255;                                 //ת��8λ
			if(max == r)h =(g - b) / (max - min) * 255/60;
			if(max == g)h = 85 + (b - r) / (max - min) * 255/60;
			if(max == b)h= 170 + (r - g) / (max - min) * 255/60;
			if(h<0) h=h+255;
			
			//TODO:
			
			
			
			LCD_WriteData_Color(TFTLCD->LCD_DATA);
		}
	}		
	
//	myfree(SRAMEX,(void *)hsv);
	printf("��ȡ��ֵͼ�ɹ���\r\n");
}
//void CapLCDToGray(vu8 *GrayImage)//GrayImageΪ����������׵�ַָ�뼴��һ��Ԫ��
//{
//	vu8 r=0,g=0,b=0;
//	vu16 Imagetmp,i,j;
//	LCD_Display_Dir(0);
//  LCD_Set_Window(0,0,240-1,320-1);
//	for(j=1;j<=320;j++)
//	{
//		for(i=1;i<=240;i++)
//		{
//			Imagetmp=TFTLCD->LCD_DATA;
//			LCD_WriteData_Color(TFTLCD->LCD_DATA);
//			r=(Imagetmp>>10)&0x3f;
//			g=(Imagetmp>>5)&0x1f;
//			b=Imagetmp&0x3f;
//			*GrayImage++=(vu8)((0.2126*r)+(0.7152*g)+(0.0722*b));//ȡ�Ҷ�,�洢��GrayImage[76800]����
//		}
//	}	
//	printf("��ȡ�Ҷ�ͼ�ɹ���\r\n");
//}
static u16 ArrayAddup(vu8 *Array,u8 Num)
{
  vu8 *pArray=Array;
	u16 res=0;
  while(Num--)
	{
	  res+=*pArray;
	  pArray++;
	}
  return res;
}
void OptionalFilter(vu8 *Image) //����Ӧѡ����ƽ���˲���������9�����ӣ�����Ч��������
{
	vu8 ranknum,tmppointnum;
  vu8 *pImage=Image;	
  vu8 rank[9]={0},tmppoint[9];//tmppoint�洢ÿ��ģ������ƽ������ֵ
	vu8 *prank;
	vu16 i,j;
	vu32 variance[9]={0},Minvariance=0xffffffff;//variance�洢���Ӧ��ģ�����ط����
	
#ifdef CONFIG_LVGL_USE_MALLOC
  vu8 *TmpImage=(vu8 *)malloc(240*320),*pTmpImage=TmpImage;
	memset((void *)pTmpImage,255,240*320);
#else
	vu8 *TmpImage=(vu8 *)lv_malloc(240*320),*pTmpImage=TmpImage;
	lv_memset((void *)pTmpImage,255,240*320);		 
#endif /*CONFIG_LVGL_USE_MALLOC*/
	
	pTmpImage=TmpImage+240*2;pImage=Image+240*2; //ָ���������������
  for(j=3;j<=320-2;j++)
	{
		for(i=3;i<=240-2;i++)
		{
      if(i==3)          //����x���һ��������
			{
				pTmpImage+=2;
			  pImage+=2;
			}
			
			rank[0]=*(pImage-240-1);  //��һ������
			rank[1]=*(pImage-240);
      rank[2]=*(pImage-240+1);
			rank[3]=*(pImage-1);
			rank[4]=*pImage;
			rank[5]=*(pImage+1);
			rank[6]=*(pImage+240-1);
			rank[7]=*(pImage+240);
			rank[8]=*(pImage+240+1);
			tmppoint[0]=ArrayAddup(rank,9)/9;
			prank=rank;
			for(ranknum=0;ranknum<9;ranknum++)
			{
				if(tmppoint[0]>*prank) variance[0]+=tmppoint[0]*tmppoint[0]-(*prank**prank);
			  else variance[0]+=*prank**prank-(tmppoint[0]*tmppoint[0]);
			}
			
			
			rank[0]=*(pImage-240-2);  //�ڶ�������
			rank[1]=*(pImage-240-1);
      rank[2]=*(pImage-2);
			rank[3]=*(pImage-1);					
			rank[4]=*(pImage+240-2);
			rank[5]=*(pImage+240-1);
      rank[6]=*pImage;
			tmppoint[1]=ArrayAddup(rank,7)/7;
			prank=rank;
			for(ranknum=0;ranknum<7;ranknum++)
			{
				if(tmppoint[1]>*prank) variance[1]+=tmppoint[1]*tmppoint[1]-(*prank**prank);
			  else variance[1]+=*prank**prank-(tmppoint[1]*tmppoint[1]);
			}

			
			rank[0]=*(pImage-240*2-1);  //����������
			rank[1]=*(pImage-240*2);
      rank[2]=*(pImage-240*2+1);
			rank[3]=*(pImage-240-1);
			rank[4]=*(pImage-240);		
			rank[5]=*(pImage-240+1);
			rank[6]=*pImage;
			tmppoint[2]=ArrayAddup(rank,7)/7;
			prank=rank;
			for(ranknum=0;ranknum<7;ranknum++)
			{
				if(tmppoint[2]>*prank) variance[2]+=tmppoint[2]*tmppoint[2]-(*prank**prank);
			  else variance[2]+=*prank**prank-(tmppoint[2]*tmppoint[2]);
			}

						
			rank[0]=*(pImage-240+1);  //���ĸ�����
			rank[1]=*(pImage-240+2);
      rank[2]=*(pImage+1);
			rank[3]=*(pImage+2);
			rank[4]=*(pImage+240+1);		
			rank[5]=*(pImage+240+2);
			rank[6]=*pImage;
			tmppoint[3]=ArrayAddup(rank,7)/7;
			prank=rank;
			for(ranknum=0;ranknum<7;ranknum++)
			{
				if(tmppoint[3]>*prank) variance[3]+=tmppoint[3]*tmppoint[3]-(*prank**prank);
			  else variance[3]+=*prank**prank-(tmppoint[3]*tmppoint[3]);
			}

			
			rank[0]=*(pImage+240-1);  //���������
			rank[1]=*(pImage+240);
      rank[2]=*(pImage+240+1);
			rank[3]=*(pImage+240*2-1);
			rank[4]=*(pImage+240*2);		
			rank[5]=*(pImage+240*2+1);
			rank[6]=*pImage;
			tmppoint[4]=ArrayAddup(rank,7)/7;
			prank=rank;
			for(ranknum=0;ranknum<7;ranknum++)
			{
				if(tmppoint[4]>*prank) variance[4]+=tmppoint[4]*tmppoint[4]-(*prank**prank);
			  else variance[4]+=*prank**prank-(tmppoint[4]*tmppoint[4]);
			}

						
			rank[0]=*(pImage-240*2-2);  //����������
			rank[1]=*(pImage-240*2-1);
      rank[2]=*(pImage-240-2);
			rank[3]=*(pImage-240-1);
			rank[4]=*(pImage-240);		
			rank[5]=*(pImage-1);
			rank[6]=*pImage;
			tmppoint[5]=ArrayAddup(rank,7)/7;
			prank=rank;
			for(ranknum=0;ranknum<7;ranknum++)
			{
				if(tmppoint[5]>*prank) variance[5]+=tmppoint[5]*tmppoint[5]-(*prank**prank);
			  else variance[5]+=*prank**prank-(tmppoint[5]*tmppoint[5]);
			}

						
			rank[0]=*(pImage-240*2+2);  //���߸�����
			rank[1]=*(pImage-240*2+1);
      rank[2]=*(pImage-240+2);
			rank[3]=*(pImage-240+1);
			rank[4]=*(pImage-240);		
			rank[5]=*(pImage+1);
			rank[6]=*pImage;
			tmppoint[6]=ArrayAddup(rank,7)/7;
			prank=rank;
			for(ranknum=0;ranknum<7;ranknum++)
			{
				if(tmppoint[6]>*prank) variance[6]+=tmppoint[6]*tmppoint[6]-(*prank**prank);
			  else variance[6]+=*prank**prank-(tmppoint[6]*tmppoint[6]);
			}

						
			rank[0]=*(pImage+1);  //�ڰ˸�����
			rank[1]=*(pImage+240);
      rank[2]=*(pImage+240+1);
			rank[3]=*(pImage+240+1);
			rank[4]=*(pImage+240*2+1);		
			rank[5]=*(pImage+240*2+2);
			rank[6]=*pImage;
			tmppoint[7]=ArrayAddup(rank,7)/7;
			prank=rank;
			for(ranknum=0;ranknum<7;ranknum++)
			{
				if(tmppoint[7]>*prank) variance[7]+=tmppoint[7]*tmppoint[7]-(*prank**prank);
			  else variance[7]+=*prank**prank-(tmppoint[7]*tmppoint[7]);
			}

						
			rank[0]=*(pImage-1);  //�ھŸ�����
			rank[1]=*(pImage+240);
      rank[2]=*(pImage+240-1);
			rank[3]=*(pImage+240-2);
			rank[4]=*(pImage+240*2-1);		
			rank[5]=*(pImage+240*2-2);
			rank[6]=*pImage;
			tmppoint[8]=ArrayAddup(rank,7)/7;
			prank=rank;
			for(ranknum=0;ranknum<7;ranknum++)
			{
				if(tmppoint[8]>*prank) variance[8]+=tmppoint[8]*tmppoint[8]-(*prank**prank);
			  else variance[8]+=*prank**prank-(tmppoint[8]*tmppoint[8]);
			}

			
			
			for(ranknum=0;ranknum<9;ranknum++)  //��������͵����飬�ҵ�������С�����ӣ��õ����±�
			{
			  if(Minvariance>=variance[ranknum])
				{
				  Minvariance=variance[ranknum];
					tmppointnum=ranknum;
				}			
			}
			
			*pTmpImage=tmppoint[tmppointnum];  //�Ѷ�Ӧ�±����ʱ���ظ�ֵ����ʱ��������
			pTmpImage++;               //ָ���ƶ�����һ������
			pImage++;
			
	    if(i==240-2)          //����x�������������
			{
				pTmpImage+=2;
			  pImage+=2;
			}
		}
	}
	
	pTmpImage=TmpImage;pImage=Image;
	for(j=1;j<=320;j++)               //��ԭͼ�����鸳ֵ
	{
		for(i=1;i<=240;i++)
		{
			*pImage++=*pTmpImage;
			pTmpImage++;
		}
	}	
	
	
#ifdef CONFIG_LVGL_USE_MALLOC
  free((void *)TmpImage);
#else
	lv_free((void *)TmpImage);		 
#endif /*CONFIG_LVGL_USE_MALLOC*/
}

void MedianFilter(vu8 *Image)
{
	vu8 ranknum,tmpranknum,tmpimage;
	vu8 rank[9]={0};
	vu16 i,j;
	
#ifdef CONFIG_LVGL_USE_MALLOC
  vu8 *TmpImage=(vu8 *)malloc(240*320);
	vu8 *pTmpImage=TmpImage,*pImage=Image;
	memset((void *)pTmpImage,255,240*320);
#else
	vu8 *TmpImage=(vu8 *)lv_malloc(240*320);
	vu8 *pTmpImage=TmpImage,*pImage=Image;
	lv_memset((void *)pTmpImage,255,240*320);		 
#endif /*CONFIG_LVGL_USE_MALLOC*/
	
	//	if(TmpImage!=NULL)	printf("��ֵ�˲��ڴ����ɹ�\r\n");		
//	else printf("��ֵ�˲��ڴ����ʧ��\r\n");
	pTmpImage=TmpImage+240;pImage=Image+240; //ָ����������ڶ���
	for(j=2;j<=320-1;j++)
	{
		for(i=2;i<=240-1;i++)
		{
			if(i==2)          //����x���һ������
			{
				pTmpImage++;
			  pImage++;
			}
			rank[0]=*(pImage-240-1);  //ȡ���������ϡ��ϡ����ϡ����¡��¡����¡����ҡ��Լ�����9���������
			rank[1]=*(pImage-240);
      rank[2]=*(pImage-240+1);
			rank[3]=*(pImage-1);
			rank[4]=*pImage;
			rank[5]=*(pImage+1);
			rank[6]=*(pImage+240-1);
			rank[7]=*(pImage+240);
			rank[8]=*(pImage+240+1);

//			fastsort(rank,rank+4);  //��������
			for(ranknum=8;ranknum>0;ranknum--)       //ð�ݷ����򣬴Ӵ�С
			{
			  for(tmpranknum=0;tmpranknum<ranknum;tmpranknum++)
				{
				  if(rank[tmpranknum]<rank[tmpranknum+1]) 
					{
					  tmpimage=rank[tmpranknum];
						rank[tmpranknum]=rank[tmpranknum+1];
						rank[tmpranknum+1]=tmpimage;
					}
				}
			}

			*pTmpImage=rank[4];    //������Ԥ��ֵ��ֵ���洢����ʱ����
			pTmpImage++;               //ָ���ƶ�����һ������
			pImage++;
			if(i==240-1)        //����x�����һ������
			{
			  pTmpImage++;
			  pImage++;
			}
		}
	}	
	
	pTmpImage=TmpImage;pImage=Image;
	for(j=1;j<=320;j++)               //��ԭͼ�����鸳ֵ
	{
		for(i=1;i<=240;i++)
		{		
			*pImage++=*pTmpImage;
			pTmpImage++;
		}
	}	
	
#ifdef CONFIG_LVGL_USE_MALLOC
  free((void *)TmpImage);
#else
	lv_free((void *)TmpImage);		 
#endif /*CONFIG_LVGL_USE_MALLOC*/
	printf("��ֵ�˲��ɹ���\r\n");
}

static __IO u8 *GrayGradient_Diffrential(vu8 *GrayImage,u8 GradientThreshold)//��������洢��ÿ�����ص��ݶȣ���ֵ���ϵ��ݶ���Ϊ255������˫���ִ���˫��΢��
{
  vu8 *pGrayImage=GrayImage;
	vu8 tmpGradientx,tmpGradienty;
	vu16 i,j;
#ifdef CONFIG_LVGL_USE_MALLOC
	vu8 *Gradient=(vu8 *)malloc(240*320),*pGradient=Gradient;
#else
	vu8 *Gradient=(vu8 *)lv_malloc(240*320),*pGradient=Gradient;
#endif /*CONFIG_LVGL_USE_MALLOC*/	
	for(j=1;j<=320-1;j++)
	{
	  for(i=1;i<=240-1;i++)
		{
		  if(*pGrayImage>*(pGrayImage+240))tmpGradienty=*pGrayImage-*(pGrayImage+240);
			else tmpGradienty=*(pGrayImage+240)-*pGrayImage;
			if(*pGrayImage>*(pGrayImage+1))tmpGradientx=*pGrayImage-*(pGrayImage+1);
			else tmpGradientx=*(pGrayImage+1)-*pGrayImage;
			
			*pGradient=tmpGradienty+tmpGradientx;	
      if(*Gradient>GradientThreshold||255<tmpGradienty+tmpGradientx) *pGradient=255;	
			pGrayImage++;
			pGradient++;	
      if(i==240-1)		
			{			
			  pGrayImage++;
			  pGradient++;
			}				
		}
	}	
	return Gradient;
}
static __IO u8 *GrayGradient_Roberts(vu8 *GrayImage,u8 GradientThreshold) //Roberts�������ݶ�
{
  vu8 *pGrayImage=GrayImage;
	vu8 tmpGradientfirst,tmpGradientysecond;
	vu16 i,j;
#ifdef CONFIG_LVGL_USE_MALLOC
	vu8 *Gradient=(vu8 *)malloc(240*320),*pGradient=Gradient;
#else
	vu8 *Gradient=(vu8 *)lv_malloc(240*320),*pGradient=Gradient;
#endif /*CONFIG_LVGL_USE_MALLOC*/
	for(j=1;j<=320-1;j++)
	{
	  for(i=1;i<=240-1;i++)
		{
		  if(*(pGrayImage+1)>*(pGrayImage+240))tmpGradientfirst=*(pGrayImage+1)-*(pGrayImage+240);
			else tmpGradientfirst=*(pGrayImage+240)-*(pGrayImage+1);
			if(*pGrayImage>*(pGrayImage+240+1))tmpGradientysecond=*pGrayImage-*(pGrayImage+240+1);
			else tmpGradientysecond=*(pGrayImage+240+1)-*pGrayImage;
			
			*pGradient=tmpGradientfirst+tmpGradientysecond;	
      if(*Gradient>GradientThreshold||255<tmpGradientfirst+tmpGradientysecond) *pGradient=255;	
			pGrayImage++;
			pGradient++;
			
		  if(i==240-1)
			{
		  	pGrayImage++;
				pGradient++;
			}
		}
	}
	
	return Gradient;
}
static __IO u8 *GrayGradient_Sobel(vu8 *GrayImage,u8 GradientThreshold) //Sobel�������ݶ�
{
	vu8 rank[6]={0};
	vu8 *pGrayImage=GrayImage;
	vu16 i,j;
	__IO int16_t tmpGradientH,tmpGradientyV;
#ifdef CONFIG_LVGL_USE_MALLOC
	vu8 *Gradient=(vu8 *)malloc(240*320),*pGradient=Gradient;
#else
	vu8 *Gradient=(vu8 *)lv_malloc(240*320),*pGradient=Gradient;
#endif /*CONFIG_LVGL_USE_MALLOC*/
	pGrayImage=GrayImage+240;pGradient=Gradient+240; //ָ����������ڶ���
	for(j=2;j<=320-1;j++)
	{
		for(i=2;i<=240-1;i++)
		{
			if(i==2)          //����x���һ������
			{
				pGrayImage++;
			  pGradient++;
			}
			rank[0]=*(pGrayImage-240-1);
			rank[1]=*(pGrayImage-240);
			rank[2]=*(pGrayImage-240+1);
			rank[3]=*(pGrayImage+240-1);
			rank[4]=*(pGrayImage+240);
			rank[5]=*(pGrayImage+240+1);
			
			tmpGradientH=rank[3]+rank[4]*2+rank[5]-rank[0]-rank[1]*2-rank[2];
			if(0>tmpGradientH) tmpGradientH&=0x7fff;
			
			rank[0]=*(pGrayImage-240-1);
			rank[1]=*(pGrayImage-1);
			rank[2]=*(pGrayImage+240-1);
			rank[3]=*(pGrayImage-240+1);
			rank[4]=*(pGrayImage+1);
			rank[5]=*(pGrayImage+240+1);

			tmpGradientyV=rank[0]+rank[1]*2+rank[2]-rank[3]-rank[4]*2-rank[5];
			if(0>tmpGradientyV) tmpGradientyV&=0x7fff;
			
			*pGradient=tmpGradientH+tmpGradientyV;
			if(*Gradient>GradientThreshold||255<tmpGradientH+tmpGradientyV) *pGradient=255;	
      pGrayImage++;
			pGradient++;			
					
			if(i==240-1)          //����x���������
			{
				pGrayImage++;
			  pGradient++;
			}			
		}
	}

  return Gradient;
}

__IO u8 *GrayGradientToMask(vu8 *Gradient) //�����ݶ�ͼ������ģ�����ڲ���Ϊ255
{
   vu8 *Mask=Gradient,*pMask=Mask;
	 vu16 i,j;
	
	for(j=1;j<=320;j++)
	{
	  for(i=1;i<=240;i++)
		{
			if(*pMask==255&&*(pMask+1)!=255) //�ӱ��ؿ�ʼ��������ǰ������ֱ����һ�����أ���255
			{
			  pMask++;
				*pMask=255;
			}
			else pMask++;
		}
	}
	
  return Mask;
}

void BinaryToEdge(vu8 *BinaryImage) //�Ϳ��ڲ��㷨ȡ��ǰ������
{
  vu8 *pBinaryImage=BinaryImage;
	vu8 rank[8]={0},ranknum;
  vu16 i,j;
#ifdef CONFIG_LVGL_USE_MALLOC
	vu8 *Edge=(vu8 *)malloc(240*320),*pEdge=Edge;
#else
	vu8 *Edge=(vu8 *)lv_malloc(240*320),*pEdge=Edge;
#endif /*CONFIG_LVGL_USE_MALLOC*/
	pBinaryImage=BinaryImage+240;pEdge=Edge+240; //ָ����������ڶ���
  for(j=2;j<=320-1;j++)
	{
		for(i=2;i<=240-1;i++)
		{
			if(i==2)          //����x���һ������
			{
				pBinaryImage++;
			  pEdge++;
			}
			
			if(0==*pBinaryImage)
			{
				rank[0]=*(pBinaryImage-240-1);  //ȡ��������Χ8���������
			  rank[1]=*(pBinaryImage-240);
        rank[2]=*(pBinaryImage-240+1);
			  rank[3]=*(pBinaryImage-1);
			  rank[4]=*(pBinaryImage+1);
			  rank[5]=*(pBinaryImage+240-1);
			  rank[6]=*(pBinaryImage+240);
			  rank[7]=*(pBinaryImage+240+1);

				for(ranknum=0;ranknum<8;ranknum++)	
        {
				  rank[0]|=rank[ranknum];			
				}		
        if(0==rank[0]) *pEdge=255;	//����ȫΪ����Ϊǰ������255
			}	
			pBinaryImage++;
			pEdge++;
						
			if(i==240-1)          //����x�����һ������
			{
				pBinaryImage++;
			  pEdge++;
			}
		}
	}
	
	pBinaryImage=BinaryImage;pEdge=Edge;
	for(j=1;j<=320;j++)
	{
	  for(i=1;i<=240;i++)
		{
		  if(0!=*pEdge)      //��Ϊ0�������Ϳղ��֣�ԭͼ��255
			{
		    *pBinaryImage=255;
			}	
      pBinaryImage++;	
      pEdge++;			
		}	
	}
#ifdef CONFIG_LVGL_USE_MALLOC
	free((void *)Edge);
#else
	lv_free((void *)Edge);
#endif /*CONFIG_LVGL_USE_MALLOC*/
}

__IO u8 *ObjectMeasure_FromBinary(vu8 *BinaryImage,vu32 *ObjectSize)//��Ǹ������壬�����أ����ֵ��1��ʼ��ObjectSize�����¼ÿ���������ռ�����ش�С
{
  vu8 *pBinaryImage=BinaryImage;
	vu8 ObjectFillFlag=0;
	vu16 i,j;
#ifdef CONFIG_LVGL_USE_MALLOC
	vu8 *ObjectFlag=(vu8 *)malloc(240*320),*pObjectFlag=ObjectFlag;
#else
	vu8 *ObjectFlag=(vu8 *)lv_malloc(240*320),*pObjectFlag=ObjectFlag;
#endif /*CONFIG_LVGL_USE_MALLOC*/
	vu32 *pObjectSize=ObjectSize;
	
	pBinaryImage=BinaryImage+240;pObjectFlag=ObjectFlag+240; //ָ����������ڶ���
  for(j=2;j<=320-1;j++)
	{
		myASSERT(0==ObjectFillFlag);
		for(i=2;i<=240-1;i++)
		{
			if(i==2)          //����x���һ������
			{
				pBinaryImage++;
			  pObjectFlag++;
			}
			
			if(0==*pBinaryImage)
			{
			  
				//TODO:
			}
			
			
			
			pBinaryImage++;
			pObjectFlag++;
			
			if(i==240-1)          //����x�����һ������
			{
				pBinaryImage++;
			  pObjectFlag++;
			}			
		}
	}
	
	
	
  return ObjectFlag;
}


static void Corrode(vu8 *Image)
{
  vu8 ranknum,tmpimage;
	vu8 rank[5]={0};
	vu16 i,j;
#ifdef CONFIG_LVGL_USE_MALLOC
	vu8 *TmpImage=(vu8 *)malloc(240*320);
	vu8 *pTmpImage=TmpImage,*pImage=Image;
#else
	vu8 *TmpImage=(vu8 *)lv_malloc(240*320);
	vu8 *pTmpImage=TmpImage,*pImage=Image;
#endif /*CONFIG_LVGL_USE_MALLOC*/
	pTmpImage=TmpImage+240;pImage=Image+240; //ָ����������ڶ���
	for(j=2;j<=320-1;j++)
	{
		for(i=2;i<=240-1;i++)
		{
			if(i==2)          //����x���һ������
			{
				pTmpImage++;
			  pImage++;
			}
			rank[0]=*(pImage-240);  //ȡ���������������Լ�����5���������
			rank[1]=*(pImage-1);
      rank[2]=*pImage;
			rank[3]=*(pImage+1);
			rank[4]=*(pImage+240);
//			#ifdef Charater_DeepColor
			*pTmpImage=0;
		  for(ranknum=0;ranknum<5;ranknum++)
			{
				if(128<rank[ranknum])
				{
			    *pTmpImage=255;
          break;					
				}
			}
//			#endif /* Charater_DeepColor */
//			#ifdef Charater_LightColor				
//			*pTmpImage=255;
//		  for(ranknum=0;ranknum<5;ranknum++)
//			{
//				if(128>=rank[ranknum])
//				{
//			    *pTmpImage=0;
//          break;					
//				}
//			}
//			#endif /* Charater_LightColor */
			pTmpImage++;               //ָ���ƶ�����һ������
			pImage++;
			if(i==240-1)        //����x�����һ������
			{
			  pTmpImage++;
			  pImage++;
			}
		}
	}	
	pTmpImage=TmpImage+240;pImage=Image+240; //ָ����������ڶ���
	for(j=2;j<=320-1;j++)               //��ԭͼ�����鸳ֵ
	{
		for(i=2;i<=240-1;i++)
		{
			if(i==2)          //����x���һ������
			{
				pTmpImage++;
			  pImage++;
			}
		
			*pImage++=*pTmpImage;
			pTmpImage++;
			if(i==240-1)        //����x�����һ������
			{
			  pTmpImage++;
			  pImage++;
			}
		}
	}	
#ifdef CONFIG_LVGL_USE_MALLOC
	free((void *)TmpImage);
#else
	lv_free((void *)TmpImage);
#endif /*CONFIG_LVGL_USE_MALLOC*/
}

static void Expand(vu8 *Image)
{
  vu8 ranknum,tmpimage;
	vu8 rank[5]={0};
	vu16 i,j;
#ifdef CONFIG_LVGL_USE_MALLOC
	vu8 *TmpImage=(vu8 *)malloc(240*320);
	vu8 *pTmpImage=TmpImage,*pImage=Image;
#else
	vu8 *TmpImage=(vu8 *)lv_malloc(240*320);
	vu8 *pTmpImage=TmpImage,*pImage=Image;
#endif /*CONFIG_LVGL_USE_MALLOC*/
	pTmpImage=TmpImage+240;pImage=Image+240; //ָ����������ڶ���
	for(j=2;j<=320-1;j++)
	{
		for(i=2;i<=240-1;i++)
		{
			if(i==2)          //����x���һ������
			{
				pTmpImage++;
			  pImage++;
			}
			rank[0]=*(pImage-240);  ///ȡ���������������Լ�����5���������
			rank[1]=*(pImage-1);
      rank[2]=*pImage;
			rank[3]=*(pImage+1);
			rank[4]=*(pImage+240);
//			#ifdef Charater_DeepColor
			*pTmpImage=255;
		  for(ranknum=0;ranknum<5;ranknum++)
			{
				if(128>rank[ranknum])
				{
			    *pTmpImage=0;
          break;					
				}
			}
//			#endif /* Charater_DeepColor */
//			#ifdef Charater_LightColor				
//			*pTmpImage=0;
//		  for(ranknum=0;ranknum<5;ranknum++)
//			{
//				if(128<=rank[ranknum])
//				{
//			    *pTmpImage=255;
//          break;					
//				}
//			}
//			#endif /* Charater_LightColor */
			pTmpImage++;               //ָ���ƶ�����һ������
			pImage++;
			if(i==240-1)        //����x�����һ������
			{
			  pTmpImage++;
			  pImage++;
			}
		}
	}	
	pTmpImage=TmpImage+240;pImage=Image+240; //ָ����������ڶ���
	for(j=2;j<=320-1;j++)               //��ԭͼ�����鸳ֵ
	{
		for(i=2;i<=240-1;i++)
		{
			if(i==2)          //����x���һ������
			{
				pTmpImage++;
			  pImage++;
			}
		
			*pImage++=*pTmpImage;
			pTmpImage++;
			if(i==240-1)        //����x�����һ������
			{
			  pTmpImage++;
			  pImage++;
			}
		}
	}	
#ifdef CONFIG_LVGL_USE_MALLOC
	free((void *)TmpImage);
#else
	lv_free((void *)TmpImage);
#endif /*CONFIG_LVGL_USE_MALLOC*/
}

void CloseOperation(vu8 *Image) //������
{
	 Expand(Image);  //����
   Corrode(Image);  //��ʴ
}

void OpenOperation(vu8 *Image) //������
{
   Corrode(Image);  //��ʴ
	 Expand(Image);  //����
}

static u8 CalcThresold_Average(vu8 *GrayImage)
{
  vu8 *TmpGrayImage=GrayImage;
  vu16 i,j;
	u32 sum=0;
	for(j=1;j<=320;j++)
	{
    for(i=1;i<=240;i++)
		{
      sum+=*TmpGrayImage;
			TmpGrayImage++;
		}
	}
  return sum/(240*320);
}

static u8 CalcThresold_MultiAverage(vu8 *GrayImage) //������ֵ��
{
	vu8 Thresold=CalcThresold_Average(GrayImage),ThresoldBefore=0;
  vu8 *TmpGrayImage=GrayImage;
  vu16 i,j;
	u32 sum1=0,sum2=0,cout1=0,cout2=0;
	while(Thresold!=ThresoldBefore)
	{
		ThresoldBefore=Thresold;
		for(j=1;j<=320;j++)
		{
			for(i=1;i<=240;i++)
			{
				if(*TmpGrayImage>Thresold)
				{
					sum1+=*TmpGrayImage;
					cout1++;
				}
				else
				{
				  sum2+=*TmpGrayImage;
					cout2++;
				}
			}
			TmpGrayImage++;
		}
		Thresold=((sum1/cout1)+(sum2/cout2))/2;
		TmpGrayImage=GrayImage;
		sum1=0;sum2=0;cout1=0;cout2=0;
	}
	return Thresold;
}

static __IO float *CalcGrayHist_Chance(vu8 *GrayImage) //����Ҷ�ֱ��ͼ���Ը��ʷֲ�����ʽ����
{
	vu8 *pGrayImage=GrayImage;
	vu16 i,j;
#ifdef CONFIG_LVGL_USE_MALLOC
	float *histchance=(float *)malloc(256),*phistchance=histchance;
  vu32 *hist=(vu32 *)malloc(256),*phist=hist;
#else
	float *histchance=(float *)lv_malloc(256),*phistchance=histchance;
  vu32 *hist=(vu32 *)lv_malloc(256),*phist=hist;
#endif /*CONFIG_LVGL_USE_MALLOC*/
	for(j=1;j<=320;j++)
	{
    for(i=1;i<=240;i++)
		{
		  hist[*pGrayImage]++; //��Ӧ�Ҷȵ�ֱ��ͼ��ֵ+1
			pGrayImage++;
		}		
	}
	for(j=1;j<=256;j++)
	{   
		*phistchance=(float)(*phist)/(240*320);//��������ֱ��ͼ���������صõ����ʷֲ�
		phistchance++;
		phist++;
	}

#ifdef CONFIG_LVGL_USE_MALLOC
	free((void *)hist);
#else
	lv_free((void *)hist);
#endif /*CONFIG_LVGL_USE_MALLOC*/
  return histchance;
}

static __IO u8 *GrayImageMapping_ByChanceHist(vu8 *GrayImage)
{
	vu16 i,j;

#ifdef CONFIG_LVGL_USE_MALLOC
	vu8 *mapping=(vu8 *)malloc(256);
  vu8 *pGrayImage=GrayImage,*pmapping=mapping;
  __IO float *pChance=CalcGrayHist_Chance(pGrayImage),*pChanceTmp=pChance;
	float *addupchance=(float *)malloc(256),*paddupchance=addupchance;
#else
	vu8 *mapping=(vu8 *)lv_malloc(256);
  vu8 *pGrayImage=GrayImage,*pmapping=mapping;
  __IO float *pChance=CalcGrayHist_Chance(pGrayImage),*pChanceTmp=pChance;
	float *addupchance=(float *)lv_malloc(256),*paddupchance=addupchance;
#endif /*CONFIG_LVGL_USE_MALLOC*/	
	for(i=1;i<=256;i++)
	{
	  if(i==1)*paddupchance=*pChanceTmp;
		else
		{
		  *paddupchance=*(paddupchance-1)+*pChanceTmp;		//�ۼƷֲ�����
		}
		*pmapping++=(u8)((255.0f**paddupchance)+0.5f);    //+0.5f����ȡ��
		paddupchance++;
		pChanceTmp++;
	}	
#ifdef CONFIG_LVGL_USE_MALLOC
  free((void *)pChance);
  free((void *)addupchance);
#else
  lv_free((void *)pChance);
  lv_free((void *)addupchance);
#endif /*CONFIG_LVGL_USE_MALLOC*/
	return mapping;
}

void GrayEven(vu8 *GrayImage)  //�Ҷ�ͼ���Ȼ�
{
	vu8 image;
	vu8 *pGrayImage=GrayImage;
	vu8 *mapping=GrayImageMapping_ByChanceHist(pGrayImage),*pmapping=mapping;
	vu16 i,j;
	
	pGrayImage=GrayImage;
	for(j=1;j<=320;j++)
	{
    for(i=1;i<=240;i++)
		{
      image=*pGrayImage;
			*pGrayImage++=pmapping[image]; 			
		}		
	}
#ifdef CONFIG_LVGL_USE_MALLOC
  free((void *)mapping);
#else
  lv_free((void *)mapping);
#endif /*CONFIG_LVGL_USE_MALLOC*/
}

void GrayChange_Linear(vu8 *GrayImage,float modulus,int8_t trans) //���Ըı�Ҷ�ֵ
{
  vu8 *pGrayImage=GrayImage;
//  vu8 *TmpGrayImage=(vu8 *)malloc(SRAMEX,240*320),*pTmpGrayImage=TmpGrayImage;
  vu16 i,j;
	float tempimage;
	
	for(j=1;j<=320;j++)
	{
    for(i=1;i<=240;i++)
		{
			tempimage=*pGrayImage*modulus+trans;
			if(255<tempimage) tempimage=255;
			else if(0>tempimage) tempimage=0;
			*pGrayImage=(u8)tempimage; 	
      pGrayImage++;			
		}		
	}	
}

static u8 CalcThresold_SimpleTwoPeak_ByGrayHist(vu8 *GrayImage)
{
	vu8 histMaxGray,GrayForbinary=0,ifindpeak=0,jfindpeak=0;
	vu8 *pGrayImage=GrayImage;
	vu16 i,j,itmp,jtmp;
	vu32 histMax=0,histSecond=0;
#ifdef CONFIG_LVGL_USE_MALLOC
  vu32 *hist=(vu32 *)malloc(256);
#else
  vu32 *hist=(vu32 *)lv_malloc(256);
#endif /*CONFIG_LVGL_USE_MALLOC*/
	for(j=1;j<=320;j++)
	{
    for(i=1;i<=240;i++)
		{
		  hist[*pGrayImage]++; //��Ӧ�Ҷȵ�ֱ��ͼ��ֵ+1
			pGrayImage++;
		}		
	}	
  for(j=1;j<=256;j++)
	{
		if(histMax<hist[j-1])//ȷ���Ҷ�ֱ��ͼ���
	  histMax=hist[j-1];		
	}
	for(j=1;j<=256;j++)
	{
		if(histMax==hist[j-1])//ȷ���Ҷȼ�����ڻҶ�
	  histMaxGray=j-1;		
	}
  for(j=histMaxGray;j>0;j--) //�������Ѱ�ҵڶ���
	{
	  if(hist[j]<3840||j==0) //С��5%���صĻҶ�ֵΪ������ֹͣ����
		{
			jtmp=j;
			break;
		}		
	}
	for(j=jtmp;j>0;j--) //�������Ѱ�ҵڶ���
	{
	  if(hist[j]>3840||j==0) 
		{		  
			jfindpeak++;
			break;
		}	
	}
	for(i=histMaxGray;i<256;i++) //���ҵ���Ѱ�ҵڶ���
	{
	  if(hist[i]<3840||i==256) //С��5%���صĻҶ�ֵΪ������ֹͣ����
		{
			itmp=i;
			break;
		}		
	}
	for(i=itmp;i<256;i++) //���ҵ���Ѱ�ҵڶ���
	{
	  if(hist[i]>3840||i==256) 
		{		  
			ifindpeak++;
			break;
		}	
	}
	
  if(jfindpeak!=0&&jtmp!=0)GrayForbinary=(vu8)jtmp;
	else if(ifindpeak!=0&&itmp!=256)GrayForbinary=(vu8)itmp;
	
#ifdef CONFIG_LVGL_USE_MALLOC
	free((void *)hist);
#else
	lv_free((void *)hist);
#endif /*CONFIG_LVGL_USE_MALLOC*/
	printf("�õ��Ҷ�ͼ��ֵ��\r\n");
  return GrayForbinary; //����ֵ��Ϊ��ֵ������ֵ
}

//�ӻҶ�ͼ�õ���ֵͼ
void GrayToBinary(vu8 *GrayImage)
{
  vu16 i,j;
	vu8 *TmpGrayImage=GrayImage;
	vu8 GrayThreshold=CalcThresold_MultiAverage(TmpGrayImage); //�ӻҶ�ֱ��ͼ�����ֵ
  for(j=1;j<=320;j++)
	{
    for(i=1;i<=240;i++)
		{
		  if(*GrayImage>GrayThreshold) *GrayImage++=0xff;//�Ҷ�����ת��Ϊ��ֵ����
			else *GrayImage++=0;				
		}		
	}	
	printf("��ȡ��ֵͼ�ɹ���\r\n");
}
void ShowGrayToLCD(vu8 *GrayImage) 
{
	vu8 *pGrayImage=GrayImage;
  vu16 i,j;
  LCD_Set_Window(0,0,240-1,320-1);
	for(j=1;j<=320;j++)
	{
    for(i=1;i<=240;i++)
		{
		  LCD_WriteData(GrayTORGB565(*pGrayImage));					
      pGrayImage++;			
		}		
	}	
}

//��ʾ��ֵͼ��LCD����BinaryImageͨ����GrayImage����һ������
void ShowBinaryToLCD(vu8 *BinaryImage) 
{
	vu8 *pBinaryImage=BinaryImage;
  vu16 i,j;
  LCD_Set_Window(0,0,240-1,320-1);
	for(j=1;j<=320;j++)
	{
    for(i=1;i<=240;i++)
		{
		  if(*pBinaryImage==0xff) LCD_WriteData(0xffff);
			else LCD_WriteData(0);		
      pBinaryImage++;			
		}		
	}	
}

__inline static u8 asb8(volatile int8_t val)
{
	return val&0x7f;
}
//*offset С��0��ʾ���ƣ�����0��ʾ����
void ImageAjust_Horizontal(vu8 *ImageToAjust,volatile int8_t *offset) 
{
  vu8 *pImageToAjust=ImageToAjust;
	__IO int8_t *poffset=offset;
	vu16 i,j;
	
	for(j=1;j<=320;j++)
	{
		if(0>*poffset)
		{
			for(i=1;i<=240;i++)
			{
				if(i-1<asb8(*poffset))  //ʹƫ��ָ��ָ����߽����Ӧƫ����
				{
					pImageToAjust++;
					continue;
				}
				else
				{
					
					*(pImageToAjust-asb8(*poffset))=*pImageToAjust; //���ָ�븳ֵƫ�ƺ�ָ�����ݣ�����ʵ������
					pImageToAjust++;
				}
			}					
	  }
		else
		{
		  pImageToAjust+=240-1; //ָ����ĩβ
		  for(i=240;i>=1;i--)
			{
				if(i+1>240-*poffset) //���ٵ�������,��ֹ��ȡ��߽�������
				{
					//pImageToAjust--;
					continue;
				}
				else
				{					
					*pImageToAjust=*(pImageToAjust-*poffset);		
          pImageToAjust--;					
				}
			}		
      pImageToAjust+=240-*poffset;//ָ����һ��
		}
		poffset++;
	}
}
//����ͶӰ��x��
__IO u16 *Projective_H(vu8 *BinaryImage)
{
	vu8 *pBinaryImage=BinaryImage;
  vu16 i,j;
#ifdef CONFIG_LVGL_USE_MALLOC
  vu16 *projection=(vu16 *)malloc(240),*pprojection=projection;
#else
  vu16 *projection=(vu16 *)lv_malloc(240),*pprojection=projection;
#endif /*CONFIG_LVGL_USE_MALLOC*/	
	for(j=1;j<=320;j++)
	{
    for(i=1;i<=240;i++)
		{
			if(*pBinaryImage==0)(*pprojection)++; //������Ϊ�ڣ�ͶӰֵ+1
			pBinaryImage++;
			pprojection++;
		}	
		pprojection=projection; //ָ��ָ��ͶӰ���ݿ�ͷ
	}
	
  return projection;
}
//����ͶӰ��y��
__IO u16 *Projective_V(vu8 *BinaryImage) 
{
	vu8 *pBinaryImage=BinaryImage;
  vu16 i,j;
#ifdef CONFIG_LVGL_USE_MALLOC
	vu16 *projection=(vu16 *)malloc(320);
	vu16 *pprojection=projection;
#else
	vu16 *projection=(vu16 *)lv_malloc(320);
	vu16 *pprojection=projection;
#endif /*CONFIG_LVGL_USE_MALLOC*/
  for(j=1;j<=240;j++)
	{
		pBinaryImage=BinaryImage+j-1;//ָ��ָ��x�����һ������
    for(i=1;i<=320;i++)
		{
		  if(*pBinaryImage==0) (*pprojection)++;
			pBinaryImage+=240;    //����ָ��ָ�����ص�ͬһx�������һ������
			pprojection++;
		}	
	}	
	return projection;
}
