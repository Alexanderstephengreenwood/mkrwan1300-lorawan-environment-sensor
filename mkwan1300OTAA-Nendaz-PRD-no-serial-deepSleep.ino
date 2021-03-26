/*
This sketch uses the arduino mkr1300 to send environment data over the lorawan network
using the Boesch BME680 sensor and driver

The sketch is based on the mkrwan example LoraSendAndReceive
https://github.com/arduino-libraries/MKRWAN/tree/master/examples/LoraSendAndReceive

and the Boesch BSEC-Arduino-library
https://github.com/BoschSensortec/BSEC-Arduino-library/blob/master/examples/basic/basic.ino

Sensor is the Adafruit sensor. Connection
VCC to VCC
GRD to GRD
SCL to SCL
SDR to SDR

Due to Lorawan basic carrier subscription the data sent is limited to 2 payloads per hour in my case
You can read more about Lorawan and LPN for my project at this link
https://github.com/Alexanderstephengreenwood/LoRa-Network-Mkr1300.git

Todo: Add battery level measurement on the external AAA or AA input connector and send it over the LPN to the App server for maintenance
*/

#include "ArduinoLowPower.h"
#include <MKRWAN.h>
#include "bsec.h"
#include <math.h>;
void checkIaqSensorStatus(void);
void errLeds(void);
String output;
Bsec iaqSensor;
LoRaModem modem;
#include "arduino_secrets.h"
String appEui = SECRET_APP_EUI;
String appKey = SECRET_APP_KEY;
String devEUI = SECRET_DEV_EUI;

void setup() {
    //Initialise BME680 ******************************************************************/
    Wire.begin(); //BME680 Wire comms
    iaqSensor.begin(BME680_I2C_ADDR_PRIMARY, Wire); //initialise the BME680
    checkIaqSensorStatus();
    bsec_virtual_sensor_t sensorList[10] = {
    BSEC_OUTPUT_RAW_TEMPERATURE,
    BSEC_OUTPUT_RAW_PRESSURE,
    BSEC_OUTPUT_RAW_HUMIDITY,
    BSEC_OUTPUT_RAW_GAS,
    BSEC_OUTPUT_IAQ,
    BSEC_OUTPUT_STATIC_IAQ,
    BSEC_OUTPUT_CO2_EQUIVALENT,
    BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
  };
    iaqSensor.updateSubscription(sensorList, 10, BSEC_SAMPLE_RATE_LP);
   
//Lora initialisation********************************************************/
    if (!modem.begin(EU868)) {
      while (1) {}
    };
    delay(10000); // give the modem some time to initialise

    int connected = modem.joinOTAA(appEui, appKey, devEUI);
      if (!connected){
          blinking(); //blink on the board to indicate the OTAA failes
          while (1) {}
    }
}

void loop() {
  int temp;
  int hum;
  int pres;
  int airquality;

  if (iaqSensor.run()) { 
      temp = iaqSensor.rawTemperature;
      hum = iaqSensor.rawHumidity;
      pres = iaqSensor.pressure;
      airquality = iaqSensor.staticIaq;
  } else {
      checkIaqSensorStatus();
    }
   int presbitify = round(((pres / 100.00) - 800)); //reduce the result to a 1 bite value to reduce the payload size on the lorawan network.
   String msg = "";
   msg.concat(temp);
   msg.concat(",");
   msg.concat(hum);
   msg.concat(",");
   msg.concat(presbitify);
   msg.concat(",");
   msg.concat(airquality);

  int err;
  modem.beginPacket();
  modem.print(msg); //send the packet over the Lorawan in HEX base64
  err = modem.endPacket(true);
  if (err > 0) {
        //all went well the data was sent
  } else {
    blinking(); //something went wrong with OTAA. No signal? Wrong credentials?
  }
  LowPower.deepSleep(1800 * 1000); //Good night for the 30 minutes. This is used because the mkrwan is connected to 2x AA or AAA batteries
}

// Helper function definitions BME680
void checkIaqSensorStatus(void)
{
  if (iaqSensor.status != BSEC_OK) {
    if (iaqSensor.status < BSEC_OK) {
      output = "BSEC error code : " + String(iaqSensor.status);
     // Serial.println(output);
      for (;;)
        errLeds(); /* Halt in case of failure */
    } else {
      output = "BSEC warning code : " + String(iaqSensor.status);
     // Serial.println(output);
    }
  }

  if (iaqSensor.bme680Status != BME680_OK) {
    if (iaqSensor.bme680Status < BME680_OK) {
      output = "BME680 error code : " + String(iaqSensor.bme680Status);
    //  Serial.println(output);
      for (;;)
        errLeds(); /* Halt in case of failure */
    } else {
      output = "BME680 warning code : " + String(iaqSensor.bme680Status);
    //  Serial.println(output);
    }
  }
}

void errLeds(void)
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
}

void blinking(){
      digitalWrite(LED_BUILTIN, HIGH);
      delay(250);
      digitalWrite(LED_BUILTIN, LOW);
      delay(250);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(250);
      digitalWrite(LED_BUILTIN, LOW);
      delay(250);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(250);
      digitalWrite(LED_BUILTIN, LOW);
      delay(250);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(250);
      digitalWrite(LED_BUILTIN, LOW);
      delay(250);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(250);
      digitalWrite(LED_BUILTIN, LOW);
      delay(250);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(250);
      digitalWrite(LED_BUILTIN, LOW);
      delay(250);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(250);
      digitalWrite(LED_BUILTIN, LOW);
      delay(250);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(250);
      digitalWrite(LED_BUILTIN, LOW);
      delay(250);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(250);
      digitalWrite(LED_BUILTIN, LOW);
      delay(250);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(250);
      digitalWrite(LED_BUILTIN, LOW);
  }

  void blinkingSlow(){
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW);
      delay(1000);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW);
      delay(1000);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW);
      delay(1000);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW);
      delay(1000);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW);
      delay(1000);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW);
      delay(1000);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW);
      delay(1000);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW);
      delay(1000);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW);
      delay(1000);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(1000);
      digitalWrite(LED_BUILTIN, LOW);
  }
