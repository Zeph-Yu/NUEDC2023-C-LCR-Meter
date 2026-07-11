/*
 * oled.h
 *
 *  Created on: 2024年4月29日
 *      Author: zhangchenglin
 *
 * 中景园1.3寸OLED屏幕，带字库芯片，主控SSH1106
 * 按照字符方向，插入开发板OLED屏幕接口，不能反接
 */

#ifndef OLED_H_
#define OLED_H_

/* DriverLib Includes */
#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>
#include "ti_msp_dl_config.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

void delay(uint32_t);

void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr);

void OLED_ShowString(uint8_t x,uint8_t y, uint8_t *p);


// SPI_CLK (SCK)
#define OLED_SCL_Clr() DL_GPIO_clearPins(GPIOB, DL_GPIO_PIN_9)
#define OLED_SCL_Set() DL_GPIO_setPins(GPIOB, DL_GPIO_PIN_9)
// SPI_MOSI (SDA)
#define OLED_SDA_Clr() DL_GPIO_clearPins(GPIOB, DL_GPIO_PIN_8)
#define OLED_SDA_Set() DL_GPIO_setPins(GPIOB, DL_GPIO_PIN_8)
// DC (RS)
#define OLED_DC_Clr()  DL_GPIO_clearPins(GPIOB, DL_GPIO_PIN_10)
#define OLED_DC_Set()  DL_GPIO_setPins(GPIOB, DL_GPIO_PIN_10)
// CS1
#define OLED_CS_Clr()  DL_GPIO_clearPins(GPIOB, DL_GPIO_PIN_11)
#define OLED_CS_Set()  DL_GPIO_setPins(GPIOB, DL_GPIO_PIN_11)
// SPI_MISO (FS0)
#define OLED_READ_FS0() DL_GPIO_readPins(GPIOB, DL_GPIO_PIN_14)
// CS2
#define OLED_ROM_CS_Clr() DL_GPIO_clearPins(GPIOB, DL_GPIO_PIN_26)
#define OLED_ROM_CS_Set() DL_GPIO_setPins(GPIOB, DL_GPIO_PIN_26)

#define OLED_CMD  0 //写命令
#define OLED_DATA 1 //写数据

/*
 * 反转屏幕色彩
 * i : 0,正常显示; 1,反色显示
 */
void OLED_ColorTurn(uint8_t i);

/*
 * 旋转显示方向180°
 * i : 0,正常显示; 1,旋转显示
 */
void OLED_DisplayTurn(uint8_t i);

/*
 * 开启OLED显示
 */
void OLED_DisPlay_On(void);

/*
 * 关闭OLED显示
 */
void OLED_DisPlay_Off(void);

/*
 * 向SH1106写入字节
 * dat : 数据
 * cmd : OLED_CMD,写命令; OLED_DATA,写数据
 */
void OLED_WR_Byte(uint8_t dat,uint8_t cmd);

/*
 * 清屏
 */
void OLED_Clear(void);

/*
 * 设置起始地址
 * x : 坐标
 * y : 坐标
 */
void OLED_address(uint8_t x,uint8_t y);

/*
 * 全屏显示128x64点阵图像
 * dp : 数据指针
 */
void OLED_Display_128x64(uint8_t *dp);

/*
 * 显示16x16点阵图像、汉字、生僻字或16x16点阵的其他图标
 * x : 坐标
 * y : 坐标
 * dp :数据指针
 */
void OLED_Display_16x16(uint8_t x,uint8_t y,uint8_t *dp);

/*
 * 显示8x16点阵图像、ASCII, 或8x16点阵的自造字符、其他图标
 * x : 坐标
 * y : 坐标
 * dp :数据指针
 */
void OLED_Display_8x16(uint8_t x,uint8_t y,uint8_t *dp);

/*
 * 显示5*7点阵图像、ASCII, 或5x7点阵的自造字符、其他图标
 * x : 坐标
 * y : 坐标
 * dp : 数据指针
 */
void OLED_Display_5x7(uint8_t x,uint8_t y,uint8_t *dp);

/*
 * 向字库IC写入指令
 */
void Send_Command_to_ROM(uint8_t dat);

/*
 * 从字库IC中取1个字节汉字或字符数据
 *
 * 返回值：
 */
uint8_t Get_data_from_ROM(void);

/*
 * 从地址中连续读出数据存入对应地址
 * addrHigh ：读地址高字节
 * addrMid ： 读地址中字节
 * addrLow ： 读地址低字节
 * DataLen : 读出数据长度（字节）
 * pbuff : 存地址
 */
void OLED_get_data_from_ROM(uint8_t addrHigh,uint8_t addrMid,uint8_t addrLow,uint8_t *pbuff,uint8_t DataLen);

/*
 * 显示GB2312汉字
 * x : 坐标
 * y : 坐标
 * text : 字符串
 */
void OLED_Display_GB2312_string(uint8_t x,uint8_t y,const char *text);

/*
 * 显示GB2312汉字，5x7大小
 * x : 坐标
 * y : 坐标
 * text : 字符串
 */
void OLED_Display_string_5x7(uint8_t x,uint8_t y,const char *text);

/*
 * 显示数字
 * x : 坐标
 * y : 坐标
 * num : 数字
 * len : 显示位数
 */
void OLED_ShowNum(uint8_t x,uint8_t y,float num,uint8_t len);

void OLED_DrawPoint(uint8_t x,uint8_t y);

/*
 * 初始化OLED
 */
void OLED_Init(void);

#endif /* OLED_H_ */
