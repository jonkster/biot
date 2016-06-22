#include <stdio.h>
#include <inttypes.h>

#include <xtimer.h>
#include <periph/adc.h>
//#include <periph/gpio.h>

#define ADC_IN_USE                  ADC_0
//#define ADC_IN_USE                  0
#define ADC_CHANNEL_USE             2
#define RES                         ADC_RES_12BIT

//#define DELAY           (100 * 1000U)

int getVoltage(void)
{
    if (adc_init_local(ADC_IN_USE, RES) != 0)
    {
        puts("initialisation... [failed]");
        return 0;
    }

    int value = adc_sample_local(ADC_IN_USE, ADC_CHANNEL_USE);
    return value/33;
}

int adc_cmd(int argc, char **argv)
{
    int v = getVoltage();
    printf("v=%d mV\n", v);
    return 0;
}

