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

map<string,bool> Reflection::validcls;
//map<string,string> Reflection::clspaths;
map<string,int> Reflection::nmspcIds;
map<string,string> Reflection::nmspcIdVals;

Reflection::Reflection() {
	////logger = //LoggerFactory::getLogger("Reflection");
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
	//logger << ("Searching directory " + cwd + " for pattern .h") << endl;
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
	if(RegexUtil::find(data, "[ \t]*template[ \t]*<[ \t]*")!=-1)
		return;
	RegexUtil::replace(data, "[ \t]*const[; \t]+", "");
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


void Reflection::emptyBlocks(string& data, size_t start)
{
	if(data.find("{")!=string::npos) {
			size_t nstart = data.find("{", start+1);
			if(nstart!=string::npos)
			{
				start = nstart;
			}
			else
			{
				string temp = data.substr(0, start);
				string temp1 = data.substr(start);
				RegexUtil::replace(temp1, "\\{[^}]*\\}", ";");
				data = temp + temp1;
				start = 0;
			}
			emptyBlocks(data, start);
		}
}

void Reflection::handleNamespace(string data, string namepsc, map<string, ClassStructure>& clsvec, map<string, vector<string> >& glbnmspcs)
{
	StringUtil::trim(data);
	if(data=="")return;
	string nmspc;
	int nmspcst = RegexUtil::find(data, "[ \t]*using[ \t]*namespace[ \t]*[^;]+;");
	if(nmspcst!=-1)
	{
		string temp = data.substr(nmspcst);
		nmspcst += temp.find(" namespace");
	}
	if(RegexUtil::find(data, "^[ \t]*using[ \t]*namespace[ \t]*[^;]+;")==0)
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
	else if(data.find(" namespace ")!=string::npos && nmspcst!=(int)data.find(" namespace "))
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
		emptyBlocks(data, 0);
		RegexUtil::replace(data, "\"[^\"]+\"", "\"\"");
		vector<string> parts = StringUtil::split(data, ";");
		if(parts.size()>0)
		{
			string flag = "private";
			for (int var = 0; var < (int)parts.size(); ++var) {
				int pust, puen;
				RegexUtil::find(parts.at(var), "[ \t]*public[ \t]*:", pust, puen);
				int pvst, pven;
				RegexUtil::find(parts.at(var), "[ \t]*private[ \t]*:", pvst, pven);
				int prst, pren;
				RegexUtil::find(parts.at(var), "[ \t]*protected[ \t]*:", prst, pren);

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


map<string, ClassStructure> Reflection::getClassStructures(string className, string appName)
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
				if(RegexUtil::find(data, "[ \t]*virtual[ \t]*[a-zA-Z0-9_]+")!=-1)
				{
					return clsvec;
				}
				allcont.append(data);
				allcont.append(" ");
			}
		}
		infile.close();

		data = allcont;
		RegexUtil::replace(data, "[ \t]*#include[ \t]*\"[^\"]+\"", " ");
		RegexUtil::replace(data, "[ \t]*#include[ \t]*<[^<>]+>", " ");
		RegexUtil::replace(data, "[ \t]*#define[ \t]*[a-zA-Z0-9_]*[ \t]*[0-9]*", " ");
		RegexUtil::replace(data, "[ \t]*#define[ \t]*[a-zA-Z0-9_]*[ \t]*\"[^\"]+\"", " ");
		RegexUtil::replace(data, "[ \t]*#ifndef[ \t]*[a-zA-Z0-9_]*[ \t]*", " ");
		RegexUtil::replace(data, "[ \t]*#endif[ \t]*", " ");

		vector<string> typedefs = RegexUtil::search(data, "[ \t]*typedef[ \t]*[^;]+;");
		RegexUtil::replace(data, "[ \t]*typedef[ \t]*[^;]+;", " ");

		string file = className.substr(className.find_last_of("/")+1);

		map<string, vector<string> > glbnmspcs;
		handleNamespace(data, "", clsvec, glbnmspcs);
		map<string, ClassStructure>::iterator it;
		vector<string> remnmspcs;
		for (it=clsvec.begin();it!=clsvec.end();++it) {

			if(it->second.pub.size()>0)
			{
				it->second.appName = appName;
				it->second.incfile = className;
				clspaths[it->second.getTreatedClassName(true)] = file;
				StringUtil::trim(it->second.nmSpc);
				if(it->second.nmSpc.find("::")!=string::npos)
				{
					it->second.nmSpc = it->second.nmSpc.substr(0, it->second.nmSpc.find_last_of("::")+1);
				}
				if(it->second.nmSpc!="" && nmspcIds.find(appName+it->second.nmSpc)==nmspcIds.end())
				{
					nmspcIds[appName+it->second.nmSpc] = nmspcIds.size()+1;
					string nmspcvv = it->second.nmSpc;
					StringUtil::replaceAll(nmspcvv, "::", ".");
					nmspcIdVals[Reflection::getNameSpaceId(it->second.nmSpc, appName)] = nmspcvv;
				}
				classNamespaces[it->second.nmSpc+it->second.classN] = "YES";
				validcls[appName+it->second.nmSpc+it->second.classN] = true;
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
		for (int var = 0; var < (int)remnmspcs.size(); ++var) {
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
				////logger << results.size() << flush;
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
	////logger << pub.size() << pri.size() << pro.size() << flush;
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
				RegexUtil::replace(meth, "[ \t]*,[ \t]*", ",");
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

string Reflection::generateClassDefinitionsAll(map<string, map<string, ClassStructure> > clsstrucMaps,string &includeRef,vector<string> apps)
{
	string ret = "";
	//includeRef = "#ifndef REFLECTOR_H_\n#define REFLECTOR_H_\n#include \"ClassInfo.h\"\n#include \"string\"\n#include \"Method.h\"\n#include \"Field.h\"\n";
	//includeRef += "#include \"XmlParser.h\"\n#include <stdio.h>\n#include <sys/wait.h>\n#include <stdexcept>\n#include <execinfo.h>\n#include <dlfcn.h>\n#include <cxxabi.h>\n#include <stdio.h>\n#include <stdlib.h>\n#include \"string\"\n#include <sstream>\n#include <typeinfo>\n";
	string typedefs,classes,methods,opers;
	string inc = "#include \"ClassInfo.h\"\n#include \"string\"\n#include \"Method.h\"\n#include \"Field.h\"\n";
	ret += "extern \"C\"\n{\n";
	for (unsigned int var = 0; var < apps.size(); ++var)
	{
		////logger << "\nstarting for classes " << all.size() << "\n" << flush;
		ret += this->generateClassDefinition(clsstrucMaps[StringUtil::trimCopy(apps.at(var))],inc,typedefs,classes,methods,opers,StringUtil::trimCopy(apps.at(var)));

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

string Reflection::generateClassDefinitions(map<string, ClassStructure> allclsmap,string &includesDefs,string &typedefs,string &classes,string &methods,string &opers,string app)
{
	//vector<string> includes = list(includeDir);
	string ret,in,ty,cl,me;
	/*map<string, ClassStructure> allclsmap;
	for (unsigned int var = 0; var < includes.size(); ++var)
	{
		map<string, ClassStructure> clsmap = getClassStructures(includes.at(var));
		allclsmap.insert(clsmap.begin(), clsmap.end());
	}*/
	ret = generateClassDefinition(allclsmap,includesDefs,typedefs,classes,methods,opers,app);
	/*for (unsigned int var = 0; var < includes.size(); ++var)
	{
		////logger << "\ngenerating for file" << includes.at(var) << "\n" << flush;
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
		////logger << "\ndone generating for file" << includes.at(var) << "\n" << flush;
	}*/
	return ret;
}

string Reflection::generateClassDefinition(map<string, ClassStructure> allclsmap,string &includesDefs,string &typedefs,string &classes,string &methods,string &opers,string app)
{
	string refDef, testStr, teststrfuncs;
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

		testStr += "void test"+app+"getReflectionCIFor" + classStructure.getTreatedClassName(true) + "()\n{\n";
		testStr += "ClassInfo classInfo = "+app+"getReflectionCIFor" + classStructure.getTreatedClassName(true) + "();\n";
		testStr += "assert(classInfo.getClassName()==\"" + classStructure.classN+"\");\n";
		testStr += "assert(classInfo.getNamespace()==\"" + classStructure.nmSpc+"\");\n";
		testStr += "assert(classInfo.getBase()==\"" + classStructure.bcvisib + " " + classStructure.baseClassN +"\");\n";
		testStr += ("\nConstructor ctor;\nMethod me;\nField f;\n");
		testStr += ("args argu;\n");

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
					testStr += ("ctor.clear();\nme.clear();\n");
					publm += classStructure.pub.at(i);
					meth = classStructure.pub.at(i);
					StringUtil::replaceFirst(meth,";","");

					string argts = meth.substr(meth.find("("),meth.find(")")-meth.find("("));
					StringUtil::replaceFirst(argts,"(","");
					StringUtil::replaceAll(argts,")","");
					meth = meth.substr(0,meth.find("("));
					StringUtil::trim(meth);
					RegexUtil::replace(meth, "[ \t]*,[ \t]*", ",");

					bool methstat = false;
					if(RegexUtil::find(meth, "[ \t]*static[ \t]+")!=-1 || RegexUtil::find(meth, "[ \t]+static[ \t]+")!=-1)
					{
						RegexUtil::replace(meth, "[ \t]*static[ \t]+", " ");
						RegexUtil::replace(meth, "[ \t]+static[ \t]+", " ");
						methstat = true;
					}
					RegexUtil::replace(meth, "[ \t]*inline[ \t]+", " ");
					RegexUtil::replace(meth, "[ \t]+inline[ \t]+", " ");
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
							size_t spindx = argpmtemp.find_last_of(" ");
							size_t ptindx = argpmtemp.find_last_of("*");
							size_t rfindx = argpmtemp.find_last_of("&");
							size_t teindx = argpmtemp.find_last_of(">");
							if(ptindx==argpmtemp.length()-1 ||
									(ptindx!=string::npos && teindx!=string::npos && ptindx>teindx) ||
									(teindx==string::npos && ptindx!=string::npos && spindx!=string::npos && ptindx>spindx))
							{	typ = "*";
								argpmtemp = argpmtemp.substr(0, ptindx);
							}
							else if(rfindx==argpmtemp.length()-1 ||
									(rfindx!=string::npos && teindx!=string::npos && rfindx>teindx) ||
									(teindx==string::npos && rfindx!=string::npos && spindx!=string::npos && rfindx>spindx))
							{
								typ = "&";
								StringUtil::replaceLast(argpmtemp,"&","");
							}
							if(teindx!=string::npos && spindx!=string::npos && teindx>spindx)
							{
								argpmtemp = argpmtemp.substr(0, teindx);
							}
							else if(spindx!=string::npos)
							{
								argpmtemp = argpmtemp.substr(0, spindx);
							}
							StringUtil::trim(argpmtemp);
							argpm.push_back(argpmtemp+typ);
						}
					}
					string typdefName,methsd,valsd,valsa;
					//bool ctor = false;
					if(methpm.size()>0 && methpm.at(0).find("virtual")!=string::npos)
						return "";
					//for(unsigned int j = 0; j < methpm.size(); j++)
					{
						if(methpm.at(0)==classStructure.getTreatedClassName(false))
						{
							//refDef += ("ctor.setName(\""+classStructure.getTreatedClassName(true)+"\");\n");
							//refDef += ("me.setMethodName(\""+classStructure.getTreatedClassName(true)+methpm.at(j)+"\");\n");
							//ctor = true;
							methsd += (classStructure.getTreatedClassName(true));
						}
						else if(meth.find(" operator")==string::npos)
						{
							string fqcn = getFullyQualifiedClassName(methpm.at(0), classStructure.namespaces);
							refDef += ("me.setReturnType(\""+fqcn+"\");\n");
							typedefs += ("typedef " + methpm.at(0) + " ");
							methsd += (classStructure.getTreatedClassName(true)+methpm.at(1));
						}
						else
						{

							//refDef += (classStructure.getFullyQualifiedClassName() + methpm.at(j)+" = &"+classStructure.getTreatedClassName(true)+"::"+methpm.at(j)+";\n");
							typedefs += ("("+classStructure.getTreatedClassName(true)+"::*"+classStructure.getTreatedClassName(true) +methpm.at(0));
							methsd += (classStructure.getTreatedClassName(true)+methpm.at(1));
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
					int numargs = argpm.size();
					for(unsigned int j = 0; j < argpm.size(); j++)
					{
						vector<string> argtn;
						string type12 = "";
						size_t ptindx = argpm.at(j).find_last_of("*");
						size_t rfindx = argpm.at(j).find_last_of("&");
						if(ptindx==argpm.at(j).length()-1)
							type12 = "*";
						else if(rfindx==argpm.at(j).length()-1)
							type12 = "&";
						//StringUtil::split(argtn, argpm.at(j), (" "));
						////logger << "testing::::" << argpm.at(j) << argtn.size();
						//StringUtil::replaceAll(argtn.at(0)," ","");
						if(meth.find(" operator")==string::npos)
						{
							string fqcn = getFullyQualifiedClassName(argpm.at(j), classStructure.namespaces);
							refDef += ("argu.push_back(\""+fqcn+"\");\n");
							testStr += ("argu.push_back(\""+fqcn+"\");\n");
						}
						if(type12=="*")
						{
							StringUtil::replaceLast(argpm.at(j),"*","");
							string fqcn = getFullyQualifiedClassName(argpm.at(j), classStructure.namespaces);
							valsd += "\t\t"+(fqcn  + " *_" + CastUtil::lexical_cast<string>(j)+" = ("+fqcn+"*)values.at("+CastUtil::lexical_cast<string>(j)+");");
							StringUtil::replaceAll(argpm.at(j),"*","");
						}
						else if(type12=="&")
						{
							StringUtil::replaceLast(argpm.at(j),"&","");
							string fqcn = getFullyQualifiedClassName(argpm.at(j), classStructure.namespaces);
							valsd += "\t\t"+(fqcn + " *_" + CastUtil::lexical_cast<string>(j)+" = ("+fqcn+"*)values.at("+CastUtil::lexical_cast<string>(j)+");");
							StringUtil::replaceAll(argpm.at(j),"&","");
						}
						else
						{
							string fqcn = getFullyQualifiedClassName(argpm.at(j), classStructure.namespaces);
							valsd += "\t\t"+(fqcn + " *_" + CastUtil::lexical_cast<string>(j)+" = ("+fqcn+"*)values.at("+CastUtil::lexical_cast<string>(j)+");");
						}
						if(type12=="*")
							valsa += "_" + CastUtil::lexical_cast<string>(j);
						else
							valsa += "*_" + CastUtil::lexical_cast<string>(j);
						//if(methpm.at(0)!=classStructure.getFullyQualifiedClassName())
						//{
							typedefs += argpm.at(j);
							typdefName += argpm.at(j);
							string fqcn = getTreatedFullyQualifiedClassName(argpm.at(j), classStructure.namespaces);
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
						meth = meth.substr(meth.find(" operator"));
						StringUtil::replaceAll(meth, " ", "");
					}
					if(meth.find("operator<")!=string::npos || meth.find("operator>")!=string::npos || meth.find("operator<=")!=string::npos
							|| meth.find("operator>=")!=string::npos || meth.find("operator==")!=string::npos || meth.find("operator!=")!=string::npos || meth.find("operator!")!=string::npos
							|| meth.find("operator<<")!=string::npos || meth.find("operator>>")!=string::npos || meth.find("operator+")!=string::npos || meth.find("operator-")!=string::npos
							|| meth.find("operator*")!=string::npos || meth.find("operator/")!=string::npos || meth.find("operator[]")!=string::npos || meth.find("operator()")!=string::npos
							|| meth.find("operator&")!=string::npos || meth.find("operator&&")!=string::npos || meth.find("operator||")!=string::npos || meth.find("operator|")!=string::npos)
					{
						if(methpm.at(0).find(" ")!=string::npos)
						{
							methpm.at(0) = methpm.at(0).substr(0, methpm.at(0).find_last_of(" "));
						}
						if(meth.find("<<")!=string::npos)
						{

						}
						else if(meth.find(">>")!=string::npos)
						{

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
						else if(meth.find("<")!=string::npos)
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
						StringUtil::replaceAll(methsd, "::", "_");
						StringUtil::replaceAll(methsd, " ", "");
						StringUtil::replaceAll(methsd, "*", "ptr");
						StringUtil::replaceAll(methsd, "&", "");
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
							methods += "\nvoid* " +app+ "invokeReflectionCIMethodFor"+methsd+"(void* instance,vals values)\n{\n\t";
							if(!methstat)
							{
								methods += classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\n";
							}
							if(methpm.at(0)=="void")
							{
								methods += valsd;
								if(methstat)
								{
									methods += "\n\tvoid* returnValue=NULL;\n\t_"+classStructure.getFullyQualifiedClassName()+"::"+methpm.at(1)+"("+valsa+");";
								}
								else
								{
									methods += "\n\tvoid* returnValue=NULL;\n\t_obj->"+methpm.at(1)+"("+valsa+");";
								}
								methods += "\n\treturn returnValue;";
							}
							else
							{
								string fqcn = getFullyQualifiedClassName(methpm.at(0), classStructure.namespaces);
								methods += "\n\t"+fqcn+" *_retVal=new "+fqcn+";\n";
								methods += valsd;
								if(methstat)
								{
									methods += "\n\t*_retVal = ("+classStructure.getFullyQualifiedClassName()+"::"+methpm.at(1)+"("+valsa+"));";
								}
								else
								{
									methods += "\n\t*_retVal = (_obj->"+methpm.at(1)+"("+valsa+"));";
								}
								methods += "\n\treturn _retVal;";
							}
							methods += "\n}";
							refDef += ("me.setMethodName(\""+methsd+"\");\n");
							methsall[methsd] = true;
							refDef += ("me.setArgumentTypes(argu);\n");
							if(methstat)
							{
								refDef += ("me.setIsStatic(true);\n");
							}
							refDef += ("argu.clear();\n");
							refDef += ("if(me.getMethodName()!=\"\")\n{\nclassInfo.addMethod(me);\n}\n");

							testStr += "me = classInfo.getMethod(\""+methsd+"\", argu);\n";
							testStr += "assert(me.getName()==\"" + methsd +"\");\n";
							if(methstat)
							{
								testStr += "assert(me.isStatic());\n";
							}
							testStr += ("argu.clear();\n");

							testStr += classStructure.getFullyQualifiedClassName()+ " _tstinstance;\n";
							testStr += classStructure.getFullyQualifiedClassName()+ " _rtstinstance = Reflector::newInstance();\n";
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

							testStr += "ctor = classInfo.getConstructor(argu);\n";
							testStr += "assert(ctor.getName()==\"" + methsd +"\");\n";
							testStr += ("argu.clear();\n");

							if(numargs==0)
							{
								ctorisp = true;
							}
						}
					}
				}
				else if(classStructure.pub.at(i).find("~")==string::npos)
				{
					refDef += ("f.clear();\n");
					testStr += ("f.clear();\n");

					publf += classStructure.pub.at(i);

					fld = classStructure.pub.at(i);

					StringUtil::replaceAll(fld, "{", "");
					StringUtil::replaceAll(fld, "}", "");
					StringUtil::trim(fld);
					if(fld.length()==0)continue;

					bool fldstatic = false;
					if(RegexUtil::find(fld, "[ \t]*static[ \t]+")!=-1 || RegexUtil::find(fld, "[ \t]+static[ \t]+")!=-1)
					{
						RegexUtil::replace(fld, "[ \t]*static[ \t]+", " ");
						RegexUtil::replace(fld, "[ \t]+static[ \t]+", " ");
						fldstatic = true;
					}
					RegexUtil::replace(fld, "[ \t]*const[ \t]+", " ");
					RegexUtil::replace(fld, "[ \t]+const[ \t]+", " ");

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

					if(fldp.size()>1)
					{
						for(unsigned int j = 0; j < fldp.size(); j++)
						{
							if(j==0)
							{
								refDef += ("f.setType(\""+fldp.at(j)+"\");\n");
								testStr += "assert(f.getType()==\""+fldp.at(j)+"\");\n";
							}
							else if(j==1)
							{
								refDef += ("f.setFieldName(\""+fldp.at(j)+"\");\n");
								testStr += "assert(f.getFieldName()==\""+fldp.at(j)+"\");\n";
							}
						}
						//if(fldp.size()==2)
						//	structinf += (fldp.at(0)+" "+fldp.at(1)+";\n");
						string fqcn = getFullyQualifiedClassName(fldp.at(0), classStructure.namespaces);
						if(ptr)
						{
							fqcn += "*";
						}
						methods += "\n"+fqcn+" " +app+ "invokeReflectionCIFieldFor"+classStructure.getTreatedClassName(true)+fldp.at(1)+"(void* instance)\n{\n\t"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\n\treturn _obj->"+fldp.at(1)+";\n}\n";
						refDef += ("if(f.getFieldName()!=\"\")\n{\nclassInfo.addField(f);\n}\n");
					}
					else
					{
						////logger << fld << " error" << endl;
					}
				}
				else if(classStructure.pub.at(i).find("~")!=string::npos)
				{
					methods += "\nvoid " +app+ "invokeReflectionCIDtorFor"+classStructure.getTreatedClassName(true)+"(void* instance)\n{";
					methods += "\n\t"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\n\t";
					methods += "_obj->~"+classStructure.getTreatedClassName(false)+"();";
					methods += "\n}";
					ddtorisp = true;
				}
			}
		}
		if (classStructure.pri.size() > 0)
		{
			for (unsigned int i = 0; i < classStructure.pri.size(); i++)
			{
				if((tes=classStructure.pri.at(i).find("("))!=string::npos && (tes=classStructure.pri.at(i).find(")"))!=string::npos)
				{
					//refDef += ("me.clear();\n");
					privm += classStructure.pri.at(i);
					meth = classStructure.pri.at(i);
					StringUtil::replaceFirst(meth,";","");

					string argts = meth.substr(meth.find("("),meth.find(")")-meth.find("("));
					StringUtil::replaceFirst(argts,"(","");
					StringUtil::replaceAll(argts,")","");
					meth = meth.substr(0,meth.find("("));
					StringUtil::trim(meth);
					RegexUtil::replace(meth, "[ \t]*,[ \t]*", ",");

					bool methstat = false;
					if(RegexUtil::find(meth, "[ \t]*static[ \t]+")!=-1 || RegexUtil::find(meth, "[ \t]+static[ \t]+")!=-1)
					{
						RegexUtil::replace(meth, "[ \t]*static[ \t]+", " ");
						RegexUtil::replace(meth, "[ \t]+static[ \t]+", " ");
						methstat = true;
					}
					RegexUtil::replace(meth, "[ \t]*inline[ \t]+", " ");
					RegexUtil::replace(meth, "[ \t]+inline[ \t]+", " ");
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
							argpm.push_back(argpmtemp+typ);
						}
					}
					string typdefName,methsd,valsd,valsa;
					//bool ctor = false;
					if(methpm.size()>0 && methpm.at(0).find("virtual")!=string::npos)
						return "";

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
					int numargs = argpm.size();
					if(classStructure.pri.at(i).find("~")==string::npos && methpm.at(0)==classStructure.getTreatedClassName(false))
					{
						if(numargs==0)
						{
							ctorisp = true;
						}
					}
					else
					{
						ddtorisp = true;
					}
				}
				else if(classStructure.pri.at(i).find("~")==string::npos)
				{
					refDef += ("f.clear();\n");
					testStr += ("f.clear();\n");
					privf += classStructure.pri.at(i);

					fld = classStructure.pri.at(i);

					bool fldstatic = false;
					if(RegexUtil::find(fld, "[ \t]*static[ \t]+")!=-1 || RegexUtil::find(fld, "[ \t]+static[ \t]+")!=-1)
					{
						RegexUtil::replace(fld, "[ \t]*static[ \t]+", " ");
						RegexUtil::replace(fld, "[ \t]+static[ \t]+", " ");
						fldstatic = true;
					}
					RegexUtil::replace(fld, "[ \t]*const[ \t]+", " ");
					RegexUtil::replace(fld, "[ \t]+const[ \t]+", " ");

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

					if(fldp.size()>1)
					{
						for(unsigned int j = 0; j < fldp.size(); j++)
						{
							if(j==0)
							{
								refDef += ("f.setType(\""+fldp.at(j)+"\");\n");
								testStr += "assert(f.getType()==\""+fldp.at(j)+"\");\n";
							}
							else if(j==1)
							{
								refDef += ("f.setFieldName(\""+fldp.at(j)+"\");\n");
								testStr += "assert(f.getFieldName()==\""+fldp.at(j)+"\");\n";
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
						////logger << fld << " error" << endl;
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

					string argts = meth.substr(meth.find("("),meth.find(")")-meth.find("("));
					StringUtil::replaceFirst(argts,"(","");
					StringUtil::replaceAll(argts,")","");
					meth = meth.substr(0,meth.find("("));
					StringUtil::trim(meth);
					RegexUtil::replace(meth, "[ \t]*,[ \t]*", ",");

					bool methstat = false;
					if(RegexUtil::find(meth, "[ \t]*static[ \t]+")!=-1 || RegexUtil::find(meth, "[ \t]+static[ \t]+")!=-1)
					{
						RegexUtil::replace(meth, "[ \t]*static[ \t]+", " ");
						RegexUtil::replace(meth, "[ \t]+static[ \t]+", " ");
						methstat = true;
					}
					RegexUtil::replace(meth, "[ \t]*inline[ \t]+", " ");
					RegexUtil::replace(meth, "[ \t]+inline[ \t]+", " ");
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
							argpm.push_back(argpmtemp+typ);
						}
					}
					string typdefName,methsd,valsd,valsa;
					//bool ctor = false;
					if(methpm.size()>0 && methpm.at(0).find("virtual")!=string::npos)
						return "";

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
					int numargs = argpm.size();
					if(classStructure.pri.at(i).find("~")==string::npos && methpm.at(0)==classStructure.getTreatedClassName(false))
					{
						if(numargs==0)
						{
							ctorisp = true;
						}
					}
					else
					{
						ddtorisp = true;
					}
				}
				else if(classStructure.pro.at(i).find("~")==string::npos)
				{
					refDef += ("f.clear();\n");
					testStr += ("f.clear();\n");

					protf += classStructure.pro.at(i);

					fld = classStructure.pro.at(i);

					bool fldstatic = false;
					if(RegexUtil::find(fld, "[ \t]*static[ \t]+")!=-1 || RegexUtil::find(fld, "[ \t]+static[ \t]+")!=-1)
					{
						RegexUtil::replace(fld, "[ \t]*static[ \t]+", " ");
						RegexUtil::replace(fld, "[ \t]+static[ \t]+", " ");
						fldstatic = true;
					}
					RegexUtil::replace(fld, "[ \t]*const[ \t]+", " ");
					RegexUtil::replace(fld, "[ \t]+const[ \t]+", " ");

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

					if(fldp.size()>1)
					{
						for(unsigned int j = 0; j < fldp.size(); j++)
						{
							if(j==0)
							{
								refDef += ("f.setType(\""+fldp.at(j)+"\");\n");
								testStr += "assert(f.getType()==\""+fldp.at(j)+"\");\n";
							}
							else if(j==1)
							{
								refDef += ("f.setFieldName(\""+fldp.at(j)+"\");\n");
								testStr += "assert(f.getFieldName()==\""+fldp.at(j)+"\");\n";
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
						////logger << fld << " error" << endl;
					}
				}
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

			testStr += ("argu.clear();\n");
			testStr += "ctor = classInfo.getConstructor(argu);\n";
			testStr += "assert(ctor.getName()==\"" + classStructure.getTreatedClassName(true) +"\");\n";
			testStr += ("argu.clear();\n");
		}
		if(!ddtorisp)
		{
			methods += "\nvoid " +app+ "invokeReflectionCIDtorFor"+classStructure.getTreatedClassName(true)+"(void* instance)\n{";
			methods += "\n\t"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\n\t";
			methods += "_obj->~"+classStructure.getTreatedClassName(false)+"();";
			methods += "\n}";
			ddtorisp = true;
		}
		//refDef += ("\nclassInfo.setMeths(meths);");
		//refDef += ("\nclassInfo.setMethods(methVec);");
		//refDef += ("\nclassInfo.setFields(fldVec);");
		refDef += "\nreturn classInfo;\n}\n";
		/*TODO
		refDef += "\nvoid " +app+ "invokeAddToContFor"+classStructure.getTreatedClassName(true)+"(void* _vec,void* _instance,string contType){"
				+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)_instance;\n"
				+ "SerializeBase::addValueToNestedContainer<"+classStructure.getFullyQualifiedClassName()+">(contType, *_obj, _vec);\n}";
		refDef += "\nvoid* " +app+ "invokeGetNewContFor"+classStructure.getTreatedClassName(true)+"(string contType){\nreturn "
				+ "SerializeBase::getNewNestedContainer<"+classStructure.getFullyQualifiedClassName()+">(contType);\n}";
		refDef += "\nint " +app+ "invokeGetContSizeFor"+classStructure.getTreatedClassName(true)+"(void* _vec,string contType){"
				+ "SerializeBase::getNestedContainerSize<vector<"+classStructure.getFullyQualifiedClassName()+"> >(contType, _vec);\n}";
		refDef += "\nvoid* " +app+ "invokeGetContPElementFor"+classStructure.getTreatedClassName(true)+"(void* _vec,int pos,string contType){"
				+classStructure.getFullyQualifiedClassName()+" *_obj = new "+classStructure.getFullyQualifiedClassName()+";\n"
				+ "*_obj = SerializeBase::getValueFromNestedContainer<"+classStructure.getFullyQualifiedClassName()+">(contType, _vec, pos);\nreturn _obj;\n}";
		refDef += "\n" +classStructure.getFullyQualifiedClassName()+" "+app+ "invokeGetContElementFor"+classStructure.getTreatedClassName(true)
				+"(void* _vec,int pos,string contType){"
				+ "return SerializeBase::getValueFromNestedContainer<"+classStructure.getFullyQualifiedClassName()+">(contType, _vec, pos);\n}";
		*/
		//refDef = (structinf+"};\n"+refDef);
	}
	return refDef;
}

string Reflection::generateSerDefinitionAll(map<string, map<string, ClassStructure> > clsstrucMaps,string &includeRef, bool isBinary,string& objs, string& ajaxret, string& headers, string& typerefs,vector<string> apps)
{
	string ret = "";
	includeRef = "\n#include \"vector\"\n#include \"list\"\n#include \"queue\"\n#include \"deque\"\n#include \"set\"\n#include \"DateFormat.h\"\n" ;
	includeRef += "#include \"SerializeBase.h\"\n#include \"sstream\"\n#include \"CastUtil.h\"\n#include <algorithm>\n";
	string typedefs,classes,methods,rert1;
	for (unsigned int var = 0; var < apps.size(); ++var)
	{
		rert1 += this->generateSerDefinitions(clsstrucMaps[StringUtil::trimCopy(apps.at(var))],includeRef,typedefs,classes,methods,isBinary,objs,ajaxret,headers,typerefs,StringUtil::trimCopy(apps.at(var)));
	}
	headers = includeRef;
	includeRef = ("#include \"AfcInclude.h\"\n\nextern \"C\"{\n" + classes + typedefs + methods);
	ret += includeRef;
	ret += "}\n";
	includeRef += typedefs;

	//Ajax JSON Serialization part
	/*string ttem = "Date readDate(JSONElement& obj){DateFormat formt(\"yyyy-mm-dd hh:mi:ss\");\nreturn *formt.parse(obj.getValue());}";
	ttem += "Date* readDateP(JSONElement& obj){DateFormat formt(\"yyyy-mm-dd hh:mi:ss\");\nreturn formt.parse(obj.getValue());}";
	ttem += "\nstring fromDateToJSON(Date d){DateFormat formt(\"yyyy-mm-dd hh:mi:ss\");\nreturn formt.format(d);}";
	ttem += "\nstring fromDateVPToJSON(Date *d){DateFormat formt(\"yyyy-mm-dd hh:mi:ss\");\nreturn formt.format(*d);}";
	ttem += "BinaryData readBinaryData(JSONElement& obj){BinaryData bd;return bd;}";
	ttem += "BinaryData* readBinaryDataP(JSONElement& obj){return new BinaryData;}";
	ttem += "\nstring fromBinaryDataToJSON(BinaryData d){return \"\";}";
	ttem += "\nstring fromBinaryDataVPToJSON(BinaryData *d){return \"\";}";*/
	ajaxret = /*ttem + */typerefs + ajaxret;
	ajaxret ="#include \"AfcInclude.h\"\n\nextern \"C\"\n{\n" + ajaxret;

	return ret;
}

string Reflection::generateSerDefinitions(map<string, ClassStructure> allclsmap,string &includesDefs,string &typedefs,string &classes,string &methods,bool isBinary,
		string& objs, string &ajaxret, string& headers, string& typerefs,string app)
{
	//vector<string> includes = list(includeDir);
	string ret;
	/*map<string, ClassStructure> allclsmap;
	for (unsigned int var = 0; var < includes.size(); ++var)
	{
		map<string, ClassStructure> clsmap = getClassStructures(includes.at(var));
		allclsmap.insert(clsmap.begin(), clsmap.end());
	}*/
	ret = generateAllSerDefinition(allclsmap,includesDefs,typedefs,classes,methods,app);
	//ret += generateSerDefinitionBinary(allclsmap,includesDefs,typedefs,classes,methods,app);

	map<string, ClassStructure>::iterator it;
	for (it=allclsmap.begin();it!=allclsmap.end();++it)
	{
		vector<string> pinfo;
		bool isOpForSet = false;
		vector<string> minfo = getAfcObjectData(it->second,false,pinfo,isOpForSet);
		pinfo.clear();
		vector<string> info = getAfcObjectData(it->second,true,pinfo,isOpForSet);
		//ajaxret += AfcUtil::generateJsObjects(info,it->second,headers,objs,pinfo,isOpForSet,typerefs,minfo,app,getClassPath(it->second.getTreatedClassName(true)),*this);
	}

	/*for (unsigned int var = 0; var < includes.size(); ++var)
	{
		////logger << "\ngenerating Ser for file" << includes.at(var) << "\n" << flush;
		if(invalidcls.find(includes.at(var))==invalidcls.end())
		{

			vector<string> pinfo;
			bool isOpForSet = false;
			vector<string> minfo = getAfcObjectData(includes.at(var),false,pinfo,isOpForSet);
			pinfo.clear();
			vector<string> info = getAfcObjectData(includes.at(var),true,pinfo,isOpForSet);
			ajaxret += AfcUtil::generateJsObjects(info,this->classN,headers,includeDir,objs,pinfo,isOpForSet,typerefs,minfo,app,getClassPath(getTreatedClassName(true)));
		}
		////logger << "\ndone generating Ser for file" << includes.at(var) << "\n" << flush;
	}*/
	return ret;
}

string Reflection::generateAllSerDefinition(map<string, ClassStructure> allclsmap,string &includesDefs,string &typedefs,string &classes,string &methods,string app)
{
	string refDef;
	map<string, ClassStructure>::iterator it;
	for (it=allclsmap.begin();it!=allclsmap.end();++it)
	{
		ClassStructure classStructure = it->second;
		includesDefs += "#include \"" + getClassPath(it->second.getTreatedClassName(true)) + "\"\n";
		classStructure.prosetser = false;
		classes += "\nstring " +app+ "serialize" + classStructure.getTreatedClassName(true)
				+ "(void* obje, SerializeBase* base);\nvoid* " +app+ "unSerialize" + classStructure.getTreatedClassName(true) + "(void* intermediateObject, SerializeBase* base);";
		methods += "\nstring " +app+ "serialize" + classStructure.getTreatedClassName(true)
				+ "(void* obje, SerializeBase* base)\n{\n"+classStructure.getFullyQualifiedClassName()+" *__obj=("+classStructure.getFullyQualifiedClassName()+"*)obje"
				+";\nvoid* serobject = base->getSerializableObject();\nbase->startObjectSerialization(serobject, \""
				+classStructure.getTreatedClassName(true)+"\");\n";
		typedefs += "\nvoid* " +app+ "unSerialize" + classStructure.getTreatedClassName(true) + "(void* intermediateObject, SerializeBase* base)\n{\n";
		typedefs += classStructure.getFullyQualifiedClassName()+" *__obj=new " + classStructure.getFullyQualifiedClassName()
				 +";\nif(!base->isValidClassNamespace(intermediateObject, \""
				 +classStructure.getTreatedClassName(false)+"\", \""+classStructure.nmSpc+"\"))\n\treturn NULL;\n";
		typedefs += "int totsize = base->getContainerSize(intermediateObject);\nfor(int i=0;i<totsize;i++)\n{\n";

		string publf, privf, protf ,publm, privm, protm;
		string meth,fld;
		size_t tes;
		vector<string> fldnames;
		map<string, bool> fldptr;
		map<string, int> fldstat;
		if (classStructure.pri.size() > 0)
		{
			for (unsigned int i = 0; i < classStructure.pri.size(); i++)
			{
				if(((tes=classStructure.pri.at(i).find("("))==string::npos &&
						(tes=classStructure.pri.at(i).find(")"))==string::npos && classStructure.pri.at(i).find("~")==string::npos))
				{
					fld = classStructure.pri.at(i);

					bool fldstatic = false;
					if(RegexUtil::find(fld, "[ \t]*static[ \t]+")!=-1 || RegexUtil::find(fld, "[ \t]+static[ \t]+")!=-1)
					{
						RegexUtil::replace(fld, "[ \t]*static[ \t]+", " ");
						RegexUtil::replace(fld, "[ \t]+static[ \t]+", " ");
						fldstatic = true;
					}
					RegexUtil::replace(fld, "[ \t]*const[ \t]+", " ");
					RegexUtil::replace(fld, "[ \t]+const[ \t]+", " ");

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
		}
		if (classStructure.pro.size() > 0)
		{
			for (unsigned int i = 0; i < classStructure.pro.size(); i++)
			{
				if(((tes=classStructure.pro.at(i).find("("))==string::npos
						&& (tes=classStructure.pro.at(i).find(")"))==string::npos && classStructure.pro.at(i).find("~")==string::npos))
				{
					fld = classStructure.pro.at(i);

					bool fldstatic = false;
					if(RegexUtil::find(fld, "[ \t]*static[ \t]+")!=-1 || RegexUtil::find(fld, "[ \t]+static[ \t]+")!=-1)
					{
						RegexUtil::replace(fld, "[ \t]*static[ \t]+", " ");
						RegexUtil::replace(fld, "[ \t]+static[ \t]+", " ");
						fldstatic = true;
					}
					RegexUtil::replace(fld, "[ \t]*const[ \t]+", " ");
					RegexUtil::replace(fld, "[ \t]+const[ \t]+", " ");

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
		}
		if (classStructure.pub.size() > 0)
		{
			for (unsigned int i = 0; i < classStructure.pub.size(); i++)
			{
				if((tes=classStructure.pub.at(i).find("("))!=string::npos && (tes=classStructure.pub.at(i).find(")"))!=string::npos && classStructure.pub.at(i).find("~")==string::npos
					&& fldnames.size()>0)
				{
					meth = classStructure.pub.at(i);
					StringUtil::replaceFirst(meth,";","");

					string argts = meth.substr(meth.find("("),meth.find(")")-meth.find("("));
					StringUtil::replaceFirst(argts,"(","");
					StringUtil::replaceAll(argts,")","");
					meth = meth.substr(0,meth.find("("));
					StringUtil::trim(meth);
					RegexUtil::replace(meth, "[ \t]*,[ \t]*", ",");

					bool methstat = false;
					if(RegexUtil::find(meth, "[ \t]*static[ \t]+")!=-1 || RegexUtil::find(meth, "[ \t]+static[ \t]+")!=-1)
					{
						RegexUtil::replace(meth, "[ \t]*static[ \t]+", " ");
						RegexUtil::replace(meth, "[ \t]+static[ \t]+", " ");
						methstat = true;
					}
					RegexUtil::replace(meth, "[ \t]*inline[ \t]+", " ");
					RegexUtil::replace(meth, "[ \t]+inline[ \t]+", " ");
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
					{
						if(meth.find("operator>")!=string::npos && meth.find("operator>=")==string::npos && meth.find("operator>>")==string::npos)
						{
							classStructure.prosetser = true;
						}
						else if(meth.find("operator<")!=string::npos && meth.find("operator<=")==string::npos && meth.find("operator<<")==string::npos)
						{
							classStructure.prosetser = true;
						}
					}
					else
					{
						if(methpm.at(0)!=classStructure.getTreatedClassName(false))
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
		}
		if (classStructure.pub.size() > 0)
		{
			for (unsigned int i = 0; i < classStructure.pub.size(); i++)
			{
				if(((tes=classStructure.pub.at(i).find("("))==string::npos
						&& (tes=classStructure.pub.at(i).find(")"))==string::npos && classStructure.pub.at(i).find("~")==string::npos))
				{
					fld = classStructure.pub.at(i);

					bool fldstatic = false;
					if(RegexUtil::find(fld, "[ \t]*static[ \t]+")!=-1 || RegexUtil::find(fld, "[ \t]+static[ \t]+")!=-1)
					{
						RegexUtil::replace(fld, "[ \t]*static[ \t]+", " ");
						RegexUtil::replace(fld, "[ \t]+static[ \t]+", " ");
						fldstatic = true;
					}
					RegexUtil::replace(fld, "[ \t]*const[ \t]+", " ");
					RegexUtil::replace(fld, "[ \t]+const[ \t]+", " ");

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
						if(isPrimitiveDataType(fldp.at(0)))
						{
							fldp.at(0) = getTypeName(fldp.at(0));
							if(!ptr)
							{
								methods += fldp.at(0) + " _objProp" + fldp.at(1) + " = " + "__obj->"+fldp.at(1) + ";\n";
								methods += "base->addObjectPrimitiveProperty(serobject, \""+fldp.at(1)+"\", \""+fldp.at(0)+"\", &_objProp"+fldp.at(1)+");\n"
										+"base->afterAddObjectProperty(serobject);\n";
								string cam = AfcUtil::camelCased(fldp.at(1));
								typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldp.at(1)+"\", i))\n{\n"
										+fldp.at(0)+"* _val = ("+fldp.at(0)+"*)base->getObjectPrimitiveValue(base->getObjectProperty(intermediateObject, i), \""+fldp.at(0)+"\", \""+fldp.at(1)+"\");__obj->"+fldp.at(1)
										+" = *_val;\ndelete _val;\n}\n";
							}
							else
							{
								methods += "base->addObjectPrimitiveProperty(serobject, \""+fldp.at(1)+"\", \""+fldp.at(0)+"\", __obj->"+fldp.at(1)+");\n"
										+"base->afterAddObjectProperty(serobject);\n";
								string cam = AfcUtil::camelCased(fldp.at(1));
								typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldp.at(1)+"\", i))\n{\n"
										+fldp.at(0)+"* _val = ("+fldp.at(0)+"*)base->getObjectPrimitiveValue(base->getObjectProperty(intermediateObject, i), \""+fldp.at(0)+"\", \""+fldp.at(1)+"\");__obj->"+fldp.at(1)
										+" = _val;\n}\n";
							}
						}
						else if(fldp.at(0)=="Date")
						{
							if(!ptr)
							{
								methods += fldp.at(0) + " _objProp" + fldp.at(1) + " = " + "__obj->"+fldp.at(1) + ";\n";
								methods += ("base->addObjectPrimitiveProperty(serobject, \""+fldp.at(1)+"\", \""
										+fldp.at(0)+"\", &_objProp"+fldp.at(1)+");\n"
										+"base->afterAddObjectProperty(serobject);\n");
								string cam = AfcUtil::camelCased(fldp.at(1));
								typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldp.at(1)+"\", i))\n{\n"
										+"string* _val = (string*)base->getObjectPrimitiveValue(base->getObjectProperty(intermediateObject, i), \"std::string\", \""+fldp.at(1)+"\");\nDateFormat formt"
										+fldp.at(1)+"(\"yyyy-mm-dd hh:mi:ss\");\n__obj->"
										+fldp.at(1)+" = *(formt"+fldp.at(1)+".parse(*_val));\ndelete _val;\n}\n";
							}
							else
							{
								methods += ("base->addObjectPrimitiveProperty(serobject, \""+fldp.at(1)+"\", \""
										+fldp.at(0)+"\", __obj->"+fldp.at(1)+");\n"
										+"base->afterAddObjectProperty(serobject);\n");
								string cam = AfcUtil::camelCased(fldp.at(1));
								typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldp.at(1)+"\", i))\n{\n"
										+"string* _val = (string*)base->getObjectPrimitiveValue(base->getObjectProperty(intermediateObject, i), \"std::string\", \""+fldp.at(1)+"\");\nDateFormat formt"
										+fldp.at(1)+"(\"yyyy-mm-dd hh:mi:ss\");\n__obj->"
										+fldp.at(1)+" = (formt"+fldp.at(1)+".parse(*_val));\ndelete _val;\n}\n";
							}
						}
						else if(fldp.at(0)=="BinaryData")
						{
							if(!ptr)
							{
								methods += fldp.at(0) + " _objProp" + fldp.at(1) + " = " + "__obj->"+fldp.at(1) + ";\n";
								methods += ("base->addObjectPrimitiveProperty(serobject, \""+fldp.at(1)+"\", \""
										+fldp.at(0)+"\", &_objProp"+fldp.at(1)+");\n"
										+"base->afterAddObjectProperty(serobject);\n");
								string cam = AfcUtil::camelCased(fldp.at(1));
								typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldp.at(1)+"\", i))\n{\n"
										+"string* _val = (string*)base->getObjectPrimitiveValue(base->getObjectProperty(intermediateObject, i), \"std::string\", \""+fldp.at(1)+"\");"
										+ "\n__obj->"+fldp.at(1)+" = *(BinaryData::unSerilaize(*_val));\ndelete _val;\n}\n";
							}
							else
							{
								methods += ("base->addObjectPrimitiveProperty(serobject, \""+fldp.at(1)+"\", \""
										+fldp.at(0)+"\", __obj->"+fldp.at(1)+");\n"
										+"base->afterAddObjectProperty(serobject);\n");
								string cam = AfcUtil::camelCased(fldp.at(1));
								typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldp.at(1)+"\", i))\n{\n"
										+"string* _val = (string*)base->getObjectPrimitiveValue(base->getObjectProperty(intermediateObject, i), \"std::string\", \""+fldp.at(1)+"\");"
										+ "\n__obj->"+fldp.at(1)+" = (BinaryData::unSerilaize(*_val));\ndelete _val;\n}\n";
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
							//StringUtil::replaceFirst(stlcnt," ","");

							string fqcn = contType + getFullyQualifiedClassName(stlcnt, classStructure.namespaces) + " >";
							contType += stlcnt + ",";

							if(!ptr)
							{
								methods += (fqcn+" __temp_obj_ser"+fldp.at(1)+" = __obj->"+fldp.at(1)+";\n");
								methods += ("base->addObjectProperty(serobject, \""+fldp.at(1)+"\", \""+fldp.at(0)+"\", "
										+ "SerializeBase::serialize<"+fqcn+" >(__temp_obj_ser"+fldp.at(1)+",\""+app+"\", base));\n"
										+"base->afterAddObjectProperty(serobject);\n");
								string cam = AfcUtil::camelCased(fldp.at(1));
								typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldp.at(1)+"\", i))\n__obj->"+fldp.at(1)+" = "
										 + "SerializeBase::unSerialize<"+fqcn+" >(base->getContainerElement("
										 + "intermediateObject, i, 0),\""+app+"\", base);\n";
							}
							else
							{
								methods += (fqcn+"* __temp_obj_ser"+fldp.at(1)+" = __obj->"+fldp.at(1)+";\n");
								methods += ("if(__obj->"+fldp.at(1)+"!=NULL)base->addObjectProperty(serobject, \""+fldp.at(1)+"\", \""+fldp.at(0)+"\", "
										+ "SerializeBase::serializePointer<"+fqcn+" >(__temp_obj_ser"+fldp.at(1)+",\""+app+"\", base));\n"
										+"base->afterAddObjectProperty(serobject);\n");
								string cam = AfcUtil::camelCased(fldp.at(1));
								typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldp.at(1)+"\", i))\n__obj->"+fldp.at(1)+" = "
										 + "SerializeBase::unSerializeToPointer<"+fqcn+" >(base->getContainerElement("
										 +"intermediateObject, i, 0),\""+app+"\", base);\n";
							}
						}
						else
						{
							string fqcn = getFullyQualifiedClassName(fldp.at(0), classStructure.namespaces);
							if(!ptr)
							{
								methods += (fqcn+" __temp_obj_ser"+fldp.at(1)+" = __obj->"+fldp.at(1)+";\n");
								methods += ("base->addObjectProperty(serobject, \""+fldp.at(1)+"\", \""+fldp.at(0)+"\", "
										+ "SerializeBase::serialize<"+fqcn+" >(__temp_obj_ser"+fldp.at(1)+",\""+app+"\", base));\n"
										+"base->afterAddObjectProperty(serobject);\n");
								string cam = AfcUtil::camelCased(fldp.at(1));
								typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldp.at(1)+"\", i))\n__obj->"+fldp.at(1)+" = "
										 + "SerializeBase::unSerialize<"+fqcn+" >(base->getContainerElement("
										 +"intermediateObject, i, 0),\""+app+"\", base);\n";
							}
							else
							{
								methods += (fldp.at(0)+"* __temp_obj_ser"+fldp.at(1)+" = __obj->"+fldp.at(1)+";\n");
								methods += ("if(__obj->"+fldp.at(1)+"!=NULL)base->addObjectProperty(serobject, \""+fldp.at(1)+"\", \""+fldp.at(0)+"\", "
										+ "SerializeBase::serializePointer<"+fqcn+" >(__temp_obj_ser"+fldp.at(1)+",\""+app+"\", base));\n"
										+"base->afterAddObjectProperty(serobject);\n");
								string cam = AfcUtil::camelCased(fldp.at(1));
								typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldp.at(1)+"\", i))\n__obj->"+fldp.at(1)+" = "
										 + "SerializeBase::unSerializeToPointer<"+fqcn+" >(base->getContainerElement("
										 +"intermediateObject, i, 0),\""+app+"\", base);\n";
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

					string argts = meth.substr(meth.find("("),meth.find(")")-meth.find("("));
					StringUtil::replaceFirst(argts,"(","");
					StringUtil::replaceAll(argts,")","");
					meth = meth.substr(0,meth.find("("));
					StringUtil::trim(meth);
					RegexUtil::replace(meth, "[ \t]*,[ \t]*", ",");

					bool methstat = false;
					if(RegexUtil::find(meth, "[ \t]*static[ \t]+")!=-1 || RegexUtil::find(meth, "[ \t]+static[ \t]+")!=-1)
					{
						RegexUtil::replace(meth, "[ \t]*static[ \t]+", " ");
						RegexUtil::replace(meth, "[ \t]+static[ \t]+", " ");
						methstat = true;
					}
					RegexUtil::replace(meth, "[ \t]*inline[ \t]+", " ");
					RegexUtil::replace(meth, "[ \t]+inline[ \t]+", " ");
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
					{
						if(meth.find("operator>")!=string::npos && meth.find("operator>=")==string::npos && meth.find("operator>>")==string::npos)
						{
							classStructure.prosetser = true;
						}
						else if(meth.find("operator<")!=string::npos && meth.find("operator<=")==string::npos && meth.find("operator<<")==string::npos)
						{
							classStructure.prosetser = true;
						}
					}
					else
					{
						if(methpm.at(0)!=classStructure.getTreatedClassName(false))
						{
							for(unsigned int k = 0; k < fldnames.size(); k=k+2)
							{
								if(fldstat[fldnames.at(k+1)]!=2 && ptr==fldptr[fldnames.at(k+1)])
									continue;
								ptr = fldptr[fldnames.at(k+1)];
								string cam = AfcUtil::camelCased(fldnames.at(k+1));
								if("set"+cam==methpm.at(1) && argpm.size()==1 && argpm.at(0)==fldnames.at(k) && methpm.at(0)=="void")
								{
									if(isPrimitiveDataType(argpm.at(0)))
									{
										argpm.at(0) = getTypeName(argpm.at(0));
										if(!ptr)
											typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldnames.at(k+1)+"\", i))\n{\n"
													 +argpm.at(0)+"* _val = ("+argpm.at(0)+"*)base->getObjectPrimitiveValue(base->getObjectProperty(intermediateObject, i), \""
													 +argpm.at(0)+"\", \""+fldnames.at(k+1)+"\");\n__obj->"+methpm.at(1)+"(*_val);\ndelete _val;\n}\n";
										else
											typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldnames.at(k+1)+"\", i))\n{\n"
													 +argpm.at(0)+"* _val = ("+argpm.at(0)+"*)base->getObjectPrimitiveValue(base->getObjectProperty(intermediateObject, i), \""
													 +argpm.at(0)+"\", \""+fldnames.at(k+1)+"\");\n__obj->"+methpm.at(1)+"(_val);\n}\n";
									}
									else if(argpm.at(0)=="Date")
									{
										if(!ptr)
											typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldnames.at(k+1)+"\", i))\n{\n"
													 +"string* _val = (string*)base->getObjectPrimitiveValue(base->getObjectProperty(intermediateObject, i), \"std::string\", \""+fldnames.at(k+1)+"\");\n"
													 +"DateFormat formt"+cam+"(\"yyyy-mm-dd hh:mi:ss\");\n__obj->"
													 +methpm.at(1)+"(*(formt"+cam+".parse(*_val)));\ndelete _val;\n}\n";
										else
											typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldnames.at(k+1)+"\", i))\n{\n"
													 +"string* _val = (string*)base->getObjectPrimitiveValue(base->getObjectProperty(intermediateObject, i), \"std::string\", \""+fldnames.at(k+1)+"\");\n"
													 +"DateFormat formt"+cam+"(\"yyyy-mm-dd hh:mi:ss\");\n__obj->"
													 +methpm.at(1)+"(formt"+cam+".parse(*_val));\ndelete _val;\n}\n";
									}
									else if(argpm.at(0)=="BinaryData")
									{
										if(!ptr)
											typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldnames.at(k+1)+"\", i))\n{\n"
													+"string* _val = (string*)base->getObjectPrimitiveValue(base->getObjectProperty(intermediateObject, i), \"std::string\", \""+fldnames.at(k+1)+"\");"
													+ "\n__obj->"+methpm.at(1)+"(*(BinaryData::unSerilaize(*_val)));\ndelete _val;\n}\n";
										else
											typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldnames.at(k+1)+"\", i))\n{\n"
													+"string* _val = (string*)base->getObjectPrimitiveValue(base->getObjectProperty(intermediateObject, i), \"std::string\", \""+fldnames.at(k+1)+"\");"
													+ "\n__obj->"+methpm.at(1)+"((BinaryData::unSerilaize(*_val)));\ndelete _val;\n}\n";
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
										//StringUtil::replaceFirst(stlcnt," ","");

										string fqcn = contType + getFullyQualifiedClassName(stlcnt, classStructure.namespaces) + ">";
										contType += stlcnt + ",";

										if(!ptr)
										{
											typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldnames.at(k+1)+"\", i)){";
											typedefs += "\n__obj->set"+cam+"(SerializeBase::unSerialize<"+fqcn+" >"
													 + "(base->getContainerElement(intermediateObject, i, 0),\""+app+"\", base));\n";
											typedefs += "\n}\n";
										}
										else
										{
											typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldnames.at(k+1)+"\", i)){";
											typedefs += "\n__obj->set"+cam+"(SerializeBase::unSerializeToPointer<"+fqcn+" >"
													 + "(base->getContainerElement(intermediateObject, i, 0),\""+app+"\", base));\n";
											typedefs += "\n}\n";
										}
									}
									else
									{
										string fqcn = getFullyQualifiedClassName(argpm.at(0), classStructure.namespaces);
										if(!ptr)
											typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldnames.at(k+1)+"\", i))"
													 + "\n__obj->"+methpm.at(1)+"(SerializeBase::unSerialize<"+fqcn+" >"
													 + "(base->getContainerElement(intermediateObject, i, 0),\""+app+"\", base));\n";
										else
											typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldnames.at(k+1)+"\", i))"
													 + "\n__obj->"+methpm.at(1)+"(SerializeBase::unSerializeToPointer<"+fqcn+" >"
													 + "(base->getContainerElement(intermediateObject, i, 0),\""+app+"\", base));\n";
									}
								}
								else if("get"+cam==methpm.at(1) && argpm.size()==0 && methpm.at(0)==fldnames.at(k))
								{
									if(isPrimitiveDataType(methpm.at(0)))
									{
										methpm.at(0) = getTypeName(methpm.at(0));
										if(!ptr)
										{
											methods += methpm.at(0) + " _objProp" + fldnames.at(k+1) + " = " + "__obj->"+methpm.at(1) + "();\n";
											methods += ("base->addObjectPrimitiveProperty(serobject, \""+fldnames.at(k+1)+"\", \""+methpm.at(0)+"\", &_objProp"+fldnames.at(k+1)+");\n"
													+"base->afterAddObjectProperty(serobject);\n");
										}
										else
										{
											methods += ("if(__obj->"+methpm.at(1)+"()!=NULL)base->addObjectPrimitiveProperty(serobject, \""+fldnames.at(k+1)+"\", \""+methpm.at(0)+"\", "
													+"__obj->"+methpm.at(1)+"());\n"
													+"base->afterAddObjectProperty(serobject);\n");
										}
									}
									else if(methpm.at(0)=="Date")
									{
										if(!ptr)
										{
											methods += methpm.at(0) + " _objProp" + fldnames.at(k+1) + " = " + "__obj->"+methpm.at(1) + "();\n";
											methods += ("base->addObjectPrimitiveProperty(serobject, \""
													+fldnames.at(k+1)+"\", \""+methpm.at(0)+"\", &_objProp"+fldnames.at(k+1)+");\n"
													+"base->afterAddObjectProperty(serobject);\n");
										}
										else
										{
											methods += ("if(__obj->"+methpm.at(1)+"()!=NULL){\n"
													+"base->addObjectPrimitiveProperty(serobject, \""+fldnames.at(k+1)+"\", \""+methpm.at(0)+"\", "
													+"__obj->"+methpm.at(1)+"());\n"
													+"base->afterAddObjectProperty(serobject);\n");
										}
									}
									else if(methpm.at(0)=="BinaryData")
									{
										if(!ptr)
										{
											methods += methpm.at(0) + " _objProp" + fldnames.at(k+1) + " = " + "__obj->"+methpm.at(1) + "();\n";
											methods += ("base->addObjectPrimitiveProperty(serobject, \""+fldnames.at(k+1)+"\", \""+methpm.at(0)+"\", "
													+"&_objProp"+fldnames.at(k+1)+");\n"
													+"base->afterAddObjectProperty(serobject);\n");
										}
										else
										{
											methods += ("if(__obj->"+methpm.at(1)+"()!=NULL)base->addObjectPrimitiveProperty(serobject, \""+fldnames.at(k+1)+"\", \""+methpm.at(0)+"\", "
													"__obj->"+methpm.at(1)+"());\n"
													+"base->afterAddObjectProperty(serobject);\n");
										}
									}
									else if(methpm.at(0).find("vector")!=string::npos || methpm.at(0).find("queue")!=string::npos ||
											methpm.at(0).find("deque")!=string::npos || methpm.at(0).find("set")!=string::npos ||
											methpm.at(0).find("list")!=string::npos || methpm.at(0).find("multiset")!=string::npos)
									{
										string contType;
										string stlcnt = methpm.at(0);
										string stltyp = methpm.at(0);
										StringUtil::replaceFirst(stltyp,"std::","");
										StringUtil::replaceFirst(stltyp,"<","::");
										StringUtil::replaceFirst(stltyp,">","");
										StringUtil::replaceFirst(stltyp," ","");
										string stlcnttyp = "";
										if(methpm.at(0).find("vector")!=string::npos)
										{
											contType = "std::vector<";
											stlcnttyp = "Vec";
										}
										else if(methpm.at(0).find("queue")!=string::npos)
										{
											contType = "std::queue<";
											stlcnttyp = "Q";
										}
										else if(methpm.at(0).find("deque")!=string::npos)
										{
											contType = "std::deque<";
											stlcnttyp = "Dq";
										}
										else if(methpm.at(0).find("list")!=string::npos)
										{
											contType = "std::list<";
											stlcnttyp = "Lis";
										}
										else if(methpm.at(0).find("multiset")!=string::npos)
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
										//StringUtil::replaceFirst(stlcnt," ","");

										string fqcn = contType + getFullyQualifiedClassName(stlcnt, classStructure.namespaces) + " >";
										string stlcontwosp = methpm.at(0);
										StringUtil::trim(stlcontwosp);

										if(!ptr)
										{
											methods += (fqcn+" __temp_obj_ser"+fldnames.at(k+1)+" = __obj->"+methpm.at(1)+"();\n");
											methods += ("base->addObjectProperty(serobject, \""+fldnames.at(k+1)+"\", \""+stlcontwosp+"\", "
													+"SerializeBase::serialize<"+fqcn+" >(__temp_obj_ser"+fldnames.at(k+1)+",\""+app+"\", base));\n"
													+"base->afterAddObjectProperty(serobject);\n");
										}
										else
										{
											methods += (fqcn+"* __temp_obj_ser"+fldnames.at(k+1)+" = __obj->"+methpm.at(1)+"();\n");
											methods += ("if(__obj->"+methpm.at(1)+"()!=NULL)base->addObjectProperty(serobject, \""+fldnames.at(k+1)+"\", \""+stlcontwosp+"\", "
													+"SerializeBase::serializePointer<"+fqcn+" >(__temp_obj_ser"+fldnames.at(k+1)+",\""+app+"\", base));\n"
													+"base->afterAddObjectProperty(serobject);\n");
										}
									}
									else
									{
										string fqcn = getFullyQualifiedClassName(methpm.at(0), classStructure.namespaces);
										if(!ptr)
										{
											methods += (methpm.at(0)+" __temp_obj_ser"+fldnames.at(k+1)+" = __obj->"+methpm.at(1)+"();\n");
											methods += ("base->addObjectProperty(serobject, \""+fldnames.at(k+1)+"\", \""+methpm.at(0)+"\", "
													+ "SerializeBase::serialize<"+fqcn+" >(__temp_obj_ser"+fldnames.at(k+1)+",\""+app+"\", base));\n"
													+"base->afterAddObjectProperty(serobject);\n");
										}
										else
										{
											methods += (methpm.at(0)+"* __temp_obj_ser"+fldnames.at(k+1)+" = __obj->"+methpm.at(1)+"();\n");
											methods += ("if(__obj->"+methpm.at(1)+"()!=NULL)base->addObjectProperty(serobject, \""+fldnames.at(k+1)+"\", \""+methpm.at(0)+"\", "
													+"SerializeBase::serializePointer<"+fqcn+" >(__temp_obj_ser"+fldnames.at(k+1)+",\""+app+"\", base));\n"
													+"base->afterAddObjectProperty(serobject);\n");
										}
									}
								}
							}
						}
					}
				}
			}
		}

		methods += "base->endObjectSerialization(serobject, \""+classStructure.getTreatedClassName(true)+"\");\n"
				+"string ser = base->fromSerializableObjectToString(serobject);\nbase->cleanSerializableObject(serobject);\nreturn ser;\n}\n";
		methods += "\nstring " +app+ "serialize" + classStructure.getTreatedClassName(true) + "Vec(void* obje, SerializeBase* base)\n{\nvector<"
				+classStructure.getFullyQualifiedClassName()+"> *__obj=(vector<"+classStructure.getFullyQualifiedClassName()+">*)obje;\n"
				+"void* object = base->getSerializableObject();\nbase->startContainerSerialization(object, \""+classStructure.getTreatedClassName(true)+"\", \"std::vector\");\n"
				+"int cnt = 0;\nint size = __obj->size();\nstd::vector<"+classStructure.getFullyQualifiedClassName()+">::iterator it = __obj->begin();\n"
				+"for(it = __obj->begin(); it!= __obj->end(); ++it)\n{\nbase->addContainerSerializableElement(object, "+app+"serialize"
				+classStructure.getTreatedClassName(true)+"(&(*it), base));\nbase->afterAddContainerSerializableElement(object, cnt++, size);\n}\n"
				+"base->endContainerSerialization(object, \""+classStructure.getTreatedClassName(true)+"\", \"std::vector\");\nstring ser = base->fromSerializableObjectToString(object);\n"
				+"base->cleanSerializableObject(object);\nreturn ser;\n}\n";
		methods += "\nstring " +app+ "serialize" + classStructure.getTreatedClassName(true) + "Dq(void* obje, SerializeBase* base)\n{\ndeque<"
				+classStructure.getFullyQualifiedClassName()+"> *__obj=(deque<"+classStructure.getFullyQualifiedClassName()+">*)obje;\n"
				+"void* object = base->getSerializableObject();\nbase->startContainerSerialization(object, \""+classStructure.getTreatedClassName(true)+"\", \"std::deque\");\n"
				+"int cnt = 0;\nint size = __obj->size();\nstd::deque<"+classStructure.getFullyQualifiedClassName()+">::iterator it = __obj->begin();\n"
				+"for(it = __obj->begin(); it!= __obj->end(); ++it)\n{\nbase->addContainerSerializableElement(object, "+app+"serialize"
				+classStructure.getTreatedClassName(true)+"(&(*it), base));\nbase->afterAddContainerSerializableElement(object, cnt++, size);\n}\n"
				+"base->endContainerSerialization(object, \""+classStructure.getTreatedClassName(true)+"\", \"std::deque\");\nstring ser = base->fromSerializableObjectToString(object);\n"
				+"base->cleanSerializableObject(object);\nreturn ser;\n}\n";
		methods += "\nstring " +app+ "serialize" + classStructure.getTreatedClassName(true) + "Lis(void* obje, SerializeBase* base)\n{\nlist<"
				+classStructure.getFullyQualifiedClassName()+"> *__obj=(list<"+classStructure.getFullyQualifiedClassName()+">*)obje;\n"
				+"void* object = base->getSerializableObject();\nbase->startContainerSerialization(object, \""+classStructure.getTreatedClassName(true)+"\", \"std::list\");\n"
				+"int cnt = 0;\nint size = __obj->size();\nstd::list<"+classStructure.getFullyQualifiedClassName()+">::iterator it = __obj->begin();\n"
				+"for(it = __obj->begin(); it!= __obj->end(); ++it)\n{\nbase->addContainerSerializableElement(object, "+app+"serialize"
				+classStructure.getTreatedClassName(true)+"(&(*it), base));\nbase->afterAddContainerSerializableElement(object, cnt++, size);\n}\n"
				+"base->endContainerSerialization(object, \""+classStructure.getTreatedClassName(true)+"\", \"std::list\");\nstring ser = base->fromSerializableObjectToString(object);\n"
				+"base->cleanSerializableObject(object);\nreturn ser;\n}\n";
		methods += "\nstring " +app+ "serialize" + classStructure.getTreatedClassName(true) + "Q(void* obje, SerializeBase* base)\n{\nstd::queue<"
				+classStructure.getFullyQualifiedClassName()+"> *__obj=(std::queue<"+classStructure.getFullyQualifiedClassName()+">*)obje;\nstd::queue<"+
				classStructure.getFullyQualifiedClassName()+"> *tt = new std::queue<"+classStructure.getFullyQualifiedClassName()
				+">;	*tt = *__obj;void* object = base->getSerializableObject();\nbase->startContainerSerialization(object, \""+classStructure.getTreatedClassName(true)+"\", \"std::queue\");\n"
				+"int cnt = 0;\nint size = __obj->size();\n"
				+"for(int var=0;var<size;var++)\n{\nbase->addContainerSerializableElement(object, "+app+"serialize"
				+classStructure.getTreatedClassName(true)+"(&(tt->front()), base));\ntt->pop();\nbase->afterAddContainerSerializableElement(object, cnt++, size);\n}\n"
				+"base->endContainerSerialization(object, \""+classStructure.getTreatedClassName(true)+"\", \"std::queue\");\ndelete tt;\nstring ser = base->fromSerializableObjectToString(object);\n"
				+"base->cleanSerializableObject(object);\nreturn ser;\n}\n";
		classes += "\nstring " +app+ "serialize"+classStructure.getTreatedClassName(true)+"Vec(void* obje, SerializeBase* base);\n"
				+"string " +app+ "serialize"+classStructure.getTreatedClassName(true)+"Lis(void* obje, SerializeBase* base);\n"
				+"string " +app+ "serialize"+classStructure.getTreatedClassName(true)+"Dq(void* obje, SerializeBase* base);\n"
				+"string " +app+ "serialize"+classStructure.getTreatedClassName(true)+"Q(void* obje, SerializeBase* base);\n";
		if(classStructure.prosetser)
		{
			methods += "\nstring " +app+ "serialize" + classStructure.getTreatedClassName(true) + "Set(void* obje, SerializeBase* base)\n{\nset<"
					+classStructure.getFullyQualifiedClassName()+"> *__obj=(set<"+classStructure.getFullyQualifiedClassName()+">*)obje;\n"
					+"void* object = base->getSerializableObject();\nbase->startContainerSerialization(object, \""+classStructure.getTreatedClassName(true)+"\", \"std::set\");\n"
					+"int cnt = 0;\nint size = __obj->size();\nstd::set<"+classStructure.getFullyQualifiedClassName()+">::iterator it = __obj->begin();\n"
					+"for(it = __obj->begin(); it!= __obj->end(); ++it)\n{\nbase->addContainerSerializableElement(object, "+app+"serialize"
					+classStructure.getTreatedClassName(true)+"((void*)&(*it), base));\nbase->afterAddContainerSerializableElement(object, cnt++, size);\n}\n"
					+"base->endContainerSerialization(object, \""+classStructure.getTreatedClassName(true)+"\", \"std::set\");\nstring ser = base->fromSerializableObjectToString(object);\n"
					+"base->cleanSerializableObject(object);\nreturn ser;\n}\n";
			methods += "\nstring " +app+ "serialize" + classStructure.getTreatedClassName(true) + "MulSet(void* obje, SerializeBase* base)\n{\nmultiset<"
					+classStructure.getFullyQualifiedClassName()+"> *__obj=(multiset<"+classStructure.getFullyQualifiedClassName()+">*)obje;\n"
					+"void* object = base->getSerializableObject();\nbase->startContainerSerialization(object, \""+classStructure.getTreatedClassName(true)+"\", \"std::multiset\");\n"
					+"int cnt = 0;\nint size = __obj->size();\nstd::multiset<"+classStructure.getFullyQualifiedClassName()+">::iterator it = __obj->begin();\n"
					+"for(it = __obj->begin(); it!= __obj->end(); ++it)\n{\nbase->addContainerSerializableElement(object, "+app+"serialize"
					+classStructure.getTreatedClassName(true)+"((void*)&(*it), base));\nbase->afterAddContainerSerializableElement(object, cnt++, size);\n}\n"
					+"base->endContainerSerialization(object, \""+classStructure.getTreatedClassName(true)+"\", \"std::multiset\");\nstring ser = base->fromSerializableObjectToString(object);\n"
					+"base->cleanSerializableObject(object);\nreturn ser;\n}\n";
			classes += "\nstring " +app+ "serialize"+classStructure.getTreatedClassName(true)+"Set(void* obje, SerializeBase* base);\n"
					+"string " +app+ "serialize"+classStructure.getTreatedClassName(true)+"MulSet(void* obje, SerializeBase* base);";
		}

		typedefs += "\n}\nreturn __obj;\n}\n";
		typedefs += "\nvoid* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"Dq(void* cintobject, SerializeBase* base){\ndeque<"
				 +classStructure.getFullyQualifiedClassName()+"> *t = new deque<"+classStructure.getFullyQualifiedClassName()
				 +">;\nif(!base->isValidClassNamespace(cintobject, \"std::deque<"
				 +classStructure.getTreatedClassName(false)+">\", \""+classStructure.nmSpc+"\", true))\n\treturn NULL;\n"
				 +"int totsize = base->getContainerSize(cintobject);\nfor(int i=0;i<totsize;i++)\n{\n"
				 +"void* ciintobject = base->getContainerElement(cintobject, i);\n"
				 +"if(ciintobject!=NULL)\n{\nt->push_back(*("
				 +classStructure.getFullyQualifiedClassName()+"*)"+app+"unSerialize"+classStructure.getTreatedClassName(true)
				 +"(ciintobject, base));\n}\n}\nreturn t;\n}\n";
		typedefs += "\nvoid* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"Q(void* cintobject, SerializeBase* base){\nstd::queue<"
				 +classStructure.getFullyQualifiedClassName()+"> *t = new std::queue<"+classStructure.getFullyQualifiedClassName()
				 +">;\nif(!base->isValidClassNamespace(cintobject, \"std::queue<"
				 +classStructure.getTreatedClassName(false)+">\", \""+classStructure.nmSpc+"\", true))\n\treturn NULL;\n"
				 +"int totsize = base->getContainerSize(cintobject);\nfor(int i=0;i<totsize;i++)\n{\n"
				 +"void* ciintobject = base->getContainerElement(cintobject, i);\n"
				 +"if(ciintobject!=NULL)\n{\nt->push(*("
				 +classStructure.getFullyQualifiedClassName()+"*)"+app+"unSerialize"+classStructure.getTreatedClassName(true)
				 +"(ciintobject, base));\n}\n}\nreturn t;\n}\n";
		typedefs += "\nvoid* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"Lis(void* cintobject, SerializeBase* base){\nlist<"
				 +classStructure.getFullyQualifiedClassName()+"> *t = new std::list<"+classStructure.getFullyQualifiedClassName()
				 +">;\nif(!base->isValidClassNamespace(cintobject, \"std::list<"
				 +classStructure.getTreatedClassName(false)+">\", \""+classStructure.nmSpc+"\", true))\n\treturn NULL;\n"
				 +"int totsize = base->getContainerSize(cintobject);\nfor(int i=0;i<totsize;i++)\n{\n"
				 +"void* ciintobject = base->getContainerElement(cintobject, i);\n"
				 +"if(ciintobject!=NULL)\n{\nt->push_back(*("
				 +classStructure.getFullyQualifiedClassName()+"*)"+app+"unSerialize"+classStructure.getTreatedClassName(true)
				 +"(ciintobject, base));\n}\n}\nreturn t;\n}\n";
		typedefs += "\nvoid* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"Vec(void* cintobject, SerializeBase* base){\nvector<"
				 +classStructure.getFullyQualifiedClassName()+"> *t = new std::vector<"+classStructure.getFullyQualifiedClassName()
				 +">;\nif(!base->isValidClassNamespace(cintobject, \"std::vector<"
				 +classStructure.getTreatedClassName(false)+">\", \""+classStructure.nmSpc+"\", true))\n\treturn NULL;\n"
				 +"int totsize = base->getContainerSize(cintobject);\nfor(int i=0;i<totsize;i++)\n{\n"
				 +"void* ciintobject = base->getContainerElement(cintobject, i);\n"
				 +"if(ciintobject!=NULL)\n{\nt->push_back(*("
				 +classStructure.getFullyQualifiedClassName()+"*)"+app+"unSerialize"+classStructure.getTreatedClassName(true)
				 +"(ciintobject, base));\n}\n}\nreturn t;\n}\n";
		classes += "\nvoid* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"Vec(void* cintobject, SerializeBase* base);\n"
				+"void* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"Q(void* cintobject, SerializeBase* base);\n"
				+"void* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"Dq(void* cintobject, SerializeBase* base);\n"
				+"void* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"Lis(void* cintobject, SerializeBase* base);\n";
		if(classStructure.prosetser)
		{
			typedefs += "\nvoid* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"Set(void* cintobject, SerializeBase* base){\nset<"
					 +classStructure.getFullyQualifiedClassName()+"> *t = new std::set<"+classStructure.getFullyQualifiedClassName()
					 +">;\nif(!base->isValidClassNamespace(cintobject, \"std::set<"
					 +classStructure.getTreatedClassName(false)+">\", \""+classStructure.nmSpc+"\", true))\n\treturn NULL;\n"
					 +"int totsize = base->getContainerSize(cintobject);\nfor(int i=0;i<totsize;i++)\n{\n"
					 +"void* ciintobject = base->getContainerElement(cintobject, i);\n"
					 +"if(ciintobject!=NULL)\n{\nt->insert(*("
					 +classStructure.getFullyQualifiedClassName()+"*)"+app+"unSerialize"+classStructure.getTreatedClassName(true)
					 +"(ciintobject, base));\n}\n}\nreturn t;\n}\n";
			typedefs += "\nvoid* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"MulSet(void* cintobject, SerializeBase* base){\nmultiset<"
					 +classStructure.getFullyQualifiedClassName()+"> *t = new std::multiset<"+classStructure.getFullyQualifiedClassName()
					 +">;if(!base->isValidClassNamespace(cintobject, \"std::multiset<"
					 +classStructure.getTreatedClassName(false)+">\", \""+classStructure.nmSpc+"\", true))\n\treturn NULL;\n"
					 +"int totsize = base->getContainerSize(cintobject);\nfor(int i=0;i<totsize;i++)\n{\n"
					 +"void* ciintobject = base->getContainerElement(cintobject, i);\n"
					 +"if(ciintobject!=NULL)\n{\nt->insert(*("
					 +classStructure.getFullyQualifiedClassName()+"*)"+app+"unSerialize"+classStructure.getTreatedClassName(true)
					 +"(ciintobject, base));\n}\n}\nreturn t;\n}\n";
			classes += "\nvoid* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"Set(void* cintobject, SerializeBase* base);\nvoid* "
					+app+ "unSerialize"+classStructure.getTreatedClassName(true)+"MulSet(void* cintobject, SerializeBase* base);\n";
		}
	}
	return refDef;
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
		classes += "\nstring " +app+ "serialize" + classStructure.getTreatedClassName(true)
				+ "(void* obje);\nvoid* " +app+ "unSerialize" + classStructure.getTreatedClassName(true) + "(string objXml);";
		methods += "\nstring " +app+ "serialize" + classStructure.getTreatedClassName(true)
				+ "(void* obje)\n{\n"+classStructure.getFullyQualifiedClassName()+" *__obj=("+classStructure.getFullyQualifiedClassName()+"*)obje;\n";
		methods += "string objxml = \"<"+classStructure.getTreatedClassName(false)+" namespace=\\\""+classStructure.nmSpc+"\\\">\";\n";
		typedefs += "\nvoid* " +app+ "unSerialize" + classStructure.getTreatedClassName(true) + "(string objXml)\n{\n";
		typedefs += classStructure.getFullyQualifiedClassName()+" *__obj=new " + classStructure.getFullyQualifiedClassName()
				+";\nXmlParser parser(\"Parser\");\nElement root = parser.getDocument(objXml).getRootElement();\nif(root.getTagName()!=\""
				+classStructure.getTreatedClassName(false)+"\" || root.getAttribute(\"namespace\")!=\""+classStructure.nmSpc+"\")\nreturn NULL;\n";
		typedefs += "for(unsigned int i=0;i<root.getChildElements().size();i++)\n{\n";
		typedefs += "string nam=root.getChildElements().at(i).getTagName();\n";

		string publf, privf, protf ,publm, privm, protm;
		string meth,fld;
		size_t tes;
		vector<string> fldnames;
		map<string, bool> fldptr;
		map<string, int> fldstat;
		if (classStructure.pri.size() > 0)
		{
			for (unsigned int i = 0; i < classStructure.pri.size(); i++)
			{
				if(((tes=classStructure.pri.at(i).find("("))==string::npos &&
						(tes=classStructure.pri.at(i).find(")"))==string::npos && classStructure.pri.at(i).find("~")==string::npos))
				{
					fld = classStructure.pri.at(i);

					bool fldstatic = false;
					if(RegexUtil::find(fld, "[ \t]*static[ \t]+")!=-1 || RegexUtil::find(fld, "[ \t]+static[ \t]+")!=-1)
					{
						RegexUtil::replace(fld, "[ \t]*static[ \t]+", " ");
						RegexUtil::replace(fld, "[ \t]+static[ \t]+", " ");
						fldstatic = true;
					}
					RegexUtil::replace(fld, "[ \t]*const[ \t]+", " ");
					RegexUtil::replace(fld, "[ \t]+const[ \t]+", " ");

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
		}
		if (classStructure.pro.size() > 0)
		{
			for (unsigned int i = 0; i < classStructure.pro.size(); i++)
			{
				if(((tes=classStructure.pro.at(i).find("("))==string::npos
						&& (tes=classStructure.pro.at(i).find(")"))==string::npos && classStructure.pro.at(i).find("~")==string::npos))
				{
					fld = classStructure.pro.at(i);

					bool fldstatic = false;
					if(RegexUtil::find(fld, "[ \t]*static[ \t]+")!=-1 || RegexUtil::find(fld, "[ \t]+static[ \t]+")!=-1)
					{
						RegexUtil::replace(fld, "[ \t]*static[ \t]+", " ");
						RegexUtil::replace(fld, "[ \t]+static[ \t]+", " ");
						fldstatic = true;
					}
					RegexUtil::replace(fld, "[ \t]*const[ \t]+", " ");
					RegexUtil::replace(fld, "[ \t]+const[ \t]+", " ");

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
		}
		if (classStructure.pub.size() > 0)
		{
			for (unsigned int i = 0; i < classStructure.pub.size(); i++)
			{
				if(((tes=classStructure.pub.at(i).find("("))==string::npos
						&& (tes=classStructure.pub.at(i).find(")"))==string::npos && classStructure.pub.at(i).find("~")==string::npos))
				{
					fld = classStructure.pub.at(i);

					bool fldstatic = false;
					if(RegexUtil::find(fld, "[ \t]*static[ \t]+")!=-1 || RegexUtil::find(fld, "[ \t]+static[ \t]+")!=-1)
					{
						RegexUtil::replace(fld, "[ \t]*static[ \t]+", " ");
						RegexUtil::replace(fld, "[ \t]+static[ \t]+", " ");
						fldstatic = true;
					}
					RegexUtil::replace(fld, "[ \t]*const[ \t]+", " ");
					RegexUtil::replace(fld, "[ \t]+const[ \t]+", " ");

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
						if(isPrimitiveDataType(fldp.at(0)))
						{
							fldp.at(0) = getTypeName(fldp.at(0));
							if(!ptr)
							{
								methods += ("objxml += \"<"+fldp.at(1)+" type=\\\""+fldp.at(0)+"\\\">\"+CastUtil::lexical_cast<string>(__obj->"+fldp.at(1)+")");
								string cam = AfcUtil::camelCased(fldp.at(1));
								methods += ("+\"</"+nam+">\";\n");
								typedefs += "if(nam==\""+fldp.at(1)+"\")\n__obj->"+fldp.at(1)
										+" = CastUtil::lexical_cast<"+fldp.at(0)+">(root.getChildElements().at(i).getText());\n";
							}
							else
							{
								methods += ("if(__obj->"+fldp.at(1)+"!=NULL)objxml += \"<"+fldp.at(1)+" type=\\\""+
										fldp.at(0)+"\\\">\"+CastUtil::lexical_cast<string>(*__obj->"+fldp.at(1)+")");
								string cam = AfcUtil::camelCased(fldp.at(1));
								methods += ("+\"</"+nam+">\";\n");
								typedefs += "if(nam==\""+fldp.at(1)+"\")\n__obj->"+fldp.at(1)
										+" = new "+fldp.at(0)+"(CastUtil::lexical_cast<"+fldp.at(0)+">(root.getChildElements().at(i).getText()));\n";
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
							//StringUtil::replaceFirst(stlcnt," ","");

							string fqcn = contType + getFullyQualifiedClassName(stlcnt, classStructure.namespaces) + ">";
							contType += stlcnt + ",";

							if(!ptr)
							{
								methods += (fqcn+" __temp_obj_ser"+stlcnt+stlcnttyp+classStructure.classN+fldp.at(1)+" = __obj->"+fldp.at(1)+";\n");
								//if(stlcnt=="int" || stlcnt=="long" || stlcnt=="short" || stlcnt=="float" || stlcnt=="string" || stlcnt=="std::string" || stlcnt=="double" || stlcnt=="bool" || stlcnt=="unsigned int" || stlcnt=="unsigned long" || stlcnt=="unsigned short" || stlcnt=="long long" || stlcnt=="unsigned long long")
									methods += ("objxml += \"<"+fldp.at(1)+" type=\\\""+stltyp+"\\\">\"+XMLSerialize::serialize<"+fqcn+" >(__temp_obj_ser"+stlcnt+stlcnttyp+classStructure.classN+fldp.at(1)+",\""+app+"\")");
								//else
								//	methods += ("objxml += \"<"+fldp.at(1)+" type=\\\""+stltyp+"\\\">\"+"+app+"serialize"+stlcnt+stlcnttyp+"(&__temp_obj_ser"+stlcnt+stlcnttyp+classStructure.classN+fldp.at(1)+")");
								string cam = AfcUtil::camelCased(fldp.at(1));
								methods += ("+\"</"+nam+">\";\n");
								//if(stlcnt=="int" || stlcnt=="long" || stlcnt=="short" || stlcnt=="float" || stlcnt=="string" || stlcnt=="std::string" || stlcnt=="double" || stlcnt=="bool" || stlcnt=="unsigned int" || stlcnt=="unsigned long" || stlcnt=="unsigned short" || stlcnt=="long long" || stlcnt=="unsigned long long")
									typedefs += "if(nam==\""+fldp.at(1)+"\")\n__obj->"+fldp.at(1)+" = XMLSerialize::unserialize<"+fqcn+" >(root.getChildElements().at(i).renderChildren(),\""+app+"\");\n";
								//else
								//	typedefs += "if(nam==\""+fldp.at(1)+"\")\n__obj->"+fldp.at(1)+" = *("+fqcn+"*)"+app+"unSerialize"+stlcnt+stlcnttyp+"(root.getChildElements().at(i).renderChildren());\n";
							}
							else
							{
								methods += (fqcn+"* __temp_obj_ser"+stlcnt+stlcnttyp+classStructure.classN+fldp.at(1)+" = __obj->"+fldp.at(1)+";\n");
								//if(stlcnt=="int" || stlcnt=="long" || stlcnt=="short" || stlcnt=="float" || stlcnt=="string" || stlcnt=="std::string" || stlcnt=="double" || stlcnt=="bool" || stlcnt=="unsigned int" || stlcnt=="unsigned long" || stlcnt=="unsigned short" || stlcnt=="long long" || stlcnt=="unsigned long long")
									methods += ("if(__obj->"+fldp.at(1)+"!=NULL)objxml += \"<"+fldp.at(1)+" type=\\\""+stltyp+"\\\">\"+XMLSerialize::serializePointer<"+fqcn+" >(__temp_obj_ser"+stlcnt+stlcnttyp+classStructure.classN+fldp.at(1)+",\""+app+"\")");
								//else
								//	methods += ("if(__obj->"+fldp.at(1)+"!=NULL)objxml += \"<"+fldp.at(1)+" type=\\\""+stltyp+"\\\">\"+"+app+"serialize"+stlcnt+stlcnttyp+"(__temp_obj_ser"+stlcnt+stlcnttyp+classStructure.classN+fldp.at(1)+")");
								string cam = AfcUtil::camelCased(fldp.at(1));
								methods += ("+\"</"+nam+">\";\n");
								//if(stlcnt=="int" || stlcnt=="long" || stlcnt=="short" || stlcnt=="float" || stlcnt=="string" || stlcnt=="std::string" || stlcnt=="double" || stlcnt=="bool" || stlcnt=="unsigned int" || stlcnt=="unsigned long" || stlcnt=="unsigned short" || stlcnt=="long long" || stlcnt=="unsigned long long")
									typedefs += "if(nam==\""+fldp.at(1)+"\")\n__obj->"+fldp.at(1)+" = ("+fqcn+"*)XMLSerialize::unserializeToPointer<"+fqcn+" >(root.getChildElements().at(i).renderChildren(),\""+app+"\");\n";
								//else
								//	typedefs += "if(nam==\""+fldp.at(1)+"\")\n__obj->"+fldp.at(1)+" = ("+fldp.at(0)+"*)"+app+"unSerialize"+stlcnt+stlcnttyp+"(root.getChildElements().at(i).renderChildren());\n";
							}
						}
						else
						{
							string fqcn = getFullyQualifiedClassName(fldp.at(0), classStructure.namespaces);
							if(!ptr)
							{
								methods += (fqcn+" __temp_obj_ser"+fldp.at(1)+" = __obj->"+fldp.at(1)+";\n");
								methods += ("objxml += \"<"+fldp.at(1)+" type=\\\""+fqcn+"\\\">\"+XMLSerialize::serialize<"+fqcn+" >(__temp_obj_ser"+fldp.at(1)+",\""+app+"\")");
								string cam = AfcUtil::camelCased(fldp.at(1));
								methods += ("+\"</"+nam+">\";\n");
								typedefs += "if(nam==\""+fldp.at(1)+"\")\n__obj->"+fldp.at(1)+" = XMLSerialize::unserialize<"+fqcn+" >(root.getChildElements().at(i).renderChildren(),\""+app+"\");\n";
							}
							else
							{
								methods += (fldp.at(0)+"* __temp_obj_ser"+fldp.at(1)+" = __obj->"+fldp.at(1)+";\n");
								methods += ("if(__obj->"+fldp.at(1)+"!=NULL)objxml += \"<"+fldp.at(1)+" type=\\\""+fldp.at(0)+"\\\">\"+XMLSerialize::serializePointer<"+fqcn+" >(__temp_obj_ser"+fldp.at(1)+",\""+app+"\")");
								string cam = AfcUtil::camelCased(fldp.at(1));
								methods += ("+\"</"+nam+">\";\n");
								typedefs += "if(nam==\""+fldp.at(1)+"\")\n__obj->"+fldp.at(1)+" = ("+fqcn+"*)XMLSerialize::unserializeToPointer<"+fqcn+" >(root.getChildElements().at(i).renderChildren(),\""+app+"\");\n";
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

					string argts = meth.substr(meth.find("("),meth.find(")")-meth.find("("));
					StringUtil::replaceFirst(argts,"(","");
					StringUtil::replaceAll(argts,")","");
					meth = meth.substr(0,meth.find("("));
					StringUtil::trim(meth);
					RegexUtil::replace(meth, "[ \t]*,[ \t]*", ",");

					bool methstat = false;
					if(RegexUtil::find(meth, "[ \t]*static[ \t]+")!=-1 || RegexUtil::find(meth, "[ \t]+static[ \t]+")!=-1)
					{
						RegexUtil::replace(meth, "[ \t]*static[ \t]+", " ");
						RegexUtil::replace(meth, "[ \t]+static[ \t]+", " ");
						methstat = true;
					}
					RegexUtil::replace(meth, "[ \t]*inline[ \t]+", " ");
					RegexUtil::replace(meth, "[ \t]+inline[ \t]+", " ");
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
					{
						if(meth.find("operator>")!=string::npos && meth.find("operator>=")==string::npos && meth.find("operator>>")==string::npos)
						{
							classStructure.prosetser = true;
						}
						else if(meth.find("operator<")!=string::npos && meth.find("operator<=")==string::npos && meth.find("operator<<")==string::npos)
						{
							classStructure.prosetser = true;
						}
					}
					else
					{
						if(methpm.at(0)!=classStructure.getTreatedClassName(false))
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

						if(methpm.at(0)!=classStructure.getTreatedClassName(false))
						{
							for(unsigned int k = 0; k < fldnames.size(); k=k+2)
							{
								if(fldstat[fldnames.at(k+1)]!=2 && ptr==fldptr[fldnames.at(k+1)])
									continue;
								ptr = fldptr[fldnames.at(k+1)];
								string cam = AfcUtil::camelCased(fldnames.at(k+1));
								if("set"+cam==methpm.at(1) && argpm.size()==1 && argpm.at(0)==fldnames.at(k) && methpm.at(0)=="void")
								{
									if(isPrimitiveDataType(argpm.at(0)))
									{
										argpm.at(0) = getTypeName(argpm.at(0));
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
										//StringUtil::replaceFirst(stlcnt," ","");

										string fqcn = contType + getFullyQualifiedClassName(stlcnt, classStructure.namespaces) + ">";
										contType += stlcnt + ",";

										if(!ptr)
										{
											typedefs += "if(nam==\""+fldnames.at(k+1)+"\"){";
											//if(stlcnt=="int" || stlcnt=="long" || stlcnt=="short" || stlcnt=="float" || stlcnt=="string" || stlcnt=="std::string" || stlcnt=="double" || stlcnt=="bool" || stlcnt=="unsigned int" || stlcnt=="unsigned long" || stlcnt=="unsigned short" || stlcnt=="long long" || stlcnt=="unsigned long long")
												typedefs += "\n__obj->set"+cam+"(XMLSerialize::unserialize<"+fqcn+" >(root.getChildElements().at(i).renderChildren(),\""+app+"\"));\n";
											//else
											//	typedefs += "\n__obj->set"+cam+"(*("+fqcn+"*)"+app+"unSerialize"+stlcnt+stlcnttyp+"(root.getChildElements().at(i).renderChildren()));\n";
											typedefs += "\n}\n";
										}
										else
										{
											typedefs += "if(nam==\""+fldnames.at(k+1)+"\"){";
											//if(stlcnt=="int" || stlcnt=="long" || stlcnt=="short" || stlcnt=="float" || stlcnt=="string" || stlcnt=="std::string" || stlcnt=="double" || stlcnt=="bool" || stlcnt=="unsigned int" || stlcnt=="unsigned long" || stlcnt=="unsigned short" || stlcnt=="long long" || stlcnt=="unsigned long long")
												typedefs += "\n__obj->set"+cam+"(XMLSerialize::unserializeToPointer<"+fqcn+" >(root.getChildElements().at(i).renderChildren(),\""+app+"\"));\n";
											//else
											//	typedefs += "\n__obj->set"+cam+"(("+fqcn+"*)"+app+"unSerialize"+stlcnt+stlcnttyp+"(root.getChildElements().at(i).renderChildren()));\n";
											typedefs += "\n}\n";
										}
									}
									else
									{
										string fqcn = getFullyQualifiedClassName(argpm.at(0), classStructure.namespaces);
										if(!ptr)
											typedefs += "if(nam==\""+fldnames.at(k+1)+"\")\n__obj->"+methpm.at(1)+"(XMLSerialize::unserialize<"+fqcn+" >(root.getChildElements().at(i).renderChildren(),\""+app+"\"));\n";
										else
											typedefs += "if(nam==\""+fldnames.at(k+1)+"\")\n__obj->"+methpm.at(1)+"(XMLSerialize::unserializeToPointer<"+fqcn+" >(root.getChildElements().at(i).renderChildren(),\""+app+"\"));\n";
									}
								}
								else if("get"+cam==methpm.at(1) && argpm.size()==0 && methpm.at(0)==fldnames.at(k))
								{
									if(isPrimitiveDataType(methpm.at(0)))
									{
										methpm.at(0) = getTypeName(methpm.at(0));
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
										string contType;
										string stlcnt = methpm.at(0);
										string stltyp = methpm.at(0);
										StringUtil::replaceFirst(stltyp,"std::","");
										StringUtil::replaceFirst(stltyp,"<","::");
										StringUtil::replaceFirst(stltyp,">","");
										StringUtil::replaceFirst(stltyp," ","");
										string stlcnttyp = "";
										if(methpm.at(0).find("vector")!=string::npos)
										{
											contType = "std::vector<";
											stlcnttyp = "Vec";
										}
										else if(methpm.at(0).find("queue")!=string::npos)
										{
											contType = "std::queue<";
											stlcnttyp = "Q";
										}
										else if(methpm.at(0).find("deque")!=string::npos)
										{
											contType = "std::deque<";
											stlcnttyp = "Dq";
										}
										else if(methpm.at(0).find("list")!=string::npos)
										{
											contType = "std::list<";
											stlcnttyp = "Lis";
										}
										else if(methpm.at(0).find("multiset")!=string::npos)
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
										//StringUtil::replaceFirst(stlcnt," ","");

										string fqcn = contType + getFullyQualifiedClassName(stlcnt, classStructure.namespaces) + ">";

										if(!ptr)
										{
											methods += (fqcn+" __temp_obj_ser"+stlcnt+stlcnttyp+classStructure.classN+methpm.at(1)+" = __obj->"+methpm.at(1)+"();\n");
											//if(stlcnt=="int" || stlcnt=="long" || stlcnt=="short" || stlcnt=="float" || stlcnt=="string" || stlcnt=="std::string" || stlcnt=="double" || stlcnt=="bool" || stlcnt=="unsigned int" || stlcnt=="unsigned long" || stlcnt=="unsigned short" || stlcnt=="long long" || stlcnt=="unsigned long long")
												methods += ("objxml += \"<"+fldnames.at(k+1)+" type=\\\""+stltyp+"\\\">\"+XMLSerialize::serialize<"+fqcn+" >(__temp_obj_ser"+stlcnt+stlcnttyp+classStructure.classN+methpm.at(1)+",\""+app+"\")");
											//else
											//	methods += ("objxml += \"<"+fldnames.at(k+1)+" type=\\\""+stltyp+"\\\">\"+"+app+"serialize"+stlcnt+stlcnttyp+"(&__temp_obj_ser"+stlcnt+stlcnttyp+classStructure.classN+methpm.at(1)+")");
											//string cam = AfcUtil::camelCased(methpm.at(1));
											methods += ("+\"</"+fldnames.at(k+1)+">\";\n");
											//if(methsall[classStructure.getTreatedClassName(true)+"get"+cam+methpm.at(0)])typedefs += "if(nam==\""+methpm.at(1)+"\")\n__obj->set"+cam+"(*("+methpm.at(0)+"*)"+app+"unSerialize"+stlcnt+stlcnttyp+"(root.getChildElements().at(i).render()));\n";
										}
										else
										{
											methods += (fqcn+"* __temp_obj_ser"+stlcnt+stlcnttyp+classStructure.classN+methpm.at(1)+" = __obj->"+methpm.at(1)+"();\n");
											//if(stlcnt=="int" || stlcnt=="long" || stlcnt=="short" || stlcnt=="float" || stlcnt=="string" || stlcnt=="std::string" || stlcnt=="double" || stlcnt=="bool" || stlcnt=="unsigned int" || stlcnt=="unsigned long" || stlcnt=="unsigned short" || stlcnt=="long long" || stlcnt=="unsigned long long")
												methods += ("if(__obj->"+methpm.at(1)+"()!=NULL)objxml += \"<"+fldnames.at(k+1)+" type=\\\""+stltyp+"\\\">\"+XMLSerialize::serializePointer<"+fqcn+" >(__temp_obj_ser"+stlcnt+stlcnttyp+classStructure.classN+methpm.at(1)+",\""+app+"\")");
											//else
											//	methods += ("if(__obj->"+methpm.at(1)+"()!=NULL)objxml += \"<"+fldnames.at(k+1)+" type=\\\""+stltyp+"\\\">\"+"+app+"serialize"+stlcnt+stlcnttyp+"(__temp_obj_ser"+stlcnt+stlcnttyp+classStructure.classN+methpm.at(1)+")");
											//string cam = AfcUtil::camelCased(methpm.at(1));
											methods += ("+\"</"+fldnames.at(k+1)+">\";\n");
											//if(methsall[classStructure.getTreatedClassName(true)+"get"+cam+methpm.at(0)])typedefs += "if(nam==\""+methpm.at(1)+"\")\n__obj->set"+cam+"(*("+methpm.at(0)+"*)"+app+"unSerialize"+stlcnt+stlcnttyp+"(root.getChildElements().at(i).render()));\n";
										}
									}
									else
									{
										string fqcn = getFullyQualifiedClassName(methpm.at(0), classStructure.namespaces);
										if(!ptr)
										{
											methods += (methpm.at(0)+" __temp_obj_ser"+methpm.at(1)+" = __obj->"+methpm.at(1)+"();\n");
											methods += ("objxml += \"<"+fldnames.at(k+1)+" type=\\\""+methpm.at(0)+"\\\">\"+XMLSerialize::serialize<"+fqcn+" >(__temp_obj_ser"+methpm.at(1)+",\""+app+"\")");
											methods += ("+\"</"+fldnames.at(k+1)+">\";\n");
											//string cam = AfcUtil::camelCased(methpm.at(1));
											//if(methsall[classStructure.getTreatedClassName(true)+"get"+cam+methpm.at(0)])typedefs += "if(nam==\""+methpm.at(1)+"\")\n__obj->set"+cam+"(*("+methpm.at(0)+"*)"+app+"unSerialize"+methpm.at(0)+"(root.getChildElements().at(i).render()));\n";
										}
										else
										{
											methods += (methpm.at(0)+"* __temp_obj_ser"+methpm.at(1)+" = __obj->"+methpm.at(1)+"();\n");
											methods += ("if(__obj->"+methpm.at(1)+"()!=NULL)objxml += \"<"+fldnames.at(k+1)+" type=\\\""+methpm.at(0)+"\\\">\"+XMLSerialize::serializePointer<"+fqcn+" >(__temp_obj_ser"+methpm.at(1)+",\""+app+"\")");
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
		methods += "string objxml=\"<vector-"+classStructure.getTreatedClassName(false)+" namespace=\\\""+classStructure.nmSpc+"\\\">\";\nfor(unsigned int i=0;i<__obj->size();i++)\n{\nobjxml+="+app+"serialize"+classStructure.getTreatedClassName(true)+"(&(__obj->at(i)));\n}\nobjxml+=\"</vector-"+classStructure.getTreatedClassName(false)+" namespace=\\\""+classStructure.nmSpc+"\\\">\";\n";
		methods += "return objxml;}\n";
		methods += "\nstring " +app+ "serialize"+classStructure.getTreatedClassName(true)+"Q(void *t){std::queue<"+classStructure.getFullyQualifiedClassName()+"> *_t=(std::queue<"+classStructure.getFullyQualifiedClassName()+">*)t;std::queue<"+classStructure.getFullyQualifiedClassName()+"> *tt = new std::queue<"+classStructure.getFullyQualifiedClassName()+">;	*tt = *_t;	string objXml = \"<queue-"+classStructure.getTreatedClassName(false)+" namespace=\\\""+classStructure.nmSpc+"\\\">\";	for(unsigned int var=0;var<tt->size();var++)	{		objXml += "+app+"serialize"+classStructure.getTreatedClassName(true)+"(&(tt->front()));		tt->pop();	}	objXml += \"</queue-"+classStructure.getTreatedClassName(false)+" namespace=\\\""+classStructure.nmSpc+"\\\">\";	return objXml;}";
		methods += "\nstring " +app+ "serialize"+classStructure.getTreatedClassName(true)+"Dq(void *_t){deque<"+classStructure.getFullyQualifiedClassName()+"> *t=(deque<"+classStructure.getFullyQualifiedClassName()+">*)_t;string objXml = \"<deque-"+classStructure.getTreatedClassName(false)+" namespace=\\\""+classStructure.nmSpc+"\\\">\";	for(unsigned int var=0;var<t->size();var++)	{		objXml += "+app+"serialize"+classStructure.getTreatedClassName(true)+"(&(t->at(var)));	}	objXml += \"</deque-"+classStructure.getTreatedClassName(false)+" namespace=\\\""+classStructure.nmSpc+"\\\">\";	return objXml;}";
		methods += "\nstring " +app+ "serialize"+classStructure.getTreatedClassName(true)+"Lis(void *_t){	list<"+classStructure.getFullyQualifiedClassName()+"> *t=(list<"+classStructure.getFullyQualifiedClassName()+">*)_t;list<"+classStructure.getFullyQualifiedClassName()+">::iterator it;	string objXml = \"<list-"+classStructure.getTreatedClassName(false)+" namespace=\\\""+classStructure.nmSpc+"\\\">\";	for(it=t->begin();it!=t->end();++it)	{"+classStructure.getFullyQualifiedClassName()+" _temp=*it;	objXml += "+app+"serialize"+classStructure.getTreatedClassName(true)+"(&_temp);	}	objXml += \"</list-"+classStructure.getTreatedClassName(false)+" namespace=\\\""+classStructure.nmSpc+"\\\">\";	return objXml;}";
		classes += "\nstring " +app+ "serialize" + classStructure.getTreatedClassName(true) + "Vec(void* obje);\nstring " +app+ "serialize"+classStructure.getTreatedClassName(true)+"Q(void *t);\nstring " +app+ "serialize"+classStructure.getTreatedClassName(true)+"Dq(void *_t);\nstring " +app+ "serialize"+classStructure.getTreatedClassName(true)+"Lis(void *_t);";
		if(classStructure.prosetser)
		{
			methods += "\nstring " +app+ "serialize"+classStructure.getTreatedClassName(true)+"Set(void *_t){	set<"+classStructure.getFullyQualifiedClassName()+"> *t=(set<"+classStructure.getFullyQualifiedClassName()+">*)_t;set<"+classStructure.getFullyQualifiedClassName()+">::iterator it;	string objXml = \"<set-"+classStructure.getTreatedClassName(false)+" namespace=\\\""+classStructure.nmSpc+"\\\">\";	for(it=t->begin();it!=t->end();++it)	{"+classStructure.getFullyQualifiedClassName()+" _temp=*it;	objXml += "+app+"serialize"+classStructure.getTreatedClassName(true)+"(&_temp);	}	objXml += \"</set-"+classStructure.getTreatedClassName(false)+" namespace=\\\""+classStructure.nmSpc+"\\\">\";	return objXml;}";
			methods += "\nstring " +app+ "serialize"+classStructure.getTreatedClassName(true)+"MulSet(void *_t){	multiset<"+classStructure.getFullyQualifiedClassName()+"> *t=(multiset<"+classStructure.getFullyQualifiedClassName()+">*)_t;multiset<"+classStructure.getFullyQualifiedClassName()+">::iterator it;	string objXml = \"<multiset-"+classStructure.getTreatedClassName(false)+" namespace=\\\""+classStructure.nmSpc+"\\\">\";	for(it=t->begin();it!=t->end();++it)	{"+classStructure.getFullyQualifiedClassName()+" _temp=*it;	objXml += "+app+"serialize"+classStructure.getTreatedClassName(true)+"(&_temp);	}	objXml += \"</multiset-"+classStructure.getTreatedClassName(false)+" namespace=\\\""+classStructure.nmSpc+"\\\">\";	return objXml;}";
			classes += "\nstring " +app+ "serialize"+classStructure.getTreatedClassName(true)+"Set(void *_t);\nstring " +app+ "serialize"+classStructure.getTreatedClassName(true)+"MulSet(void *_t);";
		}

		typedefs += "\n}\nreturn __obj;\n}";
		typedefs += "\nvoid* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"Dq(string objXml){deque<"+classStructure.getFullyQualifiedClassName()+"> *t = new deque<"+classStructure.getFullyQualifiedClassName()+">;XmlParser parser(\"Parser\");\nDocument doc = parser.getDocument(objXml);\nElement message = doc.getRootElement();\nif(message.getTagName()!=\"deque-"+classStructure.getTreatedClassName(false)+"\" || message.getAttribute(\"namespace\")!=\""+classStructure.nmSpc+"\" || message.getChildElements().size()==0)\nreturn NULL;\nfor (int var = 0; var < (int)message.getChildElements().size(); var++){	Element ele = message.getChildElements().at(var);	if(ele.getTagName()==\""+classStructure.getTreatedClassName(true)+"\")	{		t->push_back(*("+classStructure.getFullyQualifiedClassName()+"*)"+app+"unSerialize"+classStructure.getTreatedClassName(true)+"(ele.render()));	}}return t;}";
		typedefs += "\nvoid* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"Q(string objXml){std::queue<"+classStructure.getFullyQualifiedClassName()+"> *t = new std::queue<"+classStructure.getFullyQualifiedClassName()+">;XmlParser parser(\"Parser\");\nDocument doc = parser.getDocument(objXml);\nElement message = doc.getRootElement();\nif(message.getTagName()!=\"queue-"+classStructure.getTreatedClassName(false)+"\" || message.getAttribute(\"namespace\")!=\""+classStructure.nmSpc+"\" || message.getChildElements().size()==0)\nreturn NULL;\nfor (int var = 0; var < (int)message.getChildElements().size(); var++){	Element ele = message.getChildElements().at(var);	if(ele.getTagName()==\""+classStructure.getTreatedClassName(true)+"\")	{		t->push(*("+classStructure.getFullyQualifiedClassName()+"*)"+app+"unSerialize"+classStructure.getTreatedClassName(true)+"(ele.render()));	}}return t;}";
		typedefs += "\nvoid* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"Lis(string objXml){list<"+classStructure.getFullyQualifiedClassName()+"> *t = new list<"+classStructure.getFullyQualifiedClassName()+">;XmlParser parser(\"Parser\");\nDocument doc =parser.getDocument(objXml);\nElement message = doc.getRootElement();\nif(message.getTagName()!=\"list-"+classStructure.getTreatedClassName(false)+"\" || message.getAttribute(\"namespace\")!=\""+classStructure.nmSpc+"\" || message.getChildElements().size()==0)\nreturn NULL;\nfor (int var = 0; var < (int)message.getChildElements().size(); var++){	Element ele = message.getChildElements().at(var);	if(ele.getTagName()==\""+classStructure.getTreatedClassName(true)+"\")	{		t->push_back(*("+classStructure.getFullyQualifiedClassName()+"*)"+app+"unSerialize"+classStructure.getTreatedClassName(true)+"(ele.render()));	}}return t;}";
		classes += "\nvoid* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"Vec(string objXml);\nvoid* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"Q(string objXml);\nvoid* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"Dq(string objXml);\nvoid* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"Lis(string objXml);";
		if(classStructure.prosetser)
		{
			typedefs += "\nvoid* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"Set(string objXml){set<"+classStructure.getFullyQualifiedClassName()+"> *t = new set<"+classStructure.getFullyQualifiedClassName()+">;XmlParser parser(\"Parser\");\nDocument doc = parser.getDocument(objXml);\nElement message = doc.getRootElement();\nif(message.getTagName()!=\"set-"+classStructure.getTreatedClassName(false)+"\" || message.getAttribute(\"namespace\")!=\""+classStructure.nmSpc+"\" || message.getChildElements().size()==0)\nreturn NULL;\nfor (int var = 0; var < (int)message.getChildElements().size(); var++){	Element ele = message.getChildElements().at(var);	if(ele.getTagName()==\""+classStructure.getTreatedClassName(true)+"\")	{		t->insert(*("+classStructure.getFullyQualifiedClassName()+"*)"+app+"unSerialize"+classStructure.getTreatedClassName(true)+"(ele.render()));	}}return t;}";
			typedefs += "\nvoid* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"MulSet(string objXml){multiset<"+classStructure.getFullyQualifiedClassName()+"> *t = new multiset<"+classStructure.getFullyQualifiedClassName()+">;XmlParser parser(\"Parser\");\nDocument doc = parser.getDocument(objXml);\nElement message = doc.getRootElement();\nif(message.getTagName()!=\"multiset-"+classStructure.getTreatedClassName(false)+"\" || message.getAttribute(\"namespace\")!=\""+classStructure.nmSpc+"\" || message.getChildElements().size()==0)\nreturn NULL;\nfor (int var = 0; var < (int)message.getChildElements().size(); var++){	Element ele = message.getChildElements().at(var);	if(ele.getTagName()==\""+classStructure.getTreatedClassName(true)+"\")	{		t->insert(*("+classStructure.getFullyQualifiedClassName()+"*)"+app+"unSerialize"+classStructure.getTreatedClassName(true)+"(ele.render()));	}}return t;}";
			classes += "\nvoid* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"Set(string objXml);\nvoid* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"MulSet(string objXml);";
		}
		typedefs += "\nvoid* " +app+ "unSerialize"+classStructure.getTreatedClassName(true)+"Vec(string objXml){vector<"+classStructure.getFullyQualifiedClassName()+"> *t = new vector<"+classStructure.getFullyQualifiedClassName()+">;XmlParser parser(\"Parser\");\nDocument doc = parser.getDocument(objXml);\nElement message = doc.getRootElement();\nif(message.getTagName()!=\"vector-"+classStructure.getTreatedClassName(false)+"\" || message.getAttribute(\"namespace\")!=\""+classStructure.nmSpc+"\" || message.getChildElements().size()==0)\nreturn NULL;\nfor (int var = 0; var < (int)message.getChildElements().size(); var++){	Element ele = message.getChildElements().at(var);	if(ele.getTagName()==\""+classStructure.getTreatedClassName(true)+"\")	{		t->push_back(*("+classStructure.getFullyQualifiedClassName()+"*)"+app+"unSerialize"+classStructure.getTreatedClassName(true)+"(ele.render()));	}}return t;}";
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
		classes += "\nstring " +app+ "binarySerialize" + classStructure.getTreatedClassName(true)
				+ "(void* obje);\nvoid* " +app+ "binaryUnSerialize" + classStructure.getTreatedClassName(true) + "(string objXml);";
		methods += "\nstring " +app+ "binarySerialize" + classStructure.getTreatedClassName(true)
				+ "(void* obje)\n{\n"+classStructure.getFullyQualifiedClassName()+" *__obj=("+classStructure.getFullyQualifiedClassName()+"*)obje;\n";
		methods += "AMEFEncoder enc;\nAMEFObject object;\nobject.setName(\""+classStructure.getTreatedClassName(true)+"\");\n";
		typedefs += "\nvoid* " +app+ "binaryUnSerialize" + classStructure.getTreatedClassName(true) + "(string objXml)\n{\n";
		typedefs += classStructure.getFullyQualifiedClassName()+" *__obj=new "+classStructure.getFullyQualifiedClassName()
				+";\nAMEFDecoder dec;\nAMEFObject* root = dec.decodeB(objXml, true, false);\n";
		typedefs += "if(root->getNameStr()!=\""+classStructure.getTreatedClassName(true)+"\")return NULL;\n";
		typedefs += "for(unsigned int i=0;i<root->getPackets().size();i++)\n{\n";
		typedefs += "string nam=root->getPackets().at(i)->getNameStr();\n";

		string publf, privf, protf ,publm, privm, protm;
		string meth,fld;
		size_t tes;
		vector<string> fldnames;
		map<string, bool> fldptr;
		map<string, int> fldstat;
		if (classStructure.pri.size() > 0)
		{
			for (unsigned int i = 0; i < classStructure.pri.size(); i++)
			{
				if(((tes=classStructure.pri.at(i).find("("))==string::npos && (tes=classStructure.pri.at(i).find(")"))==string::npos && classStructure.pri.at(i).find("~")==string::npos))
				{
					fld = classStructure.pri.at(i);

					bool fldstatic = false;
					if(RegexUtil::find(fld, "[ \t]*static[ \t]+")!=-1 || RegexUtil::find(fld, "[ \t]+static[ \t]+")!=-1)
					{
						RegexUtil::replace(fld, "[ \t]*static[ \t]+", " ");
						RegexUtil::replace(fld, "[ \t]+static[ \t]+", " ");
						fldstatic = true;
					}
					RegexUtil::replace(fld, "[ \t]*const[ \t]+", " ");
					RegexUtil::replace(fld, "[ \t]+const[ \t]+", " ");

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
		}
		if (classStructure.pro.size() > 0)
		{
			for (unsigned int i = 0; i < classStructure.pro.size(); i++)
			{
				if(((tes=classStructure.pro.at(i).find("("))==string::npos && (tes=classStructure.pro.at(i).find(")"))==string::npos && classStructure.pro.at(i).find("~")==string::npos))
				{
					fld = classStructure.pro.at(i);

					bool fldstatic = false;
					if(RegexUtil::find(fld, "[ \t]*static[ \t]+")!=-1 || RegexUtil::find(fld, "[ \t]+static[ \t]+")!=-1)
					{
						RegexUtil::replace(fld, "[ \t]*static[ \t]+", " ");
						RegexUtil::replace(fld, "[ \t]+static[ \t]+", " ");
						fldstatic = true;
					}
					RegexUtil::replace(fld, "[ \t]*const[ \t]+", " ");
					RegexUtil::replace(fld, "[ \t]+const[ \t]+", " ");

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
		}
		if (classStructure.pub.size() > 0)
		{
			for (unsigned int i = 0; i < classStructure.pub.size(); i++)
			{
				if(((tes=classStructure.pub.at(i).find("("))==string::npos && (tes=classStructure.pub.at(i).find(")"))==string::npos && classStructure.pub.at(i).find("~")==string::npos))
				{
					fld = classStructure.pub.at(i);

					bool fldstatic = false;
					if(RegexUtil::find(fld, "[ \t]*static[ \t]+")!=-1 || RegexUtil::find(fld, "[ \t]+static[ \t]+")!=-1)
					{
						RegexUtil::replace(fld, "[ \t]*static[ \t]+", " ");
						RegexUtil::replace(fld, "[ \t]+static[ \t]+", " ");
						fldstatic = true;
					}
					RegexUtil::replace(fld, "[ \t]*const[ \t]+", " ");
					RegexUtil::replace(fld, "[ \t]+const[ \t]+", " ");

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
						if(isPrimitiveDataType(fldp.at(0)))
						{
							fldp.at(0) = getTypeName(fldp.at(0));
							string argtype = StringUtil::capitalizedCopy(fldp.at(0));
							string vallu = "root->getPackets().at(i)->get";
							if(fldp.at(0)=="unsigned int")
							{
								vallu += "UIntValue()";
							}
							else if(fldp.at(0)=="unsigned short")
							{
								vallu += "UShortValue()";
							}
							else if(fldp.at(0)=="unsigned long")
							{
								vallu += "ULongValue()";
							}
							else if(fldp.at(0)=="unsigned long long")
							{
								vallu += "ULongLongValue()";
							}
							else if(fldp.at(0)=="long long")
							{
								vallu += "LongLongValue()";
							}
							else if(fldp.at(0)=="string" || fldp.at(0)=="std::string")
							{
								vallu += "ValueStr()";
							}
							else
							{
								vallu += argtype+"Value()";
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
							//StringUtil::replaceFirst(stlcnt," ","");

							string fqcn = contType + getFullyQualifiedClassName(stlcnt, classStructure.namespaces) + ">";
							contType += stlcnt + ",";

							if(!ptr)
							{
								methods += (fqcn+" __temp_obj_ser"+stlcnt+stlcnttyp+classStructure.getTreatedClassName(true)+fldp.at(1)+" = __obj->"+fldp.at(1)+";\n");
								//if(stlcnt=="int" || stlcnt=="long" || stlcnt=="short" || stlcnt=="float" || stlcnt=="string" || stlcnt=="std::string" || stlcnt=="double" || stlcnt=="bool" || stlcnt=="unsigned int" || stlcnt=="unsigned long" || stlcnt=="unsigned short" || stlcnt=="long long" || stlcnt=="unsigned long long")
									methods += ("object.addPacket(Serialize::serialize<"+fqcn+" >(__temp_obj_ser"+stlcnt+stlcnttyp+classStructure.getTreatedClassName(true)+fldp.at(1)+",\""+app+"\"),\""+fldp.at(1)+"\");\n");
								//else
								//	methods += ("object.addPacket("+app+"binarySerialize"+stlcnt+stlcnttyp+"(&__temp_obj_ser"+stlcnt+stlcnttyp+classStructure.getTreatedClassName(true)+fldp.at(1)+"),\""+fldp.at(1)+"\");\n");
								string cam = StringUtil::capitalizedCopy(fldp.at(1));
								//methods += ("+\"</"+nam+">\";\n");
								typedefs += "if(nam==\""+fldp.at(1)+"\" && root->getPackets().at(i)->getValue()!=\"\"){";
								typedefs += "\nAMEFEncoder enc;\n";
								//if(stlcnt=="int" || stlcnt=="long" || stlcnt=="short" || stlcnt=="float" || stlcnt=="string" || stlcnt=="std::string" || stlcnt=="double" || stlcnt=="bool" || stlcnt=="unsigned int" || stlcnt=="unsigned long" || stlcnt=="unsigned short" || stlcnt=="long long" || stlcnt=="unsigned long long")
									typedefs += "\n__obj->"+fldp.at(1)+" = Serialize::unserialize<"+fqcn+" >(root->getPackets().at(i)->getValue(),\""+app+"\");\n";
								//else
								//	typedefs += "\n__obj->"+fldp.at(1)+" = *("+fldp.at(0)+"*)"+app+"binaryUnSerialize"+stlcnt+stlcnttyp+"(root->getPackets().at(i)->getValue());\n";
								typedefs += "\n}\n";
							}
							else
							{
								methods += (fqcn+"* __temp_obj_ser"+stlcnt+stlcnttyp+classStructure.getTreatedClassName(true)+fldp.at(1)+" = __obj->"+fldp.at(1)+";\n");
								//if(stlcnt=="int" || stlcnt=="long" || stlcnt=="short" || stlcnt=="float" || stlcnt=="string" || stlcnt=="std::string" || stlcnt=="double" || stlcnt=="bool" || stlcnt=="unsigned int" || stlcnt=="unsigned long" || stlcnt=="unsigned short" || stlcnt=="long long" || stlcnt=="unsigned long long")
									methods += ("if(__obj->"+fldp.at(1)+"!=NULL)object.addPacket(Serialize::serializePointer<"+fqcn+" >(__temp_obj_ser"+stlcnt+stlcnttyp+classStructure.getTreatedClassName(true)+fldp.at(1)+",\""+app+"\"),\""+fldp.at(1)+"\");\n");
								//else
								//	methods += ("if(__obj->"+fldp.at(1)+"!=NULL)object.addPacket("+app+"binarySerialize"+stlcnt+stlcnttyp+"(__temp_obj_ser"+stlcnt+stlcnttyp+classStructure.getTreatedClassName(true)+fldp.at(1)+"),\""+fldp.at(1)+"\");\n");
								string cam = StringUtil::capitalizedCopy(fldp.at(1));
								//methods += ("+\"</"+nam+">\";\n");
								typedefs += "if(nam==\""+fldp.at(1)+"\" && root->getPackets().at(i)->getValue()!=\"\"){";
								typedefs += "\nAMEFEncoder enc;\n";
								//@TODO Is a concern if pointer is used, the address reference might lead to data issues
								//if(stlcnt=="int" || stlcnt=="long" || stlcnt=="short" || stlcnt=="float" || stlcnt=="string" || stlcnt=="std::string" || stlcnt=="double" || stlcnt=="bool" || stlcnt=="unsigned int" || stlcnt=="unsigned long" || stlcnt=="unsigned short" || stlcnt=="long long" || stlcnt=="unsigned long long")
									typedefs += "\n__obj->"+fldp.at(1)+" = ("+fqcn+"*)Serialize::unserializeToPointer<"+fqcn+" >(root->getPackets().at(i)->getValue(),\""+app+"\");\n";
								//else
								//	typedefs += "\n__obj->"+fldp.at(1)+" = ("+fldp.at(0)+"*)"+app+"binaryUnSerialize"+stlcnt+stlcnttyp+"(root->getPackets().at(i)->getValue());\n";
								typedefs += "\n}\n";
							}
						}
						else
						{
							string fqcn = getFullyQualifiedClassName(fldp.at(0), classStructure.namespaces);
							if(!ptr)
							{
								methods += (fqcn+" __temp_obj_ser"+fldp.at(1)+" = __obj->"+fldp.at(1)+";\n");
								methods += ("object.addPacket(Serialize::serialize<"+fqcn+" >(__temp_obj_ser"+fldp.at(1)+",\""+app+"\"),\""+fldp.at(1)+"\");\n");
								string cam = StringUtil::capitalizedCopy(fldp.at(1));
								//methods += ("+\"</"+nam+">\";\n");
								typedefs += "if(nam==\""+fldp.at(1)+"\" && root->getPackets().at(i)->getValue()!=\"\")\n__obj->"+fldp.at(1)+" = Serialize::unserialize<"+fqcn+" >(root->getPackets().at(i)->getValue(),\""+app+"\");\n";
							}
							else
							{
								methods += (fqcn+"* __temp_obj_ser"+fldp.at(1)+" = __obj->"+fldp.at(1)+";\n");
								methods += ("if(__obj->"+fldp.at(1)+"!=NULL)object.addPacket(Serialize::serializePointer<"+fqcn+" >(__temp_obj_ser"+fldp.at(1)+",\""+app+"\"),\""+fldp.at(1)+"\");\n");
								string cam = StringUtil::capitalizedCopy(fldp.at(1));
								//methods += ("+\"</"+nam+">\";\n");
								typedefs += "if(nam==\""+fldp.at(1)+"\" && root->getPackets().at(i)->getValue()!=\"\")\n__obj->"+fldp.at(1)+" = ("+fqcn+"*)Serialize::unserializeToPointer<"+fqcn+" >(root->getPackets().at(i)->getValue(),\""+app+"\");\n";
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

					string argts = meth.substr(meth.find("("),meth.find(")")-meth.find("("));
					StringUtil::replaceFirst(argts,"(","");
					StringUtil::replaceAll(argts,")","");
					meth = meth.substr(0,meth.find("("));
					StringUtil::trim(meth);
					RegexUtil::replace(meth, "[ \t]*,[ \t]*", ",");

					bool methstat = false;
					if(RegexUtil::find(meth, "[ \t]*static[ \t]+")!=-1 || RegexUtil::find(meth, "[ \t]+static[ \t]+")!=-1)
					{
						RegexUtil::replace(meth, "[ \t]*static[ \t]+", " ");
						RegexUtil::replace(meth, "[ \t]+static[ \t]+", " ");
						methstat = true;
					}
					RegexUtil::replace(meth, "[ \t]*inline[ \t]+", " ");
					RegexUtil::replace(meth, "[ \t]+inline[ \t]+", " ");
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
					{
						if(meth.find("operator>")!=string::npos && meth.find("operator>=")==string::npos && meth.find("operator>>")==string::npos)
						{
							classStructure.prosetser = true;
						}
						else if(meth.find("operator<")!=string::npos && meth.find("operator<=")==string::npos && meth.find("operator<<")==string::npos)
						{
							classStructure.prosetser = true;
						}
					}
					else
					{
						if(methpm.at(0)!=classStructure.getTreatedClassName(false))
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

						if(methpm.at(0)!=classStructure.getTreatedClassName(false))
						{
							for(unsigned int k = 0; k < fldnames.size(); k=k+2)
							{
								if(fldstat[fldnames.at(k+1)]!=2 && ptr==fldptr[fldnames.at(k+1)])
									continue;
								ptr = fldptr[fldnames.at(k+1)];
								string cam = StringUtil::capitalizedCopy(fldnames.at(k+1));
								string fldNamewoptr = StringUtil::replaceFirstCopy(fldnames.at(k), "*", "");
								////logger << "setter check " << fldNamewoptr << " "<<methpm.at(0) << " "<< methpm.at(1) << " "<< cam<< endl;
								if(argpm.size()==1)
								{
									StringUtil::replaceFirst(argpm.at(0), "*", "");
									////logger << argpm.at(0) << " " << argpm.size() << endl;
								}
								if("set"+cam==methpm.at(1) && argpm.size()==1 && argpm.at(0)==fldNamewoptr && methpm.at(0)=="void")
								{
									////logger << " inside setter " << endl;
									if(isPrimitiveDataType(argpm.at(0)))
									{
										argpm.at(0) = getTypeName(argpm.at(0));
										string argtype = StringUtil::capitalizedCopy(argpm.at(0));
										string vallu = "root->getPackets().at(i)->get";
										if(argpm.at(0)=="unsigned int")
										{
											vallu += "UIntValue()";
										}
										else if(argpm.at(0)=="unsigned short")
										{
											vallu += "UShortValue()";
										}
										else if(argpm.at(0)=="unsigned long")
										{
											vallu += "ULongValue()";
										}
										else if(argpm.at(0)=="unsigned long long")
										{
											vallu += "ULongLongValue()";
										}
										else if(argpm.at(0)=="long long")
										{
											vallu += "LongLongValue()";
										}
										else if(argpm.at(0)=="string" || argpm.at(0)=="std::string")
										{
											vallu += "ValueStr()";
										}
										else
										{
											vallu += argtype+"Value()";
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
										//StringUtil::replaceFirst(stlcnt," ","");

										string fqcn = contType + getFullyQualifiedClassName(stlcnt, classStructure.namespaces) + ">";
										contType += stlcnt + ",";

										if(!ptr)
										{
											typedefs += "if(nam==\""+fldnames.at(k+1)+"\" && root->getPackets().at(i)->getValue()!=\"\"){";
											typedefs += "\nAMEFEncoder enc;";
											//if(stlcnt=="int" || stlcnt=="long" || stlcnt=="short" || stlcnt=="float" || stlcnt=="string" || stlcnt=="std::string" || stlcnt=="double" || stlcnt=="bool" || stlcnt=="unsigned int" || stlcnt=="unsigned long" || stlcnt=="unsigned short" || stlcnt=="long long" || stlcnt=="unsigned long long")
												typedefs += "\n__obj->set"+cam+"(Serialize::unserialize<"+fqcn+" >(root->getPackets().at(i)->getValue(),\""+app+"\"));\n";
											//else
											//	typedefs += "\n__obj->set"+cam+"(*("+argpm.at(0)+"*)"+app+"binaryUnSerialize"+stlcnt+stlcnttyp+"(root->getPackets().at(i)->getValue()));\n";
											typedefs += "\n}\n";
										}
										else
										{
											typedefs += "if(nam==\""+fldnames.at(k+1)+"\" && root->getPackets().at(i)->getValue()!=\"\"){";
											typedefs += "\nAMEFEncoder enc;";
											//if(stlcnt=="int" || stlcnt=="long" || stlcnt=="short" || stlcnt=="float" || stlcnt=="string" || stlcnt=="std::string" || stlcnt=="double" || stlcnt=="bool" || stlcnt=="unsigned int" || stlcnt=="unsigned long" || stlcnt=="unsigned short" || stlcnt=="long long" || stlcnt=="unsigned long long")
												typedefs += "\n__obj->set"+cam+"(("+fqcn+"*)Serialize::unserializeToPointer<"+fqcn+" >(root->getPackets().at(i)->getValue(),\""+app+"\"));\n";
											//else
											//	typedefs += "\n__obj->set"+cam+"(("+argpm.at(0)+"*)"+app+"binaryUnSerialize"+stlcnt+stlcnttyp+"(root->getPackets().at(i)->getValue()));\n";
											typedefs += "\n}\n";
										}
										//if(methsall[classStructure.getTreatedClassName(true)+"get"+cam+methpm.at(0)])typedefs += "if(nam==\""+methpm.at(1)+"\")\n__obj->set"+cam+"(*("+methpm.at(0)+"*)"+app+"binaryUnSerialize"+stlcnt+stlcnttyp+"(root.getChildElements().at(i).render()));\n";
									}
									else
									{
										string fqcn = getFullyQualifiedClassName(argpm.at(0), classStructure.namespaces);
										if(!ptr)
										{
											typedefs += "if(nam==\""+fldnames.at(k+1)+"\" && root->getPackets().at(i)->getValue()!=\"\")\n{\nAMEFEncoder enc;\n__obj->"+methpm.at(1)+"(Serialize::unserialize<"+fqcn+" >(root->getPackets().at(i)->getValueStr(),\""+app+"\"));}\n";
										}
										else
										{
											typedefs += "if(nam==\""+fldnames.at(k+1)+"\" && root->getPackets().at(i)->getValue()!=\"\")\n{\nAMEFEncoder enc;\n__obj->"+methpm.at(1)+"(("+fqcn+"*)Serialize::unserializeToPointer<"+fqcn+" >(root->getPackets().at(i)->getValueStr(),\""+app+"\"));}\n";
										}
									}
								}
								else if("get"+cam==methpm.at(1) && argpm.size()==0 && methpm.at(0)==fldNamewoptr)
								{
									if(isPrimitiveDataType(methpm.at(0)))
									{
										methpm.at(0) = getTypeName(methpm.at(0));
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
										string contType;
										if(methpm.at(0).find("vector")!=string::npos)
										{
											contType = "std::vector<";
											stlcnttyp = "Vec";
										}
										else if(methpm.at(0).find("queue")!=string::npos)
										{
											contType = "std::queue<";
											stlcnttyp = "Q";
										}
										else if(methpm.at(0).find("deque")!=string::npos)
										{
											contType = "std::deque<";
											stlcnttyp = "Dq";
										}
										else if(methpm.at(0).find("list")!=string::npos)
										{
											contType = "std::list<";
											stlcnttyp = "Lis";
										}
										else if(methpm.at(0).find("multiset")!=string::npos)
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
										//StringUtil::replaceFirst(stlcnt," ","");

										string fqcn = contType + getFullyQualifiedClassName(stlcnt, classStructure.namespaces) + ">";

										if(!ptr)
										{
											methods += (fqcn+" __temp_obj_ser"+stlcnt+stlcnttyp+classStructure.getTreatedClassName(true)+methpm.at(1)+" = __obj->"+methpm.at(1)+"();\n");
											//if(stlcnt=="int" || stlcnt=="long" || stlcnt=="short" || stlcnt=="float" || stlcnt=="string" || stlcnt=="std::string" || stlcnt=="double" || stlcnt=="bool" || stlcnt=="unsigned int" || stlcnt=="unsigned long" || stlcnt=="unsigned short" || stlcnt=="long long" || stlcnt=="unsigned long long")
												methods += ("object.addPacket(Serialize::serialize<"+fqcn+" >(__temp_obj_ser"+stlcnt+stlcnttyp+classStructure.getTreatedClassName(true)+methpm.at(1)+",\""+app+"\"),\""+fldnames.at(k+1)+"\");\n");
											//else
											//	methods += ("object.addPacket("+app+"binarySerialize"+stlcnt+stlcnttyp+"(&__temp_obj_ser"+stlcnt+stlcnttyp+classStructure.getTreatedClassName(true)+methpm.at(1)+"),\""+fldnames.at(k+1)+"\");\n");
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
											methods += (fqcn+"* __temp_obj_ser"+stlcnt+stlcnttyp+classStructure.getTreatedClassName(true)+methpm.at(1)+" = __obj->"+methpm.at(1)+"();\n");
											//if(stlcnt=="int" || stlcnt=="long" || stlcnt=="short" || stlcnt=="float" || stlcnt=="string" || stlcnt=="std::string" || stlcnt=="double" || stlcnt=="bool" || stlcnt=="unsigned int" || stlcnt=="unsigned long" || stlcnt=="unsigned short" || stlcnt=="long long" || stlcnt=="unsigned long long")
												methods += ("if(__obj->"+methpm.at(1)+"()!=NULL)object.addPacket(Serialize::serializePointer<"+fqcn+" >(__temp_obj_ser"+stlcnt+stlcnttyp+classStructure.getTreatedClassName(true)+methpm.at(1)+",\""+app+"\"),\""+fldnames.at(k+1)+"\");\n");
											//else
											//	methods += ("if(__obj->"+methpm.at(1)+"()!=NULL)object.addPacket("+app+"binarySerialize"+stlcnt+stlcnttyp+"(__temp_obj_ser"+stlcnt+stlcnttyp+classStructure.getTreatedClassName(true)+methpm.at(1)+"),\""+fldnames.at(k+1)+"\");\n");
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
									else if(isValidClass(methpm.at(0), app))
									{
										string fqcn = getFullyQualifiedClassName(methpm.at(0), classStructure.namespaces);
										//string cam = StringUtil::capitalizedCopy(methpm.at(1));
										//if(methsall[classStructure.getTreatedClassName(true)+"get"+cam+methpm.at(0)])typedefs += "if(nam==\""+methpm.at(1)+"\")\n__obj->set"+cam+"(*("+methpm.at(0)+"*)"+app+"binaryUnSerialize"+methpm.at(0)+"(root.getChildElements().at(i).render()));\n";
										if(!ptr)
										{
											methods += (fqcn+" __temp_obj_ser"+methpm.at(1)+" = __obj->"+methpm.at(1)+"();\n");
											methods += ("object.addPacket(Serialize::serialize<"+fqcn+" >(__temp_obj_ser"+methpm.at(1)+"),\""+fldnames.at(k+1)+"\");\n");
											//typedefs += "if(nam==\""+methpm.at(1)+"\")\n__obj->set"+cam+"(*("+methpm.at(0)+"*)"+app+"binaryUnSerialize"+methpm.at(0)+"(root->getPackets().at(i)->getValue()));\n";
										}
										else
										{
											methods += (fqcn+"* __temp_obj_ser"+methpm.at(1)+" = __obj->"+methpm.at(1)+"();\n");
											methods += ("if(__obj->"+methpm.at(1)+"()!=NULL)object.addPacket(Serialize::serializePointer<"+fqcn+" >(__temp_obj_ser"+methpm.at(1)+"),\""+fldnames.at(k+1)+"\");\n");
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
				+"string objXml;\nAMEFObject object;AMEFEncoder enc;\nfor(it=t->begin();it!=t->end();++it)	{"+classStructure.getFullyQualifiedClassName()+" _temp=*it;object.addPacket("+app+"binarySerialize"+classStructure.getTreatedClassName(true)+"(&(_temp)));	}\nreturn  enc.encodeB(&object, false);\n}";
		classes += "\nstring " +app+ "binarySerialize" + classStructure.getTreatedClassName(true) + "Vec(void* obje);\nstring " +app+ "binarySerialize"+classStructure.getTreatedClassName(true)+"Q(void *t);\nstring " +app+ "binarySerialize"+classStructure.getTreatedClassName(true)+"Dq(void *_t);\nstring " +app+ "binarySerialize"+classStructure.getTreatedClassName(true)+"Lis(void *_t);";
		if(classStructure.prosetser)
		{
			methods += "\nstring " +app+ "binarySerialize"+classStructure.getTreatedClassName(true)+"Set(void *_t)\n{\nset<"+classStructure.getFullyQualifiedClassName()+"> *t=(set<"+classStructure.getFullyQualifiedClassName()+">*)_t;set<"+classStructure.getFullyQualifiedClassName()+">::iterator it;"
					+"string objXml;\nAMEFObject object;AMEFEncoder enc;\nfor(it=t->begin();it!=t->end();++it)	{"+classStructure.getFullyQualifiedClassName()+" _temp=*it;object.addPacket("+app+"binarySerialize"+classStructure.getTreatedClassName(true)+"(&(_temp)));\n}\nreturn  enc.encodeB(&object, false);\n}";
			methods += "\nstring " +app+ "binarySerialize"+classStructure.getTreatedClassName(true)+"MulSet(void *_t)\n{\nmultiset<"+classStructure.getFullyQualifiedClassName()+"> *t=(multiset<"+classStructure.getFullyQualifiedClassName()+">*)_t;multiset<"+classStructure.getFullyQualifiedClassName()+">::iterator it;"
					+"string objXml;\nAMEFObject object;AMEFEncoder enc;\nfor(it=t->begin();it!=t->end();++it)	{"+classStructure.getFullyQualifiedClassName()+" _temp=*it;object.addPacket("+app+"binarySerialize"+classStructure.getTreatedClassName(true)+"(&(_temp)));\n}\nreturn  enc.encodeB(&object, false);\n}";
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

string Reflection::getXSDDefinitions(map<string, ClassStructure> allclsmap, string fqcn, Reflection ref, string appname, string &trgnmspc,
		set<string> &allnmspcs, string dfnmspc, string resp)
{
	string int_obj_binding, obj_binding, tmpnmspc;
	ClassStructure *clstruct = NULL;
	map<string, ClassStructure>::iterator it;
	for (it=allclsmap.begin();it!=allclsmap.end();++it)
	{
		if(it->second.getFullyQualifiedClassName()==fqcn)
		{
			clstruct = &it->second;
			break;
		}
	}
	if(clstruct==NULL)
	{
		//logger << ("Error generating web-service xsd definitions, as class "+fqcn + " not found...") << endl;
		return "";
	}

	string nmspcid = Reflection::getNameSpaceId(clstruct->nmSpc, appname);
	if(nmspcid=="")
		nmspcid = "tns";
	trgnmspc = nmspcid;
	allnmspcs.insert(nmspcid);
	//obj_binding.append("<xsd:complexType name=\""+nmspcid+":"+clstruct->getTreatedClassName(false)+"\">\n");
	//obj_binding.append("<xsd:sequence>\n");
	vector<string> fldnames;
	string fld;
	size_t tes;
	bool  ptr;
	map<string, bool> fldptr;
	map<string, int> fldstat;
	if (clstruct->pri.size() > 0)
	{
		for (unsigned int i = 0; i < clstruct->pri.size(); i++)
		{
			if(((tes=clstruct->pri.at(i).find("("))==string::npos && (tes=clstruct->pri.at(i).find(")"))==string::npos && clstruct->pri.at(i).find("~")==string::npos))
			{
				fld = clstruct->pri.at(i);
				StringUtil::replaceFirst(fld,";","");
				vector<string> fldp;
				StringUtil::split(fldp, fld, (" "));
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
	}
	if (clstruct->pro.size() > 0)
	{
		for (unsigned int i = 0; i < clstruct->pro.size(); i++)
		{
			if(((tes=clstruct->pro.at(i).find("("))==string::npos && (tes=clstruct->pro.at(i).find(")"))==string::npos && clstruct->pro.at(i).find("~")==string::npos))
			{
				fld = clstruct->pro.at(i);
				StringUtil::replaceFirst(fld,";","");
				vector<string> fldp;
				StringUtil::split(fldp, fld, (" "));
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
	}
	for(unsigned int i=0;i<clstruct->pub.size();i++)
	{
		if(((tes=clstruct->pub.at(i).find("("))==string::npos && (tes=clstruct->pub.at(i).find(")"))==string::npos && clstruct->pub.at(i).find("~")==string::npos))
		{
			fld = clstruct->pub.at(i);

			bool fldstatic = false;
			if(RegexUtil::find(fld, "[ \t]*static[ \t]+")!=-1 || RegexUtil::find(fld, "[ \t]+static[ \t]+")!=-1)
			{
				RegexUtil::replace(fld, "[ \t]*static[ \t]+", " ");
				RegexUtil::replace(fld, "[ \t]+static[ \t]+", " ");
				fldstatic = true;
			}
			RegexUtil::replace(fld, "[ \t]*const[ \t]+", " ");
			RegexUtil::replace(fld, "[ \t]+const[ \t]+", " ");

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

				if(isPrimitiveDataType(fldp.at(0)))
				{
					fldp.at(0) = getTypeName(fldp.at(0));
					if(!ptr)
					{
						if(fldp.at(0)=="long long")
						{
							fldp.at(0) = "long";
						}
						else if(fldp.at(0)=="unsigned long long")
						{
							fldp.at(0) = "unsignedlong";
						}
						else if(fldp.at(0).find("unsigned")==0)
						{
							StringUtil::replaceAll(fldp.at(0), " ", "");
						}
						else if(fldp.at(0)=="bool")
						{
							fldp.at(0) = "boolean";
						}
						obj_binding.append("\n<xsd:element name=\""+nam+"\" type=\"xsd:"+fldp.at(0)+"\"/>\n");
					}
					else
					{}
				}
				else if(fldp.at(0)=="Date")
				{
					if(!ptr)
					{}
					else
					{}
				}
				else if(fldp.at(0)=="BinaryData")
				{
					if(!ptr)
					{}
					else
					{}
				}
				else if(fldp.at(0).find("vector")!=string::npos || fldp.at(0).find("queue")!=string::npos || fldp.at(0).find("deque")!=string::npos || fldp.at(0).find("set")!=string::npos || fldp.at(0).find("list")!=string::npos)
				{
					string nam = fldp.at(1);
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
					//StringUtil::replaceFirst(stlcnt," ","");

					string fqcn = ref.getFullyQualifiedClassName(stlcnt, clstruct->getNamespaces());
					contType += stlcnt + ",";

					if(!ptr)
					{
						string namespc;
						if(isPrimitiveDataType(fqcn))
						{
							fqcn = getTypeName(fqcn);
							if(fqcn=="long long")
							{
								fqcn = "long";
							}
							else if(fqcn=="unsigned long long")
							{
								fqcn = "unsignedlong";
							}
							else if(fqcn.find("unsigned")==0)
							{
								StringUtil::replaceAll(fqcn, " ", "");
							}
							else if(fqcn=="bool")
							{
								fqcn = "boolean";
							}
							string xstyp = fqcn;
							StringUtil::replaceAll(xstyp, "::", "_");
							obj_binding.append("<xsd:complexType name=\"ArrayOf"+clstruct->getTreatedClassName(true)+fqcn+"\"> <xsd:sequence>\n");
							obj_binding.append("<xsd:element maxOccurs=\"unbounded\" minOccurs=\"0\" name=\""+nam+"\" nillable=\"true\" type=\"xsd:"+xstyp+"\" />\n");
							obj_binding.append("</xsd:sequence> </xsd:complexType>\n");
						}
						else
						{
							string clnms = fqcn;
							if(fqcn.find("::")!=string::npos)
							{
								clnms = fqcn.substr(0, fqcn.find_last_of("::")+2);
								namespc = Reflection::getNameSpaceId(clnms, appname);
							}
							else
							{
								namespc = "tns";
							}
							string xstyp = fqcn;
							StringUtil::replaceAll(xstyp, "::", "_");
							obj_binding.append("<xsd:complexType name=\"ArrayOf"+clstruct->getTreatedClassName(true)+xstyp+"\"> <xsd:sequence>\n");
							obj_binding.append("<xsd:element maxOccurs=\"unbounded\" minOccurs=\"0\" name=\""+nam+"\" nillable=\"true\" type=\""+namespc+":"+xstyp+"\" />\n");
							obj_binding.append("</xsd:sequence> </xsd:complexType>\n");
							int_obj_binding.append(getXSDDefinitions(allclsmap, fqcn, ref, appname, tmpnmspc, allnmspcs, dfnmspc, resp));
						}
					}
					else
					{}
				}
				else
				{
					string namespc;
					string nam = fldp.at(1);
					string fqcn = ref.getFullyQualifiedClassName(fldp.at(0), clstruct->getNamespaces());
					if(!ptr)
					{
						string clnms = fqcn;
						if(fqcn.find("::")!=string::npos)
						{
							clnms = fqcn.substr(0, fqcn.find_last_of("::")+2);
							namespc = Reflection::getNameSpaceId(clnms, appname);
						}
						else
						{
							namespc = "tns";
						}
						string xstyp = fqcn;
						StringUtil::replaceAll(xstyp, "::", "_");
						obj_binding.append("\n<xsd:element name=\""+nam+"\" type=\""+namespc+":"+xstyp+"\"/>\n");
						int_obj_binding.append(getXSDDefinitions(allclsmap, fqcn, ref, appname, tmpnmspc, allnmspcs, dfnmspc, resp));
					}
					else
					{}
				}
			}
		}
		if((tes=clstruct->pub.at(i).find("("))!=string::npos && (tes=clstruct->pub.at(i).find(")"))!=string::npos && clstruct->pub.at(i).find("~")==string::npos
				&& fldnames.size()>0)
		{
			string meth = clstruct->pub.at(i);
			StringUtil::replaceFirst(meth,";","");

			string argts = meth.substr(meth.find("("),meth.find(")")-meth.find("("));
			StringUtil::replaceFirst(argts,"(","");
			StringUtil::replaceAll(argts,")","");
			meth = meth.substr(0,meth.find("("));
			StringUtil::trim(meth);
			RegexUtil::replace(meth, "[ \t]*,[ \t]*", ",");

			bool methstat = false;
			if(RegexUtil::find(meth, "[ \t]*static[ \t]+")!=-1 || RegexUtil::find(meth, "[ \t]+static[ \t]+")!=-1)
			{
				RegexUtil::replace(meth, "[ \t]*static[ \t]+", " ");
				RegexUtil::replace(meth, "[ \t]+static[ \t]+", " ");
				methstat = true;
			}
			RegexUtil::replace(meth, "[ \t]*inline[ \t]+", " ");
			RegexUtil::replace(meth, "[ \t]+inline[ \t]+", " ");
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
			{

			}
			else
			{
				if(methpm.at(0)!=clstruct->getTreatedClassName(false))
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

				if(methpm.at(0)!=clstruct->getTreatedClassName(false))
				{
					for(unsigned int k = 0; k < fldnames.size(); k=k+2)
					{
						if(fldstat[fldnames.at(k+1)]!=2 && ptr==fldptr[fldnames.at(k+1)])
							continue;
						string nam = fldnames.at(k+1);
						string cam = AfcUtil::camelCased(fldnames.at(k+1));
						if("set"+cam==methpm.at(1) && argpm.size()==1 && argpm.at(0)==fldnames.at(k) && methpm.at(0)=="void")
						{
							if(isPrimitiveDataType(argpm.at(0)))
							{
								argpm.at(0) = getTypeName(argpm.at(0));
								if(!ptr)
								{
									if(argpm.at(0)=="long long")
									{
										argpm.at(0) = "long";
									}
									else if(argpm.at(0)=="unsigned long long")
									{
										argpm.at(0) = "unsignedlong";
									}
									else if(argpm.at(0).find("unsigned")==0)
									{
										StringUtil::replaceAll(argpm.at(0), " ", "");
									}
									else if(argpm.at(0)=="bool")
									{
										argpm.at(0) = "boolean";
									}
									obj_binding.append("\n<xsd:element name=\""+nam+"\" type=\"xsd:"+argpm.at(0)+"\"/>\n");
								}
								else
								{}
							}
							else if(argpm.at(0)=="Date")
							{
								if(!ptr)
								{}
								else
								{}
							}
							else if(argpm.at(0)=="BinaryData")
							{
								if(!ptr)
								{}
								else
								{}
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
								//StringUtil::replaceFirst(stlcnt," ","");

								string fqcn = ref.getFullyQualifiedClassName(stlcnt, clstruct->getNamespaces());
								contType += stlcnt + ",";

								string namespc;
								if(!ptr)
								{
									if(isPrimitiveDataType(fqcn))
									{
										fqcn = getTypeName(fqcn);
										if(fqcn=="long long")
										{
											fqcn = "long";
										}
										else if(fqcn=="unsigned long long")
										{
											fqcn = "unsignedlong";
										}
										else if(fqcn.find("unsigned")==0)
										{
											StringUtil::replaceAll(fqcn, " ", "");
										}
										else if(fqcn=="bool")
										{
											fqcn = "boolean";
										}
										string xstyp = fqcn;
										StringUtil::replaceAll(xstyp, "::", "_");
										obj_binding.append("<xsd:complexType name=\"ArrayOf"+clstruct->getTreatedClassName(true)+fqcn+"\"> <xsd:sequence>\n");
										obj_binding.append("<xsd:element maxOccurs=\"unbounded\" minOccurs=\"0\" name=\""+xstyp+"\" nillable=\"true\" type=\"xsd:"+fqcn+"\" />\n");
										obj_binding.append("</xsd:sequence> </xsd:complexType>\n");
									}
									else
									{
										string clnms = fqcn;
										if(fqcn.find("::")!=string::npos)
										{
											clnms = fqcn.substr(0, fqcn.find_last_of("::")+2);
											namespc = Reflection::getNameSpaceId(clnms, appname);
										}
										else
										{
											namespc = "tns";
										}
										string xstyp = fqcn;
										StringUtil::replaceAll(xstyp, "::", "_");
										obj_binding.append("<xsd:complexType name=\"ArrayOf"+clstruct->getTreatedClassName(true)+xstyp+"\"> <xsd:sequence>\n");
										obj_binding.append("<xsd:element maxOccurs=\"unbounded\" minOccurs=\"0\" name=\""+xstyp+"\" nillable=\"true\" type=\""+namespc+":"+xstyp+"\" />\n");
										obj_binding.append("</xsd:sequence> </xsd:complexType>\n");
										int_obj_binding.append(getXSDDefinitions(allclsmap, fqcn, ref, appname, tmpnmspc, allnmspcs, dfnmspc, resp));
									}
								}
								else
								{}
							}
							else
							{
								string namespc;
								string fqcn = ref.getFullyQualifiedClassName(argpm.at(0), clstruct->getNamespaces());
								if(!ptr)
								{
									string clnms = fqcn;
									if(fqcn.find("::")!=string::npos)
									{
										clnms = fqcn.substr(0, fqcn.find_last_of("::")+2);
										namespc = Reflection::getNameSpaceId(clnms, appname);
									}
									else
									{
										namespc = "tns";
									}
									string xstyp = fqcn;
									StringUtil::replaceAll(xstyp, "::", "_");
									obj_binding.append("\n<xsd:element name=\""+nam+"\" type=\""+namespc+":"+xstyp+"\"/>\n");
									int_obj_binding.append(getXSDDefinitions(allclsmap, fqcn, ref, appname, tmpnmspc, allnmspcs, dfnmspc, resp));
								}
								else
								{}
							}
						}
					}
				}
			}
		}
	}
	//obj_binding.append("</xsd:sequence>\n");
	//obj_binding.append("</xsd:complexType>\n");
	StringContext cntxt;
	cout << "nmspcid = " + nmspcid << endl;
	cout << "dfnmspc = " + dfnmspc << endl;
	string nmspcidval = Reflection::getNameSpaceIdValue(nmspcid);
	if(nmspcidval=="")
		nmspcidval = dfnmspc;
	cout << "nmspcidval = " + nmspcidval << endl;
	cntxt["WS_NMSPC"] = nmspcidval;
	cntxt["OBJ"] = clstruct->getTreatedClassName(false);
	cntxt["OBJ_MEMBERS"] = obj_binding;
	obj_binding = TemplateEngine::evaluate(resp+"templateObjBin.wsdl",cntxt);
	return int_obj_binding + obj_binding;
}



bool Reflection::isPrimitiveDataType(string& type)
{
	StringUtil::trim(type);
	if(type=="short" || type=="short int" || type=="signed short" || type=="signed short int"
			|| type=="unsigned short" || type=="unsigned short int"
			|| type=="signed" || type=="int" || type=="signed int"
			|| type=="unsigned" || type=="unsigned int" || type=="long"
			|| type=="long int" || type=="signed long" || type=="signed long int"
			|| type=="unsigned long" || type=="unsigned long int"
			|| type=="long long" || type=="long long int" || type=="signed long long"
			|| type=="signed long long int" || type=="unsigned long long"
			|| type=="unsigned long long int" || type=="long double" || type=="bool"
			|| type=="float" || type=="double" || type=="string" || type=="std::string"
			|| type=="char" || type=="signed char" || type=="unsigned char"
			|| type=="wchar_t")
	{
		type = getTypeName(type);
		return true;
	}
	return false;
}

string Reflection::getTypeName(string type)
{
	StringUtil::trim(type);
	if(type=="short" || type=="short int" || type=="signed short" || type=="signed short int")
	{
		return "short";
	}
	else if(type=="unsigned short" || type=="unsigned short int")
	{
		return "unsigned short";
	}
	else if(type=="signed" || type=="int" || type=="signed int")
	{
		return "int";
	}
	else if(type=="unsigned" || type=="unsigned int")
	{
		return "unsigned int";
	}
	else if(type=="long" || type=="long int" || type=="signed long" || type=="signed long int")
	{
		return "long";
	}
	else if(type=="unsigned long" || type=="unsigned long int")
	{
		return "unsigned long";
	}
	else if(type=="long long" || type=="long long int" || type=="signed long long" || type=="signed long long int")
	{
		return "long long";
	}
	else if(type=="unsigned long long" || type=="unsigned long long int")
	{
		return "unsigned long long";
	}
	else if(type=="long double")
	{
		return "long double";
	}
	return type;
}
