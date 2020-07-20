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

#include "event/loop.h"

#include "system/timer.h"

namespace canio {
namespace event {

void Loop::post(uint8_t id) {
  get().events_[id]  = {true, 0, 0};
}

void Loop::postDelayed(uint8_t id, const uint32_t ms) {
  get().events_[id] = {true, ms, system::Timer::millis()};
}

void Loop::remove(uint8_t id) {
  get().events_[id] = {false, 0, 0};
}

void Loop::addHandler(Handler *ph) { get().handlers.push(ph); }

void Loop::removeHandler(Handler *ph) {
  auto &handlers = get().handlers;
  for (auto it = handlers.begin(); it != handlers.end(); ++it) {
    if (*it == ph) {
      handlers.erase(it);
      break;
    }
  }
}

void Loop::dispatch() { get().dispatch_impl(); }

Loop &Loop::get() {
  static Loop loop;
  return loop;
}

Loop::Loop() {}

void Loop::dispatch_impl() {
  for (uint8_t i = 0; i != EVENT_LAST; ++i) {
    if (!events_[i].active) continue;

    // Process and erase events
    if (events_[i].delay == 0 ||
        ((system::Timer::millis() - events_[i].posted) >= events_[i].delay)) {
      events_[i] = {false, 0, 0};
      for (auto handler : handlers) handler->onEvent(i);
    }
  }
}

}  // namespace event
}  // namespace canio
