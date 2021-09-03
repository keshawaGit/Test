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

  float shuntvoltage_1 = 0;
  float busvoltage_1 = 0;

  float shuntvoltage_2 = 0;
  float busvoltage_2 = 0;

  float shuntvoltage_3 = 0;
  float busvoltage_3 = 0;
  
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

boolean gpsRequest = true;
boolean tempRequest = true;
boolean humRequest =true;
boolean presRequest = true;
boolean coRequest = true;
boolean o3Request = true;
boolean no2Request = true;
boolean so2Request = true;


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
  //ina226.setAverage(AVERAGE_16); // choose mode and uncomment for change of default

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
  //ina226.setConversionTime(CONV_TIME_1100); //choose conversion time and uncomment for change of default
  
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
  pinMode(ledPin, OUTPUT);
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

  // If a message is received on the topic esp32/output, you check if the message  
  // Changes the output state according to the message
  if (String(topic) == "esp32/output") {
    Serial.print("Changing output to ");
    if(messageTemp == "TxON"){
      Serial.println("Transmitter ON");
      gpsRequest = true;
      //digitalWrite(ledPin, HIGH);
    }
    else if(messageTemp == "TxOFF"){
      Serial.println("Transmitter OFF");
      gpsRequest = false;
      //digitalWrite(ledPin, LOW);
    }     
  }
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
  
    //node_data();
    bus1_data();
    //bus2_data();
    //bus3_data();
    
  long now = millis();
  if (now - lastMsg > 1) {
    lastMsg = now;
    
    //node_data();
    //bus1_data();
    //bus2_data();
    //bus3_data();
  }
}

void node_data(){
    StaticJsonDocument<96> doc;

    doc["Device"] = "Node 1";
    doc["status"] = wl_status_to_string(WiFi.status());
    doc["network"] = String(WiFi.SSID());
    doc["rssi"] = String(WiFi.RSSI());
    doc["mqttIP"] = WiFi.localIP().toString();
  
    char output[100];
    size_t n = serializeJson(doc, output);
    //client.publish("outTopic", buffer, n);
    
    serializeJson(doc, output);
    
    Serial.println("Sending message to MQTT topic..");
    Serial.println(output);
   
    if (client.publish("esp32/data", output) == true) {
      Serial.println("Success sending message");
    } else {
      Serial.println("Error sending message");
    }
  
}

void bus1_data(){
  StaticJsonDocument<96> bus1;

  //bus1["shuntvoltage_1"] = ina219.getShuntVoltage_mV();
  //bus1["busvoltage_1"] = ina219.getBusVoltage_V();
  //shuntvoltage_1 = ina219.getShuntVoltage_mV();
  //busvoltage_1 = ina219.getBusVoltage_V();
 // bus1["loadvoltage_1"] = busvoltage_1 + (shuntvoltage_1 / 1000);
  //bus1["current_mA_1"] = ina219.getCurrent_mA();
  bus1["power_mW_1"] = ina219.getPower_mW();
  

  char bus1_output[100];
  serializeJson(bus1, bus1_output);
  Serial.println("Sending message to ESP32/Status");
  Serial.println(bus1_output);
 
  if (client.publish("esp32/Bus_1", bus1_output) == true) {
    Serial.println("Success Bus1");
  } else {
    Serial.println("Error Bus1");
  }
}

void bus2_data(){
  StaticJsonDocument<96> bus2;

  ina226_1.readAndClearFlags();
  //bus2["shuntvoltage_2"] = ina226_1.getShuntVoltage_mV();
  bus2["busvoltage_2"] = ina226_1.getBusVoltage_V();
  shuntvoltage_2 = ina226_1.getShuntVoltage_mV();
  busvoltage_2 = ina226_1.getBusVoltage_V();
  bus2["loadvoltage_2"] = busvoltage_2 + (shuntvoltage_2/1000);
  bus2["current_mA_2"] = ina226_1.getCurrent_mA();
  bus2["power_mW_2"] = ina226_1.getBusPower();
  

  char bus2_output[100];
  serializeJson(bus2, bus2_output);
  Serial.println("Sending message to ESP32/Status");
  Serial.println(bus2_output);
 
  if (client.publish("esp32/Bus_2", bus2_output) == true) {
    Serial.println("Success Bus2");
  } else {
    Serial.println("Error Bus2");
  }
}

void bus3_data(){
  StaticJsonDocument<96> bus3;

  ina226_2.readAndClearFlags();
  //bus3["shuntvoltage_3"] = ina226_2.getShuntVoltage_mV();
  bus3["busvoltage_3"] = ina226_2.getBusVoltage_V();
  shuntvoltage_3 = ina226_2.getShuntVoltage_mV();
  busvoltage_3 = ina226_2.getBusVoltage_V();
  bus3["loadvoltage_3"] = busvoltage_3 + (shuntvoltage_3/1000);
  bus3["current_mA_3"] = ina226_2.getCurrent_mA();
  bus3["power_mW_3"] = ina226_2.getBusPower();


  char bus3_output[100];
  serializeJson(bus3, bus3_output);
  Serial.println("Sending message to ESP32/Status");
  Serial.println(bus3_output);
 
  if (client.publish("esp32/Bus_3", bus3_output) == true) {
    Serial.println("Success Bus3");
  } else {
    Serial.println("Error Bus3");
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
