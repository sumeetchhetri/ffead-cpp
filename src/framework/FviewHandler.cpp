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
 * FviewHandler.cpp
 *
 *  Created on: Jun 17, 2012
 *      Author: Sumeet
 */

#include "FviewHandler.h"

bool FviewHandler::handle(HttpRequest* req, HttpResponse* res)
{
	std::map<std::string, std::string, std::less<> >& fviewmap = ConfigurationData::getInstance()->fviewMappingMap.find(req->getCntxt_name())->second;
	//Logger logger = LoggerFactory::getLogger("FviewHandler");
	std::string content;
	//logger << ("Inside fview " + req->getFile()) << std::endl;
	std::string file = req->getFile();
	StringUtil::replaceFirst(file,"fview","html");
	std::string ffile = req->getCntxt_root()+"/fviews/"+file;
	//logger << ffile << std::endl;
	std::ifstream infile(ffile.c_str(), std::ios::binary);
	std::string temp;
	if(infile.is_open())
	{
		content = "";
		while(getline(infile, temp))
		{
			if(temp.find("<?")==std::string::npos && temp.find("?>")==std::string::npos)
				content.append(temp);
		}
		int h = content.find("</head>");
		int ht = content.find("<html>");
		if(h!=(int)std::string::npos)
		{
			std::string st = content.substr(0,h-1);
			std::string en = content.substr(h);
			content = st + "<script type=\"text/javascript\" src=\"json2.js\"></script>";
			content += "<script type=\"text/javascript\" src=\"prototype.js\"></script>";
			content += "<script type=\"text/javascript\" src=\"afc.js\"></script>";
			content += "<script type=\"text/javascript\" src=\"_afc_Objects.js\"></script>";
			content += "<script type=\"text/javascript\" src=\"_afc_Interfaces.js\"></script>";
			content += "<script type=\"text/javascript\" src=\""+fviewmap[file]+".js\"></script>" + en;
		}
		else
		{
			if(ht!=(int)std::string::npos)
			{
				std::string st = content.substr(0,ht+6);
				std::string en = content.substr(ht+6);
				content = st + "<script type=\"text/javascript\" src=\"json2.js\"></script>";
				content += "<script type=\"text/javascript\" src=\"prototype.js\"></script>";
				content += "<script type=\"text/javascript\" src=\"afc.js\"></script>";
				content += "<script type=\"text/javascript\" src=\"_afc_Objects.js\"></script>";
				content += "<script type=\"text/javascript\" src=\"_afc_Interfaces.js\"></script>";
				content += "<script type=\"text/javascript\" src=\""+fviewmap[file]+".js\"></script>" + en;
			}
		}
		res->addHeader(HttpResponse::ContentType, ContentTypes::CONTENT_TYPE_TEXT_SHTML);
		res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
		res->setContent(content);
		infile.close();
		return true;
	}
	return false;
}
