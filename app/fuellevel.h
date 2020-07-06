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

#ifndef FUELLEVEL_H
#define FUELLEVEL_H

#include <stdint.h>

#include "utils/movingaverage.h"

namespace canio {
namespace app {

class FuelLevel {
  public:
    FuelLevel();

    uint16_t recalculate(uint16_t tank_sensors, uint16_t ml_used);
    bool initialSamplesCollected() const;
    void reset();

  private:
    utils::MovingAverage<uint32_t, 20> tank_sensor_average_;
    uint16_t samples_collected_;
    uint16_t fuel_level_reference_ml_;
};

}  // namespace app
}  // namespace canio

#endif  // FUELLEVEL_H