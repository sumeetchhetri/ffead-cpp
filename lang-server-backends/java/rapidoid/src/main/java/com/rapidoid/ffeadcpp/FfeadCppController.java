package com.rapidoid.ffeadcpp;

import static com.ffead.cpp.FfeadCppInterface.ffead_cpp_handle;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.util.Map;

import org.rapidoid.buffer.Buf;
import org.rapidoid.http.AbstractHttpServer;
import org.rapidoid.http.HttpResponseCodes;
import org.rapidoid.http.HttpStatus;
import org.rapidoid.http.impl.lowlevel.HttpIO;
import org.rapidoid.net.abstracts.Channel;
import org.rapidoid.net.impl.RapidoidHelper;

import com.ffead.cpp.FfeadCppInterface.FfeadRequest;
import com.ffead.cpp.FfeadCppInterface.FfeadResponse;

public class FfeadCppController extends AbstractHttpServer {
	static final byte[]                CONN_KEEPALIVE = "Connection: keep-alive\r\n".getBytes();
	
	public FfeadCppController() {
		super("rapidoid", "", "", false);
	}

	@Override
	protected HttpStatus handle(Channel ctx, Buf buf, RapidoidHelper data) {
		FfeadRequest freq = new FfeadRequest();
    	freq.setMethod(data.verb.str(buf));
    	String fullPath = data.path.str(buf);
    	if(!data.query.isEmpty()) {
    		fullPath += "?" + data.query.str(buf);
    	}
    	freq.setPath(fullPath);
    	freq.setServerName("rapidoid");
    	freq.setVersion(1);
    	freq.setBody(data.body.str(buf));
    	
    	Map<String, String> headers = data.headers.toMap(buf.bytes(), 0, data.headers.count-1, ":");
    	for (Map.Entry<String, String> entry: headers.entrySet()) {
			freq.getHeaders().put(entry.getKey().trim(), entry.getValue().trim());
		}
    	
    	//System.out.println(String.format("%s %s %s", freq.getMethod(), freq.getPath(), freq.getBody()));
    	
    	//System.out.println(freq);
    	FfeadResponse fres = ffead_cpp_handle(freq);
    	//System.out.println(fres);
		
		if (fres.getStatusCode()>0) {
			byte[] hdrs = toBytes(fres.getHeaderStr());
    		byte[] body = toBytes(fres.getBody());
			ctx.write(hdrs);
			if (!data.isKeepAlive.value) {
				ctx.write(CONN_CLOSE_HDR);
			} else {
				ctx.write(CONN_KEEPALIVE);
			}
			ctx.write(SERVER_HDR);
			writeDateHeader(ctx);
    		ctx.write(CR_LF);
    		ctx.write(body, 0, body.length);
    		return HttpStatus.DONE;
    	} else {
    		if(fres.getUrl()!=null) {
	    		File out = new File(fres.getUrl());
	    		if(out.exists()) {
	    			byte[] fileContent = null;
					try {
						fileContent = Files.readAllBytes(out.toPath());
					} catch (IOException e) {
						e.printStackTrace();
					}
	    			ctx.write(HttpResponseCodes.get(200));
	    			if (!data.isKeepAlive.value) {
	    				ctx.write(CONN_CLOSE_HDR);
	    			} else {
	    				ctx.write(CONN_KEEPALIVE);
	    			}
	    			ctx.write(SERVER_HDR);
	        		ctx.write(CONTENT_TYPE_TXT);
	        		ctx.write(fres.getMimeType().getBytes());
	        		ctx.write(CR_LF);
	        		HttpIO.INSTANCE.writeContentLengthHeader(ctx, fileContent.length);
	    			writeDateHeader(ctx);
	        		ctx.write(CR_LF);
	        		ctx.write(fileContent, 0, fileContent.length);
	    			return HttpStatus.DONE;
	    		}
    		}
    	}
		return HttpStatus.NOT_FOUND;
	}

	private static byte[] toBytes(String c) {
    	try {
			return c.getBytes("UTF-8");
		} catch (Exception e) {
			return c.getBytes();
		}
    }
}
