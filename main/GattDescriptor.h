#ifndef GATTDESCR_H
#define GATTDESCR_H
#include <stdint.h>
#include <esp_gatt_defs.h>

class GattChar;
class GattDescriptor {
    uint16_t _handle;

    public:
    GattDescriptor(GattChar* characteristic, esp_bt_uuid_t uuid,
	    esp_gatt_perm_t permissions, esp_attr_value_t value);

    GattChar* _characteristic;
    esp_bt_uuid_t _uuid;
    esp_gatt_perm_t _permissions;
    esp_attr_value_t _value;

    void set_handle(uint16_t h);
    uint16_t handle();
};

#endif
