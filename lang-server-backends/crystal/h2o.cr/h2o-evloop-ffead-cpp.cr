require "reset/prelude"
require "reset/patches"
require "reset/json"
require "h2o/h2o_evloop"
require "option_parser"
require "./ffead-cpp-lib"

class H2oFfeadCppCry < H2o
  @[ThreadLocal]
  @fresponse: Void*?
  @ffead_cpp_directory = "/root/ffead-cpp-4.0"
  @port = 8080
  @config = LibH2o::H2oGlobalconfT.new
  @ctx = LibH2o::H2oContextT.new
  @accept_ctx = LibH2o::H2oAcceptCtxT.new
  @loop : LibH2o::H2oLoopT*

  macro ffeadcpphandler
    Handler.new do |handler, req|
      #puts "#{@fresponse}"
      if !@fresponse.nil?
          #puts "cleaning up"
          LibFfeadCpp.ffead_cpp_resp_cleanup(@fresponse)
      end
      
      generator = uninitialized LibH2o::H2oGeneratorT[2]

      headers = Array(LibFfeadCpp::PhrHeaderFcp).new(100)

      counter = 0
      while counter < req.value.headers.size
        hdr = LibFfeadCpp::PhrHeaderFcp.new
        hdr.name = req.value.headers.entries[counter].name.base
        hdr.name_len = req.value.headers.entries[counter].name.len
        hdr.value = req.value.headers.entries[counter].value.base
        hdr.value_len = req.value.headers.entries[counter].value.len
        headers << hdr
        #puts "#{hdr.name_len} #{hdr.value_len]}"
        counter += 1
      end

      freq = LibFfeadCpp::FfeadRequest.new
      freq.server_str = "h2o-cry"
      freq.server_str_len = 7
      freq.method = req.value.method.base
      freq.method_len = req.value.method.len
      freq.path = req.value.path.base
      freq.path_len = req.value.path.len
      freq.body = req.value.entity.base
      freq.body_len = req.value.entity.len
      freq.version = req.value.version
      freq.headers = headers.to_unsafe()
      freq.headers_len = counter

      scode = 0;

      @fresponse = LibFfeadCpp.ffead_cpp_handle_crystal_1(pointerof(freq), pointerof(scode), out smsg, out smsg_len, out out_mime, out out_mime_len,
              out out_url, out out_url_len, freq.headers, out out_headers_len, out out_body, out out_body_len)

      #puts "#{scode} #{smsg_len} #{out_url_len} #{out_mime_len} #{out_headers_len} #{out_body_len}"
      if scode > 0
        statmsg = String.new(smsg, smsg_len)
        body = h2o_iovec_init(out_body, out_body_len)
        req.value.res.status = scode
        req.value.res.reason = statmsg
        counter = 0
        while counter < out_headers_len
          hdr = headers[counter]
          k = String.new(hdr.name, hdr.name_len)
          v = String.new(hdr.value, hdr.value_len)
          #puts "#{k} = #{v}"
          h2o_add_header(req, k, v)
          counter += 1
        end
        h2o_start_response(req, generator)
        h2o_send(req, pointerof(body), 1, LibH2o::H2oSendState::H2OSendStateFinal)
      else
        out_url = String.new(out_url, out_url_len)
        out_mime = String.new(out_mime, out_mime_len)
        #puts "#{out_url} #{out_mime}"
        if File.exists?(out_url)
            req.value.res.status = 200
            req.value.res.reason = "OK"
            file = File.open out_url, "rb"
            bytes_to_read = file.read_byte.not_nil!
            data = Bytes.new(bytes_to_read)
            file.read_fully(data)
            h2o_start_response(req, generator)
            body = h2o_iovec_init(data, bytes_to_read)
            h2o_send(req, pointerof(body), 1, LibH2o::H2oSendState::H2OSendStateFinal)
        else
            context.response.status_code = 404
        end
      end
      0
    end
  end

  def on_accept(listener : LibH2o::H2oSocketT*, err : LibC::Char*) : Void
    return if err
    return unless s = h2o_evloop_socket_accept(listener)
    h2o_accept(pointerof(@accept_ctx), s)
  end

  def create_listener : Int32
    addr = uninitialized LibC::SockaddrIn

    pointerof(addr).clear
    addr.sin_family = LibC::AF_INET
    addr.sin_addr.s_addr = 0 # 0x100007f # b32(0x7f000001)
    addr.sin_port = @port   # b16(7890)

    option = 1
    if (fd = socket(LibC::AF_INET, LibC::SOCK_STREAM | LibC::O_NONBLOCK | LibC::O_CLOEXEC, 0)) == -1 ||
       setsockopt(fd, LibC::SOL_SOCKET, LibC::SO_REUSEADDR, pointerof(option), 4) != 0 ||
       setsockopt(fd, LibC::SOL_SOCKET, LibC::SO_REUSEPORT, pointerof(option), 4) != 0 ||
       setsockopt(fd, LibC::IPPROTO_TCP, LibC::TCP_QUICKACK, pointerof(option), 4) != 0 ||
       ((option = H2O_DEFAULT_HANDSHAKE_TIMEOUT_IN_SECS) &&
       setsockopt(fd, LibC::IPPROTO_TCP, LibC::TCP_DEFER_ACCEPT, pointerof(option), 4) != 0) ||
       ((option = DEFAULT_TCP_FASTOPEN_QUEUE_LEN) &&
       setsockopt(fd, LibC::IPPROTO_TCP, LibC::TCP_FASTOPEN, pointerof(option), 4) != 0) ||
       bind(fd, pointerof(addr).as(LibC::Sockaddr*), sizeof(LibC::SockaddrIn)) != 0 ||
       listen(fd, LibC::SOMAXCONN) != 0
      return -1
    end

    sock = h2o_evloop_socket_create(@ctx.loop, fd, H2O_SOCKET_FLAG_DONT_READ)
    h2o_socket_read_start(sock, LibH2o::H2oSocketCb.new { |listener, err|
      {{@type}}.instance.on_accept(listener, err)
    })
    0
  end

  def register_handler(hostconf : LibH2o::H2oHostconfT*, path : String, on_req : Handler) : Void
    pathconf = h2o_config_register_path(hostconf, path, 0)
    handler = h2o_create_handler(pathconf, sizeof(LibH2o::H2oHandlerT))
    handler.value.on_req = on_req
  end

  def initialize
    @loop = h2o_evloop_create()
  end

  def run : Void
    OptionParser.parse! do |parser|
      parser.banner = ""
      parser.on("-f DIR", "--ffead-cpp-dir=DIR", "ffead-cpp directory") { |dir| @ffead_cpp_directory = dir }
      parser.on("-t PORT", "--to=PORT", "Specifies the port") { |port| @port = port.to_i }
    end

    LibFfeadCpp.ffead_cpp_bootstrap(@ffead_cpp_directory, @ffead_cpp_directory.bytesize , 13)
    LibFfeadCpp.ffead_cpp_init()

    h2o_config_init(pointerof(@config))
    @config.server_name = h2o_iovec_init("h2o-cry")

    hostconf = h2o_config_register_host(pointerof(@config), h2o_iovec_init("default"), 65535)
    register_handler(hostconf, "/*", ffeadcpphandler)

    h2o_context_init(pointerof(@ctx), @loop, pointerof(@config))

    @accept_ctx.ctx = pointerof(@ctx)
    @accept_ctx.hosts = @config.hosts

    if create_listener != 0
      return 1
    end

    while h2o_evloop_run(@ctx.loop, Int32::MAX) == 0; end

    LibFfeadCpp.ffead_cpp_cleanup()
  end
end

H2oFfeadCppCry.new.run
