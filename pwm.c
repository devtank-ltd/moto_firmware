#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/gpio.h>

#include "pwm.h"
#include "pinmap.h"
#include "log.h"

typedef struct
{
    unsigned       freq;
    unsigned       duty;
    uint32_t       tim;
    enum tim_oc_id oc_id;
    port_n_pins_t  pin;
    uint32_t       alt_func_num;
} pwm_setup_t;


static pwm_setup_t pwm_setups[PWM_COUNT] =
{
    { 1000, 50, PWM0_TIM, PWM0_TIM_CH, {PWM0_PORT, PWM0_PIN}, PWM0_ALTFUNC },
    { 1000, 50, PWM1_TIM, PWM1_TIM_CH, {PWM1_PORT, PWM1_PIN}, PWM1_ALTFUNC },
};



static void start_timer(uint32_t timer_peripheral, enum tim_oc_id oc_id)
{
    timer_set_period(timer_peripheral, 1000000);

    timer_disable_oc_output(timer_peripheral,oc_id);
    timer_set_oc_mode(timer_peripheral,oc_id, TIM_OCM_PWM1);
    timer_enable_oc_output(timer_peripheral,oc_id);

    timer_enable_counter(timer_peripheral);

    timer_set_oc_value(timer_peripheral, oc_id, 0);
}


static void set_pwm(uint32_t timer_peripheral, enum tim_oc_id oc_id, unsigned freq, unsigned duty)
{
    timer_set_period(timer_peripheral,  1000000/freq);

    if (duty > 100)
        duty = 100;

    timer_set_oc_value(timer_peripheral, oc_id, 1000000/freq * duty / 100);

    timer_set_counter(timer_peripheral, 1000000/freq * duty / 4 / 100);
}


static void _setup_pwm(pwm_setup_t * pwm_setup)
{
    uint32_t tim = pwm_setup->tim;

    rcc_periph_clock_enable(PORT_TO_RCC(pwm_setup->pin.port));

    timer_disable_counter(tim);

    timer_set_mode(tim,
                   TIM_CR1_CKD_CK_INT,
                   TIM_CR1_CMS_EDGE,
                   TIM_CR1_DIR_UP);
    timer_set_prescaler(tim, rcc_ahb_frequency/1000000);

    timer_enable_preload(tim);
    timer_continuous_mode(tim);
}


static void _start_pwm(pwm_setup_t * pwm_setup)
{
    gpio_mode_setup( pwm_setup->pin.port, GPIO_MODE_AF, GPIO_PUPD_NONE, pwm_setup->pin.pins);
    gpio_set_af( pwm_setup->pin.port, pwm_setup->alt_func_num, pwm_setup->pin.pins );
    gpio_set_output_options( pwm_setup->pin.port, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, pwm_setup->pin.pins );

    start_timer(pwm_setup->tim, pwm_setup->oc_id);

    unsigned duty = 100 - pwm_setup->duty;
    set_pwm(pwm_setup->tim, pwm_setup->oc_id, pwm_setup->freq, duty);
}


static void _stop_pwm(pwm_setup_t * pwm_setup)
{
    timer_disable_oc_output(pwm_setup->tim, pwm_setup->oc_id);
    timer_disable_counter(pwm_setup->tim);
}


void pwm_enable(unsigned pwm, bool enable)
{
    if (pwm >= PWM_COUNT)
        return;
    if (enable)
    {
        pwm_setup_t * pwm_setup = &pwm_setups[pwm];
        _start_pwm(pwm_setup);
        log_debug(DEBUG_PWM, "Started PWM %u %uhz %u / 100", pwm, pwm_setup->freq, pwm_setup->duty);
    }
    else
    {
        _stop_pwm(&pwm_setups[pwm]);
        log_debug(DEBUG_PWM, "Stopped PWM %u", pwm);
    }
}


void pwm_set_freq(unsigned pwm, unsigned pwm_freq)
{
    if (pwm >= PWM_COUNT)
        return;
    pwm_setups[pwm].freq = pwm_freq;
}


void pwm_set_duty(unsigned pwm, unsigned pwm_duty)
{
    if (pwm >= PWM_COUNT)
        return;
    if (pwm_duty > 100)
        pwm_duty = 100;

    pwm_setups[pwm].duty = pwm_duty;
}


bool pwm_get(unsigned pwm, unsigned * pwm_freq, unsigned * pwm_duty)
{
    if (pwm >= PWM_COUNT)
        return false;

    pwm_setup_t * pwm_setup = &pwm_setups[pwm];
    if (pwm_freq)
        *pwm_freq = pwm_setup->freq;
    if (pwm_duty)
        *pwm_duty = pwm_setup->duty;
    return true;
}


void pwm_init(void)
{
    rcc_periph_clock_enable(RCC_PWM0_TIM);
    rcc_periph_clock_enable(RCC_PWM1_TIM);

    timer_enable_break_main_output(PWM0_TIM);

    _setup_pwm(&pwm_setups[0]);
    _setup_pwm(&pwm_setups[1]);

    pwm_enable(0, true);
    pwm_enable(1, true);
}
