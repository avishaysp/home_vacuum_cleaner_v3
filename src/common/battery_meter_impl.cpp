// src/common/battery_meter_imp.cpp
#include "battery_meter_impl.h"
#include "consts.h"

BatteryMeterImpl::BatteryMeterImpl(size_t& battery_level) : battery_level(battery_level) {}

size_t BatteryMeterImpl::getBatteryState() const {
    return battery_level / BATTERY_FACTOR;
}