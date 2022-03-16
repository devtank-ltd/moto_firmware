#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <stdio.h>
#include <inttypes.h>

#include "log.h"
#include "drv8704.h"
#include "pinmap.h"


/* REG ADDRESSES */
#define DRV8704_ADDR_CTRL                   0x00
#define DRV8704_ADDR_TORQUE                 0x01
#define DRV8704_ADDR_OFF                    0x02
#define DRV8704_ADDR_BLANK                  0x03
#define DRV8704_ADDR_DECAY                  0x04
#define DRV8704_ADDR_DRIVE                  0x06
#define DRV8704_ADDR_STATUS                 0x07

/* CTRL REG */
#define DRV8704_ADDR_CTRL_ENBL              0x001

#define DRV8704_ADDR_CTRL_ISGAIN_5V_V       0       /* 0b00 */
#define DRV8704_ADDR_CTRL_ISGAIN_10V_V      1       /* 0b01 */
#define DRV8704_ADDR_CTRL_ISGAIN_20V_V      2       /* 0b10 */
#define DRV8704_ADDR_CTRL_ISGAIN_40V_V      3       /* 0b11 */
#define DRV8704_ADDR_CTRL_ISGAIN_SHIFT      8
#define DRV8704_ADDR_CTRL_ISGAIN_MASK       0x300

#define DRV8704_ADDR_CTRL_DTIME_410_NS      0       /* 0b00 */
#define DRV8704_ADDR_CTRL_DTIME_460_NS      1       /* 0b01 */
#define DRV8704_ADDR_CTRL_DTIME_670_NS      2       /* 0b10 */
#define DRV8704_ADDR_CTRL_DTIME_880_NS      3       /* 0b11 */
#define DRV8704_ADDR_CTRL_DTIME_SHIFT       10
#define DRV8704_ADDR_CTRL_DTIME_MASK        0xC00


/* TORQUE REG */
#define DRV8704_ADDR_TORQUE_TORQUE_SHIFT    8
#define DRV8704_ADDR_TORQUE_TORQUE_MASK     0x0FF


/* OFF REG */
#define DRV8704_ADDR_OFF_TOFF_SHIFT         0
#define DRV8704_ADDR_OFF_TOFF_MASK          0x0FF

#define DRV8704_ADDR_OFF_PWMMODE            0x100


/* BLANK REG */
#define DRV8704_ADDR_BLANK_TBLANK_SHIFT     0
#define DRV8704_ADDR_BLANK_TBLANK_MASK      0x0FF


/* DECAY REG */
#define DRV8704_ADDR_DECAY_DECMOD_FSLOW     0       /* 0b000 */
#define DRV8704_ADDR_DECAY_DECMOD_FFAST     2       /* 0b010 */
#define DRV8704_ADDR_DECAY_DECMOD_MIXED     3       /* 0b011 */
#define DRV8704_ADDR_DECAY_DECMOD_AMIXED    5       /* 0b101 */
#define DRV8704_ADDR_DECAY_DECMOD_SHIFT     8
#define DRV8704_ADDR_DECAY_DECMOD_MASK      0x700

#define DRV8704_ADDR_DECAY_TDECAY_SHIFT     0
#define DRV8704_ADDR_DECAY_TDECAY_MASK      0x0FF


/* DRIVE REG */
#define DRV8704_ADDR_DRIVE_OCPTH_250_MV     0       /* 0b00 */
#define DRV8704_ADDR_DRIVE_OCPTH_500_MV     1       /* 0b01 */
#define DRV8704_ADDR_DRIVE_OCPTH_750_MV     2       /* 0b10 */
#define DRV8704_ADDR_DRIVE_OCPTH_1000_MV    3       /* 0b11 */
#define DRV8704_ADDR_DRIVE_OCPTH_SHIFT      0
#define DRV8704_ADDR_DRIVE_OCPTH_MASK       0x003

#define DRV8704_ADDR_DRIVE_OCPDEG_1_05_US   0       /* 0b00 */
#define DRV8704_ADDR_DRIVE_OCPDEG_2_1_US    1       /* 0b01 */
#define DRV8704_ADDR_DRIVE_OCPDEG_4_2_US    2       /* 0b10 */
#define DRV8704_ADDR_DRIVE_OCPDEG_8_4_US    3       /* 0b11 */
#define DRV8704_ADDR_DRIVE_OCPDEG_SHIFT     2
#define DRV8704_ADDR_DRIVE_OCPDEG_MASK      0x00C

#define DRV8704_ADDR_DRIVE_TDRIVEN_263_NS   0       /* 0b00 */
#define DRV8704_ADDR_DRIVE_TDRIVEN_525_NS   1       /* 0b01 */
#define DRV8704_ADDR_DRIVE_TDRIVEN_1050_NS  2       /* 0b10 */
#define DRV8704_ADDR_DRIVE_TDRIVEN_2100_NS  3       /* 0b11 */
#define DRV8704_ADDR_DRIVE_TDRIVEN_SHIFT    4
#define DRV8704_ADDR_DRIVE_TDRIVEN_MASK     0x030

#define DRV8704_ADDR_DRIVE_TDRIVEP_263_NS   0       /* 0b00 */
#define DRV8704_ADDR_DRIVE_TDRIVEP_525_NS   1       /* 0b01 */
#define DRV8704_ADDR_DRIVE_TDRIVEP_1050_NS  2       /* 0b10 */
#define DRV8704_ADDR_DRIVE_TDRIVEP_2100_NS  3       /* 0b11 */
#define DRV8704_ADDR_DRIVE_TDRIVEP_SHIFT    6
#define DRV8704_ADDR_DRIVE_TDRIVEP_MASK     0x0C0

#define DRV8704_ADDR_DRIVE_IDRIVEN_100_mA   0       /* 0b00 */
#define DRV8704_ADDR_DRIVE_IDRIVEN_200_mA   1       /* 0b01 */
#define DRV8704_ADDR_DRIVE_IDRIVEN_300_mA   2       /* 0b10 */
#define DRV8704_ADDR_DRIVE_IDRIVEN_400_mA   3       /* 0b11 */
#define DRV8704_ADDR_DRIVE_IDRIVEN_SHIFT    8
#define DRV8704_ADDR_DRIVE_IDRIVEN_MASK     0x300

#define DRV8704_ADDR_DRIVE_IDRIVEP_100_mA   0       /* 0b00 */
#define DRV8704_ADDR_DRIVE_IDRIVEP_200_mA   1       /* 0b01 */
#define DRV8704_ADDR_DRIVE_IDRIVEP_300_mA   2       /* 0b10 */
#define DRV8704_ADDR_DRIVE_IDRIVEP_400_mA   3       /* 0b11 */
#define DRV8704_ADDR_DRIVE_IDRIVEP
#define DRV8704_ADDR_DRIVE_IDRIVEP_SHIFT    10
#define DRV8704_ADDR_DRIVE_IDRIVEP_MASK     0xC00


/* STATUS REG */
#define DRV8704_ADDR_STATUS_UVLO            0x20
#define DRV8704_ADDR_STATUS_BPDF            0x10
#define DRV8704_ADDR_STATUS_APDF            0x08
#define DRV8704_ADDR_STATUS_BOCP            0x04
#define DRV8704_ADDR_STATUS_AOCP            0x02
#define DRV8704_ADDR_STATUS_OTS             0x01


#define DRV8704_XFER_SIZE                   3


typedef union _drv8704_xfer_t
{
    struct
    {
        uint16_t value:12;   /**< Register value */
        uint16_t addr:3;    /**< Register offset address */
        uint16_t wr:1;
    } info;
    uint16_t buffer;
} drv8704_xfer_t;


static void drv8704_delay(uint32_t delay)
{
    for (unsigned n=0; n<delay; n++)
    {
        asm("nop");
    }
}


void drv8704_reset(void)
{
    log_debug(DEBUG_SPI, "Resetting...");
    gpio_clear(DRV8704_RS_PORT, DRV8704_RS_PIN);
    drv8704_delay(10000);
    gpio_set(DRV8704_RS_PORT, DRV8704_RS_PIN);
}


static void drv8704_enable_device(void)
{
    gpio_set(DRV8704_CS_PORT, DRV8704_CS_PIN);
}


static void drv8704_disable_device(void)
{
    drv8704_delay(10000);
    gpio_clear(DRV8704_CS_PORT, DRV8704_CS_PIN);
}


static void drv8704_xfer(drv8704_xfer_t* xfer)
{
    log_debug(DEBUG_SPI, "XFER WR:%04"PRIX16, xfer->buffer);
    drv8704_enable_device();
    xfer->buffer = spi_xfer(SPI1, xfer->buffer);
    drv8704_disable_device();
    log_debug(DEBUG_SPI, "XFER RD:%04"PRIX16, xfer->buffer);
}


static void drv8704_write_register(uint8_t addr, uint16_t value)
{
    log_debug(DEBUG_SPI, "drv8704_write_register addr:0x%02"PRIX8" value:0x%03"PRIX16, addr, value);
    drv8704_xfer_t xfer = {.info = {.wr = 0, .addr = addr, .value = value}};
    drv8704_xfer(&xfer);
}


static uint16_t drv8704_read_register(uint8_t addr)
{
    drv8704_xfer_t xfer = {.info = {.wr = 1, .addr = addr, .value = 0x0FFF}};
    drv8704_xfer(&xfer);
    log_debug(DEBUG_SPI, "drv8704_read_register addr:0x%02"PRIX8" value:0x%03"PRIX16, addr, xfer.info.value);
    return xfer.info.value;
}


void drv8704_init(void)
{
    // Assign clock
    rcc_periph_clock_enable(PORT_TO_RCC(DRV8704_SPI_PORT));
    rcc_periph_clock_enable(PORT_TO_RCC(DRV8704_CS_PORT));
    rcc_periph_clock_enable(RCC_SPI1);

    gpio_mode_setup(DRV8704_CS_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, DRV8704_CS_PIN);
    gpio_mode_setup(DRV8704_RS_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, DRV8704_RS_PIN);

    gpio_clear(DRV8704_RS_PORT, DRV8704_RS_PIN);

    // Assign alternative function 0 to SPI GPIO pins
    gpio_mode_setup(DRV8704_SPI_PORT,
                    GPIO_MODE_AF,
                    GPIO_PUPD_NONE,
                    DRV8704_SPI_AF_GPIOS);

    gpio_set_af(DRV8704_SPI_PORT,
                GPIO_AF0,
                DRV8704_SPI_AF_GPIOS);

    spi_reset(SPI1);

    // Init as SPI master and set data size = 8-bits
    spi_init_master(SPI1,
                    SPI_CR1_BAUDRATE_FPCLK_DIV_256,
                    SPI_CR1_CPOL_CLK_TO_1_WHEN_IDLE,
                    SPI_CR1_CPHA_CLK_TRANSITION_2,
                    SPI_CR1_MSBFIRST);

    spi_set_data_size(SPI1, SPI_CR2_DS_16BIT);
    spi_set_clock_polarity_1(SPI1);
    spi_set_clock_phase_1(SPI1);
    spi_set_nss_low(SPI1);
    spi_enable_software_slave_management(SPI1);
    spi_enable(SPI1);
}


void drv8704_write(uint8_t addr, uint16_t value)
{
    if (addr > 8)
    {
        log_debug(DEBUG_SPI, "Max address is 0x07.");
        return;
    }
    if (value & 0xF000)
    {
        log_debug(DEBUG_SPI, "Register size is 12 bits.");
        return;
    }
    drv8704_write_register(addr, value);
}


bool drv8704_read(uint8_t addr, uint16_t* value)
{
    if (addr > 8)
    {
        log_debug(DEBUG_SPI, "Max address is 0x07.");
        return false;
    }
    if (!value)
    {
        log_debug(DEBUG_SPI, "Handed a NULL pointer.");
        return false;
    }
    *value = drv8704_read_register(addr);
    return true;
}
