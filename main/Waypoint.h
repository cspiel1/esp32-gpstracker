#ifndef WAYPOINT_H
#define WAYPOINT_H
class Waypoint {
	public:
		unsigned int _time;
		unsigned char _satused;
		float _alt;
		unsigned char _satview;
        double _latitude;
        char _north;
        double _longtitude;
        char _east;

		Waypoint(unsigned int time, unsigned char satused, float alt,
				unsigned char satview, double latitude, char north,
				double longtitude, char east);
};

#endif

