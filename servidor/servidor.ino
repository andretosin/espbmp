#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <Adafruit_BMP085.h>


const char *SSID = "Casa Sala 2.4G";
const char *PWD = "7621287700";
WebServer server(5003);
Adafruit_BMP085 bmp;
StaticJsonDocument<250> jsonDocument;
char buffer[250];

void setupBMP() {
  if (!bmp.begin()) {
	  Serial.println("Could not find a valid BMP085/BMP180 sensor, check wiring!");
	  while (1) { }
  }
}

void create_json(float temp, float pres, float alt) {
  jsonDocument.clear();
  jsonDocument["temperatura"] = temp;
  jsonDocument["pressao"] = pres;
  jsonDocument["altitude"] = alt;
  serializeJson(jsonDocument, buffer);
}


void connectToWifi() {
  Serial.print("Connecting to ");
  Serial.println(SSID);
  WiFi.begin(SSID, PWD);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    // we can even make the ESP32 to sleep
  }

  Serial.print("Connected. IP: ");
  Serial.println(WiFi.localIP());
}

void setupRouting() {
  server.on("/bmp180", getBMP);
  Serial.println("server.begin()");
  server.begin();
}

void getBMP() {
  Serial.println("getBMP");
  create_json(bmp.readTemperature(), bmp.readPressure(), bmp.readAltitude());
  server.send(200, "application/json", buffer);
}



void setup() {
  Serial.begin(9600);
  connectToWifi();
  setupRouting();
  setupBMP();
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
}
