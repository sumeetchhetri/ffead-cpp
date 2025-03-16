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
/*
 * MessageHandler.cpp
 *
 *  Created on: Sep 27, 2009
 *      Author: sumeet
 */

#include "MessageHandler.h"


MessageHandler::MessageHandler(const std::string& path)
{
	running = false;
	logger = LoggerFactory::getLogger("MessageHandler");
	this->path = path;
}
MessageHandler* MessageHandler::instance = NULL;

Message MessageHandler::readMessageFromQ(const std::string& fileName, const bool& erase)
{
	std::ifstream file;
	size_t fileSize;
	char *fileContents = NULL, *remcontents;
	file.open(fileName.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
	if (file.is_open())
	{
		fileContents = new char[4];
		file.seekg(0, std::ios::beg);
		if(!file.read(fileContents, 4))
		{
			instance->logger << "Failed to readMessageFromQ" << std::endl;
		}
		int len = (int)AMEFResources::charArrayToLong(fileContents, 4);
		fileContents = new char[len];
		file.seekg(4, std::ios::beg);
		if(!file.read(fileContents, len))
		{
			instance->logger << "Failed to readMessageFromQ" << std::endl;
		}
		if(erase)
		{
			fileSize = (int)file.tellg() - len - 4;
			remcontents = new char[fileSize];
			file.seekg(4+len, std::ios::beg);
			if(!file.read(remcontents, fileSize))
			{
				instance->logger << "Failed to readMessageFromQ" << std::endl;
			}
		}
		file.close();
	}
	Message m;
	if(fileContents==NULL) return m;
	std::string f(fileContents);
	AMEFDecoder dec;
	AMEFObject* obj = dec.decodeB(f, false);
	m.setTimestamp(obj->getPackets().at(0)->getValue());
	m.setType(obj->getPackets().at(1)->getValue());
	m.setPriority(obj->getPackets().at(2)->getValue());
	m.setUserId(obj->getPackets().at(3)->getValue());
	m.setEncoding(obj->getPackets().at(4)->getValue());
	m.setBody(obj->getPackets().at(5)->getValue());
	m.getDestination().setName(obj->getPackets().at(5)->getValue());
	m.getDestination().setType(obj->getPackets().at(5)->getValue());
	delete[] fileContents;
	if(erase)
	{
		std::ofstream myfile;
		myfile.open(fileName.c_str(), std::ios::binary | std::ios::trunc);
		myfile << remcontents;
		myfile.close();
		delete[] remcontents;
	}
	return m;
}

void MessageHandler::writeMessageToQ(const Message& msg, const std::string& fileName)
{
	AMEFEncoder enc;
	AMEFObject ob;
	ob.addPacket(msg.getTimestamp());
	ob.addPacket(msg.getType());
	ob.addPacket(msg.getPriority());
	ob.addPacket(msg.getUserId());
	ob.addPacket(msg.getEncoding());
	ob.addPacket(msg.getBody());
	ob.addPacket(msg.getDestination().getName());
	ob.addPacket(msg.getDestination().getType());
	std::ofstream myfile;
	myfile.open(fileName.c_str(), std::ios::binary | std::ios::app);
	myfile << enc.encodeB(&ob);
	myfile.close();
}

bool MessageHandler::tempUnSubscribe(const std::string& subs, const std::string& fileName)
{
	std::string subscribers;
	std::ifstream myfile1;
	myfile1.open(fileName.c_str());
	if (myfile1.is_open())
	{
		while(getline(myfile1,subscribers))
		{
			//instance->logger << subscribers << std::flush;
			break;
		}
	}
	myfile1.close();
	std::ofstream myfile;
	myfile.open(fileName.c_str());
	std::string rep = subs + ":";
	StringUtil::replaceFirst(subscribers,rep,"");
	myfile.write(subscribers.c_str(),subscribers.length());
	myfile.close();
	if(subscribers.find(":")!=std::string::npos)
		return false;
	else
		return true;
}

Message MessageHandler::readMessageFromT(const std::string& fileName, const std::string& subs)
{
	bool done = tempUnSubscribe(subs,fileName+":SubslistTemp");
	Message msg = readMessageFromQ(fileName, done);
	return msg;
}

void MessageHandler::writeMessageToT(const Message& msg, const std::string& fileName)
{
	writeMessageToQ(msg ,fileName);
}

void MessageHandler::subscribe(const std::string& subs, std::string fileName)
{
	std::ifstream myfile1;
	myfile1.open(fileName.c_str());
	std::string subscribers;
	if (myfile1.is_open())
	{
		while(getline(myfile1,subscribers))
		{
			//instance->logger << subscribers << std::flush;
			break;
		}
	}
	myfile1.close();
	std::ofstream myfile;
	myfile.open(fileName.c_str());
	subscribers += (subs + ":");
	myfile.write(subscribers.c_str(),subscribers.length());
	myfile.close();
	fileName += "Temp";
	myfile.open(fileName.c_str());
	myfile.write(subscribers.c_str(),subscribers.length());
	myfile.close();
}

void MessageHandler::unSubscribe(const std::string& subs, std::string fileName)
{
	std::string subscribers;
	std::ifstream myfile1;
	myfile1.open(fileName.c_str());
	if (myfile1.is_open())
	{
		while(getline(myfile1,subscribers))
		{
			//instance->logger << subscribers << std::flush;
			break;
		}
	}
	myfile1.close();
	std::ofstream myfile;
	myfile.open(fileName.c_str());
	std::string rep = subs + ":";
	StringUtil::replaceFirst(subscribers,rep,"");
	myfile.write(subscribers.c_str(),subscribers.length());
	myfile.close();
	fileName += "Temp";
	myfile.open(fileName.c_str());
	myfile.write(subscribers.c_str(),subscribers.length());
	myfile.close();
}

void* MessageHandler::service(void* arg)
{
	int fd = *(int*)arg;
	char buf[MAXBUFLEN];
	std::string results;
	//int bytes = recv(fd, buf, sizeof buf, 0);
	instance->server.Receive(fd,results,1024);
	//string temp,results;
	/*std::stringstream ss;
	ss << buf;
	while(getline(ss,temp))
	{
		instance->logger << temp << std::flush;
		results.append(temp);
	}*/
	results = results.substr(0,results.find_last_of(">")+1);
	instance->logger << results << std::flush;

	if(results.find("<")!=std::string::npos && results.find(">")!=std::string::npos)
	{
		std::string h = "Received Message----";
		Cont test;
		try
		{
			Message msg(results);
			std::string fileName = instance->path+msg.getDestination().getName()+":"+msg.getDestination().getType();
			if(msg.getDestination().getType()=="Queue")
				instance->writeMessageToQ(msg,fileName);
			else if(msg.getDestination().getType()=="Topic")
				instance->writeMessageToT(msg,fileName);
		}
		catch(const Exception& e)
		{
			instance->logger << e.getMessage() << std::flush;
		}
		instance->server.Send(fd,h);
		//if (send(fd,&h[0] , h.length(), 0) == -1)
		//	instance->logger << "send failed" << std::flush;
		instance->logger << h << std::flush;
	}
	else if(results.find("GET FROM ")!=std::string::npos)
	{
		Message msg;
		if(results.find("Queue")!=std::string::npos)
		{
			StringUtil::replaceFirst(results,"GET FROM ",instance->path);
			msg = instance->readMessageFromQ(results, true);
		}
		else if(results.find("Topic")!=std::string::npos)
		{
			std::string subs = results.substr(results.find("-")+1);
			std::string te = "-" + subs;
			StringUtil::replaceFirst(results,te,"");
			StringUtil::replaceFirst(results,"GET FROM ",instance->path);
			msg = instance->readMessageFromT(results,subs);
		}
		std::string h;
		if(results.find("Queue")!=std::string::npos || results.find("Topic")!=std::string::npos)
		{
			h = msg.toXml();
			instance->logger << h << std::flush;
		}
		else
			h = "Improper Destination";
		instance->server.Send(fd,h);
		//if (send(fd,&h[0] , h.length(), 0) == -1)
		//	instance->logger << "send failed" << std::flush;
	}
	else if(results.find("SUBSCRIBE ")!=std::string::npos)
	{
		int len = results.find("TO") - results.find("SUBSCRIBE ") - 11;
		std::string subs  = results.substr(results.find("SUBSCRIBE ")+10,len);
		results = results.substr(results.find("TO ")+3);
		results = (instance->path+results+":Subslist");
		instance->subscribe(subs,results);
		std::string h = "Subscribed";
		instance->server.Send(fd,h);
		//if (send(fd,&h[0] , h.length(), 0) == -1)
		//	instance->logger << "send failed" << std::flush;
	}
	else if(results.find("UNSUBSCRIBE ")!=std::string::npos)
	{
		int len = results.find("TO") - results.find("UNSUBSCRIBE ") - 12;
		std::string subs  = results.substr(results.find("UNSUBSCRIBE ")+11,len);
		results = results.substr(results.find("TO ")+3);
		results = (instance->path+results+":Subslist");
		instance->subscribe(subs,results);
		std::string h = "Unsubscribed";
		instance->server.Send(fd,h);
		//if (send(fd,&h[0] , h.length(), 0) == -1)
		//	instance->logger << "send failed" << std::flush;
	}
	memset(&buf[0], 0, sizeof(buf));
	close(fd);
	return NULL;
}


void MessageHandler::init(const std::string& path)
{
	if(instance==NULL)
	{
		instance = new MessageHandler(path);
		instance->running = false;
	}
}

void MessageHandler::trigger(const std::string& port, const std::string& path)
{
	init(path);
	if(instance->running)
		return;
	Server serv(port,false,500,&service,2);
	instance->server = serv;
	instance->server.start();
	instance->running = true;
	return;
}

void MessageHandler::stop()
{
	if(instance!=NULL) {
		instance->server.stop();
		delete instance;
	}
}

