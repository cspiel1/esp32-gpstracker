#include "GC_Location_and_Speed.h"
#include "D_ClientCharConf.h"

#include <strings.h>

#define TAG "Gatt"

GC_Location_and_Speed::GC_Location_and_Speed(esp_gatt_if_t gatt_if):
    GattChar({ESP_UUID_LEN_16, {GATTS_CHAR_UUID_Location_and_Speed}},
            gatt_if) {
    bzero(_uuid.uuid.uuid128+2, ESP_UUID_LEN_128-2);
    _property = ESP_GATT_CHAR_PROP_BIT_NOTIFY;
    _descr_val=0x0001u;
    esp_attr_value_t value={2, 2, (uint8_t*)&_descr_val};
    _descriptor = new D_ClientCharConf(this,
            ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, value);
}

void GC_Location_and_Speed::notify(int32_t lat, int32_t lon, uint64_t utc) {
    uint16_t flags=0x004c;

    uint16_t y = (utc & 0xffff000000000000) >> (6*8);
    uint8_t  m = (utc & 0x0000ff0000000000) >> (5*8);
    uint8_t  d = (utc & 0x000000ff00000000) >> (4*8);
    uint8_t hh = (utc & 0x00000000ff000000) >> (3*8);
    uint8_t mi = (utc & 0x0000000000ff0000) >> (2*8);
    uint8_t ss = (utc & 0x000000000000ff00) >> (1*8);

    _value.append_u16(flags);
    _value.append_i32(lat);
    _value.append_i32(lon);
    _value.append(y);
    _value.append(m);
    _value.append(d);
    _value.append(hh);
    _value.append(mi);
    _value.append(ss);
    GattChar::notify();
}
