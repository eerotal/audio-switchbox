# Audio input switch and volume control device firmware

This repository contains MCU firmware for a device which can
be used to switch between multiple audio inputs and control
volume using an IR remote controller.

The project electronics are custom designed and include, among other things:

  - A PIC16F15344 microcontroller
  - Analog MUX circuitry for input selection
  - A motor controller potentiometer for volume control
  - MOSFET H-bridge motor driver for controlling the potentiometer
  - An IR receiver for remote control

Copyright Eero Talus 2022
