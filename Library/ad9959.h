/*
 * ad9959.h
 *
 *  Created on: 2025年5月27日
 *      Author: zhangchenglin
 */

#ifndef AD9959_H_
#define AD9959_H_

/* DriverLib Includes */
#include "ti_msp_dl_config.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

//AD9959管脚宏定义
// SCLK
#define AD9959_SCLK_Clr() DL_GPIO_clearPins(GPIOB, DL_GPIO_PIN_23)
#define AD9959_SCLK_Set() DL_GPIO_setPins(GPIOB, DL_GPIO_PIN_23)
// SDIO
#define AD9959_SDIO_Clr() DL_GPIO_clearPins(GPIOB, DL_GPIO_PIN_6)
#define AD9959_SDIO_Set() DL_GPIO_setPins(GPIOB, DL_GPIO_PIN_6)
// CSB
#define AD9959_CSB_Clr()  DL_GPIO_clearPins(GPIOB, DL_GPIO_PIN_27)
#define AD9959_CSB_Set()  DL_GPIO_setPins(GPIOB, DL_GPIO_PIN_27)
// IOUP
#define AD9959_IOUP_Clr()  DL_GPIO_clearPins(GPIOA, DL_GPIO_PIN_29)
#define AD9959_IOUP_Set()  DL_GPIO_setPins(GPIOA, DL_GPIO_PIN_29)
// RST
#define AD9959_RST_Clr()  DL_GPIO_clearPins(GPIOB, DL_GPIO_PIN_7)
#define AD9959_RST_Set()  DL_GPIO_setPins(GPIOB, DL_GPIO_PIN_7)

//AD9959寄存器地址定义
#define CSR_ADD   0x00   //CSR 通道选择寄存器
#define FR1_ADD   0x01   //FR1 功能寄存器1
#define FR2_ADD   0x02   //FR2 功能寄存器2
#define CFR_ADD   0x03   //CFR 通道功能寄存器
#define CFTW0_ADD 0x04   //CTW0 通道频率转换字寄存器
#define CPOW0_ADD 0x05   //CPW0 通道相位转换字寄存器
#define ACR_ADD   0x06   //ACR 幅度控制寄存器
#define LSRR_ADD  0x07   //LSR 通道线性扫描寄存器
#define RDW_ADD   0x08   //RDW 通道线性向上扫描寄存器
#define FDW_ADD   0x09   //FDW 通道线性向下扫描寄存器

void AD9959_delay(uint32_t length);

/**
 * @brief 复位AD9959芯片
 * @details 通过控制RST引脚对芯片进行硬件复位操作
 */
void AD9959_Reset(void);

/**
 * @brief 更新AD9959芯片数据
 * @details 触发IOUP引脚，使配置数据生效
 */
void AD9959_Update(void);


void WriteData_AD9959(uint8_t RegisterAddress, uint8_t NumberofRegisters, uint8_t *RegisterData,uint8_t temp);

/**
 * @brief 初始化AD9959芯片
 * @details 配置基本工作模式和参数，准备使用
 */
void AD9959_Init(void);

/**
 * @brief 设置指定通道的输出频率
 * @param Channel 通道号(0-3)
 * @param Freq 频率值(Hz)
 * @details 计算并设置对应通道的频率控制字
 */
void AD9959_Set_frequence(uint8_t Channel,uint32_t Freq);

/**
 * @brief 设置指定通道的输出幅度
 * @param Channel 通道号(0-3)
 * @param Ampli 幅度值(0-4095)
 * @details 设置对应通道的幅度控制寄存器
 */
void AD9959_Set_Amplitude(uint8_t Channel, uint16_t Ampli);

/**
 * @brief 设置指定通道的输出相位
 * @param Channel 通道号(0-3)
 * @param Phase 相位值(度)
 * @details 计算并设置对应通道的相位控制字
 */
void AD9959_Set_Phase(uint8_t Channel,double Phase);

/**
 * @brief 同时设置四个通道的输出频率
 * @param Freq0 通道0频率(Hz)
 * @param Freq1 通道1频率(Hz)
 * @param Freq2 通道2频率(Hz)
 * @param Freq3 通道3频率(Hz)
 * @details 依次设置四个通道的频率值
 */
void AD9959_SetFrequency4Channel(uint32_t Freq0,uint32_t Freq1,uint32_t Freq2,uint32_t Freq3);

/**
 * @brief 同时设置四个通道的输出幅度
 * @param Amp0 通道0幅度值(0-4095)
 * @param Amp1 通道1幅度值(0-4095)
 * @param Amp2 通道2幅度值(0-4095)
 * @param Amp3 通道3幅度值(0-4095)
 * @details 依次设置四个通道的幅度值
 */
void AD9959_SetAmp4Channel(uint32_t Amp0,uint32_t Amp1,uint32_t Amp2,uint32_t Amp3);

/**
 * @brief 同时设置四个通道的输出相位
 * @param Phase0 通道0相位(度)
 * @param Phase1 通道1相位(度)
 * @param Phase2 通道2相位(度)
 * @param Phase3 通道3相位(度)
 * @details 依次设置四个通道的相位值
 */
void AD9959_SetPhase4Channel(double Phase0,double Phase1,double Phase2,double Phase3);

/**
 * @brief 设置所有通道为相同频率
 * @param Freq 频率值(Hz)
 * @details 循环设置四个通道为同一频率
 */
void AD9959_All_Channel_Set_Same_Freq(uint32_t Freq);

/**
 * @brief 配置AD9959为扫描模式
 * @param starthz 起始频率(Hz)
 * @param stophz 终止频率(Hz)
 * @param stephz 频率步进(Hz)
 * @details 配置线性频率扫描模式参数
 */
void AD9959_Sweep_Mode(uint32_t starthz, uint32_t stophz, uint32_t stephz);



#endif /* AD9959_H_ */
