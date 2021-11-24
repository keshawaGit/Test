#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "printf.h"
RF24 radio(4, 5); // CE, CSN

struct package{
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

const int numChannel = 10;
const int spectRange =65; // 120 -60 Mhz
int channelRecovered[numChannel];
int recoverdNode[numChannel];
int channelNONRecovered[spectRange];

int key_recovered = 4; // 7 out of 10
int scan =0; // currrent count of the recoverd channel
unsigned long startTime =0;
unsigned long endTime =0;
int prvScanIndex =0;
int prvNONScanIndex=0;
const int guradBand = 1; // 1MHz BW
const int maxBW = 125;
const int minBW =60;
int scanning =0;
bool randomSearch =false;
int indexRec=0;
int randomCount =0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {} // hold in infinite loop
  }
  //radio.setChannel(115);
  radio.setPALevel(RF24_PA_MAX); // RF24_PA_MAX is default.
  radio.setDataRate( RF24_2MBPS ); //RF24_250KBPS 
  setRx();
  radio.setRetries(0,0);
  Serial.println("Rx Node");
}

void loop() {
  startTime = micros();
  int BWshift =0;
  int tempBW =minBW;
  while(scan <= key_recovered && (randomCount <300)){
    int nonScan = checkBW(tempBW);
    scanning = nonScan; //+BWshift;
    if(scanning >0){
      scanChannel(scanning); // Scan the channel and store the recovered channel in array
      delay(100);
    }else{
      while(scan <= key_recovered){
        scanChannel(nonRecBW());
        Serial.print("Scan ==>");
        Serial.println(scan);               
        if(channelRecovered[prvScanIndex] != 0){
          //Serial.println("scan++");
          scan++;
        }
        if(randomCount == 300){
          break;
        }
        randomCount++;
      }
    }
    //BWshift++;
    tempBW++;
    if(channelRecovered[prvScanIndex] != 0){
      scan++;
      BWshift=0;
    }
  }
  endTime =micros();
  Serial.print("Recoverd Spectrum ==>");
  print_arrayR();
  Serial.print("NON Recoverd Spectrum ==>");
  print_arrayNR();
  Serial.print("Recoverd Node ==>");
  print_node();
  Serial.print("Scan finished with :");
  Serial.print(endTime - startTime);
  Serial.println("  uS");
  while(1){}
}

int checkBW(int band){
  int nextBW=0;
  for(int i=1;i <=numChannel;i++){
    if(channelRecovered[i] == band ){
      if(band != maxBW){
        nextBW =band+1;
        break;
      }
      else{ // if reach to the maximum BW of channel
        nextBW = -1; // to read from non-recoverd BW arary OR scan from the begining of spectrum
        randomSearch =true;
        Serial.println("End of the spectrum scanning - First Time");
          Serial.print("Recoverd Spectrum ==>");
          print_arrayR();
          Serial.print("NON Recoverd Spectrum ==>");
          print_arrayNR();
        break;
      }
    }
    nextBW = band;
  }
  return nextBW;
}

int arraySizeN0(){
  int nonScanned =0;    // sizeof(channelNONRecovered)/sizeof(channelNONRecovered[0]);
  for (int i = 0; i < sizeof(channelNONRecovered)/sizeof(channelNONRecovered[0]); i++)
  {
    if(channelNONRecovered[i]){
      nonScanned++;
    }
  }
  Serial.print("Count of Non scanned BW :");
  Serial.println(nonScanned);
  return nonScanned;
}

int nonRecBW(){
  //get the size of non_recoverd_array without non zeroes
  int maxIndexNON = arraySizeN0();
  int retBW=0;
  do{
  indexRec = random(0,maxIndexNON-1);
  }while(channelNONRecovered[indexRec] <0);
  // return the BW randomly
  Serial.print("Return Index :");
  Serial.println(indexRec);
  retBW = channelNONRecovered[indexRec];
  Serial.print("Random BW scanning :");
  Serial.println(retBW);
  return retBW;
}

int scanChannel(int channel){
  bool keyRecover =false;
  int spectrum=channel;
  //Recoverd channel ===> channelRecovered[]
  //NON Recoverd channel ===> channelNONRecovered[]

  //0 => 2400 Mhz (RF24 channel 1)
  //1 => 2401 Mhz (RF24 channel 2)
  //76 => 2476 Mhz (RF24 channel 77)
  //83// => 2483 Mhz (RF24 channel 84)
  //124 => 2524 Mhz (RF24 channel 125)
  //125 => 2525 Mhz (RF24 channel 126)

  radio.setChannel(channel);
  /*
  Serial.print("Scanning Channel :" );
  Serial.print(spectrum);
  Serial.println(" channel");
  */
  uint8_t pipe;
  if (radio.available(&pipe)) {             // is there a payload? get the pipe number that recieved it
    uint8_t bytes = radio.getPayloadSize(); // get the size of the payload
    radio.read(&data, sizeof(data));            // fetch payload from FIFO
    /*
    Serial.print(("Received "));
    Serial.print(bytes);                    // print the size of the payload
    Serial.print((" bytes on pipe "));
    Serial.print(pipe);                     // print the pipe number
    Serial.print((" from node "));
    Serial.print(data.id);           // print the payload's origin
    Serial.print((" key ID: "));
    Serial.println(data.keyID);      // print the payload's number
    Serial.print(F("  KEY: "));
    Serial.println(data.key);     
    //  store recovered channel into array
    Serial.print("Spectrum recovered ===>");
    Serial.println(spectrum);
    */
    recoverdNode[prvScanIndex]=data.id;
    channelRecovered[prvScanIndex] = spectrum;
    prvScanIndex++;
    if(randomSearch){
    channelNONRecovered[indexRec]= -1;
    }    
  }else{
    Serial.print("Spectrum Not recovered ===>");
    Serial.println(spectrum);

    if(!randomSearch){
    channelNONRecovered[prvNONScanIndex]=spectrum;
    prvNONScanIndex++;
    }

  }

  return spectrum;
}

void setRx(){
    // Set the addresses for all pipes to TX nodes
    for (uint8_t i = 0; i < 6; ++i){
      radio.openReadingPipe(i, address[i]);
    }
    radio.startListening(); // put radio in RX mode

}

void print_arrayR(){
  for (int i=0; i < 10; i++) {
    Serial.print("[");
    Serial.print(channelRecovered[i]);
    Serial.print("] ");
    delay(5);
    }
    Serial.println();
}

void print_node(){
  for (int i=0; i < 10; i++) {
    Serial.print("[");
    Serial.print(recoverdNode[i]);
    Serial.print("] ");
    delay(5);
    }
    Serial.println();
}

void print_arrayNR(){
  for (int i=0; i < sizeof(channelNONRecovered) / sizeof(channelNONRecovered[0]); i++) {
    Serial.print("[");
    Serial.print(channelNONRecovered[i]);
    Serial.print("] ");
    delay(5);
    }
    Serial.println();
}
