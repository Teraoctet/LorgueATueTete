from uosc.client import Bundle, Client, create_message
import network
import time

print('Start scanning')
ssid = 'LeNet'
pwd = 'connectemoi'
sta_if = network.WLAN(network.STA_IF)
sta_if.active(True)
print('Trying to connect to ' + str(ssid) +' / ' + str(pwd) + ' ...')
sta_if.connect(ssid, pwd)

while not sta_if.isconnected():
    time.sleep(0.01)

print('Connected !')
print(sta_if.ifconfig())

osc = Client('192.168.43.227', 12345)
osc.send('/controls/frobnicator', 42, 3.1419, "spamm")
b = Bundle()
b.add(create_message("/foo", 123))
b.add(create_message("/spamm", 12345))
osc.send(b)
print('DONE')