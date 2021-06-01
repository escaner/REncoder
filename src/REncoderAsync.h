/*
 *   REncoderAsync.h - Library to handle rotary encoders with async
 *  polling wait.
 *
 *   Copyright (C) 2019 Óscar Laborda
 *
 *   This file is part of REncoder library.
 *
 *   REncoder is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *   REncoder is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *  along with REncoder.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef _RENCODERASYNC_H_
#define _RENCODERASYNC_H_

/*
 *   Define REA_MANAGE_INTERRUPTS if you need update() to disable interrupts
 *  inside its code. Normally it is not needed unless interrupts are manually
 *  enabled inside the ISR or update() is not called from an ISR.
 *   E.g.:
 *  #define REA_MANAGE_INTERRUPTS
 */

#ifdef REA_MANAGE_INTERRUPTS
#  include <util/atomic.h>
#endif

#include "REncoder.h"


/*
 *   When dealing with computers and DirectX events, some time is required
 *  to pass between the "press" and "release" events in order for the
 *  computer to correctly process the event.
 *   This library adds a counter _NumEvents to the update() method to keep the
 *  number of events that still need to be sent out while timing the correct
 *  time separation through getEvent() method.
 *   When a step to the opposite side to the events in the queue is received,
 *  the number of pending events is reduced instead of adding a new one
 *  to the opposite side.
 *   A boolean _NextEventRelease keeps track of a pending release event even
 *  when _NumEvents changes sign (encoder direction).
 *   Convention: clockwise (CW) events are positive in sign, while
 *  counterclockwise (CCW) events are negative. This is also followed in
 *  
 */
 
// Inherit from base class protected: disable external access to
// REncoder::update()
class REncoderAsync: protected REncoder
{
public:
  // Events that can be returned to be performed by caller
  static const int8_t EV_CCW_PRESS = -1;
  static const int8_t EV_CCW_RELEASE = -2;
  static const int8_t EV_NONE = 0;
  static const int8_t EV_CW_PRESS = 1;
  static const int8_t EV_CW_RELEASE = 2;

  static const uint8_t DEFAULT_DELAY = 32;

  // Public methods
  REncoderAsync(uint8_t DelayPressRelease = DEFAULT_DELAY,
    uint8_t DelayReleasePress = DEFAULT_DELAY);
  inline int8_t update(uint8_t A, uint8_t B);  // Overwrite REncoder definition
  int8_t getEvent();
  void setDelayPressRel(uint8_t DelayMs = DEFAULT_DELAY);
  void setDelayRelPress(uint8_t DelayMs = DEFAULT_DELAY);

protected:
  // Desired delay between two events, in milliseconds
  uint8_t _DelayPressRelease;
  uint8_t _DelayReleasePress;

  // Event queue handling
  volatile int8_t _NumEvents;  // Number of pending press events; sign=direction
  int8_t _NextEventRelease;  // EV_* with a pending release event or EV_NONE
  unsigned long _LastEventTime;  // Time when last event was issued

  // Protected methods
  int8_t _nextEvent(unsigned long CurrentTime);
};


/*
 *   Updates the state machine with new encoder signals, keeps track of
 *  the number of events to send and returns whether a step has been completed.
 *   Effectively overwrites REncoder::update() by calling it and updating
 *  _NumEvents member.
 *   =NOTE= assumes to be called inside an ISR therefore it does not disable
 *  interrutps or reenables them: they should already be disabled. If
 *  if interrupts need to be disabled, define REA_MANAGE_INTERRUPTS macro.
 *   Parameters:
 *   * A (HIGH, LOW): value of A pin signal
 *   * B (HIGH, LOW): value of B pin signal
 *   Return:
 *   * 0: no step has been completed
 *   * 1: clockwise step has been completed
 *   * -1: counter clockwise step has been completed
 */
inline int8_t REncoderAsync::update(uint8_t A, uint8_t B)
{
  int8_t Step;

  // Update encoder status with superclass method
  Step = REncoder::update(A, B);

#ifdef REA_MANAGE_INTERRUPTS
  // Protect _NumEvents update from interruts and restore it after
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
#endif

  // Keep count of the events in queue
  _NumEvents += Step;

#ifdef REA_MANAGE_INTERRUPTS
  }
#endif

  return Step;
}

#endif  // _RENCODERASYNC_H_