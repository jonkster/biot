/*
 * Copyright (C) 2016 Jon Kelly
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <thread.h>
#include <xtimer.h>

#include "../modules/dataCache/dataCache.h"

int main(void)
{
    puts("Biotz Root Node\n");
    testHash();
    return 0;
}
