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
const char* mqtt_server = "192.168.1.10";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
String data;
int ser;
//String local_IP =WiFi.localIP().toString();

// LED Pin
const int ledPin = 4;

boolean bv3Request = false;
boolean lv3Request = false;
boolean cur3Request =false;
boolean pow3Request = true;


boolean isStart= true;
boolean isDevice = false;
boolean isAll_3 = true;
boolean isSerial = false;

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
   if(messageTemp == "bv3ON"){
      bv3Request = true;
      //isAll_3 =true;
    }
    else if(messageTemp == "bv3OFF"){
      bv3Request = false;
    }     
    else if(messageTemp == "lv3ON"){
      //Serial.println("Transmitter ON");
      lv3Request = true;
      //isAll_3 =true;
    }
    else if(messageTemp == "lv3OFF"){
      lv3Request = false;
    }    
    else if(messageTemp == "cur3ON"){
      //Serial.println("Transmitter ON");
      cur3Request = true;
      //isAll_3 =true;
    }
    else if(messageTemp == "cur3OFF"){
      cur3Request = false;
    }
    else if(messageTemp == "pow3ON"){
      //Serial.println("Transmitter ON");
      pow3Request = true;
     // isAll_3 =true;
    }
    else if(messageTemp == "pow3OFF"){
      pow3Request = false;
    }  

   else if (messageTemp == "start"){
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
  
  //ina226_1.waitUntilConversionCompleted(); //if you comment this line the first data might be zero
  //ina226_2.waitUntilConversionCompleted(); //if you comment this line the first data might be zero
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
    if (client.connect("Esp32Client")) {
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
  //Serial.println("In loop");
  if (!client.connected()) {
    reconnect();
  }
  //Serial.println("In loop");
  client.loop();
  long now = millis();
  
  bus3_default();
  if(true){
     // bus3_data();
      delayMicroseconds(180);
  }     
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


void bus3_default(){
    StaticJsonDocument<96> bus3;

    if(bv3Request){      
      bus3["d3_Bvol"] = "x";
      }else{bus3["d3_Bvol"] = -1;}
     if(lv3Request){
      bus3["d3_Lvol"] = "x"; 
      } else{bus3["d3_Lvol"] = -1;}
     if(cur3Request){
      bus3["d3_cur"] = "x";
      } else{bus3["d3_cur"] =-1;}
     if(pow3Request){
      bus3["d3_pow"] = "x";
      }else{bus3["d3_pow"]=-1;}
      bus3["data"] = "set End";
    //time_end=micros();
    //Serial.print(" Time "); Serial.print(time_end-time_start); Serial.println("us");
    
    char bus3_output[100];
    size_t n = serializeJson(bus3, bus3_output);
    //client.publish("outTopic", buffer, n);
    
    serializeJson(bus3, bus3_output);
    
    Serial.println("Sending message to MQTT topic..");
    Serial.println(bus3_output);
   
    if (client.publish("esp32/bus3", bus3_output) == true) {
      Serial.println("Success sending message");
    } else {
      Serial.println("Error sending message");
    }
}

int bus3_power(){
  ina226_2.readAndClearFlags();
  int power_temp = ina226_2.getBusPower();
  return power_temp;
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
    
    Serial.println("Sending message to MQTT topic..");
    Serial.println(bus3_output);
   
    if (client.publish("esp32/bus3", bus3_output) == true) {
      Serial.println("Success sending message");
    } else {
      Serial.println("Error sending message");
    }  
}

void checkStatus(){
  StaticJsonDocument<256> tel_status;

  tel_status["bv3_status"] = bv3Request;
  tel_status["lv3_status"] = lv3Request;
  tel_status["cur3_status"] = cur3Request;
  tel_status["pow3_status"] = pow3Request;
  

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
