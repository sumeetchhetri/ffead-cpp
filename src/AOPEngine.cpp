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
 * AOPEngine.cpp
 *
 *  Created on: Oct 17, 2009
 *      Author: sumeet
 */

#include "AOPEngine.h"

AOPEngine::AOPEngine() {
	// TODO Auto-generated constructor stub

}

AOPEngine::~AOPEngine() {
	// TODO Auto-generated destructor stub
}

void AOPEngine::execute(string fileName)
{
	string data;
	string include;
	ifstream infile;
	ofstream ofile;
	infile.open(fileName.c_str());
	Aspect aspect;
	if(infile)
	{
		while(getline(infile, data))
		{
			if(data!="")
			{
				if(data.find("@INCLUDE")!=string::npos)
				{
					StringUtil::eraseAll(data,"@INCLUDE(\"");
					StringUtil::eraseAll(data,"\")");
					args tr;
					StringUtil::split(tr, data, (";"));
					for(unsigned int i=0;i<tr.size();i++)
					{
						include += ("#include \"" + tr.at(i) + "\"\n");
					}
				}
				else if(data.find("@ASPECTBEGIN")!=string::npos)
				{
					aspect.clear();
				}
				else if(data.find("@EXECUTION")!=string::npos)
				{
					StringUtil::eraseAll(data,"@EXECUTION(\"");
					StringUtil::eraseAll(data,"\")");
					args tr;
					StringUtil::split(tr, data, (" "));
					for(unsigned int i=0;i<tr.size();i++)
					{
						if(i==0)
							aspect.setClassName(tr.at(i));
						else
							aspect.setMethodName(tr.at(i));
					}
				}
				else if(data.find("@STRICT")!=string::npos)
				{
					StringUtil::eraseAll(data,"@STRICT(\"");
					StringUtil::eraseAll(data,"\")");
					aspect.setStrict(data);
				}
				else if(data.find("@ARGS")!=string::npos)
				{
					StringUtil::eraseAll(data,"@ARGS(\"");
					StringUtil::eraseAll(data,"\")");
					aspect.setArguTypes(data);
					args tr;
					StringUtil::split(tr, data, (";"));
					aspect.setArgumentTypes(tr);
				}
				else if(data.find("@RETURN")!=string::npos)
				{
					StringUtil::eraseAll(data,"@RETURN(\"");
					StringUtil::eraseAll(data,"\")");
					aspect.setReturnType(data);
				}
				else if(data.find("@WHEN")!=string::npos)
				{
					StringUtil::eraseAll(data,"@WHEN(\"");
					StringUtil::eraseAll(data,"\")");
					aspect.setWhen(data);
				}
				else if(data.find("@CODE")!=string::npos)
				{
					StringUtil::eraseAll(data,"@CODE(");
					StringUtil::eraseAll(data,")");
					aspect.setCode(data);
				}
				else if(data.find("@ASPECTEND")!=string::npos)
				{
					this->aspects.push_back(aspect);
				}
			}
		}
		infile.close();
	}
	if(this->aspects.size()>0)
	{
		for(unsigned int i=0;i<this->aspects.size();i++)
		{
			aspect.clear();
			aspect = this->aspects.at(i);
			string classN = "/home/sumeet/workspace/default/"+aspect.getClassName()+".cpp";
			infile.open(classN.c_str());
			string content;
			if(infile)
			{
				while(getline(infile, data))
				{
					content += (data + "\n");
				}
				string signature;
				if(aspect.getReturnType()=="*")
				{
					if(aspect.getStrict()=="true")
						signature += (" "+aspect.getClassName()+"::"+aspect.getMethodName()+"(");
					else
						signature += (aspect.getMethodName()+"(");
				}
				else
				{
					if(aspect.getStrict()=="true")
						signature += (aspect.getReturnType()+" "+aspect.getClassName()+"::"+aspect.getMethodName()+"(");
					else
						signature += (aspect.getReturnType()+" "+aspect.getMethodName()+"(");
				}
				string prior,later,temp,argus,arginit,argsig;
				unsigned int f = content.find(signature);
				if(f!=string::npos)
				{
					prior = content.substr(0,f);
					later = content.substr(f);
					unsigned s = later.find_first_of("(")+1;
					unsigned e = later.find_first_of(")");
					argus = later.substr(s,e-s);
					args tr,tc;
					StringUtil::split(tr, argus, (","));
					for(unsigned int i=0;i<tr.size();i++)
					{
						StringUtil::split(tc, tr.at(i), (" "));
						stringstream ss;
						ss << (i+1);
						string te;
						ss >> te;
						arginit += (tc.at(0) + " _" + te + " = " + tc.at(1)) + ";\n";
						argsig += (tc.at(0));
						if(i!=tr.size()-1)
							argsig += ",";
					}
					if(aspect.getArguTypes()==argsig)
					{
						unsigned g = later.find_first_of("{");
						prior = content.substr(0,f+g+1);
						later = content.substr(f+g+1);
						unsigned h = later.find_first_of("}");
						temp = later.substr(0,h);
						later = later.substr(h);
						if(aspect.getWhen()=="BEFORE")
						{
							temp = ("\n" + arginit + aspect.getCode() + "\n" + temp);
						}
						else
						{
							temp = (temp + "\n" + arginit + aspect.getCode() + "\n");
						}
						content = (include + prior + temp + later);
						string aop_name = "/home/sumeet/Desktop/_AOP_"+aspect.getClassName()+".cpp";
						ofile.open(aop_name.c_str());
						ofile.write(content.c_str(),content.length());
						ofile.close();
					}
				}
				infile.close();
			}
		}
	}
}
