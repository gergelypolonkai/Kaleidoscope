/* -*- mode: c++ -*-
 * Kaleidoscope-LayerFocus -- Focus commands to work with layers
 * Copyright (C) 2020, 2021  Keyboard.io, Inc
 * Copyright (C) 2020  DygmaLab, SE.
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

#include "kaleidoscope/event_handler_result.h"  // for EventHandlerResult
#include "kaleidoscope/plugin.h"                // for Plugin

namespace kaleidoscope {
namespace plugin {

class LayerFocus : public kaleidoscope::Plugin {
 public:
  LayerFocus() {}

  EventHandlerResult onNameQuery();
  EventHandlerResult onFocusEvent(const char *command);
};

}  // namespace plugin
}  // namespace kaleidoscope

extern kaleidoscope::plugin::LayerFocus LayerFocus;
