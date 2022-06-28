# Maverick Flight Code

<img src="https://github.com/MaddyKilmurray/Maverick_Flight_Code/blob/main/Computer%20Details.png?raw=true">

<img src="https://github.com/MaddyKilmurray/Maverick_Flight_Code/blob/main/Computer%20steps.png?raw=true">

# Text Version

The Basics
Double-Sided PCB Board
22AWG Solid Core Wire
60/40 Rosin Core 0.5mm Solder Wire
1/2" Kapton Tape
Non-Static Sponge

The Main Processor
For this project, I decided to use a Teensy 4.1. The large amount of pins, combined with the beefy processing power and the on-board SD card reader was the winning combo.

Communication Sensors
Radio: Adafruit RFM69HCW Transceiver Radio Breakout 433 MHz
GPS: Adafruit Ultimate GPS Breakout
SD Card: 32GB

Sensors Continued...
NeoPixel Ring - 12 x 5050 RGB LED
Piezo Transducer - 15V Buzzer
ADXL377 - High-G Triple-Axis Accelerometer
MS5611 High Resolution Atmospheric Pressure Module

It's Got The Power!
To power the on-board computer, a 3.7V LP963450 1800mAh battery was connected to a PowerBoost 1000 Basic 5V USB Boost via a USB to MicroUSB cable to the Teensy MicroUSB port.

And Finally, the Ground Computer
The Ground Computer was a much simpler set-up, made up of an Arduino Uno and an Adafruit RFM69HCW Transceiver, connected via a breadboard and connected to a laptop.
