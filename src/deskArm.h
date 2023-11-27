#ifndef DESKARM
#define DESKARM

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <esp_now.h>

typedef struct struct_message {
  uint8_t id;
  float data;
}struct_message;

// Motor A
const int motor1Pin1 = 27; 
const int motor1Pin2 = 26; 
const int enable1Pin = 14; 

// Setting PWM properties
const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;
const int dutyCycle = 200;

void driveMotor();
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len);

#endif