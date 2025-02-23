#ifndef _exti_H
#define _exti_H

#ifdef __cplusplus
extern "C" {
#endif
	
#include "system.h"

extern volatile u8 ov_sta;	
	
void My_EXTI_Init(void);
void key_EXTI_Init(void);
void EXTI7_Init(void);
void EXTI_ITConfig(uint32_t EXTI_Line,FunctionalState NewState);
	
#ifdef __cplusplus
}
#endif /*extern "C"*/

#endif
