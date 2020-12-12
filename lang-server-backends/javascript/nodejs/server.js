var http = require('http');
const fs = require('fs');
const buf = fs.readFileSync('./ffead-cpp-glue.wasm');
const ffeadCppLib = await WebAssembly.instantiate(new Uint8Array(buf)).
  then(res => res.instance.exports);

ffeadCppLib.bootstrap("/installs/ffead-cpp-5.0");
ffeadCppLib.init();

var server = http.createServer(function (req, res) {
	console.log(req);
});

server.listen(8080);
console.log('Node.js http server at port 8080 is running..')