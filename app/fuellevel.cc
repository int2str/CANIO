// CANIO - CAN I/O board firmware
// Copyright (C) 2020 Andre Eisenbach
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// See LICENSE for a copy of the GNU General Public License or see
// it online at <http://www.gnu.org/licenses/>.

#include "fuellevel.h"

namespace {

// Tune this constant here to get an initial warm-up period, and make sure
// there's room in the tank_sensor_average_ as needed:
constexpr uint16_t INITIAL_SAMPLES_TO_COLLECT = 60;

constexpr uint16_t map_range(uint16_t value, uint16_t in_min, uint16_t in_max,
                             uint16_t out_min, uint16_t out_max) {
  return static_cast<uint32_t>(value - in_min) * (out_max - out_min) /
             (in_max - in_min) +
         out_min;
}

constexpr uint16_t tankSensorToMl(uint16_t tank_sensor_adc) {
  // ADC = 0 ... 1023
  // Fuel = 0 ... 63l
  // Voltage = ~0.6V ... ~2V
  // =======
  // Expected ADC = ~122 ... ~410
  constexpr uint16_t EMPTY_TANK_ADC = 122;
  constexpr uint16_t FULL_TANK_ADC = 410;
  constexpr uint16_t FULL_TANK_ML = 63000;

  return map_range(tank_sensor_adc, EMPTY_TANK_ADC, FULL_TANK_ADC, 0,
                   FULL_TANK_ML);
}

}  // namespace

namespace canio {
namespace app {

FuelLevel::FuelLevel() : samples_collected_{0} {}

uint16_t FuelLevel::recalculate(uint16_t tank_sensors, uint16_t ml_used) {
  // Assumption time....
  // This function is called about every 50ms. This is fully outside of the
  // control of this function/class. This matters for how long the initial
  // sample period is.

  if (!initialSamplesCollected()) {
    // Initial sample period is used to accumulate values for or initial
    // baseline. Consumption is ignored at this point.

    ++samples_collected_;
    tank_sensor_average_.push(tank_sensors);
    fuel_level_reference_ml_ = tankSensorToMl(tank_sensor_average_.get());

  } else {
    // Now we start subtracting fuel consumption ...

    if (fuel_level_reference_ml_ > ml_used) {
      fuel_level_reference_ml_ -= ml_used;
    } else {
      fuel_level_reference_ml_ = 0;
    }
  }

  return fuel_level_reference_ml_;
}

void FuelLevel::reset() {
  tank_sensor_average_.clear();
  samples_collected_ = 0;
}

bool FuelLevel::initialSamplesCollected() const {
  return samples_collected_ >= INITIAL_SAMPLES_TO_COLLECT;
}

}  // namespace app
}  // namespace canio