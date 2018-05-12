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
#if defined(IS_SENDFILE)
#include <sys/sendfile.h>
#endif

class SocketInterface {
	friend class RequestReaderHandler;
	friend class ServiceHandler;
	friend class HandlerRequest;
	friend class HttpWriteTask;
protected:
	SocketUtil sockUtil;
	std::string buffer;
	std::atomic<int> t;
	int fd;
	std::atomic<int> reqPos;
	std::atomic<int> current;
	void init(const SOCKET& fd) {
		reqPos = 0;
		current = 0;
		address = StringUtil::toHEX((long long)this);
		this->fd = fd;
		sockUtil.init(fd);
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
			//cout << "writing data " << fd << " " << identifier << std::endl;
			int count = sockUtil.writeData(data, true, offset);
			//cout << "done writing data " << fd << " " << identifier << std::endl;
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
		int sent_bytes = 0;
		/* Sending file data */
#if defined(IS_SENDFILE)
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
				int tt = Timer::getTimestamp() - 1203700;
				t = tt;
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
	long identifier;
	std::string address;
	virtual std::string getProtocol(void* context)=0;
	virtual int getTimeout()=0;
	virtual void* readRequest(void*& context, int& pending, int& reqPos)=0;
	virtual bool writeResponse(void* req, void* res, void* context)=0;
	virtual void onOpen()=0;
	virtual void onClose()=0;
	virtual ~SocketInterface() {}
	bool isClosed() {
		return sockUtil.closed;
	}
	void setIdentifier(const long& identifier) {
		this->identifier = identifier;
	}
};

#endif /* SOCKETINTERFACE_H_ */
