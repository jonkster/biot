#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "biotIdentify.h"
#include <xtimer.h>
#include "board.h"

void identifyYourself(char *name)
{
    printf("%s\n", name);
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
