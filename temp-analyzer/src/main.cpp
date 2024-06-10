// Libraries
#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_I2CDevice.h> 
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

// >>>>>>>>>> DHT variables
#define DHTPIN 4     // Digital pin connected to the DHT sensor 
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.
#define DHTTYPE    DHT22     // DHT 22 (AM2302)

// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview

DHT dht(DHTPIN, DHTTYPE);
// <<<<<<<<<<<< DHT variables

// >>>>>>>>>> SSD1306 variables
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#define MAX_BRIGHTNESS 255

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// <<<<<<<<<<<< SSD1306 variables

// >>>>>>>>>> SD variables
String data_file = "";
void initSDCard();
void appendFile(fs::FS &fs, const char * path, const char * message);
int getDirFiles(fs::FS &fs, const char * dirname);
// <<<<<<<<<<<< SD variables


void initSSD1306();           // Initialization of display

void setup(){
  Serial.begin(115200); // initialize serial communication at 115200 bits per second:

  // Initialize SD card
  initSDCard();
  // Initialize DHT sensor
  dht.begin();
  //Initilize SSD1306
  initSSD1306();
}

void loop(){
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)){ 
    t = 00.00;
    h = 000.00;
  }

  String temperature = "Temperature: " + String(t) + " C";
  String humidity = "Humidity: " + String(h) + " %";
  String out_data = "File: " + data_file;
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);

  display.println(out_data);
  display.println(temperature);
  display.println(humidity);

  display.display();

  String data = "  - - " + String(t) + "\n    - " + String(h) + "\n";
  appendFile(SD, data_file.c_str(), data.c_str());

  delay(5000);
}

void initSSD1306 (){
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();
  delay(2000);
  display.clearDisplay();
  delay(2000);
}

void initSDCard(){
  if(!SD.begin()){
    Serial.println("Card Mount Failed");
    return;
  }
  int id = getDirFiles(SD, "/");
  data_file = "/data-" + String(id) + ".yaml";
  appendFile(SD, data_file.c_str(), "data: \n");
}

int getDirFiles(fs::FS &fs, const char * dirname){
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if(!root){
    Serial.println("Failed to open directory");
    return 0;
  }
  if(!root.isDirectory()){
    Serial.println("Not a directory");
    return 0;
  }

  File file = root.openNextFile();
  int number_of_files = 0;
  while(file){
    if(file.isDirectory()){
      Serial.print("  DIR : ");
      Serial.println(file.name());
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
      number_of_files++;
    }
    file.close();
    file = root.openNextFile();
  }
  return number_of_files;
}

void appendFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file){
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)){
      Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}
