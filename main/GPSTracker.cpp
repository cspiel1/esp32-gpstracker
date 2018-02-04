#include "GPSTracker.h"
#include <Arduino.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/uart.h>
#include <nvs.h>
#include <nvs_flash.h>
#include <soc/soc.h>
#include <soc/rtc_cntl_reg.h>
#include <OtaUpdate.h>

#undef swap
#include <Wire.h>
#include <WiFi.h>

#include <stdio.h>

#define MYSSID "OpenWrt-chris"
#define WIFI_PWD "zippezappe538"

#define SDA     18
#define SCL     19

#define BME280_ADR 0x76

#define SEALEVELPRESSURE_HPA 1013.25f

GPSTracker::GPSTracker() : _display(0), _bmeok(false), _seaLevel(SEALEVELPRESSURE_HPA),
    _run(false) {
    _seaLevel=SEALEVELPRESSURE_HPA;
}

void GPSTracker::cb_task(void *parm)
{
    GPSTracker* self=static_cast<GPSTracker*>(parm);
    self->run();
}

void GPSTracker::cb_otatask(void *parm)
{
    GPSTracker* self=static_cast<GPSTracker*>(parm);
    self->ota();
}

void GPSTracker::cb_uarttask(void *parm)
{
    GPSTracker* self=static_cast<GPSTracker*>(parm);
    self->uart();
}

void GPSTracker::run() {
    _display=0;
    _bmeok=false;
    while (true) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        if (_run)
            tick();
    }
}

void GPSTracker::ota() {
    printf("%s\n", __FUNCTION__);
    _run=false;
    init_wifi();
    OtaUpdate* otaupdate= new OtaUpdate("37.120.186.210", 80);
    otaupdate->setFileName("share/gpstracker.bin");
    otaupdate->start();
}

#define TXD  (GPIO_NUM_1)
#define RXD  (GPIO_NUM_3)
#define RTS  (UART_PIN_NO_CHANGE)
#define CTS  (UART_PIN_NO_CHANGE)
#define BUF_SIZE (128)

void GPSTracker::uart() {
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
        .use_ref_tick = false
    };
    uart_param_config(UART_NUM_0, &uart_config);
    uart_set_pin(UART_NUM_0, TXD, RXD, RTS, CTS);
    uart_driver_install(UART_NUM_0, BUF_SIZE * 2, 0, 0, NULL, 0);

    // Configure a temporary buffer for the incoming data
    printf("%s %d\n", __PRETTY_FUNCTION__, __LINE__);
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);
    printf("%s %d\n", __PRETTY_FUNCTION__, __LINE__);

    char line[BUF_SIZE+1];
    line[0]=0;
    while (true) {
        // Read data from the UART
        int len = uart_read_bytes(UART_NUM_0, data, BUF_SIZE, 20 / portTICK_RATE_MS);
        if (len) {
            data[len]=0;
            if (strlen(line)+strlen((char*)data)>BUF_SIZE)
                line[0]=0;
            strcat(line, (char*)data);
            if (memchr(data, '\n', len)) {
                len=strlen(line);
                if (len>=2)
                    line[len-2]=0;
                if (!strcmp(line,"ota")) {
                    ota();
                } else {
                    printf("Unkown command: |%s|\n", line);
                }
                line[0]=0;
            }
        }
        delay(1);
    }
}

void GPSTracker::wifi_scan() {
    int n = WiFi.scanNetworks();
    printf("scan done\n");
    if (n == 0) {
        printf("no networks found\n");
    } else {
        printf("n=%d\n", n);
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            printf("%d : %s %d %s\n", i+1, WiFi.SSID(i).c_str(), WiFi.RSSI(i),
                    WiFi.encryptionType(i) == WIFI_AUTH_OPEN?" ":"*");
        }
    }
}

void GPSTracker::init_wifi() {
    // Initialize NVS.
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        // OTA app partition table has a smaller NVS partition size than the non-OTA
        // partition table. This size mismatch may cause NVS initialization to fail.
        // If this happens, we erase NVS partition and initialize NVS again.
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

//    printf("%s %d\n", __FUNCTION__, __LINE__);
//    WiFi.mode(WIFI_STA);
//    printf("%s %d\n", __FUNCTION__, __LINE__);
//    WiFi.disconnect();
//    printf("%s %d\n", __FUNCTION__, __LINE__);
//    wifi_scan();
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
    printf("%s %d\n", __FUNCTION__, __LINE__);
    WiFi.begin(MYSSID, WIFI_PWD);
    printf("%s %d\n", __FUNCTION__, __LINE__);
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 1); //enable brownout detector
    while (WiFi.status() != WL_CONNECTED) {
        printf("%s %d\n", __FUNCTION__, __LINE__);
        delay(500);
        printf(".");
        fflush(stdout);
    }

    printf("\n");
    printf("WiFi connected\n");
    printf("IP address: %s\n", WiFi.localIP().toString().c_str());
}

bool GPSTracker::start() {
    printf("%s this=%p\n", __FUNCTION__, this);
    _run=true;
    xTaskCreate(&cb_uarttask, "UART Task", 8192, this, 5, NULL);
    Wire.setBus(1);
    Wire.begin(SDA, SCL);
    return pdPASS==xTaskCreate(&cb_task, "GPSTracker Task", 2048, this, 5, NULL);
}

bool GPSTracker::init_display()  {
    printf("%s %d\n", __PRETTY_FUNCTION__, __LINE__);
    _display = new Adafruit_SSD1306(-1);
    printf("%s %d\n", __PRETTY_FUNCTION__, __LINE__);
    _display->begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS, false);
    printf("%s %d\n", __PRETTY_FUNCTION__, __LINE__);
    return true;
}

bool GPSTracker::init_bme280()  {

    printf("%s %d\n", __PRETTY_FUNCTION__, __LINE__);
    if (_bme.begin(BME280_ADR)) {
        printf("BME280 sensor found!\n");
        _bmeok=true;
        return true;
    } else {
        printf("ERR - BME280 sensor not found!\n");
        return false;
    }
}

bool GPSTracker::init_gps()  {
    return true;
}

void GPSTracker::display_altitude(float alt) {
    if (!_display) return;

    _display->clearDisplay();
    // text display tests
    _display->setTextSize(1);
    _display->setTextColor(WHITE);
    _display->setCursor(0,0);
    _display->println("Sming Framework");
    _display->println("");
    //----
    _display->println(String("altitude = ") + alt + " m");
    _display->display();
}

void GPSTracker::display_info(int row, const char* info) {
    // text display tests
    _display->setTextSize(1);
    _display->setTextColor(WHITE);
    _display->setCursor(0,row*20);
    _display->println(info);
    //----
    _display->display();
}

void GPSTracker::tick() {
    printf("tick\n");
    if (!_display) {
        printf("%s %d\n", __PRETTY_FUNCTION__, __LINE__);
        init_display();
    } else if (!_bmeok) {
        printf("%s %d\n", __PRETTY_FUNCTION__, __LINE__);
        if (_display) {
            _display->clearDisplay();
            display_info(0, "Display works");
            display_info(1, "Try to init bme280.");
        }
        init_bme280();
        if (_display) {
            if (_bmeok)
                display_info(2, "   ok");
            else
                display_info(2, "   failed");
        }
        _seaLevel=SEALEVELPRESSURE_HPA;
    } else {
        float v=_bme.readAltitude(_seaLevel);
        printf("h=%f _seaLevel=%f\n", v, _seaLevel);
        display_altitude(v);
    }
}
