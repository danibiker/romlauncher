#pragma once

#include <stdio.h>
#include <string.h>
#include "utils/compat/libs.h"

void decodeError(int r){
    char buff[100];
    strerror_s(buff, 100, r);
    printf("str_trim_left.error: %d %s\n", r, buff);
}