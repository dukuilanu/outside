#include <ESP8266WiFi.h>
#include "Adafruit_MCP9808.h"

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

  void sendToServer()
  {
    unsigned long currentTempMillis = millis();
    if (currentTempMillis >= serverTempMillis + 60000) {
      serverTempMillis = millis();
      float c = tempsensor.readTempC();
      float f = c * 9.0 / 5.0 + 32;

    };

    WiFiClient client;
    if (!client.connect(host, httpPort)) {
      failCount++;
      Serial.println(failCount);
      delay(1000);
      if (failCount == 5) {
        connected = 0;
        connect();
        failCount = 0;
        return 1;
      };
      return 1;
    };

    String url = "/thermostat_api.php?outSub=true&temp=";
    url = url + f;
    url = url + "&humidity=0&pressure=0";
    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");

  };

};

comm cc = comm();

void setup() {
  Serial.begin(9600);
  delay(100);

  cc.connect();

  cc.serverTempMillis = millis();

}

void loop() {
  cc.sendToServer();
}
