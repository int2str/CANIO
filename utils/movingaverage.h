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

#ifndef MOVINGAVERAGE_H
#define MOVINGAVERAGE_H

#include <stdlib.h>

namespace canio {
namespace utils {

template <class T, size_t WINDOW>
class MovingAverage {
 public:
  MovingAverage() : buffer{0}, index(0), sum(0) {}

  void push(const T value) {
    sum -= buffer[index];
    sum += value;
    buffer[index] = value;
    if (++index == WINDOW) index = 0;
  }

  T get() const { return sum / WINDOW; }

  void clear() {
    sum = 0;
    index = 0;
    for (auto &i : buffer) i = 0;
  }

 private:
  T buffer[WINDOW];
  size_t index;
  uint32_t sum;
};

}  // namespace utils
}  // namespace canio

#endif  // MOVINGAVERAGE_H