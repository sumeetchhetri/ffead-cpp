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
 * Reflection.cpp
 *
 *  Created on: Aug 21, 2009
 *      Author: sumeet
 */

#include "Reflection.h"

using namespace std;

//map<string,bool> Reflection::invalidcls;
//map<string,string> Reflection::clspaths;
//map<string,int> Reflection::nmspcIds;

Reflection::Reflection() {
	logger = Logger::getLogger("Reflection");
}

Reflection::~Reflection() {
	// TODO Auto-generated destructor stub
}

vector<string> Reflection::list(string cwd)
{
	FILE *pipe_fp;
	string command;
	vector<string> files;
	if(chdir(cwd.c_str())!=0)
		return files;
	command = ("find . \\( ! -name . -prune \\) \\( -type f -o -type l \\) -name '*.h' 2>/dev/null");
	//command = "ls -F1 "+cwd+"|grep '.h'";
	logger << ("Searching directory " + cwd + " for pattern .h") << endl;
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
			StringUtil::replaceFirst(fileName,"*","");
			StringUtil::replaceFirst(fileName,"./","");
			if(fileName.find("~")==string::npos)
			{
				fileName = cwd+"/"+fileName;
				StringUtil::replaceFirst(fileName,"//","/");
				files.push_back(fileName);
			}
			fileName = "";
		}
	}
	pclose(pipe_fp);
	return files;
}


void Reflection::collectInfo(string data, string flag, ClassStructure& cls)
{
	if(flag=="public")
		cls.pub.push_back(data);
	else if(flag=="protected")
		cls.pro.push_back(data);
	else if(flag=="private")
		cls.pri.push_back(data);
}

int Reflection::findless(int a, int b, int c)
{
	if(a!=-1 && b!=-1 && c!=-1)
	{
		if(a<b && a<c)
		{
			return a;
		}
		if(b<a && b<c)
		{
			return b;
		}
		if(c<a && c<b)
		{
			return c;
		}
	}
	else if(a!=-1 && b!=-1)
	{
		if(a<b)
		{
			return a;
		}
		else
		{
			return b;
		}
	}
	else if(a!=-1 && c!=-1)
	{
		if(a<c)
		{
			return a;
		}
		else
		{
			return c;
		}
	}
	else if(b!=-1 && c!=-1)
	{
		if(b<c)
		{
			return b;
		}
		else
		{
			return c;
		}
	}
	else if(a!=-1)
	{
		return a;
	}
	else if(b!=-1)
	{
		return b;
	}
	else if(c!=-1)
	{
		return c;
	}
	return -1;
}

void Reflection::handleNamespace(string data, string namepsc, map<string, ClassStructure>& clsvec, map<string, vector<string> >& glbnmspcs)
{
	StringUtil::trim(data);
	if(data=="")return;
	string nmspc;
	cout << clsvec.size() << endl;
	if(RegexUtil::find(data, "^[ \\t]*using[ \\t]*namespace[ \\t]*[^;]+;")==0)
	{
		string nmspace = data.substr(0, data.find(";"));
		nmspace = nmspace.substr(nmspace.find_last_of(" ")+1);
		StringUtil::trim(nmspace);
		data = data.substr(data.find(";")+1);
		if(nmspace!="")
		{
			if(clsvec.find(namepsc)!=clsvec.end())
			{
				clsvec[namepsc].namespaces.push_back(nmspace);
			}
			else
			{
				glbnmspcs[namepsc].push_back(nmspace);
			}
		}
		handleNamespace(data, namepsc, clsvec, glbnmspcs);
	}
	else if(data.find("namespace ")==0)
	{
		string temp = data.substr(0, data.find("{"));
		StringUtil::trim(temp);
		StringUtil::replaceFirst(temp,"namespace ","");
		nmspc = namepsc + temp+"::";
		namepsc += temp+"::";
		data = data.substr(data.find("{"));
		string nmdata = data;
		size_t cbst = nmdata.find("{", 1);
		size_t enind = nmdata.find("}");
		bool flag = false;
		if(enind!=string::npos && cbst!=string::npos && enind>cbst)
		{
			enind = 0;
			while(nmdata.find("}", enind)!=string::npos)
			{
				string test = nmdata.substr(0, nmdata.find("}", enind)+1);
				int stcnt = StringUtil::countOccurrences(test, "{");
				int encnt = StringUtil::countOccurrences(test, "}");
				if(stcnt==encnt)
				{
					enind = nmdata.find("}", enind);
					flag = true;
					break;
				}
				enind = nmdata.find("}", enind+1) + 1;
			}
		}
		else if(enind!=string::npos && nmdata.find("{")!=string::npos)
		{
			flag = true;
		}
		if(flag)
		{
			int st = data.find("{");
			int en = enind;
			string tdata = data.substr(en+1);
			data = data.substr(st+1, en-st-1);
			StringUtil::trim(data);
			StringUtil::trim(tdata);
			cout << "nmspc = " << nmspc << endl;
			cout << data << endl;
			cout << tdata << endl;
			handleNamespace(data, nmspc, clsvec, glbnmspcs);
			handleNamespace(tdata, namepsc, clsvec, glbnmspcs);
		}
		else
		{
			cout << "error" << endl;
		}
	}
	else if(data.find("class ")==0)
	{
		vector<string> results;
		string temp = data.substr(0, data.find("{"));
		StringUtil::trim(temp);
		StringUtil::replaceFirst(temp,"class ","");
		StringUtil::replaceFirst(temp,":"," ");
		RegexUtil::replace(temp, "[ ]+", " ");
		StringUtil::split(results, temp, (" "));
		string classN,baseClassN,bcvisib,namSpc;
		classN = results.at(0);
		namSpc = namepsc;
		if(results.size()>=3)
		{
			bcvisib = results.at(1);
			StringUtil::replaceAll(bcvisib," ","");
			baseClassN = results.at(2);
			StringUtil::replaceAll(baseClassN," ","");
		}
		nmspc = namepsc + classN+"::";
		namepsc += temp+"::";
		data = data.substr(data.find("{"));
		string nmdata = data;
		size_t cbst = nmdata.find("{", 1);
		size_t enind = nmdata.find("}");
		bool flag = false;
		if(enind!=string::npos && cbst!=string::npos && enind>cbst)
		{
			enind = 0;
			while(nmdata.find("}", enind)!=string::npos)
			{
				string test = nmdata.substr(0, nmdata.find("}", enind)+1);
				int stcnt = StringUtil::countOccurrences(test, "{");
				int encnt = StringUtil::countOccurrences(test, "}");
				if(stcnt==encnt)
				{
					enind = nmdata.find("}", enind);
					flag = true;
					break;
				}
				enind = nmdata.find("}", enind+1) + 1;
			}
		}
		else if(enind!=string::npos && nmdata.find("{")!=string::npos)
		{
			flag = true;
		}
		if(flag)
		{
			ClassStructure cstruc;
			cstruc.classN = classN;
			cstruc.baseClassN = baseClassN;
			cstruc.bcvisib = bcvisib;
			cstruc.nmSpc = namSpc;
			clsvec[nmspc] = cstruc;
			int st = data.find("{");
			int en = enind;
			string tdata = data.substr(en+1);
			data = data.substr(st+1, en-st-1);
			StringUtil::trim(data);
			StringUtil::trim(tdata);
			cout << "classnmpsc = " << nmspc << endl;
			cout << data << endl;
			cout << tdata << endl;
			handleNamespace(data, nmspc, clsvec, glbnmspcs);
			handleNamespace(tdata, namepsc, clsvec, glbnmspcs);
		}
		else
		{
			cout << "error" << endl;
		}
	}
	else if(data.find(" namespace ")!=string::npos)
	{
		string sdata = data.substr(0, data.find(" namespace "));
		handleNamespace(sdata, namepsc, clsvec, glbnmspcs);
		handleNamespace(data.substr(data.find(" namespace ")), namepsc, clsvec, glbnmspcs);
	}
	else if(data.find(" class ")!=string::npos)
	{
		string sdata = data.substr(0, data.find(" class "));
		handleNamespace(sdata, namepsc, clsvec, glbnmspcs);
		handleNamespace(data.substr(data.find(" class ")), namepsc, clsvec, glbnmspcs);
	}
	else if(clsvec.find(namepsc)!=clsvec.end())
	{
		while(data.find("{")!=string::npos)
		{
			RegexUtil::replace(data, "{[^{}]*}", ";");
		}
		RegexUtil::replace(data, "\"[^\"]+\"", "\"\"");
		vector<string> parts = StringUtil::split(data, ";");
		if(parts.size()>0)
		{
			string flag = "private";
			for (int var = 0; var < (int)parts.size(); ++var) {
				int pust, puen;
				RegexUtil::find(parts.at(var), "[ \\t]*public[ \\t]*:", pust, puen);
				int pvst, pven;
				RegexUtil::find(parts.at(var), "[ \\t]*private[ \\t]*:", pvst, pven);
				int prst, pren;
				RegexUtil::find(parts.at(var), "[ \\t]*protected[ \\t]*:", prst, pren);

				int pls = findless(pvst, prst, pust);
				string pubdata, privdata, protdata;
				if(pls!=-1 && pls == pvst)
				{
					string predat = parts.at(var).substr(0, pvst);
					string postdat = parts.at(var).substr(pven);

					if(flag == "public")
					{
						pubdata += predat;
					}
					else if(flag == "private")
					{
						privdata += predat;
					}
					else
					{
						protdata += predat;
					}

					pls = findless(-1, prst, pust);
					if(pls!=-1 && pls == prst)
					{
						privdata += postdat.substr(0, prst);
						postdat = postdat.substr(pren);

						if(pust!=-1)
						{
							protdata += postdat.substr(0, pust);
							postdat = postdat.substr(puen);
							pust = -1;
						}
					}

					if(pust!=-1)
					{
						privdata += postdat.substr(0, pust);
						postdat = postdat.substr(puen);
					}

					StringUtil::trim(postdat);
					if(postdat!="")
					{
						privdata += postdat;
					}
				}
				else if(pls!=-1 && pls == prst)
				{
					string predat = parts.at(var).substr(0, prst);
					string postdat = parts.at(var).substr(pren);

					if(flag == "public")
					{
						pubdata += predat;
					}
					else if(flag == "private")
					{
						privdata += predat;
					}
					else
					{
						protdata += predat;
					}

					pls = findless(pvst, -1, pust);
					if(pls!=-1 && pls == pvst)
					{
						protdata += postdat.substr(0, pvst);
						postdat = postdat.substr(pven);

						if(pust!=-1)
						{
							privdata += postdat.substr(0, pust);
							postdat = postdat.substr(puen);
							pust = -1;
						}
					}

					if(pust!=-1)
					{
						protdata += postdat.substr(0, pust);
						postdat = postdat.substr(puen);
					}

					StringUtil::trim(postdat);
					if(postdat!="")
					{
						protdata += postdat;
					}
				}
				else if(pls!=-1 && pls == pust)
				{
					string predat = parts.at(var).substr(0, pust);
					string postdat = parts.at(var).substr(puen);

					if(flag == "public")
					{
						pubdata += predat;
					}
					else if(flag == "private")
					{
						privdata += predat;
					}
					else
					{
						protdata += predat;
					}

					pls = findless(pvst, prst, -1);
					if(pls!=-1 && pls == pvst)
					{
						pubdata += postdat.substr(0, pvst);
						postdat = postdat.substr(pven);

						if(prst!=-1)
						{
							privdata += postdat.substr(0, prst);
							postdat = postdat.substr(pren);
							prst = -1;
						}
					}

					if(prst!=-1)
					{
						pubdata += postdat.substr(0, prst);
						postdat = postdat.substr(pren);
					}

					StringUtil::trim(postdat);
					if(postdat!="")
					{
						pubdata += postdat;
					}
				}
				else
				{
					if(flag == "public")
					{
						pubdata = parts.at(var);
					}
					else if(flag == "private")
					{
						privdata = parts.at(var);
					}
					else
					{
						protdata = parts.at(var);
					}
				}
				if(pubdata!="")
				{
					flag = "public";
					StringUtil::trim(pubdata);
					collectInfo(pubdata, "public", clsvec[namepsc]);
				}
				else if(privdata!="")
				{
					flag = "private";
					StringUtil::trim(privdata);
					collectInfo(privdata, "private", clsvec[namepsc]);
				}
				else if(protdata!="")
				{
					flag = "protected";
					StringUtil::trim(protdata);
					collectInfo(protdata, "protected", clsvec[namepsc]);
				}
			}
		}
	}
	return;
}


map<string, ClassStructure> Reflection::getClassStructures(string className)
{
	map<string, ClassStructure> clsvec;
	string data;
	ifstream infile;
	infile.open(className.c_str());
	string allcont;
	if(infile.is_open())
	{
		bool commstrts = false;
		while(getline(infile, data))
		{
			RegexUtil::replace(data, "[\t]+", " ");
			RegexUtil::replace(data, "[ ]+", " ");
			StringUtil::trim(data);
			if(data.find("//")==0)
			{
				continue;
			}
			else if(data.find("//")!=string::npos)
			{
				data = data.substr(0, data.find("//"));
			}
			if(commstrts && data.find("*/")!=string::npos)
			{
				commstrts = false;
				data = data.substr(data.find("*/")+2);
			}
			while(data.find("/*")!=string::npos)
			{
				if(data.find("*/")!=string::npos)
				{
					commstrts = false;
					data = data.substr(0, data.find("/*")) + data.substr(data.find("*/")+2);
				}
				else
				{
					commstrts = true;
					data = data.substr(0, data.find("/*"));
					break;
				}
			}
			if(!commstrts)
			{
				StringUtil::trim(data);
				allcont.append(data);
				allcont.append(" ");
			}
		}
		infile.close();

		data = allcont;
		RegexUtil::replace(data, "^[ \\t]*#include[ \\t]*\"[^\"]+\"", "");
		RegexUtil::replace(data, "^[ \\t]*#include[ \\t]*<[^<>]+>", "");
		RegexUtil::replace(data, "^[ \\t]*#define[ \\t]*[a-zA-Z0-9_]*[ \\t]*[0-9]*", "");
		RegexUtil::replace(data, "^[ \\t]*#define[ \\t]*[a-zA-Z0-9_]*[ \\t]*\"[^\"]+\"", "");
		RegexUtil::replace(data, "^[ \\t]*typedef[ \\t]*[^;]+;", "");

		string file = className.substr(className.find_last_of("/")+1);

		map<string, vector<string> > glbnmspcs;
		handleNamespace(data, "", clsvec, glbnmspcs);
		map<string, ClassStructure>::iterator it;
		vector<string> remnmspcs;
		for (it=clsvec.begin();it!=clsvec.end();++it) {

			if(it->second.pub.size()>0)
			{
				clspaths[it->second.getTreatedClassName(true)] = file;
				StringUtil::trim(it->second.nmSpc);
				if(it->second.nmSpc!="" && nmspcIds.find(it->second.nmSpc)==nmspcIds.end())
				{
					nmspcIds[it->second.nmSpc] = nmspcIds.size()+1;
				}
				classNamespaces[it->second.nmSpc+it->second.classN];
			}
			else
			{
				remnmspcs.push_back(it->first);
			}

			vector<string> nnspcs;
			if(glbnmspcs.find(it->second.nmSpc)!=glbnmspcs.end())
			{
				copy(glbnmspcs[it->second.nmSpc].begin(), glbnmspcs[it->second.nmSpc].end(), std::back_inserter(nnspcs));
			}
			copy(it->second.namespaces.begin(), it->second.namespaces.end(), std::back_inserter(nnspcs));
			it->second.namespaces = nnspcs;
			cout << "=========================================" << endl;
			it->second.toString();
			cout << "=========================================" << endl;
		}
		for (int var = 0; var < remnmspcs.size(); ++var) {
			clsvec.erase(remnmspcs.at(var));
		}
	}
	return clsvec;
}


/*bool Reflection::generateClassInfoFromDD(string alldata)
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
			vector<string> results;
			StringUtil::replaceFirst(data,":","");
			StringUtil::split(results, data, (" "));
			this->classN = results.at(1);
			if(results.size()>3)
			{
				this->bcvisib = results.at(2);
				this->baseClassN = results.at(3);
				//logger << results.size() << flush;
			}
			//StringUtil::split(results, data, (": "));
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
	//logger << pub.size() << pri.size() << pro.size() << flush;
	if (this->pub.size() > 0 || this->pri.size() > 0 || this->pro.size() > 0)
		return true;
	else
		return false;
}*/


propMap Reflection::getDbTableInfo(string file)
{
	propMap tabInfo;
	string temp;
	vector<string> all;
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

vector<string> Reflection::getAfcObjectData(ClassStructure classStructure,bool object,vector<string>& privf, bool &isOpForSet)
{
	isOpForSet = false;
	vector<string> refDef;
	vector<string> publf, protf ,publm, privm, protm;

	size_t tes;
	if (classStructure.pub.size() > 0)
	{
		for (unsigned int i = 0; i < classStructure.pub.size(); i++)
		{
			if((tes=classStructure.pub.at(i).find("("))!=string::npos && (tes=classStructure.pub.at(i).find(")"))!=string::npos)
			{
				publm.push_back(classStructure.pub.at(i));

				string meth = classStructure.pub.at(i);
				StringUtil::replaceFirst(meth,";","");
				RegexUtil::replace(meth, "[\t]+", " ");
				RegexUtil::replace(meth, "[ ]+", " ");
				//RegexUtil::replace(meth, "[ ?, ?]+", ",");
				meth = meth.substr(0,meth.find("("));
				if(meth.find("operator")!=string::npos)
				{
					if(meth.find("<")!=string::npos)
					{
						isOpForSet = true;
					}
					else if(meth.find(">")!=string::npos)
					{
						isOpForSet = true;
					}
				}
			}
			else
			{
				publf.push_back(classStructure.pub.at(i));
			}
		}
	}
	if (classStructure.pri.size() > 0)
	{
		for (unsigned int i = 0; i < classStructure.pri.size(); i++)
		{
			if((tes=classStructure.pri.at(i).find("("))!=string::npos && (tes=classStructure.pri.at(i).find(")"))!=string::npos)
			{
				privm.push_back(classStructure.pri.at(i));
			}
			else
			{
				privf.push_back(classStructure.pri.at(i));
			}
		}
	}
	if (classStructure.pro.size() > 0)
	{
		for (unsigned int i = 0; i < classStructure.pro.size(); i++)
		{
			if((tes=classStructure.pro.at(i).find("("))!=string::npos && (tes=classStructure.pro.at(i).find(")"))!=string::npos)
			{
				protm.push_back(classStructure.pro.at(i));
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

/*vector<string> Reflection::getAfcObjectData(string className,bool object)
{
	vector<string> refDef;
	if (!generateClassInfo(className))
	{
		return refDef;
	}
	vector<string> publf, privf, protf ,publm, privm, protm;

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
}*/

string Reflection::generateClassDefinitionsAll(vector<string> all,string &includeRef,vector<string> apps)
{
	string ret = "";
	//includeRef = "#ifndef REFLECTOR_H_\n#define REFLECTOR_H_\n#include \"ClassInfo.h\"\n#include \"string\"\n#include \"Method.h\"\n#include \"Field.h\"\n";
	//includeRef += "#include \"XmlParser.h\"\n#include <stdio.h>\n#include <sys/wait.h>\n#include <stdexcept>\n#include <execinfo.h>\n#include <dlfcn.h>\n#include <cxxabi.h>\n#include <stdio.h>\n#include <stdlib.h>\n#include \"string\"\n#include <sstream>\n#include <typeinfo>\n";
	string typedefs,classes,methods,opers;
	string inc = "#include \"ClassInfo.h\"\n#include \"string\"\n#include \"Method.h\"\n#include \"Field.h\"\n";
	ret += "extern \"C\"\n{\n";
	for (unsigned int var = 0; var < all.size(); ++var)
	{
		//logger << "\nstarting for classes " << all.size() << "\n" << flush;
		ret += this->generateClassDefinitions(all.at(var),inc,typedefs,classes,methods,opers,StringUtil::trimCopy(apps.at(var)));

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

string Reflection::generateClassDefinitions(string includeDir,string &includesDefs,string &typedefs,string &classes,string &methods,string &opers,string app)
{
	vector<string> includes = list(includeDir);
	string ret,in,ty,cl,me;
	map<string, ClassStructure> allclsmap;
	for (unsigned int var = 0; var < includes.size(); ++var)
	{
		map<string, ClassStructure> clsmap = getClassStructures(includes.at(var));
		allclsmap.insert(clsmap.begin(), clsmap.end());
	}
	generateClassDefinition(allclsmap,includesDefs,typedefs,classes,methods,opers,app);
	/*for (unsigned int var = 0; var < includes.size(); ++var)
	{
		//logger << "\ngenerating for file" << includes.at(var) << "\n" << flush;
		string includesDefs1,typedefs1,classes1,methods1,opers1;
		string ret1 = generateClassDefinition(includes.at(var),includesDefs1,typedefs1,classes1,methods1,opers1,app);
		ret += ret1;
		if(ret1!="")
		{
			includesDefs += includesDefs1;
			typedefs += typedefs1;
			classes += classes1;
			methods += methods1;
			opers += opers1;
		}
		else
		{
			invalidcls[includes.at(var)] = true;
		}
		//logger << "\ndone generating for file" << includes.at(var) << "\n" << flush;
	}*/
	return ret;
}

string Reflection::generateClassDefinition(map<string, ClassStructure> allclsmap,string &includesDefs,string &typedefs,string &classes,string &methods,string &opers,string app)
{
	string refDef;
	/*if (!generateClassInfo(className))
	{
		return refDef;
	}*/



	map<string, ClassStructure>::iterator it;
	for (it=allclsmap.begin();it!=allclsmap.end();++it)
	{
		ClassStructure classStructure = it->second;
		includesDefs += "#include \"" + getClassPath(it->second.getTreatedClassName(true)) + "\"\n";
		//classes += "\tif(className==\""+classStructure.getTreatedClassName(true)+"\")\n\t\treturn get"+classStructure.getTreatedClassName(true)+"();\n";

		//string structinf = "\nstruct struct"+classStructure.classN+"{\n";
		refDef += "ClassInfo "+app+"getReflectionCIFor" + classStructure.getTreatedClassName(true) + "()\n{\nClassInfo classInfo;";
		refDef += ("\nclassInfo.setClassName(\"" + classStructure.classN + "\");");
		refDef += ("\nclassInfo.setNamespace(\"" + classStructure.nmSpc + "\");");
		//refDef += ("\nclassInfo.setInstance(new " + classStructure.getTreatedClassName(true) + ");");
		refDef += ("\nclassInfo.setBase(\"" + classStructure.bcvisib + " " + classStructure.baseClassN + "\");");
		refDef += ("\nConstructor ctor;\nMethod me;\nField f;\n");
		refDef += ("args argu;\n");
		string publf, privf, protf ,publm, privm, protm;
		string meth,fld;
		size_t tes;
		bool ctorisp = false,ddtorisp = false;;
		if (classStructure.pub.size() > 0)
		{
			for (unsigned int i = 0; i < classStructure.pub.size(); i++)
			{
				if((tes=classStructure.pub.at(i).find("("))!=string::npos && (tes=classStructure.pub.at(i).find(")"))!=string::npos && classStructure.pub.at(i).find("~")==string::npos)
				{
					refDef += ("ctor.clear();\nme.clear();\n");
					publm += classStructure.pub.at(i);
					meth = classStructure.pub.at(i);
					StringUtil::replaceFirst(meth,";","");

					string argts = meth.substr(meth.find("("),meth.find(")")-meth.find("("));
					StringUtil::replaceFirst(argts,"(","");
					StringUtil::replaceAll(argts,")","");
					meth = meth.substr(0,meth.find("("));
					//StringUtil::replaceFirst(meth,")"," ");
					vector<string> methp,methpm,argp,argpm,argpmtemp;
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
							string argpmtemp = argp.at(j);
							argpmtemp = argpmtemp.substr(argpmtemp.find_first_not_of(" "));
							argpm.push_back(argpmtemp);
						}
					}
					string typdefName,methsd,valsd,valsa;
					//bool ctor = false;
					if(methpm.size()>0 && methpm.at(0).find("virtual")!=string::npos)
						return "";
					for(unsigned int j = 0; j < methpm.size(); j++)
					{
						if(j==0)
						{
							if(methpm.at(0)==classStructure.getTreatedClassName(false))
							{
								refDef += ("ctor.setName(\""+classStructure.getTreatedClassName(true)+"\");\n");
								//refDef += ("me.setMethodName(\""+classStructure.getTreatedClassName(true)+methpm.at(j)+"\");\n");
								//ctor = true;
								methsd += (classStructure.getTreatedClassName(true));
							}
							else if(meth.find(" operator")==string::npos)
							{
								string fqcn = getFullyQualifiedClassName(methpm.at(j), classStructure.namespaces);
								refDef += ("me.setReturnType(\""+fqcn+"\");\n");
								typedefs += ("typedef " + methpm.at(j) + " ");
							}
						}
						else if(j==1 && methpm.at(0)!=classStructure.getTreatedClassName(false) && meth.find(" operator")==string::npos)
						{

							//refDef += (classStructure.getFullyQualifiedClassName() + methpm.at(j)+" = &"+classStructure.getTreatedClassName(true)+"::"+methpm.at(j)+";\n");
							typedefs += ("("+classStructure.getTreatedClassName(true)+"::*"+classStructure.getTreatedClassName(true) +methpm.at(j));
							methsd += (classStructure.getTreatedClassName(true)+methpm.at(j));
						}
						/*else if(methpm.at(j)!="")
						{
							refDef += ("argu.push_back(\""+methpm.at(j)+"\");\n");
							valsd += "\t\t"+(methpm.at(j) + " *_" + CastUtil::lexical_cast<string>(j-1)+" = ("+methpm.at(j)+"*)values.at("+CastUtil::lexical_cast<string>(j-2)+");");
							valsa += "*_" + CastUtil::lexical_cast<string>(j-1);
							if(methpm.at(0)!=classStructure.getFullyQualifiedClassName())
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

					bool tmpltarg = false;
					for(unsigned int j = 0; j < argpm.size(); j++)
					{
						if(tmpltarg && argpm.at(j).find(">")!=string::npos)
						{
							string ttt = argpmtemp.at(argpmtemp.size()-1)+","+argpm.at(j);
							argpmtemp.at(argpmtemp.size()-1) = ttt;
						}
						else if(argpm.at(j).find("<")!=string::npos)
						{
							argpmtemp.push_back(argpm.at(j));
							tmpltarg = true;
						}
						else
						{
							argpmtemp.push_back(argpm.at(j));
						}
					}
					argpm = argpmtemp;
					for(unsigned int j = 0; j < argpm.size(); j++)
					{
						vector<string> argtn;
						string type12 = "";
						if(argpm.at(j).find("*")!=string::npos)
							type12 = "*";
						else if(argpm.at(j).find("&")!=string::npos)
							type12 = "&";
						StringUtil::split(argtn, argpm.at(j), (" "));
						//logger << "testing::::" << argpm.at(j) << argtn.size();
						StringUtil::replaceAll(argtn.at(0)," ","");
						if(meth.find(" operator")==string::npos)
						{
							string fqcn = getFullyQualifiedClassName(argtn.at(0), classStructure.namespaces);
							refDef += ("argu.push_back(\""+fqcn+"\");\n");
						}
						if(argtn.at(0).find("*")!=string::npos || type12=="*")
						{
							StringUtil::replaceAll(argtn.at(0),"*","");
							valsd += "\t\t"+(argtn.at(0) + " *_" + CastUtil::lexical_cast<string>(j)+" = ("+argtn.at(0)+"*)values.at("+CastUtil::lexical_cast<string>(j)+");");
						}
						else if(argtn.at(0).find("&")!=string::npos || type12=="&")
						{
							StringUtil::replaceAll(argtn.at(0),"&","");
							valsd += "\t\t"+(argtn.at(0) + " *_" + CastUtil::lexical_cast<string>(j)+" = ("+argtn.at(0)+"*)values.at("+CastUtil::lexical_cast<string>(j)+");");
						}
						else
							valsd += "\t\t"+(argtn.at(0) + " *_" + CastUtil::lexical_cast<string>(j)+" = ("+argtn.at(0)+"*)values.at("+CastUtil::lexical_cast<string>(j)+");");
						if(type12=="*")
							valsa += "_" + CastUtil::lexical_cast<string>(j);
						else
							valsa += "*_" + CastUtil::lexical_cast<string>(j);
						//if(methpm.at(0)!=classStructure.getFullyQualifiedClassName())
						//{
							typedefs += argtn.at(0);
							typdefName += argtn.at(0);
							string fqcn = getTreatedFullyQualifiedClassName(argtn.at(0), classStructure.namespaces);
							methsd += fqcn + (type12=="*"?"ptr":"");
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
							opers += "\nvoid* " +app+ "operator"+classStructure.getTreatedClassName(true)+"LT(void* instance,vals values)\n{\n\t"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\n";
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
							opers += "\nvoid* " +app+ "operator"+classStructure.getTreatedClassName(true)+"GT(void* instance,vals values)\n{\n\t"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\n";
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
							opers += "\nvoid* " +app+ "operator"+classStructure.getTreatedClassName(true)+"EQ(void* instance,vals values)\n{\n\t"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\n";
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
							opers += "\nvoid* " +app+ "operator"+classStructure.getTreatedClassName(true)+"NE(void* instance,vals values)\n{\n\t"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\n";
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
							opers += "\nvoid* " +app+ "operator"+classStructure.getTreatedClassName(true)+"LE(void* instance,vals values)\n{\n\t"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\n";
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
							opers += "\nvoid* " +app+ "operator"+classStructure.getTreatedClassName(true)+"GT(void* instance,vals values)\n{\n\t"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\n";
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
							opers += "\nvoid* " +app+ "operator"+classStructure.getTreatedClassName(true)+"NT(void* instance,vals values)\n{\n\t"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\n";
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
							opers += "\nvoid* " +app+ "operator"+classStructure.getTreatedClassName(true)+"AD(void* instance,vals values)\n{\n\t"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\n";
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
							opers += "\nvoid* " +app+ "operator"+classStructure.getTreatedClassName(true)+"SU(void* instance,vals values)\n{\n\t"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\n";
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
							opers += "\nvoid* " +app+ "operator"+classStructure.getTreatedClassName(true)+"DI(void* instance,vals values)\n{\n\t"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\n";
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
							opers += "\nvoid* " +app+ "operator"+classStructure.getTreatedClassName(true)+"MU(void* instance,vals values)\n{\n\t"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\n";
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
						StringUtil::replaceFirst(methsd,"<","ts");
						StringUtil::replaceFirst(methsd,"<","ts");
						StringUtil::replaceFirst(methsd,">","te");
						StringUtil::replaceFirst(methsd,",","");
						if(methsd.find("std::")!=string::npos)
						{
							StringUtil::replaceFirst(methsd,"std::","");
						}
						else if(methsd.find("::")!=string::npos)
						{
							StringUtil::replaceFirst(methsd,"::","ns");
						}
						//StringUtil::replaceFirst(methsd,"*","ptr");
						//StringUtil::replaceFirst(methsd,"&","adr");
						if(methpm.at(0)!=classStructure.getTreatedClassName(false))
						{
							typedefs += (") ("+typdefName+");\n");
							methods += "\nvoid* " +app+ "invokeReflectionCIMethodFor"+methsd+"(void* instance,vals values)\n{\n\t"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\n";
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
							methods += "\nvoid* " +app+ "invokeReflectionCICtorFor"+methsd+"(vals values)\n{";
							methods += "\n\t"+classStructure.getFullyQualifiedClassName()+" *_retVal = NULL;\n";
							methods += valsd;
							methods += "\n\t_retVal = (new "+classStructure.getFullyQualifiedClassName()+"("+valsa+"));";
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
				else if(classStructure.pub.at(i).find("~")==string::npos)
				{
					refDef += ("f.clear();\n");
					publf += classStructure.pub.at(i);
					fld = classStructure.pub.at(i);
					StringUtil::replaceFirst(fld,";","");
					vector<string> fldp;
					StringUtil::split(fldp, fld, (" "));
					if(fldp.size()>1)
					{
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
						string fqcn = getFullyQualifiedClassName(fldp.at(0), classStructure.namespaces);
						methods += "\n"+fqcn+" " +app+ "invokeReflectionCIFieldFor"+classStructure.getTreatedClassName(true)+fldp.at(1)+"(void* instance)\n{\n\t"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\n\treturn _obj->"+fldp.at(1)+";\n}\n";
						refDef += ("if(f.getFieldName()!=\"\")\n{\nclassInfo.addField(f);\n}\n");
					}
					else
					{
						//logger << fld << " error" << endl;
					}
				}
				else if(classStructure.pub.at(i).find("~")!=string::npos)
				{
					methods += "\nvoid " +app+ "invokeReflectionCIDtorFor"+classStructure.getTreatedClassName(true)+"(void* instance)\n{";
					methods += "\n\t"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\n\t";
					methods += "_obj->~"+classStructure.getFullyQualifiedClassName()+"();";
					methods += "\n}";
					ddtorisp = true;
				}
			}
			if(!ctorisp)
			{
				refDef += ("ctor.setName(\""+classStructure.getTreatedClassName(true)+"\");\n");
				refDef += ("argu.clear();\n");
				methods += "\nvoid* " +app+ "invokeReflectionCICtorFor"+classStructure.getTreatedClassName(true)+"(vals values)\n{";
				methods += "\n\t"+classStructure.getFullyQualifiedClassName()+" *_retVal = NULL;\n";
				methods += "\n\t_retVal = (new "+classStructure.getFullyQualifiedClassName()+"());";
				methods += "\n\treturn _retVal;";
				methods += "\n}";
				refDef += ("ctor.setArgumentTypes(argu);\n");
				refDef += ("argu.clear();\n");
				refDef += ("classInfo.addConstructor(ctor);\n");
			}
			if(!ddtorisp)
			{
				methods += "\nvoid " +app+ "invokeReflectionCIDtorFor"+classStructure.getTreatedClassName(true)+"(void* instance)\n{";
				methods += "\n\t"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\n\t";
				methods += "_obj->~"+classStructure.getFullyQualifiedClassName()+"();";
				methods += "\n}";
				ddtorisp = true;
			}
		}
		if (classStructure.pri.size() > 0)
		{
			for (unsigned int i = 0; i < classStructure.pri.size(); i++)
			{
				if((tes=classStructure.pri.at(i).find("("))!=string::npos && (tes=classStructure.pri.at(i).find(")"))!=string::npos && classStructure.pri.at(i).find("~")==string::npos)
				{
					//refDef += ("me.clear();\n");
					privm += classStructure.pri.at(i);
					meth = classStructure.pri.at(i);
					StringUtil::replaceFirst(meth,";","");
					StringUtil::replaceFirst(meth,"("," ");
					StringUtil::replaceAll(meth,","," ");
					StringUtil::replaceFirst(meth,")"," ");
					vector<string> methp;
					StringUtil::split(methp, meth, (" "));
					for(unsigned int j = 0; j < methp.size(); j++)
					{
						if(j==0)
						{
							if(methp.at(0)==classStructure.getTreatedClassName(false))
							{
								//refDef += ("me.setReturnType(\"Constructor\");\n");
								//refDef += ("me.setMethodName(\""+methp.at(j)+"\");\n");
							}
							else
							{
								//refDef += ("me.setReturnType(\""+methp.at(j)+"\");\n");
							}
						}
						else if(j==1 && methp.at(0)!=classStructure.getTreatedClassName(false))
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
				else if(classStructure.pri.at(i).find("~")==string::npos)
				{
					refDef += ("f.clear();\n");
					privf += classStructure.pri.at(i);
					fld = classStructure.pri.at(i);
					StringUtil::replaceFirst(fld,";","");
					vector<string> fldp;
					StringUtil::split(fldp, fld, (" "));
					if(fldp.size()>1)
					{
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
						//methods += "\n"+fldp.at(0)+" " +app+ "invokeReflectionCIFieldFor"+classStructure.getTreatedClassName(true)+fldp.at(1)+"(void* instance)\n{\n\t"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\nstruct"
						//+classStructure.getFullyQualifiedClassName()+" *__obj=(struct"+classStructure.getFullyQualifiedClassName()+"*)_obj;\n\treturn __obj->"+fldp.at(1)+";\n}\n";
						refDef += ("if(f.getFieldName()!=\"\")\n{\nclassInfo.addField(f);\n}\n");
					}
					else
					{
						//logger << fld << " error" << endl;
					}
				}
			}
		}
		if (classStructure.pro.size() > 0)
		{
			for (unsigned int i = 0; i < classStructure.pro.size(); i++)
			{
				if((tes=classStructure.pro.at(i).find("("))!=string::npos && (tes=classStructure.pro.at(i).find(")"))!=string::npos && classStructure.pro.at(i).find("~")==string::npos)
				{
					//refDef += ("me.clear();\n");
					protm += classStructure.pro.at(i);
					meth = classStructure.pro.at(i);
					StringUtil::replaceFirst(meth,";","");
					StringUtil::replaceFirst(meth,"("," ");
					StringUtil::replaceAll(meth,","," ");
					StringUtil::replaceFirst(meth,")"," ");
					vector<string> methp;
					StringUtil::split(methp, meth, (" "));
					for(unsigned int j = 0; j < methp.size(); j++)
					{
						if(j==0)
						{
							if(methp.at(0)==classStructure.getTreatedClassName(false))
							{
								//refDef += ("me.setReturnType(\"Constructor\");\n");
								//refDef += ("me.setMethodName(\""+methp.at(j)+"\");\n");
							}
							else
							{
								//refDef += ("me.setReturnType(\""+methp.at(j)+"\");\n");
							}
						}
						else if(j==1 && methp.at(0)!=classStructure.getTreatedClassName(false))
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
				else if(classStructure.pro.at(i).find("~")==string::npos)
				{
					refDef += ("f.clear();\n");
					protf += classStructure.pro.at(i);
					fld = classStructure.pro.at(i);
					StringUtil::replaceFirst(fld,";","");
					vector<string> fldp;
					StringUtil::split(fldp, fld, (" "));
					if(fldp.size()>1)
					{
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
						//methods += "\n"+fldp.at(0)+" " +app+ "invokeReflectionCIFieldFor"+classStructure.getTreatedClassName(true)+fldp.at(1)+"(void* instance)\n{\n\t"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\nstruct"
						//+classStructure.getFullyQualifiedClassName()+" *__obj=(struct"+classStructure.getFullyQualifiedClassName()+"*)_obj;\n\treturn __obj->"+fldp.at(1)+";\n}\n";
						refDef += ("if(f.getFieldName()!=\"\")\n{\nclassInfo.addField(f);\n}\n");
					}
					else
					{
						//logger << fld << " error" << endl;
					}
				}
			}
		}
		//refDef += ("\nclassInfo.setMeths(meths);");
		//refDef += ("\nclassInfo.setMethods(methVec);");
		//refDef += ("\nclassInfo.setFields(fldVec);");
		refDef += "\nreturn classInfo;\n}\n";
		refDef += "\nvoid " +app+ "invokeAdToVecFor"+classStructure.getTreatedClassName(true)+"(void* _vec,void* _instance){"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)_instance;vector<"+classStructure.getFullyQualifiedClassName()+"> *_objvec = (vector<"+classStructure.getFullyQualifiedClassName()+"> *)_vec;_objvec->push_back(*_obj);}";
		refDef += "\nvoid* " +app+ "invokeGetNewVecFor"+classStructure.getTreatedClassName(true)+"(){vector<"+classStructure.getFullyQualifiedClassName()+"> *_objvec = new vector<"+classStructure.getFullyQualifiedClassName()+">;return _objvec;}";
		refDef += "\nint " +app+ "invokeGetVecSizeFor"+classStructure.getTreatedClassName(true)+"(void* _vec){vector<"+classStructure.getFullyQualifiedClassName()+"> *_objvec = (vector<"+classStructure.getFullyQualifiedClassName()+">*)_vec;return (int)_objvec->size();}";
		refDef += "\nvoid* " +app+ "invokeGetVecElementFor"+classStructure.getTreatedClassName(true)+"(void* _vec,int pos){vector<"+classStructure.getFullyQualifiedClassName()+"> *_objvec = (vector<"+classStructure.getFullyQualifiedClassName()+">*)_vec;return &(_objvec->at(pos));}";
		//refDef = (structinf+"};\n"+refDef);
	}
	return refDef;
}

string Reflection::generateSerDefinitionAll(vector<string> all,string &includeRef, bool isBinary,string& objs, string& ajaxret, string& headers, string& typerefs,vector<string> apps)
{
	string ret = "#include \"XmlParser.h\"\n#include \"CastUtil.h\"\n#include \"AMEFResources.h\"\n#include \"Serialize.h\"\n#include \"XMLSerialize.h\"\n";
	includeRef = "#include \"Reflector.h\"\n#include \"vector\"\n#include \"list\"\n#include \"queue\"\n#include \"deque\"\n#include \"set\"\n#include \"DateFormat.h\"\n" ;
	includeRef = "#include \"CastUtil.h\"\n#include \"JSONUtil.h\"\n#include \"sstream\"\n#include \"CastUtil.h\"\n#include <algorithm>\n";
	string typedefs,classes,methods,rert1;
	for (unsigned int var = 0; var < all.size(); ++var)
	{
		rert1 += this->generateSerDefinitions(all.at(var),includeRef,typedefs,classes,methods,isBinary,objs,ajaxret,headers,typerefs,StringUtil::trimCopy(apps.at(var)));
	}
	includeRef += ("extern \"C\"{\n" + classes + typedefs + methods);
	ret += includeRef;
	ret += "}\n";
	includeRef += typedefs;

	//Ajax JSON Serialization part
	string ttem = "Date readDate(JSONElement& obj){DateFormat formt(\"yyyy-mm-dd hh:mi:ss\");\nreturn *formt.parse(obj.getValue());}";
	ttem += "Date* readDateP(JSONElement& obj){DateFormat formt(\"yyyy-mm-dd hh:mi:ss\");\nreturn formt.parse(obj.getValue());}";
	ttem += "\nstring fromDateToJSON(Date d){DateFormat formt(\"yyyy-mm-dd hh:mi:ss\");\nreturn formt.format(d);}";
	ttem += "\nstring fromDateVPToJSON(Date *d){DateFormat formt(\"yyyy-mm-dd hh:mi:ss\");\nreturn formt.format(*d);}";
	ttem += "BinaryData readBinaryData(JSONElement& obj){BinaryData bd;return bd;}";
	ttem += "BinaryData* readBinaryDataP(JSONElement& obj){return new BinaryData;}";
	ttem += "\nstring fromBinaryDataToJSON(BinaryData d){return \"\";}";
	ttem += "\nstring fromBinaryDataVPToJSON(BinaryData *d){return \"\";}";
	ajaxret = ttem + typerefs + ajaxret;
	ajaxret ="#include \"AfcInclude.h\"\n\nextern \"C\"\n{\n" + ajaxret;
	headers += "#include \"CastUtil.h\"\n#include \"JSONUtil.h\"\n#include \"sstream\"\n#include <algorithm>\n#include \"JSONSerialize.h\"\n";
	headers += "#include \"set\"\n#include \"list\"\n#include \"queue\"\n#include \"deque\"\n#include \"DateFormat.h\"\n";

	return ret;
}

string Reflection::generateSerDefinitions(string includeDir,string &includesDefs,string &typedefs,string &classes,string &methods,bool isBinary,
		string& objs, string &ajaxret, string& headers, string& typerefs,string app)
{
	vector<string> includes = list(includeDir);
	string ret;
	map<string, ClassStructure> allclsmap;
	for (unsigned int var = 0; var < includes.size(); ++var)
	{
		map<string, ClassStructure> clsmap = getClassStructures(includes.at(var));
		allclsmap.insert(clsmap.begin(), clsmap.end());
	}
	ret = generateSerDefinition(allclsmap,includesDefs,typedefs,classes,methods,app);
	ret += generateSerDefinitionBinary(allclsmap,includesDefs,typedefs,classes,methods,app);

	map<string, ClassStructure>::iterator it;
	for (it=allclsmap.begin();it!=allclsmap.end();++it)
	{
		vector<string> pinfo;
		bool isOpForSet = false;
		vector<string> minfo = getAfcObjectData(it->second,false,pinfo,isOpForSet);
		pinfo.clear();
		vector<string> info = getAfcObjectData(it->second,true,pinfo,isOpForSet);
		ajaxret += AfcUtil::generateJsObjects(info,it->second.classN,headers,includeDir,objs,pinfo,isOpForSet,typerefs,minfo,app,getClassPath(it->second.getTreatedClassName(true)));
	}

	/*for (unsigned int var = 0; var < includes.size(); ++var)
	{
		//logger << "\ngenerating Ser for file" << includes.at(var) << "\n" << flush;
		if(invalidcls.find(includes.at(var))==invalidcls.end())
		{

			vector<string> pinfo;
			bool isOpForSet = false;
			vector<string> minfo = getAfcObjectData(includes.at(var),false,pinfo,isOpForSet);
			pinfo.clear();
			vector<string> info = getAfcObjectData(includes.at(var),true,pinfo,isOpForSet);
			ajaxret += AfcUtil::generateJsObjects(info,this->classN,headers,includeDir,objs,pinfo,isOpForSet,typerefs,minfo,app,getClassPath(getTreatedClassName(true)));
		}
		//logger << "\ndone generating Ser for file" << includes.at(var) << "\n" << flush;
	}*/
	return ret;
}

string Reflection::generateSerDefinition(map<string, ClassStructure> allclsmap,string &includesDefs,string &typedefs,string &classes,string &methods,string app)
{
	string refDef;
	/*string opers;
	if (!generateClassInfo(className))
	{
		return refDef;
	}*/


	map<string, ClassStructure>::iterator it;
	for (it=allclsmap.begin();it!=allclsmap.end();++it)
	{
		ClassStructure classStructure = it->second;
		includesDefs += "#include \"" + getClassPath(it->second.getTreatedClassName(true)) + "\"\n";
		classStructure.prosetser = false;
		//classes += "\tif(className==\""+classStructure.getTreatedClassName(true)+"\")\n\t\tobjXml = get"+classStructure.getTreatedClassName(true)+"XML(t);\n";
		//refDef += "\tif(className==\""+classStructure.getTreatedClassName(true)+"\")\n\t\tt = getObject"+classStructure.getTreatedClassName(true)+"(objXml);\n";

		//string structinf = "\nstruct struct"+classStructure.getTreatedClassName(true)+"{\n";
		classes += "\nstring " +app+ "serialize" + classStructure.getTreatedClassName(true) + "(void* obje);\nvoid* " +app+ "unSerialize" + classStructure.getTreatedClassName(true) + "(string objXml);";
		methods += "\nstring " +app+ "serialize" + classStructure.getTreatedClassName(true) + "(void* obje)\n{\n"+classStructure.getFullyQualifiedClassName()+" *__obj=("+classStructure.getFullyQualifiedClassName()+"*)obje;\n";
		methods += "string objxml = \"<"+classStructure.getTreatedClassName(false)+" namespace=\""+classStructure.nmSpc+"\">\";\n";
		typedefs += "\nvoid* " +app+ "unSerialize" + classStructure.getTreatedClassName(true) + "(string objXml)\n{\n";
		typedefs += classStructure.getFullyQualifiedClassName()+" *__obj=new "+classStructure.getFullyQualifiedClassName()+";\nXmlParser parser(\"Parser\");\nElement root = parser.getDocument(objXml).getRootElement();\nif(root.getTagName()==\"\" || root.getChildElements().size()==0 || root.getAttribute(\"namespace\")!=\""+classStructure.nmSpc+"\")\nreturn NULL;\n";
		typedefs += "for(unsigned int i=0;i<root.getChildElements().size();i++)\n{\n";
		typedefs += "string nam=root.getChildElements().at(i).getTagName();\n";

		string publf, privf, protf ,publm, privm, protm;
		string meth,fld;
		size_t tes;
		vector<string> fldnames;
		if (classStructure.pri.size() > 0)
		{
			for (unsigned int i = 0; i < classStructure.pri.size(); i++)
			{
				if(((tes=classStructure.pri.at(i).find("("))==string::npos && (tes=classStructure.pri.at(i).find(")"))==string::npos && classStructure.pri.at(i).find("~")==string::npos))
				{
					fld = classStructure.pri.at(i);
					StringUtil::replaceFirst(fld,";","");
					vector<string> fldp;
					StringUtil::split(fldp, fld, (" "));
					if(fldp.size()==2)
					{
						string nam = fldp.at(1);
						fldnames.push_back(fldp.at(0));
						fldnames.push_back(nam);
					}
				}
			}
		}
		if (classStructure.pro.size() > 0)
		{
			for (unsigned int i = 0; i < classStructure.pro.size(); i++)
			{
				if(((tes=classStructure.pro.at(i).find("("))==string::npos && (tes=classStructure.pro.at(i).find(")"))==string::npos && classStructure.pro.at(i).find("~")==string::npos))
				{
					fld = classStructure.pro.at(i);
					StringUtil::replaceFirst(fld,";","");
					vector<string> fldp;
					StringUtil::split(fldp, fld, (" "));
					if(fldp.size()==2)
					{
						string nam = fldp.at(1);
						fldnames.push_back(fldp.at(0));
						fldnames.push_back(nam);
					}
				}
			}
		}
		if (classStructure.pub.size() > 0)
		{
			for (unsigned int i = 0; i < classStructure.pub.size(); i++)
			{
				if(((tes=classStructure.pub.at(i).find("("))==string::npos && (tes=classStructure.pub.at(i).find(")"))==string::npos && classStructure.pub.at(i).find("~")==string::npos))
				{
					fld = classStructure.pub.at(i);
					RegexUtil::replace(fld, "[\t]+", " ");
					RegexUtil::replace(fld, "[ ]+", " ");
					//RegexUtil::replace(fld, "[ ?, ?]+", ",");
					StringUtil::replaceFirst(fld,";","");
					bool ptr = false;
					if(fld.find("*")!=string::npos)
					{
						ptr = true;
						StringUtil::replaceFirst(fld,"*","");
					}
					vector<string> fldp;
					StringUtil::split(fldp, fld, (" "));
					if(fldp.size()==2)
					{
						string nam = fldp.at(1);
						if(fldp.at(0)=="int" || fldp.at(0)=="long" || fldp.at(0)=="short" || fldp.at(0)=="float" || fldp.at(0)=="string" || fldp.at(0)=="std::string" || fldp.at(0)=="double" || fldp.at(0)=="bool" || fldp.at(0)=="unsigned int" || fldp.at(0)=="unsigned long" || fldp.at(0)=="unsigned short" || fldp.at(0)=="long long" || fldp.at(0)=="unsigned long long")
						{
							if(!ptr)
							{
								methods += ("objxml += \"<"+fldp.at(1)+" type=\\\""+fldp.at(0)+"\\\">\"+CastUtil::lexical_cast<string>(__obj->"+fldp.at(1)+")");
								string cam = AfcUtil::camelCased(fldp.at(1));
								methods += ("+\"</"+nam+">\";\n");
								typedefs += "if(nam==\""+fldp.at(1)+"\")\n__obj->"+fldp.at(1)+" = CastUtil::lexical_cast<"+fldp.at(0)+">(root.getChildElements().at(i).getText());\n";
							}
							else
							{
								methods += ("if(__obj->"+fldp.at(1)+"!=NULL)objxml += \"<"+fldp.at(1)+" type=\\\""+fldp.at(0)+"\\\">\"+CastUtil::lexical_cast<string>(*__obj->"+fldp.at(1)+")");
								string cam = AfcUtil::camelCased(fldp.at(1));
								methods += ("+\"</"+nam+">\";\n");
								typedefs += "if(nam==\""+fldp.at(1)+"\")\n__obj->"+fldp.at(1)+" = new "+fldp.at(0)+"(CastUtil::lexical_cast<"+fldp.at(0)+">(root.getChildElements().at(i).getText()));\n";
							}
						}
						else if(fldp.at(0)=="Date")
						{
							if(!ptr)
							{
								methods += ("DateFormat formt"+fldp.at(1)+"(\"yyyy-mm-dd hh:mi:ss\");\nobjxml += \"<"+fldp.at(1)+" type=\\\""+fldp.at(0)+"\\\">\"+formt"+fldp.at(1)+".format(__obj->"+fldp.at(1)+")");
								string cam = AfcUtil::camelCased(fldp.at(1));
								methods += ("+\"</"+nam+">\";\n");
								typedefs += "if(nam==\""+fldp.at(1)+"\")\n{\nDateFormat formt"+fldp.at(1)+"(\"yyyy-mm-dd hh:mi:ss\");\n__obj->"+fldp.at(1)+" = *(formt"+fldp.at(1)+".parse(root.getChildElements().at(i).getText()));\n}\n";
							}
							else
							{
								methods += ("if(__obj->"+fldp.at(1)+"!=NULL){DateFormat formt"+fldp.at(1)+"(\"yyyy-mm-dd hh:mi:ss\");\nobjxml += \"<"+fldp.at(1)+" type=\\\""+fldp.at(0)+"\\\">\"+formt"+fldp.at(1)+".format(*__obj->"+fldp.at(1)+")");
								string cam = AfcUtil::camelCased(fldp.at(1));
								methods += ("+\"</"+nam+">\";}\n");
								typedefs += "if(nam==\""+fldp.at(1)+"\")\n{\nDateFormat formt"+fldp.at(1)+"(\"yyyy-mm-dd hh:mi:ss\");\n__obj->"+fldp.at(1)+" = (formt"+fldp.at(1)+".parse(root.getChildElements().at(i).getText()));\n}\n";
							}
						}
						else if(fldp.at(0)=="BinaryData")
						{
							if(!ptr)
							{
								methods += ("objxml += \"<"+fldp.at(1)+" type=\\\""+fldp.at(0)+"\\\">\"+BinaryData::serilaize(__obj->"+fldp.at(1)+")");
								string cam = AfcUtil::camelCased(fldp.at(1));
								methods += ("+\"</"+nam+">\";\n");
								typedefs += "if(nam==\""+fldp.at(1)+"\")\n__obj->"+fldp.at(1)+" = *(BinaryData::unSerilaize(root.getChildElements().at(i).getText()));\n";
							}
							else
							{
								methods += ("if(__obj->"+fldp.at(1)+"!=NULL)objxml += \"<"+fldp.at(1)+" type=\\\""+fldp.at(0)+"\\\">\"+BinaryData::serilaize(*__obj->"+fldp.at(1)+")");
								string cam = AfcUtil::camelCased(fldp.at(1));
								methods += ("+\"</"+nam+">\";\n");
								typedefs += "if(nam==\""+fldp.at(1)+"\")\n__obj->"+fldp.at(1)+" = (BinaryData::unSerilaize(root.getChildElements().at(i).getText()));\n";
							}
						}
						else if(fldp.at(0).find("vector")!=string::npos || fldp.at(0).find("queue")!=string::npos || fldp.at(0).find("deque")!=string::npos || fldp.at(0).find("set")!=string::npos || fldp.at(0).find("list")!=string::npos)
						{
							string stlcnt = fldp.at(0);
							string stltyp = fldp.at(0);
							string contType;
							StringUtil::replaceFirst(stltyp,"std::","");
							StringUtil::replaceFirst(stltyp,"<","::");
							StringUtil::replaceFirst(stltyp,">","");
							StringUtil::replaceFirst(stltyp," ","");
							string stlcnttyp = "";
							if(fldp.at(0).find("vector")!=string::npos)
							{
								contType = "std::vector<";
								stlcnttyp = "Vec";
							}
							else if(fldp.at(0).find("queue")!=string::npos)
							{
								contType = "std::queue<";
								stlcnttyp = "Q";
							}
							else if(fldp.at(0).find("deque")!=string::npos)
							{
								contType = "std::deque<";
								stlcnttyp = "Dq";
							}
							else if(fldp.at(0).find("list")!=string::npos)
							{
								contType = "std::list<";
								stlcnttyp = "Lis";
							}
							else if(fldp.at(0).find("multiset")!=string::npos)
							{
								contType = "std::multiset<";
								stlcnttyp = "MulSet";
							}
							else
							{
								contType = "std::set<";
								stlcnttyp = "Set";
							}
							StringUtil::replaceFirst(stlcnt,"std::","");
							StringUtil::replaceFirst(stlcnt,"vector","");
							StringUtil::replaceFirst(stlcnt,"queue","");
							StringUtil::replaceFirst(stlcnt,"deque","");
							StringUtil::replaceFirst(stlcnt,"multiset","");
							StringUtil::replaceFirst(stlcnt,"set","");
							StringUtil::replaceFirst(stlcnt,"list","");
							StringUtil::replaceFirst(stlcnt,"<","");
							StringUtil::replaceFirst(stlcnt,">","");
							StringUtil::replaceFirst(stlcnt," ","");

							contType += stlcnt + ",";

							if(!ptr)
							{
								methods += (fldp.at(0)+" __temp_obj_ser"+stlcnt+stlcnttyp+classStructure.classN+fldp.at(1)+" = __obj->"+fldp.at(1)+";\n");
								if(stlcnt=="int" || stlcnt=="long" || stlcnt=="short" || stlcnt=="float" || stlcnt=="string" || stlcnt=="std::string" || stlcnt=="double" || stlcnt=="bool" || stlcnt=="unsigned int" || stlcnt=="unsigned long" || stlcnt=="unsigned short" || stlcnt=="long long" || stlcnt=="unsigned long long")
									methods += ("objxml += \"<"+fldp.at(1)+" type=\\\""+stltyp+"\\\">\"+XMLSerialize::serialize<"+fldp.at(0)+" >(__temp_obj_ser"+stlcnt+stlcnttyp+classStructure.classN+fldp.at(1)+",\""+app+"\")");
								else
									methods += ("objxml += \"<"+fldp.at(1)+" type=\\\""+stltyp+"\\\">\"+"+app+"serialize"+stlcnt+stlcnttyp+"(&__temp_obj_ser"+stlcnt+stlcnttyp+classStructure.classN+fldp.at(1)+")");
								string cam = AfcUtil::camelCased(fldp.at(1));
								methods += ("+\"</"+nam+">\";\n");
								if(stlcnt=="int" || stlcnt=="long" || stlcnt=="short" || stlcnt=="float" || stlcnt=="string" || stlcnt=="std::string" || stlcnt=="double" || stlcnt=="bool" || stlcnt=="unsigned int" || stlcnt=="unsigned long" || stlcnt=="unsigned short" || stlcnt=="long long" || stlcnt=="unsigned long long")
									typedefs += "if(nam==\""+fldp.at(1)+"\")\n__obj->"+fldp.at(1)+" = XMLSerialize::unserialize<"+fldp.at(0)+" >(root.getChildElements().at(i).renderChildren(),\""+app+"\");\n";
								else
									typedefs += "if(nam==\""+fldp.at(1)+"\")\n__obj->"+fldp.at(1)+" = *("+fldp.at(0)+"*)"+app+"unSerialize"+stlcnt+stlcnttyp+"(root.getChildElements().at(i).renderChildren());\n";
							}
							else
							{
								methods += (fldp.at(0)+"* __temp_obj_ser"+stlcnt+stlcnttyp+classStructure.classN+fldp.at(1)+" = __obj->"+fldp.at(1)+";\n");
								if(stlcnt=="int" || stlcnt=="long" || stlcnt=="short" || stlcnt=="float" || stlcnt=="string" || stlcnt=="std::string" || stlcnt=="double" || stlcnt=="bool" || stlcnt=="unsigned int" || stlcnt=="unsigned long" || stlcnt=="unsigned short" || stlcnt=="long long" || stlcnt=="unsigned long long")
									methods += ("if(__obj->"+fldp.at(1)+"!=NULL)objxml += \"<"+fldp.at(1)+" type=\\\""+stltyp+"\\\">\"+XMLSerialize::serialize<"+fldp.at(0)+" >(*__temp_obj_ser"+stlcnt+stlcnttyp+classStructure.classN+fldp.at(1)+",\""+app+"\")");
								else
									methods += ("if(__obj->"+fldp.at(1)+"!=NULL)objxml += \"<"+fldp.at(1)+" type=\\\""+stltyp+"\\\">\"+"+app+"serialize"+stlcnt+stlcnttyp+"(__temp_obj_ser"+stlcnt+stlcnttyp+classStructure.classN+fldp.at(1)+")");
								string cam = AfcUtil::camelCased(fldp.at(1));
								methods += ("+\"</"+nam+">\";\n");
								if(stlcnt=="int" || stlcnt=="long" || stlcnt=="short" || stlcnt=="float" || stlcnt=="string" || stlcnt=="std::string" || stlcnt=="double" || stlcnt=="bool" || stlcnt=="unsigned int" || stlcnt=="unsigned long" || stlcnt=="unsigned short" || stlcnt=="long long" || stlcnt=="unsigned long long")
									typedefs += "if(nam==\""+fldp.at(1)+"\")\n__obj->"+fldp.at(1)+" = ("+fldp.at(0)+"*)XMLSerialize::unSerializeUnknown(root.getChildElements().at(i).renderChildren(),\""+contType+"\",\""+app+"\");\n";
								else
									typedefs += "if(nam==\""+fldp.at(1)+"\")\n__obj->"+fldp.at(1)+" = ("+fldp.at(0)+"*)"+app+"unSerialize"+stlcnt+stlcnttyp+"(root.getChildElements().at(i).renderChildren());\n";
							}
						}
						else
						{
							if(!ptr)
							{
								methods += (fldp.at(0)+" __temp_obj_ser"+fldp.at(1)+" = __obj->"+fldp.at(1)+";\n");
								methods += ("objxml += \"<"+fldp.at(1)+" type=\\\""+fldp.at(0)+"\\\">\"+"+app+"serialize"+fldp.at(0)+"(&__temp_obj_ser"+fldp.at(1)+")");
								string cam = AfcUtil::camelCased(fldp.at(1));
								methods += ("+\"</"+nam+">\";\n");
								typedefs += "if(nam==\""+fldp.at(1)+"\")\n__obj->"+fldp.at(1)+" = *("+fldp.at(0)+"*)"+app+"unSerialize"+fldp.at(0)+"(root.getChildElements().at(i).renderChildren());\n";
							}
							else
							{
								methods += (fldp.at(0)+"* __temp_obj_ser"+fldp.at(1)+" = __obj->"+fldp.at(1)+";\n");
								methods += ("if(__obj->"+fldp.at(1)+"!=NULL)objxml += \"<"+fldp.at(1)+" type=\\\""+fldp.at(0)+"\\\">\"+"+app+"serialize"+fldp.at(0)+"(__temp_obj_ser"+fldp.at(1)+")");
								string cam = AfcUtil::camelCased(fldp.at(1));
								methods += ("+\"</"+nam+">\";\n");
								typedefs += "if(nam==\""+fldp.at(1)+"\")\n__obj->"+fldp.at(1)+" = ("+fldp.at(0)+"*)"+app+"unSerialize"+fldp.at(0)+"(root.getChildElements().at(i).renderChildren());\n";
							}
						}
						//structinf += (fldp.at(0)+" "+fldp.at(1)+";\n");
					}
				}
				if((tes=classStructure.pub.at(i).find("("))!=string::npos && (tes=classStructure.pub.at(i).find(")"))!=string::npos && classStructure.pub.at(i).find("~")==string::npos
						&& fldnames.size()>0)
				{
					meth = classStructure.pub.at(i);
					StringUtil::replaceFirst(meth,";","");
					RegexUtil::replace(meth, "[\t]+", " ");
					RegexUtil::replace(meth, "[ ]+", " ");
					//RegexUtil::replace(meth, "[ ?, ?]+", ",");
					bool ptr = false;
					if(meth.find("*")!=string::npos)
					{
						ptr = true;
						StringUtil::replaceFirst(meth,"*","");
					}

					string argts = meth.substr(meth.find("("),meth.find(")")-meth.find("("));
					StringUtil::replaceFirst(argts,"(","");
					StringUtil::replaceAll(argts,")","");
					meth = meth.substr(0,meth.find("("));

					if(meth.find("operator")!=string::npos)
					{
						if(meth.find("<")!=string::npos)
						{
							classStructure.prosetser = true;
						}
						else if(meth.find(">")!=string::npos)
						{
							classStructure.prosetser = true;
						}
					}
					else
					{
						//StringUtil::replaceFirst(meth,")"," ");
						vector<string> methp,methpm,argp,argpm;
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

						if(methpm.at(0)!=classStructure.getTreatedClassName(false))
						{
							for(unsigned int k = 0; k < fldnames.size(); k=k+2)
							{
								string cam = AfcUtil::camelCased(fldnames.at(k+1));
								if("set"+cam==methpm.at(1) && argpm.size()==1 && argpm.at(0)==fldnames.at(k) && methpm.at(0)=="void")
								{
									if(argpm.at(0)=="int" || argpm.at(0)=="long" || argpm.at(0)=="short" || argpm.at(0)=="float" || argpm.at(0)=="string" || argpm.at(0)=="std::string" || argpm.at(0)=="double" || argpm.at(0)=="bool" || argpm.at(0)=="unsigned int" || argpm.at(0)=="unsigned long" || argpm.at(0)=="unsigned short" || argpm.at(0)=="long long" || argpm.at(0)=="unsigned long long")
									{
										if(!ptr)
											typedefs += "if(nam==\""+fldnames.at(k+1)+"\")\n__obj->"+methpm.at(1)+"(CastUtil::lexical_cast<"+argpm.at(0)+">(root.getChildElements().at(i).getText()));\n";
										else
											typedefs += "if(nam==\""+fldnames.at(k+1)+"\")\n__obj->"+methpm.at(1)+"(new "+argpm.at(0)+"(CastUtil::lexical_cast<"+argpm.at(0)+">(root.getChildElements().at(i).getText())));\n";
									}
									else if(argpm.at(0)=="Date")
									{
										if(!ptr)
											typedefs += "if(nam==\""+fldnames.at(k+1)+"\")\n{\nDateFormat formt"+cam+"(\"yyyy-mm-dd hh:mi:ss\");\n__obj->"+methpm.at(1)+"(*(formt"+cam+".parse(root.getChildElements().at(i).getText())));\n}\n";
										else
											typedefs += "if(nam==\""+fldnames.at(k+1)+"\")\n{\nDateFormat formt"+cam+"(\"yyyy-mm-dd hh:mi:ss\");\n__obj->"+methpm.at(1)+"((formt"+cam+".parse(root.getChildElements().at(i).getText())));\n}\n";
									}
									else if(argpm.at(0)=="BinaryData")
									{
										if(!ptr)
											typedefs += "if(nam==\""+fldnames.at(k+1)+"\")\n__obj->"+methpm.at(1)+"(*(BinaryData::unSerilaize(root.getChildElements().at(i).getText())));\n";
										else
											typedefs += "if(nam==\""+fldnames.at(k+1)+"\")\n__obj->"+methpm.at(1)+"((BinaryData::unSerilaize(root.getChildElements().at(i).getText())));\n";
									}
									else if(argpm.at(0).find("vector")!=string::npos || argpm.at(0).find("queue")!=string::npos ||
											argpm.at(0).find("deque")!=string::npos || argpm.at(0).find("set")!=string::npos ||
											argpm.at(0).find("list")!=string::npos || argpm.at(0).find("multiset")!=string::npos)
									{
										string stlcnt = argpm.at(0);
										string stltyp = argpm.at(0);
										string contType;
										StringUtil::replaceFirst(stltyp,"std::","");
										StringUtil::replaceFirst(stltyp,"<","::");
										StringUtil::replaceFirst(stltyp,">","");
										StringUtil::replaceFirst(stltyp," ","");
										string stlcnttyp = "";
										if(argpm.at(0).find("vector")!=string::npos)
										{
											contType = "std::vector<";
											stlcnttyp = "Vec";
										}
										else if(argpm.at(0).find("queue")!=string::npos)
										{
											contType = "std::queue<";
											stlcnttyp = "Q";
										}
										else if(argpm.at(0).find("deque")!=string::npos)
										{
											contType = "std::deque<";
											stlcnttyp = "Dq";
										}
										else if(argpm.at(0).find("list")!=string::npos)
										{
											contType = "std::list<";
											stlcnttyp = "Lis";
										}
										else if(argpm.at(0).find("multiset")!=string::npos)
										{
											contType = "std::multiset<";
											stlcnttyp = "MulSet";
										}
										else
										{
											contType = "std::set<";
											stlcnttyp = "Set";
										}
										StringUtil::replaceFirst(stlcnt,"std::","");
										StringUtil::replaceFirst(stlcnt,"vector","");
										StringUtil::replaceFirst(stlcnt,"queue","");
										StringUtil::replaceFirst(stlcnt,"deque","");
										StringUtil::replaceFirst(stlcnt,"multiset","");
										StringUtil::replaceFirst(stlcnt,"set","");
										StringUtil::replaceFirst(stlcnt,"list","");
										StringUtil::replaceFirst(stlcnt,"<","");
										StringUtil::replaceFirst(stlcnt,">","");
										StringUtil::replaceFirst(stlcnt," ","");

										contType += stlcnt + ",";

										if(!ptr)
										{
											typedefs += "if(nam==\""+fldnames.at(k+1)+"\"){";
											if(stlcnt=="int" || stlcnt=="short" || stlcnt=="long" || stlcnt=="float" || stlcnt=="string" ||
													stlcnt=="std::string" || stlcnt=="double" || stlcnt=="bool")
												typedefs += "\n__obj->set"+cam+"(XMLSerialize::unserialize<"+argpm.at(0)+" >(root.getChildElements().at(i).renderChildren(),\""+app+"\"));\n";
											else
												typedefs += "\n__obj->set"+cam+"(*("+argpm.at(0)+"*)"+app+"unSerialize"+stlcnt+stlcnttyp+"(root.getChildElements().at(i).renderChildren()));\n";
											typedefs += "\n}\n";
										}
										else
										{
											typedefs += "if(nam==\""+fldnames.at(k+1)+"\"){";
											if(stlcnt=="int" || stlcnt=="short" || stlcnt=="long" || stlcnt=="float" || stlcnt=="string" ||
													stlcnt=="std::string" || stlcnt=="double" || stlcnt=="bool")
												typedefs += "\n__obj->set"+cam+"(("+argpm.at(0)+"*)XMLSerialize::unSerializeUnknown(root.getChildElements().at(i).renderChildren(),\""+contType+"\",\""+app+"\"));\n";
											else
												typedefs += "\n__obj->set"+cam+"(("+argpm.at(0)+"*)"+app+"unSerialize"+stlcnt+stlcnttyp+"(root.getChildElements().at(i).renderChildren()));\n";
											typedefs += "\n}\n";
										}
									}
									else
									{
										if(!ptr)
											typedefs += "if(nam==\""+fldnames.at(k+1)+"\")\n__obj->"+methpm.at(1)+"(*("+argpm.at(0)+"*)"+app+"unSerialize"+argpm.at(0)+"(root.getChildElements().at(i).renderChildren()));\n";
										else
											typedefs += "if(nam==\""+fldnames.at(k+1)+"\")\n__obj->"+methpm.at(1)+"(("+argpm.at(0)+"*)"+app+"unSerialize"+argpm.at(0)+"(root.getChildElements().at(i).renderChildren()));\n";
									}
								}
								else if("get"+cam==methpm.at(1) && argpm.size()==0 && methpm.at(0)==fldnames.at(k))
								{
									if(methpm.at(0)=="int" || methpm.at(0)=="long" || methpm.at(0)=="short" || methpm.at(0)=="float" || methpm.at(0)=="string" || methpm.at(0)=="std::string" || methpm.at(0)=="double" || methpm.at(0)=="bool" || methpm.at(0)=="unsigned int" || methpm.at(0)=="unsigned long" || methpm.at(0)=="unsigned short" || methpm.at(0)=="long long" || methpm.at(0)=="unsigned long long")
									{
										if(!ptr)
										{
											methods += ("objxml += \"<"+fldnames.at(k+1)+" type=\\\""+methpm.at(0)+"\\\">\"+CastUtil::lexical_cast<string>(__obj->"+methpm.at(1)+"())");
											methods += ("+\"</"+fldnames.at(k+1)+">\";\n");
											if(methsall[classStructure.getTreatedClassName(true)+"get"+cam+methpm.at(0)])typedefs += "if(nam==\""+fldnames.at(k+1)+"\")\n__obj->set"+cam+"(CastUtil::lexical_cast<"+methpm.at(0)+">(root.getChildElements().at(i).getText()));\n";
										}
										else
										{
											methods += ("if(__obj->"+methpm.at(1)+"()!=NULL)objxml += \"<"+fldnames.at(k+1)+" type=\\\""+methpm.at(0)+"\\\">\"+CastUtil::lexical_cast<string>(*__obj->"+methpm.at(1)+"())");
											methods += ("+\"</"+fldnames.at(k+1)+">\";\n");
											if(methsall[classStructure.getTreatedClassName(true)+"get"+cam+methpm.at(0)])typedefs += "if(nam==\""+fldnames.at(k+1)+"\")\n__obj->set"+cam+"(new "+methpm.at(0)+"(CastUtil::lexical_cast<"+methpm.at(0)+">(root.getChildElements().at(i).getText())));\n";
										}
									}
									else if(methpm.at(0)=="Date")
									{
										if(!ptr)
										{
											methods += ("DateFormat formt"+fldnames.at(k+1)+"(\"yyyy-mm-dd hh:mi:ss\");\nobjxml += \"<"+fldnames.at(k+1)+" type=\\\""+methpm.at(0)+"\\\">\"+formt"+fldnames.at(k+1)+".format(__obj->"+methpm.at(1)+"())");
											methods += ("+\"</"+fldnames.at(k+1)+">\";\n");
											if(methsall[classStructure.getTreatedClassName(true)+"get"+cam+methpm.at(0)])typedefs += "if(nam==\""+fldnames.at(k+1)+"\")\n{\nDateFormat formt"+fldnames.at(k+1)+"(\"yyyy-mm-dd hh:mi:ss\");\n__obj->set"+cam+"(*(formt"+fldnames.at(k+1)+".parse(root.getChildElements().at(i).getText())));\n}\n";
										}
										else
										{
											methods += ("if(__obj->"+methpm.at(1)+"()!=NULL){DateFormat formt"+fldnames.at(k+1)+"(\"yyyy-mm-dd hh:mi:ss\");\nobjxml += \"<"+fldnames.at(k+1)+" type=\\\""+methpm.at(0)+"\\\">\"+formt"+fldnames.at(k+1)+".format(*__obj->"+methpm.at(1)+"())");
											methods += ("+\"</"+fldnames.at(k+1)+">\";}\n");
											if(methsall[classStructure.getTreatedClassName(true)+"get"+cam+methpm.at(0)])typedefs += "if(nam==\""+fldnames.at(k+1)+"\")\n{\nDateFormat formt"+fldnames.at(k+1)+"(\"yyyy-mm-dd hh:mi:ss\");\n__obj->set"+cam+"((formt"+fldnames.at(k+1)+".parse(root.getChildElements().at(i).getText())));\n}\n";
										}
									}
									else if(methpm.at(0)=="BinaryData")
									{
										if(!ptr)
										{
											methods += ("objxml += \"<"+fldnames.at(k+1)+" type=\\\""+methpm.at(0)+"\\\">\"+BinaryData::serilaize(__obj->"+methpm.at(1)+"())");
											methods += ("+\"</"+fldnames.at(k+1)+">\";\n");
											if(methsall[classStructure.getTreatedClassName(true)+"get"+cam+methpm.at(0)])typedefs += "if(nam==\""+fldnames.at(k+1)+"\")\n__obj->set"+cam+"(*(BinaryData::unSerilaize(root.getChildElements().at(i).getText())));\n";
										}
										else
										{
											methods += ("if(__obj->"+methpm.at(1)+"()!=NULL)objxml += \"<"+fldnames.at(k+1)+" type=\\\""+methpm.at(0)+"\\\">\"+BinaryData::serilaize(*__obj->"+methpm.at(1)+"())");
											methods += ("+\"</"+fldnames.at(k+1)+">\";\n");
											if(methsall[classStructure.getTreatedClassName(true)+"get"+cam+methpm.at(0)])typedefs += "if(nam==\""+fldnames.at(k+1)+"\")\n__obj->set"+cam+"((BinaryData::unSerilaize(root.getChildElements().at(i).getText())));\n";
										}
									}
									else if(methpm.at(0).find("vector")!=string::npos || methpm.at(0).find("queue")!=string::npos ||
											methpm.at(0).find("deque")!=string::npos || methpm.at(0).find("set")!=string::npos ||
											methpm.at(0).find("list")!=string::npos || methpm.at(0).find("multiset")!=string::npos)
									{
										string stlcnt = methpm.at(0);
										string stltyp = methpm.at(0);
										StringUtil::replaceFirst(stltyp,"std::","");
										StringUtil::replaceFirst(stltyp,"<","::");
										StringUtil::replaceFirst(stltyp,">","");
										StringUtil::replaceFirst(stltyp," ","");
										string stlcnttyp = "";
										if(methpm.at(0).find("vector")!=string::npos)
											stlcnttyp = "Vec";
										else if(methpm.at(0).find("queue")!=string::npos)
											stlcnttyp = "Q";
										else if(methpm.at(0).find("deque")!=string::npos)
											stlcnttyp = "Dq";
										else if(methpm.at(0).find("list")!=string::npos)
											stlcnttyp = "Lis";
										else if(methpm.at(0).find("multiset")!=string::npos)
											stlcnttyp = "MulSet";
										else
											stlcnttyp = "Set";
										StringUtil::replaceFirst(stlcnt,"std::","");
										StringUtil::replaceFirst(stlcnt,"vector","");
										StringUtil::replaceFirst(stlcnt,"queue","");
										StringUtil::replaceFirst(stlcnt,"deque","");
										StringUtil::replaceFirst(stlcnt,"multiset","");
										StringUtil::replaceFirst(stlcnt,"set","");
										StringUtil::replaceFirst(stlcnt,"list","");
										StringUtil::replaceFirst(stlcnt,"<","");
										StringUtil::replaceFirst(stlcnt,">","");
										StringUtil::replaceFirst(stlcnt," ","");

										if(!ptr)
										{
											methods += (methpm.at(0)+" __temp_obj_ser"+stlcnt+stlcnttyp+classStructure.classN+methpm.at(1)+" = __obj->"+methpm.at(1)+"();\n");
											if(stlcnt=="int" || stlcnt=="short" || stlcnt=="long" || stlcnt=="float" ||
													stlcnt=="string" || stlcnt=="std::string" || stlcnt=="double" || stlcnt=="bool")
												methods += ("objxml += \"<"+fldnames.at(k+1)+" type=\\\""+stltyp+"\\\">\"+XMLSerialize::serialize<"+methpm.at(0)+" >(__temp_obj_ser"+stlcnt+stlcnttyp+classStructure.classN+methpm.at(1)+",\""+app+"\")");
											else
												methods += ("objxml += \"<"+fldnames.at(k+1)+" type=\\\""+stltyp+"\\\">\"+"+app+"serialize"+stlcnt+stlcnttyp+"(&__temp_obj_ser"+stlcnt+stlcnttyp+classStructure.classN+methpm.at(1)+")");
											//string cam = AfcUtil::camelCased(methpm.at(1));
											methods += ("+\"</"+fldnames.at(k+1)+">\";\n");
											//if(methsall[classStructure.getTreatedClassName(true)+"get"+cam+methpm.at(0)])typedefs += "if(nam==\""+methpm.at(1)+"\")\n__obj->set"+cam+"(*("+methpm.at(0)+"*)"+app+"unSerialize"+stlcnt+stlcnttyp+"(root.getChildElements().at(i).render()));\n";
										}
										else
										{
											methods += (methpm.at(0)+"* __temp_obj_ser"+stlcnt+stlcnttyp+classStructure.classN+methpm.at(1)+" = __obj->"+methpm.at(1)+"();\n");
											if(stlcnt=="int" || stlcnt=="short" || stlcnt=="long" || stlcnt=="float" || stlcnt=="string" ||
													stlcnt=="std::string" || stlcnt=="double" || stlcnt=="bool")
												methods += ("if(__obj->"+methpm.at(1)+"()!=NULL)objxml += \"<"+fldnames.at(k+1)+" type=\\\""+stltyp+"\\\">\"+XMLSerialize::serialize<"+methpm.at(0)+" >(*__temp_obj_ser"+stlcnt+stlcnttyp+classStructure.classN+methpm.at(1)+",\""+app+"\")");
											else
												methods += ("if(__obj->"+methpm.at(1)+"()!=NULL)objxml += \"<"+fldnames.at(k+1)+" type=\\\""+stltyp+"\\\">\"+"+app+"serialize"+stlcnt+stlcnttyp+"(__temp_obj_ser"+stlcnt+stlcnttyp+classStructure.classN+methpm.at(1)+")");
											//string cam = AfcUtil::camelCased(methpm.at(1));
											methods += ("+\"</"+fldnames.at(k+1)+">\";\n");
											//if(methsall[classStructure.getTreatedClassName(true)+"get"+cam+methpm.at(0)])typedefs += "if(nam==\""+methpm.at(1)+"\")\n__obj->set"+cam+"(*("+methpm.at(0)+"*)"+app+"unSerialize"+stlcnt+stlcnttyp+"(root.getChildElements().at(i).render()));\n";
										}
									}
									else
									{
										if(!ptr)
										{
											methods += (methpm.at(0)+" __temp_obj_ser"+methpm.at(1)+" = __obj->"+methpm.at(1)+"();\n");
											methods += ("objxml += \"<"+fldnames.at(k+1)+" type=\\\""+methpm.at(0)+"\\\">\"+"+app+"serialize"+methpm.at(0)+"(&__temp_obj_ser"+methpm.at(1)+")");
											methods += ("+\"</"+fldnames.at(k+1)+">\";\n");
											//string cam = AfcUtil::camelCased(methpm.at(1));
											//if(methsall[classStructure.getTreatedClassName(true)+"get"+cam+methpm.at(0)])typedefs += "if(nam==\""+methpm.at(1)+"\")\n__obj->set"+cam+"(*("+methpm.at(0)+"*)"+app+"unSerialize"+methpm.at(0)+"(root.getChildElements().at(i).render()));\n";
										}
										else
										{
											methods += (methpm.at(0)+"* __temp_obj_ser"+methpm.at(1)+" = __obj->"+methpm.at(1)+"();\n");
											methods += ("if(__obj->"+methpm.at(1)+"()!=NULL)objxml += \"<"+fldnames.at(k+1)+" type=\\\""+methpm.at(0)+"\\\">\"+"+app+"serialize"+methpm.at(0)+"(__temp_obj_ser"+methpm.at(1)+")");
											methods += ("+\"</"+fldnames.at(k+1)+">\";\n");
											//string cam = AfcUtil::camelCased(methpm.at(1));
											//if(methsall[classStructure.getTreatedClassName(true)+"get"+cam+methpm.at(0)])typedefs += "if(nam==\""+methpm.at(1)+"\")\n__obj->set"+cam+"(*("+methpm.at(0)+"*)"+app+"unSerialize"+methpm.at(0)+"(root.getChildElements().at(i).render()));\n";
										}
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
		methods += "objxml += \"</"+classStructure.getTreatedClassName(true)+">\";\nreturn objxml;\n}\n";
		methods += "\nstring " +app+ "serialize" + classStructure.getTreatedClassName(true) + "Vec(void* obje)\n{\nvector<"+classStructure.getFullyQualifiedClassName()+"> *__obj=(vector<"+classStructure.getFullyQualifiedClassName()+">*)obje;\n";
		methods += "string objxml=\"<vector-"+classStructure.getTreatedClassName(true)+">\";\nfor(unsigned int i=0;i<__obj->size();i++)\n{\nobjxml+="+app+"serialize"+classStructure.getTreatedClassName(true)+"(&(__obj->at(i)));\n}\nobjxml+=\"</vector-"+classStructure.getTreatedClassName(true)+">\";\n";
		methods += "return objxml;}\n";
		methods += "\nstring " +app+ "serialize"+classStructure.getTreatedClassName(true)+"Q(void *t){std::queue<"+classStructure.getFullyQualifiedClassName()+"> *_t=(std::queue<"+classStructure.getFullyQualifiedClassName()+">*)t;std::queue<"+classStructure.getFullyQualifiedClassName()+"> *tt = new std::queue<"+classStructure.getFullyQualifiedClassName()+">;	*tt = *_t;	string objXml = \"<queue-"+classStructure.getTreatedClassName(true)+">\";	for(unsigned int var=0;var<tt->size();var++)	{		objXml += "+app+"serialize"+classStructure.getTreatedClassName(true)+"(&(tt->front()));		tt->pop();	}	objXml += \"</queue-"+classStructure.getTreatedClassName(true)+">\";	return objXml;}";
		methods += "\nstring " +app+ "serialize"+classStructure.getTreatedClassName(true)+"Dq(void *_t){deque<"+classStructure.getFullyQualifiedClassName()+"> *t=(deque<"+classStructure.getFullyQualifiedClassName()+">*)_t;string objXml = \"<deque-"+classStructure.getTreatedClassName(true)+">\";	for(unsigned int var=0;var<t->size();var++)	{		objXml += "+app+"serialize"+classStructure.getTreatedClassName(true)+"(&(t->at(var)));	}	objXml += \"</deque-"+classStructure.getTreatedClassName(true)+">\";	return objXml;}";
		methods += "\nstring " +app+ "serialize"+classStructure.getTreatedClassName(true)+"Lis(void *_t){	list<"+classStructure.getFullyQualifiedClassName()+"> *t=(list<"+classStructure.getFullyQualifiedClassName()+">*)_t;list<"+classStructure.getFullyQualifiedClassName()+">::iterator it;	string objXml = \"<list-"+classStructure.getTreatedClassName(true)+">\";	for(it=t->begin();it!=t->end();++it)	{"+classStructure.getTreatedClassName(true)+" _temp=*it;	objXml += "+app+"serialize"+classStructure.getTreatedClassName(true)+"(&_temp);	}	objXml += \"</list-"+classStructure.getTreatedClassName(true)+">\";	return objXml;}";
		classes += "\nstring " +app+ "serialize" + classStructure.getTreatedClassName(true) + "Vec(void* obje);\nstring " +app+ "serialize"+classStructure.getTreatedClassName(true)+"Q(void *t);\nstring " +app+ "serialize"+classStructure.getTreatedClassName(true)+"Dq(void *_t);\nstring " +app+ "serialize"+classStructure.getTreatedClassName(true)+"Lis(void *_t);";
		if(classStructure.prosetser)
		{
			methods += "\nstring " +app+ "serialize"+classStructure.getTreatedClassName(true)+"Set(void *_t){	set<"+classStructure.getFullyQualifiedClassName()+"> *t=(set<"+classStructure.getFullyQualifiedClassName()+">*)_t;set<"+classStructure.getFullyQualifiedClassName()+">::iterator it;	string objXml = \"<set-"+classStructure.getTreatedClassName(true)+">\";	for(it=t->begin();it!=t->end();++it)	{"+classStructure.getTreatedClassName(true)+" _temp=*it;	objXml += "+app+"serialize"+classStructure.getTreatedClassName(true)+"(&_temp);	}	objXml += \"</set-"+classStructure.getTreatedClassName(true)+">\";	return objXml;}";
			methods += "\nstring " +app+ "serialize"+classStructure.getTreatedClassName(true)+"MulSet(void *_t){	multiset<"+classStructure.getFullyQualifiedClassName()+"> *t=(multiset<"+classStructure.getFullyQualifiedClassName()+">*)_t;multiset<"+classStructure.getFullyQualifiedClassName()+">::iterator it;	string objXml = \"<multiset-"+classStructure.getTreatedClassName(true)+">\";	for(it=t->begin();it!=t->end();++it)	{"+classStructure.getTreatedClassName(true)+" _temp=*it;	objXml += "+app+"serialize"+classStructure.getTreatedClassName(true)+"(&_temp);	}	objXml += \"</multiset-"+classStructure.getTreatedClassName(true)+">\";	return objXml;}";
			classes += "\nstring " +app+ "serialize"+classStructure.getTreatedClassName(true)+"Set(void *_t);\nstring " +app+ "serialize"+classStructure.getTreatedClassName(true)+"MulSet(void *_t);";
		}

		typedefs += "\n}\nreturn __obj;\n}";
		typedefs += "\nvoid* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"Dq(string objXml){deque<"+classStructure.getFullyQualifiedClassName()+"> *t = new deque<"+classStructure.getFullyQualifiedClassName()+">;XmlParser parser(\"Parser\");\nDocument doc = parser.getDocument(objXml);\nElement message = doc.getRootElement();\nif(message.getTagName()==\"\" && message.getChildElements().size()==0)\nreturn NULL;\nfor (int var = 0; var < (int)message.getChildElements().size(); var++){	Element ele = message.getChildElements().at(var);	if(ele.getTagName()==\""+classStructure.getTreatedClassName(true)+"\")	{		t->push_back(*("+classStructure.getFullyQualifiedClassName()+"*)"+app+"unSerialize"+classStructure.getTreatedClassName(true)+"(ele.render()));	}}return t;}";
		typedefs += "\nvoid* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"Q(string objXml){std::queue<"+classStructure.getFullyQualifiedClassName()+"> *t = new std::queue<"+classStructure.getFullyQualifiedClassName()+">;XmlParser parser(\"Parser\");\nDocument doc = parser.getDocument(objXml);\nElement message = doc.getRootElement();\nif(message.getTagName()==\"\" && message.getChildElements().size()==0)\nreturn NULL;\nfor (int var = 0; var < (int)message.getChildElements().size(); var++){	Element ele = message.getChildElements().at(var);	if(ele.getTagName()==\""+classStructure.getTreatedClassName(true)+"\")	{		t->push(*("+classStructure.getFullyQualifiedClassName()+"*)"+app+"unSerialize"+classStructure.getTreatedClassName(true)+"(ele.render()));	}}return t;}";
		typedefs += "\nvoid* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"Lis(string objXml){list<"+classStructure.getFullyQualifiedClassName()+"> *t = new list<"+classStructure.getFullyQualifiedClassName()+">;XmlParser parser(\"Parser\");\nDocument doc =parser.getDocument(objXml);\nElement message = doc.getRootElement();\nif(message.getTagName()==\"\" && message.getChildElements().size()==0)\nreturn NULL;\nfor (int var = 0; var < (int)message.getChildElements().size(); var++){	Element ele = message.getChildElements().at(var);	if(ele.getTagName()==\""+classStructure.getTreatedClassName(true)+"\")	{		t->push_back(*("+classStructure.getFullyQualifiedClassName()+"*)"+app+"unSerialize"+classStructure.getTreatedClassName(true)+"(ele.render()));	}}return t;}";
		classes += "\nvoid* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"Vec(string objXml);\nvoid* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"Q(string objXml);\nvoid* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"Dq(string objXml);\nvoid* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"Lis(string objXml);";
		if(classStructure.prosetser)
		{
			typedefs += "\nvoid* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"Set(string objXml){set<"+classStructure.getFullyQualifiedClassName()+"> *t = new set<"+classStructure.getFullyQualifiedClassName()+">;XmlParser parser(\"Parser\");\nDocument doc = parser.getDocument(objXml);\nElement message = doc.getRootElement();\nif(message.getTagName()==\"\" && message.getChildElements().size()==0)\nreturn NULL;\nfor (int var = 0; var < (int)message.getChildElements().size(); var++){	Element ele = message.getChildElements().at(var);	if(ele.getTagName()==\""+classStructure.getTreatedClassName(true)+"\")	{		t->insert(*("+classStructure.getFullyQualifiedClassName()+"*)"+app+"unSerialize"+classStructure.getTreatedClassName(true)+"(ele.render()));	}}return t;}";
			typedefs += "\nvoid* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"MulSet(string objXml){multiset<"+classStructure.getFullyQualifiedClassName()+"> *t = new multiset<"+classStructure.getFullyQualifiedClassName()+">;XmlParser parser(\"Parser\");\nDocument doc = parser.getDocument(objXml);\nElement message = doc.getRootElement();\nif(message.getTagName()==\"\" && message.getChildElements().size()==0)\nreturn NULL;\nfor (int var = 0; var < (int)message.getChildElements().size(); var++){	Element ele = message.getChildElements().at(var);	if(ele.getTagName()==\""+classStructure.getTreatedClassName(true)+"\")	{		t->insert(*("+classStructure.getFullyQualifiedClassName()+"*)"+app+"unSerialize"+classStructure.getTreatedClassName(true)+"(ele.render()));	}}return t;}";
			classes += "\nvoid* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"Set(string objXml);\nvoid* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"MulSet(string objXml);";
		}
		typedefs += "\nvoid* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"Vec(string objXml){vector<"+classStructure.getFullyQualifiedClassName()+"> *t = new vector<"+classStructure.getFullyQualifiedClassName()+">;XmlParser parser(\"Parser\");\nDocument doc = parser.getDocument(objXml);\nElement message = doc.getRootElement();\nif(message.getTagName()==\"\" && message.getChildElements().size()==0)\nreturn NULL;\nfor (int var = 0; var < (int)message.getChildElements().size(); var++){	Element ele = message.getChildElements().at(var);	if(ele.getTagName()==\""+classStructure.getTreatedClassName(true)+"\")	{		t->push_back(*("+classStructure.getFullyQualifiedClassName()+"*)"+app+"unSerialize"+classStructure.getTreatedClassName(true)+"(ele.render()));	}}return t;}";
		//typedefs = (structinf+"};\n"+typedefs);
	}
	return refDef;
}

string Reflection::generateSerDefinitionBinary(map<string, ClassStructure> allclsmap,string &includesDefs,string &typedefs,string &classes,string &methods,string app)
{
	string refDef;
	string opers;
	/*if (!generateClassInfo(className))
	{
		return refDef;
	}*/

	map<string, ClassStructure>::iterator it;
	for (it=allclsmap.begin();it!=allclsmap.end();++it)
	{
		ClassStructure classStructure = it->second;
		includesDefs += "#include \"" + getClassPath(it->second.getTreatedClassName(true)) + "\"\n";
		classStructure.prosetser = false;
		//classes += "\tif(classStructure.getTreatedClassName(true)ame==\""+classStructure.getTreatedClassName(true)+"\")\n\t\tobjXml = get"+classStructure.getTreatedClassName(true)+"XML(t);\n";
		//refDef += "\tif(classStructure.getTreatedClassName(true)ame==\""+classStructure.getTreatedClassName(true)+"\")\n\t\tt = getObject"+classStructure.getTreatedClassName(true)+"(objXml);\n";

		//string structinf = "\nstruct struct"+classStructure.getTreatedClassName(true)+"{\n";
		classes += "\nstring " +app+ "binarySerialize" + classStructure.getTreatedClassName(true) + "(void* obje);\nvoid* " +app+ "binaryUnSerialize" + classStructure.getTreatedClassName(true) + "(string objXml);";
		methods += "\nstring " +app+ "binarySerialize" + classStructure.getTreatedClassName(true) + "(void* obje)\n{\n"+classStructure.getFullyQualifiedClassName()+" *__obj=("+classStructure.getFullyQualifiedClassName()+"*)obje;\n";
		methods += "AMEFEncoder enc;\nAMEFObject object;\nobject.setName(\""+classStructure.getTreatedClassName(true)+"\");\n";
		typedefs += "\nvoid* " +app+ "binaryUnSerialize" + classStructure.getTreatedClassName(true) + "(string objXml)\n{\n";
		typedefs += classStructure.getFullyQualifiedClassName()+" *__obj=new "+classStructure.getFullyQualifiedClassName()+";\nAMEFDecoder dec;\nAMEFObject* root = dec.decodeB(objXml, true, false);\n";
		typedefs += "if(root->getNameStr()!=\""+classStructure.getTreatedClassName(true)+"\")throw \"Invalid Binary Object\";";
		typedefs += "for(unsigned int i=0;i<root->getPackets().size();i++)\n{\n";
		typedefs += "string nam=root->getPackets().at(i)->getNameStr();\n";

		string publf, privf, protf ,publm, privm, protm;
		string meth,fld;
		size_t tes;
		vector<string> fldnames;
		if (classStructure.pri.size() > 0)
		{
			for (unsigned int i = 0; i < classStructure.pri.size(); i++)
			{
				if(((tes=classStructure.pri.at(i).find("("))==string::npos && (tes=classStructure.pri.at(i).find(")"))==string::npos && classStructure.pri.at(i).find("~")==string::npos))
				{
					fld = classStructure.pri.at(i);
					StringUtil::replaceFirst(fld,";","");
					vector<string> fldp;
					StringUtil::split(fldp, fld, (" "));
					if(fldp.size()==2)
					{
						string nam = fldp.at(1);
						fldnames.push_back(fldp.at(0));
						fldnames.push_back(nam);
					}
				}
			}
		}
		if (classStructure.pro.size() > 0)
		{
			for (unsigned int i = 0; i < classStructure.pro.size(); i++)
			{
				if(((tes=classStructure.pro.at(i).find("("))==string::npos && (tes=classStructure.pro.at(i).find(")"))==string::npos && classStructure.pro.at(i).find("~")==string::npos))
				{
					fld = classStructure.pro.at(i);
					StringUtil::replaceFirst(fld,";","");
					vector<string> fldp;
					StringUtil::split(fldp, fld, (" "));
					if(fldp.size()==2)
					{
						string nam = fldp.at(1);
						fldnames.push_back(fldp.at(0));
						fldnames.push_back(nam);
					}
				}
			}
		}
		if (classStructure.pub.size() > 0)
		{
			for (unsigned int i = 0; i < classStructure.pub.size(); i++)
			{
				if(((tes=classStructure.pub.at(i).find("("))==string::npos && (tes=classStructure.pub.at(i).find(")"))==string::npos && classStructure.pub.at(i).find("~")==string::npos))
				{
					fld = classStructure.pub.at(i);
					RegexUtil::replace(fld, "[\t]+", " ");
					RegexUtil::replace(fld, "[ ]+", " ");
					//RegexUtil::replace(fld, "[ ?, ?]+", ",");
					StringUtil::replaceFirst(fld,";","");
					bool ptr = false;
					if(fld.find("*")!=string::npos)
					{
						ptr = true;
						StringUtil::replaceFirst(fld,"*","");
					}
					vector<string> fldp;
					StringUtil::split(fldp, fld, (" "));
					if(fldp.size()==2)
					{
						string nam = fldp.at(1);
						if(fldp.at(0)=="int" || fldp.at(0)=="long" || fldp.at(0)=="short" || fldp.at(0)=="float" || fldp.at(0)=="string" || fldp.at(0)=="std::string" || fldp.at(0)=="double" || fldp.at(0)=="bool" || fldp.at(0)=="unsigned int" || fldp.at(0)=="unsigned long" || fldp.at(0)=="unsigned short" || fldp.at(0)=="long long" || fldp.at(0)=="unsigned long long")
						{
							string argtype = StringUtil::capitalizedCopy(fldp.at(0));
							string vallu = "root->getPackets().at(i)->get"+argtype+"Value()";
							if(fldp.at(0)=="string" || fldp.at(0)=="std::string")
							{
								vallu = "root->getPackets().at(i)->getValueStr()";
							}
							if(!ptr)
							{
								methods += ("object.addPacket(__obj->"+fldp.at(1)+",\""+fldp.at(1)+"\");\n");
								string cam = StringUtil::capitalizedCopy(fldp.at(1));
								typedefs += "if(nam==\""+fldp.at(1)+"\" && root->getPackets().at(i)->getValue()!=\"\")\n__obj->"+fldp.at(1)+" = "+vallu+";\n";
							}
							else
							{
								methods += ("if(__obj->"+fldp.at(1)+"!=NULL)object.addPacket(*__obj->"+fldp.at(1)+",\""+fldp.at(1)+"\");\n");
								string cam = StringUtil::capitalizedCopy(fldp.at(1));
								typedefs += "if(nam==\""+fldp.at(1)+"\" && root->getPackets().at(i)->getValue()!=\"\")\n__obj->"+fldp.at(1)+" = new "+fldp.at(0)+"("+vallu+");\n";
							}
						}
						else if(fldp.at(0)=="Date")
						{
							if(!ptr)
							{
								methods += ("DateFormat formt"+fldp.at(1)+"(\"yyyy-mm-dd hh:mi:ss\");\n");
								methods += ("object.addPacket(formt"+fldp.at(1)+".format(__obj->"+fldp.at(1)+"),\""+fldp.at(1)+"\");\n");
								//methods += ("DateFormat formt"+fldp.at(1)+"(\"yyyy-mm-dd hh:mi:ss\");\nobjxml += \"<"+fldp.at(1)+" type=\\\""+fldp.at(0)+"\\\">\"+formt"+fldp.at(1)+".format(__obj->"+fldp.at(1)+")");
								string cam = StringUtil::capitalizedCopy(fldp.at(1));
								//methods += ("+\"</"+nam+">\";\n");
								typedefs += "if(nam==\""+fldp.at(1)+"\" && root->getPackets().at(i)->getValue()!=\"\")\n{\nDateFormat formt"+fldp.at(1)+"(\"yyyy-mm-dd hh:mi:ss\");\n__obj->"+fldp.at(1)+" = *(formt"+fldp.at(1)+".parse(root->getPackets().at(i)->getValue()));\n}\n";
							}
							else
							{
								methods += ("if(__obj->"+fldp.at(1)+"!=NULL){DateFormat formt"+fldp.at(1)+"(\"yyyy-mm-dd hh:mi:ss\");\n");
								methods += ("object.addPacket(formt"+fldp.at(1)+".format(*__obj->"+fldp.at(1)+"),\""+fldp.at(1)+"\");}\n");
								//methods += ("DateFormat formt"+fldp.at(1)+"(\"yyyy-mm-dd hh:mi:ss\");\nobjxml += \"<"+fldp.at(1)+" type=\\\""+fldp.at(0)+"\\\">\"+formt"+fldp.at(1)+".format(__obj->"+fldp.at(1)+")");
								string cam = StringUtil::capitalizedCopy(fldp.at(1));
								//methods += ("+\"</"+nam+">\";\n");
								typedefs += "if(nam==\""+fldp.at(1)+"\" && root->getPackets().at(i)->getValue()!=\"\")\n{\nDateFormat formt"+fldp.at(1)+"(\"yyyy-mm-dd hh:mi:ss\");\n__obj->"+fldp.at(1)+" = (formt"+fldp.at(1)+".parse(root->getPackets().at(i)->getValue()));\n}\n";
							}
						}
						else if(fldp.at(0)=="BinaryData")
						{
							if(!ptr)
							{
								methods += ("object.addPacket(BinaryData::serilaize(__obj->"+fldp.at(1)+"),\""+fldp.at(1)+"\");\n");
								//methods += ("objxml += \"<"+fldp.at(1)+" type=\\\""+fldp.at(0)+"\\\">\"+BinaryData::serilaize(__obj->"+fldp.at(1)+")");
								string cam = StringUtil::capitalizedCopy(fldp.at(1));
								//methods += ("+\"</"+nam+">\";\n");
								typedefs += "if(nam==\""+fldp.at(1)+"\" && root->getPackets().at(i)->getValue()!=\"\")\n__obj->"+fldp.at(1)+" = *(BinaryData::unSerilaize(root->getPackets().at(i)->getValue()));\n";
							}
							else
							{
								methods += ("if(__obj->"+fldp.at(1)+"!=NULL)object.addPacket(BinaryData::serilaize(*__obj->"+fldp.at(1)+"),\""+fldp.at(1)+"\");\n");
								//methods += ("objxml += \"<"+fldp.at(1)+" type=\\\""+fldp.at(0)+"\\\">\"+BinaryData::serilaize(__obj->"+fldp.at(1)+")");
								string cam = StringUtil::capitalizedCopy(fldp.at(1));
								//methods += ("+\"</"+nam+">\";\n");
								typedefs += "if(nam==\""+fldp.at(1)+"\" && root->getPackets().at(i)->getValue()!=\"\")\n__obj->"+fldp.at(1)+" = (BinaryData::unSerilaize(root->getPackets().at(i)->getValue()));\n";
							}
						}
						else if(fldp.at(0).find("vector")!=string::npos || fldp.at(0).find("queue")!=string::npos ||
								fldp.at(0).find("deque")!=string::npos || fldp.at(0).find("set")!=string::npos ||
								fldp.at(0).find("list")!=string::npos || fldp.at(0).find("multiset")!=string::npos)
						{
							string stlcnt = fldp.at(0);
							string stltyp = fldp.at(0);
							string contType;
							StringUtil::replaceFirst(stltyp,"std::","");
							StringUtil::replaceFirst(stltyp,"<","::");
							StringUtil::replaceFirst(stltyp,">","");
							StringUtil::replaceFirst(stltyp," ","");
							string stlcnttyp = "";
							if(fldp.at(0).find("vector")!=string::npos)
							{
								contType = "std::vector<";
								stlcnttyp = "Vec";
							}
							else if(fldp.at(0).find("queue")!=string::npos)
							{
								contType = "std::queue<";
								stlcnttyp = "Q";
							}
							else if(fldp.at(0).find("deque")!=string::npos)
							{
								contType = "std::deque<";
								stlcnttyp = "Dq";
							}
							else if(fldp.at(0).find("list")!=string::npos)
							{
								contType = "std::list<";
								stlcnttyp = "Lis";
							}
							else if(fldp.at(0).find("multiset")!=string::npos)
							{
								contType = "std::multiset<";
								stlcnttyp = "MulSet";
							}
							else
							{
								contType = "std::set<";
								stlcnttyp = "Set";
							}
							StringUtil::replaceFirst(stlcnt,"std::","");
							StringUtil::replaceFirst(stlcnt,"vector","");
							StringUtil::replaceFirst(stlcnt,"queue","");
							StringUtil::replaceFirst(stlcnt,"deque","");
							StringUtil::replaceFirst(stlcnt,"multiset","");
							StringUtil::replaceFirst(stlcnt,"set","");
							StringUtil::replaceFirst(stlcnt,"list","");
							StringUtil::replaceFirst(stlcnt,"<","");
							StringUtil::replaceFirst(stlcnt,">","");
							StringUtil::replaceFirst(stlcnt," ","");

							contType += stlcnt + ",";

							if(!ptr)
							{
								methods += (fldp.at(0)+" __temp_obj_ser"+stlcnt+stlcnttyp+classStructure.getTreatedClassName(true)+fldp.at(1)+" = __obj->"+fldp.at(1)+";\n");
								if(stlcnt=="int" || stlcnt=="long" || stlcnt=="short" || stlcnt=="float" || stlcnt=="string" || stlcnt=="std::string" || stlcnt=="double" || stlcnt=="bool" || stlcnt=="unsigned int" || stlcnt=="unsigned long" || stlcnt=="unsigned short" || stlcnt=="long long" || stlcnt=="unsigned long long")
									methods += ("object.addPacket(Serialize::serialize<"+fldp.at(0)+" >(__temp_obj_ser"+stlcnt+stlcnttyp+classStructure.getTreatedClassName(true)+fldp.at(1)+",\""+app+"\"),\""+fldp.at(1)+"\");\n");
								else
									methods += ("object.addPacket("+app+"binarySerialize"+stlcnt+stlcnttyp+"(&__temp_obj_ser"+stlcnt+stlcnttyp+classStructure.getTreatedClassName(true)+fldp.at(1)+"),\""+fldp.at(1)+"\");\n");
								string cam = StringUtil::capitalizedCopy(fldp.at(1));
								//methods += ("+\"</"+nam+">\";\n");
								typedefs += "if(nam==\""+fldp.at(1)+"\" && root->getPackets().at(i)->getValue()!=\"\"){";
								typedefs += "\nAMEFEncoder enc;\n";
								if(stlcnt=="int" || stlcnt=="short" || stlcnt=="long" || stlcnt=="float" || stlcnt=="string" ||
										stlcnt=="std::string" || stlcnt=="double" || stlcnt=="bool")
									typedefs += "\n__obj->"+fldp.at(1)+" = Serialize::unserialize<"+fldp.at(0)+" >(root->getPackets().at(i)->getValue(),\""+app+"\");\n";
								else
									typedefs += "\n__obj->"+fldp.at(1)+" = *("+fldp.at(0)+"*)"+app+"binaryUnSerialize"+stlcnt+stlcnttyp+"(root->getPackets().at(i)->getValue());\n";
								typedefs += "\n}\n";
							}
							else
							{
								methods += (fldp.at(0)+"* __temp_obj_ser"+stlcnt+stlcnttyp+classStructure.getTreatedClassName(true)+fldp.at(1)+" = __obj->"+fldp.at(1)+";\n");
								if(stlcnt=="int" || stlcnt=="short" || stlcnt=="long" || stlcnt=="float" || stlcnt=="string" ||
										stlcnt=="std::string" || stlcnt=="double" || stlcnt=="bool")
									methods += ("if(__obj->"+fldp.at(1)+"!=NULL)object.addPacket(Serialize::serialize<"+fldp.at(0)+" >(*__temp_obj_ser"+stlcnt+stlcnttyp+classStructure.getTreatedClassName(true)+fldp.at(1)+",\""+app+"\"),\""+fldp.at(1)+"\");\n");
								else
									methods += ("if(__obj->"+fldp.at(1)+"!=NULL)object.addPacket("+app+"binarySerialize"+stlcnt+stlcnttyp+"(__temp_obj_ser"+stlcnt+stlcnttyp+classStructure.getTreatedClassName(true)+fldp.at(1)+"),\""+fldp.at(1)+"\");\n");
								string cam = StringUtil::capitalizedCopy(fldp.at(1));
								//methods += ("+\"</"+nam+">\";\n");
								typedefs += "if(nam==\""+fldp.at(1)+"\" && root->getPackets().at(i)->getValue()!=\"\"){";
								typedefs += "\nAMEFEncoder enc;\n";
								//@TODO Is a concern if pointer is used, the address reference might lead to data issues
								if(stlcnt=="int" || stlcnt=="long" || stlcnt=="short" || stlcnt=="float" || stlcnt=="string" || stlcnt=="std::string" || stlcnt=="double" || stlcnt=="bool" || stlcnt=="unsigned int" || stlcnt=="unsigned long" || stlcnt=="unsigned short" || stlcnt=="long long" || stlcnt=="unsigned long long")
									typedefs += "\n__obj->"+fldp.at(1)+" = ("+fldp.at(0)+"*)Serialize::unSerializeUnknown(root->getPackets().at(i)->getValue(),\""+contType+"\",\""+app+"\");\n";
								else
									typedefs += "\n__obj->"+fldp.at(1)+" = ("+fldp.at(0)+"*)"+app+"binaryUnSerialize"+stlcnt+stlcnttyp+"(root->getPackets().at(i)->getValue());\n";
								typedefs += "\n}\n";
							}
						}
						else
						{
							if(!ptr)
							{
								methods += (fldp.at(0)+" __temp_obj_ser"+fldp.at(1)+" = __obj->"+fldp.at(1)+";\n");
								methods += ("object.addPacket("+app+"binarySerialize"+fldp.at(0)+"(&__temp_obj_ser"+fldp.at(1)+"),\""+fldp.at(1)+"\");\n");
								string cam = StringUtil::capitalizedCopy(fldp.at(1));
								//methods += ("+\"</"+nam+">\";\n");
								typedefs += "if(nam==\""+fldp.at(1)+"\" && root->getPackets().at(i)->getValue()!=\"\")\n__obj->"+fldp.at(1)+" = *("+fldp.at(0)+"*)"+app+"binaryUnSerialize"+fldp.at(0)+"(root->getPackets().at(i)->getValue());\n";
							}
							else
							{
								methods += (fldp.at(0)+"* __temp_obj_ser"+fldp.at(1)+" = __obj->"+fldp.at(1)+";\n");
								methods += ("if(__obj->"+fldp.at(1)+"!=NULL)object.addPacket("+app+"binarySerialize"+fldp.at(0)+"(__temp_obj_ser"+fldp.at(1)+"),\""+fldp.at(1)+"\");\n");
								string cam = StringUtil::capitalizedCopy(fldp.at(1));
								//methods += ("+\"</"+nam+">\";\n");
								typedefs += "if(nam==\""+fldp.at(1)+"\" && root->getPackets().at(i)->getValue()!=\"\")\n__obj->"+fldp.at(1)+" = ("+fldp.at(0)+"*)"+app+"binaryUnSerialize"+fldp.at(0)+"(root->getPackets().at(i)->getValue());\n";
							}
						}
						//structinf += (fldp.at(0)+" "+fldp.at(1)+";\n");
					}
				}
				if((tes=classStructure.pub.at(i).find("("))!=string::npos && (tes=classStructure.pub.at(i).find(")"))!=string::npos && classStructure.pub.at(i).find("~")==string::npos
						&& fldnames.size()>0)
				{
					meth = classStructure.pub.at(i);
					StringUtil::replaceFirst(meth,";","");
					RegexUtil::replace(meth, "[\t]+", " ");
					RegexUtil::replace(meth, "[ ]+", " ");
					//RegexUtil::replace(meth, "[ ?, ?]+", ",");
					bool ptr = false;
					if(meth.find("*")!=string::npos)
					{
						ptr = true;
						StringUtil::replaceFirst(meth,"*","");
					}

					string argts = meth.substr(meth.find("("),meth.find(")")-meth.find("("));
					StringUtil::replaceFirst(argts,"(","");
					StringUtil::replaceAll(argts,")","");
					meth = meth.substr(0,meth.find("("));
					if(meth.find("operator")!=string::npos)
					{
						if(meth.find("<")!=string::npos)
						{
							classStructure.prosetser = true;
						}
						else if(meth.find(">")!=string::npos)
						{
							classStructure.prosetser = true;
						}
					}
					else
					{
						//StringUtil::replaceFirst(meth,")"," ");
						vector<string> methp,methpm,argp,argpm;
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

						if(methpm.at(0)!=classStructure.getTreatedClassName(false))
						{
							for(unsigned int k = 0; k < fldnames.size(); k=k+2)
							{
								string cam = StringUtil::capitalizedCopy(fldnames.at(k+1));
								string fldNamewoptr = StringUtil::replaceFirstCopy(fldnames.at(k), "*", "");
								//logger << "setter check " << fldNamewoptr << " "<<methpm.at(0) << " "<< methpm.at(1) << " "<< cam<< endl;
								if(argpm.size()==1)
								{
									StringUtil::replaceFirst(argpm.at(0), "*", "");
									//logger << argpm.at(0) << " " << argpm.size() << endl;
								}
								if("set"+cam==methpm.at(1) && argpm.size()==1 && argpm.at(0)==fldNamewoptr && methpm.at(0)=="void")
								{
									//logger << " inside setter " << endl;
									if(argpm.at(0)=="int" || argpm.at(0)=="long" || argpm.at(0)=="short" || argpm.at(0)=="float" || argpm.at(0)=="string" || argpm.at(0)=="std::string" || argpm.at(0)=="double" || argpm.at(0)=="bool" || argpm.at(0)=="unsigned int" || argpm.at(0)=="unsigned long" || argpm.at(0)=="unsigned short" || argpm.at(0)=="long long" || argpm.at(0)=="unsigned long long")
									{
										string argtype = StringUtil::capitalizedCopy(argpm.at(0));
										string vallu = "root->getPackets().at(i)->get"+argtype+"Value()";
										if(argpm.at(0)=="string" || argpm.at(0)=="std::string")
										{
											vallu = "root->getPackets().at(i)->getValueStr()";
										}
										if(!ptr)
										{
											typedefs += "if(nam==\""+fldnames.at(k+1)+"\" && root->getPackets().at(i)->getValue()!=\"\")\n__obj->"+methpm.at(1)+"("+vallu+");\n";
										}
										else
										{
											typedefs += "if(nam==\""+fldnames.at(k+1)+"\" && root->getPackets().at(i)->getValue()!=\"\")\n__obj->"+methpm.at(1)+"(new "+argpm.at(0)+"("+vallu+"));\n";
										}
									}
									else if(argpm.at(0)=="Date")
									{
										if(!ptr)
										{
											typedefs += "if(nam==\""+fldnames.at(k+1)+"\" && root->getPackets().at(i)->getValue()!=\"\")\n{\nDateFormat formt"+cam+"(\"yyyy-mm-dd hh:mi:ss\");\n__obj->"+methpm.at(1)+"(*(formt"+cam+".parse(root->getPackets().at(i)->getValue())));\n}\n";
										}
										else
										{
											typedefs += "if(nam==\""+fldnames.at(k+1)+"\" && root->getPackets().at(i)->getValue()!=\"\")\n{\nDateFormat formt"+cam+"(\"yyyy-mm-dd hh:mi:ss\");\n__obj->"+methpm.at(1)+"((formt"+cam+".parse(root->getPackets().at(i)->getValue())));\n}\n";
										}
									}
									else if(argpm.at(0)=="BinaryData")
									{
										if(!ptr)
										{
											typedefs += "if(nam==\""+fldnames.at(k+1)+"\" && root->getPackets().at(i)->getValue()!=\"\")\n__obj->"+methpm.at(1)+"(*(BinaryData::unSerilaize(root->getPackets().at(i)->getValue())));\n";
										}
										else
										{
											typedefs += "if(nam==\""+fldnames.at(k+1)+"\" && root->getPackets().at(i)->getValue()!=\"\")\n__obj->"+methpm.at(1)+"((BinaryData::unSerilaize(root->getPackets().at(i)->getValue())));\n";
										}
									}
									else if(argpm.at(0).find("vector")!=string::npos || argpm.at(0).find("queue")!=string::npos ||
											argpm.at(0).find("deque")!=string::npos || argpm.at(0).find("set")!=string::npos ||
											argpm.at(0).find("list")!=string::npos || argpm.at(0).find("multiset")!=string::npos)
									{
										string stlcnt = argpm.at(0);
										string stltyp = argpm.at(0);
										string contType;
										StringUtil::replaceFirst(stltyp,"std::","");
										StringUtil::replaceFirst(stltyp,"<","::");
										StringUtil::replaceFirst(stltyp,">","");
										StringUtil::replaceFirst(stltyp," ","");
										string stlcnttyp = "";
										if(argpm.at(0).find("vector")!=string::npos)
										{
											contType = "std::vector<";
											stlcnttyp = "Vec";
										}
										else if(argpm.at(0).find("queue")!=string::npos)
										{
											contType = "std::queue<";
											stlcnttyp = "Q";
										}
										else if(argpm.at(0).find("deque")!=string::npos)
										{
											contType = "std::deque<";
											stlcnttyp = "Dq";
										}
										else if(argpm.at(0).find("list")!=string::npos)
										{
											contType = "std::list<";
											stlcnttyp = "Lis";
										}
										else if(argpm.at(0).find("multiset")!=string::npos)
										{
											contType = "std::multiset<";
											stlcnttyp = "MulSet";
										}
										else
										{
											contType = "std::set<";
											stlcnttyp = "Set";
										}
										StringUtil::replaceFirst(stlcnt,"std::","");
										StringUtil::replaceFirst(stlcnt,"vector","");
										StringUtil::replaceFirst(stlcnt,"queue","");
										StringUtil::replaceFirst(stlcnt,"deque","");
										StringUtil::replaceFirst(stlcnt,"multiset","");
										StringUtil::replaceFirst(stlcnt,"set","");
										StringUtil::replaceFirst(stlcnt,"list","");
										StringUtil::replaceFirst(stlcnt,"<","");
										StringUtil::replaceFirst(stlcnt,">","");
										StringUtil::replaceFirst(stlcnt," ","");

										contType += stlcnt + ",";

										if(!ptr)
										{
											typedefs += "if(nam==\""+fldnames.at(k+1)+"\" && root->getPackets().at(i)->getValue()!=\"\"){";
											typedefs += "\nAMEFEncoder enc;";
											if(stlcnt=="int" || stlcnt=="short" || stlcnt=="long" || stlcnt=="float" || stlcnt=="string" ||
													stlcnt=="std::string" || stlcnt=="double" || stlcnt=="bool")
												typedefs += "\n__obj->set"+cam+"(Serialize::unserialize<"+argpm.at(0)+" >(root->getPackets().at(i)->getValue(),\""+app+"\"));\n";
											else
												typedefs += "\n__obj->set"+cam+"(*("+argpm.at(0)+"*)"+app+"binaryUnSerialize"+stlcnt+stlcnttyp+"(root->getPackets().at(i)->getValue()));\n";
											typedefs += "\n}\n";
										}
										else
										{
											typedefs += "if(nam==\""+fldnames.at(k+1)+"\" && root->getPackets().at(i)->getValue()!=\"\"){";
											typedefs += "\nAMEFEncoder enc;";
											if(stlcnt=="int" || stlcnt=="short" || stlcnt=="long" || stlcnt=="float" || stlcnt=="string" ||
													stlcnt=="std::string" || stlcnt=="double" || stlcnt=="bool")
												typedefs += "\n__obj->set"+cam+"(("+argpm.at(0)+"*)Serialize::unSerializeUnknown(root->getPackets().at(i)->getValue(),\""+contType+"\",\""+app+"\"));\n";
											else
												typedefs += "\n__obj->set"+cam+"(("+argpm.at(0)+"*)"+app+"binaryUnSerialize"+stlcnt+stlcnttyp+"(root->getPackets().at(i)->getValue()));\n";
											typedefs += "\n}\n";
										}
										//if(methsall[classStructure.getTreatedClassName(true)+"get"+cam+methpm.at(0)])typedefs += "if(nam==\""+methpm.at(1)+"\")\n__obj->set"+cam+"(*("+methpm.at(0)+"*)"+app+"binaryUnSerialize"+stlcnt+stlcnttyp+"(root.getChildElements().at(i).render()));\n";
									}
									else
									{
										if(!ptr)
										{
											typedefs += "if(nam==\""+fldnames.at(k+1)+"\" && root->getPackets().at(i)->getValue()!=\"\")\n{\nAMEFEncoder enc;\n__obj->"+methpm.at(1)+"(*("+argpm.at(0)+"*)"+app+"binaryUnSerialize"+argpm.at(0)+"(root->getPackets().at(i)->getValueStr()));}\n";
										}
										else
										{
											typedefs += "if(nam==\""+fldnames.at(k+1)+"\" && root->getPackets().at(i)->getValue()!=\"\")\n{\nAMEFEncoder enc;\n__obj->"+methpm.at(1)+"(("+argpm.at(0)+"*)"+app+"binaryUnSerialize"+argpm.at(0)+"(root->getPackets().at(i)->getValueStr()));}\n";
										}
									}
								}
								else if("get"+cam==methpm.at(1) && argpm.size()==0 && methpm.at(0)==fldNamewoptr)
								{
									if(methpm.at(0)=="int" || methpm.at(0)=="long" || methpm.at(0)=="short" || methpm.at(0)=="float" || methpm.at(0)=="string" || methpm.at(0)=="std::string" || methpm.at(0)=="double" || methpm.at(0)=="bool" || methpm.at(0)=="unsigned int" || methpm.at(0)=="unsigned long" || methpm.at(0)=="unsigned short" || methpm.at(0)=="long long" || methpm.at(0)=="unsigned long long")
									{
										if(!ptr)
										{
											methods += ("object.addPacket(__obj->"+methpm.at(1)+"(),\""+fldnames.at(k+1)+"\");\n");
											//methods += ("objxml += \"<"+fldnames.at(k+1)+" type=\\\""+methpm.at(0)+"\\\">\"+CastUtil::lexical_cast<string>(__obj->"+methpm.at(1)+"())");
											//methods += ("+\"</"+fldnames.at(k+1)+">\";\n");
											//if(methsall[classStructure.getTreatedClassName(true)+"get"+cam+methpm.at(0)])typedefs += "if(nam==\""+fldnames.at(k+1)+"\")\n__obj->set"+cam+"(CastUtil::lexical_cast<"+methpm.at(0)+">(root->getPackets().at(i)->getValue()));\n";
										}
										else
										{
											methods += ("if(__obj->"+methpm.at(1)+"()!=NULL)object.addPacket(*__obj->"+methpm.at(1)+"(),\""+fldnames.at(k+1)+"\");\n");
											//methods += ("objxml += \"<"+fldnames.at(k+1)+" type=\\\""+methpm.at(0)+"\\\">\"+CastUtil::lexical_cast<string>(__obj->"+methpm.at(1)+"())");
											//methods += ("+\"</"+fldnames.at(k+1)+">\";\n");
											//if(methsall[classStructure.getTreatedClassName(true)+"get"+cam+methpm.at(0)])typedefs += "if(nam==\""+fldnames.at(k+1)+"\")\n__obj->set"+cam+"(new "+methpm.at(0)+"(CastUtil::lexical_cast<"+methpm.at(0)+">(root->getPackets().at(i)->getValue())));\n";
										}
									}
									else if(methpm.at(0)=="Date")
									{
										if(!ptr)
										{
											methods += ("DateFormat formt"+fldnames.at(k+1)+"(\"yyyy-mm-dd hh:mi:ss\");\n");
											methods += ("object.addPacket(formt"+fldnames.at(k+1)+".format(__obj->"+methpm.at(1)+"()),\""+fldnames.at(k+1)+"\");\n");
											//methods += ("DateFormat formt"+fldnames.at(k+1)+"(\"yyyy-mm-dd hh:mi:ss\");\nobjxml += \"<"+fldnames.at(k+1)+" type=\\\""+methpm.at(0)+"\\\">\"+formt"+fldnames.at(k+1)+".format(__obj->"+methpm.at(1)+"())");
											//methods += ("+\"</"+fldnames.at(k+1)+">\";\n");
											//if(methsall[classStructure.getTreatedClassName(true)+"get"+cam+methpm.at(0)])typedefs += "if(nam==\""+fldnames.at(k+1)+"\")\n{\nDateFormat formt"+fldnames.at(k+1)+"(\"yyyy-mm-dd hh:mi:ss\");\n__obj->set"+cam+"(*(formt"+fldnames.at(k+1)+".parse(root->getPackets().at(i)->getValue())));\n}\n";
										}
										else
										{
											methods += ("if(__obj->"+methpm.at(1)+"()!=NULL){DateFormat formt"+fldnames.at(k+1)+"(\"yyyy-mm-dd hh:mi:ss\");\n");
											methods += ("object.addPacket(formt"+fldnames.at(k+1)+".format(*__obj->"+methpm.at(1)+"()),\""+fldnames.at(k+1)+"\");}\n");
											//methods += ("DateFormat formt"+fldnames.at(k+1)+"(\"yyyy-mm-dd hh:mi:ss\");\nobjxml += \"<"+fldnames.at(k+1)+" type=\\\""+methpm.at(0)+"\\\">\"+formt"+fldnames.at(k+1)+".format(__obj->"+methpm.at(1)+"())");
											//methods += ("+\"</"+fldnames.at(k+1)+">\";\n");
											//if(methsall[classStructure.getTreatedClassName(true)+"get"+cam+methpm.at(0)])typedefs += "if(nam==\""+fldnames.at(k+1)+"\")\n{\nDateFormat formt"+fldnames.at(k+1)+"(\"yyyy-mm-dd hh:mi:ss\");\n__obj->set"+cam+"((formt"+fldnames.at(k+1)+".parse(root->getPackets().at(i)->getValue())));\n}\n";
										}
									}
									else if(methpm.at(0)=="BinaryData")
									{
										if(!ptr)
										{
											methods += ("object.addPacket(BinaryData::serilaize(__obj->"+methpm.at(1)+"()),\""+fldnames.at(k+1)+"\");\n");
											//methods += ("objxml += \"<"+fldnames.at(k+1)+" type=\\\""+methpm.at(0)+"\\\">\"+BinaryData::serilaize(__obj->"+methpm.at(1)+"())");
											//methods += ("+\"</"+fldnames.at(k+1)+">\";\n");
											//if(methsall[classStructure.getTreatedClassName(true)+"get"+cam+methpm.at(0)])typedefs += "if(nam==\""+fldnames.at(k+1)+"\")\n__obj->set"+cam+"(*(BinaryData::unSerilaize(root->getPackets().at(i)->getValue())));\n";
										}
										else
										{
											methods += ("if(__obj->"+methpm.at(1)+"()!=NULL)object.addPacket(BinaryData::serilaize(*__obj->"+methpm.at(1)+"()),\""+fldnames.at(k+1)+"\");\n");
											//methods += ("objxml += \"<"+fldnames.at(k+1)+" type=\\\""+methpm.at(0)+"\\\">\"+BinaryData::serilaize(__obj->"+methpm.at(1)+"())");
											//methods += ("+\"</"+fldnames.at(k+1)+">\";\n");
											//if(methsall[classStructure.getTreatedClassName(true)+"get"+cam+methpm.at(0)])typedefs += "if(nam==\""+fldnames.at(k+1)+"\")\n__obj->set"+cam+"((BinaryData::unSerilaize(root->getPackets().at(i)->getValue())));\n";
										}
									}
									else if(methpm.at(0).find("vector")!=string::npos || methpm.at(0).find("queue")!=string::npos ||
											methpm.at(0).find("deque")!=string::npos || methpm.at(0).find("set")!=string::npos ||
											methpm.at(0).find("list")!=string::npos || methpm.at(0).find("multiset")!=string::npos)
									{
										string stlcnt = methpm.at(0);
										string stltyp = methpm.at(0);
										StringUtil::replaceFirst(stltyp,"std::","");
										StringUtil::replaceFirst(stltyp,"<","::");
										StringUtil::replaceFirst(stltyp,">","");
										StringUtil::replaceFirst(stltyp," ","");
										string stlcnttyp = "";
										if(methpm.at(0).find("vector")!=string::npos)
											stlcnttyp = "Vec";
										else if(methpm.at(0).find("queue")!=string::npos)
											stlcnttyp = "Q";
										else if(methpm.at(0).find("deque")!=string::npos)
											stlcnttyp = "Dq";
										else if(methpm.at(0).find("list")!=string::npos)
											stlcnttyp = "Lis";
										else if(methpm.at(0).find("multiset")!=string::npos)
										stlcnttyp = "MulSet";
										else
											stlcnttyp = "Set";
										StringUtil::replaceFirst(stlcnt,"std::","");
										StringUtil::replaceFirst(stlcnt,"vector","");
										StringUtil::replaceFirst(stlcnt,"queue","");
										StringUtil::replaceFirst(stlcnt,"deque","");
										StringUtil::replaceFirst(stlcnt,"multiset","");
										StringUtil::replaceFirst(stlcnt,"set","");
										StringUtil::replaceFirst(stlcnt,"list","");
										StringUtil::replaceFirst(stlcnt,"<","");
										StringUtil::replaceFirst(stlcnt,">","");
										StringUtil::replaceFirst(stlcnt," ","");

										if(!ptr)
										{
											methods += (methpm.at(0)+" __temp_obj_ser"+stlcnt+stlcnttyp+classStructure.getTreatedClassName(true)+methpm.at(1)+" = __obj->"+methpm.at(1)+"();\n");
											if(stlcnt=="int" || stlcnt=="long" || stlcnt=="short" || stlcnt=="float" || stlcnt=="string" || stlcnt=="std::string" || stlcnt=="double" || stlcnt=="bool" || stlcnt=="unsigned int" || stlcnt=="unsigned long" || stlcnt=="unsigned short" || stlcnt=="long long" || stlcnt=="unsigned long long")
												methods += ("object.addPacket(Serialize::serialize<"+methpm.at(0)+" >(__temp_obj_ser"+stlcnt+stlcnttyp+classStructure.getTreatedClassName(true)+methpm.at(1)+",\""+app+"\"),\""+fldnames.at(k+1)+"\");\n");
											else
												methods += ("object.addPacket("+app+"binarySerialize"+stlcnt+stlcnttyp+"(&__temp_obj_ser"+stlcnt+stlcnttyp+classStructure.getTreatedClassName(true)+methpm.at(1)+"),\""+fldnames.at(k+1)+"\");\n");
											//string cam = StringUtil::capitalizedCopy(methpm.at(1));
											//methods += ("+\"</"+nam+">\";\n");
											/*if(methsall[classStructure.getTreatedClassName(true)+"get"+cam+methpm.at(0)])
											{
												typedefs += "if(nam==\""+fldnames.at(k+1)+"\"){";
												typedefs += "\nAMEFEncoder enc;";
												typedefs += "\n__obj->set"+cam+"(*("+methpm.at(0)+"*)"+app+"binaryUnSerialize"+stlcnt+stlcnttyp+"(enc.encodeB(root->getPackets().at(i), false)));\n";
												typedefs += "\n}\n";
											}*/
										}
										else
										{
											methods += (methpm.at(0)+"* __temp_obj_ser"+stlcnt+stlcnttyp+classStructure.getTreatedClassName(true)+methpm.at(1)+" = __obj->"+methpm.at(1)+"();\n");
											if(stlcnt=="int" || stlcnt=="long" || stlcnt=="short" || stlcnt=="float" || stlcnt=="string" || stlcnt=="std::string" || stlcnt=="double" || stlcnt=="bool" || stlcnt=="unsigned int" || stlcnt=="unsigned long" || stlcnt=="unsigned short" || stlcnt=="long long" || stlcnt=="unsigned long long")
												methods += ("if(__obj->"+methpm.at(1)+"()!=NULL)object.addPacket(Serialize::serialize<"+methpm.at(0)+" >(*__temp_obj_ser"+stlcnt+stlcnttyp+classStructure.getTreatedClassName(true)+methpm.at(1)+",\""+app+"\"),\""+fldnames.at(k+1)+"\");\n");
											else
												methods += ("if(__obj->"+methpm.at(1)+"()!=NULL)object.addPacket("+app+"binarySerialize"+stlcnt+stlcnttyp+"(__temp_obj_ser"+stlcnt+stlcnttyp+classStructure.getTreatedClassName(true)+methpm.at(1)+"),\""+fldnames.at(k+1)+"\");\n");
											//string cam = StringUtil::capitalizedCopy(methpm.at(1));
											//methods += ("+\"</"+nam+">\";\n");
											/*if(methsall[classStructure.getTreatedClassName(true)+"get"+cam+methpm.at(0)])
											{
												typedefs += "if(nam==\""+fldnames.at(k+1)+"\"){";
												typedefs += "\nAMEFEncoder enc;";
												typedefs += "\n__obj->set"+cam+"(("+methpm.at(0)+"*)"+app+"binaryUnSerialize"+stlcnt+stlcnttyp+"(enc.encodeB(root->getPackets().at(i), false)));\n";
												typedefs += "\n}\n";
											}*/
										}
										//if(methsall[classStructure.getTreatedClassName(true)+"get"+cam+methpm.at(0)])typedefs += "if(nam==\""+methpm.at(1)+"\")\n__obj->set"+cam+"(*("+methpm.at(0)+"*)"+app+"binaryUnSerialize"+stlcnt+stlcnttyp+"(root.getChildElements().at(i).render()));\n";
									}
									else
									{
										//string cam = StringUtil::capitalizedCopy(methpm.at(1));
										//if(methsall[classStructure.getTreatedClassName(true)+"get"+cam+methpm.at(0)])typedefs += "if(nam==\""+methpm.at(1)+"\")\n__obj->set"+cam+"(*("+methpm.at(0)+"*)"+app+"binaryUnSerialize"+methpm.at(0)+"(root.getChildElements().at(i).render()));\n";
										if(!ptr)
										{
											methods += (methpm.at(0)+" __temp_obj_ser"+methpm.at(1)+" = __obj->"+methpm.at(1)+"();\n");
											methods += ("object.addPacket("+app+"binarySerialize"+methpm.at(0)+"(&__temp_obj_ser"+methpm.at(1)+"),\""+fldnames.at(k+1)+"\");\n");
											//typedefs += "if(nam==\""+methpm.at(1)+"\")\n__obj->set"+cam+"(*("+methpm.at(0)+"*)"+app+"binaryUnSerialize"+methpm.at(0)+"(root->getPackets().at(i)->getValue()));\n";
										}
										else
										{
											methods += (methpm.at(0)+"* __temp_obj_ser"+methpm.at(1)+" = __obj->"+methpm.at(1)+"();\n");
											methods += ("if(__obj->"+methpm.at(1)+"()!=NULL)object.addPacket("+app+"binarySerialize"+methpm.at(0)+"(&__temp_obj_ser"+methpm.at(1)+"),\""+fldnames.at(k+1)+"\");\n");
											//typedefs += "if(nam==\""+methpm.at(1)+"\")\n__obj->set"+cam+"(("+methpm.at(0)+"*)"+app+"binaryUnSerialize"+methpm.at(0)+"(root->getPackets().at(i)->getValue()));\n";
										}
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
		methods += "return enc.encodeB(&object, false);\n}\n";
		methods += "\nstring " +app+ "binarySerialize" + classStructure.getTreatedClassName(true) + "Vec(void* obje)\n{\nvector<"+classStructure.getFullyQualifiedClassName()+"> *__obj=(vector<"+classStructure.getFullyQualifiedClassName()+">*)obje;\n"
				+"string xml;\nAMEFObject object;AMEFEncoder enc;\nfor(unsigned int i=0;i<__obj->size();i++)\n{\nobject.addPacket("+app+"binarySerialize"+classStructure.getTreatedClassName(true)+"(&(__obj->at(i))));\n}\nreturn enc.encodeB(&object, false);\n}\n";
		methods += "\nstring " +app+ "binarySerialize"+classStructure.getTreatedClassName(true)+"Q(void *t){\nstd::queue<"+classStructure.getFullyQualifiedClassName()+"> *_t=(std::queue<"+classStructure.getFullyQualifiedClassName()+">*)t;std::queue<"+classStructure.getFullyQualifiedClassName()+"> *tt = new std::queue<"+classStructure.getFullyQualifiedClassName()+">;	*tt = *_t;"
				+"\nstring objXml;\nAMEFObject object;AMEFEncoder enc;\nfor(unsigned int var=0;var<tt->size();var++){\nobject.addPacket("+app+"binarySerialize"+classStructure.getTreatedClassName(true)+"(&(tt->front())));tt->pop();}\nreturn enc.encodeB(&object, false);\n}";
		methods += "\nstring " +app+ "binarySerialize"+classStructure.getTreatedClassName(true)+"Dq(void *t)\n{\ndeque<"+classStructure.getFullyQualifiedClassName()+"> *_t=(deque<"+classStructure.getFullyQualifiedClassName()+">*)t;"
				+"\nstring objXml;\nAMEFObject object;AMEFEncoder enc;\nfor(unsigned int var=0;var<_t->size();var++){\nobject.addPacket("+app+"binarySerialize"+classStructure.getTreatedClassName(true)+"(&(_t->at(var))));}\nreturn enc.encodeB(&object, false);\n}";
		methods += "\nstring " +app+ "binarySerialize"+classStructure.getTreatedClassName(true)+"Lis(void *_t)\n{\nlist<"+classStructure.getFullyQualifiedClassName()+"> *t=(list<"+classStructure.getFullyQualifiedClassName()+">*)_t;list<"+classStructure.getFullyQualifiedClassName()+">::iterator it;"
				+"string objXml;\nAMEFObject object;AMEFEncoder enc;\nfor(it=t->begin();it!=t->end();++it)	{"+classStructure.getTreatedClassName(true)+" _temp=*it;object.addPacket("+app+"binarySerialize"+classStructure.getTreatedClassName(true)+"(&(_temp)));	}\nreturn  enc.encodeB(&object, false);\n}";
		classes += "\nstring " +app+ "binarySerialize" + classStructure.getTreatedClassName(true) + "Vec(void* obje);\nstring " +app+ "binarySerialize"+classStructure.getTreatedClassName(true)+"Q(void *t);\nstring " +app+ "binarySerialize"+classStructure.getTreatedClassName(true)+"Dq(void *_t);\nstring " +app+ "binarySerialize"+classStructure.getTreatedClassName(true)+"Lis(void *_t);";
		if(classStructure.prosetser)
		{
			methods += "\nstring " +app+ "binarySerialize"+classStructure.getTreatedClassName(true)+"Set(void *_t)\n{\nset<"+classStructure.getFullyQualifiedClassName()+"> *t=(set<"+classStructure.getFullyQualifiedClassName()+">*)_t;set<"+classStructure.getFullyQualifiedClassName()+">::iterator it;"
					+"string objXml;\nAMEFObject object;AMEFEncoder enc;\nfor(it=t->begin();it!=t->end();++it)	{"+classStructure.getTreatedClassName(true)+" _temp=*it;object.addPacket("+app+"binarySerialize"+classStructure.getTreatedClassName(true)+"(&(_temp)));\n}\nreturn  enc.encodeB(&object, false);\n}";
			methods += "\nstring " +app+ "binarySerialize"+classStructure.getTreatedClassName(true)+"MulSet(void *_t)\n{\nmultiset<"+classStructure.getFullyQualifiedClassName()+"> *t=(multiset<"+classStructure.getFullyQualifiedClassName()+">*)_t;multiset<"+classStructure.getFullyQualifiedClassName()+">::iterator it;"
					+"string objXml;\nAMEFObject object;AMEFEncoder enc;\nfor(it=t->begin();it!=t->end();++it)	{"+classStructure.getTreatedClassName(true)+" _temp=*it;object.addPacket("+app+"binarySerialize"+classStructure.getTreatedClassName(true)+"(&(_temp)));\n}\nreturn  enc.encodeB(&object, false);\n}";
			classes += "\nstring " +app+ "binarySerialize"+classStructure.getTreatedClassName(true)+"Set(void *_t);\nstring " +app+ "binarySerialize"+classStructure.getTreatedClassName(true)+"MulSet(void *_t);";
		}

		typedefs += "\n}\nreturn __obj;\n}";
		typedefs += "\nvoid* " +app+ "binaryUnSerialize"+classStructure.getTreatedClassName(true)+"Dq(string objXml){deque<"+classStructure.getFullyQualifiedClassName()+"> *t = new deque<"+classStructure.getFullyQualifiedClassName()+">;"
				 +"\nAMEFDecoder dec;\nAMEFObject* root = dec.decodeB(objXml, true, false);for (int var = 0; var < (int)root->getPackets().size(); var++){"
				 +"t->push_back(*("+classStructure.getFullyQualifiedClassName()+"*)"+app+"binaryUnSerialize"+classStructure.getTreatedClassName(true)+"(root->getPackets().at(var)->getValue()));	}return t;}";
		typedefs += "\nvoid* " +app+ "binaryUnSerialize"+classStructure.getTreatedClassName(true)+"Q(string objXml){std::queue<"+classStructure.getFullyQualifiedClassName()+"> *t = new std::queue<"+classStructure.getFullyQualifiedClassName()+">;"
				 +"\nAMEFDecoder dec;\nAMEFObject* root = dec.decodeB(objXml, true, false);for (int var = 0; var < (int)root->getPackets().size(); var++){"
				 +"t->push(*("+classStructure.getFullyQualifiedClassName()+"*)"+app+"binaryUnSerialize"+classStructure.getTreatedClassName(true)+"(root->getPackets().at(var)->getValue()));	}return t;}";
		typedefs += "\nvoid* " +app+ "binaryUnSerialize"+classStructure.getTreatedClassName(true)+"Lis(string objXml){list<"+classStructure.getFullyQualifiedClassName()+"> *t = new list<"+classStructure.getFullyQualifiedClassName()+">;"
				 +"\nAMEFDecoder dec;\nAMEFObject* root = dec.decodeB(objXml, true, false);for (int var = 0; var < (int)root->getPackets().size(); var++){"
				 +"t->push_back(*("+classStructure.getFullyQualifiedClassName()+"*)"+app+"binaryUnSerialize"+classStructure.getTreatedClassName(true)+"(root->getPackets().at(var)->getValue()));	}return t;}";
		typedefs += "\nvoid* " +app+ "binaryUnSerialize"+classStructure.getTreatedClassName(true)+"Vec(string objXml){vector<"+classStructure.getFullyQualifiedClassName()+"> *t = new vector<"+classStructure.getFullyQualifiedClassName()+">;"
				 +"\nAMEFDecoder dec;\nAMEFObject* root = dec.decodeB(objXml, true, false);for (int var = 0; var < (int)root->getPackets().size(); var++){"
				 +"t->push_back(*("+classStructure.getFullyQualifiedClassName()+"*)"+app+"binaryUnSerialize"+classStructure.getTreatedClassName(true)+"(root->getPackets().at(var)->getValue()));	}return t;}";
		classes += "\nvoid* " +app+ "binaryUnSerialize"+classStructure.getTreatedClassName(true)+"Vec(string objXml);\nvoid* " +app+ "binaryUnSerialize"+classStructure.getTreatedClassName(true)+"Q(string objXml);\nvoid* " +app+ "binaryUnSerialize"+classStructure.getTreatedClassName(true)+"Dq(string objXml);\nvoid* " +app+ "binaryUnSerialize"+classStructure.getTreatedClassName(true)+"Lis(string objXml);";
		if(classStructure.prosetser)
		{
			typedefs += "\nvoid* " +app+ "binaryUnSerialize"+classStructure.getTreatedClassName(true)+"Set(string objXml){set<"+classStructure.getFullyQualifiedClassName()+"> *t = new set<"+classStructure.getFullyQualifiedClassName()+">;"
					 +"\nAMEFDecoder dec;\nAMEFObject* root = dec.decodeB(objXml, true, false);for (int var = 0; var < (int)root->getPackets().size(); var++){"
					 +"t->insert(*("+classStructure.getFullyQualifiedClassName()+"*)"+app+"binaryUnSerialize"+classStructure.getTreatedClassName(true)+"(root->getPackets().at(var)->getValue()));	}return t;}";
			typedefs += "\nvoid* " +app+ "binaryUnSerialize"+classStructure.getTreatedClassName(true)+"MulSet(string objXml){multiset<"+classStructure.getFullyQualifiedClassName()+"> *t = new multiset<"+classStructure.getFullyQualifiedClassName()+">;"
					 +"\nAMEFDecoder dec;\nAMEFObject* root = dec.decodeB(objXml, true, false);for (int var = 0; var < (int)root->getPackets().size(); var++){"
					 +"t->insert(*("+classStructure.getFullyQualifiedClassName()+"*)"+app+"binaryUnSerialize"+classStructure.getTreatedClassName(true)+"(root->getPackets().at(var)->getValue()));	}return t;}";
			classes += "\nvoid* " +app+ "binaryUnSerialize"+classStructure.getTreatedClassName(true)+"Set(string objXml);\nvoid* " +app+ "binaryUnSerialize"+classStructure.getTreatedClassName(true)+"MulSet(string objXml);";
		}

		//typedefs = (structinf+"};\n"+typedefs);
	}
	return refDef;
}
