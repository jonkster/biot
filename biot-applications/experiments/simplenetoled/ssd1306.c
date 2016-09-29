#include <thread.h>
#include "ssd1306.h"
#include "font.h"

static int spi_dev = -1;
static int spi_mode = -1;
static int spi_speed = -1;
static gpio_t spi_cs = -1;
static gpio_t oled_dc = -1;
char displayBuffer[BUFFER_SIZE];
bool displayBufferReady = false;
static mutex_t mutex = MUTEX_INIT;

void delayUsec(uint16_t us)
{
    thread_yield();
    xtimer_usleep(us);
}

static int setupPins(void)
{

    oled_dc = GPIO_PIN(SSD1306_DC_PORT, SSD1306_DC_PIN);
    int res = gpio_init(oled_dc, GPIO_OUT);
    if (res < 0)
    {
        printf("gpioInit: error init GPIO%ld as DC line (code %i)\n", (long)oled_dc, res);
        return 1;
    }
    // bring low to enable command mode
    gpio_clear(oled_dc);

    oled_reset = GPIO_PIN(SSD1306_RESET_PORT, SSD1306_RESET_PIN);
    res = gpio_init(oled_reset, GPIO_OUT);
    if (res < 0)
    {
        printf("gpioInit: error init GPIO%ld as RESET line (code %i)\n", (long)oled_reset, res);
        return 1;
    }
    // bring high to enable run mode
    gpio_set(oled_reset);

    return 0;
}

void oledClearAll(void)
{
    uint8_t page_address;
    uint8_t column_address;
    for (page_address = 0; page_address < SSD1306_PAGECOUNT; page_address++)
    {
        oledSetPageAddress(page_address);
        oledSetColumnAddress(0);
        for (column_address = 0; column_address <  SSD1306_COLCOUNT; column_address++)
        {
            oledWriteData(0x00);
        }
    }
    oledSetPageAddress(0);
    oledSetColumnAddress(0);
    oledWriteText("OLED Test:");
}

static int oledInterfaceInit(void)
{
    spi_dev = SPI_1;
    spi_mode = SPI_CONF_FIRST_RISING;
    //spi_speed = SPI_SPEED_400KHZ;
    spi_speed = SPI_SPEED_100KHZ;
    //spi_speed = SPI_SPEED_1MHZ;


    spi_acquire(spi_dev);
    int res = spi_init_master(spi_dev, spi_mode, spi_speed);
    spi_release(spi_dev);

    if (res < 0)
    {
        printf("spi_init_master: error init SPI_%i device (code %i)\n", spi_dev, res);
        return 1;
    }

    // set up chip select stuff
    spi_cs = GPIO_PIN(SSD1306_CS_PORT, SSD1306_CS_PIN);
    res = gpio_init(spi_cs, GPIO_OUT);
    if (res < 0)
    {
        printf("gpio_init: error init GPIO_%ld as CS line (code %i)\n", (long)spi_cs, res);
        return 1;
    }
    oledUnselectChip();

    //printf("SPI_%i initialized as master, cs: GPIO_%ld, mode: %i, speed: %i\n", spi_dev, (long)spi_cs, spi_mode, spi_speed);
    return 0;
}

int oledDeviceInit(void)
{
    int status = setupPins();
    if (status != 0)
    {
        return status;
    }

    // Do a hard reset of the OLED display controller
    oledHardReset();

    // Initialize the interface
    status = oledInterfaceInit();
    if (status != 0)
    {
        return status;
    }

    puts("setting up display\n");

    // 1/32 Duty (0x0F~0x3F)
    status = oledWriteCommand(SSD1306_CMD_SET_MULTIPLEX_RATIO);
    if (status != 0)
    {
        puts("fail setting up display\n");
        return status;
    }
    puts("OK setting up display\n");
    status = oledWriteCommand(0x1F);
    if (status != 0)
    {
        return status;
    }

    // Shift Mapping RAM Counter (0x00~0x3F)
    status = oledWriteCommand(SSD1306_CMD_SET_DISPLAY_OFFSET);
    if (status != 0)
    {
        return status;
    }
    status = oledWriteCommand(0x00);
    if (status != 0)
    {
        return status;
    }

    // Set Mapping RAM Display Start Line (0x00~0x3F)
    status = oledWriteCommand(SSD1306_CMD_SET_START_LINE(0x00));
    if (status != 0)
    {
        return status;
    }

    // Set Column Address 0 Mapped to SEG0
    status = oledWriteCommand(SSD1306_CMD_SET_SEGMENT_RE_MAP_COL127_SEG0);
    if (status != 0)
    {
        return status;
    }

    // Set COM/Row Scan Scan from COM63 to 0
    status = oledWriteCommand(SSD1306_CMD_SET_COM_OUTPUT_SCAN_DOWN);
    if (status != 0)
    {
        return status;
    }

    // Set COM Pins hardware configuration
    status = oledWriteCommand(SSD1306_CMD_SET_COM_PINS);
    if (status != 0)
    {
        return status;
    }
    status = oledWriteCommand(0x02);
    if (status != 0)
    {
        return status;
    }

    oledSetContrast(SSD1306_DISPLAY_CONTRAST_MAX);

    // Disable Entire display On
    status = oledWriteCommand(SSD1306_CMD_ENTIRE_DISPLAY_AND_GDDRAM_ON);
    if (status != 0)
    {
        return status;
    }

    oledDisplayInvertDisable();

    // Set Display Clock Divide Ratio / Oscillator Frequency (Default => 0x80)
    status = oledWriteCommand(SSD1306_CMD_SET_DISPLAY_CLOCK_DIVIDE_RATIO);
    if (status != 0)
    {
        return status;
    }
    status = oledWriteCommand(0x80);
    if (status != 0)
    {
        return status;
    }

    // Enable charge pump regulator
    status = oledWriteCommand(SSD1306_CMD_SET_CHARGE_PUMP_SETTING);
    if (status != 0)
    {
        return status;
    }
    status = oledWriteCommand(0x14);
    if (status != 0)
    {
        return status;
    }

    // Set VCOMH Deselect Level
    status = oledWriteCommand(SSD1306_CMD_SET_VCOMH_DESELECT_LEVEL);
    if (status != 0)
    {
        return status;
    }
    status = oledWriteCommand(0x40); // Default => 0x20 (0.77*VCC)
    if (status != 0)
    {
        return status;
    }

    // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    status = oledWriteCommand(SSD1306_CMD_SET_PRE_CHARGE_PERIOD);
    if (status != 0)
    {
        return status;
    }
    status = oledWriteCommand(0xF1);
    if (status != 0)
    {
        return status;
    }
    
    //oledDisplayInvertEnable();
    oledClearAll();

    oledDisplayOn();
    puts("ready to use...\n");

    return 0;
}

int oledWriteCommand(uint8_t command)
{
    oledSetCommandMode()
    mutex_lock(&mutex);
    oledSelectChip();
    spi_acquire(spi_dev);
    int res = spi_transfer_byte(spi_dev, (char) command, (char*) NULL);
    spi_release(spi_dev);
    delayUsec(2*SSD1306_LATENCY); // At least 3us
    oledUnselectChip();
    mutex_unlock(&mutex);

    /* look at the results */
    if (res < 0) {
        printf("error: transfer data to slave (code: %i)\n", res);
        return 1;
    }
    else {
        //printf("command %i bytes:%i:\n", command, res);
        return 0;
    }
    return 0;
}

int oledWriteData(uint8_t command)
{

    oledSetDataMode()
    mutex_lock(&mutex);
    oledSelectChip();
    spi_acquire(spi_dev);
    int res = spi_transfer_byte(spi_dev, (char) command, (char*) NULL);
    spi_release(spi_dev);
    delayUsec(2*SSD1306_LATENCY); // At least 3us
    oledUnselectChip();
    mutex_unlock(&mutex);

    /* look at the results */
    if (res < 0) {
        printf("error: transfer data to slave (code: %i)\n", res);
        return 1;
    }
    else {
        //printf("data %i -> %s bytes:%i:\n", command, buffer, res);
        return 0;
    }
    return 0;
}

void oledWriteText(const char *string)
{
    uint8_t *char_ptr;
    uint8_t i;

    while (*string != 0) {
	char_ptr = font_table[(*string - 32) & 0x7F];
	for (i = 1; i <= char_ptr[0]; i++) {
	    oledWriteData(char_ptr[i]);
	}
	oledWriteData(0x00);
	string++;
    }
}

static void *display_loop(void)
{
    int status = oledDeviceInit();
    if (status == 0)
    {
        oledDisplayOn();
        while(1)
        {
            if (displayBufferReady)
            {
                puts(displayBuffer);
                oledWriteText(displayBuffer);
                memset(&displayBuffer, 0, BUFFER_SIZE);
                displayBufferReady = false;
            }
            else
            {
                delayUsec(SSD1306_LATENCY);
                thread_yield();
            }
        }
    }
    else
    {
        puts("FAILED!");
    }
    return 0;
}

void *display_handler(char *arg)
{
    (void) arg;
    display_loop();
    /* never reached */
    return NULL;
}

int oledCmd(int argc, char **argv)
{
    if (! displayBufferReady)
    {
        if (argc < 2)
        {
            displayBuffer[0] = '?';
        }
        else
        {
            for (int i = 0; i < strlen(argv[1]); i++)
            {
                displayBuffer[i] = argv[1][i];
            }
        }
        displayBufferReady = true;
        return 0;
    }
    else
    {
        return 1;
    }
}
