# GPS Tracker Project

An esp32 project that reads altitude from bme280 i2c sensor and GPS data from
NEO-6M GPS Module. An ssd1306 oled display is connected.

This project uses arduino-esp32 library.

Provides a new class for ota http update:
components/other/OtaUpdate.h

Features:
- bme280 sensor (i2c)
- ssd1306 oled display (i2c)
- serial input
- ota http update
- store GPS data on spiff
- TODO: read and store GPS data
- TODO: BLE for GPS data transfer to host

Pins:
```
  I2C
      SDA     GPIO18
      SCL     GPIO19

  GPS NEO-6M  esp32
      RxD     TxD - GPIO17
      TxD     RxD - GPIO16
```

Usage:
- Edit user.mk and set OTASERVER, OTAFILE, MYSSID, WIFI_PWD, SERVERPATH
    adequately for the ota update.
- Build and flash the project by calling:

$ make

$ make flash

- Prepair your websever and upload the .bin file from your build directory.

$ make deploy

- make monitor did not work, so I added the serialpy.sh script. Call it:

$ ./serialpy.sh

- Now you may start the upgrade by typing "ota" then press Enter!
