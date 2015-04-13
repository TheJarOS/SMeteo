/*
ArduinoCode.ino
13.04.2015 The JarOS/Mateusz Jaros

Wiring:
  DHT11/22 ---> 2 pin
  LDR ---> A0 pin
  Rain Sensor ---> A1 pin
  DS18B20 ---> 3 pin
  LED ---> 13 pin

If LED is fast bilniking there is a problem with wiring sensors.


You need to send '1' on Serial to get data from sensors.
  Data which you rceive from serial is in format:
    temperature   humidity  preasure  light rain.
*/

// Liblaries
#include <Wire.h> // Requied for OneWire
#include <Adafruit_BMP085.h> // Need for BMP085
#include "DHT.h" // Library for DHT 11
#include <OneWire.h> // OneWire library
#include <DallasTemperature.h> // For DS18B20

// Pins 
const int DHTpin = 2; // DHT11
const int LDRpin = A0; // LDR
const int RAINpin = A1; // analog sensor for rain
const int DS18B20pin = 3; // DS18B20
const int LEDpin = 13;
// Objects && Others
DHT dht(DHTpin, DHT11); // You can switch between DHT 11 or DHT 22 by commentnig & uncomenting 
//DHT dht(DHTpin, DHT22); // DHT22 
OneWire oneWire(DS18B20pin);
DallasTemperature sensor(&oneWire);
Adafruit_BMP085 bmp;
bool error = false;

void setup() {
  // Stting pins as output
  pinMode(LDRpin, INPUT);
  pinMode(RAINpin, INPUT);
  
  Serial.begin(9600);
  dht.begin();
  // Is it working? Let's test it :D 
  if(!bmp.begin())
    error = true;
  sensor.requestTemperatures();
  if(sensor.getTempCByIndex(0) < -100)
    error = true;
  
  // blink the LED if error 
  if(error) {
    while(true){
     digitalWrite(LEDpin,HIGH);
     delay(100);
     digitalWrite(LEDpin, LOW);
     delay(100);
    }
  }
}

void loop() {
  // Getting:
  sensor.requestTemperatures();
  float temperature = sensor.getTempCByIndex(0); // temperature in celsius
  int humidity = dht.readHumidity(); // humidity RH
  int preassure = bmp.readPressure()/100; // preasure in hPa
  float light = Light(); // light in lux 
  int rain = map(analogRead(RAINpin), 0, 1023, 0, 100); // rain in %

  if(Serial.available() > 0){ 
    int val = Serial.read() -'0';
    	if(val == 1){ // If you print '1' on Serial print:
        Serial.print(temperature);
    	  Serial.print("\t");
    	  Serial.print(humidity);
    	  Serial.print("\t");
    	  Serial.print(preassure);
    	  Serial.print("\t");
    	  Serial.print(light);
    	  Serial.print("\t");
    	  Serial.println(rain);
      }
  }
}
// calculating ADC to LUX 
double Light () {
  int RawADC0 = analogRead(LDRpin);
  double Vout=RawADC0*0.0048828125;
  //int lux=500/(10*((5-Vout)/Vout));//use this equation if the LDR is in the upper part of the divider
  float lux=(2500/Vout-500)/10;
  return lux;
}
