#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "tinyxml.h"

#include <FastLED.h>

#define NUM_LEDS 1
#define BUFFER_SIZE 1024

CRGB leds[NUM_LEDS];
String buffer = "";

// Initialize the client library
WiFiClient client;

bool initialise = false;

bool ToBoolean(String value)
    {
        if (value == "true")
        return true;
          else
        return false;
    }

IPAddress local_IP(192, 168, 1, 50);   // Adresse IP statique
IPAddress gateway(192, 168, 1, 0);    // Passerelle par défaut
IPAddress subnet(255, 255, 255, 0);

void setup() {
  Serial.begin(9600);
  while(!Serial);

  Serial.println("[INFO] Starting");

  FastLED.addLeds<WS2812, 10, RGB>(leds, NUM_LEDS);

  Serial.println("[INFO] Start Wifi STA");
  WiFi.mode(WIFI_STA);

  pinMode(LOCO_MOTOR_PIN_FORWARD, OUTPUT);
  pinMode(LOCO_MOTOR_PIN_BACKWARD, OUTPUT);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    leds[0] = CRGB::Red;
    FastLED.show();
    if (!WiFi.config(local_IP, gateway, subnet)) {
        Serial.println("Erreur de configuration de l'adresse IP");
    }
    Serial.print("[INFO] Connect to SSID ");
    Serial.println(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("[INFO] Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print('.');
      delay(1000);
    }
    leds[0] = CRGB::Yellow;
    FastLED.show();
    Serial.println();
    Serial.print("[INFO] Connected with ip ");
    Serial.println(WiFi.localIP());
  } else if (!client.connected()) {
    initialise = false;
    leds[0] = CRGB::Yellow;
    FastLED.show();
    Serial.print("[INFO] Connection to central at ");
    Serial.println(CENTRAL_IP);
    if (client.connect(CENTRAL_IP, 5555)) {
      Serial.println("[INFO] Connected to central");
      delay(500);
    } else {
      Serial.println("[INFO] Fail connect to central");
    }
  } else {
    leds[0] = CRGB::Green;
    FastLED.show();
    if (!initialise) {
      initialise = true;
      Serial.println("[INFO] Initialise");
      client.print("<setId id=\"");
      client.print(LOCO_ID);
      client.println(",\"/>");
    }
    String chara = client.readStringUntil('\n');
    Serial.print(chara);
    while (chara != nullptr) {
        Serial.println("[INFO] New CMD");
        Serial.println(buffer);
        TiXmlDocument doc = TiXmlDocument();
        doc.Parse(chara.c_str());

        auto value = doc.RootElement()->Attribute("V");
        auto dir = doc.RootElement()->Attribute("dir");
        Serial.println(value);
        if (ToBoolean(String(dir))) {
          Serial.println(String(value).toInt());
          Serial.println((256.0 / 100.0));
          Serial.println((256.0 / 100.0) * String(value).toInt());
          analogWrite(LOCO_MOTOR_PIN_FORWARD, int(trunc((1024.0 / 100.0) * String(value).toInt())));
          analogWrite(LOCO_MOTOR_PIN_BACKWARD, 0);
        } else {
          analogWrite(LOCO_MOTOR_PIN_FORWARD, 0);
          analogWrite(LOCO_MOTOR_PIN_BACKWARD, int(trunc((1024.0 / 100.0) * String(value).toInt())));
        }
        
        chara = client.readStringUntil('\n');
    }
    Serial.flush();
  }
}