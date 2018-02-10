#ifndef WAYPOINT_H
#define WAYPOINT_H
#include <stdint.h>

class Waypoint {
	public:
		uint32_t _time;
		uint8_t _satused;
		float _alt;
		uint8_t _satview;
        double _latitude;
        char _north;
        double _longtitude;
        char _east;

		Waypoint(uint32_t time, uint8_t satused, float alt,
				uint8_t satview, double latitude, char north,
				double longtitude, char east);
};

#endif

