// src/common/battery_meter_imp.h
#pragma once

#include "BatteryMeter.h"
#include "logger.h"



class BatteryMeterImpl : public BatteryMeter {
    size_t& battery_level;

public:
    BatteryMeterImpl(size_t& battery_level);
	std::size_t getBatteryState() const override;
};

