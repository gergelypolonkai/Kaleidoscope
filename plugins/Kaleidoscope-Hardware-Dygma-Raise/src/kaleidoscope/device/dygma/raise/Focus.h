/* -*- mode: c++ -*-
 * kaleidoscope::device::dygma::Raise -- Kaleidoscope device plugin for Dygma Raise
 * Copyright (C) 2017-2019  Keyboard.io, Inc
 * Copyright (C) 2017-2019  Dygma Lab S.L.
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

#ifdef ARDUINO_SAMD_RAISE

#include "kaleidoscope/plugin.h"

namespace kaleidoscope {
namespace device {
namespace dygma {
namespace raise {

class Focus : public kaleidoscope::Plugin {
 public:
  EventHandlerResult onFocusEvent(const char *command);
};

}  // namespace raise
}  // namespace dygma
}  // namespace device
}  // namespace kaleidoscope

extern kaleidoscope::device::dygma::raise::Focus RaiseFocus;

#endif
