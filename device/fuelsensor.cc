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

#include "fuelsensor.h"

#include <avr/interrupt.h>

namespace {

volatile uint32_t s_pulses = 0;
volatile uint8_t s_last_state = 0;

ISR(PCINT0_vect) {
  uint8_t change = PINB ^ s_last_state;
  s_last_state = PINB;
  // Count on falling edge change
  if ((change & (1 << PB5)) && (PINB & (1 << PB5)) == 0) ++s_pulses;
}

}  // namespace

namespace canio {
namespace device {

FuelSensor::FuelSensor() {
  PORTB |= (1 << PB5);  // Enable internal pull-up
  disableUpdates();
}

void FuelSensor::enableUpdates() {
  PCMSK0 = (1 << PCINT5);  // PCINT5 = PB5 = IO_3
  PCICR = (1 << PCIE0);
}

void FuelSensor::disableUpdates() { PCICR = 0; }

void FuelSensor::reset() { s_pulses = 0; }

uint32_t FuelSensor::get() const { return s_pulses; }

}  // namespace device
}  // namespace canio