#ifndef D_CLIENTCHARCONF_H
#define D_CLIENTCHARCONF_H

#include "GattDescriptor.h"
#include <stdint.h>
#include "esp_bt_defs.h"

class D_ClientCharConf: public GattDescriptor {

    public:
    D_ClientCharConf(GattChar* characteristic, esp_gatt_perm_t permissions,
            esp_attr_value_t value);

};

#endif

