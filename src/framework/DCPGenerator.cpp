/*
	Copyright 2009-2020, Sumeet Chhetri 
  
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
 * DCPGenerator.cpp
 *
 *  Created on: Sep 5, 2009
 *      Author: sumeet
 */

#include "DCPGenerator.h"


DCPGenerator::DCPGenerator() {
	

}

DCPGenerator::~DCPGenerator() {
	
}
std::string DCPGenerator::generateDCPAll()
{
	std::map<std::string,std::string, std::less<> > fileNames = ConfigurationData::getInstance()->dynamicCppPagesMap;
	std::string bodies,headersb="#include \"AfcUtil.h\"",funcdefs;
	std::map<std::string, std::string>::iterator it;
	for (it=fileNames.begin();it!=fileNames.end();++it)
	{
		bodies += generateDCP(it->first,headersb,funcdefs,it->second);
	}
	bodies = (headersb+"\nextern \"C\"\n{\n"+funcdefs+bodies+"}\n");
	return bodies;
}

std::string DCPGenerator::generateDCP(const std::string& fileName, std::string &headersb, std::string &funcdefs, const std::string& app)
{
	std::ifstream infile;
	std::string data,allcontent;
	infile.open(fileName.c_str());
	std::string file,dir;
	/*int s,en;
	s = fileName.find_last_of("/")+1;
	dir = fileName.substr(0,s-1);
	en = fileName.find_last_of(".");
	file = fileName.substr(s,en-s);*/
	file = fileName;
	RegexUtil::replace(file,"[/]+","/");
	StringUtil::replaceFirst(file, ConfigurationData::getInstance()->coreServerProperties.webPath, "");
	RegexUtil::replace(file, "[^a-zA-Z0-9_]+", "");
	if(infile)
	{
		while(getline(infile, data))
		{
			if(data.find("<import>")!=std::string::npos && data.find("</import>")!=std::string::npos)
			{
				int s = data.find("<import>")+8;
				int e = data.find("</import>");
				data=data.substr(s,e-s);
				std::string file1 = app + "/dcp/" + data;
				RegexUtil::replace(file1, "[^a-zA-Z0-9_]+", "");
				allcontent.append("<DCPB>screen << _"+file1+"emittHTML();\n</DCPB>");
				/*ifstream inf(data.c_str());
				if(inf)
				{
					while(getline(inf, data))
					{
						allcontent.append(data+"\n");
					}
				}*/
			}
			else
				allcontent.append(data+"\n");
		}
	}
	std::string header,bodies,funcs;
	int b = allcontent.find("<DCPH>") + 6;
	int e = allcontent.find("</DCPH>");
	int len = e - b;
	if(len>0)
	{
		header.append(allcontent.substr(b,len));
		allcontent = allcontent.substr(e+7);
		headersb.append(header+"\n");
	}

	while(allcontent.find("<DCPF>")!=std::string::npos)
	{
		b = allcontent.find("<DCPF>");
		e = allcontent.find("</DCPF>") + 7;
		std::string temp1 = allcontent.substr(b,e-b);
		StringUtil::replaceAll(allcontent,temp1,"");

		//string ter(allcontent.substr(0,b-6));
		//StringUtil::replaceAll(ter,"\n","");
		//StringUtil::replaceAll(ter,"\"","\\\"");
		e = temp1.find("</DCPF>");
		len = e - b;
		funcs.append(temp1.substr(6,e-6));
		//allcontent = allcontent.substr(e+7);
	}
	bodies.append(funcs);
	//bodies.append();
	funcdefs.append("std::string _"+file+"emittHTML();\n");
	bodies.append("std::string _"+file+"emittHTML()\n{\n");
	bodies.append("std::stringstream screen;\n");
	while(allcontent.find("<DCPB>")!=std::string::npos)
	{
		b = allcontent.find("<DCPB>") + 6;
		std::string ter(allcontent.substr(0,b-6));
		StringUtil::replaceAll(ter,"\n","");
		StringUtil::replaceAll(ter,"\"","\\\"");
		bodies.append("screen << \""+ter+"\";");
		e = allcontent.find("</DCPB>");
		len = e - b;
		bodies.append(allcontent.substr(b,len));
		allcontent = allcontent.substr(e+7);
	}
	std::string ter(allcontent.substr(0));
	StringUtil::replaceAll(ter,"\n","");
	StringUtil::replaceAll(ter,"\"","\\\"");
	bodies.append("screen << \""+ter+"\";\nstd::string scr;\nscr = screen.str();\n");
	//bodies.append("\nAfcUtil::writeTofile(\""+dir+"_"+file+".html\",scr,true);\n");
	bodies.append("\nreturn scr;\n");
	bodies.append("\n}\n");
	//bodies.append("}\n");
	return bodies;
}


