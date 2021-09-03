#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <ArduinoJson.h>

#include <Adafruit_INA219.h>
#include <INA226_WE.h>
#define I2C_ADDRESS_1 0x41
#define I2C_ADDRESS_2 0x44

Adafruit_INA219 ina219; // 0X40
INA226_WE ina226_1 = INA226_WE(I2C_ADDRESS_1);
INA226_WE ina226_2 = INA226_WE(I2C_ADDRESS_2);

  float shuntvoltage_0 = 0;
  float d1_Bvol = 0;
  float d1_cur = 0;
  float d1_Lvol = 0;
  float d1_pow = 0;

  float shuntvoltage_1 = 0;
  float d2_Bvol = 0;
  float d2_cur = 0;
  float ld2_Lvol = 0;
  float d2_pow = 0;

  float shuntvoltage_2 = 0;
  float d3_Bvol = 0;
  float d3_cur = 0;
  float d3_Lvol = 0;
  float dev3_power = 0;
  
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

// LED Pin
const int ledPin = 4;

boolean bv1Request = true;
boolean lv1Request = true;
boolean cur1Request =true;
boolean pow1Request = true;

boolean bv2Request = false;
boolean lv2Request = false;
boolean cur2Request =false;
boolean pow2Request = false;

boolean bv3Request = false;
boolean lv3Request = false;
boolean cur3Request =false;
boolean pow3Request = true;

boolean bv0Request = false;
boolean lv0Request = false;
boolean cur0Request =false;
boolean pow0Request = false;

boolean isStart= true;
boolean isDevice = false;
boolean isAll_1 = false;
boolean isAll_2 = false;
boolean isAll_3 = true;

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
    if(messageTemp == "bv1ON"){
      bv1Request = true;
      isAll_1 =true;
      Serial.println("bv1ON");
    }
    else if(messageTemp == "bv1OFF"){
      bv1Request = false;
    }     
    else if(messageTemp == "lv1ON"){
      //Serial.println("Transmitter ON");
      lv1Request = true;
      isAll_1 =true;
    }
    else if(messageTemp == "lv1OFF"){
      lv1Request = false;
    }    
    else if(messageTemp == "cur1ON"){
      //Serial.println("Transmitter ON");
      cur1Request = true;
      isAll_1 =true;
    }
    else if(messageTemp == "cur1OFF"){
      cur1Request = false;
    }
    else if(messageTemp == "pow1ON"){
      //Serial.println("Transmitter ON");
      pow1Request = true;
      isAll_1 =true;
    }
    else if(messageTemp == "pow1OFF"){
      pow1Request = false;
    }    

    else if(messageTemp == "bv2ON"){
      bv2Request = true;
      isAll_2 =true;
    }
    else if(messageTemp == "bv2OFF"){
      bv2Request = false;
    }     
    else if(messageTemp == "lv2ON"){
      //Serial.println("Transmitter ON");
      lv2Request = true;
      isAll_2 =true;
    }
    else if(messageTemp == "lv2OFF"){
      lv2Request = false;
    }    
    else if(messageTemp == "cur2ON"){
      //Serial.println("Transmitter ON");
      cur2Request = true;
      isAll_2 =true;
    }
    else if(messageTemp == "cur2OFF"){
      cur2Request = false;
    }
    else if(messageTemp == "pow2ON"){
      //Serial.println("Transmitter ON");
      pow2Request = true;
      isAll_2 =true;
    }
    else if(messageTemp == "pow2OFF"){
      pow2Request = false;
    }  

    else if(messageTemp == "bv3ON"){
      bv3Request = true;
      isAll_3 =true;
    }
    else if(messageTemp == "bv3OFF"){
      bv3Request = false;
    }     
    else if(messageTemp == "lv3ON"){
      //Serial.println("Transmitter ON");
      lv3Request = true;
      isAll_3 =true;
    }
    else if(messageTemp == "lv3OFF"){
      lv3Request = false;
    }    
    else if(messageTemp == "cur3ON"){
      //Serial.println("Transmitter ON");
      cur3Request = true;
      isAll_3 =true;
    }
    else if(messageTemp == "cur3OFF"){
      cur3Request = false;
    }
    else if(messageTemp == "pow3ON"){
      //Serial.println("Transmitter ON");
      pow3Request = true;
      isAll_3 =true;
    }
    else if(messageTemp == "pow3OFF"){
      pow3Request = false;
    }  

    else if(messageTemp == "bv0ON"){
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
    if (messageTemp == "bus1ON"){
      isAll_1 =true;
      }
    else if(messageTemp == "bus1OFF"){
      isAll_1 = false;       
      }  
    if (messageTemp == "bus2ON"){
      isAll_2 =true;
      }
    else if(messageTemp == "bus2OFF"){
      isAll_2 = false;       
      } 
    if (messageTemp == "bus3ON"){
      isAll_3 =true;
      }
    else if(messageTemp == "bus3OFF"){
      isAll_3 = false;       
      }                 
  }
}
void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
    while (!Serial) {
      delay(1);
  }

  uint32_t currentFrequency;

  if (! ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }
  }
  // To use a slightly lower 32V, 1A range (higher precision on amps):
  //ina219.setCalibration_32V_1A();
  // Or to use a lower 16V, 400mA range (higher precision on volts and amps):
  //ina219.setCalibration_16V_400mA();
  
  ina226_1.init();
  ina226_2.init();
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
  ina226_2.setAverage(AVERAGE_1); // choose mode and uncomment for change of default
  
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
  
  Serial.println("INA226 Current Sensor Example Sketch - Continuous");
  
  ina226_1.waitUntilConversionCompleted(); //if you comment this line the first data might be zero
  ina226_2.waitUntilConversionCompleted(); //if you comment this line the first data might be zero
  //pinMode(ledPin, bus1_output);
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
    if (client.connect("esp32/data")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
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
  } 
    //static uint32_t time_start,time_end_0,time_end_1,time_end_2,time_end_3;
    //time_start = micros();
    
   
    if(isDevice){
      device_data();
      }
    
    //time_end_0 = micros();

    if(isAll_1 && isStart){
      bus1_data();
      delayMicroseconds(180);
      }
    
    time_end_1 = micros();
    if(isAll_2 && isStart){
      bus2_data();
      }
    */
  //  time_end_2 = micros();
    if(isAll_3 && isStart){
      bus3_data();
      delayMicroseconds(50);
      }
    
   // time_end_3=micros();
    
    //Serial.print(" Time 0 : "); Serial.print(time_end_0-time_start); Serial.println(" us");
    //Serial.print(" Time 1 : "); Serial.print(time_end_1-time_start); Serial.println(" us");
    //Serial.print(" Time 2 : "); Serial.print(time_end_2-time_start); Serial.println(" us");
    //Serial.print(" Time 3 : "); Serial.print(time_end_3-time_start); Serial.println(" us");    
  
}

void device_data(){
 StaticJsonDocument<200> doc;

    doc["Node"] = "Node 1";
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
   
    if (client.publish("esp32/data", output) == true) {
      //Serial.println("Success sending message");
    } else {
      //Serial.println("Error sending message");
    }  
  
}

void bus1_data(){
    StaticJsonDocument<96> bus1;

    if(bv1Request){      
      bus1["d1_Bvol"] = ina219.getBusVoltage_V();
      }//else{bus1["d1_Bvol"] = 0;}
     if(lv1Request){
      d1_Bvol = ina219.getBusVoltage_V();
      shuntvoltage_0 = ina219.getShuntVoltage_mV();
      bus1["d1_Lvol"] = d1_Bvol + (shuntvoltage_0 / 1000); 
      }// else{bus1["d1_Lvol"] = 0;}
     if(cur1Request){
      bus1["d1_cur"] = ina219.getCurrent_mA();
      } //else{bus1["d1_cur"] =0;}
     if(pow1Request){
      bus1["d1_pow"] = ina219.getPower_mW();
      }//else{bus1["d1_pow"]=0;}
    
    //time_end=micros();
    //Serial.print(" Time "); Serial.print(time_end-time_start); Serial.println("us");
    
    char bus1_output[100];
    size_t n = serializeJson(bus1, bus1_output);
    //client.publish("outTopic", buffer, n);
    
    serializeJson(bus1, bus1_output);
    
    //Serial.println("Sending message to MQTT topic..");
   // Serial.println(bus1_output);
   
    if (client.publish("esp32/bus1", bus1_output) == true) {
      Serial.println("Success sending message");
    } else {
      Serial.println("Error sending message");
    }
}

void bus2_data(){
    StaticJsonDocument<96> bus2;

    //static uint32_t time_start,time_end_0;

    if(bv2Request){
       ina226_1.readAndClearFlags();
       bus2["d2_Bvol"] = ina226_1.getBusVoltage_V();
      }else{bus2["d2_Bvol"] =0;}
    if(lv2Request){
      shuntvoltage_1 = ina226_1.getShuntVoltage_mV();
      d2_Bvol = ina226_1.getBusVoltage_V();    
      bus2["d2_Lvol"] = d2_Bvol + (shuntvoltage_1/1000);
      }else{bus2["d2_Lvol"] =0;}
    if(cur2Request){
      bus2["d2_cur"] = ina226_1.getCurrent_mA();
      }else{bus2["d2_cur"] =0;}

    if(pow2Request){
      bus2["d2_pow"] = ina226_1.getBusPower();     
      }else{bus2["d2_pow"] =0;}
    //time_start = micros();
  
      //time_end_0 = micros();
        
    char bus2_output[100];
    size_t n = serializeJson(bus2, bus2_output);
    //client.publish("outTopic", buffer, n);
    
    serializeJson(bus2, bus2_output);
    
    //Serial.println("Sending message to MQTT topic..");
    Serial.println(bus2_output);
   //Serial.print(" Time 0 : "); Serial.print(time_end_0-time_start); Serial.println(" us");
    if (client.publish("esp32/bus2", bus2_output) == true) {
      //Serial.println("Success sending message");
    } else {
     //Serial.println("Error sending message");
    }  
}

void bus3_data(){
    StaticJsonDocument<96> bus3;     

    if(bv3Request){
      ina226_2.readAndClearFlags();
      bus3["d3_Bvol"] = ina226_2.getBusVoltage_V();
      }else{bus3["d3_Bvol"] =-1;}
    if(lv3Request){
      shuntvoltage_2 = ina226_2.getShuntVoltage_mV();
      d3_Bvol = ina226_2.getBusVoltage_V(); 
      bus3["d3_Lvol"] = d3_Bvol + (shuntvoltage_2/1000); 
      }else{bus3["d3_Lvol"] =-1;}
    if(cur3Request){
      bus3["d3_cur"] = ina226_2.getCurrent_mA();
      }else{bus3["d3_cur"] =-1;}
    if(pow3Request){
      bus3["dev3_power"] = ina226_2.getBusPower();
      }else{bus3["dev3_power"] =-1;}

    char bus3_output[100];
    size_t n = serializeJson(bus3, bus3_output);
    //client.publish("outTopic", buffer, n);
    
    serializeJson(bus3, bus3_output);
    
    //Serial.println("Sending message to MQTT topic..");
    Serial.println(bus3_output);
   
    if (client.publish("esp32/bus3", bus3_output) == true) {
     // Serial.println("Success sending message");
    } else {
      Serial.println("Error sending message");
    }  
}

void checkStatus(){
  StaticJsonDocument<256> tel_status;

  tel_status["bv1_status"] = bv1Request;
  tel_status["lv1_status"] = lv1Request;
  tel_status["cur1_status"] = cur1Request;
  tel_status["pow1_status"] = pow1Request;

  tel_status["bv2_status"] = bv2Request;
  tel_status["lv2_status"] = lv2Request;
  tel_status["cur2_status"] = cur2Request;
  tel_status["pow2_status"] = pow2Request;
/*
  tel_status["bv3_status"] = bv3Request;
  tel_status["lv3_status"] = lv3Request;
  tel_status["cur3_status"] = cur3Request;
  tel_status["pow3_status"] = pow3Request;
  */

  tel_status["start_status"] = isStart; 
  tel_status["dev_status"] = isDevice; 

  char status_output[256];
  serializeJson(tel_status, status_output);
  //Serial.println("Sending message to ESP32/Status");
  Serial.println(status_output);
 
  if (client.publish("esp32/TEL_status", status_output) == true) {
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
