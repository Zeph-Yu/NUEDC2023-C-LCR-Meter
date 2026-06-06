/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
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
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G350X
#define CONFIG_MSPM0G3507

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)


#define CPUCLK_FREQ                                                     32000000



/* Defines for ADC_TIM */
#define ADC_TIM_INST                                                     (TIMA0)
#define ADC_TIM_INST_IRQHandler                                 TIMA0_IRQHandler
#define ADC_TIM_INST_INT_IRQN                                   (TIMA0_INT_IRQn)
#define ADC_TIM_INST_LOAD_VALUE                                          (3999U)
#define ADC_TIM_INST_PUB_0_CH                                                (1)
#define ADC_TIM_INST_PUB_1_CH                                                (2)



/* Defines for UART_ADC */
#define UART_ADC_INST                                                      UART0
#define UART_ADC_INST_FREQUENCY                                         32000000
#define UART_ADC_INST_IRQHandler                                UART0_IRQHandler
#define UART_ADC_INST_INT_IRQN                                    UART0_INT_IRQn
#define GPIO_UART_ADC_RX_PORT                                              GPIOA
#define GPIO_UART_ADC_TX_PORT                                              GPIOA
#define GPIO_UART_ADC_RX_PIN                                      DL_GPIO_PIN_11
#define GPIO_UART_ADC_TX_PIN                                      DL_GPIO_PIN_10
#define GPIO_UART_ADC_IOMUX_RX                                   (IOMUX_PINCM22)
#define GPIO_UART_ADC_IOMUX_TX                                   (IOMUX_PINCM21)
#define GPIO_UART_ADC_IOMUX_RX_FUNC                    IOMUX_PINCM22_PF_UART0_RX
#define GPIO_UART_ADC_IOMUX_TX_FUNC                    IOMUX_PINCM21_PF_UART0_TX
#define UART_ADC_BAUD_RATE                                                (9600)
#define UART_ADC_IBRD_32_MHZ_9600_BAUD                                     (208)
#define UART_ADC_FBRD_32_MHZ_9600_BAUD                                      (21)





/* Defines for ADC_UR */
#define ADC_UR_INST                                                         ADC0
#define ADC_UR_INST_IRQHandler                                   ADC0_IRQHandler
#define ADC_UR_INST_INT_IRQN                                     (ADC0_INT_IRQn)
#define ADC_UR_ADCMEM_0                                       DL_ADC12_MEM_IDX_0
#define ADC_UR_ADCMEM_0_REF                      DL_ADC12_REFERENCE_VOLTAGE_VDDA
#define ADC_UR_ADCMEM_0_REF_VOLTAGE_V                                        3.3
#define ADC_UR_INST_SUB_CH                                                   (1)
#define GPIO_ADC_UR_C0_PORT                                                GPIOA
#define GPIO_ADC_UR_C0_PIN                                        DL_GPIO_PIN_27
#define GPIO_ADC_UR_IOMUX_C0                                     (IOMUX_PINCM60)
#define GPIO_ADC_UR_IOMUX_C0_FUNC                 (IOMUX_PINCM60_PF_UNCONNECTED)

/* Defines for ADC_UX */
#define ADC_UX_INST                                                         ADC1
#define ADC_UX_INST_IRQHandler                                   ADC1_IRQHandler
#define ADC_UX_INST_INT_IRQN                                     (ADC1_INT_IRQn)
#define ADC_UX_ADCMEM_0                                       DL_ADC12_MEM_IDX_0
#define ADC_UX_ADCMEM_0_REF                      DL_ADC12_REFERENCE_VOLTAGE_VDDA
#define ADC_UX_ADCMEM_0_REF_VOLTAGE_V                                        3.3
#define ADC_UX_INST_SUB_CH                                                   (2)
#define GPIO_ADC_UX_C0_PORT                                                GPIOA
#define GPIO_ADC_UX_C0_PIN                                        DL_GPIO_PIN_15
#define GPIO_ADC_UX_IOMUX_C0                                     (IOMUX_PINCM37)
#define GPIO_ADC_UX_IOMUX_C0_FUNC                 (IOMUX_PINCM37_PF_UNCONNECTED)



/* Defines for DMA_UR */
#define DMA_UR_CHAN_ID                                                       (0)
#define ADC_UR_INST_DMA_TRIGGER                       (DMA_ADC0_EVT_GEN_BD_TRIG)
/* Defines for DMA_UX */
#define DMA_UX_CHAN_ID                                                       (1)
#define ADC_UX_INST_DMA_TRIGGER                       (DMA_ADC1_EVT_GEN_BD_TRIG)


/* Port definition for Pin Group LED */
#define LED_PORT                                                         (GPIOB)

/* Defines for PIN_1: GPIOB.13 with pinCMx 30 on package pin 1 */
#define LED_PIN_1_PIN                                           (DL_GPIO_PIN_13)
#define LED_PIN_1_IOMUX                                          (IOMUX_PINCM30)
/* Defines for PA_18: GPIOA.18 with pinCMx 40 on package pin 11 */
#define KEY_PA_18_PORT                                                   (GPIOA)
// pins affected by this interrupt request:["PA_18"]
#define KEY_GPIOA_INT_IRQN                                      (GPIOA_INT_IRQn)
#define KEY_GPIOA_INT_IIDX                      (DL_INTERRUPT_GROUP1_IIDX_GPIOA)
#define KEY_PA_18_IIDX                                      (DL_GPIO_IIDX_DIO18)
#define KEY_PA_18_PIN                                           (DL_GPIO_PIN_18)
#define KEY_PA_18_IOMUX                                          (IOMUX_PINCM40)
/* Defines for PB_21: GPIOB.21 with pinCMx 49 on package pin 20 */
#define KEY_PB_21_PORT                                                   (GPIOB)
// pins affected by this interrupt request:["PB_21"]
#define KEY_GPIOB_INT_IRQN                                      (GPIOB_INT_IRQn)
#define KEY_GPIOB_INT_IIDX                      (DL_INTERRUPT_GROUP1_IIDX_GPIOB)
#define KEY_PB_21_IIDX                                      (DL_GPIO_IIDX_DIO21)
#define KEY_PB_21_PIN                                           (DL_GPIO_PIN_21)
#define KEY_PB_21_IOMUX                                          (IOMUX_PINCM49)




/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_ADC_TIM_init(void);
void SYSCFG_DL_UART_ADC_init(void);
void SYSCFG_DL_ADC_UR_init(void);
void SYSCFG_DL_ADC_UX_init(void);
void SYSCFG_DL_DMA_init(void);



bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
