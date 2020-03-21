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

#include "watchdog.h"

#include <avr/wdt.h>

namespace canio {
namespace system {

void Watchdog::enable() { wdt_enable(WDTO_8S); }

void Watchdog::reset() { wdt_reset(); }

void Watchdog::forceRestart() {
  wdt_enable(WDTO_15MS);
  while (1) {
  }
}

}  // namespace system
}  // namespace canio
