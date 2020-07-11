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

// 68'000 pulses/gallon
static constexpr uint16_t PULSES_PER_ML = 18;

static canio::device::FuelSensor* s_sensors = nullptr;

ISR(PCINT0_vect) {
  if (s_sensors) s_sensors->updateIrq0();
}

ISR(PCINT1_vect) {
  if (s_sensors) s_sensors->updateIrq1();
}

namespace canio {
namespace device {

FuelSensor::FuelSensor()
    : enabled_bit_mask_(0), pcint0_last_state_(0), pcint1_last_state_(0) {
  disable();
}

void FuelSensor::enable(uint8_t enable_bit_mask) {
  reset();

  enabled_bit_mask_ = enable_bit_mask;

  PCMSK0 = 0;
  PCMSK1 = 0;

  uint8_t irq_enable = 0;

  if (enabled_bit_mask_ & 0x01) {
    PCMSK1 |= (1 << PCINT12);
    irq_enable |= (1 << PCIE1);
  }

  if (enabled_bit_mask_ & 0x02) {
    PCMSK1 |= (1 << PCINT14);
    irq_enable |= (1 << PCIE1);
  }

  if (enabled_bit_mask_ & 0x04) {
    PCMSK0 |= (1 << PCINT5);
    irq_enable |= (1 << PCIE0);
  }

  if (enabled_bit_mask_ & 0x08) {
    PCMSK0 |= (1 << PCINT7);
    irq_enable |= (1 << PCIE0);
  }

  if (enabled_bit_mask_ & 0x10) {
    PCMSK0 |= (1 << PCINT2);
    irq_enable |= (1 << PCIE0);
  }

  if (enabled_bit_mask_ & 0x20) {
    PCMSK1 |= (1 << PCINT13);
    irq_enable |= (1 << PCIE1);
  }

  if (enabled_bit_mask_ & 0x40) {
    PCMSK1 |= (1 << PCINT15);
    irq_enable |= (1 << PCIE1);
  }

  if (enabled_bit_mask_ & 0x80) {
    PCMSK0 |= (1 << PCINT6);
    irq_enable |= (1 << PCIE0);
  }

  s_sensors = this;
  PCICR = irq_enable;
}

void FuelSensor::disable() {
  PCICR = 0;
  s_sensors = nullptr;
}

void FuelSensor::reset() {
  for (auto& pulses : pulses_) pulses = 0;
}

uint16_t FuelSensor::getMl(uint8_t offset) {
  uint16_t pulses = pulses_[offset];
  if (pulses < PULSES_PER_ML) return 0;

  uint16_t ml = pulses / PULSES_PER_ML;
  pulses_[offset] -= (ml * PULSES_PER_ML);

  return ml;
}

void FuelSensor::updateIrq0() {
  uint8_t change = PINB ^ pcint0_last_state_;
  pcint0_last_state_ = PINB;

  if ((enabled_bit_mask_ & (1 << 2)) && (change & (1 << PB5)) &&
      (PINB & (1 << PB5)) == 0) {
    ++pulses_[2];
  }

  if ((enabled_bit_mask_ & (1 << 3)) && (change & (1 << PB7)) &&
      (PINB & (1 << PB7)) == 0) {
    ++pulses_[3];
  }

  if ((enabled_bit_mask_ & (1 << 4)) && (change & (1 << PB2)) &&
      (PINB & (1 << PB2)) == 0) {
    ++pulses_[4];
  }

  if ((enabled_bit_mask_ & (1 << 7)) && (change & (1 << PB6)) &&
      (PINB & (1 << PB6)) == 0) {
    ++pulses_[7];
  }
}

void FuelSensor::updateIrq1() {
  uint8_t change = PINC ^ pcint1_last_state_;
  pcint1_last_state_ = PINC;

  if ((enabled_bit_mask_ & (1 << 0)) && (change & (1 << PC4)) &&
      (PINC & (1 << PC4)) == 0) {
    ++pulses_[0];
  }

  if ((enabled_bit_mask_ & (1 << 1)) && (change & (1 << PC6)) &&
      (PINC & (1 << PC6)) == 0) {
    ++pulses_[1];
  }

  if ((enabled_bit_mask_ & (1 << 5)) && (change & (1 << PC5)) &&
      (PINC & (1 << PC5)) == 0) {
    ++pulses_[5];
  }

  if ((enabled_bit_mask_ & (1 << 6)) && (change & (1 << PC7)) &&
      (PINC & (1 << PC7)) == 0) {
    ++pulses_[6];
  }
}

}  // namespace device
}  // namespace canio