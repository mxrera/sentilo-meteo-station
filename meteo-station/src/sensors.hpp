#ifndef SENSORS_H
#define SENSORS_H

void initBME();
void initSensors();
float getHumidity();
float getTemperature();
float getPressure();

#endif