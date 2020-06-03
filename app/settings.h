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

const uint8_t IO_COUNT = 4;

enum InputType : uint8_t {
  UNUSED = 0,
  ANALOG_IN = 1,
  FUEL_SENSOR = 2,
};

const uint16_t FLAG_ENABLE_PULLUP = 0x1000;  // For ADC only

struct Settings {
  uint16_t marker_start;
  uint8_t revision;

  // Rev 1 2.0
  uint16_t can_baud_rate;
  uint16_t can_base_id;

  uint8_t io_config[IO_COUNT];
  uint8_t io_params[IO_COUNT];

  uint16_t marker_end;
};

class EepromSettings {
 public:
  static void load(Settings &settings);
  static void save(Settings &settings);

 private:
  static void defaults(Settings &settings);
  static void upgrade(Settings &settings);
};

}  // namespace app
}  // namespace canio

#endif  // SETTINGS_H