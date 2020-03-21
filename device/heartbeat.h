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

#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include "event/handler.h"
#include "utils/cpp.h"

namespace canio {
namespace device {

// Simple pulse generator used for debugging/benchmarking.
// Toggles SCK_A pin.
class Heartbeat : public event::Handler {
  Heartbeat();

 public:
  static Heartbeat& init();

 private:
  void onEvent(const event::Event& event) override;

  DISALLOW_COPY_AND_ASSIGN(Heartbeat);
};

}  // namespace device
}  // namespace canio

#endif  // HEARTBEAT_H
