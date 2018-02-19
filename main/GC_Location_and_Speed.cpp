#include "GC_Location_and_Speed.h"
#include "D_ClientCharConf.h"

#include <strings.h>

#define TAG "Gatt"

GC_Location_and_Speed::GC_Location_and_Speed():
    GattChar({ESP_UUID_LEN_16, {GATTS_CHAR_UUID_Location_and_Speed}}) {
    bzero(_uuid.uuid.uuid128+2, ESP_UUID_LEN_128-2);
    _property = ESP_GATT_CHAR_PROP_BIT_NOTIFY;
    _descr_val=0x0001u;
    esp_attr_value_t value={2, 2, (uint8_t*)&_descr_val};
    _descriptor = new D_ClientCharConf(this,
            ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, value);

    uint16_t flags=0x004c;
    int32_t lat=478345530;
    int32_t lon=133057300;

    uint16_t y=2018;
    uint8_t m=2;
    uint8_t d=16;
    uint8_t hh=17;
    uint8_t mi=20;
    uint8_t ss=10;

    _value.append_u16(flags);
    _value.append_i32(lat);
    _value.append_i32(lon);
    _value.append(y);
    _value.append(m);
    _value.append(d);
    _value.append(hh);
    _value.append(mi);
    _value.append(ss);
}
