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
string AfcUtil::generateJsObjectsAll(vector<string> obj,strVec files,vector<bool> stat,string &headers,string &objs,string &infjs,vector<string> pv,
		string& ajaxret, string& typrefs,map<string, string> ajintpthMap)
{
	Reflection ref;
	string ttem = "Date readDate(JSONElement& obj){DateFormat formt(\"yyyy-mm-dd hh:mi:ss\");\nreturn *formt.parse(obj.getValue());}";
	ttem += "Date* readDateP(JSONElement& obj){DateFormat formt(\"yyyy-mm-dd hh:mi:ss\");\nreturn formt.parse(obj.getValue());}";
	ttem += "\nstring fromDateToJSON(Date d){DateFormat formt(\"yyyy-mm-dd hh:mi:ss\");\nreturn formt.format(d);}";
	ttem += "\nstring fromDateVPToJSON(Date *d){DateFormat formt(\"yyyy-mm-dd hh:mi:ss\");\nreturn formt.format(*d);}";
	ttem += "BinaryData readBinaryData(JSONElement& obj){BinaryData bd;return bd;}";
	ttem += "BinaryData* readBinaryDataP(JSONElement& obj){return new BinaryData;}";
	ttem += "\nstring fromBinaryDataToJSON(BinaryData d){return \"\";}";
	ttem += "\nstring fromBinaryDataVPToJSON(BinaryData *d){return \"\";}";
	ajaxret = ttem + typrefs + ajaxret;
	string ret ="#include \"AfcInclude.h\"\n\nextern \"C\"\n{\n" + ajaxret;
	headers += "#include \"CastUtil.h\"\n#include \"JSONUtil.h\"\n#include \"sstream\"\n#include <algorithm>\n#include \"JSONSerialize.h\"\n";
	headers += "#include \"set\"\n#include \"list\"\n#include \"queue\"\n#include \"deque\"\n#include \"DateFormat.h\"\n";
	if(obj.size()==0)return ret + "}";
	//ret += "\nconst mValue& find_value(const mObject& obj, const string& name)\n{\nmObject::const_iterator i = obj.find(name);\nreturn i->second;\n}\n";
	for (unsigned int var = 0; var < obj.size(); ++var)
	{
		/*if(stat.at(var))
		{
			strVec pinfo;
			bool isOpForSet = false;
			strVec info = ref.getAfcObjectData(obj.at(var)+files.at(var)+".h", true,pinfo,isOpForSet);
			ret += generateJsObjects(info,files.at(var),headers,obj.at(var),objs,pinfo,isOpForSet);
		}
		else*/
		if(!stat.at(var))
		{
			//logger << "=============" << obj.at(var)+files.at(var)+".h" << flush;
			strVec pinfo;
			bool isOpForSet = false;
			strVec info = ref.getAfcObjectData(obj.at(var)+files.at(var)+".h", false,pinfo,isOpForSet);
			ret += generateJsInterfaces(info,files.at(var),headers,obj.at(var),infjs,pv.at(var),ajintpthMap);
		}
	}
	//headers += "\nusing namespace json_spirit;\n";
	ret = (ret + "\n}\n");
	//writeTofile("/home/sumeet/workspace/inter/AfcInclude.h",headers,true);
	return ret;
}

string AfcUtil::generateJsObjects(strVec obj,string claz,string &headers,string path,string &objs,strVec pobj, bool isOpForSet, string& typrefs,strVec minfo)
{
	if(doneMap.find(claz)==doneMap.end())
		doneMap[claz] = "done";
	else
		return "";
	string tes,tes1,fres,fres1;
	Reflection ref;
	string retu;
	string test = "function _"+claz+"()\n{\n";
	tes += "JSONElement* _node;\n";
	tes1 += "JSONElement* _node;\n";
	fres = "string json=\"{\";\n";
	fres1 = "string json=\"{\";\n";
	bool priv = false;
	size_t tess;
	string meth;
	strVec fldnames;
	map<string, int> fldstat;
	for (unsigned int i = 0; i < pobj.size(); i++)
	{
		if(((tess=pobj.at(i).find("("))==string::npos && (tess=pobj.at(i).find(")"))==string::npos && pobj.at(i).find("~")==string::npos))
		{
			string fld = pobj.at(i);
			StringUtil::replaceFirst(fld,";","");
			RegexUtil::replace(fld, "[\t]+", " ");
			RegexUtil::replace(fld, "[ ]+", " ");
			strVec fldp;
			StringUtil::split(fldp, fld, (" "));
			if(fldp.size()==2)
			{
				string nam = fldp.at(1);
				fldnames.push_back(fldp.at(0));
				fldnames.push_back(nam);
				fldstat[nam] = 0;
			}
		}
	}
	for(unsigned int i=0;i<minfo.size();i++)
	{
		if((tess=minfo.at(i).find("("))!=string::npos && (tess=minfo.at(i).find(")"))!=string::npos && minfo.at(i).find("~")==string::npos
							&& fldnames.size()>0)
		{
			meth = minfo.at(i);
			StringUtil::replaceFirst(meth,";","");
			RegexUtil::replace(meth, "[\t]+", " ");
			RegexUtil::replace(meth, "[ ]+", " ");
			RegexUtil::replace(meth, "[ ?, ?]+", ",");
			//bool ptr = false;
			if(meth.find("*")!=string::npos)
			{
				//ptr = true;
				StringUtil::replaceFirst(meth,"*","");
			}

			string argts = meth.substr(meth.find("("),meth.find(")")-meth.find("("));
			StringUtil::replaceFirst(argts,"(","");
			StringUtil::replaceAll(argts,")","");
			meth = meth.substr(0,meth.find("("));

			if(meth.find("operator")!=string::npos)
			{
			}
			else
			{
				strVec methp,methpm,argp,argpm;
				StringUtil::split(argp, argts, (","));
				StringUtil::split(methp, meth, (" "));
				for(unsigned int j = 0; j < methp.size(); j++)
				{
					if(methp.at(j)!="")
						methpm.push_back(methp.at(j));
				}
				for(unsigned int j = 0; j < argp.size(); j++)
				{
					if(argp.at(j)!="" && argp.at(j)!="(")
					{
						string tty = argp.at(j);
						StringUtil::trim(tty);
						if(tty.find(" ")!=string::npos)
						{
							vector<string> temargt = StringUtil::split(tty, " ");
							argpm.push_back(temargt.at(0));
						}
						else
						{
							argpm.push_back(tty);
						}
					}
				}

				if(methpm.at(0)!=claz)
				{
					for(unsigned int k = 0; k < fldnames.size(); k=k+2)
					{
						string cam = AfcUtil::camelCased(fldnames.at(k+1));
						if("set"+cam==methpm.at(1) && argpm.size()==1 && argpm.at(0)==fldnames.at(k) && methpm.at(0)=="void")
						{
							fldstat[fldnames.at(k+1)]++;
						}
						else if("get"+cam==methpm.at(1) && argpm.size()==0 && methpm.at(0)==fldnames.at(k))
						{
							fldstat[fldnames.at(k+1)]++;
						}
					}
				}
			}
		}
	}
	pobj.clear();
	for(unsigned int k = 0; k < fldnames.size(); k=k+2)
	{
		if(fldstat[fldnames.at(k+1)]==2)
		{
			string data = fldnames.at(k) + " " + fldnames.at(k+1) + ";";
			//logger << "===========> " << data << endl;
			pobj.push_back(data);
		}
	}
	for(unsigned int i=0;i<pobj.size();i++)
	{
		obj.push_back(pobj.at(i));
	}
	strVec tobj;
	for(unsigned int i=0;i<obj.size();i++)
	{
		strVec vemp;
		string data = obj.at(i);
		//bool ptr = false;
		if(data.find("*")!=string::npos)
		{
			//ptr = true;
			StringUtil::replaceFirst(data,"*","");
		}
		StringUtil::replaceFirst(data,";","");
		StringUtil::split(vemp, data, (" "));
		if(vemp.size()<2)
		{
			//logger << data << " error" <<  endl;
		}
		else
		{
			tobj.push_back(obj.at(i));
		}
	}
	obj = tobj;
	for(unsigned int i=0;i<obj.size();i++)
	{
		strVec vemp;
		string data = obj.at(i);
		bool ptr = false;
		if(data.find("*")!=string::npos)
		{
			ptr = true;
			StringUtil::replaceFirst(data,"*","");
		}
		StringUtil::replaceFirst(data,";","");
		StringUtil::split(vemp, data, (" "));
		if(vemp.size()<2)
		{
			//logger << data << " error" <<  endl;
			continue;
		}

		priv = (fldstat[vemp.at(1)]==2?true:false);

		test += "this." + vemp.at(1) + "= null;\n";
		tes += generateReadObjects(vemp.at(0), vemp.at(1), priv, ptr, ".");
		tes1 += generateReadObjects(vemp.at(0), vemp.at(1), priv, ptr, "->");
		fres += generateToJSONObjects(vemp.at(0), vemp.at(1), priv, obj, i, retu, headers, path, objs, ".", ptr);
		fres1 += generateToJSONObjects(vemp.at(0), vemp.at(1), priv, obj, i, retu, headers, path, objs, "->", ptr);
	}

	/*for(unsigned int i=0;i<pobj.size();i++)
	{
		priv = true;
		strVec vemp;
		string data = pobj.at(i);
		bool ptr = false;
		if(data.find("*")!=string::npos)
		{
			ptr = true;
			StringUtil::replaceFirst(data,"*","");
		}
		StringUtil::replaceFirst(data,";","");
		StringUtil::split(vemp, data, (" "));
		if(vemp.size()<2)
		{
			logger << data << " error" <<  endl;
			continue;
		}

		test += "this." + vemp.at(1) + "= null;\n";
		tes += generateReadObjects(vemp.at(0), vemp.at(1), priv, ptr, ".");
		tes1 += generateReadObjects(vemp.at(0), vemp.at(1), priv, ptr, "->");
		fres += generateToJSONObjects(vemp.at(0), vemp.at(1), priv, pobj, i, retu, headers, path, objs, ".", ptr);
		fres1 += generateToJSONObjects(vemp.at(0), vemp.at(1), priv, pobj, i, retu, headers, path, objs, "->", ptr);
	}*/
	fres += "\njson += \"}\";";
	fres1 += "\njson += \"}\";";
	//logger << fres << flush;
	test += "}";
	//logger << test << flush;
	objs += test;
	headers += "#include \""+claz+".h\"\n";

	typrefs += claz + " read"+claz+"(JSONElement& obj);\n" + claz + "* read"+claz+"P(JSONElement& obj);\n";
	typrefs += "\nstring from"+claz+"ToJSON("+claz+" _obj);\nstring from"+claz+"VPToJSON("+claz+"* _obj);\n";
	typrefs	+= claz + " to"+claz+"(string s);\n" + claz + "* to"+claz+"P(string s);\n";
	typrefs	+= "vector<"+claz + "> to"+claz+"Vec(string s);\nvector<" + claz + ">* to"+claz+"VecVP(string s);\n";
	typrefs	+= "list<"+claz + "> to"+claz+"Lis(string s);\nlist<" + claz + ">* to"+claz+"LisVP(string s);\n";
	typrefs	+= "deque<"+claz + "> to"+claz+"Dq(string s);\ndeque<" + claz + ">* to"+claz+"DqVP(string s);\n";
	typrefs	+= "std::queue<"+claz + "> to"+claz+"Q(string s);\nstd::queue<" + claz + ">* to"+claz+"QVP(string s);\n";
	if(isOpForSet)
	{
		typrefs	+= "set<"+claz + "> to"+claz+"Set(string s);\nset<" + claz + ">* to"+claz+"SetVP(string s);\n";
		typrefs	+= "multiset<"+claz + "> to"+claz+"MulSet(string s);\nmultiset<" + claz + ">* to"+claz+"MulSetVP(string s);\n";
	}
	typrefs += "\nstring from"+claz+"VecToJSON(vector<"+claz+"> _obj);\nstring from"+claz+"VecVPToJSON(vector<"+claz+">* _obj);\n";
	typrefs += "\nstring from"+claz+"LisToJSON(list<"+claz+"> _obj);\nstring from"+claz+"LisVPToJSON(list<"+claz+">* _obj);\n";
	typrefs += "\nstring from"+claz+"DqToJSON(deque<"+claz+"> _obj);\nstring from"+claz+"DqVPToJSON(deque<"+claz+">* _obj);\n";
	typrefs += "\nstring from"+claz+"QToJSON(std::queue<"+claz+"> _obj);\nstring from"+claz+"QVPToJSON(std::queue<"+claz+">* _obj);\n";
	if(isOpForSet)
	{
		typrefs += "\nstring from"+claz+"SetToJSON(set<"+claz+"> _obj);\nstring from"+claz+"SetVPToJSON(set<"+claz+">* _obj);\n";
		typrefs += "\nstring from"+claz+"MulSetToJSON(multiset<"+claz+"> _obj);\nstring from"+claz+"MulSetVPToJSON(multiset<"+claz+">* _obj);\n";
	}

	test = retu+ "\n\n" + claz + " read"+claz+"(JSONElement& obj)\n{\n"+claz+" _obj;\n";
	test += tes + "\nreturn _obj;\n}\n";
	test += "\n\n" + claz + "* read"+claz+"P(JSONElement& obj)\n{\n"+claz+"* _obj = new "+claz+";\n";
	test += tes1 + "\nreturn _obj;\n}\n";
	test += claz + " to"+claz+"(string s)\n{\nJSONElement element = JSONUtil::getDocument(s);\n"+ claz +" _obj = read"+claz+"(element);\nreturn _obj;\n}\n";
	test += "vector<"+claz+"> to"+claz+"Vec(string s)\n{\nJSONElement element = JSONUtil::getDocument(s);\n" +
			"vector<"+claz+"> vec;\nfor(int i=0;i<(int)element.getChildren().size();i++){\n" +
			claz +" _obj = read"+claz+"(*element.getChildren().at(i));\nvec.push_back(_obj);\n" +
			"}\nreturn vec;\n}\n";
	test += "list<"+claz+"> to"+claz+"Lis(string s)\n{\nvector<"+claz+"> vec = to"+claz+"Vec(s);\n" +
			"list<"+claz+"> tt;\nstd::copy(vec.begin(), vec.end(), std::back_inserter(tt));\n" +
			"return tt;\n}\n";
	if(isOpForSet)
	{
		test += "set<"+claz+"> to"+claz+"Set(string s)\n{\nvector<"+claz+"> vec = to"+claz+"Vec(s);\n" +
				"set<"+claz+"> tt;\nstd::copy(vec.begin(), vec.end(), std::inserter(tt, tt.begin()));\n" +
				"return tt;\n}\n";
		test += "multiset<"+claz+"> to"+claz+"MulSet(string s)\n{\nvector<"+claz+"> vec = to"+claz+"Vec(s);\n" +
				"multiset<"+claz+"> tt;\nstd::copy(vec.begin(), vec.end(), std::inserter(tt, tt.begin()));\n" +
				"return tt;\n}\n";
	}
	test += "deque<"+claz+"> to"+claz+"Dq(string s)\n{\nvector<"+claz+"> vec = to"+claz+"Vec(s);\n" +
			"deque<"+claz+"> tt;\nstd::copy(vec.begin(), vec.end(), std::inserter(tt, tt.begin()));\n" +
			"return tt;\n}\n";
	test += "std::queue<"+claz+"> to"+claz+"Q(string s)\n{\nvector<"+claz+"> vec = to"+claz+"Vec(s);\n" +
			"std::queue<"+claz+"> tt;for (int var = 0; var < (int)vec.size(); ++var) {\ntt.push(vec.at(var));\n}\n" +
			"return tt;\n}\n";
	test += "vector<"+claz+">* to"+claz+"VecVP(string s)\n{\nJSONElement element = JSONUtil::getDocument(s);\n" +
			"vector<"+claz+">* vec = new vector<"+claz+">;\nfor(int i=0;i<element.getChildren().size();i++){\n" +
			claz +" _obj = read"+claz+"(*element.getChildren().at(i));\nvec->push_back(_obj);\n" +
			"}\nreturn vec;\n}\n";
	test += "list<"+claz+">* to"+claz+"LisVP(string s)\n{\nvector<"+claz+"> vec = to"+claz+"Vec(s);\n" +
			"list<"+claz+">* tt = new list<"+claz+">;\nstd::copy(vec.begin(), vec.end(), std::back_inserter(*tt));\n" +
			"return tt;\n}\n";
	if(isOpForSet)
	{
		test += "set<"+claz+">* to"+claz+"SetVP(string s)\n{\nvector<"+claz+"> vec = to"+claz+"Vec(s);\n" +
			"set<"+claz+">* tt = new set<"+claz+">;\nstd::copy(vec.begin(), vec.end(), std::inserter(*tt, tt->begin()));\n" +
			"return tt;\n}\n";
		test += "multiset<"+claz+">* to"+claz+"MulSetVP(string s)\n{\nvector<"+claz+"> vec = to"+claz+"Vec(s);\n" +
				"multiset<"+claz+">* tt = new multiset<"+claz+">;\nstd::copy(vec.begin(), vec.end(), std::inserter(*tt, tt->begin()));\n" +
				"return tt;\n}\n";
	}
	test += "deque<"+claz+">* to"+claz+"DqVP(string s)\n{\nvector<"+claz+"> vec = to"+claz+"Vec(s);\n" +
			"deque<"+claz+">* tt = new deque<"+claz+">;\nstd::copy(vec.begin(), vec.end(), std::inserter(*tt, tt->begin()));\n" +
			"return tt;\n}\n";
	test += "std::queue<"+claz+">* to"+claz+"QVP(string s)\n{\nvector<"+claz+"> vec = to"+claz+"Vec(s);\n" +
			"std::queue<"+claz+">* tt = new std::queue<"+claz+">;for (int var = 0; var < (int)vec.size(); ++var) {\ntt->push(vec.at(var));\n}\n" +
			"return tt;\n}\n";
	test += "void* toVoidP"+claz+"(string s)\n{\nJSONElement element = JSONUtil::getDocument(s);\n"+ claz +" *_obj = new "+claz+";\n*_obj = read"+claz+"(element);\nreturn _obj;\n}\n";
	test += "\nstring from"+claz+"ToJSON("+claz+" _obj)\n{\n"+fres+"\nreturn json;\n}\n";
	test += "\nstring from"+claz+"VPToJSON("+claz+"* _obj)\n{\n"+fres1+"\nreturn json;\n}\n";
	test += "\nstring from"+claz+"VecToJSON(vector<"+claz+"> _vecobj)\n{\nstring json = \"[\";\nfor(int i=0;i<(int)_vecobj.size();i++){\n" +
			"json += from"+claz+"ToJSON(_vecobj.at(i));\n" +
			"if(i!=(int)_vecobj.size()-1)json += \",\";\n" +
			"\n}\njson += \"]\";\nreturn json;\n}\n";
	test += "\nstring from"+claz+"LisToJSON(list<"+claz+"> _lisobj)\n{\nvector<"+claz+"> _vecobj;\nstd::copy(_lisobj.begin(), _lisobj.end(), std::back_inserter(_vecobj));" +
			"\nstring json = \"[\";\nfor(int i=0;i<(int)_vecobj.size();i++){\n" +
			"json += from"+claz+"ToJSON(_vecobj.at(i));\n" +
			"if(i!=(int)_vecobj.size()-1)json += \",\";\n" +
			"\n}\njson += \"]\";\nreturn json;\n}\n";
	if(isOpForSet)
	{
		test += "\nstring from"+claz+"SetToJSON(set<"+claz+"> _lisobj)\n{\nvector<"+claz+"> _vecobj;\nstd::copy(_lisobj.begin(), _lisobj.end(), std::back_inserter(_vecobj));" +
				"\nstring json = \"[\";\nfor(int i=0;i<(int)_vecobj.size();i++){\n" +
				"json += from"+claz+"ToJSON(_vecobj.at(i));\n" +
				"if(i!=(int)_vecobj.size()-1)json += \",\";\n" +
				"\n}\njson += \"]\";\nreturn json;\n}\n";
		test += "\nstring from"+claz+"MulSetToJSON(multiset<"+claz+"> _lisobj)\n{\nvector<"+claz+"> _vecobj;\nstd::copy(_lisobj.begin(), _lisobj.end(), std::back_inserter(_vecobj));" +
				"\nstring json = \"[\";\nfor(int i=0;i<(int)_vecobj.size();i++){\n" +
				"json += from"+claz+"ToJSON(_vecobj.at(i));\n" +
				"if(i!=(int)_vecobj.size()-1)json += \",\";\n" +
				"\n}\njson += \"]\";\nreturn json;\n}\n";
	}
	test += "\nstring from"+claz+"DqToJSON(deque<"+claz+"> _lisobj)\n{\nvector<"+claz+"> _vecobj;\nstd::copy(_lisobj.begin(), _lisobj.end(), std::back_inserter(_vecobj));" +
			"\nstring json = \"[\";\nfor(int i=0;i<(int)_vecobj.size();i++){\n" +
			"json += from"+claz+"ToJSON(_vecobj.at(i));\n" +
			"if(i!=(int)_vecobj.size()-1)json += \",\";\n" +
			"\n}\njson += \"]\";\nreturn json;\n}\n";
	test += "\nstring from"+claz+"QToJSON(std::queue<"+claz+"> _lisobj)\n{\nvector<"+claz+"> _vecobj;" +
			"std::queue<"+claz+"> qq = _lisobj;\nfor (int var = 0; var < (int)qq.size(); ++var)\n{" +
			"_vecobj.push_back(qq.front());\nqq.pop();\n}\n" +
			"\nstring json = \"[\";\nfor(int i=0;i<(int)_vecobj.size();i++){\n" +
			"json += from"+claz+"ToJSON(_vecobj.at(i));\n" +
			"if(i!=(int)_vecobj.size()-1)json += \",\";\n" +
			"\n}\njson += \"]\";\nreturn json;\n}\n";
	test += "\nstring from"+claz+"VecVPToJSON(vector<"+claz+">* _vecobj)\n{\nstring json = \"[\";\nfor(int i=0;i<_vecobj->size();i++){\n" +
			"json += from"+claz+"ToJSON(_vecobj->at(i));\n" +
			"if(i!=_vecobj->size()-1)json += \",\";\n" +
			"\n}\njson += \"]\";\nreturn json;\n}\n";
	test += "\nstring from"+claz+"LisVPToJSON(list<"+claz+">* _lisobj)\n{\nvector<"+claz+"> _vecobj;\nstd::copy(_lisobj->begin(), _lisobj->end(), std::back_inserter(_vecobj));" +
			"\nstring json = \"[\";\nfor(int i=0;i<(int)_vecobj.size();i++){\n" +
			"json += from"+claz+"ToJSON(_vecobj.at(i));\n" +
			"if(i!=(int)_vecobj.size()-1)json += \",\";\n" +
			"\n}\njson += \"]\";\nreturn json;\n}\n";
	if(isOpForSet)
	{
		test += "\nstring from"+claz+"SetVPToJSON(set<"+claz+">* _lisobj)\n{\nvector<"+claz+"> _vecobj;\nstd::copy(_lisobj->begin(), _lisobj->end(), std::back_inserter(_vecobj));" +
				"\nstring json = \"[\";\nfor(int i=0;i<(int)_vecobj.size();i++){\n" +
				"json += from"+claz+"ToJSON(_vecobj.at(i));\n" +
				"if(i!=(int)_vecobj.size()-1)json += \",\";\n" +
				"\n}\njson += \"]\";\nreturn json;\n}\n";
		test += "\nstring from"+claz+"MulSetVPToJSON(multiset<"+claz+">* _lisobj)\n{\nvector<"+claz+"> _vecobj;\nstd::copy(_lisobj->begin(), _lisobj->end(), std::back_inserter(_vecobj));" +
				"\nstring json = \"[\";\nfor(int i=0;i<(int)_vecobj.size();i++){\n" +
				"json += from"+claz+"ToJSON(_vecobj.at(i));\n" +
				"if(i!=(int)_vecobj.size()-1)json += \",\";\n" +
				"\n}\njson += \"]\";\nreturn json;\n}\n";
	}
	test += "\nstring from"+claz+"DqVPToJSON(deque<"+claz+">* _lisobj)\n{\nvector<"+claz+"> _vecobj;\nstd::copy(_lisobj->begin(), _lisobj->end(), std::back_inserter(_vecobj));" +
			"\nstring json = \"[\";\nfor(int i=0;i<(int)_vecobj.size();i++){\n" +
			"json += from"+claz+"ToJSON(_vecobj.at(i));\n" +
			"if(i!=(int)_vecobj.size()-1)json += \",\";\n" +
			"\n}\njson += \"]\";\nreturn json;\n}\n";
	test += "\nstring from"+claz+"QVPToJSON(std::queue<"+claz+">* _lisobj)\n{\nvector<"+claz+"> _vecobj;\n" +
			"std::queue<"+claz+"> qq = *_lisobj;\nfor (int var = 0; var < (int)qq.size(); ++var)\n{" +
			"_vecobj.push_back(qq.front());\nqq.pop();\n}\n" +
			"\nstring json = \"[\";\nfor(int i=0;i<(int)_vecobj.size();i++){\n" +
			"json += from"+claz+"ToJSON(_vecobj.at(i));\n" +
			"if(i!=(int)_vecobj.size()-1)json += \",\";\n" +
			"\n}\njson += \"]\";\nreturn json;\n}\n";
	return test;
}


string AfcUtil::generateReadObjects(string type, string name, bool priv, bool ptr, string typ)
{
	string tes;
	if(type=="int" || type=="short" || type=="long" || type=="double" || type=="float" || type=="string" || type=="bool")
	{
		tes += "_node = obj.getNode(\""+name+"\");\n";
		if(!ptr)
		{
			if(!priv)tes += "if(_node!=NULL)_obj"+typ + name + "= CastUtil::lexical_cast<"+type+">(_node->getValue());\n";
			else tes += "if(_node!=NULL)_obj"+typ+"set"+camelCased(name)+"(CastUtil::lexical_cast<"+type+">(_node->getValue()));\n";
		}
		else
		{
			if(!priv)tes += "if(_node!=NULL)_obj"+typ + name + "= new "+type+"(CastUtil::lexical_cast<"+type+">(_node->getValue()));\n";
			else tes += "if(_node!=NULL)_obj"+typ+"set"+camelCased(name)+"(new "+type+"(CastUtil::lexical_cast<"+type+">(_node->getValue())));\n";
		}
	}
	else
	{
		tes += "_node = obj.getNode(\""+name+"\");\n";
		string tempp = StringUtil::replaceAllCopy(type, " ", "");
		if(tempp.find("vector<")!=string::npos || tempp.find("list<")!=string::npos || tempp.find("set<")!=string::npos
				|| tempp.find("multiset<")!=string::npos || tempp.find("queue<")!=string::npos || tempp.find("deque<")!=string::npos)
		{
			string stlcnttyp;
			if(tempp.find("vector")!=string::npos)
				stlcnttyp = "vector";
			else if(tempp.find("queue")!=string::npos)
				stlcnttyp = "std::queue";
			else if(tempp.find("deque")!=string::npos)
				stlcnttyp = "deque";
			else if(tempp.find("list")!=string::npos)
				stlcnttyp = "list";
			else if(tempp.find("multiset")!=string::npos)
				stlcnttyp = "multiset";
			else
				stlcnttyp = "set";
			tempp = StringUtil::replaceAllCopy(tempp, "vector<", "");
			tempp = StringUtil::replaceAllCopy(tempp, "multiset<", "");
			tempp = StringUtil::replaceAllCopy(tempp, "set<", "");
			tempp = StringUtil::replaceAllCopy(tempp, "queue<", "");
			tempp = StringUtil::replaceAllCopy(tempp, "deque<", "");
			tempp = StringUtil::replaceAllCopy(tempp, "list<", "");
			tempp = StringUtil::replaceAllCopy(tempp, "std::", "");
			tempp = StringUtil::replaceAllCopy(tempp, ">", "");
			tes += "if(_node!=NULL){\n";
			tes += "for(int i=0;i<_node->getChildren().size();i++)\n{\n";
			tes += "JSONElement* __node = _node->getChildren().at(i);\n";
			tes += generateReadVectorObjects(tempp, name, priv, ptr, typ, stlcnttyp);
			tes += "}\n}\n";

		}
		else if(Reflection::isValidClass(tempp))
		{
			if(!ptr)
			{
				if(!priv)tes += "if(_node!=NULL)_obj"+typ + name + "= read"+tempp+"(*_node);\n";
				else tes += "if(_node!=NULL)_obj"+typ+"set"+camelCased(name)+"(read"+tempp+"(*_node));\n";
			}
			else
			{
				if(!priv)tes += "if(_node!=NULL)_obj"+typ + name + "= read"+tempp+"P(*_node);\n";
				else tes += "if(_node!=NULL)_obj"+typ+"set"+camelCased(name)+"(read"+tempp+"P(*_node));\n";
			}
		}
	}
	return tes;
}


string AfcUtil::generateReadVectorObjects(string type, string name, bool priv, bool ptr, string typ, string conttype)
{
	string tes;
	string act = ((conttype=="set" || conttype=="multiset")?"insert":(conttype=="std::queue"?"push":"push_back"));
	if(type=="int" || type=="short" || type=="long" || type=="double" || type=="float" || type=="string" || type=="bool")
	{
		if(!ptr)
		{
			if(!priv)tes += "if(__node!=NULL)_obj"+typ + name + "."+act+"(CastUtil::lexical_cast<"+type+">(__node->getValue()));\n";
			else tes += "if(__node!=NULL)_obj"+typ+"get"+camelCased(name)+"()."+act+"(CastUtil::lexical_cast<"+type+">(__node->getValue()));\n";
		}
		else
		{
			if(!priv)tes += "if(__node!=NULL){_obj"+typ + name + " = new "+conttype+"<"+type+">;\n_obj"+typ + name + "->"+act+"(CastUtil::lexical_cast<"+type+">(__node->getValue()));\n}\n";
			else tes += "if(__node!=NULL){_obj"+typ +"set"+camelCased(name)+"(new "+conttype+"<"+type+">);\n_obj"+typ+"get"+camelCased(name)+"()->"+act+"(CastUtil::lexical_cast<"+type+">(__node->getValue()));\n}\n";
		}
	}
	else if(Reflection::isValidClass(type))
	{
		if(!ptr)
		{
			if(!priv)tes += "if(__node!=NULL)_obj"+typ + name + "."+act+"(read"+type+"(*__node));\n";
			else tes += "if(__node!=NULL)_obj"+typ+"get"+camelCased(name)+"()."+act+"(read"+type+"(*__node));\n";
		}
		else
		{
			if(!priv)tes += "if(__node!=NULL){_obj"+typ + name + " = new "+conttype+"<"+type+">;\n_obj"+typ + name + "->"+act+"(read"+type+"(*__node));\n}\n";
			else tes += "if(__node!=NULL){_obj"+typ +"set"+camelCased(name)+"(new "+conttype+"<"+type+">);\n_obj"+typ+"get"+camelCased(name)+"()->"+act+"(read"+type+"(*__node));\n}\n";
		}
	}
	return tes;
}

string AfcUtil::generateToJSONObjects(string type, string name, bool priv, strVec obj, int i, string &retu, string &headers, string path, string &objs, string typ, bool ptr)
{
	string fres;
	if(type=="int" || type=="short" || type=="long" || type=="double" || type=="float" || type=="string" || type=="bool")
	{
		if(type=="string")
		{
			if(!ptr)
			{
				if(!priv)fres += "json += \"\\\""+name+"\\\" : \\\"\"+_obj"+typ+name+"+\"\\\"\";";
				else fres += "json += \"\\\""+name+"\\\" : \\\"\"+_obj"+typ+"get"+camelCased(name)+"()+\"\\\"\";";
			}
			else
			{
				if(!priv)fres += "if(_obj"+typ+name+"!=NULL)json += \"\\\""+name+"\\\" : \\\"\"+*_obj"+typ+name+"+\"\\\"\";\nelse json += \"null\";\n";
				else fres += "if(_obj"+typ+"get"+camelCased(name)+"()!=NULL)json += \"\\\""+name+"\\\" : \\\"\"+*_obj"+typ+"get"+camelCased(name)+"()+\"\\\"\";\nelse json += \"null\";\n";
			}
		}
		else
		{
			if(!ptr)
			{
				if(!priv)fres += "json += \"\\\""+name+"\\\" : \"+CastUtil::lexical_cast<string>(_obj"+typ+name+");";
				else fres += "json += \"\\\""+name+"\\\" : \"+CastUtil::lexical_cast<string>(_obj"+typ+"get"+camelCased(name)+"());";
			}
			else
			{
				if(!priv)fres += "if(_obj"+typ+name+"!=NULL)json += \"\\\""+name+"\\\" : \"+CastUtil::lexical_cast<string>(*_obj"+typ+name+");\nelse json += \"null\";\n";
				else fres += "if(_obj"+typ+"get"+camelCased(name)+"()!=NULL)json += \"\\\""+name+"\\\" : \"+CastUtil::lexical_cast<string>(*_obj"+typ+"get"+camelCased(name)+"());\nelse json += \"null\";\n";
			}
		}

		if(i!=((int)obj.size()-1))
		{
			fres += "\njson += \",\";\n";
		}
	}
	else
	{
		string tempp = StringUtil::replaceAllCopy(type, " ", "");
		if(tempp.find("vector<")!=string::npos || tempp.find("list<")!=string::npos || tempp.find("set<")!=string::npos
				|| tempp.find("multiset<")!=string::npos || tempp.find("queue<")!=string::npos || tempp.find("deque<")!=string::npos)
		{
			string stlcnttyp;
			if(tempp.find("vector")!=string::npos)
				stlcnttyp = "vector";
			else if(tempp.find("queue")!=string::npos)
				stlcnttyp = "std::queue";
			else if(tempp.find("deque")!=string::npos)
				stlcnttyp = "deque";
			else if(tempp.find("list")!=string::npos)
				stlcnttyp = "list";
			else if(tempp.find("multiset")!=string::npos)
				stlcnttyp = "multiset";
			else
				stlcnttyp = "set";
			tempp = StringUtil::replaceAllCopy(tempp, "vector<", "");
			tempp = StringUtil::replaceAllCopy(tempp, "multiset<", "");
			tempp = StringUtil::replaceAllCopy(tempp, "set<", "");
			tempp = StringUtil::replaceAllCopy(tempp, "queue<", "");
			tempp = StringUtil::replaceAllCopy(tempp, "deque<", "");
			tempp = StringUtil::replaceAllCopy(tempp, "list<", "");
			tempp = StringUtil::replaceAllCopy(tempp, "std::", "");
			tempp = StringUtil::replaceAllCopy(tempp, ">", "");

			fres += "json += \"\\\""+name+"\\\" : \";\n";
			if(!priv)
			{
				if(ptr)
				{
					fres += "if(_obj"+typ+name+"!=NULL)\n";
					fres += "json += JSONSerialize::serialize<"+stlcnttyp+"<"+tempp+"> >(*_obj"+typ+name+");\n";
				}
				else
					fres += "json += JSONSerialize::serialize<"+stlcnttyp+"<"+tempp+"> >(_obj"+typ+name+");\n";
			}
			else
			{
				if(ptr)
				{
					fres += "if(_obj"+typ+"get"+camelCased(name)+"()!=NULL)\n";
					fres += "json += JSONSerialize::serialize<"+stlcnttyp+"<"+tempp+"> >(*_obj"+typ+"get"+camelCased(name)+"());\n";
				}
				else
					fres += "json += JSONSerialize::serialize<"+stlcnttyp+"<"+tempp+"> >(_obj"+typ+"get"+camelCased(name)+"());\n";
			}
			//fres += generateToJSONVectorObjects(tempp, name, priv, retu, headers, path, objs, typ, ptr, stlcnttyp);
		}
		else if(Reflection::isValidClass(type))
		{
			if(!ptr)
			{
				if(!priv)fres += "json += \"\\\""+name+"\\\" : \"+from"+type+"ToJSON(_obj"+typ+name+");";
				else fres += "json += \"\\\""+name+"\\\" : \"+from"+type+"ToJSON(_obj"+typ+"get"+camelCased(name)+"());";
			}
			else
			{
				if(!priv)fres += "if(_obj"+typ+name+"!=NULL)json += \"\\\""+name+"\\\" : \"+from"+type+"ToJSON(*_obj"+typ+name+");\nelse json += \"null\";\n";
				else fres += "if(_obj"+typ+"get"+camelCased(name)+"()!=NULL)json += \"\\\""+name+"\\\" : \"+from"+type+"ToJSON(*_obj"+typ+"get"+camelCased(name)+"());\nelse json += \"null\";\n";
			}
		}
		if(i!=((int)obj.size()-1))
		{
			fres += "json += \",\";\n";
		}
	}
	return fres;
}

string AfcUtil::generateToJSONVectorObjects(string type, string name, bool priv, string &retu, string &headers, string path, string &objs, string typ, bool ptr, string stlcnttyp)
{
	string fres = "json += \"\\\""+name+"\\\" : [\";\n";
	string vtyp = ".";
	if(ptr)
		vtyp = "->";
	//Is list/set then use iterator
	//if queue then use pop front etc after copying to another queue
	if(ptr)fres += "if(_obj"+typ+name+"!=NULL)\n";
	if(!priv)
	{
		if(stlcnttyp=="vector")
			fres += "for(int j=0;j<_obj"+typ+name+vtyp+"size();j++)\n{\n";
		else if(stlcnttyp=="list" || stlcnttyp=="set" || stlcnttyp=="multiset" || stlcnttyp=="deque")
			fres += stlcnttyp+"<"+type+">::iterator it"+name+";\nint j"+name+" = 0;\nfor(it"+name+"=_obj"+typ+name+vtyp+"begin();it"+name+"!=_obj"+typ+name+vtyp+"end();it"+name+"++,j"+name+"++)\n{\n";
		else
		{
			if(!ptr)
				fres += "std::queue<"+type+"> tt"+name+" = _obj"+typ+name+";\nif(!tt"+name+vtyp+"empty()){\n" +
					"for(int j=0;j<(int)tt"+name+vtyp+"size();j++)\n{\n";
			else
				fres += "std::queue<"+type+">* tt"+name+" = _obj"+typ+name+";\nif(!tt"+name+"->empty()){\n" +
					"for(int j=0;j<(int)tt"+name+"->size();j++)\n{\n";
		}
	}
	else
	{
		if(stlcnttyp=="vector")
			fres += "for(int j=0;j<_obj"+typ+"get"+camelCased(name)+"()"+vtyp+"size();j++)\n{\n";
		else if(stlcnttyp=="list" || stlcnttyp=="set" || stlcnttyp=="multiset" || stlcnttyp=="deque")
			fres += stlcnttyp+"<"+type+">::iterator it"+name+";\nint j"+name+" = 0;\nfor(it"+name+"=_obj"+typ+"get"+camelCased(name)+"()"+vtyp+"begin();it"+name+"!=_obj"+typ+"get"+camelCased(name)+"()"+vtyp+"end();it"+name+"++,j"+name+"++)\n{\n";
		else
		{
			if(!ptr)
				fres += "std::queue<"+type+"> tt"+name+" = _obj"+typ+"get"+camelCased(name)+"();\nif(!tt"+name+vtyp+"empty()){\n" +
					"for(int j=0;j<(int)tt"+name+vtyp+"size();j++)\n{\n";
			else
				fres += "std::queue<"+type+">* tt"+name+" = _obj"+typ+"get"+camelCased(name)+"();\nif(!tt"+name+"!=NULL && !tt"+name+"->empty()){\n" +
					"for(int j=0;j<(int)tt"+name+"->size();j++)\n{\n";
		}
	}
	if(type=="int" || type=="short" || type=="long" || type=="double" || type=="float" || type=="string" || type=="bool")
	{
		if(type=="string")
		{
			if(!ptr)
			{
				if(stlcnttyp=="vector")
				{
					if(!priv)fres += "json += \"\\\"\"+_obj"+typ+name+".at(j)+\"\\\"\";";
					else fres += "json += \"\\\"\"+_obj"+typ+"get"+camelCased(name)+"().at(j)+\"\\\"\";";
				}
				else if(stlcnttyp=="list" || stlcnttyp=="set" || stlcnttyp=="multiset" || stlcnttyp=="deque")
				{
					fres += "json += \"\\\"\";json += *it"+name+";json += \"\\\"\";";
				}
				else
				{
					if(!ptr)
						fres += "json += \"\\\"\";json += tt"+name+".front();json +=\"\\\"\";\ntt"+name+".pop();\n";
					else
						fres += "json += \"\\\"\";json += tt"+name+"->front();json +=\"\\\"\";\ntt"+name+"->pop();\n";
				}
			}
			else
			{
				if(stlcnttyp=="vector")
				{
					if(!priv)fres += "json += \"\\\"\"+_obj"+typ+name+"->at(j)+\"\\\"\";";
					else fres += "json += \"\\\"\"+_obj"+typ+"get"+camelCased(name)+"()->at(j)+\"\\\"\";";
				}
				else if(stlcnttyp=="list" || stlcnttyp=="set" || stlcnttyp=="multiset" || stlcnttyp=="deque")
				{
					fres += "json += \"\\\"\"+*it"+name+"+\"\\\"\";";
				}
				else
				{
					if(!ptr)
						fres += "json += \"\\\"\"+tt"+name+".front()+\"\\\"\";\ntt"+name+".pop();\n";
					else
						fres += "json += \"\\\"\"+tt"+name+"->front()+\"\\\"\";\ntt"+name+"->pop();\n";
				}
			}
		}
		else
		{
			if(!ptr)
			{
				if(stlcnttyp=="vector")
				{
					if(!priv)fres += "json += CastUtil::lexical_cast<string>(_obj"+typ+name+".at(j));";
					else fres += "json += CastUtil::lexical_cast<string>(_obj"+typ+"get"+camelCased(name)+"().at(j));";
				}
				else if(stlcnttyp=="list" || stlcnttyp=="set" || stlcnttyp=="multiset" || stlcnttyp=="deque")
				{
					fres += "json += CastUtil::lexical_cast<string>(*it"+name+");";
				}
				else
				{
					if(!ptr)
						fres += "json += CastUtil::lexical_cast<string>(tt"+name+".front());\ntt"+name+".pop();\n";
					else
						fres += "json += CastUtil::lexical_cast<string>(tt"+name+"->front());\ntt"+name+"->pop();\n";
				}
			}
			else
			{
				if(stlcnttyp=="vector")
				{
					if(!priv)fres += "json += CastUtil::lexical_cast<string>(_obj"+typ+name+"->at(j));";
					else fres += "json += CastUtil::lexical_cast<string>(_obj"+typ+"get"+camelCased(name)+"()->at(j));";
				}
				else if(stlcnttyp=="list" || stlcnttyp=="set" || stlcnttyp=="multiset" || stlcnttyp=="deque")
				{
					fres += "json += CastUtil::lexical_cast<string>(*it"+name+");";
				}
				else
				{
					if(!ptr)
						fres += "json += CastUtil::lexical_cast<string>(tt"+name+".front());\ntt"+name+".pop();\n";
					else
						fres += "json += CastUtil::lexical_cast<string>(tt"+name+"->front());\ntt"+name+"->pop();\n";
				}
			}
		}
	}
	else if(Reflection::isValidClass(type))
	{
		if(!ptr)
		{
			if(stlcnttyp=="vector")
			{
				if(!priv)fres += "json += from"+type+"ToJSON(_obj"+typ+name+".at(j));";
				else fres += "json += from"+type+"ToJSON(_obj"+typ+"get"+camelCased(name)+"().at(j));";
			}
			else if(stlcnttyp=="list" || stlcnttyp=="set" || stlcnttyp=="multiset" || stlcnttyp=="deque")
			{
				if(!priv)fres += "json += from"+type+"ToJSON(*it"+name+");";
				else fres += "json += from"+type+"ToJSON(*it"+name+");";
			}
			else
			{
				if(!ptr)
				{
					if(!priv)fres += "json += from"+type+"ToJSON(tt"+name+".front());\ntt"+name+".pop();\n";
					else fres += "json += from"+type+"ToJSON(tt"+name+".front());\ntt"+name+".pop();\n";
				}
				else
				{
					if(!priv)fres += "json += from"+type+"ToJSON(tt"+name+"->front());\ntt"+name+"->pop();\n";
					else fres += "json += from"+type+"ToJSON(tt"+name+"->front());\ntt"+name+"->pop();\n";
				}
			}
		}
		else
		{
			if(stlcnttyp=="vector")
			{
				if(!priv)fres += "json += from"+type+"ToJSON(_obj"+typ+name+"->at(j));";
				else fres += "json += from"+type+"ToJSON(_obj"+typ+"get"+camelCased(name)+"()->at(j));";
			}
			else if(stlcnttyp=="list" || stlcnttyp=="set" || stlcnttyp=="multiset" || stlcnttyp=="deque")
			{
				if(!priv)fres += "json += from"+type+"ToJSON(*it"+name+");";
				else fres += "json += from"+type+"ToJSON(*it"+name+");";
			}
			else
			{
				if(!ptr)
				{
					if(!priv)fres += "json += from"+type+"ToJSON(tt"+name+".front());\ntt"+name+".pop();\n";
					else fres += "json += from"+type+"ToJSON(tt"+name+".front());\ntt"+name+".pop();\n";
				}
				else
				{
					if(!priv)fres += "json += from"+type+"ToJSON(tt"+name+"->front());\ntt"+name+"->pop();\n";
					else fres += "json += from"+type+"ToJSON(tt"+name+"->front());\ntt"+name+"->pop();\n";
				}
			}
		}
	}
	if(!priv)
	{
		if(stlcnttyp=="list" || stlcnttyp=="set" || stlcnttyp=="multiset" || stlcnttyp=="deque")
			fres += "\nif(j"+name+"!=_obj"+typ+name+vtyp+"size()-1)json += \",\";\n";
		else
			fres += "\nif(j!=_obj"+typ+name+vtyp+"size()-1)json += \",\";\n";
	}
	else
	{
		if(stlcnttyp=="list" || stlcnttyp=="set" || stlcnttyp=="multiset" || stlcnttyp=="deque")
			fres += "\nif(j"+name+"!=_obj"+typ+"get"+camelCased(name)+"()"+vtyp+"size()-1)json += \",\";\n";
		else
			fres += "\nif(j!=_obj"+typ+"get"+camelCased(name)+"()"+vtyp+"size()-1)json += \",\";\n";
	}
	if(stlcnttyp=="std::queue")
		fres += "}\n";
	fres += "}\njson += \"]\";\n";
	return fres;
}

string AfcUtil::generateJsInterfaces(strVec obj,string claz,string &headers,string path,string &infjs,string pv,map<string, string> ajintpthMap)
{
	string test,intf,intff,inc;
	headers += "#include \"" + claz + ".h\"\n";
	//writeTofile("/home/sumeet/workspace/inter/AfcInclude.h",inc,false);
	//inc = "\nextern \"C\"{\n";//string executeAFC(string fn,strVec _inp){\nstring ret;\n";
	//bool fl = false;
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
					test += "AfcCall(\""+claz+"\",\""+emp.at(1)+"\",new Array("+jsonstr+"),_cb,(_url==null?\""+ajintpthMap[claz]+"\":_url),_cntxt);\n";
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
					//fl = true;
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
	//logger << test << flush;

	return test;
}

void AfcUtil::execute(HttpRequest req, HttpResponse* res, string claz)
{
	logger << "Inside Ajax Interface Execute" << endl;
	strVec vemp;
	string methName = req.getRequestParam("method");
	if(methName=="")
	{
		res->setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
		return;
	}
	string temp = req.getRequestParam("paramsize");
	int paramSize = 0;
	if(temp!="")
	{
		try {
			paramSize = CastUtil::lexical_cast<int>(temp.c_str());
		} catch(...) {
			res->setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
			return;
		}
	}
	logger << "Reading Ajax params" << endl;
	for(int i=0;i<paramSize;i++)
	{
		stringstream s;
		string ss;
		s << (i+1);
		s >> ss;
		ss = "param_" + ss;
		//logger << ss << flush;
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
		logger << ("Calling method " + metn) << endl;
		re = f2(vemp);
		logger << "Completed method call" << endl;
		res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
		res->setContent_type("text/plain");
		res->setContent_str(re);
		return;
	}
	else
	{
		res->setHTTPResponseStatus(HTTPResponseStatus::InternalServerError);
		return;
	}
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
