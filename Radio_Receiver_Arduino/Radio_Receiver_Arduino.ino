// This code was repurposed from the RadioHead RF69 sample code.
// Other than small changes, I did not write nor do I own this code.

#include <SPI.h>
#include <RH_RF69.h>
#include <RHReliableDatagram.h>

#define RF69_FREQ 433.0

#define MY_ADDRESS     15

  #define RFM69_INT     3   
  #define RFM69_CS      4  
  #define RFM69_RST     2  

RH_RF69 rf69(RFM69_CS, RFM69_INT);

RHReliableDatagram rf69_manager(rf69, MY_ADDRESS);

int16_t packetnum = 0;

void setup() 
{
  Serial.begin(115200);
  while (!Serial) { delay(1); }
    
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

  Serial.println("RFM69 RX Test!");
  Serial.println();

  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);
  
  if (!rf69_manager.init()) {
    Serial.println("RFM69 radio init failed");
    while (1);
  }
  Serial.println("RFM69 radio init OK!");

  if (!rf69.setFrequency(RF69_FREQ)) {
    Serial.println("setFrequency failed");
  }

  rf69.setTxPower(20, true); 

  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  rf69.setEncryptionKey(key);

  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");
}

uint8_t data[] = "Received";

uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];

void loop() {
  if (rf69.available())
  {
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (rf69_manager.recvfromAck(buf, &len, &from)) {
      buf[len] = 0;
      
      Serial.print("Got packet from : 0x"); 
      Serial.print(from, HEX);
      Serial.print("] : ");
      Serial.println((char*)buf);

      if (!rf69_manager.sendtoWait(data, sizeof(data), from))
        Serial.println("Sending failed (no ack)");
    }
  }
}
