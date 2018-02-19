#include "GattChar.h"

#define TAG "Gatt"
#include <esp_log.h>
#include <stdlib.h>
#include <string.h>

GattChar::GattChar(esp_bt_uuid_t uuid) {
    _uuid=uuid;
    _handle=0;
    _property=0;
    _descriptor=0;
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
    clear();
}

void GattData::clear() {
    free(data);
    _memlen=0;
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
    _memlen=len;
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
    append(&v, sizeof(v));
}

void GattData::append_u16(uint16_t v) {
    append((uint8_t*)&v, sizeof(v));
}

void GattData::append_i32(int32_t v) {
    append((uint8_t*)&v, sizeof(v));
}
