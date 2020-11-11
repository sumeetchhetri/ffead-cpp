using HTTP
using Dates

struct phr_header_fcp
	name::Ptr{UInt8}
	name_len::Csize_t
	value::Ptr{UInt8}
	value_len::Csize_t
end

struct ffead_request
	server_str::Ptr{UInt8}
	server_str_len::Csize_t
	method::Ptr{UInt8}
	method_len::Csize_t
	path::Ptr{UInt8}
	path_len::Csize_t
	headers::Ptr{Cvoid}
	headers_len::Csize_t
	body::Ptr{UInt8}
	body_len::Csize_t
	version::Cint
end

println("Bootstrapping ffead-cpp start...")
ccall((:ffead_cpp_bootstrap, "libffead-framework.so"), Cvoid, (Cstring, Csize_t, Cint), ARGS[1], length(ARGS[1]), 10)
println("Bootstrapping ffead-cpp end...")

println("Initializing ffead-cpp start...");
ccall((:ffead_cpp_init, "libffead-framework.so"), Cvoid, ())
println("Initializing ffead-cpp end...");

srv = "Julia-HTTP"

##fresList = Array{Ptr{Cvoid},1}(undef, Threads.nthreads())
##for i in 1:Threads.nthreads()
##	fresList[i] = C_NULL
##end

HTTP.listen("0.0.0.0", 8080, reuseaddr=true) do http
	hdrs = Array{phr_header_fcp,1}(undef, 50)
	c = 1
	for header in http.message.headers
		hdrs[c] = phr_header_fcp(pointer(header[1]), length(header[1]), pointer(header[2]), length(header[2]))
		c = c + 1
	  	#println(header[1])
	    #println(header[2])
	end
	
	body = HTTP.payload(http.message)
	freq = ffead_request(pointer(srv), length(srv), pointer(http.message.method), length(http.message.method), 
							pointer(http.message.target), length(http.message.target), pointer(hdrs), c-1, pointer(body), length(body), 1)
	
	
	scode = [0]
	out_url = Ptr{UInt8}[C_NULL]
	out_url_len = [0]
	out_mime = Ptr{UInt8}[C_NULL]
	out_mime_len = [0]
	out_headers_len = [0]
	out_body = Ptr{UInt8}[C_NULL]
	out_body_len = [0]
	
	##fres = fresList[Threads.threadid()]
	##if fres != C_NULL
	##	println(Threads.threadid())
	##	println(fres)
	##	ccall((:ffead_cpp_resp_cleanup, "libffead-framework.so"), Cvoid, (Ptr{Cvoid},), fres)
	##end
						
	##fresList[Threads.threadid()] =
	fres = ccall((:ffead_cpp_handle_rust_swift_1, "libffead-framework.so"), Ptr{Cvoid},
					(Ref{ffead_request}, Ptr{Cvoid}, Ptr{Ptr{UInt8}}, Ptr{Cvoid}, Ptr{Ptr{UInt8}}, Ptr{Cvoid}, Ptr{phr_header_fcp}, Ptr{Cvoid},
						Ptr{Ptr{UInt8}}, Ptr{Cvoid}), freq, scode, pointer(out_url), out_url_len,
						pointer(out_mime), out_mime_len, hdrs, out_headers_len, pointer(out_body), out_body_len)
	
	#=
	@show scode
	if out_url_len[1] > 0
		@show out_url[1]
	end
	if out_mime_len[1] > 0
		@show out_mime[1]
	end
	if out_body_len[1] > 0
		@show out_body[1]
	end
	@show out_headers_len
	=#
	
	HTTP.setheader(http, "Server" => "Julia-HTTP")
   	HTTP.setheader(http, "Date" => Dates.format(Dates.now(), Dates.RFC1123Format) * " GMT")
   	
	if scode[1] > 0
		HTTP.setstatus(http, scode[1])
		for i in 1:out_headers_len[1]
			#println(unsafe_string(hdrs[i].name, hdrs[i].name_len))
			HTTP.setheader(http, unsafe_string(hdrs[i].name, hdrs[i].name_len) => unsafe_string(hdrs[i].value, hdrs[i].value_len))
		end
		startwrite(http)
		write(http, unsafe_string(out_body[1], out_body_len[1]))
	else
		s = open(unsafe_string(out_url[1], out_url_len[1])) do file
		    read(file, String)
		end
		if length(s) > 0
			HTTP.setheader(http, "Content-Type" => unsafe_string(out_mime[1], out_mime_len[1]))
			HTTP.setheader(http, "Content-Length" => string(length(s)))
			HTTP.setstatus(http, 200)
			startwrite(http)
			write(http, s)
		else
			HTTP.setstatus(http, 404)
			startwrite(http)
			write(http, "Not Found")
		end
	end
	
    ccall((:ffead_cpp_resp_cleanup, "libffead-framework.so"), Cvoid, (Ptr{Cvoid},), fres)
end

println("Cleaning up ffead-cpp start...")
ccall((:ffead_cpp_cleanup, "libffead-framework.so"), Cvoid, ())
println("Cleaning up ffead-cpp end...")
