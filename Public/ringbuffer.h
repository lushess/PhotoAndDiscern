#ifndef _RINGBUFFER_H
#define _RINGBUFFER_H

#include "Config/Config.h"

typedef unsigned          char uint8_t;
typedef unsigned short     int uint16_t;
typedef unsigned           int uint32_t;

/*需设置此结构体的初值如
*RingBuffer rb = {
*    .buffer_start = external_sram_base,
*    .buffer_end = external_sram_base + EXTERNAL_SRAM_SIZE,
*    .head = external_sram_base,
*    .tail = external_sram_base
*};
*/
typedef struct {
	uint8_t *bufferStart; //缓冲区基址
	uint8_t *bufferEnd;  //缓冲区尾
	volatile uint8_t *head; //缓存数据头
	volatile uint8_t *tail; //缓存数据尾
}RingBuffer;

typedef uint8_t  rbReturn;
#define rbTrue		0
#define rbFalse		(!rbTrue)

#ifdef __INRAM
extern RingBuffer ringBuffer;
#endif //__INRAM

rbReturn RingBuffer_Init(RingBuffer *rb,uint8_t *addrBase,uint32_t size);
uint32_t RingBuffer_FreeSpace(RingBuffer *rb);
uint32_t RingBuffer_Available(RingBuffer *rb);
rbReturn RingBuffer_Write(RingBuffer *rb, const uint8_t *data, uint32_t len);
rbReturn RingBuffer_Read(RingBuffer *rb, uint8_t *data, uint32_t len);
inline rbReturn RingBuffer_IsEmpty(RingBuffer *rb);

#if USE_FREERTOS
rbReturn RingBuffer_Write_FreeRTOS(RingBuffer *rb, const uint8_t *data, uint16_t len);
rbReturn RingBuffer_Read_FreeRTOS(RingBuffer *rb,uint8_t *data, uint16_t len);
#endif //#if USE_FREERTOS

#endif //_RINGBUFFER_H
