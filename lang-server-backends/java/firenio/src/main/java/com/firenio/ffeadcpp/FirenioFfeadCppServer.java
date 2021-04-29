/*
	Copyright 2009-2020, Sumeet Chhetri

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/
package com.firenio.ffeadcpp;

import static com.ffead.cpp.FfeadCppInterface.ffead_cpp_handle;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.util.Map;

import com.ffead.cpp.FfeadCppInterface;
import com.ffead.cpp.FfeadCppInterface.FfeadRequest;
import com.ffead.cpp.FfeadCppInterface.FfeadResponse;
import com.firenio.Options;
import com.firenio.buffer.ByteBuf;
import com.firenio.codec.http11.HttpCodec;
import com.firenio.codec.http11.HttpDateUtil;
import com.firenio.codec.http11.HttpFrame;
import com.firenio.codec.http11.HttpHeader;
import com.firenio.codec.http11.HttpStatus;
import com.firenio.collection.ByteTree;
import com.firenio.collection.IntMap;
import com.firenio.common.Util;
import com.firenio.component.Channel;
import com.firenio.component.ChannelAcceptor;
import com.firenio.component.ChannelEventListenerAdapter;
import com.firenio.component.Frame;
import com.firenio.component.IoEventHandle;
import com.firenio.component.NioEventLoopGroup;
import com.firenio.component.SocketOptions;
import com.firenio.log.DebugUtil;
import com.firenio.log.LoggerFactory;

public class FirenioFfeadCppServer {
	static final byte[]                CONN_CLOSE = "Connection: close".getBytes();
	static final byte[]                CONN_KEEPALIVE = "Connection: keep-alive".getBytes();
	static final byte[]                EOH = "\r\n\r\n".getBytes();
	static final byte[]                CONT_TYPE = "Content-Type: ".getBytes();
	static final byte[]                CONT_LEN = "Content-Length: ".getBytes();
	static final byte[]                EH = "\r\n".getBytes();
	
    public static void main(String[] args) throws Exception {
    	FfeadCppInterface.init(args[0].trim(), 20);
    	
        boolean lite      = Util.getBooleanProperty("lite");
        boolean read      = Util.getBooleanProperty("read");
        boolean pool      = Util.getBooleanProperty("pool");
        boolean epoll     = Util.getBooleanProperty("epoll");
        boolean direct    = Util.getBooleanProperty("direct");
        boolean inline    = Util.getBooleanProperty("inline");
        boolean nodelay   = Util.getBooleanProperty("nodelay");
        boolean cachedurl = Util.getBooleanProperty("cachedurl");
        boolean unsafeBuf = Util.getBooleanProperty("unsafeBuf");
        int     core      = Util.getIntProperty("core", 1);
        int     frame     = Util.getIntProperty("frame", 16);
        int     level     = Util.getIntProperty("level", 1);
        int     readBuf   = Util.getIntProperty("readBuf", 16);
        LoggerFactory.setEnableSLF4JLogger(false);
        LoggerFactory.setLogLevel(LoggerFactory.LEVEL_INFO);
        Options.setBufAutoExpansion(false);
        Options.setChannelReadFirst(read);
        Options.setEnableEpoll(epoll);
        Options.setEnableUnsafeBuf(unsafeBuf);
        DebugUtil.info("lite: {}", lite);
        DebugUtil.info("read: {}", read);
        DebugUtil.info("pool: {}", pool);
        DebugUtil.info("core: {}", core);
        DebugUtil.info("epoll: {}", epoll);
        DebugUtil.info("frame: {}", frame);
        DebugUtil.info("level: {}", level);
        DebugUtil.info("direct: {}", direct);
        DebugUtil.info("inline: {}", inline);
        DebugUtil.info("readBuf: {}", readBuf);
        DebugUtil.info("nodelay: {}", nodelay);
        DebugUtil.info("cachedurl: {}", cachedurl);
        DebugUtil.info("unsafeBuf: {}", unsafeBuf);

        IoEventHandle eventHandle = new IoEventHandle() {

            @Override
            public void accept(Channel ch, Frame frame) throws Exception {
                HttpFrame f = (HttpFrame) frame;
                FfeadRequest freq = new FfeadRequest();
            	freq.setMethod(f.getMethod().getValue());
            	String fullPath = f.getRequestURL();
            	Map<String, String> params = f.getRequestParams();
            	if(params.size()>0) {
            		fullPath += "?";
	            	for (String key : params.keySet()) {
	            		fullPath += key + "=" + params.get(key) + "&";
					}
            	}
            	freq.setPath(fullPath);
            	freq.setServerName("firenio");
            	freq.setVersion(1);
            	if(f.getContent()!=null) {
            		freq.setBody(new String((byte[])f.getContent()));
            	}
            	
            	boolean isKeepAlive = false;
            	IntMap<String> headers = f.getRequestHeaders();
            	try {
            		headers.scan();
            		while(headers.hasNext()) {
                		int keyId = headers.key();
                		HttpHeader key = HttpHeader.get(keyId);
                		String value = headers.value();
                		freq.getHeaders().put(key.getKey(), value);
                		if(key==HttpHeader.Connection && value.toLowerCase().startsWith("keep-alive")) {
                			isKeepAlive = true;
                		}
                	}
				} catch (Exception e) {
				}
            	
            	//System.out.println(String.format("%s %s %s", freq.getMethod(), freq.getPath(), freq.getBody()));
            	//System.out.println(freq);
            	
            	FfeadResponse fres = ffead_cpp_handle(freq);
            	
            	//System.out.println(fres);
            	
            	byte[] date_line = HttpDateUtil.getDateLine();
        		byte[] conn_line = null;
        		if(isKeepAlive) {
        			conn_line = CONN_KEEPALIVE;
        		} else {
        			conn_line = CONN_CLOSE;
        		}
        		
        		boolean done = false;
            	if(fres.getStatusCode()>0) {
            		byte[] hdrs = toBytes(fres.getHeaderStr());
            		byte[] body = toBytes(fres.getBody());
            		int res_len = hdrs.length + date_line.length + conn_line.length + EOH.length + body.length;
            		ByteBuf buf = ch.alloc().allocate(res_len);
            		buf.writeBytes(hdrs);
            		buf.writeBytes(conn_line);
            		buf.writeBytes(date_line);
            		buf.writeBytes(EOH);
            		buf.writeBytes(body);
            		ch.write(buf);
            		ch.flush();
            		done = true;
            	} else {
            		if(fres.getUrl()!=null) {
        	    		File out = new File(fres.getUrl());
        	    		if(out.exists()) {
        	    			byte[] fileContent = null;
        					try {
        						byte[] _200 = HttpStatus.get(200).getLine();
        						fileContent = Files.readAllBytes(out.toPath());
        						byte[] clen = String.valueOf(fileContent.length).getBytes();
        						int res_len = CONT_TYPE.length + fres.getMimeType().getBytes().length + CONT_LEN.length + clen.length + date_line.length + 
        								conn_line.length + EOH.length + fileContent.length + EOH.length + _200.length + EH.length;
        	            		ByteBuf buf = ch.alloc().allocate(res_len);
        	            		buf.writeBytes(_200);
        	            		buf.writeBytes(EH);
        	            		buf.writeBytes(CONT_TYPE);
        	            		buf.writeBytes(fres.getMimeType().getBytes());
        	            		buf.writeBytes(EH);
        	            		buf.writeBytes(CONT_LEN);
        	            		buf.writeBytes(clen);
        	            		buf.writeBytes(EH);
        	            		buf.writeBytes(conn_line);
        	            		buf.writeBytes(date_line);
        	            		buf.writeBytes(EOH);
        	            		buf.writeBytes(fileContent);
        	            		ch.write(buf);
        	            		ch.flush();
        	            		done = true;
        					} catch (IOException e) {
        						e.printStackTrace();
        					}
        	    		} else {
        	    		}
            		}
            	}
            	
            	if(!done) {
            		byte[] _404 = HttpStatus.get(404).getLine();
            		int res_len = _404.length + date_line.length + conn_line.length + EOH.length + EH.length;
            		ByteBuf buf = ch.alloc().allocate(res_len);
            		buf.writeBytes(_404);
            		buf.writeBytes(EH);
            		buf.writeBytes(conn_line);
            		buf.writeBytes(date_line);
            		buf.writeBytes(EOH);
            		ch.write(buf);
            		ch.flush();
            	}
            	
                ch.release(f);
            }
        };

        int fcache    = 1024 * 16;
        int pool_cap  = 1024 * 128;
        int pool_unit = 256;
        if (inline) {
            pool_cap = 1024 * 8;
            pool_unit = 256 * 16;
        }
        HttpDateUtil.start();
        NioEventLoopGroup group   = new NioEventLoopGroup();
        ChannelAcceptor   context = new ChannelAcceptor(group, Integer.parseInt(args[1].trim()));
        group.setMemoryPoolCapacity(pool_cap);
        group.setEnableMemoryPoolDirect(direct);
        group.setEnableMemoryPool(pool);
        group.setMemoryPoolUnit(pool_unit);
        group.setWriteBuffers(32);
        group.setChannelReadBuffer(1024 * readBuf);
        group.setEventLoopSize(Util.availableProcessors() * core);
        group.setConcurrentFrameStack(false);
        if (nodelay) {
            context.addChannelEventListener(new ChannelEventListenerAdapter() {

                @Override
                public void channelOpened(Channel ch) throws Exception {
                    ch.setOption(SocketOptions.TCP_NODELAY, 1);
                    ch.setOption(SocketOptions.SO_KEEPALIVE, 0);
                }
            });
        }
        ByteTree cachedUrls = null;
        if (cachedurl) {
            cachedUrls = new ByteTree();
            cachedUrls.add("/*");
        }
        context.addProtocolCodec(new HttpCodec("firenio", fcache, lite, inline, cachedUrls));
        context.setIoEventHandle(eventHandle);
        context.bind(1024 * 8);
        
        //FfeadCppInterface.cleanUp();
    }
    
    private static byte[] toBytes(String c) {
    	try {
			return c.getBytes("UTF-8");
		} catch (Exception e) {
			return c.getBytes();
		}
    }
}
