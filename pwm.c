#include "pwm.h"
#include "pinmap.h"

typedef struct
{
    bool     enabled;
    unsigned pwm_freq;
    unsigned pwm_duty;
} pwm_setup_t;


static pwm_setup_t pwm_setups[PWM_COUNT] = { 0 };


void pwm_enable(unsigned pwm, bool enable)
{
    if (pwm >= PWM_COUNT)
        return;
    pwm_setups[pwm].enabled = enable;
}


void pwm_set_freq(unsigned pwm, unsigned pwm_freq)
{
    if (pwm >= PWM_COUNT)
        return;
    pwm_setups[pwm].pwm_freq = pwm_freq;
}


void pwm_set_duty(unsigned pwm, unsigned pwm_duty)
{
    if (pwm >= PWM_COUNT)
        return;
    pwm_setups[pwm].pwm_duty = pwm_duty;
}


bool pwm_get(unsigned pwm, unsigned * pwm_freq, unsigned * pwm_duty)
{
    if (pwm >= PWM_COUNT)
        return false;

    pwm_setup_t * pwm_setup = &pwm_setups[pwm];
    if (pwm_freq)
        *pwm_freq = pwm_setup->pwm_freq;
    if (pwm_duty)
        *pwm_duty = pwm_setup->pwm_duty;
    return true;
}


void pwm_init(void)
{
}
