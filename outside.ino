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
#include "Adafruit_MCP9808.h"

// Create the MCP9808 temperature sensor object
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

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
        WiFi.begin(ssid, password);
        started == 1;
        unsigned long connectMillis = millis();
         while (WiFi.status() != WL_CONNECTED) {
          if (connectMillis + 30000 <= millis()) {
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
    } else {
      if (connected == 0) {
        unsigned long connectMillis = millis();
        WiFi.reconnect();
        while (WiFi.status() != WL_CONNECTED) {
          if (connectMillis + 30000 <= millis()) {
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
  
  };

  bool send() {
    WiFiClient client;
    if (!client.connect(host, httpPort)) {
      failCount++;
      Serial.println(failCount);
      delay(1000);
      if (failCount == 5) {
        connected = 0;
        connect();
        failCount = 0;
      };
    };

    String url = "/thermostat_api.php?outSub=true&temp=";
    url = url + read();
    url = url + "&humidity=0&pressure=0";
    Serial.println(url);
    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
  };

  float read(){
    Serial.println("wake up MCP9808.... "); // wake up MCP9808 - power consumption ~200 mikro Ampere
    tempsensor.wake();   // wake up, ready to read!
  
    // Read and print out the temperature, also shows the resolution mode used for reading.
    float f = tempsensor.readTempF();
    tempsensor.shutdown_wake(1); // shutdown MSP9808 - power consumption ~0.1 mikro Ampere, stops temperature sampling
    return f;
  };
  
};

comm cc = comm();

void setup() {
  Serial.begin(9600);
  while (!Serial);
  if (!tempsensor.begin(0x18)) {
    Serial.println("Couldn't find MCP9808! Check your connections and verify the address is correct.");
    while (1);
  }
    
  Serial.println("Found MCP9808!");

  tempsensor.setResolution(3);

  cc.connect();

}

void loop() {
  cc.send();
  delay(1000);
  ESP.deepSleep(300e6);
       
}
