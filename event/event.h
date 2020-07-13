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

#pragma once

#include <stdbool.h>
#include <stdint.h>

namespace canio {
namespace event {

class Event {
 public:
  Event(const uint8_t id, const uint8_t param = 0);
  Event() = delete;

  bool operator==(const Event &rhs) const;

  const uint8_t id;
  const uint8_t param;
  uint32_t delay;
  uint32_t posted;
};

}  // namespace event
}  // namespace canio
