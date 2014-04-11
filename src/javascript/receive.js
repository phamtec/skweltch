var zmq = require('/usr/local/lib/node_modules/zmq'), 
	msgpack = require('/usr/local/lib/node_modules/msgpack');

if (process.argv.length < 4) {
	console.log("usage: ", process.argv[0], process.argv[1], " bind|connect port");
	process.exit(1);
}

var receiver = zmq.socket('pull');

if (process.argv[2] == "bind") {
	receiver.bind(process.argv[3]);
}
else if (process.argv[2] == "connect") {
	receiver.connect(process.argv[3]);
}
else {
	console.log("bind or connect");
	process.exit(1);
}

receiver.on('message', function(message) {
	console.log(msgpack.unpack(message));
});
