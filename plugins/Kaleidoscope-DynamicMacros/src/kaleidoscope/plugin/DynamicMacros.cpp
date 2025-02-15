/* DynamicMacros - Dynamic macro support for Kaleidoscope.
 * Copyright (C) 2019-2022  Keyboard.io, Inc.
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

#include "kaleidoscope/plugin/DynamicMacros.h"

#include <Arduino.h>                   // for delay, PSTR, strcmp_P, F, __FlashStri...
#include <Kaleidoscope-FocusSerial.h>  // for Focus, FocusSerial
#include <Kaleidoscope-Ranges.h>       // for DYNAMIC_MACRO_FIRST, DYNAMIC_MACRO_LAST

#include "kaleidoscope/KeyAddr.h"                 // for KeyAddr
#include "kaleidoscope/KeyEvent.h"                // for KeyEvent
#include "kaleidoscope/Runtime.h"                 // for Runtime, Runtime_
#include "kaleidoscope/device/device.h"           // for VirtualProps::Storage, Base<>::Storage
#include "kaleidoscope/keyswitch_state.h"         // for INJECTED, IS_PRESSED, WAS_PRESSED
#include "kaleidoscope/plugin/EEPROM-Settings.h"  // for EEPROMSettings
// This is a special exception to the rule of only including a plugin's
// top-level header file, because DynamicMacros doesn't depend on the Macros
// plugin itself; it's just using the same macro step definitions.
#include "kaleidoscope/plugin/Macros/MacroSteps.h"  // for MACRO_ACTION_END, MACRO_ACTION_STEP_E...

namespace kaleidoscope {
namespace plugin {

uint16_t DynamicMacros::storage_base_;
uint16_t DynamicMacros::storage_size_;
uint16_t DynamicMacros::map_[];
uint8_t DynamicMacros::macro_count_;
Key DynamicMacros::active_macro_keys_[];

// =============================================================================
// It might be possible to use Macros instead of reproducing it
void DynamicMacros::press(Key key) {
  Runtime.handleKeyEvent(KeyEvent(KeyAddr::none(), IS_PRESSED | INJECTED, key));
  for (Key &mkey : active_macro_keys_) {
    if (mkey == Key_NoKey) {
      mkey = key;
      break;
    }
  }
}

void DynamicMacros::release(Key key) {
  for (Key &mkey : active_macro_keys_) {
    if (mkey == key) {
      mkey = Key_NoKey;
    }
  }
  Runtime.handleKeyEvent(KeyEvent(KeyAddr::none(), WAS_PRESSED | INJECTED, key));
}

void DynamicMacros::tap(Key key) {
  Runtime.handleKeyEvent(KeyEvent(KeyAddr::none(), IS_PRESSED | INJECTED, key));
  Runtime.handleKeyEvent(KeyEvent(KeyAddr::none(), WAS_PRESSED | INJECTED, key));
}

uint8_t DynamicMacros::updateDynamicMacroCache() {
  uint16_t pos       = storage_base_;
  uint8_t current_id = 0;
  macro_t macro      = MACRO_ACTION_END;

  map_[0] = 0;

  while (pos < storage_base_ + storage_size_) {
    macro = Runtime.storage().read(pos++);
    switch (macro) {
    case MACRO_ACTION_STEP_EXPLICIT_REPORT:
    case MACRO_ACTION_STEP_IMPLICIT_REPORT:
    case MACRO_ACTION_STEP_SEND_REPORT:
      break;

    case MACRO_ACTION_STEP_INTERVAL:
    case MACRO_ACTION_STEP_WAIT:
    case MACRO_ACTION_STEP_KEYCODEDOWN:
    case MACRO_ACTION_STEP_KEYCODEUP:
    case MACRO_ACTION_STEP_TAPCODE:
      pos++;
      break;

    case MACRO_ACTION_STEP_KEYDOWN:
    case MACRO_ACTION_STEP_KEYUP:
    case MACRO_ACTION_STEP_TAP:
      pos += 2;
      break;

    case MACRO_ACTION_STEP_TAP_SEQUENCE: {
      uint8_t keyCode, flags;
      do {
        flags   = Runtime.storage().read(pos++);
        keyCode = Runtime.storage().read(pos++);
      } while (!(flags == 0 && keyCode == 0));
      break;
    }

    case MACRO_ACTION_STEP_TAP_CODE_SEQUENCE: {
      uint8_t keyCode, flags;
      do {
        keyCode = Runtime.storage().read(pos++);
      } while (keyCode != 0);
      break;
    }

    case MACRO_ACTION_END:
      map_[++current_id] = pos - storage_base_;
      break;

    default:
      // When we encounter an unknown step type, stop processing. Whatever we
      // encounter after is unknown, and there's no guarantee we can parse it
      // properly.
      return current_id;
    }
  }

  return current_id;
}

// public
void DynamicMacros::play(uint8_t macro_id) {
  macro_t macro    = MACRO_ACTION_END;
  uint8_t interval = 0;
  uint16_t pos;
  Key key;

  // If the requested ID is higher than the number of macros we found during the
  // cache update, bail out. Our map beyond `macro_count_` is unreliable.
  if (macro_id >= macro_count_)
    return;

  pos = storage_base_ + map_[macro_id];

  while (pos < storage_base_ + storage_size_) {
    switch (macro = Runtime.storage().read(pos++)) {
    case MACRO_ACTION_STEP_EXPLICIT_REPORT:
    case MACRO_ACTION_STEP_IMPLICIT_REPORT:
    case MACRO_ACTION_STEP_SEND_REPORT:
      break;

    case MACRO_ACTION_STEP_INTERVAL:
      interval = Runtime.storage().read(pos++);
      break;
    case MACRO_ACTION_STEP_WAIT: {
      uint8_t wait = Runtime.storage().read(pos++);
      delay(wait);
      break;
    }

    case MACRO_ACTION_STEP_KEYDOWN:
      key.setFlags(Runtime.storage().read(pos++));
      key.setKeyCode(Runtime.storage().read(pos++));
      press(key);
      break;
    case MACRO_ACTION_STEP_KEYUP:
      key.setFlags(Runtime.storage().read(pos++));
      key.setKeyCode(Runtime.storage().read(pos++));
      release(key);
      break;
    case MACRO_ACTION_STEP_TAP:
      key.setFlags(Runtime.storage().read(pos++));
      key.setKeyCode(Runtime.storage().read(pos++));
      tap(key);
      break;

    case MACRO_ACTION_STEP_KEYCODEDOWN:
      key.setFlags(0);
      key.setKeyCode(Runtime.storage().read(pos++));
      press(key);
      break;
    case MACRO_ACTION_STEP_KEYCODEUP:
      key.setFlags(0);
      key.setKeyCode(Runtime.storage().read(pos++));
      release(key);
      break;
    case MACRO_ACTION_STEP_TAPCODE:
      key.setFlags(0);
      key.setKeyCode(Runtime.storage().read(pos++));
      tap(key);
      break;

    case MACRO_ACTION_STEP_TAP_SEQUENCE: {
      while (true) {
        key.setFlags(Runtime.storage().read(pos++));
        key.setKeyCode(Runtime.storage().read(pos++));
        if (key == Key_NoKey)
          break;
        tap(key);
        delay(interval);
      }
      break;
    }
    case MACRO_ACTION_STEP_TAP_CODE_SEQUENCE: {
      while (true) {
        key.setFlags(0);
        key.setKeyCode(Runtime.storage().read(pos++));
        if (key.getKeyCode() == 0)
          break;
        tap(key);
        delay(interval);
      }
      break;
    }

    case MACRO_ACTION_END:
    default:
      return;
    }

    delay(interval);
  }
}

bool isDynamicMacrosKey(Key key) {
  return (key.getRaw() >= ranges::DYNAMIC_MACRO_FIRST &&
          key.getRaw() <= ranges::DYNAMIC_MACRO_LAST);
}

// -----------------------------------------------------------------------------
EventHandlerResult DynamicMacros::onKeyEvent(KeyEvent &event) {
  if (!isDynamicMacrosKey(event.key))
    return EventHandlerResult::OK;

  if (keyToggledOn(event.state)) {
    uint8_t macro_id = event.key.getRaw() - ranges::DYNAMIC_MACRO_FIRST;
    play(macro_id);
  } else {
    for (Key key : active_macro_keys_) {
      release(key);
    }
  }

  return EventHandlerResult::EVENT_CONSUMED;
}

EventHandlerResult DynamicMacros::beforeReportingState(const KeyEvent &event) {
  // Here we add keycodes to the HID report for keys held in a macro sequence.
  // This is necessary because Kaleidoscope doesn't know about the supplemental
  // `active_macro_keys_[]` array.
  for (Key key : active_macro_keys_) {
    if (key != Key_NoKey)
      Runtime.addToReport(key);
  }
  return EventHandlerResult::OK;
}

EventHandlerResult DynamicMacros::onNameQuery() {
  return ::Focus.sendName(F("DynamicMacros"));
}

EventHandlerResult DynamicMacros::onFocusEvent(const char *command) {
  if (::Focus.handleHelp(command, PSTR("macros.map\nmacros.trigger")))
    return EventHandlerResult::OK;

  if (strncmp_P(command, PSTR("macros."), 7) != 0)
    return EventHandlerResult::OK;

  if (strcmp_P(command + 7, PSTR("map")) == 0) {
    if (::Focus.isEOL()) {
      for (uint16_t i = 0; i < storage_size_; i++) {
        uint8_t b;
        b = Runtime.storage().read(storage_base_ + i);
        ::Focus.send(b);
      }
    } else {
      uint16_t pos = 0;

      while (!::Focus.isEOL() && pos < storage_size_) {
        uint8_t b;
        ::Focus.read(b);

        Runtime.storage().update(storage_base_ + pos++, b);
      }
      Runtime.storage().commit();
      macro_count_ = updateDynamicMacroCache();
    }
  }

  if (strcmp_P(command + 7, PSTR("trigger")) == 0) {
    uint8_t id = 0;
    ::Focus.read(id);
    play(id);
  }

  return EventHandlerResult::EVENT_CONSUMED;
}

// public
void DynamicMacros::reserve_storage(uint16_t size) {
  storage_base_ = ::EEPROMSettings.requestSlice(size);
  storage_size_ = size;
  macro_count_  = updateDynamicMacroCache();
}

}  // namespace plugin
}  // namespace kaleidoscope

kaleidoscope::plugin::DynamicMacros DynamicMacros;
