/*
 * ad9959.c
 *
 *  Created on: 2025年5月27日
 *      Author: zhangchenglin
 */

#include "ad9959.h"

// 寄存器配置数据
uint8_t CSR_DATA0[1] = {0x10};      // 开 CH0
uint8_t CSR_DATA1[1] = {0x20};      // 开 CH1
uint8_t CSR_DATA2[1] = {0x40};      // 开 CH2
uint8_t CSR_DATA3[1] = {0x80};      // 开 CH3

uint8_t FR1_DATA[3] = {0xD0, 0x00, 0x00};//20倍频
uint8_t FR2_DATA[2] = {0x00, 0x00};//default Value = 0x0000
uint8_t CFR_DATA[3] = {0x00, 0x03, 0x02};//default Value = 0x000302
uint8_t CPOW0_DATA[2] = {0x00, 0x00};//default Value = 0x0000   @ = POW/2^14*360
uint8_t LSRR_DATA[2] = {0x00, 0x00};
uint8_t RDW_DATA[4] = {0x00, 0x00, 0x00, 0x00};
uint8_t FDW_DATA[4] = {0x00, 0x00, 0x00, 0x00};
uint8_t ACRdata[3] = {0x00, 0x10, 0x00};//default Value = 0x--0000 Rest = 18.91/Iout


void AD9959_delay(uint32_t length)
{
    delay_cycles(length*10);
}
// SPI数据写入
void WriteData_AD9959(uint8_t RegisterAddress, uint8_t NumberofRegisters, uint8_t *RegisterData, uint8_t temp)
{
    uint8_t ControlValue = 0;
    uint8_t ValueToWrite = 0;
    uint8_t RegisterIndex = 0;
    uint8_t i = 0;

    ControlValue = RegisterAddress;
    // 写入地址
    AD9959_SCLK_Clr();
    AD9959_CSB_Clr();
    for (i = 0; i < 8; i++)
    {
        AD9959_SCLK_Clr();
        if (0x80 == (ControlValue & 0x80))
            AD9959_SDIO_Set();
        else
            AD9959_SDIO_Clr();
        AD9959_SCLK_Set();
        ControlValue <<= 1;
    }
    AD9959_SCLK_Clr();

    // 写入数据
    for (RegisterIndex = 0; RegisterIndex < NumberofRegisters; RegisterIndex++)
    {
        ValueToWrite = RegisterData[RegisterIndex];
        for (i = 0; i < 8; i++)
        {
            AD9959_SCLK_Clr();
            if (0x80 == (ValueToWrite & 0x80))
                AD9959_SDIO_Set();
            else
                AD9959_SDIO_Clr();
            AD9959_SCLK_Set();
            ValueToWrite <<= 1;
        }
        AD9959_SCLK_Clr();
    }

    if (temp == 1)
        AD9959_Update();
    AD9959_CSB_Set();
}

void AD9959_Init(void)
{

    DL_GPIO_initDigitalOutput(IOMUX_PINCM4);
    DL_GPIO_initDigitalOutput(IOMUX_PINCM23);
    DL_GPIO_initDigitalOutput(IOMUX_PINCM51);
    DL_GPIO_initDigitalOutput(IOMUX_PINCM58);
    DL_GPIO_initDigitalOutput(IOMUX_PINCM24);
    DL_GPIO_clearPins(GPIOB, DL_GPIO_PIN_23 |
            DL_GPIO_PIN_6 |
            DL_GPIO_PIN_27 |
            DL_GPIO_PIN_7);
    DL_GPIO_clearPins(GPIOA, DL_GPIO_PIN_29);
    DL_GPIO_enableOutput(GPIOB, DL_GPIO_PIN_23 |
            DL_GPIO_PIN_6 |
            DL_GPIO_PIN_27 |
            DL_GPIO_PIN_7);
    DL_GPIO_enableOutput(GPIOA, DL_GPIO_PIN_29);

    // 初始配置
    AD9959_CSB_Set();  // CSB置高
    AD9959_SCLK_Clr(); // SCLK置低
    AD9959_IOUP_Clr(); // IOUP置低
    AD9959_SDIO_Clr(); // SDIO置低
    AD9959_Reset();
    WriteData_AD9959(FR1_ADD, 3, FR1_DATA, 1);//写功能寄存器1
}

void AD9959_Reset(void)
{
    AD9959_RST_Clr();  // RST置低
    AD9959_delay(1);
    AD9959_RST_Set();  // RST置高
    AD9959_delay(30);
    AD9959_RST_Clr();  // RST置低
}

void AD9959_Update(void)
{
    AD9959_IOUP_Clr();  // IOUP置低
    AD9959_delay(2);
    AD9959_IOUP_Set();  // IOUP置高
    AD9959_delay(4);
    AD9959_IOUP_Clr();  // IOUP置低
}

void AD9959_Set_frequence(uint8_t Channel, uint32_t Freq)
{
    uint8_t CFTW0_DATA[4] = {0x00, 0x00, 0x00, 0x00};  // 中间变量
    uint32_t Temp;
    //将输入频率因子分为四个字节  8.589934592=(2^32)/500000000
    Temp = (uint32_t)Freq * 8.589934592;

    CFTW0_DATA[3] = (uint8_t)Temp;
    CFTW0_DATA[2] = (uint8_t)(Temp >> 8);
    CFTW0_DATA[1] = (uint8_t)(Temp >> 16);
    CFTW0_DATA[0] = (uint8_t)(Temp >> 24);

    if (Channel == 0)
    {
        WriteData_AD9959(CSR_ADD, 1, CSR_DATA0, 0); // 控制寄存器写入CH0通道
        WriteData_AD9959(CFTW0_ADD, 4, CFTW0_DATA, 0); // 输出CH0设定频率
    }
    else if (Channel == 1)
    {
        WriteData_AD9959(CSR_ADD, 1, CSR_DATA1, 0); // 控制寄存器写入CH1通道
        WriteData_AD9959(CFTW0_ADD, 4, CFTW0_DATA, 0); // 输出CH1设定频率
    }
    else if (Channel == 2)
    {
        WriteData_AD9959(CSR_ADD, 1, CSR_DATA2, 0); // 控制寄存器写入CH2通道
        WriteData_AD9959(CFTW0_ADD, 4, CFTW0_DATA, 0); // 输出CH2设定频率
    }
    else if (Channel == 3)
    {
        WriteData_AD9959(CSR_ADD, 1, CSR_DATA3, 0); // 控制寄存器写入CH3通道
        WriteData_AD9959(CFTW0_ADD, 4, CFTW0_DATA, 0); // 输出CH3设定频率
    }
}

void AD9959_Set_Amplitude(uint8_t Channel, uint16_t Ampli)
{
    uint16_t A_temp;
    uint8_t ACR_DATA[3] = {0x00, 0x00, 0x00};

    A_temp = Ampli | 0x1000;
    ACR_DATA[2] = (uint8_t)A_temp;  // 低位数据
    ACR_DATA[1] = (uint8_t)(A_temp >> 8); // 高位数据

    if (Channel == 0)
    {
        WriteData_AD9959(CSR_ADD, 1, CSR_DATA0, 0);
        WriteData_AD9959(ACR_ADD, 3, ACR_DATA, 0);
    }
    else if (Channel == 1)
    {
        WriteData_AD9959(CSR_ADD, 1, CSR_DATA1, 0);
        WriteData_AD9959(ACR_ADD, 3, ACR_DATA, 0);
    }
    else if (Channel == 2)
    {
        WriteData_AD9959(CSR_ADD, 1, CSR_DATA2, 0);
        WriteData_AD9959(ACR_ADD, 3, ACR_DATA, 0);
    }
    else if (Channel == 3)
    {
        WriteData_AD9959(CSR_ADD, 1, CSR_DATA3, 0);
        WriteData_AD9959(ACR_ADD, 3, ACR_DATA, 0);
    }
}

void AD9959_Set_Phase(uint8_t Channel, double Phase)
{
    uint16_t P_temp = 0;
    double rPhase = 0;

    // 输入参数范围检查
    if (Phase < 0.0)
    {
        Phase = 0;
    }
    if (Phase > 360.0)
    {
        Phase = 360.0;
    }

    rPhase = 360 - Phase; // 实际是超前，反向处理
    P_temp = (uint16_t)(rPhase / 360.0 * 16384.0 + 0.5); // 四舍五入转整数
    CPOW0_DATA[1] = (uint8_t)P_temp;
    CPOW0_DATA[0] = (uint8_t)(P_temp >> 8);

    if (Channel == 0)
    {
        WriteData_AD9959(CSR_ADD, 1, CSR_DATA0, 0);
        WriteData_AD9959(CPOW0_ADD, 2, CPOW0_DATA, 0);
    }
    else if (Channel == 1)
    {
        WriteData_AD9959(CSR_ADD, 1, CSR_DATA1, 0);
        WriteData_AD9959(CPOW0_ADD, 2, CPOW0_DATA, 0);
    }
    else if (Channel == 2)
    {
        WriteData_AD9959(CSR_ADD, 1, CSR_DATA2, 0);
        WriteData_AD9959(CPOW0_ADD, 2, CPOW0_DATA, 0);
    }
    else if (Channel == 3)
    {
        WriteData_AD9959(CSR_ADD, 1, CSR_DATA3, 0);
        WriteData_AD9959(CPOW0_ADD, 2, CPOW0_DATA, 0);
    }
}

void AD9959_SetFrequency4Channel(uint32_t Freq0, uint32_t Freq1, uint32_t Freq2, uint32_t Freq3)
{
    AD9959_Set_frequence(0, Freq0);
    AD9959_Set_frequence(1, Freq1);
    AD9959_Set_frequence(2, Freq2);
    AD9959_Set_frequence(3, Freq3);
}

void AD9959_SetAmp4Channel(uint32_t Amp0, uint32_t Amp1, uint32_t Amp2, uint32_t Amp3)
{
    AD9959_Set_Amplitude(0, Amp0);
    AD9959_Set_Amplitude(1, Amp1);
    AD9959_Set_Amplitude(2, Amp2);
    AD9959_Set_Amplitude(3, Amp3);
}

void AD9959_SetPhase4Channel(double Phase0, double Phase1, double Phase2, double Phase3)
{
    AD9959_Set_Phase(0, Phase0);
    AD9959_Set_Phase(1, Phase1);
    AD9959_Set_Phase(2, Phase2);
    AD9959_Set_Phase(3, Phase3);
}

/* 四通道同频设置 */
void AD9959_All_Channel_Set_Same_Freq(uint32_t Freq)
{
    uint8_t CFTW0_DATA[4] = {0x00, 0x00, 0x00, 0x00};  // 中间变量
    uint32_t Temp;
    Temp = (uint32_t)Freq * 8.589934592;  // 频率计算

    CFTW0_DATA[3] = (uint8_t)Temp;
    CFTW0_DATA[2] = (uint8_t)(Temp >> 8);
    CFTW0_DATA[1] = (uint8_t)(Temp >> 16);
    CFTW0_DATA[0] = (uint8_t)(Temp >> 24);

    // 统一写入四通道
    uint8_t channelCsr[4] = {0x10, 0x20, 0x40, 0x80};
    for (uint8_t ch = 0; ch < 4; ch++) {
        WriteData_AD9959(CSR_ADD, 1, &channelCsr[ch], 0);
        WriteData_AD9959(CFTW0_ADD, 4, CFTW0_DATA, 0);
    }
}

void AD9959_Sweep_Mode(uint32_t starthz, uint32_t stophz, uint32_t stephz)
{
    uint32_t i = 0;
    uint32_t stepCount = 0;

    // 计算步进次数
    if (starthz < stophz) { // 从低到高
        stepCount = (stophz - starthz) / stephz;
        for (i = 0; i <= stepCount; i++) {
            AD9959_All_Channel_Set_Same_Freq(starthz + i * stephz);
            AD9959_delay(1000); // 添加延时确保频率稳定
        }
    } else { // 从高到低
        stepCount = (starthz - stophz) / stephz;
        for (i = 0; i <= stepCount; i++) {
            AD9959_All_Channel_Set_Same_Freq(starthz - i * stephz);
            AD9959_delay(1000); // 添加延时确保频率稳定
        }
    }
}
