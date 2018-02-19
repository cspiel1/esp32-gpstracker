#ifndef GC_LNFEATURE_H
#define GC_LNFEATURE_H
#include "GattChar.h"
#include <stdint.h>
#include "esp_bt_defs.h"

#define GATTS_CHAR_UUID_LN_Feature          0x2A6A

class GC_LnFeature: public GattChar {

    public:
    GC_LnFeature();

};

#endif

