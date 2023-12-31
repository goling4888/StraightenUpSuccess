#include <M5StickCPlus.h>
#include <utility/MPU6886.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <esp_now.h>


#define PI 3.1415926535897932384626433832795
#define GRAVITY 9.80665
#define BAD_HEAD_ANGLE 30
#define BAD_CERVICAL_ANGLE 45

float accX, accY, accZ;
float convX,convY,convZ;
float rateCalibrationX, rateCalibrationY, rateCalibrationZ;
float angle, badAngle;

uint8_t imuID;
int badPostureCounts;
// Address of desk ESP32
byte broadcastAddress[] = {0xA0,0xA3,0xB3,0x2A,0x75,0xC0};
byte headAddr[] = {0xD4, 0xD4, 0xDA, 0xBC, 0xF6, 0xA0};
byte mac[6];
byte cervicalAddr[] = {0x64, 0xB7, 0x08, 0x80, 0xEA, 0xE4};

typedef struct struct_message {
    uint8_t id; // must be unique for each sender board
    float data;
} struct_message;

struct_message myData;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void calibrateAccel()
{
  //USBSerial.println("Lets calibrate");
  const int numSamples = 2000;
  for(int i = 0; i < numSamples; i++)
  {
    M5.IMU.getAccelData(&accX, &accY, &accZ);

    rateCalibrationX += accX;
    rateCalibrationY += accY;
    rateCalibrationZ += accZ;
    delay(1);
  }

  rateCalibrationX = rateCalibrationX/numSamples;
  rateCalibrationY = rateCalibrationY/numSamples;
  rateCalibrationZ = rateCalibrationZ/numSamples;

}

void getAngle()
{
  M5.IMU.getAccelData(&accX, &accY, &accZ);
  accX -= rateCalibrationX;
  accY -= rateCalibrationY;
  accZ -= rateCalibrationZ - 1; //-1 to account for 1g

  // Convert accelerometer data to m/s^2
  convX = accX*GRAVITY;
  convY = accY*GRAVITY;
  convZ = accZ*GRAVITY;

  // Display Accelerometer data and angle on LCD
  M5.Lcd.setCursor(0, 45);
  M5.Lcd.printf("X:%5.2f\nY:%5.2f\nZ:%5.2f\n", convX, convY, convZ);
  
  float deg = (atan(convZ/convY) * 180)/PI;
  angle = deg;

  M5.Lcd.printf("Angle:%5.2f\n", angle);
  Serial.print(angle);
  Serial.println("");

}

void setup() {
  Serial.begin(115200);

  /*
    Determining code for head and cervical M5 Sticks-C Plus
    Based on mac addr
  */
  WiFi.macAddress(mac);
  if (memcmp(mac, headAddr, 6) == 0)
  {
    badAngle = BAD_HEAD_ANGLE;
    imuID = 0;
  }
  else if (memcmp(mac, cervicalAddr, 6) == 0)
  {
    badAngle = BAD_CERVICAL_ANGLE;
    imuID = 1;
  }

  /*
    Configure ESP-NOW
  */
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }


  /*
    Initialize M5-Stick C Plus
  */
  M5.begin();
  M5.IMU.Init();
  //M5.IMU.SetAccelFsr(M5.IMU.AFS_16G);
  calibrateAccel();
}

void loop() {
  // if(badAngle == BAD_CERVICAL_ANGLE)
  // {
  //   Serial.println("I am cervical");
  // }
  // if(badAngle == BAD_HEAD_ANGLE)
  // {
  //   Serial.println("I am head");
  // }
  getAngle();
  myData.id = imuID;
  myData.data = angle;

  // Send message via ESP-NOW
  if((angle*-1) > badAngle || angle > badAngle)
  {
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
    badPostureCounts++;
  }

  if(angle <= badAngle && badPosture)
  {
    // send info to deal with good posture and desk arm to stop
    badPostureCounts = 0;
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
  }
  delay(500);
}


