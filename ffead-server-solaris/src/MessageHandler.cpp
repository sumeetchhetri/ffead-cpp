/*
	Copyright 2010, Sumeet Chhetri 
  
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

using namespace std;
MessageHandler::MessageHandler(string path)
{
	logger = Logger::getLogger("MessageHandler");
	this->path = path;
}
MessageHandler* _mess_instance = NULL;
Message MessageHandler::readMessageFromQ(string fileName)
{
	MQueue q;
	Message msg;
	ifstream myfile1;
	myfile1.open(fileName.c_str());
	if (myfile1.is_open())
	{
		boost::archive::text_iarchive ia(myfile1);
		ia >> q.msgs;
	}
	msg = q.msgs.front();
	q.msgs.erase(q.msgs.begin());
	ofstream myfile;
	myfile.open(fileName.c_str());
	boost::archive::text_oarchive oa(myfile);
	oa << q.msgs;
	return msg;
}

void MessageHandler::writeMessageToQ(Message msg,string fileName)
{
	MQueue q;
	ifstream myfile1;
	myfile1.open(fileName.c_str());
	if (myfile1.is_open())
	{
		boost::archive::text_iarchive ia(myfile1);
		ia >> q.msgs;
	}
	q.msgs.push_back(msg);
	ofstream myfile;
	myfile.open(fileName.c_str());
	boost::archive::text_oarchive oa(myfile);
	oa << q.msgs;
}

bool MessageHandler::tempUnSubscribe(string subs,string fileName)
{
	string subscribers;
	ifstream myfile1;
	myfile1.open(fileName.c_str());
	if (myfile1.is_open())
	{
		while(getline(myfile1,subscribers))
		{
			_mess_instance->logger << subscribers << flush;
			break;
		}
	}
	myfile1.close();
	ofstream myfile;
	myfile.open(fileName.c_str());
	string rep = subs + ":";
	boost::replace_first(subscribers,rep,"");
	myfile.write(subscribers.c_str(),subscribers.length());
	myfile.close();
	if(subscribers.find(":")!=string::npos)
		return false;
	else
		return true;
}

Message MessageHandler::readMessageFromT(string fileName,string subs)
{
	MQueue t;
	Message msg;
	ifstream myfile1;
	myfile1.open(fileName.c_str());
	if (myfile1.is_open())
	{
		boost::archive::text_iarchive ia(myfile1);
		ia >> t.msgs;
	}
	msg = t.msgs.front();
	bool done = tempUnSubscribe(subs,fileName+":SubslistTemp");
	if(done)
		t.msgs.erase(t.msgs.begin());
	ofstream myfile;
	myfile.open(fileName.c_str());
	boost::archive::text_oarchive oa(myfile);
	oa << t.msgs;
	return msg;
}

void MessageHandler::writeMessageToT(Message msg,string fileName)
{
	MQueue t;
	ifstream myfile1;
	myfile1.open(fileName.c_str());
	if (myfile1.is_open())
	{
		boost::archive::text_iarchive ia(myfile1);
		ia >> t.msgs;
	}
	t.msgs.push_back(msg);
	ofstream myfile;
	myfile.open(fileName.c_str());
	boost::archive::text_oarchive oa(myfile);
	oa << t.msgs;
}

void MessageHandler::subscribe(string subs,string fileName)
{
	ifstream myfile1;
	myfile1.open(fileName.c_str());
	string subscribers;
	if (myfile1.is_open())
	{
		while(getline(myfile1,subscribers))
		{
			_mess_instance->logger << subscribers << flush;
			break;
		}
	}
	myfile1.close();
	ofstream myfile;
	myfile.open(fileName.c_str());
	subscribers += (subs + ":");
	myfile.write(subscribers.c_str(),subscribers.length());
	myfile.close();
	fileName += "Temp";
	myfile.open(fileName.c_str());
	myfile.write(subscribers.c_str(),subscribers.length());
	myfile.close();
}

void MessageHandler::unSubscribe(string subs,string fileName)
{
	string subscribers;
	ifstream myfile1;
	myfile1.open(fileName.c_str());
	if (myfile1.is_open())
	{
		while(getline(myfile1,subscribers))
		{
			_mess_instance->logger << subscribers << flush;
			break;
		}
	}
	myfile1.close();
	ofstream myfile;
	myfile.open(fileName.c_str());
	string rep = subs + ":";
	boost::replace_first(subscribers,rep,"");
	myfile.write(subscribers.c_str(),subscribers.length());
	myfile.close();
	fileName += "Temp";
	myfile.open(fileName.c_str());
	myfile.write(subscribers.c_str(),subscribers.length());
	myfile.close();
}

void MessageHandler::service(int fd)
{
	char buf[MAXBUFLEN];
	string results;
	//int bytes = recv(fd, buf, sizeof buf, 0);
	_mess_instance->getServer()->Receive(fd,results,1024);
	//string temp,results;
	/*stringstream ss;
	ss << buf;
	while(getline(ss,temp))
	{
		_mess_instance->logger << temp << flush;
		results.append(temp);
	}*/
	results = results.substr(0,results.find_last_of(">")+1);
	_mess_instance->logger << results << flush;

	if(results.find("<")!=string::npos && results.find(">")!=string::npos)
	{
		string h = "Received Message----";
		Cont test;
		try
		{
			Message msg(results);
			string fileName = _mess_instance->path+msg.getDestination().getName()+":"+msg.getDestination().getType();
			if(msg.getDestination().getType()=="Queue")
				_mess_instance->writeMessageToQ(msg,fileName);
			else if(msg.getDestination().getType()=="Topic")
				_mess_instance->writeMessageToT(msg,fileName);
		}
		catch(Exception *e)
		{
			_mess_instance->logger << e->what() << flush;
		}
		_mess_instance->getServer()->Send(fd,h);
		//if (send(fd,&h[0] , h.length(), 0) == -1)
		//	_mess_instance->logger << "send failed" << flush;
		_mess_instance->logger << h << flush;
	}
	else if(results.find("GET FROM ")!=string::npos)
	{
		Message msg;
		if(results.find("Queue")!=string::npos)
		{
			boost::replace_first(results,"GET FROM ",_mess_instance->path);
			msg = _mess_instance->readMessageFromQ(results);
		}
		else if(results.find("Topic")!=string::npos)
		{
			string subs = results.substr(results.find("-")+1);
			string te = "-" + subs;
			boost::replace_first(results,te,"");
			boost::replace_first(results,"GET FROM ",_mess_instance->path);
			msg = _mess_instance->readMessageFromT(results,subs);
		}
		string h;
		if(results.find("Queue")!=string::npos || results.find("Topic")!=string::npos)
		{
			h = msg.toXml();
			_mess_instance->logger << h << flush;
		}
		else
			h = "Improper Destination";
		_mess_instance->getServer()->Send(fd,h);
		//if (send(fd,&h[0] , h.length(), 0) == -1)
		//	_mess_instance->logger << "send failed" << flush;
	}
	else if(results.find("SUBSCRIBE ")!=string::npos)
	{
		int len = results.find("TO") - results.find("SUBSCRIBE ") - 11;
		string subs  = results.substr(results.find("SUBSCRIBE ")+10,len);
		results = results.substr(results.find("TO ")+3);
		results = (_mess_instance->path+results+":Subslist");
		_mess_instance->subscribe(subs,results);
		string h = "Subscribed";
		_mess_instance->getServer()->Send(fd,h);
		//if (send(fd,&h[0] , h.length(), 0) == -1)
		//	_mess_instance->logger << "send failed" << flush;
	}
	else if(results.find("UNSUBSCRIBE ")!=string::npos)
	{
		int len = results.find("TO") - results.find("UNSUBSCRIBE ") - 12;
		string subs  = results.substr(results.find("UNSUBSCRIBE ")+11,len);
		results = results.substr(results.find("TO ")+3);
		results = (_mess_instance->path+results+":Subslist");
		_mess_instance->subscribe(subs,results);
		string h = "Unsubscribed";
		_mess_instance->getServer()->Send(fd,h);
		//if (send(fd,&h[0] , h.length(), 0) == -1)
		//	_mess_instance->logger << "send failed" << flush;
	}
	memset(&buf[0], 0, sizeof(buf));
	close(fd);
}


void MessageHandler::init(string path)
{
	if(_mess_instance==NULL)
	{
		_mess_instance = new MessageHandler(path);
		_mess_instance->running = false;
	}
}

void MessageHandler::trigger(string port,string path)
{
	init(path);
	if(_mess_instance->running)
		return;
	_mess_instance->setServer(new Server(port,false,500,&service,true));
	_mess_instance->running = true;
	return;
}

