/**************************************************************************/
/*!
This is a demo for the Adafruit MCP9808 breakout
----> http://www.adafruit.com/products/1782
Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!
*/
/**************************************************************************/
#include <ESP8266WiFi.h>
#include <Wire.h>
#include "Adafruit_SHT31.h"

// Create the MCP9808 temperature sensor object
Adafruit_SHT31 tempsensor = Adafruit_SHT31();

class comm {
  public:
  comm()
  {

  }

  const char* ssid     = "errans";
  const char* password = "zamb0rah";
  const char* host = "192.168.1.143";
  const int httpPort = 80;
  int failCount = 0;
  bool connected = 0;
  bool started = 0;
  unsigned long serverTempMillis = 0;
  float f;

  bool connect() {
    if (started == 0) {
        Serial.print("Connecting to ");
        Serial.println(ssid);
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
        started == 1;
        IPAddress staticIP(192, 168, 1, 5); //static IP address
        IPAddress gateway(192, 168, 1, 1); //Router's IP address
        IPAddress subnet(255, 255, 255, 0);
        unsigned long connectMillis = millis();
        while (WiFi.status() != WL_CONNECTED) {
          if (connectMillis + 30000 <= millis()) {
            //we failed--reset and try again next time.
            Serial.println("Connect timed out.");
            Serial.println();
            WiFi.disconnect();
            connected = 0;
            return 1;
          };
          delay(1000);
          Serial.print(WiFi.status());
        };
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        connected = 1;
    } else {
      if (connected == 0) {
        unsigned long connectMillis = millis();
        WiFi.reconnect();
        while (WiFi.status() != WL_CONNECTED) {
          if (connectMillis + 60000 <= millis()) {
            //we failed--reset and try again next time.
            WiFi.disconnect();
            connected = 0;
            return 1;
          };
          delay(500);
          Serial.print(".");
        };
    
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        connected = 1;
        return 0;
      };
    };
    return 0;
  };

  bool send() {
    WiFiClient client;
    while (!client.connect(host, httpPort)) {
      failCount++;
      Serial.print("Server connect failed, #");
      Serial.println(failCount);
      delay(1000);
      if (failCount == 5) {
        connected = 0;
        connect();
        failCount = 0;
      };
    };

    String url = "/thermostat_api.php?inSub=true&temp=";
    url = url + readt();
    url = url + "&humidity=";
    url = url + readh();
    url = url + "&id=1";
    Serial.println(url);
    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    return 0;
  };

  float readt(){
    // Read and print out the temperature, also shows the resolution mode used for reading.
    float f = tempsensor.readTemperature();
    f = f * 9/5 + 32;
    return f;
  };

  float readh(){
    // Read and print out the temperature, also shows the resolution mode used for reading.
    float h = tempsensor.readHumidity();
    return h;
  };
  
};

comm cc = comm();

void setup() {
  Serial.begin(9600);
  while (!Serial);
  if (!tempsensor.begin(0x44)) {
    Serial.println("Couldn't find tempsensor! Check your connections and verify the address is correct.");
    while (1);
  }
    
  Serial.println("Found tempSensor!");

  tempsensor.heater(false);

  while (!cc.connect() == 0);

}

void loop() {
  cc.send();
  delay(300000);
  //ESP.deepSleep(30e6);
       
}
