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
typedef std::map<std::string,strVec> mapofvec;

AfcUtil::AfcUtil() {
}

AfcUtil::~AfcUtil() {
	// TODO Auto-generated destructor stub
}
static std::map<std::string,std::string> doneMap;
std::string AfcUtil::generateJsInterfacessAll(std::map<std::string, ClassStructure>& allclsmap, std::string &infjs, std::map<std::string, std::string>& ajintpthMap, strVec& afcd, Reflection& ref)
{
	std::string ret;
	/*map<std::string, ClassStructure> allclsmap;
	for (unsigned int var = 0; var < obj.size(); ++var)
	{
		std::map<std::string, ClassStructure> clsmap = ref.getClassStructures(obj.at(var)+files.at(var)+".h");
		std::map<std::string, ClassStructure>::iterator it;
		for (it=clsmap.begin();it!=clsmap.end();++it)
		{
			it->second.appName = pv.at(var);
		}
		allclsmap.insert(clsmap.begin(), clsmap.end());
	}*/
	std::map<std::string, ClassStructure>::iterator it;
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

std::string AfcUtil::generateJsObjectsAll(std::map<std::string, ClassStructure>& allclsmap)
{
	Reflection ref;
	std::string ret;
	/*strVec includes = ref.list(includeDir);
	std::map<std::string, ClassStructure> allclsmap;
	for (unsigned int var = 0; var < includes.size(); ++var)
	{
		std::map<std::string, ClassStructure> clsmap = ref.getClassStructures(includes.at(var));
		allclsmap.insert(clsmap.begin(), clsmap.end());
	}*/
	std::map<std::string, ClassStructure>::iterator it;
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


std::string AfcUtil::generateJsObjects(strVec obj, const std::string& claz, strVec pobj, const strVec& minfo)
{
	if(doneMap.find(claz)==doneMap.end())
		doneMap[claz] = "done";
	else
		return "";
	Reflection ref;
	std::string retu;
	std::string test = "function _"+claz+"()\n{\n";
	bool priv = false;
	size_t tess;
	std::string meth;
	strVec fldnames;
	std::map<std::string, int> fldstat;
	for (unsigned int i = 0; i < pobj.size(); i++)
	{
		if(((tess=pobj.at(i).find("("))==std::string::npos && (tess=pobj.at(i).find(")"))==std::string::npos && pobj.at(i).find("~")==std::string::npos))
		{
			std::string fld = pobj.at(i);

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
			if(fld.find("*")!=std::string::npos)
			{
				ptr = true;
				StringUtil::replaceFirst(fld,"*","");
			}

			std::vector<std::string> fldp;
			fldp.push_back(fld.substr(0, fld.find_last_of(" ")));
			fldp.push_back(fld.substr(fld.find_last_of(" ")+1));

			if(fldp.size()==2)
			{
				std::string nam = fldp.at(1);
				fldnames.push_back(fldp.at(0));
				fldnames.push_back(nam);
				fldstat[nam] = 0;
			}
		}
	}
	for(unsigned int i=0;i<minfo.size();i++)
	{
		if((tess=minfo.at(i).find("("))!=std::string::npos && (tess=minfo.at(i).find(")"))!=std::string::npos && minfo.at(i).find("~")==std::string::npos
							&& fldnames.size()>0)
		{
			meth = minfo.at(i);
			StringUtil::replaceFirst(meth,";","");

			std::string argts = meth.substr(meth.find("("),meth.find(")")-meth.find("("));
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
			std::vector<std::string> methp,methpm,argp,argpm,argpmtemp;
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
					std::string argpmtemp = argp.at(j);
					StringUtil::trim(argpmtemp);
					std::string typ;
					if(argpmtemp.find("*")!=std::string::npos)
						typ = "*";
					else if(argpmtemp.find("&")!=std::string::npos)
						typ = "&";
					if(argpmtemp.find(" ")!=std::string::npos)
					{
						argpmtemp = argpmtemp.substr(0, argpmtemp.find_last_of(" "));
					}
					StringUtil::trim(argpmtemp);
					argpm.push_back(argpmtemp);
				}
			}
			bool ptr = false;
			if(meth.find("*")!=std::string::npos)
			{
				ptr = true;
				StringUtil::replaceFirst(meth,"*","");
			}

			if(meth.find(" operator")!=std::string::npos)
			{
				meth = meth.substr(meth.find(" operator"));
				StringUtil::replaceAll(meth, " ", "");
			}
			if(meth.find("operator<")!=std::string::npos || meth.find("operator>")!=std::string::npos || meth.find("operator<=")!=std::string::npos
				|| meth.find("operator>=")!=std::string::npos || meth.find("operator==")!=std::string::npos || meth.find("operator!=")!=std::string::npos || meth.find("operator!")!=std::string::npos
				|| meth.find("operator<<")!=std::string::npos || meth.find("operator>>")!=std::string::npos || meth.find("operator+")!=std::string::npos || meth.find("operator-")!=std::string::npos
				|| meth.find("operator*")!=std::string::npos || meth.find("operator/")!=std::string::npos || meth.find("operator[]")!=std::string::npos || meth.find("operator()")!=std::string::npos
				|| meth.find("operator&")!=std::string::npos || meth.find("operator&&")!=std::string::npos || meth.find("operator||")!=std::string::npos || meth.find("operator|")!=std::string::npos)
			{}
			else
			{
				if(methpm.at(0)!=claz)
				{
					for(unsigned int k = 0; k < fldnames.size(); k=k+2)
					{
						std::string cam = AfcUtil::camelCased(fldnames.at(k+1));
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
			std::string data = fldnames.at(k) + " " + fldnames.at(k+1) + ";";
			//logger << "===========> " << data << std::endl;
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
		std::string data = obj.at(i);
		//bool ptr = false;
		if(data.find("*")!=std::string::npos)
		{
			//ptr = true;
			StringUtil::replaceFirst(data,"*","");
		}
		StringUtil::replaceFirst(data,";","");
		StringUtil::split(vemp, data, (" "));
		if(vemp.size()<2)
		{
			//logger << data << " error" <<  std::endl;
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
		std::string data = obj.at(i);
		bool ptr = false;
		if(data.find("*")!=std::string::npos)
		{
			ptr = true;
			StringUtil::replaceFirst(data,"*","");
		}
		StringUtil::replaceFirst(data,";","");
		if(data.find(" ")==std::string::npos)
		{
			//logger << data << " error" <<  std::endl;
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

std::string AfcUtil::generateJsObjects(const strVec& obj, ClassStructure& classstruc, std::string &headers, std::string &objs, strVec pobj, const bool& isOpForSet, std::string& typrefs, const strVec& minfo, const std::string& app, const std::string& clspth, Reflection& ref)
{
	if(doneMap.find(classstruc.getFullyQualifiedClassName())==doneMap.end())
		doneMap[classstruc.getFullyQualifiedClassName()] = "done";
	else
		return "";
	std::string tes,tes1,fres,fres1;
	std::string retu;
	std::string test = "function _"+classstruc.getTreatedClassName(false)+"()\n{\n";
	tes += "JSONElement* _node;\n";
	tes1 += "JSONElement* _node;\n";
	fres = "std::string json=\"{\";\n";
	fres1 = "std::string json=\"{\";\n";
	bool priv = false;
	size_t tess;
	std::string meth;
	strVec fldnames;
	std::map<std::string, int> fldstat;
	std::map<std::string, bool> fldptr;
	for (unsigned int i = 0; i < pobj.size(); i++)
	{
		if(((tess=pobj.at(i).find("("))==std::string::npos && (tess=pobj.at(i).find(")"))==std::string::npos && pobj.at(i).find("~")==std::string::npos))
		{
			std::string fld = pobj.at(i);

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
			if(fld.find("*")!=std::string::npos)
			{
				ptr = true;
				StringUtil::replaceFirst(fld,"*","");
			}

			std::vector<std::string> fldp;
			fldp.push_back(fld.substr(0, fld.find_last_of(" ")));
			fldp.push_back(fld.substr(fld.find_last_of(" ")+1));

			if(fldp.size()==2)
			{
				std::string nam = fldp.at(1);
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
		if((tess=minfo.at(i).find("("))!=std::string::npos && (tess=minfo.at(i).find(")"))!=std::string::npos && minfo.at(i).find("~")==std::string::npos
							&& fldnames.size()>0)
		{
			meth = minfo.at(i);
			StringUtil::replaceFirst(meth,";","");

			std::string argts = meth.substr(meth.find("("),meth.find(")")-meth.find("("));
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
			std::vector<std::string> methp,methpm,argp,argpm,argpmtemp;
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
					std::string argpmtemp = argp.at(j);
					StringUtil::trim(argpmtemp);
					std::string typ;
					if(argpmtemp.find("*")!=std::string::npos)
						typ = "*";
					else if(argpmtemp.find("&")!=std::string::npos)
						typ = "&";
					if(argpmtemp.find(" ")!=std::string::npos)
					{
						argpmtemp = argpmtemp.substr(0, argpmtemp.find_last_of(" "));
					}
					StringUtil::trim(argpmtemp);
					argpm.push_back(argpmtemp);
				}
			}
			bool ptr = false;
			if(meth.find("*")!=std::string::npos)
			{
				ptr = true;
				StringUtil::replaceFirst(meth,"*","");
			}

			if(meth.find(" operator")!=std::string::npos)
			{
				meth = meth.substr(meth.find(" operator"));
				StringUtil::replaceAll(meth, " ", "");
			}
			if(meth.find("operator<")!=std::string::npos || meth.find("operator>")!=std::string::npos || meth.find("operator<=")!=std::string::npos
				|| meth.find("operator>=")!=std::string::npos || meth.find("operator==")!=std::string::npos || meth.find("operator!=")!=std::string::npos || meth.find("operator!")!=std::string::npos
				|| meth.find("operator<<")!=std::string::npos || meth.find("operator>>")!=std::string::npos || meth.find("operator+")!=std::string::npos || meth.find("operator-")!=std::string::npos
				|| meth.find("operator*")!=std::string::npos || meth.find("operator/")!=std::string::npos || meth.find("operator[]")!=std::string::npos || meth.find("operator()")!=std::string::npos
				|| meth.find("operator&")!=std::string::npos || meth.find("operator&&")!=std::string::npos || meth.find("operator||")!=std::string::npos || meth.find("operator|")!=std::string::npos)
			{}
			else
			{
				if(methpm.at(0)!=classstruc.getTreatedClassName(false))
				{
					for(unsigned int k = 0; k < fldnames.size(); k=k+2)
					{
						std::string cam = AfcUtil::camelCased(fldnames.at(k+1));
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
			std::string data = fldnames.at(k) + " " + fldnames.at(k+1) + ";";

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
		std::string data = obj.at(i);
		//bool ptr = false;
		if(data.find("*")!=std::string::npos)
		{
			//ptr = true;
			StringUtil::replaceFirst(data,"*","");
		}
		StringUtil::replaceFirst(data,";","");
		StringUtil::split(vemp, data, (" "));
		if(vemp.size()<2)
		{
			//logger << data << " error" <<  std::endl;
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
		std::string data = obj.at(i);
		bool ptr = false;
		if(data.find("*")!=std::string::npos)
		{
			ptr = true;
			StringUtil::replaceFirst(data,"*","");
		}
		StringUtil::replaceFirst(data,";","");
		StringUtil::split(vemp, data, (" "));
		if(vemp.size()<2)
		{
			//logger << data << " error" <<  std::endl;
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
		std::string data = pobj.at(i);
		bool ptr = false;
		if(data.find("*")!=std::string::npos)
		{
			ptr = true;
			StringUtil::replaceFirst(data,"*","");
		}
		StringUtil::replaceFirst(data,";","");
		StringUtil::split(vemp, data, (" "));
		if(vemp.size()<2)
		{
			logger << data << " error" <<  std::endl;
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
	//logger << fres << std::flush;
	test += "}";
	//logger << test << std::flush;
	objs += test;
	headers += "#include \""+clspth+"\"\n";

	typrefs += classstruc.getFullyQualifiedClassName() + " " + app + "read"+classstruc.getTreatedClassName(true)+"(JSONElement& obj);\n" + classstruc.getFullyQualifiedClassName() + "* "+app+"read"+classstruc.getTreatedClassName(true)+"P(JSONElement& obj);\n";
	typrefs += "\nstd::string " + app + "from"+classstruc.getTreatedClassName(true)+"ToJSON("+classstruc.getFullyQualifiedClassName()+" _obj);\nstd::string " + app + "from"+classstruc.getTreatedClassName(true)+"VPToJSON("+classstruc.getFullyQualifiedClassName()+"* _obj);\n";
	typrefs	+= classstruc.getFullyQualifiedClassName() + " " + app + "to"+classstruc.getTreatedClassName(true)+"(std::string s);\n" + classstruc.getFullyQualifiedClassName() + "* " + app + "to"+classstruc.getTreatedClassName(true)+"P(std::string s);\n";
	typrefs	+= "std::vector<"+classstruc.getFullyQualifiedClassName()+"> " + app + "to"+classstruc.getTreatedClassName(true)+"Vec(std::string s);\nstd::vector<"+classstruc.getFullyQualifiedClassName()+">* " + app + "to"+classstruc.getTreatedClassName(true)+"VecVP(std::string s);\n";
	typrefs	+= "std::list<"+classstruc.getFullyQualifiedClassName()+"> " + app + "to"+classstruc.getTreatedClassName(true)+"Lis(std::string s);\nstd::list<"+classstruc.getFullyQualifiedClassName()+">* " + app + "to"+classstruc.getTreatedClassName(true)+"LisVP(std::string s);\n";
	typrefs	+= "std::deque<"+classstruc.getFullyQualifiedClassName()+"> " + app + "to"+classstruc.getTreatedClassName(true)+"Dq(std::string s);\nstd::deque<"+classstruc.getFullyQualifiedClassName()+">* " + app + "to"+classstruc.getTreatedClassName(true)+"DqVP(std::string s);\n";
	typrefs	+= "std::queue<"+classstruc.getFullyQualifiedClassName()+"> " + app + "to"+classstruc.getTreatedClassName(true)+"Q(std::string s);\nstd::queue<"+classstruc.getFullyQualifiedClassName()+">* " + app + "to"+classstruc.getTreatedClassName(true)+"QVP(std::string s);\n";
	if(isOpForSet)
	{
		typrefs	+= "std::set<"+classstruc.getFullyQualifiedClassName()+"> " + app + "to"+classstruc.getTreatedClassName(true)+"Set(std::string s);\nstd::set<"+classstruc.getFullyQualifiedClassName()+">* " + app + "to"+classstruc.getTreatedClassName(true)+"SetVP(std::string s);\n";
		typrefs	+= "std::multiset<"+classstruc.getFullyQualifiedClassName()+"> " + app + "to"+classstruc.getTreatedClassName(true)+"MulSet(std::string s);\nstd::multiset<"+classstruc.getFullyQualifiedClassName()+">* " + app + "to"+classstruc.getTreatedClassName(true)+"MulSetVP(std::string s);\n";
	}
	typrefs += "\nstd::string " + app + "from"+classstruc.getTreatedClassName(true)+"VecToJSON(std::vector<"+classstruc.getFullyQualifiedClassName()+"> _obj);\nstd::string " + app + "from"+classstruc.getTreatedClassName(true)+"VecVPToJSON(std::vector<"+classstruc.getFullyQualifiedClassName()+">* _obj);\n";
	typrefs += "\nstd::string " + app + "from"+classstruc.getTreatedClassName(true)+"LisToJSON(std::list<"+classstruc.getFullyQualifiedClassName()+"> _obj);\nstd::string " + app + "from"+classstruc.getTreatedClassName(true)+"LisVPToJSON(std::list<"+classstruc.getFullyQualifiedClassName()+">* _obj);\n";
	typrefs += "\nstd::string " + app + "from"+classstruc.getTreatedClassName(true)+"DqToJSON(std::deque<"+classstruc.getFullyQualifiedClassName()+"> _obj);\nstd::string " + app + "from"+classstruc.getTreatedClassName(true)+"DqVPToJSON(std::deque<"+classstruc.getFullyQualifiedClassName()+">* _obj);\n";
	typrefs += "\nstd::string " + app + "from"+classstruc.getTreatedClassName(true)+"QToJSON(std::queue<"+classstruc.getFullyQualifiedClassName()+"> _obj);\nstd::string " + app + "from"+classstruc.getTreatedClassName(true)+"QVPToJSON(std::queue<"+classstruc.getFullyQualifiedClassName()+">* _obj);\n";
	if(isOpForSet)
	{
		typrefs += "\nstd::string " + app + "from"+classstruc.getTreatedClassName(true)+"SetToJSON(std::set<"+classstruc.getFullyQualifiedClassName()+"> _obj);\nstd::string " + app + "from"+classstruc.getTreatedClassName(true)+"SetVPToJSON(std::set<"+classstruc.getFullyQualifiedClassName()+">* _obj);\n";
		typrefs += "\nstd::string " + app + "from"+classstruc.getTreatedClassName(true)+"MulSetToJSON(std::multiset<"+classstruc.getFullyQualifiedClassName()+"> _obj);\nstd::string " + app + "from"+classstruc.getTreatedClassName(true)+"MulSetVPToJSON(std::multiset<"+classstruc.getFullyQualifiedClassName()+">* _obj);\n";
	}

	test = retu+ "\n\n" + classstruc.getFullyQualifiedClassName() + " " + app + "read"+classstruc.getTreatedClassName(true)+"(JSONElement& obj)\n{\n"+classstruc.getFullyQualifiedClassName()+" _obj;\n";
	test += tes + "\nreturn _obj;\n}\n";
	test += "\n\n" + classstruc.getFullyQualifiedClassName() + "* "+app+"read"+classstruc.getTreatedClassName(true)+"P(JSONElement& obj)\n{\n"+classstruc.getFullyQualifiedClassName()+"* _obj = new "+classstruc.getFullyQualifiedClassName()+";\n";
	test += tes1 + "\nreturn _obj;\n}\n";
	test += classstruc.getFullyQualifiedClassName() + " " + app + "to"+classstruc.getTreatedClassName(true)+"(std::string s)\n{\nJSONElement element = JSONUtil::getDocument(s);\n"+ classstruc.getFullyQualifiedClassName() +" _obj = " + app + "read"+classstruc.getTreatedClassName(true)+"(element);\nreturn _obj;\n}\n";
	test += "std::vector<"+classstruc.getFullyQualifiedClassName()+"> " + app + "to"+classstruc.getTreatedClassName(true)+"Vec(std::string s)\n{\nJSONElement element = JSONUtil::getDocument(s);\n" +
			"std::vector<"+classstruc.getFullyQualifiedClassName()+"> vec;\nfor(int i=0;i<(int)element.getChildren().size();i++){\n" +
			classstruc.getFullyQualifiedClassName() +" _obj = " + app + "read"+classstruc.getTreatedClassName(true)+"(*element.getChildren().at(i));\nvec.push_back(_obj);\n" +
			"}\nreturn vec;\n}\n";
	test += "std::list<"+classstruc.getFullyQualifiedClassName()+"> " + app + "to"+classstruc.getTreatedClassName(true)+"Lis(std::string s)\n{\nstd::vector<"+classstruc.getFullyQualifiedClassName()+"> vec = " + app + "to"+classstruc.getTreatedClassName(true)+"Vec(s);\n" +
			"std::list<"+classstruc.getFullyQualifiedClassName()+"> tt;\nstd::copy(vec.begin(), vec.end(), std::back_inserter(tt));\n" +
			"return tt;\n}\n";
	if(isOpForSet)
	{
		test += "std::set<"+classstruc.getFullyQualifiedClassName()+"> " + app + "to"+classstruc.getTreatedClassName(true)+"Set(std::string s)\n{\nstd::vector<"+classstruc.getFullyQualifiedClassName()+"> vec = " + app + "to"+classstruc.getTreatedClassName(true)+"Vec(s);\n" +
				"std::set<"+classstruc.getFullyQualifiedClassName()+"> tt;\nstd::copy(vec.begin(), vec.end(), std::inserter(tt, tt.begin()));\n" +
				"return tt;\n}\n";
		test += "std::multiset<"+classstruc.getFullyQualifiedClassName()+"> " + app + "to"+classstruc.getTreatedClassName(true)+"MulSet(std::string s)\n{\nstd::vector<"+classstruc.getFullyQualifiedClassName()+"> vec = " + app + "to"+classstruc.getTreatedClassName(true)+"Vec(s);\n" +
				"std::multiset<"+classstruc.getFullyQualifiedClassName()+"> tt;\nstd::copy(vec.begin(), vec.end(), std::inserter(tt, tt.begin()));\n" +
				"return tt;\n}\n";
	}
	test += "std::deque<"+classstruc.getFullyQualifiedClassName()+"> " + app + "to"+classstruc.getTreatedClassName(true)+"Dq(std::string s)\n{\nstd::vector<"+classstruc.getFullyQualifiedClassName()+"> vec = " + app + "to"+classstruc.getTreatedClassName(true)+"Vec(s);\n" +
			"std::deque<"+classstruc.getFullyQualifiedClassName()+"> tt;\nstd::copy(vec.begin(), vec.end(), std::inserter(tt, tt.begin()));\n" +
			"return tt;\n}\n";
	test += "std::queue<"+classstruc.getFullyQualifiedClassName()+"> " + app + "to"+classstruc.getTreatedClassName(true)+"Q(std::string s)\n{\nstd::vector<"+classstruc.getFullyQualifiedClassName()+"> vec = " + app + "to"+classstruc.getTreatedClassName(true)+"Vec(s);\n" +
			"std::queue<"+classstruc.getFullyQualifiedClassName()+"> tt;for (int var = 0; var < (int)vec.size(); ++var) {\ntt.push(vec.at(var));\n}\n" +
			"return tt;\n}\n";
	test += "std::vector<"+classstruc.getFullyQualifiedClassName()+">* " + app + "to"+classstruc.getTreatedClassName(true)+"VecVP(std::string s)\n{\nJSONElement element = JSONUtil::getDocument(s);\n" +
			"std::vector<"+classstruc.getFullyQualifiedClassName()+">* vec = new std::vector<"+classstruc.getFullyQualifiedClassName()+">;\nfor(int i=0;i<element.getChildren().size();i++){\n" +
			classstruc.getFullyQualifiedClassName() +" _obj = " + app + "read"+classstruc.getTreatedClassName(true)+"(*element.getChildren().at(i));\nvec->push_back(_obj);\n" +
			"}\nreturn vec;\n}\n";
	test += "std::list<"+classstruc.getFullyQualifiedClassName()+">* " + app + "to"+classstruc.getTreatedClassName(true)+"LisVP(std::string s)\n{\nstd::vector<"+classstruc.getFullyQualifiedClassName()+"> vec = " + app + "to"+classstruc.getTreatedClassName(true)+"Vec(s);\n" +
			"std::list<"+classstruc.getFullyQualifiedClassName()+">* tt = new std::list<"+classstruc.getFullyQualifiedClassName()+">;\nstd::copy(vec.begin(), vec.end(), std::back_inserter(*tt));\n" +
			"return tt;\n}\n";
	if(isOpForSet)
	{
		test += "std::set<"+classstruc.getFullyQualifiedClassName()+">* " + app + "to"+classstruc.getTreatedClassName(true)+"SetVP(std::string s)\n{\nstd::vector<"+classstruc.getFullyQualifiedClassName()+"> vec = " + app + "to"+classstruc.getTreatedClassName(true)+"Vec(s);\n" +
			"std::set<"+classstruc.getFullyQualifiedClassName()+">* tt = new std::set<"+classstruc.getFullyQualifiedClassName()+">;\nstd::copy(vec.begin(), vec.end(), std::inserter(*tt, tt->begin()));\n" +
			"return tt;\n}\n";
		test += "std::multiset<"+classstruc.getFullyQualifiedClassName()+">* " + app + "to"+classstruc.getTreatedClassName(true)+"MulSetVP(std::string s)\n{\nstd::vector<"+classstruc.getFullyQualifiedClassName()+"> vec = " + app + "to"+classstruc.getTreatedClassName(true)+"Vec(s);\n" +
				"std::multiset<"+classstruc.getFullyQualifiedClassName()+">* tt = new std::multiset<"+classstruc.getFullyQualifiedClassName()+">;\nstd::copy(vec.begin(), vec.end(), std::inserter(*tt, tt->begin()));\n" +
				"return tt;\n}\n";
	}
	test += "std::deque<"+classstruc.getFullyQualifiedClassName()+">* " + app + "to"+classstruc.getTreatedClassName(true)+"DqVP(std::string s)\n{\nstd::vector<"+classstruc.getFullyQualifiedClassName()+"> vec = " + app + "to"+classstruc.getTreatedClassName(true)+"Vec(s);\n" +
			"std::deque<"+classstruc.getFullyQualifiedClassName()+">* tt = new std::deque<"+classstruc.getFullyQualifiedClassName()+">;\nstd::copy(vec.begin(), vec.end(), std::inserter(*tt, tt->begin()));\n" +
			"return tt;\n}\n";
	test += "std::queue<"+classstruc.getFullyQualifiedClassName()+">* " + app + "to"+classstruc.getTreatedClassName(true)+"QVP(std::string s)\n{\nstd::vector<"+classstruc.getFullyQualifiedClassName()+"> vec = " + app + "to"+classstruc.getTreatedClassName(true)+"Vec(s);\n" +
			"std::queue<"+classstruc.getFullyQualifiedClassName()+">* tt = new std::queue<"+classstruc.getFullyQualifiedClassName()+">;for (int var = 0; var < (int)vec.size(); ++var) {\ntt->push(vec.at(var));\n}\n" +
			"return tt;\n}\n";
	test += "void* " + app + "toVoidP"+classstruc.getTreatedClassName(true)+"(std::string s)\n{\nJSONElement element = JSONUtil::getDocument(s);\n"+ classstruc.getFullyQualifiedClassName()+"*_obj = new "+classstruc.getFullyQualifiedClassName()+";\n*_obj = " + app + "read"+classstruc.getTreatedClassName(true)+"(element);\nreturn _obj;\n}\n";
	test += "\nstd::string " + app + "from"+classstruc.getTreatedClassName(true)+"ToJSON("+classstruc.getFullyQualifiedClassName()+" _obj)\n{\n"+fres+"\nreturn json;\n}\n";
	test += "\nstd::string " + app + "from"+classstruc.getTreatedClassName(true)+"VPToJSON("+classstruc.getFullyQualifiedClassName()+"* _obj)\n{\n"+fres1+"\nreturn json;\n}\n";
	test += "\nstd::string " + app + "from"+classstruc.getTreatedClassName(true)+"VecToJSON(std::vector<"+classstruc.getFullyQualifiedClassName()+"> _vecobj)\n{\nstd::string json = \"[\";\nfor(int i=0;i<(int)_vecobj.size();i++){\n" +
			"json += " + app + "from"+classstruc.getTreatedClassName(true)+"ToJSON(_vecobj.at(i));\n" +
			"if(i!=(int)_vecobj.size()-1)json += \",\";\n" +
			"\n}\njson += \"]\";\nreturn json;\n}\n";
	test += "\nstd::string " + app + "from"+classstruc.getTreatedClassName(true)+"LisToJSON(std::list<"+classstruc.getFullyQualifiedClassName()+"> _lisobj)\n{\nstd::vector<"+classstruc.getFullyQualifiedClassName()+"> _vecobj;\nstd::copy(_lisobj.begin(), _lisobj.end(), std::back_inserter(_vecobj));" +
			"\nstd::string json = \"[\";\nfor(int i=0;i<(int)_vecobj.size();i++){\n" +
			"json += " + app + "from"+classstruc.getTreatedClassName(true)+"ToJSON(_vecobj.at(i));\n" +
			"if(i!=(int)_vecobj.size()-1)json += \",\";\n" +
			"\n}\njson += \"]\";\nreturn json;\n}\n";
	if(isOpForSet)
	{
		test += "\nstd::string " + app + "from"+classstruc.getTreatedClassName(true)+"SetToJSON(std::set<"+classstruc.getFullyQualifiedClassName()+"> _lisobj)\n{\nstd::vector<"+classstruc.getFullyQualifiedClassName()+"> _vecobj;\nstd::copy(_lisobj.begin(), _lisobj.end(), std::back_inserter(_vecobj));" +
				"\nstd::string json = \"[\";\nfor(int i=0;i<(int)_vecobj.size();i++){\n" +
				"json += " + app + "from"+classstruc.getTreatedClassName(true)+"ToJSON(_vecobj.at(i));\n" +
				"if(i!=(int)_vecobj.size()-1)json += \",\";\n" +
				"\n}\njson += \"]\";\nreturn json;\n}\n";
		test += "\nstd::string " + app + "from"+classstruc.getTreatedClassName(true)+"MulSetToJSON(std::multiset<"+classstruc.getFullyQualifiedClassName()+"> _lisobj)\n{\nstd::vector<"+classstruc.getFullyQualifiedClassName()+"> _vecobj;\nstd::copy(_lisobj.begin(), _lisobj.end(), std::back_inserter(_vecobj));" +
				"\nstd::string json = \"[\";\nfor(int i=0;i<(int)_vecobj.size();i++){\n" +
				"json += " + app + "from"+classstruc.getTreatedClassName(true)+"ToJSON(_vecobj.at(i));\n" +
				"if(i!=(int)_vecobj.size()-1)json += \",\";\n" +
				"\n}\njson += \"]\";\nreturn json;\n}\n";
	}
	test += "\nstd::string " + app + "from"+classstruc.getTreatedClassName(true)+"DqToJSON(std::deque<"+classstruc.getFullyQualifiedClassName()+"> _lisobj)\n{\nstd::vector<"+classstruc.getFullyQualifiedClassName()+"> _vecobj;\nstd::copy(_lisobj.begin(), _lisobj.end(), std::back_inserter(_vecobj));" +
			"\nstd::string json = \"[\";\nfor(int i=0;i<(int)_vecobj.size();i++){\n" +
			"json += " + app + "from"+classstruc.getTreatedClassName(true)+"ToJSON(_vecobj.at(i));\n" +
			"if(i!=(int)_vecobj.size()-1)json += \",\";\n" +
			"\n}\njson += \"]\";\nreturn json;\n}\n";
	test += "\nstd::string " + app + "from"+classstruc.getTreatedClassName(true)+"QToJSON(std::queue<"+classstruc.getFullyQualifiedClassName()+"> _lisobj)\n{\nstd::vector<"+classstruc.getFullyQualifiedClassName()+"> _vecobj;" +
			"std::queue<"+classstruc.getFullyQualifiedClassName()+"> qq = _lisobj;\nfor (int var = 0; var < (int)qq.size(); ++var)\n{" +
			"_vecobj.push_back(qq.front());\nqq.pop();\n}\n" +
			"\nstd::string json = \"[\";\nfor(int i=0;i<(int)_vecobj.size();i++){\n" +
			"json += " + app + "from"+classstruc.getTreatedClassName(true)+"ToJSON(_vecobj.at(i));\n" +
			"if(i!=(int)_vecobj.size()-1)json += \",\";\n" +
			"\n}\njson += \"]\";\nreturn json;\n}\n";
	test += "\nstd::string " + app + "from"+classstruc.getTreatedClassName(true)+"VecVPToJSON(std::vector<"+classstruc.getFullyQualifiedClassName()+">* _vecobj)\n{\nstd::string json = \"[\";\nfor(int i=0;i<_vecobj->size();i++){\n" +
			"json += " + app + "from"+classstruc.getTreatedClassName(true)+"ToJSON(_vecobj->at(i));\n" +
			"if(i!=_vecobj->size()-1)json += \",\";\n" +
			"\n}\njson += \"]\";\nreturn json;\n}\n";
	test += "\nstd::string " + app + "from"+classstruc.getTreatedClassName(true)+"LisVPToJSON(std::list<"+classstruc.getFullyQualifiedClassName()+">* _lisobj)\n{\nstd::vector<"+classstruc.getFullyQualifiedClassName()+"> _vecobj;\nstd::copy(_lisobj->begin(), _lisobj->end(), std::back_inserter(_vecobj));" +
			"\nstd::string json = \"[\";\nfor(int i=0;i<(int)_vecobj.size();i++){\n" +
			"json += " + app + "from"+classstruc.getTreatedClassName(true)+"ToJSON(_vecobj.at(i));\n" +
			"if(i!=(int)_vecobj.size()-1)json += \",\";\n" +
			"\n}\njson += \"]\";\nreturn json;\n}\n";
	if(isOpForSet)
	{
		test += "\nstd::string " + app + "from"+classstruc.getTreatedClassName(true)+"SetVPToJSON(std::set<"+classstruc.getFullyQualifiedClassName()+">* _lisobj)\n{\nstd::vector<"+classstruc.getFullyQualifiedClassName()+"> _vecobj;\nstd::copy(_lisobj->begin(), _lisobj->end(), std::back_inserter(_vecobj));" +
				"\nstd::string json = \"[\";\nfor(int i=0;i<(int)_vecobj.size();i++){\n" +
				"json += " + app + "from"+classstruc.getTreatedClassName(true)+"ToJSON(_vecobj.at(i));\n" +
				"if(i!=(int)_vecobj.size()-1)json += \",\";\n" +
				"\n}\njson += \"]\";\nreturn json;\n}\n";
		test += "\nstd::string " + app + "from"+classstruc.getTreatedClassName(true)+"MulSetVPToJSON(std::multiset<"+classstruc.getFullyQualifiedClassName()+">* _lisobj)\n{\nstd::vector<"+classstruc.getFullyQualifiedClassName()+"> _vecobj;\nstd::copy(_lisobj->begin(), _lisobj->end(), std::back_inserter(_vecobj));" +
				"\nstd::string json = \"[\";\nfor(int i=0;i<(int)_vecobj.size();i++){\n" +
				"json += " + app + "from"+classstruc.getTreatedClassName(true)+"ToJSON(_vecobj.at(i));\n" +
				"if(i!=(int)_vecobj.size()-1)json += \",\";\n" +
				"\n}\njson += \"]\";\nreturn json;\n}\n";
	}
	test += "\nstd::string " + app + "from"+classstruc.getTreatedClassName(true)+"DqVPToJSON(std::deque<"+classstruc.getFullyQualifiedClassName()+">* _lisobj)\n{\nstd::vector<"+classstruc.getFullyQualifiedClassName()+"> _vecobj;\nstd::copy(_lisobj->begin(), _lisobj->end(), std::back_inserter(_vecobj));" +
			"\nstd::string json = \"[\";\nfor(int i=0;i<(int)_vecobj.size();i++){\n" +
			"json += " + app + "from"+classstruc.getTreatedClassName(true)+"ToJSON(_vecobj.at(i));\n" +
			"if(i!=(int)_vecobj.size()-1)json += \",\";\n" +
			"\n}\njson += \"]\";\nreturn json;\n}\n";
	test += "\nstd::string " + app + "from"+classstruc.getTreatedClassName(true)+"QVPToJSON(std::queue<"+classstruc.getFullyQualifiedClassName()+">* _lisobj)\n{\nstd::vector<"+classstruc.getFullyQualifiedClassName()+"> _vecobj;\n" +
			"std::queue<"+classstruc.getFullyQualifiedClassName()+"> qq = *_lisobj;\nfor (int var = 0; var < (int)qq.size(); ++var)\n{" +
			"_vecobj.push_back(qq.front());\nqq.pop();\n}\n" +
			"\nstd::string json = \"[\";\nfor(int i=0;i<(int)_vecobj.size();i++){\n" +
			"json += " + app + "from"+classstruc.getTreatedClassName(true)+"ToJSON(_vecobj.at(i));\n" +
			"if(i!=(int)_vecobj.size()-1)json += \",\";\n" +
			"\n}\njson += \"]\";\nreturn json;\n}\n";
	return test;
}


std::string AfcUtil::generateReadObjects(const std::string& type, const std::string& name, const bool& priv, const bool& ptr, const std::string& typ, const std::string& app, ClassStructure& classstruc, Reflection& ref)
{
	std::string tes;
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
		std::string tempp = type;//StringUtil::replaceAllCopy(type, " ", "");
		std::string fqcn = ref.getFullyQualifiedClassName(tempp, classstruc.getNamespaces());
		if(tempp.find("vector<")!=std::string::npos || tempp.find("list<")!=std::string::npos || tempp.find("set<")!=std::string::npos
				|| tempp.find("multiset<")!=std::string::npos || tempp.find("queue<")!=std::string::npos || tempp.find("deque<")!=std::string::npos)
		{
			std::string stlcnttyp;
			if(tempp.find("vector")!=std::string::npos)
				stlcnttyp = "vector";
			else if(tempp.find("queue")!=std::string::npos)
				stlcnttyp = "std::queue";
			else if(tempp.find("deque")!=std::string::npos)
				stlcnttyp = "deque";
			else if(tempp.find("list")!=std::string::npos)
				stlcnttyp = "list";
			else if(tempp.find("multiset")!=std::string::npos)
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
			std::string tapp = "";
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
			std::string tapp = app;
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


std::string AfcUtil::generateReadVectorObjects(const std::string& type, const std::string& name, const bool& priv, const bool& ptr, const std::string& typ, const std::string& conttype, const std::string& app, ClassStructure& classstruc, Reflection& ref)
{
	std::string tes;
	std::string act = ((conttype=="set" || conttype=="multiset")?"insert":(conttype=="std::queue"?"push":"push_back"));
	std::string fqcn = ref.getFullyQualifiedClassName(type, classstruc.getNamespaces());
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
		std::string tapp = "";
		std::string tfqcn = fqcn;
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
		std::string tapp = app;
		std::string tfqcn = fqcn;
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

std::string AfcUtil::generateToJSONObjects(const std::string& type, const std::string& name, const bool& priv, const bool& end, std::string &retu, std::string &headers, std::string &objs, const std::string& typ, const bool& ptr, const std::string& app, ClassStructure& classstruc, Reflection& ref)
{
	std::string fres;
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
				if(!priv)fres += "json += \"\\\""+name+"\\\" : \"+CastUtil::lexical_cast<std::string>(_obj"+typ+name+");";
				else fres += "json += \"\\\""+name+"\\\" : \"+CastUtil::lexical_cast<std::string>(_obj"+typ+"get"+camelCased(name)+"());";
			}
			else
			{
				if(!priv)fres += "if(_obj"+typ+name+"!=NULL)json += \"\\\""+name+"\\\" : \"+CastUtil::lexical_cast<std::string>(*_obj"+typ+name+");\nelse json += \"null\";\n";
				else fres += "if(_obj"+typ+"get"+camelCased(name)+"()!=NULL)json += \"\\\""+name+"\\\" : \"+CastUtil::lexical_cast<std::string>(*_obj"+typ+"get"+camelCased(name)+"());\nelse json += \"null\";\n";
			}
		}

		if(!end)
		{
			fres += "\njson += \",\";\n";
		}
	}
	else
	{
		std::string fqcn = ref.getFullyQualifiedClassName(type, classstruc.getNamespaces());
		std::string tempp = type;//StringUtil::replaceAllCopy(type, " ", "");
		if(tempp.find("vector<")!=std::string::npos || tempp.find("list<")!=std::string::npos || tempp.find("set<")!=std::string::npos
				|| tempp.find("multiset<")!=std::string::npos || tempp.find("queue<")!=std::string::npos || tempp.find("deque<")!=std::string::npos)
		{
			std::string stlcnttyp;
			if(tempp.find("vector")!=std::string::npos)
				stlcnttyp = "vector";
			else if(tempp.find("queue")!=std::string::npos)
				stlcnttyp = "std::queue";
			else if(tempp.find("deque")!=std::string::npos)
				stlcnttyp = "deque";
			else if(tempp.find("list")!=std::string::npos)
				stlcnttyp = "list";
			else if(tempp.find("multiset")!=std::string::npos)
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

			std::string fqcn = ref.getFullyQualifiedClassName(tempp, classstruc.getNamespaces());

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
			std::string tapp = "";
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
			std::string tapp = app;
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

std::string AfcUtil::generateToJSONVectorObjects(const std::string& type, const std::string& name, const bool& priv, std::string &retu, std::string &headers, std::string &objs, const std::string& typ, const bool& ptr, const std::string& stlcnttyp, const std::string& app, ClassStructure& classstruc, Reflection& ref)
{
	std::string fres = "json += \"\\\""+name+"\\\" : [\";\n";
	std::string vtyp = ".";
	if(ptr)
		vtyp = "->";
	//Is list/set then use iterator
	//if queue then use pop front etc after copying to another queue
	if(ptr)fres += "if(_obj"+typ+name+"!=NULL)\n";

	if(!priv)
	{
		std::string fqcn = ref.getFullyQualifiedClassName(type, classstruc.getNamespaces());
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
		std::string fqcn = ref.getFullyQualifiedClassName(type, classstruc.getNamespaces());
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
	std::string fqcn = ref.getFullyQualifiedClassName(type, classstruc.getNamespaces());
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
					fres += "json += \"\\\"\";json += tt"+name+".front();json +=\"\\\"\";\ntt"+name+".pop();\n";
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
					if(!priv)fres += "json += CastUtil::lexical_cast<std::string>(_obj"+typ+name+".at(j));";
					else fres += "json += CastUtil::lexical_cast<std::string>(_obj"+typ+"get"+camelCased(name)+"().at(j));";
				}
				else if(stlcnttyp=="list" || stlcnttyp=="set" || stlcnttyp=="multiset" || stlcnttyp=="deque")
				{
					fres += "json += CastUtil::lexical_cast<std::string>(*it"+name+");";
				}
				else
				{
					if(!ptr)
						fres += "json += CastUtil::lexical_cast<std::string>(tt"+name+".front());\ntt"+name+".pop();\n";
					else
						fres += "json += CastUtil::lexical_cast<std::string>(tt"+name+"->front());\ntt"+name+"->pop();\n";
				}
			}
			else
			{
				if(stlcnttyp=="vector")
				{
					if(!priv)fres += "json += CastUtil::lexical_cast<std::string>(_obj"+typ+name+"->at(j));";
					else fres += "json += CastUtil::lexical_cast<std::string>(_obj"+typ+"get"+camelCased(name)+"()->at(j));";
				}
				else if(stlcnttyp=="list" || stlcnttyp=="set" || stlcnttyp=="multiset" || stlcnttyp=="deque")
				{
					fres += "json += CastUtil::lexical_cast<std::string>(*it"+name+");";
				}
				else
				{
					if(!ptr)
						fres += "json += CastUtil::lexical_cast<std::string>(tt"+name+".front());\ntt"+name+".pop();\n";
					else
						fres += "json += CastUtil::lexical_cast<std::string>(tt"+name+"->front());\ntt"+name+"->pop();\n";
				}
			}
		}
	}
	else if(fqcn=="Date" || fqcn=="BinaryData")
	{
		std::string tapp = "";
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
				if(!priv)fres += "json += "+tapp+"from"+fqcn+"ToJSON(tt"+name+".front());\ntt"+name+".pop();\n";
				else fres += "json += "+tapp+"from"+fqcn+"ToJSON(tt"+name+".front());\ntt"+name+".pop();\n";
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
				if(!priv)fres += "json += "+app+"from"+fqcn+"ToJSON(tt"+name+"->front());\ntt"+name+"->pop();\n";
				else fres += "json += "+app+"from"+fqcn+"ToJSON(tt"+name+"->front());\ntt"+name+"->pop();\n";
			}
		}
	}
	else if(Reflection::isValidClass(fqcn, app))
	{
		std::string tapp = app;
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

std::string AfcUtil::generateJsInterfaces(const strVec& obj, ClassStructure& classstruc, const std::string& path, std::string &infjs, const std::string& appName, std::map<std::string, std::string>& ajintpthMap, Reflection& ref)
{
	std::string test,intf,intff,inc;
	//headers += "#include \"" + claz + ".h\"\n";
	//writeTofile("/home/sumeet/workspace/inter/AfcInclude.h",inc,false);
	//inc = "\nextern \"C\"{\n";//string executeAFC(std::string fn,strVec _inp){\nstd::string ret;\n";
	//bool fl = false;
	test = ("var " + classstruc.getTreatedClassName(false) + "= {\n");
	for(unsigned int i=0;i<obj.size();i++)
	{
		strVec vemp,emp;
		std::string data = obj.at(i);
		StringUtil::replaceFirst(data,";","");
		std::vector<std::string> delimiters;
		delimiters.push_back(" ");
		delimiters.push_back(",");
		delimiters.push_back(")");
		delimiters.push_back("(");
		StringUtil::split(vemp, data, delimiters);
		for(unsigned int k=0;k<vemp.size();k++)
		{
			if(vemp.at(k)!="")
				emp.push_back(vemp.at(k));
			//logger << vemp.at(i) << "\n" << std::flush;
		}
		//logger << "\n------------------------------------\n" << std::flush;
		if(emp.size()>1)
		{
			size_t te = emp.at(1).find("~");
			if(te==std::string::npos)
			{
				std::string pars,parswt,types,jsonstr;
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
						std::stringstream st,st1,st2;
						//types += ("\"" + emp.at(j) + "\"");
						st << "_";
						st << (j-1);
						if(j!=emp.size()-1)
						{
							st << ",";
							//types += ",";
						}
						std::string h;
						st >> h;
						test += h;
						pars += h;
						parswt += ("std::string " + h);
						//st1 << emp.at(j) << " __" << (j-1);
						//st1 << " = (" << emp.at(j) << ")_" << (j-1) << "->c_str();\n";
						std::string h1;
						//st1 >> h1;
						if(Reflection::isPrimitiveDataType(emp.at(j)))
						{
							types.append(emp.at(j));
							types.append(" _");
							jsonstr += "_"+CastUtil::lexical_cast<std::string>(j-1);
							types.append(CastUtil::lexical_cast<std::string>(j-1));
							types.append(" = CastUtil::lexical_cast<");
							types.append(emp.at(j));
							types.append(">(_inp.at(");
							types.append(CastUtil::lexical_cast<std::string>(j-2));
							types.append("));\n");
						}
						else
						{
							std::string varname = CastUtil::lexical_cast<std::string>(j-1);
							std::string retType = emp.at(j);
							bool ptr = retType.find("*")!=std::string::npos;
							std::string tempp = StringUtil::replaceAllCopy(retType, " ", "");
							std::string fqcn = ref.getFullyQualifiedClassName(retType, classstruc.getNamespaces());
							if(tempp.find("vector<")!=std::string::npos || tempp.find("list<")!=std::string::npos || tempp.find("set<")!=std::string::npos
									|| tempp.find("multiset<")!=std::string::npos || tempp.find("queue<")!=std::string::npos || tempp.find("deque<")!=std::string::npos)
							{
								std::string stlcnttyp;
								if(tempp.find("vector")!=std::string::npos)
									stlcnttyp = "vector";
								else if(tempp.find("queue")!=std::string::npos)
									stlcnttyp = "std::queue";
								else if(tempp.find("deque")!=std::string::npos)
									stlcnttyp = "deque";
								else if(tempp.find("list")!=std::string::npos)
									stlcnttyp = "list";
								else if(tempp.find("multiset")!=std::string::npos)
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

								std::string fqcn = ref.getFullyQualifiedClassName(tempp, classstruc.getNamespaces());

								types.append(emp.at(j));
								if(ptr)
									types.append("* _");
								else
									types.append(" _");
								jsonstr += "JSON.stringify(_"+CastUtil::lexical_cast<std::string>(j-1)+")";

								types.append(varname);
								std::string adden = "", padden = "";
								if(ptr)
								{
									adden = ", \""+stlcnttyp+"<"+fqcn+">\"";
									padden = "delete _" + varname + ";\n";
									types.append(" = ("+emp.at(j)+"*)JSONSerialize::unSerializeUnknown(_inp.at(");
								}
								else
									types.append(" = JSONSerialize::unserialize<"+stlcnttyp+"<"+fqcn+"> >(_inp.at(");
								types.append(CastUtil::lexical_cast<std::string>(j-2));
								types.append(")"+adden+", \""+appName+"\");\n"+padden);
							}
							else if(fqcn=="Date" || fqcn=="BinaryData")
							{
								std::string tapp = "";
								types.append(retType);
								if(ptr)
									types.append("* _");
								else
									types.append(" _");

								std::string adden = "", padden = "";
								jsonstr += "JSON.stringify(_"+CastUtil::lexical_cast<std::string>(j-1)+")";
								types.append(varname);
								if(ptr)
								{
									padden = "delete _" + varname + ";\n";
									adden = ", \""+fqcn+"\"";
									types.append(" = ("+fqcn+"*)JSONSerialize::unSerializeUnknown(_inp.at(");
								}
								else
									types.append(" = JSONSerialize::unserialize<"+fqcn+">(_inp.at(");
								types.append(CastUtil::lexical_cast<std::string>(j-2));
								types.append(")"+adden+", \""+appName+"\");\n"+padden);
							}
							else if(Reflection::isValidClass(fqcn, appName))
							{
								std::string tapp = appName;
								types.append(retType);
								if(ptr)
									types.append("* _");
								else
									types.append(" _");

								std::string adden = "", padden = "";
								jsonstr += "JSON.stringify(_"+CastUtil::lexical_cast<std::string>(j-1)+")";
								types.append(CastUtil::lexical_cast<std::string>(j-1));
								if(ptr)
								{
									padden = "delete _" + varname + ";\n";
									adden = ", \""+fqcn+"\"";
									types.append(" = ("+fqcn+"*)JSONSerialize::unSerializeUnknown(_inp.at(");
								}
								else
									types.append(" = JSONSerialize::unserialize<"+fqcn+">(_inp.at(");
								types.append(CastUtil::lexical_cast<std::string>(j-2));
								types.append(")"+adden+", \""+appName+"\");\n"+padden);
							}
						}
						if(j!=emp.size()-1)
						{
							jsonstr += ",";
						}
						//types += (emp.at(j).c_str() + " __" + (j-1));
						//logger << vemp.at(i) << "\n" << std::flush;
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
	//logger << test << std::flush;
}

std::string AfcUtil::updateAjaxInterface(const strVec& emp, ClassStructure& classstruc, const std::string& pars, const std::string& parswt, const std::string& types, const std::string& appName, Reflection& ref)
{
	std::string test;
	std::string retType = emp.at(0);
	std::string funcName = emp.at(1);
	bool ptr = retType.find("*")!=std::string::npos;
	StringUtil::replaceAll(retType, "*", "");
	StringUtil::replaceAll(retType, "&", "");
	//test = "#include \"" + claz + ".h\"\n#include "CastUtil.h"\n\n";
	test += ("std::string "+appName+"invokeAjaxMethodFor" + classstruc.getTreatedClassName(true) + funcName + "(strVec _inp");
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
		test += "return CastUtil::lexical_cast<std::string>(_obj."+funcName+"("+pars+"));\n}\n";
	}
	else
	{
		std::string tempp = StringUtil::replaceAllCopy(retType, " ", "");
		std::string fqcn = ref.getFullyQualifiedClassName(tempp, classstruc.getNamespaces());
		if(tempp.find("vector<")!=std::string::npos || tempp.find("list<")!=std::string::npos || tempp.find("set<")!=std::string::npos
				|| tempp.find("multiset<")!=std::string::npos || tempp.find("queue<")!=std::string::npos || tempp.find("deque<")!=std::string::npos)
		{
			std::string stlcnttyp;
			if(tempp.find("vector")!=std::string::npos)
				stlcnttyp = "vector";
			else if(tempp.find("queue")!=std::string::npos)
				stlcnttyp = "std::queue";
			else if(tempp.find("deque")!=std::string::npos)
				stlcnttyp = "deque";
			else if(tempp.find("list")!=std::string::npos)
				stlcnttyp = "list";
			else if(tempp.find("multiset")!=std::string::npos)
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

			std::string fqcn = ref.getFullyQualifiedClassName(tempp, classstruc.getNamespaces());

			if(ptr)
			{
				test += tempp + "* __val__ = _obj."+funcName+"("+pars+");\n";
				test += "if(__val__!=NULL) {\n";
				test += "return JSONSerialize::serializeUnknown(__val__, \""+stlcnttyp+"<"+fqcn+">\", \""+appName+"\");\ndelete __val__;\n}\n";
				test += "else return \"\";\n";
			}
			else
			{
				test += tempp + " __val__ = _obj."+funcName+"("+pars+");\n";
				test += "return JSONSerialize::serializeUnknown(&__val__, \""+stlcnttyp+"<"+fqcn+">\", \""+appName+"\");\n";
			}
			test += "\n}\n";
		}
		else if(fqcn=="Date" || fqcn=="BinaryData")
		{
			if(ptr)
			{
				test += fqcn + "* __val__ = _obj."+funcName+"("+pars+");\n";
				test += "if(__val__!=NULL) {\n";
				test += "return JSONSerialize::serializeUnknown(__val__, \""+fqcn+"\", \""+appName+"\");\ndelete __val__;\n}\n";
				test += "else return \"\";\n";
			}
			else
			{
				test += tempp + " __val__ = _obj."+funcName+"("+pars+");\n";
				test += "return JSONSerialize::serializeUnknown(&__val__, \""+fqcn+"\", \""+appName+"\");\n";
			}
			test += "\n}\n";
		}
		else if(Reflection::isValidClass(fqcn, appName))
		{
			if(ptr)
			{
				test += fqcn + "* __val__ = _obj."+funcName+"("+pars+");\n";
				test += "if(__val__!=NULL) {\n";
				test += "return JSONSerialize::serializeUnknown(__val__, \""+fqcn+"\", \""+appName+"\");\ndelete __val__;\n}\n";
				test += "else return \"\";\n";
			}
			else
			{
				test += tempp + " __val__ = _obj."+funcName+"("+pars+");\n";
				test += "return JSONSerialize::serializeUnknown(&__val__, \""+fqcn+"\", \""+appName+"\");\n";
			}
			test += "\n}\n";
		}
		else
		{
			test += "return \"\";\n}\n";
		}
	}
	return test;
}

void AfcUtil::writeTofile(const std::string& fileName, const std::string& data, const bool& trunc)
{
	std::ofstream myfile1;
	if(trunc)
		myfile1.open(fileName.c_str(),std::ios::trunc | std::ios::binary);
	else
		myfile1.open(fileName.c_str(),std::ios::app | std::ios::binary);
	if (myfile1.is_open())
	{
		myfile1.write(data.c_str(),data.length());
		myfile1.close();
	}
}


std::string AfcUtil::camelCased(const std::string& s)
{
	std::string str(s);
	str[0] = toupper(str[0]);
	return str;
}

std::string AfcUtil::reverseCamelCased(const std::string& s)
{
	std::string str(s);
	str[0] = tolower(str[0]);
	return str;
}
