/*
 * FilterHandler.cpp
 *
 *  Created on: Jun 17, 2012
 *      Author: Sumeet
 */

#include "FilterHandler.h"

FilterHandler::FilterHandler() {
	// TODO Auto-generated constructor stub

}

FilterHandler::~FilterHandler() {
	// TODO Auto-generated destructor stub
}

void FilterHandler::handleIn(HttpRequest* req, HttpResponse& res, map<string, vector<string> > filterMap, void* dlib,
		string ext)
{
	if(filterMap.find(req->getCntxt_name()+"*.*in")!=filterMap.end() || filterMap.find(req->getCntxt_name()+ext+"in")!=filterMap.end())
	{
		vector<string> tempp;
		if(filterMap.find(req->getCntxt_name()+"*.*in")!=filterMap.end())
			tempp = filterMap[req->getCntxt_name()+"*.*in"];
		else
			tempp = filterMap[req->getCntxt_name()+ext+"in"];

		for (int var = 0; var < (int)tempp.size(); ++var)
		{
			string clasz = tempp.at(var);
			clasz = "getReflectionCIFor" + clasz;
			cout << "filter handled by class " << clasz << " " << dlib << endl;
			if(dlib == NULL)
			{
				cout << "error" << endl;
				cerr << dlerror() << endl;
				exit(-1);
			}
			void *mkr = dlsym(dlib, clasz.c_str());
			if(mkr!=NULL)
			{
				FunPtr f =  (FunPtr)mkr;
				ClassInfo srv = f();
				args argus;
				Constructor ctor = srv.getConstructor(argus);
				Reflector ref;
				void *_temp = ref.newInstanceGVP(ctor);
				Filter *filter = (Filter*)_temp;
				filter->doInputFilter(req);
				cout << "filter called" << endl;
				delete _temp;
			}
		}
	}
}

void FilterHandler::handleOut(HttpRequest* req, HttpResponse& res, map<string, vector<string> > filterMap, void* dlib,
		string ext)
{
	if(filterMap.find(req->getCntxt_name()+"*.*out")!=filterMap.end() || filterMap.find(req->getCntxt_name()+ext+"out")!=filterMap.end())
	{
		vector<string> tempp;
		if(filterMap.find(req->getCntxt_name()+"*.*out")!=filterMap.end())
			tempp = filterMap[req->getCntxt_name()+"*.*out"];
		else
			tempp = filterMap[req->getCntxt_name()+ext+"out"];

		for (int var = 0; var < (int)tempp.size(); ++var)
		{
			string clasz = tempp.at(var);
			clasz = "getReflectionCIFor" + clasz;
			cout << "filter handled by class " << clasz << endl;
			if(dlib == NULL)
			{
				cerr << dlerror() << endl;
				exit(-1);
			}
			void *mkr = dlsym(dlib, clasz.c_str());
			if(mkr!=NULL)
			{
				FunPtr f =  (FunPtr)mkr;
				ClassInfo srv = f();
				args argus;
				Constructor ctor = srv.getConstructor(argus);
				Reflector ref;
				void *_temp = ref.newInstanceGVP(ctor);
				Filter *filter = (Filter*)_temp;
				filter->doOutputFilter(&res);
				cout << "filter called" << endl;
				delete _temp;
			}
		}
	}
}
