#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(4, 5); // CE, CSN
const byte address[6] = "00001";
//boolean button_state = 0;
//int led_pin = 3;

void setup() {
//pinMode(6, OUTPUT);
Serial.begin(115200);
radio.begin();
radio.openReadingPipe(0, address);   //Setting the address at which we will receive the data
radio.setPALevel(RF24_PA_MAX);       //You can set this as minimum or maximum depending on the distance between the transmitter and receiver.
radio.startListening();              //This sets the module as receiver
Serial.println("Rx");
}
void loop(){
if (radio.available())              //Looking for the data.
{
char RXtext[192] = "";                 //Saving the incoming data
byte text[24];
radio.read(&text, sizeof(text));    //Reading the data
        for(int i = 0; i < 24; i++) {
          Serial.print(text[i],HEX);
          RXtext[i] = (text[i],HEX);
          //Serial.print(", ");
          if(i == 23) {Serial.println(" ");} 
        }
//Serial.println(text);
}

delay(5);
}
