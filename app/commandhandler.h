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

#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H

#include "device/fuelsensor.h"
#include "event/handler.h"

namespace canio {
namespace app {

class CommandHandler : public event::Handler {
  CommandHandler();

 public:
  static CommandHandler& init();

 private:
  void onCANReceived(uint8_t mob);
  void onUpdateFuel();

  void onEvent(const event::Event& event) override;

  device::FuelSensor fuel_sensor_;
  uint16_t pulses_per_ml_cached_;
};

}  // namespace app
}  // namespace canio

#endif