#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define DHTPIN 4 // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22 // DHT 22 (AM2302)

Adafruit_BMP280 bmp; // I2C
DHT dht(DHTPIN, DHTTYPE);

float previous_tempreature, previous_humidity, previous_pressure;

void initBME() {  
  if (!bmp.begin(0x76)) {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1) delay(10);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_FORCED,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}

void initSensors() {
    initBME(); // Initialize BME sensor
    dht.begin(); // Initialize DHT sensor
    previous_humidity = 0.0;
    previous_tempreature = 0.0;
    previous_pressure = 0.0;
}

float getHumidity() {
  float humidity = dht.readHumidity();
  if (isnan(humidity)) {
    humidity = previous_humidity;
  }
  previous_humidity = humidity;
  return humidity;
}

float getTemperature() {
  float temperature = dht.readTemperature();
  if (isnan(temperature)) {
    temperature = previous_tempreature;
  }
  previous_tempreature = temperature;
  return temperature;
}

float getPressure() {
  if (!bmp.takeForcedMeasurement()) {
    Serial.println("Could not read data from BMP280 sensor");
    return previous_pressure;
  }
  float pressure = bmp.readPressure();
  if (isnan(pressure)) {
    pressure = previous_pressure;
  }
  previous_pressure = pressure;
  return pressure;
}
