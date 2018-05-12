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

std::string TemplateGenerator::generateTempCd(const std::string& fileName, std::string &headersb, std::string &funcdefs, const std::string& app)
{
	std::ifstream infile;
	std::string data;
	std::vector<std::string> allcontent;
	infile.open(fileName.c_str());
	std::string file,dir;
	/*int s,en;
	s = fileName.find_last_of("/")+1;
	dir = fileName.substr(0,s-1);
	en = fileName.find_last_of(".");
	file = fileName.substr(s,en-s);*/
	file = fileName;
	RegexUtil::replace(file,"[/]+","/");
	StringUtil::replaceFirst(file, ConfigurationData::getInstance()->coreServerProperties.webPath, "");
	RegexUtil::replace(file, "[^a-zA-Z0-9_]+", "");
	if(infile)
	{
		while(getline(infile, data))
		{
			allcontent.push_back(data+"\n");
		}
	}
	std::string header,bodies,funcs,declars;
	funcdefs.append("std::string _"+file+"emittTemplateHTML(std::map<std::string, GenericObject>* args);\n");
	declars.append("std::string _"+file+"emittTemplateHTML(std::map<std::string, GenericObject>* args)\n{\nstd::string screen;\n");
	std::string tempo;
	std::map<std::string,std::string> uselocVars;
	std::vector<std::string> inplaceVarValues;
	//bool startedFor = false, startedIf = false, startedWhile = false;
	for (int var = 0; var < (int)allcontent.size(); ++var) {
		std::string temp = allcontent.at(var);
		StringUtil::trim(temp);
		StringUtil::replaceAll(temp,"\"","\\\"");
		if(temp.find("#declare")==0 && temp.at(temp.length()-1)=='#')
		{
			StringUtil::replaceAll(temp,"#declare ","");
			StringUtil::replaceAll(temp,"#","");
			std::vector<std::string> tvec;
			StringUtil::split(tvec, temp, (" "));
			if(tvec.size()==2)
			{
				declars.append(temp+";\n");
				declars.append("if(args->find(\""+tvec.at(1)+"\")!=args->end())\n{\n");
				inplaceVarValues.push_back(tvec.at(1));
				/*if(tvec.at(0).find("*")==std::string::npos)
				{
					declars.append(tvec.at(1)+" = *("+tvec.at(0)+"*)args[\""+tvec.at(1)+"\"];\n");
				}
				else
				{
					declars.append(tvec.at(1)+" = ("+tvec.at(0)+"*)args[\""+tvec.at(1)+"\"];\n");
				}*/
				declars.append("args->find(\""+tvec.at(1)+"\")->second.get("+tvec.at(1)+");\n");
				declars.append("}\n");
			}
		}
		else if(temp.find("#define")==0 && temp.at(temp.length()-1)=='#')
		{
			StringUtil::replaceAll(temp,"#","");
			tempo.append(temp + "\n");
		}
		else if(temp.find("#for(")==0 && temp.at(temp.length()-1)=='#')
		{
			StringUtil::replaceAll(temp,"#","");
			tempo.append(temp + "\n{\n");
			//startedFor = true;
		}
		else if(temp.find("#while(")==0 && temp.at(temp.length()-1)=='#')
		{
			StringUtil::replaceAll(temp,"#","");
			tempo.append(temp + "\n{\n");
			//startedWhile = true;
		}
		else if(temp.find("#if(")==0 && temp.at(temp.length()-1)=='#')
		{
			StringUtil::replaceAll(temp,"#","");
			tempo.append(temp + "\n{\n");
			//startedIf = true;
		}
		else if(temp.find("#elseif(")==0 && temp.at(temp.length()-1)=='#')
		{
			StringUtil::replaceAll(temp,"#","");
			tempo.append("}\n" + temp + "\n{\n");
			//startedIf = true;
		}
		else if(temp.find("#else")==0 && temp.at(temp.length()-1)=='#')
		{
			StringUtil::replaceAll(temp,"#","");
			tempo.append("}\n" + temp + "\n{\n");
			//startedIf = true;
		}
		else if(temp.find("#rof")==0 && temp.at(temp.length()-1)=='#')
		{
			//startedFor = false;
			tempo.append("}\n");
		}
		else if(temp.find("#fi")==0 && temp.at(temp.length()-1)=='#')
		{
			//startedIf = false;
			tempo.append("}\n");
		}
		else if(temp.find("#elihw")==0 && temp.at(temp.length()-1)=='#')
		{
			//startedWhile = false;
			tempo.append("}\n");
		}
		else
		{
			for (int var1 = 0; var1 < (int)inplaceVarValues.size(); ++var1)
			{
				std::string rep = "${"+inplaceVarValues.at(var1)+"}";
				std::string strrep = "$_S{"+inplaceVarValues.at(var1)+"}";
				if(temp.find(rep)!=std::string::npos)
				{
					std::string repVal = inplaceVarValues.at(var1);
					StringUtil::replaceAll(temp,rep,"\" + " + repVal + " + \"");
				}
				else if(temp.find(strrep)!=std::string::npos)
				{
					std::string repVal = inplaceVarValues.at(var1);
					StringUtil::replaceAll(temp,strrep,"\" + CastUtil::lexical_cast<std::string>(" + repVal + ") + \"");
				}
				else if(temp.find("$_S{")!=std::string::npos && temp.find("}")!=std::string::npos
							&& temp.find("$_S{"+inplaceVarValues.at(var1))!=std::string::npos)
				{
					while(temp.find("$_S{")!=std::string::npos && temp.find("}")!=std::string::npos
							&& temp.find("$_S{"+inplaceVarValues.at(var1))!=std::string::npos)
					{
						std::string reps = temp.substr(temp.find("$_S{")+4, temp.find("}")-(temp.find("$_S{")+4));
						std::string oreps = temp.substr(temp.find("$_S{"), temp.find("}")+1-(temp.find("$_S{")));
						if(reps.find(".")!=std::string::npos)
						{
							std::string repst = reps.substr(0, reps.find("."));
							if(repst==inplaceVarValues.at(var1))
							{
								StringUtil::replaceAll(temp, oreps, "\" + CastUtil::lexical_cast<std::string>(" + reps + ") + \"");
							}
						}
					}
				}
				else
				{
					while(temp.find("${")!=std::string::npos && temp.find("}")!=std::string::npos
							&& temp.find("${"+inplaceVarValues.at(var1))!=std::string::npos)
					{
						std::string reps = temp.substr(temp.find("${")+2, temp.find("}")-(temp.find("${")+2));
						std::string oreps = temp.substr(temp.find("${"), temp.find("}")+1-(temp.find("${")));
						if(reps.find(".")!=std::string::npos)
						{
							std::string repst = reps.substr(0, reps.find("."));
							if(repst==inplaceVarValues.at(var1))
							{
								StringUtil::replaceAll(temp, oreps, "\" + " + reps + " + \"");
							}
						}
					}
				}
			}
			while(temp.find("$_S{")!=std::string::npos && temp.find("}")!=std::string::npos)
			{
				std::string reps = temp.substr(temp.find("$_S{")+4, temp.find("}")-(temp.find("$_S{")+4));
				std::string oreps = temp.substr(temp.find("$_S{"), temp.find("}")+1-(temp.find("$_S{")));
				//if(reps.find(".")!=std::string::npos)
				{
					//string repst = reps.substr(0, reps.find("."));
					StringUtil::replaceAll(temp, oreps, "\" + CastUtil::lexical_cast<std::string>(" + reps + ") + \"");
				}
			}
			while(temp.find("${")!=std::string::npos && temp.find("}")!=std::string::npos)
			{
				std::string reps = temp.substr(temp.find("${")+2, temp.find("}")-(temp.find("${")+2));
				std::string oreps = temp.substr(temp.find("${"), temp.find("}")+1-(temp.find("${")));
				//if(reps.find(".")!=std::string::npos)
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

std::string TemplateGenerator::generateTempCdAll(const std::string& serverRootDirectory)
{
	std::map<std::string, std::string> templateMappingMap = ConfigurationData::getInstance()->templateFilesMap;
	std::string bodies,headersb="#include \"AfcInclude.h\"",funcdefs;
	std::map<std::string, std::string>::iterator msssit;
	for (msssit=templateMappingMap.begin();msssit!=templateMappingMap.end();++msssit)
	{
		bodies += generateTempCd(msssit->first,headersb,funcdefs,msssit->second);
	}
	bodies = (headersb+"\nextern \"C\"\n{\n"+funcdefs+bodies+"}\n");
	return bodies;
}


