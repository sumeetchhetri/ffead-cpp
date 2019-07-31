/*
 * SocketInterface.h
 *
 *  Created on: 02-Jan-2015
 *      Author: sumeetc
 */

#ifndef SOCKETINTERFACE_H_
#define SOCKETINTERFACE_H_
#include "SocketUtil.h"
#include "Timer.h"
#include "Mutex.h"
#include <sys/stat.h>
#include <fcntl.h>
#if defined(OS_DARWIN) || defined(OS_BSD)
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#elif defined(IS_SENDFILE)
#include <sys/sendfile.h>
#endif
#include "vector"

class SocketInterface;

class ResponseTask {
	void* request;
	void* context;
	void* response;
	int reqPos;
	ResponseTask* next;
	friend class SocketInterface;
	ResponseTask() {
		this->request = NULL;
		this->response = NULL;
		this->context = NULL;
		this->reqPos = -1;
		this->next = NULL;
	}
public:
	ResponseTask(void* request, void* response, void* context, int reqPos) {
		this->request = request;
		this->response = response;
		this->context = context;
		this->reqPos = reqPos;
		this->next = NULL;
	}
};

class SocketInterface {
	friend class RequestReaderHandler;
	friend class ServiceHandler;
	friend class HandlerRequest;
	friend class HttpServiceTask;
	friend class HttpReadTask;
	friend class SelEpolKqEvPrt;
	Mutex m;
protected:
	int fd;
	SocketUtil sockUtil;
	std::string buffer;
	std::atomic<long> t1;
	std::vector<ResponseTask> wtl;
	std::atomic<int> reqPos;
	std::atomic<int> current;
	void pushResponse(void* request, void* response, void* context, int reqPos) {
		if(isCurrentRequest(reqPos)) {
			endRequest();
			writeResponse(request, response, context);
		} else {
			std::vector<ResponseTask>::iterator it;
			m.lock();
			wtl.push_back(ResponseTask(request, response, context, reqPos));
			it = wtl.begin();
			while (it!=wtl.end()) {
				ResponseTask& t = *it;
				if(isCurrentRequest(t.reqPos)) {
					endRequest();
					writeResponse(t.request, t.response, t.context);
					it = wtl.erase(it);
				} else {
					++it;
				}
			}
			m.unlock();
		}
	}
	void init(const SOCKET& fd) {
		reqPos = 0;
		current = 0;
		address = StringUtil::toHEX((long long)this);
		this->fd = fd;
		//sockUtil->init(fd);
	}
	int startRequest() {
		return ++reqPos;
	}
	int endRequest() {
		return ++current;
	}
	bool allRequestsDone() {
		return current == reqPos;
	}
	bool isCurrentRequest(int reqp) {
		return reqp == (current + 1);
	}
	bool writeTo(const std::string& data)
	{
		int offset = 0;
		while(!isClosed() && offset<(int)data.length())
		{
			int count = sockUtil.writeData(data, true, offset);
			if(count>0)
			{
				offset += count;
			}
			else if (count == -1 && errno == EAGAIN)
			{
				/* If errno == EAGAIN, that means we have read all
				 data. So go back to the main loop. */
				break;
			}
			else
			{
				close();
				break;
			}
		}
		return isClosed();
	}
public:
	bool writeFile(int fdes, int remain_data)
	{
		off_t offset = 0;
#if defined(OS_DARWIN)
		off_t sent_bytes1 = BUFSIZ;
		while ((sendfile(fdes, fd, offset, &sent_bytes1, NULL, 0) == 0) && sent_bytes1>0 && (remain_data > 0))
		{
				remain_data -= sent_bytes1;
		}
#elif defined(OS_BSD)
		off_t sent_bytes1 = BUFSIZ;
		while ((sendfile(fdes, fd, offset, BUFSIZ, 0, &sent_bytes1, 0) == 0) && sent_bytes1>0 && (remain_data > 0))
		{
				remain_data -= sent_bytes1;
		}
#elif defined(IS_SENDFILE)
		int sent_bytes = 0;
		while (((sent_bytes = sendfile(fd, fdes, &offset, BUFSIZ)) > 0) && (remain_data > 0))
		{
			remain_data -= sent_bytes;
		}
#else
		while (remain_data > 0) {
			char buffer[1024];
		    int bytes_read = read(fdes, buffer, sizeof(buffer));
		    if (bytes_read == 0)
		        break;

		    if (bytes_read < 0) {
		        return false;
		    }

		    void *p = buffer;
		    while (bytes_read > 0) {
		        int bytes_written = write(fd, p, bytes_read);
		        if (bytes_read == 0) {
		        	close();
		        	return false;
		        }
		        if (bytes_written < 0) {
		        	return false;
		        }
		        remain_data -= bytes_written;
		        bytes_read -= bytes_written;
		        p += bytes_written;
		    }
		}
#endif
		return isClosed();
	}
	bool readFrom()
	{
		while (!isClosed())
		{
			ssize_t count;
			std::string temp;
			count = sockUtil.readData(MAXBUFLENM, temp);
			if(count>0)
			{
				buffer.append(temp);
			}
			else if (count == -1 && errno == EAGAIN)
			{
				/* If errno == EAGAIN, that means we have read all
				 data. So go back to the main loop. */
				break;
			}
			else
			{
				close();
				break;
			}
		}
		return isClosed();
	}
	void close() {
		sockUtil.closeSocket();
	}
	int getDescriptor() {
		return fd;
	}
	std::string getAddress() {
		return address;
	}
	std::string address;
	virtual std::string getProtocol(void* context)=0;
	virtual int getTimeout()=0;
	virtual void* readRequest(void*& context, int& pending, int& reqPos)=0;
	virtual bool writeResponse(void* req, void* res, void* context)=0;
	virtual void onOpen()=0;
	virtual void onClose()=0;
	virtual void addHandler(SocketInterface* handler)=0;
	virtual ~SocketInterface() {
		sockUtil.closeSocket();
	}
	bool isClosed() {
		return sockUtil.closed;
	}
};

#endif /* SOCKETINTERFACE_H_ */
