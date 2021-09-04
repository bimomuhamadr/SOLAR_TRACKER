#include <Wire.h>
#include <Adafruit_INA219.h>
Adafruit_INA219 ina219;
//-----------------------------------------//
float shuntvoltage = 0;
float busvoltage = 0;
float current_mA = 0;
float loadvoltage = 0;
float power_mW = 0;
//-----------------------------------------//
#include <BH1750.h>
BH1750 lightMeter;
//-----------------------------------------//
int period = 1000;
unsigned long time_now = 0;
//-----------------------------------------//
#define ANALOG_IN_PIN A0
float adc_voltage = 0.0;
float in_voltage = 0.0;
float R1 = 30000.0;
float R2 = 7500.0;
float ref_voltage = 5.0;
int adc_value = 0;
//-----------------------------------------//
#include <Servo.h>
Servo horizontal;
int servoh = 90;
int servohLimitHigh = 170;
int servohLimitLow = 10;
Servo vertical;
int servov = 90;
int servovLimitHigh = 130;
int servovLimitLow = 50;
int ldrlt = 1; //LDR top left - BOTTOM LEFT <--- BDG
int ldrrt = 2; //LDR top rigt - BOTTOM RIGHT
int ldrld = 3; //LDR down left - TOP LEFT
int ldrrd = 4; //ldr down rigt - TOP RIGHT

void setup() {
  Serial.begin(9600);
  ina219.begin();
  //  ina219.setCalibration_32V_2A();    // set measurement range to 32V, 2A  (do not exceed 26V!)
  //  ina219.setCalibration_32V_1A();    // set measurement range to 32V, 1A  (do not exceed 26V!)
  //  ina219.setCalibration_16V_400mA(); // set measurement range to 16V, 400mA
  //-----------------------------------------//
  Wire.begin();
  lightMeter.begin();
  //-----------------------------------------//
  horizontal.attach(10);
  vertical.attach(9);
  horizontal.write(90);
  vertical.write(90);
  delay(1000);
}

void loop() {
  if (millis() > time_now + period) {
    time_now = millis();
    SensorLux();
    SensorCurrent();
    SensorVoltage();
    Serial.println();
  }
  Tracker();
}

void SensorCurrent() {
  // read data from ina219
  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  Serial.print("v "); Serial.println(busvoltage);
  Serial.print("Shunt Voltage : "); Serial.print(shuntvoltage); Serial.println(" mV");
  Serial.print("Load Voltage  : "); Serial.print(loadvoltage); Serial.println(" V");
  Serial.print("ma "); Serial.println(current_mA);
  Serial.print("mw "); Serial.println(power_mW);
}

void SensorLux() {
  float lux = lightMeter.readLightLevel();
  Serial.print("lux "); Serial.println(lux);
}

void SensorVoltage() {
  adc_value = analogRead(ANALOG_IN_PIN);
  adc_voltage  = (adc_value * ref_voltage) / 1024.0;
  in_voltage = adc_voltage / (R2 / (R1 + R2));
  Serial.print("Input Voltage = ");
  Serial.println(in_voltage, 2);
}

void Tracker() {
  int lt = analogRead(ldrlt); // top left
  int rt = analogRead(ldrrt); // top right
  int ld = analogRead(ldrld); // down left
  int rd = analogRead(ldrrd); // down right
  int dtime = 30; int tol = 90; // dtime=diffirence time, tol=toleransi
  int avt = (lt + rt) / 2; // average value top
  int avd = (ld + rd) / 2; // average value down
  int avl = (lt + ld) / 2; // average value left
  int avr = (rt + rd) / 2; // average value right
  int dvert = avt - avd; // check the diffirence of up and down
  int dhoriz = avl - avr;// check the diffirence og left and rigt
  if (-1 * tol > dvert || dvert > tol) // check if the diffirence is in the
    //tolerance else change vertical angle
  {
    if (avt > avd)
    {
      servov = ++servov;
      if (servov > servovLimitHigh)
      {
        servov = servovLimitHigh;
      }
    }
    else if (avt < avd)
    {
      servov = --servov;
      if (servov < servovLimitLow)
      {
        servov = servovLimitLow;
      }
    }
    vertical.write(servov);
  }
  if (-1 * tol > dhoriz || dhoriz > tol) // check if the diffirence is in the
    //tolerance else change horizontal angle
  {
    if (avl > avr)
    {
      servoh = --servoh;
      if (servoh < servohLimitLow)
      {
        servoh = servohLimitLow;
      }
    }
    else if (avl < avr)
    {
      servoh = ++servoh;
      if (servoh > servohLimitHigh)
      {
        servoh = servohLimitHigh;
      }
    }
    else if (avl = avr)
    {
      delay(1000);
    }
    horizontal.write(servoh);
  }
  delay(dtime);
}
