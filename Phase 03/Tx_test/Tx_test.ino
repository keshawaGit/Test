#include <SPI.h>
#include "printf.h"
#include "RF24.h"
RF24 radio(45, 43); //45,43  | 9,10

char role = 'T';
struct package
{
  unsigned long id=6;
  unsigned long keyID = 0;
  char  key[100] = "a78a123d60baec0c5dd41b33";

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
  while (!Serial) {}
  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {} // hold in infinite loop
  }
  radio.setChannel(115); 
  radio.setPALevel(RF24_PA_MAX); // RF24_PA_MAX is default.
  radio.setDataRate( RF24_2MBPS) ; // |RF24_2MBPS
    int pipe =2;
    // Set the address on pipe 0 to the RX node.
    radio.stopListening(); // put radio in TX mode
    radio.openWritingPipe(address[pipe]);
    radio.setRetries(0,0);
    delay(1000);
    Serial.println("Tx starting...");
}

void loop() {
  
    unsigned long start_timer = micros();                    // start the timer
    bool report = radio.write(&data, sizeof(data));    // transmit & save the report
    unsigned long end_timer = micros();                      // end the timer

    if (report) {
      // payload was delivered
      Serial.print("\nNode : ");
      Serial.print(data.id);
      Serial.print("  key ID : ");
      Serial.print(data.keyID);
      Serial.print("  KEY : ");
      Serial.println(data.key);

      data.keyID = data.keyID + 1;
      Serial.print(F("  Time to transmit: "));
      Serial.print(end_timer - start_timer);                 // print the timer result
      Serial.println(F(" us"));
      while(1){}
    } else {
      Serial.println(F("Transmission failed or timed out")); // payload was not delivered
    }

    // to make this example readable in the serial monitor
    delay(200); // slow transmissions down by 1 second
}
