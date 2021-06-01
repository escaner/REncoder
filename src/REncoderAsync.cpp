/*
 *   REncoderAsync.cpp - Library to handle rotary encoders with async
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

#include "REncoderAsync.h"


/*
 *   Constructor.
 *   Initializes state machine to neutral state. Assumes encoder in rest
 *  position.
 *   Parameters:
 *   * DelayPressRelease: desired minimum delay between a press and its release
 *     in milliseconds
 *   * DelayReleasePress: desired minimum delay between a release and next press
 *     in milliseconds
 */
REncoderAsync::REncoderAsync(uint8_t DelayPressRelease,
  uint8_t DelayReleasePress):
  REncoder(),
  _DelayPressRelease(DelayPressRelease),
  _DelayReleasePress(DelayReleasePress)
{
  _NumEvents = 0;
  _NextEventRelease = EV_NONE;

  // Initialize last event time to 0. This means that for the special case
  // of the first event, we will need to wait a full delay since the
  // controller boot, even though it would not be necessary.
  // The additional complexity of the code for a wait that would normally have
  // anyway already elapsed (normally 20-50ms) make me choose this
  // implementation decision.
  _LastEventTime = 0UL;
}


/*
 *   Returns an identifier with the next event to be sent if the delay
 *  has passed (or none otherwise), consuming it from the queue.
 *   This method manages only an initial check for pending events
 *  which most of the time will fail thuse saving computing time, and then
 *  checks whether the delay has expired. Method _nextEvent() handles
 *  the event itself.
 *   Returns: REncoderAsync::EV_*
 */
int8_t REncoderAsync::getEvent()
{
  unsigned long CurrentTime;
  uint8_t Delay;

  // Quick check of pending events; _NumEvents is 1 byte: it is an atomic query,
  // no need to disable interrupts
  if (_NextEventRelease!=EV_NONE || _NumEvents!=0)
  {
    // Get the correct delay value (pressing or releasing key?)
    Delay = _NextEventRelease!=EV_NONE? _DelayPressRelease: _DelayReleasePress;

    // Check if the delay has been accomplished to send next event
    CurrentTime = millis();
    // Works even if millis() had overflow and CurTime < LastEventTime
    if (CurrentTime - _LastEventTime >= Delay)
      // Gets the event, updates queue and returns its value. NOTE: Returned
      // event could be EV_NONE if we got an async update() since first check
      return _nextEvent(CurrentTime);
  }

  // No pending events or delay not fulfilled
  return EV_NONE;
}


/*
 *   Sets a new delay between the press and next release event in milliseconds.
 *  Parameters:
 *  * DelayMs: the new delay, in milliseconds
 */
void REncoderAsync::setDelayPressRel(uint8_t DelayMs)
{
  _DelayPressRelease = DelayMs;
}


/*
 *   Sets a new delay between the release and next press event in milliseconds.
 *  Parameters:
 *  * DelayMs: the new delay, in milliseconds
 */
void REncoderAsync::setDelayRelPress(uint8_t DelayMs)
{
  _DelayReleasePress = DelayMs;
}


/*
 *   Assumes that the delay has passed and returns an identifier with the next
 *  event to be sent, consuming it from the queue.
 *   Note that even if getEvent() has checked that there are pending events,
 *  an update() call from an ISR could have changed that, we need to do
 *  an atomic check & update of the queue here (only _NumEvents is
 *  updated in the ISR). Therefore, interrupts are disabled for that piece of
 *  code.
 *   Parameters:
 *   * CurrentTime: current time as returned by millis() function
 *   Returns: REncoderAsync::EV_*
 *
 */
int8_t REncoderAsync::_nextEvent(unsigned long CurrentTime)
{
  int8_t Event;

  // Check for pending release event.
  if (_NextEventRelease != EV_NONE)
  {
    // Register pending release event and update queue and event time
    Event = _NextEventRelease;
    _NextEventRelease = EV_NONE;
    _LastEventTime = CurrentTime;
  }
  else
  {
    // Check for pending press events in the queue
    // NOTE: Protect _NumEvents update from ISR
    noInterrupts();
    if (_NumEvents)
    {
      // Positive events mean clockwise encoder direction
      if (_NumEvents > 0)
      {
        // Register pending cw press event and update queue and event time
        _NumEvents--;
        interrupts();
        Event = EV_CW_PRESS;
        _NextEventRelease = EV_CW_RELEASE;
      }
      else  // _NumEvents < 0: counterclockwise events pending
      {
        // Register pending ccw press event and update queue and event time
        _NumEvents++;
        interrupts();
        Event = EV_CCW_PRESS;
        _NextEventRelease = EV_CCW_RELEASE;
      }
      // Update event time for press events
      _LastEventTime = CurrentTime;
    }
    else
    {
      // No pending press events! update() happened since getEvent() check
      // and removed remaining event(s)
      interrupts();
      Event = EV_NONE;
    }
  }
  return Event;
}