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
 * FviewHandler.cpp
 *
 *  Created on: Jun 17, 2012
 *      Author: Sumeet
 */

#include "FviewHandler.h"

FviewHandler::FviewHandler() {
	// TODO Auto-generated constructor stub

}

FviewHandler::~FviewHandler() {
	// TODO Auto-generated destructor stub
}

string FviewHandler::handle(HttpRequest* req, HttpResponse& res, map<string, string> fviewmap)
{
	Logger logger = Logger::getLogger("FviewHandler");
	string content;
	logger << "inside fview " << req->getFile() << endl;
	string file = req->getFile();
	StringUtil::replaceFirst(file,"fview","html");
	string ffile = req->getCntxt_root()+"/fviews/"+file;
	logger << ffile << endl;
	ifstream infile(ffile.c_str());
	string temp;
	if(infile.is_open())
	{
		content = "";
		while(getline(infile, temp))
		{
			if(temp.find("<?")==string::npos && temp.find("?>")==string::npos)
				content.append(temp);
		}
		int h = content.find("</head>");
		int ht = content.find("<html>");
		if(h!=string::npos)
		{
			string st = content.substr(0,h-1);
			string en = content.substr(h);
			content = st + "<script type=\"text/javascript\" src=\"public/json2.js\"></script>";
			content += "<script type=\"text/javascript\" src=\"public/prototype.js\"></script>";
			content += "<script type=\"text/javascript\" src=\"public/afc.js\"></script>";
			content += "<script type=\"text/javascript\" src=\"public/_afc_Objects.js\"></script>";
			content += "<script type=\"text/javascript\" src=\"public/_afc_Interfaces.js\"></script>";
			content += "<script type=\"text/javascript\" src=\"public/"+fviewmap[file]+".js\"></script>" + en;
		}
		else
		{
			if(ht!=string::npos)
			{
				string st = content.substr(0,ht+6);
				string en = content.substr(ht+6);
				content = st + "<script type=\"text/javascript\" src=\"public/json2.js\"></script>";
				content += "<script type=\"text/javascript\" src=\"public/prototype.js\"></script>";
				content += "<script type=\"text/javascript\" src=\"public/afc.js\"></script>";
				content += "<script type=\"text/javascript\" src=\"public/_afc_Objects.js\"></script>";
				content += "<script type=\"text/javascript\" src=\"public/_afc_Interfaces.js\"></script>";
				content += "<script type=\"text/javascript\" src=\"public/"+fviewmap[file]+".js\"></script>" + en;
			}
		}
	}
	infile.close();
	res.setContent_type("text/html");
	//logger << content << flush;
	return content;
}
