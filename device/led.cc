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

#include "led.h"

#include <avr/io.h>

namespace canio {
namespace device {

Led::Led() {
  DDRB |= (1 << PB1);
  off();
}

void Led::on() { PORTB |= (1 << PB1); }

void Led::off() { PORTB &= ~(1 << PB1); }

}  // namespace device
}  // namespace canio
