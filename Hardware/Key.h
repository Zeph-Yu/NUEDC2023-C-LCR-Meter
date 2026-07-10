#ifndef KEY_H
#define KEY_H

#include "ti_msp_dl_config.h"
#include "System_State.h"

#define KEY_PRESSED 			1
#define KEY_UNPRESSED 			0

#define KEY1_TRIGGER_LEVEL		1 //PA_18高电平有效
#define KEY2_TRIGGER_LEVEL		0 //PB_21低电平有效

#define KEY_NUM_KEYS			2
#define KEY_CYCLE_TIME          10 // ms

//按键结构体
typedef struct
{
    GPIO_Regs* Port;
    uint32_t Pins;
    uint8_t TriggerLevel;
}Key_t;

void Key_Tick(void);

#endif // KEY_H