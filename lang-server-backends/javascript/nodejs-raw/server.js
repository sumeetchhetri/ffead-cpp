var HTTPParser = process.binding('http_parser').HTTPParser;
const TCPWrap = process.binding('tcp_wrap');
const { TCP } = TCPWrap;
const {
  WriteWrap,
  kReadBytesOrError,
  kArrayBufferOffset,
  streamBaseState
} = process.binding('stream_wrap');

var cannedResponse = Buffer.from(
  "HTTP1.1 200 OK\r\n" +
  "Content-Length: 12\r\n" +
  "Connection: Keep-Alive\r\n" +
  "\r\n" +
  "Hello World\n"
);

function noop() {}

var server = new TCP(TCPWrap.constants.SOCKET);
server.bind("0.0.0.0", 8080);
server.listen(200);
server.onconnection = function (err, client) {
  var parser = new HTTPParser(HTTPParser.REQUEST);
  parser.onHeadersComplete = function (info) {
    // console.log("info", info);
  };
  parser.onBody = function (buffer, offset, length) {
    buffer = buffer.slice(offset, length);
    // console.log("body", buffer);
  };
  parser.onMessageComplete = function () {
    var ret = client.writeBuffer(cannedResponse);
    ret.oncomplete = function () {
      // client.close();
    };
  };

  client.onread = function (buffer) {
  	const offset = streamBaseState[kArrayBufferOffset];
    const nread = streamBaseState[kReadBytesOrError];
  	console.log(buffer.toString('utf8'));
  	console.log(offset);
  	console.log(nread);
    if (buffer) {
      parser.execute(buffer, offset, nread);
    }
    else {
      parser.finish();
      client.close();
    }
  };
  client.readStart();
  console.log(client);
};