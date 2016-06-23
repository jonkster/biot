#ifndef SSD1306_H_INCLUDED
#define SSD1306_H_INCLUDED

#include <inttypes.h>
#include <stdio.h>
#include <xtimer.h>

#include "periph/i2c.h"

#define ioport_set_pin_low printf
#define ioport_set_pin_high printf

#define     SSD1306_CLOCK_SPEED   1
#define     SSD1306_CS_PIN   1
#define     SSD1306_DC_PIN   1
#define     SSD1306_DISPLAY_CONTRAST_MAX   40
#define     SSD1306_DISPLAY_CONTRAST_MIN   30
#define     SSD1306_RES_PIN   "1"
#define     SSD1306_USART_SPI   1


#define delay_us xtimer_usleep

#ifdef __cplusplus
extern "C" {
#endif



#define SSD1306_CMD_SET_LOW_COL(column)             (0x00 | (column))
#define SSD1306_CMD_SET_HIGH_COL(column)            (0x10 | (column))
#define SSD1306_CMD_SET_MEMORY_ADDRESSING_MODE      0x20
#define SSD1306_CMD_SET_COLUMN_ADDRESS              0x21
#define SSD1306_CMD_SET_PAGE_ADDRESS                0x22
#define SSD1306_CMD_SET_START_LINE(line)            (0x40 | (line))
#define SSD1306_CMD_SET_CONTRAST_CONTROL_FOR_BANK0  0x81
#define SSD1306_CMD_SET_CHARGE_PUMP_SETTING         0x8D
#define SSD1306_CMD_SET_SEGMENT_RE_MAP_COL0_SEG0    0xA0
#define SSD1306_CMD_SET_SEGMENT_RE_MAP_COL127_SEG0  0xA1
#define SSD1306_CMD_ENTIRE_DISPLAY_AND_GDDRAM_ON    0xA4
#define SSD1306_CMD_ENTIRE_DISPLAY_ON               0xA5
#define SSD1306_CMD_SET_NORMAL_DISPLAY              0xA6
#define SSD1306_CMD_SET_INVERSE_DISPLAY             0xA7
#define SSD1306_CMD_SET_MULTIPLEX_RATIO             0xA8
#define SSD1306_CMD_SET_DISPLAY_ON                  0xAF
#define SSD1306_CMD_SET_DISPLAY_OFF                 0xAE
#define SSD1306_CMD_SET_PAGE_START_ADDRESS(page)    (0xB0 | (page & 0x07))
#define SSD1306_CMD_SET_COM_OUTPUT_SCAN_UP          0xC0
#define SSD1306_CMD_SET_COM_OUTPUT_SCAN_DOWN        0xC8
#define SSD1306_CMD_SET_DISPLAY_OFFSET              0xD3
#define SSD1306_CMD_SET_DISPLAY_CLOCK_DIVIDE_RATIO  0xD5
#define SSD1306_CMD_SET_PRE_CHARGE_PERIOD           0xD9
#define SSD1306_CMD_SET_COM_PINS                    0xDA
#define SSD1306_CMD_SET_VCOMH_DESELECT_LEVEL        0xDB
#define SSD1306_CMD_NOP                             0xE3



#define SSD1306_CMD_SCROLL_H_RIGHT                  0x26
#define SSD1306_CMD_SCROLL_H_LEFT                   0x27
#define SSD1306_CMD_CONTINUOUS_SCROLL_V_AND_H_RIGHT 0x29
#define SSD1306_CMD_CONTINUOUS_SCROLL_V_AND_H_LEFT  0x2A
#define SSD1306_CMD_DEACTIVATE_SCROLL               0x2E
#define SSD1306_CMD_ACTIVATE_SCROLL                 0x2F
#define SSD1306_CMD_SET_VERTICAL_SCROLL_AREA        0xA3


#define ssd1306_reset_clear()    ioport_set_pin_low(SSD1306_RES_PIN)
#define ssd1306_reset_set()      ioport_set_pin_high(SSD1306_RES_PIN)

#define ssd1306_sel_data()       ioport_set_pin_high(SSD1306_DC_PIN)
#define ssd1306_sel_cmd()        ioport_set_pin_low(SSD1306_DC_PIN)


    static inline void ssd1306_write_command(uint8_t command)
    {
    }

    static inline void ssd1306_write_data(uint8_t data)
    {
    }

    static inline uint8_t ssd1306_read_data(void)
    {
        return 0;
    }

    static inline uint8_t ssd1306_get_status(void)
    {
        return 0;
    }



    static inline void ssd1306_hard_reset(void)
    {
    }



    static inline void ssd1306_sleep_enable(void)
    {
        ssd1306_write_command(SSD1306_CMD_SET_DISPLAY_OFF);
    }

    static inline void ssd1306_sleep_disable(void)
    {
        ssd1306_write_command(SSD1306_CMD_SET_DISPLAY_ON);
    }



    static inline void ssd1306_set_page_address(uint8_t address)
    {
        // Make sure that the address is 4 bits (only 8 pages)
        address &= 0x0F;
        ssd1306_write_command(SSD1306_CMD_SET_PAGE_START_ADDRESS(address));
    }

    static inline void ssd1306_set_column_address(uint8_t address)
    {
        // Make sure the address is 7 bits
        address &= 0x7F;
        ssd1306_write_command(SSD1306_CMD_SET_HIGH_COL(address >> 4));
        ssd1306_write_command(SSD1306_CMD_SET_LOW_COL(address & 0x0F));
    }

    static inline void ssd1306_set_display_start_line_address(uint8_t address)
    {
        // Make sure address is 6 bits
        address &= 0x3F;
        ssd1306_write_command(SSD1306_CMD_SET_START_LINE(address));
    }



    static inline void ssd1306_display_on(void)
    {
        ssd1306_write_command(SSD1306_CMD_SET_DISPLAY_ON);
    }

    static inline void ssd1306_display_off(void)
    {
        ssd1306_write_command(SSD1306_CMD_SET_DISPLAY_OFF);
    }

    static inline uint8_t ssd1306_set_contrast(uint8_t contrast)
    {
        ssd1306_write_command(SSD1306_CMD_SET_CONTRAST_CONTROL_FOR_BANK0);
        ssd1306_write_command(contrast);
        return contrast;
    }

    static inline void ssd1306_display_invert_enable(void)
    {
        ssd1306_write_command(SSD1306_CMD_SET_INVERSE_DISPLAY);
    }

    static inline void ssd1306_display_invert_disable(void)
    {
        ssd1306_write_command(SSD1306_CMD_SET_NORMAL_DISPLAY);
    }

    static inline void ssd1306_clear(void)
    {
        uint8_t page = 0;
        uint8_t col = 0;

        for (page = 0; page < 4; ++page)
        {
            ssd1306_set_page_address(page);
            ssd1306_set_column_address(0);
            for (col = 0; col < 128; ++col)
            {
                ssd1306_write_data(0x00);
            }
        }
    }


    void ssd1306_init(void);


    void ssd1306_write_text(const char *string);

    int oled_cmd(int argc, char **argv)
    {
        printf("init i2c\n");
        ssd1306_init();
        return 0;
    }

#ifdef __cplusplus
}
#endif

#endif /* SSD1306_H_INCLUDED */
