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

#include <avr/interrupt.h>

#include "app/commandhandler.h"
#include "event/loop.h"
#include "events.h"
#include "system/watchdog.h"

namespace canio {

void powerSave() {
  // Un-float pins; enable pull-ups where possible
  DDRB = 0;
  PORTB = 0xFF;
  DDRC = 0;
  PORTC = ~((1 << PC2) | (1 << PC3));  // Exclude RXCAN and TXCAN
  DDRD = 0;
  PORTD = 0xFF;

  // Engage Power Reduction Register flags
  PRR = (1 << PRPSC) | (1 << PRSPI) | (1 << PRLIN) | (1 << PRTIM1) |
        (1 << PRTIM1);
}

void run() {
  powerSave();

  system::Watchdog::enable();
  app::CommandHandler::init();

  while (true) {
    event::Loop::post({EVENT_UPDATE});
    event::Loop::dispatch();
    system::Watchdog::reset();
  }
}

}  // namespace canio

int main() {
  sei();
  canio::run();
}
