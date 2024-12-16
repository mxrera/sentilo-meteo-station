#include <Arduino.h>
#include "restapi.h"
#include "sensors.hpp"

#define TEMPERATURE_SENSOR "temperatura"
#define HUMIDITY_SENSOR "humitat"
#define PRESSURE_SENSOR "pressio"

unsigned long delay_time;
void printValues(float humidity, float temperature, float pressure);

void setup()
{
  Serial.begin(115200); // initialize serial communication at 115200 bits per second:
  while (!Serial);

  Serial.println("Starting setup...");
  initWifi(); // Initialize WiFi connection
  initSensors(); // Initialize sensors

  // Set delay time
  delay_time = 20000;
}

void loop()
{
  float humidity = getHumidity();
  float temperature = getTemperature();
  float pressure = getPressure();
  
  printValues(humidity, temperature, pressure);

  sendPostRequest(HUMIDITY_SENSOR, humidity);
  sendPostRequest(TEMPERATURE_SENSOR, temperature);
  sendPostRequest(PRESSURE_SENSOR, pressure);

  delay(delay_time);
}

void printValues(float humidity, float temperature, float pressure)
{
  Serial.println("--------------------");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" ºC");
  Serial.print("Pressure: ");
  Serial.print(pressure);
  Serial.println(" Pa");
  Serial.println();
}