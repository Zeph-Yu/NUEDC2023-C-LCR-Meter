/* DriverLib Includes */
#include "ti_msp_dl_config.h"
#include "ad9959.h"
/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// 初始化DDS模块
void DDS_Init(void)
{
    AD9959_Init();
}

// 生成分压网络的激励信号
void DDS_GenExcitation(void)
{
    // 1. 设置频率
    AD9959_Set_frequence(0, 100000);

    // 2. 设置相位
    AD9959_Set_Phase(0, 0);

    // 3. 设置幅度
    AD9959_Set_Amplitude(0, 2000);

    //这里是同步更新
    AD9959_Update();
}

// 生成参考信号sin
void DDS_GenReference_sine(void)
{
    AD9959_Set_frequence(1, 10000);
    AD9959_Set_Phase(1, 90);
    AD9959_Set_Amplitude(1, 1000);
    //每次设置完DDS最好统一更新下
    AD9959_Update();
}

// 生成参考信号cos
void DDS_GenReference_cosine(void)
{
    AD9959_Set_frequence(1, 10000);
    AD9959_Set_Phase(1, 0);
    AD9959_Set_Amplitude(1, 1000);
    //每次设置完DDS最好统一更新下
    AD9959_Update();
}
