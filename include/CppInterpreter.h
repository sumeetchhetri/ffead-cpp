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
 * CppInterpreter.h
 *
 *  Created on: Aug 22, 2009
 *      Author: sumeet
 */

#ifndef CPPINTERPRETER_H_
#define CPPINTERPRETER_H_
#include "string"
#include "map"
#include "Object.h"
#include "Reflector.h"
#include "iostream"
#include "CastUtil.h"
#include "StringUtil.h"
#include "RegexUtil.h"
#include "bitset"
#include "LoggerFactory.h"
typedef map<string,Object> mapVars;
typedef map<string,string> mapStrs;
using namespace std;

class Obj
{
	string type;
	void* pointer;
public:
	Obj(){}
	~Obj(){}
    string getType() const
    {
        return type;
    }

    void setType(string type)
    {
        this->type = type;
    }

    void *getPointer() const
    {
        return pointer;
    }

    void setPointer(void *pointer)
    {
        this->pointer = pointer;
    }

};
class CppInterpreter {
	Logger logger;
	mapVars localVariables;
	mapVars boundVariables;
	mapStrs literals;
	mapVars getLocalVariables() const;
	void setLocalVariables(mapVars);
	mapVars getBoundVariables() const;
	void setBoundVariables(mapVars);
	bool isInBuiltType(string);
	void storeInbuilt(string,string);
	void storeCustom(string,string);
	void evaluateUpdateInbuilt(string,string,string,vector<string>,bool);
	void evaluateUpdateCustom(string,string,string,vector<string>,bool);
	void executeStatement(string sep,vector<string> lhs,vector<string> rhs);
	bool evaluateCondition(string condition);
	bool evalCond(vector<string> str);
	bool isCommand(string test);
	bool containsChar(string varname);
	bool retState(string type,Object source,Object target);
	bool retState(string type,Object source,string target);
	void hanldeIF(vector<string>::iterator &iter);
	void handleELSE(vector<string>::iterator &iter);
	void hanldeFOR(vector<string>::iterator &iter);
	void hanldeWHILE(vector<string>::iterator &iter);
	void hanldeCommand(vector<string>::iterator &itr);
	void handleStatement(vector<string>::iterator &itr);
	void skipStatement(vector<string>::iterator &itr);
	void skipCommand(vector<string>::iterator &itr);
	Obj handleObjectMethodInvocation(string,string,vector<string>::iterator &itr);
	template<class T> string evalBrackets(vector<string>::iterator &itr,vector<string>::iterator enditr)
	{
		string token = *(itr);
		vector<string> curr,going;
		bool st = false;
		while(itr<enditr)
		{
			token = *(itr);
			if(token=="(")
			{
				if(st)
					curr.push_back(evalBrackets<T>(itr,enditr));
				st = true;
			}
			else if(token==")")
			{
				curr.push_back(handleAssignment<T>(going));
				st = false;
				going.clear();
			}
			else
			{
				if(containsChar(token))
				{
					Object o = localVariables[token];
					token = CastUtil::lexical_cast<string>(o.getValue<int>());
				}
				if(st)
					going.push_back(token);
				else
					curr.push_back(token);
			}
			++itr;
		}
		return handleAssignment<T>(curr);
	}
	template<class T> string handleAssignment(vector<string> opr)
	{
		vector<string> temp;
		while(opr.size()>1)
		{
			bool continu = false;
			temp.clear();
			while(1)
			{
				//unsigned int found = -1;
				for(int i=0;i<(int)opr.size();i++)
				{
					if(opr.at(i)=="/" && !continu)
					{
						T f;
						f = CastUtil::lexical_cast<T>(opr.at(i-1))/CastUtil::lexical_cast<T>(opr.at(i+1));
						for(int k=0;k<(i-1);k++)
						{
							temp.push_back(opr.at(k));
						}
						temp.push_back(CastUtil::lexical_cast<string>(f));
						for(int k=i+2;k<(int)opr.size();k++)
						{
							temp.push_back(opr.at(k));
						}
						continu = true;
						//found = i;
					}
					/*else if((opr.size()>(i+2) && opr.at(i+2)!='/') && i!=found && i!=found+1 && i!=found-1)
					{
						temp.push_back(opr.at(i));
					}*/
				}
				if(!continu)
					break;
				else
				{
					continu = false;
					if(temp.size()>0)opr = temp;
					temp.clear();
				}
			}
			continu = false;
			temp.clear();
			while(1)
			{
				//unsigned int found = -1;
				for(int i=0;i<(int)opr.size();i++)
				{
					if(opr.at(i)=="*" && !continu)
					{
						T f;
						f = CastUtil::lexical_cast<T>(opr.at(i-1))*CastUtil::lexical_cast<T>(opr.at(i+1));
						for(int k=0;k<i-1;k++)
						{
							temp.push_back(opr.at(k));
						}
						temp.push_back(CastUtil::lexical_cast<string>(f));
						for(int k=i+2;k<(int)opr.size();k++)
						{
							temp.push_back(opr.at(k));
						}
						continu = true;
						//found = i;
					}
					/*else if(i!=found && i!=found+1 && i!=found-1)
					{
						temp.push_back(opr.at(i));
					}*/
				}
				if(!continu)
					break;
				else
				{
					continu = false;
					if(temp.size()>0)opr = temp;
					temp.clear();
				}
			}
			continu = false;
			temp.clear();
			/*while(1)
			{
				unsigned int found = -1;
				for(int i=0;i<(int)opr.size();i++)
				{
					if(opr.at(i)=="-" && !continu)
					{
						T f;
						f = CastUtil::lexical_cast<T>(opr.at(i-1))-CastUtil::lexical_cast<T>(opr.at(i+1));
						for(int k=0;k<i-1;k++)
						{
							temp.push_back(opr.at(k));
						}
						temp.push_back(CastUtil::lexical_cast<string>(f));
						for(int k=i+2;k<(int)opr.size();k++)
						{
							temp.push_back(opr.at(k));
						}
						continu = true;
						found = i;
					}
				}
				if(!continu)
					break;
				else
				{
					continu = false;
					if(temp.size()>0)opr = temp;
					temp.clear();
				}
			}
			continu = false;
			temp.clear();*/
			while(1)
			{
				//unsigned int found = -1;
				for(int i=0;i<(int)opr.size();i++)
				{
					if(opr.at(i)=="+" && !continu)
					{
						T f;
						f = CastUtil::lexical_cast<T>(opr.at(i-1))+CastUtil::lexical_cast<T>(opr.at(i+1));
						for(int k=0;k<i-1;k++)
						{
							temp.push_back(opr.at(k));
						}
						temp.push_back(CastUtil::lexical_cast<string>(f));
						for(int k=i+2;k<(int)opr.size();k++)
						{
							temp.push_back(opr.at(k));
						}
						continu = true;
						//found = i;
					}
					else if(opr.at(i)=="-" && !continu)
					{
						T f;
						f = CastUtil::lexical_cast<T>(opr.at(i-1))+CastUtil::lexical_cast<T>(opr.at(i)+opr.at(i+1));
						for(int k=0;k<i-1;k++)
						{
							temp.push_back(opr.at(k));
						}
						temp.push_back(CastUtil::lexical_cast<string>(f));
						for(int k=i+2;k<(int)opr.size();k++)
						{
							temp.push_back(opr.at(k));
						}
						continu = true;
						//found = i;
					}
					/*else if(i!=found && i!=found+1 && i!=found-1)
					{
						temp.push_back(opr.at(i));
					}*/
				}
				if(!continu)
					break;
				else
				{
					continu = false;
					if(temp.size()>0)opr = temp;
					temp.clear();
				}
			}
		}
		//if(opr.size()>0)
			return opr.at(0);
		//return "";
	}
public:
	CppInterpreter();
	virtual ~CppInterpreter();
    void eval(string);
    template <class T> void bind(string name,T &t)
    {
    	Object o;
    	o << t;
    	boundVariables[name] = o;
    }
    template <class T> void bind(string name,T *t)
	{
		Object o;
		o << t;
		boundVariables[name] = o;
	}
    /*template <class T> void bind(map<string,T> mapT)
    {
    	map<string,string>::iterator it;
    	for(it=mapT.begin();it!=mapT.end();++it)
    	{
			Object o;
			o << it->second;
			boundVariables[it->first] = o;
    	}
    }
    template <class T> T getVariable(string name)
    {
    	Object o = localVariables[name];
    	return o.getValue<T>();
    }
    template <class T> T getCollectionVariable(string name,string type,string index="")
    {
    	if(type=="vector")
    	{

    	}
    	else if(type=="deque")
    	{

    	}
    	else if(type=="list")
    	{

    	}
    	else if(type=="stack")
    	{

    	}
    	else if(type=="queue")
    	{

    	}
    	else if(type=="priority_queue")
    	{

    	}
    	else if(type=="set")
    	{

    	}
    	else if(type=="multiset")
    	{

    	}
    	else if(type=="map")
    	{

    	}
    	else if(type=="bitset")
    	{

    	}
    }*/
};

#endif /* CPPINTERPRETER_H_ */
