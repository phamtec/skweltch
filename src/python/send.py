
import zmq
import msgpack
import sys

if len(sys.argv) < 4:
	print "usage: ", sys.argv[0], " bind|connect port msg"
	exit(1)

context = zmq.Context()

sender = context.socket(zmq.PUSH)
if sys.argv[1] == "bind":
	sender.bind(sys.argv[2])
elif sys.argv[1] == "connect":
	sender.connect(sys.argv[2])
else:
	print "bind or connect"
	exit(1)

packed = msgpack.packb(sys.argv[3:], use_bin_type=True)
sender.send(packed)

sender.close()
context.term()

exit(0)
