#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUDP.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include "WifiUDP.h"
#include <EEPROM.h>

//#define SOLIST // COMMENT FOR NON SOLISTS

/////////////////
// ID and NAME //
/////////////////
#ifndef SOLIST
const int SKULL_ID = 2; // SET SKULL ID HERE: 1 to 7
#else
const int SKULL_ID = 0; // do not change
#endif

String SKULL_NAMES[8] = { "Jack", "Sissi", "Ninon", "Hubert", "Jerry", "Nancy", "Franck", "Pat"};
const String SKULL_NAME = SKULL_NAMES[SKULL_ID];

//////////
// WiFi //
//////////
const char* ssid = "LeNet";
const char* password = "connectemoi";
const unsigned int outPort = 12345;
const unsigned int listenPort = 54321;
WiFiUDP Udp;
IPAddress outIp(192, 168, 43, 125); // the last byte will be set by the EEPROM or the handshake

int pingTimeInMs = 3000;
unsigned long lastPingTime = 0;

///////////
// Servo //
///////////
const int SERVO_INIT_VALUE = 90; // init servo at mid position

#ifndef SOLIST
#include <Servo.h>
const int SERVO_PIN = 5;
Servo servo;

#else
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define SERVOMIN  150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  574 // this is the 'maximum' pulse length count (out of 4096)
#include <Adafruit_PWMServoDriver.h>

#endif


/////////////////
// Util functions
/////////////////
void send_simple_message(char* address)
{
  OSCMessage msg(address);
  send_message(msg);
}

void send_message(OSCMessage &msg)
{
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();
}

////////
// SETUP
////////  
void setup(void)
{
  // set up wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)delay(5);
  Udp.begin(listenPort);
    
  // set up servo
  #ifndef SOLIST
  pinMode(SERVO_PIN, OUTPUT);
  servo.attach(SERVO_PIN);
  servo.write(SERVO_INIT_VALUE);
  #else
  pwm.begin();
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
  #endif

  // read target ip from EEPROM
  EEPROM.begin(512);
  outIp[3] = EEPROM.read(0);

  // broadcast handshake message
  OSCMessage msg = handshake_message();
  Udp.beginPacket("192.168.43.255", outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();
}

OSCMessage handshake_message()
{
  OSCMessage msg("/handshake");
  msg.add(SKULL_ID);
  msg.add(SKULL_NAME.c_str());
  msg.add(WiFi.localIP().toString().c_str());

  // get and send target IP
  msg.add(outIp.toString().c_str());
  return msg;
}

void loop(void)
{
  // send ping
  if ((unsigned long)(millis() - lastPingTime) > pingTimeInMs)
  {
    OSCMessage pingmsg("/ping");
    pingmsg.add(SKULL_ID);
    pingmsg.add(SKULL_NAME.c_str());
    pingmsg.add(WiFi.localIP().toString().c_str());
    send_message(pingmsg);
    lastPingTime = millis();
  }

  // route messages
  OSCMessage bundleIN;
  int packet_size;
  if ( (packet_size = Udp.parsePacket()) > 0)
  {
    while (packet_size--)
      bundleIN.fill(Udp.read());

    if (!bundleIN.hasError())
    {
      bundleIN.route("/handshake", get_handshake);
      bundleIN.route("/config", send_config);
      bundleIN.route("/servo", set_servo);
    }
  }
  
  // Sanity delay
  delay(5);
}

void get_handshake(OSCMessage &msg, int addrOffset)
{
  // exchange IP addresses
  if (msg.isInt(0) && msg.isInt(1) && msg.isInt(2) && msg.isInt(3))
  { 
    // set remote IP
    outIp[0] = msg.getInt(0);
    outIp[1] = msg.getInt(1);
    outIp[2] = msg.getInt(2);
    outIp[3] = msg.getInt(3);

    // store last byte of ip address
    EEPROM.write(0, (byte)outIp[3]);
    EEPROM.commit();
  }
}

void send_config(OSCMessage &msg, int addrOffset)
{
  OSCMessage m = handshake_message();
  send_message(m);
}


void set_servo(OSCMessage &msg, int addrOffset)
{
  #ifndef SOLIST
  if (msg.isInt(0))
  {
    int servoValue = min(180, max(0, SERVO_INIT_VALUE + msg.getInt(0)));
    servo.write(servoValue);
  }
  else send_simple_message("/error");
  #else
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
    int servoValue = min(180, max(0, SERVO_INIT_VALUE + msg.getInt(0)));
    pwm.setPWM(index, 0, map(servoValue, 0, 180, SERVOMIN, SERVOMAX));
  }
  else sendSimpleOSCMessage("/error");

  #endif
}

