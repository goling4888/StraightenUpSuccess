#ifndef NECKPOSTURESENSING
#define NECKPOSTURESENSING

#include <M5StickCPlus.h>
#include <utility/MPU6886.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <esp_now.h>

const byte broadcastAddress[] = {0xA0,0xA3,0xB3,0x2A,0x75,0xC0};    //Addr of desk ESP32
const byte headAddr[] = {0xD4, 0xD4, 0xDA, 0xBC, 0xF6, 0xA0};
const byte cervicalAddr[] = {0x64, 0xB7, 0x08, 0x80, 0xEA, 0xE4};
const float GRAVITY=9.80665;
const int BAD_HEAD_ANGLE=30;
const int BAD_CERVICAL_ANGLE=45;


typedef struct struct_message {
    uint8_t id;
    float data;
} struct_message;

/**
 * Callback function for when data is sent
 * 
 * @param mac_addr Destination MAC Address
 * @param status Status of packet sent
 * @return none
 * 
*/
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);

/**
 * Calibrates accelerometer for IMU
 * 
 * @param none
 * @return none
 * 
*/
void calibrateAccel();

/**
 * Displays IMU angle onto M5 Stick's LCD
 * 
 * @param none
 * @return none
 * 
*/
void getAngle();

#endif