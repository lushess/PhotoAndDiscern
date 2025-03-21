/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "FreeRTOS.h"
#include "task.h"
#include "time.h"
#include "Config/Config.h"
#include "usart.h"
//#include "lvgl/src/lv_conf_internal.h"
//#include "lv_port_disp.h"

#if !LV_TICK_CUSTOM
#include "lvgl/src/hal/lv_hal_tick.h"
#endif //!LV_TICK_CUSTOM
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  UDEBUG("HardFault Error!\r\n");
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
	UDEBUG("内存管理故障地址:0x%lX",*(unsigned long *)SCB->MMFAR);
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
	UDEBUG("总线故障地址:0x%lX",*(unsigned long *)SCB->BFAR);
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

///**
//  * @brief  This function handles SVCall exception.
//  * @param  None
//  * @retval None
//  */
//void SVC_Handler(void)
//{
//}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

///**
//  * @brief  This function handles PendSVC exception.
//  * @param  None
//  * @retval None
//  */
//void PendSV_Handler(void)
//{
//}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
extern void xPortSysTickHandler(void);
//systick中断服务函数
void SysTick_Handler(void)
{	
    #if (INCLUDE_xTaskGetSchedulerState  == 1 )
      if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
      {
    #endif  /* INCLUDE_xTaskGetSchedulerState */  
        xPortSysTickHandler();
				#if !LV_TICK_CUSTOM
				    lv_tick_inc(1);
				#endif
    #if (INCLUDE_xTaskGetSchedulerState  == 1 )
      }
    #endif  /* INCLUDE_xTaskGetSchedulerState */
}

extern volatile u8 usart_dma_tx_busy_buf1;
extern volatile u8 usart_dma_tx_busy_buf2;
void DMA1_Channel4_IRQHandler(void)
{
		if(DMA_GetITStatus(DMA1_IT_TC4)){
				DMA_ClearITPendingBit(DMA1_IT_TC4);
				usart_dma_tx_busy_buf1 = 0;
				usart_dma_tx_busy_buf2 = 0;
		}
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

//volatile uint32_t CPU_RunTime = 0UL;

//void  BASIC_TIM_IRQHandler (void)
//{
////	uint32_t ulReturn;
////	
////	ulReturn=taskENTER_CRITICAL_FROM_ISR();

//	if ( TIM_GetITStatus( BASIC_TIM, TIM_IT_Update) != RESET ) 
//	{	
//    CPU_RunTime++;	
//	  TIM_ClearITPendingBit(BASIC_TIM , TIM_FLAG_Update);  		
//	}	
////   taskEXIT_CRITICAL_FROM_ISR( ulReturn );	
//}


__INRAM static u8 ov_frame=0; 	//统计帧数

/*******************************************************************************
* 函 数 名         : TIM4_IRQHandler
* 函数功能		   : TIM4中断函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
//void TIM4_IRQHandler(void)
//{
//	uint32_t ulReturn;
//	
//	ulReturn=taskENTER_CRITICAL_FROM_ISR();

//	if(TIM_GetITStatus(TIM4,TIM_IT_Update))
//	{
//		printf("frame:%dfps\r\n",ov_frame);	//打印帧率
//		ov_frame=0;
//	}
//	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);	
//  taskEXIT_CRITICAL_FROM_ISR( ulReturn );	
//}



/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
