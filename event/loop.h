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

#ifndef LOOP_H
#define LOOP_H

#include "event/handler.h"
#include "utils/cpp.h"
#include "utils/queue.h"

#include "events.h"

namespace canio {
namespace event {

// Basic event loop driver.
// |dispatch| must be called regularly in order to distribute events.
// This is a singleton; there shall be only one.
class Loop {
  struct Evt {
    bool active;
    uint32_t delay;
    uint32_t posted;
  };

  Loop();

 public:
  // Posts to all registered handlers on the next call to |dispatch|
  static void post(uint8_t id);

  // Posts to all registered handlers when |dispatch| is called after
  // |ms| have elapsed
  static void postDelayed(uint8_t id, const uint32_t ms);

  // Remove an |event| from the event loop
  // Should be most helpful to remove delayed event before they are
  // fired.
  static void remove(uint8_t id);

  // Must be called regularly to dispatch events
  static void dispatch();

  // "Handler" sub-classes do this automatically...
  static void addHandler(Handler *ph);

  static void removeHandler(Handler *ph);

 protected:
  friend class Handler;

 private:
  static Loop &get();
  void dispatch_impl();

  Evt events_[EVENT_LAST];
  utils::Queue<Handler *> handlers;

  DISALLOW_COPY_AND_ASSIGN(Loop);
};

}
}

#endif  // LOOP_H