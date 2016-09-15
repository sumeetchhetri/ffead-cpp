/*
	Copyright 2009-2012, Sumeet Chhetri 
  
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

Message::Message(const std::string& xml)
{
	XmlParser parser("Parser");
	Document doc;
	parser.parse(xml, doc);
	const Element& message = doc.getRootElement();
	if(message.getTagName()!="message")
	{
		throw InvalidMessageException("No message Tag\n");
	}
	else if(message.getChildElements().size()!=2)
	{
		throw InvalidMessageException("Every message should have a headers and body tag\n");
	}
	Element* headers = message.getChildElements().at(0);
	Element* body = message.getChildElements().at(1);
	if(headers->getTagName()!="headers")
	{
		throw InvalidMessageException("No headers Tag\n");
	}
	else if(body->getTagName()!="body")
	{
		throw InvalidMessageException("No body Tag\n");
	}
	Element* destination = headers->getElementByName("destination");
	Element* encoding = headers->getElementByName("encoding");
	Element* timestamp = headers->getElementByName("timestamp");
	Element* priority = headers->getElementByName("priority");
	Element* type = headers->getElementByName("type");
	Element* userid = headers->getElementByName("userid");
	if(destination->getTagName()!="destination")
	{
		throw InvalidMessageException("Destination Header is mandatory\n");
	}
	else if (destination->getAttributes().size()!=2)
	{
		throw InvalidMessageException("Type and Name Attributes should be speciifes for a Destination\n");
	}
	else if (destination->getAttribute("name")=="" || destination->getAttribute("type")=="")
	{
		throw InvalidMessageException("Type and Name Attributes cannot be blank for a Destination\n");
	}
	else if(type->getTagName()!="type")
	{
		throw InvalidMessageException("Type Header is mandatory\n");
	}
	Destination des;
	des.setName(destination->getAttribute("name"));
	des.setType(destination->getAttribute("type"));
	this->destination = des;
	this->body = body->getText();
	this->timestamp = timestamp->getText();
	this->type = type->getText();
	this->priority = priority->getText();
	this->userId = userid->getText();
	this->encoding = encoding->getText();
}

Message::Message(Document& doc)
{
	const Element& message = doc.getRootElement();
	if(message.getTagName()!="message")
	{
		throw InvalidMessageException("No message Tag\n");
	}
	else if(message.getChildElements().size()!=2)
	{
		throw InvalidMessageException("Every message should have a headers and body tag\n");
	}
	Element* headers = message.getChildElements().at(0);
	Element* body = message.getChildElements().at(1);
	if(headers->getTagName()!="headers")
	{
		throw InvalidMessageException("No headers Tag\n");
	}
	else if(body->getTagName()!="body")
	{
		throw InvalidMessageException("No body Tag\n");
	}
	Element* destination = headers->getElementByName("destination");
	Element* encoding = headers->getElementByName("encoding");
	Element* timestamp = headers->getElementByName("timestamp");
	Element* priority = headers->getElementByName("priority");
	Element* type = headers->getElementByName("type");
	Element* userid = headers->getElementByName("userid");
	if(destination->getTagName()!="destination")
	{
		throw InvalidMessageException("Destination Header is mandatory\n");
	}
	else if (destination->getAttributes().size()!=2)
	{
		throw InvalidMessageException("Type and Name Attributes should be speciifes for a Destination\n");
	}
	else if (destination->getAttribute("name")=="" || destination->getAttribute("type")=="")
	{
		throw InvalidMessageException("Type and Name Attributes cannot be blank for a Destination\n");
	}
	else if(type->getTagName()!="type")
	{
		throw InvalidMessageException("Type Header is mandatory\n");
	}
	Destination des;
	des.setName(destination->getAttribute("name"));
	des.setType(destination->getAttribute("type"));
	this->destination = des;
	this->body = body->getText();
	this->timestamp = timestamp->getText();
	this->type = type->getText();
	this->priority = priority->getText();
	this->userId = userid->getText();
	this->encoding = encoding->getText();
}

std::string Message::toXml() const
{
	std::string xml;
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

void Message::setDestination(const Destination& destination)
{
	this->destination = destination;
}

std::string Message::getTimestamp() const
{
	return this->timestamp;
}

void Message::setTimestamp(const std::string& timestamp)
{
	this->timestamp = timestamp;
}

std::string Message::getType() const
{
	return this->type;
}

void Message::setType(const std::string& type)
{
	this->type = type;
}

std::string Message::getPriority() const
{
	return this->priority;
}

void Message::setPriority(const std::string& priority)
{
	this->priority = priority;
}

std::string Message::getUserId() const
{
	return this->userId;
}

void Message::setUserId(const std::string& userId)
{
	this->userId = userId;
}

std::string Message::getBody() const
{
	return this->body;
}

void Message::setBody(const std::string& body)
{
	this->body = body;
}

std::string Message::getEncoding() const
{
	return this->encoding;
}

void Message::setEncoding(const std::string& encoding)
{
	this->encoding = encoding;
}
