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
 * AfcUtil.cpp
 *
 *  Created on: Aug 27, 2009
 *      Author: sumeet
 */

#include "AfcUtil.h"
#include <dlfcn.h>
typedef map<string,strVec> mapofvec;
AfcUtil::AfcUtil() {
	// TODO Auto-generated constructor stub

}

AfcUtil::~AfcUtil() {
	// TODO Auto-generated destructor stub
}
static map<string,string> doneMap;
string AfcUtil::generateJsObjectsAll(vector<string> obj,strVec files,vector<bool> stat,string &headers,string &objs,string &infjs)
{
	Reflection ref;
	string ret="#include \"AfcInclude.h\"\n\nextern \"C\"\n{\n";
	headers = "#include <boost/lexical_cast.hpp>\n#include \"json_spirit.h\"\n";
	if(obj.size()==0)return ret + "}";
	ret += "\nconst mValue& find_value(const mObject& obj, const string& name)\n{\nmObject::const_iterator i = obj.find(name);\nreturn i->second;\n}\n";
	for (unsigned int var = 0; var < obj.size(); ++var)
	{
		if(stat.at(var))
		{
			strVec info = ref.getAfcObjectData(obj.at(var)+files.at(var)+".h", true);
			ret += generateJsObjects(info,files.at(var),headers,obj.at(var),objs);
		}
		else
		{
			strVec info = ref.getAfcObjectData(obj.at(var)+files.at(var)+".h", false);
			ret += generateJsInterfaces(info,files.at(var),headers,obj.at(var),infjs);
		}
	}
	headers += "\nusing namespace json_spirit;\n";
	ret = (ret + "\n}\n");
	//writeTofile("/home/sumeet/workspace/inter/AfcInclude.h",headers,true);
	return ret;
}

string AfcUtil::generateJsObjects(strVec obj,string claz,string &headers,string path,string &objs)
{
	if(doneMap.find(claz)==doneMap.end())
		doneMap[claz] = "done";
	else
		return "";
	string tes,fres;
	Reflection ref;
	string retu;
	string test = "function _"+claz+"()\n{\n";
	fres = "string json=\"{";
	for(unsigned int i=0;i<obj.size();i++)
	{
		strVec vemp;
		string data = obj.at(i);
		boost::replace_first(data,";","");
		boost::iter_split(vemp, data, boost::first_finder(" "));
		test += "this." + vemp.at(1) + "= null;\n";
		if(vemp.at(0)=="float")
		{
			tes += "double te_ = find_value( obj, \""+vemp.at(1)+"\" ).get_value<double>();\n";
			tes += "_obj."+vemp.at(1)+" = boost::lexical_cast<"+vemp.at(0)+">(te_);";
		}
		else if(vemp.at(0)=="int" || vemp.at(0)=="double" || vemp.at(0)=="string")
		{
			tes += "_obj." + vemp.at(1) + "= find_value( obj, \""+vemp.at(1)+"\" ).get_value<"+vemp.at(0)+">();\n";
		}
		else
		{
			tes += "_obj." + vemp.at(1) + "= read"+vemp.at(0)+"(find_value( obj, \""+vemp.at(1)+"\" ).get_obj());\n";
		}
		if(vemp.at(0)=="int" || vemp.at(0)=="float" || vemp.at(0)=="double" || vemp.at(0)=="string")
		{
			if(vemp.at(0)=="string")
				fres += "'"+vemp.at(1)+"' : '\"+_obj."+vemp.at(1)+"+\"'";
			else
				fres += "'"+vemp.at(1)+"' : '\"+boost::lexical_cast<string>(_obj."+vemp.at(1)+")+\"'";
			if(i!=obj.size()-1)
			{
				fres += ",";
			}
		}
		else
		{
			strVec info = ref.getAfcObjectData(path+vemp.at(0)+".h", true);
			retu += generateJsObjects(info,vemp.at(0),headers,path,objs);
			fres += "'"+vemp.at(1)+"' : '\"+from"+vemp.at(0)+"ToJSON(_obj."+vemp.at(1)+")+\"'";
			if(i!=obj.size()-1)
			{
				fres += ",";
			}
		}
	}
	fres += "}\";";
	cout << fres << flush;
	test += "}";
	cout << test << flush;
	//writeTofile("/home/sumeet/server/web/default/_afc_Objects.js",test,true);
	objs += test;
	headers += "#include \""+claz+".h\"\n";

	//test = "\nconst mValue& find_value(const mObject& obj, const string& name)\n{\nmObject::const_iterator i = obj.find(name);\nreturn i->second;\n}\n";
	test = retu+ "\n\n" + claz + " read"+claz+"(const mObject& obj)\n{\n"+claz+" _obj;\n";
	/*for(unsigned int i=0;i<obj.size();i++)
	{
		strVec vemp;
		string data = obj.at(i);
		boost::replace_first(data,";","");
		boost::iter_split(vemp, data, boost::first_finder(" "));
		if(vemp.at(0)=="float")
		{
			test += "double te_ = find_value( obj, \""+vemp.at(1)+"\" ).get_value<double>();\n";
			test += "_obj."+vemp.at(1)+" = boost::lexical_cast<"+vemp.at(0)+">(te_);";
		}
		else
			test += "_obj." + vemp.at(1) + "= find_value( obj, \""+vemp.at(1)+"\" ).get_value<"+vemp.at(0)+">();\n";
	}*/
	test += tes + "\nreturn _obj;\n}\n";
	test += claz + " to"+claz+"(string s)\n{\nmValue value;\nread(s,value);\n"+ claz +" _obj = read"+claz+"(value.get_obj());\nreturn _obj;\n}\n";
	test += "\nstring from"+claz+"ToJSON("+claz+" _obj)\n{\n"+fres+"\nreturn json;\n}\n";
	return test;
}

string AfcUtil::generateJsInterfaces(strVec obj,string claz,string &headers,string path,string &infjs)
{
	string test,intf,intff,inc;
	headers += "#include \"" + claz + ".h\"\n";
	//writeTofile("/home/sumeet/workspace/inter/AfcInclude.h",inc,false);
	//inc = "\nextern \"C\"{\n";//string executeAFC(string fn,strVec _inp){\nstring ret;\n";
	bool fl = false;
	test = ("var " + claz + "= {\n");
	for(unsigned int i=0;i<obj.size();i++)
	{
		strVec vemp,emp;
		string data = obj.at(i);
		boost::replace_first(data,";","");
		boost::split(vemp, data, boost::is_any_of(" ,)("));
		for(unsigned int k=0;k<vemp.size();k++)
		{
			if(vemp.at(k)!="")
				emp.push_back(vemp.at(k));
			//cout << vemp.at(i) << "\n" << flush;
		}
		//cout << "\n------------------------------------\n" << flush;
		if(emp.size()>1)
		{
			size_t te = emp.at(1).find("~");
			if(te==string::npos)
			{
				string pars,parswt,types,jsonstr;
				if(emp.size()==2)
				{
					test += emp.at(1) + ": function(){\n";
				}
				else
				{
					test += emp.at(1) + ": function(";
					for(unsigned int j=2;j<emp.size();j++)
					{
						stringstream st,st1,st2;
						//types += ("\"" + emp.at(j) + "\"");
						st << "_";
						st << (j-1);
						if(j!=emp.size()-1)
						{
							st << ",";
							//types += ",";
						}
						string h;
						st >> h;
						test += h;
						pars += h;
						parswt += ("string " + h);
						//st1 << emp.at(j) << " __" << (j-1);
						//st1 << " = (" << emp.at(j) << ")_" << (j-1) << "->c_str();\n";
						string h1;
						//st1 >> h1;
						if(emp.at(j)=="string" || emp.at(j)=="int" || emp.at(j)=="float" || emp.at(j)=="double")
						{
							types.append(emp.at(j).c_str());
							types.append(" _");
							st1 << (j-1);
							getline(st1,h1);
							jsonstr += "_"+h1;
							types.append(h1.c_str());
							types.append("=boost::lexical_cast<");
							types.append(emp.at(j).c_str());
							types.append(">(_inp.at(");
							st2 << (j-2);
							getline(st2,h1);
							types.append(h1.c_str());
							types.append(").c_str());\n");
						}
						else
						{
							types.append(emp.at(j).c_str());
							types.append(" _");
							st1 << (j-1);
							getline(st1,h1);
							jsonstr += "JSON.stringify(_"+h1+")";
							types.append(h1.c_str());
							types.append("=to"+emp.at(j)+"(_inp.at(");
							st2 << (j-2);
							getline(st2,h1);
							types.append(h1.c_str());
							types.append("));\n");

						}
						if(j!=emp.size()-1)
						{
							jsonstr += ",";
						}
						//types += (emp.at(j).c_str() + " __" + (j-1));
						//cout << vemp.at(i) << "\n" << flush;
					}
					fl = true;
					test += "){\n";
					test += "AfcCall(\""+claz+"\",\""+emp.at(1)+"\",new Array("+jsonstr+"));\n";
				}
				inc += updateAjaxInterface(emp,claz,pars,parswt,types);
				test += "}\n";
				if(i!=obj.size()-1)
					test += ",";
			}
		}

	}
	test += "};\n";
	//intff += "return ret;}\n}\n";
	//intff += intf;
	//intf = (inc + intf + intff);
	//inc += "}";
	//writeTofile("/home/sumeet/server/web/default/_afc_Interfaces.js",test,true);
	infjs += test;
	//writeTofile("/home/sumeet/workspace/inter/AjaxInterface.cpp",inc,false);
	return inc;
	//cout << test << flush;
}

string AfcUtil::updateAjaxInterface(strVec emp,string claz,string pars,string parswt,string types)
{
	string test;
	string retType = emp.at(0);
	string funcName = emp.at(1);
	//test = "#include \"" + claz + ".h\"\n#include <boost/lexical_cast.hpp>\n\n";
	test += ("string invokeAjaxMethodFor" + claz + funcName + "(strVec _inp");
	test += ")\n{\n" + types;
	test += (claz + " _obj;\n");
	if(retType=="void")
	{
		test += "return \"\";\n}\n";
	}
	else if(retType=="string" || retType=="int" || retType=="float" || retType=="double")
	{
		test += "return _obj."+funcName+"("+pars+");\n}\n";
	}
	else
		test += "return from"+retType+"ToJSON(_obj."+funcName+"("+pars+"));\n}\n";
	cout << test << flush;

	return test;
}

string AfcUtil::execute(HttpRequest req)
{
	cout << "\ninside executeAFC::" << flush;
	strVec vemp;
	string methName = req.getRequestParam("method");
	string claz = req.getRequestParam("claz");
	string temp = req.getRequestParam("paramsize");
	int paramSize = boost::lexical_cast<int>(temp.c_str());
	cout << "\nreading params::" << flush;
	for(int i=0;i<paramSize;i++)
	{
		stringstream s;
		string ss;
		s << (i+1);
		s >> ss;
		ss = "param_" + ss;
		cout << ss << flush;
		string tem = req.getRequestParam(ss);
		vemp.push_back(tem);
	}
	string libName = "libinter.so";
	void *dlib = dlopen(libName.c_str(), RTLD_NOW);
	if(dlib == NULL){
	 cerr << dlerror() << endl;
	 exit(-1);
	}
	string funcName;
	string metn,re;
	metn = "invokeAjaxMethodFor"+claz+methName;
	void *mkr = dlsym(dlib, metn.c_str());
	if(mkr!=NULL)
	{
		typedef string (*Funptr2) (strVec);
		Funptr2 f2 = (Funptr2)mkr;
		cout << "\ncalling method: " << metn << flush;
		re = f2(vemp);
		cout << "\nend of executeAFC::" << re << flush;
	}
	return re;
}


void AfcUtil::writeTofile(string fileName,string data,bool trunc)
{
	ofstream myfile1;
	if(trunc)
		myfile1.open(fileName.c_str(),ios::trunc | ios::binary);
	else
		myfile1.open(fileName.c_str(),ios::app | ios::binary);
	if (myfile1.is_open())
	{
		myfile1.write(data.c_str(),data.length());
		myfile1.close();
	}
}


string AfcUtil::camelCased(const string& s)
{
	string str(s);
	str[0] = toupper(str[0]);
	return str;
}

string AfcUtil::reverseCamelCased(const string& s)
{
	string str(s);
	str[0] = tolower(str[0]);
	return str;
}
