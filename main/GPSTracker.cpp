#include "GPSTracker.h"

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <Wire.h>

#include "Arduino.h"

#define SDA     18
#define SCL     19

#define BME280_ADR 0x76

GPSTracker::GPSTracker() : _display(0), _bmeok(false) {
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
    _display->end();
}

bool GPSTracker::start() {
    printf("%s this=%p\n", __FUNCTION__, this);
    return pdPASS==xTaskCreate(&cb_task, "GPSTracker Task", 2048, this, 5, NULL);
}

bool GPSTracker::init_display()  {
    _display = new Adafruit_SSD1306(0x3c, SDA, SCL);
    return _display->init();
}

bool GPSTracker::init_bme280()  {
    TwoWire wire1(0);
    wire1.begin(SDA, SCL);

    if (_bme.begin(BME280_ADR,&wire1)) {
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
    printf("Display: some text\n");
    _display->clear();
    // text display tests
    _display->setFont("ArialMT_Plain_24");
    _display->setColor(WHITE);
    _display->drawString(0,0,"Sming Framework");
    //----
    _display->drawString(0,28,String("altitude = ") + alt + " m");
    _display->display();
}

void GPSTracker::display_info(int row, const char* info) {
    // text display tests
    _display->setFont("ArialMT_Plain_24");
    _display->setColor(WHITE);
    _display->drawString(0,row*26,info);
    //----
    _display->display();
}

#define SEALEVELPRESSURE_HPA (1013.25)
float _seaLevel=SEALEVELPRESSURE_HPA;

void GPSTracker::tick() {
    printf("tick this=%p _display=%p _bmeok=%d\n", this, _display, _bmeok);
    if (!_display) {
        init_display();
    } else if (!_bmeok) {
        _display->clear();
        display_info(0, "Display works");
        display_info(1, "Try to init bme280.");
        init_bme280();
        if (_bmeok)
            display_info(2, "   ok");
        else
            display_info(2, "   failed");
    } else {
//        float v=_bme.readAltitude(_seaLevel);
//        Serial.printf("v=%f\n", v);
//        display_altitude(v);
    }
}
