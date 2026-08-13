#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "Adafruit_SHT31.h"

// Create the MCP9808 temperature sensor object
Adafruit_SHT31 tempsensor = Adafruit_SHT31();
WiFiClient espClient;
PubSubClient client(espClient);
  

class comm {
  public:
  comm()
  {

  }

  const char* ssid     = "errans.iot";
  const char* password = "zamb0rah";
  const char* host = "homeassistant.iot";
  const int mqtt_port = 1883;

  int failCount = 0;
  bool connected = 0;
  bool started = 0;
  unsigned long serverTempMillis = 0;
  float f;



  bool wfconnect() {
    if (started == 0) {
        Serial.print("Connecting to ");
        Serial.println(ssid);
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
        started == 1;
        //IPAddress staticIP(192, 168, 1, 5); //static IP address
        //IPAddress gateway(192, 168, 1, 1); //Router's IP address
        //IPAddress subnet(255, 255, 255, 0);
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

  bool mqttconnect() {
    while (!client.connected()) {
      Serial.print("Attempting MQTT connection...");
      client.setServer(host, mqtt_port);
      // Create a unique client ID based on ESP MAC address
      String clientId = "8266iot-bedroom" + String(random(0, 1000));
      
      if (client.connect(clientId.c_str(), "mqttuser", "949500")) {
        Serial.println("connected");
      } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" trying again in 5 seconds");
        delay(5000);
      };
    };
    return 0;
  };

  bool send() {
    if (!client.connected()) {
      Serial.println("Send loop needs to reconnect MQTT");
      mqttconnect();
    };

    char* mqtt_topic = "bedroom/sensor/temperature";
    char payloadStr[8];
    dtostrf(readt(), 1, 2, payloadStr);

    Serial.print("Publishing sensor reading: ");
    Serial.println(payloadStr);
    
    if (client.publish(mqtt_topic, payloadStr)) {
      Serial.println("Publish successful!");
    } else {
      Serial.println("Publish failed.");
    };

    mqtt_topic = "bedroom/sensor/humidity";
    dtostrf(readh(), 1, 2, payloadStr);

    Serial.print("Publishing sensor reading: ");
    Serial.println(payloadStr);
    
    if (client.publish(mqtt_topic, payloadStr)) {
      Serial.println("Publish successful!");
    } else {
      Serial.println("Publish failed.");
    };

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

  while (!cc.wfconnect() == 0);
  cc.mqttconnect();

}

void loop() {
  cc.send();
  Serial.println("sent, delaying...");
  delay(300000);
  Serial.println("delayed, looping");
  //ESP.deepSleep(30e6);
       
}
