#include "usart.h"	
#include <stdlib.h>

#if 0
#pragma import(__use_no_semihosting)
struct __FILE
{
	int handle;
};
FILE __stdout;
void _sys_exit(int x)
{
	x = x;
}
#endif
__INRAM volatile u8 usart_dma_tx_busy_buf1 = 0;
__INRAM volatile u8 usart_dma_tx_busy_buf2 = 0;
__INRAM volatile u8 usartbuf1_ready = 0;
__INRAM volatile u8 usartbuf2_ready = 0;
__INRAM char usartbuf1[DEBUG_BUFF_SIZE];
__INRAM char usartbuf2[DEBUG_BUFF_SIZE];


#ifdef USART_OUTPUT_DEBUG
		#ifdef DEBUG_USART_USE_DMA 
				int fputc(int ch,FILE *p)  //函数默认的，在使用printf函数时自动调用
				{
					volatile static u16 index = 0;
					usartbuf1[index] = (u8)ch;	
					if(usartbuf1[index] == '\0') {
						UsartTX_DMA_Enable(usartbuf1,index + 1);
						index = 0;
						return ch;
					}
					index ++;
					return ch;
				}
		#else
				int fputc(int ch,FILE *p)  //函数默认的，在使用printf函数时自动调用
				{
					//if(USART_GetFlagStatus(USART1,USART_FLAG_IDLE)==RESET) return 0;
					USART_SendData(USART1,(u8)ch);	
					while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
					return ch;
				}
		#endif //DEBUG_USART_USE_DMA
#endif //USART_OUTPUT_DEBUG

/*******************************************************************************
* 函 数 名         : USART1_Init
* 函数功能		   : USART1初始化函数
* 输    入         : bound:波特率
* 输    出         : 无
*******************************************************************************/ 
void USART1_Init(void)
{
   //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	 //打开时钟
 
	
	/*  配置GPIO的模式和IO口 */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//TX			   //串口输出PA9
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;	    //复用推挽输出
	GPIO_Init(GPIOA,&GPIO_InitStructure);  /* 初始化串口输入IO */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//RX			 //串口输入PA10
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;		  //浮空输入
	GPIO_Init(GPIOA,&GPIO_InitStructure); /* 初始化GPIO */
	

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = USART_BOUND;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
	USART_Cmd(USART1, ENABLE);  //使能串口1 
#ifdef DEBUG_USART_USE_DMA	
	UsartTX_DMA_Init(); //初始化DMA传输
#endif //DEBUG_USART_USE_DMA

//	USART_ClearFlag(USART1, USART_FLAG_TC);
		
//	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
//	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
//	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化NVIC寄存器、	
}

void UsartRX_DMA_Init(void)
{
  DMA_InitTypeDef  DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//DMA1时钟使能 
	
	DMA_DeInit(DMA1_Channel5);
	
	/* 配置 DMA Stream */
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);//DMA外设地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)usartbuf1;//DMA 存储器0地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//外设到存储器模式
	DMA_InitStructure.DMA_BufferSize = NULL;//数据传输量 ，初始为0
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据长度:8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//存储器数据长度:8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;// 使用循环模式 
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;//优先级
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道不设置为内存到内存传输
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);//初始化DMA Stream
	
	/* 配置中断NVIC寄存器 */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;  // DMA1通道5中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 14;  // 抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;         // 子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            // 使能中断
	NVIC_Init(&NVIC_InitStructure);
	
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
	DMA_ITConfig(DMA1_Channel5, DMA_IT_TC,ENABLE);

}

void UsartRX_DMA_Enable(uint16_t bufsize)
{
  DMA_Cmd(DMA1_Channel5, DISABLE);                      //关闭DMA传输 
	DMA_SetCurrDataCounter(DMA1_Channel5,bufsize);          //数据传输量   
	DMA_Cmd(DMA1_Channel5, ENABLE);                      //开启DMA传输 
}

void UsartTX_DMA_Init(void)
{
  DMA_InitTypeDef  DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//DMA1时钟使能 
	
	DMA_DeInit(DMA1_Channel4);
	
	/* 配置 DMA Stream */
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);//DMA外设地址
	DMA_InitStructure.DMA_MemoryBaseAddr = 0;//DMA 存储器0地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;//存储器到外设模式
	DMA_InitStructure.DMA_BufferSize = NULL;//数据传输量 ，初始为0
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据长度:8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//存储器数据长度:8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// 不使用循环模式 
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//优先级
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道不设置为内存到内存传输
	DMA_Init(DMA1_Channel4, &DMA_InitStructure);//初始化DMA Stream
  
	/* 配置中断NVIC寄存器 */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;  // DMA1通道4中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 14;  // 抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;         // 子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            // 使能中断
	NVIC_Init(&NVIC_InitStructure);
	
	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC,ENABLE);
}

void UsartTX_DMA_Enable(char *buff,uint16_t bufsize)
{
  DMA_Cmd(DMA1_Channel4, DISABLE);                     	//关闭DMA传输 
	DMA_ClearFlag(DMA1_FLAG_TC4);													//清除传输完成标志位
	DMA_SetCurrDataCounter(DMA1_Channel4,bufsize);        //数据传输量 
  DMA1_Channel4->CMAR = (uint32_t)buff;                 //设置存储器地址
	DMA_Cmd(DMA1_Channel4, ENABLE);                      	//开启DMA传输 
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
}

/*******************************************************************************
* 函 数 名         : USART1_IRQHandler
* 函数功能		   : USART1中断函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/ 
//void USART1_IRQHandler(void)                	//串口1中断服务程序
//{
//	u8 r;
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断
//	{
//		r =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
//		USART_SendData(USART1,r);
//		while(USART_GetFlagStatus(USART1,USART_FLAG_TC) != SET);
//	} 
//	USART_ClearFlag(USART1,USART_FLAG_TC);
//} 	


