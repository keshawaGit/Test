#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(45, 43); // CE, CSN         
const byte address[6] = "00001";     //Byte of array representing the address. This is the address where we will send the data. This should be same on the receiving side.
boolean button_state = 0;

char key[5] ="my";

struct PayloadStruct
{
  unsigned long nodeID;
  unsigned long payloadID;
  char data0[24];
};
PayloadStruct payload ;

void setup() {
radio.begin();                  //Starting the Wireless communication
Serial.begin(115200);
radio.setChannel(101);
radio.openWritingPipe(address); //Setting the address where we will send the data
radio.setPALevel(RF24_PA_MAX);  //You can set it as minimum or maximum depending on the distance between the transmitter and receiver.
radio.stopListening();          //This sets the module as transmitter
radio.setChannel(101);
}
void loop(){

//const char key[] = "0xa78a123d60baec0c5dd41b33a542463a8255391af64c74ee";
//const char key[] = "0xa78a12";

char  text[24]= {
 0xa7, 0x8a, 0x12, 0x3d, 0x60, 0xba, 0xec, 0x0c,
 0x5d, 0xd4, 0x1b, 0x33, 0xa5, 0x42, 0x46, 0x3a,
 0x82, 0x55, 0x39, 0x1a, 0xf6, 0x4c, 0x74, 0xee
};
/*
if (Serial.available() > 0) {
  //data = Serial.readStringUntil('\n');
  int ser = Serial.read() - '0';
  if(ser == 1){
    
    radio.write(&text, sizeof(text));                  //Sending the message to receiver
    Serial.println(ser);
    Serial.println(key);
    delay(1000);
    }
  }
  
*/
    //strcpy(payload.data0,text);
    payload.payloadID++;
    radio.write(&payload, sizeof(payload));                  //Sending the message to receiver
    Serial.println(payload.data0);
    delay(3000);

}
