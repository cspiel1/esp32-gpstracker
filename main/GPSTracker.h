#ifndef GPS_TRACKER_H
#define GPS_TRACKER_H

#include "Adafruit_SSD1306.h"
#include "Adafruit_BME280.h"

class GPSTracker {
	private:
		Adafruit_SSD1306* _display;
		Adafruit_BME280 _bme;
		bool _bmeok;

		bool init_display();
		bool init_bme280();
		bool init_gps();
		void display_altitude(float alt);
		void display_info(int row, const char* info);
		void tick();
		static void cb_task(void *parm);
		void run();

	public:
		GPSTracker();
		bool start();
};
#endif

