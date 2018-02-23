#include "GC_LnFeature.h"
#include <strings.h>

#define TAG "Gatt"

GC_LnFeature::GC_LnFeature(esp_gatt_if_t gatt_if):
    GattChar({ESP_UUID_LEN_16, {GATTS_CHAR_UUID_LN_Feature}},
            gatt_if) {
    bzero(_uuid.uuid.uuid128+2, ESP_UUID_LEN_128-2);
    _property = ESP_GATT_CHAR_PROP_BIT_READ;

    uint32_t flags=0x0000004c;
    _value.append(flags);
}

