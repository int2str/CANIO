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

#ifndef CAN_H
#define CAN_H

#include <stdint.h>

constexpr uint16_t CAN_PROTOCOL_VERSION = 0x0103;

constexpr uint16_t CAN_BASE_ID = 0x4AE;
constexpr uint16_t CAN_BAUD_RATE = 500;

enum CanEvents : uint8_t {
  CAN_EVT_BOOT_COMPLETE = 0xE1,
};


#endif  // CAN_H