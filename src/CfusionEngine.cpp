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
 * CfusionEngine.cpp
 *
 *  Created on: Nov 12, 2009
 *      Author: sumeet
 */

#include "CfusionEngine.h"

CfusionEngine::CfusionEngine() {
	// TODO Auto-generated constructor stub

}

CfusionEngine::~CfusionEngine() {
	// TODO Auto-generated destructor stub
}

void CfusionEngine::execute(string file)
{
	string codeh,codem,pub,pri,pro;
	string path = file.substr(0,file.find_last_of("/")+1);
	XmlParser parser("Parser");
	Element root = parser.getDocument(file).getRootElement();
	ElementList eles = root.getChildElements();
	string className = root.getAttribute("name");
	boost::to_upper(className);
	codeh += ("#ifndef "+className+"_H_\n");
	codeh += ("#define "+className+"_H_\n");
	codem += ("#include \""+root.getAttribute("name")+".h\"\n\n");
	for (unsigned int var = 0; var < eles.size(); var++)
	{
		if(eles.at(var).getTagName()=="includes")
		{
			string inc = eles.at(var).getAttribute("names");
			strVec incs;
			boost::iter_split(incs, inc, boost::first_finder(";"));
			for (unsigned int i = 0; i < incs.size(); i++)
			{
				codeh += ("#include \"" + incs.at(i) + "\"\n");
			}
		}
		else if(eles.at(var).getTagName()=="using")
		{
			codeh += ("using namespace " + eles.at(var).getAttribute("ns") + ";\n"+"class " + root.getAttribute("name") + "{\n");
		}
		else if(eles.at(var).getTagName()=="constructor")
		{
			if(eles.at(var).getAttribute("access")=="public" || eles.at(var).getAttribute("access")=="")
			{
				pub += (root.getAttribute("name") + "("+eles.at(var).getAttribute("args")+");\n");
			}
			else if(eles.at(var).getAttribute("access")=="private")
			{
				pri += (root.getAttribute("name") + "("+eles.at(var).getAttribute("args")+");\n");
			}
			else if(eles.at(var).getAttribute("access")=="protected")
			{
				pro += (root.getAttribute("name") + "("+eles.at(var).getAttribute("args")+");\n");
			}
			//codeh += (root.getAttribute("name") + "("+eles.at(var).getAttribute("args")+");\n");
			codem += (root.getAttribute("name")+"::"+root.getAttribute("name")+"("+eles.at(var).getAttribute("args")+")\n{\n}\n");
		}
		else if(eles.at(var).getTagName()=="property")
		{
			if(eles.at(var).getAttribute("access")=="public" || eles.at(var).getAttribute("access")=="")
			{
				pub += (eles.at(var).getAttribute("type") + " " + eles.at(var).getAttribute("name"));
				/*if(eles.at(var).getAttribute("initVal")!="")
				{
					if(eles.at(var).getAttribute("type")!="string")
						pub += (" = " + eles.at(var).getAttribute("initVal"));
					else
						pub += (" = \"" + eles.at(var).getAttribute("initVal") + "\"");
				}*/
				pub += ";\n";
			}
			else if(eles.at(var).getAttribute("access")=="private")
			{
				pri += (eles.at(var).getAttribute("type") + " " + eles.at(var).getAttribute("name"));
				/*if(eles.at(var).getAttribute("initVal")!="")
				{
					if(eles.at(var).getAttribute("type")!="string")
						pri += (" = " + eles.at(var).getAttribute("initVal"));
					else
						pri += (" = \"" + eles.at(var).getAttribute("initVal") + "\"");
				}*/
				pri += ";\n";
			}
			else if(eles.at(var).getAttribute("access")=="protected")
			{
				pro += (eles.at(var).getAttribute("type") + " " + eles.at(var).getAttribute("name"));
				/*if(eles.at(var).getAttribute("initVal")!="")
				{
					if(eles.at(var).getAttribute("type")!="string")
						pro += (" = " + eles.at(var).getAttribute("initVal"));
					else
						pro += (" = \"" + eles.at(var).getAttribute("initVal") + "\"");
				}*/
				pro += ";\n";
			}
		}
		else if(eles.at(var).getTagName()=="member")
		{
			map<string,string> vars;
			if(eles.at(var).getAttribute("access")=="public" || eles.at(var).getAttribute("access")=="")
			{
				pub += (eles.at(var).getAttribute("return") + " " + eles.at(var).getAttribute("name")
						+ "(" + eles.at(var).getAttribute("args") + ");\n");
				codem += (eles.at(var).getAttribute("return") + " " + root.getAttribute("name")+"::"+
						  eles.at(var).getAttribute("name")+"("+eles.at(var).getAttribute("args")+")\n{\n");
			}
			else if(eles.at(var).getAttribute("access")=="private")
			{
				pri += (eles.at(var).getAttribute("return") + " " + eles.at(var).getAttribute("name")
						+ "(" + eles.at(var).getAttribute("args") + ");\n");
				codem += (eles.at(var).getAttribute("return") + " " + root.getAttribute("name")+"::"+
						  eles.at(var).getAttribute("name")+"("+eles.at(var).getAttribute("args")+")\n{\n");
			}
			else if(eles.at(var).getAttribute("access")=="protected")
			{
				pro += (eles.at(var).getAttribute("return") + " " + eles.at(var).getAttribute("name")
						+ "(" + eles.at(var).getAttribute("args") + ");\n");
				codem += (eles.at(var).getAttribute("return") + " " + root.getAttribute("name")+"::"+
						  eles.at(var).getAttribute("name")+"("+eles.at(var).getAttribute("args")+")\n{\n");
			}
			ElementList elesc = eles.at(var).getChildElements();
			for (unsigned int var1 = 0; var1 < elesc.size(); var1++)
			{
				if(elesc.at(var1).getTagName()=="new")
				{
					vars[elesc.at(var1).getAttribute("name")] = elesc.at(var1).getAttribute("type");
					codem += (elesc.at(var1).getAttribute("type")+ " "+elesc.at(var1).getAttribute("name"));
					if(elesc.at(var1).getAttribute("initVal")!="")
					{
						if(elesc.at(var1).getAttribute("type")!="string")
							codem += (" = " + elesc.at(var1).getAttribute("initVal"));
						else
							codem += (" = \"" + elesc.at(var1).getAttribute("initVal") + "\"");
					}
					else if(elesc.at(var1).getText()!="")
					{
						if(elesc.at(var1).getAttribute("type")!="string")
							codem += (" = " + elesc.at(var1).getText());
						else
							codem += (" = \"" + elesc.at(var1).getText() + "\"");
					}
					codem += ";\n";
				}
				else if(elesc.at(var1).getTagName()=="assign")
				{
					string clasprop = elesc.at(var1).getAttribute("name");
					boost::replace_first(clasprop,"this:","this->");
					if(elesc.at(var1).getText()!="")
					{
						codem += (clasprop);
						if(vars[elesc.at(var1).getAttribute("name")]!="string")
							codem += (" = " + elesc.at(var1).getText());
						else
							codem += (" = \"" + elesc.at(var1).getText() + "\"");
						codem += ";\n";
					}
					else
					{
						codem += (clasprop + " = ");
						if(elesc.at(var1).getChildElements().at(0).getTagName()=="value")
						{
							if(elesc.at(var1).getChildElements().at(0).getChildElements().size()==1)
							{
								Element ele = elesc.at(var1).getChildElements().at(0);
								if(ele.getTagName()=="call")
								{
									codem += (ele.getAttribute("name"));
									codem += ("("+ele.getChildElements().at(0).getText()+")");
								}
							}
							/*else
							{
								if(vars[elesc.at(var1).getAttribute("name")]!="string")
									codem += (elesce.at(var2).getText());
								else
									codem += ("\"" + elesce.at(var2).getText() + "\"");
							}*/
						}
						else if(elesc.at(var1).getChildElements().at(0).getTagName()=="var")
						{
							clasprop = elesc.at(var1).getChildElements().at(0).getAttribute("name");
							boost::replace_first(clasprop,"this:","this->");
							codem += (clasprop);
						}
						codem += ";\n";
					}
				}
				else if(elesc.at(var1).getTagName()=="assignAdd" && elesc.at(var1).getAttribute("name")!="")
				{
					string clasprop = elesc.at(var1).getAttribute("name");
					boost::replace_first(clasprop,"this:","this->");
					if(elesc.at(var1).getAttribute("rhs")!="" && elesc.at(var1).getAttribute("lhs")!="")
					{
						codem += (clasprop + " = " + elesc.at(var1).getAttribute("rhs") + " + " + elesc.at(var1).getAttribute("lhs") + ";\n");
					}
					else
					{
						codem += (clasprop + " = ");
						ElementList elesce = elesc.at(var1).getChildElements();
						for (unsigned int var2 = 0; var2 < elesce.size(); var2++)
						{
							if(elesce.at(var2).getTagName()=="value")
							{
								if(elesce.at(var2).getChildElements().size()==1)
								{
									if(elesce.at(var2).getChildElements().at(0).getTagName()=="call")
									{
										codem += (elesce.at(var2).getChildElements().at(0).getAttribute("name"));
										codem += ("("+elesce.at(var2).getChildElements().at(0).getChildElements().at(0).getText()+")");
									}
								}
								else
								{
									if(vars[elesc.at(var1).getAttribute("name")]!="string")
										codem += (elesce.at(var2).getText());
									else
										codem += ("\"" + elesce.at(var2).getText() + "\"");
								}
							}
							else if(elesce.at(var2).getTagName()=="var")
							{
								clasprop = elesce.at(var2).getAttribute("name");
								boost::replace_first(clasprop,"this:","this->");
								codem += (clasprop);
							}
							if(var2!=elesce.size()-1)
								codem += " + ";
						}
						codem += ";\n";
					}
				}
				else if(elesc.at(var1).getTagName()=="return")
				{
					string clasprop = elesc.at(var1).getAttribute("name");
					boost::replace_first(clasprop,"this:","this->");
					if(eles.at(var).getAttribute("return")=="string" && elesc.at(var1).getText()!="")
						codem += "return \""+elesc.at(var1).getText()+"\";\n";
					else if(clasprop!="")
						codem += "return "+clasprop+";\n";
					else if(elesc.at(var1).getText()!="")
						codem += "return "+elesc.at(var1).getText()+";\n";
				}
			}
			codem += "}\n";
		}
	}
	codeh += ("private:\n"+pri+"protected:\n"+pro+"public:\n"+pub+"};\n#endif");
	path = "/home/sumeet/workspace/inter/";
	AfcUtil::writeTofile((path+root.getAttribute("name")+".h"),codeh,true);
	AfcUtil::writeTofile((path+root.getAttribute("name")+".cpp"),codem,true);
}
