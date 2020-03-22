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

namespace canio {
namespace event {

void Loop::post(const Event &event) { get().events.push(event); }

void Loop::remove(const Event &event) {
  auto &events = get().events;
  for (auto it = events.begin(); it != events.end();) {
    if (event == *it)
      events.erase(it++);
    else
      ++it;
  }
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
  for (auto it = events.begin(); it != events.end();) {
    // Process and erase events
    for (auto handler : handlers) handler->onEvent(*it);
    events.erase(it++);
  }
}

}  // namespace event
}  // namespace canio
