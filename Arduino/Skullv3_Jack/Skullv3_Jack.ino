#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUDP.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <Adafruit_NeoPixel.h>
#include "Leds.h"
#include "WifiUDP.h"
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

const int SKULL_ID = 0;
const char* SKULL_NAME = "Jack";

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
const int LEDS_PIN = 14;
#define NUMLEDS      2
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMLEDS, LEDS_PIN, NEO_GRB + NEO_KHZ800);
float ledVal = 0;
float ledInc = .5;
boolean ledActive = false;

////////////
// Servos //
////////////
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define SERVOMIN  150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  574 // this is the 'maximum' pulse length count (out of 4096)
const int SERVO_INIT_VALUE = 90;


void setup(void)
{
  // Init leds
  pixels.begin();
  leds_setAll(0, 0, 0, 0);

  // set up wifi
  wifi_setup();
    
  
  EEPROM.begin(512);
  outIp[3] = EEPROM.read(0);
  wifi_broadcast_ip();
  
  leds_setAll(0, 100, 0, 0);
  
  pwm.begin();
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
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
  int index = -1;
  if (msg.match("/0", addrOffset)) index = 0;
  if (msg.match("/1", addrOffset)) index = 1;
  if (msg.match("/2", addrOffset)) index = 2;
  if (msg.match("/3", addrOffset)) index = 3;
  if (msg.match("/4", addrOffset)) index = 4;
  if (msg.match("/5", addrOffset)) index = 5;
  if (msg.match("/6", addrOffset)) index = 6;
  
  if (index >= 0 && msg.isInt(0))
  {
    int val = msg.getInt(0);
    int servoValue = min(180, max(0, SERVO_INIT_VALUE + val));
    
    OSCMessage answer("/value");
    answer.add(servoValue);
    
    servoValue =  map(servoValue, 0, 180, SERVOMIN, SERVOMAX);
    answer.add(servoValue);
    pwm.setPWM(index, 0, servoValue);

    wifi_sendOSCMessage(answer);
  }
  else sendSimpleOSCMessage("/error");
}

