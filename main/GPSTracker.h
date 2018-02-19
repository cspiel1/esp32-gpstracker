#ifndef GPS_TRACKER_H
#define GPS_TRACKER_H

#include "Waypoint.h"
#include <Adafruit_SSD1306.h>
#include <Adafruit_BME280.h>

#undef swap
#include <vector>

class GPSTracker {
	private:
		Adafruit_SSD1306* _display;
		Adafruit_BME280 _bme;
		bool _bmeok;
		float _seaLevel;
		bool _run;
		uint8_t _satused;
		float _alt;
		char _uAlt;
		uint8_t _satview;
        double _latitude;
        char _north;
        double _longtitude;
        char _east;

		std::vector<Waypoint*> _wps;
		void flush_wps();

		bool init_display();
		bool init_bme280();
		bool init_gps();
		bool init_uart();
		bool init_spiff();
		void init_nvs();
		void init_wifi();
		void init_ble();

		void display_altitude(float alt);
		void display_info(int row, const char* info);
		void tick();
		void wifi_scan();
		char* scan(const char* line, char& c);
		char* scan(const char* line, int& i);
		char* scan(const char* line, uint8_t& i);
		char* scan(const char* line, float& f);
		char* scan(const char* line, double& d);
		char* scan(const char* line, unsigned int& u);
		void process_nmea(const char* line);
		void process_ubx(uint8_t* data, int len);
		void print_ubx(uint8_t* data, int len);

		static void cb_task(void *parm);
		static void cb_otatask(void *parm);
		static void cb_uarttask(void *parm);
		static void cb_gpstask(void *parm);

		void run();
		void ota();
		void uart();
		void gps();

		void send_ubx(uint8_t cls, uint8_t id, const char* data, size_t len);

	public:
		GPSTracker();
		bool start();
};
#endif

