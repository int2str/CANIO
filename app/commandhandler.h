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

#include "fuellevel.h"
#include "device/adc.h"
#include "device/fuelsensor.h"
#include "device/led.h"

namespace canio {
namespace app {

class CommandHandler {
  CommandHandler();

 public:
  static CommandHandler& init();
  void update();

 private:
  void onCANReceived(uint8_t mob);
  void onUpdateValues();

  device::Adc adc_;
  device::FuelSensor fuel_sensor_;
  device::Led led_;
  FuelLevel fuel_level_;
};

}  // namespace app
}  // namespace canio

#endif  // COMMANDHANDLER_H