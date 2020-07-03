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

#ifndef LED_H
#define LED_H

#include <avr/interrupt.h>
#include <stdint.h>

#include "utils/cpp.h"

namespace canio {
namespace device {

class Led {
 public:
  Led();

  void on();
  void off();

  void timedOn(uint16_t ticks);
  
  void update();

 private:
  uint16_t ticks_;

  DISALLOW_COPY_AND_ASSIGN(Led);
};

}  // namespace device
}  // namespace canio

#endif  // LED_H
