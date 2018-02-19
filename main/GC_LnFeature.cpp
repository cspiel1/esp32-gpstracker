#include "GC_LnFeature.h"
#include <strings.h>

#define TAG "Gatt"

GC_LnFeature::GC_LnFeature():
    GattChar({ESP_UUID_LEN_16, {GATTS_CHAR_UUID_LN_Feature}}) {
    bzero(_uuid.uuid.uuid128+2, ESP_UUID_LEN_128-2);
    _property = ESP_GATT_CHAR_PROP_BIT_READ;

    uint32_t flags=0x0000004c;
    _value.append(flags);
}

