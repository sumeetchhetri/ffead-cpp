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



std::map<std::string,bool> Reflection::validcls;
//map<std::string,std::string> Reflection::clspaths;
std::map<std::string,int> Reflection::nmspcIds;
std::map<std::string,std::string> Reflection::nmspcIdVals;

Reflection::Reflection() {
	////logger = //LoggerFactory::getLogger("Reflection");
}

Reflection::~Reflection() {
	// TODO Auto-generated destructor stub
}

void Reflection::addMarker(const Marker& m) {
	handler.addMarker(m);
}

/*vector<std::string> Reflection::list(const std::string& cwd)
{
	FILE *pipe_fp;
	std::string command;
	std::vector<std::string> files;
	if(chdir(cwd.c_str())!=0)
		return files;
	command = ("find . \\( ! -name . -prune \\) \\( -type f -o -type l \\) -name '*.h' 2>/dev/null");
	//command = "ls -F1 "+cwd+"|grep '.h'";
	//logger << ("Searching directory " + cwd + " for pattern .h") << std::endl;
	if ((pipe_fp = popen(command.c_str(), "r")) == NULL)
	{
		printf("pipe open error in cmd_list\n");
		return files;
	}
	int t_char;
	std::string fileName;
	while ((t_char = fgetc(pipe_fp)) != EOF)
	{
		if(t_char!='\n')
		{
			std::stringstream ss;
			ss << (char)t_char;
			std::string temp;
			ss >> temp;
			fileName.append(temp);
		}
		else if(fileName!="")
		{
			StringUtil::replaceFirst(fileName,"*","");
			StringUtil::replaceFirst(fileName,"./","");
			if(fileName.find("~")==std::string::npos)
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
}*/

void Reflection::collectInfo(std::string data, const std::string& flag, ClassStructure& cls, const std::vector<std::string>& mrktxt)
{
	if(RegexUtil::find(data, "[ \t]*template[ \t]*<[ \t]*")!=-1)
		return;
	RegexUtil::replace(data, "[ \t]*const[; \t]+", "");

	if(data.find("(")!=std::string::npos && data.find(")")!=std::string::npos)
	{
		MethStructure ps;
		ps.decl = data;
		std::string t = data.substr(0, data.find("("));
		t = t.substr(0, t.find_last_of(" "));
		if(t.find("const ")==0) {
			t = t.substr(6);
		}
		if(t.find("virtual ")==0) {
			t = t.substr(8);
		}
		if(t.find("~")!=std::string::npos) {
			t = "void";
		}
		StringUtil::trim(t);
		ps.retType = t;
		for (int pi = 0; pi < (int)mrktxt.size(); ++pi) {
			std::string prg = mrktxt.at(pi);
			try {
				prg = prg.substr(0, prg.length()-1);
				Marker m = handler.processMarker(prg, 2);
				if(m.getName()!="")
				{
					std::cout << prg << " - Method marker Valid" << std::endl;
					ps.markers[m.getName()].push_back(m);
				}
			} catch(const std::exception& c) {
				std::cout << prg << " - " <<  c.what() << std::endl;
			}
		}
		if(flag=="public")
		{
			cls.pubms.push_back(ps);
		}
		else if(flag=="protected")
		{
			cls.proms.push_back(ps);
		}
		else if(flag=="private")
		{
			cls.prims.push_back(ps);
		}
	}
	else
	{
		PropStructure ps;
		ps.decl = data;
		for (int pi = 0; pi < (int)mrktxt.size(); ++pi) {
			std::string prg = mrktxt.at(pi);
			try {
				prg = prg.substr(0, prg.length()-1);
				Marker m = handler.processMarker(prg, 1);
				if(m.getName()!="")
				{
					std::cout << prg << " - Property marker Valid" << std::endl;
					ps.markers[m.getName()].push_back(m);
				}
			} catch(const std::exception& c) {
				std::cout << prg << " - " <<  c.what() << std::endl;
			}
		}
		if(flag=="public")
		{
			cls.pubps.push_back(ps);
		}
		else if(flag=="protected")
		{
			cls.props.push_back(ps);
		}
		else if(flag=="private")
		{
			cls.prips.push_back(ps);
		}
	}

	if(flag=="public")
	{
		cls.pub.push_back(data);
	}
	else if(flag=="protected")
	{
		cls.pro.push_back(data);
	}
	else if(flag=="private")
	{
		cls.pri.push_back(data);
	}
}

int Reflection::findless(const int& a, const int& b, const int& c)
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


void Reflection::emptyBlocks(std::string& data, size_t start)
{
	if(data.find("{")!=std::string::npos) {
		size_t nstart = data.find("{", start+1);
		if(nstart!=std::string::npos)
		{
			start = nstart;
		}
		else
		{
			std::string temp = data.substr(0, start);
			std::string temp1 = data.substr(start);
			RegexUtil::replace(temp1, "\\{[^}]*\\}", ";");
			data = temp + temp1;
			start = 0;
		}
		emptyBlocks(data, start);
	}
}

void Reflection::handleNamespace(std::string data, std::string namepsc, std::map<std::string, ClassStructure>& clsvec, std::map<std::string, std::vector<std::string> >& glbnmspcs, std::vector<std::string> pragmas)
{
	StringUtil::trim(data);
	if(data.length()>0 && data.at(0)==';') {
		data = data.substr(1);
	}
	StringUtil::trim(data);
	if(data=="")return;
	std::string nmspc;
	int nmspcst = RegexUtil::find(data, "[ \t]*using[ \t]*namespace[ \t]*[^;]+;");
	if(nmspcst!=-1)
	{
		std::string temp = data.substr(nmspcst);
		nmspcst += temp.find(" namespace");
	}
	std::cout << namepsc << "||" << data << std::endl;
	if(clsvec.find(namepsc)==clsvec.end() && RegexUtil::find(data, "^[ \t]*#[ \t]*pragma[ \t]*[^`]+`", true)==0)
	{
		int spos, epos;
		RegexUtil::find(data, "^[ \t]*#[ \t]*pragma[ \t]*[^`]+`", spos, epos, true);
		pragmas.push_back(data.substr(spos, epos-spos));
		std::string sdata = data.substr(0, spos);
		data = data.substr(epos);
		handleNamespace(sdata, namepsc, clsvec, glbnmspcs, pragmas);
		handleNamespace(data, namepsc, clsvec, glbnmspcs, pragmas);
	}
	else if(RegexUtil::find(data, "^[ \t]*using[ \t]*namespace[ \t]*[^;]+;")==0)
	{
		std::string nmspace = data.substr(0, data.find(";"));
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
		handleNamespace(data, namepsc, clsvec, glbnmspcs, pragmas);
	}
	else if(data.find("namespace ")==0)
	{
		pragmas.clear();
		std::string temp = data.substr(0, data.find("{"));
		StringUtil::trim(temp);
		StringUtil::replaceFirst(temp,"namespace ","");
		nmspc = namepsc + temp+"::";
		//namepsc += temp+"::";
		data = data.substr(data.find("{"));
		std::string nmdata = data;
		size_t cbst = nmdata.find("{", 1);
		size_t enind = nmdata.find("}");
		bool flag = false;
		if(enind!=std::string::npos && cbst!=std::string::npos && enind>cbst)
		{
			enind = 0;
			while(nmdata.find("}", enind)!=std::string::npos)
			{
				std::string test = nmdata.substr(0, nmdata.find("}", enind)+1);
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
		else if(enind!=std::string::npos && nmdata.find("{")!=std::string::npos)
		{
			flag = true;
		}
		if(flag)
		{
			int st = data.find("{");
			int en = enind;
			std::string tdata = data.substr(en+1);
			data = data.substr(st+1, en-st-1);
			StringUtil::trim(data);
			StringUtil::trim(tdata);
			std::cout << "nmspc = " << nmspc << std::endl;
			std::cout << data << std::endl;
			std::cout << tdata << std::endl;
			handleNamespace(data, nmspc, clsvec, glbnmspcs, pragmas);
			handleNamespace(tdata, namepsc, clsvec, glbnmspcs, pragmas);
		}
		else
		{
			std::cout << "error" << std::endl;
		}
	}
	else if(data.find("class ")==0)
	{
		std::vector<std::string> results;
		std::string temp = data.substr(0, data.find("{"));
		StringUtil::trim(temp);
		StringUtil::replaceFirst(temp,"class ","");
		StringUtil::replaceFirst(temp,":"," ");
		RegexUtil::replace(temp, "[ ]+", " ");
		StringUtil::split(results, temp, (" "));
		std::string classN,baseClassN,bcvisib,namSpc;
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
		//namepsc += temp+"::";
		data = data.substr(data.find("{"));
		std::string nmdata = data;
		size_t cbst = nmdata.find("{", 1);
		size_t enind = nmdata.find("}");
		bool flag = false;
		if(enind!=std::string::npos && cbst!=std::string::npos && enind>cbst)
		{
			enind = 0;
			while(nmdata.find("}", enind)!=std::string::npos)
			{
				std::string test = nmdata.substr(0, nmdata.find("}", enind)+1);
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
		else if(enind!=std::string::npos && nmdata.find("{")!=std::string::npos)
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
			for (int pi = 0; pi < (int)pragmas.size(); ++pi) {
				std::string prg = pragmas.at(pi);
				try {
					prg = prg.substr(0, prg.length()-1);
					Marker m = handler.processMarker(prg, 0);
					if(m.getName()!="")
					{
						std::cout << prg << " - Valid" << std::endl;
					}
					cstruc.markers[m.getName()].push_back(m);
				} catch(const std::exception& c) {
					std::cout << prg << " - " <<  c.what() << std::endl;
				}
			}
			clsvec[nmspc] = cstruc;
			int st = data.find("{");
			int en = enind;
			std::string tdata = data.substr(en+1);
			data = data.substr(st+1, en-st-1);
			StringUtil::trim(data);
			StringUtil::trim(tdata);
			std::cout << "classnmpsc = " << nmspc << std::endl;
			std::cout << data << std::endl;
			std::cout << tdata << std::endl;
			pragmas.clear();
			handleNamespace(data, nmspc, clsvec, glbnmspcs, pragmas);
			handleNamespace(tdata, namepsc, clsvec, glbnmspcs, pragmas);
		}
		else
		{
			std::cout << "error" << std::endl;
		}
		pragmas.clear();
	}
	else if(data.find(" namespace ")!=std::string::npos && nmspcst!=(int)data.find(" namespace "))
	{
		std::string sdata = data.substr(0, data.find(" namespace "));
		pragmas.clear();
		handleNamespace(sdata, namepsc, clsvec, glbnmspcs, pragmas);
		handleNamespace(data.substr(data.find(" namespace ")), namepsc, clsvec, glbnmspcs, pragmas);
	}
	else if(data.find(" class ")!=std::string::npos)
	{
		std::string sdata = data.substr(0, data.find(" class "));
		if(pragmas.size()>0)
		{
			std::string ssdata = sdata;
			RegexUtil::replace(ssdata, "[ \t]+", "");
			RegexUtil::replace(ssdata, "\n", "");
			if(ssdata!="")
			{
				pragmas.clear();
			}
		}
		handleNamespace(sdata, namepsc, clsvec, glbnmspcs, pragmas);
		handleNamespace(data.substr(data.find(" class ")), namepsc, clsvec, glbnmspcs, pragmas);
		pragmas.clear();
	}
	else if(clsvec.find(namepsc)!=clsvec.end())
	{
		emptyBlocks(data, 0);
		//TODO do we need this??? RegexUtil::replace(data, "\"[^\"]+\"", "\"\"");
		std::vector<std::string> parts = StringUtil::splitAndReturn<std::vector<std::string> >(data, ";");
		if(parts.size()>0)
		{
			std::string flag = "private";
			for (int var = 0; var < (int)parts.size(); ++var) {
				std::string partsva = parts.at(var);

				int bps, bpe;
				RegexUtil::find(partsva, "[ \t]*public[ \t]*:", bps, bpe);
				if(bps==0) {
					flag = "public";
					partsva = partsva.substr(bpe);
				}
				RegexUtil::find(partsva, "[ \t]*private[ \t]*:", bps, bpe);
				if(bps==0) {
					flag = "private";
					partsva = partsva.substr(bpe);
				}
				RegexUtil::find(partsva, "[ \t]*protected[ \t]*:", bps, bpe);
				if(bps==0) {
					flag = "protected";
					partsva = partsva.substr(bpe);
				}

				std::vector<std::string> mrktxt;
				while(RegexUtil::find(partsva, "^[ \t]*#[ \t]*pragma[ \t]*[^`]+`", true)==0)
				{
					int spos, epos;
					RegexUtil::find(partsva, "^[ \t]*#[ \t]*pragma[ \t]*[^`]+`", spos, epos, true);
					mrktxt.push_back(partsva.substr(spos, epos-spos));
					partsva = partsva.substr(0, spos) + partsva.substr(epos);
				}

				int pust, puen;
				RegexUtil::find(partsva, "[ \t]*public[ \t]*:", pust, puen);
				int pvst, pven;
				RegexUtil::find(partsva, "[ \t]*private[ \t]*:", pvst, pven);
				int prst, pren;
				RegexUtil::find(partsva, "[ \t]*protected[ \t]*:", prst, pren);

				int pls = findless(pvst, prst, pust);
				std::string pubdata, privdata, protdata;
				if(pls!=-1 && pls == pvst)
				{
					std::string predat = partsva.substr(0, pvst);
					std::string postdat = partsva.substr(pven);

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
					std::string predat = partsva.substr(0, prst);
					std::string postdat = partsva.substr(pren);

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
					std::string predat = partsva.substr(0, pust);
					std::string postdat = partsva.substr(puen);

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
						pubdata = partsva;
					}
					else if(flag == "private")
					{
						privdata = partsva;
					}
					else
					{
						protdata = partsva;
					}
				}
				if(pubdata!="")
				{
					flag = "public";
					StringUtil::trim(pubdata);
					collectInfo(pubdata, "public", clsvec[namepsc], mrktxt);
				}
				else if(privdata!="")
				{
					flag = "private";
					StringUtil::trim(privdata);
					collectInfo(privdata, "private", clsvec[namepsc], mrktxt);
				}
				else if(protdata!="")
				{
					flag = "protected";
					StringUtil::trim(protdata);
					collectInfo(protdata, "protected", clsvec[namepsc], mrktxt);
				}
			}
		}
	}
	return;
}


std::map<std::string, ClassStructure> Reflection::getClassStructures(const std::string& className, const std::string& appName)
{
	std::map<std::string, ClassStructure> clsvec;
	std::string data;
	std::ifstream infile;
	infile.open(className.c_str(), std::ios::binary);
	std::string allcont;
	std::vector<std::string> pragmas;
	if(infile.is_open())
	{
		bool commstrts = false;
		std::string prevline;
		while(getline(infile, data))
		{
			StringUtil::trim(data);
			if(data.length()>0 && data.at(data.length()-1)=='\\')
			{
				while(data.length()>0 && data.at(data.length()-1)=='\\')
				{
					data = data.substr(0, data.length()-1);
					StringUtil::trim(data);
				}
				prevline = prevline + " " + data;
				continue;
			}
			else if(prevline!="")
			{
				data = prevline + " " + data;
				prevline = "";
			}
			RegexUtil::replace(data, "[\t]+", " ");
			RegexUtil::replace(data, "[ ]+", " ");
			if(RegexUtil::find(data, "^[ \t]*#[ \t]*pragma[ \t]*.*", true)==-1)
			{
				RegexUtil::replace(data, "\"[^\"]+\"", "\"\"");
			}
			RegexUtil::replace(data, "\"[\"]+\"", "\"\"");
			RegexUtil::replace(data, "[ \t]*#[ \t]*include[ \t]*.*", " ", true);
			RegexUtil::replace(data, "[ \t]*#[ \t]*define[ \t]*.*", " ", true);
			RegexUtil::replace(data, "[ \t]*#[ \t]*undef[ \t]*.*", " ", true);
			RegexUtil::replace(data, "[ \t]*#[ \t]*if[ \t]*.*", " ", true);
			RegexUtil::replace(data, "[ \t]*#[ \t]*ifdef[ \t]*.*", " ", true);
			RegexUtil::replace(data, "[ \t]*#[ \t]*ifndef[ \t]*.*", " ", true);
			RegexUtil::replace(data, "[ \t]*#[ \t]*error[ \t]*.*", " ", true);
			RegexUtil::replace(data, "[ \t]*#[ \t]*line[ \t]*.*", " ", true);
			RegexUtil::replace(data, "[ \t]*#[ \t]*using[ \t]*.*", " ", true);
			RegexUtil::replace(data, "[ \t]*#[ \t]*elif[ \t]*.*", " ", true);
			RegexUtil::replace(data, "[ \t]*#[ \t]*import[ \t]*.*", " ", true);
			RegexUtil::replace(data, "[ \t]*#[ \t]*else[ \t]*.*", " ", true);
			RegexUtil::replace(data, "[ \t]*#[ \t]*endif[ \t]*.*", " ", true);
			RegexUtil::replace(data, "[\t]+", " ");
			RegexUtil::replace(data, "[ ]+", " ");

			if(data.find("//")==0)
			{
				continue;
			}
			else if(data.find("//")!=std::string::npos)
			{
				data = data.substr(0, data.find("//"));
			}
			if(commstrts && data.find("*/")!=std::string::npos)
			{
				commstrts = false;
				data = data.substr(data.find("*/")+2);
			}
			while(data.find("/*")!=std::string::npos)
			{
				if(data.find("*/")!=std::string::npos)
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
				if(RegexUtil::find(data, "^[ \t]*#[ \t]*pragma[ \t]*.*", true)==0)
				{
					allcont.append("`");
				}
				else
				{
					allcont.append(" ");
				}
			}
		}
		infile.close();

		data = allcont;
		/*
		RegexUtil::replace(data, "[ \t]*#include[ \t]*\"[^\"]+\"", " ");
		RegexUtil::replace(data, "[ \t]*#include[ \t]*<[^<>]+>", " ");
		RegexUtil::replace(data, "[ \t]*#define[ \t]*[a-zA-Z0-9_]*[ \t]*[0-9]*", " ");
		RegexUtil::replace(data, "[ \t]*#define[ \t]*[a-zA-Z0-9_]*[ \t]*\"[^\"]+\"", " ");
		RegexUtil::replace(data, "[ \t]*#ifndef[ \t]*[a-zA-Z0-9_]*[ \t]*", " ");
		RegexUtil::replace(data, "[ \t]*#endif[ \t]*", " ");*/

		std::vector<std::string> typedefs = RegexUtil::search(data, "[ \t]*typedef[ \t]*[^;]+;");
		RegexUtil::replace(data, "[ \t]*typedef[ \t]*[^;]+;", " ");
		RegexUtil::replace(data, "[ \t]*friend[ \t]*[^;]+;", " ");

		std::string file = className.substr(className.find_last_of("/")+1);

		std::map<std::string, std::vector<std::string> > glbnmspcs;
		handleNamespace(data, "", clsvec, glbnmspcs, pragmas);
		std::map<std::string, ClassStructure>::iterator it;
		std::vector<std::string> remnmspcs;
		for (it=clsvec.begin();it!=clsvec.end();++it) {

			if(it->second.pub.size()>0)
			{
				it->second.appName = appName;
				it->second.incfile = className;
				clspaths[it->second.getTreatedClassName(true)] = file;
				StringUtil::trim(it->second.nmSpc);
				if(it->second.nmSpc.find("::")!=std::string::npos)
				{
					it->second.nmSpc = it->second.nmSpc.substr(0, it->second.nmSpc.find_last_of("::")+1);
				}
				if(it->second.nmSpc!="" && nmspcIds.find(appName+it->second.nmSpc)==nmspcIds.end())
				{
					nmspcIds[appName+it->second.nmSpc] = nmspcIds.size()+1;
					std::string nmspcvv = it->second.nmSpc;
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

			std::vector<std::string> nnspcs;
			if(glbnmspcs.find(it->second.nmSpc)!=glbnmspcs.end())
			{
				copy(glbnmspcs[it->second.nmSpc].begin(), glbnmspcs[it->second.nmSpc].end(), std::back_inserter(nnspcs));
			}
			copy(it->second.namespaces.begin(), it->second.namespaces.end(), std::back_inserter(nnspcs));
			it->second.namespaces = nnspcs;
			std::cout << "=========================================" << std::endl;
			it->second.toString();
			std::cout << "=========================================" << std::endl;
		}
		for (int var = 0; var < (int)remnmspcs.size(); ++var) {
			clsvec.erase(remnmspcs.at(var));
		}
	}
	return clsvec;
}


/*bool Reflection::generateClassInfoFromDD(const std::string& alldata)
{
	std::string data;
	std::string flag = "";
	std::stringstream ss;
	ss << alldata;
	while (getline(ss, data))
	{
		size_t tes;
		if ((tes = data.find("class")) != std::string::npos)
		{
			std::vector<std::string> results;
			StringUtil::replaceFirst(data,":","");
			StringUtil::split(results, data, (" "));
			this->classN = results.at(1);
			if(results.size()>3)
			{
				this->bcvisib = results.at(2);
				this->baseClassN = results.at(3);
				////logger << results.size() << std::flush;
			}
			//StringUtil::split(results, data, (": "));
		}
		else if ((tes = data.find("}")) != std::string::npos)
			break;
		else if ((tes = data.find("public")) != std::string::npos)
			flag = "public";
		else if ((tes = data.find("protected")) != std::string::npos)
			flag = "protected";
		else if ((tes = data.find("private")) != std::string::npos)
			flag = "private";
		else
		{
			if (flag == "" && data != "{")
				flag = "private";
			collectInfo(data, flag);
		}
	}
	////logger << pub.size() << pri.size() << pro.size() << std::flush;
	if (this->pub.size() > 0 || this->pri.size() > 0 || this->pro.size() > 0)
		return true;
	else
		return false;
}*/


propMap Reflection::getDbTableInfo(const std::string& file)
{
	propMap tabInfo;
	std::string temp;
	std::vector<std::string> all;
	std::ifstream infile;
	infile.open(file.c_str(), std::ios::binary);
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

std::vector<std::string> Reflection::getAfcObjectData(ClassStructure& classStructure, const bool& object, std::vector<std::string>& privf, bool &isOpForSet)
{
	isOpForSet = false;
	std::vector<std::string> refDef;
	std::vector<std::string> publf, protf ,publm, privm, protm;

	size_t tes;
	if (classStructure.pub.size() > 0)
	{
		for (unsigned int i = 0; i < classStructure.pub.size(); i++)
		{
			if((tes=classStructure.pub.at(i).find("("))!=std::string::npos && (tes=classStructure.pub.at(i).find(")"))!=std::string::npos)
			{
				publm.push_back(classStructure.pub.at(i));

				std::string meth = classStructure.pub.at(i);
				StringUtil::replaceFirst(meth,";","");
				RegexUtil::replace(meth, "[\t]+", " ");
				RegexUtil::replace(meth, "[ ]+", " ");
				RegexUtil::replace(meth, "[ \t]*,[ \t]*", ",");
				meth = meth.substr(0,meth.find("("));
				if(meth.find("operator")!=std::string::npos)
				{
					if(meth.find("<")!=std::string::npos)
					{
						isOpForSet = true;
					}
					else if(meth.find(">")!=std::string::npos)
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
			if((tes=classStructure.pri.at(i).find("("))!=std::string::npos && (tes=classStructure.pri.at(i).find(")"))!=std::string::npos)
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
			if((tes=classStructure.pro.at(i).find("("))!=std::string::npos && (tes=classStructure.pro.at(i).find(")"))!=std::string::npos)
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

/*vector<std::string> Reflection::getAfcObjectData(const std::string& className, const bool& object)
{
	std::vector<std::string> refDef;
	if (!generateClassInfo(className))
	{
		return refDef;
	}
	std::vector<std::string> publf, privf, protf ,publm, privm, protm;

	size_t tes;
	if (this->pub.size() > 0)
	{
		for (unsigned int i = 0; i < this->pub.size(); i++)
		{
			if((tes=this->pub.at(i).find("("))!=std::string::npos && (tes=this->pub.at(i).find(")"))!=std::string::npos)
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
			if((tes=this->pri.at(i).find("("))!=std::string::npos && (tes=this->pri.at(i).find(")"))!=std::string::npos)
			{
				privm.push_back(this->pri.at(i));
			}
		}
	}
	if (this->pro.size() > 0)
	{
		for (unsigned int i = 0; i < this->pro.size(); i++)
		{
			if((tes=this->pro.at(i).find("("))!=std::string::npos && (tes=this->pro.at(i).find(")"))!=std::string::npos)
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

std::string Reflection::generateClassDefinitionsAll(std::map<std::string, std::map<std::string, ClassStructure> >& clsstrucMaps, std::string &includeRef, const std::vector<std::string>& apps)
{
	std::string ret = "";
	//includeRef = "#ifndef REFLECTOR_H_\n#define REFLECTOR_H_\n#include \"ClassInfo.h\"\n#include \"string\"\n#include \"Method.h\"\n#include \"Field.h\"\n";
	//includeRef += "#include \"XmlParser.h\"\n#include <stdio.h>\n#include <sys/wait.h>\n#include <stdexcept>\n#include <execinfo.h>\n#include <dlfcn.h>\n#include <cxxabi.h>\n#include <stdio.h>\n#include <stdlib.h>\n#include \"string\"\n#include <sstream>\n#include <typeinfo>\n";
	std::string typedefs,classes,methods,opers;
	std::string inc = "#include \"AppDefines.h\"\n#include \"Reflector.h\"\n#include \"string\"\n#include \"Method.h\"\n#include \"Field.h\"\n";
	ret += "extern \"C\"\n{\n";
	for (unsigned int var = 0; var < apps.size(); ++var)
	{
		////logger << "\nstarting for classes " << all.size() << "\n" << std::flush;
		std::string app = StringUtil::trimCopy(apps.at(var));
		StringUtil::replaceAll(app, "-", "_");
		RegexUtil::replace(app, "[^a-zA-Z0-9_]+", "");
		ret += this->generateClassDefinition(clsstrucMaps[StringUtil::trimCopy(apps.at(var))],inc,typedefs,classes,methods,opers,app);

	}
	/*ret += "ClassInfo Reflector::getClassInfo(std::string className)\n{\n";
	ret += classes;
	ret += "\n\treturn info;\n}\n";
	ret += "void* Reflector::invokeMethod(void* instance,Method method,vals values,bool cleanvals)\n{\n";
	ret += methods;
	ret += "\n\treturn returnValue;\n}\n";
	ret += "bool Reflector::instanceOf(void* instance,std::string className)\n{\n";
	ret += "Trace tr;\nstd::string cn = tr.getClassName(instance);\nif(cn==className)\nreturn true;\nelse\nreturn false;\n}\n";*/
	ret +=  methods+opers+ "\n}\n";
	ret = (inc+ret);
	return ret;
}

std::string Reflection::generateClassDefinitions(std::map<std::string, ClassStructure>& allclsmap, std::string &includesDefs, std::string &typedefs, std::string &classes, std::string &methods, std::string &opers, const std::string& app)
{
	return "";
}

std::string Reflection::generateClassDefinition(std::map<std::string, ClassStructure>& allclsmap, std::string &includesDefs, std::string &typedefs, std::string &classes, std::string &methods, std::string &opers, const std::string& app)
{
	std::string refDef, testStr, teststrfuncs;

	std::map<std::string, ClassStructure>::iterator it;
	for (it=allclsmap.begin();it!=allclsmap.end();++it)
	{
		int opcounter = 1, methcounter = 1, fldcounter = 1;
		ClassStructure classStructure = it->second;
		classStructure.prosetser = false;
		includesDefs += "#include \"" + getClassPath(it->second.getTreatedClassName(true)) + "\"\n";
		//classes += "\tif(className==\""+classStructure.getTreatedClassName(true)+"\")\n\t\treturn get"+classStructure.getTreatedClassName(true)+"();\n";

		//string structinf = "\nstruct struct"+classStructure.classN+"{\n";

		testStr += "void test"+app+"_fcrcif_" + classStructure.getTreatedClassName(true) + "()\n{\n";
		testStr += "ClassInfo ci = "+app+"_fcrcif_" + classStructure.getTreatedClassName(true) + "();\n";
		testStr += "assert(ci.getClassName()==\"" + classStructure.classN+"\");\n";
		testStr += "assert(ci.getNamespace()==\"" + classStructure.nmSpc+"\");\n";
		testStr += "assert(ci.getBase()==\"" + classStructure.bcvisib + " " + classStructure.baseClassN +"\");\n";
		testStr += ("\nConstructor ctor;\nMethod me;\nField f;\n");
		testStr += ("args argu;\n");

		refDef += "ClassInfo "+app+"_" + classStructure.getTreatedClassName(true) + "()\n{\nClassInfo ci;";
		refDef += ("\nci.setClassName(\"" + classStructure.classN + "\");");
		refDef += ("\nci.setNamespace(\"" + classStructure.nmSpc + "\");");
		//refDef += ("\nci.setInstance(new " + classStructure.getTreatedClassName(true) + ");");
		refDef += ("\nci.setBase(\"" + classStructure.bcvisib + " " + classStructure.baseClassN + "\");");
		refDef += ("\nConstructor ctor;\nMethod me;\nField f;\n");
		refDef += ("args argu;\n");
		std::string publf, privf, protf ,publm, privm, protm;
		std::string meth,fld;
		bool ctorisp = false,ddtorisp = false;;
		if (classStructure.pubps.size() > 0 || classStructure.pubms.size() > 0)
		{
			for (unsigned int i = 0; i < classStructure.pubms.size(); i++) {
				MethStructure ms = classStructure.pubms.at(i);
				std::string pubdecl = ms.decl;
				if(pubdecl.find("~")!=std::string::npos)
				{
					std::string mmn = app+ "_" + classStructure.getTreatedClassName(true) + "_m" +
							CastUtil::lexical_cast<std::string>(methcounter++);
					methods += "\nvoid " +mmn+"(void* instance)\n{";
					methods += "\n\t"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\n\t";
					methods += "_obj->~"+classStructure.getTreatedClassName(false)+"();";
					methods += "\n}";
					ddtorisp = true;
					refDef += ("ci.setDestRefName(\""+mmn+"\");\n");
				}
				else
				{
					refDef += ("ctor.clear();\nme.clear();\n");
					testStr += ("ctor.clear();\nme.clear();\n");
					publm += pubdecl;
					meth = pubdecl;
					StringUtil::replaceFirst(meth,";","");

					std::string argts = meth.substr(meth.find("("),meth.find(")")-meth.find("("));
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
					if(methpm.at(0).at(methpm.at(0).length()-1)=='&')
						methpm.at(0) = methpm.at(0).substr(0, methpm.at(0).length()-1);
					for(unsigned int j = 0; j < argp.size(); j++)
					{
						if(argp.at(j)!="" && argp.at(j)!="(")
						{
							std::string argpmtemp = argp.at(j);
							StringUtil::trim(argpmtemp);
							std::string typ;
							size_t markerindx = argpmtemp.find_last_of("`");
							size_t spindx = argpmtemp.find_last_of(" ");
							size_t ptindx = argpmtemp.find_last_of("*");
							size_t rfindx = argpmtemp.find_last_of("&");
							size_t teindx = argpmtemp.find_last_of(">");

							std::string markerstr;
							if(markerindx!=std::string::npos)
							{
								markerstr = argpmtemp.substr(0, markerindx);
								argpmtemp = argpmtemp.substr(markerindx+1);
								StringUtil::trim(markerstr);
								markerstr += "`";

								StringUtil::trim(argpmtemp);
								spindx = argpmtemp.find_last_of(" ");
								ptindx = argpmtemp.find_last_of("*");
								rfindx = argpmtemp.find_last_of("&");
								teindx = argpmtemp.find_last_of(">");
							}

							if(ptindx==argpmtemp.length()-1 ||
									(ptindx!=std::string::npos && teindx!=std::string::npos && ptindx>teindx) ||
									(teindx==std::string::npos && ptindx!=std::string::npos && spindx!=std::string::npos && ptindx>spindx))
							{	typ = "*";
								argpmtemp = argpmtemp.substr(0, ptindx);
							}
							else if(rfindx==argpmtemp.length()-1 ||
									(rfindx!=std::string::npos && teindx!=std::string::npos && rfindx>teindx) ||
									(teindx==std::string::npos && rfindx!=std::string::npos && spindx!=std::string::npos && rfindx>spindx))
							{
								typ = "&";
								StringUtil::replaceLast(argpmtemp,"&","");
							}
							if(teindx!=std::string::npos && spindx!=std::string::npos && teindx>spindx)
							{
								argpmtemp = argpmtemp.substr(0, teindx);
							}
							else if(spindx!=std::string::npos)
							{
								argpmtemp = argpmtemp.substr(0, spindx);
							}
							StringUtil::trim(argpmtemp);
							argpm.push_back(markerstr+argpmtemp+typ);
						}
					}
					std::string typdefName,methsd,valsd,valsa,valsades;
					//bool ctor = false;
					if(methpm.size()>0 && methpm.at(0).find("virtual")!=std::string::npos)
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
						else if(meth.find(" operator")==std::string::npos)
						{
							std::string fqcn = getFullyQualifiedClassName(methpm.at(0), classStructure.namespaces);
							refDef += ("me.setReturnType(\""+fqcn+"\");\n");
							typedefs += ("typedef " + methpm.at(0) + " ");
							methsd += (classStructure.getTreatedClassName(true)+methpm.at(1));
							ms.name = methpm.at(1);
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
							valsd += "\t\t"+(methpm.at(j) + " *_" + CastUtil::lexical_cast<std::string>(j-1)+" = ("+methpm.at(j)+"*)values.at("+CastUtil::lexical_cast<std::string>(j-2)+");");
							valsa += "*_" + CastUtil::lexical_cast<std::string>(j-1);
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
						if(tmpltarg && argpm.at(j).find(">")!=std::string::npos)
						{
							std::string ttt = argpmtemp.at(argpmtemp.size()-1)+","+argpm.at(j);
							argpmtemp.at(argpmtemp.size()-1) = ttt;
						}
						else if(argpm.at(j).find("<")!=std::string::npos)
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
					int argNumm = 1;
					for(unsigned int j = 0; j < argpm.size(); j++)
					{
						std::vector<std::string> argtn;
						std::string type12 = "";
						size_t ptindx = argpm.at(j).find_last_of("*");
						size_t rfindx = argpm.at(j).find_last_of("&");
						if(ptindx==argpm.at(j).length()-1)
							type12 = "*";
						else if(rfindx==argpm.at(j).length()-1)
							type12 = "&";
						//StringUtil::split(argtn, argpm.at(j), (" "));
						////logger << "testing::::" << argpm.at(j) << argtn.size();
						//StringUtil::replaceAll(argtn.at(0)," ","");
						std::string mdecl = argpm.at(j);
						if(meth.find(" operator")==std::string::npos)
						{
							std::vector<Marker> mrkrs;
							while(RegexUtil::find(mdecl, "^[ \t]*#[ \t]*pragma[ \t]*[^`]+`", true)==0)
							{
								int spos, epos;
								RegexUtil::find(mdecl, "^[ \t]*#[ \t]*pragma[ \t]*[^`]+`", spos, epos, true);
								std::string argprg = mdecl.substr(spos, epos-spos-1);
								mdecl = mdecl.substr(0, spos) + mdecl.substr(epos);

								try {
									Marker m = handler.processMarker(argprg, 3);
									if(m.getName()!="")
									{
										std::cout << argprg << " - Valid" << std::endl;
										ms.argMarkers[argNumm][m.getName()].push_back(m);
									}
								} catch(const std::exception& c) {
									std::cout << argprg << " - " <<  c.what() << std::endl;
								}
							}

							std::string fqcn = getFullyQualifiedClassName(mdecl, classStructure.namespaces);
							ms.argstypes[argNumm++] = fqcn;
							refDef += ("argu.push_back(\""+fqcn+"\");\n");
							testStr += ("argu.push_back(\""+fqcn+"\");\n");
						}
						if(type12=="*")
						{
							StringUtil::replaceLast(mdecl,"*","");
							std::string fqcn = getFullyQualifiedClassName(mdecl, classStructure.namespaces);
							valsd += "\t\t"+(fqcn  + " *_" + CastUtil::lexical_cast<std::string>(j)+" = ("+fqcn+"*)values.at("+CastUtil::lexical_cast<std::string>(j)+");\n");
							StringUtil::replaceAll(mdecl,"*","");
						}
						else if(type12=="&")
						{
							StringUtil::replaceLast(mdecl,"&","");
							std::string fqcn = getFullyQualifiedClassName(mdecl, classStructure.namespaces);
							valsd += "\t\t"+(fqcn + " *_" + CastUtil::lexical_cast<std::string>(j)+" = ("+fqcn+"*)values.at("+CastUtil::lexical_cast<std::string>(j)+");\n");
							StringUtil::replaceAll(mdecl,"&","");
						}
						else
						{
							std::string fqcn = getFullyQualifiedClassName(mdecl, classStructure.namespaces);
							valsd += "\t\t"+(fqcn + " *_" + CastUtil::lexical_cast<std::string>(j)+" = ("+fqcn+"*)values.at("+CastUtil::lexical_cast<std::string>(j)+");\n");
						}
						if(type12=="*")
							valsa += "_" + CastUtil::lexical_cast<std::string>(j);
						else
							valsa += "*_" + CastUtil::lexical_cast<std::string>(j);
						std::string fqcn = getTreatedFullyQualifiedClassName(mdecl, classStructure.namespaces);
						valsades += "\t\tif(cleanvals)delete ("+fqcn+"*)values.at("+CastUtil::lexical_cast<std::string>(j)+");\n";
						//if(methpm.at(0)!=classStructure.getFullyQualifiedClassName())
						//{
							typedefs += mdecl;
							typdefName += mdecl;
							//std::string fqcn = getTreatedFullyQualifiedClassName(mdecl, classStructure.namespaces);
							methsd += fqcn + (type12=="*"?"ptr":"");
							if(j!=argpm.size()-1)
							{
								typdefName += ",";
								valsa += ",";
							}
						//}
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
					{
						if(methpm.at(0).find(" ")!=std::string::npos)
						{
							methpm.at(0) = methpm.at(0).substr(0, methpm.at(0).find_last_of(" "));
						}

						/*if(meth.find("operator>")!=std::string::npos && meth.find("operator>=")==std::string::npos && meth.find("operator=>")==std::string::npos)
						{
							classStructure.prosetser = true;
						}
						else */if(meth.find("operator<")!=std::string::npos/* && meth.find("operator<=")==std::string::npos && meth.find("operator=<")==std::string::npos*/)
						{
							classStructure.prosetser = true;
						}

						if(meth.find("<<")!=std::string::npos)
						{

						}
						else if(meth.find(">>")!=std::string::npos)
						{

						}
						else if(meth.find("<=")!=std::string::npos)
						{
							std::string omn = app+ "_" + classStructure.getTreatedClassName(true) + "_o" +
									CastUtil::lexical_cast<std::string>(opcounter++);
							opers += "\nvoid* " + omn + "(void* instance,vals values,bool cleanvals)\n{\n\t"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\n";
							if(methpm.at(0)=="void")
							{
								opers += valsd;
								opers += "\n\tvoid* returnValue=NULL;\n\t*_obj<="+valsa+";";
								opers += valsades;
								opers += "\n\treturn returnValue;";
							}
							else
							{
								opers += "\n\t"+methpm.at(0)+" *_retVal=new "+methpm.at(0)+";\n";
								opers += valsd;
								opers += "\n\ttry{*_retVal = (*_obj<="+valsa+");} catch(...) {delete ("+methpm.at(0)+"*)_retVal;"+valsades+"throw;}\n";
								opers += valsades;
								opers += "\n\treturn _retVal;";
							}
							refDef += ("\nci.addOperatorRefName(\"<=\", \""+omn+"\");");
							opers += "\n}";
						}
						else if(meth.find(">=")!=std::string::npos)
						{
							std::string omn = app+ "_" + classStructure.getTreatedClassName(true) + "_o" +
									CastUtil::lexical_cast<std::string>(opcounter++);
							opers += "\nvoid* " +omn+"(void* instance,vals values,bool cleanvals)\n{\n\t"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\n";
							if(methpm.at(0)=="void")
							{
								opers += valsd;
								opers += "\n\tvoid* returnValue=NULL;\n\t*_obj>="+valsa+";";
								opers += valsades;
								opers += "\n\treturn returnValue;";
							}
							else
							{
								opers += "\n\t"+methpm.at(0)+" *_retVal=new "+methpm.at(0)+";\n";
								opers += valsd;
								opers += "\n\ttry{*_retVal = (*_obj>="+valsa+");} catch(...) {delete ("+methpm.at(0)+"*)_retVal;"+valsades+"throw;}\n";
								opers += valsades;
								opers += "\n\treturn _retVal;";
							}
							refDef += ("\nci.addOperatorRefName(\">=\", \""+omn+"\");");
							opers += "\n}";
						}
						else if(meth.find("<")!=std::string::npos)
						{
							std::string omn = app+ "_" + classStructure.getTreatedClassName(true) + "_o" +
									CastUtil::lexical_cast<std::string>(opcounter++);
							opers += "\nvoid* " +omn+"(void* instance,vals values,bool cleanvals)\n{\n\t"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\n";
							if(methpm.at(0)=="void")
							{
								opers += valsd;
								opers += "\n\tvoid* returnValue=NULL;\n\t*_obj<"+valsa+";";
								opers += valsades;
								opers += "\n\treturn returnValue;";
							}
							else
							{
								opers += "\n\t"+methpm.at(0)+" *_retVal=new "+methpm.at(0)+";\n";
								opers += valsd;
								opers += "\n\ttry{*_retVal = (*_obj<"+valsa+");} catch(...) {delete ("+methpm.at(0)+"*)_retVal;"+valsades+"throw;}\n";
								opers += valsades;
								opers += "\n\treturn _retVal;";
							}
							refDef += ("\nci.addOperatorRefName(\"<\", \""+omn+"\");");
							opers += "\n}";
						}
						else if(meth.find(">")!=std::string::npos)
						{
							std::string omn = app+ "_" + classStructure.getTreatedClassName(true) + "_o" +
									CastUtil::lexical_cast<std::string>(opcounter++);
							opers += "\nvoid* " +omn+"(void* instance,vals values,bool cleanvals)\n{\n\t"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\n";
							if(methpm.at(0)=="void")
							{
								opers += valsd;
								opers += "\n\tvoid* returnValue=NULL;\n\t*_obj>"+valsa+";";
								opers += valsades;
								opers += "\n\treturn returnValue;";
							}
							else
							{
								opers += "\n\t"+methpm.at(0)+" *_retVal=new "+methpm.at(0)+";\n";
								opers += valsd;
								opers += "\n\ttry{*_retVal = (*_obj>"+valsa+");} catch(...) {delete ("+methpm.at(0)+"*)_retVal;"+valsades+"throw;}\n";
								opers += valsades;
								opers += "\n\treturn _retVal;";
							}
							refDef += ("\nci.addOperatorRefName(\">\", \""+omn+"\");");
							opers += "\n}";
						}
						else if(meth.find("==")!=std::string::npos)
						{
							std::string omn = app+ "_" + classStructure.getTreatedClassName(true) + "_o" +
									CastUtil::lexical_cast<std::string>(opcounter++);
							opers += "\nvoid* " +omn+"(void* instance,vals values,bool cleanvals)\n{\n\t"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\n";
							if(methpm.at(0)=="void")
							{
								opers += valsd;
								opers += "\n\tvoid* returnValue=NULL;\n\t*_obj=="+valsa+";";
								opers += valsades;
								opers += "\n\treturn returnValue;";
							}
							else
							{
								opers += "\n\t"+methpm.at(0)+" *_retVal=new "+methpm.at(0)+";\n";
								opers += valsd;
								opers += "\n\ttry{*_retVal = (*_obj=="+valsa+");} catch(...) {delete ("+methpm.at(0)+"*)_retVal;"+valsades+"throw;}\n";
								opers += valsades;
								opers += "\n\treturn _retVal;";
							}
							refDef += ("\nci.addOperatorRefName(\"==\", \""+omn+"\");");
							opers += "\n}";
						}
						else if(meth.find("!=")!=std::string::npos)
						{
							std::string omn = app+ "_" + classStructure.getTreatedClassName(true) + "_o" +
									CastUtil::lexical_cast<std::string>(opcounter++);
							opers += "\nvoid* " +omn+"(void* instance,vals values,bool cleanvals)\n{\n\t"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\n";
							if(methpm.at(0)=="void")
							{
								opers += valsd;
								opers += "\n\tvoid* returnValue=NULL;\n\t*_obj!="+valsa+";";
								opers += valsades;
								opers += "\n\treturn returnValue;";
							}
							else
							{
								opers += "\n\t"+methpm.at(0)+" *_retVal=new "+methpm.at(0)+";\n";
								opers += valsd;
								opers += "\n\ttry{*_retVal = (*_obj!="+valsa+");} catch(...) {delete ("+methpm.at(0)+"*)_retVal;"+valsades+"throw;}\n";
								opers += valsades;
								opers += "\n\treturn _retVal;";
							}
							refDef += ("\nci.addOperatorRefName(\"!=\", \""+omn+"\");");
							opers += "\n}";
						}
						else if(meth.find("!")!=std::string::npos)
						{
							std::string omn = app+ "_" + classStructure.getTreatedClassName(true) + "_o" +
									CastUtil::lexical_cast<std::string>(opcounter++);
							opers += "\nvoid* " +omn+"(void* instance,vals values,bool cleanvals)\n{\n\t"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\n";
							if(methpm.at(0)=="void")
							{
								opers += valsd;
								opers += "\n\tvoid* returnValue=NULL;\n\t!*_obj;";
								opers += valsades;
								opers += "\n\treturn returnValue;";
							}
							else
							{
								opers += "\n\t"+methpm.at(0)+" *_retVal=new "+methpm.at(0)+";\n";
								opers += valsd;
								opers += "\n\ttry{*_retVal = (!*_obj);} catch(...) {delete ("+methpm.at(0)+"*)_retVal;"+valsades+"throw;}\n";
								opers += valsades;
								opers += "\n\treturn _retVal;";
							}
							refDef += ("\nci.addOperatorRefName(\"!\", \""+omn+"\");");
							opers += "\n}";
						}
						else if(meth.find("+")!=std::string::npos)
						{
							std::string omn = app+ "_" + classStructure.getTreatedClassName(true) + "_o" +
									CastUtil::lexical_cast<std::string>(opcounter++);
							opers += "\nvoid* " +omn+"(void* instance,vals values,bool cleanvals)\n{\n\t"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\n";
							if(methpm.at(0)=="void")
							{
								opers += valsd;
								opers += "\n\tvoid* returnValue=NULL;\n\t*_obj+"+valsa+";";
								opers += valsades;
								opers += "\n\treturn returnValue;";
							}
							else
							{
								opers += "\n\t"+methpm.at(0)+" *_retVal=new "+methpm.at(0)+";\n";
								opers += valsd;
								opers += "\n\ttry{*_retVal = (*_obj+"+valsa+");} catch(...) {delete ("+methpm.at(0)+"*)_retVal;"+valsades+"throw;}\n";
								opers += valsades;
								opers += "\n\treturn _retVal;";
							}
							refDef += ("\nci.addOperatorRefName(\"+\", \""+omn+"\");");
							opers += "\n}";
						}
						else if(meth.find("-")!=std::string::npos)
						{
							std::string omn = app+ "_" + classStructure.getTreatedClassName(true) + "_o" +
									CastUtil::lexical_cast<std::string>(opcounter++);
							opers += "\nvoid* " +omn+"(void* instance,vals values,bool cleanvals)\n{\n\t"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\n";
							if(methpm.at(0)=="void")
							{
								opers += valsd;
								opers += "\n\tvoid* returnValue=NULL;\n\t*_obj-"+valsa+";";
								opers += valsades;
								opers += "\n\treturn returnValue;";
							}
							else
							{
								opers += "\n\t"+methpm.at(0)+" *_retVal=new "+methpm.at(0)+";\n";
								opers += valsd;
								opers += "\n\ttry{*_retVal = (*_obj-"+valsa+");} catch(...) {delete ("+methpm.at(0)+"*)_retVal;"+valsades+"throw;}\n";
								opers += valsades;
								opers += "\n\treturn _retVal;";
							}
							refDef += ("\nci.addOperatorRefName(\"-\", \""+omn+"\");");
							opers += "\n}";
						}
						else if(meth.find("/")!=std::string::npos)
						{
							std::string omn = app+ "_" + classStructure.getTreatedClassName(true) + "_o" +
									CastUtil::lexical_cast<std::string>(opcounter++);
							opers += "\nvoid* " +omn+"(void* instance,vals values,bool cleanvals)\n{\n\t"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\n";
							if(methpm.at(0)=="void")
							{
								opers += valsd;
								opers += "\n\tvoid* returnValue=NULL;\n\t*_obj/"+valsa+";";
								opers += valsades;
								opers += "\n\treturn returnValue;";
							}
							else
							{
								opers += "\n\t"+methpm.at(0)+" *_retVal=new "+methpm.at(0)+";\n";
								opers += valsd;
								opers += "\n\ttry{*_retVal = (*_obj/"+valsa+");} catch(...) {delete ("+methpm.at(0)+"*)_retVal;"+valsades+"throw;}\n";
								opers += valsades;
								opers += "\n\treturn _retVal;";
							}
							refDef += ("\nci.addOperatorRefName(\"-\", \""+omn+"\");");
							opers += "\n}";
						}
						else if(meth.find("*")!=std::string::npos)
						{
							std::string omn = app+ "_" + classStructure.getTreatedClassName(true) + "_o" +
									CastUtil::lexical_cast<std::string>(opcounter++);
							opers += "\nvoid* " +omn+"(void* instance,vals values,bool cleanvals)\n{\n\t"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\n";
							if(methpm.at(0)=="void")
							{
								opers += valsd;
								opers += "\n\tvoid* returnValue=NULL;\n\t*_obj*"+valsa+";";
								opers += valsades;
								opers += "\n\treturn returnValue;";
							}
							else
							{
								opers += "\n\t"+methpm.at(0)+" *_retVal=new "+methpm.at(0)+";\n";
								opers += valsd;
								opers += "\n\ttry{*_retVal = (*_obj*"+valsa+");} catch(...) {delete ("+methpm.at(0)+"*)_retVal;"+valsades+"throw;}\n";
								opers += valsades;
								opers += "\n\treturn _retVal;";
							}
							refDef += ("\nci.addOperatorRefName(\"*\", \""+omn+"\");");
							opers += "\n}";
						}
						else if(meth.find("&&")!=std::string::npos)
						{

						}
						else if(meth.find("&")!=std::string::npos)
						{

						}
						else if(meth.find("||")!=std::string::npos)
						{

						}
						else if(meth.find("|")!=std::string::npos)
						{

						}
						else if(meth.find("[")!=std::string::npos && meth.find("]")!=std::string::npos)
						{

						}
						else if(meth.find("(")!=std::string::npos && meth.find(")")!=std::string::npos)
						{

						}
					}
					else
					{
						StringUtil::replaceAll(methsd,"std::","");
						StringUtil::replaceAll(methsd, " ", "");
						StringUtil::replaceAll(methsd, "*", "ptr");
						StringUtil::replaceAll(methsd, "&", "");
						/*if(methsd.find("std::")!=std::string::npos)
						{
							StringUtil::replaceFirst(methsd,"std::","");
						}
						else if(methsd.find("::")!=std::string::npos)
						{
							StringUtil::replaceFirst(methsd,"::","ns");
						}*/
						//StringUtil::replaceFirst(methsd,"*","ptr");
						//StringUtil::replaceFirst(methsd,"&","adr");
						if(methpm.at(0)!=classStructure.getTreatedClassName(false))
						{
							ms.name =methpm.at(1);
							typedefs += (") ("+typdefName+");\n");
							std::string mmn = app+ "_" + classStructure.getTreatedClassName(true) + "_m" +
										CastUtil::lexical_cast<std::string>(methcounter++);
							methods += "\nvoid* " +mmn+"(void* instance,vals values,bool cleanvals)\n{\n\t";
							if(!methstat)
							{
								methods += classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\n";
							}
							if(methpm.at(0)=="void")
							{
								methods += valsd;
								if(methstat)
								{
									methods += "\n\tvoid* returnValue=NULL;\n\t_"+classStructure.getFullyQualifiedClassName()+"::"+methpm.at(1)+"("+valsa+");\n";
								}
								else
								{
									methods += "\n\tvoid* returnValue=NULL;\n\t_obj->"+methpm.at(1)+"("+valsa+");\n";
								}
								methods += valsades;
								methods += "\n\treturn returnValue;";
							}
							else
							{
								std::string fqcn = getFullyQualifiedClassName(methpm.at(0), classStructure.namespaces);
								methods += "\n\t"+fqcn+" *_retVal=new "+fqcn+";\n";
								methods += valsd;
								if(methstat)
								{
									methods += "\n\ttry{*_retVal = ("+classStructure.getFullyQualifiedClassName()+"::"+methpm.at(1)+"("+valsa+"));} catch(...) {delete ("+fqcn+"*)_retVal;"+valsades+"throw;}\n";
								}
								else
								{
									methods += "\n\ttry{*_retVal = (_obj->"+methpm.at(1)+"("+valsa+"));} catch(...) {delete ("+fqcn+"*)_retVal;"+valsades+"throw;}\n";
								}
								methods += valsades;
								methods += "\n\treturn _retVal;";
							}
							methods += "\n}";
							refDef += ("me.setMethodName(\""+methsd+"\");\n");
							refDef += ("me.setRefName(\""+mmn+"\");\n");
							methsall[methsd] = true;
							refDef += ("me.setArgumentTypes(argu);\n");
							if(methstat)
							{
								refDef += ("me.setIsStatic(true);\n");
							}
							refDef += ("argu.clear();\n");
							refDef += ("if(me.getMethodName()!=\"\")\n{\nci.addMethod(me);\n}\n");

							testStr += "me = ci.getMethod(\""+methsd+"\", argu);\n";
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
							std::string mmn = app+ "_" + classStructure.getTreatedClassName(true) + "_m" +
									CastUtil::lexical_cast<std::string>(methcounter++);
							typedefs += (") ("+typdefName+");\n");
							methods += "\nvoid* " +mmn+"(vals values,bool cleanvals)\n{";
							methods += "\n\t"+classStructure.getFullyQualifiedClassName()+" *_retVal = NULL;\n";
							methods += valsd;
							methods += "\n\t_retVal = (new "+classStructure.getFullyQualifiedClassName()+"("+valsa+"));";
							methods += valsades;
							methods += "\n\treturn _retVal;";
							methods += "\n}";
							refDef += ("ctor.setName(\""+methsd+"\");\n");
							refDef += ("ctor.setRefName(\""+mmn+"\");\n");
							refDef += ("ctor.setArgumentTypes(argu);\n");
							refDef += ("argu.clear();\n");
							refDef += ("ci.addConstructor(ctor);\n");

							testStr += "ctor = ci.getConstructor(argu);\n";
							testStr += "assert(ctor.getName()==\"" + methsd +"\");\n";
							testStr += ("argu.clear();\n");

							if(numargs==0)
							{
								ctorisp = true;
							}
						}
					}
				}
				classStructure.pubms.at(i) = ms;
			}
			std::vector<PropStructure> extpubps;
			for (unsigned int i = 0; i < classStructure.pubps.size(); i++) {
				PropStructure ms = classStructure.pubps.at(i);
				std::string pubdecl = ms.decl;

				publf += pubdecl;

				fld = pubdecl;

				StringUtil::replaceAll(fld, "{", "");
				StringUtil::replaceAll(fld, "}", "");
				StringUtil::trim(fld);
				if(fld.length()==0)continue;

				//bool fldstatic = false;
				if(RegexUtil::find(fld, "[ \t]*static[ \t]+")!=-1 || RegexUtil::find(fld, "[ \t]+static[ \t]+")!=-1)
				{
					RegexUtil::replace(fld, "[ \t]*static[ \t]+", " ");
					RegexUtil::replace(fld, "[ \t]+static[ \t]+", " ");
					//fldstatic = true;
				}
				RegexUtil::replace(fld, "[ \t]*const[ \t]+", " ");
				RegexUtil::replace(fld, "[ \t]+const[ \t]+", " ");

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
				if(fld.find(",")!=std::string::npos) {
					std::string fldsbc = fld.substr(0, fld.find(","));
					std::string fldss = fld.substr(fld.find(",")+1);
					fldp = StringUtil::splitAndReturn<std::vector<std::string> >(fldss, ",");
					fldp.insert(fldp.begin(), fldsbc.substr(0, fldsbc.find_last_of(" ")));
					fldp.insert(fldp.begin(), fldsbc.substr(fldsbc.find_last_of(" ")+1));
				} else {
					fldp.push_back(fld.substr(0, fld.find_last_of(" ")));
					fldp.push_back(fld.substr(fld.find_last_of(" ")+1));
				}
				for(unsigned int j = 1; j < fldp.size(); j++)
				{
					StringUtil::trim(fldp.at(j));
				}

				if(fldp.size()>1)
				{
					std::string fqcn = getFullyQualifiedClassName(fldp.at(0), classStructure.namespaces);
					if(ptr)
					{
						fqcn += "*";
					}

					for(unsigned int j = 1; j < fldp.size(); j++)
					{
						PropStructure tms;
						tms.type = ms.type;
						tms.name = ms.name;
						tms.decl = ms.decl;
						tms.markers = ms.markers;

						refDef += ("f.clear();\n");
						testStr += ("f.clear();\n");

						refDef += ("f.setType(\""+fldp.at(0)+"\");\n");
						testStr += "assert(f.getType()==\""+fldp.at(0)+"\");\n";
						tms.type = fldp.at(0);

						refDef += ("f.setFieldName(\""+fldp.at(j)+"\");\n");
						testStr += "assert(f.getFieldName()==\""+fldp.at(j)+"\");\n";
						tms.name = fldp.at(j);

						std::string fmn = app+ "_" + classStructure.getTreatedClassName(true) + "_f" +
								CastUtil::lexical_cast<std::string>(fldcounter++);
						methods += "\n"+fqcn+" " +fmn+"(void* instance)\n{\n\t"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\n\treturn _obj->"+fldp.at(j)+";\n}\n";
						refDef += ("if(f.getFieldName()!=\"\")\n{f.setRefName(\""+fmn+"\");\n\nci.addField(f);\n}\n");

						if(j>1) {
							extpubps.push_back(tms);
						} else {
							classStructure.pubps.at(i) = tms;
						}
					}
				}
			}
			for(unsigned int j = 0; j < extpubps.size(); j++)
			{
				classStructure.pubps.push_back(extpubps.at(j));
			}
		}
		if (classStructure.prips.size() > 0 || classStructure.prims.size() > 0)
		{
			for (unsigned int i = 0; i < classStructure.prims.size(); i++) {
				MethStructure ms = classStructure.prims.at(i);
				std::string pubdecl = ms.decl;

				//refDef += ("me.clear();\n");
				privm += pubdecl;
				meth = pubdecl;
				StringUtil::replaceFirst(meth,";","");

				std::string argts = meth.substr(meth.find("("),meth.find(")")-meth.find("("));
				StringUtil::replaceFirst(argts,"(","");
				StringUtil::replaceAll(argts,")","");
				meth = meth.substr(0,meth.find("("));
				StringUtil::trim(meth);
				RegexUtil::replace(meth, "[ \t]*,[ \t]*", ",");

				//bool methstat = false;
				if(RegexUtil::find(meth, "[ \t]*static[ \t]+")!=-1 || RegexUtil::find(meth, "[ \t]+static[ \t]+")!=-1)
				{
					RegexUtil::replace(meth, "[ \t]*static[ \t]+", " ");
					RegexUtil::replace(meth, "[ \t]+static[ \t]+", " ");
					//methstat = true;
				}
				RegexUtil::replace(meth, "[ \t]*inline[ \t]+", " ");
				RegexUtil::replace(meth, "[ \t]+inline[ \t]+", " ");
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
						argpm.push_back(argpmtemp+typ);
					}
				}
				std::string typdefName,methsd,valsd,valsa;
				//bool ctor = false;
				if(methpm.size()>0 && methpm.at(0).find("virtual")!=std::string::npos)
					return "";

				bool tmpltarg = false;
				for(unsigned int j = 0; j < argpm.size(); j++)
				{
					if(tmpltarg && argpm.at(j).find(">")!=std::string::npos)
					{
						std::string ttt = argpmtemp.at(argpmtemp.size()-1)+","+argpm.at(j);
						argpmtemp.at(argpmtemp.size()-1) = ttt;
					}
					else if(argpm.at(j).find("<")!=std::string::npos)
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
				if(pubdecl.find("~")==std::string::npos && methpm.at(0)==classStructure.getTreatedClassName(false))
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
				classStructure.prims.at(i) = ms;
			}
			std::vector<PropStructure> extprips;
			for (unsigned int i = 0; i < classStructure.prips.size(); i++) {
				PropStructure ms = classStructure.prips.at(i);
				std::string pubdecl = ms.decl;

				privf += pubdecl;

				fld = pubdecl;

				//bool fldstatic = false;
				if(RegexUtil::find(fld, "[ \t]*static[ \t]+")!=-1 || RegexUtil::find(fld, "[ \t]+static[ \t]+")!=-1)
				{
					RegexUtil::replace(fld, "[ \t]*static[ \t]+", " ");
					RegexUtil::replace(fld, "[ \t]+static[ \t]+", " ");
					//fldstatic = true;
				}
				RegexUtil::replace(fld, "[ \t]*const[ \t]+", " ");
				RegexUtil::replace(fld, "[ \t]+const[ \t]+", " ");

				RegexUtil::replace(fld, "[\t]+", " ");
				RegexUtil::replace(fld, "[ ]+", " ");
				StringUtil::replaceFirst(fld,";","");
				StringUtil::trim(fld);

				//bool ptr = false;
				if(fld.find("*")!=std::string::npos)
				{
					//ptr = true;
					StringUtil::replaceFirst(fld,"*","");
				}

				std::vector<std::string> fldp;
				if(fld.find(",")!=std::string::npos) {
					std::string fldsbc = fld.substr(0, fld.find(","));
					std::string fldss = fld.substr(fld.find(",")+1);
					fldp = StringUtil::splitAndReturn<std::vector<std::string> >(fldss, ",");
					fldp.insert(fldp.begin(), fldsbc.substr(0, fldsbc.find_last_of(" ")));
					fldp.insert(fldp.begin(), fldsbc.substr(fldsbc.find_last_of(" ")+1));
				} else {
					fldp.push_back(fld.substr(0, fld.find_last_of(" ")));
					fldp.push_back(fld.substr(fld.find_last_of(" ")+1));
				}
				for(unsigned int j = 1; j < fldp.size(); j++)
				{
					StringUtil::trim(fldp.at(j));
				}

				if(fldp.size()>1)
				{
					for(unsigned int j = 1; j < fldp.size(); j++)
					{
						PropStructure tms;
						tms.type = ms.type;
						tms.name = ms.name;
						tms.decl = ms.decl;
						tms.markers = ms.markers;

						refDef += ("f.clear();\n");
						testStr += ("f.clear();\n");

						refDef += ("f.setType(\""+fldp.at(0)+"\");\n");
						testStr += "assert(f.getType()==\""+fldp.at(0)+"\");\n";
						tms.type = fldp.at(0);

						refDef += ("f.setFieldName(\""+fldp.at(j)+"\");\n");
						testStr += "assert(f.getFieldName()==\""+fldp.at(j)+"\");\n";
						tms.name = fldp.at(j);
						refDef += ("if(f.getFieldName()!=\"\")\n{\nci.addField(f);\n}\n");

						if(j>1) {
							extprips.push_back(tms);
						} else {
							classStructure.prips.at(i) = tms;
						}
					}

				}
			}
			for(unsigned int j = 0; j < extprips.size(); j++)
			{
				classStructure.prips.push_back(extprips.at(j));
			}
		}
		if (classStructure.props.size() > 0 || classStructure.proms.size() > 0)
		{
			for (unsigned int i = 0; i < classStructure.proms.size(); i++) {
				MethStructure ms = classStructure.proms.at(i);
				std::string pubdecl = ms.decl;

				//refDef += ("me.clear();\n");
				protm += pubdecl;
				meth = pubdecl;
				StringUtil::replaceFirst(meth,";","");

				std::string argts = meth.substr(meth.find("("),meth.find(")")-meth.find("("));
				StringUtil::replaceFirst(argts,"(","");
				StringUtil::replaceAll(argts,")","");
				meth = meth.substr(0,meth.find("("));
				StringUtil::trim(meth);
				RegexUtil::replace(meth, "[ \t]*,[ \t]*", ",");

				//bool methstat = false;
				if(RegexUtil::find(meth, "[ \t]*static[ \t]+")!=-1 || RegexUtil::find(meth, "[ \t]+static[ \t]+")!=-1)
				{
					RegexUtil::replace(meth, "[ \t]*static[ \t]+", " ");
					RegexUtil::replace(meth, "[ \t]+static[ \t]+", " ");
					//methstat = true;
				}
				RegexUtil::replace(meth, "[ \t]*inline[ \t]+", " ");
				RegexUtil::replace(meth, "[ \t]+inline[ \t]+", " ");
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
						argpm.push_back(argpmtemp+typ);
					}
				}
				std::string typdefName,methsd,valsd,valsa;
				//bool ctor = false;
				if(methpm.size()>0 && methpm.at(0).find("virtual")!=std::string::npos)
					return "";

				bool tmpltarg = false;
				for(unsigned int j = 0; j < argpm.size(); j++)
				{
					if(tmpltarg && argpm.at(j).find(">")!=std::string::npos)
					{
						std::string ttt = argpmtemp.at(argpmtemp.size()-1)+","+argpm.at(j);
						argpmtemp.at(argpmtemp.size()-1) = ttt;
					}
					else if(argpm.at(j).find("<")!=std::string::npos)
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
				if(pubdecl.find("~")==std::string::npos && methpm.at(0)==classStructure.getTreatedClassName(false))
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
				classStructure.proms.at(i) = ms;
			}
			std::vector<PropStructure> extprops;
			for (unsigned int i = 0; i < classStructure.props.size(); i++) {
				PropStructure ms = classStructure.props.at(i);
				std::string pubdecl = ms.decl;

				protf += pubdecl;

				fld = pubdecl;

				//bool fldstatic = false;
				if(RegexUtil::find(fld, "[ \t]*static[ \t]+")!=-1 || RegexUtil::find(fld, "[ \t]+static[ \t]+")!=-1)
				{
					RegexUtil::replace(fld, "[ \t]*static[ \t]+", " ");
					RegexUtil::replace(fld, "[ \t]+static[ \t]+", " ");
					//fldstatic = true;
				}
				RegexUtil::replace(fld, "[ \t]*const[ \t]+", " ");
				RegexUtil::replace(fld, "[ \t]+const[ \t]+", " ");

				RegexUtil::replace(fld, "[\t]+", " ");
				RegexUtil::replace(fld, "[ ]+", " ");
				StringUtil::replaceFirst(fld,";","");
				StringUtil::trim(fld);

				//bool ptr = false;
				if(fld.find("*")!=std::string::npos)
				{
					//ptr = true;
					StringUtil::replaceFirst(fld,"*","");
				}

				std::vector<std::string> fldp;
				if(fld.find(",")!=std::string::npos) {
					std::string fldsbc = fld.substr(0, fld.find(","));
					std::string fldss = fld.substr(fld.find(",")+1);
					fldp = StringUtil::splitAndReturn<std::vector<std::string> >(fldss, ",");
					fldp.insert(fldp.begin(), fldsbc.substr(0, fldsbc.find_last_of(" ")));
					fldp.insert(fldp.begin(), fldsbc.substr(fldsbc.find_last_of(" ")+1));
				} else {
					fldp.push_back(fld.substr(0, fld.find_last_of(" ")));
					fldp.push_back(fld.substr(fld.find_last_of(" ")+1));
				}
				for(unsigned int j = 1; j < fldp.size(); j++)
				{
					StringUtil::trim(fldp.at(j));
				}

				if(fldp.size()>1)
				{
					for(unsigned int j = 1; j < fldp.size(); j++)
					{
						PropStructure tms;
						tms.type = ms.type;
						tms.name = ms.name;
						tms.decl = ms.decl;
						tms.markers = ms.markers;

						refDef += ("f.clear();\n");
						testStr += ("f.clear();\n");

						refDef += ("f.setType(\""+fldp.at(0)+"\");\n");
						testStr += "assert(f.getType()==\""+fldp.at(0)+"\");\n";
						tms.type = fldp.at(0);

						refDef += ("f.setFieldName(\""+fldp.at(j)+"\");\n");
						testStr += "assert(f.getFieldName()==\""+fldp.at(j)+"\");\n";
						tms.name = fldp.at(j);
						refDef += ("if(f.getFieldName()!=\"\")\n{\nci.addField(f);\n}\n");

						if(j>1) {
							extprops.push_back(tms);
						} else {
							classStructure.props.at(i) = tms;
						}
					}

				}
			}
			for(unsigned int j = 0; j < extprops.size(); j++)
			{
				classStructure.props.push_back(extprops.at(j));
			}
		}
		if(!ctorisp)
		{
			std::string mmn = app+ "_" + classStructure.getTreatedClassName(true) + "_m" +
					CastUtil::lexical_cast<std::string>(methcounter++);
			refDef += ("ctor.setName(\""+classStructure.getTreatedClassName(true)+"\");\n");
			refDef += ("argu.clear();\n");
			methods += "\nvoid* " +mmn+"(vals values,bool cleanvals)\n{";
			methods += "\n\t"+classStructure.getFullyQualifiedClassName()+" *_retVal = NULL;\n";
			methods += "\n\t_retVal = (new "+classStructure.getFullyQualifiedClassName()+"());";
			methods += "\n\treturn _retVal;";
			methods += "\n}";
			refDef += ("ctor.setRefName(\""+mmn+"\");\n");
			refDef += ("ctor.setArgumentTypes(argu);\n");
			refDef += ("argu.clear();\n");
			refDef += ("ci.addConstructor(ctor);\n");

			testStr += ("argu.clear();\n");
			testStr += "ctor = ci.getConstructor(argu);\n";
			testStr += "assert(ctor.getName()==\"" + classStructure.getTreatedClassName(true) +"\");\n";
			testStr += ("argu.clear();\n");
		}
		if(!ddtorisp)
		{
			std::string mmn = app+ "_" + classStructure.getTreatedClassName(true) + "_m" +
					CastUtil::lexical_cast<std::string>(methcounter++);
			methods += "\nvoid " +mmn+"(void* instance)\n{";
			methods += "\n\t"+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)instance;\n\t";
			methods += "_obj->~"+classStructure.getTreatedClassName(false)+"();";
			methods += "\n}";
			ddtorisp = true;
			refDef += ("ci.setDestRefName(\""+mmn+"\");\n");
		}
		//refDef += ("\nci.setMeths(meths);");
		//refDef += ("\nci.setMethods(methVec);");
		/*refDef += ("\nci.addContRefName(0, \""+app+ "_"+classStructure.getTreatedClassName(true)+"c1\");");
		refDef += ("\nci.addContRefName(2, \""+app+ "_"+classStructure.getTreatedClassName(true)+"c2\");");
		refDef += ("\nci.addContRefName(4, \""+app+ "_"+classStructure.getTreatedClassName(true)+"c3\");");
		refDef += ("\nci.addContRefName(6, \""+app+ "_"+classStructure.getTreatedClassName(true)+"c4\");");
		refDef += ("\nci.addContRefName(8, \""+app+ "_"+classStructure.getTreatedClassName(true)+"c5\");");*/

		refDef += ("\nci.setContRefName(\""+app+ "_"+classStructure.getTreatedClassName(true)+"co\");");

		methods += "\nvoid* " +app+ "_"+classStructure.getTreatedClassName(true)+"co(void* _vec,void* _instance,int pos,std::string contType,int t){"
				+ "\nif(t==-1)return Reflector::destroyNestedContainer<"+classStructure.getFullyQualifiedClassName()+">(contType, _vec);\n"
				+ "\nelse if(t==1)return Reflector::getNewNestedContainer<"+classStructure.getFullyQualifiedClassName()+">(contType);\n"
				+ "\nelse if(t==2){int* _obj = new int;\n*_obj = Reflector::getNestedContainerSize<"+classStructure.getFullyQualifiedClassName()+">(contType,_vec);return _obj;}\n"
				+ "\nelse if(t==3){Reflector::addValueToNestedContainer<"+classStructure.getFullyQualifiedClassName()+">(contType, *(("+classStructure.getFullyQualifiedClassName()+"*)_instance),_vec);return NULL;}\n"
				+ "\nelse if(t==4){"+classStructure.getFullyQualifiedClassName()+" *_obj = new "+classStructure.getFullyQualifiedClassName()+";\n*_obj = Reflector::getValueFromNestedContainer<"+classStructure.getFullyQualifiedClassName()+">(contType, _vec, pos);\nreturn _obj;\n}\n"
				+ "\nelse if(t==5)return Reflector::getPValueFromNestedContainer<"+classStructure.getFullyQualifiedClassName()+">(contType, _vec, pos);\n"
				+ "return NULL;\n}";

		/*methods += "\nvoid* " +app+ "_"+classStructure.getTreatedClassName(true)+"c1(std::string contType){\nreturn "
				+ "Reflector::getNewNestedContainer<"+classStructure.getFullyQualifiedClassName()+">(contType);\n}";
		methods += "\nint " +app+ "_"+classStructure.getTreatedClassName(true)+"c2(void* _vec,std::string contType){\n"
				+ "return Reflector::getNestedContainerSize<"+classStructure.getFullyQualifiedClassName()+">(contType, _vec);\n}";
		methods += "\nvoid " +app+ "_"+classStructure.getTreatedClassName(true)+"c3(void* _vec,void* _instance,std::string contType){\n"
				+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)_instance;\n"
				+ "Reflector::addValueToNestedContainer<"+classStructure.getFullyQualifiedClassName()+">(contType, *_obj, _vec);\n}";
		methods += "\nvoid* " +app+ "_"+classStructure.getTreatedClassName(true)+"c4(void* _vec,int pos,std::string contType){\n"
				+classStructure.getFullyQualifiedClassName()+" *_obj = new "+classStructure.getFullyQualifiedClassName()+";\n"
				+ "*_obj = Reflector::getValueFromNestedContainer<"+classStructure.getFullyQualifiedClassName()+">(contType, _vec, pos);\nreturn _obj;\n}\n";
		methods += "\nvoid* " +app+ "_"+classStructure.getTreatedClassName(true)+"c5(void* _vec,int pos,std::string contType){"
			+ "return Reflector::getPValueFromNestedContainer<"+classStructure.getFullyQualifiedClassName()+">(contType, _vec, pos);\n}\n";
		*/

		if(classStructure.prosetser)
		{
			/*refDef += ("\nci.addContRefName(1, \""+app+ "_"+classStructure.getTreatedClassName(true)+"c6\");");
			refDef += ("\nci.addContRefName(3, \""+app+ "_"+classStructure.getTreatedClassName(true)+"c7\");");
			refDef += ("\nci.addContRefName(5, \""+app+ "_"+classStructure.getTreatedClassName(true)+"c8\");");
			refDef += ("\nci.addContRefName(7, \""+app+ "_"+classStructure.getTreatedClassName(true)+"c9\");");
			refDef += ("\nci.addContRefName(9, \""+app+ "_"+classStructure.getTreatedClassName(true)+"c10\");");

			methods += "\nvoid* " +app+ "_"+classStructure.getTreatedClassName(true)+"c6(std::string contType){\nreturn "
					+ "Reflector::getNewNestedContainerSV<"+classStructure.getFullyQualifiedClassName()+">(contType);\n}";
			methods += "\nvoid " +app+ "_"+classStructure.getTreatedClassName(true)+"c7(void* _vec,void* _instance,std::string contType){\n"
					+classStructure.getFullyQualifiedClassName()+" *_obj = ("+classStructure.getFullyQualifiedClassName()+"*)_instance;\n"
					+ "Reflector::addValueToNestedContainerSV<"+classStructure.getFullyQualifiedClassName()+">(contType, *_obj, _vec);\n}";
			methods += "\nint " +app+ "_"+classStructure.getTreatedClassName(true)+"c8(void* _vec,std::string contType){\n"
					+ "return Reflector::getNestedContainerSizeSV<"+classStructure.getFullyQualifiedClassName()+">(contType, _vec);\n}";
			methods += "\nvoid* " +app+ "_"+classStructure.getTreatedClassName(true)+"c9(void* _vec,int pos,std::string contType){\n"
					+classStructure.getFullyQualifiedClassName()+" *_obj = new "+classStructure.getFullyQualifiedClassName()+";\n"
					+ "*_obj = Reflector::getValueFromNestedContainerSV<"+classStructure.getFullyQualifiedClassName()+">(contType, _vec, pos);\nreturn _obj;\n}\n";
			methods += "\nvoid* " +app+ "_"+classStructure.getTreatedClassName(true)+"c10(void* _vec,int pos,std::string contType){\n"
					+ "return Reflector::getPValueFromNestedContainerSV<"+classStructure.getFullyQualifiedClassName()+">(contType, _vec, pos);\n}\n";
			*/
			methods += "\nvoid* " +app+ "_"+classStructure.getTreatedClassName(true)+"cosv(void* _vec,void* _instance,int pos,std::string contType,int t){"
					+ "\nif(t==0)return Reflector::destroyNestedContainerSV<"+classStructure.getFullyQualifiedClassName()+">(contType, _vec);\n"
					+ "\nelse if(t==6)return Reflector::getNewNestedContainerSV<"+classStructure.getFullyQualifiedClassName()+">(contType);\n"
					+ "\nif(t==7){int* _obj = new int;\n*_obj = Reflector::getNestedContainerSizeSV<"+classStructure.getFullyQualifiedClassName()+">(contType,_vec);return _obj;}\n"
					+ "\nif(t==8){Reflector::addValueToNestedContainerSV<"+classStructure.getFullyQualifiedClassName()+">(contType, *(("+classStructure.getFullyQualifiedClassName()+"*)_instance),_vec);return NULL;}\n"
					+ "\nif(t==9){"+classStructure.getFullyQualifiedClassName()+" *_obj = new "+classStructure.getFullyQualifiedClassName()+";\n*_obj = Reflector::getValueFromNestedContainerSV<"+classStructure.getFullyQualifiedClassName()+">(contType, _vec, pos);\nreturn _obj;\n}\n"
					+ "\nif(t==10)return Reflector::getPValueFromNestedContainerSV<"+classStructure.getFullyQualifiedClassName()+">(contType, _vec, pos);\n"
					+ "}";
		}
		refDef += "\nreturn ci;\n}\n";
		allclsmap[it->first] = classStructure;
	}
	return refDef;
}

std::string Reflection::generateClassDefinition_Old(std::map<std::string, ClassStructure>& allclsmap, std::string &includesDefs, std::string &typedefs, std::string &classes, std::string &methods, std::string &opers, const std::string& app)
{
	return "";
}

std::string Reflection::generateSerDefinitionAll(std::map<std::string, std::map<std::string, ClassStructure> >& clsstrucMaps, std::string &includeRef, const bool& isBinary, std::string& objs, std::string& ajaxret, std::string& headers, std::string& typerefs, const std::vector<std::string>& apps)
{
	std::string ret = "";
	includeRef = "\n#include \"AppDefines.h\"\n#include \"vector\"\n#include \"list\"\n#include \"queue\"\n#include \"deque\"\n#include \"set\"\n#include \"DateFormat.h\"\n" ;
	includeRef += "#include \"SerializeBase.h\"\n#include \"sstream\"\n#include \"CastUtil.h\"\n#include <algorithm>\n";
	std::string typedefs,classes,methods,rert1;
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
	ttem += "\nstd::string fromDateToJSON(Date d){DateFormat formt(\"yyyy-mm-dd hh:mi:ss\");\nreturn formt.format(d);}";
	ttem += "\nstd::string fromDateVPToJSON(Date *d){DateFormat formt(\"yyyy-mm-dd hh:mi:ss\");\nreturn formt.format(*d);}";
	ttem += "BinaryData readBinaryData(JSONElement& obj){BinaryData bd;return bd;}";
	ttem += "BinaryData* readBinaryDataP(JSONElement& obj){return new BinaryData;}";
	ttem += "\nstd::string fromBinaryDataToJSON(BinaryData d){return \"\";}";
	ttem += "\nstd::string fromBinaryDataVPToJSON(BinaryData *d){return \"\";}";*/
	ajaxret = /*ttem + */typerefs + ajaxret;
	ajaxret ="#include \"AfcInclude.h\"\n#include \"JSONSerialize.h\"\n\nextern \"C\"\n{\n" + ajaxret;

	return ret;
}

std::string Reflection::generateSerDefinitions(std::map<std::string, ClassStructure>& allclsmap, std::string &includesDefs, std::string &typedefs, std::string &classes, std::string &methods, const bool& isBinary, std::string& objs, std::string &ajaxret, std::string& headers, std::string& typerefs, const std::string& app)
{
	//vector<std::string> includes;
	//CommonUtils::listFiles(includes, includeDir, ".h");
	std::string ret;
	/*map<std::string, ClassStructure> allclsmap;
	for (unsigned int var = 0; var < includes.size(); ++var)
	{
		std::map<std::string, ClassStructure> clsmap = getClassStructures(includes.at(var));
		allclsmap.insert(clsmap.begin(), clsmap.end());
	}*/
	ret = generateAllSerDefinition(allclsmap,includesDefs,typedefs,classes,methods,app);

	std::map<std::string, ClassStructure>::iterator it;
	for (it=allclsmap.begin();it!=allclsmap.end();++it)
	{
		std::vector<std::string> pinfo;
		bool isOpForSet = false;
		std::vector<std::string> minfo = getAfcObjectData(it->second,false,pinfo,isOpForSet);
		pinfo.clear();
		std::vector<std::string> info = getAfcObjectData(it->second,true,pinfo,isOpForSet);
	}

	/*for (unsigned int var = 0; var < includes.size(); ++var)
	{
		////logger << "\ngenerating Ser for file" << includes.at(var) << "\n" << std::flush;
		if(invalidcls.find(includes.at(var))==invalidcls.end())
		{

			std::vector<std::string> pinfo;
			bool isOpForSet = false;
			std::vector<std::string> minfo = getAfcObjectData(includes.at(var),false,pinfo,isOpForSet);
			pinfo.clear();
			std::vector<std::string> info = getAfcObjectData(includes.at(var),true,pinfo,isOpForSet);
		}
		////logger << "\ndone generating Ser for file" << includes.at(var) << "\n" << std::flush;
	}*/
	return ret;
}

std::string Reflection::generateAllSerDefinition(std::map<std::string, ClassStructure>& allclsmap, std::string &includesDefs, std::string &typedefs, std::string &classes, std::string &methods, const std::string& app)
{
	std::string ttapp = StringUtil::trimCopy(app);
	StringUtil::replaceAll(ttapp, "-", "_");
	RegexUtil::replace(ttapp, "[^a-zA-Z0-9_]+", "");
	std::string refDef;
	std::map<std::string, ClassStructure>::iterator it;
	for (it=allclsmap.begin();it!=allclsmap.end();++it)
	{
		ClassStructure classStructure = it->second;
		includesDefs += "#include \"" + getClassPath(it->second.getTreatedClassName(true)) + "\"\n";
		classStructure.prosetser = false;
		classes += "\nstd::string " +ttapp+ "serialize" + classStructure.getTreatedClassName(true)
				+ "(void* obje, SerializeBase* base);\nvoid* " +ttapp+ "unSerialize" + classStructure.getTreatedClassName(true) + "(void* intermediateObject, SerializeBase* base);";
		methods += "\nstd::string " +ttapp+ "serialize" + classStructure.getTreatedClassName(true)
				+ "(void* obje, SerializeBase* base)\n{\n"+classStructure.getFullyQualifiedClassName()+" *__obj=("+classStructure.getFullyQualifiedClassName()+"*)obje"
				+";\nvoid* serobject = base->getSerializableObject();\nbase->startObjectSerialization(serobject, \""
				+classStructure.getTreatedClassName(true)+"\");\n";
		typedefs += "\nvoid* " +ttapp+ "unSerialize" + classStructure.getTreatedClassName(true) + "(void* intermediateObject, SerializeBase* base)\n{\n";
		typedefs += "if(!base->isValidClassNamespace(intermediateObject, \""
				 + classStructure.getTreatedClassName(false)+"\", \""+classStructure.nmSpc+"\"))\n\treturn NULL;\n"
				 + classStructure.getFullyQualifiedClassName()+" *__obj=new " + classStructure.getFullyQualifiedClassName() +";\n";
		typedefs += "int totsize = base->getContainerSize(intermediateObject);\nfor(int i=0;i<totsize;i++)\n{\n";

		std::string publf, privf, protf ,publm, privm, protm;
		std::string meth,fld;
		size_t tes;
		std::vector<std::string> fldnames;
		std::map<std::string, bool> fldptr;
		std::map<std::string, int> fldstat;
		if (classStructure.pri.size() > 0)
		{
			for (unsigned int i = 0; i < classStructure.pri.size(); i++)
			{
				if(((tes=classStructure.pri.at(i).find("("))==std::string::npos &&
						(tes=classStructure.pri.at(i).find(")"))==std::string::npos && classStructure.pri.at(i).find("~")==std::string::npos))
				{
					fld = classStructure.pri.at(i);

					//bool fldstatic = false;
					if(RegexUtil::find(fld, "[ \t]*static[ \t]+")!=-1 || RegexUtil::find(fld, "[ \t]+static[ \t]+")!=-1)
					{
						RegexUtil::replace(fld, "[ \t]*static[ \t]+", " ");
						RegexUtil::replace(fld, "[ \t]+static[ \t]+", " ");
						//fldstatic = true;
					}
					RegexUtil::replace(fld, "[ \t]*const[ \t]+", " ");
					RegexUtil::replace(fld, "[ \t]+const[ \t]+", " ");

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
					if(fld.find(",")!=std::string::npos) {
						std::string fldsbc = fld.substr(0, fld.find(","));
						std::string fldss = fld.substr(fld.find(",")+1);
						fldp = StringUtil::splitAndReturn<std::vector<std::string> >(fldss, ",");
						fldp.insert(fldp.begin(), fldsbc.substr(0, fldsbc.find_last_of(" ")));
						fldp.insert(fldp.begin(), fldsbc.substr(fldsbc.find_last_of(" ")+1));
					} else {
						fldp.push_back(fld.substr(0, fld.find_last_of(" ")));
						fldp.push_back(fld.substr(fld.find_last_of(" ")+1));
					}
					for(unsigned int j = 1; j < fldp.size(); j++)
					{
						StringUtil::trim(fldp.at(j));
					}

					for(unsigned int j = 1; j < fldp.size(); j++)
					{
						std::string nam = fldp.at(j);
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
				if(((tes=classStructure.pro.at(i).find("("))==std::string::npos
						&& (tes=classStructure.pro.at(i).find(")"))==std::string::npos && classStructure.pro.at(i).find("~")==std::string::npos))
				{
					fld = classStructure.pro.at(i);

					//bool fldstatic = false;
					if(RegexUtil::find(fld, "[ \t]*static[ \t]+")!=-1 || RegexUtil::find(fld, "[ \t]+static[ \t]+")!=-1)
					{
						RegexUtil::replace(fld, "[ \t]*static[ \t]+", " ");
						RegexUtil::replace(fld, "[ \t]+static[ \t]+", " ");
						//fldstatic = true;
					}
					RegexUtil::replace(fld, "[ \t]*const[ \t]+", " ");
					RegexUtil::replace(fld, "[ \t]+const[ \t]+", " ");

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
					if(fld.find(",")!=std::string::npos) {
						std::string fldsbc = fld.substr(0, fld.find(","));
						std::string fldss = fld.substr(fld.find(",")+1);
						fldp = StringUtil::splitAndReturn<std::vector<std::string> >(fldss, ",");
						fldp.insert(fldp.begin(), fldsbc.substr(0, fldsbc.find_last_of(" ")));
						fldp.insert(fldp.begin(), fldsbc.substr(fldsbc.find_last_of(" ")+1));
					} else {
						fldp.push_back(fld.substr(0, fld.find_last_of(" ")));
						fldp.push_back(fld.substr(fld.find_last_of(" ")+1));
					}
					for(unsigned int j = 1; j < fldp.size(); j++)
					{
						StringUtil::trim(fldp.at(j));
					}

					for(unsigned int j = 1; j < fldp.size(); j++)
					{
						std::string nam = fldp.at(j);
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
				if((tes=classStructure.pub.at(i).find("("))!=std::string::npos && (tes=classStructure.pub.at(i).find(")"))!=std::string::npos && classStructure.pub.at(i).find("~")==std::string::npos
					&& fldnames.size()>0)
				{
					meth = classStructure.pub.at(i);
					StringUtil::replaceFirst(meth,";","");

					std::string argts = meth.substr(meth.find("("),meth.find(")")-meth.find("("));
					StringUtil::replaceFirst(argts,"(","");
					StringUtil::replaceAll(argts,")","");
					meth = meth.substr(0,meth.find("("));
					StringUtil::trim(meth);
					RegexUtil::replace(meth, "[ \t]*,[ \t]*", ",");

					//bool methstat = false;
					if(RegexUtil::find(meth, "[ \t]*static[ \t]+")!=-1 || RegexUtil::find(meth, "[ \t]+static[ \t]+")!=-1)
					{
						RegexUtil::replace(meth, "[ \t]*static[ \t]+", " ");
						RegexUtil::replace(meth, "[ \t]+static[ \t]+", " ");
						//methstat = true;
					}
					RegexUtil::replace(meth, "[ \t]*inline[ \t]+", " ");
					RegexUtil::replace(meth, "[ \t]+inline[ \t]+", " ");
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
					//bool ptr = false;
					if(meth.find("*")!=std::string::npos)
					{
						//ptr = true;
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
					{
						/*if(meth.find("operator>")!=std::string::npos && meth.find("operator>=")==std::string::npos && meth.find("operator=>")==std::string::npos)
						{
							classStructure.prosetser = true;
						}
						else */if(meth.find("operator<")!=std::string::npos/* && meth.find("operator<=")==std::string::npos && meth.find("operator=<")==std::string::npos*/)
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
								std::string cam = StringUtil::capitalizedCopy(fldnames.at(k+1));
								std::string arggk;
								if(argpm.size()==1)
								{
									arggk = argpm.at(0);
									StringUtil::replaceFirst(arggk, "&", "");
								}
								std::string methhk = methpm.at(0);
								StringUtil::replaceFirst(methhk, "&", "");
								if("set"+cam==methpm.at(1) && argpm.size()==1 && arggk==fldnames.at(k) && methpm.at(0)=="void")
								{
									fldstat[fldnames.at(k+1)]++;
								}
								else if("get"+cam==methpm.at(1) && argpm.size()==0 && methhk==fldnames.at(k))
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
				if(((tes=classStructure.pub.at(i).find("("))==std::string::npos
						&& (tes=classStructure.pub.at(i).find(")"))==std::string::npos && classStructure.pub.at(i).find("~")==std::string::npos))
				{
					fld = classStructure.pub.at(i);

					//bool fldstatic = false;
					if(RegexUtil::find(fld, "[ \t]*static[ \t]+")!=-1 || RegexUtil::find(fld, "[ \t]+static[ \t]+")!=-1)
					{
						RegexUtil::replace(fld, "[ \t]*static[ \t]+", " ");
						RegexUtil::replace(fld, "[ \t]+static[ \t]+", " ");
						//fldstatic = true;
					}
					RegexUtil::replace(fld, "[ \t]*const[ \t]+", " ");
					RegexUtil::replace(fld, "[ \t]+const[ \t]+", " ");

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
					if(fld.find(",")!=std::string::npos) {
						std::string fldsbc = fld.substr(0, fld.find(","));
						std::string fldss = fld.substr(fld.find(",")+1);
						fldp = StringUtil::splitAndReturn<std::vector<std::string> >(fldss, ",");
						fldp.insert(fldp.begin(), fldsbc.substr(0, fldsbc.find_last_of(" ")));
						fldp.insert(fldp.begin(), fldsbc.substr(fldsbc.find_last_of(" ")+1));
					} else {
						fldp.push_back(fld.substr(0, fld.find_last_of(" ")));
						fldp.push_back(fld.substr(fld.find_last_of(" ")+1));
					}
					for(unsigned int j = 1; j < fldp.size(); j++)
					{
						StringUtil::trim(fldp.at(j));
					}

					for(unsigned int j = 1; j < fldp.size(); j++)
					{
						std::string nam = fldp.at(j);
						if(isPrimitiveDataType(fldp.at(0)))
						{
							std::string typ = getTypeName(fldp.at(0));
							std::string serOpt = CastUtil::lexical_cast<std::string>(SerializeBase::identifySerOption(typ));
							if(!ptr)
							{
								methods += typ + " _objProp" + fldp.at(j) + " = " + "__obj->"+fldp.at(j) + ";\n";
								methods += "base->addObjectPrimitiveProperty(serobject, "+serOpt+", \""+fldp.at(j)+"\", \""+typ+"\", &_objProp"+fldp.at(j)+");\n"
										+"base->afterAddObjectProperty(serobject);\n";
								std::string cam = StringUtil::capitalizedCopy(fldp.at(j));
								typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldp.at(j)+"\", i))\n{\n"
										+typ+"* _val = ("+typ+"*)base->getObjectPrimitiveValue(base->getObjectProperty(intermediateObject, i), "+serOpt+", \""+typ+"\", \""+fldp.at(j)+"\");__obj->"+fldp.at(j)
										+" = *_val;\ndelete _val;\n}\n";
							}
							else
							{
								methods += "base->addObjectPrimitiveProperty(serobject, "+serOpt+", \""+fldp.at(j)+"\", \""+typ+"\", __obj->"+fldp.at(j)+");\n"
										+"base->afterAddObjectProperty(serobject);\n";
								std::string cam = StringUtil::capitalizedCopy(fldp.at(j));
								typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldp.at(j)+"\", i))\n{\n"
										+typ+"* _val = ("+typ+"*)base->getObjectPrimitiveValue(base->getObjectProperty(intermediateObject, i), "+serOpt+", \""+typ+"\", \""+fldp.at(j)+"\");__obj->"+fldp.at(j)
										+" = _val;\n}\n";
							}
						}
						else if(fldp.at(0)=="Date")
						{
							std::string serOpt = CastUtil::lexical_cast<std::string>(SerializeBase::identifySerOption(fldp.at(0)));
							if(!ptr)
							{
								methods += fldp.at(0) + " _objProp" + fldp.at(j) + " = " + "__obj->"+fldp.at(j) + ";\n";
								methods += ("base->addObjectPrimitiveProperty(serobject, "+serOpt+", \""+fldp.at(j)+"\", \""
										+fldp.at(0)+"\", &_objProp"+fldp.at(j)+");\n"
										+"base->afterAddObjectProperty(serobject);\n");
								std::string cam = StringUtil::capitalizedCopy(fldp.at(j));
								typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldp.at(j)+"\", i))\n{\n"
										+"std::string* _val = (std::string*)base->getObjectPrimitiveValue(base->getObjectProperty(intermediateObject, i), "+serOpt+", \"std::string\", \""+fldp.at(j)+"\");\nDateFormat formt"
										+fldp.at(j)+"(\"yyyy-mm-dd hh:mi:ss\");\n__obj->"
										+fldp.at(j)+" = *(formt"+fldp.at(j)+".parse(*_val));\ndelete _val;\n}\n";
							}
							else
							{
								methods += ("base->addObjectPrimitiveProperty(serobject, "+serOpt+", \""+fldp.at(j)+"\", \""
										+fldp.at(0)+"\", __obj->"+fldp.at(j)+");\n"
										+"base->afterAddObjectProperty(serobject);\n");
								std::string cam = StringUtil::capitalizedCopy(fldp.at(j));
								typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldp.at(j)+"\", i))\n{\n"
										+"std::string* _val = (std::string*)base->getObjectPrimitiveValue(base->getObjectProperty(intermediateObject, i), "+serOpt+", \"std::string\", \""+fldp.at(j)+"\");\nDateFormat formt"
										+fldp.at(j)+"(\"yyyy-mm-dd hh:mi:ss\");\n__obj->"
										+fldp.at(j)+" = (formt"+fldp.at(j)+".parse(*_val));\ndelete _val;\n}\n";
							}
						}
						else if(fldp.at(0)=="BinaryData")
						{
							std::string serOpt = CastUtil::lexical_cast<std::string>(SerializeBase::identifySerOption(fldp.at(0)));
							if(!ptr)
							{
								methods += fldp.at(0) + " _objProp" + fldp.at(j) + " = " + "__obj->"+fldp.at(j) + ";\n";
								methods += ("base->addObjectPrimitiveProperty(serobject, "+serOpt+", \""+fldp.at(j)+"\", \""
										+fldp.at(0)+"\", &_objProp"+fldp.at(j)+");\n"
										+"base->afterAddObjectProperty(serobject);\n");
								std::string cam = StringUtil::capitalizedCopy(fldp.at(j));
								typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldp.at(j)+"\", i))\n{\n"
										+"std::string* _val = (std::string*)base->getObjectPrimitiveValue(base->getObjectProperty(intermediateObject, i), "+serOpt+", \"std::string\", \""+fldp.at(j)+"\");"
										+ "\n__obj->"+fldp.at(j)+" = *(BinaryData::unSerilaize(*_val));\ndelete _val;\n}\n";
							}
							else
							{
								methods += ("base->addObjectPrimitiveProperty(serobject, "+serOpt+", \""+fldp.at(j)+"\", \""
										+fldp.at(0)+"\", __obj->"+fldp.at(j)+");\n"
										+"base->afterAddObjectProperty(serobject);\n");
								std::string cam = StringUtil::capitalizedCopy(fldp.at(j));
								typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldp.at(j)+"\", i))\n{\n"
										+"std::string* _val = (std::string*)base->getObjectPrimitiveValue(base->getObjectProperty(intermediateObject, i), "+serOpt+", \"std::string\", \""+fldp.at(j)+"\");"
										+ "\n__obj->"+fldp.at(j)+" = (BinaryData::unSerilaize(*_val));\ndelete _val;\n}\n";
							}
						}
						else if(fldp.at(0).find("vector")!=std::string::npos || fldp.at(0).find("queue")!=std::string::npos || fldp.at(0).find("deque")!=std::string::npos || fldp.at(0).find("set")!=std::string::npos || fldp.at(0).find("list")!=std::string::npos)
						{
							std::string stlcnt = fldp.at(0);
							std::string stltyp = fldp.at(0);
							std::string contType;
							StringUtil::replaceFirst(stltyp,"std::","");
							StringUtil::replaceFirst(stltyp,"<","::");
							StringUtil::replaceFirst(stltyp,">","");
							StringUtil::replaceFirst(stltyp," ","");
							std::string stlcnttyp = "";
							if(fldp.at(0).find("vector")!=std::string::npos)
							{
								contType = "std::vector<";
								stlcnttyp = "Vec";
							}
							else if(fldp.at(0).find("queue")!=std::string::npos)
							{
								contType = "std::queue<";
								stlcnttyp = "Q";
							}
							else if(fldp.at(0).find("deque")!=std::string::npos)
							{
								contType = "std::deque<";
								stlcnttyp = "Dq";
							}
							else if(fldp.at(0).find("list")!=std::string::npos)
							{
								contType = "std::list<";
								stlcnttyp = "Lis";
							}
							else if(fldp.at(0).find("multiset")!=std::string::npos)
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

							std::string fqcn = contType + getFullyQualifiedClassName(stlcnt, classStructure.namespaces);
							if(fqcn.find(">")==std::string::npos) {
								fqcn += ">";
							} else {
								fqcn += " >";
							}
							contType += getFullyQualifiedClassName(stlcnt, classStructure.namespaces) + ",";
							std::string serOpt = CastUtil::lexical_cast<std::string>(SerializeBase::identifySerOption(fqcn));

							if(!ptr)
							{
								//methods += (fqcn+" __temp_obj_ser"+fldp.at(j)+" = __obj->"+fldp.at(j)+";\n");
								methods += ("base->addObjectProperty(serobject, \""+fldp.at(j)+"\", \""+fldp.at(0)+"\", "
										+ "SerializeBase::serializeUnknown(&(__obj->"+fldp.at(j)+"),"+serOpt+",\""+fqcn+"\",\""+app+"\", base));\n"
										+"base->afterAddObjectProperty(serobject);\n");
								std::string cam = StringUtil::capitalizedCopy(fldp.at(j));
								typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldp.at(j)+"\", i))\n__obj->"+fldp.at(j)+" = "
										 + "SerializeBase::unSerializeKnown<"+fqcn+" >(base->getContainerElement("
										 + "intermediateObject, i, 0),"+serOpt+",\""+contType+"\",\""+app+"\", base);\n";
							}
							else
							{
								//methods += (fqcn+"* __temp_obj_ser"+fldp.at(j)+" = __obj->"+fldp.at(j)+";\n");
								methods += ("if(__obj->"+fldp.at(j)+"!=NULL)base->addObjectProperty(serobject, \""+fldp.at(j)+"\", \""+fldp.at(0)+"\", "
										+ "SerializeBase::serializeUnknown(__obj->"+fldp.at(j)+","+serOpt+",\""+fqcn+"\",\""+app+"\", base));\n"
										+"base->afterAddObjectProperty(serobject);\n");
								std::string cam = StringUtil::capitalizedCopy(fldp.at(j));
								typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldp.at(j)+"\", i))\n__obj->"+fldp.at(j)+" = "
										 + "SerializeBase::unSerializeKnownToPointer<"+fqcn+" >(base->getContainerElement("
										 +"intermediateObject, i, 0),"+serOpt+",\""+contType+"\",\""+app+"\", base);\n";
							}
						}
						else
						{
							std::string fqcn = getFullyQualifiedClassName(fldp.at(0), classStructure.namespaces);
							std::string serOpt = CastUtil::lexical_cast<std::string>(SerializeBase::identifySerOption(fqcn));
							if(!ptr)
							{
								//methods += (fqcn+" __temp_obj_ser"+fldp.at(j)+" = __obj->"+fldp.at(j)+";\n");
								methods += ("base->addObjectProperty(serobject, \""+fldp.at(j)+"\", \""+fldp.at(0)+"\", "
										+ "SerializeBase::serializeUnknown(&(__obj->"+fldp.at(j)+"),"+serOpt+",\""+fqcn+"\",\""+app+"\", base));\n"
										+"base->afterAddObjectProperty(serobject);\n");
								std::string cam = StringUtil::capitalizedCopy(fldp.at(j));
								typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldp.at(j)+"\", i))\n__obj->"+fldp.at(j)+" = "
										 + "SerializeBase::unSerializeKnown<"+fqcn+" >(base->getContainerElement("
										 +"intermediateObject, i, 0),"+serOpt+",\""+fqcn+"\",\""+app+"\", base);\n";
							}
							else
							{
								//methods += (fldp.at(0)+"* __temp_obj_ser"+fldp.at(j)+" = __obj->"+fldp.at(j)+";\n");
								methods += ("if(__obj->"+fldp.at(j)+"!=NULL)base->addObjectProperty(serobject, \""+fldp.at(j)+"\", \""+fldp.at(0)+"\", "
										+ "SerializeBase::serializeUnknown(__obj->"+fldp.at(j)+","+serOpt+",\""+fqcn+"\",\""+app+"\", base));\n"
										+"base->afterAddObjectProperty(serobject);\n");
								std::string cam = StringUtil::capitalizedCopy(fldp.at(j));
								typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldp.at(j)+"\", i))\n__obj->"+fldp.at(j)+" = "
										 + "SerializeBase::unSerializeKnownToPointer<"+fqcn+" >(base->getContainerElement("
										 +"intermediateObject, i, 0),"+serOpt+",\""+fqcn+"\",\""+app+"\", base);\n";
							}
						}
						//structinf += (fldp.at(0)+" "+fldp.at(j)+";\n");
					}
				}
				if((tes=classStructure.pub.at(i).find("("))!=std::string::npos && (tes=classStructure.pub.at(i).find(")"))!=std::string::npos && classStructure.pub.at(i).find("~")==std::string::npos
						&& fldnames.size()>0)
				{
					meth = classStructure.pub.at(i);
					StringUtil::replaceFirst(meth,";","");

					std::string argts = meth.substr(meth.find("("),meth.find(")")-meth.find("("));
					StringUtil::replaceFirst(argts,"(","");
					StringUtil::replaceAll(argts,")","");
					meth = meth.substr(0,meth.find("("));
					StringUtil::trim(meth);
					RegexUtil::replace(meth, "[ \t]*,[ \t]*", ",");

					//bool methstat = false;
					if(RegexUtil::find(meth, "[ \t]*static[ \t]+")!=-1 || RegexUtil::find(meth, "[ \t]+static[ \t]+")!=-1)
					{
						RegexUtil::replace(meth, "[ \t]*static[ \t]+", " ");
						RegexUtil::replace(meth, "[ \t]+static[ \t]+", " ");
						//methstat = true;
					}
					RegexUtil::replace(meth, "[ \t]*inline[ \t]+", " ");
					RegexUtil::replace(meth, "[ \t]+inline[ \t]+", " ");
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
					{
						/*if(meth.find("operator>")!=std::string::npos && meth.find("operator>=")==std::string::npos && meth.find("operator=>")==std::string::npos)
						{
							classStructure.prosetser = true;
						}
						else */if(meth.find("operator<")!=std::string::npos/* && meth.find("operator<=")==std::string::npos && meth.find("operator=<")==std::string::npos*/)
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
								std::string cam = StringUtil::capitalizedCopy(fldnames.at(k+1));
								PropStructure cps = classStructure.getPs(fldnames.at(k+1));
								if(cps.markers.find("@IgnoreSer")!=cps.markers.end()) {
									continue;
								}
								if(argpm.size()==1)
									StringUtil::replaceFirst(argpm.at(0), "&", "");
								StringUtil::replaceFirst(methpm.at(0), "&", "");
								if("set"+cam==methpm.at(1) && argpm.size()==1 && argpm.at(0)==fldnames.at(k) && methpm.at(0)=="void")
								{
									if(isPrimitiveDataType(argpm.at(0)))
									{
										argpm.at(0) = getTypeName(argpm.at(0));
										std::string serOpt = CastUtil::lexical_cast<std::string>(SerializeBase::identifySerOption(argpm.at(0)));
										if(!ptr)
											typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldnames.at(k+1)+"\", i))\n{\n"
													 +argpm.at(0)+"* _val = ("+argpm.at(0)+"*)base->getObjectPrimitiveValue(base->getObjectProperty(intermediateObject, i), "+serOpt+", \""
													 +argpm.at(0)+"\", \""+fldnames.at(k+1)+"\");\n__obj->"+methpm.at(1)+"(*_val);\ndelete _val;\n}\n";
										else
											typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldnames.at(k+1)+"\", i))\n{\n"
													 +argpm.at(0)+"* _val = ("+argpm.at(0)+"*)base->getObjectPrimitiveValue(base->getObjectProperty(intermediateObject, i), "+serOpt+", \""
													 +argpm.at(0)+"\", \""+fldnames.at(k+1)+"\");\n__obj->"+methpm.at(1)+"(_val);\n}\n";
									}
									else if(argpm.at(0)=="Date")
									{
										std::string serOpt = CastUtil::lexical_cast<std::string>(SerializeBase::identifySerOption(argpm.at(0)));
										if(!ptr)
											typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldnames.at(k+1)+"\", i))\n{\n"
													 +"std::string* _val = (std::string*)base->getObjectPrimitiveValue(base->getObjectProperty(intermediateObject, i), "+serOpt+", \"std::string\", \""+fldnames.at(k+1)+"\");\n"
													 +"DateFormat formt"+cam+"(\"yyyy-mm-dd hh:mi:ss\");\n__obj->"
													 +methpm.at(1)+"(*(formt"+cam+".parse(*_val)));\ndelete _val;\n}\n";
										else
											typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldnames.at(k+1)+"\", i))\n{\n"
													 +"std::string* _val = (std::string*)base->getObjectPrimitiveValue(base->getObjectProperty(intermediateObject, i), "+serOpt+", \"std::string\", \""+fldnames.at(k+1)+"\");\n"
													 +"DateFormat formt"+cam+"(\"yyyy-mm-dd hh:mi:ss\");\n__obj->"
													 +methpm.at(1)+"(formt"+cam+".parse(*_val));\ndelete _val;\n}\n";
									}
									else if(argpm.at(0)=="BinaryData")
									{
										std::string serOpt = CastUtil::lexical_cast<std::string>(SerializeBase::identifySerOption(argpm.at(0)));
										if(!ptr)
											typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldnames.at(k+1)+"\", i))\n{\n"
													+"std::string* _val = (std::string*)base->getObjectPrimitiveValue(base->getObjectProperty(intermediateObject, i), "+serOpt+", \"std::string\", \""+fldnames.at(k+1)+"\");"
													+ "\n__obj->"+methpm.at(1)+"(*(BinaryData::unSerilaize(*_val)));\ndelete _val;\n}\n";
										else
											typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldnames.at(k+1)+"\", i))\n{\n"
													+"std::string* _val = (std::string*)base->getObjectPrimitiveValue(base->getObjectProperty(intermediateObject, i), "+serOpt+", \"std::string\", \""+fldnames.at(k+1)+"\");"
													+ "\n__obj->"+methpm.at(1)+"((BinaryData::unSerilaize(*_val)));\ndelete _val;\n}\n";
									}
									else if(argpm.at(0).find("vector")!=std::string::npos || argpm.at(0).find("queue")!=std::string::npos ||
											argpm.at(0).find("deque")!=std::string::npos || argpm.at(0).find("set")!=std::string::npos ||
											argpm.at(0).find("list")!=std::string::npos || argpm.at(0).find("multiset")!=std::string::npos)
									{
										std::string stlcnt = argpm.at(0);
										std::string stltyp = argpm.at(0);
										std::string contType;
										StringUtil::replaceFirst(stltyp,"std::","");
										StringUtil::replaceFirst(stltyp,"<","::");
										StringUtil::replaceFirst(stltyp,">","");
										StringUtil::replaceFirst(stltyp," ","");
										std::string stlcnttyp = "";
										if(argpm.at(0).find("vector")!=std::string::npos)
										{
											contType = "std::vector<";
											stlcnttyp = "Vec";
										}
										else if(argpm.at(0).find("queue")!=std::string::npos)
										{
											contType = "std::queue<";
											stlcnttyp = "Q";
										}
										else if(argpm.at(0).find("deque")!=std::string::npos)
										{
											contType = "std::deque<";
											stlcnttyp = "Dq";
										}
										else if(argpm.at(0).find("list")!=std::string::npos)
										{
											contType = "std::list<";
											stlcnttyp = "Lis";
										}
										else if(argpm.at(0).find("multiset")!=std::string::npos)
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

										std::string fqcn = contType + getFullyQualifiedClassName(stlcnt, classStructure.namespaces) + " >";
										contType += getFullyQualifiedClassName(stlcnt, classStructure.namespaces) + ",";
										std::string serOpt = CastUtil::lexical_cast<std::string>(SerializeBase::identifySerOption(fqcn));

										if(!ptr)
										{
											typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldnames.at(k+1)+"\", i)){";
											typedefs += "\n__obj->set"+cam+"(SerializeBase::unSerializeKnown<"+fqcn+" >"
													 + "(base->getContainerElement(intermediateObject, i, 0),"+serOpt+",\""+contType+"\",\""+app+"\", base));\n";
											typedefs += "\n}\n";
										}
										else
										{
											typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldnames.at(k+1)+"\", i)){";
											typedefs += "\n__obj->set"+cam+"(SerializeBase::unSerializeKnownToPointer<"+fqcn+" >"
													 + "(base->getContainerElement(intermediateObject, i, 0),"+serOpt+",\""+contType+"\",\""+app+"\", base));\n";
											typedefs += "\n}\n";
										}
									}
									else
									{
										std::string fqcn = getFullyQualifiedClassName(argpm.at(0), classStructure.namespaces);
										std::string serOpt = CastUtil::lexical_cast<std::string>(SerializeBase::identifySerOption(fqcn));
										if(!ptr)
											typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldnames.at(k+1)+"\", i))"
													 + "\n__obj->"+methpm.at(1)+"(SerializeBase::unSerializeKnown<"+fqcn+" >"
													 + "(base->getContainerElement(intermediateObject, i, 0),"+serOpt+",\""+fqcn+"\",\""+app+"\", base));\n";
										else
											typedefs += "if(base->isValidObjectProperty(intermediateObject, \""+fldnames.at(k+1)+"\", i))"
													 + "\n__obj->"+methpm.at(1)+"(SerializeBase::unSerializeKnownToPointer<"+fqcn+" >"
													 + "(base->getContainerElement(intermediateObject, i, 0),"+serOpt+",\""+fqcn+"\",\""+app+"\", base));\n";
									}
								}
								else if("get"+cam==methpm.at(1) && argpm.size()==0 && methpm.at(0)==fldnames.at(k))
								{
									if(isPrimitiveDataType(methpm.at(0)))
									{
										methpm.at(0) = getTypeName(methpm.at(0));
										std::string serOpt = CastUtil::lexical_cast<std::string>(SerializeBase::identifySerOption(methpm.at(0)));
										if(!ptr)
										{
											methods += methpm.at(0) + " _objProp" + fldnames.at(k+1) + " = " + "__obj->"+methpm.at(1) + "();\n";
											methods += ("base->addObjectPrimitiveProperty(serobject, "+serOpt+", \""+fldnames.at(k+1)+"\", \""+methpm.at(0)+"\", &_objProp"+fldnames.at(k+1)+");\n"
													+"base->afterAddObjectProperty(serobject);\n");
										}
										else
										{
											methods += ("if(__obj->"+methpm.at(1)+"()!=NULL)base->addObjectPrimitiveProperty(serobject, "+serOpt+", \""+fldnames.at(k+1)+"\", \""+methpm.at(0)+"\", "
													+"__obj->"+methpm.at(1)+"());\n"
													+"base->afterAddObjectProperty(serobject);\n");
										}
									}
									else if(methpm.at(0)=="Date")
									{
										std::string serOpt = CastUtil::lexical_cast<std::string>(SerializeBase::identifySerOption(methpm.at(0)));
										if(!ptr)
										{
											methods += methpm.at(0) + " _objProp" + fldnames.at(k+1) + " = " + "__obj->"+methpm.at(1) + "();\n";
											methods += ("base->addObjectPrimitiveProperty(serobject, "+serOpt+", \""
													+fldnames.at(k+1)+"\", \""+methpm.at(0)+"\", &_objProp"+fldnames.at(k+1)+");\n"
													+"base->afterAddObjectProperty(serobject);\n");
										}
										else
										{
											methods += ("if(__obj->"+methpm.at(1)+"()!=NULL){\n"
													+"base->addObjectPrimitiveProperty(serobject, "+serOpt+", \""+fldnames.at(k+1)+"\", \""+methpm.at(0)+"\", "
													+"__obj->"+methpm.at(1)+"());\n"
													+"base->afterAddObjectProperty(serobject);\n");
										}
									}
									else if(methpm.at(0)=="BinaryData")
									{
										std::string serOpt = CastUtil::lexical_cast<std::string>(SerializeBase::identifySerOption(methpm.at(0)));
										if(!ptr)
										{
											methods += methpm.at(0) + " _objProp" + fldnames.at(k+1) + " = " + "__obj->"+methpm.at(1) + "();\n";
											methods += ("base->addObjectPrimitiveProperty(serobject, "+serOpt+", \""+fldnames.at(k+1)+"\", \""+methpm.at(0)+"\", "
													+"&_objProp"+fldnames.at(k+1)+");\n"
													+"base->afterAddObjectProperty(serobject);\n");
										}
										else
										{
											methods += ("if(__obj->"+methpm.at(1)+"()!=NULL)base->addObjectPrimitiveProperty(serobject, "+serOpt+", \""+fldnames.at(k+1)+"\", \""+methpm.at(0)+"\", "
													"__obj->"+methpm.at(1)+"());\n"
													+"base->afterAddObjectProperty(serobject);\n");
										}
									}
									else if(methpm.at(0).find("vector")!=std::string::npos || methpm.at(0).find("queue")!=std::string::npos ||
											methpm.at(0).find("deque")!=std::string::npos || methpm.at(0).find("set")!=std::string::npos ||
											methpm.at(0).find("list")!=std::string::npos || methpm.at(0).find("multiset")!=std::string::npos)
									{
										std::string contType;
										std::string stlcnt = methpm.at(0);
										std::string stltyp = methpm.at(0);
										StringUtil::replaceFirst(stltyp,"std::","");
										StringUtil::replaceFirst(stltyp,"<","::");
										StringUtil::replaceFirst(stltyp,">","");
										StringUtil::replaceFirst(stltyp," ","");
										std::string stlcnttyp = "";
										if(methpm.at(0).find("vector")!=std::string::npos)
										{
											contType = "std::vector<";
											stlcnttyp = "Vec";
										}
										else if(methpm.at(0).find("queue")!=std::string::npos)
										{
											contType = "std::queue<";
											stlcnttyp = "Q";
										}
										else if(methpm.at(0).find("deque")!=std::string::npos)
										{
											contType = "std::deque<";
											stlcnttyp = "Dq";
										}
										else if(methpm.at(0).find("list")!=std::string::npos)
										{
											contType = "std::list<";
											stlcnttyp = "Lis";
										}
										else if(methpm.at(0).find("multiset")!=std::string::npos)
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

										std::string fqcn = contType + getFullyQualifiedClassName(stlcnt, classStructure.namespaces);
										if(fqcn.find(">")==std::string::npos) {
											fqcn += ">";
										} else {
											fqcn += " >";
										}
										std::string stlcontwosp = methpm.at(0);
										StringUtil::trim(stlcontwosp);
										std::string serOpt = CastUtil::lexical_cast<std::string>(SerializeBase::identifySerOption(fqcn));

										if(!ptr)
										{
											methods += (fqcn+" __temp_obj_ser"+fldnames.at(k+1)+" = __obj->"+methpm.at(1)+"();\n");
											methods += ("base->addObjectProperty(serobject, \""+fldnames.at(k+1)+"\", \""+stlcontwosp+"\", "
													+"SerializeBase::serializeUnknown(&__temp_obj_ser"+fldnames.at(k+1)+","+serOpt+",\""+fqcn+"\",\""+app+"\", base));\n"
													+"base->afterAddObjectProperty(serobject);\n");
										}
										else
										{
											methods += (fqcn+"* __temp_obj_ser"+fldnames.at(k+1)+" = __obj->"+methpm.at(1)+"();\n");
											methods += ("if(__obj->"+methpm.at(1)+"()!=NULL)base->addObjectProperty(serobject, \""+fldnames.at(k+1)+"\", \""+stlcontwosp+"\", "
													+"SerializeBase::serializeUnknown(__temp_obj_ser"+fldnames.at(k+1)+","+serOpt+",\""+fqcn+"\",\""+app+"\", base));\n"
													+"base->afterAddObjectProperty(serobject);\n");
										}
									}
									else
									{
										std::string fqcn = getFullyQualifiedClassName(methpm.at(0), classStructure.namespaces);
										std::string serOpt = CastUtil::lexical_cast<std::string>(SerializeBase::identifySerOption(fqcn));
										if(!ptr)
										{
											methods += (methpm.at(0)+" __temp_obj_ser"+fldnames.at(k+1)+" = __obj->"+methpm.at(1)+"();\n");
											methods += ("base->addObjectProperty(serobject, \""+fldnames.at(k+1)+"\", \""+methpm.at(0)+"\", "
													+ "SerializeBase::serializeUnknown(&__temp_obj_ser"+fldnames.at(k+1)+","+serOpt+",\""+fqcn+"\",\""+app+"\", base));\n"
													+"base->afterAddObjectProperty(serobject);\n");
										}
										else
										{
											methods += (methpm.at(0)+"* __temp_obj_ser"+fldnames.at(k+1)+" = __obj->"+methpm.at(1)+"();\n");
											methods += ("if(__obj->"+methpm.at(1)+"()!=NULL)base->addObjectProperty(serobject, \""+fldnames.at(k+1)+"\", \""+methpm.at(0)+"\", "
													+"SerializeBase::serializeUnknown(__temp_obj_ser"+fldnames.at(k+1)+","+serOpt+",\""+fqcn+"\",\""+app+"\", base));\n"
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
				+"std::string ser = base->fromSerializableObjectToString(serobject);\nbase->cleanSerializableObject(serobject);\nreturn ser;\n}\n";
		methods += "\nstd::string " +ttapp+ "serialize" + classStructure.getTreatedClassName(true) + "Container(void* obje, SerializeBase* base, const std::string& _cont)\n{\nvoid* object = base->getSerializableObject();\nbase->startContainerSerialization(object, \""+classStructure.getTreatedClassName(true)+"\", _cont);\n"
				+"int size = SerializeBase::getNestedContainerSize<"+classStructure.getFullyQualifiedClassName()+">(_cont,obje);\n"
				+"for(int i=0;i<size;i++)\n{\n" + classStructure.getFullyQualifiedClassName() + " _lo =SerializeBase::getValueFromNestedContainer<"+classStructure.getFullyQualifiedClassName()+">(_cont,obje,i);\n;base->addContainerSerializableElement(object, "+ttapp+"serialize"
				+classStructure.getTreatedClassName(true)+"(&_lo, base));\nbase->afterAddContainerSerializableElement(object,i,size);\n}\n"
				+"base->endContainerSerialization(object, \""+classStructure.getTreatedClassName(true)+"\", _cont);\nstd::string ser = base->fromSerializableObjectToString(object);\n"
				+"base->cleanSerializableObject(object);\nreturn ser;\n}\n";
		/*methods += "\nstd::string " +ttapp+ "serialize" + classStructure.getTreatedClassName(true) + "Vec(void* obje, SerializeBase* base)\n{\nstd::vector<"
				+classStructure.getFullyQualifiedClassName()+"> *__obj=(std::vector<"+classStructure.getFullyQualifiedClassName()+">*)obje;\n"
				+"void* object = base->getSerializableObject();\nbase->startContainerSerialization(object, \""+classStructure.getTreatedClassName(true)+"\", \"vector\");\n"
				+"int cnt = 0;\nint size = __obj->size();\nstd::vector<"+classStructure.getFullyQualifiedClassName()+">::iterator it = __obj->begin();\n"
				+"for(it = __obj->begin(); it!= __obj->end(); ++it)\n{\nbase->addContainerSerializableElement(object, "+ttapp+"serialize"
				+classStructure.getTreatedClassName(true)+"(&(*it), base));\nbase->afterAddContainerSerializableElement(object, cnt++, size);\n}\n"
				+"base->endContainerSerialization(object, \""+classStructure.getTreatedClassName(true)+"\", \"std::vector\");\nstd::string ser = base->fromSerializableObjectToString(object);\n"
				+"base->cleanSerializableObject(object);\nreturn ser;\n}\n";
		methods += "\nstd::string " +ttapp+ "serialize" + classStructure.getTreatedClassName(true) + "Dq(void* obje, SerializeBase* base)\n{\nstd::deque<"
				+classStructure.getFullyQualifiedClassName()+"> *__obj=(std::deque<"+classStructure.getFullyQualifiedClassName()+">*)obje;\n"
				+"void* object = base->getSerializableObject();\nbase->startContainerSerialization(object, \""+classStructure.getTreatedClassName(true)+"\", \"deque\");\n"
				+"int cnt = 0;\nint size = __obj->size();\nstd::deque<"+classStructure.getFullyQualifiedClassName()+">::iterator it = __obj->begin();\n"
				+"for(it = __obj->begin(); it!= __obj->end(); ++it)\n{\nbase->addContainerSerializableElement(object, "+ttapp+"serialize"
				+classStructure.getTreatedClassName(true)+"(&(*it), base));\nbase->afterAddContainerSerializableElement(object, cnt++, size);\n}\n"
				+"base->endContainerSerialization(object, \""+classStructure.getTreatedClassName(true)+"\", \"std::deque\");\nstd::string ser = base->fromSerializableObjectToString(object);\n"
				+"base->cleanSerializableObject(object);\nreturn ser;\n}\n";
		methods += "\nstd::string " +ttapp+ "serialize" + classStructure.getTreatedClassName(true) + "Lis(void* obje, SerializeBase* base)\n{\nstd::list<"
				+classStructure.getFullyQualifiedClassName()+"> *__obj=(std::list<"+classStructure.getFullyQualifiedClassName()+">*)obje;\n"
				+"void* object = base->getSerializableObject();\nbase->startContainerSerialization(object, \""+classStructure.getTreatedClassName(true)+"\", \"list\");\n"
				+"int cnt = 0;\nint size = __obj->size();\nstd::list<"+classStructure.getFullyQualifiedClassName()+">::iterator it = __obj->begin();\n"
				+"for(it = __obj->begin(); it!= __obj->end(); ++it)\n{\nbase->addContainerSerializableElement(object, "+ttapp+"serialize"
				+classStructure.getTreatedClassName(true)+"(&(*it), base));\nbase->afterAddContainerSerializableElement(object, cnt++, size);\n}\n"
				+"base->endContainerSerialization(object, \""+classStructure.getTreatedClassName(true)+"\", \"std::list\");\nstd::string ser = base->fromSerializableObjectToString(object);\n"
				+"base->cleanSerializableObject(object);\nreturn ser;\n}\n";
		methods += "\nstd::string " +ttapp+ "serialize" + classStructure.getTreatedClassName(true) + "Q(void* obje, SerializeBase* base)\n{\nstd::queue<"
				+classStructure.getFullyQualifiedClassName()+"> *__obj=(std::queue<"+classStructure.getFullyQualifiedClassName()+">*)obje;\nstd::queue<"+
				classStructure.getFullyQualifiedClassName()+"> *tt = new std::queue<"+classStructure.getFullyQualifiedClassName()
				+">;	*tt = *__obj;void* object = base->getSerializableObject();\nbase->startContainerSerialization(object, \""+classStructure.getTreatedClassName(true)+"\", \"queue\");\n"
				+"int cnt = 0;\nint size = __obj->size();\n"
				+"for(int var=0;var<size;var++)\n{\nbase->addContainerSerializableElement(object, "+ttapp+"serialize"
				+classStructure.getTreatedClassName(true)+"(&(tt->front()), base));\ntt->pop();\nbase->afterAddContainerSerializableElement(object, cnt++, size);\n}\n"
				+"base->endContainerSerialization(object, \""+classStructure.getTreatedClassName(true)+"\", \"std::queue\");\ndelete tt;\nstd::string ser = base->fromSerializableObjectToString(object);\n"
				+"base->cleanSerializableObject(object);\nreturn ser;\n}\n";
		classes += "\nstd::string " +ttapp+ "serialize"+classStructure.getTreatedClassName(true)+"Vec(void* obje, SerializeBase* base);\n"
				+"std::string " +ttapp+ "serialize"+classStructure.getTreatedClassName(true)+"Lis(void* obje, SerializeBase* base);\n"
				+"std::string " +ttapp+ "serialize"+classStructure.getTreatedClassName(true)+"Dq(void* obje, SerializeBase* base);\n"
				+"std::string " +ttapp+ "serialize"+classStructure.getTreatedClassName(true)+"Q(void* obje, SerializeBase* base);\n";*/
		classes += "\nstd::string " +ttapp+ "serialize"+classStructure.getTreatedClassName(true)+"Container(void* obje, SerializeBase* base, const std::string& _cont);\n";
		if(classStructure.prosetser)
		{
			methods += "\nstd::string " +ttapp+ "serialize" + classStructure.getTreatedClassName(true) + "ContainerSV(void* obje, SerializeBase* base, const std::string& _cont)\n{\nvoid* object = base->getSerializableObject();\nbase->startContainerSerialization(object, \""+classStructure.getTreatedClassName(true)+"\", _cont);\n"
					+"int size = SerializeBase::getNestedContainerSizeSV<"+classStructure.getFullyQualifiedClassName()+">(_cont,obje);\n"
					+"for(int i=0;i<size;i++)\n{\n" + classStructure.getFullyQualifiedClassName() + " _lo =SerializeBase::getValueFromNestedContainerSV<"+classStructure.getFullyQualifiedClassName()+">(_cont,obje,i);\n;base->addContainerSerializableElement(object, "+ttapp+"serialize"
					+classStructure.getTreatedClassName(true)+"(&_lo, base));\nbase->afterAddContainerSerializableElement(object,i,size);\n}\n"
					+"base->endContainerSerialization(object, \""+classStructure.getTreatedClassName(true)+"\", _cont);\nstd::string ser = base->fromSerializableObjectToString(object);\n"
					+"base->cleanSerializableObject(object);\nreturn ser;\n}\n";
			/*methods += "\nstd::string " +ttapp+ "serialize" + classStructure.getTreatedClassName(true) + "Set(void* obje, SerializeBase* base)\n{\nstd::set<"
					+classStructure.getFullyQualifiedClassName()+"> *__obj=(std::set<"+classStructure.getFullyQualifiedClassName()+">*)obje;\n"
					+"void* object = base->getSerializableObject();\nbase->startContainerSerialization(object, \""+classStructure.getTreatedClassName(true)+"\", \"set\");\n"
					+"int cnt = 0;\nint size = __obj->size();\nstd::set<"+classStructure.getFullyQualifiedClassName()+">::iterator it = __obj->begin();\n"
					+"for(it = __obj->begin(); it!= __obj->end(); ++it)\n{\nbase->addContainerSerializableElement(object, "+ttapp+"serialize"
					+classStructure.getTreatedClassName(true)+"((void*)&(*it), base));\nbase->afterAddContainerSerializableElement(object, cnt++, size);\n}\n"
					+"base->endContainerSerialization(object, \""+classStructure.getTreatedClassName(true)+"\", \"std::set\");\nstd::string ser = base->fromSerializableObjectToString(object);\n"
					+"base->cleanSerializableObject(object);\nreturn ser;\n}\n";
			methods += "\nstd::string " +ttapp+ "serialize" + classStructure.getTreatedClassName(true) + "MulSet(void* obje, SerializeBase* base)\n{\nstd::multiset<"
					+classStructure.getFullyQualifiedClassName()+"> *__obj=(std::multiset<"+classStructure.getFullyQualifiedClassName()+">*)obje;\n"
					+"void* object = base->getSerializableObject();\nbase->startContainerSerialization(object, \""+classStructure.getTreatedClassName(true)+"\", \"multiset\");\n"
					+"int cnt = 0;\nint size = __obj->size();\nstd::multiset<"+classStructure.getFullyQualifiedClassName()+">::iterator it = __obj->begin();\n"
					+"for(it = __obj->begin(); it!= __obj->end(); ++it)\n{\nbase->addContainerSerializableElement(object, "+ttapp+"serialize"
					+classStructure.getTreatedClassName(true)+"((void*)&(*it), base));\nbase->afterAddContainerSerializableElement(object, cnt++, size);\n}\n"
					+"base->endContainerSerialization(object, \""+classStructure.getTreatedClassName(true)+"\", \"std::multiset\");\nstd::string ser = base->fromSerializableObjectToString(object);\n"
					+"base->cleanSerializableObject(object);\nreturn ser;\n}\n";
			classes += "\nstd::string " +ttapp+ "serialize"+classStructure.getTreatedClassName(true)+"Set(void* obje, SerializeBase* base);\n"
					+"std::string " +ttapp+ "serialize"+classStructure.getTreatedClassName(true)+"MulSet(void* obje, SerializeBase* base);";*/
			classes += "\nstd::string " +ttapp+ "serialize"+classStructure.getTreatedClassName(true)+"ContainerSV(void* obje, SerializeBase* base, const std::string& _cont);\n";
		}

		typedefs += "\n}\nreturn __obj;\n}\n";
		typedefs += "\nvoid* " +ttapp+ "unSerialize"+classStructure.getTreatedClassName(true)+"Container(void* cintobject, SerializeBase* base, const std::string& _cont){\n"
			 +"if(!base->isValidClassNamespace(cintobject, _cont+\"-"
			 +classStructure.getTreatedClassName(false)+"\", \""+classStructure.nmSpc+"\", true))\n\treturn NULL;\nvoid* t = SerializeBase::getNewNestedContainer<"+classStructure.getFullyQualifiedClassName()
			 +">(_cont);\n"
			 +"int totsize = base->getContainerSize(cintobject);\nfor(int i=0;i<totsize;i++)\n{\n"
			 +"void* ciintobject = base->getContainerElement(cintobject, i);\n"
			 +"if(ciintobject!=NULL)\n{\n"+classStructure.getFullyQualifiedClassName()+"* __val__ = ("
			 +classStructure.getFullyQualifiedClassName()+"*)"+ttapp+"unSerialize"+classStructure.getTreatedClassName(true)
			 +"(ciintobject, base);\nSerializeBase::addValueToNestedContainer<"+classStructure.getFullyQualifiedClassName()
			 +">(_cont,*__val__,t);delete __val__;\n}\n}\nreturn t;\n}\n";
		classes += "\nvoid* " +ttapp+ "unSerialize"+classStructure.getTreatedClassName(true)+"Container(void* cintobject, SerializeBase* base, const std::string& _cont);\n";
		if(classStructure.prosetser)
		{
			typedefs += "\nvoid* " +ttapp+ "unSerialize"+classStructure.getTreatedClassName(true)+"ContainerSV(void* cintobject, SerializeBase* base, const std::string& _cont){\n"
					 +"if(!base->isValidClassNamespace(cintobject, _cont+\"-"
					 +classStructure.getTreatedClassName(false)+"\", \""+classStructure.nmSpc+"\", true))\n\treturn NULL;\nvoid* t = SerializeBase::getNewNestedContainerSV<"+classStructure.getFullyQualifiedClassName()
					 +">(_cont);\n"
					 +"int totsize = base->getContainerSize(cintobject);\nfor(int i=0;i<totsize;i++)\n{\n"
					 +"void* ciintobject = base->getContainerElement(cintobject, i);\n"
					 +"if(ciintobject!=NULL)\n{\n"+classStructure.getFullyQualifiedClassName()+"* __val__ = ("
					 +classStructure.getFullyQualifiedClassName()+"*)"+ttapp+"unSerialize"+classStructure.getTreatedClassName(true)
					 +"(ciintobject, base);\nSerializeBase::addValueToNestedContainerSV<"+classStructure.getFullyQualifiedClassName()
					 +">(_cont,*__val__,t);delete __val__;\n}\n}\nreturn t;\n}\n";
			classes += "\nvoid* " +ttapp+ "unSerialize"+classStructure.getTreatedClassName(true)+"ContainerSV(void* cintobject, SerializeBase* base, const std::string& _cont);\n";
		}
	}
	return refDef;
}


std::string Reflection::generateSerDefinition(std::map<std::string, ClassStructure>& allclsmap, std::string &includesDefs, std::string &typedefs, std::string &classes, std::string &methods, const std::string& app)
{
	return "";
}

std::string Reflection::generateSerDefinitionBinary(std::map<std::string, ClassStructure>& allclsmap, std::string &includesDefs, std::string &typedefs, std::string &classes, std::string &methods, const std::string& app)
{
	return "";
}

std::string Reflection::getXSDDefinitions(std::map<std::string, ClassStructure>& allclsmap, const std::string& fqcn, Reflection& ref, const std::string& appname, std::string &trgnmspc, std::set<std::string> &allnmspcs, const std::string& dfnmspc, const std::string& resp)
{
	std::string int_obj_binding, obj_binding, tmpnmspc;
	ClassStructure *clstruct = NULL;
	std::map<std::string, ClassStructure>::iterator it;
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
		//logger << ("Error generating web-service xsd definitions, as class "+fqcn + " not found...") << std::endl;
		return "";
	}

	std::string nmspcid = Reflection::getNameSpaceId(clstruct->nmSpc, appname);
	if(nmspcid=="")
		nmspcid = "tns";
	trgnmspc = nmspcid;
	allnmspcs.insert(nmspcid);
	//obj_binding.append("<xsd:complexType name=\""+nmspcid+":"+clstruct->getTreatedClassName(false)+"\">\n");
	//obj_binding.append("<xsd:sequence>\n");
	std::vector<std::string> fldnames;
	std::string fld;
	size_t tes;
	bool  ptr = false;
	std::map<std::string, bool> fldptr;
	std::map<std::string, int> fldstat;
	if (clstruct->pri.size() > 0)
	{
		for (unsigned int i = 0; i < clstruct->pri.size(); i++)
		{
			if(((tes=clstruct->pri.at(i).find("("))==std::string::npos && (tes=clstruct->pri.at(i).find(")"))==std::string::npos && clstruct->pri.at(i).find("~")==std::string::npos))
			{
				fld = clstruct->pri.at(i);
				StringUtil::replaceFirst(fld,";","");
				std::vector<std::string> fldp;
				StringUtil::split(fldp, fld, (" "));
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
	}
	if (clstruct->pro.size() > 0)
	{
		for (unsigned int i = 0; i < clstruct->pro.size(); i++)
		{
			if(((tes=clstruct->pro.at(i).find("("))==std::string::npos && (tes=clstruct->pro.at(i).find(")"))==std::string::npos && clstruct->pro.at(i).find("~")==std::string::npos))
			{
				fld = clstruct->pro.at(i);
				StringUtil::replaceFirst(fld,";","");
				std::vector<std::string> fldp;
				StringUtil::split(fldp, fld, (" "));
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
	}
	for(unsigned int i=0;i<clstruct->pub.size();i++)
	{
		if(((tes=clstruct->pub.at(i).find("("))==std::string::npos && (tes=clstruct->pub.at(i).find(")"))==std::string::npos && clstruct->pub.at(i).find("~")==std::string::npos))
		{
			fld = clstruct->pub.at(i);

			//bool fldstatic = false;
			if(RegexUtil::find(fld, "[ \t]*static[ \t]+")!=-1 || RegexUtil::find(fld, "[ \t]+static[ \t]+")!=-1)
			{
				RegexUtil::replace(fld, "[ \t]*static[ \t]+", " ");
				RegexUtil::replace(fld, "[ \t]+static[ \t]+", " ");
				//fldstatic = true;
			}
			RegexUtil::replace(fld, "[ \t]*const[ \t]+", " ");
			RegexUtil::replace(fld, "[ \t]+const[ \t]+", " ");

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
				else if(fldp.at(0).find("vector")!=std::string::npos || fldp.at(0).find("queue")!=std::string::npos || fldp.at(0).find("deque")!=std::string::npos || fldp.at(0).find("set")!=std::string::npos || fldp.at(0).find("list")!=std::string::npos)
				{
					std::string nam = fldp.at(1);
					std::string stlcnt = fldp.at(0);
					std::string stltyp = fldp.at(0);
					std::string contType;
					StringUtil::replaceFirst(stltyp,"std::","");
					StringUtil::replaceFirst(stltyp,"<","::");
					StringUtil::replaceFirst(stltyp,">","");
					StringUtil::replaceFirst(stltyp," ","");
					std::string stlcnttyp = "";
					if(fldp.at(0).find("vector")!=std::string::npos)
					{
						contType = "std::vector<";
						stlcnttyp = "Vec";
					}
					else if(fldp.at(0).find("queue")!=std::string::npos)
					{
						contType = "std::queue<";
						stlcnttyp = "Q";
					}
					else if(fldp.at(0).find("deque")!=std::string::npos)
					{
						contType = "std::deque<";
						stlcnttyp = "Dq";
					}
					else if(fldp.at(0).find("list")!=std::string::npos)
					{
						contType = "std::list<";
						stlcnttyp = "Lis";
					}
					else if(fldp.at(0).find("multiset")!=std::string::npos)
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

					std::string fqcn = ref.getFullyQualifiedClassName(stlcnt, clstruct->getNamespaces());
					contType += getFullyQualifiedClassName(stlcnt, clstruct->getNamespaces()) + ",";

					if(!ptr)
					{
						std::string namespc;
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
							std::string xstyp = fqcn;
							StringUtil::replaceAll(xstyp, "::", "_");
							obj_binding.append("<xsd:complexType name=\"ArrayOf"+clstruct->getTreatedClassName(true)+fqcn+"\"> <xsd:sequence>\n");
							obj_binding.append("<xsd:element maxOccurs=\"unbounded\" minOccurs=\"0\" name=\""+nam+"\" nillable=\"true\" type=\"xsd:"+xstyp+"\" />\n");
							obj_binding.append("</xsd:sequence> </xsd:complexType>\n");
						}
						else
						{
							std::string clnms = fqcn;
							if(fqcn.find("::")!=std::string::npos)
							{
								clnms = fqcn.substr(0, fqcn.find_last_of("::")+2);
								namespc = Reflection::getNameSpaceId(clnms, appname);
							}
							else
							{
								namespc = "tns";
							}
							std::string xstyp = fqcn;
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
					std::string namespc;
					std::string nam = fldp.at(1);
					std::string fqcn = ref.getFullyQualifiedClassName(fldp.at(0), clstruct->getNamespaces());
					if(!ptr)
					{
						std::string clnms = fqcn;
						if(fqcn.find("::")!=std::string::npos)
						{
							clnms = fqcn.substr(0, fqcn.find_last_of("::")+2);
							namespc = Reflection::getNameSpaceId(clnms, appname);
						}
						else
						{
							namespc = "tns";
						}
						std::string xstyp = fqcn;
						StringUtil::replaceAll(xstyp, "::", "_");
						obj_binding.append("\n<xsd:element name=\""+nam+"\" type=\""+namespc+":"+xstyp+"\"/>\n");
						int_obj_binding.append(getXSDDefinitions(allclsmap, fqcn, ref, appname, tmpnmspc, allnmspcs, dfnmspc, resp));
					}
					else
					{}
				}
			}
		}
		if((tes=clstruct->pub.at(i).find("("))!=std::string::npos && (tes=clstruct->pub.at(i).find(")"))!=std::string::npos && clstruct->pub.at(i).find("~")==std::string::npos
				&& fldnames.size()>0)
		{
			std::string meth = clstruct->pub.at(i);
			StringUtil::replaceFirst(meth,";","");

			std::string argts = meth.substr(meth.find("("),meth.find(")")-meth.find("("));
			StringUtil::replaceFirst(argts,"(","");
			StringUtil::replaceAll(argts,")","");
			meth = meth.substr(0,meth.find("("));
			StringUtil::trim(meth);
			RegexUtil::replace(meth, "[ \t]*,[ \t]*", ",");

			//bool methstat = false;
			if(RegexUtil::find(meth, "[ \t]*static[ \t]+")!=-1 || RegexUtil::find(meth, "[ \t]+static[ \t]+")!=-1)
			{
				RegexUtil::replace(meth, "[ \t]*static[ \t]+", " ");
				RegexUtil::replace(meth, "[ \t]+static[ \t]+", " ");
				//methstat = true;
			}
			RegexUtil::replace(meth, "[ \t]*inline[ \t]+", " ");
			RegexUtil::replace(meth, "[ \t]+inline[ \t]+", " ");
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
			{

			}
			else
			{
				if(methpm.at(0)!=clstruct->getTreatedClassName(false))
				{
					for(unsigned int k = 0; k < fldnames.size(); k=k+2)
					{
						std::string cam = StringUtil::capitalizedCopy(fldnames.at(k+1));
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
						std::string nam = fldnames.at(k+1);
						std::string cam = StringUtil::capitalizedCopy(fldnames.at(k+1));
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
							else if(argpm.at(0).find("vector")!=std::string::npos || argpm.at(0).find("queue")!=std::string::npos ||
									argpm.at(0).find("deque")!=std::string::npos || argpm.at(0).find("set")!=std::string::npos ||
									argpm.at(0).find("list")!=std::string::npos || argpm.at(0).find("multiset")!=std::string::npos)
							{
								std::string stlcnt = argpm.at(0);
								std::string stltyp = argpm.at(0);
								std::string contType;
								StringUtil::replaceFirst(stltyp,"std::","");
								StringUtil::replaceFirst(stltyp,"<","::");
								StringUtil::replaceFirst(stltyp,">","");
								StringUtil::replaceFirst(stltyp," ","");
								std::string stlcnttyp = "";
								if(argpm.at(0).find("vector")!=std::string::npos)
								{
									contType = "std::vector<";
									stlcnttyp = "Vec";
								}
								else if(argpm.at(0).find("queue")!=std::string::npos)
								{
									contType = "std::queue<";
									stlcnttyp = "Q";
								}
								else if(argpm.at(0).find("deque")!=std::string::npos)
								{
									contType = "std::deque<";
									stlcnttyp = "Dq";
								}
								else if(argpm.at(0).find("list")!=std::string::npos)
								{
									contType = "std::list<";
									stlcnttyp = "Lis";
								}
								else if(argpm.at(0).find("multiset")!=std::string::npos)
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

								std::string fqcn = ref.getFullyQualifiedClassName(stlcnt, clstruct->getNamespaces());
								contType += getFullyQualifiedClassName(stlcnt, clstruct->getNamespaces()) + ",";

								std::string namespc;
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
										std::string xstyp = fqcn;
										StringUtil::replaceAll(xstyp, "::", "_");
										obj_binding.append("<xsd:complexType name=\"ArrayOf"+clstruct->getTreatedClassName(true)+fqcn+"\"> <xsd:sequence>\n");
										obj_binding.append("<xsd:element maxOccurs=\"unbounded\" minOccurs=\"0\" name=\""+xstyp+"\" nillable=\"true\" type=\"xsd:"+fqcn+"\" />\n");
										obj_binding.append("</xsd:sequence> </xsd:complexType>\n");
									}
									else
									{
										std::string clnms = fqcn;
										if(fqcn.find("::")!=std::string::npos)
										{
											clnms = fqcn.substr(0, fqcn.find_last_of("::")+2);
											namespc = Reflection::getNameSpaceId(clnms, appname);
										}
										else
										{
											namespc = "tns";
										}
										std::string xstyp = fqcn;
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
								std::string namespc;
								std::string fqcn = ref.getFullyQualifiedClassName(argpm.at(0), clstruct->getNamespaces());
								if(!ptr)
								{
									std::string clnms = fqcn;
									if(fqcn.find("::")!=std::string::npos)
									{
										clnms = fqcn.substr(0, fqcn.find_last_of("::")+2);
										namespc = Reflection::getNameSpaceId(clnms, appname);
									}
									else
									{
										namespc = "tns";
									}
									std::string xstyp = fqcn;
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
	std::cout << "nmspcid = " + nmspcid << std::endl;
	std::cout << "dfnmspc = " + dfnmspc << std::endl;
	std::string nmspcidval = Reflection::getNameSpaceIdValue(nmspcid);
	if(nmspcidval=="")
		nmspcidval = dfnmspc;
	std::cout << "nmspcidval = " + nmspcidval << std::endl;
	cntxt["WS_NMSPC"] = nmspcidval;
	cntxt["OBJ"] = clstruct->getTreatedClassName(false);
	cntxt["OBJ_MEMBERS"] = obj_binding;
	obj_binding = TemplateEngine::evaluate(resp+"templateObjBin.wsdl",cntxt);
	return int_obj_binding + obj_binding;
}



bool Reflection::isPrimitiveDataType(std::string type)
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

std::string Reflection::getTypeName(std::string type)
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
