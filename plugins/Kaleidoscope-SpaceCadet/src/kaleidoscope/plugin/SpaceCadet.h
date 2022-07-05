/* -*- mode: c++ -*-
 * Kaleidoscope-SpaceCadet -- Space Cadet Shift Extended
 * Copyright (C) 2016, 2017, 2018  Keyboard.io, Inc, Ben Gemperline
 * Copyright (C) 2019-2021  Keyboard.io, Inc
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <Kaleidoscope-Ranges.h>  // for SC_FIRST, SC_LAST
#include <stdint.h>               // for uint16_t, uint8_t, int8_t

#include "kaleidoscope/KeyAddrEventQueue.h"     // for KeyAddrEventQueue
#include "kaleidoscope/KeyEvent.h"              // for KeyEvent
#include "kaleidoscope/KeyEventTracker.h"       // for KeyEventTracker
#include "kaleidoscope/event_handler_result.h"  // for EventHandlerResult
#include "kaleidoscope/key_defs.h"              // for Key, Key_NoKey
#include "kaleidoscope/plugin.h"                // for Plugin

#ifndef SPACECADET_MAP_END
#define SPACECADET_MAP_END                         \
  (kaleidoscope::plugin::SpaceCadet::KeyBinding) { \
    Key_NoKey, Key_NoKey, 0                        \
  }
#endif

constexpr Key Key_SpaceCadetEnable  = Key(kaleidoscope::ranges::SC_FIRST);
constexpr Key Key_SpaceCadetDisable = Key(kaleidoscope::ranges::SC_LAST);

namespace kaleidoscope {
namespace plugin {

class SpaceCadet : public kaleidoscope::Plugin {
 public:
  // Internal Class
  // Declarations for the modifier key mapping
  class KeyBinding {
   public:
    // Empty constructor; set the vars separately
    KeyBinding() {}
    // Constructor with input and output
    KeyBinding(Key input, Key output);
    // Constructor with all three set
    KeyBinding(Key input, Key output, uint16_t timeout);
    // The key that is pressed
    Key input;
    // the key that is sent
    Key output;
    // The timeout (default to global timeout)
    uint16_t timeout = 0;
    // to check for the end of a list (SPACECADET_MAP_END)
    bool isEmpty() const {
      return (input == Key_NoKey && output == Key_NoKey && timeout == 0);
    }
  };

  SpaceCadet(void);

  // Methods
  static void enable() {
    mode_ = Mode::ON;
  }
  static void disable() {
    mode_ = Mode::OFF;
  }
  static void enableWithoutDelay() {
    mode_ = Mode::NO_DELAY;
  }
  static bool active() {
    return (mode_ == Mode::ON || mode_ == Mode::NO_DELAY);
  }

  // Publically accessible variables
  static uint16_t time_out;            //  The global timeout in milliseconds
  static SpaceCadet::KeyBinding *map;  // The map of key bindings

  EventHandlerResult onNameQuery();
  EventHandlerResult onKeyswitchEvent(KeyEvent &event);
  EventHandlerResult afterEachCycle();

 private:
  enum Mode : uint8_t {
    ON,
    OFF,
    NO_DELAY,
  };
  static uint8_t mode_;

  static KeyEventTracker event_tracker_;

  // The maximum number of events in the queue at a time.
  static constexpr uint8_t queue_capacity_{4};

  // The event queue stores a series of press and release events.
  KeyAddrEventQueue<queue_capacity_> event_queue_;

  static int8_t pending_map_index_;

  int8_t getSpaceCadetKeyIndex(Key key) const;

  void flushEvent(bool is_tap = false);
  void flushQueue();
};

}  // namespace plugin
}  // namespace kaleidoscope

extern kaleidoscope::plugin::SpaceCadet SpaceCadet;
