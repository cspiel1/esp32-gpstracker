#include "GPSTracker.h"
#include <Arduino.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/uart.h>

#undef swap
#include <Wire.h>
#include <WiFi.h>

#include <stdio.h>

#define SSID "OpenWrt-chris"
#define WIFI_PWD "zippezappe538"

#define SDA     18
#define SCL     19

#define BME280_ADR 0x76

#define SEALEVELPRESSURE_HPA 1013.25f

GPSTracker::GPSTracker() : _display(0), _bmeok(false), _seaLevel(SEALEVELPRESSURE_HPA) {
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
    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        tick();
    }
}

void GPSTracker::ota() {
    printf("%s\n", __FUNCTION__);
    init_wifi();
}

#define TXD  (GPIO_NUM_1)
#define RXD  (GPIO_NUM_3)
#define RTS  (UART_PIN_NO_CHANGE)
#define CTS  (UART_PIN_NO_CHANGE)
#define BUF_SIZE (1024)

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
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);

    String line;
    while (1) {
        // Read data from the UART
        int len = uart_read_bytes(UART_NUM_0, data, BUF_SIZE, 20 / portTICK_RATE_MS);
        data[len]=0;
        line = line + (char*) data;
        if (memchr(data, '\n', len)) {
            if (line=="ota") {
                ota();
            } else {
                printf("Unkown command: |%s|\n", line.c_str());
            }
            line="";
        }
    }
}

void GPSTracker::init_wifi() {
    printf("%s\n", __FUNCTION__);
    WiFi.begin(SSID, WIFI_PWD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

bool GPSTracker::start() {
    printf("%s this=%p\n", __FUNCTION__, this);
    xTaskCreate(&cb_uarttask, "UART Task", 2048, this, 5, NULL);
    Wire.setBus(1);
    Wire.begin(SDA, SCL);
    return pdPASS==xTaskCreate(&cb_task, "GPSTracker Task", 2048, this, 5, NULL);
}

bool GPSTracker::init_display()  {
    _display = new Adafruit_SSD1306(-1);
    _display->begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS, false);
    return true;
}

bool GPSTracker::init_bme280()  {

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
        init_display();
    } else if (!_bmeok) {
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
    } else {
        _seaLevel=SEALEVELPRESSURE_HPA;
        float v=_bme.readAltitude(_seaLevel);
        printf("h=%f _seaLevel=%f\n", v, _seaLevel);
        display_altitude(v);
    }
}
