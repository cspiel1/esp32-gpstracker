#ifndef GC_LOCATIONANDSPEED_H
#define GC_LOCATIONANDSPEED_H
#include "GattChar.h"
#include <stdint.h>
#include "esp_bt_defs.h"

#define GATTS_CHAR_UUID_Location_and_Speed  0x2A67

class GC_Location_and_Speed: public GattChar {

    uint16_t _descr_val;
    public:
    GC_Location_and_Speed();
};

#endif

