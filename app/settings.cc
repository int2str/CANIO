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

#include "settings.h"

#include <avr/eeprom.h>

#define SETTINGS_OFFSET 0x10
#define SETTINGS_MARKER 0xAEAE
#define SETTINGS_REVISION 0x01

namespace canio {
namespace app {

Settings::Settings() { defaults(); }

void Settings::defaults() {
  marker = SETTINGS_MARKER;

  // Force upgrade
  revision = 0;
  upgrade();
}

void Settings::upgrade() {
  // Rev 1
  if (revision < 1) {
    can_baud_rate = 1000;
    pulses_per_ml = 18;  // 68,000 p/gal = 18 p/ml
  }

  revision = SETTINGS_REVISION;
}

Settings EepromSettings::load() {
  Settings settings;
  const void *addr = reinterpret_cast<void *>(SETTINGS_OFFSET);
  eeprom_read_block(&settings, addr, sizeof(Settings));

  if (settings.marker != SETTINGS_MARKER ||
      settings.revision > SETTINGS_REVISION)
    settings.defaults();

  if (settings.revision < SETTINGS_REVISION) settings.upgrade();

  return settings;
}

void EepromSettings::save(Settings &settings) {
  void *addr = reinterpret_cast<void *>(SETTINGS_OFFSET);
  eeprom_write_block(&settings, addr, sizeof(Settings));
}

}  // namespace app
}  // namespace canio