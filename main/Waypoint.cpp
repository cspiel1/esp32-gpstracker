#include "Waypoint.h"

Waypoint::Waypoint(unsigned int time, unsigned char satused, float alt, unsigned char satview,
		double latitude, char north, double longtitude, char east):
	_time(time), _satused(satused), _alt(alt), _satview(satview),
	_latitude(latitude), _north(north), _longtitude(longtitude), _east(east) {
	}

