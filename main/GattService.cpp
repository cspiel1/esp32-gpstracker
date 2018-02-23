#include "GattService.h"

#include <freertos/FreeRTOSConfig.h>
#include <esp_log.h>
#include <esp_err.h>
#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_gatt_common_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAG "Gatt"

#define GATTS_DEVICE_NAME       "CSPIEL_GPSTRACKER"
#define GATTS_SERVICE_UUID      0x00FF
#define GATTS_NUM_HANDLE        8

#define PROFILE_APP_ID 0

#define adv_config_flag      (1 << 0)
#define scan_rsp_config_flag (1 << 1)

static uint8_t service_uuid_def[16] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
    //16bit, [12],[13] is the value
    //32bit, [12],[13],[14],[15] is the value
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00
};

// The length of adv data must be less than 31 bytes
//static uint8_t test_manufacturer[TEST_MANUFACTURER_DATA_LEN] =  {0x12, 0x23, 0x45, 0x56};
//adv data
static esp_ble_adv_data_t adv_data_def = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x20,
    .max_interval = 0x40,
    .appearance = 0x00,
    .manufacturer_len = 0, //TEST_MANUFACTURER_DATA_LEN,
    .p_manufacturer_data =  NULL, //&test_manufacturer[0],
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = 16,
    .p_service_uuid = service_uuid_def,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};
// scan response data
static esp_ble_adv_data_t scan_rsp_data_def = {
    .set_scan_rsp = true,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x20,
    .max_interval = 0x40,
    .appearance = 0x00,
    .manufacturer_len = 0, //TEST_MANUFACTURER_DATA_LEN,
    .p_manufacturer_data =  NULL, //&test_manufacturer[0],
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = 16,
    .p_service_uuid = service_uuid_def,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

static esp_ble_adv_params_t adv_params = {
    .adv_int_min        = 0x20,
    .adv_int_max        = 0x40,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    .peer_addr          = {0,0,0,0,0,0},
    .peer_addr_type     = BLE_ADDR_TYPE_PUBLIC,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy  = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};


void GattService::gap_event_handler(
        esp_gap_ble_cb_event_t event,
        esp_ble_gap_cb_param_t *param) {
    if (!_instance) return;

    _instance->process_gap_event(event, param);
}

void GattService::process_gap_event(esp_gap_ble_cb_event_t event,
        esp_ble_gap_cb_param_t *param) {

    switch (event) {
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        _adv_config_done &= (~adv_config_flag);
        if (!_adv_config_done) {
            ESP_LOGI(TAG, "Starting gap. Line=%d\n", __LINE__);
            esp_ble_gap_start_advertising(&adv_params);
        }
        break;
    case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
        _adv_config_done &= (~scan_rsp_config_flag);
        if (!_adv_config_done) {
            ESP_LOGI(TAG, "Starting gap. Line=%d\n", __LINE__);
            esp_ble_gap_start_advertising(&adv_params);
        }
        break;
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        ESP_LOGI(TAG, "Advertising started. Status %d\n", param->adv_start_cmpl.status);
        //advertising start complete event to indicate advertising start successfully or failed
        if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(TAG, "Advertising start failed\n");
        }
        break;
    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
        if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(TAG, "Advertising stop failed\n");
        }
        else {
            ESP_LOGI(TAG, "Stop adv successfully\n");
        }
        break;
    case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
         ESP_LOGI(TAG, "update connetion params status = %d, min_int = %d, max_int = %d,conn_int = %d,latency = %d, timeout = %d",
                  param->update_conn_params.status,
                  param->update_conn_params.min_int,
                  param->update_conn_params.max_int,
                  param->update_conn_params.conn_int,
                  param->update_conn_params.latency,
                  param->update_conn_params.timeout);
        break;
    default:
        break;
    }
}

void GattService::write_event_env(esp_gatt_if_t gatt_if, esp_ble_gatts_cb_param_t *param){
    esp_gatt_status_t status = ESP_GATT_OK;
    if (param->write.need_rsp){
        if (param->write.is_prep){
            esp_gatt_rsp_t *gatt_rsp = (esp_gatt_rsp_t *)malloc(sizeof(esp_gatt_rsp_t));
            gatt_rsp->attr_value.len = param->write.len;
            gatt_rsp->attr_value.handle = param->write.handle;
            gatt_rsp->attr_value.offset = param->write.offset;
            gatt_rsp->attr_value.auth_req = ESP_GATT_AUTH_REQ_NONE;
            memcpy(gatt_rsp->attr_value.value, param->write.value, param->write.len);
            esp_err_t response_err = esp_ble_gatts_send_response(gatt_if,
                    param->write.conn_id, param->write.trans_id, status, gatt_rsp);
            if (response_err != ESP_OK){
               ESP_LOGE(TAG, "Send response error\n");
            }
            free(gatt_rsp);
            if (status != ESP_GATT_OK){
                return;
            }
            _writebuf.append(param->write.value, param->write.len);

        }else{
            esp_ble_gatts_send_response(gatt_if, param->write.conn_id, param->write.trans_id, status, NULL);
        }
    }
}

void GattService::exec_write_event_env(esp_ble_gatts_cb_param_t *param){
    if (param->exec_write.exec_write_flag == ESP_GATT_PREP_WRITE_EXEC){
        // TODO: Pass to subclass for processing.
        esp_log_buffer_hex(TAG, _writebuf.data, _writebuf.len);
    }else{
        ESP_LOGI(TAG,"ESP_GATT_PREP_WRITE_CANCEL");
    }

    _writebuf.clear();
}

void GattService::gatts_event_handler(
        esp_gatts_cb_event_t event,
        esp_gatt_if_t gatt_if,
        esp_ble_gatts_cb_param_t *param) {

    if (!_instance) return;
    _instance->process_gatt_event(event, gatt_if, param);
}

GattService::GattService(esp_bt_uuid_t uuid) {
    _adv_config_done=0;
    _gatt_if=0;
    // Prepare _service_uuid, adv_data and scan_rsp_data.
    memcpy(_service_uuid, service_uuid_def, sizeof(service_uuid_def));
    int len=uuid.len;
    if (len>4)
        len=4;
    memcpy(_service_uuid+12, &uuid.uuid.uuid16, len);

    memcpy(&adv_data, &adv_data_def, sizeof(esp_ble_adv_data_t));
    adv_data.p_service_uuid=_service_uuid;

    memcpy(&scan_rsp_data, &scan_rsp_data_def, sizeof(esp_ble_adv_data_t));
    scan_rsp_data.p_service_uuid=_service_uuid;
}

GattService* GattService::_instance=0;

void GattService::setInstance(GattService* instance) {
    _instance=instance;
}

void GattService::process_gatt_event(
        esp_gatts_cb_event_t event,
        esp_gatt_if_t gatt_if,
        esp_ble_gatts_cb_param_t *param) {

    if (event == ESP_GATTS_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            _gatt_if = gatt_if;
            makeGattChars();
        } else {
            ESP_LOGI(TAG, "Reg app failed, app_id %04x, status %d\n",
                    param->reg.app_id,
                    param->reg.status);
            return;
        }
    }

    if ((_gatt_if!=ESP_GATT_IF_NONE) && (_gatt_if!=gatt_if))
        return;

    switch (event) {
    case ESP_GATTS_REG_EVT: {
        ESP_LOGI(TAG, "ESP_GATTS_REG_EVT, status %d, app_id %d\n",
                param->reg.status, param->reg.app_id);
        _service_id.is_primary = true;
        _service_id.id.inst_id = 0x00;
        _service_id.id.uuid.len = ESP_UUID_LEN_16;
        _service_id.id.uuid.uuid.uuid16 = GATTS_SERVICE_UUID;

        esp_err_t ret = esp_ble_gap_set_device_name(GATTS_DEVICE_NAME);
        if (ret){
            ESP_LOGE(TAG, "set device name failed, error code = %x", ret);
        }
        //config adv data
        ret = esp_ble_gap_config_adv_data(&adv_data);
        if (ret){
            ESP_LOGE(TAG, "config adv data failed, error code = %x", ret);
        }
        _adv_config_done |= adv_config_flag;
        //config scan response data
        ret = esp_ble_gap_config_adv_data(&scan_rsp_data);
        if (ret){
            ESP_LOGE(TAG, "config scan response data failed, error code = %x", ret);
        }
        _adv_config_done |= scan_rsp_config_flag;

        esp_ble_gatts_create_service(gatt_if, &_service_id, GATTS_NUM_HANDLE);
        break;
    }
    case ESP_GATTS_READ_EVT: {
        ESP_LOGI(TAG, "GATT_READ_EVT, conn_id %d, trans_id %d, handle %d\n",
                param->read.conn_id, param->read.trans_id, param->read.handle);
        GattChar* gc=find_gattchar(param->read.handle);
        if (gc) {
            GattData* d=gc->value();
            esp_gatt_rsp_t rsp;
            memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
            rsp.attr_value.handle = param->read.handle;
            rsp.attr_value.len = d->len;
            memcpy(rsp.attr_value.value, d->data, d->len);
            esp_ble_gatts_send_response(gatt_if, param->read.conn_id,
                    param->read.trans_id, ESP_GATT_OK, &rsp);
        }
        break;
    }
    case ESP_GATTS_WRITE_EVT: {
        ESP_LOGI(TAG, "GATT_WRITE_EVT, conn_id %d, trans_id %d, handle %d",
                param->write.conn_id, param->write.trans_id, param->write.handle);
        if (!param->write.is_prep) {
            ESP_LOGI(TAG, "GATT_WRITE_EVT, value len %d, value :", param->write.len);
            esp_log_buffer_hex(TAG, param->write.value, param->write.len);
            GattDescriptor* gd=find_gattDescriptor(param->write.handle);
            if (gd && gd->handle() == param->write.handle && param->write.len == 2) {
                GattChar* gc=gd->_characteristic;
                uint16_t descr_value = param->write.value[1]<<8 | param->write.value[0];
                if (descr_value == 0x0001){
                    if (gc->_property & ESP_GATT_CHAR_PROP_BIT_NOTIFY) {
                        ESP_LOGI(TAG, "notify enable");
                        GattData* d=gc->value();
                        esp_ble_gatts_send_indicate(gatt_if, param->write.conn_id,
                                gc->_handle, d->len, d->data, false);
                    }
                }else if (descr_value == 0x0002) {
                    if (gc->_property & ESP_GATT_CHAR_PROP_BIT_INDICATE){
                        GattData* d=gc->value();
                        ESP_LOGI(TAG, "indicate enable");
                        esp_ble_gatts_send_indicate(gatt_if, param->write.conn_id,
                                gc->_handle, d->len, d->data, true);
                    }
                }
                else if (descr_value == 0x0000){
                    ESP_LOGI(TAG, "notify/indicate disable ");
                }else{
                    ESP_LOGE(TAG, "unknown descr value");
                    esp_log_buffer_hex(TAG, param->write.value, param->write.len);
                }
            }
        }
        write_event_env(gatt_if, param);
        break;
    }
    case ESP_GATTS_EXEC_WRITE_EVT:
        ESP_LOGI(TAG,"ESP_GATTS_EXEC_WRITE_EVT");
        esp_ble_gatts_send_response(gatt_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, NULL);
        exec_write_event_env(param);
        break;
    case ESP_GATTS_MTU_EVT:
        ESP_LOGI(TAG, "ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
        break;
    case ESP_GATTS_UNREG_EVT:
        break;
    case ESP_GATTS_CREATE_EVT: {
        ESP_LOGI(TAG, "ESP_GATTS_CREATE_EVT, status %d,  service_handle %d\n",
                param->create.status, param->create.service_handle);
        _service_handle = param->create.service_handle;

        esp_ble_gatts_start_service(_service_handle);

        // now create characteristics
        std::list<GattChar*>::iterator it;
        for (it=_char_list.begin(); it!=_char_list.end(); it++) {
            GattChar* gc=*it;
            ESP_LOGI(TAG, "ESP_GATTS_CREATE_EVT, adding characteristic %x,"
                    "  service_handle %d\n", gc->_uuid.uuid.uuid16,
                    _service_handle);
            esp_err_t ret = esp_ble_gatts_add_char(_service_handle, &gc->_uuid,
                    ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, gc->_property, 0, 0);
            if (ret){
                ESP_LOGE(TAG, "add char failed, error code =%x",ret);
            }
        }
        break;
        }
    case ESP_GATTS_ADD_INCL_SRVC_EVT:
        break;
    case ESP_GATTS_ADD_CHAR_EVT: {
        ESP_LOGI(TAG, "ESP_GATTS_ADD_CHAR_EVT,  attr_handle %d,"
                " service_handle %d char_uuid=0x%x", param->add_char.attr_handle,
                param->add_char.service_handle,
                param->add_char.char_uuid.uuid.uuid16);
        if (param->add_char.status!=ESP_GATT_OK) {
            ESP_LOGE(TAG, "Add _characteristic status 0x%02x.",
                    param->add_char.status);
        }

        GattChar* gc=find_gattchar(&param->add_char.char_uuid);
        if (gc) {
            // set characteristic handle
            gc->_handle=param->add_char.attr_handle;

            GattDescriptor* d=gc->descriptor();
            // Add Descriptor.
            if (d) {
                ESP_LOGI(TAG, "ESP_GATTS_ADD_CHAR_EVT, adding descriptor.");
                esp_err_t ret = esp_ble_gatts_add_char_descr(
                    param->add_char.service_handle,
                    &d->_uuid, d->_permissions, &d->_value, 0);
                if (ret)
                    ESP_LOGE(TAG, "add char descr failed, error code =%x", ret);
            }
        }
        break;
    }
    case ESP_GATTS_ADD_CHAR_DESCR_EVT: {
        ESP_LOGI(TAG, "ESP_GATTS_ADD_CHAR_DESCR_EVT, status %d, attr_handle %d,"
                " service_handle %d\n",
                 param->add_char_descr.status, param->add_char_descr.attr_handle,
                 param->add_char_descr.service_handle);
        GattChar* gc=find_gattchar(&param->add_char_descr.char_uuid);
        if (gc && gc->descriptor())
            gc->descriptor()->set_handle(param->add_char_descr.attr_handle);
        }
        break;
    case ESP_GATTS_DELETE_EVT:
        break;
    case ESP_GATTS_START_EVT:
        ESP_LOGI(TAG, "ESP_GATTS_START_EVT, status %d, service_handle %d\n",
                 param->start.status, param->start.service_handle);
        break;
    case ESP_GATTS_STOP_EVT:
        break;
    case ESP_GATTS_CONNECT_EVT: {
        esp_ble_conn_update_params_t conn_params;
        bzero(&conn_params, sizeof(conn_params));
        memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
        /* For the IOS system, please reference the apple official documents
         * about the ble connection parameters restrictions. */
        conn_params.latency = 0;
        conn_params.max_int = 0x20;    // max_int = 0x20*1.25ms = 40ms
        conn_params.min_int = 0x10;    // min_int = 0x10*1.25ms = 20ms
        conn_params.timeout = 400;    // timeout = 400*10ms = 4000ms
        ESP_LOGI(TAG, "ESP_GATTS_CONNECT_EVT, conn_id %d, remote %02x:%02x:%02x:%02x:%02x:%02x:",
         param->connect.conn_id, param->connect.remote_bda[0],
         param->connect.remote_bda[1], param->connect.remote_bda[2],
         param->connect.remote_bda[3], param->connect.remote_bda[4],
         param->connect.remote_bda[5]);
        _conn_id = param->connect.conn_id;
        std::list<GattChar*>::iterator it;
        for (it=_char_list.begin(); it!=_char_list.end(); it++)
            (*it)->set_conn_id(_conn_id);
        //start sent the update connection parameters to the peer device.
        esp_ble_gap_update_conn_params(&conn_params);
        break;
    }
    case ESP_GATTS_DISCONNECT_EVT: {
        ESP_LOGI(TAG, "ESP_GATTS_DISCONNECT_EVT");
        std::list<GattChar*>::iterator it;
        for (it=_char_list.begin(); it!=_char_list.end(); it++)
            (*it)->set_conn_id(0);
        esp_ble_gap_start_advertising(&adv_params);
        }
        break;
    case ESP_GATTS_CONF_EVT:
        ESP_LOGI(TAG, "ESP_GATTS_CONF_EVT, status %d", param->conf.status);
        if (param->conf.status != ESP_GATT_OK){
            esp_log_buffer_hex(TAG, param->conf.value, param->conf.len);
        }
        break;
    case ESP_GATTS_OPEN_EVT:
    case ESP_GATTS_CANCEL_OPEN_EVT:
    case ESP_GATTS_CLOSE_EVT:
    case ESP_GATTS_LISTEN_EVT:
    case ESP_GATTS_CONGEST_EVT:
    default:
        break;
    }
}

void GattService::setup() {
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_err_t ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(TAG, "%s initialize controller failed\n", __func__);
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(TAG, "%s enable controller failed\n", __func__);
        return;
    }
    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(TAG, "%s init bluetooth failed\n", __func__);
        return;
    }
    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(TAG, "%s enable bluetooth failed\n", __func__);
        return;
    }

    ret = esp_ble_gatts_register_callback(GattService::gatts_event_handler);
    if (ret){
        ESP_LOGE(TAG, "gatts register error, error code = %x", ret);
        return;
    }
    ret = esp_ble_gap_register_callback(GattService::gap_event_handler);
    if (ret){
        ESP_LOGE(TAG, "gap register error, error code = %x", ret);
        return;
    }
    ret = esp_ble_gatts_app_register(PROFILE_APP_ID);
    if (ret){
        ESP_LOGE(TAG, "gatts app register error, error code = %x", ret);
        return;
    }
    esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(500);
    if (local_mtu_ret){
        ESP_LOGE(TAG, "set local  MTU failed, error code = %x", local_mtu_ret);
    }
}

GattChar* GattService::find_gattchar(esp_bt_uuid_t* uuid) {
    std::list<GattChar*>::iterator it;
    for (it=_char_list.begin(); it!=_char_list.end(); it++) {
        GattChar* g=*it;
        if (g->_uuid.len!=uuid->len)
            continue;
        if (uuid->len==ESP_UUID_LEN_16 && uuid->uuid.uuid16==g->_uuid.uuid.uuid16)
            return g;
        if (uuid->len==ESP_UUID_LEN_32 && uuid->uuid.uuid32==g->_uuid.uuid.uuid32)
            return g;
        if (uuid->len==ESP_UUID_LEN_128 &&
                !memcmp(&uuid->uuid.uuid128,&g->_uuid.uuid.uuid16, ESP_UUID_LEN_128))
            return g;
    }
    return 0;
}

GattChar* GattService::find_gattchar(uint16_t attr_handle) {
    std::list<GattChar*>::iterator it;
    for (it=_char_list.begin(); it!=_char_list.end(); it++) {
        GattChar* g=*it;
        if (g->_handle==attr_handle)
            return g;
    }
    return 0;
}

GattDescriptor* GattService::find_gattDescriptor(uint16_t attr_handle) {
    std::list<GattChar*>::iterator it;
    for (it=_char_list.begin(); it!=_char_list.end(); it++) {
        GattChar* g=*it;
        if (g->descriptor() && g->descriptor()->handle()==attr_handle)
            return g->descriptor();
    }
    return 0;
}

