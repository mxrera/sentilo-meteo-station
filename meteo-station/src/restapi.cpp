#include <WiFi.h>
#include <HTTPClient.h>

const char *ssid = "***";
const char *password = "***";

const char *serverUrl = "http://147.83.83.21:8081/data/grup_2-102@prov";
const char *identityKey = "d25c8f8b85d154df01129853c84cae0a678742c0791ef893c7e2548eed5a3a2c";

void initWifi()
{
    Serial.print("Connecting to WiFi..");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("Connected to the WiFi network");
}

void sendPostRequest(const char *sensor, float value)
{
    HTTPClient http;
    String url = String(serverUrl) + "/" + sensor + "/" + value;

    http.begin(url.c_str());
    http.addHeader("IDENTITY_KEY", identityKey);

    int httpResponseCode = http.PUT("");

    if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.print("Send PUT request to ");
        Serial.print(url);
        Serial.print(" with HTTP response code ");
        Serial.print(httpResponseCode);
        Serial.print(" (");
        Serial.print(response);
        Serial.println(").");
    } else {
        Serial.print("Error on sending PUT: ");
        Serial.println(httpResponseCode);
    }
    http.end();
}
