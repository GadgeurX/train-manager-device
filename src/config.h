#ifndef CONFIG_H_
#define CONFIG_H_

#include <WiFi.h>

#define WIFI_SSID "Wifi_Philippe"
#define WIFI_PASSWORD "98956263"
IPAddress CENTRAL_IP(192,168,1,24);

#define RGB_LED_PIN 10

// LOCO --------------

#define IS_LOCO true
#define LOCO_MOTOR_PIN_FORWARD 0
#define LOCO_MOTOR_PIN_BACKWARD 2
#define LOCO_ID "BR208"

#endif