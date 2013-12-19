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
 * FormHandler.cpp
 *
 *  Created on: Jun 17, 2012
 *      Author: Sumeet
 */

#include "FormHandler.h"

FormHandler::FormHandler() {
	// TODO Auto-generated constructor stub

}

FormHandler::~FormHandler() {
	// TODO Auto-generated destructor stub
}

string FormHandler::handle(HttpRequest* req, HttpResponse& res)
{
	map<string, Element> formMap = ConfigurationData::getInstance()->formMap;
	Logger logger = LoggerFactory::getLogger("FormHandler");
	Reflector ref;
	Element ele = formMap[req->getFile()];
	//logger << ele.getTagName() << endl;
	//logger << ele.render() << endl;
	ClassInfo binfo = ref.getClassInfo(ele.getAttribute("bean"), req->getCntxt_name());
	ElementList eles = ele.getChildElements();
	string json = "{";
	for (unsigned int apps = 0; apps < eles.size(); apps++)
	{
		if(eles.at(apps).getTagName()=="field")
		{
			string name = eles.at(apps).getAttribute("name");
			Field fld = binfo.getField(eles.at(apps).getAttribute("prop"));
			if(fld.getType()=="string")
				json += "\""+eles.at(apps).getAttribute("prop")+"\": \"" + req->getParamValue(name) + "\",";
			else
			{
				if(fld.getType()=="int" || fld.getType()=="long")
				{
					if(req->getQueryParam(name)=="")
						json += "\""+eles.at(apps).getAttribute("prop")+"\": 0,";
					else
						json += "\""+eles.at(apps).getAttribute("prop")+"\": " + req->getParamValue(name) + ",";
				}
				else if(fld.getType()=="double" || fld.getType()=="float")
				{
					if(req->getQueryParam(name)=="")
						json += "\""+eles.at(apps).getAttribute("prop")+"\": 0.0,";
					else
						json += "\""+eles.at(apps).getAttribute("prop")+"\": " + req->getParamValue(name) + ",";
				}
				else if(fld.getType()=="bool")
				{
					if(req->getQueryParam(name)=="")
						json += "\""+eles.at(apps).getAttribute("prop")+"\": false,";
					else
						json += "\""+eles.at(apps).getAttribute("prop")+"\": " + req->getParamValue(name) + ",";
				}
			}
		}
	}
	if(json.find(",")!=string::npos)
	{
		json = json.substr(0,json.length()-1);
	}
	json += "}";
	logger << json << endl;

	void *_temp = ConfigurationData::getInstance()->ffeadContext.getBean("form_"+req->getCntxt_name()+ele.getAttribute("controller"), req->getCntxt_name());
	logger << ("Fetching Formcontroller for " + ele.getAttribute("bean")) << endl;
	if(_temp!=NULL)
	{
		void *_beaninst = JSONSerialize::unSerializeUnknown(json, ele.getAttribute("bean"), req->getCntxt_name());

		FormController* formController = (FormController*)_temp;
		if(formController!=NULL)
		{
			formController->onSubmit(_beaninst,&res);
			logger << "Successfully called Formcontroller" << endl;
		}
		else
		{
			res.setHTTPResponseStatus(HTTPResponseStatus::NotFound);
			res.addHeaderValue(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_TEXT_PLAIN);
			res.setContent("Formcontroller Method Not Found");
			logger << "Formcontroller Method Not Found" << endl;
		}
	}
	return json;
}
