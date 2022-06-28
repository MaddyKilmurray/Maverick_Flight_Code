<h1>Maverick Flight Code</h1>

<img src="https://github.com/MaddyKilmurray/Maverick_Flight_Code/blob/main/Computer%20Details.png?raw=true">

<img src="https://github.com/MaddyKilmurray/Maverick_Flight_Code/blob/main/Computer%20steps.png?raw=true">

<h1>Text Version</h1>

<h3>The Basics</h3>
<ul>
<li>Double-Sided PCB Board</li>
<li>22AWG Solid Core Wire</li>
<li>60/40 Rosin Core 0.5mm Solder Wire</li>
<li>1/2" Kapton Tape</li>
<li>Non-Static Sponge</li>
</ul>

<h3>The Main Processor</h3>
<p>For this project, I decided to use a Teensy 4.1. The large amount of pins, combined with the beefy processing power and the on-board SD card reader was the winning combo.</p>

<h3>Communication Sensors</h3>
<ul>
<li>Radio: Adafruit RFM69HCW Transceiver Radio Breakout 433 MHz</li>
<li>GPS: Adafruit Ultimate GPS Breakout</li>
<li>SD Card: 32GB</li>
</ul>

Sensors Continued...
NeoPixel Ring - 12 x 5050 RGB LED
Piezo Transducer - 15V Buzzer
ADXL377 - High-G Triple-Axis Accelerometer
MS5611 High Resolution Atmospheric Pressure Module

It's Got The Power!
To power the on-board computer, a 3.7V LP963450 1800mAh battery was connected to a PowerBoost 1000 Basic 5V USB Boost via a USB to MicroUSB cable to the Teensy MicroUSB port.

And Finally, the Ground Computer
The Ground Computer was a much simpler set-up, made up of an Arduino Uno and an Adafruit RFM69HCW Transceiver, connected via a breadboard and connected to a laptop.
