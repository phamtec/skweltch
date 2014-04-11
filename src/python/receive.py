
import zmq
import msgpack
import sys

if len(sys.argv) < 3:
	print "usage: ", sys.argv[0], " bind|connect port"
	exit(1)

context = zmq.Context()

receiver = context.socket(zmq.PULL)
if sys.argv[1] == "bind":
	receiver.bind(sys.argv[2])
elif sys.argv[1] == "connect":
	receiver.connect(sys.argv[2])
else:
	print "bind or connect"
	exit(1)
	
while 1:
	message = receiver.recv()
	print msgpack.unpackb(message, encoding='utf-8')
	
receiver.close()
context.term()

exit(0)
