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
 * ApplicationUtil.cpp
 *
 *  Created on: Dec 30, 2009
 *      Author: sumeet
 */

#include "ApplicationUtil.h"

ApplicationUtil::ApplicationUtil()
{
}

ApplicationUtil::~ApplicationUtil() {
	// TODO Auto-generated destructor stub
}

string ApplicationUtil::buildAllApplications(vector<string> files,vector<string> apps,map<string,string> &appMap)
{
	string headers = "#include \"fstream\"\n#include \"string\"\n#include \"HttpSession.h\"\n#include <boost/lexical_cast.hpp>\nusing namespace std;\n";
	string code;
	string meth,methdef,vars;
	for (unsigned int var = 0; var < files.size(); ++var)
	{
		string path = files.at(var).substr(0,files.at(var).find_last_of("/")+1);
		string appName = apps.at(var).substr(0,apps.at(var).find_last_of("/"));

		XmlParser parser("Parser");
		Element root = parser.getDocument(files.at(var)).getRootElement();
		ElementList eles = root.getChildElements();
		if(eles.size()==0)
			appMap[appName] = "false";
		else
		{
			meth += "string "+appName+"checkRules(string to,HttpSession session)\n{\nstring curr=session.getAttribute(\"CURR\");\n";
			for (unsigned int var = 0; var < eles.size(); var++)
			{
				if(eles.at(var).getTagName()=="include")
				{
					string inc = eles.at(var).getText();
					strVec incs;
					boost::iter_split(incs, inc, boost::first_finder(" "));
					for (unsigned int i = 0; i < incs.size(); i++)
					{
						headers += ("#include \"" + incs.at(i) + "\"\n");
					}
				}
				else if(eles.at(var).getTagName()=="page" || eles.at(var).getTagName()=="welcome")
				{
					meth += ("string " + eles.at(var).getAttribute("id") + " = \""+path+eles.at(var).getAttribute("path")+"\";\n");
					if(eles.at(var).getAttribute("who")!="" && eles.at(var).getAttribute("where")!="")
					{
						if(eles.at(var).getAttribute("where").find("FILE:")!=string::npos)
						{
							string fileName = eles.at(var).getAttribute("where");
							boost::replace_first(fileName,"FILE:","");
							fileName = (path + fileName);
							meth += "string path;\nif(to=="+eles.at(var).getAttribute("id")+")\n{";
							meth += "string user = session.getAttribute(\"USER\");\n";
							meth += "string pass = session.getAttribute(\"PASS\");\n";
							meth += "ifstream f(\""+fileName+"\");\n";
							meth += "if(f.is_open())\n{\nstring temp;\nwhile(getline(f,temp,'\\n'))\n{";
							meth += "if(temp==(user+\" \"+pass)){f.close();path = "+eles.at(var).getAttribute("id")+";break;}\n}}\nif(path==\"\")\npath=\"FAILED\";\n}";
						}
					}
					else if(eles.at(var).getAttribute("onsuccess")!="" && eles.at(var).getAttribute("onfail")!="")
					{

					}
				}
				else if(eles.at(var).getTagName()=="rule")
				{
					meth += "\nif(to=="+eles.at(var).getAttribute("topage")+" && curr=="+eles.at(var).getAttribute("currpage")+")\n{";
					ElementList elesc = eles.at(var).getChildElements();
					for (unsigned int var1 = 0; var1 < elesc.size(); var1++)
					{
						if(elesc.at(var1).getTagName()=="execute")
						{
							string call = elesc.at(var1).getAttribute("call");
							string clas = call.substr(0,call.find("."));
							string method = call.substr(call.find(".")+1);
							headers += ("#include \""+clas+".h\"\n");
							meth += clas + " _object;\n";
							string args;
							ElementList elesce = elesc.at(var1).getChildElements();
							for (unsigned int var2 = 0; var2 < elesce.size(); var2++)
							{
								meth += (elesce.at(var2).getTagName() + " _" + boost::lexical_cast<string>(var2+1) + " = boost::lexical_cast<"+elesce.at(var2).getTagName()+">(\""+elesce.at(var2).getText()+"\");\n");
								args += ("_"+boost::lexical_cast<string>(var2+1));
								if(var2!=elesce.size()-1)
									args += ",";
							}
							if(elesc.at(var1).getAttribute("rettype")!="" || elesc.at(var1).getAttribute("rettype")!="void")
							{
								meth += (elesc.at(var1).getAttribute("rettype") + " " + elesc.at(var1).getAttribute("retname") + " = ");
								meth += "_object."+method+"("+args+");\n";
							}
						}
					}
					meth += "path=to;\n}\n";
				}
			}
			meth += "return path;}\n";
			code += methdef;
		}
	}
	code += (headers + "extern \"C\"\n{\n"+vars+meth+"}\n");
	cout << code <<flush;
	return code;

}
