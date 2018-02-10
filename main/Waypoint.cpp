#include "Waypoint.h"

Waypoint::Waypoint(uint32_t time, uint8_t satused, float alt, uint8_t satview,
		double latitude, char north, double longtitude, char east):
	_time(time), _satused(satused), _alt(alt), _satview(satview),
	_latitude(latitude), _north(north), _longtitude(longtitude), _east(east) {
	}

