/*
 * oled.c
 *
 *  Created on: 2024年4月29日
 *      Author: zhangchenglin
 */

/* DriverLib Includes */
#include "ti_msp_dl_config.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <math.h>

#include "oled.h"
#include "oledfont.h"
void delay(uint32_t t)
{
    delay_cycles(t);
}

#define SIZE 16
#define XLevelL     0x02
#define XLevelH     0x10
#define Max_Column  128
#define Max_Row     64
#define Brightness  0xFF
#define X_WIDTH     128
#define Y_WIDTH     64

unsigned char oled_RAM[128][8];

void OLED_Set_Pos(unsigned char x, unsigned char y)
{
    OLED_WR_Byte(0xb0+y,OLED_CMD);
    OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
    OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD);
}

void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr)
{
    unsigned char c=0,i=0;
        c=chr-' ';//得到偏移后的值
        if(x>Max_Column-1){x=0;y=y+2;}
        if(SIZE ==16)
            {
            OLED_Set_Pos(x,y);
            for(i=0;i<8;i++)
            OLED_WR_Byte(F8X16[c*16+i],OLED_DATA);
            OLED_Set_Pos(x,y+1);
            for(i=0;i<8;i++)
            OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);
            }
            else {
                OLED_Set_Pos(x,y+1);
                for(i=0;i<6;i++)
                OLED_WR_Byte(F6x8[c][i],OLED_DATA);

            }
}

void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr)
{
    unsigned char j;
    j = 0;
    while (chr[j]!='\0')
    {       OLED_ShowChar(x,y,chr[j]);
            x+=8;
        if(x>120){x=0;y+=2;}
            j++;
    }
}
/*
 * 初始化OLED
 */
void OLED_Init()
{
    DL_GPIO_initDigitalOutput(IOMUX_PINCM26);
    DL_GPIO_initDigitalOutput(IOMUX_PINCM25);
    DL_GPIO_initDigitalOutput(IOMUX_PINCM27);
    DL_GPIO_initDigitalOutput(IOMUX_PINCM28);
    DL_GPIO_initDigitalInputFeatures(IOMUX_PINCM31,
         DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
         DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_initDigitalOutput(IOMUX_PINCM57);
    DL_GPIO_setPins(GPIOB, DL_GPIO_PIN_9 |
            DL_GPIO_PIN_8 |
            DL_GPIO_PIN_10 |
            DL_GPIO_PIN_11 |
            DL_GPIO_PIN_26);
    DL_GPIO_enableOutput(GPIOB, DL_GPIO_PIN_9 |
                DL_GPIO_PIN_8 |
                DL_GPIO_PIN_10 |
                DL_GPIO_PIN_11 |
                DL_GPIO_PIN_26);
    OLED_WR_Byte(0xAE,OLED_CMD); /*display off*/
    OLED_WR_Byte(0x00,OLED_CMD); /*set lower column address*/
    /* SSD1306可以驱动128*64个像素点，
     * SH1106可以驱动132*64个像素点
     * 所用屏幕为128*64分辨率
     * 因此SH1106的
     * (2,0)对应实际可显示区域的(0,0)
     * (129,63)对应实际可显示区域的(128,63)
     * 需要设置低位地址为0x02*/
    OLED_WR_Byte(0x10,OLED_CMD); /*set higher column address*/
    OLED_WR_Byte(0x40,OLED_CMD); /*set display start line*/
    OLED_WR_Byte(0xB0,OLED_CMD); /*set page address*/
    OLED_WR_Byte(0x81,OLED_CMD); /*contract control*/
    OLED_WR_Byte(0xcf,OLED_CMD); /*128*/
    OLED_WR_Byte(0xA1,OLED_CMD); /*set segment remap*/
    OLED_WR_Byte(0xA6,OLED_CMD); /*normal / reverse*/
    OLED_WR_Byte(0xA8,OLED_CMD); /*multiplex ratio*/
    OLED_WR_Byte(0x3F,OLED_CMD); /*duty = 1/64*/
    OLED_WR_Byte(0xad,OLED_CMD); /*set charge pump enable*/
    OLED_WR_Byte(0x8b,OLED_CMD); /* 0x8B 内供 VCC */
    OLED_WR_Byte(0x33,OLED_CMD); /*0X30---0X33 set VPP 9V */
    OLED_WR_Byte(0xC8,OLED_CMD); /*Com scan direction*/
    OLED_WR_Byte(0xD3,OLED_CMD); /*set display offset*/
    OLED_WR_Byte(0x00,OLED_CMD); /* 0x20 */
    OLED_WR_Byte(0xD5,OLED_CMD); /*set osc division*/
    OLED_WR_Byte(0x80,OLED_CMD);
    OLED_WR_Byte(0xD9,OLED_CMD); /*set pre-charge period*/
    OLED_WR_Byte(0x1f,OLED_CMD); /*0x22*/
    OLED_WR_Byte(0xDA,OLED_CMD); /*set COM pins*/
    OLED_WR_Byte(0x12,OLED_CMD);
    OLED_WR_Byte(0xdb,OLED_CMD); /*set vcomh*/
    OLED_WR_Byte(0x40,OLED_CMD);
    OLED_Clear();
    OLED_WR_Byte(0xAF,OLED_CMD); /*display ON*/
    delay(240000);
    OLED_DisplayTurn(0);
}

/*
 * 反转屏幕色彩
 * i : 0,正常显示; 1,反色显示
 */
void OLED_ColorTurn(uint8_t i)
{
    if(i==0)
    {
        OLED_WR_Byte(0xA6,OLED_CMD);
    }
    if(i==1)
    {
        OLED_WR_Byte(0xA7,OLED_CMD);
    }
}

/*
 * 旋转显示方向180°
 * i : 0,正常显示; 1,旋转显示
 */
void OLED_DisplayTurn(uint8_t i)
{
    if(i==0)
    {
        OLED_WR_Byte(0xC8,OLED_CMD);
        OLED_WR_Byte(0xA1,OLED_CMD);
    }
    if(i==1)
    {
        OLED_WR_Byte(0xC0,OLED_CMD);
        OLED_WR_Byte(0xA0,OLED_CMD);
    }
}

/*
 * 开启OLED显示
 */
void OLED_DisPlay_On(void)
{
    OLED_WR_Byte(0x8D,OLED_CMD);//电荷泵使能
    OLED_WR_Byte(0x14,OLED_CMD);//开启电荷泵
    OLED_WR_Byte(0xAF,OLED_CMD);//点亮屏幕
}

/*
 * 关闭OLED显示
 */
void OLED_DisPlay_Off(void)
{
    OLED_WR_Byte(0x8D,OLED_CMD);//电荷泵使能
    OLED_WR_Byte(0x10,OLED_CMD);//关闭电荷泵
    OLED_WR_Byte(0xAE,OLED_CMD);//关闭屏幕
}


/*
 * 向SH1106写入字节
 * dat : 数据
 * cmd : OLED_CMD,写命令; OLED_DATA,写数据
 */
void OLED_WR_Byte(uint8_t dat,uint8_t cmd)
{
    uint8_t i;
    if(cmd)
      OLED_DC_Set();
    else
      OLED_DC_Clr();
//    delay(10);
    OLED_CS_Clr();
    delay(10);
    for(i=0;i<8;i++)
    {
        OLED_SCL_Clr();
        delay(10);
        if(dat&0x80)
           OLED_SDA_Set();
        else
           OLED_SDA_Clr();
        delay(10);
        OLED_SCL_Set();
        delay(10);
        dat<<=1;
    }
    OLED_CS_Set();
    delay(10);
    OLED_DC_Set();
//    delay(10);
}

/*
 * 清屏
 */
void OLED_Clear(void)
{
    uint8_t i,n;
    for(i=0;i<8;i++)
    {
         OLED_WR_Byte(0xb0+i,OLED_CMD);//设置页地址
       OLED_WR_Byte(0x10,OLED_CMD);  //设置列地址的高4位
       OLED_WR_Byte(0x02,OLED_CMD);  //设置列地址的低4位
       for(n=0;n<128;n++)
         {
             OLED_WR_Byte(0x00,OLED_DATA);//清除所有数据
         }
  }
}

/*
 * 设置起始地址
 */
void OLED_address(uint8_t x,uint8_t y)
{
    x+=2;
    OLED_WR_Byte(0xb0+y,OLED_CMD);              //设置页地址
    OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);  //设置列地址的高4位
    OLED_WR_Byte((x&0x0f),OLED_CMD);            //设置列地址的低4位
}

/*
 * 全屏显示128x64点阵图像
 */
void OLED_Display_128x64(uint8_t *dp)
{
    uint8_t i,j;
    for(i=0;i<8;i++)
    {
        OLED_address(0,i);
        for(j=0;j<128;j++)
        {
            OLED_WR_Byte(*dp,OLED_DATA); //写数据到OLED,每写完一个8位的数据后列地址自动加1
            dp++;
    }
  }
}

/*
 * 显示16x16点阵图像、汉字、生僻字或16x16点阵的其他图标
 * x : 坐标
 * y : 坐标
 * dp :数据
 */
void OLED_Display_16x16(uint8_t x,uint8_t y,uint8_t *dp)
{
    uint8_t i,j;
    for(j=0;j<2;j++)
    {
        OLED_address(x,y);
        for(i=0;i<16;i++)
        {
            OLED_WR_Byte(*dp,OLED_DATA);  //写数据到OLED,每写完一个8位的数据后列地址自动加1
            dp++;
    }
        y++;
  }
}

/*
 * 显示8x16点阵图像、ASCII, 或8x16点阵的自造字符、其他图标
 * x : 坐标
 * y : 坐标
 * dp :数据
 */
void OLED_Display_8x16(uint8_t x,uint8_t y,uint8_t *dp)
{
    uint8_t i,j;
    for(j=0;j<2;j++)
    {
        OLED_address(x,y);
        for(i=0;i<8;i++)
        {
            OLED_WR_Byte(*dp,OLED_DATA);   //写数据到LCD,每写完一个8位的数据后列地址自动加1
            dp++;
    }
        y++;
  }
}

/*
 * 显示5*7点阵图像、ASCII, 或5x7点阵的自造字符、其他图标
 * x : 坐标
 * y : 坐标
 * dp : 数据
 */
void OLED_Display_5x7(uint8_t x,uint8_t y,uint8_t *dp)
{
    uint8_t i;
  OLED_address(x,y);
    for(i=0;i<6;i++)
    {
        OLED_WR_Byte(*dp,OLED_DATA);
        dp++;
  }
}

/*
 * 向字库IC写入指令
 * dat : 指令
 */
void Send_Command_to_ROM(uint8_t dat)
{
    uint8_t i;
    for(i=0;i<8;i++)
    {
        OLED_SCL_Clr();
        delay(10);
        if(dat&0x80)
        {
            OLED_SDA_Set();
    }
        else
        {
            OLED_SDA_Clr();
    }
        delay(10);
        dat<<=1;
        OLED_SCL_Set();
        delay(10);
  }
}

/*
 * 从字库IC中取1个字节汉字或字符数据
 *
 * 返回值：
 */
uint8_t Get_data_from_ROM(void)
{
    uint8_t i,read=0;
    OLED_SCL_Set();
    delay(10);
    for(i=0;i<8;i++)
    {
        OLED_SCL_Clr();
        read<<=1;
        delay(10);
        if(OLED_READ_FS0())
        {
            read++;
//            read = read | 0x01;
        }
        else
        {
//            read &= 0xfe;
        }
        OLED_SCL_Set();
        delay(10);
  }
    return read;
}


/*
 * 从地址中连续读出数据存入对应地址
 * addrHigh ：读地址高字节
 * addrMid ： 读地址中字节
 * addrLow ： 读地址低字节
 * DataLen : 读出数据长度（字节）
 * pbuff : 存地址
 */
void OLED_get_data_from_ROM(uint8_t addrHigh,uint8_t addrMid,uint8_t addrLow,uint8_t *pbuff,uint8_t DataLen)
{
    uint8_t i;
    OLED_ROM_CS_Clr();
    delay(20);
    Send_Command_to_ROM(0x03);
    Send_Command_to_ROM(addrHigh);
    Send_Command_to_ROM(addrMid);
    Send_Command_to_ROM(addrLow);
    for(i=0;i<DataLen;i++)
    {
//        *(pbuff+i)=Get_data_from_ROM();
        pbuff[i] = Get_data_from_ROM();
    }
    OLED_ROM_CS_Set();
    delay(20);
}


uint32_t fontaddr=0;

/*
 * 显示GB2312汉字
 * x : 坐标
 * y : 坐标
 * text : 字符串
 */
void OLED_Display_GB2312_string(uint8_t x,uint8_t y,const char *text)
{
    uint8_t i=0;
    uint8_t addrHigh,addrMid,addrLow;
    volatile uint8_t fontbuf[32];
//    uint8_t temp;
//    uint32_t add_temp;
    while(text[i]>0x00)
    {
        if((text[i]>=0xb0)&&(text[i]<=0xf7)&&(text[i+1]>=0xa1))
        {
            //国标简体（GB2312）汉字在晶联讯字库IC中的地址由以下公式来计算：
            //Address = ((MSB - 0xB0) * 94 + (LSB - 0xA1)+ 846)*32+ BaseAdd;BaseAdd=0
            //由于担心8位单片机有乘法溢出问题，所以分三部取地址
//            temp = text[i];
            fontaddr=(text[i]-0xB0);
//            add_temp = fontaddr;
            fontaddr=fontaddr*94;
//            add_temp = fontaddr;
//            temp = text[i+1];
            fontaddr+=(text[i+1]-0xA1)+846;
//            add_temp = fontaddr;
            fontaddr=fontaddr*32;
//            add_temp = fontaddr;

            addrHigh=(fontaddr&0xff0000)>>16;   //地址的高8位,共24位
            addrMid=(fontaddr&0xff00)>>8;       //地址的中8位,共24位
            addrLow=(fontaddr&0xff);            //地址的低8位,共24位

            OLED_get_data_from_ROM(addrHigh,addrMid,addrLow,(uint8_t *)fontbuf,32);
            //取32个字节的数据，存到"fontbuf[32]"
            OLED_Display_16x16(x,y,(uint8_t *)fontbuf);
            //显示汉字到LCD上，y为页地址，x为列地址，fontbuf[]为数据
            x+=16;
            i+=2;
    }
        else if((text[i]>=0xa1)&&(text[i]<=0xa3)&&(text[i+1]>=0xa1))
        {

            fontaddr=(text[i]-0xa1)*94;
            fontaddr+=(text[i+1]-0xa1);
            fontaddr=fontaddr*32;

            addrHigh=(fontaddr&0xff0000)>>16;
            addrMid=(fontaddr&0xff00)>>8;
            addrLow=(fontaddr&0xff);

            OLED_get_data_from_ROM(addrHigh,addrMid,addrLow,(uint8_t *)fontbuf,32);
            OLED_Display_16x16(x,y,(uint8_t *)fontbuf);
            x+=16;
            i+=2;
    }
        else if((text[i]>=0x20)&&(text[i]<=0x7e))
        {
            unsigned char fontbuf[16];
            fontaddr=(text[i]-0x20);
            fontaddr=(unsigned long)(fontaddr*16);
            fontaddr=(unsigned long)(fontaddr+0x3cf80);

            addrHigh=(fontaddr&0xff0000)>>16;
            addrMid=(fontaddr&0xff00)>>8;
            addrLow=fontaddr&0xff;

            OLED_get_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,16);
            OLED_Display_8x16(x,y,fontbuf);
            x+=8;
            i+=1;
    }
        else
            i++;
  }
}

/*
 * 显示GB2312汉字，5x7大小
 * x : 坐标
 * y : 坐标
 * text : 字符串
 */
void OLED_Display_string_5x7(uint8_t x,uint8_t y,const char *text)
{
    uint8_t i=0;
    uint8_t addrHigh,addrMid,addrLow;
    while(text[i]>0x00)
    {
        if((text[i]>=0x20)&&(text[i]<=0x7e))
        {
            uint8_t fontbuf[8];
            fontaddr=(text[i]-0x20);
            fontaddr=(unsigned long)(fontaddr*8);
            fontaddr=(unsigned long)(fontaddr+0x3bfc0);

            addrHigh=(fontaddr&0xff0000)>>16;
            addrMid=(fontaddr&0xff00)>>8;
            addrLow=fontaddr&0xff;

            OLED_get_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,8);
            OLED_Display_5x7(x,y,fontbuf);
            x+=6;
            i+=1;
    }
        else
            i++;
  }
}

void OLED_DrawPoint(uint8_t x,uint8_t y)
{
    static unsigned char data1;
    data1=(unsigned char)(0x01<<((y%8)));
    OLED_Set_Pos(x, (unsigned char)(y >> 3));
    OLED_WR_Byte((unsigned char)(0xb0 + (y >> 3)),0);
    OLED_WR_Byte((unsigned char)(((x & 0xf0) >> 4) | 0x10),0);
    OLED_WR_Byte((unsigned char)((x & 0x0f) | 0x00),0);
    OLED_WR_Byte(data1,1);
}


/*
 * 显示数字
 * x : 坐标
 * y : 坐标
 * num : 数字
 * len : 显示位数
 */
uint32_t oled_pow(uint8_t m,uint8_t n)
{
    uint32_t result=1;
    while(n--)result*=m;
    return result;
}
void OLED_ShowNum(uint8_t x,uint8_t y,float num,uint8_t len)
{
    uint8_t i;
    uint32_t t,num1;
    x=x+len*8+8;//要显示的小数最低位的横坐标
    num1=num*100;//将小数左移两位并转化为整数
    OLED_Display_GB2312_string(x-24,y,".");//显示小数点
    for(i=0;i<len;i++)
    {
        t=num1%10;//取个位数的数值
        num1=num1/10;//将整数右移一位
        x-=8;
        if(i==2){x-=8;}//当显示出来两个小数之后，空出小数点的位置
        switch(t)
        {
            case 0 :OLED_Display_GB2312_string(x,y,"0");break;
            case 1 :OLED_Display_GB2312_string(x,y,"1");break;
            case 2 :OLED_Display_GB2312_string(x,y,"2");break;
            case 3 :OLED_Display_GB2312_string(x,y,"3");break;
            case 4 :OLED_Display_GB2312_string(x,y,"4");break;
            case 5 :OLED_Display_GB2312_string(x,y,"5");break;
            case 6 :OLED_Display_GB2312_string(x,y,"6");break;
            case 7 :OLED_Display_GB2312_string(x,y,"7");break;
            case 8 :OLED_Display_GB2312_string(x,y,"8");break;
            case 9 :OLED_Display_GB2312_string(x,y,"9");break;
        }
    }
//    uint8_t t,temp;
//    uint8_t size2 = 16;
//    uint8_t enshow=0;
//    for(t=0;t<len;t++)
//    {
////        temp=(num/oled_pow(10,len-t-1))%10;
//        temp = oled_pow(10,len-t-1);
//        temp = num/temp;
//        temp = temp%10;
//        if(enshow==0&&t<(len-1))
//        {
//            if(temp==0)
//            {
//                OLED_ShowChar(x+(size2/2)*t,y,' ');
//                continue;
//            }else enshow=1;
//
//        }
//        OLED_ShowChar(x+(size2/2)*t,y,temp+'0');
//    }
}

