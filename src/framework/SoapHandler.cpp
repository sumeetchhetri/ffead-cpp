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
 * SoapHandler.cpp
 *
 *  Created on: Jun 17, 2012
 *      Author: Sumeet
 */

#include "SoapHandler.h"

void SoapHandler::handle(HttpRequest* req, HttpResponse* res, void* dlib, std::string ws_name)
{
	Logger logger = LoggerFactory::getLogger("SoapHandler");
	std::string wsUrl = "http://" + ConfigurationData::getInstance()->coreServerProperties.ip_address + "/";
	wsUrl += req->getCurl();
	logger << ("WsUrl is " + wsUrl) << std::endl;

	std::string xmlcnttype = CommonUtils::getMimeType(".xml");
	std::string meth,env;
	Element* soapenv = NULL;
	logger.info("request => "+req->getContent());
	Element* soapbody = NULL;
	try
	{
		XmlParser parser("Validator");
		Document doc;
		parser.parse(req->getContent(), doc);
		soapenv = &(doc.getRootElement());

		soapbody = soapenv->getElementByNameIgnoreCase("body");
		if(soapbody == NULL) {
			throw "SOAP Body not found in request";
		}

		//logger << soapbody->getTagName() << "----\n" << std::flush;
		Element* method = (Element*)&(soapbody->getChildElements().at(0));
		//logger << method.getTagName() << "----\n" << std::flush;
		meth = method->getTagName();
		std::string methodname = req->getCntxt_name() + meth + ws_name;
		//logger << methodname << "----\n" << std::flush;
		void *mkr = dlsym(dlib, methodname.c_str());
		if(mkr!=NULL)
		{
			typedef std::string (*WsPtr) (Element*);
			WsPtr f =  (WsPtr)mkr;
			std::string outpt = f(method);
			typedef std::map<std::string,std::string> AttributeList;
			AttributeList attl = soapbody->getAttributes();
			AttributeList::iterator it;
			std::string bod = "<" + soapbody->getTagNameSpc();
			for(it=attl.begin();it!=attl.end();it++)
			{
				bod.append(" " + it->first + "=\"" + it->second + "\" ");
			}
			bod.append(">"+outpt + "</" + soapbody->getTagNameSpc()+">");
			attl = soapenv->getAttributes();
			env = "<" + soapenv->getTagNameSpc();
			for(it=attl.begin();it!=attl.end();it++)
			{
				env.append(" " + it->first + "=\"" + it->second + "\" ");
			}
			env.append(">"+bod + "</" + soapenv->getTagNameSpc()+">");
			//delete mkr;
		}
		else
		{
			AttributeList attl = soapbody->getAttributes();
			AttributeList::iterator it;
			std::string bod = "<" + soapbody->getTagNameSpc();
			for(it=attl.begin();it!=attl.end();it++)
			{
				bod.append(" " + it->first + "=\"" + it->second + "\" ");
			}
			bod.append("><soap-fault><faultcode>soap:Server</faultcode><faultstring>Operation not supported</faultstring><faultactor/><detail>No such method error</detail><soap-fault></" + soapbody->getTagNameSpc()+">");
			attl = soapenv->getAttributes();
			env = "<" + soapenv->getTagNameSpc();
			for(it=attl.begin();it!=attl.end();it++)
			{
				env.append(" " + it->first + "=\"" + it->second + "\" ");
			}
			env.append(">"+bod + "</" + soapenv->getTagNameSpc()+">");
		}
		//logger << "\n----------------------------------------------------------------------------\n" << std::flush;
		//logger << env << "\n----------------------------------------------------------------------------\n" << std::flush;
	}
	catch(const char* faultc)
	{
		std::string fault(faultc);
		std::string bod = "", btag = "";
		AttributeList attl;
		AttributeList::iterator it;
		if(soapbody!=NULL)
		{
			attl = soapbody->getAttributes();
			btag = soapbody->getTagNameSpc();
			bod = "<" + soapbody->getTagNameSpc();
			for(it=attl.begin();it!=attl.end();it++)
			{
				bod.append(" " + it->first + "=\"" + it->second + "\" ");
			}
		}
		else
		{
			btag = soapenv->getNameSpc() + ":body";
			bod = "<" + btag;
		}
		bod.append("><soap-fault><faultcode>soap:Server</faultcode><faultstring>"+fault+"</faultstring><detail></detail><soap-fault></" + btag+">");
		attl = soapenv->getAttributes();
		env = "<" + soapenv->getTagNameSpc();
		for(it=attl.begin();it!=attl.end();it++)
		{
			env.append(" " + it->first + "=\"" + it->second + "\" ");
		}
		env.append(">"+bod + "</" + soapenv->getTagNameSpc()+">");
		logger << ("Soap fault - " + fault) << std::flush;
	}
	catch(const std::string &fault)
	{
		std::string bod = "", btag = "";
		AttributeList attl;
		AttributeList::iterator it;
		if(soapbody!=NULL)
		{
			attl = soapbody->getAttributes();
			btag = soapbody->getTagNameSpc();
			bod = "<" + soapbody->getTagNameSpc();
			for(it=attl.begin();it!=attl.end();it++)
			{
				bod.append(" " + it->first + "=\"" + it->second + "\" ");
			}
		}
		else
		{
			btag = soapenv->getNameSpc() + ":body";
			bod = "<" + btag;
		}
		bod.append("><soap-fault><faultcode>soap:Server</faultcode><faultstring>"+fault+"</faultstring><detail></detail><soap-fault></" + btag+">");
		attl = soapenv->getAttributes();
		env = "<" + soapenv->getTagNameSpc();
		for(it=attl.begin();it!=attl.end();it++)
		{
			env.append(" " + it->first + "=\"" + it->second + "\" ");
		}
		env.append(">"+bod + "</" + soapenv->getTagNameSpc()+">");
		logger << ("Soap fault - " + fault) << std::flush;
	}
	catch(const Exception& e)
	{
		std::string bod = "", btag = "";
		AttributeList attl;
		AttributeList::iterator it;
		if(soapbody!=NULL)
		{
			attl = soapbody->getAttributes();
			btag = soapbody->getTagNameSpc();
			bod = "<" + soapbody->getTagNameSpc();
			for(it=attl.begin();it!=attl.end();it++)
			{
				bod.append(" " + it->first + "=\"" + it->second + "\" ");
			}
		}
		else
		{
			btag = soapenv->getNameSpc() + ":body";
			bod = "<" + btag;
		}
		bod.append("><soap-fault><faultcode>soap:Server</faultcode><faultstring>"+e.getMessage()+"</faultstring><detail></detail><soap-fault></" + btag+">");
		attl = soapenv->getAttributes();
		env = "<" + soapenv->getTagNameSpc();
		for(it=attl.begin();it!=attl.end();it++)
		{
			env.append(" " + it->first + "=\"" + it->second + "\" ");
		}
		env.append(">"+bod + "</" + soapenv->getTagNameSpc()+">");
		logger << ("Soap fault - " + e.getMessage()) << std::flush;
	}
	catch(...)
	{
		std::string bod = "", btag = "";
		AttributeList attl;
		AttributeList::iterator it;
		if(soapbody!=NULL)
		{
			attl = soapbody->getAttributes();
			btag = soapbody->getTagNameSpc();
			bod = "<" + soapbody->getTagNameSpc();
			for(it=attl.begin();it!=attl.end();it++)
			{
				bod.append(" " + it->first + "=\"" + it->second + "\" ");
			}
		}
		else
		{
			btag = soapenv->getNameSpc() + ":body";
			bod = "<" + btag;
		}
		bod.append("><soap-fault><faultcode>soap:Server</faultcode><faultstring>Standard Exception</faultstring><detail></detail><soap-fault></" + btag+">");
		attl = soapenv->getAttributes();
		env = "<" + soapenv->getTagNameSpc();
		for(it=attl.begin();it!=attl.end();it++)
		{
			env.append(" " + it->first + "=\"" + it->second + "\" ");
		}
		env.append(">"+bod + "</" + soapenv->getTagNameSpc()+">");
		logger << "Soap Standard Exception" << std::flush;
	}
	res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
	res->addHeaderValue(HttpResponse::ContentType, xmlcnttype);
	res->setContent(env);
	res->setDone(true);
}
