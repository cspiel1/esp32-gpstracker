#include "GPSTracker.h"

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <Wire.h>

#include "Arduino.h"

#define SDA     18
#define SCL     19

#define BME280_ADR 0x76

#define SEALEVELPRESSURE_HPA 1013.25f

GPSTracker::GPSTracker() : _display(0), _bmeok(false), _seaLevel(SEALEVELPRESSURE_HPA) {
}

void GPSTracker::cb_task(void *parm)
{
    GPSTracker* self=static_cast<GPSTracker*>(parm);
    printf("%s self=%p\n", __FUNCTION__,self);
    self->run();
}

void GPSTracker::run() {
    _display=0;
    _bmeok=false;
    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        tick();
    }
}

bool GPSTracker::start() {
    printf("%s this=%p\n", __FUNCTION__, this);
    return pdPASS==xTaskCreate(&cb_task, "GPSTracker Task", 2048, this, 5, NULL);
}

bool GPSTracker::init_display()  {
    _display = new Adafruit_SSD1306(-1);
    Wire.setBus(1);
    Wire.begin(SDA, SCL);
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
    printf("tick this=%p _display=%p _bmeok=%d\n", this, _display, _bmeok);
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
        _seaLevel=SEALEVELPRESSURE_HPA;
    } else {
        float v=_bme.readAltitude(_seaLevel);
        printf("h=%f _seaLevel=%f\n", v, _seaLevel);
        display_altitude(v);
    }
}
