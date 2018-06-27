#include <Servo.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUDP.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <Adafruit_NeoPixel.h>
#include "Leds.h"
#include "WifiUDP.h"
/*
const int SKULL_ID = 1;
const char* SKULL_NAME = "Sissi";

const int SKULL_ID = 2;
const char* SKULL_NAME = "Ninon";

const int SKULL_ID = 3;
const char* SKULL_NAME = "Jack";

*/
const int SKULL_ID = 4;
const char* SKULL_NAME = "Hubert";

//////////
// WiFi //
//////////
const char* ssid = "LeNet";
const char* password = "connectemoi";
const unsigned int outPort = 12345;
const unsigned int listenPort = 54321;
WiFiUDP Udp;
IPAddress outIp(192, 168, 43, 52); // the real address will be set by the handshake

int pingTimeInMs = 3000;
unsigned long lastPingTime = 0;

///////////
// Leds //
///////////
const int LEDS_PIN = 0;
#define NUMLEDS      2
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMLEDS, LEDS_PIN, NEO_GRB + NEO_KHZ800);
float ledVal = 0;
float ledInc = .5;
boolean ledActive = false;

///////////
// Servo //
///////////
const int SERVO_PIN = 5;
Servo servo;
const int SERVO_INIT_VALUE = 90; // init servo at mid position
int servoValue = SERVO_INIT_VALUE;

void setup(void)
{
  // Init leds
  pixels.begin();
  leds_setAll(0, 0, 0, 0);

  // set up wifi
  wifi_setup();
    
  // set up servo
  pinMode(SERVO_PIN, OUTPUT);
  servo.attach(SERVO_PIN);
  servo.write(servoValue);
  
  leds_setAll(0, 100, 0, 0);
}

void loop(void)
{
  if (ledActive) leds_roar();
  
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

void ledsActive(OSCMessage &msg, int addrOffset)
{
  ledActive = !ledActive;
}

void setLed(OSCMessage &msg, int addrOffset)
{
  if (msg.isInt(0) && msg.isInt(1) && msg.isInt(2))
  {
    int r = msg.getInt(0);
    int g = msg.getInt(1);
    int b = msg.getInt(2);
    leds_setAll(r,g,b,0);
  }
  else sendSimpleOSCMessage("/error");
}

void setServo(OSCMessage &msg, int addrOffset)
{
  if (msg.isInt(0))
  {
    int val = msg.getInt(0);
    servoValue = min(180, max(0, SERVO_INIT_VALUE + val));
    servo.write(servoValue);
    //leds_setAll(val, val, val,0);
  }
  else sendSimpleOSCMessage("/error");
}

