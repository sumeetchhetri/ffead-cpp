/*
	Copyright 2009-2020, Sumeet Chhetri

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
 * TemplateGenerator.cpp
 *
 *  Created on: 15-Feb-2013
 *      Author: sumeetc
 */

#include "TemplateGenerator.h"

TemplateGenerator::TemplateGenerator() {
}

TemplateGenerator::~TemplateGenerator() {
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
			allcontent.push_back(data);
		}
	}
	std::string header,bodies,funcs,declars;
	funcdefs.append("void _"+file+"emittTemplateHTML(std::map<std::string, void*>* _args_i__, fcpstream& _screen_i__);\n");
	declars.append("void _"+file+"emittTemplateHTML(std::map<std::string, void*>* _args_i__, fcpstream& _screen_i__)\n{\n");
	std::string tempo;
	std::string destruct;
	std::map<std::string,std::string> uselocVars;
	std::vector<std::string> inplaceVarValues;
	std::string notmpl;
	//bool startedFor = false, startedIf = false, startedWhile = false;
	for (int var = 0; var < (int)allcontent.size(); ++var) {
		std::string temp = allcontent.at(var);
		StringUtil::trim(temp);
		StringUtil::replaceAll(temp,"\"","\\\"");
		if(temp.find("#")!=0 && temp.find("${")==std::string::npos && temp.find("$_S{")==std::string::npos)
		{
			notmpl += temp;
			continue;
		}
		else
		{
			if(notmpl.length()>0) {
				tempo.append("_screen_i__ << \"" + notmpl + "\";\n");
				notmpl = "";
			}
		}

		if(temp.find("#declare")==0)
		{
			bool noDelete = temp.find("#declareref ")==0;
			if(noDelete) {
				StringUtil::replaceFirst(temp,"#declareref ","");
			} else {
				StringUtil::replaceFirst(temp,"#declare ","");
			}
			if(temp.at(temp.length()-1)=='#') {
				temp = temp.substr(0, temp.length()-1);
			}
			std::vector<std::string> tvec;
			StringUtil::split(tvec, temp, (" "));
			if(tvec.size()==2)
			{
				declars.append(temp+";\n");
				declars.append("if(_args_i__->find(\""+tvec.at(1)+"\")!=_args_i__->end())\n{\n");
				inplaceVarValues.push_back(tvec.at(1));
				/*if(tvec.at(0).find("*")==std::string::npos)
				{
					declars.append(tvec.at(1)+" = *("+tvec.at(0)+"*)_args_i__[\""+tvec.at(1)+"\"];\n");
				}
				else
				{
					declars.append(tvec.at(1)+" = ("+tvec.at(0)+"*)_args_i__[\""+tvec.at(1)+"\"];\n");
				}*/
				if(temp.find("*")==std::string::npos) {
					declars.append(tvec.at(1) + "= *(" + tvec.at(0) + "*)");
					if(!noDelete) {
						destruct.append("delete (" + tvec.at(0) + "*)_args_i__->find(\""+tvec.at(1)+"\")->second;\n");
					}
				} else {
					declars.append(tvec.at(1) + "= (" + tvec.at(0) + ")");
					if(!noDelete) {
						destruct.append("delete (" + tvec.at(0) + ")_args_i__->find(\""+tvec.at(1)+"\")->second;\n");
					}
				}
				declars.append("_args_i__->find(\""+tvec.at(1)+"\")->second;\n");
				declars.append("}\n");
			}
		}
		else if(temp.find("#define")==0)
		{
			StringUtil::replaceFirst(temp, "#define ","");
			if(temp.at(temp.length()-1)=='#') {
				temp = temp.substr(0, temp.length()-1);
			}
			tempo.append(temp + "\n");
		}
		else if(temp.find("#for(")==0)
		{
			StringUtil::replaceFirst(temp, "#for", "for");
			if(temp.at(temp.length()-1)=='#') {
				temp = temp.substr(0, temp.length()-1);
			}
			tempo.append(temp + "\n{\n");
			//startedFor = true;
		}
		else if(temp.find("#while(")==0)
		{
			StringUtil::replaceFirst(temp, "#while", "while");
			if(temp.at(temp.length()-1)=='#') {
				temp = temp.substr(0, temp.length()-1);
			}
			tempo.append(temp + "\n{\n");
			//startedWhile = true;
		}
		else if(temp.find("#if(")==0)
		{
			StringUtil::replaceFirst(temp, "#if", "if");
			if(temp.at(temp.length()-1)=='#') {
				temp = temp.substr(0, temp.length()-1);
			}
			tempo.append(temp + "\n{\n");
			//startedIf = true;
		}
		else if(temp.find("#elseif(")==0)
		{
			StringUtil::replaceFirst(temp, "elseif", "else if");
			if(temp.at(temp.length()-1)=='#') {
				temp = temp.substr(0, temp.length()-1);
			}
			tempo.append("}\n" + temp + "\n{\n");
			//startedIf = true;
		}
		else if(temp.find("#else")==0)
		{
			StringUtil::replaceFirst(temp, "#else", "else");
			if(temp.at(temp.length()-1)=='#') {
				temp = temp.substr(0, temp.length()-1);
			}
			tempo.append("}\n" + temp + "\n{\n");
			//startedIf = true;
		}
		else if(temp.find("#rof")==0)
		{
			//startedFor = false;
			tempo.append("}\n");
		}
		else if(temp.find("#fi")==0)
		{
			//startedIf = false;
			tempo.append("}\n");
		}
		else if(temp.find("#elihw")==0)
		{
			//startedWhile = false;
			tempo.append("}\n");
		}
		else if(temp.find("#end")==0)
		{
			//startedWhile = false;
			tempo.append("}\n");
		}
		else
		{
			/*for (int var1 = 0; var1 < (int)inplaceVarValues.size(); ++var1)
			{
				std::string rep = "${"+inplaceVarValues.at(var1)+"}";
				std::string strrep = "$_S{"+inplaceVarValues.at(var1)+"}";
				if(temp.find(rep)!=std::string::npos)
				{
					std::string repVal = inplaceVarValues.at(var1);
					StringUtil::replaceAll(temp, rep, "\" << " + repVal + " << \"");
				}
				else if(temp.find(strrep)!=std::string::npos)
				{
					std::string repVal = inplaceVarValues.at(var1);
					StringUtil::replaceAll(temp,strrep,"\" << std::to_string(" + repVal + ") << \"");
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
								StringUtil::replaceAll(temp, oreps, "\" << std::to_string(" + reps + ") << \"");
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
								StringUtil::replaceAll(temp, oreps, "\" << " + reps + " << \"");
							}
						}
					}
				}
			}*/
			while(temp.find("$_S{")!=std::string::npos && temp.find("}")!=std::string::npos)
			{
				std::string reps = temp.substr(temp.find("$_S{")+4, temp.find("}")-(temp.find("$_S{")+4));
				std::string oreps = temp.substr(temp.find("$_S{"), temp.find("}")+1-(temp.find("$_S{")));
				StringUtil::replaceAll(temp, oreps, "\" << std::to_string(" + reps + ") << \"");
			}
			while(temp.find("${")!=std::string::npos && temp.find("}")!=std::string::npos)
			{
				std::string reps = temp.substr(temp.find("${")+2, temp.find("}")-(temp.find("${")+2));
				std::string oreps = temp.substr(temp.find("${"), temp.find("}")+1-(temp.find("${")));
				StringUtil::replaceAll(temp, oreps, "\" << " + reps + " << \"");
			}
			tempo.append("_screen_i__ << \"" + temp + "\";\n");
		}
	}
	if(notmpl.length()>0) {
		tempo.append("_screen_i__ << \"" + notmpl + "\";\n");
	}
	bodies.append(tempo);
	bodies.append(destruct);
	bodies.append("}\n");
	declars.append(bodies);
	return declars;
}

std::string TemplateGenerator::generateTempCdFast(const std::string& fileName, std::string &headersb, std::string &funcdefs, const std::string& app)
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
			allcontent.push_back(data);
		}
	}
	std::string header,bodies,funcs,declars;
	funcdefs.append("void _"+file+"emittTemplateHTML_s(std::map<std::string, void*>* _args_i__, std::string& _screen_i__);\n");
	declars.append("void _"+file+"emittTemplateHTML_s(std::map<std::string, void*>* _args_i__, std::string& _screen_i__)\n{\n");
	std::string tempo;
	std::string destruct;
	std::map<std::string,std::string> uselocVars;
	std::vector<std::string> inplaceVarValues;
	std::string notmpl;
	//bool startedFor = false, startedIf = false, startedWhile = false;
	for (int var = 0; var < (int)allcontent.size(); ++var) {
		std::string temp = allcontent.at(var);
		StringUtil::trim(temp);
		StringUtil::replaceAll(temp,"\"","\\\"");
		if(temp.find("#")!=0 && temp.find("${")==std::string::npos && temp.find("$_S{")==std::string::npos)
		{
			notmpl += temp;
			continue;
		}
		else
		{
			if(notmpl.length()>0) {
				tempo.append("_screen_i__.append(\"" + notmpl + "\");\n");
				notmpl = "";
			}
		}

		if(temp.find("#declare")==0)
		{
			bool noDelete = temp.find("#declareref ")==0;
			if(noDelete) {
				StringUtil::replaceFirst(temp,"#declareref ","");
			} else {
				StringUtil::replaceFirst(temp,"#declare ","");
			}
			if(temp.at(temp.length()-1)=='#') {
				temp = temp.substr(0, temp.length()-1);
			}
			std::vector<std::string> tvec;
			StringUtil::split(tvec, temp, (" "));
			if(tvec.size()==2)
			{
				declars.append(temp+";\n");
				declars.append("if(_args_i__->find(\""+tvec.at(1)+"\")!=_args_i__->end())\n{\n");
				inplaceVarValues.push_back(tvec.at(1));
				/*if(tvec.at(0).find("*")==0)
				{
					declars.append(tvec.at(1)+" = *("+tvec.at(0)+"*)_args_i__[\""+tvec.at(1)+"\"];\n");
				}
				else
				{
					declars.append(tvec.at(1)+" = ("+tvec.at(0)+"*)_args_i__[\""+tvec.at(1)+"\"];\n");
				}*/
				if(temp.find("*")==std::string::npos) {
					declars.append(tvec.at(1) + "= *(" + tvec.at(0) + "*)");
					if(!noDelete) {
						destruct.append("delete (" + tvec.at(0) + "*)_args_i__->find(\""+tvec.at(1)+"\")->second;\n");
					}
				} else {
					declars.append(tvec.at(1) + "= (" + tvec.at(0) + ")");
					if(!noDelete) {
						destruct.append("delete (" + tvec.at(0) + ")_args_i__->find(\""+tvec.at(1)+"\")->second;\n");
					}
				}
				declars.append("_args_i__->find(\""+tvec.at(1)+"\")->second;\n");
				declars.append("}\n");
			}
		}
		else if(temp.find("#define")==0)
		{
			StringUtil::replaceFirst(temp, "#define ","");
			if(temp.at(temp.length()-1)=='#') {
				temp = temp.substr(0, temp.length()-1);
			}
			tempo.append(temp + "\n");
		}
		else if(temp.find("#for(")==0)
		{
			StringUtil::replaceFirst(temp, "#for", "for");
			if(temp.at(temp.length()-1)=='#') {
				temp = temp.substr(0, temp.length()-1);
			}
			tempo.append(temp + "\n{\n");
			//startedFor = true;
		}
		else if(temp.find("#while(")==0)
		{
			StringUtil::replaceFirst(temp, "#while", "while");
			if(temp.at(temp.length()-1)=='#') {
				temp = temp.substr(0, temp.length()-1);
			}
			tempo.append(temp + "\n{\n");
			//startedWhile = true;
		}
		else if(temp.find("#if(")==0)
		{
			StringUtil::replaceFirst(temp, "#if", "if");
			if(temp.at(temp.length()-1)=='#') {
				temp = temp.substr(0, temp.length()-1);
			}
			tempo.append(temp + "\n{\n");
			//startedIf = true;
		}
		else if(temp.find("#elseif(")==0)
		{
			StringUtil::replaceFirst(temp, "elseif", "else if");
			if(temp.at(temp.length()-1)=='#') {
				temp = temp.substr(0, temp.length()-1);
			}
			tempo.append("}\n" + temp + "\n{\n");
			//startedIf = true;
		}
		else if(temp.find("#else")==0)
		{
			StringUtil::replaceFirst(temp, "#else", "else");
			if(temp.at(temp.length()-1)=='#') {
				temp = temp.substr(0, temp.length()-1);
			}
			tempo.append("}\n" + temp + "\n{\n");
			//startedIf = true;
		}
		else if(temp.find("#rof")==0)
		{
			//startedFor = false;
			tempo.append("}\n");
		}
		else if(temp.find("#fi")==0)
		{
			//startedIf = false;
			tempo.append("}\n");
		}
		else if(temp.find("#elihw")==0)
		{
			//startedWhile = false;
			tempo.append("}\n");
		}
		else if(temp.find("#end")==0)
		{
			//startedWhile = false;
			tempo.append("}\n");
		}
		else
		{
			/*for (int var1 = 0; var1 < (int)inplaceVarValues.size(); ++var1)
			{
				std::string rep = "${"+inplaceVarValues.at(var1)+"}";
				std::string strrep = "$_S{"+inplaceVarValues.at(var1)+"}";
				if(temp.find(rep)!=std::string::npos)
				{
					std::string repVal = inplaceVarValues.at(var1);
					tempo.append("_screen_i__.append(\"" + notmpl + "\");\n");
					StringUtil::replaceAll(temp, rep, "\" << " + repVal + " << \"");
				}
				else if(temp.find(strrep)!=std::string::npos)
				{
					std::string repVal = inplaceVarValues.at(var1);
					StringUtil::replaceAll(temp,strrep,"\" << std::to_string(" + repVal + ") << \"");
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
								StringUtil::replaceAll(temp, oreps, "\" << std::to_string(" + reps + ") << \"");
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
								StringUtil::replaceAll(temp, oreps, "\" << " + reps + " << \"");
							}
						}
					}
				}
			}*/
			while(temp.find("$_S{")!=std::string::npos && temp.find("}")!=std::string::npos)
			{
				std::string reps = temp.substr(temp.find("$_S{")+4, temp.find("}")-(temp.find("$_S{")+4));
				//std::string oreps = temp.substr(temp.find("$_S{"), temp.find("}")+1-(temp.find("$_S{")));
				std::string sp = temp.substr(0, temp.find("$_S{"));
				temp = temp.substr(temp.find("}")+1);
				if(sp.length()>0) {
					tempo.append("_screen_i__.append(\"" + sp + "\");\n");
				}
				tempo.append("_screen_i__.append(std::to_string(" + reps + "));\n");
				//StringUtil::replaceAll(temp, oreps, "\" << std::to_string(" + reps + ") << \"");
			}
			while(temp.find("${")!=std::string::npos && temp.find("}")!=std::string::npos)
			{
				std::string reps = temp.substr(temp.find("${")+2, temp.find("}")-(temp.find("${")+2));
				std::string sp = temp.substr(0, temp.find("${"));
				//std::string oreps = temp.substr(temp.find("${"), temp.find("}")+1-(temp.find("${")));
				temp = temp.substr(temp.find("}")+1);
				if(sp.length()>0) {
					tempo.append("_screen_i__.append(\"" + sp + "\");\n");
				}
				tempo.append("_screen_i__.append(" + reps + ");\n");
				//StringUtil::replaceAll(temp, oreps, "\" << " + reps + " << \"");
			}
			tempo.append("_screen_i__.append(\"" + temp + "\");\n");
		}
	}
	if(notmpl.length()>0) {
		tempo.append("_screen_i__.append(\"" + notmpl + "\");\n");
	}
	bodies.append(tempo);
	bodies.append(destruct);
	bodies.append("}\n");
	declars.append(bodies);
	return declars;
}

std::string TemplateGenerator::generateTempCdAll(const std::string& serverRootDirectory)
{
	std::map<std::string, std::string, std::less<> > templateMappingMap = ConfigurationData::getInstance()->templateFilesMap;
	std::string bodies,headersb="#include \"AfcInclude.h\"",funcdefs;
	std::map<std::string, std::string, std::less<>>::iterator msssit;
	for (msssit=templateMappingMap.begin();msssit!=templateMappingMap.end();++msssit)
	{
		bodies += generateTempCd(msssit->first,headersb,funcdefs,msssit->second);
		bodies += generateTempCdFast(msssit->first,headersb,funcdefs,msssit->second);
	}
	bodies = (headersb+"\nextern \"C\"\n{\n"+funcdefs+bodies+"}\n");
	return bodies;
}
