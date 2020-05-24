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

constexpr uint16_t CAN_PROTOCOL_VERSION = 0x0100;

enum CanEvents : uint8_t {
  CAN_EVT_BOOT_COMPLETE = 0xE1,
  CAN_EVT_ERROR = 0xEE,
};

enum CanCommands : uint8_t {
  CAN_CMD_ENABLE_UPDATES = 0x01,
  CAN_CMD_GET_BAUD = 0x02,
  CAN_CMD_SET_BAUD = 0x03,
  CAN_CMD_GET_BASE_ID = 0x04,
  CAN_CMD_SET_BASE_ID = 0x05,
  CAN_CMD_GET_IO_CONFIG = 0x06,
  CAN_CMD_SET_IO_CONFIG = 0x07,
  CAN_CMD_GET_IO_PARAMS = 0x08,
  CAN_CMD_SET_IO_PARAMS = 0x09,
  CAN_CMD_RESET = 0xFE
};

enum CanErrors : uint8_t {
  CAN_ERR_UNKOWN_CMD = 0x01,
  CAN_ERR_INVALID_PARAMETER = 0x02,
};

constexpr uint16_t CAN_RESET_CONFIRM = 0xDEAD;


#endif  // CAN_H