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
typedef map<string,strVec> mapofvec;

AfcUtil::AfcUtil() {
}

AfcUtil::~AfcUtil() {
	// TODO Auto-generated destructor stub
}
static map<string,string> doneMap;
string AfcUtil::generateJsInterfacessAll(map<string, ClassStructure> allclsmap,string &infjs,map<string, string> ajintpthMap,strVec afcd, Reflection ref)
{
	string ret;
	/*map<string, ClassStructure> allclsmap;
	for (unsigned int var = 0; var < obj.size(); ++var)
	{
		map<string, ClassStructure> clsmap = ref.getClassStructures(obj.at(var)+files.at(var)+".h");
		map<string, ClassStructure>::iterator it;
		for (it=clsmap.begin();it!=clsmap.end();++it)
		{
			it->second.appName = pv.at(var);
		}
		allclsmap.insert(clsmap.begin(), clsmap.end());
	}*/
	map<string, ClassStructure>::iterator it;
	for (int var = 0; var < (int)afcd.size(); ++var) {
		for (it=allclsmap.begin();it!=allclsmap.end();++it)
		{
			if(it->second.getFullyQualifiedClassName()==afcd.at(var))
			{
				strVec pinfo;
				bool isOpForSet = false;
				strVec info = ref.getAfcObjectData(it->second, false,pinfo,isOpForSet);
				ret += generateJsInterfaces(info,it->second,"",infjs,it->second.appName,ajintpthMap,ref);
				break;
			}
		}
	}
	/*for (unsigned int var = 0; var < obj.size(); ++var)
	{
		strVec pinfo;
		bool isOpForSet = false;
		strVec info = ref.getAfcObjectData(obj.at(var)+files.at(var)+".h", false,pinfo,isOpForSet);
		ret += generateJsInterfaces(info,files.at(var),obj.at(var),infjs,pv.at(var),ajintpthMap);
	}*/
	return ret;
}

string AfcUtil::generateJsObjectsAll(map<string, ClassStructure> allclsmap)
{
	Reflection ref;
	string ret;
	/*strVec includes = ref.list(includeDir);
	map<string, ClassStructure> allclsmap;
	for (unsigned int var = 0; var < includes.size(); ++var)
	{
		map<string, ClassStructure> clsmap = ref.getClassStructures(includes.at(var));
		allclsmap.insert(clsmap.begin(), clsmap.end());
	}*/
	map<string, ClassStructure>::iterator it;
	for (it=allclsmap.begin();it!=allclsmap.end();++it)
	{
		strVec pinfo;
		bool isOpForSet = false;
		strVec minfo = ref.getAfcObjectData(it->second,false,pinfo,isOpForSet);
		pinfo.clear();
		strVec info = ref.getAfcObjectData(it->second,true,pinfo,isOpForSet);
		ret += generateJsObjects(info,it->second.getTreatedClassName(false),pinfo,minfo);
	}
	/*for (unsigned int var = 0; var < includes.size(); ++var)
	{
		strVec pinfo;
		bool isOpForSet = false;
		strVec minfo = ref.getAfcObjectData(includes.at(var),false,pinfo,isOpForSet);
		pinfo.clear();
		strVec info = ref.getAfcObjectData(includes.at(var),true,pinfo,isOpForSet);
		ret += generateJsObjects(info,ref.getTreatedClassName(false),pinfo,minfo);
	}*/
	doneMap.clear();
	return ret;
}


string AfcUtil::generateJsObjects(strVec obj,string claz,strVec pobj,strVec minfo)
{
	if(doneMap.find(claz)==doneMap.end())
		doneMap[claz] = "done";
	else
		return "";
	Reflection ref;
	string retu;
	string test = "function _"+claz+"()\n{\n";
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

			bool fldstatic = false;
			if(RegexUtil::find(fld, "[ \\t]*static[ \\t]+")!=-1 || RegexUtil::find(fld, "[ \\t]+static[ \\t]+")!=-1)
			{
				RegexUtil::replace(fld, "[ \\t]*static[ \\t]+", " ");
				RegexUtil::replace(fld, "[ \\t]+static[ \\t]+", " ");
				fldstatic = true;
			}
			RegexUtil::replace(fld, "[ \\t]*const[ \\t]+", " ");
			RegexUtil::replace(fld, "[ \\t]+const[ \\t]+", " ");

			RegexUtil::replace(fld, "[\t]+", " ");
			RegexUtil::replace(fld, "[ ]+", " ");
			StringUtil::replaceFirst(fld,";","");
			StringUtil::trim(fld);

			bool ptr = false;
			if(fld.find("*")!=string::npos)
			{
				ptr = true;
				StringUtil::replaceFirst(fld,"*","");
			}

			vector<string> fldp;
			fldp.push_back(fld.substr(0, fld.find_last_of(" ")));
			fldp.push_back(fld.substr(fld.find_last_of(" ")+1));

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

			string argts = meth.substr(meth.find("("),meth.find(")")-meth.find("("));
			StringUtil::replaceFirst(argts,"(","");
			StringUtil::replaceAll(argts,")","");
			meth = meth.substr(0,meth.find("("));
			StringUtil::trim(meth);
			RegexUtil::replace(meth, "[ \\t]*,[ \\t]*", ",");

			bool methstat = false;
			if(RegexUtil::find(meth, "[ \\t]*static[ \\t]+")!=-1 || RegexUtil::find(meth, "[ \\t]+static[ \\t]+")!=-1)
			{
				RegexUtil::replace(meth, "[ \\t]*static[ \\t]+", " ");
				RegexUtil::replace(meth, "[ \\t]+static[ \\t]+", " ");
				methstat = true;
			}
			RegexUtil::replace(meth, "[ \\t]*inline[ \\t]+", " ");
			RegexUtil::replace(meth, "[ \\t]+inline[ \\t]+", " ");
			StringUtil::trim(meth);

			//StringUtil::replaceFirst(meth,")"," ");
			vector<string> methp,methpm,argp,argpm,argpmtemp;
			StringUtil::split(argp, argts, (","));
			methp.push_back(meth.substr(0, meth.find_last_of(" ")));
			methp.push_back(meth.substr(meth.find_last_of(" ")+1));
			//StringUtil::split(methp, meth, (" "));
			for(unsigned int j = 0; j < methp.size(); j++)
			{
				if(methp.at(j)!="")
				{
					StringUtil::trim(methp.at(j));
					methpm.push_back(methp.at(j));
				}
			}
			for(unsigned int j = 0; j < argp.size(); j++)
			{
				if(argp.at(j)!="" && argp.at(j)!="(")
				{
					string argpmtemp = argp.at(j);
					StringUtil::trim(argpmtemp);
					string typ;
					if(argpmtemp.find("*")!=string::npos)
						typ = "*";
					else if(argpmtemp.find("&")!=string::npos)
						typ = "&";
					if(argpmtemp.find(" ")!=string::npos)
					{
						argpmtemp = argpmtemp.substr(0, argpmtemp.find_last_of(" "));
					}
					StringUtil::trim(argpmtemp);
					argpm.push_back(argpmtemp);
				}
			}
			bool ptr = false;
			if(meth.find("*")!=string::npos)
			{
				ptr = true;
				StringUtil::replaceFirst(meth,"*","");
			}

			if(meth.find(" operator")!=string::npos)
			{
				meth = meth.substr(meth.find(" operator"));
				StringUtil::replaceAll(meth, " ", "");
			}
			if(meth.find("operator<")!=string::npos || meth.find("operator>")!=string::npos || meth.find("operator<=")!=string::npos
				|| meth.find("operator>=")!=string::npos || meth.find("operator==")!=string::npos || meth.find("operator!=")!=string::npos || meth.find("operator!")!=string::npos
				|| meth.find("operator<<")!=string::npos || meth.find("operator>>")!=string::npos || meth.find("operator+")!=string::npos || meth.find("operator-")!=string::npos
				|| meth.find("operator*")!=string::npos || meth.find("operator/")!=string::npos || meth.find("operator[]")!=string::npos || meth.find("operator()")!=string::npos
				|| meth.find("operator&")!=string::npos || meth.find("operator&&")!=string::npos || meth.find("operator||")!=string::npos || meth.find("operator|")!=string::npos)
			{}
			else
			{
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
	/*strVec tobj;
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
	obj = tobj;*/
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
		if(data.find(" ")==string::npos)
		{
			//logger << data << " error" <<  endl;
			continue;
		}
		data = data.substr(data.find_last_of(" ")+1);
		StringUtil::trim(data);

		priv = (fldstat[data]==2?true:false);

		test += "this." + data + "= null;\n";
	}
	test += "}";
	return test;
}

string AfcUtil::generateJsObjects(strVec obj,ClassStructure classstruc,string &headers,string &objs,strVec pobj, bool isOpForSet, string& typrefs,strVec minfo,string app,string clspth,Reflection ref)
{
	if(doneMap.find(classstruc.getFullyQualifiedClassName())==doneMap.end())
		doneMap[classstruc.getFullyQualifiedClassName()] = "done";
	else
		return "";
	string tes,tes1,fres,fres1;
	string retu;
	string test = "function _"+classstruc.getTreatedClassName(false)+"()\n{\n";
	tes += "JSONElement* _node;\n";
	tes1 += "JSONElement* _node;\n";
	fres = "string json=\"{\";\n";
	fres1 = "string json=\"{\";\n";
	bool priv = false;
	size_t tess;
	string meth;
	strVec fldnames;
	map<string, int> fldstat;
	map<string, bool> fldptr;
	for (unsigned int i = 0; i < pobj.size(); i++)
	{
		if(((tess=pobj.at(i).find("("))==string::npos && (tess=pobj.at(i).find(")"))==string::npos && pobj.at(i).find("~")==string::npos))
		{
			string fld = pobj.at(i);

			bool fldstatic = false;
			if(RegexUtil::find(fld, "[ \\t]*static[ \\t]+")!=-1 || RegexUtil::find(fld, "[ \\t]+static[ \\t]+")!=-1)
			{
				RegexUtil::replace(fld, "[ \\t]*static[ \\t]+", " ");
				RegexUtil::replace(fld, "[ \\t]+static[ \\t]+", " ");
				fldstatic = true;
			}
			RegexUtil::replace(fld, "[ \\t]*const[ \\t]+", " ");
			RegexUtil::replace(fld, "[ \\t]+const[ \\t]+", " ");

			RegexUtil::replace(fld, "[\t]+", " ");
			RegexUtil::replace(fld, "[ ]+", " ");
			StringUtil::replaceFirst(fld,";","");
			StringUtil::trim(fld);

			bool ptr = false;
			if(fld.find("*")!=string::npos)
			{
				ptr = true;
				StringUtil::replaceFirst(fld,"*","");
			}

			vector<string> fldp;
			fldp.push_back(fld.substr(0, fld.find_last_of(" ")));
			fldp.push_back(fld.substr(fld.find_last_of(" ")+1));

			if(fldp.size()==2)
			{
				string nam = fldp.at(1);
				fldnames.push_back(fldp.at(0));
				fldnames.push_back(nam);
				if(ptr)
				{
					fldptr[nam] = true;
				}
				else
				{
					fldptr[nam] = false;
				}
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

			string argts = meth.substr(meth.find("("),meth.find(")")-meth.find("("));
			StringUtil::replaceFirst(argts,"(","");
			StringUtil::replaceAll(argts,")","");
			meth = meth.substr(0,meth.find("("));
			StringUtil::trim(meth);
			RegexUtil::replace(meth, "[ \\t]*,[ \\t]*", ",");

			bool methstat = false;
			if(RegexUtil::find(meth, "[ \\t]*static[ \\t]+")!=-1 || RegexUtil::find(meth, "[ \\t]+static[ \\t]+")!=-1)
			{
				RegexUtil::replace(meth, "[ \\t]*static[ \\t]+", " ");
				RegexUtil::replace(meth, "[ \\t]+static[ \\t]+", " ");
				methstat = true;
			}
			RegexUtil::replace(meth, "[ \\t]*inline[ \\t]+", " ");
			RegexUtil::replace(meth, "[ \\t]+inline[ \\t]+", " ");
			StringUtil::trim(meth);

			//StringUtil::replaceFirst(meth,")"," ");
			vector<string> methp,methpm,argp,argpm,argpmtemp;
			StringUtil::split(argp, argts, (","));
			methp.push_back(meth.substr(0, meth.find_last_of(" ")));
			methp.push_back(meth.substr(meth.find_last_of(" ")+1));
			//StringUtil::split(methp, meth, (" "));
			for(unsigned int j = 0; j < methp.size(); j++)
			{
				if(methp.at(j)!="")
				{
					StringUtil::trim(methp.at(j));
					methpm.push_back(methp.at(j));
				}
			}
			for(unsigned int j = 0; j < argp.size(); j++)
			{
				if(argp.at(j)!="" && argp.at(j)!="(")
				{
					string argpmtemp = argp.at(j);
					StringUtil::trim(argpmtemp);
					string typ;
					if(argpmtemp.find("*")!=string::npos)
						typ = "*";
					else if(argpmtemp.find("&")!=string::npos)
						typ = "&";
					if(argpmtemp.find(" ")!=string::npos)
					{
						argpmtemp = argpmtemp.substr(0, argpmtemp.find_last_of(" "));
					}
					StringUtil::trim(argpmtemp);
					argpm.push_back(argpmtemp);
				}
			}
			bool ptr = false;
			if(meth.find("*")!=string::npos)
			{
				ptr = true;
				StringUtil::replaceFirst(meth,"*","");
			}

			if(meth.find(" operator")!=string::npos)
			{
				meth = meth.substr(meth.find(" operator"));
				StringUtil::replaceAll(meth, " ", "");
			}
			if(meth.find("operator<")!=string::npos || meth.find("operator>")!=string::npos || meth.find("operator<=")!=string::npos
				|| meth.find("operator>=")!=string::npos || meth.find("operator==")!=string::npos || meth.find("operator!=")!=string::npos || meth.find("operator!")!=string::npos
				|| meth.find("operator<<")!=string::npos || meth.find("operator>>")!=string::npos || meth.find("operator+")!=string::npos || meth.find("operator-")!=string::npos
				|| meth.find("operator*")!=string::npos || meth.find("operator/")!=string::npos || meth.find("operator[]")!=string::npos || meth.find("operator()")!=string::npos
				|| meth.find("operator&")!=string::npos || meth.find("operator&&")!=string::npos || meth.find("operator||")!=string::npos || meth.find("operator|")!=string::npos)
			{}
			else
			{
				if(methpm.at(0)!=classstruc.getTreatedClassName(false))
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

			priv = (fldstat[fldnames.at(k+1)]==2?true:false);

			test += "this." + fldnames.at(k+1) + "= null;\n";
			tes += generateReadObjects(fldnames.at(k), fldnames.at(k+1), priv, fldptr[fldnames.at(k+1)], ".", app, classstruc, ref);
			tes1 += generateReadObjects(fldnames.at(k), fldnames.at(k+1), priv, fldptr[fldnames.at(k+1)], "->", app, classstruc, ref);
			fres += generateToJSONObjects(fldnames.at(k), fldnames.at(k+1), priv, k!=fldnames.size()-2, retu, headers, objs, ".", fldptr[fldnames.at(k+1)], app, classstruc, ref);
			fres1 += generateToJSONObjects(fldnames.at(k), fldnames.at(k+1), priv, k!=fldnames.size()-2, retu, headers, objs, "->", fldptr[fldnames.at(k+1)], app, classstruc, ref);
		}
	}
	/*for(unsigned int i=0;i<pobj.size();i++)
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
		tes += generateReadObjects(vemp.at(0), vemp.at(1), priv, ptr, ".", app, classstruc, ref);
		tes1 += generateReadObjects(vemp.at(0), vemp.at(1), priv, ptr, "->", app, classstruc, ref);
		fres += generateToJSONObjects(vemp.at(0), vemp.at(1), priv, obj, i, retu, headers, objs, ".", ptr, app, classstruc, ref);
		fres1 += generateToJSONObjects(vemp.at(0), vemp.at(1), priv, obj, i, retu, headers, objs, "->", ptr, app, classstruc, ref);
	}*/

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
	headers += "#include \""+clspth+"\"\n";

	typrefs += classstruc.getFullyQualifiedClassName() + " " + app + "read"+classstruc.getTreatedClassName(true)+"(JSONElement& obj);\n" + classstruc.getFullyQualifiedClassName() + "* "+app+"read"+classstruc.getTreatedClassName(true)+"P(JSONElement& obj);\n";
	typrefs += "\nstring " + app + "from"+classstruc.getTreatedClassName(true)+"ToJSON("+classstruc.getFullyQualifiedClassName()+" _obj);\nstring " + app + "from"+classstruc.getTreatedClassName(true)+"VPToJSON("+classstruc.getFullyQualifiedClassName()+"* _obj);\n";
	typrefs	+= classstruc.getFullyQualifiedClassName() + " " + app + "to"+classstruc.getTreatedClassName(true)+"(string s);\n" + classstruc.getFullyQualifiedClassName() + "* " + app + "to"+classstruc.getTreatedClassName(true)+"P(string s);\n";
	typrefs	+= "vector<"+classstruc.getFullyQualifiedClassName()+"> " + app + "to"+classstruc.getTreatedClassName(true)+"Vec(string s);\nvector<"+classstruc.getFullyQualifiedClassName()+">* " + app + "to"+classstruc.getTreatedClassName(true)+"VecVP(string s);\n";
	typrefs	+= "list<"+classstruc.getFullyQualifiedClassName()+"> " + app + "to"+classstruc.getTreatedClassName(true)+"Lis(string s);\nlist<"+classstruc.getFullyQualifiedClassName()+">* " + app + "to"+classstruc.getTreatedClassName(true)+"LisVP(string s);\n";
	typrefs	+= "deque<"+classstruc.getFullyQualifiedClassName()+"> " + app + "to"+classstruc.getTreatedClassName(true)+"Dq(string s);\ndeque<"+classstruc.getFullyQualifiedClassName()+">* " + app + "to"+classstruc.getTreatedClassName(true)+"DqVP(string s);\n";
	typrefs	+= "std::queue<"+classstruc.getFullyQualifiedClassName()+"> " + app + "to"+classstruc.getTreatedClassName(true)+"Q(string s);\nstd::queue<"+classstruc.getFullyQualifiedClassName()+">* " + app + "to"+classstruc.getTreatedClassName(true)+"QVP(string s);\n";
	if(isOpForSet)
	{
		typrefs	+= "set<"+classstruc.getFullyQualifiedClassName()+"> " + app + "to"+classstruc.getTreatedClassName(true)+"Set(string s);\nset<"+classstruc.getFullyQualifiedClassName()+">* " + app + "to"+classstruc.getTreatedClassName(true)+"SetVP(string s);\n";
		typrefs	+= "multiset<"+classstruc.getFullyQualifiedClassName()+"> " + app + "to"+classstruc.getTreatedClassName(true)+"MulSet(string s);\nmultiset<"+classstruc.getFullyQualifiedClassName()+">* " + app + "to"+classstruc.getTreatedClassName(true)+"MulSetVP(string s);\n";
	}
	typrefs += "\nstring " + app + "from"+classstruc.getTreatedClassName(true)+"VecToJSON(vector<"+classstruc.getFullyQualifiedClassName()+"> _obj);\nstring " + app + "from"+classstruc.getTreatedClassName(true)+"VecVPToJSON(vector<"+classstruc.getFullyQualifiedClassName()+">* _obj);\n";
	typrefs += "\nstring " + app + "from"+classstruc.getTreatedClassName(true)+"LisToJSON(list<"+classstruc.getFullyQualifiedClassName()+"> _obj);\nstring " + app + "from"+classstruc.getTreatedClassName(true)+"LisVPToJSON(list<"+classstruc.getFullyQualifiedClassName()+">* _obj);\n";
	typrefs += "\nstring " + app + "from"+classstruc.getTreatedClassName(true)+"DqToJSON(deque<"+classstruc.getFullyQualifiedClassName()+"> _obj);\nstring " + app + "from"+classstruc.getTreatedClassName(true)+"DqVPToJSON(deque<"+classstruc.getFullyQualifiedClassName()+">* _obj);\n";
	typrefs += "\nstring " + app + "from"+classstruc.getTreatedClassName(true)+"QToJSON(std::queue<"+classstruc.getFullyQualifiedClassName()+"> _obj);\nstring " + app + "from"+classstruc.getTreatedClassName(true)+"QVPToJSON(std::queue<"+classstruc.getFullyQualifiedClassName()+">* _obj);\n";
	if(isOpForSet)
	{
		typrefs += "\nstring " + app + "from"+classstruc.getTreatedClassName(true)+"SetToJSON(set<"+classstruc.getFullyQualifiedClassName()+"> _obj);\nstring " + app + "from"+classstruc.getTreatedClassName(true)+"SetVPToJSON(set<"+classstruc.getFullyQualifiedClassName()+">* _obj);\n";
		typrefs += "\nstring " + app + "from"+classstruc.getTreatedClassName(true)+"MulSetToJSON(multiset<"+classstruc.getFullyQualifiedClassName()+"> _obj);\nstring " + app + "from"+classstruc.getTreatedClassName(true)+"MulSetVPToJSON(multiset<"+classstruc.getFullyQualifiedClassName()+">* _obj);\n";
	}

	test = retu+ "\n\n" + classstruc.getFullyQualifiedClassName() + " " + app + "read"+classstruc.getTreatedClassName(true)+"(JSONElement& obj)\n{\n"+classstruc.getFullyQualifiedClassName()+" _obj;\n";
	test += tes + "\nreturn _obj;\n}\n";
	test += "\n\n" + classstruc.getFullyQualifiedClassName() + "* "+app+"read"+classstruc.getTreatedClassName(true)+"P(JSONElement& obj)\n{\n"+classstruc.getFullyQualifiedClassName()+"* _obj = new "+classstruc.getFullyQualifiedClassName()+";\n";
	test += tes1 + "\nreturn _obj;\n}\n";
	test += classstruc.getFullyQualifiedClassName() + " " + app + "to"+classstruc.getTreatedClassName(true)+"(string s)\n{\nJSONElement element = JSONUtil::getDocument(s);\n"+ classstruc.getFullyQualifiedClassName() +" _obj = " + app + "read"+classstruc.getTreatedClassName(true)+"(element);\nreturn _obj;\n}\n";
	test += "vector<"+classstruc.getFullyQualifiedClassName()+"> " + app + "to"+classstruc.getTreatedClassName(true)+"Vec(string s)\n{\nJSONElement element = JSONUtil::getDocument(s);\n" +
			"vector<"+classstruc.getFullyQualifiedClassName()+"> vec;\nfor(int i=0;i<(int)element.getChildren().size();i++){\n" +
			classstruc.getFullyQualifiedClassName() +" _obj = " + app + "read"+classstruc.getTreatedClassName(true)+"(*element.getChildren().at(i));\nvec.push_back(_obj);\n" +
			"}\nreturn vec;\n}\n";
	test += "list<"+classstruc.getFullyQualifiedClassName()+"> " + app + "to"+classstruc.getTreatedClassName(true)+"Lis(string s)\n{\nvector<"+classstruc.getFullyQualifiedClassName()+"> vec = " + app + "to"+classstruc.getTreatedClassName(true)+"Vec(s);\n" +
			"list<"+classstruc.getFullyQualifiedClassName()+"> tt;\nstd::copy(vec.begin(), vec.end(), std::back_inserter(tt));\n" +
			"return tt;\n}\n";
	if(isOpForSet)
	{
		test += "set<"+classstruc.getFullyQualifiedClassName()+"> " + app + "to"+classstruc.getTreatedClassName(true)+"Set(string s)\n{\nvector<"+classstruc.getFullyQualifiedClassName()+"> vec = " + app + "to"+classstruc.getTreatedClassName(true)+"Vec(s);\n" +
				"set<"+classstruc.getFullyQualifiedClassName()+"> tt;\nstd::copy(vec.begin(), vec.end(), std::inserter(tt, tt.begin()));\n" +
				"return tt;\n}\n";
		test += "multiset<"+classstruc.getFullyQualifiedClassName()+"> " + app + "to"+classstruc.getTreatedClassName(true)+"MulSet(string s)\n{\nvector<"+classstruc.getFullyQualifiedClassName()+"> vec = " + app + "to"+classstruc.getTreatedClassName(true)+"Vec(s);\n" +
				"multiset<"+classstruc.getFullyQualifiedClassName()+"> tt;\nstd::copy(vec.begin(), vec.end(), std::inserter(tt, tt.begin()));\n" +
				"return tt;\n}\n";
	}
	test += "deque<"+classstruc.getFullyQualifiedClassName()+"> " + app + "to"+classstruc.getTreatedClassName(true)+"Dq(string s)\n{\nvector<"+classstruc.getFullyQualifiedClassName()+"> vec = " + app + "to"+classstruc.getTreatedClassName(true)+"Vec(s);\n" +
			"deque<"+classstruc.getFullyQualifiedClassName()+"> tt;\nstd::copy(vec.begin(), vec.end(), std::inserter(tt, tt.begin()));\n" +
			"return tt;\n}\n";
	test += "std::queue<"+classstruc.getFullyQualifiedClassName()+"> " + app + "to"+classstruc.getTreatedClassName(true)+"Q(string s)\n{\nvector<"+classstruc.getFullyQualifiedClassName()+"> vec = " + app + "to"+classstruc.getTreatedClassName(true)+"Vec(s);\n" +
			"std::queue<"+classstruc.getFullyQualifiedClassName()+"> tt;for (int var = 0; var < (int)vec.size(); ++var) {\ntt.push(vec.at(var));\n}\n" +
			"return tt;\n}\n";
	test += "vector<"+classstruc.getFullyQualifiedClassName()+">* " + app + "to"+classstruc.getTreatedClassName(true)+"VecVP(string s)\n{\nJSONElement element = JSONUtil::getDocument(s);\n" +
			"vector<"+classstruc.getFullyQualifiedClassName()+">* vec = new vector<"+classstruc.getFullyQualifiedClassName()+">;\nfor(int i=0;i<element.getChildren().size();i++){\n" +
			classstruc.getFullyQualifiedClassName() +" _obj = " + app + "read"+classstruc.getTreatedClassName(true)+"(*element.getChildren().at(i));\nvec->push_back(_obj);\n" +
			"}\nreturn vec;\n}\n";
	test += "list<"+classstruc.getFullyQualifiedClassName()+">* " + app + "to"+classstruc.getTreatedClassName(true)+"LisVP(string s)\n{\nvector<"+classstruc.getFullyQualifiedClassName()+"> vec = " + app + "to"+classstruc.getTreatedClassName(true)+"Vec(s);\n" +
			"list<"+classstruc.getFullyQualifiedClassName()+">* tt = new list<"+classstruc.getFullyQualifiedClassName()+">;\nstd::copy(vec.begin(), vec.end(), std::back_inserter(*tt));\n" +
			"return tt;\n}\n";
	if(isOpForSet)
	{
		test += "set<"+classstruc.getFullyQualifiedClassName()+">* " + app + "to"+classstruc.getTreatedClassName(true)+"SetVP(string s)\n{\nvector<"+classstruc.getFullyQualifiedClassName()+"> vec = " + app + "to"+classstruc.getTreatedClassName(true)+"Vec(s);\n" +
			"set<"+classstruc.getFullyQualifiedClassName()+">* tt = new set<"+classstruc.getFullyQualifiedClassName()+">;\nstd::copy(vec.begin(), vec.end(), std::inserter(*tt, tt->begin()));\n" +
			"return tt;\n}\n";
		test += "multiset<"+classstruc.getFullyQualifiedClassName()+">* " + app + "to"+classstruc.getTreatedClassName(true)+"MulSetVP(string s)\n{\nvector<"+classstruc.getFullyQualifiedClassName()+"> vec = " + app + "to"+classstruc.getTreatedClassName(true)+"Vec(s);\n" +
				"multiset<"+classstruc.getFullyQualifiedClassName()+">* tt = new multiset<"+classstruc.getFullyQualifiedClassName()+">;\nstd::copy(vec.begin(), vec.end(), std::inserter(*tt, tt->begin()));\n" +
				"return tt;\n}\n";
	}
	test += "deque<"+classstruc.getFullyQualifiedClassName()+">* " + app + "to"+classstruc.getTreatedClassName(true)+"DqVP(string s)\n{\nvector<"+classstruc.getFullyQualifiedClassName()+"> vec = " + app + "to"+classstruc.getTreatedClassName(true)+"Vec(s);\n" +
			"deque<"+classstruc.getFullyQualifiedClassName()+">* tt = new deque<"+classstruc.getFullyQualifiedClassName()+">;\nstd::copy(vec.begin(), vec.end(), std::inserter(*tt, tt->begin()));\n" +
			"return tt;\n}\n";
	test += "std::queue<"+classstruc.getFullyQualifiedClassName()+">* " + app + "to"+classstruc.getTreatedClassName(true)+"QVP(string s)\n{\nvector<"+classstruc.getFullyQualifiedClassName()+"> vec = " + app + "to"+classstruc.getTreatedClassName(true)+"Vec(s);\n" +
			"std::queue<"+classstruc.getFullyQualifiedClassName()+">* tt = new std::queue<"+classstruc.getFullyQualifiedClassName()+">;for (int var = 0; var < (int)vec.size(); ++var) {\ntt->push(vec.at(var));\n}\n" +
			"return tt;\n}\n";
	test += "void* " + app + "toVoidP"+classstruc.getTreatedClassName(true)+"(string s)\n{\nJSONElement element = JSONUtil::getDocument(s);\n"+ classstruc.getFullyQualifiedClassName()+"*_obj = new "+classstruc.getFullyQualifiedClassName()+";\n*_obj = " + app + "read"+classstruc.getTreatedClassName(true)+"(element);\nreturn _obj;\n}\n";
	test += "\nstring " + app + "from"+classstruc.getTreatedClassName(true)+"ToJSON("+classstruc.getFullyQualifiedClassName()+" _obj)\n{\n"+fres+"\nreturn json;\n}\n";
	test += "\nstring " + app + "from"+classstruc.getTreatedClassName(true)+"VPToJSON("+classstruc.getFullyQualifiedClassName()+"* _obj)\n{\n"+fres1+"\nreturn json;\n}\n";
	test += "\nstring " + app + "from"+classstruc.getTreatedClassName(true)+"VecToJSON(vector<"+classstruc.getFullyQualifiedClassName()+"> _vecobj)\n{\nstring json = \"[\";\nfor(int i=0;i<(int)_vecobj.size();i++){\n" +
			"json += " + app + "from"+classstruc.getTreatedClassName(true)+"ToJSON(_vecobj.at(i));\n" +
			"if(i!=(int)_vecobj.size()-1)json += \",\";\n" +
			"\n}\njson += \"]\";\nreturn json;\n}\n";
	test += "\nstring " + app + "from"+classstruc.getTreatedClassName(true)+"LisToJSON(list<"+classstruc.getFullyQualifiedClassName()+"> _lisobj)\n{\nvector<"+classstruc.getFullyQualifiedClassName()+"> _vecobj;\nstd::copy(_lisobj.begin(), _lisobj.end(), std::back_inserter(_vecobj));" +
			"\nstring json = \"[\";\nfor(int i=0;i<(int)_vecobj.size();i++){\n" +
			"json += " + app + "from"+classstruc.getTreatedClassName(true)+"ToJSON(_vecobj.at(i));\n" +
			"if(i!=(int)_vecobj.size()-1)json += \",\";\n" +
			"\n}\njson += \"]\";\nreturn json;\n}\n";
	if(isOpForSet)
	{
		test += "\nstring " + app + "from"+classstruc.getTreatedClassName(true)+"SetToJSON(set<"+classstruc.getFullyQualifiedClassName()+"> _lisobj)\n{\nvector<"+classstruc.getFullyQualifiedClassName()+"> _vecobj;\nstd::copy(_lisobj.begin(), _lisobj.end(), std::back_inserter(_vecobj));" +
				"\nstring json = \"[\";\nfor(int i=0;i<(int)_vecobj.size();i++){\n" +
				"json += " + app + "from"+classstruc.getTreatedClassName(true)+"ToJSON(_vecobj.at(i));\n" +
				"if(i!=(int)_vecobj.size()-1)json += \",\";\n" +
				"\n}\njson += \"]\";\nreturn json;\n}\n";
		test += "\nstring " + app + "from"+classstruc.getTreatedClassName(true)+"MulSetToJSON(multiset<"+classstruc.getFullyQualifiedClassName()+"> _lisobj)\n{\nvector<"+classstruc.getFullyQualifiedClassName()+"> _vecobj;\nstd::copy(_lisobj.begin(), _lisobj.end(), std::back_inserter(_vecobj));" +
				"\nstring json = \"[\";\nfor(int i=0;i<(int)_vecobj.size();i++){\n" +
				"json += " + app + "from"+classstruc.getTreatedClassName(true)+"ToJSON(_vecobj.at(i));\n" +
				"if(i!=(int)_vecobj.size()-1)json += \",\";\n" +
				"\n}\njson += \"]\";\nreturn json;\n}\n";
	}
	test += "\nstring " + app + "from"+classstruc.getTreatedClassName(true)+"DqToJSON(deque<"+classstruc.getFullyQualifiedClassName()+"> _lisobj)\n{\nvector<"+classstruc.getFullyQualifiedClassName()+"> _vecobj;\nstd::copy(_lisobj.begin(), _lisobj.end(), std::back_inserter(_vecobj));" +
			"\nstring json = \"[\";\nfor(int i=0;i<(int)_vecobj.size();i++){\n" +
			"json += " + app + "from"+classstruc.getTreatedClassName(true)+"ToJSON(_vecobj.at(i));\n" +
			"if(i!=(int)_vecobj.size()-1)json += \",\";\n" +
			"\n}\njson += \"]\";\nreturn json;\n}\n";
	test += "\nstring " + app + "from"+classstruc.getTreatedClassName(true)+"QToJSON(std::queue<"+classstruc.getFullyQualifiedClassName()+"> _lisobj)\n{\nvector<"+classstruc.getFullyQualifiedClassName()+"> _vecobj;" +
			"std::queue<"+classstruc.getFullyQualifiedClassName()+"> qq = _lisobj;\nfor (int var = 0; var < (int)qq.size(); ++var)\n{" +
			"_vecobj.push_back(qq.front());\nqq.pop();\n}\n" +
			"\nstring json = \"[\";\nfor(int i=0;i<(int)_vecobj.size();i++){\n" +
			"json += " + app + "from"+classstruc.getTreatedClassName(true)+"ToJSON(_vecobj.at(i));\n" +
			"if(i!=(int)_vecobj.size()-1)json += \",\";\n" +
			"\n}\njson += \"]\";\nreturn json;\n}\n";
	test += "\nstring " + app + "from"+classstruc.getTreatedClassName(true)+"VecVPToJSON(vector<"+classstruc.getFullyQualifiedClassName()+">* _vecobj)\n{\nstring json = \"[\";\nfor(int i=0;i<_vecobj->size();i++){\n" +
			"json += " + app + "from"+classstruc.getTreatedClassName(true)+"ToJSON(_vecobj->at(i));\n" +
			"if(i!=_vecobj->size()-1)json += \",\";\n" +
			"\n}\njson += \"]\";\nreturn json;\n}\n";
	test += "\nstring " + app + "from"+classstruc.getTreatedClassName(true)+"LisVPToJSON(list<"+classstruc.getFullyQualifiedClassName()+">* _lisobj)\n{\nvector<"+classstruc.getFullyQualifiedClassName()+"> _vecobj;\nstd::copy(_lisobj->begin(), _lisobj->end(), std::back_inserter(_vecobj));" +
			"\nstring json = \"[\";\nfor(int i=0;i<(int)_vecobj.size();i++){\n" +
			"json += " + app + "from"+classstruc.getTreatedClassName(true)+"ToJSON(_vecobj.at(i));\n" +
			"if(i!=(int)_vecobj.size()-1)json += \",\";\n" +
			"\n}\njson += \"]\";\nreturn json;\n}\n";
	if(isOpForSet)
	{
		test += "\nstring " + app + "from"+classstruc.getTreatedClassName(true)+"SetVPToJSON(set<"+classstruc.getFullyQualifiedClassName()+">* _lisobj)\n{\nvector<"+classstruc.getFullyQualifiedClassName()+"> _vecobj;\nstd::copy(_lisobj->begin(), _lisobj->end(), std::back_inserter(_vecobj));" +
				"\nstring json = \"[\";\nfor(int i=0;i<(int)_vecobj.size();i++){\n" +
				"json += " + app + "from"+classstruc.getTreatedClassName(true)+"ToJSON(_vecobj.at(i));\n" +
				"if(i!=(int)_vecobj.size()-1)json += \",\";\n" +
				"\n}\njson += \"]\";\nreturn json;\n}\n";
		test += "\nstring " + app + "from"+classstruc.getTreatedClassName(true)+"MulSetVPToJSON(multiset<"+classstruc.getFullyQualifiedClassName()+">* _lisobj)\n{\nvector<"+classstruc.getFullyQualifiedClassName()+"> _vecobj;\nstd::copy(_lisobj->begin(), _lisobj->end(), std::back_inserter(_vecobj));" +
				"\nstring json = \"[\";\nfor(int i=0;i<(int)_vecobj.size();i++){\n" +
				"json += " + app + "from"+classstruc.getTreatedClassName(true)+"ToJSON(_vecobj.at(i));\n" +
				"if(i!=(int)_vecobj.size()-1)json += \",\";\n" +
				"\n}\njson += \"]\";\nreturn json;\n}\n";
	}
	test += "\nstring " + app + "from"+classstruc.getTreatedClassName(true)+"DqVPToJSON(deque<"+classstruc.getFullyQualifiedClassName()+">* _lisobj)\n{\nvector<"+classstruc.getFullyQualifiedClassName()+"> _vecobj;\nstd::copy(_lisobj->begin(), _lisobj->end(), std::back_inserter(_vecobj));" +
			"\nstring json = \"[\";\nfor(int i=0;i<(int)_vecobj.size();i++){\n" +
			"json += " + app + "from"+classstruc.getTreatedClassName(true)+"ToJSON(_vecobj.at(i));\n" +
			"if(i!=(int)_vecobj.size()-1)json += \",\";\n" +
			"\n}\njson += \"]\";\nreturn json;\n}\n";
	test += "\nstring " + app + "from"+classstruc.getTreatedClassName(true)+"QVPToJSON(std::queue<"+classstruc.getFullyQualifiedClassName()+">* _lisobj)\n{\nvector<"+classstruc.getFullyQualifiedClassName()+"> _vecobj;\n" +
			"std::queue<"+classstruc.getFullyQualifiedClassName()+"> qq = *_lisobj;\nfor (int var = 0; var < (int)qq.size(); ++var)\n{" +
			"_vecobj.push_back(qq.front());\nqq.pop();\n}\n" +
			"\nstring json = \"[\";\nfor(int i=0;i<(int)_vecobj.size();i++){\n" +
			"json += " + app + "from"+classstruc.getTreatedClassName(true)+"ToJSON(_vecobj.at(i));\n" +
			"if(i!=(int)_vecobj.size()-1)json += \",\";\n" +
			"\n}\njson += \"]\";\nreturn json;\n}\n";
	return test;
}


string AfcUtil::generateReadObjects(string type, string name, bool priv, bool ptr, string typ, string app, ClassStructure classstruc,Reflection ref)
{
	string tes;
	if(Reflection::isPrimitiveDataType(type))
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
		string tempp = type;//StringUtil::replaceAllCopy(type, " ", "");
		string fqcn = ref.getFullyQualifiedClassName(tempp, classstruc.getNamespaces());
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
			tes += generateReadVectorObjects(tempp, name, priv, ptr, typ, stlcnttyp, app, classstruc, ref);
			tes += "}\n}\n";

		}
		else if(fqcn=="Date" || fqcn=="BinaryData")
		{
			string tapp = "";
			StringUtil::replaceAll(fqcn, "::", "_");
			if(!ptr)
			{
				if(!priv)tes += "if(_node!=NULL)_obj"+typ + name + "= "+tapp+"read"+fqcn+"(*_node);\n";
				else tes += "if(_node!=NULL)_obj"+typ+"set"+camelCased(name)+"("+tapp+"read"+fqcn+"(*_node));\n";
			}
			else
			{
				if(!priv)tes += "if(_node!=NULL)_obj"+typ + name + "= "+tapp+"read"+fqcn+"P(*_node);\n";
				else tes += "if(_node!=NULL)_obj"+typ+"set"+camelCased(name)+"("+tapp+"read"+fqcn+"P(*_node));\n";
			}
		}
		else if(Reflection::isValidClass(fqcn, app))
		{
			string tapp = app;
			StringUtil::replaceAll(fqcn, "::", "_");
			if(!ptr)
			{
				if(!priv)tes += "if(_node!=NULL)_obj"+typ + name + "= "+tapp+"read"+fqcn+"(*_node);\n";
				else tes += "if(_node!=NULL)_obj"+typ+"set"+camelCased(name)+"("+tapp+"read"+fqcn+"(*_node));\n";
			}
			else
			{
				if(!priv)tes += "if(_node!=NULL)_obj"+typ + name + "= "+tapp+"read"+fqcn+"P(*_node);\n";
				else tes += "if(_node!=NULL)_obj"+typ+"set"+camelCased(name)+"("+tapp+"read"+fqcn+"P(*_node));\n";
			}
		}
	}
	return tes;
}


string AfcUtil::generateReadVectorObjects(string type, string name, bool priv, bool ptr, string typ, string conttype,string app, ClassStructure classstruc,Reflection ref)
{
	string tes;
	string act = ((conttype=="set" || conttype=="multiset")?"insert":(conttype=="std::queue"?"push":"push_back"));
	string fqcn = ref.getFullyQualifiedClassName(type, classstruc.getNamespaces());
	if(Reflection::isPrimitiveDataType(type))
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
	else if(fqcn=="Date" || fqcn=="BinaryData")
	{
		string tapp = "";
		string tfqcn = fqcn;
		StringUtil::replaceAll(tfqcn, "::", "_");
		if(!ptr)
		{
			if(!priv)tes += "if(__node!=NULL)_obj"+typ + name + "."+act+"("+tapp+"read"+tfqcn+"(*__node));\n";
			else tes += "if(__node!=NULL)_obj"+typ+"get"+camelCased(name)+"()."+act+"("+tapp+"read"+tfqcn+"(*__node));\n";
		}
		else
		{
			if(!priv)tes += "if(__node!=NULL){_obj"+typ + name + " = new "+conttype+"<"+fqcn+">;\n_obj"+typ + name + "->"+act+"("+tapp+"read"+tfqcn+"(*__node));\n}\n";
			else tes += "if(__node!=NULL){_obj"+typ +"set"+camelCased(name)+"(new "+conttype+"<"+fqcn+">);\n_obj"+typ+"get"+camelCased(name)+"()->"+act+"("+tapp+"read"+tfqcn+"(*__node));\n}\n";
		}
	}
	else if(Reflection::isValidClass(fqcn, app))
	{
		string tapp = app;
		string tfqcn = fqcn;
		StringUtil::replaceAll(tfqcn, "::", "_");
		if(!ptr)
		{
			if(!priv)tes += "if(__node!=NULL)_obj"+typ + name + "."+act+"("+tapp+"read"+tfqcn+"(*__node));\n";
			else tes += "if(__node!=NULL)_obj"+typ+"get"+camelCased(name)+"()."+act+"("+tapp+"read"+tfqcn+"(*__node));\n";
		}
		else
		{
			if(!priv)tes += "if(__node!=NULL){_obj"+typ + name + " = new "+conttype+"<"+fqcn+">;\n_obj"+typ + name + "->"+act+"("+tapp+"read"+tfqcn+"(*__node));\n}\n";
			else tes += "if(__node!=NULL){_obj"+typ +"set"+camelCased(name)+"(new "+conttype+"<"+fqcn+">);\n_obj"+typ+"get"+camelCased(name)+"()->"+act+"("+tapp+"read"+tfqcn+"(*__node));\n}\n";
		}
	}
	return tes;
}

string AfcUtil::generateToJSONObjects(string type, string name, bool priv, bool end, string &retu, string &headers, string &objs, string typ, bool ptr,string app, ClassStructure classstruc,Reflection ref)
{
	string fres;
	if(Reflection::isPrimitiveDataType(type))
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

		if(!end)
		{
			fres += "\njson += \",\";\n";
		}
	}
	else
	{
		string fqcn = ref.getFullyQualifiedClassName(type, classstruc.getNamespaces());
		string tempp = type;//StringUtil::replaceAllCopy(type, " ", "");
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

			string fqcn = ref.getFullyQualifiedClassName(tempp, classstruc.getNamespaces());

			fres += "json += \"\\\""+name+"\\\" : \";\n";
			if(!priv)
			{
				if(ptr)
				{
					fres += "if(_obj"+typ+name+"!=NULL)\n";
					fres += "json += JSONSerialize::serialize<"+stlcnttyp+"<"+fqcn+"> >(*_obj"+typ+name+", \""+app+"\");\n";
				}
				else
					fres += "json += JSONSerialize::serialize<"+stlcnttyp+"<"+fqcn+"> >(_obj"+typ+name+", \""+app+"\");\n";
			}
			else
			{
				if(ptr)
				{
					fres += "if(_obj"+typ+"get"+camelCased(name)+"()!=NULL)\n";
					fres += "json += JSONSerialize::serialize<"+stlcnttyp+"<"+fqcn+"> >(*_obj"+typ+"get"+camelCased(name)+"(), \""+app+"\");\n";
				}
				else
					fres += "json += JSONSerialize::serialize<"+stlcnttyp+"<"+fqcn+"> >(_obj"+typ+"get"+camelCased(name)+"(), \""+app+"\");\n";
			}
			//fres += generateToJSONVectorObjects(tempp, name, priv, retu, headers, path, objs, typ, ptr, stlcnttyp);
		}
		else if(fqcn=="Date" || fqcn=="BinaryData")
		{
			string tapp = "";
			StringUtil::replaceAll(fqcn, "::", "_");
			if(!ptr)
			{
				if(!priv)fres += "json += \"\\\""+name+"\\\" : \"+"+tapp+"from"+fqcn+"ToJSON(_obj"+typ+name+");";
				else fres += "json += \"\\\""+name+"\\\" : \"+"+tapp+"from"+fqcn+"ToJSON(_obj"+typ+"get"+camelCased(name)+"());";
			}
			else
			{
				if(!priv)fres += "if(_obj"+typ+name+"!=NULL)json += \"\\\""+name+"\\\" : \"+"+app+"from"+fqcn+"ToJSON(*_obj"+typ+name+");\nelse json += \"null\";\n";
				else fres += "if(_obj"+typ+"get"+camelCased(name)+"()!=NULL)json += \"\\\""+name+"\\\" : \"+"+app+"from"+fqcn+"ToJSON(*_obj"+typ+"get"+camelCased(name)+"());\nelse json += \"null\";\n";
			}
		}
		else if(Reflection::isValidClass(fqcn, app))
		{
			string tapp = app;
			StringUtil::replaceAll(fqcn, "::", "_");
			if(!ptr)
			{
				if(!priv)fres += "json += \"\\\""+name+"\\\" : \"+"+tapp+"from"+fqcn+"ToJSON(_obj"+typ+name+");";
				else fres += "json += \"\\\""+name+"\\\" : \"+"+tapp+"from"+fqcn+"ToJSON(_obj"+typ+"get"+camelCased(name)+"());";
			}
			else
			{
				if(!priv)fres += "if(_obj"+typ+name+"!=NULL)json += \"\\\""+name+"\\\" : \"+"+app+"from"+fqcn+"ToJSON(*_obj"+typ+name+");\nelse json += \"null\";\n";
				else fres += "if(_obj"+typ+"get"+camelCased(name)+"()!=NULL)json += \"\\\""+name+"\\\" : \"+"+app+"from"+fqcn+"ToJSON(*_obj"+typ+"get"+camelCased(name)+"());\nelse json += \"null\";\n";
			}
		}
		if(!end)
		{
			fres += "json += \",\";\n";
		}
	}
	return fres;
}

string AfcUtil::generateToJSONVectorObjects(string type, string name, bool priv, string &retu, string &headers, string &objs, string typ, bool ptr, string stlcnttyp,string app, ClassStructure classstruc,Reflection ref)
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
		string fqcn = ref.getFullyQualifiedClassName(type, classstruc.getNamespaces());
		if(stlcnttyp=="vector")
			fres += "for(int j=0;j<_obj"+typ+name+vtyp+"size();j++)\n{\n";
		else if(stlcnttyp=="list" || stlcnttyp=="set" || stlcnttyp=="multiset" || stlcnttyp=="deque")
			fres += stlcnttyp+"<"+fqcn+">::iterator it"+name+";\nint j"+name+" = 0;\nfor(it"+name+"=_obj"+typ+name+vtyp+"begin();it"+name+"!=_obj"+typ+name+vtyp+"end();it"+name+"++,j"+name+"++)\n{\n";
		else
		{
			if(!ptr)
				fres += "std::queue<"+fqcn+"> tt"+name+" = _obj"+typ+name+";\nif(!tt"+name+vtyp+"empty()){\n" +
					"for(int j=0;j<(int)tt"+name+vtyp+"size();j++)\n{\n";
			else
				fres += "std::queue<"+fqcn+">* tt"+name+" = _obj"+typ+name+";\nif(!tt"+name+"->empty()){\n" +
					"for(int j=0;j<(int)tt"+name+"->size();j++)\n{\n";
		}
	}
	else
	{
		string fqcn = ref.getFullyQualifiedClassName(type, classstruc.getNamespaces());
		if(stlcnttyp=="vector")
			fres += "for(int j=0;j<_obj"+typ+"get"+camelCased(name)+"()"+vtyp+"size();j++)\n{\n";
		else if(stlcnttyp=="list" || stlcnttyp=="set" || stlcnttyp=="multiset" || stlcnttyp=="deque")
			fres += stlcnttyp+"<"+fqcn+">::iterator it"+name+";\nint j"+name+" = 0;\nfor(it"+name+"=_obj"+typ+"get"+camelCased(name)+"()"+vtyp+"begin();it"+name+"!=_obj"+typ+"get"+camelCased(name)+"()"+vtyp+"end();it"+name+"++,j"+name+"++)\n{\n";
		else
		{
			if(!ptr)
				fres += "std::queue<"+fqcn+"> tt"+name+" = _obj"+typ+"get"+camelCased(name)+"();\nif(!tt"+name+vtyp+"empty()){\n" +
					"for(int j=0;j<(int)tt"+name+vtyp+"size();j++)\n{\n";
			else
				fres += "std::queue<"+fqcn+">* tt"+name+" = _obj"+typ+"get"+camelCased(name)+"();\nif(!tt"+name+"!=NULL && !tt"+name+"->empty()){\n" +
					"for(int j=0;j<(int)tt"+name+"->size();j++)\n{\n";
		}
	}
	string fqcn = ref.getFullyQualifiedClassName(type, classstruc.getNamespaces());
	if(Reflection::isPrimitiveDataType(type))
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
	else if(fqcn=="Date" || fqcn=="BinaryData")
	{
		string tapp = "";
		StringUtil::replaceAll(fqcn, "::", "_");
		if(!ptr)
		{
			if(stlcnttyp=="vector")
			{
				if(!priv)fres += "json += "+tapp+"from"+fqcn+"ToJSON(_obj"+typ+name+".at(j));";
				else fres += "json += "+tapp+"from"+fqcn+"ToJSON(_obj"+typ+"get"+camelCased(name)+"().at(j));";
			}
			else if(stlcnttyp=="list" || stlcnttyp=="set" || stlcnttyp=="multiset" || stlcnttyp=="deque")
			{
				if(!priv)fres += "json += "+tapp+"from"+fqcn+"ToJSON(*it"+name+");";
				else fres += "json += "+tapp+"from"+fqcn+"ToJSON(*it"+name+");";
			}
			else
			{
				if(!ptr)
				{
					if(!priv)fres += "json += "+tapp+"from"+fqcn+"ToJSON(tt"+name+".front());\ntt"+name+".pop();\n";
					else fres += "json += "+tapp+"from"+fqcn+"ToJSON(tt"+name+".front());\ntt"+name+".pop();\n";
				}
				else
				{
					if(!priv)fres += "json += "+tapp+"from"+fqcn+"ToJSON(tt"+name+"->front());\ntt"+name+"->pop();\n";
					else fres += "json += "+tapp+"from"+fqcn+"ToJSON(tt"+name+"->front());\ntt"+name+"->pop();\n";
				}
			}
		}
		else
		{
			if(stlcnttyp=="vector")
			{
				if(!priv)fres += "json += "+app+"from"+fqcn+"ToJSON(_obj"+typ+name+"->at(j));";
				else fres += "json += "+app+"from"+fqcn+"ToJSON(_obj"+typ+"get"+camelCased(name)+"()->at(j));";
			}
			else if(stlcnttyp=="list" || stlcnttyp=="set" || stlcnttyp=="multiset" || stlcnttyp=="deque")
			{
				if(!priv)fres += "json += "+app+"from"+fqcn+"ToJSON(*it"+name+");";
				else fres += "json += "+app+"from"+fqcn+"ToJSON(*it"+name+");";
			}
			else
			{
				if(!ptr)
				{
					if(!priv)fres += "json += "+app+"from"+fqcn+"ToJSON(tt"+name+".front());\ntt"+name+".pop();\n";
					else fres += "json += "+app+"from"+fqcn+"ToJSON(tt"+name+".front());\ntt"+name+".pop();\n";
				}
				else
				{
					if(!priv)fres += "json += "+app+"from"+fqcn+"ToJSON(tt"+name+"->front());\ntt"+name+"->pop();\n";
					else fres += "json += "+app+"from"+fqcn+"ToJSON(tt"+name+"->front());\ntt"+name+"->pop();\n";
				}
			}
		}
	}
	else if(Reflection::isValidClass(fqcn, app))
	{
		string tapp = app;
		StringUtil::replaceAll(fqcn, "::", "_");
		if(!ptr)
		{
			if(stlcnttyp=="vector")
			{
				if(!priv)fres += "json += "+tapp+"from"+fqcn+"ToJSON(_obj"+typ+name+".at(j));";
				else fres += "json += "+tapp+"from"+fqcn+"ToJSON(_obj"+typ+"get"+camelCased(name)+"().at(j));";
			}
			else if(stlcnttyp=="list" || stlcnttyp=="set" || stlcnttyp=="multiset" || stlcnttyp=="deque")
			{
				if(!priv)fres += "json += "+tapp+"from"+fqcn+"ToJSON(*it"+name+");";
				else fres += "json += "+tapp+"from"+fqcn+"ToJSON(*it"+name+");";
			}
			else
			{
				if(!ptr)
				{
					if(!priv)fres += "json += "+tapp+"from"+fqcn+"ToJSON(tt"+name+".front());\ntt"+name+".pop();\n";
					else fres += "json += "+tapp+"from"+fqcn+"ToJSON(tt"+name+".front());\ntt"+name+".pop();\n";
				}
				else
				{
					if(!priv)fres += "json += "+tapp+"from"+fqcn+"ToJSON(tt"+name+"->front());\ntt"+name+"->pop();\n";
					else fres += "json += "+tapp+"from"+fqcn+"ToJSON(tt"+name+"->front());\ntt"+name+"->pop();\n";
				}
			}
		}
		else
		{
			if(stlcnttyp=="vector")
			{
				if(!priv)fres += "json += "+app+"from"+fqcn+"ToJSON(_obj"+typ+name+"->at(j));";
				else fres += "json += "+app+"from"+fqcn+"ToJSON(_obj"+typ+"get"+camelCased(name)+"()->at(j));";
			}
			else if(stlcnttyp=="list" || stlcnttyp=="set" || stlcnttyp=="multiset" || stlcnttyp=="deque")
			{
				if(!priv)fres += "json += "+app+"from"+fqcn+"ToJSON(*it"+name+");";
				else fres += "json += "+app+"from"+fqcn+"ToJSON(*it"+name+");";
			}
			else
			{
				if(!ptr)
				{
					if(!priv)fres += "json += "+app+"from"+fqcn+"ToJSON(tt"+name+".front());\ntt"+name+".pop();\n";
					else fres += "json += "+app+"from"+fqcn+"ToJSON(tt"+name+".front());\ntt"+name+".pop();\n";
				}
				else
				{
					if(!priv)fres += "json += "+app+"from"+fqcn+"ToJSON(tt"+name+"->front());\ntt"+name+"->pop();\n";
					else fres += "json += "+app+"from"+fqcn+"ToJSON(tt"+name+"->front());\ntt"+name+"->pop();\n";
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

string AfcUtil::generateJsInterfaces(strVec obj,ClassStructure classstruc,string path,string &infjs,string appName,map<string, string> ajintpthMap, Reflection ref)
{
	string test,intf,intff,inc;
	//headers += "#include \"" + claz + ".h\"\n";
	//writeTofile("/home/sumeet/workspace/inter/AfcInclude.h",inc,false);
	//inc = "\nextern \"C\"{\n";//string executeAFC(string fn,strVec _inp){\nstring ret;\n";
	//bool fl = false;
	test = ("var " + classstruc.getTreatedClassName(false) + "= {\n");
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
					test += "AfcCall(\""+classstruc.getTreatedClassName(false)+"\",\""+emp.at(1)+"\",new Array("+jsonstr+"),_cb,(_url==null?\""+ajintpthMap[classstruc.getTreatedClassName(false)]+"\":_url),_cntxt);\n";
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
						if(Reflection::isPrimitiveDataType(emp.at(j)))
						{
							types.append(emp.at(j));
							types.append(" _");
							jsonstr += "_"+CastUtil::lexical_cast<string>(j-1);
							types.append(CastUtil::lexical_cast<string>(j-1));
							types.append(" = CastUtil::lexical_cast<");
							types.append(emp.at(j));
							types.append(">(_inp.at(");
							types.append(CastUtil::lexical_cast<string>(j-2));
							types.append("));\n");
						}
						else
						{
							string retType = emp.at(j);
							bool ptr = retType.find("*")!=string::npos;
							string tempp = StringUtil::replaceAllCopy(retType, " ", "");
							string fqcn = ref.getFullyQualifiedClassName(retType, classstruc.getNamespaces());
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

								string fqcn = ref.getFullyQualifiedClassName(tempp, classstruc.getNamespaces());

								types.append(emp.at(j));
								if(ptr)
									types.append("* _");
								else
									types.append(" _");
								jsonstr += "JSON.stringify(_"+CastUtil::lexical_cast<string>(j-1)+")";
								types.append(CastUtil::lexical_cast<string>(j-1));
								if(ptr)
									types.append(" = ("+emp.at(j)+"*)JSONSerialize::unSerializeUnknown<"+stlcnttyp+"<"+fqcn+"> >(_inp.at(");
								else
									types.append(" = JSONSerialize::unserialize<"+stlcnttyp+"<"+fqcn+"> >(_inp.at(");
								types.append(CastUtil::lexical_cast<string>(j-2));
								types.append("), \""+appName+"\");\n");
							}
							else if(fqcn=="Date" || fqcn=="BinaryData")
							{
								string tapp = "";
								types.append(retType);
								if(ptr)
									types.append("* _");
								else
									types.append(" _");

								jsonstr += "JSON.stringify(_"+CastUtil::lexical_cast<string>(j-1)+")";
								types.append(CastUtil::lexical_cast<string>(j-1));
								if(ptr)
									types.append(" = ("+fqcn+"*)JSONSerialize::unSerializeUnknown<"+fqcn+">(_inp.at(");
								else
									types.append(" = JSONSerialize::unserialize<"+fqcn+">(_inp.at(");
								types.append(CastUtil::lexical_cast<string>(j-2));
								types.append("), \""+appName+"\");\n");
							}
							else if(Reflection::isValidClass(fqcn, appName))
							{
								string tapp = appName;
								types.append(retType);
								if(ptr)
									types.append("* _");
								else
									types.append(" _");

								jsonstr += "JSON.stringify(_"+CastUtil::lexical_cast<string>(j-1)+")";
								types.append(CastUtil::lexical_cast<string>(j-1));
								if(ptr)
									types.append(" = ("+fqcn+"*)JSONSerialize::unSerializeUnknown<"+fqcn+">(_inp.at(");
								else
									types.append(" = JSONSerialize::unserialize<"+fqcn+">(_inp.at(");
								types.append(CastUtil::lexical_cast<string>(j-2));
								types.append("), \""+appName+"\");\n");
							}
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
					test += "AfcCall(\""+classstruc.getTreatedClassName(false)+"\",\""+emp.at(1)+"\",new Array("+jsonstr+"),_cb,(_url==null?\""+ajintpthMap[classstruc.getTreatedClassName(false)]+"\":_url),_cntxt);\n";
				}
				inc += updateAjaxInterface(emp,classstruc,pars,parswt,types,appName,ref);
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

string AfcUtil::updateAjaxInterface(strVec emp,ClassStructure classstruc,string pars,string parswt,string types,string appName, Reflection ref)
{
	string test;
	string retType = emp.at(0);
	string funcName = emp.at(1);
	bool ptr = retType.find("*")!=string::npos;
	StringUtil::replaceAll(retType, "*", "");
	StringUtil::replaceAll(retType, "&", "");
	//test = "#include \"" + claz + ".h\"\n#include "CastUtil.h"\n\n";
	test += ("string "+appName+"invokeAjaxMethodFor" + classstruc.getTreatedClassName(true) + funcName + "(strVec _inp");
	test += ")\n{\n" + types;
	test += (classstruc.getFullyQualifiedClassName() + " _obj;\n");
	if(retType=="void")
	{
		test += "return \"\";\n}\n";
	}
	else if(retType=="string")
	{
		test += "return _obj."+funcName+"("+pars+");\n}\n";
	}
	else if(Reflection::isPrimitiveDataType(retType))
	{
		test += "return CastUtil::lexical_cast<string>(_obj."+funcName+"("+pars+"));\n}\n";
	}
	else
	{
		string tempp = StringUtil::replaceAllCopy(retType, " ", "");
		string fqcn = ref.getFullyQualifiedClassName(tempp, classstruc.getNamespaces());
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

			string fqcn = ref.getFullyQualifiedClassName(tempp, classstruc.getNamespaces());

			if(ptr)
			{
				test += "if(_obj."+funcName+"("+pars+")!=NULL)\n";
				test += "return JSONSerialize::serialize<"+stlcnttyp+"<"+fqcn+"> >(*_obj."+funcName+"("+pars+"), \""+appName+"\");\n";
				test += "else return \"\";\n";
			}
			else
				test += "return JSONSerialize::serialize<"+stlcnttyp+"<"+fqcn+"> >(_obj."+funcName+"("+pars+"), \""+appName+"\");\n";
			test += "\n}\n";
		}
		else if(fqcn=="Date" || fqcn=="BinaryData")
		{
			if(ptr)
			{
				test += "if(_obj."+funcName+"("+pars+")!=NULL)\n";
				test += "return JSONSerialize::serialize<"+fqcn+">(*_obj."+funcName+"("+pars+"), \""+appName+"\");\n";
				test += "else return \"\";\n";
			}
			else
				test += "return JSONSerialize::serialize<"+fqcn+">(_obj."+funcName+"("+pars+"), \""+appName+"\");\n";
			test += "\n}\n";
		}
		else if(Reflection::isValidClass(fqcn, appName))
		{
			if(ptr)
			{
				test += "if(_obj."+funcName+"("+pars+")!=NULL)\n";
				test += "return JSONSerialize::serialize<"+fqcn+">(*_obj."+funcName+"("+pars+"), \""+appName+"\");\n";
				test += "else return \"\";\n";
			}
			else
				test += "return JSONSerialize::serialize<"+fqcn+">(_obj."+funcName+"("+pars+"), \""+appName+"\");\n";
			test += "\n}\n";
		}
		else
		{
			test += "return \"\";\n}\n";
		}
	}
	return test;
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
