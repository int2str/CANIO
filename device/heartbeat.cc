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

#include "heartbeat.h"

#include <avr/io.h>

#include "events.h"

namespace canio {
namespace device {

constexpr uint8_t HEARTBEAT_PIN = (1 << PB6);

Heartbeat& Heartbeat::init() {
  static Heartbeat heartbeat;
  return heartbeat;
}

Heartbeat::Heartbeat() { DDRB |= HEARTBEAT_PIN; }

void Heartbeat::onEvent(const event::Event& event) {
  if (event.id == EVENT_UPDATE) PINB = HEARTBEAT_PIN;
}

}  // namespace device
}  // namespace canio
