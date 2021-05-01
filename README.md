# DueCarRemote

This is a project to remotely control a robot car kit using an Arduino Due
(because it has USB Host mode support), a USB Gamepad and a BLE chip
with a SPI interface.

# Parts

These are _not_ affiliate links. I'm not trying to drive ad revenue, or profit
off affiliate links, I'm just documenting how I did a project that I found fun.
These are the exact parts I ordered and the exact source I ordered them from
(with the exception of the gamepad - that doesn't seem to be available anymore).

The robot car part of this was done using:

* [ELEGOO Smart Robot Car Kit V 3.0](https://www.amazon.com/dp/B07KPZ8RSZ)

And the parts for this remote control part (this project) are:

* [Arduino Due](https://www.amazon.com/dp/B00A6C3JN2/)
* [Logitech F310 Gamepad](https://www.amazon.com/dp/B003VAHYQY/)
* [DSD TECH HM-19 BLE Module](https://www.amazon.com/dp/B07MBLVHH8/) [Datasheet](HM-18 HM-19_en-V1.pdf)
* [USB OTG Adapter](https://www.amazon.com/dp/B00GM0OZ4O/)
* Any random USB power bank + a USB A<->USB Micro cable

# Software

Conceptually, the software is pretty simple: read bits from the gamepad, make
steering decisions based on the gamepad bits, and send those decisions down the
BLE link, which looks a lot like a serial connection.

## Reading Bits From The Gamepad

This is the part that took me the longest to get working. The Arduino USBHost
library is a relatively complex bit of machinery, and it took a bit for me to
1) figure out how to extend it to support a HID-but-non-HID-boot device, and 2)
how to debug my work along the way.

Eventually I achieved success with my generic [hidgamepad](remote/hidgamepad.h)
interface and my [GamepadController](remote/GamepadController.cpp)
implementation that provides specific support for the gamepad I'm using.

Each axis (digital or analog) and button is assigned a unique ID. When that
axis or button changes state, an
[event handler function](remote/due_remote.ino) for that type of event
is called with the ID and new value.

## Making Decisions

The robot car has two motor control channels - one for left, and one for right.
Each channel can be independently instructed to go forward or reverse at
various speeds. By setting different speeds to the left and right wheels, the
car can be made to go straight forward, foward-turn, turn-in-place, reverse,
and reverse-turn.  This maps somewhat nicely to the dpad, but doesn't give
the user access to speed control.

The idea, then, is to make the dpad direct the car as indicated above.
Meanwhile the analog pads are used to provide _independent_ and _direct_
control of the left and right channels.  Push both sticks fully forward, and
the car moves fully forward.  Push them both gently foward, and the car will go
more slowly.  Push on gently forward and the other gently back, it will turn
slowly in place.

## Controlling The Car

This family of BLE modules has the really neat property that in peripheral mode
(the default), you can
[treat them just like a serial device](https://makersportal.com/blog/2017/9/20/hm-10-bluetooth-module).

For our purposes (operating as a BLE Central), there's a little bit more work
involved in setting them up, but they're still pretty straightforward.

Once the connection is made to the car, we can just send the one-byte message
that corresponds with the action we want the car to take.

| 7    | 6 | 5 | 4    | 3    | 2 | 1 | 0    |
| ---- | - | - | ---- | ---- | - | - | ---- |
| LMSB |   |   | LLSB | RMSB |   |   | RLSB |

LMSB - Left motor channel, most significant bit (2s complement)

LLSB - Left motor channel, least significant bit (2s complement)

RMSB - Right motor channel, most significant bit (2s complement)

RLSB - Right motor channel, least significant bit (2s complement)

Four bits of 2s complement should give us a range of -8 to +7, which should
provide plenty of resolution for controlling vehicle speed, especially after
we account for the "dead zone" where the requested speed is too slow to allow
the vehicle to start.

If we were to expand the messages to more than one byte, we could do things
like distinguish between coasting vs active driving/braking, but that's a
question for another day.
