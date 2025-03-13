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
				int fputc(int ch,FILE *p)  //����Ĭ�ϵģ���ʹ��printf����ʱ�Զ�����
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
				int fputc(int ch,FILE *p)  //����Ĭ�ϵģ���ʹ��printf����ʱ�Զ�����
				{
					//if(USART_GetFlagStatus(USART1,USART_FLAG_IDLE)==RESET) return 0;
					USART_SendData(USART1,(u8)ch);	
					while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
					return ch;
				}
		#endif //DEBUG_USART_USE_DMA
#endif //USART_OUTPUT_DEBUG

/*******************************************************************************
* �� �� ��         : USART1_Init
* ��������		   : USART1��ʼ������
* ��    ��         : bound:������
* ��    ��         : ��
*******************************************************************************/ 
void USART1_Init(void)
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
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;		  //��������
	GPIO_Init(GPIOA,&GPIO_InitStructure); /* ��ʼ��GPIO */
	

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = USART_BOUND;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	
	USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1 
#ifdef DEBUG_USART_USE_DMA	
	UsartTX_DMA_Init(); //��ʼ��DMA����
#endif //DEBUG_USART_USE_DMA

//	USART_ClearFlag(USART1, USART_FLAG_TC);
		
//	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
//	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
//	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��NVIC�Ĵ�����	
}

void UsartRX_DMA_Init(void)
{
  DMA_InitTypeDef  DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//DMA1ʱ��ʹ�� 
	
	DMA_DeInit(DMA1_Channel5);
	
	/* ���� DMA Stream */
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);//DMA�����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)usartbuf1;//DMA �洢��0��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//���赽�洢��ģʽ
	DMA_InitStructure.DMA_BufferSize = NULL;//���ݴ����� ����ʼΪ0
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�洢������ģʽ
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//�������ݳ���:8λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//�洢�����ݳ���:8λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;// ʹ��ѭ��ģʽ 
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;//���ȼ�
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);//��ʼ��DMA Stream
	
	/* �����ж�NVIC�Ĵ��� */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;  // DMA1ͨ��5�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 14;  // ��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;         // �����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            // ʹ���ж�
	NVIC_Init(&NVIC_InitStructure);
	
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
	DMA_ITConfig(DMA1_Channel5, DMA_IT_TC,ENABLE);

}

void UsartRX_DMA_Enable(uint16_t bufsize)
{
  DMA_Cmd(DMA1_Channel5, DISABLE);                      //�ر�DMA���� 
	DMA_SetCurrDataCounter(DMA1_Channel5,bufsize);          //���ݴ�����   
	DMA_Cmd(DMA1_Channel5, ENABLE);                      //����DMA���� 
}

void UsartTX_DMA_Init(void)
{
  DMA_InitTypeDef  DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//DMA1ʱ��ʹ�� 
	
	DMA_DeInit(DMA1_Channel4);
	
	/* ���� DMA Stream */
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);//DMA�����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = 0;//DMA �洢��0��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;//�洢��������ģʽ
	DMA_InitStructure.DMA_BufferSize = NULL;//���ݴ����� ����ʼΪ0
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�洢������ģʽ
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//�������ݳ���:8λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//�洢�����ݳ���:8λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// ��ʹ��ѭ��ģʽ 
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//���ȼ�
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA1_Channel4, &DMA_InitStructure);//��ʼ��DMA Stream
  
	/* �����ж�NVIC�Ĵ��� */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;  // DMA1ͨ��4�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 14;  // ��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;         // �����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            // ʹ���ж�
	NVIC_Init(&NVIC_InitStructure);
	
	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC,ENABLE);
}

void UsartTX_DMA_Enable(char *buff,uint16_t bufsize)
{
  DMA_Cmd(DMA1_Channel4, DISABLE);                     	//�ر�DMA���� 
	DMA_ClearFlag(DMA1_FLAG_TC4);													//���������ɱ�־λ
	DMA_SetCurrDataCounter(DMA1_Channel4,bufsize);        //���ݴ����� 
  DMA1_Channel4->CMAR = (uint32_t)buff;                 //���ô洢����ַ
	DMA_Cmd(DMA1_Channel4, ENABLE);                      	//����DMA���� 
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
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


