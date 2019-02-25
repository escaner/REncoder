https://github.com/escaner/REncoder

REncoder is an Arduino library to process rotary encoder signals and determine
step events. Two classes are provided:

* REncoder has a method update() that keeps track of the encoder position,
  performs debouncing and returns a value indicating whether a step has been
  completed and its direction.

* REncoderAsync adds asynchronous functionality for press/release events.
  When comunicating with computers via HID/DirectX, a certain amount of time
  needs to pass between press and release events.
  Mehod update() should be called from the ISR or the polling code, while
  getEvent() will return the event (clockwise/counterclockwise and
  press/release) that needs to be sent when the desired delay has passed.
  It keeps track of the number of pending events. Note that while there are
  pending steps to one side, registered steps to the other side just discount
  and there will be no events for them.


Copyright (C) 2019, Óscar Laborda

This file is part of REncoder library.

REncoder is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

REncoder is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with REncoder.  If not, see <https://www.gnu.org/licenses/>.
