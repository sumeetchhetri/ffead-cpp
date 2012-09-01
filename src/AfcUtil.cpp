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

Logger AfcUtil::logger;

AfcUtil::AfcUtil() {
	logger = Logger::getLogger("AfcUtil");
}

AfcUtil::~AfcUtil() {
	// TODO Auto-generated destructor stub
}
static map<string,string> doneMap;
string AfcUtil::generateJsObjectsAll(vector<string> obj,strVec files,vector<bool> stat,string &headers,string &objs,string &infjs,vector<string> pv)
{
	Reflection ref;
	string ret="#include \"AfcInclude.h\"\n\nextern \"C\"\n{\n";
	headers = "#include \"CastUtil.h\"\n#include \"JSONUtil.h\"\n#include \"sstream\"\n#include \"CastUtil.h\"\n";
	if(obj.size()==0)return ret + "}";
	//ret += "\nconst mValue& find_value(const mObject& obj, const string& name)\n{\nmObject::const_iterator i = obj.find(name);\nreturn i->second;\n}\n";
	for (unsigned int var = 0; var < obj.size(); ++var)
	{
		if(stat.at(var))
		{
			strVec pinfo;
			strVec info = ref.getAfcObjectData(obj.at(var)+files.at(var)+".h", true,pinfo);
			ret += generateJsObjects(info,files.at(var),headers,obj.at(var),objs,pinfo);
		}
		else
		{
			//logger << "=============" << obj.at(var)+files.at(var)+".h" << flush;
			strVec pinfo;
			strVec info = ref.getAfcObjectData(obj.at(var)+files.at(var)+".h", false,pinfo);
			ret += generateJsInterfaces(info,files.at(var),headers,obj.at(var),infjs,pv.at(var));
		}
	}
	//headers += "\nusing namespace json_spirit;\n";
	ret = (ret + "\n}\n");
	//writeTofile("/home/sumeet/workspace/inter/AfcInclude.h",headers,true);
	return ret;
}

string AfcUtil::generateJsObjects(strVec obj,string claz,string &headers,string path,string &objs,strVec pobj)
{
	if(doneMap.find(claz)==doneMap.end())
		doneMap[claz] = "done";
	else
		return "";
	string tes,fres;
	Reflection ref;
	string retu;
	string test = "function _"+claz+"()\n{\n";
	tes += "JSONElement* _node;\n";
	fres = "string json=\"{";
	bool priv = false;
	if(obj.size()==0 && pobj.size()>0)
	{
		obj = pobj;
		priv = true;
	}
	for(unsigned int i=0;i<obj.size();i++)
	{
		strVec vemp;
		string data = obj.at(i);
		StringUtil::replaceFirst(data,";","");
		StringUtil::split(vemp, data, (" "));
		if(vemp.size()<2)
		{
			logger << data << " error" <<  endl;
			continue;
		}
		test += "this." + vemp.at(1) + "= null;\n";
		if(vemp.at(0)=="int" || vemp.at(0)=="double" || vemp.at(0)=="float" || vemp.at(0)=="string")
		{
			tes += "_node = obj.getNode(\""+vemp.at(1)+"\");\n";
			if(!priv)tes += "if(_node!=NULL)_obj." + vemp.at(1) + "= CastUtil::lexical_cast<"+vemp.at(0)+">(_node->getValue());\n";
			else tes += "if(_node!=NULL)_obj.set"+camelCased(vemp.at(1))+"(CastUtil::lexical_cast<"+vemp.at(0)+">(_node->getValue()));\n";
		}
		else
		{
			tes += "_node = obj.getNode(\""+vemp.at(1)+"\");\n";
			if(!priv)tes += "if(_node!=NULL)_obj." + vemp.at(1) + "= read"+vemp.at(0)+"(*_node);\n";
			else tes += "if(_node!=NULL)_obj.set"+camelCased(vemp.at(1))+"(read"+vemp.at(0)+"(*_node));\n";
		}
		if(vemp.at(0)=="int" || vemp.at(0)=="float" || vemp.at(0)=="double" || vemp.at(0)=="string")
		{
			if(vemp.at(0)=="string")
			{
				if(!priv)fres += "\\\""+vemp.at(1)+"\\\" : \\\"\"+_obj."+vemp.at(1)+"+\"\\\"";
				else fres += "\\\""+vemp.at(1)+"\\\" : \\\"\"+_obj.get"+camelCased(vemp.at(1))+"()+\"\\\"";
			}
			else
			{
				if(!priv)fres += "\\\""+vemp.at(1)+"\\\" : \\\"\"+CastUtil::lexical_cast<string>(_obj."+vemp.at(1)+")+\"\\\"";
				else fres += "\\\""+vemp.at(1)+"\\\" : \\\"\"+CastUtil::lexical_cast<string>(_obj.get"+camelCased(vemp.at(1))+"())+\"\\\"";
			}
			if(i!=obj.size()-1)
			{
				fres += ",";
			}
		}
		else
		{
			strVec pppinfo;
			strVec info = ref.getAfcObjectData(path+vemp.at(0)+".h", true, pppinfo);
			retu += generateJsObjects(info,vemp.at(0),headers,path,objs,pppinfo);
			if(!priv)fres += "\\\""+vemp.at(1)+"\\\" : \\\"\"+from"+vemp.at(0)+"ToJSON(_obj."+vemp.at(1)+")+\"\\\"";
			else fres += "\\\""+vemp.at(1)+"\\\" : \\\"\"+from"+vemp.at(0)+"ToJSON(_obj.get."+camelCased(vemp.at(1))+"())+\"\\\"";
			if(i!=obj.size()-1)
			{
				fres += ",";
			}
		}
	}
	fres += "}\";";
	logger << fres << flush;
	test += "}";
	logger << test << flush;
	//writeTofile("/home/sumeet/server/web/default/_afc_Objects.js",test,true);
	objs += test;
	headers += "#include \""+claz+".h\"\n";

	//test = "\nconst mValue& find_value(const mObject& obj, const string& name)\n{\nmObject::const_iterator i = obj.find(name);\nreturn i->second;\n}\n";
	test = retu+ "\n\n" + claz + " read"+claz+"(JSONElement& obj)\n{\n"+claz+" _obj;\n";
	/*for(unsigned int i=0;i<obj.size();i++)
	{
		strVec vemp;
		string data = obj.at(i);
		StringUtil::replaceFirst(data,";","");
		StringUtil::split(vemp, data, (" "));
		if(vemp.at(0)=="float")
		{
			test += "double te_ = find_value( obj, \""+vemp.at(1)+"\" ).get_value<double>();\n";
			test += "_obj."+vemp.at(1)+" = CastUtil::lexical_cast<"+vemp.at(0)+">(te_);";
		}
		else
			test += "_obj." + vemp.at(1) + "= find_value( obj, \""+vemp.at(1)+"\" ).get_value<"+vemp.at(0)+">();\n";
	}*/
	test += tes + "\nreturn _obj;\n}\n";
	test += claz + " to"+claz+"(string s)\n{\nJSONElement element = JSONUtil::getDocument(s);\n"+ claz +" _obj = read"+claz+"(element);\nreturn _obj;\n}\n";
	test += "void* toVoidP"+claz+"(string s)\n{\nJSONElement element = JSONUtil::getDocument(s);\n"+ claz +" *_obj = new "+claz+";\n*_obj = read"+claz+"(element);\nreturn _obj;\n}\n";
	test += "\nstring from"+claz+"ToJSON("+claz+" _obj)\n{\n"+fres+"\nreturn json;\n}\n";
	return test;
}

string AfcUtil::generateJsInterfaces(strVec obj,string claz,string &headers,string path,string &infjs,string pv)
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
		StringUtil::replaceFirst(data,";","");
		vector<string> delimiters;
		delimiters.push_back(" ");
		delimiters.push_back(",");
		delimiters.push_back(")");
		delimiters.push_back("(");
		StringUtil::split(vemp, data, delimiters);
		for(unsigned int k=0;k<vemp.size();k++)
		{
			if(vemp.at(k)!="")
				emp.push_back(vemp.at(k));
			//logger << vemp.at(i) << "\n" << flush;
		}
		//logger << "\n------------------------------------\n" << flush;
		if(emp.size()>1)
		{
			size_t te = emp.at(1).find("~");
			if(te==string::npos)
			{
				string pars,parswt,types,jsonstr;
				if(emp.size()==2)
				{
					test += emp.at(1) + ": function(_cb,_url,_cntxt){\n";
					test += "AfcCall(\""+claz+"\",\""+emp.at(1)+"\",new Array("+jsonstr+"),_cb,(_url==null?\""+pv+"\":_url),_cntxt);\n";
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
							types.append("=CastUtil::lexical_cast<");
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
						//logger << vemp.at(i) << "\n" << flush;
					}
					fl = true;
					test += ",_cb,_url,_cntxt){\n";
					test += "AfcCall(\""+claz+"\",\""+emp.at(1)+"\",new Array("+jsonstr+"),_cb,(_url==null?\""+pv+"\":_url),_cntxt);\n";
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
	//logger << test << flush;
}

string AfcUtil::updateAjaxInterface(strVec emp,string claz,string pars,string parswt,string types)
{
	string test;
	string retType = emp.at(0);
	string funcName = emp.at(1);
	//test = "#include \"" + claz + ".h\"\n#include "CastUtil.h"\n\n";
	test += ("string invokeAjaxMethodFor" + claz + funcName + "(strVec _inp");
	test += ")\n{\n" + types;
	test += (claz + " _obj;\n");
	if(retType=="void")
	{
		test += "return \"\";\n}\n";
	}
	else if(retType=="string")
	{
		test += "return _obj."+funcName+"("+pars+");\n}\n";
	}
	else if(retType=="int" || retType=="float" || retType=="double")
	{
		test += "string outp = CastUtil::lexical_cast<string>(_obj."+funcName+"("+pars+"));\nreturn outp;\n}\n";
	}
	else
		test += "return from"+retType+"ToJSON(_obj."+funcName+"("+pars+"));\n}\n";
	logger << test << flush;

	return test;
}

string AfcUtil::execute(HttpRequest req)
{
	logger << "\ninside executeAFC::" << flush;
	strVec vemp;
	string methName = req.getRequestParam("method");
	string claz = req.getRequestParam("claz");
	string temp = req.getRequestParam("paramsize");
	int paramSize = CastUtil::lexical_cast<int>(temp.c_str());
	logger << "\nreading params::" << flush;
	for(int i=0;i<paramSize;i++)
	{
		stringstream s;
		string ss;
		s << (i+1);
		s >> ss;
		ss = "param_" + ss;
		logger << ss << flush;
		string tem = req.getRequestParam(ss);
		vemp.push_back(tem);
	}
	string libName = Constants::INTER_LIB_FILE;
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
		logger << "\ncalling method: " << metn << flush;
		re = f2(vemp);
		logger << "\nend of executeAFC::" << re << flush;
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
