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
constexpr uint32_t FUEL_UPDATE_DELAY_MS = 1'000;

canio::device::CANmsg makeEvent16(uint8_t evt, uint16_t data) {
  canio::device::CANmsg canmsg = {3, {0}};
  canmsg.data[0] = evt;
  canmsg.data[1] = (data >> 8) & 0xFF;
  canmsg.data[2] = data & 0xFF;
  return canmsg;
}

canio::device::CANmsg makeEvent32(uint8_t evt, uint32_t data) {
  canio::device::CANmsg canmsg = {5, {0}};
  canmsg.data[0] = evt;
  canmsg.data[1] = (data >> 24) & 0xFF;
  canmsg.data[2] = (data >> 16) & 0xFF;
  canmsg.data[3] = (data >> 8) & 0xFF;
  canmsg.data[4] = data & 0xFF;
  return canmsg;
}

uint16_t getUint16(const canio::device::CANmsg& msg, uint8_t offset) {
  return (msg.data[offset] << 8) + msg.data[offset + 1];
}

uint16_t getBaudrate() {
  canio::app::Settings settings = canio::app::EepromSettings::load();
  return settings.can_baud_rate;
}

void setBaudrate(uint16_t baud) {
  canio::app::Settings settings = canio::app::EepromSettings::load();
  settings.can_baud_rate = baud;
  canio::app::EepromSettings::save(settings);
}

uint16_t getFlowrate() {
  canio::app::Settings settings = canio::app::EepromSettings::load();
  return settings.pulses_per_ml;
}

void setFlowrate(uint16_t pulses_per_ml) {
  canio::app::Settings settings = canio::app::EepromSettings::load();
  settings.pulses_per_ml = pulses_per_ml;
  canio::app::EepromSettings::save(settings);
}

}  // namespace

namespace canio {
namespace app {

CommandHandler& CommandHandler::init() {
  static CommandHandler commandhandler;
  return commandhandler;
}

CommandHandler::CommandHandler() {
  device::CANbus& canbus = device::CANbus::get();
  canbus.setBaudrate(getBaudrate());

  canbus.send(CAN_ID_COMMANDS_OUT,
              makeEvent16(CAN_EVT_BOOT_COMPLETE, CAN_PROTOCOL_VERSION));

  canbus.registerReceiver(MOB_COMMAND_RX, CAN_ID_COMMANDS_IN);

  pulses_per_ml_cached_ = getFlowrate();
  fuel_sensor_.enableUpdates();

  adc_.enable();  // @@@ TODO: Check settings
}

void CommandHandler::onCANReceived(uint8_t mob) {
  device::CANbus& canbus = device::CANbus::get();
  device::CANmsg msg = canbus.getMessage(mob);
  if (msg.length < 1) return;

  switch (msg.data[0]) {
    case CAN_CMD_GET_BAUD:
      canbus.send(CAN_ID_COMMANDS_OUT,
                  makeEvent16(CAN_CMD_GET_BAUD, getBaudrate()));
      return;

    case CAN_CMD_SET_BAUD: {
      if (msg.length != 3) break;
      uint16_t baud = getUint16(msg, 1);
      setBaudrate(baud);
      canbus.send(CAN_ID_COMMANDS_OUT, makeEvent16(CAN_CMD_SET_BAUD, baud));
      return;
    }

    case CAN_CMD_GET_FLOWRATE:
      canbus.send(CAN_ID_COMMANDS_OUT,
                  makeEvent16(CAN_CMD_GET_FLOWRATE, getFlowrate()));
      return;

    case CAN_CMD_SET_FLOWRATE: {
      if (msg.length != 3) break;
      uint16_t rate = getUint16(msg, 1);
      setFlowrate(rate);
      pulses_per_ml_cached_ = rate;
      canbus.send(CAN_ID_COMMANDS_OUT, makeEvent16(CAN_CMD_SET_FLOWRATE, rate));
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
      canbus.send(CAN_ID_COMMANDS_OUT,
                  makeEvent16(CAN_EVT_ERROR, CAN_ERR_UNKOWN_CMD));
      return;
  }

  canbus.send(CAN_ID_COMMANDS_OUT,
              makeEvent16(CAN_EVT_ERROR, CAN_ERR_INVALID_PARAMETER));
}

void CommandHandler::onUpdateFuel() {
  uint32_t pulses = fuel_sensor_.get();
  if (pulses < pulses_per_ml_cached_) return;

  fuel_sensor_.reset();
  device::CANbus::get().send(
      CAN_ID_FUEL_FLOW,
      makeEvent16(CAN_EVT_FUEL_UPDATE, pulses / pulses_per_ml_cached_));
}

void CommandHandler::onUpdateAdc() {
  uint32_t adc = adc_.get();
  device::CANbus::get().send(CAN_ID_ADC, makeEvent32(CAN_EVT_ADC_UPDATE, adc));
}

void CommandHandler::update() {
  if (device::CANbus::get().hasMessage(MOB_COMMAND_RX))
    onCANReceived(MOB_COMMAND_RX);

  static uint16_t update_delay = 1;
  if (update_delay++ == 0) onUpdateFuel();

  static uint16_t adc_delay = 0;  // TODO: Needs more frequent updates
  if (adc_delay++ == 6000) {
    adc_delay = 0;
    onUpdateAdc();
  }
}

}  // namespace app
}  // namespace canio