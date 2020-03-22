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

#include <avr/interrupt.h>
#include <avr/io.h>

#include "canbus.h"  // Must be after <avr/interrupt.h>!

#include "event/loop.h"
#include "events.h"

ISR(CAN_INT_vect) {
  CANPAGE = (CANHPMOB & 0xF0);

  if (CANSTMOB & (1 << TXOK)) {
    CANSTMOB = 0;
    canio::device::CANbus::get().onTxComplete(CANPAGE >> 4, 0);
  }

  else if (CANSTMOB & (1 << RXOK)) {
    CANSTMOB = 0;

    canio::device::CANmsg msg;
    msg.length = (CANCDMOB & 0xF);
    for (uint8_t i = 0; i != msg.length; ++i) {
      msg.data[i] = CANMSG;
    }

    canio::device::CANbus::get().onRxComplete(CANPAGE >> 4, msg);
  }

  // @@@ TODO: Handle errors
}

namespace {

void enableCAN() {
  CANGCON = (1 << ENASTB);
  // Endless loop until CAN bus is ready.
  // In case the CAN bus has issues, this should trigger
  // a watchdog timeout.
  while ((CANGSTA & (1 << ENFG)) == 0) {
  }
}

void resetCAN() { CANGCON = (1 << SWRES); }

// Relevant values taken from datasheet section:
// "20.10. Examples of CAN baud rate setting"
void setBaudrateRegisters(uint16_t kbit) {
  switch (kbit) {
    default:
    case 1000:  // 16x Tscl, sampling at 69%
      CANBT1 = 0x00;
      break;

    case 500:  // 16x Tscl, sampling at 75%
      CANBT1 = 0x02;
      break;

    case 250:  // 16x Tscl, sampling at 75%
      CANBT1 = 0x06;
      break;

    case 100:  // 16x Tscl, sampling at 75%
      CANBT1 = 0x12;
      break;
  }

  CANBT2 = 0x0C;
  CANBT3 = 0x36;
}

void selectPage(uint8_t page) { CANPAGE = (page << 4); }

void resetMOb() {
  CANSTMOB = 0;
  CANCDMOB = 0;
}

void resetMObs() {
  for (uint8_t page = 0; page != 6; ++page) {
    selectPage(page);
    resetMOb();
  }
}

void setID(uint16_t id) {
  CANIDT1 = (id >> 3) & 0xFF;
  CANIDT2 = (id << 5);
  CANIDT3 = 0;
  CANIDM4 = 0;
}

void setMask(uint16_t id) {
  CANIDM1 = (id >> 3) & 0xFF;
  CANIDM2 = (id << 5);
  CANIDM3 = 0;
  CANIDM4 = (id << IDMSK0);
}

void setData(uint8_t length, const uint8_t* data) {
  CANCDMOB = (CANCDMOB & 0xF0) | length;
  for (auto i = 0; i != length; ++i) {
    CANMSG = data[i];
  }
}

template <typename T>
void setData(const T& n) {
  setData(sizeof(T), reinterpret_cast<const uint8_t*>(&n));
}

void setTxEnabled() { CANCDMOB = (CANCDMOB & 0x0F) | (1 << CONMOB0); }

void setRxEnabled() { CANCDMOB = (1 << CONMOB1); }

void enableGlobalInterrupts() {
  CANGIE = (1 << ENIT) | (1 << ENRX) | (1 << ENTX);
}

void enableMobIrq(uint8_t mob) { CANIE2 |= (1 << mob); }

}  // namespace

namespace canio {
namespace device {

CANbus& CANbus::get() {
  static CANbus canbus;
  return canbus;
}

CANbus::CANbus() {
  resetCAN();
  setBaudrate(1000);
  resetMObs();

  enableGlobalInterrupts();
  enableCAN();
}

void CANbus::setBaudrate(uint16_t baud) { setBaudrateRegisters(baud); }

void CANbus::send(uint16_t id, CANmsg message) {
  selectPage(0);
  resetMOb();
  setID(id);
  setData(message.length, message.data);
  enableMobIrq(0);  // Enable MOb 0 interrupt for TX
  setTxEnabled();
}

void CANbus::registerReceiver(uint8_t mob, uint16_t id) {
  selectPage(mob);
  resetMOb();
  setID(id);
  setMask(0x7FF);

  enableMobIrq(mob);
  setRxEnabled();
}

CANmsg CANbus::getMessage(uint8_t mob) {
  if (mob < 1 || mob > 5) return CANmsg{0, 0};
  return cache_[mob - 1];
}

void CANbus::onTxComplete(uint8_t, uint8_t) {}

void CANbus::onRxComplete(uint8_t mob, const CANmsg& msg) {
  cache_[mob - 1] = msg;
  event::Loop::postPending(event::Event(EVENT_CAN_RX, mob));
  resetMOb();
  setRxEnabled();
}

}  // namespace device
}  // namespace canio
