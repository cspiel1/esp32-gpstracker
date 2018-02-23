#ifndef GATTCHAR_H
#define GATTCHAR_H
#include "GattDescriptor.h"
#include <stdint.h>
#include "esp_bt_defs.h"

class GattData {
    private:
    int _memlen;

    public:
    uint8_t *data;
    int len;
    GattData();
    ~GattData();
    void clear();

    void store(uint8_t* data, int len);
    void append(uint8_t* data, int len);

    void append(uint8_t v);
    void append_u16(uint16_t v);
    void append_i32(int32_t v);
};

class GattChar {

    protected:
    GattDescriptor* _descriptor;
    GattData _value;
    uint16_t _conn_id;
    bool _connected;
    esp_gatt_if_t _gatt_if;

    public:
    GattChar(esp_bt_uuid_t uuid, esp_gatt_if_t gatt_if);

    uint16_t _handle;
    esp_bt_uuid_t _uuid;
    esp_gatt_char_prop_t _property;
    GattDescriptor* descriptor();

    GattData* value();

    void set_conn_id(uint16_t id);
    void disconnected();
    void notify();
};

#endif

