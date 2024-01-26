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

require "http/server"
require "option_parser"
require "./ffead-cpp-lib"

class HttpServerFfeadCppCry
    @[ThreadLocal]
    @fresponse : Pointer(Void)
    @ffead_cpp_directory = "/root/ffead-cpp-7.0"
    @port = 8080
    @server : HTTP::Server

    def initialize
        @fresponse = Pointer(Void).null
        @server = HTTP::Server.new do |context|
            #puts "#{@fresponse}"
            if !@fresponse.null?
                #puts "cleaning up"
                LibFfeadCpp.ffead_cpp_resp_cleanup(@fresponse.as(Pointer(Void)))
            end

            req = context.request
            headers = Array(LibFfeadCpp::PhrHeaderFcp).new(100)

            counter = 0
            req.headers.each do |k, v|
                hdr = LibFfeadCpp::PhrHeaderFcp.new
                hdr.name = k
                hdr.name_len = k.bytesize
                hdr.value = v[0]
                hdr.value_len = v[0].bytesize
                #puts "#{k} #{v[0]}"
                headers << hdr
                counter += 1
            end

            freq = LibFfeadCpp::FfeadRequest.new
            freq.server_str = "crystal-http-server"
            freq.server_str_len = 19
            freq.method = req.method
            freq.method_len = req.method.bytesize
            freq.path = req.path
            freq.path_len = req.path.bytesize
            if !req.query.nil?
                freq.qstr = req.query.not_nil!
                freq.qstr_len = req.query.not_nil!.bytesize
            end
            if !req.body.nil?
                body = req.body.not_nil!.gets_to_end()
                freq.body = body
                freq.body_len = body.bytesize
            end
            freq.version = 1
            freq.headers = headers.to_unsafe()
            freq.headers_len = counter

            scode = 0;

            @fresponse = LibFfeadCpp.ffead_cpp_handle_crystal_js_1(pointerof(freq), pointerof(scode), out smsg, out smsg_len, out out_mime, out out_mime_len,
                    out out_url, out out_url_len, freq.headers, out out_headers_len, out out_body, out out_body_len)

            #puts "#{scode} #{smsg_len} #{out_url_len} #{out_mime_len} #{out_headers_len} #{out_body_len}"
            if scode > 0
                statmsg = String.new(smsg, smsg_len)
                body = String.new(out_body, out_body_len)
                #puts "#{statmsg} #{body}"
                counter = 0
                while counter < out_headers_len
                    hdr = headers[counter]
                    k = String.new(hdr.name, hdr.name_len)
                    v = String.new(hdr.value, hdr.value_len)
                    #puts "#{k} = #{v}"
                    context.response.headers[k] = v
                    counter += 1
                end
                context.response.headers["Server"] = "Crystal"
                context.response.headers["Date"] = HTTP.format_time(Time.utc)
                context.response.status_code = scode
                context.response.print(body)
            else
                out_url = String.new(out_url, out_url_len)
                out_mime = String.new(out_mime, out_mime_len)
                #puts "#{out_url} #{out_mime}"
                if File.exists?(out_url)
                    file = File.open out_url, "rb"
                    data = Bytes.new(file.size.to_u)
                    file.read(data)
                    file.close
                    context.response.headers["Server"] = "Crystal"
                    context.response.headers["Date"] = HTTP.format_time(Time.utc)
                    context.response.content_type = out_mime
                    context.response.write(data)
                else
                    context.response.headers["Server"] = "Crystal"
                    context.response.headers["Date"] = HTTP.format_time(Time.utc)
                    context.response.status_code = 404
                end
            end
        end
    end

    def run
        OptionParser.parse do |parser|
            parser.banner = ""
            parser.on("-f DIR", "--ffead-cpp-dir=DIR", "ffead-cpp directory") { |dir| @ffead_cpp_directory = dir }
            parser.on("-t PORT", "--to=PORT", "Specifies the port") { |port| @port = port.to_i }
        end

        LibFfeadCpp.ffead_cpp_bootstrap(@ffead_cpp_directory, @ffead_cpp_directory.bytesize , 18)
        LibFfeadCpp.ffead_cpp_init()

        address = @server.bind_tcp "0.0.0.0", @port, reuse_port: true
        puts "ffead-cpp directory #{@ffead_cpp_directory}"
        puts "Listening on http://#{address}"
        @server.listen

        LibFfeadCpp.ffead_cpp_cleanup()
    end
end

HttpServerFfeadCppCry.new.run
