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
#include <sys/sendfile.h>

class SocketInterface {
	friend class RequestReaderHandler;
	friend class ServiceHandler;
	friend class HandlerRequest;
protected:

	SocketUtil* sockUtil;
	std::string buffer;
	Timer t;
	int fd;
	bool write(const std::string& data)
	{
		int offset = 0;
		while(!isClosed() && offset<(int)data.length())
		{
			//cout << "writing data " << fd << " " << identifier << std::endl;
			int ret = sockUtil->writeData(data, true, offset);
			//cout << "done writing data " << fd << " " << identifier << std::endl;
			if(ret==0)
			{
				break;
			}
			else if(ret>0)
			{
				offset += ret;
			}
		}
		return isClosed();
	}public:
	bool writeFile(int fdes, int remain_data)
	{
		off_t offset = 0;
		int sent_bytes = 0;
		/* Sending file data */
		while (((sent_bytes = sendfile(fd, fdes, &offset, BUFSIZ)) > 0) && (remain_data > 0))
		{
			remain_data -= sent_bytes;
		}
		return isClosed();
	}
	bool read()
	{
		while (!isClosed())
		{
			ssize_t count;
			std::string temp;
			count = sockUtil->readData(MAXBUFLENM, temp);
			if(count>0)
			{
				t.start();
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
		sockUtil->closeSocket();
	}
	int getDescriptor() {
		return fd;
	}
long identifier;
	virtual std::string getProtocol(void* context)=0;
	virtual int getTimeout()=0;
	virtual void* readRequest(void*& context, int& pending)=0;
	virtual bool writeResponse(void* req, void* res, void* context)=0;
	virtual void onOpen()=0;
	virtual void onClose()=0;
	virtual ~SocketInterface() {}
	bool isClosed() {
		sockUtil->lock.lock();
		bool fl = sockUtil->closed;
		sockUtil->lock.unlock();
		return fl;
	}
	void setIdentifier(const long& identifier) {
		this->identifier = identifier;
		fd = sockUtil->fd;
	}
};

#endif /* SOCKETINTERFACE_H_ */
