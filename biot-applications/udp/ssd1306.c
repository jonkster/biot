#include "ssd1306.h"
#include "font.h"

#define I2CDEV 0
#define I2CSPEED 0
static void ssd1306_interface_init(void)
{
    printf("%d %d\n", I2CDEV, I2CSPEED);
    int status = i2c_init_master(I2CDEV, I2CSPEED);
    if (status != 0)
    {
        printf("FAILED TO INIT I2C: %d\n", status);
    }
    else
    {
        puts("I2C OK");
    }
}

void ssd1306_init(void)
{
    // Do a hard reset of the OLED display controller
    ssd1306_hard_reset();

    // Initialize the interface
    ssd1306_interface_init();

    // 1/32 Duty (0x0F~0x3F)
    ssd1306_write_command(SSD1306_CMD_SET_MULTIPLEX_RATIO);
    ssd1306_write_command(0x1F);

    // Shift Mapping RAM Counter (0x00~0x3F)
    ssd1306_write_command(SSD1306_CMD_SET_DISPLAY_OFFSET);
    ssd1306_write_command(0x00);

    // Set Mapping RAM Display Start Line (0x00~0x3F)
    ssd1306_write_command(SSD1306_CMD_SET_START_LINE(0x00));

    // Set Column Address 0 Mapped to SEG0
    ssd1306_write_command(SSD1306_CMD_SET_SEGMENT_RE_MAP_COL127_SEG0);

    // Set COM/Row Scan Scan from COM63 to 0
    ssd1306_write_command(SSD1306_CMD_SET_COM_OUTPUT_SCAN_DOWN);

    // Set COM Pins hardware configuration
    ssd1306_write_command(SSD1306_CMD_SET_COM_PINS);
    ssd1306_write_command(0x02);

    ssd1306_set_contrast(0x8F);

    // Disable Entire display On
    ssd1306_write_command(SSD1306_CMD_ENTIRE_DISPLAY_AND_GDDRAM_ON);

    ssd1306_display_invert_disable();

    // Set Display Clock Divide Ratio / Oscillator Frequency (Default => 0x80)
    ssd1306_write_command(SSD1306_CMD_SET_DISPLAY_CLOCK_DIVIDE_RATIO);
    ssd1306_write_command(0x80);

    // Enable charge pump regulator
    ssd1306_write_command(SSD1306_CMD_SET_CHARGE_PUMP_SETTING);
    ssd1306_write_command(0x14);

    // Set VCOMH Deselect Level
    ssd1306_write_command(SSD1306_CMD_SET_VCOMH_DESELECT_LEVEL);
    ssd1306_write_command(0x40); // Default => 0x20 (0.77*VCC)

    // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    ssd1306_write_command(SSD1306_CMD_SET_PRE_CHARGE_PERIOD);
    ssd1306_write_command(0xF1);

    ssd1306_display_on();
}

void ssd1306_write_text(const char *string)
{
    uint8_t *char_ptr;
    uint8_t i;

    while (*string != 0) {
	char_ptr = font_table[(*string - 32) & 0x7F];
	for (i = 1; i <= char_ptr[0]; i++) {
	    ssd1306_write_data(char_ptr[i]);
	}
	ssd1306_write_data(0x00);
	string++;
    }
}
