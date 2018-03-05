#ifndef GATTSERVICE_H
#define GATTSERVICE_H
#include "GattChar.h"

#include <esp_gatts_api.h>
#include <esp_gap_ble_api.h>
#include <stdint.h>
#include <list>

class GattService {
	static void gatts_event_handler(
			esp_gatts_cb_event_t event,
			esp_gatt_if_t gatt_if,
			esp_ble_gatts_cb_param_t *param);

	static void gap_event_handler(
			esp_gap_ble_cb_event_t event,
			esp_ble_gap_cb_param_t *param);

	void process_gatt_event(
			esp_gatts_cb_event_t event,
			esp_gatt_if_t gatt_if,
			esp_ble_gatts_cb_param_t *param);

	void process_gap_event(
			esp_gap_ble_cb_event_t event,
			esp_ble_gap_cb_param_t *param);

    esp_gatt_srvc_id_t _service_id;
    uint16_t _app_id;
    uint16_t _conn_id;
    uint16_t _service_handle;

	uint8_t _adv_config_done;

	virtual void makeGattChars()=0;
	GattChar* find_gattchar(esp_bt_uuid_t* uuid);
	GattChar* find_gattchar(uint16_t attr_handle);
	GattDescriptor* find_gattDescriptor(uint16_t attr_handle);

	static GattService* _instance;

	uint8_t _service_uuid[16];
	esp_ble_adv_data_t adv_data;
	esp_ble_adv_data_t scan_rsp_data;

	GattData _writebuf;
	void write_event_env(esp_gatt_if_t gatt_if,
			esp_ble_gatts_cb_param_t *param);
	void exec_write_event_env(esp_ble_gatts_cb_param_t *param);

	const char* _name;

	protected:
    uint16_t _gatt_if;
	GattService(esp_bt_uuid_t uuid, const char* name);
	std::list<GattChar*> _char_list;

	public:
	static void setInstance(GattService* instance);

	void setup();
};

#endif

