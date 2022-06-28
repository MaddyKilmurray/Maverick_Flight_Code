#include <MS5611.h>
#include <Wire.h>
#include <Adafruit_GPS.h>
#include <SD.h>
#include <SPI.h>
#include <RHReliableDatagram.h>
#include <RH_RF69.h>
#include <RHHardwareSPI1.h>
#include <pitches.h>

// Tone configuration for "Ready to Fly" tone
#define OCTAVE_OFFSET 0

// Radio configuration
#define CLIENT_ADDRESS  101
#define SERVER_ADDRESS  15
#define RF69_FREQ       433.0
#define RFM69_CS        38
#define RFM69_INT       digitalPinToInterrupt(37)
#define RFM69_RST       36
#define RFM69_MISO      39

// GPS Definition Configurations
#define GPSSerial Serial2
#define GPSECHO true

// Neopixel Configuration
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

// GPS Information Configuration
Adafruit_GPS GPS(&GPSSerial);
uint32_t timer = millis();

// SD Card Configuration
const int chipSelect = BUILTIN_SDCARD;

// Axis Configuration
int scale = 1000;
const int xInput = 16;
const int yInput = 15;
const int zInput = 14;

// These min max values were configured manually once the sensor was connected.
int xRawMin = 408;
int xRawMax = 410;

int yRawMin = 409;
int yRawMax = 411;

int zRawMin = 408;
int zRawMax = 412;

// Buzzer Configuration
int buzz1 = 33;

// Neopixel Configuration
int pin = 13;
int numpixels = 12;
int pixelcount = 0;
Adafruit_NeoPixel pixels(numpixels, pin, NEO_GRB + NEO_KHZ800);

// Empty strings for transmission
String dataString;
String gpsString;

// Altitude calculation configuation
MS5611 MS5611(0x77);
int MS5611_AVERAGE = 32;
float sea_pressure = 998; // Air Pressure needs to be manually changed based on location
float sea_temperature;
float current_altitude, current_pressure, current_temperature;

// Flight altitude flag - switches when over 500m
bool flightAltitude = false;

// Radio Driver Configuration
RH_RF69 driver(RFM69_CS, RFM69_INT, hardware_spi1);
RHReliableDatagram manager(driver, CLIENT_ADDRESS);

// Counter for Radio transmissions
int radioCount = 0;
int transmissionCount = 0;

// Empty String for GPS data to be written to
String gpsData;

// Arrays required for transmission of radio data
uint8_t radioData[45];
uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];

// Configuration for musical tones
int notes[] = { 0,
                262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494,
                523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988,
                1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976,
                2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951
              };
char *song = "MainStre:d=16,o=6,b=125:8g,8d,8g,8a,8b,8d7,8c7,8a,8b,8g,8e,8a,8f#,8e,f#,e,8d,8g,8d,8g,8a,8b,8d7,8c7,8a,8b,8g,8e,8a,f#,e,f#,d,8g,8p,g,p,b,c7,8d7,g,p,8e,a,g,f#,e,f#,d,g,p,g,a,b,a,g,f#,8e,a,p,f#,e,f#,d,g,p,b,c7,8d7,g,p,8e,a,g,f#,e,f#,d,g,a,b,g,a,b,a,g,f#,e,f#,d,c7";

// When the computer is turned on, this is the set-up method.
// Initially, the NeoPixels lights light up one at a time, then flash
// Then, as each sensor is initialised, two lights are lit up. If all sensors are connected,
// the NeopIxel will turn green and play the "Success" tone.
// If any sensors do not work, the matchng lights will show up as red, and once all sensors 
// have been checked, it will play the "Failed" tone.
void setUpLights() {

  SPI1.setCS(RFM69_CS);
  SPI1.setMISO(RFM69_MISO);
  SPI1.begin();

  Serial.begin(115200);

  pixels.begin();

  pixels.clear();
  pixels.setBrightness(10);
  for (int i=0; i < 12; i++) {
    pixels.setPixelColor(i, pixels.Color(32, 212, 80));
    pixels.show();
    delay(200);
  }

  pixels.clear();
  pixels.setPixelColor(0, pixels.Color(32, 212, 80));
  pixels.show();
  delay(200);
  pixels.setPixelColor(1, pixels.Color(32, 212, 80));
  pixels.show();

  if (!MS5611.begin()) {
    Serial.println("MS5611 not found, check wiring!");
    for (int i=2; i < 4; i++) {
      pixels.setPixelColor(i, pixels.Color(250, 24, 7));
      pixels.show();
      delay(200);
    }
    while (1);
  }
  else {
    for (int i=2; i < 4; i++) {
      pixels.setPixelColor(i, pixels.Color(32, 212, 80));
      pixels.show();
      delay(200);
    }
    MS5611.read();

    for (int i=0; i < MS5611_AVERAGE; i++) {
      sea_temperature = sea_temperature + MS5611.getTemperature();
    }
    
    sea_temperature = sea_temperature / MS5611_AVERAGE;
  }

  if (!GPS.begin(9600)) {
    Serial.println("GPS not found, check wiring!");
    for (int i=4; i < 6; i++) {
      pixels.setPixelColor(i, pixels.Color(250, 24, 7));
      pixels.show();
      delay(200);
    }
    while (1);
  }
  else {
    for (int i=4; i < 6; i++) {
      pixels.setPixelColor(i, pixels.Color(32, 212, 80));
      pixels.show();
      delay(200);
    }
  }

  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    for (int i=6; i < 8; i++) {
      pixels.setPixelColor(i, pixels.Color(250, 24, 7));
      pixels.show();
      delay(200);
    }
  }
  else {
    for (int i=6; i < 8; i++) {
      pixels.setPixelColor(i, pixels.Color(32, 212, 80));
      pixels.show();
      delay(200);
    }
  }

  if (analogRead(xInput) == 0 or analogRead(yInput) == 0 or analogRead(zInput) == 0) {
    Serial.println("ADXL377 not working, check wiring!");
    for (int i=8; i < 10; i++) {
      pixels.setPixelColor(i, pixels.Color(250, 24, 7));
      pixels.show();
      delay(200);
    }
    while (1);
  }
  else {
    for (int i=8; i < 10; i++) {
      pixels.setPixelColor(i, pixels.Color(32, 212, 80));
      pixels.show();
      delay(200);
    }
  }

  if (!setUpRadio()) {
    Serial.println("Radio not working, check wiring!");
    for (int i=10; i < 12; i++) {
      pixels.setPixelColor(i, pixels.Color(250, 24, 7));
      pixels.show();
      delay(200);
    }
    while (1);
  }
  else {
    for (int i=10; i < 12; i++) {
      pixels.setPixelColor(i, pixels.Color(32, 212, 80));
      pixels.show();
      delay(200);
    }
  }

  if ((pixels.getPixelColor(1) == pixels.getPixelColor(3)) and (pixels.getPixelColor(5) == pixels.getPixelColor(7)) and (pixels.getPixelColor(9) == pixels.getPixelColor(11))) {
    pixels.clear();
    for (int i=0; i < 4; i++) {
      pixels.setBrightness(0);
      pixels.show();
      delay(400);
      pixels.clear();
      pixels.setBrightness(10);
      for (int j=0; j < 12; j++) {
        pixels.setPixelColor(j, pixels.Color(32, 212, 80));
      }
      pixels.show();
      delay(400);
    }
    launchTone();
    delay(5000);
  }
  else {
    pixels.clear();
    for (int i=0; i < 4; i++) {
      pixels.setBrightness(0);
      pixels.show();
      delay(400);
      pixels.clear();
      pixels.setBrightness(10);
      for (int j=0; j < 12; j++) {
        pixels.setPixelColor(j, pixels.Color(250, 24, 7));
      }
      pixels.show();
      delay(400);
    }
    failedSensorsTone();
  } 
}

// This method initialises the GPS sensor. It will continue to loop until 
// the sensor has picked up a satelitte fix. Once it has, it will play
// the final setting up tone.
bool setUpGPS() {
  if (!GPS.begin(9600)) {
    return false;
  }
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_100_MILLIHERTZ); // 10 second update time
  GPS.sendCommand(PGCMD_ANTENNA);

  delay(1000);

  GPSSerial.println(PMTK_Q_RELEASE);

  while (getGPSFix() < 1) {
    pixels.clear();
    pixels.setBrightness(10);
    for (int i=0; i < (12/2); i++) {
      pixels.setPixelColor(i, pixels.Color(255, 0, 0));
      pixels.setPixelColor(i + (12/2), pixels.Color(255, 0, 0));
      pixels.show();
    }
  }

  pixels.clear();
    for (int i=0; i < 4; i++) {
      pixels.setBrightness(0);
      pixels.show();
      delay(400);
      pixels.clear();
      pixels.setBrightness(10);
      for (int j=0; j < 12; j++) {
        pixels.setPixelColor(j, pixels.Color(32, 212, 80));
      }
      pixels.show();
      delay(400);
    }
    play_rtttl(song);
    delay(5000);
    return true;
}

// This method initialises the radio, sets the frequency, encryption and power.
bool setUpRadio() {

  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

  // manual reset
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);

  if (!manager.init()) {
    Serial.println("init failed");
    return false;
  }

  driver.setFrequency(RF69_FREQ);

  driver.setTxPower(20, true);

  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  driver.setEncryptionKey(key);

  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");
  return true;
}

// This method lights up the Neopixel when all is running correctly and has been initialised.
// When the main method is running, this will refresh so quickly that it is hard to tell the 
// difference between the lights. However, due to the delay needed for the radio to send a 
// message, the lights will pause and appear as intended. This is a good way to troubleshoot
// that the radio is working without looking at the ground computer.
void launchLights() {
  bool testingComplete = false;
  bool flash = false;
  pixels.clear();
  while (!testingComplete) {
    if (flash) {  
      pixels.setBrightness(10);
      pixels.setPixelColor(0, pixels.Color(58,65,92));
      pixels.setPixelColor(1, pixels.Color(255, 204, 0));
      pixels.setPixelColor(2, pixels.Color(58,65,92));
      pixels.setPixelColor(3, pixels.Color(255, 204, 0));
      pixels.setPixelColor(4, pixels.Color(58,65,92));
      pixels.setPixelColor(5, pixels.Color(255, 204, 0));
      pixels.setPixelColor(6, pixels.Color(58,65,92));
      pixels.setPixelColor(7, pixels.Color(255, 204, 0));
      pixels.setPixelColor(8, pixels.Color(58,65,92));
      pixels.setPixelColor(9, pixels.Color(255, 204, 0));
      pixels.setPixelColor(10, pixels.Color(58,65,92));
      pixels.setPixelColor(11, pixels.Color(255, 204, 0));
      pixels.show();
      flash = !flash;
      testingComplete = true;
    }
    else {
      pixels.setBrightness(10);
      pixels.setPixelColor(0, pixels.Color(255, 204, 0));
      pixels.setPixelColor(1, pixels.Color(58,65,92));
      pixels.setPixelColor(2, pixels.Color(255, 204, 0));
      pixels.setPixelColor(3, pixels.Color(58,65,92));
      pixels.setPixelColor(4, pixels.Color(255, 204, 0));
      pixels.setPixelColor(5, pixels.Color(58,65,92));
      pixels.setPixelColor(6, pixels.Color(255, 204, 0));
      pixels.setPixelColor(7, pixels.Color(58,65,92));
      pixels.setPixelColor(8, pixels.Color(255, 204, 0));
      pixels.setPixelColor(9, pixels.Color(58,65,92));
      pixels.setPixelColor(10, pixels.Color(255, 204, 0));
      pixels.setPixelColor(11, pixels.Color(58,65,92));
      pixels.show();
      flash = !flash;
    }
  }  
}

String getAxisInfo() {

  int rawX = analogRead(xInput);
  int rawY = analogRead(yInput);
  int rawZ = analogRead(zInput);

  float scaledX, scaledY, scaledZ; // Scaled values for each axis
  // Convert raw values to 'milli-Gs"
    scaledX = map(rawX, xRawMin, xRawMax, -1000, 1000);
    scaledY = map(rawY, yRawMin, yRawMax, -1000, 1000);
    scaledZ = map(rawZ, zRawMin, zRawMax, -1000, 1000);
  
    // re-scale to fractional Gs
    float xAccel = scaledX / 1000.0;
    float yAccel = scaledY / 1000.0;
    float zAccel = scaledZ / 1000.0;
    
  // Return scaled X,Y,Z accelerometer readings
  return (String(xAccel,3) + ',' + String(yAccel,3) + ',' + String(zAccel,3));
}

String getAltitude() {
  MS5611.read();

  current_pressure = MS5611.getPressure();
  current_temperature = MS5611.getTemperature();

  for (int i=0; i < MS5611_AVERAGE; i++) {
    current_altitude = (((pow((sea_pressure / current_pressure), 1/5.257) - 1.0) * (current_temperature + 273.15)) / 0.0065);
  }
  current_altitude = current_altitude / MS5611_AVERAGE;

  if (current_altitude > 500) {
    flightAltitude = true;
  }
  return(String(current_altitude, 3) + ',' + current_pressure + ',' + current_temperature);
}

int getGPSFix() {
  GPS.read();

  if (GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA())) 
      return 0; 
  }
  return (int)GPS.fix;
}

String getGPS() {
  char c = GPS.read();

  if (GPSECHO)
    if (c) Serial.print(c);
  if (GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA()))
      return "";
  }

    float s = GPS.seconds + GPS.milliseconds / 1000. + GPS.secondsSinceTime();
    int m = GPS.minute;
    int h = GPS.hour;
    int d = GPS.day;
    // Adjust time and day forward to account for elapsed time.
    // This will break at month boundaries
    while (s > 60) {
      s -= 60;
      m++;
    }
    while (m > 60) {
      m -= 60;
      h++;
    }
    while (h > 24) {
      h -= 24;
      d++;
    }

    gpsString = "Time: " + (String)(h +1) + ':' + (String)m + ':' + (String)s + '.' + (String)GPS.milliseconds + ',' ;
    gpsString = gpsString + d + '/' + GPS.month + '/' + GPS.year + ',' + GPS.fix + ',' ;
    gpsString = gpsString + GPS.fixquality + ',' + "Seconds since fix: " + GPS.secondsSinceFix() + ',';
    gpsString = gpsString + GPS.latitude + ',' + GPS.lat + ',' + GPS.longitude + ',' + GPS.lon + ','; 
    gpsString = gpsString + GPS.speed + ',' + GPS.angle + ',' + GPS.altitude + ',' + GPS.satellites;
  
  return gpsString;
}

String getGPSSimple() {
  char c = GPS.read();

  if (GPSECHO)
    if (c) Serial.print(c);
  if (GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA()))
      return ""; 
  }

    float s = GPS.seconds + GPS.milliseconds / 1000. + GPS.secondsSinceTime();
    int m = GPS.minute;
    int h = GPS.hour;
    int d = GPS.day;
    // Adjust time and day forward to account for elapsed time.
    // This will break at month boundaries!
    while (s > 60) {
      s -= 60;
      m++;
    }
    while (m > 60) {
      m -= 60;
      h++;
    }
    while (h > 24) {
      h -= 24;
      d++;
    }
    
    gpsString = "Latitude: " + (String)GPS.latitude + ',' + GPS.lat + ',' + "Longitude: " + (String)GPS.longitude + ',' + GPS.lon + ',' + "T: " + transmissionCount;
  
  return gpsString;
}

float mapf(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void writeToCard() {
  File dataFile = SD.open("LAUNCH_DAY_240622_f2.txt", FILE_WRITE);

  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  } else {
    Serial.println("error opening datalog.txt");
  }
}

void sendRadio() {
  Serial.println("Sending to rf69_reliable_datagram_server");
    
  if (manager.sendtoWait(radioData, sizeof(radioData), SERVER_ADDRESS))
  {
    uint8_t len = sizeof(buf);
    uint8_t from;   
    if (manager.recvfromAckTimeout(buf, &len, 3000, &from))
    {
      Serial.print("got reply from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char*)buf);
    }
    else
    {
      Serial.println("No reply, is rf69_reliable_datagram_server running?");
    }
  }
  else
    Serial.println("sendtoWait failed");
  transmissionCount++;
  delay(500);
}

void setup() {
  setUpLights();  
  setUpGPS();
}

void loop() {
  launchLights();
  GPS.read();

  if (flightAltitude) {
    rescueTone();
  }

  // if (millis() - timer > 500) {
  //   timer = millis();
  //   dataString = "";
  //   dataString = dataString + getAltitude() + ",";
  //   dataString = dataString + getAxisInfo() + ",";
  //   dataString = dataString + getGPS();
  //   Serial.print("Test data: "); Serial.println(dataString);
  //   writeToCard();
  // }

 if (millis() - timer > 200) {
   timer = millis();
   dataString = "";
   dataString = dataString + getAltitude() + ",";
   dataString = dataString + getAxisInfo() + ",";
   dataString = dataString + getGPS();
   Serial.print("Test data: "); Serial.println(dataString);
   writeToCard();

   gpsData = getGPSSimple();
   char charString[gpsData.length() + 1];
   strcpy(charString, gpsData.c_str());
   for (int i = 0; i < gpsData.length(); i++) {
     radioData[i] = charString[i];
   }
   if (radioCount > 40) {
     sendRadio();
     radioCount = 0;
   }
   else {
     radioCount++;
   }
 }

}

void launchTone() {
  // plays a tone on launch of engine 
    tone(buzz1, 523.25, 133);
    delay(133);
    tone(buzz1, 523.25, 133);
    delay(133);
    tone(buzz1, 523.25, 133);
    delay(133);
    tone(buzz1, 523.25, 400);
    delay(400);
    tone(buzz1, 415.30, 400);
    delay(400);
    tone(buzz1, 466.16, 400);
    delay(400);
    tone(buzz1, 523.25, 133);
    delay(133);
    delay(133);
    tone(buzz1, 466.16, 133);
    delay(133);
    tone(buzz1, 523.25, 1200);
    delay(1200);
}

void failedSensorsTone() {
  // plays a tone on failed initiated sensors 
    tone(buzz1, 200, 200);
    delay(500);
    tone(buzz1, 200, 200);
    delay(500);
    tone(buzz1, 200, 200);
    delay(500);
}

void rescueTone() {
  // Plays tone after reaching apogee to assist with recovery
    tone(buzz1, 2800, 1000);
    delay(500);
    tone(buzz1, 500, 1000);
    delay(500);
    tone(buzz1, 1500, 1000);
    delay(500);
    int var = 0;
    while (var < 3) {
      tone(buzz1, 2800, 200);
      delay(500);
      var++;
    }
}

void play_rtttl(char *p)
{
// Plays "Electric Parade" once set up has been completed
  byte default_dur = 4;
  byte default_oct = 6;
  int bpm = 63;
  int num;
  long wholenote;
  long duration;
  byte note;
  byte scale;

  while (*p != ':') p++;   
  p++;                    

  if (*p == 'd')
  {
    p++; p++;              
    num = 0;
    while (isdigit(*p))
    {
      num = (num * 10) + (*p++ - '0');
    }
    if (num > 0) default_dur = num;
    p++;                   
  }

  if (*p == 'o')
  {
    p++; p++;            
    num = *p++ - '0';
    if (num >= 3 && num <= 7) default_oct = num;
    p++;                  
  }

  if (*p == 'b')
  {
    p++; p++;              
    num = 0;
    while (isdigit(*p))
    {
      num = (num * 10) + (*p++ - '0');
    }
    bpm = num;
    p++;                  
  }

  wholenote = (60 * 1000L / bpm) * 4;  

  while (*p)
  {
    num = 0;
    while (isdigit(*p))
    {
      num = (num * 10) + (*p++ - '0');
    }

    if (num) duration = wholenote / num;
    else duration = wholenote / default_dur;  
    
    note = 0;

    switch (*p)
    {
      case 'c':
        note = 1;
        break;
      case 'd':
        note = 3;
        break;
      case 'e':
        note = 5;
        break;
      case 'f':
        note = 6;
        break;
      case 'g':
        note = 8;
        break;
      case 'a':
        note = 10;
        break;
      case 'b':
        note = 12;
        break;
      case 'p':
      default:
        note = 0;
    }
    p++;

    if (*p == '#')
    {
      note++;
      p++;
    }

    if (*p == '.')
    {
      duration += duration / 2;
      p++;
    }

    if (isdigit(*p))
    {
      scale = *p - '0';
      p++;
    }
    else
    {
      scale = default_oct;
    }

    scale += OCTAVE_OFFSET;

    if (*p == ',')
      p++; 

    if (note)
    {
      tone(buzz1, notes[(scale - 4) * 12 + note]);
      delay(duration);
      noTone(buzz1);
    }
    else
    {
      delay(duration);
    }
  }
}
