#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(45, 43); // CE, CSN         
const byte address[6] = "00001";     //Byte of array representing the address. This is the address where we will send the data. This should be same on the receiving side.
boolean button_state = 0;

void setup() {
radio.begin();                  //Starting the Wireless communication
Serial.begin(115200);
radio.openWritingPipe(address); //Setting the address where we will send the data
radio.setPALevel(RF24_PA_MAX);  //You can set it as minimum or maximum depending on the distance between the transmitter and receiver.
radio.stopListening();          //This sets the module as transmitter
}
void loop()
{

//const char text[] = 0xa78a123d60baec0c5dd41b33a542463a8255391af64c74ee;

char  text[24]= {
 0xa7, 0x8a, 0x12, 0x3d, 0x60, 0xba, 0xec, 0x0c,
 0x5d, 0xd4, 0x1b, 0x33, 0xa5, 0x42, 0x46, 0x3a,
 0x82, 0x55, 0x39, 0x1a, 0xf6, 0x4c, 0x74, 0xee
};

radio.write(&text, sizeof(text));                  //Sending the message to receiver
Serial.println(text);
delay(2000);
}
