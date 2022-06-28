<h1>Maverick Flight Code</h1>

<img src="https://github.com/MaddyKilmurray/Maverick_Flight_Code/blob/main/Computer%20Details.png?raw=true">

<img src="https://github.com/MaddyKilmurray/Maverick_Flight_Code/blob/main/Computer%20steps.png?raw=true">

<h1>Text Version</h1>

<h2>Maverick Flight Computer</h2>
<h3>Components and Construction</h3>

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

<h3>Sensors Continued...</h3>
<ul>
<li>NeoPixel Ring - 12 x 5050 RGB LED</li>
<li>Piezo Transducer - 15V Buzzer</li>
<li>ADXL377 - High-G Triple-Axis Accelerometer</li>
<li>MS5611 High Resolution Atmospheric Pressure Module</li>
</ul>
  
<h3>It's Got The Power!</h3>
<p>To power the on-board computer, a 3.7V LP963450 1800mAh battery was connected to a PowerBoost 1000 Basic 5V USB Boost via a USB to MicroUSB cable to the Teensy MicroUSB port.</p>

<h3>And Finally, the Ground Computer</h3>
<p>The Ground Computer was a much simpler set-up, made up of an Arduino Uno and an Adafruit RFM69HCW Transceiver, connected via a breadboard and connected to a laptop.</p>

<h2>Maverick Flight Computer</h2>
<h3>Step by Step</h3>

<ol>
  <li>
    <h4>Plug in the Battery</h4>
    <p>Once the battery is plugged in, the set-up method begins. The NeoPixel flashes up, confirming that power has been received. </p>
  </li>
  <li>
    <h4>Run Set-Up of Sensors</h4>
    <p>Each sensor will be initialised. If a sensor is connected and working correctly, two lights on the Neopixel will show green. Otherwise, they will show red. If all lights are green, all sensors are initialised correctly.</p>
  </li>
  <li>
    <h4>Where am I?</h4>
    <p>The GPS sensor will now search for a Satelitte fix. This can take up to 10 minutes, and the sensor needs to be outside. Once fixed, a success tone will play.</p>
  </li>
  <li>
    <h4>Time to collect data</h4>
    <p>Now that all the sensors are tested, every 200 milliseconds, the main loop will request a reading from each sensor, and write them to the on-board SD memory card.</p>
  </li>
  <li>
    <h4>Ground control to Major Tom!</h4>
    <p>Every 8 seconds, the on-board computer will send the last reading of longitude and latitude data to the ground computer.</p>
    <p>The ground computer will send back an acknowledgement for any packets received.</p>
  </li>
  <li>
    <h4>Rescue me!</h4>
    <p>After the altimeter reads a height higher than 500m, the rescue tone is added to the loop, and will keep going until the rocker is rescued</p>
  </li>
</ol>
