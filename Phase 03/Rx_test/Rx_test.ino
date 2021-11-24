#include <SPI.h>
#include "printf.h"
#include "RF24.h"
RF24 radio(4, 5);

char role = 'R';
struct package
{
  unsigned long id=0;
  unsigned long keyID = 0;
  char  key[100] ="empty";

};
 
typedef struct package Package;
Package data;

uint64_t address[6] = {0x7878787878LL,
                       0xB3B4B5B6F1LL,
                       0xB3B4B5B6CDLL,
                       0xB3B4B5B6A3LL,
                       0xB3B4B5B60FLL,
                       0xB3B4B5B605LL
                      };

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    // some boards need to wait to ensure access to serial over USB
  }
  delay(1000);
  // initialize the transceiver on the SPI bus
  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {} // hold in infinite loop
  }
  radio.setChannel(115); 
  radio.setPALevel(RF24_PA_MAX); // RF24_PA_MAX is default.
  radio.setDataRate( RF24_2MBPS ) ;
  for (uint8_t i = 0; i < 6; ++i)
    radio.openReadingPipe(i, address[i]);

  radio.startListening(); // put radio in RX mode
  Serial.println("Rx starting...");
}

void loop() {
    uint8_t pipe;
    if (radio.available(&pipe)) {             // is there a payload? get the pipe number that recieved it
      uint8_t bytes = radio.getPayloadSize(); // get the size of the payload
      radio.read(&data, sizeof(data));            // fetch payload from FIFO
      Serial.print(F("Received "));
      Serial.print(bytes);                    // print the size of the payload
      Serial.print(F(" bytes on pipe "));
      Serial.println(pipe);                     // print the pipe number
      Serial.print(F(" Node : "));
      Serial.print(data.id);           // print the payload's origin
      Serial.print(F("key ID : "));
      Serial.print(data.keyID);      // print the payload's number
      Serial.print(F("  KEY: "));
      Serial.println(data.key);      // print the payload's number

    }
}
