#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

#define DHTPIN D4
#define DHTTYPE DHT22
#define MQPIN A0

const char* ssid = "POCO";
const char* password = "11111111";

const char* mqtt_server = "c96739e1879046b4b64c91e3ffc10c6a.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_user = "node_uasklp5";
const char* mqtt_pass = "uasklp5!";

WiFiClientSecure espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("Node1_Udara", mqtt_user, mqtt_pass)) {
      // berhasil nyambung
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  setup_wifi();
  espClient.setInsecure();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  float suhu = dht.readTemperature();
  float lembab = dht.readHumidity();
  int gas = analogRead(MQPIN);

  if (!isnan(suhu) && !isnan(lembab)) {
    StaticJsonDocument<128> doc;
    doc["suhu"] = suhu;
    doc["kelembaban"] = lembab;
    doc["gas"] = gas;

    char buffer[128];
    serializeJson(doc, buffer);

    client.publish("environment/node1", buffer);

    Serial.println(buffer);
  }

  delay(3000);
}
