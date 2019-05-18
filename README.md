# LED Cube 4x4

This project contains the sourcecode for a LED Cube with 4x4x4 LEDs with a single color. It is based on an Atmega 328p, which is the same microcontroller as used on the Arduino UNO.

## Table of content

1. [How to build](#how-to-build-sourcecode)
2. [Hardware description](#hardware-description)
   1. [Fuses](#fuses)
   2. [Pin description](#pin-description)
   3. [Circuit](#circuit)
   4. [Parts](#parts)

## How to build sourcecode

The project is build with Atmel Studio 6.2. The project-file is included, so building should be possible without hesitations. Please also read the [description](#hardware-description) of the device-hardware, since fuses has to be set for correct behaviour.

The compiled code can be uploaded to the atmega by using an Atmel Studio external tool. I set it up like the following:

|Name|Value
|-|-
|Title|`USBTiny Atmega328p`
|Command|`C:\Program Files\AVRDude\bin\avrdude.exe`
|Arguments|`-c usbtiny -p m328p -vvvv -P usb -U flash:w:$(TargetDir)$(TargetName).hex:a`

Of course you have to adapt the values depending on your avrdude path and your programmer.

## Hardware description

As already said, the qubes use an atmega 328p as known from the Arduino UNO.

### Fuses

Both the low and the high fuse of the atmega has to be set to the following values:
- **low**: 0xD9
- **high**: 0xE2

This can be done using AVRdude. Using an usbtiny as programmer, the commands are the following:

```
avrdude -c usbtiny -p m328p -U hfuse:w:0xD9:m
avrdude -c usbtiny -p m328p -U lfuse:w:0xE2:m
```

### Pin description

- **Port A**: LED-Stack 1-8 (Anode)
- **Port C**: LED-Stack 9-16 (Anode)
- **Pins B0 - B3**: LED-Layer 1-4 (Cathode)
- **Pins B4 & B5**: Buttons
- **Pin B6**: Reset-Button

### Circuit

![Circuit image](docs/circuit.svg)

### Parts

- Atmega 328p
- 16 resistors 150 Ohm *
- 4 resistors 10k Ohm
- 3 resistors 1k Ohm
- 1 capacitor (size not important, just for stability)
- 64 LEDs
- 3 push-buttons
- 4 transistors
- 1 breadboard (got it on 62mm*62mm or 25 holes per direction)

*Normaly 220Ohm should be used for 5V (depending on color of LED), but since each LED is used only 25% of the time, a higher current and consequential a lower resistance can be used