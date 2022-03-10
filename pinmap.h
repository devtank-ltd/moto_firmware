#ifndef __PINMAPS__
#define __PINMAPS__

#include <libopencm3/stm32/rcc.h>
#include <stdint.h>

typedef struct
{
    uint32_t port;
    uint32_t pins;
} port_n_pins_t;

#define PORT_TO_RCC(_port_)   (RCC_GPIOA + ((_port_ - GPIO_PORT_A_BASE) / 0x400))

#define LED_PORT   GPIOA
#define LED_PIN    GPIO5

#define ADCS_PORT_N_PINS                            \
{                                                   \
    {GPIOA, GPIO4},      /* ADC 0  = Channel 4  */  \
    {GPIOA, GPIO6},      /* ADC 1  = Channel 6  */  \
    {GPIOA, GPIO7},      /* ADC 2  = Channel 7  */  \
    {GPIOB, GPIO0},      /* ADC 3  = Channel 8  */  \
    {GPIOB, GPIO1},      /* ADC 4  = Channel 9  */  \
    {GPIOC, GPIO0},      /* ADC 5  = Channel 10 */  \
    {GPIOC, GPIO1},      /* ADC 6  = Channel 11 */  \
    {GPIOC, GPIO3},      /* ADC 7  = Channel 13 */  \
    {GPIOC, GPIO4},      /* ADC 8  = Channel 14 */  \
    {GPIOC, GPIO5},      /* ADC 9  = Channel 15 */  \
}

#define ADC_CHANNELS  {4,6,7,8,9,10,11,13,14,15}

#define ADC_COUNT 10

#define PPS_PORT_N_PINS             \
{                                   \
    {GPIOB, GPIO3},     /* PPS 0 */ \
    {GPIOC, GPIO7},     /* PPS 1 */ \
}


#define PPS_EXTI      \
{                     \
    {TIM1, EXTI3},    \
    {TIM14, EXTI7},   \
}

#define PPS_INIT                     \
{                                    \
    {RCC_TIM1, NVIC_EXTI2_3_IRQ },   \
    {RCC_TIM14, NVIC_EXTI4_15_IRQ},  \
}

#define PPS0_EXTI_ISR        exti2_3_isr
#define PPS1_EXTI_ISR        exti4_15_isr

/*subset of usb_cdc_line_coding_bParityType*/
typedef enum
{
    uart_parity_none = 0,
    uart_parity_odd  = 1,
    uart_parity_even = 2,
} uart_parity_t;

/*matches usb_cdc_line_coding_bCharFormat*/
typedef enum
{
    uart_stop_bits_1   = 0,
    uart_stop_bits_1_5 = 1,
    uart_stop_bits_2   = 2,
} uart_stop_bits_t;

typedef struct
{
    uint32_t              usart;
    enum rcc_periph_clken uart_clk;
    uint32_t              baud;
    uint8_t               databits:4;
    uint8_t               parity:2 /*uart_parity_t*/;
    uint8_t               stop:2 /*uart_stop_bits_t*/;
    uint32_t              gpioport;
    uint16_t              pins;
    uint8_t               alt_func_num;
    uint8_t               irqn;
    uint32_t              dma_addr;
    uint8_t               dma_irqn;
    uint8_t               dma_channel;
    uint8_t               priority;
    uint8_t               enabled;
} uart_channel_t;

#define UART_CHANNELS                                                                                                   \
{                                                                                                                       \
    { USART2, RCC_USART2, UART_2_SPEED, UART_2_DATABITS, UART_2_PARITY, UART_2_STOP, GPIOA, GPIO2  | GPIO3,  GPIO_AF1, NVIC_USART2_IRQ,   (uint32_t)&USART2_TDR, NVIC_DMA1_CHANNEL4_7_DMA2_CHANNEL3_5_IRQ, DMA_CHANNEL4, UART2_PRIORITY, 0 }, /* UART 0 */ \
}

#define UART_CHANNELS_COUNT 1
#define UART_DEBUG USART2


#define IOS_PORT_N_PINS            \
{                                  \
    {GPIOB, GPIO14},   /* IO 0 - PWM 0 */ \
    {GPIOA, GPIO6},    /* IO 1 - PWM 1 */ \
}

#define IO_AS_INPUT     0x0100
#define IO_DIR_LOCKED   0x0200
#define IO_SPECIAL_EN   0x0400
#define IO_UART_TX      0x0800
#define IO_RELAY        0x1000
#define IO_HIGHSIDE     0x2000
#define IO_PWM0         0x3000
#define IO_PWM1         0x4000
#define IO_TYPE_MASK    0xF000
#define IO_PULL_MASK    0x0003

#define IOS_STATE                                                            \
{                                                                            \
    IO_AS_INPUT | IO_PWM0 | IO_SPECIAL_EN,            /* GPIO 0 - PWM 0 */   \
    IO_AS_INPUT | IO_PWM1 | IO_SPECIAL_EN,            /* GPIO 1 - PWM 1 */   \
}

#define PWM0_IO_NUM          0
#define PWM1_IO_NUM          1

#endif //__PINMAPS__
