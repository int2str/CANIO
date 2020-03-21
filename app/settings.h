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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdint.h>

namespace canio {
namespace app {

class Settings {
 public:
  uint16_t marker;
  uint8_t revision;

  // Rev 1
  uint16_t can_baud_rate;
  uint16_t pulses_per_ml;

  Settings();

 private:
  void defaults();
  void upgrade();

  friend class EepromSettings;
};

class EepromSettings {
 public:
  static Settings load();
  static void save(Settings &settings);
};

}  // namespace app
}  // namespace canio

#endif  // SETTINGS_H