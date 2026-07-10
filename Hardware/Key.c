#include "Key.h"

Key_t Keys[KEY_NUM_KEYS] = 
{
    {GPIOA, KEY_PA_18_PIN, KEY1_TRIGGER_LEVEL},
    {GPIOB, KEY_PB_21_PIN, KEY2_TRIGGER_LEVEL}   
};

static uint8_t ReadKeyState(uint8_t keyIndex)
{
    return (DL_GPIO_readPins(Keys[keyIndex].Port, Keys[keyIndex].Pins) 
    == Keys[keyIndex].TriggerLevel) ? KEY_PRESSED : KEY_UNPRESSED;
}

void Key_Tick(void)
{
    static uint8_t Count;
    static uint8_t CurrRaw[KEY_NUM_KEYS], PrevRaw[KEY_NUM_KEYS];
    
    Count++;
    if(Count >= KEY_CYCLE_TIME)
    {
        Count = 0;
        for (uint8_t i = 0; i < KEY_NUM_KEYS; i++)
        {
            PrevRaw[i] = CurrRaw[i];
            CurrRaw[i] = ReadKeyState(i);
            if(PrevRaw[i] == KEY_UNPRESSED && CurrRaw[i] == KEY_PRESSED)  //按键按下
            {
                // 按键按下事件处理
                if (i == 0) // PA18
                {
                    state = STATE_GENEXCITATION;
                }
                else if (i == 1) // PB21
                {
                    state = STATE_INIT_SAMPLING;
                }
            }
        }
    }
}

