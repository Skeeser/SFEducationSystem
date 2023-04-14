#ifndef __DEBUG_H
#define __DEBUG_H

//#include <Wire.h>
#include <Arduino.h>

#define USE_DEBUG 1
#if USE_DEBUG
#define Debug(__info)         \
    {                         \
        Serial.begin(115200); \
        Serial.print(__info); \
    }
#else
#define Debug(__info)
#endif

#endif
