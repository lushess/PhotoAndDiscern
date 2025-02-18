#include "tftlcd.h"
#include "stdlib.h"
#include "font.h" 
#include "usart.h"	 
#include "SysTick.h"	   


//LCD的画笔颜色和背景色	   
u16 FRONT_COLOR=BLACK;	//画笔颜色
u16 BACK_COLOR=WHITE;  //背景色 

_tftlcd_data tftlcd_data; 
  

void TFTLCD_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOG,ENABLE);//使能PORTD,E,G时钟
	
 	//PORTD复用推挽输出  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_14|GPIO_Pin_15;				 //	//PORTD复用推挽输出  
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //复用推挽输出   
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOD, &GPIO_InitStructure); 
  	 
	//PORTE复用推挽输出  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;				 //	//PORTD复用推挽输出  
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //复用推挽输出   
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOE, &GPIO_InitStructure);    	    	 											 

   	//	//PORTG12复用推挽输出 A10	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_12;	 //	//PORTG复用推挽输出  
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //复用推挽输出   
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOG, &GPIO_InitStructure);  
	
}

void TFTLCD_FSMC_Init(void)
{
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  	FSMC_NORSRAMTimingInitTypeDef  FSMC_ReadTimingInitStructure; 
	FSMC_NORSRAMTimingInitTypeDef  FSMC_WriteTimingInitStructure;
	
  	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);	//使能FSMC时钟
	

	FSMC_ReadTimingInitStructure.FSMC_AddressSetupTime = 0x01;	 //地址建立时间（ADDSET）为2个HCLK 1/36M=27ns
  	FSMC_ReadTimingInitStructure.FSMC_AddressHoldTime = 0x00;	 //地址保持时间（ADDHLD）模式A未用到	
  	FSMC_ReadTimingInitStructure.FSMC_DataSetupTime = 0x0f;		 // 数据保存时间为16个HCLK,因为液晶驱动IC的读数据的时候，速度不能太快，尤其对1289这个IC。
  	FSMC_ReadTimingInitStructure.FSMC_BusTurnAroundDuration = 0x00;
  	FSMC_ReadTimingInitStructure.FSMC_CLKDivision = 0x00;
  	FSMC_ReadTimingInitStructure.FSMC_DataLatency = 0x00;
  	FSMC_ReadTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;	 //模式A 
    

#ifndef TFTLCD_ILI9486
#if 0
	FSMC_WriteTimingInitStructure.FSMC_AddressSetupTime = 0x00;	 //地址建立时间（ADDSET）为1个HCLK  
  	FSMC_WriteTimingInitStructure.FSMC_AddressHoldTime = 0x00;	 //地址保持时间		
  	FSMC_WriteTimingInitStructure.FSMC_DataSetupTime = 0x03;		 //数据保存时间为4个HCLK	
  	FSMC_WriteTimingInitStructure.FSMC_BusTurnAroundDuration = 0x00;
  	FSMC_WriteTimingInitStructure.FSMC_CLKDivision = 0x00;
  	FSMC_WriteTimingInitStructure.FSMC_DataLatency = 0x00;
  	FSMC_WriteTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;	 //模式A 
#endif
#if 1
	FSMC_WriteTimingInitStructure.FSMC_AddressSetupTime = 0x15;	 //地址建立时间（ADDSET）为16个HCLK  
  	FSMC_WriteTimingInitStructure.FSMC_AddressHoldTime = 0x15;	 //地址保持时间		
  	FSMC_WriteTimingInitStructure.FSMC_DataSetupTime = 0x05;		 //数据保存时间为6个HCLK	
  	FSMC_WriteTimingInitStructure.FSMC_BusTurnAroundDuration = 0x00;
  	FSMC_WriteTimingInitStructure.FSMC_CLKDivision = 0x00;
  	FSMC_WriteTimingInitStructure.FSMC_DataLatency = 0x00;
  	FSMC_WriteTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;	 //模式A  
#endif

#else
	FSMC_WriteTimingInitStructure.FSMC_AddressSetupTime = 0x15;	 //地址建立时间（ADDSET）为16个HCLK  
  	FSMC_WriteTimingInitStructure.FSMC_AddressHoldTime = 0x15;	 //地址保持时间		
  	FSMC_WriteTimingInitStructure.FSMC_DataSetupTime = 0x05;		 //数据保存时间为6个HCLK	
  	FSMC_WriteTimingInitStructure.FSMC_BusTurnAroundDuration = 0x00;
  	FSMC_WriteTimingInitStructure.FSMC_CLKDivision = 0x00;
  	FSMC_WriteTimingInitStructure.FSMC_DataLatency = 0x00;
  	FSMC_WriteTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;	 //模式A 	
#endif

 
  	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;//  这里我们使用NE4 ，也就对应BTCR[6],[7]。
  	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; // 不复用数据地址
  	FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;// FSMC_MemoryType_SRAM;  //SRAM   
  	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//存储器数据宽度为16bit   
  	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;// FSMC_BurstAccessMode_Disable; 
  	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable; 
  	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;   
  	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;  
  	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;	//  存储器写使能
  	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;   
  	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable; // 读写使用不同的时序
  	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable; 
  	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &FSMC_ReadTimingInitStructure; //读写时序
  	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &FSMC_WriteTimingInitStructure;  //写时序

  	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //初始化FSMC配置

 	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);  // 使能BANK1 
	
}
	 
//写寄存器函数
//cmd:寄存器值
__inline void LCD_WriteCmd(u16 cmd)
{

#ifdef TFTLCD_R61509V3	
	TFTLCD->LCD_CMD=cmd;//写入要写的寄存器序号
#endif
	
}

//写数据
//data:要写入的值
__inline void LCD_WriteData(u16 data)
{

#ifdef TFTLCD_R61509V3	
	TFTLCD->LCD_DATA=data;//写入要写的寄存器序号
#endif
	
}

__inline void LCD_WriteCmdData(u16 cmd,u16 data)
{
	LCD_WriteCmd(cmd);
	LCD_WriteData(data);
}


u32 LCD_RGBColor_Change(u16 color)
{
	u8 r,g,b=0;
	
	r=(color>>11)&0x1f;
	g=(color>>5)&0x3f;
	b=color&0x1f;
	
	return ((r<<13)|(g<<6)|(b<<1));
}
__inline void LCD_WriteData_Color(u16 color)
{
	
#ifdef TFTLCD_R61509V3
			TFTLCD->LCD_DATA=color;
#endif

}

//读数据
//返回值:读到的值
u16 LCD_ReadData(void)
{

#ifdef TFTLCD_R61509V3
	return TFTLCD->LCD_DATA;
#endif
	
}


//设置LCD显示方向
//dir:0,竖屏；1,横屏
void LCD_Display_Dir(u8 dir)
{
	if(dir==0)  //默认竖屏方向
	{		
		
#ifdef TFTLCD_R61509V3		
		LCD_WriteCmd(0x0001);   
		LCD_WriteData(0x0100);
		LCD_WriteCmd(0x0003);
		LCD_WriteData(0x1030);
		tftlcd_data.height=400;
		tftlcd_data.width=240;
#endif
		
		tftlcd_data.dir=0;
		
	}
	else
	{	

#ifdef TFTLCD_R61509V3		
		LCD_WriteCmd(0x0001);   
		LCD_WriteData(0x0000);
		LCD_WriteCmd(0x0003);
		LCD_WriteData(0x1038);
		tftlcd_data.height=240;
		tftlcd_data.width=400;
#endif
		
		tftlcd_data.dir=1;
	}	
}

void TFTLCD_Init(void)
{
	u16 i;
	TFTLCD_GPIO_Init();
	TFTLCD_FSMC_Init();
	
	delay_ms(50); 
	
#ifdef TFTLCD_R61509V3	
	LCD_WriteCmd(0X0000);				   	
	tftlcd_data.id=LCD_ReadData();    
#endif	
	
	UDEBUG(" LCD ID:%x\r\n",tftlcd_data.id); //打印LCD ID

#ifdef TFTLCD_R61509V3
	/* --R61509V_CPT3.0 --- */

	LCD_WriteCmd(0x0000);
	LCD_WriteCmd(0x0000);
	delay_ms(10);
	LCD_WriteCmd(0x0000);
	LCD_WriteCmd(0x0000);
	LCD_WriteCmd(0x0000);
	LCD_WriteCmd(0x0000);
	LCD_WriteCmd(0x0000);
	LCD_WriteCmd(0x0000);
	LCD_WriteCmd(0x0000);
	LCD_WriteCmd(0x0000);
	delay_ms(10);
	
	LCD_WriteCmd(0x0400); LCD_WriteData(0x6200);
	LCD_WriteCmd(0x0008); LCD_WriteData(0x0808);
	
	LCD_WriteCmd(0x0300); LCD_WriteData(0x0005);//gamma
	LCD_WriteCmd(0x0301); LCD_WriteData(0x4C06);
	LCD_WriteCmd(0x0302); LCD_WriteData(0x0602);
	LCD_WriteCmd(0x0303); LCD_WriteData(0x050C);
	LCD_WriteCmd(0x0304); LCD_WriteData(0x3300);
	LCD_WriteCmd(0x0305); LCD_WriteData(0x0C05);
	LCD_WriteCmd(0x0306); LCD_WriteData(0x4206);
	LCD_WriteCmd(0x0307); LCD_WriteData(0x060C);
	LCD_WriteCmd(0x0308); LCD_WriteData(0x0500);
	LCD_WriteCmd(0x0309); LCD_WriteData(0x0033);
	
	LCD_WriteCmd(0x0010); LCD_WriteData(0x0014);//
	LCD_WriteCmd(0x0011); LCD_WriteData(0x0101);//
	LCD_WriteCmd(0x0012); LCD_WriteData(0x0000);//
	LCD_WriteCmd(0x0013); LCD_WriteData(0x0001);//
	
	LCD_WriteCmd(0x0100); LCD_WriteData(0x0330);//BT AP
	LCD_WriteCmd(0x0101); LCD_WriteData(0x0247);//DC0,DC1,VC
	LCD_WriteCmd(0x0103); LCD_WriteData(0x1000);//VDV
	LCD_WriteCmd(0x0280); LCD_WriteData(0xDE00);//VCM
	LCD_WriteCmd(0x0102); LCD_WriteData(0xD1B0);//VR,VCMR,PSON,PON
	delay_ms(10);
	
	LCD_WriteCmd(0x0001); LCD_WriteData(0x0100);
	LCD_WriteCmd(0x0002); LCD_WriteData(0x0100);
	LCD_WriteCmd(0x0003); LCD_WriteData(0x1030);
	LCD_WriteCmd(0x0009); LCD_WriteData(0x0001);
	LCD_WriteCmd(0x000C); LCD_WriteData(0x0000);
	LCD_WriteCmd(0x0090); LCD_WriteData(0x8000);
	LCD_WriteCmd(0x000F); LCD_WriteData(0x0000);
	LCD_WriteCmd(0x0210); LCD_WriteData(0x0000);
	LCD_WriteCmd(0x0211); LCD_WriteData(0x00EF);
	LCD_WriteCmd(0x0212); LCD_WriteData(0x0000);
	LCD_WriteCmd(0x0213); LCD_WriteData(0x018F);//432=1AF, 400=18F
	LCD_WriteCmd(0x0500); LCD_WriteData(0x0000);
	LCD_WriteCmd(0x0501); LCD_WriteData(0x0000);
	LCD_WriteCmd(0x0502); LCD_WriteData(0x005F);
	LCD_WriteCmd(0x0401); LCD_WriteData(0x0001);
	LCD_WriteCmd(0x0404); LCD_WriteData(0x0000);
	delay_ms(10);
							 
	LCD_WriteCmd(0x0007); LCD_WriteData(0x0100);//BASEE
	delay_ms(10);
	
	LCD_WriteCmd(0x0200); LCD_WriteData(0x0000);
	LCD_WriteCmd(0x0201); LCD_WriteData(0x0000);
	delay_ms(10);
	LCD_WriteCmd(0x0202);
#endif

	LCD_Display_Dir(TFTLCD_DIR);		//0：竖屏  1：横屏  默认竖屏
	LCD_Clear(WHITE);
}

//设置窗口,并自动设置画点坐标到窗口左上角(sx,sy).
//sx,sy:窗口起始坐标(左上角)
//width,height:窗口宽度和高度,必须大于0!!
//窗体大小:width*height. 
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height)
{    	

#ifdef TFTLCD_R61509V3	
	if(tftlcd_data.dir==0)
	{
		LCD_WriteCmd(0x0210);   
	    LCD_WriteData(sx);
		LCD_WriteCmd(0x0211);  
	    LCD_WriteData(width);
		LCD_WriteCmd(0x0212);   
	    LCD_WriteData(sy);
		LCD_WriteCmd(0x0213);   
	    LCD_WriteData(height);
	
		LCD_WriteCmd(0x0200);   
	    LCD_WriteData(sx);
		LCD_WriteCmd(0x0201);   
	    LCD_WriteData(sy);	
	}   	
	else
	{
		LCD_WriteCmd(0x0212);   
	    LCD_WriteData(sx);
		LCD_WriteCmd(0x0213);  
	    LCD_WriteData(width);
		LCD_WriteCmd(0x0210);   
	    LCD_WriteData(sy);
		LCD_WriteCmd(0x0211);   
	    LCD_WriteData(height);
	
		LCD_WriteCmd(0x0201);   
	    LCD_WriteData(sx);
		LCD_WriteCmd(0x0200);   
	    LCD_WriteData(sy);	
	}
	LCD_WriteCmd(0x0202);
#endif

}

//清屏函数
//color:要清屏的填充色
void LCD_Clear(u16 color)
{
	uint16_t i, j ;

	LCD_Set_Window(0, 0, tftlcd_data.width-1, tftlcd_data.height-1);	 //作用区域
  	for(i=0; i<tftlcd_data.width; i++)
	{
		for (j=0; j<tftlcd_data.height; j++)
		{
			LCD_WriteData_Color(color);
		}
	} 
}

//在指定区域内填充单个颜色
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
void LCD_Fill(u16 xState,u16 yState,u16 xEnd,u16 yEnd,u16 color)
{          
	uint16_t temp;

    if((xState > xEnd) || (yState > yEnd))
    {
        return;
    }   
	LCD_Set_Window(xState, yState, xEnd, yEnd); 
    xState = xEnd - xState + 1;
	yState = yEnd - yState + 1;

	while(xState--)
	{
	 	temp = yState;
		while (temp--)
	 	{			
			LCD_WriteData_Color(color);	
		}
	}	
} 

//在指定区域内填充多个颜色
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
void LCD_Fill_ColorBuf(u16 xState,u16 yState,u16 xEnd,u16 yEnd,u16 *color)
{          
	uint16_t i,j; 
	uint16_t height,width;     
	width = xEnd - xState + 1;          
	height = yEnd - yState + 1;   
	LCD_Set_Window(xState,yState,xEnd,yEnd);     

	for(i=0; i<height; i++)   
	{   
			for(j=0; j<width; j++)
			{
					LCD_WriteData_Color(color[i*width+j]);
			}
	}  	  
} 

//画点
//x,y:坐标
//FRONT_COLOR:此点的颜色
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_Set_Window(x, y, x, y);  //设置点的位置
	LCD_WriteData_Color(FRONT_COLOR);	
}

//快速画点
//x,y:坐标
//color:颜色
void LCD_DrawFRONT_COLOR(u16 x,u16 y,u16 color)
{	   
	LCD_Set_Window(x, y, x, y);
	LCD_WriteData_Color(color);	
} 

//读取个某点的颜色值	 
//x,y:坐标
//返回值:此点的颜色
u16 LCD_ReadPoint(u16 x,u16 y)
{
 	u16 r=0,g=0,b=0;
	u16 r1,r2,r3;
	u32 value;
	
	if(x>=tftlcd_data.width||y>=tftlcd_data.height)return 0;	//超过了范围,直接返回		     
	LCD_Set_Window(x, y, x, y);


#ifdef TFTLCD_R61509V3	
	LCD_WriteCmd(0X0202);     		 				    
 	r=LCD_ReadData();								//dummy Read	   
 	r=LCD_ReadData(); 
	r=LCD_ReadData();	  						
#endif

 	return r;						
}

//画线
//x1,y1:起点坐标
//x2,y2:终点坐标  
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		LCD_DrawPoint(uRow,uCol);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
} 

void LCD_DrawLine_Color(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	u16 incx,incy,uRow,uCol; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		LCD_DrawFRONT_COLOR(uRow,uCol,color);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
} 


// 画一个十字的标记
// x：标记的X坐标
// y：标记的Y坐标
// color：标记的颜色
void LCD_DrowSign(uint16_t x, uint16_t y, uint16_t color)
{
    uint8_t i;

    /* 画点 */
    LCD_Set_Window(x-1, y-1, x+1, y+1);
    for(i=0; i<9; i++)
    {
		LCD_WriteData_Color(color);   
    }

    /* 画竖 */
    LCD_Set_Window(x-4, y, x+4, y);
    for(i=0; i<9; i++)
    {
		LCD_WriteData_Color(color); 
    }

    /* 画横 */
    LCD_Set_Window(x, y-4, x, y+4);
    for(i=0; i<9; i++)
    {
		LCD_WriteData_Color(color); 
    }
}

//画矩形	  
//(x1,y1),(x2,y2):矩形的对角坐标
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}
//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		LCD_DrawPoint(x0+a,y0-b);             //5
 		LCD_DrawPoint(x0+b,y0-a);             //0           
		LCD_DrawPoint(x0+b,y0+a);             //4               
		LCD_DrawPoint(x0+a,y0+b);             //6 
		LCD_DrawPoint(x0-a,y0+b);             //1       
 		LCD_DrawPoint(x0-b,y0+a);             
		LCD_DrawPoint(x0-a,y0-b);             //2             
  		LCD_DrawPoint(x0-b,y0-a);             //7     	         
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
} 



//在指定位置显示一个字符
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16/24
//mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{  							  
    u8 temp,t1,t;
	u16 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数	
 	num=num-' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
	for(t=0;t<csize;t++)
	{   
		if(size==12)temp=ascii_1206[num][t]; 	 	//调用1206字体
		else if(size==16)temp=ascii_1608[num][t];	//调用1608字体
		else if(size==24)temp=ascii_2412[num][t];	//调用2412字体
		else return;								//没有的字库
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)LCD_DrawFRONT_COLOR(x,y,FRONT_COLOR);
			else if(mode==0)LCD_DrawFRONT_COLOR(x,y,BACK_COLOR);
			temp<<=1;
			y++;
			if(y>=tftlcd_data.height)return;		//超区域了
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=tftlcd_data.width)return;	//超区域了
				break;
			}
		}  	 
	}  	    	   	 	  
}   
//m^n函数
//返回值:m^n次方.
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}			 
//显示数字,高位为0,则不显示
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//color:颜色 
//num:数值(0~4294967295);	 
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,' ',size,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0); 
	}
} 

//显示数字,高位为0,还是显示
//x,y:起点坐标
//num:数值(0~999999999);	 
//len:长度(即要显示的位数)
//size:字体大小
//mode:
//[7]:0,不填充;1,填充0.
//[6:1]:保留
//[0]:0,非叠加显示;1,叠加显示.
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
{  
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)LCD_ShowChar(x+(size/2)*t,y,'0',size,mode&0X01);  
				else LCD_ShowChar(x+(size/2)*t,y,' ',size,mode&0X01);  
 				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,mode&0X01); 
	}
} 
//显示字符串
//x,y:起点坐标
//width,height:区域大小  
//size:字体大小
//*p:字符串起始地址		  
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{         
	u8 x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//退出
        LCD_ShowChar(x,y,*p,size,0);
        x+=size/2;
        p++;
    }  
}

/****************************************************************************
*函数名：LCD_ShowFontHZ
*输  入：x：汉字显示的X坐标
*      * y：汉字显示的Y坐标
*      * cn：要显示的汉字
*      * wordColor：文字的颜色
*      * backColor：背景颜色
*输  出：
*功  能：写二号楷体汉字
****************************************************************************/
#if 0
void LCD_ShowFontHZ(u16 x, u16 y, u8 *cn)	 
{  
	u8 i, j, wordNum;
	u16 color;
	while (*cn != '\0')
	{
		LCD_Set_Window(x, y, x+31, y+28);
		for (wordNum=0; wordNum<20; wordNum++)
		{	//wordNum扫描字库的字数
			if ((CnChar32x29[wordNum].Index[0]==*cn)
			     &&(CnChar32x29[wordNum].Index[1]==*(cn+1)))
			{
				for(i=0; i<116; i++) 
				{	//MSK的位数
					color=CnChar32x29[wordNum].Msk[i];
					for(j=0;j<8;j++) 
					{
						if((color&0x80)==0x80)
						{
							LCD_WriteData_Color(FRONT_COLOR); 						
						} 						
						else
						{
							LCD_WriteData_Color(BACK_COLOR); 
						} 
						color<<=1;
					}//for(j=0;j<8;j++)结束
				}    
			}
		} //for (wordNum=0; wordNum<20; wordNum++)结束 	
		cn += 2;
		x += 32;
	}
}
#endif


#if 1
void LCD_ShowFontHZ(u16 x, u16 y, u8 *cn)
{
	u8 i, j, wordNum;
	u16 color;
	u16 x0=x; 
	u16 y0=y; 
	while (*cn != '\0')
	{
		for (wordNum=0; wordNum<20; wordNum++)
		{	//wordNum扫描字库的字数
			if ((CnChar32x29[wordNum].Index[0]==*cn)
			     &&(CnChar32x29[wordNum].Index[1]==*(cn+1)))
			{
				for(i=0; i<116; i++) 
				{	//MSK的位数
					color=CnChar32x29[wordNum].Msk[i];
					for(j=0;j<8;j++) 
					{
						if((color&0x80)==0x80)
						{
							LCD_DrawFRONT_COLOR(x,y,FRONT_COLOR);
						} 						
						else
						{
							LCD_DrawFRONT_COLOR(x,y,BACK_COLOR);
						} 
						color<<=1;
						x++;
						if((x-x0)==32)
						{
							x=x0;
							y++;
							if((y-y0)==29)
							{
								y=y0;
							}
						}
					}//for(j=0;j<8;j++)结束
				}	
			}
			
		} //for (wordNum=0; wordNum<20; wordNum++)结束 	
		cn += 2;
		x += 32;
		x0=x;
	}
}	
#endif

void LCD_ShowPicture(u16 x, u16 y, u16 wide, u16 high,u8 *pic)
{
	u16 temp = 0;
	long tmp=0,num=0;
	LCD_Set_Window(x, y, x+wide-1, y+high-1);
	num = wide * high*2 ;
	do
	{  
		temp = pic[tmp + 1];
		temp = temp << 8;
		temp = temp | pic[tmp];
		LCD_WriteData_Color(temp);//逐点显示
		tmp += 2;
	}
	while(tmp < num);	
}

void LCD_DMA_Init(void)
{
  DMA_InitTypeDef  DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);//DMA2时钟使能 
	
	DMA_DeInit(DMA2_Channel5);
	
	/* 配置 DMA Stream */
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&(TFTLCD->LCD_DATA);//DMA外设地址
	DMA_InitStructure.DMA_MemoryBaseAddr = NULL;//DMA 存储器0地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;//存储器到外设模式
	DMA_InitStructure.DMA_BufferSize = NULL;//数据传输量，初始为0
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//外设数据长度:16位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//存储器数据长度:16位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// 使用普通模式 
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;//优先级
	DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;  //DMA通道设置为内存到内存传输
	DMA_Init(DMA2_Channel5, &DMA_InitStructure);//初始化DMA Stream
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel4_5_IRQn;//中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	
	
	DMA_ITConfig(DMA2_Channel5, DMA_IT_TC,ENABLE);
}

void LCD_DMA_Enable(u16 *src, u16 ndtr)
{
  DMA_Cmd(DMA2_Channel5, DISABLE);                      //关闭DMA传输 	
	DMA2_Channel5->CMAR = (uint32_t)src;
	DMA_SetCurrDataCounter(DMA2_Channel5,ndtr);          //数据传输量   
	DMA_Cmd(DMA2_Channel5, ENABLE);                      //开启DMA传输 
}
