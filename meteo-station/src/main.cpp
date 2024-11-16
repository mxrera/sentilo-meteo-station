// Libraries
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <DHT.h>
#include "restapi.h"
// >>>>>>>>>> DHT variables
#define DHTPIN 4 // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.
#define DHTTYPE DHT22 // DHT 22 (AM2302)
#define TEMPERATURE_SENSOR "temperatura"
#define HUMIDITY_SENSOR "humitat"

// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview

DHT dht(DHTPIN, DHTTYPE);
// <<<<<<<<<<<< DHT variables

// >>>>>>>>>> Global variables
float previous_tempreature, previous_humidity;

void setup()
{
  Serial.begin(115200); // initialize serial communication at 115200 bits per second:

  initWifi(); // Initialize WiFi connection

  // Initialize DHT sensor
  dht.begin();
  previous_humidity = 0.0;
  previous_tempreature = 0.0;
}

void loop()
{
  float humidity = 10;
  float temperature = 20;
  sendPostRequest(HUMIDITY_SENSOR, humidity);
  sendPostRequest(TEMPERATURE_SENSOR, temperature);
  delay(20000);
}

double getHumidity()
{
  float humidity = dht.readHumidity();
  if (isnan(humidity))
  {
    humidity = previous_humidity;
  }
  previous_humidity = humidity;
  return humidity;
}

double getTemperature()
{
  float temperature = dht.readTemperature();
  if (isnan(temperature))
  {
    temperature = previous_tempreature;
  }
  previous_tempreature = temperature;
  return temperature;
}
