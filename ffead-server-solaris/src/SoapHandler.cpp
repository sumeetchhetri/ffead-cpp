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
 * SoapHandler.cpp
 *
 *  Created on: Jun 17, 2012
 *      Author: Sumeet
 */

#include "SoapHandler.h"

SoapHandler::SoapHandler() {
	// TODO Auto-generated constructor stub

}

SoapHandler::~SoapHandler() {
	// TODO Auto-generated destructor stub
}

void SoapHandler::handle(HttpRequest* req, HttpResponse& res, void* dlib, string xmlcnttype)
{
	Logger logger = Logger::getLogger("SoapHandler");
	string meth,ws_name,env;
	ws_name = req->getFile();
	Element soapenv;
	logger.info("request => "+req->getContent());
	Element soapbody;
	try
	{
		XmlParser parser("Parser");
		Document doc = parser.getDocument(req->getContent());
		soapenv = doc.getRootElement();
		logger << soapenv.getTagName() << "----\n" << flush;

		if(soapenv.getChildElements().size()==1
				&& soapenv.getChildElements().at(0).getTagName()=="Body")
			soapbody = soapenv.getChildElements().at(0);
		else if(soapenv.getChildElements().size()==2
				&& soapenv.getChildElements().at(1).getTagName()=="Body")
			soapbody = soapenv.getChildElements().at(1);
		logger << soapbody.getTagName() << "----\n" << flush;
		Element method = soapbody.getChildElements().at(0);
		logger << method.getTagName() << "----\n" << flush;
		meth = method.getTagName();
		string methodname = meth + ws_name;
		logger << methodname << "----\n" << flush;
		void *mkr = dlsym(dlib, methodname.c_str());
		if(mkr!=NULL)
		{
			typedef string (*WsPtr) (Element);
			WsPtr f =  (WsPtr)mkr;
			string outpt = f(method);
			typedef map<string,string> AttributeList;
			AttributeList attl = soapbody.getAttributes();
			AttributeList::iterator it;
			string bod = "<" + soapbody.getTagNameSpc();
			for(it=attl.begin();it!=attl.end();it++)
			{
				bod.append(" " + it->first + "=\"" + it->second + "\" ");
			}
			bod.append(">"+outpt + "</" + soapbody.getTagNameSpc()+">");
			attl = soapenv.getAttributes();
			env = "<" + soapenv.getTagNameSpc();
			for(it=attl.begin();it!=attl.end();it++)
			{
				env.append(" " + it->first + "=\"" + it->second + "\" ");
			}
			env.append(">"+bod + "</" + soapenv.getTagNameSpc()+">");
			//delete mkr;
		}
		else
		{
			typedef map<string,string> AttributeList;
			AttributeList attl = soapbody.getAttributes();
			AttributeList::iterator it;
			string bod = "<" + soapbody.getTagNameSpc();
			for(it=attl.begin();it!=attl.end();it++)
			{
				bod.append(" " + it->first + "=\"" + it->second + "\" ");
			}
			bod.append("><soap-fault><faultcode>soap:Server</faultcode><faultstring>Operation not supported</faultstring><faultactor/><detail>No such method error</detail><soap-fault></" + soapbody.getTagNameSpc()+">");
			attl = soapenv.getAttributes();
			env = "<" + soapenv.getTagNameSpc();
			for(it=attl.begin();it!=attl.end();it++)
			{
				env.append(" " + it->first + "=\"" + it->second + "\" ");
			}
			env.append(">"+bod + "</" + soapenv.getTagNameSpc()+">");
		}
		logger << "\n----------------------------------------------------------------------------\n" << flush;
		logger << env << "\n----------------------------------------------------------------------------\n" << flush;
	}
	catch(string &fault)
	{
		typedef map<string,string> AttributeList;
		AttributeList attl = soapbody.getAttributes();
		AttributeList::iterator it;
		string bod = "<" + soapbody.getTagNameSpc();
		for(it=attl.begin();it!=attl.end();it++)
		{
			bod.append(" " + it->first + "=\"" + it->second + "\" ");
		}
		bod.append("><soap-fault><faultcode>soap:Server</faultcode><faultstring>"+fault+"</faultstring><detail></detail><soap-fault></" + soapbody.getTagNameSpc()+">");
		attl = soapenv.getAttributes();
		env = "<" + soapenv.getTagNameSpc();
		for(it=attl.begin();it!=attl.end();it++)
		{
			env.append(" " + it->first + "=\"" + it->second + "\" ");
		}
		env.append(">"+bod + "</" + soapenv.getTagNameSpc()+">");
		logger << fault << flush;
	}
	catch(Exception *e)
	{
		typedef map<string,string> AttributeList;
		AttributeList attl = soapbody.getAttributes();
		AttributeList::iterator it;
		string bod = "<" + soapbody.getTagNameSpc();
		for(it=attl.begin();it!=attl.end();it++)
		{
			bod.append(" " + it->first + "=\"" + it->second + "\" ");
		}
		bod.append("><soap-fault><faultcode>soap:Server</faultcode><faultstring>"+e->what()+"</faultstring><detail></detail><soap-fault></" + soapbody.getTagNameSpc()+">");
		attl = soapenv.getAttributes();
		env = "<" + soapenv.getTagNameSpc();
		for(it=attl.begin();it!=attl.end();it++)
		{
			env.append(" " + it->first + "=\"" + it->second + "\" ");
		}
		env.append(">"+bod + "</" + soapenv.getTagNameSpc()+">");
		logger << e->what() << flush;
	}
	catch(...)
	{
		typedef map<string,string> AttributeList;
		AttributeList attl = soapbody.getAttributes();
		AttributeList::iterator it;
		string bod = "<" + soapbody.getTagNameSpc();
		for(it=attl.begin();it!=attl.end();it++)
		{
			bod.append(" " + it->first + "=\"" + it->second + "\" ");
		}
		bod.append("><soap-fault><faultcode>soap:Server</faultcode><faultstring>Standard Exception</faultstring><detail></detail><soap-fault></" + soapbody.getTagNameSpc()+">");
		attl = soapenv.getAttributes();
		env = "<" + soapenv.getTagNameSpc();
		for(it=attl.begin();it!=attl.end();it++)
		{
			env.append(" " + it->first + "=\"" + it->second + "\" ");
		}
		env.append(">"+bod + "</" + soapenv.getTagNameSpc()+">");
		logger << "Standard Exception" << flush;
	}
	res.setStatusCode("200");
	res.setStatusMsg("OK");
	res.setContent_type(xmlcnttype);
	res.setContent_str(env);
	//res.setContent_len(boost::lexical_cast<string>(env.length()));
}
