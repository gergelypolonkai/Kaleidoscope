/* -*- mode: c++ -*-
 * Kaleidoscope-Syster -- Symbolic input system
 * Copyright (C) 2017-2021  Keyboard.io, Inc
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

#include <Kaleidoscope-Ranges.h>  // for SYSTER
#include <stdint.h>               // for int8_t, uint8_t

#include "kaleidoscope/KeyEvent.h"              // for KeyEvent
#include "kaleidoscope/event_handler_result.h"  // for EventHandlerResult
#include "kaleidoscope/key_defs.h"              // for Key
#include "kaleidoscope/plugin.h"                // for Plugin

#define SYSTER_MAX_SYMBOL_LENGTH 32

constexpr Key Key_Syster = Key(kaleidoscope::ranges::SYSTER);
constexpr Key SYSTER     = Key_Syster;

namespace kaleidoscope {
namespace plugin {

class Syster : public kaleidoscope::Plugin {
 public:
  typedef enum {
    StartAction,
    EndAction,
    SymbolAction
  } action_t;

  Syster() {}

  static void reset();

  bool is_active();

  EventHandlerResult onNameQuery();
  EventHandlerResult onKeyEvent(KeyEvent &event);

 private:
  static char symbol_[SYSTER_MAX_SYMBOL_LENGTH + 1];
  static uint8_t symbol_pos_;
  static bool is_active_;
};

}  // namespace plugin

void eraseChars(int8_t n);

}  // namespace kaleidoscope

const char keyToChar(Key key);

void systerAction(kaleidoscope::plugin::Syster::action_t action, const char *symbol);

extern kaleidoscope::plugin::Syster Syster;
