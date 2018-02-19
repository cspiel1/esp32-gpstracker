#ifndef GC_POSQUAL_H
#define GC_POSQUAL_H
#include "GattChar.h"
#include <stdint.h>

#define GATTS_CHAR_UUID_Position_Quality    0x2A69

class GC_Position_Quality: public GattChar {

    public:
    GC_Position_Quality();

};

#endif

