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

#ifndef EVENTS_H
#define EVENTS_H

// !!! EVENT NUMBERS MUST BE UNIQUE !!!
// !!! Must also fit inside uint8_t !!!

enum CANIO_EVENTS : uint8_t {
  EVENT_UPDATE = 0,

  EVENT_UPDATE_DRIVER_INPUTS,
  EVENT_UPDATE_FUEL_DATA,

  EVENT_TANK_SAMPLING_STARTED,
  EVENT_TANK_SAMPLING_COMPLETED,

  EVENT_LAST
};

#endif