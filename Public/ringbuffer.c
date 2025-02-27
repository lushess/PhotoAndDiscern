#include "ringbuffer.h"
#include <string.h>

#ifdef __INRAM
__INRAM RingBuffer ringBuffer; //结构体置于内部SRAM，方便管理
#endif //__INRAM

rbReturn RingBuffer_Init(RingBuffer *rb,uint8_t *addrBase,uint32_t size){
	rb->bufferStart = addrBase;
	rb->bufferEnd = addrBase + size;
	rb->head = addrBase;
	rb->tail = addrBase;
}

// 可用写入空间（字节数）
uint32_t RingBuffer_FreeSpace(RingBuffer *rb) {
    if (rb->head >= rb->tail) {
        return (rb->head - rb->tail);
    } else {
        return (rb->bufferEnd - rb->tail) + (rb->head - rb->bufferStart);
    }
}

// 可读取数据量（字节数）
uint32_t RingBuffer_Available(RingBuffer *rb) {
    if (rb->head >= rb->tail) {
        return (rb->bufferEnd - rb->head) + (rb->tail - rb->bufferStart);
    } else {
        return (rb->tail - rb->head);
    }
}

rbReturn RingBuffer_Write(RingBuffer *rb, const uint8_t *data, uint32_t len) {
    uint32_t free_space = RingBuffer_FreeSpace(rb);
    if (free_space < len) return rbFalse; // 缓冲区满

    // 分两段写入（处理指针绕回）
    uint32_t first_chunk = rb->bufferEnd - rb->tail;
    if (first_chunk >= len) {
        memcpy((void *)rb->tail, data, len);
        rb->tail += len;
    } else {
        memcpy((void *)rb->tail, data, first_chunk);
        memcpy(rb->bufferStart, data + first_chunk, len - first_chunk);
        rb->tail = rb->bufferStart + (len - first_chunk);
    }

    // 处理指针绕回
    if (rb->tail >= rb->bufferEnd) {
        rb->tail = rb->bufferStart;
    }
    return rbTrue;
}

rbReturn RingBuffer_Read(RingBuffer *rb, uint8_t *data, uint32_t len) {
    uint32_t available = RingBuffer_Available(rb);
    if (available < len) return rbFalse; // 数据不足

    // 分两段读取
    uint32_t first_chunk = rb->bufferEnd - rb->head;
    if (first_chunk >= len) {
        memcpy(data, (void *)rb->head, len);
        rb->head += len;
    } else {
        memcpy(data, (void *)rb->head, first_chunk);
        memcpy(data + first_chunk, rb->bufferStart, len - first_chunk);
        rb->head = rb->bufferStart + (len - first_chunk);
    }

    // 处理指针绕回
    if (rb->head >= rb->bufferEnd) {
        rb->head = rb->bufferStart;
    }
    return rbTrue;
}

inline rbReturn RingBuffer_IsEmpty(RingBuffer *rb){
	return (rb->head == rb->tail)? rbTrue : rbFalse;
}


#if USE_FREERTOS
#include "freeRTOS.h"
#include "semphr.h"
// 添加互斥锁保护
static SemaphoreHandle_t rb_mutex = NULL;

rbReturn RingBuffer_Write_FreeRTOS(RingBuffer *rb, const uint8_t *data, uint16_t len) {
#if (INCLUDE_xTaskGetSchedulerState  == 1 )
      if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
      {
#endif  /* INCLUDE_xTaskGetSchedulerState */  
					if (rb_mutex == NULL) rb_mutex = xSemaphoreCreateMutex();
					if (xSemaphoreTake(rb_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
							rbReturn result = RingBuffer_Write(rb, data, len);
							xSemaphoreGive(rb_mutex);
							return result;
					}
					return rbFalse; // 获取锁超时
#if (INCLUDE_xTaskGetSchedulerState  == 1 )
			}
#endif  /* INCLUDE_xTaskGetSchedulerState */
}

rbReturn RingBuffer_Read_FreeRTOS(RingBuffer *rb,uint8_t *data, uint16_t len) {
#if (INCLUDE_xTaskGetSchedulerState  == 1 )
      if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
      {
#endif  /* INCLUDE_xTaskGetSchedulerState */  
					if (rb_mutex == NULL) rb_mutex = xSemaphoreCreateMutex();
					if (xSemaphoreTake(rb_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
							rbReturn result = RingBuffer_Read(rb, data, len);
							xSemaphoreGive(rb_mutex);
							return result;
					}
					return rbFalse; // 获取锁超时
#if (INCLUDE_xTaskGetSchedulerState  == 1 )
			}
#endif  /* INCLUDE_xTaskGetSchedulerState */
}

#endif //INC_FREERTOS_H
