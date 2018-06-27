import machine
import time
import servo
import neopixel
import network
import uosc.server

machine.Pin(0, machine.Pin.OUT).value(1)
machine.Pin(2, machine.Pin.OUT).value(1)

leds = neopixel.NeoPixel(machine.Pin(14), 2)

def set_leds(r1, g1, b1):
	leds[0] = (r1, g1, b1)
	leds[1] = (r1, g1, b1) 
	leds.write()
set_leds(0, 0, 0)

print('Initializing PCA9685...')
i2c = machine.I2C(scl=machine.Pin(5), sda=machine.Pin(4))
servos = servo.Servos(i2c)

print('Move !')
for i in range(8):
	servos.position(i, 90)
	time.sleep(0.1)

for i in range(8):
	servos.position(i, 0)
	time.sleep(0.1)

time.sleep(1.0)

print('Start scanning')
ssid = 'LeNet'
pwd = 'connectemoi'
sta_if = network.WLAN(network.STA_IF)
sta_if.active(True)
print('Trying to connect to ' + str(ssid) +' / ' + str(pwd) + ' ...')
sta_if.connect(ssid, pwd)

while not sta_if.isconnected():
	for i in range(30):
		set_leds(3*i, i, 0)
		time.sleep(0.01)
	for i in range(30, 0, -1):
		set_leds(3*i, i, 0)
		time.sleep(0.01)

print('Connected !')
print(sta_if.ifconfig())
# machine.Pin(2, machine.Pin.OUT).value(0)
set_leds(0, 150, 0)

leds.write()

def osc_handler(data, src, dispatch=None, strict=False):
	try:
		head, _ = uosc.server.split_oscstr(data, 0)

		if head.startswith('/'):
			messages = [(-1, uosc.server.parse_message(data, strict))]
		elif head == '#bundle':
			messages = uosc.server.parse_bundle(data, strict)
	except:
		if __debug__:
			print("Could not parse message from %s:%i.", *uosc.server.get_hostport(src))
			print("Data: %r", data)
		return

	try:
		for timetag, (oscaddr, tags, args) in messages:
			print("OSC : %s" % oscaddr )
			if oscaddr == "/servo" and tags == 'ii':
				print(args)
				index, value = args[0], args[1]
				index = min(8, max(0, index))
				value = min(180, max(0, value))
				servos.position(index, value)

			elif oscaddr == "/servos" and tags == 'iiiiiiii':
				for index, value in enumerate(args):
					index = min(8, max(0, index))
					value = min(180, max(0, value))
					servos.position(index, value)

			elif oscaddr == "/leds" and tags == 'iii':
				set_leds(args[0], args[1], args[2])

			if dispatch:
			    dispatch(timetag, (oscaddr, tags, args, src))
	except Exception as exc:
	    print("Exception in OSC handler: %s", exc)

print('Setting up OSC server...')
uosc.server.run_server('0.0.0.0', 12345, handler=osc_handler)

print('Setting up OSC client...')
osc = Client(OSC_SERVER, OSC_PORT)
