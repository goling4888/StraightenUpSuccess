#include <Arduino.h>
#include "deskArm.h"
#include <iostream>
#include <fstream>
// #include <ofstream>
using namespace std;

struct_message myData;
struct_message board1;
struct_message board2;
struct_message boardsStruct[numSenders] = {board1, board2};

int goodHeadPostureCount, goodCervPostureCount; // in terms of seconds
int badPostureCounts;
int initTime;
int boardBool[numSenders] = {0,0};
// Driving Motor code
void driveMotor()
{
  Serial.println("Moving Forward");
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH); 
  delay(500);

  // Stop the DC motor
  Serial.println("Motor stopped");
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  delay(300);

  Serial.println("Moving Backwards");
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW); 
  delay(500);

  // Stop the DC motor
  Serial.println("Motor stopped");
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  delay(300);
}

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.printf("Board ID %u: %u bytes\n", myData.id, len);
  // Update the structures with the new incoming data
  boardsStruct[myData.id-1].data = myData.data;
  boardsStruct[myData.id-1].data = myData.data;

  //Serial.printf("Data: %5.2f \n", boardsStruct[myData.id-1].data);
  Serial.printf("Data: %d \n", boardsStruct[myData.id-1].data);
  Serial.println();
  // Serial.printf("Note: 1) %d, 2) %d\n", boardBool[0],  boardBool[1]);
  if(boardsStruct[myData.id-1].data != 0) // encoutering overflow somewhere so instead of == 1 we do != 0
  {
    driveMotor();
  }

}

void setup() {

  // sets the pins as outputs:
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);
  
  // configure LED PWM functionalitites
  ledcSetup(pwmChannel, freq, resolution);
  ledcWrite(pwmChannel, dutyCycle);   

  // attach the channel to the GPIO to be controlled
  ledcAttachPin(enable1Pin, pwmChannel);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  
  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);


}

void loop() {

}