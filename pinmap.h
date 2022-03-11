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
    { USART3, RCC_USART3, UART_3_SPEED, UART_3_DATABITS, UART_3_PARITY, UART_3_STOP, GPIOB, GPIO10 | GPIO11, GPIO_AF4, NVIC_USART3_4_IRQ, (uint32_t)&USART3_TDR, NVIC_DMA1_CHANNEL4_7_DMA2_CHANNEL3_5_IRQ, DMA_CHANNEL7, UART3_PRIORITY, 0 }, /* UART 0 */ \
}

#define UART_CHANNELS_COUNT 1
#define UART_DEBUG          USART3


#define PWM0_IO_NUM          0
#define PWM1_IO_NUM          1

#define PWM0_PORT          GPIOB
#define PWM0_PIN           GPIO14
#define PWM0_ALTFUNC       GPIO_AF1

#define PWM1_PORT          GPIOA
#define PWM1_PIN           GPIO6
#define PWM1_ALTFUNC       GPIO_AF1

#define RCC_PWM0_TIM RCC_TIM15
#define RCC_PWM1_TIM RCC_TIM3

#define PWM0_TIM TIM15
#define PWM1_TIM TIM3

#define PWM0_TIM_CH TIM_OC1
#define PWM1_TIM_CH TIM_OC1

#define PWM_COUNT 2


#define IOS_PORT_N_PINS                         \
{                                               \
    {PWM0_PORT, PWM0_PIN},   /* IO 0 - PWM 0 */ \
    {PWM1_PORT, PWM1_PIN},   /* IO 1 - PWM 1 */ \
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

#endif //__PINMAPS__
