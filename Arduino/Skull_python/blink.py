import machine
import time

redLED = machine.Pin(0, machine.Pin.OUT)
blueLED = machine.Pin(2, machine.Pin.OUT)
print("start")

#while True:
redLED.value(0)
time.sleep(1.0)
redLED.value(1)
time.sleep(1.0)

redLED.value(0)
time.sleep(1.0)
redLED.value(1)
time.sleep(1.0)

redLED.value(0)
time.sleep(1.0)
redLED.value(1)
time.sleep(1.0)

redLED.value(0)
time.sleep(1.0)
redLED.value(1)
time.sleep(1.0)

print('stop')