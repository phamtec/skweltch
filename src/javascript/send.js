var zmq = require('/usr/local/lib/node_modules/zmq'), 
	msgpack = require('/usr/local/lib/node_modules/msgpack');

if (process.argv.length < 5) {
	console.log("usage: ", process.argv[0], process.argv[1], " bind|connect port msg");
	process.exit(1);
}

var sender = zmq.socket('push');

if (process.argv[2] == "bind") {
	sender.bind(process.argv[3]);
}
else if (process.argv[2] == "connect") {
	sender.connect(process.argv[3]);
}
else {
	console.log("bind or connect");
	process.exit(1);
}

var packed = msgpack.pack(process.argv.slice(4))
sender.send(packed);

process.exit(0);
