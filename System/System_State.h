#ifndef __SYSTEM_STATE_H
#define __SYSTEM_STATE_H

#include "ti_msp_dl_config.h"
#include <stdio.h>
#include "oled.h"
#include "ADC.h"
#include "IQDemod.h"
#include "DDS.h"

typedef enum
{
    STATE_IDLE,             // 空闲，等待按键触发测量
    STATE_GENEXCITATION,    // 生成分压网络激励（预留）
    STATE_STABLE_WAIT,      // 稳定等待
    STATE_INIT_SAMPLING,    // 初始化采样
    STATE_SAMPLING,         // 测量待测元件
    STATE_CALC,             // 对采集的数据进行处理
    STATE_SHOWRESULT        // 显示结果
} SystemState;

extern volatile SystemState state;
extern volatile bool UR_complete;
extern volatile bool UX_complete;
extern IQResult UR_IQ;
extern IQResult UX_IQ;
extern ZResult Z_Data;
extern CResult C_Data;
extern LResult L_Data;

void state_idle(void);
void state_genexcitation(void);
void state_stable_wait(void);
void state_init_sampling(void);
void state_sampling(void);
void state_calc(void);
void state_showresult(void);

#endif
