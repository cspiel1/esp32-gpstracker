#include "D_ClientCharConf.h"

#define TAG "Gatt"

D_ClientCharConf::D_ClientCharConf(GattChar* characteristic,
        esp_gatt_perm_t permissions, esp_attr_value_t value): GattDescriptor(
        characteristic, {ESP_UUID_LEN_16, {ESP_GATT_UUID_CHAR_CLIENT_CONFIG}},
        permissions,
        value) {
}

