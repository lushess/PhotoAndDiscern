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

static u16 RGB565ToHSV745(vu16 RGB565) //RGB565转HSV745
{                                          
	vu8 max = 0,min = 0;
	vu8 r = 0,g = 0,b = 0;
  vu8 V=0,S=0;//定义HSV值
  __IO int8_t H=0;
	r=R8FromRGB565(RGB565);g=G8FromRGB565(RGB565);b=B8FromRGB565(RGB565);
	
	max = r;
	min = r;
	
	if(g >= max)max = g;
	if(b >= max)max = b;
	if(g <= min)min = g;
	if(b <= min)min = b;
	
	V = max;                                                    //转换8位
	S = (max - min) / max *255;                                 //转换8位
	if(max == r)H =(g - b) / (max - min) * 255/60;
	if(max == g)H = 85 + (b - r) / (max - min) * 255/60;
	if(max == b)H = 170 + (r - g) / (max - min) * 255/60;
	if(H<0) H=H+255;
	
	return hsv24Tohsv16(H,S,V) ;
}

void CapLCDToGray(vu8 *GrayImage)//GrayImage为像素数组的首地址指针即第一个元素
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
//			*GrayImage++=(vu8)((0.2126*r)+(0.7152*g)+(0.0722*b));//取灰度,存储于GrayImage[76800]数组，精度差
//			*GrayImage++=(vu8)((r*38+g*75+b*15)>>7);//Gray = R0.299 + G0.587 + B*0.114;放大2^7计算后,右移7位
			*GrayImage++=(vu8)((r*19595+g*38469+b*7472)>>16);
		}
	}	
	printf("读取灰度图成功！\r\n");
}
void CapLCDToBinary_ByRG565_HSV745IsTmp(vu8 *BinaryImage)//未实现
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
			
			v = max;                                                    //转换8位
			s = (max - min) / max *255;                                 //转换8位
			if(max == r)h =(g - b) / (max - min) * 255/60;
			if(max == g)h = 85 + (b - r) / (max - min) * 255/60;
			if(max == b)h= 170 + (r - g) / (max - min) * 255/60;
			if(h<0) h=h+255;
			
			//TODO:
			
			
			
			LCD_WriteData_Color(TFTLCD->LCD_DATA);
		}
	}		
	
//	myfree(SRAMEX,(void *)hsv);
	printf("读取二值图成功！\r\n");
}
//void CapLCDToGray(vu8 *GrayImage)//GrayImage为像素数组的首地址指针即第一个元素
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
//			*GrayImage++=(vu8)((0.2126*r)+(0.7152*g)+(0.0722*b));//取灰度,存储于GrayImage[76800]数组
//		}
//	}	
//	printf("读取灰度图成功！\r\n");
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
void OptionalFilter(vu8 *Image) //自适应选择性平滑滤波，其内有9个算子，能有效保留边沿
{
	vu8 ranknum,tmppointnum;
  vu8 *pImage=Image;	
  vu8 rank[9]={0},tmppoint[9];//tmppoint存储每个模板所求平均像素值
	vu8 *prank;
	vu16 i,j;
	vu32 variance[9]={0},Minvariance=0xffffffff;//variance存储其对应的模板像素方差和
	
#ifdef CONFIG_LVGL_USE_MALLOC
  vu8 *TmpImage=(vu8 *)malloc(240*320),*pTmpImage=TmpImage;
	memset((void *)pTmpImage,255,240*320);
#else
	vu8 *TmpImage=(vu8 *)lv_malloc(240*320),*pTmpImage=TmpImage;
	lv_memset((void *)pTmpImage,255,240*320);		 
#endif /*CONFIG_LVGL_USE_MALLOC*/
	
	pTmpImage=TmpImage+240*2;pImage=Image+240*2; //指向像素数组第三行
  for(j=3;j<=320-2;j++)
	{
		for(i=3;i<=240-2;i++)
		{
      if(i==3)          //跳过x轴第一二个像素
			{
				pTmpImage+=2;
			  pImage+=2;
			}
			
			rank[0]=*(pImage-240-1);  //第一个算子
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
			
			
			rank[0]=*(pImage-240-2);  //第二个算子
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

			
			rank[0]=*(pImage-240*2-1);  //第三个算子
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

						
			rank[0]=*(pImage-240+1);  //第四个算子
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

			
			rank[0]=*(pImage+240-1);  //第五个算子
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

						
			rank[0]=*(pImage-240*2-2);  //第六个算子
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

						
			rank[0]=*(pImage-240*2+2);  //第七个算子
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

						
			rank[0]=*(pImage+1);  //第八个算子
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

						
			rank[0]=*(pImage-1);  //第九个算子
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

			
			
			for(ranknum=0;ranknum<9;ranknum++)  //遍历方差和的数组，找到方差最小的算子，得到其下标
			{
			  if(Minvariance>=variance[ranknum])
				{
				  Minvariance=variance[ranknum];
					tmppointnum=ranknum;
				}			
			}
			
			*pTmpImage=tmppoint[tmppointnum];  //把对应下标的临时像素赋值给临时像素数组
			pTmpImage++;               //指针移动至下一个像素
			pImage++;
			
	    if(i==240-2)          //跳过x轴最后两个像素
			{
				pTmpImage+=2;
			  pImage+=2;
			}
		}
	}
	
	pTmpImage=TmpImage;pImage=Image;
	for(j=1;j<=320;j++)               //对原图像数组赋值
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
	
	//	if(TmpImage!=NULL)	printf("中值滤波内存分配成功\r\n");		
//	else printf("中值滤波内存分配失败\r\n");
	pTmpImage=TmpImage+240;pImage=Image+240; //指向像素数组第二行
	for(j=2;j<=320-1;j++)
	{
		for(i=2;i<=240-1;i++)
		{
			if(i==2)          //跳过x轴第一个像素
			{
				pTmpImage++;
			  pImage++;
			}
			rank[0]=*(pImage-240-1);  //取本像素左上、上、右上、左下、下、右下、左、右、以及自身9点组成数组
			rank[1]=*(pImage-240);
      rank[2]=*(pImage-240+1);
			rank[3]=*(pImage-1);
			rank[4]=*pImage;
			rank[5]=*(pImage+1);
			rank[6]=*(pImage+240-1);
			rank[7]=*(pImage+240);
			rank[8]=*(pImage+240+1);

//			fastsort(rank,rank+4);  //快速排序
			for(ranknum=8;ranknum>0;ranknum--)       //冒泡法排序，从大到小
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

			*pTmpImage=rank[4];    //本像素预赋值中值，存储于临时数组
			pTmpImage++;               //指针移动至下一个像素
			pImage++;
			if(i==240-1)        //跳过x轴最后一个像素
			{
			  pTmpImage++;
			  pImage++;
			}
		}
	}	
	
	pTmpImage=TmpImage;pImage=Image;
	for(j=1;j<=320;j++)               //对原图像数组赋值
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
	printf("中值滤波成功！\r\n");
}

static __IO u8 *GrayGradient_Diffrential(vu8 *GrayImage,u8 GradientThreshold)//返回数组存储其每个像素的梯度，阈值以上的梯度置为255。运用双向差分代替双向微分
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
static __IO u8 *GrayGradient_Roberts(vu8 *GrayImage,u8 GradientThreshold) //Roberts算子求梯度
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
static __IO u8 *GrayGradient_Sobel(vu8 *GrayImage,u8 GradientThreshold) //Sobel算子求梯度
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
	pGrayImage=GrayImage+240;pGradient=Gradient+240; //指向像素数组第二行
	for(j=2;j<=320-1;j++)
	{
		for(i=2;i<=240-1;i++)
		{
			if(i==2)          //跳过x轴第一个像素
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
					
			if(i==240-1)          //跳过x轴最后像素
			{
				pGrayImage++;
			  pGradient++;
			}			
		}
	}

  return Gradient;
}

__IO u8 *GrayGradientToMask(vu8 *Gradient) //基于梯度图制作掩模，遮掩部分为255
{
   vu8 *Mask=Gradient,*pMask=Mask;
	 vu16 i,j;
	
	for(j=1;j<=320;j++)
	{
	  for(i=1;i<=240;i++)
		{
			if(*pMask==255&&*(pMask+1)!=255) //从边沿开始，不断向前迭代，直到下一个边沿，置255
			{
			  pMask++;
				*pMask=255;
			}
			else pMask++;
		}
	}
	
  return Mask;
}

void BinaryToEdge(vu8 *BinaryImage) //掏空内部点法取得前景边沿
{
  vu8 *pBinaryImage=BinaryImage;
	vu8 rank[8]={0},ranknum;
  vu16 i,j;
#ifdef CONFIG_LVGL_USE_MALLOC
	vu8 *Edge=(vu8 *)malloc(240*320),*pEdge=Edge;
#else
	vu8 *Edge=(vu8 *)lv_malloc(240*320),*pEdge=Edge;
#endif /*CONFIG_LVGL_USE_MALLOC*/
	pBinaryImage=BinaryImage+240;pEdge=Edge+240; //指向像素数组第二行
  for(j=2;j<=320-1;j++)
	{
		for(i=2;i<=240-1;i++)
		{
			if(i==2)          //跳过x轴第一个像素
			{
				pBinaryImage++;
			  pEdge++;
			}
			
			if(0==*pBinaryImage)
			{
				rank[0]=*(pBinaryImage-240-1);  //取本像素周围8点组成数组
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
        if(0==rank[0]) *pEdge=255;	//邻域全为黑则为前景，置255
			}	
			pBinaryImage++;
			pEdge++;
						
			if(i==240-1)          //跳过x轴最后一个像素
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
		  if(0!=*pEdge)      //不为0则是需掏空部分，原图置255
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

__IO u8 *ObjectMeasure_FromBinary(vu8 *BinaryImage,vu32 *ObjectSize)//标记各个物体，并返回，标记值从1开始，ObjectSize数组记录每个标记物体占用像素大小
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
	
	pBinaryImage=BinaryImage+240;pObjectFlag=ObjectFlag+240; //指向像素数组第二行
  for(j=2;j<=320-1;j++)
	{
		myASSERT(0==ObjectFillFlag);
		for(i=2;i<=240-1;i++)
		{
			if(i==2)          //跳过x轴第一个像素
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
			
			if(i==240-1)          //跳过x轴最后一个像素
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
	pTmpImage=TmpImage+240;pImage=Image+240; //指向像素数组第二行
	for(j=2;j<=320-1;j++)
	{
		for(i=2;i<=240-1;i++)
		{
			if(i==2)          //跳过x轴第一个像素
			{
				pTmpImage++;
			  pImage++;
			}
			rank[0]=*(pImage-240);  //取本像素上下左右以及自身5点组成数组
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
			pTmpImage++;               //指针移动至下一个像素
			pImage++;
			if(i==240-1)        //跳过x轴最后一个像素
			{
			  pTmpImage++;
			  pImage++;
			}
		}
	}	
	pTmpImage=TmpImage+240;pImage=Image+240; //指向像素数组第二行
	for(j=2;j<=320-1;j++)               //对原图像数组赋值
	{
		for(i=2;i<=240-1;i++)
		{
			if(i==2)          //跳过x轴第一个像素
			{
				pTmpImage++;
			  pImage++;
			}
		
			*pImage++=*pTmpImage;
			pTmpImage++;
			if(i==240-1)        //跳过x轴最后一个像素
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
	pTmpImage=TmpImage+240;pImage=Image+240; //指向像素数组第二行
	for(j=2;j<=320-1;j++)
	{
		for(i=2;i<=240-1;i++)
		{
			if(i==2)          //跳过x轴第一个像素
			{
				pTmpImage++;
			  pImage++;
			}
			rank[0]=*(pImage-240);  ///取本像素上下左右以及自身5点组成数组
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
			pTmpImage++;               //指针移动至下一个像素
			pImage++;
			if(i==240-1)        //跳过x轴最后一个像素
			{
			  pTmpImage++;
			  pImage++;
			}
		}
	}	
	pTmpImage=TmpImage+240;pImage=Image+240; //指向像素数组第二行
	for(j=2;j<=320-1;j++)               //对原图像数组赋值
	{
		for(i=2;i<=240-1;i++)
		{
			if(i==2)          //跳过x轴第一个像素
			{
				pTmpImage++;
			  pImage++;
			}
		
			*pImage++=*pTmpImage;
			pTmpImage++;
			if(i==240-1)        //跳过x轴最后一个像素
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

void CloseOperation(vu8 *Image) //闭运算
{
	 Expand(Image);  //膨胀
   Corrode(Image);  //腐蚀
}

void OpenOperation(vu8 *Image) //开运算
{
   Corrode(Image);  //腐蚀
	 Expand(Image);  //膨胀
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

static u8 CalcThresold_MultiAverage(vu8 *GrayImage) //迭代阈值法
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

static __IO float *CalcGrayHist_Chance(vu8 *GrayImage) //计算灰度直方图，以概率分布的形式保存
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
		  hist[*pGrayImage]++; //对应灰度的直方图数值+1
			pGrayImage++;
		}		
	}
	for(j=1;j<=256;j++)
	{   
		*phistchance=(float)(*phist)/(240*320);//像素数量直方图除以总像素得到概率分布
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
		  *paddupchance=*(paddupchance-1)+*pChanceTmp;		//累计分布概率
		}
		*pmapping++=(u8)((255.0f**paddupchance)+0.5f);    //+0.5f用于取整
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

void GrayEven(vu8 *GrayImage)  //灰度图均匀化
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

void GrayChange_Linear(vu8 *GrayImage,float modulus,int8_t trans) //线性改变灰度值
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
		  hist[*pGrayImage]++; //对应灰度的直方图数值+1
			pGrayImage++;
		}		
	}	
  for(j=1;j<=256;j++)
	{
		if(histMax<hist[j-1])//确定灰度直方图尖峰
	  histMax=hist[j-1];		
	}
	for(j=1;j<=256;j++)
	{
		if(histMax==hist[j-1])//确定灰度尖峰所在灰度
	  histMaxGray=j-1;		
	}
  for(j=histMaxGray;j>0;j--) //向左迭代寻找第二峰
	{
	  if(hist[j]<3840||j==0) //小于5%像素的灰度值为噪声，停止迭代
		{
			jtmp=j;
			break;
		}		
	}
	for(j=jtmp;j>0;j--) //向左迭代寻找第二峰
	{
	  if(hist[j]>3840||j==0) 
		{		  
			jfindpeak++;
			break;
		}	
	}
	for(i=histMaxGray;i<256;i++) //向右迭代寻找第二峰
	{
	  if(hist[i]<3840||i==256) //小于5%像素的灰度值为噪声，停止迭代
		{
			itmp=i;
			break;
		}		
	}
	for(i=itmp;i<256;i++) //向右迭代寻找第二峰
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
	printf("得到灰度图阈值！\r\n");
  return GrayForbinary; //返回值作为二值化的阈值
}

//从灰度图得到二值图
void GrayToBinary(vu8 *GrayImage)
{
  vu16 i,j;
	vu8 *TmpGrayImage=GrayImage;
	vu8 GrayThreshold=CalcThresold_MultiAverage(TmpGrayImage); //从灰度直方图获得阈值
  for(j=1;j<=320;j++)
	{
    for(i=1;i<=240;i++)
		{
		  if(*GrayImage>GrayThreshold) *GrayImage++=0xff;//灰度像素转化为二值像素
			else *GrayImage++=0;				
		}		
	}	
	printf("获取二值图成功！\r\n");
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

//显示二值图像到LCD屏，BinaryImage通常与GrayImage共用一个数组
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
//*offset 小于0表示左移，大于0表示右移
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
				if(i-1<asb8(*poffset))  //使偏移指针指向左边界后相应偏移量
				{
					pImageToAjust++;
					continue;
				}
				else
				{
					
					*(pImageToAjust-asb8(*poffset))=*pImageToAjust; //左侧指针赋值偏移后指针内容，即可实现左移
					pImageToAjust++;
				}
			}					
	  }
		else
		{
		  pImageToAjust+=240-1; //指向行末尾
		  for(i=240;i>=1;i--)
			{
				if(i+1>240-*poffset) //减少迭代次数,防止读取左边界外数据
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
      pImageToAjust+=240-*poffset;//指向下一行
		}
		poffset++;
	}
}
//纵向投影到x轴
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
			if(*pBinaryImage==0)(*pprojection)++; //若像素为黑，投影值+1
			pBinaryImage++;
			pprojection++;
		}	
		pprojection=projection; //指针指向投影数据开头
	}
	
  return projection;
}
//横向投影到y轴
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
		pBinaryImage=BinaryImage+j-1;//指针指向x轴的下一个像素
    for(i=1;i<=320;i++)
		{
		  if(*pBinaryImage==0) (*pprojection)++;
			pBinaryImage+=240;    //像素指针指向本像素的同一x坐标的下一个像素
			pprojection++;
		}	
	}	
	return projection;
}
