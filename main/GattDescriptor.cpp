#include "GattDescriptor.h"

GattDescriptor::GattDescriptor(GattChar* characteristic, esp_bt_uuid_t uuid,
		esp_gatt_perm_t permissions, esp_attr_value_t value):
	_characteristic(characteristic), _uuid(uuid), _permissions(permissions),
	_value(value) {
}

void GattDescriptor::set_handle(uint16_t h) {
	_handle=h;
}

uint16_t GattDescriptor::handle() {
	return _handle;
}

