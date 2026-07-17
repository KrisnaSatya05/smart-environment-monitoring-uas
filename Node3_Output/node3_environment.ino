#define BLYNK_TEMPLATE_ID "TMPL6aTzQp7V6"
#define BLYNK_TEMPLATE_NAME "Smart Environment Monitoring"
#define BLYNK_AUTH_TOKEN "ss3MnHaihcVFn9egO-PfxZEr1b5v2WIU"

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <BlynkSimpleEsp8266.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define LED_HIJAU D1
#define LED_MERAH D2
#define BUZZER D4

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

char auth[] = BLYNK_AUTH_TOKEN;
const char* ssid = "POCO";
const char* password = "11111111";

const char* mqtt_server = "c96739e1879046b4b64c91e3ffc10c6a.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_user = "node_uasklp5";
const char* mqtt_pass = "uasklp5!";

WiFiClientSecure espClient;
PubSubClient client(espClient);

int gasValue = 0;
float suhuValue = 0;
float lembabValue = 0;
int levelAirRaw = 4095;
String levelAirStatus = "RENDAH";

void tampilkanOLED(String status) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.print("S:");
  display.print(suhuValue, 1);
  display.print("C L:");
  display.print(lembabValue, 1);
  display.println("%");

  display.setCursor(0, 10);
  display.print("Gas:");
  display.print(gasValue);
  display.print(" Air:");
  display.println(levelAirStatus);

  display.drawLine(0, 20, 128, 20, SSD1306_WHITE);

  display.setTextSize(2);
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(status, 0, 0, &x1, &y1, &w, &h);
  int xPos = (128 - w) / 2;
  display.setCursor(xPos, 23);
  display.println(status);

  display.display();
}

void evaluasiStatus() {
  digitalWrite(LED_HIJAU, LOW);
  digitalWrite(LED_MERAH, LOW);
  digitalWrite(BUZZER, LOW);

  bool airBahaya = (levelAirStatus == "TINGGI");
  bool airSiaga = (levelAirStatus == "SEDANG");
  bool gasBahaya = gasValue > 700;
  bool gasSiaga = gasValue > 400 && gasValue <= 700;

  String status;

  if (airBahaya || gasBahaya) {
    digitalWrite(LED_MERAH, HIGH);
    digitalWrite(BUZZER, HIGH);
    status = "BAHAYA";
  } else if (airSiaga || gasSiaga) {
    digitalWrite(LED_HIJAU, HIGH);
    digitalWrite(LED_MERAH, HIGH);
    status = "SIAGA";
  } else {
    digitalWrite(LED_HIJAU, HIGH);
    status = "AMAN";
  }

  Blynk.virtualWrite(V5, status);
  tampilkanOLED(status);

  if (status == "BAHAYA") {
    Blynk.logEvent("kondisi_bahaya", "Peringatan! Kondisi lingkungan terdeteksi bahaya");
  }

  Serial.print("Status: "); Serial.println(status);
}

void callback(char* topic, byte* payload, unsigned int length) {
  String pesan;
  for (unsigned int i = 0; i < length; i++) {
    pesan += (char)payload[i];
  }

  String topicStr = String(topic);

  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, pesan);

  if (error) {
    Serial.println("Gagal parsing JSON");
    return;
  }

  if (topicStr == "environment/node1") {
    suhuValue = doc["suhu"];
    lembabValue = doc["kelembaban"];
    gasValue = doc["gas"];

    Blynk.virtualWrite(V0, suhuValue);
    Blynk.virtualWrite(V1, lembabValue);
    Blynk.virtualWrite(V2, gasValue);
  }

  if (topicStr == "environment/node2") {
    levelAirRaw = doc["level_air_raw"];
    levelAirStatus = doc["level_air_status"].as<String>();

    Blynk.virtualWrite(V3, levelAirRaw);
    Blynk.virtualWrite(V4, levelAirStatus);
  }

  evaluasiStatus();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Mencoba connect ke MQTT... ");
    if (client.connect("Node3_Output", mqtt_user, mqtt_pass)) {
      Serial.println("berhasil!");
      client.subscribe("environment/node1");
      client.subscribe("environment/node2");
    } else {
      Serial.print("gagal, rc=");
      Serial.print(client.state());
      Serial.println(" coba lagi dalam 2 detik");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_HIJAU, OUTPUT);
  pinMode(LED_MERAH, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  Wire.begin(D7, D6);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED gagal terdeteksi");
  } else {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Menghubungkan...");
    display.display();
  }

  Blynk.begin(auth, ssid, password);
  espClient.setInsecure();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  Blynk.run();
  if (!client.connected()) reconnect();
  client.loop();
}
