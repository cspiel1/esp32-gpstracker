#include "GS_Location_and_Navigation.h"

#include "GC_LnFeature.h"
#include "GC_Location_and_Speed.h"
#include "GC_Position_Quality.h"

GS_Location_and_Navigation::GS_Location_and_Navigation():
    GattService({ESP_UUID_LEN_16, {GS_UUID_Location_and_Navigation}}) {

    GattService::setInstance(this);
}

void GS_Location_and_Navigation::makeGattChars() {
    _char_list.push_back(new GC_Location_and_Speed());
    _char_list.push_back(new GC_Position_Quality());
    _char_list.push_back(new GC_LnFeature());
}

