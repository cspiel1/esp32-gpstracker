#ifndef GPS_TRACKER_H
#define GPS_TRACKER_H

#include "Adafruit_SSD1306.h"
#include "Adafruit_BME280.h"

class GPSTracker {
	private:
		Adafruit_SSD1306* _display;
		Adafruit_BME280 _bme;
		bool _bmeok;
		float _seaLevel;
		bool _run;

		bool init_display();
		bool init_bme280();
		bool init_gps();
		void display_altitude(float alt);
		void display_info(int row, const char* info);
		void tick();
		void init_wifi();
		void wifi_scan();

		static void cb_task(void *parm);
		static void cb_otatask(void *parm);
		static void cb_uarttask(void *parm);
		static void cb_gpstask(void *parm);

		void run();
		void ota();
		void uart();
		void gps();

	public:
		GPSTracker();
		bool start();
};
#endif

