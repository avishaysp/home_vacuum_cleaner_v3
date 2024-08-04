#include "battery_meter_imp.h"


BatteryMeterImp::BatteryMeterImp(size_t& battery_level) : battery_level(battery_level) {}

size_t BatteryMeterImp::getBatteryState() const {
    return battery_level / 100;
}