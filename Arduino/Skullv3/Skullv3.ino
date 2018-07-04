#include <Servo.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUDP.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include "WifiUDP.h"
#include <EEPROM.h>

const int SKULL_ID = 1;
const char* SKULL_NAME = "Sissi";
/*
const int SKULL_ID = 2;
const char* SKULL_NAME = "Ninon";

const int SKULL_ID = 3;
const char* SKULL_NAME = "Gordon";

const int SKULL_ID = 4;
const char* SKULL_NAME = "Hubert";
*/
//////////
// WiFi //
//////////
const char* ssid = "LeNet";
const char* password = "connectemoi";
const unsigned int outPort = 12345;
const unsigned int listenPort = 54321;
WiFiUDP Udp;
IPAddress outIp(192, 168, 43, 125); // the real address will be set by the handshake

int pingTimeInMs = 3000;
unsigned long lastPingTime = 0;

///////////
// Servo //
///////////
const int SERVO_PIN = 5;
Servo servo;
const int SERVO_INIT_VALUE = 90; // init servo at mid position
int servoValue = SERVO_INIT_VALUE;

void setup(void)
{
  // set up wifi
  wifi_setup();
    
  // set up servo
  pinMode(SERVO_PIN, OUTPUT);
  servo.attach(SERVO_PIN);
  servo.write(servoValue);
  
  EEPROM.begin(512);
  outIp[3] = EEPROM.read(0);
  wifi_broadcast_ip();
}

void loop(void)
{
  wifi_sendPing();
  wifi_routeMessages();

  // Sanity delay
  delay(5);
}

void sendSimpleOSCMessage(char* address)
{
  OSCMessage msg(address);
  wifi_sendOSCMessage(msg);
}

void setServo(OSCMessage &msg, int addrOffset)
{
  if (msg.isInt(0))
  {
    int val = msg.getInt(0);
    servoValue = min(180, max(0, SERVO_INIT_VALUE + val));
    servo.write(servoValue);
  }
  else sendSimpleOSCMessage("/error");
}

