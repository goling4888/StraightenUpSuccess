#include <Arduino.h>
#include "deskArm.h"

struct_message myData;
struct_message board1;
struct_message board2;
struct_message boardsStruct[2] = {board1, board2};

int badPostureCounts;
int initTime;

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
  Serial.printf("Data: %5.2f \n", boardsStruct[myData.id-1].data);
  Serial.println();

  badPostureCounts++;
  if(badPostureCounts >= 6)
  {
    // Activate motor/arm stuff here

    // if(goodPostureCounts < 10)
    // {
    //   badPostureCounts = 0;
    //   driveMotor();
    // }
    
  }
}

void setup() {

  // sets the pins as outputs:
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);
  
  // configure LED PWM functionalitites
  ledcSetup(pwmChannel, freq, resolution);
  
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
  // Serial.println("hola");

  // int board1 = boardsStruct[0].data;
  // int board2 = boardsStruct[1].data;

}