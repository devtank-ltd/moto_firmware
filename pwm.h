#ifndef __PWM__
#define __PWM__

#include <stdbool.h>

extern void pwm_enable(unsigned pwm, bool enable);
extern void pwm_set_freq(unsigned pwm, unsigned pwm_duty);
extern void pwm_set_duty(unsigned pwm, unsigned pwm_duty);

extern bool pwm_get(unsigned pwm, unsigned * pwm_freq, unsigned * pwm_duty);

extern void pwm_init(void);


#endif //__PWM__
