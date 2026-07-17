#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define SOIL1_PIN 34

const char* ssid = "POCO";
const char* password = "11111111";

const char* mqtt_server = "c96739e1879046b4b64c91e3ffc10c6a.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_user = "node_uasklp5";
const char* mqtt_pass = "uasklp5!";

WiFiClientSecure espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Mencoba connect ke MQTT... ");
    if (client.connect("Node2_Air_ESP32", mqtt_user, mqtt_pass)) {
      Serial.println("berhasil!");
    } else {
      Serial.print("gagal, rc=");
      Serial.print(client.state());
      Serial.println(" coba lagi dalam 2 detik");
      delay(2000);
    }
  }
}

String tentukanLevelAir(int nilai) {
  if (nilai > 3367) {
    return "RENDAH";
  } else if (nilai > 2059) {
    return "SEDANG";
  } else {
    return "TINGGI";
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  espClient.setInsecure();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  int nilaiAir = analogRead(SOIL1_PIN);
  String levelAir = tentukanLevelAir(nilaiAir);

  StaticJsonDocument<128> doc;
  doc["level_air_raw"] = nilaiAir;
  doc["level_air_status"] = levelAir;

  char buffer[128];
  serializeJson(doc, buffer);

  client.publish("environment/node2", buffer);

  Serial.print("Nilai Air: "); Serial.print(nilaiAir);
  Serial.print(" | Status: "); Serial.println(levelAir);

  delay(3000);
}
