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
 * WsUtil.cpp
 *
 *  Created on: Sep 21, 2009
 *      Author: sumeet
 */

#include "WsUtil.h"

WsUtil::WsUtil() {
	logger = LoggerFactory::getLogger("WsUtil");
}

WsUtil::~WsUtil() {
	// TODO Auto-generated destructor stub
}

std::string WsUtil::generateAllWSDL(const std::vector<WsDetails>& wsdvec, const std::string& resp, Reflection& ref, std::map<std::string, std::map<std::string, ClassStructure> >& clsstrucMaps)
{
	std::string ret, headers="#include \"AppDefines.h\"\n#include \"Exception.h\"\n#include \"string\"\n#include <sstream>\n#include \"CastUtil.h\"\n#include \"XmlParser.h\"\n#include \"Reflection.h\"\n#include \"XMLSerialize.h\"\n";
	for(unsigned int var = 0; var < wsdvec.size(); ++var)
	{
		handleWebService(ret, wsdvec.at(var), clsstrucMaps, resp, headers, ref);
	}
	return (headers + "\n\n" + "extern \"C\"\n{\nvoid normalizeNamespaces(Element* ele, std::string appname, std::string& allnmspcs) { 	ElementList elements = ele->getChildElements(); 	std::string nmspc = ele->getAttribute(\"namespace\"); 	if(nmspc!=\"\") 	{ 		std::string nmspcid = Reflection::getNameSpaceId(nmspc, appname); 		ele->setTagName(nmspcid+\":\"+ele->getTagName()); 		ele->removeAttribute(\"namespace\"); 		allnmspcs += (\" xmlns:\" + nmspcid + \"=\\\"\" + nmspc + \"\\\"\"); 	} 	for(unsigned int i=0;i<elements.size();i++) 	{ 		normalizeNamespaces((Element*)&(elements.at(i)), appname, allnmspcs);		} }" + ret + "\n}\n");
}

std::vector<WsDetails> WsUtil::getWsDetails(const std::vector<std::string>& apps, const std::string& serverRootDirectory)
{
	std::vector<WsDetails> wsdvec;

	for(unsigned int var = 0; var < apps.size(); ++var)
	{
		std::string webdir = serverRootDirectory+"/web/"+apps.at(var);
		std::string file = webdir+"/config/ws.xml";

		XmlParser parser("Parser");
		Document doc;
		parser.readDocument(file, doc);
		const Element& root = doc.getRootElement();
		typedef std::map<std::string,std::string> strMap;
		typedef std::map<std::string,strMap> meth_Map;
		if(root.getChildElements().size()==0)
			continue;

		std::string ws_funcs,obj_mapng,retObj_xml;
		for(unsigned int ii=0;ii<root.getChildElements().size();ii++)
		{
			std::map<std::string, std::string> outnmmp;
			Element* ws = (Element*)&(root.getChildElements().at(ii));
			for(unsigned int i=0;i<ws->getChildElements().size();i++)
			{
				outnmmp[ws->getChildElements().at(i).getTagName()] = ws->getChildElements().at(i).getAttribute("outname");
			}
			WsDetails wsd;
			wsd.claz = ws->getAttribute("class");
			wsd.location = ws->getAttribute("location");
			wsd.namespc = ws->getAttribute("namespace");
			wsd.outnmmp = outnmmp;
			wsd.appname = apps.at(var);
			wsdvec.push_back(wsd);
			ConfigurationData::getInstance()->webserviceDetailMap[wsd.appname].push_back(wsd);
		}
	}
	return wsdvec;
}

void WsUtil::handleWebService(std::string& ws_funcs, const WsDetails& wsd, std::map<std::string, std::map<std::string, ClassStructure> >& clsstrucMaps, const std::string& resp, std::string &headers, Reflection& ref)
{
	typedef std::map<std::string,std::string> strMap;
	typedef std::map<std::string,strMap> meth_Map;
	typedef std::map<std::string,meth_Map> ws_inp_out_Map;
	std::string claz = wsd.claz;
	std::string location = wsd.location;
	std::string namespc = wsd.namespc;
	std::map<std::string, std::string> outnmmp = wsd.outnmmp;
	std::string appname = wsd.appname;
	std::map<std::string, ClassStructure>& allclsmap = clsstrucMaps[appname];

	ws_inp_out_Map ws_info;
	meth_Map meth_info;
	StringContext gcntxt;
	std::string ws_name,reqr_res_bind,wsdl_msgs,wsdl_ops,wsdl_bind,wsdl,wsdl_obj_bind;

	ws_name = claz;
	std::string ows_name = ws_name;
	StringUtil::replaceAll(ws_name, "::", "_");
	if(StringUtil::trimCopy(ws_name)=="")
	{
		logger << ("No class name defined for web-service, skipping...") << std::endl;
		return;
	}
	if(StringUtil::trimCopy(location)=="")
	{
		logger << ("No location defined for web-service, skipping...") << std::endl;
		return;
	}
	gcntxt["WS_NAME"] = ws_name;
	gcntxt["WS_PATH"] = location;
	if(gcntxt["WS_PATH"].at(0)=='/')
	{
		gcntxt["WS_PATH"] = gcntxt["WS_PATH"].substr(1);
		StringUtil::replaceAll(gcntxt["WS_PATH"], "//", "/");
	}
	gcntxt["WS_NMSPC"] = StringUtil::trimCopy(namespc);

	logger << ("Web service " + ws_name + " found for appname " + appname) << std::endl;

	ClassStructure *clstruct = NULL;
	std::map<std::string, ClassStructure>::iterator it;
	for (it=allclsmap.begin();it!=allclsmap.end();++it)
	{
		if(it->second.getFullyQualifiedClassName()==claz)
		{
			clstruct = &it->second;
			break;
		}
	}
	if(clstruct==NULL)
	{
		logger << ("Error generating web-service artifacts, as class "+claz + "not found...") << std::endl;
		return;
	}

	if(clstruct->getNamespace()=="" && gcntxt["WS_NMSPC"]=="")
	{
		logger << ("No namespace defined for web-service, skipping...") << std::endl;
		return;
	}
	if(gcntxt["WS_NMSPC"]=="")
	{
		gcntxt["WS_NMSPC"] = clstruct->getNamespace();
	}
	StringUtil::replaceAll(gcntxt["WS_NMSPC"], "::", ".");

	std::vector<std::string> pinfo;
	bool isOpForSet = false;
	strVec info = ref.getAfcObjectData(*clstruct,false,pinfo,isOpForSet);
	headers.append("#include \""+ref.getClassPath(clstruct->getTreatedClassName(true))+"\"\n");

	std::set<std::string> allnmspcs;
	std::map<std::string, std::string> trgnms;
	for(unsigned int i=0;i<info.size();i++)
	{
		std::string temp,temp11;
		strMap in_out_info;
		temp = info.at(i);
		temp11 = temp.substr(temp.find("(")+1);
		temp = temp.substr(0,temp.find("("));
		temp11 = temp11.substr(0, temp11.find(")"));

		//StringUtil::replaceFirst(temp,"("," ");
		StringUtil::replaceAll(temp11,";","");
		StringUtil::replaceFirst(temp11,")","");
		strVec results,results1;
		StringUtil::split(results, temp, (" "));
		RegexUtil::replace(temp11, "[ \\t]*,[ \\t]", ",");
		StringUtil::split(results1, temp11, (","));

		std::string retType,methName,inp_params;
		if(results.size()<2)
			continue;
		if(results.size()>0 && (results.at(0)==it->second.getTreatedClassName(false) || temp.find("~")!=std::string::npos))
			continue;

		methName = results.at(1);

		in_out_info["RETURN"] = outnmmp[methName];
		if(in_out_info["RETURN"]=="")
			in_out_info["RETURN"] = "result";
		//logger << in_out_info["RETURN"] << std::flush;
		in_out_info["RETURNTYP"] = results.at(0);
		std::string xsdobjdef;
		if(results.at(0).find("vector<")!=std::string::npos)
		{
			std::string vecn = results.at(0);
			StringUtil::replaceFirst(vecn,"std::"," ");
			StringUtil::replaceFirst(vecn,"vector<"," ");
			StringUtil::replaceFirst(vecn,">"," ");
			StringUtil::trim(vecn);
			if(vecn=="int" || vecn=="string" || vecn=="short" || vecn=="float" || vecn=="double" || vecn=="bool"
					|| vecn=="long long" || vecn=="long"
					|| vecn=="unsigned int" || vecn=="unsigned short" || vecn=="unsigned long long" || vecn=="unsigned long"
					|| vecn=="char" || vecn=="unsigned char")
			{
				if(vecn=="long long")
				{
					vecn = "long";
				}
				else if(vecn=="unsigned long long")
				{
					vecn = "unsignedlong";
				}
				else if(vecn=="char")
				{
					vecn = "byte";
				}
				else if(vecn=="unsigned char")
				{
					vecn = "unsignedbyte";
				}
				else if(vecn=="unsigned short")
				{
					vecn = "unsignedshort";
				}
				else if(vecn=="unsigned int")
				{
					vecn = "unsignedint";
				}
				else if(vecn.find("unsigned")==0)
				{
					StringUtil::replaceAll(vecn, " ", "");
				}
				else if(vecn=="bool")
				{
					vecn = "boolean";
				}
				retType = "\n<xsd:element minOccurs=\"0\" maxOccurs=\"unbounded\" name=\""+in_out_info["RETURN"]+"\" type=\"xsd:"+vecn+"\"/>";
			}
			else
			{
				std::string trgnmspc;
				std::string fqcn = ref.getFullyQualifiedClassName(vecn, clstruct->getNamespaces());
				if(trgnms.find(fqcn)==trgnms.end())
				{
					xsdobjdef = ref.getXSDDefinitions(allclsmap, fqcn, ref, appname, trgnmspc, allnmspcs, gcntxt["WS_NMSPC"], resp);
					trgnms[fqcn] = trgnmspc;
				}
				else
					trgnmspc = trgnms[fqcn];
				logger << "in result " + vecn + " " + trgnmspc << std::endl;
				if(vecn.find("::")!=std::string::npos)
					vecn = vecn.substr(vecn.find_last_of("::")+1);
				logger << "in result after " + vecn + " " + trgnmspc << std::endl;
				retType = "\n<xsd:element minOccurs=\"0\" maxOccurs=\"unbounded\" name=\""+in_out_info["RETURN"]+"\" type=\""+trgnmspc+":"+vecn+"\"/>";
			}
		}
		else
		{
			if(results.at(0)!="void")
			{
				if(results.at(0)=="int" || results.at(0)=="string" || results.at(0)=="short" || results.at(0)=="float" || results.at(0)=="double"
						|| results.at(0)=="bool" || results.at(0)=="long long" || results.at(0)=="long"
						|| results.at(0)=="unsigned int" || results.at(0)=="unsigned short" || results.at(0)=="unsigned long long"
						|| results.at(0)=="unsigned long" || results.at(0)=="char" || results.at(0)=="unsigned char")
				{
					if(results.at(0)=="long long")
					{
						results.at(0) = "long";
					}
					else if(results.at(0)=="unsigned long long")
					{
						results.at(0) = "unsignedlong";
					}
					else if(results.at(0)=="char")
					{
						results.at(0) = "byte";
					}
					else if(results.at(0)=="unsigned char")
					{
						results.at(0) = "unsignedbyte";
					}
					else if(results.at(0)=="unsigned short")
					{
						results.at(0) = "unsignedshort";
					}
					else if(results.at(0)=="unsigned int")
					{
						results.at(0) = "unsignedint";
					}
					else if(results.at(0).find("unsigned")==0)
					{
						StringUtil::replaceAll(results.at(0), " ", "");
					}
					else if(results.at(0)=="bool")
					{
						results.at(0) = "boolean";
					}
					retType = "\n<xsd:element name=\""+in_out_info["RETURN"]+"\" type=\"xsd:"+results.at(0)+"\"/>";
				}
				else
				{
					std::string type = results.at(0);
					std::string trgnmspc;
					std::string fqcn = ref.getFullyQualifiedClassName(type, clstruct->getNamespaces());
					if(trgnms.find(fqcn)==trgnms.end())
					{
						xsdobjdef = ref.getXSDDefinitions(allclsmap, fqcn, ref, appname, trgnmspc, allnmspcs, gcntxt["WS_NMSPC"], resp);
						trgnms[fqcn] = trgnmspc;
					}
					else
						trgnmspc = trgnms[fqcn];
					logger << "in result " + type + " " + trgnmspc << std::endl;
					if(type.find("::")!=std::string::npos)
						type = type.substr(type.find_last_of("::")+1);
					logger << "in result after " + type + " " + trgnmspc << std::endl;
					retType = "\n<xsd:element name=\""+in_out_info["RETURN"]+"\" type=\""+trgnmspc+":"+type+"\"/>";
				}
			}
		}
		wsdl_obj_bind.append(xsdobjdef);

		for(unsigned int j=0;j<results1.size();j++)
		{
			std::string type;
			strVec results2;
			StringUtil::split(results2, results1.at(j), (" "));
			if(results2.size()<2)
			{
				results2.push_back("in"+CastUtil::lexical_cast<std::string>(j+1));
			}
			type = results2.at(0);
			int srn = j;
			std::stringstream ss;
			ss << srn;
			std::string te;
			ss >> te;
			if(type=="int" || type=="string" || type=="short" || type=="float" || type=="double" || type=="bool" || type=="long long" || type=="long"
					|| type=="unsigned int" || type=="unsigned short" || type=="unsigned long long" || type=="unsigned long"
					|| type=="char" || type=="unsigned char")
			{
				std::string ttyp = type;
				if(type=="long long")
				{
					type = "long";
				}
				else if(type=="unsigned long long")
				{
					type = "unsignedlong";
				}
				else if(type=="char")
				{
					type = "byte";
				}
				else if(type=="unsigned char")
				{
					type = "unsignedbyte";
				}
				else if(type=="unsigned short")
				{
					type = "unsignedshort";
				}
				else if(type=="unsigned int")
				{
					type = "unsignedint";
				}
				else if(type.find("unsigned")==0)
				{
					StringUtil::replaceAll(type, " ", "");
				}
				else if(type=="bool")
				{
					type = "boolean";
				}
				inp_params.append("\n<xsd:element name=\""+results2.at(1)+"\" type=\"xsd:"+type+"\"/>");
				in_out_info[results2.at(1)] = ttyp;
			}
			else if(type!="")
			{
				if(results2.size()>=2)
				{
					in_out_info[results2.at(1)] = type;
				}
				else
				{
					logger << ("Invalid thing happening " + results1.at(j)) << std::endl;
				}
				strMap allfs,tyfs;
				std::string xsdobjdef, xdstype;
				if(type.find("vector<")!=std::string::npos && results2.size()==2)
				{
					std::string vecn = type;
					StringUtil::replaceFirst(vecn,"vector<"," ");
					StringUtil::replaceFirst(vecn,"std::"," ");
					StringUtil::replaceFirst(vecn,">"," ");
					StringUtil::trim(vecn);
					if(vecn=="int" || vecn=="short" || vecn=="float" || vecn=="double" || vecn=="bool" || vecn=="long long" || vecn=="long"
							|| vecn=="string" || vecn=="unsigned int" || vecn=="unsigned short"
							|| vecn=="unsigned long long" || vecn=="unsigned long"
							|| vecn=="char" || vecn=="unsigned char")
					{
						if(vecn=="long long")
						{
							vecn = "long";
						}
						else if(vecn=="unsigned long long")
						{
							vecn = "unsignedlong";
						}
						else if(vecn=="char")
						{
							vecn = "byte";
						}
						else if(vecn=="unsigned char")
						{
							vecn = "unsignedbyte";
						}
						else if(vecn=="unsigned short")
						{
							vecn = "unsignedshort";
						}
						else if(vecn=="unsigned int")
						{
							vecn = "unsignedint";
						}
						else if(vecn.find("unsigned")==0)
						{
							StringUtil::replaceAll(vecn, " ", "");
						}
						else if(vecn=="bool")
						{
							vecn = "boolean";
						}
						type = "std::vector<" + vecn + ">";
						inp_params.append("\n<xsd:element minOccurs=\"0\" maxOccurs=\"unbounded\" name=\""+results2.at(1)+"\" type=\"xsd:"+vecn+"\"/>");
					}
					else
					{
						std::string trgnmspc;
						std::string fqcn = ref.getFullyQualifiedClassName(vecn, clstruct->getNamespaces());
						type = "std::vector<" + fqcn + ">";
						if(trgnms.find(fqcn)==trgnms.end())
						{
							xsdobjdef = ref.getXSDDefinitions(allclsmap, fqcn, ref, appname, trgnmspc, allnmspcs, gcntxt["WS_NMSPC"], resp);
							trgnms[fqcn] = trgnmspc;
						}
						else
							trgnmspc = trgnms[fqcn];
						logger << "in result " + vecn + " " + trgnmspc << std::endl;
						if(vecn.find("::")!=std::string::npos)
							vecn = vecn.substr(vecn.find_last_of("::")+1);
						logger << "in result after " + vecn + " " + trgnmspc << std::endl;
						inp_params.append("\n<xsd:element minOccurs=\"0\" maxOccurs=\"unbounded\" name=\""+results2.at(1)+"\" type=\""+trgnmspc+":"+vecn+"\"/>");
					}
				}
				else if(results2.size()==2)
				{
					std::string trgnmspc;
					std::string fqcn = ref.getFullyQualifiedClassName(type, clstruct->getNamespaces());
					if(trgnms.find(fqcn)==trgnms.end())
					{
						xsdobjdef = ref.getXSDDefinitions(allclsmap, fqcn, ref, appname, trgnmspc, allnmspcs, gcntxt["WS_NMSPC"], resp);
						trgnms[fqcn] = trgnmspc;
					}
					else
						trgnmspc = trgnms[fqcn];
					logger << "in result " + type + " " + trgnmspc << std::endl;
					if(type.find("::")!=std::string::npos)
						type = type.substr(type.find_last_of("::")+1);
					logger << "in result after " + type + " " + trgnmspc << std::endl;
					inp_params.append("\n<xsd:element minOccurs=\"0\" name=\""+results2.at(1)+"\" type=\""+trgnmspc+":"+type+"\"/>");
				}

				wsdl_obj_bind.append(xsdobjdef);
			}
		}
		meth_info[methName] = in_out_info;
		StringContext cntxt;
		cntxt["METH_NAME"] = methName;
		cntxt["RET_TYPE"] = retType;
		cntxt["INP_PARAMS"] = inp_params;
		reqr_res_bind.append("\n"+TemplateEngine::evaluate(resp+"templateReqRes.wsdl",cntxt));
		wsdl_msgs.append("\n"+TemplateEngine::evaluate(resp+"templateWsdlMsg.wsdl",cntxt));
		wsdl_ops.append("\n"+TemplateEngine::evaluate(resp+"templateWsdlOpe.wsdl",cntxt));
		wsdl_bind.append("\n"+TemplateEngine::evaluate(resp+"templateWsdlBind.wsdl",cntxt));
	}
	ws_info[ows_name] = meth_info;
	gcntxt["REQ_RES_BINDING"] = reqr_res_bind;
	gcntxt["WSDL_MESSAGES"] = wsdl_msgs;
	gcntxt["WSDL_OPERATIONS"] = wsdl_ops;
	gcntxt["WSDL_BINDING"] = wsdl_bind;
	gcntxt["OBJ_BINDING"] = wsdl_obj_bind;
	std::string extranmspcs;
	if(allnmspcs.size()>0)
	{
		std::set<std::string>::iterator it;
		for(it=allnmspcs.begin();it!=allnmspcs.end();++it) {
			std::string nmspcid = *it;
			if(nmspcid!="tns")
			{
				std::string nmspcidval = Reflection::getNameSpaceIdValue(nmspcid);
				if(nmspcidval!="") {
					extranmspcs.append(" xmlns:" + nmspcid + "=\""+nmspcidval+"\"");
				}
			}
		}
	}
	gcntxt["EXTR_NMSPCS"] = extranmspcs;
	gcntxt["URL"] = "http://" + ConfigurationData::getInstance()->coreServerProperties.ip_address + "/" + appname;
	std::string wspath = gcntxt["URL"] + "/" + gcntxt["WS_PATH"];
	ConfigurationData::getInstance()->wsdlMap[appname][wspath] = ws_name;
	wsdl = TemplateEngine::evaluate(resp+"template.wsdl",gcntxt);
	AfcUtil::writeTofile(resp+"../web/"+appname+"/public/"+ws_name+".wsdl",wsdl,true);
	//ws_funcs.append(obj_mapng);
	//ws_funcs.append(retObj_xml);
	//ws_funcs.append("extern \"C\"\n{\n");
	ws_inp_out_Map::iterator iter;
	for(iter=ws_info.begin();iter!=ws_info.end();iter++)
	{
		std::string ws_n = iter->first;
		meth_Map meth = iter->second;
		meth_Map::iterator iter1;
		for(iter1=meth.begin();iter1!=meth.end();iter1++)
		{
			std::string me_n = iter1->first;
			strMap pars = iter1->second;
			std::string ws_nn = ws_n;
			StringUtil::replaceAll(ws_nn, "::", "_");
			ws_funcs.append("std::string "+appname+me_n+ws_nn+"(Element* _req)\n{\nElement* ele = NULL;\n");
			ws_funcs.append("std::string _retStr;\n");
			ws_funcs.append("try{\n/*_req->validateNs();*/\n");
			strMap::iterator iter2;
			std::string args;
			unsigned int ter = 1;
			//logger << me_n << ws_n << pars.size() << std::endl;
			for(iter2=pars.begin();iter2!=pars.end();iter2++)
			{
				if(iter2->first!="RETURN" && iter2->first!="RETURNTYP")
				{
					std::string argname =  iter2->first;
					if(iter2->second=="int" || iter2->second=="short" || iter2->second=="double" || iter2->second=="float" || iter2->second=="string"
							|| iter2->second=="bool" || iter2->second=="long" || iter2->second=="long long" || iter2->second=="unsigned int"
								|| iter2->second=="unsigned short" || iter2->second=="unsigned long" || iter2->second=="unsigned long long"
										|| iter2->second=="std::string" || iter2->second=="char" || iter2->second=="unsigned char")
					{
						ws_funcs.append("ele = _req->getElementByNameIgnoreCase(\""+argname+"\");\n");
						ws_funcs.append(iter2->second+" "+argname+";\n");
						ws_funcs.append("if(ele!=NULL)");
						ws_funcs.append(argname+" = CastUtil::lexical_cast<"+iter2->second+">(ele->getText());\n");
					}
					else if(iter2->second.find("vector<")!=std::string::npos)
					{
						std::string vecn = iter2->second;
						StringUtil::replaceFirst(vecn,"std::"," ");
						StringUtil::replaceFirst(vecn,"vector<"," ");
						StringUtil::replaceFirst(vecn,">"," ");
						StringUtil::trim(vecn);
						if(vecn=="int" || vecn=="short" || vecn=="float" || vecn=="double" || vecn=="bool" || vecn=="long long" || vecn=="long"
								|| vecn=="string" || vecn=="unsigned int" || vecn=="unsigned short"
								|| vecn=="unsigned long long" || vecn=="unsigned long"
								|| vecn=="char" || vecn=="unsigned char")
						{
							ws_funcs.append("std::vector<"+vecn+" > "+argname+";\n");
							ws_funcs.append("ElementList list = _req->getElementsByName(\""+argname+"\");\n");
							ws_funcs.append("for(int i=0;i<list.size();i++)");
							ws_funcs.append(argname+".push_back(CastUtil::lexical_cast<"+vecn+">(list.at(i)->getText()));\n");
						}
						else
						{
							ws_funcs.append("std::vector<"+vecn+" > "+argname+";\n");
							ws_funcs.append("ElementList list = _req->getElementsByName(\""+argname+"\");\n");
							ws_funcs.append("for(int i=0;i<list.size();i++)\n{\n");
							ws_funcs.append(argname+".push_back(XMLSerialize::unserialize<std::vector<"+vecn+" > >(list.at(i), \"vector<"+vecn+" >\", \""+appname+"\"));\n}\n");
						}
					}
					else
					{
						ws_funcs.append("ele = _req->getElementByNameIgnoreCase(\""+argname+"\");\n");
						ws_funcs.append(iter2->second+" "+argname+";\n");
						ws_funcs.append("if(ele!=NULL)");
						ws_funcs.append("{\nele->setTagName(\""+iter2->second+"\");");
						//ref.getTreatedFullyQualifiedClassName(argname, )
						ws_funcs.append("\n"+argname+" = XMLSerialize::unserialize<"+iter2->second+" >(ele, \""+iter2->second+"\", \""+appname+"\");\n");
						//ws_funcs.append("\ncout << ele->renderSerialization() << std::endl;");
						ws_funcs.append("}\n");
					}
					args.append(argname);
					if(ter++<pars.size()-2)
						args.append(",");
				}
			}
			ws_funcs.append(ws_n+" _obj;\n");

			//ws_funcs.append("AttributeList attl = _req->getAttributes();\n");
			//ws_funcs.append("AttributeList::iterator it;\n");
			ws_funcs.append("_retStr = \"<tns:\" + _req->getTagName() + \"Response\";\n");
			//ws_funcs.append("for(it=attl.begin();it!=attl.end();it++)\n");
			//ws_funcs.append("_retStr.append(\" \" + it->first + \"=\\\"\" + it->second + \"\\\" \");\n");
			ws_funcs.append("_retStr.append(\">\");\n");
			if(pars["RETURNTYP"]=="void")
			{
				ws_funcs.append("_obj."+me_n+"("+args+");\n");
			}
			else if(pars["RETURNTYP"]=="int" || pars["RETURNTYP"]=="short" || pars["RETURNTYP"]=="float" || pars["RETURNTYP"]=="double"
					|| pars["RETURNTYP"]=="bool" || pars["RETURNTYP"]=="long long" || pars["RETURNTYP"]=="long"
					|| pars["RETURNTYP"]=="unsigned int" || pars["RETURNTYP"]=="unsigned short" || pars["RETURNTYP"]=="string"
					|| pars["RETURNTYP"]=="unsigned long long" || pars["RETURNTYP"]=="unsigned long"
					|| pars["RETURNTYP"]=="char" || pars["RETURNTYP"]=="unsigned char")
			{
				ws_funcs.append(pars["RETURNTYP"]+" _retval;\n");
				ws_funcs.append("_retval = _obj."+me_n+"("+args+");\n");
				ws_funcs.append("_retStr += \"<tns:"+pars["RETURN"]+">\"+CastUtil::lexical_cast<std::string>(_retval)+\"</tns:"+pars["RETURN"]+">\";\n");
			}
			else if(pars["RETURNTYP"]!="")
			{
				ws_funcs.append(pars["RETURNTYP"]+" _retval;\n");
				ws_funcs.append("_retval = _obj."+me_n+"("+args+");\n");
				if(pars["RETURNTYP"].find("vector<")!=std::string::npos)
				{
					std::string vecn = pars["RETURNTYP"];
					StringUtil::replaceFirst(vecn,"std::"," ");
					StringUtil::replaceFirst(vecn,"vector<"," ");
					StringUtil::replaceFirst(vecn,">"," ");
					StringUtil::trim(vecn);
					if(vecn=="int" || vecn=="short" || vecn=="float" || vecn=="double" || vecn=="bool" || vecn=="long long" || vecn=="long"
							|| vecn=="string" || vecn=="unsigned int" || vecn=="unsigned short"
							|| vecn=="unsigned long long" || vecn=="unsigned long"
							|| vecn=="char" || vecn=="unsigned char")
					{
						ws_funcs.append("for(int i=0;i<_retval.size();i++)");
						ws_funcs.append("_retStr += \"<tns:"+pars["RETURN"]+">\"+CastUtil::lexical_cast<std::string>(_retval.at(i))+\"</tns:"+pars["RETURN"]+">\";\n");
					}
					else
					{
						ws_funcs.append("std::string allnmspcs;\n");
						ws_funcs.append("\nXmlParser parser(\"Parser\");\n");
						ws_funcs.append("for(int i=0;i<_retval.size();i++)\n{\n");
						ws_funcs.append("allnmspcs = \"\";\n");
						ws_funcs.append("Document doc;\nparser.parse(XMLSerialize::serialize<"+pars["RETURNTYP"]+" >(&(_retval.at(i)),\""+vecn+"\",\""+appname+"\"), doc);\n");
						//ws_funcs.append("\ncout << XMLSerialize::serialize<"+pars["RETURNTYP"]+" >(&(_retval.at(i)),\""+vecn+"\",\""+appname+"\") << std::endl;");
						ws_funcs.append("normalizeNamespaces(&(doc.getRootElement()),\""+appname+"\",allnmspcs);\n");
						ws_funcs.append("_retStr += \"<tns:"+pars["RETURN"]+">\"+doc.getRootElement().renderChildren()+\"</tns:"+pars["RETURN"]+">\";\n}\n");
					}
				}
				else
				{
					ws_funcs.append("std::string allnmspcs;\n");
					ws_funcs.append("XmlParser parser(\"Parser\");\n");
					ws_funcs.append("Document doc;\nparser.parse(XMLSerialize::serialize<"+pars["RETURNTYP"]+" >(&_retval,\""+pars["RETURNTYP"]+"\",\""+appname+"\"), doc);\n");
					ws_funcs.append("normalizeNamespaces(&(doc.getRootElement()),\""+appname+"\",allnmspcs);\n");
					ws_funcs.append("_retStr += \"<tns:"+pars["RETURN"]+">\"+doc.getRootElement().renderChildren()+\"</tns:"+pars["RETURN"]+">\";\n");
				}
			}
			ws_funcs.append("_retStr += \"</tns:\" + _req->getTagName() + \"Response>\";\n");
			ws_funcs.append("}catch(const Exception& e){\n");
			ws_funcs.append("return e.getMessage();\n} ");
			ws_funcs.append("catch(const char* e){\n");
			ws_funcs.append("return std::string(e);\n} ");
			ws_funcs.append("catch(...){\n");
			ws_funcs.append("return \"<soap:Fault><faultcode>soap:Server</faultcode><faultstring>Exception occurred</faultstring></soap:Fault>\";\n}\n");
			ws_funcs.append("return _retStr;\n}\n");
		}
		//ws_funcs.append("}\n");
	}
	//AfcUtil::writeTofile(rtdcfp+"WsInterface.cpp",ws_funcs,true);
}
