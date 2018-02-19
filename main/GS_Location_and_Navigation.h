#ifndef GS_LOCNAV_H
#define GS_LOCNAV_H
#include "GattService.h"
#include <list>

#define GS_UUID_Location_and_Navigation      0x1819

class GS_Location_and_Navigation: public GattService {
		virtual void makeGattChars();

	public:
		GS_Location_and_Navigation();
};

#endif

