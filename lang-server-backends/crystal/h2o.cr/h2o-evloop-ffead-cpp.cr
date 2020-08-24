#
#	Copyright 2009-2020, Sumeet Chhetri
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#
require "reset/prelude"
require "reset/patches"
require "h2o/h2o_evloop"
require "option_parser"
require "./ffead-cpp-lib"

module FfeadCppRespTL
  extend self

  @[ThreadLocal]
  @@fresponse: Void*?

  def set(a)
    @@fresponse = a
  end
  def get
    @@fresponse
  end
end

class H2oFfeadCppCry < H2o
  @ffead_cpp_directory = "/installs/ffead-cpp-5.0"
  @port = 8080
  @config = LibH2o::H2oGlobalconfT.new
  @ctx = LibH2o::H2oContextT.new
  @accept_ctx = LibH2o::H2oAcceptCtxT.new
  @loop : LibH2o::H2oLoopT*

  macro ffeadcpphandler
    Handler.new do |handler, req|
      #puts "#{FfeadCppRespTL.get}"
      if !FfeadCppRespTL.get().is_a?(Nil)
          #puts "cleaning up"
          LibFfeadCpp.ffead_cpp_resp_cleanup(FfeadCppRespTL.get)
      end
      
      generator = uninitialized LibH2o::H2oGeneratorT[2]

      headers = Array(LibFfeadCpp::PhrHeaderFcp).new(100)

      counter = 0
      while counter < req.value.headers.size
        hdr = LibFfeadCpp::PhrHeaderFcp.new
        hdr.name = req.value.headers.entries[counter].name.value.base
        hdr.name_len = req.value.headers.entries[counter].name.value.len
        hdr.value = req.value.headers.entries[counter].val.base
        hdr.value_len = req.value.headers.entries[counter].val.len
        headers << hdr
        #puts "#{String.new(hdr.name, hdr.name_len)} #{String.new(hdr.value, hdr.value_len)}"
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

	  scode = 0
	  smsg_len: UInt64 = 0
	  out_mime_len: UInt64 = 0
	  out_url_len: UInt64 = 0
	  out_headers_len: UInt64 = 0
	  out_body_len: UInt64 = 0

      FfeadCppRespTL.set LibFfeadCpp.ffead_cpp_handle_crystal_picov_1(pointerof(freq), pointerof(scode), out smsg, pointerof(smsg_len), out out_mime, pointerof(out_mime_len),
              out out_url, pointerof(out_url_len), freq.headers, pointerof(out_headers_len), out out_body, pointerof(out_body_len))
      
      #puts "scode=#{scode} smsg_len=#{smsg_len} out_url_len=#{out_url_len} out_mime_len=#{out_mime_len} out_headers_len=#{out_headers_len} out_body_len=#{out_body_len}"
      #if smsg_len > 0
      #  puts "smsg=#{String.new(smsg, smsg_len)}"
      #end
      #if out_mime_len > 0
      #  puts "out_mime=#{String.new(out_mime, out_mime_len)}"
      #end
      #if out_url_len > 0
      #  puts "out_url=#{String.new(out_url, out_url_len)}"
      #end
      #if out_body_len > 0
      #  puts "out_body=#{String.new(out_body, out_body_len)}"
      #end
      
      if scode > 0
        statmsg = String.new(smsg, smsg_len)
        body = LibH2o::H2oIovecT.new(base: out_body, len: out_body_len)
        req.value.res.status = scode
        req.value.res.reason = statmsg
        counter = 0
        while counter < out_headers_len
          hdr = headers[counter]
          #puts "#{String.new(hdr.name, hdr.name_len)} #{String.new(hdr.value, hdr.value_len)}"
          LibH2o.h2o_add_header_by_str(req.offset_at(576).as(LibH2o::H2oMemPoolT*), req.offset_at(360).as(LibH2o::H2oHeadersT*), hdr.name, hdr.name_len, 1, hdr.name, hdr.value, hdr.value_len)
          counter += 1
        end
        h2o_start_response(req, generator)
        h2o_send(req, pointerof(body), 1, LibH2o::H2oSendState::H2OSendStateFinal)
      else
        out_url = String.new(out_url, out_url_len)
        out_mime = LibH2o::H2oIovecT.new(base: out_mime, len: out_mime_len)
        if File.exists?(out_url)
        	req.value.res.reason = "OK";
    		req.value.res.status = 200;
        	LibH2o.h2o_file_send(req, req.value.res.status, req.value.res.reason, out_url, out_mime, 0)
        else
            req.value.res.status = 404
      		req.value.res.reason = "Not Found"
      		h2o_start_response(req, generator)
      		h2o_send(req, NULL, 0, LibH2o::H2oSendState::H2OSendStateFinal)
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
    addr.sin_port = b16(8080)   # b16(7890)

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
  	#OptionParser.parse! do |parser|
	#  parser.banner = ""
	#  parser.on("-f DIR", "--ffead-cpp-dir=DIR", "ffead-cpp directory") { |dir| @ffead_cpp_directory = dir }
	#  parser.on("-t PORT", "--to=PORT", "Specifies the port") { |port| @port = port.to_i }
	#end
	
    LibFfeadCpp.ffead_cpp_bootstrap(@ffead_cpp_directory, @ffead_cpp_directory.bytesize , 13)
    LibFfeadCpp.ffead_cpp_init()

    h2o_config_init(pointerof(@config))
    @config.server_name = h2o_iovec_init("h2o-cry")

    hostconf = h2o_config_register_host(pointerof(@config), h2o_iovec_init("default"), 65535)
    register_handler(hostconf, "/", ffeadcpphandler)

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

H2oFfeadCppCry.run
