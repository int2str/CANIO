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

#include <avr/interrupt.h>
#include <stdint.h>

#include "utils/cpp.h"

extern "C" void PCINT0_vect() __attribute__((signal));
extern "C" void PCINT1_vect() __attribute__((signal));

namespace canio {
namespace device {

class FuelSensor {
 public:
  FuelSensor();

  void enable(uint8_t enable_bit_mask, uint8_t* pulses_per_ml);
  void disable();

  void get(uint16_t* values);

 private:
  uint8_t enabled_bit_mask_;
  uint8_t pulses_per_ml_[4];
  uint16_t pulses_[4];

  uint8_t pcint0_last_state_;
  uint8_t pcint1_last_state_;

  friend void ::PCINT0_vect();
  friend void ::PCINT1_vect();

  void updateIrq0();
  void updateIrq1();

  void reset();

  DISALLOW_COPY_AND_ASSIGN(FuelSensor);
};

}  // namespace device
}  // namespace canio

#endif  // FUELSENSOR_H
