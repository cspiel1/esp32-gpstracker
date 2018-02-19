#include "GC_Position_Quality.h"
#include <strings.h>

#define TAG "Gatt"

GC_Position_Quality::GC_Position_Quality():
    GattChar({ESP_UUID_LEN_16, {GATTS_CHAR_UUID_Position_Quality}}) {
    bzero(_uuid.uuid.uuid128+2, ESP_UUID_LEN_128-2);
    _property = ESP_GATT_CHAR_PROP_BIT_READ;

    uint16_t flags=0x0003;
    uint8_t s=3;
    uint8_t v=5;
    _value.append(flags);
    _value.append(s);
    _value.append(v);
}

