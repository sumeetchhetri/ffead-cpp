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
 * CppInterpreter.cpp
 *
 *  Created on: Aug 22, 2009
 *      Author: sumeet
 */

#include "CppInterpreter.h"

CppInterpreter::CppInterpreter() {
	// TODO Auto-generated constructor stub

}

CppInterpreter::~CppInterpreter() {
	// TODO Auto-generated destructor stub
}

mapVars CppInterpreter::getLocalVariables() const
{
	return localVariables;
}

void CppInterpreter::setLocalVariables(mapVars localVariables)
{
	this->localVariables = localVariables;
}

mapVars CppInterpreter::getBoundVariables() const
{
	return boundVariables;
}

void CppInterpreter::setBoundVariables(mapVars boundVariables)
{
	this->boundVariables = boundVariables;
}

bool CppInterpreter::isInBuiltType(string type)
{
    return (type=="int" || type=="float" || type=="double" || type=="string" || type=="std::string");
}
/*
vector<string> splitPattern(string str)
{
	string comm;
	vector<string> commands;//stack of command structure
	for(unsigned int l=0;l<str.length();l++)
	{
		if(str.substr(l,1)=="+")
		{
			if(comm=="")
				commands.push_back("(");
			else
			{
				comm = comm.substr(comm.find_first_not_of(" "));
				commands.push_back(comm);
				commands.push_back("(");
			}
			comm = "";
		}
		else if(str.substr(l,1)==")")
		{
			if(comm=="")
				commands.push_back(")");
			else
			{
				comm = comm.substr(comm.find_first_not_of(" "));
				commands.push_back(comm);
				commands.push_back(")");
			}
			comm = "";
		}
		else if(str.substr(l,1)=="{")
		{
			if(comm=="")
				commands.push_back("{");
			else
			{
				comm = comm.substr(comm.find_first_not_of(" "));
				commands.push_back(comm);
				commands.push_back("{");
			}
			comm = "";
		}
		else if(str.substr(l,1)=="}")
		{
			if(comm=="")
				commands.push_back("}");
			else
			{
				comm = comm.substr(comm.find_first_not_of(" "));
				commands.push_back(comm);
				commands.push_back("}");
			}
			comm = "";
		}
		else if(str.substr(l,1)==";")
		{
			if(comm=="")
				commands.push_back(";");
			else
			{
				comm = comm.substr(comm.find_first_not_of(" "));
				commands.push_back(comm);
				commands.push_back(";");
			}
			comm = "";
		}
		else
			comm += str.substr(l,1);
	}
	if(comm!="")
	{
		comm = comm.substr(comm.find_first_not_of(" "));
		commands.push_back(comm);
	}
	return commands;
}
*/
void CppInterpreter::storeInbuilt(string type,string name)
{
    if(type=="int")
    {
        int *_temp = new int;
        Object o;
        o << *_temp;
        localVariables[name] = o;
    }
    else if(type=="float")
    {
        float *_temp = new float;
        Object o;
        o << *_temp;
        localVariables[name] = o;
    }
    else if(type=="double")
    {
        double *_temp = new double;
        Object o;
        o << *_temp;
        localVariables[name] = o;
    }
    else if(type=="string")
    {
        string *_temp = new string;
        Object o;
        o << *_temp;
        localVariables[name] = o;
    }
}

void CppInterpreter::storeCustom(string type,string name)
{
    Reflector ref;
    ClassInfo clas = ref.getClassInfo(type);
    args argus;
	Constructor ctor = clas.getConstructor(argus);
	void *_temp = ref.newInstanceGVP(ctor);
    Object o;
    o << _temp;
    o.setTypeName(type);
    localVariables[name] = o;
}

bool CppInterpreter::containsChar(string varname)
{
	string allchars = "abcdefghijklmnopqrstuvwxyz_$";
	for(unsigned int l=0;l<varname.length();l++)
	{
		for(unsigned int l1=0;l1<allchars.length();l1++)
		{
			if(allchars.substr(l1,1)==varname.substr(l,1))
			{
				return true;
			}
		}
	}
	return false;
}


bool CppInterpreter::isCommand(string test)
{
	return (test=="if" || test=="else" || test=="elseif" || test=="while" || test=="for");
}

bool CppInterpreter::retState(string type,Object source,Object target)
{
	if(isInBuiltType(source.getTypeName()))
	{
		if(source.getTypeName()=="int")
		{
			if(type=="==")
				return (source.getValue<int>()==target.getValue<int>());
			else if(type=="<=")
				return (source.getValue<int>()<=target.getValue<int>());
			else  if(type==">=")
				return (source.getValue<int>()>=target.getValue<int>());
			else  if(type=="!=")
				return (source.getValue<int>()!=target.getValue<int>());
			else  if(type==">")
				return (source.getValue<int>()>target.getValue<int>());
			else  if(type=="<")
				return (source.getValue<int>()<target.getValue<int>());
		}
		else if(source.getTypeName()=="float")
		{
			if(type=="==")
				return (source.getValue<float>()==target.getValue<float>());
			else if(type=="<=")
				return (source.getValue<float>()<=target.getValue<float>());
			else  if(type==">=")
				return (source.getValue<float>()>=target.getValue<float>());
			else  if(type=="!=")
				return (source.getValue<float>()!=target.getValue<float>());
			else  if(type==">")
				return (source.getValue<float>()>target.getValue<float>());
			else  if(type=="<")
				return (source.getValue<float>()<target.getValue<float>());
		}
		else if(source.getTypeName()=="double")
		{
			if(type=="==")
				return (source.getValue<double>()==target.getValue<double>());
			else if(type=="<=")
				return (source.getValue<double>()<=target.getValue<double>());
			else  if(type==">=")
				return (source.getValue<double>()>=target.getValue<double>());
			else  if(type=="!=")
				return (source.getValue<double>()!=target.getValue<double>());
			else  if(type==">")
				return (source.getValue<double>()>target.getValue<double>());
			else  if(type=="<")
				return (source.getValue<double>()<target.getValue<double>());
		}
		else if(source.getTypeName()=="string")
		{
			if(type=="==")
				return (source.getValue<string>()==target.getValue<string>());
			else if(type=="<=")
				return (source.getValue<string>()<=target.getValue<string>());
			else  if(type==">=")
				return (source.getValue<string>()>=target.getValue<string>());
			else  if(type=="!=")
				return (source.getValue<string>()!=target.getValue<string>());
			else  if(type==">")
				return (source.getValue<string>()>target.getValue<string>());
			else  if(type=="<")
				return (source.getValue<string>()<target.getValue<string>());
		}
	}
	return false;
}

bool CppInterpreter::retState(string type,Object source,string target)
{
	if(isInBuiltType(source.getTypeName()))
	{
		if(source.getTypeName()=="int")
		{
			if(type=="==")
				return (source.getValue<int>()==boost::lexical_cast<int>(target));
			else if(type=="<=")
				return (source.getValue<int>()<=boost::lexical_cast<int>(target));
			else  if(type==">=")
				return (source.getValue<int>()>=boost::lexical_cast<int>(target));
			else  if(type=="!=")
				return (source.getValue<int>()!=boost::lexical_cast<int>(target));
			else  if(type==">")
				return (source.getValue<int>()>boost::lexical_cast<int>(target));
			else  if(type=="<")
				return (source.getValue<int>()<boost::lexical_cast<int>(target));
		}
		else if(source.getTypeName()=="float")
		{
			if(type=="==")
				return (source.getValue<float>()==boost::lexical_cast<float>(target));
			else if(type=="<=")
				return (source.getValue<float>()<=boost::lexical_cast<float>(target));
			else  if(type==">=")
				return (source.getValue<float>()>=boost::lexical_cast<float>(target));
			else  if(type=="!=")
				return (source.getValue<float>()!=boost::lexical_cast<float>(target));
			else  if(type==">")
				return (source.getValue<float>()>boost::lexical_cast<float>(target));
			else  if(type=="<")
				return (source.getValue<float>()<boost::lexical_cast<float>(target));
		}
		else if(source.getTypeName()=="double")
		{
			if(type=="==")
				return (source.getValue<double>()==boost::lexical_cast<double>(target));
			else if(type=="<=")
				return (source.getValue<double>()<=boost::lexical_cast<double>(target));
			else  if(type==">=")
				return (source.getValue<double>()>=boost::lexical_cast<double>(target));
			else  if(type=="!=")
				return (source.getValue<double>()!=boost::lexical_cast<double>(target));
			else  if(type==">")
				return (source.getValue<double>()>boost::lexical_cast<double>(target));
			else  if(type=="<")
				return (source.getValue<double>()<boost::lexical_cast<double>(target));
		}
		else if(source.getTypeName()=="string")
		{
			if(type=="==")
				return (source.getValue<string>()==target);
			else if(type=="<=")
				return (source.getValue<string>()<=target);
			else  if(type==">=")
				return (source.getValue<string>()>=target);
			else  if(type=="!=")
				return (source.getValue<string>()!=target);
			else  if(type==">")
				return (source.getValue<string>()>target);
			else  if(type=="<")
				return (source.getValue<string>()<target);
		}
	}
	return false;
}

bool CppInterpreter::evaluateCondition(string condition)
{
	bool state = false;
	string token;
	if(condition.find("==")!=string::npos)
	{
		token = "==";
	}
	else if(condition.find("!=")!=string::npos)
	{
		token = "!=";
	}
	else if(condition.find("<=")!=string::npos || condition.find("=<")!=string::npos)
	{
		token = "<=";
	}
	else if(condition.find(">=")!=string::npos || condition.find("=>")!=string::npos)
	{
		token = ">=";
	}
	else if(condition.find(">")!=string::npos)
	{
		token = ">";
	}
	else if(condition.find("<")!=string::npos)
	{
		token = "<";
	}
	if(token!="")
	{
		vector<string> bs;
		boost::iter_split(bs, condition, boost::first_finder(token));
		if(bs.size()==2 && bs.at(0)!="" && bs.at(1)!="")
		{
			Object source,target;
			if(localVariables.find(bs.at(0))!=localVariables.end())
			{
				source = localVariables[bs.at(0)];
			}
			else if(boundVariables.find(bs.at(0))!=boundVariables.end())
			{
				source = boundVariables[bs.at(0)];
			}
			if(containsChar(bs.at(1)))
			{
				if(localVariables.find(bs.at(1))!=localVariables.end())
				{
					target = localVariables[bs.at(1)];
				}
				else if(boundVariables.find(bs.at(1))!=boundVariables.end())
				{
					target = boundVariables[bs.at(1)];
				}
				state = retState(token,source,target);
			}
			else
			{
				state = retState(token,source,bs.at(1));
			}
		}
	}
	return state;
}

void CppInterpreter::skipStatement(vector<string>::iterator &itr)
{
	string nextToken = *(itr);
	while(nextToken!=";")
	{
		nextToken = *(++itr);
	}
	//++itr;
}

void CppInterpreter::skipCommand(vector<string>::iterator &itr)
{
	int crcnt = 0;
	int cucnt = 0;
	string nextToken = *(itr);
	while(1)
	{
		if(nextToken=="(")
		{
			crcnt++;
		}
		else if(nextToken==")")
		{
			crcnt--;
			if(crcnt==0)
				break;
		}
		nextToken = *(itr++);
	}
	if(nextToken=="{")
	{
		while(1)
		{
			if(nextToken=="{")
				cucnt++;
			else if(nextToken=="}")
			{
				cucnt--;
				if(cucnt==0)
				{
					//++itr;
					break;
				}
			}
			nextToken = *(++itr);
		}
	}
	else
	{
		if(isCommand(nextToken))
			skipCommand(itr);
		else
			skipStatement(itr);
	}
}

void CppInterpreter::evaluateUpdateCustom(string sep,string type,string name,vector<string> opr,bool local)
{
	Object o;
	if(local)
		o = localVariables[name];
	else
		o = boundVariables[name];
	Object i;
	if(localVariables.find(opr.at(0))!=localVariables.end())
	{
		i = localVariables[opr.at(0)];
	}
	else if(boundVariables.find(opr.at(0))!=boundVariables.end())
	{
		i = boundVariables[opr.at(0)];
	}
	if(i.getTypeName()!="" && o.getTypeName()!="")
	{
		Reflector reflector;
		ClassInfo clas = reflector.getClassInfo(type);
		string name = "invokeReflectionCIAssignMethodFor"+i.getTypeName();
		args argus;
		argus.push_back("void*");
		argus.push_back("void*");
		Method meth = clas.getMethod(name,argus);
		vals valus;
		valus.push_back(o.getVoidPointer());
		reflector.invokeMethod<void*>(i.getVoidPointer(),meth,valus);
	}
}

void CppInterpreter::evaluateUpdateInbuilt(string sep,string type,string name,vector<string> opr,bool local)
{
	if(type=="int")
	{
		Object o;
		if(local)
			o = localVariables[name];
		else
			o = boundVariables[name];
		int *_temp = (int*)o.getVoidPointer();
		vector<string> curr,going,temp;
		int scnt = 0,ecnt = 0,bcnt = 0;
		bool gost= false;
		if(sep=="++" || sep=="--")
			opr.clear();
		/*for(unsigned int i=0;i<opr.size();i++)
		{
			if(opr.at(i)=="(")
			{
				//ecnt++;
				scnt++;
				gost = true;
				//ecnt = scnt+1;
				if(ecnt)
				{
					going.clear();
					gost = true;
				}
			}
			else if(opr.at(i)==")")
			{
				ecnt++;
				if(scnt==ecnt)
				{
					curr.push_back(handleAssignment<int>(going));
					//curr = going;
					going.clear();
				}
				else
					curr.push_back(handleAssignment<int>(going));
				going.clear();
				gost = false;
			}
			else if(gost)
				going.push_back(opr.at(i));
			else
				curr.push_back(opr.at(i));

		}*/
		vector<string>::iterator itre,endre;
		itre = opr.begin();
		endre = opr.end();
		string res = evalBrackets<int>(itre,endre);
		/*opr.clear();
		for(unsigned int i=0;i<curr.size();i++)
		{
			if(containsChar(curr.at(i)))
			{
				Object o = localVariables[curr.at(i)];
				opr.push_back(boost::lexical_cast<string>(o.getValue<int>()));
			}
			else
				opr.push_back(curr.at(i));
		}
		if(sep=="=")
			*_temp = boost::lexical_cast<int>(handleAssignment<int>(opr));
		else if(sep=="+=")
			*_temp = *_temp + boost::lexical_cast<int>(handleAssignment<int>(opr));
		else if(sep=="-=")
			*_temp = *_temp - boost::lexical_cast<int>(handleAssignment<int>(opr));
		else if(sep=="++")
			*_temp = (*_temp+1);
		else if(sep=="--")
			*_temp = (*_temp-1);*/
		if(sep=="=")
			*_temp = boost::lexical_cast<int>(res);
		else if(sep=="+=")
			*_temp = *_temp + boost::lexical_cast<int>(res);
		else if(sep=="-=")
			*_temp = *_temp - boost::lexical_cast<int>(res);
		else if(sep=="++")
			*_temp = (*_temp+1);
		else if(sep=="--")
			*_temp = (*_temp-1);
	}
	/*else if(type=="float")
	{
		float *_temp = new float;
		Object o;
		o << *_temp;
		localVariables[name] = o;
	}
	else if(type=="double")
	{
		double *_temp = new double;
		Object o;
		o << *_temp;
		localVariables[name] = o;
	}
	else if(type=="string" || type=="std::string")
	{
		string *_temp = new string;
		Object o;
		o << *_temp;
		localVariables[name] = o;
	}*/
}

void CppInterpreter::executeStatement(string sep,vector<string> lhs,vector<string> rhs)
{
	if(sep!="")
	{
		if(lhs.size()>0)//probable declaration
		{
			vector<string> bs;
			boost::iter_split(bs, lhs.at(0), boost::first_finder(" "));
			if(bs.size()==1)
			{
				Object source;
				bool local = false;
				if(localVariables.find(bs.at(0))!=localVariables.end())
				{
					source = localVariables[bs.at(0)];
					local = true;
				}
				else if(boundVariables.find(bs.at(0))!=boundVariables.end())
				{
					source = boundVariables[bs.at(0)];
				}
				if(isInBuiltType(source.getTypeName()))
				{
					evaluateUpdateInbuilt(sep,source.getTypeName(),bs.at(0),rhs,local);
				}
				else
				{
					evaluateUpdateCustom(sep,bs.at(0),bs.at(1),rhs,local);
				}
			}
			else
			{
				if(isInBuiltType(bs.at(0)))
				{
					storeInbuilt(bs.at(0),bs.at(1));
					evaluateUpdateInbuilt(sep,bs.at(0),bs.at(1),rhs,true);
				}
				else
				{
					storeCustom(bs.at(0),bs.at(1));
					evaluateUpdateCustom(sep,bs.at(0),bs.at(1),rhs,true);
				}
			}
		}
	}
	else
	{
		vector<string> bs;
		boost::iter_split(bs, lhs.at(0), boost::first_finder(" "));
		if(isInBuiltType(bs.at(0)))
		{
			storeInbuilt(bs.at(0),bs.at(1));
			//evaluateUpdateInbuilt(sep,bs.at(0),bs.at(1),rhs,true);
		}
		else
		{
			storeCustom(bs.at(0),bs.at(1));
			//evaluateUpdateCustom(sep,bs.at(0),bs.at(1),rhs,true);
		}
	}
}

Obj CppInterpreter::handleObjectMethodInvocation(string objn,string methn,vector<string>::iterator &itr)
{
	string token = *(++itr);
	int crcnt = 1;
	string statement;
	vals valus;
	args argus;
	Obj obj;
	string objname,methName;
	bool objmem = false;
	while(1)
	{
		if(token=="(")
		{
			crcnt++;
			if(objmem)
			{
				methName = statement;
				obj = handleObjectMethodInvocation(objname,methName,itr);
				statement = "";
			}
		}
		else if(token==")")
		{
			crcnt--;
			if(crcnt==0)
			{
				if(obj.getType()!="")
				{
					valus.push_back(obj.getPointer());
					argus.push_back(obj.getType());
					obj.setPointer(NULL);
					obj.setType("");
				}
				else if(statement!="")
				{
					Object src;
					if(localVariables.find(statement)!=localVariables.end())
					{
						src = localVariables[statement];
					}
					else if(boundVariables.find(statement)!=boundVariables.end())
					{
						src = boundVariables[statement];
					}
					if(src.getTypeName()!="")
					{
						argus.push_back(src.getTypeName());
						valus.push_back(src.getVoidPointer());
					}
				}
				break;
			}
		}
		else if((token=="." || token=="->"))
		{
			objmem = true;
			objname = statement;
		}
		else if(token==",")
		{
			if(obj.getType()!="")
			{
				valus.push_back(obj.getPointer());
				argus.push_back(obj.getType());
				obj.setPointer(NULL);
				obj.setType("");
			}
			else if(statement!="")
			{
				Object src;
				if(localVariables.find(statement)!=localVariables.end())
				{
					src = localVariables[statement];
				}
				else if(boundVariables.find(statement)!=boundVariables.end())
				{
					src = boundVariables[statement];
				}
				if(src.getTypeName()!="")
				{
					argus.push_back(src.getTypeName());
					valus.push_back(obj.getPointer());
				}
			}
		}
		statement += token;
		token = *(++itr);
	}

	Object source;
	if(localVariables.find(objn)!=localVariables.end())
	{
		source = localVariables[objn];
	}
	else if(boundVariables.find(objn)!=boundVariables.end())
	{
		source = boundVariables[objn];
	}
	if(source.getTypeName()!="")
	{
		Reflector reflector;
		ClassInfo clas = reflector.getClassInfo(source.getTypeName());
		Method meth = clas.getMethod(methn,argus);
		void *po = reflector.invokeMethodGVP(source.getVoidPointer(),meth,valus);
		obj.setPointer(po);
		obj.setType(meth.getReturnType());
	}
	return obj;
}

void CppInterpreter::handleStatement(vector<string>::iterator &itr)
{
	string token = *itr;
	string statement;
	string sep;
	vector<string> lhs,rhs;
	string objname,methName;
	bool rhsa = false,objmem = false;
	int argn = 1;
	bool objectex = false;
	while(token!=";")
	{
		if(token=="=")
		{
			//rhs.push_back(statement);
			statement = "";
			sep = "=";
			//rhs.push_back("=");
		}
		else if(token=="-=")
		{
			//rhs.push_back(statement);
			statement = "";
			sep = "-=";
			//rhs.push_back("=");
		}
		else if(token=="+=")
		{
			//rhs.push_back(statement);
			statement = "";
			sep = "+=";
			//rhs.push_back("=");
		}
		else if(token=="+")
		{
			rhsa = true;
			if(statement!="")rhs.push_back(statement);
			statement = "";
			rhs.push_back("+");
		}
		else if(token=="-")
		{
			rhsa = true;
			if(statement!="")rhs.push_back(statement);
			statement = "";
			rhs.push_back("-");
		}
		else if(token=="*")
		{
			rhsa = true;
			if(statement!="")rhs.push_back(statement);
			statement = "";
			rhs.push_back("*");
		}
		else if(token=="/")
		{
			rhsa = true;
			if(statement!="")rhs.push_back(statement);
			statement = "";
			rhs.push_back("/");
		}
		else if(token=="(")
		{
			if(objmem)
			{
				Obj obj = handleObjectMethodInvocation(objname,methName,itr);
				if(obj.getType()!="" && sep!="")
				{
					Object o;
					o << obj.getPointer();
					o.setTypeName(obj.getType());
					string argname = "_argno"+boost::lexical_cast<string>(argn++);
					localVariables[argname] = o;
					//rhs.push_back(argname);
					token = *(itr);
					statement = argname;
					rhsa = true;
					objmem = false;
				}
				else
					objectex = true;
			}
			else
			{
				rhsa = true;
				if(statement!="")rhs.push_back(statement);
				statement = "";
				rhs.push_back("(");
			}
		}
		else if(token==")")
		{
			rhsa = true;
			if(statement!="")rhs.push_back(statement);
			statement = "";
			rhs.push_back(")");
		}
		else if(token=="++")
		{
			//rhs.push_back(statement);
			statement = "";
			sep = "++";
			//rhs.push_back("=");
		}
		else if(token=="--")
		{
			//rhs.push_back(statement);
			statement = "";
			sep = "--";
			//rhs.push_back("=");
		}
		else if(token=="--")
		{
			//rhs.push_back(statement);
			statement = "";
			sep = "--";
			//rhs.push_back("=");
		}
		else if(token.find(".")!=string::npos)
		{
			objmem = true;
			vector<string> bs;
			boost::iter_split(bs,token, boost::first_finder("."));
			objname = bs.at(0);
			methName = bs.at(1);
		}
		if(sep=="")
		{
			lhs.push_back(token);
		}
		else if(sep!=token && !rhsa)
		{
			statement += token;
		}
		rhsa = false;
		token = *(++itr);
	}
	if(sep!=token && statement!="")
		rhs.push_back(statement);
	if(!objectex)
		executeStatement(sep,lhs,rhs);
	return;
}

bool CppInterpreter::evalCond(vector<string> str)
{
	vector<string>::iterator iter;
	iter = str.begin();
	bool state = true;
	string st = *(iter);
	string prev,cond;
	int crcnt = 0;
	while(1)
	{
		if(st=="(")
		{
			crcnt++;
			cond = "";
		}
		else if(st==")")
		{
			crcnt--;
			if(prev=="&")
			{
				state = state && evaluateCondition(cond);
				cout << "prev=" << prev << " cond=" << cond << " state=" << state << "\n" << flush;
			}
			else if(prev=="|")
			{
				state = state || evaluateCondition(cond);
				cout << "prev=" << prev << " cond=" << cond << " state=" << state << "\n" << flush;
			}
			else if(cond!="")
			{
				state = evaluateCondition(cond);
				cout << "prev=" << prev << " cond=" << cond << " state=" << state << "\n" << flush;
			}
			prev = "";
			cond = "";
			if(crcnt==0)
				break;
		}
		else if(st=="||")
		{
			if(prev=="&")
			{
				state = state && evaluateCondition(cond);
				cout << "prev=" << prev << " cond=" << cond << " state=" << state << "\n" << flush;
			}
			else if(prev=="|")
			{
				state = state || evaluateCondition(cond);
				cout << "prev=" << prev << " cond=" << cond << " state=" << state << "\n" << flush;
			}
			else if(cond!="")
			{
				state = evaluateCondition(cond);
				cout << "prev=" << prev << " cond=" << cond << " state=" << state << "\n" << flush;
			}
			prev = "|";
			cond = "";
		}
		else if(st=="&&")
		{
			if(prev=="&")
			{
				state = state && evaluateCondition(cond);
				cout << "prev=" << prev << " cond=" << cond << " state=" << state << "\n" << flush;
			}
			else if(prev=="|")
			{
				state = state || evaluateCondition(cond);
				cout << "prev=" << prev << " cond=" << cond << " state=" << state << "\n" << flush;
			}
			else if(cond!="")
			{
				state = evaluateCondition(cond);
				cout << "prev=" << prev << " cond=" << cond << " state=" << state << "\n" << flush;
			}
			prev = "&";
			cond = "";
		}
		else
			cond += st;
		++iter;
		if(iter<str.end())
			st = *(iter);
		else
			break;
	}
	if(cond!="" && cond!=" ")
	{
		cout << "state=" << state << "\n" << flush;
		state = evaluateCondition(cond);
	}
	return state;
}

void CppInterpreter::handleELSE(vector<string>::iterator &iter)
{
	string nextToken = *(++iter);
	int cucnt = 0;
	if(nextToken=="{")
	{
		while(1)
		{
			if(nextToken=="{")
				cucnt++;
			else if(nextToken=="}")
			{
				cucnt--;
				if(cucnt==0)
					break;
			}
			else if(isCommand(nextToken))
				hanldeCommand(iter);
			else
				handleStatement(iter);
			nextToken = *(++iter);
		}
	}
	else
	{
		bool gone = false;
		if(isCommand(nextToken))
		{
			hanldeCommand(iter);
			gone = true;
		}
		else
		{
			handleStatement(iter);
			gone = true;
		}
		if(gone)
			nextToken = *(++iter);
	}
}


void CppInterpreter::hanldeIF(vector<string>::iterator &iter)
{
	bool state = true;
	string st = *(++iter);
	string prev,cond;
	int crcnt = 0;
	while(1)
	{
		if(st=="(")
		{
			crcnt++;
			cond = "";
		}
		else if(st==")")
		{
			crcnt--;
			if(prev=="&")
			{
				state = state && evaluateCondition(cond);
				cout << "prev=" << prev << " cond=" << cond << " state=" << state << "\n" << flush;
			}
			else if(prev=="|")
			{
				state = state || evaluateCondition(cond);
				cout << "prev=" << prev << " cond=" << cond << " state=" << state << "\n" << flush;
			}
			else if(cond!="")
			{
				state = evaluateCondition(cond);
				cout << "prev=" << prev << " cond=" << cond << " state=" << state << "\n" << flush;
			}
			prev = "";
			cond = "";
			if(crcnt==0)
				break;
		}
		else if(st=="||")
		{
			if(prev=="&")
			{
				state = state && evaluateCondition(cond);
				cout << "prev=" << prev << " cond=" << cond << " state=" << state << "\n" << flush;
			}
			else if(prev=="|")
			{
				state = state || evaluateCondition(cond);
				cout << "prev=" << prev << " cond=" << cond << " state=" << state << "\n" << flush;
			}
			else if(cond!="")
			{
				state = evaluateCondition(cond);
				cout << "prev=" << prev << " cond=" << cond << " state=" << state << "\n" << flush;
			}
			prev = "|";
			cond = "";
		}
		else if(st=="&&")
		{
			if(prev=="&")
			{
				state = state && evaluateCondition(cond);
				cout << "prev=" << prev << " cond=" << cond << " state=" << state << "\n" << flush;
			}
			else if(prev=="|")
			{
				state = state || evaluateCondition(cond);
				cout << "prev=" << prev << " cond=" << cond << " state=" << state << "\n" << flush;
			}
			else if(cond!="")
			{
				state = evaluateCondition(cond);
				cout << "prev=" << prev << " cond=" << cond << " state=" << state << "\n" << flush;
			}
			prev = "&";
			cond = "";
		}
		else
			cond += st;
		st = *(++iter);
	}
	string nextToken = *(++iter);
	if(state)
	{
		int cucnt = 0;
		if(nextToken=="{")
		{
			while(1)
			{
				if(nextToken=="{")
					cucnt++;
				else if(nextToken=="}")
				{
					cucnt--;
					if(cucnt==0)
						break;
				}
				else if(isCommand(nextToken))
					hanldeCommand(iter);
				else
					handleStatement(iter);
				nextToken = *(++iter);
			}
		}
		else
		{
			bool gone = false;
			if(isCommand(nextToken))
			{
				hanldeCommand(iter);
				gone = true;
			}
			else
			{
				handleStatement(iter);
				gone = true;
			}
			if(gone)
				nextToken = *(++iter);
		}
	}
	else
	{
		if(nextToken=="{")
		{
			int cucnt = 0;
			while(1)
			{
				if(nextToken=="{")
					cucnt++;
				else if(nextToken=="}")
				{
					cucnt--;
					if(cucnt==0)
					{
						//nextToken = *(++iter);
						break;
					}
				}
				nextToken = *(++iter);
			}
		}
		else
		{
			bool gone = false;
			if(isCommand(nextToken))
			{
				skipCommand(iter);
				gone = true;
			}
			else
			{
				skipStatement(iter);
				gone = true;
			}
			if(gone)
				nextToken = *(++iter);
		}
	}
	if(nextToken=="else")
	{
		nextToken = *(++iter);
		if(nextToken=="if")
		{
			hanldeIF(iter);
		}
		else if(!state)
		{
			--iter;
			handleELSE(iter);
		}
	}
}

void CppInterpreter::hanldeWHILE(vector<string>::iterator &itr)
{
	string st = *(++itr);
	string prev,cond;
	int crcnt = 0;
	vector<string> whlstr;
	while(1)
	{
		whlstr.push_back(st);
		if(st=="(")
		{
			crcnt++;
		}
		else if(st==")")
		{
			crcnt--;
			if(crcnt==0)
				break;
		}
		st = *(++itr);
	}
	string nextToken = *(++itr);
	vector<string> stmt;
	bool blk = false;
	bool whd = false;
	if(nextToken=="{")
	{
		int cucnt = 0;
		while(1)
		{
			stmt.push_back(nextToken);
			if(nextToken=="{")
				cucnt++;
			else if(nextToken=="}")
			{
				cucnt--;
				if(cucnt==0)
				{
					break;
				}
			}
			nextToken = *(++itr);
		}
		blk = true;
	}
	else
	{
		while(nextToken!=";")
		{
			stmt.push_back(nextToken);
			nextToken = *(++itr);
		}
		stmt.push_back(nextToken);
	}
	while(evalCond(whlstr))
	{
		whd = true;
		vector<string>::iterator tok = stmt.begin();
		string stok = *(tok);
		int cucnt = 0;
		while(tok<stmt.end())
		{
			if(blk)
			{
				while(1)
				{
					if(stok=="{")
						cucnt++;
					else if(stok=="}")
					{
						cucnt--;
						if(cucnt==0)
							break;
					}
					else if(isCommand(stok))
						hanldeCommand(tok);
					else
						handleStatement(tok);
					stok = *(++tok);
				}
				++tok;
			}
			else
			{
				bool gone = false;
				if(isCommand(stok))
				{
					hanldeCommand(tok);
					gone = true;
				}
				else
				{
					handleStatement(tok);
					gone = true;
				}
				if(gone)
					stok = *(++tok);
			}
		}
	}
}

void CppInterpreter::hanldeFOR(vector<string>::iterator &itr)
{
	string st = *(++itr);
	string prev,cond;
	int crcnt = 0;
	bool condit=false,inc=false;
	vector<string> whlstr,incr;
	while(1)
	{
		if(st!=";" && condit)
			whlstr.push_back(st);
		if(st=="(")
		{
			crcnt++;
			++itr;
			handleStatement(itr);
			condit = true;
		}
		else if(st==";")
		{
			condit = false;
			inc = true;
		}
		else if(st==")")
		{
			crcnt--;
			if(crcnt==0)
			{
				incr.push_back(";");
				break;
			}
		}
		if(st!=";" && inc)
			incr.push_back(st);
		st = *(++itr);
	}
	string nextToken = *(++itr);
	vector<string> stmt;
	bool blk = false;
	bool whd = false;
	if(nextToken=="{")
	{
		int cucnt = 0;
		while(1)
		{
			stmt.push_back(nextToken);
			if(nextToken=="{")
				cucnt++;
			else if(nextToken=="}")
			{
				cucnt--;
				if(cucnt==0)
				{
					stmt.erase(stmt.begin()+stmt.size()-1);
					if(incr.at(0)=="++" || incr.at(0)=="--")
					{
						for(unsigned int l=0;l<stmt.size();l++)
						{
							incr.push_back(stmt.at(l));
						}
						stmt = incr;
					}
					else
					{
						for(unsigned int l=0;l<incr.size();l++)
						{
							stmt.push_back(incr.at(l));
						}
					}
					stmt.push_back(nextToken);
					break;
				}
			}
			nextToken = *(++itr);
		}
		 blk = true;
	}
	else
	{
		while(nextToken!=";")
		{
			stmt.push_back(nextToken);
			nextToken = *(++itr);
		}
		stmt.push_back(nextToken);
	}
	while(evalCond(whlstr))
	{
		whd = true;
		vector<string>::iterator tok = stmt.begin();
		string stok = *(tok);
		int cucnt = 0;
		while(tok<stmt.end())
		{
			if(blk)
			{
				while(1)
				{
					if(stok=="{")
						cucnt++;
					else if(stok=="}")
					{
						cucnt--;
						if(cucnt==0)
							break;
					}
					else if(isCommand(stok))
						hanldeCommand(tok);
					else
						handleStatement(tok);
					stok = *(++tok);
				}
				++tok;
			}
			else
			{
				bool gone = false;
				if(isCommand(stok))
				{
					hanldeCommand(tok);
					gone = true;
				}
				else
				{
					handleStatement(tok);
					gone = true;
				}
				if(gone)
					stok = *(++tok);
			}
		}
	}
}

void CppInterpreter::hanldeCommand(vector<string>::iterator &itr)
{
	if(*itr=="if" || *itr=="else if")
	{
		hanldeIF(itr);
	}
	else if(*itr=="for")
	{
		hanldeFOR(itr);
	}
	else if(*itr=="while")
	{
		hanldeWHILE(itr);
	}
}


void CppInterpreter::eval(string str)
{
	cout << str << flush;
	string temp = str;
	int st = 1;
	while(temp.find("\"")!=string::npos)
	{
		size_t s = temp.find_first_of("\"")+1;
		string temp1 = temp.substr(s);
		size_t e = temp1.find_first_of("\"");
		string litval;
		litval = temp.substr(s,e);
		temp = temp.substr(e+s+1);
		string varn =  ("_"+ boost::lexical_cast<string>(st++));
		literals[varn] = litval;
		string ini = str.substr(0,s-1);
		str = (ini + varn + temp);
		temp = str;
		cout << "\n" << flush;
		cout << litval << flush;
		cout << "\n" << flush;
		cout << s << e << flush;
		cout << "\n" << flush;
		cout << temp << flush;
		cout << "\n" << flush;
	}
	cout << str << flush;
	boost::regex pattern("\\s+",boost::regex_constants::icase|boost::regex_constants::perl);
	str = boost::regex_replace (str, pattern, " ");
	cout << "\n" << flush;
	cout << str << flush;
	cout << "\n" << flush;

	string comm;
	vector<string> commands;//stack of command structure
	bool eqop = false;
	string command;
	for(unsigned int l=0;l<str.length();l++)
	{
		if(str.substr(l,1)=="{" || str.substr(l,1)=="}" || str.substr(l,1)==";" || str.substr(l,1)==")"
			|| str.substr(l,1)=="(" || str.substr(l,1)=="+" || str.substr(l,1)=="-" || str.substr(l,1)=="/"
			|| str.substr(l,1)=="*" || str.substr(l,1)=="=" || str.substr(l,1)=="!" || str.substr(l,1)=="|"
			|| str.substr(l,1)=="&" || str.substr(l,1)=="<" || str.substr(l,1)==">")// || str.substr(l,1)==".")
		{
			if(comm=="")
			{
				if(eqop)
				{
					eqop = false;
				}
				else if(!eqop)
				{
					commands.push_back(str.substr(l,1));
				}
			}
			else
			{
				if(comm.find_first_not_of(" ")!=string::npos && comm.find_last_not_of(" ")!=string::npos)
				{
					comm = comm.substr(comm.find_first_not_of(" "),comm.find_last_not_of(" ")+1);
					commands.push_back(comm);
				}
				if(str.substr(l,2)=="==" || str.substr(l,2)=="||" || str.substr(l,2)=="&&" || str.substr(l,2)=="!="
					|| str.substr(l,2)==">=" || str.substr(l,2)=="<=" || str.substr(l,2)=="=>" || str.substr(l,2)=="=<"
					|| str.substr(l,2)=="<<" || str.substr(l,2)==">>" || str.substr(l,2)=="++" || str.substr(l,2)=="--"
					|| str.substr(l,2)=="+=" || str.substr(l,2)=="-=")
				{
					eqop=true;
					commands.push_back(str.substr(l,2));
				}
				else if(!eqop)
					commands.push_back(str.substr(l,1));
			}
			comm = "";
		}
		else
		{
			comm += str.substr(l,1);
			if(comm=="else ")
			{
				commands.push_back("else");
				comm = "";
			}
		}
	}
	if(comm!="")
	{
		comm = comm.substr(comm.find_first_not_of(" "));
		commands.push_back(comm);
	}
	vector<string>::iterator iter;
	iter = commands.begin();
	string nextToken = *(iter);
	while(iter<commands.end())
	{
		int cucnt = 0;
		if(nextToken=="{")
		{
			while(1)
			{
				if(nextToken=="{")
					cucnt++;
				else if(nextToken=="}")
				{
					cucnt--;
					if(cucnt==0)
						break;
				}
				else if(isCommand(nextToken))
					hanldeCommand(iter);
				else
					handleStatement(iter);
				nextToken = *(++iter);
			}
		}
		else
		{
			if(isCommand(nextToken))
				hanldeCommand(iter);
			else
				handleStatement(iter);
		}
		++iter;
		if(iter<commands.end())
			nextToken = *(iter);
	}
	//while(iter!=commands.end())
	//{
		//cout << commands.at(k) << "\n" << flush;
		/*string curr = *(iter++);
		int crcnt = 0;
		vector<string> state;
		string cond;
		string prev;
		while(curr=="if")
		{
			string st = *(iter++);
			if(st=="(")
			{
				crcnt++;
				cond = "";
			}
			else if(st==")")
			{
				crcnt--;
				state.push_back(evaluate(cond,"IF"));
				cout << cond << flush;
				if(crcnt==0)
					break;
			}
			else if(st=="||")
			{
				state.push_back(evaluate(cond,"IF"));
				state.push_back("OR");
				cout << cond << flush;
				cout << " OR " << flush;
				cond = "";
				prev = "OR";
			}
			else if(st=="&&")
			{
				state.push_back(evaluate(cond,"IF"));
				state.push_back("AND");
				cout << cond << flush;
				cout << " AND " << flush;
				cond = "";
				prev = "AND";
			}
			else
				cond += st;
		}*/
		/*if(commands.at(k).find("=")!=string::npos)//inititialization operation
		{
			vector<string> bs,lhs;
			boost::iter_split(bs, commands.at(k), boost::first_finder("="));
			boost::iter_split(lhs, bs.at(0), boost::first_finder(" "));
			if(lhs.size()==2)
			{
				if(isInBuiltType(lhs.at(0)))
				{
					storeInbuilt(lhs.at(0),lhs.at(1));
					evaluateUpdateInbuilt(lhs.at(0),lhs.at(1),bs.at(1),true);
				}
				else
				{
					storeCustom(lhs.at(0),lhs.at(1));
				}
			}
			else
			{
				bool local = true;
				Object target = localVariables[lhs.at(0)];
				if(target.getTypeName()=="")
				{
					target = boundVariables[lhs.at(0)];
					mapVars::iterator it;
					it=localVariables.find(lhs.at(0));
					localVariables.erase (it);
					local = false;
				}
				if(isInBuiltType(target.getTypeName()))
					evaluateUpdateInbuilt(target.getTypeName(),lhs.at(0),bs.at(1),local);
			}
		}*/
		/*else if(tokens.size()==4 && tokens.at(2)=="=")//assignment and initialization operations
		{
			if(isInBuiltType(tokens.at(0)))
			{
				storeInbuilt(tokens.at(0),tokens.at(1));
			}
			else
			{
				storeCustom(tokens.at(0),tokens.at(1));
			}
			Object target = localVariables[tokens.at(1)];
			if(target.getType()=="")
				target = boundVariables[tokens.at(1)];
			Object source = localVariables[tokens.at(1)];
			if(source.getType()=="")
				source = boundVariables[tokens.at(1)];
			if(target.getType()!="" && source.getType()!="" && target.getType()==source.getType())
				target.setPointer(source.getPointer());
			else
				cout << "run time exception" << flush;
		}
		else if(tokens.size()==3 && tokens.at(1)=="=")//assignment operation
		{
			Object target = localVariables[tokens.at(0)];
			if(target.getType()=="")
				target = boundVariables[tokens.at(0)];
			Object source = localVariables[tokens.at(2)];
			if(source.getType()=="")
				source = boundVariables[tokens.at(2)];
			if(target.getType()!="" && source.getType()!="" && target.getType()==source.getType())
				target.setPointer(source.getPointer());
			else
				cout << "run time exception" << flush;
		}
		else if(tokens.size()>=5 && tokens.at(1)=="=")//multiple opeartions and final addition on this line
		{

		}*/
	//}
}
