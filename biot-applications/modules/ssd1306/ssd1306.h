#ifndef SSD1306_H_INCLUDED
#define SSD1306_H_INCLUDED

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <thread.h>
#include <xtimer.h>

#include "periph/spi.h"
#include "periph/gpio.h"
#include "mutex.h"


#ifdef __cplusplus
extern "C" {
#endif

#define     SSD1306_CS_PORT   PB
#define     SSD1306_CS_PIN    22 
#define     SSD1306_DATA_PORT PB
#define     SSD1306_DATA_PIN  02
#define     SSD1306_CLK_PORT  PB
#define     SSD1306_CLK_PIN   03

#define     SSD1306_DC_PORT      PA
#define     SSD1306_DC_PIN       22   // PA22
#define     SSD1306_RESET_PORT   PA
#define     SSD1306_RESET_PIN    23   // PA23

#define     SSD1306_DISPLAY_CONTRAST_MAX   40
#define     SSD1306_DISPLAY_CONTRAST_MIN   30
#define     SSD1306_PAGECOUNT               4 
#define     SSD1306_COLCOUNT              128   

#define SSD1306_LATENCY  1



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


#define oledUnselectChip()          gpio_set(spi_cs);
#define oledSelectChip()            gpio_clear(spi_cs);

#define oledSetDataMode()           gpio_set(oled_dc);
#define oledSetCommandMode()        gpio_clear(oled_dc);

#define BUFFER_SIZE 30

    static gpio_t oled_reset = -1;

    void oledClearAll(void);
    void oledClearLine(uint8_t lineNo);
    thread_task_func_t display_handler(char *arg);
    void oledWriteText(const char *string);
    int oledPrint(uint8_t lineNo, char* text);
    int oledWriteCommand(uint8_t command);
    int oledWriteData(uint8_t data);
    void delayUsec(uint16_t us);

    static inline uint8_t oledReadData(void)
    {
        puts("WOAH! read data fail!");
        return 0;
    }

    static inline uint8_t oledGetStatus(void)
    {
        puts("WOAH! get status  fail!");
        return 0;
    }



    static inline void oledHardReset(void)
    {
	gpio_set(oled_reset);
	delayUsec(SSD1306_LATENCY);
	gpio_clear(oled_reset);
	delayUsec(SSD1306_LATENCY);
	gpio_set(oled_reset);
	delayUsec(SSD1306_LATENCY);
    }



    static inline void oledSleepEnable(void)
    {
        oledWriteCommand(SSD1306_CMD_SET_DISPLAY_OFF);
    }

    static inline void oledSleepDisable(void)
    {
        oledWriteCommand(SSD1306_CMD_SET_DISPLAY_ON);
    }



    static inline void oledSetPageAddress(uint8_t address)
    {
        // Make sure that the address is 4 bits (only 8 pages)
        address &= 0x0F;
        oledWriteCommand(SSD1306_CMD_SET_PAGE_START_ADDRESS(address));
    }

    static inline void oledSetColumnAddress(uint8_t address)
    {
        // Make sure the address is 7 bits
        address &= 0x7F;
        oledWriteCommand(SSD1306_CMD_SET_HIGH_COL(address >> 4));
        oledWriteCommand(SSD1306_CMD_SET_LOW_COL(address & 0x0F));
    }

    static inline void oledSetDisplayStartLineAddress(uint8_t address)
    {
        // Make sure address is 6 bits
        address &= 0x3F;
        oledWriteCommand(SSD1306_CMD_SET_START_LINE(address));
    }



    static inline void oledDisplayOn(void)
    {
        oledWriteCommand(SSD1306_CMD_SET_DISPLAY_ON);
    }

    static inline void oledDisplayOff(void)
    {
        oledWriteCommand(SSD1306_CMD_SET_DISPLAY_OFF);
    }

    static inline uint8_t oledSetContrast(uint8_t contrast)
    {
        oledWriteCommand(SSD1306_CMD_SET_CONTRAST_CONTROL_FOR_BANK0);
        oledWriteCommand(contrast);
        return contrast;
    }

    static inline void oledDisplayInvertEnable(void)
    {
        oledWriteCommand(SSD1306_CMD_SET_INVERSE_DISPLAY);
    }

    static inline void oledDisplayInvertDisable(void)
    {
        oledWriteCommand(SSD1306_CMD_SET_NORMAL_DISPLAY);
    }

#ifdef __cplusplus
}
#endif

#endif /* SSD1306_H_INCLUDED */
