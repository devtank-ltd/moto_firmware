#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>

#include "pwm.h"
#include "pinmap.h"

typedef struct
{
    unsigned       freq;
    unsigned       duty;
    uint32_t       tim;
    enum tim_oc_id oc_id;
} pwm_setup_t;


static pwm_setup_t pwm_setups[PWM_COUNT] =
{
    { 1000, 100, PWM1_TIM, PWM1_TIM_CH },
    { 1000, 100, PWM2_TIM, PWM2_TIM_CH },
};


static void _setup_pwm_timer(uint32_t tim)
{
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
    timer_set_period(pwm_setup->tim, 1000000 / pwm_setup->freq);

    timer_disable_oc_output(pwm_setup->tim, pwm_setup->oc_id);
    timer_set_oc_mode(pwm_setup->tim, pwm_setup->oc_id, TIM_OCM_PWM1);
    timer_enable_oc_output(pwm_setup->tim, pwm_setup->oc_id);

    timer_enable_counter(pwm_setup->tim);
    unsigned duty = 100 - pwm_setup->duty;

    timer_set_oc_value(pwm_setup->tim, pwm_setup->oc_id, 1000000/pwm_setup->freq * duty / 100);
    timer_set_counter(pwm_setup->tim, 0);
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
        _start_pwm(&pwm_setups[pwm]);
    else
        _stop_pwm(&pwm_setups[pwm]);
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
    rcc_periph_clock_enable(RCC_PWM1_TIM);
    rcc_periph_clock_enable(RCC_PWM2_TIM);

    _setup_pwm_timer(PWM1_TIM);
    _setup_pwm_timer(PWM2_TIM);
}
