/*
 *   REncoderAsync.h - Library to handle rotary encoders with async
 *  polling wait.
 *   Created by Escaner, 2019.
 */

#ifndef _RENCODERASYNC_H_
#define _RENCODERASYNC_H_

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
 *   
 */
 
 // Inherit protected: disable external access to REncoder::update()
class REncoderAsync: protected REncoder
{
public:
  // Events that can be returned to be performed by caller
  static const int8_t EV_LEFT_PRESS = -1;
  static const int8_t EV_LEFT_RELEASE = -2;
  static const int8_t EV_NONE = 0;
  static const int8_t EV_RIGHT_PRESS = 1;
  static const int8_t EV_RIGHT_RELEASE = 2;

  static const uint8_t DEFAULT_DELAY = 32;

  // Public methods
  REncoderAsync(uint8_t DelayPressRelease = DEFAULT_DELAY,
    uint8_t DelayReleasePress = DEFAULT_DELAY);
  inline int8_t update(uint8_t A, uint8_t B);  // Overwrite REncoder definition
  uint8_t getEvent();

protected:
  // Desired delay between two events
  const uint8_t _DelayPressRelease;
  const uint8_t _DelayReleasePress;

  // Event queue handling
  int8_t _NumEvents;  // Number of pending press events; sign is direction
  int8_t _NextEventRelease;  // EV_* with a pending release event or EV_NONE
  unsigned long _LastEventTime;  // Time when last event was issued
};

#endif  // _RENCODERASYNC_H_