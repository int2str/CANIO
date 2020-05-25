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

#ifndef CANBUS_H
#define CANBUS_H

#include <stdint.h>

namespace canio {
namespace device {

struct CANmsg {
  uint16_t length;
  union {
    uint8_t data[8];
    uint16_t u16[4];
  };
} __attribute__((packed));

class CANbus {
  CANbus();

 public:
  static CANbus& get();

  void setBaudrate(uint16_t baud);

  void send(uint16_t id, CANmsg message);
  void registerReceiver(uint8_t mob, uint16_t id);

  bool hasMessage(uint8_t mob);
  CANmsg getMessage(uint8_t mob);
};

}  // namespace device
}  // namespace canio

#endif  // CANBUS_H
