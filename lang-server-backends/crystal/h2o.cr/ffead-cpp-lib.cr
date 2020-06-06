@[Link("ffead-framework")]
lib LibFfeadCpp
    struct PhrHeaderFcp
        name: LibC::Char*
        name_len: LibC::SizeT
        value: LibC::Char*
        value_len: LibC::SizeT
    end
    struct FfeadRequest
        server_str: LibC::Char*
        server_str_len: LibC::SizeT
        method: LibC::Char*
        method_len: LibC::SizeT
        path: LibC::Char*
        path_len: LibC::SizeT
        qstr: LibC::Char*
        qstr_len: LibC::SizeT
        headers: PhrHeaderFcp*
        headers_len: LibC::SizeT
        body: LibC::Char*
        body_len: LibC::SizeT 
        version: LibC::Int
    end

    fun ffead_cpp_bootstrap(srv: LibC::Char*, srv_len: LibC::SizeT , stype: LibC::Int)
    fun ffead_cpp_init()
    fun ffead_cpp_cleanup()
    fun ffead_cpp_handle_crystal_picov_1(request: FfeadRequest*, scode: LibC::Int*, smsg: LibC::Char**, smsg_len: LibC::SizeT*,
        out_mime: LibC::Char**, out_mime_len: LibC::SizeT*, out_url: LibC::Char**, out_url_len: LibC::SizeT*,
        out_headers: PhrHeaderFcp*, out_headers_len: LibC::SizeT*,
        out_body: LibC::Char**, out_body_len: LibC::SizeT*
    ) : Void*
    fun ffead_cpp_resp_cleanup(ptr: Void*)
end