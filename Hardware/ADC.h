#ifndef __ADC_H
#define __ADC_H

#include "ti_msp_dl_config.h"
#include "System_State.h"

extern uint16_t UR_data[256];
extern uint16_t UX_data[256];
extern char message[128]; // 定义一个足够大的缓冲区存放每行字符串
extern volatile bool UR_complete;
extern volatile bool UX_complete;

void DMA_Init(void);

#endif // __ADC_H
