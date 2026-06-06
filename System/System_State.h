#ifndef __SYSTEM_STATE_H
#define __SYSTEM_STATE_H

#include "ti_msp_dl_config.h"
#include <stdio.h>
#include "oled.h"
#include "ADC.h"
#include "IQDemod.h"

typedef enum
{
    STATE_SELECT_MODE, //选择模式
    STATE_GENEXCITATION,  // 生成分压网络激励
    STATE_STABLE_WAIT,
    STATE_INIT_SAMPLING,  // 初始化采样
    STATE_SAMPLING,  //测量待测元件
    STATE_CALC,  //对采集的数据进行处理，计算出结果并输出
    STATE_SHOWRESULT
}SystemState;

extern volatile SystemState state;
extern volatile bool UR_complete;
extern volatile bool UX_complete;

void state_stable_wait(void);
void state_init_sampling(void);
void state_sampling(void);
void state_calc(void);
void state_showresult(void);

#endif
