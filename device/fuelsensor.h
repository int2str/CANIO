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

#ifndef FUELSENSOR_H
#define FUELSENSOR_H

#include <stdint.h>

#include "utils/cpp.h"

namespace canio {
namespace device {

class FuelSensor {
 public:
  FuelSensor();

  void enableUpdates();
  void disableUpdates();

  void reset();
  uint32_t get() const;

  DISALLOW_COPY_AND_ASSIGN(FuelSensor);
};

}  // namespace device
}  // namespace canio

#endif  // FUELSENSOR_H
