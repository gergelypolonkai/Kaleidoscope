/* -*- mode: c++ -*-
 * Kaleidoscope-Hardware-EZ-ErgoDox -- ErgoDox hardware support for Kaleidoscope
 * Copyright (C) 2018  Keyboard.io, Inc
 *
 * Based on QMK (commit 8cdb4a915)
 *  (C) Jack Humbert, Erez Zukerman, Oleg Kostyuk
 * Original QMK sources:
 *  - keyboards/ergodox_ez/ergodox_ez.c
 *  - keyboards/ergodox_ez/ergodox_ez.h
 *  - keyboards/ergodox_ez/matrix.c
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#ifdef ARDUINO_AVR_ERGODOX

#include <Arduino.h>

#include "kaleidoscope/hardware/ErgoDox/ErgoDoxScanner.h"

#define HARDWARE_IMPLEMENTATION kaleidoscope::hardware::ErgoDox
#include "Kaleidoscope-HIDAdaptor-KeyboardioHID.h"

#include "kaleidoscope/macro_helpers.h"

struct cRGB {
  uint8_t r, g, b;
};

#define CRGB(r,g,b) (cRGB){b, g, r}

namespace kaleidoscope {
namespace hardware {

class ErgoDox {
 public:
  ErgoDox(void) {}

  static constexpr byte matrix_columns = 6;
  static constexpr byte matrix_rows = 14;
  static constexpr int8_t led_count = 0;

  void syncLeds(void) {}
  void setCrgbAt(byte row, byte col, cRGB color) {}
  void setCrgbAt(int8_t i, cRGB crgb) {}
  cRGB getCrgbAt(int8_t i) {
    return CRGB(0, 0, 0);
  }
  int8_t getLedIndex(byte row, byte col) {
    return -1;
  }

  void scanMatrix(void);
  void readMatrix(void);
  void actOnMatrixScan(void);
  void setup();

  /** Detaching from / attaching to the host.
   *
   * These two functions should detach the device from (or attach it to) the
   * host, preferably without rebooting the device. Their purpose is to allow
   * one to do some configuration inbetween, so the re-attach happens with
   * different properties. The device remains powered between these operations,
   * only the connection to the host gets severed.
   */
  void detachFromHost();
  void attachToHost();

  /* Key masking
   * -----------
   *
   * There are situations when one wants to ignore key events for a while, and
   * mask them out. These functions help do that. In isolation, they do nothing,
   * plugins and the core firmware is expected to make use of these.
   *
   * See `handleKeyswitchEvent` in the Kaleidoscope sources for a use-case.
   */
  void maskKey(byte row, byte col);
  void unMaskKey(byte row, byte col);
  bool isKeyMasked(byte row, byte col);

  /** Key switch states
   *
   * These methods offer a way to peek at the key switch states, for those cases
   * where we need to deal with the state closest to the hardware. Some methods
   * offer a way to check if a key is pressed, others return the number of
   * pressed keys.
   */
  /**
   * Check if a key is pressed at a given position.
   *
   * @param row is the row the key is located at in the matrix.
   * @param col is the column the key is located at in the matrix.
   *
   * @returns true if the key is pressed, false otherwise.
   */
  bool isKeyswitchPressed(byte row, byte col);
  /**
   * Check if a key is pressed at a given position.
   *
   * @param keyIndex is the key index, as calculated by `keyIndex`.
   *
   * @note Key indexes start at 1, not 0!
   *
   * @returns true if the key is pressed, false otherwise.
   */
  bool isKeyswitchPressed(uint8_t keyIndex);
  /**
   * Check the number of key switches currently pressed.
   *
   * @returns the number of keys pressed.
   */
  uint8_t pressedKeyswitchCount();

  // ErgoDox-specific stuff
  void setStatusLED(uint8_t led, bool state = true);
  void setStatusLEDBrightness(uint8_t led, uint8_t brightness);

  void resetDevice();

  static uint8_t debounce;

 private:
  static ErgoDoxScanner scanner_;
  static uint8_t previousKeyState_[matrix_rows];
  static uint8_t keyState_[matrix_rows];
  static uint8_t masks_[matrix_rows];
  static uint8_t debounce_matrix_[matrix_rows][matrix_columns];

  static uint8_t debounceMaskForRow(uint8_t row);
  static void debounceRow(uint8_t change, uint8_t row);
  static void readMatrixRow(uint8_t row);
};

#define KEYMAP_STACKED(                                                 \
    /* left hand, spatial positions */                                  \
    r0c0, r0c1, r0c2, r0c3, r0c4, r0c5, r0c6,                           \
    r1c0, r1c1, r1c2, r1c3, r1c4, r1c5, r1c6,                           \
    r2c0, r2c1, r2c2, r2c3, r2c4, r2c5,                                 \
    r3c0, r3c1, r3c2, r3c3, r3c4, r3c5, r3c6,                           \
    r4c0, r4c1, r4c2, r4c3, r4c4,                                       \
                            r5c5, r5c6,                                 \
                                  r5c4,                                 \
                      r5c3, r5c2, r5c1,                                 \
                                                                        \
    /* right hand, spatial positions */                                 \
    r0c7,  r0c8,  r0c9,  r0c10, r0c11, r0c12, r0c13,                    \
    r1c7,  r1c8,  r1c9,  r1c10, r1c11, r1c12, r1c13,                    \
           r2c8,  r2c9,  r2c10, r2c11, r2c12, r2c13,                    \
    r3c7,  r3c8,  r3c9,  r3c10, r3c11, r3c12, r3c13,                    \
    r4c9,  r4c10, r4c11, r4c12, r4c13,                                  \
    r5c7,  r5c8,                                                        \
    r5c9,                                                               \
    r5c12, r5c11, r5c10 )                                               \
                                                                        \
  /* matrix positions */                                                \
  {                                                                     \
    { r0c0, r1c0, r2c0, r3c0, r4c0, XXX  },                             \
    { r0c1, r1c1, r2c1, r3c1, r4c1, r5c1 },                             \
    { r0c2, r1c2, r2c2, r3c2, r4c2, r5c2 },                             \
    { r0c3, r1c3, r2c3, r3c3, r4c3, r5c3 },                             \
    { r0c4, r1c4, r2c4, r3c4, r4c4, r5c4 },                             \
    { r0c5, r1c5, r2c5, r3c5, XXX,  r5c5 },                             \
    { r0c6, r1c6, XXX,  r3c6, XXX,  r5c6 },                             \
                                                                        \
    { r0c7,  r1c7,  XXX,   r3c7,  XXX,   r5c7  },                       \
    { r0c8,  r1c8,  r2c8,  r3c8,  XXX,   r5c8  },                       \
    { r0c9,  r1c9,  r2c9,  r3c9,  r4c9,  r5c9  },                       \
    { r0c10, r1c10, r2c10, r3c10, r4c10, r5c10 },                       \
    { r0c11, r1c11, r2c11, r3c11, r4c11, r5c11 },                       \
    { r0c12, r1c12, r2c12, r3c12, r4c12, r5c12 },                       \
    { r0c13, r1c13, r2c13, r3c13, r4c13, XXX   }                        \
  }
}
}

#include "kaleidoscope/hardware/key_indexes.h"

extern kaleidoscope::hardware::ErgoDox &ErgoDox;

#endif