#pragma once

#include "../skelaton/battery_meter.h"
#include "../logger.h"



class BatteryMeterImp : public BatteryMeter {
    size_t& battery_level;

public:
    BatteryMeterImp(size_t& battery_level);
	std::size_t getBatteryState() const override;
};

