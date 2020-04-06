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

constexpr uint16_t CAN_ID_COMMANDS_IN = 0x4AF;
constexpr uint16_t CAN_ID_COMMANDS_OUT = 0x4AD;

constexpr uint16_t CAN_ID_FUEL_FLOW = 0x4AE;
constexpr uint8_t CAN_EVT_FUEL_UPDATE = 0xE2;

constexpr uint16_t CAN_ID_ADC = 0x4AC;
constexpr uint8_t CAN_EVT_ADC_UPDATE = 0xE3;

constexpr uint8_t CAN_EVT_BOOT_COMPLETE = 0xE1;

constexpr uint8_t CAN_CMD_GET_BAUD = 0x02;
constexpr uint8_t CAN_CMD_SET_BAUD = 0x03;

constexpr uint8_t CAN_CMD_GET_FLOWRATE = 0x04;
constexpr uint8_t CAN_CMD_SET_FLOWRATE = 0x05;

constexpr uint8_t CAN_CMD_RESET = 0xFE;
constexpr uint16_t CAN_RESET_CONFIRM = 0xDEAD;

constexpr uint8_t CAN_EVT_ERROR = 0xEE;
constexpr uint8_t CAN_ERR_UNKOWN_CMD = 0x01;
constexpr uint8_t CAN_ERR_INVALID_PARAMETER = 0x02;

#endif  // CAN_H