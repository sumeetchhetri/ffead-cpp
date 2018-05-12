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

std::string ApplicationUtil::buildAllApplications(const std::vector<std::string>& files, const std::vector<std::string>& apps)
{
	std::string headers = "#include \"fstream\"\n#include \"string\"\n#include \"HttpSession.h\"\n#include \"CastUtil.h\"\n\n";
	std::string code;
	std::string meth,methdef,vars;
	for (unsigned int var = 0; var < files.size(); ++var)
	{
		std::string path = files.at(var).substr(0,files.at(var).find_last_of("/")+1);
		std::string appName = apps.at(var).substr(0,apps.at(var).find_last_of("/"));

		XmlParser parser("Parser");
		Document doc;
		parser.readDocument(files.at(var), doc);
		const Element& root = doc.getRootElement();
		ElementList eles = root.getChildElements();
		if(eles.size()==0)
		{
			ConfigurationData::getInstance()->applicationFlowMap[appName] = false;
		}
		else
		{
			meth += "std::string "+appName+"checkRules(std::string to,HttpSession session)\n{\nstd::string curr=session.getAttribute(\"CURR\");\n";
			for (unsigned int var = 0; var < eles.size(); var++)
			{
				if(eles.at(var).getTagName()=="include")
				{
					std::string inc = eles.at(var).getText();
					strVec incs;
					StringUtil::split(incs, inc, (" "));
					for (unsigned int i = 0; i < incs.size(); i++)
					{
						headers += ("#include \"" + incs.at(i) + "\"\n");
					}
				}
				else if(eles.at(var).getTagName()=="page" || eles.at(var).getTagName()=="welcome")
				{
					meth += ("std::string " + eles.at(var).getAttribute("id") + " = \""+path+eles.at(var).getAttribute("path")+"\";\n");
					if(eles.at(var).getAttribute("who")!="" && eles.at(var).getAttribute("where")!="")
					{
						if(eles.at(var).getAttribute("where").find("FILE:")!=std::string::npos)
						{
							std::string fileName = eles.at(var).getAttribute("where");
							StringUtil::replaceFirst(fileName,"FILE:","");
							fileName = (path + fileName);
							meth += "std::string path;\nif(to=="+eles.at(var).getAttribute("id")+")\n{";
							meth += "std::string user = session.getAttribute(\"USER\");\n";
							meth += "std::string pass = session.getAttribute(\"PASS\");\n";
							meth += "ifstream f(\""+fileName+"\");\n";
							meth += "if(f.is_open())\n{\nstd::string temp;\nwhile(getline(f,temp,'\\n'))\n{\n";
							meth += "if(temp==(user+\" \"+pass))\n{\nf.close();path = "+eles.at(var).getAttribute("id");
							meth += ";break;}\n}\nf.close();\n}\nif(path==\"\")\npath=\"FAILED\";\n}\n";
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
							std::string call = elesc.at(var1).getAttribute("call");
							std::string clas = call.substr(0,call.find("."));
							std::string method = call.substr(call.find(".")+1);
							headers += ("#include \""+clas+".h\"\n");
							meth += clas + " _object;\n";
							std::string args;
							ElementList elesce = elesc.at(var1).getChildElements();
							for (unsigned int var2 = 0; var2 < elesce.size(); var2++)
							{
								meth += (elesce.at(var2).getTagName() + " _" + CastUtil::lexical_cast<std::string>(var2+1) + " = CastUtil::lexical_cast<"+elesce.at(var2).getTagName()+">(\""+elesce.at(var2).getText()+"\");\n");
								args += ("_"+CastUtil::lexical_cast<std::string>(var2+1));
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
			ConfigurationData::getInstance()->applicationFlowMap[appName] = true;
		}
	}
	code += (headers + "extern \"C\"\n{\n"+vars+meth+"}\n");
	return code;

}
