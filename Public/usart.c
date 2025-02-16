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
int fputc(int ch,FILE *p)  //����Ĭ�ϵģ���ʹ��printf����ʱ�Զ�����
{
	//if(USART_GetFlagStatus(USART1,USART_FLAG_IDLE)==RESET) return 0;
	USART_SendData(USART1,(u8)ch);	
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	return ch;
}

/*******************************************************************************
* �� �� ��         : USART1_Init
* ��������		   : USART1��ʼ������
* ��    ��         : bound:������
* ��    ��         : ��
*******************************************************************************/ 
void USART1_Init()
{
   //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	 //��ʱ��
 
	
	/*  ����GPIO��ģʽ��IO�� */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//TX			   //�������PA9
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;	    //�����������
	GPIO_Init(GPIOA,&GPIO_InitStructure);  /* ��ʼ����������IO */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//RX			 //��������PA10
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;		  //ģ������
	GPIO_Init(GPIOA,&GPIO_InitStructure); /* ��ʼ��GPIO */
	

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = USARTbound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	
	USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1 
	
//	USART_ClearFlag(USART1, USART_FLAG_TC);
		
//	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
//	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
//	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����	
}

u8 *usartbuf = NULL;
void UsartRX_DMA_Init(void)
{
  DMA_InitTypeDef  DMA_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//DMA1ʱ��ʹ�� 
	
	DMA_DeInit(DMA1_Channel5);
	
	/* ���� DMA Stream */
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);//DMA�����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)usartbuf;//DMA �洢��0��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//���赽�洢��ģʽ
	DMA_InitStructure.DMA_BufferSize = NULL;//���ݴ����� ����ʼΪ0
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�洢������ģʽ
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//�������ݳ���:8λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//�洢�����ݳ���:8λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;// ʹ��ѭ��ģʽ 
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;//���ȼ�
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);//��ʼ��DMA Stream
	
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
	DMA_ITConfig(DMA1_Channel5, DMA_IT_TC,ENABLE);
}

void UsartRX_DMA_Enable(uint16_t bufsize)
{
  DMA_Cmd(DMA1_Channel5, DISABLE);                      //�ر�DMA���� 
	if(usartbuf != NULL) free((void *)usartbuf);
	usartbuf = (u8 *)malloc(bufsize);
	DMA_SetCurrDataCounter(DMA1_Channel5,bufsize);          //���ݴ�����   
	DMA_Cmd(DMA1_Channel5, ENABLE);                      //����DMA���� 
}

/*******************************************************************************
* �� �� ��         : USART1_IRQHandler
* ��������		   : USART1�жϺ���
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/ 
//void USART1_IRQHandler(void)                	//����1�жϷ������
//{
//	u8 r;
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�
//	{
//		r =USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������
//		USART_SendData(USART1,r);
//		while(USART_GetFlagStatus(USART1,USART_FLAG_TC) != SET);
//	} 
//	USART_ClearFlag(USART1,USART_FLAG_TC);
//} 	


