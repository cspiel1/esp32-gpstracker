#include "GS_Location_and_Navigation.h"

GS_Location_and_Navigation::GS_Location_and_Navigation():
    GattService({ESP_UUID_LEN_16, {GS_UUID_Location_and_Navigation}}) {

    GattService::setInstance(this);
}

void GS_Location_and_Navigation::makeGattChars() {
    _gc_ls=new GC_Location_and_Speed(_gatt_if);
    _gc_lf=new GC_LnFeature(_gatt_if);
    _gc_pq=new GC_Position_Quality(_gatt_if);

    _char_list.push_back(_gc_lf);
    _char_list.push_back(_gc_ls);
    _char_list.push_back(_gc_pq);
}

void GS_Location_and_Navigation::setDate(uint16_t year, uint8_t month,
        uint8_t day) {
    _year=year;
    _month=month;
    _day=day;
}

void GS_Location_and_Navigation::reportWaypoint(Waypoint* wp) {
    uint32_t t = wp->_time;
    uint64_t utc;
    utc  = ((uint64_t) _year)  << (6*8);
    utc |= ((uint64_t) _month) << (5*8);
    utc |= ((uint64_t) _day)   << (4*8);
    utc |= ((uint64_t) t)      << (1*8);

    _gc_ls->notify(wp->_latitude, wp->_longtitude, utc);
}
