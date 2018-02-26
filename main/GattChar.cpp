#include "GattChar.h"

#define TAG "Gatt"
#include <esp_log.h>
#include <esp_gatt_defs.h>
#include <esp_gatts_api.h>
#include <stdlib.h>
#include <string.h>

GattChar::GattChar(esp_bt_uuid_t uuid, esp_gatt_if_t gatt_if) {
    _uuid=uuid;
    _handle=0;
    _property=0;
    _descriptor=0;
    _conn_id=0;
    _gatt_if=gatt_if;
    _connected=false;
}

GattDescriptor* GattChar::descriptor() {
    return _descriptor;
}

GattData* GattChar::value() {
    return &_value;
}

GattData::GattData(): data(0), len(0) {
    _memlen=20;
    data= (uint8_t*) malloc(_memlen);
}

GattData::~GattData() {
    free(data);
    _memlen=0;
    len=0;
}

void GattData::clear() {
    len=0;
}

void GattData::store(uint8_t* data, int len) {
    if (this->len > _memlen) {
        _memlen=len+10;
        this->data=(uint8_t*) realloc(data, _memlen);
    }
    if (!this->data) {
        ESP_LOGE(TAG, "ERR - alloc error.");
        return;
    }
    memcpy(this->data, data, len);
    this->len=len;
}

void GattData::append(uint8_t* data, int len) {
    int w=this->len;
    this->len+=len;
    if (this->len > _memlen) {
        _memlen=this->len+10;
        this->data=(uint8_t*) realloc(this->data, _memlen);
    }
    memcpy(this->data+w, data, len);
}

void GattData::append(uint8_t v) {
    append(&v, 1);
}

void GattData::append_u16(uint16_t v) {
    append(uint8_t(v));
    append(uint8_t(v >> 8));
}

void GattData::append_i32(int32_t v) {
    append(uint8_t(v));
    append(uint8_t(v >> 8));
    append(uint8_t(v >> 16));
    append(uint8_t(v >> 24));
}

void GattChar::set_conn_id(uint16_t id) {
    _conn_id=id;
    _connected=true;
}

void GattChar::disconnected() {
    _connected=false;
}

void GattChar::notify() {
    if (!_connected) return;
    if (_gatt_if<1) {
        ESP_LOGW(TAG, "_gatt_if=%d is out of range.", _gatt_if);
        return;
    }

    GattData* d=value();
    ESP_LOGI(TAG, "%s notify data %d bytes", __PRETTY_FUNCTION__, d->len)
//    uint8_t notify_data[15];
//    for (int i = 0; i < (uint8_t) sizeof(notify_data); ++i) {
//        notify_data[i] = i % 0xff;
//    }
//    esp_ble_gatts_send_indicate(_gatt_if, _conn_id, _handle,
//            sizeof(notify_data), notify_data, false);
    esp_ble_gatts_send_indicate(_gatt_if, _conn_id, _handle,
            d->len, d->data, false);
}
