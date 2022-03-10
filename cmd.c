#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <libopencm3/stm32/gpio.h>

#include "pinmap.h"
#include "log.h"
#include "cmd.h"
#include "uarts.h"
#include "usb_uarts.h"
#include "io.h"
#include "pwm.h"

static char   * rx_buffer;
static unsigned rx_buffer_len = 0;
static unsigned rx_pos        = 0;


typedef struct
{
    const char * key;
    const char * desc;
    void (*cb)(void);
} cmd_t;


static char * skip_space(char * pos)
{
    while(*pos == ' ')
        pos++;
    return pos;
}

static char * skip_to_space(char * pos)
{
    while(*pos && *pos != ' ')
        pos++;
    return pos;
}


void io_cb(void)
{
    char * pos = NULL;
    unsigned io = strtoul(rx_buffer + rx_pos, &pos, 10);
    pos = skip_space(pos);
    bool do_read = false;

    if (*pos == ':')
    {
        do_read = true;
        bool as_input;
        pos = skip_space(pos + 1);
        if (strncmp(pos, "IN", 2) == 0 || *pos == 'I')
        {
            pos = skip_to_space(pos);
            as_input = true;
        }
        else if (strncmp(pos, "OUT", 3) == 0 || *pos == 'O')
        {
            pos = skip_to_space(pos);
            as_input = false;
        }
        else
        {
            log_error("Malformed gpio type command");
            return;
        }

        unsigned pull = GPIO_PUPD_NONE;

        pos = skip_space(pos);

        if (*pos && *pos != '=')
        {
            if (strncmp(pos, "UP", 2) == 0 || *pos == 'U')
            {
                pos = skip_to_space(pos);
                pull = GPIO_PUPD_PULLUP;
            }
            else if (strncmp(pos, "DOWN", 4) == 0 || *pos == 'D')
            {
                pos = skip_to_space(pos);
                pull = GPIO_PUPD_PULLDOWN;
            }
            else if (strncmp(pos, "NONE", 4) == 0 || *pos == 'N')
            {
                pos = skip_to_space(pos);
            }
            else
            {
                log_error("Malformed gpio pull command");
                return;
            }
            pos = skip_space(pos);
        }

        io_configure(io, as_input, pull);
    }

    if (*pos == '=')
    {
        pos = skip_space(pos + 1);
        if (strncmp(pos, "ON", 2) == 0 || *pos == '1')
        {
            pos = skip_to_space(pos);
            if (!io_is_input(io))
            {
                io_on(io, true);
                if (!do_read)
                    log_out("IO %02u = ON", io);
            }
            else log_error("IO %02u is input but output command.", io);
        }
        else if (strncmp(pos, "OFF", 3) == 0 || *pos == '0')
        {
            pos = skip_to_space(pos);
            if (!io_is_input(io))
            {
                io_on(io, false);
                if (!do_read)
                    log_out("IO %02u = OFF", io);
            }
            else log_error("IO %02u is input but output command.", io);
        }
        else
        {
            log_error("Malformed gpio on/off command");
            return;
        }
    }
    else do_read = true;

    if (do_read)
        io_log(io);
}


void special_cb(void)
{
    char * pos = NULL;
    unsigned io = strtoul(rx_buffer + rx_pos, &pos, 10);

    if (io_enable_special(io))
        log_out("IO %02u special enabled", io);
    else
        log_out("IO %02u has no special", io);
}


void pwm_cb(void)
{
    char * pos = rx_buffer + rx_pos;
    unsigned pwm = strtoul(pos, &pos, 10);
    unsigned freq, duty;
    if (pos && *pos)
    {
        pos = skip_space(pos);
        freq = strtoul(pos, &pos, 10);
        pwm_set_freq(pwm, freq);
        if (pos && *pos)
        {
            pos = skip_space(pos);
            duty = strtoul(pos, &pos, 10);
            pwm_set_duty(pwm, duty);
        }

        pwm_enable(pwm, false);
        pwm_enable(pwm, true);
    }

    if (pwm_get(pwm, &freq, &duty))
        log_out("PWM %u %uhz @ %u / 100", pwm, freq, duty);
    else
        log_out("Invalid PWM");
}


void version_cb(void)
{
    log_out("Version : %s", GIT_VERSION);
}


static cmd_t cmds[] = {
    { "ios",      "Print all IOs.",          ios_log},
    { "io",       "Get/set IO set.",         io_cb},
    { "sio",      "Enable Special IO.",      special_cb},
    { "pwm",      "Get/Set PWM settings.",   pwm_cb},
    { "version",  "Print version.",          version_cb},
    { NULL },
};


void cmds_process(char * command, unsigned len)
{
    if (!len)
        return;

    log_debug(DEBUG_SYS, "Command \"%s\"", command);

    rx_buffer = command;
    rx_buffer_len = len;

    bool found = false;
    log_out(LOG_START_SPACER);
    for(cmd_t * cmd = cmds; cmd->key; cmd++)
    {
        unsigned keylen = strlen(cmd->key);
        if(rx_buffer_len >= keylen &&
           !strncmp(cmd->key, rx_buffer, keylen) &&
           (rx_buffer[keylen] == '\0' || rx_buffer[keylen] == ' '))
        {
            rx_pos = keylen;
            found = true;
            cmd->cb();
            break;
        }
    }
    if (!found)
    {
        log_out("Unknown command \"%s\"", rx_buffer);
        log_out(LOG_SPACER);
        for(cmd_t * cmd = cmds; cmd->key; cmd++)
            log_out("%10s : %s", cmd->key, cmd->desc);
    }
    log_out(LOG_END_SPACER);
}


void cmds_init()
{
}
