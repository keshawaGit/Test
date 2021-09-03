#include <Wire.h>
//#include <Adafruit_INA219.h>
#include <INA226_WE.h>
#define I2C_ADDRESS_1 0x40
//#define I2C_ADDRESS_2 0x41

//Adafruit_INA219 ina219; // 0X40
INA226_WE ina226_1 = INA226_WE(I2C_ADDRESS_1);
//INA226_WE ina226_2 = INA226_WE(I2C_ADDRESS_2);


  float shuntvoltage_1 = 0;
  float busvoltage_1 = 0;
  float current_mA_1 = 0;
  float loadvoltage_1 = 0;
  float power_mW_1 = 0;


  
void setup() {
  Serial.begin(115200);
  while (!Serial) {
      delay(1);
  }

  uint32_t currentFrequency;
/*
  if (! ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }
  }*/
  // To use a slightly lower 32V, 1A range (higher precision on amps):
  //ina219.setCalibration_32V_1A();
  // Or to use a lower 16V, 400mA range (higher precision on volts and amps):
  //ina219.setCalibration_16V_400mA();
  
  ina226_1.init();
 // ina226_2.init();
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
  
  //ina226_1.waitUntilConversionCompleted(); //if you comment this line the first data might be zero
  //ina226_2.waitUntilConversionCompleted(); //if you comment this line the first data might be zero
}

void loop() {

 // ina226_1.readAndClearFlags();
  shuntvoltage_1 = ina226_1.getShuntVoltage_mV();
  busvoltage_1 = ina226_1.getBusVoltage_V();
  current_mA_1 = ina226_1.getCurrent_mA();
  power_mW_1 = ina226_1.getBusPower();
  loadvoltage_1  = busvoltage_1 + (shuntvoltage_1/1000);



  Serial.print("Bus 1 : ");
  Serial.print("B Vol:"); Serial.print(busvoltage_1); Serial.print(" V    ");
  Serial.print("S Vol:"); Serial.print(shuntvoltage_1); Serial.print(" mV   ");
  Serial.print("L Vol:"); Serial.print(loadvoltage_1); Serial.print(" V   ");
  Serial.print("Cur:"); Serial.print(current_mA_1); Serial.print(" mA    ");
  Serial.print("Pow:"); Serial.print(power_mW_1); Serial.println(" mW   ");

  delay(1000);
}
