#include <SPI.h>
#include <RH_RF95.h>

#include "sensors.hpp"
#include "pose.hpp"
#include "motors.hpp"
#include "messages.hpp"

// Options
constexpr bool kWaitForSerial = false;
constexpr bool kIsSim = true;  // is running in simulation environment

// Accelerometer SPI
#define LSM_CS 6 
#define LSM_SCK 15 
#define LSM_MISO 14 
#define LSM_MOSI 16 
rexquad::IMU imureal(LSM_CS);
rexquad::IMUSimulated imusim;

// Motors
#define FRONT_LEFT_PIN 9 
#define FRONT_RIGHT_PIN 10 
#define BACK_RIGHT_PIN 11 
#define BACK_LEFT_PIN 12 
rexquad::QuadMotors motors(FRONT_LEFT_PIN, FRONT_RIGHT_PIN, BACK_RIGHT_PIN, BACK_LEFT_PIN);

// LoRa
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 915.0
#define LED_PIN 13
#define LED 13
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Aliases
using Pose = rexquad::PoseMsg;
using Control = rexquad::ControlMsg;
using StateControl = rexquad::StateControlMsg;

// Constants
constexpr int kPoseSize = sizeof(Pose) + 1;
constexpr uint8_t kPoseID = Pose::MsgID();
constexpr int kControlSize = sizeof(Control) + 1;
constexpr uint8_t kControlID = Control::MsgID;
constexpr int kStateControlSize = sizeof(StateControl) + 1;
constexpr uint8_t kStateControlID = StateControl::MsgID;


// Buffers
uint8_t bufsend[kStateControlSize];  // buffer for sending state estimate and control info back
uint8_t bufrecv[kPoseSize];  // buffer for receiving pose of LoRa
uint8_t bufpose[kPoseSize];

Pose pose_mocap;
StateControl statecontrol;

void setup() {
  // Serial setup
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(256000);
  Serial1.begin(256000);

  if (kWaitForSerial) {
    while (!Serial) {
      digitalWrite(LED_PIN, HIGH);
      delay(100);
      digitalWrite(LED_PIN, LOW);
      delay(100);
    }
  }
  digitalWrite(LED_PIN, HIGH);

  // Accelerometer Setup
  Serial.println("Sensor test!");
  while (!imureal.Connect()) {
    Serial.println("Failed to find LSM6DSO32 chip");
    delay(1000);
  }
  Serial.println("Found LSM6DSO32 chip!");

  imureal.SetAccelRange(rexquad::IMU::AccelRange::Accel8g);
  // imureal.SetGyroRange(rexquad::IMU::GyroRange::Gyro250dps);
  // imureal.SetAccelRate(rexquad::IMU::DataRate::Rate12_5Hz);
  // imureal.SetGyroRate(rexquad::IMU::DataRate::Rate12_5Hz);

  // LoRa setup
  pinMode(LED, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1);
  }

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }

  rf95.setTxPower(23, false);
  // rf95.set_dragons();
  // rf95.setModemConfig(RH_RF95::Bw125Cr45Sf128);
  rf95.setModemConfig(RH_RF95::Bw500Cr45Sf128);
  // rf95.setModemConfig(RH_RF95::Bw500Cr45Sf64);
}

void loop() {
  if (!kIsSim) {
    imureal.ReadSensor();
  }
  // imu->PrintData(Serial);
  // delay(100);

  // Relay packet
  if (rf95.available()) {
    digitalWrite(LED, HIGH);
    uint8_t lenrecv = sizeof(bufrecv);
    uint8_t lensend = sizeof(bufsend);

    if (rf95.recv(bufrecv, &lenrecv)) {
      // Convert bytes into pose message
      rexquad::PoseFromBytes(pose_mocap, (char*)bufrecv);

      // TODO: Update state estimate
      statecontrol.x = pose_mocap.x;
      statecontrol.y = pose_mocap.y;
      statecontrol.z = pose_mocap.z;
      statecontrol.qw = pose_mocap.qw;
      statecontrol.qx = pose_mocap.qx;
      statecontrol.qy = pose_mocap.qy;
      statecontrol.qz = pose_mocap.qz;

      // TODO: Implement control policy
      statecontrol.u[0] = -100;
      statecontrol.u[1] = -100;
      statecontrol.u[2] = -100;
      statecontrol.u[3] = -100;

      if (kIsSim) {
        // In simulation mode, the IMU data gets sent over serial
        //   before the pose information gets sent over LoRa
        // This reads the imu data sent over serial
        Serial.setTimeout(1000);
        imusim.ReadSensor();
        Serial.setTimeout(10);
        const sensors_event_t& accel = imusim.GetAccel();
        const sensors_event_t& gyro = imusim.GetGyro();
        statecontrol.vx = accel.acceleration.x;
        statecontrol.vy = accel.acceleration.y;
        statecontrol.vz = accel.acceleration.z;
        statecontrol.wx = gyro.gyro.x;
        statecontrol.wy = gyro.gyro.y;
        statecontrol.wz = gyro.gyro.z; 

        // Create StateControl message

        // // Copy LoRa pose into out buffer
        // memcpy(bufsend, bufrecv, lenrecv);

        // // TODO: Copy controls
        // bufsend[lenrecv] = kControlID;
        // uint8_t bufctrl[16];
        // rexquad::ControlMsg ctrl = {-100, -100, -100, -100};
        // rexquad::ControlMsgToBytes(ctrl, bufctrl);

        // for (int i = 0; i < 16; ++i) {
        //   bufsend[lenrecv + 1 + i] = bufctrl[i];
        // }
        rexquad::StateControlMsgToBytes(statecontrol, bufsend);

        // Send pose and controls back over serial
        Serial.write(bufsend, lensend);
      }
    }
    digitalWrite(LED, LOW);
  }
}