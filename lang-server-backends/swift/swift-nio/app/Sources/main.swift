import Foundation
import NIO
import NIOHTTP1
import FfeadCpp

enum Constants {
    static let httpVersion = HTTPVersion(major: 1, minor: 1)
    static let serverName = "SwiftNIO"
}

private final class HTTPHandler: ChannelInboundHandler {
    public typealias InboundIn = HTTPServerRequestPart
    public typealias OutboundOut = HTTPServerResponsePart
    
    let dateCache: RFC1123DateCache
    var headers = [phr_header_fcp]()
    var hrequest: HTTPRequestHead?
    var hbody: [UInt8]?
    var body_len: size_t

    init(channel: Channel) {
        self.dateCache = .on(channel.eventLoop)
        self.body_len = 0
        self.hbody = [UInt8](repeating: 0x01, count: 1)
    }

    func channelRead(context: ChannelHandlerContext, data: NIOAny) {
        switch self.unwrapInboundIn(data) {
        case .head(let request):
            hrequest = request
        case .body(var b):
            hbody = b.readBytes(length: b.readableBytes)!
            body_len = hbody!.count
            break
        case .end:
            var scode: Int32 = 1
            var out_url: UnsafePointer<CChar>?
            var out_url_len = 0
            var out_mime: UnsafePointer<CChar>?
            var out_mime_len = 0
            var out_headers_len = 0
            var out_body: UnsafePointer<CChar>?
            var out_body_len = 0
            
            var freq = ffead_request();
            Constants.serverName.withCString { srvptr in
                hrequest!.method.rawValue.withCString { methptr in
                    hrequest!.uri.withCString { uriptr in
                        hbody?.withUnsafeBytes { bodyptr in
                            for (k, v) in hrequest!.headers {
                                var h = phr_header_fcp()
                                h.name = strdup(k)
                                h.name_len = k.count
                                h.value = strdup(v)
                                h.value_len = v.count
                                headers.append(h)
                            }
                            headers.withUnsafeMutableBytes { hdrptr in
                                freq.headers = hdrptr.bindMemory(to: phr_header_fcp.self).baseAddress
                                freq.headers_len = hrequest!.headers.count
                                
                                if body_len > 0 {
                                    freq.body = bodyptr.bindMemory(to: Int8.self).baseAddress
                                    freq.body_len = body_len
                                }
                                
                                freq.server_str = srvptr
                                freq.server_str_len = Constants.serverName.count
                                
                                freq.method = methptr
                                freq.method_len = hrequest!.method.rawValue.count
                                
                                freq.path = uriptr
                                freq.path_len = hrequest!.uri.count
                                
                                var out_headers = [phr_header_fcp]()
                                for _ in 0..<50 {
                                    out_headers.append(phr_header_fcp())
                                }
                                
                                let fres = ffead_cpp_handle_rust_swift_1(&freq, &scode, &out_url, &out_url_len, &out_mime, &out_mime_len,
                                    &out_headers, &out_headers_len, &out_body, &out_body_len)
                                
                                if scode > 0 {
                                    var rheaders = HTTPHeaders()
                                    for i in 0..<out_headers_len {
                                        let n = String.init(data: Data.init(bytes: out_headers[i].name, count: out_headers[i].name_len), encoding: String.Encoding.utf8)
                                        let v = String.init(data: Data.init(bytes: out_headers[i].value, count: out_headers[i].value_len), encoding: String.Encoding.utf8);
                                        rheaders.add(name: n!, value: v!)
                                    }
                                    
                                    let responseHead = self.responseHead(headers: rheaders, scode: scode)
                                    let body = String.init(data: Data.init(bytes: out_body!, count: out_body_len), encoding: String.Encoding.utf8)
                                    let allocator = ByteBufferAllocator()
                                    var buff = allocator.buffer(capacity: out_body_len)
                                    buff.writeString(body!)
                                    context.write(self.wrapOutboundOut(.head(responseHead)), promise: nil)
                                    context.write(wrapOutboundOut(.body(.byteBuffer(buff))), promise: nil)
                                    ffead_cpp_resp_cleanup(fres)
                                } else {
                                    let file = String.init(data: Data.init(bytes: out_url!, count: out_url_len), encoding: String.Encoding.utf8);
                                    let path = URL(fileURLWithPath: file!)
                                    do {
                                        let text = try String(contentsOf: path)
                                        let ctype = String.init(data: Data.init(bytes: out_mime!, count: out_mime_len), encoding: String.Encoding.utf8);
                                        let clen = String(text.count)
                                        let responseHead = self.responseHead(contentType: ctype!, contentLength: clen)
                                        let allocator = ByteBufferAllocator()
                                        var buff = allocator.buffer(capacity: text.count)
                                        buff.writeString(text)
                                        context.write(self.wrapOutboundOut(.head(responseHead)), promise: nil)
                                        context.write(wrapOutboundOut(.body(.byteBuffer(buff))), promise: nil)
                                    } catch _ {
                                        let responseHead = self.response404()
                                        context.write(wrapOutboundOut(.head(responseHead)), promise: nil)
                                    }
                                    ffead_cpp_resp_cleanup(fres)
                                }
                                context.write(self.wrapOutboundOut(.end(nil)), promise: nil)
                            }
                            
                            for (h) in headers {
                                free(h.name)
                                free(h.value)
                            }
                        }
                    }
                }
            }
        }
    }

    func channelReadComplete(context: ChannelHandlerContext) {
        context.flush()
        context.fireChannelReadComplete()
    }

    private func response404() -> HTTPResponseHead {
        var headers = HTTPHeaders()
        headers.add(name: "server", value: Constants.serverName)
        headers.add(name: "date", value: self.dateCache.currentTimestamp())
        return HTTPResponseHead(
            version: Constants.httpVersion,
            status: .notFound,
            headers: headers
        )
    }

    private func responseHead(contentType: String, contentLength: String) -> HTTPResponseHead {
        var headers = HTTPHeaders()
        headers.add(name: "content-type", value: contentType)
        headers.add(name: "content-length", value: contentLength)
        headers.add(name: "server", value: Constants.serverName)
        headers.add(name: "date", value: self.dateCache.currentTimestamp())
        return HTTPResponseHead(
            version: Constants.httpVersion,
            status: .ok,
            headers: headers
        )
    }

    private func responseHead(headers: HTTPHeaders, scode: Int32) -> HTTPResponseHead {
        var headers = HTTPHeaders()
        headers.add(name: "server", value: Constants.serverName)
        headers.add(name: "date", value: self.dateCache.currentTimestamp())
        return HTTPResponseHead(
            version: Constants.httpVersion,
            status: HTTPResponseStatus(statusCode: Int(scode)),
            headers: headers
        )
    }
}

let group = MultiThreadedEventLoopGroup(numberOfThreads: System.coreCount)
let bootstrap = ServerBootstrap(group: group)
    .serverChannelOption(ChannelOptions.backlog, value: 8192)
    .serverChannelOption(ChannelOptions.socket(SocketOptionLevel(SOL_SOCKET), SO_REUSEADDR), value: 1)
    .childChannelInitializer { channel in
        channel.pipeline.configureHTTPServerPipeline(withPipeliningAssistance: false).flatMap {
            channel.pipeline.addHandler(HTTPHandler(channel: channel))
        }
    }
    .childChannelOption(ChannelOptions.socket(SocketOptionLevel(SOL_SOCKET), SO_REUSEADDR), value: 1)
    .childChannelOption(ChannelOptions.maxMessagesPerRead, value: 16)

defer {
    try! group.syncShutdownGracefully()
}

var serverPath = "/root/ffead-cpp-5.0"
var ipAddress = "0.0.0.0"
var port = 8080

var c = 0
for argument in CommandLine.arguments {
    switch(c) {
    case 1:
        serverPath = argument
    case 2:
        ipAddress = argument
    case 3:
        port = Int(argument) ?? port
    default:
        break;
    }
    c = c + 1
}

ffead_cpp_bootstrap(serverPath, serverPath.count, 13)
ffead_cpp_init()

let channel = try bootstrap.bind(host: ipAddress, port: port).wait()

guard let localAddress = channel.localAddress else {
    fatalError("Address was unable to bind. Please check that the socket was not closed or that the address family was understood.")
}

try channel.closeFuture.wait()

ffead_cpp_cleanup()
