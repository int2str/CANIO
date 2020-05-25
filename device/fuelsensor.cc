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

#include "utils/byteorder.h"

static canio::device::FuelSensor* s_sensors = nullptr;

ISR(PCINT0_vect) {
  if (s_sensors) s_sensors->updateIrq0();
}

ISR(PCINT1_vect) {
  if (s_sensors) s_sensors->updateIrq1();
}

namespace {

const uint8_t MINIMUM_ML_TO_REPORT = 5;

}  // namespace

namespace canio {
namespace device {

FuelSensor::FuelSensor()
    : enabled_bit_mask_(0), pcint0_last_state_(0), pcint1_last_state_(0) {
  disable();
}

void FuelSensor::enable(uint8_t enable_bit_mask, uint8_t* pulses_per_ml) {
  reset();

  enabled_bit_mask_ = enable_bit_mask;
  for (uint8_t i = 0; i != 4; ++i) pulses_per_ml_[i] = pulses_per_ml[i];

  PCMSK0 = 0;
  PCMSK1 = 0;

  uint8_t irq_enable = 0;

  if (enable_bit_mask & 0x01) {
    PCMSK1 |= (1 << PCINT12);
    irq_enable |= (1 << PCIE1);
  }

  if (enable_bit_mask & 0x02) {
    PCMSK1 |= (1 << PCINT13);
    irq_enable |= (1 << PCIE1);
  }

  if (enable_bit_mask & 0x04) {
    PCMSK0 |= (1 << PCINT5);
    irq_enable |= (1 << PCIE0);
  }

  if (enable_bit_mask & 0x08) {
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

void FuelSensor::get(uint16_t* values) {
  for (uint8_t i = 0; i != 4; ++i) {
    if ((enabled_bit_mask_ & (1 << i)) == 0) continue;
    if (pulses_[i] < (pulses_per_ml_[i] * MINIMUM_ML_TO_REPORT)) continue;
    values[i] = utils::lsb_to_msb(pulses_[i] / pulses_per_ml_[i]);
    pulses_[i] = pulses_[i] % pulses_per_ml_[i];
  }
}

void FuelSensor::updateIrq0() {
  uint8_t change = PINB ^ pcint0_last_state_;
  pcint0_last_state_ = PINB;

  if ((enabled_bit_mask_ & (1 << 2)) && (change & (1 << PB5)) &&
      (PINB & (1 << PB5)) == 0) {
    ++pulses_[2];
  }

  if ((enabled_bit_mask_ & (1 << 3)) && (change & (1 << PB6)) &&
      (PINB & (1 << PB6)) == 0) {
    ++pulses_[3];
  }
}

void FuelSensor::updateIrq1() {
  uint8_t change = PINC ^ pcint1_last_state_;
  pcint1_last_state_ = PINC;

  if ((enabled_bit_mask_ & (1 << 0)) && (change & (1 << PC4)) &&
      (PINC & (1 << PC4)) == 0) {
    ++pulses_[0];
  }

  if ((enabled_bit_mask_ & (1 << 1)) && (change & (1 << PC5)) &&
      (PINC & (1 << PC5)) == 0) {
    ++pulses_[1];
  }
}

}  // namespace device
}  // namespace canio