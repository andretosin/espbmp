#include <SSD1306.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

SSD1306 display(0x3C, 21, 22);

const char* ssid = "Casa Sala 2.4G";
const char* password = "7621287700";
const char* serverAddress = "192.168.100.245";
const int serverPort = 80;

void connectToWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    displayPrint("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  displayPrint("Connected!");
}

void setupDisplay() {
  Wire.begin();
  display.init();
  displayPrint("Starting...");
}

void displayPrint(char* str) {
  display.resetDisplay();
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, str); 
  display.display();
}

void displayBMPPrint(float temp, int pres, float alt) {
  display.resetDisplay();
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Temperatura: " + String(temp) + " °C");
  display.drawString(0, 12, "Pressão.......: " + String(pres) + " Pa");
  display.drawString(0, 24, "Altitude........: " + String(alt) + " m");
  display.display();
}

void setup() {
  Serial.begin(9600);
  setupDisplay(); 
  connectToWiFi();
}

void loop() {
  HTTPClient http;

  String url = "http://" + String(serverAddress) + "/bmp180";
  Serial.println("Making GET request to: " + url);
  http.begin(url);
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    String response = http.getString();
    Serial.println("Response: " + response); 
    const size_t capacity = JSON_OBJECT_SIZE(3) + 70;
    DynamicJsonDocument jsonDoc(capacity);
    DeserializationError error = deserializeJson(jsonDoc, response);
    if (error) {
      Serial.print("Error during JSON parsing: ");
      Serial.println(error.c_str());
    } else {
      // Extrai os valores e armazena-os em variáveis
      float temperatura = jsonDoc["temperatura"];
      int pressao = jsonDoc["pressao"];
      float altitude = jsonDoc["altitude"];
      displayBMPPrint(temperatura, pressao, altitude); 
    }
  } else {
    Serial.println("Error: " + http.errorToString(httpCode));
  }
  http.end();
  delay(2000);
}
