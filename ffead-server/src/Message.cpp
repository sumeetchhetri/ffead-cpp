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
 * Message.cpp
 *
 *  Created on: Sep 21, 2009
 *      Author: sumeet
 */

#include "Message.h"

Message::Message()
{

}

Message::Message(string xml)
{
	try
	{
		XmlParser parser("Parser");
		Document doc = parser.getDocument(xml);
		Element message = doc.getRootElement();
		if(message.getTagName()!="message")
		{
			throw new InvalidMessageException("No message Tag\n");
		}
		else if(message.getChildElements().size()!=2)
		{
			throw new InvalidMessageException("Every message should have a headers and body tag\n");
		}
		Element headers = message.getChildElements().at(0);
		Element body = message.getChildElements().at(1);
		if(headers.getTagName()!="headers")
		{
			throw new InvalidMessageException("No headers Tag\n");
		}
		else if(body.getTagName()!="body")
		{
			throw new InvalidMessageException("No body Tag\n");
		}
		Element destination = headers.getElementByName("destination");
		Element encoding = headers.getElementByName("encoding");
		Element timestamp = headers.getElementByName("timestamp");
		Element priority = headers.getElementByName("priority");
		Element type = headers.getElementByName("type");
		Element userid = headers.getElementByName("userid");
		if(destination.getTagName()!="destination")
		{
			throw new InvalidMessageException("Destination Header is mandatory\n");
		}
		else if (destination.getAttributes().size()!=2)
		{
			throw new InvalidMessageException("Type and Name Attributes should be speciifes for a Destination\n");
		}
		else if (destination.getAttribute("name")=="" || destination.getAttribute("type")=="")
		{
			throw new InvalidMessageException("Type and Name Attributes cannot be blank for a Destination\n");
		}
		else if(type.getTagName()!="type")
		{
			throw new InvalidMessageException("Type Header is mandatory\n");
		}
		Destination des;
		des.setName(destination.getAttribute("name"));
		des.setType(destination.getAttribute("type"));
		this->destination = des;
		this->body = body.getText();
		this->timestamp = timestamp.getText();
		this->type = type.getText();
		this->priority = priority.getText();
		this->userId = userid.getText();
		this->encoding = encoding.getText();
	}
	catch(Exception *e)
	{
		cout << e->what() << flush;
	}
}

Message::Message(Document doc)
{
	Element message = doc.getRootElement();
	if(message.getTagName()!="message")
	{
		throw new InvalidMessageException("No message Tag\n");
	}
	else if(message.getChildElements().size()!=2)
	{
		throw new InvalidMessageException("Every message should have a headers and body tag\n");
	}
	Element headers = message.getChildElements().at(0);
	Element body = message.getChildElements().at(1);
	if(headers.getTagName()!="headers")
	{
		throw new InvalidMessageException("No headers Tag\n");
	}
	else if(body.getTagName()!="body")
	{
		throw new InvalidMessageException("No body Tag\n");
	}
	Element destination = headers.getElementByName("destination");
	Element encoding = headers.getElementByName("encoding");
	Element timestamp = headers.getElementByName("timestamp");
	Element priority = headers.getElementByName("priority");
	Element type = headers.getElementByName("type");
	Element userid = headers.getElementByName("userid");
	if(destination.getTagName()!="destination")
	{
		throw new InvalidMessageException("Destination Header is mandatory\n");
	}
	else if (destination.getAttributes().size()!=2)
	{
		throw new InvalidMessageException("Type and Name Attributes should be speciifes for a Destination\n");
	}
	else if (destination.getAttribute("name")=="" || destination.getAttribute("type")=="")
	{
		throw new InvalidMessageException("Type and Name Attributes cannot be blank for a Destination\n");
	}
	else if(type.getTagName()!="type")
	{
		throw new InvalidMessageException("Type Header is mandatory\n");
	}
	Destination des;
	des.setName(destination.getAttribute("name"));
	des.setType(destination.getAttribute("type"));
	this->destination = des;
	this->body = body.getText();
	this->timestamp = timestamp.getText();
	this->type = type.getText();
	this->priority = priority.getText();
	this->userId = userid.getText();
	this->encoding = encoding.getText();
}

string Message::toXml()
{
	string xml;
	xml = "<message>\n<headers>\n<destination ";
	xml += ("name=\""+this->getDestination().getName());
	xml += ("\" type=\""+this->getDestination().getType());
	xml += ("\"></destination>\n");
	xml += ("<timestamp>\n");
	xml += (this->getTimestamp());
	xml += ("</timestamp>\n");
	xml += ("<priority>\n");
	xml += (this->getPriority());
	xml += ("</priority>\n");
	xml += ("<type>\n");
	xml += (this->getType());
	xml += ("</type>\n");
	xml += ("<userId>\n");
	xml += (this->getUserId());
	xml += ("</userId>\n");
	xml += ("<encoding>\n");
	xml += (this->getEncoding());
	xml += ("</encoding>\n");
	xml += ("</headers>\n");
	xml += ("<body>\n");
	xml += (this->getBody());
	xml += ("</body>\n");
	xml += ("</message>\n");
	return xml;
}

Message::~Message() {
	// TODO Auto-generated destructor stub
}

Destination Message::getDestination() const
{
	return this->destination;
}

void Message::setDestination(Destination destination)
{
	this->destination = destination;
}

string Message::getTimestamp() const
{
	return this->timestamp;
}

void Message::setTimestamp(string timestamp)
{
	this->timestamp = timestamp;
}

string Message::getType() const
{
	return this->type;
}

void Message::setType(string type)
{
	this->type = type;
}

string Message::getPriority() const
{
	return this->priority;
}

void Message::setPriority(string priority)
{
	this->priority = priority;
}

string Message::getUserId() const
{
	return this->userId;
}

void Message::setUserId(string userId)
{
	this->userId = userId;
}

string Message::getBody() const
{
	return this->body;
}

void Message::setBody(string body)
{
	this->body = body;
}

string Message::getEncoding() const
{
	return this->encoding;
}

void Message::setEncoding(string encoding)
{
	this->encoding = encoding;
}
