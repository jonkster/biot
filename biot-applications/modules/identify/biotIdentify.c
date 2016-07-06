/*
 * Copyright (C) 2015 Cenk Gündoğan <cnkgndgn@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @brief       Introductory example for the IEEE WF-IOT 2015 Tutorial
 * @author      Cenk Gündoğan <cnkgndgn@gmail.com>
 */

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
            LED_RGB_G_ON;
            LED0_OFF;
            xtimer_usleep(50000);
            LED_RGB_OFF;
            LED0_ON;
            xtimer_usleep(50000);
        }
        xtimer_usleep(1000000);
    }
}
