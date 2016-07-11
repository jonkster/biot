#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "biotIdentify.h"
#include <xtimer.h>
#include "board.h"
#if (defined NOOLED)
    #define LED1_OFF do {} while (0)
    #define LED1_ON do {} while (0)
    #define LED_RGB_OFF do {} while (0)
    #define LED_RGB_R_ON do {} while (0)
    #define LED_RGB_G_ON do {} while (0)
    #define LED_RGB_B_ON do {} while (0)
#else
    #include "../ssd1306/ssd1306.h"
#endif


void identifyYourself(void)
{
    #if (!defined NOOLED)
    oledPrint(1, "Here I AM!!");
    #endif
    for (uint8_t i = 0; i < 3; i++)
    {
        for (uint8_t j = 0; j < 10; j++)
        {
            LED0_OFF;
            xtimer_usleep(50000);
            LED0_ON;
            xtimer_usleep(50000);
        }
        xtimer_usleep(1000000);
    }
}
