#ifndef _TIMERCAPTURE_H
#define _TIMERCAPTURE_H

#include "system.h"

extern u8 TIM5_CH1_CAPTURE_STA; //���벶���״̬
extern u16 TIM5_CH1_CAPTURE_VAL;//���벶��ֵ


void TIM5_CH1_Input_Init(u16 arr,u16 psc);
	
#endif /* _TIMERCAPTURE_H */
