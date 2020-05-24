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

#include "commandhandler.h"

#include "app/settings.h"
#include "can.h"
#include "device/canbus.h"
#include "system/watchdog.h"

namespace {

constexpr uint8_t MOB_COMMAND_RX = 1;

canio::device::CANmsg makeEvent16(uint8_t evt, uint16_t data) {
  canio::device::CANmsg canmsg = {3, {0}};
  canmsg.data[0] = evt;
  canmsg.data[1] = (data >> 8) & 0xFF;
  canmsg.data[2] = data & 0xFF;
  return canmsg;
}

uint16_t getUint16(const canio::device::CANmsg& msg, uint8_t offset) {
  return (msg.data[offset] << 8) + msg.data[offset + 1];
}

uint8_t getEnabledBitMask(uint8_t *array, uint8_t match) {
  uint8_t mask = 0;
  for (uint8_t i = 0; i != 4; ++i) {
    if (array[i] == match) mask |= (1 << i);
  }
  return mask;
}

}  // namespace

namespace canio {
namespace app {

CommandHandler& CommandHandler::init() {
  static CommandHandler commandhandler;
  return commandhandler;
}

CommandHandler::CommandHandler() : updates_enabled_(1) {
  EepromSettings::load(settings_);

  device::CANbus& canbus = device::CANbus::get();
  canbus.setBaudrate(settings_.can_baud_rate);
  canbus.registerReceiver(MOB_COMMAND_RX, settings_.can_base_id);

  canbus.send(settings_.can_base_id + 1,
              makeEvent16(CAN_EVT_BOOT_COMPLETE, CAN_PROTOCOL_VERSION));

  fuel_sensor_.enable(getEnabledBitMask(settings_.io_config, FUEL_SENSOR), settings_.io_params);
  adc_.enable(getEnabledBitMask(settings_.io_config, ANALOG_IN));
}

void CommandHandler::onCANReceived(uint8_t mob) {
  device::CANbus& canbus = device::CANbus::get();
  device::CANmsg msg = canbus.getMessage(mob);
  if (msg.length < 1) return;

  switch (msg.data[0]) {
    case CAN_CMD_ENABLE_UPDATES:
      if (msg.length != 2) break;
      updates_enabled_ = msg.data[1];
      return;

    case CAN_CMD_GET_BAUD:
      canbus.send(settings_.can_base_id + 1,
                  makeEvent16(CAN_CMD_GET_BAUD, settings_.can_baud_rate));
      return;

    case CAN_CMD_SET_BAUD: {
      if (msg.length != 3) break;
      settings_.can_baud_rate = getUint16(msg, 1);
      EepromSettings::save(settings_);
      canbus.send(settings_.can_base_id + 1, msg);
      return;
    }

    case CAN_CMD_GET_BASE_ID: {
      canbus.send(settings_.can_base_id + 1,
                  makeEvent16(CAN_CMD_GET_BASE_ID, settings_.can_base_id));
      return;
    }

    case CAN_CMD_SET_BASE_ID: {
      if (msg.length != 3) break;
      settings_.can_base_id = getUint16(msg, 1);
      EepromSettings::save(settings_);
      canbus.send(settings_.can_base_id + 1, msg);
      return;
    }

    case CAN_CMD_GET_IO_CONFIG: {
      canio::device::CANmsg ret = {5, {0}};
      ret.data[0] = CAN_CMD_GET_IO_CONFIG;
      for (uint8_t i = 0; i != 4; ++i) ret.data[1 + i] = settings_.io_config[i];
      canbus.send(settings_.can_base_id + 1, ret);
      return;
    }

    case CAN_CMD_SET_IO_CONFIG: {
      if (msg.length != 5) break;
      for (uint8_t i = 0; i != 4; ++i) settings_.io_config[i] = msg.data[1 + i];
      EepromSettings::save(settings_);
      canbus.send(settings_.can_base_id + 1, msg);
      return;
    }

    case CAN_CMD_GET_IO_PARAMS: {
      canio::device::CANmsg ret = {5, {0}};
      ret.data[0] = CAN_CMD_GET_IO_PARAMS;
      for (uint8_t i = 0; i != 4; ++i) ret.data[1 + i] = settings_.io_params[i];
      canbus.send(settings_.can_base_id + 1, ret);
      return;
    }

    case CAN_CMD_SET_IO_PARAMS: {
      if (msg.length != 5) break;
      for (uint8_t i = 0; i != 4; ++i) settings_.io_params[i] = msg.data[1 + i];
      EepromSettings::save(settings_);
      canbus.send(settings_.can_base_id + 1, msg);
      return;
    }

    case CAN_CMD_RESET: {
      if (msg.length != 3) break;
      uint16_t confirm = getUint16(msg, 1);
      if (confirm != CAN_RESET_CONFIRM) break;
      system::Watchdog::forceRestart();
      return;
    }

    default:
      canbus.send(settings_.can_base_id + 1,
                  makeEvent16(CAN_EVT_ERROR, CAN_ERR_UNKOWN_CMD));
      return;
  }

  canbus.send(settings_.can_base_id + 1,
              makeEvent16(CAN_EVT_ERROR, CAN_ERR_INVALID_PARAMETER));
}

void CommandHandler::onUpdateValues() {
  if (!updates_enabled_) return;
  device::CANmsg canmsg = {4, {0}};
  adc_.get(canmsg.data);
  fuel_sensor_.get(canmsg.data);
  device::CANbus::get().send(settings_.can_base_id, canmsg);
}

void CommandHandler::update() {
  if (device::CANbus::get().hasMessage(MOB_COMMAND_RX))
    onCANReceived(MOB_COMMAND_RX);

  // A value of 6000 here roughly equates to 50ms between updates or a 20Hz
  // update rate. Note that this is not absolute as timers are disabled and this
  // depends on the frequency at which the main update loop calls this update()
  // function. If more stability/accuracy is required here, switch to actual
  // timers...
  static uint16_t update_delay = 0;
  if (update_delay++ == 6000) {
    update_delay = 0;
    onUpdateValues();
  }
}

}  // namespace app
}  // namespace canio