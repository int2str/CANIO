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
#define SETTINGS_MARKER 0xAE10
#define SETTINGS_REVISION 0x01

namespace {

template<typename T>
void array_set(T *array, uint8_t length, T value) {
  for (auto i = 0; i != length; ++i) array[i] = value;
}

}  // namespace

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
    can_base_id = 0x4AE;

    array_set(io_config, 4, static_cast<uint8_t>(0));
    array_set(io_params, 4, static_cast<uint8_t>(0));
  }

  revision = SETTINGS_REVISION;
}

void EepromSettings::load(Settings &settings) {
  const void *addr = reinterpret_cast<void *>(SETTINGS_OFFSET);
  eeprom_read_block(&settings, addr, sizeof(Settings));
  bool needs_saving = false;

  if (settings.marker != SETTINGS_MARKER ||
      settings.revision > SETTINGS_REVISION) {
    settings.defaults();
    needs_saving = true;
  }

  if (settings.revision < SETTINGS_REVISION) {
    settings.upgrade();
    needs_saving = true;
  }

  if (needs_saving) EepromSettings::save(settings);
}

void EepromSettings::save(Settings &settings) {
  void *addr = reinterpret_cast<void *>(SETTINGS_OFFSET);
  eeprom_write_block(&settings, addr, sizeof(Settings));
}

}  // namespace app
}  // namespace canio