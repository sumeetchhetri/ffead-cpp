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
 * Reflection.cpp
 *
 *  Created on: Aug 21, 2009
 *      Author: sumeet
 */

#include "Reflection.h"

using namespace std;
Reflection::Reflection() {
	// TODO Auto-generated constructor stub

}

Reflection::~Reflection() {
	// TODO Auto-generated destructor stub
}

strVec Reflection::list(string cwd)
{
	FILE *pipe_fp;
	string command;
	strVec files;
	command = "ls -F1 "+cwd+"|grep '.h'";
	cout << "\nCommand:" << command << flush;
	if ((pipe_fp = popen(command.c_str(), "r")) == NULL)
	{
		printf("pipe open error in cmd_list\n");
		return files;
	}
	int t_char;
	string fileName;
	while ((t_char = fgetc(pipe_fp)) != EOF)
	{
		if(t_char!='\n')
		{
			stringstream ss;
			ss << (char)t_char;
			string temp;
			ss >> temp;
			fileName.append(temp);
		}
		else if(fileName!="")
		{
			boost::replace_first(fileName,"*","");
			if(fileName.find("~")==string::npos)
			{
				files.push_back(cwd+"/"+fileName);
				cout << "\nlist for file" << (cwd+"/"+fileName) << "\n" << flush;
			}
			fileName = "";
		}
	}
	pclose(pipe_fp);
	return files;
}

void trimSpaces(string& str)
{
    // Trim Both leading and trailing spaces
    size_t startpos = str.find_first_not_of(" \t"); // Find the first character position after excluding leading blank spaces
    size_t endpos = str.find_last_not_of(" \t"); // Find the first character position from reverse af

     // if all spaces or empty return an empty string
     if(( string::npos == startpos ) || ( string::npos == endpos))
     {
         str = "";
     }
    else
         str = str.substr( startpos, endpos-startpos+1 );

}

void Reflection::collectInfo(string data,string flag)
{
	trimSpaces(data);
	if(flag=="public")
		this->pub.push_back(data);
	else if(flag=="protected")
		this->pro.push_back(data);
	else if(flag=="private")
		this->pri.push_back(data);
}

bool Reflection::generateClassInfo(string className)
{
	this->pub.clear();this->pri.clear();this->pro.clear();
	this->classN="";this->baseClassN="";this->bcvisib="";
	string data;
	//className += ".h";
	ifstream infile;
	cout << "Reading from the file" << endl;
	//className = "/home/sumeet/workspace/weblib/" + className;
	infile.open(className.c_str());
	string flag = "";
	boost::regex e1,e2;

	if(infile.is_open())
	{
		bool classdone = false;
		bool start = false;
		bool classset = false;
		bool commstrts = false;
		size_t tes;
		int cnt = 0;

		//e1.assign("\\t+");
		e2.assign("\\s+");
		while(getline(infile, data))
		{
			//data = boost::regex_replace(data, e1, "", boost::match_default | boost::format_all);
			data = boost::regex_replace(data, e2, " ", boost::match_default | boost::format_all);
			classset = false;
			if((tes=data.find("/*"))!=string::npos)
			{
				commstrts = true;
				if((tes=data.find("*/"))!=string::npos)
					commstrts = false;
			}
			else if((tes=data.find("*/"))!=string::npos)
			{
				commstrts = false;
			}
			else if((tes=data.find("//"))!=string::npos)
			{
				commstrts = false;
			}
			else if(!commstrts)
			{
				if((tes=data.find("template"))!=string::npos)
					return false;
				if(data.find("friend")!=string::npos)
					continue;
				if((tes=data.find("class"))!=string::npos && !classdone)
				{
					strVec results;
					boost::replace_first(data,":","");
					boost::replace_first(data,"class ","");
					if((tes=data.find("{"))!=string::npos)
					{
						start = true;
						cnt += 1;
					}
					boost::replace_first(data,"{","");
					boost::replace_first(data,":","");
					boost::iter_split(results, data, boost::first_finder(" "));
					this->classN = results.at(0);
					boost::replace_all(this->classN," ","");
					if(results.size()==3)
					{
						this->bcvisib = results.at(1);
						boost::replace_all(this->bcvisib," ","");
						this->baseClassN = results.at(2);
						boost::replace_all(this->baseClassN," ","");
						//cout << results.size() << flush;
					}
					classdone = true;
					classset = true;
					//boost::iter_split(results, data, boost::first_finder(": "));
				}
				else if((tes=data.find("{"))!=string::npos && !start)
				{
					start = true;
					boost::replace_first(data,"{","");
					cnt += 1;
				}
				if(start && !classset && data!="")
				{
					if((tes=data.find("{"))!=string::npos)
					{
						cnt += 1;
					}
					else if((tes=data.find("}"))!=string::npos)
					{
						cnt -= 1;
					}
					else if(cnt==0)
						break;
					else if(cnt==1)
					{
						if((tes=data.find("public"))!=string::npos)
							flag = "public";
						else if((tes=data.find("protected"))!=string::npos)
							flag = "protected";
						else if((tes=data.find("private"))!=string::npos)
							flag = "private";
						else
						{
							if(flag=="")
								flag = "private";
							collectInfo(data,flag);
						}
					}
				}
			}
		}
	}
	infile.close();
	if(this->pub.size()>0 || this->pri.size()>0 || this->pro.size()>0)
		return true;
	else
		return false;
}


bool Reflection::generateClassInfoFromDD(string alldata)
{
	string data;
	string flag = "";
	stringstream ss;
	ss << alldata;
	while (getline(ss, data))
	{
		size_t tes;
		if ((tes = data.find("class")) != string::npos)
		{
			strVec results;
			boost::replace_first(data,":","");
			boost::iter_split(results, data, boost::first_finder(" "));
			this->classN = results.at(1);
			if(results.size()>3)
			{
				this->bcvisib = results.at(2);
				this->baseClassN = results.at(3);
				//cout << results.size() << flush;
			}
			//boost::iter_split(results, data, boost::first_finder(": "));
		}
		else if ((tes = data.find("}")) != string::npos)
			break;
		else if ((tes = data.find("public")) != string::npos)
			flag = "public";
		else if ((tes = data.find("protected")) != string::npos)
			flag = "protected";
		else if ((tes = data.find("private")) != string::npos)
			flag = "private";
		else
		{
			if (flag == "" && data != "{")
				flag = "private";
			collectInfo(data, flag);
		}
	}
	//cout << pub.size() << pri.size() << pro.size() << flush;
	if (this->pub.size() > 0 || this->pri.size() > 0 || this->pro.size() > 0)
		return true;
	else
		return false;
}

string Reflection::updateClassDefinition(string className, bool file)
{
	string refDef,typedefs;
	if (file && !generateClassInfo(className))
	{
		return refDef;
	}
	else if (!file && !generateClassInfoFromDD(className))
	{
		return refDef;
	}
	typedefs = "#include \"ClassInfo.h\"\n#include \"string\"\n#include \"Method.h\"\n#include \"Field.h\"\n";
	typedefs += "#include \"" + this->classN + ".h\"\n";
	string structinf = "extern \"C\"\n{\nstruct struct"+this->classN+"{\n";
	refDef += "ClassInfo get" + this->classN + "()\n{\nClassInfo classInfo;";
	refDef += ("\nclassInfo.setClassName(\"" + this->classN + "\");");
	refDef += ("\nclassInfo.setInstance(new " + this->classN + ");");
	refDef += ("\nclassInfo.setBase(\"" + this->bcvisib + " " + this->baseClassN + "\");");
	refDef += ("\nvector<Method> methVec;\nvector<Field> fldVec;\nMethod me;\nField f;\nmethMap meths;\n");
	refDef += ("args argu;\n");
	string publf, privf, protf ,publm, privm, protm;
	string meth,fld;
	size_t tes;
	if (this->pub.size() > 0)
	{
		for (unsigned int i = 0; i < this->pub.size(); i++)
		{
			if((tes=this->pub.at(i).find("("))!=string::npos && (tes=this->pub.at(i).find(")"))!=string::npos && this->pub.at(i).find("~")==string::npos)
			{
				refDef += ("me.clear();\n");
				publm += this->pub.at(i);
				meth = this->pub.at(i);
				boost::replace_first(meth,";","");
				boost::replace_first(meth,"("," ");
				boost::replace_all(meth,","," ");
				boost::replace_first(meth,")"," ");
				strVec methp,methpm;
				boost::iter_split(methp, meth, boost::first_finder(" "));
				for(unsigned int j = 0; j < methp.size(); j++)
				{
					if(methp.at(j)!="")
						methpm.push_back(methp.at(j));
				}
				for(unsigned int j = 0; j < methpm.size(); j++)
				{
					if(j==0)
					{
						if(methpm.at(0)==this->classN)
						{
							refDef += ("me.setReturnType(\"Constructor\");\n");
							refDef += ("me.setMethodName(\""+methpm.at(j)+"\");\n");
						}
						else
						{
							refDef += ("me.setReturnType(\""+methpm.at(j)+"\");\n");
							//typedefs += ("typedef " + methpm.at(j) + " ");
						}
					}
					else if(j==1 && methpm.at(0)!=this->classN)
					{
						refDef += ("me.setMethodName(\""+methpm.at(j)+"\");\n");
						//refDef += (this->classN + methpm.at(j)+" = &"+this->classN+"::"+methpm.at(j)+";\n");
						//typedefs += ("("+this->classN+"::*" + this->classN + methpm.at(j) + ") (");
					}
					else if(methpm.at(j)!="")
					{
						refDef += ("argu.push_back(\""+methpm.at(j)+"\");\n");
						/*typedefs += methpm.at(j);
						if(j!=methpm.size()-1)
							typedefs += ",";*/
					}
				}
				/*if(methpm.at(0)!=this->classN)
					typedefs += ");\n";*/
				refDef += ("me.setArgumentTypes(argu);\n");
				refDef += ("if(me.getMethodName()!=\"\")\n{\nmethVec.push_back(me);\n}\n");
			}
			else if(this->pub.at(i).find("~")==string::npos)
			{
				refDef += ("f.clear();\n");
				publf += this->pub.at(i);
				fld = this->pub.at(i);
				boost::replace_first(fld,";","");
				strVec fldp;
				boost::iter_split(fldp, fld, boost::first_finder(" "));
				for(unsigned int j = 0; j < fldp.size(); j++)
				{
					if(j==0)
					{
						refDef += ("f.setType(\""+fldp.at(j)+"\");\n");
					}
					else if(j==1)
					{
						refDef += ("f.setFieldName(\""+fldp.at(j)+"\");\n");
					}
				}
				if(fldp.size()==2)
					structinf += (fldp.at(0)+" "+fldp.at(1)+";\n");
				refDef += ("if(f.getFieldName()!=\"\")\n{\nfldVec.push_back(f);\n}\n");
			}
		}
	}
	if (this->pri.size() > 0)
	{
		for (unsigned int i = 0; i < this->pri.size(); i++)
		{
			if((tes=this->pri.at(i).find("("))!=string::npos && (tes=this->pri.at(i).find(")"))!=string::npos && this->pri.at(i).find("~")==string::npos)
			{
				refDef += ("me.clear();\n");
				privm += this->pri.at(i);
				meth = this->pri.at(i);
				boost::replace_first(meth,";","");
				boost::replace_first(meth,"("," ");
				boost::replace_all(meth,","," ");
				boost::replace_first(meth,")"," ");
				strVec methp;
				boost::iter_split(methp, meth, boost::first_finder(" "));
				for(unsigned int j = 0; j < methp.size(); j++)
				{
					if(j==0)
					{
						if(methp.at(0)==this->classN)
						{
							refDef += ("me.setReturnType(\"Constructor\");\n");
							refDef += ("me.setMethodName(\""+methp.at(j)+"\");\n");
						}
						else
							refDef += ("me.setReturnType(\""+methp.at(j)+"\");\n");
					}
					else if(j==1 && methp.at(0)!=this->classN)
					{
						refDef += ("me.setMethodName(\""+methp.at(j)+"\");\n");
					}
					else if(methp.at(j)!="")
					{
						refDef += ("argu.push_back(\""+methp.at(j)+"\");\n");
					}
				}
				refDef += ("me.setArgumentTypes(argu);\n");
				refDef += ("if(me.getMethodName()!=\"\")\n{\nmethVec.push_back(me);\n}\n");
			}
			else if(this->pri.at(i).find("~")==string::npos)
			{
				refDef += ("f.clear();\n");
				privf += this->pri.at(i);
				fld = this->pri.at(i);
				boost::replace_first(fld,";","");
				strVec fldp;
				boost::iter_split(fldp, fld, boost::first_finder(" "));
				for(unsigned int j = 0; j < fldp.size(); j++)
				{
					if(j==0)
					{
						refDef += ("f.setType(\""+fldp.at(j)+"\");\n");
					}
					else if(j==1)
					{
						refDef += ("f.setFieldName(\""+fldp.at(j)+"\");\n");
					}
				}
				if(fldp.size()==2)
					structinf += (fldp.at(0)+" "+fldp.at(1)+";\n");
				refDef += ("if(f.getFieldName()!=\"\")\n{\nfldVec.push_back(f);\n}\n");
			}
		}
	}
	if (this->pro.size() > 0)
	{
		for (unsigned int i = 0; i < this->pro.size(); i++)
		{
			if((tes=this->pro.at(i).find("("))!=string::npos && (tes=this->pro.at(i).find(")"))!=string::npos && this->pro.at(i).find("~")==string::npos)
			{
				refDef += ("me.clear();\n");
				protm += this->pro.at(i);
				meth = this->pro.at(i);
				boost::replace_first(meth,";","");
				boost::replace_first(meth,"("," ");
				boost::replace_all(meth,","," ");
				boost::replace_first(meth,")"," ");
				strVec methp;
				boost::iter_split(methp, meth, boost::first_finder(" "));
				for(unsigned int j = 0; j < methp.size(); j++)
				{
					if(j==0)
					{
						if(methp.at(0)==this->classN)
						{
							refDef += ("me.setReturnType(\"Constructor\");\n");
							refDef += ("me.setMethodName(\""+methp.at(j)+"\");\n");
						}
						else
							refDef += ("me.setReturnType(\""+methp.at(j)+"\");\n");
					}
					else if(j==1 && methp.at(0)!=this->classN)
					{
						refDef += ("me.setMethodName(\""+methp.at(j)+"\");\n");
					}
					else if(methp.at(j)!="")
					{
						refDef += ("argu.push_back(\""+methp.at(j)+"\");\n");
					}
				}
				refDef += ("me.setArgumentTypes(argu);\n");
				refDef += ("if(me.getMethodName()!=\"\")\n{\nmethVec.push_back(me);\n}\n");
			}
			else if(this->pro.at(i).find("~")==string::npos)
			{
				refDef += ("f.clear();\n");
				protf += this->pro.at(i);
				fld = this->pro.at(i);
				boost::replace_first(fld,";","");
				strVec fldp;
				boost::iter_split(fldp, fld, boost::first_finder(" "));
				for(unsigned int j = 0; j < fldp.size(); j++)
				{
					if(j==0)
					{
						refDef += ("f.setType(\""+fldp.at(j)+"\");\n");
					}
					else if(j==1)
					{
						refDef += ("f.setFieldName(\""+fldp.at(j)+"\");\n");
					}
				}
				if(fldp.size()==2)
					structinf += (fldp.at(0)+" "+fldp.at(1)+";\n");
				refDef += ("if(f.getFieldName()!=\"\")\n{\nfldVec.push_back(f);\n}\n");
			}
		}
	}
	//refDef += ("\nclassInfo.setMeths(meths);");
	//refDef += ("\nclassInfo.setMethods(methVec);");
	//refDef += ("\nclassInfo.setFields(fldVec);");
	refDef += "\nreturn classInfo;\n}\n";
	refDef += "}";
	structinf += "};\n";
	refDef = (typedefs + structinf + refDef);
	return refDef;
}

string Reflection::updateTemplateContextDefinition(string className, bool file)
{
	string refDef;
	if (file && !generateClassInfo(className))
	{
		return refDef;
	}
	else if (!file && !generateClassInfoFromDD(className))
	{
		return refDef;
	}
	refDef = "#include \"ClassInfo.h\"\n#include \"string\"\n";
	refDef += "#include \"" + this->classN + ".h\"\n\n";
	refDef += "extern \"C\"\n{\nClassInfo get" + this->classN + "()\n{\nClassInfo classInfo;";
	refDef += ("\nclassInfo.setInstance(new " + this->classN + ");");
	refDef += "\nreturn classInfo;\n}\n}";
	return refDef;
}


propMap Reflection::getDbTableInfo(string file)
{
	propMap tabInfo;
	string temp;
	strVec all;
	ifstream infile;
	infile.open(file.c_str());
	if(infile)
	{
		while(getline(infile,temp))
		{
			if(temp!="")
				all.push_back(temp);
		}
		for(unsigned int i=0;i<(all.size()/2);i++)
		{
			tabInfo[all.at(i)] = all.at(i+1);
		}
	}
	return tabInfo;
}

strVec Reflection::getAfcObjectData(string className,bool object,strVec& privf)
{
	strVec refDef;
	if (!generateClassInfo(className))
	{
		return refDef;
	}
	strVec publf, protf ,publm, privm, protm;

	size_t tes;
	if (this->pub.size() > 0)
	{
		for (unsigned int i = 0; i < this->pub.size(); i++)
		{
			if((tes=this->pub.at(i).find("("))!=string::npos && (tes=this->pub.at(i).find(")"))!=string::npos)
			{
				publm.push_back(this->pub.at(i));
			}
			else
			{
				publf.push_back(this->pub.at(i));
			}
		}
	}
	if (this->pri.size() > 0)
	{
		for (unsigned int i = 0; i < this->pri.size(); i++)
		{
			if((tes=this->pri.at(i).find("("))!=string::npos && (tes=this->pri.at(i).find(")"))!=string::npos)
			{
				privm.push_back(this->pri.at(i));
			}
			else
			{
				privf.push_back(this->pri.at(i));
			}
		}
	}
	if (this->pro.size() > 0)
	{
		for (unsigned int i = 0; i < this->pro.size(); i++)
		{
			if((tes=this->pro.at(i).find("("))!=string::npos && (tes=this->pro.at(i).find(")"))!=string::npos)
			{
				protm.push_back(this->pro.at(i));
			}
		}
	}
	if(!object)
	{
		return publm;
	}
	else
	{
		return publf;
	}
}

strVec Reflection::getAfcObjectData(string className,bool object)
{
	strVec refDef;
	if (!generateClassInfo(className))
	{
		return refDef;
	}
	strVec publf, privf, protf ,publm, privm, protm;

	size_t tes;
	if (this->pub.size() > 0)
	{
		for (unsigned int i = 0; i < this->pub.size(); i++)
		{
			if((tes=this->pub.at(i).find("("))!=string::npos && (tes=this->pub.at(i).find(")"))!=string::npos)
			{
				publm.push_back(this->pub.at(i));
			}
			else
			{
				publf.push_back(this->pub.at(i));
			}
		}
	}
	if (this->pri.size() > 0)
	{
		for (unsigned int i = 0; i < this->pri.size(); i++)
		{
			if((tes=this->pri.at(i).find("("))!=string::npos && (tes=this->pri.at(i).find(")"))!=string::npos)
			{
				privm.push_back(this->pri.at(i));
			}
		}
	}
	if (this->pro.size() > 0)
	{
		for (unsigned int i = 0; i < this->pro.size(); i++)
		{
			if((tes=this->pro.at(i).find("("))!=string::npos && (tes=this->pro.at(i).find(")"))!=string::npos)
			{
				protm.push_back(this->pro.at(i));
			}
		}
	}
	if(!object)
	{
		return publm;
	}
	else
	{
		return publf;
	}
}

string Reflection::generateClassDefinitionsAll(strVec all,string &includeRef)
{
	string ret = "";
	//includeRef = "#ifndef REFLECTOR_H_\n#define REFLECTOR_H_\n#include \"ClassInfo.h\"\n#include \"string\"\n#include \"Method.h\"\n#include \"Field.h\"\n#include \"Trace.h\"\n";
	//includeRef += "#include \"XmlParser.h\"\n#include <stdio.h>\n#include <sys/wait.h>\n#include <stdexcept>\n#include <execinfo.h>\n#include <dlfcn.h>\n#include <cxxabi.h>\n#include <stdio.h>\n#include <stdlib.h>\n#include \"string\"\n#include <sstream>\n#include <typeinfo>\n";
	string typedefs,classes,methods,opers;
	string inc = "#include \"ClassInfo.h\"\n#include \"string\"\n#include \"Method.h\"\n#include \"Field.h\"\n";
	ret += "extern \"C\"\n{\n";
	for (unsigned int var = 0; var < all.size(); ++var)
	{
		cout << "\nstarting for classes " << all.size() << "\n" << flush;
		ret += this->generateClassDefinitions(all.at(var),inc,typedefs,classes,methods,opers);

	}
	/*ret += "ClassInfo Reflector::getClassInfo(string className)\n{\n";
	ret += classes;
	ret += "\n\treturn info;\n}\n";
	ret += "void* Reflector::invokeMethod(void* instance,Method method,vals values)\n{\n";
	ret += methods;
	ret += "\n\treturn returnValue;\n}\n";
	ret += "bool Reflector::instanceOf(void* instance,string className)\n{\n";
	ret += "Trace tr;\nstring cn = tr.getClassName(instance);\nif(cn==className)\nreturn true;\nelse\nreturn false;\n}\n";*/
	ret +=  methods+opers+ "\n}\n";
	ret = (inc+ret);
	return ret;
}

string Reflection::generateClassDefinitions(string includeDir,string &includesDefs,string &typedefs,string &classes,string &methods,string &opers)
{
	strVec includes = list(includeDir);
	string ret,in,ty,cl,me;
	for (unsigned int var = 0; var < includes.size(); ++var)
	{
		cout << "\ngenerating for file" << includes.at(var) << "\n" << flush;
		ret += generateClassDefinition(includes.at(var),includesDefs,typedefs,classes,methods,opers);
		cout << "\ndone generating for file" << includes.at(var) << "\n" << flush;
	}
	return ret;
}

string Reflection::generateClassDefinition(string className,string &includesDefs,string &typedefs,string &classes,string &methods,string &opers)
{
	string refDef;
	if (!generateClassInfo(className))
	{
		return refDef;
	}
	classes += "\tif(className==\""+this->classN+"\")\n\t\treturn get"+this->classN+"();\n";
	includesDefs += "#include \"" + this->classN + ".h\"\n";
	//string structinf = "\nstruct struct"+this->classN+"{\n";
	refDef += "ClassInfo getReflectionCIFor" + this->classN + "()\n{\nClassInfo classInfo;";
	refDef += ("\nclassInfo.setClassName(\"" + this->classN + "\");");
	//refDef += ("\nclassInfo.setInstance(new " + this->classN + ");");
	refDef += ("\nclassInfo.setBase(\"" + this->bcvisib + " " + this->baseClassN + "\");");
	refDef += ("\nConstructor ctor;\nMethod me;\nField f;\n");
	refDef += ("args argu;\n");
	string publf, privf, protf ,publm, privm, protm;
	string meth,fld;
	size_t tes;
	bool ctorisp = false,ddtorisp = false;;
	if (this->pub.size() > 0)
	{
		for (unsigned int i = 0; i < this->pub.size(); i++)
		{
			if((tes=this->pub.at(i).find("("))!=string::npos && (tes=this->pub.at(i).find(")"))!=string::npos && this->pub.at(i).find("~")==string::npos)
			{
				refDef += ("ctor.clear();\nme.clear();\n");
				publm += this->pub.at(i);
				meth = this->pub.at(i);
				boost::replace_first(meth,";","");

				string argts = meth.substr(meth.find("("),meth.find(")")-meth.find("("));
				boost::replace_first(argts,"(","");
				boost::replace_all(argts,")","");
				meth = meth.substr(0,meth.find("("));
				//boost::replace_first(meth,")"," ");
				strVec methp,methpm,argp,argpm;
				boost::iter_split(argp, argts, boost::first_finder(","));
				boost::iter_split(methp, meth, boost::first_finder(" "));
				for(unsigned int j = 0; j < methp.size(); j++)
				{
					if(methp.at(j)!="")
						methpm.push_back(methp.at(j));
				}
				for(unsigned int j = 0; j < argp.size(); j++)
				{
					if(argp.at(j)!="" && argp.at(j)!="(")
						argpm.push_back(argp.at(j));
				}
				string typdefName,methsd,valsd,valsa;
				//bool ctor = false;
				for(unsigned int j = 0; j < methpm.size(); j++)
				{
					if(j==0)
					{
						if(methpm.at(0)==this->classN)
						{
							refDef += ("ctor.setName(\""+this->classN+"\");\n");
							//refDef += ("me.setMethodName(\""+this->classN+methpm.at(j)+"\");\n");
							//ctor = true;
							methsd += (this->classN);
						}
						else if(meth.find(" operator")==string::npos)
						{
							refDef += ("me.setReturnType(\""+methpm.at(j)+"\");\n");
							typedefs += ("typedef " + methpm.at(j) + " ");
						}
					}
					else if(j==1 && methpm.at(0)!=this->classN && meth.find(" operator")==string::npos)
					{

						//refDef += (this->classN + methpm.at(j)+" = &"+this->classN+"::"+methpm.at(j)+";\n");
						typedefs += ("("+this->classN+"::*"+this->classN +methpm.at(j));
						methsd += (this->classN+methpm.at(j));
					}
					/*else if(methpm.at(j)!="")
					{
						refDef += ("argu.push_back(\""+methpm.at(j)+"\");\n");
						valsd += "\t\t"+(methpm.at(j) + " *_" + boost::lexical_cast<string>(j-1)+" = ("+methpm.at(j)+"*)values.at("+boost::lexical_cast<string>(j-2)+");");
						valsa += "*_" + boost::lexical_cast<string>(j-1);
						if(methpm.at(0)!=this->classN)
						{
							typedefs += methpm.at(j);
							typdefName += methpm.at(j);
							methsd += methpm.at(j);
							if(j!=methpm.size()-1)
							{
								typdefName += ",";
								valsa += ",";
							}
						}
					}*/
				}
				for(unsigned int j = 0; j < argpm.size(); j++)
				{
					strVec argtn;
					string type12 = "";
					if(argpm.at(j).find("*")!=string::npos)
						type12 = "*";
					else if(argpm.at(j).find("&")!=string::npos)
						type12 = "&";
					boost::iter_split(argtn, argpm.at(j), boost::first_finder(" "));
					boost::replace_all(argtn.at(0)," ","");
					if(meth.find(" operator")==string::npos)refDef += ("argu.push_back(\""+argtn.at(0)+"\");\n");
					if(argtn.at(0).find("*")!=string::npos || type12=="*")
					{
						boost::replace_all(argtn.at(0),"*","");
						valsd += "\t\t"+(argtn.at(0) + " *_" + boost::lexical_cast<string>(j)+" = ("+argtn.at(0)+"*)values.at("+boost::lexical_cast<string>(j)+");");
					}
					else if(argtn.at(0).find("&")!=string::npos || type12=="&")
					{
						boost::replace_all(argtn.at(0),"&","");
						valsd += "\t\t"+(argtn.at(0) + " *_" + boost::lexical_cast<string>(j)+" = ("+argtn.at(0)+"*)values.at("+boost::lexical_cast<string>(j)+");");
					}
					else
						valsd += "\t\t"+(argtn.at(0) + " *_" + boost::lexical_cast<string>(j)+" = ("+argtn.at(0)+"*)values.at("+boost::lexical_cast<string>(j)+");");
					if(type12=="*")
						valsa += "_" + boost::lexical_cast<string>(j);
					else
						valsa += "*_" + boost::lexical_cast<string>(j);
					//if(methpm.at(0)!=this->classN)
					//{
						typedefs += argtn.at(0);
						typdefName += argtn.at(0);
						methsd += argtn.at(0);
						if(j!=argpm.size()-1)
						{
							typdefName += ",";
							valsa += ",";
						}
					//}
				}
				if(meth.find(" operator")!=string::npos)
				{
					if(meth.find("<")!=string::npos)
					{
						opers += "\nvoid* operator"+this->classN+"LT(void* instance,vals values)\n{\n\t"+this->classN+" *_obj = ("+this->classN+"*)instance;\n";
						if(methpm.at(0)=="void")
						{
							opers += valsd;
							opers += "\n\tvoid* returnValue=NULL;\n\t*_obj<"+valsa+";";
							opers += "\n\treturn returnValue;";
						}
						else
						{
							opers += "\n\t"+methpm.at(0)+" *_retVal=new "+methpm.at(0)+";\n";
							opers += valsd;
							opers += "\n\t*_retVal = (*_obj<"+valsa+");";
							opers += "\n\treturn _retVal;";
						}
						opers += "\n}";
					}
					else if(meth.find(">")!=string::npos)
					{
						opers += "\nvoid* operator"+this->classN+"GT(void* instance,vals values)\n{\n\t"+this->classN+" *_obj = ("+this->classN+"*)instance;\n";
						if(methpm.at(0)=="void")
						{
							opers += valsd;
							opers += "\n\tvoid* returnValue=NULL;\n\t*_obj>"+valsa+";";
							opers += "\n\treturn returnValue;";
						}
						else
						{
							opers += "\n\t"+methpm.at(0)+" *_retVal=new "+methpm.at(0)+";\n";
							opers += valsd;
							opers += "\n\t*_retVal = (*_obj>"+valsa+");";
							opers += "\n\treturn _retVal;";
						}
						opers += "\n}";
					}
					else if(meth.find("==")!=string::npos)
					{
						opers += "\nvoid* operator"+this->classN+"EQ(void* instance,vals values)\n{\n\t"+this->classN+" *_obj = ("+this->classN+"*)instance;\n";
						if(methpm.at(0)=="void")
						{
							opers += valsd;
							opers += "\n\tvoid* returnValue=NULL;\n\t*_obj=="+valsa+";";
							opers += "\n\treturn returnValue;";
						}
						else
						{
							opers += "\n\t"+methpm.at(0)+" *_retVal=new "+methpm.at(0)+";\n";
							opers += valsd;
							opers += "\n\t*_retVal = (*_obj=="+valsa+");";
							opers += "\n\treturn _retVal;";
						}
						opers += "\n}";
					}
					else if(meth.find("!=")!=string::npos)
					{
						opers += "\nvoid* operator"+this->classN+"NE(void* instance,vals values)\n{\n\t"+this->classN+" *_obj = ("+this->classN+"*)instance;\n";
						if(methpm.at(0)=="void")
						{
							opers += valsd;
							opers += "\n\tvoid* returnValue=NULL;\n\t*_obj!="+valsa+";";
							opers += "\n\treturn returnValue;";
						}
						else
						{
							opers += "\n\t"+methpm.at(0)+" *_retVal=new "+methpm.at(0)+";\n";
							opers += valsd;
							opers += "\n\t*_retVal = (*_obj!="+valsa+");";
							opers += "\n\treturn _retVal;";
						}
						opers += "\n}";
					}
					else if(meth.find("<=")!=string::npos)
					{
						opers += "\nvoid* operator"+this->classN+"LE(void* instance,vals values)\n{\n\t"+this->classN+" *_obj = ("+this->classN+"*)instance;\n";
						if(methpm.at(0)=="void")
						{
							opers += valsd;
							opers += "\n\tvoid* returnValue=NULL;\n\t*_obj<="+valsa+";";
							opers += "\n\treturn returnValue;";
						}
						else
						{
							opers += "\n\t"+methpm.at(0)+" *_retVal=new "+methpm.at(0)+";\n";
							opers += valsd;
							opers += "\n\t*_retVal = (*_obj<="+valsa+");";
							opers += "\n\treturn _retVal;";
						}
						opers += "\n}";
					}
					else if(meth.find(">=")!=string::npos)
					{
						opers += "\nvoid* operator"+this->classN+"GT(void* instance,vals values)\n{\n\t"+this->classN+" *_obj = ("+this->classN+"*)instance;\n";
						if(methpm.at(0)=="void")
						{
							opers += valsd;
							opers += "\n\tvoid* returnValue=NULL;\n\t*_obj>="+valsa+";";
							opers += "\n\treturn returnValue;";
						}
						else
						{
							opers += "\n\t"+methpm.at(0)+" *_retVal=new "+methpm.at(0)+";\n";
							opers += valsd;
							opers += "\n\t*_retVal = (*_obj>="+valsa+");";
							opers += "\n\treturn _retVal;";
						}
						opers += "\n}";
					}
					else if(meth.find("!")!=string::npos)
					{
						opers += "\nvoid* operator"+this->classN+"NT(void* instance,vals values)\n{\n\t"+this->classN+" *_obj = ("+this->classN+"*)instance;\n";
						if(methpm.at(0)=="void")
						{
							opers += valsd;
							opers += "\n\tvoid* returnValue=NULL;\n\t!*_obj;";
							opers += "\n\treturn returnValue;";
						}
						else
						{
							opers += "\n\t"+methpm.at(0)+" *_retVal=new "+methpm.at(0)+";\n";
							opers += valsd;
							opers += "\n\t*_retVal = (!*_obj);";
							opers += "\n\treturn _retVal;";
						}
						opers += "\n}";
					}
					else if(meth.find("<<")!=string::npos)
					{

					}
					else if(meth.find(">>")!=string::npos)
					{

					}
					else if(meth.find("+")!=string::npos)
					{
						opers += "\nvoid* operator"+this->classN+"AD(void* instance,vals values)\n{\n\t"+this->classN+" *_obj = ("+this->classN+"*)instance;\n";
						if(methpm.at(0)=="void")
						{
							opers += valsd;
							opers += "\n\tvoid* returnValue=NULL;\n\t*_obj+"+valsa+";";
							opers += "\n\treturn returnValue;";
						}
						else
						{
							opers += "\n\t"+methpm.at(0)+" *_retVal=new "+methpm.at(0)+";\n";
							opers += valsd;
							opers += "\n\t*_retVal = (*_obj+"+valsa+");";
							opers += "\n\treturn _retVal;";
						}
						opers += "\n}";
					}
					else if(meth.find("-")!=string::npos)
					{
						opers += "\nvoid* operator"+this->classN+"SU(void* instance,vals values)\n{\n\t"+this->classN+" *_obj = ("+this->classN+"*)instance;\n";
						if(methpm.at(0)=="void")
						{
							opers += valsd;
							opers += "\n\tvoid* returnValue=NULL;\n\t*_obj-"+valsa+";";
							opers += "\n\treturn returnValue;";
						}
						else
						{
							opers += "\n\t"+methpm.at(0)+" *_retVal=new "+methpm.at(0)+";\n";
							opers += valsd;
							opers += "\n\t*_retVal = (*_obj-"+valsa+");";
							opers += "\n\treturn _retVal;";
						}
						opers += "\n}";
					}
					else if(meth.find("/")!=string::npos)
					{
						opers += "\nvoid* operator"+this->classN+"DI(void* instance,vals values)\n{\n\t"+this->classN+" *_obj = ("+this->classN+"*)instance;\n";
						if(methpm.at(0)=="void")
						{
							opers += valsd;
							opers += "\n\tvoid* returnValue=NULL;\n\t*_obj/"+valsa+";";
							opers += "\n\treturn returnValue;";
						}
						else
						{
							opers += "\n\t"+methpm.at(0)+" *_retVal=new "+methpm.at(0)+";\n";
							opers += valsd;
							opers += "\n\t*_retVal = (*_obj/"+valsa+");";
							opers += "\n\treturn _retVal;";
						}
						opers += "\n}";
					}
					else if(meth.find("*")!=string::npos)
					{
						opers += "\nvoid* operator"+this->classN+"MU(void* instance,vals values)\n{\n\t"+this->classN+" *_obj = ("+this->classN+"*)instance;\n";
						if(methpm.at(0)=="void")
						{
							opers += valsd;
							opers += "\n\tvoid* returnValue=NULL;\n\t*_obj*"+valsa+";";
							opers += "\n\treturn returnValue;";
						}
						else
						{
							opers += "\n\t"+methpm.at(0)+" *_retVal=new "+methpm.at(0)+";\n";
							opers += valsd;
							opers += "\n\t*_retVal = (*_obj*"+valsa+");";
							opers += "\n\treturn _retVal;";
						}
						opers += "\n}";
					}
					else if(meth.find("&&")!=string::npos)
					{

					}
					else if(meth.find("&")!=string::npos)
					{

					}
					else if(meth.find("||")!=string::npos)
					{

					}
					else if(meth.find("|")!=string::npos)
					{

					}
					else if(meth.find("[")!=string::npos && meth.find("]")!=string::npos)
					{

					}
					else if(meth.find("(")!=string::npos && meth.find(")")!=string::npos)
					{

					}
				}
				else
				{
					boost::replace_first(methsd,"<","");
					boost::replace_first(methsd,">","");
					boost::replace_first(methsd,"*","");
					boost::replace_first(methsd,"&","");
					if(methpm.at(0)!=this->classN)
					{
						typedefs += (") ("+typdefName+");\n");
						methods += "\nvoid* invokeReflectionCIMethodFor"+methsd+"(void* instance,vals values)\n{\n\t"+this->classN+" *_obj = ("+this->classN+"*)instance;\n";
						if(methpm.at(0)=="void")
						{
							methods += valsd;
							methods += "\n\tvoid* returnValue=NULL;\n\t_obj->"+methpm.at(1)+"("+valsa+");";
							methods += "\n\treturn returnValue;";
						}
						else
						{
							methods += "\n\t"+methpm.at(0)+" *_retVal=new "+methpm.at(0)+";\n";
							methods += valsd;
							methods += "\n\t*_retVal = (_obj->"+methpm.at(1)+"("+valsa+"));";
							methods += "\n\treturn _retVal;";
						}
						methods += "\n}";
						refDef += ("me.setMethodName(\""+methsd+"\");\n");
						methsall[methsd] = true;
						refDef += ("me.setArgumentTypes(argu);\n");
						refDef += ("argu.clear();\n");
						refDef += ("if(me.getMethodName()!=\"\")\n{\nclassInfo.addMethod(me);\n}\n");
					}
					else
					{
						typedefs += (") ("+typdefName+");\n");
						methods += "\nvoid* invokeReflectionCICtorFor"+methsd+"(vals values)\n{";
						methods += "\n\t"+this->classN+" *_retVal = NULL;\n";
						methods += valsd;
						methods += "\n\t_retVal = (new "+this->classN+"("+valsa+"));";
						methods += "\n\treturn _retVal;";
						methods += "\n}";
						refDef += ("ctor.setName(\""+methsd+"\");\n");
						refDef += ("ctor.setArgumentTypes(argu);\n");
						refDef += ("argu.clear();\n");
						refDef += ("classInfo.addConstructor(ctor);\n");
						ctorisp = true;
					}
				}
			}
			else if(this->pub.at(i).find("~")==string::npos)
			{
				refDef += ("f.clear();\n");
				publf += this->pub.at(i);
				fld = this->pub.at(i);
				boost::replace_first(fld,";","");
				strVec fldp;
				boost::iter_split(fldp, fld, boost::first_finder(" "));
				for(unsigned int j = 0; j < fldp.size(); j++)
				{
					if(j==0)
					{
						refDef += ("f.setType(\""+fldp.at(j)+"\");\n");
					}
					else if(j==1)
					{
						refDef += ("f.setFieldName(\""+fldp.at(j)+"\");\n");
					}
				}
				//if(fldp.size()==2)
				//	structinf += (fldp.at(0)+" "+fldp.at(1)+";\n");
				methods += "\n"+fldp.at(0)+" invokeReflectionCIFieldFor"+this->classN+fldp.at(1)+"(void* instance)\n{\n\t"+this->classN+" *_obj = ("+this->classN+"*)instance;\n\treturn _obj->"+fldp.at(1)+";\n}\n";
				refDef += ("if(f.getFieldName()!=\"\")\n{\nclassInfo.addField(f);\n}\n");
			}
			else if(this->pub.at(i).find("~")!=string::npos)
			{
				methods += "\nvoid invokeReflectionCIDtorFor"+this->classN+"(void* instance)\n{";
				methods += "\n\t"+this->classN+" *_obj = ("+this->classN+"*)instance;\n\t";
				methods += "_obj->~"+this->classN+"();";
				methods += "\n}";
				ddtorisp = true;
			}
		}
		if(!ctorisp)
		{
			refDef += ("ctor.setName(\""+this->classN+"\");\n");
			refDef += ("argu.clear();\n");
			methods += "\nvoid* invokeReflectionCICtorFor"+this->classN+"(vals values)\n{";
			methods += "\n\t"+this->classN+" *_retVal = NULL;\n";
			methods += "\n\t_retVal = (new "+this->classN+"());";
			methods += "\n\treturn _retVal;";
			methods += "\n}";
			refDef += ("ctor.setArgumentTypes(argu);\n");
			refDef += ("argu.clear();\n");
			refDef += ("classInfo.addConstructor(ctor);\n");
		}
		if(!ddtorisp)
		{
			methods += "\nvoid invokeReflectionCIDtorFor"+this->classN+"(void* instance)\n{";
			methods += "\n\t"+this->classN+" *_obj = ("+this->classN+"*)instance;\n\t";
			methods += "_obj->~"+this->classN+"();";
			methods += "\n}";
			ddtorisp = true;
		}
	}
	if (this->pri.size() > 0)
	{
		for (unsigned int i = 0; i < this->pri.size(); i++)
		{
			if((tes=this->pri.at(i).find("("))!=string::npos && (tes=this->pri.at(i).find(")"))!=string::npos && this->pri.at(i).find("~")==string::npos)
			{
				//refDef += ("me.clear();\n");
				privm += this->pri.at(i);
				meth = this->pri.at(i);
				boost::replace_first(meth,";","");
				boost::replace_first(meth,"("," ");
				boost::replace_all(meth,","," ");
				boost::replace_first(meth,")"," ");
				strVec methp;
				boost::iter_split(methp, meth, boost::first_finder(" "));
				for(unsigned int j = 0; j < methp.size(); j++)
				{
					if(j==0)
					{
						if(methp.at(0)==this->classN)
						{
							//refDef += ("me.setReturnType(\"Constructor\");\n");
							//refDef += ("me.setMethodName(\""+methp.at(j)+"\");\n");
						}
						else
						{
							//refDef += ("me.setReturnType(\""+methp.at(j)+"\");\n");
						}
					}
					else if(j==1 && methp.at(0)!=this->classN)
					{
						//refDef += ("me.setMethodName(\""+methp.at(j)+"\");\n");
					}
					else if(methp.at(j)!="")
					{
						//refDef += ("argu.push_back(\""+methp.at(j)+"\");\n");
					}
				}
				//refDef += ("me.setArgumentTypes(argu);\n");
				//refDef += ("if(me.getMethodName()!=\"\")\n{\nmethVec.push_back(me);\n}\n");
			}
			else if(this->pri.at(i).find("~")==string::npos)
			{
				refDef += ("f.clear();\n");
				privf += this->pri.at(i);
				fld = this->pri.at(i);
				boost::replace_first(fld,";","");
				strVec fldp;
				boost::iter_split(fldp, fld, boost::first_finder(" "));
				for(unsigned int j = 0; j < fldp.size(); j++)
				{
					if(j==0)
					{
						refDef += ("f.setType(\""+fldp.at(j)+"\");\n");
					}
					else if(j==1)
					{
						refDef += ("f.setFieldName(\""+fldp.at(j)+"\");\n");
					}
				}
				//if(fldp.size()==2)
				//	structinf += (fldp.at(0)+" "+fldp.at(1)+";\n");
				//methods += "\n"+fldp.at(0)+" invokeReflectionCIFieldFor"+this->classN+fldp.at(1)+"(void* instance)\n{\n\t"+this->classN+" *_obj = ("+this->classN+"*)instance;\nstruct"
				//+this->classN+" *__obj=(struct"+this->classN+"*)_obj;\n\treturn __obj->"+fldp.at(1)+";\n}\n";
				refDef += ("if(f.getFieldName()!=\"\")\n{\nclassInfo.addField(f);\n}\n");
			}
		}
	}
	if (this->pro.size() > 0)
	{
		for (unsigned int i = 0; i < this->pro.size(); i++)
		{
			if((tes=this->pro.at(i).find("("))!=string::npos && (tes=this->pro.at(i).find(")"))!=string::npos && this->pro.at(i).find("~")==string::npos)
			{
				//refDef += ("me.clear();\n");
				protm += this->pro.at(i);
				meth = this->pro.at(i);
				boost::replace_first(meth,";","");
				boost::replace_first(meth,"("," ");
				boost::replace_all(meth,","," ");
				boost::replace_first(meth,")"," ");
				strVec methp;
				boost::iter_split(methp, meth, boost::first_finder(" "));
				for(unsigned int j = 0; j < methp.size(); j++)
				{
					if(j==0)
					{
						if(methp.at(0)==this->classN)
						{
							//refDef += ("me.setReturnType(\"Constructor\");\n");
							//refDef += ("me.setMethodName(\""+methp.at(j)+"\");\n");
						}
						else
						{
							//refDef += ("me.setReturnType(\""+methp.at(j)+"\");\n");
						}
					}
					else if(j==1 && methp.at(0)!=this->classN)
					{
						//refDef += ("me.setMethodName(\""+methp.at(j)+"\");\n");
					}
					else if(methp.at(j)!="")
					{
						//refDef += ("argu.push_back(\""+methp.at(j)+"\");\n");
					}
				}
				//refDef += ("me.setArgumentTypes(argu);\n");
				//refDef += ("if(me.getMethodName()!=\"\")\n{\nmethVec.push_back(me);\n}\n");
			}
			else if(this->pro.at(i).find("~")==string::npos)
			{
				refDef += ("f.clear();\n");
				protf += this->pro.at(i);
				fld = this->pro.at(i);
				boost::replace_first(fld,";","");
				strVec fldp;
				boost::iter_split(fldp, fld, boost::first_finder(" "));
				for(unsigned int j = 0; j < fldp.size(); j++)
				{
					if(j==0)
					{
						refDef += ("f.setType(\""+fldp.at(j)+"\");\n");
					}
					else if(j==1)
					{
						refDef += ("f.setFieldName(\""+fldp.at(j)+"\");\n");
					}
				}
				//if(fldp.size()==2)
				//	structinf += (fldp.at(0)+" "+fldp.at(1)+";\n");
				//methods += "\n"+fldp.at(0)+" invokeReflectionCIFieldFor"+this->classN+fldp.at(1)+"(void* instance)\n{\n\t"+this->classN+" *_obj = ("+this->classN+"*)instance;\nstruct"
				//+this->classN+" *__obj=(struct"+this->classN+"*)_obj;\n\treturn __obj->"+fldp.at(1)+";\n}\n";
				refDef += ("if(f.getFieldName()!=\"\")\n{\nclassInfo.addField(f);\n}\n");
			}
		}
	}
	//refDef += ("\nclassInfo.setMeths(meths);");
	//refDef += ("\nclassInfo.setMethods(methVec);");
	//refDef += ("\nclassInfo.setFields(fldVec);");
	refDef += "\nreturn classInfo;\n}\n";
	refDef += "\nvoid invokeAdToVecFor"+this->classN+"(void* _vec,void* _instance){"+this->classN+" *_obj = ("+this->classN+"*)_instance;vector<"+this->classN+"> *_objvec = (vector<"+this->classN+"> *)_vec;_objvec->push_back(*_obj);}";
	refDef += "\nvoid* invokeGetNewVecFor"+this->classN+"(){vector<"+this->classN+"> *_objvec = new vector<"+this->classN+">;return _objvec;}";
	refDef += "\nint invokeGetVecSizeFor"+this->classN+"(void* _vec){vector<"+this->classN+"> *_objvec = (vector<"+this->classN+">*)_vec;return (int)_objvec->size();}";
	refDef += "\nvoid* invokeGetVecElementFor"+this->classN+"(void* _vec,int pos){vector<"+this->classN+"> *_objvec = (vector<"+this->classN+">*)_vec;return &(_objvec->at(pos));}";
	//refDef = (structinf+"};\n"+refDef);
	return refDef;
}

string Reflection::generateSerDefinitionAll(strVec all,string &includeRef)
{
	string ret = "#include \"XmlParser.h\"\n#include <boost/lexical_cast.hpp>\n";
	includeRef = "#include \"Reflector.h\"\n#include \"Trace.h\"\n#include \"vector\"\n#include \"list\"\n#include \"queue\"\n#include \"deque\"\n#include \"set\"\n#include \"DateFormat.h\"\n" ;
	string typedefs,classes,methods,rert1;
	for (unsigned int var = 0; var < all.size(); ++var)
	{
		rert1 += this->generateSerDefinitions(all.at(var),includeRef,typedefs,classes,methods);

	}
	includeRef += ("extern \"C\"{\n" + classes + typedefs + methods);
	ret += includeRef;
	/*ret += "\npublic:\ntemplate <class T> string serialize(T t)\n{\nstring objXml;\n";
	ret += "string className;\nint status;\nconst char *mangled = typeid(t).name();\nusing namespace abi;\nmangled = __cxa_demangle(mangled, NULL, 0, &status);\nstringstream ss;\nss << mangled;\nss >> className;";
	ret += classes;
	ret += "\n\treturn objXml;\n}\n";
	ret += "template <class T> T unSerialize(string objXml)\n{\nT t;\nstring className;\nint status;\nconst char *mangled = typeid(t).name();\nusing namespace abi;\nmangled = __cxa_demangle(mangled, NULL, 0, &status);\nstringstream ss;\nss << mangled;\nss >> className;\n";
	ret += rert1;
	ret += "\n\treturn t;\n}\n";
	ret += "}\n";*/
	ret += "}\n";
	includeRef += typedefs;
	return ret;
}

string Reflection::generateSerDefinitions(string includeDir,string &includesDefs,string &typedefs,string &classes,string &methods)
{
	strVec includes = list(includeDir);
	string ret;
	for (unsigned int var = 0; var < includes.size(); ++var)
	{
		cout << "\ngenerating Ser for file" << includes.at(var) << "\n" << flush;
		ret += generateSerDefinition(includes.at(var),includesDefs,typedefs,classes,methods);
		cout << "\ndone generating Ser for file" << includes.at(var) << "\n" << flush;
	}
	return ret;
}

string Reflection::generateSerDefinition(string className,string &includesDefs,string &typedefs,string &classes,string &methods)
{
	string refDef;
	string opers;
	if (!generateClassInfo(className))
	{
		return refDef;
	}
	prosetser = false;
	//classes += "\tif(className==\""+this->classN+"\")\n\t\tobjXml = get"+this->classN+"XML(t);\n";
	refDef += "\tif(className==\""+this->classN+"\")\n\t\tt = getObject"+this->classN+"(objXml);\n";
	includesDefs += "#include \"" + this->classN + ".h\"\n";
	//string structinf = "\nstruct struct"+this->classN+"{\n";
	classes += "\nstring serialize" + this->classN + "(void* obje);\nvoid* unSerialize" + this->classN + "(string objXml);";
	methods += "\nstring serialize" + this->classN + "(void* obje)\n{\n"+this->classN+" *__obj=("+this->classN+"*)obje;\n";
	methods += "string objxml = \"<"+this->classN+">\";\n";
	typedefs += "\nvoid* unSerialize" + this->classN + "(string objXml)\n{\n";
	typedefs += this->classN+" *__obj=new "+this->classN+";\nXmlParser parser(\"Parser\");\nElement root=parser.getDocument(objXml).getRootElement();\n";
	typedefs += "for(unsigned int i=0;i<root.getChildElements().size();i++)\n{\n";
	typedefs += "string nam=root.getChildElements().at(i).getTagName();\n";

	string publf, privf, protf ,publm, privm, protm;
	string meth,fld;
	size_t tes;
	strVec fldnames;
	if (this->pri.size() > 0)
	{
		for (unsigned int i = 0; i < this->pri.size(); i++)
		{
			if(((tes=this->pri.at(i).find("("))==string::npos && (tes=this->pri.at(i).find(")"))==string::npos && this->pri.at(i).find("~")==string::npos))
			{
				fld = this->pri.at(i);
				boost::replace_first(fld,";","");
				strVec fldp;
				boost::iter_split(fldp, fld, boost::first_finder(" "));
				if(fldp.size()==2)
				{
					string nam = fldp.at(1);
					fldnames.push_back(fldp.at(0));
					fldnames.push_back(nam);
				}
			}
		}
	}
	if (this->pro.size() > 0)
	{
		for (unsigned int i = 0; i < this->pro.size(); i++)
		{
			if(((tes=this->pro.at(i).find("("))==string::npos && (tes=this->pro.at(i).find(")"))==string::npos && this->pro.at(i).find("~")==string::npos))
			{
				fld = this->pro.at(i);
				boost::replace_first(fld,";","");
				strVec fldp;
				boost::iter_split(fldp, fld, boost::first_finder(" "));
				if(fldp.size()==2)
				{
					string nam = fldp.at(1);
					fldnames.push_back(fldp.at(0));
					fldnames.push_back(nam);
				}
			}
		}
	}
	if (this->pub.size() > 0)
	{
		for (unsigned int i = 0; i < this->pub.size(); i++)
		{
			if(((tes=this->pub.at(i).find("("))==string::npos && (tes=this->pub.at(i).find(")"))==string::npos && this->pub.at(i).find("~")==string::npos))
			{
				fld = this->pub.at(i);
				boost::replace_first(fld,";","");
				strVec fldp;
				boost::iter_split(fldp, fld, boost::first_finder(" "));
				if(fldp.size()==2)
				{
					string nam = fldp.at(1);
					if(fldp.at(0)=="int" || fldp.at(0)=="float" || fldp.at(0)=="string" || fldp.at(0)=="double" || fldp.at(0)=="bool")
					{
						methods += ("objxml += \"<"+fldp.at(1)+" type=\\\""+fldp.at(0)+"\\\">\"+boost::lexical_cast<string>(__obj->"+fldp.at(1)+")");
						string cam = AfcUtil::camelCased(fldp.at(1));
						methods += ("+\"</"+nam+">\";\n");
						typedefs += "if(nam==\""+fldp.at(1)+"\")\n__obj->"+fldp.at(1)+" = boost::lexical_cast<"+fldp.at(0)+">(root.getChildElements().at(i).getText());\n";
					}
					else if(fldp.at(0)=="Date")
					{
						methods += ("DateFormat formt"+fldp.at(1)+"(\"yyyy-mm-dd hh:mi:ss\");\nobjxml += \"<"+fldp.at(1)+" type=\\\""+fldp.at(0)+"\\\">\"+formt"+fldp.at(1)+".format(__obj->"+fldp.at(1)+")");
						string cam = AfcUtil::camelCased(fldp.at(1));
						methods += ("+\"</"+nam+">\";\n");
						typedefs += "if(nam==\""+fldp.at(1)+"\")\n{\nDateFormat formt"+fldp.at(1)+"(\"yyyy-mm-dd hh:mi:ss\");\n__obj->"+fldp.at(1)+" = *(formt"+fldp.at(1)+".parse(root.getChildElements().at(i).getText()));\n}\n";
					}
					else if(fldp.at(0)=="BinaryData")
					{
						methods += ("objxml += \"<"+fldp.at(1)+" type=\\\""+fldp.at(0)+"\\\">\"+BinaryData::serilaize(__obj->"+fldp.at(1)+")");
						string cam = AfcUtil::camelCased(fldp.at(1));
						methods += ("+\"</"+nam+">\";\n");
						typedefs += "if(nam==\""+fldp.at(1)+"\")\n__obj->"+fldp.at(1)+" = *(BinaryData::unSerilaize(root.getChildElements().at(i).getText()));\n";
					}
					else if(fldp.at(0).find("vector")!=string::npos || fldp.at(0).find("queue")!=string::npos || fldp.at(0).find("deque")!=string::npos || fldp.at(0).find("set")!=string::npos || fldp.at(0).find("list")!=string::npos)
					{
						string stlcnt = fldp.at(0);
						string stltyp = fldp.at(0);
						boost::replace_first(stltyp,"<","::");
						boost::replace_first(stltyp,">","");
						boost::replace_first(stltyp," ","");
						string stlcnttyp = "";
						if(fldp.at(0).find("vector")!=string::npos)
							stlcnttyp = "Vec";
						else if(fldp.at(0).find("vector")!=string::npos)
							stlcnttyp = "Q";
						else if(fldp.at(0).find("vector")!=string::npos)
							stlcnttyp = "Dq";
						else if(fldp.at(0).find("vector")!=string::npos)
							stlcnttyp = "Lis";
						else
							stlcnttyp = "Set";
						boost::replace_first(stlcnt,"vector","");
						boost::replace_first(stlcnt,"queue","");
						boost::replace_first(stlcnt,"deque","");
						boost::replace_first(stlcnt,"set","");
						boost::replace_first(stlcnt,"list","");
						boost::replace_first(stlcnt,"<","");
						boost::replace_first(stlcnt,">","");
						boost::replace_first(stlcnt," ","");

						methods += (fldp.at(0)+" __temp_obj_ser = __obj->"+fldp.at(1)+";\n");
						methods += ("objxml += \"<"+fldp.at(1)+" type=\\\""+stltyp+"\\\">\"+serialize"+stlcnt+stlcnttyp+"(&__temp_obj_ser)");
						string cam = AfcUtil::camelCased(fldp.at(1));
						methods += ("+\"</"+nam+">\";\n");
						typedefs += "if(nam==\""+fldp.at(1)+"\")\n__obj->"+fldp.at(1)+" = *("+fldp.at(0)+"*)unSerialize"+stlcnt+stlcnttyp+"(root.getChildElements().at(i).renderChildren());\n";
					}
					else
					{
						methods += (fldp.at(0)+" __temp_obj_ser = __obj->"+fldp.at(1)+";\n");
						methods += ("objxml += \"<"+fldp.at(1)+" type=\\\""+fldp.at(0)+"\\\">\"+serialize"+fldp.at(0)+"(&__temp_obj_ser)");
						string cam = AfcUtil::camelCased(fldp.at(1));
						methods += ("+\"</"+nam+">\";\n");
						typedefs += "if(nam==\""+fldp.at(1)+"\")\n__obj->"+fldp.at(1)+" = *("+fldp.at(0)+"*)unSerialize"+fldp.at(0)+"(root.getChildElements().at(i).renderChildren());\n";
					}
					//structinf += (fldp.at(0)+" "+fldp.at(1)+";\n");
				}
			}
			if((tes=this->pub.at(i).find("("))!=string::npos && (tes=this->pub.at(i).find(")"))!=string::npos && this->pub.at(i).find("~")==string::npos
					&& fldnames.size()>0)
			{
				meth = this->pub.at(i);
				boost::replace_first(meth,";","");

				string argts = meth.substr(meth.find("("),meth.find(")")-meth.find("("));
				boost::replace_first(argts,"(","");
				boost::replace_all(argts,")","");
				meth = meth.substr(0,meth.find("("));
				if(meth.find("operator")!=string::npos)
				{
					if(meth.find("<")!=string::npos)
					{
						prosetser = true;
					}
					else if(meth.find(">")!=string::npos)
					{
						prosetser = true;
					}
				}
				else
				{
					//boost::replace_first(meth,")"," ");
					strVec methp,methpm,argp,argpm;
					boost::iter_split(argp, argts, boost::first_finder(","));
					boost::iter_split(methp, meth, boost::first_finder(" "));
					for(unsigned int j = 0; j < methp.size(); j++)
					{
						if(methp.at(j)!="")
							methpm.push_back(methp.at(j));
					}
					for(unsigned int j = 0; j < argp.size(); j++)
					{
						if(argp.at(j)!="" && argp.at(j)!="(")
							argpm.push_back(argp.at(j));
					}

					if(methpm.at(0)!=this->classN)
					{
						for(unsigned int k = 0; k < fldnames.size(); k=k+2)
						{
							string cam = AfcUtil::camelCased(fldnames.at(k+1));
							if("set"+cam==methpm.at(1) && argpm.size()==1 && argpm.at(0)==fldnames.at(k) && methpm.at(0)=="void")
							{
								if(argpm.at(0)=="int" || argpm.at(0)=="float" || argpm.at(0)=="string" || argpm.at(0)=="double" || argpm.at(0)=="bool")
								{
									typedefs += "if(nam==\""+fldnames.at(k+1)+"\")\n__obj->"+methpm.at(1)+"(boost::lexical_cast<"+argpm.at(0)+">(root.getChildElements().at(i).getText()));\n";
								}
								else if(argpm.at(0)=="Date")
								{
									typedefs += "if(nam==\""+fldnames.at(k+1)+"\")\n{\nDateFormat formt"+cam+"(\"yyyy-mm-dd hh:mi:ss\");\n__obj->"+methpm.at(1)+"(*(formt"+cam+".parse(root.getChildElements().at(i).getText())));\n}\n";
								}
								else if(argpm.at(0)=="BinaryData")
								{
									typedefs += "if(nam==\""+fldnames.at(k+1)+"\")\n__obj->"+methpm.at(1)+"(*(BinaryData::unSerilaize(root.getChildElements().at(i).getText())));\n";
								}
								else
								{
									typedefs += "if(nam==\""+fldnames.at(k+1)+"\")\n__obj->"+methpm.at(1)+"(*("+argpm.at(0)+"*)unSerialize"+argpm.at(0)+"(root.getChildElements().at(i).renderChildren()));\n";
								}
							}
							else if("get"+cam==methpm.at(1) && argpm.size()==0 && methpm.at(0)==fldnames.at(k))
							{
								if(methpm.at(0)=="int" || methpm.at(0)=="float" || methpm.at(0)=="string" || methpm.at(0)=="double" || methpm.at(0)=="bool")
								{
									methods += ("objxml += \"<"+fldnames.at(k+1)+" type=\\\""+methpm.at(0)+"\\\">\"+boost::lexical_cast<string>(__obj->"+methpm.at(1)+"())");
									methods += ("+\"</"+fldnames.at(k+1)+">\";\n");
									if(methsall[this->classN+"get"+cam+methpm.at(0)])typedefs += "if(nam==\""+fldnames.at(k+1)+"\")\n__obj->set"+cam+"(boost::lexical_cast<"+methpm.at(0)+">(root.getChildElements().at(i).getText()));\n";
								}
								else if(methpm.at(0)=="Date")
								{
									methods += ("DateFormat formt"+fldnames.at(k+1)+"(\"yyyy-mm-dd hh:mi:ss\");\nobjxml += \"<"+fldnames.at(k+1)+" type=\\\""+methpm.at(0)+"\\\">\"+formt"+fldnames.at(k+1)+".format(__obj->"+methpm.at(1)+"())");
									methods += ("+\"</"+fldnames.at(k+1)+">\";\n");
									if(methsall[this->classN+"get"+cam+methpm.at(0)])typedefs += "if(nam==\""+fldnames.at(k+1)+"\")\n{\nDateFormat formt"+fldnames.at(k+1)+"(\"yyyy-mm-dd hh:mi:ss\");\n__obj->set"+cam+"(*(formt"+fldnames.at(k+1)+".parse(root.getChildElements().at(i).getText())));\n}\n";
								}
								else if(methpm.at(0)=="BinaryData")
								{
									methods += ("objxml += \"<"+fldnames.at(k+1)+" type=\\\""+methpm.at(0)+"\\\">\"+BinaryData::serilaize(__obj->"+methpm.at(1)+"())");
									methods += ("+\"</"+fldnames.at(k+1)+">\";\n");
									if(methsall[this->classN+"get"+cam+methpm.at(0)])typedefs += "if(nam==\""+fldnames.at(k+1)+"\")\n__obj->set"+cam+"(*(BinaryData::unSerilaize(root.getChildElements().at(i).getText())));\n";
								}
								else if(methpm.at(0).find("vector")!=string::npos || methpm.at(0).find("queue")!=string::npos || methpm.at(0).find("deque")!=string::npos || methpm.at(0).find("set")!=string::npos || methpm.at(0).find("list")!=string::npos)
								{
									string stlcnt = methpm.at(0);
									string stltyp = methpm.at(0);
									boost::replace_first(stltyp,"<","::");
									boost::replace_first(stltyp,">","");
									boost::replace_first(stltyp," ","");
									string stlcnttyp = "";
									if(methpm.at(0).find("vector")!=string::npos)
										stlcnttyp = "Vec";
									else if(methpm.at(0).find("queue")!=string::npos)
										stlcnttyp = "Q";
									else if(methpm.at(0).find("deque")!=string::npos)
										stlcnttyp = "Dq";
									else if(methpm.at(0).find("list")!=string::npos)
										stlcnttyp = "Lis";
									else
										stlcnttyp = "Set";
									boost::replace_first(stlcnt,"vector","");
									boost::replace_first(stlcnt,"queue","");
									boost::replace_first(stlcnt,"deque","");
									boost::replace_first(stlcnt,"set","");
									boost::replace_first(stlcnt,"list","");
									boost::replace_first(stlcnt,"<","");
									boost::replace_first(stlcnt,">","");
									boost::replace_first(stlcnt," ","");

									methods += (methpm.at(0)+" __temp_obj_ser = __obj->"+methpm.at(1)+"();\n");
									methods += ("objxml += \"<"+fldnames.at(k+1)+" type=\\\""+stltyp+"\\\">\"+serialize"+stlcnt+stlcnttyp+"(&__temp_obj_ser)");
									//string cam = AfcUtil::camelCased(methpm.at(1));
									methods += ("+\"</"+fldnames.at(k+1)+">\";\n");
									//if(methsall[this->classN+"get"+cam+methpm.at(0)])typedefs += "if(nam==\""+methpm.at(1)+"\")\n__obj->set"+cam+"(*("+methpm.at(0)+"*)unSerialize"+stlcnt+stlcnttyp+"(root.getChildElements().at(i).renderChildren()));\n";
								}
								else
								{
									methods += (methpm.at(0)+" __temp_obj_ser = __obj->"+methpm.at(1)+"();\n");
									methods += ("objxml += \"<"+fldnames.at(k+1)+" type=\\\""+methpm.at(0)+"\\\">\"+serialize"+methpm.at(0)+"(&__temp_obj_ser)");
									methods += ("+\"</"+fldnames.at(k+1)+">\";\n");
									//string cam = AfcUtil::camelCased(methpm.at(1));
									//if(methsall[this->classN+"get"+cam+methpm.at(0)])typedefs += "if(nam==\""+methpm.at(1)+"\")\n__obj->set"+cam+"(*("+methpm.at(0)+"*)unSerialize"+methpm.at(0)+"(root.getChildElements().at(i).renderChildren()));\n";
								}
							}
						}
					}
				}
			}
		}
	}

	//refDef += ("\nclassInfo.setMeths(meths);");
	//refDef += ("\nclassInfo.setMethods(methVec);");
	//refDef += ("\nclassInfo.setFields(fldVec);");
	//refDef += "\nreturn classInfo;\n}\n";
	methods += "objxml += \"</"+this->classN+">\";\nreturn objxml;\n}\n";
	methods += "\nstring serialize" + this->classN + "Vec(void* obje)\n{\nvector<"+this->classN+"> *__obj=(vector<"+this->classN+">*)obje;\n";
	methods += "string xml=\"<vector-"+this->classN+">\";\nfor(unsigned int i=0;i<__obj->size();i++)\n{\nxml+=serialize"+this->classN+"(&(__obj->at(i)));\n}\nxml+=\"</vector-"+this->classN+">\";\n";
	methods += "return xml;}\n";
	methods += "\nstring serialize"+this->classN+"Q(void *t){queue<"+this->classN+"> *_t=(queue<"+this->classN+">*)t;queue<"+this->classN+"> *tt = new queue<"+this->classN+">;	*tt = *_t;	string objXml = \"<queue-"+this->classN+">\";	for(unsigned int var=0;var<tt->size();var++)	{		objXml += serialize"+this->classN+"(&(tt->front()));		tt->pop();	}	objXml += \"</queue-"+this->classN+">\";	return objXml;}";
	methods += "\nstring serialize"+this->classN+"Dq(void *_t){deque<"+this->classN+"> *t=(deque<"+this->classN+">*)_t;string objXml = \"<deque-"+this->classN+">\";	for(unsigned int var=0;var<t->size();var++)	{		objXml += serialize"+this->classN+"(&(t->at(var)));	}	objXml += \"</deque-"+this->classN+">\";	return objXml;}";
	methods += "\nstring serialize"+this->classN+"Lis(void *_t){	list<"+this->classN+"> *t=(list<"+this->classN+">*)_t;list<"+this->classN+">::iterator it;	string objXml = \"<list-"+this->classN+">\";	for(it=t->begin();it!=t->end();++it)	{"+this->classN+" _temp=*it;	objXml += serialize"+this->classN+"(&_temp);	}	objXml += \"</list-"+this->classN+">\";	return objXml;}";
	classes += "\nstring serialize" + this->classN + "Vec(void* obje);\nstring serialize"+this->classN+"Q(void *t);\nstring serialize"+this->classN+"Dq(void *_t);\nstring serialize"+this->classN+"Lis(void *_t);";
	if(this->prosetser)
	{
		methods += "\nstring serialize"+this->classN+"Set(void *_t){	set<"+this->classN+"> *t=(set<"+this->classN+">*)_t;set<"+this->classN+">::iterator it;	string objXml = \"<set-"+this->classN+">\";	for(it=t->begin();it!=t->end();++it)	{"+this->classN+" _temp=*it;	objXml += serialize"+this->classN+"(&_temp);	}	objXml += \"</set-"+this->classN+">\";	return objXml;}";
		methods += "\nstring serialize"+this->classN+"MulSet(void *_t){	multiset<"+this->classN+"> *t=(multiset<"+this->classN+">*)_t;multiset<"+this->classN+">::iterator it;	string objXml = \"<multiset-"+this->classN+">\";	for(it=t->begin();it!=t->end();++it)	{"+this->classN+" _temp=*it;	objXml += serialize"+this->classN+"(&_temp);	}	objXml += \"</multiset-"+this->classN+">\";	return objXml;}";
		classes += "\nstring serialize"+this->classN+"Set(void *_t);\nstring serialize"+this->classN+"MulSet(void *_t);";
	}

	typedefs += "\n}\nreturn __obj;\n}";
	typedefs += "\nvoid* unSerialize"+this->classN+"Dq(string objXml){deque<"+this->classN+"> *t = new deque<"+this->classN+">;XmlParser parser(\"Parser\");Document doc = parser.getDocument(objXml);Element message = doc.getRootElement();for (int var = 0; var < (int)message.getChildElements().size(); var++){	Element ele = message.getChildElements().at(var);	if(ele.getTagName()==\""+this->classN+"\")	{		t->push_back(*("+this->classN+"*)unSerialize"+this->classN+"(ele.renderChildren()));	}}return t;}";
	typedefs += "\nvoid* unSerialize"+this->classN+"Q(string objXml){queue<"+this->classN+"> *t = new queue<"+this->classN+">;XmlParser parser(\"Parser\");Document doc = parser.getDocument(objXml);Element message = doc.getRootElement();for (int var = 0; var < (int)message.getChildElements().size(); var++){	Element ele = message.getChildElements().at(var);	if(ele.getTagName()==\""+this->classN+"\")	{		t->push(*("+this->classN+"*)unSerialize"+this->classN+"(ele.renderChildren()));	}}return t;}";
	typedefs += "\nvoid* unSerialize"+this->classN+"Lis(string objXml){list<"+this->classN+"> *t = new list<"+this->classN+">;XmlParser parser(\"Parser\");Document doc = parser.getDocument(objXml);Element message = doc.getRootElement();for (int var = 0; var < (int)message.getChildElements().size(); var++){	Element ele = message.getChildElements().at(var);	if(ele.getTagName()==\""+this->classN+"\")	{		t->push_back(*("+this->classN+"*)unSerialize"+this->classN+"(ele.renderChildren()));	}}return t;}";
	classes += "\nvoid* unSerialize"+this->classN+"Vec(string objXml);\nvoid* unSerialize"+this->classN+"Q(string objXml);\nvoid* unSerialize"+this->classN+"Dq(string objXml);\nvoid* unSerialize"+this->classN+"Lis(string objXml);";
	if(this->prosetser)
	{
		typedefs += "\nvoid* unSerialize"+this->classN+"Set(string objXml){set<"+this->classN+"> *t = new set<"+this->classN+">;XmlParser parser(\"Parser\");Document doc = parser.getDocument(objXml);Element message = doc.getRootElement();for (int var = 0; var < (int)message.getChildElements().size(); var++){	Element ele = message.getChildElements().at(var);	if(ele.getTagName()==\""+this->classN+"\")	{		t->insert(*("+this->classN+"*)unSerialize"+this->classN+"(ele.renderChildren()));	}}return t;}";
		typedefs += "\nvoid* unSerialize"+this->classN+"MulSet(string objXml){multiset<"+this->classN+"> *t = new multiset<"+this->classN+">;XmlParser parser(\"Parser\");Document doc = parser.getDocument(objXml);Element message = doc.getRootElement();for (int var = 0; var < (int)message.getChildElements().size(); var++){	Element ele = message.getChildElements().at(var);	if(ele.getTagName()==\""+this->classN+"\")	{		t->insert(*("+this->classN+"*)unSerialize"+this->classN+"(ele.renderChildren()));	}}return t;}";
		classes += "\nvoid* unSerialize"+this->classN+"Set(string objXml);\nvoid* unSerialize"+this->classN+"MulSet(string objXml);";
	}
	typedefs += "\nvoid* unSerialize"+this->classN+"Vec(string objXml){vector<"+this->classN+"> *t = new vector<"+this->classN+">;XmlParser parser(\"Parser\");Document doc = parser.getDocument(objXml);Element message = doc.getRootElement();for (int var = 0; var < (int)message.getChildElements().size(); var++){	Element ele = message.getChildElements().at(var);	if(ele.getTagName()==\""+this->classN+"\")	{		t->push_back(*("+this->classN+"*)unSerialize"+this->classN+"(ele.renderChildren()));	}}return t;}";
	//typedefs = (structinf+"};\n"+typedefs);
	return refDef;
}
