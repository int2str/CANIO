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

#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#define SETTINGS_OFFSET 0x10
#define SETTINGS_MARKER_START 0xAE00
#define SETTINGS_MARKER_END 0x00AE
#define SETTINGS_REVISION 0x01

namespace {

template<typename T>
void array_set(T *array, uint8_t length, T value) {
  for (auto i = 0; i != length; ++i) array[i] = value;
}

}  // namespace

namespace canio {
namespace app {

void EepromSettings::defaults(Settings &settings) {
  settings.marker_start = SETTINGS_MARKER_START;
  settings.marker_end = SETTINGS_MARKER_END;

  // Force upgrade
  settings.revision = 0;
  upgrade(settings);
}

void EepromSettings::upgrade(Settings &settings) {
  // Rev 1
  if (settings.revision < 1) {
    settings.can_baud_rate = 500;
    settings.can_base_id = 0x4AE;

    array_set(settings.io_config, 4, static_cast<uint8_t>(0));
    array_set(settings.io_params, 4, static_cast<uint8_t>(0));
  }

  settings.revision = SETTINGS_REVISION;
}

void EepromSettings::load(Settings &settings) {
  cli();

  bool needs_saving = false;

  uint8_t retries = 5;
  uint8_t valid = 0;

  while (!valid && retries--) {
    const void *addr = reinterpret_cast<void *>(SETTINGS_OFFSET);
    eeprom_read_block(&settings, addr, sizeof(Settings));

    if (settings.marker_start != SETTINGS_MARKER_START ||
        settings.marker_end != SETTINGS_MARKER_END ||
        settings.revision > SETTINGS_REVISION) {
      valid = false;
      _delay_ms(250);
    } else {
      valid = true;
    }
  }

  if (!valid) {
    defaults(settings);
    needs_saving = true;
  }

  if (settings.revision < SETTINGS_REVISION) {
    upgrade(settings);
    needs_saving = true;
  }

  sei();

  if (needs_saving) EepromSettings::save(settings);
}

void EepromSettings::save(Settings &settings) {
  cli();
  void *addr = reinterpret_cast<void *>(SETTINGS_OFFSET);
  eeprom_write_block(&settings, addr, sizeof(Settings));
  sei();
}

}  // namespace app
}  // namespace canio