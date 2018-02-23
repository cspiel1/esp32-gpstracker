#ifndef GS_LOCNAV_H
#define GS_LOCNAV_H
#include "GattService.h"
#include "Waypoint.h"
#include "GC_LnFeature.h"
#include "GC_Location_and_Speed.h"
#include "GC_Position_Quality.h"

#include <list>

#define GS_UUID_Location_and_Navigation      0x1819

class GS_Location_and_Navigation: public GattService {
		virtual void makeGattChars();
		GC_Location_and_Speed* _gc_ls;
		GC_Position_Quality* _gc_pq;
		GC_LnFeature* _gc_lf;

		uint16_t _year;
		uint8_t _month;
		uint8_t _day;

	public:
		GS_Location_and_Navigation();
		void reportWaypoint(Waypoint* wp);
		void setDate(uint16_t year, uint8_t month, uint8_t day);
};

#endif

