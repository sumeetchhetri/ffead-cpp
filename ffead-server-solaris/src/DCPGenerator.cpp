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
 * DCPGenerator.cpp
 *
 *  Created on: Sep 5, 2009
 *      Author: sumeet
 */

#include "DCPGenerator.h"


DCPGenerator::DCPGenerator() {
	// TODO Auto-generated constructor stub

}

DCPGenerator::~DCPGenerator() {
	// TODO Auto-generated destructor stub
}
string DCPGenerator::generateDCPAll(strVec fileNames)
{
	string bodies,headersb="#include \"AfcUtil.h\"",funcdefs;
	for (unsigned int var = 0; var < fileNames.size(); ++var)
	{
		bodies += generateDCP(fileNames.at(var),headersb,funcdefs);
	}
	bodies = (headersb+"\nextern \"C\"\n{\n"+funcdefs+bodies+"}\n");
	return bodies;
}
string DCPGenerator::generateDCP(string fileName,string &headersb,string &funcdefs)
{
	ifstream infile;
	string data,allcontent;
	infile.open(fileName.c_str());
	string file,dir;
	int s,en;
	s = fileName.find_last_of("/")+1;
	dir = fileName.substr(0,s-1);
	en = fileName.find_last_of(".");
	file = fileName.substr(s,en-s);
	if(infile)
	{
		while(getline(infile, data))
		{
			if(data.find("<import>")!=string::npos && data.find("</import>")!=string::npos)
			{
				int s = data.find("<import>")+8;
				int e = data.find("</import>");
				data=data.substr(s,e-s);
				s = data.find_last_of("/")+1;
				en = data.find_last_of(".");
				string file1 = data.substr(s,en-s);
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
	string header,bodies,funcs;
	int b = allcontent.find("<DCPH>") + 6;
	int e = allcontent.find("</DCPH>");
	int len = e - b;
	if(len>0)
	{
		header.append(allcontent.substr(b,len));
		allcontent = allcontent.substr(e+7);
		headersb.append(header+"\n");
	}

	while(allcontent.find("<DCPF>")!=string::npos)
	{
		b = allcontent.find("<DCPF>");
		e = allcontent.find("</DCPF>") + 7;
		string temp1 = allcontent.substr(b,e-b);
		boost::replace_all(allcontent,temp1,"");

		//string ter(allcontent.substr(0,b-6));
		//boost::replace_all(ter,"\n","");
		//boost::replace_all(ter,"\"","\\\"");
		e = temp1.find("</DCPF>");
		len = e - b;
		funcs.append(temp1.substr(6,e-6));
		//allcontent = allcontent.substr(e+7);
	}
	bodies.append(funcs);
	//bodies.append();
	funcdefs.append("string _"+file+"emittHTML();\n");
	bodies.append("string _"+file+"emittHTML()\n{\n");
	bodies.append("stringstream screen;\n");
	while(allcontent.find("<DCPB>")!=string::npos)
	{
		b = allcontent.find("<DCPB>") + 6;
		string ter(allcontent.substr(0,b-6));
		boost::replace_all(ter,"\n","");
		boost::replace_all(ter,"\"","\\\"");
		bodies.append("screen << \""+ter+"\";");
		e = allcontent.find("</DCPB>");
		len = e - b;
		bodies.append(allcontent.substr(b,len));
		allcontent = allcontent.substr(e+7);
	}
	string ter(allcontent.substr(0));
	boost::replace_all(ter,"\n","");
	boost::replace_all(ter,"\"","\\\"");
	bodies.append("screen << \""+ter+"\";\nstring scr;\nscr = screen.str();\n");
	bodies.append("\nAfcUtil::writeTofile(\""+dir+"_"+file+".html\",scr,true);\n");
	bodies.append("\nreturn scr;\n");
	bodies.append("\n}\n");
	//bodies.append("}\n");
	return bodies;
}


