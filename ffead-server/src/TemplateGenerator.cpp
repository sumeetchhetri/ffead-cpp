/*
 * TemplateGenerator.cpp
 *
 *  Created on: 15-Feb-2013
 *      Author: sumeetc
 */

#include "TemplateGenerator.h"

TemplateGenerator::TemplateGenerator() {
	// TODO Auto-generated constructor stub

}

TemplateGenerator::~TemplateGenerator() {
	// TODO Auto-generated destructor stub
}

string TemplateGenerator::generateTempCd(string fileName,string &headersb,string &funcdefs,string app)
{
	ifstream infile;
	string data;
	vector<string> allcontent;
	infile.open(fileName.c_str());
	string file,dir;
	int s,en;
	s = fileName.find_last_of("/")+1;
	dir = fileName.substr(0,s-1);
	en = fileName.find_last_of(".");
	file = fileName.substr(s,en-s);
	if(infile)
	{
		while(getline(infile, data))
		{
			allcontent.push_back(data+"\n");
		}
	}
	string header,bodies,funcs,declars;
	funcdefs.append("string _"+app+file+"emittTemplateHTML(map<string, void*> args);\n");
	declars.append("string _"+app+file+"emittTemplateHTML(map<string, void*> args)\n{\nstring screen;\n");
	string tempo;
	map<string,string> uselocVars;
	vector<string> inplaceVarValues;
	bool startedFor = false, startedIf = false;
	for (int var = 0; var < (int)allcontent.size(); ++var) {
		string temp = allcontent.at(var);
		StringUtil::trim(temp);
		StringUtil::replaceAll(temp,"\"","\\\"");
		if(temp.find("#declare")==0 && temp.at(temp.length()-1)=='#')
		{
			StringUtil::replaceAll(temp,"#declare ","");
			StringUtil::replaceAll(temp,"#","");
			vector<string> tvec;
			StringUtil::split(tvec, temp, (" "));
			if(tvec.size()==2)
			{
				declars.append(temp+";\n");
				declars.append("if(args[\""+tvec.at(1)+"\"]!=NULL)\n{\n");
				inplaceVarValues.push_back(tvec.at(1));
				if(tvec.at(0).find("*")==string::npos)
				{
					declars.append(tvec.at(1)+" = *("+tvec.at(0)+"*)args[\""+tvec.at(1)+"\"];\n");
				}
				else
				{
					declars.append(tvec.at(1)+" = ("+tvec.at(0)+"*)args[\""+tvec.at(1)+"\"];\n");
				}
				declars.append("}\n");
			}
		}
		else if(temp.find("#for(")==0 && temp.at(temp.length()-1)=='#')
		{
			StringUtil::replaceAll(temp,"#","");
			tempo.append(temp + "\n{\n");
			startedFor = true;
		}
		else if(temp.find("#rof")==0 && temp.at(temp.length()-1)=='#')
		{
			startedFor = false;
			tempo.append("}\n");
		}
		else if(temp.find("#if(")==0 && temp.at(temp.length()-1)=='#')
		{
			StringUtil::replaceAll(temp,"#","");
			tempo.append(temp + "\n{\n");
			startedIf = true;
		}
		else if(temp.find("#elseif(")==0 && temp.at(temp.length()-1)=='#')
		{
			StringUtil::replaceAll(temp,"#","");
			tempo.append("}\n" + temp + "\n{\n");
			startedIf = true;
		}
		else if(temp.find("#else")==0 && temp.at(temp.length()-1)=='#')
		{
			StringUtil::replaceAll(temp,"#","");
			tempo.append("}\n" + temp + "\n{\n");
			startedIf = true;
		}
		else if(temp.find("#rof")==0 && temp.at(temp.length()-1)=='#')
		{
			startedFor = false;
			tempo.append("}\n");
		}
		else if(temp.find("#fi")==0 && temp.at(temp.length()-1)=='#')
		{
			startedIf = false;
			tempo.append("}\n");
		}
		else
		{
			for (int var1 = 0; var1 < (int)inplaceVarValues.size(); ++var1)
			{
				string rep = "${"+inplaceVarValues.at(var1)+"}";
				string strrep = "$_S{"+inplaceVarValues.at(var1)+"}";
				if(temp.find(rep)!=string::npos)
				{
					string repVal = inplaceVarValues.at(var1);
					StringUtil::replaceAll(temp,rep,"\" + " + repVal + " + \"");
				}
				else if(temp.find(strrep)!=string::npos)
				{
					string repVal = inplaceVarValues.at(var1);
					StringUtil::replaceAll(temp,strrep,"\" + CastUtil::lexical_cast<string>(" + repVal + ") + \"");
				}
				else if(temp.find("$_S{")!=string::npos && temp.find("}")!=string::npos
							&& temp.find("$_S{"+inplaceVarValues.at(var1))!=string::npos)
				{
					while(temp.find("$_S{")!=string::npos && temp.find("}")!=string::npos
							&& temp.find("$_S{"+inplaceVarValues.at(var1))!=string::npos)
					{
						string reps = temp.substr(temp.find("$_S{")+4, temp.find("}")-(temp.find("$_S{")+4));
						string oreps = temp.substr(temp.find("$_S{"), temp.find("}")+1-(temp.find("$_S{")));
						if(reps.find(".")!=string::npos)
						{
							string repst = reps.substr(0, reps.find("."));
							if(repst==inplaceVarValues.at(var1))
							{
								StringUtil::replaceAll(temp, oreps, "\" + CastUtil::lexical_cast<string>(" + reps + ") + \"");
							}
						}
					}
				}
				else
				{
					while(temp.find("${")!=string::npos && temp.find("}")!=string::npos
							&& temp.find("${"+inplaceVarValues.at(var1))!=string::npos)
					{
						string reps = temp.substr(temp.find("${")+2, temp.find("}")-(temp.find("${")+2));
						string oreps = temp.substr(temp.find("${"), temp.find("}")+1-(temp.find("${")));
						if(reps.find(".")!=string::npos)
						{
							string repst = reps.substr(0, reps.find("."));
							if(repst==inplaceVarValues.at(var1))
							{
								StringUtil::replaceAll(temp, oreps, "\" + " + reps + " + \"");
							}
						}
					}
				}
			}
			while(temp.find("$_S{")!=string::npos && temp.find("}")!=string::npos)
			{
				string reps = temp.substr(temp.find("$_S{")+4, temp.find("}")-(temp.find("$_S{")+4));
				string oreps = temp.substr(temp.find("$_S{"), temp.find("}")+1-(temp.find("$_S{")));
				//if(reps.find(".")!=string::npos)
				{
					//string repst = reps.substr(0, reps.find("."));
					StringUtil::replaceAll(temp, oreps, "\" + CastUtil::lexical_cast<string>(" + reps + ") + \"");
				}
			}
			while(temp.find("${")!=string::npos && temp.find("}")!=string::npos)
			{
				string reps = temp.substr(temp.find("${")+2, temp.find("}")-(temp.find("${")+2));
				string oreps = temp.substr(temp.find("${"), temp.find("}")+1-(temp.find("${")));
				//if(reps.find(".")!=string::npos)
				{
					//string repst = reps.substr(0, reps.find("."));
					StringUtil::replaceAll(temp, oreps, "\" + " + reps + " + \"");
				}
			}
			tempo.append("screen += \"" + temp + "\";\n");
		}
	}
	bodies.append(tempo);
	bodies.append("\nreturn screen;\n");
	bodies.append("}\n");
	declars.append(bodies);
	return declars;
}

string TemplateGenerator::generateTempCdAll(map<string, string> fileNames)
{
	string bodies,headersb="#include \"AfcInclude.h\"",funcdefs;
	map<string, string>::iterator it;
	for (it=fileNames.begin();it!=fileNames.end();++it)
	{
		bodies += generateTempCd(it->first,headersb,funcdefs,it->second);
	}
	bodies = (headersb+"\nextern \"C\"\n{\n"+funcdefs+bodies+"}\n");
	return bodies;
}


