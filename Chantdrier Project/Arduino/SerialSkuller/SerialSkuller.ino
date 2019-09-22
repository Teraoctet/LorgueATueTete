const String FIRMWARE_VERSION = "v3.0";

//#define MULTI_SERVO // COMMENT FOR CHOIR SKULLS


/////////////////
// ID and NAME //
/////////////////
#ifndef MULTI_SERVO
const int SKULL_ID = 2; // SET SKULL ID HERE: 1 to 7
#else
const int SKULL_ID = 0; // do not change
#endif

///////////
// Servo //
///////////
const int SERVO_INIT_VALUE = 90; // init servo at mid position

#ifndef MULTI_SERVO
#include <Servo.h>
const int SERVO_PIN = 5;
Servo servo;
#else
#include <Adafruit_PWMServoDriver.h>
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define SERVOMIN  150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  574 // this is the 'maximum' pulse length count (out of 4096)
#endif


//////////
// Ping //
//////////
int pingTimeInMs = 1000;
unsigned long lastPingTime = 0;

void setup() {

  // set up servo
#ifndef MULTI_SERVO
  pinMode(SERVO_PIN, OUTPUT);
  servo.attach(SERVO_PIN);
  servo.write(SERVO_INIT_VALUE);
#else
  pwm.begin();
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
#endif

  servo.write(0);
  Serial.begin(115200);
}

void loop() {
  if ((unsigned long)(millis() - lastPingTime) > pingTimeInMs)
  {
    Serial.print("id ");
    Serial.println(SKULL_ID);
    lastPingTime = millis();
  }
  
  while (Serial.available()) {
    byte incoming[3];
    Serial.readBytesUntil(255, incoming, 3);

    if (sizeof(incoming) == 3)
    {
      int servoIndex = incoming[0];
      if (servoIndex > 7) return;
      int servoValue = constrain(SERVO_INIT_VALUE + incoming[1], 0, 180);

#ifndef MULTI_SERVO
      servo.write(servoValue);
#else
      pwm.setPWM(servoIndex, 0, map(servoValue, 0, 180, SERVOMIN, SERVOMAX));

#endif
    }
  }
}
