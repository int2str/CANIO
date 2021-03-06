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

#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

namespace canio {
namespace system {

// Simple tick counter for various timing tasks.
// Using this will enable global interrupts and use
// timer0.
class Timer {
  Timer();

 public:
  // This is lazily initialized. First call will always
  // return 0. TImer overflows every ~49 days.
  static uint32_t millis();

 private:
  uint32_t millis_impl() const;
};

}  // namespace system
}  // namespace canio

#endif  // TIMER_H