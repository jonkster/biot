#include "ssd1306.h"
#include "font.h"


static int setupPins(void)
{

    oled_dc = GPIO_PIN(SSD1306_DC_PORT, SSD1306_DC_PIN);
    int res = gpio_init(oled_dc, GPIO_OUT);
    if (res < 0)
    {
        printf("gpio_init: error initializing GPIO_%ld as DC line (code %i)\n", (long)oled_dc, res);
        return 1;
    }
    // bring low to enable command mode
    gpio_clear(oled_dc);

    oled_reset = GPIO_PIN(SSD1306_RESET_PORT, SSD1306_RESET_PIN);
    res = gpio_init(oled_reset, GPIO_OUT);
    if (res < 0)
    {
        printf("gpio_init: error initializing GPIO_%ld as RESET line (code %i)\n", (long)oled_reset, res);
        return 1;
    }
    // bring high to enable run mode
    gpio_set(oled_reset);

    gpio_t spi_clk = GPIO_PIN(SSD1306_CLK_PORT, SSD1306_CLK_PIN);
    res = gpio_init(spi_clk, GPIO_OUT);
    if (res < 0)
    {
        printf("gpio_init: error initializing GPIO_%ld (code %i)\n", (long)spi_clk, res);
        return 1;
    }
    gpio_t spi_data = GPIO_PIN(SSD1306_DATA_PORT, SSD1306_DATA_PIN);
    res = gpio_init(spi_data, GPIO_OUT);
    if (res < 0)
    {
        printf("gpio_init: error initializing GPIO_%ld (code %i)\n", (long)spi_data, res);
        return 1;
    }

    return 0;
}

static int ssd1306_interface_init(void)
{
    spi_dev = SPI_1;
    spi_mode = SPI_CONF_FIRST_RISING;
    //spi_speed = SPI_SPEED_400KHZ;
    spi_speed = SPI_SPEED_100KHZ;


    spi_acquire(spi_dev);
    int res = spi_init_master(spi_dev, spi_mode, spi_speed);
    spi_release(spi_dev);

    if (res < 0)
    {
        printf("spi_init_master: error initializing SPI_%i device (code %i)\n", spi_dev, res);
        return 1;
    }

    // set up chip select stuff
    spi_cs = GPIO_PIN(SSD1306_CS_PORT, SSD1306_CS_PIN);
    res = gpio_init(spi_cs, GPIO_OUT);
    if (res < 0)
    {
        printf("gpio_init: error initializing GPIO_%ld as CS line (code %i)\n", (long)spi_cs, res);
        return 1;
    }
    ssd1306_unselectChip();

    printf("SPI_%i successfully initialized as master, cs: GPIO_%ld, mode: %i, speed: %i\n", spi_dev, (long)spi_cs, spi_mode, spi_speed);
    return 0;
}

int ssd1306_init(void)
{
    mutex_lock(&ssd1306mtx);
    int status = setupPins();
    if (status != 0)
    {
        mutex_unlock(&ssd1306mtx);
        return status;
    }

    // Do a hard reset of the OLED display controller
    ssd1306_hard_reset();

    // Initialize the interface
    status = ssd1306_interface_init();
    if (status != 0)
    {
        mutex_unlock(&ssd1306mtx);
        return status;
    }

    printf("now setting up display\n");

    printf("display off...\n");
    ssd1306_display_off();

    // 1/32 Duty (0x0F~0x3F)
    status = ssd1306_write_command(SSD1306_CMD_SET_MULTIPLEX_RATIO);
    if (status != 0)
    {
        mutex_unlock(&ssd1306mtx);
        return status;
    }
    status = ssd1306_write_command(0x1F);
    if (status != 0)
    {
        mutex_unlock(&ssd1306mtx);
        return status;
    }

    // Shift Mapping RAM Counter (0x00~0x3F)
    status = ssd1306_write_command(SSD1306_CMD_SET_DISPLAY_OFFSET);
    if (status != 0)
    {
        mutex_unlock(&ssd1306mtx);
        return status;
    }
    status = ssd1306_write_command(0x00);
    if (status != 0)
    {
        mutex_unlock(&ssd1306mtx);
        return status;
    }

    // Set Mapping RAM Display Start Line (0x00~0x3F)
    status = ssd1306_write_command(SSD1306_CMD_SET_START_LINE(0x00));
    if (status != 0)
    {
        mutex_unlock(&ssd1306mtx);
        return status;
    }

    // Set Column Address 0 Mapped to SEG0
    status = ssd1306_write_command(SSD1306_CMD_SET_SEGMENT_RE_MAP_COL127_SEG0);
    if (status != 0)
    {
        mutex_unlock(&ssd1306mtx);
        return status;
    }

    // Set COM/Row Scan Scan from COM63 to 0
    status = ssd1306_write_command(SSD1306_CMD_SET_COM_OUTPUT_SCAN_DOWN);
    if (status != 0)
    {
        mutex_unlock(&ssd1306mtx);
        return status;
    }

    // Set COM Pins hardware configuration
    status = ssd1306_write_command(SSD1306_CMD_SET_COM_PINS);
    if (status != 0)
    {
        mutex_unlock(&ssd1306mtx);
        return status;
    }
    status = ssd1306_write_command(0x02);
    if (status != 0)
    {
        mutex_unlock(&ssd1306mtx);
        return status;
    }

    ssd1306_set_contrast(0x8F);

    // Disable Entire display On
    status = ssd1306_write_command(SSD1306_CMD_ENTIRE_DISPLAY_AND_GDDRAM_ON);
    if (status != 0)
    {
        mutex_unlock(&ssd1306mtx);
        return status;
    }

    ssd1306_display_invert_disable();

    // Set Display Clock Divide Ratio / Oscillator Frequency (Default => 0x80)
    status = ssd1306_write_command(SSD1306_CMD_SET_DISPLAY_CLOCK_DIVIDE_RATIO);
    if (status != 0)
    {
        mutex_unlock(&ssd1306mtx);
        return status;
    }
    status = ssd1306_write_command(0x80);
    if (status != 0)
    {
        mutex_unlock(&ssd1306mtx);
        return status;
    }

    // Enable charge pump regulator
    status = ssd1306_write_command(SSD1306_CMD_SET_CHARGE_PUMP_SETTING);
    if (status != 0)
    {
        mutex_unlock(&ssd1306mtx);
        return status;
    }
    status = ssd1306_write_command(0x14);
    if (status != 0)
    {
        mutex_unlock(&ssd1306mtx);
        return status;
    }

    // Set VCOMH Deselect Level
    status = ssd1306_write_command(SSD1306_CMD_SET_VCOMH_DESELECT_LEVEL);
    if (status != 0)
    {
        mutex_unlock(&ssd1306mtx);
        return status;
    }
    status = ssd1306_write_command(0x40); // Default => 0x20 (0.77*VCC)
    if (status != 0)
    {
        mutex_unlock(&ssd1306mtx);
        return status;
    }

    // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    status = ssd1306_write_command(SSD1306_CMD_SET_PRE_CHARGE_PERIOD);
    if (status != 0)
    {
        mutex_unlock(&ssd1306mtx);
        return status;
    }
    status = ssd1306_write_command(0xF1);
    if (status != 0)
    {
        mutex_unlock(&ssd1306mtx);
        return status;
    }

    printf("display on...\n");
    ssd1306_display_on();
    printf("should now be ready to use...\n\n");

    mutex_unlock(&ssd1306mtx);
    return 0;
}

int ssd1306_write_command(uint8_t command)
{
    ssd1306_setCmdMode()
    ssd1306_selectChip();
    char buffer[2];

    spi_acquire(spi_dev);
    int res = spi_transfer_byte(spi_dev, (char) command, buffer);
    xtimer_usleep(SSD1306_LATENCY); // At least 3us
    spi_release(spi_dev);
    ssd1306_unselectChip();

    /* look at the results */
    if (res < 0) {
        printf("error: unable to transfer data to slave (code: %i)\n", res);
        return 1;
    }
    else {
        //printf("command %i -> %s bytes:%i:\n", command, buffer, res);
        return 0;
    }
    return 0;
}

int ssd1306_write_data(uint8_t command)
{

    ssd1306_setDataMode()
    ssd1306_selectChip();
    
    char buffer[2];
    spi_acquire(spi_dev);
    int res = spi_transfer_byte(spi_dev, (char) command, buffer);
    xtimer_usleep(SSD1306_LATENCY); // At least 3us
    spi_release(spi_dev);
    ssd1306_unselectChip();

    /* look at the results */
    if (res < 0) {
        printf("error: unable to transfer data to slave (code: %i)\n", res);
        return 1;
    }
    else {
        //printf("data %i -> %s bytes:%i:\n", command, buffer, res);
        return 0;
    }
    return 0;
}

void ssd1306_write_text(const char *string)
{
    uint8_t *char_ptr;
    uint8_t i;

    printf("write: %s\n", string);
    mutex_lock(&ssd1306mtx);
    while (*string != 0) {
	char_ptr = font_table[(*string - 32) & 0x7F];
	for (i = 1; i <= char_ptr[0]; i++) {
	    ssd1306_write_data(char_ptr[i]);
	}
	ssd1306_write_data(0x00);
	string++;
    }
    mutex_unlock(&ssd1306mtx);
    printf("written\n");
}