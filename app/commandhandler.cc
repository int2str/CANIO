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

#include "device/canbus.h"
#include "system/watchdog.h"
#include "utils/byteorder.h"

#include "can.h"
#include "events.h"

namespace {

constexpr uint8_t MOB_COMMAND_RX = 0;
constexpr uint8_t MOB_COMMAND_TX = 1;
constexpr uint8_t MOB_DRIVER_INPUTS_TX = 2;
constexpr uint8_t MOB_FUEL_DATA_TX = 3;

// A value of 6'000 here roughly equates to 50ms between updates or a 20Hz
// update rate. Note that this is not absolute as timers are disabled and this
// depends on the frequency at which the main update loop calls this update()
// function. If more stability/accuracy is required here, switch to actual
// timers...
constexpr uint16_t UPDATED_EVERY_N_LOOPS = 6'000;

// Defined in the PDM config...
constexpr uint16_t CAN_KEYPAD_EVENT_ID = 0x520;

canio::device::CANmsg makeEvent16(uint8_t evt, uint16_t data) {
  canio::device::CANmsg canmsg = {3, {0}};
  canmsg.u8[0] = evt;
  canmsg.u8[1] = (data >> 8) & 0xFF;
  canmsg.u8[2] = data & 0xFF;
  return canmsg;
}

void welcomeBlink(canio::device::Led& led) {
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

CommandHandler::CommandHandler() : fuel_used_total_ml_{0} {
  welcomeBlink(led_);

  device::CANbus& canbus = device::CANbus::get();
  canbus.setBaudrate(CAN_BAUD_RATE);
  canbus.registerReceiver(MOB_COMMAND_RX, CAN_KEYPAD_EVENT_ID);

  canbus.send(MOB_COMMAND_TX, CAN_BASE_ID,
              makeEvent16(CAN_EVT_BOOT_COMPLETE, CAN_PROTOCOL_VERSION));

  fuel_sensor_.enable(0x40);
  adc_.enable(0x3E);
}

void CommandHandler::updateDriverInputs() {
  device::CANmsg canmsg1 = {8, {0}};
  canmsg1.u16[0] = utils::lsb_to_msb(adc_.get(1));
  canmsg1.u16[1] = utils::lsb_to_msb(adc_.get(2));
  canmsg1.u16[2] = utils::lsb_to_msb(adc_.get(3));
  canmsg1.u16[3] = utils::lsb_to_msb(adc_.get(5));
  device::CANbus::get().send(MOB_DRIVER_INPUTS_TX, CAN_BASE_ID + 1, canmsg1);
}

void CommandHandler::updateFuel() {
  uint16_t tank_sensor = adc_.get(4);
  uint16_t fuel_used_ml = fuel_sensor_.getMl(6);
  uint16_t fuel_level_ml =
      fuel_level_.recalculate(tank_sensor, fuel_used_ml);
  fuel_used_total_ml_ += fuel_used_ml;

  device::CANmsg canmsg2 = {8, {0}};
  canmsg2.u16[0] = utils::lsb_to_msb(tank_sensor);
  canmsg2.u16[1] = utils::lsb_to_msb(fuel_level_ml);
  canmsg2.u32[1] = utils::lsb_to_msb(fuel_used_total_ml_);
  device::CANbus::get().send(MOB_FUEL_DATA_TX, CAN_BASE_ID + 2, canmsg2);
}

void CommandHandler::onCANReceived(uint8_t mob) {
  device::CANbus& canbus = device::CANbus::get();
  device::CANmsg msg = canbus.getMessage(mob);

  if (msg.length < 5) return;
  uint8_t fuel_reset_button_pressed = msg.u8[4];
  if (fuel_reset_button_pressed && fuel_level_.initialSamplesCollected()) {
    fuel_level_.reset();
  }
}

void CommandHandler::onUpdateValues() {
  updateDriverInputs();
  updateFuel();
}

void CommandHandler::onEvent(const event::Event &event) {
  if (event.id != EVENT_UPDATE) return;
  led_.update();

  if (device::CANbus::get().hasMessage(MOB_COMMAND_RX)) {
    onCANReceived(MOB_COMMAND_RX);
  }

  static uint16_t update_delay = 0;
  if (update_delay++ == UPDATED_EVERY_N_LOOPS) {
    update_delay = 0;
    onUpdateValues();
  }
}

}  // namespace app
}  // namespace canio