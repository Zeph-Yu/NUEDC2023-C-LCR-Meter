/*
 * Copyright (c) 2023, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.c =============
 *  Configured MSPM0 DriverLib module definitions
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */

#include "ti_msp_dl_config.h"

DL_TimerA_backupConfig gADC_TIMBackup;

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform any initialization needed before using any board APIs
 */
SYSCONFIG_WEAK void SYSCFG_DL_init(void)
{
    SYSCFG_DL_initPower();
    SYSCFG_DL_GPIO_init();
    /* Module-Specific Initializations*/
    SYSCFG_DL_SYSCTL_init();
    SYSCFG_DL_ADC_TIM_init();
    SYSCFG_DL_UART_ADC_init();
    SYSCFG_DL_ADC_UR_init();
    SYSCFG_DL_ADC_UX_init();
    SYSCFG_DL_DMA_init();
    /* Ensure backup structures have no valid state */
	gADC_TIMBackup.backupRdy 	= false;


}
/*
 * User should take care to save and restore register configuration in application.
 * See Retention Configuration section for more details.
 */
SYSCONFIG_WEAK bool SYSCFG_DL_saveConfiguration(void)
{
    bool retStatus = true;

	retStatus &= DL_TimerA_saveConfiguration(ADC_TIM_INST, &gADC_TIMBackup);

    return retStatus;
}


SYSCONFIG_WEAK bool SYSCFG_DL_restoreConfiguration(void)
{
    bool retStatus = true;

	retStatus &= DL_TimerA_restoreConfiguration(ADC_TIM_INST, &gADC_TIMBackup, false);

    return retStatus;
}

SYSCONFIG_WEAK void SYSCFG_DL_initPower(void)
{
    DL_GPIO_reset(GPIOA);
    DL_GPIO_reset(GPIOB);
    DL_TimerA_reset(ADC_TIM_INST);
    DL_UART_Main_reset(UART_ADC_INST);
    DL_ADC12_reset(ADC_UR_INST);
    DL_ADC12_reset(ADC_UX_INST);

    DL_MathACL_reset(MATHACL);

    DL_GPIO_enablePower(GPIOA);
    DL_GPIO_enablePower(GPIOB);
    DL_TimerA_enablePower(ADC_TIM_INST);
    DL_UART_Main_enablePower(UART_ADC_INST);
    DL_ADC12_enablePower(ADC_UR_INST);
    DL_ADC12_enablePower(ADC_UX_INST);

    DL_MathACL_enablePower(MATHACL);
    delay_cycles(POWER_STARTUP_DELAY);
}

SYSCONFIG_WEAK void SYSCFG_DL_GPIO_init(void)
{

    DL_GPIO_initPeripheralOutputFunction(
        GPIO_UART_ADC_IOMUX_TX, GPIO_UART_ADC_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_UART_ADC_IOMUX_RX, GPIO_UART_ADC_IOMUX_RX_FUNC);

    DL_GPIO_initDigitalOutput(LED_PIN_1_IOMUX);

    DL_GPIO_initDigitalInputFeatures(KEY_PA_18_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(KEY_PB_21_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_setUpperPinsPolarity(GPIOA, DL_GPIO_PIN_18_EDGE_RISE);
    DL_GPIO_clearInterruptStatus(GPIOA, KEY_PA_18_PIN);
    DL_GPIO_enableInterrupt(GPIOA, KEY_PA_18_PIN);
    DL_GPIO_clearPins(GPIOB, LED_PIN_1_PIN);
    DL_GPIO_enableOutput(GPIOB, LED_PIN_1_PIN);
    DL_GPIO_setUpperPinsPolarity(GPIOB, DL_GPIO_PIN_21_EDGE_FALL);
    DL_GPIO_clearInterruptStatus(GPIOB, KEY_PB_21_PIN);
    DL_GPIO_enableInterrupt(GPIOB, KEY_PB_21_PIN);

}


SYSCONFIG_WEAK void SYSCFG_DL_SYSCTL_init(void)
{

	//Low Power Mode is configured to be SLEEP0
    DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_0);

    DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE);
    DL_SYSCTL_setULPCLKDivider(DL_SYSCTL_ULPCLK_DIV_1);
    DL_SYSCTL_setMCLKDivider(DL_SYSCTL_MCLK_DIVIDER_DISABLE);

}



/*
 * Timer clock configuration to be sourced by BUSCLK /  (4000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   4000000 Hz = 4000000 Hz / (8 * (0 + 1))
 */
static const DL_TimerA_ClockConfig gADC_TIMClockConfig = {
    .clockSel    = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_8,
    .prescale    = 0U,
};

/*
 * Timer load value (where the counter starts from) is calculated as (timerPeriod * timerClockFreq) - 1
 * ADC_TIM_INST_LOAD_VALUE = (1ms * 4000000 Hz) - 1
 */
static const DL_TimerA_TimerConfig gADC_TIMTimerConfig = {
    .period     = ADC_TIM_INST_LOAD_VALUE,
    .timerMode  = DL_TIMER_TIMER_MODE_ONE_SHOT,
    .startTimer = DL_TIMER_STOP,
};

SYSCONFIG_WEAK void SYSCFG_DL_ADC_TIM_init(void) {

    DL_TimerA_setClockConfig(ADC_TIM_INST,
        (DL_TimerA_ClockConfig *) &gADC_TIMClockConfig);

    DL_TimerA_initTimerMode(ADC_TIM_INST,
        (DL_TimerA_TimerConfig *) &gADC_TIMTimerConfig);
    DL_TimerA_enableInterrupt(ADC_TIM_INST , DL_TIMERA_INTERRUPT_ZERO_EVENT);
    DL_TimerA_enableClock(ADC_TIM_INST);


    DL_TimerA_enableEvent(ADC_TIM_INST, DL_TIMERA_EVENT_ROUTE_1, (DL_TIMERA_EVENT_ZERO_EVENT));

    DL_TimerA_setPublisherChanID(ADC_TIM_INST, DL_TIMERA_PUBLISHER_INDEX_0, ADC_TIM_INST_PUB_0_CH);

    DL_TimerA_enableEvent(ADC_TIM_INST, DL_TIMERA_EVENT_ROUTE_2, (DL_TIMERA_EVENT_ZERO_EVENT));

    DL_TimerA_setPublisherChanID(ADC_TIM_INST, DL_TIMERA_PUBLISHER_INDEX_1, ADC_TIM_INST_PUB_1_CH);


}


static const DL_UART_Main_ClockConfig gUART_ADCClockConfig = {
    .clockSel    = DL_UART_MAIN_CLOCK_BUSCLK,
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1
};

static const DL_UART_Main_Config gUART_ADCConfig = {
    .mode        = DL_UART_MAIN_MODE_NORMAL,
    .direction   = DL_UART_MAIN_DIRECTION_TX_RX,
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
    .parity      = DL_UART_MAIN_PARITY_NONE,
    .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_MAIN_STOP_BITS_ONE
};

SYSCONFIG_WEAK void SYSCFG_DL_UART_ADC_init(void)
{
    DL_UART_Main_setClockConfig(UART_ADC_INST, (DL_UART_Main_ClockConfig *) &gUART_ADCClockConfig);

    DL_UART_Main_init(UART_ADC_INST, (DL_UART_Main_Config *) &gUART_ADCConfig);
    /*
     * Configure baud rate by setting oversampling and baud rate divisors.
     *  Target baud rate: 9600
     *  Actual baud rate: 9600.24
     */
    DL_UART_Main_setOversampling(UART_ADC_INST, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_Main_setBaudRateDivisor(UART_ADC_INST, UART_ADC_IBRD_32_MHZ_9600_BAUD, UART_ADC_FBRD_32_MHZ_9600_BAUD);


    /* Configure FIFOs */
    DL_UART_Main_enableFIFOs(UART_ADC_INST);
    DL_UART_Main_setRXFIFOThreshold(UART_ADC_INST, DL_UART_RX_FIFO_LEVEL_1_2_FULL);
    DL_UART_Main_setTXFIFOThreshold(UART_ADC_INST, DL_UART_TX_FIFO_LEVEL_1_2_EMPTY);

    DL_UART_Main_enable(UART_ADC_INST);
}

/* ADC_UR Initialization */
static const DL_ADC12_ClockConfig gADC_URClockConfig = {
    .clockSel       = DL_ADC12_CLOCK_ULPCLK,
    .divideRatio    = DL_ADC12_CLOCK_DIVIDE_1,
    .freqRange      = DL_ADC12_CLOCK_FREQ_RANGE_24_TO_32,
};
SYSCONFIG_WEAK void SYSCFG_DL_ADC_UR_init(void)
{
    DL_ADC12_setClockConfig(ADC_UR_INST, (DL_ADC12_ClockConfig *) &gADC_URClockConfig);
    DL_ADC12_initSingleSample(ADC_UR_INST,
        DL_ADC12_REPEAT_MODE_ENABLED, DL_ADC12_SAMPLING_SOURCE_AUTO, DL_ADC12_TRIG_SRC_EVENT,
        DL_ADC12_SAMP_CONV_RES_12_BIT, DL_ADC12_SAMP_CONV_DATA_FORMAT_UNSIGNED);
    DL_ADC12_configConversionMem(ADC_UR_INST, ADC_UR_ADCMEM_0,
        DL_ADC12_INPUT_CHAN_0, DL_ADC12_REFERENCE_VOLTAGE_VDDA, DL_ADC12_SAMPLE_TIMER_SOURCE_SCOMP0, DL_ADC12_AVERAGING_MODE_DISABLED,
        DL_ADC12_BURN_OUT_SOURCE_DISABLED, DL_ADC12_TRIGGER_MODE_AUTO_NEXT, DL_ADC12_WINDOWS_COMP_MODE_DISABLED);
    DL_ADC12_enableFIFO(ADC_UR_INST);
    DL_ADC12_setPowerDownMode(ADC_UR_INST,DL_ADC12_POWER_DOWN_MODE_MANUAL);
    DL_ADC12_setSampleTime0(ADC_UR_INST,2);
    DL_ADC12_enableDMA(ADC_UR_INST);
    DL_ADC12_setDMASamplesCnt(ADC_UR_INST,1);
    DL_ADC12_enableDMATrigger(ADC_UR_INST,(DL_ADC12_DMA_MEM11_RESULT_LOADED
		 | DL_ADC12_DMA_MEM1_RESULT_LOADED
		 | DL_ADC12_DMA_MEM3_RESULT_LOADED
		 | DL_ADC12_DMA_MEM5_RESULT_LOADED
		 | DL_ADC12_DMA_MEM7_RESULT_LOADED
		 | DL_ADC12_DMA_MEM9_RESULT_LOADED));
    DL_ADC12_setSubscriberChanID(ADC_UR_INST,ADC_UR_INST_SUB_CH);
    /* Enable ADC12 interrupt */
    DL_ADC12_clearInterruptStatus(ADC_UR_INST,(DL_ADC12_INTERRUPT_DMA_DONE));
    DL_ADC12_enableInterrupt(ADC_UR_INST,(DL_ADC12_INTERRUPT_DMA_DONE));
    DL_ADC12_enableConversions(ADC_UR_INST);
}
/* ADC_UX Initialization */
static const DL_ADC12_ClockConfig gADC_UXClockConfig = {
    .clockSel       = DL_ADC12_CLOCK_ULPCLK,
    .divideRatio    = DL_ADC12_CLOCK_DIVIDE_1,
    .freqRange      = DL_ADC12_CLOCK_FREQ_RANGE_24_TO_32,
};
SYSCONFIG_WEAK void SYSCFG_DL_ADC_UX_init(void)
{
    DL_ADC12_setClockConfig(ADC_UX_INST, (DL_ADC12_ClockConfig *) &gADC_UXClockConfig);
    DL_ADC12_initSingleSample(ADC_UX_INST,
        DL_ADC12_REPEAT_MODE_ENABLED, DL_ADC12_SAMPLING_SOURCE_AUTO, DL_ADC12_TRIG_SRC_EVENT,
        DL_ADC12_SAMP_CONV_RES_12_BIT, DL_ADC12_SAMP_CONV_DATA_FORMAT_UNSIGNED);
    DL_ADC12_configConversionMem(ADC_UX_INST, ADC_UX_ADCMEM_0,
        DL_ADC12_INPUT_CHAN_0, DL_ADC12_REFERENCE_VOLTAGE_VDDA, DL_ADC12_SAMPLE_TIMER_SOURCE_SCOMP0, DL_ADC12_AVERAGING_MODE_DISABLED,
        DL_ADC12_BURN_OUT_SOURCE_DISABLED, DL_ADC12_TRIGGER_MODE_AUTO_NEXT, DL_ADC12_WINDOWS_COMP_MODE_DISABLED);
    DL_ADC12_enableFIFO(ADC_UX_INST);
    DL_ADC12_setPowerDownMode(ADC_UX_INST,DL_ADC12_POWER_DOWN_MODE_MANUAL);
    DL_ADC12_setSampleTime0(ADC_UX_INST,2);
    DL_ADC12_enableDMA(ADC_UX_INST);
    DL_ADC12_setDMASamplesCnt(ADC_UX_INST,1);
    DL_ADC12_enableDMATrigger(ADC_UX_INST,(DL_ADC12_DMA_MEM11_RESULT_LOADED
		 | DL_ADC12_DMA_MEM1_RESULT_LOADED
		 | DL_ADC12_DMA_MEM3_RESULT_LOADED
		 | DL_ADC12_DMA_MEM5_RESULT_LOADED
		 | DL_ADC12_DMA_MEM7_RESULT_LOADED
		 | DL_ADC12_DMA_MEM9_RESULT_LOADED));
    DL_ADC12_setSubscriberChanID(ADC_UX_INST,ADC_UX_INST_SUB_CH);
    /* Enable ADC12 interrupt */
    DL_ADC12_clearInterruptStatus(ADC_UX_INST,(DL_ADC12_INTERRUPT_DMA_DONE));
    DL_ADC12_enableInterrupt(ADC_UX_INST,(DL_ADC12_INTERRUPT_DMA_DONE));
    DL_ADC12_enableConversions(ADC_UX_INST);
}

static const DL_DMA_Config gDMA_URConfig = {
    .transferMode   = DL_DMA_FULL_CH_REPEAT_SINGLE_TRANSFER_MODE,
    .extendedMode   = DL_DMA_NORMAL_MODE,
    .destIncrement  = DL_DMA_ADDR_INCREMENT,
    .srcIncrement   = DL_DMA_ADDR_UNCHANGED,
    .destWidth      = DL_DMA_WIDTH_WORD,
    .srcWidth       = DL_DMA_WIDTH_WORD,
    .trigger        = ADC_UR_INST_DMA_TRIGGER,
    .triggerType    = DL_DMA_TRIGGER_TYPE_EXTERNAL,
};

SYSCONFIG_WEAK void SYSCFG_DL_DMA_UR_init(void)
{
    DL_DMA_initChannel(DMA, DMA_UR_CHAN_ID , (DL_DMA_Config *) &gDMA_URConfig);
}
static const DL_DMA_Config gDMA_UXConfig = {
    .transferMode   = DL_DMA_FULL_CH_REPEAT_SINGLE_TRANSFER_MODE,
    .extendedMode   = DL_DMA_NORMAL_MODE,
    .destIncrement  = DL_DMA_ADDR_INCREMENT,
    .srcIncrement   = DL_DMA_ADDR_UNCHANGED,
    .destWidth      = DL_DMA_WIDTH_WORD,
    .srcWidth       = DL_DMA_WIDTH_WORD,
    .trigger        = ADC_UX_INST_DMA_TRIGGER,
    .triggerType    = DL_DMA_TRIGGER_TYPE_EXTERNAL,
};

SYSCONFIG_WEAK void SYSCFG_DL_DMA_UX_init(void)
{
    DL_DMA_initChannel(DMA, DMA_UX_CHAN_ID , (DL_DMA_Config *) &gDMA_UXConfig);
}
SYSCONFIG_WEAK void SYSCFG_DL_DMA_init(void){
    SYSCFG_DL_DMA_UR_init();
    SYSCFG_DL_DMA_UX_init();
}


