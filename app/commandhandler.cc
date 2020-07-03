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

#include <util/delay.h>

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

void welcomeBlink(canio::device::Led &led) {
  uint8_t blink = 3;
  while (blink--) {
    led.on();
    _delay_ms(150);
    led.off();
    _delay_ms(150);
  }
}

}  // namespace

namespace canio {
namespace app {

CommandHandler& CommandHandler::init() {
  static CommandHandler commandhandler;
  return commandhandler;
}

CommandHandler::CommandHandler() : updates_enabled_(1) {
  welcomeBlink(led_);

  device::CANbus& canbus = device::CANbus::get();
  canbus.setBaudrate(CAN_BAUD_RATE);
  canbus.registerReceiver(MOB_COMMAND_RX, CAN_BASE_ID);

  canbus.send(CAN_BASE_ID,
              makeEvent16(CAN_EVT_BOOT_COMPLETE, CAN_PROTOCOL_VERSION));

  fuel_sensor_.enable();
  adc_.enable(0x3F);
}

void CommandHandler::onCANReceived(uint8_t mob) {
  device::CANbus& canbus = device::CANbus::get();
  device::CANmsg msg = canbus.getMessage(mob);
  if (msg.length < 1) return;
  led_.timedOn(12'000);

  switch (msg.data[0]) {
    case CAN_CMD_ENABLE_UPDATES:
      if (msg.length != 2) break;
      updates_enabled_ = msg.data[1];
      return;

    case CAN_CMD_RESET: {
      if (msg.length != 3) break;
      if (getUint16(msg, 1) != CAN_RESET_CONFIRM) break;
      system::Watchdog::forceRestart();
      return;
    }

    default:
      canbus.send(CAN_BASE_ID+1, makeEvent16(CAN_EVT_ERROR, CAN_ERR_UNKOWN_CMD));
      return;
  }

  canbus.send(CAN_BASE_ID+1, makeEvent16(CAN_EVT_ERROR, CAN_ERR_INVALID_PARAMETER));
}

void CommandHandler::onUpdateValues() {
  if (!updates_enabled_) return;
  device::CANmsg canmsg = {8, {0}};
  adc_.get(canmsg.u16);
  fuel_sensor_.get(canmsg.u16);
  device::CANbus::get().send(CAN_BASE_ID, canmsg);
}

void CommandHandler::update() {
  led_.update();

  if (device::CANbus::get().hasMessage(MOB_COMMAND_RX)) {
    onCANReceived(MOB_COMMAND_RX);
  }

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