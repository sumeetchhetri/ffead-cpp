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

string FormHandler::handle(HttpRequest* req, HttpResponse& res, map<string, Element> formMap, void* dlib)
{
	Reflector ref;
	Element ele = formMap[req->getFile()];
	cout << ele.getTagName() << endl;
	cout << ele.render() << endl;
	ClassInfo binfo = ref.getClassInfo(ele.getAttribute("bean"));
	ElementList eles = ele.getChildElements();
	string json = "{";
	for (unsigned int apps = 0; apps < eles.size(); apps++)
	{
		if(eles.at(apps).getTagName()=="field")
		{
			string name = eles.at(apps).getAttribute("name");
			Field fld = binfo.getField(eles.at(apps).getAttribute("prop"));
			if(fld.getType()=="string")
				json += "\""+eles.at(apps).getAttribute("prop")+"\": \"" + req->getQueryParam(name) + "\",";
			else
			{
				if(fld.getType()=="int" || fld.getType()=="long")
				{
					if(req->getQueryParam(name)=="")
						json += "\""+eles.at(apps).getAttribute("prop")+"\": 0,";
					else
						json += "\""+eles.at(apps).getAttribute("prop")+"\": " + req->getQueryParam(name) + ",";
				}
				else if(fld.getType()=="double" || fld.getType()=="float")
				{
					if(req->getQueryParam(name)=="")
						json += "\""+eles.at(apps).getAttribute("prop")+"\": 0.0,";
					else
						json += "\""+eles.at(apps).getAttribute("prop")+"\": " + req->getQueryParam(name) + ",";
				}
				else if(fld.getType()=="bool")
				{
					if(req->getQueryParam(name)=="")
						json += "\""+eles.at(apps).getAttribute("prop")+"\": false,";
					else
						json += "\""+eles.at(apps).getAttribute("prop")+"\": " + req->getQueryParam(name) + ",";
				}
			}
		}
	}
	if(json.find(",")!=string::npos)
	{
		json = json.substr(0,json.length()-1);
	}
	json += "}";
	cout << json << endl;
	string libName = "libinter.a";
	if(dlib == NULL)
	{
		cerr << dlerror() << endl;
		exit(-1);
	}
	string meth = "toVoidP" + ele.getAttribute("bean");
	cout << meth << endl;
	void *mkr = dlsym(dlib, meth.c_str());
	if(mkr!=NULL)
	{
		toVoidP f1 = (toVoidP)mkr;
		void *_beaninst = f1(json);
		//FunPtr f =  (FunPtr)mkr;
		ClassInfo srv = ref.getClassInfo(ele.getAttribute("controller"));
		args argus;
		vals valus;
		Constructor ctor = srv.getConstructor(argus);
		void *_temp = ref.newInstanceGVP(ctor);
		argus.push_back("void*");
		argus.push_back("HttpResponse*");
		valus.push_back(_beaninst);
		valus.push_back(&res);
		Method meth = srv.getMethod("onSubmit",argus);
		if(meth.getMethodName()!="")
		{
			ref.invokeMethodUnknownReturn(_temp,meth,valus);
			cout << "successfully called formcontroller" << endl;
		}
		else
		{
			res.setStatusCode("404");
			res.setStatusMsg("Not Found");
			res.setContent_type("text/plain");
			res.setContent_str("Controller Method Not Found");
			cout << "Controller Method Not Found" << endl;
		}
	}
	return json;
}
