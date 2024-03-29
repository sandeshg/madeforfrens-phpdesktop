// Code taken from here: http://stackoverflow.com/a/323302/623622

#pragma once

// #include <random> - TR1

#include "defines.h"
#include <time.h>
#include <windows.h>

unsigned long srand2_mix(unsigned long a, unsigned long b, unsigned long c)
{
    a=a-b;  a=a-c;  a=a^(c >> 13);
    b=b-c;  b=b-a;  b=b^(a << 8);
    c=c-a;  c=c-b;  c=c^(b >> 13);
    a=a-b;  a=a-c;  a=a^(c >> 12);
    b=b-c;  b=b-a;  b=b^(a << 16);
    c=c-a;  c=c-b;  c=c^(b >> 5);
    a=a-b;  a=a-c;  a=a^(c >> 3);
    b=b-c;  b=b-a;  b=b^(a << 10);
    c=c-a;  c=c-b;  c=c^(b >> 15);
    return c;
}
void srand2() {
    unsigned long seed = srand2_mix(
            clock(),
            (unsigned long)time(NULL),
            GetProcessId(GetModuleHandle(NULL)));
    srand(seed);
}
