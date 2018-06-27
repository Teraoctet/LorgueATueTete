import socket

def handle_osc(data, src, dispatch=None, strict=False):
	print('osc')
	print(data)
	print(src)

def run_server(saddr, port, handler=handle_osc):
	MAX_DGRAM_SIZE = 1472
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	sock.bind((saddr, port))
	print("Listening for OSC messages")

	try:
		while True:
			data, caddr = sock.recvfrom(MAX_DGRAM_SIZE)
			handler(data, caddr)
	finally:
		sock.close()
		print("Bye!")