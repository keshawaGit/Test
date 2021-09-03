#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <INA226_WE.h>

#define I2C_ADDRESS_1 0x40

INA226_WE ina226_1 = INA226_WE(I2C_ADDRESS_1);
RF24 radio(4, 5); // CE, CSN
const byte address[6] = "00001";


  float shuntvoltage_0 = 0;
  float d0_Bvol = 0;
  float d0_cur = 0;
  float d0_Lvol = 0;
  float d0_pow = 0;


  
// Replace the next variables with your SSID/Password combination
//const char* ssid = "EIE_WLAN_5";
//const char* password = "";
//const char* mqtt_server = "10.50.20.44";

// Add your MQTT Broker IP address, example:
const char* ssid = "SLT-4G_B84E2";
const char* password = "prolink12345";
const char* mqtt_server = "192.168.1.11";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

//String local_IP =WiFi.localIP().toString();

boolean bv0Request = false;
boolean lv0Request = false;
boolean cur0Request =false;
boolean pow0Request = true;

boolean isStart= true;
boolean isDevice = false;
boolean isAll_0 = true;
  byte RXtext[24];
  String strRXtext;

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // If a message is received on the topic esp32/bus1_output, you check if the message  
  // Changes the bus1_output state according to the message
  if (String(topic) == "esp32/output") {
    Serial.print("Changing bus1_output to ");

     if(messageTemp == "bv0ON"){
      bv0Request = true;
    }
    else if(messageTemp == "bv0OFF"){
      bv0Request = false;
    }     
    else if(messageTemp == "lv0ON"){
      //Serial.println("Transmitter ON");
      lv0Request = true;
    }
    else if(messageTemp == "lv0OFF"){
      lv0Request = false;
    }    
    else if(messageTemp == "cur0ON"){
      //Serial.println("Transmitter ON");
      cur0Request = true;
    }
    else if(messageTemp == "cur0OFF"){
      cur0Request = false;
    }
    else if(messageTemp == "pow0ON"){
      //Serial.println("Transmitter ON");
      pow0Request = true;
    }
    else if(messageTemp == "pow0OFF"){
      pow0Request = false;
    }   

    if (messageTemp == "start"){
      isStart =true;
      }
    else if(messageTemp == "stop"){
      isStart = false;       
      }
     if (messageTemp == "devON"){
      isDevice =true;
      }
    else if(messageTemp == "devOFF"){
      isDevice = false;       
      }     
    if (messageTemp == "bus0ON"){
      isAll_0 =true;
      }
    else if(messageTemp == "bus0OFF"){
      isAll_0 = false;       
      }  
 
  }
}
void setup() {
  Serial.begin(115200);
  Wire.begin();
  radio.begin();
  radio.openReadingPipe(0, address);   //Setting the address at which we will receive the data
  radio.setPALevel(RF24_PA_MIN);       //You can set this as minimum or maximum depending on the distance between the transmitter and receiver.
  radio.startListening();              //This sets the module as receiver
Serial.println("Rx");
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
    while (!Serial) {
      delay(1);
  }

  uint32_t currentFrequency;

  
  ina226_1.init();

    /* Set Number of measurements for shunt and bus voltage which shall be averaged
  * Mode *     * Number of samples *
  AVERAGE_1            1 (default)
  AVERAGE_4            4
  AVERAGE_16          16
  AVERAGE_64          64
  AVERAGE_128        128
  AVERAGE_256        256
  AVERAGE_512        512
  AVERAGE_1024      1024
  */
  ina226_1.setAverage(AVERAGE_1); // choose mode and uncomment for change of default
 
  /* Set conversion time in microseconds
     One set of shunt and bus voltage conversion will take: 
     number of samples to be averaged x conversion time x 2
     
     * Mode *         * conversion time *
     CONV_TIME_140          140 µs
     CONV_TIME_204          204 µs
     CONV_TIME_332          332 µs
     CONV_TIME_588          588 µs
     CONV_TIME_1100         1.1 ms (default)
     CONV_TIME_2116       2.116 ms
     CONV_TIME_4156       4.156 ms
     CONV_TIME_8244       8.244 ms  
  */
 // ina226_1.setConversionTime(CONV_TIME_588); //choose conversion time and uncomment for change of default
 // ina226_2.setConversionTime(CONV_TIME_140); //choose conversion time and uncomment for change of default
  /* Set measure mode
  POWER_DOWN - INA226 switched off
  TRIGGERED  - measurement on demand
  CONTINUOUS  - continuous measurements (default)
  */
  //ina226.setMeasureMode(CONTINUOUS); // choose mode and uncomment for change of default
  
  /* Set Current Range
    * Mode *   * Max Current *
     MA_400          400 mA
     MA_800          800 mA (default)
  */
  //ina226.setCurrentRange(MA_800); // choose gain and uncomment for change of default
  
  /* If the current values delivered by the INA226 differ by a constant factor
     from values obtained with calibrated equipment you can define a correction factor.
     Correction factor = current delivered from calibrated equipment / current delivered by INA226
  */
  // ina226.setCorrectionFactor(0.95);
  
  Serial.println("INA226 Current Sensor  - Continuous");
  
  //ina226_1.waitUntilConversionCompleted(); //if you comment this line the first data might be zero

}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("esp32/Rx")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/outputRx");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  /*
  if (now - lastMsg > 800) {
    lastMsg = now;
    checkStatus();
  } */
    //static uint32_t time_start,time_end_0,time_end_1,time_end_2,time_end_3;
    //time_start = micros();
    
   /* 
    if(isDevice){
      device_data();
      }
    */
    //time_end_0 = micros();
    byte text[24];
    if (radio.available()){

    radio.read(&text, sizeof(text));    //Reading the data
            for(int i = 0; i < 24; i++) {
              Serial.print(text[i],HEX);
              
              //RXtext[i] = (text[i],HEX);
              //strRXtext += String(RXtext[i]);
              //Serial.print(", ");
              if(i == 23) {Serial.println(" ");} 
              //Serial.print(RXtext[i]);
              
            }
    }

    if(isAll_0 && isStart){
      bus0_data();
      delayMicroseconds(50);
      }
      //Serial.println("loop");
     delay(5);
    /*
    time_end_1 = micros();
    
    //Serial.print(" Time 0 : "); Serial.print(time_end_0-time_start); Serial.println(" us");  
  */
}

void device_data(){
 StaticJsonDocument<200> doc;

    doc["Node"] = "Node Rx";
    doc["status"] = wl_status_to_string(WiFi.status());
    doc["network"] = String(WiFi.SSID());
    doc["rssi"] = String(WiFi.RSSI());
    String ip = WiFi.localIP().toString();
    doc["mqttIP"] = String(ip);
    //Serial.println(ip);
    
    char output[256];
    size_t n = serializeJson(doc, output);
    //client.publish("outTopic", buffer, n);
    
    serializeJson(doc, output);
    
   // Serial.println("Sending message to MQTT topic..");
    //Serial.println(output);
   
    if (client.publish("esp32/DevRx", output) == true) {
      Serial.println("Success sending message");
    } else {
      Serial.println("Error sending message");
    }  
  
}

void bus0_data(){
    StaticJsonDocument<256> bus0;

    //static uint32_t time_start,time_end_0;

    if(bv0Request){
       ina226_1.readAndClearFlags();
       bus0["d0_Bvol"] = ina226_1.getBusVoltage_V();
      }else{bus0["d0_Bvol"] =-1;}
    if(lv0Request){
      shuntvoltage_0 = ina226_1.getShuntVoltage_mV();
      d0_Bvol = ina226_1.getBusVoltage_V();    
      bus0["d0_Lvol"] = d0_Bvol + (shuntvoltage_0/1000);
      }else{bus0["d0_Lvol"] =-1;}
    if(cur0Request){
      bus0["d0_cur"] = ina226_1.getCurrent_mA();
      }else{bus0["d0_cur"] =-1;}

    if(pow0Request){
      bus0["d0_pow"] = ina226_1.getBusPower();     
      }else{bus0["d0_pow"] =-1;}
      //bus0["d0_RX"] = RXtext;
    //time_start = micros();
  
      //time_end_0 = micros();
        
    char bus0_output[256];
    size_t n = serializeJson(bus0, bus0_output);
    //client.publish("outTopic", buffer, n);
    
    serializeJson(bus0, bus0_output);
    
    //Serial.println("Sending message to MQTT topic..");
    Serial.println(bus0_output);
   //Serial.print(" Time 0 : "); Serial.print(time_end_0-time_start); Serial.println(" us");
    if (client.publish("esp32/Rx", bus0_output) == true) {
      //Serial.println("Success sending message");
    } else {
     //Serial.println("Error sending message");
    }  
}

void checkStatus(){
  StaticJsonDocument<256> tel_status;

  tel_status["bv0_status"] = bv0Request;
  tel_status["lv0_status"] = lv0Request;
  tel_status["cur0_status"] = cur0Request;
  tel_status["pow0_status"] = pow0Request;

  tel_status["start_status"] = isStart; 
  tel_status["dev_status"] = isDevice; 

  char status_output[256];
  serializeJson(tel_status, status_output);
  //Serial.println("Sending message to ESP32/Status");
  Serial.println(status_output);
 
  if (client.publish("esp32/TELRx", status_output) == true) {
    //Serial.println("Success status");
  } else {
    Serial.println("Error status");
  }
    
}

const char* wl_status_to_string(wl_status_t status) {
  switch (status) {
    case WL_NO_SHIELD: return "WL_NO_SHIELD";
    case WL_IDLE_STATUS: return "WL_IDLE_STATUS";
    case WL_NO_SSID_AVAIL: return "WL_NO_SSID_AVAIL";
    case WL_SCAN_COMPLETED: return "WL_SCAN_COMPLETED";
    case WL_CONNECTED: return "WL_CONNECTED";
    case WL_CONNECT_FAILED: return "WL_CONNECT_FAILED";
    case WL_CONNECTION_LOST: return "WL_CONNECTION_LOST";
    case WL_DISCONNECTED: return "WL_DISCONNECTED";
  }
}
