/* -*- mode: c++ -*-
 * kaleidoscope::plugin::PersistentLEDMode -- Persist the current LED mode to Storage
 * Copyright (C) 2019-2022  Keyboard.io, Inc.
 * Copyright (C) 2019  Dygma, Inc.
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

#include <stdint.h>  // for uint16_t, uint8_t

#include "kaleidoscope/event_handler_result.h"  // for EventHandlerResult
#include "kaleidoscope/plugin.h"                // for Plugin

namespace kaleidoscope {
namespace plugin {

class PersistentLEDMode : public kaleidoscope::Plugin {
 public:
  PersistentLEDMode() {}

  EventHandlerResult onSetup();
  EventHandlerResult onNameQuery();
  EventHandlerResult onLEDModeChange();
  EventHandlerResult onFocusEvent(const char *command);

  void setAutoSave(bool state);
  bool getAutoSave() {
    return settings_.auto_save == 1;
  }

 private:
  static uint16_t settings_base_;
  static struct settings {
    uint8_t auto_save : 1;
    uint8_t default_mode_index : 7;
  } settings_;
};

}  // namespace plugin
}  // namespace kaleidoscope

extern kaleidoscope::plugin::PersistentLEDMode PersistentLEDMode;
